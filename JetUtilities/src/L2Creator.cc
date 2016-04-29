// This class libraries
#include "JetMETAnalysis/JetUtilities/interface/L2Creator.hh"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// implement L2Creator class
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
L2Creator::L2Creator() {
  cout << "WARNING::L2Creator() Default constructor not recommended." << endl;
  input = "None";
  era = "None";
  l3input = "l3.root";
  output = "l2.root";
  outputDir = "./";
  formats = {".png",".eps"};
  algs = {"ak4pf"};
  l2l3 = true;
  l2calofit = "standard";
  l2pffit = "standard";
  mpv = true;
  delphes = false;
  maxFitIter = 30;
}

//______________________________________________________________________________
L2Creator::L2Creator(CommandLine& cl) {
  //
  // evaluate command-line / configuration file options
  //
  input      = cl.getValue<string>  ("input");
  era        = cl.getValue<string>  ("era");
  l3input    = cl.getValue<string>  ("l3input",    "l3.root");
  output     = cl.getValue<TString> ("output",     "l2.root");
  outputDir  = cl.getValue<TString> ("outputDir",       "./");
  formats    = cl.getVector<string> ("formats",           "");
  algs       = cl.getVector<string> ("algs",              "");
  l2l3       = cl.getValue<bool>    ("l2l3",            true);
  l2calofit  = cl.getValue<TString> ("l2calofit", "standard");
  l2pffit    = cl.getValue<TString> ("l2pffit",   "standard");
  mpv        = cl.getValue<bool>    ("mpv",             true);
  delphes    = cl.getValue<bool>    ("delphes",        false);
  maxFitIter = cl.getValue<int>     ("maxFitIter",        30);

  if (!cl.partialCheck()) return;
  cl.print();
}

void L2Creator::resetForNextAlgorithm() {
  if(ji!=0)
  	delete ji;
  if(odir!=0)
  	delete odir;
  if(l3dir!=0)
  	delete l3dir;
	if(gl3rsp!=0)
    delete gl3rsp;
	if(fl3rsp!=0)
    delete fl3rsp;

  hl_rsp.reset();
  hl_refpt.reset();
	hl_jetpt.reset();

	vabsrsp_eta.erase(vabsrsp_eta.begin(),vabsrsp_eta.end());
  vabscor_eta.erase(vabscor_eta.begin(),vabscor_eta.end());
  vrelcor_eta.erase(vrelcor_eta.begin(),vrelcor_eta.end());
}

//______________________________________________________________________________
void L2Creator::openOutputFile() {
  if(!outputDir.EndsWith("/")) outputDir+="/";
 	ofile = TFile::Open(outputDir+output,"RECREATE");
  if (!ofile->IsOpen()) {
  	cout<<"Can't create "<<output<<endl;
  	assert(ofile->IsOpen());
  }
}

//______________________________________________________________________________
void L2Creator::openInputFile() {
  ifile = TFile::Open(input.c_str(),"READ");
  if (!ifile->IsOpen()) {
  	cout<<"Can't open "<<input<<endl;
  	assert(ifile->IsOpen());
  }
}

//______________________________________________________________________________
void L2Creator::openL3File() {
  l3file = 0;
  if (!l2l3) {
    l3file = new TFile(l3input.c_str(),"READ");
    if (!l3file->IsOpen()) {
      cout<<"Can't open "<<l3input<<endl;
      assert(l3file->IsOpen());
    }
  }
}

//______________________________________________________________________________
void L2Creator::loopOverDirectories() {
  if (algs.size()==0) {
    TIter nextDir(ifile->GetListOfKeys());
    TKey* dirKey(0);
    while ((dirKey=(TKey*)nextDir())) {
      if (strcmp(dirKey->GetClassName(),"TDirectoryFile")!=0) continue;
      algs.push_back(dirKey->GetName());
    }
  }
}

//______________________________________________________________________________
void L2Creator::loopOverAlgorithms() {
  TIter nextDir(ifile->GetListOfKeys());
  TKey* dirKey(0);
  int algIndex(-1);
  while ((dirKey=(TKey*)nextDir())) {
    if (strcmp(dirKey->GetClassName(),"TDirectoryFile")!=0) continue;
    TDirectoryFile* idir = (TDirectoryFile*)dirKey->ReadObj();
    string alg(idir->GetName()); if (!contains(algs,alg)) continue;
    
    algIndex++;
    cout << alg << " ... " << endl;

    //
    // Reset for the next algorithm
    //
    if(algIndex>0)
      resetForNextAlgorithm();

    //
    // Make the JetInfo object
    //
    ji = new JetInfo(alg);

    //
    // Get the response from the l3 file only if l2l3 is set to false;
    //
    gl3rsp = 0;
    fl3rsp = 0;
    l3dir = 0;
    if (!l2l3) {
      bool gotL3Rsp = getL3Rsp();
      if(!gotL3Rsp) continue;
    }//if (!l2l3)

    //
    // Make output directory in the file l2.root (or other name if reset by user)
    //
    odir = (TDirectoryFile*)ofile->mkdir(alg.c_str());
    odir->cd();
     
    //
    // Load the input histograms from jra.root or jra_f.root (or other name if reset by user)
    //
    hl_rsp.load_objects(idir,"RelRsp:JetEta:RefPt");     
    hl_refpt.load_objects(idir,"RefPt:JetEta:RefPt");
    hl_jetpt.load_objects(idir,"JetPt:JetEta:RefPt");
     
     
    //
    // Absolute response/correction as a function of pT for each eta bin
    // Needed for both L2 only corrections and L2L3 corrections
    //
    loopOverEtaBins();

    if(!l2l3) {
	    //
    	// Relative (L2) response/correction as a function of pT for each eta bin
     	// Needed for L2 only corrections, but not the L2L3 corrections
     	//
     	doRelCorFits();
 	  }

    //
    // write the L2 correction text file for the current algorithm
    //
    writeTextFileForCurrentAlgorithm();
     
    cout<<alg<<" is DONE."<<endl;
  }
}

//______________________________________________________________________________
void L2Creator::loopOverEtaBins() {
	string alg = ji->getAbbreviation();
	vector<unsigned int> indices;
	TH1F* hrsp(0);
	hl_rsp.begin_loop();

	while ((hrsp=hl_rsp.next_object(indices))) {

		unsigned int ieta=indices[0];
		unsigned int ipt =indices[1];

    //
    // create new graphs if a new eta bin comes around
    //
		if (ipt==0) {
			vabsrsp_eta.push_back(new TGraphErrors());
			vabscor_eta.push_back(new TGraphErrors());
			stringstream ss;
			ss<<hl_rsp.minimum(0,ieta)<<"to"<<hl_rsp.maximum(0,ieta);
			vabsrsp_eta.back()->SetName(("AbsRspVsRefPt_JetEta"+ss.str()).c_str());
			vabscor_eta.back()->SetName(("AbsCorVsJetPt_JetEta"+ss.str()).c_str());
		}

    //
    // only add points to the graphs if the current histo is not empty
    // the current setting might be a little high
    //
    if (hrsp->GetEntries() > 10) {//hrsp->Integral()!=0) { 

			TF1*  frsp    = (TF1*)hrsp->GetListOfFunctions()->Last();
      //std::cout << "hrspName = " << hrsp->GetName() << ": frsp = " << frsp << std::endl;
			TH1F* hrefpt  = hl_refpt.object(indices);
			TH1F* hjetpt  = hl_jetpt.object(indices);

			assert(hrefpt->GetEntries()>0&&hjetpt->GetEntries()>0);

			double refpt  =hrefpt->GetMean();
			double erefpt =hrefpt->GetMeanError();
			double jetpt  =hjetpt->GetMean();
			double ejetpt =hjetpt->GetMeanError();

			double peak;
			double epeak;
			if(alg.find("calo")!=string::npos) {
				peak = (frsp==0 || !mpv)?hrsp->GetMean():frsp->GetParameter(1);
				epeak = (frsp==0 || !mpv)?hrsp->GetMeanError():frsp->GetParError(1);
			}
			else if(alg.find("pf")!=string::npos) {
				peak = (frsp==0 || !mpv)?hrsp->GetMean():frsp->GetParameter(1);
				epeak = (frsp==0 || !mpv)?hrsp->GetMeanError():frsp->GetParError(1);
			}
			else {
				peak = (frsp==0 || !mpv)?hrsp->GetMean():frsp->GetParameter(1);
				epeak = (frsp==0 || !mpv)?hrsp->GetMeanError():frsp->GetParError(1);
			}

			double absrsp = peak;
			double eabsrsp = epeak;
			double abscor = 0.0;
			double eabscor = 0.0;

			if (absrsp > 0) {  
				abscor  =1.0/absrsp;
				eabscor = abscor*abscor*epeak;
			} 
			if ((abscor>0) && (absrsp>0) && (eabscor>1e-5) && (eabscor/abscor<0.5) && (eabsrsp>1e-4) && (eabsrsp/absrsp<0.5)) { 
				int n = vabsrsp_eta.back()->GetN();
				vabsrsp_eta.back()->SetPoint     (n,refpt, absrsp);
				vabsrsp_eta.back()->SetPointError(n,erefpt,eabsrsp);
				vabscor_eta.back()->SetPoint     (n,jetpt, abscor);
				vabscor_eta.back()->SetPointError(n,ejetpt,eabscor);      
			}
			else cout << "absrsp " << absrsp << " and eabsrsp " << eabsrsp << " and abscor " << abscor << " and eabscor " << eabscor << endl;
		}

    //
    // fit graphs if last pt of the current eta bin comes around
    //
		if (ipt==hl_jetpt.nobjects(1)-1 && (vabsrsp_eta.back())->GetN()!=0 && (vabscor_eta.back())->GetN()!=0) {
			cout << "Doing fits for " << vabscor_eta.back()->GetName() << " ... " << endl;
			TGraphErrors* gabsrsp = vabsrsp_eta.back();
			TGraphErrors* gabscor = vabscor_eta.back();
			TF1*          fabscor(0);
			int npoints = gabscor->GetN(); 
			double xmin(1.0),xmax(100.0);
			if (npoints > 0) {
        //
        // we don't want to fit for pt less than 5 GeV as even a corrected calo jet of 10 
        // will be at least 5 Gev in raw energy.
        //
        //xmin = gabscor->GetX()[0];
        //xmin = max(gabscor->GetX()[0],3.0);
				xmin = max(gabscor->GetX()[0],6.0);
				xmax = gabscor->GetX()[gabscor->GetN()-1];
			}

			if (npoints<3 && !delphes) {
				gabscor->SetPoint     (0, 10.0,1.0);
				gabscor->SetPointError(0,  0.0,0.0);
				gabscor->SetPoint     (1,100.0,1.0);
				gabscor->SetPointError(1,  0.0,0.0);
				fabscor = new TF1("fit","[0]",10.0,100.0);
				fabscor->FixParameter(0,1.0);
			}
			else if (!l2l3 && npoints > 2 && gabscor->GetN()<10) {
				fabscor=new TF1("fit","[0]+[1]*log10(x)+[2]*pow(log10(x),2)",xmin,xmax);
				fabscor->SetParameter(0,1.0);
				fabscor->SetParameter(1,0.0);
				fabscor->SetParameter(2,0.0);
			}
			else {
				if (alg.find("pf")!=string::npos || alg.find("puppi")!=string::npos) {
					//
					// Delphes
					//
					if(delphes) {
                     	//fcn = "[0]+[1]*log10(x)+[2]*pow(log10(x),2)+[3]*pow(log10(x),3)+[4]*pow(log10(x),4)+[5]*pow(log10(x),5)+[6]*pow(log10(x),6)+[7]*pow(log10(x),7)+[8]*pow(log10(x),8)+[9]*pow(log10(x),9)";
						TString fcn = "[0]+[1]*log10(x)+[2]*pow(log10(x),2)+([3]/pow(log10(x),3))+([4]/pow(log10(x),4))+([5]/pow(log10(x),5))";
						fabscor=new TF1("fit",fcn.Data(),xmin,xmax);
					}

          //
          // online (HLT)
          //
					if(alg.find("HLT")!=string::npos){
						fabscor=new TF1("fit","(x>=[6])*([0]+[1]/(pow(log10(x),2)+[2])+[3]*exp(-[4]*(log10(x)-[5])*(log10(x)-[5])))+(x<[6])*[7]",xmin,xmax);
                        fabscor->SetParameter(0,0.5);
                        fabscor->SetParameter(1,9.0);
                        fabscor->SetParameter(2,8.0);
                        fabscor->SetParameter(3,-0.3);
                        fabscor->SetParameter(4,0.6);
                        fabscor->SetParameter(5,1.0);
						fabscor->FixParameter(6,xmin);
						fabscor->FixParameter(7,0.0);
					}
          //
          // offline
          //
					else {
						if (xmin<10) xmin=10;
						TString fcn = getOfflinePFFunction();
						fabscor=new TF1("fit",fcn.Data(),xmin,xmax);
						setOfflinePFParameters(gabscor, fabscor,xmin);
					}
				}
        else if (alg.find("trk")!=string::npos) {
        	fabscor=new TF1("fit","[0]+[1]*pow(x/500.0,[2])+[3]/log10(x)+[4]*log10(x)",xmin,xmax);
        	fabscor->SetParameter(0,1.7);
        	fabscor->SetParameter(1,0.7);
        	fabscor->SetParameter(2,3.0);
        	fabscor->SetParLimits(2,1,10);
        	fabscor->SetParameter(3,0.0);
        	fabscor->SetParameter(4,0.0);
        }
        else if (alg.find("jpt")!=string::npos || alg.find("tau")!=string::npos) {
        	fabscor=new TF1("fit","[0]+[1]/(pow(log10(x),2)+[2])+[3]*exp(-[4]*(log10(x)-[5])*(log10(x)-[5]))",xmin,xmax);
          //
          // INITIAL PARAMS: These are the fitted parameters that work for JetEta0.261to0.348
          //
        	fabscor->SetParameter(0,-8.9);
        	fabscor->SetParameter(1,530);
        	fabscor->SetParameter(2,16);
        	fabscor->SetParameter(3,-22);
        	fabscor->SetParameter(4,0.06);
        	fabscor->SetParameter(5,-0.28);
        }
        else if (alg.find("calo")!=string::npos) {
        	if(l2calofit.EqualTo("standard",TString::kIgnoreCase)) {
        		if (xmin<6) xmin=6;
         		fabscor=new TF1("fit","[0]+[1]/(pow(log10(x),[2])+[3])",xmin,xmax);
         		fabscor->SetParameter(0,1.0);
         		fabscor->SetParameter(1,5.0);
         		fabscor->SetParameter(2,3.0);
         		fabscor->SetParameter(3,3.0);

            fabscor->SetParLimits(3,0,100);
          }
          else if(l2calofit.EqualTo("DynamicMin",TString::kIgnoreCase)) {
      			xmin=findPeak(gabscor,0,3,3,false);
      			TString fcn = "[0] + ([1]/(pow(log10(x),2)+[2])) + ([3]*exp(-([4]*((log10(x)-[5])*(log10(x)-[5]))))) + ([6]*exp(-([7]*((log10(x)-[8])*(log10(x)-[8])))))";
      			fabscor=new TF1("fit",fcn.Data(),xmin,xmax);
      			fabscor->SetParameter(0,-0.0221278);
      			fabscor->SetParameter(1,119.265);
      			fabscor->SetParameter(2,100);
      			fabscor->SetParameter(3,-0.0679365);
      			fabscor->SetParameter(4,2.82597);
      			fabscor->SetParameter(5,1.8277);
      			fabscor->SetParameter(6,-0.0679365);
      			fabscor->SetParameter(7,3.82597);
      			fabscor->SetParameter(8,1.8277);
      			fabscor->SetParLimits(6,-20,10);
      			fabscor->SetParLimits(7,0,100);
      			fabscor->SetParLimits(3,-15,15);
      			fabscor->SetParLimits(4,0,500);
      			fabscor->SetParLimits(0,-2,25);
      			fabscor->SetParLimits(1,0,250);
      		}
      	}
        else {
        	cout << "WARNING::Cannot determine fit function for " << alg << "." << endl;
        }
      }

      //
      // obtain the best fit of the function fabscor to the histo gabscor
      //
      int origIgnoreLevel = gErrorIgnoreLevel;
      gErrorIgnoreLevel = kBreak;
      perform_smart_fit(gabscor,fabscor,maxFitIter);
      gErrorIgnoreLevel = origIgnoreLevel;
      if (alg.find("pf")!=string::npos) {
      	if (alg.find("HLT")!=string::npos) {
      		((TF1*)gabscor->GetListOfFunctions()->First())->FixParameter(7,fabscor->Eval(fabscor->GetParameter(6)));
      		fabscor->FixParameter(7,fabscor->Eval(fabscor->GetParameter(6)));
      	}
      }

      //
      // format the graphs
      //
     	gabscor->GetListOfFunctions()->First()->ResetBit(TF1::kNotDraw);
     	gabsrsp->SetMarkerStyle(20); 
     	gabscor->SetMarkerStyle(20);
      odir->cd();
     	gabsrsp->Write();
     	gabscor->Write();
      ofile->Write();
    }
  }
}

//______________________________________________________________________________
bool L2Creator::getL3Rsp() {
	string alg = ji->getAbbreviation();
  l3dir = (TDirectoryFile*)l3file->Get(alg.c_str());
  if (l3dir==0) {
    cout<<"Failed to rerieve L3 correction for "<<alg<<", skip"<<endl;
    return false;
  }
    
  gl3rsp = (TGraphErrors*)l3dir->Get("L3RspVsRefPt");
  fl3rsp = (TF1*)gl3rsp->GetListOfFunctions()->First();
  if (0==fl3rsp) {
    cout<<"Failed to retrieve L3 correction for "<<alg<<", skip"<<endl;
    return false;
  }

  return true;
}

//______________________________________________________________________________
void L2Creator::setAndFitFLogAndFGaus(TGraphErrors* gabscor, TF1* flog, TF1* fgaus, double xmin) {
	flog = new TF1("flog", "[3]+TMath::LogNormal(TMath::Log10(x), [0], [1], [2])", 3, 20);
	flog->SetLineColor(kBlue);
	flog->SetRange(xmin,20.0);
	flog->SetParameters(0.5, 0, 1);
	flog->SetParLimits(0,0.00000001,1000);
	flog->SetParLimits(1,0,1000);
	flog->SetParLimits(2,0.00000001,1000);
	flog->FixParameter(1,0.0);
  gabscor->Fit("flog","RBQS");

	fgaus = new TF1("fgaus","[0]+[1]/(pow(log10(x),2)+[2])+[3]*exp(-[4]*(log10(x)-[5])*(log10(x)-[5]))",20,4000);
	fgaus->SetLineColor(kOrange);
	fgaus->SetRange(20,4000);
	fgaus->SetParameter(0,0.5);
	fgaus->SetParameter(1,9.0);
	fgaus->SetParameter(2,8.0);
	fgaus->SetParameter(3,-0.3);
	fgaus->SetParameter(4,0.6);
	fgaus->SetParameter(5,1.0);
	fgaus->SetParLimits(2,0.1,100);
	fgaus->SetParLimits(3,-100,0);
	fgaus->SetParLimits(4,0,100);
  gabscor->Fit("fgaus","RQS");
}

//______________________________________________________________________________
TString L2Creator::getOfflinePFFunction() {
	if(l2pffit.EqualTo("standard",TString::kIgnoreCase)) {
		return "[0]+[1]/(pow(log10(x),2)+[2])+[3]*exp(-[4]*(log10(x)-[5])*(log10(x)-[5]))";
	}
	else if(l2pffit.EqualTo("standard+Gaussian",TString::kIgnoreCase)) {
		return "[0] + ([1]/(pow(log10(x),2)+[2])) + ([3]*exp(-([4]*((log10(x)-[5])*(log10(x)-[5]))))) + ([6]*exp(-([7]*((log10(x)-[8])*(log10(x)-[8])))))";
	}
	else if(l2pffit.EqualTo("LogNormal+Gaussian+fixed",TString::kIgnoreCase)) {
		return "([0]+TMath::LogNormal(TMath::Log10(x),[1],[2],[3]))+([4]+[5]/(pow(log10(x),2)+[6])+[7]*exp(-[8]*(log10(x)-[9])*(log10(x)-[9])))";
	}
	else if(l2pffit.EqualTo("LogNormal+Gaussian",TString::kIgnoreCase)) {
		return "(x<=[10])*([0]+TMath::LogNormal(TMath::Log10(x), [1], [2], [3]))+(x>[10])*([4]+[5]/(pow(log10(x),2)+[6])+[7]*exp(-[8]*(log10(x)-[9])*(log10(x)-[9])))";
		//return "(x<=[10])*([0]+((1.0/(TMath::Log10(x)*TMath::Sqrt(2.0*TMath::Pi()*TMath::Power([2],2))))*TMath::Exp(-TMath::Power(TMath::Log(TMath::Log10(x))-[1],2)/(2.0*TMath::Power([2],2)))))+(x>[10])*([4]+[5]/(pow(log10(x),2)+[6])+[7]*exp(-[8]*(log10(x)-[9])*(log10(x)-[9])))";
	}
	else {
		cout << "ERROR::getOfflinePFFunction::Unknown PF function choice." << endl;
		return "";		
	}
}

//______________________________________________________________________________
void L2Creator::setOfflinePFParameters(TGraphErrors* gabscor, TF1* fabscor, double xmin) {
	TF1* flog(0);
	TF1* fgaus(0);
	if(l2pffit.Contains("LogNormal+Gaussian",TString::kIgnoreCase)) {
		//int origIgnoreLevel = gErrorIgnoreLevel;
		//gErrorIgnoreLevel = kBreak;
		setAndFitFLogAndFGaus(gabscor, flog, fgaus, xmin);
		//gErrorIgnoreLevel = origIgnoreLevel;
		//Original function
	}

	if(l2pffit.EqualTo("standard",TString::kIgnoreCase)) {
		fabscor->SetParameter(0,0.5);
		fabscor->SetParameter(1,9.0);
		fabscor->SetParameter(2,8.0);
		fabscor->SetParameter(3,-0.3);
		fabscor->SetParameter(4,0.6);
		fabscor->SetParameter(5,1.0);
		fabscor->SetParLimits(2,0.1,100);
		fabscor->SetParLimits(3,-100,0);
		fabscor->SetParLimits(4,0,100);
	}
	else if(l2pffit.EqualTo("standard+Gaussian",TString::kIgnoreCase)) {
		fabscor->SetParameter(0,-0.0221278);
		fabscor->SetParameter(1,119.265);
		fabscor->SetParameter(2,100);
		fabscor->SetParameter(3,-0.0679365);
		fabscor->SetParameter(4,2.82597);
		fabscor->SetParameter(5,1.8277);
		fabscor->SetParameter(6,-0.0679365);
		fabscor->SetParameter(7,3.82597);
		fabscor->SetParameter(8,1.8277);
		fabscor->SetParLimits(6,-20,10);
		fabscor->SetParLimits(7,0,100);
		fabscor->SetParLimits(3,-15,15);
		fabscor->SetParLimits(4,0,500);
		fabscor->SetParLimits(0,-2,25);
		fabscor->SetParLimits(1,0,250);
	}
	else if(l2pffit.EqualTo("LogNormal+Gaussian+fixed",TString::kIgnoreCase)) {
		fabscor->SetRange(3,2000);
    	fabscor->SetParameter(0,flog->GetParameter(3));
    	fabscor->SetParameter(1,flog->GetParameter(0));
    	fabscor->SetParameter(2,flog->GetParameter(1));
    	fabscor->SetParameter(3,flog->GetParameter(2));
    	fabscor->SetParLimits(1,0,5);
    	fabscor->SetParLimits(2,-20,20);
    	fabscor->SetParLimits(3,0,5);
    	fabscor->SetParameter(4,fgaus->GetParameter(0));
    	fabscor->SetParameter(5,fgaus->GetParameter(1));
    	fabscor->SetParameter(6,fgaus->GetParameter(2));
    	fabscor->SetParameter(7,fgaus->GetParameter(3));
    	fabscor->SetParameter(8,fgaus->GetParameter(4));
    	fabscor->SetParameter(9,fgaus->GetParameter(5));
    	fabscor->SetParLimits(6,0.1,100);
    	fabscor->SetParLimits(7,-100,0);
    	fabscor->SetParLimits(8,0,100);
	}
	else if(l2pffit.EqualTo("LogNormal+Gaussian",TString::kIgnoreCase)) {
		fabscor->SetRange(xmin,4000);
    	fabscor->SetParameter(0,flog->GetParameter(3));
    	fabscor->SetParameter(1,flog->GetParameter(0));
    	fabscor->SetParameter(2,flog->GetParameter(1));
    	fabscor->SetParameter(3,flog->GetParameter(2));
    	fabscor->SetParLimits(1,0.00000001,1000);
    	fabscor->SetParLimits(2,0,1000);
    	fabscor->SetParLimits(3,0.00000001,1000);
    	fabscor->FixParameter(2,0.0);
    	fabscor->SetParameter(4,fgaus->GetParameter(0));
    	fabscor->SetParameter(5,fgaus->GetParameter(1));
    	fabscor->SetParameter(6,fgaus->GetParameter(2));
    	fabscor->SetParameter(7,fgaus->GetParameter(3));
    	fabscor->SetParameter(8,fgaus->GetParameter(4));
    	fabscor->SetParameter(9,fgaus->GetParameter(5));
    	fabscor->SetParLimits(6,0.1,100);
    	fabscor->SetParLimits(7,-100,0);
    	fabscor->SetParLimits(8,0,100);
    	fabscor->SetParameter(10,20);
	}
}

//______________________________________________________________________________
Double_t L2Creator::findPeak(TGraphErrors* gabscor, int ipeak, int npeaks, int res, bool verbose) {
	//Make a TH1F to find the peak
	int size = gabscor->GetN();
	map<Float_t,pair<Float_t,Float_t> > points;
	Float_t* xlow = new Float_t[size];
	Float_t* y = new Float_t[size];
	Float_t* ye = new Float_t[size];
	for(int ipoint=0; ipoint<size; ipoint++) {
		points[gabscor->GetX()[ipoint]] = make_pair(gabscor->GetY()[ipoint],gabscor->GetEY()[ipoint]);
	}
	int ipoint=0;
	for (std::map<Float_t,pair<Float_t,Float_t> >::iterator it=points.begin(); it!=points.end(); ++it) {
		xlow[ipoint] = it->first;
		y[ipoint] = it->second.first;
		ye[ipoint] = it->second.second;
		ipoint++;
	}
	TH1F* hpeak = new TH1F("hpeak","hpeak",gabscor->GetN()-1,xlow);
	for(int ibin=0; ibin<hpeak->GetNbinsX(); ibin++) {
		hpeak->SetBinContent(ibin,y[ibin]);
		hpeak->SetBinError(ibin,ye[ibin]);
	}
	TSpectrum *spec = new TSpectrum(npeaks);
	spec->Search(hpeak,res,"nobackground nodraw goff");
	Double_t* xpos = spec->GetPositionX();
	Double_t* ypos = spec->GetPositionY();
	if(verbose) {
		std::cout << "peak: " << xpos[ipeak] << std::endl;
		std::cout << "peak height: " << ypos[ipeak] << std::endl;
	}
	delete hpeak;
	return xpos[ipeak];
}

//______________________________________________________________________________
void L2Creator::doRelCorFits() {
	string fnc_as_str = (ji->getAbbreviation().find("trk")!=string::npos) ? 
       "[0]+[1]*log10(x)+[2]*pow(log10(x),2)+[3]*pow(log10(x),3)+[4]*pow(x/500.0,3)" :
       "[0]+[1]*log10(x)+[2]*pow(log10(x),2)+[3]*pow(log10(x),3)+[4]*pow(log10(x),4)";
     
    vector<unsigned int> indices;
    TH1F* hjetpt(0);
    hl_jetpt.begin_loop();
    while ((hjetpt=hl_jetpt.next_object(indices))) {

    	unsigned int ieta = indices[0];
    	unsigned int ipt  = indices[1];

        // create a new graph if a new eta bin comes around
    	if (ipt==0) {
    		vrelcor_eta.push_back(new TGraphErrors());
    		stringstream ss;
    		ss<<hl_jetpt.minimum(0,ieta)<<"to"<<hl_jetpt.maximum(0,ieta);
    		vrelcor_eta.back()->SetName(("RelCorVsJetPt_JetEta"+ss.str()).c_str());
    	}

        // only add a point to the graph if the current histo is not empty
    	if (hjetpt->Integral()!=0) {
    		TF1*   fabscor  =vabscor_eta[ieta]->GetFunction("fit");
    		double jetpt    =hjetpt->GetMean();
    		if(!fabscor) continue;
    		double refpt    =jetpt*fabscor->Eval(jetpt);
    		double l3cor    = 1;
    		if (!l2l3) l3cor = fl3rsp->Eval(refpt);
    		double controlpt=refpt*l3cor;
    		double relcor   =controlpt/jetpt;
    		if (relcor > 5)
    			cout<<"WARNING !!! suspicious point: "<<hjetpt->GetName()
    		<<", jet pt = "<<jetpt<<", ref pt = "<<refpt<<" "<<endl;
    		else { 
    			int n=vrelcor_eta.back()->GetN();
    			vrelcor_eta.back()->SetPoint(n,jetpt,relcor);
    		}
    	}

        // fit the graph if the last pt of the current eta bin comes around
    	if (ipt==hl_jetpt.nobjects(1)-1 && (vrelcor_eta.back())->GetN()!=0) {
    		TGraph* grelcor = vrelcor_eta.back();
    		double  xmin    = grelcor->GetX()[0];
    		double  xmax    = grelcor->GetX()[grelcor->GetN()-1];
    		TF1*    frelcor = new TF1("fit",fnc_as_str.c_str(),xmin,xmax);

    		frelcor->SetParameter(0,0.0);
    		frelcor->SetParameter(1,0.0);
    		frelcor->SetParameter(2,0.0);
    		frelcor->SetParameter(3,0.0);
    		frelcor->SetParameter(4,0.0);
    		frelcor->SetParameter(5,0.0);

    		if (grelcor->GetN()<2) {
    			grelcor->SetPoint(0,10,1.0);
    			grelcor->SetPoint(1,100,1.0);
    			frelcor->FixParameter(1,0.0);
    			frelcor->FixParameter(2,0.0);
    			frelcor->FixParameter(3,0.0);
    			frelcor->FixParameter(4,0.0);
    			frelcor->FixParameter(5,0.0);
    		}
    		else if (grelcor->GetN()==2) {
    			frelcor->FixParameter(2,0.0);
    			frelcor->FixParameter(3,0.0);
    			frelcor->FixParameter(4,0.0);
    			frelcor->FixParameter(5,0.0);
    		}

    		grelcor->Fit(frelcor,"QRB0");
    		grelcor->GetListOfFunctions()->First()->ResetBit(TF1::kNotDraw);
    		grelcor->SetMarkerStyle(20); 
    		grelcor->Write();
    	}
    }
}

//______________________________________________________________________________
bool L2Creator::contains(const vector<string>& collection,const string& element)
{
	vector<string>::const_iterator it;
  	for (it=collection.begin();it!=collection.end();++it)
    	if ((*it)==element) return true;
  	return false;
}

//______________________________________________________________________________
void L2Creator::perform_smart_fit(TGraphErrors * gabscor, TF1 * fabscor, int maxFitIter) {
	int fitIter = 0;
	vector<double> bestPars;
	double bestRChi2 = 0;
	do {         
	  	//
	  	// do the fit, get the results and the parameters of the fitted function
	  	//
	  	TFitResultPtr fitResPtr = gabscor->Fit(fabscor,"RQS+");
	  	vector<double> auxPars = fitResPtr.Get()->Parameters();
	
	    //
	    // compute the reduced chi2 of this fit and if it is the best fit so far
	    // then save the parameters
	    //
	    double rchi2 = fitResPtr.Get()->Chi2()/ fitResPtr.Get()->Ndf();
	    if (fitResPtr.Get()->Ndf() == 0) rchi2 = 0;
	    if (rchi2 > 0 && (rchi2<bestRChi2 || bestRChi2==0)){
	      bestRChi2 = rchi2;
	      bestPars  = auxPars;
	    } 
	
	    //
	    // increment the counter
	    //
	    fitIter++;
	}while(( bestRChi2 > 2 || bestRChi2 == 0 ) && fitIter < maxFitIter);
	 
	//
	// set the best parameters and chi2 to the fit function
	//
	TF1 * ffh = gabscor->GetFunction("fit");
	for (unsigned int np=0;np < bestPars.size() ; np++){
	  	ffh->SetParameter(np,bestPars[np]);
	  	fabscor->SetParameter(np,bestPars[np]);
	}
	fabscor->SetChisquare(bestRChi2 * fabscor->GetNDF());
	ffh->SetChisquare(bestRChi2 * fabscor->GetNDF());
	    
	//
	// warn if the fit diverges at low pt
	//
	if (fabscor->Integral(0,10) > 60)
	   cout << "\t***ERROR***, fit for histo " << gabscor->GetName() << " diverges at low pt" << endl;
	
	//   
	// check for failed fits
	// a chi2 of zero is symptomatic of a failed fit.
	//
	if (bestRChi2 < 0.001) {
		cout<<"\t***ERROR***, FIT HAS FAILED for histo "<<gabscor->GetName()
			<<" which has a reduced chi2="<<bestRChi2
			<<" after "<<fitIter<<" iterations. "<<endl;
	}
	
	//
	// check for large reduced chi2's
	// above 10 is a plain error; between 5 and 10 is a warning
	//
	if (bestRChi2 > 5){
	  	if (bestRChi2 > 10)
	    	cout<<"\t***ERROR***,";
	  	else
	    	cout<<"\tWARNING,";
	
	    cout<<" fit for histo "<<gabscor->GetName()
	  		<<" has a reduced chi2="<<bestRChi2
	  		<<" after "<<fitIter<<" iterations"<<endl;
	}
}

//______________________________________________________________________________
void L2Creator::writeTextFileForCurrentAlgorithm() {
	TString txtfilename = outputDir + era + "_L2Relative_" + ji->getAlias() + ".txt";
	ofstream fout(txtfilename);
	fout.setf(ios::right); 

	unsigned int vector_size = 0;
    if(l2l3) vector_size = vabscor_eta.size(); //For L2L3 Corrections Together
    else vector_size = vrelcor_eta.size(); //For L2 & L3 Corrections Separate
    for (unsigned int ieta=0;ieta<vector_size;ieta++) { 
    	TGraph* grelcor;
      	if(l2l3) grelcor = vabscor_eta[ieta]; //For L2L3 Corrections Together
      	else grelcor = vrelcor_eta[ieta]; //For L2 & L3 Corrections Separate
      	TF1* frelcor = (TF1*)grelcor->GetListOfFunctions()->Last();
      	if(frelcor!=0) {
      		if(ieta==0 || (ieta==1 && delphes))
               fout<<"{1 JetEta 1 JetPt max(0.0001,"<<frelcor->GetTitle()<<") Correction L2Relative}"<<endl;
               //fout<<"{1 JetEta 1 JetPt max(0.0001,"<<frelcor->GetExpFormula()<<") Correction L2Relative}"<<endl;
      		double  etamin  = hl_jetpt.minimum(0,ieta);
      		double  etamax  = hl_jetpt.maximum(0,ieta);
      		double  ptmin = grelcor->GetX()[0];
      		double  ptmax = grelcor->GetX()[grelcor->GetN()-1];
      		fout<<setw(11)<<etamin
      		<<setw(11)<<etamax
        	    <<setw(11)<<(int)(frelcor->GetNpar()+2) //Number of parameters + 2 
        	    <<setw(12)<<ptmin
        	    <<setw(12)<<ptmax;
        	    for(int p=0; p<frelcor->GetNpar(); p++) {
        	    	fout<<setw(13)<<frelcor->GetParameter(p);
        	    }
        	    fout<<endl;
        }
    }
    fout.close();
}

//______________________________________________________________________________
void L2Creator::closeFiles() {
  	cout<<"Write "<<output<<" ... "<<flush;
    ofile->Write();
  	ofile->Close();
  	delete ofile;
  	ifile->Close();
  	delete ifile;
  	cout<<"DONE"<<endl;
}

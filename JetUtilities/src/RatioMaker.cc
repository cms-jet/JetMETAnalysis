// This class libraries
#include "JetMETAnalysis/JetUtilities/interface/RatioMaker.hh"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// implement RatioMaker class
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
RatioMaker::RatioMaker() {
  	cout << "WARNING::RatioMaker() Default constructor not recommended." << endl;
  	filepath1    	= "";
	filepath2    	= "";
	algo         	= "";
	numerator    	= "";
	denominator  	= "";
	CMEnergy	 	= 13000;
    eta_max         = 4.7;
	doflavor     	= false;
	draw_guidelines = true;
	draw_residual	= true;
	outputDir    	= "./";
	outputFormat 	= {".png",".eps",".pdf"};

   	ji = new JetInfo(algo);

  	if(JetInfo::checkAlgorithm(TString(numerator)).CompareTo("unknown",TString::kIgnoreCase) &&
  	   JetInfo::checkAlgorithm(TString(denominator)).CompareTo("unknown",TString::kIgnoreCase)) {
		algo = numerator+"Over"+denominator;
		jiNumerator = new JetInfo(numerator);
		jiDenominator = new JetInfo(denominator);
      	algDiv = true;
   	}
   	else {
   		algDiv = false;
   	}

   	objects_loaded = false;
}

//______________________________________________________________________________
RatioMaker::RatioMaker(CommandLine& cl) {
  	//
  	// evaluate command-line / configuration file options
  	//
  	filepath1    	= cl.getValue<string>   ("filepath1");
  	filepath2    	= cl.getValue<string>   ("filepath2");
  	algo         	= cl.getValue<string>   ("algo");
  	numerator    	= cl.getValue<string>   ("numerator",            "53X");
  	denominator  	= cl.getValue<string>   ("denominator",          "52X");
  	CMEnergy     	= cl.getValue<double>   ("CMEnergy",  			 13000);
    eta_max         = cl.getValue<double>   ("eta_max",                4.7);
  	doflavor     	= cl.getValue<bool>     ("doflavor",             false);
  	draw_guidelines = cl.getValue<bool>		("draw_guidelines",       true);
  	draw_residual	= cl.getValue<bool>		("draw_residual",		  true);
  	outputDir    	= cl.getValue<TString>  ("outputDir",         	  "./");
  	outputFormat 	= cl.getVector<TString> ("outputFormat", ".png:::.eps");
	bool help    	= cl.getValue<bool>     ("help",                 false);

    if (help) {cl.print(); return;}
	if (!cl.partialCheck()) return;
  	cl.print();

   	ji = new JetInfo(algo);

  	if(JetInfo::checkAlgorithm(TString(numerator)).CompareTo("unknown",TString::kIgnoreCase) &&
  	   JetInfo::checkAlgorithm(TString(denominator)).CompareTo("unknown",TString::kIgnoreCase)) {
		algo = numerator+"Over"+denominator;
		jiNumerator = new JetInfo(numerator);
		jiDenominator = new JetInfo(denominator);
      	algDiv = true;
   	}
   	else {
   		algDiv = false;
   	}

   	objects_loaded = false;
}

//______________________________________________________________________________
void RatioMaker::openInputFiles() {
	ifile1 = TFile::Open(filepath1.c_str(),"READ");
	if(ifile1 == 0) {
		cout << "ERROR::RatioMaker::openInputFiles() Could not open the file " << filepath1 << endl;
		assert(ifile1!=0);
	}

	ifile2 = TFile::Open(filepath2.c_str(),"READ");
	if(ifile2 == 0) {
		cout << "ERROR::RatioMaker::openInputFiles() Could not open the file " << filepath2 << endl;
		assert(ifile2!=0);
	}
}

//______________________________________________________________________________
void RatioMaker::getHistograms(bool multiBin) {
	if(objects_loaded) {
		hl_1.reset();
		hl_2.reset();
	}

	TDirectoryFile* idir1;
	TDirectoryFile* idir2;
	if(algDiv) {
		idir1 = (TDirectoryFile*)ifile1->GetDirectory(numerator.c_str());
		idir2 = (TDirectoryFile*)ifile2->GetDirectory(denominator.c_str());
	}
	else{
		idir1 = (TDirectoryFile*)ifile1->GetDirectory(algo.c_str());
		idir2 = (TDirectoryFile*)ifile2->GetDirectory(algo.c_str());
	}

	if(var == VARIABLES::refpt) {
		hl_1.load_objects(idir1,"ClosureVsRefPt:JetEta"); 
		hl_2.load_objects(idir2,"ClosureVsRefPt:JetEta");
		objects_loaded = true;
		if(hl_1.nobjects()!=NDetectorNames) {
      		cout << "One or more of the histogram pointers from file " << filepath1 << " is NULL." << endl
	      		 << "Histogram count is " << hl_1.nobjects() << ", but it should be " << NDetectorNames << endl;
   		}
   		if(hl_2.nobjects()!=NDetectorNames) {
      		cout << "One or more of the histogram pointers from file " << filepath2 << " is NULL." << endl
	      		 << "Histogram count is " << hl_2.nobjects() << ", but it should be " << NDetectorNames << endl;
   		}
	}
	else if(var == VARIABLES::jteta) {
		if(!multiBin) {
			hl_1.load_objects(idir1,"ClosureVsJetEta:RefPt");
			hl_2.load_objects(idir2,"ClosureVsJetEta:RefPt");
			objects_loaded = true;
			if(hl_1.nobjects()!=NPtBins) {
				cout << "One or more of the histogram pointers from file " << filepath1 << " is NULL." << endl
					 << "Histogram count is " << hl_1.nobjects() << ", but it should be " << NPtBins << endl;
			}
			if(hl_2.nobjects()!=NPtBins) {
				cout << "One or more of the histogram pointers from file " << filepath2 << " is NULL." << endl
					 << "Histogram count is " << hl_2.nobjects() << ", but it should be " << NPtBins << endl;
			}
		}
		else {
			hl_1.load_objects(idir1,"ClosureVsJetEta_MultiBin:RefPt");
			hl_2.load_objects(idir2,"ClosureVsJetEta_MultiBin:RefPt");
			objects_loaded = true;
			if(hl_1.nobjects()!=1) {
				cout << "One or more of the histogram pointers from file " << filepath1 << " is NULL." << endl
					 << "Histogram count is " << hl_1.nobjects() << ", but it should be 1" << endl;
			}
			if(hl_2.nobjects()!=1) {
				cout << "One or more of the histogram pointers from file " << filepath2 << " is NULL." << endl
					 << "Histogram count is " << hl_2.nobjects() << ", but it should be 1" << endl;
			}
		}
	}
	else {
		cout << "ERROR RatioMaker::getHistograms Unknown variable." << endl;
		std::terminate();
	}

	//
	// Make sure you have an equal number of numerators and denominators
	//
	try {
		mismatch_exception::areEqual(hl_1.nobjects(),hl_2.nobjects());
  	}
  	catch (mismatch_exception& e) {
  		std::cerr << e.what(string("hl_1"),string("hl_2"));
    	std::terminate();
  	}
}

//______________________________________________________________________________
void RatioMaker::openOutputFile() {
   	if(doflavor) {
      	flavor1 = filepath1.substr(int(filepath1.rfind("_")+1),int(filepath1.rfind(".root")-filepath1.rfind("_")-1));
      	flavor2 = filepath2.substr(int(filepath2.rfind("_")+1),int(filepath2.rfind(".root")-filepath2.rfind("_")-1));
   	}

   	if(!gSystem->OpenDirectory(outputDir)) gSystem->mkdir(outputDir);
   	TString ofname = Form("%s/ClosureRatioVs%s_%s.root",outputDir.Data(),
   	                      getVariableTitleString(var).c_str(),algo.c_str());
   	if(doflavor) ofname = Form("%s/ClosureRatioVs%s_%sover%s_%s.root",outputDir.Data(),
   	                           getVariableTitleString(var).c_str(),flavor1.Data(),
   	                           flavor2.Data(),algo.c_str());
   	ofile = TFile::Open(ofname,"RECREATE");
}

//______________________________________________________________________________
void RatioMaker::closeFiles() {
	ofile->Close();
	ifile1->Close();
	ifile2->Close();
}

//______________________________________________________________________________
void RatioMaker::makeLines() {
	pair<int,int> min_max;
	//if(var.EqualTo("pt",TString::kIgnoreCase)) {
	if(var == VARIABLES::refpt || var == VARIABLES::jtpt) {
		min_max = make_pair(0,5000);
	}
	//else if(var.EqualTo("eta",TString::kIgnoreCase)) {
	else if(var == VARIABLES::refeta || var == VARIABLES::jteta) {
		min_max = make_pair(-6,6);
	}
	else {
		min_max = make_pair(-10000,10000);
	}
	line = new TF1("line","0*x+1",min_max.first,min_max.second);
   	line->SetLineColor(1);
   	line->SetLineWidth(1);
   	line->SetLineStyle(2);
    linePlus = new TF1("linePlus","0*x+1.01",min_max.first,min_max.second);
   	linePlus->SetLineColor(1);
   	linePlus->SetLineWidth(1);
   	linePlus->SetLineStyle(2);
    lineMinus = new TF1("lineMinus","0*x+0.99",min_max.first,min_max.second);
   	lineMinus->SetLineColor(1);
   	lineMinus->SetLineWidth(1);
   	lineMinus->SetLineStyle(2);
}

//______________________________________________________________________________
void RatioMaker::makeRatio(const VARIABLES::Variable ivar, bool multiBin) {
	//
	// Store the variable choice for the other functions to use
	// This is simply so that we don't have to pass around this variable to every function
	//
	var = ivar;

	//
	// Open the files containing the original closure plots and retrieve the histograms
	//
	openInputFiles();
	getHistograms(multiBin);

   	//
   	// Open/create the output directory and file
   	//
   	openOutputFile();

	//
	// Create guides (lines) for the output histograms
	//
	makeLines();

   	//
   	// Divide the input histograms and clone into the ratio histogram container
   	//
   	TH1F *hvar1(0), *hvar2(0);
   	vector<unsigned int> indices;
   	hl_1.begin_loop(); hl_2.begin_loop();
   	while ((hvar1=hl_1.next_object(indices))) {

		unsigned int ibin=indices[0];
		hvar2 = hl_2.object(indices);

		//
		// Clone the numerator histogram so that you can keep a copy of the numerator, denominator, and ratio
		// Reset the ratio histogram's name
		//
		ratioHist.push_back((TH1F*)hvar1->Clone(Form("%s_clone",hvar1->GetName())));
		TString hname;
		if(var == VARIABLES::refpt) {
			hname = Form("ClosureRatioVsRefPt_JetEta%gto%g",hl_1.minimum(0,ibin),hl_1.maximum(0,ibin));
			if (doflavor) hname = Form("ClosureRatioVsRefPt_JetEta%gto%g_%sover%s",hl_1.minimum(0,ibin),hl_1.maximum(0,ibin),flavor1.Data(),flavor2.Data());
		}
		else if(var == VARIABLES::jteta) {
			hname = Form("ClosureRatioVsJetEta_RefPt%gto%g",hl_1.minimum(hl_1.nvariables()-1,indices.back()),hl_1.maximum(hl_1.nvariables()-1,indices.back()));
			if (doflavor) hname = Form("ClosureRatioVsJetEta_RefPt%gto%g_%sover%s",hl_1.minimum(hl_1.nvariables()-1,indices.back()),hl_1.maximum(hl_1.nvariables()-1,indices.back()),flavor1.Data(),flavor2.Data());
		}
		ratioHist.back()->SetNameTitle(hname,hname);

		//
		// Divide the numerator (copy) by the denominator
		//
		ratioHist.back()->Divide(hvar2);

		//
		// Format and save the output
		//

		//
		// TPaveText formating
		//
		//float relPosY    = 0.035;
		//float t = 0.05; 0.08
		//float b = 0.13; 0.01
		//1-t - relPosY*(1-t-b)
		int npavelabels = 2;
		double paveymax = 1-0.08-0.035*(1-0.08-0.01)-0.01;//0.89;
		if(!draw_residual) paveymax+=0.02;
		double paveymin = paveymax - (npavelabels)*0.055;
		if(!algDiv)
			pave.push_back(tdrText(0.3,0.74,0.8,0.88));
		else
			pave.push_back(tdrText(0.475,paveymin,0.675,paveymax));

		//pave.back()->AddText("QCD Monte Carlo");
		if(!algDiv) {
			pave.back()->AddText(JetInfo::get_legend_title(algo).c_str());
		}
		else {
			pave.back()->AddText(JetInfo::get_legend_title(algo,true,false).c_str());
		}
		if(var == VARIABLES::refpt) {
			pave.back()->AddText(detector_regions_eta[ibin]);
		}
		else if(var == VARIABLES::jteta) {
            if (!multiBin) {
               pave.back()->AddText(Form("%g < %s < %g GeV",hl_1.minimum(0,ibin),
                                         getVariableAxisTitleString(VARIABLES::refpt,false).c_str(),
                                         hl_1.maximum(0,ibin)));
            }
            else {
               pave.back()->AddText(Form("%s > %g GeV",getVariableAxisTitleString(VARIABLES::refpt,false).c_str(),
                                       hl_1.minimum(0,ibin)));
            }
		}

		//
		// Setup the frame, canvas, and legend
		//
		TH1D* frame = new TH1D();
		TH1D* frameRatio = new TH1D();
		if(var == VARIABLES::refpt) {
			if(TString(algo).Contains("pf",TString::kIgnoreCase)) {
				frame->GetXaxis()->SetLimits(XminPF[ibin],Xmax[ibin]);
				frameRatio->GetXaxis()->SetLimits(XminPF[ibin],Xmax[ibin]);
			}
			else {
				frame->GetXaxis()->SetLimits(XminCalo[ibin],Xmax[ibin]);
				frameRatio->GetXaxis()->SetLimits(XminCalo[ibin],Xmax[ibin]);
			}
			//frame->GetYaxis()->SetRangeUser(0.95,1.05);
			//frame->GetYaxis()->SetRangeUser(0.35,1.35);
            frame->GetYaxis()->SetRangeUser(0.00,1.3);
		}
		else if(var == VARIABLES::refeta || var == VARIABLES::jteta) {
			float etaMax = min(eta_max,TMath::ACosH(CMEnergy/2.0/vpt[ibin]));
			if(hl_1.minimum(0,ibin)==30 && hl_1.maximum(0,ibin)==10000) {
				float minEta = max(-eta_max,veta[0]);
				float maxEta = min(eta_max,veta[NETA]);
				frame->GetXaxis()->SetLimits(minEta,maxEta);
				frameRatio->GetXaxis()->SetLimits(minEta,maxEta);
			}	
			else {
				frame->GetXaxis()->SetLimits(-etaMax,etaMax);
				frameRatio->GetXaxis()->SetLimits(-etaMax,etaMax);
			}
			//frame->GetYaxis()->SetRangeUser(0.95,1.05);
			frame->GetYaxis()->SetRangeUser(0.55,1.40);
		}
		frame->GetXaxis()->SetMoreLogLabels();
		frame->GetXaxis()->SetNoExponent();
		frame->GetXaxis()->SetTitle(getVariableAxisTitleString(var).c_str());
		frame->GetYaxis()->SetTitle("Response");
		frameRatio->GetXaxis()->SetMoreLogLabels();
		frameRatio->GetXaxis()->SetNoExponent();
		frameRatio->GetXaxis()->SetTitle(getVariableAxisTitleString(var).c_str());
		//frameRatio->GetYaxis()->SetRangeUser(0.95,1.05);
		frameRatio->GetYaxis()->SetRangeUser(0.0,2.0);
		//frameRatio->GetYaxis()->SetTitle("Ratio");
		if(algDiv)
			frameRatio->GetYaxis()->SetTitle(jiNumerator->getJetType()+"/"+jiDenominator->getJetType());
		else
			frameRatio->GetYaxis()->SetTitle((numerator+"/"+denominator).c_str());
		if(draw_residual) {
			canvases_legends.push_back(make_pair(tdrDiCanvas(hname,frame,frameRatio,14,11),
												 tdrLeg(0.78,paveymin,0.95,paveymax)));
			pad_numbers = make_pair(1,2);
			frameRatio->GetXaxis()->SetTitleOffset(frameRatio->GetXaxis()->GetTitleOffset()+0.05);
		}
		else {
			canvases_legends.push_back(make_pair(tdrCanvas(hname,frame,14,11,true),
												 tdrLeg(0.78,paveymin,0.95,paveymax+0.02)));
			pad_numbers = make_pair(0,-1);
			frame->GetXaxis()->SetTitleOffset(frame->GetXaxis()->GetTitleOffset()+0.05);
		}
		if(var == VARIABLES::refpt && ibin<3) {
			canvases_legends.back().first->GetPad(pad_numbers.first)->SetLogx();
			if(draw_residual) {
				canvases_legends.back().first->GetPad(pad_numbers.second)->SetLogx();
			}
		}

		//
		// Add text to the legend
		//
		/*
		TString text;
		if(var == VARIABLES::refpt) {
			text = detector_regions_eta[ibin];
		}
		else if(var == VARIABLES::jteta) {
			text = Form("%g < p_{T}^{ptcl} < %g GeV",hl_1.minimum(0,ibin),hl_1.maximum(0,ibin));
		}
		canvases_legends.back().second->AddEntry((TObject*)0,text,"");
		if(!algDiv) {
			text = JetInfo::get_legend_title(algo).c_str();
		}
		else {
			text = JetInfo::get_legend_title(algo,true,false).c_str();
		}
		canvases_legends.back().second->AddEntry((TObject*)0,text,"");
		*/

		//
		// cd to the upper canvas
		//
		canvases_legends.back().first->cd(pad_numbers.first);

		//
		// Draw the guide lines
		//
		line->Draw("same");
		if(draw_guidelines) {
			linePlus->Draw("same");
			lineMinus->Draw("same");
		}
		if(draw_residual) {
			canvases_legends.back().first->cd(pad_numbers.second);
			line->Draw("same");
		}
		canvases_legends.back().first->cd(pad_numbers.first);

		//
		// Set the range of the histograms themselves in case you are mixing PF and Calo algorithms
		//
		if(var == VARIABLES::refpt && algDiv) {
			if(TString(numerator).Contains("PF",TString::kIgnoreCase) &&
			   TString(denominator).Contains("Calo",TString::kIgnoreCase)) {
				hvar1->GetXaxis()->SetRangeUser(XminPF[ibin],Xmax[ibin]);
				hvar2->GetXaxis()->SetRangeUser(XminCalo[ibin],Xmax[ibin]);
				ratioHist.back()->GetXaxis()->SetRangeUser(XminCalo[ibin],Xmax[ibin]);
			}
			else if(TString(numerator).Contains("Calo",TString::kIgnoreCase) &&
			        TString(denominator).Contains("PF",TString::kIgnoreCase)) {
				hvar1->GetXaxis()->SetRangeUser(XminCalo[ibin],Xmax[ibin]);
				hvar2->GetXaxis()->SetRangeUser(XminPF[ibin],Xmax[ibin]);
				ratioHist.back()->GetXaxis()->SetRangeUser(XminCalo[ibin],Xmax[ibin]);	
			}
		}
		else if(var == VARIABLES::refeta || var == VARIABLES::jteta) {
			float etaMax = min(eta_max,TMath::ACosH(CMEnergy/2.0/vpt[ibin]));
			if(hl_1.minimum(0,ibin)==30 && hl_1.maximum(0,ibin)==10000) {
				float minEta = max(-eta_max,veta[0]);
				float maxEta = min(eta_max,veta[NETA]);
				hvar1->GetXaxis()->SetRangeUser(minEta,maxEta);
				hvar2->GetXaxis()->SetRangeUser(minEta,maxEta);
				ratioHist.back()->GetXaxis()->SetRangeUser(minEta,maxEta);
			}	
			else {
				hvar1->GetXaxis()->SetRangeUser(-etaMax,etaMax);
				hvar2->GetXaxis()->SetRangeUser(-etaMax,etaMax);
				ratioHist.back()->GetXaxis()->SetRangeUser(-etaMax,etaMax);
			}
		}

		//
		// Draw the histograms
		//
		tdrDraw(hvar1,"EP",kFullSquare,kBlue+1,kSolid,kBlue+1);
		tdrDraw(hvar2,"EP",kFullCircle,kRed+1,kSolid,kRed+1);
		if(draw_residual) {
			canvases_legends.back().first->cd(pad_numbers.second);
			tdrDraw(ratioHist.back(),"EP",kFullCircle,kBlack,kSolid,kBlack);
		}
		canvases_legends.back().first->cd(pad_numbers.first);

		//
		// Set the legend entries and draw the legend
		//
		if(algDiv) {
			canvases_legends.back().second->AddEntry(hvar1,jiNumerator->getJetType(),"lep");
			canvases_legends.back().second->AddEntry(hvar2,jiDenominator->getJetType(),"lep");
			//canvases_legends.back().second->AddEntry(ratioHist.back(),"Ratio = #frac{"+
			//                                         jiNumerator->getJetType()+"}{"+jiDenominator->getJetType()+
			//                                         "}","lep");
		}
		else {
			canvases_legends.back().second->AddEntry(hvar1,numerator.c_str(),"lep");
			canvases_legends.back().second->AddEntry(hvar2,denominator.c_str(),"lep");			
			//canvases_legends.back().second->AddEntry(ratioHist.back(),"Ratio = #frac{"+TString(numerator)+"}{"+TString(denominator)+"}","lep");
		}
		canvases_legends.back().second->Draw("same");

		//
		// Draw the text
		//
		if(pave.back())
			pave.back()->Draw("EP");

		//
		// Save the canvas
		//
		for(unsigned int f=0; f<outputFormat.size(); f++) {
			canvases_legends.back().first->SaveAs(outputDir+"/"+hname+"_"+algo+outputFormat[f]);
		}
		ofile->cd();
		ratioHist.back()->Write();
		canvases_legends.back().first->Write();
	}

	//
	// Close the input and output files
	//
	closeFiles();
}


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
    histMet = "mu_h";
    histogramMetric = HistUtil::getHistogramMetricType(histMet);
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
    delphes    = cl.getValue<bool>    ("delphes",        false);
    maxFitIter = cl.getValue<int>     ("maxFitIter",        30);
    histMet    = cl.getValue<string>  ("histMet",       "mu_h");
    histogramMetric = HistUtil::getHistogramMetricType(histMet);

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
    vabscor_eta_spline.erase(vabscor_eta_spline.begin(),vabscor_eta_spline.end());
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
void L2Creator::loopOverAlgorithms(string makeCanvasVariable) {
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
        // Write the L2 correction text file for the current algorithm
        // Don't need splines for the separated L2/L3 file because no splines are implemented for thos fits
        //
        if(l2l3 && (alg.find("pf")!=string::npos || alg.find("puppi")!=string::npos) && l2pffit.Contains("spline"))
            writeTextFileForCurrentAlgorithm_spline();
        else
            writeTextFileForCurrentAlgorithm();

        //
        // Check that the FormulaEvaluator returns the same value as the TF1 used to create the fit
        // This is necessary because several times in the past the FormulaEvaluator has returned strange values
        //
        assert(checkFormulaEvaluator());

        //
        // Draw several canvases of the graphs and associated fits
        //
        if(!makeCanvasVariable.empty()) {
            makeCanvas(makeCanvasVariable);
        }

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
        if (hrsp->GetEntries() > 4) {//hrsp->Integral()!=0) {

            //TF1*  frsp    = (TF1*)hrsp->GetListOfFunctions()->Last();
            //std::cout << "hrspName = " << hrsp->GetName() << ": frsp = " << frsp << std::endl;
            TH1F* hrefpt  = hl_refpt.object(indices);
            TH1F* hjetpt  = hl_jetpt.object(indices);

            assert(hrefpt->GetEntries()>0&&hjetpt->GetEntries()>0);

            double ejetpt;
            double jetpt;
            double erefpt;
            double refpt;
            //if(histogramMetric == HistUtil::median) {
            //    ejetpt =HistUtil::getHistogramMetric1D(histogramMetric,hjetpt).second;
            //    jetpt  =HistUtil::getHistogramMetric1D(histogramMetric,hjetpt).first;
            //    erefpt =HistUtil::getHistogramMetric1D(histogramMetric,hrefpt).second;
            //    refpt  =HistUtil::getHistogramMetric1D(histogramMetric,hrefpt).first;
            //}
            //else {
            ejetpt =HistUtil::getHistogramMetric1D(HistUtil::mu_h,hjetpt).second;
            jetpt  =HistUtil::getHistogramMetric1D(HistUtil::mu_h,hjetpt).first;
            erefpt =HistUtil::getHistogramMetric1D(HistUtil::mu_h,hrefpt).second;
            refpt  =HistUtil::getHistogramMetric1D(HistUtil::mu_h,hrefpt).first;
            //}

            double peak = HistUtil::getHistogramMetric1D(histogramMetric,hrsp).first;
            double epeak = HistUtil::getHistogramMetric1D(histogramMetric,hrsp).second;
            //if(alg.find("calo")!=string::npos) {
            //    peak = (frsp==0 || !mpv)?hrsp->GetMean():frsp->GetParameter(1);
            //    epeak = (frsp==0 || !mpv)?hrsp->GetMeanError():frsp->GetParError(1);
            //}
            //else if(alg.find("pf")!=string::npos) {
            //    peak = (frsp==0 || !mpv)?hrsp->GetMean():frsp->GetParameter(1);
            //    epeak = (frsp==0 || !mpv)?hrsp->GetMeanError():frsp->GetParError(1);
            //}
            //else {
            //    peak = (frsp==0 || !mpv)?hrsp->GetMean():frsp->GetParameter(1);
            //    epeak = (frsp==0 || !mpv)?hrsp->GetMeanError():frsp->GetParError(1);
            //}

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
                // Sort the points. Doesn't matter which ref pt produced a given jet pt as long as they are sorted.
                gabscor->Sort();
                xmin = max(gabscor->GetX()[0],10.0);
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
                        fabscor->FixParameter(6,xmin);
                        fabscor->FixParameter(7,0.0);
                    }
                    //
                    // offline
                    //
                    else {
                        TString fcn = getOfflinePFFunction();

                        if(l2pffit.Contains("ErrorFunction",TString::kIgnoreCase))
                            xmin = gabscor->GetX()[0];
                        else if(l2pffit.Contains("standard+spline",TString::kIgnoreCase)) {
                            xmin = findNext(10.0,gabscor,false);
                            if(xmax>1000.0)
                                xmax = min(findNext(1000.0,gabscor,true),xmax);
                            vector<double> merge_points = {xmin,xmax};
                            vabscor_eta_spline.push_back(new PiecewiseSpline(string("spline_")+gabscor->GetName(),gabscor,merge_points,PiecewiseSpline::getROOTSplineType(string(l2pffit)),!l2pffit.Contains("akima",TString::kIgnoreCase)&&!l2pffit.Contains("steffen",TString::kIgnoreCase)));
                        }
                        else if(l2pffit.EqualTo("spline3",TString::kIgnoreCase) || l2pffit.EqualTo("spline5",TString::kIgnoreCase) || l2pffit.EqualTo("splineAkima",TString::kIgnoreCase) || l2pffit.EqualTo("splineSteffen",TString::kIgnoreCase)) {
                            xmin = gabscor->GetX()[0];
                            vabscor_eta_spline.push_back(new PiecewiseSpline(string("spline_")+gabscor->GetName(),gabscor,{},PiecewiseSpline::getROOTSplineType(string(l2pffit)),!l2pffit.Contains("akima",TString::kIgnoreCase)&&!l2pffit.Contains("steffen",TString::kIgnoreCase)));
                        }
                        if(l2pffit.Contains("akima",TString::kIgnoreCase)) {
                            gsl_spline *spline_akima = gsl_spline_alloc(gsl_interp_akima, gabscor->GetN());
                            gsl_spline_init(spline_akima, gabscor->GetX(), gabscor->GetY(), gabscor->GetN());
                            vabscor_eta_spline.back()->setSpline(PiecewiseSpline::gslToROOT_spline(spline_akima,"TSpline3_akima"));
                        }

                        fabscor=new TF1("fit",fcn.Data(),xmin,xmax);
                        setOfflinePFParameters(gabscor, fabscor,xmin,xmax);

                        if(l2pffit.Contains("spline",TString::kIgnoreCase)) {
                            vabscor_eta_spline.back()->setPartialFunction(fabscor);
                        }
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
                    if(l2calofit.EqualTo("standard",TString::kIgnoreCase)) {
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
                    else if(l2calofit.EqualTo("Erf",TString::kIgnoreCase)) {
                        xmin=hl_jetpt.minimum(0,ieta);
                        fabscor = new TF1("fit","[0]+[1]*TMath::Erf([2]*(log10(x)-[3]))+[4]*exp([5]*(log10(x)-[6])*(log10(x)-[6]))",xmin,xmax);

                        fabscor->SetParameter(0,-4790.13);
                        fabscor->SetParameter(1,-4791.24);
                        fabscor->SetParameter(2,-3.95017);
                        fabscor->SetParameter(3,0.393983);
                        fabscor->SetParameter(4,0.445609);
                        fabscor->SetParameter(5,-1.18887);
                        fabscor->SetParameter(6,0.956648);

                        if(ieta==3) {
                           fabscor->SetParameter(0,-4790.16);
                           fabscor->SetParameter(1,-4791.21);
                           fabscor->SetParameter(2,-93.1228);
                           fabscor->SetParameter(3,1.21101);
                           fabscor->SetParameter(4,0.112504);
                           fabscor->SetParameter(5,-10.5307);
                           fabscor->SetParameter(6,1.36997);
                        }
                        if(ieta>=11 && ieta<=14) {
                            fabscor->SetParameter(0,-4790.19);
                            fabscor->SetParameter(1,-4791.18);
                            fabscor->SetParameter(2,-5.25329);
                            fabscor->SetParameter(3,0.714413);
                            fabscor->SetParameter(4,0.266478);
                            fabscor->SetParameter(5,-1.73264);
                            fabscor->SetParameter(6,1.24294);
                        }
                        if(ieta>=15 && ieta<=69) {
                           fabscor->SetParameter(0,-5796.16);
                           fabscor->SetParameter(1,-5797.2);
                           fabscor->SetParameter(2,-4.23654);
                           fabscor->SetParameter(3,0.729816);
                           fabscor->SetParameter(4,0.14041);
                           fabscor->SetParameter(5,-0.995273);
                           fabscor->SetParameter(6,0.870276);
                        }
                        if(ieta==77) {
                            fabscor->SetParameter(0,-4790.16);
                            fabscor->SetParameter(1,-4791.21);
                            fabscor->SetParameter(2,-94.5755);
                            fabscor->SetParameter(3,1.20769);
                            fabscor->SetParameter(4,0.109479);
                            fabscor->SetParameter(5,-11.3274);
                            fabscor->SetParameter(6,1.37828);
                        }
                    }
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
                    else if(l2calofit.EqualTo("Erf",TString::kIgnoreCase)) {
                        if (xmin<6) xmin=6;
                        fabscor = new TF1("fit","[0]+[1]*TMath::Erf([2]*(log10(x)-[3]))+[4]*exp([5]*(log10(x)-[6])*(log10(x)-[6]))",xmin,xmax);

                        fabscor->SetParameter(0,-4790.13);
                        fabscor->SetParameter(1,-4791.24);
                        fabscor->SetParameter(2,-3.95017);
                        fabscor->SetParameter(3,0.393983);
                        fabscor->SetParameter(4,0.445609);
                        fabscor->SetParameter(5,-1.18887);
                        fabscor->SetParameter(6,0.956648);
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
            if(gabscor->GetListOfFunctions()->GetSize()>0)
                gabscor->GetListOfFunctions()->First()->ResetBit(TF1::kNotDraw);
            gabsrsp->SetMarkerStyle(20);
            gabscor->SetMarkerStyle(20);
            odir->cd();
            gabsrsp->Write();
            gabscor->Write();
            ofile->Write();
        }
        else if (ipt==hl_jetpt.nobjects(1)-1 && ((vabsrsp_eta.back())->GetN()==0 || (vabscor_eta.back())->GetN()==0)) {
            vabscor_eta_spline.push_back(nullptr);
        }
    }
}

//______________________________________________________________________________
bool L2Creator::checkFormulaEvaluator() {
    vector<double> pt_to_check  = {10.0,30.0,100.0,500.0,1000.0,2000.0,3000.0,4000.0};

    unsigned int vector_size = 0;
    if(l2l3) vector_size = vabscor_eta.size(); //For L2L3 Corrections Together
    else vector_size = vrelcor_eta.size(); //For L2 & L3 Corrections Separate
    for (unsigned int ieta=0;ieta<vector_size;ieta++) {
        //
        // Load the appropriate graph
        //
        TGraph* gcor;
        if(l2l3) gcor = vabscor_eta[ieta]; //For L2L3 Corrections Together
        else gcor = vrelcor_eta[ieta]; //For L2 & L3 Corrections Separate

        //
        // Load the appropriate TF1
        //
        TF1 *root_func(nullptr);
        PiecewiseSpline* spline(nullptr);

        // This checks if this is a spline function or simply a regular TF1
        if(vabscor_eta_spline.size()>0) {
            //Load the spline function
            spline = vabscor_eta_spline[ieta];
            //If the spline is a nullptr then skip this ieta as it indicates that the abscor graph wasn't filled
            if (spline==nullptr) continue;
            root_func = spline->getFullFunction();
        }
        else {
            //Load the regular TF1
            root_func = (TF1*)gcor->GetListOfFunctions()->Last();
        }

        //
        // Load the FactorizedJetCorrector
        //
        string txtfilename = string(outputDir + era + "_L2Relative_" + ji->getAlias() + ".txt");
        JetCorrectorParameters *L2JetPar = new JetCorrectorParameters(txtfilename);
        vector<JetCorrectorParameters> vPar;
        vPar.push_back(*L2JetPar);
        FactorizedJetCorrector *JetCorrector = new FactorizedJetCorrector(vPar);

        //
        // Do the actual testing
        //
        if(root_func!=nullptr) {
            double  eta_avg  = (hl_jetpt.minimum(0,ieta)+hl_jetpt.maximum(0,ieta))/2.0;
            double  ptmin = gcor->GetX()[0];
            double  ptmax = gcor->GetX()[gcor->GetN()-1];

            double pt_limit = 70.0;
            //For eta-dependent spline clipping
            if      (abs(eta_avg) < 0.6091) pt_limit = 3000;
            else if (abs(eta_avg) < 0.9571) pt_limit = 2700;
            else if (abs(eta_avg) < 1.3051) pt_limit = 2000;
            else if (abs(eta_avg) < 2.0431) pt_limit = 1400;
            else if (abs(eta_avg) < 2.51  ) pt_limit = 900;
            else if (abs(eta_avg) < 2.9641) pt_limit = 500;
            else if (abs(eta_avg) < 3.6641) pt_limit = 300;
            else if (abs(eta_avg) < 4.0131) pt_limit = 200;
            else if (abs(eta_avg) < 4.5381) pt_limit = 100;

            for(auto ipt : pt_to_check) {

                //
                // Check that ipt is not outside [ptmin,ptmax]
                //
                if (ipt<ptmin || ipt>ptmax) continue;

                //
                // Check that the ipt is not outside the pt clipping area
                //
                if(ipt > pt_limit) continue;

                //
                // Set the inputs for the FactorizedJetCorrector
                // Need to return the correction here because one must reset the inputs each time there is a call to getCorrection()
                //
                JetCorrector->setJetPt(ipt);
                JetCorrector->setJetEta(eta_avg);
                double fe_value = JetCorrector->getCorrection();

                //
                // Need the actual pT value to determine the spline section and load the appropriate parameters.
                //
                int spline_section = -1;
                pair<double,double> spline_section_bounds;
                if(vabscor_eta_spline.size()>0) {
                    root_func = spline->setParameters(spline->getSection(ipt));
                    spline_section = spline->getSection(ipt);
                    spline_section_bounds = spline->getSectionBounds(spline_section);
                }

                //
                // Do the comparison
                //
                if(abs(fe_value-std::max(0.0001,root_func->Eval(ipt)))>0.0006) {
                    cout << "ERROR::L2Creator::checkFormulaEvaluator TF1 and FormulaEvaluator do not agree!" << endl
                         << "pT: " << ipt << " eta: " << eta_avg << endl
                         << " TF1: " << std::setprecision(10) << root_func->Eval(ipt) << endl
                         << " TF1: "; root_func->Print();
                    cout << " TF1: ";
                    for(int ipar=0; ipar<root_func->GetNpar(); ipar++) {
                       cout << " " << root_func->GetParameter(ipar) << " ";
                    }
                    cout << endl;
                    if(spline_section>-1) {
                       cout << " TF1: section=" << spline_section << endl
                            << " TF1: section bounds=(" << spline_section_bounds.first << ","
                            << spline_section_bounds.second << ")" << endl;
                    }
                    cout << " FormulaEvaluator: " << std::setprecision(10) << fe_value << endl
                         << " FormulaEvaluator: " << L2JetPar->definitions().formula() << endl
                         << " FormulaEvaluator: ";
                    vector<float> parVar = {(float)eta_avg,(float)ipt};
                    int bi = L2JetPar->binIndex(parVar);
                    for(unsigned int ipar=0; ipar<L2JetPar->record(bi).nParameters(); ipar++) {
                       cout << " " << L2JetPar->record(bi).parameter(ipar) << " ";
                    }
                    cout << endl;
                    L2JetPar->printScreen();
                    return false;
                }
            }
        }
    }

    cout << "L2Creator::checkFormulaEvaluator All pT and eta values checked agree for TF1 and FormulaEvaluator." << endl;

    return true;



    /*

TF1* f = new TF1("f","[0]+[1]/(pow(log10(x),2)+[2])+[3]*exp(-[4]*(log10(x)-[5])*(log10(x)-[5]))",3.70269,3499.16)
f->SetParameters( 0.7111,9.24906,16.3009,-0.127602,0.96894,1.57828)
f->Eval(10.0)




    */
}

//______________________________________________________________________________
void L2Creator::makeCanvas(string makeCanvasVariable) {
    //
    // Delete any canvases that may still exist from other algorithms
    //
    TList* loc = (TList*)gROOT->GetListOfCanvases();
    TListIter itc(loc);
    TObject *o(0);
    while ((o = itc())) delete o;

    //
    // Containers and canvas settings
    //
    vector<TCanvas*> c;
    vector<TGraphErrors*> graphs;
    vector<TLegend*> vleg;
    int nperpad(4), pos(0);

    //
    // The objects we will be working with
    //
    if     (makeCanvasVariable=="AbsRspVsRefPt:JetEta") graphs = vabsrsp_eta;
    else if(makeCanvasVariable=="AbsCorVsJetPt:JetEta") graphs = vabscor_eta;
    //else if(makeCanvasVariable=="RelCorVsJetPt:JetEta") graphs = vrelcor_eta;
    else {
        cout << "ERROR::L2Creator::makeCanvas Unknown makeCanvasVariable!" << endl;
        return;
    }

    //
    // The frame and axis settings
    //
    TH1D* frame = new TH1D();
    frame->GetXaxis()->SetLimits(3.0001,5000.0);
    frame->GetXaxis()->SetMoreLogLabels();
    frame->GetXaxis()->SetNoExponent();
    frame->GetXaxis()->SetTitle(makeCanvasVariable.substr(makeCanvasVariable.find("Vs")+2,5).c_str());
    frame->GetYaxis()->SetRangeUser(0.8,2.2);
    frame->GetYaxis()->SetTitle(makeCanvasVariable.substr(0,makeCanvasVariable.find("Vs")).c_str());

    for(unsigned int igraph=0; igraph<graphs.size(); igraph++) {

        int icnv = igraph/nperpad;
        string gname = string(graphs[igraph]->GetName());

        //
        // For each new canvas
        //
        if (c.size()==0||igraph%nperpad==0) {
            pos = makeCanvasVariable.find(":");
            stringstream sscname; sscname<<makeCanvasVariable.substr(0,pos)<<"_"<<makeCanvasVariable.substr(pos+1)<<"_"<<icnv;
            c.push_back(tdrCanvas(sscname.str().c_str(),frame,14,0,true));
            c.back()->SetLogx();

            double legx(0.45), legy(0.9), legw(0.4);
            double legxmin = legx;
            double legymin = legy;
            double legxmax = legx+legw;
            double legymax = legymin - (nperpad+1)*0.055;

            vleg.push_back(tdrLeg(legxmin,legymin,legxmax,legymax));
            vleg.back()->SetHeader(makeCanvasVariable.c_str());
        }

        //
        // Draw the graph and TF1 fit
        // Create a legend entry for that graph
        //
        if(graphs[igraph]->GetFunction("fit"))
            graphs[igraph]->GetFunction("fit")->SetLineColor(igraph%nperpad+1);
        tdrDraw(graphs[igraph],"P",kFullCircle,igraph%nperpad+1,kSolid,igraph%nperpad+1);
        pos = gname.find(makeCanvasVariable.substr(makeCanvasVariable.find(":")+1));
        vleg.back()->AddEntry(graphs[igraph],gname.substr(pos+makeCanvasVariable.substr(makeCanvasVariable.find(":")+1).length()).c_str(),"l");

        //
        // Turn off the fit stats because apparently they are not being turned off by the gStyle
        //
        c.back()->Update();
        TPaveStats* stat = (TPaveStats*)graphs[igraph]->GetListOfFunctions()->FindObject("stats");
        if(stat) {
            stat->SetOptFit(0);
            stat->SetOptStat(0);
            stat->SetX1NDC(1.1);
            stat->SetX2NDC(1.1);
            stat->SetY1NDC(1.1);
            stat->SetY2NDC(1.1);
            c.back()->Modified();
        }
        else {
            cout << "WARNING::L2Creator::makeCanvas Did not find the \"stats\" object" << endl;
        }

        //
        // Draw the splines for each graph, if they exist
        //
        if(vabscor_eta_spline.size()>0) {
            PiecewiseSpline* spline = vabscor_eta_spline[igraph];
            for(int isection=0; spline && isection<spline->getNSections(); isection++) {
                pair<double,double> bounds = spline->getSectionBounds(isection);
                TF1* spline_func = (TF1*)spline->setParameters(isection)->Clone("tmp_func");
                spline_func->SetRange(bounds.first,bounds.second);
                spline_func->SetLineColor(igraph%nperpad+1);
                spline_func->SetLineStyle(kDotted);
                spline_func->Draw("same");
            }
        }

        //
        // Draw the legend and save the canvas (last thing before making a new canvas or ending loop)
        //
        if((igraph+1)%nperpad==0 || igraph==graphs.size()-1) {
            vleg.back()->Draw("same");
            c.back()->Write();
            for(unsigned int iformat=0; iformat<formats.size(); iformat++) {
                c.back()->SaveAs(Form("%s%s%s",outputDir.Data(),c.back()->GetName(),formats[iformat].c_str()));
            }
        }
    }
    //make overview canvas
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
    if(l2pffit.EqualTo("standard",TString::kIgnoreCase) || (l2pffit.Contains("standard",TString::kIgnoreCase)&&l2pffit.Contains("spline",TString::kIgnoreCase)) ) {
        return "[0]+([1]/(pow(log10(x),2)+[2]))+([3]*exp(-[4]*(log10(x)-[5])*(log10(x)-[5])))";
    }
    else if(l2pffit.EqualTo("standard+Gaussian",TString::kIgnoreCase)) {
        return "[0]+([1]/(pow(log10(x),2)+[2]))+([3]*exp(-([4]*((log10(x)-[5])*(log10(x)-[5])))))+([6]*exp(-([7]*((log10(x)-[8])*(log10(x)-[8])))))";
    }
    else if(l2pffit.EqualTo("LogNormal+Gaussian+fixed",TString::kIgnoreCase)) {
        return "([0]+TMath::LogNormal(TMath::Log10(x),[1],[2],[3]))+([4]+[5]/(pow(log10(x),2)+[6])+[7]*exp(-[8]*(log10(x)-[9])*(log10(x)-[9])))";
    }
    else if(l2pffit.EqualTo("LogNormal+Gaussian",TString::kIgnoreCase)) {
        return "(x<=[10])*([0]+TMath::LogNormal(TMath::Log10(x),[1],[2],[3]))+(x>[10])*([4]+[5]/(pow(log10(x),2)+[6])+[7]*exp(-[8]*(log10(x)-[9])*(log10(x)-[9])))";
        //return "(x<=[10])*([0]+((1.0/(TMath::Log10(x)*TMath::Sqrt(2.0*TMath::Pi()*TMath::Power([2],2))))*TMath::Exp(-TMath::Power(TMath::Log(TMath::Log10(x))-[1],2)/(2.0*TMath::Power([2],2)))))+(x>[10])*([4]+[5]/(pow(log10(x),2)+[6])+[7]*exp(-[8]*(log10(x)-[9])*(log10(x)-[9])))";
    }
    else if(l2pffit.EqualTo("ErrorFunction+standard",TString::kIgnoreCase)) {
        return "[0]+([1]*TMath::Erf([2]*log10(x)-[3]))+([4]/(pow(log10(x),2)+[5]))+([6]*exp(-[7]*pow(log10(x)-[8],2)))";
    }
    else if(l2pffit.EqualTo("ErrorFunction+Gaussian",TString::kIgnoreCase)) {
        return "[0]+([1]*TMath::Erf([2]*(log10(x)-[3])))+([4]*exp([5]*(log10(x)-[6])*(log10(x)-[6])))";
    }
    else if(l2pffit.EqualTo("ErrorFunction+standard+Gaussian",TString::kIgnoreCase)) {
        return "[0]+([1]*TMath::Erf([2]*log10(x)-[3]))+([4]/(pow(log10(x),2)+[5]))+([6]*exp(-[7]*pow(log10(x)-[8],2)))+([9]*exp(-[10]*pow(log10(x)-[11],2)))";
    }
    else if(l2pffit.EqualTo("spline3",TString::kIgnoreCase) || l2pffit.EqualTo("splineAkima",TString::kIgnoreCase) || l2pffit.EqualTo("splineSteffen",TString::kIgnoreCase)) {
        return "[0]+((x-[1])*([2]+((x-[1])*([3]+((x-[1])*[4])))))";
    }
    else if(l2pffit.EqualTo("spline5",TString::kIgnoreCase)) {
        return "[0]+((x-[1])*([2]+((x-[1])*([3]+((x-[1])*([4]+((x-[1])*([5]+((x-[1])*[6])))))))))";
    }
    //else if(l2pffit.EqualTo("standard+spline3",TString::kIgnoreCase)) {
    //    return "[0]+([1]/(pow(log10(x),2)+[2]))+([3]*exp(-[4]*(log10(x)-[5])*(log10(x)-[5])))";
    //}
    //else if(l2pffit.EqualTo("standard+spline5",TString::kIgnoreCase)) {
    //    return "[0]+([1]/(pow(log10(x),2)+[2]))+([3]*exp(-[4]*(log10(x)-[5])*(log10(x)-[5])))";
    //}
    //else if(l2pffit.EqualTo("standard+splineAkima",TString::kIgnoreCase)) {
    //    return "[0]+([1]/(pow(log10(x),2)+[2]))+([3]*exp(-[4]*(log10(x)-[5])*(log10(x)-[5])))";
    //}
    else {
        cout << "ERROR::getOfflinePFFunction::Unknown PF function choice." << endl;
        return "";
    }
}

//______________________________________________________________________________
void L2Creator::setOfflinePFParameters(TGraphErrors* gabscor, TF1* fabscor, double xmin, double xmax) {
    TF1* flog(0);
    TF1* fgaus(0);
    if(l2pffit.Contains("LogNormal+Gaussian",TString::kIgnoreCase)) {
        //int origIgnoreLevel = gErrorIgnoreLevel;
        //gErrorIgnoreLevel = kBreak;
        setAndFitFLogAndFGaus(gabscor, flog, fgaus, xmin);
        //gErrorIgnoreLevel = origIgnoreLevel;
        //Original function
    }

    if(l2pffit.EqualTo("standard",TString::kIgnoreCase) || l2pffit.EqualTo("standard+spline3",TString::kIgnoreCase) ||
       l2pffit.EqualTo("standard+spline5",TString::kIgnoreCase) || l2pffit.EqualTo("standard+splineAkima",TString::kIgnoreCase) ||
       l2pffit.EqualTo("standard+splineSteffen",TString::kIgnoreCase)) {
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
  else if(l2pffit.EqualTo("ErrorFunction+standard",TString::kIgnoreCase)) {
    fabscor->SetParameter(0,1.0);
    fabscor->SetParameter(1,1.0);
    fabscor->SetParameter(2,-1.0);
    fabscor->SetParameter(3,2.0);
    fabscor->SetParameter(4,9.0);
    fabscor->SetParameter(5,8.0);
    fabscor->SetParameter(6,-0.3);
    fabscor->SetParameter(7,0.6);
    fabscor->SetParameter(8,1.0);
  }
  else if(l2pffit.EqualTo("ErrorFunction+Gaussian",TString::kIgnoreCase)) {
    fabscor->SetParameter(0,1.0);
    fabscor->SetParameter(1,1.0);
    fabscor->SetParameter(2,-1.0);
    fabscor->SetParameter(3,2.0);
    fabscor->SetParameter(4,-0.3);
    fabscor->SetParameter(5,1.5);
    fabscor->SetParameter(6,3.0);

    fabscor->SetParLimits(0,-10,10);
    fabscor->SetParLimits(1,-100,100);
    fabscor->SetParLimits(4,-100,0);
    fabscor->SetParLimits(5,0,100);
  }
  else if(l2pffit.EqualTo("ErrorFunction+standard+Gaussian",TString::kIgnoreCase)) {
    fabscor->SetParameter(0,1.0);
    fabscor->SetParameter(1,1.0);
    fabscor->SetParameter(2,-1.0);
    fabscor->SetParameter(3,2.0);
    fabscor->SetParameter(4,9.0);
    fabscor->SetParameter(5,8.0);
    fabscor->SetParameter(6,-0.3);
    fabscor->SetParameter(7,0.6);
    fabscor->SetParameter(8,1.0);
    fabscor->SetParameter(9,-0.0679365);
    fabscor->SetParameter(10,3.82597);
    fabscor->SetParameter(11,1.8277);
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
            double jetpt    =HistUtil::getHistogramMetric1D(HistUtil::mu_h,hjetpt).first;//hjetpt->GetMean();
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
double L2Creator::findNext(double xvalue,TGraph* g, bool highest, bool debug) {
    if(xvalue<g->GetX()[0] || xvalue>g->GetX()[g->GetN()-1]) {
        cout << "WARNING::L2Creator::findNext The xvalue you indicated(" << xvalue << ") is outside the graph range ["
             << g->GetX()[0] << ", "<< g->GetX()[g->GetN()-1] <<"]" << endl;
        return -9999.0;
    }

    if(debug) cout << "\tStarting value: " << xvalue << endl;

    double next = (highest) ? g->GetX()[0] : g->GetX()[g->GetN()-1];
    if(highest) {
        for(int ipoint=0; ipoint<g->GetN(); ipoint++) {
            if(xvalue>next) next=g->GetX()[ipoint];
            if(debug) cout << "\t\tNext: " << next << endl;
        }
    }
    else {
        for(int ipoint=g->GetN()-1; ipoint>=0; ipoint--) {
            if(xvalue<next) next = g->GetX()[ipoint];
            if(debug) cout << "\t\tNext: " << next << endl;
        }
    }

    if(debug) cout << "\tEnding value: " << next << endl;

    return next;
}

//______________________________________________________________________________
void L2Creator::perform_smart_fit(TGraphErrors * gabscor, TF1 * fabscor, int maxFitIter) {
    cout << "\tFitting on range [" << fabscor->GetXmin() << "," << fabscor->GetXmax() << "] ... " << endl;
    int fitIter = 0, bestIter = 0;
    double bestRChi2 = 0.0;
    vector<TFitResultPtr> fitResultPtrs;
    do {
        //
        // do the fit, get the results and the parameters of the fitted function
        //
        fitResultPtrs.push_back(gabscor->Fit(fabscor,"RQS"));
        //vector<double> auxPars = fitResPtr.Get()->Parameters();

        //
        // compute the reduced chi2 of this fit and if it is the best fit so far
        // then save the parameters
        //
        double rchi2 = fitResultPtrs.back().Get()->Chi2()/ fitResultPtrs.back().Get()->Ndf();
        if (fitResultPtrs.back().Get()->Ndf() == 0) rchi2 = 0;
        if (rchi2 > 0 && (rchi2<bestRChi2 || bestRChi2==0)){
            bestIter = fitIter;
            bestRChi2 = rchi2;
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
    for (unsigned int np=0;np < fitResultPtrs.size() ; np++){
        ffh->SetParameters(&fitResultPtrs[bestIter].Get()->Parameters()[0]);
        fabscor->SetParameters(&fitResultPtrs[bestIter].Get()->Parameters()[0]);
    }
    ffh->SetChisquare(fitResultPtrs[bestIter].Get()->Chi2());
    fabscor->SetChisquare(fitResultPtrs[bestIter].Get()->Chi2());

    //
    // Skip the warnings if using a pure spline and the fit was merely structural
    //
    if(l2pffit.EqualTo("spline3",TString::kIgnoreCase) || l2pffit.EqualTo("spline5",TString::kIgnoreCase) ||
       l2pffit.EqualTo("splineAkima",TString::kIgnoreCase) || l2pffit.EqualTo("splineSteffen",TString::kIgnoreCase))
        return;

    //
    // warn if the fit diverges from graph at low pt
    //
    //if (fabscor->Integral(0,10) > 60)
    //if(abs(gabscor->GetY()[0]-fabscor->Eval(gabscor->GetX()[0]))>0.05)
    //   cout << "\t***ERROR***, fit for histo " << gabscor->GetName() << " diverges at low pt" << endl;

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
            fout<<setw(8)<<etamin<<setw(8)<<etamax
                <<setw(6)<<(int)(frelcor->GetNpar()+2) //Number of parameters + 2
                <<setw(12)<<setprecision(8)<<ptmin
                <<setw(12)<<setprecision(8)<<ptmax;
                for(int p=0; p<frelcor->GetNpar(); p++) {
                   fout<<setw(17)<<setprecision(10)<<frelcor->GetParameter(p);
                }
                fout<<endl;
        }
    }
    fout.close();
}

//______________________________________________________________________________
void L2Creator::writeTextFileForCurrentAlgorithm_spline() {
    TString txtfilename = outputDir + era + "_L2Relative_" + ji->getAlias() + ".txt";
    ofstream fout(txtfilename);
    fout.setf(ios::right);
    bool head_printed = false;

    //For eta-dependent spline clipping
    int pt_limit = 70;

    unsigned int vector_size = 0;
    vector_size = vabscor_eta.size();
    for (unsigned int ieta=0;ieta<vector_size;ieta++) {
        TGraph* grelcor;
        grelcor = vabscor_eta[ieta];
        TF1* frelcor = (TF1*)grelcor->GetListOfFunctions()->Last();
        PiecewiseSpline* spline = vabscor_eta_spline[ieta];
        if(frelcor!=0) {
            string function = spline->getFullFormula();

            if(!head_printed) {
                fout<<"{2 JetEta JetPt 1 JetPt max(0.0001,"<<function<<") Correction L2Relative}"<<endl;
                head_printed = true;
            }

            double  etamin  = hl_jetpt.minimum(0,ieta);
            double  etamax  = hl_jetpt.maximum(0,ieta);

            //For eta-dependent spline clipping
            if      ( (etamax>0 && etamax < 0.6091) || (etamin<0 && etamax > -0.6091) ) pt_limit = 3000;
            else if ( (etamax>0 && etamax < 0.9571) || (etamin<0 && etamax > -0.9571) ) pt_limit = 2700;
            else if ( (etamax>0 && etamax < 1.3051) || (etamin<0 && etamax > -1.3051) ) pt_limit = 2000;
            else if ( (etamax>0 && etamax < 2.0431) || (etamin<0 && etamax > -2.0431) ) pt_limit = 1400;
            else if ( (etamax>0 && etamax < 2.51  ) || (etamin<0 && etamax > -2.51  ) ) pt_limit = 900;
            else if ( (etamax>0 && etamax < 2.9641) || (etamin<0 && etamax > -2.9641) ) pt_limit = 500;
            else if ( (etamax>0 && etamax < 3.6641) || (etamin<0 && etamax > -3.6641) ) pt_limit = 300;
            else if ( (etamax>0 && etamax < 4.0131) || (etamin<0 && etamax > -4.0131) ) pt_limit = 200;
            else if ( (etamax>0 && etamax < 4.5381) || (etamin<0 && etamax > -4.5381) ) pt_limit = 100;

            bool abovePtLimit = false;
            bool lastLine = false;

            for(int isection=0; isection<spline->getNSections(); isection++) {
                if(lastLine) continue;

                //Could put section checking for a jump in the JEC
                //|splineDerivativ_{i}-SplineDerivative_{i-1}|>derivative_threshold
                //sign(splineDerivative_{i})!=sign(splineDerivative_{i+1})
                //Then cut out this and all further spline segments
                //Might want to check this for the upcoming segment (i=j+1 where j is the current segment)
                //  because then we can set the last bound to 6500

                pair<double,double> bounds = spline->getSectionBounds(isection);
                //When you go beyond a range of validity the default behavior is to return to the correction value at the closest bound to the range of validity
                //When you go outside a bin boundary (i.e. the program cannot find the bin you are supposed to be in) then the default behavior is to return 1.0
                //This will protext against that happening when the pT is just above where the last MC bin is.
                //6500 is chosen ass that is the 2015-2017 beam energy.
                //if(isection==spline->getNSections()-1) {
                //    bounds.second = 6500;
                //}

                if(isection==spline->getNSections()-1) lastLine = true;
                if(bounds.second >= pt_limit) {
                    abovePtLimit = true;
                    lastLine = true;
                }

                //For expediency of Summer16_25nsV5_MC do eta-dependent clipping
                fout<<setw(8) <<etamin<<setw(8)<<etamax
                    <<setw(10)<<setprecision(6)<<(isection ? bounds.first : 0.001)
                    <<setw(10)<<setprecision(6)<<(lastLine ? 6500 : bounds.second)
                    <<setw(6)<<(int)(spline->getNpar()+2) //Number of parameters + 2
                    <<setw(12)<<setprecision(8)<<bounds.first
                    <<setw(12)<<setprecision(8)<<(abovePtLimit ? pt_limit : bounds.second);
                TF1* spline_func = spline->setParameters(isection);
                for(int p=0; p<spline->getNpar(); p++) {
                   fout<<setw(17)<<setprecision(10)<<spline_func->GetParameter(p);
                }
                fout<<endl;
            }
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

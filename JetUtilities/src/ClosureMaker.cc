// This class libraries
#include "JetMETAnalysis/JetUtilities/interface/ClosureMaker.hh"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// implement ClosureMaker class
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
ClosureMaker::ClosureMaker() {
    cout << "WARNING::ClosureMaker() Default constructor not recommended." << endl;
    flavor          = "";
    path            = "";
    filename        = "Closure";
    histMet         = "mu_h";
    histogramMetric = HistUtil::getHistogramMetricType(string(histMet));
    nsigma          = 1.5;
    outputDir       = "./";
    outputFilename  = "";
    outputFormat    = {".png",".eps",".pdf"};
    CMEnergy        = 13000;
    draw_guidelines = true;

    //
    // Check that the input path exists
    // Also check that the input path has a slash at the end of it
    // Otherwise we'd have to handle this on a case by case basis
    //
    if(path.IsNull()) path = string (gSystem->pwd())+"/";
    if(!path.EndsWith("/")) path+="/";

    //
    // Check that the output path ends with a slash and that the path exists
    //
    if(!outputDir.EndsWith("/")) outputDir+="/";
    if(!gSystem->OpenDirectory(outputDir)) gSystem->mkdir(outputDir);

    objects_loaded = false;
}

//______________________________________________________________________________
ClosureMaker::ClosureMaker(CommandLine& cl) {
    //
    // evaluate command-line / configuration file options
    //
    algs            = cl.getVector<TString> ("algs",                    "");
    flavor          = cl.getValue<TString>  ("flavor",                  "");
    path            = cl.getValue<TString>  ("path",                    "");
    filename        = cl.getValue<TString>  ("filename",         "Closure");
    nsigma          = cl.getValue<double>   ("nsigma",                 1.5);
    draw_guidelines = cl.getValue<bool>     ("draw_guidelines",       true);
    outputDir       = cl.getValue<TString>  ("outputDir",             "./");
    outputFilename  = cl.getValue<TString>  ("outputFilename",          "");
    outputFormat    = cl.getVector<TString> ("outputFormat", ".png:::.eps");
    CMEnergy        = cl.getValue<double>   ("CMEnergy",             13000);
    histMet         = cl.getValue<TString>  ("histMet",             "mu_h");
    histogramMetric = HistUtil::getHistogramMetricType(string(histMet));
    bool help       = cl.getValue<bool>     ("help",                 false);

    if (help) {cl.print(); return;}
    if (!cl.partialCheck()) return;
    cl.print();

    //
    // Check that the input path exists
    // Also check that the input path has a slash at the end of it
    // Otherwise we'd have to handle this on a case by case basis
    //
    if(path.IsNull()) path = string (gSystem->pwd())+"/";
    if(!path.EndsWith("/")) path+="/";

    //
    // Check that the output path ends with a slash and that the path exists
    //
    if(!outputDir.EndsWith("/")) outputDir+="/";
    if(!gSystem->OpenDirectory(outputDir)) gSystem->mkdir(outputDir);

    objects_loaded = false;
}

//______________________________________________________________________________
void ClosureMaker::openInputFile() {
    //
    // Open the file containing the original, unformated closure plots
    //
    if(!flavor.IsNull())
        ifile = TFile::Open(path+filename+"_"+flavor+".root","READ");
    else
        ifile = TFile::Open(path+filename+".root","READ");
    
    if(ifile == 0) {
        cout << "ERROR::ClosureMaker::openInputFiles() Could not open the file " << path << endl;
        std::terminate();
    }
}

//______________________________________________________________________________
void ClosureMaker::getHistograms(TDirectoryFile* idir) {
    if(objects_loaded) {
        hl.reset();
    }

    if(var == VARIABLES::refpt || var == VARIABLES::ptclpt) {
        hl.load_objects(idir,"RelRspVsRefPt:JetEta"); 
        objects_loaded = true;
        if(hl.nobjects()!=NDetectorNames) {
            cout << "One or more of the histogram pointers from file " << path << " is NULL." << endl;
        }
    }
    else if(var == VARIABLES::jteta) {
        hl.load_objects(idir,"RelRspVsJetEta:RefPt");
        objects_loaded = true;
        if(hl.nobjects()!=NPtBins) {
            cout << "One or more of the histogram pointers from file " << path << " is NULL." << endl;
        }
    }
    else {
        cout << "ERROR RatioMaker::getHistograms Unknown variable." << endl;
        std::terminate();
    }
}

//______________________________________________________________________________
void ClosureMaker::openOutputFile() {
    //
    // Open/create the output directory and file
    //   
    TString ofname = Form("%s/ClosureVs%s.root",outputDir.Data(),getVariableTitleString(var).c_str());
    if(!flavor.IsNull()) ofname = Form("%s/ClosureVs%s_%s.root",outputDir.Data(),
                                       getVariableTitleString(var).c_str(),flavor.Data());
    if(!outputFilename.IsNull()) ofname = Form("%s/%s",outputDir.Data(),outputFilename.Data());
    ofile = TFile::Open(ofname,"RECREATE");
}

//______________________________________________________________________________
void ClosureMaker::closeFiles() {
    ofile->Close();
    ifile->Close();
}

//______________________________________________________________________________
void ClosureMaker::makeLines() {
    pair<int,int> min_max;
    //if(var.EqualTo("pt",TString::kIgnoreCase)) {
    if(var == VARIABLES::refpt || var == VARIABLES::jtpt || var == VARIABLES::ptclpt) {
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
    line->SetLineWidth(2);
    line->SetLineStyle(2);
    linePlus = new TF1("linePlus","0*x+1.01",min_max.first,min_max.second);
    linePlus->SetLineColor(1);
    linePlus->SetLineWidth(2);
    linePlus->SetLineStyle(3);
    lineMinus = new TF1("lineMinus","0*x+0.99",min_max.first,min_max.second);
    lineMinus->SetLineColor(1);
    lineMinus->SetLineWidth(2);
    lineMinus->SetLineStyle(3);
}

//______________________________________________________________________________
void ClosureMaker::loopOverDirectories() {
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
void ClosureMaker::resetForNextAlgorithm() {
    if(ji!=0)
        delete ji;
    if(objects_loaded) {
        hl.reset();
        objects_loaded = false;
    }
    if(line!=0)
        delete line;
    if(linePlus!=0)
        delete linePlus;
    if(lineMinus!=0)
        delete lineMinus;
    h.erase(h.begin(),h.end());
    func.erase(func.begin(),func.end());
    hClosure.erase(hClosure.begin(),hClosure.end());
    canvases_legends.erase(canvases_legends.begin(),canvases_legends.end());
    pave.erase(pave.begin(),pave.end());
}

//______________________________________________________________________________
void ClosureMaker::loopOverAlgorithms() {
    TIter nextDir(ifile->GetListOfKeys());
    TKey* dirKey(0);
    int algIndex(-1);
    while ((dirKey=(TKey*)nextDir())) {
        if (strcmp(dirKey->GetClassName(),"TDirectoryFile")!=0) continue;
        TDirectoryFile* idir = (TDirectoryFile*)dirKey->ReadObj();
        alg = idir->GetName(); if (!JetInfo::contains(algs,alg)) continue;
        
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
        // Make output directory
        //
        odir = (TDirectoryFile*)ofile->mkdir(alg.Data());
        odir->cd();

        //
        // Load the input histograms from input file
        //
        getHistograms(idir);

        //
        // Loop over the TH2 histograms and find the mean/median/mpv response for each bin
        //
        loopOverHistograms();
        if(var == VARIABLES::refeta || var == VARIABLES::jteta) {
            etaClosureMergedPtBins(idir);
        }

        //
        // Perform basic sanity checks of max and min responses
        //
        checkResponse();

        //
        // Make the guide lines
        //
        makeLines();

        //
        // Formats the final canvases
        //
        makeCanvases();
        if(var == VARIABLES::refpt || var == VARIABLES::jtpt || var == VARIABLES::ptclpt) {
            makeMergedCanvas();
        }

        writeToFile();

        cout<<alg<<" is DONE."<<endl;
    }
}

//______________________________________________________________________________
void ClosureMaker::loopOverHistograms() {
    vector<unsigned int> indices;
    TH2F* hvar(0);
    hl.begin_loop();
    while ((hvar=hl.next_object(indices))) {

        unsigned int ibin=indices[0];

        //
        // Create the output histogram
        //
        TString name;
        if(var == VARIABLES::refpt || var == VARIABLES::ptclpt) {
            name = Form("ClosureVsRefPt_JetEta%gto%g",hl.minimum(0,ibin),hl.maximum(0,ibin));
            hClosure.push_back(new TH1F(name,name,NPtBins,vpt));
        }
        else if(var == VARIABLES::jteta) {
            name = Form("ClosureVsJetEta_RefPt%gto%g",hl.minimum(hl.nvariables()-1,indices.back()),hl.maximum(hl.nvariables()-1,indices.back()));
            hClosure.push_back(new TH1F(name,name,NETA,veta));
        }

        loopOverBins(hvar, ibin);
    }
}

//______________________________________________________________________________
void ClosureMaker::etaClosureMergedPtBins(TDirectoryFile* idir) {
    //
    // book histogram for total pt range
    //
    TString name;
    TH3F* RelRspVsJetEtaVsRefPt = ((TH3F*)idir->Get("RespVsEtaVsPt"));
    if(RelRspVsJetEtaVsRefPt == 0) {
        cout << "ERROR ClosureMaker::etaClosureMergedPtBins The RelRspVsJetEtaVsRefPt histogram with name (RespVsEtaVsPt) cannot be found." << endl;
        std::terminate();
    }

    if(alg.Contains("calo",TString::kIgnoreCase) || JetInfo::contains_loose(algs,TString("calo"))) {
      RelRspVsJetEtaVsRefPt->GetXaxis()->SetRangeUser(30,vpt[NPtBins]);
      name = Form("ClosureVsJetEta_MultiBin_RefPt%sto%s","30",Pt[NPtBins]);
    }
    else {
      name = Form("ClosureVsJetEta_MultiBin_RefPt%sto%s",Pt[0],Pt[NPtBins]);
    }
    TH2F* RelRspVsJetEta = (TH2F*)(RelRspVsJetEtaVsRefPt->Project3D("zyoe"));
    hClosure.push_back(new TH1F(name,name,NETA,veta));

    loopOverBins(RelRspVsJetEta, NETA+1);
}

//______________________________________________________________________________
void ClosureMaker::loopOverBins(TH2F* hvar, unsigned int iVarBin) {

    TString name;
    string const_bin, hvar_name(hvar->GetName());
    vector<string> varBins;
    unsigned int nbins = 0;

    if(var == VARIABLES::refpt || var == VARIABLES::jtpt || var == VARIABLES::ptclpt) {
        nbins = NPtBins;
        varBins.insert(varBins.end(), &Pt[0], &Pt[NPtBins+1]);
    }
    else if(var == VARIABLES::refeta || var == VARIABLES::jteta) {
        nbins = NETA;
        varBins.insert(varBins.end(), &eta_boundaries[0], &eta_boundaries[NETA+1]);
    }

    const_bin = hvar_name.substr(hvar_name.find("_")+1);

    for(unsigned int ibin=0; ibin<nbins; ibin++) {
        name = Form("Response_%s_%d_%s%sto%s",const_bin.c_str(),
                    ibin,getVariableTitleString(var).c_str(),
                    varBins[ibin].c_str(),varBins[ibin+1].c_str());
        h.push_back(hvar->ProjectionY(name,ibin+1,ibin+1,"e"));

        if (h.back()->GetEntries()>4) {
            if(histogramMetric==HistUtil::mu_f || histogramMetric==HistUtil::mpv) {
                int nbins = 50;//100;
                TSpectrum *spec = new TSpectrum(10);
                if(nbins < 100) spec->Search(h.back(),6,"nobackground nodraw goff"); //turn off background removal when nbins too small
                else spec->Search(h.back(),6,"nodraw goff");
                Double_t* xpos = spec->GetPositionX();
                Double_t p = xpos[0];
                double fitrange_min = p-nsigma*h.back()->GetRMS();
                double fitrange_max = p+nsigma*h.back()->GetRMS();

                name = Form("FitResponse_%s_%d_%s%sto%s",const_bin.c_str(),
                            ibin,getVariableTitleString(var).c_str(),
                            varBins[ibin].c_str(),varBins[ibin+1].c_str());
                //func.push_back(new TF1(name,"gaus",h.back()->GetMean()-1.5*h.back()->GetRMS(),
                //                 h.back()->GetMean()+1.5*h.back()->GetRMS()));
                adjust_fitrange(h.back(),fitrange_min,fitrange_max);
                if(TString(alg).Contains("calo",TString::kIgnoreCase) && ibin<20) {
                   fitrange_min = p;
                   fitrange_max += 0.1;
                }
                func.push_back(new TF1(name,"gaus",fitrange_min,fitrange_max));
                func.back()->SetLineColor(4);
                func.back()->SetLineWidth(2);
                func.back()->SetParNames("N","#mu","#sigma");
                func.back()->SetParameters(h.back()->GetMaximumStored(),p,h.back()->GetRMS());
                h.back()->Fit(func.back(),"RQ");

                hClosure.back()->SetBinContent(ibin+1,func.back()->GetParameter(1));
                hClosure.back()->SetBinError(ibin+1,func.back()->GetParError(1));

                delete spec;
            }
            else if(histogramMetric==HistUtil::mu_h || histogramMetric==HistUtil::median) {
                hClosure.back()->SetBinContent(ibin+1,getHistogramMetric1D(histogramMetric,h.back()).first);
                hClosure.back()->SetBinError(ibin+1,getHistogramMetric1D(histogramMetric,h.back()).second);
            }
            else{
                cout << "ERROR::ClosureMaker::loopOverBins Unknown histogramMetric. Skipping bin " << ibin << "." << endl;
                continue;
            }
        }
        else if(h.back()->GetEntries()<=4 && h.back()->GetEntries()>1) {
            hClosure.back()->SetBinContent(ibin+1,h.back()->GetMean());
            hClosure.back()->SetBinError(ibin+1,h.back()->GetMeanError());
        }
        else {
            continue;
        }
    }
}

//______________________________________________________________________________
void ClosureMaker::adjust_fitrange(TH1* h,double& min,double& max) {
   int imin=1; while (h->GetBinLowEdge(imin)<min) imin++;
   int imax=1; while (h->GetBinLowEdge(imax)<max) imax++;
   while ((imax-imin)<8) {
      if (imin>1) {imin--; min = h->GetBinCenter(imin); }
      if (imax<h->GetNbinsX()-1) { imax++; max=h->GetBinCenter(imax); }
   }
}

//______________________________________________________________________________
void ClosureMaker::checkResponse() {
    for(unsigned int ih=0; ih<hClosure.size();ih++) {
        for(int ibin=1; ibin<=hClosure[ih]->GetNbinsX(); ibin++) {
            double binCont = hClosure[ih]->GetBinContent(ibin);
            double binErr = hClosure[ih]->GetBinError(ibin);
            if (binCont > 1.10){
                cout << "\tWARNING Closure for " << hClosure[ih]->GetName() << " at "
                     << hClosure[ih]->GetBinLowEdge(ibin) << " < "
                     << getVariableTitleString(var) << " < " << hClosure[ih]->GetBinLowEdge(ibin+1)
                     <<" has relresp as high as " << binCont << " +/- " << binErr << endl;
            }
            if (binCont > 0 && binCont < 0.9){
                cout << "\tWARNING Closure for " << hClosure[ih]->GetName() << " at "
                     << hClosure[ih]->GetBinLowEdge(ibin) << " < "
                     << getVariableTitleString(var) << " < " << hClosure[ih]->GetBinLowEdge(ibin+1)
                     <<" has relresp as low as " << binCont << " +/- " << binErr << endl;         
            }
        }
    }
}

//______________________________________________________________________________
pair<double,double> ClosureMaker::determineCanvasRange(double xmin, double xmax) {
  bool above105 = false, below95 = false;
  bool above115 = false, below85 = false;
  for(unsigned int ih=0; ih<hClosure.size(); ih++) {
    for(int ibin=hClosure[ih]->FindBin(xmin); ibin<hClosure[ih]->FindBin(xmax)+1; ibin++) {
      if(hClosure[ih]->GetBinContent(ibin)>1.05) above105 = true;
      if(hClosure[ih]->GetBinContent(ibin)<0.95 &&
         hClosure[ih]->GetBinContent(ibin)!=0.0) below95 = true;
      if(hClosure[ih]->GetBinContent(ibin)>1.15) above115 = true;
      if(hClosure[ih]->GetBinContent(ibin)<0.85 &&
         hClosure[ih]->GetBinContent(ibin)!=0.0) below85 = true;
    }
  }
  if(above115 || below85)      return make_pair(0.35,1.35);
  else if(above105 || below95) return make_pair(0.85,1.15);
  else                         return make_pair(0.95,1.05);
}

//______________________________________________________________________________
void ClosureMaker::makeCanvases() {
    for(unsigned int ih=0; ih<hClosure.size(); ih++) {
        TString name = Form("%s_%s",hClosure[ih]->GetName(),alg.Data());
        if(!flavor.IsNull()) name+="_"+flavor;

        //
        // Setup the frame, canvas, and legend
        //
        TH1D* frame = new TH1D();
        if(var == VARIABLES::refpt || var == VARIABLES::jtpt || var == VARIABLES::ptclpt) {
            if(TString(alg).Contains("pf",TString::kIgnoreCase) ||
         TString(alg).Contains("puppi",TString::kIgnoreCase)) {
                frame->GetXaxis()->SetLimits(XminPF[ih],Xmax[ih]);
                hClosure[ih]->GetXaxis()->SetLimits(XminPF[ih],Xmax[ih]);
            }
            else {
                frame->GetXaxis()->SetLimits(XminCalo[ih],Xmax[ih]);
                hClosure[ih]->GetXaxis()->SetLimits(XminCalo[ih],Xmax[ih]);
            }
        }
        else if(var == VARIABLES::refeta || var == VARIABLES::jteta) {
            float etaMax = min(4.7,TMath::ACosH(CMEnergy/2.0/vpt[ih]));
            if(ih==NPtBins) {
                float minEta = max(-4.7,veta[0]);
                float maxEta = min(4.7,veta[NETA]);
                frame->GetXaxis()->SetLimits(minEta,maxEta);
                hClosure[ih]->GetXaxis()->SetLimits(minEta,maxEta);
            }
            else {
                frame->GetXaxis()->SetLimits(-etaMax,etaMax);
                hClosure[ih]->GetXaxis()->SetLimits(-etaMax,etaMax);
            }
        }
        frame->GetXaxis()->SetMoreLogLabels();
        frame->GetXaxis()->SetNoExponent();
        frame->GetYaxis()->SetRangeUser(0.95,1.05);
        //frame->GetYaxis()->SetRangeUser(0.35,1.35);
        //frame->GetYaxis()->SetRangeUser(0.00,1.3);
        frame->GetXaxis()->SetTitle(getVariableAxisTitleString(var).c_str());
        frame->GetYaxis()->SetTitle("Response");
        canvases_legends.push_back(make_pair(tdrCanvas(name,frame,14,11,true),
                                             tdrLeg(0.58,0.16,0.9,0.4)));
        if((var == VARIABLES::refpt || var == VARIABLES::jtpt || var == VARIABLES::ptclpt) && ih<3)
            canvases_legends.back().first->GetPad(0)->SetLogx();
        
        //
        // Format and draw the pave
        //
        pave.push_back(tdrText(0.5,0.75,0.93,1-gPad->GetTopMargin()-0.045*(1-gPad->GetTopMargin()-gPad->GetBottomMargin()),31));
        pave.back()->AddText("QCD Monte Carlo");
        pave.back()->AddText(JetInfo::get_legend_title(string(alg)).c_str());
        if(var == VARIABLES::refpt || var == VARIABLES::jtpt || var == VARIABLES::ptclpt) {
            pave.back()->AddText(detector_regions_eta[ih]);
        }
        else if(var == VARIABLES::refeta || var == VARIABLES::jteta) {
            if(ih==NPtBins) {
                if(alg.Contains("calo",TString::kIgnoreCase) || JetInfo::contains_loose(algs,TString("calo"))) {
                    pave.back()->AddText("30 GeV < p_{T}^{ptcl} < "+TString(Pt[NPtBins])+" GeV");
                }
                else {
                    pave.back()->AddText(TString(Pt[0])+" GeV < p_{T}^{ptcl} < "+TString(Pt[NPtBins])+" GeV");
                }
            }
            else
                pave.back()->AddText(TString(Pt[ih])+" GeV < p_{T}^{ptcl} < "+TString(Pt[ih+1])+" GeV");
        }
        pave.back()->Draw("same");

        //
        // Draw the guide lines
        //
        line->Draw("same");
        if(draw_guidelines) {
            linePlus->Draw("same");
            lineMinus->Draw("same");
        }

        //
        // Format and draw the histogram
        //
        tdrDraw(hClosure[ih],"EP",kFullCircle,kBlue,kSolid,kBlue);
    }

}

//______________________________________________________________________________
void ClosureMaker::makeMergedCanvas() {
    TString name = Form("ClosureVs%s_Overview_%s",getVariableTitleString(var).c_str(),alg.Data());
    if(!flavor.IsNull()) name+="_"+flavor;

    //
    // Setup the frame, canvas, legend, and pave
    //
    TH1D* frame = new TH1D();
    if(TString(alg).Contains("pf",TString::kIgnoreCase) ||
     TString(alg).Contains("puppi",TString::kIgnoreCase))
        frame->GetXaxis()->SetLimits(XminPF[0],Xmax[0]);
    else
        frame->GetXaxis()->SetLimits(XminCalo[0],Xmax[0]);
    frame->GetXaxis()->SetMoreLogLabels();
    frame->GetXaxis()->SetNoExponent();
  pair<double,double> range = determineCanvasRange(frame->GetXaxis()->GetXmin(),frame->GetXaxis()->GetXmax());
    //frame->GetYaxis()->SetRangeUser(0.95,1.05);
    //frame->GetYaxis()->SetRangeUser(0.35,1.35);
  frame->GetYaxis()->SetRangeUser(range.first,range.second);
    frame->GetXaxis()->SetTitle(getVariableAxisTitleString(var).c_str());
    frame->GetYaxis()->SetTitle("Response");
    canvases_legends.push_back(make_pair(tdrCanvas(name,frame,14,11,true),
                                         tdrLeg(0.58,0.16,0.9,0.4)));
    canvases_legends.back().first->GetPad(0)->SetLogx();
    pave.push_back(tdrText(0.5,0.75,0.93,1-gPad->GetTopMargin()-0.045*(1-gPad->GetTopMargin()-gPad->GetBottomMargin()),31));

    //
    // Draw the guide lines
    //
    line->Draw("same");
    if(draw_guidelines) {
        linePlus->Draw("same");
        lineMinus->Draw("same");
    }

    //
    // Format and draw the histograms
    //
    for(unsigned int ih=0; ih<hClosure.size(); ih++) {
        if(TString(alg).Contains("pf",TString::kIgnoreCase) ||
       TString(alg).Contains("puppi",TString::kIgnoreCase)) {
            hClosure[ih]->GetXaxis()->SetLimits(XminPF[ih],Xmax[ih]);
            hClosure[ih]->GetXaxis()->SetRangeUser(XminPF[ih],Xmax[ih]);
        }
        else {
            hClosure[ih]->GetXaxis()->SetLimits(XminCalo[ih],Xmax[ih]);
            hClosure[ih]->GetXaxis()->SetRangeUser(XminCalo[ih],Xmax[ih]);
        }
        hClosure[ih]->SetStats(kFALSE);
        tdrDraw(hClosure[ih],"EP",closureShapes[ih],
                closureColors[ih],kSolid,closureColors[ih]);
        canvases_legends.back().second->AddEntry(hClosure[ih],detector_regions_eta[ih],"lep");
    }

    //
    // Format the pave and draw
    //
    pave.back()->AddText("QCD Monte Carlo");
    pave.back()->AddText(JetInfo::get_legend_title(string(alg)).c_str());
    pave.back()->Draw("same");

    //
    // Draw the legend
    //
    canvases_legends.back().second->Draw("same");
}

//______________________________________________________________________________
void ClosureMaker::writeToFile() {
    odir->cd();
    TDirectory* hdir = odir->mkdir("RelRspHistograms");
    hdir->cd();
    for(auto it : h) {
       it->Write();
    }
    odir->cd();
    for(auto it : hClosure) {
        it->Write();
    }
    for(auto it : canvases_legends) {
        it.first->Write();
        for(unsigned int iformat=0; iformat<outputFormat.size(); iformat++) {
            it.first->SaveAs(outputDir+it.first->GetName()+outputFormat[iformat]);
        }
    }
}


//______________________________________________________________________________
void ClosureMaker::makeClosure(const VARIABLES::Variable ivar) {
    //
    // Store the variable choice for the other functions to use
    // This is simply so that we don't have to pass around this variable to every function
    //
    var = ivar;

    openInputFile();
    openOutputFile();
    loopOverDirectories();
    loopOverAlgorithms();
    closeFiles();
}



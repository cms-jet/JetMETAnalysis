///////////////////////////////////////////////////////////////////
//
// jet_draw_corrections_x
// ----------------------
//
//            09/01/2011 Ricardo Eusebi eusebi@tamu.edu
//
// This executable reads the relevant txt files and validates the 
// output that is obtained when using through FactorizedJetCorrector
// It will complain strongly on the presence of nan's or inf's or 
// corrections factors outside the range (0.8,3.0) and
// it will produce validation plots as well.
///////////////////////////////////////////////////////////////////

#include "JetMETAnalysis/JetAnalyzers/interface/Style.h"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/JetInfo.hh"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"

#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TObject.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TF1.h"
#include "TString.h"
#include "TPaveText.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TMath.h"
#include "TColor.h"

#include <fstream>
#include <string>
#include <cmath>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// local functions
////////////////////////////////////////////////////////////////////////////////
FactorizedJetCorrector * getFactorizedCorrector(TString algo, CommandLine & cl, TString & );

void analyzeAlgo(TString algo, CommandLine & cl);

void analyzeAllAlgs(vector<TString>& algs, vector<pair<FactorizedJetCorrector*,TString> >& allJetCorrs, CommandLine & cl);

TCanvas * getCorrectionVsEtaCanvas(TString algo, FactorizedJetCorrector * jetCorr, TString suffix);

TCanvas * getCorrectionVsEtaCanvasTDR(TString algo, FactorizedJetCorrector * jetCorr, TString suffix);

vector<TCanvas*> getCorrectionVsEtaComparisonCanvasTDR(vector<TString>& algs, vector<pair<FactorizedJetCorrector*,TString> > allJetCorrs,
                                                       TString suffix, TString normAlg = "");

TCanvas * getCorrectionVsPtCanvas(TString algo, FactorizedJetCorrector * jetCorr, TString suffix);

TCanvas * getCorrectionVsPtComparisonCanvasTDR(vector<TString>& algs, vector<pair<FactorizedJetCorrector*,TString> > allJetCorrs,
                                               TString suffix);

vector<Int_t> getColors();

vector<Int_t> getMarkerNumbers();

string getAlias(TString s);

TString getAlgNameLong(TString algo, int coneSize = 0);

///CMS Preliminary label;
void cmsPrelim(double intLUMI = 0);

////////////////////////////////////////////////////////////////////////////////
// global variables
////////////////////////////////////////////////////////////////////////////////
vector<pair<FactorizedJetCorrector*,TString> > allJetCorrs;
double fixedRho;

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////
//______________________________________________________________________________
int main(int argc,char**argv)
{
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);

  gSystem->Load("libFWCoreFWLite.so");

  // evaluate command-line / configuration file options
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;

  vector<TString> algs         = cl.getVector<TString> ("algs");
  bool            combineAlgs  = cl.getValue<bool>     ("combineAlgs", false);
  TString         path         = cl.getValue<TString>  ("path",           "");
  string          outputDir    = cl.getValue<string>   ("outputDir",    "./");
  if (algs.size()==0){
    cout<<"ERROR you must use the flag -algs to select one or more algorithms!"<<endl;
  }

  allJetCorrs.clear();

  // Loop over the algorithms 
  for(unsigned int a=0; a<algs.size(); a++){
    
    // create the output file
    if(outputDir.length() > 0 && outputDir[outputDir.length()-1] != '/') outputDir += "/";
    TString outf_str = outputDir+"Corrections_Overview_"+algs[a]+".root" ;
    TFile *outf = new TFile(outf_str,"RECREATE");
    if (!outf) {
      cout<<" Output file="<<outf_str<<" could not be created"<<endl;
      exit(0);
    }
    
    // analyze that algorithm.
    analyzeAlgo(algs[a], cl);
    
    // write and close the output file
    outf->cd();
    outf->Write();
    outf->Close();
    
  }// for algos

  // If we need to combine the algorithms into one canvas
  if (combineAlgs) {
    analyzeAllAlgs(algs, allJetCorrs, cl);
  }
  
}//main


//---------------------------------------------------------------------
// Do here all the analysis for the given algorithm
void analyzeAlgo(TString algo, CommandLine & cl){

  // Report
  cout<<"Doing algorithm "<<algo<<endl;

  // get the factorized corrector according to the command line options
  TString corrLabel = "";
  FactorizedJetCorrector * jetCorr = getFactorizedCorrector(algo, cl, corrLabel);
  if (!jetCorr){
    cout<<"ERROR FactorizedJetCorrector could not be defined. Skipping algo."<<endl;
    return;
  }

  // see if flavor separation is required
  bool            combineAlgs  = cl.getValue<bool>     ("combineAlgs",          false);
  TString         flavor       = cl.getValue<TString>  ("flavor",                  "");
  string          outputDir    = cl.getValue<string>   ("outputDir",         "images");
  vector<TString> outputFormat = cl.getVector<TString> ("outputFormat", ".png:::.eps");
  bool            tdr          = cl.getValue<bool>     ("tdr",                   true);

  if(combineAlgs) {
    allJetCorrs.push_back(make_pair(jetCorr,corrLabel));
  }

  if (tdr) {
     setTDRStyle();
  }

  if(outputDir.length() > 0 && outputDir[outputDir.length()-1] != '/') outputDir += "/";

  //Create the suffix for the names
  TString suffix;
  suffix += "_" + algo;
  if( !flavor.IsNull() ) suffix += "_" + flavor;
  suffix += "_" + corrLabel;

  // get the canvas of correction vs eta, write and save to file
  TCanvas * ove = getCorrectionVsEtaCanvas(algo, jetCorr, suffix);
  for(unsigned int of=0; of<outputFormat.size(); of++) {
     ove->SaveAs(outputDir+string(ove->GetName())+outputFormat[of]);
  }
  ove->Write();

  // get the canvas of correction vs eta in tdr format, write and save to file
  if(tdr) {
     TCanvas * ovetdr = getCorrectionVsEtaCanvasTDR(algo, jetCorr, suffix);
     for(unsigned int of=0; of<outputFormat.size(); of++) {
        ovetdr->SaveAs(outputDir+string(ovetdr->GetName())+outputFormat[of]);
     }
     ovetdr->Write();     
  }

  // get the canvas of correction vs pt, write and save to file
  TCanvas * ovp = getCorrectionVsPtCanvas(algo, jetCorr, suffix);
  for(unsigned int of=0; of<outputFormat.size(); of++) {
     ovp->SaveAs(outputDir+string(ovp->GetName())+outputFormat[of]);
  }
  ovp->Write();
  

}//analyzeAlgo

//---------------------------------------------------------------------
void analyzeAllAlgs(vector<TString>& algs, vector<pair<FactorizedJetCorrector*,TString> >& allJetCorrs, CommandLine & cl) {
  // see if flavor separation is required
  TString         flavor       = cl.getValue<TString>  ("flavor",                  "");
  string          outputDir    = cl.getValue<string>   ("outputDir",         "images");
  vector<TString> outputFormat = cl.getVector<TString> ("outputFormat", ".png:::.eps");
  bool            tdr          = cl.getValue<bool>     ("tdr",                   true);
  TString         normAlg      = cl.getValue<TString>  ("normAlg",                 "");

  if (tdr) {
     setTDRStyle();
  }

  if(algs.size()!=allJetCorrs.size()) {
    cout << "ERROR::analyzeAllAlgs::The number of algs and the number of FactorizedJetCorrectors don't match."<<endl;
    return;
  }

  TString allAlgs;
  for(unsigned int ialg=0; ialg<algs.size(); ialg++) {
    allAlgs+=algs[ialg];
    if(ialg<algs.size()-1)
      allAlgs+="_";
  }

  // create the output file
  if(outputDir.length() > 0 && outputDir[outputDir.length()-1] != '/') outputDir += "/";
  TString outf_str = outputDir+"Corrections_Comparison_"+allAlgs+".root" ;
  TFile *outf = new TFile(outf_str,"RECREATE");
  if (!outf) {
    cout<<" Output file="<<outf_str<<" could not be created"<<endl;
    exit(0);
  }

  //Create the suffix for the names
  TString suffix;
  suffix += "_" + allAlgs;
  if( !flavor.IsNull() ) suffix += "_" + flavor;
  suffix += "_" + allJetCorrs[0].second;

  // get the canvas of correction vs eta in tdr format, write and save to file
  if(tdr) {
     vector<TCanvas*> ovetdr = getCorrectionVsEtaComparisonCanvasTDR(algs, allJetCorrs, suffix, normAlg);
     for(unsigned int c=0; c<ovetdr.size(); c++) {
       for(unsigned int of=0; of<outputFormat.size(); of++) {
          ovetdr[c]->SaveAs(outputDir+string(ovetdr[c]->GetName())+outputFormat[of]);
       }
       ovetdr[c]->Write();
     }

     TCanvas * ovptdr = getCorrectionVsPtComparisonCanvasTDR(algs, allJetCorrs, suffix);
     for(unsigned int of=0; of<outputFormat.size(); of++) {
        ovptdr->SaveAs(outputDir+string(ovptdr->GetName())+outputFormat[of]);
     }
     ovptdr->Write();
  }

  // write and close the output file
  outf->cd();
  outf->Write();
  outf->Close();

}//analyzeAllAlgs

//---------------------------------------------------------------------
TCanvas * getCorrectionVsEtaCanvas(TString algo, FactorizedJetCorrector * jetCorr, TString suffix) {

 //Create canvas vs eta for different pts  
  vector<double> PtVals;
  PtVals.push_back(0.00001);
  PtVals.push_back(0.1);
  PtVals.push_back(1);
  PtVals.push_back(5);
  PtVals.push_back(7);
  PtVals.push_back(10);
  PtVals.push_back(20);
  PtVals.push_back(30);
  PtVals.push_back(40);
  PtVals.push_back(50);
  PtVals.push_back(150);
  PtVals.push_back(250);
  PtVals.push_back(500);
  PtVals.push_back(1000);
  PtVals.push_back(1500);
  PtVals.push_back(2500);

  //Create the canvas with multiple pads
  TString ss("CorrectionVsEta_Overview");
  ss += suffix;
  TCanvas *ove = new TCanvas(ss,ss,1200,800);
  ove->Divide(4,4);

  // loop over all pads
  for (int c = 0; c < ove->GetListOfPrimitives()->GetSize(); c++) {

    // just make we don't write the extra pads 
    if (c <= (int) PtVals.size()){

      //Create and fill the histo
      TString hstr; hstr.Form("EtaSF_%d",c);
      TH1F * cc = new TH1F(hstr,hstr,NETA,veta);
      for (int b = 1; b <= cc->GetNbinsX(); b++){
	jetCorr->setJetPt(PtVals[c]);
	jetCorr->setJetEta(cc->GetBinCenter(b));
    jetCorr->setRho(fixedRho);
    jetCorr->setJetA(TMath::Pi()*TMath::Power(JetInfo(algo).coneSize/10.0,2));
	double cor = jetCorr->getCorrection();
	if (std::isnan((double)cor) || std::isinf((double)cor) ){
	  cout<<" *** ERROR *** getCorrectionVsEtaCanvas(). For eta="<<cc->GetBinCenter(b)
	      <<" and pt="<<PtVals[c]<<" the correction is "<<cor<<"!!"<<endl;
	  cor = 10000;
	}
	if ( cor < 0.8  || cor > 3 ){
	  cout<<" WARNING  *** getCorrectionVsEtaCanvas(). Correction of "<<cor<<" is out of the (0.8,3) range"<<endl;
	}

	cc->SetBinContent(b,cor);
      }//for eta bins
      
      cc->GetXaxis()->SetTitle("#eta");
      cc->GetYaxis()->SetTitle("Corr. Factor");
      cc->GetYaxis()->SetTitleOffset(1.5);
      cc->GetYaxis()->SetTitleSize(0.058);
      cc->GetXaxis()->SetTitleSize(0.058);
      cc->GetXaxis()->SetTitleOffset(0.95);

      cc->GetYaxis()->SetRangeUser(0.70,3.5);

      //Create a pave indicating the pt 
      TString ptstr;
      if (PtVals[c]<0.1)
	ptstr.Form("P_{T}=%f",PtVals[c]);
      else 
	ptstr.Form("P_{T}=%.1f",PtVals[c]);

      TPaveText * pave = new TPaveText(0.3,0.75,0.8,0.9,"NDC");
      pave->AddText(algo);
      pave->AddText(ptstr);      
      pave->SetFillColor(0);
      pave->SetShadowColor(0);

      ove->cd(c+1);
      cc->SetFillColor(30);
      cc->SetFillStyle(3001);
      cc->Draw();
      pave->Draw();

    } else break;

  }//for loop

  // return the canvas
  return ove;

}//getCorrectionVsEtaCanvas()

//---------------------------------------------------------------------
TCanvas * getCorrectionVsEtaCanvasTDR(TString algo, FactorizedJetCorrector * jetCorr, TString suffix) {

  //Create canvas vs eta for different pts  
  vector<double> PtVals;
  PtVals.push_back(30);
  PtVals.push_back(100);
  PtVals.push_back(300);

  //Create the canvas with multiple pads
  TString ss("CorrectionVsEta_Overview_TDR");
  ss += suffix;
  TCanvas *ove = new TCanvas(ss,ss,800,800);
  ove->cd();

  // Create a legend for pt values
  TLegend * leg = new TLegend(0.3,0.7,0.8,0.9);
  //TLegend * leg = new TLegend(0.5,0.7,0.9,0.9);
  leg->SetTextSize(0.04);
  leg->SetBorderSize(0);
  leg->SetFillColor(0);

  //Create a pave indicating the algorithm name
  TString algNameLong = getAlgNameLong(algo);
  leg->AddEntry((TObject*)0,algNameLong,"");
  leg->AddEntry((TObject*)0,"","");

  //TPaveText * pave = new TPaveText(0.3,0.84,0.8,0.94,"NDC");
  //pave->AddText(algNameLong);
  //pave->SetFillColor(0);
  //pave->SetShadowColor(0);
  //pave->SetTextFont(42);
  //pave->SetTextSize(0.05);

  //variables for min and max range
  double minY = 1.0;
  double maxY = 1.0;

  // loop over all pads
  for (unsigned int c = 0; c < PtVals.size(); c++) {

     //Create and fill the histo
     TString hstr; hstr.Form("EtaSF_TDR_%d",c);
     TH1F * cc = new TH1F(hstr,hstr,NETA,veta);
     for (int b = 1; b <= cc->GetNbinsX(); b++){
        jetCorr->setJetPt(PtVals[c]);
        jetCorr->setJetEta(cc->GetBinCenter(b));
        jetCorr->setRho(fixedRho);
        jetCorr->setJetA(TMath::Pi()*TMath::Power(JetInfo(algo).coneSize/10.0,2));
        double cor = jetCorr->getCorrection();
        if (std::isnan((double)cor) || std::isinf((double)cor) ){
           cout<<" *** ERROR *** getCorrectionVsEtaCanvas(). For eta="<<cc->GetBinCenter(b)
               <<" and pt="<<PtVals[c]<<" the correction is "<<cor<<"!!"<<endl;
           cor = 10000;
        }
        if ( cor < 0.8  || cor > 3 ){
           cout<<" WARNING  *** getCorrectionVsEtaCanvas(). Correction of "<<cor<<" is out of the (0.8,3) range"<<endl;
           }
        if(cor > maxY) maxY = cor;
        if(cor < minY) minY = cor;
        
        cc->SetBinContent(b,cor);
     }//for eta bins
     
     cc->GetXaxis()->SetTitle("#eta");
     cc->GetYaxis()->SetTitle("Corr. Factor");
     if(algo.Contains("calo"))
        cc->GetYaxis()->SetRangeUser(0.90,2.5);
     else
        cc->GetYaxis()->SetRangeUser(0.90,3.0);
        //cc->GetYaxis()->SetRangeUser(0.90,1.8);
     cc->SetFillColor(30);
     cc->SetFillStyle(3001);

     //Set marker colors and styles
     if(c == 0) {
        cc->SetMarkerStyle(24);
        cc->SetMarkerColor(1);
        cc->SetLineColor(1);
        cc->SetMarkerSize(1.6);
     }
     else if(c == 1) {
        cc->SetMarkerStyle(21);
        cc->SetMarkerColor(2);
        cc->SetLineColor(2);
        cc->SetMarkerSize(1.6);
     }
     else if(c == 2) {
        cc->SetMarkerStyle(20);
        cc->SetMarkerColor(38);
        cc->SetLineColor(38);
        cc->SetMarkerSize(1.6);
     }
     else {
        cc->SetMarkerStyle(25);
        cc->SetMarkerColor(6);
        cc->SetLineColor(6);
        cc->SetMarkerSize(1.6);
     }

     if(c == 0)
        cc->Draw("P");
     else
        cc->Draw("Psame");

     TString ptstr;
     if (PtVals[c]<0.1)
        ptstr.Form("%f",PtVals[c]);
     else 
        ptstr.Form("%.0f",PtVals[c]);

     leg->AddEntry(cc,"P_{T} = "+ptstr+" GeV","p");

  }//for loop

  //pave->Draw("same");
  leg->Draw("same");
  cmsPrelim();
  
  // return the canvas
  return ove;
  
}//getCorrectionVsEtaCanvasTDR()

//---------------------------------------------------------------------
vector<TCanvas*> getCorrectionVsEtaComparisonCanvasTDR(vector<TString>& algs, vector<pair<FactorizedJetCorrector*,TString> > allJetCorrs,
                                                       TString suffix, TString normAlg) {
  setTDRStyle();

  vector<TString> normHSTR;
  int normAlgIndex = -1;
  if(!normAlg.IsNull()) {
    normAlgIndex = JetInfo::vfind(algs,normAlg);
    if(normAlgIndex>-1)
      algs.insert(algs.begin(),algs[normAlgIndex]);
  }

  //Create canvas vs eta for different pts  
  vector<double> PtVals;
  PtVals.push_back(30);
  PtVals.push_back(100);
  //PtVals.push_back(300);
  //PtVals.push_back(500);
  PtVals.push_back(1000);
  //PtVals.push_back(1500);

  //Create the canvas with multiple pads
  //TString ss("CorrectionVsEta_Comparison_TDR");
  //ss += suffix;
  //TCanvas *ove = new TCanvas(ss,ss,1200,400); //800->400
  //ove->Divide(3,1); //2->1
  TH1D* frame = new TH1D();
  frame->GetXaxis()->SetLimits(-5.0,5.0);
  frame->GetYaxis()->SetRangeUser(0.90,1.80);
  frame->GetXaxis()->SetTitle("#eta");
  frame->GetYaxis()->SetTitle("Corr. Factor");
  TH1D* frameRatio = new TH1D();
  frameRatio->GetXaxis()->SetLimits(-5.0,5.0);
  frameRatio->GetYaxis()->SetRangeUser(0.9,1.2);
  frameRatio->GetXaxis()->SetTitle("#eta");
  frameRatio->GetYaxis()->SetTitle("Ratio to R=0.5");

  vector<Int_t> colors = getColors();
  vector<Int_t> markers = getMarkerNumbers();

  // Create a vector of objects for each of the pt values
  vector<TCanvas*> cans;
  vector<TLegend*> legs;
  vector<TPaveText*> pave;

  // loop over all pt values.
  map<TString,TH1F*> cc;
  map<TString,TH1F*> cc_norm;
  for (unsigned int c = 0; c < PtVals.size(); c++) {

    //ove->cd(c+1);
    TString ss("CorrectionVsEta_Comparison_TDR");
    ss = Form("%s_%i%s",ss.Data(),c,suffix.Data());
    //cans.push_back(tdrCanvas(ss,frame,2,11,true));
    setTDRStyle();
    cans.push_back(tdrDiCanvas(ss,frame,frameRatio,2,11));
    continue;
    cans.back()->cd(1);

    bool allAlgsSame = true;
    for(unsigned int ialg=0; ialg<algs.size(); ialg++) {
      if(getAlgNameLong(algs[0],1).CompareTo(getAlgNameLong(algs[ialg],1))!=0) {
        allAlgsSame=false;
        break;
      }
    }
    if(allAlgsSame) {
      //legs.push_back(new TLegend(0.25,0.55,0.9,0.75));
      legs.push_back(tdrLeg(0.25,0.53,0.9,0.73));
      legs.back()->SetNColumns(2);
      legs.back()->SetBorderSize(0);
    }
    else {
      legs.push_back(new TLegend(0.20,0.5,0.83,0.9));
      legs.back()->SetBorderSize(0);
      legs.back()->SetFillStyle(0);
      legs.back()->SetTextSize(0.045);
      legs.back()->SetFillColor(0);
    }

    TString ptstr;
    if (PtVals[c]<0.1)
       ptstr.Form("p_{T} = %f GeV",PtVals[c]);
    else 
       ptstr.Form("p_{T} = %.0f GeV",PtVals[c]);

    //pave.push_back(new TPaveText(0.3,0.75,0.8,0.9,"NDC"));
    //pave.back()->SetTextSize(0.045);
    //pave.back()->SetFillColor(0);
    //pave.back()->SetBorderSize(0);
    pave.push_back(tdrText(0.5,0.75,0.93,1-gPad->GetTopMargin()-0.045*(1-gPad->GetTopMargin()-gPad->GetBottomMargin())-0.01,31));
    pave.back()->AddText("QCD Monte Carlo");
    pave.back()->AddText(JetInfo::get_legend_title(string(algs[0]),false).c_str());
    pave.back()->AddText(ptstr);

    for (unsigned int ialg=0; ialg<algs.size(); ialg++) {
      //Create and fill the histo
      TString hstr; hstr.Form("EtaSF_TDR_%d_%s",c,algs[ialg].Data());
      if(!normAlg.IsNull() && ialg==0) {
        hstr.Form("EtaSF_TDR_%d_%s_norm",c,algs[ialg].Data());
        normHSTR.push_back(hstr);
      }
      cc[hstr] = new TH1F(hstr,hstr,NETA,veta);

      for (int b = 1; b <= cc[hstr]->GetNbinsX(); b++){
         double cor = 1.0;
         if(!normAlg.IsNull() && ialg==0) {
            allJetCorrs[normAlgIndex].first->setJetPt(PtVals[c]);
            allJetCorrs[normAlgIndex].first->setJetEta(cc[hstr]->GetBinCenter(b));
            allJetCorrs[normAlgIndex].first->setRho(fixedRho);
            allJetCorrs[normAlgIndex].first->setJetA(TMath::Pi()*TMath::Power(JetInfo(algs[ialg]).coneSize/10.0,2));
            cor = allJetCorrs[normAlgIndex].first->getCorrection();
         }
         else if (!normAlg.IsNull() && ialg>0){
            allJetCorrs[ialg-1].first->setJetPt(PtVals[c]);
            allJetCorrs[ialg-1].first->setJetEta(cc[hstr]->GetBinCenter(b));
            allJetCorrs[ialg-1].first->setRho(fixedRho);
            allJetCorrs[ialg-1].first->setJetA(TMath::Pi()*TMath::Power(JetInfo(algs[ialg]).coneSize/10.0,2));

            cor = allJetCorrs[ialg-1].first->getCorrection();
         }
         else {
            allJetCorrs[ialg].first->setJetPt(PtVals[c]);
            allJetCorrs[ialg].first->setJetEta(cc[hstr]->GetBinCenter(b));
            allJetCorrs[ialg].first->setRho(fixedRho);
            allJetCorrs[ialg].first->setJetA(TMath::Pi()*TMath::Power(JetInfo(algs[ialg]).coneSize/10.0,2));
            cor = allJetCorrs[ialg].first->getCorrection();
         }
         if (std::isnan((double)cor) || std::isinf((double)cor) ){
            cout<<" *** ERROR *** getCorrectionVsEtaComparisonCanvas(). For eta="<<cc[hstr]->GetBinCenter(b)
                <<" and pt="<<PtVals[c]<<" the correction is "<<cor<<"!!"<<endl;
            cor = 10000;
         }
         if ( cor < 0.8  || cor > 3 ){
            cout<<" WARNING  *** getCorrectionVsEtaComparisonCanvas(). Correction of "<<cor<<" is out of the (0.8,3) range"<<endl;
            }
         
         cc[hstr]->SetBinContent(b,cor);
      }//for eta bins

      //cc[hstr]->GetXaxis()->SetTitle("#eta");
      //cc[hstr]->GetYaxis()->SetTitle("Corr. Factor");
      if(!normAlg.IsNull()) {
        cc_norm[Form("%s_norm",hstr.Data())] = (TH1F*)cc[hstr]->Clone(Form("%s_norm",hstr.Data()));
        cc_norm[Form("%s_norm",hstr.Data())]->GetYaxis()->SetTitle(Form("Corr. Factor / Corr. Factor (%s)",getAlias(normAlg).c_str()));
        //cc_norm[Form("%s_norm",hstr.Data())]->GetYaxis()->SetRangeUser(0.85,1.15);
      }
      //if(algs[ialg].Contains("calo"))
      //   cc[hstr]->GetYaxis()->SetRangeUser(0.90,2.5);
      //else
      //   cc[hstr]->GetYaxis()->SetRangeUser(0.90,1.8);
      //cc[hstr]->SetFillColor(30);
      //cc[hstr]->SetFillStyle(3001);

      //Set marker colors and styles
      //cc[hstr]->SetMarkerSize(0.7);
      //if(!normAlg.IsNull() && ialg>0) {
      //  cc_norm[Form("%s_norm",hstr.Data())]->SetMarkerStyle(markers[ialg-1]);
      //  cc_norm[Form("%s_norm",hstr.Data())]->SetMarkerColor(colors[ialg-1]);
      //  cc_norm[Form("%s_norm",hstr.Data())]->SetLineColor(colors[ialg-1]);
      //}
      //else {
      //  cc[hstr]->SetMarkerStyle(markers[ialg]);
      //  cc[hstr]->SetMarkerColor(colors[ialg]);
      //  cc[hstr]->SetLineColor(colors[ialg]);
      //}

      if(!normAlg.IsNull() && ialg!=0) {
        //cout << hstr << " is being divided by " << normHSTR.back() << endl;
        cc_norm[Form("%s_norm",hstr.Data())]->Divide(cc[normHSTR.back()]);
        cans.back()->cd(2);
        tdrDraw(cc_norm[Form("%s_norm",hstr.Data())],"P",markers[ialg],colors[ialg],kSolid,colors[ialg],kNone,0);
      }

      if( (ialg == 0 && normAlg.IsNull()) || (ialg==1 && !normAlg.IsNull()) ) {
         //cc[hstr]->Draw("P");
        cans.back()->cd(1);
        tdrDraw(cc[hstr],"P",markers[ialg],colors[ialg],kSolid,colors[ialg],kNone,0);
      }
      else if (ialg == 0 && !normAlg.IsNull()) {
         continue;
      }
      else {
        //cc[hstr]->Draw("Psame");
        cans.back()->cd(1);
        tdrDraw(cc[hstr],"P",markers[ialg],colors[ialg],kSolid,colors[ialg],kNone,0);
      }

      //Create a pave indicating the algorithm name
      if ( (normAlg.IsNull()) || (ialg>0 && !normAlg.IsNull()) ) {
        if(allAlgsSame)
          legs.back()->AddEntry(cc[hstr],getAlgNameLong(algs[ialg],2),"p");
        else
          legs.back()->AddEntry(cc[hstr],getAlgNameLong(algs[ialg]),"p");
      }
    }//for alg

    //pave->Draw("same");
    cans.back()->cd(1);
    legs.back()->Draw("same");
    pave.back()->Draw("same");
    //cmsPrelim();

    //gPad->RedrawAxis();
  }//for pt bins

  //algs.erase(algs.begin());

  // return the canvas
  return cans;

}//getCorrectionVsEtaComparisonCanvasTDR

//---------------------------------------------------------------------
TCanvas * getCorrectionVsPtCanvas(TString algo, FactorizedJetCorrector * jetCorr, TString suffix) {

 //Create canvas vs eta for different pts  
  vector<double> EtaVals;
  EtaVals.push_back(-4.8);
  EtaVals.push_back(-4.0);
  EtaVals.push_back(-3.2);
  EtaVals.push_back(-2.4);
  EtaVals.push_back(-1.6);
  EtaVals.push_back(-0.8);
  EtaVals.push_back(0);
  EtaVals.push_back( 0.8);
  EtaVals.push_back( 1.6);
  EtaVals.push_back( 2.4);
  EtaVals.push_back( 3.2);
  EtaVals.push_back( 4.0);
  EtaVals.push_back( 4.8);

  //Create the canvas with multiple pads
  TString ss("CorrectionVsPt_Overview");
  ss += suffix;
  TCanvas *ovp = new TCanvas(ss,ss,1200,800);
  ovp->Divide(4,4);

  // loop over all pads
  for (int c = 0; c < ovp->GetListOfPrimitives()->GetSize(); c++) {

    // just make we don't write the extra pads 
    if (c <= (int) EtaVals.size()){

      //Create and fill the histo
      TString hstr; hstr.Form("PtSF_%d",c);
      TH1F * cc = new TH1F(hstr,hstr,NPtBinsHLT,vpt_HLT);
      for (int b = 1; b <= cc->GetNbinsX(); b++){
	jetCorr->setJetPt(cc->GetBinCenter(b));
	jetCorr->setJetEta(EtaVals[c]);
    jetCorr->setRho(fixedRho);
    jetCorr->setJetA(TMath::Pi()*TMath::Power(JetInfo(algo).coneSize/10.0,2));
	double cor = jetCorr->getCorrection();
	if (std::isnan((double)cor) ||  std::isinf((double)cor) ){
	  cout<<" *** ERROR *** getCorrectionVsPtCanvas(). For eta="<<EtaVals[c]
	      <<" and pt="<<cc->GetBinCenter(b)<<" the correction is "<<cor<<"!!"<<endl;
	  cor = 10000;
	}
	if ( cor < 0.8  || cor > 3 ){
	  cout<<" WARNING  *** getCorrectionVsPtCanvas(). Correction of "<<cor<<" is out of the (0.8,3) range"<<endl;
	}

	cc->SetBinContent(b,cor);
      }//for pt bins
      cc->GetXaxis()->SetTitle("Pt");
      cc->GetYaxis()->SetTitle("Corr. Factor");
      cc->GetYaxis()->SetRangeUser(0.70,3.0);

      //Create a pave indicating the eta 
      TString ptstr;
      ptstr.Form("#eta=%.1f",EtaVals[c]);
      TPaveText * pave = new TPaveText(0.3,0.75,0.8,0.9,"NDC");
      pave->AddText(algo);
      pave->AddText(ptstr);      
      pave->SetFillColor(0);
      pave->SetShadowColor(0);

      (ovp->cd(c+1))->SetLogx(1);
      cc->SetFillColor(30);
      cc->SetFillStyle(3001);
      cc->Draw();
      pave->Draw();

    } else break;

  }//for loop

  // return the canvas
  return ovp;

}//getCorrectionVsPtCanvas()

//---------------------------------------------------------------------
TCanvas * getCorrectionVsPtComparisonCanvasTDR(vector<TString>& algs, vector<pair<FactorizedJetCorrector*,TString> > allJetCorrs,
                                               TString suffix) {

  //Create canvas vs eta for different pts  
  vector<double> EtaVals;
  EtaVals.push_back(-4.8);
  EtaVals.push_back(-4.0);
  EtaVals.push_back(-3.2);
  EtaVals.push_back(-2.4);
  EtaVals.push_back(-1.6);
  EtaVals.push_back(-0.8);
  EtaVals.push_back(0);
  EtaVals.push_back( 0.8);
  EtaVals.push_back( 1.6);
  EtaVals.push_back( 2.4);
  EtaVals.push_back( 3.2);
  EtaVals.push_back( 4.0);
  EtaVals.push_back( 4.8);

  //Create the canvas with multiple pads
  TString ss("CorrectionVsPt_Comparison_TDR");
  ss += suffix;
  TCanvas *ovp = new TCanvas(ss,ss,1200,1200);
  ovp->Divide(4,4);

  vector<Int_t> colors = getColors();
  vector<Int_t> markers = getMarkerNumbers();

  // Create a legend for pt values
  vector<TLegend*> legs;

  // loop over all pt values.
  map<TString,vector<TH1F*> > cc;
  for (unsigned int c = 0; c < EtaVals.size(); c++) {

    ovp->cd(c+1)->SetLogx(1);

    legs.push_back(new TLegend(0.20,0.5,0.85,0.9));
    legs.back()->SetTextSize(0.04);
    legs.back()->SetBorderSize(0);
    legs.back()->SetFillColor(0);
    TString etastr;
    etastr.Form("%.1f",EtaVals[c]);
    //leg->AddEntry(cc,"P_{T} = "+ptstr+" GeV","p");
    legs.back()->AddEntry((TObject*)0,"#eta = "+etastr,"");
    legs.back()->AddEntry((TObject*)0,"","");

    for (unsigned int ialg=0; ialg<algs.size(); ialg++) {
      //Create and fill the histo
      TString hstr; hstr.Form("PtSF_TDR_%d_%s",c,algs[ialg].Data());
      TH1F * cc = new TH1F(hstr,hstr,NPtBinsHLT,vpt_HLT);;
      for (int b = 1; b <= cc->GetNbinsX(); b++){
         allJetCorrs[ialg].first->setJetEta(EtaVals[c]);
         allJetCorrs[ialg].first->setJetPt(cc->GetBinCenter(b));
         allJetCorrs[ialg].first->setRho(fixedRho);
         allJetCorrs[ialg].first->setJetA(TMath::Pi()*TMath::Power(JetInfo(algs[ialg]).coneSize/10.0,2));
         double cor = allJetCorrs[ialg].first->getCorrection();
         if (std::isnan((double)cor) || std::isinf((double)cor) ){
            cout<<" *** ERROR *** getCorrectionVsEtaComparisonCanvas(). For eta="<<cc->GetBinCenter(b)
                <<" and pt="<<EtaVals[c]<<" the correction is "<<cor<<"!!"<<endl;
            cor = 10000;
         }
         if ( cor < 0.8  || cor > 3 ){
            cout<<" WARNING  *** getCorrectionVsEtaComparisonCanvas(). Correction of "<<cor<<" is out of the (0.8,3) range"<<endl;
            }
         
         cc->SetBinContent(b,cor);
      }//for eta bins

      cc->GetXaxis()->SetTitle("p_{T}");
      cc->GetYaxis()->SetTitle("Corr. Factor");
      if(abs(EtaVals[c])>2.0)
         cc->GetYaxis()->SetRangeUser(0.7,6.0);
      else
         cc->GetYaxis()->SetRangeUser(0.7,3.0);
      cc->SetFillColor(30);
      cc->SetFillStyle(3001);

      //Set marker colors and styles
      cc->SetMarkerSize(0.7);
      cc->SetMarkerStyle(markers[ialg]);
      cc->SetMarkerColor(colors[ialg]);
      cc->SetLineColor(colors[ialg]);

      if(ialg == 0)
         cc->Draw("P");
      else
         cc->Draw("Psame");

      //Create a pave indicating the algorithm name
      TString algNameLong = getAlgNameLong(algs[ialg]);
      legs.back()->AddEntry(cc,algNameLong,"p");
    }//for alg

    //pave->Draw("same");
    legs.back()->Draw("same");
    cmsPrelim();
  }//for eta bins
  
  // return the canvas
  return ovp;

}//getCorrectionVsPtComparisonCanvasTDR

//---------------------------------------------------------------------
FactorizedJetCorrector * getFactorizedCorrector(TString algo, CommandLine & cl, TString & label) {

  string  path         = cl.getValue<string> ("path","./");
  string  era          = cl.getValue<string> ("era");
  bool    useL1FasCor  = cl.getValue<bool>   ("useL1FasCor"  , false   );
  bool    useL1OffCor  = cl.getValue<bool>   ("useL1OffCor"  , false   );
  bool    useL2Cor     = cl.getValue<bool>   ("useL2Cor"     , false   );
  bool    useL3Cor     = cl.getValue<bool>   ("useL3Cor"     , false   );
  bool    useL2L3ResCor= cl.getValue<bool>   ("useL2L3ResCor", false   );
          fixedRho     = cl.getValue<double> ("fixedRho"     , 10.0    );

  if (era.length()==0) {
    cout<<"ERROR flag -era must be specified"<<endl;
    return 0;
  }
    

  string alias = getAlias(algo);

  // Create the corrections from the text files
  vector<JetCorrectorParameters> vPar;
  if(useL1FasCor)
    {
      string txtfname = path + era + "_L1FastJet_"   + alias + ".txt";
      cout << "\tUsing " << txtfname<<endl;
      JetCorrectorParameters * jcPar = new JetCorrectorParameters(txtfname);
      vPar.push_back(*jcPar);
      label += "L1Fast";
    }
  if(useL1OffCor)
    {
      string txtfname = path + era + "_L1Offset_"   + alias + ".txt";
      cout << "\tUsing " << txtfname<<endl;
      JetCorrectorParameters * jcPar = new JetCorrectorParameters(txtfname);
      vPar.push_back(*jcPar);
      label += "L1Off";
    }
  if(useL2Cor)
    {
      string txtfname = path + era + "_L2Relative_"   + alias + ".txt";
      cout << "\tUsing " << txtfname<<endl;
      JetCorrectorParameters * jcPar = new JetCorrectorParameters(txtfname);
      vPar.push_back(*jcPar);
      label += "L2";
    }
  if(useL3Cor)
    {
      string txtfname = path + era + "_L3Absolute_"   + alias + ".txt";
      cout << "\tUsing " << txtfname<<endl;
      JetCorrectorParameters * jcPar = new JetCorrectorParameters(txtfname);
      vPar.push_back(*jcPar);
      label += "L3";
    }
  if(useL2L3ResCor)
    {
      string txtfname = path + era + "_L2L3Residual_"   + alias + ".txt";
      cout << "\tUsing " << txtfname<<endl;
      JetCorrectorParameters * jcPar = new JetCorrectorParameters(txtfname);
      vPar.push_back(*jcPar);
      label += "L2L3Res";
    }
  if (vPar.size()==0){
    cout<<"ERROR you must set one of following flags to true:"<<endl
	<<"\t-useL1FasCor  "<<endl
	<<"\t-useL1OffCor  "<<endl
	<<"\t-useL2Cor	    "<<endl
	<<"\t-useL3Cor	    "<<endl
	<<"\t-useL2L3ResCor"<<endl;
    return 0;
  }
  
  return new FactorizedJetCorrector(vPar);

}//getFactorizedCorrector


//______________________________________________________________________________
string getAlias(TString s)
{
   if (s=="ic5calo")
      return "IC5Calo";
   else if (s=="ic5pf")
      return "IC5PF";
   else if (s=="ak5calo")
      return "AK5Calo";  
   else if (s=="ak5calol1")
      return "AK5Calol1";
   else if (s=="ak5calol1off")
      return "AK5Calol1off";
   else if (s=="ak5calol1offl2l3")
      return "AK5Calol1off";
   else if (s=="ak7calo")
      return "AK7Calo";
   else if (s=="ak7calol1")
      return "AK7Calol1";
   else if (s=="ak7calol1off")
      return "AK7Calol1off";
   else if (s=="ak5caloHLT")
      return "AK5CaloHLT";
   else if (s=="ak5caloHLTl1")
      return "AK5CaloHLTl1";
   else if (s=="ak1pf")
      return "AK1PF";
   else if (s=="ak1pfl1")
      return "AK1PFl1";
   else if (s=="ak2pf")
      return "AK2PF";
   else if (s=="ak2pfl1")
      return "AK2PFl1";
   else if (s=="ak3pf")
      return "AK3PF";
   else if (s=="ak3pfl1")
      return "AK3PFl1";
   else if (s=="ak4pf")
      return "AK4PF";
   else if (s=="ak4pfl1")
      return "AK4PFl1";
   else if (s=="ak5pf")
      return "AK5PF";
   else if (s=="ak5pfl1")
      return "AK5PFl1";
   else if (s=="ak5pfl1l2l3")
      return "AK5PFl1";
   else if (s=="ak5pfl1off")
      return "AK5PFl1off";
   else if (s=="ak6pf")
      return "AK6PF";
   else if (s=="ak6pfl1")
      return "AK6PFl1";
   else if (s=="ak7pf")
      return "AK7PF";
   else if (s=="ak7pfl1")
      return "AK7PFl1";
   else if (s=="ak7pfl1off")
      return "AK7PFl1off";
   else if (s=="ak8pf")
      return "AK8PF";
   else if (s=="ak8pfl1")
      return "AK8PFl1";
   else if (s=="ak9pf")
      return "AK9PF";
   else if (s=="ak9pfl1")
      return "AK9PFl1";
   else if (s=="ak10pf")
      return "AK10PF";
   else if (s=="ak10pfl1")
      return "AK10PFl1";
   else if (s=="ak1pfchs")
      return "AK1PFchs";
   else if (s=="ak1pfchsl1")
      return "AK1PFchsl1";
   else if (s=="ak2pfchs")
      return "AK2PFchs";
   else if (s=="ak2pfchsl1")
      return "AK2PFchsl1";
   else if (s=="ak3pfchs")
      return "AK3PFchs";
   else if (s=="ak3pfchsl1")
      return "AK3PFchsl1";
   else if (s=="ak4pfchs")
      return "AK4PFchs";
   else if (s=="ak4pfchsl1")
      return "AK4PFchsl1";
   else if (s=="ak5pfchs")
      return "AK5PFchs";
   else if (s=="ak5pfchsl1")
      return "AK5PFchsl1";
   else if (s=="ak5pfchsl1l2l3")
      return "AK5PFchsl1";
   else if (s=="ak5pfchsl1off")
      return "AK5PFchsl1off";
   else if (s=="ak6pfchs")
      return "AK6PFchs";
   else if (s=="ak6pfchsl1")
      return "AK6PFchsl1";
   else if (s=="ak7pfchs")
      return "AK7PFchs";
   else if (s=="ak7pfchsl1")
      return "AK7PFchsl1";
   else if (s=="ak7pfchsl1off")
      return "AK7PFchsl1off";
   else if (s=="ak8pfchs")
      return "AK8PFchs";
   else if (s=="ak8pfchsl1")
      return "AK8PFchsl1";
   else if (s=="ak9pfchs")
      return "AK9PFchs";
   else if (s=="ak9pfchsl1")
      return "AK9PFchsl1";
   else if (s=="ak10pfchs")
      return "AK10PFchs";
   else if (s=="ak10pfchsl1")
      return "AK10PFchsl1";
   else if (s=="ak5pfHLT")
      return "AK5PFHLT";
  else if (s=="ak5pfHLTl1")
      return "AK5PFHLTl1";
   else if (s=="ak5pfchsHLT")
      return "AK5PFchsHLT";
   else if (s=="ak5pfchsHLTl1")
      return "AK5PFchsHLTl1";
   else if (s=="ak5jpt")
      return "AK5JPT";
   else if (s=="ak5jptl1")
      return "AK5JPTl1";
   else if (s=="ak5jptl1off")
      return "AK5JPTl1off";
   else if (s=="ak5jptl1l2l3")
      return "AK5JPTl1";
   else if (s=="ak5jptl1offl2l3")
      return "AK5JPTl1off";
   else if (s=="ak7jpt")
      return "AK7JPT";
   else if (s=="ak7jptl1")
      return "AK7JPTl1";
   else if (s=="ak7jptl1off")
      return "AK7JPTl1off";
   else if (s=="sc5calo")
      return "SC5Calo";
   else if (s=="sc5pf")
      return "SC5PF";
   else if (s=="sc7calo")
      return "SC5Calo";
   else if (s=="sc7pf")
      return "SC5PF";
   else if (s=="kt4calo")
      return "KT4Calo";
   else if (s=="kt4pf")
      return "KT4PF";
   else if (s=="kt6calo")
      return "KT6Calo";
   else if (s=="kt6pf")
      return "KT6PF";
   else if (s=="ak5calordl1")
      return "AK5CaloRDl1";
   else if (s=="ak5pfrdl1")
      return "AK5PFRDl1";
   else if (s=="ak5pfchsrdl1")
      return "AK5PFchsRDl1";
   else if (s=="ak7calordl1")
      return "AK7CaloRDl1";
   else if (s=="ak7pfrdl1")
      return "AK7PFRDl1";
   else if (s=="ak7pfchsrdl1")
      return "AK7PFchsRDl1";
   else
      return "unknown";
}

//______________________________________________________________________________
//coneSize = 0 the entire name
//coneSize = 1 no cone size
//coneSize = 2 only cone size
TString getAlgNameLong(TString algo, int coneSize) {
  TString algNameLong;

  if (coneSize<2)
    if(algo.Contains("ak"))        algNameLong += "Anti-kT";
  if(coneSize==0 || coneSize==2) {
     if(algo.Contains("ak1") && !algo.Contains("ak10")) algNameLong += " R=0.1";
    else if(algo.Contains("2"))    algNameLong += " R=0.2";
    else if(algo.Contains("3"))    algNameLong += " R=0.3";
    else if(algo.Contains("4"))    algNameLong += " R=0.4";
    else if(algo.Contains("5"))    algNameLong += " R=0.5";
    else if(algo.Contains("6"))    algNameLong += " R=0.6";
    else if(algo.Contains("7"))    algNameLong += " R=0.7";
    else if(algo.Contains("8"))    algNameLong += " R=0.8";
    else if(algo.Contains("9"))    algNameLong += " R=0.9";
    else if(algo.Contains("10"))   algNameLong += " R=1.0";
  }
  if(coneSize<2) {
    if(algo.Contains("pfchs"))     algNameLong += ", PF+CHS";
    //else if(algo.Contains("pf"))   algNameLong += ", PFlow";
    else if(algo.Contains("pf"))   algNameLong += ", Particle-Flow Jets";
    else if(algo.Contains("calo")) algNameLong += ", Calo";
    else if(algo.Contains("jpt"))  algNameLong += ", JPT";

    if(algo.Contains("rd")) algNameLong += ", (RD)";
  }

  return algNameLong;
}

//______________________________________________________________________________
vector<Int_t> getColors() {
  vector<Int_t> ret;
  ret.push_back(kBlack);
  ret.push_back(kRed);
  ret.push_back(kOrange+1);
  ret.push_back(kYellow+2);
  ret.push_back(kGreen);
  ret.push_back(kCyan);
  ret.push_back(kAzure+1);
  ret.push_back(kViolet+1);
  ret.push_back(kMagenta);
  ret.push_back(kGray+2);
  return ret;
}

//______________________________________________________________________________
vector<Int_t> getMarkerNumbers() {
  vector<Int_t> ret;
  ret.push_back(20);
  ret.push_back(33);
  ret.push_back(22);
  ret.push_back(21);
  ret.push_back(24);
  ret.push_back(27);
  ret.push_back(26);
  ret.push_back(25);
  ret.push_back(28);
  ret.push_back(34);
  return ret;
}

//______________________________________________________________________________
void cmsPrelim(double intLUMI)
{
   const float LUMINOSITY = intLUMI;
   TLatex latex;
   latex.SetNDC();
   latex.SetTextSize(0.045);

   latex.SetTextAlign(31); // align right
   latex.DrawLatex(0.93,0.96,"#sqrt{s} = 13 TeV");
   if (LUMINOSITY > 0.) {
      latex.SetTextAlign(31); // align right
      //latex.DrawLatex(0.82,0.7,Form("#int #font[12]{L} dt = %d pb^{-1}", (int) LUMINOSITY)); //Original
      latex.DrawLatex(0.65,0.85,Form("#int #font[12]{L} dt = %d pb^{-1}", (int) LUMINOSITY)); //29/07/2011
   }
   latex.SetTextAlign(11); // align left
   //latex.DrawLatex(0.16,0.96,"CMS preliminary");// 2012");
   latex.DrawLatex(0.16,0.96,"CMS Simulation");// 2012");
}

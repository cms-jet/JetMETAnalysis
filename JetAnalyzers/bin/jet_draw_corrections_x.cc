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

#include "JetMETAnalysis/JetUtilities/interface/Style.h"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/JetInfo.hh"
#include "JetMETAnalysis/JetUtilities/interface/Variogram.hh"
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
#include "TGraphErrors.h"
#include "TF1.h"
#include "TString.h"
#include "TPaveText.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TMath.h"
#include "TColor.h"
#include "TLine.h"

#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <stdlib.h>

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

TCanvas * getCorrectionMap(TString algo, FactorizedJetCorrector * jetCorr, TString suffix, double CMEnergy);

TCanvas * draw_response(TString algo, FactorizedJetCorrector * jetCorr, TString suffix, bool doATLAS=false);

vector<Int_t> getColors();

vector<Int_t> getMarkerNumbers();

// Move to mu-based mapping, which is better for comparing
// different PU scenarios as it considers both IT and OOT PU,
// plus we have a number directly comparable to ATLAS
double rhoFromMu(double mu);

// Use this for solving ptmeas from ptgen = JEC(ptmeas) * ptmeas
Double_t fJECPt(Double_t *x, Double_t *p);

// Response as a function of pTprime (instead of JEC vs pTmeas)
double getResp(TF1 *_jecpt, double ptgen, double eta, double jeta, double mu);

////////////////////////////////////////////////////////////////////////////////
// global variables
////////////////////////////////////////////////////////////////////////////////
vector<pair<FactorizedJetCorrector*,TString> > allJetCorrs;
double fixedRho;
FactorizedJetCorrector* _jec;

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
  double          CMEnergy     = cl.getValue<double>   ("CMEnergy",             13000);

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

  // get the canvas of correction vs eta and pt, write and save to file
  TCanvas * omap = getCorrectionMap(algo, jetCorr,suffix,CMEnergy);
  for(unsigned int of=0; of<outputFormat.size(); of++) {
     omap->SaveAs(outputDir+string(omap->GetName())+outputFormat[of]);
  }
  omap->Write();

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

  // get the canvas of response vs pt in the ATLAS style, write and save to file
  TCanvas * oATLAS = draw_response(algo, jetCorr, suffix, true);
  for(unsigned int of=0; of<outputFormat.size(); of++) {
     oATLAS->SaveAs(outputDir+string(oATLAS->GetName())+outputFormat[of]);
  }
  oATLAS->Write();

  // get the canvas of response vs pt in the CMS style, write and save to file
  TCanvas * oCMS = draw_response(algo, jetCorr, suffix, false);
  for(unsigned int of=0; of<outputFormat.size(); of++) {
     oCMS->SaveAs(outputDir+string(oCMS->GetName())+outputFormat[of]);
  }
  oCMS->Write();

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
  TString algNameLong = JetInfo::get_legend_title(string(algo));
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
      if(JetInfo::get_legend_title(algs[0],false).CompareTo(JetInfo::get_legend_title(algs[ialg],false))!=0) {
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
        cc_norm[Form("%s_norm",hstr.Data())]->GetYaxis()->SetTitle(Form("Corr. Factor / Corr. Factor (%s)",JetInfo(normAlg).getAlias().Data()));
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
          legs.back()->AddEntry(cc[hstr],Form(" R=%.1f",JetInfo(algs[ialg]).coneSize/10.0),"p");
        else
          legs.back()->AddEntry(cc[hstr],JetInfo::get_legend_title(algs[ialg]),"p");
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
      TString algNameLong = JetInfo::get_legend_title(algs[ialg]);
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
TCanvas * getCorrectionMap(TString algo, FactorizedJetCorrector * jetCorr,
                           TString suffix, double CMEnergy) {
   //gStyle->SetOptTitle(0);
   //gStyle->SetOptStat(0);

   TString hstr = "CorrMap";
   TH2D * cc = new TH2D(hstr,hstr,NPtBins,vpt,NETA,veta);

   TString ss("CorrectionMap_Overview_TDR");
   ss += suffix;
   TH1D* frame = new TH1D();
   frame->GetXaxis()->SetLimits(1,10000);
   frame->GetXaxis()->SetTitle("p_{T}^{raw}");
   frame->GetYaxis()->SetRangeUser(cc->GetYaxis()->GetXmin(),cc->GetYaxis()->GetXmax());
   frame->GetYaxis()->SetTitle("#eta");
   TCanvas* ove = tdrCanvas(ss,frame,14,0,true);
   ove->cd(0);
   ove->SetLogx();
   ove->SetLeftMargin(ove->GetLeftMargin()*0.75);
   ove->SetRightMargin(ove->GetRightMargin()*2.5);
   frame->GetYaxis()->SetTitleOffset(0.8);

/*
   TCanvas *ove = new TCanvas(ss,ss,900,900);
   ove->cd();
   ove->SetLogx();
   ove->SetLeftMargin(0.145089);
   ove->SetRightMargin(0.132812);
   ove->SetTopMargin(0.0860092);
   ove->SetBottomMargin(0.178899);
*/

   // loop over all pads
   for (unsigned int b = 0; b < NETA; b++){
      double eta = (veta[b] + veta[b+1])/2;


      for (unsigned int c = 0; c < NPtBins; c++) {
         double pt = (vpt[c]+vpt[c+1])/2;
         float etaMax = min(5.191,TMath::ACosH(CMEnergy/2.0/pt));
         if(abs(eta)>etaMax || pt>CMEnergy/2.0) continue;

         jetCorr->setJetPt(vpt[c]);
         jetCorr->setJetEta(eta);
         jetCorr->setRho(fixedRho);
         jetCorr->setJetA(TMath::Pi()*TMath::Power(JetInfo(algo).coneSize/10.0,2));
         double cor = jetCorr->getCorrection();
         if (std::isnan((double)cor) || std::isinf((double)cor) ){
            cout<<" *** ERROR *** " << endl;//getCorrectionVsEtaCanvas(). For eta="<<cc->GetBinCenter(b)
               //              <<" and pt="<<PtVals[c]<<" the correction is "<<cor<<"!!"<<endl;
               //cor = 10000;
               }
         //        if ( cor < 0.8  || cor > 3 ){
         //cout<<" WARNING  *** getCorrectionVsEtaCanvas(). Correction of "<<cor<<" is out of the (0.8,3) range"<<endl;
         //}
         cout << pt << " "<< eta  << " " << cor <<  endl;

         //if (pt > vPar[0].record(b).parameter(0) && pt < vPar[0].record(b).parameter(1)){
           cc->Fill(pt,eta, cor);
       //}
       }//for eta bins

  }//for loop

   cc->GetYaxis()->SetTitle("#eta");
   cc->GetXaxis()->SetRangeUser(1,10000);
   cc->GetXaxis()->SetTitle("p_{T}^{raw}");
   cc->GetXaxis()->SetLabelSize(0.04);

   ove->Update();
   /*TPaletteAxis *palette = (TPaletteAxis*)cc->GetListOfFunctions()->FindObject("palette");
     palette->SetX1NDC(0.879464);
     palette->SetX2NDC(0.917411);
     palette->SetY1NDC(0.172018);
     palette->SetY2NDC(0.915138);*/
   ove->Modified();
   ove->Update();
   cc->SetContour(20);
   gStyle->SetPalette(55);
   tdrDraw(cc,"colz",kNone,kNone,kNone,kNone);
   fixOverlay();
   //cc->Draw("colz");

   // Draw a subpad with a semi-variogram
   map<string,double> *variogram_params = new map<string,double>();
   Variogram v(cc);
   v.dist_to_vector_from_squareform(false);
   int tolerance = 1;
   vector<int> lags(30);
   int val = 0;
   for(unsigned int i=0; i<lags.size(); i++, val+=tolerance*1) {
      lags[i] = val;
   }
   v.getSemivariogram(lags,1,true);
   TCanvas* subcan = v.plotSemivariogram(lags,1,"spherical:6",true,variogram_params);
   ove->cd();
   TPad *subpad = new TPad("subpad", "subpad", 0.69, 0.18, 0.85, 0.34);
   subpad->SetBottomMargin(0);
   subpad->SetTopMargin(0);
   subpad->SetLeftMargin(0);
   subpad->SetRightMargin(0);
   //subpad->SetFillColor(kRed);
   subpad->SetFillStyle(1001);
   //subpad->SetFillStyle(4000);
   gStyle->SetPalette(55);
   subpad->Draw("same");
   subpad->cd();
   subcan->DrawClonePad();

   ove->cd();
   //Create a legend indicating the algorithm name
   TString algNameLong = JetInfo::get_legend_title(algo);
   TLegend* ll = tdrLeg(0.63,0.87,0.87,0.90);
   ll->SetTextFont(42);
   ll->SetTextSize(0.020);
   ll->SetHeader(algNameLong);
   ll->Draw("same");

   TLegend* l = tdrLeg(0.63,0.73,0.87,0.87);
   l->SetTextFont(42);
   l->SetTextSize(0.020);
   l->SetHeader("Variogram Parameters:");
   l->AddEntry((TObject*)0,"Model = Spherical","");
   l->AddEntry((TObject*)0,Form("%s = %.3f",variogram_params->find("Smp. Var.")->first.c_str(),variogram_params->find("Smp. Var.")->second),"");
   l->AddEntry((TObject*)0,Form("%s = %.3f",variogram_params->find("a (model)")->first.c_str(),variogram_params->find("a (model)")->second),"");
   l->AddEntry((TObject*)0,Form("%s = %.3f",variogram_params->find("sill (model)")->first.c_str(),variogram_params->find("sill (model)")->second),"");
   l->Draw("same");

   // return the canvas
   return ove;

}//getCorrectionMap()

//---------------------------------------------------------------------
TCanvas * draw_response(TString algo, FactorizedJetCorrector * jetCorr, TString suffix, bool doATLAS) {
    _jec = jetCorr;
    TF1 *_jecpt = new TF1("jecpt",fJECPt,0,4000,3);

  // doATLAS:
  // If true, these values represent energies
  // If false, these values represent pt
  double vars[] = {30, 60, 110, 400, 2000};
  const int nvar = sizeof(vars)/sizeof(vars[0]);
  const int neta = 48;//52;
  const int jeta = TMath::Pi()*0.5*0.5;
  const int mu = 0;

  TGraph *gs[nvar];
  for (int ivar = 0; ivar != nvar; ++ivar) {

    double independent_variable = vars[ivar];

    TGraph *g = new TGraph(0); gs[ivar] = g;
    for (int ieta = 0; ieta != neta; ++ieta) {
      
      double eta = (ieta+0.5)*0.1;
      double dependent_variable;
      if(doATLAS) dependent_variable = independent_variable / cosh(eta);
      else dependent_variable = independent_variable * cosh(eta);
      if ((doATLAS && dependent_variable > 10. && independent_variable < 4000.) ||
          (independent_variable > 10. && dependent_variable < 4000.)) {
        double jes;
        if(doATLAS) jes = getResp(_jecpt, dependent_variable, eta, jeta, mu);
        else jes = getResp(_jecpt, independent_variable, eta, jeta, mu);
        int n = g->GetN();
        g->SetPoint(n, eta, jes);
      }
    } // for ie
  } // for ieta

  // Draw results
  //TCanvas *c1 = new TCanvas("c1","c1",600,600);
  //TCanvas *c1 = new TCanvas("c1","c1",800,600); // ATLAS shape
  TH1D *h;
  if(doATLAS) h = new TH1D("h",";Jet |#eta|;Jet response at PF scale",40,0,4.8);
  else h = new TH1D("h",";Jet |#eta|;Simulated jet response",40,0,4.8);
  h->SetMaximum(1.25);
  h->SetMinimum(0.5);
  //h->Draw("AXIS");
  TString ss;
  if(doATLAS) ss+="ATLASresponse";
  else ss+="CMSresponse";
  ss += suffix;
  TCanvas *c1 = tdrCanvas(ss.Data(),h,14,0,doATLAS ? kRectangular : kSquare);

  TLegend *leg1 = tdrLeg(0.25,0.25,0.55,0.30);
  TLegend *leg2 = tdrLeg(0.25,0.20,0.55,0.25);
  TLegend *leg3 = tdrLeg(0.25,0.15,0.55,0.20);
  TLegend *leg4 = tdrLeg(0.55,0.25,0.85,0.30);
  TLegend *leg5 = tdrLeg(0.55,0.20,0.85,0.25);
  TLegend *legs[nvar] = {leg1, leg2, leg3, leg4, leg5};

  int colors[] = {kGreen+2, kBlack, kOrange+1, kBlue, kRed+1};
  int markers[] = {kFullCircle, kOpenCircle, kFullSquare, kOpenSquare,
       kFullTriangleUp};

  for (int ivar = 0; ivar != nvar; ++ivar) {
    
    TGraph *g = gs[ivar];
    g->SetMarkerColor(colors[ivar]);
    g->SetMarkerStyle(markers[ivar]);
    g->Draw("SAMEP");

    //TLegend *leg = (ie<3 ? leg1 : leg2);
    TLegend *leg = legs[ivar];
    leg->SetTextColor(colors[ivar]);
    TString var_name;
    if(doATLAS) var_name = "E";
    else var_name = "p_{T}";
    leg->AddEntry(g, Form("%s = %1.0f GeV",var_name.Data(),vars[ivar]), "P");
  }


  TLatex *tex = new TLatex();
  tex->SetNDC();
  tex->SetTextSize(0.045);
  
  TLine *l = new TLine();
  l->DrawLine(1.3,0.7,1.3,1.1);
  l->DrawLine(2.5,0.7,2.5,1.1);
  l->DrawLine(3.0,0.7,3.0,1.1);
  l->DrawLine(4.5,0.7,4.5,1.1);
  l->SetLineStyle(kDashed);
  l->DrawLine(3.2,0.7,3.2,1.1);

  TLatex* tex_tmp = tex->DrawLatex(0.35,0.86,"2016 JES: "+JetInfo::get_legend_title(algo));
  //The size of the x-axis in axis coordinates
  double x_axis_width = c1->GetUxmax()-c1->GetUxmin();
  //The width of the pad in x-axis coordinates = x-axis width/percentage that the x-axis takes up in NDC coordinates
  double total_width = x_axis_width/(1.0-c1->GetRightMargin()-c1->GetLeftMargin());
  //The blank space on either size of the text in NDC coordinates starting from the left margin (i.e. in the frame)
  double side_padding = (1.0-c1->GetRightMargin()-c1->GetLeftMargin()-(tex_tmp->GetXsize()/total_width))/2.0;
  //Add back in the left margin so that the text is centered in the frame and not the pad
  tex_tmp->SetX(c1->GetLeftMargin()+side_padding);
  tex_tmp->Draw("same");

  tex->DrawLatex(0.19,0.78,"Barrel");
  tex->DrawLatex(0.47,0.78,"Endcap"); //0.42
  tex->DrawLatex(0.73,0.78,"Forward");

  tex->DrawLatex(0.21,0.73,"BB");
  tex->DrawLatex(0.43,0.73,"EC1");
  tex->DrawLatex(0.57,0.73,"EC2");
  tex->DrawLatex(0.77,0.73,"HF");

  return c1;
}

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
    

  string alias = JetInfo(algo).getAlias().Data();

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
// Move to mu-based mapping, which is better for comparing
// different PU scenarios as it considers both IT and OOT PU,
// plus we have a number directly comparable to ATLAS
double rhoFromMu(double mu) {
  // Eta_0.0-1.3, jt320
  return (1.01272 + 0.551183*mu + 0.000362936*mu*mu);
}

//______________________________________________________________________________
// Use this for solving ptmeas from ptgen = JEC(ptmeas) * ptmeas
Double_t fJECPt(Double_t *x, Double_t *p) {

  double ptmeas = x[0];
  double eta = p[0];
  double jeta = p[1];
  double rho = p[2];

  _jec->setJetPt(ptmeas);
  _jec->setJetEta(eta);
  _jec->setJetA(jeta);
  _jec->setRho(rho);

  double jec = _jec->getCorrection();
  
  return (ptmeas * jec);
}

//______________________________________________________________________________
// Response as a function of pTprime (instead of JEC vs pTmeas)
double getResp(TF1 *_jecpt, double ptgen, double eta, double jeta, double mu) {

  _jecpt->SetParameters(eta, jeta, rhoFromMu(mu));
  double ptmeas = _jecpt->GetX(ptgen, 1, 4000);
  double resp = ptmeas / _jecpt->Eval(ptmeas); // 1/jec

  return resp;
}

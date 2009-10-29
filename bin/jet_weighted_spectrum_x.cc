////////////////////////////////////////////////////////////////////////////////
//
// jet_weighted_spectrum_x
// -----------------------
//
//            04/25/2009 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/RootStyle.h"

#include <TApplication.h>
#include <TStyle.h>
#include <TFile.h>
#include <TDirectory.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TColor.h>
#include <THStack.h>
#include <TH1F.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>


using namespace std;


////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv)
{
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;
  
  vector<string> inputs   = cl.getVector<string>("inputs");
  string         datapath = cl.getValue <string>("datapath",        "");
  string         algorithm= cl.getValue <string>("algorithm","kt4calo");
  bool           logx     = cl.getValue <bool>  ("logx",         false);
  bool           logy     = cl.getValue <bool>  ("logy",          true);
  bool           batch    = cl.getValue <bool>  ("batch",        false);
  vector<string> formats  = cl.getVector<string>("formats",         "");

  if(!cl.check()) return 0;
  cl.print();
  
  argc = (batch) ? 2 : 1; if (batch) argv[1] = (char*)"-b";
  TApplication* app = new TApplication("jet_weighted_spectrum_x",&argc,argv);

  //TH1::SetDefaultSumw2();
  set_root_style();
  gStyle->SetOptStat(0);
  TColor::SetPalette(1,0);
  
  TCanvas* cRefPt  = new TCanvas("RefPt" ,"RefPt",   0,0,790,600);
  TCanvas* cRefPtW = new TCanvas("RefPtW","RefPtW",800,0,790,600);

  TLegend* leg = new TLegend(0.835,0.96,1.0,0.145);
  leg->SetLineColor(10);
  leg->SetFillColor(10);
  leg->SetBorderSize(0);

  THStack* stRefPt  = new THStack("RefPt", "");
  THStack* stRefPtW = new THStack("RefPtW","");

  for (unsigned int i=0;i<inputs.size();i++) {
    size_t pos      = inputs[i].find(":");
    string sample   = inputs[i].substr(0,pos);
    string filename = sample + ".root";

    float  weight   = 1.0;
    if (pos!=string::npos) {
      stringstream ss;ss<<inputs[i].substr(pos+1); ss>>weight;
    }

    // DEBUG
    cout<<"filename="<<filename<<", weight="<<weight<<endl;
    
    TFile* file = new TFile((datapath+"/"+filename).c_str(),"READ");
    if (!file->IsOpen()) {
      cout<<"Can't open file "<<filename<<endl;
      continue;
    }

    TDirectory* dir = (TDirectory*)file->Get(algorithm.c_str());
    if (0==dir) {
      cout<<"file "<<filename<<" does not contain dir "<<algorithm<<endl;
      continue;
    }

    TTree* tree = (TTree*)dir->Get("t");
    if (0==tree) {
      cout<<"dir "<<algorithm<<" does not contain tree t"<<endl;
      continue;
    }
    
    TH1F* hRefPt  = new TH1F("RefPt", "",1000,0,4500);
    TH1F* hRefPtW = new TH1F("RefPtW","",1000,0,4500);
    
    weight /= tree->GetEntries();
    stringstream wsel; wsel<<weight<<"*(1)";

    tree->Project("RefPt", "refpt");
    tree->Project("RefPtW","refpt",wsel.str().c_str());
    
    Color_t color = TColor::GetColorPalette(i);

    hRefPt->SetLineWidth(1);
    hRefPt->SetLineColor(color);
    hRefPt->SetFillColor(color);
    hRefPt->SetFillStyle(1001);

    hRefPtW->SetLineWidth(1);
    hRefPtW->SetLineColor(color);
    hRefPtW->SetFillColor(color);
    hRefPtW->SetFillStyle(1001);
    
    leg->AddEntry(hRefPt,sample.c_str(),"f");
    
    stRefPt ->Add(hRefPt);
    stRefPtW->Add(hRefPtW);
  }

  cRefPt->cd();
  gPad->SetLeftMargin(0.1);
  gPad->SetRightMargin(0.17);
  stRefPt->Draw();
  stRefPt->GetXaxis()->SetTitle("p_{T}^{GEN} [GeV]");
  stRefPt->SetMinimum(9.E-01);
  if (logx) gPad->SetLogx();
  if (logy) gPad->SetLogy();
  leg->Clone()->Draw();
  cRefPt->Update();

  cRefPtW->cd();
  gPad->SetLeftMargin(0.1);
  gPad->SetRightMargin(0.17);
  stRefPtW->Draw();
  stRefPtW->GetXaxis()->SetTitle("p_{T}^{GEN} [GeV]");
  stRefPtW->SetMinimum(9E-11);
  stRefPtW->SetMaximum(5.E09);
  if (logx) gPad->SetLogx();
  if (logy) gPad->SetLogy();
  leg->Clone()->Draw();
  cRefPtW->Update();

  if (!batch) app->Run();
  
  return 0;
}



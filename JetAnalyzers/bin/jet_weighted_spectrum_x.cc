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
  
  vector<string> inputs    = cl.getVector<string>("inputs");
  string         datapath  = cl.getValue <string>("datapath",               "");
  string         algorithm = cl.getValue <string>("algorithm",       "ak5calo");
  string         selection = cl.getValue <string>("selection",             "1");
  string         variable  = cl.getValue <string>("variable",          "refpt");
  string         xtitle    = cl.getValue <string>("xtitle","p_{T}^{GEN} [GeV]");
  int            nbinsx    = cl.getValue <int>   ("nbinsx",               1000);
  double         xmin      = cl.getValue <double>("xmin",                  0.0);
  double         xmax      = cl.getValue <double>("xmax",               3500.0);
  bool           logx      = cl.getValue <bool>  ("logx",                false);
  bool           logy      = cl.getValue <bool>  ("logy",                 true);
  bool           batch     = cl.getValue <bool>  ("batch",               false);
  vector<string> formats   = cl.getVector<string>("formats",                "");
  
  if(!cl.check()) return 0;
  cl.print();
  
  if (datapath.empty()) datapath=".";
  
  argc = (batch) ? 2 : 1; if (batch) argv[1] = (char*)"-b";
  TApplication* app = new TApplication("jet_weighted_spectrum_x",&argc,argv);
  
  //TH1::SetDefaultSumw2();
  set_root_style();
  gStyle->SetOptStat(0);
  TColor::SetPalette(1,0);
  
  TLegend* leg = new TLegend(0.835,0.96,1.0,0.145);
  leg->SetLineColor(10);
  leg->SetFillColor(10);
  leg->SetBorderSize(0);
  
  THStack* st  = new THStack("st", "");
  THStack* stW = new THStack("stW","");
  
  for (unsigned i=0;i<inputs.size();i++) {
    size_t pos      = inputs[i].find(":");
    string sample   = inputs[i].substr(0,pos);
    string filename = sample + ".root";
    float  weight   = 1.0;
    if (pos!=string::npos){stringstream ss;ss<<inputs[i].substr(pos+1);ss>>weight;}
    
    TFile* file = new TFile((datapath+"/"+filename).c_str(),"READ");
    if (!file->IsOpen()) { cout<<"Can't open file "<<filename<<endl; continue; }
    
    TDirectory* dir = (TDirectory*)file->Get(algorithm.c_str());
    if (0==dir) { cout<<"No dir "<<algorithm<<"found in "<<filename<<endl;continue;}
    
    TTree* tree = (TTree*)dir->Get("t");
    if (0==tree) {cout<<"No tree 't' in dir "<<algorithm<<endl; continue; }
    
    cout<<"filename="<<filename<<", "<<tree->GetEntries()<<" events, xsec="<<weight
	<<endl;
    
    string htitle = ";"+xtitle;
    TH1F* h  = new TH1F("h", htitle.c_str(),nbinsx,xmin,xmax);
    TH1F* hW = new TH1F("hW",htitle.c_str(),nbinsx,xmin,xmax);
    
    weight /= tree->GetEntries();
    stringstream wsel; wsel<<weight<<"*("<<selection<<")";

    tree->Project("h", variable.c_str());
    tree->Project("hW",variable.c_str(),wsel.str().c_str());
    
    Color_t color = TColor::GetColorPalette(i);

    h->SetLineWidth(1);
    h->SetLineColor(color);
    h->SetFillColor(color);
    h->SetFillStyle(1001);

    hW->SetLineWidth(1);
    hW->SetLineColor(color);
    hW->SetFillColor(color);
    hW->SetFillStyle(1001);
    
    leg->AddEntry(h,sample.c_str(),"f");
    
    st ->Add(h);
    stW->Add(hW);
  }

  TCanvas* c = new TCanvas(variable.c_str(),variable.c_str(),0,0,790,600);
  c->cd();
  gPad->SetLeftMargin(0.1);
  gPad->SetRightMargin(0.17);
  st->Draw();
  st->GetXaxis()->SetTitle(xtitle.c_str());
  st->SetMinimum(9.E-01);
  if (logx) gPad->SetLogx();
  if (logy) gPad->SetLogy();
  leg->Clone()->Draw();
  c->Update();
  
  TCanvas* cW = new TCanvas((variable+"W").c_str(),
			    (variable+"W").c_str(),800,0,790,600);
  cW->cd();
  gPad->SetLeftMargin(0.1);
  gPad->SetRightMargin(0.17);
  stW->Draw();
  stW->GetXaxis()->SetTitle(xtitle.c_str());
  stW->SetMinimum(9E-11);
  stW->SetMaximum(5.E09);
  if (logx) gPad->SetLogx();
  if (logy) gPad->SetLogy();
  leg->Clone()->Draw();
  cW->Update();

  if (!batch) app->Run();
  
  return 0;
}



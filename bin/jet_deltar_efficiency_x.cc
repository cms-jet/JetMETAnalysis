////////////////////////////////////////////////////////////////////////////////
//
// jet_deltar_efficiency_x
// -----------------------
//
//            05/11/2009 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/RootStyle.h"

#include <TApplication.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TColor.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TTree.h>
#include <TH1F.h>
#include <TF1.h>
#include <TKey.h>

#include <iostream>
#include <sstream>
#include <string>
#include <cmath>


using namespace std;


////////////////////////////////////////////////////////////////////////////////
// define local functions
////////////////////////////////////////////////////////////////////////////////

/// set binomial errors for efficiency from two historgrams
void setBinomialErrors(TH1F* hEff,const TH1F* hEnum, const TH1F* hDenom);

/// transform the alg label into a title, e.g.: kt4calo -> k_{T}, D=0.4 (Calo)
string get_legend_title(const string& alg);


//______________________________________________________________________________
int main(int argc,char**argv)
{
  //
  // evaluate command-line / configuration file options
  //
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;

  vector<string> inputs   = cl.getVector<string>("inputs");
  vector<string> algs     = cl.getVector<string>("algs", "kt4calo");
  float          ptmin    = cl.getValue<float>  ("ptmin",      0.0);
  string         datapath = cl.getValue<string> ("datapath",    "");
  bool           logy     = cl.getValue<bool>   ("logy",      true);
  bool           batch    = cl.getValue<bool>   ("batch",    false);
  vector<string> formats  = cl.getVector<string>("formats",     "");
  
  if (!cl.check()) return 0;
  cl.print();
  
  TH1::SetDefaultSumw2();
  set_root_style();
  
  vector<TH1F*> hDeltaR;
  vector<TH1F*> hDeltaRAll;
  vector<TH1F*> hDeltaRSel;
  
  
  //
  // open input files and loop over input directories/trees (=algorithms!)
  //
  for (unsigned int ifile=0;ifile<inputs.size();ifile++) {
    
    unsigned pos    = inputs[ifile].find(":");
    string sample   = inputs[ifile].substr(0,pos);
    string filename = datapath+"/"+sample;
    if (filename.find(".root")==string::npos) filename += ".root";
    float  weight   = 1.0;
    if (pos!=string::npos){
      stringstream ss;
      ss<<inputs[ifile].substr(pos+1);
      ss>>weight;
    }
    
    // DEBUG
    cout<<"filename="<<filename<<", xsec="<<weight<<endl;
    
    
    TFile* file = new TFile(filename.c_str(),"READ");
    if (!file->IsOpen()) {
      cout<<"Can't open "<<filename<<endl;
      continue;
    }
    
    for (unsigned int ialg=0;ialg<algs.size();ialg++) {
      
      string alg = algs[ialg];

      TDirectory* dir = (TDirectory*)file->Get(alg.c_str());
      if (0==dir) {
	cout<<"file "<<filename<<" does not contain dir "<<alg<<endl;
	continue;
      }
      
      TTree* tree = (TTree*)dir->Get("t");
      if (0==tree) {
	cout<<"dir "<<alg<<" does not contain tree t"<<endl;
	continue;
      }
      
      if (ialg==0&&weight!=1.0) weight /= tree->GetEntries();

      cout<<alg<<" ... "<<flush;      

      unsigned char nref(0);
      float refdrjt[100];
      float refpt[100];
      tree->SetBranchAddress("nref",     &nref);
      tree->SetBranchAddress("refdrjt",refdrjt);
      tree->SetBranchAddress("refpt",    refpt);

      if (ifile==0) {
	hDeltaR.push_back(new TH1F(("DeltaR_"+alg).c_str(),"",100,0,1));
	hDeltaR.back()->SetXTitle("#Delta R");
	hDeltaRAll.push_back(new TH1F(("DeltaRAll_"+alg).c_str(),"",100,0,1));
	hDeltaRSel.push_back(new TH1F(("DeltaRSel_"+alg).c_str(),"",100,0,1));
      }
      
      unsigned int nevt = (unsigned int)tree->GetEntries();
      for (unsigned int ievt=0;ievt<nevt;ievt++) {
	tree->GetEntry(ievt);
	for (unsigned int iref=0;iref<nref;iref++) {
	  if (refpt[iref]<ptmin) continue;
	  for (unsigned int idrmax=0;idrmax<100;idrmax++) {
	    float drmax = idrmax*0.01+0.005;
	    hDeltaRAll[ialg]->Fill(drmax,weight);
	    if (refdrjt[iref]<drmax) hDeltaRSel[ialg]->Fill(drmax,weight);
	  }
	}
      }
      
      stringstream wsel;wsel<<weight<<"*(refpt>"<<ptmin<<")";
      TH1F* hDeltaRTmp = new TH1F("DeltaRTmp","",100,0.,1.0);
      tree->Project("DeltaRTmp","refdrjt",wsel.str().c_str());
      hDeltaR[ialg]->Add(hDeltaRTmp);
      delete hDeltaRTmp;
      
      cout<<" DONE."<<endl;
    }
  }
  
  
  argc= (batch) ? 2 : 1; if (batch) argv[1] = "-b";
  TApplication* app = new TApplication("jet_deltar_efficiency_x",&argc,argv);

  TLatex tex; tex.SetNDC();
  tex.SetTextSize(0.0375);
  stringstream ssptmin;
  ssptmin<<"p_{T}^{REF} > "<<ptmin<<" GeV";

  gStyle->SetOptStat(0);
  TColor::SetPalette(1,0);
  Color_t colors[7] = {kRed,kBlue,kMagenta,kCyan,kGreen+1,kBlue+4,kRed+4};
  
  // draw results
  TCanvas* cDeltaR = new TCanvas("DeltaR","DeltaR",0,0,700,700);
  cDeltaR->cd();
  if (logy) gPad->SetLogy();
  TLegend* legDeltaR = new TLegend(0.5,0.92,0.9,0.92-algs.size()*0.065);
  legDeltaR->SetLineColor(10);
  legDeltaR->SetFillColor(10);
  legDeltaR->SetShadowColor(10);
  for (unsigned int i=0;i<algs.size();i++) {
    legDeltaR->AddEntry(hDeltaR[i],get_legend_title(algs[i]).c_str(),"l");
    string drawopt = (i==0) ? "H" : "HSAME";
    hDeltaR[i]->SetLineColor(colors[i]);
    hDeltaR[i]->Draw(drawopt.c_str());
    cout<<hDeltaR[i]->Integral()<<" / "<<hDeltaR[i]->GetEntries()<<endl;
  }
  legDeltaR->Draw();
  tex.DrawLatex(0.2,0.2,ssptmin.str().c_str());


  TCanvas* cEffVsDeltaR = new TCanvas("EffVsDeltaR","EffVsDeltaR",700,0,700,700);
  cEffVsDeltaR->cd();
  //if (logy) gPad->SetLogy();
  gPad->SetLeftMargin(0.2);
  TLegend* legEffVsDeltaR = new TLegend(0.5,0.2,0.9,0.2+algs.size()*0.065);
  legEffVsDeltaR->SetLineColor(10);
  legEffVsDeltaR->SetFillColor(10);
  legEffVsDeltaR->SetShadowColor(10);
  for (unsigned int i=0;i<algs.size();i++) {
    
    TH1F* hEffVsDeltaR = new TH1F(("EffVsDeltaR_"+algs[i]).c_str(),"",100,0.,1.);
    hEffVsDeltaR->Divide(hDeltaRSel[i],hDeltaRAll[i],1.0,1.0,"B");
    setBinomialErrors(hEffVsDeltaR,hDeltaRSel[i],hDeltaRAll[i]);
    hEffVsDeltaR->SetXTitle("#Delta R_{max}");
    hEffVsDeltaR->SetYTitle("Matching Efficiency");
    hEffVsDeltaR->GetYaxis()->CenterTitle();
    
    legEffVsDeltaR->AddEntry(hEffVsDeltaR,get_legend_title(algs[i]).c_str(),"l");

    string alg=algs[i];
    double min(0.0),max(1.0);
    if      (alg.find("pf")  !=string::npos) { min=0.05; max=0.2; }
    else if (alg.find("calo")!=string::npos) { min=0.15; max=0.3; }
    
    TF1* fEffVsDeltaR = new TF1(("fitEffVsDeltaR_"+algs[i]).c_str(),"pol3",min,max);
    
    fEffVsDeltaR->SetLineColor(colors[i]);
    fEffVsDeltaR->SetNpx(200);
    hEffVsDeltaR->Fit(fEffVsDeltaR,"QR0");
    
    cout<<algs[i]<<": dR(85%) = "<<fEffVsDeltaR->GetX(0.85)<<endl;
    
    string drawopt = (i==0) ? "E" : "ESAME";
    hEffVsDeltaR->SetLineColor(colors[i]);
    hEffVsDeltaR->Draw(drawopt.c_str());
    fEffVsDeltaR->Draw("SAME");
  }
  legEffVsDeltaR->Draw();
  tex.DrawLatex(0.25,0.875,ssptmin.str().c_str());

  for (unsigned int i=0;i<formats.size();i++) {
    cDeltaR->Print((string(cDeltaR->GetName())+"."+formats[i]).c_str());
    cEffVsDeltaR->Print((string(cEffVsDeltaR->GetName())+"."+formats[i]).c_str());
  }
  
  
  // run application
  if (!batch) app->Run();

  return 0;
}



////////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void setBinomialErrors(TH1F* hEff,const TH1F* hEnum, const TH1F* hDenom)
{
  for (int i=1;i<=hEff->GetNbinsX();i++) {
    float nenum =hEnum ->GetBinContent(i);
    float ndenom=hDenom->GetBinContent(i);
    float eeff=(ndenom>0.0) ? std::sqrt(nenum/(ndenom*ndenom)*(1-nenum/ndenom)):0.0;
    hEff->SetBinError(i,eeff);
  }
}


//______________________________________________________________________________
string get_legend_title(const string& alg)
{
  string title;
  string tmp(alg);
  if      (alg.find("kt")==0) { title = "k_{T}, R=";      tmp = tmp.substr(2); }
  else if (alg.find("sc")==0) { title = "SISCone, R=";    tmp = tmp.substr(2); }
  else if (alg.find("ic")==0) { title = "ItCone, R=";     tmp = tmp.substr(2); }
  else if (alg.find("mc")==0) { title = "MidCone, R=";    tmp = tmp.substr(2); }
  else if (alg.find("ca")==0) { title = "Cam/Aachen, R="; tmp = tmp.substr(2); }
  else if (alg.find("ak")==0) { title = "Anti k_{T}, R="; tmp = tmp.substr(2); }
  else if (alg.find("gk")==0) { title = "Gen k_{T},  R="; tmp = tmp.substr(2); }
  
  assert(!title.empty());
  
  string            reco[4] = { "calo","pf","trk","jpt" };
  string            RECO[4] = { "(Calo)", "(PFlow)", "(Tracks)", "(JPT)" };

  string::size_type pos=string::npos; int ireco=-1;
  while (pos==string::npos&&ireco<3) { pos = tmp.find(reco[++ireco]); }
  assert(pos!=string::npos);
  
  double jet_size; stringstream ss1; ss1<<tmp.substr(0,pos); ss1>>jet_size;
  jet_size/=10.0;  stringstream ss2; ss2<<jet_size;
  title += ss2.str() + " " + RECO[ireco];

  return title;
}

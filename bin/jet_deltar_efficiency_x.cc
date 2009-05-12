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
#include <TTree.h>
#include <TH1F.h>
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

/// check if a vector of strings contains a certain element
bool contains(const vector<string>& collection,const string& element);


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
  string         datapath = cl.getValue<string> ("datapath",    "");
  bool           logy     = cl.getValue<bool>   ("logy",      true);

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
    string filename = sample + ".root";
    float  weight   = 1.0;
    if (pos!=string::npos){
      stringstream ss;
      ss<<inputs[ifile].substr(pos+1);
      ss>>weight;
    }
    
    // DEBUG
    cout<<"filename="<<filename<<", weight="<<weight<<endl;
    
    
    TFile* file = new TFile((datapath+"/"+filename).c_str(),"READ");
    if (!file->IsOpen()) {
      cout<<"Can't open "<<datapath<<"/"<<filename<<endl;
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
      
      cout<<alg<<" ... "<<flush;      

      unsigned char nref(0);
      float refdrjt[100];
      tree->SetBranchAddress("nref",     &nref);
      tree->SetBranchAddress("refdrjt",refdrjt);

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
	  for (unsigned int idrmax=0;idrmax<100;idrmax++) {
	    float drmax = idrmax*0.01+0.005;
	    hDeltaRAll[ialg]->Fill(drmax,weight);
	    if (refdrjt[iref]<drmax) hDeltaRSel[ialg]->Fill(drmax,weight);
	  }
	}
      }
      
      stringstream wsel;wsel<<weight<<"*(1)";
      TH1F* hDeltaRTmp = new TH1F("DeltaRTmp","",100,0.,1.0);
      tree->Project("DeltaRTmp","refdrjt",wsel.str().c_str());
      hDeltaR[ialg]->Add(hDeltaRTmp);
      delete hDeltaRTmp;
      
      cout<<" DONE."<<endl;
    }
  }
  
  
  argc=1;
  TApplication* app = new TApplication("jet_deltar_efficiency_x",&argc,argv);

  gStyle->SetOptStat(0);
  TColor::SetPalette(1,0);
  Color_t colors[7] = {kRed,kBlue,kMagenta,kCyan,kGreen+1,kBlue+4,kRed+4};
  
  // draw results
  TCanvas* cDeltaR = new TCanvas("DeltaR","DeltaR",800,800);
  cDeltaR->cd();
  if (logy) gPad->SetLogy();
  TLegend* legDeltaR = new TLegend(0.7,0.92,0.9,0.92-algs.size()*0.065);
  legDeltaR->SetLineColor(10);
  legDeltaR->SetFillColor(10);
  legDeltaR->SetShadowColor(10);
  for (unsigned int i=0;i<algs.size();i++) {
    legDeltaR->AddEntry(hDeltaR[i],algs[i].c_str(),"l");
    string drawopt = (i==0) ? "H" : "HSAME";
    hDeltaR[i]->SetLineColor(colors[i]);
    hDeltaR[i]->Draw(drawopt.c_str());
  }
  legDeltaR->Draw();


  TCanvas* cEffVsDeltaR = new TCanvas("EffVsDeltaR","EffVsDeltaR",800,0,800,800);
  cEffVsDeltaR->cd();
  if (logy) gPad->SetLogy();
  gPad->SetLeftMargin(0.2);
  TLegend* legEffVsDeltaR = new TLegend(0.7,0.2,0.9,0.2+algs.size()*0.065);
  legEffVsDeltaR->SetLineColor(10);
  legEffVsDeltaR->SetFillColor(10);
  legEffVsDeltaR->SetShadowColor(10);
  for (unsigned int i=0;i<algs.size();i++) {
    
    TH1F* hEffVsDeltaR = new TH1F(("EffVsDeltaR_"+algs[i]).c_str(),"",100,0.,1.);
    hEffVsDeltaR->Divide(hDeltaRSel[i],hDeltaRAll[i],1.0,1.0,"B");
    setBinomialErrors(hEffVsDeltaR,hDeltaRSel[i],hDeltaRAll[i]);
    hEffVsDeltaR->SetXTitle("#Delta R_{max}");
    hEffVsDeltaR->SetYTitle("Matching Efficiency");
    
    legEffVsDeltaR->AddEntry(hEffVsDeltaR,algs[i].c_str(),"l");
    
    string drawopt = (i==0) ? "E" : "ESAME";
    hEffVsDeltaR->SetLineColor(colors[i]);
    hEffVsDeltaR->Draw(drawopt.c_str());
  }
  legEffVsDeltaR->Draw();
  

  // run application
  app->Run();

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
bool contains(const vector<string>& collection,const string& element)
{
  vector<string>::const_iterator it;
  for (it=collection.begin();it!=collection.end();++it)
    if ((*it)==element) return true;
  return false;
}

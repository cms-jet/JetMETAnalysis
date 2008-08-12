////////////////////////////////////////////////////////////////////////////////
//
// jet_inspect_jra_histos_x
// ------------------------
//
//            07/05/2008 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/HistogramLoader.h"
#include "JetMETAnalysis/JetUtilities/interface/RootStyle.h"

#include <TApplication.h>
#include <TStyle.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TF1.h>
#include <TText.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>


using namespace std;


////////////////////////////////////////////////////////////////////////////////
// declare local functions
////////////////////////////////////////////////////////////////////////////////
void set_xaxis_range(TH1* h);
void set_draw_attributes(TH1* h);


////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char** argv)
{
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0
			      ;
  string input      = cl.getValue<string>("input");
  string algorithm  = cl.getValue<string>("algorithm",    "kt4calo");
  string variable   = cl.getValue<string>("variable","AbsRsp:RefPt");
  int    npercanvas = cl.getValue<int>   ("npercanvas",           0);
  bool   logx       = cl.getValue<bool>  ("logx",             false);
  bool   logy       = cl.getValue<bool>  ("logy",             false);

  if (!cl.check()) return 0;
  cl.print();
  
  TFile* file=new TFile(input.c_str(),"READ");
  if (!file->IsOpen()) { cout<<"Can't open "<<input<<endl; return 0; }

  TDirectory* dir =(TDirectory*)file->Get(algorithm.c_str());
  if (0==dir) { cout<<"No dir "<<algorithm<<" found"<<endl; return 0; }

  HistogramLoader hl;
  hl.load_histograms(dir,variable);
  
  argc=1;
  TApplication* app=new TApplication("inspect_calib_histos",&argc,argv);
  
  set_root_style();
  gStyle->SetOptStat(0);
  
  if (npercanvas==0) npercanvas=hl.nhistograms(hl.nvariables()-1);
  int nx=(int)std::sqrt((float)npercanvas);
  int ny=nx;
  if (nx*ny<npercanvas) nx++;
  if (nx*ny<npercanvas) ny++;
  
  vector<TCanvas*> cvec;
  hl.begin_loop();
  vector<unsigned int> indices; TH1F* h(0); unsigned int ihisto(0);
  while ((h=hl.next_histogram(indices))) {
    
    if (cvec.size()==0||ihisto%npercanvas==0) {
      stringstream ss;ss<<algorithm<<"_"<<cvec.size();
      cvec.push_back(new TCanvas(ss.str().c_str(),ss.str().c_str()));
      cvec.back()->Divide(nx,ny,1e-04,1e-04);
    }
    
    cvec.back()->cd(ihisto%npercanvas+1);
    if (logx) gPad->SetLogx();
    if (logy) gPad->SetLogy();
    set_xaxis_range(h);
    set_draw_attributes(h);
    h->Draw("EH");
    ihisto++;
  }

  app->Run();
  
  return 0;
}


////////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void set_xaxis_range(TH1* h)
{
  if (h->GetNbinsX()<=100) return;
  int imin=-1; int imax=-1;
  for (int i=1;i<h->GetNbinsX();i++) {
    double bc = h->GetBinContent(i);
    if (bc>0) {
      if (imin==-1) imin=i;
      imax=i;
    }
  }
  h->GetXaxis()->SetRange(imin,imax);
}


//______________________________________________________________________________
void set_draw_attributes(TH1* h)
{
  TF1* fitfnc = h->GetFunction("fit");
  if (0==fitfnc) return;
  fitfnc->SetLineWidth(2);
  fitfnc->SetLineColor(kRed);
}

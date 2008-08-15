////////////////////////////////////////////////////////////////////////////////
//
// jet_l3_correction_x
// -------------------
//
//            08/08/2008 Kostas Kousouris                    <kkousour@fnal.gov>
//                       Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/HistogramLoader.h"
#include "JetMETAnalysis/JetUtilities/interface/RootStyle.h"


#include <TApplication.h>
#include <TFile.h>
#include <TKey.h>
#include <TH1F.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TCanvas.h>


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>


using namespace std;


////////////////////////////////////////////////////////////////////////////////
// define local functions
////////////////////////////////////////////////////////////////////////////////

/// check if a vector of strings contains a certain element
bool   contains(const vector<string>& collection,const string& element);


////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv)
{
  //
  // evaluate command-line / configuration file options
  // 
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;

  string         input   = cl.getValue<string> ("input");
  vector<string> formats = cl.getVector<string>("formats", "");
  vector<string> algs    = cl.getVector<string>("algs",    "");
  bool           batch   = cl.getValue<bool>   ("batch",false);

  if (!cl.check()) return 0;
  cl.print();
  

  //
  // run a tapp if not in batch mode
  //
  argc = (batch) ? 2 : 1; if (batch) argv[1] = "-b";
  TApplication* app = new TApplication("jet_l3_correction_x",&argc,argv);
  set_root_style();

  
  //
  // open input file and loop over directories (algorithms)
  //
  TFile* ifile = new TFile(input.c_str(),"READ");
  if (!ifile->IsOpen()) { cout<<"Can't open "<<input<<endl; return 0; }

  if (algs.size()==0) {
    TIter nextDir(ifile->GetListOfKeys());
    TKey* dirKey(0);
    while ((dirKey=(TKey*)nextDir())) {
      if (strcmp(dirKey->GetClassName(),"TDirectoryFile")!=0) continue;
      algs.push_back(dirKey->GetName());
    }
  }
  
  TIter nextDir(ifile->GetListOfKeys());
  TKey* dirKey(0);
  while ((dirKey=(TKey*)nextDir())) {
    if (strcmp(dirKey->GetClassName(),"TDirectoryFile")!=0) continue;
    TDirectoryFile* idir = (TDirectoryFile*)dirKey->ReadObj();
    string alg(idir->GetName()); if (!contains(algs,alg)) continue;
    
    cout<<alg<<" ... "<<flush;
    
    TGraphErrors* grsp = new TGraphErrors();
    TGraphErrors* gcor = new TGraphErrors();
    
    grsp->SetName("L3RspVsRefPt");
    gcor->SetName("L3CorVsJetPt");
    
    HistogramLoader hl_absrsp;
    hl_absrsp.load_histograms(idir,"AbsRsp_Barrel:RefPt");
    
    HistogramLoader hl_refpt;
    hl_refpt.load_histograms(idir,"RefPt_Barrel:RefPt");
    
    HistogramLoader hl_jetpt;
    hl_jetpt.load_histograms(idir,"JetPt_Barrel:RefPt");
        
    vector<unsigned int> indices; TH1F* habsrsp(0);
    hl_absrsp.begin_loop();
    while ((habsrsp=hl_absrsp.next_histogram(indices))) {
      if (habsrsp->Integral()==0) continue;
      
      TF1*  fabsrsp = habsrsp->GetFunction("fit");
      TH1F* hrefpt  = hl_refpt.histogram(indices);
      TH1F* hjetpt  = hl_jetpt.histogram(indices);

      assert(hrefpt->GetEntries()>0);
      assert(hjetpt->GetEntries()>0);

      double refpt   =hrefpt->GetMean();
      double erefpt  =hrefpt->GetMeanError();
      double jetpt   =hjetpt->GetMean();
      double ejetpt  =hjetpt->GetMeanError();

      double peak    =(fabsrsp==0)?habsrsp->GetMean():fabsrsp->GetParameter(1);
      double epeak   =(fabsrsp==0)?habsrsp->GetMeanError():fabsrsp->GetParError(1);
      
      double refptsq =refpt*refpt;
      double erefptsq=erefpt*erefpt;
      double peaksq  =peak*peak;
      double epeaksq =epeak*epeak;

      double rsp     =(refpt+peak)/refpt;
      double ersp    =std::abs(rsp-1.)*std::sqrt(epeaksq/peaksq+erefptsq/refptsq);
      double cor     =1.0/rsp;
      double ecor    =std::abs(refpt*peak)/(refpt+peak)/(refpt+peak)*
	              std::sqrt(epeaksq/peaksq+erefptsq/refptsq);

      assert(grsp->GetN()==gcor->GetN());
      
      int n = grsp->GetN();
      grsp->SetPoint     (n,refpt, rsp);
      grsp->SetPointError(n,erefpt,ersp);
      gcor->SetPoint     (n,jetpt, cor);
      gcor->SetPointError(n,ejetpt,ecor);
    }

    // response
    TCanvas* crsp = new TCanvas("crsp","crsp",0,0,700,600); crsp->cd();
    grsp->Draw("AP");
    TH1F* hrsp = grsp->GetHistogram();
    hrsp->SetXTitle("p_{T}^{REF}");
    hrsp->SetYTitle("p_{T}/p_{T}^{REF}");
    
    
    // correction
    TCanvas* ccor = new TCanvas("ccor","ccor",715,0,700,600); ccor->cd();
    gcor->Draw("AP");
    TH1F* hcor = gcor->GetHistogram();
    hcor->SetXTitle("p_{T}");
    hcor->SetYTitle("L3 correction");
    
    cout<<"DONE"<<endl;
  }
  
  
  if (!batch) app->Run();

  return 0;
}


////////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
bool contains(const vector<string>& collection,const string& element)
{
  vector<string>::const_iterator it;
  for (it=collection.begin();it!=collection.end();++it)
    if ((*it)==element) return true;
  return false;
}

////////////////////////////////////////////////////////////////////////////////
//
// jet_l2_correction_x
// -------------------
//
//            08/16/2008 Kostas Kousouris                    <kkousour@fnal.gov>
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
#include <TLatex.h>


#include <iostream>
#include <iomanip>
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

/// transform the alg label into a title, e.g.: kt4calo -> k_{T}, D=0.4 (Calo)
string get_legend_title(const string& alg);


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
  string         l3input = cl.getValue<string> ("l3input","l3.root");
  string         output  = cl.getValue<string> ("output", "l2.root");
  vector<string> formats = cl.getVector<string>("formats",       "");
  vector<string> algs    = cl.getVector<string>("algs",          "");
  bool           batch   = cl.getValue<bool>   ("batch",      false);
  bool           logx    = cl.getValue<bool>   ("logx",       false);
  bool           logy    = cl.getValue<bool>   ("logy",       false);

  if (!cl.check()) return 0;
  cl.print();

  //
  // run a tapplication if not in batch mode
  //
  argc = (batch) ? 2 : 1; if (batch) argv[1] = "-b";
  TApplication* app = new TApplication("jet_l2_correction_x",&argc,argv);
  set_root_style();
  
  
  //
  // open output file
  //
  TFile* ofile = new TFile(output.c_str(),"RECREATE");
  if (!ofile->IsOpen()) { cout<<"Can't create "<<output<<endl; return 0; }
  

  //
  // open input & l3input files and loop over directories (algorithms)
  //
  TFile* ifile = new TFile(input.c_str(),"READ");
  if (!ifile->IsOpen()) { cout<<"Can't open "<<input<<endl; return 0; }
  
  TFile* l3file = new TFile(l3input.c_str(),"READ");
  if (!l3file->IsOpen()) { cout<<"Can't open "<<l3input<<endl; return 0; }
  
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
  
    TDirectoryFile* l3dir = (TDirectoryFile)l3file->Get(alg.c_str());
    if (l3dir==0) {
      cout<<"Failed to rerieve L3 correction for "<<alg<<", skip"<<endl;
      continue;
    }
    
    TGraphErrors* gl3rsp = (TGraphErrors*)l3dir->Get("L3RspVsRefPt");
    TF1*          fl3rsp = l3dir->GetListOfFunctions()->First();
    if (0==fl3rsp) {
      cout<<"Failed to retrieve L3 correction for "<<alg<<", skip"<<endl;
      continue;
    }

    TDirectoryFile* odir = (TDirectoryFile*)ofile->mkdir(alg.c_str());
    odir->cd();
    
    HistogramLoader hl_absrsp;
    hl_absrsp.load_histograms(idir,"AbsRsp:JetEta:RefPt");

    HistogramLoader hl_refpt;
    hl_refpt.load_histograms(idir,"RefPt:JetEta:RefPt");

    HistogramLoader hl_jetpt;
    hl_jetpt.load_histograms(idir,"JetPt:JetEta:RefPt");

    //
    // absolute response/correction as a function of pT for each eta bin
    //
    vector<TGraphErrors*> vabsrsp_eta;
    vector<TGraphErrors*> vabscor_eta;
    
    vector<unsigned int> indices; TH1F* absrsp(0);
    hl_absrsp->begin_loop();
    while ((habsrsp=hl_absrsp.next_histogram(indices))) {
      if (habsrsp->Integral()==0) continue;
      
      TF1*  fabsrsp = habsrsp->GetFunction("fit");
      TH1F* hrefpt  = hl_refpt.histogram(indices);
      TH1F* hjetpt  = hl_jetpt.histogram(indices);

      assert(hrefpt->GetEntries()>0&&hjetpt->GetEntries()>0);

      double refpt   =hrefpt->GetMean();
      double erefpt  =hrefpt->GetMeanError();
      double jetpt   =hjetpt->GetMean();
      double ejetpt  =hjetpt->GetMeanError();

      Double peak    =(fabsrsp==0)?habsrsp->GetMean():fabsrsp->GetParameter(1);
      double epeak   =(fabsrsp==0)?habsrsp->GetMeanError():fabsrsp->GetParError(1);
      
      double refptsq =refpt*refpt;
      double erefptsq=erefpt*erefpt;
      double peaksq  =peak*peak;
      double epeaksq =epeak*epeak;

      double absrsp  =(refpt+peak)/refpt;
      double eabsrsp =std::abs(rsp-1.)*std::sqrt(epeaksq/peaksq+erefptsq/refptsq);
      double abscor  =1.0/rsp;
      double eabscor =std::abs(refpt*peak)/(refpt+peak)/(refpt+peak)*
	              std::sqrt(epeaksq/peaksq+erefptsq/refptsq);

      double l3jetpt =jetpt*fl3cor->Eval(jetpt);
      double         = 
      double relrsp = 

      if (indices[0]==0) {
	vabsrsp_eta.push_back(new TGraphErrors());
	vabscor_eta.push_back(new TGraphErrors());
	
	stringstream ss_suffix;
	ss_suffix<<hl_absrsp.minimum(0,indices[0])<<"to"
		 <<hl_absrsp.maximum(0,indices[0]);
	
	vabsrsp_eta.back()->SetName(("AbsRspVsRefPt_JetEta"+
				     ss_suffix.str()).c_str());
	vabscor_eta.back()->SetName(("AbsCorVsJetPt_JetEta"+
				     ss_suffix.str()).c_str());
      }
      
      assert(vabsrsp_eta.back()->GetN()==vabscor_eta.back()->GetN());

      int n = vabsrsp_eta.back()->GetN();
      vabsrsp_eta.back()->SetPoint     (n,refpt, absrsp);
      vabsrsp_eta.back()->SetPointError(n,erefpt,eabsrsp);
      vabscor_eta.back()->SetPoint     (n,jetpt, abscor);
      vabscor_eta.back()->SetPointError(n,ejetpt,eabscor);      
    }
    
    
    //
    // fit absolute correction as a function of pT in each eta bin
    //
    for (unsigned int ieta=0;ieta<vcor_eta.size();ieta++) {
      TGraphErrors* gabscor = vabscor_eta[ieta];
      TF1*          fabscor(0);

      double xmin = gabscor->GetX()[0];
      double xmax = gabscor->GetX()[gabscor->GetN()-1];
      
      if (gabscor->GetN()==0) {
	gabscor->SetPoint     (0, 10.0,1.0);
	gabscor->SetPointError(0,  0.0,0.0);
	gabscor->SetPoint     (1,100.0,1.0);
	gabscor->SetPointError(1,  0.0,0.0);
	fabscor = new TF1("fit","[0]",10.0,100.0);
      }
      else if (gabscor->GetN()<10) {
	fabscor = new TF1("fit","[0]+[1]*log10(x)+[2]*pow(log10(x),2)",xmin,xmax);
	fabscor->SetParameter(0,0.0);
	fabscor->SetParameter(1,0.0);
	fabscor->SetParameter(2,0.0);

      }
      else {
	fabscor = new TF1("fit","[0]-[1]/(pow(log10(x),[2])+[3])+[4]/x",xmin,xmax);
	fabscor->SetParameter(0,0.0);
	fabscor->SetParameter(1,0.0);
	fabscor->SetParameter(2,0.0);
	fabscor->SetParameter(3,0.0);
	fabscor->SetParameter(4,0.0);
      }
      
      gabscor->Fit(fabscor,"QR0");
      gabscor->GetListOfFunctions()->First()->ResetBit(kNotDraw);
    }
    

    //
    // relative (L2) response/correction as a function of pT for each eta bin
    //
    vector<TGraphErrors*> vabsrsp_eta;
    vector<TGraphErrors*> vabscor_eta;

    HistogramLoader hl_jetpt;
    hl_jetpt.load_histograms(idir,"JetPt:RefPt");
    
    TH1F* hjetpt(0);
    hl_jetpt.begin_loop();
    while ((hjetpt=hl_jetpt.next_histogram(indices))) {
      if (hjetpt->Integral()==0) continue;
      
      unsigned int ieta = indices[0];
      
      double jetpt   =hjetpt->GetMean();
      double ejetpt  =hjetpt->GetMeanError();
      
      double l3jetpt = 
    }
    

    cout<<"DONE"<<endl;
  }

  
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


//______________________________________________________________________________
string get_legend_title(const string& alg)
{
  string title;
  string tmp(alg);
  if      (alg.find("kt")==0) { title = "k_{T}, D=";      tmp = tmp.substr(2); }
  else if (alg.find("sc")==0) { title = "SISCone, R=";    tmp = tmp.substr(2); }
  else if (alg.find("ic")==0) { title = "ItCone, R=";     tmp = tmp.substr(2); }
  else if (alg.find("mc")==0) { title = "MidCone. R=";    tmp = tmp.substr(2); }
  else if (alg.find("ca")==0) { title = "Cam/Aachen, D="; tmp = tmp.substr(2); }
  else if (alg.find("ak")==0) { title = "Anti k_{T}, D="; tmp = tmp.substr(2); }
  
  assert(!title.empty());
  
  string            reco[4] = { "calo","pf","trk","jpt" };
  string            RECO[4] = { "(Calo)", "(PFlow)", "(Tracks)", "(Jet+Tracks)" };

  string::size_type pos=string::npos; int ireco=-1;
  while (pos==string::npos&&ireco<3) { pos = tmp.find(reco[++ireco]); }
  assert(pos!=string::npos);
  
  double jet_size; stringstream ss1; ss1<<tmp.substr(0,pos); ss1>>jet_size;
  jet_size/=10.0;  stringstream ss2; ss2<<jet_size;
  title += ss2.str() + " " + RECO[ireco];

  return title;
}

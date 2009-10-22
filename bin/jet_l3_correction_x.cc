////////////////////////////////////////////////////////////////////////////////
//
// jet_l3_correction_x
// -------------------
//
//            08/08/2008 Kostas Kousouris                    <kkousour@fnal.gov>
//                       Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/ObjectLoader.h"
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
  string         output  = cl.getValue<string> ("output","l3.root");
  string         tag     = cl.getValue<string> ("tag",          "");
  vector<string> formats = cl.getVector<string>("formats",      "");
  vector<string> algs    = cl.getVector<string>("algs",         "");
  bool           batch   = cl.getValue<bool>   ("batch",     false);
  bool           logx    = cl.getValue<bool>   ("logx",      false);
  bool           logy    = cl.getValue<bool>   ("logy",      false);
    
  if (!cl.check()) return 0;
  cl.print();
  

  //
  // run a tapplication if not in batch mode
  //
  argc = (batch) ? 2 : 1; if (batch) argv[1] = (char*)"-b";
  TApplication* app = new TApplication("jet_l3_correction_x",&argc,argv);
  set_root_style();

  
  //
  // open output file
  //
  TFile* ofile = new TFile(output.c_str(),"RECREATE");
  if (!ofile->IsOpen()) { cout<<"Can't create "<<output<<endl; return 0; }
  

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
  
    TDirectoryFile* odir = (TDirectoryFile*)ofile->mkdir(alg.c_str());
    odir->cd();
    
    TGraphErrors* grsp = new TGraphErrors();
    TGraphErrors* gcor = new TGraphErrors();
    
    grsp->SetName("L3RspVsRefPt");
    gcor->SetName("L3CorVsJetPt");
    
    ObjectLoader<TH1F> hl_absrsp;
    hl_absrsp.load_objects(idir,"AbsRsp_Barrel:RefPt");
    
    ObjectLoader<TH1F> hl_refpt;
    hl_refpt.load_objects(idir,"RefPt_Barrel:RefPt");
    
    ObjectLoader<TH1F> hl_jetpt;
    hl_jetpt.load_objects(idir,"JetPt_Barrel:RefPt");
        
    vector<unsigned int> indices; TH1F* habsrsp(0);
    hl_absrsp.begin_loop();
    while ((habsrsp=hl_absrsp.next_object(indices))) {
      if (habsrsp->Integral()==0) continue;
      
      TF1*  fabsrsp = habsrsp->GetFunction("fit");
      TH1F* hrefpt  = hl_refpt.object(indices);
      TH1F* hjetpt  = hl_jetpt.object(indices);

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

    
    TLatex tex;
    tex.SetNDC(true);
    tex.SetTextAlign(12);
    tex.SetTextFont(22);
    tex.SetTextSize(0.06);
    

    // response
    TF1* fitrsp;
    if ((int)alg.find("pf")>0) {
      fitrsp = new TF1("fitrsp","[0]-[1]/(pow(log10(x),2)+[2])-[3]*exp(-[4]*(log10(x)-[5])*(log10(x)-[5]))",
			  1.0,grsp->GetX()[grsp->GetN()-1]);
      fitrsp->SetParameter(0,1.0);
      fitrsp->SetParameter(1,1.0);
      fitrsp->SetParameter(2,1.0);
      fitrsp->SetParameter(3,1.0);
      fitrsp->SetParameter(4,1.0);
      fitrsp->SetParameter(5,1.0);
    }
    else {
    fitrsp = new TF1("fitrsp","[0]-[1]/(pow(log10(x),[2])+[3])+[4]/x",
			  1.0,grsp->GetX()[grsp->GetN()-1]);
    fitrsp->SetParameter(0,1.0);
    fitrsp->SetParameter(1,1.0);
    fitrsp->SetParameter(2,1.0);
    fitrsp->SetParameter(3,1.0);
    fitrsp->SetParameter(4,1.0);
    }
    fitrsp->SetLineWidth(2);
    grsp->Fit(fitrsp,"QR");
    
    string cname_rsp = string(grsp->GetName())+"_"+alg;
    TCanvas* crsp = new TCanvas(cname_rsp.c_str(),cname_rsp.c_str(),0,0,700,600);
    crsp->cd();
    gPad->SetLeftMargin(0.2);
    gPad->SetRightMargin(0.05);
    gPad->SetTopMargin(0.05);
    gPad->SetBottomMargin(0.15);
    if (logx) gPad->SetLogx();
    if (logy) gPad->SetLogy();
    
    grsp->Draw("AP");
    TH1F* hrsp = grsp->GetHistogram();
    hrsp->SetXTitle("p_{T}^{REF}");
    hrsp->SetYTitle("p_{T}/p_{T}^{REF}");
    grsp->SetLineWidth(2);
    grsp->SetMarkerStyle(kFullCircle);

    tex.DrawLatex(0.6,0.3,get_legend_title(alg).c_str());
    
    grsp->Write();

    for (unsigned int iformat=0;iformat<formats.size();iformat++)
      crsp->Print((string(crsp->GetName())+"."+formats[iformat]).c_str());
    
    
    // correction
    TF1* fitcor;
    if ((int)alg.find("pf")>0) {
      fitcor = new TF1("fitcor","[0]+[1]/(pow(log10(x),2)+[2])+[3]*exp(-[4]*(log10(x)-[5])*(log10(x)-[5]))",
	  5.0/* KK */,gcor->GetX()[gcor->GetN()-1]);
      fitcor->SetParameter(0,1.0);
      fitcor->SetParameter(1,1.0);
      fitcor->SetParameter(2,1.0);
      fitcor->SetParameter(3,1.0); 
      fitcor->SetParameter(4,1.0);
      fitcor->SetParameter(5,1.0);
    }
    else {
      fitcor = new TF1("fitcor","[0]+[1]/(pow(log10(x),[2])+[3])",
			  2.0/* KK */,gcor->GetX()[gcor->GetN()-1]);
      fitcor->SetParameter(0,1.0);
      fitcor->SetParameter(1,7.0);
      fitcor->SetParameter(2,4.0);
      fitcor->SetParameter(3,4.0);       
    }
    fitcor->SetLineWidth(2);
    gcor->Fit(fitcor,"QR");
    
    string cname_cor = string(gcor->GetName())+"_"+alg;
    TCanvas* ccor = new TCanvas(cname_cor.c_str(),cname_cor.c_str(),715,0,700,600);
    ccor->cd();
    gPad->SetLeftMargin(0.2);
    gPad->SetRightMargin(0.05);
    gPad->SetTopMargin(0.05);
    gPad->SetBottomMargin(0.15);
    if (logx) gPad->SetLogx();
    if (logy) gPad->SetLogy();
    
    gcor->Draw("AP");
    TH1F* hcor = gcor->GetHistogram();
    hcor->SetXTitle("p_{T}");
    hcor->SetYTitle("L3 correction");
    gcor->SetLineWidth(2);
    gcor->SetMarkerStyle(kFullCircle);
    
    tex.DrawLatex(0.6,0.8,get_legend_title(alg).c_str());

    gcor->Write();
    string txtfilename = "l3_"+alg;
    if (!tag.empty())
      txtfilename+="_"+tag;
    txtfilename+=".jec";
    ofstream fout(txtfilename.c_str());
    fout.setf(ios::left);
    if ((int)alg.find("pf")>0) {
      fout<<setw(12)<<-5.191                  // eta_min
	<<setw(12)<<+5.191                  // eta_max
	<<setw(12)<<8                       // number of parameters + 2
	<<setw(12)<<4.0                     // minimum pT
	<<setw(12)<<5000.0                  // maximum pT
	<<setw(12)<<fitcor->GetParameter(0) // p0
	<<setw(12)<<fitcor->GetParameter(1) // p1
	<<setw(12)<<fitcor->GetParameter(2) // p2
	<<setw(12)<<fitcor->GetParameter(3) // p3
        <<setw(12)<<fitcor->GetParameter(4) // p4
	<<setw(12)<<fitcor->GetParameter(5);// p5
    } 
    else {
      fout<<setw(12)<<-5.191                  // eta_min
	<<setw(12)<<+5.191                  // eta_max
	<<setw(12)<<6                       // number of parameters + 2
	<<setw(12)<<4.0                     // minimum pT
	<<setw(12)<<5000.0                  // maximum pT
	<<setw(12)<<fitcor->GetParameter(0) // p0
	<<setw(12)<<fitcor->GetParameter(1) // p1
	<<setw(12)<<fitcor->GetParameter(2) // p2
	<<setw(12)<<fitcor->GetParameter(3);// p3
    }
    fout.close();
    
    for (unsigned int iformat=0;iformat<formats.size();iformat++)
      ccor->Print((string(ccor->GetName())+"."+formats[iformat]).c_str());
    

    cout<<"DONE"<<endl;
  }
  

  //
  // close output file
  //
  cout<<"Write "<<output<<" ... "<<flush;
  ofile->Close();
  delete ofile;
  ifile->Close();
  delete ifile;
  cout<<"DONE"<<endl;
  

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
  else return alg;
  
  string reco[5] = { "gen",  "calo",   "pf",      "trk",      "jpt" };
  string RECO[5] = { "(Gen)","(Calo)", "(PFlow)", "(Tracks)", "(JPT)" };

  string::size_type pos=string::npos; int ireco=-1;
  while (pos==string::npos&&ireco<4) { pos = tmp.find(reco[++ireco]); }
  if (pos==string::npos) return alg;
  
  double jet_size; stringstream ss1; ss1<<tmp.substr(0,pos); ss1>>jet_size;
  jet_size/=10.0;  stringstream ss2; ss2<<jet_size;

  title += ss2.str() + " " + RECO[ireco];

  return title;
}

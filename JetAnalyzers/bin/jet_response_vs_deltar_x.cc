////////////////////////////////////////////////////////////////////////////////
//
// jet_response_vs_deltar_x
// ------------------------
//
//            05/15/2009 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
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
#include <TMultiGraph.h>
#include <TGraphErrors.h>
#include <TH1F.h>
#include <TF1.h>
#include <TKey.h>

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>


using namespace std;


////////////////////////////////////////////////////////////////////////////////
// define local functions
////////////////////////////////////////////////////////////////////////////////

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
  vector<string> algs     = cl.getVector<string>("algs","kt4calol2l3");
  string         datapath = cl.getValue<string> ("datapath",       "");
  int            nbinsdr  = cl.getValue<int>    ("nbinsdr",        25);
  double         drmin    = cl.getValue<double> ("drmin",        0.05);
  double         drmax    = cl.getValue<double> ("drmax",         0.5);
  int            nbinsrsp = cl.getValue<int>    ("nbinsrsp",       50);
  double         rspmin   = cl.getValue<double> ("rspmin",        0.0);
  double         rspmax   = cl.getValue<double> ("rspmax",        2.0);
  double         ptmin    = cl.getValue<double> ("ptmin",        30.0);
  double         ptmax    = cl.getValue<double> ("ptmax",        40.0);
  double         etamin   = cl.getValue<double> ("etamin",       -1.3);
  double         etamax   = cl.getValue<double> ("etamax",        1.3);
  bool           batch    = cl.getValue<bool>   ("batch",       false);
  vector<string> formats  = cl.getVector<string>("formats",        "");
  
  if (!cl.check()) return 0;
  cl.print();
  
  TH1::SetDefaultSumw2();
  set_root_style();
  
  vector<TH1F**> hRspVsDeltaR;
  
  
  //
  // open input files and loop over input directories/trees (=algorithms!)
  //
  for (unsigned int ifile=0;ifile<inputs.size();ifile++) {
    
    size_t pos      = inputs[ifile].find(":");
    string sample   = inputs[ifile].substr(0,pos);
    string filename = datapath+"/"+sample;
    if (filename.find(".root")==string::npos) filename += ".root";
    float  weight   = 1.0;
    if (pos!=string::npos){
      stringstream ss;
      ss<<inputs[ifile].substr(pos+1);
      ss>>weight;
    }
    cout<<"filename="<<filename<<", weight="<<weight<<endl;
    
    
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
      
      cout<<alg<<" ... "<<flush;      

      unsigned char nref(0);
      vector<Float_t>* refpt;
      vector<Float_t>* refeta;
      vector<Float_t>* refdrjt;
      vector<Float_t>* jtpt;
      tree->SetBranchAddress("nref",      &nref);
      tree->SetBranchAddress("refpt",    &refpt);
      tree->SetBranchAddress("refeta",  &refeta);
      tree->SetBranchAddress("refdrjt",&refdrjt);
      tree->SetBranchAddress("jtpt",      &jtpt);
      
      if (ifile==0) {
	hRspVsDeltaR.push_back(new TH1F*[nbinsdr]);
	for (int i=0;i<nbinsdr;i++) {
	  double drcut = drmin+i*(drmax-drmin)/(nbinsdr-1);
	  stringstream sshname;
	  sshname<<"Rsp_"<<algs[ialg]<<"_dr0"<<drcut*100;
	  hRspVsDeltaR.back()[i] = new TH1F(sshname.str().c_str(),"",
					    nbinsrsp,rspmin,rspmax);
	}
      }
      
      
      // fill histograms
      unsigned int nevt = (unsigned int)tree->GetEntries();
      for (unsigned int ievt=0;ievt<nevt;ievt++) {
	tree->GetEntry(ievt);
	for (unsigned int iref=0;iref<nref;iref++) {
	  if (refpt->at(iref)<ptmin||refpt->at(iref)>ptmax) continue;
	  if (refeta->at(iref)<etamin||refeta->at(iref)>etamax) continue;
	  for (int idr=0;idr<nbinsdr;idr++) {
	    double drcut = drmin+idr*(drmax-drmin)/(nbinsdr-1);
	    if (refdrjt->at(iref)>drcut) continue;
	    hRspVsDeltaR[ialg][idr]->Fill(jtpt->at(iref)/refpt->at(iref),weight);
	  }
	}
      }
      
      cout<<" DONE."<<endl;
    }
  }
  
  
  // instantiate root application object, enable batch mode if requested
  argc= (batch) ? 2 : 1; if (batch) argv[1] = (char*)"-b";
  TApplication* app = new TApplication("jet_response_vs_deltar_x",&argc,argv);
  
  
  gStyle->SetOptStat(0);
  TColor::SetPalette(1,0);
  Color_t colors[6]  = {kRed,kBlue,kMagenta,kCyan,kGreen+1,kBlue+4};
  Style_t markers[6] = {kOpenCircle,kOpenSquare,kOpenTriangleUp,27,28,30};

  // plot results, compare several algorithms
  int nx = (int)std::sqrt(nbinsdr);
  int ny = nx;
  if (nx*ny<nbinsdr) nx++;
  if (nx*ny<nbinsdr) ny++;
  
  TCanvas** cRsp = new TCanvas*[algs.size()];

  TCanvas* cRspMean  = new TCanvas("RspMean", "RspMean",   0,  0,375,375);
  TCanvas* cRspMu    = new TCanvas("RspMu",   "RspMu",   400,  0,375,375);
  TCanvas* cResRMS   = new TCanvas("ResRMS",  "ResRMS",    0,400,375,375);
  TCanvas* cResSigma = new TCanvas("ResSigma","ResSigma",400,400,375,375);

  stringstream sspteta;
  sspteta<<ptmin<<" GeV<p_{T}<"<<ptmax<<" GeV     "<<etamin<<"<#eta<"<<etamax;
  TLatex texpteta; texpteta.SetNDC();
  
  cRspMean ->cd();
  gPad->SetLeftMargin(0.2);
  gPad->SetTopMargin(0.08);
  texpteta.DrawLatex(0.25,0.95,sspteta.str().c_str());
  
  cRspMu   ->cd();
  gPad->SetLeftMargin(0.2);
  gPad->SetTopMargin(0.08);
  texpteta.DrawLatex(0.25,0.95,sspteta.str().c_str());

  cResRMS  ->cd();
  gPad->SetLeftMargin(0.2);
  gPad->SetTopMargin(0.08);
  texpteta.DrawLatex(0.25,0.95,sspteta.str().c_str());
    
  cResSigma->cd();
  gPad->SetLeftMargin(0.2);
  gPad->SetTopMargin(0.08);
  texpteta.DrawLatex(0.25,0.95,sspteta.str().c_str());
  
  TLegend* legRspMean  = new TLegend(0.4,0.2,0.9,0.2+algs.size()*0.065);
  TLegend* legRspMu    = new TLegend(0.4,0.2,0.9,0.2+algs.size()*0.065);
  TLegend* legResRMS   = new TLegend(0.4,0.9,0.9,0.9-algs.size()*0.065);
  TLegend* legResSigma = new TLegend(0.4,0.9,0.9,0.9-algs.size()*0.065);
  
  legRspMean->SetLineColor(10);
  legRspMean->SetFillColor(10);
  legRspMean->SetShadowColor(10);
  
  legRspMu->SetLineColor(10);
  legRspMu->SetFillColor(10);
  legRspMu->SetShadowColor(10);
  
  legResRMS->SetLineColor(10);
  legResRMS->SetFillColor(10);
  legResRMS->SetShadowColor(10);
  
  legResSigma->SetLineColor(10);
  legResSigma->SetFillColor(10);
  legResSigma->SetShadowColor(10);
  
  TMultiGraph* mgRspMean  = new TMultiGraph();
  TMultiGraph* mgRspMu    = new TMultiGraph();
  TMultiGraph* mgResRMS   = new TMultiGraph();
  TMultiGraph* mgResSigma = new TMultiGraph();
  
  for (unsigned int ialg=0;ialg<algs.size();ialg++) {
    string cname = "Rsp_"+algs[ialg];
    cRsp[ialg] = new TCanvas(cname.c_str(),cname.c_str(),800,ialg*30,750,750);
    cRsp[ialg]->Divide(nx,ny);
    
    TGraphErrors* gRspMean  = new TGraphErrors(0);
    TGraphErrors* gRspMu    = new TGraphErrors(0);
    TGraphErrors* gResRMS   = new TGraphErrors(0);
    TGraphErrors* gResSigma = new TGraphErrors(0);
    
    mgRspMean ->Add(gRspMean);
    mgRspMu   ->Add(gRspMu);
    mgResRMS  ->Add(gResRMS);
    mgResSigma->Add(gResSigma);
    
    gRspMean->SetMarkerStyle(markers[ialg]);
    gRspMean->SetMarkerColor(colors[ialg]);
    gRspMean->SetLineColor(colors[ialg]);

    gRspMu->SetMarkerStyle(markers[ialg]);
    gRspMu->SetMarkerColor(colors[ialg]);
    gRspMu->SetLineColor(colors[ialg]);

    gResRMS->SetMarkerStyle(markers[ialg]);
    gResRMS->SetMarkerColor(colors[ialg]);
    gResRMS->SetLineColor(colors[ialg]);

    gResSigma->SetMarkerStyle(markers[ialg]);
    gResSigma->SetMarkerColor(colors[ialg]);
    gResSigma->SetLineColor(colors[ialg]);
    
    
    for (int idr=0;idr<nbinsdr;idr++) {
      double drcut = drmin+idr*(drmax-drmin)/(nbinsdr-1);
      TH1F* hRsp = hRspVsDeltaR[ialg][idr];
      hRsp->SetXTitle("p_{T}/p_{T}^{REF}");
      
      TF1*  fRsp = new TF1("fRsp","gaus",
			   hRsp->GetMean()-2.5*hRsp->GetRMS(),
			   hRsp->GetMean()+2.5*hRsp->GetRMS());
      fRsp->SetLineColor(colors[ialg]);
      hRsp->Fit(fRsp,"QR0");

      cRsp[ialg]->cd(idr+1);
      hRsp->Draw();
      fRsp->Draw("SAME");
      
      stringstream ss; ss<<"#DeltaR<"<<drcut;
      TLatex tex;
      tex.SetTextSize(0.09);
      tex.SetNDC();
      tex.DrawLatex(0.18,0.85,ss.str().c_str());

      gRspMean ->SetPoint     (idr,drcut,hRsp->GetMean());
      gRspMean ->SetPointError(idr,  0.0,hRsp->GetMeanError());
      gRspMu   ->SetPoint     (idr,drcut,fRsp->GetParameter(1));
      gRspMu   ->SetPointError(idr,  0.0,fRsp->GetParError(1));
      gResRMS  ->SetPoint     (idr,drcut,hRsp->GetRMS()/hRsp->GetMean());
      gResRMS  ->SetPointError(idr,  0.0,hRsp->GetRMSError()/hRsp->GetMean());
      gResSigma->SetPoint   (idr,drcut,fRsp->GetParameter(2)/fRsp->GetParameter(1));
      gResSigma->SetPointError(idr, 0.0,fRsp->GetParError(2)/fRsp->GetParameter(1));
    } 
    
    legRspMean ->AddEntry(gRspMean, get_legend_title(algs[ialg]).c_str(),"L");
    legRspMu   ->AddEntry(gRspMu,   get_legend_title(algs[ialg]).c_str(),"L");
    legResRMS  ->AddEntry(gResRMS,  get_legend_title(algs[ialg]).c_str(),"L");
    legResSigma->AddEntry(gResSigma,get_legend_title(algs[ialg]).c_str(),"L");
  }

  cRspMean ->cd(); mgRspMean ->Draw("AP"); legRspMean ->Draw();
  TH1F* hRspMean  = mgRspMean ->GetHistogram();  
  cRspMu   ->cd(); mgRspMu   ->Draw("AP"); legRspMu   ->Draw();
  TH1F* hRspMu    = mgRspMu   ->GetHistogram();
  cResRMS  ->cd(); mgResRMS  ->Draw("AP"); legResRMS  ->Draw();
  TH1F* hResRMS   = mgResRMS  ->GetHistogram();
  cResSigma->cd(); mgResSigma->Draw("AP"); legResSigma->Draw();
  TH1F* hResSigma = mgResSigma->GetHistogram();
  
  hRspMean ->SetXTitle("#DeltaR_{max}");
  hRspMu   ->SetXTitle("#DeltaR_{max}");
  hResRMS  ->SetXTitle("#DeltaR_{max}");
  hResSigma->SetXTitle("#DeltaR_{max}");

  hRspMean ->SetYTitle("<p_{T}/p_{T}^{REF}>");
  hRspMu   ->SetYTitle("#mu(p_{T}/p_{T}^{REF})");
  hResRMS  ->SetYTitle("RMS(p_{T}/p_{T}^{REF})/<p_{T}/p_{T}^{REF}>");
  hResSigma->SetYTitle("#sigma(p_{T}/p_{T}^{REF})/<p_{T}/p_{T}^{REF}>");

  hRspMean ->SetMinimum(0.8);   hRspMean ->SetMaximum(1.2);
  hRspMu   ->SetMinimum(0.8);   hRspMu   ->SetMaximum(1.2);
  hResRMS  ->SetMinimum(0.1);   hResRMS  ->SetMaximum(0.6);
  hResSigma->SetMinimum(0.1);   hResSigma->SetMaximum(0.6);

  cRspMean ->Modified();
  cRspMu   ->Modified();
  cResRMS  ->Modified();
  cResSigma->Modified();

  for (unsigned int iformat=0;iformat<formats.size();iformat++) {
    cRspMean ->Print((string(cRspMean ->GetName())+"."+formats[iformat]).c_str());
    cRspMu   ->Print((string(cRspMu   ->GetName())+"."+formats[iformat]).c_str());
    cResRMS  ->Print((string(cResRMS  ->GetName())+"."+formats[iformat]).c_str());
    cResSigma->Print((string(cResSigma->GetName())+"."+formats[iformat]).c_str());
  }
  
  
  // run application
  if (!batch) app->Run();
  
  return 0;
}



////////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

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
  
  string reco[5] = { "gen",   "calo",   "pf",      "trk",      "jpt" };
  string RECO[5] = { "(Gen)", "(Calo)", "(PFlow)", "(Tracks)", "(JPT)" };

  size_t pos=string::npos; int ireco=-1;
  while (pos==string::npos&&ireco<4) { pos = tmp.find(reco[++ireco]); }
  if (pos==string::npos) return alg;
  
  double jet_size; stringstream ss1; ss1<<tmp.substr(0,pos); ss1>>jet_size;
  jet_size/=10.0;  stringstream ss2; ss2<<jet_size;

  title += ss2.str() + " " + RECO[ireco];

  return title;
}

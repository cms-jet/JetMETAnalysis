////////////////////////////////////////////////////////////////////////////////
//
// jet_inspect_profiles_x
// -------------------------
//
//            03/30/2010 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/RootStyle.h"

#include <TApplication.h>
#include <TStyle.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TLatex.h>
#include <TH1F.h>
#include <TF1.h>
#include <TText.h>
#include <TLine.h>
#include <TLegend.h>

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
int main(int argc,char** argv)
{
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;

  vector<string> inputs     = cl.getVector<string>("inputs");
  vector<string> algs       = cl.getVector<string>("algs",              "ak5pf");
  vector<string> selections = cl.getVector<string>("selections",             "");
  string         varexpr    = cl.getValue <string>("varexpr","jtpt/refpt:jteta");
  string         treename   = cl.getValue <string>("treename",              "t");
  int            nbinsx     = cl.getValue <int>   ("nbinsx",                 50);
  double         xmin       = cl.getValue <double>("xmin",                 -5.0);
  double         xmax       = cl.getValue <double>("xmax",                  5.0);
  bool           logx       = cl.getValue<bool>   ("logx",                false);
  bool           logy       = cl.getValue<bool>   ("logy",                false);
  vector<string> labels     = cl.getVector<string>("labels",                 "");
  string         output     = cl.getValue<string> ("output",          "profile");
  vector<string> formats    = cl.getVector<string>("formats",                "");
  bool           batch      = cl.getValue<bool>   ("batch",               false);

  if (!cl.check()) return 0;
  cl.print();

  if (prefix.empty()) prefix=algs[0];
  if (batch&&formats.size()==0) formats.push_back("pdf");
  
  argc = (batch) ? 2 : 1; if (batch) argv[1] = (char*)"-b";
  TApplication* app=new TApplication("jet_inspect_profiles_x",&argc,argv);
  
  set_root_style();
  
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  
  vector<TProfile*> profiles;
  
  
  /// LOOP OVER FILES
  for (unsigned int ifile=0;ifile<inputs.size();ifile++) {
    
    string input(inputs[ifile]);
    TFile* file=new TFile(input.c_str(),"READ");
    if (!file->IsOpen()) {cout<<"Can't open "<<file->GetName()<<endl;return 0;}
    
    /// LOOP OVER ALGORITHMS
    for (unsigned int ialg=0;ialg<algs.size();ialg++) {
      
      string alg = algs[ialg];
      TDirectory* dir =(TDirectory*)file->Get(alg.c_str());
      if (0==dir) { cout<<"No dir '"<<alg<<"' found"<<endl; return 0; }
      
      TTree* tree=(TTree*)dir->Get(treename.c_str());
      if (0==tree) { cout<<"No tree '">>treename<<"' for "<<alg<<endl; return 0; }
      
      /// LOOP OVER SELECTIONS
      for (unsigned int isel=0;isel<selections.size();isel++) {
	
      } // selections
    } // algorithms
  } // inputs
  
  TCanvas* c=new TCanvas(output.c_str(),output.c_str(),0,0,600,600);
  for (unsigned iprof=0;iprof<profiles.size();iprof++) {
    
  }

  for (unsigned int ifmt=0;ifmt<formats.size();ifmt++)
    c->Print((output+"."+formats[ifmt]).c_str());
  
  if (!batch) app->Run();
  
  return 0;
}


////////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void set_xaxis_range(TH1* h1,TH1* h2)
{
  if (h1->GetEntries()==0) return;
  int binmin,binmax;
  get_xaxis_range(h1,binmin,binmax);
  if (0!=h2) {
    int binmin2,binmax2;
    get_xaxis_range(h2,binmin2,binmax2);
    binmin = std::min(binmin,binmin2);
    binmax = std::max(binmax,binmax2);
  }
  h1->GetXaxis()->SetRange(binmin,binmax);
}


//______________________________________________________________________________
void get_xaxis_range(TH1* h,int& binmin,int &binmax)
{
  binmin=-1; binmax=-1;
  for (int i=1;i<h->GetNbinsX();i++) {
    double bc = h->GetBinContent(i);
    if (bc>0) {
      if (binmin==-1) binmin=i;
      binmax=i;
    }
  }
}


//______________________________________________________________________________
void set_draw_attributes(TH1* h,unsigned index,bool fill)
{
  if (fill) {
    Style_t fillstyle = (index==0) ? 3002 : 3001;
    h->SetFillColor(h->GetLineColor());
    h->SetFillStyle(fillstyle);
  }
  TF1* fitfnc = h->GetFunction("fit");
  if (0!=fitfnc) {
    fitfnc->SetLineWidth(2);
    fitfnc->SetLineColor(h->GetLineColor());
  }
}


//______________________________________________________________________________
void draw_stats(TH1* h,double xoffset,Color_t color,Color_t fitColor)
{
  TF1* fitfnc = h->GetFunction("fit");
  stringstream ssentries;
  ssentries<<setw(6) <<setiosflags(ios::left)<<"N:"
	   <<setw(10)<<resetiosflags(ios::left)<<setprecision(4)<<h->GetEntries();
  
  stringstream ssmean;
  ssmean<<setw(6)<<setiosflags(ios::left)<<"Mean:"
	<<setw(9)<<resetiosflags(ios::left)<<setprecision(4)<<h->GetMean();

  stringstream ssrms;
  ssrms<<setw(6)<<setiosflags(ios::left)<<"RMS:"
       <<setw(9)<<resetiosflags(ios::left)<<setprecision(4)<<h->GetRMS();

  stringstream sspeak;
  if (0!=fitfnc)
    sspeak<<setw(6)<<setiosflags(ios::left)<<"Peak: "
	  <<setw(10)<<resetiosflags(ios::left)<<setprecision(4)
	  <<fitfnc->GetParameter(1);

  stringstream sssgma;
  if (0!=fitfnc)
    sssgma<<setw(6)<<setiosflags(ios::left)<<"Sigma:"
	  <<setw(9)<<resetiosflags(ios::left)<<setprecision(4)
	  <<fitfnc->GetParameter(2);
  
  TLatex stats;
  stats.SetNDC(true);
  stats.SetTextAlign(12);
  stats.SetTextSize(0.045);
  stats.SetTextColor(color);
  stats.SetTextFont(42);
  
  stats.DrawLatex(xoffset,0.84,ssentries.str().c_str());
  stats.DrawLatex(xoffset,0.805,ssmean.str().c_str());
  stats.DrawLatex(xoffset,0.77,ssrms.str().c_str());
  if (0!=fitfnc) {
    stats.SetTextColor(fitColor);
    stats.DrawLatex(xoffset,0.735,sspeak.str().c_str());
    stats.DrawLatex(xoffset,0.70,sssgma.str().c_str());
  }
}


//______________________________________________________________________________
void draw_range(const ObjectLoader<TH1F>& hl,
		const vector<unsigned int>& indices,
		bool  addFixedVars)
{
  TLatex range;
  range.SetNDC(true);
  range.SetTextAlign(13);
  range.SetTextSize(0.055);
  range.SetTextFont(42);

  string varnameEta = "#eta";
  for (unsigned int i=0;i<hl.nvariables();i++)
    if (hl.variable(i)=="JetEta"&&hl.minimum(i,0)>=0) varnameEta="|#eta|";
  
  stringstream ssrange;

  for (unsigned int i=0;i<hl.nvariables();i++) {
    
    if (hl.nobjects(i)==1&&!addFixedVars) continue;
    
    string varname = hl.variable(i);
    string unit    = "";
    double varmin  = hl.minimum(i,indices[i]);
    double varmax  = hl.maximum(i,indices[i]);
    
    if (varname=="RefPt")    { varname = "p_{T}^{REF}"; unit = " GeV"; }
    if (varname=="JetPt")    { varname = "p_{T}";       unit = " GeV"; }
    if (varname=="JetEta")   { varname = varnameEta;    unit =     ""; }
    if (varname=="JetPhi")   { varname = "#varphi";     unit =     ""; }
    if (varname=="PtRel")    { varname = "p_{T}^{rel}", unit = " GeV"; }
    if (varname=="RelLepPt") { varname = "p_{T}^{l}/p_{T}^{jet}",unit = ""; }

    ssrange<<varmin<<" < "<<varname<<" < "<<varmax<<unit<<"    ";
  }
  
  range.DrawLatex(0.1,0.96,ssrange.str().c_str());
}


//______________________________________________________________________________
void draw_line_mean(TH1* h)
{
  double mean = h->GetMean();
  int binMean;
  for (binMean=1;binMean<h->GetNbinsX();binMean++) {
    float binLowEdge = h->GetBinLowEdge(binMean);
    float binHighEdge = binLowEdge + h->GetBinWidth(binMean);
    if (mean>=binLowEdge&&mean<binHighEdge) break;
  }
  double xmin = mean;
  double xmax = mean;
  double ymin = h->GetMinimum();
  double ymax = h->GetBinContent(binMean);
  TLine* lineMean = new TLine(xmin,ymin,xmax,ymax);
  lineMean->SetLineColor(h->GetLineColor());
  lineMean->SetLineStyle(2);
  lineMean->SetLineWidth(3);
  lineMean->Draw("SAME");
}


//______________________________________________________________________________
void draw_line_median(TH1* h)
{
  double median;
  double prb(0.5);
  h->GetQuantiles(1,&median,&prb);
  int binMedian;
  for (binMedian=1;binMedian<h->GetNbinsX();binMedian++) {
    double binLowEdge = h->GetBinLowEdge(binMedian);
    double binHighEdge = binLowEdge + h->GetBinWidth(binMedian);
    if (median>=binLowEdge&&median<binHighEdge) break;
  }
  double xmin = median;
  double xmax = median;
  double ymin = h->GetMinimum();
  double ymax = h->GetBinContent(binMedian);
  TLine* lineMedian = new TLine(xmin,ymin,xmax,ymax);
  lineMedian->SetLineColor(h->GetLineColor());
  lineMedian->SetLineStyle(3);
  lineMedian->SetLineWidth(3);
  lineMedian->Draw("SAME");
}


//______________________________________________________________________________
void draw_line_peak(TH1* h)
{
  TF1* fitFnc = h->GetFunction("fit"); if (0==fitFnc) return;
  double peak = fitFnc->GetParameter(1);
  double xmin = peak;
  double xmax = peak;
  double ymin = h->GetMinimum();
  double ymax = fitFnc->Eval(peak);
  TLine* linePeak = new TLine(xmin,ymin,xmax,ymax);
  linePeak->SetLineColor(h->GetLineColor());
  linePeak->SetLineStyle(4);
  linePeak->SetLineWidth(3);
  linePeak->Draw("SAME");
}


//______________________________________________________________________________
void draw_line_legend(bool mean,bool median,bool peak)
{
  if (!mean&&!median&&!peak) return;
  TLegend* leg = new TLegend(0.75,0.5,0.95,0.5-(mean+median+peak)*0.055);
  leg->SetLineColor(10);
  leg->SetFillColor(10);
  leg->SetBorderSize(0);
  if (mean) {
    TLine* lMean = new TLine();
    lMean->SetLineWidth(3);
    lMean->SetLineStyle(2);
    leg->AddEntry(lMean,"mean","l");
  }
  if (median) {
    TLine* lMedian = new TLine();
    lMedian->SetLineWidth(3);
    lMedian->SetLineStyle(3);
    leg->AddEntry(lMedian,"median","l");
  }
  if (peak) {
    TLine* lPeak = new TLine();
    lPeak->SetLineWidth(3);
    lPeak->SetLineStyle(4);
    leg->AddEntry(lPeak,"peak","l");
  }
  leg->Draw();
}


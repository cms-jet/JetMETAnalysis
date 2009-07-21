////////////////////////////////////////////////////////////////////////////////
//
// jet_inspect_histos_x
// --------------------
//
//            07/19/2009 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/ObjectLoader.h"
#include "JetMETAnalysis/JetUtilities/interface/RootStyle.h"

#include <TApplication.h>
#include <TStyle.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TLatex.h>
#include <TH1F.h>
#include <TF1.h>
#include <TText.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>


using namespace std;


////////////////////////////////////////////////////////////////////////////////
// declare local functions
////////////////////////////////////////////////////////////////////////////////
void set_xaxis_range(TH1* h);
void set_draw_attributes(TH1* h,Color_t fitColor=kRed);
void draw_stats(TH1* h,double xoffset,Color_t color,Color_t fitColor);
void draw_range(const ObjectLoader<TH1F>& hl,
		const vector<unsigned int>& indices,
		bool  addFixedVars=true);


////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char** argv)
{
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;

  vector<string> inputs     = cl.getVector<string>("inputs");
  vector<string> algs       = cl.getVector<string>("algs",          "kt4calo");
  vector<string> variables  = cl.getVector<string>("variables","RelRsp:RefPt");
  int            npercanvas = cl.getValue<int>    ("npercanvas",            0);
  bool           norm       = cl.getValue<bool>   ("norm",              false);
  bool           logx       = cl.getValue<bool>   ("logx",              false);
  bool           logy       = cl.getValue<bool>   ("logy",              false);
  vector<string> formats    = cl.getVector<string>("formats",              "");
  bool           batch      = cl.getValue<bool>   ("batch",             false);

  if (!cl.check()) return 0;
  cl.print();

  bool put_range = (npercanvas!=0);
  
  argc = (batch) ? 2 : 1; if (batch) argv[1] = "-b";
  TApplication* app=new TApplication("jet_inspect_histos",&argc,argv);
  
  set_root_style();
    
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  
  vector<TCanvas*> c; int nx(1),ny(1);
  
  /// LOOP OVER FILES
  for (unsigned int ifile=0;ifile<inputs.size();ifile++) {

    string input = inputs[ifile];
    TFile* file=new TFile(input.c_str(),"READ");
    if (!file->IsOpen()) {cout<<"Can't open "<<file->GetName()<<endl;return 0;}
    
    /// LOOP OVER ALGORITHMS
    for (unsigned int ialg=0;ialg<algs.size();ialg++) {

      string alg = algs[ialg];
      TDirectory* dir =(TDirectory*)file->Get(alg.c_str());
      if (0==dir) { cout<<"No dir "<<algs[ialg]<<" found"<<endl; return 0; }

      /// LOOP OVER VARIABLES
      for (unsigned int ivar=0;ivar<variables.size();ivar++) {
	
	string variable=variables[ivar];
	
	ObjectLoader<TH1F> hl;
	hl.load_objects(dir,variable);
	
	if (ifile==0&&ialg==0&&ivar==0) {
	  if (npercanvas==0) npercanvas=hl.nobjects(hl.nvariables()-1);
	  nx=(int)std::sqrt((float)npercanvas);
	  ny=nx;
	  if (nx*ny<npercanvas) nx++;
	  if (nx*ny<npercanvas) ny++;
	}
	
	hl.begin_loop();
	vector<unsigned int> indices; TH1F* h(0); unsigned int ihisto(0);
	while ((h=hl.next_object(indices))) {
	  
	  if (norm) {
	    TF1* f = h->GetFunction("fit");
	    if (0!=f) f->SetParameter(0,f->GetParameter(0)/h->Integral());
	    h->Sumw2();
	    h->Scale(1./h->Integral());
	  }
	  
	  if (ifile==0&&ialg==0&&ivar==0&&
	      (c.size()==0||ihisto%npercanvas==0)) {
	    stringstream sscname;sscname<<alg<<"_"<<hl.quantity();
	    for (unsigned int i=0;i<hl.nvariables();i++) {
	      if (variables.size()>1&&hl.nobjects(i)==1) continue;
	      sscname<<"_"<<hl.variable(i);
	      if (i<hl.nvariables()-1||put_range)
		sscname<<hl.minimum(i,indices[i])<<"to"
		       <<hl.maximum(i,indices[i]);
	    }
	    c.push_back(new TCanvas(sscname.str().c_str(),
				    sscname.str().c_str(),
				    1000,1000));
	    c.back()->Divide(nx,ny,1e-04,1e-04);
	  }
	  
	  int icnv = ihisto/npercanvas;
	  int ipad = ihisto%npercanvas+1;
	  c[icnv]->cd(ipad);
	  
	  if (ifile==0&&ialg==0&&ivar==0) {
	    if (logx) gPad->SetLogx();
	    if (logy) gPad->SetLogy();
	    gPad->SetLeftMargin(0.1);
	    gPad->SetRightMargin(0.05);
	    gPad->SetTopMargin(0.12);
	    gPad->SetBottomMargin(0.15);
	    set_xaxis_range(h);
	    set_draw_attributes(h,kRed);
	    h->Draw("EH");
	    h->SetMaximum(1.5*h->GetMaximum());
	    if (logy) h->SetMaximum(10.*h->GetMaximum());
	    draw_stats(h,0.65,kBlack,kRed);
	    draw_range(hl,indices,(variables.size()==1));
	  }
	  else {
	    h->SetLineColor(kBlue);
	    set_draw_attributes(h,kMagenta);
	    h->Draw("ESAME");
	    draw_stats(h,0.15,kBlue,kMagenta);
	  }
	  
	  ihisto++;
	  
	} // histos
      } // variables
    } // algorithms
  } // inputs
  
  for (unsigned int icanvas=0;icanvas<c.size();icanvas++)
    for (unsigned int iformat=0;iformat<formats.size();iformat++)
      c[icanvas]->Print((string(c[icanvas]->GetName())+"."+formats[iformat]).c_str());
  
  if (!batch) app->Run();
  
  return 0;
}


////////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void set_xaxis_range(TH1* h)
{
  if (h->GetEntries()==0) return;
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
void set_draw_attributes(TH1* h,Color_t color)
{
  TF1* fitfnc = h->GetFunction("fit");
  if (0==fitfnc) return;
  fitfnc->SetLineWidth(1);
  fitfnc->SetLineColor(color);
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
    
    if (varname=="RefPt")  { varname = "p_{T}^{REF}"; unit = " GeV"; }
    if (varname=="JetPt")  { varname = "p_{T}";       unit = " GeV"; }
    if (varname=="JetEta") { varname = varnameEta;    unit =     ""; }
    if (varname=="JetPhi") { varname = "#varphi";     unit =     ""; }

    ssrange<<varmin<<" < "<<varname<<" < "<<varmax<<unit<<"    ";
  }
  
  range.DrawLatex(0.1,0.96,ssrange.str().c_str());
}

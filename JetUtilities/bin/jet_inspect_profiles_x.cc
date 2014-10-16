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
#include <TTree.h>
#include <TCut.h>
#include <TCanvas.h>
#include <TLatex.h>
#include <TProfile.h>
#include <TH1F.h>
#include <TH2F.h>
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
  vector<string> selections = cl.getVector<string>("selections",            "1");
  string         presel     = cl.getValue <string>("presel",                "1");
  string         xvariable  = cl.getValue <string>("xvariable",         "jteta");
  string         yvariable  = cl.getValue <string>("yvariable",    "jtpt/refpt");
  string         treename   = cl.getValue <string>("treename",              "t");
  int            nbinsx     = cl.getValue <int>   ("nbinsx",                 50);
  double         xmin       = cl.getValue <double>("xmin",                 -5.0);
  double         xmax       = cl.getValue <double>("xmax",                  5.0);
  int            nbinsy     = cl.getValue <int>   ("nbinsy",                 50);
  double         ymin       = cl.getValue <double>("ymin",                  0.0);
  double         ymax       = cl.getValue <double>("ymax",                  1.2);
  bool           logx       = cl.getValue<bool>   ("logx",                false);
  bool           logy       = cl.getValue<bool>   ("logy",                false);
  string         xtitle     = cl.getValue<string> ("xtitle",         "jet #eta");
  string         ytitle     = cl.getValue<string> ("ytitle","p_{T}/p_{T}^{REF}");
  vector<int>    colors     = cl.getVector<int>   ("colors",                 "");
  vector<string> labels     = cl.getVector<string>("labels",                 "");
  vector<string> hlines     = cl.getVector<string>("hlines",                 "");
  string         opath      = cl.getValue <string>("opath",                  "");
  string         output     = cl.getValue<string> ("output",          "profile");
  vector<string> formats    = cl.getVector<string>("formats",                "");
  bool           batch      = cl.getValue<bool>   ("batch",               false);

  if (!cl.check()) return 0;
  cl.print();

  if (batch&&formats.size()==0) formats.push_back("pdf");
  string varexpr = yvariable + ":" + xvariable;
  string htitle = ";" + xtitle + ";" + ytitle;
  
  TCut preselection(presel.c_str());
  
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
      if (0==tree) { cout<<"No tree '"<<treename<<"' for "<<alg<<endl; return 0; }
      
      /// LOOP OVER SELECTIONS
      for (unsigned int isel=0;isel<selections.size();isel++) {
	
	TCut selection = preselection && TCut(selections[isel].c_str());
	
	//string selection = selections[isel];
	//if (!presel.empty()) selection = presel + "&&" + selection;
	
	stringstream hname; hname<<"h2_f"<<ifile+1<<"_a"<<ialg+1<<"_s"<<isel+1;
	TH2F* h2=new TH2F(hname.str().c_str(),
			  htitle.c_str(),
			  nbinsx,xmin,xmax,
			  nbinsy,0.,3.);
	tree->Project(hname.str().c_str(),varexpr.c_str(),selection);
	profiles.push_back(h2->ProfileX());
	profiles.back()->SetTitle(htitle.c_str());
	
	
      } // SELECTIONS

    } // ALGORITHMS (DIRS)

  } // INPUTS (FILES)
  

  // DRAW PROFILES
  TCanvas* c=new TCanvas(output.c_str(),output.c_str(),0,0,600,600);
  if (logx) gPad->SetLogx();
  if (logy) gPad->SetLogy();
  for (unsigned iprof=0;iprof<profiles.size();iprof++) {
    TProfile* profile = profiles[iprof];
    string drawopt = (iprof==0) ? "" : "SAME";
    profile->Draw(drawopt.c_str());
    profile->SetLineWidth(2);
    Color_t color = (colors.size()>iprof) ? colors[iprof] : kBlack+iprof;
    profile->SetLineColor(color);
    if (iprof==0) {
      profile->SetMinimum(ymin);
      profile->SetMaximum(ymax);
    }
  }
  
  // DRAW LEGEND
  if (labels.size()==profiles.size()) {
    TLegend* leg = new TLegend(0.65,0.2,0.9,0.2+labels.size()*0.05);
    leg->SetLineColor(10);
    leg->SetFillColor(10);
    leg->SetBorderSize(0);
    for (unsigned ilabel=0;ilabel<labels.size();ilabel++)
      leg->AddEntry(profiles[ilabel],labels[ilabel].c_str(),"l");
    leg->Draw();
  }
  
  // DRAW HORIZONTAL LINES
  for (unsigned iline=0;iline<hlines.size();iline++) {
    size_t pos = hlines[iline].find(':');
    string y_as_str = hlines[iline].substr(0,pos);
    string s_as_str("1");
    if (pos!=string::npos) s_as_str = hlines[iline].substr(pos+1);
    stringstream ssy; ssy<<y_as_str; double y; ssy>>y;
    stringstream sss; sss<<s_as_str; int s; sss>>s;
    TLine* line = new TLine(profiles.front()->GetXaxis()->GetXmin(),y,
			    profiles.front()->GetXaxis()->GetXmax(),y);
    line->SetLineStyle(s);
    line->Draw("SAME");
  }
  
  // CREATE FILES
  if (!opath.empty()) output = opath + "/" + output;
  for (unsigned int ifmt=0;ifmt<formats.size();ifmt++)
    c->Print((output+"."+formats[ifmt]).c_str());
  
  if (!batch) app->Run();
  
  return 0;
}


////////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

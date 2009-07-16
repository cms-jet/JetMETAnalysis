////////////////////////////////////////////////////////////////////////////////
//
// jet_response_plotter_x
// ----------------------
//
//            08/08/2008 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/ObjectLoader.h"
#include "JetMETAnalysis/JetUtilities/interface/RootStyle.h"


#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <cmath>

#include <TApplication.h>
#include <TROOT.h>
#include <TFile.h>
#include <TKey.h>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <TGraphErrors.h>


using namespace std;


////////////////////////////////////////////////////////////////////////////////
// define local functions
////////////////////////////////////////////////////////////////////////////////

/// check if a vector of strings contains a certain element
bool   contains(const vector<string>& collection,const string& element);

/// set the xtitle of a graph
void   set_xtitle(string& xtitle, const ObjectLoader<TH1F>& hl);

/// set the draw attributes of a graph
void   set_color_and_style(TGraph* g,const string& alg);

/// add the graph to the legend if the current algorithm isn't registered
void   update_legend(TLegend* l,TGraph* g,const string& alg);

/// transform the alg label into a title, e.g.: kt4calo -> k_{T}, D=0.4 (Calo)
string get_legend_title(const string& alg);


/// bookeeping
set<string>         algs_in_rsp_legend;
set<string>         algs_in_res_legend;
queue<Color_t>      colors;
queue<Style_t>      markers;
map<string,Color_t> alg_to_color;
map<string,Style_t> alg_to_marker;


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

  string         input    = cl.getValue<string> ("input");
  string         output   = cl.getValue<string> ("output",      "jrp.root");
  vector<string> formats  = cl.getVector<string>("formats",             "");
  string         variable = cl.getValue<string> ("variable","AbsRsp:RefPt");
  vector<string> algs     = cl.getVector<string>("algs",                "");
  bool           fitres   = cl.getValue<bool>   ("fitres",            true);
  bool           logx     = cl.getValue<bool>   ("logx",             false);
  bool           logy     = cl.getValue<bool>   ("logy",             false);
  bool           batch    = cl.getValue<bool>   ("batch",            false);

  if (!cl.check()) return 0;
  cl.print();


  //
  // open output file and declare vectors for response & resolution graphs
  //
  TFile* ofile = new TFile(output.c_str(),"RECREATE");
  if (!ofile->IsOpen()) { cout<<"Can't create "<<output<<endl; return 0; }
  vector<TGraphErrors*> vrsp;
  vector<TGraphErrors*> vres;

  vector<TMultiGraph*>  vrsp_mg;
  vector<TMultiGraph*>  vres_mg;

  set_root_style();
  
  TLegend* leg_rsp(0);
  TLegend* leg_res(0);
  string   xtitle("");
  
  
  //
  // open input file and loop over input directories (=algorithms)
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

  leg_rsp = new TLegend(0.5,0.6,0.9,0.6-algs.size()*0.06);
  leg_res = new TLegend(0.5,0.9,0.9,0.9-algs.size()*0.06);
  leg_rsp->SetFillColor(10);
  leg_res->SetFillColor(10);
  
  unsigned int ialg(0);
  TIter nextDir(ifile->GetListOfKeys());
  TKey* dirKey(0);
  while ((dirKey=(TKey*)nextDir())) {
    if (strcmp(dirKey->GetClassName(),"TDirectoryFile")!=0) continue;
    TDirectoryFile* idir = (TDirectoryFile*)dirKey->ReadObj();
    string alg(idir->GetName()); if (!contains(algs,alg)) continue;
    
    cout<<alg<<" ... "<<flush;
    
    TDirectoryFile* odir = (TDirectoryFile*)ofile->mkdir(alg.c_str());
    odir->cd();
    
    ObjectLoader<TH1F> hlrsp;
    hlrsp.load_objects(idir,variable);

    string varexp=hlrsp.variable(hlrsp.nvariables()-1)+
                  variable.substr(variable.find(':'));
    
    ObjectLoader<TH1F> hlvar;
    hlvar.load_objects(idir,varexp);
    
    set_xtitle(xtitle,hlvar);
    
    vector<unsigned int> indices;
    TH1F* hrsp(0); TGraphErrors* grsp(0); TGraphErrors* gres(0);
    hlrsp.begin_loop();
    while ((hrsp=hlrsp.next_object(indices))) {
      
      // create new graphs for response & resolution
      if (indices.back()==0) {
	grsp = new TGraphErrors(0); vrsp.push_back(grsp);
	gres = new TGraphErrors(0); vres.push_back(gres);
	string grsp_name = "RspVs"+hlrsp.variable(hlrsp.nvariables()-1);
	string gres_name = "ResVs"+hlrsp.variable(hlrsp.nvariables()-1);
	if (hlrsp.nvariables()>1) {
	  for (unsigned int i=0;i<hlrsp.nvariables()-1;i++) {
	    stringstream suffix;
	    suffix<<"_"<<hlrsp.variable(i)
		  <<hlrsp.minimum(i,indices[i])<<"to"
		  <<hlrsp.maximum(i,indices[i]);
	    grsp_name += suffix.str();
	    gres_name += suffix.str();
	  }
	}
	grsp->SetName(grsp_name.c_str());
	gres->SetName(gres_name.c_str());

	set_color_and_style(grsp,alg);
	set_color_and_style(gres,alg);
	
	update_legend(leg_rsp,grsp,alg);
	update_legend(leg_res,gres,alg);
	
	if (ialg==0) {
	  vrsp_mg.push_back(new TMultiGraph());
	  vres_mg.push_back(new TMultiGraph());
	}

	vrsp_mg[indices.front()]->Add(grsp);
	vres_mg[indices.front()]->Add(gres);
      }
      
      // add new points to current response & resolution graphs
      if (hrsp->Integral()==0) continue;
      
      TF1*   frsp = hrsp->GetFunction("fit");
      TH1F*  hvar = hlvar.object(indices);

      assert(hvar->GetEntries()>0);
      
      double x  = hvar->GetMean();
      double ex = hvar->GetMeanError();
      double y  = (frsp==0) ? hrsp->GetMean()      : frsp->GetParameter(1);
      double ey = (frsp==0) ? hrsp->GetMeanError() : frsp->GetParError(1);
      double e  = (frsp==0) ? hrsp->GetRMS()       : frsp->GetParameter(2);
      double ee = (frsp==0) ? hrsp->GetRMSError()  : frsp->GetParError(2);
      
      if (hlrsp.quantity().find("AbsRsp")==0) {
	
	double yabs  = (x+y)/x;
	double eyabs = std::abs(yabs-1)*std::sqrt(ey*ey/y/y+ex*ex/x/x);
	double eabs  = e/x;
	double eeabs = eabs*std::sqrt(ee*ee/e/e+ex*ex/x/x);
	
	y  = yabs;
	ey = eyabs;
	e  = eabs;
	ee = eeabs;
      }
      else if (hlrsp.quantity().find("RelRsp")==0) {
	double erel  = e/y;
	double eerel = erel*std::sqrt(ee*ee/e/e+ey*ey/y/y);
	
	e  = erel;
	ee = eerel;
      }
      
      int n = grsp->GetN();
      grsp->SetPoint(n,x,y);
      grsp->SetPointError(n,ex,ey);
      gres->SetPoint(n,x,e);
      gres->SetPointError(n,ex,ee);
    }
    
    // fit resolution if requested
    if (fitres) {
      for (unsigned int igraph=0;igraph<vres.size();igraph++) {
	TGraphErrors* g = vres[igraph];
	double xmin(g->GetX()[0]);
	double xmax(-1e100);
	for (int ipoint=0;ipoint<g->GetN();ipoint++)
	  if (g->GetX()[ipoint]>xmax) xmax = g->GetX()[ipoint];
	TF1* fitfnc = new TF1("fit","sqrt(([0]/x)**2+[1]**2/x+[2]**2)",xmin,xmax);
	fitfnc->SetLineWidth(2);
	fitfnc->SetLineColor(g->GetLineColor());
	fitfnc->SetParameter(0,0.5);
	fitfnc->SetParameter(1,0.5);
	fitfnc->SetParameter(2,0.1);
	g->Fit(fitfnc,"QR");
      }
    }


    // write response & resolution graphs to output root file
    for (unsigned int igraph=0;igraph<vrsp.size();igraph++) vrsp[igraph]->Write();
    for (unsigned int igraph=0;igraph<vres.size();igraph++) vres[igraph]->Write();
    
    ialg++;
    
    cout<<" DONE."<<endl;
  }
  
  
  //
  // run application if not in batch mode
  //
  argc = (batch) ? 2 : 1; if (batch) argv[1] = "-b";
  TApplication* app = new TApplication("jet_response_plotter_x",&argc,argv);

  // response
  for (unsigned int img=0;img<vrsp_mg.size();img++) {
    string cname=vrsp_mg[img]->GetListOfGraphs()->First()->GetName();
    TCanvas* crsp=new TCanvas(cname.c_str(),cname.c_str(),0,0,700,600);crsp->cd();
    gPad->SetLeftMargin(0.2);
    gPad->SetRightMargin(0.05);
    gPad->SetTopMargin(0.05);
    gPad->SetBottomMargin(0.15);
    if (logx) gPad->SetLogx();
    if (logy) gPad->SetLogy();

    vrsp_mg[img]->Draw("AP");
    vrsp_mg[img]->SetMinimum(0.01);
    vrsp_mg[img]->SetMaximum(1.2);
    TH1* h = vrsp_mg[img]->GetHistogram();
    h->SetXTitle(xtitle.c_str());
    h->SetYTitle("p_{T}/p_{T}^{REF}");
    h->GetXaxis()->SetTitleOffset(1.3);
    h->GetYaxis()->SetTitleOffset(1.6);
    leg_rsp->Clone()->Draw();

    for (unsigned int iformat=0;iformat<formats.size();iformat++)
      crsp->Print((string(crsp->GetName())+"."+formats[iformat]).c_str());
  }

  // resolution
  for (unsigned int img=0;img<vres_mg.size();img++) {
    string cname=vres_mg[img]->GetListOfGraphs()->First()->GetName();
    TCanvas* cres=new TCanvas(cname.c_str(),cname.c_str(),715,0,700,600);cres->cd();
    gPad->SetLeftMargin(0.2);
    gPad->SetRightMargin(0.05);
    gPad->SetTopMargin(0.05);
    gPad->SetBottomMargin(0.15);
    if (logx) gPad->SetLogx();
    if (logy) gPad->SetLogy();

    vres_mg[img]->Draw("AP");
    vres_mg[img]->SetMinimum(0.01);
    vres_mg[img]->SetMaximum(0.5);
    TH1* h = vres_mg[img]->GetHistogram();
    h->SetXTitle(xtitle.c_str());
    h->SetYTitle("#sigma(p_{T}/p_{T}^{REF})/<p_{T}/p_{T}^{REF}>");
    h->GetXaxis()->SetTitleOffset(1.3);
    h->GetYaxis()->SetTitleOffset(1.6);
    leg_res->Clone()->Draw();

    for (unsigned int iformat=0;iformat<formats.size();iformat++)
      cres->Print((string(cres->GetName())+"."+formats[iformat]).c_str());
  }
  
  

  //
  // close input & output files
  //
  ifile->Close();
  delete ifile;

  ofile->Close();
  delete ofile;
  

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
void set_xtitle(string& xtitle, const ObjectLoader<TH1F>& hl)
{
  if (!xtitle.empty()) return;
  xtitle = hl.quantity();
  if      (xtitle=="JetPt")  xtitle = "p_{T} [GeV]";
  else if (xtitle=="RefPt")  xtitle = "p_{T}^{REF} [GeV]";
  else if (xtitle=="JetEta") xtitle = "#eta";
  else if (xtitle=="JetPhi") xtitle = "#phi";
}


//______________________________________________________________________________
void set_color_and_style(TGraph* g,const string& alg)
{
  if (colors.size()==0) {
    colors.push(kBlack);
    colors.push(kRed);
    colors.push(kGreen);
    colors.push(kBlue);
    colors.push(kCyan);
    colors.push(kMagenta+1);
    colors.push(kOrange-3);
    colors.push(kGreen+4);
    colors.push(kBlue+3);
    colors.push(kCyan+3);
    colors.push(kMagenta+4);
  }
  
  if (markers.size()==0) {
    markers.push(kOpenCircle);
    markers.push(kOpenSquare);
    markers.push(kOpenTriangleUp);
    markers.push(27);
    markers.push(28);
    markers.push(30);
    markers.push(kFullTriangleUp);
    markers.push(kFullTriangleDown);
    markers.push(kFullCircle);
    markers.push(kFullSquare);

  }
  
  g->SetLineWidth(2);
  
  Color_t color;
  map<string,Color_t>::const_iterator it_color = alg_to_color.find(alg);
  if (it_color==alg_to_color.end()) {
    color = colors.front();
    colors.pop();
    alg_to_color[alg] = color;
  }
  else {
    color = it_color->second;
  }
  g->SetLineColor(color);
  g->SetMarkerColor(color);
  
  Style_t marker;
  map<string,Style_t>::const_iterator it_marker = alg_to_marker.find(alg);
  if (it_marker==alg_to_marker.end()) {
    marker = markers.front();
    markers.pop();
    alg_to_marker[alg] = marker;
  }
  else {
    marker = it_marker->second;
  }
  g->SetMarkerStyle(marker);
}


//______________________________________________________________________________
void update_legend(TLegend* l,TGraph* g,const string& alg)
{
  string gname = g->GetName();
  set<string>& algs_in_legend =
    (gname.find("Rsp")==0) ? algs_in_rsp_legend : algs_in_res_legend;
  
  if (algs_in_legend.find(alg)==algs_in_legend.end()) {
    l->AddEntry(g,get_legend_title(alg).c_str(),"LP");
    algs_in_legend.insert(alg);
  }
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
  else if (alg.find("gk")==0) { title = "Gen k_{T}, R="; tmp = tmp.substr(2); }
  
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

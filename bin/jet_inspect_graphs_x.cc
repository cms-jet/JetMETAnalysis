////////////////////////////////////////////////////////////////////////////////
//
// jet_inspect_graphs_x
// --------------------
//
//            07/20/2009 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/ObjectLoader.h"
#include "JetMETAnalysis/JetUtilities/interface/RootStyle.h"

#include <TApplication.h>
#include <TStyle.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TMultiGraph.h>
#include <TGraphErrors.h>
#include <TH1F.h>
#include <TF1.h>
#include <TText.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <cmath>


using namespace std;


////////////////////////////////////////////////////////////////////////////////
// declare local functions
////////////////////////////////////////////////////////////////////////////////
void   draw_range(const string& range);
string get_range(const ObjectLoader<TGraphErrors>& gl,
		 const vector<unsigned int>& indices,
		 bool  addFixedVars=true);
void   draw_legend(TLegend* leg,const string& quantity);
string get_legend_title(const string& alg);
void   set_graph_style(TGraphErrors* g,unsigned int ngraph);
void   set_mg_histogram(TMultiGraph* mg,const string& quantity,bool logy);

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
  bool           logx       = cl.getValue<bool>   ("logx",              false);
  bool           logy       = cl.getValue<bool>   ("logy",              false);
  vector<string> formats    = cl.getVector<string>("formats",              "");
  bool           batch      = cl.getValue<bool>   ("batch",             false);

  if (!cl.check()) return 0;
  cl.print();

  argc = (batch) ? 2 : 1; if (batch) argv[1] = "-b";
  TApplication* app=new TApplication("jet_inspect_graphs",&argc,argv);
  
  set_root_style();
    
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  
  set<string> quantities;
  for (unsigned int i=0;i<variables.size();i++) {
    unsigned int pos = variables[i].find(':');
    quantities.insert(variables[i].substr(0,pos));
  }
  bool isRangeComparison = (variables.size()>0&&
			    quantities.size()!=variables.size());
  
   
  map<string,TMultiGraph*> mgmap;
  map<string,TLegend*>     legmap;
  map<string,string>       rngmap;
  
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
	
	ObjectLoader<TGraphErrors> gl;
	gl.load_objects(dir,variable);
	
	gl.begin_loop();
	vector<unsigned int> indices;
	TGraphErrors* g(0);
	while ((g=gl.next_object(indices))) {
	  
	  string       quantity(gl.quantity());
	  TMultiGraph* mg(0);
	  TLegend*     leg(0);
	  map<string,TMultiGraph*>::iterator it = mgmap.find(quantity);
	  if (it==mgmap.end()) {
	    stringstream sscname;sscname<<quantity;
	    for (unsigned int i=0;i<gl.nvariables();i++) {
	      if (variables.size()>1&&gl.nobjects(i)==1) continue;
	      sscname<<"_"<<gl.variable(i)
		     <<gl.minimum(i,indices[i])<<"to"
		     <<gl.maximum(i,indices[i]);
	    }
	    
	    mg=new TMultiGraph(sscname.str().c_str(),"");
	    int nleg = 
	      (inputs.size()>1) ? inputs.size() :
	      (algs.size()>1) ? algs.size() :
	      variables.size();
	    double ymax=(quantity.find("Rsp")==string::npos)?0.85:0.5;
	    leg=new TLegend(0.55,ymax,0.9,ymax-nleg*0.08);
	    mgmap [quantity]=mg;
	    legmap[quantity]=leg;
	    rngmap[quantity]=get_range(gl,indices,!isRangeComparison);
	  }
	  else {
	    mg = it->second;
	    leg=legmap[gl.quantity()];
	  }
	  
	  mg->Add(g);
	  set_graph_style(g,mg->GetListOfGraphs()->GetEntries()-1);
	  leg->AddEntry(g,get_legend_title(alg).c_str(),"lp");
	  
	} // graphs
      } // variables
    } // algorithms
  } // inputs


  vector<TCanvas*> c;
  map<string,TMultiGraph*>::iterator it;
  for (it=mgmap.begin();it!=mgmap.end();++it) {
    string       quantity(it->first);
    string       range(rngmap[quantity]);
    TMultiGraph* mg(it->second);
    TLegend*     leg(legmap[quantity]);
    c.push_back(new TCanvas(mg->GetName(),mg->GetName(),
			    c.size()*20,c.size()*20,600,600));
    gPad->SetLeftMargin(0.18);
    gPad->SetRightMargin(0.05);
    gPad->SetTopMargin(0.08);
    gPad->SetBottomMargin(0.14);
    if (logx) gPad->SetLogx();
    if (logy) gPad->SetLogy();
    mg->Draw("AP");
    draw_legend(leg,quantity);
    draw_range(range);
    set_mg_histogram(mg,quantity,logy);
  }
  

  for (unsigned int icanvas=0;icanvas<c.size();icanvas++)
    for (unsigned int iformat=0;iformat<formats.size();iformat++)
      c[icanvas]->Print((string(c[icanvas]->GetName())+"."+
			 formats[iformat]).c_str());
  
  
  if (!batch) app->Run();
  
  return 0;
}


////////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void draw_range(const string& range)
{
  TLatex tex;
  tex.SetNDC(true);
  tex.SetTextAlign(13);
  tex.SetTextSize(0.055);
  tex.SetTextFont(42);
  tex.DrawLatex(0.18,0.98,range.c_str());
}


//______________________________________________________________________________
string get_range(const ObjectLoader<TGraphErrors>& gl,
		 const vector<unsigned int>& indices,
		 bool  addFixedVars)
{
  string varnameEta = "#eta";
  for (unsigned int i=0;i<gl.nvariables();i++)
    if (gl.variable(i)=="JetEta"&&gl.minimum(i,0)>=0) varnameEta="|#eta|";
  
  stringstream ssrange;

  for (unsigned int i=0;i<gl.nvariables();i++) {
    
    if (gl.nobjects(i)==1&&!addFixedVars) continue;
    
    string varname = gl.variable(i);
    string unit    = "";
    double varmin  = gl.minimum(i,indices[i]);
    double varmax  = gl.maximum(i,indices[i]);

    if (varname=="RefPt")  { varname = "p_{T}^{REF}"; unit = " GeV"; }
    if (varname=="JetPt")  { varname = "p_{T}";       unit = " GeV"; }
    if (varname=="JetEta") { varname = varnameEta;    unit =     ""; }
    if (varname=="JetPhi") { varname = "#varphi";     unit =     ""; }

    ssrange<<varmin<<" < "<<varname<<" < "<<varmax<<unit<<"    ";
  }
  
  return ssrange.str();
}


//______________________________________________________________________________
void draw_legend(TLegend* leg,const string& quantity)
{
  
  string mode("");
  unsigned int pos = quantity.find("Vs");
  if (pos!=string::npos) {
    string str=quantity.substr(0,pos);
    if (str=="Rsp"||str=="RelRsp"||str=="AbsRsp") mode="Rsp";
    if (str=="Res"||str=="RelRes"||str=="AbsRes") mode="Res";
  }
  leg->SetFillColor(10);
  leg->Draw();
}


//______________________________________________________________________________
string get_legend_title(const string& alg)
{
  string title;
  string tmp(alg);
  if      (alg.find("kt")==0) { title = "k_{T}, D=";      tmp = tmp.substr(2); }
  else if (alg.find("sc")==0) { title = "SISCone, R=";    tmp = tmp.substr(2); }
  else if (alg.find("ic")==0) { title = "ItCone, R=";     tmp = tmp.substr(2); }
  else if (alg.find("mc")==0) { title = "MidCone, R=";    tmp = tmp.substr(2); }
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


//______________________________________________________________________________
void set_graph_style(TGraphErrors* g, unsigned int ngraph)
{
  Color_t colors[10]  = {
    kBlue+1,kRed+1,kGreen+2,kMagenta+2,kCyan+3,
    kViolet+4,kOrange+1,kGreen-1,kMagenta-4,kCyan-2
  };
  Style_t lines[10]   = {
    1,1,1,1,1,1,1,1,1,1
  };
  Style_t markers[10] = {
    kOpenCircle,kOpenSquare,kOpenTriangleUp,kOpenDiamond,kOpenCross,
    kOpenStar,kFullCircle,kFullSquare,kFullTriangleUp,kFullTriangleDown
  };
  
  Color_t color = colors[std::min(ngraph,(unsigned)9)];
  Style_t line  = lines[std::min(ngraph,(unsigned)9)];
  Style_t marker = markers[std::min(ngraph,(unsigned)9)];
  
  g->SetLineColor(color);
  g->SetMarkerColor(color);
  g->SetMarkerStyle(marker);
  
  TF1* f(0);
  if (g->GetListOfFunctions()->GetEntries()>0) {
    f=(TF1*)(g->GetListOfFunctions()->At(0));
    f->SetLineColor(color);
    f->SetLineStyle(line);
  }
  
  return;
}


//______________________________________________________________________________
void set_mg_histogram(TMultiGraph* mg,const string& quantity,bool logy)
{
  TH1* h = mg->GetHistogram();
  if (0==h) {
    cout<<"Can't aquire histogram for multi-graph "<<mg->GetName()<<endl;
    return;
  }

  string xtitle("");
  string ytitle("");
  unsigned int pos = quantity.find("Vs");
  if (pos!=string::npos) {
    string ystr=quantity.substr(0,pos);
    string xstr=quantity.substr(pos+2);

    if (ystr=="Rsp"||ystr=="RelRsp"||ystr=="AbsRsp") {
      ytitle="p_{T}/p_{T}^{REF}";
      if (logy) h->SetMinimum(0.01); else h->SetMinimum(0.0);
      h->SetMaximum(1.2);
    }
    if (ystr=="Res"||ystr=="RelRes"||ystr=="AbsRes") {
      ytitle="#sigma(p_{T}/p_{T}^{REF})/<p_{T}/p_{T}^{REF}>";
      if (logy) h->SetMinimum(0.01); else h->SetMinimum(0.0);
      h->SetMaximum(0.5);
    }
    
    if (xstr=="RefPt")  xtitle="p_{T}^{REF} [GeV]";
    if (xstr=="JetPt")  xtitle="p_{T} [GeV]";
    if (xstr=="JetEta") xtitle="#eta";
    if (xstr=="JetPhi") xtitle="#varphi";
  }
  
  h->SetXTitle(xtitle.c_str());
  h->SetYTitle(ytitle.c_str());
  h->GetXaxis()->SetTitleOffset(1.1);
  h->GetYaxis()->SetTitleOffset(1.4);
  h->GetYaxis()->CenterTitle();
  h->GetYaxis()->SetNdivisions(505);
  return;
}

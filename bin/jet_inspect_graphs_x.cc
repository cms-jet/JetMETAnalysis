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
string get_legend_label_from_alg(const string& alg);
string get_legend_label_from_input(const string& input);
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
  vector<string> algs       = cl.getVector<string>("algs",           "ak5calo");
  vector<string> variables  = cl.getVector<string>("variables","RelRspVsRefPt");
  vector<string> labels     = cl.getVector<string>("labels",                "");
  bool           logx       = cl.getValue<bool>   ("logx",               false);
  bool           logy       = cl.getValue<bool>   ("logy",               false);
  string         prefix     = cl.getValue<string> ("prefix",                "");
  vector<string> formats    = cl.getVector<string>("formats",               "");
  bool           batch      = cl.getValue<bool>   ("batch",              false);

  if (!cl.check()) return 0;
  cl.print();
  
  // sanity check
  if ((inputs.size()>1&&algs.size()>1)||
      (algs.size()>1&&variables.size()>1)||
      (inputs.size()>1&&variables.size()>1)) {
    cout<<"Provide more than one value only for one of inputs/algs/variables!"
	<<endl;
    return 0;
  }

  argc = (batch) ? 2 : 1; if (batch) argv[1] = (char*)"-b";
  TApplication* app=new TApplication("jet_inspect_graphs",&argc,argv);
  
  set_root_style();
  
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  
  // is the *same* quantity compared for several (e.g. eta-) ranges?
  set<string> quantities;
  for (unsigned int i=0;i<variables.size();i++) {
    unsigned int pos = variables[i].find(':');
    quantities.insert(variables[i].substr(0,pos));
  }
  if (quantities.size()!=1) {
    cout<<"ERROR: don't try to compare different quantities!"<<endl;
    return 0;
  }
  
  // determine legend labels
  if (algs.size()>1) {
    if (labels.size()>0) {
      if (labels.size()!=algs.size()) {
	cout<<"ERROR: labels / algs mismatch!"<<endl;
	return 0;
      }
    }
    else {
      for (unsigned int ialg=0;ialg<algs.size();ialg++)
	labels.push_back(get_legend_label_from_alg(algs[ialg]));
    }
  }
  else if (inputs.size()>1) {
    if (labels.size()>0) {
      if (labels.size()!=inputs.size()) {
	cout<<"ERROR: labels / inputs mismatch!"<<endl;
      }
    }
    else {
      for (unsigned int iinput=0;iinput<inputs.size();iinput++)
	labels.push_back(get_legend_label_from_input(inputs[iinput]));
    }
  }
  else if (variables.size()>1) {
    if (labels.size()>0) {
      if (labels.size()!=variables.size()) {
	cout<<"ERROR: labels / variables mismatch!"<<endl;
	return 0;
      }
    }
  }
  else {
    labels.push_back(get_legend_label_from_alg(algs[0]));
  }
  
  TMultiGraph* mg(0);
  TLegend*     leg(0);
  string       range;
  string       quantity=(*quantities.begin());
  
  /// LOOP OVER FILES
  for (unsigned int iinput=0;iinput<inputs.size();iinput++) {
    
    string input=inputs[iinput].substr(0,inputs[iinput].find(':'));
    TFile* file=new TFile(input.c_str(),"READ");
    if (!file->IsOpen()) {cout<<"Can't open "<<file->GetName()<<endl;return 0;}
    
    /// LOOP OVER ALGORITHMS
    for (unsigned int ialg=0;ialg<algs.size();ialg++) {

      string alg=algs[ialg];
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
	  
	  if (0==mg) {
	    stringstream sscname;
	    if (!prefix.empty()) sscname<<prefix<<"_";
	    sscname<<quantity;
	    for (unsigned int i=0;i<gl.nvariables();i++) {
	      if (variables.size()>1&&gl.nobjects(i)==1) continue; //??
	      sscname<<"_"<<gl.variable(i)
		     <<gl.minimum(i,indices[i])<<"to"
		     <<gl.maximum(i,indices[i]);
	    }
	    int nlabels=(labels.size()>0)?labels.size():variables.size();
	    mg=new TMultiGraph(sscname.str().c_str(),"");
	    double ymax=(quantity.find("Rsp")==string::npos)?0.85:0.5;
	    leg=new TLegend(0.55,ymax,0.9,ymax-nlabels*0.055);
	    range=get_range(gl,indices,variables.size()==1);
	  }
	  
	  int   ilabel=(inputs.size()>1) ? iinput:(algs.size()>1) ? ialg:ivar;
	  string label=(variables.size()>1&&labels.size()==0) ?
	    get_range(gl,indices,true) : labels[ilabel];
	  
	  mg->Add(g);
	  set_graph_style(g,mg->GetListOfGraphs()->GetEntries()-1);
	  leg->AddEntry(g,label.c_str(),"lp");

	  
	  // print fit parameters
	  TF1* fitfnc = (TF1*)g->GetFunction("fit");
	  if (0!=fitfnc) {
	    cout<<label<<":"<<endl;
	    for (int ipar=0;ipar<fitfnc->GetNpar();ipar++)
	      cout<<fitfnc->GetParameter(ipar)<<" +- "
		  <<fitfnc->GetParError(ipar)<<endl;
	  }
	  cout<<endl;
	  // end print fit parameters

	  
	} // graphs
      } // variables
    } // algorithms
  } // inputs

  if (0==mg) { cout<<"Buh!"<<endl; return 0; }
  
  vector<TCanvas*> c;
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
string get_legend_label_from_alg(const string& alg)
{
  string label;
  string tmp(alg);
  if      (alg.find("kt")==0) { label = "k_{T}, R=";      tmp = tmp.substr(2); }
  else if (alg.find("sc")==0) { label = "SISCone, R=";    tmp = tmp.substr(2); }
  else if (alg.find("ic")==0) { label = "ItCone, R=";     tmp = tmp.substr(2); }
  else if (alg.find("mc")==0) { label = "MidCone, R=";    tmp = tmp.substr(2); }
  else if (alg.find("ca")==0) { label = "Cam/Aachen, R="; tmp = tmp.substr(2); }
  else if (alg.find("ak")==0) { label = "Anti k_{T}, R="; tmp = tmp.substr(2); }
  else if (alg.find("gk")==0) { label = "Gen k_{T}, R=";  tmp = tmp.substr(2); }
  else return alg;
  
  string reco[5] = { "gen",  "calo",   "pf",      "trk",      "jpt" };
  string RECO[5] = { "(Gen)","(Calo)", "(PFlow)", "(Tracks)", "(JPT)" };

  string::size_type pos=string::npos; int ireco=-1;
  while (pos==string::npos&&ireco<4) { pos = tmp.find(reco[++ireco]); }
  if (pos==string::npos) return alg;

  double jet_size; stringstream ss1; ss1<<tmp.substr(0,pos); ss1>>jet_size;
  jet_size/=10.0;  stringstream ss2; ss2<<jet_size;
  
  label += ss2.str() + " " + RECO[ireco];
  
  return label;
}


//______________________________________________________________________________
string get_legend_label_from_input(const string& input)
{
  string label;
  string tmp(input);
  unsigned int pos=tmp.find(':');
  if (pos!=string::npos) {
    label = tmp.substr(pos+1);
  }
  else {
    pos=tmp.find_last_of('/');
    if (pos!=string::npos) tmp=tmp.substr(pos+1);
    pos=tmp.find(".root");
    if (pos!=string::npos) tmp=tmp.substr(0,pos);
    pos=tmp.find('_');
    label=tmp.substr(0,pos);
  }
  return label;
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
      //h->SetMaximum(0.5);
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

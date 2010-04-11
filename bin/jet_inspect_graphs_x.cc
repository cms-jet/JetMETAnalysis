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

#include <fstream>


using namespace std;


////////////////////////////////////////////////////////////////////////////////
// declare local functions
////////////////////////////////////////////////////////////////////////////////
void   draw_range(const string& range);
void   draw_text(const string& text);
string get_range(const ObjectLoader<TGraphErrors>& gl,
		 const vector<unsigned int>& indices,
		 bool  addFixedVars=true);
string get_legend_label_from_alg(const string& alg);
string get_legend_label_from_input(const string& input);
void   set_graph_style(TGraphErrors* g,unsigned int ngraph,bool nocolor);
void   set_axis_titles(TH1*h,const string& quantity,float ymin,float ymax);

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char** argv)
{
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;

  vector<string> inputs    = cl.getVector<string>("inputs");
  vector<string> algs      = cl.getVector<string>("algs",           "ak5calo");
  vector<string> variables = cl.getVector<string>("variables","RelRspVsRefPt");
  vector<string> labels    = cl.getVector<string>("labels",                "");
  string         text      = cl.getValue<string> ("text",                  "");
  bool           logx      = cl.getValue<bool>   ("logx",               false);
  bool           logy      = cl.getValue<bool>   ("logy",               false);
  float          ymin      = cl.getValue<float>  ("ymin",                -1.0);
  float          ymax      = cl.getValue<float>  ("ymax",                -1.0);
  bool           nocolor   = cl.getValue<bool>   ("nocolor",            false);
  bool           overlay   = cl.getValue<bool>   ("overlay",             true);
  string         prefix    = cl.getValue<string> ("prefix",                "");
  string         suffix    = cl.getValue<string> ("suffix",                "");
  string         opath     = cl.getValue<string> ("opath",                 "");
  vector<string> formats   = cl.getVector<string>("formats",               "");
  bool           batch     = cl.getValue<bool>   ("batch",              false);
  bool           latex     = cl.getValue<bool>   ("latex",              false);

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
  
  if (ymin<0.0&&logy) ymin = 0.01;

  if (batch&&formats.size()==0) formats.push_back("pdf");
  
  argc = (batch) ? 2 : 1; if (batch) argv[1] = (char*)"-b";
  TApplication* app=new TApplication("jet_inspect_graphs",&argc,argv);
  
  set_root_style();
  
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  
  // is the *same* quantity compared for several (e.g. eta-) ranges?
  set<string> quantities;
  for (unsigned int i=0;i<variables.size();i++) {
    size_t pos = variables[i].find(':');
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

  TMultiGraph*          mg(0);
  TLegend*              leg(0);
  vector<TGraphErrors*> graphs;
  vector<string>        ranges;
  string                quantity=(*quantities.begin());
  
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

	  graphs.push_back(g);
	  ranges.push_back(get_range(gl,indices,variables.size()==1));
	  
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
	    if (!suffix.empty()) sscname<<"_"<<suffix;
	    int nlabels=(labels.size()>0)?labels.size():variables.size();
	    mg=new TMultiGraph(sscname.str().c_str(),"");
	    double ymax=(quantity.find("Rsp")==string::npos)?0.85:0.4;
	    leg=new TLegend(0.5,ymax,0.9,ymax-nlabels*0.06);
	    //range=get_range(gl,indices,variables.size()==1);
	  }

	  int   ilabel=(inputs.size()>1) ? iinput:(algs.size()>1) ? ialg:ivar;
	  string label=(variables.size()>1&&labels.size()==0) ?
	    get_range(gl,indices,true) : labels[ilabel];
	  
	  mg->Add(g);
	  set_graph_style(g,overlay*(graphs.size()-1),nocolor);
	  leg->AddEntry(g,label.c_str(),"lp");

	  // print fit parameters
	  TF1* fitfnc = (TF1*)g->GetFunction("fit");
	  if (0!=fitfnc) {
	    cout<<label<<":"<<endl;
	    for (int ipar=0;ipar<fitfnc->GetNpar();ipar++)
	      cout<<fitfnc->GetParameter(ipar)<<" +- "
		  <<fitfnc->GetParError(ipar)<<endl;
	  }

	  // end print fit parameters

	  // save latex table //hh
	  
	  if (latex&&(0!=fitfnc)) {

	    ofstream texfile; stringstream texfilename; stringstream texinput;
	    
	    if (!opath.empty()) texfilename<<opath<<"/";
	    texfilename<<alg<<"_"<<mg->GetName()<<".tex";
	    texfile.open(texfilename.str().c_str(),ofstream::trunc);
	    if (!texfile.is_open()) {
	      cout<<"tex-ERROR: Could not create "<<texfilename.str()<<endl;continue;
	    }
	    texinput<<"%%fitfnc: "<<fitfnc->GetExpFormula()<<endl;

	    if (alg.find("pf")!=string::npos)texinput<<"\\pfjets & ";
	    else if (alg.find("calo")!=string::npos)texinput<<"\\calojets & ";
	    else if (alg.find("jpt")!=string::npos)texinput<<"\\jptjets & ";
	    else {
	      cout<<"tex-ERROR: Did not recognize alg "<<alg<<endl;continue;
	    }

	    for (int ipar=0;ipar<fitfnc->GetNpar()-1;ipar++) {
	      texinput<<"$ ("<<setprecision(5)<<fixed
		      <<fitfnc->GetParameter(ipar)
		      <<" $ & $ \\pm $ & "
		      <<"$ "<<setprecision(5)<<fixed
		      <<fitfnc->GetParError(ipar)<<") $ & ";
	    }
	    texinput<<"$ ("<<setprecision(5)<<fixed
		    <<fitfnc->GetParameter(fitfnc->GetNpar()-1)
		    <<" $ & $ \\pm $ & "
		    <<"$ "<<setprecision(5)<<fixed
		    <<fitfnc->GetParError(fitfnc->GetNpar()-1)<<") $";

	      texfile<<texinput.str().c_str()<<endl; texfile.close();
	    cout<<"Created tex file with fit table: "<<texfilename.str()<<endl<<endl;
	  }
	  else cout<<endl;

	  // end latex table //hh

	  
	} // graphs
      } // variables
    } // algorithms
  } // inputs

  if (0==mg) { cout<<"ERROR: No data points in the graph!"<<endl; return 0; }

  TCanvas* c;
  
  if (overlay) {
    c = new TCanvas(mg->GetName(),mg->GetName(),600,600);
    gPad->SetLeftMargin(0.18);
    gPad->SetRightMargin(0.05);
    gPad->SetTopMargin(0.08);
    gPad->SetBottomMargin(0.14);
    gPad->SetLogx(logx);
    gPad->SetLogy(logy);
    mg->Draw("AP");
    leg->SetLineColor(10);
    leg->SetFillColor(10);
    leg->SetBorderSize(0);
    leg->Draw();
    draw_range(ranges.front());
    draw_text(text);
    set_axis_titles(mg->GetHistogram(),quantity,ymin,ymax);
  }
  else {
    c = new TCanvas(mg->GetName(),mg->GetName(),600,600);
    unsigned nx = (unsigned)std::sqrt((float)graphs.size());
    unsigned ny = nx;
    if (nx*ny<graphs.size()) nx++;
    if (nx*ny<graphs.size()) ny++;
    c->Divide(nx,ny);
    for (unsigned i=0;i<graphs.size();i++) {
      c->cd(i+1);
      gPad->SetLeftMargin(0.18);
      gPad->SetRightMargin(0.05);
      gPad->SetTopMargin(0.08);
      gPad->SetBottomMargin(0.14);
      gPad->SetLogx(logx);
      gPad->SetLogy(logy);
      graphs[i]->Draw("AP");
      draw_range(ranges[i]);
      draw_text(text);
      set_axis_titles(graphs[i]->GetHistogram(),quantity,ymin,ymax);

      if (algs.size()>1||inputs.size()>1) {
	TLatex tex;
	tex.SetNDC();
	tex.SetTextFont(42);
	tex.SetTextSize(0.05);
	string txt = (algs.size()>1) ?
	  get_legend_label_from_alg(algs[i]) :
	  get_legend_label_from_input(inputs[i]);
	tex.DrawLatex(0.6,0.85,txt.c_str());
      }

    }
  }
  
  string output = c->GetName();
  if (!opath.empty()) output = opath + "/" + output;
  for (unsigned int iformat=0;iformat<formats.size();iformat++)
    c->Print((output+"."+formats[iformat]).c_str());
  
  
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
void draw_text(const string& text)
{
  if (text.empty()) return;
  double x(0.22);
  double y(0.88);
  string tmp(text);
  size_t pos = tmp.find(':');
  if (pos!=string::npos) {
    string x_as_str = tmp.substr(0,pos);
    tmp = tmp.substr(pos+1);
    pos = tmp.find(':'); assert(pos!=string::npos);
    string y_as_str = tmp.substr(0,pos);
    tmp = tmp.substr(pos+1);
    stringstream ssx; ssx<<x_as_str; ssx>>x;
    stringstream ssy; ssy<<y_as_str; ssy>>y;
  }
  TLatex tex;
  tex.SetNDC(true);
  tex.SetTextAlign(13);
  tex.SetTextSize(0.045);
  tex.SetTextFont(42);
  tex.DrawLatex(x,y,tmp.c_str());
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

    if (varname=="RefPt")    { varname = "p_{T}^{REF}"; unit = " GeV"; }
    if (varname=="JetPt")    { varname = "p_{T}";       unit = " GeV"; }
    if (varname=="JetEta")   { varname = varnameEta;    unit =     ""; }
    if (varname=="JetPhi")   { varname = "#varphi";     unit =     ""; }
    if (varname=="PtRel")    { varname = "p_{T}^{rel}"; unit = " GeV"; }
    if (varname=="RelLepPt") { varname = "p_{T}^{l}/p_{T}^{jet}"; unit =""; }

    ssrange<<varmin<<" < "<<varname<<" < "<<varmax<<unit<<"    ";
  }
  
  return ssrange.str();
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

  size_t pos=string::npos; int ireco=-1;
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
  size_t pos=tmp.find(':');
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
void set_graph_style(TGraphErrors* g, unsigned int ngraph,bool nocolor)
{
  Color_t colors[10] = {
    kBlue+1,kRed+1,kGreen+2,kMagenta+2,kCyan+3,
    kViolet+4,kOrange+1,kGreen-1,kMagenta-4,kCyan-2
  };
  Style_t lines[10]   = {
    1,1,1,1,1,1,1,1,1,1
  };
  Style_t markers[10] =
    { kOpenCircle,kOpenSquare,kOpenTriangleUp,kOpenDiamond,kOpenCross,
      kOpenStar,kFullCircle,kFullSquare,kFullTriangleUp,kFullTriangleDown
    };
  
  if (nocolor) {
    markers[0]=kFullCircle;       markers[1]=kOpenCircle;
    markers[2]=kFullSquare;       markers[3]=kOpenSquare;
    markers[4]=kFullTriangleUp;   markers[5]=kOpenTriangleUp;
    markers[6]=kFullStar;         markers[7]=kOpenStar;
    markers[8]=kFullTriangleDown; markers[9]=kOpenCross;
  }
  
  Color_t color = colors[std::min(ngraph,(unsigned)9)];
  Style_t line  = lines[std::min(ngraph,(unsigned)9)];
  Style_t marker = markers[std::min(ngraph,(unsigned)9)];
  
  if (nocolor) color = kBlack;
  
  g->SetLineColor(color);
  g->SetMarkerColor(color);
  g->SetMarkerStyle(marker);
  g->SetMarkerSize(0.8);
  
  TF1* f(0);
  if (g->GetListOfFunctions()->GetEntries()>0) {
    f=(TF1*)(g->GetListOfFunctions()->At(0));
    f->SetLineColor(color);
    f->SetLineStyle(line);
    f->SetLineWidth(1);
  }
  
  return;
}


//______________________________________________________________________________
void set_axis_titles(TH1* h,const string& quantity,float ymin,float ymax)
{
  if (0==h) {
    cout<<"set_axis_title ERROR: h is NULL!"<<endl;
    return;
  }
  h->SetTitle("");
  
  ymin = (ymin<0.0) ? 0.0 : ymin;
  h->SetMinimum(ymin);
  
  string xtitle("");
  string ytitle("");
  size_t pos = quantity.find("Vs");
  if (pos!=string::npos) {
    string ystr=quantity.substr(0,pos);
    string xstr=quantity.substr(pos+2);
    
    if (ystr=="Rsp"||ystr=="RelRsp"||ystr=="AbsRsp") {
      ytitle="p_{T} / p_{T}^{REF}";
      ymax = (ymax<0.0) ? 1.2 : ymax;
      h->SetMaximum(ymax);
    }
    if (ystr=="Res"||ystr=="RelRes"||ystr=="AbsRes") {
      ytitle="#sigma(p_{T}/p_{T}^{REF}) / <p_{T}/p_{T}^{REF}>";
      if (ymax>0.0) h->SetMaximum(ymax);
    }
    
    if (xstr=="RefPt")    xtitle="p_{T}^{REF} [GeV]";
    if (xstr=="JetPt")    xtitle="p_{T} [GeV]";
    if (xstr=="JetEta")   xtitle="#eta";
    if (xstr=="JetPhi")   xtitle="#varphi";
    if (xstr=="PtRel")    xtitle="p_{T}^{rel} [GeV]";
    if (xstr=="RelLepPt") xtitle="p_{T}^{l} / p_{T}^{jet}";
  }
  
  h->SetXTitle(xtitle.c_str());
  h->SetYTitle(ytitle.c_str());
  h->GetXaxis()->SetTitleOffset(1.1);
  h->GetYaxis()->SetTitleOffset(1.4);
  h->GetYaxis()->CenterTitle();
  h->GetYaxis()->SetNdivisions(505);
  return;
}

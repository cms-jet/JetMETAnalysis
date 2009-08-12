////////////////////////////////////////////////////////////////////////////////
//
// jet_response_and_resolution_x
// -----------------------------
//
//            07/21/2009 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/ObjectLoader.h"

#include <TROOT.h>
#include <TApplication.h>
#include <TFile.h>
#include <TKey.h>
#include <TH1F.h>
#include <TF1.h>
#include <TGraphErrors.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <cmath>


using namespace std;


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

  string var_default =
    string("RelRsp:RefPt,RelRsp:JetEta,RelRsp:JetPhi,") +
    string("AbsRsp:RefPt,AbsRsp:JetEta,AbsRsp:JetPhi,") +
    string("RelRsp:JetEta:RefPt,RelRsp:JetEta#1:RefPt,") +
    string("AbsRsp:JetEta:RefPt,AbsRsp:JetEta#1:RefPt,") +
    string("EtaRsp:RefPt,EtaRsp:JetEta,PhiRsp:RefPt,PhiRsp:JetEta");
  
  string         input     = cl.getValue<string> ("input");
  string         output    = cl.getValue<string> ("output",               "");
  vector<string> variables = cl.getVector<string>("variables",   var_default);
  vector<string> algs      = cl.getVector<string>("algs",                 "");
  bool           fitres    = cl.getValue<bool>   ("fitres",             true);

  if (!cl.check()) return 0;
  cl.print();
  
  
  //
  // open output file and declare vectors for response & resolution graphs
  //
  if (output.empty()) {
    unsigned int pos=input.find(".root");
    output=input.substr(0,pos)+"_g.root";
    cout<<"*** write output to "<<output<<endl;
  }
  
  TFile* ofile = new TFile(output.c_str(),"RECREATE");
  if (!ofile->IsOpen()) { cout<<"Can't create "<<output<<endl; return 0; }
  vector<TGraphErrors*> vrsp;
  vector<TGraphErrors*> vres;
  
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
  
  for (unsigned int ialg=0;ialg<algs.size();++ialg) {
    
    string alg = algs[ialg];
    TDirectory* idir = (TDirectory*)ifile->Get(alg.c_str());
    if (0==idir) { cout<<"No dir "<<alg<<" found"<<endl; return 0; }
    
    cout<<alg<<" ... "<<flush;
    
    TDirectory* odir = (TDirectory*)ofile->mkdir(alg.c_str());
    gROOT->cd();
    
    for (unsigned int ivar=0;ivar<variables.size();ivar++) {
      
      string variable = variables[ivar];
      ObjectLoader<TH1F> hlrsp;
      hlrsp.load_objects(idir,variable);
      
      string varexp=hlrsp.variable(hlrsp.nvariables()-1)+
	variable.substr(variable.find(':'));
      
      ObjectLoader<TH1F> hlvar;
      hlvar.load_objects(idir,varexp);
      
      vector<unsigned int> indices;
      TH1F* hrsp(0); TGraphErrors* grsp(0); TGraphErrors* gres(0);
      hlrsp.begin_loop();
      while ((hrsp=hlrsp.next_object(indices))) {

	// create new graphs for response & resolution
	if (indices.back()==0) {
	  grsp = new TGraphErrors(0); vrsp.push_back(grsp);
	  gres = new TGraphErrors(0); vres.push_back(gres);
	  string prefix = hlrsp.quantity().substr(0,3);
	  string grsp_name=prefix+"RspVs"+hlrsp.variable(hlrsp.nvariables()-1);
	  string gres_name=prefix+"ResVs"+hlrsp.variable(hlrsp.nvariables()-1);
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
	}
	
	// add new points to current response & resolution graphs
	if (hrsp->Integral()==0) continue;
	
	double x(0.0),ex(0.0);
	if (hlvar.nobjects()>0) {
	  TH1F*  hvar = hlvar.object(indices);
	  assert(hvar->GetEntries()>0);
	  x  = hvar->GetMean();
	  ex = hvar->GetMeanError();
	}
	else {
	  double min = hlrsp.minimum(hlrsp.nvariables()-1,indices.back());
	  double max = hlrsp.maximum(hlrsp.nvariables()-1,indices.back());
	  x=0.5*(min+max);
	  //cout<<grsp->GetName()<<" x="<<x<<" min="<<min<<" max="<<max<<endl;
	}
	
	TF1*   frsp = hrsp->GetFunction("fit");
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
	  if (g->GetN()==0) continue;
	  double xmin(g->GetX()[0]);
	  double xmax(-1e100);
	  for (int ipoint=0;ipoint<g->GetN();ipoint++)
	    if (g->GetX()[ipoint]>xmax) xmax = g->GetX()[ipoint];
	  TF1* fnc=new TF1("fit","sqrt(([0]/x)**2+[1]**2/x+[2]**2)",xmin,xmax);
	  fnc->SetLineWidth(2);
	  fnc->SetLineColor(g->GetLineColor());
	  fnc->SetParameter(0,0.5);
	  fnc->SetParameter(1,0.5);
	  fnc->SetParameter(2,0.1);
	  g->Fit(fnc,"QR");
	}
      }
    }
    
    // write response & resolution graphs to output root file
    odir->cd();
    for (unsigned int igraph=0;igraph<vrsp.size();igraph++) {
      //cout<<"alg="<<alg<<": write "<<vrsp[igraph]->GetName()<<endl;
      vrsp[igraph]->Write();
    }
    for (unsigned int igraph=0;igraph<vres.size();igraph++) {
      //cout<<"alg="<<alg<<": write "<<vres[igraph]->GetName()<<endl;
      vres[igraph]->Write();
    }
    vrsp.clear(); vres.clear();
    
    cout<<" DONE."<<endl;
  }
  
  
  //
  // close input & output files
  //
  ifile->Close();
  delete ifile;

  gROOT->GetListOfFiles()->Remove(ofile);
  ofile->Close();
  delete ofile;
  

  return 0;
}

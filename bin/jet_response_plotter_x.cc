////////////////////////////////////////////////////////////////////////////////
//
// jet_response_plotter_x
// ----------------------
//
//            08/08/2008 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/HistogramLoader.h"
#include "JetMETAnalysis/JetUtilities/interface/RootStyle.h"


#include <iostream>
#include <sstream>
#include <string>
#include <vector>
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
bool contains(const vector<string>& collection,const string& element);


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

  vector<TMultiGraph*>  vrsp_mg; vrsp_mg.push_back(new TMultiGraph());
  vector<TMultiGraph*>  vres_mg; vres_mg.push_back(new TMultiGraph());

  set_root_style();
  
  
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
  
  TIter nextDir(ifile->GetListOfKeys());
  TKey* dirKey(0);
  while ((dirKey=(TKey*)nextDir())) {
    if (strcmp(dirKey->GetClassName(),"TDirectoryFile")!=0) continue;
    TDirectoryFile* idir = (TDirectoryFile*)dirKey->ReadObj();
    string alg(idir->GetName()); if (!contains(algs,alg)) continue;
    
    cout<<alg<<" ... "<<flush;
    
    TDirectoryFile* odir = (TDirectoryFile*)ofile->mkdir(alg.c_str());
    odir->cd();
    
    HistogramLoader hlrsp;
    hlrsp.load_histograms(idir,variable);
    
    string varexp=hlrsp.variable(hlrsp.nvariables()-1);
    for (unsigned int i=0;i<hlrsp.nvariables();i++) varexp+=":"+hlrsp.variable(i);

    HistogramLoader hlvar;
    hlvar.load_histograms(idir,varexp);
    
    vector<unsigned int> indices;
    TH1F* hrsp(0); TGraphErrors* grsp(0); TGraphErrors* gres(0);
    hlrsp.begin_loop();
    while ((hrsp=hlrsp.next_histogram(indices))) {
      
      // create new graphs for response & resolution
      if (indices[0]==0) {
	grsp = new TGraphErrors(); vrsp.push_back(grsp);
	gres = new TGraphErrors(); vres.push_back(gres);
	string grsp_name = "RspVs"+hlrsp.variable(hlrsp.nvariables()-1);
	string gres_name = "ResVs"+hlrsp.variable(hlrsp.nvariables()-1);
	if (hlrsp.nvariables()>1) {
	  for (unsigned int i=0;i<hlrsp.nvariables()-1;i++) {
	    stringstream suffix;
	    suffix<<"_"<<hlrsp.variable(i)
		  <<hlrsp.minimum(i,0)<<"to"<<hlrsp.maximum(i,0);
	    grsp_name += suffix.str();
	    gres_name += suffix.str();
	  }
	}
	grsp->SetName(grsp_name.c_str());
	gres->SetName(gres_name.c_str());

	vrsp_mg.back()->Add(grsp);
	vres_mg.back()->Add(gres);
      }
      
      // add new points to current response & resolution graphs
      if (hrsp->Integral()==0) continue;
      
      TF1*   frsp = hrsp->GetFunction("fit");
      TH1F*  hvar = hlvar.histogram(indices);

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
      
      int n = grsp->GetN();
      grsp->SetPoint(n,x,y);
      grsp->SetPointError(n,ex,ey);
      gres->SetPoint(n,x,e);
      gres->SetPointError(n,ex,ee);
    }
    
    for (unsigned int igraph=0;igraph<vrsp.size();igraph++) vrsp[igraph]->Write();
    for (unsigned int igraph=0;igraph<vres.size();igraph++) vres[igraph]->Write();

    cout<<" DONE."<<endl;
  }
  
  
  //
  // run application if not in batch mode
  //
  argc = (batch) ? 2 : 1; if (batch) argv[1] = "-b";
  TApplication* app = new TApplication("jet_response_plotter_x",&argc,argv);

  TCanvas* crsp = new TCanvas("crsp","crsp"); crsp->cd();
  for (unsigned int img=0;img<vrsp_mg.size();img++) vrsp_mg[img]->Draw("AP");
  
  TCanvas* cres = new TCanvas("cres","cres"); cres->cd();
  for (unsigned int img=0;img<vres_mg.size();img++) vres_mg[img]->Draw("AP");
  
  

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

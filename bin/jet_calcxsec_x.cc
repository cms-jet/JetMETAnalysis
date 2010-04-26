////////////////////////////////////////////////////////////////////////////////
//
// jet_calcxsec_x
// --------------
//
//            26/04/2010 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"


#include <TFile.h>
#include <TTree.h>
#include <TEventList.h>


#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <cassert>


using namespace std;



////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv)
{
  //
  // INTERPRET COMMAND LINE ARGUMENTS
  //
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;

  string         sample   = cl.getValue <string>("sample");
  vector<string> inputs   = cl.getVector<string>("inputs");
  string         datapath = cl.getValue <string>("datapath",      "");
  string         dirname  = cl.getValue <string>("dirname","ak5calo");
  string         treename = cl.getValue <string>("treename",     "t");
  
  if (!cl.check()) return 0;
  cl.print();
  

  //
  // DETERMINE SAMPLES, LOWER PTHAT BOUNDS & CROSS-SECTIONS
  //
  vector<string> samples;
  vector<double> pthats;
  vector<double> xsecs;
  
  cout<<endl<<setw(12)<<"sample"<<setw(12)<<"pthat"<<setw(12)<<"xsection"<<endl;
  for (unsigned i=0;i<inputs.size();i++) {
    size_t pos = inputs[i].find(':');
    assert(pos!=string::npos);
    samples.push_back(inputs[i].substr(0,pos));
    stringstream ss; ss<<inputs[i].substr(pos+1);
    double xsec; ss>>xsec;
    xsecs.push_back(xsec);
    size_t pos2 = inputs[i].find(sample);
    assert(pos2==0);
    stringstream ss2; ss2<<inputs[i].substr(sample.size(),pos);
    double pthat; ss2>>pthat;
    pthats.push_back(pthat);
    cout<<setw(12)<<samples.back()
	<<setw(12)<<pthats.back()
	<<setw(12)<<xsecs.back()
	<<endl;
  }
  

  //
  // LOOP OVER SAMPLES AND PTHAT BOUNDS, DETERMINE EFFIFICENY MATRIX
  //
  double** eff = new double*[samples.size()];
  for (unsigned i=0;i<samples.size();i++) {
    string filename = samples[i];
    if (filename.find(".root")==string::npos) filename += ".root";
    if (!datapath.empty()) filename = datapath + "/" + filename;
    TFile* file = new TFile(filename.c_str(),"READ");
    if (!file->IsOpen()) { cout<<"Can't open "<<filename<<endl; return 0; }
    TDirectory* dir = (TDirectory*)file->Get(dirname.c_str());
    if (0==dir) { cout<<"No dir "<<dirname<<" in "<<filename<<endl; return 0; }
    TTree* tree = (TTree*)dir->Get(treename.c_str());
    if (0==tree) { cout<<"no tree "<<treename<<" in "<<filename<<endl; return 0; }
    
    double ntotal_i = tree->GetEntries();
    
    eff[i] = new double[samples.size()];    
    for (unsigned j=0;j<samples.size();j++) {
      stringstream sssel; sssel<<"pthat>"<<pthats[j];
      if (j<=i) eff[i][j]=1.0;
      else {
	TEventList* el = new TEventList("el","el");
	tree->Draw(">>el",sssel.str().c_str());
	double nsel_j = (double)el->GetN();
	eff[i][j] = nsel_j/ntotal_i;
      }
      cout<<"eff["<<samples[i]<<"]["<<sssel.str()<<"] = "<<eff[i][j]<<endl;
    }
  }
  

  //
  // CALCULATE EFFECTIVE CROSS-SECTIONS FOR EACH SAMPLE
  //
  cout<<"Effective cross-sections:"<<endl;
  for (unsigned i=0;i<samples.size();i++) {
    double xsec = xsecs[i];
    for (unsigned j=0;j<i;j++) xsec -= eff[j][i]*xsecs[j];
    cout<<samples[i]<<":"<<ios::scientific<<xsec<<endl;
  }
  
  
  return 0;
}


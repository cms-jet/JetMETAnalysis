////////////////////////////////////////////////////////////////////////////////
//
// jet_add_pudensity_x
// -------------------
//
//                      04/21/2017 Alexx Perloff <alexx.stephen.perloff@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/JetInfo.hh"
#include "JetMETAnalysis/JetUtilities/interface/JRAEvent.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"

#include "TSystem.h"
#include "TEnv.h"
#include "TMath.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TTree.h"
#include "TKey.h"
#include "TString.h"

#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <sys/stat.h>

using namespace std;


////////////////////////////////////////////////////////////////////////////////
// declare local functions
////////////////////////////////////////////////////////////////////////////////
const vector<double> vz = {0.0,1.7,2.6,3.0,3.5,4.2,5.2,6.0,7.5,9.0,12.0};
int getBin(double x, const double boundaries[], int length);


////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv)
{
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;
  
  string         input     = cl.getValue<string>  ("input");
  string         output    = cl.getValue<string>  ("output",       "");
  vector<string> algs      = cl.getVector<string> ("algs",         "");
  bool           debug     = cl.getValue<bool>    ("debug",     false);
  
  if(!cl.check()) return 0;
  cl.print();
  
  //
  // Speed up loading of data from the input file
  //
  gEnv->SetValue("TFile.AsyncPrefetching", 1);
  
  if (output.empty()) {
    output=input.substr(0,input.find(".root"))+"_gpudensity.root";
    cout<<"Write output to "<<output<<endl;
  }
 
  
  TFile* ifile = TFile::Open(input.c_str(),"READ");
  if (!ifile) { cout<<"Can't open file "<<input<<endl; return 0; }

  TFile* ofile = TFile::Open(output.c_str(),"RECREATE");
  if (!ofile) { cout<<"Can't open file "<<output<<endl; return 0; }
  
  if (algs.size()==0) {
    TIter nextDir(ifile->GetListOfKeys());
    TKey* dirKey(0);
    while ((dirKey=(TKey*)nextDir())) {
      if (strcmp(dirKey->GetClassName(),"TDirectoryFile")!=0) continue;
      string alg=dirKey->GetName();
      algs.push_back(dirKey->GetName());
    }
  }
  
  for (unsigned int ialg=0;ialg<algs.size();++ialg) {
    string alg=algs[ialg];
    JetInfo jetInfo(algs[ialg]);
    
    TDirectory* idir=(TDirectory*)ifile->Get(alg.c_str());
    if (0==idir) { cout<<"No dir "<<alg<<" found"<<endl; return 0; }
    
    cout<<"jet algorithm: "<<alg<<endl;
    
    TTree*      itree=(TTree*)idir->Get("t");    
    TDirectory* odir;
    
    stringstream ssodirname; ssodirname<<jetInfo.abbreviation;
    odir=(TDirectory*)ofile->mkdir(ssodirname.str().c_str()); odir->cd();
    if (debug) cout << "Cloning the input tree to the output tree ... " << flush;
    TTree*      otree = (debug) ? itree->CloneTree(10000) : itree->CloneTree(-1,"fast");
    if (debug) cout << "DONE" << endl;
    JRAEvent* JRAEvt = new JRAEvent(itree,85);
    TBranch* b_gpudensity=otree->Branch("gpudensity", &JRAEvt->gpudensity, "gpudensity/F");

    if (debug) cout << "Starting event loop ... " << endl;
    int nevt = (debug) ? 10000 : itree->GetEntries();
    for (int ievt=0;ievt<nevt;ievt++) {
       if (ievt % 100000 == 0)
          cout<<ievt<<endl;
       itree->GetEntry(ievt);
       int zbin = getBin(abs(JRAEvt->refpvz),&vz.at(0),vz.size()-1);
       JRAEvt->gpudensity = 0;
       for (unsigned int iz=0;iz<JRAEvt->zpositions->size();iz++) {
          if (getBin(abs(JRAEvt->zpositions->at(iz)),&vz.at(0),vz.size()-1)==zbin) JRAEvt->gpudensity++;
       }
       JRAEvt->gpudensity/=(20.0*(vz[zbin+1]-vz[zbin]));
       b_gpudensity->Fill();
    }
    otree->Write();
    delete itree;
    delete otree;
  }
  
  // close files
  ifile->Close();
  delete ifile;
  ofile->Close();
  delete ofile;
  
  return 0;
}


////////////////////////////////////////////////////////////////////////////////
// implementation of local functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int getBin(double x, const double boundaries[], int length)
{
   int i;
   int n = length;
   if (n<=0) return -1;
   if (x<boundaries[0] || x>=boundaries[n])
      return -1;
   for(i=0;i<n;i++)
   {
      if (x>=boundaries[i] && x<boundaries[i+1])
         return i;
   }
   return 0;
}

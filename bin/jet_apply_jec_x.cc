////////////////////////////////////////////////////////////////////////////////
//
// jet_apply_jec_x
// ---------------
//
//            08/30/2009 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"

#include <TSystem.h>
#include <TFile.h>
#include <TDirectory.h>
#include <TTree.h>
#include <TKey.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>


using namespace std;


////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv)
{
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;
  
  string         input    = cl.getValue<string> ("input");
  string         tag      = cl.getValue<string> ("tag");
  string         output   = cl.getValue<string> ("output",   "");
  vector<string> algs     = cl.getVector<string>("algs",     "");
  vector<int>    levels   = cl.getVector<int>   ("levels","2,3");
  
  if(!cl.check()) return 0;
  cl.print();
  
  if (output.empty()) {
    output=input.substr(0,input.find(".root"))+"_jec.root";
    cout<<"Write output to "<<output<<endl;
  }
  
  TFile* ifile = new TFile(input.c_str(),"READ");
  if (!ifile->IsOpen()) { cout<<"Can't open file "<<input<<endl; return 0; }

  TFile* ofile = new TFile(output.c_str(),"RECREATE");
  if (!ofile->IsOpen()) { cout<<"Can't open file "<<output<<endl; return 0; }
  
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
    
    TDirectory* idir=(TDirectory*)ifile->Get(alg.c_str());
    if (0==idir) { cout<<"No dir "<<alg<<" found"<<endl; return 0; }
    
    bool exclude(false);
    for (unsigned int i=0;i<levels.size();i++) {
      stringstream sslvl; sslvl<<"l"<<levels[i];
      if (alg.find(sslvl.str())!=string::npos) exclude=true;
    }
    if (exclude) {
      cout<<"exclude "<<alg<<endl;
      continue;
    }
    cout<<alg<<endl;
    
    TTree*      itree=(TTree*)idir->Get("t");    
    TDirectory* odir =(TDirectory*)ofile->mkdir(idir->GetName()); odir->cd();
    TTree*      otree=itree->CloneTree();
    
    stringstream ssodirname; ssodirname<<alg;
    for (unsigned int i=0;i<levels.size();i++) ssodirname<<"l"<<levels[i];
    cout<<"odirname="<<ssodirname.str()<<endl;
    odir=(TDirectory*)ofile->mkdir(ssodirname.str().c_str()); odir->cd();
    itree->SetBranchStatus("jtpt",0);
    otree=itree->CloneTree();
    itree->SetBranchStatus("jtpt",1);
    unsigned char nref;
    float         jtpt[100];
    float         jteta[100];
    itree->SetBranchAddress("nref",&nref);
    itree->SetBranchAddress("jtpt", jtpt);
    itree->SetBranchAddress("jteta",jteta);
    TBranch* b_jtpt=otree->Branch("jtpt",jtpt,"jtpt[nref]/F");

    int nevt=itree->GetEntries();
    for (int ievt=0;ievt<nevt;ievt++) {
      itree->GetEntry(ievt);
      // TODO: compute correction!
      b_jtpt->Fill();
    }
  }
  
  // close files
  ifile->Close();
  delete ifile;
  ofile->Write();
  ofile->Close();
  delete ofile;
  
  return 0;
}

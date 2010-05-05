////////////////////////////////////////////////////////////////////////////////
//
// jet_scale_resolution_x
// ----------------------
//
//            05/05/2010 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"

#include <TSystem.h>
#include <TFile.h>
#include <TDirectory.h>
#include <TTree.h>
#include <TKey.h>

#include <cstdlib>
#include <stdexcept>
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
  
  string         input    = cl.getValue <string>("input");
  string         output   = cl.getValue <string>("output",  "");
  vector<string> algs     = cl.getVector<string>("algs",    "");
  double         scale    = cl.getValue <double>("scale",  0.1);
  
  if(!cl.check()) return 0;
  cl.print();
  
  if (output.empty()) {
    output=input.substr(0,input.find(".root"))+"_smeared.root";
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
      if (alg.find("l2l3")!=string::npos) algs.push_back(dirKey->GetName());
    }
  }
  
  for (unsigned int ialg=0;ialg<algs.size();++ialg) {
    string alg=algs[ialg];
    
    TDirectory* idir=(TDirectory*)ifile->Get(alg.c_str());
    if (0==idir) { cout<<"No dir "<<alg<<" found"<<endl; return 0; }

    if (alg.find("l2l3")==string::npos) { cout<<"skip "<<alg<<endl; continue; }
    cout<<"jet algorithm: "<<alg<<endl;

    
    TTree*      itree=(TTree*)idir->Get("t");    
    TDirectory* odir =(TDirectory*)ofile->mkdir(idir->GetName()); odir->cd();

    //itree->CloneTree()->Write();
    
    itree->SetBranchStatus("jtpt",0);
    itree->SetBranchStatus("jte", 0);
    TTree*      otree=itree->CloneTree();
    itree->SetBranchStatus("jtpt",1);
    itree->SetBranchStatus("jte", 1);
    
    unsigned char nref;
    float         refpt[100];
    float         jtpt[100];
    float         jte[100];
    itree->SetBranchAddress("nref",&nref);
    itree->SetBranchAddress("refpt",refpt);
    itree->SetBranchAddress("jtpt", jtpt);
    TBranch* b_jtpt=otree->Branch("jtpt",jtpt,"jtpt[nref]/F");
    TBranch* b_jte =otree->Branch("jte", jte, "jte[nref]/F");
    
    int nevt=itree->GetEntries();
    for (int ievt=0;ievt<nevt;ievt++) {
      itree->GetEntry(ievt);
      for (unsigned int ijt=0;ijt<nref;ijt++) {
	float deltapt = (jtpt[ijt]-refpt[ijt])*scale;
	float ptscale = (jtpt[ijt]+deltapt)/jtpt[ijt];
	jtpt[ijt] *= ptscale;
	jte[ijt]  *= ptscale;
      }
      b_jtpt->Fill();
      b_jte ->Fill();
    }
    otree->Write();
  }
  
  // close files
  ifile->Close();
  delete ifile;
  ofile->Close();
  delete ofile;
  
  return 0;
}

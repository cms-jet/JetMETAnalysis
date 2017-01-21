////////////////////////////////////////////////////////////////////////////////
//
// jet_apply_jec_x
// ---------------
//
//            08/30/2009 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
//                       Hauke Held                               <held@cern.ch>
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
//#include <stdexcept>
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

/// returns the postfix associated with a specific level and algorithm
string getPostfix(vector<string> postfix, string alg, int level);

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv)
{
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;
  
  string         input     = cl.getValue<string>  ("input");
  string         era       = cl.getValue<string>  ("era");
  vector<int>    levels    = cl.getVector<int>    ("levels");
  string         output    = cl.getValue<string>  ("output",       "");
  string         jecpath   = cl.getValue<string>  ("jecpath",      "");
  vector<string> algs      = cl.getVector<string> ("algs",         "");
  bool           L1FastJet = cl.getValue<bool>    ("L1FastJet",  true);
  vector<string> postfix   = cl.getVector<string> ("postfix",      "");
  bool           useTags   = cl.getValue<bool>    ("useTags",   false);
  bool           saveitree = cl.getValue<bool>    ("saveitree",  true);
  bool           debug     = cl.getValue<bool>    ("debug",     false);
  
  if(!cl.check()) return 0;
  cl.print();
  
  //
  // Speed up loading of data from the input file
  //
  gEnv->SetValue("TFile.AsyncPrefetching", 1);
  
  if (output.empty()) {
    output=input.substr(0,input.find(".root"))+"_jec.root";
    cout<<"Write output to "<<output<<endl;
  }
  
  if (jecpath.empty()) {
    string cmssw_base(getenv("CMSSW_BASE"));
    string cmssw_release_base(getenv("CMSSW_RELEASE_BASE"));
    jecpath = cmssw_base + "/src/CondFormats/JetMETObjects/data";
    struct stat st;
    if (stat(jecpath.c_str(),&st)!=0)
      jecpath = cmssw_release_base + "/src/CondFormats/JetMETObjects/data";
    if (stat(jecpath.c_str(),&st)!=0) {
      cout<<"ERROR: tried to set jecpath but failed, abort."<<endl;
      return 0;
    }
    else cout<<"jecpath set to "<<jecpath<<endl;
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
    
    bool exclude(false);
    for (unsigned int i=0;i<levels.size();i++) {
      stringstream sslvl; sslvl<<"l"<<levels[i];
      if (alg.find(sslvl.str())!=string::npos) exclude=true;
    }
    if (exclude) {
      cout<<"exclude "<<alg<<endl;
      continue;
    }
    cout<<"jet algorithm: "<<alg<<endl;
    cout<<"correction level: "<<JetInfo::get_correction_levels(levels,L1FastJet)<<endl;
    cout<<"correction tag: "<<JetInfo::get_correction_tags(era,alg,levels,jecpath,L1FastJet)<<endl;

    //
    // Get the corrections from the text files
    //
    if (debug) cout << "Setting up the FactorizedJetCorrector ... " << flush;
    FactorizedJetCorrector *corrector;
    if(useTags)
      {
         corrector = new FactorizedJetCorrector(JetInfo::get_correction_levels(levels,L1FastJet),
                                                JetInfo::get_correction_tags(era,alg,levels,jecpath,L1FastJet));
      }
    else
      {
        //
        // Make sure the levels are in the correct order (lowest level to highest)
        //
        sort (levels.begin(),levels.end());
        vector<JetCorrectorParameters> vPar;
        for(unsigned int ilevel=0; ilevel<levels.size(); ilevel++)
          {
            vPar.push_back(JetCorrectorParameters(string(jecpath + era + 
                                                  JetInfo::get_level_tag(levels[ilevel],L1FastJet) + 
                                                  jetInfo.getAlias() + 
                                                  getPostfix(postfix,alg,levels[ilevel]) + ".txt")));
          }
        corrector = new FactorizedJetCorrector(vPar);
      }
    if (debug) cout << "DONE" << endl;
    
    TTree*      itree=(TTree*)idir->Get("t");    
    TDirectory* odir;
    if(saveitree) {
      if (debug) cout << "Cloning and writing the input tree ... " << flush;
      odir =(TDirectory*)ofile->mkdir(idir->GetName()); odir->cd();
      itree->CloneTree()->Write();
      if (debug) cout << "DONE" << endl;
    }
    
    stringstream ssodirname; ssodirname<<jetInfo.abbreviation;
    for (unsigned int i=0;i<levels.size();i++) ssodirname<<"l"<<levels[i];
    odir=(TDirectory*)ofile->mkdir(ssodirname.str().c_str()); odir->cd();
    itree->SetBranchStatus("jtpt",0);
    itree->SetBranchStatus("jte", 0);
    if (debug) cout << "Cloning the input tree to the output tree ... " << flush;
    TTree*      otree = (debug) ? itree->CloneTree(10000) : itree->CloneTree(-1,"fast");
    if (debug) cout << "DONE" << endl;
    itree->SetBranchStatus("jtpt",1);
    itree->SetBranchStatus("jte", 1);
    JRAEvent* JRAEvt = new JRAEvent(itree,85);
    TBranch* b_jtpt=otree->Branch("jtpt", "vector<Float_t>", &JRAEvt->jtpt);
    TBranch* b_jte =otree->Branch("jte", "vector<Float_t>", &JRAEvt->jte);

    if (debug) cout << "Starting event loop ... " << endl;
    int nevt = (debug) ? 10000 : itree->GetEntries();
    for (int ievt=0;ievt<nevt;ievt++) {
       if (ievt % 100000 == 0)
          cout << "Loop on events: " << ievt << endl;
       itree->GetEntry(ievt);
       for (unsigned int ijt=0;ijt<JRAEvt->nref;ijt++) {
          corrector->setJetPt(JRAEvt->jtpt->at(ijt));
          corrector->setJetE(JRAEvt->jte->at(ijt));
          corrector->setJetEta(JRAEvt->jteta->at(ijt));
          if (TString(JetInfo::get_correction_levels(levels,L1FastJet)).Contains("L1FastJet")) {
             if (JRAEvt->jtarea->at(ijt)!=0)
                corrector->setJetA(JRAEvt->jtarea->at(ijt));
             else if (jetInfo.coneSize>0)
                corrector->setJetA(TMath::Pi()*TMath::Power(jetInfo.coneSize/10.0,2));
             else {
                cout << "WARNING::Unknown jet area. Skipping event." << endl;
                continue;
             }

             if (jetInfo.isHLT())
                corrector->setRho(JRAEvt->rho_hlt);
             else
                corrector->setRho(JRAEvt->rho);
          }
          if(!L1FastJet) corrector->setNPV(JRAEvt->npv);
          float jec=corrector->getCorrection();
          JRAEvt->jtpt->at(ijt)*=jec;
          JRAEvt->jte->at(ijt) *=jec;
       }
       b_jtpt->Fill();
       b_jte ->Fill();
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
string getPostfix(vector<string> postfix, string alg, int level)
{
  for(unsigned int ipostfix=0; ipostfix<postfix.size(); ipostfix+=3)
    {
      TString tmp(postfix[ipostfix+1]);
      if(postfix[ipostfix].compare(alg)==0 && atoi(tmp.Data())==level)
        return postfix[ipostfix+2];
    }
  return "";
}

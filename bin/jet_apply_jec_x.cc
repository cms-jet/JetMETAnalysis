////////////////////////////////////////////////////////////////////////////////
//
// jet_apply_jec_x
// ---------------
//
//            08/30/2009 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
//                       Hauke Held                               <held@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"

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
// declare local functions
////////////////////////////////////////////////////////////////////////////////
string get_correction_levels(const vector<int>& levels);
string get_correction_tags(const string& tag,const string& alg,
			   const vector<int>& levels,const string& jecpath);


////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv)
{
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;
  
  string         input    = cl.getValue<string> ("input");
  string         era      = cl.getValue<string> ("era");
  vector<int>    levels   = cl.getVector<int>   ("levels");
  string         output   = cl.getValue<string> ("output",   "");
  string         jecpath  = cl.getValue<string> ("jecpath",  "");
  vector<string> algs     = cl.getVector<string>("algs",     "");

  
  if(!cl.check()) return 0;
  cl.print();
  
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
    cout<<"jet algorithm: "<<alg<<endl;

    FactorizedJetCorrector corrector(get_correction_levels(levels),
				     get_correction_tags(era,alg,levels,jecpath));
    
    
    TTree*      itree=(TTree*)idir->Get("t");    
    TDirectory* odir =(TDirectory*)ofile->mkdir(idir->GetName()); odir->cd();
    itree->CloneTree()->Write();
    
    itree->SetBranchStatus("jtpt",0);
    itree->SetBranchStatus("jte", 0);
    TTree*      otree=itree->CloneTree();
    itree->SetBranchStatus("jtpt",1);
    itree->SetBranchStatus("jte", 1);
    
    stringstream ssodirname; ssodirname<<alg;
    for (unsigned int i=0;i<levels.size();i++) ssodirname<<"l"<<levels[i];
    odir=(TDirectory*)ofile->mkdir(ssodirname.str().c_str()); odir->cd();
    unsigned char nref;
    float         jtpt[100];
    float         jteta[100];
    float         jte[100];
    itree->SetBranchAddress("nref",&nref);
    itree->SetBranchAddress("jtpt", jtpt);
    itree->SetBranchAddress("jteta",jteta);
    TBranch* b_jtpt=otree->Branch("jtpt",jtpt,"jtpt[nref]/F");
    TBranch* b_jte =otree->Branch("jte", jte, "jte[nref]/F");
    
    int nevt=itree->GetEntries();
    for (int ievt=0;ievt<nevt;ievt++) {
      itree->GetEntry(ievt);
      for (unsigned int ijt=0;ijt<nref;ijt++) {
	corrector.setJetPt(jtpt[ijt]);
	corrector.setJetEta(jteta[ijt]);
	float jec=corrector.getCorrection();
	jtpt[ijt]*=jec;
	jte[ijt] *=jec;
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


////////////////////////////////////////////////////////////////////////////////
// implementation of local functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
string get_correction_levels(const vector<int>& levels)
{
  stringstream ssresult;
  for (unsigned int ilevel=0;ilevel<levels.size();++ilevel) {
    if (ilevel!=0) ssresult<<":";
    int level(levels[ilevel]);
    switch (level) {
    case 1 : ssresult<<"L1Offset"; break;
    case 2 : ssresult<<"L2Relative"; break;
    case 3 : ssresult<<"L3Absolute"; break;
    case 4 : ssresult<<"L4EMF"; break;
    case 5 : ssresult<<"L5Flavor"; break;
    case 6 : ssresult<<"L6SLB"; break;
    case 7 : ssresult<<"L7Parton"; break;
    default: throw std::runtime_error(((string)"get_correction_levels ERROR: "+
				       (string)"invalid correction level").c_str());
    }
  }
    return ssresult.str();
}


//______________________________________________________________________________
string get_correction_tags(const string& era,const string& alg,
			   const vector<int>& levels,
			   const string& jecpath)
{
  stringstream ssresult;
  for (unsigned int ilevel=0;ilevel<levels.size();ilevel++) {
    
    if (ilevel!=0) ssresult<<":";
    
    int level=levels[ilevel];
    stringstream ssera;
    
    ssera<<jecpath<<"/";

    if      (level==1) ssera<<"L1Offset_";
    else if (level==2) ssera<<era<<"_L2Relative_";
    else if (level==3) ssera<<era<<"_L3Absolute_";
    else if (level==4) ssera<<"L4EMF_";
    else if (level==5) ssera<<"L5Flavor_";
    else if (level==6) ssera<<"L6SLB_";
    else if (level==7) ssera<<"L7Parton_";
    else throw std::runtime_error("unknown correction level");
    
    if (level==6) {
      ssresult<<ssera.str()<<".txt";
      continue;
    }
    
    if      (alg.find("ak5")==0) ssera<<"AK5";
    else if (alg.find("ak7")==0) ssera<<"AK7";
    else if (alg.find("kt4")==0) ssera<<"KT4";
    else if (alg.find("kt6")==0) ssera<<"KT6";
    else if (alg.find("ca4")==0) ssera<<"CA4";
    else if (alg.find("ca6")==0) ssera<<"CA6";
    else if (alg.find("sc5")==0) ssera<<"SC5";
    else if (alg.find("sc7")==0) ssera<<"SC7";
    else if (alg.find("ic5")==0) ssera<<"IC5";
    if (alg.find("tau")==3) ssera<<std::string(alg, 3);

    if (level==1 || level==4 || level==5 || level==7) {
      ssresult<<ssera.str()<<".txt";
      continue;
    }
    
    if      (alg.find("calo")!=string::npos ||
	     alg.find("Calo")!=string::npos ||
	     alg.find("CALO")!=string::npos) ssera<<"Calo";
    else if (alg.find("pf")!=string::npos   ||
	     alg.find("Pf")!=string::npos   ||
	     alg.find("PF")!=string::npos)   ssera<<"PF";
    else if (alg.find("jpt")!=string::npos  ||
	     alg.find("Jpt")!=string::npos  ||
	     alg.find("JPT")!=string::npos)  ssera<<"JPT";
    
    ssresult<<ssera.str()<<".txt";
    cout<<ssera.str()<<".txt"<<endl;
  }
  
  
  return ssresult.str();
}

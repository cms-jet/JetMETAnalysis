////////////////////////////////////////////////////////////////////////////////
//
// jet_apply_jec_x
// ---------------
//
//            08/30/2009 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "CondFormats/JetMETObjects/interface/CombinedJetCorrector.h"

#include <TSystem.h>
#include <TFile.h>
#include <TDirectory.h>
#include <TTree.h>
#include <TKey.h>

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
			   const vector<int>& levels);

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
    cout<<"jet algorithm: "<<alg<<endl;
    
    TTree*      itree=(TTree*)idir->Get("t");    
    TDirectory* odir =(TDirectory*)ofile->mkdir(idir->GetName()); odir->cd();
    itree->CloneTree()->Write();
    //odir->Write();

    itree->SetBranchStatus("jtpt",0);
    TTree*      otree=itree->CloneTree();
    itree->SetBranchStatus("jtpt",1);
    
    stringstream ssodirname; ssodirname<<alg;
    for (unsigned int i=0;i<levels.size();i++) ssodirname<<"l"<<levels[i];
    odir=(TDirectory*)ofile->mkdir(ssodirname.str().c_str()); odir->cd();
    unsigned char nref;
    float         jtpt[100];
    float         jteta[100];
    itree->SetBranchAddress("nref",&nref);
    itree->SetBranchAddress("jtpt", jtpt);
    itree->SetBranchAddress("jteta",jteta);
    TBranch* b_jtpt=otree->Branch("jtpt",jtpt,"jtpt[nref]/F");

    CombinedJetCorrector corrector(get_correction_levels(levels),
				   get_correction_tags(tag,alg,levels));
    
    int nevt=itree->GetEntries();
    for (int ievt=0;ievt<nevt;ievt++) {
      itree->GetEntry(ievt);
      for (unsigned int ijt=0;ijt<nref;ijt++) {
	float pt =jtpt[ijt];
	float eta=jteta[ijt];
	float jec=corrector.getCorrection(pt,eta,pt);
	jtpt[ijt]*=jec;
	//if (ijt==0) cout<<"ptraw="<<pt<<" jec="<<jec<<" ptcorr="<<pt*jec
	//	<<endl;
      }
      b_jtpt->Fill();
    }
    otree->Write();
    //odir->Write();
  }
  
  // close files
  ifile->Close();
  delete ifile;
  //ofile->Write();
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
    ssresult<<"L"<<levels[ilevel];
  }
  //cout<<"levels as string: "<<ssresult.str()<<endl;
  return ssresult.str();
}


//______________________________________________________________________________
string get_correction_tags(const string& tag,const string& alg,
			   const vector<int>& levels)
{
  stringstream ssresult;
  for (unsigned int ilevel=0;ilevel<levels.size();ilevel++) {
    
    if (ilevel!=0) ssresult<<":";
    
    int level=levels[ilevel];
    stringstream sstag;
    sstag<<tag<<"_";

    if      (level==1) sstag<<"L1Offset_";
    else if (level==2) sstag<<"L2Relative_";
    else if (level==3) sstag<<"L3Absolute_";
    else if (level==4) sstag<<"L4EMF_";
    else if (level==5) sstag<<"L5Flavor_";
    else if (level==6) sstag<<"L6Hadron_";
    else if (level==7) sstag<<"L7Parton_";
    else throw std::runtime_error("unknown correction level");
    
    if      (alg.find("ak5")==0) sstag<<"AK5";
    else if (alg.find("ak7")==0) sstag<<"AK7";
    else if (alg.find("kt4")==0) sstag<<"KT4";
    else if (alg.find("kt6")==0) sstag<<"KT6";
    else if (alg.find("ca4")==0) sstag<<"CA4";
    else if (alg.find("ca6")==0) sstag<<"CA6";
    else if (alg.find("sc5")==0) sstag<<"SC5";
    else if (alg.find("sc7")==0) sstag<<"SC7";
    else if (alg.find("ic5")==0) sstag<<"IC5";
    
    if      (alg.find("calo")>0) sstag<<"Calo";
    else if (alg.find("pf")>0)   sstag<<"PF";
    else if (alg.find("jpt")>0)  sstag<<"JPT";

    ssresult<<sstag.str();
  }
  
  //cout<<"tags="<<ssresult.str()<<endl;
  
  return ssresult.str();
}

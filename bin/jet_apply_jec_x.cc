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
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"

#include "TSystem.h"
#include "TMath.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TTree.h"
#include "TKey.h"
#include "TString.h"

#include <cstdlib>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>


using namespace std;


////////////////////////////////////////////////////////////////////////////////
// declare local functions
////////////////////////////////////////////////////////////////////////////////

/// returns a string containing all of the correction levels to be applied
string get_correction_levels(const vector<int>& levels);

/// returns the full path of each correction level concatinated into a single string
string get_correction_tags(const string& tag,const string& alg,
                           const vector<int>& levels,const string& jecpath, bool L1FastJet);

/// return the full name of the correction level
string get_level_tag(int level, bool L1FastJet);

/// get the uppercase version of the algorithm name
string getAlias(TString s);

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
  bool           L1FastJet = cl.getValue<bool>    ("L1FastJet", false);
  vector<string> postfix   = cl.getVector<string> ("postfix",      "");
  bool           useTags   = cl.getValue<bool>    ("useTags",    true);

  
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
    cout<<"correction level: "<<get_correction_levels(levels)<<endl;
    cout<<"correction tag: "<<get_correction_tags(era,alg,levels,jecpath,L1FastJet)<<endl;

    //
    // Get the corrections from the text files
    //
    FactorizedJetCorrector *corrector;
    if(useTags)
      {
        corrector = new FactorizedJetCorrector(get_correction_levels(levels),
                                               get_correction_tags(era,alg,levels,jecpath,L1FastJet));
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
            vPar.push_back(JetCorrectorParameters(jecpath + era + 
                                                  get_level_tag(levels[ilevel],L1FastJet) + 
                                                  getAlias(alg) + 
                                                  getPostfix(postfix,alg,levels[ilevel]) + ".txt"));
          }
        corrector = new FactorizedJetCorrector(vPar);
      }
    
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
    float         rho;
    float         rho_hlt;
    vector<int>* npus = new vector<int>;
    itree->SetBranchAddress("nref",&nref);
    itree->SetBranchAddress("jtpt", jtpt);
    itree->SetBranchAddress("jteta",jteta);
    itree->SetBranchAddress("npus",&npus);
    itree->SetBranchAddress("rho",&rho);
    itree->SetBranchAddress("rho_hlt",&rho_hlt);
    TBranch* b_jtpt=otree->Branch("jtpt",jtpt,"jtpt[nref]/F");
    TBranch* b_jte =otree->Branch("jte", jte, "jte[nref]/F");
    
    int nevt=itree->GetEntries();
    for (int ievt=0;ievt<nevt;ievt++) {
       if (ievt % 100000 == 0)
          cout<<ievt<<endl;
       itree->GetEntry(ievt);
       for (unsigned int ijt=0;ijt<nref;ijt++) {
          corrector->setJetPt(jtpt[ijt]);
          corrector->setJetEta(jteta[ijt]);
          if (TString(get_correction_levels(levels)).Contains("L1FastJet")) {
             corrector->setJetA(TMath::Pi()*TMath::Power(0.5,2));
             if (TString(alg).Contains("HLT")||TString(alg).Contains("hlt"))
                corrector->setRho(rho_hlt);
             else
                corrector->setRho(rho);
          }
          if(!L1FastJet) corrector->setNPV((*npus)[0]+(*npus)[1]+(*npus)[2]);
          float jec=corrector->getCorrection();
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
       //case 1 : ssresult<<"L1Offset"; break;
    case 1 : ssresult<<"L1FastJet"; break;
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
               const string& jecpath, bool L1FastJet)
{
  stringstream ssresult;
  for (unsigned int ilevel=0;ilevel<levels.size();ilevel++) {
    
    if (ilevel!=0) ssresult<<":";
    
    int level=levels[ilevel];
    stringstream ssera;
    
    if(jecpath[jecpath.length()-1] != '/') ssera<<jecpath<<"/"<<era<<"_";
    else ssera<<jecpath<<era<<"_";

    if      (level==1 && !L1FastJet) ssera<<"L1Offset_";
    else if (level==1 && L1FastJet) ssera<<"L1FastJet_";
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

    if (/*level==1 || */level==4 || level==5 || level==7) {
      ssresult<<ssera.str()<<".txt";
      continue;
    }

    //
    // This will eventually need to be updated to include l1 and l1off corrected collections
    //
    if      (alg.find("calohlt")!=string::npos ||
             alg.find("caloHLT")!=string::npos ||
             alg.find("CaloHLT")!=string::npos ||
             alg.find("CALOHLT")!=string::npos) ssera<<"CaloHLT";
    else if (alg.find("calo")!=string::npos ||
             alg.find("Calo")!=string::npos ||
             alg.find("CALO")!=string::npos) ssera<<"Calo";
    else if (alg.find("pfchshlt")!=string::npos ||
             alg.find("PFCHSHLT")!=string::npos ||
             alg.find("PFchsHLT")!=string::npos ||
             alg.find("pfchsHLT")!=string::npos) ssera<<"PFchsHLT";
    else if (alg.find("pfchs")!=string::npos   ||
             alg.find("Pfchs")!=string::npos   ||
             alg.find("PFCHS")!=string::npos   ||
             alg.find("PFchs")!=string::npos)   ssera<<"PFchs";
    else if (alg.find("pfhlt")!=string::npos ||
             alg.find("pfHLT")!=string::npos ||
             alg.find("PFHLT")!=string::npos) ssera<<"PFHLT";
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

//______________________________________________________________________________
string get_level_tag(int level, bool L1FastJet)
{
    if      (level==1 && !L1FastJet) return "_L1Offset_";
    else if (level==1 && L1FastJet)  return "_L1FastJet_";
    else if (level==2)               return "_L2Relative_";
    else if (level==3)               return "_L3Absolute_";
    else if (level==4)               return "_L4EMF_";
    else if (level==5)               return "_L5Flavor_";
    else if (level==6)               return "_L6SLB_";
    else if (level==7)               return "_L7Parton_";
    else if (level==23)              return "_L2L3Residual_";
    else throw std::runtime_error("unknown correction level");
}

//______________________________________________________________________________
string getAlias(TString s)
{
  if (s=="ic5calo")
    return "IC5Calo";
  else if (s=="ic5pf")
    return "IC5PF";
  else if (s=="ak5calo")
    return "AK5Calo";  
  else if (s=="ak5calol1")
    return "AK5Calol1";
  else if (s=="ak5calol1off")
    return "AK5Calol1off";
  else if (s=="ak7calo")
    return "AK7Calo";
  else if (s=="ak7calol1")
    return "AK7Calol1";
  else if (s=="ak7calol1off")
    return "AK7Calol1off";
  else if (s=="ak5caloHLT")
     return "AK5CaloHLT";
  else if (s=="ak5caloHLTl1")
     return "AK5CaloHLTl1";
  else if (s=="ak5pf")
    return "AK5PF";
  else if (s=="ak5pfl1")
    return "AK5PFl1";
  else if (s=="ak5pfl1off")
     return "AK5PFl1off";
  else if (s=="ak7pf")
    return "AK7PF";
  else if (s=="ak7pfl1")
    return "AK7PFl1";
  else if (s=="ak7pfl1off")
    return "AK7PFl1off";
  else if (s=="ak5pfchs")
    return "AK5PFchs";
  else if (s=="ak5pfchsl1")
    return "AK5PFchsl1";
  else if (s=="ak5pfchsl1off")
     return "AK5PFchsl1off";
  else if (s=="ak7pfchs")
    return "AK7PFchs";
  else if (s=="ak7pfchsl1")
    return "AK7PFchsl1";
  else if (s=="ak7pfchsl1off")
    return "AK7PFchsl1off";
  else if (s=="ak5pfHLT")
     return "AK5PF_HLT";
  else if (s=="ak5pfHLTl1")
     return "AK5PFHLTl1";
  else if (s=="ak5pfchsHLT")
     return "AK5PFchsHLT";
  else if (s=="ak5pfchsHLTl1")
     return "AK5PFchsHLTl1";
  else if (s=="ak5jpt")
    return "AK5JPT";
  else if (s=="ak5jptl1")
    return "AK5JPTl1";
  else if (s=="ak7jpt")
    return "AK7JPT";
  else if (s=="ak7jptl1")
    return "AK7JPTl1";
  else if (s=="sc5calo")
    return "SC5Calo";
  else if (s=="sc5pf")
    return "SC5PF";
  else if (s=="sc7calo")
    return "SC5Calo";
  else if (s=="sc7pf")
    return "SC5PF";
  else if (s=="kt4calo")
    return "KT4Calo";
  else if (s=="kt4pf")
    return "KT4PF";
  else if (s=="kt6calo")
    return "KT6Calo";
  else if (s=="kt6pf")
    return "KT6PF";
  else
    return "unknown";
}

//______________________________________________________________________________
string getPostfix(vector<string> postfix, string alg, int level)
{
  for(unsigned int ipostfix; ipostfix<postfix.size(); ipostfix+=3)
    {
      TString tmp(postfix[ipostfix+1]);
      if(postfix[ipostfix].compare(alg)==0 && atoi(tmp.Data())==level)
        return postfix[ipostfix+2];
    }
  return "";
}

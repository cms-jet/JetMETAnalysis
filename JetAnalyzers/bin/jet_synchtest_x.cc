// ROOT Libraries
#include "TROOT.h"
#include "TSystem.h"
#include "TEnv.h"
#include "TChain.h"
#include "TFile.h"
#include "TChain.h"
#include "TString.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TProfile3D.h"
#include "THnSparse.h"
#include "TBenchmark.h"

// C++ Libraries
#include <iostream>
#include <vector>
#include <map>
#include <sys/stat.h>

// JetMETAnalysis Libraries
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/JRAEvent.h"
#include "JetMETAnalysis/JetUtilities/interface/JetInfo.hh"
#include "JetMETAnalysis/JetUtilities/interface/EvtID.hh"
#include "JetMETAnalysis/JetUtilities/interface/ProgressBar.hh"

// CMSSW Libraries
#include "DataFormats/Provenance/interface/RunLumiEventNumber.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"

using namespace std;

typedef map<double, pair<Int_t, Int_t> > ITJ;

////////////////////////////////////////////////////////////////////////////////
// declare class
////////////////////////////////////////////////////////////////////////////////

class MatchEventsAndJets {
public:

   MatchEventsAndJets();
   MatchEventsAndJets(string algo1_, string algo2_, bool iftest_);
   ~MatchEventsAndJets() {;}

   void   getMaxDeltaR();
   double getMaxDeltaR(string algName);
   void   SetMaxEvts(int me) {maxEvts = me;}
   void   SetNRefMax(int nrm) {nrefmax = nrm;}
   void   SetDoNotSaveFlag(bool dns) {doNotSaveFlag = dns;}
   void   SetWeightParameters(bool useweight_, bool pThatReweight_, double bias2SelectionRef_, double bias2SelectionPow_);
   void   SetupLumiWeights(string dataFile, string mcFile, string dataHist, string mcHist);
   void   OpenInputFiles(string filenamePU, string filenameNoPU);
   void   MakeMatchedEventsMaps(string treeName, string outputPath);
   void   ConvertEvtMapToVector(const ITS& mapTree, vector<evtid>& vevtid, vector<pair<ull,ull> >& vll);
   void   ConvertEvtMapToTTree(const ITS& mapTree, TTree* treeMap);
   void   WriteMatchedEventsMaps(ITS mapTree, bool noPU, string outputPath);
   void   ConvertTTreeToMap(ITS& mapTree, TTree* treeMap);
   void   ReadMatchedEventsMaps(string pathToMaps);
   ITS    fillMap(bool noPU, string treeName, string outputPath);
   void   GetNtuples(string treeName = "t");
   void   OpenOutputFile(string outputPath = "./");
   void   SetJEC(string JECPar = "parameters_ak5pf.txt");
   void   SetNpvRhoNpuValues(int NBins, int Width) {NBinsNpvRhoNpu=NBins; npvRhoNpuBinWidth=Width;}
   void   SetVptBins(vector<int> vptb) {vptBins = vptb;}
   void   DeclareHistograms(bool reduceHistograms);
   void   LoopOverEvents(bool verbose, bool reduceHistograms, string readJetMap);
   void   FillJetMap();
   void   FillRecToRecThroughGenMap();
   bool   GetJetMap(string readJetMap);
   bool   JetMapTreeFound() {return jetMapTreeFound;}
   void   ReadJetMap(int ientry, string readJetMap);
   bool   FillHistograms(bool reduceHistograms);
   void   WriteOutput(string outputPath, bool writeJetMap);
   void   Report();

private:

   //Files
   TFile * fpu;
   TFile * fnopu;
   TFile * fout;

   //Algorithms
   string algo1;
   string algo2;
   JetInfo algo1JetInfo;
   JetInfo algo2JetInfo;
   double  maxDeltaR;

   //Ntuples
   JRAEvent* tpu;
   JRAEvent* tnopu;

   //JEC
   bool ApplyJEC;
   JetCorrectorParameters*        L1JetPar;
   vector<JetCorrectorParameters> vPar;
   FactorizedJetCorrector*        JetCorrector;

   //Maps and items used for looping through jets
   ITS mapTreePU;
   ITS mapTreeNoPU;
   //A map holding the equivalance of jets in two given events
   map<Int_t, Int_t>        jetMap;
   map<TString, TH1*>       histograms;
   map<TString, THnSparse*> hsparse;
   //Used for filling the THnSparse
   Double_t* fValue;
   TTree*                   jetMapTree;
   vector<int>*             recoJetIndexPU;
   vector<int>*             recoJetIndexNoPU;


   //Loop
   vector<int>          vptBins;
   int                  nrefmax;
   int                  nevs;
   int                  NBinsNpvRhoNpu;
   int                  npvRhoNpuBinWidth;
   int                  iIT;
   int                  inpv;  
   int                  inpv_low;
   int                  inpv_high;
   int                  irho;
   int                  irho_low;
   int                  irho_high;
   int                  itnpu;
   int                  itnpu_low;
   int                  itnpu_high;
   int                  inpu;
   int                  inpu_low;
   int                  inpu_high;
   double               weight;
   bool                 useweight;
   bool                 pThatReweight;
   double               bias2SelectionRef;
   double               bias2SelectionPow;
   edm::LumiReWeighting LumiWeights_;
   bool                 LumiWeightsSet_;
   bool                 jetMapTreeFound;
   bool                 doNotSaveFlag;

   //Debug
   bool iftest;
   int  noPUNpvGTOneEventCounter;
   int  maxEvts;

   double avg_debug;
   int entries_debug;
   Int_t *bins_debug;
};

////////////////////////////////////////////////////////////////////////////////
// define class
////////////////////////////////////////////////////////////////////////////////
//______________________________________________________________________________
MatchEventsAndJets::MatchEventsAndJets() : algo1("ak5pf"), algo2("ak5pf"), iftest(false) {
   JetCorrector = 0;
   nevs = 0;
   NBinsNpvRhoNpu = 6;
   npvRhoNpuBinWidth = 5;
   noPUNpvGTOneEventCounter = 0;
   maxEvts = 0;
   algo1JetInfo = JetInfo(algo1);
   algo2JetInfo = JetInfo(algo2);
   getMaxDeltaR();
   LumiWeightsSet_ = false;
   recoJetIndexPU = NULL;
   recoJetIndexNoPU = NULL;
   jetMapTree = NULL;
   fValue = NULL;
}

//______________________________________________________________________________
MatchEventsAndJets::MatchEventsAndJets(string algo1_, string algo2_, bool iftest_)
                              : algo1(algo1_), algo2(algo2_), iftest(iftest_){
   JetCorrector = 0;
   nevs = 0;
   NBinsNpvRhoNpu = 6;
   npvRhoNpuBinWidth = 5;
   noPUNpvGTOneEventCounter = 0;
   maxEvts = 0;
   algo1JetInfo = JetInfo(algo1);
   algo2JetInfo = JetInfo(algo2);
   getMaxDeltaR();
   LumiWeightsSet_ = false;
   recoJetIndexPU = NULL;
   recoJetIndexNoPU = NULL;
   jetMapTree = NULL;
   fValue = NULL;
}

//______________________________________________________________________________
void MatchEventsAndJets::SetWeightParameters(bool useweight_, bool pThatReweight_, double bias2SelectionRef_, double bias2SelectionPow_) {
   useweight = useweight_;
   pThatReweight = pThatReweight_;
   bias2SelectionRef = bias2SelectionRef_;
   bias2SelectionPow = bias2SelectionPow_;
}


//______________________________________________________________________________
void MatchEventsAndJets::SetupLumiWeights(string dataFile, string mcFile, string dataHist, string mcHist) {
   if(!dataFile.empty() && !mcFile.empty()) {
      LumiWeights_    = edm::LumiReWeighting(mcFile,dataFile,mcHist,dataHist);
      LumiWeightsSet_ = true;
      cout << endl;
   }
   else {
      cout << "WARNING::MatchEventsAndJets::SetupLumiWeights LumiWeights not set." << std::endl
           << "\tOne or both of the input files was not set." << endl << endl;
   }
}

//______________________________________________________________________________
void MatchEventsAndJets::getMaxDeltaR() {
   double minConeSize = min(algo1JetInfo.coneSize,algo2JetInfo.coneSize)/20.0;
   maxDeltaR = min(0.25,minConeSize);

   cout << setw(10) << " " << setw(13) << "Algo1" << setw(13) << "Algo2" << endl;
   cout << std::setfill ('=') << setw(34) << " " << std::setfill (' ') << endl;
   cout << setw(10) << "Cone Size:" << setw(13) << algo1JetInfo.coneSize
        << setw(13) << algo2JetInfo.coneSize << endl;
   cout << setw(10) << "Jet Type:" << setw(13) << algo1JetInfo.jetType
        << setw(13) << algo2JetInfo.jetType << endl;
   cout << setw(10) << "Alias:" << setw(13) << algo1JetInfo.alias
        << setw(13) << algo2JetInfo.alias << endl;
   cout << std::setfill ('-') << setw(34) << " " << std::setfill (' ') << endl;
   cout << setw(18) << "0.5 * minConeSize:" << setw(4) << minConeSize << endl;
   cout << setw(18) << "maxDeltaR:" << setw(4) << maxDeltaR << endl << endl;
}

//______________________________________________________________________________
double MatchEventsAndJets::getMaxDeltaR(string algName) {
   return min(0.25,JetInfo(algName).coneSize/20.0);
}

//______________________________________________________________________________
void MatchEventsAndJets::OpenInputFiles(string filenamePU, string filenameNoPU) {
   fpu = TFile::Open(filenamePU.c_str(),"READ");
   fnopu = TFile::Open(filenameNoPU.c_str(),"READ");
}

//______________________________________________________________________________
void MatchEventsAndJets::MakeMatchedEventsMaps(string treeName, string outputPath) {
   mapTreePU   = fillMap(false, treeName, outputPath);
   mapTreeNoPU = fillMap(true, treeName, outputPath);
}

//______________________________________________________________________________
ITS MatchEventsAndJets::fillMap(bool noPU, string treeName, string outputPath) {
   TFile* f;
   JRAEvent* t;
   string algo;
   ITS mapTree;
   if(noPU == false) {
      f = fpu;
      algo = algo1;
      t = tpu;
   }
   else if(noPU == true) {
      f = fnopu;
      algo = algo2;
      t = tnopu;
   }

   // Load only what's needed this WAY FASTER.
   t->fChain->SetBranchStatus("*",0);
   t->fChain->SetBranchStatus("run",1);
   t->fChain->SetBranchStatus("evt",1);
   t->fChain->SetBranchStatus("refpt",1);
   Int_t lumi = 0;

   cout << "Filling map with event signatures from: "<<endl;
   cout << "\tfile: "<<f->GetName()<< endl;
   cout << "\talgo: "<<algo<< endl;
   cout << "\ttreename: "<<treeName<< endl;
   cout <<"\tprogress:" << endl;

   Long64_t no_ref_events = 0;
   ull nentries = t->fChain->GetEntriesFast();
   for (ull jentry=0; jentry<nentries;jentry++) {
      long long ientry = t->LoadTree(jentry);
      if (ientry < 0) break;
      t->GetEntry(ientry);

      if(t->refpt->size()==0) {
         no_ref_events++;
         continue;
      }

      if(mapTree.find(evtid(t->run, lumi, t->evt, t->refpt->at(0)))!=mapTree.end()) {
         cout << "\tWARNING::This evtid already exists in the map." << endl;
      }

      mapTree[evtid(t->run, lumi, t->evt, t->refpt->at(0))] = std::make_pair(jentry, (ull)ientry);
      loadbar2(jentry+1,nentries,50,"\t\t");
   }

   cout << endl << "\tRead " << mapTree.size() << " unique signatures" << endl;
   if(no_ref_events>0) {
      cout << "\tWARNING::There were " << no_ref_events << " events which don't contain any ref jets" << endl
           << "\t\tThese events will be skipped" << endl;
   }
   t->fChain->SetBranchStatus("*",1);

   if(!doNotSaveFlag)
      WriteMatchedEventsMaps(mapTree,noPU,outputPath);

   return mapTree;
}

//______________________________________________________________________________
void MatchEventsAndJets::ConvertEvtMapToVector(const ITS& mapTree, vector<evtid>& vevtid, vector<pair<ull,ull> >& vll) {
   vevtid.clear();
   vll.clear();
   for(IT::const_iterator it=mapTree.begin(); it!=mapTree.end(); it++) {
      vevtid.push_back(it->first);
      vll.push_back(it->second);
   }
}

//______________________________________________________________________________
void MatchEventsAndJets::ConvertEvtMapToTTree(const ITS& mapTree, TTree* treeMap) {
   evtid tmpid;
   pair<Long64_t,Long64_t> tmpIndex;
   treeMap->Branch("evtid","evtid",&tmpid);
   treeMap->Branch("indices","pair<Long64_t,Long64_t>",&tmpIndex);
   for(IT::const_iterator it=mapTree.begin(); it!=mapTree.end(); it++) {
      tmpid = it->first;
      tmpIndex = it->second;
      treeMap->Fill();
   }
}

//______________________________________________________________________________
void MatchEventsAndJets::WriteMatchedEventsMaps(map<evtid, pair<ull,ull>, evtid> mapTree, bool noPU, string outputPath) {
   TDirectory* curDir = gDirectory;
   string outputFilename = "matchedEventsMaps_"+algo1+"_"+algo2+".root";
   if (algo1 == algo2)
      outputFilename = "matchedEventsMaps_"+algo1+".root";
   outputFilename = outputPath+outputFilename;
   string name = "mapTree";
   cout << "\tWriting " << name+(noPU ? "NoPU" : "PU") << " to " << outputFilename << " ... " << flush; 
   string option = (noPU ? "UPDATE" : "RECREATE");
   TFile* mapFile = TFile::Open(outputFilename.c_str(),option.c_str());
   mapFile->WriteObject(&mapTree,(name+(noPU ? "NoPU" : "PU")).c_str());
   mapFile->Write();
   mapFile->Close();
   cout << "DONE" << endl;
   curDir->cd();
}

//______________________________________________________________________________
void MatchEventsAndJets::ConvertTTreeToMap(ITS& mapTree, TTree* treeMap) {
   evtid* tmpid;
   pair<Long64_t,Long64_t>* tmpIndex;
   treeMap->SetBranchAddress("evtid",&tmpid);
   treeMap->SetBranchAddress("indices",&tmpIndex);
   int nentries = treeMap->GetEntries();
   for(int ientry=0; ientry<nentries; ientry++) {
      loadbar2(ientry+1,nentries,50,"\t\t");
      treeMap->GetEntry(ientry);
      mapTree[*tmpid] = *tmpIndex;
   }
}

//______________________________________________________________________________
void MatchEventsAndJets::ReadMatchedEventsMaps(string pathToMaps) {
   cout << "Reading matched event maps:" << endl
        << "\tfile: " << pathToMaps << endl;

   TDirectory* curDir = gDirectory;
   TFile* mapFile = TFile::Open(pathToMaps.c_str(),"READ");

   auto inMapPointer = (ITS*)mapFile->Get("mapTreePU");
   if(inMapPointer) {
      mapTreePU = *inMapPointer;
      cout << "\tmapTreePU:" << endl
           << "\t\tunique signatures: " << mapTreePU.size() << endl;
   }
   else {
      cout << "ERROR::MatchEventsAndJets::ReadMatchedEventsMaps Could not retrieve the mapTreePU pointer from " << pathToMaps << endl;
      std::terminate();
   }

   inMapPointer = nullptr;
   inMapPointer = (ITS*)mapFile->Get("mapTreeNoPU");
   if(inMapPointer) {
      mapTreeNoPU = *inMapPointer;
      cout << "\tmapTreeNoPU:" << endl
           << "\t\tunique signatures: " << mapTreeNoPU.size() << endl;
   }
   else {
      cout << "ERROR::MatchEventsAndJets::ReadMatchedEventsMaps Could not retrieve the mapTreeNoPU pointer from " << pathToMaps << endl;
      std::terminate();
   }

   mapFile->Close();
   curDir->cd();
}

//______________________________________________________________________________
void MatchEventsAndJets::GetNtuples(string treeName) {
   int algo1_bit_number = (algo1JetInfo.jetType.Contains("calo",TString::kIgnoreCase)) ? 53 : 85;
   int algo2_bit_number = (algo2JetInfo.jetType.Contains("calo",TString::kIgnoreCase)) ? 53 : 85;

   fpu->cd(algo1.c_str());
   tpu   = new JRAEvent((TTree*) fpu->Get((algo1+"/"+treeName).c_str()),algo1_bit_number);

   fnopu->cd(algo2.c_str());
   tnopu = new JRAEvent((TTree*) fnopu->Get((algo2+"/"+treeName).c_str()),algo2_bit_number);
}

//______________________________________________________________________________
void MatchEventsAndJets::OpenOutputFile(string outputPath) {
   string outputFilename = "output_"+algo1+"_"+algo2+".root";
   if (algo1 == algo2)
      outputFilename = "output_"+algo1+".root";
   outputFilename = outputPath+outputFilename;
   fout = new TFile(outputFilename.c_str(),"RECREATE");
}

//______________________________________________________________________________
void MatchEventsAndJets::SetJEC(string JECPar) {
   L1JetPar = new JetCorrectorParameters(JECPar);
   vPar.push_back(*L1JetPar);
   JetCorrector = new FactorizedJetCorrector(vPar);
}

//______________________________________________________________________________
void MatchEventsAndJets::DeclareHistograms(bool reduceHistograms) {

   //=========================================================
   //              DECLARATION OF HISTOS START HERE
   //=========================================================

   //
   // HISTOS OF GENERAL QUANTITIES.General, hence the g_ prefix
   //
   histograms["g_LumiWeight"]  = new TH1D("g_LumiWeight", "g_LumiWeight;LumiWeight;Events", 1000,0,10);
   histograms["g_GenWeight"]   = new TH1D("g_GenWeight", "g_GenWeight;log_{10}(GenWeight);Events", 1000,-48,2);
   histograms["g_pThatWeight"] = new TH1D("g_pThatWeight;log_{10}(pThatWeight);Events","g_pThatWeight", 1000,-48,2);
   histograms["g_weight"]      = new TH1D("g_weight","g_weight;log_{10}(EvtWeight);Events", 1000,-48,2);
   histograms["g_pthat"]       = new TH1D("g_pthat","g_pthat;#hat{p}_{T}^{PU};Events",(int)vpt[NPtBins]/10.0,vpt[0],vpt[NPtBins]);   
   if(!reduceHistograms) {
      histograms["g_nj"]       = new TH2D("g_nj","g_nj",30,0,30,30,0,30);
      histograms["g_npv"]      = new TH2D("g_npv","g_npv",50,0,50,50,0,50);
      histograms["g_rho"]      = new TH2D("g_rho","g_rho",50,0,50,50,0,50);
      histograms["g_deltaNpv"] = new TProfile("g_deltaNpv","g_deltaNpv",50,0,50);
   }

   //
   // HISTOS OF EVENT or JET MATCHING performance, hence the m_ prefix
   //
   if(!reduceHistograms) {
      histograms["m_refpt_diff"]     = new TH1D("m_refpt_diff","refpt_diff;p_{T, j1}- p_{T, j2}; Number of events",300,-150,150);
      histograms["m_refpdgid_diff"]  = new TH1D("m_refpdgid_diff","m_refpdgid_diff;pdgid_{j1}- pdgid_{j2}; Number of events",100,-50,50);
      histograms["m_deltaPthat"]     = new TProfile("m_deltaPthat","m_deltaPthat;pthat_{pu}(GeV);pthat_{pu}-pthat_{nopu}(GeV)",300,0,3000);

      // To show the excees of jets at low-pt for the sample with pu.
      histograms["m_njet_pt_pu"]   = new TH1D("m_njet_pt_pu","m_njet_pt_pu;p_{T}^{RECO};#jets;",NPtBins, vpt); // number of jets
      histograms["m_njet_pt_nopu"] = new TH1D("m_njet_pt_nopu","m_njet_pt-nopu;p_{T}^{RECO};#jets;",NPtBins, vpt); // number of jets
      histograms["m_njet_pthigh_pu"]   = new TH1D("m_njet_pthigh_pu","m_njet_pthigh_pu;p_{T}^{RECO};#jets;",NPtBins, vpt); // number of jets
      histograms["m_njet_pthigh_nopu"] = new TH1D("m_njet_pthigh_nopu","m_njet_pthigh_nopu;p_{T}^{RECO};#jets;",NPtBins, vpt); // number of jets

      // Fraction of matched jets in all, barrel, endcap and forward regions
      histograms["m_frac_nj_pt_b_match_pu"]   = new TProfile("m_frac_nj_pt_b_match_pu","m_frac_nj_pt_b_match_pu;pt_{jet};fraction of matched jets;", NPtBins, vpt);
      histograms["m_frac_nj_pt_b_match_nopu"] = new TProfile("m_frac_nj_pt_b_match_nopu","m_frac_nj_pt_b_match_nopu;pt_{jet};fraction of matched jets;", NPtBins,vpt);
      histograms["m_frac_nj_pt_e_match_pu"]   = new TProfile("m_frac_nj_pt_e_match_pu","m_frac_nj_pt_e_match_pu;pt_{jet};fraction of matched jets;", NPtBins, vpt);
      histograms["m_frac_nj_pt_e_match_nopu"] = new TProfile("m_frac_nj_pt_e_match_nopu","m_frac_nj_pt_e_match_nopu;pt_{jet};fraction of matched jets;", NPtBins,vpt);
      histograms["m_frac_nj_pt_f_match_pu"]   = new TProfile("m_frac_nj_pt_f_match_pu","m_frac_nj_pt_f_match_pu;pt_{jet};fraction of matched jets;", NPtBins, vpt);
      histograms["m_frac_nj_pt_f_match_nopu"] = new TProfile("m_frac_nj_pt_f_match_nopu","m_frac_nj_pt_f_match_nopu;pt_{jet};fraction of matched jets;", NPtBins,vpt);

      // Fraction of matched jets in barrel regions, with different NPV
      histograms["m_frac_nj_pt_b_match_pu_npv10"]   = new TProfile("m_frac_nj_pt_b_match_pu_npv10","m_frac_nj_pt_b_match_pu_npv;pt_{jet};fraction of matched jets;", NPtBins, vpt);
      histograms["m_frac_nj_pt_b_match_nopu_npv1"]  = new TProfile("m_frac_nj_pt_b_match_nopu_npv1","m_frac_nj_pt_b_match_nopu_npv;pt_{jet};fraction of matched jets;", NPtBins,vpt);
      histograms["m_frac_nj_pt_b_match_pu_npv20"]   = new TProfile("m_frac_nj_pt_b_match_pu_npv20","m_frac_nj_pt_b_match_pu_npv;pt_{jet};fraction of matched jets;", NPtBins, vpt);
      histograms["m_frac_nj_pt_b_match_nopu_npv2"]  = new TProfile("m_frac_nj_pt_b_match_nopu_npv2","m_frac_nj_pt_b_match_nopu_npv;pt_{jet};fraction of matched jets;", NPtBins,vpt);
      histograms["m_frac_nj_pt_b_match_pu_npv30"]   = new TProfile("m_frac_nj_pt_b_match_pu_npv30","m_frac_nj_pt_b_match_pu_npv;pt_{jet};fraction of matched jets;", NPtBins, vpt);
      histograms["m_frac_nj_pt_b_match_nopu_npv3"]  = new TProfile("m_frac_nj_pt_b_match_nopu_npv3","m_frac_nj_pt_b_match_nopu_npv;pt_{jet};fraction of matched jets;", NPtBins,vpt);
      histograms["m_frac_nj_pt_b_match_pu_npvO"]    = new TProfile("m_frac_nj_pt_b_match_pu_npvO","m_frac_nj_pt_b_match_pu_npv;pt_{jet};fraction of matched jets;", NPtBins, vpt);
      histograms["m_frac_nj_pt_b_match_nopu_npvO"]  = new TProfile("m_frac_nj_pt_b_match_nopu_npvO","m_frac_nj_pt_b_match_nopu_npv;pt_{jet};fraction of matched jets;", NPtBins,vpt);

      // Fraction of matched jets (reconstructed--->generated) in barrel regions, with different NPV
      histograms["m_frac_nj_pt_b_match_RG_pu"]       = new TProfile("m_frac_nj_pt_b_match_RG_pu","m_frac_nj_pt_b_match_RG_pu;pt_{jet};fraction of matched jets;", NPtBins, vpt);
      histograms["m_frac_nj_pt_e_match_RG_pu"]       = new TProfile("m_frac_nj_pt_e_match_RG_pu","m_frac_nj_pt_e_match_RG_pu;pt_{jet};fraction of matched jets;", NPtBins, vpt);
      histograms["m_frac_nj_pt_f_match_RG_pu"]       = new TProfile("m_frac_nj_pt_f_match_RG_pu","m_frac_nj_pt_f_match_RG_pu;pt_{jet};fraction of matched jets;", NPtBins, vpt);
      histograms["m_frac_nj_pt_b_match_RG_nopu"]     = new TProfile("m_frac_nj_pt_b_match_RG_nopu","m_frac_nj_pt_b_match_RG_nopu;pt_{jet};fraction of matched jets;", NPtBins, vpt);
      histograms["m_frac_nj_pt_e_match_RG_nopu"]     = new TProfile("m_frac_nj_pt_e_match_RG_nopu","m_frac_nj_pt_e_match_RG_nopu;pt_{jet};fraction of matched jets;", NPtBins, vpt);
      histograms["m_frac_nj_pt_f_match_RG_nopu"]     = new TProfile("m_frac_nj_pt_f_match_RG_nopu","m_frac_nj_pt_f_match_RG_nopu;pt_{jet};fraction of matched jets;", NPtBins, vpt);
      histograms["m_frac_nj_pt_b_match_RG_pu_npv10"] = new TProfile("m_frac_nj_pt_b_match_RG_pu_npv10","m_frac_nj_pt_b_match_RG_pu_npv;pt_{jet};fraction of matched jets;", NPtBins, vpt);
      histograms["m_frac_nj_pt_b_match_RG_pu_npv20"] = new TProfile("m_frac_nj_pt_b_match_RG_pu_npv20","m_frac_nj_pt_b_match_RG_pu_npv;pt_{jet};fraction of matched jets;", NPtBins, vpt);
      histograms["m_frac_nj_pt_b_match_RG_pu_npv30"] = new TProfile("m_frac_nj_pt_b_match_RG_pu_npv30","m_frac_nj_pt_b_match_RG_pu_npv;pt_{jet};fraction of matched jets;", NPtBins, vpt);
      histograms["m_frac_nj_pt_b_match_RG_pu_npvO"]  = new TProfile("m_frac_nj_pt_b_match_RG_pu_npvO","m_frac_nj_pt_b_match_RG_pu_npv;pt_{jet};fraction of matched jets;", NPtBins, vpt);

      // Number of all jets vs. NPV in PU sample
      histograms["m_all_nj_npv"] = new TProfile("m_all_nj_npv","m_all_nj_npv;NPV;# of jets/event",30,0.5,30.5);

      // Number of matched jets vs. NPV in PU sample
      histograms["m_matched_nj_npv"] = new TProfile("m_matched_nj_npv","m_matched_nj_npv;NPV;# of jets/event",30,0.5,30.5);

      // Number of unmatched jets vs. NPV in PU sample
      histograms["m_unmatched_nj_npv"] = new TProfile("m_unmatched_nj_npv","m_unmatched_nj_npv;NPV;# of jets/event",30,0.5,30.5);

      // Average jet pt vs. NPV in PU sample
      histograms["m_all_jtpt_npv"] = new TProfile("m_all_jtpt_npv","m_all_jtpt_npv;NPV;<jet pt> (GeV)", 30, 0.5, 30.5);

      // Average matched jet pt vs. NPV in PU sample
      histograms["m_matched_jtpt_npv"] = new TProfile("m_matched_jtpt_npv","m_matched_jtpt_npv;NPV;<jet pt> (GeV)", 30, 0.5, 30.5);

      // Average unmatched jet pt vs. NPV in PU sample
      histograms["m_unmatched_jtpt_npv"] = new TProfile("m_unmatched_jtpt_npv","m_unmatched_jtpt_npv;NPV;<jet pt> (GeV)", 30, 0.5, 30.5);

      // Number of unmatched jets per event in different NPV bins.
      // 6 old NPV bins 0-4, 5-9, 10-14, 15-19, 20-24, 25-29
      for (int n = 0 ; n < NBinsNpvRhoNpu ; n++) {
         TString titles = ";p_{T,J};# PU Jets /event;";
         TString name = Form("m_njet_pt_npv%i_%i_unmatch",n*npvRhoNpuBinWidth,n*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1);
         histograms[name] = new TProfile(name, name+titles, NPtBins, vpt); // number of jets

         titles = ";#eta_{J}; p_{T,J};# PU Jets /event;";
         name = Form("m_njet_etaVspt_npv%i_%i_unmatch",n*npvRhoNpuBinWidth,n*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1);
         histograms[name] = new TProfile2D(name, name+titles, NETA, veta, NPtBins, vpt); // number of jets
      }
   }

   //
   // HISTOS OF PU!!!, hence the p_suffix
   //
   if(!reduceHistograms) {
      histograms["p_rhoVsRho"]       = new TProfile ("p_rhoVsRho","p_rhoVsRho;",80,0,80);
      histograms["p_npvVsNpv"]       = new TProfile ("p_npvVsNpv","p_npvVsNpv;",80,0,80);
      histograms["p_tnpuVsTnpu"]     = new TProfile ("p_tnpuVsTnpu","p_tnpuVsTnpu;",80,0,80);
      histograms["p_npuVsNpu"]     = new TProfile ("p_npuVsNpu","p_npuVsNpu;",80,0,80);
      histograms["p_matchedjet_off"] = new TProfile("p_matchedjet_off","p_matchedjet_off;<p_{T} Offset>_{jets} (GeV);# of matched jets",80,0,80);
      histograms["p_drVsrefpt"]      = new TProfile("p_drVsrefpt","p_drVsrefpt;p_{T}^{GEN};d_{R}",NPtBins, vpt);

      histograms["p_off_etaVsNpv"]        = new TProfile2D("p_off_etaVsNpv","p_off_etaVsNpv;#eta_{j};N_{PV};Offset (p_{T}, GeV)",NETA, veta, 50,0,50);
      histograms["p_off_etaVsRho"]        = new TProfile2D("p_off_etaVsRho","p_off_etaVsRho;#eta_{j};Rho;Offset (p_{T}, GeV)",NETA, veta, 50,0,50);
      histograms["p_off_etaVspueff"]      = new TProfile2D("p_off_etaVsPUEff","p_off_etaVsPUEff;#eta_{j};PU_{Eff};Offset (p_{T}, GeV)",NETA, veta, 75,0,75);
      histograms["p_off_etaVsGenSumPtOA"] = new TProfile2D("p_off_etaVsGenSumPtOA","p_off_etaVsGenSumPtOA;#eta_{j};GenSumPtOA;Offset (p_{T}, GeV)",NETA, veta, 100,0,2000);
      histograms["p_off_etaVsJetPt"]      = new TProfile2D("p_off_etaVsJetPt","p_off_etaVsJetPt;#eta_{j};p_{T}^{pu};Offset (p_{T}, GeV)",NETA, veta,NPtBins, vpt);
      histograms["p_offOverA_etaVsJetPt"] = new TProfile2D("p_offOverA_etaVsJetPt","p_offOverA_etaVsJetPt;#eta_{j};p_{T}^{pu};OffsetOverArea",NETA, veta,NPtBins, vpt);

      histograms["p_off_EOOTVsITVsLOOT"]         = new TProfile3D("p_off_EOOTVsITVsLOOT","p_off_EOOTVsITVsLOOT;EOOT;IT;LOOT",NRHO,vrho,NRHO,vrho,NRHO,vrho);

      //Rho
      histograms["p_offOverA_etaVsRhoVsJetPt"]   = new TProfile3D("p_offOverA_etaVsRhoVsJetPt","p_offOverA_etaVsRhoVsJetPt;#eta_{j};Rho;p_{T}^{gen};OffsetOverAre",NETA,veta,NRHO,vrho,NPtBins,vpt);
      histograms["p_PtAve_etaVsRhoVsJetPt"]      = new TProfile3D("p_PtAve_etaVsRhoVsJetPt","p_PtAve_etaVsRhoVsJetPt;#eta_{j};Rho;p_{T}^{gen};PtAve",NETA,veta,NRHO,vrho,NPtBins,vpt);
      histograms["p_RhoAve_etaVsRhoVsJetPt"]   = new TProfile3D("p_RhoAve_etaVsRhoVsJetPt","p_RhoAve_etaVsRhoVsJetPt;#eta_{j};Rho;p_{T}^{gen};RhoAve",NETA,veta,NRHO,vrho,NPtBins,vpt);

      //NPV
      histograms["p_offOverA_etaVsNPVVsJetPt"]   = new TProfile3D("p_offOverA_etaVsNPVVsJetPt","p_offOverA_etaVsNPVVsJetPt;#eta_{j};NPV;p_{T}^{gen};OffsetOverAre",NETA,veta,NRHO,vrho,NPtBins,vpt);
      histograms["p_PtAve_etaVsNPVVsJetPt"]      = new TProfile3D("p_PtAve_etaVsNPVVsJetPt","p_PtAve_etaVsNPVVsJetPt;#eta_{j};NPV;p_{T}^{gen};PtAve",NETA,veta,NRHO,vrho,NPtBins,vpt);
      histograms["p_RhoAve_etaVsNPVVsJetPt"]   = new TProfile3D("p_RhoAve_etaVsNPVVsJetPt","p_RhoAve_etaVsNPVVsJetPt;#eta_{j};NPV;p_{T}^{gen};RhoAve",NETA,veta,NRHO,vrho,NPtBins,vpt);
   }

   //TNPU
   histograms["p_offOverA_etaVsTnpusVsJetPt"] = new TProfile3D("p_offOverA_etaVsTnpusVsJetPt","p_offOverA_etaVsTnpusVsJetPt;#eta_{j};tnpu;p_{T}^{gen};OffsetOverAre",NETA,veta,NTNPU,vtnpu,NPtBins,vpt);
   histograms["p_PtAve_etaVsTnpusVsJetPt"]    = new TProfile3D("p_PtAve_etaVsTnpusVsJetPt","p_PtAve_etaVsTnpusVsJetPt;#eta_{j};Tnpus;p_{T}^{gen};PtAve",NETA,veta,NTNPU,vtnpu,NPtBins,vpt);
   histograms["p_RhoAve_etaVsTnpusVsJetPt"]   = new TProfile3D("p_RhoAve_etaVsTnpusVsJetPt","p_RhoAve_etaVsTnpusVsJetPt;#eta_{j};Tnpus;p_{T}^{gen};PtAve",NETA,veta,NTNPU,vtnpu,NPtBins,vpt);
   //THnSparse with 4 dimensions
   Int_t bins[4] = {NETA, NRHO, NTNPU, NPtBins};
   Double_t min[4] = {veta[0], vrho[0], vtnpu[0], vpt[0]};
   Double_t max[4] = {veta[NETA-1], vrho[NRHO-1], vtnpu[NTNPU-1], vpt[NPtBins-1]};
   hsparse["p_offOverA_etaRhoVsTnpusVsJetPt"] = new THnSparseF("p_offOverA_etaRhoVsTnpusVsJetPt", "p_offOverA_etaVsRhoVsTnpusVsJetPt", 4, bins, min, max);
   hsparse["p_offOverA_etaRhoVsTnpusVsJetPt"]->SetBinEdges(0,veta);
   hsparse["p_offOverA_etaRhoVsTnpusVsJetPt"]->SetBinEdges(1,vrho);
   hsparse["p_offOverA_etaRhoVsTnpusVsJetPt"]->SetBinEdges(2,vtnpu);
   hsparse["p_offOverA_etaRhoVsTnpusVsJetPt"]->SetBinEdges(3,vpt);
   hsparse["p_offOverA_etaRhoVsTnpusVsJetPt"]->GetAxis(0)->SetTitle("#eta_{j}");
   hsparse["p_offOverA_etaRhoVsTnpusVsJetPt"]->GetAxis(1)->SetTitle("Rho");
   hsparse["p_offOverA_etaRhoVsTnpusVsJetPt"]->GetAxis(2)->SetTitle("tnpu");
   hsparse["p_offOverA_etaRhoVsTnpusVsJetPt"]->GetAxis(3)->SetTitle("p_{T}^{gen}");
   hsparse["p_offOverA_etaRhoVsTnpusVsJetPt"]->Sumw2();
   hsparse["p_PtAve_etaRhoVsTnpusVsJetPt"] = new THnSparseF("p_PtAve_etaRhoVsTnpusVsJetPt", "p_PtAve_etaVsRhoVsTnpusVsJetPt", 4, bins, min, max);
   hsparse["p_PtAve_etaRhoVsTnpusVsJetPt"]->SetBinEdges(0,veta);
   hsparse["p_PtAve_etaRhoVsTnpusVsJetPt"]->SetBinEdges(1,vrho);
   hsparse["p_PtAve_etaRhoVsTnpusVsJetPt"]->SetBinEdges(2,vtnpu);
   hsparse["p_PtAve_etaRhoVsTnpusVsJetPt"]->SetBinEdges(3,vpt);
   hsparse["p_PtAve_etaRhoVsTnpusVsJetPt"]->GetAxis(0)->SetTitle("#eta_{j}");
   hsparse["p_PtAve_etaRhoVsTnpusVsJetPt"]->GetAxis(1)->SetTitle("Rho");
   hsparse["p_PtAve_etaRhoVsTnpusVsJetPt"]->GetAxis(2)->SetTitle("tnpu");
   hsparse["p_PtAve_etaRhoVsTnpusVsJetPt"]->GetAxis(3)->SetTitle("p_{T}^{gen}");
   hsparse["p_PtAve_etaRhoVsTnpusVsJetPt"]->Sumw2();
   hsparse["p_entries_etaRhoVsTnpusVsJetPt"] = new THnSparseF("p_entries_etaRhoVsTnpusVsJetPt", "p_entries_etaRhoVsTnpusVsJetPt", 4, bins, min, max);
   hsparse["p_entries_etaRhoVsTnpusVsJetPt"]->SetBinEdges(0,veta);
   hsparse["p_entries_etaRhoVsTnpusVsJetPt"]->SetBinEdges(1,vrho);
   hsparse["p_entries_etaRhoVsTnpusVsJetPt"]->SetBinEdges(2,vtnpu);
   hsparse["p_entries_etaRhoVsTnpusVsJetPt"]->SetBinEdges(3,vpt);
   hsparse["p_entries_etaRhoVsTnpusVsJetPt"]->GetAxis(0)->SetTitle("#eta_{j}");
   hsparse["p_entries_etaRhoVsTnpusVsJetPt"]->GetAxis(1)->SetTitle("Rho");
   hsparse["p_entries_etaRhoVsTnpusVsJetPt"]->GetAxis(2)->SetTitle("tnpu");
   hsparse["p_entries_etaRhoVsTnpusVsJetPt"]->GetAxis(3)->SetTitle("p_{T}^{gen}");
   hsparse["p_entries_etaRhoVsTnpusVsJetPt"]->Sumw2();
   if (!fValue) fValue = new Double_t[4];
   //if (!bins_debug) {
   //   bins_debug = new Int_t[4];
   //   bins_debug[0] = 11;
   //   bins_debug[1] = 3;
   //   bins_debug[2] = 6;
   //   bins_debug[3] = 4;
   //}

   //NPU
   histograms["p_offOverA_etaVsNpusVsJetPt"] = new TProfile3D("p_offOverA_etaVsNpusVsJetPt","p_offOverA_etaVsNpusVsJetPt;#eta_{j};npu;p_{T}^{gen};OffsetOverAre",NETA,veta,NNPU,vnpu,NPtBins,vpt);
   histograms["p_PtAve_etaVsNpusVsJetPt"]    = new TProfile3D("p_PtAve_etaVsNpusVsJetPt","p_PtAve_etaVsNpusVsJetPt;#eta_{j};Npus;p_{T}^{gen};PtAve",NETA,veta,NNPU,vnpu,NPtBins,vpt);
   histograms["p_RhoAve_etaVsNpusVsJetPt"]   = new TProfile3D("p_RhoAve_etaVsNpusVsJetPt","p_RhoAve_etaVsNpusVsJetPt;#eta_{j};Npus;p_{T}^{gen};PtAve",NETA,veta,NNPU,vnpu,NPtBins,vpt);

   if(!reduceHistograms) {
      //NPV+Rho
      histograms["p_offOverA_etaVsN_RVsJetPt"]   = new TProfile3D("p_offOverA_etaVsN_RVsJetPt","p_offOverA_etaVsN_RVsJetPt;#eta_{j};(NPV+Rho)/2;p_{T}^{gen};OffsetOverAre",NETA,veta,NRHO,vrho,NPtBins,vpt);
      histograms["p_PtAve_etaVsN_RVsJetPt"]      = new TProfile3D("p_PtAve_etaVsN_RVsJetPt","p_PtAve_etaVsN_RVsJetPt;#eta_{j};(NPV+Rho)/2;p_{T}^{gen};PtAve",NETA,veta,NRHO,vrho,NPtBins,vpt);

      histograms["p_offsetOA_rho_npv_refpt_BB"]  = new TProfile3D("p_offsetOA_rho_npv_refpt_BB","p_offsetOA_rho_npv_refpt_BB;Rho;N_{PV};p_{T}^{GEN};offsetOA",NRHO,vrho,NRHO,vrho,NPtBins,vpt);
      histograms["p_npvVsoff"]               = new TH2F("p_npvVsOff","p_npvVsOff;<p_{T} Offset>_{jets} (GeV);N_{PV}",80,0,80,80,0,80);
      histograms["p_rhoVsoff"]               = new TH2F("p_rhoVsOff","p_rhoVsOff;<p_{T} Offset>_{jets} (GeV);Rho",80,0,80,80,0,80);
      histograms["p_areaVsrefpt"]            = new TH2F("p_areaVsrefpt","p_areaVsrefpt;p_{T}^{GEN};jtarea^{pu}-jtarea^{nopu}",NPtBins, vpt,100,-1,1);
      histograms["p_areaVsoffset_1000"]      = new TH2F("p_areaVsoffset_1000","p_areaVsoffset_1000;offset;jtarea^{pu}-jtarea^{nopu}",100,-500,500,100,-1,1);
      histograms["p_areaVsoffset_30_50"]     = new TH2F("p_areaVsoffset_30_50","p_areaVsoffset_30_50;offset;jtarea^{pu}-jtarea^{nopu}",100,-100,100,100,-1,1);

      histograms["p_rho_npv_refpt_BB"] = new TH3F("p_rho_npv_refpt_BB","p_rho_npv_refpt_BB;Rho;N_{PV};p_{T}^{GEN}",NRHO,vrho,NRHO,vrho,NPtBins,vpt);
   }

  // Break into 4 different detector region
  TString hname = "";
  for (int det=0;det<NDetectorNames;det++) {
      TString detectorAbbreviation = JetInfo::get_detector_abbreviation(detector_names[det]);
      detectorAbbreviation.ToLower();

      //Offset Vs. X where X = {npv, rho, npu, tnpu}
      hname = Form("p_npvVsOff_%s",detectorAbbreviation.Data());
      histograms[hname] =  new TH2D(hname,hname+";<p_{T} Offset>_{jets} (GeV);N_{PV}",
                                    80,0,80,80,0,80);
      hname = Form("p_rhoVsOff_%s",detectorAbbreviation.Data());
      histograms[hname] =  new TH2D(hname,hname+";<p_{T} Offset>_{jets} (GeV);Rho",
                                    80,0,80,80,0,80);

      //Offset Vs. p_{T} in bins of PF category
      for (int iPF=0;iPF<NPFcat;iPF++) {
         hname = Form("p_offResVsrefpt_%s_%s",detectorAbbreviation.Data(),PFstr[iPF].Data());
         histograms[hname] = new TH2D(hname,hname+";p_{T}^{GEN};offset_"+PFstr[iPF]+" (GeV)",NPtBins,vpt,1200,-300,300);
         hname = Form("p_offResOtnpuVsrefpt_%s_%s",detectorAbbreviation.Data(),PFstr[iPF].Data());
         histograms[hname] = new TH2D(hname,hname+";p_{T}^{GEN};offset_"+PFstr[iPF]+"/#mu (GeV)",NPtBins,vpt,1200,-300,300);
         hname = Form("prof_offResVsrefpt_%s_%s",detectorAbbreviation.Data(),PFstr[iPF].Data());
         histograms[hname] = new TProfile(hname,hname+";p_{T}^{GEN};offset_"+PFstr[iPF]+" (GeV)",NPtBins,vpt);
         hname = Form("prof_offResOtnpuVsrefpt_%s_%s",detectorAbbreviation.Data(),PFstr[iPF].Data());
         histograms[hname] = new TProfile(hname,hname+";p_{T}^{GEN};offset_"+PFstr[iPF]+"/#mu (GeV)",NPtBins,vpt);
      }

      //Resolution by detector region (all X bins where X = {npv, rho, npu, tnpu})
      hname = Form("p_offResVsrefpt_%s_all",detectorAbbreviation.Data());
      histograms[hname] = new TH2D(hname,hname+";p_{T}^{GEN};offset (GeV)",
                                   NPtBins,vpt,1200,-300,300);
      hname = Form("p_offResOtnpuVsrefpt_%s_all",detectorAbbreviation.Data());
      histograms[hname] = new TH2D(hname,hname+";p_{T}^{GEN};offset/#mu (GeV)",
                                   NPtBins,vpt,1200,-300,300);
      hname = Form("prof_offResVsrefpt_%s_all",detectorAbbreviation.Data());
      histograms[hname] = new TProfile(hname,hname+";p_{T}^{GEN};offset (GeV)",NPtBins,vpt);
      hname = Form("prof_offResOtnpuVsrefpt_%s_all",detectorAbbreviation.Data());
      histograms[hname] = new TProfile(hname,hname+";p_{T}^{GEN};offset/#mu (GeV)",NPtBins,vpt);
      hname = Form("p_resVsrefpt_%s",detectorAbbreviation.Data());
      histograms[hname] = new TH2D(hname,hname+";p^{GEN}_{T}; p_{T}^{pu}/p_{T}^{GEN};",NPtBins, vpt,100,0,5);
      hname = Form("np_resVsrefpt_%s",detectorAbbreviation.Data());
      histograms[hname] = new TH2D(hname,hname+";p^{GEN}_{T}; p_{T}^{nopu}/p_{T}^{GEN};",NPtBins, vpt,100,0,5);

      //Resolution by detector region in bins of X where X = {npv, rho, npu, tnpu}
      //Same as histograms above, but with explicit high and low X bins instad of the "_all" label
      hname = Form("p_offresVsrefpt_%s_npv%i_%i",detectorAbbreviation.Data(),0,
                   ((NBinsNpvRhoNpu-1)*npvRhoNpuBinWidth)+npvRhoNpuBinWidth-1);
      histograms[hname] = new TH2D(hname,hname+";p^{GEN}_{T}; p_{T}/p_{T}^{nopu};",
                                   NPtBins, vpt,1000,-300,300);
      hname = Form("p_offresVsrefpt_%s_rho%i_%i",detectorAbbreviation.Data(),0,
                   ((NBinsNpvRhoNpu-1)*npvRhoNpuBinWidth)+npvRhoNpuBinWidth-1);
      histograms[hname] = new TH2D(hname,hname+";p^{GEN}_{T}; p_{T}/p_{T}^{nopu};",
                                   NPtBins, vpt,1000,-300,300);
      hname = Form("p_offresVsrefpt_%s_tnpu%i_%i",detectorAbbreviation.Data(),0,
                   ((NBinsNpvRhoNpu-1)*npvRhoNpuBinWidth)+npvRhoNpuBinWidth-1);
      histograms[hname] = new TH2D(hname,hname+";p^{GEN}_{T}; p_{T}/p_{T}^{nopu};",
                                   NPtBins, vpt,1000,-300,300);
      hname = Form("p_offresVsrefpt_%s_npu%i_%i",detectorAbbreviation.Data(),0,
                   ((NBinsNpvRhoNpu-1)*npvRhoNpuBinWidth)+npvRhoNpuBinWidth-1);
      histograms[hname] = new TH2D(hname,hname+";p^{GEN}_{T}; p_{T}/p_{T}^{nopu};",
                                   NPtBins, vpt,1000,-300,300);

      // Generate 4 histograms for eta 4 detector regions, and 6 for the npv regions
      for (int npv=0;npv<NBinsNpvRhoNpu;npv++) {
         // To obtain the response ratio to pt_nopu a function of pT and NPV
         hname = Form("p_resnopuVsrefpt_%s_npv%i_%i",detectorAbbreviation.Data(),
                      npv*npvRhoNpuBinWidth,npv*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1);
         histograms[hname] = new TH2D(hname,hname+";p^{GEN}_{T}; p_{T}/p_{T}^{nopu};",
                                      NPtBins, vpt,100,0,5);
         hname = Form("p_nopuresVsrefpt_%s_npv%i_%i",detectorAbbreviation.Data(),
                      npv*npvRhoNpuBinWidth,npv*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1);
         histograms[hname] = new TH2D(hname,hname+";p^{GEN}_{T}; p_{T}^{nopu}/p_{T}^{GEN};",
                                      NPtBins, vpt,100,0,5);
         hname = Form("p_resVsrefpt_%s_npv%i_%i",detectorAbbreviation.Data(),
                      npv*npvRhoNpuBinWidth,npv*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1);
         histograms[hname] = new TH2D(hname,hname+";p^{GEN}_{T}; p_{T}^{pu}/p_{T}^{GEN};",
                                      NPtBins, vpt,100,0,5);

         // To obtain the offset / response ratio to pt_nopu a function of pT and NPV
         hname = Form("p_offresVsrefpt_%s_npv%i_%i",detectorAbbreviation.Data(),
                      npv*npvRhoNpuBinWidth,npv*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1);
         histograms[hname] = new TH2D(hname,hname+";p^{GEN}_{T}; p_{T}^{pu}-p_{T}^{nopu};",
                                      NPtBins, vpt,1000,-300,300);
         hname = Form("p_offresOrefptVsrefpt_%s_npv%i_%i",detectorAbbreviation.Data(),
                      npv*npvRhoNpuBinWidth,npv*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1);
         histograms[hname] = new TH2D(hname,hname+";p^{GEN}_{T}; (p_{T}^{pu}-p_{T}^{nopu})/p_{T}^{GEN};",NPtBins, vpt,40,-10,10);
         hname = Form("p_offAfterOoffBeforeVsrefpt_%s_npv%i_%i",detectorAbbreviation.Data(),
                      npv*npvRhoNpuBinWidth,npv*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1);
         histograms[hname] = new TH2D(hname,hname+";p^{GEN}_{T}; (p_{T}^{pu+L1}-p_{T}^{nopu})/(p_{T}^{pu}-p_{T}^{nopu});",NPtBins, vpt,40,-10,10);

         // To obtain the resolution as a function of pT and Rho
         hname = Form("p_resnopuVsrefpt_%s_rho%i_%i",detectorAbbreviation.Data(),
                      npv*npvRhoNpuBinWidth,npv*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1);
         histograms[hname] = new TH2D(hname,hname+";p^{GEN}_{T}; p_{T}/p_{T}^{nopu};",
                                      NPtBins, vpt,100,0,5);
         hname = Form("p_nopuresVsrefpt_%s_rho%i_%i",detectorAbbreviation.Data(),
                      npv*npvRhoNpuBinWidth,npv*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1);
         histograms[hname] = new TH2D(hname,hname+";p^{GEN}_{T}; p_{T}^{nopu}/p_{T}^{GEN};",
                                      NPtBins, vpt,100,0,5);
         hname = Form("p_resVsrefpt_%s_rho%i_%i",detectorAbbreviation.Data(),
                      npv*npvRhoNpuBinWidth,npv*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1);
         histograms[hname] = new TH2D(hname,hname+";p^{GEN}_{T}; p_{T}^{pu}-p_{T}^{nopu};",
                                      NPtBins, vpt,100,0,5);

         // To obtain the offset/response as a function of pT and Rho
         hname = Form("p_offresVsrefpt_%s_rho%i_%i",detectorAbbreviation.Data(),
                      npv*npvRhoNpuBinWidth,npv*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1);
         histograms[hname] = new TH2D(hname,hname+";p^{GEN}_{T}; p_{T}/p_{T}^{nopu};",
                                      NPtBins, vpt,1000,-300,300);
         hname = Form("p_offresOrefptVsrefpt_%s_rho%i_%i",detectorAbbreviation.Data(),
                      npv*npvRhoNpuBinWidth,npv*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1);
         histograms[hname] = new TH2D(hname,hname+";p^{GEN}_{T}; (p_{T}^{pu}-p_{T}^{nopu})/p_{T}^{GEN};",NPtBins, vpt,40,-10,10);
         hname = Form("p_offAfterOoffBeforeVsrefpt_%s_rho%i_%i",detectorAbbreviation.Data(),
                      npv*npvRhoNpuBinWidth,npv*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1);
         histograms[hname] = new TH2D(hname,hname+";p^{GEN}_{T}; (p_{T}^{pu+L1}-p_{T}^{nopu})/(p_{T}^{pu}-p_{T}^{nopu});",NPtBins, vpt,40,-10,10);

         // To obtain the resolution as a function of pT and TNPU
         hname = Form("p_nopuresVsrefpt_%s_tnpu%i_%i",detectorAbbreviation.Data(),npv*npvRhoNpuBinWidth,npv*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1);
         histograms[hname] = new TH2D(hname,hname+";p^{GEN}_{T}; p_{T}^{nopu}/p_{T}^{GEN};",NPtBins,vpt,100,0,5);
         hname = Form("p_resVsrefpt_%s_tnpu%i_%i",detectorAbbreviation.Data(),npv*npvRhoNpuBinWidth,npv*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1);
         histograms[hname] = new TH2D(hname,hname+";p^{GEN}_{T}; p_{T}^{pu}-p_{T}^{nopu};",NPtBins,vpt,100,0,5);

         // To obtain the offset/response as a function of pT and TNPU
         hname = Form("p_offresVsrefpt_%s_tnpu%i_%i",detectorAbbreviation.Data(),
                      npv*npvRhoNpuBinWidth,npv*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1);
         histograms[hname] = new TH2D(hname,hname+";p^{GEN}_{T}; p_{T}/p_{T}^{nopu};",NPtBins, vpt,1000,-300,300);

         // To obtain the resolution as a function of pT and NPU
         hname = Form("p_nopuresVsrefpt_%s_npu%i_%i",detectorAbbreviation.Data(),npv*npvRhoNpuBinWidth,npv*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1);
         histograms[hname] = new TH2D(hname,hname+";p^{GEN}_{T}; p_{T}^{nopu}/p_{T}^{GEN};",NPtBins,vpt,100,0,5);
         hname = Form("p_resVsrefpt_%s_npu%i_%i",detectorAbbreviation.Data(),npv*npvRhoNpuBinWidth,npv*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1);
         histograms[hname] = new TH2D(hname,hname+";p^{GEN}_{T}; p_{T}^{pu}-p_{T}^{nopu};",NPtBins,vpt,100,0,5);

         // To obtain the offset/response as a function of pT and NPU
         hname = Form("p_offresVsrefpt_%s_npu%i_%i",detectorAbbreviation.Data(),npv*npvRhoNpuBinWidth,npv*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1);
         histograms[hname] = new TH2D(hname,hname+";p^{GEN}_{T}; p_{T}/p_{T}^{nopu};",NPtBins, vpt,1000,-300,300);
    }//X bin {npv, rho, tnpu, npu}

    for (int ipt=0;ipt<NPtBins;ipt++) {
       hname = Form("p_resVsnpu_%s_pt%.1f_%.1f",detectorAbbreviation.Data(),vpt[ipt],vpt[ipt+1]);
       histograms[hname] = new TH2D(hname,hname+";N_{PU}; p_{T}^{pu}-p_{T}^{nopu};",40,0,200,100,0,5);
    }//pt

    //0, 1-3, 4, 5, 21, all, quarks
    for (int ipdgid=0;ipdgid<NPDGIDcat;ipdgid++) {
         hname = Form("p_offresVsrefpt_%s_pdgid_%s",detectorAbbreviation.Data(),pdgidstr[ipdgid].Data());
         histograms[hname] = new TH2D(hname,hname+";p^{GEN}_{T}; p_{T}/p_{T}^{nopu};",NPtBins, vpt,1000,-300,300);
    }//pdgid
  }

  //=========================================================
  //              DECLARATION OF HISTOS ENDS HERE
  //=========================================================

  //=========================================================
  //              SET SUMW2 FOR ALL HISTOS
  //=========================================================
  for(map<TString, TH1*>::iterator it=histograms.begin(); it!=histograms.end(); ++it) {
      it->second->Sumw2();
  }
}

//______________________________________________________________________________
void MatchEventsAndJets::LoopOverEvents(bool verbose, bool reduceHistograms, string readJetMap) {
   //First just figure out if the jetMapTree exists, assuming readJetMap is set.
   //It might be that the program failed after the event mapping, so the event maps exist, but not the jet maps
   //In this case the event maps should be read, but the jet maps should be recreated.
   jetMapTreeFound = (readJetMap.empty()) ? false : GetJetMap(readJetMap);

   cout << endl << "Looping over the mapped events:" << endl << "\tprogress:" << endl;
   ull nentries = mapTreePU.size();
   int jetMapIndex = -1;
   for (IT::const_iterator it = mapTreePU.begin(); it != mapTreePU.end(); ++it) {

      if (iftest && nevs >= maxEvts) return;

      //if (nevs%10000==0) cout << "\t"<<nevs << endl;
      loadbar2(nevs+1,nentries,50,"\t\t");

      // if this entry does not exist on the second ntuple just skip this event
      if (mapTreeNoPU.find(it->first) == mapTreeNoPU.end()) {
         if(verbose) {
            cout << "\tWARNING::mapTreeNoPU.find(it->first) == mapTreeNoPU.end() failed" << endl
                 << "\tit->first.run_ == " << it->first.run() << endl
                 << "\tit->first.ls_ == " << it->first.luminosityBlock() << endl
                 << "\tit->first.evt_ == " << it->first.event() << endl
                 << "\tit->first.refpt0_ == " << it->first.pt() << endl;
         }
         continue;
      }

      // Load the entries at the proper place.
      tpu->GetEntry(mapTreePU[it->first].second);
      tnopu->GetEntry(mapTreeNoPU[it->first].second);

      //Skip events without any primary vertex as these make no sense
      if (tpu->npv == 0 || tnopu->npv == 0) continue;

      // Set the in-time pileup index after the first event only
      if(nevs==0) iIT = tpu->itIndex();

      // Create the mapping of matched jets.
      // key is PU, value is for NoPU
      //if(!readJetMap) FillJetMap();
      if(readJetMap.empty() || !jetMapTreeFound) {
         FillRecToRecThroughGenMap();
      }
      else {
         jetMapIndex++;
         ReadJetMap(jetMapIndex,readJetMap);
      }  

      if(FillHistograms(reduceHistograms)) nevs++;

   }//for
}

//______________________________________________________________________________
void MatchEventsAndJets::FillJetMap() {
   // Create an aux map with the dR of all the possible
   // combinations of jets in both events
   map<double, pair<Int_t, Int_t> >  auxMap;

   // Order j1 according to reco-jet pT
   vector<int> j1o;
   j1o.push_back(0);
   for (int j1=1; j1 < tpu->nref; j1++){
      for (unsigned ito = 0; ito < j1o.size(); ++ito){
         if (tpu->jtpt->at(j1) > tpu->jtpt->at(j1o[ito])){
            j1o.insert(j1o.begin()+ito,j1);
            break;
         }
      }
   }
   if (nrefmax > 0 && nrefmax < (int)j1o.size()) j1o.resize(nrefmax);

   for (int j1=0; j1 < tpu->nref; j1++){
      for (int j2=0; j2 < tnopu->nref; j2++){
         double dR  = pow(tpu->jteta->at(j1o[j1]) - tnopu->jteta->at(j2),2);
         dR += pow(tpu->jtphi->at(j1o[j1]) - tnopu->jtphi->at(j2),2);
         dR = sqrt(dR);
         auxMap[dR] = std::make_pair(j1o[j1], j2);
      }
   }

   // First clear the map for this new set of events
   jetMap.clear();

   // 1-Find the pair of jets with the smallest dr. Add them to resulting map
   // 2-Remove the pair from the map, rinse and repeat.
   while (auxMap.size() > 0){
      // 1- The first element which is the one with the smallest dR. Get the jet indexes
      int j1 = auxMap.begin()->second.first;
      int j2 = auxMap.begin()->second.second;

      // Add to the results
      if (auxMap.begin()->first < maxDeltaR && fabs(tpu->refpt->at(j1) - tnopu->refpt->at(j2))<1 && tnopu->refdrjt->at(j2) < maxDeltaR)
         jetMap[j1] = j2;

      // 2- Now remove all elements from the auxMap that contain either the first or second jet
      ITJ::iterator itr = auxMap.begin();
      while(itr != auxMap.end()){
         if (itr->second.first == j1 || itr->second.second == j2)
            auxMap.erase(itr++);
         else
            ++itr;
      }//while removing
   }//while
}//FillJetMap

//______________________________________________________________________________
void MatchEventsAndJets::FillRecToRecThroughGenMap() {
   jetMap.clear();
   if(!recoJetIndexPU) recoJetIndexPU = new vector<int>;
   if(!recoJetIndexNoPU) recoJetIndexNoPU = new vector<int>;
   recoJetIndexPU->clear();
   recoJetIndexNoPU->clear();
   if (nrefmax>=0) tpu->nref = std::min((int)tpu->nref,nrefmax);
   for (int iRefPU=0; iRefPU < tpu->nref; iRefPU++) {
      int j1 = iRefPU;
      int j2 = -1;
      for (int iRefNoPU=0; iRefNoPU < tnopu->nref; iRefNoPU++) {
         if(tnopu->refpt->at(iRefNoPU)==tpu->refpt->at(iRefPU)) {
            j2 = iRefNoPU;
            break;
         }
      }
      if(j1 >= 0 && j2 >= 0 && j1 < tpu->nref && j2 < tnopu->nref &&
         tpu->refdrjt->at(j1) < maxDeltaR && tnopu->refdrjt->at(j2) < maxDeltaR && 
         fabs(tpu->refpt->at(j1) - tnopu->refpt->at(j2))<0.0001) {
         jetMap[j1] = j2;
      }
      recoJetIndexPU->push_back(j1);
      recoJetIndexNoPU->push_back(j2);
   }

   //Initialize the jetMapTree if it has not already been initialized
   if(!jetMapTree) {
      jetMapTree = new TTree("jetMapTree","jetMapTree");
      jetMapTree->Branch("recoJetIndexPU",  "vector<int>",&recoJetIndexPU);
      jetMapTree->Branch("recoJetIndexNoPU","vector<int>",&recoJetIndexNoPU);
      jetMapTree->SetDirectory(0);
   }
   //Always fill the jet map tree
   jetMapTree->Fill();

}//FillRecToRecThroughGenMap

//______________________________________________________________________________
bool MatchEventsAndJets::GetJetMap(string readJetMap) {
   //Retrieve the tree from a file
   cout << endl << "\tReading matched jets tree:" << endl
        << "\t\tfile: " << readJetMap << endl;

   TDirectory* curDir = gDirectory;
   TFile* mapFile = TFile::Open(readJetMap.c_str(),"READ");

   auto inTreePointer = (TTree*)mapFile->Get("jetMapTree");
   if(inTreePointer) {
      jetMapTree = (TTree*)inTreePointer->Clone();
      cout << "\t\tjetMapTree:" << endl
           << "\t\t\tnevts: " << jetMapTree->GetEntries() << endl;
   }
   else {
      cout << "\t\tWARNING::MatchEventsAndJets::GetJetMap Could not retrieve the jetMapTree pointer from " << readJetMap << endl;
      return false;
   }
   jetMapTree->SetDirectory(0);
   jetMapTree->SetBranchAddress("recoJetIndexPU",  &recoJetIndexPU);
   jetMapTree->SetBranchAddress("recoJetIndexNoPU",&recoJetIndexNoPU);

   mapFile->Close();
   curDir->cd();
   return true;
}

//______________________________________________________________________________
void MatchEventsAndJets::ReadJetMap(int ientry, string readJetMap) {
   //Just a small sanity check
   if(!jetMapTree) {
      cout << "ERROR::MatchEventsAndJets::ReadJetMap At this point the jetMapTree should have been retrieved from " << readJetMap << endl;
      std::terminate();
   }

   //Create the map based on the vectors from the jetMapTree
   jetMapTree->GetEntry(ientry);
   jetMap.clear();
   for(unsigned int i = 0; i<recoJetIndexPU->size() && ((nrefmax>=0)?i<(unsigned)nrefmax:true); i++) {
      if(recoJetIndexPU->at(i) >= 0 && recoJetIndexNoPU->at(i) >= 0 &&
         recoJetIndexPU->at(i) < tpu->nref && recoJetIndexNoPU->at(i) < tnopu->nref &&
         tpu->refdrjt->at(recoJetIndexPU->at(i)) < maxDeltaR &&
         tnopu->refdrjt->at(recoJetIndexNoPU->at(i)) < maxDeltaR &&
         fabs(tpu->refpt->at(recoJetIndexPU->at(i)) - tnopu->refpt->at(recoJetIndexNoPU->at(i)))<0.0001) {
         jetMap[recoJetIndexPU->at(i)] = recoJetIndexNoPU->at(i);
      }
   }
}


//______________________________________________________________________________
bool MatchEventsAndJets::FillHistograms(bool reduceHistograms) {
   //=========================================================
   //              FILLING OF HISTOS START HERE
   //=========================================================

   //
   // retrieve the correct weight and fill some histograms to keep track of them
   //
   weight = 1.0;
   if(useweight) {
      weight *= tpu->weight;
      dynamic_cast<TH1D*>(histograms["g_GenWeight"])     ->Fill(log(tpu->weight));                          // The GenWeight
   }
   if(LumiWeightsSet_) {
      weight *= LumiWeights_.weight(tpu->tnpus->at(iIT));
      dynamic_cast<TH1D*>(histograms["g_LumiWeight"])    ->Fill(LumiWeights_.weight(tpu->tnpus->at(iIT)));  // The LumiWeight
   }
   if(pThatReweight) {
      weight *= pow(tpu->pthat/bias2SelectionRef,bias2SelectionPow);
      dynamic_cast<TH1D*>(histograms["g_pThatWeight"])   ->Fill(log(pow(tpu->pthat/bias2SelectionRef,bias2SelectionPow))); // The pThat weight
   }
   dynamic_cast<TH1D*>(histograms["g_weight"])           ->Fill(log(weight));                               // The overall weight

   // GENERAL HISTOS, no cuts.
   dynamic_cast<TH1D*>(histograms["g_pthat"])            ->Fill(tpu->pthat,weight);                         // pthat distributions
   if(!reduceHistograms) {
      dynamic_cast<TH2D*>(histograms["g_nj"])            ->Fill(tpu->nref,tnopu->nref,weight);              // njet distributions
      dynamic_cast<TH2D*>(histograms["g_npv"])           ->Fill(tpu->npv,tnopu->npv,weight);                // npv dist.
      dynamic_cast<TH2D*>(histograms["g_rho"])           ->Fill(tpu->rho,tnopu->rho,weight);                // rho dist
      dynamic_cast<TProfile*>(histograms["g_deltaNpv"])  ->Fill(tpu->npv,tpu->npv - tnopu->npv,weight);     // Does the number of NPV change?
      dynamic_cast<TProfile*>(histograms["m_deltaPthat"])->Fill(tpu->pthat,tpu->pthat-tnopu->pthat,weight); // pthat sanity check
   }

   //Skip events where the noPU sample has more than one vertex
   if (tnopu->npv!=1) {
      noPUNpvGTOneEventCounter++;
      if(noPUNpvGTOneEventCounter==0) {
         cout << "\tWARNING::The NoPU sample has more than 1 PV." << endl
              << "\tSome events will be skipped (including this one)." << endl;
      }
      return false;
   }

   inpv       = JetInfo::getBinIndex(tpu->npv,NBinsNpvRhoNpu,npvRhoNpuBinWidth);
   inpv_low   = inpv*npvRhoNpuBinWidth;
   inpv_high  = inpv*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1;
   irho       = JetInfo::getBinIndex(tpu->rho,NBinsNpvRhoNpu,npvRhoNpuBinWidth);
   irho_low   = irho*npvRhoNpuBinWidth;
   irho_high  = irho*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1;
   itnpu      = JetInfo::getBinIndex(tpu->tnpus->at(iIT),NBinsNpvRhoNpu,npvRhoNpuBinWidth);
   itnpu_low  = itnpu*npvRhoNpuBinWidth;
   itnpu_high = itnpu*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1;
   inpu       = JetInfo::getBinIndex(tpu->npus->at(iIT),NBinsNpvRhoNpu,npvRhoNpuBinWidth);
   inpu_low   = inpu*npvRhoNpuBinWidth;
   inpu_high  = inpu*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1;
   TString hname = "";

   //
   // Applying JEC from textfile
   //
   vector<double> tpu_jtpt_raw;
   if (JetCorrector) {
      double correction = 1.0;
      //for (int j1 = 0; j1 < tpu->nref; j1++) {
      for (map<Int_t, Int_t>::const_iterator j1it = jetMap.begin(); j1it != jetMap.end(); j1it++) {
         int j1 = j1it->first;
         JetCorrector->setJetEta(tpu->jteta->at(j1));
         JetCorrector->setJetPt(tpu->jtpt->at(j1));
         JetCorrector->setJetA(tpu->jtarea->at(j1));
         JetCorrector->setRho(tpu->rho);
         correction = JetCorrector->getCorrection();
         //cout <<correction<<" "<<tpu->jtpt->at(j1);
         tpu_jtpt_raw.push_back(tpu->jtpt->at(j1));
         tpu->jtpt->at(j1) *= correction;
         //cout <<" "<<tpu->jtpt->at(j1)<<endl;
      }
   }

   if(!reduceHistograms) {
      double avg_jtpt_all       = 0;
      double avg_jtpt_matched   = 0;
      double avg_jtpt_unmatched = 0;

      // fill unmatch jets for PU sample
      for (map<Int_t, Int_t>::const_iterator j1it = jetMap.begin(); j1it != jetMap.end(); j1it++) {
         int j1 = j1it->first;
         if(j1 == -1) continue;

         // matching recon-jet with gen-jet
         bool ismatchRG = tpu->refdrjt->at(j1)<getMaxDeltaR(algo1);

         histograms["m_njet_pt_pu"]->Fill(tpu->jtpt->at(j1));
         if(tpu->refpt->at(j1)>10){
            histograms["m_njet_pthigh_pu"]->Fill(tpu->jtpt->at(j1));
         }

         avg_jtpt_all += tpu->jtpt->at(j1);

         // if j1 is matched in this sample
         bool ismatch =  jetMap.find(j1) != jetMap.end();

         if (ismatch) avg_jtpt_matched   += tpu->jtpt->at(j1);
         else     avg_jtpt_unmatched += tpu->jtpt->at(j1);

         if (fabs(tpu->jteta->at(j1))<1.3) {
            histograms["m_frac_nj_pt_b_match_pu"]   ->Fill(tpu->jtpt->at(j1),ismatch);
            histograms["m_frac_nj_pt_b_match_RG_pu"]->Fill(tpu->jtpt->at(j1),ismatchRG);
            if (tpu->npv<=10) {
               histograms["m_frac_nj_pt_b_match_pu_npv10"]   ->Fill(tpu->jtpt->at(j1),ismatch);
               histograms["m_frac_nj_pt_b_match_RG_pu_npv10"]->Fill(tpu->jtpt->at(j1),ismatchRG);
            }
            else if (tpu->npv<=20) {
               histograms["m_frac_nj_pt_b_match_pu_npv20"]   ->Fill(tpu->jtpt->at(j1),ismatch);
               histograms["m_frac_nj_pt_b_match_RG_pu_npv20"]->Fill(tpu->jtpt->at(j1),ismatchRG);
            }
            else if (tpu->npv<=30) {
               histograms["m_frac_nj_pt_b_match_pu_npv30"]   ->Fill(tpu->jtpt->at(j1),ismatch);
               histograms["m_frac_nj_pt_b_match_RG_pu_npv30"]->Fill(tpu->jtpt->at(j1),ismatchRG);
            }
            else {
               histograms["m_frac_nj_pt_b_match_pu_npvO"]   ->Fill(tpu->jtpt->at(j1),ismatch);
               histograms["m_frac_nj_pt_b_match_RG_pu_npvO"]->Fill(tpu->jtpt->at(j1),ismatchRG);
            }
         }
         else if (fabs(tpu->jteta->at(j1))<3) {
            histograms["m_frac_nj_pt_e_match_pu"]   ->Fill(tpu->jtpt->at(j1),ismatch);
            histograms["m_frac_nj_pt_e_match_RG_pu"]->Fill(tpu->jtpt->at(j1),ismatchRG);
         }
         else {
            histograms["m_frac_nj_pt_f_match_pu"]   ->Fill(tpu->jtpt->at(j1),ismatch);
            histograms["m_frac_nj_pt_f_match_RG_pu"]->Fill(tpu->jtpt->at(j1),ismatchRG);
         }
         if (!ismatch) {
            hname = Form("m_njet_pt_npv%i_%i_unmatch",inpv*npvRhoNpuBinWidth,inpv*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1);
            histograms[hname]->Fill(tpu->jtpt->at(j1),+1);
            hname = Form("m_njet_etaVspt_npv%i_%i_unmatch",inpv*npvRhoNpuBinWidth,inpv*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1);
            dynamic_cast<TProfile2D*>(histograms[hname])->Fill(tpu->jteta->at(j1), tpu->jtpt->at(j1),+1);
         }
      }

      if (tpu->nref>0)               avg_jtpt_all        /= (double) tpu->nref;     else avg_jtpt_all = 0;
      if (jetMap.size()>0)           avg_jtpt_matched    /= (double) jetMap.size(); else avg_jtpt_matched = 0;
      if (tpu->nref-jetMap.size()>0) avg_jtpt_unmatched  /= (double) (tpu->nref - jetMap.size()); else avg_jtpt_unmatched = 0;

      histograms["m_all_nj_npv"]      ->Fill(tpu->npv,tpu->nref);
      histograms["m_matched_nj_npv"]  ->Fill(tpu->npv, jetMap.size());
      histograms["m_unmatched_nj_npv"]->Fill(tpu->npv, tpu->nref-jetMap.size());

      histograms["m_all_jtpt_npv"]      ->Fill(tpu->npv, avg_jtpt_all);
      histograms["m_matched_jtpt_npv"]  ->Fill(tpu->npv, avg_jtpt_matched);
      histograms["m_unmatched_jtpt_npv"]->Fill(tpu->npv, avg_jtpt_unmatched);

      // fill unmatch jets for NOPU sample
      for (map<Int_t, Int_t>::const_iterator j1it = jetMap.begin(); j1it != jetMap.end(); j1it++) {
         int j1 = j1it->second;
         if(j1 == -1) continue;

         histograms["m_njet_pt_nopu"]->Fill(tnopu->jtpt->at(j1));
         if(tnopu->refpt->at(j1)>10){
            histograms["m_njet_pthigh_nopu"]->Fill(tnopu->jtpt->at(j1));
         }

         bool ismatchRG = tnopu->refdrjt->at(j1)<getMaxDeltaR(algo2);

         // if j1 is matched in this sample. More complicated b/c I need
         //  to search the value of the map, not the key.
         bool ismatch = false;
         for (map<Int_t, Int_t>::const_iterator itj = jetMap.begin(); itj != jetMap.end(); itj++) {
            if (itj->second == j1){
               ismatch = true;
               break;
            }
         }

         if (fabs(tnopu->jteta->at(j1))<1.3) {
            histograms["m_frac_nj_pt_b_match_nopu"]   ->Fill(tnopu->jtpt->at(j1),ismatch);
            histograms["m_frac_nj_pt_b_match_RG_nopu"]->Fill(tnopu->jtpt->at(j1),ismatchRG);
            if (tpu->npv<=10)
               histograms["m_frac_nj_pt_b_match_nopu_npv1"]->Fill(tnopu->jtpt->at(j1),ismatch);
            else if (tpu->npv<=20)
               histograms["m_frac_nj_pt_b_match_nopu_npv2"]->Fill(tnopu->jtpt->at(j1),ismatch);
            else if (tpu->npv<=30)
               histograms["m_frac_nj_pt_b_match_nopu_npv3"]->Fill(tnopu->jtpt->at(j1),ismatch);
            else
               histograms["m_frac_nj_pt_b_match_nopu_npvO"]->Fill(tnopu->jtpt->at(j1),ismatch);
         }
         else if (fabs(tnopu->jteta->at(j1))<3) {
            histograms["m_frac_nj_pt_e_match_nopu"]   ->Fill(tnopu->jtpt->at(j1),ismatch);
            histograms["m_frac_nj_pt_e_match_RG_nopu"]->Fill(tnopu->jtpt->at(j1),ismatchRG);
         }
         else {
            histograms["m_frac_nj_pt_f_match_nopu"]   ->Fill(tnopu->jtpt->at(j1),ismatch);
            histograms["m_frac_nj_pt_f_match_RG_nopu"]->Fill(tnopu->jtpt->at(j1),ismatchRG);
         }
         if (!ismatch) {
            hname = Form("m_njet_pt_npv%i_%i_unmatch",inpv*npvRhoNpuBinWidth,inpv*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1);
            histograms[hname]->Fill(tnopu->jtpt->at(j1),-1);
            hname = Form("m_njet_etaVspt_npv%i_%i_unmatch",inpv*npvRhoNpuBinWidth,inpv*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1);
            dynamic_cast<TProfile2D*>(histograms[hname])->Fill(tnopu->jteta->at(j1), tnopu->jtpt->at(j1),-1);
         }
      }
   }

   double avg_offset = 0;
   double avg_offset_det[NDetectorNames] = {0,0,0,0};
   double njet_det[NDetectorNames] = {0,0,0,0};
   
   // MATCHING HISTOS. 
   // Loop over matched jets
   int jpu   = -1;
   int jnopu = -1;
   int idet  = -1;
   TString detectorAbbreviation;
   double offset = 0, offset_raw = 0, offsetOA = 0, offsetOrefpt = 0, areaDiff = 0, resp = 0,
          respTonopu = 0, respNopu = 0, PUEff = 0, GenSumPtOA = 0;
   int diff_pdgid = 0;
   vector<double> offset_PFcat;
   for (map<Int_t, Int_t>::const_iterator itj = jetMap.begin(); itj != jetMap.end(); itj++) {

      jpu = itj->first;
      jnopu = itj->second;
      if(jpu == -1 || jnopu == -1) continue;

      idet = JetInfo::getDetIndex(tpu->jteta->at(jpu));
      detectorAbbreviation = JetInfo::get_detector_abbreviation(detector_names[idet]);
      detectorAbbreviation.ToLower();
      vector<int> pdgid_indecies = JetInfo::getPDGIDIndecies(tpu->refpdgid->at(jpu));

      diff_pdgid    = tpu->refpdgid->at(jpu) - tnopu->refpdgid->at(jnopu);
      offset        = tpu->jtpt->at(jpu) - tnopu->jtpt->at(jnopu);
      offset_raw    = (tpu_jtpt_raw.size()>0) ? tpu_jtpt_raw[jpu] - tnopu->jtpt->at(jnopu) : -1.0;
      offsetOA      = offset / tpu->jtarea->at(jpu);
      offsetOrefpt  = offset / tpu->refpt->at(jpu);
      areaDiff      = tpu->jtarea->at(jpu) - tnopu->jtarea->at(jnopu);
      resp          = tpu->jtpt->at(jpu) / tpu->refpt->at(jpu);   // response relative to reference jet
      respTonopu    = tpu->jtpt->at(jpu) / tnopu->jtpt->at(jnopu);// response relative to no pu jet
      respNopu      = tnopu->jtpt->at(jnopu) / tnopu->refpt->at(jnopu); // response no pu jet to reference jet
      PUEff         = 0.020*(tpu->sumEOOT())+0.975*(tpu->npus->at(iIT))+0.005*(tpu->sumLOOT()); // effective pu
      GenSumPtOA    = (0.020*(tpu->sumpt_lowpt->at(0))+0.975*(tpu->sumpt_lowpt->at(1))+0.005*(tpu->sumpt_lowpt->at(2)))/tpu->jtarea->at(jpu);

      if(!reduceHistograms) {
         dynamic_cast<TProfile2D*>(histograms["p_off_etaVsNpv"])       ->Fill(tpu->jteta->at(jpu),tpu->npv,offset,weight);
         dynamic_cast<TProfile2D*>(histograms["p_off_etaVsRho"])       ->Fill(tpu->jteta->at(jpu),tpu->rho,offset,weight);
         dynamic_cast<TProfile2D*>(histograms["p_off_etaVspueff"])     ->Fill(tpu->jteta->at(jpu),PUEff,offset,weight);
         dynamic_cast<TProfile2D*>(histograms["p_off_etaVsGenSumPtOA"])->Fill(tpu->jteta->at(jpu),GenSumPtOA,offset,weight);
         dynamic_cast<TProfile2D*>(histograms["p_off_etaVsJetPt"])     ->Fill(tpu->jteta->at(jpu),tpu->jtpt->at(jpu),offset,weight);
         dynamic_cast<TProfile2D*>(histograms["p_offOverA_etaVsJetPt"])->Fill(tpu->jteta->at(jpu),tpu->jtpt->at(jpu),offsetOA,weight);
         dynamic_cast<TProfile3D*>(histograms["p_off_EOOTVsITVsLOOT"]) ->Fill(tpu->sumEOOT(),tpu->npus->at(iIT),tpu->sumLOOT(),offset,weight);

         //Rho
         dynamic_cast<TProfile3D*>(histograms["p_offOverA_etaVsRhoVsJetPt"])->Fill(tpu->jteta->at(jpu),tpu->rho,tpu->refpt->at(jpu),offsetOA,weight);
         dynamic_cast<TProfile3D*>(histograms["p_RhoAve_etaVsRhoVsJetPt"])  ->Fill(tpu->jteta->at(jpu),tpu->rho,tpu->refpt->at(jpu),tpu->rho,weight);
         dynamic_cast<TProfile3D*>(histograms["p_PtAve_etaVsRhoVsJetPt"])   ->Fill(tpu->jteta->at(jpu),tpu->rho,tpu->refpt->at(jpu),tpu->jtpt->at(jpu),weight);

         //NPV
         dynamic_cast<TProfile3D*>(histograms["p_offOverA_etaVsNPVVsJetPt"])->Fill(tpu->jteta->at(jpu),tpu->npv,tpu->refpt->at(jpu),offsetOA,weight);
         dynamic_cast<TProfile3D*>(histograms["p_RhoAve_etaVsNPVVsJetPt"])  ->Fill(tpu->jteta->at(jpu),tpu->npv,tpu->refpt->at(jpu),tpu->rho,weight);
         dynamic_cast<TProfile3D*>(histograms["p_PtAve_etaVsNPVVsJetPt"])   ->Fill(tpu->jteta->at(jpu),tpu->npv,tpu->refpt->at(jpu),tpu->jtpt->at(jpu),weight);
      }

      //TNPU
      dynamic_cast<TProfile3D*>(histograms["p_offOverA_etaVsTnpusVsJetPt"])->Fill(tpu->jteta->at(jpu),tpu->tnpus->at(iIT),tpu->refpt->at(jpu),offsetOA,weight);
      dynamic_cast<TProfile3D*>(histograms["p_PtAve_etaVsTnpusVsJetPt"])   ->Fill(tpu->jteta->at(jpu),tpu->tnpus->at(iIT),tpu->refpt->at(jpu),tpu->jtpt->at(jpu),weight);
      dynamic_cast<TProfile3D*>(histograms["p_RhoAve_etaVsTnpusVsJetPt"])  ->Fill(tpu->jteta->at(jpu),tpu->tnpus->at(iIT),tpu->refpt->at(jpu),tpu->rho,weight);
      fValue[0] = tpu->jteta->at(jpu);
      fValue[1] = tpu->rho;
      fValue[2] = tpu->tnpus->at(iIT);
      fValue[3] = tpu->refpt->at(jpu);
      hsparse["p_offOverA_etaRhoVsTnpusVsJetPt"]->Fill(fValue,offsetOA*weight);
      hsparse["p_PtAve_etaRhoVsTnpusVsJetPt"]->Fill(fValue,tpu->jtpt->at(jpu)*weight);
      hsparse["p_entries_etaRhoVsTnpusVsJetPt"]->Fill(fValue,weight);
      //if(hsparse["p_offOverA_etaRhoVsTnpusVsJetPt"]->GetBin(fValue)==8500) {
      //  avg_debug +=offsetOA;
      //  entries_debug++;
      //}

      //NPU (weight of NPU if tnpu is single valued at 20 = 1.0/TMath::Gaus(tpu->npus->at(iIT),20,sqrt(20))
      dynamic_cast<TProfile3D*>(histograms["p_offOverA_etaVsNpusVsJetPt"])->Fill(tpu->jteta->at(jpu),tpu->npus->at(iIT),tpu->refpt->at(jpu),offsetOA,weight);
      dynamic_cast<TProfile3D*>(histograms["p_PtAve_etaVsNpusVsJetPt"])   ->Fill(tpu->jteta->at(jpu),tpu->npus->at(iIT),tpu->refpt->at(jpu),tpu->jtpt->at(jpu),weight);
      dynamic_cast<TProfile3D*>(histograms["p_RhoAve_etaVsNpusVsJetPt"])  ->Fill(tpu->jteta->at(jpu),tpu->npus->at(iIT),tpu->refpt->at(jpu),tpu->rho,weight);

      if(!reduceHistograms) {
         //NPV+Rho
         dynamic_cast<TProfile3D*>(histograms["p_offOverA_etaVsN_RVsJetPt"])  ->Fill(tpu->jteta->at(jpu),(tpu->rho+tpu->npv)/2.,tpu->refpt->at(jpu),offsetOA,weight);
         dynamic_cast<TProfile3D*>(histograms["p_PtAve_etaVsN_RVsJetPt"])     ->Fill(tpu->jteta->at(jpu),(tpu->rho+tpu->npv)/2,tpu->refpt->at(jpu),tpu->jtpt->at(jpu),weight);
         dynamic_cast<TH2F*>(histograms["p_areaVsrefpt"])                     ->Fill(tpu->refpt->at(jpu),areaDiff,weight);
         if (tpu->refpt->at(jpu)>1000)
            dynamic_cast<TH2F*>(histograms["p_areaVsoffset_1000"])            ->Fill(offset,areaDiff,weight);
         if (tpu->refpt->at(jpu)>30 && tpu->refpt->at(jpu)<50)
            dynamic_cast<TH2F*>(histograms["p_areaVsoffset_30_50"])           ->Fill(offset,areaDiff,weight);

         dynamic_cast<TProfile*>(histograms["p_drVsrefpt"])->Fill(tpu->refpt->at(jpu),tpu->refdrjt->at(jpu),weight);
         dynamic_cast<TH1D*>(histograms["m_refpt_diff"])   ->Fill(tpu->refpt->at(jpu) - tnopu->refpt->at(jnopu),weight);
         dynamic_cast<TH1D*>(histograms["m_refpdgid_diff"])->Fill(diff_pdgid,weight);

         if (idet == 0) {
            dynamic_cast<TH3F*>(histograms["p_rho_npv_refpt_BB"])               ->Fill(tpu->rho,tpu->npv,tpu->refpt->at(jpu),weight);
            dynamic_cast<TProfile3D*>(histograms["p_offsetOA_rho_npv_refpt_BB"])->Fill(tpu->rho,tpu->npv,tpu->refpt->at(jpu),offsetOA,weight);
         }

         //Break into detector regions and PF candidate types
         //nef,cef,muf,nhf,hfhf,hfef,chf
         if(!algo1JetInfo.jetType.Contains("calo",TString::kIgnoreCase) && !algo2JetInfo.jetType.Contains("calo",TString::kIgnoreCase)) {
            offset_PFcat = vector<double>{tpu->jtpt->at(jpu)*tpu->jtnef->at(jpu) - tnopu->jtpt->at(jnopu)*tnopu->jtnef->at(jnopu),
                                          tpu->jtpt->at(jpu)*tpu->jtcef->at(jpu) - tnopu->jtpt->at(jnopu)*tnopu->jtcef->at(jnopu),
                                          tpu->jtpt->at(jpu)*tpu->jtmuf->at(jpu) - tnopu->jtpt->at(jnopu)*tnopu->jtmuf->at(jnopu),
                                          tpu->jtpt->at(jpu)*tpu->jtnhf->at(jpu) - tnopu->jtpt->at(jnopu)*tnopu->jtnhf->at(jnopu),
                                          tpu->jtpt->at(jpu)*tpu->jthfhf->at(jpu) - tnopu->jtpt->at(jnopu)*tnopu->jthfhf->at(jnopu),
                                          tpu->jtpt->at(jpu)*tpu->jthfef->at(jpu) - tnopu->jtpt->at(jnopu)*tnopu->jthfef->at(jnopu),
                                          tpu->jtpt->at(jpu)*tpu->jtchf->at(jpu) - tnopu->jtpt->at(jnopu)*tnopu->jtchf->at(jnopu)};
         }
         else {
            offset_PFcat = vector<double>{0,0,0,0,0,0,0};
         }
         for (int iPF=0;iPF<NPFcat;iPF++) {
            hname = Form("p_offResVsrefpt_%s_%s",detectorAbbreviation.Data(),PFstr[iPF].Data());
            dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),offset_PFcat[iPF],weight);
            hname = Form("p_offResOtnpuVsrefpt_%s_%s",detectorAbbreviation.Data(),PFstr[iPF].Data());
            dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),offset_PFcat[iPF]/tpu->tnpus->at(iIT),weight);
            hname = Form("prof_offResVsrefpt_%s_%s",detectorAbbreviation.Data(),PFstr[iPF].Data());
            dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),offset_PFcat[iPF],weight);
            hname = Form("prof_offResOtnpuVsrefpt_%s_%s",detectorAbbreviation.Data(),PFstr[iPF].Data());
            dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),offset_PFcat[iPF]/tpu->tnpus->at(iIT),weight);
         }
         hname = Form("p_offResVsrefpt_%s_all",detectorAbbreviation.Data());
         dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),offset,weight);
         hname = Form("p_offResOtnpuVsrefpt_%s_all",detectorAbbreviation.Data());
         dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),offset/tpu->tnpus->at(iIT),weight);
         hname = Form("prof_offResVsrefpt_%s_all",detectorAbbreviation.Data());
         dynamic_cast<TProfile*>(histograms[hname])->Fill(tpu->refpt->at(jpu),offset,weight);
         hname = Form("prof_offResOtnpuVsrefpt_%s_all",detectorAbbreviation.Data());
         dynamic_cast<TProfile*>(histograms[hname])->Fill(tpu->refpt->at(jpu),offset/tpu->tnpus->at(iIT),weight);
      }

      //Break into detector regions
      hname = Form("p_resVsrefpt_%s",detectorAbbreviation.Data());
      dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),resp,weight);
      hname = Form("np_resVsrefpt_%s",detectorAbbreviation.Data());
      dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),respNopu,weight);

      //TNPU
      hname = Form("p_resVsrefpt_%s_tnpu%i_%i",detectorAbbreviation.Data(),itnpu_low,itnpu_high);
      dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),resp,weight);
      hname = Form("p_offresVsrefpt_%s_tnpu%i_%i",detectorAbbreviation.Data(),itnpu_low,itnpu_high);
      dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),offset,weight);
      hname = Form("p_offresVsrefpt_%s_tnpu%i_%i",detectorAbbreviation.Data(),0,((NBinsNpvRhoNpu-1)*npvRhoNpuBinWidth)+npvRhoNpuBinWidth-1);
      dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),offset,weight);
      hname = Form("p_nopuresVsrefpt_%s_tnpu%i_%i",detectorAbbreviation.Data(),itnpu_low,itnpu_high);
      dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),respNopu,weight);

      //NPU
      hname = Form("p_resVsrefpt_%s_npu%i_%i",detectorAbbreviation.Data(),inpu_low,inpu_high);
      dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),resp,weight);
      hname = Form("p_offresVsrefpt_%s_npu%i_%i",detectorAbbreviation.Data(),inpu_low,inpu_high);
      dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),offset,weight);
      hname = Form("p_offresVsrefpt_%s_npu%i_%i",detectorAbbreviation.Data(),0,((NBinsNpvRhoNpu-1)*npvRhoNpuBinWidth)+npvRhoNpuBinWidth-1);
      dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),offset,weight);
      hname = Form("p_nopuresVsrefpt_%s_npu%i_%i",detectorAbbreviation.Data(),inpu_low,inpu_high);
      dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),respNopu,weight);

      if(!reduceHistograms) {
         //NPV
         hname = Form("p_resVsrefpt_%s_npv%i_%i",detectorAbbreviation.Data(),inpv_low,inpv_high);
         dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),resp,weight);
         hname = Form("p_offresVsrefpt_%s_npv%i_%i",detectorAbbreviation.Data(),inpv_low,inpv_high);
         dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),offset,weight);
         hname = Form("p_offresOrefptVsrefpt_%s_npv%i_%i",detectorAbbreviation.Data(),inpv_low,inpv_high);
         dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),offsetOrefpt,weight);
         hname = Form("p_offresVsrefpt_%s_npv%i_%i",detectorAbbreviation.Data(),0,((NBinsNpvRhoNpu-1)*npvRhoNpuBinWidth)+npvRhoNpuBinWidth-1);
         dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),offset,weight);
         hname = Form("p_resnopuVsrefpt_%s_npv%i_%i",detectorAbbreviation.Data(),inpv_low,inpv_high);
         dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),respTonopu,weight);
         hname = Form("p_nopuresVsrefpt_%s_npv%i_%i",detectorAbbreviation.Data(),inpv_low,inpv_high);
         dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),respNopu,weight);
         hname = Form("p_offAfterOoffBeforeVsrefpt_%s_npv%i_%i",detectorAbbreviation.Data(),inpv_low,inpv_high);
         dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),offset/offset_raw,weight);
   
         //RHO
         hname = Form("p_resVsrefpt_%s_rho%i_%i",detectorAbbreviation.Data(),irho_low,irho_high);
         dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),resp,weight);
         hname = Form("p_offresVsrefpt_%s_rho%i_%i",detectorAbbreviation.Data(),irho_low,irho_high);
         dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),offset,weight);
         hname = Form("p_offresOrefptVsrefpt_%s_rho%i_%i",detectorAbbreviation.Data(),irho_low,irho_high);
         dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),offsetOrefpt,weight);
         hname = Form("p_offresVsrefpt_%s_rho%i_%i",detectorAbbreviation.Data(),0,((NBinsNpvRhoNpu-1)*npvRhoNpuBinWidth)+npvRhoNpuBinWidth-1);
         dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),offset,weight);
         hname = Form("p_resnopuVsrefpt_%s_rho%i_%i",detectorAbbreviation.Data(),irho_low,irho_high);
         dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),respTonopu,weight);
         hname = Form("p_nopuresVsrefpt_%s_rho%i_%i",detectorAbbreviation.Data(),irho_low,irho_high);
         dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),respNopu,weight);
         hname = Form("p_offAfterOoffBeforeVsrefpt_%s_rho%i_%i",detectorAbbreviation.Data(),irho_low,irho_high);
         dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),offset/offset_raw,weight);
   
         //OTHER
         hname = Form("p_resVsnpu_%s_pt%.1f_%.1f",detectorAbbreviation.Data(),
                      vpt[JetInfo::getBinIndex(tpu->refpt->at(jpu),vpt,NPtBins)],vpt[JetInfo::getBinIndex(tpu->refpt->at(jpu),vpt,NPtBins)+1]);
         if(tpu->refpt->at(jpu)>10.0) {
            dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->npus->at(iIT),resp,weight);
         }
         for (unsigned int ipdgid=0; ipdgid<pdgid_indecies.size(); ipdgid++) {
            hname = Form("p_offresVsrefpt_%s_pdgid_%s",detectorAbbreviation.Data(),pdgidstr[ipdgid].Data());
            dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),offset,weight);
         }
      }

      avg_offset +=  offset;
      avg_offset_det[idet]+=offset;
      njet_det[idet]+=1.;

   } // for matched jets 

   avg_offset /= jetMap.size();
   for (int det=0;det<NDetectorNames;det++) {
      if (njet_det[det]!=0.) {
         TString detectorAbbreviation = JetInfo::get_detector_abbreviation(detector_names[det]);
         detectorAbbreviation.ToLower();
         avg_offset_det[det]  /= njet_det[det];
         hname = Form("p_npvVsOff_%s",detectorAbbreviation.Data());
         dynamic_cast<TH2D*>(histograms[hname])->Fill(avg_offset_det[det],tpu->npv,weight);
         hname = Form("p_rhoVsOff_%s",detectorAbbreviation.Data());
         dynamic_cast<TH2D*>(histograms[hname])->Fill(avg_offset_det[det],tpu->rho,weight);
      }
   }
   if(!reduceHistograms) {
      dynamic_cast<TH2F*>(histograms["p_npvVsoff"])          ->Fill(avg_offset,tpu->npv,weight);
      dynamic_cast<TH2F*>(histograms["p_rhoVsoff"])          ->Fill(avg_offset,tpu->rho,weight);
      dynamic_cast<TProfile*>(histograms["p_rhoVsRho"])      ->Fill(tpu->rho,tpu->rho,weight);
      dynamic_cast<TProfile*>(histograms["p_npvVsNpv"])      ->Fill(tpu->npv,tpu->npv,weight);
      dynamic_cast<TProfile*>(histograms["p_tnpuVsTnpu"])    ->Fill(tpu->tnpus->at(iIT),tpu->tnpus->at(iIT),weight);
      dynamic_cast<TProfile*>(histograms["p_npuVsNpu"])      ->Fill(tpu->npus->at(iIT),tpu->npus->at(iIT),weight);
      dynamic_cast<TProfile*>(histograms["p_matchedjet_off"])->Fill(avg_offset,jetMap.size(),weight);
   }

   //=========================================================
   //              FILLING OF HISTOS ENDS HERE
   //=========================================================

   return true;
}

//______________________________________________________________________________
void MatchEventsAndJets::WriteOutput(string outputPath, bool writeJetMap){
   cout << "Writing file " << fout->GetName() << " ... " << flush;
   fout->cd();
   for(map<TString, THnSparse*>::const_iterator it=hsparse.begin(); it!=hsparse.end();it++) {
      it->second->Write();
   }
   fout->Write();
   fout->Close();
   cout << "DONE" << endl;

   if(writeJetMap && !doNotSaveFlag) {
      cout << endl << "Saving the jetMapTree to a file for later use ... " << flush;
      string outputFilename = "matchedEventsMaps_"+algo1+"_"+algo2+".root";
      if (algo1 == algo2)
         outputFilename = "matchedEventsMaps_"+algo1+".root";
      outputFilename = outputPath+outputFilename;
      TFile* mapFile = TFile::Open(outputFilename.c_str(),"UPDATE");
      jetMapTree->Write();
      mapFile->Close();
      cout << "DONE" << endl << endl;
   }
}

//______________________________________________________________________________
void MatchEventsAndJets::Report() {
   cout << "Event-matching report" << endl
        << "\t Number of events skipped because the NoPU sample had more than 1 NPV: " << noPUNpvGTOneEventCounter << endl
        << "\t Total number of unique events in first  sample: " << mapTreePU.size() << endl
        << "\t Total number of unique events in second sample: " << mapTreeNoPU.size() << endl
        << "\t Number of matched events we ran over " << nevs << endl;

   //cout << "THnSparse::Debug::Avg OffsetOverA of bin 8500: " << avg_debug/entries_debug << endl;
}

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

// ------------------------------------------------------------------
int main(int argc,char**argv)
{
   CommandLine cl;
   if (!cl.parse(argc,argv)) return 0;
   string       samplePU          = cl.getValue<string>  ("samplePU");
   string       sampleNoPU        = cl.getValue<string>  ("sampleNoPU");
   string       basepath          = cl.getValue<string>  ("basepath", "/fdata/hepx/store/user/aperloff/");
   string       algo1             = cl.getValue<string>  ("algo1",                               "ak5pf");
   string       algo2             = cl.getValue<string>  ("algo2",                               "ak5pf");
   bool         iftest            = cl.getValue<bool>    ("iftest",                                false);
   int          maxEvts           = cl.getValue<int>     ("maxEvts",                               40000);
   int          nrefmax           = cl.getValue<int>     ("nrefmax",                                  -1);
   bool         useweight         = cl.getValue<bool>    ("useweight",                             false);
   bool         pThatReweight     = cl.getValue<bool>    ("pThatReweight",                         false);
   double       bias2SelectionRef = cl.getValue<double>  ("bias2SelectionRef",                        15);
   double       bias2SelectionPow = cl.getValue<double>  ("bias2SelectionPow",                       6.0);
   string       MCPUReWeighting   = cl.getValue<string>  ("MCPUReWeighting",                          "");
   string       MCPUHistoName     = cl.getValue<string>  ("MCPUHistoName",                      "pileup");
   string       DataPUReWeighting = cl.getValue<string>  ("DataPUReWeighting",                        "");
   string       DataPUHistoName   = cl.getValue<string>  ("DataPUHistoName",                    "pileup");
   bool         ApplyJEC          = cl.getValue<bool>    ("ApplyJEC",                              false);
   string       JECpar            = cl.getValue<string>  ("JECpar",               "parameters_ak5pf.txt");
   string       outputPath        = cl.getValue<string>  ("outputPath",                             "./");
   string       readEvtMaps       = cl.getValue<string>  ("readEvtMaps",                              "");
   bool         doNotSave         = cl.getValue<bool>    ("doNotSave",                             false);
   string       treeName          = cl.getValue<string>  ("treeName",                                "t");
   int          npvRhoNpuBinWidth = cl.getValue<int>     ("npvRhoNpuBinWidth",                         5);
   int          NBinsNpvRhoNpu    = cl.getValue<int>     ("NBinsNpvRhoNpu",                            6);
   vector<int>  vptBins           = cl.getVector<int>    ("vptBins",       "14:::18:::20:::24:::28:::30");
   bool         reduceHistograms  = cl.getValue<bool>    ("reduceHistograms",                       true);
   bool         verbose           = cl.getValue<bool>    ("verbose",                               false);
   bool         help              = cl.getValue<bool>    ("help",                                  false);

   if (help) {cl.print(); return 0;}
   if (!cl.check()) return 0;
   cl.print();

   TBenchmark* m_benchmark = new TBenchmark();
   m_benchmark->Reset();
   m_benchmark->Start("event");

   //
   // Do some additional sanity checks
   //

   // Check that if pThatReweight is set then useweight is also set
   if(pThatReweight && !useweight) {
      cout << "ERROR::jet_synchtest_x Can't reweight the pThat spectrum without first using the existing"
           << " weights to return to an unmodified spectrum. Set the \"useweight\" option to true." << endl;
           return -1;
   }

   gEnv->SetValue("TFile.AsyncPrefetching", 1);

   if(outputPath.empty()) outputPath = string(gSystem->pwd())+"/";
   if(outputPath.back() != '/') outputPath+='/';
   if(basepath.back() != '/') basepath+='/';

   MatchEventsAndJets* mej = new MatchEventsAndJets(algo1,algo2,iftest);
   mej->SetDoNotSaveFlag(doNotSave);
   mej->SetMaxEvts(maxEvts);
   mej->SetNRefMax(nrefmax);
   mej->SetWeightParameters(useweight,pThatReweight,bias2SelectionRef,bias2SelectionPow);
   mej->SetupLumiWeights((DataPUReWeighting.empty())? "" : basepath+DataPUReWeighting,
                         (MCPUReWeighting.empty()) ? "" : basepath+MCPUReWeighting,
                         DataPUHistoName,MCPUHistoName);
   mej->OpenInputFiles(basepath+samplePU,basepath+sampleNoPU);
   mej->GetNtuples(treeName);
   if(readEvtMaps.empty())
      mej->MakeMatchedEventsMaps(treeName,outputPath);
   else
      mej->ReadMatchedEventsMaps(readEvtMaps);
   mej->OpenOutputFile(outputPath);
   if (ApplyJEC) {
      cout << "jet_synchtest_x::Setting the JEC parameter file to " << JECpar << " ... ";
      mej->SetJEC(JECpar);
      cout << "DONE" << endl;
   }
   mej->SetNpvRhoNpuValues(NBinsNpvRhoNpu,npvRhoNpuBinWidth);
   mej->SetVptBins(vptBins);
   mej->DeclareHistograms(reduceHistograms);
   mej->LoopOverEvents(verbose,reduceHistograms,readEvtMaps);
   mej->WriteOutput(outputPath,readEvtMaps.empty()||!mej->JetMapTreeFound());
   mej->Report();

   m_benchmark->Stop("event");
   cout << "jet_synchtest_x" << endl
        << "\tCPU time = " << m_benchmark->GetCpuTime("event") << " s" << endl
        << "\tReal time = " << m_benchmark->GetRealTime("event") << " s" << endl;
   delete m_benchmark;

   return 0;
}

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
#include "TBenchmark.h"

// C++ Libraries
#include <iostream>
#include <vector>
#include <map>

// JetMETAnalysis Libraries
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/JRAEvent.h"
#include "JetMETAnalysis/JetUtilities/interface/JetInfo.hh"
#include "JetMETAnalysis/JetUtilities/interface/EvtID.hh"
#include "JetMETAnalysis/JetUtilities/interface/ProgressBar.hh"

// CMSSW Libraries
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"

using namespace std;

typedef map<double, pair<Int_t, Int_t> > ITJ;

/*
const int NPtBins    = 30;//37
const double vpt[NPtBins + 1] = {10,10.5,11,11.5,12,12.5,13,13.5,14,15,17,20,23,27,30,35,40,45,57,72,90,120,150,200,300,400,550,750,1000,1500,2000};//,2500,3000,3500,4000};//,4500,5000,10000};
*/

////////////////////////////////////////////////////////////////////////////////
// declare class
////////////////////////////////////////////////////////////////////////////////

class MatchEventsAndJets {
public:

   MatchEventsAndJets();
   MatchEventsAndJets(TString algo1_, TString algo2_, bool iftest_, bool runDep_);
   ~MatchEventsAndJets() {;}

   void getMaxDeltaR();
   double getMaxDeltaR(TString algName);
   void SetMaxEvts(int me) {maxEvts = me;}
   void OpenInputFiles(TString filenamePU, TString filenameNoPU);
   void MakeMatchedEventsMaps(TString treeName);
   map<evtid, pair<Long64_t, Long64_t>, evtid> fillMap(bool noPU, TString treeName);
   void GetNtuples(TString treeName = "t");
   void OpenOutputFile(TString outputPath = "./", TString order1 = "0", TString order2 = "0");
   void SetJEC(TString JECPar = "parameters_ak4pfHLT.txt");
   void SetNpvRhoNpuValues(int NBins, int Width) {NBinsNpvRhoNpu=NBins; npvRhoNpuBinWidth=Width;}
   void SetVptBins(vector<int> vptb) {vptBins = vptb;}
   void DeclareHistograms(bool reduceHistograms);
   void LoopOverEvents(bool verbose, bool reduceHistograms);
   void FillJetMap();
   bool FillHistograms(bool reduceHistograms);
   void WriteOutput();
   void Report();

private:

   //Files
   TFile * fpu;
   TFile * fnopu;
   TFile * fout;

   //Algorithms
   TString algo1;
   TString algo2;
   JetInfo algo1JetInfo;
   JetInfo algo2JetInfo;
   double maxDeltaR;

   //Ntuples
   JRAEvent* tpu;
   JRAEvent* tnopu;
   
   //JEC
   bool ApplyJEC;
   JetCorrectorParameters* L1JetPar;
   vector<JetCorrectorParameters> vPar;
   FactorizedJetCorrector* JetCorrector;

   //Maps
   map<evtid, pair<Long64_t, Long64_t>, evtid> mapTreePU;
   map<evtid, pair<Long64_t, Long64_t>, evtid> mapTreeNoPU;
   //A map holding the equivalance of jets in two given events
   map<Int_t, Int_t> jetMap;
   map<TString, TH1*> histograms;

   //Loop
   int nevs;
   int NBinsNpvRhoNpu;
   int npvRhoNpuBinWidth;
   vector<int> vptBins;

   //Debug
   bool iftest;
   int  noPUNpvGTOneEventCounter;
   int  maxEvts;

   //Settings
   bool runDep;
};

////////////////////////////////////////////////////////////////////////////////
// define class
////////////////////////////////////////////////////////////////////////////////
//______________________________________________________________________________
MatchEventsAndJets::MatchEventsAndJets() : algo1("ak5pf"), algo2("ak5pf"), iftest(false), runDep(false) {
   JetCorrector = 0;
   nevs = 0;
   NBinsNpvRhoNpu = 6;
   npvRhoNpuBinWidth = 5;
   noPUNpvGTOneEventCounter = 0;
   maxEvts = 0;
   algo1JetInfo = JetInfo(algo1);
   algo2JetInfo = JetInfo(algo2);
   getMaxDeltaR();
}

//______________________________________________________________________________
MatchEventsAndJets::MatchEventsAndJets(TString algo1_, TString algo2_, bool iftest_, bool runDep_)
                              : algo1(algo1_), algo2(algo2_), iftest(iftest_), runDep(runDep_) {
   JetCorrector = 0;
   nevs = 0;
   NBinsNpvRhoNpu = 6;
   npvRhoNpuBinWidth = 5;
   noPUNpvGTOneEventCounter = 0;
   maxEvts = 0;
   algo1JetInfo = JetInfo(algo1);
   algo2JetInfo = JetInfo(algo2);
   getMaxDeltaR();
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
double MatchEventsAndJets::getMaxDeltaR(TString algName) {
   return min(0.25,JetInfo(algName).coneSize/20.0);
}

//______________________________________________________________________________
void MatchEventsAndJets::OpenInputFiles(TString filenamePU, TString filenameNoPU) {
   fpu = TFile::Open(filenamePU,"READ");
   fnopu = TFile::Open(filenameNoPU,"READ");
}

//______________________________________________________________________________
void MatchEventsAndJets::MakeMatchedEventsMaps(TString treeName) {
   mapTreePU   = fillMap(false, treeName);
   mapTreeNoPU = fillMap(true, treeName);
}

//______________________________________________________________________________
map<evtid, pair<Long64_t, Long64_t>, evtid> MatchEventsAndJets::fillMap(bool noPU, TString treeName) {
   TFile* f;
   JRAEvent* t;
   TString algo;
   map<evtid, pair<Long64_t, Long64_t>, evtid> mapTree;
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
   Long64_t nentries = t->fChain->GetEntriesFast();
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = t->LoadTree(jentry);
      if (ientry < 0) break;
      t->GetEntry(ientry);

      if(t->refpt->size()==0) {
         no_ref_events++;
         continue;
      }

      if(mapTree.find(evtid(t->run, lumi, t->evt, t->refpt->at(0), runDep))!=mapTree.end()) {
         cout << "\tWARNING::This evtid already exists in the map." << endl;
      }

      mapTree[evtid(t->run, lumi, t->evt, t->refpt->at(0), runDep)] = std::make_pair(jentry, ientry);
      loadbar2(jentry+1,nentries,50,"\t\t");
   }

   cout << endl << "\tRead " << mapTree.size() << " unique signatures" << endl;
   if(no_ref_events>0) {
      cout << "\tWARNING::There were " << no_ref_events << " events which don't contain any ref jets" << endl
           << "\t\tThese events were be skipped" << endl << endl;
   }
   t->fChain->SetBranchStatus("*",1);
   return mapTree;
}

//______________________________________________________________________________
void MatchEventsAndJets::GetNtuples(TString treeName) {
   //int algo1_bit_number = (algo1JetInfo.jetType.Contains("calo",TString::kIgnoreCase)) ? 53 : 85;
   //int algo2_bit_number = (algo2JetInfo.jetType.Contains("calo",TString::kIgnoreCase)) ? 53 : 85;

   fpu->cd(algo1);
   tpu   = new JRAEvent((TTree*) fpu->Get(algo1+"/"+treeName));
   //tpu   = new JRAEvent((TTree*) fpu->Get(algo1+"/"+treeName),algo1_bit_number);

   fnopu->cd(algo2);
   tnopu = new JRAEvent((TTree*) fnopu->Get(algo2+"/"+treeName));
   //tnopu = new JRAEvent((TTree*) fnopu->Get(algo2+"/"+treeName),algo2_bit_number);
}

//______________________________________________________________________________
void MatchEventsAndJets::OpenOutputFile(TString outputPath, TString order1, TString order2) {
   TString outputFilename = "output_"+algo1+"_"+algo2+"_"+order1+"_"+order2+".root";
   if (algo1.EqualTo(algo2)) 
      outputFilename = "output_"+algo1+"_"+order1+"_"+order2+".root";
   outputFilename = outputPath+outputFilename;
   fout = new TFile(outputFilename,"RECREATE");
}

//______________________________________________________________________________
void MatchEventsAndJets::SetJEC(TString JECPar) {
   L1JetPar = new JetCorrectorParameters(JECPar.Data());
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
   if(!reduceHistograms) {
      histograms["g_nj"]       = new TH2D("g_nj","g_nj",30,0,30,30,0,30);
      histograms["g_npv"]      = new TH2D("g_npv","g_npv",50,0,50,50,0,50);
      histograms["g_rho"]      = new TH2D("g_rho","g_rho",50,0,50,50,0,50);
      histograms["g_pthat"]    = new TH2D("g_pthat","g_pthat",300,0,3000,300,0,300);
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
   }

   //TNPU
   histograms["p_offOverA_etaVsTnpusVsJetPt"] = new TProfile3D("p_offOverA_etaVsTnpusVsJetPt","p_offOverA_etaVsTnpusVsJetPt;#eta_{j};tnpu;p_{T}^{gen};OffsetOverAre",NETA,veta,NTNPU,vtnpu,NPtBins,vpt);
   histograms["p_PtAve_etaVsTnpusVsJetPt"]    = new TProfile3D("p_PtAve_etaVsTnpusVsJetPt","p_PtAve_etaVsTnpusVsJetPt;#eta_{j};Tnpus;p_{T}^{gen};PtAve",NETA,veta,NTNPU,vtnpu,NPtBins,vpt);
   histograms["p_RhoAve_etaVsTnpusVsJetPt"]   = new TProfile3D("p_RhoAve_etaVsTnpusVsJetPt","p_RhoAve_etaVsTnpusVsJetPt;#eta_{j};Tnpus;p_{T}^{gen};PtAve",NETA,veta,NTNPU,vtnpu,NPtBins,vpt);

   //NPU
   histograms["p_offOverA_etaVsNpusVsJetPt"] = new TProfile3D("p_offOverA_etaVsNpusVsJetPt","p_offOverA_etaVsNpusVsJetPt;#eta_{j};npu;p_{T}^{gen};OffsetOverAre",NETA,veta,NNPU,vnpu,NPtBins,vpt);
   histograms["p_PtAve_etaVsNpusVsJetPt"]    = new TProfile3D("p_PtAve_etaVsNpusVsJetPt","p_PtAve_etaVsNpusVsJetPt;#eta_{j};Npus;p_{T}^{gen};PtAve",NETA,veta,NNPU,vnpu,NPtBins,vpt);
   histograms["p_RhoAve_etaVsNpusVsJetPt"]   = new TProfile3D("p_RhoAve_etaVsNpusVsJetPt","p_RhoAve_etaVsNpusVsJetPt;#eta_{j};Npus;p_{T}^{gen};PtAve",NETA,veta,NNPU,vnpu,NPtBins,vpt);

   //Rho
   histograms["p_offOverA_etaVsRhoVsJetPt"]   = new TProfile3D("p_offOverA_etaVsRhoVsJetPt","p_offOverA_etaVsRhoVsJetPt;#eta_{j};Rho;p_{T}^{gen};OffsetOverAre",NETA,veta,NRHO,vrho,NPtBins,vpt);
   histograms["p_PtAve_etaVsRhoVsJetPt"]      = new TProfile3D("p_PtAve_etaVsRhoVsJetPt","p_PtAve_etaVsRhoVsJetPt;#eta_{j};Rho;p_{T}^{gen};PtAve",NETA,veta,NRHO,vrho,NPtBins,vpt);
   histograms["p_RhoAve_etaVsRhoVsJetPt"]   = new TProfile3D("p_RhoAve_etaVsRhoVsJetPt","p_RhoAve_etaVsRhoVsJetPt;#eta_{j};Rho;p_{T}^{gen};RhoAve",NETA,veta,NRHO,vrho,NPtBins,vpt);

   //NPV
   histograms["p_offOverA_etaVsNPVVsJetPt"]   = new TProfile3D("p_offOverA_etaVsNPVVsJetPt","p_offOverA_etaVsNPVVsJetPt;#eta_{j};NPV;p_{T}^{gen};OffsetOverAre",NETA,veta,NRHO,vrho,NPtBins,vpt);
   histograms["p_PtAve_etaVsNPVVsJetPt"]      = new TProfile3D("p_PtAve_etaVsNPVVsJetPt","p_PtAve_etaVsNPVVsJetPt;#eta_{j};NPV;p_{T}^{gen};PtAve",NETA,veta,NRHO,vrho,NPtBins,vpt);
   histograms["p_RhoAve_etaVsNPVVsJetPt"]   = new TProfile3D("p_RhoAve_etaVsNPVVsJetPt","p_RhoAve_etaVsNPVVsJetPt;#eta_{j};NPV;p_{T}^{gen};RhoAve",NETA,veta,NRHO,vrho,NPtBins,vpt);

   if(!reduceHistograms) {
      //NPV+Rho
      histograms["p_offOverA_etaVsN_RVsJetPt"]   = new TProfile3D("p_offOverA_etaVsN_RVsJetPt","p_offOverA_etaVsN_RVsJetPt;#eta_{j};(NPV+Rho)/2;p_{T}^{gen};OffsetOverAre",NETA,veta,NRHO,vrho,NPtBins,vpt);
      histograms["p_PtAve_etaVsN_RVsJetPt"]      = new TProfile3D("p_PtAve_etaVsN_RVsJetPt","p_PtAve_etaVsN_RVsJetPt;#eta_{j};(NPV+Rho)/2;p_{T}^{gen};PtAve",NETA,veta,NRHO,vrho,NPtBins,vpt);
      
      histograms["p_offsetOA_rho_npv_refpt_BB"]  = new TProfile3D("p_offsetOA_rho_npv_refpt_BB","p_offsetOA_rho_npv_refpt_BB;Rho;N_{PV};p_{T}^{GEN};offsetOA",NRHO,vrho,NRHO,vrho,NPtBins,vpt);
      histograms["p_npvVsoff"]               = new TH2F("p_npvVsOff","p_npvVsOff;<p_{T} Offset>_{jets} (GeV);N_{PV}",80,0,80,80,0,80);
      histograms["p_rhoVsoff"]               = new TH2F("p_rhoVsOff","p_rhoVsOff;<p_{T} Offset>_{jets} (GeV);Rho",80,0,80,80,0,80);
      histograms["p_npvVsRho_offset_15_15h"] = new TH2F("p_npvVsRho_offset_15_15h","p_npvVsRho_offset_15_15h;Rho;N_{PV}",80,0,80,80,0,80); //ZQ
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
void MatchEventsAndJets::LoopOverEvents(bool verbose, bool reduceHistograms) {
   for (IT::const_iterator it = mapTreePU.begin(); it != mapTreePU.end(); ++it) {

      if (iftest && nevs >= maxEvts) return;
      
      //if (nevs%10000==0) cout << "\t"<<nevs << endl;

      // if this entry does not exist on the second ntuple just skip this event
      if (mapTreeNoPU.find(it->first) == mapTreeNoPU.end()) {
         if(verbose) {
            cout << "\tWARNING::mapTreeNoPU.find(it->first) == mapTreeNoPU.end() failed" << endl
                 << "\tit->first.run_ == " << it->first.run_ << endl
                 << "\tit->first.ls_ == " << it->first.ls_ << endl
                 << "\tit->first.evt_ == " << it->first.evt_ << endl
                 << "\tit->first.refpt0_ == " << it->first.refpt0_ << endl;
         }
         continue;
      }

      // Load the entries at the proper place.
      tpu->GetEntry(mapTreePU[it->first].second);
      tnopu->GetEntry(mapTreeNoPU[it->first].second);
      if (tpu->npv ==0 || tnopu->npv == 0) continue;
      // HERE correct the jets in tpu if requested

      // Create the mapping of matched jets.
      // key is PU, value is for NoPU
      FillJetMap();

      if(FillHistograms(reduceHistograms)) {
       nevs++;
      }
    
   }//for
}

//______________________________________________________________________________
void MatchEventsAndJets::FillJetMap() {
   // Create an aux map with the dR of all the possible 
   // combinations of jets in both events
   map<double, pair<Int_t, Int_t> >  auxMap;
   for (int j1=0; j1 < tpu->nref; j1++){
      for (int j2=0; j2 < tnopu->nref; j2++){
         double dR  = pow(tpu->jteta->at(j1) - tnopu->jteta->at(j2),2);
         dR += pow(tpu->jtphi->at(j1) - tnopu->jtphi->at(j2),2);
         dR = sqrt(dR);
         auxMap[dR] = std::make_pair(j1, j2);
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
bool MatchEventsAndJets::FillHistograms(bool reduceHistograms) {
   //=========================================================
   //              FILLING OF HISTOS START HERE
   //=========================================================

   // GENERAL HISTOS, no cuts.
   if(!reduceHistograms) {
      histograms["g_nj"]        ->Fill(tpu->nref,tnopu->nref);              // njet distributions
      histograms["g_npv"]       ->Fill(tpu->npv,tnopu->npv);                // npv dist.
      histograms["g_rho"]       ->Fill(tpu->rho_hlt,tnopu->rho_hlt);        // rho dist
      histograms["g_pthat"]     ->Fill(tpu->pthat,tnopu->pthat);           // pthat distributions
      histograms["g_deltaNpv"]  ->Fill(tpu->npv,tpu->npv - tnopu->npv);     // Does the number of NPV change?
      histograms["m_deltaPthat"]->Fill(tpu->pthat,tpu->pthat-tnopu->pthat); // pthat sanity check
   }

   if (tnopu->npv!=1) {
      noPUNpvGTOneEventCounter++;
      if(noPUNpvGTOneEventCounter==0) {
         cout << "\tWARNING::The no PU sample has more than 1 PV." << endl
              << "\tSome events will be skipped (including this one)." << endl;
      }
      return false;
   }
   
   int iIT        = tpu->itIndex();
   int inpv       = JetInfo::getBinIndex(tpu->npv,NBinsNpvRhoNpu,npvRhoNpuBinWidth);
   int inpv_low   = inpv*npvRhoNpuBinWidth;
   int inpv_high  = inpv*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1;
   int irho       = JetInfo::getBinIndex(tpu->rho,NBinsNpvRhoNpu,npvRhoNpuBinWidth);
   int irho_low   = irho*npvRhoNpuBinWidth;
   int irho_high  = irho*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1;
   int itnpu      = JetInfo::getBinIndex(tpu->tnpus->at(iIT),NBinsNpvRhoNpu,npvRhoNpuBinWidth);
   int itnpu_low  = itnpu*npvRhoNpuBinWidth;
   int itnpu_high = itnpu*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1;
   int inpu       = JetInfo::getBinIndex(tpu->npus->at(iIT),NBinsNpvRhoNpu,npvRhoNpuBinWidth);
   int inpu_low   = inpu*npvRhoNpuBinWidth;
   int inpu_high  = inpu*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1;
   TString hname = "";
   
   //
   // Applying JEC from textfile
   //
   vector<double> tpu_jtpt_raw;
   if (JetCorrector) {
      for (int j1 = 0; j1 < tpu->nref; j1++) {
         JetCorrector->setJetEta(tpu->jteta->at(j1));
         JetCorrector->setJetPt(tpu->jtpt->at(j1));
         JetCorrector->setJetA(tpu->jtarea->at(j1));
         JetCorrector->setRho(tpu->rho_hlt);
         double correction = JetCorrector->getCorrection();
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
      for (int j1 = 0; j1 < tpu->nref; j1++){

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
      for (int j1 = 0; j1 < tnopu->nref; j1++) {

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
  for (map<Int_t, Int_t>::const_iterator itj = jetMap.begin(); itj != jetMap.end(); itj++) {

    int jpu = itj->first;
    int jnopu = itj->second;
        
    int idet = JetInfo::getDetIndex(tpu->jteta->at(jpu));
    TString detectorAbbreviation = JetInfo::get_detector_abbreviation(detector_names[idet]);
    detectorAbbreviation.ToLower();
    vector<int> pdgid_indecies = JetInfo::getPDGIDIndecies(tpu->refpdgid->at(jpu));

    //double eta_avg = 0.5*(tpu->jteta->at(jpu)+tnopu->jteta->at(jnopu));
    double offset     = tpu->jtpt->at(jpu) - tnopu->jtpt->at(jnopu);
    double offset_raw = -1.0;
    if(tpu_jtpt_raw.size()>0)
       offset_raw = tpu_jtpt_raw[jpu] - tnopu->jtpt->at(jnopu);
    int diff_pdgid       = tpu->refpdgid->at(jpu) - tnopu->refpdgid->at(jnopu);
    double areaDiff      = tpu->jtarea->at(jpu) - tnopu->jtarea->at(jnopu);
    double resp          = tpu->jtpt->at(jpu) / tpu->refpt->at(jpu);   // response relative to reference jet
    double respTonopu    = tpu->jtpt->at(jpu) / tnopu->jtpt->at(jnopu);// response relative to no pu jet
    double respNopu       = tnopu->jtpt->at(jnopu) / tnopu->refpt->at(jnopu); // response no pu jet to reference jet
    double PUEff      = 0.020*(tpu->sumEOOT())+0.975*(tpu->npus->at(iIT))+0.005*(tpu->sumLOOT()); // effective pu
    double GenSumPtOA    = (0.020*(tpu->sumpt_lowpt->at(0))+0.975*(tpu->sumpt_lowpt->at(1))+0.005*(tpu->sumpt_lowpt->at(2)))/tpu->jtarea->at(jpu);
    //nef,cef,muf,nhf,hfhf,hfef,chf
    vector<double> offset_PFcat;
    //double offset_PFcat[NPFcat];
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
    double offsetOA      = offset / tpu->jtarea->at(jpu);
    double offsetOrefpt  = offset / tpu->refpt->at(jpu);

    if(!reduceHistograms) {
       dynamic_cast<TProfile2D*>(histograms["p_off_etaVsNpv"])                ->Fill(tpu->jteta->at(jpu),tpu->npv,offset);
       dynamic_cast<TProfile2D*>(histograms["p_off_etaVsRho"])                ->Fill(tpu->jteta->at(jpu),tpu->rho_hlt,offset);
       dynamic_cast<TProfile2D*>(histograms["p_off_etaVspueff"])              ->Fill(tpu->jteta->at(jpu),PUEff,offset);
       dynamic_cast<TProfile2D*>(histograms["p_off_etaVsGenSumPtOA"])         ->Fill(tpu->jteta->at(jpu),GenSumPtOA,offset);
       dynamic_cast<TProfile2D*>(histograms["p_off_etaVsJetPt"])              ->Fill(tpu->jteta->at(jpu),tpu->jtpt->at(jpu),offset);
       dynamic_cast<TProfile2D*>(histograms["p_offOverA_etaVsJetPt"])         ->Fill(tpu->jteta->at(jpu),tpu->jtpt->at(jpu),offsetOA);
       dynamic_cast<TProfile3D*>(histograms["p_off_EOOTVsITVsLOOT"])        ->Fill(tpu->sumEOOT(),tpu->npus->at(iIT),tpu->sumLOOT(),offset);
    }
    
    //TNPU
    dynamic_cast<TProfile3D*>(histograms["p_offOverA_etaVsTnpusVsJetPt"])->Fill(tpu->jteta->at(jpu),tpu->tnpus->at(iIT),tpu->refpt->at(jpu),offsetOA);
    dynamic_cast<TProfile3D*>(histograms["p_PtAve_etaVsTnpusVsJetPt"])   ->Fill(tpu->jteta->at(jpu),tpu->tnpus->at(iIT),tpu->refpt->at(jpu),tpu->jtpt->at(jpu));
    dynamic_cast<TProfile3D*>(histograms["p_RhoAve_etaVsTnpusVsJetPt"])  ->Fill(tpu->jteta->at(jpu),tpu->tnpus->at(iIT),tpu->refpt->at(jpu),tpu->rho_hlt);

    //NPU
    //dynamic_cast<TProfile3D*>(histograms["p_offOverA_etaVsNpusVsJetPt"])->Fill(tpu->jteta->at(jpu),tpu->npus->at(iIT),tpu->refpt->at(jpu),offsetOA,1.0/TMath::Gaus(tpu->npus->at(iIT),20,sqrt(20)));
    //dynamic_cast<TProfile3D*>(histograms["p_PtAve_etaVsNpusVsJetPt"])   ->Fill(tpu->jteta->at(jpu),tpu->npus->at(iIT),tpu->refpt->at(jpu),tpu->jtpt->at(jpu),1.0/TMath::Gaus(tpu->npus->at(iIT),20,sqrt(20)));
    //dynamic_cast<TProfile3D*>(histograms["p_RhoAve_etaVsNpusVsJetPt"])  ->Fill(tpu->jteta->at(jpu),tpu->npus->at(iIT),tpu->refpt->at(jpu),tpu->rho,1.0/TMath::Gaus(tpu->npus->at(iIT),20,sqrt(20)));
    dynamic_cast<TProfile3D*>(histograms["p_offOverA_etaVsNpusVsJetPt"])->Fill(tpu->jteta->at(jpu),tpu->npus->at(iIT),tpu->refpt->at(jpu),offsetOA);
    dynamic_cast<TProfile3D*>(histograms["p_PtAve_etaVsNpusVsJetPt"])   ->Fill(tpu->jteta->at(jpu),tpu->npus->at(iIT),tpu->refpt->at(jpu),tpu->jtpt->at(jpu));
    dynamic_cast<TProfile3D*>(histograms["p_RhoAve_etaVsNpusVsJetPt"])  ->Fill(tpu->jteta->at(jpu),tpu->npus->at(iIT),tpu->refpt->at(jpu),tpu->rho_hlt);
    
    //Rho
    dynamic_cast<TProfile3D*>(histograms["p_offOverA_etaVsRhoVsJetPt"])  ->Fill(tpu->jteta->at(jpu),tpu->rho_hlt,tpu->refpt->at(jpu),offsetOA);
    dynamic_cast<TProfile3D*>(histograms["p_RhoAve_etaVsRhoVsJetPt"])  ->Fill(tpu->jteta->at(jpu),tpu->rho_hlt,tpu->refpt->at(jpu),tpu->rho_hlt);
    dynamic_cast<TProfile3D*>(histograms["p_PtAve_etaVsRhoVsJetPt"])     ->Fill(tpu->jteta->at(jpu),tpu->rho_hlt,tpu->refpt->at(jpu),tpu->jtpt->at(jpu));

    //NPV
    dynamic_cast<TProfile3D*>(histograms["p_offOverA_etaVsNPVVsJetPt"])  ->Fill(tpu->jteta->at(jpu),tpu->npv,tpu->refpt->at(jpu),offsetOA);
    dynamic_cast<TProfile3D*>(histograms["p_RhoAve_etaVsNPVVsJetPt"])     ->Fill(tpu->jteta->at(jpu),tpu->npv,tpu->refpt->at(jpu),tpu->rho_hlt);
    dynamic_cast<TProfile3D*>(histograms["p_PtAve_etaVsNPVVsJetPt"])     ->Fill(tpu->jteta->at(jpu),tpu->npv,tpu->refpt->at(jpu),tpu->jtpt->at(jpu));

    if(!reduceHistograms) {
       //NPV+Rho
       dynamic_cast<TProfile3D*>(histograms["p_offOverA_etaVsN_RVsJetPt"])  ->Fill(tpu->jteta->at(jpu),(tpu->rho_hlt+tpu->npv)/2.,tpu->refpt->at(jpu),offsetOA);
       dynamic_cast<TProfile3D*>(histograms["p_PtAve_etaVsN_RVsJetPt"])     ->Fill(tpu->jteta->at(jpu),(tpu->rho_hlt+tpu->npv)/2,tpu->refpt->at(jpu),tpu->jtpt->at(jpu));
       histograms["p_areaVsrefpt"]->Fill(tpu->refpt->at(jpu),areaDiff);
       if (tpu->refpt->at(jpu)>1000)
         histograms["p_areaVsoffset_1000"]->Fill(offset,areaDiff);
       if (tpu->refpt->at(jpu)>30 && tpu->refpt->at(jpu)<50)
         histograms["p_areaVsoffset_30_50"]->Fill(offset,areaDiff);

       histograms["p_drVsrefpt"]    ->Fill(tpu->refpt->at(jpu),tpu->refdrjt->at(jpu));
       histograms["m_refpt_diff"]   ->Fill(tpu->refpt->at(jpu) - tnopu->refpt->at(jnopu));
       histograms["m_refpdgid_diff"]->Fill(diff_pdgid);

       //2D histo npv vs. rho with 15<offset<15.5
       if (offset > 15 && offset < 15.5)  histograms["p_npvVsRho_offset_15_15h"]->Fill(tpu->rho_hlt,tpu->npv);
       if (idet == 0) {
          dynamic_cast<TH3F*>(histograms["p_rho_npv_refpt_BB"])               ->Fill(tpu->rho_hlt,tpu->npv,tpu->refpt->at(jpu));
          dynamic_cast<TProfile3D*>(histograms["p_offsetOA_rho_npv_refpt_BB"])->Fill(tpu->rho_hlt,tpu->npv,tpu->refpt->at(jpu),offsetOA);
       }
    }

   //Break into detector regions
   for (int iPF=0;iPF<NPFcat;iPF++) {
      hname = Form("p_offResVsrefpt_%s_%s",detectorAbbreviation.Data(),PFstr[iPF].Data());
      histograms[hname]->Fill(tpu->refpt->at(jpu),offset_PFcat[iPF]);
      hname = Form("p_offResOtnpuVsrefpt_%s_%s",detectorAbbreviation.Data(),PFstr[iPF].Data());
      histograms[hname]->Fill(tpu->refpt->at(jpu),offset_PFcat[iPF]/tpu->tnpus->at(iIT));
      hname = Form("prof_offResVsrefpt_%s_%s",detectorAbbreviation.Data(),PFstr[iPF].Data());
      histograms[hname]->Fill(tpu->refpt->at(jpu),offset_PFcat[iPF]);
      hname = Form("prof_offResOtnpuVsrefpt_%s_%s",detectorAbbreviation.Data(),PFstr[iPF].Data());
      histograms[hname]->Fill(tpu->refpt->at(jpu),offset_PFcat[iPF]/tpu->tnpus->at(iIT));
   }
   hname = Form("p_offResVsrefpt_%s_all",detectorAbbreviation.Data());
   dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),offset);
   hname = Form("p_offResOtnpuVsrefpt_%s_all",detectorAbbreviation.Data());
   dynamic_cast<TH2D*>(histograms[hname])->Fill(tpu->refpt->at(jpu),offset/tpu->tnpus->at(iIT));
   hname = Form("prof_offResVsrefpt_%s_all",detectorAbbreviation.Data());
   dynamic_cast<TProfile*>(histograms[hname])->Fill(tpu->refpt->at(jpu),offset);
   hname = Form("prof_offResOtnpuVsrefpt_%s_all",detectorAbbreviation.Data());
   dynamic_cast<TProfile*>(histograms[hname])->Fill(tpu->refpt->at(jpu),offset/tpu->tnpus->at(iIT));

    hname = Form("p_resVsrefpt_%s",detectorAbbreviation.Data());
    histograms[hname]->Fill(tpu->refpt->at(jpu),resp);
    hname = Form("np_resVsrefpt_%s",detectorAbbreviation.Data());
    histograms[hname]->Fill(tpu->refpt->at(jpu),respNopu);

    hname = Form("p_resVsrefpt_%s_npv%i_%i",detectorAbbreviation.Data(),inpv_low,inpv_high);
    histograms[hname]->Fill(tpu->refpt->at(jpu),resp);
    hname = Form("p_offresVsrefpt_%s_npv%i_%i",detectorAbbreviation.Data(),inpv_low,inpv_high);
    histograms[hname]->Fill(tpu->refpt->at(jpu),offset);
    hname = Form("p_offresOrefptVsrefpt_%s_npv%i_%i",detectorAbbreviation.Data(),inpv_low,inpv_high);
    histograms[hname]->Fill(tpu->refpt->at(jpu),offsetOrefpt);
    hname = Form("p_resVsrefpt_%s_rho%i_%i",detectorAbbreviation.Data(),irho_low,irho_high);
    histograms[hname]->Fill(tpu->refpt->at(jpu),resp);
    hname = Form("p_resVsrefpt_%s_tnpu%i_%i",detectorAbbreviation.Data(),itnpu_low,itnpu_high);
    histograms[hname]->Fill(tpu->refpt->at(jpu),resp);
    hname = Form("p_resVsrefpt_%s_npu%i_%i",detectorAbbreviation.Data(),inpu_low,inpu_high);
    histograms[hname]->Fill(tpu->refpt->at(jpu),resp);
    hname = Form("p_resVsnpu_%s_pt%.1f_%.1f",detectorAbbreviation.Data(),
                 vpt[JetInfo::getBinIndex(tpu->refpt->at(jpu),vpt,NPtBins)],vpt[JetInfo::getBinIndex(tpu->refpt->at(jpu),vpt,NPtBins)+1]);
    if(tpu->refpt->at(jpu)>10.0) {
       histograms[hname]->Fill(tpu->npus->at(iIT),resp);
    }
    hname = Form("p_offresVsrefpt_%s_rho%i_%i",detectorAbbreviation.Data(),irho_low,irho_high);
    histograms[hname]->Fill(tpu->refpt->at(jpu),offset);
    hname = Form("p_offresOrefptVsrefpt_%s_rho%i_%i",detectorAbbreviation.Data(),irho_low,irho_high);
    histograms[hname]->Fill(tpu->refpt->at(jpu),offsetOrefpt);
    hname = Form("p_offresVsrefpt_%s_tnpu%i_%i",detectorAbbreviation.Data(),itnpu_low,itnpu_high);
    histograms[hname]->Fill(tpu->refpt->at(jpu),offset);
    hname = Form("p_offresVsrefpt_%s_npu%i_%i",detectorAbbreviation.Data(),inpu_low,inpu_high);
    histograms[hname]->Fill(tpu->refpt->at(jpu),offset);
    hname = Form("p_offresVsrefpt_%s_npv%i_%i",detectorAbbreviation.Data(),0,((NBinsNpvRhoNpu-1)*npvRhoNpuBinWidth)+npvRhoNpuBinWidth-1);
    histograms[hname]->Fill(tpu->refpt->at(jpu),offset);
    hname = Form("p_offresVsrefpt_%s_rho%i_%i",detectorAbbreviation.Data(),0,((NBinsNpvRhoNpu-1)*npvRhoNpuBinWidth)+npvRhoNpuBinWidth-1);
    histograms[hname]->Fill(tpu->refpt->at(jpu),offset);
    hname = Form("p_offresVsrefpt_%s_tnpu%i_%i",detectorAbbreviation.Data(),0,((NBinsNpvRhoNpu-1)*npvRhoNpuBinWidth)+npvRhoNpuBinWidth-1);
    histograms[hname]->Fill(tpu->refpt->at(jpu),offset);
    hname = Form("p_offresVsrefpt_%s_npu%i_%i",detectorAbbreviation.Data(),0,((NBinsNpvRhoNpu-1)*npvRhoNpuBinWidth)+npvRhoNpuBinWidth-1);
    histograms[hname]->Fill(tpu->refpt->at(jpu),offset);

    for (unsigned int ipdgid=0; ipdgid<pdgid_indecies.size(); ipdgid++) {
      hname = Form("p_offresVsrefpt_%s_pdgid_%s",detectorAbbreviation.Data(),pdgidstr[ipdgid].Data());
      histograms[hname]->Fill(tpu->refpt->at(jpu),offset);
    }

    hname = Form("p_resnopuVsrefpt_%s_npv%i_%i",detectorAbbreviation.Data(),inpv_low,inpv_high);
    histograms[hname]->Fill(tpu->refpt->at(jpu),respTonopu);
    hname = Form("p_resnopuVsrefpt_%s_rho%i_%i",detectorAbbreviation.Data(),irho_low,irho_high);
    histograms[hname]->Fill(tpu->refpt->at(jpu),respTonopu);
    hname = Form("p_nopuresVsrefpt_%s_npv%i_%i",detectorAbbreviation.Data(),inpv_low,inpv_high);
    histograms[hname]->Fill(tpu->refpt->at(jpu),respNopu);
    hname = Form("p_nopuresVsrefpt_%s_rho%i_%i",detectorAbbreviation.Data(),irho_low,irho_high);
    histograms[hname]->Fill(tpu->refpt->at(jpu),respNopu);
    hname = Form("p_nopuresVsrefpt_%s_tnpu%i_%i",detectorAbbreviation.Data(),itnpu_low,itnpu_high);
    histograms[hname]->Fill(tpu->refpt->at(jpu),respNopu);
    hname = Form("p_nopuresVsrefpt_%s_npu%i_%i",detectorAbbreviation.Data(),inpu_low,inpu_high);
    histograms[hname]->Fill(tpu->refpt->at(jpu),respNopu);
    hname = Form("p_offAfterOoffBeforeVsrefpt_%s_npv%i_%i",detectorAbbreviation.Data(),inpv_low,inpv_high);
    histograms[hname]->Fill(tpu->refpt->at(jpu),offset/offset_raw);
    hname = Form("p_offAfterOoffBeforeVsrefpt_%s_rho%i_%i",detectorAbbreviation.Data(),irho_low,irho_high);
    histograms[hname]->Fill(tpu->refpt->at(jpu),offset/offset_raw);

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
        histograms[hname]->Fill(avg_offset_det[det],tpu->npv);
        hname = Form("p_rhoVsOff_%s",detectorAbbreviation.Data());
        histograms[hname]->Fill(avg_offset_det[det],tpu->rho_hlt);
     }
  }
  if(!reduceHistograms) {
     histograms["p_npvVsoff"]      ->Fill(avg_offset,tpu->npv);
     histograms["p_rhoVsoff"]      ->Fill(avg_offset,tpu->rho_hlt);
     histograms["p_rhoVsRho"]      ->Fill(tpu->rho_hlt,tpu->rho_hlt);
     histograms["p_npvVsNpv"]      ->Fill(tpu->npv,tpu->npv);
     histograms["p_tnpuVsTnpu"]    ->Fill(tpu->tnpus->at(iIT),tpu->tnpus->at(iIT));
     histograms["p_npuVsNpu"]    ->Fill(tpu->npus->at(iIT),tpu->npus->at(iIT));
     histograms["p_matchedjet_off"]->Fill(avg_offset,jetMap.size());
  }

  //=========================================================
  //              FILLING OF HISTOS ENDS HERE
  //=========================================================

  return true;
}

//______________________________________________________________________________
void MatchEventsAndJets::WriteOutput(){
   cout << endl << "Writing file " << fout->GetName() << " ... " << flush;
   fout->cd();
   fout->Write();
   fout->Close();
   cout << "DONE" << endl;
}

//______________________________________________________________________________
void MatchEventsAndJets::Report() {
   cout << "Event-matching report" << endl
        << "\t Number of events skipped because no PU sample had more than 1 NPV: " << noPUNpvGTOneEventCounter << endl
        << "\t Total number of unique events in first  sample: " << tpu->fChain->GetEntries() << endl
        << "\t Total number of unique events in second sample: " << tnopu->fChain->GetEntries() << endl
        << "\t Number of matched events we ran over " << nevs << endl;
}

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

// ------------------------------------------------------------------
int main(int argc,char**argv)
{
   CommandLine cl;
   if (!cl.parse(argc,argv)) return 0;
   TString samplePU          = cl.getValue<TString> ("samplePU", "QCDHLT/QCD_Pt-15to3000_TuneCUETP8M1_Flat_13TeV_pythia8/crab_QCDHLTJEC/161129_150822/0000/JRA_1.root");
   TString sampleNoPU        = cl.getValue<TString> ("sampleNoPU", "QCDHLT_NoPU/QCD_Pt-15to3000_TuneCUETP8M1_Flat_13TeV_pythia8/crab_QCDHLTJECNoPU/161129_041315/0000/JRA_1.root");
   TString basepath          = cl.getValue<TString> ("basepath", "/afs/cern.ch/user/f/fengwang/eos/cms/store/user/fengwang/");
   TString algo1             = cl.getValue<TString> ("algo1",                            "ak4pfHLT");
   TString algo2             = cl.getValue<TString> ("algo2",                            "ak4pfHLT");
   TString order1            = cl.getValue<TString> ("order1",                                  "0");
   TString order2            = cl.getValue<TString> ("order2",                                  "0");
   bool    iftest            = cl.getValue<bool>    ("iftest",                                false);
   int     maxEvts           = cl.getValue<int>     ("maxEvts",                               40000);
   bool    ApplyJEC          = cl.getValue<bool>    ("ApplyJEC",                              false);
   string  JECpar            = cl.getValue<string>  ("JECpar",            "parameters_ak4pfHLT.txt");
   bool    runDep            = cl.getValue<bool>    ("runDep",                                 true);
   TString outputPath        = cl.getValue<TString> ("outputPath",                             "./");
   TString treeName          = cl.getValue<TString> ("treeName",                                "t");
   int     npvRhoNpuBinWidth = cl.getValue<int>     ("npvRhoNpuBinWidth",                         5);
   int     NBinsNpvRhoNpu    = cl.getValue<int>     ("NBinsNpvRhoNpu",                            6);
   vector<int> vptBins       = cl.getVector<int>    ("vptBins",       "14:::18:::20:::24:::28:::30");  
   bool    reduceHistograms  = cl.getValue<bool>    ("reduceHistograms",                      false);
   bool    verbose           = cl.getValue<bool>    ("verbose",                               false);

   if (!cl.check()) return 0;
   cl.print();

   TBenchmark* m_benchmark = new TBenchmark();
   m_benchmark->Reset();
   m_benchmark->Start("event");

   gEnv->SetValue("TFile.AsyncPrefetching", 1);

   if(outputPath.IsNull()) outputPath = string (gSystem->pwd())+"/";
   if(!outputPath.EndsWith("/")) outputPath+="/";
   if(!basepath.EndsWith("/")) basepath+="/";

   MatchEventsAndJets* mej = new MatchEventsAndJets(algo1,algo2,iftest,runDep);
   mej->SetMaxEvts(maxEvts);
   mej->OpenInputFiles(basepath+samplePU,basepath+sampleNoPU);
   mej->GetNtuples(treeName);
   mej->MakeMatchedEventsMaps(treeName);
   mej->OpenOutputFile(outputPath,order1,order2);
   if (ApplyJEC) {
      cout << "jet_synchtest_x::Setting the JEC parameter file to " << JECpar << " ... ";
      mej->SetJEC(JECpar);
      cout << "DONE" << endl;
   }
   mej->SetNpvRhoNpuValues(NBinsNpvRhoNpu,npvRhoNpuBinWidth);
   mej->SetVptBins(vptBins);
   mej->DeclareHistograms(reduceHistograms);
   mej->LoopOverEvents(verbose,reduceHistograms);
   mej->WriteOutput();
   mej->Report();

   m_benchmark->Stop("event");
   cout << "jet_synchtest_x" << endl
        << "\tCPU time = " << m_benchmark->GetCpuTime("event") << " s" << endl
        << "\tReal time = " << m_benchmark->GetRealTime("event") << " s" << endl;
   delete m_benchmark;
}

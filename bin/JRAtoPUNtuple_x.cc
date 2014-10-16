// ROOT Libraries
#include "TROOT.h"
#include "TSystem.h"
#include "TChain.h"
#include "TFile.h"
#include "TChain.h"
#include "TString.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TProfile3D.h"

// C++ Libraries
#include <iostream>
#include <vector>
#include <map>

// JetMETAnalysis Libraries
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/JRANtuple.h"
#include "JetMETAnalysis/JetUtilities/interface/PUNtuple.hh"
#include "JetMETAnalysis/JetUtilities/interface/JetInfo.hh"
#include "JetMETAnalysis/JetUtilities/interface/EvtID.hh"

// CMSSW Libraries
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"

using namespace std;

typedef map<double, pair<Int_t, Int_t> > ITJ;


////////////////////////////////////////////////////////////////////////////////
// declare class
////////////////////////////////////////////////////////////////////////////////

class MatchEventsAndJets {
   public:
      
      MatchEventsAndJets();
      MatchEventsAndJets(TString algo1_, TString algo2_, bool iftest_, bool runDep_);
      ~MatchEventsAndJets() {;}
      
      void OpenInputFiles(TString filenamePU, TString filenameNoPU);
      void MakeMatchedEventsMaps(TString treeName);
      void fillMap(map<evtid, pair<Long64_t, Long64_t>, evtid> & mapTree,
                   TFile* f, TString algo, TString treeName);
      void GetNtuples(TString treeName = "t");
      void OpenOutputFile(TString outputPath = "./");
      void SetJEC(TString JECPar = "parameters_ak5pf.txt");
      void SetNpvRhoNpuValues(int NBins, int Width) {NBinsNpvRhoNpu=NBins; npvRhoNpuBinWidth=Width;}
      void CreatePUNtuple();
      void DeclareHistograms();
      void LoopOverEvents();
      void FillJetMap();
      bool FillHistograms();
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
      
      //Ntuples
      JRANtuple* tpu;
      JRANtuple* tnopu;
      PUNtuple* puntuple;
      
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
      
      //Debug
      bool iftest;
      
      //Settings
      bool runDep;
      
      //TTree
      TTree* putree;
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
   putree =0;
   puntuple =0;
}

//______________________________________________________________________________
MatchEventsAndJets::MatchEventsAndJets(TString algo1_, TString algo2_, bool iftest_, bool runDep_)
                              : algo1(algo1_), algo2(algo2_), iftest(iftest_), runDep(runDep_) {
   JetCorrector = 0;
   nevs = 0;
   NBinsNpvRhoNpu = 6;
   npvRhoNpuBinWidth = 5;
   putree =0;
   puntuple =0;
}

//______________________________________________________________________________
void MatchEventsAndJets::OpenInputFiles(TString filenamePU, TString filenameNoPU) {
   fpu = TFile::Open(filenamePU,"READ");
   fnopu = TFile::Open(filenameNoPU,"READ");
}

//______________________________________________________________________________
void MatchEventsAndJets::MakeMatchedEventsMaps(TString treeName) {
   fillMap(mapTreePU, fpu, algo1, treeName);
   fillMap(mapTreeNoPU, fnopu, algo2, treeName);
}

//______________________________________________________________________________
void MatchEventsAndJets::fillMap(map<evtid, pair<Long64_t, Long64_t>, evtid> & mapTree,
             TFile* f, TString algo, TString treeName) {

   // Get the tree
   f->cd(algo);
   TChain * chain = (TChain*) f->Get(algo+"/"+treeName);

   // Load only what's needed this WAY FASTER.
   chain->SetBranchStatus("*",0);
   chain->SetBranchStatus("run",1);
   //chain->SetBranchStatus("lumi",1);
   chain->SetBranchStatus("evt",1);
   chain->SetBranchStatus("refpt",1);

   // Book 42X tree
   TBranch *b_run, /**b_lumi,*/ *b_evt, *b_refpt;;
   Long64_t run; 
   Int_t    lumi = 0;
   Long64_t evt;
   Float_t refpt[92];
  
   chain->SetBranchAddress("run",&run,&b_run);
   //chain->SetBranchAddress("lumi",&lumi,&b_lumi);
   chain->SetBranchAddress("evt",&evt,&b_evt);
   chain->SetBranchAddress("refpt",&refpt,&b_refpt);

   cout << "Filling map with event signatures from: "<<endl;
   cout << "\tfile "<<f->GetName()<< endl;
   cout << "\talgo "<<algo<< endl;
   cout << "\ttreename "<<treeName<< endl;
   cout<<"\tprogress:";

//   Long64_t nentries = chain->GetEntriesFast();
   Long64_t nentries = 1000000;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = chain->LoadTree(jentry);
      if (ientry < 0) break;
      b_run->GetEntry(ientry);
      //b_lumi->GetEntry(ientry);
      b_evt->GetEntry(ientry);

      if(mapTree.find(evtid(run, lumi, evt, refpt[0], runDep))!=mapTree.end()) {
         cout << "\tWARNING::This evtid already exists in the map." << endl;
      }
      mapTree[evtid(run, lumi, evt, refpt[0], runDep)] = std::make_pair(jentry, ientry);

      if (jentry%100000==0) cout << "." << flush;
   }
   cout << endl;
   cout << "\tRead " << mapTree.size() << " unique signatures" << endl;
   cout << endl;
  
   chain->SetBranchStatus("*",1);
}

//______________________________________________________________________________
void MatchEventsAndJets::GetNtuples(TString treeName) {
   fpu->cd(algo1);
   tpu   = new JRANtuple((TTree*) fpu->Get(algo1+"/"+treeName));

   fnopu->cd(algo2);
   tnopu = new JRANtuple((TTree*) fnopu->Get(algo2+"/"+treeName));
}

//______________________________________________________________________________
void MatchEventsAndJets::OpenOutputFile(TString outputPath) {
   TString outputFilename = "output_"+algo1+"_"+algo2+".root";
   if (algo1.EqualTo(algo2)) 
      outputFilename = "output_"+algo1+".root";
   outputFilename = outputPath+outputFilename;
   fout = new TFile(outputFilename,"RECREATE");
}

//______________________________________________________________________________
void MatchEventsAndJets::DeclareHistograms() {
   cout << "Declaration of histos!: " <<endl;
}
//______________________________________________________________________________
void MatchEventsAndJets::CreatePUNtuple(){
   putree = new TTree("puntuple", "puntuple");
   puntuple = new PUNtuple();
   putree->Branch("puntuple","PUNtuple", &puntuple);
}
//______________________________________________________________________________
void MatchEventsAndJets::LoopOverEvents() {


   for (IT::const_iterator it = mapTreePU.begin(); it != mapTreePU.end(); ++it) {

      if (iftest && nevs >= 40000) return;
      
      if (nevs%10000==0) cout << "\t"<<nevs << endl;

      // if this entry does not exist on the second ntuple just skip this event
      if (mapTreeNoPU.find(it->first) == mapTreeNoPU.end()) {
         cout << "\tWARNING::mapTreeNoPU.find(it->first) == mapTreeNoPU.end() failed" << endl
              << "\tit->first.run_ == " << it->first.run_ << endl
              << "\tit->first.ls_ == " << it->first.ls_ << endl
              << "\tit->first.evt_ == " << it->first.evt_ << endl
              << "\tit->first.refpt0_ == " << it->first.refpt0_ << endl;
         continue;
      }

      // Load the entries at the proper place.
      tpu->GetEntry(mapTreePU[it->first].second);
      tnopu->GetEntry(mapTreeNoPU[it->first].second);
      cout<<"SFSG"<<endl;
      if (tpu->npv ==0 || tnopu->npv == 0) continue;
      // HERE correct the jets in tpu if requested

      // Create the mapping of matched jets.
      // key is PU, value is for NoPU
      FillJetMap();

      //LLenar toda la informacion PUNTuple
//      puntuple->Reset();

      puntuple->run = 1;
      putree->Fill();

      if(FillHistograms()) {
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
         double dR  = pow(tpu->jteta[j1] - tnopu->jteta[j2],2);
         dR += pow(tpu->jtphi[j1] - tnopu->jtphi[j2],2);
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
      if (auxMap.begin()->first < 0.25 && fabs(tpu->refpt[j1] - tnopu->refpt[j2])<1)
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
bool MatchEventsAndJets::FillHistograms() {
  //=========================================================
  //              FILLING OF HISTOS START HERE
  //=========================================================

  if (tnopu->npv!=1) {
    cout << "\tWARNING::The no PU sample has more than 1 PV." << endl
    << "\tSkipping this event." << endl;
    return false;
  }

  /*
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
  */
  TString hname = "";
    
  vector<double> tpu_jtpt_raw;
  double avg_jtpt_all       = 0;
  double avg_jtpt_matched   = 0;
  double avg_jtpt_unmatched = 0;

    if (tpu->nref>0)               avg_jtpt_all        /= (double) tpu->nref;     else avg_jtpt_all = 0;
  if (jetMap.size()>0)           avg_jtpt_matched    /= (double) jetMap.size(); else avg_jtpt_matched = 0;
  if (tpu->nref-jetMap.size()>0) avg_jtpt_unmatched  /= (double) (tpu->nref - jetMap.size()); else avg_jtpt_unmatched = 0;

  double avg_offset = 0;
  //double avg_offset_det[NDetectorNames] = {0,0,0,0};
  //double njet_det[NDetectorNames] = {0,0,0,0};

  // MATCHING HISTOS. 
  // Loop over matched jets
  for (map<Int_t, Int_t>::const_iterator itj = jetMap.begin(); itj != jetMap.end(); itj++) {

    int jpu = itj->first;
    //int jnopu = itj->second;
        
    int idet = JetInfo::getDetIndex(tpu->jteta[jpu]);
    TString detectorAbbreviation = JetInfo::get_detector_abbreviation(detector_names[idet]);
    detectorAbbreviation.ToLower();
    vector<int> pdgid_indecies = JetInfo::getPDGIDIndecies(tpu->refpdgid[jpu]);

/*
    double eta_avg = 0.5*(tpu->jteta[jpu]+tnopu->jteta[jnopu]);
    double offset     = tpu->jtpt[jpu] - tnopu->jtpt[jnopu];
    double offset_raw = -1.0;
    if(tpu_jtpt_raw.size()>0)
      offset_raw = tpu_jtpt_raw[jpu] - tnopu->jtpt[jnopu];

    int diff_pdgid       = tpu->refpdgid[jpu] - tnopu->refpdgid[jnopu];
    double areaDiff      = tpu->jtarea[jpu] - tnopu->jtarea[jnopu];
    double resp          = tpu->jtpt[jpu] / tpu->refpt[jpu];   // response relative to reference jet
    double respTonopu    = tpu->jtpt[jpu] / tnopu->jtpt[jnopu];// response relative to no pu jet
    double respNopu       = tnopu->jtpt[jnopu] / tnopu->refpt[jnopu]; // response no pu jet to reference jet
    double PUEff      = 0.020*(tpu->sumEOOT())+0.975*(tpu->npus->at(iIT))+0.005*(tpu->sumLOOT()); // effective pu
    double GenSumPtOA    = (0.020*(tpu->sumpt_lowpt->at(0))+0.975*(tpu->sumpt_lowpt->at(1))+0.005*(tpu->sumpt_lowpt->at(2)))/tpu->jtarea[jpu];
    //chf,nhf,nef,cef,hfhf,hfef
    double offset_PFcat[NPFcat] = {tpu->jtpt[jpu]*tpu->jtchf[jpu] - tnopu->jtpt[jnopu]*tnopu->jtchf[jnopu],
                                   tpu->jtpt[jpu]*tpu->jtnhf[jpu] - tnopu->jtpt[jnopu]*tnopu->jtnhf[jnopu],
                                   tpu->jtpt[jpu]*tpu->jtnef[jpu] - tnopu->jtpt[jnopu]*tnopu->jtnef[jnopu],
                                   tpu->jtpt[jpu]*tpu->jtcef[jpu] - tnopu->jtpt[jnopu]*tnopu->jtcef[jnopu],
                                   tpu->jtpt[jpu]*tpu->jthfhf[jpu] - tnopu->jtpt[jnopu]*tnopu->jthfhf[jnopu],
                                   tpu->jtpt[jpu]*tpu->jthfef[jpu] - tnopu->jtpt[jnopu]*tnopu->jthfef[jnopu]};
    double offsetOA      = offset / tpu->jtarea[jpu];
    double offsetOrefpt  = offset / tpu->refpt[jpu];
    */
  } // for matched jets 
  
  avg_offset /= jetMap.size();
  //=========================================================
  //              FILLING OF HISTOS ENDS HERE
  //=========================================================
  
  return true;
}

//______________________________________________________________________________
void MatchEventsAndJets::WriteOutput(){
   cout << " Writing file " << fout->GetName() << " ... " << flush;
   fout->cd();
   fout->Write();
   fout->Close();
   cout << "DONE" << endl;
}

//______________________________________________________________________________
void MatchEventsAndJets::Report() {
   cout << "Event-matching report" << endl
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

   /*CommandLine cl;
   if (!cl.parse(argc,argv)) return 0;
   TString samplePU          = cl.getValue<TString> ("samplePU");
   TString sampleNoPU        = cl.getValue<TString> ("sampleNoPU");
   TString basepath          = cl.getValue<TString> ("basepath", "/fdata/hepx/store/user/aperloff/");
   TString algo1             = cl.getValue<TString> ("algo1",                               "ak5pf");
   TString algo2             = cl.getValue<TString> ("algo2",                               "ak5pf");
   bool    iftest            = cl.getValue<bool>    ("iftest",                                false);
   bool    ApplyJEC          = cl.getValue<bool>    ("ApplyJEC",                              false);
   string  JECpar            = cl.getValue<string>  ("JECpar",               "parameters_ak5pf.txt");
   bool    runDep            = cl.getValue<bool>    ("runDep",                                 true);
   TString outputPath        = cl.getValue<TString> ("outputPath",                             "./");
   TString treeName          = cl.getValue<TString> ("treeName",                                "t");
   int     npvRhoNpuBinWidth = cl.getValue<int>     ("npvRhoNpuBinWidth",                         5);
   int     NBinsNpvRhoNpu    = cl.getValue<int>     ("NBinsNpvRhoNpu",                            6);
  
   if (!cl.check()) return 0;
   cl.print();

   if(outputPath.IsNull()) outputPath = string (gSystem->pwd())+"/";
   if(!outputPath.EndsWith("/")) outputPath+="/";
   if(!basepath.EndsWith("/")) basepath+="/";*/

   TString basepath = "/fdata/hepx/store/user/delgado_andrea/";
   TString sampleNoPU = "JRA_outfiles_53X_20140129_NoPileup_pbs/JRA/JRA.root";
   TString samplePU =  "JRA_outfiles_53X_20140129_pbs/JRA/JRA.root";
   TString algo1 = "ak5pf";
   TString algo2 = "ak5pf";
   bool iftest = false;
   bool runDep = true;
   TString treeName = "t";
   TString outputPath = "./";
   //bool ApplyJEC = false;
   TString JECpar = "parameter.txt";
   //int npvRhoNpuBinWidth = 5;
   //int NBinsNpvRhoNpu = 6;

   MatchEventsAndJets* mej = new MatchEventsAndJets(algo1,algo2,iftest,runDep);
   mej->OpenInputFiles(basepath+samplePU,basepath+sampleNoPU);
   mej->MakeMatchedEventsMaps(treeName);
   mej->GetNtuples(treeName);
   mej->OpenOutputFile(outputPath);
//   mej->SetNpvRhoNpuValues(NBinsNpvRhoNpu,npvRhoNpuBinWidth);
   mej->CreatePUNtuple();
//   mej->DeclareHistograms();
   mej->LoopOverEvents();
   mej->WriteOutput();
   mej->Report();
   
}


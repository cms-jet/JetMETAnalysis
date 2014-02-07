#include <map>
#include <iostream>
#include <vector>

#include "TFile.h"
#include "TChain.h"
#include "JetMETAnalysis/JetUtilities/interface/JECTree.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TProfile3D.h"
#include "TSystem.h"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"

#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"

using namespace std;

class evtid {
private:
   int run_, ls_, evt_;
public :
   evtid() : run_(0), ls_(0), evt_(0) {}
   evtid(int run, int ls, int evt) : run_(run), ls_(ls), evt_(evt) {}
   bool operator()(evtid const& a, evtid const& b) {
      if (a.run_ < b.run_) return true;
      if (a.ls_ < b.ls_) return true;
      if (a.evt_ < b.evt_) return true;
      else return false;
   }
};

// To make notation clearer
typedef map<evtid, pair<Long64_t, Long64_t> > IT;

const int NPtBins    = 30;//37
const double vpt[NPtBins + 1] = {10,10.5,11,11.5,12,12.5,13,13.5,14,15,17,20,23,27,30,35,40,45,57,72,90,120,150,200,300,400,550,750,1000,1500,2000};//,2500,3000,3500,4000};//,4500,5000,10000};

const int NETA = 82;
const double veta[NETA+1] = {-5.191,-4.889,-4.716,-4.538,-4.363,-4.191,-4.013,-3.839,-3.664,-3.489,
                             -3.314, -3.139,-2.964,-2.853,-2.65, -2.5,  -2.322,-2.172,-2.043,-1.93,
                             -1.83,  -1.74, -1.653,-1.566,-1.479,-1.392,-1.305,-1.218,-1.131,-1.044,
                             -0.957, -0.879,-0.783,-0.696,-0.609,-0.522,-0.435,-0.348,-0.261,-0.174,
                             -0.087, 0,     0.087, 0.174, 0.261, 0.348, 0.435 ,0.522, 0.609, 0.696,
                             0.783,  0.879, 0.957, 1.044, 1.131, 1.218, 1.305 ,1.392, 1.479, 1.566,
                             1.653,  1.74,  1.83,  1.93,  2.043, 2.172, 2.322 ,2.5,   2.65,  2.853,
                             2.964,  3.139, 3.314, 3.489, 3.664, 3.839, 4.013 ,4.191, 4.363, 4.538,
                             4.716,4.889,5.191};
const int NRHO = 50;
const double vrho[NRHO+1] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50};

TString detstr[4] ={"bb","ei","eo","ff"};
TString PFstr[6] = {"chf","nhf","nef","cef","hfhf","hfef"};

// ------------------------------------------------------------------
int getNpvIndex(int npv){
   if (npv<0)
      return 0;
   if (npv>29)
      return 5;

   return int (npv / 5.0);
}//getNpvIndex


// ------------------------------------------------------------------
int getDetIndex(double eta){
   eta = fabs(eta);

   if (eta<1.3)
      return 0;
   else if (eta<2.5)
      return 1;
   else if (eta<3)
      return 2;

   return 3;

}//getDetIndex



// ------------------------------------------------------------------
void fillMap(map<evtid, pair<Long64_t, Long64_t>, evtid> & mapTree,
             TString filename, TString algo, TString treeName){

   // Open the file and get the tree
   TFile * f = TFile::Open(filename);
   f->cd(algo);
   TChain * chain = (TChain*) f->Get(algo+"/"+treeName);

   // Load only what's needed this WAY FASTER.
   chain->SetBranchStatus("*",0);
   chain->SetBranchStatus("run",1);
   chain->SetBranchStatus("evt",1);

   // Book 42X tree
   TBranch *b_run, *b_evt; // *b_lbn
   Long64_t run; 
   Int_t    lbn = 0;
   Long64_t evt; 
  
   chain->SetBranchAddress("run",&run,&b_run);
   //chain->SetBranchAddress("lbn",&lbn,&b_lbn);
   chain->SetBranchAddress("evt",&evt,&b_evt);

   cout << "Filling map with event signatures from: "<<endl;
   cout << "\tfile "<<filename<< endl;
   cout << "\talgo "<<algo<< endl;
   cout << "\ttreename "<<treeName<< endl;
   cout<<"\tprogress:";

   Long64_t nentries = chain->GetEntriesFast();
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = chain->LoadTree(jentry);
      if (ientry < 0) break;
      b_run->GetEntry(ientry);
      //b_lbn->GetEntry(ientry);
      b_evt->GetEntry(ientry);

      mapTree[evtid(run, lbn, evt)] = std::make_pair(jentry, ientry);

      if (jentry%100000==0) cout << "." << flush;
   }
   cout << endl;
   cout << "\tRead " << mapTree.size() << " unique signatures" << endl;
   cout << endl;
  
   f->Close();
  
}//fillMap

// ------------------------------------------------------------------
void fillJetMap(map<Int_t, Int_t> & jetMap, JECTree * t1, JECTree * t2){

   typedef map<double, pair<Int_t, Int_t> > ITJ;

   // Create an aux map with the dR of all the possible 
   // combinations of jets in both events
   map<double, pair<Int_t, Int_t> >  auxMap;
   for (int j1=0; j1 < t1->nref; j1++){
      for (int j2=0; j2 < t2->nref; j2++){
         double dR  = pow(t1->jteta[j1] - t2->jteta[j2],2);
         dR += pow(t1->jtphi[j1] - t2->jtphi[j2],2);
         dR = sqrt(dR);
         auxMap[dR] = std::make_pair(j1, j2);
         //if (t1->nref != t2->nref)
         //cout<< j1 <<" " <<j2<<" "<<dR<<endl;
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
      if (auxMap.begin()->first < 0.25 && fabs(t1->refpt[j1]-t2->refpt[j2])<1)
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

}//fillJetMap

// ------------------------------------------------------------------
void SynchTest(TString calgo1="ak5pf",TString calgo2="ak5pf",int iftest=0, int ApplyJEC = 0, TString JECPar="parameters_ak5pf.txt"){
  
   //TString basepath = "/uscms_data/d2/aperloff/JRA_outfiles_53X_20120911/JRA/";
   TString basepath ="/fdata/hepx/store/user/aperloff/";

   //TString sample1 =  basepath+"JRA_outfiles_53X_20131228_pbs/JRA/JRA.root"; //PF // with latest L1's
   //TString sample1 =  basepath+"JRA_outfiles_53X_20140122_pbs/JRA/JRA.root"; //PFchs // with latest L1's
   TString sample1 =  basepath+"Delphes_QCDjets_20PU_1M_v2/DelphesJRA.root"; //Delphes // with latest L1's
   TString algo1(calgo1);

   //TString sample2 =  basepath+"JRA_outfiles_53X_20131228_NoPileup_pbs/JRA/JRA.root"; //PF
   //TString sample2 =  basepath+"JRA_outfiles_53X_20140122_NoPileup_pbs/JRA/JRA.root"; //PFchs
   TString sample2 =  basepath+"Delphes_QCDjets_20PU_1M_v2/DelphesJRA.root"; //Delphes
   //TString algo2 = algo1;
   TString algo2(calgo2);

   TString treeName = "t";

   // Make the first map and fill it
   map<evtid, pair<Long64_t, Long64_t>, evtid> mapTreePU;
   fillMap(mapTreePU, sample1 ,algo1,treeName);

   // Make the second map and fill it. Using same old tree for comparison. Needs to be changed
   map<evtid, pair<Long64_t, Long64_t>, evtid> mapTreeNoPU;
   fillMap(mapTreeNoPU, sample2 ,algo2,treeName);

   TFile * fpu = TFile::Open(sample1);
   fpu->cd(algo1);
   JECTree * tpu   = new JECTree((TTree*) fpu->Get(algo1+"/"+treeName));

   TFile * fnopu = TFile::Open(sample2);
   fnopu->cd(algo2);
   JECTree * tnopu = new JECTree((TTree*) fnopu->Get(algo2+"/"+treeName));

   //A map holding the equivalance of jets in two given events
   map<Int_t,Int_t> mapJet;

   typedef map<Int_t, Int_t> ITJ;
 
   TString outputFilename = "output_"+algo1+"_"+algo2+".root";
   if (algo1.EqualTo(algo2)) 
      outputFilename = "output_"+algo1+".root";
   TFile * fout = new TFile(outputFilename,"RECREATE");

//	Get the libraries ready if to apply JEC

   JetCorrectorParameters *L1JetPar;
   vector<JetCorrectorParameters> vPar;
   FactorizedJetCorrector *JetCorrector;
   if (ApplyJEC != 0)
   {
      L1JetPar  = new JetCorrectorParameters(JECPar.Data());
      vPar.push_back(*L1JetPar);
      JetCorrector = new FactorizedJetCorrector(vPar);
   
   }


   //=========================================================
   //              DECLARATION OF HISTOS START HERE
   //=========================================================

   // HISTOS OF GENERAL QUANTITIES.General, hence the g_ prefix
   TH2D * g_nj    = new TH2D("g_nj","g_nj",30,0,30,30,0,30);
   TH2D * g_npv   = new TH2D("g_npv","g_npv",50,0,50,50,0,50);
   TH2D * g_rho   = new TH2D("g_rho","g_rho",50,0,50,50,0,50);
   TH2D * g_pthat = new TH2D("g_pthat","g_pthat",300,0,3000,300,0,300);		//ZQ
   TProfile * g_deltaNpv = new TProfile("g_deltaNpv","g_deltaNpv",50,0,50);

    
   // HISTOS OF EVENT or JET MATCHING performance, hence the m_ prefix

   // KEEP - show the  
   TH1D * m_refpt_diff = new TH1D("m_refpt_diff","refpt_diff;p_{T, j1}- p_{T, j2}; Number of events",300,-150,150);
   TH1D * m_refpdgid_diff = new TH1D("m_refpdgid_diff","m_refpdgid_diff;pdgid_{j1}- pdgid_{j2}; Number of events",100,-50,50);
   TProfile * m_deltaPthat = new TProfile("m_deltaPthat","m_deltaPthat;pthat_{pu}(GeV);pthat_{pu}-pthat_{nopu}(GeV)",300,0,3000);		//ZQ

   // KEEP - to show the excees of jets at low-pt for the sample with pu. NICE!
   TH1D * m_njet_pt_pu = new TH1D("m_njet_pt_pu","m_njet_pt_pu;jet pt;#jets;",NPtBins, vpt); // number of jets
   TH1D * m_njet_pt_nopu = new TH1D("m_njet_pt_nopu","m_njet_pt-nopu;jet pt;#jets;",NPtBins, vpt); // number of jets
  
   // KEEP  -  fraction of matched jets in all, barrel, endcap and forward regions
   TProfile * m_frac_nj_pt_b_match_pu   = new TProfile("m_frac_nj_pt_b_match_pu","m_frac_nj_pt_b_match_pu;pt_{jet};fraction of matched jets;", NPtBins, vpt);
   TProfile * m_frac_nj_pt_b_match_nopu = new TProfile("m_frac_nj_pt_b_match_nopu","m_frac_nj_pt_b_match_nopu;pt_{jet};fraction of matched jets;", NPtBins,vpt);
   TProfile * m_frac_nj_pt_e_match_pu   = new TProfile("m_frac_nj_pt_e_match_pu","m_frac_nj_pt_e_match_pu;pt_{jet};fraction of matched jets;", NPtBins, vpt);
   TProfile * m_frac_nj_pt_e_match_nopu = new TProfile("m_frac_nj_pt_e_match_nopu","m_frac_nj_pt_e_match_nopu;pt_{jet};fraction of matched jets;", NPtBins,vpt);
   TProfile * m_frac_nj_pt_f_match_pu   = new TProfile("m_frac_nj_pt_f_match_pu","m_frac_nj_pt_f_match_pu;pt_{jet};fraction of matched jets;", NPtBins, vpt);
   TProfile * m_frac_nj_pt_f_match_nopu = new TProfile("m_frac_nj_pt_f_match_nopu","m_frac_nj_pt_f_match_nopu;pt_{jet};fraction of matched jets;", NPtBins,vpt);

   //-  fraction of matched jets in barrel regions, with different NPV
   TProfile * m_frac_nj_pt_b_match_pu_npv10   = new TProfile("m_frac_nj_pt_b_match_pu_npv10","m_frac_nj_pt_b_match_pu_npv;pt_{jet};fraction of matched jets;", NPtBins, vpt);
   TProfile * m_frac_nj_pt_b_match_nopu_npv1 = new TProfile("m_frac_nj_pt_b_match_nopu_npv1","m_frac_nj_pt_b_match_nopu_npv;pt_{jet};fraction of matched jets;", NPtBins,vpt);
   TProfile * m_frac_nj_pt_b_match_pu_npv20   = new TProfile("m_frac_nj_pt_b_match_pu_npv20","m_frac_nj_pt_b_match_pu_npv;pt_{jet};fraction of matched jets;", NPtBins, vpt);
   TProfile * m_frac_nj_pt_b_match_nopu_npv2 = new TProfile("m_frac_nj_pt_b_match_nopu_npv2","m_frac_nj_pt_b_match_nopu_npv;pt_{jet};fraction of matched jets;", NPtBins,vpt);
   TProfile * m_frac_nj_pt_b_match_pu_npv30   = new TProfile("m_frac_nj_pt_b_match_pu_npv30","m_frac_nj_pt_b_match_pu_npv;pt_{jet};fraction of matched jets;", NPtBins, vpt);
   TProfile * m_frac_nj_pt_b_match_nopu_npv3 = new TProfile("m_frac_nj_pt_b_match_nopu_npv3","m_frac_nj_pt_b_match_nopu_npv;pt_{jet};fraction of matched jets;", NPtBins,vpt);
   TProfile * m_frac_nj_pt_b_match_pu_npvO    = new TProfile("m_frac_nj_pt_b_match_pu_npvO","m_frac_nj_pt_b_match_pu_npv;pt_{jet};fraction of matched jets;", NPtBins, vpt);
   TProfile * m_frac_nj_pt_b_match_nopu_npvO  = new TProfile("m_frac_nj_pt_b_match_nopu_npvO","m_frac_nj_pt_b_match_nopu_npv;pt_{jet};fraction of matched jets;", NPtBins,vpt);
  
   //-		fraction of matched jets (reconstructed--->generated) in barrel regions, with different NPV
   TProfile * m_frac_nj_pt_b_match_RG_pu   = new TProfile("m_frac_nj_pt_b_match_RG_pu","m_frac_nj_pt_b_match_RG_pu;pt_{jet};fraction of matched jets;", NPtBins, vpt);
   TProfile * m_frac_nj_pt_e_match_RG_pu   = new TProfile("m_frac_nj_pt_e_match_RG_pu","m_frac_nj_pt_e_match_RG_pu;pt_{jet};fraction of matched jets;", NPtBins, vpt);
   TProfile * m_frac_nj_pt_f_match_RG_pu   = new TProfile("m_frac_nj_pt_f_match_RG_pu","m_frac_nj_pt_f_match_RG_pu;pt_{jet};fraction of matched jets;", NPtBins, vpt);
   TProfile * m_frac_nj_pt_b_match_RG_nopu   = new TProfile("m_frac_nj_pt_b_match_RG_nopu","m_frac_nj_pt_b_match_RG_nopu;pt_{jet};fraction of matched jets;", NPtBins, vpt);
   TProfile * m_frac_nj_pt_e_match_RG_nopu   = new TProfile("m_frac_nj_pt_e_match_RG_nopu","m_frac_nj_pt_e_match_RG_nopu;pt_{jet};fraction of matched jets;", NPtBins, vpt);
   TProfile * m_frac_nj_pt_f_match_RG_nopu   = new TProfile("m_frac_nj_pt_f_match_RG_nopu","m_frac_nj_pt_f_match_RG_nopu;pt_{jet};fraction of matched jets;", NPtBins, vpt);
   TProfile * m_frac_nj_pt_b_match_RG_pu_npv10   = new TProfile("m_frac_nj_pt_b_match_RG_pu_npv10","m_frac_nj_pt_b_match_RG_pu_npv;pt_{jet};fraction of matched jets;", NPtBins, vpt);
   TProfile * m_frac_nj_pt_b_match_RG_pu_npv20   = new TProfile("m_frac_nj_pt_b_match_RG_pu_npv20","m_frac_nj_pt_b_match_RG_pu_npv;pt_{jet};fraction of matched jets;", NPtBins, vpt);
   TProfile * m_frac_nj_pt_b_match_RG_pu_npv30   = new TProfile("m_frac_nj_pt_b_match_RG_pu_npv30","m_frac_nj_pt_b_match_RG_pu_npv;pt_{jet};fraction of matched jets;", NPtBins, vpt);
   TProfile * m_frac_nj_pt_b_match_RG_pu_npvO    = new TProfile("m_frac_nj_pt_b_match_RG_pu_npvO","m_frac_nj_pt_b_match_RG_pu_npv;pt_{jet};fraction of matched jets;", NPtBins, vpt);

   // Number of all jets vs. NPV in PU sample
   TProfile *m_all_nj_npv = new TProfile("m_all_nj_npv","m_all_nj_npv;NPV;# of jets/event",30,0.5,30.5);

   // Number of matched jets vs. NPV in PU sample
   TProfile *m_matched_nj_npv = new TProfile("m_matched_nj_npv","m_matched_nj_npv;NPV;# of jets/event",30,0.5,30.5);

   // Number of unmatched jets vs. NPV in PU sample
   TProfile *m_unmatched_nj_npv = new TProfile("m_unmatched_nj_npv","m_unmatched_nj_npv;NPV;# of jets/event",30,0.5,30.5);

   // Average jet pt vs. NPV in PU sample  
   TProfile *m_all_jtpt_npv = new TProfile("m_all_jtpt_npv","m_all_jtpt_npv;NPV;<jet pt> (GeV)", 30, 0.5, 30.5);

   // Average matched jet pt vs. NPV in PU sample  
   TProfile *m_matched_jtpt_npv = new TProfile("m_matched_jtpt_npv","m_matched_jtpt_npv;NPV;<jet pt> (GeV)", 30, 0.5, 30.5);

   // Average unmatched jet pt vs. NPV in PU sample  
   TProfile *m_unmatched_jtpt_npv = new TProfile("m_unmatched_jtpt_npv","m_unmatched_jtpt_npv;NPV;<jet pt> (GeV)", 30, 0.5, 30.5);

   // KEEP -  Number of unmatched jets per event in different NPv bins. 
   TProfile *  m_njet_pt_npv_unmatch[6]; // 0-4, 5-9, 10-14, 15-19, 20-24, 25-29
   for (int n = 0 ; n < 6 ; n++){
      TString titles = ";p_{T,J};# PU Jets /event;";
      TString name = Form("m_njet_pt_npv%i_%i_unmatch",n*5,n*5+4);
      m_njet_pt_npv_unmatch[n] = new TProfile(name, name+titles, NPtBins, vpt); // number of jets
   }

   // KEEP -  Number of unmatched jets per event in different NPv bins. 
   TProfile2D *  m_njet_etaVspt_npv_unmatch[6]; // 0-4, 5-9, 10-14, 15-19, 20-24, 25-29
   for (int n = 0 ; n < 6 ; n++){
      TString titles = ";#eta_{J}; p_{T,J};# PU Jets /event;";
      TString name = Form("m_njet_etaVspt_npv%i_%i_unmatch",n*5,n*5+4);
      m_njet_etaVspt_npv_unmatch[n] = new TProfile2D(name, name+titles, NETA, veta, NPtBins, vpt); // number of jets
   }


   // KEEP - HISTOS OF PU!!!, hence the p_suffix  
   TProfile2D * p_off_etaVsNpv 	= new TProfile2D("p_off_etaVsNpv","p_off_etaVsNpv;#eta_{j};N_{PV};Offset (p_{T}, GeV)",NETA, veta, 50,0,50);
   TProfile2D * p_off_etaVsRho 	= new TProfile2D("p_off_etaVsRho","p_off_etaVsRho;#eta_{j};Rho;Offset (p_{T}, GeV)",NETA, veta, 50,0,50);
   TProfile2D * p_off_etaVspueff	= new TProfile2D("p_off_etaVsPUEff","p_off_etaVsPUEff;#eta_{j};PU_{Eff};Offset (p_{T}, GeV)",NETA, veta, 75,0,75);
   TProfile2D * p_off_etaVsGenSumPtOA = new TProfile2D("p_off_etaVsGenSumPtOA","p_off_etaVsGenSumPtOA;#eta_{j};GenSumPtOA;Offset (p_{T}, GeV)",NETA, veta, 100,0,2000);
   TProfile2D * p_off_etaVsJetPt	= new TProfile2D("p_off_etaVsJetPt","p_off_etaVsJetPt;#eta_{j};p_{T}^{pu};Offset (p_{T}, GeV)",NETA, veta,NPtBins, vpt);
   TProfile2D * p_offOverA_etaVsJetPt	= new TProfile2D("p_offOverA_etaVsJetPt","p_offOverA_etaVsJetPt;#eta_{j};p_{T}^{pu};OffsetOverArea",NETA, veta,NPtBins, vpt);

   TProfile3D * p_offOverA_etaVsTnpusVsJetPt = new TProfile3D("p_offOverA_etaVsTnpusVsJetPt","p_offOverA_etaVsTnpusVsJetPt;#eta_{j};tnpu;p_{T}^{gen};OffsetOverAre",NETA,veta,NRHO,vrho,NPtBins,vpt);
   TProfile3D * p_PtAve_etaVsTnpusVsJetPt = new TProfile3D("p_PtAve_etaVsTnpusVsJetPt","p_PtAve_etaVsTnpusVsJetPt;#eta_{j};Tnpus;p_{T}^{gen};PtAve",NETA,veta,NRHO,vrho,NPtBins,vpt);
   TProfile3D * p_RhoAve_etaVsTnpusVsJetPt = new TProfile3D("p_RhoAve_etaVsTnpusVsJetPt","p_RhoAve_etaVsTnpusVsJetPt;#eta_{j};Tnpus;p_{T}^{gen};PtAve",NETA,veta,NRHO,vrho,NPtBins,vpt);


   TProfile3D * p_offOverA_etaVsRhoVsJetPt = new TProfile3D("p_offOverA_etaVsRhoVsJetPt","p_offOverA_etaVsRhoVsJetPt;#eta_{j};Rho;p_{T}^{gen};OffsetOverAre",NETA,veta,NRHO,vrho,NPtBins,vpt);
   TProfile3D * p_offOverA_etaVsNPVVsJetPt = new TProfile3D("p_offOverA_etaVsNPVVsJetPt","p_offOverA_etaVsNPVVsJetPt;#eta_{j};NPV;p_{T}^{gen};OffsetOverAre",NETA,veta,NRHO,vrho,NPtBins,vpt);
   TProfile3D * p_offOverA_etaVsN_RVsJetPt = new TProfile3D("p_offOverA_etaVsN_RVsJetPt","p_offOverA_etaVsN_RVsJetPt;#eta_{j};(NPV+Rho)/2;p_{T}^{gen};OffsetOverAre",NETA,veta,NRHO,vrho,NPtBins,vpt);
  
   TProfile3D * p_PtAve_etaVsRhoVsJetPt = new TProfile3D("p_PtAve_etaVsRhoVsJetPt","p_PtAve_etaVsRhoVsJetPt;#eta_{j};Rho;p_{T}^{gen};PtAve",NETA,veta,NRHO,vrho,NPtBins,vpt);
   TProfile3D * p_PtAve_etaVsNPVVsJetPt = new TProfile3D("p_PtAve_etaVsNPVVsJetPt","p_PtAve_etaVsNPVVsJetPt;#eta_{j};NPV;p_{T}^{gen};PtAve",NETA,veta,NRHO,vrho,NPtBins,vpt);
   TProfile3D * p_PtAve_etaVsN_RVsJetPt = new TProfile3D("p_PtAve_etaVsN_RVsJetPt","p_PtAve_etaVsN_RVsJetPt;#eta_{j};(NPV+Rho)/2;p_{T}^{gen};PtAve",NETA,veta,NRHO,vrho,NPtBins,vpt);
  
   TH2F * p_npvVsoff	=	 new TH2F("p_npvVsOff","p_npvVsOff;<p_{T} Offset>_{jets} (GeV);N_{PV}",80,0,80,80,0,80);		//ZQ  
   TH2F * p_rhoVsoff	=	 new TH2F("p_rhoVsOff","p_rhoVsOff;<p_{T} Offset>_{jets} (GeV);Rho",80,0,80,80,0,80);		//ZQ

   TH2D *p_offresVsrefpt_bb[6];
   for (int iPF=0;iPF<6;iPF++)
   {
      TString hname = Form("p_offResVsrefpt_bb_%s",PFstr[iPF].Data());
      p_offresVsrefpt_bb[iPF] = new TH2D(hname,hname+";p_{T}^{ref};offset_"+PFstr[iPF]+" (GeV)",NPtBins,vpt,1000,-300,300);
   }
  
   TH2D * p_npvVsoff_det[4], * p_rhoVsoff_det[4];																								//ZQ  Break into 4 different detector region
   for (int det=0;det<4;det++)
   {
      TString hname = Form("p_npvVsOff_%s",detstr[det].Data());
      p_npvVsoff_det[det]	=	new TH2D(hname,hname+";<p_{T} Offset>_{jets} (GeV);N_{PV}",80,0,80,80,0,80);
      hname = Form("p_rhoVsOff_%s",detstr[det].Data());
      p_rhoVsoff_det[det]	=	new TH2D(hname,hname+";<p_{T} Offset>_{jets} (GeV);Rho",80,0,80,80,0,80);
   }
  
  
   TProfile *	p_matchedjet_off	=	new TProfile("p_matchedjet_off","p_matchedjet_off;<p_{T} Offset>_{jets} (GeV);# of matched jets",80,0,80);
  
  
   TH2F * p_npvVsRho_offset_15_15h = new TH2F("p_npvVsRho_offset_15_15h","p_npvVsRho_offset_15_15h;Rho;N_{PV}",80,0,80,80,0,80);	//ZQ
   TH3F * p_rho_npv_refpt_BB = new TH3F("p_rho_npv_refpt_BB","p_rho_npv_refpt_BB;Rho;N_{PV};p_{T}^{ref}",NRHO,vrho,NRHO,vrho,NPtBins,vpt);
   TProfile3D *p_offsetOA_rho_npv_refpt_BB = new TProfile3D("p_offsetOA_rho_npv_refpt_BB","p_offsetOA_rho_npv_refpt_BB;Rho;N_{PV};p_{T}^{ref};offsetOA",NRHO,vrho,NRHO,vrho,NPtBins,vpt);
   TProfile * p_drVsrefpt = new TProfile("p_drVsrefpt","p_drVsrefpt;p_{T}^{ref};d_{R}",NPtBins, vpt);			//ZQ

   TH2F * p_areaVsrefpt = new TH2F("p_areaVsrefpt","p_areaVsrefpt;p_{T}^{ref};jtarea^{pu}-jtarea^{nopu}",NPtBins, vpt,100,-1,1);
   TH2F * p_areaVsoffset_1000 = new TH2F("p_areaVsoffset_1000","p_areaVsoffset_1000;offset;jtarea^{pu}-jtarea^{nopu}",100,-500,500,100,-1,1);
   TH2F * p_areaVsoffset_30_50 = new TH2F("p_areaVsoffset_30_50","p_areaVsoffset_30_50;offset;jtarea^{pu}-jtarea^{nopu}",100,-100,100,100,-1,1);


   //  To obtain the response ratio to pt_nopu a function of pt and Npv
   // generate 4 histograms for eta 4 detector regions, and 6 for the npv regions 
   TH2D * p_resnopu_refptVsNpv[4][6]; 
   for (int det=0;det<4;det++){
      for (int npv=0;npv<6;npv++){
         TString hname = Form("p_resnopuVsrefpt_%s_npv%i_%i",detstr[det].Data(),npv*5,npv*5+4);
         p_resnopu_refptVsNpv[det][npv] = new TH2D(hname,hname+";p^{ref}_{T}; p_{T}/p_{T}^{nopu};",NPtBins, vpt,100,0,5) ;
      }//npv
   }//det

   //  To obtain the resolution as a function of pt and Npv
   // generate 4 histograms for eta 4 detector regions, and 6 for the npv regions 
   TH2D * p_resnopu_refptVsRho[4][6]; 
   for (int det=0;det<4;det++){
      for (int npv=0;npv<6;npv++){
         TString hname = Form("p_resnopuVsrefpt_%s_rho%i_%i",detstr[det].Data(),npv*5,npv*5+4);
         p_resnopu_refptVsRho[det][npv] = new TH2D(hname,hname+";p^{ref}_{T}; p_{T}/p_{T}^{nopu};",NPtBins, vpt,100,0,5) ;
      }//npv
   }//det


   //  To obtain the response ratio to pt_nopu a function of pt and Npv
   // generate 4 histograms for eta 4 detector regions, and 6 for the npv regions 
   TH2D * p_res_refptVsNpv[4][6]; 

   for (int det=0;det<4;det++){
      for (int npv=0;npv<6;npv++){
         TString hname = Form("p_resVsrefpt_%s_npv%i_%i",detstr[det].Data(),npv*5,npv*5+4);
         p_res_refptVsNpv[det][npv] = new TH2D(hname,hname+";p^{ref}_{T}; p_{T}^{pu}/p_{T}^{ref};",NPtBins, vpt,100,0,5) ;
      }//npv
   }//det


   TH2D * p_res_refpt[4];
   for (int det=0;det<4;det++){

      TString hname = Form("p_resVsrefpt_%s",detstr[det].Data());
      p_res_refpt[det] = new TH2D(hname,hname+";p^{ref}_{T}; p_{T}^{pu}/p_{T}^{ref};",NPtBins, vpt,100,0,5) ;

   }//det
  
   TH2D * np_res_refpt[4];
   for (int det=0;det<4;det++){

      TString hname = Form("np_resVsrefpt_%s",detstr[det].Data());
      np_res_refpt[det] = new TH2D(hname,hname+";p^{ref}_{T}; p_{T}^{nopu}/p_{T}^{ref};",NPtBins, vpt,100,0,5) ;

   }//det
  
  
   //  To obtain the offset /response ratio to pt_nopu a function of pt and Npv-------------------ZQ
   // generate 4 histograms for eta 4 detector regions, and 6 for the npv regions 
   TH2D * p_offres_refptVsNpv[4][6]; 
   for (int det=0;det<4;det++){
      for (int npv=0;npv<6;npv++){
         TString hname = Form("p_offresVsrefpt_%s_npv%i_%i",detstr[det].Data(),npv*5,npv*5+4);
         p_offres_refptVsNpv[det][npv] = new TH2D(hname,hname+";p^{ref}_{T}; p_{T}^{pu}-p_{T}^{nopu};",NPtBins, vpt,1000,-300,300) ;
      }//npv
   }//det


   //  To obtain the resolution as a function of pt and Rho
   // generate 4 histograms for eta 4 detector regions, and 6 for the Rho regions 
   TH2D * p_res_refptVsRho[4][6]; 
   for (int det=0;det<4;det++){
      for (int npv=0;npv<6;npv++){
         TString hname = Form("p_resVsrefpt_%s_rho%i_%i",detstr[det].Data(),npv*5,npv*5+4);
         p_res_refptVsRho[det][npv] = new TH2D(hname,hname+";p^{ref}_{T}; p_{T}^{pu}-p_{T}^{nopu};",NPtBins, vpt,100,0,5) ;
      }//npv
   }//det

   //  To obtain the offset/response as a function of pt and Rho--------------------------ZQ
   // generate 4 histograms for eta 4 detector regions, and 6 for the Rho regions 
   TH2D * p_offres_refptVsRho[4][6]; 
   for (int det=0;det<4;det++){
      for (int npv=0;npv<6;npv++){
         TString hname = Form("p_offresVsrefpt_%s_rho%i_%i",detstr[det].Data(),npv*5,npv*5+4);
         p_offres_refptVsRho[det][npv] = new TH2D(hname,hname+";p^{ref}_{T}; p_{T}/p_{T}^{nopu};",NPtBins, vpt,1000,-300,300) ;
      }//npv
   }//det
  

   //=========================================================
   //              DECLARATION OF HISTOS ENDS HERE
   //=========================================================

   //Test, loop over events in the map
   int nevs = 0;

   //for (IT::const_iterator it = mapTreePU.begin(); it != mapTreePU.end()    ; ++it) {			//Change back
   for (IT::const_iterator it = mapTreePU.begin(); it != mapTreePU.end()  &&  (!iftest || (nevs < 400000))   ; ++it) {

      if (nevs%10000==0) cout << "\t"<<nevs << endl;

      // if this entry does not exist on the second ntuple just skip this event
      if (mapTreeNoPU.find(it->first) == mapTreeNoPU.end())
         continue;

      // Load the entries at the proper place.
      tpu->GetEntry(mapTreePU[it->first].second);
      tnopu->GetEntry(mapTreeNoPU[it->first].second);
      if (tpu->npv ==0 || tnopu->npv == 0) continue;
      // HERE correct the jets in tpu if requested

      // Create the mapping of matched jets.
      // key is PU, value is for NoPU
      fillJetMap(mapJet,tpu,tnopu);

      //=========================================================
      //              FILLING OF HISTOS START HERE
      //=========================================================

      // GENERAL HISTOS, no cuts.
      g_nj ->Fill(tpu->nref,tnopu->nref);// njet distributions
      g_npv->Fill(tpu->npv,tnopu->npv);  // npv dist.
      g_rho->Fill(tpu->rho,tnopu->rho);  // rho dist
      g_pthat->Fill(tpu->pthat,tnopu->pthat);			//pthat distributions--ZQ
      g_deltaNpv->Fill(tpu->npv,tpu->npv - tnopu->npv);     // Does the number of NPv changes ?
      m_deltaPthat->Fill(tpu->pthat,tpu->pthat-tnopu->pthat);	//pthat sanity check--ZQ

      if (tnopu->npv!=1) {
         cout << "\tWARNING::The no PU sample has more than 1 PV." << endl
              << "\tSkipping this event." << endl;
         continue;
      }


      int inpv = getNpvIndex(tpu->npv);
      int irho = getNpvIndex(tpu->rho);
      //	Applying JEC from textfile

      if (ApplyJEC != 0)
      {
         for (int j1 = 0; j1 < tpu->nref; j1++)
         {
            JetCorrector->setJetEta(tpu->jteta[j1]);
            JetCorrector->setJetPt(tpu->jtpt[j1]);
            JetCorrector->setJetA(tpu->jtarea[j1]);
            JetCorrector->setRho(tpu->rho);
            double correction = JetCorrector->getCorrection();
//         cout <<correction<<" "<<tpu->jtpt[j1]<<endl;
            tpu->jtpt[j1] *= correction;
//         cout <<tpu->jtpt[j1]<<endl;
         }

      }

      double avg_jtpt_all = 0;
      double avg_jtpt_matched = 0;
      double avg_jtpt_unmatched = 0;

      // fill unmatch jets for PU sample
      for (int j1 = 0; j1 < tpu->nref; j1++){

         // matching recon-jet with gen-jet
         bool ismatchRG = tpu->refdrjt[j1]<0.25;
    	

         m_njet_pt_pu->Fill(tpu->jtpt[j1]);
      
         avg_jtpt_all += tpu->jtpt[j1];

         // if j1 is matched in this sample
         bool ismatch =  mapJet.find(j1) != mapJet.end();
      
         if (ismatch)	avg_jtpt_matched   += tpu->jtpt[j1];
         else				avg_jtpt_unmatched += tpu->jtpt[j1];
      
         if (fabs(tpu->jteta[j1])<1.3)
         {
			m_frac_nj_pt_b_match_pu->Fill(tpu->jtpt[j1],ismatch);
			m_frac_nj_pt_b_match_RG_pu->Fill(tpu->jtpt[j1],ismatchRG);
			if (tpu->npv<=10)
			{
               m_frac_nj_pt_b_match_pu_npv10->Fill(tpu->jtpt[j1],ismatch);
               m_frac_nj_pt_b_match_RG_pu_npv10->Fill(tpu->jtpt[j1],ismatchRG);
			}
			else if (tpu->npv<=20)
			{
               m_frac_nj_pt_b_match_pu_npv20->Fill(tpu->jtpt[j1],ismatch);
               m_frac_nj_pt_b_match_RG_pu_npv20->Fill(tpu->jtpt[j1],ismatchRG);
			}
			else if (tpu->npv<=30)
			{
               m_frac_nj_pt_b_match_pu_npv30->Fill(tpu->jtpt[j1],ismatch);
               m_frac_nj_pt_b_match_RG_pu_npv30->Fill(tpu->jtpt[j1],ismatchRG);
			}
			else
			{
               m_frac_nj_pt_b_match_pu_npvO->Fill(tpu->jtpt[j1],ismatch);
               m_frac_nj_pt_b_match_RG_pu_npvO->Fill(tpu->jtpt[j1],ismatchRG);
			}
         }
         else if (fabs(tpu->jteta[j1])<3)
         {
			m_frac_nj_pt_e_match_pu->Fill(tpu->jtpt[j1],ismatch);
			m_frac_nj_pt_e_match_RG_pu->Fill(tpu->jtpt[j1],ismatchRG);
         }
         else 
         {
			m_frac_nj_pt_f_match_pu->Fill(tpu->jtpt[j1],ismatch);
			m_frac_nj_pt_f_match_RG_pu->Fill(tpu->jtpt[j1],ismatchRG);
         }
         if (!ismatch){
            m_njet_pt_npv_unmatch[inpv]->Fill(tpu->jtpt[j1],+1);
            m_njet_etaVspt_npv_unmatch[inpv]->Fill(tpu->jteta[j1], tpu->jtpt[j1],+1);
         }
      }

      if (tpu->nref>0)               avg_jtpt_all        /= (double) tpu->nref;     else avg_jtpt_all = 0;
      if (mapJet.size()>0)           avg_jtpt_matched    /= (double) mapJet.size(); else avg_jtpt_matched = 0;
      if (tpu->nref-mapJet.size()>0) avg_jtpt_unmatched  /= (double) (tpu->nref - mapJet.size()); else avg_jtpt_unmatched = 0;
      m_all_nj_npv          ->Fill(tpu->npv,tpu->nref);
      m_matched_nj_npv      ->Fill(tpu->npv, mapJet.size());
      m_unmatched_nj_npv    ->Fill(tpu->npv, tpu->nref-mapJet.size());

      m_all_jtpt_npv        ->Fill(tpu->npv, avg_jtpt_all);
      m_matched_jtpt_npv    ->Fill(tpu->npv, avg_jtpt_matched);
      m_unmatched_jtpt_npv  ->Fill(tpu->npv, avg_jtpt_unmatched);
      // fill unmatch jets for NOPU sample
      for (int j1 = 0; j1 < tnopu->nref; j1++){

         m_njet_pt_nopu->Fill(tnopu->jtpt[j1]);

         bool ismatchRG = tnopu->refdrjt[j1]<0.25;

         // if j1 is matched in this sample. More complicated b/c I need
         //  to search the value of the map, not the key.
         bool ismatch = false;
         for (ITJ::const_iterator itj = mapJet.begin(); itj != mapJet.end(); itj++) {
            if (itj->second == j1){
               ismatch = true;
               break;
            }
         }//for 

         if (fabs(tnopu->jteta[j1])<1.3)
         {
            m_frac_nj_pt_b_match_nopu->Fill(tnopu->jtpt[j1],ismatch);
            m_frac_nj_pt_b_match_RG_nopu->Fill(tnopu->jtpt[j1],ismatchRG);
            if (tpu->npv<=10)
               m_frac_nj_pt_b_match_nopu_npv1->Fill(tnopu->jtpt[j1],ismatch);
            else if (tpu->npv<=20)
               m_frac_nj_pt_b_match_nopu_npv2->Fill(tnopu->jtpt[j1],ismatch);
            else if (tpu->npv<=30)
               m_frac_nj_pt_b_match_nopu_npv3->Fill(tnopu->jtpt[j1],ismatch);
            else
               m_frac_nj_pt_b_match_nopu_npvO->Fill(tnopu->jtpt[j1],ismatch);
         }
         else if (fabs(tnopu->jteta[j1])<3)
         {
			m_frac_nj_pt_e_match_nopu->Fill(tnopu->jtpt[j1],ismatch);
			m_frac_nj_pt_e_match_RG_nopu->Fill(tnopu->jtpt[j1],ismatchRG);
         }
         else 
         {
			m_frac_nj_pt_f_match_nopu->Fill(tnopu->jtpt[j1],ismatch);
			m_frac_nj_pt_f_match_RG_nopu->Fill(tnopu->jtpt[j1],ismatchRG);
         }
         if (!ismatch){
            m_njet_pt_npv_unmatch[inpv]->Fill(tnopu->jtpt[j1],-1);
            m_njet_etaVspt_npv_unmatch[inpv]->Fill(tnopu->jteta[j1], tnopu->jtpt[j1],-1);
         }
      }

      double avg_offset = 0;
      double avg_offset_det[4] = {0,0,0,0};
      double njet_det[4]=	{0,0,0,0};

      // MATCHING HISTOS. 
      // Loop over matched jets
      for (ITJ::const_iterator itj = mapJet.begin(); itj != mapJet.end(); itj++) {

         int jpu = itj->first;
         int jnopu = itj->second;
         //double eta_avg = 0.5*(tpu->jteta[jpu]+tnopu->jteta[jnopu]);
         double offset  = tpu->jtpt[jpu] - tnopu->jtpt[jnopu] ;
//      cout <<"=====  "<<tpu->jtpt[jpu]<<" "<<tnopu->jtpt[jnopu]<<endl;
         double areaDiff= tpu->jtarea[jpu]-tnopu->jtarea[jnopu];
         //double off_per_npv = 20.0 * offset / tpu->npv;
         int diff_pdgid = tpu->refpdgid[jpu] - tnopu->refpdgid[jnopu];
         double resp       = tpu->jtpt[jpu]/tpu->refpt[jpu];   // response relative to reference jet
         double respTonopu = tpu->jtpt[jpu]/tnopu->jtpt[jnopu];// response relative to no pu jet
         double respNopu	= tnopu->jtpt[jnopu]/tnopu->refpt[jnopu]; // response no pu jet to reference jet
         double PUEff		= 0.020*(tpu->npus->at(0))+0.975*(tpu->npus->at(1))+0.005*(tpu->npus->at(2)); // effective pu
         double GenSumPtOA	= (0.020*(tpu->sumpt_lowpt->at(0))+0.975*(tpu->sumpt_lowpt->at(1))+0.005*(tpu->sumpt_lowpt->at(2)))/tpu->jtarea[jpu];
         double offset_jtchf = tpu->jtpt[jpu]*tpu->jtchf[jpu]-tnopu->jtpt[jnopu]*tnopu->jtchf[jnopu];
         double offset_jtnhf = tpu->jtpt[jpu]*tpu->jtnhf[jpu]-tnopu->jtpt[jnopu]*tnopu->jtnhf[jnopu];
         double offset_jtnef = tpu->jtpt[jpu]*tpu->jtnef[jpu]-tnopu->jtpt[jnopu]*tnopu->jtnef[jnopu];
         double offset_jtcef = tpu->jtpt[jpu]*tpu->jtcef[jpu]-tnopu->jtpt[jnopu]*tnopu->jtcef[jnopu];
         double offset_jthfhf = tpu->jtpt[jpu]*tpu->jthfhf[jpu]-tnopu->jtpt[jnopu]*tnopu->jthfhf[jnopu];
         double offset_jthfef = tpu->jtpt[jpu]*tpu->jthfef[jpu]-tnopu->jtpt[jnopu]*tnopu->jthfef[jnopu];
         double offsetOA = offset/tpu->jtarea[jpu];

         p_off_etaVsNpv->Fill(tpu->jteta[jpu],tpu->npv,offset);
         p_off_etaVsRho->Fill(tpu->jteta[jpu],tpu->rho,offset);
         p_off_etaVspueff->Fill(tpu->jteta[jpu],PUEff,offset);
         p_off_etaVsGenSumPtOA->Fill(tpu->jteta[jpu],GenSumPtOA,offset);
         p_off_etaVsJetPt->Fill(tpu->jteta[jpu],tpu->jtpt[jpu],offset);
         p_offOverA_etaVsJetPt->Fill(tpu->jteta[jpu],tpu->jtpt[jpu],offsetOA);
      
         p_offOverA_etaVsTnpusVsJetPt->Fill(tpu->jteta[jpu],tpu->tnpus->at(1),tpu->refpt[jpu],offsetOA);
         p_PtAve_etaVsTnpusVsJetPt   ->Fill(tpu->jteta[jpu],tpu->tnpus->at(1),tpu->refpt[jpu],tpu->jtpt[jpu]);
         p_RhoAve_etaVsTnpusVsJetPt  ->Fill(tpu->jteta[jpu],tpu->tnpus->at(1),tpu->refpt[jpu],tpu->rho);

         p_offOverA_etaVsRhoVsJetPt->Fill(tpu->jteta[jpu],tpu->rho,tpu->refpt[jpu],offsetOA);
         p_PtAve_etaVsRhoVsJetPt->Fill(tpu->jteta[jpu],tpu->rho,tpu->refpt[jpu],tpu->jtpt[jpu]);
         p_offOverA_etaVsNPVVsJetPt->Fill(tpu->jteta[jpu],tpu->npv,tpu->refpt[jpu],offsetOA);
         p_PtAve_etaVsNPVVsJetPt->Fill(tpu->jteta[jpu],tpu->npv,tpu->refpt[jpu],tpu->jtpt[jpu]);
         p_offOverA_etaVsN_RVsJetPt->Fill(tpu->jteta[jpu],(tpu->rho+tpu->npv)/2.,tpu->refpt[jpu],offsetOA);
         p_PtAve_etaVsN_RVsJetPt->Fill(tpu->jteta[jpu],(tpu->rho+tpu->npv)/2,tpu->refpt[jpu],tpu->jtpt[jpu]);
      
         p_offresVsrefpt_bb[0]->Fill(tpu->refpt[jpu],offset_jtchf);
         p_offresVsrefpt_bb[1]->Fill(tpu->refpt[jpu],offset_jtnhf);
         p_offresVsrefpt_bb[2]->Fill(tpu->refpt[jpu],offset_jtnef);
         p_offresVsrefpt_bb[3]->Fill(tpu->refpt[jpu],offset_jtcef);
         p_offresVsrefpt_bb[4]->Fill(tpu->refpt[jpu],offset_jthfhf);
         p_offresVsrefpt_bb[5]->Fill(tpu->refpt[jpu],offset_jthfef);

      
         p_areaVsrefpt->Fill(tpu->refpt[jpu],areaDiff);
         if (tpu->refpt[jpu]>1000)
            p_areaVsoffset_1000->Fill(offset,areaDiff);
         if (tpu->refpt[jpu]>30 && tpu->refpt[jpu]<50)
            p_areaVsoffset_30_50->Fill(offset,areaDiff);
      
         p_drVsrefpt->Fill(tpu->refpt[jpu],tpu->refdrjt[jpu]);

         m_refpt_diff->Fill(tpu->refpt[jpu] - tnopu->refpt[jnopu]);
         m_refpdgid_diff->Fill(diff_pdgid);

         int idet = getDetIndex(tpu->jteta[jpu]);		 

         p_res_refpt[idet]->Fill(tpu->refpt[jpu],resp);
         np_res_refpt[idet]->Fill(tpu->refpt[jpu],respNopu);

      
         p_res_refptVsNpv[idet][inpv]->Fill(tpu->refpt[jpu],resp);
         p_offres_refptVsNpv[idet][inpv]->Fill(tpu->refpt[jpu],offset);			//ZQ
//      p_offresOrefpt_refptVsNpv[idet][inpv]->Fill(tpu->refpt[jpu],offset/tpu->refpt[jpu]);
         p_res_refptVsRho[idet][irho]->Fill(tpu->refpt[jpu],resp);
         p_offres_refptVsRho[idet][irho]->Fill(tpu->refpt[jpu],offset);			//ZQ
//      p_offresOrefpt_refptVsRho[idet][irho]->Fill(tpu->refpt[jpu],offset/tpu->refpt[jpu]);
         p_resnopu_refptVsNpv[idet][inpv]->Fill(tpu->refpt[jpu],respTonopu);
         p_resnopu_refptVsRho[idet][irho]->Fill(tpu->refpt[jpu],respTonopu);
         if (offset > 15 && offset < 15.5)	p_npvVsRho_offset_15_15h->Fill(tpu->rho,tpu->npv);		//2D histo npv vs. rho with 15<offset<15.5---ZQ		    
         if (idet == 0) 
         {
			p_rho_npv_refpt_BB->Fill(tpu->rho,tpu->npv,tpu->refpt[jpu]);
			p_offsetOA_rho_npv_refpt_BB->Fill(tpu->rho,tpu->npv,tpu->refpt[jpu],offsetOA);
         }
         avg_offset +=  offset;
         avg_offset_det[idet]+=	offset;													//ZQ 10.29
         njet_det[idet]+=1.;

      } // for matched jets 

      avg_offset /= mapJet.size();
      for (int det=0;det<4;det++)																//ZQ 10.29
      {
         if (njet_det[det]!=0.)
         {
            avg_offset_det[det]	/=	njet_det[det];
            p_npvVsoff_det[det]->Fill(avg_offset_det[det],tpu->npv);
            p_rhoVsoff_det[det]->Fill(avg_offset_det[det],tpu->rho);
         }
      }
      p_npvVsoff->Fill(avg_offset,tpu->npv);
      p_rhoVsoff->Fill(avg_offset,tpu->rho);
      p_matchedjet_off->Fill(avg_offset,mapJet.size());							//ZQ 10.29


      //=========================================================
      //              FILLING OF HISTOS ENDS HERE
      //=========================================================

    
      nevs++;
    
   }//for

   fout->Write();
   cout<<" Writing file "<<fout->GetName()<<"..."<<flush;
   fout->Close();
   cout<<" Done!"<<endl;

   cout<<"Event-matching report"<<endl;
   cout<<"\t Total number of unique events in first  sample: "<<tpu->fChain->GetEntries()<<endl;
   cout<<"\t Total number of unique events in second sample: "<<tnopu->fChain->GetEntries()<<endl;
   cout<<"\t Number of matched events we ran over "<<nevs<<endl;

}//SynchTest
int main(int argc,char**argv)
{
   CommandLine cl;
   if (!cl.parse(argc,argv)) return 0;
   string         algo1     = cl.getValue<string>  ("algo1",   "ak5pf");
   string         algo2     = cl.getValue<string>  ("algo2",   "ak5pf");
   int            iftest    = cl.getValue<int>     ("iftest",   0);
   int            ApplyJEC  = cl.getValue<int>     ("ApplyJEC", 0);
   string         JECpar    = cl.getValue<string>  ("JECpar",   "parameters_ak5pf.txt");
  
  
   SynchTest(algo1,algo2,iftest,ApplyJEC,JECpar);
}

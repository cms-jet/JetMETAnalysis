///////////////////////////////////////////////////////////////////
//
// jet_correction_analyzer_x
// -------------------------
//
//            12/08/2011 Alexx Perloff  <aperloff@physics.tamu.edu>
///////////////////////////////////////////////////////////////////

#include "JetMETAnalysis/JetAnalyzers/interface/VectorWrapper.h"
#include "JetMETAnalysis/JetAnalyzers/interface/VectorWrapper2D.h"
#include "JetMETAnalysis/JetUtilities/interface/TProfileMDF.h"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/JetInfo.hh"
#include "JetMETAnalysis/JetUtilities/interface/JRAEvent.h"
#include "JetMETAnalysis/JetUtilities/interface/ProgressBar.hh"

#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"
#if __has_include("xrootd/XrdCl/XrdClFileSystem.hh")
#include "xrootd/XrdCl/XrdClFileSystem.hh"
#define has_xrdcl 1
#else
#define has_xrdcl 0
#endif

#include "TROOT.h"
#include "TSystem.h"
#include "TEnv.h"
#include <TObjectTable.h>
#include "TFile.h"
#include "TFileCollection.h"
#include "TTree.h"
#include "TChain.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2.h"
#include "TH2F.h"
#include "TH3D.h"
#include "TF1.h"
#include "TString.h"
#include "TMath.h"
#include "TFitResult.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TProfile3D.h"
#include "TBenchmark.h"

#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdarg.h>
#include <cstring>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// define local functions
////////////////////////////////////////////////////////////////////////////////

/// get the bin number for a specific ptgen according to the vector of bin edges 
int getBin(double x, const double boundaries[], int length);

/// get the flavor name used in initializing the JetCorrectorParameters
string get_flavor_name(int pdgid);

/// make the resolution vs. eta histogram
//void makeResolutionHistogram(TH2F* RelRspVs_, TH1F* ResolutionVs_, bool mpv);
void makeResolutionHistogram(TH3F* RespVs_, TH1F* ResolutionVs_, TString slice, bool mpv,
                             int slice_min = 0, int slice_max = -1);

/// add the error for one TProfile in quadrature with the value of another TProfile
void addErrorQuadrature(TProfile* hist, TProfile* ehist);

/// check the amount of IT pileup and see if it is in the specified range
bool it_pileup(int itlow, int ithigh, vector<int>* npus, int iIT);

/// check the amount of OOT pileup before nad after the event and see if it is in the specified range
bool oot_pileup(int earlyootlow, int earlyoothigh, int lateootlow, int lateoothigh,
                vector<int>* npus, int iIT);

/// check the sum of the OOT pileup before and after the event and see if it is in the specified range
bool total_oot_pileup(int totalootlow, int totaloothigh, vector<int>* npus, int iIT);

/// check the sum of all of the pileup in the event and see if it is in the specified range
bool total_pileup(int totallow, int totalhigh, vector<int>* npus, int iIT);

/// combines the booleans from the IT, OOT, and TotalOOT functions into one boolean
bool pileup_cut(int itlow, int ithigh, int earlyootlow, int earlyoothigh, 
                int lateootlow, int lateoothigh, int totalootlow, int totaloothigh, 
                int totallow, int totalhigh, vector<int>* npus, vector<int>* bxns);

/// returns the index in bxns, npus, and tnpus that corresponds to the IT PU
int itIndex(vector<int>* bxns);

/// returns the number of PUs before the index iIT (i.e. the current BX index)
double sumEOOT(vector<int>* npus, unsigned int iIT);

/// returns the number of PUs after the index iIT (i.e. the current BX index)
double sumLOOT(vector<int>* npus, unsigned int iIT);

/// returns the postfix associated with a specific level and algorithm
string getPostfix(vector<string> postfix, string alg, int level);

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv)
{
   gROOT->ProcessLine("#include<vector>");
   gSystem->Load("libFWCoreFWLite.so");
   gEnv->SetValue("TFile.AsyncPrefetching", 1);

   //
   // evaluate command-line / configuration file options
   // 
   CommandLine cl;
   if (!cl.parse(argc,argv)) return 0;

   vector<string>  algs              = cl.getVector<string>      ("algs");
   string          path              = cl.getValue<string>       ("path");
   string          era               = cl.getValue<string>       ("era");
   string          inputFilename     = cl.getValue<string>       ("inputFilename");
   string          inputFilePath     = cl.getValue<string>       ("inputFilePath",        "");
   string          fileList          = cl.getValue<string>       ("fileList",             "");
   string          url_string        = cl.getValue<string>       ("url_string",           "");
   TString         outputDir         = cl.getValue<TString>      ("outputDir",            "");
   TString         suffix            = cl.getValue<TString>      ("suffix",               "");
   vector<int>     levels            = cl.getVector<int>         ("levels",               "");
   bool            useTags           = cl.getValue<bool>         ("useTags",            true);
   bool            L1FastJet         = cl.getValue<bool>         ("L1FastJet",          true);
   vector<string>  postfix           = cl.getVector<string>      ("postfix",              "");
   bool            doflavor          = cl.getValue<bool>         ("doflavor",          false);
   bool            doTProfileMDF     = cl.getValue<bool>         ("doTProfileMDF",     false);
   bool            reduceHistograms  = cl.getValue<bool>         ("reduceHistograms",   true);
   bool            useweight         = cl.getValue<bool>         ("useweight",         false);
   float           pThatReweight     = cl.getValue<float>        ("pThatReweight",     -9999);
   float           xsection          = cl.getValue<float>        ("xsection",            0.0);
   float           luminosity        = cl.getValue<float>        ("luminosity",          1.0);
   int             pdgid             = cl.getValue<int>          ("pdgid",                 0);
   vector<double>  drmax             = cl.getVector<double>      ("drmax",                "");
   double          ptmin             = cl.getValue<double>       ("ptmin",                 0);
   double          ptgenmin          = cl.getValue<double>       ("ptgenmin",              0);
   double          ptrawmin          = cl.getValue<double>       ("ptrawmin",              0);
   float           pthatmin          = cl.getValue<float>        ("pthatmin",            0.0);
   float           pthatmax          = cl.getValue<float>        ("pthatmax",           -1.0);
   double          etamax            = cl.getValue<double>       ("etamax",                0);
   double          dphimin           = cl.getValue<double>       ("dphimin",               0);
   unsigned int    nrefmax           = cl.getValue<unsigned int> ("nrefmax",               0);
   int             nbinsrelrsp       = cl.getValue<int>          ("nbinsrelrsp",         200);
   float           relrspmin         = cl.getValue<float>        ("relrspmin",           0.0);
   float           relrspmax         = cl.getValue<float>        ("relrspmax",           2.0);
   unsigned int    evtmax            = cl.getValue<unsigned int> ("evtmax",                0);
   bool            printnpu          = cl.getValue<bool>         ("printnpu",          false);
   int             itlow             = cl.getValue<int>          ("itlow",                 0);
   int             ithigh            = cl.getValue<int>          ("ithigh",           100000);
   int             earlyootlow       = cl.getValue<int>          ("earlyootlow",           0);
   int             earlyoothigh      = cl.getValue<int>          ("earlyoothigh",     100000);
   int             lateootlow        = cl.getValue<int>          ("lateootlow",            0);
   int             lateoothigh       = cl.getValue<int>          ("lateoothigh",      100000);
   int             totalootlow       = cl.getValue<int>          ("totalootlow",           0);
   int             totaloothigh      = cl.getValue<int>          ("totaloothigh",     100000);
   int             totallow          = cl.getValue<int>          ("totallow",              0);
   int             totalhigh         = cl.getValue<int>          ("totalhigh",        100000);
   TString         weightfilename    = cl.getValue<TString>      ("weightfilename",       "");
   TString         MCPUReWeighting   = cl.getValue<TString>      ("MCPUReWeighting",      "");
   TString         DataPUReWeighting = cl.getValue<TString>      ("DataPUReWeighting",    "");
   bool            mpv               = cl.getValue<bool>         ("mpv",               false);
   TString         readRespVsPileup  = cl.getValue<TString>      ("readRespVsPileup",     "");
   bool            verbose           = cl.getValue<bool>         ("verbose",           false);
   bool            debug             = cl.getValue<bool>         ("debug",             false);

   if (!cl.check()) return 0;
   cl.print();

   TBenchmark* m_benchmark = new TBenchmark();
   m_benchmark->Reset();
   m_benchmark->Start("event");

   //
   // Do some additional check
   //

   // Check that if pThatReweight is set then useweight is also set
   if(pThatReweight!=-9999 && useweight==false) {
      cout << "ERROR::jet_correction_analyzer_x Can't reweight the pThat spectrum without first using the existing"
           << " weights to return to an unmodified spectrum. Set the \"useweight\" option to true." << endl;
           return -1;
   }

   // Check that the size of the drmax values matches that of the algs
   if(drmax.size()>0 && algs.size()!=drmax.size()) {
      cout << "ERROR::jet_correction_analyzer_x The size of the drmax vector must match the size of the algs vector" << endl;
      return 0;
   }

   //
   // Some useful quantities
   //
   const char pusources[3][10] = {"EOOT","IT","LOOT"};
   double vresp[nbinsrelrsp+1];
   double vcorr[nbinsrelrsp+1];
   for(int i=0; i<=nbinsrelrsp; i++) {
      vresp[i] = (i*((relrspmax-relrspmin)/(double)nbinsrelrsp));
      vcorr[i] = (i*((CorrHigh-CorrLow)/(double)nbinsrelrsp));
   }
   double vrho[NRhoBins+1];
   for(int i=0; i<=NRhoBins; i++) {
      vrho[i] = (i*((RhoHigh-RhoLow)/(double)NRhoBins));
   }

   edm::LumiReWeighting LumiWeights_;
   if(!MCPUReWeighting.IsNull() && !DataPUReWeighting.IsNull()) {
      LumiWeights_ = edm::LumiReWeighting(string(MCPUReWeighting),string(DataPUReWeighting),"pileup","pileup");
   }

   if(!outputDir.IsNull() && !outputDir.EndsWith("/")) outputDir += "/";
   TFile *outf = TFile::Open(outputDir+"Closure_"+JetInfo::ListToString(algs,string("_"))+suffix+".root","RECREATE");

   //
   // Loop over the algorithms
   //
   for(unsigned int a=0; a<algs.size(); a++) {
      TFile *weightFile(nullptr);
      TH2D *weightHist(nullptr);
      if(!weightfilename.IsNull()) {
         weightFile = TFile::Open(weightfilename,"READ");
         if (!weightFile->IsOpen()) { cout<<"Can't open "<<weightfilename<<endl; }
         cout << "Getting the weight histogram all_ ... " << flush; 
         weightHist = (TH2D*)weightFile->Get((algs[a]+"/all_").c_str());
         if(weightHist==nullptr) { cout<<"FAIL!"<<endl<<"Histogram of weights named \"all_\" was not in file "<<weightfilename<<endl; return 0; } 
         cout << "DONE" << endl;
      }
   
      JetInfo jetInfo(TString(algs[a]));

      //
      // setup the tree for reading
      //
      int file_count(0);
      TChain* chain;
      if(!inputFilename.empty() && inputFilePath.empty()) {
         TFile *inf = TFile::Open(inputFilename.c_str());
         TDirectoryFile *idir = (TDirectoryFile*)inf->Get(algs[a].c_str());
         if (idir) 
            cout << "The directory is " << idir->GetName() << endl;
         else {
            cout << "ERROR::Directory " << algs[a] <<" could not be found in file " << inf->GetName() << endl;
            cout << " SKIPPING ALGO " << algs[a] << endl;
            continue;
         }
         chain = (TChain*)idir->Get("t");
         file_count = 1;
      }
      else if(!fileList.empty()) {
         cout<<"\tAdding files from the list " << inputFilePath << "/" << fileList<<endl;
         chain = new TChain((algs[a]+"/t").c_str());
         TFileCollection fc("fc","",(inputFilePath+"/"+fileList).c_str());
         chain->AddFileInfoList((TCollection*)fc.GetList());
         if(chain->GetListOfFiles()->GetEntries()!=fc.GetNFiles()) {
            cout << "ERROR::DelphesNtupleToJRANtuple_x::main Something went wrong and the number of files in the filesList doesn't equal the number of files in the chain." << endl;
            return -1;           
         }
         file_count = chain->GetListOfFiles()->GetEntries();
      }
      #if(has_xrdcl)
         else if(!url_string.empty()) {
            chain = new TChain((algs[a]+"/t").c_str());
            XrdCl::DirectoryList *response;
            XrdCl::DirListFlags::Flags flags = XrdCl::DirListFlags::None;
            XrdCl::URL url(url_string);
            XrdCl::FileSystem fs(url);
            fs.DirList(inputFilePath,flags,response);
            for(auto iresp=response->Begin(); iresp!=response->End(); iresp++) {
               if((*iresp)->GetName().find(".root")!=std::string::npos) {
                  cout << "\tAdding " << url_string << inputFilePath << (*iresp)->GetName() << endl;
                  file_count = chain->Add((url_string+inputFilePath+(*iresp)->GetName()).c_str());
               }
            }
         }
      #endif
      else {
         cout<<"\tAdding "<<inputFilePath+"/"+inputFilename+"*.root"<<endl;
         chain = new TChain((algs[a]+"/t").c_str());
         file_count = chain->Add((inputFilePath+"/"+inputFilename+"*.root").c_str());
      }
      if (file_count==0){
         cout << "\tNo files found!  Aborting.\n";
         return 0;
      }
      if (0==chain) { cout<<"no tree/chain found."<<endl; continue; }
      JRAEvent* JRAEvt = new JRAEvent(chain,85);
      chain->SetBranchStatus("*",0);
      vector<string> branch_names = {"nref","refpt","refeta","jtpt","jteta","jtphi","jtarea",
                                     "bxns","npus","tnpus","sumpt_lowpt","refdrjt",
                                     "refpdgid","npv","rho","rho_hlt","pthat","weight"};
      for(auto n : branch_names) {
         if(!doflavor && n=="refpdgid") continue;
         if(n=="rho_hlt" && 0==chain->GetBranch("rho_hlt")) continue;
         if(n=="weight") {
            if (xsection>0.0) { 
                useweight = false;
            }
            if (useweight) {
                if (0==chain->GetBranch(n.c_str()))
                    cout<<"branch 'weight' not found, events will NOT be weighted!"<<endl;
                else
                    chain->SetBranchStatus(n.c_str(),1);
            }
            continue;
         }
         chain->SetBranchStatus(n.c_str(),1);
      }

      //
      // move to the output directory
      //
      TDirectoryFile* odir = (TDirectoryFile*)outf->mkdir(algs[a].c_str());
      odir->cd();
  
      int j,k;
      char name[1024];

      TH1F *pThatDistribution(nullptr);
      vector<TH2F*> RelRspVsRefPt;
      TH2F *RelRspVsJetEta[NPtBins];
      TH3F *RespVsEtaVsPt(nullptr);
      TH3F *ScaleVsEtaVsPt(nullptr);
      TProfile *RelRspVsSumPt(nullptr);
      TH1F *SumPtDistributions[NPileup/2];
      TH1F *ResolutionVsEta[NPtBins];
      TH1F *ResolutionVsPt(nullptr);
      TH2D *EtaVsPt(nullptr);
      TH1F *RefEtaDistribution(nullptr);
      TH1F *EtaDistribution(nullptr);
      TH1F *iEtaDistribution(nullptr);
      TH1F *EtaDistributionPU0(nullptr);
      TH1F *EtaDistributionPU[10];
      TH1F *ThetaDistribution(nullptr);
      TH1F *SolidAngleDist(nullptr);
      TH1F *HigherDist(nullptr);
      TH1F *MiddleDist(nullptr);
      TH1F *LowerDist(nullptr);
      TH1F *RelContributions[NPtBins];
      TProfile *rhoVsRhoHLT(nullptr);
      TProfile *npvVsRhoHLT(nullptr);
      TH1F *TPUDistribution(nullptr);
      TProfile *DPtVsNPU[3];
      TProfile *DPtVsPtGen[3];
      TProfile *RespRatioVsPtGen[3];
      TProfile *ErrorForNPU[3];
      TProfile *ErrorForPtGen[3];
      TProfile *Error2ForPtGen[3];
      TProfileMDF *RespVsPileup(nullptr); //For pileup studies (fully implemented)
      vector<Double_t> coord;
      TProfileMDF *RhoVsPileupVsEta(nullptr);
      vector<Double_t> coord2;
      TProfile3D *RespVsRho(nullptr); //For Mikko and Ricardo to create Calo and PF HLT L1 files
      TProfile2D *OffVsRhoVsEta(nullptr); //For Ricardo to create Calo and PF HLT L1 files
      TProfile2D *RhoVsOffETVsEta(nullptr);
      TProfile2D *RhoVsOffITVsEta(nullptr);
      TProfile2D *RhoVsOffLTVsEta(nullptr);
      TProfile2D *RespVsEtaVsPtProfile(nullptr);
      TProfile *RespVsPtProfile(nullptr);

      //
      // Get the corrections from the text files
      //
      bool exclude(false);
      for (unsigned int i=0;i<levels.size();i++) {
         stringstream sslvl; sslvl<<"l"<<levels[i];
         if (algs[a].find(sslvl.str())!=string::npos) exclude=true;
      }
      if (exclude) {
         cout<<"exclude "<<algs[a]<<endl;
         continue;
      }
      cout<<"jet algorithm: "<<algs[a]<<endl;
      cout<<"correction level: "<<JetInfo::get_correction_levels(levels,L1FastJet)<<endl;
      cout<<"correction tag: "<<JetInfo::get_correction_tags(era,algs[a],levels,path,L1FastJet)<<endl;

      cout << "Setting up the FactorizedJetCorrector ... " << flush;
      FactorizedJetCorrector *JetCorrector;
      if(levels.size()>0 && useTags) {
         JetCorrector = new FactorizedJetCorrector(JetInfo::get_correction_levels(levels,L1FastJet),
                                                   JetInfo::get_correction_tags(era,algs[a],levels,path,L1FastJet));
      }
      else if(levels.size()>0) {
         //
         // Make sure the levels are in the correct order (lowest level to highest)
         //
         sort (levels.begin(),levels.end());
         vector<JetCorrectorParameters> vPar;
         for(unsigned int ilevel=0; ilevel<levels.size(); ilevel++) {
            vPar.push_back(JetCorrectorParameters(string(path + era + JetInfo::get_level_tag(levels[ilevel],L1FastJet) + 
                                                         jetInfo.getAlias() + getPostfix(postfix,algs[a],levels[ilevel]) + ".txt")));
         }
         JetCorrector = new FactorizedJetCorrector(vPar);
      }
      else {
         JetCorrector = nullptr;
      }
      cout << "DONE" << endl;

      //
      // book histograms
      //
      pThatDistribution = new TH1F("pThat","pThat",(int)vpt[NPtBins]/10.0,vpt[0],vpt[NPtBins]);
      pThatDistribution->Sumw2();
      for(int ieta=0; ieta<NETA_Coarse; ieta++) {
         if(veta_coarse[ieta]<0) continue;
         else {
            TString hname = Form("RelRspVsRefPt_JetEta%sto%s",eta_boundaries_coarse[ieta],eta_boundaries_coarse[ieta+1]);
            RelRspVsRefPt.push_back(new TH2F(hname,hname,NPtBins,vpt,nbinsrelrsp,relrspmin,relrspmax));
            RelRspVsRefPt.back()->Sumw2();
         }
      }
      RespVsEtaVsPt = new TH3F("RespVsEtaVsPt","RespVsEtaVsPt",NPtBins,vpt,NETA,veta,nbinsrelrsp,vresp);
      RespVsEtaVsPt->Sumw2();
      ScaleVsEtaVsPt = new TH3F("ScaleVsEtaVsPt","ScaleVsEtaVsPt",NPtBins,vpt,NETA,veta,nbinsrelrsp,vcorr);
      ScaleVsEtaVsPt->Sumw2();  
      if(!reduceHistograms) {
         RespVsPtProfile = new TProfile("RespVsPtProfile","RespVsPtProfile",NPtBins,vpt);
         RespVsPtProfile->Sumw2();
         RespVsPtProfile->SetErrorOption("s");
         HigherDist = new TH1F("HigherDist","HigherDist",1999,1,2000);
         MiddleDist = new TH1F("MiddleDist","MiddleDist",1999,1,2000);
         LowerDist = new TH1F("LowerDist","LowerDist",1999,1,2000);              
         RespVsEtaVsPtProfile = new TProfile2D("RespVsEtaVsPtProfile","RespVsEtaVsPtProfile",NPtBins,vpt,NETA,veta);
         RespVsEtaVsPtProfile->Sumw2();
         RespVsEtaVsPtProfile->SetErrorOption("s");
         RelRspVsSumPt = new TProfile("RelRspVsSumPt","RelRspVsSumPt",NPtBins,vpt);
         RelRspVsSumPt->Sumw2();
         ResolutionVsPt = new TH1F("ResolutionVsPt","ResolutionVsPt",NPtBins,vpt);
         ResolutionVsPt->Sumw2();
         iEtaDistribution  = new TH1F("iEtaDistribution"   ,"iEtaDistribution",NETA,veta);
         iEtaDistribution->Sumw2();
         RefEtaDistribution  = new TH1F("RefEtaDistribution"   ,"RefEtaDistribution",220, -5.5, 5.5);
         RefEtaDistribution  ->Sumw2();
         EtaDistribution     = new TH1F("EtaDistribution"   ,"EtaDistribution",220, -5.5, 5.5);
         EtaDistribution     ->Sumw2(); 
         EtaDistributionPU0  = new TH1F("EtaDistributionPU0","EtaDistributionPU0",220, -5.5, 5.5);
         EtaDistributionPU0  ->Sumw2(); 
         EtaVsPt  = new TH2D("EtaVsPt","EtaVsPt",220, -5.5, 5.5,170,1,4);
         EtaVsPt  ->Sumw2();
         for (int i=0 ; i<50 ; i += 5 ){
            stringstream ss;
            ss<<"EtaDistributionPU"<<i<<"_"<<i+4;
            EtaDistributionPU[int(i/5)]  = new TH1F(ss.str().c_str(),ss.str().c_str(),240,-5.5,5.5);
            EtaDistributionPU[int(i/5)] -> Sumw2();
         }
         ThetaDistribution = new TH1F("ThetaDistribution","ThetaDistribution",100, 0, TMath::Pi());
         //
         // dN/dOmega = dN/2*Pi*d(cos(theta))
         //
         SolidAngleDist    = new TH1F("SolidAngleDist","SolidAngleDist",200, -2*TMath::Pi(),2*TMath::Pi());
      }
      for(int i=0;i<NPtBins;i++)
      {
         sprintf(name,"RelRspVsJetEta_RefPt%sto%s",Pt[i],Pt[i+1]);
         RelRspVsJetEta[i] = new TH2F(name,name,NETA,veta,nbinsrelrsp,relrspmin,relrspmax);
         if(!reduceHistograms) {
            sprintf(name,"RelContributions_RefPt%sto%s",Pt[i],Pt[i+1]);
            RelContributions[i] = new TH1F(name,name,1999,1,2000);
            sprintf(name,"ResolutionVsEta_RefPt%sto%s",Pt[i],Pt[i+1]);
            ResolutionVsEta[i] = new TH1F(name,name,NETA,veta);
            ResolutionVsEta[i]->Sumw2();
         }
      }//for(int i=0;i<NPtBins;i++)
      if(!reduceHistograms && doTProfileMDF && readRespVsPileup.IsNull())
      {
         const double vpt_Coarse[16] = {15, 20, 25, 30, 35, 40, 50, 70, 90, 120, 150, 200, 300, 400, 600, 1000};
         RespVsPileup = new TProfileMDF("RespVsPileup","RespVsPileup");
         RespVsPileup->AddAxis("pt",15,vpt_Coarse);
         RespVsPileup->AddAxis("eta",NETA,veta);
         //RespVsPileup->AddAxis("rho",15,0,20);
         RespVsPileup->AddAxis("EOOT",26,0,26);
         RespVsPileup->AddAxis("IT",26,0,26);
         RespVsPileup->AddAxis("LOOT",26,0,26);
         RespVsPileup->Sumw2();
         coord.assign(RespVsPileup->GetNaxis(),0);

         RespVsRho = new TProfile3D("RespVsRhoVsEtaVsPt","RespVsRhoVsEtaVsPt",NPtBins,vpt,NETA_HLT,veta_HLT,NRhoBins,vrho);
         RespVsRho->Sumw2();

         RhoVsPileupVsEta = new TProfileMDF("RhoVsPileupVsEta","RhoVsPileupVsEta");
         RhoVsPileupVsEta->AddAxis("eta",NETA,veta);
         RhoVsPileupVsEta->AddAxis("EOOT",26,0,26);
         RhoVsPileupVsEta->AddAxis("IT",26,0,26);
         RhoVsPileupVsEta->AddAxis("LOOT",26,0,26);
         RhoVsPileupVsEta->Sumw2();
         coord2.assign(RhoVsPileupVsEta->GetNaxis(),0);
      }
      else if(!reduceHistograms && doTProfileMDF)
      {
         RhoVsPileupVsEta = new TProfileMDF("RhoVsPileupVsEta","RhoVsPileupVsEta");
         RespVsPileup = new TProfileMDF("RespVsPileup","RespVsPileup");
         RespVsPileup->ReadFromFile(readRespVsPileup,"RespVsPileup");
         coord.assign(RespVsPileup->GetNaxis(),0);
         
         TFile tempin(readRespVsPileup);
         tempin.cd();
         RespVsRho = (TProfile3D*)gDirectory->Get("RespVsRhoVsEtaVsPt");
         if (!RespVsRho) {
            cout << "ERROR::Could not retrieve RespVsRhoVsEtaVsPt" << endl
                 << "Ending program !!" << endl;
            return 0;
         }
         RespVsRho->SetDirectory(0);
      }
      odir->cd();
      if(!reduceHistograms) {
         OffVsRhoVsEta = new TProfile2D("OffVsRhoVsEta","OffVsRhoVsEta",26,0,26,NETA,veta);
         OffVsRhoVsEta->Sumw2();
         RhoVsOffETVsEta = new TProfile2D("RhoVsOffETVsEta","RhoVsOffETVsEta",100,0,50,NETA,veta);
         RhoVsOffETVsEta->Sumw2();
         RhoVsOffITVsEta = new TProfile2D("RhoVsOffITVsEta","RhoVsOffITVsEta",100,0,50,NETA,veta);
         RhoVsOffITVsEta->Sumw2();
         RhoVsOffLTVsEta = new TProfile2D("RhoVsOffLTVsEta","RhoVsOffLTVsEta",100,0,50,NETA,veta);
         RhoVsOffLTVsEta->Sumw2();
         rhoVsRhoHLT = new TProfile("rhoVsRhoHLT","rhoVsRhoHLT",1000,0,100);
         rhoVsRhoHLT->Sumw2();
         rhoVsRhoHLT->GetXaxis()->SetTitle("Rho^{HLT}");
         rhoVsRhoHLT->GetYaxis()->SetTitle("Rho^{RECO}");
         npvVsRhoHLT = new TProfile("npvVsRhoHLT","npvVsRhoHLT",100,0,100);
         npvVsRhoHLT->Sumw2();
         npvVsRhoHLT->GetXaxis()->SetTitle("Rho^{HLT}");
         npvVsRhoHLT->GetYaxis()->SetTitle("NPV^{RECO}");
         for(unsigned int i=0; i<NPileup/2; i++)
         {
            sprintf(name,"SumPtDistribution_NPU%sto%s",pileup_boundaries[i*2],pileup_boundaries[(i*2)+1]);
            SumPtDistributions[i] = new TH1F(name,name,NPtBins,vpt);
         }//for(unsigned int i=0; i<NPileup/2; i++)
         for(int i=0; i<3; i++)
         {
            sprintf(name,"DPtVsNPU_%s",pusources[i]);
            DPtVsNPU[i] = new TProfile(name,name,26,0,26);
            DPtVsNPU[i]->Sumw2();
            sprintf(name,"DPtVsPtGen_%s",pusources[i]);
            DPtVsPtGen[i] = new TProfile(name,name,NPtBins,vpt);
            DPtVsPtGen[i]->Sumw2();
            sprintf(name,"RespRatioVsPtGen_%s",pusources[i]);
            RespRatioVsPtGen[i] = new TProfile(name,name,NPtBins,vpt);
            RespRatioVsPtGen[i]->Sumw2();
            sprintf(name,"ErrorForNPU_%s",pusources[i]);
            ErrorForNPU[i] = new TProfile(name,name,26,0,26);
            //ErrorForNPU[i]->SetDirectory(0);
            sprintf(name,"ErrorForPtGen_%s",pusources[i]);
            ErrorForPtGen[i] = new TProfile(name,name,NPtBins,vpt);
            //ErrorForPtGen[i]->SetDirectory(0);
            sprintf(name,"Error2ForPtGen_%s",pusources[i]);
            Error2ForPtGen[i] = new TProfile(name,name,NPtBins,vpt);
            //Error2ForPtGen[i]->SetDirectory(0);
         }//for(int i=0; i<3; i++)
         TPUDistribution = new TH1F("TPUDistribution","TPUDistribution",1000,0,100);
      }

      //
      // fill histograms
      //
      unsigned int nevt = (evtmax>0) ? evtmax : (unsigned int)chain->GetEntries();
      cout << "Jet Collection: " << algs[a] << " ...... Processing " << nevt << " of " << chain->GetEntries() << " entries:" << endl;
      int min_npu=100;
      for (unsigned int ievt=0;ievt<nevt;ievt++) {
         loadbar2(ievt+1,nevt,50,"\t");

         chain->GetEntry(ievt);

         int iIT = itIndex(JRAEvt->bxns);
         int npu = sumEOOT(JRAEvt->npus,iIT)+JRAEvt->npus->at(iIT)+sumLOOT(JRAEvt->npus,iIT);
         int eootnpu = (int)sumEOOT(JRAEvt->npus,iIT);
         int itnpu = JRAEvt->npus->at(iIT);
         int lootnpu = (int)sumLOOT(JRAEvt->npus,iIT);
         double sumpt = JRAEvt->sumpt_lowpt->at(1);
         float pthat = JRAEvt->pthat;
         float evt_fill = true;
         if (printnpu) cout<<" ievt = "<<ievt<<"\tnpu = "<<npu<<endl;
         if (npu<min_npu) min_npu = npu;

         if (!pileup_cut(itlow,ithigh,earlyootlow,earlyoothigh,lateootlow,lateoothigh,
                         totalootlow,totaloothigh,totallow,totalhigh,JRAEvt->npus,JRAEvt->bxns)) {
            cout << "WARNING::Failed the pileup cut." << endl << "Skipping this event." << endl;
            continue;
         }
         if (dphimin>0 && abs(JRAEvt->jtphi->at(0)-JRAEvt->jtphi->at(1))<dphimin) continue;
         if (pthatmin>0.0 && pthat<pthatmin) {
            if(verbose) cout << "WARNING::The pthat of this event is less than the minimum pthat!" << endl;
            continue;
         }
         if (pthatmax!=-1.0 && pthat>pthatmax) {
            if(verbose) cout << "WARNING::The pthat of this event is greater than the maximum pthat!" << endl;
            continue;
         }

         if(!reduceHistograms) {
            rhoVsRhoHLT->Fill(JRAEvt->rho_hlt,JRAEvt->rho);
            npvVsRhoHLT->Fill(JRAEvt->rho_hlt,JRAEvt->npv);
         }

         if(nrefmax>0 && JRAEvt->nref>nrefmax) JRAEvt->nref = nrefmax;
         for (unsigned char iref=0;iref<JRAEvt->nref;iref++) {
            float rho = JRAEvt->rho;
            float rho_hlt = (0!=chain->GetBranch("rho_hlt")) ? JRAEvt->rho_hlt : 0;
            float ptgen  = JRAEvt->refpt->at(iref);
            if (ptgen<ptgenmin) continue;
            if (doflavor && abs(pdgid)!=123 && abs(JRAEvt->refpdgid->at(iref))!=abs(pdgid)) continue;
            else if (doflavor && abs(pdgid)==123 && (abs(JRAEvt->refpdgid->at(iref))>2 || abs(JRAEvt->refpdgid->at(iref))==0)) continue;
            float eta    = JRAEvt->jteta->at(iref);
            if (etamax>0 && TMath::Abs(eta)>etamax) continue;
            float pt     = JRAEvt->jtpt->at(iref);
            if (pt > 14000) {
               cout << "WARNING::pt>14000 GeV (pt = " << pt << " GeV)." << endl << "Skipping this jet." << endl;
               continue;
            }
            float dr     = JRAEvt->refdrjt->at(iref);
            if (drmax.size()>0 && dr > drmax[a]) continue;
            if(JetCorrector) {
               JetCorrector->setJetPt(pt);
               JetCorrector->setJetEta(eta);
               if (TString(JetInfo::get_correction_levels(levels,L1FastJet)).Contains("L1FastJet")) {
                  if (JRAEvt->jtarea->at(iref)!=0)
                     JetCorrector->setJetA(JRAEvt->jtarea->at(iref));
                  else if (jetInfo.coneSize>0)
                     JetCorrector->setJetA(TMath::Pi()*TMath::Power(jetInfo.coneSize/10.0,2));
                  else {
                     cout << "WARNING::Unknown jet area. Skipping event." << endl;
                     continue;
                  }

                  if (jetInfo.isHLT())
                     JetCorrector->setRho(JRAEvt->rho_hlt);
                  else
                     JetCorrector->setRho(JRAEvt->rho);
               }
               if(!L1FastJet) JetCorrector->setNPV(JRAEvt->npv);
            }
            float scale = (JetCorrector) ? JetCorrector->getCorrection() : 1.0;

            //
            // we have to fill this histogram before we kill the event
            //
            ScaleVsEtaVsPt->Fill(ptgen,eta,scale);
            if (scale < 0) continue;
            if (pt<ptrawmin) continue;
            if ((pt*scale)<ptmin) continue;
            float relrsp = scale*JRAEvt->jtpt->at(iref)/JRAEvt->refpt->at(iref);
            float theta  = 2.0*atan(exp(-eta));
            double weight(1.0);

            if(xsection>0.0) weight = (xsection*luminosity)/nevt;
            if(useweight) weight = JRAEvt->weight;
            if(!(xsection>0.0) && !useweight) weight = 1.0;
            if(weightHist!=nullptr) weight *= weightHist->GetBinContent(weightHist->FindBin(ptgen,eta));
            if(!MCPUReWeighting.IsNull() && !DataPUReWeighting.IsNull()) {
               double LumiWeight = LumiWeights_.weight(JRAEvt->tnpus->at(iIT));
               weight *= LumiWeight;
            }
            if(pThatReweight!=-9999) weight*=pow(pthat/15.,pThatReweight);


            if(evt_fill) {pThatDistribution->Fill(pthat,weight); evt_fill=false;}
            //-4 to cut off the negative side of the detector
            if(fabs(eta)<veta_coarse[NETA_Coarse]) {
               if(debug && ievt>5400000) {
                  cout << "fabs(eta)="<< fabs(eta) << endl;
                  cout << "veta_coarse[NETA_Coarse]=" << veta_coarse[NETA_Coarse] << endl;
                  cout << "getBin(fabs(eta),veta_coarse,NETA_Coarse)-4=" << getBin(fabs(eta),veta_coarse,NETA_Coarse)-(NETA_Coarse/2) << endl;
               }
               RelRspVsRefPt[getBin(fabs(eta),veta_coarse,NETA_Coarse)-(NETA_Coarse/2)]->Fill(ptgen,relrsp,weight);
            }

            //if (fabs(eta)<=1.3)
            //{
            //   RespVsPt_Bar->Fill(ptgen,relrsp,weight);
            //}
            //if ((fabs(eta)<=3.0) && (fabs(eta)>1.3))
            //{
            //   RespVsPt_End->Fill(ptgen,relrsp,weight);
            //}
            //if ((fabs(eta)<=2.5) && (fabs(eta)>1.3))
            //{
            //   RespVsPt_IEnd->Fill(ptgen,relrsp,weight);
            //}
            //if ((fabs(eta)<=3.0) && (fabs(eta)>2.5))
            //{
            //   RespVsPt_OEnd->Fill(ptgen,relrsp,weight);
            //}
            //if ((fabs(eta)<=5.0) && (fabs(eta)>3))
            //{
            //   RespVsPt_Fwd->Fill(ptgen,relrsp,weight); 
            //}
            RespVsEtaVsPt->Fill(ptgen,eta,relrsp,weight);
            if(!reduceHistograms) {
               if(HigherDist->FindBin(scale*pt) < HigherDist->FindBin(ptgen)) HigherDist->Fill(scale*pt,weight);
               if(MiddleDist->FindBin(scale*pt) == MiddleDist->FindBin(ptgen)) MiddleDist->Fill(scale*pt,weight);
               if(LowerDist->FindBin(scale*pt) > LowerDist->FindBin(ptgen)) LowerDist->Fill(scale*pt,weight);
               RespVsEtaVsPtProfile->Fill(ptgen,eta,relrsp,weight);
               RespVsPtProfile->Fill(ptgen,relrsp,weight);
               EtaVsPt->Fill(eta, log10(pt*scale),weight);
               TPUDistribution->Fill(JRAEvt->tnpus->at(iIT),weight);
            }

            j = getBin(ptgen,vpt,NPtBins);
            k = getBin(eta,veta,NETA);
            if (j<NPtBins && j>=0 && k<NETA && k>=0)
            {
               RelRspVsJetEta[j]->Fill(eta,relrsp,weight);

               if(!reduceHistograms) {
                  RelContributions[j]->Fill(scale*pt,weight);

                  if(doTProfileMDF && readRespVsPileup.IsNull())
                  { 
                     coord[0] = ptgen;
                     coord[1] = eta;
                     /*
                     if(!algs[a].Contains("HLT"))
                        coord[2] = rho;
                     else
                        coord[2] = rho_hlt;
                     */
                     coord[2] = sumEOOT(JRAEvt->npus,iIT);
                     coord[3] = JRAEvt->npus->at(iIT);
                     coord[4] = sumLOOT(JRAEvt->npus,iIT);
                     RespVsPileup->Fill(coord,relrsp);
                  
                     if(!jetInfo.isHLT())
                        RespVsRho->Fill(ptgen,eta,rho,relrsp);
                     else
                        RespVsRho->Fill(ptgen,eta,rho_hlt,relrsp);

                     coord2[0] = eta;
                     coord2[1] = sumEOOT(JRAEvt->npus,iIT);
                     coord2[2] = JRAEvt->npus->at(iIT);
                     coord2[3] = sumLOOT(JRAEvt->npus,iIT);
                     if(!jetInfo.isHLT())
                        RhoVsPileupVsEta->Fill(coord2,rho);
                     else
                        RhoVsPileupVsEta->Fill(coord2,rho_hlt);
                  }
                  else if(doTProfileMDF)
                  {
                     coord[0] = ptgen;
                     coord[1] = eta;
                     /*
                     if(!algs[a].Contains("HLT"))
                        coord[2] = rho;
                     else
                        coord[2] = rho_hlt;
                     */
                     coord[2] = 5;
                     coord[3] = JRAEvt->npus->at(iIT);
                     coord[4] = sumLOOT(JRAEvt->npus,iIT);
                     double resp_EOOT = RespVsPileup->GetBinContent(RespVsPileup->FindBin(coord));
                     double eresp_EOOT = RespVsPileup->GetBinError(RespVsPileup->FindBin(coord));

                     coord[2] = sumEOOT(JRAEvt->npus,iIT);
                     coord[3] = 5;
                     coord[4] = sumLOOT(JRAEvt->npus,iIT);
                     double resp_IT   = RespVsPileup->GetBinContent(RespVsPileup->FindBin(coord));
                     double eresp_IT = RespVsPileup->GetBinError(RespVsPileup->FindBin(coord));

                     coord[2] = sumEOOT(JRAEvt->npus,iIT);
                     coord[3] = JRAEvt->npus->at(iIT);
                     coord[4] = 5;
                     double resp_LOOT = RespVsPileup->GetBinContent(RespVsPileup->FindBin(coord));
                     double eresp_LOOT = RespVsPileup->GetBinError(RespVsPileup->FindBin(coord));

                     double resp_rho = RespVsRho->GetBinContent(RespVsRho->FindBin(ptgen,eta,1));

                     //
                     // Psi = {RelRsp[p_T^GEN,EOOT,IT,LOOT]-RelRsp[p_T^GEN,EOOT,IT,LOOT]}*p_T^GEN
                     // where either EOOT,IT, or LOOT are 5 for the second RelRsp. The first RelRsp
                     // is the relative response of the current jet.
                     //
                     double Psi_EOOT = (relrsp-resp_EOOT)*ptgen;
                     double ePsi_EOOT = eresp_EOOT*ptgen;
                     double Psi_IT = (relrsp-resp_IT)*ptgen;
                     double ePsi_IT = eresp_IT*ptgen;
                     double Psi_LOOT = (relrsp-resp_LOOT)*ptgen;
                     double ePsi_LOOT = eresp_LOOT*ptgen;
   
                     double off_rho = (relrsp-resp_rho)*ptgen;
   
                     //
                     // PsiPrime = RelRsp[p_T^GEN,EOOT,IT,LOOT]/RelRsp[p_T^GEN,EOOT,IT,LOOT]
                     // where either EOOT,IT, or LOOT are 5 for the RelRsp in the denominator.
                     // The RelRsp in the numerator is the relative response of the current jet.
                     //
                     double PsiPrime_EOOT = relrsp/resp_EOOT;
                     double ePsiPrime_EOOT = (PsiPrime_EOOT*eresp_EOOT)/resp_EOOT;
                     double PsiPrime_IT = relrsp/resp_IT;
                     double ePsiPrime_IT = (PsiPrime_IT*eresp_IT)/resp_IT;
                     double PsiPrime_LOOT = relrsp/resp_LOOT;
                     double ePsiPrime_LOOT = (PsiPrime_LOOT*eresp_LOOT)/resp_LOOT;
   
                     if(resp_EOOT!=0)
                     {
                        DPtVsNPU[0]->Fill(eootnpu,Psi_EOOT);
                        DPtVsPtGen[0]->Fill(ptgen,Psi_EOOT);
                        RespRatioVsPtGen[0]->Fill(ptgen,PsiPrime_EOOT);
                        ErrorForNPU[0]->Fill(eootnpu,ePsi_EOOT);
                        ErrorForPtGen[0]->Fill(ptgen,ePsi_EOOT);
                        Error2ForPtGen[0]->Fill(ptgen,ePsiPrime_EOOT);
   
                        RhoVsOffETVsEta->Fill(eootnpu,eta,rho_hlt);
                     }
                     if(resp_IT!=0)
                     {
                        DPtVsNPU[1]->Fill(itnpu,Psi_IT); 
                        DPtVsPtGen[1]->Fill(ptgen,Psi_IT);
                        RespRatioVsPtGen[1]->Fill(ptgen,PsiPrime_IT);
                        ErrorForNPU[1]->Fill(itnpu,ePsi_IT);
                        ErrorForPtGen[1]->Fill(ptgen,ePsi_IT);
                        Error2ForPtGen[1]->Fill(ptgen,ePsiPrime_IT);
   
                        RhoVsOffITVsEta->Fill(itnpu,eta,rho_hlt);
                     } 
                     if(resp_LOOT!=0)
                     {
                        DPtVsNPU[2]->Fill(lootnpu,Psi_LOOT);
                        DPtVsPtGen[2]->Fill(ptgen,Psi_LOOT);
                        RespRatioVsPtGen[2]->Fill(ptgen,PsiPrime_LOOT);
                        ErrorForNPU[2]->Fill(lootnpu,ePsi_LOOT);
                        ErrorForPtGen[2]->Fill(ptgen,ePsi_LOOT);
                        Error2ForPtGen[2]->Fill(ptgen,ePsiPrime_LOOT);
   
                        RhoVsOffLTVsEta->Fill(lootnpu,eta,rho_hlt);
                     }
   
                     OffVsRhoVsEta->Fill(rho_hlt,eta,off_rho);
                  }//if(!readRespVsPileup.IsNull())
               }//if(!reduceHistograms)
            }//if (j<NPtBins && j>=0)

            if(!reduceHistograms) {
               RelRspVsSumPt->Fill(sumpt,relrsp);
               for(int spd=0; spd<NPileup/2; spd++)
               {
                  if(itnpu>=vpileup[spd*2] && itnpu<=vpileup[(spd*2)+1])
                     SumPtDistributions[spd]->Fill(sumpt);
               }
               RefEtaDistribution->Fill(JRAEvt->refeta->at(iref));
               EtaDistribution->Fill(eta);
               iEtaDistribution->Fill(eta);
               //
               // These bins correspont to 0-4, 5-9, 10-14, 15-19, 20-24, 25-29, 30-34, 35-39, 40-44, 45-infinity
               //
               int in_npu = npu/5;
               if (in_npu > 9) in_npu = 9;
                 
               EtaDistributionPU[in_npu]->Fill(eta);
               if (npu==0) EtaDistributionPU0->Fill(eta);
               ThetaDistribution ->Fill(theta);
               SolidAngleDist ->Fill(2*TMath::Pi()*cos(theta));
            }
         }//for (unsigned char iref=0;iref<nrefmax;iref++) 
      }//for (unsigned int ievt=0;ievt<nevt;ievt++)

      //
      // make histograms that rely on other, completely filled, histograms
      //
      if(!reduceHistograms) {
         for (int i=1; i<=NPtBins; i++) {
            makeResolutionHistogram(RespVsEtaVsPt,ResolutionVsEta[i-1],"y",mpv,i,i);
         }
         makeResolutionHistogram(RespVsEtaVsPt,ResolutionVsPt,"x",mpv);
      }

      /*if(!readRespVsPileup.IsNull()) {
        for(int i=0; i<3; i++) {
        addErrorQuadrature(DPtVsNPU[i],ErrorForNPU[i]);
        addErrorQuadrature(DPtVsPtGen[i],ErrorForPtGen[i]);
        addErrorQuadrature(RespRatioVsPtGen[i],Error2ForPtGen[i]);
        }
        }*/

      //
      // final cout statements
      //
      cout << endl << " min_npu="<<min_npu<<endl;

      //
      // close files
      //
      if(!reduceHistograms && doTProfileMDF && readRespVsPileup.IsNull())
      {
         cout << "Write " << "RespVsPileup_" << algs[a] << ".root" << " ... ";
         RespVsPileup->WriteToFile(outputDir+"RespVsPileup_"+jetInfo.alias+".root");
         TFile tempout(outputDir+"RespVsPileup_"+jetInfo.alias+".root","UPDATE");
         tempout.cd();
         RespVsRho->Write();
         tempout.Close();
         RhoVsPileupVsEta->WriteToFile(outputDir+"RhoVsPileupVsEta_"+jetInfo.alias+".root");
         cout << "DONE" << endl;
      }

      delete chain;
   }//for(unsigned int a=0; a<algs.size(); a++)

   cout << "Write " << "Closure.root" << " ... ";
   outf->cd();
   outf->Write();
   cout << "DONE" << endl;
   outf->Close();

   m_benchmark->Stop("event"); 
   cout << "jet_correction_analyzer_x" << endl << "\tCPU time = " << m_benchmark->GetCpuTime("event") << " s" << endl
        << "\tReal time = " << m_benchmark->GetRealTime("event") << " s" << endl;
   delete m_benchmark;

   return 0;
}

////////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int getBin(double x, const double boundaries[], int length)
{
   int i;
   int n = length;
   if (n<=0) return -1;
   if (x<boundaries[0] || x>=boundaries[n])
      return -1;
   for(i=0;i<n;i++)
   {
      if (x>=boundaries[i] && x<boundaries[i+1])
         return i;
   }
   return 0; 
}

//______________________________________________________________________________
string get_flavor_name(int pdgid)
{
   string result;
   int abspdgid = abs(pdgid);
   if      (abspdgid==1 || abspdgid==2) result = "qJ";
   else if (abspdgid==123)              result = "qJ"; 
   else if (abspdgid==3)                result = "sJ";
   else if (abspdgid==4)                result = "cJ";
   else if (abspdgid==5)                result = "bJ";
   else if (abspdgid==21)               result = "gJ";
   else if (abspdgid==9999)             result = "aJ";
   else {
      cout << "***ERROR***get_flavor_name::flavor for PDGID="<<pdgid<<" is not known"<<endl;
   } 
   return result;
}

//______________________________________________________________________________
void makeResolutionHistogram(TH3F* RespVs_, TH1F* ResolutionVs_, TString slice, bool mpv, int slice_min, int slice_max)
{
   int bins = 0;
   if(slice.CompareTo("y")==0) bins = RespVs_->GetNbinsY();
   else if(slice.CompareTo("x")==0) bins = RespVs_->GetNbinsX();
   else
   {
      cout << " WARNING::Unknown designation for option \"slice\"" << endl
           << " Returning NULL histogram." << endl;
      return;
   }
   for(int i=1; i<=bins; i++)
   {
      double res = 0;
      double reserr = 0;
      double mean = 0;
      double meanerr = 0;
      double error = 0;
      TH1D *h = 0;
      int oldLevel = gErrorIgnoreLevel;

      gErrorIgnoreLevel = kError;
      if(slice.CompareTo("y")==0)
         h = RespVs_->ProjectionZ("_pz",slice_min,slice_max,i,i);
      else if(slice.CompareTo("x")==0)
         h = RespVs_->ProjectionZ("_pz",i,i,slice_min,slice_max);
      gErrorIgnoreLevel = oldLevel;

      if(mpv)
      {
         h->Fit("gaus","S");
         TF1 *f = (TF1*)h->GetListOfFunctions()->Last();
         if(!f==0)
         {
            res = f->GetParameter(2);
            reserr = f->GetParError(2);
            mean = f->GetParameter(1);
            if(mean==0) continue;
            meanerr = f->GetParError(1);
            ResolutionVs_->SetBinContent(i,res/mean);
            error = (TMath::Power(1/mean,2)*TMath::Power(reserr,2))+
               (TMath::Power(-res/TMath::Power(mean,2),2)*TMath::Power(meanerr,2));
            error = TMath::Sqrt(error);
            ResolutionVs_->SetBinError(i,error);
         }
      }
      else
      {
         res = h->GetRMS();
         reserr = h->GetRMSError();
         mean = h->GetMean();
         if(mean==0) continue;
         meanerr = h->GetMeanError();
         ResolutionVs_->SetBinContent(i,res/mean);
         error = (TMath::Power(1/mean,2)*TMath::Power(reserr,2))+
            (TMath::Power(-res/TMath::Power(mean,2),2)*TMath::Power(meanerr,2));
         error = TMath::Sqrt(error);
         ResolutionVs_->SetBinError(i,error);
      }
   }
}

//______________________________________________________________________________
void addErrorQuadrature(TProfile* hist, TProfile* ehist)
{
   if (hist->GetNbinsX() != ehist->GetNbinsX()) {
      cout << "WARNING::TProfile* hist and TProfile* ehist do not have the same number of bins in x-axis." << endl
           << "         The errors cannot be added in quadrature." << endl;
      return;
   }
   
   for (int i=1; i<=hist->GetNbinsX(); i++) {
      double err_hist = hist->GetBinError(i);
      double err_ehist = ehist->GetBinContent(i);
      double enew = TMath::Sqrt(TMath::Power(err_hist,2)+TMath::Power(err_ehist,2))*hist->GetBinEntries(i);
      hist->SetBinError(i,enew);
   }
}

//______________________________________________________________________________
bool it_pileup(int itlow, int ithigh, vector<int>* npus, int iIT)
{
   if((*npus)[iIT]>=itlow && (*npus)[iIT]<=ithigh) return true;
   return false;
}


//______________________________________________________________________________
bool oot_pileup(int earlyootlow, int earlyoothigh, int lateootlow, int lateoothigh,
                vector<int>* npus, int iIT)
{
   if(sumEOOT(npus,iIT)>=earlyootlow && sumEOOT(npus,iIT)<=earlyoothigh && 
      sumLOOT(npus,iIT)>=lateootlow && sumLOOT(npus,iIT)<=lateoothigh) return true;
   return false;
}


//______________________________________________________________________________
bool total_oot_pileup(int totalootlow, int totaloothigh, vector<int>* npus, int iIT)
{
   double sumOOT = sumEOOT(npus,iIT) + sumLOOT(npus,iIT);
   if(sumOOT>=totalootlow && sumOOT<=totaloothigh) return true;
   return false;
}

//______________________________________________________________________________
bool total_pileup(int totallow, int totalhigh, vector<int>* npus, int iIT)
{
   double PU = sumEOOT(npus,iIT) + (*npus)[iIT] + sumLOOT(npus,iIT);
   if(PU>=totallow && PU<=totalhigh) return true;
   return false;
}

//______________________________________________________________________________
bool pileup_cut(int itlow, int ithigh, int earlyootlow, int earlyoothigh, 
                int lateootlow, int lateoothigh, int totalootlow, int totaloothigh, 
                int totallow, int totalhigh, vector<int>* npus, vector<int>* bxns)
{
   int iIT = itIndex(bxns);
   if(it_pileup(itlow,ithigh,npus,iIT) && 
      total_oot_pileup(totalootlow,totaloothigh,npus,iIT) && 
      oot_pileup(earlyootlow,earlyoothigh,lateootlow,lateoothigh,npus,iIT) &&
      total_pileup(totallow,totalhigh,npus,iIT)) return true;
   return false;
}

//______________________________________________________________________________
int itIndex(vector<int>* bxns) {
   for(unsigned int ibx=0; ibx<(*bxns).size(); ibx++) {
      if((*bxns)[ibx]==0) return ibx;
   }
   return -1;
}

//______________________________________________________________________________
double sumEOOT(vector<int>* npus, unsigned int iIT) {
   if(iIT>(*npus).size()-1) return 0;
   double sum = 0;
   for(unsigned int ipu=0; ipu<iIT; ipu++) {
      sum+=(*npus)[ipu];
   }
   return sum;
}

//______________________________________________________________________________
double sumLOOT(vector<int>* npus, unsigned int iIT) {
   if(iIT>(*npus).size()-1) return 0;
   double sum = 0;
   for(unsigned int ipu=(*npus).size()-1; ipu>iIT; ipu--) {
      sum+=(*npus)[ipu];
   }
   return sum;
}

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

/*
  TO DO::FOR FLAVOR ANALYSIES
  -give list of pdgids
  -create folders for each pdgid on list
  -check one pdgid at a time
  -map pdgid==1 || pdgid==2 || pdgid==3 to light quarks
*/

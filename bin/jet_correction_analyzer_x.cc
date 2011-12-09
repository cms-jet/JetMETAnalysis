///////////////////////////////////////////////////////////////////
//
// jet_correction_analyzer_x
// -------------------------
//
//            12/08/2011 Alexx Perloff  <aperloff@physics.tamu.edu>
///////////////////////////////////////////////////////////////////

#include "JetMETAnalysis/JetAnalyzers/interface/Settings.h"
#include "JetMETAnalysis/JetAnalyzers/interface/VectorWrapper.h"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"

#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"

#include "TROOT.h"
#include "TSystem.h"
#include <TObjectTable.h>
#include "TFile.h"
#include "TTree.h"
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
#include "TProfile3D.h"

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
int getBin(double x, const double boundaries[NPtBins]);

/// get the uppercase version of the algorithm name
string getAlias(TString s);

/// make the resolution vs. eta histogram
//void makeResolutionHistogram(TH2F* RelRspVs_, TH1F* ResolutionVs_, bool mpv);
void makeResolutionHistogram(TH3F* RespVs_, TH1F* ResolutionVs_, TString slice, bool mpv);

/// check the amount of IT pileup and see if it is in the specified range
bool it_pileup(int itlow, int ithigh, vector<int>* npus);

/// check the amount of OOT pileup before nad after the event and see if it is in the specified range
bool oot_pileup(int earlyootlow, int earlyoothigh, int lateootlow, int lateoothigh,
                vector<int>* npus);

/// check the sum of the OOT pileup before and after the event and see if it is in the specified range
bool total_oot_pileup(int totalootlow, int totaloothigh, vector<int>* npus);

/// check the sum of all of the pileup in the event and see if it is in the specified range
bool total_pileup(int totallow, int totalhigh, vector<int>* npus);

/// combines the booleans from the IT, OOT, and TotalOOT functions into one boolean
bool pileup_cut(int itlow, int ithigh, int earlyootlow, int earlyoothigh, 
                int lateootlow, int lateoothigh, int totalootlow, int totaloothigh, 
                int totallow, int totalhigh, vector<int>* npus);

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv)
{
  gROOT->ProcessLine("#include<vector>");
  gSystem->Load("libFWCoreFWLite.so");
  
  //
  // evaluate command-line / configuration file options
  // 
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;

  TString         inputFilename     = cl.getValue<TString>      ("inputFilename");
  vector<TString> algs              = cl.getVector<TString>     ("algs");
  string          path              = cl.getValue<string>       ("path");
  string          era               = cl.getValue<string>       ("era");
  TString         outputDir         = cl.getValue<TString>      ("outputDir",            "");
  bool            useL1Cor          = cl.getValue<bool>         ("useL1Cor",          false);
  bool            useL2Cor          = cl.getValue<bool>         ("useL2Cor",           true);
  bool            useL3Cor          = cl.getValue<bool>         ("useL3Cor",          false);
  bool            useL2L3ResCor     = cl.getValue<bool>         ("useL2L3ResCor",     false);
  bool            doflavor          = cl.getValue<bool>         ("doflavor",          false);
  int             pdgid             = cl.getValue<int>          ("pdgid",                 0);
  double          drmax             = cl.getValue<double>       ("drmax",                 0);
  double          ptmin             = cl.getValue<double>       ("ptmin",                 0);
  double          ptgenmin          = cl.getValue<double>       ("ptgenmin",              0);
  double          etamax            = cl.getValue<double>       ("etamax",                0);
  double          dphimin           = cl.getValue<double>       ("dphimin",               0);
  unsigned int    evtmax            = cl.getValue<unsigned int> ("evtmax",                0);
  bool            printnpu          = cl.getValue<bool>         ("printnpu",          false);
  int             itlow             = cl.getValue<int>          ("itlow",                 0);
  int             ithigh            = cl.getValue<int>          ("ithigh",             1000);
  int             earlyootlow       = cl.getValue<int>          ("earlyootlow",           0);
  int             earlyoothigh      = cl.getValue<int>          ("earlyoothigh",       1000);
  int             lateootlow        = cl.getValue<int>          ("lateootlow",            0);
  int             lateoothigh       = cl.getValue<int>          ("lateoothigh",        1000);
  int             totalootlow       = cl.getValue<int>          ("totalootlow",           0);
  int             totaloothigh      = cl.getValue<int>          ("totaloothigh",       1000);
  int             totallow          = cl.getValue<int>          ("totallow",              0);
  int             totalhigh         = cl.getValue<int>          ("totalhigh",          1000);
  TString         weightfilename    = cl.getValue<TString>      ("weightfilename",       "");
  bool            mpv               = cl.getValue<bool>         ("mpv",               false);
  TString         readRespVsPileup  = cl.getValue<TString>      ("readRespVsPileup",     "");

  if (!cl.check()) return 0;
  cl.print();

  //
  // Some useful quantities
  //
  const char pusources[3][10] = {"EOOT","IT","LOOT"};
  double vresp[NRespBins+1];
  double vcorr[NRespBins+1];
  for(int i=0; i<=NRespBins; i++)
    {
      vresp[i] = (i*((RespHigh-RespLow)/(double)NRespBins));
      vcorr[i] = (i*((CorrHigh-CorrLow)/(double)NRespBins));
    }//for(int i=0; i<=NRespBins; i++)
  
  //
  // To get weights for ptgen distribution
  //
  TFile *weightFile;
  TH1D *weightHist=0;
  if(!weightfilename.IsNull())
    {
      weightFile = new TFile(weightfilename,"READ");
      if (!weightFile->IsOpen()) {cout<<"Can't open ff.root to get weights for ptgen"<<endl;}
      weightHist = (TH1D*)gDirectory->Get("we");
      if (weightHist==0) {cout<<"weightHist named \"we\" was not in file ff.root"<<endl; return 0;}
      weightHist->Scale(1./weightHist->Integral(1,weightHist->FindBin(3)));
    }

  //
  // Loop over the algorithms
  //
  for(unsigned int a=0; a<algs.size(); a++)
    {
      string alias = getAlias(algs[a]);

      TFile *inf = new TFile(inputFilename);
      if(!outputDir.IsNull() && !outputDir.EndsWith("/")) outputDir += "/";
      TFile *outf = new TFile(outputDir+"Closure_"+algs[a]+".root","RECREATE");
  
      char name[1024];
      float refpt[100];
      float refeta[100];
      float refphi[100];
      float jtpt[100];
      float jteta[100];
      float jtphi[100];
      float refdrjt[100];
      float refdphijt[100];
      int   refpdgid[100];
      vector<int>* npus = new vector<int>;
      vector<Float_t>* sumpt_lowpt = new vector<Float_t>;
      TH2F *RespVsPt_Bar;
      TH2F *RespVsPt_End;
      TH2F *RespVsPt_Fwd;
      TH2F *RespVsEta[NPtBins];
      TH3F *RespVsEtaVsPt;
      TH3F *ScaleVsEtaVsPt;
      TProfile *RelRspVsSumPt;
      TH1F *SumPtDistributions[NPileup/2];
      TH1F *ResolutionVsEta;
      TH1F *ResolutionVsPt;
      TH2D *EtaVsPt;
      TH1F *RefEtaDistribution;
      TH1F *EtaDistribution;
      TH1F *iEtaDistribution;
      TH1F *EtaDistributionPU0;
      TH1F *EtaDistributionPU[10];
      TH1F *ThetaDistribution;
      TH1F *SolidAngleDist;
      TH1F *HigherDist;
      TH1F *MiddleDist;
      TH1F *LowerDist;
      TH1F *RelContributions[NPtBins];
      VectorWrapper RespVsPileup;
      TProfile *DPtVsNPU[3];
      TProfile *DPtVsPtGen[3];
      TProfile *RespRatioVsPtGen[3];
      int j;
      unsigned int nrefmax = 2;
      unsigned char nref;

      //
      // Get the corrections from the text files
      //
      JetCorrectorParameters *L1JetPar;
      JetCorrectorParameters *L2JetPar;
      JetCorrectorParameters *L3JetPar;
      JetCorrectorParameters *ResJetPar;
      vector<JetCorrectorParameters> vPar;

      if(useL1Cor)
        {
          L1JetPar = new JetCorrectorParameters(path + era + "_L1FastJet_"    + alias + ".txt");
          vPar.push_back(*L1JetPar);
          cout << "Using " << path << era << "_L1FastJet_" << alias << ".txt" << endl;
        }
      if(useL2Cor)
        {
          L2JetPar = new JetCorrectorParameters(path + era + "_L2Relative_"   + alias + ".txt");
          vPar.push_back(*L2JetPar);
          cout << "Using " << path << era << "_L2Relative_" << alias << ".txt" << endl;
        }
      if(useL3Cor)
        {
          L3JetPar = new JetCorrectorParameters(path + era + "_L3Absolute_"   + alias + ".txt");
          vPar.push_back(*L3JetPar);
          cout << "Using " << path << era << "_L3Absolute_" << alias << ".txt" << endl;
        }
      if(useL2L3ResCor)
        {
          ResJetPar = new JetCorrectorParameters(path + era + "_L2L3Residual_" + alias + ".txt"); 
          vPar.push_back(*ResJetPar);
          cout << "Using " << path << era << "_L2L3Residual_" << alias << ".txt" << endl;
        }
      FactorizedJetCorrector *JetCorrector = new FactorizedJetCorrector(vPar);

      //
      // setup the tree for reading
      //
      TDirectoryFile *idir = (TDirectoryFile*)inf->Get(algs[a]);
      if (idir) 
        cout << "The directory is " << idir->GetName() << endl;
      else {
        cout << "ERROR::Directory " << algs[a] <<" could not be found in file " << inf->GetName() << endl;
        cout << " SKIPPING ALGO " << algs[a] << endl;
        continue;
      }

      TTree *tree = (TTree*)idir->Get("t");
      tree->SetBranchAddress("nref",        &nref);
      tree->SetBranchAddress("refpt",       refpt);
      tree->SetBranchAddress("refeta",      refeta);
      tree->SetBranchAddress("refphi",      refphi);
      tree->SetBranchAddress("jtpt",        jtpt);
      tree->SetBranchAddress("jteta",       jteta);
      tree->SetBranchAddress("jtphi",       jtphi);
      tree->SetBranchAddress("npus",        &npus);
      tree->SetBranchAddress("sumpt_lowpt", &sumpt_lowpt);
      tree->SetBranchAddress("refdrjt",     refdrjt);
      if (doflavor) tree->SetBranchAddress("refpdgid",refpdgid);

      //
      // book histograms
      //
      RespVsPt_Bar = new TH2F("RespVsPt_Bar","RespVsPt_Bar",NPtBins,vpt,NRespBins,RespLow,RespHigh);
      RespVsPt_Bar->Sumw2(); 
      RespVsPt_End = new TH2F("RespVsPt_End","RespVsPt_End",NPtBins,vpt,NRespBins,RespLow,RespHigh);
      RespVsPt_End->Sumw2();
      RespVsPt_Fwd = new TH2F("RespVsPt_Fwd","RespVsPt_Fwd",NPtBins,vpt,NRespBins,RespLow,RespHigh);
      RespVsPt_Fwd->Sumw2();
      HigherDist = new TH1F("HigherDist","HigherDist",1999,1,2000);
      MiddleDist = new TH1F("MiddleDist","MiddleDist",1999,1,2000);
      LowerDist = new TH1F("LowerDist","LowerDist",1999,1,2000);
      ScaleVsEtaVsPt = new TH3F("ScaleVsEtaVsPt","ScaleVsEtaVsPt",NPtBins,vpt,NETA,veta,NRespBins,vcorr);
      ScaleVsEtaVsPt->Sumw2();
      RespVsEtaVsPt = new TH3F("RespVsEtaVsPt","RespVsEtaVsPt",NPtBins,vpt,NETA,veta,NRespBins,vresp);
      RespVsEtaVsPt->Sumw2();
      RelRspVsSumPt = new TProfile("RelRspVsSumPt","RelRspVsSumPt",NPtBins,vpt);
      RelRspVsSumPt->Sumw2();
      ResolutionVsEta = new TH1F("ResolutionVsEta","ResolutionVsEta",NETA,veta);
      ResolutionVsEta->Sumw2();
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
      for(int i=0;i<NPtBins;i++)
        {
          sprintf(name,"RespVsEta_RefPt%sto%s",Pt[i],Pt[i+1]);
          RespVsEta[i] = new TH2F(name,name,NETA,veta,NRespBins,RespLow,RespHigh);
          sprintf(name,"RelContributions_RefPt%sto%s",Pt[i],Pt[i+1]);
          RelContributions[i] = new TH1F(name,name,1999,1,2000);
        }//for(int i=0;i<NPtBins;i++)
      RespVsPileup.SetName("RespVsPileup");
      if(readRespVsPileup.IsNull())
        {
          for(int i=0;i<NPtBins;i++)
            {  
              sprintf(name,"RespVsPileupSource_RefPt%sto%s",Pt[i],Pt[i+1]);
              RespVsPileup.push_back(new TProfile3D(name,name,26,0,26,26,0,26,26,0,26));
            }
        }
      else
        {
          RespVsPileup.readFile(readRespVsPileup);
          cout << "Entries: " << RespVsPileup[19]->GetEntries() << endl;
        }
      outf->cd();
      for(unsigned int i=0; i<NPileup/2; i++)
        {
          sprintf(name,"SumPtDistribution_NPU%sto%s",pileup_boundaries[i*2],pileup_boundaries[(i*2)+1]);
          SumPtDistributions[i] = new TH1F(name,name,NPtBins,vpt);
        }//for(unsigned int i=0; i<NPileup/2; i++)
      for(int i=0; i<3; i++)
        {
          sprintf(name,"DPtVsNPU_%s",pusources[i]);
          DPtVsNPU[i] = new TProfile(name,name,26,0,26);
          sprintf(name,"DPtVsPtGen_%s",pusources[i]);
          DPtVsPtGen[i] = new TProfile(name,name,NPtBins,vpt);
          sprintf(name,"RespRatioVsPtGen_%s",pusources[i]);
          RespRatioVsPtGen[i] = new TProfile(name,name,NPtBins,vpt);
        }//for(int i=0; i<3; i++)

      //
      // fill histograms
      //
      unsigned int nevt = (unsigned int)tree->GetEntries();
      cout<<algs[a]<<"......"<<nevt<<" entries:"<<endl;
      int min_npu=100;
      for (unsigned int ievt=0;ievt<nevt;ievt++) 
        {
          if (evtmax>0 && ievt>evtmax) continue;
          if (ievt % 100000 == 0) 
            cout<<ievt<<endl;
          tree->GetEntry(ievt);

          int npu = (*npus)[0]+(*npus)[1]+(*npus)[2];
          int itnpu = (*npus)[1];
          double sumpt = (*sumpt_lowpt)[1];
          if (printnpu) cout<<" ievt = "<<ievt<<"\tnpu = "<<npu<<endl;
          if (npu<min_npu) min_npu = npu;

          if (!pileup_cut(itlow,ithigh,earlyootlow,earlyoothigh,lateootlow,lateoothigh,
                          totalootlow,totaloothigh,totallow,totalhigh,npus)) continue;
          if (dphimin>0 && abs(jtphi[0]-jtphi[1])<dphimin) continue;

          for (unsigned char iref=0;iref<nrefmax;iref++) 
            {
              float ptgen  = refpt[iref];
              if (ptgen<ptgenmin) continue;
              if (doflavor && refpdgid[iref]!=pdgid) continue;
              float eta    = jteta[iref];
              if (etamax>0 && TMath::Abs(eta)>etamax) continue;
              float pt     = jtpt[iref];
              if (pt > 14000) 
                {
                  cout << "WARNING::pt>14000 GeV (pt = " << pt << " GeV)." << endl << "Skipping this jet." << endl;
                  continue;
                }
              float dr     = refdrjt[iref];
              if (drmax > 0 && dr > drmax) continue;
              JetCorrector->setJetPt(pt);
              JetCorrector->setJetEta(eta);
              float scale  = JetCorrector->getCorrection();
              //
              // we have to fill this histogram before we kill the event
              //
              ScaleVsEtaVsPt->Fill(ptgen,eta,scale);
              if (scale < 0) continue;
              if ((pt*scale)<ptmin) continue;
              float relrsp = scale*jtpt[iref]/refpt[iref];
              float theta  = 2.0*atan(exp(-eta));
              double weight;

              if(weightHist!=0) weight = weightHist->GetBinContent(weightHist->FindBin(log10(ptgen)));
              else weight = 1;

              if (fabs(eta)<=1.3)
                {
                  RespVsPt_Bar->Fill(ptgen,relrsp,weight);
                }
              if ((fabs(eta)<=3.0) && (fabs(eta)>1.3))
                {
                  RespVsPt_End->Fill(ptgen,relrsp,weight);
                }
              if ((fabs(eta)<=5.0) && (fabs(eta)>3))
                {
                  RespVsPt_Fwd->Fill(ptgen,relrsp,weight); 
                }
              
              if(HigherDist->FindBin(scale*pt) < HigherDist->FindBin(ptgen)) HigherDist->Fill(scale*pt);
              if(MiddleDist->FindBin(scale*pt) == MiddleDist->FindBin(ptgen)) MiddleDist->Fill(scale*pt);
              if(LowerDist->FindBin(scale*pt) > LowerDist->FindBin(ptgen)) LowerDist->Fill(scale*pt);
              
              j = getBin(ptgen,vpt);
              if (j<NPtBins && j>=0)
                {
                  RespVsEta[j]->Fill(eta,relrsp);
                  RelContributions[j]->Fill(scale*pt);
                  if(readRespVsPileup.IsNull())
                    { 
                      RespVsPileup[j]->Fill((*npus)[0],(*npus)[1],(*npus)[2],relrsp);
                    }
                  else
                    {
                      //
                      // Psi = {RelRsp[p_T^GEN,EOOT,IT,LOOT]-RelRsp[p_T^GEN,EOOT,IT,LOOT]}*p_T^GEN
                      // where either EOOT,IT, or LOOT are zero for the second RelRsp. The first RelRsp
                      // is the relative response of the current jet.
                      //
                      double resp_EOOT = RespVsPileup[j]->
                        GetBinContent(RespVsPileup[j]->FindBin(0,(*npus)[1],(*npus)[2]));
                      double resp_IT   = RespVsPileup[j]->
                        GetBinContent(RespVsPileup[j]->FindBin((*npus)[0],0,(*npus)[2]));
                      double resp_LOOT = RespVsPileup[j]->
                        GetBinContent(RespVsPileup[j]->FindBin((*npus)[0],(*npus)[1],0));
                      double Psi_EOOT = (relrsp-resp_EOOT)*ptgen;
                      double Psi_IT = (relrsp-resp_IT)*ptgen;
                      double Psi_LOOT = (relrsp-resp_LOOT)*ptgen;

                      //
                      // PsiPrime = RelRsp[p_T^GEN,EOOT,IT,LOOT]/RelRsp[p_T^GEN,EOOT,IT,LOOT]
                      // where either EOOT,IT, or LOOT are zero for the RelRsp in the denominator.
                      // The RelRsp in the numerator is the relative response of the current jet.
                      //
                      double PsiPrime_EOOT = relrsp/resp_EOOT;
                      double PsiPrime_IT = relrsp/resp_IT;
                      double PsiPrime_LOOT = relrsp/resp_LOOT;
                  
                      if(resp_EOOT!=0)
                        {
                          DPtVsNPU[0]->Fill(itnpu,Psi_EOOT);
                          DPtVsPtGen[0]->Fill(ptgen,Psi_EOOT);
                          RespRatioVsPtGen[0]->Fill(ptgen,PsiPrime_EOOT);
                        }
                      if(resp_IT!=0)
                        {
                          DPtVsNPU[1]->Fill(itnpu,Psi_IT); 
                          DPtVsPtGen[1]->Fill(ptgen,Psi_IT);
                          RespRatioVsPtGen[1]->Fill(ptgen,PsiPrime_IT);
                        } 
                      if(resp_LOOT!=0)
                        {
                          DPtVsNPU[2]->Fill(itnpu,Psi_LOOT);
                          DPtVsPtGen[2]->Fill(ptgen,Psi_LOOT);
                          RespRatioVsPtGen[2]->Fill(ptgen,PsiPrime_LOOT);
                        }
                    }//if(!readRespVsPileup.IsNull())
                }//if (j<NPtBins && j>=0)

              EtaVsPt->Fill(eta, log10(pt*scale));
              RelRspVsSumPt->Fill(sumpt,relrsp);
              for(int spd=0; spd<NPileup/2; spd++)
                {
                  if(itnpu>=vpileup[spd*2] && itnpu<=vpileup[(spd*2)+1])
                    SumPtDistributions[spd]->Fill(sumpt);
                }
              RespVsEtaVsPt->Fill(ptgen,eta,relrsp);
              RefEtaDistribution->Fill(refeta[iref]);
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
            }//for (unsigned char iref=0;iref<nrefmax;iref++) 
        }//for (unsigned int ievt=0;ievt<nevt;ievt++)

      //
      // make histograms that rely on other, completely filled, histograms
      //
      makeResolutionHistogram(RespVsEtaVsPt,ResolutionVsEta,"y",mpv);
      makeResolutionHistogram(RespVsEtaVsPt,ResolutionVsPt,"x",mpv);

      //
      // final cout statements
      //
      cout << " min_npu="<<min_npu<<endl;

      //
      // close files
      //
      cout << "Write " << "RespVsPileup_" << algs[a] << ".root" << " ... ";
      if(readRespVsPileup.IsNull())
        {
          RespVsPileup.writeFile(outputDir+"RespVsPileup_"+algs[a]+".root");
        }
      cout << "DONE" << endl << "Write " << "Closure_" << algs[a] << ".root" << " ... ";
      outf->cd();
      outf->Write();
      outf->Close();
      cout << "DONE" << endl;
    }//for(unsigned int a=0; a<algs.size(); a++)
}

////////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int getBin(double x, const double boundaries[NPtBins+1])
{
  int i;
  int n = NPtBins;
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
void makeResolutionHistogram(TH3F* RespVs_, TH1F* ResolutionVs_, TString slice, bool mpv)
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
        h = RespVs_->ProjectionZ("_pz",0,-1,i,i);
      else if(slice.CompareTo("x")==0)
        h = RespVs_->ProjectionZ("_pz",i,i,0,-1);
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
bool it_pileup(int itlow, int ithigh, vector<int>* npus)
{
  if((*npus)[1]>=itlow && (*npus)[1]<=ithigh) return true;
  return false;
}


//______________________________________________________________________________
bool oot_pileup(int earlyootlow, int earlyoothigh, int lateootlow, int lateoothigh,
                vector<int>* npus)
{
  if((*npus)[0]>=earlyootlow && (*npus)[0]<=earlyoothigh && 
     (*npus)[2]>=lateootlow && (*npus)[2]<=lateoothigh) return true;
  return false;
}


//______________________________________________________________________________
bool total_oot_pileup(int totalootlow, int totaloothigh, vector<int>* npus)
{
  if((*npus)[0]+(*npus)[2]>=totalootlow && (*npus)[0]+(*npus)[2]<=totaloothigh) return true;
  return false;
}

//______________________________________________________________________________
bool total_pileup(int totallow, int totalhigh, vector<int>* npus)
{
  if((*npus)[0]+(*npus)[1]+(*npus)[2]>=totallow && (*npus)[0]+(*npus)[1]+(*npus)[2]<=totalhigh) return true;
  return false;
}

//______________________________________________________________________________
bool pileup_cut(int itlow, int ithigh, int earlyootlow, int earlyoothigh, 
                int lateootlow, int lateoothigh, int totalootlow, int totaloothigh, 
                int totallow, int totalhigh, vector<int>* npus)
{
  if(it_pileup(itlow,ithigh,npus) && 
     total_oot_pileup(totalootlow,totaloothigh,npus) && 
     oot_pileup(earlyootlow,earlyoothigh,lateootlow,lateoothigh,npus) &&
     total_pileup(totallow,totalhigh,npus)) return true;
  return false;
}

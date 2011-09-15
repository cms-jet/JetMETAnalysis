///////////////////////////////////////////////////////////////////
//
// jet_correction_analyzer_x
// -------------------------
//
//            08/25/2011 Alexx Perloff  <aperloff@physics.tamu.edu>
///////////////////////////////////////////////////////////////////

#include "JetMETAnalysis/JetAnalyzers/interface/Settings.h"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"

#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"

#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2.h"
#include "TH2F.h"
#include "TString.h"
#include "TMath.h"

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

/// boolean for whether the event has the appropriate amount of OOT pileup
bool checkOOTPileup(bool checkForOOTPileup, bool OOTPileup,vector<int>* npus);

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv)
{
  gSystem->Load("libFWCoreFWLite.so");
  
  //
  // evaluate command-line / configuration file options
  // 
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;

  TString         inputFilename     = cl.getValue<TString>  ("inputFilename");
  vector<TString> algs              = cl.getVector<TString> ("algs");
  string          path              = cl.getValue<string>   ("path");
  string          era               = cl.getValue<string>   ("era");
  TString         outputDir         = cl.getValue<TString>  ("outputDir",            "");
  bool            useL1Cor          = cl.getValue<bool>     ("useL1Cor",          false);
  bool            useL2Cor          = cl.getValue<bool>     ("useL2Cor",           true);
  bool            useL3Cor          = cl.getValue<bool>     ("useL3Cor",          false);
  bool            useL2L3ResCor     = cl.getValue<bool>     ("useL2L3ResCor",     false);
  bool            checkForOOTPileup = cl.getValue<bool>     ("checkForOOTPileup", false);
  bool            OOTPileup         = cl.getValue<bool>     ("OOTPileup",         false);
  bool            doflavor          = cl.getValue<bool>     ("doflavor",          false);
  int             pdgid             = cl.getValue<int>      ("pdgid",                 0);
  double          drmax             = cl.getValue<double>   ("drmax",                 0);
  bool            printnpu          = cl.getValue<bool>     ("printnpu",          false);
  TString         weightfilename    = cl.getValue<TString>  ("weightfilename",       "");

  if (!cl.check()) return 0;
  cl.print();

  //
  // The first & last bin are not calo towers.
  //
  double binseta[85] = 
    {-5.4, -5.191, -4.889, -4.716, -4.538, -4.363, -4.191, -4.013, -3.839, -3.664, 
     -3.489, -3.314, -3.139, -2.964, -2.853, -2.650, -2.500, -2.322, -2.172, 
     -2.043, -1.930, -1.830, -1.740, -1.653, -1.566, -1.479, -1.392, -1.305, 
     -1.218, -1.131, -1.044, -0.957, -0.879, -0.783, -0.696, -0.609, -0.522, 
     -0.435, -0.348, -0.261, -0.174, -0.087,    
     +0.000,    
     +0.087, +0.174, +0.261, +0.348, +0.435, +0.522, +0.609, +0.696, +0.783, 
     +0.879, +0.957, +1.044, +1.131, +1.218, +1.305, +1.392, +1.479, +1.566, 
     +1.653, +1.740, +1.830, +1.930, +2.043, +2.172, +2.322, +2.500, +2.650, 
     +2.853, +2.964, +3.139, +3.314, +3.489, +3.664, +3.839, +4.013, +4.191, 
     +4.363, +4.538, +4.716, +4.889, +5.191, + 5.4};
  
  //
  // To get weights for ptgen distribution
  //
  TFile *weightFile;
  TH1D *weightHist;
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
      TH2F *RespVsPt_Bar;
      TH2F *RespVsPt_End;
      TH2F *RespVsPt_Fwd;
      TH2F *RespVsEta[NPtBins];
      TH2F *ScaleVsEta;
      TH2D *EtaVsPt;
      TH1F *RefEtaDistribution;
      TH1F *EtaDistribution;
      TH1F *iEtaDistribution;
      TH1F *EtaDistributionPU0;
      TH1F *EtaDistributionPU[10];
      TH1F *ThetaDistribution;
      TH1F *SolidAngleDist;
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
      cout << "The directory is " << idir->GetName() << endl;

      TTree *tree = (TTree*)idir->Get("t");
      tree->SetBranchAddress("nref",   &nref);
      tree->SetBranchAddress("refpt",   refpt);
      tree->SetBranchAddress("refeta",  refeta);
      tree->SetBranchAddress("refphi",  refphi);
      tree->SetBranchAddress("jtpt",    jtpt);
      tree->SetBranchAddress("jteta",   jteta);
      tree->SetBranchAddress("jtphi",   jtphi);
      tree->SetBranchAddress("npus",    &npus);
      tree->SetBranchAddress("refdrjt", refdrjt);
      if (doflavor) tree->SetBranchAddress("refpdgid",refpdgid);

      //
      // book histograms
      //
      RespVsPt_Bar = new TH2F("RespVsPt_Bar","RespVsPt_Bar",NPtBins,vpt,200,0,2);
      RespVsPt_Bar->Sumw2(); 
      RespVsPt_End = new TH2F("RespVsPt_End","RespVsPt_End",NPtBins,vpt,200,0,2);
      RespVsPt_End->Sumw2();
      RespVsPt_Fwd = new TH2F("RespVsPt_Fwd","RespVsPt_Fwd",NPtBins,vpt,200,0,2);
      RespVsPt_Fwd->Sumw2();
      ScaleVsEta    = new TH2F("ScaleVsEta","ScaleVsEta",220,-5.5,5.5,200,0,10);
      iEtaDistribution  = new TH1F("iEtaDistribution"   ,"iEtaDistribution",84, binseta);
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
      // dN/dOmega = dN/2*Pi*d(cos(theta))
      SolidAngleDist    = new TH1F("SolidAngleDist","SolidAngleDist",200, -2*TMath::Pi(),2*TMath::Pi());
      for(int i=0;i<NPtBins;i++)
        {
          sprintf(name,"RespVsEta_RefPt%sto%s",Pt[i],Pt[i+1]);
          RespVsEta[i] = new TH2F(name,name,NETA,veta,200,0,2);
        }

      //
      // fill histograms
      //
      unsigned int nevt = (unsigned int)tree->GetEntries();
      cout<<algs[a]<<"......"<<nevt<<" entries:"<<endl;
      int min_npu=100;
      for (unsigned int ievt=0;ievt<nevt;ievt++) 
        {
          if (ievt % 100000 == 0) 
            cout<<ievt<<endl;
          tree->GetEntry(ievt);
          int npu = (*npus)[0]+(*npus)[1]+(*npus)[2] ;
          if (printnpu) cout<<" ievt = "<<ievt<<"\tnpu = "<<npu<<endl;
          if (npu<min_npu) min_npu = npu;
          if(checkOOTPileup(checkForOOTPileup,OOTPileup,npus))
            {
              for (unsigned char iref=0;iref<nrefmax;iref++) 
                {
                  if(doflavor && refpdgid[iref]!=pdgid) continue;
                  float eta    = jteta[iref];
                  float pt     = jtpt[iref];
                  float dr     = refdrjt[iref];
                  if(drmax > 0 && dr > drmax) continue;
                  JetCorrector->setJetPt(pt);
                  JetCorrector->setJetEta(eta);
                  float scale  = JetCorrector->getCorrection();
                  float ptgen  = refpt[iref];
                  float theta  = 2.0*atan(exp(-eta));
                  float relrsp = scale*jtpt[iref]/refpt[iref];
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

                  j = getBin(ptgen,vpt);
                  if (j<NPtBins && j>=0)
                    RespVsEta[j]->Fill(eta,relrsp);

                  EtaVsPt->Fill(eta, log10(pt*scale));
                  ScaleVsEta->Fill(eta,scale);
                  RefEtaDistribution->Fill(refeta[iref]);
                  EtaDistribution->Fill(eta);
                  iEtaDistribution->Fill(eta);
                  int in_npu = npu/5;// 0-4, 5-9, 10-14, 15-19, 20-24, 25-29, 30-34, 35-39, 40-44, 45-infinity
                  if (in_npu > 9) in_npu = 9;
              
                  EtaDistributionPU[in_npu]->Fill(eta);
                  if (npu==0) EtaDistributionPU0->Fill(eta);
                  ThetaDistribution ->Fill(theta);
                  SolidAngleDist ->Fill(2*TMath::Pi()*cos(theta));
                } 
            }
        }

      //
      // close files
      //
      cout<<" min_npu="<<min_npu<<endl;
      outf->cd();
      outf->Write();
      outf->Close();
    }
}

////////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int getBin(double x, const double boundaries[NPtBins])
{
  int i;
  int n = NPtBins-1;
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
  else if (s=="ak7calo")
    return "AK7Calo";
  else if (s=="ak7calol1")
    return "AK7Calol1";
  else if (s=="ak5pf")
    return "AK5PF";
  else if (s=="ak5pfl1")
    return "AK5PFl1";
  else if (s=="ak7pf")
    return "AK7PF";
  else if (s=="ak7pfl1")
    return "AK7PFl1";
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
bool checkOOTPileup(bool checkForOOTPileup, bool OOTPileup,vector<int>* npus)
{
  if(!checkForOOTPileup) return true;
  else if(checkForOOTPileup && OOTPileup && ((*npus)[0]+(*npus)[2])>0) return true;
  else if(checkForOOTPileup && !OOTPileup && ((*npus)[0]+(*npus)[2])==0) return true;
  else return false;
}

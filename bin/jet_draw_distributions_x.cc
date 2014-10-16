///////////////////////////////////////////////////////////////////
//
// jet_draw_distributions_x
// ------------------------
//
//            09/16/2011 Alexx Perloff  <aperloff@physics.tamu.edu>
///////////////////////////////////////////////////////////////////

#include "JetMETAnalysis/JetAnalyzers/interface/Settings.h"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"

#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2.h"
#include "TH2F.h"
#include "TF1.h"
#include "THStack.h"
#include "TString.h"
#include "TPaveText.h"
#include "TLegend.h"

#include <fstream>
#include <string>
#include <stdio.h>
#include <stdarg.h>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv)
{
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);

  gSystem->Load("libFWCoreFWLite.so");
  
  //
  // evaluate command-line / configuration file options
  // 
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;
  
  vector<TString> algs = cl.getVector<TString> ("algs");
  TString flavor       = cl.getValue<TString>  ("flavor",           "");
  TString path         = cl.getValue<TString>  ("path",             "");
  TString outputDir    = cl.getValue<TString>  ("outputDir",  "images");
  TString outputFormat = cl.getValue<TString>  ("outputFormat", ".png");

  if (!cl.check()) return 0;
  cl.print();

  for(unsigned int a=0; a<algs.size(); a++)
    {
      //
      // Open the file containing the original, unformated closure plots
      //
      TFile *inf;
      if(path.IsNull()) path = string (gSystem->pwd())+"/";
      if(!path.EndsWith("/")) path+="/";

      if (!flavor.IsNull()) inf = new TFile(path+"Closure_"+algs[a]+"_"+flavor+".root");
      else inf = new TFile(path+"Closure_"+algs[a]+".root");

      //
      // Open/create the output directory and file
      //
      if(!outputDir.EndsWith("/")) outputDir+="/";
      if(!gSystem->OpenDirectory(outputDir)) gSystem->mkdir(outputDir);
      TString ofname = outputDir+"ClosureDistributions_"+algs[a]+".root";
      if(!flavor.IsNull()) ofname = outputDir+"ClosureDistributions_"+algs[a]+"_"+flavor+".root";
      TFile* outf = new TFile(ofname,"RECREATE");

      //
      // book histograms and functions
      //
      TH1F* HigherDist = (TH1F*)inf->Get("HigherDist");
      //cout << "HigherDist="<<HigherDist << endl;
      HigherDist->SetFillColor(kRed);
      TH1F* MiddleDist = (TH1F*)inf->Get("MiddleDist");
      MiddleDist->SetFillColor(kBlack);
      TH1F* LowerDist = (TH1F*)inf->Get("LowerDist");
      LowerDist->SetFillColor(kBlue);
      THStack* HMLHist = new THStack("HMLHist","HMLHist");
      HMLHist->Add(LowerDist);
      HMLHist->Add(MiddleDist);
      HMLHist->Add(HigherDist);
      TLegend* leg_HML = new TLegend(0.8,0.45,1.0,0.55);
      leg_HML->AddEntry(LowerDist,"genPt < jetPt","fl");
      leg_HML->AddEntry(MiddleDist,"genPt = jetPt","fl");
      leg_HML->AddEntry(HigherDist,"genPt > jetPt","fl");

      char name[1024];
      TH1F *SumPtDistributions[NPileup/2];
      THStack *SumPtStack = new THStack("SumPtDistributionStack","Pileup p_{T}-Sum Distributions by NPU");
      TLegend* leg_SumPtDistributions = new TLegend(0.7,0.4,1.0,0.6);
      for(unsigned int i=0; i<NPileup/2; i++)
        {
          sprintf(name,"SumPtDistribution_NPU%sto%s",pileup_boundaries[i*2],pileup_boundaries[(i*2)+1]);
          SumPtDistributions[i] = (TH1F*)inf->Get(name);
          SumPtDistributions[i]->SetFillColor(i+1);
          SumPtStack->Add(SumPtDistributions[i]);
          leg_SumPtDistributions->AddEntry(SumPtDistributions[i],name,"fl");
        }
  
      TH1F* RelContributions[NPtBins];
      THStack* RelDist = new THStack("RelDist","RelDist");
      TLegend* leg_RelContributions = new TLegend(0.8,0.1,1.0,0.9);
      for(int i=0; i<NPtBins; i++)
        {
          sprintf(name,"RelContributions_RefPt%sto%s",Pt[i],Pt[i+1]);
          RelContributions[i]=(TH1F*)inf->Get(name);
          RelContributions[i]->SetFillColor(i);
          RelDist->Add(RelContributions[i]);
          leg_RelContributions->AddEntry(RelContributions[i],name,"fl");
        }

      TString ss("ClosureDistributions");
      if(!flavor.IsNull()) ss+="_"+algs[a]+"_"+flavor;
      else ss+="_"+algs[a];

      TCanvas *can = new TCanvas(ss,ss,1600,800);
      can->Divide(2,1);

      can->cd(1)->SetLogx();
      HMLHist->Draw();
      HMLHist->GetXaxis()->SetTitle("Jet p_{T} (GeV)");
      HMLHist->GetYaxis()->SetTitle("NEvents");
      HMLHist->GetXaxis()->SetLabelSize(0.04);
      HMLHist->GetXaxis()->SetMoreLogLabels();
      HMLHist->GetXaxis()->SetNoExponent();
      HMLHist->GetYaxis()->SetLabelSize(0.04);
      HMLHist->GetYaxis()->SetTitleOffset(1.3);
      leg_HML->Draw("same");

      can->cd(2)->SetLogx();
      RelDist->Draw();
      RelDist->GetXaxis()->SetTitle("Jet p_{T} (GeV)");
      RelDist->GetYaxis()->SetTitle("NEvents");
      RelDist->GetXaxis()->SetLabelSize(0.04);
      RelDist->GetXaxis()->SetMoreLogLabels();
      RelDist->GetXaxis()->SetNoExponent();
      RelDist->GetYaxis()->SetLabelSize(0.04);
      RelDist->GetYaxis()->SetTitleOffset(1.3);
      leg_RelContributions->Draw("same");

      TCanvas *can2 = new TCanvas("PileupPtSumDistributionsByNPU","PileupPtSumDistributionsByNPU",800,800);
      can2->cd();
      gPad->SetLogx();
      SumPtStack->Draw();
      SumPtStack->GetXaxis()->SetTitle("Pileup p_{T}-Sum (GeV)");
      SumPtStack->GetYaxis()->SetTitle("a.u.");
      SumPtStack->GetYaxis()->SetTitleOffset(1.15);
      SumPtStack->GetXaxis()->SetLabelSize(0.03);
      SumPtStack->GetYaxis()->SetLabelSize(0.03);
      leg_SumPtDistributions->Draw("same");

      can->SaveAs(outputDir+ss+outputFormat);
      can2->SaveAs(outputDir+"PileupPtSumDistributionsByNPU"+outputFormat);
      can->Write();
      can2->Write();
      outf->Write();
      outf->Close();
    }
}

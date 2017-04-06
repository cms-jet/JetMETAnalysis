#include <map>
#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include <locale> // std::locale, std::tolower

#include "TFile.h"
#include "TChain.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TProfile3D.h"
#include "TGraph2DErrors.h"
#include "TSystem.h"
#include "TF2.h"
#include "TH2.h"
#include "TCutG.h"
#include "TMath.h"
#include "TH1.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TDirectory.h"
#include "TSystem.h"
#include "TObject.h"
#include "TList.h"
#include "TKey.h"
#include "TLegend.h"
#include "TGaxis.h"
#include "THStack.h"

#include "JetMETAnalysis/JetUtilities/interface/JetInfo.hh"
#include "JetMETAnalysis/JetUtilities/src/SynchFittingProcedure.hh"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/ObjectLoader.h"
#include "JetMETAnalysis/JetAnalyzers/interface/REStyle.h"
#include "JetMETAnalysis/JetUtilities/interface/Style.h"

using namespace std;

bool writeFlag = true;

int colPU = kBlack;
int colNoPU = kRed+1;

int binNum3035 = 15;
int binNum2023 = 12;

vector<pair<int,int> > npvRhoNpuBins;
int minNpvRhoNpu;
int maxNpvRhoNpu;

vector<TString> outputFormat;

void clearHistograms(vector<TH2*>& hResRho, vector<TH2*>& hOffRho, vector<TH2*>& hOffPdgid, vector<TProfile*>& pOffPF, int resetSize = -1) {
   hResRho.clear();
   hOffRho.clear();
   hOffPdgid.clear();
   pOffPF.clear();
   if(resetSize>-1) {
      hResRho = vector<TH2*>(resetSize,(TH2*)0);
      hOffRho = vector<TH2*>(resetSize,(TH2*)0);
      hOffPdgid = vector<TH2*>(resetSize,(TH2*)0);
      pOffPF = vector<TProfile*>(resetSize,(TProfile*)0);
   }
}

void SynchPlots(TString inputDir="./",TString calgo1="ak5pf",TString calgo2="ak5pf", TString outDir = "./images", bool fixedRange = true){
   TString algo1(calgo1);
   TString algo2(calgo2);
   TString algo12 = algo1+"_"+algo2;
   if (algo1.EqualTo(algo2)) 
      algo12 = algo1;
   TString algo(algo12);
   TString filename=Form("%s/output_%s.root",inputDir.Data(),algo.Data());
   cout<<"Producing plots from file "<<filename<<endl;
   TFile *fin = new TFile(filename);

   // Just aux pointers
   TCanvas * c = 0;
   TLegend * leg = 0;

// Reading in histograms from root files
//==================================================================================
   map<TString, TH1*> histograms;
   TIter nextHist(gDirectory->GetListOfKeys());
   TKey* histKey(0);
   while ((histKey=(TKey*)nextHist())) {
      if (JetInfo::vfind(histogram_types, NHistogramTypes, TString(histKey->GetClassName()))<0) continue;
      histograms[histKey->GetName()] = (TH1*)histKey->ReadObj();
   }
   TString hname = "";
   TString detectorAbbreviation = "";
   std::locale loc;
//==================================================================================

/*
//PLaying around with the ObjectLoader. This would be helpful if the histograms were named appropriately.
   ObjectLoader<TProfile> hl_rsp;
   //m_njet_pt_npv0_4_unmatch
   //hl_rsp.load_objects(idir,"RelRsp:JetEta:RefPt");
   hl_rsp.load_objects(gDirectory,"m_njet_pt:npv:unmatch");
   cout << "NObjects="<<hl_rsp.nobjects() << endl;
   cout << "NVariables="<<hl_rsp.nvariables() << endl;
   for(unsigned int i=0; i<hl_rsp.nvariables(); i++)
      cout << "\t" << hl_rsp.variable(i) << endl;
   vector<unsigned int> indices; TProfile* hrsp(0);
   hl_rsp.begin_loop();
   while ((hrsp=hl_rsp.next_object(indices))) {
      cout << "Nindices="<< indices.size() << endl;
   }
   return;
   */

   // Event-Matching performance 
   if(histograms.find("m_refpt_diff")!=histograms.end()) {
      c = new TCanvas("RefPtDiff","RefPtDiff");
      c->SetLogy();
      histograms["m_refpt_diff"]->Draw();
   }
  
   // Event-Matching performance 
   if(histograms.find("m_refpdgid_diff")!=histograms.end()) {
      c = new TCanvas("RefPdgidDiff","RefPdgidDiff");
      c->SetLogy();
      histograms["m_refpdgid_diff"]->Draw();
   }
  
   // Sanity check: g_pthat
   if(histograms.find("m_deltaPthat")!=histograms.end()) {
      c = new TCanvas("PthatDiff","PthatDiff");
      histograms["m_deltaPthat"]->Draw();
      histograms["m_deltaPthat"]->GetYaxis()->SetRangeUser(-1,1);
      histograms["m_deltaPthat"]->Draw();
   }

   // Number of Jets vs PT in both samples
   if(histograms.find("m_njet_pt_pu")!=histograms.end() &&
      histograms.find("m_njet_pt_nopu")!=histograms.end()) {
      c = new TCanvas("NJetsVsPt","NJetsVsPt");
      c->SetLogx();
      setHistoColor(histograms["m_njet_pt_pu"], colPU);
      setHistoColor(histograms["m_njet_pt_nopu"], colNoPU);
      histograms["m_njet_pt_pu"]->GetYaxis()->SetRangeUser(0,4.5e6);
      histograms["m_njet_pt_pu"]->Draw("E");
      histograms["m_njet_pt_nopu"]->Draw("sameE");
  
      leg = new TLegend(0.7,0.4,0.9,0.6);
      leg->SetFillColor(0);
      leg->SetBorderSize(0);
      leg->AddEntry(histograms["m_njet_pt_pu"], "PU sample","lep");
      leg->AddEntry(histograms["m_njet_pt_nopu"], "NoPU sample","lep");
      leg->Draw();
   }

   // Ratio of number of Jets vs PT in both samples
   if(histograms.find("m_njet_pt_pu")!=histograms.end()) {
      c = new TCanvas("NJetsVsPt_Ratio","NJetsVsPt_Ratio");
      c->SetLogx();
      TH1D* ratio = (TH1D*)histograms["m_njet_pt_pu"]->Clone("NJetsVsPt_Ratio");
      ratio->Sumw2();
      ratio->GetYaxis()->SetRangeUser(0,10);
      ratio->Divide(histograms["m_njet_pt_nopu"]);
      ratio->Draw("E");
      
      leg = new TLegend(0.7,0.4,0.9,0.6);
      leg->SetFillColor(0);
      leg->SetBorderSize(0);
      leg->AddEntry(ratio, "#frac{PU sample}{NoPU sample}","lep");
      leg->Draw();
   }

   // Ratio of number of Jets vs PT in both samples (with 10 GeV pT^{GEN} cut)
   if(histograms.find("m_njet_pthigh_pu")!=histograms.end()) {
      c = new TCanvas("NJetsVsPt_Ratio_GenPtCut","NJetsVsPt_Ratio_GenPtCut");
      c->SetLogx();
      TH1D* ratio_GenPtCut = (TH1D*)histograms["m_njet_pthigh_pu"]->Clone("NJetsVsPt_Ratio_GenPtCut");
      ratio_GenPtCut->Sumw2();
      ratio_GenPtCut->GetYaxis()->SetRangeUser(0,10);
      ratio_GenPtCut->Divide(histograms["m_njet_pthigh_nopu"]);
      ratio_GenPtCut->Draw("E");
      
      leg = new TLegend(0.7,0.4,0.9,0.6);
      leg->SetFillColor(0);
      leg->SetBorderSize(0);
      leg->AddEntry(ratio_GenPtCut, "#frac{PU sample}{NoPU sample}","lep");
      leg->Draw();
   }
  
   // njet vs npv
   if(histograms.find("m_all_nj_npv")!=histograms.end() &&
      histograms.find("m_matched_nj_npv")!=histograms.end() &&
      histograms.find("m_unmatched_nj_npv")!=histograms.end()) {
      c = new TCanvas("NJetsVsNPV","NJetsVsNPV");
      setHistoColor(histograms["m_all_nj_npv"],       colDetRegions[0]);
      setHistoColor(histograms["m_matched_nj_npv"],   colDetRegions[1]);
      setHistoColor(histograms["m_unmatched_nj_npv"], colDetRegions[2]);
      histograms["m_all_nj_npv"]->GetYaxis()->SetRangeUser(0,20);
      histograms["m_all_nj_npv"]->Draw("E");
      histograms["m_matched_nj_npv"]->Draw("sameE");
      histograms["m_unmatched_nj_npv"]->Draw("sameE");
      
      leg = new TLegend(0.7,0.75,0.9,0.95);
      leg->SetFillColor(0);
      leg->SetBorderSize(0);
      leg->AddEntry(histograms["m_all_nj_npv"], "All jets","lep");
      leg->AddEntry(histograms["m_matched_nj_npv"], "Matched jets","lep");
      leg->AddEntry(histograms["m_unmatched_nj_npv"], "UnMatched jets","lep");
      leg->Draw();
   }

   // jetpt vs npv
   if(histograms.find("m_all_jtpt_npv")!=histograms.end() &&
      histograms.find("m_matched_jtpt_npv")!=histograms.end() &&
      histograms.find("m_unmatched_jtpt_npv")!=histograms.end()) {
      c = new TCanvas("JtptVsNPV","JtptVsNPV");
      setHistoColor(histograms["m_all_jtpt_npv"],       colDetRegions[0]);
      setHistoColor(histograms["m_matched_jtpt_npv"],   colDetRegions[1]);
      setHistoColor(histograms["m_unmatched_jtpt_npv"], colDetRegions[2]);
      histograms["m_all_jtpt_npv"]->GetYaxis()->SetRangeUser(0,100);
      histograms["m_all_jtpt_npv"]->Draw("E");
      histograms["m_matched_jtpt_npv"]->Draw("sameE");
      histograms["m_unmatched_jtpt_npv"]->Draw("sameE");
      
      leg = new TLegend(0.7,0.75,0.9,0.95);
      leg->SetFillColor(0);
      leg->SetBorderSize(0);
      leg->AddEntry(histograms["m_all_jtpt_npv"], "All jets","lep");
      leg->AddEntry(histograms["m_matched_jtpt_npv"], "Matched jets","lep");
      leg->AddEntry(histograms["m_unmatched_jtpt_npv"], "UnMatched jets","lep");
      leg->Draw();
   }
  

   // Fraction of Matched Jets
   c = new TCanvas("FractionMatchedJetsNoPU","FractionMatchedJets NoPU Sample");
   c->SetLogx();
   leg = new TLegend(0.7,0.4,0.9,0.6);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   for(int det=0; det<NDetectorRegions; det++) {
      hname = detector_regions[det];
      hname.ToLower();
      hname = hname(0,1);
      hname = Form("m_frac_nj_pt_%s_match_nopu",hname.Data());
      if(histograms.find(hname)==histograms.end()) continue;
      setHistoColor(histograms[hname], colDetRegions[det]);
      if(det==0)
         histograms[hname]->Draw();
      else
         histograms[hname]->Draw("same");
      histograms[hname]->GetYaxis()->SetRangeUser(0.3,1.1);
      leg->AddEntry(histograms[hname],detector_regions[det],"lep");
   }
   leg->Draw();


   // Fraction of Matched Jets
   c = new TCanvas("FractionMatchedJetsPU","FractionMatchedJets PU Sample");
   c->SetLogx();
   leg = new TLegend(0.7,0.4,0.9,0.6);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   for(int det=0; det<NDetectorRegions; det++) {
      hname = detector_regions[det];
      hname.ToLower();
      hname = hname(0,1);
      hname = Form("m_frac_nj_pt_%s_match_pu",hname.Data());
      if(histograms.find(hname)==histograms.end()) continue;
      setHistoColor(histograms[hname], colDetRegions[det]);
      if(det==0)
         histograms[hname]->Draw();
      else
         histograms[hname]->Draw("same");
      histograms[hname]->GetYaxis()->SetRangeUser(0.3,1.1);
      leg->AddEntry(histograms[hname],detector_regions[det],"lep");
   }
   leg->Draw();

    
   // Fraction of RG-Matched Jets
   c = new TCanvas("FractionRGMatchedJetsNoPU","FractionRGMatchedJets NoPU Sample");
   c->SetLogx();
   leg = new TLegend(0.7,0.4,0.9,0.6);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   for(int det=0; det<NDetectorRegions; det++) {
      hname = detector_regions[det];
      hname.ToLower();
      hname = hname(0,1);
      hname = Form("m_frac_nj_pt_%s_match_RG_nopu",hname.Data());
      if(histograms.find(hname)==histograms.end()) continue;
      setHistoColor(histograms[hname], colDetRegions[det]);
      if(det==0)
         histograms[hname]->Draw();
      else
         histograms[hname]->Draw("same");
      histograms[hname]->GetYaxis()->SetRangeUser(0.5,1.1);
      leg->AddEntry(histograms[hname],detector_regions[det],"lep");
   }
   leg->Draw();      
  
   // Fraction of RG-Matched Jets
   c = new TCanvas("FractionRGMatchedJetsPU","FractionRGMatchedJets PU Sample");
   c->SetLogx();
   leg = new TLegend(0.7,0.4,0.9,0.6);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   for(int det=0; det<NDetectorRegions; det++) {
      hname = detector_regions[det];
      hname.ToLower();
      hname = hname(0,1);
      hname = Form("m_frac_nj_pt_%s_match_RG_pu",hname.Data());
      if(histograms.find(hname)==histograms.end()) continue;
      setHistoColor(histograms[hname], colDetRegions[det]);
      if(det==0) {
         histograms[hname]->Draw();
      }
      else
         histograms[hname]->Draw("same");
      histograms[hname]->GetYaxis()->SetRangeUser(0.5,1.1);
      leg->AddEntry(histograms[hname],detector_regions[det],"lep");
   }
   leg->Draw();

   // Fraction of Matched Jets-PU-NPV
   if(histograms.find("m_frac_nj_pt_b_match_pu_npv10")!=histograms.end() && 
      histograms.find("m_frac_nj_pt_b_match_pu_npv20")!=histograms.end() && 
      histograms.find("m_frac_nj_pt_b_match_pu_npv30")!=histograms.end() && 
      histograms.find("m_frac_nj_pt_b_match_pu_npvO")!=histograms.end()) {
      c = new TCanvas("FractionMatchedJetsPU_NPV","FractionMatchedJets vs. npv PU Sample");
      c->SetLogx();
      setHistoColor(histograms["m_frac_nj_pt_b_match_pu_npv10"], colNpv[0]);
      setHistoColor(histograms["m_frac_nj_pt_b_match_pu_npv20"], colNpv[2]);
      setHistoColor(histograms["m_frac_nj_pt_b_match_pu_npv30"], colNpv[3]);
      setHistoColor(histograms["m_frac_nj_pt_b_match_pu_npvO"], colNpv[5]);
      histograms["m_frac_nj_pt_b_match_pu_npv10"]->GetYaxis()->SetTitle("fraction of matched jets (Barrel)");
      histograms["m_frac_nj_pt_b_match_pu_npv10"]->Draw();
      histograms["m_frac_nj_pt_b_match_pu_npv10"]->GetYaxis()->SetRangeUser(0.3,1.1);
      histograms["m_frac_nj_pt_b_match_pu_npv20"]->Draw("same");
      histograms["m_frac_nj_pt_b_match_pu_npv30"]->Draw("same");
      histograms["m_frac_nj_pt_b_match_pu_npvO"]->Draw("same");
      
      leg = new TLegend(0.7,0.4,0.9,0.6);
      leg->SetFillColor(0);
      leg->SetBorderSize(0);
      leg->AddEntry(histograms["m_frac_nj_pt_b_match_pu_npv10"],"0 < N_{PV}^{PU}<=10","lep");
      leg->AddEntry(histograms["m_frac_nj_pt_b_match_pu_npv20"],"10< N_{PV}^{PU}<=20","lep");
      leg->AddEntry(histograms["m_frac_nj_pt_b_match_pu_npv30"],"20< N_{PV}^{PU}<=30","lep");
      leg->AddEntry(histograms["m_frac_nj_pt_b_match_pu_npvO"], "30< N_{PV}^{PU}    ","lep");
      leg->Draw();
   }

   // Fraction of RG-Matched Jets-PU-NPV
   if(histograms.find("m_frac_nj_pt_b_match_RG_pu_npv10")!=histograms.end() &&
      histograms.find("m_frac_nj_pt_b_match_RG_pu_npv20")!=histograms.end() &&
      histograms.find("m_frac_nj_pt_b_match_RG_pu_npv30")!=histograms.end() &&
      histograms.find("m_frac_nj_pt_b_match_RG_pu_npvO")!=histograms.end()) {
      c = new TCanvas("FractionRGMatchedJetsPU_NPV","FractionRGMatchedJets vs. npv PU Sample");
      c->SetLogx();
      setHistoColor(histograms["m_frac_nj_pt_b_match_RG_pu_npv10"], colNpv[0]);
      setHistoColor(histograms["m_frac_nj_pt_b_match_RG_pu_npv20"], colNpv[2]);
      setHistoColor(histograms["m_frac_nj_pt_b_match_RG_pu_npv30"], colNpv[3]);
      setHistoColor(histograms["m_frac_nj_pt_b_match_RG_pu_npvO"], colNpv[5]);
      histograms["m_frac_nj_pt_b_match_RG_pu_npv10"]->Draw();
      histograms["m_frac_nj_pt_b_match_RG_pu_npv10"]->GetYaxis()->SetRangeUser(0.1,1.1);
      histograms["m_frac_nj_pt_b_match_RG_pu_npv20"]->Draw("same");
      histograms["m_frac_nj_pt_b_match_RG_pu_npv30"]->Draw("same");
      histograms["m_frac_nj_pt_b_match_RG_pu_npvO"]->Draw("same");
      
      leg = new TLegend(0.7,0.4,0.9,0.6);
      leg->SetFillColor(0);
      leg->SetBorderSize(0);
      leg->AddEntry(histograms["m_frac_nj_pt_b_match_RG_pu_npv10"],"0 < N_{PV}^{PU}<=10","lep");
      leg->AddEntry(histograms["m_frac_nj_pt_b_match_RG_pu_npv20"],"10< N_{PV}^{PU}<=20","lep");
      leg->AddEntry(histograms["m_frac_nj_pt_b_match_RG_pu_npv30"],"20< N_{PV}^{PU}<=30","lep");
      leg->AddEntry(histograms["m_frac_nj_pt_b_match_RG_pu_npvO"], "30< N_{PV}^{PU}    ","lep");
      leg->Draw();
   }

   // Fraction of Matched Jets-NOPU-NPV
   if(histograms.find("m_frac_nj_pt_b_match_nopu_npv1")!=histograms.end() &&
      histograms.find("m_frac_nj_pt_b_match_nopu_npv2")!=histograms.end() &&
      histograms.find("m_frac_nj_pt_b_match_nopu_npv3")!=histograms.end() &&
      histograms.find("m_frac_nj_pt_b_match_nopu_npvO")!=histograms.end()) {
      c = new TCanvas("FractionMatchedJetsNoPU_NPV","FractionMatchedJets vs. npv NoPU Sample");
      c->SetLogx();
      setHistoColor(histograms["m_frac_nj_pt_b_match_nopu_npv1"], colNpv[0]);
      setHistoColor(histograms["m_frac_nj_pt_b_match_nopu_npv2"], colNpv[2]);
      setHistoColor(histograms["m_frac_nj_pt_b_match_nopu_npv3"], colNpv[3]);
      setHistoColor(histograms["m_frac_nj_pt_b_match_nopu_npvO"], colNpv[5]);
      histograms["m_frac_nj_pt_b_match_nopu_npv1"]->Draw();
      histograms["m_frac_nj_pt_b_match_nopu_npv1"]->GetYaxis()->SetRangeUser(0.3,1.1);
      histograms["m_frac_nj_pt_b_match_nopu_npv2"]->Draw("same");
      histograms["m_frac_nj_pt_b_match_nopu_npv3"]->Draw("same");
      histograms["m_frac_nj_pt_b_match_nopu_npvO"]->Draw("same");
      
      leg = new TLegend(0.7,0.4,0.9,0.6);
      leg->SetFillColor(0);
      leg->SetBorderSize(0);
      leg->AddEntry(histograms["m_frac_nj_pt_b_match_nopu_npv1"],"0 < N_{PV}^{PU}<=10","lep");
      leg->AddEntry(histograms["m_frac_nj_pt_b_match_nopu_npv2"],"10< N_{PV}^{PU}<=20","lep");
      leg->AddEntry(histograms["m_frac_nj_pt_b_match_nopu_npv3"],"20< N_{PV}^{PU}<=30","lep");
      leg->AddEntry(histograms["m_frac_nj_pt_b_match_nopu_npvO"],"30< N_{PV}^{PU}    ","lep");
      leg->Draw();
   }


   // PU distribution of jets per event per NPv
   // Number of unmatched jets in pu sample minus #of unmatch jets in nopu sample
   c = new TCanvas("PUJetDistribution","PUJetDistribution");
   c->SetLogx();
   leg = new TLegend(0.7,0.65,0.9,0.85);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("m_njet_pt_npv%i_%i_unmatch",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      if(histograms.find(hname)==histograms.end()) continue;
      setHistoColor(histograms[hname],colNpv[ibin]);
      if(ibin==0) {
         histograms[hname]->Draw("E");
         histograms[hname]->GetYaxis()->SetRangeUser(0,1);
      }
      else
         histograms[hname]->Draw("sameE");
      if(ibin<npvRhoNpuBins.size()-1)
         leg->AddEntry(histograms[hname]  ,Form("%*i <= N_{PV}<%i",2,npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second+1),"lep");
      else
         leg->AddEntry(histograms[hname]  ,Form("%*i <= N_{PV}",2,npvRhoNpuBins[ibin].first),"lep");
   }
   leg->Draw();

   // INTEGRAL of PU distribution of jets per event per NPV
   // Number of unmatched jets in pu sample minus #of unmatch jets in nopu sample
  
   vector<TProfile*> pnJetPt(npvRhoNpuBins.size(),(TProfile*)0);
   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("m_njet_pt_npv%i_%i_unmatch",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      pnJetPt[ibin] = dynamic_cast<TProfile*>(histograms[hname]);
   }
   c = getCanvasIntegral("IntePUJetDistribution",algo,"Integral # PU Jets /event",pnJetPt,npvRhoNpuBins);
   c->SetLogx();
   c->Draw();
  
   c = getCanvasAverage("AvgPUJetEneDistribution",algo,"Avg. Jet Ene (GeV)",pnJetPt,npvRhoNpuBins);
   c->SetLogx();
   c->Draw();
  
   //
   //	2D histogram of jtarea diff. vs. refpt	
   //
   if(histograms.find("p_areaVsrefpt")!=histograms.end()) {
      c = new TCanvas("areaVsrefpt","areaVsrefpt");
      histograms["p_areaVsrefpt"]->Draw("colZ");
      c->SetLogx();  
      
      TProfile *p_areaVsrefpt_prof = dynamic_cast<TH2F*>(histograms["p_areaVsrefpt"])->ProfileX();
      c = new TCanvas("areaVsrefptProf","areaVsrefptProf");
      p_areaVsrefpt_prof->GetYaxis()->SetTitle("<jtarea^{PU}-jtarea^{noPU}>");
      p_areaVsrefpt_prof->GetYaxis()->SetRangeUser(-0.006,0.006);
      TGaxis::SetMaxDigits(3);
      c->SetLogx();
      p_areaVsrefpt_prof->Draw("");
   }

   //
   //	2D histogram of jtarea diff. vs. refpt	
   //
   if(histograms.find("p_areaVsoffset_1000")!=histograms.end()) {
      c = new TCanvas("areaVsOffset_1000","areaVsOffset_1000");
      histograms["p_areaVsoffset_1000"]->GetXaxis()->SetRangeUser(-100,100);
      histograms["p_areaVsoffset_1000"]->Draw("colZ");
      TProfile *p_areaVsoffset_1000_prof = dynamic_cast<TH2F*>(histograms["p_areaVsoffset_1000"])->ProfileX();
      c = new TCanvas("areaVsOffset_1000Prof","areaVsOffset_1000Prof");
      p_areaVsoffset_1000_prof->GetYaxis()->SetTitle("<jtarea^{PU}-jtarea^{noPU}>");
      p_areaVsoffset_1000_prof->GetXaxis()->SetRangeUser(-100,100);
      p_areaVsoffset_1000_prof->GetYaxis()->SetRangeUser(-0.3,0.3);
      p_areaVsoffset_1000_prof->Draw("");
   }
  
  
   //
   //	2D histogram of jtarea diff. vs. refpt	
   //
   if(histograms.find("p_areaVsoffset_30_50")!=histograms.end()) {
      c = new TCanvas("areaVsOffset_30_50","areaVsOffset_30_50");
      histograms["p_areaVsoffset_30_50"]->GetXaxis()->SetRangeUser(-100,100);
      histograms["p_areaVsoffset_30_50"]->Draw("colZ");
      TProfile *p_areaVsoffset_30_50_prof = dynamic_cast<TH2F*>(histograms["p_areaVsoffset_30_50"])->ProfileX();
      c = new TCanvas("areaVsOffset_30_50Prof","areaVsOffset_30_50Prof");
      p_areaVsoffset_30_50_prof->GetYaxis()->SetTitle("<jtarea^{PU}-jtarea^{noPU}>");
      p_areaVsoffset_30_50_prof->GetXaxis()->SetRangeUser(-100,100);
      p_areaVsoffset_30_50_prof->GetYaxis()->SetRangeUser(-0.3,0.3);
      p_areaVsoffset_30_50_prof->Draw("");
   }

  
   //	
   //	2D histogram of NPV vs Rho with 15<offset<20 PU
   //
   if(histograms.find("p_npvVsRho_offset_15_15h")!=histograms.end()) {
      c = new TCanvas("npvVsRhoOffset1515h","npvVsRhoOffset1515h"); 
      histograms["p_npvVsRho_offset_15_15h"]->Draw("COLZ"); 
   }
  
  
   //	
   //	2D histogram of NPV vs Rho with 15<offset<20 PU
   //
   if(histograms.find("p_npvVsRho_offset_15_15h")!=histograms.end()) {
      c = new TCanvas("npvVsRhoOffset1515hPeak","npvVsRhoOffset1515hPeak"); 
      histograms["p_npvVsRho_offset_15_15h"]->Draw("COLZ"); 
      //p_npvVsRho_offset_15_15h->ShowPeaks(2,"nodraw",0.2);
      //TList *functions = p_npvVsRho_offset_15_15h->GetListOfFunctions();
      //TPolyMarker *pm = (TPolyMarker*)functions->FindObject("TPolyMarker");
      Double_t peakX,peakY;
      //peakX = *(pm->GetX());peakY = *(pm->GetY());
      //cout <<peakX<<" "<<peakY<<endl;
      peakX = 14.5; peakY = 16.5;
      TH1F *p_npvVsRho_offset_15_15h_x = new TH1F("p_npvVsRho_offset_15_15h_x","p_npvVsRho_offset_15_15h_x;Rho;Events/Bin",80,0,80);
      for (int it=0;it<= histograms["p_npvVsRho_offset_15_15h"]->GetXaxis()->GetNbins() ; it++)
      {
         p_npvVsRho_offset_15_15h_x->SetBinContent(it,histograms["p_npvVsRho_offset_15_15h"]->GetBinContent(it,(int)peakY));
      }
      TH1F *p_npvVsRho_offset_15_15h_y = new TH1F("p_npvVsRho_offset_15_15h_y","p_npvVsRho_offset_15_15h_y;N_{PV};Events/Bin",80,0,80);
      for (int it=0;it<= histograms["p_npvVsRho_offset_15_15h"]->GetYaxis()->GetNbins() ; it++)
      {
         p_npvVsRho_offset_15_15h_y->SetBinContent(it,histograms["p_npvVsRho_offset_15_15h"]->GetBinContent((int)peakX,it));
      }
      c = new TCanvas("npvVsRhoOffset1515hX","npvVsRhoOffset1515hX"); 
      p_npvVsRho_offset_15_15h_x->Draw();
      p_npvVsRho_offset_15_15h_x->Fit("gaus");
      c = new TCanvas("npvVsRhoOffset1515hY","npvVsRhoOffset1515hY"); 
      p_npvVsRho_offset_15_15h_y->Draw();
      p_npvVsRho_offset_15_15h_y->Fit("gaus");
   }
  
   //
   //	Profile of dr vs. refpt for the matched jets 
   //
   if(histograms.find("p_drVsrefpt")!=histograms.end()) {
      c = new TCanvas("drVsrefptMatchedJets","drVsrefptMatchedJets"); 
      dynamic_cast<TProfile*>(histograms["p_drVsrefpt"])->SetErrorOption("s");
      c->SetLogx();
      histograms["p_drVsrefpt"]->Draw();
      histograms["p_drVsrefpt"]->GetYaxis()->SetRangeUser(0,0.3);
      histograms["p_drVsrefpt"]->GetYaxis()->SetTitle("<#DeltaR> #pm #sigma(#DeltaR)");
   }
  
   //
   //	Profile of npv and rho vs offset PU
   //
   if(histograms.find("p_npvVsOff")!=histograms.end()) {
      c = new TCanvas("npvrhoVsOffset","npvrhoVsOffset");
      TProfile *p_npvVsOff_prof = dynamic_cast<TH2F*>(histograms["p_npvVsOff"])->ProfileX();
      TProfile *p_rhoVsOff_prof = dynamic_cast<TH2F*>(histograms["p_rhoVsOff"])->ProfileX();
      p_npvVsOff_prof->SetErrorOption("s");  
      setHistoColor(p_npvVsOff_prof,colPU);
      p_npvVsOff_prof->Draw("E1");
      p_npvVsOff_prof->GetYaxis()->SetRangeUser(0,45);
      p_npvVsOff_prof->GetYaxis()->SetTitle("<X> #pm #sigma(X)");
      setHistoColor(p_rhoVsOff_prof,colNoPU);
      p_rhoVsOff_prof->SetErrorOption("s");  
      p_rhoVsOff_prof->Draw("sameE1");
      
      leg = new TLegend(0.2,0.72,0.4,0.92);
      leg->SetFillColor(0);
      leg->SetBorderSize(0);
      leg->AddEntry(p_npvVsOff_prof," N_{PV}","lep");
      leg->AddEntry(p_rhoVsOff_prof," Rho ","lep");
      leg->Draw();
   }
  
   // Profiles of npv, rho, tnpu vs themselves
   if(histograms.find("p_npvVsNpv")!=histograms.end() &&
      histograms.find("p_rhoVsRho")!=histograms.end() &&
      histograms.find("p_tnpuVsTnpu")!=histograms.end() &&
      histograms.find("p_npuVsNpu")!=histograms.end()) {
      c = new TCanvas("XVsX","XVsX {x=N_{PV},#rho,TNPU}",1600,400);
      c->Divide(4,1);
      c->cd(1);
      dynamic_cast<TProfile*>(histograms["p_npvVsNpv"])->SetErrorOption("s");
      setHistoColor(histograms["p_npvVsNpv"],colPU);
      histograms["p_npvVsNpv"]->Draw("E1");
      histograms["p_npvVsNpv"]->GetYaxis()->SetRangeUser(0,45);
      //p_npvVsNpv->GetYaxis()->SetTitle("<X> #pm #sigma(X)");
      leg = new TLegend(0.2,0.72,0.4,0.92);
      leg->SetFillColor(0);
      leg->SetBorderSize(0);
      leg->AddEntry(histograms["p_npvVsNpv"]," N_{PV}","lep");
      leg->Draw();
      c->cd(2);
      dynamic_cast<TProfile*>(histograms["p_rhoVsRho"])->SetErrorOption("s");
      setHistoColor(histograms["p_rhoVsRho"],colPU);
      histograms["p_rhoVsRho"]->Draw("E1");
      histograms["p_rhoVsRho"]->GetYaxis()->SetRangeUser(0,45);
      //p_rhoVsRho->GetYaxis()->SetTitle("<X> #pm #sigma(X)");
      leg = new TLegend(0.2,0.72,0.4,0.92);
      leg->SetFillColor(0);
      leg->SetBorderSize(0);
      leg->AddEntry(histograms["p_rhoVsRho"]," #rho","lep");
      leg->Draw();
      c->cd(3);
      dynamic_cast<TProfile*>(histograms["p_tnpuVsTnpu"])->SetErrorOption("s");
      setHistoColor(histograms["p_tnpuVsTnpu"],colPU);
      histograms["p_tnpuVsTnpu"]->Draw("E1");
      histograms["p_tnpuVsTnpu"]->GetYaxis()->SetRangeUser(0,45);
      //p_tnpuVsTnpu->GetYaxis()->SetTitle("<X> #pm #sigma(X)");
      leg = new TLegend(0.2,0.72,0.4,0.92);
      leg->SetFillColor(0);
      leg->SetBorderSize(0);
      leg->AddEntry(histograms["p_tnpuVsTnpu"]," TNPU","lep");
      leg->Draw();
      c->cd(4);
      dynamic_cast<TProfile*>(histograms["p_npuVsNpu"])->SetErrorOption("s");
      setHistoColor(histograms["p_npuVsNpu"],colPU);
      histograms["p_npuVsNpu"]->Draw("E1");
      histograms["p_npuVsNpu"]->GetYaxis()->SetRangeUser(0,45);
      //p_tnpuVsTnpu->GetYaxis()->SetTitle("<X> #pm #sigma(X)");
      leg = new TLegend(0.2,0.72,0.4,0.92);
      leg->SetFillColor(0);
      leg->SetBorderSize(0);
      leg->AddEntry(histograms["p_npuVsNpu"]," NPU","lep");
      leg->Draw();
   }

   //
   //	2D plot of npv vs offset PU
   //
   if(histograms.find("p_npvVsOff")!=histograms.end()) {
      c = new TCanvas("npvVsOffset2D","npvVsOffset2D"); 
      histograms["p_npvVsOff"]->SetTitle("2D Histogram of N_{PV} and <p_{T} Offset>_{jets}, LogZ");
      histograms["p_npvVsOff"]->GetYaxis()->SetRangeUser(0,45);
     //p_npvVsOff->GetXaxis()->SetRangeUser(0,45);
      histograms["p_npvVsOff"]->GetYaxis()->SetTitle("N_{PV}");
      histograms["p_npvVsOff"]->GetXaxis()->SetTitle("<p_{T} Offset>_{jets}");
      histograms["p_npvVsOff"]->Draw("CONTZ");
      c->SetLogz();
   }
  
   //
   //	2D plot of rho vs offset PU
   //
   if(histograms.find("p_rhoVsOff")!=histograms.end()) {
      c = new TCanvas("rhoVsOffset2D","rhoVsOffset2D"); 
      histograms["p_rhoVsOff"]->SetTitle("2D Histogram of N_{PV} and <p_{T} Offset>_{jets}, LogZ");
      histograms["p_rhoVsOff"]->GetYaxis()->SetRangeUser(0,45);
      //p_rhoVsOff->GetXaxis()->SetRangeUser(0,45);
      histograms["p_rhoVsOff"]->GetYaxis()->SetTitle("Rho");
      histograms["p_rhoVsOff"]->GetXaxis()->SetTitle("<p_{T} Offset>_{jets}");
      histograms["p_rhoVsOff"]->Draw("CONTZ");
      c->SetLogz();
   }
  
   //
   //	Profile of npv vs offset PU  breakdown into detector parts
   //
   c = new TCanvas("npvVsOffset","npvVsOffset"); 
   TProfile * hnpvOff_prof[NDetectorNames];
   for (int det=0;det<NDetectorNames;det++)
   {
      detectorAbbreviation = JetInfo::get_detector_abbreviation(detector_names[det]);
      detectorAbbreviation.ToLower();
      hname = Form("p_npvVsOff_%s",detectorAbbreviation.Data());
      hnpvOff_prof[det] = dynamic_cast<TH2D*>(histograms[hname])->ProfileX();
      hnpvOff_prof[det]->SetErrorOption("s"); 
      setHistoColor(hnpvOff_prof[det],colDet[det]);
      hnpvOff_prof[det]->GetYaxis()->SetRangeUser(0,45);
   }
   hnpvOff_prof[0]->GetYaxis()->SetTitle("<N_{PV}> #pm #sigma(N_{PV})");
   hnpvOff_prof[0]->Draw("E1");
   for (int det=1;det<NDetectorNames;det++)
   {
      hnpvOff_prof[det]->Draw("sameE1");
   }
   leg = new TLegend(0.2,0.72,0.4,0.92);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   for (int det=0;det<NDetectorNames;det++)
   {
      leg->AddEntry(hnpvOff_prof[det],detector_names[det],"lep");
   }
   leg->Draw();


   //
   //	Profile of rho vs offset PU  breakdown into detector parts
   //
   c = new TCanvas("rhoVsOffset","rhoVsOffset"); 
   TProfile * hrhoOff_prof[4];
   for (int det=0;det<NDetectorNames;det++)
   {
      detectorAbbreviation = JetInfo::get_detector_abbreviation(detector_names[det]);
      detectorAbbreviation.ToLower();
      hname = Form("p_rhoVsOff_%s",detectorAbbreviation.Data());
      hrhoOff_prof[det] = dynamic_cast<TH2D*>(histograms[hname])->ProfileX();
      hrhoOff_prof[det]->SetErrorOption("s"); 
      setHistoColor(hrhoOff_prof[det],colDet[det]);
      hrhoOff_prof[det]->GetYaxis()->SetRangeUser(0,45);
   }
   hrhoOff_prof[0]->GetYaxis()->SetTitle("<Rho> #pm #sigma(Rho)");
   hrhoOff_prof[0]->Draw("E1");
   for (int det=1;det<NDetectorNames;det++)
   {
      hrhoOff_prof[det]->Draw("sameE1");
   }
   leg = new TLegend(0.2,0.72,0.4,0.92);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   for (int det=0;det<NDetectorNames;det++)
   {
      leg->AddEntry(hrhoOff_prof[det],detector_names[det],"lep");
   }
   leg->Draw();
  
   //
   //	Jet Energy Resolution (sigma(pt/ptref)/mean(pt/ptref) vs. ptref) PU  breakdown into detector parts
   //
   vector<TH2*> hresResPt(NDetectorNames,(TH2*)0);
   for(int det=0; det<NDetectorNames; det++) {
      detectorAbbreviation = JetInfo::get_detector_abbreviation(detector_names[det]);
      detectorAbbreviation.ToLower();
      hname = Form("p_resVsrefpt_%s",detectorAbbreviation.Data());
      hresResPt[det] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResponseResolution("PUresponseResolutionVsptref",algo, "#sigma(p_{T}^{PU}/p_{T}^{GEN})/<p_{T}^{PU}/p_{T}^{GEN}>",hresResPt);
   c->Draw();
  

   for(int det=0; det<NDetectorNames; det++) {
      detectorAbbreviation = JetInfo::get_detector_abbreviation(detector_names[det]);
      detectorAbbreviation.ToLower();
      hname = Form("np_resVsrefpt_%s",detectorAbbreviation.Data());
      hresResPt[det] = dynamic_cast<TH2D*>(histograms[hname]);
   }  
   c = getCanvasResponseResolution("NoPUresponseResolutionVsptref",algo, "#sigma(p_{T}^{noPU}/p_{T}^{GEN})/<p_{T}^{noPU}/p_{T}^{GEN}>",hresResPt);
   c->Draw();
  
  

   //
   //	profile # of matchedjet vs offset
   //
   if(histograms.find("p_matchedjet_off")!=histograms.end()) { 
      c = new TCanvas("MatchedJetOffset","MatchedJetOffset"); 
      dynamic_cast<TProfile*>(histograms["p_matchedjet_off"])->SetErrorOption("s"); 
      histograms["p_matchedjet_off"]->Draw();
   }



   // Offset PT energy distribution, constructed from  pt(pu)-pt(nopu) VS NPV
   if(histograms.find("p_off_etaVsNpv")!=histograms.end()) { 
      c = new TCanvas("OffsetDistributionVsNPV","OffsetDistributionVsNPV");
      histograms["p_off_etaVsNpv"]->GetYaxis()->SetNdivisions(6);
      histograms["p_off_etaVsNpv"]->Draw("lego2");
      histograms["p_off_etaVsNpv"]->GetZaxis()->SetRangeUser(-10,50);
   }

   // Offset PT energy distribution, constructed from  pt(pu)-pt(nopu) VS Rho
   if(histograms.find("p_off_etaVsRho")!=histograms.end()) { 
      c = new TCanvas("OffsetDistributionVsRho","OffsetDistributionVsRho");
      histograms["p_off_etaVsRho"]->GetYaxis()->SetNdivisions(6);
      histograms["p_off_etaVsRho"]->Draw("lego2");
      histograms["p_off_etaVsRho"]->GetZaxis()->SetRangeUser(-10,50);
   }
  
   // Offset PT energy distribution, constructed from  pt(pu)-pt(nopu) VS PUEff
   if(histograms.find("p_off_etaVsPUEff")!=histograms.end()) { 
      c = new TCanvas("OffsetDistributionVsPUEff","OffsetDistributionVsPUEff");
      histograms["p_off_etaVsPUEff"]->Draw("lego2");
      histograms["p_off_etaVsPUEff"]->GetZaxis()->SetRangeUser(-10,50);
   }

   // Offset PT energy distribution, constructed from  pt(pu)-pt(nopu) VS GenSumPtOA
   if(histograms.find("p_off_etaVsGenSumPtOA")!=histograms.end()) { 
      c = new TCanvas("OffsetDistributionVsGenSumPtOA","OffsetDistributionVsGenSumPtOA");
      histograms["p_off_etaVsGenSumPtOA"]->GetYaxis()->SetTitle("SumPt/jetArea");
      histograms["p_off_etaVsGenSumPtOA"]->GetYaxis()->SetNdivisions(6);
      histograms["p_off_etaVsGenSumPtOA"]->GetYaxis()->SetTitleOffset(1.3);
      histograms["p_off_etaVsGenSumPtOA"]->Draw("lego2");
      histograms["p_off_etaVsGenSumPtOA"]->GetZaxis()->SetRangeUser(-10,30);
   }

   // Offset PT energy distribution, constructed from  pt(pu)-pt(nopu) VS JetPt
   if(histograms.find("p_off_etaVsJetPt")!=histograms.end()) { 
      c = new TCanvas("OffsetDistributionVsJetPt","OffsetDistributionVsJetPt");
      histograms["p_off_etaVsJetPt"]->Draw("lego2");
      c->SetLogy();
      histograms["p_off_etaVsJetPt"]->GetZaxis()->SetRangeUser(-40,100);
   }

   // OffsetOverArea PT energy distribution, constructed from  (pt(pu)-pt(nopu))/area(pu) VS JetPt
   if(histograms.find("p_offOverA_etaVsJetPt")!=histograms.end()) { 
      c = new TCanvas("OffsetOverAreaDistributionVsJetPt","OffsetOverAreaDistributionVsJetPt");
      histograms["p_offOverA_etaVsJetPt"]->Draw("lego2");
      c->SetLogy();
      histograms["p_offOverA_etaVsJetPt"]->GetZaxis()->SetRangeUser(-40,100);
   }

   // do the fitting in each eta range and return the parameters. 
   // the last parameter is the name of the file name with which all functions are saved to.
   c = getCanvasFromFittingProcedure("ParametersVsNpv",dynamic_cast<TProfile2D*>(histograms["p_off_etaVsNpv"]),"fittingFunctionsNpv_"+algo+".root");
   if(c)
      c->Draw();
   fin->cd();

   // do the fitting in each eta range and return the parameters
   c = getCanvasFromFittingProcedure("ParametersVsRho",dynamic_cast<TProfile2D*>(histograms["p_off_etaVsRho"]),"fittingFunctionsRho_"+algo+".root");
   if(c)
      c->Draw();
   fin->cd();
  
   // do the fitting in each eta range and return the parameters. 
   // the last parameter is the name of the file name with which all functions are saved to.
   c = getCanvasFromFittingProcedure("ParametersVsPUEff",dynamic_cast<TProfile2D*>(histograms["p_off_etaVsPUEff"]),"fittingFunctionsPUEff_"+algo+".root");
   if(c)
      c->Draw();
   fin->cd();
  
  
   // do the fitting in each eta range and return the parameters. 
   // the last parameter is the name of the file name with which all functions are saved to.
   c = getCanvasFromFittingProcedure("ParametersVsGenSumPtOA",dynamic_cast<TProfile2D*>(histograms["p_off_etaVsGenSumPtOA"]),"fittingFunctionsGenSumPtOA_"+algo+".root");
   if(c)
      c->Draw();
   fin->cd();

   // do the fitting in each eta range and return the parameters. 
   // the last parameter is the name of the file name with which all functions are saved to.
   c = getCanvasFromFittingProcedure("ParametersOffOverAVsJetPt",dynamic_cast<TProfile2D*>(histograms["p_offOverA_etaVsJetPt"]),"fittingFunctionsOffOverAJetPt_"+algo+".root");
//  c->SetLogy();
   if(c)
      c->Draw();
   fin->cd();


   // get the canvas from the resolution for bb
   vector<TH2*> hResRho;
   vector<TH2*> hOffRho;
   vector<TH2*> hOffPdgid;
   vector<TProfile*> pOffPF;

   clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());
   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_resnopuVsrefpt_bb_rho%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResolution("ResolutionRho_BB",algo, "#sigma(p_{T}/p_{T}^{noPU})/<p_{T}/p_{T}^{noPU}>",hResRho,0,npvRhoNpuBins);
   c->Draw();
   clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_resnopuVsrefpt_ei_rho%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResolution("ResolutionRho_EI",algo, "#sigma(p_{T}/p_{T}^{noPU})/<p_{T}/p_{T}^{noPU}>",hResRho,0,npvRhoNpuBins);
   c->Draw();
   clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_resnopuVsrefpt_eo_rho%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResolution("ResolutionRho_EO",algo,"#sigma(p_{T}/p_{T}^{noPU})/<p_{T}/p_{T}^{noPU}>",hResRho,0,npvRhoNpuBins);
   c->Draw();
   clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_resnopuVsrefpt_ff_rho%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResolution("ResolutionRho_FF",algo,"#sigma(p_{T}/p_{T}^{noPU})/<p_{T}/p_{T}^{noPU}>",hResRho,0,npvRhoNpuBins);
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   // The same plots for resolution to respect to refpt
   // get the canvas from the resolution for bb
   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_resVsrefpt_bb_rho%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResolution("ResolutionRhoRef_BB",algo,"#sigma(p_{T}/p_{T}^{GEN})/<p_{T}/p_{T}^{GEN}>",hResRho,0,npvRhoNpuBins);
   c->Draw();
   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_nopuresVsrefpt_bb_rho%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho.push_back(dynamic_cast<TH2D*>(histograms[hname]));
   }
   c = getCanvasResolution("ResolutionRhoRef_BB_PU_NoPU",algo,"#sigma(p_{T}/p_{T}^{GEN})/<p_{T}/p_{T}^{GEN}>",hResRho,0,npvRhoNpuBins);
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_resVsrefpt_ei_rho%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResolution("ResolutionRhoRef_EI",algo,"#sigma(p_{T}/p_{T}^{GEN})/<p_{T}/p_{T}^{GEN}>",hResRho,0,npvRhoNpuBins);
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_resVsrefpt_eo_rho%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResolution("ResolutionRhoRef_EO",algo,"#sigma(p_{T}/p_{T}^{GEN})/<p_{T}/p_{T}^{GEN}>",hResRho,0,npvRhoNpuBins);
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_resVsrefpt_ff_rho%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResolution("ResolutionRhoRef_FF",algo,"#sigma(p_{T}/p_{T}^{GEN})/<p_{T}/p_{T}^{GEN}>",hResRho,0,npvRhoNpuBins);
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_resVsrefpt_ei_tnpu%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResolution("ResolutionTnpuRef_EI",algo,"#sigma(p_{T}/p_{T}^{GEN})/<p_{T}/p_{T}^{GEN}>",hResRho,0,npvRhoNpuBins);
   c->Draw();
   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_nopuresVsrefpt_ei_tnpu%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho.push_back(dynamic_cast<TH2D*>(histograms[hname]));
   }
   c = getCanvasResolution("ResolutionTnpuRef_EI_PU_NoPU",algo,"#sigma(p_{T}/p_{T}^{GEN})/<p_{T}/p_{T}^{GEN}>",hResRho,0,npvRhoNpuBins);
   c->Draw();
   clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_resVsrefpt_eo_tnpu%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResolution("ResolutionTnpuRef_EO",algo,"#sigma(p_{T}/p_{T}^{GEN})/<p_{T}/p_{T}^{GEN}>",hResRho,0,npvRhoNpuBins);
   c->Draw();
   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_nopuresVsrefpt_eo_tnpu%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho.push_back(dynamic_cast<TH2D*>(histograms[hname]));
   }
   c = getCanvasResolution("ResolutionTnpuRef_EO_PU_NoPU",algo,"#sigma(p_{T}/p_{T}^{GEN})/<p_{T}/p_{T}^{GEN}>",hResRho,0,npvRhoNpuBins);
   c->Draw();
   clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_resVsrefpt_ff_tnpu%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResolution("ResolutionTnpuRef_FF",algo,"#sigma(p_{T}/p_{T}^{GEN})/<p_{T}/p_{T}^{GEN}>",hResRho,0,npvRhoNpuBins);
   c->Draw();
   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_nopuresVsrefpt_ff_tnpu%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho.push_back(dynamic_cast<TH2D*>(histograms[hname]));
   }
   c = getCanvasResolution("ResolutionTnpuRef_FF_PU_NoPU",algo,"#sigma(p_{T}/p_{T}^{GEN})/<p_{T}/p_{T}^{GEN}>",hResRho,0,npvRhoNpuBins);
   c->Draw();
   clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   //Resolution of response for hard scatter
   // get the canvas from the resolution for bb
   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_nopuresVsrefpt_bb_rho%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResolution("ResolutionRhoNoPU_BB",algo, "#sigma(p_{T}^{noPU}/p_{T}^{GEN})/<p_{T}^{noPU}/p_{T}^{GEN}>",hResRho,0,npvRhoNpuBins);
   if(c)
      c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_nopuresVsrefpt_ei_rho%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResolution("ResolutionRhoNoPU_EI",algo, "#sigma(p_{T}^{noPU}/p_{T}^{GEN})/<p_{T}^{noPU}/p_{T}^{GEN}>",hResRho,0,npvRhoNpuBins);
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_nopuresVsrefpt_eo_rho%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResolution("ResolutionRhoNoPU_EO",algo,"#sigma(p_{T}^{noPU}/p_{T}^{GEN})/<p_{T}^{noPU}/p_{T}^{GEN}>",hResRho,0,npvRhoNpuBins);
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_nopuresVsrefpt_ff_rho%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResolution("ResolutionRhoNoPU_FF",algo,"#sigma(p_{T}^{noPU}/p_{T}^{GEN})/<p_{T}^{noPU}/p_{T}^{GEN}>",hResRho,0,npvRhoNpuBins);
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());


   //Resolution of response for PU
   // get the canvas from the resolution for bb
   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_offresOrefptVsrefpt_bb_rho%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResolution("ResolutionOffResRho_BB",algo, "#sigma((p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{GEN})/<(p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{GEN}>",hResRho,0,npvRhoNpuBins);
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_offresOrefptVsrefpt_ei_rho%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResolution("ResolutionOffResRho_EI",algo, "#sigma((p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{GEN})/<(p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{GEN}>",hResRho,0,npvRhoNpuBins);
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_offresOrefptVsrefpt_eo_rho%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResolution("ResolutionOffResRho_EO",algo, "#sigma((p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{GEN})/<(p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{GEN}>",hResRho,0,npvRhoNpuBins);
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_offresOrefptVsrefpt_ff_rho%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResolution("ResolutionOffResRho_FF",algo, "#sigma((p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{GEN})/<(p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{GEN}>",hResRho,0,npvRhoNpuBins);
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,NPFcat);

   // canvases for the PF contributions
   for(int ipf=0; ipf<NPFcat; ipf++) {
      hname = Form("p_offResVsrefpt_bb_%s",PFstr[ipf].Data());
      hResRho[ipf] = dynamic_cast<TH2D*>(histograms[hname]);
      hname = Form("prof_offResVsrefpt_bb_%s",PFstr[ipf].Data());
      pOffPF[ipf] = dynamic_cast<TProfile*>(histograms[hname]);
   }
   c = getCanvasResolution("ResolutionOffRefPF_BB",algo,"#sigma(p_{T}^{PU}-p_{T}^{noPU})",hResRho,1,npvRhoNpuBins);
   c->Draw();  
   c = getGausMeanOffset("MeanOffRefPF_BB","<p_{T}^{PU}-p_{T}^{noPU}>",algo,hResRho,fixedRange,npvRhoNpuBins);
   c->Draw();
   c = getGausMeanOffsetWithSum("MeanOffRefPFWithSum_BB","<p_{T}^{PU}-p_{T}^{noPU}>",algo,hResRho,dynamic_cast<TH2D*>(histograms["p_offResVsrefpt_bb_all"]),fixedRange,npvRhoNpuBins,make_pair(minNpvRhoNpu,maxNpvRhoNpu));
   c->Draw();
   TString unassociatedCHF = inputDir+"/output_ak5pf.root";
   if(!unassociatedCHF.IsNull()) {
      pOffPF.back()->SetNameTitle("prof_offResVsrefpt_bb_unassociatedchf","prof_offResVsrefpt_bb_unassociatedchf");
      TDirectory* cdir = gDirectory;
      TFile* unassociatedCHFFile = TFile::Open(unassociatedCHF);
      if(unassociatedCHFFile) {
         pOffPF.push_back((TProfile*)(unassociatedCHFFile->Get("prof_offResVsrefpt_bb_all"))->Clone("prof_offResVsrefpt_bb_all_PFCHF"));
         pOffPF.back()->SetDirectory(0);
         pOffPF.back()->SetNameTitle("prof_offResVsrefpt_bb_all_PFCHF","prof_offResVsrefpt_bb_all_PFCHF");
         unassociatedCHFFile->Close();
         cdir->cd();
      }
   }
   c = getOffsetStack("MeanOffRefPFStack_BB","#LTp_{T,offset}#GT/#LT#mu#GT",algo,pOffPF,fixedRange,npvRhoNpuBins);
   c->Draw();
   pOffPF.pop_back();
   //c = getGausMeanOffsetStackWithSum("MeanOffRefPFStackWithSum_BB","<p_{T}^{PU}-p_{T}^{noPU}>/<#mu>",algo,hResRho,dynamic_cast<TH2D*>(histograms["p_offResVsrefpt_bb_all"]),fixedRange,npvRhoNpuBins,make_pair(minNpvRhoNpu,maxNpvRhoNpu));
   c = getOffsetStackWithSum("MeanOffRefPFStackWithSum_BB","#LTp_{T,offset}#GT/#LT#mu#GT",algo,pOffPF,dynamic_cast<TProfile*>(histograms["prof_offResVsrefpt_bb_all"]),fixedRange,npvRhoNpuBins,make_pair(minNpvRhoNpu,maxNpvRhoNpu),19.41,0.00179);
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   // get the canvas from the resolution for bb
   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_resVsrefpt_bb_rho%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
      hname = Form("p_offresVsrefpt_bb_rho%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hOffRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResolution_v2("OffResolutionRhoRef_BB",algo,"#sigma(p_{T}^{PU}-p_{T}^{noPU})/<p_{T}^{noPU}/p_{T}^{GEN}>",hResRho,hOffRho,npvRhoNpuBins);
   c->Draw();
   c = getGausMeanOffset("OffMeanrhoRef_BB","<offset> (GeV)",algo,hOffRho,fixedRange,npvRhoNpuBins);
   c->Draw();
   c = getGausMeanOffsetWithSum("OffMeanrhoRefWithSum_BB","<offset> (GeV)",algo,hOffRho,dynamic_cast<TH2D*>(histograms[Form("p_offresVsrefpt_bb_rho%i_%i",minNpvRhoNpu,maxNpvRhoNpu)]),fixedRange,npvRhoNpuBins,make_pair(minNpvRhoNpu,maxNpvRhoNpu));
   c->Draw();
   c = getGausMeanOffsetOverPtref("OffMeanOverPtrhoRef_BB","<offset>/p_{T}^{GEN}",algo,hOffRho,fixedRange,npvRhoNpuBins);
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_offresVsrefpt_bb_tnpu%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hOffRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getGausMeanOffset("OffMeantnpuRef_BB","<offset> (GeV)",algo,hOffRho,fixedRange,npvRhoNpuBins);
   c->Draw();
   c = getGausMeanOffsetWithSum("OffMeantnpuRefWithSum_BB","#LTp_{T,offset}#GT (GeV)",algo,hOffRho,dynamic_cast<TH2D*>(histograms["p_offresVsrefpt_bb_tnpu0_49"]),fixedRange,npvRhoNpuBins,make_pair(minNpvRhoNpu,maxNpvRhoNpu));
   c->Draw();
   c = getGausMeanOffsetOverPtref("OffMeanOverPttnpuRef_BB","<offset>/p_{T}^{GEN}",algo,hOffRho,fixedRange,npvRhoNpuBins);
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   // get the canvas from the resolution for bb (tnpu)
   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_resVsrefpt_bb_tnpu%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResolution("ResolutionTnpuRef_BB",algo,"#sigma(p_{T}/p_{T}^{ptcl})/#LTp_{T}/p_{T}^{ptcl}#GT",hResRho,0,npvRhoNpuBins);
   c->Draw();
   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_nopuresVsrefpt_bb_tnpu%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho.push_back(dynamic_cast<TH2D*>(histograms[hname]));
   }
   //c = getCanvasResolution("ResolutionTnpuRef_BB_PU_NoPU",algo,"#sigma(p_{T}/p_{T}^{ptcl})/#LTp_{T}/p_{T}^{ptcl}#GT",hResRho,0,npvRhoNpuBins);
   c = getCanvasResolution("ResolutionTnpuRef_BB_PU_NoPU",algo,"JER",hResRho,0,npvRhoNpuBins);
   c->Draw();
   clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());


   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_offresVsrefpt_bb_npu%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hOffRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getGausMeanOffset("OffMeannpuRef_BB","<offset> (GeV)",algo,hOffRho,fixedRange,npvRhoNpuBins);
   c->Draw();
   c = getGausMeanOffsetWithSum("OffMeannpuRefWithSum_BB","<offset> (GeV)",algo,hOffRho,dynamic_cast<TH2D*>(histograms["p_offresVsrefpt_bb_npu0_49"]),fixedRange,npvRhoNpuBins,make_pair(minNpvRhoNpu,maxNpvRhoNpu));
   c->Draw();
   c = getGausMeanOffsetOverPtref("OffMeanOverPtnpuRef_BB","<offset>/p_{T}^{GEN}",algo,hOffRho,fixedRange,npvRhoNpuBins);
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_offresVsrefpt_ei_npu%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hOffRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getGausMeanOffset("OffMeannpuRef_EI","<offset> (GeV)",algo,hOffRho,fixedRange,npvRhoNpuBins);
   c->Draw();
   c = getGausMeanOffsetWithSum("OffMeannpuRefWithSum_EI","<offset> (GeV)",algo,hOffRho,dynamic_cast<TH2D*>(histograms["p_offresVsrefpt_ei_npu0_49"]),fixedRange,npvRhoNpuBins,make_pair(minNpvRhoNpu,maxNpvRhoNpu));
   c->Draw();
   c = getGausMeanOffsetOverPtref("OffMeanOverPtnpuRef_EI","<offset>/p_{T}^{GEN}",algo,hOffRho,fixedRange,npvRhoNpuBins);
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_offresVsrefpt_eo_npu%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hOffRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getGausMeanOffset("OffMeannpuRef_EO","<offset> (GeV)",algo,hOffRho,fixedRange,npvRhoNpuBins);
   c->Draw();
   c = getGausMeanOffsetWithSum("OffMeannpuRefWithSum_EO","<offset> (GeV)",algo,hOffRho,dynamic_cast<TH2D*>(histograms["p_offresVsrefpt_eo_npu0_49"]),fixedRange,npvRhoNpuBins,make_pair(minNpvRhoNpu,maxNpvRhoNpu));
   c->Draw();
   c = getGausMeanOffsetOverPtref("OffMeanOverPtnpuRef_EO","<offset>/p_{T}^{GEN}",algo,hOffRho,fixedRange,npvRhoNpuBins);
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_offresVsrefpt_ff_npu%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hOffRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getGausMeanOffset("OffMeannpuRef_FF","<offset> (GeV)",algo,hOffRho,fixedRange,npvRhoNpuBins);
   c->Draw();
   c = getGausMeanOffsetWithSum("OffMeannpuRefWithSum_FF","<offset> (GeV)",algo,hOffRho,dynamic_cast<TH2D*>(histograms["p_offresVsrefpt_ff_npu0_49"]),fixedRange,npvRhoNpuBins,make_pair(minNpvRhoNpu,maxNpvRhoNpu));
   c->Draw();
   c = getGausMeanOffsetOverPtref("OffMeanOverPtnpuRef_FF","<offset>/p_{T}^{GEN}",algo,hOffRho,fixedRange,npvRhoNpuBins);
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   // get the canvas from the resolution for bb (npu)
   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_resVsrefpt_bb_npu%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResolution("ResolutionNpuRef_BB",algo,"#sigma(p_{T}/p_{T}^{GEN})/<p_{T}/p_{T}^{GEN}>",hResRho,0,npvRhoNpuBins);
   c->Draw();
   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_nopuresVsrefpt_bb_npu%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho.push_back(dynamic_cast<TH2D*>(histograms[hname]));
   }
   c = getCanvasResolution("ResolutionNpuRef_BB_PU_NoPU",algo,"#sigma(p_{T}/p_{T}^{GEN})/<p_{T}/p_{T}^{GEN}>",hResRho,0,npvRhoNpuBins);
   c->Draw();
   clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_resVsrefpt_ei_rho%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
      hname = Form("p_offresVsrefpt_ei_rho%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hOffRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResolution_v2("OffResolutionRhoRef_EI",algo,"#sigma(p_{T}^{PU}-p_{T}^{noPU})/<p_{T}^{noPU}/p_{T}^{GEN}>",hResRho,hOffRho,npvRhoNpuBins);
   c->Draw();
   c = getGausMeanOffset("OffMeanrhoRef_EI","<offset> (GeV)",algo,hOffRho,fixedRange,npvRhoNpuBins);
   c->Draw();
   c = getGausMeanOffsetWithSum("OffMeanrhoRefWithSum_EI","<offset> (GeV)",algo,hOffRho,dynamic_cast<TH2D*>(histograms[Form("p_offresVsrefpt_ei_rho%i_%i",minNpvRhoNpu,maxNpvRhoNpu)]),fixedRange,npvRhoNpuBins,make_pair(minNpvRhoNpu,maxNpvRhoNpu));
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_offresVsrefpt_ei_tnpu%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hOffRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getGausMeanOffset("OffMeantnpuRef_EI","<offset> (GeV)",algo,hOffRho,fixedRange,npvRhoNpuBins);
   c->Draw();
   c = getGausMeanOffsetWithSum("OffMeantnpuRefWithSum_EI","<offset> (GeV)",algo,hOffRho,dynamic_cast<TH2D*>(histograms["p_offresVsrefpt_ei_tnpu0_29"]),fixedRange,npvRhoNpuBins,make_pair(minNpvRhoNpu,maxNpvRhoNpu));
   c->Draw();
   c = getGausMeanOffsetOverPtref("OffMeanOverPttnpuRef_EI","<offset>/p_{T}^{GEN}",algo,hOffRho,fixedRange,npvRhoNpuBins);
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());
  
   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_resVsrefpt_eo_rho%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
      hname = Form("p_offresVsrefpt_eo_rho%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hOffRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResolution_v2("OffResolutionRhoRef_EO",algo,"#sigma(p_{T}^{PU}-p_{T}^{noPU})/<p_{T}^{noPU}/p_{T}^{GEN}>",hResRho,hOffRho,npvRhoNpuBins);
   c->Draw();
   c = getGausMeanOffset("OffMeanrhoRef_EO","<offset> (GeV)",algo,hOffRho,fixedRange,npvRhoNpuBins);
   c->Draw();
   c = getGausMeanOffsetWithSum("OffMeanrhoRefWithSum_EO","<offset> (GeV)",algo,hOffRho,dynamic_cast<TH2D*>(histograms[Form("p_offresVsrefpt_eo_rho%i_%i",minNpvRhoNpu,maxNpvRhoNpu)]),fixedRange,npvRhoNpuBins,make_pair(minNpvRhoNpu,maxNpvRhoNpu));
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());  
  
   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_offresVsrefpt_eo_tnpu%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hOffRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getGausMeanOffset("OffMeantnpuRef_EO","<offset> (GeV)",algo,hOffRho,fixedRange,npvRhoNpuBins);
   c->Draw();
   c = getGausMeanOffsetWithSum("OffMeantnpuRefWithSum_EO","<offset> (GeV)",algo,hOffRho,dynamic_cast<TH2D*>(histograms["p_offresVsrefpt_eo_tnpu0_29"]),fixedRange,npvRhoNpuBins,make_pair(minNpvRhoNpu,maxNpvRhoNpu));
   c->Draw();
   c = getGausMeanOffsetOverPtref("OffMeanOverPttnpuRef_EO","<offset>/p_{T}^{GEN}",algo,hOffRho,fixedRange,npvRhoNpuBins);
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_resVsrefpt_ff_rho%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
      hname = Form("p_offresVsrefpt_ff_rho%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hOffRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResolution_v2("OffResolutionRhoRef_FF",algo,"#sigma(p_{T}^{PU}-p_{T}^{noPU})/<p_{T}^{noPU}/p_{T}^{GEN}>",hResRho,hOffRho,npvRhoNpuBins);
   c->Draw(); 
   c = getGausMeanOffset("OffMeanrhoRef_FF","<offset> (GeV)",algo,hOffRho,fixedRange,npvRhoNpuBins);
   c->Draw();
   c = getGausMeanOffsetWithSum("OffMeanrhoRefWithSum_FF","<offset> (GeV)",algo,hOffRho,dynamic_cast<TH2D*>(histograms[Form("p_offresVsrefpt_ff_rho%i_%i",minNpvRhoNpu,maxNpvRhoNpu)]),fixedRange,npvRhoNpuBins,make_pair(minNpvRhoNpu,maxNpvRhoNpu));
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_offresVsrefpt_ff_tnpu%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hOffRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getGausMeanOffset("OffMeantnpuRef_FF","<offset> (GeV)",algo,hOffRho,fixedRange,npvRhoNpuBins);
   c->Draw();
   c = getGausMeanOffsetWithSum("OffMeantnpuRefWithSum_FF","<offset> (GeV)",algo,hOffRho,dynamic_cast<TH2D*>(histograms["p_offresVsrefpt_ff_tnpu0_29"]),fixedRange,npvRhoNpuBins,make_pair(minNpvRhoNpu,maxNpvRhoNpu));
   c->Draw();
   c = getGausMeanOffsetOverPtref("OffMeanOverPttnpuRef_FF","<offset>/p_{T}^{GEN}",algo,hOffRho,fixedRange,npvRhoNpuBins);
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());
 

   //Resolution of response for PU
   // get the canvas from the resolution for bb
   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_offresOrefptVsrefpt_bb_npv%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResolution("ResolutionOffResNpv_BB",algo, "#sigma((p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{GEN})/<(p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{GEN}>",hResRho,0,npvRhoNpuBins);
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_offresOrefptVsrefpt_ei_npv%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResolution("ResolutionOffResNpv_EI",algo, "#sigma((p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{GEN})/<(p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{GEN}>",hResRho,0,npvRhoNpuBins);
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_offresOrefptVsrefpt_eo_npv%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResolution("ResolutionOffResNpv_EO",algo, "#sigma((p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{GEN})/<(p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{GEN}>",hResRho,0,npvRhoNpuBins);
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_offresOrefptVsrefpt_ff_npv%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResolution("ResolutionOffResNpv_FF",algo, "#sigma((p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{GEN})/<(p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{GEN}>",hResRho,0,npvRhoNpuBins);
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

   // get the canvas from the resolution for bb
   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_resVsrefpt_bb_npv%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
      hname = Form("p_offresVsrefpt_bb_npv%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hOffRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }

   //cout <<"here0"<<endl;
   c = getCanvasResolution_v2("OffResolutionnpvRef_BB",algo,"#sigma(p_{T}^{PU}-p_{T}^{noPU})/<p_{T}^{noPU}/p_{T}^{GEN}>",hResRho,hOffRho,npvRhoNpuBins);
   c->Draw();
   //cout <<"here1"<<endl;
   //c = getResolutionNumDenom("OffResolutionnpvRef_BB_N_D","bb_npv15_19",algo, hResRho[3], hOffRho[3]);
   c->Draw();

   c = getGausMeanOffset("OffMeannpvRef_BB","<offset> (GeV)",algo,hOffRho,fixedRange,npvRhoNpuBins);
   c->Draw();
   c = getGausMeanOffsetOverPtref("OffMeanOverPtnpvRef_BB","<offset>/p_{T}^{GEN}",algo,hOffRho,fixedRange,npvRhoNpuBins);
   c->Draw();

   c = getGausMeanOffsetScale("OffMeannpvRef_BB_3035","<offset>/<offset(30<pt<35)>",algo,hOffRho,binNum3035,fixedRange,npvRhoNpuBins);
   c->Draw();
  
   c = getGausMeanOffsetScale("OffMeannpvRef_BB_2023","<offset>/<offset(20<pt<23)>",algo,hOffRho,binNum2023,fixedRange,npvRhoNpuBins);
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());
  
   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_resVsrefpt_ei_npv%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
      hname = Form("p_offresVsrefpt_ei_npv%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hOffRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
  
   c = getCanvasResolution_v2("OffResolutionnpvRef_EI",algo,"#sigma(p_{T}^{PU}-p_{T}^{noPU})/<p_{T}^{noPU}/p_{T}^{GEN}>",hResRho,hOffRho,npvRhoNpuBins);
   c->Draw();
  
   c = getGausMeanOffset("OffMeannpvRef_EI","<offset> (GeV)",algo,hOffRho,fixedRange,npvRhoNpuBins);
   c->Draw();
  
   c = getGausMeanOffsetScale("OffMeannpvRef_EI_3035","<offset>/<offset(30<pt<35)>",algo,hOffRho,binNum3035,fixedRange,npvRhoNpuBins);
   c->Draw();
  
   c = getGausMeanOffsetScale("OffMeannpvRef_EI_2023","<offset>/<offset(20<pt<23)>",algo,hOffRho,binNum2023,fixedRange,npvRhoNpuBins);
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());  

   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_resVsrefpt_eo_npv%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
      hname = Form("p_offresVsrefpt_eo_npv%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hOffRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
  
   c = getCanvasResolution_v2("OffResolutionnpvRef_EO",algo,"#sigma(p_{T}^{PU}-p_{T}^{noPU})/<p_{T}^{noPU}/p_{T}^{GEN}>",hResRho,hOffRho,npvRhoNpuBins);
   c->Draw();
   c = getGausMeanOffset("OffMeannpvRef_EO","<offset> (GeV)",algo,hOffRho,fixedRange,npvRhoNpuBins);
   c->Draw();  
  
   c = getGausMeanOffsetScale("OffMeannpvRef_EO_3035","<offset>/<offset(30<pt<35)>",algo,hOffRho,binNum3035,fixedRange,npvRhoNpuBins);
   c->Draw();
  
   c = getGausMeanOffsetScale("OffMeannpvRef_EO_2023","<offset>/<offset(20<pt<23)>",algo,hOffRho,binNum2023,fixedRange,npvRhoNpuBins);
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());  
  

   for(unsigned int ibin=0; ibin<npvRhoNpuBins.size(); ibin++) {
      hname = Form("p_resVsrefpt_ff_npv%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hResRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
      hname = Form("p_offresVsrefpt_ff_npv%i_%i",npvRhoNpuBins[ibin].first,npvRhoNpuBins[ibin].second);
      hOffRho[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getCanvasResolution_v2("OffResolutionnpvRef_FF",algo,"#sigma(p_{T}^{PU}-p_{T}^{noPU})/<p_{T}^{noPU}/p_{T}^{GEN}>",hResRho,hOffRho,npvRhoNpuBins);
   c->Draw(); 
   c = getGausMeanOffset("OffMeannpvRef_FF","<offset> (GeV)",algo,hOffRho,fixedRange,npvRhoNpuBins);
   c->Draw();
  
   c = getGausMeanOffsetScale("OffMeannpvRef_FF_3035","<offset>/<offset(30<pt<35)>",algo,hOffRho,binNum3035,fixedRange,npvRhoNpuBins);
   c->Draw();
  
   c = getGausMeanOffsetScale("OffMeannpvRef_FF_2023","<offset>/<offset(20<pt<23)>",algo,hOffRho,binNum2023,fixedRange,npvRhoNpuBins);
   c->Draw();
	clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,NPDGIDcat);  

   for(int ibin=0; ibin<NPDGIDcat; ibin++) {
      hname = Form("p_offresVsrefpt_bb_pdgid_%s",pdgidstr[ibin].Data());
      hOffPdgid[ibin] = dynamic_cast<TH2D*>(histograms[hname]);
   }
   c = getGausMeanOffset("OffMeanpdgidRef_BB","<offset> (GeV)",algo,hOffPdgid,fixedRange,npvRhoNpuBins);
   c->Draw();
   clearHistograms(hResRho,hOffRho,hOffPdgid,pOffPF,npvRhoNpuBins.size());

  
   if (writeFlag == true)
   {
      TFile* ofile = new TFile(outDir+"/canvases_synchplot_"+algo+".root","RECREATE");
      ofile->mkdir("histograms");
      // Save all canvases as image files
      cout<<"\t Saving all canvases as image files."<<endl;
      TIter next(gROOT->GetListOfCanvases());
      TObject * obj (0);
      while ( (obj = next()) ) {
         TCanvas * can = (TCanvas *) obj;
         if (can){
            TString canname = outDir+"/"+can->GetName()+ "_"+algo;
            for(unsigned int f=0; f<outputFormat.size(); f++) {
               can->Print(canname+outputFormat[f]);
            }
            ofile->cd();
            can->Write();
            ofile->cd("histograms");
            TIter nh(can->GetListOfPrimitives());
            TObject* oh (0);
            while ( (oh = nh()) ) {
               if(TString(oh->ClassName()).Contains("TH1D")) {
                  ((TH1D*)oh)->Write();
               }
               else if(TString(oh->ClassName()).Contains("TH1F")) {
                  ((TH1F*)oh)->Write();
               }
               else if(TString(oh->ClassName()).Contains("TH2D")) {
                  ((TH2D*)oh)->Write();
               }
               else if(TString(oh->ClassName()).Contains("TH2F")) {
                  ((TH2F*)oh)->Write();
               }
            }
         }
      }//while
      ofile->Close();
   }// writeFlag

}//SynchPlots




int main(int argc,char**argv)
{
   CommandLine cl;
   if (!cl.parse(argc,argv)) return 0;
   string         inputDir          = cl.getValue<string>   ("inputDir",       "./");
   string         algo1             = cl.getValue<string>   ("algo1",       "ak5pf");
   string         algo2             = cl.getValue<string>   ("algo2",       "ak5pf");
   string         outDir            = cl.getValue<string>   ("outDir",   "./images");
                  outputFormat      = cl.getVector<TString> ("outputFormat", ".eps");
   bool           fixedRange        = cl.getValue<bool>     ("fixedRange",     true);
   bool           tdr               = cl.getValue<bool>     ("tdr",           false);
   int            npvRhoNpuBinWidth = cl.getValue<int>      ("npvRhoNpuBinWidth", 5);
   int            NBinsNpvRhoNpu    = cl.getValue<int>      ("NBinsNpvRhoNpu",    6);
   int            binOffset         = cl.getValue<int>      ("binOffset",         0);
                  minNpvRhoNpu      = cl.getValue<int>      ("minNpvRhoNpu",      0);
                  maxNpvRhoNpu      = cl.getValue<int>      ("maxNpvRhoNpu",    159);

   if (!cl.check()) return 0;
   cl.print();

   if (tdr) {
      setTDRStyle();
   }

   setREStyle();

   for(int ibin=0; ibin<NBinsNpvRhoNpu; ibin++) {
      if(ibin*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1+binOffset <= maxNpvRhoNpu)
         npvRhoNpuBins.push_back(make_pair(ibin*npvRhoNpuBinWidth+binOffset,ibin*npvRhoNpuBinWidth+npvRhoNpuBinWidth-1+binOffset));
   }

   SynchPlots(inputDir, algo1, algo2, outDir, fixedRange);
}

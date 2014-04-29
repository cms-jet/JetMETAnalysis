#include <map>
#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>

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

#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetAnalyzers/interface/REStyle.h"

using namespace std;

bool writeFlag = true;

int colB = kGreen+3; // barrel
int colE = kBlue+1; // endcap
int colF = kRed+1; // forward

int colPU = kBlack;
int colNoPU = kRed+1;

int colNpv0  = kYellow+2;
int colNpv5  = kGreen+2;
int colNpv10 = kCyan+2;
int colNpv15 = kBlue+2;
int colNpv20 = kMagenta+2;
int colNpv25 = kRed+2;

int colnJ  = kYellow+2;
int colqJ  = kGreen+2;
int colcJ  = kCyan+2;
int colbJ  = kBlue+2;
int colgJ  = kRed+2;
int colaJ  = kBlack;
int colaqJ = kMagenta+2;

int colDet[4] = {kGreen+3,kBlue+1,kYellow+2,kRed+1}; //bb,ei,eo,ff

int binNum3035 = 15;
int binNum2023 = 12;

TString detName[4] ={"Barrel","Inner Endcap","Outer Endcap","Forward"};

void setHistoColor(TH1* h, int c){
   h->SetMarkerColor(c);
   h->SetLineColor(c);
}
#include "JetMETAnalysis/JetUtilities/src/SynchFittingProcedure.hh"

void SynchPlots(TString calgo1="ak5pf",TString calgo2="ak5pf", TString outDir = "./images", bool fixedRange = true){
   TString algo1(calgo1);
   TString algo2(calgo2);
   TString algo12 = algo1+"_"+algo2;
   if (algo1.EqualTo(algo2)) 
      algo12 = algo1;
   TString algo(algo12);
   TString filename=Form("output_%s.root",algo.Data());
   cout<<"Producing plots from file "<<filename<<endl;
   TFile *fin = new TFile(filename);

   // Just aux pointers
   TCanvas * c = 0;
   TLegend * leg = 0;

// Reading in histograms from root files
//==================================================================================
//   TH2D	*	g_nj	=	(	TH2D	*)	fin->Get("g_nj");
//   TH2D	*	g_npv	=	(	TH2D	*)	fin->Get("g_npv");
//   TH2D	*	g_rho	=	(	TH2D	*)	fin->Get("g_rho");
//   TH2D	*	g_pthat	=	(	TH2D	* )	fin->Get("g_pthat");
//   TProfile	*	g_deltaNpv	=	(	TProfile	* )	fin->Get("g_deltaNpv");
   TH1D	*	m_refpt_diff	=	(	TH1D	* )	fin->Get("m_refpt_diff");
   TH1D	*	m_refpdgid_diff	=	(	TH1D	* )	fin->Get("m_refpdgid_diff");
   TProfile	*	m_deltaPthat	=	(	TProfile	* )	fin->Get("m_deltaPthat");
   TH1D	*	m_njet_pt_pu	=	(	TH1D	* )	fin->Get("m_njet_pt_pu");
   TH1D	*	m_njet_pt_nopu	=	(	TH1D	* )	fin->Get("m_njet_pt_nopu");
   TProfile	*	m_frac_nj_pt_b_match_pu	=	(	TProfile	* )	fin->Get("m_frac_nj_pt_b_match_pu");
   TProfile	*	m_frac_nj_pt_b_match_nopu	=	(	TProfile	* )	fin->Get("m_frac_nj_pt_b_match_nopu");
   TProfile	*	m_frac_nj_pt_e_match_pu	=	(	TProfile	* )	fin->Get("m_frac_nj_pt_e_match_pu");
   TProfile	*	m_frac_nj_pt_e_match_nopu	=	(	TProfile	* )	fin->Get("m_frac_nj_pt_e_match_nopu");
   TProfile	*	m_frac_nj_pt_f_match_pu	=	(	TProfile	* )	fin->Get("m_frac_nj_pt_f_match_pu");
   TProfile	*	m_frac_nj_pt_f_match_nopu	=	(	TProfile	* )	fin->Get("m_frac_nj_pt_f_match_nopu");
   TProfile	*	m_frac_nj_pt_b_match_pu_npv10	=	(	TProfile	* )	fin->Get("m_frac_nj_pt_b_match_pu_npv10");
   TProfile	*	m_frac_nj_pt_b_match_nopu_npv1	=	(	TProfile	* )	fin->Get("m_frac_nj_pt_b_match_nopu_npv1");
   TProfile	*	m_frac_nj_pt_b_match_pu_npv20	=	(	TProfile	* )	fin->Get("m_frac_nj_pt_b_match_pu_npv20");
   TProfile	*	m_frac_nj_pt_b_match_nopu_npv2	=	(	TProfile	* )	fin->Get("m_frac_nj_pt_b_match_nopu_npv2");
   TProfile	*	m_frac_nj_pt_b_match_pu_npv30	=	(	TProfile	* )	fin->Get("m_frac_nj_pt_b_match_pu_npv30");
   TProfile	*	m_frac_nj_pt_b_match_nopu_npv3	=	(	TProfile	* )	fin->Get("m_frac_nj_pt_b_match_nopu_npv3");
   TProfile	*	m_frac_nj_pt_b_match_pu_npvO	=	(	TProfile	* )	fin->Get("m_frac_nj_pt_b_match_pu_npvO");
   TProfile	*	m_frac_nj_pt_b_match_nopu_npvO	=	(	TProfile	* )	fin->Get("m_frac_nj_pt_b_match_nopu_npvO");
   TProfile	*	m_frac_nj_pt_b_match_RG_pu	=	(	TProfile	* )	fin->Get("m_frac_nj_pt_b_match_RG_pu");
   TProfile	*	m_frac_nj_pt_e_match_RG_pu	=	(	TProfile	* )	fin->Get("m_frac_nj_pt_e_match_RG_pu");
   TProfile	*	m_frac_nj_pt_f_match_RG_pu	=	(	TProfile	* )	fin->Get("m_frac_nj_pt_f_match_RG_pu");
   TProfile	*	m_frac_nj_pt_b_match_RG_nopu	=	(	TProfile	* )	fin->Get("m_frac_nj_pt_b_match_RG_nopu");
   TProfile	*	m_frac_nj_pt_e_match_RG_nopu	=	(	TProfile	* )	fin->Get("m_frac_nj_pt_e_match_RG_nopu");
   TProfile	*	m_frac_nj_pt_f_match_RG_nopu	=	(	TProfile	* )	fin->Get("m_frac_nj_pt_f_match_RG_nopu");
   TProfile	*	m_frac_nj_pt_b_match_RG_pu_npv10	=	(	TProfile	* )	fin->Get("m_frac_nj_pt_b_match_RG_pu_npv10");
   TProfile	*	m_frac_nj_pt_b_match_RG_pu_npv20	=	(	TProfile	* )	fin->Get("m_frac_nj_pt_b_match_RG_pu_npv20");
   TProfile	*	m_frac_nj_pt_b_match_RG_pu_npv30	=	(	TProfile	* )	fin->Get("m_frac_nj_pt_b_match_RG_pu_npv30");
   TProfile	*	m_frac_nj_pt_b_match_RG_pu_npvO	=	(	TProfile	* )	fin->Get("m_frac_nj_pt_b_match_RG_pu_npvO");
   TProfile	*	m_njet_pt_npv0_4_unmatch	=	(	TProfile	* )	fin->Get("m_njet_pt_npv0_4_unmatch");
   TProfile	*	m_njet_pt_npv5_9_unmatch	=	(	TProfile	* )	fin->Get("m_njet_pt_npv5_9_unmatch");
   TProfile	*	m_njet_pt_npv10_14_unmatch	=	(	TProfile	* )	fin->Get("m_njet_pt_npv10_14_unmatch");
   TProfile	*	m_njet_pt_npv15_19_unmatch	=	(	TProfile	* )	fin->Get("m_njet_pt_npv15_19_unmatch");
   TProfile	*	m_njet_pt_npv20_24_unmatch	=	(	TProfile	* )	fin->Get("m_njet_pt_npv20_24_unmatch");
   TProfile	*	m_njet_pt_npv25_29_unmatch	=	(	TProfile	* )	fin->Get("m_njet_pt_npv25_29_unmatch");
//   TProfile2D	*	m_njet_etaVspt_npv0_4_unmatch	=	(	TProfile2D	* )	fin->Get("m_njet_etaVspt_npv0_4_unmatch");
//   TProfile2D	*	m_njet_etaVspt_npv5_9_unmatch	=	(	TProfile2D	* )	fin->Get("m_njet_etaVspt_npv5_9_unmatch");
//   TProfile2D	*	m_njet_etaVspt_npv10_14_unmatch	=	(	TProfile2D	* )	fin->Get("m_njet_etaVspt_npv10_14_unmatch");
//   TProfile2D	*	m_njet_etaVspt_npv15_19_unmatch	=	(	TProfile2D	* )	fin->Get("m_njet_etaVspt_npv15_19_unmatch");
//   TProfile2D	*	m_njet_etaVspt_npv20_24_unmatch	=	(	TProfile2D	* )	fin->Get("m_njet_etaVspt_npv20_24_unmatch");
//   TProfile2D	*	m_njet_etaVspt_npv25_29_unmatch	=	(	TProfile2D	* )	fin->Get("m_njet_etaVspt_npv25_29_unmatch");
   TProfile2D	*	p_off_etaVsNpv	=	(	TProfile2D	* )	fin->Get("p_off_etaVsNpv");
   TProfile2D	*	p_off_etaVsRho	=	(	TProfile2D	* )	fin->Get("p_off_etaVsRho");
   TProfile2D	*	p_off_etaVsPUEff	=	(	TProfile2D	* )	fin->Get("p_off_etaVsPUEff");
   TProfile2D	*	p_off_etaVsGenSumPtOA	=	(	TProfile2D	* )	fin->Get("p_off_etaVsGenSumPtOA");
   TProfile2D	*	p_off_etaVsJetPt	=	(	TProfile2D	* )	fin->Get("p_off_etaVsJetPt");
   TProfile2D	*	p_offOverA_etaVsJetPt	=	(	TProfile2D	* )	fin->Get("p_offOverA_etaVsJetPt");
//   TProfile3D	*	p_offOverA_etaVsTnpusVsJetPt	=	(	TProfile3D	* )	fin->Get("p_offOverA_etaVsTnpusVsJetPt");
//   TProfile3D	*	p_PtAve_etaVsTnpusVsJetPt	=	(	TProfile3D	* )	fin->Get("p_PtAve_etaVsTnpusVsJetPt");
//   TProfile3D	*	p_RhoAve_etaVsTnpusVsJetPt	=	(	TProfile3D	* )	fin->Get("p_RhoAve_etaVsTnpusVsJetPt");
//   TProfile3D	*	p_offOverA_etaVsRhoVsJetPt	=	(	TProfile3D	* )	fin->Get("p_offOverA_etaVsRhoVsJetPt");
//   TProfile3D	*	p_offOverA_etaVsNPVVsJetPt	=	(	TProfile3D	* )	fin->Get("p_offOverA_etaVsNPVVsJetPt");
//   TProfile3D	*	p_offOverA_etaVsN_RVsJetPt	=	(	TProfile3D	* )	fin->Get("p_offOverA_etaVsN_RVsJetPt");
//   TProfile3D	*	p_PtAve_etaVsRhoVsJetPt	=	(	TProfile3D	* )	fin->Get("p_PtAve_etaVsRhoVsJetPt");
//   TProfile3D	*	p_PtAve_etaVsNPVVsJetPt	=	(	TProfile3D	* )	fin->Get("p_PtAve_etaVsNPVVsJetPt");
//   TProfile3D	*	p_PtAve_etaVsN_RVsJetPt	=	(	TProfile3D	* )	fin->Get("p_PtAve_etaVsN_RVsJetPt");
   TH2F	*	p_npvVsOff	=	(	TH2F	* )	fin->Get("p_npvVsOff");
   TH2F	*	p_rhoVsOff	=	(	TH2F	* )	fin->Get("p_rhoVsOff");
   TProfile * p_rhoVsRho   = ( TProfile * ) fin->Get("p_rhoVsRho");
   TProfile * p_npvVsNpv   = ( TProfile * ) fin->Get("p_npvVsNpv");
   TProfile * p_tnpuVsTnpu = ( TProfile * ) fin->Get("p_tnpuVsTnpu");
   TH2D	*	p_offResVsrefpt_bb_chf	=	(	TH2D	* )	fin->Get("p_offResVsrefpt_bb_chf");
   TH2D	*	p_offResVsrefpt_bb_nhf	=	(	TH2D	* )	fin->Get("p_offResVsrefpt_bb_nhf");
   TH2D	*	p_offResVsrefpt_bb_nef	=	(	TH2D	* )	fin->Get("p_offResVsrefpt_bb_nef");
   TH2D	*	p_offResVsrefpt_bb_cef	=	(	TH2D	* )	fin->Get("p_offResVsrefpt_bb_cef");
   TH2D	*	p_offResVsrefpt_bb_hfhf	=	(	TH2D	* )	fin->Get("p_offResVsrefpt_bb_hfhf");
   TH2D	*	p_offResVsrefpt_bb_hfef	=	(	TH2D	* )	fin->Get("p_offResVsrefpt_bb_hfef");
   TH2D	*	p_offResVsrefpt_bb_all	=	(	TH2D	* )	fin->Get("p_offResVsrefpt_bb_all");
   TH2D	*	p_npvVsOff_bb	=	(	TH2D	* )	fin->Get("p_npvVsOff_bb");
   TH2D	*	p_rhoVsOff_bb	=	(	TH2D	* )	fin->Get("p_rhoVsOff_bb");
   TH2D	*	p_npvVsOff_ei	=	(	TH2D	* )	fin->Get("p_npvVsOff_ei");
   TH2D	*	p_rhoVsOff_ei	=	(	TH2D	* )	fin->Get("p_rhoVsOff_ei");
   TH2D	*	p_npvVsOff_eo	=	(	TH2D	* )	fin->Get("p_npvVsOff_eo");
   TH2D	*	p_rhoVsOff_eo	=	(	TH2D	* )	fin->Get("p_rhoVsOff_eo");
   TH2D	*	p_npvVsOff_ff	=	(	TH2D	* )	fin->Get("p_npvVsOff_ff");
   TH2D	*	p_rhoVsOff_ff	=	(	TH2D	* )	fin->Get("p_rhoVsOff_ff");
   TProfile	*	p_matchedjet_off	=	(	TProfile	* )	fin->Get("p_matchedjet_off");
   TH2F	*	p_npvVsRho_offset_15_15h	=	(	TH2F	* )	fin->Get("p_npvVsRho_offset_15_15h");
//   TH3F	*	p_rho_npv_refpt_BB	=	(	TH3F	* )	fin->Get("p_rho_npv_refpt_BB");
//   TProfile3D	*	p_offsetOA_rho_npv_refpt_BB	=	(	TProfile3D	* )	fin->Get("p_offsetOA_rho_npv_refpt_BB");
   TProfile	*	p_drVsrefpt	=	(	TProfile	* )	fin->Get("p_drVsrefpt");
   TH2F	*	p_areaVsrefpt	=	(	TH2F	* )	fin->Get("p_areaVsrefpt");
   TH2F	*	p_areaVsoffset_1000	=	(	TH2F	* )	fin->Get("p_areaVsoffset_1000");
   TH2F	*	p_areaVsoffset_30_50	=	(	TH2F	* )	fin->Get("p_areaVsoffset_30_50");
//   TH2D	*	p_resnopuVsrefpt_bb_npv0_4	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_bb_npv0_4");
//   TH2D	*	p_resnopuVsrefpt_bb_npv5_9	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_bb_npv5_9");
//   TH2D	*	p_resnopuVsrefpt_bb_npv10_14	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_bb_npv10_14");
//   TH2D	*	p_resnopuVsrefpt_bb_npv15_19	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_bb_npv15_19");
//   TH2D	*	p_resnopuVsrefpt_bb_npv20_24	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_bb_npv20_24");
//   TH2D	*	p_resnopuVsrefpt_bb_npv25_29	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_bb_npv25_29");
//   TH2D	*	p_resnopuVsrefpt_ei_npv0_4	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_ei_npv0_4");
//   TH2D	*	p_resnopuVsrefpt_ei_npv5_9	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_ei_npv5_9");
//   TH2D	*	p_resnopuVsrefpt_ei_npv10_14	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_ei_npv10_14");
//   TH2D	*	p_resnopuVsrefpt_ei_npv15_19	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_ei_npv15_19");
//   TH2D	*	p_resnopuVsrefpt_ei_npv20_24	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_ei_npv20_24");
//   TH2D	*	p_resnopuVsrefpt_ei_npv25_29	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_ei_npv25_29");
//   TH2D	*	p_resnopuVsrefpt_eo_npv0_4	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_eo_npv0_4");
//   TH2D	*	p_resnopuVsrefpt_eo_npv5_9	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_eo_npv5_9");
//   TH2D	*	p_resnopuVsrefpt_eo_npv10_14	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_eo_npv10_14");
//   TH2D	*	p_resnopuVsrefpt_eo_npv15_19	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_eo_npv15_19");
//   TH2D	*	p_resnopuVsrefpt_eo_npv20_24	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_eo_npv20_24");
//   TH2D	*	p_resnopuVsrefpt_eo_npv25_29	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_eo_npv25_29");
//   TH2D	*	p_resnopuVsrefpt_ff_npv0_4	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_ff_npv0_4");
//   TH2D	*	p_resnopuVsrefpt_ff_npv5_9	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_ff_npv5_9");
//   TH2D	*	p_resnopuVsrefpt_ff_npv10_14	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_ff_npv10_14");
//   TH2D	*	p_resnopuVsrefpt_ff_npv15_19	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_ff_npv15_19");
//   TH2D	*	p_resnopuVsrefpt_ff_npv20_24	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_ff_npv20_24");
//   TH2D	*	p_resnopuVsrefpt_ff_npv25_29	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_ff_npv25_29");
   TH2D	*	p_resnopuVsrefpt_bb_rho0_4	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_bb_rho0_4");
   TH2D	*	p_resnopuVsrefpt_bb_rho5_9	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_bb_rho5_9");
   TH2D	*	p_resnopuVsrefpt_bb_rho10_14	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_bb_rho10_14");
   TH2D	*	p_resnopuVsrefpt_bb_rho15_19	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_bb_rho15_19");
   TH2D	*	p_resnopuVsrefpt_bb_rho20_24	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_bb_rho20_24");
   TH2D	*	p_resnopuVsrefpt_bb_rho25_29	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_bb_rho25_29");
   TH2D	*	p_resnopuVsrefpt_ei_rho0_4	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_ei_rho0_4");
   TH2D	*	p_resnopuVsrefpt_ei_rho5_9	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_ei_rho5_9");
   TH2D	*	p_resnopuVsrefpt_ei_rho10_14	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_ei_rho10_14");
   TH2D	*	p_resnopuVsrefpt_ei_rho15_19	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_ei_rho15_19");
   TH2D	*	p_resnopuVsrefpt_ei_rho20_24	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_ei_rho20_24");
   TH2D	*	p_resnopuVsrefpt_ei_rho25_29	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_ei_rho25_29");
   TH2D	*	p_resnopuVsrefpt_eo_rho0_4	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_eo_rho0_4");
   TH2D	*	p_resnopuVsrefpt_eo_rho5_9	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_eo_rho5_9");
   TH2D	*	p_resnopuVsrefpt_eo_rho10_14	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_eo_rho10_14");
   TH2D	*	p_resnopuVsrefpt_eo_rho15_19	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_eo_rho15_19");
   TH2D	*	p_resnopuVsrefpt_eo_rho20_24	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_eo_rho20_24");
   TH2D	*	p_resnopuVsrefpt_eo_rho25_29	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_eo_rho25_29");
   TH2D	*	p_resnopuVsrefpt_ff_rho0_4	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_ff_rho0_4");
   TH2D	*	p_resnopuVsrefpt_ff_rho5_9	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_ff_rho5_9");
   TH2D	*	p_resnopuVsrefpt_ff_rho10_14	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_ff_rho10_14");
   TH2D	*	p_resnopuVsrefpt_ff_rho15_19	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_ff_rho15_19");
   TH2D	*	p_resnopuVsrefpt_ff_rho20_24	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_ff_rho20_24");
   TH2D	*	p_resnopuVsrefpt_ff_rho25_29	=	(	TH2D	* )	fin->Get("p_resnopuVsrefpt_ff_rho25_29");
   TH2D  *  p_nopuresVsrefpt_bb_rho0_4    =  (  TH2D  * )   fin->Get("p_nopuresVsrefpt_bb_rho0_4");
   TH2D  *  p_nopuresVsrefpt_bb_rho5_9    =  (  TH2D  * )   fin->Get("p_nopuresVsrefpt_bb_rho5_9");
   TH2D  *  p_nopuresVsrefpt_bb_rho10_14  =  (  TH2D  * )   fin->Get("p_nopuresVsrefpt_bb_rho10_14");
   TH2D  *  p_nopuresVsrefpt_bb_rho15_19  =  (  TH2D  * )   fin->Get("p_nopuresVsrefpt_bb_rho15_19");
   TH2D  *  p_nopuresVsrefpt_bb_rho20_24  =  (  TH2D  * )   fin->Get("p_nopuresVsrefpt_bb_rho20_24");
   TH2D  *  p_nopuresVsrefpt_bb_rho25_29  =  (  TH2D  * )   fin->Get("p_nopuresVsrefpt_bb_rho25_29");
   TH2D  *  p_nopuresVsrefpt_ei_rho0_4    =  (  TH2D  * )   fin->Get("p_nopuresVsrefpt_ei_rho0_4");
   TH2D  *  p_nopuresVsrefpt_ei_rho5_9    =  (  TH2D  * )   fin->Get("p_nopuresVsrefpt_ei_rho5_9");
   TH2D  *  p_nopuresVsrefpt_ei_rho10_14  =  (  TH2D  * )   fin->Get("p_nopuresVsrefpt_ei_rho10_14");
   TH2D  *  p_nopuresVsrefpt_ei_rho15_19  =  (  TH2D  * )   fin->Get("p_nopuresVsrefpt_ei_rho15_19");
   TH2D  *  p_nopuresVsrefpt_ei_rho20_24  =  (  TH2D  * )   fin->Get("p_nopuresVsrefpt_ei_rho20_24");
   TH2D  *  p_nopuresVsrefpt_ei_rho25_29  =  (  TH2D  * )   fin->Get("p_nopuresVsrefpt_ei_rho25_29");
   TH2D  *  p_nopuresVsrefpt_eo_rho0_4    =  (  TH2D  * )   fin->Get("p_nopuresVsrefpt_eo_rho0_4");
   TH2D  *  p_nopuresVsrefpt_eo_rho5_9    =  (  TH2D  * )   fin->Get("p_nopuresVsrefpt_eo_rho5_9");
   TH2D  *  p_nopuresVsrefpt_eo_rho10_14  =  (  TH2D  * )   fin->Get("p_nopuresVsrefpt_eo_rho10_14");
   TH2D  *  p_nopuresVsrefpt_eo_rho15_19  =  (  TH2D  * )   fin->Get("p_nopuresVsrefpt_eo_rho15_19");
   TH2D  *  p_nopuresVsrefpt_eo_rho20_24  =  (  TH2D  * )   fin->Get("p_nopuresVsrefpt_eo_rho20_24");
   TH2D  *  p_nopuresVsrefpt_eo_rho25_29  =  (  TH2D  * )   fin->Get("p_nopuresVsrefpt_eo_rho25_29");
   TH2D  *  p_nopuresVsrefpt_ff_rho0_4    =  (  TH2D  * )   fin->Get("p_nopuresVsrefpt_ff_rho0_4");
   TH2D  *  p_nopuresVsrefpt_ff_rho5_9    =  (  TH2D  * )   fin->Get("p_nopuresVsrefpt_ff_rho5_9");
   TH2D  *  p_nopuresVsrefpt_ff_rho10_14  =  (  TH2D  * )   fin->Get("p_nopuresVsrefpt_ff_rho10_14");
   TH2D  *  p_nopuresVsrefpt_ff_rho15_19  =  (  TH2D  * )   fin->Get("p_nopuresVsrefpt_ff_rho15_19");
   TH2D  *  p_nopuresVsrefpt_ff_rho20_24  =  (  TH2D  * )   fin->Get("p_nopuresVsrefpt_ff_rho20_24");
   TH2D  *  p_nopuresVsrefpt_ff_rho25_29  =  (  TH2D  * )   fin->Get("p_nopuresVsrefpt_ff_rho25_29");
   TH2D	*	p_resVsrefpt_bb_npv0_4	=	(	TH2D	* )	fin->Get("p_resVsrefpt_bb_npv0_4");
   TH2D	*	p_resVsrefpt_bb_npv5_9	=	(	TH2D	* )	fin->Get("p_resVsrefpt_bb_npv5_9");
   TH2D	*	p_resVsrefpt_bb_npv10_14	=	(	TH2D	* )	fin->Get("p_resVsrefpt_bb_npv10_14");
   TH2D	*	p_resVsrefpt_bb_npv15_19	=	(	TH2D	* )	fin->Get("p_resVsrefpt_bb_npv15_19");
   TH2D	*	p_resVsrefpt_bb_npv20_24	=	(	TH2D	* )	fin->Get("p_resVsrefpt_bb_npv20_24");
   TH2D	*	p_resVsrefpt_bb_npv25_29	=	(	TH2D	* )	fin->Get("p_resVsrefpt_bb_npv25_29");
   TH2D	*	p_resVsrefpt_ei_npv0_4	=	(	TH2D	* )	fin->Get("p_resVsrefpt_ei_npv0_4");
   TH2D	*	p_resVsrefpt_ei_npv5_9	=	(	TH2D	* )	fin->Get("p_resVsrefpt_ei_npv5_9");
   TH2D	*	p_resVsrefpt_ei_npv10_14	=	(	TH2D	* )	fin->Get("p_resVsrefpt_ei_npv10_14");
   TH2D	*	p_resVsrefpt_ei_npv15_19	=	(	TH2D	* )	fin->Get("p_resVsrefpt_ei_npv15_19");
   TH2D	*	p_resVsrefpt_ei_npv20_24	=	(	TH2D	* )	fin->Get("p_resVsrefpt_ei_npv20_24");
   TH2D	*	p_resVsrefpt_ei_npv25_29	=	(	TH2D	* )	fin->Get("p_resVsrefpt_ei_npv25_29");
   TH2D	*	p_resVsrefpt_eo_npv0_4	=	(	TH2D	* )	fin->Get("p_resVsrefpt_eo_npv0_4");
   TH2D	*	p_resVsrefpt_eo_npv5_9	=	(	TH2D	* )	fin->Get("p_resVsrefpt_eo_npv5_9");
   TH2D	*	p_resVsrefpt_eo_npv10_14	=	(	TH2D	* )	fin->Get("p_resVsrefpt_eo_npv10_14");
   TH2D	*	p_resVsrefpt_eo_npv15_19	=	(	TH2D	* )	fin->Get("p_resVsrefpt_eo_npv15_19");
   TH2D	*	p_resVsrefpt_eo_npv20_24	=	(	TH2D	* )	fin->Get("p_resVsrefpt_eo_npv20_24");
   TH2D	*	p_resVsrefpt_eo_npv25_29	=	(	TH2D	* )	fin->Get("p_resVsrefpt_eo_npv25_29");
   TH2D	*	p_resVsrefpt_ff_npv0_4	=	(	TH2D	* )	fin->Get("p_resVsrefpt_ff_npv0_4");
   TH2D	*	p_resVsrefpt_ff_npv5_9	=	(	TH2D	* )	fin->Get("p_resVsrefpt_ff_npv5_9");
   TH2D	*	p_resVsrefpt_ff_npv10_14	=	(	TH2D	* )	fin->Get("p_resVsrefpt_ff_npv10_14");
   TH2D	*	p_resVsrefpt_ff_npv15_19	=	(	TH2D	* )	fin->Get("p_resVsrefpt_ff_npv15_19");
   TH2D	*	p_resVsrefpt_ff_npv20_24	=	(	TH2D	* )	fin->Get("p_resVsrefpt_ff_npv20_24");
   TH2D	*	p_resVsrefpt_ff_npv25_29	=	(	TH2D	* )	fin->Get("p_resVsrefpt_ff_npv25_29");
   TH2D	*	p_resVsrefpt_bb	=	(	TH2D	* )	fin->Get("p_resVsrefpt_bb");
   TH2D	*	p_resVsrefpt_ei	=	(	TH2D	* )	fin->Get("p_resVsrefpt_ei");
   TH2D	*	p_resVsrefpt_eo	=	(	TH2D	* )	fin->Get("p_resVsrefpt_eo");
   TH2D	*	p_resVsrefpt_ff	=	(	TH2D	* )	fin->Get("p_resVsrefpt_ff");
   TH2D	*	np_resVsrefpt_bb	=	(	TH2D	* )	fin->Get("np_resVsrefpt_bb");
   TH2D	*	np_resVsrefpt_ei	=	(	TH2D	* )	fin->Get("np_resVsrefpt_ei");
   TH2D	*	np_resVsrefpt_eo	=	(	TH2D	* )	fin->Get("np_resVsrefpt_eo");
   TH2D	*	np_resVsrefpt_ff	=	(	TH2D	* )	fin->Get("np_resVsrefpt_ff");
   TH2D	*	p_offresVsrefpt_bb_npv0_4	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_bb_npv0_4");
   TH2D	*	p_offresVsrefpt_bb_npv5_9	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_bb_npv5_9");
   TH2D	*	p_offresVsrefpt_bb_npv10_14	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_bb_npv10_14");
   TH2D	*	p_offresVsrefpt_bb_npv15_19	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_bb_npv15_19");
   TH2D	*	p_offresVsrefpt_bb_npv20_24	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_bb_npv20_24");
   TH2D	*	p_offresVsrefpt_bb_npv25_29	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_bb_npv25_29");
   TH2D	*	p_offresVsrefpt_ei_npv0_4	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_ei_npv0_4");
   TH2D	*	p_offresVsrefpt_ei_npv5_9	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_ei_npv5_9");
   TH2D	*	p_offresVsrefpt_ei_npv10_14	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_ei_npv10_14");
   TH2D	*	p_offresVsrefpt_ei_npv15_19	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_ei_npv15_19");
   TH2D	*	p_offresVsrefpt_ei_npv20_24	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_ei_npv20_24");
   TH2D	*	p_offresVsrefpt_ei_npv25_29	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_ei_npv25_29");
   TH2D	*	p_offresVsrefpt_eo_npv0_4	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_eo_npv0_4");
   TH2D	*	p_offresVsrefpt_eo_npv5_9	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_eo_npv5_9");
   TH2D	*	p_offresVsrefpt_eo_npv10_14	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_eo_npv10_14");
   TH2D	*	p_offresVsrefpt_eo_npv15_19	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_eo_npv15_19");
   TH2D	*	p_offresVsrefpt_eo_npv20_24	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_eo_npv20_24");
   TH2D	*	p_offresVsrefpt_eo_npv25_29	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_eo_npv25_29");
   TH2D	*	p_offresVsrefpt_ff_npv0_4	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_ff_npv0_4");
   TH2D	*	p_offresVsrefpt_ff_npv5_9	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_ff_npv5_9");
   TH2D	*	p_offresVsrefpt_ff_npv10_14	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_ff_npv10_14");
   TH2D	*	p_offresVsrefpt_ff_npv15_19	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_ff_npv15_19");
   TH2D	*	p_offresVsrefpt_ff_npv20_24	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_ff_npv20_24");
   TH2D	*	p_offresVsrefpt_ff_npv25_29	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_ff_npv25_29");
   TH2D  *  p_offresOrefptVsrefpt_bb_npv0_4     =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_bb_npv0_4");
   TH2D  *  p_offresOrefptVsrefpt_bb_npv5_9     =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_bb_npv5_9");
   TH2D  *  p_offresOrefptVsrefpt_bb_npv10_14   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_bb_npv10_14");
   TH2D  *  p_offresOrefptVsrefpt_bb_npv15_19   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_bb_npv15_19");
   TH2D  *  p_offresOrefptVsrefpt_bb_npv20_24   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_bb_npv20_24");
   TH2D  *  p_offresOrefptVsrefpt_bb_npv25_29   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_bb_npv25_29");
   TH2D  *  p_offresOrefptVsrefpt_ei_npv0_4     =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_ei_npv0_4");
   TH2D  *  p_offresOrefptVsrefpt_ei_npv5_9     =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_ei_npv5_9");
   TH2D  *  p_offresOrefptVsrefpt_ei_npv10_14   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_ei_npv10_14");
   TH2D  *  p_offresOrefptVsrefpt_ei_npv15_19   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_ei_npv15_19");
   TH2D  *  p_offresOrefptVsrefpt_ei_npv20_24   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_ei_npv20_24");
   TH2D  *  p_offresOrefptVsrefpt_ei_npv25_29   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_ei_npv25_29");
   TH2D  *  p_offresOrefptVsrefpt_eo_npv0_4     =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_eo_npv0_4");
   TH2D  *  p_offresOrefptVsrefpt_eo_npv5_9     =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_eo_npv5_9");
   TH2D  *  p_offresOrefptVsrefpt_eo_npv10_14   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_eo_npv10_14");
   TH2D  *  p_offresOrefptVsrefpt_eo_npv15_19   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_eo_npv15_19");
   TH2D  *  p_offresOrefptVsrefpt_eo_npv20_24   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_eo_npv20_24");
   TH2D  *  p_offresOrefptVsrefpt_eo_npv25_29   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_eo_npv25_29");
   TH2D  *  p_offresOrefptVsrefpt_ff_npv0_4     =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_ff_npv0_4");
   TH2D  *  p_offresOrefptVsrefpt_ff_npv5_9     =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_ff_npv5_9");
   TH2D  *  p_offresOrefptVsrefpt_ff_npv10_14   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_ff_npv10_14");
   TH2D  *  p_offresOrefptVsrefpt_ff_npv15_19   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_ff_npv15_19");
   TH2D  *  p_offresOrefptVsrefpt_ff_npv20_24   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_ff_npv20_24");
   TH2D  *  p_offresOrefptVsrefpt_ff_npv25_29   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_ff_npv25_29");
   TH2D	*	p_resVsrefpt_bb_rho0_4	=	(	TH2D	* )	fin->Get("p_resVsrefpt_bb_rho0_4");
   TH2D	*	p_resVsrefpt_bb_rho5_9	=	(	TH2D	* )	fin->Get("p_resVsrefpt_bb_rho5_9");
   TH2D	*	p_resVsrefpt_bb_rho10_14	=	(	TH2D	* )	fin->Get("p_resVsrefpt_bb_rho10_14");
   TH2D	*	p_resVsrefpt_bb_rho15_19	=	(	TH2D	* )	fin->Get("p_resVsrefpt_bb_rho15_19");
   TH2D	*	p_resVsrefpt_bb_rho20_24	=	(	TH2D	* )	fin->Get("p_resVsrefpt_bb_rho20_24");
   TH2D	*	p_resVsrefpt_bb_rho25_29	=	(	TH2D	* )	fin->Get("p_resVsrefpt_bb_rho25_29");
   TH2D	*	p_resVsrefpt_ei_rho0_4	=	(	TH2D	* )	fin->Get("p_resVsrefpt_ei_rho0_4");
   TH2D	*	p_resVsrefpt_ei_rho5_9	=	(	TH2D	* )	fin->Get("p_resVsrefpt_ei_rho5_9");
   TH2D	*	p_resVsrefpt_ei_rho10_14	=	(	TH2D	* )	fin->Get("p_resVsrefpt_ei_rho10_14");
   TH2D	*	p_resVsrefpt_ei_rho15_19	=	(	TH2D	* )	fin->Get("p_resVsrefpt_ei_rho15_19");
   TH2D	*	p_resVsrefpt_ei_rho20_24	=	(	TH2D	* )	fin->Get("p_resVsrefpt_ei_rho20_24");
   TH2D	*	p_resVsrefpt_ei_rho25_29	=	(	TH2D	* )	fin->Get("p_resVsrefpt_ei_rho25_29");
   TH2D	*	p_resVsrefpt_eo_rho0_4	=	(	TH2D	* )	fin->Get("p_resVsrefpt_eo_rho0_4");
   TH2D	*	p_resVsrefpt_eo_rho5_9	=	(	TH2D	* )	fin->Get("p_resVsrefpt_eo_rho5_9");
   TH2D	*	p_resVsrefpt_eo_rho10_14	=	(	TH2D	* )	fin->Get("p_resVsrefpt_eo_rho10_14");
   TH2D	*	p_resVsrefpt_eo_rho15_19	=	(	TH2D	* )	fin->Get("p_resVsrefpt_eo_rho15_19");
   TH2D	*	p_resVsrefpt_eo_rho20_24	=	(	TH2D	* )	fin->Get("p_resVsrefpt_eo_rho20_24");
   TH2D	*	p_resVsrefpt_eo_rho25_29	=	(	TH2D	* )	fin->Get("p_resVsrefpt_eo_rho25_29");
   TH2D	*	p_resVsrefpt_ff_rho0_4	=	(	TH2D	* )	fin->Get("p_resVsrefpt_ff_rho0_4");
   TH2D	*	p_resVsrefpt_ff_rho5_9	=	(	TH2D	* )	fin->Get("p_resVsrefpt_ff_rho5_9");
   TH2D	*	p_resVsrefpt_ff_rho10_14	=	(	TH2D	* )	fin->Get("p_resVsrefpt_ff_rho10_14");
   TH2D	*	p_resVsrefpt_ff_rho15_19	=	(	TH2D	* )	fin->Get("p_resVsrefpt_ff_rho15_19");
   TH2D	*	p_resVsrefpt_ff_rho20_24	=	(	TH2D	* )	fin->Get("p_resVsrefpt_ff_rho20_24");
   TH2D	*	p_resVsrefpt_ff_rho25_29	=	(	TH2D	* )	fin->Get("p_resVsrefpt_ff_rho25_29");
   TH2D	*	p_offresVsrefpt_bb_rho0_4	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_bb_rho0_4");
   TH2D	*	p_offresVsrefpt_bb_rho5_9	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_bb_rho5_9");
   TH2D	*	p_offresVsrefpt_bb_rho10_14	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_bb_rho10_14");
   TH2D	*	p_offresVsrefpt_bb_rho15_19	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_bb_rho15_19");
   TH2D	*	p_offresVsrefpt_bb_rho20_24	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_bb_rho20_24");
   TH2D	*	p_offresVsrefpt_bb_rho25_29	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_bb_rho25_29");
   TH2D	*	p_offresVsrefpt_ei_rho0_4	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_ei_rho0_4");
   TH2D	*	p_offresVsrefpt_ei_rho5_9	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_ei_rho5_9");
   TH2D	*	p_offresVsrefpt_ei_rho10_14	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_ei_rho10_14");
   TH2D	*	p_offresVsrefpt_ei_rho15_19	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_ei_rho15_19");
   TH2D	*	p_offresVsrefpt_ei_rho20_24	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_ei_rho20_24");
   TH2D	*	p_offresVsrefpt_ei_rho25_29	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_ei_rho25_29");
   TH2D	*	p_offresVsrefpt_eo_rho0_4	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_eo_rho0_4");
   TH2D	*	p_offresVsrefpt_eo_rho5_9	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_eo_rho5_9");
   TH2D	*	p_offresVsrefpt_eo_rho10_14	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_eo_rho10_14");
   TH2D	*	p_offresVsrefpt_eo_rho15_19	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_eo_rho15_19");
   TH2D	*	p_offresVsrefpt_eo_rho20_24	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_eo_rho20_24");
   TH2D	*	p_offresVsrefpt_eo_rho25_29	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_eo_rho25_29");
   TH2D	*	p_offresVsrefpt_ff_rho0_4	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_ff_rho0_4");
   TH2D	*	p_offresVsrefpt_ff_rho5_9	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_ff_rho5_9");
   TH2D	*	p_offresVsrefpt_ff_rho10_14	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_ff_rho10_14");
   TH2D	*	p_offresVsrefpt_ff_rho15_19	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_ff_rho15_19");
   TH2D	*	p_offresVsrefpt_ff_rho20_24	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_ff_rho20_24");
   TH2D	*	p_offresVsrefpt_ff_rho25_29	=	(	TH2D	* )	fin->Get("p_offresVsrefpt_ff_rho25_29");
   TH2D  *  p_offresOrefptVsrefpt_bb_rho0_4     =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_bb_rho0_4");
   TH2D  *  p_offresOrefptVsrefpt_bb_rho5_9     =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_bb_rho5_9");
   TH2D  *  p_offresOrefptVsrefpt_bb_rho10_14   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_bb_rho10_14");
   TH2D  *  p_offresOrefptVsrefpt_bb_rho15_19   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_bb_rho15_19");
   TH2D  *  p_offresOrefptVsrefpt_bb_rho20_24   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_bb_rho20_24");
   TH2D  *  p_offresOrefptVsrefpt_bb_rho25_29   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_bb_rho25_29");
   TH2D  *  p_offresOrefptVsrefpt_ei_rho0_4     =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_ei_rho0_4");
   TH2D  *  p_offresOrefptVsrefpt_ei_rho5_9     =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_ei_rho5_9");
   TH2D  *  p_offresOrefptVsrefpt_ei_rho10_14   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_ei_rho10_14");
   TH2D  *  p_offresOrefptVsrefpt_ei_rho15_19   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_ei_rho15_19");
   TH2D  *  p_offresOrefptVsrefpt_ei_rho20_24   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_ei_rho20_24");
   TH2D  *  p_offresOrefptVsrefpt_ei_rho25_29   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_ei_rho25_29");
   TH2D  *  p_offresOrefptVsrefpt_eo_rho0_4     =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_eo_rho0_4");
   TH2D  *  p_offresOrefptVsrefpt_eo_rho5_9     =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_eo_rho5_9");
   TH2D  *  p_offresOrefptVsrefpt_eo_rho10_14   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_eo_rho10_14");
   TH2D  *  p_offresOrefptVsrefpt_eo_rho15_19   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_eo_rho15_19");
   TH2D  *  p_offresOrefptVsrefpt_eo_rho20_24   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_eo_rho20_24");
   TH2D  *  p_offresOrefptVsrefpt_eo_rho25_29   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_eo_rho25_29");
   TH2D  *  p_offresOrefptVsrefpt_ff_rho0_4     =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_ff_rho0_4");
   TH2D  *  p_offresOrefptVsrefpt_ff_rho5_9     =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_ff_rho5_9");
   TH2D  *  p_offresOrefptVsrefpt_ff_rho10_14   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_ff_rho10_14");
   TH2D  *  p_offresOrefptVsrefpt_ff_rho15_19   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_ff_rho15_19");
   TH2D  *  p_offresOrefptVsrefpt_ff_rho20_24   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_ff_rho20_24");
   TH2D  *  p_offresOrefptVsrefpt_ff_rho25_29   =  (  TH2D  * )   fin->Get("p_offresOrefptVsrefpt_ff_rho25_29");
   TH2D  *  p_offresVsrefpt_bb_tnpu0_4          =  (  TH2D  * )   fin->Get("p_offresVsrefpt_bb_tnpu0_4");
   TH2D  *  p_offresVsrefpt_bb_tnpu5_9          =  (  TH2D  * )   fin->Get("p_offresVsrefpt_bb_tnpu5_9");
   TH2D  *  p_offresVsrefpt_bb_tnpu10_14        =  (  TH2D  * )   fin->Get("p_offresVsrefpt_bb_tnpu10_14");
   TH2D  *  p_offresVsrefpt_bb_tnpu15_19        =  (  TH2D  * )   fin->Get("p_offresVsrefpt_bb_tnpu15_19");
   TH2D  *  p_offresVsrefpt_bb_tnpu20_24        =  (  TH2D  * )   fin->Get("p_offresVsrefpt_bb_tnpu20_24");
   TH2D  *  p_offresVsrefpt_bb_tnpu25_29        =  (  TH2D  * )   fin->Get("p_offresVsrefpt_bb_tnpu25_29");
   TH2D  *  p_offresVsrefpt_bb_tnpu0_29         =  (  TH2D  * )   fin->Get("p_offresVsrefpt_bb_tnpu0_29");
   TH2D  *  p_offresVsrefpt_ei_tnpu0_4          =  (  TH2D  * )   fin->Get("p_offresVsrefpt_ei_tnpu0_4");
   TH2D  *  p_offresVsrefpt_ei_tnpu5_9          =  (  TH2D  * )   fin->Get("p_offresVsrefpt_ei_tnpu5_9");
   TH2D  *  p_offresVsrefpt_ei_tnpu10_14        =  (  TH2D  * )   fin->Get("p_offresVsrefpt_ei_tnpu10_14");
   TH2D  *  p_offresVsrefpt_ei_tnpu15_19        =  (  TH2D  * )   fin->Get("p_offresVsrefpt_ei_tnpu15_19");
   TH2D  *  p_offresVsrefpt_ei_tnpu20_24        =  (  TH2D  * )   fin->Get("p_offresVsrefpt_ei_tnpu20_24");
   TH2D  *  p_offresVsrefpt_ei_tnpu25_29        =  (  TH2D  * )   fin->Get("p_offresVsrefpt_ei_tnpu25_29");
   TH2D  *  p_offresVsrefpt_ei_tnpu0_29         =  (  TH2D  * )   fin->Get("p_offresVsrefpt_ei_tnpu0_29");
   TH2D  *  p_offresVsrefpt_eo_tnpu0_4          =  (  TH2D  * )   fin->Get("p_offresVsrefpt_eo_tnpu0_4");
   TH2D  *  p_offresVsrefpt_eo_tnpu5_9          =  (  TH2D  * )   fin->Get("p_offresVsrefpt_eo_tnpu5_9");
   TH2D  *  p_offresVsrefpt_eo_tnpu10_14        =  (  TH2D  * )   fin->Get("p_offresVsrefpt_eo_tnpu10_14");
   TH2D  *  p_offresVsrefpt_eo_tnpu15_19        =  (  TH2D  * )   fin->Get("p_offresVsrefpt_eo_tnpu15_19");
   TH2D  *  p_offresVsrefpt_eo_tnpu20_24        =  (  TH2D  * )   fin->Get("p_offresVsrefpt_eo_tnpu20_24");
   TH2D  *  p_offresVsrefpt_eo_tnpu25_29        =  (  TH2D  * )   fin->Get("p_offresVsrefpt_eo_tnpu25_29");
   TH2D  *  p_offresVsrefpt_eo_tnpu0_29         =  (  TH2D  * )   fin->Get("p_offresVsrefpt_eo_tnpu0_29");
   TH2D  *  p_offresVsrefpt_ff_tnpu0_4          =  (  TH2D  * )   fin->Get("p_offresVsrefpt_ff_tnpu0_4");
   TH2D  *  p_offresVsrefpt_ff_tnpu5_9          =  (  TH2D  * )   fin->Get("p_offresVsrefpt_ff_tnpu5_9");
   TH2D  *  p_offresVsrefpt_ff_tnpu10_14        =  (  TH2D  * )   fin->Get("p_offresVsrefpt_ff_tnpu10_14");
   TH2D  *  p_offresVsrefpt_ff_tnpu15_19        =  (  TH2D  * )   fin->Get("p_offresVsrefpt_ff_tnpu15_19");
   TH2D  *  p_offresVsrefpt_ff_tnpu20_24        =  (  TH2D  * )   fin->Get("p_offresVsrefpt_ff_tnpu20_24");
   TH2D  *  p_offresVsrefpt_ff_tnpu25_29        =  (  TH2D  * )   fin->Get("p_offresVsrefpt_ff_tnpu25_29");
   TH2D  *  p_offresVsrefpt_ff_tnpu0_29         =  (  TH2D  * )   fin->Get("p_offresVsrefpt_ff_tnpu0_29");
   TH2D  *  p_offresVsrefpt_bb_pdgid_nJ         =  (  TH2D  * )   fin->Get("p_offresVsrefpt_bb_pdgid_nJ");
   TH2D  *  p_offresVsrefpt_bb_pdgid_qJ         =  (  TH2D  * )   fin->Get("p_offresVsrefpt_bb_pdgid_qJ");
   TH2D  *  p_offresVsrefpt_bb_pdgid_cJ         =  (  TH2D  * )   fin->Get("p_offresVsrefpt_bb_pdgid_cJ");
   TH2D  *  p_offresVsrefpt_bb_pdgid_bJ         =  (  TH2D  * )   fin->Get("p_offresVsrefpt_bb_pdgid_bJ");
   TH2D  *  p_offresVsrefpt_bb_pdgid_gJ         =  (  TH2D  * )   fin->Get("p_offresVsrefpt_bb_pdgid_gJ");
   TH2D  *  p_offresVsrefpt_bb_pdgid_aJ         =  (  TH2D  * )   fin->Get("p_offresVsrefpt_bb_pdgid_aJ");
   TH2D  *  p_offresVsrefpt_bb_pdgid_aqJ        =  (  TH2D  * )   fin->Get("p_offresVsrefpt_bb_pdgid_aqJ");
   TH2D  *  p_offresVsrefpt_ei_pdgid_nJ         =  (  TH2D  * )   fin->Get("p_offresVsrefpt_ei_pdgid_nJ");
   TH2D  *  p_offresVsrefpt_ei_pdgid_qJ         =  (  TH2D  * )   fin->Get("p_offresVsrefpt_ei_pdgid_qJ");
   TH2D  *  p_offresVsrefpt_ei_pdgid_cJ         =  (  TH2D  * )   fin->Get("p_offresVsrefpt_ei_pdgid_cJ");
   TH2D  *  p_offresVsrefpt_ei_pdgid_bJ         =  (  TH2D  * )   fin->Get("p_offresVsrefpt_ei_pdgid_bJ");
   TH2D  *  p_offresVsrefpt_ei_pdgid_gJ         =  (  TH2D  * )   fin->Get("p_offresVsrefpt_ei_pdgid_gJ");
   TH2D  *  p_offresVsrefpt_ei_pdgid_aJ         =  (  TH2D  * )   fin->Get("p_offresVsrefpt_ei_pdgid_aJ");
   TH2D  *  p_offresVsrefpt_ei_pdgid_aqJ        =  (  TH2D  * )   fin->Get("p_offresVsrefpt_ei_pdgid_aqJ");
   TH2D  *  p_offresVsrefpt_eo_pdgid_nJ         =  (  TH2D  * )   fin->Get("p_offresVsrefpt_eo_pdgid_nJ");
   TH2D  *  p_offresVsrefpt_eo_pdgid_qJ         =  (  TH2D  * )   fin->Get("p_offresVsrefpt_eo_pdgid_qJ");
   TH2D  *  p_offresVsrefpt_eo_pdgid_cJ         =  (  TH2D  * )   fin->Get("p_offresVsrefpt_eo_pdgid_cJ");
   TH2D  *  p_offresVsrefpt_eo_pdgid_bJ         =  (  TH2D  * )   fin->Get("p_offresVsrefpt_eo_pdgid_bJ");
   TH2D  *  p_offresVsrefpt_eo_pdgid_gJ         =  (  TH2D  * )   fin->Get("p_offresVsrefpt_eo_pdgid_gJ");
   TH2D  *  p_offresVsrefpt_eo_pdgid_aJ         =  (  TH2D  * )   fin->Get("p_offresVsrefpt_eo_pdgid_aJ");
   TH2D  *  p_offresVsrefpt_eo_pdgid_aqJ        =  (  TH2D  * )   fin->Get("p_offresVsrefpt_eo_pdgid_aqJ");
   TH2D  *  p_offresVsrefpt_ff_pdgid_nJ         =  (  TH2D  * )   fin->Get("p_offresVsrefpt_ff_pdgid_nJ");
   TH2D  *  p_offresVsrefpt_ff_pdgid_qJ         =  (  TH2D  * )   fin->Get("p_offresVsrefpt_ff_pdgid_qJ");
   TH2D  *  p_offresVsrefpt_ff_pdgid_cJ         =  (  TH2D  * )   fin->Get("p_offresVsrefpt_ff_pdgid_cJ");
   TH2D  *  p_offresVsrefpt_ff_pdgid_bJ         =  (  TH2D  * )   fin->Get("p_offresVsrefpt_ff_pdgid_bJ");
   TH2D  *  p_offresVsrefpt_ff_pdgid_gJ         =  (  TH2D  * )   fin->Get("p_offresVsrefpt_ff_pdgid_gJ");
   TH2D  *  p_offresVsrefpt_ff_pdgid_aJ         =  (  TH2D  * )   fin->Get("p_offresVsrefpt_ff_pdgid_aJ");
   TH2D  *  p_offresVsrefpt_ff_pdgid_aqJ        =  (  TH2D  * )   fin->Get("p_offresVsrefpt_ff_pdgid_aqJ");

   TProfile * m_all_nj_npv = (TProfile * ) fin->Get("m_all_nj_npv");
   TProfile * m_matched_nj_npv = (TProfile * ) fin->Get("m_matched_nj_npv");
   TProfile * m_unmatched_nj_npv = (TProfile * ) fin->Get("m_unmatched_nj_npv");
   TProfile * m_all_jtpt_npv = (TProfile * ) fin->Get("m_all_jtpt_npv");
   TProfile * m_matched_jtpt_npv = (TProfile * ) fin->Get("m_matched_jtpt_npv");
   TProfile * m_unmatched_jtpt_npv = (TProfile * ) fin->Get("m_unmatched_jtpt_npv");
   
   

//==================================================================================

   // Event-Matching performance 
   c = new TCanvas("RefPtDiff","RefPtDiff");
   c->SetLogy();
   m_refpt_diff->Draw();
  
   // Event-Matching performance 
   c = new TCanvas("RefPdgidDiff","RefPdgidDiff");
   c->SetLogy();
   m_refpdgid_diff->Draw();
  
   // Sanity check: g_pthat
   c = new TCanvas("PthatDiff","PthatDiff");
   m_deltaPthat->Draw();
   m_deltaPthat->GetYaxis()->SetRangeUser(-1,1);
   m_deltaPthat->Draw();

   // Number of Jets vs PT in both samples
   c = new TCanvas("NJetsVsPt","NJetsVsPt");
   c->SetLogx();
   setHistoColor(m_njet_pt_pu, colPU);
   setHistoColor(m_njet_pt_nopu, colNoPU);
   m_njet_pt_pu->GetYaxis()->SetRangeUser(0,4.5e6);
   m_njet_pt_pu->Draw("E");
   m_njet_pt_nopu->Draw("sameE");
  
   leg = new TLegend(0.7,0.4,0.9,0.6);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   leg->AddEntry(m_njet_pt_pu, "PU sample","lep");
   leg->AddEntry(m_njet_pt_nopu, "NoPU sample","lep");
   leg->Draw();

   // Ratio of number of Jets vs PT in both samples
   c = new TCanvas("NJetsVsPt_Ratio","NJetsVsPt_Ratio");
   c->SetLogx();
   TH1D* ratio = (TH1D*)m_njet_pt_pu->Clone("NJetsVsPt_Ratio");
   ratio->Sumw2();
   ratio->GetYaxis()->SetRangeUser(0,10);
   ratio->Divide(m_njet_pt_nopu);
   ratio->Draw("E");
  
   leg = new TLegend(0.7,0.4,0.9,0.6);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   leg->AddEntry(ratio, "#frac{PU sample}{NoPU sample}","lep");
   leg->Draw();
  
   // njet vs npv
   c = new TCanvas("NJetsVsNPV","NJetsVsNPV");
   setHistoColor(m_all_nj_npv,       colB);
   setHistoColor(m_matched_nj_npv,   colE);
   setHistoColor(m_unmatched_nj_npv, colF);
   m_all_nj_npv->GetYaxis()->SetRangeUser(0,20);
   m_all_nj_npv->Draw("E");
   m_matched_nj_npv->Draw("sameE");
   m_unmatched_nj_npv->Draw("sameE");

   leg = new TLegend(0.7,0.75,0.9,0.95);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   leg->AddEntry(m_all_nj_npv, "All jets","lep");
   leg->AddEntry(m_matched_nj_npv, "Matched jets","lep");
   leg->AddEntry(m_unmatched_nj_npv, "UnMatched jets","lep");
   leg->Draw();

   // jetpt vs npv
   c = new TCanvas("JtptVsNPV","JtptVsNPV");
   setHistoColor(m_all_jtpt_npv,       colB);
   setHistoColor(m_matched_jtpt_npv,   colE);
   setHistoColor(m_unmatched_jtpt_npv, colF);
   m_all_jtpt_npv->GetYaxis()->SetRangeUser(0,100);
   m_all_jtpt_npv->Draw("E");
   m_matched_jtpt_npv->Draw("sameE");
   m_unmatched_jtpt_npv->Draw("sameE");

   leg = new TLegend(0.7,0.75,0.9,0.95);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   leg->AddEntry(m_all_jtpt_npv, "All jets","lep");
   leg->AddEntry(m_matched_jtpt_npv, "Matched jets","lep");
   leg->AddEntry(m_unmatched_jtpt_npv, "UnMatched jets","lep");
   leg->Draw();
  

   // Fraction of Matched Jets
   c = new TCanvas("FractionMatchedJetsNoPU","FractionMatchedJets NoPU Sample");
   c->SetLogx();
   setHistoColor(m_frac_nj_pt_b_match_nopu, colB);
   setHistoColor(m_frac_nj_pt_e_match_nopu, colE);
   setHistoColor(m_frac_nj_pt_f_match_nopu, colF);
   m_frac_nj_pt_b_match_nopu->Draw();
   m_frac_nj_pt_b_match_nopu->GetYaxis()->SetRangeUser(0.3,1.1);
   m_frac_nj_pt_e_match_nopu->Draw("same");
   m_frac_nj_pt_f_match_nopu->Draw("same");
  
   leg = new TLegend(0.7,0.4,0.9,0.6);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   leg->AddEntry(m_frac_nj_pt_b_match_nopu,"Barrel","lep");
   leg->AddEntry(m_frac_nj_pt_e_match_nopu,"Endcap","lep");
   leg->AddEntry(m_frac_nj_pt_f_match_nopu,"Forward","lep");
   leg->Draw();


   // Fraction of Matched Jets
   c = new TCanvas("FractionMatchedJetsPU","FractionMatchedJets PU Sample");
   c->SetLogx();
   setHistoColor(m_frac_nj_pt_b_match_pu, colB);
   setHistoColor(m_frac_nj_pt_e_match_pu, colE);
   setHistoColor(m_frac_nj_pt_f_match_pu, colF);
   m_frac_nj_pt_b_match_pu->Draw();
   m_frac_nj_pt_b_match_pu->GetYaxis()->SetRangeUser(0.3,1.1);
   m_frac_nj_pt_e_match_pu->Draw("same");
   m_frac_nj_pt_f_match_pu->Draw("same");
  
   leg = new TLegend(0.7,0.4,0.9,0.6);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   leg->AddEntry(m_frac_nj_pt_b_match_pu,"Barrel","lep");
   leg->AddEntry(m_frac_nj_pt_e_match_pu,"Endcap","lep");
   leg->AddEntry(m_frac_nj_pt_f_match_pu,"Forward","lep");
   leg->Draw();

    
   // Fraction of RG-Matched Jets
   c = new TCanvas("FractionRGMatchedJetsNoPU","FractionRGMatchedJets NoPU Sample");
   c->SetLogx();
   setHistoColor(m_frac_nj_pt_b_match_RG_nopu, colB);
   setHistoColor(m_frac_nj_pt_e_match_RG_nopu, colE);
   setHistoColor(m_frac_nj_pt_f_match_RG_nopu, colF);
   m_frac_nj_pt_b_match_RG_nopu->Draw();
   m_frac_nj_pt_b_match_RG_nopu->GetYaxis()->SetRangeUser(0.5,1.1);
   m_frac_nj_pt_e_match_RG_nopu->Draw("same");
   m_frac_nj_pt_f_match_RG_nopu->Draw("same");
  
   leg = new TLegend(0.7,0.4,0.9,0.6);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   leg->AddEntry(m_frac_nj_pt_b_match_RG_nopu,"Barrel","lep");
   leg->AddEntry(m_frac_nj_pt_e_match_RG_nopu,"Endcap","lep");
   leg->AddEntry(m_frac_nj_pt_f_match_RG_nopu,"Forward","lep");
   leg->Draw();
  
   // Fraction of RG-Matched Jets
   c = new TCanvas("FractionRGMatchedJetsPU","FractionRGMatchedJets PU Sample");
   c->SetLogx();
   setHistoColor(m_frac_nj_pt_b_match_RG_pu, colB);
   setHistoColor(m_frac_nj_pt_e_match_RG_pu, colE);
   setHistoColor(m_frac_nj_pt_f_match_RG_pu, colF);
   m_frac_nj_pt_b_match_RG_pu->Draw();
   m_frac_nj_pt_b_match_RG_pu->GetYaxis()->SetRangeUser(0.5,1.1);
   m_frac_nj_pt_e_match_RG_pu->Draw("same");
   m_frac_nj_pt_f_match_RG_pu->Draw("same");
  
   leg = new TLegend(0.7,0.4,0.9,0.6);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   leg->AddEntry(m_frac_nj_pt_b_match_RG_pu,"Barrel","lep");
   leg->AddEntry(m_frac_nj_pt_e_match_RG_pu,"Endcap","lep");
   leg->AddEntry(m_frac_nj_pt_f_match_RG_pu,"Forward","lep");
   leg->Draw();

   // Fraction of Matched Jets-PU-NPV
   c = new TCanvas("FractionMatchedJetsPU_NPV","FractionMatchedJets vs. npv PU Sample");
   c->SetLogx();
   setHistoColor(m_frac_nj_pt_b_match_pu_npv10, colNpv0);
   setHistoColor(m_frac_nj_pt_b_match_pu_npv20, colNpv10);
   setHistoColor(m_frac_nj_pt_b_match_pu_npv30, colNpv15);
   setHistoColor(m_frac_nj_pt_b_match_pu_npvO, colNpv25);
   m_frac_nj_pt_b_match_pu_npv10->GetYaxis()->SetTitle("fraction of matched jets (Barrel)");
   m_frac_nj_pt_b_match_pu_npv10->Draw();
   m_frac_nj_pt_b_match_pu_npv10->GetYaxis()->SetRangeUser(0.3,1.1);
   m_frac_nj_pt_b_match_pu_npv20->Draw("same");
   m_frac_nj_pt_b_match_pu_npv30->Draw("same");
   m_frac_nj_pt_b_match_pu_npvO->Draw("same");
  
   leg = new TLegend(0.7,0.4,0.9,0.6);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   leg->AddEntry(m_frac_nj_pt_b_match_pu_npv10,"0 < N_{PV}^{PU}<=10","lep");
   leg->AddEntry(m_frac_nj_pt_b_match_pu_npv20,"10< N_{PV}^{PU}<=20","lep");
   leg->AddEntry(m_frac_nj_pt_b_match_pu_npv30,"20< N_{PV}^{PU}<=30","lep");
   leg->AddEntry(m_frac_nj_pt_b_match_pu_npvO, "30< N_{PV}^{PU}    ","lep");
   leg->Draw();
  
   // Fraction of RG-Matched Jets-PU-NPV
   c = new TCanvas("FractionRGMatchedJetsPU_NPV","FractionRGMatchedJets vs. npv PU Sample");
   c->SetLogx();
   setHistoColor(m_frac_nj_pt_b_match_RG_pu_npv10, colNpv0);
   setHistoColor(m_frac_nj_pt_b_match_RG_pu_npv20, colNpv10);
   setHistoColor(m_frac_nj_pt_b_match_RG_pu_npv30, colNpv15);
   setHistoColor(m_frac_nj_pt_b_match_RG_pu_npvO, colNpv25);
   m_frac_nj_pt_b_match_RG_pu_npv10->Draw();
   m_frac_nj_pt_b_match_RG_pu_npv10->GetYaxis()->SetRangeUser(0.1,1.1);
   m_frac_nj_pt_b_match_RG_pu_npv20->Draw("same");
   m_frac_nj_pt_b_match_RG_pu_npv30->Draw("same");
   m_frac_nj_pt_b_match_RG_pu_npvO->Draw("same");
  
   leg = new TLegend(0.7,0.4,0.9,0.6);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   leg->AddEntry(m_frac_nj_pt_b_match_RG_pu_npv10,"0 < N_{PV}^{PU}<=10","lep");
   leg->AddEntry(m_frac_nj_pt_b_match_RG_pu_npv20,"10< N_{PV}^{PU}<=20","lep");
   leg->AddEntry(m_frac_nj_pt_b_match_RG_pu_npv30,"20< N_{PV}^{PU}<=30","lep");
   leg->AddEntry(m_frac_nj_pt_b_match_RG_pu_npvO, "30< N_{PV}^{PU}    ","lep");
   leg->Draw();
  
  

   // Fraction of Matched Jets-NOPU-NPV
   c = new TCanvas("FractionMatchedJetsNoPU_NPV","FractionMatchedJets vs. npv NoPU Sample");
   c->SetLogx();
   setHistoColor(m_frac_nj_pt_b_match_nopu_npv1, colNpv0);
   setHistoColor(m_frac_nj_pt_b_match_nopu_npv2, colNpv10);
   setHistoColor(m_frac_nj_pt_b_match_nopu_npv3, colNpv15);
   setHistoColor(m_frac_nj_pt_b_match_nopu_npvO, colNpv25);
   m_frac_nj_pt_b_match_nopu_npv1->Draw();
   m_frac_nj_pt_b_match_nopu_npv1->GetYaxis()->SetRangeUser(0.3,1.1);
   m_frac_nj_pt_b_match_nopu_npv2->Draw("same");
   m_frac_nj_pt_b_match_nopu_npv3->Draw("same");
   m_frac_nj_pt_b_match_nopu_npvO->Draw("same");
  
   leg = new TLegend(0.7,0.4,0.9,0.6);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   leg->AddEntry(m_frac_nj_pt_b_match_nopu_npv1,"0 < N_{PV}^{PU}<=10","lep");
   leg->AddEntry(m_frac_nj_pt_b_match_nopu_npv2,"10< N_{PV}^{PU}<=20","lep");
   leg->AddEntry(m_frac_nj_pt_b_match_nopu_npv3,"20< N_{PV}^{PU}<=30","lep");
   leg->AddEntry(m_frac_nj_pt_b_match_nopu_npvO,"30< N_{PV}^{PU}    ","lep");
   leg->Draw();


   // PU distribution of jets per event per NPv
   // Number of unmatched jets in pu sample minus #of unmatch jets in nopu sample
   c = new TCanvas("PUJetDistribution","PUJetDistribution");
   c->SetLogx();
   setHistoColor(m_njet_pt_npv0_4_unmatch  ,colNpv0);
   setHistoColor(m_njet_pt_npv5_9_unmatch  ,colNpv5);
   setHistoColor(m_njet_pt_npv10_14_unmatch,colNpv10);
   setHistoColor(m_njet_pt_npv15_19_unmatch,colNpv15);
   setHistoColor(m_njet_pt_npv20_24_unmatch,colNpv20);
   setHistoColor(m_njet_pt_npv25_29_unmatch,colNpv25);
   m_njet_pt_npv0_4_unmatch  ->Draw("E");
   m_njet_pt_npv0_4_unmatch->GetYaxis()->SetRangeUser(0,1);
   m_njet_pt_npv5_9_unmatch  ->Draw("sameE");
   m_njet_pt_npv10_14_unmatch->Draw("sameE");
   m_njet_pt_npv15_19_unmatch->Draw("sameE");
   m_njet_pt_npv20_24_unmatch->Draw("sameE");
   m_njet_pt_npv25_29_unmatch->Draw("sameE");
   leg = new TLegend(0.7,0.65,0.9,0.85);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   leg->AddEntry(m_njet_pt_npv0_4_unmatch  ," 0 <= N_{PV}<5","lep");
   leg->AddEntry(m_njet_pt_npv5_9_unmatch  ," 5 <= N_{PV}<10","lep");
   leg->AddEntry(m_njet_pt_npv10_14_unmatch,"10 <= N_{PV}<15","lep");
   leg->AddEntry(m_njet_pt_npv15_19_unmatch,"15 <= N_{PV}<20","lep");
   leg->AddEntry(m_njet_pt_npv20_24_unmatch,"20 <= N_{PV}<25","lep");
   leg->AddEntry(m_njet_pt_npv25_29_unmatch,"25 <= N_{PV}","lep");
   leg->Draw();

   // INTEGRAL of PU distribution of jets per event per NPv--------ZQ
   // Number of unmatched jets in pu sample minus #of unmatch jets in nopu sample
  
   TProfile * pnJetPt[6];
   pnJetPt[0]	=	m_njet_pt_npv0_4_unmatch	;
   pnJetPt[1]	=	m_njet_pt_npv5_9_unmatch	;
   pnJetPt[2]	=	m_njet_pt_npv10_14_unmatch	;
   pnJetPt[3]	=	m_njet_pt_npv15_19_unmatch	;
   pnJetPt[4]	=	m_njet_pt_npv20_24_unmatch	;
   pnJetPt[5]	=	m_njet_pt_npv25_29_unmatch	;
  
   c = getCanvasIntegral("IntePUJetDistribution",algo,"Integral # PU Jets /event",pnJetPt);
  
   c->SetLogx();
   c->Draw();
  
   c = getCanvasAverage("AvgPUJetEneDistribution",algo,"Avg. Jet Ene (GeV)",pnJetPt);
   c->SetLogx();
   c->Draw();
  
   //
   //	2D histogram of jtarea diff. vs. refpt	
   //
   c = new TCanvas("areaVsrefpt","areaVsrefpt");
   p_areaVsrefpt->Draw("colZ");
   c->SetLogx();  
  
   TProfile *p_areaVsrefpt_prof = p_areaVsrefpt->ProfileX();
   c = new TCanvas("areaVsrefptProf","areaVsrefptProf");
   p_areaVsrefpt_prof->GetYaxis()->SetTitle("<jtarea^{pu}-jtarea^{nopu}>");
   p_areaVsrefpt_prof->GetYaxis()->SetRangeUser(-0.006,0.006);
   TGaxis::SetMaxDigits(3);
   c->SetLogx();
   p_areaVsrefpt_prof->Draw("");

   //
   //	2D histogram of jtarea diff. vs. refpt	
   //
   c = new TCanvas("areaVsOffset_1000","areaVsOffset_1000");
   p_areaVsoffset_1000->GetXaxis()->SetRangeUser(-100,100);
   p_areaVsoffset_1000->Draw("colZ");
   TProfile *p_areaVsoffset_1000_prof = p_areaVsoffset_1000->ProfileX();
   c = new TCanvas("areaVsOffset_1000Prof","areaVsOffset_1000Prof");
   p_areaVsoffset_1000_prof->GetYaxis()->SetTitle("<jtarea^{pu}-jtarea^{nopu}>");
   p_areaVsoffset_1000_prof->GetXaxis()->SetRangeUser(-100,100);
   p_areaVsoffset_1000_prof->GetYaxis()->SetRangeUser(-0.3,0.3);
   p_areaVsoffset_1000_prof->Draw("");
  
  
   //
   //	2D histogram of jtarea diff. vs. refpt	
   //
   c = new TCanvas("areaVsOffset_30_50","areaVsOffset_30_50");
   p_areaVsoffset_30_50->GetXaxis()->SetRangeUser(-100,100);
   p_areaVsoffset_30_50->Draw("colZ");
   TProfile *p_areaVsoffset_30_50_prof = p_areaVsoffset_30_50->ProfileX();
   c = new TCanvas("areaVsOffset_30_50Prof","areaVsOffset_30_50Prof");
   p_areaVsoffset_30_50_prof->GetYaxis()->SetTitle("<jtarea^{pu}-jtarea^{nopu}>");
   p_areaVsoffset_30_50_prof->GetXaxis()->SetRangeUser(-100,100);
   p_areaVsoffset_30_50_prof->GetYaxis()->SetRangeUser(-0.3,0.3);
   p_areaVsoffset_30_50_prof->Draw("");
  

  
   //	
   //	2D histogram of NPV vs Rho with 15<offset<20 PU
   //
  
   c = new TCanvas("npvVsRhoOffset1515h","npvVsRhoOffset1515h"); 
   p_npvVsRho_offset_15_15h->Draw("COLZ"); 
  
  
   //	
   //	2D histogram of NPV vs Rho with 15<offset<20 PU
   //
  
  
   c = new TCanvas("npvVsRhoOffset1515hPeak","npvVsRhoOffset1515hPeak"); 
   p_npvVsRho_offset_15_15h->Draw("COLZ"); 
   //p_npvVsRho_offset_15_15h->ShowPeaks(2,"nodraw",0.2);
   //TList *functions = p_npvVsRho_offset_15_15h->GetListOfFunctions();
   //TPolyMarker *pm = (TPolyMarker*)functions->FindObject("TPolyMarker");
   Double_t peakX,peakY;
   //peakX = *(pm->GetX());peakY = *(pm->GetY());
   //cout <<peakX<<" "<<peakY<<endl;
   peakX = 14.5; peakY = 16.5;
   TH1F *p_npvVsRho_offset_15_15h_x = new TH1F("p_npvVsRho_offset_15_15h_x","p_npvVsRho_offset_15_15h_x;Rho;Events/Bin",80,0,80);
   for (int it=0;it<= p_npvVsRho_offset_15_15h->GetXaxis()->GetNbins() ; it++)
   {
      p_npvVsRho_offset_15_15h_x->SetBinContent(it,p_npvVsRho_offset_15_15h->GetBinContent(it,(int)peakY));
   }
   TH1F *p_npvVsRho_offset_15_15h_y = new TH1F("p_npvVsRho_offset_15_15h_y","p_npvVsRho_offset_15_15h_y;N_{PV};Events/Bin",80,0,80);
   for (int it=0;it<= p_npvVsRho_offset_15_15h->GetYaxis()->GetNbins() ; it++)
   {
      p_npvVsRho_offset_15_15h_y->SetBinContent(it,p_npvVsRho_offset_15_15h->GetBinContent((int)peakX,it));
   }
   c = new TCanvas("npvVsRhoOffset1515hX","npvVsRhoOffset1515hX"); 
   p_npvVsRho_offset_15_15h_x->Draw();
   p_npvVsRho_offset_15_15h_x->Fit("gaus");
   c = new TCanvas("npvVsRhoOffset1515hY","npvVsRhoOffset1515hY"); 
   p_npvVsRho_offset_15_15h_y->Draw();
   p_npvVsRho_offset_15_15h_y->Fit("gaus");
  
  
  
   //
   //	Profile of dr vs. refpt for the matched jets 
   //
  
   c = new TCanvas("drVsrefptMatchedJets","drVsrefptMatchedJets"); 
   p_drVsrefpt->SetErrorOption("s");
   c->SetLogx();
   p_drVsrefpt->Draw();
   p_drVsrefpt->GetYaxis()->SetRangeUser(0,0.3);
   p_drVsrefpt->GetYaxis()->SetTitle("<#DeltaR> #pm #sigma(#DeltaR)");

  
  
   //
   //	Profile of npv and rho vs offset PU
   //
   c = new TCanvas("npvrhoVsOffset","npvrhoVsOffset"); 
  
  
   TProfile *p_npvVsOff_prof = p_npvVsOff->ProfileX();
   TProfile *p_rhoVsOff_prof = p_rhoVsOff->ProfileX();
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
  
   // Profiles of npv, rho, tnpu vs themselves
   c = new TCanvas("XVsX","XVsX {x=N_{PV},#rho,TNPU}",1200,400);
   c->Divide(3,1);
   c->cd(1);
   p_npvVsNpv->SetErrorOption("s");
   setHistoColor(p_npvVsNpv,colPU);
   p_npvVsNpv->Draw("E1");
   p_npvVsNpv->GetYaxis()->SetRangeUser(0,45);
   //p_npvVsNpv->GetYaxis()->SetTitle("<X> #pm #sigma(X)");
   leg = new TLegend(0.2,0.72,0.4,0.92);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   leg->AddEntry(p_npvVsNpv," N_{PV}","lep");
   leg->Draw();
   c->cd(2);
   p_rhoVsRho->SetErrorOption("s");
   setHistoColor(p_rhoVsRho,colPU);
   p_rhoVsRho->Draw("E1");
   p_rhoVsRho->GetYaxis()->SetRangeUser(0,45);
   //p_rhoVsRho->GetYaxis()->SetTitle("<X> #pm #sigma(X)");
   leg = new TLegend(0.2,0.72,0.4,0.92);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   leg->AddEntry(p_rhoVsRho," #rho","lep");
   leg->Draw();
   c->cd(3);
   p_tnpuVsTnpu->SetErrorOption("s");
   setHistoColor(p_tnpuVsTnpu,colPU);
   p_tnpuVsTnpu->Draw("E1");
   p_tnpuVsTnpu->GetYaxis()->SetRangeUser(0,45);
   //p_tnpuVsTnpu->GetYaxis()->SetTitle("<X> #pm #sigma(X)");
   leg = new TLegend(0.2,0.72,0.4,0.92);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   leg->AddEntry(p_tnpuVsTnpu," TNPU","lep");
   leg->Draw();

   //
   //	2D plot of npv vs offset PU
   //
   c = new TCanvas("npvVsOffset2D","npvVsOffset2D"); 
  
   p_npvVsOff->SetTitle("2D Histogram of N_{PV} and <p_{T} Offset>_{jets}, LogZ");
   p_npvVsOff->GetYaxis()->SetRangeUser(0,45);
//  p_npvVsOff->GetXaxis()->SetRangeUser(0,45);
   p_npvVsOff->GetYaxis()->SetTitle("N_{PV}");
   p_npvVsOff->GetXaxis()->SetTitle("<p_{T} Offset>_{jets}");
   p_npvVsOff->Draw("CONTZ");
   c->SetLogz();
  
   //
   //	2D plot of rho vs offset PU
   //
   c = new TCanvas("rhoVsOffset2D","rhoVsOffset2D"); 
  
   p_rhoVsOff->SetTitle("2D Histogram of N_{PV} and <p_{T} Offset>_{jets}, LogZ");
   p_rhoVsOff->GetYaxis()->SetRangeUser(0,45);
//  p_rhoVsOff->GetXaxis()->SetRangeUser(0,45);
   p_rhoVsOff->GetYaxis()->SetTitle("Rho");
   p_rhoVsOff->GetXaxis()->SetTitle("<p_{T} Offset>_{jets}");
   p_rhoVsOff->Draw("CONTZ");
   c->SetLogz();
  
  
   //
   //	Profile of npv vs offset PU  breakdown into detector parts
   //
  
  
   c = new TCanvas("npvVsOffset","npvVsOffset"); 
   TProfile * hnpvOff_prof[4];
   hnpvOff_prof[0] = p_npvVsOff_bb->ProfileX();
   hnpvOff_prof[1] = p_npvVsOff_ei->ProfileX();
   hnpvOff_prof[2] = p_npvVsOff_eo->ProfileX();
   hnpvOff_prof[3] = p_npvVsOff_ff->ProfileX();
  
   for (int det=0;det<4;det++)
   {
      hnpvOff_prof[det]->SetErrorOption("s"); 
      setHistoColor(hnpvOff_prof[det],colDet[det]);
      hnpvOff_prof[det]->GetYaxis()->SetRangeUser(0,45);
   }
   hnpvOff_prof[0]->GetYaxis()->SetTitle("<N_{PV}> #pm #sigma(N_{PV})");
   hnpvOff_prof[0]->Draw("E1");
   for (int det=1;det<4;det++)
   {
      hnpvOff_prof[det]->Draw("sameE1");
   }
   leg = new TLegend(0.2,0.72,0.4,0.92);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   for (int det=0;det<4;det++)
   {
      leg->AddEntry(hnpvOff_prof[det],detName[det],"lep");
   }
   leg->Draw();


   //
   //	Profile of rho vs offset PU  breakdown into detector parts
   //
  
  
   c = new TCanvas("rhoVsOffset","rhoVsOffset"); 
   TProfile * hrhoOff_prof[4];
   hrhoOff_prof[0] = p_rhoVsOff_bb->ProfileX();
   hrhoOff_prof[1] = p_rhoVsOff_ei->ProfileX();
   hrhoOff_prof[2] = p_rhoVsOff_eo->ProfileX();
   hrhoOff_prof[3] = p_rhoVsOff_ff->ProfileX();
  
   for (int det=0;det<4;det++)
   {
      hrhoOff_prof[det]->SetErrorOption("s"); 
      setHistoColor(hrhoOff_prof[det],colDet[det]);
      hrhoOff_prof[det]->GetYaxis()->SetRangeUser(0,45);
   }
   hrhoOff_prof[0]->GetYaxis()->SetTitle("<Rho> #pm #sigma(Rho)");
   hrhoOff_prof[0]->Draw("E1");
   for (int det=1;det<4;det++)
   {
      hrhoOff_prof[det]->Draw("sameE1");
   }
   leg = new TLegend(0.2,0.72,0.4,0.92);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   for (int det=0;det<4;det++)
   {
      leg->AddEntry(hrhoOff_prof[det],detName[det],"lep");
   }
   leg->Draw();
  
  
  
   //
   //	Jet Energy Resolution (sigma(pt/ptref)/mean(pt/ptref) vs. ptref) PU  breakdown into detector parts
   //
  
  

   TH2 * hresResPt[4];
   hresResPt[0] = p_resVsrefpt_bb;
   hresResPt[1] = p_resVsrefpt_ei;
   hresResPt[2] = p_resVsrefpt_eo;
   hresResPt[3] = p_resVsrefpt_ff;
  
   c = getCanvasResponseResolution("PUresponseResolutionVsptref",algo, "#sigma(p_{T}^{pu}/p_{T}^{ref})/<p_{T}^{pu}/p_{T}^{ref}>",hresResPt);
   c->Draw();
  


   hresResPt[0] = np_resVsrefpt_bb;
   hresResPt[1] = np_resVsrefpt_ei;
   hresResPt[2] = np_resVsrefpt_eo;
   hresResPt[3] = np_resVsrefpt_ff;
  
   c = getCanvasResponseResolution("NoPUresponseResolutionVsptref",algo, "#sigma(p_{T}^{nopu}/p_{T}^{ref})/<p_{T}^{nopu}/p_{T}^{ref}>",hresResPt);
   c->Draw();
  
  

   //
   //	profile # of matchedjet vs offset
   //
 
   c = new TCanvas("MatchedJetOffset","MatchedJetOffset"); 
   p_matchedjet_off->SetErrorOption("s"); 
   p_matchedjet_off->Draw();



   // Offset PT energy distribution, constructed from  pt(pu)-pt(nopu) VS NPV
   c = new TCanvas("OffsetDistributionVsNPV","OffsetDistributionVsNPV");
   p_off_etaVsNpv->GetYaxis()->SetNdivisions(6);
   p_off_etaVsNpv->Draw("lego2");
   p_off_etaVsNpv->GetZaxis()->SetRangeUser(-10,50);

   // Offset PT energy distribution, constructed from  pt(pu)-pt(nopu) VS Rho
   c = new TCanvas("OffsetDistributionVsRho","OffsetDistributionVsRho");
   p_off_etaVsRho->GetYaxis()->SetNdivisions(6);
   p_off_etaVsRho->Draw("lego2");
   p_off_etaVsRho->GetZaxis()->SetRangeUser(-10,50);
  
  
   // Offset PT energy distribution, constructed from  pt(pu)-pt(nopu) VS PUEff
   c = new TCanvas("OffsetDistributionVsPUEff","OffsetDistributionVsPUEff");
   p_off_etaVsPUEff->Draw("lego2");
   p_off_etaVsPUEff->GetZaxis()->SetRangeUser(-10,50);


   // Offset PT energy distribution, constructed from  pt(pu)-pt(nopu) VS GenSumPtOA
   c = new TCanvas("OffsetDistributionVsGenSumPtOA","OffsetDistributionVsGenSumPtOA");
   p_off_etaVsGenSumPtOA->GetYaxis()->SetTitle("SumPt/jetArea");
   p_off_etaVsGenSumPtOA->GetYaxis()->SetNdivisions(6);
   p_off_etaVsGenSumPtOA->GetYaxis()->SetTitleOffset(1.3);
   p_off_etaVsGenSumPtOA->Draw("lego2");
   p_off_etaVsGenSumPtOA->GetZaxis()->SetRangeUser(-10,30);

   // Offset PT energy distribution, constructed from  pt(pu)-pt(nopu) VS JetPt
   c = new TCanvas("OffsetDistributionVsJetPt","OffsetDistributionVsJetPt");
   p_off_etaVsJetPt->Draw("lego2");
   c->SetLogy();
   p_off_etaVsJetPt->GetZaxis()->SetRangeUser(-40,100);

   // OffsetOverArea PT energy distribution, constructed from  (pt(pu)-pt(nopu))/area(pu) VS JetPt
   c = new TCanvas("OffsetOverAreaDistributionVsJetPt","OffsetOverAreaDistributionVsJetPt");
   p_offOverA_etaVsJetPt->Draw("lego2");
   c->SetLogy();
   p_offOverA_etaVsJetPt->GetZaxis()->SetRangeUser(-40,100);




   // do the fitting in each eta range and return the parameters. 
   // the last parameter is the name of the file name with which all functions are saved to.
   c = getCanvasFromFittingProcedure("ParametersVsNpv",p_off_etaVsNpv,"fittingFunctionsNpv_"+algo+".root");
   if(c)
      c->Draw();
   fin->cd();

   // do the fitting in each eta range and return the parameters
   c = getCanvasFromFittingProcedure("ParametersVsRho",p_off_etaVsRho,"fittingFunctionsRho_"+algo+".root");
   if(c)
      c->Draw();
   fin->cd();
  
   // do the fitting in each eta range and return the parameters. 
   // the last parameter is the name of the file name with which all functions are saved to.
   c = getCanvasFromFittingProcedure("ParametersVsPUEff",p_off_etaVsPUEff,"fittingFunctionsPUEff_"+algo+".root");
   if(c)
      c->Draw();
   fin->cd();
  
  
   // do the fitting in each eta range and return the parameters. 
   // the last parameter is the name of the file name with which all functions are saved to.
   c = getCanvasFromFittingProcedure("ParametersVsGenSumPtOA",p_off_etaVsGenSumPtOA,"fittingFunctionsGenSumPtOA_"+algo+".root");
   if(c)
      c->Draw();
   fin->cd();

   // do the fitting in each eta range and return the parameters. 
   // the last parameter is the name of the file name with which all functions are saved to.
   c = getCanvasFromFittingProcedure("ParametersOffOverAVsJetPt",p_offOverA_etaVsJetPt,"fittingFunctionsOffOverAJetPt_"+algo+".root");
//  c->SetLogy();
   if(c)
      c->Draw();
   fin->cd();


   // get the canvas from the resolution for bb
   TH2 * hResRho[6];
   TH2 * hOffRho[6];
   TH2 * hOffPdgid[7];
   hResRho[0] = p_resnopuVsrefpt_bb_rho0_4  ;  
   hResRho[1] = p_resnopuVsrefpt_bb_rho5_9  ;
   hResRho[2] = p_resnopuVsrefpt_bb_rho10_14;
   hResRho[3] = p_resnopuVsrefpt_bb_rho15_19;
   hResRho[4] = p_resnopuVsrefpt_bb_rho20_24;
   hResRho[5] = p_resnopuVsrefpt_bb_rho25_29;
   c = getCanvasResolution("ResolutionRho_BB",algo, "#sigma(p_{T}/p_{T}^{nopu})/<p_{T}/p_{T}^{nopu}>",hResRho,0);
   c->Draw();

   hResRho[0] = p_resnopuVsrefpt_ei_rho0_4  ;
   hResRho[1] = p_resnopuVsrefpt_ei_rho5_9  ;
   hResRho[2] = p_resnopuVsrefpt_ei_rho10_14;
   hResRho[3] = p_resnopuVsrefpt_ei_rho15_19;
   hResRho[4] = p_resnopuVsrefpt_ei_rho20_24;
   hResRho[5] = p_resnopuVsrefpt_ei_rho25_29;
   c = getCanvasResolution("ResolutionRho_EI",algo, "#sigma(p_{T}/p_{T}^{nopu})/<p_{T}/p_{T}^{nopu}>",hResRho,0);
   c->Draw();


   hResRho[0] = p_resnopuVsrefpt_eo_rho0_4  ;
   hResRho[1] = p_resnopuVsrefpt_eo_rho5_9  ;
   hResRho[2] = p_resnopuVsrefpt_eo_rho10_14;
   hResRho[3] = p_resnopuVsrefpt_eo_rho15_19;
   hResRho[4] = p_resnopuVsrefpt_eo_rho20_24;
   hResRho[5] = p_resnopuVsrefpt_eo_rho25_29;
   c = getCanvasResolution("ResolutionRho_EO",algo,"#sigma(p_{T}/p_{T}^{nopu})/<p_{T}/p_{T}^{nopu}>",hResRho,0);
   c->Draw();


   hResRho[0] = p_resnopuVsrefpt_ff_rho0_4  ;
   hResRho[1] = p_resnopuVsrefpt_ff_rho5_9  ;
   hResRho[2] = p_resnopuVsrefpt_ff_rho10_14;
   hResRho[3] = p_resnopuVsrefpt_ff_rho15_19;
   hResRho[4] = p_resnopuVsrefpt_ff_rho20_24;
   hResRho[5] = p_resnopuVsrefpt_ff_rho25_29;
   c = getCanvasResolution("ResolutionRho_FF",algo,"#sigma(p_{T}/p_{T}^{nopu})/<p_{T}/p_{T}^{nopu}>",hResRho,0);
   c->Draw();

   // The same plots for resolution to respect to refpt
   // get the canvas from the resolution for bb
   hResRho[0] = p_resVsrefpt_bb_rho0_4  ;  
   hResRho[1] = p_resVsrefpt_bb_rho5_9  ;
   hResRho[2] = p_resVsrefpt_bb_rho10_14;
   hResRho[3] = p_resVsrefpt_bb_rho15_19;
   hResRho[4] = p_resVsrefpt_bb_rho20_24;
   hResRho[5] = p_resVsrefpt_bb_rho25_29;
   c = getCanvasResolution("ResolutionRhoRef_BB",algo,"#sigma(p_{T}/p_{T}^{ref})/<p_{T}/p_{T}^{ref}>",hResRho,0);
   c->Draw();

   hResRho[0] = p_resVsrefpt_ei_rho0_4  ;
   hResRho[1] = p_resVsrefpt_ei_rho5_9  ;
   hResRho[2] = p_resVsrefpt_ei_rho10_14;
   hResRho[3] = p_resVsrefpt_ei_rho15_19;
   hResRho[4] = p_resVsrefpt_ei_rho20_24;
   hResRho[5] = p_resVsrefpt_ei_rho25_29;
   c = getCanvasResolution("ResolutionRhoRef_EI",algo,"#sigma(p_{T}/p_{T}^{ref})/<p_{T}/p_{T}^{ref}>",hResRho,0);
   c->Draw();


   hResRho[0] = p_resVsrefpt_eo_rho0_4  ;
   hResRho[1] = p_resVsrefpt_eo_rho5_9  ;
   hResRho[2] = p_resVsrefpt_eo_rho10_14;
   hResRho[3] = p_resVsrefpt_eo_rho15_19;
   hResRho[4] = p_resVsrefpt_eo_rho20_24;
   hResRho[5] = p_resVsrefpt_eo_rho25_29;
   c = getCanvasResolution("ResolutionRhoRef_EO",algo,"#sigma(p_{T}/p_{T}^{ref})/<p_{T}/p_{T}^{ref}>",hResRho,0);
   c->Draw();


   hResRho[0] = p_resVsrefpt_ff_rho0_4  ;
   hResRho[1] = p_resVsrefpt_ff_rho5_9  ;
   hResRho[2] = p_resVsrefpt_ff_rho10_14;
   hResRho[3] = p_resVsrefpt_ff_rho15_19;
   hResRho[4] = p_resVsrefpt_ff_rho20_24;
   hResRho[5] = p_resVsrefpt_ff_rho25_29;
   c = getCanvasResolution("ResolutionRhoRef_FF",algo,"#sigma(p_{T}/p_{T}^{ref})/<p_{T}/p_{T}^{ref}>",hResRho,0);
   c->Draw();


   //Resolution of response for hard scatter
   // get the canvas from the resolution for bb
   hResRho[0] = p_nopuresVsrefpt_bb_rho0_4  ;  
   hResRho[1] = p_nopuresVsrefpt_bb_rho5_9  ;
   hResRho[2] = p_nopuresVsrefpt_bb_rho10_14;
   hResRho[3] = p_nopuresVsrefpt_bb_rho15_19;
   hResRho[4] = p_nopuresVsrefpt_bb_rho20_24;
   hResRho[5] = p_nopuresVsrefpt_bb_rho25_29;
   c = getCanvasResolution("ResolutionRhoNoPU_BB",algo, "#sigma(p_{T}^{nopu}/p_{T}^{ref})/<p_{T}^{nopu}/p_{T}^{ref}>",hResRho,0);
   if(c)
      c->Draw();

   hResRho[0] = p_nopuresVsrefpt_ei_rho0_4  ;
   hResRho[1] = p_nopuresVsrefpt_ei_rho5_9  ;
   hResRho[2] = p_nopuresVsrefpt_ei_rho10_14;
   hResRho[3] = p_nopuresVsrefpt_ei_rho15_19;
   hResRho[4] = p_nopuresVsrefpt_ei_rho20_24;
   hResRho[5] = p_nopuresVsrefpt_ei_rho25_29;
   c = getCanvasResolution("ResolutionRhoNoPU_EI",algo, "#sigma(p_{T}^{nopu}/p_{T}^{ref})/<p_{T}^{nopu}/p_{T}^{ref}>",hResRho,0);
   c->Draw();


   hResRho[0] = p_nopuresVsrefpt_eo_rho0_4  ;
   hResRho[1] = p_nopuresVsrefpt_eo_rho5_9  ;
   hResRho[2] = p_nopuresVsrefpt_eo_rho10_14;
   hResRho[3] = p_nopuresVsrefpt_eo_rho15_19;
   hResRho[4] = p_nopuresVsrefpt_eo_rho20_24;
   hResRho[5] = p_nopuresVsrefpt_eo_rho25_29;
   c = getCanvasResolution("ResolutionRhoNoPU_EO",algo,"#sigma(p_{T}^{nopu}/p_{T}^{ref})/<p_{T}^{nopu}/p_{T}^{ref}>",hResRho,0);
   c->Draw();


   hResRho[0] = p_nopuresVsrefpt_ff_rho0_4  ;
   hResRho[1] = p_nopuresVsrefpt_ff_rho5_9  ;
   hResRho[2] = p_nopuresVsrefpt_ff_rho10_14;
   hResRho[3] = p_nopuresVsrefpt_ff_rho15_19;
   hResRho[4] = p_nopuresVsrefpt_ff_rho20_24;
   hResRho[5] = p_nopuresVsrefpt_ff_rho25_29;
   c = getCanvasResolution("ResolutionRhoNoPU_FF",algo,"#sigma(p_{T}^{nopu}/p_{T}^{ref})/<p_{T}^{nopu}/p_{T}^{ref}>",hResRho,0);
   c->Draw();


   //Resolution of response for PU
   // get the canvas from the resolution for bb
   hResRho[0] = p_offresOrefptVsrefpt_bb_rho0_4  ;  
   hResRho[1] = p_offresOrefptVsrefpt_bb_rho5_9  ;
   hResRho[2] = p_offresOrefptVsrefpt_bb_rho10_14;
   hResRho[3] = p_offresOrefptVsrefpt_bb_rho15_19;
   hResRho[4] = p_offresOrefptVsrefpt_bb_rho20_24;
   hResRho[5] = p_offresOrefptVsrefpt_bb_rho25_29;
   c = getCanvasResolution("ResolutionOffResRho_BB",algo, "#sigma((p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{ref})/<(p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{ref}>",hResRho,0);
   c->Draw();

   hResRho[0] = p_offresOrefptVsrefpt_ei_rho0_4  ;  
   hResRho[1] = p_offresOrefptVsrefpt_ei_rho5_9  ;
   hResRho[2] = p_offresOrefptVsrefpt_ei_rho10_14;
   hResRho[3] = p_offresOrefptVsrefpt_ei_rho15_19;
   hResRho[4] = p_offresOrefptVsrefpt_ei_rho20_24;
   hResRho[5] = p_offresOrefptVsrefpt_ei_rho25_29;
   c = getCanvasResolution("ResolutionOffResRho_EI",algo, "#sigma((p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{ref})/<(p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{ref}>",hResRho,0);
   c->Draw();

   hResRho[0] = p_offresOrefptVsrefpt_eo_rho0_4  ;  
   hResRho[1] = p_offresOrefptVsrefpt_eo_rho5_9  ;
   hResRho[2] = p_offresOrefptVsrefpt_eo_rho10_14;
   hResRho[3] = p_offresOrefptVsrefpt_eo_rho15_19;
   hResRho[4] = p_offresOrefptVsrefpt_eo_rho20_24;
   hResRho[5] = p_offresOrefptVsrefpt_eo_rho25_29;
   c = getCanvasResolution("ResolutionOffResRho_EO",algo, "#sigma((p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{ref})/<(p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{ref}>",hResRho,0);
   c->Draw();

   hResRho[0] = p_offresOrefptVsrefpt_ff_rho0_4  ;  
   hResRho[1] = p_offresOrefptVsrefpt_ff_rho5_9  ;
   hResRho[2] = p_offresOrefptVsrefpt_ff_rho10_14;
   hResRho[3] = p_offresOrefptVsrefpt_ff_rho15_19;
   hResRho[4] = p_offresOrefptVsrefpt_ff_rho20_24;
   hResRho[5] = p_offresOrefptVsrefpt_ff_rho25_29;
   c = getCanvasResolution("ResolutionOffResRho_FF",algo, "#sigma((p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{ref})/<(p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{ref}>",hResRho,0);
   c->Draw();


   hResRho[0] = p_offResVsrefpt_bb_chf  ;  
   hResRho[1] = p_offResVsrefpt_bb_nhf  ;
   hResRho[2] = p_offResVsrefpt_bb_nef;
   hResRho[3] = p_offResVsrefpt_bb_cef;
   hResRho[4] = p_offResVsrefpt_bb_hfhf;
   hResRho[5] = p_offResVsrefpt_bb_hfef;
   c = getCanvasResolution("ResolutionOffRefPF_BB",algo,"#sigma(p_{T}^{pu}-p_{T}^{nopu})",hResRho,1);
   c->Draw();  
   c = getGausMeanOffset("MeanOffRefPF_BB","<p_{T}^{pu}-p_{T}^{nopu}>",algo,hResRho,fixedRange);
   c->Draw();
   c = getGausMeanOffsetWithSum("MeanOffRefPF_BB","<p_{T}^{pu}-p_{T}^{nopu}>",algo,hResRho,p_offResVsrefpt_bb_all,fixedRange);
   c->Draw();

   // get the canvas from the resolution for bb
   hResRho[0] = p_resVsrefpt_bb_rho0_4  ;  
   hResRho[1] = p_resVsrefpt_bb_rho5_9  ;
   hResRho[2] = p_resVsrefpt_bb_rho10_14;
   hResRho[3] = p_resVsrefpt_bb_rho15_19;
   hResRho[4] = p_resVsrefpt_bb_rho20_24;
   hResRho[5] = p_resVsrefpt_bb_rho25_29;
   hOffRho[0] = p_offresVsrefpt_bb_rho0_4  ;
   hOffRho[1] = p_offresVsrefpt_bb_rho5_9  ;
   hOffRho[2] = p_offresVsrefpt_bb_rho10_14;
   hOffRho[3] = p_offresVsrefpt_bb_rho15_19;
   hOffRho[4] = p_offresVsrefpt_bb_rho20_24;
   hOffRho[5] = p_offresVsrefpt_bb_rho25_29;
  
   c = getCanvasResolution_v2("OffResolutionRhoRef_BB",algo,"#sigma(p_{T}^{PU}-p_{T}^{noPU})/<p_{T}^{noPU}/p_{T}^{ref}>",hResRho,hOffRho);
   c->Draw();
   c = getGausMeanOffset("OffMeanrhoRef_BB","<offset>",algo,hOffRho,fixedRange);
   c->Draw();
   c = getGausMeanOffsetOverPtref("OffMeanOverPtrhoRef_BB","<offset>/p_{T}^{ref}",algo,hOffRho,fixedRange);
   c->Draw();

   hOffRho[0] = p_offresVsrefpt_bb_tnpu0_4  ;
   hOffRho[1] = p_offresVsrefpt_bb_tnpu5_9  ;
   hOffRho[2] = p_offresVsrefpt_bb_tnpu10_14;
   hOffRho[3] = p_offresVsrefpt_bb_tnpu15_19;
   hOffRho[4] = p_offresVsrefpt_bb_tnpu20_24;
   hOffRho[5] = p_offresVsrefpt_bb_tnpu25_29;

   c = getGausMeanOffset("OffMeantnpuRef_BB","<offset>",algo,hOffRho,fixedRange);
   c->Draw();
   c = getGausMeanOffsetWithSum("OffMeantnpuRefWithSum_BB","<offset>",algo,hOffRho,p_offresVsrefpt_bb_tnpu0_29,fixedRange);
   c->Draw();
   c = getGausMeanOffsetOverPtref("OffMeanOverPttnpuRef_BB","<offset>/p_{T}^{ref}",algo,hOffRho,fixedRange);
   c->Draw();


   hResRho[0] = p_resVsrefpt_ei_rho0_4  ;  
   hResRho[1] = p_resVsrefpt_ei_rho5_9  ;
   hResRho[2] = p_resVsrefpt_ei_rho10_14;
   hResRho[3] = p_resVsrefpt_ei_rho15_19;
   hResRho[4] = p_resVsrefpt_ei_rho20_24;
   hResRho[5] = p_resVsrefpt_ei_rho25_29;
   hOffRho[0] = p_offresVsrefpt_ei_rho0_4  ;  
   hOffRho[1] = p_offresVsrefpt_ei_rho5_9  ;
   hOffRho[2] = p_offresVsrefpt_ei_rho10_14;
   hOffRho[3] = p_offresVsrefpt_ei_rho15_19;
   hOffRho[4] = p_offresVsrefpt_ei_rho20_24;
   hOffRho[5] = p_offresVsrefpt_ei_rho25_29;
  
   c = getCanvasResolution_v2("OffResolutionRhoRef_EI",algo,"#sigma(p_{T}^{PU}-p_{T}^{noPU})/<p_{T}^{noPU}/p_{T}^{ref}>",hResRho,hOffRho);
   c->Draw();
   c = getGausMeanOffset("OffMeanrhoRef_EI","<offset>",algo,hOffRho,fixedRange);
   c->Draw();

   hOffRho[0] = p_offresVsrefpt_ei_tnpu0_4  ;
   hOffRho[1] = p_offresVsrefpt_ei_tnpu5_9  ;
   hOffRho[2] = p_offresVsrefpt_ei_tnpu10_14;
   hOffRho[3] = p_offresVsrefpt_ei_tnpu15_19;
   hOffRho[4] = p_offresVsrefpt_ei_tnpu20_24;
   hOffRho[5] = p_offresVsrefpt_ei_tnpu25_29;

   c = getGausMeanOffset("OffMeantnpuRef_EI","<offset>",algo,hOffRho,fixedRange);
   c->Draw();
   c = getGausMeanOffsetWithSum("OffMeantnputRefWithSum_EI","<offset>",algo,hOffRho,p_offresVsrefpt_ei_tnpu0_29,fixedRange);
   c->Draw();
   c = getGausMeanOffsetOverPtref("OffMeanOverPttnpuRef_EI","<offset>/p_{T}^{ref}",algo,hOffRho,fixedRange);
   c->Draw();
  

   hResRho[0] = p_resVsrefpt_eo_rho0_4  ;  
   hResRho[1] = p_resVsrefpt_eo_rho5_9  ;
   hResRho[2] = p_resVsrefpt_eo_rho10_14;
   hResRho[3] = p_resVsrefpt_eo_rho15_19;
   hResRho[4] = p_resVsrefpt_eo_rho20_24;
   hResRho[5] = p_resVsrefpt_eo_rho25_29;
   hOffRho[0] = p_offresVsrefpt_eo_rho0_4  ;  
   hOffRho[1] = p_offresVsrefpt_eo_rho5_9  ;
   hOffRho[2] = p_offresVsrefpt_eo_rho10_14;
   hOffRho[3] = p_offresVsrefpt_eo_rho15_19;
   hOffRho[4] = p_offresVsrefpt_eo_rho20_24;
   hOffRho[5] = p_offresVsrefpt_eo_rho25_29;
  
   c = getCanvasResolution_v2("OffResolutionRhoRef_EO",algo,"#sigma(p_{T}^{PU}-p_{T}^{noPU})/<p_{T}^{noPU}/p_{T}^{ref}>",hResRho,hOffRho);
   c->Draw();
   c = getGausMeanOffset("OffMeanrhoRef_EO","<offset>",algo,hOffRho,fixedRange);
   c->Draw();  
  
   hOffRho[0] = p_offresVsrefpt_eo_tnpu0_4  ;
   hOffRho[1] = p_offresVsrefpt_eo_tnpu5_9  ;
   hOffRho[2] = p_offresVsrefpt_eo_tnpu10_14;
   hOffRho[3] = p_offresVsrefpt_eo_tnpu15_19;
   hOffRho[4] = p_offresVsrefpt_eo_tnpu20_24;
   hOffRho[5] = p_offresVsrefpt_eo_tnpu25_29;

   c = getGausMeanOffset("OffMeantnpuRef_EO","<offset>",algo,hOffRho,fixedRange);
   c->Draw();
   c = getGausMeanOffsetWithSum("OffMeantnputRefWithSum_EO","<offset>",algo,hOffRho,p_offresVsrefpt_eo_tnpu0_29,fixedRange);
   c->Draw();
   c = getGausMeanOffsetOverPtref("OffMeanOverPttnpuRef_EO","<offset>/p_{T}^{ref}",algo,hOffRho,fixedRange);
   c->Draw();

   hResRho[0] = p_resVsrefpt_ff_rho0_4  ;  
   hResRho[1] = p_resVsrefpt_ff_rho5_9  ;
   hResRho[2] = p_resVsrefpt_ff_rho10_14;
   hResRho[3] = p_resVsrefpt_ff_rho15_19;
   hResRho[4] = p_resVsrefpt_ff_rho20_24;
   hResRho[5] = p_resVsrefpt_ff_rho25_29;
   hOffRho[0] = p_offresVsrefpt_ff_rho0_4  ;  
   hOffRho[1] = p_offresVsrefpt_ff_rho5_9  ;
   hOffRho[2] = p_offresVsrefpt_ff_rho10_14;
   hOffRho[3] = p_offresVsrefpt_ff_rho15_19;
   hOffRho[4] = p_offresVsrefpt_ff_rho20_24;
   hOffRho[5] = p_offresVsrefpt_ff_rho25_29;
  
   c = getCanvasResolution_v2("OffResolutionRhoRef_FF",algo,"#sigma(p_{T}^{PU}-p_{T}^{noPU})/<p_{T}^{noPU}/p_{T}^{ref}>",hResRho,hOffRho);
   c->Draw(); 
   c = getGausMeanOffset("OffMeanrhoRef_FF","<offset>",algo,hOffRho,fixedRange);
   c->Draw();

   hOffRho[0] = p_offresVsrefpt_ff_tnpu0_4  ;
   hOffRho[1] = p_offresVsrefpt_ff_tnpu5_9  ;
   hOffRho[2] = p_offresVsrefpt_ff_tnpu10_14;
   hOffRho[3] = p_offresVsrefpt_ff_tnpu15_19;
   hOffRho[4] = p_offresVsrefpt_ff_tnpu20_24;
   hOffRho[5] = p_offresVsrefpt_ff_tnpu25_29;

   c = getGausMeanOffset("OffMeantnpuRef_FF","<offset>",algo,hOffRho,fixedRange);
   c->Draw();
   c = getGausMeanOffsetWithSum("OffMeantnputRefWithSum_FF","<offset>",algo,hOffRho,p_offresVsrefpt_ff_tnpu0_29,fixedRange);
   c->Draw();
   c = getGausMeanOffsetOverPtref("OffMeanOverPttnpuRef_FF","<offset>/p_{T}^{ref}",algo,hOffRho,fixedRange);
   c->Draw();
 

   //Resolution of response for PU
   // get the canvas from the resolution for bb
   hResRho[0] = p_offresOrefptVsrefpt_bb_npv0_4  ;  
   hResRho[1] = p_offresOrefptVsrefpt_bb_npv5_9  ;
   hResRho[2] = p_offresOrefptVsrefpt_bb_npv10_14;
   hResRho[3] = p_offresOrefptVsrefpt_bb_npv15_19;
   hResRho[4] = p_offresOrefptVsrefpt_bb_npv20_24;
   hResRho[5] = p_offresOrefptVsrefpt_bb_npv25_29;
   c = getCanvasResolution("ResolutionOffResNpv_BB",algo, "#sigma((p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{ref})/<(p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{ref}>",hResRho,0);
   c->Draw();

   hResRho[0] = p_offresOrefptVsrefpt_ei_npv0_4  ;  
   hResRho[1] = p_offresOrefptVsrefpt_ei_npv5_9  ;
   hResRho[2] = p_offresOrefptVsrefpt_ei_npv10_14;
   hResRho[3] = p_offresOrefptVsrefpt_ei_npv15_19;
   hResRho[4] = p_offresOrefptVsrefpt_ei_npv20_24;
   hResRho[5] = p_offresOrefptVsrefpt_ei_npv25_29;
   c = getCanvasResolution("ResolutionOffResNpv_EI",algo, "#sigma((p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{ref})/<(p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{ref}>",hResRho,0);
   c->Draw();

   hResRho[0] = p_offresOrefptVsrefpt_eo_npv0_4  ;  
   hResRho[1] = p_offresOrefptVsrefpt_eo_npv5_9  ;
   hResRho[2] = p_offresOrefptVsrefpt_eo_npv10_14;
   hResRho[3] = p_offresOrefptVsrefpt_eo_npv15_19;
   hResRho[4] = p_offresOrefptVsrefpt_eo_npv20_24;
   hResRho[5] = p_offresOrefptVsrefpt_eo_npv25_29;
   c = getCanvasResolution("ResolutionOffResNpv_EO",algo, "#sigma((p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{ref})/<(p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{ref}>",hResRho,0);
   c->Draw();

   hResRho[0] = p_offresOrefptVsrefpt_ff_npv0_4  ;  
   hResRho[1] = p_offresOrefptVsrefpt_ff_npv5_9  ;
   hResRho[2] = p_offresOrefptVsrefpt_ff_npv10_14;
   hResRho[3] = p_offresOrefptVsrefpt_ff_npv15_19;
   hResRho[4] = p_offresOrefptVsrefpt_ff_npv20_24;
   hResRho[5] = p_offresOrefptVsrefpt_ff_npv25_29;
   c = getCanvasResolution("ResolutionOffResNpv_FF",algo, "#sigma((p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{ref})/<(p_{T}^{PU}-p_{T}^{noPU})/p_{T}^{ref}>",hResRho,0);
   c->Draw();

   // get the canvas from the resolution for bb
   hResRho[0] = p_resVsrefpt_bb_npv0_4  ;  
   hResRho[1] = p_resVsrefpt_bb_npv5_9  ;
   hResRho[2] = p_resVsrefpt_bb_npv10_14;
   hResRho[3] = p_resVsrefpt_bb_npv15_19;
   hResRho[4] = p_resVsrefpt_bb_npv20_24;
   hResRho[5] = p_resVsrefpt_bb_npv25_29;
   hOffRho[0] = p_offresVsrefpt_bb_npv0_4  ;  
   hOffRho[1] = p_offresVsrefpt_bb_npv5_9  ;
   hOffRho[2] = p_offresVsrefpt_bb_npv10_14;
   hOffRho[3] = p_offresVsrefpt_bb_npv15_19;
   hOffRho[4] = p_offresVsrefpt_bb_npv20_24;
   hOffRho[5] = p_offresVsrefpt_bb_npv25_29;
   //cout <<"here0"<<endl;
   c = getCanvasResolution_v2("OffResolutionnpvRef_BB",algo,"#sigma(p_{T}^{PU}-p_{T}^{noPU})/<p_{T}^{noPU}/p_{T}^{ref}>",hResRho,hOffRho);
   c->Draw();
   //cout <<"here1"<<endl;
   c = getResolutionNumDenom("OffResolutionnpvRef_BB_N_D","bb_npv15_19",algo, hResRho[3], hOffRho[3]);
   c->Draw();

   c = getGausMeanOffset("OffMeannpvRef_BB","<offset>",algo,hOffRho,fixedRange);
   c->Draw();
   c = getGausMeanOffsetOverPtref("OffMeanOverPtnpvRef_BB","<offset>/p_{T}^{ref}",algo,hOffRho,fixedRange);
   c->Draw();

   c = getGausMeanOffsetScale("OffMeannpvRef_BB_3035","<offset>/<offset(30<pt<35)>",algo,hOffRho,binNum3035,fixedRange);
   c->Draw();
  
   c = getGausMeanOffsetScale("OffMeannpvRef_BB_2023","<offset>/<offset(20<pt<23)>",algo,hOffRho,binNum2023,fixedRange);
   c->Draw();
  
   hResRho[0] = p_resVsrefpt_ei_npv0_4  ;  
   hResRho[1] = p_resVsrefpt_ei_npv5_9  ;
   hResRho[2] = p_resVsrefpt_ei_npv10_14;
   hResRho[3] = p_resVsrefpt_ei_npv15_19;
   hResRho[4] = p_resVsrefpt_ei_npv20_24;
   hResRho[5] = p_resVsrefpt_ei_npv25_29;
   hOffRho[0] = p_offresVsrefpt_ei_npv0_4  ;  
   hOffRho[1] = p_offresVsrefpt_ei_npv5_9  ;
   hOffRho[2] = p_offresVsrefpt_ei_npv10_14;
   hOffRho[3] = p_offresVsrefpt_ei_npv15_19;
   hOffRho[4] = p_offresVsrefpt_ei_npv20_24;
   hOffRho[5] = p_offresVsrefpt_ei_npv25_29;
  
   c = getCanvasResolution_v2("OffResolutionnpvRef_EI",algo,"#sigma(p_{T}^{PU}-p_{T}^{noPU})/<p_{T}^{noPU}/p_{T}^{ref}>",hResRho,hOffRho);
   c->Draw();
  
   c = getGausMeanOffset("OffMeannpvRef_EI","<offset>",algo,hOffRho,fixedRange);
   c->Draw();
  
   c = getGausMeanOffsetScale("OffMeannpvRef_EI_3035","<offset>/<offset(30<pt<35)>",algo,hOffRho,binNum3035,fixedRange);
   c->Draw();
  
   c = getGausMeanOffsetScale("OffMeannpvRef_EI_2023","<offset>/<offset(20<pt<23)>",algo,hOffRho,binNum2023,fixedRange);
   c->Draw();  

   hResRho[0] = p_resVsrefpt_eo_npv0_4  ;  
   hResRho[1] = p_resVsrefpt_eo_npv5_9  ;
   hResRho[2] = p_resVsrefpt_eo_npv10_14;
   hResRho[3] = p_resVsrefpt_eo_npv15_19;
   hResRho[4] = p_resVsrefpt_eo_npv20_24;
   hResRho[5] = p_resVsrefpt_eo_npv25_29;
   hOffRho[0] = p_offresVsrefpt_eo_npv0_4  ;  
   hOffRho[1] = p_offresVsrefpt_eo_npv5_9  ;
   hOffRho[2] = p_offresVsrefpt_eo_npv10_14;
   hOffRho[3] = p_offresVsrefpt_eo_npv15_19;
   hOffRho[4] = p_offresVsrefpt_eo_npv20_24;
   hOffRho[5] = p_offresVsrefpt_eo_npv25_29;
  
   c = getCanvasResolution_v2("OffResolutionnpvRef_EO",algo,"#sigma(p_{T}^{PU}-p_{T}^{noPU})/<p_{T}^{noPU}/p_{T}^{ref}>",hResRho,hOffRho);
   c->Draw();
   c = getGausMeanOffset("OffMeannpvRef_EO","<offset>",algo,hOffRho,fixedRange);
   c->Draw();  
  
   c = getGausMeanOffsetScale("OffMeannpvRef_EO_3035","<offset>/<offset(30<pt<35)>",algo,hOffRho,binNum3035,fixedRange);
   c->Draw();
  
   c = getGausMeanOffsetScale("OffMeannpvRef_EO_2023","<offset>/<offset(20<pt<23)>",algo,hOffRho,binNum2023,fixedRange);
   c->Draw();  
  

   hResRho[0] = p_resVsrefpt_ff_npv0_4  ;  
   hResRho[1] = p_resVsrefpt_ff_npv5_9  ;
   hResRho[2] = p_resVsrefpt_ff_npv10_14;
   hResRho[3] = p_resVsrefpt_ff_npv15_19;
   hResRho[4] = p_resVsrefpt_ff_npv20_24;
   hResRho[5] = p_resVsrefpt_ff_npv25_29;
   hOffRho[0] = p_offresVsrefpt_ff_npv0_4  ;  
   hOffRho[1] = p_offresVsrefpt_ff_npv5_9  ;
   hOffRho[2] = p_offresVsrefpt_ff_npv10_14;
   hOffRho[3] = p_offresVsrefpt_ff_npv15_19;
   hOffRho[4] = p_offresVsrefpt_ff_npv20_24;
   hOffRho[5] = p_offresVsrefpt_ff_npv25_29;
  
   c = getCanvasResolution_v2("OffResolutionnpvRef_FF",algo,"#sigma(p_{T}^{PU}-p_{T}^{noPU})/<p_{T}^{noPU}/p_{T}^{ref}>",hResRho,hOffRho);
   c->Draw(); 
   c = getGausMeanOffset("OffMeannpvRef_FF","<offset>",algo,hOffRho,fixedRange);
   c->Draw();
  
   c = getGausMeanOffsetScale("OffMeannpvRef_FF_3035","<offset>/<offset(30<pt<35)>",algo,hOffRho,binNum3035,fixedRange);
   c->Draw();
  
   c = getGausMeanOffsetScale("OffMeannpvRef_FF_2023","<offset>/<offset(20<pt<23)>",algo,hOffRho,binNum2023,fixedRange);
   c->Draw();  


   hOffPdgid[0] = p_offresVsrefpt_bb_pdgid_nJ;
   hOffPdgid[1] = p_offresVsrefpt_bb_pdgid_qJ;
   hOffPdgid[2] = p_offresVsrefpt_bb_pdgid_cJ;
   hOffPdgid[3] = p_offresVsrefpt_bb_pdgid_bJ;
   hOffPdgid[4] = p_offresVsrefpt_bb_pdgid_gJ;
   hOffPdgid[5] = p_offresVsrefpt_bb_pdgid_aJ;
   hOffPdgid[6] = p_offresVsrefpt_bb_pdgid_aqJ;
   c = getGausMeanOffset("OffMeanpdgidRef_BB","<offset>",algo,hOffPdgid,fixedRange);
   c->Draw();

  
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
            TString canname = outDir+"/"+can->GetName()+ "_"+algo+".pdf";
            can->Print(canname);
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
   string         algo1      = cl.getValue<string>  ("algo1",     "ak5pf");
   string         algo2      = cl.getValue<string>  ("algo2",     "ak5pf");
   string         outDir     = cl.getValue<string>  ("outDir", "./images");
   bool           fixedRange = cl.getValue<bool>    ("fixedRange",   true);
   setREStyle();
   SynchPlots(algo1, algo2, outDir, fixedRange);
}

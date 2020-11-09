#include <map>
#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include <assert.h>

#include "TFile.h"
#include "TChain.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TProfile3D.h"
#include "TGraphErrors.h"
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
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/JetInfo.hh"
#include "JetMETAnalysis/JetAnalyzers/interface/REStyle.h"


using namespace std;

struct FitRes {
      double etalowedge;
      double etaupedge;
      TF2 * fit;
};

//===========================================================================
// This method returns the graph from all the TProfile3D's
TGraph2DErrors * getGraph2D(int iEta, TProfile3D * prof, const TProfile3D * profPt, const TProfile3D * profRho, TH3I * hist){
    // The returning graph, with its name and title
    TGraph2DErrors *graph  = new TGraph2DErrors();
    graph->SetName(Form("p_offOverA_RhoVsJetPt_%d",iEta));
    graph->SetTitle(Form("p_offOverA_RhoVsJetPt_%d Eta=%f-%f;Rho;p_{T}^{pu};OffsetOverArea",iEta,prof->GetXaxis()->GetBinLowEdge(iEta),prof->GetXaxis()->GetBinUpEdge(iEta)));
    int nEvt = 0;
    cout << "\tNBins (rho,pT):  (" << prof->GetYaxis()->GetNbins() << "," << prof->GetZaxis()->GetNbins() << ")" << endl;

    double nJets = prof->GetEntries();

    for (int itnpu = 1 ; itnpu <= prof->GetYaxis()->GetNbins() ; itnpu++){
      for (int irefpt = 1 ; irefpt <= prof->GetZaxis()->GetNbins() ; irefpt++){

        int Gbin = prof->GetBin(iEta, itnpu, irefpt);
        nEvt += prof->GetBinEntries(Gbin);
        // avoid points with empty content or too small error
        if (prof   ->GetBinError  (iEta,itnpu,irefpt)  > 0.000001 &&
        // hist   ->GetBinContent(iEta,itnpu,irefpt)  >=20  &&
        fabs(prof   ->GetBinContent  (iEta,itnpu,irefpt))  != TMath::Infinity() &&
        prof   ->GetBinError  (iEta,itnpu,irefpt)  < 5.0 &&
        prof   ->GetBinEffectiveEntries(Gbin)      > sqrt(nJets/(5.0E+06)) &&
        // (fabs(prof->GetBinError(iEta,itnpu,irefpt)/prof->GetBinContent(iEta,itnpu,irefpt)))<0.5 &&// 0.3 ==> 0.5
        // (fabs(prof->GetBinError(iEta,itnpu,irefpt)/prof->GetBinContent(iEta,itnpu,irefpt)))>0.01 &&// 0.05 ==> 0.01
        profPt ->GetBinContent(iEta,itnpu,irefpt)  > 0 &&
        profPt ->GetBinError  (iEta,itnpu,irefpt)  > 0.1 &&
        // profPt ->GetBinError  (iEta,itnpu,irefpt)  < 10*log10(profPt ->GetBinContent(iEta,itnpu,irefpt)) &&
        profRho->GetBinContent(iEta,itnpu,irefpt)  > 0 &&
        profRho->GetBinError  (iEta,itnpu,irefpt)  > 0.1){

          // get the relevant values
          double rho  = profRho->GetBinContent(iEta, itnpu, irefpt);
          double rhoe = profRho->GetBinError  (iEta, itnpu, irefpt);
          double pt   = profPt ->GetBinContent(iEta, itnpu, irefpt);
          double pte  = profPt ->GetBinError  (iEta, itnpu, irefpt);
          double ooa  = prof   ->GetBinContent(iEta, itnpu, irefpt);
          double ooae = prof   ->GetBinError  (iEta, itnpu, irefpt);

          // Store the values
          int n = graph->GetN();
          graph->SetPoint(n, rho, pt, ooa);
          graph->SetPointError(n, rhoe, pte, ooae);

        }//if

      } // irefpt

    }// itnpu

    cout << "\tgraph has (Entries,Evts):  (" << graph->GetN() << "," << nEvt << ")" << endl;

    return graph;

} // getGraph2D

//===========================================================================
void fitClosurePlots(TProfile3D * prof,
  const TProfile3D * profPt,
  const TProfile3D * profRho,
  vector <FitRes> fitResults,
  TH3I * hist,
  TFile* fout){

    cout<<"Creating clousure plots"<<endl;

    fout->mkdir("graphs");
    fout->mkdir("graphs/p_offOverA_EtaVsRho");
    fout->mkdir("graphs/p_offOverA_EtaVsJetPt");

    fout->mkdir("residualgraphs");
    fout->mkdir("residualgraphs/p_offOverA_EtaVsRho");
    fout->mkdir("residualgraphs/p_offOverA_EtaVsJetPt");

    fout->mkdir("histograms");
    fout->mkdir("histograms/eta");
    fout->mkdir("histograms/tnpu");
    fout->mkdir("histograms/refpt");

    fout->mkdir("residual");
    fout->mkdir("residual/eta");
    fout->mkdir("residual/tnpu");
    fout->mkdir("residual/refpt");

    fout->mkdir("rel_residual");
    fout->mkdir("rel_residual/eta");
    fout->mkdir("rel_residual/tnpu");
    fout->mkdir("rel_residual/refpt");

    fout->mkdir("rmsplots");

    vector<TH2D*> refpt_hists, tnpu_hists, refpt_residual_hists, tnpu_residual_hists, refpt_relative_residual_hists, tnpu_relative_residual_hists;

    TH2D * h_rms = new TH2D("rms","rms",prof->GetNbinsX(),prof->GetXaxis()->GetXbins()->GetArray(),prof->GetNbinsZ(),prof->GetZaxis()->GetXbins()->GetArray());
    TH2D * h_mean = new TH2D("mean","mean",prof->GetNbinsX(),prof->GetXaxis()->GetXbins()->GetArray(),prof->GetNbinsZ(),prof->GetZaxis()->GetXbins()->GetArray());
    TH1D * h_eff_entries = new TH1D("eff_entries","eff_entries",1000,0,1000);
    vector <vector<TH1D*> > v_rms;

    for (int itnpu = 1 ; itnpu <= prof->GetYaxis()->GetNbins() ; itnpu++) {
      tnpu_hists.push_back((TH2D*)new TH2D(Form("itnpu_%d",itnpu),Form("itnpu_%d;#eta;refpt;ooa",itnpu),prof->GetXaxis()->GetNbins(),prof->GetXaxis()->GetXmin(),prof->GetXaxis()->GetXmax(),prof->GetZaxis()->GetNbins(),prof->GetZaxis()->GetXmin(),prof->GetZaxis()->GetXmax()));
      tnpu_residual_hists.push_back((TH2D*)new TH2D(Form("itnpu_%d_residual",itnpu),Form("itnpu_%d_residual;#eta;refpt;ooa residual",itnpu),prof->GetNbinsX(),prof->GetXaxis()->GetXbins()->GetArray(),prof->GetNbinsZ(),prof->GetZaxis()->GetXbins()->GetArray()));
      tnpu_residual_hists.back()->GetZaxis()->SetRangeUser(-10,10);
      tnpu_relative_residual_hists.push_back((TH2D*)new TH2D(Form("itnpu_%d_relative_residual",itnpu),Form("itnpu_%d_relative_residual;#eta;refpt;ooa relative residual",itnpu),prof->GetNbinsX(),prof->GetXaxis()->GetXbins()->GetArray(),prof->GetNbinsZ(),prof->GetZaxis()->GetXbins()->GetArray()));
      tnpu_relative_residual_hists.back()->GetZaxis()->SetRangeUser(-20,20);
    }
    for (int irefpt = 1 ; irefpt <= prof->GetZaxis()->GetNbins() ; irefpt++) {
      refpt_hists.push_back((TH2D*)new TH2D(Form("irefpt_%d",irefpt),Form("irefpt_%d;#eta;tnpu;ooa",irefpt),prof->GetXaxis()->GetNbins(),prof->GetXaxis()->GetXmin(),prof->GetXaxis()->GetXmax(),prof->GetYaxis()->GetNbins(),prof->GetYaxis()->GetXmin(),prof->GetYaxis()->GetXmax()));
      refpt_residual_hists.push_back((TH2D*)new TH2D(Form("irefpt_%d_residual",irefpt),Form("irefpt_%d_residual;#eta;tnpu;ooa residual",irefpt),prof->GetXaxis()->GetNbins(),prof->GetXaxis()->GetXmin(),prof->GetXaxis()->GetXmax(),prof->GetYaxis()->GetNbins(),prof->GetYaxis()->GetXmin(),prof->GetYaxis()->GetXmax()));
      refpt_residual_hists.back()->GetZaxis()->SetRangeUser(-10,10);
      refpt_relative_residual_hists.push_back((TH2D*)new TH2D(Form("irefpt_%d_relative_residual",irefpt),Form("irefpt_%d_relative_residual;#eta;tnpu;ooa relative residual",irefpt),prof->GetXaxis()->GetNbins(),prof->GetXaxis()->GetXmin(),prof->GetXaxis()->GetXmax(),prof->GetYaxis()->GetNbins(),prof->GetYaxis()->GetXmin(),prof->GetYaxis()->GetXmax()));
      refpt_relative_residual_hists.back()->GetZaxis()->SetRangeUser(-20,20);
    }

    //loop over eta bins
    for (int iEta = 1 ; iEta <= prof->GetXaxis()->GetNbins() ; iEta++){
      cout<<"\r\tiEta: "<<iEta<<" out of "<<prof->GetXaxis()->GetNbins()<<flush;

      vector <TH1D*> pt_dummy;
      v_rms.push_back(pt_dummy);

      TH2D* eta_hist = new TH2D(Form("iEta_%d",iEta),Form("iEta_%d;tnpu;refpt;ooa",iEta),prof->GetYaxis()->GetNbins(),prof->GetYaxis()->GetXmin(),prof->GetYaxis()->GetXmax(),prof->GetZaxis()->GetNbins(),prof->GetZaxis()->GetXmin(),prof->GetZaxis()->GetXmax());
      TH2D* eta_residual_hist = new TH2D(Form("iEta_%d_residual",iEta),Form("iEta_%d_residual;tnpu;refpt;ooa residual",iEta),prof->GetYaxis()->GetNbins(),prof->GetYaxis()->GetXmin(),prof->GetYaxis()->GetXmax(),prof->GetZaxis()->GetNbins(),prof->GetZaxis()->GetXmin(),prof->GetZaxis()->GetXmax());
      TH2D* eta_relative_residual_hist = new TH2D(Form("iEta_%d_relative_residual",iEta),Form("iEta_%d_relative_residual;tnpu;refpt;ooa relative residual",iEta),prof->GetYaxis()->GetNbins(),prof->GetYaxis()->GetXmin(),prof->GetYaxis()->GetXmax(),prof->GetZaxis()->GetNbins(),prof->GetZaxis()->GetXmin(),prof->GetZaxis()->GetXmax());
      eta_residual_hist->GetZaxis()->SetRangeUser(-10,10);
      eta_relative_residual_hist->GetZaxis()->SetRangeUser(-20,20);

      vector<TGraphErrors*> rhographs, ptpugraphs;
      vector<TGraph*> rho_residual_graphs, ptpu_residual_graphs;

      for (int itnpu = 1 ; itnpu <= prof->GetYaxis()->GetNbins() ; itnpu++) {
        ptpugraphs.push_back((TGraphErrors*)new TGraphErrors());
        ptpu_residual_graphs.push_back((TGraph*)new TGraphErrors());
      }

      for (int irefpt = 1 ; irefpt <= prof->GetZaxis()->GetNbins() ; irefpt++) {
        rhographs.push_back((TGraphErrors*)new TGraphErrors());
        rho_residual_graphs.push_back((TGraph*)new TGraphErrors());
        v_rms[iEta-1].push_back(new TH1D(Form("rms_Eta_%d_refpt_%d",iEta,irefpt),Form("rms_%d_%d",iEta,irefpt),200,-100,100));
      }

      //loop over tnpu and refpt bins
      for (int itnpu = 1 ; itnpu <= prof->GetYaxis()->GetNbins() ; itnpu++){
        for (int irefpt = 1 ; irefpt <= prof->GetZaxis()->GetNbins() ; irefpt++){

          int Gbin = prof->GetBin(iEta, itnpu, irefpt);
          // avoid points with empty content or too small error
          if (prof   ->GetBinError  (iEta,itnpu,irefpt)  > 0.000001 &&
          prof   ->GetBinError  (iEta,itnpu,irefpt)  < 5.0 &&
          prof   ->GetBinEffectiveEntries(Gbin)      > 3 && //Add back for Flat2017
          // (fabs(prof->GetBinError(iEta,itnpu,irefpt)/prof->GetBinContent(iEta,itnpu,irefpt)))<0.5 &&// 0.3 ==> 0.5
          // (fabs(prof->GetBinError(iEta,itnpu,irefpt)/prof->GetBinContent(iEta,itnpu,irefpt)))>0.01 &&// 0.05 ==> 0.01
          profPt ->GetBinContent(iEta,itnpu,irefpt)  > 0 &&
          profPt ->GetBinError  (iEta,itnpu,irefpt)  > 0.1 &&
          // profPt ->GetBinError  (iEta,itnpu,irefpt)  < 10*log10(profPt ->GetBinContent(iEta,itnpu,irefpt)) &&
          profRho->GetBinContent(iEta,itnpu,irefpt)  > 0 &&
          profRho->GetBinError  (iEta,itnpu,irefpt)  > 0.1){

            h_eff_entries->Fill(prof->GetBinEffectiveEntries(Gbin));

            // get the relevant values
            double rho  = profRho->GetBinContent(iEta, itnpu, irefpt);
            double rhoe = profRho->GetBinError  (iEta, itnpu, irefpt);
            double pt   = profPt ->GetBinContent(iEta, itnpu, irefpt);
            double pte  = profPt ->GetBinError  (iEta, itnpu, irefpt);
            double ooa  = prof   ->GetBinContent(iEta, itnpu, irefpt);
            double ooae = prof   ->GetBinError  (iEta, itnpu, irefpt);

            double fitooa = fitResults[iEta-1].fit->Eval(rho,pt);

            // Store the values
            int nrho = rhographs[irefpt-1]->GetN();
            rhographs[irefpt-1]->SetPoint(nrho, rho, ooa);
            rho_residual_graphs[irefpt-1]->SetPoint(nrho, rho, fitooa);
            rhographs[irefpt-1]->SetPointError(nrho, rhoe, ooae);
            int ntnpu = ptpugraphs[itnpu-1]->GetN();
            ptpugraphs[itnpu-1]->SetPoint(ntnpu, pt, ooa);
            ptpu_residual_graphs[itnpu-1]->SetPoint(ntnpu, pt, fitooa);
            ptpugraphs[itnpu-1]->SetPointError(ntnpu, pte, ooae);

            eta_hist->SetBinContent(itnpu,irefpt,ooa);
            refpt_hists[irefpt-1]->SetBinContent(iEta,itnpu,ooa);
            tnpu_hists[itnpu-1]->SetBinContent(iEta,irefpt,ooa);

            eta_residual_hist->SetBinContent(itnpu,irefpt,(ooa-fitooa)/ooae);
            refpt_residual_hists[irefpt-1]->SetBinContent(iEta,itnpu,(ooa-fitooa)/ooae);
            tnpu_residual_hists[itnpu-1]->SetBinContent(iEta,irefpt,(ooa-fitooa)/ooae);

            eta_relative_residual_hist->SetBinContent(itnpu,irefpt,100.0*(ooa-fitooa)/pt);
            refpt_relative_residual_hists[irefpt-1]->SetBinContent(iEta,itnpu,100.0*(ooa-fitooa)/pt);
            tnpu_relative_residual_hists[itnpu-1]->SetBinContent(iEta,irefpt,100.0*(ooa-fitooa)/pt);

            v_rms[iEta-1][irefpt-1]->Fill(ooa-fitooa);

          }//if

        }//irefpt

      } //itnpu

      fout->cd("rmsplots");

      for (unsigned irefpt = 0; irefpt<v_rms[iEta-1].size(); irefpt++){
        v_rms[iEta-1][irefpt]->Write();
        h_rms->SetBinContent(iEta,irefpt,100*(v_rms[iEta-1][irefpt]->GetRMS())/(h_rms->GetYaxis()->GetBinCenter(irefpt)));
        h_mean->SetBinContent(iEta,irefpt,100*(v_rms[iEta-1][irefpt]->GetMean())/(h_rms->GetYaxis()->GetBinCenter(irefpt)));
      }

      if (eta_hist->GetEntries()){
        fout->cd("histograms/eta");
        eta_hist->SetStats(0);
        eta_hist->Write();
      }

      if (eta_residual_hist->GetEntries()){
        fout->cd("residual/eta");
        eta_residual_hist->SetStats(0);
        eta_residual_hist->Write();
      }

      if (eta_residual_hist->GetEntries()){
        fout->cd("rel_residual/eta");
        eta_relative_residual_hist->SetStats(0);
        eta_relative_residual_hist->Write();
      }

      fout->mkdir(Form("graphs/p_offOverA_EtaVsRho/Eta%d",iEta));
      fout->cd(Form("graphs/p_offOverA_EtaVsRho/Eta%d",iEta));


      for (unsigned irefpt = 0; irefpt<rhographs.size(); irefpt++){
        if (!rhographs[irefpt]->GetN()) continue;
        rhographs[irefpt]->SetNameTitle(Form("ieta_%d_irefpt_%d",iEta,irefpt+1),Form("ieta_%d_irefpt_%d;#rho;ooa",iEta,irefpt+1));
        rhographs[irefpt]->Write();
      }

      fout->mkdir(Form("graphs/p_offOverA_EtaVsJetPt/Eta%d",iEta));
      fout->cd(Form("graphs/p_offOverA_EtaVsJetPt/Eta%d",iEta));

      for (unsigned itnpu = 0; itnpu<ptpugraphs.size(); itnpu++){
        if (!ptpugraphs[itnpu]->GetN()) continue;
        ptpugraphs[itnpu]->SetNameTitle(Form("ieta_%d_itnpu_%d",iEta,itnpu+1),Form("ieta_%d_itnpu_%d;p_{T}^{pu};ooa",iEta,itnpu+1));
        ptpugraphs[itnpu]->Write();
      }

      fout->mkdir(Form("residualgraphs/p_offOverA_EtaVsRho/Eta%d",iEta));
      fout->cd(Form("residualgraphs/p_offOverA_EtaVsRho/Eta%d",iEta));

      for (unsigned irefpt = 0; irefpt<rho_residual_graphs.size(); irefpt++){
        if (!rho_residual_graphs[irefpt]->GetN()) continue;
        rho_residual_graphs[irefpt]->SetNameTitle(Form("ieta_%d_irefpt_%d_residual",iEta,irefpt+1),Form("ieta_%d_irefpt_%d_residual;#rho;ooa",iEta+1,irefpt+1));
        rho_residual_graphs[irefpt]->Write();
      }

      fout->mkdir(Form("residualgraphs/p_offOverA_EtaVsJetPt/Eta%d",iEta));
      fout->cd(Form("residualgraphs/p_offOverA_EtaVsJetPt/Eta%d",iEta));

      for (unsigned itnpu = 0; itnpu<ptpu_residual_graphs.size(); itnpu++){
        if (!ptpu_residual_graphs[itnpu]->GetN()) continue;
        ptpu_residual_graphs[itnpu]->SetNameTitle(Form("ieta_%d_itnpu_%d_residual",iEta,itnpu+1),Form("ieta_%d_itnpu_%d_residual;p_{T}^{pu};ooa",iEta,itnpu+1));
        ptpu_residual_graphs[itnpu]->Write();
      }

    }// iEta


    fout->cd("histograms/refpt");

    for (unsigned irefpt = 0; irefpt<refpt_hists.size(); irefpt++) if (refpt_hists[irefpt]->GetEntries()){
      refpt_hists[irefpt]->SetStats(0);
      refpt_hists[irefpt]->Write();
    }

    fout->cd("histograms/tnpu");

    for (unsigned itnpu = 0; itnpu<tnpu_hists.size(); itnpu++) if (tnpu_hists[itnpu]->GetEntries()){
      tnpu_hists[itnpu]->SetStats(0);
      tnpu_hists[itnpu]->Write();
    }

    fout->cd("residual/refpt");

    for (unsigned irefpt = 0; irefpt<refpt_residual_hists.size(); irefpt++) if (refpt_residual_hists[irefpt]->GetEntries()){
      refpt_residual_hists[irefpt]->SetStats(0);
      refpt_residual_hists[irefpt]->Write();
    }

    fout->cd("residual/tnpu");

    for (unsigned itnpu = 0; itnpu<tnpu_residual_hists.size(); itnpu++) if (tnpu_residual_hists[itnpu]->GetEntries()){
      tnpu_residual_hists[itnpu]->SetStats(0);
      tnpu_residual_hists[itnpu]->Write();
    }

    fout->cd("rel_residual/refpt");

    for (unsigned irefpt = 0; irefpt<refpt_residual_hists.size(); irefpt++) if (refpt_residual_hists[irefpt]->GetEntries()){
      refpt_residual_hists[irefpt]->SetStats(0);
      refpt_residual_hists[irefpt]->Write();
    }

    fout->cd("rel_residual/tnpu");

    for (unsigned itnpu = 0; itnpu<tnpu_residual_hists.size(); itnpu++) if (tnpu_residual_hists[itnpu]->GetEntries()){
      tnpu_relative_residual_hists[itnpu]->SetStats(0);
      tnpu_relative_residual_hists[itnpu]->Write();
    }

    fout->cd("/");

    h_rms->Write();
    h_mean->Write();
    h_eff_entries->Write();

    cout<<endl<<endl;
} // fitClosurePlots

//===========================================================================
// This method tries to obtain all the relevant TProfiles from the inputFilename
// It returns true if successfull
bool getInputProfiles(TString inputFilename, TProfile3D *& prof, TProfile3D *& profPt, TProfile3D *& profRho, TH3I *& hist, bool useNPU){
    // Open the input file
    TFile *fin= new TFile(inputFilename);
    if(!fin->IsOpen()) {
      cout << "ERROR jet_synchfit_xx::getInputProfiles() could not open file "
      <<inputFilename<< endl;
      return false;
    }

    // Get the histos
    if(useNPU) {
      prof    = (TProfile3D*) fin->Get("p_offOverA_etaVsNpusVsJetPt"); //offOverA(eta, rho, refpt)
      profPt  = (TProfile3D*) fin->Get("p_PtAve_etaVsNpusVsJetPt"); // pt(eta, rho, refpt)
      profRho = (TProfile3D*) fin->Get("p_RhoAve_etaVsNpusVsJetPt");// rho(eta, rho, refpt)
      hist    = (TH3I*)       fin->Get("p_Events_etaVsNpusVsJetPt");

      if (!prof || !profPt  || !profRho || !hist) {
        cout<<"ERROR jet_synchfit_xx::getInputProfiles() could not retrieve TProfile3D named "
        <<"either of  p_offOverA_etaVsNpusVsJetPt, p_PtAve_etaVsNpusVsJetPt, "
        <<" or p_RhoAve_etaVsNpusVsJetPt"<<endl;
        return false;
      }
    }
    else {
      prof    = (TProfile3D*) fin->Get("p_offOverA_etaVsTnpusVsJetPt"); //offOverA(eta, rho, refpt)
      profPt  = (TProfile3D*) fin->Get("p_PtAve_etaVsTnpusVsJetPt"); // pt(eta, rho, refpt)
      profRho = (TProfile3D*) fin->Get("p_RhoAve_etaVsTnpusVsJetPt");// rho(eta, rho, refpt)
      hist    = (TH3I*)       fin->Get("p_Events_etaVsTnpusVsJetPt");

      if (!prof || !profPt  || !profRho || !hist) {
        cout<<"ERROR jet_synchfit_xx::getInputProfiles() could not retrieve TProfile3D named "
        <<"either of  p_offOverA_etaVsTnpusVsJetPt, p_PtAve_etaVsTnpusVsJetPt, "
        <<" or p_RhoAve_etaVsTnpusVsJetPt"<<endl;
        return false;
      }
    }

    // if everything went well just return true.
    return true;

}//getInputProfiles

//===========================================================================
// This method creates a new fit function and fits it to the graph
TF2 * doGraphFitting(TGraph2DErrors * graph, bool highPU, string functionType, int iEta, const TProfile3D * prof){
  static vector<double> pari;
  static bool pari_set = false;
  static vector<pair<double,double> > pari_lim;
  TF2* f4 = 0;
  TString function;

  if(functionType=="standard") {
    if(!pari_set) {
      pari = {16,0.9,0.04};
      pari_set = true;
    }
    function = "[0]+([1]*(x))*(1+[2]*log(y))";
  }

  else if(functionType=="ak4") {
    if(!pari_set) {
//    pari = {15,1,-1,5,0,0};	//for complex
//    pari = {15,1};		//for simple
      pari = {15,1,-1};		//for semi simple
      pari_set = true;
    }
//    function = "[0]+[1]*(x-20.0)+[2]*log(y/30.0)+[3]*pow(log(y/30.0),2)+[4]*(x-20.0)*log(y/30.0)+[5]*(x-20.0)*pow(log(y/30.0),2)";	//complex parametrization
//    function = "[1]*(x-[0])*pow(y,0)";	//simple parametrization
      function = "[1]*(x-[0])*(1.+[2]*log(y/15.))";	//semi simple parametrization
  }

  else if(functionType=="ak8") {
    if(!pari_set) {
//    pari = {15,1,-1,5,0,0};
      pari = {15,1,-1};
      pari_set = true;
    }
//  function = "[0]+[1]*(x-20.0)+[2]*log(y/90.0)+[3]*pow(log(y/90.0),2)+[4]*(x-20.0)*log(y/90.0)+[5]*(x-20.0)*pow(log(y/90.0),2)"; //complex parametrization
    function = "[1]*(x-[0])*(1.+[2]*log(y/15.))";	//semi simple parametrization 
  }

  else if(functionType=="ak4_test") {
    if(!pari_set) {
      pari = {15,1,-1,5,0,0,0,0,0};
      pari_set = true;
    }
    function = "[0]+[1]*(x-20.0)+[2]*log(y/30.0)+[3]*pow(log(y/30.0),2)+[4]*(x-20.0)*log(y/30.0)+[5]*(x-20.0)*pow(log(y/30.0),2)+[6]*pow(x-20.0,2)+[7]*pow(x-20.0,2)*log(y/30.0)+[8]*pow(x-20.0,2)*pow(log(y/30.0),2)";
  }

  else if(functionType=="ak8_test") {
    if(!pari_set) {
      pari = {15,1,-1,5,0,0,0,0,0};
      pari_set = true;
    }
    function = "[0]+[1]*(x-20.0)+[2]*log(y/90.0)+[3]*pow(log(y/90.0),2)+[4]*(x-20.0)*log(y/90.0)+[5]*(x-20.0)*pow(log(y/90.0),2)+[6]*pow(x-20.0,2)+[7]*pow(x-20.0,2)*log(y/90.0)+[8]*pow(x-20.0,2)*pow(log(y/90.0),2)";
  }

  else if(functionType=="ak4_test2") {
    if(!pari_set) {
      pari = {15,1,-1,5,0};
      pari_set = true;
    }
    function = "[0]*(x-2)*(1+[1]*(x-20.0))*(1+[2]*log(y/30.0))+([3]+[4]*log(y))";
  }

  else if(functionType=="ak8_test2") {
    if(!pari_set) {
      pari = {15,1,-1,5,0};
      pari_set = true;
    }
    function = "[0]*(x-2)*(1+[1]*(x-20.0))*(1+[2]*log(y/90.0))+([3]+[4]*log(y))";
  }

  //simplistic
  //static vector<double> pari = {1.0,1.0,1.0,1.0,1.0,1.0};
  else if(functionType=="modifiedHandkerchief") {
    //Handkerchief Function (modified)
    pari = {-10.0,-1.0,-50.0,-40.0,0.0,0.0};
    //if(iEta>=13) pari = {-3.18926, -4.76974, -47.0724, -96.8174, 0.00414141, 0.00421783};
    //for(int iEta=1; iEta<=82; iEta++) {cout << iEta << ", " << 41-abs(iEta-41)+(iEta>41) << endl;}
    if(41-abs(iEta-41)+(iEta>41)==41) pari = {-76.8447, 2.43371, -529.969, -120.342, -0.003042, -0.0101412};
    if(41-abs(iEta-41)+(iEta>41)==40) pari = {-51.3274, 1.56577, -791.884, -63.0554, -0.00656194, -0.022223};
    if(41-abs(iEta-41)+(iEta>41)==39) pari = {-44.4371, 1.29876, -408.899, -96.5217, -0.00222077, -0.0080116};
    if(41-abs(iEta-41)+(iEta>41)==38) pari = {-60.3835, 1.64388, -371.99, -141.497, -0.00144365, -0.00529115};
    if(41-abs(iEta-41)+(iEta>41)==37) pari = {5.27442, -4.63182, -229.582, -252.359, 0.00117086, 0.00190071};
    if(41-abs(iEta-41)+(iEta>41)==36) pari = {-186.495, 5.34063, -1188.95, -133.991, -0.00698784, -0.0231839};
    if(41-abs(iEta-41)+(iEta>41)==35) pari = {-89.5476, 2.66965, -516.026, -152.121, -0.00230586, -0.00778348};
    if(41-abs(iEta-41)+(iEta>41)==34) pari = {-115.907, 3.41263, -733.659, -150.294, -0.0034369, -0.0113916};
    if(41-abs(iEta-41)+(iEta>41)==33) pari = {-69.606, 2.13182, -622.055, -100.156, -0.00443436, -0.0146278};
    if(41-abs(iEta-41)+(iEta>41)==32) pari = {214.451, -7.86659, -1373.74, -128.697, 0.0100042, 0.0297094};
    if(41-abs(iEta-41)+(iEta>41)==31) pari = {-117.381, 3.31343, -733.61, -166.283, -0.00283487, -0.00952053};
    if(41-abs(iEta-41)+(iEta>41)==30) pari = {-137.85, 4.13062, -767.227, -164.92, -0.00362462, -0.0118522};
    if(41-abs(iEta-41)+(iEta>41)==29) pari = {41.4364, -2.90509, -1031.35, -170.149, 0.00154982, 0.00337273};
    if(41-abs(iEta-41)+(iEta>41)==28) pari = {-160.515, 4.55097, -937.38, -176.282, -0.00414836, -0.0136325};
    if(41-abs(iEta-41)+(iEta>41)==27) pari = {-124.973, 3.64175, -765.608, -166.264, -0.00348996, -0.0114105};
    if(41-abs(iEta-41)+(iEta>41)==26) pari = {-115.341, 3.38935, -710.485, -164.869, -0.00379213, -0.012158};
    if(41-abs(iEta-41)+(iEta>41)==25) pari = {-52.841, 1.74631, -220.896, -138.787, -0.00200547, -0.00626572};
    if(41-abs(iEta-41)+(iEta>41)==24) pari = {-109.378, 3.67646, -582.373, -155.86, -0.00462153, -0.0140837};
    if(41-abs(iEta-41)+(iEta>41)==23) pari = {-102.919, 3.88843, -487.29, -146.096, -0.00515363, -0.0151392};
    if(41-abs(iEta-41)+(iEta>41)==22) pari = {-129.424, 5.50359, -475.85, -139.099, -0.00747056, -0.021239};
    if(41-abs(iEta-41)+(iEta>41)==21) pari = {84.0723, -3.92998, -670.284, -45.9705, 0.00259347, 0.00480303};
    if(41-abs(iEta-41)+(iEta>41)==20) pari = {1.29218, -0.339506, 5.34299, -0.0245294, -0.00667469, -0.0138323};
    if(41-abs(iEta-41)+(iEta>41)==19) pari = {0.640088, 0.140856, -1.46036, -1.58256, -0.0167095, -0.0259715};
    if(41-abs(iEta-41)+(iEta>41)==18) pari = {22.3054, -1.83057, -181.977, -3.36615, -0.00531555, -0.0194336};
    if(41-abs(iEta-41)+(iEta>41)==17) pari = {59.6557, -2.6691, -630.216, -2.06518, 0.00313721, 0.00144351};
    if(41-abs(iEta-41)+(iEta>41)==16) pari = {1.02797, 0.00323455, 4.87806, 3.25137, -0.00244485, -0.0143917};
    if(41-abs(iEta-41)+(iEta>41)==15) pari = {-20.3957, 0.901549, -656.728, -7.57845, 0.00633164, 0.00913796};
    if(41-abs(iEta-41)+(iEta>41)==14) pari = {-3.2931, -4.7954, -35.0192, -96.693, 0.00409229, 0.00423637};
    if(41-abs(iEta-41)+(iEta>41)==13) pari = {3.0007, -4.9422, -58.2853, -91.3892, 0.00509591, 0.00612996};
    if(41-abs(iEta-41)+(iEta>41)==12) pari = {-2.76875, -2.13562, -370.079, -97.4652, 0.00404468, 0.00636189};
    if(41-abs(iEta-41)+(iEta>41)==11) pari = {-12.975, -1.0913, -1823.74, -104.751, 0.00369794, 0.007533};
    if(41-abs(iEta-41)+(iEta>41)==10) pari = {-48.4921, -2.92375, -385.236, -185.967, 0.00209605, 0.0025999};
    if(41-abs(iEta-41)+(iEta>41)==9)  pari = {-36.7745, -4.33193, -172.6, -164.971, 0.00270432, 0.00296645};
    if(41-abs(iEta-41)+(iEta>41)==8)  pari = {-53.8115, -0.0177498, -1133.33, -106.878, 0.00487379, 0.0102605};
    if(41-abs(iEta-41)+(iEta>41)==7)  pari = {-37.0815, -5.62769, -103.859, -174.628, 0.00250659, 0.00236942};
    if(41-abs(iEta-41)+(iEta>41)==6)  pari = {-14.5656, -1.79994, -440.529, -104.604, 0.0050598, 0.00911403};
    if(41-abs(iEta-41)+(iEta>41)==5)  pari = {-3.06766, -4.74059, -81.701, -96.9543, 0.0055314, 0.00677566};
    if(41-abs(iEta-41)+(iEta>41)==4)  pari = {-39.1683, -5.23999, -118.088, -226.798, 0.00148175, 0.00142602};
    if(41-abs(iEta-41)+(iEta>41)==3)  pari = {-3.18926, -4.76974, -47.0724, -96.8174, 0.00414141, 0.00421783};
    if(41-abs(iEta-41)+(iEta>41)==2)  pari = {-10.9249, -2.33964, -119.69, -115.437, 0.00285224, 0.00359828};
    if(41-abs(iEta-41)+(iEta>41)==1)  pari = {-3.18926, -4.76974, -47.0724, -96.8174, 0.00414141, 0.00421783};

    pari_lim = {make_pair(-100,100),make_pair(-100,100),make_pair(-10000,10000),
      make_pair(-1000,1000),make_pair(-1,1),make_pair(-1,1)};

      function = "[0]+([1]*pow(log10(y-[2]),3))+([4]*log10(y-[2])*pow(x-[3],2))+([5]*(pow(log10(y-[2]),2)-pow(x-[3],2)))";
      //x(u,v)=u y(u,v)=v z(u,v)=1/3u^3+uv^2+2(u^2-v^2)// Handkerchief Surface http://mathworld.wolfram.com/HandkerchiefSurface.html
   }
   else if(functionType=="modifiedHandkerchief+rho") {
      if(41-abs(iEta-41)+(iEta>41)==41) pari = {-76.8447, 2.43371, -529.969, -120.342, -0.003042, -0.0101412, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==40) pari = {-51.3274, 1.56577, -791.884, -63.0554, -0.00656194, -0.022223, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==39) pari = {-44.4371, 1.29876, -408.899, -96.5217, -0.00222077, -0.0080116, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==38) pari = {-60.3835, 1.64388, -371.99, -141.497, -0.00144365, -0.00529115, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==37) pari = {5.27442, -4.63182, -229.582, -252.359, 0.00117086, 0.00190071, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==36) pari = {-186.495, 5.34063, -1188.95, -133.991, -0.00698784, -0.0231839, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==35) pari = {-89.5476, 2.66965, -516.026, -152.121, -0.00230586, -0.00778348, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==34) pari = {-115.907, 3.41263, -733.659, -150.294, -0.0034369, -0.0113916, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==33) pari = {-69.606, 2.13182, -622.055, -100.156, -0.00443436, -0.0146278, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==32) pari = {214.451, -7.86659, -1373.74, -128.697, 0.0100042, 0.0297094, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==31) pari = {-117.381, 3.31343, -733.61, -166.283, -0.00283487, -0.00952053, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==30) pari = {-137.85, 4.13062, -767.227, -164.92, -0.00362462, -0.0118522, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==29) pari = {41.4364, -2.90509, -1031.35, -170.149, 0.00154982, 0.00337273, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==28) pari = {-160.515, 4.55097, -937.38, -176.282, -0.00414836, -0.0136325, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==27) pari = {-124.973, 3.64175, -765.608, -166.264, -0.00348996, -0.0114105, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==26) pari = {-115.341, 3.38935, -710.485, -164.869, -0.00379213, -0.012158, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==25) pari = {-52.841, 1.74631, -220.896, -138.787, -0.00200547, -0.00626572, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==24) pari = {-109.378, 3.67646, -582.373, -155.86, -0.00462153, -0.0140837, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==23) pari = {-102.919, 3.88843, -487.29, -146.096, -0.00515363, -0.0151392, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==22) pari = {-129.424, 5.50359, -475.85, -139.099, -0.00747056, -0.021239, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==21) pari = {84.0723, -3.92998, -670.284, -45.9705, 0.00259347, 0.00480303, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==20) pari = {1.29218, -0.339506, 5.34299, -0.0245294, -0.00667469, -0.0138323, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==19) pari = {0.640088, 0.140856, -1.46036, -1.58256, -0.0167095, -0.0259715, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==18) pari = {22.3054, -1.83057, -181.977, -3.36615, -0.00531555, -0.0194336, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==17) pari = {59.6557, -2.6691, -630.216, -2.06518, 0.00313721, 0.00144351, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==16) pari = {1.02797, 0.00323455, 4.87806, 3.25137, -0.00244485, -0.0143917, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==15) pari = {-20.3957, 0.901549, -656.728, -7.57845, 0.00633164, 0.00913796, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==14) pari = {-3.2931, -4.7954, -35.0192, -96.693, 0.00409229, 0.00423637, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==13) pari = {3.0007, -4.9422, -58.2853, -91.3892, 0.00509591, 0.00612996, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==12) pari = {-2.76875, -2.13562, -370.079, -97.4652, 0.00404468, 0.00636189, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==11) pari = {-12.975, -1.0913, -1823.74, -104.751, 0.00369794, 0.007533, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==10) pari = {-48.4921, -2.92375, -385.236, -185.967, 0.00209605, 0.0025999, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==9)  pari = {-36.7745, -4.33193, -172.6, -164.971, 0.00270432, 0.00296645, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==8)  pari = {-53.8115, -0.0177498, -1133.33, -106.878, 0.00487379, 0.0102605, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==7)  pari = {-37.0815, -5.62769, -103.859, -174.628, 0.00250659, 0.00236942, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==6)  pari = {-14.5656, -1.79994, -440.529, -104.604, 0.0050598, 0.00911403, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==5)  pari = {-3.06766, -4.74059, -81.701, -96.9543, 0.0055314, 0.00677566, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==4)  pari = {-39.1683, -5.23999, -118.088, -226.798, 0.00148175, 0.00142602, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==3)  pari = {-3.18926, -4.76974, -47.0724, -96.8174, 0.00414141, 0.00421783, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==2)  pari = {-10.9249, -2.33964, -119.69, -115.437, 0.00285224, 0.00359828, 0.5};
      if(41-abs(iEta-41)+(iEta>41)==1)  pari = {-3.18926, -4.76974, -47.0724, -96.8174, 0.00414141, 0.00421783, 0.5};

      pari_lim = {make_pair(-1000,1000),make_pair(-100,100),make_pair(-10000,10000),
        make_pair(-1000,1000),make_pair(-1,1),make_pair(-1,1),make_pair(-10,10)};

        function = "[0]+([6]*x)+([1]*pow(log10(y-[2]),3))+([4]*log10(y-[2])*pow(x-[3],2))+([5]*(pow(log10(y-[2]),2)-pow(x-[3],2)))";
      }
      else if(functionType=="modifiedMonkeySaddle") {
        //modified Monkey Saddle
        //static vector<double> pari = {5.0,-0.05,-10.0,3.0,3.0,-10.0,2.0};
        //static vector<double> pari = {5.0,-10.0,-0.35,0.0,50.0,-4.0,150.0};
        if(!pari_set) {
          pari = {0.0,0.33,1500.0,35.0,1.0,2.0,};
          pari_set = true;
        }
      }
      else if(functionType=="puppi") {
        if(!pari_set) {
          pari = {18,-1.5,-0.35,0.0,1.0};
          //pari = {-0.5,0.5,0.1,0.0,1.0};
          pari_set = true;
        }
        function = "[0]+([1]*x)*(1+[2]*log(y))+(1/([3]*log(y)+[4]))";
      }
      else if(functionType=="other") {
        //Modified Monkey Saddle Function
        //function = "[0]+([1]*pow(x,3))-[2]*x*pow(log(y-[3]),2)";
        //function = "[0]+([1]*pow(-x-[2],[3]))+([4]*x*pow(y-[5],[6]))";
        //KPedro
        //function = "[0]+([1]*pow(-x-[2],3)*(y-[3]))-([4]*pow(y-[5],3)*(-x-[6]))";
        //KPedro2
        //function = "[0]+([1]*x*y)*TMath::Exp(-([2]*pow(x-[3],2)+[4]*pow(y-[5],2)))";
        //z=x(x^2-3y^2) // monkey saddle http://mathworld.wolfram.com/MonkeySaddle.html
        //x^3*y-y^3*x //Pedro
        //x*y*exp(-x^3*y-x*y^3) my creation
        //x*exp(-x^2-y^2) //matlab example
        //-(x*y)*Exp[-(x^2 + y^2)] //pedro
      }
      else if(functionType=="standard+taylorExpansion") {
        if(!pari_set) {
          pari = {-0.5,0.5,0.1};
          pari_set = true;
        }
        //Taylor expanded version
        function = "[0]+([1]*(x-11))*(1+[2]*(log(y)-1.47))";
      }

   cout << "\tInitial Parameters: (" << flush;
   for(unsigned int ipar=0; ipar<pari.size(); ipar++) {
      cout << pari[ipar];
      if(ipar!=pari.size()-1) cout << ", ";
      else cout << ")" << endl;
   }

   if(highPU)
      f4 = new TF2(Form("f_offOverA_RhoVsJetPt_%d",iEta),function, 0,200,1,3000);
      //f4 = new TF2("f4",function, 5,200,10,3000);
   else
      f4 = new TF2(Form("f_offOverA_RhoVsJetPt_%d",iEta),function, 0,70,1,3000);
      //f4 = new TF2("f4",function, 5,50,10,3000);

      if (functionType=="ak4") {
//      f4->SetRange(0,30,70,100);	//for simple parametrization, fits for pT:30-100GeV
	f4->SetRange(0,8,70,6500);	//for complex and semi simple parametrization, fits for pT:8-6500GeV
      }

      if (functionType=="ak8") {
        f4->SetRange(0,8,70,6500);
      }

      if(functionType=="puppi") {
        for(unsigned int ipar=0; ipar<pari.size(); ipar++) {
          f4->SetParameter(ipar,pari[ipar]);
        }
        f4->SetParLimits(0,-50,50);
        f4->SetParLimits(1,0,10);
        f4->SetParLimits(2,-2,5);
      }
      else if(functionType=="modifiedHandkerchief" || functionType=="modifiedHandkerchief+rho") {
        for(unsigned int ipar=0; ipar<pari.size(); ipar++) {
          f4->SetParameter(ipar,pari[ipar]);
          f4->SetParLimits(ipar,pari_lim[ipar].first,pari_lim[ipar].second);
        }
      }

      else {
        for(unsigned int ipar=0; ipar<pari.size(); ipar++) {
          f4->SetParameter(ipar,pari[ipar]);
        }
      }

    int nfits = 10;
    int counter=0;
    double rchi2;
    do{
//      graph->Fit(f4,"0QMER");
      graph->Fit(f4,"MER");		
      rchi2 = f4->GetChisquare()/ f4->GetNDF();
      counter++;
      if ((rchi2>9 || rchi2==0) && counter==nfits-2) {
         //Reset initial parameters
         cout << "\tResetting initial parameters" << endl;
         for(unsigned int ipar=0; ipar<pari.size(); ipar++) {
            f4->SetParameter(ipar,pari[ipar]);
        }
      }
   } while ((rchi2>9 || rchi2==0.0) && counter < nfits);
   cout <<"N. of fits: " << counter << endl;
    
   cout << "\tFinal Parameters: (";
   for(unsigned int ipar=0; ipar<pari.size(); ipar++) {
      cout << f4->GetParameter(ipar) << "\u00b1" << f4->GetParError(ipar);
      if(ipar!=pari.size()-1) cout << ", ";
      else cout << ")" << endl;
    }
    cout << "\tChi2 = " << f4->GetChisquare() << endl;
    cout << "\tNDF = " << f4->GetNDF() << endl;
    cout << "\tChi2/NDF = " << rchi2 << endl;
    cout << "\tProb = " << f4->GetProb() << endl;

    // sets initial parameters to previous results
    for(unsigned int ipar=0; ipar<pari.size(); ipar++) {
      pari[ipar] = f4->GetParameter(ipar);
    }

    return f4;

  }//doGraphFitting


  //===========================================================================
  // This method creates the txt file for the corrections
  void createTxtFile(TString txtFilename, const vector<FitRes> & fitResults){

    if(fitResults.size() == 0)   // safety protection
       return;

    // Create the stream
    ofstream outF(txtFilename.Data());

    // Produce the first line
    TString fname = Form("{1 JetEta 3 Rho JetPt JetA max(0.0001,1-(z/y)*(%s))",fitResults[0].fit->GetTitle());
    outF << fname <<" Correction L1FastJet}"<<endl;

    // loop over the vector producing the eta lines
    for (unsigned int l=0; l<fitResults.size() ; l++){

      // for each fit print this header ...
      outF<<std::setw(8)<<fitResults[l].etalowedge
      <<std::setw(8)<<fitResults[l].etaupedge
      <<std::setw(8)<<(int)(fitResults[l].fit->GetNpar()+6)
      <<std::setw(8)<<fitResults[l].fit->GetXmin()<<std::setw(12)<<fitResults[l].fit->GetXmax()
      <<std::setw(8)<<fitResults[l].fit->GetYmin()<<std::setw(12)<<fitResults[l].fit->GetYmax()
      <<std::setw(8)<<0<<std::setw(12)<<10;

      // ... followed by the parameters
      for(int p=0; p<fitResults[l].fit->GetNpar(); p++) {
        outF<<std::setw(17)<<std::setprecision(10)<<fitResults[l].fit->GetParameter(p);
      }
      outF<<std::endl;
    }//for fit results

    // Close the stream
    outF.close();
}//createTxtFile

//===========================================================================
void createPDFFile(TFile* fout, TString pdfFilename, const vector<FitRes> & fitResults){

  if(fitResults.size() == 0)   // safety protection
     return;

  // The number of parameters in the fit.
  // Assume all fitResults element have the same number
  unsigned int nfitpars = fitResults[0].fit->GetNpar();

  // Create the vectors veta from the histo, no hardcoding here
  double veta[fitResults.size()+1];
  for (unsigned int l=0;l<fitResults.size();l++){
    veta[l] = fitResults[l].etalowedge;
  }
  veta[fitResults.size()] = fitResults[fitResults.size()-1].etaupedge;

  // The vector holding all histos
  vector<TH1 *> histos;
  TString cname = "OffsetOverAreaVsRhoVsPt_";
  for (unsigned int h = 0 ; h < nfitpars ; h++){
    TString hname = cname + Form("_Par%i",h);
    histos.push_back(new TH1D(hname,hname+ Form(";#eta;Par%i;",h),
                              fitResults.size() ,veta));
  }
  // add the chi2/ndf line
  histos.push_back(new TH1D(cname+"_CHI2NDF",cname+"_CHI2NDF"+
                            ";#eta;#chi^{2}/ndf;", fitResults.size() ,veta));

  // loop over the fits
  for (unsigned int l=0; l<fitResults.size() ; l++){

    // for each line fill the histos
    for (unsigned int p=0 ; p < nfitpars  ; p++){
      histos[p]->SetBinContent(l+1, fitResults[l].fit->GetParameter(p));
      histos[p]->SetBinError  (l+1, fitResults[l].fit->GetParError(p));
    }

    // add the red chi2 info
    double redchi2 =  fitResults[l].fit->GetChisquare()/ fitResults[l].fit->GetNDF();
    histos[nfitpars]->SetBinContent(l+1, redchi2);
    histos[nfitpars]->SetBinError(l+1,2/sqrt(fitResults[l].fit->GetNDF()));
    // for large NDF's the error on the mean of the redchi2 is 2/sqrt(n) ??, not sure...

  }// loop over fits

  // Make a directory to save the histograms
  fout->mkdir("parameters");
  fout->cd("parameters");

  // Create the canvas
  TCanvas *cPar = new TCanvas("Par_","Par_",1000,1000);
  cPar->cd();
  int nx=(int)std::sqrt((float)nfitpars+1);
  int ny=nx;
  if (nx*ny<(int)nfitpars+1) nx++;
  if (nx*ny<(int)nfitpars+1) ny++;
  cPar->Divide(nx,ny,1e-04,1e-04);

  // fill the pads
  for (int unsigned h = 0 ; h < histos.size() ; h++){
    cPar->cd(h+1);
    histos[h]->Draw("E");
    histos[h]->Write();
  }

  // Save the Canvas as pdf
  cPar->SaveAs(pdfFilename);

  fout->cd();
  cPar->Write();
}// createPDFFile(pdfFilename, fitResults);

//===========================================================================
int main(int argc,char**argv){

  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;
  string         inputDir     = cl.getValue<string> ("inputDir",           "./");
  string         outputDir    = cl.getValue<string> ("outputDir",          "./");
  string         aalgo1       = cl.getValue<string> ("algo1",           "ak5pf");
  string         aalgo2       = cl.getValue<string> ("algo2",           "ak5pf");
  bool           highPU       = cl.getValue<bool>   ("highPU",            false);
  bool           useNPU       = cl.getValue<bool>   ("useNPU",            false);
  string         functionType = cl.getValue<string> ("functionType", "standard");
  string         era          = cl.getValue<string> ("era",             "<era>");

  if (!cl.check()) return 0;
  cl.print();

  if(inputDir.empty())      inputDir  = string(gSystem->pwd())+"/";
  if(outputDir.empty())     outputDir = string(gSystem->pwd())+"/";
  if(inputDir.back()!='/')  inputDir +="/";
  if(outputDir.back()!='/') outputDir+="/";

  TString algo1(aalgo1);
  TString algo2(aalgo2);

  setREStyle();

  TString algo12 = algo1+"_"+algo2;
  if (algo1.EqualTo(algo2))
  algo12 = algo1;

  // Open the input file and retrieve all relevant TProfile3D's
  TString inputFilename = inputDir+"output_"+algo12+".root";
  TProfile3D *prof=0, *profPt=0, *profRho=0;
  TH3I *hist=0;
  if (!getInputProfiles(inputFilename, prof, profPt, profRho, hist, useNPU))
  return 1;

  // Create the output file to store the graphs
  TString rootFilename = outputDir+"Parameter_"+algo12+".root";
  TFile *fout = new TFile (rootFilename,"RECREATE");
  fout->mkdir("p_offOverA_EtaVsRhoVsJetPt");
  fout->cd("p_offOverA_EtaVsRhoVsJetPt");

  // The vector to save the results of all fits
  vector<FitRes> fitResults;

  double etabin[82], Chi2NDF[82], Prob[82], Chi2[82], NDF[82];


  // Loop over all etas
  for (int iEta = 1; iEta <= prof->GetXaxis()->GetNbins(); iEta++){
    //for (int iEta = 13; iEta <= 44; iEta++){

    // Report to Screen
    cout<< "Analyzing ieta="<<iEta<<" eta="<<prof->GetXaxis()->GetBinCenter(iEta) << endl
    << "****************************" << endl;

    // Create the graph
    TGraph2DErrors *graph  = getGraph2D(iEta, prof, profPt, profRho, hist);
    cout << "Graph for pT, Eta, Rho created successfully" << endl;

    // Needs to be at least 4 entries
    if(graph->GetN()<4){
      cout<<"\t WARNING: Graph has only " << graph->GetN() << " entries. Skipping eta" << endl;
      continue;
    }

    // Do the fitting
    TF2 * fitfunc = doGraphFitting(graph, highPU, functionType, iEta, prof);
    cout << "Fitted function" << endl << endl;

    // Put this fit result in the vector fitResults
    FitRes fitres;
    fitres.etalowedge = prof->GetXaxis()->GetBinLowEdge(iEta);
    fitres.etaupedge  = prof->GetXaxis()->GetBinUpEdge(iEta);
    fitres.fit        = fitfunc;
    fitResults.push_back(fitres);

    // Save the graph to file
    graph->Write();
    fitfunc->Write();

     //===== for drawing graph Chi2/NDF vs Eta =====
            etabin[iEta-1] = (fitres.etalowedge+fitres.etaupedge)/2;
//	    cout<<"eta="<<(fitres.etalowedge+fitres.etaupedge)/2<<endl;
//	    cout<<"eta="<<prof->GetXaxis()->GetBinCenter(iEta)<<endl;
	    if(fitfunc->GetChisquare()/fitfunc->GetNDF()>1000) 
	    {
		Chi2NDF[iEta-1] =0; 
		cout<<"eta = "<<iEta-1<<"has chi2/NDF = "<<fitfunc->GetChisquare()/fitfunc->GetNDF()<<endl;
	    }
	    else 
	    {
		Chi2NDF[iEta-1] = fitfunc->GetChisquare()/fitfunc->GetNDF(); 	
	    }
	    NDF[iEta-1] = fitfunc->GetNDF();
	    Chi2[iEta-1] = fitfunc->GetChisquare();
	    Prob[iEta-1] = fitfunc->GetProb();
  	

  }// eta bins


  //===== Draw graph Chi2/NDF vs Eta =====
    TGraph *g_chi2ndf_vs_eta = new TGraph(82, etabin, Chi2NDF);
    TCanvas *pad1 = new TCanvas("pad1", "",1);
    g_chi2ndf_vs_eta->GetYaxis()->SetTitle("Chi2/ndf");
    g_chi2ndf_vs_eta->GetXaxis()->SetTitle("eta");
    g_chi2ndf_vs_eta->SetMarkerStyle(20);
    g_chi2ndf_vs_eta->Draw("AP");
    pad1->SaveAs(Form("%s/Chi2NDF_vs_eta.png",outputDir.data()));

    TGraph *g_prob_vs_eta = new TGraph(82, etabin, Prob);
    TCanvas *pad2 = new TCanvas("pad2", "",1);
    g_prob_vs_eta->GetYaxis()->SetTitle("Prob.");
    g_prob_vs_eta->GetXaxis()->SetTitle("eta");
    g_prob_vs_eta->SetMarkerStyle(20);
//  g_prob_vs_eta->GetYaxis()->SetRangeUser(0.,0.01);
    g_prob_vs_eta->Draw("AP");
    pad2->SaveAs(Form("%s/Prob_vs_eta.png",outputDir.data()));

    TGraph *g_chi2_vs_eta = new TGraph(82, etabin, Chi2);
    TCanvas *pad3 = new TCanvas("pad3", "",1);
    g_chi2_vs_eta->GetYaxis()->SetTitle("Chi2");
    g_chi2_vs_eta->GetXaxis()->SetTitle("eta");
    g_chi2_vs_eta->SetMarkerStyle(20);
    g_chi2_vs_eta->Draw("AP");
    pad3->SaveAs(Form("%s/Chi2_vs_eta.png",outputDir.data()));

    TGraph *g_ndf_vs_eta = new TGraph(82, etabin, NDF);
    TCanvas *pad4 = new TCanvas("pad4", "",1);
    g_ndf_vs_eta->GetYaxis()->SetTitle("NDF");
    g_ndf_vs_eta->GetXaxis()->SetTitle("eta");
    g_ndf_vs_eta->SetMarkerStyle(20);
    g_ndf_vs_eta->Draw("AP");
    pad4->SaveAs(Form("%s/NDF_vs_eta.png",outputDir.data()));


  fitClosurePlots(prof, profPt, profRho, fitResults, hist, fout);

  // Create the txt file from the fitResults vector
  TString txtFilename = outputDir+era+"_L1FastJet_"+((algo1.EqualTo(algo2)) ? JetInfo(algo12).alias : algo12)+".txt";
  createTxtFile(txtFilename, fitResults);

  // Create the canvas with all parameters vs eta.
  TString pdfFilename = outputDir+"Parameter_"+algo12+".pdf";
  createPDFFile(fout, pdfFilename, fitResults);

  // close the file and clean up
  fout->Close();
  delete fout;

  return 0;

} // jet_synchfit_x

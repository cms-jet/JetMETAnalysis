///////////////////////////////////////////////////////////////////
//
// jet_draw_closure_pt_x
// ---------------------
//
//            12/09/2011 Alexx Perloff  <aperloff@physics.tamu.edu>
///////////////////////////////////////////////////////////////////

#include "JetMETAnalysis/JetUtilities/interface/Style.h"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/JetInfo.hh"

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
#include "TString.h"
#include "TPaveText.h"
#include "TLatex.h"
#include "TLegend.h"

#include <fstream>
#include <string>
#include <stdio.h>
#include <stdarg.h>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// define local functions
////////////////////////////////////////////////////////////////////////////////


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
  
  vector<TString> algs         = cl.getVector<TString> ("algs");
  TString         flavor       = cl.getValue<TString>  ("flavor",           "");
  TString         path         = cl.getValue<TString>  ("path",             "");
  bool            mpv          = cl.getValue<bool>     ("mpv",            true);
  TString         outputDir    = cl.getValue<TString>  ("outputDir",  "images");
  vector<TString> outputFormat = cl.getVector<TString> ("outputFormat", ".png:::.eps:::.pdf");
  bool            tdr          = cl.getValue<bool>     ("tdr",           false);

  if (!cl.check()) return 0;
  cl.print();

  if (tdr) {
     setTDRStyle();
  }

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
      // book histograms and functions
      //
      char name[1024];
      TH2F *RespVsPt[4];
      RespVsPt[0] = (TH2F*)inf->Get("RespVsPt_Bar");
      RespVsPt[1] = (TH2F*)inf->Get("RespVsPt_IEnd");
      RespVsPt[2] = (TH2F*)inf->Get("RespVsPt_OEnd");
      RespVsPt[3] = (TH2F*)inf->Get("RespVsPt_Fwd");
  
      TF1 *func[4][NPtBins];
      TH1D *h[4][NPtBins];
      TH1F *hClosure[4];
      for(int ieta=0, ih=0; ieta<NETA_Coarse; ieta++) {
        if(veta_coarse[ieta]<0) continue;
        else {
          TString hname = Form("ClosureVsRefPt_JetEta%sto%s",eta_boundaries_coarse[ieta],eta_boundaries_coarse[ieta+1]);
          hClosure[ih] = new TH1F(hname,hname,NPtBins,vpt);
          ih++;
        }
      }
      //hClosure[0] = new TH1F("ClosureVsPt_Bar","ClosureVsPt_Bar",NPtBins,vpt);
      //hClosure[1] = new TH1F("ClosureVsPt_IEnd","ClosureVsPt_IEnd",NPtBins,vpt);
      //hClosure[2] = new TH1F("ClosureVsPt_OEnd","ClosureVsPt_OEnd",NPtBins,vpt);
      //hClosure[3] = new TH1F("ClosureVsPt_Fwd","ClosureVsPt_Fwd",NPtBins,vpt);
      double XminCalo[4] = {15,15,15,15};
      double XminPF[4] = {5,5,5,5};
      double Xmax[4] = {4000,4000,2000,190};

      //
      // Find the mean peak of the gaussian fit or the mean of the histogram and 
      // fill the final histograms
      //
      for(int i=0;i<NPtBins;i++) { 
        for(int j=0;j<4;j++) {
          sprintf(name,"CorResponse_%d_RefPt%sto%s",j,Pt[i],Pt[i+1]);
          h[j][i] = RespVsPt[j]->ProjectionY(name,i+1,i+1);
          sprintf(name,"FitResponse_%d_RefPt%sto%s",j,Pt[i],Pt[i+1]);
          func[j][i] = new TF1(name,"gaus",h[j][i]->GetMean()-1.5*h[j][i]->GetRMS(),
                               h[j][i]->GetMean()+1.5*h[j][i]->GetRMS());
          func[j][i]->SetLineColor(4);
          func[j][i]->SetLineWidth(2);
          func[j][i]->SetParNames("N","#mu","#sigma");

          if (h[j][i]->GetEntries()>5)
          {
            h[j][i]->Fit(func[j][i],"RQ");
            if(mpv)
            {
              hClosure[j]->SetBinContent(i+1,func[j][i]->GetParameter(1));
              hClosure[j]->SetBinError(i+1,func[j][i]->GetParError(1));
            }
            else
            {
              hClosure[j]->SetBinContent(i+1,h[j][i]->GetMean());
              hClosure[j]->SetBinError(i+1,h[j][i]->GetMeanError());
            }
          }
          else
          {
            hClosure[j]->SetBinContent(i+1,h[j][i]->GetMean());
            hClosure[j]->SetBinError(i+1,h[j][i]->GetMeanError());
          }  
          sprintf(name,"can_%d_RefPt%sto%s",j,Pt[i],Pt[i+1]);
          TString ss(name);
          ss+="_"+algs[a];
          /*
          if (tdr && j==0 && i>=NPtBins-6) {
           hClosure[j]->SetBinContent(i+1,0.0);
           hClosure[j]->SetBinError(i+1,0.0);
         }
         else if(tdr && hClosure[j]->GetBinError(i+1)>0.035) {
           hClosure[j]->SetBinContent(i+1,0.0);
           hClosure[j]->SetBinError(i+1,0.0);
         }
         else if(tdr && j==2 && i>=NPtBins-11) {
           hClosure[j]->SetBinContent(i+1,0.0);
           hClosure[j]->SetBinError(i+1,0.0);
         }
          */
       }
     }

      //
      // Create guides (lines) for the output histograms
      //
      TF1 *line = new TF1("line","0*x+1",0,5000);
      line->SetLineColor(1);
      line->SetLineWidth(1);
      line->SetLineStyle(2);
      TF1 *linePlus1 = new TF1("linePlus","0*x+1.01",0,5000);
      linePlus1->SetLineColor(1);
      linePlus1->SetLineWidth(1);
      linePlus1->SetLineStyle(3);
      TF1 *lineMinus1 = new TF1("lineMinus","0*x+0.99",0,5000);
      lineMinus1->SetLineColor(1);
      lineMinus1->SetLineWidth(1);
      lineMinus1->SetLineStyle(3);
      TF1 *linePlus2 = new TF1("linePlus2","0*x+1.02",0,5000);
      linePlus2->SetLineColor(1);
      linePlus2->SetLineWidth(1);
      linePlus2->SetLineStyle(4);
      TF1 *lineMinus2 = new TF1("lineMinus2","0*x+0.98",0,5000);
      lineMinus2->SetLineColor(1);
      lineMinus2->SetLineWidth(1);
      lineMinus2->SetLineStyle(4);

      TCanvas *can[4];
      //TString Text[4] = {"|#eta| < 1.3","1.3 < |#eta| < 3","3 < |#eta| < 5"};
      TString Text[4] = {"|#eta| < 1.3","1.3 < |#eta| < 2.5","2.5 < |#eta| < 3.0","3.0 < |#eta| < 5.0"};
      TPaveText *pave[4];

      //
      // Open/create the output directory and file
      //
      if(!outputDir.EndsWith("/")) outputDir+="/";
      if(!gSystem->OpenDirectory(outputDir)) gSystem->mkdir(outputDir);
      TString ofname = outputDir+"ClosureVsRefPt_"+algs[a]+".root";
      if(!flavor.IsNull()) ofname = outputDir+"ClosureVsRefPt_"+algs[a]+"_"+flavor+".root";
      TFile* outf = new TFile(ofname,"RECREATE");

      //
      // Format and save the output
      //
      for(int j=0;j<4;j++)
        {
          pave[j] = new TPaveText(0.3,0.75,0.8,0.9,"NDC");
          if (tdr) {
             pave[j]->AddText("QCD Monte Carlo");
             pave[j]->AddText(JetInfo::get_legend_title(string(algs[a])).c_str());
          }
          else {
             pave[j]->AddText(algs[a]);
          }
          pave[j]->AddText(Text[j]);
          sprintf(name,"ClosureVsRefPt_%d",j);
          TString ss(name);
          if(!flavor.IsNull()) ss+="_"+algs[a]+"_"+flavor;
          else ss+="_"+algs[a];
          can[j] = new TCanvas(ss,ss,800,800);
          if (j<3)
            gPad->SetLogx();
          if (ss.Contains("pf"))	
            hClosure[j]->GetXaxis()->SetRangeUser(XminPF[j],Xmax[j]);
          else
            hClosure[j]->GetXaxis()->SetRangeUser(XminCalo[j],Xmax[j]);
          hClosure[j]->GetXaxis()->SetTitle("p_{T}^{GEN} [GeV]"); 
          hClosure[j]->GetYaxis()->SetTitle("Corrected Response");
          if (tdr) {
             hClosure[j]->GetXaxis()->SetTitleSize(0.058);
             hClosure[j]->GetXaxis()->SetTitleOffset(0.95);
             hClosure[j]->SetMarkerStyle(20);
             //hClosure[j]->SetMarkerSize(0.5);
          }
          else {
             hClosure[j]->GetXaxis()->SetLabelSize(0.04);
             hClosure[j]->GetYaxis()->SetLabelSize(0.04);             
             //hClosure[j]->SetMarkerSize(2.0);
             hClosure[j]->GetXaxis()->SetNoExponent();
             hClosure[j]->GetXaxis()->SetMoreLogLabels();
          }
          hClosure[j]->SetMarkerColor(kBlue);
          hClosure[j]->SetLineColor(kBlue);
          hClosure[j]->SetMaximum(1.1);
          hClosure[j]->SetMinimum(0.9);
          hClosure[j]->Draw();
          line->Draw("same");
          linePlus1->Draw("same");
          lineMinus1->Draw("same");
          linePlus2->Draw("same");
          lineMinus2->Draw("same");
          pave[j]->SetFillColor(0);
          pave[j]->SetBorderSize(0);
          pave[j]->SetTextFont(42);
          pave[j]->SetTextSize(0.05);
          pave[j]->Draw("EP");
          if (tdr) cmsPrel(13,0);
          for(unsigned int iformat=0; iformat<outputFormat.size(); iformat++) {
            can[j]->SaveAs(outputDir+ss+outputFormat[iformat]);
          }
          hClosure[j]->Write();
          can[j]->Write();
        }//for(int j=0;j<3;j++)

      //
      // create overview canvas
      //
      TString ss("ClosureVsRefPt_Overview");
      if(!flavor.IsNull()) ss+="_"+algs[a]+"_"+flavor;
      else ss+="_"+algs[a];

      TCanvas *ove = new TCanvas(ss,ss,1200,300);
      ove->Divide(4,1);
      for (int c=0;c<4;c++) {
        ove->cd(c+1);
        if (c<3) 
          gPad->SetLogx();
        if (!tdr) {
           hClosure[c]->GetXaxis()->SetMoreLogLabels();
           hClosure[c]->GetXaxis()->SetNoExponent();
        }
        hClosure[c]->Draw("EP");
        line->Draw("same");
        linePlus1->Draw("same");
        lineMinus1->Draw("same");
        linePlus2->Draw("same");
        lineMinus2->Draw("same");
        pave[c]->Draw();
        if (tdr) cmsPrel(13,0);
      }
      for(unsigned int iformat=0; iformat<outputFormat.size(); iformat++) {
        ove->SaveAs(outputDir+ss+outputFormat[iformat]);
      }
      ove->Write();

      //
      // create shared overview canvas
      //
      ss = "ClosureVsRefPt_Overview2";
      if(!flavor.IsNull()) ss+="_"+algs[a]+"_"+flavor;
      else ss+="_"+algs[a];

      TLegend* leg = tdrLeg(0.58,0.16,0.9,0.4);

      TH1D* frame = new TH1D();
      frame->GetXaxis()->SetLimits(6.0,4000.0);
      frame->GetXaxis()->SetMoreLogLabels();
      frame->GetXaxis()->SetNoExponent();
      frame->GetYaxis()->SetRangeUser(0.95,1.05);
      //frame->GetXaxis()->SetTitle("p_{T}^{GEN} [GeV]");
      frame->GetXaxis()->SetTitle("p_{T}^{ptcl} [GeV]");
      frame->GetYaxis()->SetTitle("Corrected Response");
      TCanvas* ove2 = tdrCanvas(ss,frame,4,11,true);
      ove2->GetPad(0)->SetLogx();

      for (int c=0;c<4;c++) {
         hClosure[c]->GetXaxis()->SetRangeUser(6.0,4000.0);
         hClosure[c]->SetStats(kFALSE);
         if (c==0) {
            tdrDraw(hClosure[c],"EP",kFullCircle,kBlack,kSolid,kBlack);
         }
         else if (c==1) {
            tdrDraw(hClosure[c],"EP",kOpenCircle,kBlue,kSolid,kBlue);
         }
         else if (c==2) {
            tdrDraw(hClosure[c],"EP",kFullTriangleUp,kRed,kSolid,kRed);
         }
         else {
            tdrDraw(hClosure[c],"EP",kOpenTriangleUp,kMagenta,kSolid,kMagenta);
         }
         leg->AddEntry(hClosure[c],Text[c],"lep");
         line->SetLineWidth(2);
         line->Draw("same");
         linePlus1->SetLineWidth(2);
         linePlus1->Draw("same");
         lineMinus1->SetLineWidth(2);
         lineMinus1->Draw("same");
         //linePlus2->SetLineWidth(2);
         //linePlus2->Draw("same");
         //lineMinus2->SetLineWidth(2);
         //lineMinus2->Draw("same");
         delete pave[c];
         pave[c] = tdrText(0.5,0.75,0.93,1-gPad->GetTopMargin()-0.045*(1-gPad->GetTopMargin()-gPad->GetBottomMargin()),31);
         if (tdr) {
            pave[c]->AddText("QCD Monte Carlo");
            pave[c]->AddText(JetInfo::get_legend_title(string(algs[a])).c_str());
         }
         else {
            pave[c]->AddText(algs[a]);
         }
         pave[c]->Draw("same");
         leg->Draw("same");
      }
      for(unsigned int iformat=0; iformat<outputFormat.size(); iformat++) {
        ove2->SaveAs(outputDir+ss+outputFormat[iformat]);
      }
      ove2->Write();
      
      outf->Close();
    }//for(unsigned int a=0; a<algs.size(); a++)
}

////////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________


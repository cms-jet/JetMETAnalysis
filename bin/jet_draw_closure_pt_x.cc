///////////////////////////////////////////////////////////////////
//
// jet_draw_closure_pt_x
// ---------------------
//
//            12/09/2011 Alexx Perloff  <aperloff@physics.tamu.edu>
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
#include "TString.h"
#include "TPaveText.h"

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
  
  vector<TString> algs         = cl.getVector<TString> ("algs");
  TString         flavor       = cl.getValue<TString>  ("flavor",           "");
  TString         path         = cl.getValue<TString>  ("path",             "");
  bool            mpv          = cl.getValue<bool>     ("mpv",            true);
  TString         outputDir    = cl.getValue<TString>  ("outputDir",  "images");
  TString         outputFormat = cl.getValue<TString>  ("outputFormat", ".png");

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
      // book histograms and functions
      //
      char name[1024];
      TH2F *RespVsPt[3];
      RespVsPt[0] = (TH2F*)inf->Get("RespVsPt_Bar");
      RespVsPt[1] = (TH2F*)inf->Get("RespVsPt_End");
      RespVsPt[2] = (TH2F*)inf->Get("RespVsPt_Fwd");
  
      TF1 *func[3][NPtBins];
      TH1D *h[3][NPtBins];
      TH1F *hClosure[3];
      hClosure[0] = new TH1F("ClosureVsPt_Bar","ClosureVsPt_Bar",NPtBins,vpt);
      hClosure[1] = new TH1F("ClosureVsPt_End","ClosureVsPt_End",NPtBins,vpt);
      hClosure[2] = new TH1F("ClosureVsPt_Fwd","ClosureVsPt_Fwd",NPtBins,vpt);
      double XminCalo[3] = {15,15,15};
      double XminPF[3] = {5,5,5};
      double Xmax[3] = {3000,3000,190};

      //
      // Find the mean peak of the gaussian fit or the mean of the histogram and 
      // fill the final histograms
      //
      for(int i=0;i<NPtBins;i++)
        { 
          for(int j=0;j<3;j++)
            {
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
            }
        }

      //
      // Create guides (lines) for the output histograms
      //
      TF1 *line = new TF1("line","0*x+1",0,5000);
      line->SetLineColor(1);
      line->SetLineWidth(1);
      line->SetLineStyle(2);
      TF1 *linePlus = new TF1("linePlus","0*x+1.02",0,5000);
      linePlus->SetLineColor(1);
      linePlus->SetLineWidth(1);
      linePlus->SetLineStyle(2);
      TF1 *lineMinus = new TF1("lineMinus","0*x+0.98",0,5000);
      lineMinus->SetLineColor(1);
      lineMinus->SetLineWidth(1);
      lineMinus->SetLineStyle(2);

      TCanvas *can[3];
      TString Text[3] = {"|#eta| < 1.3","1.3 < |#eta| < 3","3 < |#eta| < 5"};
      TPaveText *pave[3];

      //
      // Open/create the output directory and file
      //
      if(!outputDir.EndsWith("/")) outputDir+="/";
      if(!gSystem->OpenDirectory(outputDir)) gSystem->mkdir(outputDir);
      TString ofname = outputDir+"ClosureVsPt_"+algs[a]+".root";
      if(!flavor.IsNull()) ofname = outputDir+"ClosureVsPt_"+algs[a]+"_"+flavor+".root";
      TFile* outf = new TFile(ofname,"RECREATE");

      //
      // Format and save the output
      //
      for(int j=0;j<3;j++)
        {
          pave[j] = new TPaveText(0.3,0.75,0.8,0.9,"NDC");
          pave[j]->AddText(algs[a]);
          pave[j]->AddText(Text[j]);      
          sprintf(name,"ClosureVsPt_%d",j);
          TString ss(name);
          if(!flavor.IsNull()) ss+="_"+algs[a]+"_"+flavor;
          else ss+="_"+algs[a];
          can[j] = new TCanvas(ss,ss,800,800);
          if (j<2)
            gPad->SetLogx();
          if (ss.Contains("pf"))	
            hClosure[j]->GetXaxis()->SetRangeUser(XminPF[j],Xmax[j]);
          else
            hClosure[j]->GetXaxis()->SetRangeUser(XminCalo[j],Xmax[j]);	    
          hClosure[j]->GetXaxis()->SetTitle("GenJet p_{T} (GeV)"); 
          hClosure[j]->GetYaxis()->SetTitle("Corrected Response"); 
          hClosure[j]->GetXaxis()->SetLabelSize(0.04);
          hClosure[j]->GetXaxis()->SetMoreLogLabels();
          hClosure[j]->GetXaxis()->SetNoExponent();
          hClosure[j]->GetYaxis()->SetLabelSize(0.04); 
          hClosure[j]->SetMarkerColor(kBlue);
          hClosure[j]->SetMarkerSize(2.0);
          hClosure[j]->SetLineColor(kBlue);
          hClosure[j]->SetMaximum(1.1);
          hClosure[j]->SetMinimum(0.9);
          hClosure[j]->Draw();
          line->Draw("same");
          linePlus->Draw("same");
          lineMinus->Draw("same");
          pave[j]->SetFillColor(0);
          pave[j]->SetBorderSize(0);
          pave[j]->SetTextFont(42);
          pave[j]->SetTextSize(0.05);
          pave[j]->Draw();
          can[j]->SaveAs(outputDir+ss+outputFormat);
          hClosure[j]->Write();
          can[j]->Write();
        }//for(int j=0;j<3;j++)

      //
      // create overview canvas
      //
      TString ss("ClosureVsPt_Overview");
      if(!flavor.IsNull()) ss+="_"+algs[a]+"_"+flavor;
      else ss+="_"+algs[a];

      TCanvas *ove = new TCanvas(ss,ss,1200,400);
      ove->Divide(3,1);
      for (int c=0;c<3;c++) {
        ove->cd(c+1);
        if (c<2) 
          gPad->SetLogx();
        hClosure[c]->GetXaxis()->SetMoreLogLabels();
        hClosure[c]->GetXaxis()->SetNoExponent();
        hClosure[c]->Draw();
        line->Draw("same");
        linePlus->Draw("same");
        lineMinus->Draw("same");
        pave[c]->Draw();
      }
      ove->SaveAs(outputDir+ss+outputFormat);
      ove->Write();
      
      outf->Close();
    }//for(unsigned int a=0; a<algs.size(); a++)
}

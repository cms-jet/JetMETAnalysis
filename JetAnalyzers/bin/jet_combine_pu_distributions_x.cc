//
// jet_combine_pu_distributions_x
// ------------------------------
//
//            11/09/2011 Alexx Perloff  <aperloff@physics.tamu.edu>
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
#include "TLegend.h"
#include "TProfile.h"

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

      TCanvas *can[3];
      const char histNames[3][20] = {"DPtVsNPU","DPtVsPtGen","RespRatioVsPtGen"};
      const char pusources[3][10] = {"EOOT","IT","LOOT"};
      const char xAxisTitles[3][20] = {"N_{PU}^{GEN}","p_{T}^{GEN}","p_{T}^{GEN}"};
      const char yAxisTitles[3][100] = {"(R[p_{T}^{GEN},EOOT,IT,LOOT]-R'[p_{T}^{GEN},EOOT,IT,LOOT])*p_{T}^{GEN}","(R[p_{T}^{GEN},EOOT,IT,LOOT]-R'[p_{T}^{GEN},EOOT,IT,LOOT])*p_{T}^{GEN}","R[p_{T}^{GEN},EOOT,IT,LOOT]/R'[p_{T}^{GEN},EOOT,IT,LOOT]"};
      TProfile *hists[3][3];
      vector<TLegend*> leg;
      char name[1024];

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

      //
      // Open/create the output directory and file
      //
      if(!outputDir.EndsWith("/")) outputDir+="/";
      if(!gSystem->OpenDirectory(outputDir)) gSystem->mkdir(outputDir);
      TString ofname = outputDir+"PUDistributions_"+algs[a]+".root";
      if(!flavor.IsNull()) ofname = outputDir+"PUDistributions_"+algs[a]+"_"+flavor+".root";
      TFile* outf = new TFile(ofname,"RECREATE");

      for(int i=0;i<3;i++)
        {
          leg.push_back(new TLegend(0.7,0.8,1.0,1.0));
          sprintf(name,"%s",histNames[i]);
          TString ss(name);
          if(!flavor.IsNull()) ss+="_"+algs[a]+"_"+flavor;
          else ss+="_"+algs[a];
          can[i] = new TCanvas(ss,ss,800,800);
          if(i>0)
            gPad->SetLogx();
          for(int j=0; j<3; j++)
            {
              hists[i][j] = (TProfile*)inf->Get(TString(name)+"_"+pusources[j]);
              hists[i][j]->SetTitle(ss);
              hists[i][j]->GetXaxis()->SetTitle(xAxisTitles[i]); 
              hists[i][j]->GetYaxis()->SetTitle(yAxisTitles[i]);
              hists[i][j]->GetYaxis()->SetTitleOffset(1.4);
              hists[i][j]->GetYaxis()->SetTitleSize(0.03);
              hists[i][j]->GetXaxis()->SetLabelSize(0.035);
              hists[i][j]->GetYaxis()->SetLabelSize(0.035);
              hists[i][j]->GetXaxis()->SetMoreLogLabels();
              hists[i][j]->GetXaxis()->SetNoExponent();
              hists[i][j]->GetYaxis()->SetLabelSize(0.035); 
              hists[i][j]->SetMarkerColor(j+1);
              hists[i][j]->SetMarkerSize(2.0);
              hists[i][j]->SetLineColor(j+1);
              if(i==0)
                {
                  hists[i][j]->SetMaximum(12);
                  hists[i][j]->SetMinimum(-2);
                }
              if(i==1)
                {
                  hists[i][j]->SetMaximum(12);
                  hists[i][j]->SetMinimum(-2);
                }
              if(i==2)
                {
                  hists[i][j]->SetMaximum(1.03);
                  hists[i][j]->SetMinimum(0.97);
                }

              if(j==0)
                hists[i][j]->Draw();
              else
                hists[i][j]->Draw("same");

              leg[i]->AddEntry(hists[i][j],hists[i][j]->GetName(),"le");

            }//for(int j=0; j<3; j++)

          if(i==2)
            {
              line->Draw("same");
              linePlus->Draw("same");
              lineMinus->Draw("same");
            }
          leg[i]->SetFillColor(0);
          leg[i]->SetLineColor(0);
          leg[i]->Draw("same");
          can[i]->Write();
          can[i]->SaveAs(outputDir+ss+outputFormat);
        }//for(int i=0;i<3;i++)
      outf->Close();
    }//for(unsigned int a=0; a<algs.size(); a++)
}


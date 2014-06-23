///////////////////////////////////////////////////////////////////
//
// jet_draw_pileup_comparison_x
// ----------------------------
//
//            03/05/2012 Alexx Perloff  <aperloff@physics.tamu.edu>
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
#include "TProfile.h"
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
  TString         outputDir    = cl.getValue<TString>  ("outputDir",  "images");
  TString         outputFormat = cl.getValue<TString>  ("outputFormat", ".png");

  if (!cl.check()) return 0;
  cl.print();

  const unsigned int index =3;
  TString prefix[index] = {"DPtVsNPU","DPtVsPtGen","RespRatioVsPtGen"};
  TString suffix[index] = {"EOOT","IT","LOOT"};

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
      vector<TProfile*> h;
      for(unsigned int i=0; i<index; i++)
      {
         for(unsigned int j=0; j<index; j++)
         {
            h.push_back((TProfile*)inf->Get(prefix[j]+"_"+suffix[i]));
         }
      }

      TCanvas *can[9];
      TPaveText *pave[9];

      //
      // Open/create the output directory and file
      //
      if(!outputDir.EndsWith("/")) outputDir+="/";
      if(!gSystem->OpenDirectory(outputDir)) gSystem->mkdir(outputDir);
      TString ofname = outputDir+"PileupComparison_"+algs[a]+".root";
      if(!flavor.IsNull()) ofname = outputDir+"PileupComparison_"+algs[a]+"_"+flavor+".root";
      TFile* outf = new TFile(ofname,"RECREATE");

      //
      // Format and save the output
      //
      for(unsigned int i=0; i<index; i++)
      {
         for(unsigned int j=0; j<index; j++)
         {
            int k = (3*i) + j;
            pave[k] = new TPaveText(0.3,0.75,0.8,0.9,"NDC");
            pave[k]->AddText(algs[a]);      
            sprintf(name,"%s_%s",string(prefix[j]).c_str(),string(suffix[i]).c_str());
            TString ss(name);
            if(!flavor.IsNull()) ss+="_"+algs[a]+"_"+flavor;
            else ss+="_"+algs[a];
            can[k] = new TCanvas(ss,ss,800,600);
            if (k%3 == 0) {
               h[k]->GetXaxis()->SetTitle("NPU"); 
            } else {
               gPad->SetLogx();
               h[k]->GetXaxis()->SetTitle("p_{T}^{GEN} [GeV]"); 
            }
            if (k%2 ==0 ) {
               h[k]->GetYaxis()->SetTitleSize(0.02);
               h[k]->GetYaxis()->SetTitleOffset(2.0);
            } else {
               h[k]->GetYaxis()->SetTitleSize(0.03);
               h[k]->GetYaxis()->SetTitleOffset(1.3);
            }

            if (k==0) h[k]->GetYaxis()->SetTitle("(Resp[p_{T}^{GEN},EOOT,IT,LOOT]-Resp[p_{T}^{GEN},5,IT,LOOT])*p_{T}^{GEN}");
            else if (k==1) h[k]->GetYaxis()->SetTitle("(Resp[p_{T}^{GEN},EOOT,IT,LOOT]-Resp[p_{T}^{GEN},5,IT,LOOT])*p_{T}^{GEN}");
            else if (k==2) h[k]->GetYaxis()->SetTitle("#frac{Resp[p_{T}^{GEN},EOOT,IT,LOOT]}{Resp[p_{T}^{GEN},5,IT,LOOT]}");
            else if (k==3) h[k]->GetYaxis()->SetTitle("(Resp[p_{T}^{GEN},EOOT,IT,LOOT]-Resp[p_{T}^{GEN},EOOT,5,LOOT])*p_{T}^{GEN}");
            else if (k==4) h[k]->GetYaxis()->SetTitle("(Resp[p_{T}^{GEN},EOOT,IT,LOOT]-Resp[p_{T}^{GEN},EOOT,5,LOOT])*p_{T}^{GEN}");
            else if (k==5) h[k]->GetYaxis()->SetTitle("#frac{Resp[p_{T}^{GEN},EOOT,IT,LOOT]}{Resp[p_{T}^{GEN},EOOT,5,LOOT]}");
            else if (k==6) h[k]->GetYaxis()->SetTitle("(Resp[p_{T}^{GEN},EOOT,IT,LOOT]-Resp[p_{T}^{GEN},EOOT,IT,5])*p_{T}^{GEN}");
            else if (k==7) h[k]->GetYaxis()->SetTitle("(Resp[p_{T}^{GEN},EOOT,IT,LOOT]-Resp[p_{T}^{GEN},EOOT,IT,5])*p_{T}^{GEN}");
            else h[k]->GetYaxis()->SetTitle("#frac{Resp[p_{T}^{GEN},EOOT,IT,LOOT]}{Resp[p_{T}^{GEN},EOOT,IT,5]}");

            h[k]->Draw();
            pave[k]->SetFillColor(0);
            pave[k]->SetBorderSize(0);
            pave[k]->SetTextFont(42);
            pave[k]->SetTextSize(0.05);
            pave[k]->Draw();
            can[k]->SaveAs(outputDir+ss+outputFormat);
            h[k]->Write();
            can[k]->Write();
         }//for(int j=0;j<3;j++)
      }//for(unsigned int i=0; i<index; i++)

      outf->Close();
    }//for(unsigned int a=0; a<algs.size(); a++)
}

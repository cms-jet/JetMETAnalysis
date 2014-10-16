///////////////////////////////////////////////////////////////////
//
// jet_draw_closure_eta_x
// ----------------------
//
//            09/01/2011 Alexx Perloff  <aperloff@physics.tamu.edu>
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
#include "TH3F.h"
#include "TF1.h"
#include "TString.h"
#include "TPaveText.h"
#include "TMath.h"

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
  bool            mpv          = cl.getValue<bool>     ("mpv",           false);
  TString         outputDir    = cl.getValue<TString>  ("outputDir",  "images");
  TString         outputFormat = cl.getValue<TString>  ("outputFormat", ".png");

  if (!cl.check()) return 0;
  cl.print();

  for(unsigned int a=0; a<algs.size(); a++)
    {
      //
      // open the file containing the original, unformated closure plots
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
      TH2F *RespVsEta[NPtBins+1];
      TH1F *hClosure[NPtBins+1];
      for(int i=0;i<NPtBins;i++)
        {
          sprintf(name,"RespVsEta_RefPt%sto%s",Pt[i],Pt[i+1]);
          RespVsEta[i] = (TH2F*)inf->Get(name);
          sprintf(name,"ClosureVsEta_RefPt%sto%s",Pt[i],Pt[i+1]);
          hClosure[i] = new TH1F(name,name,NETA,veta);
        }
      //
      // book histogram for total pt range
      //
      RespVsEta[NPtBins] = (TH2F*)(((TH3F*)inf->Get("RespVsEtaVsPt"))->Project3D("zyoe"));
      sprintf(name,"ClosureVsEta_RefPt%sto%s",Pt[0],Pt[NPtBins]);
      hClosure[NPtBins] = new TH1F(name,name,NETA,veta);

      TH1D *h[NPtBins+1][NETA];
      TF1 *func[NPtBins+1][NETA];

      //
      // fill histograms
      //
      for(int i=0;i<NPtBins+1;i++)
        { 
          for(int j=0;j<NETA;j++)
            {
              sprintf(name,"h%d%d",i,j);
              if (i==3&&j>=70) sprintf(name,"h2%d%d",i,j);
              h[i][j] = RespVsEta[i]->ProjectionY(name,j,j); //final bin != j+1
              sprintf(name,"fit%d%d",i,j);
              func[i][j] = new TF1(name,"gaus",h[i][j]->GetMean()-1.5*h[i][j]->GetRMS(),h[i][j]->GetMean()+1.5*h[i][j]->GetRMS());
              if (h[i][j]->GetEntries()>5)
                {
                  h[i][j]->Fit(func[i][j],"RQ");
                  if(mpv)
                    {
                      hClosure[i]->SetBinContent(j+1,func[i][j]->GetParameter(1));
                      hClosure[i]->SetBinError(j+1,func[i][j]->GetParError(1));
                    }
                  else
                    {
                      hClosure[i]->SetBinContent(j+1,h[i][j]->GetMean());
                      hClosure[i]->SetBinError(j+1,h[i][j]->GetMeanError());
                    }
                }
              else 
                {
                  hClosure[i]->SetBinContent(j+1,h[i][j]->GetMean());
                  hClosure[i]->SetBinError(j+1,h[i][j]->GetMeanError());
                }
              
              //
              // perform basic sanity checks of max and mins
              //
              double binCont = hClosure[i]->GetBinContent(j+1);
              if (binCont > 1.10){
                cout<<"\tWARNING Closure for "<<hClosure[i]->GetName()<<" at eta="<< hClosure[i]->GetBinCenter(j+1)
                    <<" has relresp as high as "<<binCont<<" +/- "<<hClosure[i]->GetBinError(j+1)<<endl;
              }
              if (binCont >0 && binCont < 0.85){
                cout<<"\tWARNING Closure for "<<hClosure[i]->GetName()<<" at eta="<< hClosure[i]->GetBinCenter(j+1)
                    <<" has relresp as low as "<<binCont<<" +/- "<<hClosure[i]->GetBinError(j+1)<<endl;	      
              }
            }// for(int j=0;j<NETA;j++)
        }// for(int i=0;i<NPtBins+1;i++)

      //
      // Create guides (lines) for the output histograms
      //
      TF1 *line = new TF1("line","0*x+1",-6,6);
      line->SetLineColor(1);
      line->SetLineWidth(1);
      line->SetLineStyle(2);
      TF1 *linePlus = new TF1("linePlus","0*x+1.02",-6,6);
      linePlus->SetLineColor(1);
      linePlus->SetLineWidth(1);
      linePlus->SetLineStyle(2);
      TF1 *lineMinus = new TF1("lineMinus","0*x+0.98",-6,6);
      lineMinus->SetLineColor(1);
      lineMinus->SetLineWidth(1);
      lineMinus->SetLineStyle(2);

      TCanvas *can[NPtBins+1];
      TPaveText *pave[NPtBins+1];

      //
      // Open/create the output directory and file
      //
      if(!gSystem->OpenDirectory(outputDir)) gSystem->mkdir(outputDir);
      if(!outputDir.EndsWith("/")) outputDir+="/";
      TString ofname = outputDir+"ClosureVsEta_"+algs[a]+".root";
      if(!flavor.IsNull()) ofname = outputDir+"ClosureVsEta_"+algs[a]+"_"+flavor+".root";
      TFile* outf = new TFile(ofname,"RECREATE");

      //
      // Format and save the output
      //
      for(int i=0;i<NPtBins+1;i++)
        {
          TString ss;
          pave[i] = new TPaveText(0.3,0.75,0.8,0.9,"NDC");
          pave[i]->AddText(algs[a]);
          if(i!=NPtBins)
            {
              pave[i]->AddText(TString(Pt[i])+" < p_{T}^{gen} < "+TString(Pt[i+1])+" GeV");
              ss = "ClosureVsEta_RefPt"+TString(Pt[i])+"to"+TString(Pt[i+1])+"_"+algs[a];
            }
          else
            { 
              pave[i]->AddText(TString(Pt[0])+" < p_{T}^{gen} < "+TString(Pt[NPtBins])+" GeV");
              ss = "ClosureVsEta_RefPt"+TString(Pt[0])+"to"+TString(Pt[NPtBins])+"_"+algs[a];
            }
          if(!flavor.IsNull()) ss+="_"+flavor;
          can[i] = new TCanvas(ss,ss,800,800);   
          hClosure[i]->GetXaxis()->SetTitle("#eta"); 
          hClosure[i]->GetYaxis()->SetTitle("Response"); 
          float etaMax = TMath::ACosH(3500./vpt[i]);
          if(i!=NPtBins)            
            hClosure[i]->GetXaxis()->SetRangeUser(-etaMax,etaMax);
          else
            hClosure[i]->GetXaxis()->SetRangeUser(veta[0],veta[NETA]);
          hClosure[i]->SetMaximum(1.1);
          hClosure[i]->SetMinimum(0.9);
          hClosure[i]->SetMarkerColor(kBlue);
          hClosure[i]->SetMarkerSize(1.5);
          hClosure[i]->SetLineColor(kBlue);
          hClosure[i]->Draw();
          line->Draw("same");
          linePlus->Draw("same");
          lineMinus->Draw("same");
          pave[i]->SetFillColor(0);
          pave[i]->SetBorderSize(0);
          pave[i]->SetTextFont(42);
          pave[i]->SetTextSize(0.05);
          pave[i]->Draw();
          can[i]->Print(outputDir+"/"+ss+outputFormat);
          hClosure[i]->Write();
        }//for(int i=0;i<NPtBins+1;i++)

      TString ss("ClosureVsEta_Overview");
      if(!flavor.IsNull()) ss+="_"+algs[a]+"_"+flavor;
      else ss+="_"+algs[a];
  
      TCanvas *ove = new TCanvas(ss,ss,1200,800);
      ove->Divide(3,2);
      int ca[6] = {1,11,15,19,23,27};
      for (int c=0;c<6;c++) {
        if (ca[c] > NPtBins-1) {
          cout<<"\tWRONG PT requested"<<endl;
          continue;
        }
        ove->cd(c+1);
        hClosure[ca[c]]->Draw();
        line->Draw("same");
        linePlus->Draw("same");
        lineMinus->Draw("same");
        pave[ca[c]]->Draw();
      }
      ove->SaveAs(outputDir+ss+outputFormat);
      ove->Write();

      outf->Close();
    }//for(unsigned int a=0; a<algs.size(); a++)
}

//
// jet_combine_closure_eta_x
// -------------------------
//
//            10/19/2011 Alexx Perloff  <aperloff@physics.tamu.edu>
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
  vector<TString> paths        = cl.getVector<TString> ("paths",            "");
  TString         outputDir    = cl.getValue<TString>  ("outputDir",  "images");
  TString         outputFormat = cl.getValue<TString>  ("outputFormat", ".png");
  bool            combinePU    = cl.getValue<bool>     ("combinePU",     false);
  TString         divByNPU0    = cl.getValue<TString>  ("divByNPU0",        "");

  if (!cl.check()) return 0;
  cl.print();

  vector<TFile*> infs;
  vector<TFile*> infsNPU0;
  TString algNames;
  vector<TString> puLabelsVec;
  TString puLabels;
  for(unsigned int a=0; a<algs.size(); a++)
    {
      //
      // Open the file containing the original, unformated closure plots
      //
      if(paths.empty()) paths.push_back(string (gSystem->pwd())+"/");
      for(unsigned int p=0; p<paths.size(); p++)
        {     
          if(!paths[p].EndsWith("/")) paths[p]+="/";
          if(!divByNPU0.IsNull() && !divByNPU0.EndsWith("/")) divByNPU0+="/";

          if (!flavor.IsNull()) infs.push_back(new TFile(paths[p]+"ClosureVsEta_"+algs[a]+"_"+flavor+".root"));
          else infs.push_back(new TFile(paths[p]+"ClosureVsEta_"+algs[a]+".root"));
          if(!infs.back()->IsOpen())
            {
              cout << "WARNING:File " << paths[p] << "ClosureVsEta_" << algs[a] << ".root is was not opened." << endl << " Check the path and filename and try again." << endl << " The program will now exit." << endl;
              return 0;
            }

          if(!divByNPU0.IsNull())
            {
              if (!flavor.IsNull()) infsNPU0.push_back(new TFile(divByNPU0+"ClosureVsEta_"+algs[a]+"_"+flavor+".root"));
              else infsNPU0.push_back(new TFile(divByNPU0+"ClosureVsEta_"+algs[a]+".root"));
              if(!infsNPU0.back()->IsOpen())
                {
                  cout << "WARNING:File " << divByNPU0 << "ClosureVsEta_" << algs[a] << ".root is was not opened." << endl << " Check the path and filename and try again." << endl << " The program will now exit." << endl;
              return 0;
                }
            }

          TString path(paths[p]);
          puLabelsVec.push_back(path((TString(path(0,path.Length()-1))).Last('/')+1, (TString(path(0,paths[p].Length()-2))).Length() - (TString(path(0,path.Length()-1))).Last('/')));
          if(a==0)
            {
              if(p==0) puLabels+=puLabelsVec.back();
              else puLabels+="_"+puLabelsVec.back();
            }
        }

      if(a==0) algNames+=algs[a];
      else algNames+="_"+algs[a];
    }

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
  
  TH1F *h=0;
  vector<vector<TH1F*> > hNPU0 (NPtBins+1,vector<TH1F*>(infs.size(),h));
  vector<vector<TH1F*> > hClosure (NPtBins+1,vector<TH1F*>(infs.size(),h));
  //cout << "size = " << infs.size() << endl;
  vector<TLegend*> leg;

  //
  // Open/create the output directory and file
  //
  if(!outputDir.EndsWith("/")) outputDir+="/";
  if(!gSystem->OpenDirectory(outputDir)) gSystem->mkdir(outputDir);
  TString ofname = outputDir+"/ClosureVsEta_"+algNames+".root";
  if(combinePU) ofname.ReplaceAll("ClosureVsEta_","ClosureVsEta_"+puLabels+"_");
  if(!flavor.IsNull()) ofname.ReplaceAll(".root","_"+flavor+".root");
  TFile* outf = new TFile(ofname,"RECREATE");
  
  //
  // Format and save the output
  //
  for(int i=0;i<NPtBins+1;i++)
    {
      TString ss;
      leg.push_back(new TLegend(0.7,0.8,1.0,1.0));
      pave[i] = new TPaveText(0.3,0.7,0.8,0.8,"NDC");
      if(i!=NPtBins)
        {
          pave[i]->AddText(TString(Pt[i])+" < p_{T}^{gen} < "+TString(Pt[i+1])+" GeV");
          ss = "ClosureVsEta_RefPt"+TString(Pt[i])+"to"+TString(Pt[i+1]);
        }
      else
        {
          pave[i]->AddText(TString(Pt[0])+" < p_{T}^{gen} < "+TString(Pt[NPtBins])+" GeV");
          ss = "ClosureVsEta_RefPt"+TString(Pt[0])+"to"+TString(Pt[NPtBins]);
        }
      if(!divByNPU0.IsNull()) ss.ReplaceAll("ClosureVsEta","ClosureVsEtaRelNPU0");
      if(combinePU) ss+="_"+puLabels+"_"+algNames;
      else ss+="_"+algNames;
      if(!flavor.IsNull()) ss+="_"+flavor;
      can[i] = new TCanvas(ss,ss,800,800);

      for(unsigned int a=0; a<infs.size(); a++)
        {
          if(i!=NPtBins)
            {
              hClosure[i][a] = (TH1F*)infs[a]->Get("ClosureVsEta_RefPt"+TString(Pt[i])+"to"+TString(Pt[i+1]));
              if(!divByNPU0.IsNull())
                {
                  hNPU0[i][a] = (TH1F*)infsNPU0[a]->Get("ClosureVsEta_RefPt"+TString(Pt[i])+"to"+TString(Pt[i+1]));
                  hNPU0[i][a]->Sumw2();
                  if(!puLabelsVec[a].Contains("0_0"))
                    {
                      hClosure[i][a]->Divide(hNPU0[i][a]);
                    } 
                }
            }
          else
            hClosure[i][a] = (TH1F*)infs[a]->Get("ClosureVsEta_RefPt"+TString(Pt[0])+"to"+TString(Pt[NPtBins]));
          //cout << hClosure[i][a] << endl;

          hClosure[i][a]->GetXaxis()->SetTitle("#eta"); 
          hClosure[i][a]->GetYaxis()->SetTitle("Corrected Response");
          hClosure[i][a]->GetYaxis()->SetTitleOffset(1.25);
          float etaMax = TMath::ACosH(3500./vpt[i]);
          if(i!=NPtBins)  
            hClosure[i][a]->GetXaxis()->SetRangeUser(-etaMax,etaMax);
          else
            hClosure[i][a]->GetXaxis()->SetRangeUser(veta[0],veta[NETA]);
          hClosure[i][a]->SetMaximum(1.1);
          hClosure[i][a]->SetMinimum(0.9);
          hClosure[i][a]->SetMarkerColor(a+1);
          hClosure[i][a]->SetMarkerSize(1.5);
          hClosure[i][a]->SetLineColor(a+1);
          if(!divByNPU0.IsNull())
            {
              hClosure[i][a]->GetYaxis()->SetTitle("p_{T}^{RECO}/p_{T}^{GEN} (Relative to NPU=0)");
              hClosure[i][a]->GetXaxis()->SetTitle("#eta (radians)");
              hClosure[i][a]->SetTitle("N_{PU}^{GEN} = "+puLabelsVec[a]);
              hClosure[i][a]->SetMaximum(1.3);
              hClosure[i][a]->SetMinimum(0.94);
            }

          if(a==0)
            hClosure[i][a]->Draw();
          else
            hClosure[i][a]->Draw("same");

          if(combinePU) leg[i]->AddEntry(hClosure[i][a],puLabelsVec[a]+"_"+algs[a/paths.size()],"le");
          else leg[i]->AddEntry(hClosure[i][a],algs[a/paths.size()],"le");
        }
      line->Draw("same");
      linePlus->Draw("same");
      lineMinus->Draw("same");
      pave[i]->SetFillColor(0);
      pave[i]->SetBorderSize(0);
      pave[i]->SetTextFont(42);
      pave[i]->SetTextSize(0.05);
      pave[i]->Draw();
      leg[i]->SetFillColor(0);
      leg[i]->SetLineColor(0);
      leg[i]->Draw("same");
      can[i]->Print(outputDir+"/"+ss+outputFormat);
      can[i]->Write();
    }
  outf->Close();
}


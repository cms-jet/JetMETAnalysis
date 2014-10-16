//
// jet_combine_relrsp_resolution_x
// -------------------------------
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
  vector<TString> paths        = cl.getVector<TString> ("paths",            "");
  TString         outputDir    = cl.getValue<TString>  ("outputDir",  "images");
  TString         outputFormat = cl.getValue<TString>  ("outputFormat", ".png");
  bool            combinePU    = cl.getValue<bool>     ("combinePU",     false);

  if (!cl.check()) return 0;
  cl.print();

  vector<TFile*> infs;
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

          if (!flavor.IsNull()) infs.push_back(new TFile(paths[p]+"Closure_"+algs[a]+"_"+flavor+".root"));
          else infs.push_back(new TFile(paths[p]+"Closure_"+algs[a]+".root"));
          
          if(!infs.back()->IsOpen())
            {
              cout << "WARNING:File " << paths[p] << "Closure_" << algs[a] << ".root is was not opened." << endl << " Check the path and filename and try again." << endl << " The program will now exit." << endl;
              return 0;
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
  
  TH1F *h=0;
  TProfile *p=0;
  vector<TProfile*> relrsp (infs.size(),p);
  vector<TH1F*> res (infs.size(),h);
  TLegend* leg_relrsp;
  TLegend* leg_res;

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
  TString ofname = outputDir+"RelRsp_Resolution_"+algNames+".root";
  if(!flavor.IsNull()) ofname.ReplaceAll(".root","_"+flavor+".root");
  TFile* outf = new TFile(ofname,"RECREATE");

  //
  // Format and save the output
  //

  leg_relrsp = new TLegend(0.7,0.8,1.0,1.0);
  leg_res = new TLegend(0.7,0.8,1.0,1.0);
  TString ss_relrsp = "RelRspVsPileupPtSum";
  TString ss_res = "ResolutionVsEta";
  if(combinePU) 
    {
      ss_relrsp+="_"+puLabels+"_"+algNames;
      ss_res+="_"+puLabels+"_"+algNames;
    }
  else
    {
      ss_relrsp+="_"+algNames;
      ss_res+="_"+algNames;
    }
  if(!flavor.IsNull()) 
    {
      ss_relrsp+="_"+flavor;
      ss_res+="_"+flavor;
    }
  TCanvas *can_relrsp = new TCanvas(ss_relrsp,ss_relrsp,800,800);
  TCanvas *can_res = new TCanvas(ss_res,ss_res,800,800); 
  
  for(unsigned int a=0; a<infs.size(); a++)
    {
      relrsp[a] = (TProfile*)infs[a]->Get("RelRspVsSumPt");
      relrsp[a]->SetTitle("RelRsp Vs. Pileup p_{T}-Sum");
      relrsp[a]->GetXaxis()->SetTitle("Pileup p_{T}-Sum"); 
      relrsp[a]->GetYaxis()->SetTitle("p_{T}^{REC}/p_{T}^{GEN}"); 
      relrsp[a]->GetYaxis()->SetTitleOffset(1.15);
      relrsp[a]->GetXaxis()->SetLabelSize(0.03);
      relrsp[a]->GetYaxis()->SetLabelSize(0.03); 
      relrsp[a]->SetMarkerColor(a+1);
      relrsp[a]->SetMarkerSize(2.0);
      relrsp[a]->SetLineColor(a+1);
      relrsp[a]->SetMaximum(1.2);
      relrsp[a]->SetMinimum(0.8);
      relrsp[a]->GetXaxis()->SetRangeUser(0,3000);

      res[a] = (TH1F*)infs[a]->Get("ResolutionVsEta");
      res[a]->SetTitle("Gaussian Response Width Vs. #eta");
      res[a]->GetXaxis()->SetTitle("#eta (radians)"); 
      res[a]->GetYaxis()->SetTitle("#sigma(p_{T}^{REC}/p_{T}^{GEN})/<p_{T}^{REC}/p_{T}^{GEN}>"); 
      res[a]->GetYaxis()->SetTitleOffset(1.15);
      res[a]->GetXaxis()->SetLabelSize(0.03);
      res[a]->GetYaxis()->SetLabelSize(0.03); 
      res[a]->SetMarkerColor(a+1);
      res[a]->SetMarkerSize(2.0);
      res[a]->SetLineColor(a+1);
      
      if(a==0)
        {
          can_relrsp->cd();
          gPad->SetLogx();
          relrsp[a]->Draw();
          can_res->cd();
          res[a]->Draw();
        }
      else
        {
          can_relrsp->cd();
          relrsp[a]->Draw("same");
          can_res->cd();
          res[a]->Draw("same");
        }
      
      if(combinePU)
        {
          leg_relrsp->AddEntry(relrsp[a],puLabelsVec[a]+"_"+algs[a/paths.size()],"le");
          leg_res->AddEntry(res[a],puLabelsVec[a]+"_"+algs[a/paths.size()],"le");
        }
      else
        {
          leg_relrsp->AddEntry(relrsp[a],algs[a/paths.size()],"le");
          leg_res->AddEntry(res[a],algs[a/paths.size()],"le");
        }
    }
  can_relrsp->cd();
  leg_relrsp->SetFillColor(0);
  leg_relrsp->SetLineColor(0);
  leg_relrsp->Draw("same");
  line->Draw("same");
  linePlus->Draw("same");
  lineMinus->Draw("same");
  can_res->cd();
  leg_res->SetFillColor(0);
  leg_res->SetLineColor(0);
  leg_res->Draw("same");

  can_relrsp->Write();
  can_res->Write();
  outf->Close();

  can_relrsp->SaveAs(outputDir+ss_relrsp+outputFormat);
  can_res->SaveAs(outputDir+ss_res+outputFormat);
}

//
// jet_combine_closure_pt_x
// ------------------------
//
//            10/18/2011 Alexx Perloff  <aperloff@physics.tamu.edu>
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
#include "TLegend.h"
#include "TLatex.h"

#include <fstream>
#include <string>
#include <stdio.h>
#include <stdarg.h>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// define local functions
////////////////////////////////////////////////////////////////////////////////

///Get colors for the markers
vector<Int_t> getColors();

///Get styles for the markers
vector<Int_t> getMarkerNumbers();

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
  vector<TString> outputFormat = cl.getVector<TString> ("outputFormat", ".png:::.eps:::.pdf");
  bool            combinePU    = cl.getValue<bool>     ("combinePU",     false);
  TString         divByNPU0    = cl.getValue<TString>  ("divByNPU0",        "");
  bool            tdr          = cl.getValue<bool>     ("tdr",           false);

  if (!cl.check()) return 0;
  cl.print();

  if (tdr) {
     setTDRStyle();
  }

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

          if (!flavor.IsNull()) infs.push_back(new TFile(paths[p]+"ClosureVsPt_"+algs[a]+"_"+flavor+".root"));
          else infs.push_back(new TFile(paths[p]+"ClosureVsPt_"+algs[a]+".root"));
          if(!infs.back()->IsOpen())
            {
              cout << "WARNING:File " << paths[p] << "ClosureVsPt_" << algs[a] << ".root is was not opened." << endl << " Check the path and filename and try again." << endl << " The program will now exit." << endl;
              return 0;
            }

          if(!divByNPU0.IsNull())
            {
              if (!flavor.IsNull()) infsNPU0.push_back(new TFile(divByNPU0+"ClosureVsPt_"+algs[a]+"_"+flavor+".root"));
              else infsNPU0.push_back(new TFile(divByNPU0+"ClosureVsPt_"+algs[a]+".root"));
              if(!infsNPU0.back()->IsOpen())
                {
                  cout << "WARNING:File " << divByNPU0 << "ClosureVsPt_" << algs[a] << ".root is was not opened." << endl << " Check the path and filename and try again." << endl << " The program will now exit." << endl;
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
  TF1 *line = new TF1("line","0*x+1",0,5000);
  line->SetLineColor(1);
  line->SetLineWidth(1);
  line->SetLineStyle(2);
  TF1 *linePlus;
  if(tdr)
     linePlus = new TF1("linePlus","0*x+1.01",0,5000);
  else
     linePlus = new TF1("linePlus","0*x+1.02",0,5000);
  linePlus->SetLineColor(1);
  linePlus->SetLineWidth(1);
  linePlus->SetLineStyle(2);
  TF1 *lineMinus;
  if(tdr)
     lineMinus = new TF1("lineMinus","0*x+0.99",0,5000);
  else
     lineMinus = new TF1("lineMinus","0*x+0.98",0,5000);
  lineMinus->SetLineColor(1);
  lineMinus->SetLineWidth(1);
  lineMinus->SetLineStyle(2);
  
  TCanvas *can[4];
  TString Text[4] = {"|#eta| < 1.3","1.3 < |#eta| < 2.5","2.5 < |#eta| < 3.0","3 < |#eta| < 5"};
  TPaveText *pave[4];

  double XminCalo[4] = {15,15,15,15};
  double XminPF[4] = {5,5,5,5};
  if(tdr) {XminPF[0] = 10; XminPF[1] = 10; XminPF[2] = 10; XminPF[3] = 10;}
  double Xmax[4] = {3000,3000,3000,190};
  if(tdr) {Xmax[0] = 1000; Xmax[1] = 1000; Xmax[2] = 390; Xmax[3] = 190;}
  vector<Int_t> colors = getColors();
  vector<Int_t> markers = getMarkerNumbers();
  
  char name[1024];
  TH1F *h=0;
  vector<vector<TH1F*> > hNPU0 (4,vector<TH1F*>(infs.size(),h));
  vector<vector<TH1F*> > hClosure (4,vector<TH1F*>(infs.size(),h));
  //cout << "size = " << infs.size() << endl;
  vector<TLegend*> leg;

  //
  // Open/create the output directory and file
  //
  if(!outputDir.EndsWith("/")) outputDir+="/";
  if(!gSystem->OpenDirectory(outputDir)) gSystem->mkdir(outputDir);
  TString ofname = outputDir+"ClosureVsPt_"+algNames+".root";
  if(combinePU) ofname.ReplaceAll("ClosureVsPt_","ClosureVsPt_"+puLabels+"_");
  if(!flavor.IsNull()) ofname.ReplaceAll(".root","_"+flavor+".root");
  TFile* outf = new TFile(ofname,"RECREATE");

  //
  // Format and save the output
  //
  TH1D* frame = new TH1D();
  frame->GetXaxis()->SetLimits(10.0,2000.0);
  frame->GetXaxis()->SetMoreLogLabels();
  frame->GetXaxis()->SetNoExponent();
  frame->GetYaxis()->SetRangeUser(0.94,1.06);
  frame->GetXaxis()->SetTitle("p_{T}^{ptcl} [GeV]");
  frame->GetYaxis()->SetTitle("Corrected Response");
  for(int j=0;j<4;j++)
    {
      if(tdr) {
         //leg.push_back(new TLegend(0.25,0.2,0.9,0.35));
         leg.push_back(tdrLeg(0.25,0.2,0.9,0.35));
         leg.back()->SetNColumns(3);
         //leg.back()->SetTextSize(0.04);
       }
      else
         leg.push_back(new TLegend(0.7,0.8,1.0,1.0));
      if(tdr) {
         pave[j] = tdrText(0.5,0.75,0.93,1-gPad->GetTopMargin()-0.045*(1-gPad->GetTopMargin()-gPad->GetBottomMargin())-0.02,31);
         //pave[j] = new TPaveText(0.3,0.75,0.8,0.9,"NDC");
       }
      else {
         pave[j] = new TPaveText(0.3,0.9,0.8,1.0,"NDC");
         pave[j]->SetTextSize(0.04);
       }
      pave[j]->AddText("QCD Monte Carlo");
      pave[j]->AddText(JetInfo::get_legend_title(string(algs[0]),false).c_str());
      pave[j]->AddText(Text[j]);      
      sprintf(name,"ClosureVsPt_%d",j);
      TString ss(name);
      if(!divByNPU0.IsNull()) ss.ReplaceAll("ClosureVsPt","ClosureVsPtRelNPU0");
      if(combinePU) ss+="_"+puLabels+"_"+algNames;
      else ss+="_"+algNames;
      if(!flavor.IsNull()) ss+="_"+flavor;
      can[j] = tdrCanvas(ss,frame,2,11,true);//new TCanvas(ss,ss,800,800);
      if (j<2)
        gPad->SetLogx();//can[j]->GetPad(0)->SetLogx();
  
      for(unsigned int a=0; a<infs.size(); a++)
        {
          if(j==0)
            {
              hClosure[j][a] = (TH1F*)infs[a]->Get("ClosureVsPt_Bar");
              if(!divByNPU0.IsNull())
                {
                  hNPU0[j][a] = (TH1F*)infsNPU0[a]->Get("ClosureVsPt_Bar");
                }
            }
          else if(j==1)
            {
              hClosure[j][a] = (TH1F*)infs[a]->Get("ClosureVsPt_IEnd");
              if(!divByNPU0.IsNull())
                { 
                  hNPU0[j][a] = (TH1F*)infsNPU0[a]->Get("ClosureVsPt_IEnd");
                }
            }
          else if(j==2)
            {
              hClosure[j][a] = (TH1F*)infs[a]->Get("ClosureVsPt_OEnd");
              if(!divByNPU0.IsNull())
                { 
                  hNPU0[j][a] = (TH1F*)infsNPU0[a]->Get("ClosureVsPt_OEnd");
                }
            }
          else if(j==3)
            {
              hClosure[j][a] = (TH1F*)infs[a]->Get("ClosureVsPt_Fwd");
              if(!divByNPU0.IsNull())
                {
                  hNPU0[j][a] = (TH1F*)infsNPU0[a]->Get("ClosureVsPt_Fwd");
                }
            }

          if(!divByNPU0.IsNull() && !puLabelsVec[a].Contains("0_0"))
            {
              hNPU0[j][a]->Sumw2();
              hClosure[j][a]->Divide(hNPU0[j][a]);
            }

          if (ss.Contains("pf"))	
            hClosure[j][a]->GetXaxis()->SetRangeUser(XminPF[j],Xmax[j]);
          else
            hClosure[j][a]->GetXaxis()->SetRangeUser(XminCalo[j],Xmax[j]);	    
          //hClosure[j][a]->GetXaxis()->SetTitle("p_{T}^{GEN} [GeV]"); 
          //hClosure[j][a]->GetYaxis()->SetTitle("Corrected Response");
          //hClosure[j][a]->GetYaxis()->SetTitleOffset(1.25);
          //hClosure[j][a]->GetXaxis()->SetLabelSize(0.035);
          //hClosure[j][a]->GetXaxis()->SetMoreLogLabels();
          //hClosure[j][a]->GetXaxis()->SetNoExponent();
          //hClosure[j][a]->GetYaxis()->SetLabelSize(0.035); 
          if (tdr) {
             //hClosure[j][a]->SetMarkerStyle(20);
             //hClosure[j][a]->SetMarkerSize(1.2);
          }
          else {
             hClosure[j][a]->SetMarkerSize(2.0);
          }
          hClosure[j][a]->SetMarkerColor(colors[a]); //a+1
          hClosure[j][a]->SetLineColor(colors[a]); //a+1
          if(tdr) {
            hClosure[j][a]->SetMaximum(1.06);
            hClosure[j][a]->SetMinimum(0.94);
          }
          else {
            hClosure[j][a]->SetMaximum(1.1);
            hClosure[j][a]->SetMinimum(0.9);
          }
          if(!divByNPU0.IsNull() && !puLabelsVec[a].Contains("0_0"))
            {
              hClosure[j][a]->GetYaxis()->SetTitle("p_{T}^{RECO}/p_{T}^{GEN} (Relative to NPU=0)");
              hClosure[j][a]->GetXaxis()->SetTitle("p_{T}^{GEN} (GeV/c)");
              hClosure[j][a]->SetTitle("N_{PU}^{GEN} = "+puLabelsVec[a]);
              //hClosure[j][a]->GetXaxis()->SetRangeUser(30,500);
              //hClosure[j][a]->SetMaximum(1.3);
              //hClosure[j][a]->SetMinimum(0.98);
              //gPad->SetLogx(0);
            }

          //if(a==0)
          //    hClosure[j][a]->Draw();
          //else
          //  hClosure[j][a]->Draw("same");
          //tdrDraw(hClosure[j][a],"",kFullCircle,colors[a],kSolid,colors[a],kNone,0);
            tdrDraw(hClosure[j][a],"",markers[a],colors[a],kSolid,colors[a],kNone,0);

          //cout << "a = " << a << "    hClosure[" << j << "][" << a <<"] = " << hClosure[j][a] << endl;
          //if (a>=4) hClosure[j][4]->Draw("same");
          hClosure[j][a]->Write();

          JetInfo ji(algs[a/paths.size()]);
          if(combinePU) leg[j]->AddEntry(hClosure[j][a],puLabelsVec[a]+"_"+algs[a/paths.size()],"lep");
          //else leg[j]->AddEntry(hClosure[j][a],algs[a/paths.size()],"lep");
          //else leg[j]->AddEntry(hClosure[j][a],JetInfo::get_legend_title(string(algs[a/paths.size()]),true).c_str(),"lep");
          else leg[j]->AddEntry(hClosure[j][a],Form("R=%.1f",ji.coneSize/10.0),"lep");
        }
      line->Draw("same");
      linePlus->Draw("same");
      lineMinus->Draw("same");
      if(tdr) {
        pave[j]->Draw("same");
        leg[j]->Draw("same");
      }
      else {
        pave[j]->SetFillColor(0);
        pave[j]->SetBorderSize(0);
        pave[j]->SetTextFont(42);
        pave[j]->SetTextSize(0.05);
        pave[j]->Draw();
        leg[j]->SetFillColor(0);
        leg[j]->SetLineColor(0);
        leg[j]->Draw("same");
      }
      //if (tdr) cmsPrelim();
      can[j]->Write();
      for(unsigned int iformat=0; iformat<outputFormat.size(); iformat++) {
         can[j]->SaveAs(outputDir+ss+outputFormat[iformat]);
      }
    }
  outf->Close();
  
}

////////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
vector<Int_t> getColors() {
  vector<Int_t> ret;
  ret.push_back(kBlack);
  ret.push_back(kRed);
  ret.push_back(kOrange+1);
  ret.push_back(kYellow+2);
  ret.push_back(kGreen);
  ret.push_back(kCyan);
  ret.push_back(kAzure+1);
  ret.push_back(kViolet+1);
  ret.push_back(kMagenta);
  ret.push_back(kGray+2);
  return ret;
}

//______________________________________________________________________________
vector<Int_t> getMarkerNumbers() {
  vector<Int_t> ret;
  ret.push_back(20);
  ret.push_back(33);
  ret.push_back(22);
  ret.push_back(21);
  ret.push_back(24);
  ret.push_back(27);
  ret.push_back(26);
  ret.push_back(25);
  ret.push_back(28);
  ret.push_back(34);
  return ret;
}

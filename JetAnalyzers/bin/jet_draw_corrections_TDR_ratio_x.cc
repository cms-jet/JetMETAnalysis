///////////////////////////////////////////////////////////////////
//
// jet_draw_correction_TDR_ratio_x
// -------------------------------
//
//            09/01/2011 Alexx Perloff  <aperloff@physics.tamu.edu>
///////////////////////////////////////////////////////////////////

#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetAnalyzers/interface/Settings.h"
#include "JetMETAnalysis/JetUtilities/interface/Style.h"

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
#include "TMath.h"

#include <vector>
#include <string>
#include <iomanip>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// define local functions
////////////////////////////////////////////////////////////////////////////////

/// get the uppercase version of the algorithm name
TString getAlias(TString s);

/// scans the histogram for a given pt and sets the bin content to -1 if the eta
/// bin is outside the physical region for that pt
void scanBins(double CMEnergy, double pt, TH1* histo);

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
  
   string          filepath1    = cl.getValue<string>   ("filepath1");
   string          filepath2    = cl.getValue<string>   ("filepath2");
   string          labelNum     = cl.getValue<string>   ("labelNum");
   string          labelDen     = cl.getValue<string>   ("labelDen");
   string          algoNum      = cl.getValue<string>   ("algoNum");
   string          algoDen      = cl.getValue<string>   ("algoDen");
   bool            doflavor     = cl.getValue<bool>     ("doflavor",             false);
   TString         outputDir    = cl.getValue<TString>  ("outputDir",         "images");
   vector<TString> outputFormat = cl.getVector<TString> ("outputFormat", ".png:::.eps");
   bool            tdr          = cl.getValue<bool>     ("tdr",                  false);
   double          CMEnergy     = cl.getValue<double>   ("CMEnergy",             13000);

   if (!cl.check()) return 0;
   cl.print();

   if (tdr) {
      setTDRStyle();
   }

   //
   // Open the files containing the original correction plots and retrieve the histograms
   //
   TFile* file1 = new TFile(filepath1.c_str(),"READ");
   TH1F* f1_pt0 = (TH1F*)gDirectory->Get("EtaSF_TDR_0");
   TH1F* f1_pt1 = (TH1F*)gDirectory->Get("EtaSF_TDR_1");
   TH1F* f1_pt2 = (TH1F*)gDirectory->Get("EtaSF_TDR_2");
   if(f1_pt0==0 || f1_pt1==0 || f1_pt2==0)
   {
      cout << "One or more of the histogram pointers from file " << filepath1 << " is NULL." << endl;
   }
   TFile* file2 = new TFile(filepath2.c_str(),"READ");
   TH1F* f2_pt0 = (TH1F*)gDirectory->Get("EtaSF_TDR_0");
   TH1F* f2_pt1 = (TH1F*)gDirectory->Get("EtaSF_TDR_1");
   TH1F* f2_pt2 = (TH1F*)gDirectory->Get("EtaSF_TDR_2");
   if(f2_pt0==0 || f2_pt1==0 || f2_pt2==0)
   {
      cout << "One or more of the histogram pointers from file " << filepath2 << " is NULL." << endl;
   }

   //
   // book histograms
   //
   TH1F* ratioHist[3];
   ratioHist[0] = new TH1F("CorrectionRatio_pt0","CorrectionRatio_pt0",f1_pt0->GetNbinsX(),f1_pt0->GetXaxis()->GetXmin(),f1_pt0->GetXaxis()->GetXmax()); //barRatio->Sumw2();
   ratioHist[1] = new TH1F("CorrectionRatio_pt1","CorrectionRatio_pt1",f1_pt1->GetNbinsX(),f1_pt1->GetXaxis()->GetXmin(),f1_pt1->GetXaxis()->GetXmax()); //endRatio->Sumw2();
   ratioHist[2] = new TH1F("CorrectionRatio_pt2","CorrectionRatio_pt2",f1_pt2->GetNbinsX(),f1_pt2->GetXaxis()->GetXmin(),f1_pt2->GetXaxis()->GetXmax()); //fwdRatio->Sumw2();

   //
   // divide histograms
   //
   TH1F* f1_pt0copy = (TH1F*)f1_pt0->Clone();
   TH1F* f1_pt1copy = (TH1F*)f1_pt1->Clone();
   TH1F* f1_pt2copy = (TH1F*)f1_pt2->Clone();
   f1_pt0copy->Divide(f2_pt0);
   f1_pt1copy->Divide(f2_pt1);
   f1_pt2copy->Divide(f2_pt2);
   ratioHist[0] = f1_pt0copy;
   ratioHist[1] = f1_pt1copy;
   ratioHist[2] = f1_pt2copy;  

   TCanvas* can[3];
   char name[1024];
   TString flavor1, flavor2;
   if(doflavor)
   {
      flavor1 = filepath1.substr(int(filepath1.rfind("_")+1),int(filepath1.rfind(".root")-filepath1.rfind("_")-1));
      flavor2 = filepath2.substr(int(filepath2.rfind("_")+1),int(filepath2.rfind(".root")-filepath2.rfind("_")-1));
   }
    //Create canvas vs eta for different pts  
   vector<double> PtVals;
   PtVals.push_back(30);
   PtVals.push_back(100);
   PtVals.push_back(300);

   // Create a legend for pt values
   TLegend * leg[3];
 
   //Create a pave indicating the algorithm name
   TString algNameLong;
   if(TString(algoNum).Contains("ak"))        algNameLong += "Anti-kT";
   if(TString(algoNum).Contains("1")&&!TString(algoNum).Contains("10")&&!TString(algoNum).Contains("l1")) algNameLong += " R=0.1";
   else if(TString(algoNum).Contains("2"))    algNameLong += " R=0.2";
   else if(TString(algoNum).Contains("3"))    algNameLong += " R=0.3";
   else if(TString(algoNum).Contains("4"))    algNameLong += " R=0.4";
   else if(TString(algoNum).Contains("5"))    algNameLong += " R=0.5";
   else if(TString(algoNum).Contains("6"))    algNameLong += " R=0.6";
   else if(TString(algoNum).Contains("7"))    algNameLong += " R=0.7";
   else if(TString(algoNum).Contains("8"))    algNameLong += " R=0.8";
   else if(TString(algoNum).Contains("9"))    algNameLong += " R=0.9";
   else if(TString(algoNum).Contains("10"))   algNameLong += " R=1.0";
   if(TString(algoNum).Contains("pfchs"))     algNameLong += ", PF+CHS";
   else if(TString(algoNum).Contains("pf"))   algNameLong += ", PF";
   else if(TString(algoNum).Contains("calo")) algNameLong += ", Calo";
   else if(TString(algoNum).Contains("jpt"))  algNameLong += ", JPT";
   else if(TString(algoNum).Contains("puppi"))  algNameLong += ", PUPPI";
 
   //
   // Open/create the output directory and file
   //
   if(!gSystem->OpenDirectory(outputDir)) gSystem->mkdir(outputDir);
   TString ofname = outputDir+"/CorrectionTDRRatio_"+algoNum+".root";
   if(doflavor) ofname = outputDir+"/CorrectionTDRRatio_"+algoNum+"_"+flavor1+"over"+flavor2+".root";
   TFile* outf = new TFile(ofname,"RECREATE");

   //
   // Format and save the output
   //
   for(int i=0; i<3; i++)
   {
      leg[i] = new TLegend(0.3,0.7,0.8,0.9);
      //TLegend * leg = new TLegend(0.5,0.7,0.9,0.9);
      leg[i]->SetTextSize(0.04);
      leg[i]->SetBorderSize(0);
      leg[i]->SetFillColor(0);
      leg[i]->AddEntry((TObject*)0,algNameLong,"");             
      TString ptstr;
      if (PtVals[i]<0.1)
         ptstr.Form("%f",PtVals[i]);
      else
         ptstr.Form("%.1f",PtVals[i]);
      leg[i]->AddEntry((TObject*)0,"P_{T}="+ptstr,"");
      leg[i]->AddEntry((TObject*)0,"","");

      sprintf(name,"CorrectionRatio_%d",i);
      TString ss(name);
      ss+="_"+algoNum;
      if (doflavor) ss += "_"+flavor1+"over"+flavor2;
      can[i] = new TCanvas(ss,ss,1200,800);
      ratioHist[i]->SetTitle(ss);
      ratioHist[i]->GetXaxis()->SetTitle("#eta");
      scanBins(CMEnergy, PtVals[i], ratioHist[i]);
      ratioHist[i]->GetYaxis()->SetTitle("Corr. Factor");
      if(TString(algoNum).Contains("calo"))
         ratioHist[i]->GetYaxis()->SetRangeUser(0.90,2.5);
      else
         ratioHist[i]->GetYaxis()->SetRangeUser(0.90,1.8);
      ratioHist[i]->SetFillColor(30);
      ratioHist[i]->SetFillStyle(3001);
      //Set marker colors and styles
      ratioHist[i]->SetMarkerColor(kBlack);
      ratioHist[i]->SetLineColor(kBlack);
      ratioHist[i]->SetMarkerSize(1.6);
      if(i == 0) {
         ratioHist[i]->SetMarkerStyle(24);
      }
      else if(i == 1) {
         ratioHist[i]->SetMarkerStyle(21);
      }
      else if(i == 2) {
         ratioHist[i]->SetMarkerStyle(20);
      }
      else {
         ratioHist[i]->SetMarkerStyle(25);
      }
      
      ratioHist[i]->Draw("P");
      leg[i]->AddEntry(ratioHist[i],"Ratio = #frac{PU}{No PU}","p");

      if (tdr) cmsPrel(13,0);
      for(unsigned int f=0; f<outputFormat.size(); f++) {
         can[i]->SaveAs(outputDir+"/"+ss+outputFormat[f]);
      }
      ratioHist[i]->Write();
      can[i]->Write();
   }
   //
   // create overview canvas
   //
   TString ss("CorrectionRatio_Overview");
   ss+="_"+algoNum;
   if(doflavor) ss+="_"+flavor1+"over"+flavor2;
   
   TCanvas *ove = new TCanvas(ss,ss,2400,800);
   ove->Divide(3,1);
   for (int c=0;c<3;c++) {
      ove->cd(c+1);
      ratioHist[c]->Draw("P");
      if (c==0) {
         f1_pt0->SetMarkerStyle(24);
         f1_pt0->SetMarkerColor(kBlue);
         f1_pt0->SetLineColor(kBlue);
         f1_pt0->SetMarkerSize(1.6);
         f2_pt0->SetMarkerStyle(24);
         f2_pt0->SetMarkerColor(kRed);
         f2_pt0->SetLineColor(kRed);
         f2_pt0->SetMarkerSize(1.6);
         f1_pt0->Draw("Psame");
         f2_pt0->Draw("Psame");
         leg[c]->AddEntry(f1_pt0,"PU","p");
         leg[c]->AddEntry(f2_pt0,"No PU","p");
      }
      else if (c==1) {
         f1_pt1->SetMarkerStyle(21);
         f1_pt1->SetMarkerColor(kBlue);
         f1_pt1->SetLineColor(kBlue);
         f1_pt1->SetMarkerSize(1.6);
         f2_pt1->SetMarkerStyle(21);
         f2_pt1->SetMarkerColor(kRed);
         f2_pt1->SetLineColor(kRed);
         f2_pt1->SetMarkerSize(1.6);
         f1_pt1->Draw("Psame");
         f2_pt1->Draw("Psame");
         leg[c]->AddEntry(f1_pt1,"PU","p");
         leg[c]->AddEntry(f2_pt1,"No PU","p");
      }
      else {
         f1_pt2->SetMarkerStyle(20);
         f1_pt2->SetMarkerColor(kBlue);
         f1_pt2->SetLineColor(kBlue);
         f1_pt2->SetMarkerSize(1.6);
         f2_pt2->SetMarkerStyle(20);
         f2_pt2->SetMarkerColor(kRed);
         f2_pt2->SetLineColor(kRed);
         f2_pt2->SetMarkerSize(1.6);
         f1_pt2->Draw("Psame");
         f2_pt2->Draw("Psame");
         leg[c]->AddEntry(f1_pt2,"PU","p");
         leg[c]->AddEntry(f2_pt2,"No PU","p");
      }
      leg[c]->Draw("same");
      cmsPrel(13,0);
   }
   for(unsigned int f=0; f<outputFormat.size(); f++) {
      ove->SaveAs(outputDir+"/"+ss+outputFormat[f]);
   }

   //
   // create overview canvas with all ratios on same plot
   //
   TString ss2("CorrectionRatio_Overview2");
   ss2+="_"+algoNum;
   if(doflavor) ss2+="_"+flavor1+"over"+flavor2;
  // Create a legend for pt values
  TLegend * leg2 = new TLegend(0.3,0.7,0.8,0.9);
  leg2->SetTextSize(0.04);
  leg2->SetBorderSize(0);
  leg2->SetFillColor(0);
  leg2->AddEntry((TObject*)0,algNameLong,"");
  leg2->AddEntry((TObject*)0,"","");
   
   TCanvas *ove2 = new TCanvas(ss2,ss2,1200,800);
   for (int c=0;c<3;c++) {
      //Set marker colors and styles
      if(!TString(algoNum).Contains("jpt"))
         //ratioHist[c]->GetYaxis()->SetRangeUser(0.96,1.06);
         //ratioHist[c]->GetYaxis()->SetRangeUser(0.90,3.0);
         ratioHist[c]->GetYaxis()->SetRangeUser(0.80,1.5);
      else
         ratioHist[c]->GetYaxis()->SetRangeUser(0.90,1.06);
      ratioHist[c]->GetYaxis()->SetTitle("Correction Ratio");
    
      if(c == 0) {
         ratioHist[c]->SetMarkerColor(1);
         ratioHist[c]->SetLineColor(1);
      }
      else if(c == 1) {
         ratioHist[c]->SetMarkerColor(2);
         ratioHist[c]->SetLineColor(2);
      }
      else if(c == 2) {
         ratioHist[c]->SetMarkerColor(38);
         ratioHist[c]->SetLineColor(38);
      }
      else {
         ratioHist[c]->SetMarkerColor(6);
         ratioHist[c]->SetLineColor(6);
      }
      if(c == 0)
         ratioHist[c]->Draw("P");
      else
         ratioHist[c]->Draw("Psame");

      TString ptstr;
      if (PtVals[c]<0.1)
         ptstr.Form("%f",PtVals[c]);
      else
         ptstr.Form("%.1f",PtVals[c]);

      leg2->AddEntry(ratioHist[c],"Ratio="+labelNum+"/"+labelDen+" (P_{T}="+ptstr+")","p");
   }
   leg2->Draw("same");
   cmsPrel(13,0);
   for(unsigned int f=0; f<outputFormat.size(); f++) {
      ove2->SaveAs(outputDir+"/"+ss2+outputFormat[f]);
   }

   ove->Write();
   ove2->Write();
   outf->Close();
   file1->Close();
   file2->Close();
}

////////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void scanBins(double CMEnergy, double pt, TH1* histo) {
      float etaMax = TMath::ACosH(CMEnergy/2.0/pt);
      for(int ibin=1; ibin<=histo->GetNbinsX(); ibin++) {
         if(TMath::Abs(histo->GetBinCenter(ibin))>etaMax)
            histo->SetBinContent(ibin,-1.0);
      }
}

//______________________________________________________________________________
TString getAlias(TString s)
{
   if (s=="ic5calo")
      return "IC5Calo";
   else if (s=="ic5pf")
      return "IC5PF";
   else if (s=="ak5calo")
      return "AK5Calo";  
   else if (s=="ak5calol1")
      return "AK5Calol1";
   else if (s=="ak5calol1off")
      return "AK5Calol1off";
   else if (s=="ak5calol1offl2l3")
      return "AK5Calol1off";
   else if (s=="ak7calo")
      return "AK7Calo";
   else if (s=="ak7calol1")
      return "AK7Calol1";
   else if (s=="ak7calol1off")
      return "AK7Calol1off";
   else if (s=="ak5caloHLT")
      return "AK5CaloHLT";
   else if (s=="ak5caloHLTl1")
      return "AK5CaloHLTl1";
   else if (s=="ak3pf")
      return "AK3PF";
   else if (s=="ak3pfl1")
      return "AK3PFl1";
   else if (s=="ak4pf")
      return "AK4PF";
   else if (s=="ak4pfl1")
      return "AK4PFl1";
   else if (s=="ak5pf")
      return "AK5PF";
   else if (s=="ak5pfl1")
      return "AK5PFl1";
   else if (s=="ak5pfl1l2l3")
      return "AK5PFl1";
   else if (s=="ak5pfl1off")
      return "AK5PFl1off";
   else if (s=="ak6pf")
      return "AK6PF";
   else if (s=="ak6pfl1")
      return "AK6PFl1";
   else if (s=="ak7pf")
      return "AK7PF";
   else if (s=="ak7pfl1")
      return "AK7PFl1";
   else if (s=="ak7pfl1off")
      return "AK7PFl1off";
   else if (s=="ak8pf")
      return "AK8PF";
   else if (s=="ak8pfl1")
      return "AK8PFl1";
   else if (s=="ak9pf")
      return "AK9PF";
   else if (s=="ak9pfl1")
      return "AK9PFl1";
   else if (s=="ak10pf")
      return "AK10PF";
   else if (s=="ak10pfl1")
      return "AK10PFl1";
   else if (s=="ak5pfchs")
      return "AK5PFchs";
   else if (s=="ak5pfchsl1")
      return "AK5PFchsl1";
   else if (s=="ak5pfchsl1l2l3")
      return "AK5PFchsl1";
   else if (s=="ak5pfchsl1off")
      return "AK5PFchsl1off";
   else if (s=="ak7pfchs")
      return "AK7PFchs";
   else if (s=="ak7pfchsl1")
      return "AK7PFchsl1";
   else if (s=="ak7pfchsl1off")
      return "AK7PFchsl1off";
   else if (s=="ak5pfHLT")
      return "AK5PFHLT";
   else if (s=="ak5pfHLTl1")
      return "AK5PFHLTl1";
   else if (s=="ak5pfchsHLT")
      return "AK5PFchsHLT";
   else if (s=="ak5pfchsHLTl1")
      return "AK5PFchsHLTl1";
   else if (s=="ak5jpt")
      return "AK5JPT";
   else if (s=="ak5jptl1")
      return "AK5JPTl1";
   else if (s=="ak5jptl1l2l3")
      return "AK5JPTl1";
   else if (s=="ak7jpt")
      return "AK7JPT";
   else if (s=="ak7jptl1")
      return "AK7JPTl1";
   else if (s=="sc5calo")
      return "SC5Calo";
   else if (s=="sc5pf")
      return "SC5PF";
   else if (s=="sc7calo")
      return "SC5Calo";
   else if (s=="sc7pf")
      return "SC5PF";
   else if (s=="kt4calo")
      return "KT4Calo";
   else if (s=="kt4pf")
      return "KT4PF";
   else if (s=="kt6calo")
      return "KT6Calo";
   else if (s=="kt6pf")
      return "KT6PF";
   else if (s=="ak1puppi")
      return "AK1PUPPI";
   else if (s=="ak2puppi")
      return "AK2PUPPI";
   else if (s=="ak3puppi")
      return "AK3PUPPI";
   else if (s=="ak4puppi")
      return "AK4PUPPI";
   else if (s=="ak5puppi")
      return "AK5PUPPI";
   else if (s=="ak6puppi")
      return "AK6PUPPI";
   else if (s=="ak7puppi")
      return "AK7PUPPI";
   else if (s=="ak8puppi")
      return "AK8PUPPI";
   else if (s=="ak9puppi")
      return "AK9PUPPI";
   else if (s=="ak10puppi")
      return "AK10PUPPI";
   else
      return "unknown";
}

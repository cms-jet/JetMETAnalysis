///////////////////////////////////////////////////////////////////
//
// jet_draw_l5_correction_x
// ------------------------
//
//            12/23/2011 Alexx Perloff  <aperloff@physics.tamu.edu>
///////////////////////////////////////////////////////////////////

#include "JetMETAnalysis/JetAnalyzers/interface/Settings.h"
#include "JetMETAnalysis/JetUtilities/interface/Style.h"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"

#include "TROOT.h"
#include "TSystem.h"
#include "TMath.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH1D.h"
#include "TAxis.h"
#include "TF1.h"
#include "TLegend.h"
#include "TString.h"
#include "TLatex.h"
#include "TPaveText.h"

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// define/implement local functions
////////////////////////////////////////////////////////////////////////////////

/// concatinates any two streamable objects into a single string
template <class T, class U>
std::string concatString(const T& obj1, const U& obj2)
{
   std::ostringstream output;
   output << obj1 << obj2;
   return output.str();
}

/// divide one graph by another, assuming they have the same abscissa
void divTGraphErrors(TGraphErrors* num, TGraphErrors* den);

/// Returns a vector of colors to be used for the various graphs
vector<Int_t> getColors();

/// Returns a vector of marker styles to be used for the various graphs
vector<Int_t> getMarkerNumbers();

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv)
{
   gROOT->SetStyle("Plain");

   gSystem->Load("libFWCoreFWLite.so");
  
   //
   // evaluate command-line / configuration file options
   // 
   CommandLine cl;
   if (!cl.parse(argc,argv)) return 0;
  
   vector<TString> algs         = cl.getVector<TString> ("algs");
   TString         filename     = cl.getValue<TString>  ("filename");
   TString         path         = cl.getValue<TString>  ("path",                    "");
   TString         outputDir    = cl.getValue<TString>  ("outputDir",         "images");
   vector<TString> outputFormat = cl.getVector<TString> ("outputFormat", ".png:::.eps");
   vector<TString> flavors      = cl.getVector<TString> ("flavors",    "all b c g uds");
   bool            normToAll    = cl.getValue<bool>     ("normToAll",             true);
   bool            drawNormed   = cl.getValue<bool>     ("drawNormed",           false);
   bool            tdr          = cl.getValue<bool>     ("tdr",                  false);
  
   if (!cl.check()) return 0;
   cl.print();

   //
   // set style to tdr style if necessary
   //
   if (tdr) {
      setTDRStyle();
      gStyle->SetOptFit(0);
      gStyle->SetOptStat(0);
   }

   //
   // set path to input files
   //
   if(path.IsNull()) path = string (gSystem->pwd())+"/";
   if(!path.EndsWith("/")) path+="/";

   //
   // Open the file containing the l5 graphs
   //
   TFile* file = new TFile(path+filename,"READ");
   for(unsigned int a=0; a<algs.size(); a++)
   {
      cout << "Doing algorithm " << algs[a] << endl;
      TDirectoryFile *idir = (TDirectoryFile*)file->Get(algs[a]);
      vector<TCanvas*> cans;
      vector<TGraphErrors*> graphs;
      vector<TF1*> fits;
      vector<TH1D*> fitHists;
      TGraphErrors* normalizationGraph;
      TF1* normalizationFit = new TF1();
      TH1D* normalizationHist = new TH1D();
      vector<TLegend*> legs;
      vector<TPaveText*> paves;
      vector<Int_t> colors = getColors();
      vector<Int_t> markers = getMarkerNumbers();

      //Create a pave indicating the algorithm name
      TString algNameLong;
      if(TString(algs[a]).Contains("ak"))        algNameLong += "Anti-k_{T}";
      if(TString(algs[a]).Contains("2")&&!TString(algs[a]).Contains("l2"))         algNameLong += " R=0.2";
      else if(TString(algs[a]).Contains("3")&&!TString(algs[a]).Contains("l3"))    algNameLong += " R=0.3";
      else if(TString(algs[a]).Contains("4"))    algNameLong += " R=0.4";
      else if(TString(algs[a]).Contains("5"))    algNameLong += " R=0.5";
      else if(TString(algs[a]).Contains("6"))    algNameLong += " R=0.6";
      else if(TString(algs[a]).Contains("7"))    algNameLong += " R=0.7";
      else if(TString(algs[a]).Contains("8"))    algNameLong += " R=0.8";
      else if(TString(algs[a]).Contains("9"))    algNameLong += " R=0.9";
      else if(TString(algs[a]).Contains("10"))    algNameLong += " R=1.0";
      if(TString(algs[a]).Contains("pfchs"))     algNameLong += ", PF+CHS";
      //else if(TString(algs[a]).Contains("pf"))   algNameLong += ", PFlow";
      else if(TString(algs[a]).Contains("pf"))   algNameLong += ", PF";
      else if(TString(algs[a]).Contains("calo")) algNameLong += ", Calo";
      else if(TString(algs[a]).Contains("jpt"))  algNameLong += ", JPT";

      //
      //Loop over eta bins
      //
      TH1D* frame = new TH1D();
      frame->GetXaxis()->SetLimits(20.0,2000.0);
      frame->GetXaxis()->SetMoreLogLabels();
      frame->GetXaxis()->SetNoExponent();
      frame->GetYaxis()->SetRangeUser(0.94,1.06);
      frame->GetXaxis()->SetTitle("Corrected Jet p_{T} [GeV]");
      frame->GetYaxis()->SetTitle("Jet Flavor Correction");
      for(int i=0; i<NETA_Coarse; i++)
      {
         cout << "\tDoing eta bin " << eta_boundaries_coarse[i] << " to " << eta_boundaries_coarse[i+1] << endl;
         //if(i%8 == 0)
         //{
         //   cans.push_back(new TCanvas(concatString("can_",i/8).c_str(),concatString("can_",i/8).c_str(),1600,900));
         //   cans.back()->Divide(4,2);
         //}
         //cans.back()->cd((i%8)+1)->SetLogx();
         TString canName = "AbsCorVsJetPt_JetEta" + TString(eta_boundaries_coarse[i]) + 
               "to" + TString(eta_boundaries_coarse[i+1]);
         cans.push_back(tdrCanvas(canName,frame,2,11,true));
         cans.back()->GetPad(0)->SetLogx();

         //legs.push_back(new TLegend(0.8,0.15,0.97,0.4));
         //legs.back()->SetTextSize(0.05);
         //legs.back()->SetBorderSize(0);
         //legs.back()->SetFillColor(0);
         legs.push_back(tdrLeg(0.8,0.17,0.97,0.47));

         if(normToAll)
         {
            TString graphName = "AbsCorVsJetPt_JetEta" + TString(eta_boundaries_coarse[i]) + 
               "to" + TString(eta_boundaries_coarse[i+1]);
            normalizationGraph = (TGraphErrors*)idir->Get(graphName);

            if(normalizationGraph == 0) {
               cout << "\t\tWARNING::Could not open graph " << graphName << endl
                    << "\t\tGraphs will NOT be normalized to total correction" << endl;
               assert(normalizationGraph);
            }
            normalizationFit  = normalizationGraph->GetFunction("fit");
            assert(normalizationFit);
            normalizationHist = (TH1D*)normalizationFit->GetHistogram();
            assert(normalizationHist);
         }

         //
         // Loop over flavors
         //
         for(unsigned int j=0; j<flavors.size(); j++)
         {
            cout << "\tDoing flavor " << flavors[j] << endl;
            //
            // read in original L5 graphs
            //
            TString graphName = flavors[j] + "AbsCorVsJetPt_JetEta" + eta_boundaries_coarse[i] + 
               "to" + eta_boundaries_coarse[i+1];
            if(flavors[j].CompareTo("all")==0)
            {
               graphName = "AbsCorVsJetPt_JetEta" + TString(eta_boundaries_coarse[i]) + "to" + 
                  TString(eta_boundaries_coarse[i+1]);
            }
            if(drawNormed)
            {
               graphName += "_norm";
            }

            graphs.push_back((TGraphErrors*)idir->Get(graphName));
            if(graphs.back() == 0)
            {
               cout << "\t\tWARNING::Could not open graph " << graphName << endl
                    << "\t\tSkipping this flavor" << endl;
               continue;
            }
            if(normToAll)
            {
               assert(normalizationFit);
               fits.push_back(graphs.back()->GetFunction("fit"));
               fits.back()->SetRange(normalizationFit->GetXmin(),normalizationFit->GetXmax());
               fitHists.push_back((TH1D*)fits.back()->GetHistogram());
            }
         
            //
            // draw and format graphs/hists
            //
            if(normToAll)
            {
               assert(normalizationHist);
               //
               // divide hist by normalization hist
               //
               /*
               cout << "fitHist bins = " << fitHists.back()->GetNbinsX() << endl
                    << "fitHist low = " << fitHists.back()->GetXaxis()->GetXmin() << endl
                    << "fitHist high = " << fitHists.back()->GetXaxis()->GetXmax() << endl
                    << "normalizationHist bins = " << normalizationHist->GetNbinsX() << endl
                    << "normalizationHist low = " << normalizationHist->GetXaxis()->GetXmin() << endl
                    << "normalizationHist high = " << normalizationHist->GetXaxis()->GetXmax() << endl;
               */
               fitHists.back()->Divide(normalizationHist);
               if(j%flavors.size() == 0)
               {
                  fitHists.back()->GetYaxis()->SetRangeUser(0.94,1.1);
                  fitHists.back()->SetMarkerStyle(7);
                  fitHists.back()->GetXaxis()->SetTitle("p_{T}^{Corr l1l2l3} (GeV)");
                  fitHists.back()->GetXaxis()->SetTitleOffset(1.1);
                  fitHists.back()->GetXaxis()->SetLabelSize(0.035);
                  fitHists.back()->GetYaxis()->SetTitle("L5 Correction");
                  fitHists.back()->GetYaxis()->SetTitleOffset(1.3);
                  fitHists.back()->GetYaxis()->SetLabelSize(0.035);
                  //fitHists.back()->Draw("P");
                  tdrDraw(fitHists.back(),"P",7,fitHists.back()->GetMarkerColor(),kSolid,fitHists.back()->GetLineColor(),kNone,0);
               }
               else
               {
                  fitHists.back()->SetMarkerStyle(7);
                  fitHists.back()->SetMarkerColor(j+1);
                  fitHists.back()->SetLineColor(j+1);
                  //fitHists.back()->Draw("P sames");
                  tdrDraw(fitHists.back(),"P",7,j+1,kSolid,j+1,kNone,0);
               }
               fitHists.back()->SetTitle(graphName);
               legs.back()->AddEntry(fitHists.back(),flavors[j],"lp");
            }
            else
            {
               if(j%flavors.size() == 0)
               {
                  if (tdr) {
                     //graphs.back()->Draw("AP");
                     //tdrDraw(graphs.back(),"P",20,graphs.back()->GetMarkerColor(),kSolid,graphs.back()->GetLineColor(),kNone,0);
                     tdrDraw(graphs.back(),"P",markers[0],colors[0],kSolid,colors[0],kNone,0);
                     graphs.back()->GetXaxis()->SetTitle("Corrected Jet p_{T} (GeV)");
                     graphs.back()->GetXaxis()->SetTitleOffset(1.1);
                     graphs.back()->GetXaxis()->SetTitleSize(0.055);
                     graphs.back()->GetXaxis()->SetLabelSize(0.045);
                     graphs.back()->GetXaxis()->SetNoExponent();
                     graphs.back()->GetXaxis()->SetMoreLogLabels();
                     //graphs.back()->GetXaxis()->SetRangeUser(20,2500);
                     graphs.back()->GetXaxis()->SetLimits(20.,2500.);
                     graphs.back()->GetYaxis()->SetTitle("Jet Flavor Correction");
                     graphs.back()->GetYaxis()->SetTitleOffset(1.3);
                     graphs.back()->GetYaxis()->SetTitleSize(0.055);
                     graphs.back()->GetYaxis()->SetLabelSize(0.045);
                     graphs.back()->GetYaxis()->SetRangeUser(0.95,1.05);
                     //graphs.back()->SetMarkerStyle(7);
                     //graphs.back()->GetFunction("fit")->SetLineColor(j+1);                     
                     graphs.back()->GetFunction("fit")->SetLineColor(colors[j]);
                  }
                  else {
                     //cans.back()->cd((i%8)+1)->SetLogx();
                     graphs.back()->GetXaxis()->SetTitle("p_{T}^{Corr l1l2l3} (GeV)");
                     graphs.back()->GetXaxis()->SetTitleOffset(1.1);
                     graphs.back()->GetXaxis()->SetLabelSize(0.035);
                     graphs.back()->GetYaxis()->SetTitle("L5 Correction");
                     graphs.back()->GetYaxis()->SetTitleOffset(1.3);
                     graphs.back()->GetYaxis()->SetLabelSize(0.035);
                     graphs.back()->GetYaxis()->SetRangeUser(0.9,1.1);
                     //graphs.back()->SetMarkerStyle(7);
                     graphs.back()->GetFunction("fit")->SetLineColor(j+1);
                     //graphs.back()->Draw("AP");
                     tdrDraw(graphs.back(),"P",20,graphs.back()->GetMarkerColor(),kSolid,j+1,kNone,0);
                  }
               }
               else
               {
                  //graphs.back()->SetMarkerStyle(7);
                  //graphs.back()->GetFunction("fit")->SetLineColor(j+1);
                  graphs.back()->GetFunction("fit")->SetLineColor(colors[j]);
                  //graphs.back()->Draw("P SAME");
                  //tdrDraw(graphs.back(),"P",20,j+1,kSolid,j+1,kNone,0);
                  tdrDraw(graphs.back(),"P",markers[j],colors[j],kSolid,colors[j],kNone,0);
                  /*
                  if (tdr && j+1==3)
                     graphs.back()->SetMarkerColor(kGreen+2);
                  else if (tdr && j+1==5)
                     graphs.back()->SetMarkerColor(kOrange+2);
                  graphs.back()->SetMarkerColor(j+1);
                  */
               }
               graphs.back()->SetTitle(graphName);
               legs.back()->AddEntry(graphs.back(),flavors[j],"lep");
            }

         }//for(unsigned int j=0; j<flavors.size() ;j++)
         //legs.back()->SetFillColor(0);
         legs.back()->Draw("same");
         if (tdr) {
            //paves.push_back(new TPaveText(0.50,0.8,0.95,0.92,"NDC"));
            //paves.push_back(new TPaveText(0.23,0.72,0.68,0.9,"NDC"));
            paves.push_back(tdrText(0.5,0.75,0.93,1-gPad->GetTopMargin()-0.045*(1-gPad->GetTopMargin()-gPad->GetBottomMargin())-0.01,31));
            paves.back()->AddText("QCD Monte Carlo");
            paves.back()->AddText(algNameLong);
            TString eta = Form("%s < #eta < %s",eta_boundaries_coarse[i],eta_boundaries_coarse[i+1]);
            //paves.back()->AddText("|#eta| < 1.3");
            paves.back()->AddText(eta);
            //paves.back()->SetFillColor(0);
            //paves.back()->SetBorderSize(0);
            //paves.back()->SetTextFont(42);
            //paves.back()->SetTextSize(0.05);
            //paves.back()->SetTextSize(0.04);
            paves.back()->Draw("same");
            //cmsPrelim();
         }
      }//for(int i=0; i<7; i++)

      //
      // Open/create the output directory and file
      //
      if(!gSystem->OpenDirectory(outputDir)) gSystem->mkdir(outputDir);
      if(!outputDir.EndsWith("/")) outputDir+="/";
      TString ofname = outputDir+"L5"+"AbsCorGraphs_"+algs[a]+".root";
      TFile* outf = new TFile(ofname,"RECREATE");

      gStyle->SetOptFit(0);
      //
      // save output
      //
      for(unsigned int i=0; i<cans.size(); i++)
      {
         string canName = cans[i]->GetName();
         for(unsigned int of=0; of<outputFormat.size(); of++) {
            cans[i]->SaveAs(outputDir+"L5"+"AbsCorGraphs_"+algs[a]+"_"+canName+outputFormat[of]);
         }
         cans[i]->Write();
      }

      outf->Close();
   }//for(unsigned int a=0; a<algs.size(); a++)
}

////////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void divTGraphErrors(TGraphErrors* num, TGraphErrors* den)
{
   for(int k=0; k<num->GetN(); k++)
   {
      double x = 0;
      double y = 0;
      double ex = 0;
      double ey = 0;
      double normx = 0;
      double normy = 0;
      //double normex = 0;
      double normey = 0;
      
      num->GetPoint(k,x,y);
      ex = num->GetErrorX(k);
      ey = num->GetErrorY(k);
      den->GetPoint(k,normx,normy);
      //normex = den->GetErrorX(k);
      normey = den->GetErrorY(k);
      
      if(x!=normx)
      {
         cout << "\tdivTGraphErrors::WARNING::The two graphs do not have the same abscissa at point " << k << endl;
      }

      num->SetPoint(k,x,y/normy);
      num->SetPointError(k,ex,TMath::Sqrt((TMath::Power((1/normy),2)*TMath::Power(ey,2))+
                                          (TMath::Power((y/TMath::Power(normy,2)),2)*TMath::Power(normey,2))));
   }
}

//______________________________________________________________________________
vector<Int_t> getColors() {
  vector<Int_t> ret;
  ret.push_back(kBlack); //all
  ret.push_back(kMagenta); //ud
  ret.push_back(kOrange+1); //s
  ret.push_back(kGreen); //c
  ret.push_back(kRed); //b
  ret.push_back(kBlue); //g
  ret.push_back(kAzure+1);
  ret.push_back(kCyan);
  ret.push_back(kViolet+1);
  ret.push_back(kGray+2);
  return ret;
}

//______________________________________________________________________________
vector<Int_t> getMarkerNumbers() {
  vector<Int_t> ret;
  ret.push_back(20); //kFullCircle
  ret.push_back(24); //kOpenCircle
  ret.push_back(21); //kFullSquare
  ret.push_back(25); //kOpenSquare
  ret.push_back(22); //kFullTriangleUp
  ret.push_back(26); //kOpenTriangleUp
  ret.push_back(23); //kFullTriangleDown
  ret.push_back(32); //kOpenTriangleDown
  ret.push_back(33); //kFullDiamond
  ret.push_back(27); //kOpenDiamond
  ret.push_back(34); //kFullCross
  ret.push_back(28); //kOpenCross
  return ret;
}



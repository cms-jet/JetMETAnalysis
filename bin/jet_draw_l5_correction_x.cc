///////////////////////////////////////////////////////////////////
//
// jet_draw_l5_correction_x
// ------------------------
//
//            12/23/2011 Alexx Perloff  <aperloff@physics.tamu.edu>
///////////////////////////////////////////////////////////////////

#include "JetMETAnalysis/JetAnalyzers/interface/Settings.h"
#include "JetMETAnalysis/JetAnalyzers/bin/tdrstyle.C"
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

/// sets the style for the canvas if the plots need to be in tdr style
void setStyle(bool fitStat = false, bool name = false);

///CMS Preliminary label;
void cmsPrelim(double intLUMI = 0);

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
   TString         path         = cl.getValue<TString>  ("path",                 "");
   TString         outputDir    = cl.getValue<TString>  ("outputDir",      "images");
   TString         outputFormat = cl.getValue<TString>  ("outputFormat",     ".png");
   vector<TString> flavors      = cl.getVector<TString> ("flavors", "all b c g uds");
   bool            normToAll    = cl.getValue<bool>     ("normToAll",          true);
   bool            drawNormed   = cl.getValue<bool>     ("drawNormed",        false);
   bool            tdr          = cl.getValue<bool>     ("tdr",               false);
  
   if (!cl.check()) return 0;
   cl.print();

   //
   // set style to tdr style if necessary
   //
   if (tdr) {
      setStyle();
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
      TDirectoryFile *idir = (TDirectoryFile*)file->Get(algs[a]);
      vector<TCanvas*> cans;
      vector<TGraphErrors*> graphs;
      vector<TF1*> fits;
      vector<TH1D*> fitHists;
      TGraphErrors* normalizationGraph;
      TF1* normalizationFit;
      TH1D* normalizationHist;
      vector<TLegend*> legs;
      vector<TPaveText*> paves;

      //
      //Loop over eta bins
      //
      for(int i=0; i<NETA_Coarse; i++)
      {
         cout << "\tDoing eta bin " << eta_boundaries_coarse[i] << " to " << eta_boundaries_coarse[i+1] << endl;
         if(i%6 == 0)
         {
            cans.push_back(new TCanvas(concatString("can_",i/6).c_str(),concatString("can_",i/6).c_str(),1200,900));
            cans.back()->Divide(3,2);
         }
         cans.back()->cd((i%6)+1)->SetLogx();

         legs.push_back(new TLegend(0.70,0.7,0.88,0.88));
         legs.back()->SetTextSize(0.05);
         legs.back()->SetBorderSize(0);
         legs.back()->SetFillColor(0);

         if(normToAll)
         {
            TString graphName = "AbsCorVsJetPt_JetEta" + TString(eta_boundaries_coarse[i]) + 
               "to" + TString(eta_boundaries_coarse[i+1]);
            normalizationGraph = (TGraphErrors*)idir->Get(graphName);
            if(normalizationGraph == 0)
               cout << "\t\tWARNING::Could not open graph " << graphName << endl
                    << "\t\tGraphs will NOT be normalized to total correction" << endl;
            else
            {
               normalizationFit = normalizationGraph->GetFunction("fit");
               normalizationHist = (TH1D*)normalizationFit->GetHistogram();
            }
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
               fits.push_back(graphs.back()->GetFunction("fit"));
               fits.back()->SetRange(normalizationFit->GetXmin(),normalizationFit->GetXmax());
               fitHists.push_back((TH1D*)fits.back()->GetHistogram());
            }
         
            //
            // draw and format graphs/hists
            //
            if(normToAll && normalizationHist!=0)
            {
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
                  fitHists.back()->Draw("P");
               }
               else
               {
                  fitHists.back()->SetMarkerStyle(7);
                  fitHists.back()->SetMarkerColor(j+1);
                  fitHists.back()->SetLineColor(j+1);
                  fitHists.back()->Draw("P sames");
               }
               fitHists.back()->SetTitle(graphName);
               legs.back()->AddEntry(fitHists.back(),flavors[j],"lp");
            }
            else
            {
               if(j%flavors.size() == 0)
               {
                  if (tdr) {
                     graphs.back()->GetXaxis()->SetTitle("Corrected Jet p_{T} (GeV)");
                     graphs.back()->GetXaxis()->SetTitleOffset(1.1);
                     graphs.back()->GetXaxis()->SetTitleSize(0.055);
                     graphs.back()->GetXaxis()->SetLabelSize(0.045);
                     graphs.back()->GetXaxis()->SetNoExponent();
                     graphs.back()->GetXaxis()->SetMoreLogLabels();
                     graphs.back()->GetYaxis()->SetTitle("Jet Flavor Correction");
                     graphs.back()->GetYaxis()->SetTitleOffset(1.3);
                     graphs.back()->GetYaxis()->SetTitleSize(0.055);
                     graphs.back()->GetYaxis()->SetLabelSize(0.045);
                     graphs.back()->GetYaxis()->SetRangeUser(0.95,1.05);
                     graphs.back()->SetMarkerStyle(7);
                     graphs.back()->Draw("AP");
                  }
                  else {
                     //cans.back()->cd((i%6)+1)->SetLogx();
                     graphs.back()->GetXaxis()->SetTitle("p_{T}^{Corr l1l2l3} (GeV)");
                     graphs.back()->GetXaxis()->SetTitleOffset(1.1);
                     graphs.back()->GetXaxis()->SetLabelSize(0.035);
                     graphs.back()->GetYaxis()->SetTitle("L5 Correction");
                     graphs.back()->GetYaxis()->SetTitleOffset(1.3);
                     graphs.back()->GetYaxis()->SetLabelSize(0.035);
                     graphs.back()->GetYaxis()->SetRangeUser(0.94,1.1);
                     graphs.back()->SetMarkerStyle(7);
                     graphs.back()->Draw("AP");
                  }
               }
               else
               {
                  graphs.back()->SetMarkerStyle(7);
                  if (tdr && j+1==3)
                     graphs.back()->SetMarkerColor(kGreen+2);
                  else if (tdr && j+1==5)
                     graphs.back()->SetMarkerColor(kYellow+2);
                  graphs.back()->SetMarkerColor(j+1);
                  graphs.back()->GetFunction("fit")->SetLineColor(j+1);
                  graphs.back()->Draw("P");
               }
               graphs.back()->SetTitle(graphName);
               legs.back()->AddEntry(graphs.back(),flavors[j],"lep");
            }

         }//for(unsigned int j=0; j<flavors.size() ;j++)
         legs.back()->SetFillColor(0);
         legs.back()->Draw();
         if (tdr) {
            paves.push_back(new TPaveText(0.2,0.78,0.6,0.88,"NDC"));
            paves.back()->AddText("QCD Monte Carlo");
            paves.back()->AddText("|#eta| < 1.3");
            paves.back()->SetFillColor(0);
            paves.back()->SetBorderSize(0);
            paves.back()->SetTextFont(42);
            paves.back()->SetTextSize(0.05);
            paves.back()->Draw("EP");
            cmsPrelim();
         }
      }//for(int i=0; i<7; i++)

      //
      // Open/create the output directory and file
      //
      if(!gSystem->OpenDirectory(outputDir)) gSystem->mkdir(outputDir);
      if(!outputDir.EndsWith("/")) outputDir+="/";
      TString ofname = outputDir+"L5"+"AbsCorGraphs_"+algs[a]+".root";
      TFile* outf = new TFile(ofname,"RECREATE");

      //
      // save output
      //
      for(unsigned int i=0; i<cans.size(); i++)
      {
         string canName = cans[i]->GetName();
         cans[i]->SaveAs(outputDir+"L5"+"AbsCorGraphs_"+algs[a]+"_"+canName+outputFormat);
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
      double normex = 0;
      double normey = 0;
      
      num->GetPoint(k,x,y);
      ex = num->GetErrorX(k);
      ey = num->GetErrorY(k);
      den->GetPoint(k,normx,normy);
      normex = den->GetErrorX(k);
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
void setStyle(bool fitStat, bool name) {
   TStyle* tdrStyle = getTDRStyle();
   tdrStyle->SetPadRightMargin(0.08);
   tdrStyle->SetLegendBorderSize(0);
   //tdrStyle->SetMarkerStyle(7);
   tdrStyle->SetEndErrorSize(0);
   if(fitStat)
   {
      if(name) tdrStyle->SetOptStat("neMR");
      else tdrStyle->SetOptStat("eMR");
      tdrStyle->SetOptFit(2211);
   }
   else
   {
      tdrStyle->SetOptStat(0);
      tdrStyle->SetOptFit(0);
   }
   tdrStyle->SetStatColor(0);
   tdrStyle->SetPalette(1);
   tdrStyle->SetTitleColor(0,"c");
   tdrStyle->SetTitleFillColor(0);
   gROOT->SetStyle(tdrStyle->GetName());
}

//______________________________________________________________________________
void cmsPrelim(double intLUMI)
{
   const float LUMINOSITY = intLUMI;
  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.05);

  latex.SetTextAlign(31); // align right
  latex.DrawLatex(0.93,0.96,"#sqrt{s} = 7 TeV");
  if (LUMINOSITY > 0.) {
    latex.SetTextAlign(31); // align right
    //latex.DrawLatex(0.82,0.7,Form("#int #font[12]{L} dt = %d pb^{-1}", (int) LUMINOSITY)); //Original
    latex.DrawLatex(0.65,0.85,Form("#int #font[12]{L} dt = %d pb^{-1}", (int) LUMINOSITY)); //29/07/2011
  }
  latex.SetTextAlign(11); // align left
  latex.DrawLatex(0.16,0.96,"CMS preliminary");// 2012");
}

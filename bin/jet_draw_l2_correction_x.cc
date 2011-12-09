///////////////////////////////////////////////////////////////////
//
// jet_draw_l2_correction_x
// ------------------------
//
//            12/09/2011 Alexx Perloff  <aperloff@physics.tamu.edu>
///////////////////////////////////////////////////////////////////

#include "JetMETAnalysis/JetAnalyzers/interface/Settings.h"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"

#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TLegend.h"
#include "TString.h"

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
  
  TString     alg          = cl.getValue<TString> ("alg");
  TString     filename     = cl.getValue<TString> ("filename");
  TString     path         = cl.getValue<TString> ("path",             "");
  TString     outputDir    = cl.getValue<TString> ("outputDir",  "images");
  TString     outputFormat = cl.getValue<TString> ("outputFormat", ".png");
  TString     absOrRelCor  = cl.getValue<TString> ("absOrRelCor",   "Abs");
  bool        sameCanvas   = cl.getValue<bool>    ("sameCanvas",     true);
  
  if (!cl.check()) return 0;
  cl.print();

  //
  // set path to input files
  //
  if(path.IsNull()) path = string (gSystem->pwd())+"/";
  if(!path.EndsWith("/")) path+="/";

  //
  // catch incorrect graph names
  //
  if(absOrRelCor.CompareTo("Abs")!=0 && absOrRelCor.CompareTo("Rel")!=0)
    {
      cout << "The variable absOrRelCor did not equal either \"Abs\" or \"Rel\"." << endl 
           << "The variable must be equal to one of those choices. Please try again." << endl 
           << "The program will now exit." << endl;
      return 0;
    }

  //
  // Open the file containing the l2 graphs
  //
  TFile* file = new TFile(path+filename,"READ");
  TDirectoryFile *idir = (TDirectoryFile*)file->Get(alg);
  vector<TCanvas*> cans;
  vector<TGraphErrors*> graphs;
  TLegend* leg = new TLegend(0.65,0.6,1.0,0.8);
  int overviewList[12] = {0,6,11,18,29,40,41,52,63,70,75,81};

  if(sameCanvas) cans.push_back(new TCanvas("can","can",1200,800));
  TCanvas *ove = new TCanvas("can_overview","can_overview",1200,800);
  ove->Divide(4,3);

  for(int i=0; i<NETA; i++)
    {
      if(!sameCanvas && i%12 == 0)
        {
          cans.push_back(new TCanvas(concatString("can_",i/12).c_str(),concatString("can_",i/12).c_str(),800,800));
          cans.back()->Divide(4,3);
        }

      //
      // read in original L2 graphs
      //
      TString graphName = absOrRelCor+"CorVsJetPt_JetEta" + eta_boundaries[i] + "to" + eta_boundaries[i+1];
      graphs.push_back((TGraphErrors*)idir->Get(graphName));
      if(graphs.back() == 0) continue;

      //
      // draw and format graphs
      //
      if(!sameCanvas)
        {
          cans.back()->cd((i%12)+1)->SetLogx();
          graphs.back()->Draw("AP");
        }
      else if(sameCanvas && i==0)
        {
          cans.back()->cd()->SetLogx();
          graphs.back()->Draw("AP");
        }
      else
        {
          graphs.back()->SetMarkerColor(i+1);
          graphs.back()->GetFunction("fit")->SetLineColor(i+1);
          graphs.back()->Draw("P");
        }

      //
      // draw overview canvas (specific graphs)
      //
      for(unsigned int j=0; j<12; j++)
        {
          if(i==overviewList[j])
            {
              ove->cd(j+1)->SetLogx();
              graphs.back()->Draw("AP");
            } 
        }

      graphs.back()->SetTitle(graphName);
      leg->AddEntry(graphs.back(),graphs.back()->GetName(),"le");
    }

  if(sameCanvas)
    {
      leg->SetFillColor(0);
      leg->Draw();
    }

  //
  // Open/create the output directory and file
  //
  if(!gSystem->OpenDirectory(outputDir)) gSystem->mkdir(outputDir);
  if(!outputDir.EndsWith("/")) outputDir+="/";
  TString ofname = outputDir+"L2"+absOrRelCor+"Graphs_"+alg+".root";
  TFile* outf = new TFile(ofname,"RECREATE");

  //
  // save output
  //
  for(unsigned int i=0; i<cans.size(); i++)
    {
      string canName = cans[i]->GetName();
      cans[i]->SaveAs(outputDir+"L2"+absOrRelCor+"CorGraphs_"+alg+"_"+canName+outputFormat);
      cans[i]->Write();
    }
  ove->SaveAs(outputDir+"L2"+absOrRelCor+"CorGraphs_"+alg+"_Overview"+outputFormat);
  ove->Write();

  outf->Close();
}

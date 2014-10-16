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
#include "TMultiGraph.h"
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
  if(absOrRelCor.CompareTo("Abs")!=0 && absOrRelCor.CompareTo("Rel")!=0) {
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
  vector<TGraphErrors*> graphs;
  vector<TMultiGraph*> mgs;
  vector<TLegend*> legs;
  TCanvas* can = new TCanvas("can","can",1200,800);
  can->Divide(4,3);
  int ican = 1;
  int ngpc = 7;
  TString axisDrawn = "";

  for(int i=0; i<NETA; i++) {
    cout << "Doing eta " << i << " ... "<< endl;
    if(i%ngpc == 0) {
      cout << "\tDoing canvas " << ican << " ... " << endl;
      can->cd(ican++)->SetLogx();
      legs.push_back(new TLegend(0.40,0.15,0.9,0.45));
      legs.back()->SetFillColor(0);
      legs.back()->SetFillStyle(0);
      legs.back()->SetBorderSize(0);
      //legs.back()->AddEntry((TObject*)0,Form("%sCorVsJetPt_JetEta",absOrRelCor.Data()),"");
      legs.back()->SetHeader(Form("%sCorVsJetPt_JetEta",absOrRelCor.Data()));
      legs.back()->SetNColumns(2);
      mgs.push_back(new TMultiGraph(Form("mg%i",ican-1),Form("mg%i",ican-1)));
    }

    //
    // read in original L2 graphs
    //
    TString graphName = absOrRelCor+"CorVsJetPt_JetEta" + eta_boundaries[i] + "to" + eta_boundaries[i+1];
    graphs.push_back((TGraphErrors*)idir->Get(graphName));
    if(graphs.back() == 0 && i%ngpc==0) axisDrawn = "A";
    else axisDrawn = "";
    if(graphs.back() == 0) {
      cout << "WARNING::Could not find graph " << graphName << endl;
    }
    else {
      //
      // draw and format graphs
      //
      graphs.back()->SetMarkerColor(i%ngpc+1);
      graphs.back()->SetLineColor(i%ngpc+1);
      graphs.back()->GetFunction("fit")->SetLineColor(i%ngpc+1);
      graphs.back()->SetTitle(graphName);
      mgs.back()->Add(graphs.back());
      legs.back()->AddEntry(graphs.back(),Form("%sto%s",eta_boundaries[i],eta_boundaries[i+1]),"l");
    }

    if((i+1)%ngpc == 0 || i==NETA-1) {
      mgs.back()->Draw("AP");
      legs.back()->Draw("same");
    }
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
  string canName = can->GetName();
  can->SaveAs(outputDir+"L2"+absOrRelCor+"CorGraphs_"+alg+outputFormat);
  can->Write();
  outf->Close();
}

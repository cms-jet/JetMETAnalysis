///////////////////////////////////////////////////////////////////
//
// jet_combine_corrections_x
// -------------------------
//
//            01/20/2014 Alexx Perloff aperloff@physics.tamu.edu
//
// This executable reads the relevant root files which have drawn
// either the corrections vs eta or the corrections vs pt. It will
// then loop over multiple files/algorithms and draw them to the
// same canvas.
///////////////////////////////////////////////////////////////////

#include "JetMETAnalysis/JetAnalyzers/interface/Settings.h"
#include "JetMETAnalysis/JetAnalyzers/interface/Style.h"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"

#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TObject.h"
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
#include "TLegend.h"
#include "TLatex.h"
#include "TMath.h"

#include <fstream>
#include <string>
#include <cmath>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////
FactorizedJetCorrector * getFactorizedCorrector(TString algo, CommandLine & cl, TString & );

void analyzeAlgo(TString algo, CommandLine & cl);

TCanvas * getCorrectionVsEtaCanvas(TString algo, FactorizedJetCorrector * jetCorr, TString suffix);

TCanvas * getCorrectionVsEtaCanvasTDR(TString algo, FactorizedJetCorrector * jetCorr, TString suffix);

TCanvas * getCorrectionVsPtCanvas(TString algo, FactorizedJetCorrector * jetCorr, TString suffix);

string getAlias(TString s);

///CMS Preliminary label;
void cmsPrelim(double intLUMI = 0);

//______________________________________________________________________________
int main(int argc,char**argv)
{
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);

  gSystem->Load("libFWCoreFWLite.so");

  // evaluate command-line / configuration file options
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;

  vector<TString> algs         = cl.getVector<TString> ("algs");
  TString         path         = cl.getValue<TString>  ("path"         , ""      );
  string          outputDir    = cl.getValue<string>   ("outputDir"    , "./");
  if (algs.size()==0){
    cout<<"ERROR you must use the flag -algs to select one or more algorithms!"<<endl;
  }
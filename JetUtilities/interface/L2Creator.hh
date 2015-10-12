////////////////////////////////////////////////////////////////////////////////
//
// L2Creator
// ---------
//
//            08/16/2008 Kostas Kousouris                    <kkousour@fnal.gov>
//                       Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
//            08/21/2015 Alexx Perloff           <alexx.stephen.perloff@cern.ch>
////////////////////////////////////////////////////////////////////////////////
#ifndef L2CREATOR_HH
#define L2CREATOR_HH

#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/ObjectLoader.h"
#include "JetMETAnalysis/JetUtilities/interface/RootStyle.h"
#include "JetMETAnalysis/JetUtilities/interface/JetInfo.hh"

#include "TApplication.h"
#include "TFile.h"
#include "TDirectoryFile.h"
#include "TKey.h"
#include "TObjArray.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TString.h"
#include "TLatex.h"
#include "TError.h"
#include "TSpectrum.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <locale>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// define L2Creator class
////////////////////////////////////////////////////////////////////////////////
class L2Creator {
public:
  L2Creator();
  L2Creator(CommandLine& cl);

  void resetForNextAlgorithm();
  void openOutputFile();
  void openInputFile();
  void openL3File();
  void loopOverDirectories();
  void loopOverAlgorithms();
  bool getL3Rsp();
  void loopOverEtaBins();
  void setAndFitFLogAndFGaus(TGraphErrors* gabscor, TF1* flog, TF1* fgaus, double xmin);
  TString getOfflinePFFunction();
  void setOfflinePFParameters(TGraphErrors* gabscor, TF1* fabscor, double xmin);
  Double_t findPeak(TGraphErrors* gabscor, int ipeak, int npeaks, int res, bool verbose = false);
  void doRelCorFits();
  /// check if a vector of strings contains a certain element
  bool contains(const vector<string>& collection,const string& element);
  /// this method performs consecutive fittings (up to maxFitIter) but uses the
  /// parameters from the one with the lowest chi2 that has not failed.
  void perform_smart_fit(TGraphErrors * gabscor, TF1 * fabscor, int maxFitIter = 30);
  void writeTextFileForCurrentAlgorithm();
  void closeFiles();

private:
  string                input, era, l3input;
  TString               output, outputDir, l2calofit, l2pffit;
  vector<string>        formats, algs;
  bool                  l2l3, mpv, delphes;
  int                   maxFitIter;
  TFile*                ofile;
  TFile*                ifile;
  TFile*                l3file;
  TDirectoryFile*       l3dir;
  TDirectoryFile*       odir;
  TGraphErrors*         gl3rsp;
  TF1* 			            fl3rsp;
  JetInfo*              ji;
  ObjectLoader<TH1F>    hl_rsp;
  ObjectLoader<TH1F>    hl_refpt;
  ObjectLoader<TH1F>    hl_jetpt;
  vector<TGraphErrors*> vabsrsp_eta;
  vector<TGraphErrors*> vabscor_eta;
  vector<TGraph*>       vrelcor_eta;
};

#endif
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

#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"

#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/ObjectLoader.h"
#include "JetMETAnalysis/JetUtilities/interface/RootStyle.h"
#include "JetMETAnalysis/JetUtilities/interface/Style.h"
#include "JetMETAnalysis/JetUtilities/interface/JetInfo.hh"
#include "JetMETAnalysis/JetUtilities/interface/HistogramUtilities.hh"
#include "JetMETAnalysis/JetUtilities/interface/PiecewiseSpline.hh"

#include "TApplication.h"
#include "TFile.h"
#include "TDirectoryFile.h"
#include "TKey.h"
#include "TObjArray.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TPaveStats.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TString.h"
#include "TLatex.h"
#include "TError.h"
#include "TSpectrum.h"
#include "TSpline.h"

//https://www.gnu.org/software/gsl/manual/html_node/1D-Interpolation-Types.html#g_t1D-Interpolation-Types
//https://github.com/ampl/gsl/tree/master/interpolation
//https://github.com/ampl/gsl/blob/master/interpolation/cspline.c
//https://github.com/ampl/gsl/blob/master/interpolation/akima.c
//https://github.com/ampl/gsl/blob/master/interpolation/steffen.c
#include <gsl/gsl_math.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_interp.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <locale>
#include <algorithm>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// define L2Creator class
////////////////////////////////////////////////////////////////////////////////
class L2Creator {
public:
    L2Creator();
    L2Creator(CommandLine& cl);

    bool     checkFormulaEvaluator();
    void     closeFiles();
    /// check if a vector of strings contains a certain element
    bool     contains(const vector<string>& collection,const string& element);
    void     doRelCorFits();
    /// find the xvalue for the next highest or lowest point in a graph
    double   findNext(double xvalue,TGraph* g, bool highest, bool debug = false);
    Double_t findPeak(TGraphErrors* gabscor, int ipeak, int npeaks, int res, bool verbose = false);
    bool     getL3Rsp();
    TString  getOfflinePFFunction();
    void     loopOverAlgorithms(string makeCanvasVariable = "");
    void     loopOverDirectories();
    void     loopOverEtaBins();
    void     makeCanvas(string makeCanvasVariable);
    void     openInputFile();
    void     openL3File();
    void     openOutputFile();
    /// this method performs consecutive fittings (up to maxFitIter) but uses the
    /// parameters from the one with the lowest chi2 that has not failed.
    void     perform_smart_fit(TGraphErrors * gabscor, TF1 * fabscor, int maxFitIter = 30);
    void     resetForNextAlgorithm();
    void     setAndFitFLogAndFGaus(TGraphErrors* gabscor, TF1* flog, TF1* fgaus, double xmin);
    void     setOfflinePFParameters(TGraphErrors* gabscor, TF1* fabscor, double xmin, double xmax);
    void     writeTextFileForCurrentAlgorithm();
    void     writeTextFileForCurrentAlgorithm_spline();

private:
    string                    input, era, l3input, histMet;
    TString                   output, outputDir, l2calofit, l2pffit;
    vector<string>            formats, algs;
    bool                      l2l3, delphes, ptclipfit;
    int                       maxFitIter, statTh;
    HistUtil::HistogramMetric histogramMetric;
    TFile*                    ofile;
    TFile*                    ifile;
    TFile*                    l3file;
    TDirectoryFile*           l3dir;
    TDirectoryFile*           odir;
    TGraphErrors*             gl3rsp;
    TF1* 			          fl3rsp;
    JetInfo*                  ji;
    ObjectLoader<TH1F>        hl_rsp;
    ObjectLoader<TH1F>        hl_refpt;
    ObjectLoader<TH1F>        hl_jetpt;
    vector<TGraphErrors*>     vabsrsp_eta;
    vector<TGraphErrors*>     vabscor_eta;
    vector<TGraph*>           vrelcor_eta;
    vector<PiecewiseSpline*>  vabscor_eta_spline;
    float                     ptclip;
};

#endif

////////////////////////////////////////////////////////////////////////////////
//
// ClosureMaker
// ------------
//
//            09/04/2015 Alexx Perloff           <alexx.stephen.perloff@cern.ch>
////////////////////////////////////////////////////////////////////////////////
#ifndef CLOSUREMAKER_HH
#define CLOSUREMAKER_HH

#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/ObjectLoader.h"
#include "JetMETAnalysis/JetUtilities/interface/RootStyle.h"
#include "JetMETAnalysis/JetUtilities/interface/JetInfo.hh"
#include "JetMETAnalysis/JetUtilities/interface/Variables.hh"
#include "JetMETAnalysis/JetUtilities/interface/Exceptions.hh"
#include "JetMETAnalysis/JetUtilities/interface/Style.h"
#include "JetMETAnalysis/JetUtilities/interface/HistogramUtilities.hh"

#include "TROOT.h"
#include "TSystem.h"
#include "TApplication.h"
#include "TMath.h"
#include "TFile.h"
#include "TDirectoryFile.h"
#include "TKey.h"
#include "TObjArray.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TString.h"
#include "TLatex.h"
#include "TPaveText.h"
#include "TError.h"
#include "TSpectrum.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <locale>
#include <stdexcept>
#include <exception>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// define constants
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// define ClosureMaker class
////////////////////////////////////////////////////////////////////////////////
class ClosureMaker {
public:
	ClosureMaker();
	ClosureMaker(CommandLine& cl);

	//
	// member functions
	//
	void 				openInputFile();
	void 				getHistograms(TDirectoryFile* idir);
	void 				openOutputFile();
	void 				closeFiles();
	void 				makeLines();
	void 				loopOverDirectories();
	void 				loopOverAlgorithms();
	void 				resetForNextAlgorithm();
	void 				loopOverHistograms();
	void 				etaClosureMergedPtBins(TDirectoryFile* idir);
	void 				loopOverBins(TH2F* hvar, unsigned int iVarBin);
    void 				adjust_fitrange(TH1* h,double& min,double& max);
	void 				checkResponse();
	pair<double,double> determineCanvasRange(double xmin, double xmax);
	void 				makeCanvases();
	void 				makeMergedCanvas();
	void 				writeToFile();
	void 				makeClosure(const VARIABLES::Variable ivar = VARIABLES::refpt);

private:
	bool           	 				 objects_loaded, draw_guidelines;
    double							 CMEnergy, nsigma;
	TString        	 				 path, filename, outputDir, outputFilename, flavor, alg, histMet;
	vector<TString>	 				 algs, outputFormat;
	JetInfo							 *ji;
	TFile							 *ifile, *ofile;
	ObjectLoader<TH2F> 				 hl;
    vector<TH1D*>                    h;
    vector<TF1*>                     func;
	vector<TH1F*> 					 hClosure;
	TF1				   				 *line, *linePlus, *lineMinus;
	vector<pair<TCanvas*,TLegend*> > canvases_legends;
	vector<TPaveText*>				 pave;
	VARIABLES::Variable 			 var;
	TDirectoryFile					 *odir;
	HistUtil::HistogramMetric        histogramMetric;
};

#endif

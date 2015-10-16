////////////////////////////////////////////////////////////////////////////////
//
// RatioMaker
// ----------
//
//            08/26/2015 Alexx Perloff           <alexx.stephen.perloff@cern.ch>
////////////////////////////////////////////////////////////////////////////////
#ifndef RATIOMAKER_HH
#define RATIOMAKER_HH

#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/ObjectLoader.h"
#include "JetMETAnalysis/JetUtilities/interface/RootStyle.h"
#include "JetMETAnalysis/JetUtilities/interface/JetInfo.hh"
#include "JetMETAnalysis/JetUtilities/interface/Variables.hh"
#include "JetMETAnalysis/JetUtilities/interface/Exceptions.hh"
#include "JetMETAnalysis/JetUtilities/interface/Style.h"

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
// define RatioMaker class
////////////////////////////////////////////////////////////////////////////////
class RatioMaker {
public:
	RatioMaker();
	RatioMaker(CommandLine& cl);

	//
	// member functions
	//
	void openInputFiles();
	void getHistograms(bool multiBin = false);
	void openOutputFile();
	void closeFiles();
	void makeLines();
	void makeRatio(const VARIABLES::Variable ivar = VARIABLES::refpt, bool multiBin = false);

private:
	string         	 				 filepath1, filepath2, algo, numerator, denominator;
    double							 CMEnergy, eta_max;
	bool           	 				 doflavor, algDiv, objects_loaded, draw_guidelines, draw_residual;
	TString        	 				 outputDir, flavor1, flavor2;
	vector<TString>	 				 outputFormat;
	JetInfo							 *ji, *jiNumerator, *jiDenominator;
	TFile							 *ifile1, *ifile2, *ofile;
	ObjectLoader<TH1F> 				 hl_1;
	ObjectLoader<TH1F> 				 hl_2;
	vector<TH1F*> 					 ratioHist;
	TF1				   				 *line, *linePlus, *lineMinus;
	vector<pair<TCanvas*,TLegend*> > canvases_legends;
	vector<TPaveText*>				 pave;
	VARIABLES::Variable 			 var;
	pair<int,int>					 pad_numbers;
};

#endif

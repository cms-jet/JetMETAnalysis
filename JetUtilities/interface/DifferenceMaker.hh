////////////////////////////////////////////////////////////////////////////////
//
// DifferenceMaker
// ---------------
//
//            09/11/2015 Alexx Perloff           <alexx.stephen.perloff@cern.ch>
////////////////////////////////////////////////////////////////////////////////
#ifndef DIFFERENCEMAKER_HH
#define DIFFERENCEMAKER_HH

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
#include "TGraphAsymmErrors.h"
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
#include <locale>
#include <stdexcept>
#include <exception>
#include <memory>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// define constants
////////////////////////////////////////////////////////////////////////////////
static vector<int> colors = {kRed+1,kBlue+1};
static vector<int> colorsMore = {kRed+1,kOrange+1,kGreen+2,kBlue+1,kViolet+1,kMagenta+1};
static vector<int> shapes = {kFullCircle,kOpenCircle};
static vector<int> shapesMore = {kFullSquare,kFullCircle,kFullDiamond,
                                 kOpenSquare,kOpenCircle,kOpenDiamond};

////////////////////////////////////////////////////////////////////////////////
// define DifferenceMaker class
////////////////////////////////////////////////////////////////////////////////
class DifferenceMaker {
public:
	DifferenceMaker();
	DifferenceMaker(CommandLine& cl);

	//
	// member functions
	//
	void makeDifference();
	void makeFlavorDifference();
	void openInputFiles();
	void openOutputFile();
	void openCanvas();
	void loopOverFilesAndDirectories();
	void loopOverFilesAndAlgorithms();
	void getGraphs(TDirectoryFile* idir);
	void makeDifferenceGraph(double epsilon_point = 0.05);
	void drawDifference(int count);
	void writeToFile();
	void closeFiles();

private:
	bool           	 				 objects_loaded, flavorDiff, flavor1AreEqual, flavor2AreEqual;
	TString        	 				 basepath, outputDir, object;
	vector<TString>	 				 filenames,outputFormat, algs, flavor1, flavor2;
	vector<TFile*>					 ifiles;
	unique_ptr<JetInfo>				 ji;
	TFile							 *ofile;
	vector<pair<TGraph*,TGraph*> >			 gl;
	vector<TGraph*>					 glDiff;
	vector<pair<TCanvas*,TLegend*> > canvases_legends;
	VARIABLES::Variable 			 var;
};

#endif

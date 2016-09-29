////////////////////////////////////////////////////////////////////////////////
//
// HistogramUtilities
// ------------------
//
//            04/07/2016 Alexx Perloff          <alexx.stephen.perloff@cern.ch>
////////////////////////////////////////////////////////////////////////////////

#ifndef HISTOGRAM_UTILITIES_HH
#define HISTOGRAM_UTILITIES_HH

//ROOT libraries
#include "TROOT.h"
#include "TSystem.h"
#include "TMath.h"
#include "TH2.h"
#include "TF1.h"

//C++ libraries
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <cmath>

#include "JetMETAnalysis/JetUtilities/interface/ProgressBar.hh"

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::pair;
using std::make_pair;

////////////////////////////////////////////////////////////////////////////////
// namespace definition
////////////////////////////////////////////////////////////////////////////////

namespace HistUtil{

    // ---------------------------------------------------------------
    //            ALL ABOUT THE DIFFERENT HISTOGRAM METRICS
    // ---------------------------------------------------------------
    // For any given histogram we can define several metrics
    // These metrics are seemingly similar, but they have profoundly different meanings
    // These metrics are:
    //     (1) The mean (mu_h) and RMS (RMS_h) of the histogram
    //     (2) The mean (mu_f or mpv) and RMS (RMS_f or sigma_f) of a fit to the histogram (i.e. a Gaussian fit)
    //     (3) The median of the histogram
    enum HistogramMetric {none, mu_h, RMS_h, mu_f, mpv, RMS_f, sigma_f, median};
    static const unsigned int nHistogramMetric = 8; 

    enum AxisDirection {X, Y, Z};
    static const unsigned int nAxisDirections = 3;

    // A routine that returns the string given the HistogramMetric 
    string getHistogramMetricString(HistogramMetric);

    // A routine that returns the HistogramMetric type given the string
    HistogramMetric getHistogramMetricType(string);

    // A routine that returns the associated metric
    HistogramMetric getAssociatedHistogramMetric(HistogramMetric);

    // A routine that returns a given HistogramMetric and its error (if any)
    pair<double,double> getHistogramMetric1D(HistogramMetric, TH1*, double fallback_threshold = 0.05, bool verbose = false);
    pair<double,double> getHistogramMetric1D(string, TH1*, double fallback_threshold = 0.05, bool verbose = false);    

    // A routine that returns the median of a given histogram and an error equal to the width of the bin that contains the histogram
    pair<double,double> getHistogramMedian1D(TH1*, bool debug = false);

    // A routine that returns the median of an array with weights w
    // Algorithm taken from https://root.cern.ch/root/html524/src/TMath.h.html#dZxoRB
    //     but this only returns the median location at the center of the bin containing the median
    // Median calculation from http://math.stackexchange.com/questions/879052/how-to-find-mean-and-median-from-histogram
    //     This contains the assumption that observations are uniformly distributed within the median bar and
    //     yields the fraction of median bar width corresponding to the position of the median plus the lower bound of the
    //     median bar.
    template <typename T>
    double Median(Long64_t n, const T *a,  const Double_t *w, const Double_t* boundaries = 0, Long64_t *work = 0, bool debug = false);

    // A simple unit test of the Median function. For an interactive test do:
    // .L ../../JetUtilities/interface/HistogramUtilities.hh
    // .L ../../JetUtilities/src/HistogramUtilities.cc
    // HistUtil::testMedian()
    // Answer should be "Median = 5.4 +/- 1"
    void testMedian();

    // A routine that returns a vector of HistogramMetrics and their errors (if any)
    // The AxisDirection is not axis along which the metric will be charted
    //      (i.e. if the AxisDirection=Y, then the metric will be plotted along X)
    vector<pair<double,double> > getHistogramMetric2D(HistogramMetric, TH2*, AxisDirection, const vector<string>&);
    vector<pair<double,double> > getHistogramMetric2D(string, TH2*, AxisDirection, const vector<string>&);
}

#endif

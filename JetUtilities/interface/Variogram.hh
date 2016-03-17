////////////////////////////////////////////////////////////////////////////////
//
// Variogram
// ---------
//
//            03/06/2016 Alexx Perloff           <alexx.stephen.perloff@cern.ch>
////////////////////////////////////////////////////////////////////////////////
#ifndef VARIOGRAM_HH
#define VARIOGRAM_HH

// ROOT Libraries
#include "TSystem.h"
#include "TMath.h"
#include "TH2D.h"
#include "TF1.h"
#include "TLine.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TPaveStats.h"
#include "TMatrixD.h"
#include "Math/GenVector/Cartesian3D.h"

// C++ Libraries
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <utility>
#include <exception>
#include <algorithm>

// JRA Libraries
#include "JetMETAnalysis/JetUtilities/interface/RootStyle.h"
#include "JetMETAnalysis/JetUtilities/interface/Style.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::map;
using std::pair;
using std::make_pair;

typedef ROOT::Math::Cartesian3D<double> point;

bool point_sort (point i, point j) { return (i.Z()<j.Z()); }

////////////////////////////////////////////////////////////////////////////////
// define Variogram class
////////////////////////////////////////////////////////////////////////////////

class Variogram {
public:
	//Constructor
   	Variogram(TH2D* data);
   
   	//Member functions
   	void dist_to_vector_from_squareform(bool debug = false);
   	inline double getMean() {
        double return_value = 0.0;
        int n = sorted_z_values.size();
        for(int i=0; i<n; i++) {
            return_value += sorted_z_values[i];
        }      
        return (return_value/n);
   	}
   	void getNormalQuantiles();
   	void getSemivariogram(vector<int> &lags, int tol, bool pedestalSub = false);
   	inline double getSill() {
   		double mean = getMean();
        double sum = 0.0;
        double temp =0.0;
        double var =0.0;
        int n = sorted_z_values.size();
        for(int j=0; j<n; j++) {
            temp = pow(sorted_z_values[j]-mean,2);
            sum += temp;
        }
        return var = sum/(sorted_z_values.size()-2);
   	}
   	TH1D* getZdistributionFromData(bool get_z_values = false, int nbins = 100);
   	vector<pair<int,int> > lagindices(int lag, int tol);
   	void pdist();
   	TCanvas* plotSemivariogram(vector<int> &lags, int tol, string model = "", bool pedestalSub = false, map<string,double> *param = 0);
   	double semivariance(vector<pair<int,int> > &indices);
   	void showScattergrams(int lag, int tol);
   	void showSemivariogram();
   	void verifyNormality();

   	//Models
   	double spherical(double *x, double *p);

protected:
	void setData(TH2D* h) {data_points = h;}
   	void setZdistribution(TH1D* z) {z_distribution = z;}
   	//vector<double> calc_uniform_order_statistic_medians(vector<double> &x);

protected:
   	TH2D* data_points;
   	TH1D* z_distribution;
   	vector<point>  sorted_points;
   	vector<double> sorted_z_values;
   	vector<double> normal_quantiles;
   	Double_t  fXq1;    //x1 coordinate of the interquartile line
   	Double_t  fXq2;    //x2 coordinate of the interquartile line
   	Double_t  fYq1;    //y1 coordinate of the interquartile line
   	Double_t  fYq2;    //y2 coordinate of the interquartile line
   	TGraph* z_vs_quantile;
   	TMatrixD* pairwise_distances;
   	vector<double> pairwise_distances_vector;
   	vector<TGraph*> scattergrams;
   	double sill;
   	vector<double> v;
   	vector<double> sv;
   	TGraph* semivariogram;
};

#endif

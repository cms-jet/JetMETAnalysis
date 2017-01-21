////////////////////////////////////////////////////////////////////////////////
//
// PiecewiseSpline
// ---------------
//
//            10/31/2016 Alexx Perloff           <alexx.stephen.perloff@cern.ch>
////////////////////////////////////////////////////////////////////////////////
#ifndef PIECEWISESPLINE_HH
#define PIECEWISESPLINE_HH

#include "JetMETAnalysis/JetUtilities/interface/TSplineSteffen.hh"

#include "TGraphErrors.h"
#include "TF1.h"
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
#include <vector>
#include <string>
#include <sstream>
#include <utility>
#include <algorithm>

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::pair;
using std::make_pair;
using std::stringstream;

typedef struct
{
  double * c;
  double * g;
  double * diag;
  double * offdiag;
} cspline_state_t;

typedef struct
{
  double *b;
  double *c;
  double *d;
  double *_m;
} akima_state_t;

typedef struct
{
  double * a;
  double * b;
  double * c;
  double * d;

  double * y_prime;
} steffen_state_t;

enum ROOT_spline_type{TS3, TS5, TSSteffen, none};

////////////////////////////////////////////////////////////////////////////////
// define PiecewiseSpline class
////////////////////////////////////////////////////////////////////////////////
class PiecewiseSpline {
public:
    PiecewiseSpline();
    PiecewiseSpline(TSpline* s, vector<double> mp) : spline(s), merge_points(mp) {};
    PiecewiseSpline(string name, TGraph* graph, vector<double> mp, ROOT_spline_type rst = TS3, bool setSpline = true) {
        spline_type = rst;

        if(setSpline) {
            if      (spline_type==TS5)       spline = new TSpline5(name.c_str(),graph);
            else if (spline_type==TSSteffen) spline = new TSplineSteffen(name.c_str(),graph);
            else                             spline = new TSpline3(name.c_str(),graph);
        }
        merge_points = mp;
        if(merge_points.size()!=0 && merge_points.size()!=2)
            cout << "ERROR::PiecewiseSpline::There must be 0 or 2 merge points. Currently there are " << merge_points.size() << endl
                 << "\tZero merge points means spline and two merge points means spline+function(+spline)" << endl;
    } 

    vector<double>          getCoeff(int i);
    string                  getFullFormula(){return full_function->GetTitle();}
    TF1*                    getFullFunction(){return full_function;}
    int                     getNSections();
    int                     getNMerges() {return merge_points.size();}
    int                     getNpar() {return full_function->GetNpar();}
    double                  getMergePoint(int ipoint) {return merge_points[ipoint];}
    static ROOT_spline_type getROOTSplineType(string s);
    int                     getSection(float x);
    pair<double,double>     getSectionBounds(int section);
    static TSpline*         gslToROOT_spline(gsl_spline* iSpline, string oName);
    bool                    isSplineSection(int section);
    int                     nSectionsBeforeFunction();
    int                     nSectionsCoveredByFunction();
    void                    setFullFunction(string function);
    void                    setMergePoints(vector<double> mp) {merge_points = mp;}
    TF1*                    setParameters(int section);
    void                    setPartialFunction(TF1* pf);
    void                    setSpline(TSpline* s) {spline = s;}
    double                  splineEval(double x) {return spline->Eval(x);}

private:
    TSpline* spline;
    vector<double> merge_points;
    TF1 *partial_function, *full_function;
    int num_pfunc_pars, num_spline_pars;
    ROOT_spline_type spline_type;
};

#endif

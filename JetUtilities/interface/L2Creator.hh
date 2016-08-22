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
#include "JetMETAnalysis/JetUtilities/interface/Style.h"
#include "JetMETAnalysis/JetUtilities/interface/JetInfo.hh"
#include <gsl/gsl_math.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_interp.h>

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

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <locale>

using namespace std;

typedef struct
{
  double *b;
  double *c;
  double *d;
  double *_m;
}akima_state_t;

////////////////////////////////////////////////////////////////////////////////
// define PiecewiseSpline class
////////////////////////////////////////////////////////////////////////////////
class PiecewiseSpline {
public:
    PiecewiseSpline();
    PiecewiseSpline(TSpline* s, vector<double> mp) : spline(s), merge_points(mp) {};
    PiecewiseSpline(string name, TGraph* graph, vector<double> mp, bool isFifthOrder = false, bool setSpline = true) {
        if(setSpline) {
            if(isFifthOrder) spline = new TSpline5(name.c_str(),graph);
            else             spline = new TSpline3(name.c_str(),graph);
        }
        merge_points = mp;
        if(merge_points.size()!=0 && merge_points.size()!=2)
            cout << "ERROR::PiecewiseSpline::There must be 0 or 2 merge points. Currently there are " << merge_points.size() << endl
                 << "\tZero merge points means spline and two merge points means spline+function(+spline)" << endl;
    } 

    vector<double>      getCoeff(int i);
    string              getFullFormula(){return full_function->GetTitle();}
    TF1*                getFullFunction(){return full_function;}
    int                 getNSections();
    int                 getNMerges() {return merge_points.size();}
    int                 getNpar() {return full_function->GetNpar();}
    double              getMergePoint(int ipoint) {return merge_points[ipoint];}
    pair<double,double> getSectionBounds(int section);
    static TSpline3*    gslToROOT_spline(gsl_spline* iSpline, string oName);
    bool                isFifthOrder() {return spline->ClassName()==string("TSpline5");}
    bool                isSplineSection(int section);
    int                 nSectionsBeforeFunction();
    int                 nSectionsCoveredByFunction();
    void                setSpline(TSpline* s) {spline = s;}
    void                setMergePoints(vector<double> mp) {merge_points = mp;}
    TF1*                setParameters(int section);
    void                setPartialFunction(TF1* pf);
    double              splineEval(double x) {return spline->Eval(x);}

private:
    TSpline* spline;
    vector<double> merge_points;
    TF1 *partial_function, *full_function;
    int num_pfunc_pars, num_spline_pars;
};

vector<double> PiecewiseSpline::getCoeff(int section) {
    double x,y,b,c,d,e,f;
    if(isFifthOrder()) {
        dynamic_cast<TSpline5*>(spline)->GetCoeff(section,x,y,b,c,d,e,f);
        return {y,x,b,c,d,e,f};
    }
    else {
        dynamic_cast<TSpline3*>(spline)->GetCoeff(section,x,y,b,c,d);
        e = 0; f = 0;
        return {y,x,b,c,d};
    }
}

int PiecewiseSpline::getNSections() {
    if(merge_points.size()==0) return spline->GetNp()-1;

    int number_sections = 0;
    double x,y;
    for(int isec=0; isec<spline->GetNp(); isec++) {
        spline->GetKnot(isec,x,y);
        if(x<partial_function->GetXmin()) number_sections++;
        else if(x>=partial_function->GetXmax()) number_sections++;
    }
    return number_sections++;
}

pair<double,double> PiecewiseSpline::getSectionBounds(int section) {
    if(section>nSectionsBeforeFunction()) section = section+(nSectionsCoveredByFunction()-1);

    if(!isSplineSection(section)) return make_pair(partial_function->GetXmin(),partial_function->GetXmax());

    double x,y,xp1,yp1;
    spline->GetKnot(section,x,y);
    spline->GetKnot(section+1,xp1,yp1);
    return make_pair(x,xp1);
}

TSpline3* PiecewiseSpline::gslToROOT_spline(gsl_spline* iSpline, string oName) {
   size_t n = iSpline->size;
   const akima_state_t *state = (const akima_state_t *) iSpline->interp->state;

   TSpline3 *oSpline = new TSpline3(oName.c_str(), iSpline->x, iSpline->y, n);

   for(unsigned int i=0; i<n; i++) {
      oSpline->SetPointCoeff(i,state->b[i],state->c[i],state->d[i]);
   }

   return oSpline;
}

bool PiecewiseSpline::isSplineSection(int section) {
    if(merge_points.size()==0) return true;

    double x,y;
    spline->GetKnot(section,x,y);
    if(x<partial_function->GetXmin() || x>=partial_function->GetXmax()) return true;
    return false;
}

int PiecewiseSpline::nSectionsBeforeFunction() {
    int ret = 0;
    for(int isec=0; isec<spline->GetNp(); isec++) {
        if(isSplineSection(isec)) ret++;
        else break;
    }
    return ret;
}

int PiecewiseSpline::nSectionsCoveredByFunction() {
    int ret = 0;
    for(int isec=0; isec<spline->GetNp(); isec++) {
        if(!isSplineSection(isec)) ret++;
    }
    return ret;
}

TF1* PiecewiseSpline::setParameters(int section) {
    if(section>nSectionsBeforeFunction()) section = section+(nSectionsCoveredByFunction()-1);

    if(isSplineSection(section) && merge_points.size()==0) {
        vector<double> coeff = getCoeff(section);
        for(int i=0; i<num_spline_pars; i++) {
            full_function->SetParameter(i,coeff[i]);
        }
    }
    else if(isSplineSection(section)) {
        for(int i=0; i<num_pfunc_pars; i++) {
            full_function->SetParameter(i,0.0);
        }
        vector<double> coeff = getCoeff(section);
        for(int i=num_pfunc_pars; i<num_pfunc_pars+num_spline_pars; i++) {
            full_function->SetParameter(i,coeff[i-num_pfunc_pars]);
        }
    }
    else {
        for(int i=0; i<num_pfunc_pars; i++) {
            full_function->SetParameter(i,partial_function->GetParameter(i));
        }
        for(int i=num_pfunc_pars; i<num_pfunc_pars+num_spline_pars; i++) {
            full_function->SetParameter(i,0.0);
        }
    }
    if(merge_points.size()>0) {
        full_function->SetParameter(num_pfunc_pars+num_spline_pars,merge_points[0]);
        full_function->SetParameter(num_pfunc_pars+num_spline_pars+1,merge_points[1]);
    }

    return full_function;
}

void PiecewiseSpline::setPartialFunction(TF1* pf) {
    partial_function = pf;
    num_pfunc_pars = partial_function->GetNpar();
    num_spline_pars = (spline->ClassName()==string("TSpline3")) ? 5 : 7;

    //Spline3: min(x>[11],x<[12])*([0]+[1]/(pow(log10(x),2)+[2])+[3]*exp(-[4]*(log10(x)-[5])*(log10(x)-[5])))+max(x>=[12],x<=[11])*([6]+(x-[7])*([8]+(x-[7])*([9]+(x-[7])*[10])))
    //Spline5: min(x>[13],x<[14])*([0]+[1]/(pow(log10(x),2)+[2])+[3]*exp(-[4]*(log10(x)-[5])*(log10(x)-[5])))+max(x>=[14],x<=[13])*([6]+(x-[7])*([8]+(x-[7])*([9]+(x-[7])*([10]+(x-[7])*([11]+(x-[7])*[12])))))
    stringstream ss;
    ss << "min(x>[" << num_pfunc_pars + num_spline_pars << "],x<[" << num_pfunc_pars + num_spline_pars + 1 << "])*(" << partial_function->GetTitle()
       << ")+max(x>=[" << num_pfunc_pars + num_spline_pars + 1 << "],x<=[" << num_pfunc_pars + num_spline_pars << "])*(["
       << num_pfunc_pars << "]+(x-[" << num_pfunc_pars+1 << "])*([" << num_pfunc_pars+2 << "]+(x-[" << num_pfunc_pars+1 << "])*(["
       << num_pfunc_pars+3 << "]+(x-[" << num_pfunc_pars+1 << "])*";

    if(spline->ClassName()==string("TSpline3")) {
        ss << "[" << num_pfunc_pars+4 << "])))";
    }
    else {
        ss << "([" << num_pfunc_pars+4 << "]+(x-[" << num_pfunc_pars+1 << "])*([" << num_pfunc_pars+5 << "]+(x-[" << num_pfunc_pars+1
           << "])*[" << num_pfunc_pars+6 << "])))))";
    }

    if(merge_points.size()>0)
        full_function = new TF1("fit",ss.str().c_str(),spline->GetXmin(),spline->GetXmax());
    else
        full_function = partial_function;
}


////////////////////////////////////////////////////////////////////////////////
// define L2Creator class
////////////////////////////////////////////////////////////////////////////////
class L2Creator {
public:
    L2Creator();
    L2Creator(CommandLine& cl);

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
    vector<PiecewiseSpline*> vabscor_eta_spline;
};

#endif
////////////////////////////////////////////////////////////////////////////////
//
// jet_response_fitter_x
// ---------------------
//
//            08/08/2008 Kostas Kousouris                    <kkousour@fnal.gov>
//                       Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"

#include <TROOT.h>
#include <TSystem.h>
#include <TFile.h>
#include <TKey.h>
#include <TH1F.h>
#include <TF1.h>
#include <TVirtualFitter.h>
#include <TMath.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;


////////////////////////////////////////////////////////////////////////////////
// define local functions
////////////////////////////////////////////////////////////////////////////////


/// default fit with gaussian in niter iteration of mean
void fit_gaussian(TH1F*& hrsp,
		  const double nsigma,
		  const double jtptmin,
		  const int niter);

/// optional double sided crystal ball fit to response distributions
int fit_dscb(TH1F*& hrsp,
	     const double nsigma,
	     const double jtptmin,
	     const int niter,
	     const string alg);

/// double sided crystal ball function definition
double fnc_dscb(double*xx,double*pp);

/// test this...
void guesstimate_fitrange(TH1* h,double& min,double& max,const string alg);


/// check if a vector of strings contains a certain element
bool contains(const vector<string>& collection,const string& element);
void adjust_fitrange(TH1* h,double& min,double& max);
template <class T>
bool from_string(T& t, 
                 const std::string& s, 
                 std::ios_base& (*f)(std::ios_base&))
{
  std::istringstream iss(s);
  return !(iss >> f >> t).fail();
}

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv)
{
  //
  // evaluate command-line / configuration file options
  // 
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;
  
  string         input   = cl.getValue<string> ("input");
  string         output  = cl.getValue<string> ("output",        "");
  double         nsigma  = cl.getValue<double> ("nsigma",       1.5);
  float          jtptmin = cl.getValue<float>  ("jtptmin",      1.0);
  int            niter   = cl.getValue<int>    ("niter",          3);
  int            ndfmin  = cl.getValue<int>    ("ndfmin",         5);
  vector<string> algs    = cl.getVector<string>("algs",          "");
  int            verbose = cl.getValue<int>    ("verbose",        0);
  int            fittype = cl.getValue<int>    ("fittype",        0);         
  
  if (!cl.check()) return 0;
  cl.print();
  
  if (fittype<0 || fittype>1) {
    cout<<"ERROR: fittype not known, choose 0 for GAUSS, 1 for DSCB";return 0;
  }
  else if (0==fittype) cout<<"*** Fitting with distributions w/ GAUSS"<<endl;
  else cout<<"*** Fitting response distributions w/ DSCB"<<endl; 

  //
  // construct output file name from input file name if none given
  //
  if (output.empty()) {
    size_t pos=input.find(".root");
    output=input.substr(0,pos)+"_f.root";
    cout<<"*** write output to "<<output<<endl;
  }
  

  //
  // open input file and loop over input directories (=algorithms)
  //
  TFile* ifile = new TFile(input.c_str(),"READ");
  if (!ifile->IsOpen()) { cout<<"Can't open "<<input<<endl; return 0; }

  TFile* ofile = new TFile(output.c_str(),"UPDATE");
  if (!ofile->IsOpen()) { cout<<"Can't create "<<output<<endl; return 0; }

  TIter nextDir(ifile->GetListOfKeys());
  TKey* dirKey(0);
  while ((dirKey=(TKey*)nextDir())) {
    
    if (strcmp(dirKey->GetClassName(),"TDirectoryFile")!=0) continue;

    TDirectoryFile* idir = (TDirectoryFile*)dirKey->ReadObj();
    string alg(idir->GetName());
    
    if (algs.size()>0&&!contains(algs,alg)) continue;

    if (0!=ofile->Get(idir->GetName())) {
      cout<<"directory '"<<alg<<"' exists already in "<<output<<", skip!"<<endl;
      continue;
    }

    TDirectoryFile* odir = (TDirectoryFile*)ofile->mkdir(idir->GetName());
    odir->cd();
    
    cout<<alg<<" ... "<<endl;
    

    //
    // loop over response histogram and fit them with the selected func (iteratively)
    //

    TIter nextHist(idir->GetListOfKeys());
    TKey* histKey(0);
    while ((histKey=(TKey*)nextHist())) {
      if (strcmp(histKey->GetClassName(),"TH1F")!=0) continue;
      
      TH1F* hrsp = (TH1F*)histKey->ReadObj();
      string histname(hrsp->GetName());
      
      if (histname.find("RelRsp")!=0&&histname.find("AbsRsp")!=0) {
	hrsp->Write();
	continue;
      }
      
      if (hrsp->Integral()>0.0) {
	int fitstatus(0);
	if (0==fittype) fit_gaussian(hrsp,nsigma,jtptmin,niter);
	else fitstatus = fit_dscb(hrsp,nsigma,jtptmin,niter,alg);

	TF1* fitfnc = (TF1*) hrsp->GetListOfFunctions()->Last();
	if (0!=fitfnc && 0==fitstatus) fitfnc->ResetBit(TF1::kNotDraw);

	if (verbose>0&&0!=fitfnc) 
	  cout<<"histo: "<<hrsp->GetName()<<"-> fnc: "<<fitfnc->GetName()<<endl;

	if (0!=fitfnc&&fitfnc->GetNDF()<ndfmin) {
	  if (verbose>0) cout<<"NDOF("<<fitfnc->GetName()<<")="
			     <<fitfnc->GetNDF()
			     <<" FOR "<<alg<<"::"<<hrsp->GetName()<<endl;
	  hrsp->GetListOfFunctions()->Delete();
	}
      }
      else {
	if (verbose>0)
	  cout<<"NOT ENOUGH ENTRIES FOR "<<alg<<"::"<<hrsp->GetName()<<endl;
      }
      hrsp->Write();
    }

    cout<<"response fits for *"+alg+"* completed ..."<<flush;
    odir->Write();
    odir->DeleteAll();
    delete odir;
    cout<<" and saved!\n"<<endl;
  }
  
  
  //
  // update the input file
  //
  cout<<"update output file "<<output<<" ..."<<flush;
  gROOT->GetListOfFiles()->Remove(ofile);
  ofile->Close();
  delete ofile;
  gROOT->GetListOfFiles()->Remove(ifile);
  ifile->Close();
  delete ifile;
  cout<<" DONE."<<endl;
  
  
  return 0;
}


////////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int fit_dscb(TH1F*& hrsp,
	     const double nsigma,
	     const double jtptmin,
	     const int niter,
	     const string alg)
{
  if (0==hrsp) {
    cout<<"ERROR: Empty pointer to fit_dscb()"<<endl;return -1;
  }

  // first use a gaussian to constrain crystal ball gaussian core

  fit_gaussian(hrsp,nsigma,jtptmin,niter);

  TF1* fgaus = hrsp->GetFunction("fgaus");
  if (0==fgaus) {
    hrsp->GetListOfFunctions()->Delete();
    return -1;
  }

  // implementation of the low pt bias threshold

  string histname = hrsp->GetName();
  double ptRefMax(1.0),rspMax(0.0);

  if (histname.find("RefDiPt")==string::npos&&
      histname.find("JetDiPt")==string::npos) {

    int pos1     = histname.find("RefPt");
    int pos2     = histname.find("to",pos1);
    string ss    = histname.substr(pos1+5,pos2);
    if (from_string(ptRefMax,ss,std::dec)) {
      if (histname.find("RelRsp")==0)
	rspMax = jtptmin/ptRefMax;
      if (histname.find("AbsRsp")==0)
	rspMax = jtptmin-ptRefMax;
    }
  }

  double fitrange_min(0.0);
  if (alg.find("pf")!=string::npos) fitrange_min = std::max(rspMax,0.3);
  else if (alg.find("PF")!=string::npos) fitrange_min = std::max(rspMax,0.3);
  else fitrange_min = std::max(rspMax,0.2);
  double fitrange_max = 1.7;

  adjust_fitrange(hrsp,fitrange_min,fitrange_max);
  //guesstimate_fitrange(hrsp,fitrange_min,fitrange_max,alg);

  
  TF1* fdscb = new TF1("fdscb",fnc_dscb,fitrange_min,fitrange_max,7);

  // set the std values

  double norm = fgaus->GetParameter(0);
  double mean = fgaus->GetParameter(1);
  double sigma= fgaus->GetParameter(2);

  double aone(2.0),atwo(2.0),pone(5.0),ptwo(5.0);
  TVirtualFitter::SetDefaultFitter("Minuit2");

  int fitstatus(0); 
  for (unsigned i=0;i<4;i++) {

    fdscb->SetParameter(0,norm); // N
    fdscb->SetParameter(1,mean); // mean
    fdscb->SetParameter(2,sigma);// sigma
    fdscb->SetParameter(3,aone); // a1
    fdscb->SetParameter(4,pone); // p1
    fdscb->SetParameter(5,atwo); // a2
    fdscb->SetParameter(6,ptwo); // p2

    fdscb->FixParameter(1,mean);
    fdscb->FixParameter(2,sigma);
    
    if (i>0) fdscb->FixParameter(3,aone);
    else fdscb->SetParLimits(3,1.,5.);

    if (i>1) fdscb->FixParameter(5,atwo);
    else fdscb->SetParLimits(5,1.,5.);

    fdscb->SetParLimits(4,0.,25.);
    fdscb->SetParLimits(6,0.,25.);

    fitstatus = hrsp->Fit(fdscb,"RQB0+");
    
    if (0==fitstatus) i=999;

    delete fdscb;
    fdscb = hrsp->GetFunction("fdscb");

    if (0==fdscb) return -1;
      
    norm  = fdscb->GetParameter(0);
    aone  = fdscb->GetParameter(3);
    pone  = fdscb->GetParameter(4);
    atwo  = fdscb->GetParameter(5);
    ptwo  = fdscb->GetParameter(6);

  }

  // reset sigma and mean to gauss values...
  fdscb->SetParameter(1,fgaus->GetParameter(1));
  fdscb->SetParError(1,fgaus->GetParError(1));
  fdscb->SetParameter(2,fgaus->GetParameter(2));
  fdscb->SetParError(2,fgaus->GetParError(2));


  if (0!=fitstatus){
    cout<<"fit_fdscb() to "<<hrsp->GetName()
    	<<" failed. Fitstatus: "<<fitstatus<<endl;
    hrsp->GetFunction("fdscb")->Delete();
  }

  return fitstatus;
}

//______________________________________________________________________________
double fnc_dscb(double*xx,double*pp)
{
  double x   = xx[0];
  // gaussian core
  double N   = pp[0];//norm
  double mu  = pp[1];//mean
  double sig = pp[2];//variance
  // transition parameters
  double a1  = pp[3];
  double p1  = pp[4];
  double a2  = pp[5];
  double p2  = pp[6];
  
  double u   = (x-mu)/sig;
  double A1  = TMath::Power(p1/TMath::Abs(a1),p1)*TMath::Exp(-a1*a1/2);
  double A2  = TMath::Power(p2/TMath::Abs(a2),p2)*TMath::Exp(-a2*a2/2);
  double B1  = p1/TMath::Abs(a1) - TMath::Abs(a1);
  double B2  = p2/TMath::Abs(a2) - TMath::Abs(a2);

  double result(N);
  if      (u<-a1) result *= A1*TMath::Power(B1-u,-p1);
  else if (u<a2)  result *= TMath::Exp(-u*u/2);
  else            result *= A2*TMath::Power(B2+u,-p2);
  return result;
}


//______________________________________________________________________________
void fit_gaussian(TH1F*& hrsp,
		  const double nsigma,
		  const double jtptmin,
		  const int niter)
{
  if (0==hrsp) {
    cout<<"ERROR: Empty pointer to fit_gaussian()"<<endl;return;
  }
  
  string histname = hrsp->GetName();
  double mean     = hrsp->GetMean();
  double rms      = hrsp->GetRMS();
  double ptRefMax(1.0),rspMax(0.0);     

  double norm  = hrsp->GetMaximumStored();
  double peak  = mean;
  double sigma = rms;

  if (histname.find("RefDiPt")==string::npos&&
      histname.find("JetDiPt")==string::npos) {
    int pos1     = histname.find("RefPt");
    int pos2     = histname.find("to",pos1);
    string ss    = histname.substr(pos1+5,pos2);
    if (from_string(ptRefMax,ss,std::dec)) {
      if (histname.find("RelRsp")==0)
	rspMax = jtptmin/ptRefMax;
      if (histname.find("AbsRsp")==0)
	rspMax = jtptmin-ptRefMax;
    }
  }
  double xmin  = hrsp->GetXaxis()->GetXmin();
  double xmax  = hrsp->GetXaxis()->GetXmax();
  TF1* fitfnc(0); int fitstatus(-1);
  for (int iiter=0;iiter<niter;iiter++) {
    vector<double> vv;
    vv.push_back(rspMax);
    vv.push_back(xmin);
    vv.push_back(peak-nsigma*sigma);   
    double fitrange_min = *std::max_element(vv.begin(),vv.end());
    double fitrange_max = std::min(xmax,peak+nsigma*sigma);
    adjust_fitrange(hrsp,fitrange_min,fitrange_max);
    fitfnc = new TF1("fgaus","gaus",fitrange_min,fitrange_max);
    fitfnc->SetParNames("N","#mu","#sigma");
    fitfnc->SetParameter(0,norm);
    fitfnc->SetParameter(1,peak);
    fitfnc->SetParameter(2,sigma);
    fitstatus = hrsp->Fit(fitfnc,"RQ0");
    delete fitfnc;
    fitfnc = hrsp->GetFunction("fgaus");
    //fitfnc->ResetBit(TF1::kNotDraw);
    norm  = fitfnc->GetParameter(0);
    peak  = fitfnc->GetParameter(1);
    sigma = fitfnc->GetParameter(2);
  }
  if (0!=fitstatus){
    cout<<"fit_gaussian() to "<<hrsp->GetName()
	<<" failed. Fitstatus: "<<fitstatus
	<<" - FNC deleted."<<endl;
    hrsp->GetListOfFunctions()->Delete();
  }
}


//______________________________________________________________________________
bool contains(const vector<string>& collection,const string& element)
{
  vector<string>::const_iterator it;
  for (it=collection.begin();it!=collection.end();++it)
    if ((*it)==element) return true;
  return false;
}


//______________________________________________________________________________
void adjust_fitrange(TH1* h,double& min,double& max)
{
  int imin=1; while (h->GetBinLowEdge(imin)<min) imin++;
  int imax=1; while (h->GetBinLowEdge(imax)<max) imax++;
  while ((imax-imin)<8) {
    if (imin>1) {imin--; min = h->GetBinCenter(imin); }
    if (imax<h->GetNbinsX()-1) { imax++; max=h->GetBinCenter(imax); }
  }
}

//______________________________________________________________________________
void guesstimate_fitrange(TH1* h,double& min,double& max, const string alg)
{

  //hh: tried a variety of different possibilities here

  const double norm = h->GetEffectiveEntries();
  if (norm<=0.0) return;
  double lsum(0.0); double hsum(0.0);
  int nmax = h->GetNbinsX();
  if (nmax<3) return;
  int imin=1; while (imin<nmax) {
    lsum+=h->GetBinContent(imin);
    if (alg.find("pf")!=string::npos) {
      if (lsum/norm>.0005) break;
    }
    else {
      if (lsum/norm>.00005) break;
    }
    imin++;
  } 
  int imax=nmax-1; while (imax>0) {
    hsum+=h->GetBinContent(imax);
    if (lsum/norm>.000001) break;
    imin--;
  }
  min = h->GetBinLowEdge(imin);
  max = h->GetBinLowEdge(imax);
}

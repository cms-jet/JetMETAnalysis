////////////////////////////////////////////////////////////////////////////////
//
// jet_response_fitter_x
// ---------------------
//
//            08/08/2008 Kostas Kousouris                    <kkousour@fnal.gov>
//                       Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/JetInfo.hh"

#include <TROOT.h>
#include <TSystem.h>
#include <TFile.h>
#include <TKey.h>
#include <TH1F.h>
#include <TF1.h>
#include <TVirtualFitter.h>
#include <TMath.h>
#include <TSpectrum.h>
#include "TRandom3.h"
#include "TCanvas.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <math.h>
#include <map>
#include <algorithm>

using namespace std;


////////////////////////////////////////////////////////////////////////////////
// define local functions
////////////////////////////////////////////////////////////////////////////////

/// set new histogram axis range to compute KS test
TH1F *hXaxis( TH1F* hist, double minX, double maxX);

/// optional modify crystal ball fit to response distribution
void fit_modified_fnc_dscb(TH1F*& hrsp, string alg, bool calibrated, string histName); // used to call the global fit
double sumGausPol7(double *x, double *par); // gaus+polynome 7 function
double exp_fcn(double x, double norm, double slope); // exp function
double gauss_fcn(double x, double norm, double mean, double sigma); // gauss function
double pol7_fcn(double x, double p0, double p1, double p2, double p3, double p4, double p5, double p6, double p7); // poly 7 function
double modified_fnc_dscb(double*xx,double*pp); // global fit using the function above

/// default fit with gaussian in niter iteration of mean
void fit_gaussian(TH1F*& hrsp,
                  const double nsigma,
                  const double jtptmin,
                  const int niter,
                  const int verbose);

/// optional double sided crystal ball fit to response distributions
int fit_dscb(TH1F*& hrsp,
             const double nsigma,
             const double jtptmin,
             const int niter,
             const string alg,
             const int verbose);

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
  
  string         input     = cl.getValue<string> ("input");
  string         output    = cl.getValue<string> ("output",        "");
  double         nsigma    = cl.getValue<double> ("nsigma",       1.5);
  float          jtptmin   = cl.getValue<float>  ("jtptmin",      1.0);
  int            niter     = cl.getValue<int>    ("niter",          3);
  int            ndfmin    = cl.getValue<int>    ("ndfmin",         5);
  vector<string> algs      = cl.getVector<string>("algs",          "");
  int            verbose   = cl.getValue<int>    ("verbose",        0);
  int            fittype   = cl.getValue<int>    ("fittype",        0);
  bool           doAbsRsp  = cl.getValue<bool>   ("doAbsRsp",    true);
  bool           doRelRsp  = cl.getValue<bool>   ("doRelRsp",    true);
  bool           doEtaRsp  = cl.getValue<bool>   ("doEtaRsp",   false);
  bool           doPhiRsp  = cl.getValue<bool>   ("doPhiRsp",   false);
  bool           doFlavor  = cl.getValue<bool>   ("doFlavor",   false);
  string         histName  = cl.getValue<string> ("histName");
  bool           calibrated= cl.getValue<bool>   ("calibrated",   false);

  if (!cl.check()) return 0;
  cl.print();
  
  if (fittype<0 || fittype>2) {
    cout<<"ERROR: fittype not known, choose 0 for GAUSS, 1 for DSCB, 2 for Modify DSCB";return 0;
  }
  else if (0==fittype) cout<<"*** Fitting with distributions w/ GAUSS"<<endl;
  else if (1==fittype) cout<<"*** Fitting response distributions w/ DSCB"<<endl; 
  else cout<<"*** Fitting response distributions w/ modify DSCB"<<endl; 

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

  TFile* ofile = new TFile(output.c_str(),"RECREATE");
  if (!ofile->IsOpen()) { cout<<"Can't create "<<output<<endl; return 0; }

  TIter nextDir(ifile->GetListOfKeys());
  TKey* dirKey(0);
  while ((dirKey=(TKey*)nextDir())) {
    
    if (strcmp(dirKey->GetClassName(),"TDirectoryFile")!=0) continue;

    TDirectoryFile* idir = (TDirectoryFile*)dirKey->ReadObj();
    string alg(idir->GetName());
    
    if (algs.size()>0&&!contains(algs,alg)) continue;

    //if (0!=ofile->Get(idir->GetName())) {
    //  cout<<"directory '"<<alg<<"' exists already in "<<output<<", skip!"<<endl;
    //  continue;
    //}

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

      //
      // check if the histogram name contains a flavor
      // if it does and doFlavor is set to false, skip the histogram
      //
      if(!doFlavor) {
        vector<string> fields;
        JetInfo::process(histname,char('_'),fields);
        if(JetInfo::contains(flavors,fields[0])) {
          hrsp->Write();
          continue;
        }
      }

      //
      // Do not fit the RefPt or JetPt histograms
      //
      if(histname.find("RefPt")<5 || histname.find("JetPt")<5) {
        hrsp->Write();
        continue;
      }

      //
      // check if the histogram name contains any of the known options
      //
      if((!doAbsRsp || histname.find("AbsRsp")!=0) &&
         (!doRelRsp || histname.find("RelRsp")>5) &&
         (!doEtaRsp || histname.find("EtaRsp")>5) &&
         (!doPhiRsp || histname.find("PhiRsp")>5)) {
        hrsp->Write();
        continue;
      }

      //if(verbose>0) cout << "Attempting to fit " << histname << " ... " << endl;

      //if (histname.find("RelRsp")>5&&histname.find("AbsRsp")!=0) {
      //  hrsp->Write();
      //  continue;
      //}
      
      if (hrsp->Integral()>0.0) {
        int fitstatus(0);
        if (0==fittype) fit_gaussian(hrsp,nsigma,jtptmin,niter,verbose);
        else if (1==fittype) fitstatus = fit_dscb(hrsp,nsigma,jtptmin,niter,alg,verbose);
	else fit_modified_fnc_dscb(hrsp, alg, calibrated, histName);        

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
TH1F *hXaxis( TH1F* hist, double minX, double maxX)
{
  double *xbins = new double [hist->GetNbinsX()+1];
  int nbins{0};
  for(int i=0; i<hist->GetNbinsX(); i++){
    double x {hist->GetXaxis()->GetBinCenter(i+1)};
    if(x>=minX && x<=maxX){
      xbins[nbins]  = hist->GetXaxis()->GetBinLowEdge(i+1);
      xbins[nbins+1]= hist->GetXaxis()->GetBinUpEdge(i+1);
      ++nbins;
    }
  }

  TH1F *hnew = new TH1F ("hnew", "", nbins, xbins);
  for(int i=0; i<nbins+1; i++){
    hnew->SetBinContent(i+1, hist->GetBinContent(hist->FindBin(hnew->GetBinCenter(i+1))));
    hnew->SetBinError  (i+1, hist->GetBinError  (hist->FindBin(hnew->GetBinCenter(i+1))));
  }
  return hnew;
}

//______________________________________________________________________________
void fit_modified_fnc_dscb(TH1F*& hrsphist, string alg, bool calibrated, string histName){

  // do no write in the output file all the histogram
  TH1::AddDirectory(kFALSE);

  // select a particular algorithm
  if (alg!="ak5tauHPSlooseCombDBcorrAll") return;
  //if (alg!="ak5tauHPSlooseCombDBcorrOneProng0Pi0") return;
  //if (alg!="ak5tauHPSlooseCombDBcorrOneProng1Pi0") return;
  //if (alg!="ak5tauHPSlooseCombDBcorrTwoProng0Pi0") return;
  //if (alg!="ak5tauHPSlooseCombDBcorrTwoProng1Pi0") return;
  //if (alg!="ak5tauHPSlooseCombDBcorrThreeProng0Pi0") return;
  //if (alg!="ak5tauHPSlooseCombDBcorrThreeProng1Pi0") return;
  //
  if (alg=="ak5tauHPSlooseCombDBcorrOneProng2Pi0") return; // =ak5tauHPSlooseCombDBcorrOne

  TH1F *hrsp = (TH1F*)hrsphist->Clone();

  string histname{hrsp->GetName()};

  // select the relative response histograms
  if ( histname.find(histName) == string::npos) return;

  // select a particular histogram
  if ( histname!="RelRsp_JetEta1.7to1.9_RefPt500to3000") return;

  cout << "\nModified CB processing "<<alg<<": " << histname << endl;

  // to avoid that the pol diverge at low value, set empty bin to 0.5+/-0.5
  if(hrsp->Integral()<300000){
    for(int i=1; i<=hrsp->GetNbinsX(); i++) if(hrsp->GetBinContent(i)==0){
      hrsp->SetBinContent(i, 0.5); hrsp->SetBinError(i, 0.5);
    }
  }

  ////////////////////////
  ////////////////////////
  // start fit procedure
  ////////////////////////
  ////////////////////////

  // find the max of the hist that correspond to the tau pic btw [0, 1.05]   
  double maxBin{0};
  int binmax = -1;
  for ( int i = 0; i <= hrsp->FindBin(1.05); ++i ){
    double newMaxBin = hrsp->GetBinContent(i);
    if ( newMaxBin > maxBin ) { 
      maxBin = newMaxBin; binmax = i; 
    }
  } 

  // pre-fit function 
  TF1* testfit{0}; 
  TF1 *gausLeft{0};
  TF1 *expLeft{0};
  TF1 *polLeft{0};
  // global fit function
  TF1* modified_fdscb{0};

  // fit definition
  string gaus = "[0]*TMath::Exp(-0.5*TMath::Power((x-[1])/[2],2.))"; 
  string expo = "[0]*TMath::Exp([1]*(x - [2]))";
  string pol = "[0]+[1]*x+[2]*TMath::Power(x,2)+[3]*TMath::Power(x,3)+[4]*TMath::Power(x,4)+[5]*TMath::Power(x,5)+[6]*TMath::Power(x,6)+[7]*TMath::Power(x,7)";

  // global fit maximun parameters and name
  int maxPar{31};
  vector<string> parName {"norm G1", "mean G1", "sigma G1 left", "norm Exp left", "slope Exp left", 
    "p0 left", "p1 left", "p2 left", "p3 left", "p4 left", "p5 left", "p6 left", "p7 left", 
    "R1", "R2", "sigma G1 right", "norm G2", "mean G2", "sigma G2", "norm G3", "mean G3", "sigma G3",
    "p0 right", "p1 right", "p2 right", "p3 right", "p4 right", "p5 right", "p6 right", "p7 right", "xbinmax" };

  // reduce chisquare and Kolmogorov-Smir test
  double chi2, ks;
  // map chi2/KS->fit
  map<double, TH1F*> mapChi2Fit;
  map<double, TH1F*> mapChi2FitB;
  map<double, TH1F*> mapChi2FitO;
  map<double, TH1F*> mapChi2FitM;
  map<double, TH1F*> mapChi2FitG;
  map<double, TH1F*> mapKsFit;  // the best out of the 4 cases
  map<double, TH1F*> mapKsFitB; // best from Blue (case 4)
  map<double, TH1F*> mapKsFitO; // best from Orange (case 3)
  map<double, TH1F*> mapKsFitM; // best from Magenta(case 2)
  map<double, TH1F*> mapKsFitG; // best from Green (case 1)

  double xbinmax = hrsp->GetXaxis()->GetBinUpEdge(binmax); // x position of the max bin
  double ybinmax = hrsp->GetBinContent(binmax);
  //int bindown = binmax - 5;
  int bindown = binmax - 4;
  double minGaus = hrsp->GetXaxis()->GetBinLowEdge(bindown); // min bin of the gauss (R2)

  //////////////////////////////////////////////////////////////////////////
  // left side Gauss pre-fit
  //////////////////////////////////////////////////////////////////////////
  cout<<"\nProcessing fit Gauss left..."<< endl;
  testfit = new TF1("testfit", gaus.data(), 0., 2.);
  testfit->SetParameter(0, hrsp->GetBinContent(binmax));
  testfit->SetParameter(1, xbinmax);
  testfit->FixParameter(1, xbinmax);
  testfit->SetParameter(2, 0.10);
  testfit->SetParLimits(2, 0., 1.e+1); 
  gausLeft = new TF1("gausLeft", gaus.data(), 0., 2.);
  do {
    hrsp->Fit(testfit, "Q0", "", minGaus, xbinmax); // Q: quiet mode; 0: do not draw default canvas

    TH1F * hrspXaxis = hXaxis(hrsp, minGaus, xbinmax); // set axis for KS
    TF1 *fit=hrsp->GetFunction("testfit");
    fit->SetNpx(hrspXaxis->GetNbinsX());
    ks=hrspXaxis->KolmogorovTest((TH1D*)fit->GetHistogram()); 
    chi2 = testfit->GetChisquare()/max(1, testfit->GetNDF()); 

    --bindown;
    minGaus = hrsp->GetXaxis()->GetBinLowEdge(bindown);
    if ( chi2 < 5. && ks>0.05) {
      gausLeft->SetParameter(0, testfit->GetParameter(0));
      gausLeft->SetParameter(1, testfit->GetParameter(1));
      gausLeft->SetParameter(2, testfit->GetParameter(2));	  
    }
  } while ( chi2 < 5. && minGaus > 0. && ks>0.05 );
  // sometimes chi2 may exceed 5 in 1st interation->set defauld value
  if(gausLeft->GetParameter(0)==0) gausLeft->SetParameter(0, testfit->GetParameter(0));
  if(gausLeft->GetParameter(1)==0) gausLeft->SetParameter(1, testfit->GetParameter(1));
  if(gausLeft->GetParameter(2)==0) gausLeft->SetParameter(2, testfit->GetParameter(2));

  ++bindown;
  minGaus = hrsp->GetXaxis()->GetBinCenter(bindown);

 
  //////////////////////////////////////////////////////////////////////////
  // left side expo pre-fit
  //////////////////////////////////////////////////////////////////////////
  cout<<"\nProcessing fit expo left..."<< endl;
  double minExpLeft = hrsp->GetXaxis()->GetBinLowEdge(bindown - 3); // min bin of the expo (R1)
  expLeft = new TF1("expLeft", expo.data(), 0., 2.);
  if ( minGaus > 0 && hrsp->Integral(hrsp->FindBin(0), hrsp->FindBin(minGaus)) > 0. ){
    delete testfit;
    testfit = new TF1("testfit", expo.data(), 0., 2.);
    testfit->SetParameter(0, gausLeft->Eval(minGaus));
    testfit->SetParameter(1, 0.1);
    testfit->SetParameter(2, minGaus);
    testfit->FixParameter(2, minGaus);
    do {
      hrsp->Fit(testfit, "Q0", "", minExpLeft, minGaus);
      --bindown;
      minExpLeft = hrsp->GetXaxis()->GetBinCenter(bindown);
      chi2 = testfit->GetChisquare()/max(1, testfit->GetNDF()); 
      if ( chi2 < 5. ) {
	expLeft->SetParameter(0, testfit->GetParameter(0));
	expLeft->SetParameter(1, testfit->GetParameter(1));
	expLeft->SetParameter(2, testfit->GetParameter(2));
      }
    }
    while ( chi2 < 5. && minExpLeft > 0 );
    // sometimes chi2 may exceed 5 in 1st interation->se t default value
    if(expLeft->GetParameter(0)==0) expLeft->SetParameter(0, testfit->GetParameter(0));
    if(expLeft->GetParameter(1)==0) expLeft->SetParameter(1, testfit->GetParameter(1));
    if(expLeft->GetParameter(2)==0) expLeft->SetParameter(2, testfit->GetParameter(2));

    ++bindown;
    minExpLeft = hrsp->GetXaxis()->GetBinCenter(bindown);
  } else {
    cout<<"no point or and/or range for the expo left fit\n!!";
  }


  //////////////////////////////////////////////////////////////////////////
  // left side Pol pre-fit
  //////////////////////////////////////////////////////////////////////////
  cout<<"\nProcessing fit pol left...\n";
  if( minExpLeft > 0 && hrsp->Integral(hrsp->FindBin(0), hrsp->FindBin(minExpLeft)) > 0. ){
    polLeft = new TF1("polLeft", pol.c_str(), 0, minExpLeft);
  } else {
    cout<<"no point or and/or range for the pol7 left fit!!\n";
  }


  ///////////////////////////////////////////////////
  // initalise the global fit
  ///////////////////////////////////////////////////

  double mu1=gausLeft->GetParameter(1); 
  double mu2=0; 
  double mu3=0.15; 
  double r1=minGaus- minExpLeft; 
  double r2=xbinmax - minGaus; 

  vector<double> rd_unfix {mu1, mu2, mu3, r1, r2, 0., 0., 0., 0., 0.};
  vector<double> rd_fixG1 {mu1, mu2, mu3, r1, r2, 1., 0., 0., 0., 0.};
  vector<double> rd_fixG2 {mu1, mu2, mu3, r1, r2, 0., 1., 0., 0., 0.};
  vector<double> rd_fixG3 {mu1, mu2, mu3, r1, r2, 0., 0., 1., 0., 0.};
  vector<double> rd_fixR1 {mu1, mu2, mu3, r1, r2, 0., 0., 0., 1., 0.};
  vector<double> rd_fixR2 {mu1, mu2, mu3, r1, r2, 0., 0., 0., 0., 1.};

  vector<vector<double>> mrd;
  mrd.push_back(rd_unfix);
  mrd.push_back(rd_fixG1);
  mrd.push_back(rd_fixG2);
  mrd.push_back(rd_fixG3);
  mrd.push_back(rd_fixR1);
  mrd.push_back(rd_fixR2);


  /////////////////////////////////////////////////
  // try "ideal" + Random value 
  /////////////////////////////////////////////////

  TH1F* hrspClone;

  TRandom *random = new TRandom3();

  cout<<"\nStart Random...\n";
  for(int i=1; i<2; i++){
    cout<<"\ntest number: "<<i<<"/99"<<endl;

    modified_fdscb = new TF1("modified_fdscb", modified_fnc_dscb, 0., 2., maxPar);
    for(int par=0; par<maxPar; par++){ modified_fdscb->SetParameter(par, 0); modified_fdscb->SetParName(par, parName[par].c_str()); }
    modified_fdscb->SetLineWidth(2);

    ////////////////
    // fit case 4
    ////////////////
    modified_fdscb->SetLineColor(kBlue); 

    if(gausLeft){
      double stepG1{0.1*random->Rndm()};
      double stepR2{0.1*random->Rndm()};

      modified_fdscb->SetParameter(0, gausLeft->GetParameter(0)); // norm
      if(i<6){ // "optimal value"
	if(mrd[i][5]==0.) modified_fdscb->SetParameter(1, mrd[i][0]); // mean
	else modified_fdscb->FixParameter(1, mrd[i][0]); 
	if(mrd[i][9]==0.) modified_fdscb->SetParameter(14, mrd[i][4]); // R2
	else modified_fdscb->FixParameter(14, mrd[i][4]);     
      }
      else if(i>=6 && i<47){ // random up from optimal value
	if(mrd[0][5]==0.) modified_fdscb->SetParameter(1, mrd[0][0]+stepG1); //mean
	else modified_fdscb->FixParameter(1, mrd[0][0]+stepG1); 
	if(mrd[0][9]==0.) modified_fdscb->SetParameter(14, mrd[0][4]+stepR2); // R2
	else modified_fdscb->FixParameter(14, mrd[0][4]+stepR2);     
      }
      else{ // random down from optimal value
	if(mrd[0][5]==0.) modified_fdscb->SetParameter(1, mrd[0][0]-stepG1); 
	else modified_fdscb->FixParameter(1, mrd[0][0]-stepG1); 
	if(mrd[0][9]==0.) modified_fdscb->SetParameter(14, mrd[0][4]-stepR2); // R2
	else modified_fdscb->FixParameter(14, mrd[0][4]-stepR2);     
      }
      modified_fdscb->SetParameter(2, gausLeft->GetParameter(2)); // sigma
      modified_fdscb->SetParLimits(2, 0., 10.);
      modified_fdscb->SetParLimits(14, 0., xbinmax);
    }else{
      for(int i=0; i<=14; i++) modified_fdscb->FixParameter(i, 0); 
    }	
    if(expLeft){  
      double stepR1{0.1*random->Rndm()};
      modified_fdscb->FixParameter(3, 0.); // norm
      modified_fdscb->SetParameter(4, expLeft->GetParameter(1)); // slope
      modified_fdscb->SetParLimits(4, 0., 1.e+4); 
      if(i<6){ // "optimal value"
	if(mrd[i][8]==0.) modified_fdscb->SetParameter(13, mrd[i][3]); // R1
	else modified_fdscb->FixParameter(13, mrd[i][3]); 
      }
      else if (i>=6 && i<47){ // random up from optimal value
	if(mrd[0][8]==0.) modified_fdscb->SetParameter(13, mrd[0][3]+stepR1); // R1
	else modified_fdscb->FixParameter(13, mrd[0][3]+stepR1); 
      }
      else{ // random down from optimal value
	if(mrd[0][8]==0.) modified_fdscb->SetParameter(13, mrd[0][3]-stepR1); // R1
	else modified_fdscb->FixParameter(13, mrd[0][3]-stepR1); 
      }
      modified_fdscb->SetParLimits(13, 0., 1.);
    }else{
      for(int i=3; i<=14; i++) modified_fdscb->FixParameter(i, 0); 
    }	
    if(polLeft){
      for(int i=1; i<=7; i++)  modified_fdscb->SetParameter(i+5, polLeft ->GetParameter(i)); 
      modified_fdscb->FixParameter(5, 0);  // P0 left
    }else{
      cout<<"no pol left: fix pol left parameters/range to 0!!"<<endl;
      for(int i=5; i<=13; i++) modified_fdscb->FixParameter(i, 0); 
    }
    //	
    double stepG2{0.1*random->Rndm()};
    double stepG3{0.1*random->Rndm()};
    if(i<6){ // "optimal value"
      if(mrd[i][6]==0.) modified_fdscb->SetParameter(17, mrd[i][1]); // G2
      else modified_fdscb->FixParameter(17, mrd[i][1]); 
      if(mrd[i][7]==0.) modified_fdscb->SetParameter(20, mrd[i][2]); // G3
      else modified_fdscb->FixParameter(20, mrd[i][2]);
    }
    else if(i>=6 && i<47){ // random up from optimal value
      if(mrd[0][6]==0.) modified_fdscb->SetParameter(17, mrd[0][1]+stepG2); 
      else modified_fdscb->FixParameter(17, mrd[0][1]+stepG2); 
      if(mrd[0][7]==0.) modified_fdscb->SetParameter(20, mrd[0][2]+stepG3);
      else modified_fdscb->FixParameter(20, mrd[0][2]+stepG3);
    }
    else{ // random down from optimal value
      if(mrd[0][6]==0.) modified_fdscb->SetParameter(17, mrd[0][1]-stepG2); 
      else modified_fdscb->FixParameter(17, mrd[0][1]+stepG2); 
      if(mrd[0][7]==0.) modified_fdscb->SetParameter(20, mrd[0][2]-stepG3);
      else modified_fdscb->FixParameter(20, mrd[0][2]+stepG3);
    }
    modified_fdscb->SetParameter(15, 0.05);  // sigma gauss 1
    modified_fdscb->SetParLimits(15, 0, 10);  
    modified_fdscb->SetParameter(16, 0.01*ybinmax); // gauss 2 norm
    modified_fdscb->SetParLimits(16, 0., ybinmax); 
    modified_fdscb->SetParLimits(17, 0., 2.); 
    modified_fdscb->SetParameter(18, 0.20);  // sigma
    modified_fdscb->SetParLimits(18, 0., 10.);  
    modified_fdscb->SetParameter(19, 0.10*ybinmax); // gauss 3 norm
    modified_fdscb->SetParLimits(19, 0., ybinmax);  
    modified_fdscb->SetParLimits(20, 0., 2.); 
    modified_fdscb->SetParameter(21, 0.10); // sigma
    modified_fdscb->SetParLimits(21, 0., 10.);  
    modified_fdscb->SetParameter(22, 0.5); 
    modified_fdscb->SetParLimits(22, 0., ybinmax);  
    modified_fdscb->SetParameter(30, xbinmax); // xbinmax
    //
    hrsp->Fit("modified_fdscb", "R"); // use Q for quiet 
    testfit = (TF1*) hrsp->GetListOfFunctions()->Last();
    testfit->SetNpx(hrsp->GetNbinsX());
    ks=hrsp->KolmogorovTest(testfit->GetHistogram()); 
    chi2=testfit->GetChisquare()/max(1, testfit->GetNDF()); 
    cout<<"ks/chi2 case4: "<<ks<<"/"<<chi2<<endl;
    hrspClone = (TH1F*) hrsp->Clone(); 
    if( ks>1e-4 && fabs(ks-1)>0.1) mapKsFitB[ks]=hrspClone; // 0 and 1 are bad ks values 1e-7!!
    if(chi2<50) mapChi2FitB[chi2]=hrspClone;

    ////////////////
    // fit case 3
    ////////////////
    modified_fdscb->SetLineColor(kOrange); 
    if(polLeft)for(int i=6; i<=12; i++) modified_fdscb->FixParameter(i, 0); // pol7/range left=cst p0
    for(int i=23; i<=29; i++) modified_fdscb->FixParameter(i, 0); // pol7 right= cst p0 
    hrsp->Fit("modified_fdscb", "R"); 
    testfit = (TF1*) hrsp->GetListOfFunctions()->Last();
    testfit->SetNpx(hrsp->GetNbinsX());
    ks=hrsp->KolmogorovTest(testfit->GetHistogram()); 
    chi2=testfit->GetChisquare()/max(1, testfit->GetNDF()); 
    cout<<"ks/chi2 case3: "<<ks<<"/"<<chi2<<endl;
    hrspClone = (TH1F*) hrsp->Clone(); 
    if( ks>1e-4 && fabs(ks-1)>0.1) mapKsFitO[ks]=hrspClone; 
    if(chi2<50) mapChi2FitO[chi2]=hrspClone;

    ////////////////
    // fit case 2
    ////////////////
    modified_fdscb->SetLineColor(kMagenta); 
    for(int i=19; i<=20; i++) modified_fdscb->FixParameter(i, 0); // gaus3 right
    modified_fdscb->FixParameter(21, 1); // sigma !=0 
    hrsp->Fit("modified_fdscb", "R"); 
    testfit = (TF1*) hrsp->GetListOfFunctions()->Last();
    testfit->SetNpx(hrsp->GetNbinsX());
    ks=hrsp->KolmogorovTest(testfit->GetHistogram()); 
    chi2=testfit->GetChisquare()/max(1, testfit->GetNDF()); 
    cout<<"ks/chi2 case2 "<<ks<<"/"<<chi2<<endl;
    hrspClone = (TH1F*) hrsp->Clone(); 
    if( ks>1e-4 && fabs(ks-1)>0.1) mapKsFitM[ks]=hrspClone; 
    if(chi2<50) mapChi2FitM[chi2]=hrspClone;

    ////////////////
    // fit case 1
    ////////////////
    modified_fdscb->SetLineColor(kGreen); 
    for(int i=16; i<=17; i++) modified_fdscb->FixParameter(i, 0); // gaus2 right
    modified_fdscb->FixParameter(18, 1); // sigma !=0 
    hrsp->Fit("modified_fdscb", "R"); 
    testfit = (TF1*) hrsp->GetListOfFunctions()->Last();
    testfit->SetNpx(hrsp->GetNbinsX());
    ks=hrsp->KolmogorovTest(testfit->GetHistogram()); 
    chi2=testfit->GetChisquare()/max(1, testfit->GetNDF()); 
    cout<<"ks/chi2 case1 "<<ks<<"/"<<chi2<<endl;
    hrspClone = (TH1F*) hrsp->Clone(); 
    if( ks>1e-4 && fabs(ks-1)>0.1) mapKsFitG[ks]=hrspClone; 
    if(chi2<50) mapChi2FitG[chi2]=hrspClone;

  } // random
  cout<<"endRandom\n";
  cout<<"retriving best fit from ks or chi2 map..."<<endl;

  if(mapKsFitB.size()!=0)mapKsFit[mapKsFitB.rbegin()->first]=mapKsFitB.rbegin()->second; 
  if(mapKsFitO.size()!=0)mapKsFit[mapKsFitO.rbegin()->first]=mapKsFitO.rbegin()->second; 
  if(mapKsFitM.size()!=0)mapKsFit[mapKsFitM.rbegin()->first]=mapKsFitM.rbegin()->second; 
  if(mapKsFitG.size()!=0)mapKsFit[mapKsFitG.rbegin()->first]=mapKsFitG.rbegin()->second; 
  //
  if(mapChi2FitB.size()!=0)mapChi2Fit[mapChi2FitB.begin()->first]=mapChi2FitB.begin()->second; 
  if(mapChi2FitO.size()!=0)mapChi2Fit[mapChi2FitO.begin()->first]=mapChi2FitO.begin()->second; 
  if(mapChi2FitM.size()!=0)mapChi2Fit[mapChi2FitM.begin()->first]=mapChi2FitM.begin()->second; 
  if(mapChi2FitG.size()!=0)mapChi2Fit[mapChi2FitG.begin()->first]=mapChi2FitG.begin()->second; 

  bool goodFit{false};

  if(mapKsFit.size()!=0){
    double maxKs{mapKsFit.rbegin()->first};
    if(maxKs<0.5) { 
      goodFit=false; //hrsphist= (TH1F*)  mapKsFit.rbegin()->second->Clone();
      cout<<"failed Ks>0.5!!, Ks: "<<mapKsFit.rbegin()->first<<endl;
    }
    else {
      goodFit=true; hrsphist= (TH1F*)  mapKsFit.rbegin()->second->Clone();
      cout<<"pass Ks>0.5!!, Ks: "<<mapKsFit.rbegin()->first<<endl;
    }
  }
  else if(mapChi2Fit.size()!=0){
      double minChi2{mapChi2Fit.begin()->first};
      if(minChi2>5) { 
	goodFit=false; //hrsphist= (TH1F*)  mapChi2Fit.begin()->second->Clone();
	cout<<"failed Chi2<5!!, Chi2: "<<mapChi2Fit.begin()->first<<endl;
      }
      else {
	goodFit=true; hrsphist= (TH1F*)  mapChi2Fit.begin()->second->Clone();
	cout<<"pass Chi2<5!!, Chi2: "<<mapChi2Fit.begin()->first<<endl;
      }
    }
  else{ goodFit=false; cout<<"no fit at all, keeping original histogram!!\n"; }

  //hrsphist->Write();

  // save fit plot
  TCanvas *c = new TCanvas("c", "", 700, 700);
  c->Divide(1,2);
  c->cd(1); hrsphist->Draw();
  c->cd(2); gPad->SetLogy(); hrsphist->Draw();
  string savePath;

  if( goodFit){
    if(!calibrated) savePath="/home/calpas/JetMETAnalysis/CMSSW_7_4_10_patch2/src/JetMETAnalysis/JetAnalyzers/test/fitPlots/uncalibrated/"+alg+"/pass/"+hrsphist->GetName()+".png";
    else savePath="/home/calpas/JetMETAnalysis/CMSSW_7_4_10_patch2/src/JetMETAnalysis/JetAnalyzers/test/fitPlots/calibrated/"+alg+"/pass/"+hrsphist->GetName()+".png";
  }
  else{
    if(!calibrated) savePath="/home/calpas/JetMETAnalysis/CMSSW_7_4_10_patch2/src/JetMETAnalysis/JetAnalyzers/test/fitPlots/uncalibrated/"+alg+"/failed/"+hrsphist->GetName()+".png";
    else savePath="/home/calpas/JetMETAnalysis/CMSSW_7_4_10_patch2/src/JetMETAnalysis/JetAnalyzers/test/fitPlots/calibrated/"+alg+"/failed/"+hrsphist->GetName()+".png";
  }

  c->Print((savePath).c_str()); cout<<"fit plot saved at: "<< savePath <<endl;

}

double exp_fcn(double x, double norm, double slope){
  return norm*TMath::Exp(slope*x);
}

double gauss_fcn(double x, double norm, double mean, double sigma){
  return norm*TMath::Exp(-0.5*TMath::Power((x - mean)/sigma, 2.));
}

double pol7_fcn(double x, double p0, double p1, double p2, double p3, double p4, double p5, double p6, double p7){
  return p0 + p1*x + p2*TMath::Power(x,2) +
    p3*TMath::Power(x,3) + p4*TMath::Power(x,4) +
    p5*TMath::Power(x,5) + p6*TMath::Power(x,6) +
    p7*TMath::Power(x,7);
}

double sumGausPol7(double *xx, double *pp){
  double x = xx[0];

  double normGaus1Left  = pp[0];
  double meanGaus1      = pp[1];
  //double sigGaus1_left  = pp[2]; 
  double valGaus1_at_meanGaus1 = normGaus1Left;

  double normGaus2      = pp[16];
  double meanGaus2      = meanGaus1 + pp[17];  
  double sigGaus2       = pp[18]; 
  double valGaus2_at_meanGaus1 = gauss_fcn(meanGaus1, normGaus2, meanGaus2, sigGaus2);
  double valGaus2       = gauss_fcn(x, normGaus2, meanGaus2, sigGaus2);

  double normGaus3      = pp[19];
  double meanGaus3      = meanGaus1 + pp[20];  
  double sigGaus3       = pp[21]; 
  double valGaus3_at_meanGaus1 = gauss_fcn(meanGaus1, normGaus3, meanGaus3, sigGaus3);
  double valGaus3       = gauss_fcn(x, normGaus3, meanGaus3, sigGaus3);

  double p0             = pp[22]; // pol7 (P0 not use, discontinuity!!)
  double p1             = pp[23];
  double p2             = pp[24]; 
  double p3             = pp[25];
  double p4             = pp[26]; 
  double p5             = pp[27]; 
  double p6             = pp[28]; 
  double p7             = pp[29]; 
  double valPol_at_meanGaus1 = pol7_fcn(meanGaus1, p0, p1, p2, p3, p4, p5, p6, p7);
  double valPol         = pol7_fcn(x, p0, p1, p2, p3, p4, p5, p6, p7);

  double normGaus1Right = valGaus1_at_meanGaus1 - (valGaus2_at_meanGaus1 + valGaus3_at_meanGaus1 + valPol_at_meanGaus1);
  double sigGaus1_right = pp[15]; 
  double valGaus1_right = gauss_fcn(x, normGaus1Right, meanGaus1, sigGaus1_right);

  return (valGaus1_right + valGaus2 + valGaus3 + valPol);
}

// global fit
double modified_fnc_dscb(double*xx,double*pp)
{
  double x = xx[0];

  double xbinmax        = pp[30]; 

  // left
  double normGaus1      = pp[0];   // G1 
  double meanGaus1      = pp[1];  
  double sigGaus1_left  = pp[2];   
  //double normExp_left_  = pp[3];   // Exp (morm not use, discontinuity!!)
  double slopeExp_left  = pp[4];   // 
  //double p0_left_       = pp[5];   // Pol7 (P0 not use, discontinuity!!)
  double p1_left        = pp[6];
  double p2_left        = pp[7]; 
  double p3_left        = pp[8];
  double p4_left        = pp[9]; 
  double p5_left        = pp[10]; 
  double p6_left        = pp[11]; 
  double p7_left        = pp[12]; 
  double R2             = xbinmax - pp[14]; // min gauss1
  double R1             = R2 - pp[13]; // min exp 

  // right 
  //double sigGaus1_right  = pp[15];
  //double normGaus2       = pp[16]; // G2
  //double meanGaus2       = meanGaus1 + pp[17];
  //double sigGaus2        = pp[18];
  //double normGaus3       = pp[19]; // G3
  //double meanGaus3       = meanGaus1 + pp[20];
  //double sigGaus3        = pp[21];
  //double p0_right        = pp[22]; // pol7 (P0 not use, discontinuity!!)
  //double p1_right        = pp[23];
  //double p2_right        = pp[24]; 
  //double p3_right        = pp[25];
  //double p4_right        = pp[26]; 
  //double p5_right        = pp[27]; 
  //double p6_right        = pp[28]; 
  //double p7_right        = pp[29]; 

  // remove discontinuity at x=R2: solve exp=gauss for norm exp and replace R2 by (meanGaus1-R2*R2)
  double normExp_left = gauss_fcn(R2, normGaus1, meanGaus1, sigGaus1_left)/TMath::Exp(slopeExp_left*R2);

  // remove discontinuity at x=R1: solve pol7=exp for p0 left 
  double p0_left = exp_fcn(R1, normExp_left, slopeExp_left) -
    ( p1_left*R1 + p2_left*pow(R1,2) +
      p3_left*pow(R1,3) + p4_left*pow(R1,4) +
      p5_left*pow(R1,5) + p6_left*pow(R1,6) +
      p7_left*pow(R1,7) ) ;

  double result = 0.;

  // left
  if      ( x < R1        ) result = pol7_fcn(x, p0_left, p1_left, p2_left, p3_left, p4_left, p5_left, p6_left, p7_left);
  else if ( x < R2        ) result = exp_fcn(x, normExp_left, slopeExp_left);
  else if ( x < meanGaus1 ) result = gauss_fcn(x, normGaus1, meanGaus1, sigGaus1_left);
  // right
  else  result = sumGausPol7(xx, pp);

  return result;
}


//______________________________________________________________________________
int fit_dscb(TH1F*& hrsp,
             const double nsigma,
             const double jtptmin,
             const int niter,
             const string alg,
             const int verbose)
{
  if (0==hrsp) {
    cout<<"ERROR: Empty pointer to fit_dscb()"<<endl;return -1;
  }

  // first use a gaussian to constrain crystal ball gaussian core

  fit_gaussian(hrsp,nsigma,jtptmin,niter,verbose);

  TF1* fgaus = hrsp->GetFunction("fgaus");
  if (0==fgaus) {
    hrsp->GetListOfFunctions()->Delete();
    return -1;
  }

  // implementation of the low pt bias threshold

  string histname = hrsp->GetName();
  double ptRefMax(1.0),rspMax(0.0);

  int pos1     = histname.find("RefPt");
  int pos2     = histname.find("to",pos1);
  string ss    = histname.substr(pos1+5,pos2);
  if (from_string(ptRefMax,ss,std::dec)) {
    if (histname.find("RelRsp")==0)
      rspMax = jtptmin/ptRefMax;
    if (histname.find("AbsRsp")==0)
      rspMax = jtptmin-ptRefMax;
  }
  
  vector<double> vv; //Added by Alexx
  double fitrange_min(0.0);
  if (alg.find("pf")!=string::npos) fitrange_min = std::max(rspMax,0.3);
  else if (alg.find("PF")!=string::npos) fitrange_min = std::max(rspMax,0.3);
  else {
     double first_nonzero_bin = 0.0;
     for(int ibin=1; ibin<=hrsp->GetNbinsX();ibin++) {
        if(first_nonzero_bin==0 && hrsp->GetBinContent(ibin)!=0) {
           first_nonzero_bin = hrsp->GetBinCenter(ibin);
           break;
        }
     }
     vv.push_back(rspMax);
     vv.push_back(hrsp->GetXaxis()->GetXmin());
     vv.push_back(0.2);
     vv.push_back(first_nonzero_bin);
     fitrange_min = *std::max_element(vv.begin(),vv.end());
  }
  //else fitrange_min = std::max(rspMax,0.2);
  double fitrange_max = 1.7;
  if (histname.find("EtaRsp")==0 ||
      histname.find("PhiRsp")==0) {
    fitrange_min = -0.2;
    fitrange_max = 0.2;
  }


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

    fdscb->SetParLimits(4,0.,100.);//25.);
    fdscb->SetParLimits(6,0.,100.);//25.);

    if(hrsp->GetEntries()==0) {
       if(verbose>0)
         cout << "Warning in <Fit>: Fit data is empty" << endl
              << "hrsp->GetName(): " << hrsp->GetName() << endl;
       return -1;
    }
    else {
       fitstatus = hrsp->Fit(fdscb,"RQB0+");
    }
    
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
                  const int niter,
                  const int verbose)
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
  int nbins = 50;//100;
  TSpectrum *spec = new TSpectrum(2);
  if(nbins < 100) spec->Search(hrsp,6,"nobackground nodraw goff"); //turn off background removal when nbins too small
  else spec->Search(hrsp,6,"nodraw goff");
  Double_t* xpos = spec->GetPositionX();
  //Double_t* ypos = spec->GetPositionY();
  Double_t p = xpos[0];
  //Double_t ph = ypos[0];
  //std::cout << "peak: " << p << std::endl;
  //std::cout << "peak height: " << ph << std::endl;
  peak = p;

  double sigma = rms;
  int pos1     = histname.find("RefPt");
  int pos2     = histname.find("to",pos1);
  string ss    = histname.substr(pos1+5,pos2);
  if (from_string(ptRefMax,ss,std::dec)) {
    if (histname.find("RelRsp")==0)
      rspMax = jtptmin/ptRefMax;
    if (histname.find("AbsRsp")==0)
      rspMax = jtptmin-ptRefMax;
    if (histname.find("EtaRsp")==0 || histname.find("PhiRsp")==0)
      rspMax = -1.0;
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
    if(hrsp->GetEntries()==0) {
       if(verbose>0)
         cout << "Warning in <Fit>: Fit data is empty" << endl
              << "hrsp->GetName(): " << hrsp->GetName() << endl;
       return;
    }
    else {
       fitstatus = hrsp->Fit(fitfnc,"RQ0");
    }
    delete fitfnc;
    fitfnc = hrsp->GetFunction("fgaus");
    //fitfnc->ResetBit(TF1::kNotDraw);
    if (fitfnc) {
       norm  = fitfnc->GetParameter(0);
       peak  = fitfnc->GetParameter(1);
       sigma = fitfnc->GetParameter(2);
    }
  }
  if(hrsp->GetFunction("fgaus")==0)
    {
      cout << "No function recorded in histogram " << hrsp->GetName() << endl;
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

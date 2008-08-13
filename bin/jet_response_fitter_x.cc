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

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// define local functions
////////////////////////////////////////////////////////////////////////////////

/// check if a vector of strings contains a certain element
bool contains(const vector<string>& collection,const string& element);
void adjust_fitrange(TH1* h,double& min,double& max);


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
  
  string         input  = cl.getValue<string> ("input");
  double         nsigma = cl.getValue<double> ("nsigma",1.5);
  int            niter  = cl.getValue<int>    ("niter",   3);
  vector<string> algs   = cl.getVector<string>("algs",   "");

  if (!cl.check()) return 0;
  cl.print();
  
  
  //
  // open input file and loop over input directories (=algorithms)
  //
  TFile* ifile = new TFile(input.c_str(),"UPDATE");
  if (!ifile->IsOpen()) { cout<<"Can't open "<<input<<endl; return 0; }

  TFile* ofile = new TFile("tmp.root","RECREATE");
  if (!ofile->IsOpen()) { cout<<"Can't create tmp.root"<<endl; return 0; }

  TIter nextDir(ifile->GetListOfKeys());
  TKey* dirKey(0);
  while ((dirKey=(TKey*)nextDir())) {
    
    if (strcmp(dirKey->GetClassName(),"TDirectoryFile")!=0) continue;

    TDirectoryFile* idir = (TDirectoryFile*)dirKey->ReadObj();
    string alg(idir->GetName());
    
    TDirectoryFile* odir = (TDirectoryFile*)ofile->mkdir(idir->GetName());
    odir->cd();
    
    cout<<alg<<" ... "<<endl;
    

    //
    // loop over response histogram and fit them with a Gaussian (iteratively)
    //
    TIter nextHist(idir->GetListOfKeys());
    TKey* histKey(0);
    while ((histKey=(TKey*)nextHist())) {
      if (strcmp(histKey->GetClassName(),"TH1F")!=0) continue;

      TH1F* hrsp = (TH1F*)histKey->ReadObj();
      string histname(hrsp->GetName());
      
      if (histname.find("RelRsp")!=0&&histname.find("AbsRsp")!=0) continue;
      if (algs.size()>0&&!contains(algs,alg)) continue;

      double integral = hrsp->Integral();
      double mean     = hrsp->GetMean();
      double rms      = hrsp->GetRMS();
      if (integral>0.0) {
	double norm  = hrsp->GetMaximumStored();
	double peak  = mean;
	double sigma = rms;
	double xmin  = hrsp->GetXaxis()->GetXmin();
	double xmax  = hrsp->GetXaxis()->GetXmax();
	TF1* fitfnc(0);
	for (int iiter=0;iiter<niter;iiter++) {
	  double fitrange_min = std::max(xmin,peak-nsigma*sigma);
	  double fitrange_max = std::min(xmax,peak+nsigma*sigma);
	  adjust_fitrange(hrsp,fitrange_min,fitrange_max);
	  fitfnc = new TF1("fit","gaus",fitrange_min,fitrange_max);
	  fitfnc->SetParNames("N","#mu","#sigma");
	  fitfnc->SetParameter(0,norm);
	  fitfnc->SetParameter(1,peak);
	  fitfnc->SetParameter(2,sigma);
	  hrsp->GetListOfFunctions()->Delete();
	  hrsp->Fit(fitfnc,"RQ0");
	  hrsp->GetFunction("fit")->ResetBit(TF1::kNotDraw);
	  norm  = fitfnc->GetParameter(0);
	  peak  = fitfnc->GetParameter(1);
	  sigma = fitfnc->GetParameter(2);
	}
	if (0!=fitfnc&&fitfnc->GetNDF()<=5) {
	  cout<<"NDOF(FITFNC)="<<fitfnc->GetNDF()<<" FOR "<<hrsp->GetName()<<endl;
	  hrsp->GetListOfFunctions()->Delete();
	}
      }
      else {
	cout<<"NOT ENOUGH ENTRIES FOR "<<hrsp->GetName()<<endl;
      }
    }
    
    cout<<"response fits for *"+alg+"* completed!"<<endl;
  }
  
  
  //
  // update the input file
  //
  cout<<"update input file "<<input<<" ..."<<flush;
  ofile->Write();
  gROOT->GetListOfFiles()->Remove(ofile);
  ofile->Close();
  delete ofile;
  gROOT->GetListOfFiles()->Remove(ifile);
  ifile->Close();
  delete ifile;
  gSystem->Exec(("mv tmp.root "+input).c_str());
  cout<<" DONE."<<endl;
  
  
  return 0;
}


////////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

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

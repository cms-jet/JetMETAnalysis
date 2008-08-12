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
  double         nrms      = cl.getValue<double> ("nrms",1.5);
  int            niter     = cl.getValue<int>    ("niter", 3);
  vector<string> algs      = cl.getVector<string>("algs", "");

  if (!cl.check()) return 0;
  cl.print();
  
  
  //
  // open input file and loop over input directories (=algorithms)
  //
  TFile* ifile = new TFile(input.c_str(),"UPDATE");
  if (!ifile->IsOpen()) { cout<<"Can't open "<<input<<endl; return 0; }

  TIter nextDir(ifile->GetListOfKeys());
  TKey* dirKey(0);
  while ((dirKey=(TKey*)nextDir())) {

    if (strcmp(dirKey->GetClassName(),"TDirectoryFile")!=0) continue;
    ifile->cd(dirKey->GetName());
    //TDirectoryFile* idir = (TDirectoryFile*)dirKey->ReadObj();
    TDirectoryFile* idir = (TDirectoryFile*)gDirectory;
    string alg(idir->GetName());
    if (algs.size()>0&&!contains(algs,alg)) continue;
    
    cout<<alg<<" ... "<<endl;
    

    //
    // loop over response histogram and fit them with a Gaussian (iteratively)
    //
    TIter nextHist(idir->GetListOfKeys());
    TKey* histKey(0);
    while ((histKey=(TKey*)nextHist())) {
      if (strcmp(histKey->GetClassName(),"TH1F")!=0) continue;
      string histname(histKey->GetName());
      if (histname.find("RelRsp")!=0&&histname.find("AbsRsp")!=0) continue;
      
      TH1F* hrsp = (TH1F*)histKey->ReadObj();
      double integral = hrsp->Integral();
      double mean     = hrsp->GetMean();
      double rms      = hrsp->GetRMS();
      if (integral>0.0) {
	double norm  = hrsp->GetMaximumStored();
	double peak  = mean;
	double sigma = rms;
	TF1* fitfnc(0);
	for (int iiter=0;iiter<niter;iiter++) {
	  double fitrange_min = std::max(hrsp->GetXaxis()->GetXmin(),mean-nrms*rms);
	  double fitrange_max = std::min(hrsp->GetXaxis()->GetXmax(),mean+nrms*rms);
	  fitfnc = new TF1("fit","gaus",fitrange_min,fitrange_max);
	  fitfnc->SetParNames("N","#mu","#sigma");
	  fitfnc->SetParameter(0,norm);
	  fitfnc->SetParameter(1,peak);
	  fitfnc->SetParameter(2,sigma);
	  hrsp->Fit(fitfnc,"RQ0");
	  hrsp->GetFunction("fit")->ResetBit(TF1::kNotDraw);
	  norm  = fitfnc->GetParameter(0);
	  peak  = fitfnc->GetParameter(1);
	  sigma = fitfnc->GetParameter(2);
	}
	if (0!=fitfnc&&fitfnc->GetNDF()<=5) {
	  cout<<"NDOF(FITFNC)="<<fitfnc->GetNDF()<<" FOR "<<hrsp->GetName()<<endl;
	  hrsp->RecursiveRemove(fitfnc);
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
  ifile->Write();
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
bool contains(const vector<string>& collection,const string& element)
{
  vector<string>::const_iterator it;
  for (it=collection.begin();it!=collection.end();++it)
    if ((*it)==element) return true;
  return false;
}

#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TTree.h"
#include "TH1D.h"

#include <iostream>
#include <string>

using namespace std;

void MakeMCHistogram(string ipath, string ifilename, string idirectory="",
                     string itreename="t", string ivar="tnpus") {
   cout << "MakeMCHistogram::Opening input file ... " << flush;
   TFile* ifile = TFile::Open((ipath+"/"+ifilename).c_str(),"READ");
   if(!ifile || !ifile->IsOpen()) {
      cout << endl << "ERROR MakeMCHistogram::The input file " << ipath << "/" << ifilename
           << " could not be opened. This program will now exit." << endl;
      return;
   }
   cout << "DONE" << endl;

   cout << "MakeMCHistogram::Retrieving input tree ... " << flush;
   TTree* itree = (TTree*)ifile->Get((idirectory+"/"+itreename).c_str());
   if(!itree) {
      cout << endl << "ERROR MakeMCHistogram::The input TTree could not be retrieved."
           << " This program will now exit." << endl;
      return;
   }
   cout << "DONE" << endl;

   cout << "MakeMCHistogram::Opening the output file and booking the output histogram ... " << flush;
   TFile* ofile = TFile::Open("MyMCPileupHistogram.root","RECREATE");
   TH1D* h = new TH1D("pileup","pileup",80,0,80);
   cout << "DONE" << endl;

   cout << "MakeMCHistogram::Filling the output histogram ... " << flush;
   itree->Draw((ivar+">>pileup").c_str(),"","goff");
   cout << "DONE" << endl;

   cout << "MakeMCHistogram::Writing the output histogram ... " << flush;
   h->Write();
   cout << "DONE" << endl;

   cout << "MakeMCHistogram::Closing the files ... " << flush;
   ofile->Close();
   ifile->Close();
   cout << "DONE" << endl;

   return;
}

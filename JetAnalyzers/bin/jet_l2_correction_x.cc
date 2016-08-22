////////////////////////////////////////////////////////////////////////////////
//
// jet_l2_correction_x
// -------------------
//
//            08/16/2008 Kostas Kousouris                    <kkousour@fnal.gov>
//                       Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
//            08/21/2015 Alexx Perloff           <alexx.stephen.perloff@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/L2Creator.hh"

#include "TApplication.h"

#include <memory>

using namespace std;


////////////////////////////////////////////////////////////////////////////////
// define local functions
////////////////////////////////////////////////////////////////////////////////

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

  bool           batch              = cl.getValue<bool>   ("batch",              false);
  string         makeCanvasVariable = cl.getValue<string> ("makeCanvasVariable",    "");

  if (!cl.partialCheck()) return 0;
  cl.print();

  //
  // run a tapplication if not in batch mode
  //
  argc = (batch) ? 2 : 1; if (batch) argv[1] = (char*)"-b";
  TApplication* app = new TApplication("jet_l2_correction_x",&argc,argv);
  
  //
  // Set the style
  //
  setTDRStyle();
  gROOT->ForceStyle();

  unique_ptr<L2Creator> l2c(new L2Creator(cl));
  //
  // open output file
  //
  l2c->openOutputFile();

  //
  // open input & l3input files and loop over directories (algorithms)
  //
  l2c->openInputFile();
  l2c->openL3File();
  l2c->loopOverDirectories();
  l2c->loopOverAlgorithms(makeCanvasVariable);
  
  //
  // close output file
  //
  l2c->closeFiles();
  
  if (!batch) app->Run();
  
  return 0;
}


////////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

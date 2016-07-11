///////////////////////////////////////////////////////////////////
//
// jet_draw_closure_x
// ------------------
//
//            08/26/2015 Alexx Perloff  <aperloff@physics.tamu.edu>
///////////////////////////////////////////////////////////////////

#include "JetMETAnalysis/JetUtilities/interface/ClosureMaker.hh"
#include "JetMETAnalysis/JetUtilities/interface/RatioMaker.hh"
#include "JetMETAnalysis/JetUtilities/interface/DifferenceMaker.hh"

#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"

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
   	gROOT->SetStyle("Plain");
   	gStyle->SetOptStat(0);

   	gSystem->Load("libFWCoreFWLite.so");

    //
   	// evaluate command-line / configuration file options
   	// 
   	CommandLine cl;
   	if (!cl.parse(argc,argv)) return 0;
  
   	bool doPt       		= cl.getValue<bool> ("doPt", 	   		   false);
   	bool doEta      		= cl.getValue<bool> ("doEta",	   		   false);
   	bool doRatioPt  		= cl.getValue<bool> ("doRatioPt",  		   false);
   	bool doRatioEta 		= cl.getValue<bool> ("doRatioEta", 		   false);
   	bool doFlavorDifference = cl.getValue<bool> ("doFlavorDifference", false);
    bool ptcl               = cl.getValue<bool> ("ptcl",               false);

   	if (!cl.partialCheck()) return 0;
   	cl.print();

	setTDRStyle();

	if(doPt || doEta) {
		unique_ptr<ClosureMaker> cm(new ClosureMaker(cl));
		if(doPt) {
            ptcl ? cm->makeClosure(VARIABLES::ptclpt) : cm->makeClosure(VARIABLES::refpt);
		}
		if(doEta) {
			if(doPt)
				cm->resetForNextAlgorithm();
			cm->makeClosure(VARIABLES::jteta);
		}
	}

	if(doRatioPt || doRatioEta) {
		unique_ptr<RatioMaker> rm(new RatioMaker(cl));
		if(doRatioPt) {
			rm->makeRatio(VARIABLES::refpt);
		}
		if(doRatioEta) {
			rm->makeRatio(VARIABLES::jteta,false);
			rm->makeRatio(VARIABLES::jteta,true);
		}
	}

	if(doFlavorDifference) {
		unique_ptr<DifferenceMaker> dm(new DifferenceMaker(cl));
		dm->makeDifference();
	}

	return 0;
}   	

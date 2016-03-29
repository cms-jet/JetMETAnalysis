#ifndef JET_RESPONSE_ANALYZER_HH
#define JET_RESPONSE_ANALYZER_HH

////////////////////////////////////////////////////////////////////////////////
//
// jet_response_analyzer
// ---------------------
//
// DESCRIPTION: jet_response_analyzer takes files written with
// the CMSSW fwk analyzer JetResponseAnalyzerProducer, and turns the trees
// in each directory (for each algorithm) into histograms. The exact
// same result can be achieved with the fwk analyzer directly! But if
// you decide to operate the fwk analyzer with doTree=true,
// doHistos=false, you can fit the output e.g. on a local disk and
// decide on the cuts and binning now. A lot more flexibility, for a
// lot more disk space ...
//
//            12/25/2013 Alexx Perloff               <aperloff@physics.tamu.edu>
////////////////////////////////////////////////////////////////////////////////

#include "JetMETAnalysis/JetAnalyzers/interface/Settings.h"
#include "JetMETAnalysis/JetUtilities/interface/JRAEvent.h"
#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
//#include "FWCore/Framework/interface/Selector.h" --> Not in 62X
//#include "FWCore/Framework/interface/SelectorBase.h" --> Not in 62X
#include "FWCore/Framework/interface/GetterOfProducts.h" //62X version
#include "FWCore/Framework/interface/ProcessMatch.h" //62X version
#include "FWCore/Framework/interface/ModuleLabelMatch.h" //62X version
#include "FWCore/Utilities/interface/value_ptr.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Provenance/interface/BranchDescription.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TKey.h>
#include <TEventList.h>

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <cmath>
#include <vector>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// class definition
////////////////////////////////////////////////////////////////////////////////

class jet_response_analyzer : public edm::EDAnalyzer
{
public:
  /// construction/destruction
  explicit jet_response_analyzer(const edm::ParameterSet& iConfig);
  virtual ~jet_response_analyzer();

  /// Book the histograms in a given TFileDirectory
  void bookHistograms(edm::Service<TFileService>& fs, string alg);

  /// Fill the handles for all of the available jet algorithms
  void getCollections(const edm::Event& iEvent, const edm::EventSetup& iSetup, vector<string> algs);

  /// get the suffix for the histogram name, e.g. JetPt100to150
  string get_suffix(const string& varname,int ibin,const vector<double>& bins);

  /// get the index of the histogram corresponding to x
  int get_index(float x,const vector<double>& binsx);

  /// get the flavors if the option doflavors is specified
  vector<string> get_flavors(bool noabsflavors);
  
  /// return the flavor name (string) given a specific pdgid
  TString pdgid_to_flavor_name(int pdgid);

  /// true is the pdgid returns a known flavor
  bool known_pdgid(int pdgid);

  /// fill the appropriate histogram (histos), based on x and binsx
  void fill_histo(float value,float weight,float x,const vector<double>& binsx,
                  string prefix,string varname);
  
  /// fill the appropriate histogram (histos), based on pdgid, x and binsx
  void fill_histo(int pdgid,float value,float weight,float x,const vector<double>& binsx,
                  string prefix,string varname, bool noabsflavors);
  
  /// fill the appropriate histogram (histos), based on x, y, binsx, and binsy
  void fill_histo(float value,float weight,float x,float y,const vector<double>& binsx,
                  const vector<double>& binsy,string prefix,string varname1,string varname2);
  
  /// fill the appropriate histogram (histos), based on pdgid, x, y, binsx, and binsy
  void fill_histo(int pdgid,float value,float weight,float x,float y,
                const vector<double>& binsx,const vector<double>& binsy,
                string prefix,string varname1,string varname2, bool noabsflavors);

  /// check if a vector of strings contains a certain element
  bool contains(const vector<string>& collection,const string& element);
  
  /// check the amount of IT pileup and see if it is in the specified range
  bool it_pileup(int itlow, int ithigh, vector<int>* npus);
  
  /// check the amount of OOT pileup before nad after the event and see if it is in the specified range
  bool oot_pileup(int earlyootlow, int earlyoothigh, int lateootlow, int lateoothigh,
                  vector<int>* npus);
  
  /// check the sum of the OOT pileup before and after the event and see if it is in the specified range
  bool total_oot_pileup(int totalootlow, int totaloothigh, vector<int>* npus);
  
  /// combines the booleans from the IT, OOT, and TotalOOT functions into one boolean
  bool pileup_cut(int itlow, int ithigh, int earlyootlow, int earlyoothigh, 
                  int lateootlow, int lateoothigh, int totalootlow, int totaloothigh, 
                  vector<int>* npus);

private:
  /// member functions
  void beginJob();
  void analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup);
  void endJob(){;}

private:
   //Inputs
   std::string    moduleLabel_;
   vector<string> algs;
   vector<double> drmaxs;
   vector<double> binspt;
   vector<double> binseta;
   vector<double> binsphi;
   vector<double> binsy;
   bool           useweight;
   float          xsection;
   int            nrefmax;
   int            nbinspt;
   int            nbinseta;
   int            nbinsphi;
   int            nbinsy;
   float          etabarrelmin;
   float          etabarrelmax;
   bool           dobalance;
   bool           doflavor;
   bool           noabsflavors;
   float          drmax;
   float          dphimin;
   bool           dojetpt;
   bool           dorefpt;
   int            nbinsrelrsp;
   float          relrspmin;
   float          relrspmax;
   int            nbinsabsrsp;
   float          absrspmin;
   float          absrspmax;
   int            nbinsetarsp;
   float          etarspmin;
   float          etarspmax;
   int            nbinsphirsp;
   float          phirspmin;
   float          phirspmax;
   float          jtptmin;
   int            itlow;
   int            ithigh;
   int            earlyootlow;
   int            earlyoothigh;
   int            lateootlow;
   int            lateoothigh;
   int            totalootlow;
   int            totaloothigh;
   TString        weightfile;
   TString        MCPUReWeighting;
   TString        DataPUReWeighting;

   //Used in calculation
   string         output, alg;
   vector<string> flavor;
   unsigned char  nref;
   bool           dorelrsp, doabsrsp, doetarsp, dophirsp, firstEvent;
   float          weight, flavorWeight;
   edm::GetterOfProducts<JRAEvent> getterOfProducts_;
   vector<edm::Handle<JRAEvent> > algHandles;
   TH1D* weightHist;
   map<TString,TH2D*> weightMap;
   edm::LumiReWeighting LumiWeights_;
   map<std::string,float> alg2drmax;
   int   refpdgid[100];
   float refpt[100];
   float refeta[100];
   float refphi[100];
   //float refy[100];
   float jtpt[100];
   float jteta[100];
   float jtphi[100];
   float jty[100];
   float refdrjt[100];
   float refdphijt[100];
   vector<int>* npus;
   vector<float>* tnpus;
   vector<TFileDirectory> subDirs;

    /// declare histograms
   /*
    vector<TH1F**>  jetPtVsJetPt;
    vector<TH1F**>  refPtVsRefPt;
    vector<TH1F**>  jetPtVsRefPt;
    vector<TH1F**>  refPtVsRefPtBarrel;
    vector<TH1F**>  jetPtVsRefPtBarrel;
    vector<TH1F**>  jetEtaVsJetEta;
    vector<TH1F**>  jetPhiVsJetPhi;
    vector<TH1F**>  jetYVsJetY;
    vector<TH1F***> jetPtVsJetEtaJetPt;
    vector<TH1F***> refPtVsJetEtaRefPt;
    vector<TH1F***> jetPtVsJetEtaRefPt;
    vector<TH1F***> jetPtVsJetYJetPt;
    vector<TH1F***> refPtVsJetYRefPt;
    vector<TH1F***> jetPtVsJetYRefPt;
    
    vector<TH1F**>  relRspVsJetPt;
    vector<TH1F**>  relRspVsRefPt;
    vector<TH1F**>  relRspVsRefPtBarrel;
    vector<TH1F**>  relRspVsJetEta;
    vector<TH1F**>  relRspVsJetPhi;
    vector<TH1F**>  relRspVsJetY;
    vector<TH1F***> relRspVsJetEtaJetPt;
    vector<TH1F***> relRspVsJetEtaRefPt;
    vector<TH1F***> relRspVsJetYJetPt;
    vector<TH1F***> relRspVsJetYRefPt;
    
    vector<TH1F**>  absRspVsJetPt;
    vector<TH1F**>  absRspVsRefPt;
    vector<TH1F**>  absRspVsRefPtBarrel;
    vector<TH1F**>  absRspVsJetEta;
    vector<TH1F**>  absRspVsJetPhi;
    vector<TH1F**>  absRspVsJetY;
    vector<TH1F***> absRspVsJetEtaJetPt;
    vector<TH1F***> absRspVsJetEtaRefPt;
    vector<TH1F***> absRspVsJetYJetPt;
    vector<TH1F***> absRspVsJetYRefPt;

    vector<TH1F**>  etaRspVsJetPt;
    vector<TH1F**>  etaRspVsRefPt;
    vector<TH1F**>  etaRspVsJetEta;
    vector<TH1F**>  etaRspVsJetPhi;
    vector<TH1F***> etaRspVsJetEtaJetPt;
    vector<TH1F***> etaRspVsJetEtaRefPt;

    vector<TH1F**>  phiRspVsJetPt;
    vector<TH1F**>  phiRspVsRefPt;
    vector<TH1F**>  phiRspVsJetEta;
    vector<TH1F**>  phiRspVsJetPhi;
    vector<TH1F***> phiRspVsJetEtaJetPt;
    vector<TH1F***> phiRspVsJetEtaRefPt;
    */
    map<string, map<string,TH1F*> > algHistos;
    map<string,TH1F*> histosPerAlg;
};
#endif

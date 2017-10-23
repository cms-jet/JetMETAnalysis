////////////////////////////////////////////////////////////////////////////////
//
// JetResponseAnalyzer
// -------------------
//
//            07/04/2008 Kostas Kousouris       <kkousour@fnal.gov>
//                       Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
//            12/08/2011 Alexx Perloff          <alexx.stephen.perloff@cern.ch>
////////////////////////////////////////////////////////////////////////////////

#ifndef JETRESPONSEANALYZER_HH
#define JETRESPONSEANALYZER_HH

#include "JetMETAnalysis/JetUtilities/interface/GenJetLeptonFinder.h"
#include "JetMETAnalysis/JetUtilities/interface/JRAEvent.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
 
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Candidate/interface/CandMatchMap.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/JetReco/interface/JPTJet.h"
#include "DataFormats/JetReco/interface/CaloJet.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "JetMETCorrections/Objects/interface/JetCorrector.h"

#include "SimDataFormats/JetMatching/interface/JetMatchedPartons.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include <TTree.h>

#include <memory>
#include <vector>
#include <sstream>
#include <cmath>
#include <thread>
#include <bitset>
#include <typeinfo>

using namespace std;

typedef edm::View<reco::PFCandidate> PFCandidateView;
const vector<double> vz = {0.0,1.7,2.6,3.0,3.5,4.2,5.2,6.0,7.5,9.0,12.0};

////////////////////////////////////////////////////////////////////////////////
// class definition
////////////////////////////////////////////////////////////////////////////////

class JetResponseAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources>
//class JetResponseAnalyzer : public edm::stream::EDAnalyzer<>
{
public:
  // construction/destruction
  explicit JetResponseAnalyzer(const edm::ParameterSet& iConfig);
  virtual ~JetResponseAnalyzer();

private:
  // member functions
  //void setupTree();
  void beginJob();
  void beginEvent() {JRAEvt_->clear();}
  void analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup);
  // get the bin number according to the vector of bin edges
  int  getBin(double x, const double boundaries[], int length);
  void endEvent() {;}
  void endJob() {;}

  JRAEvent::Flavor getFlavor(reco::PFCandidate::ParticleType id);

private:
  // member data
  std::string   moduleLabel_;
  
  edm::EDGetTokenT<reco::CandidateView> srcRef_;
  edm::EDGetTokenT<reco::CandViewMatchMap> srcJetToUncorJetMap_;
  edm::EDGetTokenT<reco::CandViewMatchMap> srcRefToJetMap_;
  edm::EDGetTokenT<reco::JetMatchedPartonsCollection> srcRefToPartonMap_;
  edm::EDGetTokenT<vector<double> > srcRhos_;
  edm::EDGetTokenT<double> srcRho_;
  edm::EDGetTokenT<double> srcRhoHLT_;
  edm::EDGetTokenT<reco::VertexCollection> srcVtx_;
  edm::EDGetTokenT<GenEventInfoProduct> srcGenInfo_;
  edm::EDGetTokenT<vector<PileupSummaryInfo> > srcPileupInfo_;
  //edm::EDGetTokenT<vector<reco::PFCandidate> > srcPFCandidates_;
  edm::EDGetTokenT<PFCandidateView> srcPFCandidates_;
  edm::EDGetTokenT<std::vector<edm::FwdPtr<reco::PFCandidate> > > srcPFCandidatesAsFwdPtr_;
  edm::EDGetTokenT<vector<reco::GenParticle> > srcGenParticles_;

  std::string   jecLabel_;
  
  bool          doComposition_;
  bool          doFlavor_;
  bool          doJetPt_;
  bool          doRefPt_;
  bool          doHLT_;
  bool          saveCandidates_;
  unsigned int  nRefMax_;

  double        deltaRMax_;
  double        deltaPhiMin_;
  double        deltaRPartonMax_;

  bool          doBalancing_;
  bool          getFlavorFromMap_;
  bool          isCaloJet_;
  bool          isJPTJet_;
  bool          isPFJet_;
  bool          isTrackJet_;
  bool          isTauJet_;

  const JetCorrector* jetCorrector_;
  
  // tree & branches
  TTree*        tree_;
  JRAEvent* 	JRAEvt_;
};

#endif

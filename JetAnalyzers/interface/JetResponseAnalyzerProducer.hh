////////////////////////////////////////////////////////////////////////////////
//
// JetResponseAnalyzerProducer
// ---------------------------
//
//                    01/28/2013 Alexx Perloff       <aperloff@physics.tamu.edu>
////////////////////////////////////////////////////////////////////////////////

#ifndef JETRESPONSEANALYZERPRODUCER_HH
#define JETRESPONSEANALYZERPRODUCER_HH


#include "JetMETAnalysis/JetUtilities/interface/GenJetLeptonFinder.h"
#include "JetMETAnalysis/JetUtilities/interface/JRAEvent.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
//#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
 
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Candidate/interface/CandMatchMap.h"
#include "DataFormats/JetReco/interface/JPTJet.h"
#include "DataFormats/JetReco/interface/CaloJet.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/Common/interface/RefVectorHolderBase.h"

#include "JetMETCorrections/Objects/interface/JetCorrector.h"

#include "SimDataFormats/JetMatching/interface/JetMatchedPartons.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include <TH1F.h>
#include <TH2F.h>
#include <TTree.h>

#include <memory>
#include <vector>
#include <sstream>
#include <cmath>


using namespace std;


////////////////////////////////////////////////////////////////////////////////
// class definition
////////////////////////////////////////////////////////////////////////////////

class JetResponseAnalyzerProducer : public edm::EDProducer
{
public:
  // construction/destruction
  explicit JetResponseAnalyzerProducer(const edm::ParameterSet& iConfig);
  virtual ~JetResponseAnalyzerProducer();

private:
  // member functions
  void beginJob();
  void beginEvent();
  //void analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup);
  void produce(edm::Event& iEvent,const edm::EventSetup& iSetup);
  void endEvent();
  void endJob() {;}

private:
  // member data
  std::string   moduleLabel_;
  
  edm::InputTag srcRef_;
  edm::InputTag srcRefToJetMap_;
  edm::InputTag srcRefToPartonMap_;
  edm::InputTag srcRho_;
  edm::InputTag srcRho50_;
  edm::InputTag srcRhoHLT_;
  edm::InputTag srcVtx_;

  std::string   jecLabel_;
  
  bool          doComposition_;
  bool          doFlavor_;
  bool          doJetPt_;
  bool          doRefPt_;
  bool          doHLT_;
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

   unique_ptr<JRAEvent> JRAEvt_;
};

#endif

////////////////////////////////////////////////////////////////////////////////
//
// MatchBackToBack
// ---------------
//
//            06/18/2008 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
 
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
 
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Candidate/interface/CandMatchMap.h"
#include "DataFormats/Math/interface/deltaPhi.h"

#include <memory>
#include <cmath>


using namespace std;
using namespace edm;
using namespace reco;


////////////////////////////////////////////////////////////////////////////////
// class definition
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
class MatchBackToBack : public edm::EDProducer
{
public:
  // construction/destruction
  MatchBackToBack(const edm::ParameterSet& iConfig);
  ~MatchBackToBack() {}
  
  // member functions
  void produce(edm::Event& iEvent,const edm::EventSetup& iSetup);
  void endJob() {;}

private:
  // member data
  edm::InputTag srcObj_;
  edm::InputTag srcRecoil_;
  double        sumPtMax_;
};




////////////////////////////////////////////////////////////////////////////////
// construction/destruction
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
MatchBackToBack::MatchBackToBack(const edm::ParameterSet& iConfig)
  : srcObj_   (iConfig.getParameter<InputTag>("srcObj"))
  , srcRecoil_(iConfig.getParameter<InputTag>("srcRecoil"))
  , sumPtMax_ (iConfig.getParameter<double>  ("sumPtMax"))
{
  produces<reco::CandViewMatchMap>();
}


////////////////////////////////////////////////////////////////////////////////
// implementation of member functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void MatchBackToBack::produce(edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  edm::Handle<CandidateView> objs;
  edm::Handle<CandidateView> recoils;
  
  iEvent.getByLabel(srcObj_,   objs);
  iEvent.getByLabel(srcRecoil_,recoils);
  
  if (objs->size()==0||recoils->size()==0) return;
  if (objs->size()>1) edm::LogWarning("MatchBackToBack")<<"nObjs>1, take first!";

  const reco::Candidate& obj = objs->at(0);
  
  unsigned int iRecoilBest=recoils->size();
  double deltaPhiMax(0.0);
  for (unsigned int iRecoil=0;iRecoil<recoils->size();iRecoil++) {
    const reco::Candidate& recoil = recoils->at(iRecoil);
    double deltaPhi = std::abs(reco::deltaPhi(obj.phi(),recoil.phi()));
    if (deltaPhi>deltaPhiMax) { iRecoilBest=iRecoil; deltaPhiMax=deltaPhi; }
  }
  
  math::XYZTLorentzVectorD sumP4;
  for (unsigned int iRecoil=0;iRecoil<recoils->size();iRecoil++) {
    if (iRecoil==iRecoilBest) continue;
    sumP4+=recoils->at(iRecoil).p4();
  }
  
  if (sumP4.pt()>sumPtMax_) return;

  unique_ptr<CandViewMatchMap> backToBackMap(new CandViewMatchMap());
  backToBackMap->insert(objs->refAt(0),recoils->refAt(iRecoilBest));
  iEvent.put(std::move(backToBackMap));
}


////////////////////////////////////////////////////////////////////////////////
// plugin definition
////////////////////////////////////////////////////////////////////////////////

DEFINE_FWK_MODULE(MatchBackToBack);

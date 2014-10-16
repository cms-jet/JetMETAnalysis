////////////////////////////////////////////////////////////////////////////////
//
// MatchRecToGenByCandidate
// ------------------------
//
//                          05/09/2014 Alexx Perloff <aperloff@physics.tamu.edu>
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
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/JetReco/interface/JPTJet.h"
#include "DataFormats/JetReco/interface/CaloJet.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/JetReco/interface/JetCollection.h"
#include "DataFormats/JetReco/interface/BasicJetCollection.h"


#include <iostream>
#include <iomanip>
#include <memory>
#include <set>

using namespace std;
using namespace edm;
using namespace reco;


////////////////////////////////////////////////////////////////////////////////
// class definition
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
typedef std::pair<double,std::pair<unsigned int,unsigned int> > Match_t;

//______________________________________________________________________________
struct MatchLtComp
{
  bool operator()(const Match_t& m1,const Match_t& m2) const
  {
    return m1.first<m2.first;
  }
};


//______________________________________________________________________________
class MatchRecToGenByCandidate : public edm::EDProducer
{
public:
  // construction/destruction
  MatchRecToGenByCandidate(const edm::ParameterSet& iConfig);
  ~MatchRecToGenByCandidate() {;}
  
  // member functions
  void produce(edm::Event& iEvent,const edm::EventSetup& iSetup);
  void endJob();
  vector<const Candidate*> convertToCandidate(reco::CandidateBaseRef recBaseRef);

private:
  // member data
  edm::InputTag srcRec_;
  edm::InputTag srcGen_;

  std::string  moduleName_;

  unsigned int nRecTot_;
  unsigned int nGenTot_;
  unsigned int nMatchedTot_;

  bool isCaloJet_;
  bool isJPTJet_;
  bool isPFJet_;
  bool isTrackJet_;
  bool isTauJet_;

  bool isCharged_;

  int  genHighPtIndex_;
  int  recoClosestDR_;

  // typdefs
  typedef set<unsigned int>                                       IndexSet_t;
  typedef IndexSet_t::const_iterator                              IndexIter_t;
  typedef set<Match_t,MatchLtComp>                                MatchSet_t;
  typedef MatchSet_t::const_iterator                              MatchIter_t;

};




////////////////////////////////////////////////////////////////////////////////
// construction/destruction
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
MatchRecToGenByCandidate::MatchRecToGenByCandidate(const edm::ParameterSet& iConfig)
  : srcRec_(iConfig.getParameter<InputTag>("srcRec"))
  , srcGen_(iConfig.getParameter<InputTag>("srcGen"))
  , moduleName_(iConfig.getParameter<string>("@module_label"))
  , nRecTot_(0)
  , nGenTot_(0)
  , nMatchedTot_(0)
{
  produces<reco::CandViewMatchMap>("rec2gen");
  produces<reco::CandViewMatchMap>("gen2rec");

  isCaloJet_  = (moduleName_.find("calo")!=string::npos);
  isJPTJet_   = (moduleName_.find("jpt") !=string::npos);
  isPFJet_    = (moduleName_.find("pf")  !=string::npos);
  isTrackJet_ = (moduleName_.find("trk") !=string::npos);
  isTauJet_   = (moduleName_.find("tau") !=string::npos);
}


////////////////////////////////////////////////////////////////////////////////
// implementation of member functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void MatchRecToGenByCandidate::produce(edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  unsigned int nRec(0);
  unsigned int nGen(0);
  unsigned int nMatched(0);

  edm::Handle<CandidateView> rec_;
  edm::Handle<CandidateView> gen_;
  
  iEvent.getByLabel(srcRec_,rec_);
  iEvent.getByLabel(srcGen_,gen_);
  
  nRec = std::min((size_t)rec_->size(),(size_t)100);
  nGen = std::min((size_t)gen_->size(),(size_t)100);
  
  IndexSet_t iRecSet;
  IndexSet_t iGenSet;
  MatchSet_t matchSet;

  for (unsigned int i=0;i<nRec;i++) iRecSet.insert(i);
  for (unsigned int i=0;i<nGen;i++) iGenSet.insert(i);

  for (unsigned int iRec=0;iRec<nRec;iRec++) {
    const reco::Candidate& rec = rec_->at(iRec);

    reco::CandidateBaseRef recBaseRef=rec_->refAt(iRec);

    vector<const Candidate*> recCand;
    if(rec.isJet())
      recCand = convertToCandidate(recBaseRef);

    for (unsigned int iGen=0;iGen<nGen;iGen++) {
      const reco::Candidate& gen = gen_->at(iGen);

      reco::CandidateBaseRef genBaseRef = gen_->refAt(iGen);

      vector<const GenParticle*> genCand;
      if(gen.isJet())
        genCand = genBaseRef.castTo<reco::GenJetRef>()->getGenConstituents();

      genHighPtIndex_ = getHighestPtCandidate(genCand);

      isCharged_ = 



      double deltaR = reco::deltaR(rec,gen);
      matchSet.insert(make_pair(deltaR,make_pair(iRec,iGen)));
    }
  }
  
  // two association maps: rec2gen and gen2rec
  auto_ptr<CandViewMatchMap> recToGenMap(new CandViewMatchMap());
  auto_ptr<CandViewMatchMap> genToRecMap(new CandViewMatchMap());
  
  MatchIter_t it=matchSet.begin();
  while (it!=matchSet.end()&&iRecSet.size()>0&&iGenSet.size()>0) {
    unsigned int iRec  = it->second.first;
    unsigned int iGen  = it->second.second;
    IndexIter_t  itRec = iRecSet.find(iRec);
    IndexIter_t  itGen = iGenSet.find(iGen);
    
    if (itRec!=iRecSet.end()&&itGen!=iGenSet.end()) {
      nMatched++;
      recToGenMap->insert(rec_->refAt(iRec),gen_->refAt(iGen));
      genToRecMap->insert(gen_->refAt(iGen),rec_->refAt(iRec));
      iRecSet.erase(itRec);
      iGenSet.erase(itGen);
    }
    
    ++it;
  }
  
  iEvent.put(recToGenMap,"rec2gen");
  iEvent.put(genToRecMap,"gen2rec");
  
  nRecTot_ += nRec;
  nGenTot_ += nGen;
  nMatchedTot_ += nMatched;
}


//______________________________________________________________________________
void MatchRecToGenByCandidate::endJob()
{
  stringstream ss;
  ss<<"nMatched = "<<nMatchedTot_<<"\n"
    <<"nRec = "<<nRecTot_<<" "
    <<"fRec = "<<100.*(nMatchedTot_/(double)nRecTot_)<<" %\n"
    <<"nGen = "<<nGenTot_<<" "
    <<"fGen = "<<100.*(nMatchedTot_/(double)nGenTot_)<<" %\n";
  edm::LogPrint("Summary")
    <<"++++++++++++++++++++++++++++++++++++++++++++++++++"
    <<"\n"<<moduleName_<<"(MatchRecToGenByCandidate) SUMMARY:\n"<<ss.str()
    <<"++++++++++++++++++++++++++++++++++++++++++++++++++";
}


//______________________________________________________________________________
vector<const Candidate*> MatchRecToGenByCandidate::convertToCandidate(reco::CandidateBaseRef recBaseRef)
{
  vector<const Candidate*> recCand;
  vector<PFCandidatePtr> recCandPF;
  vector<CaloTowerPtr> recCandCalo;


  if(isPFJet_) {
    recCandPF = recBaseRef.castTo<reco::PFJetRef>()->getPFConstituents();
    for(unsigned int i=0; i<recCandPF.size(); i++) {
      recCand.push_back((Candidate*)(&(*recCandPF[i])));
    }
  }
  else if (isCaloJet_) {
    recCandCalo = recBaseRef.castTo<reco::CaloJetRef>()->getCaloConstituents();
    for(unsigned int i=0; i<recCandCalo.size(); i++) {
      recCand.push_back((Candidate*)(&(*recCandCalo[i])));
    }
  }
  else if(isJPTJet_) {
    const reco::JPTJet& jptjet = dynamic_cast <const reco::JPTJet&> (*recBaseRef);
    edm::RefToBase<reco::Jet> jptjetRef = jptjet.getCaloJetRef();
    recCandCalo = dynamic_cast<reco::CaloJet const *>( &* jptjetRef)->getCaloConstituents();
    for(unsigned int i=0; i<recCandCalo.size(); i++) {
      recCand.push_back((Candidate*)(&(*recCandCalo[i])));
    }
  }
  
  return recCand;
}



////////////////////////////////////////////////////////////////////////////////
// plugin definition
////////////////////////////////////////////////////////////////////////////////

DEFINE_FWK_MODULE(MatchRecToGenByCandidate);

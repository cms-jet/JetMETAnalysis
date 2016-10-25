////////////////////////////////////////////////////////////////////////////////
//
// MatchRecToGen
// -------------
//
//            06/07/2008 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////

//Might want to switch to
//https://github.com/cms-sw/cmssw/blob/6b16de370881dd8ef339d34811b3d1e176c02b80/PhysicsTools/JetMCAlgos/plugins/CandOneToOneDeltaRMatcher.cc
//Though I think it will have problems with the CandViewMatchMap (AssociationMap not working)
//See:
//https://github.com/cms-sw/cmssw/blob/CMSSW_7_6_X/DataFormats/Common/interface/AssociationMap.h
//https://github.com/cms-sw/cmssw/blob/6b16de370881dd8ef339d34811b3d1e176c02b80/DataFormats/Candidate/interface/CandMatchMap.h

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/makeRefToBaseProdFrom.h"
 
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
 
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Candidate/interface/CandMatchMap.h"
#include "DataFormats/Math/interface/deltaR.h"


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
class MatchRecToGen : public edm::EDProducer
{
public:
  // construction/destruction
  MatchRecToGen(const edm::ParameterSet& iConfig);
  ~MatchRecToGen() {;}
  
  // member functions
  void produce(edm::Event& iEvent,const edm::EventSetup& iSetup);
  void endJob();

private:
  // member data
  edm::EDGetTokenT<CandidateView> srcRec_;
  edm::EDGetTokenT<CandidateView> srcGen_;

  std::string  moduleName_;

  unsigned int nRecTot_;
  unsigned int nGenTot_;
  unsigned int nMatchedTot_;

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
MatchRecToGen::MatchRecToGen(const edm::ParameterSet& iConfig)
  : srcRec_(consumes<CandidateView>(iConfig.getParameter<edm::InputTag>("srcRec")))
  , srcGen_(consumes<CandidateView>(iConfig.getParameter<edm::InputTag>("srcGen")))
  , moduleName_(iConfig.getParameter<string>("@module_label"))
  , nRecTot_(0)
  , nGenTot_(0)
  , nMatchedTot_(0)
{
  produces<CandViewMatchMap>("rec2gen");
  produces<CandViewMatchMap>("gen2rec");
}


////////////////////////////////////////////////////////////////////////////////
// implementation of member functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void MatchRecToGen::produce(edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  unsigned int nRec(0);
  unsigned int nGen(0);
  unsigned int nMatched(0);

  edm::Handle<CandidateView> rec_;
  edm::Handle<CandidateView> gen_;
  
  iEvent.getByToken(srcRec_,rec_);
  iEvent.getByToken(srcGen_,gen_);
  
  nRec = std::min((size_t)rec_->size(),(size_t)100);
  nGen = std::min((size_t)gen_->size(),(size_t)100);
  
  IndexSet_t iRecSet;
  IndexSet_t iGenSet;
  MatchSet_t matchSet;

  for (unsigned int i=0;i<nRec;i++) iRecSet.insert(i);
  for (unsigned int i=0;i<nGen;i++) iGenSet.insert(i);

  for (unsigned int iRec=0;iRec<nRec;iRec++) {
    const reco::Candidate& rec = rec_->at(iRec);
    for (unsigned int iGen=0;iGen<nGen;iGen++) {
      const reco::Candidate& gen = gen_->at(iGen);
      double deltaR = reco::deltaR(rec,gen);
      matchSet.insert(make_pair(deltaR,make_pair(iRec,iGen)));
    }
  }
  
  // two association maps: rec2gen and gen2rec
  // First check is a blank association map should be put into the event
  // (i.e. there are either no rec or no gen jets and you can't get a
  // refToBaseProd). Otherwise make and fill the map normally.
  unique_ptr<CandViewMatchMap> recToGenMap;
  unique_ptr<CandViewMatchMap> genToRecMap;
  if(nRec==0 || nGen==0) {
     recToGenMap.reset(new CandViewMatchMap());
     genToRecMap.reset(new CandViewMatchMap());
  }
  else {
     recToGenMap.reset(new CandViewMatchMap(
                          edm::makeRefToBaseProdFrom(rec_->refAt(0), iEvent),
                          edm::makeRefToBaseProdFrom(gen_->refAt(0), iEvent)));
     genToRecMap.reset(new CandViewMatchMap(
                          edm::makeRefToBaseProdFrom(gen_->refAt(0), iEvent),
                          edm::makeRefToBaseProdFrom(rec_->refAt(0), iEvent)));
  }

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
  
  iEvent.put(std::move(recToGenMap),"rec2gen");
  iEvent.put(std::move(genToRecMap),"gen2rec");
  
  nRecTot_ += nRec;
  nGenTot_ += nGen;
  nMatchedTot_ += nMatched;
}


//______________________________________________________________________________
void MatchRecToGen::endJob()
{
  stringstream ss;
  ss<<"nMatched = "<<nMatchedTot_<<"\n"
    <<"nRec = "<<nRecTot_<<" "
    <<"fRec = "<<100.*(nMatchedTot_/(double)nRecTot_)<<" %\n"
    <<"nGen = "<<nGenTot_<<" "
    <<"fGen = "<<100.*(nMatchedTot_/(double)nGenTot_)<<" %\n";
  edm::LogPrint("Summary")
    <<"++++++++++++++++++++++++++++++++++++++++++++++++++"
    <<"\n"<<moduleName_<<"(MatchRecToGen) SUMMARY:\n"<<ss.str()
    <<"++++++++++++++++++++++++++++++++++++++++++++++++++";
}


////////////////////////////////////////////////////////////////////////////////
// plugin definition
////////////////////////////////////////////////////////////////////////////////

DEFINE_FWK_MODULE(MatchRecToGen);

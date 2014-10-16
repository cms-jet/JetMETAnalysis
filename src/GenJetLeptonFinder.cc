////////////////////////////////////////////////////////////////////////////////
//
// GenJetLeptonFinder_cc
// ---------------------
//
// -> given a GenJet, this class digs for lepton (mu/el)
//    and associated neutrino constituents
//    
//
//                   16.11.2009 Hauke Held <held@cern.ch>
//
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/GenJetLeptonFinder.h"

#include "DataFormats/Candidate/interface/Candidate.h"

#include <iostream>
#include <memory>
#include <vector>
#include <string>


using namespace std;


////////////////////////////////////////////////////////////////////////////////
// -------------------- constructor && destructor ------------------------------
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
GenJetLeptonFinder::GenJetLeptonFinder(const reco::Candidate& genJet) :
  genJet_(genJet),
  lepton_(0),
  neutrino_(0),
  bmother_(0)
{

}

//______________________________________________________________________________
GenJetLeptonFinder::~GenJetLeptonFinder()
{

}


////////////////////////////////////////////////////////////////////////////////
// -------------------- member function implementation -------------------------
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
bool GenJetLeptonFinder::findLepton(bool fromB)
{
  vector<int> absPdgIds;
  absPdgIds.push_back(11);
  absPdgIds.push_back(13);
  return run(absPdgIds,fromB);
}


//______________________________________________________________________________
bool GenJetLeptonFinder::findElectron(bool fromB)
{
  vector<int> absPdgIds;
  absPdgIds.push_back(11);
  return run(absPdgIds,fromB);
}


//______________________________________________________________________________
bool GenJetLeptonFinder::findMuon(bool fromB)
{
  vector<int> absPdgIds;
  absPdgIds.push_back(13);
  return run(absPdgIds,fromB);
}


//______________________________________________________________________________
bool GenJetLeptonFinder::run(const vector<int>& absPdgIds,bool fromB)
{
  // get the GenJet constituents (but sticking to the Candidate interface)
  unsigned njetdaus = genJet_.numberOfDaughters();
  vector<const reco::Candidate*> jetdaus;
  for (unsigned i=0;i<njetdaus;i++) jetdaus.push_back(genJet_.daughter(i));
  
  // loop over constituents until lepton is found, then search neutrino
  for (unsigned itd=0;itd<njetdaus;itd++) {
    
    // retrieve constituent
    const reco::Candidate* lepcand = jetdaus[itd];
    int lepid    = lepcand->pdgId();
    int abslepid = std::abs(lepid);
    
    // decide wether lepton or not
    if (find(absPdgIds.begin(),absPdgIds.end(),abslepid)==absPdgIds.end()) continue;
    
    // search for B ancestry; if not found set pointer to zero!
    // the moving up in the mother chain is required for e.g. cascade c decays...
    bool foundB(false);
    const reco::Candidate* mother = lepcand->mother(); assert(0!=mother);
    const reco::Candidate* bcand(mother);
    while (0!=bcand&&abs(bcand->pdgId())>100&&!foundB) {
      if (abs(bcand->pdgId())>  500 && abs(bcand->pdgId())<  550) { foundB=true; break; }
      if (abs(bcand->pdgId())>10500 && abs(bcand->pdgId())<10550) { foundB=true; break; }
      if (abs(bcand->pdgId())>20500 && abs(bcand->pdgId())<20550) { foundB=true; break; }
      bcand=bcand->mother();
    }
    if (fromB&&!foundB) continue;
    
    // check for neutrino
    bool foundNu(false);
    unsigned nmodaus(mother->numberOfDaughters());
    const reco::Candidate* nucand(0);
    for (unsigned i=0;i<nmodaus&&!foundNu;i++){
      nucand = mother->daughter(i);
      if (lepcand==nucand) continue;
      if ((lepcand->pdgId())*(nucand->pdgId())==-132||
	  (lepcand->pdgId())*(nucand->pdgId())==-182) foundNu=true;
    }
    
    // save the relevant particles
    lepton_   = lepcand;
    neutrino_ = (foundNu) ? nucand : 0;
    bmother_  = (foundB)  ? bcand  : 0;    
    
    return true;
  }
  
  return false;
}



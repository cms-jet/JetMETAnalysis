#ifndef GENJETLEPTONFINDER
#define GENJETLEPTONFINDER 1

#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/Candidate/interface/Candidate.h"


//
// class declaration
//


class GenJetLeptonFinder
{
  //
  // construction/destruction
  //
public:
  GenJetLeptonFinder(const reco::GenJet& genJet);
  virtual ~GenJetLeptonFinder();

  
  //
  // member functions
  //
  bool run();
  bool foundLeptonAndNeutrino() 
                       const { return (lepton_!=0 && neutrino_!=0); }
  bool foundLepton()   const { return (lepton_!=0); }
  bool foundNeutrino() const { return (neutrino_!=0); }
  bool isMuon()        const { return (lepton_!=0 && abs(lepton_->pdgId())==13); } 
  bool isElectron()    const { return (lepton_!=0 && abs(lepton_->pdgId())==11); } 
  
  const reco::Candidate* lepton()   const { return lepton_; }
  const reco::Candidate* neutrino() const { return neutrino_; }

  int  leptonPdgId()   const { return (lepton_!=0)   ? lepton_  ->pdgId() : 0 ; }
  int  neutrinoPdgId() const { return (neutrino_!=0) ? neutrino_->pdgId() : 0 ; }

  
  //
  // member data
  //
private:
  const reco::GenJet&    genJet_;
  const reco::Candidate* lepton_;
  const reco::Candidate* neutrino_;
  
};


#endif



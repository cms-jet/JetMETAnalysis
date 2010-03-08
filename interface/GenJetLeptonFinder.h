#ifndef GENJETLEPTONFINDER
#define GENJETLEPTONFINDER 1


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
  GenJetLeptonFinder(const reco::Candidate& genJet);
  virtual ~GenJetLeptonFinder();

  
  //
  // member functions
  //
  bool run();
  bool foundLeptonAndNeutrino() 
                         const { return (lepton_!=0 && neutrino_!=0); }
  bool foundLepton()     const { return (lepton_!=0); }
  bool foundNeutrino()   const { return (neutrino_!=0); }
  bool isMuon()          const { return (lepton_!=0 && abs(lepton_->pdgId())==13); } 
  bool isElectron()      const { return (lepton_!=0 && abs(lepton_->pdgId())==11); } 
  bool foundBAncestor()  const { return (bmother_!=0); }
  
  const reco::Candidate* lepton()   const { return lepton_; }
  const reco::Candidate* neutrino() const { return neutrino_; }
  const reco::Candidate* bmother()  const { return bmother_; }

  int  leptonPdgId()   const { return (lepton_!=0)   ? lepton_  ->pdgId() : 0 ; }
  int  neutrinoPdgId() const { return (neutrino_!=0) ? neutrino_->pdgId() : 0 ; }
  int  bmotherPdgId()  const { return (bmother_!=0)  ? bmother_ ->pdgId() : 0 ; }

  //
  // member data
  //
private:
  const reco::Candidate& genJet_;
  const reco::Candidate* lepton_;
  const reco::Candidate* neutrino_;
  const reco::Candidate* bmother_;
  
};


#endif



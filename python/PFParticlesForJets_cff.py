import FWCore.ParameterSet.Config as cms

from CommonTools.ParticleFlow.PF2PAT_cff import *

pfParticlesForJets = cms.Sequence( PF2PAT )

#from PhysicsTools.PFCandProducer.pfNoPileUp_cff  import *
#from PhysicsTools.PFCandProducer.pfElectrons_cff import *
#from PhysicsTools.PFCandProducer.pfMuons_cff import *
#from PhysicsTools.PFCandProducer.ParticleSelectors.pfSortByType_cff import *
#from PhysicsTools.PFCandProducer.TopProjectors.pfNoMuon_cfi import * 
#from PhysicsTools.PFCandProducer.TopProjectors.pfNoElectron_cfi import * 

#pfParticlesForJets = cms.Sequence(
#    pfNoPileUpSequence+
#    pfAllNeutralHadrons+
#    pfAllChargedHadrons+
#    pfAllPhotons+
#    pfMuonSequence+
#    pfNoMuon+
#    pfElectronSequence+
#    pfNoElectron
#    )

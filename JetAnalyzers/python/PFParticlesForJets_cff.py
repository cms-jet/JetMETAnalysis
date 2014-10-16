import FWCore.ParameterSet.Config as cms

from CommonTools.ParticleFlow.PF2PAT_cff import *

pfParticlesForJets = cms.Sequence( PF2PAT )

#from CommonTools.ParticleFlow.pfNoPileUp_cff  import *
#from CommonTools.ParticleFlow.pfElectrons_cff import *
#from CommonTools.ParticleFlow.pfMuons_cff import *
#from CommonTools.ParticleFlow.ParticleSelectors.pfSortByType_cff import *
#from CommonTools.ParticleFlow.TopProjectors.pfNoMuon_cfi import * 
#from CommonTools.ParticleFlow.TopProjectors.pfNoElectron_cfi import * 

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

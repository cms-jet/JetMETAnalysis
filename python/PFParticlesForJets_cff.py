import FWCore.ParameterSet.Config as cms

#from PhysicsTools.PFCandProducer.pfNoPileUp_cff  import *
from PhysicsTools.PFCandProducer.pfElectrons_cff import *
from PhysicsTools.PFCandProducer.pfMuons_cff import *
from PhysicsTools.PFCandProducer.ParticleSelectors.pfSortByType_cff import *
from PhysicsTools.PFCandProducer.TopProjectors.pfNoMuon_cfi import * 
from PhysicsTools.PFCandProducer.TopProjectors.pfNoElectron_cfi import * 

pfAllNeutralHadrons.src   = 'particleFlow'
pfAllChargedHadrons.src   = 'particleFlow'
pfAllPhotons.src          = 'particleFlow'
pfAllMuons.src            = 'particleFlow'
pfAllElectrons.src        = 'particleFlow'
pfNoMuon.bottomCollection = 'particleFlow'

pfParticlesForJets = cms.Sequence(
    #pfNoPileUpSequence+
    pfAllNeutralHadrons+
    pfAllChargedHadrons+
    pfAllPhotons+
    pfMuonSequence+
    pfNoMuon+
    pfElectronSequence+
    pfNoElectron
    )

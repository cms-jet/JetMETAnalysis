import FWCore.ParameterSet.Config as cms

from JetMETCorrections.Configuration.JetCorrectionServicesAllAlgos_cff import *
from JetMETCorrections.Configuration.DefaultJEC_cff import *


# FIX RHO INPUT COLLECTION FOR CALO L1FAST CORRECTORS (CALO NOT PF)
ak5CaloL1Fastjet.srcRho = 'kt6CaloJets:rho'
ak7CaloL1Fastjet.srcRho = 'kt6CaloJets:rho'
kt4CaloL1Fastjet.srcRho = 'kt6CaloJets:rho'
kt6CaloL1Fastjet.srcRho = 'kt6CaloJets:rho'

# L1FAST JEC PRODUCERS NOT DEFINED IN DEFAULTJEC_CFF
ak5CaloJetsL1Fast = cms.EDProducer(
    'CaloJetCorrectionProducer',
    src         = cms.InputTag('ak5CaloJets'),
    correctors  = cms.vstring('ak5CaloL1Fastjet')
    )
ak7CaloJetsL1Fast = ak5CaloJetsL1Fast.clone(src='ak7CaloJets', correctors=['ak7CaloL1Fastjet'])
kt4CaloJetsL1Fast = ak5CaloJetsL1Fast.clone(src='kt4CaloJets', correctors=['kt4CaloL1Fastjet'])
kt6CaloJetsL1Fast = ak5CaloJetsL1Fast.clone(src='kt6CaloJets', correctors=['kt6CaloL1Fastjet'])

ak5PFJetsL1Fast = cms.EDProducer(
    'PFJetCorrectionProducer',
    src         = cms.InputTag('ak5PFJets'),
    correctors  = cms.vstring('ak5PFL1Fastjet')
    )
ak7PFJetsL1Fast = ak5PFJetsL1Fast.clone(src='ak7PFJets', correctors=['ak7PFL1Fastjet'])
kt4PFJetsL1Fast = ak5PFJetsL1Fast.clone(src='kt4PFJets', correctors=['kt4PFL1Fastjet'])
kt6PFJetsL1Fast = ak5PFJetsL1Fast.clone(src='kt6PFJets', correctors=['kt6PFL1Fastjet'])

# L1OFFSET JEC PRODUCERS NOT DEFINED IN DEFAULTJEC_CFF
ak5JPTJetsL1   = cms.EDProducer(
    'JPTJetCorrectionProducer',
    src         = cms.InputTag('ak5JPTJets'),
    correctors  = cms.vstring('ak5JPTL1Offset')
    )
ak7JPTJetsL1 = ak5JPTJetsL1.clone(src='ak7JPTJets', correctors=['ak7JPTL1Offset'])


# JPT L2L3
ak5JPTJetsL2L3.src = 'ak5JPTJets'
ak7JPTJetsL2L3 = ak5JPTJetsL2L3.clone(src='ak7JPTJets',correctors=['ak7JPTL2L3'])

# JPT L1L2L3
ak5JPTJetsL1L2L3.src = 'ak5JPTJets'
ak7JPTJetsL1L2L3 = ak5JPTJetsL1L2L3.clone(src='ak7JPTJets',correctors=['ak7JPTL1L2L3'])

# PFchs JEC PRODUCERS NOT DEFINED IN DEFAULTJEC_CFF
ak5PFchsL1Offset = ak5CaloL1Offset.clone(algorithm = 'AK5PFchs') #added 10/10/2011
ak7PFchsL1Offset   = ak5PFchsL1Offset.clone() #added 10/10/2011
ak5PFchsL1Fastjet = cms.ESSource( #added 10/10/2011
    'L1FastjetCorrectionService',
    era         = cms.string('Summer11'),
    level       = cms.string('L1FastJet'),
    algorithm   = cms.string('AK5PFchs'),
    section     = cms.string(''),
    srcRho      = cms.InputTag('kt6PFJets','rho'),
    useCondDB = cms.untracked.bool(True)
    )
ak7PFchsL1Fastjet   = ak5PFchsL1Fastjet.clone() #added 10/10/2011
ak5PFchsL2Relative = ak5CaloL2Relative.clone( algorithm = 'AK5PFchs' ) #added 10/10/2011
ak7PFchsL2Relative   = ak5PFchsL2Relative.clone  ( algorithm = 'AK7PFchs' ) #added 10/10/2011
ak5PFchsL3Absolute     = ak5CaloL3Absolute.clone( algorithm = 'AK5PFchs' ) #added 10/10/2011
ak7PFchsL3Absolute   = ak5PFchsL3Absolute.clone  ( algorithm = 'AK7PFchs' ) #added 10/10/2011

ak5PFchsL2L3 = cms.ESSource( #added 10/10/2011
    'JetCorrectionServiceChain',
    correctors = cms.vstring('ak5PFchsL2Relative','ak5PFchsL3Absolute')
    )
ak7PFchsL2L3 = cms.ESSource(
    'JetCorrectionServiceChain',
    correctors = cms.vstring('ak7PFchsL2Relative','ak7PFchsL3Absolute'),
    useCondDB = cms.untracked.bool(True)
    )
ak5PFchsL1L2L3 = cms.ESSource( #added 10/10/2011
    'JetCorrectionServiceChain',
    correctors = cms.vstring('ak5PFchsL1Offset','ak5PFchsL2Relative','ak5PFchsL3Absolute')
    )
ak7PFchsL1L2L3 = cms.ESSource(
    'JetCorrectionServiceChain',
    correctors = cms.vstring('ak7PFchsL1Offset','ak7PFchsL2Relative','ak7PFchsL3Absolute'),
    useCondDB = cms.untracked.bool(True)
    )
ak5PFchsL1FastL2L3 = ak5PFchsL2L3.clone() #added 10/10/2011
ak5PFchsL1FastL2L3.correctors.insert(0,'ak5PFchsL1Fastjet')
ak7PFchsL1FastL2L3 = ak7PFchsL2L3.clone()
ak7PFchsL1FastL2L3.correctors.insert(0,'ak5PFchsL1Fastjet')

ak5PFchsJetsL1Fast = cms.EDProducer(
    'PFJetCorrectionProducer',
    src         = cms.InputTag('ak5PFchsJets'),
    correctors  = cms.vstring('ak5PFchsL1Fastjet')
    ) #added 10/10/2011
ak7PFchsJetsL1Fast = ak5PFchsJetsL1Fast.clone(src='ak7PFchsJets', correctors=['ak7PFchsL1Fastjet']) #added 10/10/2011
ak5PFchsJetsL1Off = cms.EDProducer(
    'PFJetCorrectionProducer',
    src         = cms.InputTag('ak5PFchsJets'),
    correctors  = cms.vstring('ak5PFchsL1Offset')
    ) #added 10/10/2011
ak7PFchsJetsL1Off = ak5PFchsJetsL1Off.clone(src='ak7PFchsJets', correctors=['ak7PFchsL1Offset']) #added 10/10/2011

ak5PFchsJetsL2L3   = cms.EDProducer('PFJetCorrectionProducer',
    src         = cms.InputTag('ak5PFchsJets'),
    correctors  = cms.vstring('ak5PFchsL2L3')
    )

ak7PFchsJetsL2L3   = ak5PFchsJetsL2L3.clone(src = 'ak7PFchsJets', correctors = ['ak7PFchsL2L3'])

ak5PFchsJetsL1FastL2L3 = ak5PFchsJetsL2L3.clone(src = 'ak5PFchsJets', correctors = ['ak5PFchsL1FastL2L3'])
ak7PFchsJetsL1FastL2L3 = ak5PFchsJetsL2L3.clone(src = 'ak7PFchsJets', correctors = ['ak7PFchsL1FastL2L3'])

ak5PFchsJetsL1L2L3 = ak5PFchsJetsL2L3.clone(src = 'ak5PFchsJets', correctors = ['ak5PFchsL1L2L3'])
ak7PFchsJetsL1L2L3 = ak5PFchsJetsL2L3.clone(src = 'ak7PFchsJets', correctors = ['ak7PFchsL1L2L3'])

# L1OFF JEC PRODUCERS NOT DEFINED IN DEFAULTJEC_CFF
ak5CaloJetsL1Off = cms.EDProducer(
    'CaloJetCorrectionProducer',
    src         = cms.InputTag('ak5CaloJets'),
    correctors  = cms.vstring('ak5CaloL1Offset')
    )
ak7CaloJetsL1Off = ak5CaloJetsL1Off.clone(src='ak7CaloJets', correctors=['ak7CaloL1Offset'])
ak5CaloJetsL1L2L3 = ak5CaloJetsL2L3.clone(src = 'ak5CaloJets', correctors = ['ak5CaloL1L2L3'])
ak7CaloJetsL1L2L3 = ak5CaloJetsL2L3.clone(src = 'ak7CaloJets', correctors = ['ak7CaloL1L2L3'])

ak5PFJetsL1Off = cms.EDProducer(
    'PFJetCorrectionProducer',
    src         = cms.InputTag('ak5PFJets'),
    correctors  = cms.vstring('ak5PFL1Offset')
    )
ak7PFJetsL1Off = ak5PFJetsL1Off.clone(src='ak7PFJets', correctors=['ak7PFL1Offset'])
ak5PFJetsL1L2L3 = ak5PFJetsL2L3.clone(src = 'ak5PFJets', correctors = ['ak5PFL1L2L3'])
ak7PFJetsL1L2L3 = ak5PFJetsL2L3.clone(src = 'ak7PFJets', correctors = ['ak7PFL1L2L3'])

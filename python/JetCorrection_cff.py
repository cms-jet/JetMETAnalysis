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

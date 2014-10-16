import FWCore.ParameterSet.Config as cms

from JetMETCorrections.Configuration.JetCorrectionProducersAllAlgos_cff import *
from JetMETCorrections.Configuration.JetCorrectionServicesAllAlgos_cff import *
from JetMETCorrections.Configuration.DefaultJEC_cff import *

#
# FIX RHO INPUT COLLECTION FOR CALO L1FAST CORRECTORS (CALO NOT PF)
#
ak5CaloL1Fastjet.srcRho = 'kt6CaloJets:rho'
ak7CaloL1Fastjet.srcRho = 'kt6CaloJets:rho'
kt4CaloL1Fastjet.srcRho = 'kt6CaloJets:rho'
kt6CaloL1Fastjet.srcRho = 'kt6CaloJets:rho'

#
# L1FAST JEC PRODUCERS NOT DEFINED IN DEFAULTJEC_CFF
#
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
    correctors  = cms.vstring('ak5PFL1Fastjet') #cms.vstring('L1Fastjet')
    )
ak7PFJetsL1Fast = ak5PFJetsL1Fast.clone(src='ak7PFJets', correctors=['ak7PFL1Fastjet'])
kt4PFJetsL1Fast = ak5PFJetsL1Fast.clone(src='kt4PFJets', correctors=['kt4PFL1Fastjet'])
kt6PFJetsL1Fast = ak5PFJetsL1Fast.clone(src='kt6PFJets', correctors=['kt6PFL1Fastjet'])

ak5JPTJetsL1Fast = cms.EDProducer(
	'JPTJetCorrectionProducer',
	src         = cms.InputTag('ak5JPTJets'),
	correctors  = cms.vstring('ak5JPTL1Fastjet')
	)
ak7JPTJetsL1Fast = ak5JPTJetsL1Fast.clone(src='ak5JPTJets', correctors=['ak5JPTL1Fastjet'])

ak1PFL1Fastjet   = ak5PFL1Fastjet.clone( algorithm = 'AK1PF' )
ak2PFL1Fastjet   = ak5PFL1Fastjet.clone( algorithm = 'AK2PF' )
ak3PFL1Fastjet   = ak5PFL1Fastjet.clone( algorithm = 'AK3PF' )
ak4PFL1Fastjet   = ak5PFL1Fastjet.clone( algorithm = 'AK4PF' )
ak6PFL1Fastjet   = ak5PFL1Fastjet.clone( algorithm = 'AK6PF' )
ak8PFL1Fastjet   = ak5PFL1Fastjet.clone( algorithm = 'AK8PF' )
ak9PFL1Fastjet   = ak5PFL1Fastjet.clone( algorithm = 'AK9PF' )
ak10PFL1Fastjet   = ak5PFL1Fastjet.clone( algorithm = 'AK10PF' )

ak1PFJetsL1Fast = ak5PFJetsL1Fast.clone(src='ak1PFJets', correctors=['ak1PFL1Fastjet'])
ak2PFJetsL1Fast = ak5PFJetsL1Fast.clone(src='ak2PFJets', correctors=['ak2PFL1Fastjet'])
ak3PFJetsL1Fast = ak5PFJetsL1Fast.clone(src='ak3PFJets', correctors=['ak3PFL1Fastjet'])
ak4PFJetsL1Fast = ak5PFJetsL1Fast.clone(src='ak4PFJets', correctors=['ak4PFL1Fastjet'])
ak6PFJetsL1Fast = ak5PFJetsL1Fast.clone(src='ak6PFJets', correctors=['ak6PFL1Fastjet'])
ak8PFJetsL1Fast = ak5PFJetsL1Fast.clone(src='ak8PFJets', correctors=['ak8PFL1Fastjet'])
ak9PFJetsL1Fast = ak5PFJetsL1Fast.clone(src='ak9PFJets', correctors=['ak9PFL1Fastjet'])
ak10PFJetsL1Fast = ak5PFJetsL1Fast.clone(src='ak10PFJets', correctors=['ak10PFL1Fastjet'])

#
# L1OFFSET JEC PRODUCERS NOT DEFINED IN DEFAULTJEC_CFF
#
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

ak5JPTJetsL1Off  = cms.EDProducer(
    'JPTJetCorrectionProducer',
    src         = cms.InputTag('ak5JPTJets'),
    correctors  = cms.vstring('ak5L1JPTOffset')
    )
ak7JPTJetsL1Off = ak5JPTJetsL1Off.clone(src='ak7JPTJets', correctors=['ak7L1JPTOffset'])


#
# L2 AND L3 ESPRODUCERS FOR JPT
#
ak7JPTL2Relative = ak7CaloL2Relative.clone( algorithm = 'AK7JPT' )
ak7JPTL3Absolute = ak7CaloL3Absolute.clone( algorithm = 'AK7JPT' )

#
# JPT L2L3
#
ak5JPTJetsL2L3.src = 'ak5JPTJets'
ak7JPTJetsL2L3 = ak5JPTJetsL2L3.clone(src='ak7JPTJets',correctors=['ak7JPTL2L3'])

#
# JPT L1L2L3
ak5JPTJetsL1L2L3.src = 'ak5JPTJets'
ak7JPTJetsL1L2L3 = ak5JPTJetsL1L2L3.clone(src='ak7JPTJets',correctors=['ak7JPTL1L2L3'])
ak5JPTJetsL1FastL2L3 = ak5JPTJetsL1L2L3.clone(src = 'ak5JPTJets', correctors=['ak5JPTL1FastL2L3'])
ak7JPTJetsL1FastL2L3 = ak5JPTJetsL1FastL2L3.clone(src = 'ak7JPTJets', correctors=['ak7PTL1FastL2L3'])

#
# PFchs JEC PRODUCERS NOT DEFINED IN DEFAULTJEC_CFF
#
ak5PFchsL1Offset = ak5PFL1Offset.clone(algorithm = 'AK5PFchs') #added 10/10/2011
ak7PFchsL1Offset   = ak5PFchsL1Offset.clone() #added 10/10/2011
ak5PFchsL1Fastjet = cms.ESProducer( #added 10/10/2011
    'L1FastjetCorrectionESProducer',
    era         = cms.string('Summer11'),
    level       = cms.string('L1FastJet'),
    algorithm   = cms.string('AK5PFchs'),
    section     = cms.string(''),
    srcRho      = cms.InputTag('kt6PFJets','rho'),
    useCondDB = cms.untracked.bool(True)
    )
ak7PFchsL1Fastjet   = ak5PFchsL1Fastjet.clone( algorithm = 'AK7PFchs' ) #added 10/10/2011
ak5PFchsL2Relative = ak5CaloL2Relative.clone( algorithm = 'AK5PFchs' ) #added 10/10/2011
ak7PFchsL2Relative   = ak5PFchsL2Relative.clone  ( algorithm = 'AK7PFchs' ) #added 10/10/2011
ak5PFchsL3Absolute     = ak5CaloL3Absolute.clone( algorithm = 'AK5PFchs' ) #added 10/10/2011
ak7PFchsL3Absolute   = ak5PFchsL3Absolute.clone  ( algorithm = 'AK7PFchs' ) #added 10/10/2011

ak5PFchsL2L3 = cms.ESProducer( #added 10/10/2011
    'JetCorrectionESChain',
    correctors = cms.vstring('ak5PFchsL2Relative','ak5PFchsL3Absolute')
    )
ak7PFchsL2L3 = cms.ESProducer(
    'JetCorrectionESChain',
    correctors = cms.vstring('ak7PFchsL2Relative','ak7PFchsL3Absolute'),
    useCondDB = cms.untracked.bool(True)
    )
ak5PFchsL1L2L3 = cms.ESProducer( #added 10/10/2011
    'JetCorrectionESChain',
    correctors = cms.vstring('ak5PFchsL1Offset','ak5PFchsL2Relative','ak5PFchsL3Absolute')
    )
ak7PFchsL1L2L3 = cms.ESProducer(
    'JetCorrectionESChain',
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
    )
ak7PFchsJetsL1Fast = ak5PFchsJetsL1Fast.clone(src='ak7PFchsJets', correctors=['ak7PFchsL1Fastjet'])
ak5PFchsJetsL1Off = cms.EDProducer(
    'PFJetCorrectionProducer',
    src         = cms.InputTag('ak5PFchsJets'),
    correctors  = cms.vstring('ak5PFchsL1Offset')
    )
ak7PFchsJetsL1Off = ak5PFchsJetsL1Off.clone(src='ak7PFchsJets', correctors=['ak7PFchsL1Offset'])

ak5PFchsJetsL2L3   = cms.EDProducer('PFJetCorrectionProducer',
    src         = cms.InputTag('ak5PFchsJets'),
    correctors  = cms.vstring('ak5PFchsL2L3')
    )

ak7PFchsJetsL2L3   = ak5PFchsJetsL2L3.clone(src = 'ak7PFchsJets', correctors = ['ak7PFchsL2L3'])

ak5PFchsJetsL1FastL2L3 = ak5PFchsJetsL2L3.clone(src = 'ak5PFchsJets', correctors = ['ak5PFchsL1FastL2L3'])
ak7PFchsJetsL1FastL2L3 = ak5PFchsJetsL2L3.clone(src = 'ak7PFchsJets', correctors = ['ak7PFchsL1FastL2L3'])

ak5PFchsJetsL1L2L3 = ak5PFchsJetsL2L3.clone(src = 'ak5PFchsJets', correctors = ['ak5PFchsL1L2L3'])
ak7PFchsJetsL1L2L3 = ak5PFchsJetsL2L3.clone(src = 'ak7PFchsJets', correctors = ['ak7PFchsL1L2L3'])

ak1PFchsL1Fastjet   = ak5PFchsL1Fastjet.clone( algorithm = 'AK1PFchs' )
ak2PFchsL1Fastjet   = ak5PFchsL1Fastjet.clone( algorithm = 'AK2PFchs' )
ak3PFchsL1Fastjet   = ak5PFchsL1Fastjet.clone( algorithm = 'AK3PFchs' )
ak4PFchsL1Fastjet   = ak5PFchsL1Fastjet.clone( algorithm = 'AK4PFchs' )
ak6PFchsL1Fastjet   = ak5PFchsL1Fastjet.clone( algorithm = 'AK6PFchs' )
ak8PFchsL1Fastjet   = ak5PFchsL1Fastjet.clone( algorithm = 'AK8PFchs' )
ak9PFchsL1Fastjet   = ak5PFchsL1Fastjet.clone( algorithm = 'AK9PFchs' )
ak10PFchsL1Fastjet   = ak5PFchsL1Fastjet.clone( algorithm = 'AK10PFchs' )

ak1PFchsJetsL1Fast = ak5PFchsJetsL1Fast.clone(src='ak1PFchsJets', correctors=['ak1PFchsL1Fastjet'])
ak2PFchsJetsL1Fast = ak5PFchsJetsL1Fast.clone(src='ak2PFchsJets', correctors=['ak2PFchsL1Fastjet'])
ak3PFchsJetsL1Fast = ak5PFchsJetsL1Fast.clone(src='ak3PFchsJets', correctors=['ak3PFchsL1Fastjet'])
ak4PFchsJetsL1Fast = ak5PFchsJetsL1Fast.clone(src='ak4PFchsJets', correctors=['ak4PFchsL1Fastjet'])
ak6PFchsJetsL1Fast = ak5PFchsJetsL1Fast.clone(src='ak6PFchsJets', correctors=['ak6PFchsL1Fastjet'])
ak8PFchsJetsL1Fast = ak5PFchsJetsL1Fast.clone(src='ak8PFchsJets', correctors=['ak8PFchsL1Fastjet'])
ak9PFchsJetsL1Fast = ak5PFchsJetsL1Fast.clone(src='ak9PFchsJets', correctors=['ak9PFchsL1Fastjet'])
ak10PFchsJetsL1Fast = ak5PFchsJetsL1Fast.clone(src='ak10PFchsJets', correctors=['ak10PFchsL1Fastjet'])

#
# HLT JEC PRODUCERS NOT DEFINED IN DEFAULTJEC_CFF
#
ak5CaloHLTL1Offset = ak5CaloL1Offset.clone(algorithm = 'AK5CaloHLT')
ak5CaloHLTL1Fastjet = cms.ESProducer(
    'L1FastjetCorrectionESProducer',
    era         = cms.string('Summer11'),
    level       = cms.string('L1FastJet'),
    algorithm   = cms.string('AK5CaloHLT'),
    section     = cms.string(''),
    srcRho      = cms.InputTag('hltKT6CaloJets','rho'),
    useCondDB = cms.untracked.bool(True)
    )
ak5CaloHLTL2Relative = ak5CaloL2Relative.clone( algorithm = 'AK5CaloHLT' )
ak5CaloHLTL3Absolute     = ak5CaloL3Absolute.clone( algorithm = 'AK5CaloHLT' )
ak5CaloHLTL2L3 = cms.ESProducer(
    'JetCorrectionESChain',
    correctors = cms.vstring('ak5CaloHLTL2Relative','ak5CaloHLTL3Absolute')
    )
ak5CaloHLTL1L2L3 = cms.ESProducer(
    'JetCorrectionESChain',
    correctors = cms.vstring('ak5CaloHLTL1Offset','ak5CaloHLTL2Relative','ak5CaloHLTL3Absolute')
    )
ak5CaloHLTL1FastL2L3 = ak5CaloHLTL2L3.clone()
ak5CaloHLTL1FastL2L3.correctors.insert(0,'ak5CaloHLTL1Fastjet')
ak5CaloHLTJetsL1Fast = cms.EDProducer(
    'CaloJetCorrectionProducer',
    src         = cms.InputTag('hltAntiKT5CaloJets'),#'ak5CaloHLTJets'),
    correctors  = cms.vstring('ak5CaloHLTL1Fastjet')
    )
ak5CaloHLTJetsL1Off = cms.EDProducer(
    'PFJetCorrectionProducer',
    src         = cms.InputTag('hltAntiKT5CaloJets'),#'ak5CaloHLTJets'),
    correctors  = cms.vstring('ak5CaloHLTL1Offset')
    )
ak5CaloHLTJetsL2L3   = cms.EDProducer('PFJetCorrectionProducer',
    src         = cms.InputTag('hltAntiKT5CaloJets'),#'ak5CaloHLTJets'),
    correctors  = cms.vstring('ak5CaloHLTL2L3')
    )
ak5CaloHLTJetsL1FastL2L3 = ak5CaloHLTJetsL2L3.clone(src = 'hltAntiKT5CaloJets', correctors = ['ak5CaloHLTL1FastL2L3'])
ak5CaloHLTJetsL1L2L3 = ak5CaloHLTJetsL2L3.clone(src = 'hltAntiKT5CaloJets', correctors = ['ak5CaloHLTL1L2L3'])


ak5PFHLTL1Offset = ak5CaloL1Offset.clone(algorithm = 'AK5PFHLT')
ak5PFHLTL1Fastjet = cms.ESProducer(
    'L1FastjetCorrectionESProducer',
    era         = cms.string('Summer11'),
    level       = cms.string('L1FastJet'),
    algorithm   = cms.string('AK5PFHLT'),
    section     = cms.string(''),
    srcRho      = cms.InputTag('hltKT6PFJets','rho'),
    useCondDB = cms.untracked.bool(True)
     )
ak5PFHLTL2Relative = ak5CaloL2Relative.clone( algorithm = 'AK5PFHLT' )
ak5PFHLTL3Absolute     = ak5CaloL3Absolute.clone( algorithm = 'AK5PFHLT' )
ak5PFHLTL2L3 = cms.ESProducer(
    'JetCorrectionESChain',
    correctors = cms.vstring('ak5PFHLTL2Relative','ak5PFHLTL3Absolute')
    )
ak5PFHLTL1L2L3 = cms.ESProducer(
    'JetCorrectionESChain',
    correctors = cms.vstring('ak5PFHLTL1Offset','ak5PFHLTL2Relative','ak5PFHLTL3Absolute')
    )
ak5PFHLTL1FastL2L3 = ak5PFHLTL2L3.clone()
ak5PFHLTL1FastL2L3.correctors.insert(0,'ak5PFHLTL1Fastjet')
ak5PFHLTJetsL1Fast = cms.EDProducer(
    'PFJetCorrectionProducer',
    src         = cms.InputTag('hltAntiKT5PFJets'),#'ak5PFHLTJets'),
    correctors  = cms.vstring('ak5PFHLTL1Fastjet')
    )
ak5PFHLTJetsL1Off = cms.EDProducer(
    'PFJetCorrectionProducer',
    src         = cms.InputTag('hltAntiKT5PFJets'),#'ak5PFHLTJets'),
    correctors  = cms.vstring('ak5PFHLTL1Offset')
    )
ak5PFHLTJetsL2L3   = cms.EDProducer('PFJetCorrectionProducer',
    src         = cms.InputTag('hltAntiKT5PFJets'),#'ak5PFHLTJets'),
    correctors  = cms.vstring('ak5PFHLTL2L3')
    )
ak5PFHLTJetsL1FastL2L3 = ak5PFHLTJetsL2L3.clone(src = 'hltAntiKT5PFJets', correctors = ['ak5PFHLTL1FastL2L3'])
ak5PFHLTJetsL1L2L3 = ak5PFHLTJetsL2L3.clone(src = 'hltAntiKT5PFJets', correctors = ['ak5PFHLTL1L2L3'])


ak5PFchsHLTL1Offset = ak5CaloL1Offset.clone(algorithm = 'AK5PFchsHLT')
ak5PFchsHLTL1Fastjet = cms.ESProducer(
    'L1FastjetCorrectionESProducer',
    era         = cms.string('Summer11'),
    level       = cms.string('L1FastJet'),
    algorithm   = cms.string('AK5PFchsHLT'),
    section     = cms.string(''),
    srcRho      = cms.InputTag('hltKT6PFJets','rho'),
    useCondDB = cms.untracked.bool(True)
     )
ak5PFchsHLTL2Relative = ak5CaloL2Relative.clone( algorithm = 'AK5PFchsHLT' )
ak5PFchsHLTL3Absolute     = ak5CaloL3Absolute.clone( algorithm = 'AK5PFchsHLT' )
ak5PFchsHLTL2L3 = cms.ESProducer(
    'JetCorrectionESChain',
    correctors = cms.vstring('ak5PFchsHLTL2Relative','ak5PFchsHLTL3Absolute')
    )
ak5PFchsHLTL1L2L3 = cms.ESProducer(
    'JetCorrectionESChain',
    correctors = cms.vstring('ak5PFchsHLTL1Offset','ak5PFchsHLTL2Relative','ak5PFchsHLTL3Absolute')
    )
ak5PFchsHLTL1FastL2L3 = ak5PFchsHLTL2L3.clone()
ak5PFchsHLTL1FastL2L3.correctors.insert(0,'ak5PFchsHLTL1Fastjet')
ak5PFchsHLTJetsL1Fast = cms.EDProducer(
    'PFJetCorrectionProducer',
    src         = cms.InputTag('hltAntiKT5PFJetsNoPUPixelVert'),
    correctors  = cms.vstring('ak5PFchsHLTL1Fastjet')
    )
ak5PFchsHLTJetsL1Off = cms.EDProducer(
    'PFJetCorrectionProducer',
    src         = cms.InputTag('hltAntiKT5PFJetsNoPUPixelVert'),
    correctors  = cms.vstring('ak5PFchsHLTL1Offset')
    )
ak5PFchsHLTJetsL2L3   = cms.EDProducer('PFJetCorrectionProducer',
    src         = cms.InputTag('hltAntiKT5PFJetsNoPUPixelVert'),
    correctors  = cms.vstring('ak5PFchsHLTL2L3')
    )
ak5PFchsHLTJetsL1FastL2L3 = ak5PFchsHLTJetsL2L3.clone(src = 'hltAntiKT5PFJetsNoPUPixelVert', correctors = ['ak5PFchsHLTL1FastL2L3'])
ak5PFchsHLTJetsL1L2L3 = ak5PFchsHLTJetsL2L3.clone(src = 'hltAntiKT5PFJetsNoPUPixelVert', correctors = ['ak5PFchsHLTL1L2L3'])

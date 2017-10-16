import FWCore.ParameterSet.Config as cms

from JetMETCorrections.Configuration.JetCorrectionServicesAllAlgos_cff import *
from JetMETCorrections.Configuration.CorrectedJetProducersAllAlgos_cff import *
from JetMETCorrections.Configuration.CorrectedJetProducersDefault_cff import *
from JetMETCorrections.Configuration.CorrectedJetProducers_cff import *
from JetMETCorrections.Configuration.JetCorrectors_cff import *
from JetMETCorrections.Configuration.JetCorrectorsAllAlgos_cff import *

#
# PF JEC PRODUCERS NOT DEFINED in DEFAULTJEC_CFF
#
# Still missing the L1FastL2L3Corrector for AK{1-3,5-6,9-10}

ak8PFL1FastL2L3Corrector = ak8PFL2L3Corrector.clone()
ak8PFL1FastL2L3Corrector.correctors.insert(0,'ak8PFL1FastjetCorrector')
ak8PFL1FastL2L3CorrectorChain = cms.Sequence(
    ak8PFL1FastjetCorrector * ak8PFL2RelativeCorrector * ak8PFL3AbsoluteCorrector * ak8PFL1FastL2L3Corrector
)
ak8PFL1FastL2L3 = ak8PFL2L3.clone()
ak8PFL1FastL2L3.correctors.insert(0,'ak8PFL1Fastjet')

ak1PFJetsL1FastL2L3  = ak4PFJetsL2L3.clone(src = 'ak1PFJets', correctors  = ['ak1PFL1FastL2L3Corrector'])
ak2PFJetsL1FastL2L3  = ak4PFJetsL2L3.clone(src = 'ak2PFJets', correctors  = ['ak2PFL1FastL2L3Corrector'])
ak3PFJetsL1FastL2L3  = ak4PFJetsL2L3.clone(src = 'ak3PFJets', correctors  = ['ak3PFL1FastL2L3Corrector'])
ak4PFJetsL1FastL2L3  = ak4PFJetsL2L3.clone(src = 'ak4PFJets', correctors  = ['ak4PFL1FastL2L3Corrector'])
ak5PFJetsL1FastL2L3  = ak4PFJetsL2L3.clone(src = 'ak5PFJets', correctors  = ['ak5PFL1FastL2L3Corrector'])
ak6PFJetsL1FastL2L3  = ak4PFJetsL2L3.clone(src = 'ak6PFJets', correctors  = ['ak6PFL1FastL2L3Corrector'])
ak7PFJetsL1FastL2L3  = ak4PFJetsL2L3.clone(src = 'ak7PFJets', correctors  = ['ak7PFL1FastL2L3Corrector'])
ak8PFJetsL1FastL2L3  = ak4PFJetsL2L3.clone(src = 'ak8PFJets', correctors  = ['ak8PFL1FastL2L3Corrector'])
ak9PFJetsL1FastL2L3  = ak4PFJetsL2L3.clone(src = 'ak9PFJets', correctors  = ['ak9PFL1FastL2L3Corrector'])
ak10PFJetsL1FastL2L3 = ak4PFJetsL2L3.clone(src = 'ak10PFJets', correctors = ['ak10PFL1FastL2L3Corrector'])

#
# PFCHS JEC PRODUCERS NOT DEFINED IN DEFAULTJEC_CFF
#

ak8PFCHSL1FastL2L3Corrector = ak8PFL2L3Corrector.clone()
ak8PFCHSL1FastL2L3Corrector.correctors.insert(0,'ak8PFCHSL1FastjetCorrector')
ak8PFCHSL1FastL2L3CorrectorChain = cms.Sequence(
    ak8PFCHSL1FastjetCorrector * ak8PFCHSL2RelativeCorrector * ak8PFCHSL3AbsoluteCorrector * ak8PFCHSL1FastL2L3Corrector
)
ak8PFCHSL1FastL2L3 = ak8PFL2L3.clone()
ak8PFCHSL1FastL2L3.correctors.insert(0,'ak8PFCHSL1Fastjet')

ak1PFCHSJetsL1FastL2L3  = ak4PFCHSJetsL2L3.clone(src = 'ak1PFCHSJets',  correctors = ['ak1PFCHSL1FastL2L3Corrector'])
ak2PFCHSJetsL1FastL2L3  = ak4PFCHSJetsL2L3.clone(src = 'ak2PFCHSJets',  correctors = ['ak2PFCHSL1FastL2L3Corrector'])
ak3PFCHSJetsL1FastL2L3  = ak4PFCHSJetsL2L3.clone(src = 'ak3PFCHSJets',  correctors = ['ak3PFCHSL1FastL2L3Corrector'])
ak4PFCHSJetsL1FastL2L3  = ak4PFCHSJetsL2L3.clone(src = 'ak4PFCHSJets',  correctors = ['ak4PFCHSL1FastL2L3Corrector'])
ak5PFCHSJetsL1FastL2L3  = ak4PFCHSJetsL2L3.clone(src = 'ak5PFCHSJets',  correctors = ['ak5PFCHSL1FastL2L3Corrector'])
ak6PFCHSJetsL1FastL2L3  = ak4PFCHSJetsL2L3.clone(src = 'ak6PFCHSJets',  correctors = ['ak6PFCHSL1FastL2L3Corrector'])
ak7PFCHSJetsL1FastL2L3  = ak4PFCHSJetsL2L3.clone(src = 'ak7PFCHSJets',  correctors = ['ak7PFCHSL1FastL2L3Corrector'])
ak8PFCHSJetsL1FastL2L3  = ak4PFCHSJetsL2L3.clone(src = 'ak8PFCHSJets',  correctors = ['ak8PFCHSL1FastL2L3Corrector'])
ak9PFCHSJetsL1FastL2L3  = ak4PFCHSJetsL2L3.clone(src = 'ak9PFCHSJets',  correctors = ['ak9PFCHSL1FastL2L3Corrector'])
ak10PFCHSJetsL1FastL2L3 = ak4PFCHSJetsL2L3.clone(src = 'ak10PFCHSJets', correctors = ['ak10PFCHSL1FastL2L3Corrector'])

#
# PUPPI JEC PRODUCERS NOT DEFINED IN DEFAULTJEC_CFF
#

ak1PUPPIL1Fastjet = cms.ESProducer(
    'L1FastjetCorrectionESProducer',
    #era         = cms.string('Summer11'),
    level       = cms.string('L1FastJet'),
    algorithm   = cms.string('AK1PFPuppi'),
    #section     = cms.string(''),
    srcRho      = cms.InputTag('fixedGridRhoFastjetAll'),
    #useCondDB = cms.untracked.bool(True)
    )
ak2PUPPIL1Fastjet  = ak1PUPPIL1Fastjet.clone( algorithm = 'AK2PFPuppi' )
ak3PUPPIL1Fastjet  = ak1PUPPIL1Fastjet.clone( algorithm = 'AK3PFPuppi' )
ak4PUPPIL1Fastjet  = ak1PUPPIL1Fastjet.clone( algorithm = 'AK4PFPuppi' )
ak5PUPPIL1Fastjet  = ak1PUPPIL1Fastjet.clone( algorithm = 'AK5PFPuppi' )
ak6PUPPIL1Fastjet  = ak1PUPPIL1Fastjet.clone( algorithm = 'AK6PFPuppi' )
ak7PUPPIL1Fastjet  = ak1PUPPIL1Fastjet.clone( algorithm = 'AK7PFPuppi' )
ak8PUPPIL1Fastjet  = ak1PUPPIL1Fastjet.clone( algorithm = 'AK8PFPuppi' )
ak9PUPPIL1Fastjet  = ak1PUPPIL1Fastjet.clone( algorithm = 'AK9PFPuppi' )
ak10PUPPIL1Fastjet = ak1PUPPIL1Fastjet.clone( algorithm = 'AK10PFPuppi' )

ak1PUPPIL2Relative  = ak4PFL2Relative.clone( algorithm = 'AK1PFPuppi' )
ak2PUPPIL2Relative  = ak4PFL2Relative.clone( algorithm = 'AK2PFPuppi' )
ak3PUPPIL2Relative  = ak4PFL2Relative.clone( algorithm = 'AK3PFPuppi' )
ak4PUPPIL2Relative  = ak4PFL2Relative.clone( algorithm = 'AK4PFPuppi' )
ak5PUPPIL2Relative  = ak4PFL2Relative.clone( algorithm = 'AK5PFPuppi' )
ak6PUPPIL2Relative  = ak4PFL2Relative.clone( algorithm = 'AK6PFPuppi' )
ak7PUPPIL2Relative  = ak4PFL2Relative.clone( algorithm = 'AK7PFPuppi' )
ak8PUPPIL2Relative  = ak4PFL2Relative.clone( algorithm = 'AK8PFPuppi' )
ak9PUPPIL2Relative  = ak4PFL2Relative.clone( algorithm = 'AK9PFPuppi' )
ak10PUPPIL2Relative = ak4PFL2Relative.clone( algorithm = 'AK10PFPuppi' )

ak1PUPPIL3Absolute  = ak4PFL3Absolute.clone( algorithm = 'AK1PFPuppi' )
ak2PUPPIL3Absolute  = ak4PFL3Absolute.clone( algorithm = 'AK2PFPuppi' )
ak3PUPPIL3Absolute  = ak4PFL3Absolute.clone( algorithm = 'AK3PFPuppi' )
ak4PUPPIL3Absolute  = ak4PFL3Absolute.clone( algorithm = 'AK4PFPuppi' )
ak5PUPPIL3Absolute  = ak4PFL3Absolute.clone( algorithm = 'AK5PFPuppi' )
ak6PUPPIL3Absolute  = ak4PFL3Absolute.clone( algorithm = 'AK6PFPuppi' )
ak7PUPPIL3Absolute  = ak4PFL3Absolute.clone( algorithm = 'AK7PFPuppi' )
ak8PUPPIL3Absolute  = ak4PFL3Absolute.clone( algorithm = 'AK8PFPuppi' )
ak9PUPPIL3Absolute  = ak4PFL3Absolute.clone( algorithm = 'AK9PFPuppi' )
ak10PUPPIL3Absolute = ak4PFL3Absolute.clone( algorithm = 'AK10PFPuppi' )

ak1PUPPIL2L3 = cms.ESProducer(
    'JetCorrectionESChain',
    correctors = cms.vstring('ak1PUPPIL2Relative','ak1PUPPIL3Absolute')
    )
ak2PUPPIL2L3  = ak1PUPPIL2L3.clone( correctors=['ak2PUPPIL2Relative','ak2PUPPIL3Absolute'] )
ak3PUPPIL2L3  = ak1PUPPIL2L3.clone( correctors=['ak3PUPPIL2Relative','ak3PUPPIL3Absolute'] )
ak4PUPPIL2L3  = ak1PUPPIL2L3.clone( correctors=['ak4PUPPIL2Relative','ak4PUPPIL3Absolute'] )
ak5PUPPIL2L3  = ak1PUPPIL2L3.clone( correctors=['ak5PUPPIL2Relative','ak5PUPPIL3Absolute'] )
ak6PUPPIL2L3  = ak1PUPPIL2L3.clone( correctors=['ak6PUPPIL2Relative','ak6PUPPIL3Absolute'] )
ak7PUPPIL2L3  = ak1PUPPIL2L3.clone( correctors=['ak7PUPPIL2Relative','ak7PUPPIL3Absolute'] )
ak8PUPPIL2L3  = ak1PUPPIL2L3.clone( correctors=['ak8PUPPIL2Relative','ak8PUPPIL3Absolute'] )
ak9PUPPIL2L3  = ak1PUPPIL2L3.clone( correctors=['ak9PUPPIL2Relative','ak9PUPPIL3Absolute'] )
ak10PUPPIL2L3 = ak1PUPPIL2L3.clone( correctors=['ak10PUPPIL2Relative','ak10PUPPIL3Absolute'] )

ak1PUPPIL1FastL2L3 = ak1PUPPIL2L3.clone()
ak1PUPPIL1FastL2L3.correctors.insert(0,'ak1PUPPIL1Fastjet')
ak2PUPPIL1FastL2L3 = ak2PUPPIL2L3.clone()
ak2PUPPIL1FastL2L3.correctors.insert(0,'ak2PUPPIL1Fastjet')
ak3PUPPIL1FastL2L3 = ak3PUPPIL2L3.clone()
ak3PUPPIL1FastL2L3.correctors.insert(0,'ak3PUPPIL1Fastjet')
ak4PUPPIL1FastL2L3 = ak4PUPPIL2L3.clone()
ak4PUPPIL1FastL2L3.correctors.insert(0,'ak4PUPPIL1Fastjet')
ak5PUPPIL1FastL2L3 = ak5PUPPIL2L3.clone()
ak5PUPPIL1FastL2L3.correctors.insert(0,'ak5PUPPIL1Fastjet')
ak6PUPPIL1FastL2L3 = ak6PUPPIL2L3.clone()
ak6PUPPIL1FastL2L3.correctors.insert(0,'ak6PUPPIL1Fastjet')
ak7PUPPIL1FastL2L3 = ak7PUPPIL2L3.clone()
ak7PUPPIL1FastL2L3.correctors.insert(0,'ak7PUPPIL1Fastjet')
ak8PUPPIL1FastL2L3 = ak8PUPPIL2L3.clone()
ak8PUPPIL1FastL2L3.correctors.insert(0,'ak8PUPPIL1Fastjet')
ak9PUPPIL1FastL2L3 = ak9PUPPIL2L3.clone()
ak9PUPPIL1FastL2L3.correctors.insert(0,'ak9PUPPIL1Fastjet')
ak10PUPPIL1FastL2L3 = ak10PUPPIL2L3.clone()
ak10PUPPIL1FastL2L3.correctors.insert(0,'ak10PUPPIL1Fastjet')

ak4PUPPIL1FastjetCorrector = cms.EDProducer(
    'L1FastjetCorrectorProducer',
    level       = cms.string('L1FastJet'),
    algorithm   = cms.string('AK4PFPuppi'),
    srcRho      = cms.InputTag( 'fixedGridRhoFastjetAll' )
    )
ak1PUPPIL1FastjetCorrector = ak4PUPPIL1FastjetCorrector.clone( algorithm = 'AK1PFPuppi' )
ak2PUPPIL1FastjetCorrector = ak4PUPPIL1FastjetCorrector.clone( algorithm = 'AK2PFPuppi' )
ak3PUPPIL1FastjetCorrector = ak4PUPPIL1FastjetCorrector.clone( algorithm = 'AK3PFPuppi' )
ak5PUPPIL1FastjetCorrector = ak4PUPPIL1FastjetCorrector.clone( algorithm = 'AK5PFPuppi' )
ak6PUPPIL1FastjetCorrector = ak4PUPPIL1FastjetCorrector.clone( algorithm = 'AK6PFPuppi' )
ak7PUPPIL1FastjetCorrector = ak4PUPPIL1FastjetCorrector.clone( algorithm = 'AK7PFPuppi' )
ak8PUPPIL1FastjetCorrector = ak4PUPPIL1FastjetCorrector.clone( algorithm = 'AK8PFPuppi' )
ak9PUPPIL1FastjetCorrector = ak4PUPPIL1FastjetCorrector.clone( algorithm = 'AK9PFPuppi' )
ak10PUPPIL1FastjetCorrector = ak4PUPPIL1FastjetCorrector.clone( algorithm = 'AK10PFPuppi' )

ak1PUPPIL2RelativeCorrector = ak4CaloL2RelativeCorrector.clone( algorithm = 'AK1PFPuppi' )
ak2PUPPIL2RelativeCorrector = ak4CaloL2RelativeCorrector.clone( algorithm = 'AK2PFPuppi' )
ak3PUPPIL2RelativeCorrector = ak4CaloL2RelativeCorrector.clone( algorithm = 'AK3PFPuppi' )
ak4PUPPIL2RelativeCorrector = ak4CaloL2RelativeCorrector.clone( algorithm = 'AK4PFPuppi' )
ak5PUPPIL2RelativeCorrector = ak4CaloL2RelativeCorrector.clone( algorithm = 'AK5PFPuppi' )
ak6PUPPIL2RelativeCorrector = ak4CaloL2RelativeCorrector.clone( algorithm = 'AK6PFPuppi' )
ak7PUPPIL2RelativeCorrector = ak4CaloL2RelativeCorrector.clone( algorithm = 'AK7PFPuppi' )
ak8PUPPIL2RelativeCorrector = ak4CaloL2RelativeCorrector.clone( algorithm = 'AK8PFPuppi' )
ak9PUPPIL2RelativeCorrector = ak4CaloL2RelativeCorrector.clone( algorithm = 'AK9PFPuppi' )
ak10PUPPIL2RelativeCorrector = ak4CaloL2RelativeCorrector.clone( algorithm = 'AK10PFPuppi' )

ak1PUPPIL3AbsoluteCorrector = ak4CaloL3AbsoluteCorrector.clone( algorithm = 'AK1PFPuppi' )
ak2PUPPIL3AbsoluteCorrector = ak4CaloL3AbsoluteCorrector.clone( algorithm = 'AK2PFPuppi' )
ak3PUPPIL3AbsoluteCorrector = ak4CaloL3AbsoluteCorrector.clone( algorithm = 'AK3PFPuppi' )
ak4PUPPIL3AbsoluteCorrector = ak4CaloL3AbsoluteCorrector.clone( algorithm = 'AK4PFPuppi' )
ak5PUPPIL3AbsoluteCorrector = ak4CaloL3AbsoluteCorrector.clone( algorithm = 'AK5PFPuppi' )
ak6PUPPIL3AbsoluteCorrector = ak4CaloL3AbsoluteCorrector.clone( algorithm = 'AK6PFPuppi' )
ak7PUPPIL3AbsoluteCorrector = ak4CaloL3AbsoluteCorrector.clone( algorithm = 'AK7PFPuppi' )
ak8PUPPIL3AbsoluteCorrector = ak4CaloL3AbsoluteCorrector.clone( algorithm = 'AK8PFPuppi' )
ak9PUPPIL3AbsoluteCorrector = ak4CaloL3AbsoluteCorrector.clone( algorithm = 'AK9PFPuppi' )
ak10PUPPIL3AbsoluteCorrector = ak4CaloL3AbsoluteCorrector.clone( algorithm = 'AK10PFPuppi' )

ak4PUPPIL2L3Corrector = cms.EDProducer(
    'ChainedJetCorrectorProducer',
    correctors = cms.VInputTag('ak4PUPPIL2RelativeCorrector','ak4PUPPIL3AbsoluteCorrector')
    )
ak1PUPPIL2L3Corrector = ak4PUPPIL2L3Corrector.clone( correctors = ['ak1PUPPIL2RelativeCorrector','ak1PUPPIL3AbsoluteCorrector'])
ak2PUPPIL2L3Corrector = ak4PUPPIL2L3Corrector.clone( correctors = ['ak2PUPPIL2RelativeCorrector','ak2PUPPIL3AbsoluteCorrector'])
ak3PUPPIL2L3Corrector = ak4PUPPIL2L3Corrector.clone( correctors = ['ak3PUPPIL2RelativeCorrector','ak3PUPPIL3AbsoluteCorrector'])
ak5PUPPIL2L3Corrector = ak4PUPPIL2L3Corrector.clone( correctors = ['ak5PUPPIL2RelativeCorrector','ak5PUPPIL3AbsoluteCorrector'])
ak6PUPPIL2L3Corrector = ak4PUPPIL2L3Corrector.clone( correctors = ['ak6PUPPIL2RelativeCorrector','ak6PUPPIL3AbsoluteCorrector'])
ak7PUPPIL2L3Corrector = ak4PUPPIL2L3Corrector.clone( correctors = ['ak7PUPPIL2RelativeCorrector','ak7PUPPIL3AbsoluteCorrector'])
ak8PUPPIL2L3Corrector = ak4PUPPIL2L3Corrector.clone( correctors = ['ak8PUPPIL2RelativeCorrector','ak8PUPPIL3AbsoluteCorrector'])
ak9PUPPIL2L3Corrector = ak4PUPPIL2L3Corrector.clone( correctors = ['ak9PUPPIL2RelativeCorrector','ak9PUPPIL3AbsoluteCorrector'])
ak10PUPPIL2L3Corrector = ak4PUPPIL2L3Corrector.clone( correctors = ['ak10PUPPIL2RelativeCorrector','ak10PUPPIL3AbsoluteCorrector'])

ak1PUPPIL2L3CorrectorChain = cms.Sequence(
    ak1PUPPIL2RelativeCorrector * ak1PUPPIL3AbsoluteCorrector * ak1PUPPIL2L3Corrector
)
ak2PUPPIL2L3CorrectorChain = cms.Sequence(
    ak2PUPPIL2RelativeCorrector * ak2PUPPIL3AbsoluteCorrector * ak2PUPPIL2L3Corrector
)
ak3PUPPIL2L3CorrectorChain = cms.Sequence(
    ak3PUPPIL2RelativeCorrector * ak3PUPPIL3AbsoluteCorrector * ak3PUPPIL2L3Corrector
)
ak4PUPPIL2L3CorrectorChain = cms.Sequence(
    ak4PUPPIL2RelativeCorrector * ak4PUPPIL3AbsoluteCorrector * ak4PUPPIL2L3Corrector
)
ak5PUPPIL2L3CorrectorChain = cms.Sequence(
    ak5PUPPIL2RelativeCorrector * ak5PUPPIL3AbsoluteCorrector * ak5PUPPIL2L3Corrector
)
ak6PUPPIL2L3CorrectorChain = cms.Sequence(
    ak6PUPPIL2RelativeCorrector * ak6PUPPIL3AbsoluteCorrector * ak6PUPPIL2L3Corrector
)
ak7PUPPIL2L3CorrectorChain = cms.Sequence(
    ak7PUPPIL2RelativeCorrector * ak7PUPPIL3AbsoluteCorrector * ak7PUPPIL2L3Corrector
)
ak8PUPPIL2L3CorrectorChain = cms.Sequence(
    ak8PUPPIL2RelativeCorrector * ak8PUPPIL3AbsoluteCorrector * ak8PUPPIL2L3Corrector
)
ak9PUPPIL2L3CorrectorChain = cms.Sequence(
    ak9PUPPIL2RelativeCorrector * ak9PUPPIL3AbsoluteCorrector * ak9PUPPIL2L3Corrector
)
ak10PUPPIL2L3CorrectorChain = cms.Sequence(
    ak10PUPPIL2RelativeCorrector * ak10PUPPIL3AbsoluteCorrector * ak10PUPPIL2L3Corrector
)

ak1PUPPIL1FastL2L3Corrector = ak1PUPPIL2L3Corrector.clone()
ak1PUPPIL1FastL2L3Corrector.correctors.insert(0,'ak1PUPPIL1FastjetCorrector')
ak2PUPPIL1FastL2L3Corrector = ak2PUPPIL2L3Corrector.clone()
ak2PUPPIL1FastL2L3Corrector.correctors.insert(0,'ak2PUPPIL1FastjetCorrector')
ak3PUPPIL1FastL2L3Corrector = ak3PUPPIL2L3Corrector.clone()
ak3PUPPIL1FastL2L3Corrector.correctors.insert(0,'ak3PUPPIL1FastjetCorrector')
ak4PUPPIL1FastL2L3Corrector = ak4PUPPIL2L3Corrector.clone()
ak4PUPPIL1FastL2L3Corrector.correctors.insert(0,'ak4PUPPIL1FastjetCorrector')
ak5PUPPIL1FastL2L3Corrector = ak5PUPPIL2L3Corrector.clone()
ak5PUPPIL1FastL2L3Corrector.correctors.insert(0,'ak5PUPPIL1FastjetCorrector')
ak6PUPPIL1FastL2L3Corrector = ak6PUPPIL2L3Corrector.clone()
ak6PUPPIL1FastL2L3Corrector.correctors.insert(0,'ak6PUPPIL1FastjetCorrector')
ak7PUPPIL1FastL2L3Corrector = ak7PUPPIL2L3Corrector.clone()
ak7PUPPIL1FastL2L3Corrector.correctors.insert(0,'ak7PUPPIL1FastjetCorrector')
ak8PUPPIL1FastL2L3Corrector = ak8PUPPIL2L3Corrector.clone()
ak8PUPPIL1FastL2L3Corrector.correctors.insert(0,'ak8PUPPIL1FastjetCorrector')
ak9PUPPIL1FastL2L3Corrector = ak9PUPPIL2L3Corrector.clone()
ak9PUPPIL1FastL2L3Corrector.correctors.insert(0,'ak9PUPPIL1FastjetCorrector')
ak10PUPPIL1FastL2L3Corrector = ak10PUPPIL2L3Corrector.clone()
ak10PUPPIL1FastL2L3Corrector.correctors.insert(0,'ak10PUPPIL1FastjetCorrector')

ak1PUPPIL1FastL2L3CorrectorChain = cms.Sequence(
    ak1PUPPIL1FastjetCorrector * ak1PUPPIL2RelativeCorrector * ak1PUPPIL3AbsoluteCorrector * ak1PUPPIL1FastL2L3Corrector
)
ak2PUPPIL1FastL2L3CorrectorChain = cms.Sequence(
    ak2PUPPIL1FastjetCorrector * ak2PUPPIL2RelativeCorrector * ak2PUPPIL3AbsoluteCorrector * ak2PUPPIL1FastL2L3Corrector
)
ak3PUPPIL1FastL2L3CorrectorChain = cms.Sequence(
    ak3PUPPIL1FastjetCorrector * ak3PUPPIL2RelativeCorrector * ak3PUPPIL3AbsoluteCorrector * ak3PUPPIL1FastL2L3Corrector
)
ak4PUPPIL1FastL2L3CorrectorChain = cms.Sequence(
    ak4PUPPIL1FastjetCorrector * ak4PUPPIL2RelativeCorrector * ak4PUPPIL3AbsoluteCorrector * ak4PUPPIL1FastL2L3Corrector
)
ak5PUPPIL1FastL2L3CorrectorChain = cms.Sequence(
    ak5PUPPIL1FastjetCorrector * ak5PUPPIL2RelativeCorrector * ak5PUPPIL3AbsoluteCorrector * ak5PUPPIL1FastL2L3Corrector
)
ak6PUPPIL1FastL2L3CorrectorChain = cms.Sequence(
    ak6PUPPIL1FastjetCorrector * ak6PUPPIL2RelativeCorrector * ak6PUPPIL3AbsoluteCorrector * ak6PUPPIL1FastL2L3Corrector
)
ak7PUPPIL1FastL2L3CorrectorChain = cms.Sequence(
    ak7PUPPIL1FastjetCorrector * ak7PUPPIL2RelativeCorrector * ak7PUPPIL3AbsoluteCorrector * ak7PUPPIL1FastL2L3Corrector
)
ak8PUPPIL1FastL2L3CorrectorChain = cms.Sequence(
    ak8PUPPIL1FastjetCorrector * ak8PUPPIL2RelativeCorrector * ak8PUPPIL3AbsoluteCorrector * ak8PUPPIL1FastL2L3Corrector
)
ak9PUPPIL1FastL2L3CorrectorChain = cms.Sequence(
    ak9PUPPIL1FastjetCorrector * ak9PUPPIL2RelativeCorrector * ak9PUPPIL3AbsoluteCorrector * ak9PUPPIL1FastL2L3Corrector
)
ak10PUPPIL1FastL2L3CorrectorChain = cms.Sequence(
    ak10PUPPIL1FastjetCorrector * ak10PUPPIL2RelativeCorrector * ak10PUPPIL3AbsoluteCorrector * ak10PUPPIL1FastL2L3Corrector
)

ak1PUPPIJetsL1 = cms.EDProducer(
    'CorrectedPFJetProducer',
    src         = cms.InputTag('ak1PUPPIJets'),
    correctors  = cms.VInputTag('ak1PUPPIL1FastjetCorrector')
    )
ak2PUPPIJetsL1  = ak1PUPPIJetsL1.clone(src='ak2PUPPIJets', correctors=['ak2PUPPIL1FastjetCorrector'])
ak3PUPPIJetsL1  = ak1PUPPIJetsL1.clone(src='ak3PUPPIJets', correctors=['ak3PUPPIL1FastjetCorrector'])
ak4PUPPIJetsL1  = ak1PUPPIJetsL1.clone(src='ak4PUPPIJets', correctors=['ak4PUPPIL1FastjetCorrector'])
ak5PUPPIJetsL1  = ak1PUPPIJetsL1.clone(src='ak4PUPPIJets', correctors=['ak4PUPPIL1FastjetCorrector'])
ak6PUPPIJetsL1  = ak1PUPPIJetsL1.clone(src='ak6PUPPIJets', correctors=['ak6PUPPIL1FastjetCorrector'])
ak7PUPPIJetsL1  = ak1PUPPIJetsL1.clone(src='ak8PUPPIJets', correctors=['ak8PUPPIL1FastjetCorrector'])
ak8PUPPIJetsL1  = ak1PUPPIJetsL1.clone(src='ak8PUPPIJets', correctors=['ak8PUPPIL1FastjetCorrector'])
ak9PUPPIJetsL1  = ak1PUPPIJetsL1.clone(src='ak9PUPPIJets', correctors=['ak9PUPPIL1FastjetCorrector'])
ak10PUPPIJetsL1 = ak1PUPPIJetsL1.clone(src='ak10PUPPIJets', correctors=['ak10PUPPIL1FastjetCorrector'])

ak1PUPPIJetsL2L3 = cms.EDProducer('CorrectedPFJetProducer',
    src         = cms.InputTag('ak1PUPPIJets'),
    correctors  = cms.VInputTag('ak1PUPPIL2L3Corrector')
    )
ak2PUPPIJetsL2L3  = ak1PUPPIJetsL2L3.clone(src = 'ak2PUPPIJets', correctors = ['ak2PUPPIL2L3Corrector'])
ak3PUPPIJetsL2L3  = ak1PUPPIJetsL2L3.clone(src = 'ak3PUPPIJets', correctors = ['ak3PUPPIL2L3Corrector'])
ak4PUPPIJetsL2L3  = ak1PUPPIJetsL2L3.clone(src = 'ak4PUPPIJets', correctors = ['ak4PUPPIL2L3Corrector'])
ak5PUPPIJetsL2L3  = ak1PUPPIJetsL2L3.clone(src = 'ak5PUPPIJets', correctors = ['ak5PUPPIL2L3Corrector'])
ak6PUPPIJetsL2L3  = ak1PUPPIJetsL2L3.clone(src = 'ak6PUPPIJets', correctors = ['ak6PUPPIL2L3Corrector'])
ak7PUPPIJetsL2L3  = ak1PUPPIJetsL2L3.clone(src = 'ak7PUPPIJets', correctors = ['ak7PUPPIL2L3Corrector'])
ak8PUPPIJetsL2L3  = ak1PUPPIJetsL2L3.clone(src = 'ak8PUPPIJets', correctors = ['ak8PUPPIL2L3Corrector'])
ak9PUPPIJetsL2L3  = ak1PUPPIJetsL2L3.clone(src = 'ak9PUPPIJets', correctors = ['ak9PUPPIL2L3Corrector'])
ak10PUPPIJetsL2L3 = ak1PUPPIJetsL2L3.clone(src = 'ak10PUPPIJets', correctors = ['ak10PUPPIL2L3Corrector'])

ak1PUPPIJetsL1FastL2L3  = ak1PUPPIJetsL2L3.clone(src = 'ak1PUPPIJets', correctors = ['ak1PUPPIL1FastL2L3Corrector'])
ak2PUPPIJetsL1FastL2L3  = ak1PUPPIJetsL2L3.clone(src = 'ak2PUPPIJets', correctors = ['ak2PUPPIL1FastL2L3Corrector'])
ak3PUPPIJetsL1FastL2L3  = ak1PUPPIJetsL2L3.clone(src = 'ak3PUPPIJets', correctors = ['ak3PUPPIL1FastL2L3Corrector'])
ak4PUPPIJetsL1FastL2L3  = ak1PUPPIJetsL2L3.clone(src = 'ak4PUPPIJets', correctors = ['ak4PUPPIL1FastL2L3Corrector'])
ak5PUPPIJetsL1FastL2L3  = ak1PUPPIJetsL2L3.clone(src = 'ak5PUPPIJets', correctors = ['ak5PUPPIL1FastL2L3Corrector'])
ak6PUPPIJetsL1FastL2L3  = ak1PUPPIJetsL2L3.clone(src = 'ak6PUPPIJets', correctors = ['ak6PUPPIL1FastL2L3Corrector'])
ak7PUPPIJetsL1FastL2L3  = ak1PUPPIJetsL2L3.clone(src = 'ak7PUPPIJets', correctors = ['ak7PUPPIL1FastL2L3Corrector'])
ak8PUPPIJetsL1FastL2L3  = ak1PUPPIJetsL2L3.clone(src = 'ak8PUPPIJets', correctors = ['ak8PUPPIL1FastL2L3Corrector'])
ak9PUPPIJetsL1FastL2L3  = ak1PUPPIJetsL2L3.clone(src = 'ak9PUPPIJets', correctors = ['ak9PUPPIL1FastL2L3Corrector'])
ak10PUPPIJetsL1FastL2L3 = ak1PUPPIJetsL2L3.clone(src = 'ak10PUPPIJets', correctors = ['ak10PUPPIL1FastL2L3Corrector'])

import FWCore.ParameterSet.Config as cms

from JetMETCorrections.Configuration.JetCorrectionServicesAllAlgos_cff import *
from JetMETCorrections.Configuration.JetCorrectionProducersAllAlgos_cff import *


# don't use CondDB (yet) ...
ak5CaloL2Relative.useCondDB = False
ak7CaloL2Relative.useCondDB = False
kt4CaloL2Relative.useCondDB = False
kt6CaloL2Relative.useCondDB = False
ic5CaloL2Relative.useCondDB = False
ak5JPTL2Relative.useCondDB  = False
ak5PFL2Relative.useCondDB   = False
ak7PFL2Relative.useCondDB   = False
kt4PFL2Relative.useCondDB   = False
kt6PFL2Relative.useCondDB   = False
ic5PFL2Relative.useCondDB   = False

ak5CaloL3Absolute.useCondDB = False
ak7CaloL3Absolute.useCondDB = False
kt4CaloL3Absolute.useCondDB = False
kt6CaloL3Absolute.useCondDB = False
ic5CaloL3Absolute.useCondDB = False
ak5JPTL3Absolute.useCondDB  = False
ak5PFL3Absolute.useCondDB   = False
ak7PFL3Absolute.useCondDB   = False
kt4PFL3Absolute.useCondDB   = False
kt6PFL3Absolute.useCondDB   = False
ic5PFL3Absolute.useCondDB   = False


# configure missing services / producers for jpt jets
ak5JPTJetsL1.src        = 'ak5JPTJets'
ak5JPTJetsL1.correctors = ['ak5JPTL1Offset']
ak5JPTJetsL1L2L3.src    = 'ak5JPTJets'

ak7JPTL2Relative = ak5JPTL2Relative.clone( algorithm = 'AK5JPT' )
ak7JPTL3Absolute = ak5JPTL3Absolute.clone( algorithm = 'AK5JPT' )
ak7JPTJetsL1     = ak5JPTJetsL1.clone( src = 'ak7JPTJets' )
ak7JPTL2L3       = ak5JPTL2L3.clone( correctors = ['ak7JPTL2Relative','ak7JPTL3Absolute'] )
ak7JPTJetsL2L3   = ak5JPTJetsL2L3.clone( src = 'ak7JPTJets', correctors = ['ak7JPTL2L3'] )
ak7JPTL1L2L3     = ak5JPTL1L2L3.clone( correctors = ['ak5JPTL1Offset','ak7JPTL2Relative','ak7JPTL3Absolute'] )
ak7JPTJetsL1L2L3 = ak5JPTJetsL1L2L3.clone( src = 'ak7JPTJets', correctors = ['ak7JPTL1L2L3'] )


# extra l1 producers for kt jets
kt5CaloJetsL1 = kt4CaloJetsL1.clone( src = 'kt5CaloJets' )
kt7CaloJetsL1 = kt6CaloJetsL1.clone( src = 'kt7CaloJets' )
kt5PFJetsL1   = kt4PFJetsL1.clone( src  = 'kt5PFJets' )
kt7PFJetsL1   = kt6PFJetsL1.clone( src  = 'kt7PFJets' )

# extra l1 producers for ca jets
ca4CaloJetsL1 = kt4CaloJetsL1.clone( src = 'ca4CaloJets' )
ca5CaloJetsL1 = ca4CaloJetsL1.clone( src = 'ca5CaloJets' )
ca6CaloJetsL1 = ca4CaloJetsL1.clone( src = 'ca6CaloJets' )
ca7CaloJetsL1 = ca6CaloJetsL1.clone( src = 'ca7CaloJets' )

ca4PFJetsL1   = kt4PFJetsL1.clone( src = 'ca4PFJets' )
ca5PFJetsL1   = ca4PFJetsL1.clone( src = 'ca5PFJets' )
ca6PFJetsL1   = ca4PFJetsL1.clone( src = 'ca6PFJets' )
ca7PFJetsL1   = ca6PFJetsL1.clone( src = 'ca7PFJets' )


# extra l2l3 producers for jpt jets
ak5JPTJetsL2L3  = cms.EDProducer(
    'JPTJetCorrectionProducer',
    src         = cms.InputTag('ak5JPTJets'),
    correctors  = cms.vstring('ak5JPTL2L3')
    )
ic5JPTJetsL2L3 = ak5JPTJetsL2L3.clone(src='ic5JPTJets', correctors=['ic5JPTL2L3'])


# extra l2l3 producers for kt jets
kt5CaloJetsL2L3 = kt4CaloJetsL2L3.clone( src = 'kt5CaloJets' )
kt7CaloJetsL2L3 = kt6CaloJetsL2L3.clone( src = 'kt7CaloJets' )
kt5PFJetsL2L3   = kt4PFJetsL2L3.clone( src  = 'kt5PFJets' )
kt7PFJetsL2L3   = kt6PFJetsL2L3.clone( src  = 'kt7PFJets' )

# extra l2l3 producers for ca jets
ca4CaloJetsL2L3 = kt4CaloJetsL2L3.clone( src = 'ca4CaloJets' )
ca5CaloJetsL2L3 = ca4CaloJetsL2L3.clone( src = 'ca5CaloJets' )
ca6CaloJetsL2L3 = ca4CaloJetsL2L3.clone( src = 'ca6CaloJets' )
ca7CaloJetsL2L3 = ca6CaloJetsL2L3.clone( src = 'ca7CaloJets' )

ca4PFJetsL2L3   = kt4PFJetsL2L3.clone( src = 'ca4PFJets' )
ca5PFJetsL2L3   = ca4PFJetsL2L3.clone( src = 'ca5PFJets' )
ca6PFJetsL2L3   = ca4PFJetsL2L3.clone( src = 'ca6PFJets' )
ca7PFJetsL2L3   = ca6PFJetsL2L3.clone( src = 'ca7PFJets' )


# extra l1l2l3 producers for kt jets
kt5CaloJetsL1L2L3 = kt4CaloJetsL1L2L3.clone( src = 'kt5CaloJets' )
kt7CaloJetsL1L2L3 = kt6CaloJetsL1L2L3.clone( src = 'kt7CaloJets' )
kt5PFJetsL1L2L3   = kt4PFJetsL1L2L3.clone( src  = 'kt5PFJets' )
kt7PFJetsL1L2L3   = kt6PFJetsL1L2L3.clone( src  = 'kt7PFJets' )

# extra l1l2l3 producers for ca jets
ca4CaloJetsL1L2L3 = kt4CaloJetsL1L2L3.clone( src = 'ca4CaloJets' )
ca5CaloJetsL1L2L3 = ca4CaloJetsL1L2L3.clone( src = 'ca5CaloJets' )
ca6CaloJetsL1L2L3 = ca4CaloJetsL1L2L3.clone( src = 'ca6CaloJets' )
ca7CaloJetsL1L2L3 = ca6CaloJetsL1L2L3.clone( src = 'ca7CaloJets' )

ca4PFJetsL1L2L3   = kt4PFJetsL1L2L3.clone( src = 'ca4PFJets' )
ca5PFJetsL1L2L3   = ca4PFJetsL1L2L3.clone( src = 'ca5PFJets' )
ca6PFJetsL1L2L3   = ca4PFJetsL1L2L3.clone( src = 'ca6PFJets' )
ca7PFJetsL1L2L3   = ca6PFJetsL1L2L3.clone( src = 'ca7PFJets' )

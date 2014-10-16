import FWCore.ParameterSet.Config as cms

#from RecoJets.Configuration.RecoJetAssociations_cff import *
from RecoJets.JetAssociationProducers.ak5JTA_cff     import *
from RecoJets.JetAssociationProducers.ak7JTA_cff     import *
from RecoJets.Configuration.RecoJPTJets_cff          import *

ak5JPTJets = JetPlusTrackZSPCorJetAntiKt5.clone()
ak7JPTJets = ak5JPTJets.clone(
    src                            = 'ak7CaloJets',
    JetTracksAssociationAtCaloFace = 'ak7JetTracksAssociatorAtCaloFace',
    JetTracksAssociationAtVertex   = 'ak7JetTracksAssociatorAtVertex',
    alias                          = 'ak7JPTJets'
    )

recoJPTJets.replace(JetPlusTrackZSPCorJetAntiKt5,ak5JPTJets)

### Please, add also these 4 lines: it forces another type of assignment
### particles two vertex (not only eta-phi, but also vertex position is
### used). We recommend this option for JPT: in fact it was created
### speciafically for JPT.
ak5JetTracksAssociatorAtVertex.useAssigned = cms.bool(True)
ak5JetTracksAssociatorAtVertex.pvSrc = cms.InputTag("offlinePrimaryVertices")
ak7JetTracksAssociatorAtVertex.useAssigned = cms.bool(True)
ak7JetTracksAssociatorAtVertex.pvSrc = cms.InputTag("offlinePrimaryVertices") 

#ak5JPTJetsSequence = cms.Sequence(recoJetAssociations*recoJPTJets)
#ak7JPTJetsSequence = cms.Sequence(recoJetAssociations*recoJPTJets*ak7JPTJets)
ak5JPTJetsSequence = cms.Sequence(ak5JTA*recoJPTJets)
ak7JPTJetsSequence = cms.Sequence(ak5JTA*ak7JTA*recoJPTJets*ak7JPTJets)

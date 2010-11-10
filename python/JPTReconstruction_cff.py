import FWCore.ParameterSet.Config as cms


from RecoJets.Configuration.RecoJetAssociations_cff import *
from RecoJets.Configuration.RecoJPTJets_cff         import *

ic5JPTJets = JetPlusTrackZSPCorJetIcone5.clone()
ak5JPTJets = JetPlusTrackZSPCorJetAntiKt5.clone()
ak7JPTJets = ak5JPTJets.clone(
    src                            = 'ak7CaloJets',
    JetTracksAssociationAtCaloFace = 'ak7JetTracksAssociatorAtCaloFace',
    JetTracksAssociationAtVertex   = 'ak7JetTracksAssociatorAtVertex',
    alias                          = 'ak7JPTJets'
    )

recoJPTJets.replace(JetPlusTrackZSPCorJetAntiKt5,ak5JPTJets)
recoJPTJets.replace(JetPlusTrackZSPCorJetIcone5,ic5JPTJets)


ic5JPTJetsSequence = cms.Sequence(recoJetAssociations*recoJPTJets)
ak5JPTJetsSequence = cms.Sequence(recoJetAssociations*recoJPTJets)
ak7JPTJetsSequence = cms.Sequence(recoJetAssociations*recoJPTJets*ak7JPTJets)

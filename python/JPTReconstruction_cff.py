import FWCore.ParameterSet.Config as cms


from RecoJets.Configuration.RecoJetAssociations_cff import *
from RecoJets.Configuration.RecoJPTJets_cff         import *

ak5JPTJets = JetPlusTrackZSPCorJetAntiKt5.clone()
ic5JPTJets = JetPlusTrackZSPCorJetIcone5.clone()

recoJPTJets.replace(JetPlusTrackZSPCorJetAntiKt5,ak5JPTJets)
recoJPTJets.replace(JetPlusTrackZSPCorJetIcone5,ic5JPTJets)

ak5JPTJetsSequence = cms.Sequence(recoJetAssociations*recoJPTJets)
ic5JPTJetsSequence = cms.Sequence(recoJetAssociations*recoJPTJets)


import FWCore.ParameterSet.Config as cms

from RecoJets.JetPlusTracks.JetPlusTrackCorrections_cff import *
ak4JPTJets = JetPlusTrackZSPCorJetAntiKt4.clone()
ak4JPTJets.ptCUT = 5.
JetPlusTrackCorrectionsAntiKt4.replace(JetPlusTrackZSPCorJetAntiKt4,ak4JPTJets)

ak4JPTJetsSequence = cms.Sequence(JetPlusTrackCorrectionsAntiKt4)

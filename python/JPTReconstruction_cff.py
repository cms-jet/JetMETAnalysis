import FWCore.ParameterSet.Config as cms


from JetMETCorrections.Configuration.ZSPJetCorrections332_cff import *
from JetMETCorrections.Configuration.JetPlusTrackCorrections_cff import *


ak5JPTJets = JetPlusTrackZSPCorJetAntiKt5.clone()
ak5JPTJetsSequence = cms.Sequence(
    ZSPJetCorrectionsAntiKt5*
    ZSPrecoJetAssociationsAntiKt5*
    ak5JPTJets
    )

sc5JPTJets = JetPlusTrackZSPCorJetSiscone5.clone()
sc5JPTJetsSequence = cms.Sequence(
    ZSPJetCorrectionsSisCone5*
    ZSPrecoJetAssociationsSisCone5*
    sc5JPTJets
    )

ic5JPTJets = JetPlusTrackZSPCorJetIcone5.clone()
ic5JPTJetsSequence = cms.Sequence(
    ZSPJetCorrectionsIcone5*
    ZSPrecoJetAssociationsIcone5*
    ic5JPTJets
    )




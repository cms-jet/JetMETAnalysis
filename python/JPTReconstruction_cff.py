import FWCore.ParameterSet.Config as cms


from JetMETCorrections.Configuration.ZSPJetCorrections219_cff import *
from JetMETCorrections.Configuration.JetPlusTrackCorrections_cff import *

ZSPJetCorJetAntiKt5.src = 'antikt5CaloJets'

recoStandardJPTJets = cms.Path(
    ZSPJetCorrectionsIcone5+
    ZSPrecoJetAssociationsIcone5+
    JetPlusTrackCorrectionsIcone5+
    ZSPJetCorrectionsSisCone5+
    ZSPrecoJetAssociationsSisCone5+
    JetPlusTrackCorrectionsSisCone5+
    ZSPJetCorrectionsAntiKt5+
    ZSPrecoJetAssociationsAntiKt5+
    JetPlusTrackCorrectionsAntiKt5
    )

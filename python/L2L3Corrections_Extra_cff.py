import FWCore.ParameterSet.Config as cms

#################### L2L3 Module definitions ##################################

# Kt
L2L3CorJetKT5Calo = cms.EDProducer("CaloJetCorrectionProducer",
    src        = cms.InputTag("kt5CaloJets"),
    correctors = cms.vstring('L2L3JetCorrectorKT4Calo')
)
L2L3CorJetKT5PF = cms.EDProducer("PFJetCorrectionProducer",
    src        = cms.InputTag("kt5PFJets"),
    correctors = cms.vstring('L2L3JetCorrectorKT4PF')
)

L2L3CorJetKT7Calo = cms.EDProducer("CaloJetCorrectionProducer",
    src        = cms.InputTag("kt7CaloJets"),
    correctors = cms.vstring('L2L3JetCorrectorKT6Calo')
)
L2L3CorJetKT7PF = cms.EDProducer("PFJetCorrectionProducer",
    src        = cms.InputTag("kt7PFJets"),
    correctors = cms.vstring('L2L3JetCorrectorKT6PF')
)


# AntiKt
#L2L3CorJetAK5Calo = cms.EDProducer("CaloJetCorrectionProducer",
#    src        = cms.InputTag("antikt5CaloJets"),
#    correctors = cms.vstring('L2L3JetCorrectorIC5Calo')
#)
#L2L3CorJetAK5PF = cms.EDProducer("PFJetCorrectionProducer",
#    src        = cms.InputTag("antikt5PFJets"),
#    correctors = cms.vstring('L2L3JetCorrectorIC5PF')
#)

L2L3CorJetAK7Calo = cms.EDProducer("CaloJetCorrectionProducer",
    src        = cms.InputTag("antikt7CaloJets"),
    correctors = cms.vstring('L2L3JetCorrectorSC7Calo')
)
L2L3CorJetAK7PF = cms.EDProducer("PFJetCorrectionProducer",
    src        = cms.InputTag("antikt7PFJets"),
    correctors = cms.vstring('L2L3JetCorrectorSC7PF')
)


# Cambridge/Aachen
L2L3CorJetCA4Calo = cms.EDProducer("CaloJetCorrectionProducer",
    src        = cms.InputTag("ca4CaloJets"),
    correctors = cms.vstring('L2L3JetCorrectorKT4Calo')
)
L2L3CorJetCA4PF = cms.EDProducer("PFJetCorrectionProducer",
    src        = cms.InputTag("ca4PFJets"),
    correctors = cms.vstring('L2L3JetCorrectorKT4PF')
)

L2L3CorJetCA5Calo = cms.EDProducer("CaloJetCorrectionProducer",
    src        = cms.InputTag("ca5CaloJets"),
    correctors = cms.vstring('L2L3JetCorrectorKT4Calo')
)
L2L3CorJetCA5PF = cms.EDProducer("PFJetCorrectionProducer",
    src        = cms.InputTag("ca5PFJets"),
    correctors = cms.vstring('L2L3JetCorrectorKT4PF')
)

L2L3CorJetCA6Calo = cms.EDProducer("CaloJetCorrectionProducer",
    src        = cms.InputTag("ca6CaloJets"),
    correctors = cms.vstring('L2L3JetCorrectorKT6Calo')
)
L2L3CorJetCA6PF = cms.EDProducer("PFJetCorrectionProducer",
    src        = cms.InputTag("ca6PFJets"),
    correctors = cms.vstring('L2L3JetCorrectorKT6PF')
)

L2L3CorJetCA7Calo = cms.EDProducer("CaloJetCorrectionProducer",
    src        = cms.InputTag("ca7CaloJets"),
    correctors = cms.vstring('L2L3JetCorrectorKT6Calo')
)
L2L3CorJetCA7PF = cms.EDProducer("PFJetCorrectionProducer",
    src        = cms.InputTag("ca7PFJets"),
    correctors = cms.vstring('L2L3JetCorrectorKT6PF')
)


# GeneralizedKt
L2L3CorJetGK5Calo = cms.EDProducer("CaloJetCorrectionProducer",
    src        = cms.InputTag("gk5CaloJets"),
    correctors = cms.vstring('L2L3JetCorrectorIC5Calo')
)
L2L3CorJetGK5PF = cms.EDProducer("PFJetCorrectionProducer",
    src        = cms.InputTag("gk5PFJets"),
    correctors = cms.vstring('L2L3JetCorrectorIC5PF')
)

L2L3CorJetGK7Calo = cms.EDProducer("CaloJetCorrectionProducer",
    src        = cms.InputTag("gk7CaloJets"),
    correctors = cms.vstring('L2L3JetCorrectorSC7Calo')
)
L2L3CorJetGK7PF = cms.EDProducer("PFJetCorrectionProducer",
    src        = cms.InputTag("gk7PFJets"),
    correctors = cms.vstring('L2L3JetCorrectorSC7PF')
)


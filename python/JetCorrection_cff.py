import FWCore.ParameterSet.Config as cms


from JetMETCorrections.Configuration.L2L3Corrections_Summer09_cff import *
from JetMETAnalysis.JetAnalyzers.L2L3Corrections_Extra_cff import *


correctStandardCaloJets = cms.Path(L2L3CorJetAK5Calo+
                                   L2L3CorJetKT4Calo+
                                   L2L3CorJetKT6Calo+
                                   L2L3CorJetSC5Calo+
                                   L2L3CorJetSC7Calo+
                                   L2L3CorJetIC5Calo
                                   )

correctStandardPFJets = cms.Path(L2L3CorJetAK5PF+
                                 L2L3CorJetKT4PF+
                                 L2L3CorJetKT6PF+
                                 L2L3CorJetSC5PF+
                                 L2L3CorJetSC7PF+
                                 L2L3CorJetIC5PF
                                 )

correctExtraCaloJets = cms.Path(L2L3CorJetAK7Calo+
                                L2L3CorJetKT5Calo+
                                L2L3CorJetKT7Calo+
                                L2L3CorJetCA4Calo+
                                L2L3CorJetCA5Calo+
                                L2L3CorJetCA6Calo+
                                L2L3CorJetCA7Calo
                                #L2L3CorJetGK5Calo+
                                #L2L3CorJetGK7Calo
                                )

correctExtraPFJets = cms.Path(L2L3CorJetAK7PF+
                              L2L3CorJetKT5PF+
                              L2L3CorJetKT7PF+
                              L2L3CorJetCA4PF+
                              L2L3CorJetCA5PF+
                              L2L3CorJetCA6PF+
                              L2L3CorJetCA7PF
                              #L2L3CorJetGK5PF+
                              #L2L3CorJetGK7PF
                              )

import FWCore.ParameterSet.Config as cms

from JetMETAnalysis.JetAnalyzers.JRA_Paths_cff import *
from JetMETAnalysis.JetAnalyzers.JetReconstruction_cff import *
from JetMETAnalysis.JetAnalyzers.JPTReconstruction_cff import *
from JetMETAnalysis.JetAnalyzers.JetCorrection_cff import *


JRAStandardCaloJetsSchedule = cms.Schedule(
    #recoStandardCaloJets,
    jraStandardCaloJets
    )

JRAStandardCaloL2L3JetsSchedule = cms.Schedule(
    correctStandardCaloJets,
    jraStandardCaloL2L3Jets
    )

JRAExtraCaloJetsSchedule = cms.Schedule(
    recoExtraCaloJets,
    jraExtraCaloJets
    )

JRAExtraCaloL2L3JetsSchedule = cms.Schedule(
    recoExtraCaloJets,
    correctExtraCaloJets,
    jraExtraCaloL2L3Jets
    )

JRAStandardPFJetsSchedule = cms.Schedule(
    #recoStandardPFJets,
    jraStandardPFJets
    )

JRAStandardPFL2L3JetsSchedule = cms.Schedule(
    correctStandardPFJets,
    jraStandardPFL2L3Jets
    )

JRAExtraPFJetsSchedule = cms.Schedule(
    recoExtraPFJets,
    jraExtraPFJets
    )

JRAExtraPFL2L3JetsSchedule = cms.Schedule(
    recoExtraPFJets,
    correctExtraPFJets,
    jraExtraPFL2L3Jets
    )

JRAStandardJPTJetsSchedule = cms.Schedule(
    recoStandardJPTJets,
    jraStandardJPTJets
    )

JRAStandardTrkJetsSchedule = cms.Schedule(
    recoStandardTrackJets,
    jraStandardTrackJets
    )

JRAExtraTrkJetsSchedule = cms.Schedule(
    recoExtraTrackJets,
    jraExtraTrackJets
    )

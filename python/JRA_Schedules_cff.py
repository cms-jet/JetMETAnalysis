import FWCore.ParameterSet.Config as cms

from JetMETAnalysis.JetAnalyzers.JRA_Paths_cff import *
from JetMETAnalysis.JetAnalyzers.JetReconstruction_cff import *
from JetMETAnalysis.JetAnalyzers.JPTReconstruction_cff import *
from JetMETAnalysis.JetAnalyzers.JetCorrection_cff import *


JRAStandardCaloJetsSchedule = cms.Schedule(
    #recoStandardGenJets,
    #recoStandardCaloJets,
    jraStandardCaloJets
    )

JRAStandardCaloL2L3JetsSchedule = cms.Schedule(
    correctStandardCaloJets,
    jraStandardCaloL2L3Jets
    )

JRAExtraCaloJetsSchedule = cms.Schedule(
    recoExtraGenJets,
    recoExtraCaloJets,
    jraExtraCaloJets
    )

JRAExtraCaloL2L3JetsSchedule = cms.Schedule(
    recoExtraGenJets,
    recoExtraCaloJets,
    correctExtraCaloJets,
    jraExtraCaloL2L3Jets
    )

JRAStandardPFJetsSchedule = cms.Schedule(
    #recoStandardGenJets,
    #recoStandardPFJets,
    jraStandardPFJets
    )

JRAStandardPFL2L3JetsSchedule = cms.Schedule(
    correctStandardPFJets,
    jraStandardPFL2L3Jets
    )

JRAExtraPFJetsSchedule = cms.Schedule(
    recoExtraGenJets,
    recoExtraPFJets,
    jraExtraPFJets
    )

JRAExtraPFL2L3JetsSchedule = cms.Schedule(
    recoExtraGenJets,
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
    recoExtraGenJets,
    recoExtraTrackJets,
    jraExtraTrackJets
    )

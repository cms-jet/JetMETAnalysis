import FWCore.ParameterSet.Config as cms

#!
#! ADJUST JET & REFERENCE PRESELECTION, RESPONSE ANALYSIS PARAMETERS
#!
import JetMETAnalysis.JetAnalyzers.JRA_Defaults_cff as Defaults;
import JetMETAnalysis.JetAnalyzers.JRA_HistoDefaults_cff as Histos;

Defaults.JetPtEta = cms.PSet(
    etaMin = cms.double(-5.0),
    etaMax = cms.double(5.0),
    ptMin  = cms.double(1.0)
)
Defaults.RefPtEta = cms.PSet(
    etaMin = cms.double(-5.0),
    etaMax = cms.double(5.0),
    ptMin = cms.double(1.0)
)
Defaults.JetResponseParameters = Histos.JetResponseParameters


#!
#! PROCESS
#!
process = cms.Process("JETCALIB")

process.load("JetMETAnalysis.JetAnalyzers.JRA_HistoDefaults_cff")
process.load("JetMETAnalysis.JetAnalyzers.JRA_PathsAndModules_cff")


#!
#! INPUT
#!
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
    '/store/relval/CMSSW_2_1_10/RelValQCD_Pt_80_120/GEN-SIM-DIGI-RAW-HLTDEBUG-RECO/IDEAL_V9_v2/0000/046AC296-EC99-DD11-9691-000423D6A6F4.root'
    )
)


#!
#! SERVICES
#!
process.MessageLogger = cms.Service("MessageLogger",
    destinations = cms.untracked.vstring('cout'),
    cout         = cms.untracked.PSet(threshold = cms.untracked.string('WARNING'))

)
process.TFileService = cms.Service("TFileService",
    fileName      = cms.string('JRAh.root'),
    closeFileFast = cms.untracked.bool(True)
)



#!
#! SCHEDULE
#!
process.schedule = cms.Schedule(
    process.kt4caloJRA,
    process.kt6caloJRA,
    process.sc5caloJRA,
    process.sc7caloJRA,
    process.ic5caloJRA,
#    process.kt4pfJRA,
#    process.kt6pfJRA,
#    process.sc5pfJRA,
#    process.sc7pfJRA,
    process.ic5pfJRA,
#    process.kt4trkJRA,
#    process.kt6trkJRA,
#    process.sc5trkJRA,
#    process.sc7trkJRA,
#    process.ic5trkJRA
)


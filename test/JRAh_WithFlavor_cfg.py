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
    ptMin = cms.double(10.0)
)
Defaults.JetResponseParameters = Histos.JetResponseParameters


#
# JRA defaults suitable for validation histograms
#

# pT binning
Defaults.JetResponseParameters.binsPt = cms.vdouble(
    10.,15.,20.,30.,45.,65.,100.,180.,250.,350.,500.,800.
    )

# eta binning
Defaults.JetResponseParameters.binsEta = cms.vdouble(
    0.0,1.4,3.2,5.0
    )

# absolute response histogram parameters
Defaults.JetResponseParameters.nBinsAbsRsp = cms.uint32(200);
Defaults.JetResponseParameters.absRspMin   = cms.double(-300);
Defaults.JetResponseParameters.absRspMax   = cms.double(100);


#!
#! PROCESS
#!
process = cms.Process("JRA")

process.load("JetMETAnalysis.JetAnalyzers.JRA_TreeDefaults_cff")


#!
#! INPUT
#!
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))
process.source = cms.Source(
    "PoolSource",
    fileNames = cms.untracked.vstring(
    '/store/relval/CMSSW_3_1_2/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/MC_31X_V3-v1/0007/9E83A122-E978-DE11-9D04-001D09F23C73.root'
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
process.load("JetMETAnalysis.JetAnalyzers.JRA_SchedulesWithFlavor_cff")
process.schedule = cms.Schedule()
process.schedule.extend(process.JRAStandardCaloJetsSchedule)
process.schedule.extend(process.JRAStandardCaloL2L3JetsSchedule)
# process.schedule.extend(process.JRAExtraCaloJetsSchedule)
# process.schedule.extend(process.JRAExtraCaloL2L3JetsSchedule)
process.schedule.extend(process.JRAStandardPFJetsSchedule)
process.schedule.extend(process.JRAStandardPFL2L3JetsSchedule)
# process.schedule.extend(process.JRAExtraPFJetsSchedule)
# process.schedule.extend(process.JRAExtraPFL2L3JetsSchedule)
# process.schedule.extend(process.JRAStandardTrkJetsSchedule)
# process.schedule.extend(process.JRAExtraTrkJetsSchedule)
# process.schedule.extend(process.JRAStandardJPTJetsSchedule)

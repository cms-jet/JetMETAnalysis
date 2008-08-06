import FWCore.ParameterSet.Config as cms

#!
#! ADJUST PT/ETA PRESELECTION AND CALIBRATION PARAMETERS
#!
import JetMETAnalysis.JetAnalyzers.JetResponse_cff
import JetMETAnalysis.JetAnalyzers.JetRspParams_Histos_cff

JetMETAnalysis.JetAnalyzers.JetResponse_cff.JetPtEta = cms.PSet(
    etaMin = cms.double(-5.0),
    etaMax = cms.double(5.0),
    ptMin  = cms.double(1.0)
)
JetMETAnalysis.JetAnalyzers.JetResponse_cff.RefPtEta = cms.PSet(
    etaMin = cms.double(-5.0),
    etaMax = cms.double(5.0),
    ptMin = cms.double(1.0)
)
JetMETAnalysis.JetAnalyzers.JetResponse_cff.JetResponseParameters = JetMETAnalysis.JetAnalyzers.JetRspParams_Histos_cff.JetResponseParameters


#!
#! PROCESS
#!
process = cms.Process("JETCALIB")

process.load("JetMETAnalysis.JetAnalyzers.JetRspParams_Histos_cff")
process.load("JetMETAnalysis.JetAnalyzers.JetResponse_cff")


#!
#! INPUT
#!
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))
process.source = cms.Source(
    "PoolSource",
    fileNames = cms.untracked.vstring('rfio:/castor/cern.ch/user/s/schiefer/JetAlgs/21X/QCD_80_120.root')
)


#!
#! SERVICES
#!
process.MessageLogger = cms.Service(
    "MessageLogger",
    cout = cms.untracked.PSet(threshold = cms.untracked.string('WARNING')),
    destinations = cms.untracked.vstring('cout')
)
process.TFileService = cms.Service(
    "TFileService",
    closeFileFast = cms.untracked.bool(True),
    fileName = cms.string('JRAh.root')
)



#!
#! SCHEDULE
#!
process.schedule = cms.Schedule(
    process.rsp_kt4calo,
    process.rsp_kt6calo,
    process.rsp_sc5calo,
    process.rsp_sc7calo,
    process.rsp_ic5calo,
    process.rsp_kt4pflow,
    process.rsp_kt6pflow,
    process.rsp_sc5pflow,
    process.rsp_sc7pflow,
    process.rsp_ic5pflow,
    process.rsp_kt4track,
    process.rsp_kt6track,
    process.rsp_sc5track,
    process.rsp_sc7track,
    process.rsp_ic5track
    )



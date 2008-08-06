import FWCore.ParameterSet.Config as cms

process = cms.Process("JETCALIB")
process.load("JetMETAnalysis.JetAnalyzers.JetRspParams_Tree_cff")

process.load("JetMETAnalysis.JetAnalyzers.JetResponse_cff")

process.MessageLogger = cms.Service("MessageLogger",
    cout = cms.untracked.PSet(
        threshold = cms.untracked.string('WARNING')
    ),
    destinations = cms.untracked.vstring('cout')
)

process.TFileService = cms.Service("TFileService",
    closeFileFast = cms.untracked.bool(True),
    fileName = cms.string('JRAt.root')
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('rfio:/castor/cern.ch/user/s/schiefer/JetAlgs/21X/QCD_80_120.root')
)

process.JetPtEta = cms.PSet(
    etaMin = cms.double(-5.0),
    etaMax = cms.double(5.0),
    ptMin = cms.double(1.0)
)
process.RefPtEta = cms.PSet(
    etaMin = cms.double(-5.0),
    etaMax = cms.double(5.0),
    ptMin = cms.double(1.0)
)
process.schedule = cms.Schedule(process.rsp_kt4calo,process.rsp_kt6calo,process.rsp_sc5calo,process.rsp_sc7calo,process.rsp_ic5calo,process.rsp_kt4pflow,process.rsp_kt6pflow,process.rsp_sc5pflow,process.rsp_sc7pflow,process.rsp_ic5pflow,process.rsp_kt4track,process.rsp_kt6track,process.rsp_sc5track,process.rsp_sc7track,process.rsp_ic5track)



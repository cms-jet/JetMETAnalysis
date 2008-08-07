import FWCore.ParameterSet.Config as cms


#!
#! ADJUST JET & REFERENCE PRESELECTION, RESPONSE ANALYSIS PARAMETERS
#!
import JetMETAnalysis.JetAnalyzers.Defaults_cff as Defaults;
import JetMETAnalysis.JetAnalyzers.JetRspParams_Tree_cff as Tree;

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
Defaults.JetResponseParameters = Tree.JetResponseParameters

#!
#! PROCESS
#!
process = cms.Process("JETCALIB")

process.load("JetMETAnalysis.JetAnalyzers.JetRspParams_Tree_cff")
process.load("JetMETAnalysis.JetAnalyzers.JetResponse_cff")


#!
#! INPUT
#!
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
    'rfio:/castor/cern.ch/user/s/schiefer/JetAlgs/21X/QCD_80_120.root'
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
    fileName      = cms.string('JRAt.root'),
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
    process.kt4pfJRA,
    process.kt6pfJRA,
    process.sc5pfJRA,
    process.sc7pfJRA,
    process.ic5pfJRA,
    process.kt4trkJRA,
    process.kt6trkJRA,
    process.sc5trkJRA,
    process.sc7trkJRA,
    process.ic5trkJRA
)

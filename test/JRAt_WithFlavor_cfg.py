import FWCore.ParameterSet.Config as cms

#!
#! ADJUST JET & REFERENCE PRESELECTION, RESPONSE ANALYSIS PARAMETERS
#!
import JetMETAnalysis.JetAnalyzers.JRA_Defaults_cff as Defaults;
import JetMETAnalysis.JetAnalyzers.JRA_TreeDefaults_cff as Tree;

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
process = cms.Process("JRA")

process.load("JetMETAnalysis.JetAnalyzers.JRA_TreeDefaults_cff")
process.load("JetMETAnalysis.JetAnalyzers.JRA_PathsWithFlavor_cff")


#!
#! INPUT
#!
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))
process.source = cms.Source(
    "PoolSource",
    fileNames = cms.untracked.vstring(
    '/store/relval/CMSSW_3_1_0_pre10/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/IDEAL_31X_v1/0008/FA9E955C-8857-DE11-85DC-001D09F28E80.root')
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
#! RECONSTRUCTION
#!

# particle flow reconstruction
#process.load("SchieferD.Configuration.PFReconstruction_cff")

# jet reconstruction
process.load("SchieferD.Configuration.JetReconstruction_cff")
#process.load("SchieferD.Configuration.FastjetReconstruction_cff")

# jet correction
process.load("SchieferD.Configuration.JetCorrection_cff")
process.prefer("L2L3JetCorrectorSC5Calo") 


#!
#! SCHEDULE
#!
process.schedule = cms.Schedule(
    # pflow reco
    #process.recoPF,
    # jet reco
    process.recoJets,
    # uncorrected jets
    process.kt4caloJRA,
    process.kt6caloJRA,
    process.sc5caloJRA,
    process.sc7caloJRA,
    process.ic5caloJRA,
    process.ak5caloJRA,
    process.ak7caloJRA,
    process.ca4caloJRA,
    process.ca6caloJRA,
    #process.gk5caloJRA,
    #process.gk7caloJRA,
    process.kt4pfJRA,
    process.kt6pfJRA,
    process.sc5pfJRA,
    process.sc7pfJRA,
    process.ic5pfJRA,
    process.ak5pfJRA,
    process.ak7pfJRA,
    process.ca4pfJRA,
    process.ca6pfJRA,
    #process.gk5pfJRA,
    #process.gk7pfJRA,
    #process.kt4trkJRA,
    #process.kt6trkJRA,
    #process.sc5trkJRA,
    #process.sc7trkJRA,
    #process.ic5trkJRA,
    #process.ak5trkJRA,
    #process.ak7trkJRA,
    #process.ca4trkJRA,
    #process.ca6trkJRA,
    #process.gk5trkJRA,
    #process.gk7trkJRA,
    process.ic5jptJRA,
    # correct jets
    process.correctJets,
    # corrected jets
    process.kt4calol2l3JRA,
    process.kt6calol2l3JRA,
    process.sc5calol2l3JRA,
    process.sc7calol2l3JRA,
    process.ic5calol2l3JRA,
    process.ak5calol2l3JRA,
    process.ak7calol2l3JRA,
    process.ca4calol2l3JRA,
    process.ca6calol2l3JRA,
    #process.gk5calol2l3JRA,
    #process.gk7calol2l3JRA,
    process.kt4pfl2l3JRA,
    process.kt6pfl2l3JRA,
    process.sc5pfl2l3JRA,
    process.sc7pfl2l3JRA,
    process.ic5pfl2l3JRA,
    process.ak5pfl2l3JRA,
    process.ak7pfl2l3JRA,
    process.ca4pfl2l3JRA,
    process.ca6pfl2l3JRA,
    #process.gk5pfl2l3JRA,
    #process.gk7pfl2l3JRA,
)


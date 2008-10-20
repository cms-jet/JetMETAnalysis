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
process = cms.Process("JETCALIB")

process.load("JetMETAnalysis.JetAnalyzers.JRA_TreeDefaults_cff")
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
    fileName      = cms.string('JRAt.root'),
    closeFileFast = cms.untracked.bool(True)
)



#!
#! PATHS
#!
process.load("RecoJets.Configuration.GenJetParticles_cff")
process.load("SchieferD.Configuration.RecoGenJets_cff")
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("SchieferD.Configuration.RecoCaloJets_cff")
process.load("SchieferD.Configuration.RecoPFJets_cff")
process.load("SchieferD.Configuration.RecoTrackJets_cff")

process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("JetMETCorrections.Configuration.JetPlusTrackCorrections_cff")
process.load("JetMETCorrections.Configuration.ZSPJetCorrections152_cff")

process.recoJets = cms.Path(process.genJetParticles+
                            process.recoGenJets+
                            process.recoCaloJets+
                            process.recoPFJets+
                            process.recoTrackJets+
                            process.ZSPJetCorrections+
                            process.JetPlusTrackCorrections)

process.load("JetMETCorrections.Configuration.L2L3Corrections_iCSA08_S156_cff")
process.prefer("L2L3JetCorrectorIcone5") 

process.correctJets = cms.Path(process.L2L3CorJetKt4+
                               process.L2L3CorJetKt6+
                               process.L2L3CorJetScone5+
                               process.L2L3CorJetScone7+
                               process.L2L3CorJetIcone5+
                               process.L2L3CorJetPFIcone5
                               )


#!
#! SCHEDULE
#!
process.schedule = cms.Schedule(
    # uncorrected jets
    process.recoJets,
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
    process.ic5trkJRA,
    process.ic5jptJRA,
    # corrected jets
    process.correctJets,
    process.kt4calol2l3JRA,
    process.kt6calol2l3JRA,
    process.sc5calol2l3JRA,
    process.sc7calol2l3JRA,
    process.ic5calol2l3JRA,
    process.ic5pfl2l3JRA
)


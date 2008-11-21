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
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(100))
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('file:/data/Summer08/QCDDijet30to50.root')
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
#! EVENT SETUP
#!
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.FakeConditions_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("RecoJets.Configuration.CaloTowersES_cfi")


#!
#! PATHS
#!

# track reconstruction (needed by pflow, NOT track-jet reconstruction!)
process.load("RecoLocalTracker.SiPixelRecHits.SiPixelRecHits_cfi")
process.load("RecoLocalTracker.SiStripRecHitConverter.SiStripRecHitConverter_cfi")
process.load("RecoTracker.Configuration.RecoTracker_cff")
process.recoTracks = cms.Path(process.siPixelRecHits*
                              process.siStripMatchedRecHits*
                              process.ckftracks)


# pflow reconstruction
process.load("RecoEgamma.EgammaElectronProducers.electronSequence_cff")
process.load("RecoParticleFlow.Configuration.RecoParticleFlow_cff")
process.recoPF = cms.Path(process.electronSequence*
                          process.particleFlowReco)

# jet reconstruction
process.load("RecoJets.Configuration.GenJetParticles_cff")
process.load("SchieferD.Configuration.RecoGenJets_cff")
process.load("SchieferD.Configuration.RecoCaloJets_cff")
process.load("SchieferD.Configuration.RecoPFJets_cff")
process.load("SchieferD.Configuration.RecoTrackJets_cff")

# jpt
process.load("JetMETCorrections.Configuration.JetPlusTrackCorrections_cff")
process.load("JetMETCorrections.Configuration.ZSPJetCorrections152_cff")

process.recoJets = cms.Path(process.genJetParticles+
                            process.recoGenJets+
                            process.recoCaloJets+
                            process.recoPFJets+
                            process.recoTrackJets+
                            process.ZSPJetCorrections+
                            process.JetPlusTrackCorrections)


# jet correction
process.load("JetMETCorrections.Configuration.L2L3Corrections_Summer08_cff")
process.prefer("L2L3JetCorrectorSC5Calo") 

process.correctJets = cms.Path(process.L2L3CorJetKT4Calo+
                               process.L2L3CorJetKT6Calo+
                               process.L2L3CorJetSC5Calo+
                               process.L2L3CorJetSC7Calo+
                               process.L2L3CorJetIC5Calo+
                               process.L2L3CorJetKT4PF+
                               process.L2L3CorJetKT6PF+
                               process.L2L3CorJetSC5PF+
                               process.L2L3CorJetSC7PF+
                               process.L2L3CorJetIC5PF
                               )


#!
#! SCHEDULE
#!
process.schedule = cms.Schedule(
    # track reco
    process.recoTracks,
    # pflow reco
    process.recoPF,
    # jet reco
    process.recoJets,
    # uncorrected jets
    process.kt4caloJRA,
    process.kt6caloJRA,
    process.sc5caloJRA,
    process.sc7caloJRA,
    process.ic5caloJRA,
#    process.ak5caloJRA, # NEW
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
    # correct jets
    process.correctJets,
    # corrected jets
    process.kt4calol2l3JRA,
    process.kt6calol2l3JRA,
    process.sc5calol2l3JRA,
    process.sc7calol2l3JRA,
    process.ic5calol2l3JRA,
    process.ic5pfl2l3JRA,
    process.kt4pfl2l3JRA,
    process.kt6pfl2l3JRA,
    process.sc5pfl2l3JRA,
    process.sc7pfl2l3JRA
)


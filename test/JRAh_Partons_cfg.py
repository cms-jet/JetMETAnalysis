import FWCore.ParameterSet.Config as cms

#!
#! ADJUST PT/ETA PRESELECTION AND RESPONSE ANALYSIS PARAMETERS
#!
import JetMETAnalysis.JetAnalyzers.JRA_Defaults_cff as Defaults;

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
Defaults.JetResponseParameters = cms.PSet(
    # write histograms, no trees
    doHistos = cms.bool(True),
    doTree   = cms.bool(False),
    # *do* record flavor information, consider both RefPt and JetPt
    doFlavor = cms.bool(True),
    doRefPt  = cms.bool(True),
    doJetPt  = cms.bool(True),
    # MATCHING MODE: deltaR
    deltaRMax = cms.double(0.3),
    # consider all matched references
    nRefMax = cms.uint32(0),
    # define the barrel calorimeter region
    etaBarrelMin = cms.double(-1.0),
    etaBarrelMax = cms.double(1.0),
    # record relative response jtpt/refpt, set histogram x-axis range
    nBinsRelRsp = cms.uint32(100),
    relRspMin   = cms.double(0.0),
    relRspMax   = cms.double(2.0),
    # record absolute response jtpt-refpt, set histogram x-axis range
    nBinsAbsRsp = cms.uint32(100),
    absRspMin   = cms.double(1.0),
    absRspMax   = cms.double(0.0),
    # set number of bins for pT, eta, and phi distributions
    nBinsPt  = cms.uint32(50),
    nBinsEta = cms.uint32(25),
    nBinsPhi = cms.uint32(25),
    # pT binning
    binsPt = cms.vdouble(
    10.0, 20.0, 35.0, 57.0, 90.0, 120.0, 150.0, 300.0, 750.0, 1000.0
    ),
    # eta binning
    binsEta = cms.vdouble(
    -2.0, -1.7, -1.4, -1.1, -0.8, -0.5, -0.2, 0.2, 0.5, 0.8, 1.1, 1.4, 1.7, 2.0
    ),
    # phi binning
    binsPhi = cms.vdouble(
    -3.141, -2.7, -2.1, -1.5, -0.9,-0.3, 0.3, 0.9, 1.5, 2.1,2.7, 3.141
    )
)


#!
#! PROCESS
#!
process = cms.Process("JRA")


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
    fileName      = cms.string('JRAp.root'),
    closeFileFast = cms.untracked.bool(True)
)


#!
#! MODULES
#!
process.partons = cms.EDFilter("PdgIdAndStatusCandViewSelector",
    src    = cms.InputTag("genParticles"),
    status = cms.vint32(3),
    pdgId  = cms.vint32(1, 2, 3, 4, 5, 21)
)

process.kt4caloPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("kt4CaloJets")
)

process.kt4caloToParton = cms.EDFilter("MatchRecToGen",
    srcRec = cms.InputTag("kt4caloPtEta"),
    srcGen = cms.InputTag("partons")
)

process.kt4calo = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRef         = cms.InputTag("partons"),
    srcRefToJetMap = cms.InputTag("kt4caloToParton","gen2rec")
)

process.kt4caloJRA = cms.Path(
    process.partons+
    process.kt4caloPtEta+
    process.kt4caloToParton+
    process.kt4calo
)


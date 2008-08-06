import FWCore.ParameterSet.Config as cms

process = cms.Process("JETCALIB")
process.MessageLogger = cms.Service("MessageLogger",
    cout = cms.untracked.PSet(
        threshold = cms.untracked.string('WARNING')
    ),
    destinations = cms.untracked.vstring('cout')
)

process.TFileService = cms.Service("TFileService",
    closeFileFast = cms.untracked.bool(True),
    fileName = cms.string('JRAp.root')
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('/store/relvall/2008/5/4/RelVal-RelValQCD_Pt_80_120-1209247429-IDEAL_V1-2nd/0000/0CA5A748-651A-DD11-BD7B-000423D992A4.root')
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
process.JetResponseParameters = cms.PSet(
    nBinsRelRsp = cms.uint32(100),
    nBinsEta = cms.uint32(25),
    doFlavor = cms.bool(True),
    binsPt = cms.vdouble(10.0, 20.0, 35.0, 57.0, 90.0, 
        120.0, 150.0, 300.0, 750.0, 1000.0),
    doRefPt = cms.bool(True),
    nBinsPt = cms.uint32(50),
    binsEta = cms.vdouble(-2.0, -1.7, -1.4, -1.1, -0.8, 
        -0.5, -0.2, 0.2, 0.5, 0.8, 
        1.1, 1.4, 1.7, 2.0),
    relRspMin = cms.double(0.0),
    absRspMax = cms.double(0.0),
    doJetPt = cms.bool(True),
    etaBarrelMin = cms.double(-1.0),
    nBinsPhi = cms.uint32(25),
    deltaRMax = cms.double(0.3),
    nBinsAbsRsp = cms.uint32(100),
    doHistos = cms.bool(True),
    etaBarrelMax = cms.double(1.0),
    nRefMax = cms.uint32(0),
    absRspMin = cms.double(1.0),
    doTree = cms.bool(False),
    binsPhi = cms.vdouble(-3.141, -2.7, -2.1, -1.5, -0.9, 
        -0.3, 0.3, 0.9, 1.5, 2.1, 
        2.7, 3.141),
    relRspMax = cms.double(2.0)
)
process.partons = cms.EDFilter("PdgIdAndStatusCandViewSelector",
    status = cms.vint32(3),
    src = cms.InputTag("genParticles"),
    pdgId = cms.vint32(1, 2, 3, 4, 5, 
        21)
)

process.kt4CaloPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    process.JetPtEta,
    src = cms.InputTag("kt4CaloJets")
)

process.kt4PartonToCalo = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("partons"),
    srcRec = cms.InputTag("kt4CaloPtEta")
)

process.kt4calo = cms.EDAnalyzer("JetResponseAnalyzer",
    process.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("kt4PartonToCalo","gen2rec"),
    srcRef = cms.InputTag("partons")
)

process.rsp_kt4calo = cms.Path(process.partons+process.kt4CaloPtEta+process.kt4PartonToCalo+process.kt4calo)


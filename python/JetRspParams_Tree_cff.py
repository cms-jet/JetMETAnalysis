import FWCore.ParameterSet.Config as cms

JetResponseParameters = cms.PSet(
    doFlavor = cms.bool(False),
    doRefPt = cms.bool(True),
    nBinsRelRsp = cms.uint32(0),
    doJetPt = cms.bool(True),
    deltaRMax = cms.double(0.3),
    nBinsAbsRsp = cms.uint32(0),
    doHistos = cms.bool(False),
    doTree = cms.bool(True),
    nRefMax = cms.uint32(0)
)


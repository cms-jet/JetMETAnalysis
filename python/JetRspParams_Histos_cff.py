import FWCore.ParameterSet.Config as cms

################################################################################
#
# JetResponseAnalyzer parameters for 'doHistos' mode
# --------------------------------------------------
#
#            https://twiki.cern.ch/twiki/bin/view/CMS/SWGuideJetResponseAnalyzer
################################################################################

JetResponseParameters = cms.PSet(
    # write histograms, no trees
    doHistos = cms.bool(True),
    doTree   = cms.bool(False),
    # don't record flavor information, consider both RefPt and JetPt
    doFlavor = cms.bool(False),
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
    nBinsAbsRsp = cms.uint32(600),
    absRspMin   = cms.double(-1000.0),
    absRspMax   = cms.double(200.0),
    # set number of bins for pT, eta, and phi distributions
    nBinsPt  = cms.uint32(50),
    nBinsEta = cms.uint32(25),
    nBinsPhi = cms.uint32(25),
    # pT binning
    binsPt = cms.vdouble(
    5.0,    10.0,   12.0,   15.0,   20.0,   27.0,   35.0,   45.0,   57.0,   72.0, 
    90.0,   120.0,  150.0,  200.0,  300.0,  400.0,  550.0,  750.0,  1000.0, 1500.0, 
    2000.0, 2500.0, 3000.0, 3500.0, 4000.0, 4500.0, 5000.0, 10000.0
    ),
    # eta binning
    binsEta = cms.vdouble(
    -5.191,-4.889,-4.716,-4.538,-4.363,-4.191,-4.013,-3.839,-3.664,
    -3.489,-3.314,-3.139,-2.964,-2.853,-2.650,-2.500,-2.322,-2.172,
    -2.043,-1.930,-1.830,-1.740,-1.653,-1.566,-1.479,-1.392,-1.305,
    -1.218,-1.131,-1.044,-0.957,-0.879,-0.783,-0.696,-0.609,-0.522,
    -0.435,-0.348,-0.261,-0.174,-0.087,
    +0.000,
    +0.087,+0.174,+0.261,+0.348,+0.435,+0.522,+0.609,+0.696,+0.783,
    +0.879,+0.957,+1.044,+1.131,+1.218,+1.305,+1.392,+1.479,+1.566,
    +1.653,+1.740,+1.830,+1.930,+2.043,+2.172,+2.322,+2.500,+2.650,
    +2.853,+2.964,+3.139,+3.314,+3.489,+3.664,+3.839,+4.013,+4.191,
    +4.363,+4.538,+4.716,+4.889,+5.191
    ),
    # phi binning
    binsPhi = cms.vdouble(
    -3.141, -2.7, -2.1, -1.5, -0.9,-0.3, 0.3, 0.9, 1.5, 2.1,2.7, 3.141
    )
)


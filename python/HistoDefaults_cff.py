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
    deltaRMax = cms.double(0.2),
    # deltaR(ref,parton) IF doFlavor is True
    deltaRPartonMax = cms.double(0.25),
    # consider all matched references
    nRefMax = cms.uint32(0),
    # define the barrel calorimeter region
    etaBarrelMin = cms.double(-1.3),
    etaBarrelMax = cms.double(1.3),
    # record relative response jtpt/refpt, set histogram x-axis range
    nBinsRelRsp = cms.uint32( 50),
    relRspMin   = cms.double(0.0),
    relRspMax   = cms.double(2.0),
    # record absolute response jtpt-refpt, set histogram x-axis range
    nBinsAbsRsp = cms.uint32(250),
    absRspMin   = cms.double(-400.0),
    absRspMax   = cms.double(150.0),
    # record eta response |jteta|-|refpt|, set histogram x-axis range
    nBinsEtaRsp = cms.uint32(50),
    etaRspMin   = cms.double(-1.0),
    etaRspMax   = cms.double(+1.0),
    # record phi response jtphi-refphi, set histogram x-axis range
    nBinsPhiRsp = cms.uint32(50),
    phiRspMin   = cms.double(-1.0),
    phiRspMax   = cms.double(+1.0),
    # set number of bins for pT, eta, and phi distributions
    nBinsPt  = cms.uint32(50),
    nBinsEta = cms.uint32(25),
    nBinsPhi = cms.uint32(25),
    # pT binning
    binsPt = cms.vdouble(15.,20.,27.,35.,50.,80.,120.0,200.,300.,500.,800.,1200.),
    # eta binning
    binsEta = cms.vdouble(0.0,1.4,2.6,3.2,4.7),
    # phi binning
    binsPhi = cms.vdouble(-3.1,-2.7,-2.1,-1.5,-0.9,-0.3,0.,0.3,0.9,1.5,2.1,2.7,3.1)
    )


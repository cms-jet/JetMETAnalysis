import FWCore.ParameterSet.Config as cms

################################################################################
#
# JetResponseAnalyzer parameters for 'doTree' mode
# ------------------------------------------------
#
#            https://twiki.cern.ch/twiki/bin/view/CMS/SWGuideJetResponseAnalyzer
################################################################################


JetResponseParameters = cms.PSet(
    # write tree, no histograms
    doHistos = cms.bool(False),
    doTree   = cms.bool(True),
    # DON'T record flavor information, consider both RefPt and JetPt
    doFlavor = cms.bool(False),
    doRefPt  = cms.bool(True),
    doJetPt  = cms.bool(True),
    # MATCHING MODE: deltaR(ref,jet)
    deltaRMax = cms.double(0.3),
    # consider all matched references
    nRefMax = cms.uint32(0)#,
    # don't record relative / absolute response histograms
    #nBinsRelRsp = cms.uint32(0),
    #nBinsAbsRsp = cms.uint32(0)
)

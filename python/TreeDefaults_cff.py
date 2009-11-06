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
    doHistos        = cms.bool(False),
    doTree          = cms.bool(True),
    # record flavor information, consider both RefPt and JetPt
    doFlavor        = cms.bool(True),
    doRefPt         = cms.bool(True),
    doJetPt         = cms.bool(True),
    # MATCHING MODE: deltaR(ref,jet)
    deltaRMax       = cms.double(99.9),
    # deltaR(ref,parton) IF doFlavor is True
    deltaRPartonMax = cms.double(0.25),
    # consider all matched references
    nRefMax         = cms.uint32(0)
)

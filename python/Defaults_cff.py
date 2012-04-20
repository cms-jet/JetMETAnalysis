import FWCore.ParameterSet.Config as cms


#!
#! DEFAULT JET PRESELECTION
#!
JetPtEta = cms.PSet( 
    etaMin = cms.double(-5.0),
    etaMax = cms.double(5.0),
    ptMin = cms.double(1.0)
)

#!
#! DEFAULT REF PRESELECTION
#!
RefPtEta = cms.PSet(
    etaMin = cms.double(-5.0),
    etaMax = cms.double(5.0),
    ptMin = cms.double(1.0)
)

#!
#! DEFAULT RESPONSE ANALYSIS PARAMETERS
#!
JetResponseParameters = cms.PSet(
    # record flavor information, consider both RefPt and JetPt
    doComposition   = cms.bool(False),
    doFlavor        = cms.bool(True),
    doRefPt         = cms.bool(True),
    doJetPt         = cms.bool(True),
    # MATCHING MODE: deltaR(ref,jet)
    deltaRMax       = cms.double(99.9),
    # deltaR(ref,parton) IF doFlavor is True
    deltaRPartonMax = cms.double(0.25),
    # consider all matched references
    nRefMax         = cms.uint32(0),
    # is the sample an HLT sample
    doHLT           = cms.bool(False)
)

#!
#! DEFAULT KT6CALOJET PARAMETERS
#!
kt6CaloJetParameters = cms.PSet(
    Rho_EtaMax   = cms.double(4.4), # FIX LATER
    Ghost_EtaMax = cms.double(5.0)
)

#!
#! DEFAULT KT6PFJET PARAMETERS
#!
kt6PFJetParameters = cms.PSet(
    Rho_EtaMax   = cms.double(4.4), # FIX LATER
    Ghost_EtaMax = cms.double(5.0)
)

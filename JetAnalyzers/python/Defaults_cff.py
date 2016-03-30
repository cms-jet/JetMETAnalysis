import FWCore.ParameterSet.Config as cms


#!
#! DEFAULT JET PRESELECTION
#!
JetPtEta = cms.PSet( 
    etaMin = cms.double(-5.5),
    etaMax = cms.double(5.5),
    ptMin = cms.double(1.0)
)

#!
#! DEFAULT REF PRESELECTION
#!
RefPtEta = cms.PSet(
    etaMin = cms.double(-5.5),
    etaMax = cms.double(5.5),
    ptMin = cms.double(1.0)
)

#!
#! DEFAULT RESPONSE ANALYSIS PARAMETERS
#!
JetResponseParameters = cms.PSet(
    # record flavor information, consider both RefPt and JetPt
    doComposition   = cms.bool(True),
    doFlavor        = cms.bool(True),
    doRefPt         = cms.bool(True),
    doJetPt         = cms.bool(True),
    saveCandidates  = cms.bool(False),
    # MATCHING MODE: deltaR(ref,jet)
    deltaRMax       = cms.double(0.25),
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

#!
#! DEFAULT jet_response_analyzer PARAMETERS
#!
jet_response_parameters = cms.PSet(
    binspt            = cms.vdouble(10,10.5,11,11.5,12,12.5,13,13.5,14,15,17,20,23,27,30,35,40,45,57,72,90,120,150,200,300,400,550,750,1000,1500,2000,2500,3000,3500,4000,4500,5000,10000),
    binseta           = cms.vdouble(-5.191,-4.889,-4.716,-4.538,-4.363,-4.191,-4.013,-3.839,-3.664,-3.489,-3.314,-3.139,-2.964,-2.853,-2.65, -2.5,  -2.322,-2.172,-2.043,-1.93,-1.83,  -1.74, -1.653,-1.566,-1.479,-1.392,-1.305,-1.218,-1.131,-1.044,-0.957, -0.879,-0.783,-0.696,-0.609,-0.522,-0.435,-0.348,-0.261,-0.174,-0.087, 0,0.087, 0.174, 0.261, 0.348, 0.435 ,0.522, 0.609, 0.696,0.783,  0.879, 0.957, 1.044, 1.131, 1.218, 1.305 ,1.392, 1.479, 1.566,1.653,  1.74,  1.83,  1.93,  2.043, 2.172, 2.322 ,2.5,   2.65,  2.853,2.964,  3.139, 3.314, 3.489, 3.664, 3.839, 4.013 ,4.191, 4.363, 4.538,4.716,4.889,5.191),
    binsphi           = cms.vdouble(-3.141,-2.700,-2.100,-1.500,-0.900,-0.300,0.300,0.900,1.500,2.100,2.700,3.141),
    binsy             = cms.vdouble(),
    treename          = cms.string("t"),
    useweight         = cms.bool(False),
    xsection          = cms.double(0.0),
    nrefmax           = cms.int32(0),
    nbinspt           = cms.int32(50),
    nbinseta          = cms.int32(25),
    nbinsphi          = cms.int32(25),
    nbinsy            = cms.int32(25),
    etabarrelmin      = cms.double(-1.3),
    etabarrelmax      = cms.double(+1.3),
    dobalance         = cms.bool(False),
    doflavor          = cms.bool(False),
    noabsflavors      = cms.bool(False),
    drmax             = cms.double(0.3),
    dphimin           = cms.double(2.7),
    dojetpt           = cms.bool(False),
    dorefpt           = cms.bool(True),
    nbinsrelrsp       = cms.int32(50),
    relrspmin         = cms.double(0.0),
    relrspmax         = cms.double(2.0),
    nbinsabsrsp       = cms.int32(0),
    absrspmin         = cms.double(-250.0),
    absrspmax         = cms.double(100.0),
    nbinsetarsp       = cms.int32(100),
    etarspmin         = cms.double(-1.0),
    etarspmax         = cms.double(1.0),
    nbinsphirsp       = cms.int32(100),
    phirspmin         = cms.double(-1.0),
    phirspmax         = cms.double(1.0),
    jtptmin           = cms.double(1.0),
    presel            = cms.vstring(""),
    itlow             = cms.int32(0),
    ithigh            = cms.int32(1000),
    earlyootlow       = cms.int32(0),
    earlyoothigh      = cms.int32(1000),
    lateootlow        = cms.int32(0),
    lateoothigh       = cms.int32(1000),
    totalootlow       = cms.int32(0),
    totaloothigh      = cms.int32(1000),
    weightfile        = cms.string(""),
    MCPUReWeighting   = cms.string(""),
    DataPUReWeighting = cms.string("")
)

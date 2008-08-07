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
JetResponseParameters = cms.PSet()

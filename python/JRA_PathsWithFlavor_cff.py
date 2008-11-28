import FWCore.ParameterSet.Config as cms

from JetMETAnalysis.JetAnalyzers.JRA_Modules_cff import *


#
# MODULES with flavor of genjet in jet response analysis added
#

# calo
kt4calo.doFlavor          = cms.bool(True)
kt4calo.srcRefToPartonMap = cms.InputTag('kt4GenToParton','rec2gen')
kt4calo.deltaRPartonMax   = cms.double(0.25)
kt6calo.doFlavor          = cms.bool(True)
kt6calo.srcRefToPartonMap = cms.InputTag('kt6GenToParton','rec2gen')
kt6calo.deltaRPartonMax   = cms.double(0.25)
sc5calo.doFlavor          = cms.bool(True)
sc5calo.srcRefToPartonMap = cms.InputTag('sc5GenToParton','rec2gen')
sc5calo.deltaRPartonMax   = cms.double(0.25)
sc7calo.doFlavor          = cms.bool(True)
sc7calo.srcRefToPartonMap = cms.InputTag('sc7GenToParton','rec2gen')
sc7calo.deltaRPartonMax   = cms.double(0.25)
ic5calo.doFlavor          = cms.bool(True)
ic5calo.srcRefToPartonMap = cms.InputTag('ic5GenToParton','rec2gen')
ic5calo.deltaRPartonMax   = cms.double(0.25)
ak5calo.doFlavor          = cms.bool(True)
ak5calo.srcRefToPartonMap = cms.InputTag('ak5GenToParton','rec2gen')
ak5calo.deltaRPartonMax   = cms.double(0.25)
ak7calo.doFlavor          = cms.bool(True)
ak7calo.srcRefToPartonMap = cms.InputTag('ak7GenToParton','rec2gen')
ak7calo.deltaRPartonMax   = cms.double(0.25)
ca4calo.doFlavor          = cms.bool(True)
ca4calo.srcRefToPartonMap = cms.InputTag('ca4GenToParton','rec2gen')
ca4calo.deltaRPartonMax   = cms.double(0.25)
ca6calo.doFlavor          = cms.bool(True)
ca6calo.srcRefToPartonMap = cms.InputTag('ca6GenToParton','rec2gen')
ca6calo.deltaRPartonMax   = cms.double(0.25)


# pflow
kt4pf.doFlavor          = cms.bool(True)
kt4pf.srcRefToPartonMap = cms.InputTag('kt4GenToParton','rec2gen')
kt4pf.deltaRPartonMax   = cms.double(0.25)
kt6pf.doFlavor          = cms.bool(True)
kt6pf.srcRefToPartonMap = cms.InputTag('kt6GenToParton','rec2gen')
kt6pf.deltaRPartonMax   = cms.double(0.25)
sc5pf.doFlavor          = cms.bool(True)
sc5pf.srcRefToPartonMap = cms.InputTag('sc5GenToParton','rec2gen')
sc5pf.deltaRPartonMax   = cms.double(0.25)
sc7pf.doFlavor          = cms.bool(True)
sc7pf.srcRefToPartonMap = cms.InputTag('sc7GenToParton','rec2gen')
sc7pf.deltaRPartonMax   = cms.double(0.25)
ic5pf.doFlavor          = cms.bool(True)
ic5pf.srcRefToPartonMap = cms.InputTag('ic5GenToParton','rec2gen')
ic5pf.deltaRPartonMax   = cms.double(0.25)
ak5pf.doFlavor          = cms.bool(True)
ak5pf.srcRefToPartonMap = cms.InputTag('ak5GenToParton','rec2gen')
ak5pf.deltaRPartonMax   = cms.double(0.25)
ak7pf.doFlavor          = cms.bool(True)
ak7pf.srcRefToPartonMap = cms.InputTag('ak7GenToParton','rec2gen')
ak7pf.deltaRPartonMax   = cms.double(0.25)
ca4pf.doFlavor          = cms.bool(True)
ca4pf.srcRefToPartonMap = cms.InputTag('ca4GenToParton','rec2gen')
ca4pf.deltaRPartonMax   = cms.double(0.25)
ca6pf.doFlavor          = cms.bool(True)
ca6pf.srcRefToPartonMap = cms.InputTag('ca6GenToParton','rec2gen')
ca6pf.deltaRPartonMax   = cms.double(0.25)


# track
kt4trk.doFlavor          = cms.bool(True)
kt4trk.srcRefToPartonMap = cms.InputTag('kt4GenToParton','rec2gen')
kt4trk.deltaRPartonMax   = cms.double(0.25)
kt6trk.doFlavor          = cms.bool(True)
kt6trk.srcRefToPartonMap = cms.InputTag('kt6GenToParton','rec2gen')
kt6trk.deltaRPartonMax   = cms.double(0.25)
sc5trk.doFlavor          = cms.bool(True)
sc5trk.srcRefToPartonMap = cms.InputTag('sc5GenToParton','rec2gen')
sc5trk.deltaRPartonMax   = cms.double(0.25)
sc7trk.doFlavor          = cms.bool(True)
sc7trk.srcRefToPartonMap = cms.InputTag('sc7GenToParton','rec2gen')
sc7trk.deltaRPartonMax   = cms.double(0.25)
ic5trk.doFlavor          = cms.bool(True)
ic5trk.srcRefToPartonMap = cms.InputTag('ic5GenToParton','rec2gen')
ic5trk.deltaRPartonMax   = cms.double(0.25)
ak5trk.doFlavor          = cms.bool(True)
ak5trk.srcRefToPartonMap = cms.InputTag('ak5GenToParton','rec2gen')
ak5trk.deltaRPartonMax   = cms.double(0.25)
ak7trk.doFlavor          = cms.bool(True)
ak7trk.srcRefToPartonMap = cms.InputTag('ak7GenToParton','rec2gen')
ak7trk.deltaRPartonMax   = cms.double(0.25)
ca4trk.doFlavor          = cms.bool(True)
ca4trk.srcRefToPartonMap = cms.InputTag('ca4GenToParton','rec2gen')
ca4trk.deltaRPartonMax   = cms.double(0.25)
ca6trk.doFlavor          = cms.bool(True)
ca6trk.srcRefToPartonMap = cms.InputTag('ca6GenToParton','rec2gen')
ca6trk.deltaRPartonMax   = cms.double(0.25)


# jpt
ic5jpt.doFlavor          = cms.bool(True)
ic5jpt.srcRefToPartonMap = cms.InputTag('ic5GenToParton','rec2gen')
ic5jpt.deltaRPartonMax   = cms.double(0.25)


# calol2l3
kt4calol2l3.doFlavor          = cms.bool(True)
kt4calol2l3.srcRefToPartonMap = cms.InputTag('kt4GenToParton','rec2gen')
kt4calol2l3.deltaRPartonMax   = cms.double(0.25)
kt6calol2l3.doFlavor          = cms.bool(True)
kt6calol2l3.srcRefToPartonMap = cms.InputTag('kt6GenToParton','rec2gen')
kt6calol2l3.deltaRPartonMax   = cms.double(0.25)
sc5calol2l3.doFlavor          = cms.bool(True)
sc5calol2l3.srcRefToPartonMap = cms.InputTag('sc5GenToParton','rec2gen')
sc5calol2l3.deltaRPartonMax   = cms.double(0.25)
sc7calol2l3.doFlavor          = cms.bool(True)
sc7calol2l3.srcRefToPartonMap = cms.InputTag('sc7GenToParton','rec2gen')
sc7calol2l3.deltaRPartonMax   = cms.double(0.25)
ic5calol2l3.doFlavor          = cms.bool(True)
ic5calol2l3.srcRefToPartonMap = cms.InputTag('ic5GenToParton','rec2gen')
ic5calol2l3.deltaRPartonMax   = cms.double(0.25)
ak5calol2l3.doFlavor          = cms.bool(True)
ak5calol2l3.srcRefToPartonMap = cms.InputTag('ak5GenToParton','rec2gen')
ak5calol2l3.deltaRPartonMax   = cms.double(0.25)
ak7calol2l3.doFlavor          = cms.bool(True)
ak7calol2l3.srcRefToPartonMap = cms.InputTag('ak7GenToParton','rec2gen')
ak7calol2l3.deltaRPartonMax   = cms.double(0.25)
ca4calol2l3.doFlavor          = cms.bool(True)
ca4calol2l3.srcRefToPartonMap = cms.InputTag('ca4GenToParton','rec2gen')
ca4calol2l3.deltaRPartonMax   = cms.double(0.25)
ca6calol2l3.doFlavor          = cms.bool(True)
ca6calol2l3.srcRefToPartonMap = cms.InputTag('ca6GenToParton','rec2gen')
ca6calol2l3.deltaRPartonMax   = cms.double(0.25)


# pfl2l3
kt4pfl2l3.doFlavor          = cms.bool(True)
kt4pfl2l3.srcRefToPartonMap = cms.InputTag('kt4GenToParton','rec2gen')
kt4pfl2l3.deltaRPartonMax   = cms.double(0.25)
kt6pfl2l3.doFlavor          = cms.bool(True)
kt6pfl2l3.srcRefToPartonMap = cms.InputTag('kt6GenToParton','rec2gen')
kt6pfl2l3.deltaRPartonMax   = cms.double(0.25)
sc5pfl2l3.doFlavor          = cms.bool(True)
sc5pfl2l3.srcRefToPartonMap = cms.InputTag('sc5GenToParton','rec2gen')
sc5pfl2l3.deltaRPartonMax   = cms.double(0.25)
sc7pfl2l3.doFlavor          = cms.bool(True)
sc7pfl2l3.srcRefToPartonMap = cms.InputTag('sc7GenToParton','rec2gen')
sc7pfl2l3.deltaRPartonMax   = cms.double(0.25)
ic5pfl2l3.doFlavor          = cms.bool(True)
ic5pfl2l3.srcRefToPartonMap = cms.InputTag('ic5GenToParton','rec2gen')
ic5pfl2l3.deltaRPartonMax   = cms.double(0.25)
ak5pfl2l3.doFlavor          = cms.bool(True)
ak5pfl2l3.srcRefToPartonMap = cms.InputTag('ak5GenToParton','rec2gen')
ak5pfl2l3.deltaRPartonMax   = cms.double(0.25)
ak7pfl2l3.doFlavor          = cms.bool(True)
ak7pfl2l3.srcRefToPartonMap = cms.InputTag('ak7GenToParton','rec2gen')
ak7pfl2l3.deltaRPartonMax   = cms.double(0.25)
ca4pfl2l3.doFlavor          = cms.bool(True)
ca4pfl2l3.srcRefToPartonMap = cms.InputTag('ca4GenToParton','rec2gen')
ca4pfl2l3.deltaRPartonMax   = cms.double(0.25)
ca6pfl2l3.doFlavor          = cms.bool(True)
ca6pfl2l3.srcRefToPartonMap = cms.InputTag('ca6GenToParton','rec2gen')
ca6pfl2l3.deltaRPartonMax   = cms.double(0.25)



#
# PATHS with flavor
#

# calo
kt4caloJRA = cms.Path(
    kt4caloPtEta+kt4genPtEta+kt4caloJetToRef+partons+kt4GenToParton+kt4calo
    )
kt6caloJRA = cms.Path(
    kt6caloPtEta+kt6genPtEta+kt6caloJetToRef+partons+kt6GenToParton+kt6calo
    )
sc5caloJRA = cms.Path(
    sc5caloPtEta+sc5genPtEta+sc5caloJetToRef+partons+sc5GenToParton+sc5calo
    )
sc7caloJRA = cms.Path(
    sc7caloPtEta+sc7genPtEta+sc7caloJetToRef+partons+sc7GenToParton+sc7calo
    )
ic5caloJRA = cms.Path(
    ic5caloPtEta+ic5genPtEta+ic5caloJetToRef+partons+ic5GenToParton+ic5calo
    )
ak5caloJRA = cms.Path(
    ak5caloPtEta+ak5genPtEta+ak5caloJetToRef+partons+ak5GenToParton+ak5calo
    )
ak7caloJRA = cms.Path(
    ak7caloPtEta+ak7genPtEta+ak7caloJetToRef+partons+ak7GenToParton+ak7calo
    )
ca4caloJRA = cms.Path(
    ca4caloPtEta+ca4genPtEta+ca4caloJetToRef+partons+ca4GenToParton+ca4calo
    )
ca6caloJRA = cms.Path(
    ca6caloPtEta+ca6genPtEta+ca6caloJetToRef+partons+ca6GenToParton+ca6calo
    )


# pf
kt4pfJRA = cms.Path(
    kt4pfPtEta+kt4genPtEta+kt4pfJetToRef+partons+kt4GenToParton+kt4pf
    )
kt6pfJRA = cms.Path(
    kt6pfPtEta+kt6genPtEta+kt6pfJetToRef+partons+kt6GenToParton+kt6pf
    )
sc5pfJRA = cms.Path(
    sc5pfPtEta+sc5genPtEta+sc5pfJetToRef+partons+sc5GenToParton+sc5pf
    )
sc7pfJRA = cms.Path(
    sc7pfPtEta+sc7genPtEta+sc7pfJetToRef+partons+sc7GenToParton+sc7pf
    )
ic5pfJRA = cms.Path(
    ic5pfPtEta+ic5genPtEta+ic5pfJetToRef+partons+ic5GenToParton+ic5pf
    )
ak5pfJRA = cms.Path(
    ak5pfPtEta+ak5genPtEta+ak5pfJetToRef+partons+ak5GenToParton+ak5pf
    )
ak7pfJRA = cms.Path(
    ak7pfPtEta+ak7genPtEta+ak7pfJetToRef+partons+ak7GenToParton+ak7pf
    )
ca4pfJRA = cms.Path(
    ca4pfPtEta+ca4genPtEta+ca4pfJetToRef+partons+ca4GenToParton+ca4pf
    )
ca6pfJRA = cms.Path(
    ca6pfPtEta+ca6genPtEta+ca6pfJetToRef+partons+ca6GenToParton+ca6pf
    )


# trk
kt4trkJRA = cms.Path(
    kt4trkPtEta+kt4genPtEta+kt4trkJetToRef+partons+kt4GenToParton+kt4trk
    )
kt6trkJRA = cms.Path(
    kt6trkPtEta+kt6genPtEta+kt6trkJetToRef+partons+kt6GenToParton+kt6trk
    )
sc5trkJRA = cms.Path(
    sc5trkPtEta+sc5genPtEta+sc5trkJetToRef+partons+sc5GenToParton+sc5trk
    )
sc7trkJRA = cms.Path(
    sc7trkPtEta+sc7genPtEta+sc7trkJetToRef+partons+sc7GenToParton+sc7trk
    )
ic5trkJRA = cms.Path(
    ic5trkPtEta+ic5genPtEta+ic5trkJetToRef+partons+ic5GenToParton+ic5trk
    )
ak5trkJRA = cms.Path(
    ak5trkPtEta+ak5genPtEta+ak5trkJetToRef+partons+ak5GenToParton+ak5trk
    )
ak7trkJRA = cms.Path(
    ak7trkPtEta+ak7genPtEta+ak7trkJetToRef+partons+ak7GenToParton+ak7trk
    )
ca4trkJRA = cms.Path(
    ca4trkPtEta+ca4genPtEta+ca4trkJetToRef+partons+ca4GenToParton+ca4trk
    )
ca6trkJRA = cms.Path(
    ca6trkPtEta+ca6genPtEta+ca6trkJetToRef+partons+ca6GenToParton+ca6trk
    )


# jpt
ic5jptJRA = cms.Path(
    ic5jptPtEta+ic5genPtEta+ic5jptJetToRef+partons+ic5GenToParton+ic5jpt
    )


# calol2l3
kt4calol2l3JRA = cms.Path(
    kt4calol2l3PtEta+kt4genPtEta+kt4calol2l3JetToRef+partons+kt4GenToParton+kt4calol2l3
    )
kt6calol2l3JRA = cms.Path(
    kt6calol2l3PtEta+kt6genPtEta+kt6calol2l3JetToRef+partons+kt6GenToParton+kt6calol2l3
    )
sc5calol2l3JRA = cms.Path(
    sc5calol2l3PtEta+sc5genPtEta+sc5calol2l3JetToRef+partons+sc5GenToParton+sc5calol2l3
    )
sc7calol2l3JRA = cms.Path(
    sc7calol2l3PtEta+sc7genPtEta+sc7calol2l3JetToRef+partons+sc7GenToParton+sc7calol2l3
    )
ic5calol2l3JRA = cms.Path(
    ic5calol2l3PtEta+ic5genPtEta+ic5calol2l3JetToRef+partons+ic5GenToParton+ic5calol2l3
    )
ak5calol2l3JRA = cms.Path(
    ak5calol2l3PtEta+ak5genPtEta+ak5calol2l3JetToRef+partons+ak5GenToParton+ak5calol2l3
    )
ak7calol2l3JRA = cms.Path(
    ak7calol2l3PtEta+ak7genPtEta+ak7calol2l3JetToRef+partons+ak7GenToParton+ak7calol2l3
    )
ca4calol2l3JRA = cms.Path(
    ca4calol2l3PtEta+ca4genPtEta+ca4calol2l3JetToRef+partons+ca4GenToParton+ca4calol2l3
    )
ca6calol2l3JRA = cms.Path(
    ca6calol2l3PtEta+ca6genPtEta+ca6calol2l3JetToRef+partons+ca6GenToParton+ca6calol2l3
    )

# pfl2l3
kt4pfl2l3JRA = cms.Path(
    kt4pfl2l3PtEta+kt4genPtEta+kt4pfl2l3JetToRef+partons+kt4GenToParton+kt4pfl2l3
    )
kt6pfl2l3JRA = cms.Path(
    kt6pfl2l3PtEta+kt6genPtEta+kt6pfl2l3JetToRef+partons+kt6GenToParton+kt6pfl2l3
    )
sc5pfl2l3JRA = cms.Path(
    sc5pfl2l3PtEta+sc5genPtEta+sc5pfl2l3JetToRef+partons+sc5GenToParton+sc5pfl2l3
    )
sc7pfl2l3JRA = cms.Path(
    sc7pfl2l3PtEta+sc7genPtEta+sc7pfl2l3JetToRef+partons+sc7GenToParton+sc7pfl2l3
    )
ic5pfl2l3JRA = cms.Path(
    ic5pfl2l3PtEta+ic5genPtEta+ic5pfl2l3JetToRef+partons+ic5GenToParton+ic5pfl2l3
    )
ak5pfl2l3JRA = cms.Path(
    ak5pfl2l3PtEta+ak5genPtEta+ak5pfl2l3JetToRef+partons+ak5GenToParton+ak5pfl2l3
    )
ak7pfl2l3JRA = cms.Path(
    ak7pfl2l3PtEta+ak7genPtEta+ak7pfl2l3JetToRef+partons+ak7GenToParton+ak7pfl2l3
    )
ca4pfl2l3JRA = cms.Path(
    ca4pfl2l3PtEta+ca4genPtEta+ca4pfl2l3JetToRef+partons+ca4GenToParton+ca4pfl2l3
    )
ca6pfl2l3JRA = cms.Path(
    ca6pfl2l3PtEta+ca6genPtEta+ca6pfl2l3JetToRef+partons+ca6GenToParton+ca6pfl2l3
    )

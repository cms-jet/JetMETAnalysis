import FWCore.ParameterSet.Config as cms

from JetMETAnalysis.JetAnalyzers.JRA_Modules_cff import *


#
# MODULES with flavor of genjet in jet response analysis added
#

# calo
kt4calo.doFlavor          = cms.bool(True)
kt4calo.srcRefToPartonMap = cms.InputTag('kt4GenToParton','rec2gen')
kt4calo.deltaRPartonMax   = cms.double(0.25)
kt5calo.doFlavor          = cms.bool(True)
kt5calo.srcRefToPartonMap = cms.InputTag('kt5GenToParton','rec2gen')
kt5calo.deltaRPartonMax   = cms.double(0.25)
kt6calo.doFlavor          = cms.bool(True)
kt6calo.srcRefToPartonMap = cms.InputTag('kt6GenToParton','rec2gen')
kt6calo.deltaRPartonMax   = cms.double(0.25)
kt7calo.doFlavor          = cms.bool(True)
kt7calo.srcRefToPartonMap = cms.InputTag('kt7GenToParton','rec2gen')
kt7calo.deltaRPartonMax   = cms.double(0.25)
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
ca5calo.doFlavor          = cms.bool(True)
ca5calo.srcRefToPartonMap = cms.InputTag('ca5GenToParton','rec2gen')
ca5calo.deltaRPartonMax   = cms.double(0.25)
ca6calo.doFlavor          = cms.bool(True)
ca6calo.srcRefToPartonMap = cms.InputTag('ca6GenToParton','rec2gen')
ca6calo.deltaRPartonMax   = cms.double(0.25)
ca7calo.doFlavor          = cms.bool(True)
ca7calo.srcRefToPartonMap = cms.InputTag('ca7GenToParton','rec2gen')
ca7calo.deltaRPartonMax   = cms.double(0.25)
gk5calo.doFlavor          = cms.bool(True)
gk5calo.srcRefToPartonMap = cms.InputTag('gk5GenToParton','rec2gen')
gk5calo.deltaRPartonMax   = cms.double(0.25)
gk7calo.doFlavor          = cms.bool(True)
gk7calo.srcRefToPartonMap = cms.InputTag('gk7GenToParton','rec2gen')
gk7calo.deltaRPartonMax   = cms.double(0.25)


# pflow
kt4pf.doFlavor          = cms.bool(True)
kt4pf.srcRefToPartonMap = cms.InputTag('kt4GenToParton','rec2gen')
kt4pf.deltaRPartonMax   = cms.double(0.25)
kt5pf.doFlavor          = cms.bool(True)
kt5pf.srcRefToPartonMap = cms.InputTag('kt5GenToParton','rec2gen')
kt5pf.deltaRPartonMax   = cms.double(0.25)
kt6pf.doFlavor          = cms.bool(True)
kt6pf.srcRefToPartonMap = cms.InputTag('kt6GenToParton','rec2gen')
kt6pf.deltaRPartonMax   = cms.double(0.25)
kt7pf.doFlavor          = cms.bool(True)
kt7pf.srcRefToPartonMap = cms.InputTag('kt7GenToParton','rec2gen')
kt7pf.deltaRPartonMax   = cms.double(0.25)
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
ca5pf.doFlavor          = cms.bool(True)
ca5pf.srcRefToPartonMap = cms.InputTag('ca5GenToParton','rec2gen')
ca5pf.deltaRPartonMax   = cms.double(0.25)
ca6pf.doFlavor          = cms.bool(True)
ca6pf.srcRefToPartonMap = cms.InputTag('ca6GenToParton','rec2gen')
ca6pf.deltaRPartonMax   = cms.double(0.25)
ca7pf.doFlavor          = cms.bool(True)
ca7pf.srcRefToPartonMap = cms.InputTag('ca7GenToParton','rec2gen')
ca7pf.deltaRPartonMax   = cms.double(0.25)
gk5pf.doFlavor          = cms.bool(True)
gk5pf.srcRefToPartonMap = cms.InputTag('gk5GenToParton','rec2gen')
gk5pf.deltaRPartonMax   = cms.double(0.25)
gk7pf.doFlavor          = cms.bool(True)
gk7pf.srcRefToPartonMap = cms.InputTag('gk7GenToParton','rec2gen')
gk7pf.deltaRPartonMax   = cms.double(0.25)


# track
kt4trk.doFlavor          = cms.bool(True)
kt4trk.srcRefToPartonMap = cms.InputTag('kt4GenToParton','rec2gen')
kt4trk.deltaRPartonMax   = cms.double(0.25)
kt5trk.doFlavor          = cms.bool(True)
kt5trk.srcRefToPartonMap = cms.InputTag('kt5GenToParton','rec2gen')
kt5trk.deltaRPartonMax   = cms.double(0.25)
kt6trk.doFlavor          = cms.bool(True)
kt6trk.srcRefToPartonMap = cms.InputTag('kt6GenToParton','rec2gen')
kt6trk.deltaRPartonMax   = cms.double(0.25)
kt7trk.doFlavor          = cms.bool(True)
kt7trk.srcRefToPartonMap = cms.InputTag('kt7GenToParton','rec2gen')
kt7trk.deltaRPartonMax   = cms.double(0.25)
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
ca5trk.doFlavor          = cms.bool(True)
ca5trk.srcRefToPartonMap = cms.InputTag('ca5GenToParton','rec2gen')
ca5trk.deltaRPartonMax   = cms.double(0.25)
ca6trk.doFlavor          = cms.bool(True)
ca6trk.srcRefToPartonMap = cms.InputTag('ca6GenToParton','rec2gen')
ca6trk.deltaRPartonMax   = cms.double(0.25)
ca7trk.doFlavor          = cms.bool(True)
ca7trk.srcRefToPartonMap = cms.InputTag('ca7GenToParton','rec2gen')
ca7trk.deltaRPartonMax   = cms.double(0.25)
gk5trk.doFlavor          = cms.bool(True)
gk5trk.srcRefToPartonMap = cms.InputTag('gk5GenToParton','rec2gen')
gk5trk.deltaRPartonMax   = cms.double(0.25)
gk7trk.doFlavor          = cms.bool(True)
gk7trk.srcRefToPartonMap = cms.InputTag('gk7GenToParton','rec2gen')
gk7trk.deltaRPartonMax   = cms.double(0.25)


# jpt
ic5jpt.doFlavor          = cms.bool(True)
ic5jpt.srcRefToPartonMap = cms.InputTag('ic5GenToParton','rec2gen')
ic5jpt.deltaRPartonMax   = cms.double(0.25)


# calol2l3
kt4calol2l3.doFlavor          = cms.bool(True)
kt4calol2l3.srcRefToPartonMap = cms.InputTag('kt4GenToParton','rec2gen')
kt4calol2l3.deltaRPartonMax   = cms.double(0.25)
kt5calol2l3.doFlavor          = cms.bool(True)
kt5calol2l3.srcRefToPartonMap = cms.InputTag('kt5GenToParton','rec2gen')
kt5calol2l3.deltaRPartonMax   = cms.double(0.25)
kt6calol2l3.doFlavor          = cms.bool(True)
kt6calol2l3.srcRefToPartonMap = cms.InputTag('kt6GenToParton','rec2gen')
kt6calol2l3.deltaRPartonMax   = cms.double(0.25)
kt7calol2l3.doFlavor          = cms.bool(True)
kt7calol2l3.srcRefToPartonMap = cms.InputTag('kt7GenToParton','rec2gen')
kt7calol2l3.deltaRPartonMax   = cms.double(0.25)
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
ca5calol2l3.doFlavor          = cms.bool(True)
ca5calol2l3.srcRefToPartonMap = cms.InputTag('ca5GenToParton','rec2gen')
ca5calol2l3.deltaRPartonMax   = cms.double(0.25)
ca6calol2l3.doFlavor          = cms.bool(True)
ca6calol2l3.srcRefToPartonMap = cms.InputTag('ca6GenToParton','rec2gen')
ca6calol2l3.deltaRPartonMax   = cms.double(0.25)
ca7calol2l3.doFlavor          = cms.bool(True)
ca7calol2l3.srcRefToPartonMap = cms.InputTag('ca7GenToParton','rec2gen')
ca7calol2l3.deltaRPartonMax   = cms.double(0.25)
gk5calol2l3.doFlavor          = cms.bool(True)
gk5calol2l3.srcRefToPartonMap = cms.InputTag('gk5GenToParton','rec2gen')
gk5calol2l3.deltaRPartonMax   = cms.double(0.25)
gk7calol2l3.doFlavor          = cms.bool(True)
gk7calol2l3.srcRefToPartonMap = cms.InputTag('gk7GenToParton','rec2gen')
gk7calol2l3.deltaRPartonMax   = cms.double(0.25)


# pfl2l3
kt4pfl2l3.doFlavor          = cms.bool(True)
kt4pfl2l3.srcRefToPartonMap = cms.InputTag('kt4GenToParton','rec2gen')
kt4pfl2l3.deltaRPartonMax   = cms.double(0.25)
kt5pfl2l3.doFlavor          = cms.bool(True)
kt5pfl2l3.srcRefToPartonMap = cms.InputTag('kt5GenToParton','rec2gen')
kt5pfl2l3.deltaRPartonMax   = cms.double(0.25)
kt6pfl2l3.doFlavor          = cms.bool(True)
kt6pfl2l3.srcRefToPartonMap = cms.InputTag('kt6GenToParton','rec2gen')
kt6pfl2l3.deltaRPartonMax   = cms.double(0.25)
kt7pfl2l3.doFlavor          = cms.bool(True)
kt7pfl2l3.srcRefToPartonMap = cms.InputTag('kt7GenToParton','rec2gen')
kt7pfl2l3.deltaRPartonMax   = cms.double(0.25)
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
ca5pfl2l3.doFlavor          = cms.bool(True)
ca5pfl2l3.srcRefToPartonMap = cms.InputTag('ca5GenToParton','rec2gen')
ca5pfl2l3.deltaRPartonMax   = cms.double(0.25)
ca6pfl2l3.doFlavor          = cms.bool(True)
ca6pfl2l3.srcRefToPartonMap = cms.InputTag('ca6GenToParton','rec2gen')
ca6pfl2l3.deltaRPartonMax   = cms.double(0.25)
ca7pfl2l3.doFlavor          = cms.bool(True)
ca7pfl2l3.srcRefToPartonMap = cms.InputTag('ca7GenToParton','rec2gen')
ca7pfl2l3.deltaRPartonMax   = cms.double(0.25)
gk5pfl2l3.doFlavor          = cms.bool(True)
gk5pfl2l3.srcRefToPartonMap = cms.InputTag('gk5GenToParton','rec2gen')
gk5pfl2l3.deltaRPartonMax   = cms.double(0.25)
gk7pfl2l3.doFlavor          = cms.bool(True)
gk7pfl2l3.srcRefToPartonMap = cms.InputTag('gk7GenToParton','rec2gen')
gk7pfl2l3.deltaRPartonMax   = cms.double(0.25)


#
# PATHS with flavor
#

# calo
kt4caloJRA = cms.Sequence(kt4caloPtEta+kt4genPtEta+kt4caloJetToRef+partons+kt4GenToParton+kt4calo)
kt5caloJRA = cms.Sequence(kt5caloPtEta+kt5genPtEta+kt5caloJetToRef+partons+kt5GenToParton+kt5calo)
kt6caloJRA = cms.Sequence(kt6caloPtEta+kt6genPtEta+kt6caloJetToRef+partons+kt6GenToParton+kt6calo)
kt7caloJRA = cms.Sequence(kt7caloPtEta+kt7genPtEta+kt7caloJetToRef+partons+kt7GenToParton+kt7calo)
sc5caloJRA = cms.Sequence(sc5caloPtEta+sc5genPtEta+sc5caloJetToRef+partons+sc5GenToParton+sc5calo)
sc7caloJRA = cms.Sequence(sc7caloPtEta+sc7genPtEta+sc7caloJetToRef+partons+sc7GenToParton+sc7calo)
ic5caloJRA = cms.Sequence(ic5caloPtEta+ic5genPtEta+ic5caloJetToRef+partons+ic5GenToParton+ic5calo)
ak5caloJRA = cms.Sequence(ak5caloPtEta+ak5genPtEta+ak5caloJetToRef+partons+ak5GenToParton+ak5calo)
ak7caloJRA = cms.Sequence(ak7caloPtEta+ak7genPtEta+ak7caloJetToRef+partons+ak7GenToParton+ak7calo)
ca4caloJRA = cms.Sequence(ca4caloPtEta+ca4genPtEta+ca4caloJetToRef+partons+ca4GenToParton+ca4calo)
ca5caloJRA = cms.Sequence(ca5caloPtEta+ca5genPtEta+ca5caloJetToRef+partons+ca5GenToParton+ca5calo)
ca6caloJRA = cms.Sequence(ca6caloPtEta+ca6genPtEta+ca6caloJetToRef+partons+ca6GenToParton+ca6calo)
ca7caloJRA = cms.Sequence(ca7caloPtEta+ca7genPtEta+ca7caloJetToRef+partons+ca7GenToParton+ca7calo)
gk5caloJRA = cms.Sequence(gk5caloPtEta+gk5genPtEta+gk5caloJetToRef+partons+gk5GenToParton+gk5calo)
gk7caloJRA = cms.Sequence(gk7caloPtEta+gk7genPtEta+gk7caloJetToRef+partons+gk7GenToParton+gk7calo)

jraStandardCaloJets=cms.Path(ak5caloJRA+
                               kt4caloJRA+
                               kt6caloJRA+
                               sc5caloJRA+
                               sc7caloJRA+
                               kt4caloJRA+
                               kt6caloJRA)

jraExtraCaloJets=cms.Path(ak7caloJRA+
                          kt5caloJRA+
                          kt7caloJRA+
                          ca4caloJRA+
                          ca5caloJRA+
                          ca6caloJRA+
                          ca7caloJRA)


# pf
kt4pfJRA = cms.Sequence(kt4pfPtEta+kt4genPtEta+kt4pfJetToRef+partons+kt4GenToParton+kt4pf)
kt5pfJRA = cms.Sequence(kt5pfPtEta+kt5genPtEta+kt5pfJetToRef+partons+kt5GenToParton+kt5pf)
kt6pfJRA = cms.Sequence(kt6pfPtEta+kt6genPtEta+kt6pfJetToRef+partons+kt6GenToParton+kt6pf)
kt7pfJRA = cms.Sequence(kt7pfPtEta+kt7genPtEta+kt7pfJetToRef+partons+kt7GenToParton+kt7pf)
sc5pfJRA = cms.Sequence(sc5pfPtEta+sc5genPtEta+sc5pfJetToRef+partons+sc5GenToParton+sc5pf)
sc7pfJRA = cms.Sequence(sc7pfPtEta+sc7genPtEta+sc7pfJetToRef+partons+sc7GenToParton+sc7pf)
ic5pfJRA = cms.Sequence(ic5pfPtEta+ic5genPtEta+ic5pfJetToRef+partons+ic5GenToParton+ic5pf)
ak5pfJRA = cms.Sequence(ak5pfPtEta+ak5genPtEta+ak5pfJetToRef+partons+ak5GenToParton+ak5pf)
ak7pfJRA = cms.Sequence(ak7pfPtEta+ak7genPtEta+ak7pfJetToRef+partons+ak7GenToParton+ak7pf)
ca4pfJRA = cms.Sequence(ca4pfPtEta+ca4genPtEta+ca4pfJetToRef+partons+ca4GenToParton+ca4pf)
ca5pfJRA = cms.Sequence(ca5pfPtEta+ca5genPtEta+ca5pfJetToRef+partons+ca5GenToParton+ca5pf)
ca6pfJRA = cms.Sequence(ca6pfPtEta+ca6genPtEta+ca6pfJetToRef+partons+ca6GenToParton+ca6pf)
ca7pfJRA = cms.Sequence(ca7pfPtEta+ca7genPtEta+ca7pfJetToRef+partons+ca7GenToParton+ca7pf)
gk5pfJRA = cms.Sequence(gk5pfPtEta+gk5genPtEta+gk5pfJetToRef+partons+gk5GenToParton+gk5pf)
gk7pfJRA = cms.Sequence(gk7pfPtEta+gk7genPtEta+gk7pfJetToRef+partons+gk7GenToParton+gk7pf)

jraStandardPFJets=cms.Path(ak5pfJRA+
                           kt4pfJRA+
                           kt6pfJRA+
                           sc5pfJRA+
                           sc7pfJRA+
                           kt4pfJRA+
                           kt6pfJRA)

jraExtraPFJets=cms.Path(ak7pfJRA+
                        kt5pfJRA+
                        kt7pfJRA+
                        ca4pfJRA+
                        ca5pfJRA+
                        ca6pfJRA+
                        ca7pfJRA)


# trk
kt4trkJRA = cms.Sequence(kt4trkPtEta+kt4genPtEta+kt4trkJetToRef+partons+kt4GenToParton+kt4trk)
kt5trkJRA = cms.Sequence(kt5trkPtEta+kt5genPtEta+kt5trkJetToRef+partons+kt5GenToParton+kt5trk)
kt6trkJRA = cms.Sequence(kt6trkPtEta+kt6genPtEta+kt6trkJetToRef+partons+kt6GenToParton+kt6trk)
kt7trkJRA = cms.Sequence(kt7trkPtEta+kt7genPtEta+kt7trkJetToRef+partons+kt7GenToParton+kt7trk)
sc5trkJRA = cms.Sequence(sc5trkPtEta+sc5genPtEta+sc5trkJetToRef+partons+sc5GenToParton+sc5trk)
sc7trkJRA = cms.Sequence(sc7trkPtEta+sc7genPtEta+sc7trkJetToRef+partons+sc7GenToParton+sc7trk)
ic5trkJRA = cms.Sequence(ic5trkPtEta+ic5genPtEta+ic5trkJetToRef+partons+ic5GenToParton+ic5trk)
ak5trkJRA = cms.Sequence(ak5trkPtEta+ak5genPtEta+ak5trkJetToRef+partons+ak5GenToParton+ak5trk)
ak7trkJRA = cms.Sequence(ak7trkPtEta+ak7genPtEta+ak7trkJetToRef+partons+ak7GenToParton+ak7trk)
ca4trkJRA = cms.Sequence(ca4trkPtEta+ca4genPtEta+ca4trkJetToRef+partons+ca4GenToParton+ca4trk)
ca5trkJRA = cms.Sequence(ca5trkPtEta+ca5genPtEta+ca5trkJetToRef+partons+ca5GenToParton+ca5trk)
ca6trkJRA = cms.Sequence(ca6trkPtEta+ca6genPtEta+ca6trkJetToRef+partons+ca6GenToParton+ca6trk)
ca7trkJRA = cms.Sequence(ca7trkPtEta+ca7genPtEta+ca7trkJetToRef+partons+ca7GenToParton+ca7trk)
gk5trkJRA = cms.Sequence(gk5trkPtEta+gk5genPtEta+gk5trkJetToRef+partons+gk5GenToParton+gk5trk)
gk7trkJRA = cms.Sequence(gk7trkPtEta+gk7genPtEta+gk7trkJetToRef+partons+gk7GenToParton+gk7trk)

jraStandardTrackJets=cms.Path(ak5trkJRA+
                              kt4trkJRA+
                              kt6trkJRA+
                              sc5trkJRA+
                              sc7trkJRA+
                              kt4trkJRA+
                              kt6trkJRA)

jraExtraTrackJets=cms.Path(ak7trkJRA+
                           kt5trkJRA+
                           kt7trkJRA+
                           ca4trkJRA+
                           ca5trkJRA+
                           ca6trkJRA+
                           ca7trkJRA)


# jpt
ic5jptJRA = cms.Sequence(ic5jptPtEta+ic5genPtEta+ic5jptJetToRef+partons+ic5GenToParton+ic5jpt)

jraStandardJPTJets=cms.Path(ic5jptJRA)


# calol2l3
kt4calol2l3JRA = cms.Sequence(kt4calol2l3PtEta+kt4genPtEta+kt4calol2l3JetToRef+partons+kt4GenToParton+kt4calol2l3)
kt5calol2l3JRA = cms.Sequence(kt5calol2l3PtEta+kt5genPtEta+kt5calol2l3JetToRef+partons+kt5GenToParton+kt5calol2l3)
kt6calol2l3JRA = cms.Sequence(kt6calol2l3PtEta+kt6genPtEta+kt6calol2l3JetToRef+partons+kt6GenToParton+kt6calol2l3)
kt7calol2l3JRA = cms.Sequence(kt7calol2l3PtEta+kt7genPtEta+kt7calol2l3JetToRef+partons+kt7GenToParton+kt7calol2l3)
sc5calol2l3JRA = cms.Sequence(sc5calol2l3PtEta+sc5genPtEta+sc5calol2l3JetToRef+partons+sc5GenToParton+sc5calol2l3)
sc7calol2l3JRA = cms.Sequence(sc7calol2l3PtEta+sc7genPtEta+sc7calol2l3JetToRef+partons+sc7GenToParton+sc7calol2l3)
ic5calol2l3JRA = cms.Sequence(ic5calol2l3PtEta+ic5genPtEta+ic5calol2l3JetToRef+partons+ic5GenToParton+ic5calol2l3)
ak5calol2l3JRA = cms.Sequence(ak5calol2l3PtEta+ak5genPtEta+ak5calol2l3JetToRef+partons+ak5GenToParton+ak5calol2l3)
ak7calol2l3JRA = cms.Sequence(ak7calol2l3PtEta+ak7genPtEta+ak7calol2l3JetToRef+partons+ak7GenToParton+ak7calol2l3)
ca4calol2l3JRA = cms.Sequence(ca4calol2l3PtEta+ca4genPtEta+ca4calol2l3JetToRef+partons+ca4GenToParton+ca4calol2l3)
ca5calol2l3JRA = cms.Sequence(ca5calol2l3PtEta+ca5genPtEta+ca5calol2l3JetToRef+partons+ca5GenToParton+ca5calol2l3)
ca6calol2l3JRA = cms.Sequence(ca6calol2l3PtEta+ca6genPtEta+ca6calol2l3JetToRef+partons+ca6GenToParton+ca6calol2l3)
ca7calol2l3JRA = cms.Sequence(ca7calol2l3PtEta+ca7genPtEta+ca7calol2l3JetToRef+partons+ca7GenToParton+ca7calol2l3)
gk5calol2l3JRA = cms.Sequence(gk5calol2l3PtEta+gk5genPtEta+gk5calol2l3JetToRef+partons+gk5GenToParton+gk5calol2l3)
gk7calol2l3JRA = cms.Sequence(gk7calol2l3PtEta+gk7genPtEta+gk7calol2l3JetToRef+partons+gk7GenToParton+gk7calol2l3)

jraStandardCaloL2L3Jets=cms.Path(ak5calol2l3JRA+
                                 kt4calol2l3JRA+
                                 kt6calol2l3JRA+
                                 sc5calol2l3JRA+
                                 sc7calol2l3JRA+
                                 kt4calol2l3JRA+
                                 kt6calol2l3JRA)

jraExtraCaloL2L3Jets=cms.Path(ak7calol2l3JRA+
                               kt5calol2l3JRA+
                               kt7calol2l3JRA+
                               ca4calol2l3JRA+
                               ca5calol2l3JRA+
                               ca6calol2l3JRA+
                               ca7calol2l3JRA)


# pfl2l3
kt4pfl2l3JRA = cms.Sequence(kt4pfl2l3PtEta+kt4genPtEta+kt4pfl2l3JetToRef+partons+kt4GenToParton+kt4pfl2l3)
kt5pfl2l3JRA = cms.Sequence(kt5pfl2l3PtEta+kt5genPtEta+kt5pfl2l3JetToRef+partons+kt5GenToParton+kt5pfl2l3)
kt6pfl2l3JRA = cms.Sequence(kt6pfl2l3PtEta+kt6genPtEta+kt6pfl2l3JetToRef+partons+kt6GenToParton+kt6pfl2l3)
kt7pfl2l3JRA = cms.Sequence(kt7pfl2l3PtEta+kt7genPtEta+kt7pfl2l3JetToRef+partons+kt7GenToParton+kt7pfl2l3)
sc5pfl2l3JRA = cms.Sequence(sc5pfl2l3PtEta+sc5genPtEta+sc5pfl2l3JetToRef+partons+sc5GenToParton+sc5pfl2l3)
sc7pfl2l3JRA = cms.Sequence(sc7pfl2l3PtEta+sc7genPtEta+sc7pfl2l3JetToRef+partons+sc7GenToParton+sc7pfl2l3)
ic5pfl2l3JRA = cms.Sequence(ic5pfl2l3PtEta+ic5genPtEta+ic5pfl2l3JetToRef+partons+ic5GenToParton+ic5pfl2l3)
ak5pfl2l3JRA = cms.Sequence(ak5pfl2l3PtEta+ak5genPtEta+ak5pfl2l3JetToRef+partons+ak5GenToParton+ak5pfl2l3)
ak7pfl2l3JRA = cms.Sequence(ak7pfl2l3PtEta+ak7genPtEta+ak7pfl2l3JetToRef+partons+ak7GenToParton+ak7pfl2l3)
ca4pfl2l3JRA = cms.Sequence(ca4pfl2l3PtEta+ca4genPtEta+ca4pfl2l3JetToRef+partons+ca4GenToParton+ca4pfl2l3)
ca5pfl2l3JRA = cms.Sequence(ca5pfl2l3PtEta+ca5genPtEta+ca5pfl2l3JetToRef+partons+ca5GenToParton+ca5pfl2l3)
ca6pfl2l3JRA = cms.Sequence(ca6pfl2l3PtEta+ca6genPtEta+ca6pfl2l3JetToRef+partons+ca6GenToParton+ca6pfl2l3)
ca7pfl2l3JRA = cms.Sequence(ca7pfl2l3PtEta+ca7genPtEta+ca7pfl2l3JetToRef+partons+ca7GenToParton+ca7pfl2l3)
gk5pfl2l3JRA = cms.Sequence(gk5pfl2l3PtEta+gk5genPtEta+gk5pfl2l3JetToRef+partons+gk5GenToParton+gk5pfl2l3)
gk7pfl2l3JRA = cms.Sequence(gk7pfl2l3PtEta+gk7genPtEta+gk7pfl2l3JetToRef+partons+gk7GenToParton+gk7pfl2l3)

jraStandardPFL2L3Jets=cms.Path(ak5pfl2l3JRA+
                               kt4pfl2l3JRA+
                               kt6pfl2l3JRA+
                               sc5pfl2l3JRA+
                               sc7pfl2l3JRA+
                               kt4pfl2l3JRA+
                               kt6pfl2l3JRA)

jraExtraPFL2L3Jets=cms.Path(ak7pfl2l3JRA+
                            kt5pfl2l3JRA+
                            kt7pfl2l3JRA+
                            ca4pfl2l3JRA+
                            ca5pfl2l3JRA+
                            ca6pfl2l3JRA+
                            ca7pfl2l3JRA)

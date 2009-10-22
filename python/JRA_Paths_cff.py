import FWCore.ParameterSet.Config as cms

from JetMETAnalysis.JetAnalyzers.JRA_Modules_cff import *

# calo
kt4caloJRA=cms.Sequence(kt4caloPtEta+kt4genPtEta+kt4caloJetToRef+kt4calo)
kt5caloJRA=cms.Sequence(kt5caloPtEta+kt5genPtEta+kt5caloJetToRef+kt5calo)
kt6caloJRA=cms.Sequence(kt6caloPtEta+kt6genPtEta+kt6caloJetToRef+kt6calo)
kt7caloJRA=cms.Sequence(kt7caloPtEta+kt7genPtEta+kt7caloJetToRef+kt7calo)
sc5caloJRA=cms.Sequence(sc5caloPtEta+sc5genPtEta+sc5caloJetToRef+sc5calo)
sc7caloJRA=cms.Sequence(sc7caloPtEta+sc7genPtEta+sc7caloJetToRef+sc7calo)
ic5caloJRA=cms.Sequence(ic5caloPtEta+ic5genPtEta+ic5caloJetToRef+ic5calo)
ak5caloJRA=cms.Sequence(ak5caloPtEta+ak5genPtEta+ak5caloJetToRef+ak5calo)
ak7caloJRA=cms.Sequence(ak7caloPtEta+ak7genPtEta+ak7caloJetToRef+ak7calo)
ca4caloJRA=cms.Sequence(ca4caloPtEta+ca4genPtEta+ca4caloJetToRef+ca4calo)
ca5caloJRA=cms.Sequence(ca5caloPtEta+ca5genPtEta+ca5caloJetToRef+ca5calo)
ca6caloJRA=cms.Sequence(ca6caloPtEta+ca6genPtEta+ca6caloJetToRef+ca6calo)
ca7caloJRA=cms.Sequence(ca7caloPtEta+ca7genPtEta+ca7caloJetToRef+ca7calo)
gk5caloJRA=cms.Sequence(gk5caloPtEta+gk5genPtEta+gk5caloJetToRef+gk5calo)
gk7caloJRA=cms.Sequence(gk7caloPtEta+gk7genPtEta+gk7caloJetToRef+gk7calo)

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
kt4pfJRA=cms.Sequence(kt4pfPtEta+kt4genPtEta+kt4pfJetToRef+kt4pf)
kt5pfJRA=cms.Sequence(kt5pfPtEta+kt5genPtEta+kt5pfJetToRef+kt5pf)
kt6pfJRA=cms.Sequence(kt6pfPtEta+kt6genPtEta+kt6pfJetToRef+kt6pf)
kt7pfJRA=cms.Sequence(kt7pfPtEta+kt7genPtEta+kt7pfJetToRef+kt7pf)
sc5pfJRA=cms.Sequence(sc5pfPtEta+sc5genPtEta+sc5pfJetToRef+sc5pf)
sc7pfJRA=cms.Sequence(sc7pfPtEta+sc7genPtEta+sc7pfJetToRef+sc7pf)
ic5pfJRA=cms.Sequence(ic5pfPtEta+ic5genPtEta+ic5pfJetToRef+ic5pf)
ak5pfJRA=cms.Sequence(ak5pfPtEta+ak5genPtEta+ak5pfJetToRef+ak5pf)
ak7pfJRA=cms.Sequence(ak7pfPtEta+ak7genPtEta+ak7pfJetToRef+ak7pf)
ca4pfJRA=cms.Sequence(ca4pfPtEta+ca4genPtEta+ca4pfJetToRef+ca4pf)
ca5pfJRA=cms.Sequence(ca5pfPtEta+ca5genPtEta+ca5pfJetToRef+ca5pf)
ca6pfJRA=cms.Sequence(ca6pfPtEta+ca6genPtEta+ca6pfJetToRef+ca6pf)
ca7pfJRA=cms.Sequence(ca7pfPtEta+ca7genPtEta+ca7pfJetToRef+ca7pf)
gk5pfJRA=cms.Sequence(gk5pfPtEta+gk5genPtEta+gk5pfJetToRef+gk5pf)
gk7pfJRA=cms.Sequence(gk7pfPtEta+gk7genPtEta+gk7pfJetToRef+gk7pf)

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
kt4trkJRA =cms.Sequence(kt4trkPtEta+kt4genPtEta+kt4trkJetToRef+kt4trk)
kt5trkJRA =cms.Sequence(kt5trkPtEta+kt5genPtEta+kt5trkJetToRef+kt5trk)
kt6trkJRA =cms.Sequence(kt6trkPtEta+kt6genPtEta+kt6trkJetToRef+kt6trk)
kt7trkJRA =cms.Sequence(kt7trkPtEta+kt7genPtEta+kt7trkJetToRef+kt7trk)
sc5trkJRA =cms.Sequence(sc5trkPtEta+sc5genPtEta+sc5trkJetToRef+sc5trk)
sc7trkJRA =cms.Sequence(sc7trkPtEta+sc7genPtEta+sc7trkJetToRef+sc7trk)
ic5trkJRA =cms.Sequence(ic5trkPtEta+ic5genPtEta+ic5trkJetToRef+ic5trk)
ak5trkJRA =cms.Sequence(ak5trkPtEta+ak5genPtEta+ak5trkJetToRef+ak5trk)
ak7trkJRA =cms.Sequence(ak7trkPtEta+ak7genPtEta+ak7trkJetToRef+ak7trk)
ca4trkJRA =cms.Sequence(ca4trkPtEta+ca4genPtEta+ca4trkJetToRef+ca4trk)
ca5trkJRA =cms.Sequence(ca5trkPtEta+ca5genPtEta+ca5trkJetToRef+ca5trk)
ca6trkJRA =cms.Sequence(ca6trkPtEta+ca6genPtEta+ca6trkJetToRef+ca6trk)
ca7trkJRA =cms.Sequence(ca7trkPtEta+ca7genPtEta+ca7trkJetToRef+ca7trk)
gk5trkJRA =cms.Sequence(gk5trkPtEta+gk5genPtEta+gk5trkJetToRef+gk5trk)
gk7trkJRA =cms.Sequence(gk7trkPtEta+gk7genPtEta+gk7trkJetToRef+gk7trk)

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
ic5jptJRA =cms.Sequence(ic5jptPtEta+ic5genPtEta+ic5jptJetToRef+ic5jpt)
sc5jptJRA =cms.Sequence(sc5jptPtEta+sc5genPtEta+sc5jptJetToRef+sc5jpt)
ak5jptJRA =cms.Sequence(ak5jptPtEta+ak5genPtEta+ak5jptJetToRef+ak5jpt)

jraStandardJPTJets=cms.Path(ic5jptJRA+
                            sc5jptJRA+
                            ak5jptJRA)


# calol2l3
kt4calol2l3JRA=cms.Sequence(kt4calol2l3PtEta+kt4genPtEta+kt4calol2l3JetToRef+kt4calol2l3)
kt5calol2l3JRA=cms.Sequence(kt5calol2l3PtEta+kt5genPtEta+kt5calol2l3JetToRef+kt5calol2l3)
kt6calol2l3JRA=cms.Sequence(kt6calol2l3PtEta+kt6genPtEta+kt6calol2l3JetToRef+kt6calol2l3)
kt7calol2l3JRA=cms.Sequence(kt7calol2l3PtEta+kt7genPtEta+kt7calol2l3JetToRef+kt7calol2l3)
sc5calol2l3JRA=cms.Sequence(sc5calol2l3PtEta+sc5genPtEta+sc5calol2l3JetToRef+sc5calol2l3)
sc7calol2l3JRA=cms.Sequence(sc7calol2l3PtEta+sc7genPtEta+sc7calol2l3JetToRef+sc7calol2l3)
ic5calol2l3JRA=cms.Sequence(ic5calol2l3PtEta+ic5genPtEta+ic5calol2l3JetToRef+ic5calol2l3)
ak5calol2l3JRA=cms.Sequence(ak5calol2l3PtEta+ak5genPtEta+ak5calol2l3JetToRef+ak5calol2l3)
ak7calol2l3JRA=cms.Sequence(ak7calol2l3PtEta+ak7genPtEta+ak7calol2l3JetToRef+ak7calol2l3)
ca4calol2l3JRA=cms.Sequence(ca4calol2l3PtEta+ca4genPtEta+ca4calol2l3JetToRef+ca4calol2l3)
ca5calol2l3JRA=cms.Sequence(ca5calol2l3PtEta+ca5genPtEta+ca5calol2l3JetToRef+ca5calol2l3)
ca6calol2l3JRA=cms.Sequence(ca6calol2l3PtEta+ca6genPtEta+ca6calol2l3JetToRef+ca6calol2l3)
ca7calol2l3JRA=cms.Sequence(ca7calol2l3PtEta+ca7genPtEta+ca7calol2l3JetToRef+ca7calol2l3)
gk5calol2l3JRA=cms.Sequence(gk5calol2l3PtEta+gk5genPtEta+gk5calol2l3JetToRef+gk5calol2l3)
gk7calol2l3JRA=cms.Sequence(gk7calol2l3PtEta+gk7genPtEta+gk7calol2l3JetToRef+gk7calol2l3)

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
kt4pfl2l3JRA=cms.Sequence(kt4pfl2l3PtEta+kt4genPtEta+kt4pfl2l3JetToRef+kt4pfl2l3)
kt5pfl2l3JRA=cms.Sequence(kt5pfl2l3PtEta+kt5genPtEta+kt5pfl2l3JetToRef+kt5pfl2l3)
kt6pfl2l3JRA=cms.Sequence(kt6pfl2l3PtEta+kt6genPtEta+kt6pfl2l3JetToRef+kt6pfl2l3)
kt7pfl2l3JRA=cms.Sequence(kt7pfl2l3PtEta+kt7genPtEta+kt7pfl2l3JetToRef+kt7pfl2l3)
sc5pfl2l3JRA=cms.Sequence(sc5pfl2l3PtEta+sc5genPtEta+sc5pfl2l3JetToRef+sc5pfl2l3)
sc7pfl2l3JRA=cms.Sequence(sc7pfl2l3PtEta+sc7genPtEta+sc7pfl2l3JetToRef+sc7pfl2l3)
ic5pfl2l3JRA=cms.Sequence(ic5pfl2l3PtEta+ic5genPtEta+ic5pfl2l3JetToRef+ic5pfl2l3)
ak5pfl2l3JRA=cms.Sequence(ak5pfl2l3PtEta+ak5genPtEta+ak5pfl2l3JetToRef+ak5pfl2l3)
ak7pfl2l3JRA=cms.Sequence(ak7pfl2l3PtEta+ak7genPtEta+ak7pfl2l3JetToRef+ak7pfl2l3)
ca4pfl2l3JRA=cms.Sequence(ca4pfl2l3PtEta+ca4genPtEta+ca4pfl2l3JetToRef+ca4pfl2l3)
ca5pfl2l3JRA=cms.Sequence(ca5pfl2l3PtEta+ca5genPtEta+ca5pfl2l3JetToRef+ca5pfl2l3)
ca6pfl2l3JRA=cms.Sequence(ca6pfl2l3PtEta+ca6genPtEta+ca6pfl2l3JetToRef+ca6pfl2l3)
ca7pfl2l3JRA=cms.Sequence(ca7pfl2l3PtEta+ca7genPtEta+ca7pfl2l3JetToRef+ca7pfl2l3)
gk5pfl2l3JRA=cms.Sequence(gk5pfl2l3PtEta+gk5genPtEta+gk5pfl2l3JetToRef+gk5pfl2l3)
gk7pfl2l3JRA=cms.Sequence(gk7pfl2l3PtEta+gk7genPtEta+gk7pfl2l3JetToRef+gk7pfl2l3)

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

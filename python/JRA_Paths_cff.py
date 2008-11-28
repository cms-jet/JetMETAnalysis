import FWCore.ParameterSet.Config as cms

from JetMETAnalysis.JetAnalyzers.JRA_Modules_cff import *

# calo
kt4caloJRA = cms.Path(kt4caloPtEta + kt4genPtEta + kt4caloJetToRef + kt4calo)
kt6caloJRA = cms.Path(kt6caloPtEta + kt6genPtEta + kt6caloJetToRef + kt6calo)
sc5caloJRA = cms.Path(sc5caloPtEta + sc5genPtEta + sc5caloJetToRef + sc5calo)
sc7caloJRA = cms.Path(sc7caloPtEta + sc7genPtEta + sc7caloJetToRef + sc7calo)
ic5caloJRA = cms.Path(ic5caloPtEta + ic5genPtEta + ic5caloJetToRef + ic5calo)
ak5caloJRA = cms.Path(ak5caloPtEta + ak5genPtEta + ak5caloJetToRef + ak5calo)
ak7caloJRA = cms.Path(ak7caloPtEta + ak7genPtEta + ak7caloJetToRef + ak7calo)
ca4caloJRA = cms.Path(ca4caloPtEta + ca4genPtEta + ca4caloJetToRef + ca4calo)
ca6caloJRA = cms.Path(ca6caloPtEta + ca6genPtEta + ca6caloJetToRef + ca6calo)

# pf
kt4pfJRA   = cms.Path(kt4pfPtEta   + kt4genPtEta + kt4pfJetToRef   + kt4pf)
kt6pfJRA   = cms.Path(kt6pfPtEta   + kt6genPtEta + kt6pfJetToRef   + kt6pf)
sc5pfJRA   = cms.Path(sc5pfPtEta   + sc5genPtEta + sc5pfJetToRef   + sc5pf)
sc7pfJRA   = cms.Path(sc7pfPtEta   + sc7genPtEta + sc7pfJetToRef   + sc7pf)
ic5pfJRA   = cms.Path(ic5pfPtEta   + ic5genPtEta + ic5pfJetToRef   + ic5pf)
ak5pfJRA   = cms.Path(ak5pfPtEta   + ak5genPtEta + ak5pfJetToRef   + ak5pf)
ak7pfJRA   = cms.Path(ak7pfPtEta   + ak7genPtEta + ak7pfJetToRef   + ak7pf)
ca4pfJRA   = cms.Path(ca4pfPtEta   + ca4genPtEta + ca4pfJetToRef   + ca4pf)
ca6pfJRA   = cms.Path(ca6pfPtEta   + ca6genPtEta + ca6pfJetToRef   + ca6pf)

# trk
kt4trkJRA  = cms.Path(kt4trkPtEta  + kt4genPtEta + kt4trkJetToRef  + kt4trk)
kt6trkJRA  = cms.Path(kt6trkPtEta  + kt6genPtEta + kt6trkJetToRef  + kt6trk)
sc5trkJRA  = cms.Path(sc5trkPtEta  + sc5genPtEta + sc5trkJetToRef  + sc5trk)
sc7trkJRA  = cms.Path(sc7trkPtEta  + sc7genPtEta + sc7trkJetToRef  + sc7trk)
ic5trkJRA  = cms.Path(ic5trkPtEta  + ic5genPtEta + ic5trkJetToRef  + ic5trk)
ak5trkJRA  = cms.Path(ak5trkPtEta  + ak5genPtEta + ak5trkJetToRef  + ak5trk)
ak7trkJRA  = cms.Path(ak7trkPtEta  + ak7genPtEta + ak7trkJetToRef  + ak7trk)
ca4trkJRA  = cms.Path(ca4trkPtEta  + ca4genPtEta + ca4trkJetToRef  + ca4trk)
ca6trkJRA  = cms.Path(ca6trkPtEta  + ca6genPtEta + ca6trkJetToRef  + ca6trk)

# jpt
ic5jptJRA  = cms.Path(ic5jptPtEta  + ic5genPtEta + ic5jptJetToRef  + ic5jpt)

# calol2l3
kt4calol2l3JRA=cms.Path(kt4calol2l3PtEta+kt4genPtEta+kt4calol2l3JetToRef+kt4calol2l3)
kt6calol2l3JRA=cms.Path(kt6calol2l3PtEta+kt6genPtEta+kt6calol2l3JetToRef+kt6calol2l3)
sc5calol2l3JRA=cms.Path(sc5calol2l3PtEta+sc5genPtEta+sc5calol2l3JetToRef+sc5calol2l3)
sc7calol2l3JRA=cms.Path(sc7calol2l3PtEta+sc7genPtEta+sc7calol2l3JetToRef+sc7calol2l3)
ic5calol2l3JRA=cms.Path(ic5calol2l3PtEta+ic5genPtEta+ic5calol2l3JetToRef+ic5calol2l3)
ak5calol2l3JRA=cms.Path(ak5calol2l3PtEta+ak5genPtEta+ak5calol2l3JetToRef+ak5calol2l3)
ak7calol2l3JRA=cms.Path(ak7calol2l3PtEta+ak7genPtEta+ak7calol2l3JetToRef+ak7calol2l3)
ca4calol2l3JRA=cms.Path(ca4calol2l3PtEta+ca4genPtEta+ca4calol2l3JetToRef+ca4calol2l3)
ca6calol2l3JRA=cms.Path(ca6calol2l3PtEta+ca6genPtEta+ca6calol2l3JetToRef+ca6calol2l3)

# pfl2l3
kt4pfl2l3JRA  =cms.Path(kt4pfl2l3PtEta  +kt4genPtEta+kt4pfl2l3JetToRef  +kt4pfl2l3)
kt6pfl2l3JRA  =cms.Path(kt6pfl2l3PtEta  +kt6genPtEta+kt6pfl2l3JetToRef  +kt6pfl2l3)
sc5pfl2l3JRA  =cms.Path(sc5pfl2l3PtEta  +sc5genPtEta+sc5pfl2l3JetToRef  +sc5pfl2l3)
sc7pfl2l3JRA  =cms.Path(sc7pfl2l3PtEta  +sc7genPtEta+sc7pfl2l3JetToRef  +sc7pfl2l3)
ic5pfl2l3JRA  =cms.Path(ic5pfl2l3PtEta  +ic5genPtEta+ic5pfl2l3JetToRef  +ic5pfl2l3)
ak5pfl2l3JRA  =cms.Path(ak5pfl2l3PtEta  +ak5genPtEta+ak5pfl2l3JetToRef  +ak5pfl2l3)
ak7pfl2l3JRA  =cms.Path(ak7pfl2l3PtEta  +ak7genPtEta+ak7pfl2l3JetToRef  +ak7pfl2l3)
ca4pfl2l3JRA  =cms.Path(ca4pfl2l3PtEta  +ca4genPtEta+ca4pfl2l3JetToRef  +ca4pfl2l3)
ca6pfl2l3JRA  =cms.Path(ca6pfl2l3PtEta  +ca6genPtEta+ca6pfl2l3JetToRef  +ca6pfl2l3)

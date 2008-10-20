import FWCore.ParameterSet.Config as cms

import JetMETAnalysis.JetAnalyzers.JRA_Defaults_cff as Defaults;


#!
#! MODULES
#!

# genjets
kt4genPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.RefPtEta,
    src = cms.InputTag("kt4GenJets")
)

kt6genPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.RefPtEta,
    src = cms.InputTag("kt6GenJets")
)

sc5genPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.RefPtEta,
    src = cms.InputTag("sisCone5GenJets")
)

sc7genPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.RefPtEta,
    src = cms.InputTag("sisCone7GenJets")
)

ic5genPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.RefPtEta,
    src = cms.InputTag("iterativeCone5GenJets")
)

# kt4calo
kt4caloPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("kt4CaloJets")
)

kt4caloJetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("kt4genPtEta"),
    srcRec = cms.InputTag("kt4caloPtEta")
)

kt4calo = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("kt4caloJetToRef","gen2rec"),
    srcRef = cms.InputTag("kt4genPtEta")
)

# kt4calol2l3
kt4calol2l3PtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("L2L3CorJetKt4")
)

kt4calol2l3JetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("kt4genPtEta"),
    srcRec = cms.InputTag("kt4calol2l3PtEta")
)

kt4calol2l3 = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("kt4calol2l3JetToRef","gen2rec"),
    srcRef = cms.InputTag("kt4genPtEta")
)

#kt6calo
kt6caloPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("kt6CaloJets")
)

kt6caloJetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("kt6genPtEta"),
    srcRec = cms.InputTag("kt6caloPtEta")
)

kt6calo = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("kt6caloJetToRef","gen2rec"),
    srcRef = cms.InputTag("kt6genPtEta")
)

#kt6calol2l3
kt6calol2l3PtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("L2L3CorJetKt6")
)

kt6calol2l3JetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("kt6genPtEta"),
    srcRec = cms.InputTag("kt6calol2l3PtEta")
)

kt6calol2l3 = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("kt6calol2l3JetToRef","gen2rec"),
    srcRef = cms.InputTag("kt6genPtEta")
)

# sc5calo
sc5caloPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("sisCone5CaloJets")
)

sc5caloJetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("sc5genPtEta"),
    srcRec = cms.InputTag("sc5caloPtEta")
)

sc5calo = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("sc5caloJetToRef","gen2rec"),
    srcRef = cms.InputTag("sc5genPtEta")
)

# sc5calol2l3
sc5calol2l3PtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("L2L3CorJetScone5")
)

sc5calol2l3JetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("sc5genPtEta"),
    srcRec = cms.InputTag("sc5calol2l3PtEta")
)

sc5calol2l3 = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("sc5calol2l3JetToRef","gen2rec"),
    srcRef = cms.InputTag("sc5genPtEta")
)

# sc7calo
sc7caloPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("sisCone7CaloJets")
)

sc7caloJetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("sc7genPtEta"),
    srcRec = cms.InputTag("sc7caloPtEta")
)

sc7calo = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("sc7caloJetToRef","gen2rec"),
    srcRef = cms.InputTag("sc7genPtEta")
)

# sc7calol2l3
sc7calol2l3PtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("L2L3CorJetScone7")
)

sc7calol2l3JetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("sc7genPtEta"),
    srcRec = cms.InputTag("sc7calol2l3PtEta")
)

sc7calol2l3 = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("sc7calol2l3JetToRef","gen2rec"),
    srcRef = cms.InputTag("sc7genPtEta")
)

# ic5calo
ic5caloPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("iterativeCone5CaloJets")
)

ic5caloJetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("ic5genPtEta"),
    srcRec = cms.InputTag("ic5caloPtEta")
)

ic5calo = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("ic5caloJetToRef","gen2rec"),
    srcRef = cms.InputTag("ic5genPtEta")
)

# ic5calol2l3
ic5calol2l3PtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("L2L3CorJetIcone5")
)

ic5calol2l3JetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("ic5genPtEta"),
    srcRec = cms.InputTag("ic5calol2l3PtEta")
)

ic5calol2l3 = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("ic5calol2l3JetToRef","gen2rec"),
    srcRef = cms.InputTag("ic5genPtEta")
)

# kt4pflow
kt4pfPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("kt4PFJets")
)

kt4pfJetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("kt4genPtEta"),
    srcRec = cms.InputTag("kt4pfPtEta")
)

kt4pf = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("kt4pfJetToRef","gen2rec"),
    srcRef = cms.InputTag("kt4genPtEta")
)

# kt6pflow
kt6pfPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("kt6PFJets")
)

kt6pfJetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("kt6genPtEta"),
    srcRec = cms.InputTag("kt6pfPtEta")
)

kt6pf = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("kt6pfJetToRef","gen2rec"),
    srcRef = cms.InputTag("kt6genPtEta")
)

# sc5pflow
sc5pfPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("sisCone5PFJets")
)

sc5pfJetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("sc5genPtEta"),
    srcRec = cms.InputTag("sc5pfPtEta")
)

sc5pf = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("sc5pfJetToRef","gen2rec"),
    srcRef = cms.InputTag("sc5genPtEta")
)

#sc7pflow
sc7pfPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("sisCone7PFJets")
)

sc7pfJetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("sc7genPtEta"),
    srcRec = cms.InputTag("sc7pfPtEta")
)

sc7pf = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("sc7pfJetToRef","gen2rec"),
    srcRef = cms.InputTag("sc7genPtEta")
)

#ic5pflow
ic5pfPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("iterativeCone5PFJets")
)

ic5pfJetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("ic5genPtEta"),
    srcRec = cms.InputTag("ic5pfPtEta")
)

ic5pf = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("ic5pfJetToRef","gen2rec"),
    srcRef = cms.InputTag("ic5genPtEta")
)

#ic5pflowl2l3
ic5pfl2l3PtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("L2L3CorJetPFIcone5")
)

ic5pfl2l3JetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("ic5genPtEta"),
    srcRec = cms.InputTag("ic5pfl2l3PtEta")
)

ic5pfl2l3 = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("ic5pfl2l3JetToRef","gen2rec"),
    srcRef = cms.InputTag("ic5genPtEta")
)

# kt4trk
kt4trkPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("kt4TrackJets")
)

kt4trkJetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("kt4genPtEta"),
    srcRec = cms.InputTag("kt4trkPtEta")
)

kt4trk = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("kt4trkJetToRef","gen2rec"),
    srcRef = cms.InputTag("kt4genPtEta")
)

# kt6trk
kt6trkPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("kt6TrackJets")
)

kt6trkJetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("kt6genPtEta"),
    srcRec = cms.InputTag("kt6trkPtEta")
)

kt6trk = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("kt6trkJetToRef","gen2rec"),
    srcRef = cms.InputTag("kt6genPtEta")
)

#sc5trk
sc5trkPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("sisCone5TrackJets")
)

sc5trkJetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("sc5genPtEta"),
    srcRec = cms.InputTag("sc5trkPtEta")
)

sc5trk = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("sc5trkJetToRef","gen2rec"),
    srcRef = cms.InputTag("sc5genPtEta")
)

# sc7trk
sc7trkPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("sisCone7TrackJets")
)

sc7trkJetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("sc7genPtEta"),
    srcRec = cms.InputTag("sc7trkPtEta")
)

sc7trk = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("sc7trkJetToRef","gen2rec"),
    srcRef = cms.InputTag("sc7genPtEta")
)

# ic5trk
ic5trkPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("iterativeCone5TrackJets")
)

ic5trkJetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("ic5genPtEta"),
    srcRec = cms.InputTag("ic5trkPtEta")
)

ic5trk = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("ic5trkJetToRef","gen2rec"),
    srcRef = cms.InputTag("ic5genPtEta")
)

# ic5jpt
ic5jptPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("JetPlusTrackZSPCorJetIcone5")
)

ic5jptJetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("ic5genPtEta"),
    srcRec = cms.InputTag("ic5jptPtEta")
)

ic5jpt = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("ic5jptJetToRef","gen2rec"),
    srcRef = cms.InputTag("ic5genPtEta")
)


#!
#! PATHS
#!

# calo
kt4caloJRA = cms.Path(kt4caloPtEta + kt4genPtEta + kt4caloJetToRef + kt4calo)
kt6caloJRA = cms.Path(kt6caloPtEta + kt6genPtEta + kt6caloJetToRef + kt6calo)
sc5caloJRA = cms.Path(sc5caloPtEta + sc5genPtEta + sc5caloJetToRef + sc5calo)
sc7caloJRA = cms.Path(sc7caloPtEta + sc7genPtEta + sc7caloJetToRef + sc7calo)
ic5caloJRA = cms.Path(ic5caloPtEta + ic5genPtEta + ic5caloJetToRef + ic5calo)
# pf
kt4pfJRA   = cms.Path(kt4pfPtEta   + kt4genPtEta + kt4pfJetToRef   + kt4pf)
kt6pfJRA   = cms.Path(kt6pfPtEta   + kt6genPtEta + kt6pfJetToRef   + kt6pf)
sc5pfJRA   = cms.Path(sc5pfPtEta   + sc5genPtEta + sc5pfJetToRef   + sc5pf)
sc7pfJRA   = cms.Path(sc7pfPtEta   + sc7genPtEta + sc7pfJetToRef   + sc7pf)
ic5pfJRA   = cms.Path(ic5pfPtEta   + ic5genPtEta + ic5pfJetToRef   + ic5pf)
# trk
kt4trkJRA  = cms.Path(kt4trkPtEta  + kt4genPtEta + kt4trkJetToRef  + kt4trk)
kt6trkJRA  = cms.Path(kt6trkPtEta  + kt6genPtEta + kt6trkJetToRef  + kt6trk)
sc5trkJRA  = cms.Path(sc5trkPtEta  + sc5genPtEta + sc5trkJetToRef  + sc5trk)
sc7trkJRA  = cms.Path(sc7trkPtEta  + sc7genPtEta + sc7trkJetToRef  + sc7trk)
ic5trkJRA  = cms.Path(ic5trkPtEta  + ic5genPtEta + ic5trkJetToRef  + ic5trk)
# jpt
ic5jptJRA  = cms.Path(ic5jptPtEta  + ic5genPtEta + ic5jptJetToRef  + ic5jpt)

# calol2l3
kt4calol2l3JRA=cms.Path(kt4calol2l3PtEta+kt4genPtEta+kt4calol2l3JetToRef+kt4calol2l3)
kt6calol2l3JRA=cms.Path(kt6calol2l3PtEta+kt6genPtEta+kt6calol2l3JetToRef+kt6calol2l3)
sc5calol2l3JRA=cms.Path(sc5calol2l3PtEta+sc5genPtEta+sc5calol2l3JetToRef+sc5calol2l3)
sc7calol2l3JRA=cms.Path(sc7calol2l3PtEta+sc7genPtEta+sc7calol2l3JetToRef+sc7calol2l3)
ic5calol2l3JRA=cms.Path(ic5calol2l3PtEta+ic5genPtEta+ic5calol2l3JetToRef+ic5calol2l3)

# pfl2l3
ic5pfl2l3JRA  =cms.Path(ic5pfl2l3PtEta  +ic5genPtEta+ic5pfl2l3JetToRef  +ic5pfl2l3)


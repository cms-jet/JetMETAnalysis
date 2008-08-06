import FWCore.ParameterSet.Config as cms


#!
#! DEFAULT ETA/PT PRESELECTION
#!
JetPtEta = cms.PSet()
#    etaMin = cms.double(-5.0),
#    etaMax = cms.double(5.0),
#    ptMin = cms.double(1.0)
#)
RefPtEta = cms.PSet(
    etaMin = cms.double(-5.0),
    etaMax = cms.double(5.0),
    ptMin = cms.double(1.0)
)

#!
#! DEFAULT RESPONSE PARAMETERS
#!
JetResponseParameters = cms.PSet()


#!
#! MODULES
#!

# genjets
kt4GenPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    RefPtEta,
    src = cms.InputTag("kt4GenJets")
)

kt6GenPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    RefPtEta,
    src = cms.InputTag("kt6GenJets")
)

sc5GenPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    RefPtEta,
    src = cms.InputTag("sisCone5GenJets")
)

sc7GenPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    RefPtEta,
    src = cms.InputTag("sisCone7GenJets")
)

ic5GenPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    RefPtEta,
    src = cms.InputTag("iterativeCone5GenJets")
)

# kt4calo
kt4CaloPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    JetPtEta,
    src = cms.InputTag("kt4CaloJets")
)

kt4GenToCalo = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("kt4GenPtEta"),
    srcRec = cms.InputTag("kt4CaloPtEta")
)

kt4calo = cms.EDAnalyzer("JetResponseAnalyzer",
    JetResponseParameters,
    srcRefToJetMap = cms.InputTag("kt4GenToCalo","gen2rec"),
    srcRef = cms.InputTag("kt4GenPtEta")
)

#kt6calo
kt6CaloPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    JetPtEta,
    src = cms.InputTag("kt6CaloJets")
)

kt6GenToCalo = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("kt6GenPtEta"),
    srcRec = cms.InputTag("kt6CaloPtEta")
)

kt6calo = cms.EDAnalyzer("JetResponseAnalyzer",
    JetResponseParameters,
    srcRefToJetMap = cms.InputTag("kt6GenToCalo","gen2rec"),
    srcRef = cms.InputTag("kt6GenPtEta")
)

# sc5calo
sc5CaloPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    JetPtEta,
    src = cms.InputTag("sisCone5CaloJets")
)

sc5GenToCalo = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("sc5GenPtEta"),
    srcRec = cms.InputTag("sc5CaloPtEta")
)

sc5calo = cms.EDAnalyzer("JetResponseAnalyzer",
    JetResponseParameters,
    srcRefToJetMap = cms.InputTag("sc5GenToCalo","gen2rec"),
    srcRef = cms.InputTag("sc5GenPtEta")
)

# sc7calo
sc7CaloPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    JetPtEta,
    src = cms.InputTag("sisCone7CaloJets")
)

sc7GenToCalo = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("sc7GenPtEta"),
    srcRec = cms.InputTag("sc7CaloPtEta")
)

sc7calo = cms.EDAnalyzer("JetResponseAnalyzer",
    JetResponseParameters,
    srcRefToJetMap = cms.InputTag("sc7GenToCalo","gen2rec"),
    srcRef = cms.InputTag("sc7GenPtEta")
)

# ic5calo
ic5CaloPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    JetPtEta,
    src = cms.InputTag("iterativeCone5CaloJets")
)

ic5GenToCalo = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("ic5GenPtEta"),
    srcRec = cms.InputTag("ic5CaloPtEta")
)

ic5calo = cms.EDAnalyzer("JetResponseAnalyzer",
    JetResponseParameters,
    srcRefToJetMap = cms.InputTag("ic5GenToCalo","gen2rec"),
    srcRef = cms.InputTag("ic5GenPtEta")
)

# kt4pflow
kt4PFlowPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    JetPtEta,
    src = cms.InputTag("kt4PFJets")
)

kt4GenToPFlow = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("kt4GenPtEta"),
    srcRec = cms.InputTag("kt4PFlowPtEta")
)

kt4pf = cms.EDAnalyzer("JetResponseAnalyzer",
    JetResponseParameters,
    srcRefToJetMap = cms.InputTag("kt4GenToPFlow","gen2rec"),
    srcRef = cms.InputTag("kt4GenPtEta")
)

# kt6pflow
kt6PFlowPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    JetPtEta,
    src = cms.InputTag("kt6PFJets")
)

kt6GenToPFlow = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("kt6GenPtEta"),
    srcRec = cms.InputTag("kt6PFlowPtEta")
)

kt6pf = cms.EDAnalyzer("JetResponseAnalyzer",
    JetResponseParameters,
    srcRefToJetMap = cms.InputTag("kt6GenToPFlow","gen2rec"),
    srcRef = cms.InputTag("kt6GenPtEta")
)

# sc5pflow
sc5PFlowPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    JetPtEta,
    src = cms.InputTag("sisCone5PFJets")
)

sc5GenToPFlow = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("sc5GenPtEta"),
    srcRec = cms.InputTag("sc5PFlowPtEta")
)

sc5pf = cms.EDAnalyzer("JetResponseAnalyzer",
    JetResponseParameters,
    srcRefToJetMap = cms.InputTag("sc5GenToPFlow","gen2rec"),
    srcRef = cms.InputTag("sc5GenPtEta")
)

#sc7pflow
sc7PFlowPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    JetPtEta,
    src = cms.InputTag("sisCone7PFJets")
)

sc7GenToPFlow = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("sc7GenPtEta"),
    srcRec = cms.InputTag("sc7PFlowPtEta")
)

sc7pf = cms.EDAnalyzer("JetResponseAnalyzer",
    JetResponseParameters,
    srcRefToJetMap = cms.InputTag("sc7GenToPFlow","gen2rec"),
    srcRef = cms.InputTag("sc7GenPtEta")
)

#ic5pflow
ic5PFlowPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    JetPtEta,
    src = cms.InputTag("iterativeCone5PFJets")
)

ic5GenToPFlow = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("ic5GenPtEta"),
    srcRec = cms.InputTag("ic5PFlowPtEta")
)

ic5pf = cms.EDAnalyzer("JetResponseAnalyzer",
    JetResponseParameters,
    srcRefToJetMap = cms.InputTag("ic5GenToPFlow","gen2rec"),
    srcRef = cms.InputTag("ic5GenPtEta")
)

# kt4trk
kt4TrackPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    JetPtEta,
    src = cms.InputTag("kt4TrackJets")
)

kt4GenToTrack = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("kt4GenPtEta"),
    srcRec = cms.InputTag("kt4TrackPtEta")
)

kt4trk = cms.EDAnalyzer("JetResponseAnalyzer",
    JetResponseParameters,
    srcRefToJetMap = cms.InputTag("kt4GenToTrack","gen2rec"),
    srcRef = cms.InputTag("kt4GenPtEta")
)

# kt6trk
kt6TrackPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    JetPtEta,
    src = cms.InputTag("kt6TrackJets")
)

kt6GenToTrack = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("kt6GenPtEta"),
    srcRec = cms.InputTag("kt6TrackPtEta")
)

kt6trk = cms.EDAnalyzer("JetResponseAnalyzer",
    JetResponseParameters,
    srcRefToJetMap = cms.InputTag("kt6GenToTrack","gen2rec"),
    srcRef = cms.InputTag("kt6GenPtEta")
)

#sc5trk
sc5TrackPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    JetPtEta,
    src = cms.InputTag("sisCone5TrackJets")
)

sc5GenToTrack = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("sc5GenPtEta"),
    srcRec = cms.InputTag("sc5TrackPtEta")
)

sc5trk = cms.EDAnalyzer("JetResponseAnalyzer",
    JetResponseParameters,
    srcRefToJetMap = cms.InputTag("sc5GenToTrack","gen2rec"),
    srcRef = cms.InputTag("sc5GenPtEta")
)

# sc7trk
sc7TrackPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    JetPtEta,
    src = cms.InputTag("sisCone7TrackJets")
)

sc7GenToTrack = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("sc7GenPtEta"),
    srcRec = cms.InputTag("sc7TrackPtEta")
)

sc7trk = cms.EDAnalyzer("JetResponseAnalyzer",
    JetResponseParameters,
    srcRefToJetMap = cms.InputTag("sc7GenToTrack","gen2rec"),
    srcRef = cms.InputTag("sc7GenPtEta")
)

# ic5trk
ic5TrackPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    JetPtEta,
    src = cms.InputTag("iterativeCone5TrackJets")
)

ic5GenToTrack = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("ic5GenPtEta"),
    srcRec = cms.InputTag("ic5TrackPtEta")
)

ic5trk = cms.EDAnalyzer("JetResponseAnalyzer",
    JetResponseParameters,
    srcRefToJetMap = cms.InputTag("ic5GenToTrack","gen2rec"),
    srcRef = cms.InputTag("ic5GenPtEta")
)


#!
#! PATHS
#!
rsp_kt4calo  = cms.Path(kt4CaloPtEta  + kt4GenPtEta + kt4GenToCalo  + kt4calo)
rsp_kt6calo  = cms.Path(kt6CaloPtEta  + kt6GenPtEta + kt6GenToCalo  + kt6calo)
rsp_sc5calo  = cms.Path(sc5CaloPtEta  + sc5GenPtEta + sc5GenToCalo  + sc5calo)
rsp_sc7calo  = cms.Path(sc7CaloPtEta  + sc7GenPtEta + sc7GenToCalo  + sc7calo)
rsp_ic5calo  = cms.Path(ic5CaloPtEta  + ic5GenPtEta + ic5GenToCalo  + ic5calo)
rsp_kt4pflow = cms.Path(kt4PFlowPtEta + kt4GenPtEta + kt4GenToPFlow + kt4pf)
rsp_kt6pflow = cms.Path(kt6PFlowPtEta + kt6GenPtEta + kt6GenToPFlow + kt6pf)
rsp_sc5pflow = cms.Path(sc5PFlowPtEta + sc5GenPtEta + sc5GenToPFlow + sc5pf)
rsp_sc7pflow = cms.Path(sc7PFlowPtEta + sc7GenPtEta + sc7GenToPFlow + sc7pf)
rsp_ic5pflow = cms.Path(ic5PFlowPtEta + ic5GenPtEta + ic5GenToPFlow + ic5pf)
rsp_kt4track = cms.Path(kt4TrackPtEta + kt4GenPtEta + kt4GenToTrack + kt4trk)
rsp_kt6track = cms.Path(kt6TrackPtEta + kt6GenPtEta + kt6GenToTrack + kt6trk)
rsp_sc5track = cms.Path(sc5TrackPtEta + sc5GenPtEta + sc5GenToTrack + sc5trk)
rsp_sc7track = cms.Path(sc7TrackPtEta + sc7GenPtEta + sc7GenToTrack + sc7trk)
rsp_ic5track = cms.Path(ic5TrackPtEta + ic5GenPtEta + ic5GenToTrack + ic5trk)


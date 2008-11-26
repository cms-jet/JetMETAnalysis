import FWCore.ParameterSet.Config as cms

import JetMETAnalysis.JetAnalyzers.JRA_Defaults_cff as Defaults;


#!
#! MODULES
#!

#
# GenJets
#
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

ak5genPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.RefPtEta,
    src = cms.InputTag("ak5GenJets")
)

ak7genPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.RefPtEta,
    src = cms.InputTag("ak7GenJets")
)

ca4genPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.RefPtEta,
    src = cms.InputTag("ca4GenJets")
)

ca6genPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.RefPtEta,
    src = cms.InputTag("ca6GenJets")
)



#
# CaloJets
#

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
    src = cms.InputTag("L2L3CorJetKT4Calo")
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

# kt6calo
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

# kt6calol2l3
kt6calol2l3PtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("L2L3CorJetKT6Calo")
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
    src = cms.InputTag("L2L3CorJetSC5Calo")
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
    src = cms.InputTag("L2L3CorJetSC7Calo")
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
    src = cms.InputTag("L2L3CorJetIC5Calo")
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

# ak5calo
ak5caloPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("ak5CaloJets")
)

ak5caloJetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("ak5genPtEta"),
    srcRec = cms.InputTag("ak5caloPtEta")
)

ak5calo = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("ak5caloJetToRef","gen2rec"),
    srcRef = cms.InputTag("ak5genPtEta")
)

# ak5calol2l3
ak5calol2l3PtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("L2L3CorJetAK5Calo")
)

ak5calol2l3JetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("ak5genPtEta"),
    srcRec = cms.InputTag("ak5calol2l3PtEta")
)

ak5calol2l3 = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("ak5calol2l3JetToRef","gen2rec"),
    srcRef = cms.InputTag("ak5genPtEta")
)

# ak7calo
ak7caloPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("ak7CaloJets")
)

ak7caloJetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("ak7genPtEta"),
    srcRec = cms.InputTag("ak7caloPtEta")
)

ak7calo = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("ak7caloJetToRef","gen2rec"),
    srcRef = cms.InputTag("ak7genPtEta")
)

# ak7calol2l3
ak7calol2l3PtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("L2L3CorJetAK7Calo")
)

ak7calol2l3JetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("ak7genPtEta"),
    srcRec = cms.InputTag("ak7calol2l3PtEta")
)

ak7calol2l3 = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("ak7calol2l3JetToRef","gen2rec"),
    srcRef = cms.InputTag("ak7genPtEta")
)

# ca4calo
ca4caloPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("ca4CaloJets")
)

ca4caloJetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("ca4genPtEta"),
    srcRec = cms.InputTag("ca4caloPtEta")
)

ca4calo = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("ca4caloJetToRef","gen2rec"),
    srcRef = cms.InputTag("ca4genPtEta")
)

# ca4calol2l3
ca4calol2l3PtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("L2L3CorJetCA4Calo")
)

ca4calol2l3JetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("ca4genPtEta"),
    srcRec = cms.InputTag("ca4calol2l3PtEta")
)

ca4calol2l3 = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("ca4calol2l3JetToRef","gen2rec"),
    srcRef = cms.InputTag("ca4genPtEta")
)

# ca6calo
ca6caloPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("ca6CaloJets")
)

ca6caloJetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("ca6genPtEta"),
    srcRec = cms.InputTag("ca6caloPtEta")
)

ca6calo = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("ca6caloJetToRef","gen2rec"),
    srcRef = cms.InputTag("ca6genPtEta")
)

# ca6calol2l3
ca6calol2l3PtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("L2L3CorJetCA6Calo")
)

ca6calol2l3JetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("ca6genPtEta"),
    srcRec = cms.InputTag("ca6calol2l3PtEta")
)

ca6calol2l3 = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("ca6calol2l3JetToRef","gen2rec"),
    srcRef = cms.InputTag("ca6genPtEta")
)



#
# PFJets
#

# kt4pf
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

# kt4pfl2l3
kt4pfl2l3PtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("L2L3CorJetKT4PF")
)

kt4pfl2l3JetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("kt4genPtEta"),
    srcRec = cms.InputTag("kt4pfl2l3PtEta")
)

kt4pfl2l3 = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("kt4pfl2l3JetToRef","gen2rec"),
    srcRef = cms.InputTag("kt4genPtEta")
)

# kt6pf
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

# kt6pfl2l3
kt6pfl2l3PtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("L2L3CorJetKT6PF")
)

kt6pfl2l3JetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("kt6genPtEta"),
    srcRec = cms.InputTag("kt6pfl2l3PtEta")
)

kt6pfl2l3 = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("kt6pfl2l3JetToRef","gen2rec"),
    srcRef = cms.InputTag("kt6genPtEta")
)

# sc5pf
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

# sc5pfl2l3
sc5pfl2l3PtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("L2L3CorJetSC5PF")
)

sc5pfl2l3JetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("sc5genPtEta"),
    srcRec = cms.InputTag("sc5pfl2l3PtEta")
)

sc5pfl2l3 = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("sc5pfl2l3JetToRef","gen2rec"),
    srcRef = cms.InputTag("sc5genPtEta")
)

# sc7pf
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

# sc7pfl2l3
sc7pfl2l3PtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("L2L3CorJetSC7PF")
)

sc7pfl2l3JetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("sc7genPtEta"),
    srcRec = cms.InputTag("sc7pfl2l3PtEta")
)

sc7pfl2l3 = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("sc7pfl2l3JetToRef","gen2rec"),
    srcRef = cms.InputTag("sc7genPtEta")
)

# ic5pf
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

# ic5pfl2l3
ic5pfl2l3PtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("L2L3CorJetIC5PF")
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

# ak5pf
ak5pfPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("ak5PFJets")
)

ak5pfJetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("ak5genPtEta"),
    srcRec = cms.InputTag("ak5pfPtEta")
)

ak5pf = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("ak5pfJetToRef","gen2rec"),
    srcRef = cms.InputTag("ak5genPtEta")
)

# ak5pfl2l3
ak5pfl2l3PtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("L2L3CorJetAK5PF")
)

ak5pfl2l3JetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("ak5genPtEta"),
    srcRec = cms.InputTag("ak5pfl2l3PtEta")
)

ak5pfl2l3 = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("ak5pfl2l3JetToRef","gen2rec"),
    srcRef = cms.InputTag("ak5genPtEta")
)

# ak7pf
ak7pfPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("ak7PFJets")
)

ak7pfJetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("ak7genPtEta"),
    srcRec = cms.InputTag("ak7pfPtEta")
)

ak7pf = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("ak7pfJetToRef","gen2rec"),
    srcRef = cms.InputTag("ak7genPtEta")
)

# ak7pfl2l3
ak7pfl2l3PtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("L2L3CorJetAK7PF")
)

ak7pfl2l3JetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("ak7genPtEta"),
    srcRec = cms.InputTag("ak7pfl2l3PtEta")
)

ak7pfl2l3 = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("ak7pfl2l3JetToRef","gen2rec"),
    srcRef = cms.InputTag("ak7genPtEta")
)

# ca4pf
ca4pfPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("ca4PFJets")
)

ca4pfJetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("ca4genPtEta"),
    srcRec = cms.InputTag("ca4pfPtEta")
)

ca4pf = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("ca4pfJetToRef","gen2rec"),
    srcRef = cms.InputTag("ca4genPtEta")
)

# ca4pfl2l3
ca4pfl2l3PtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("L2L3CorJetCA4PF")
)

ca4pfl2l3JetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("ca4genPtEta"),
    srcRec = cms.InputTag("ca4pfl2l3PtEta")
)

ca4pfl2l3 = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("ca4pfl2l3JetToRef","gen2rec"),
    srcRef = cms.InputTag("ca4genPtEta")
)

# ca6pf
ca6pfPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("ca6PFJets")
)

ca6pfJetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("ca6genPtEta"),
    srcRec = cms.InputTag("ca6pfPtEta")
)

ca6pf = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("ca6pfJetToRef","gen2rec"),
    srcRef = cms.InputTag("ca6genPtEta")
)

# ca6pfl2l3
ca6pfl2l3PtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("L2L3CorJetCA6PF")
)

ca6pfl2l3JetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("ca6genPtEta"),
    srcRec = cms.InputTag("ca6pfl2l3PtEta")
)

ca6pfl2l3 = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("ca6pfl2l3JetToRef","gen2rec"),
    srcRef = cms.InputTag("ca6genPtEta")
)



#
# TrackJets
#

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

#ak5trk
ak5trkPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("ak5TrackJets")
)

ak5trkJetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("ak5genPtEta"),
    srcRec = cms.InputTag("ak5trkPtEta")
)

ak5trk = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("ak5trkJetToRef","gen2rec"),
    srcRef = cms.InputTag("ak5genPtEta")
)

# ak7trk
ak7trkPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("ak7TrackJets")
)

ak7trkJetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("ak7genPtEta"),
    srcRec = cms.InputTag("ak7trkPtEta")
)

ak7trk = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("ak7trkJetToRef","gen2rec"),
    srcRef = cms.InputTag("ak7genPtEta")
)

# ca4trk
ca4trkPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("ca4TrackJets")
)

ca4trkJetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("ca4genPtEta"),
    srcRec = cms.InputTag("ca4trkPtEta")
)

ca4trk = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("ca4trkJetToRef","gen2rec"),
    srcRef = cms.InputTag("ca4genPtEta")
)

# ca6trk
ca6trkPtEta = cms.EDFilter("EtaPtMinCandViewSelector",
    Defaults.JetPtEta,
    src = cms.InputTag("ca6TrackJets")
)

ca6trkJetToRef = cms.EDFilter("MatchRecToGen",
    srcGen = cms.InputTag("ca6genPtEta"),
    srcRec = cms.InputTag("ca6trkPtEta")
)

ca6trk = cms.EDAnalyzer("JetResponseAnalyzer",
    Defaults.JetResponseParameters,
    srcRefToJetMap = cms.InputTag("ca6trkJetToRef","gen2rec"),
    srcRef = cms.InputTag("ca6genPtEta")
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

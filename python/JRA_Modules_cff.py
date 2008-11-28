import FWCore.ParameterSet.Config as cms

import JetMETAnalysis.JetAnalyzers.JRA_Defaults_cff as Defaults


#
# Partons
#
partons = cms.EDFilter("PdgIdAndStatusCandViewSelector",
    src    = cms.InputTag("genParticles"),
    status = cms.vint32(3),
    pdgId  = cms.vint32(1, 2, 3, 4, 5, 21)
)


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
# GenJet -> Parton Matching
#
kt4GenToParton = cms.EDFilter("MatchRecToGen",
    srcRec = cms.InputTag("kt4genPtEta"),
    srcGen = cms.InputTag("partons")
)
kt6GenToParton = cms.EDFilter("MatchRecToGen",
    srcRec = cms.InputTag("kt6genPtEta"),
    srcGen = cms.InputTag("partons")
)
sc5GenToParton = cms.EDFilter("MatchRecToGen",
    srcRec = cms.InputTag("sc5genPtEta"),
    srcGen = cms.InputTag("partons")
)
sc7GenToParton = cms.EDFilter("MatchRecToGen",
    srcRec = cms.InputTag("sc7genPtEta"),
    srcGen = cms.InputTag("partons")
)
ic5GenToParton = cms.EDFilter("MatchRecToGen",
    srcRec = cms.InputTag("ic5genPtEta"),
    srcGen = cms.InputTag("partons")
)
ak5GenToParton = cms.EDFilter("MatchRecToGen",
    srcRec = cms.InputTag("ak5genPtEta"),
    srcGen = cms.InputTag("partons")
)
ak7GenToParton = cms.EDFilter("MatchRecToGen",
    srcRec = cms.InputTag("ak7genPtEta"),
    srcGen = cms.InputTag("partons")
)
ca4GenToParton = cms.EDFilter("MatchRecToGen",
    srcRec = cms.InputTag("ca4genPtEta"),
    srcGen = cms.InputTag("partons")
)
ca6GenToParton = cms.EDFilter("MatchRecToGen",
    srcRec = cms.InputTag("ca6genPtEta"),
    srcGen = cms.InputTag("partons")
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


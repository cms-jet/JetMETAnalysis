import FWCore.ParameterSet.Config as cms

#!
#! JET & REFERENCE KINEMATIC CUTS
#!
import JetMETAnalysis.JetAnalyzers.Defaults_cff as Defaults;

Defaults.JetPtEta = cms.PSet(
    etaMin = cms.double(-5.0),
    etaMax = cms.double(5.0),
    ptMin  = cms.double(1.0)
)
Defaults.RefPtEta = cms.PSet(
    etaMin = cms.double(-5.0),
    etaMax = cms.double(5.0),
    ptMin = cms.double(10.0)
)


#!
#! JRA PARAMETERS: TREE MODE
#!
import JetMETAnalysis.JetAnalyzers.TreeDefaults_cff as Tree;
Defaults.JetResponseParameters = Tree.JetResponseParameters


#!
#! PROCESS
#!
process = cms.Process("JRA")


#!
#! CONDITIONS (DELIVERING JEC BY DEFAULT!)
#!
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = "START38_V14::All"
# process.GlobalTag.globaltag = "START39_V8::All"


#!
#! INPUT
#!
qcdFiles = cms.untracked.vstring(
    '/store/relval/CMSSW_3_8_7/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/MC_38Y_V13-v1/0017/D6537BBF-99FC-DF11-B625-001A92810ADE.root',
    '/store/relval/CMSSW_3_8_7/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/MC_38Y_V13-v1/0017/9A95A153-9DFC-DF11-8E53-0018F3D09676.root',
    '/store/relval/CMSSW_3_8_7/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/MC_38Y_V13-v1/0017/90930ECB-9CFC-DF11-A619-002354EF3BD2.root',
    '/store/relval/CMSSW_3_8_7/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/MC_38Y_V13-v1/0017/0A1C9C2C-A6FC-DF11-B6CC-001A92971B8C.root',
    '/store/relval/CMSSW_3_8_7/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/MC_38Y_V13-v1/0016/8ED89570-91FC-DF11-A865-002618943832.root',
    '/store/relval/CMSSW_3_8_7/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/MC_38Y_V13-v1/0016/2A7FF391-93FC-DF11-9E9A-0018F3D0970C.root',
    '/store/relval/CMSSW_3_8_7/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/MC_38Y_V13-v1/0016/1034E06F-92FC-DF11-8804-0026189438DE.root',
    '/store/relval/CMSSW_3_8_7/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/MC_38Y_V13-v1/0016/04E1957A-90FC-DF11-A98A-001A92810A96.root'
    )

ttbarFiles = cms.untracked.vstring(
    '/store/relval/CMSSW_3_8_7/RelValTTbar/GEN-SIM-RECO/MC_38Y_V13-v1/0017/6E197082-93FC-DF11-9205-00261894392C.root',
    '/store/relval/CMSSW_3_8_7/RelValTTbar/GEN-SIM-RECO/MC_38Y_V13-v1/0016/FAB6D480-84FC-DF11-8F1A-00304867915A.root',
    '/store/relval/CMSSW_3_8_7/RelValTTbar/GEN-SIM-RECO/MC_38Y_V13-v1/0016/F2E75DCF-83FC-DF11-8095-003048679228.root',
    '/store/relval/CMSSW_3_8_7/RelValTTbar/GEN-SIM-RECO/MC_38Y_V13-v1/0016/C8725806-7EFC-DF11-842E-003048D42D92.root',
    '/store/relval/CMSSW_3_8_7/RelValTTbar/GEN-SIM-RECO/MC_38Y_V13-v1/0016/C846A888-7DFC-DF11-B7A0-002618943984.root',
    '/store/relval/CMSSW_3_8_7/RelValTTbar/GEN-SIM-RECO/MC_38Y_V13-v1/0016/C6DD4C8F-83FC-DF11-8998-0018F3D0965A.root',
    '/store/relval/CMSSW_3_8_7/RelValTTbar/GEN-SIM-RECO/MC_38Y_V13-v1/0016/86015F0D-7EFC-DF11-8C48-0026189438CF.root',
    '/store/relval/CMSSW_3_8_7/RelValTTbar/GEN-SIM-RECO/MC_38Y_V13-v1/0016/5605E388-7DFC-DF11-B572-0026189438B3.root',
    '/store/relval/CMSSW_3_8_7/RelValTTbar/GEN-SIM-RECO/MC_38Y_V13-v1/0016/02C6B001-7FFC-DF11-A6F2-0026189438E9.root'
    )

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(100))
process.source = cms.Source("PoolSource", fileNames = qcdFiles )


#!
#! SERVICES
#!
process.MessageLogger = cms.Service("MessageLogger",
    destinations = cms.untracked.vstring('cout'),
    cout         = cms.untracked.PSet(threshold = cms.untracked.string('WARNING'))
)
process.TFileService = cms.Service("TFileService",
    fileName      = cms.string('JRA.root'),
    closeFileFast = cms.untracked.bool(True)
)


#!
#! CHOOSE ALGORITHMS
#!
from JetMETAnalysis.JetAnalyzers.addAlgorithm import addAlgorithm

algorithms = []

algorithms.append('ak5calo')
algorithms.append('ak5calol2l3')
algorithms.append('ak7calo')
algorithms.append('ak7calol2l3')
algorithms.append('kt4calo')
algorithms.append('kt4calol2l3')
algorithms.append('kt6calo')
algorithms.append('kt6calol2l3')
algorithms.append('ic5calo')
algorithms.append('ic5calol2l3')

algorithms.append('ak5pf')
algorithms.append('ak5pfl2l3')
algorithms.append('ak7pf')
algorithms.append('ak7pfl2l3')
algorithms.append('kt4pf')
algorithms.append('kt4pfl2l3')
algorithms.append('kt6pf')
algorithms.append('kt6pfl2l3')
algorithms.append('ic5pf')
algorithms.append('ic5pfl2l3')

algorithms.append('ak5jpt')
algorithms.append('ak5jptl2l3')

#
# taus
# ----
# require additional tags in CMSSW_3_8_6:
#
# V01-00-01 DataFormats/TauReco
# V01-00-11 RecoTauTag/Configuration
# V01-00-19 RecoTauTag/RecoTau
# V01-00-03 RecoTauTag/TauTagTools
#
#algorithms.append('ak5tauAll')
#algorithms.append('ak5tauHpsLoose')
#algorithms.append('ak5tauHpsMedium')
#algorithms.append('ak5tauHpsTight')
#algorithms.append('ak5tauTaNCLoose')
#algorithms.append('ak5tauTaNCMedium')
#algorithms.append('ak5tauTaNCTight')
#algorithms.append('ak5tauCombinedLoose')
#algorithms.append('ak5tauCombinedMedium')
#algorithms.append('ak5tauCombinedTight')



# set to False to use jets from the input file (NOT RECOMMENDED)
doJetReco = True

for algorithm in algorithms:
    addAlgorithm(process,algorithm,doJetReco)


#!
#! THAT'S ALL! CAN YOU BELIEVE IT? :-D
#!

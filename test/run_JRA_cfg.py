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
#! JRA PARAMETERS: HISTO MODE
#!
# import JetMETAnalysis.JetAnalyzers.HistoDefaults_cff as Histos;
# Defaults.JetResponseParameters = Histos.JetResponseParameters


#!
#! PROCESS
#!
process = cms.Process("JRA")



#!
#! INPUT
#!
qcdFiles = cms.untracked.vstring(
    '/store/relval/CMSSW_3_8_5/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/MC_38Y_V12-v1/0040/90D11926-23D2-DF11-90AA-0018F3D0965E.root',
    '/store/relval/CMSSW_3_8_5/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/MC_38Y_V12-v1/0040/7C03C61F-23D2-DF11-94BA-003048678AF4.root',
    '/store/relval/CMSSW_3_8_5/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/MC_38Y_V12-v1/0040/10AF2CAB-B9D2-DF11-8564-002618943870.root',
    '/store/relval/CMSSW_3_8_5/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/MC_38Y_V12-v1/0039/D8419BA5-D8D1-DF11-9E15-003048678FF2.root',
    '/store/relval/CMSSW_3_8_5/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/MC_38Y_V12-v1/0039/745F327F-CCD1-DF11-949F-001A9281170A.root',
    '/store/relval/CMSSW_3_8_5/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/MC_38Y_V12-v1/0039/66D95526-D7D1-DF11-AF48-0026189438A9.root',
    '/store/relval/CMSSW_3_8_5/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/MC_38Y_V12-v1/0039/42622E31-D5D1-DF11-891A-001A928116D2.root'
    )

ttbarFiles = cms.untracked.vstring(
    '/store/relval/CMSSW_3_8_5/RelValTTbar/GEN-SIM-RECO/START38_V12-v1/0040/AA852471-22D2-DF11-ADFA-0026189438EF.root',
    '/store/relval/CMSSW_3_8_5/RelValTTbar/GEN-SIM-RECO/START38_V12-v1/0040/9CDDD25C-E9D1-DF11-BE39-001A92971B5E.root',
    '/store/relval/CMSSW_3_8_5/RelValTTbar/GEN-SIM-RECO/START38_V12-v1/0040/4A1F8072-22D2-DF11-8BA3-002618943885.root',
    '/store/relval/CMSSW_3_8_5/RelValTTbar/GEN-SIM-RECO/START38_V12-v1/0040/3667674E-EAD1-DF11-8507-002618943933.root',
    '/store/relval/CMSSW_3_8_5/RelValTTbar/GEN-SIM-RECO/START38_V12-v1/0040/1A487D44-23D2-DF11-9DB4-003048678B0C.root',
    '/store/relval/CMSSW_3_8_5/RelValTTbar/GEN-SIM-RECO/START38_V12-v1/0040/16B7DF72-22D2-DF11-9901-002618943943.root',
    '/store/relval/CMSSW_3_8_5/RelValTTbar/GEN-SIM-RECO/START38_V12-v1/0040/0840E1E4-B5D2-DF11-8F73-002618943870.root',
    '/store/relval/CMSSW_3_8_5/RelValTTbar/GEN-SIM-RECO/START38_V12-v1/0039/DAAAA626-E6D1-DF11-8EF4-0018F3D096E4.root',
    '/store/relval/CMSSW_3_8_5/RelValTTbar/GEN-SIM-RECO/START38_V12-v1/0039/D4505BAD-E7D1-DF11-B96A-00261894386B.root'
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


# set to False to use jets from the input file (NOT RECOMMENDED)
doJetReco = True

for algorithm in algorithms:
    addAlgorithm(process,algorithm,doJetReco)


#!
#! THAT'S ALL! CAN YOU BELIEVE IT? :-D
#!

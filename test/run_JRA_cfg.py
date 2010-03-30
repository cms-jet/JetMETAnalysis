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
    '/store/relval/CMSSW_3_3_1/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/MC_31X_V9-v3/0003/FC5633F4-CAC0-DE11-9B8C-0030487C6090.root',
    '/store/relval/CMSSW_3_3_1/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/MC_31X_V9-v3/0003/AC1E71D1-C9C0-DE11-AD08-0030487C6090.root',
    '/store/relval/CMSSW_3_3_1/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/MC_31X_V9-v3/0003/AAEB831F-E4C0-DE11-85CC-0030487C6090.root',
    '/store/relval/CMSSW_3_3_1/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/MC_31X_V9-v3/0003/80B9B076-C6C0-DE11-970A-000423D98B6C.root',
    '/store/relval/CMSSW_3_3_1/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/MC_31X_V9-v3/0003/7A6512C0-9CC1-DE11-9228-0030487A18F2.root',
    '/store/relval/CMSSW_3_3_1/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/MC_31X_V9-v3/0003/1CE3F169-C7C0-DE11-914D-0030487C6090.root',
    '/store/relval/CMSSW_3_3_1/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/MC_31X_V9-v3/0003/16D5C17F-C8C0-DE11-9BEE-0030487A1990.root'    
    )

ttbarFiles = cms.untracked.vstring(
    '/store/relval/CMSSW_3_3_0/RelValTTbar/GEN-SIM-RECO/MC_31X_V9-v2/0003/8C293B1C-7DBD-DE11-A138-002618943969.root',
    '/store/relval/CMSSW_3_3_0/RelValTTbar/GEN-SIM-RECO/MC_31X_V9-v2/0002/80746BC6-E1BC-DE11-B6AF-0026189438F4.root',
    '/store/relval/CMSSW_3_3_0/RelValTTbar/GEN-SIM-RECO/MC_31X_V9-v2/0002/76B9924A-E2BC-DE11-8D8F-001A92971B06.root',
    '/store/relval/CMSSW_3_3_0/RelValTTbar/GEN-SIM-RECO/MC_31X_V9-v2/0002/6A476ECE-E7BC-DE11-941A-002618943923.root',
    '/store/relval/CMSSW_3_3_0/RelValTTbar/GEN-SIM-RECO/MC_31X_V9-v2/0002/585171F0-4CBD-DE11-B96D-001A92971AA4.root',
    '/store/relval/CMSSW_3_3_0/RelValTTbar/GEN-SIM-RECO/MC_31X_V9-v2/0002/348829ED-E9BC-DE11-B2A5-0026189438C4.root',
    '/store/relval/CMSSW_3_3_0/RelValTTbar/GEN-SIM-RECO/MC_31X_V9-v2/0002/249CA932-E9BC-DE11-9C34-00261894393E.root'
    )

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(1000))
process.source = cms.Source(
    "PoolSource", fileNames = qcdFiles
    )


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
#! CHOOSE JET ENERGY CORRECTION SET
#!
from JetMETCorrections.Configuration.JetCorrectionEra_cff import JetCorrectionEra
JetCorrectionEra.era = 'Summer09_7TeV_ReReco332'


#!
#! CHOOSE ALGORITHMS
#!
from JetMETAnalysis.JetAnalyzers.addAlgorithm import addAlgorithm

algorithms = []

algorithms.append('ak5calo')
algorithms.append('ak5calol2l3')
algorithms.append('ak7calo')
algorithms.append('ak7calol2l3')
algorithms.append('sc5calo')
algorithms.append('sc5calol2l3')
algorithms.append('sc7calo')
algorithms.append('sc7calol2l3')
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
algorithms.append('sc5pf')
algorithms.append('sc5pfl2l3')
algorithms.append('sc7pf')
algorithms.append('sc7pfl2l3')
algorithms.append('kt4pf')
algorithms.append('kt4pfl2l3')
algorithms.append('kt6pf')
algorithms.append('kt6pfl2l3')
algorithms.append('ic5pf')
algorithms.append('ic5pfl2l3')

algorithms.append('ak5jpt')
algorithms.append('ak5jptl2l3')
algorithms.append('sc5jpt')
algorithms.append('sc5jptl2l3')
algorithms.append('ic5jpt')
algorithms.append('ic5jptl2l3')

# set to False to use jets from the input file (NOT RECOMMENDED)
doJetReco = True

for algorithm in algorithms:
    addAlgorithm(process,algorithm,doJetReco)


#!
#! THAT'S ALL! CAN YOU BELIEVE IT? :-D
#!

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
    '/store/relval/CMSSW_3_5_7/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/MC_3XY_V26-v1/0013/B6AAFEE4-6849-DF11-A42F-0026189437FD.root',
    '/store/relval/CMSSW_3_5_7/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/MC_3XY_V26-v1/0012/FE3569A8-5849-DF11-B926-0018F3D09652.root',
    '/store/relval/CMSSW_3_5_7/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/MC_3XY_V26-v1/0012/625514A2-5849-DF11-ACEA-0018F3D096A4.root',
    '/store/relval/CMSSW_3_5_7/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/MC_3XY_V26-v1/0012/221C3CAC-5949-DF11-8847-003048678F02.root',
    '/store/relval/CMSSW_3_5_7/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/MC_3XY_V26-v1/0012/20A090C0-5A49-DF11-B921-001A92810ADC.root',
    '/store/relval/CMSSW_3_5_7/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/MC_3XY_V26-v1/0012/1281AF09-5749-DF11-B389-001A92971B26.root',
    '/store/relval/CMSSW_3_5_7/RelValQCD_FlatPt_15_3000/GEN-SIM-RECO/MC_3XY_V26-v1/0012/082A389B-5749-DF11-8421-001A92810ACA.root'
    )

ttbarFiles = cms.untracked.vstring(
    '/store/relval/CMSSW_3_5_7/RelValTTbar/GEN-SIM-RECO/START3X_V26-v1/0013/C452DA3E-6949-DF11-8457-002618943975.root',
    '/store/relval/CMSSW_3_5_7/RelValTTbar/GEN-SIM-RECO/START3X_V26-v1/0012/F8624D39-5349-DF11-A757-001A92971B36.root',
    '/store/relval/CMSSW_3_5_7/RelValTTbar/GEN-SIM-RECO/START3X_V26-v1/0012/F4252D9B-5249-DF11-A065-001A92810AC4.root',
    '/store/relval/CMSSW_3_5_7/RelValTTbar/GEN-SIM-RECO/START3X_V26-v1/0012/E2BA50BC-5D49-DF11-B108-003048678CA2.root',
    '/store/relval/CMSSW_3_5_7/RelValTTbar/GEN-SIM-RECO/START3X_V26-v1/0012/DE696E91-5149-DF11-A286-003048679266.root',
    '/store/relval/CMSSW_3_5_7/RelValTTbar/GEN-SIM-RECO/START3X_V26-v1/0012/AE690A9A-5049-DF11-BE2D-003048679294.root',
    '/store/relval/CMSSW_3_5_7/RelValTTbar/GEN-SIM-RECO/START3X_V26-v1/0012/849A6B50-5A49-DF11-932C-001A92810ADC.root',
    '/store/relval/CMSSW_3_5_7/RelValTTbar/GEN-SIM-RECO/START3X_V26-v1/0012/5A2E40BB-5B49-DF11-B20A-003048679076.root',
    '/store/relval/CMSSW_3_5_7/RelValTTbar/GEN-SIM-RECO/START3X_V26-v1/0012/06E0961F-6149-DF11-BF10-003048678EE2.root'
    )

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(1000))
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
algorithms.append('ic5jpt')
algorithms.append('ic5jptl2l3')




# set to False to use jets from the input file (NOT RECOMMENDED)
doJetReco = True

for algorithm in algorithms:
    addAlgorithm(process,algorithm,doJetReco)


#!
#! THAT'S ALL! CAN YOU BELIEVE IT? :-D
#!

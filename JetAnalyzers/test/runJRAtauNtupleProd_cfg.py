import FWCore.ParameterSet.Config as cms

#!
#! JET & REFERENCE KINEMATIC CUTS
#!
import JetMETAnalysis.JetAnalyzers.Defaults_cff as Defaults

Defaults.JetPtEta = cms.PSet(
    etaMin = cms.double(-2.5),
    etaMax = cms.double(+2.5),
    ptMin  = cms.double(15.0)
)
Defaults.RefPtEta = cms.PSet(
    etaMin = cms.double(-2.5),
    etaMax = cms.double(+2.5),
    ptMin = cms.double(15.0)
)
Defaults.JetResponseParameters.doComposition = True
Defaults.JetResponseParameters.deltaRMax = cms.double(0.5)

#!
#! PROCESS
#!
process = cms.Process("JRAtau")


#!
#! CONDITIONS (DELIVERING JEC BY DEFAULT!)
#!
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = "PHYS14_25_V2::All"


#!
#! INPUT
#!
tauFiles = cms.untracked.vstring(
    'file:TTbarH_HToTauTau_M-125_13TeV_AODSIM.root'
)

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(100))
process.source = cms.Source("PoolSource", fileNames = tauFiles )


#!
#! SERVICES
#!
process.load('FWCore.MessageLogger.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.load('CommonTools.UtilAlgos.TFileService_cfi')
process.TFileService.fileName=cms.string('ntupleJRAtau_Ztautau.root')


#!
#! CHOOSE ALGORITHMS
#!
from JetMETAnalysis.JetAnalyzers.addAlgorithm import addAlgorithm

algorithms = []
algorithms.append('ak5tauHPSlooseCombDBcorrAll')
algorithms.append('ak5tauHPSlooseCombDBcorrOneProng0Pi0')
algorithms.append('ak5tauHPSlooseCombDBcorrOneProng1Pi0')
algorithms.append('ak5tauHPSlooseCombDBcorrOneProng2Pi0')
algorithms.append('ak5tauHPSlooseCombDBcorrThreeProng0Pi0')
algorithms.append('ak5tauHPSmediumCombDBcorrAll')
algorithms.append('ak5tauHPSmediumCombDBcorrOneProng0Pi0')
algorithms.append('ak5tauHPSmediumCombDBcorrOneProng1Pi0')
algorithms.append('ak5tauHPSmediumCombDBcorrOneProng2Pi0')
algorithms.append('ak5tauHPSmediumCombDBcorrThreeProng0Pi0')
algorithms.append('ak5tauHPStightCombDBcorrAll')
algorithms.append('ak5tauHPStightCombDBcorrOneProng0Pi0')
algorithms.append('ak5tauHPStightCombDBcorrOneProng1Pi0')
algorithms.append('ak5tauHPStightCombDBcorrOneProng2Pi0')
algorithms.append('ak5tauHPStightCombDBcorrThreeProng0Pi0')


# set to False to use jets/taus from the input file (NOT RECOMMENDED)
doJetReco = True

for algorithm in algorithms:
    addAlgorithm(process, algorithm, Defaults, doJetReco, False)


#!
#! THAT'S ALL! CAN YOU BELIEVE IT? :-D
#!

processDumpFile = open('runJRAtauNtupleProd.dump' , 'w')
print >> processDumpFile, process.dumpPython()

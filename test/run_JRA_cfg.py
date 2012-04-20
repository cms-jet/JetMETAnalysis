import FWCore.ParameterSet.Config as cms

#!
#! JET & REFERENCE KINEMATIC CUTS
#!
import JetMETAnalysis.JetAnalyzers.Defaults_cff as Defaults;

Defaults.JetPtEta = cms.PSet(
    etaMin = cms.double(-5.5),
    etaMax = cms.double(5.5),
    ptMin  = cms.double(1.0)
)
Defaults.RefPtEta = cms.PSet(
    etaMin = cms.double(-5.5),
    etaMax = cms.double(5.5),
    ptMin = cms.double(1.0)
)
Defaults.JetResponseParameters.doComposition = True
Defaults.JetResponseParameters.doHLT = True

#!
#! PROCESS
#!
process = cms.Process("JRA")


#!
#! CONDITIONS (DELIVERING JEC BY DEFAULT!)
#!
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = "START42_V17::All"


#!
#! INPUT
#!
qcdFiles = cms.untracked.vstring(
         '/store/mc/Summer12/QCD_Pt-15to3000_TuneZ2star_Flat_8TeV_pythia6/GEN-SIM-RECO/PU_S7_START52_V5-v1/0000/FEEA2CFE-9B7B-E111-BD9F-00266CFFA1FC.root'
         ##'/store/user/lpcjm/lungu/QCD_Pt-15to3000_TuneZ2_Flat_7TeV_pythia6/Fall11-QCD_Pt-15to3000_TuneZ2_7TeV_pythia6_reHLTRAWRECO_v3//564fc77dd089d3ffa20dc78b12538743/myQCD_RECO_89_1_0W5.root'
    )

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(100))
process.source = cms.Source("PoolSource", fileNames = qcdFiles )


#!
#! SERVICES
#!
process.load('FWCore.MessageLogger.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.load('CommonTools.UtilAlgos.TFileService_cfi')
process.TFileService.fileName=cms.string('JRA.root')

#!
#! NEEDED FOR PFCHS
#!
process.load('CommonTools.ParticleFlow.pfNoPileUp_cff')
process.pfPileUp.checkClosestZVertex = False


#!
#! CHOOSE ALGORITHMS
#!
from JetMETAnalysis.JetAnalyzers.addAlgorithm import addAlgorithm

algorithms = []

algorithms.append('ak5calo')
algorithms.append('ak7calo')
#algorithms.append('kt4calo')
#algorithms.append('kt6calo')
#algorithms.append('ak5caloHLT')
algorithms.append('ak5pf')
algorithms.append('ak7pf')
#algorithms.append('kt4pf')
#algorithms.append('kt6pf')
algorithms.append('ak5pfchs')
algorithms.append('ak7pfchs')
#algorithms.append('ak5pfHLT')
#algorithms.append('ak5pfchsHLT')
algorithms.append('ak5jpt')
algorithms.append('ak7jpt')

#algorithms.append('ak5calol1')
#algorithms.append('ak5caloHLTl1')
#algorithms.append('ak7calol1')
#algorithms.append('ak5pfl1')
#algorithms.append('ak7pfl1')
#algorithms.append('ak5pfchsl1')
#algorithms.append('ak7pfchsl1')
#algorithms.append('ak5pfHLTl1')
#algorithms.append('ak5pfchsHLTl1')

#algorithms.append('ak5calol1off')
#algorithms.append('ak7calol1off')
#algorithms.append('ak5caloHLTl1off')
#algorithms.append('ak5pfl1off')
#algorithms.append('ak7pfl1off')
#algorithms.append('ak5pfchsl1off')
#algorithms.append('ak7pfchsl1off')
#algorithms.append('ak5pfHLTl1off')
#algorithms.append('ak5pfchsHLTl1off')
#algorithms.append('ak5jptl1')
#algorithms.append('ak7jptl1')

#algorithms.append('ak5calol2l3')
#algorithms.append('ak7calol2l3')
#algorithms.append('kt4calol2l3')
#algorithms.append('kt6calol2l3')
#algorithms.append('ak5caloHLTl2l3')
#algorithms.append('ak5pfl2l3')
#algorithms.append('ak7pfl2l3')
#algorithms.append('kt4pfl2l3')
#algorithms.append('kt6pfl2l3')
#algorithms.append('ak5pfchsl2l3')
#algorithms.append('ak7pfchsl2l3')
#algorithms.append('ak5pfHLTl2l3')
#algorithms.append('ak5pfchsHLTl2l3')
#algorithms.append('ak5jptl2l3')
#algorithms.append('ak7jptl2l3')

#algorithms.append('ak5calol1l2l3')
#algorithms.append('ak7calol1l2l3')
#algorithms.append('kt4calol1l2l3')
#algorithms.append('kt6calol1l2l3')
#algorithms.append('ak5caloHLTl1l2l3')
#algorithms.append('ak5pfl1l2l3')
#algorithms.append('ak7pfl1l2l3')
#algorithms.append('kt4pfl1l2l3')
#algorithms.append('kt6pfl1l2l3')
#algorithms.append('ak5pfchsl1l2l3')
#algorithms.append('ak7pfchsl1l2l3')
#algorithms.append('ak5pfHLTl1l2l3')
#algorithms.append('ak5pfchsHLTl1l2l3')

#algorithms.append('ak5calol1offl2l3')
#algorithms.append('ak7calol1offl2l3')
#algorithms.append('ak5caloHLTl1offl2l3')
#algorithms.append('ak5pfl1offl2l3')
#algorithms.append('ak7pfl1offl2l3')
#algorithms.append('ak5pfchsl1offl2l3')
#algorithms.append('ak7pfchsl1offl2l3')
#algorithms.append('ak5pfHLTl1offl2l3')
#algorithms.append('ak5pfchsHLTl1offl2l3')
#algorithms.append('ak5jptl1l2l3')
#algorithms.append('ak7jptl1l2l3')



#
# taus
# ----
#
#algorithms.append('ak5tauHPSall')
#algorithms.append('ak5tauHPSloose')
#algorithms.append('ak5tauHPSmedium')
#algorithms.append('ak5tauHPStight')
##algorithms.append('ak5tauHPSlooseDBcorr')
##algorithms.append('ak5tauHPSmediumDBcorr')
##algorithms.append('ak5tauHPStightDBcorr')
##algorithms.append('ak5tauHPSlooseCombDBcorr')
##algorithms.append('ak5tauHPSmediumCombDBcorr')
##algorithms.append('ak5tauHPStightCombDBcorr')
#algorithms.append('ak5tauTaNCall')
#algorithms.append('ak5tauTaNCloose)'
#algorithms.append('ak5tauTaNCmedium')
#algorithms.append('ak5tauTaNCtight')


# set to False to use jets from the input file (NOT RECOMMENDED)
doJetReco = True

for algorithm in algorithms:
    if (algorithm.find('HLT') > 0) :
        addAlgorithm(process,algorithm,Defaults,False)
    else:
        addAlgorithm(process,algorithm,Defaults,doJetReco)

process.ak5CaloJets.jetPtMin = 1.0
process.ak5PFJets.jetPtMin = 1.0
#process.ak7CaloJets.jetPtMin = 1.0
#process.ak7PFJets.jetPtMin = 1.0


#!
#! THAT'S ALL! CAN YOU BELIEVE IT? :-D
#!

#Not sure what this does
#processDumpFile = open('runJRA.dump' , 'w')
#print >> processDumpFile, process.dumpPython()

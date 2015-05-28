import FWCore.ParameterSet.Config as cms

#!
#! JET & REFERENCE KINEMATIC CUTS
#!
import JetMETAnalysis.JetAnalyzers.Defaults_cff as Defaults

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
applyDBFile = False
era = "PHYS14_V4_MC"
doProducer = False
process = cms.Process("JRA")
if doProducer:
	process = cms.Process("JRAP")


#!
#! CHOOSE ALGORITHMS
#!

algsizetype = {'ak':[4,8]}
#algsizetype = {'ak':[1,2,3,4,5,6,7,8,9,10]}
jettype = ['puppi']
#jettype = ['calo','pf','pfchs','puppi']
corrs = ['']
#corrs = ['','l1','l2l3','l1l2l3']

algorithms = []
jcr = cms.VPSet()

loop = True

if loop:
    for k, v in algsizetype.iteritems():
	for s in v:
            for j in jettype:
		for c in corrs:
	            algorithms.append(str(k+str(s)+j+c))
		    if applyDBFile:
		       upperAlg = str(k.upper()+str(s)+j.upper().replace("CHS","chs"))
		       jcr.append(cms.PSet(record = cms.string("JetCorrectionsRecord"),
					   tag = cms.string("JetCorrectorParametersCollection_"+era+"_"+upperAlg),
					   label= cms.untracked.string(upperAlg)))

else:
    # SAMPLE
    algorithms.append('ak5calo')


#!
#! CONDITIONS (DELIVERING JEC BY DEFAULT!)
#!
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = "PHYS14_25_V2::All"

if applyDBFile:
	from CondCore.DBCommon.CondDBSetup_cfi import *
	process.jec = cms.ESSource("PoolDBESSource",CondDBSetup,
				   connect = cms.string('sqlite_file:DBFiles/'+era+'.db'),
				   #cms.string("frontier://FrontierPrep/CMS_COND_PHYSICSTOOLS"),
				   toGet =  cms.VPSet(jcr))
	process.es_prefer_jec = cms.ESPrefer("PoolDBESSource","jec")


#!
#! INPUT
#!
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))

###################################
# Matthieu Marionneau Re-Reco 731 #
###################################
#process.load("JetMETAnalysis.JetAnalyzers.mmarionn_QCDFlatWin15_731_cff")
###################################
# Matthieu Marionneau Re-Reco 732 #
###################################
#process.load("JetMETAnalysis.JetAnalyzers.mmarionn_QCDFlatWin15_732_cff")
###################################
# Matthieu Marionneau Re-Reco 733 #
###################################
#process.load("JetMETAnalysis.JetAnalyzers.mmarionn_QCDFlatWin15_733_cff")

qcdFiles = cms.untracked.vstring(
	'root://cmsxrootd.fnal.gov//store/mc/Fall14DR73/QCD_Pt-15to7000_TuneCUETP8M1_Flat_13TeV_pythia8/GEN-SIM-RECO/PFr1_Flat_20_50_50ns_GSFromFall14_MCRUN2_73_V11-v1/00000/000998AB-A8E7-E411-A76F-00261894394F.root',
    )
process.source = cms.Source("PoolSource", fileNames = qcdFiles )


#!
#! SERVICES
#!
process.load('FWCore.MessageLogger.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 5000
if not doProducer:
	process.load('CommonTools.UtilAlgos.TFileService_cfi')
	process.TFileService.fileName=cms.string('JRA.root')


#!
#! NEEDED FOR PFCHS (last two lines needed for 44X and up)
#!
#process.load('CommonTools.ParticleFlow.pfNoPileUp_cff')
#process.pfPileUp.PFCandidates = 'particleFlow' 
#process.pfNoPileUp.bottomCollection = 'particleFlow'
process.load('CommonTools.ParticleFlow.pfNoPileUpJME_cff')
process.pfPileUp.checkClosestZVertex = False


#!
#! RUN JET RESPONSE ANALYZER
#!

# set to False to use jets from the input file (NOT RECOMMENDED)
doJetReco = True
outCom = cms.untracked.vstring('drop *')
from JetMETAnalysis.JetAnalyzers.addAlgorithm import addAlgorithm
for algorithm in algorithms:
    if (algorithm.find('HLT') > 0) :
        process.load("Configuration.StandardSequences.Geometry_cff")
        process.load("Configuration.StandardSequences.MagneticField_cff")
        addAlgorithm(process,algorithm,Defaults,False,doProducer)
    else:
        addAlgorithm(process,algorithm,Defaults,doJetReco,doProducer)
    outCom.extend(['keep *_'+algorithm+'_*_*'])

#process.ak5CaloJets.jetPtMin = 1.0
#process.ak5PFJets.jetPtMin = 1.0
#process.ak7CaloJets.jetPtMin = 1.0
#process.ak7PFJets.jetPtMin = 1.0
#process.ak5PFchsJets.jetPtMin = 1.0
#process.ak7PFchsJets.jetPtMin = 1.0


#!
#! Check the keep and drop commands being added to the outputCommamnds
#!
#for oc in outCom:
#   print oc


#!
#! Output
#!
if doProducer:
    process.out = cms.OutputModule("PoolOutputModule",
	    						       fileName = cms.untracked.string('JRAP.root'),
		    					       outputCommands = outCom
			    				       #   outputCommands = cms.untracked.vstring(
				    			       #       "drop *",
					    		       #       "keep *_ak5pf_*_*"
						    	       #   )
							       )
    process.e = cms.EndPath(process.out)


#!
#! THAT'S ALL! CAN YOU BELIEVE IT? :-D
#!

#Not sure what this does
#processDumpFile = open('runJRA.dump' , 'w')
#print >> processDumpFile, process.dumpPython()
#process.options.wantSummary = True
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

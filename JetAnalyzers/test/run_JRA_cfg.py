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
#era = "Summer12_V1_MC"
#era = "ConeSizeTest_MC_PFchs"
#era = "Winter14_V1_MC"
#era = "AK4_V2_MC"
#era = "CSA14_V1_MC"
era = "TPStudies_GEM2019Upg14DR-final_phase1_age1k_PU140bx25_PH1_1K_FB_V2-v1_MC"
#era = "TPStudies_GEM2019Upg14DR-final_phase1_PU50bx25_DES19_62_V8-v1_MC"
doProducer = False
process = cms.Process("JRA")
if doProducer:
	process = cms.Process("JRAP")

#!
#! CONDITIONS (DELIVERING JEC BY DEFAULT!)
#!
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = "START53_V7F::All"

if applyDBFile:
	from CondCore.DBCommon.CondDBSetup_cfi import *
	process.jec = cms.ESSource("PoolDBESSource",CondDBSetup,
							   connect = cms.string('sqlite_file:'+era+'.db'),
							   #cms.string("frontier://FrontierPrep/CMS_COND_PHYSICSTOOLS"),
							   toGet =  cms.VPSet(
#		cms.PSet(record = cms.string("JetCorrectionsRecord"),
#				 tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK5Calo"),
#				 label= cms.untracked.string("AK5Calo")),
		cms.PSet(record = cms.string("JetCorrectionsRecord"),
				 tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK5PF"),
				 label= cms.untracked.string("AK5PF")),
		cms.PSet(record = cms.string("JetCorrectionsRecord"),
				 tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK5PFchs"),
				 label=cms.untracked.string("AK5PFchs")),
#		cms.PSet(record = cms.string("JetCorrectionsRecord"),
#				 tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK5JPT"),
#				 label=cms.untracked.string("AK5JPT")),
#		cms.PSet(record = cms.string("JetCorrectionsRecord"),
#				 tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK7Calo"),
#				 label= cms.untracked.string("AK7Calo")),
		cms.PSet(record = cms.string("JetCorrectionsRecord"),
				 tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK7PF"),
				 label= cms.untracked.string("AK7PF")),
		cms.PSet(record = cms.string("JetCorrectionsRecord"),
				 tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK7PFchs"),
				 label=cms.untracked.string("AK7PFchs")),
#		cms.PSet(record = cms.string("JetCorrectionsRecord"),
#				 tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK7JPT"),
#				 label=cms.untracked.string("AK7JPT")),
        cms.PSet(record = cms.string("JetCorrectionsRecord"),
				 tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK1PF"),
				 label= cms.untracked.string("AK1PF")),
		cms.PSet(record = cms.string("JetCorrectionsRecord"),
				 tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK2PF"),
				 label= cms.untracked.string("AK2PF")),
		cms.PSet(record = cms.string("JetCorrectionsRecord"),
				 tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK3PF"),
				 label= cms.untracked.string("AK3PF")),
		cms.PSet(record = cms.string("JetCorrectionsRecord"),
				 tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK4PF"),
				 label= cms.untracked.string("AK4PF")),
		cms.PSet(record = cms.string("JetCorrectionsRecord"),
				 tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK6PF"),
				 label= cms.untracked.string("AK6PF")),
		cms.PSet(record = cms.string("JetCorrectionsRecord"),
				 tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK8PF"),
				 label= cms.untracked.string("AK8PF")),
		cms.PSet(record = cms.string("JetCorrectionsRecord"),
				 tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK9PF"),
				 label= cms.untracked.string("AK9PF")),
		cms.PSet(record = cms.string("JetCorrectionsRecord"),
				 tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK10PF"),
				 label= cms.untracked.string("AK10PF")),
		cms.PSet(record = cms.string("JetCorrectionsRecord"),
				 tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK1PFchs"),
				 label= cms.untracked.string("AK1PFchs")),
		cms.PSet(record = cms.string("JetCorrectionsRecord"),
				 tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK2PFchs"),
				 label= cms.untracked.string("AK2PFchs")),
		cms.PSet(record = cms.string("JetCorrectionsRecord"),
				 tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK3PFchs"),
				 label= cms.untracked.string("AK3PFchs")),
		cms.PSet(record = cms.string("JetCorrectionsRecord"),
				 tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK4PFchs"),
				 label= cms.untracked.string("AK4PFchs")),
		cms.PSet(record = cms.string("JetCorrectionsRecord"),
				 tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK6PFchs"),
				 label= cms.untracked.string("AK6PFchs")),
		cms.PSet(record = cms.string("JetCorrectionsRecord"),
				 tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK8PFchs"),
				 label= cms.untracked.string("AK8PFchs")),
		cms.PSet(record = cms.string("JetCorrectionsRecord"),
				 tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK9PFchs"),
				 label= cms.untracked.string("AK9PFchs")),
		cms.PSet(record = cms.string("JetCorrectionsRecord"),
				 tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK10PFchs"),
				 label= cms.untracked.string("AK10PFchs")),
		)
							   )
	process.es_prefer_jec = cms.ESPrefer("PoolDBESSource","jec")

#!
#! INPUT
#!
qcdFiles = cms.untracked.vstring(
###########
# with PU #
###########
#	'/store/mc/Summer12_DR53X/QCD_Pt-15to3000_TuneZ2star_Flat_8TeV_pythia6/AODSIM/PU_RD1_START53_V7N-v1/00000/0432447C-346D-E311-A777-0025904B130A.root'
##############
# without PU #
##############
#   '/store/mc/Summer12_DR53X/QCD_Pt-15to3000_TuneZ2star_Flat_8TeV_pythia6/AODSIM/NoPileup_START53_V7A-v1/0000/001252D8-A0E4-E111-A131-0030487D5D8D.root',
#   '/store/mc/Summer12_DR53X/QCD_Pt-15to3000_TuneZ2star_Flat_8TeV_pythia6/AODSIM/NoPileup_START53_V7A-v1/0000/00684351-86E4-E111-AAE9-002481E10B86.root',
#   '/store/mc/Summer12_DR53X/QCD_Pt-15to3000_TuneZ2star_Flat_8TeV_pythia6/AODSIM/NoPileup_START53_V7A-v1/0000/02764AB0-ABE4-E111-9407-003048C69296.root',
###################
# Upgrade with PU #
###################
	'/store/mc/GEM2019Upg14DR/QCD_Pt-15to3000_Tune4C_Flat_14TeV_pythia8/AODSIM/final_phase1_age1k_PU140bx25_PH1_1K_FB_V2-v1/00000/00100795-421C-E411-88E9-00261894384A.root'
######################
# Upgrade without PU #
######################
	
    )
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(100))
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
#! CHOOSE ALGORITHMS
#!
from JetMETAnalysis.JetAnalyzers.addAlgorithm import addAlgorithm

algsizetype = {'ak':[4]}
#algsizetype = {'ak':[1,2,3,4,5,6,7,8,9,10]}
jettype = ['pf','pfchs']
#jettype = ['calo','pf','pfchs']
corrs = ['']
#corrs = ['','l1','l2l3']

algorithms = []

loop = True

if loop:
	for k, v in algsizetype.iteritems():
		for s in v:
			for j in jettype:
				for c in corrs:
					algorithms.append(str(k+str(s)+j+c))

else:
	# SAMPLE
    algorithms.append('ak5calo')

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
outCom = cms.untracked.vstring('drop *')

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
#process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

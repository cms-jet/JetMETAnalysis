import FWCore.ParameterSet.Config as cms

#!
#! PROCESS
#!
# Conditions source options: GT, SQLite, DB
conditionsSource = "GT"
era = "Fall15_25nsV1_MC"
doProducer = False
process = cms.Process("JRA")
if doProducer:
	process = cms.Process("JRAP")


#!
#! CHOOSE ALGORITHMS
#!
# Note: Not all combinations of options will work
# Algorithm options: ak, kt, ic, sc, ca
# Size options: integers 1-10
# Jet type options: calo, pf, pfchs, puppi
# Correction levels: '' (blank), l1, l2, l3, l2l3, l1l2l3
algsizetype = {'ak':[4,8]}
jettype = ['pf','pfchs','puppi']
corrs = ['']

algorithms = []
jcr = cms.VPSet()

for k, v in algsizetype.iteritems():
    for s in v:
	for j in jettype:
            for c in corrs:
	        algorithms.append(str(k+str(s)+j+c))
	        if conditionsSource != "GT":
                    upperAlg = str(k.upper()+str(s)+j.upper().replace("CHS","chs")).replace("PUPPI","PFPuppi")
		    jcr.append(cms.PSet(record = cms.string("JetCorrectionsRecord"),
					tag = cms.string("JetCorrectorParametersCollection_"+era+"_"+upperAlg),
					label= cms.untracked.string(upperAlg)))

# If need be you can append additional jet collections using the style below
#algorithms.append('ak5calo')


#!
#! CONDITIONS (DELIVERING JEC BY DEFAULT!)
#!
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff")
process.GlobalTag.globaltag = cms.string('74X_mcRun2_asymptotic_v2')

if conditionsSource != "GT":
    if conditionsSource == "DB":
        conditionsConnect = cms.string("frontier://FrontierPrep/CMS_COND_PHYSICSTOOLS")
    elif conditionsSource == "SQLite":
	conditionsConnect = cms.string('sqlite_file:DBFiles/'+era+'.db')    

    from CondCore.DBCommon.CondDBSetup_cfi import *
    process.jec = cms.ESSource("PoolDBESSource",CondDBSetup,
			       connect = conditionsConnect,
			       toGet =  cms.VPSet(jcr))
    process.es_prefer_jec = cms.ESPrefer("PoolDBESSource","jec")


#!
#! INPUT
#!
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(10))

########################################
# QCD_PY6_RunIISpring15DR74_Asympt50ns #
########################################
process.load("JetMETAnalysis.JetAnalyzers.QCD_PY6_RunIISpring15DR74_Asympt50ns_cff")
#############################################
# QCD_PY6_RunIISpring15DR74_AsymptNoPU_50ns #
#############################################
#process.load("JetMETAnalysis.JetAnalyzers.QCD_PY6_RunIISpring15DR74_AsymptNoPU_50ns_cff")

qcdFiles = cms.untracked.vstring(
	'root://cmsxrootd.fnal.gov//store/mc/Fall14DR73/QCD_Pt-15to7000_TuneCUETP8M1_Flat_13TeV_pythia8/GEN-SIM-RECO/PFr1_Flat_20_50_50ns_GSFromFall14_MCRUN2_73_V11-v1/00000/000998AB-A8E7-E411-A76F-00261894394F.root',
    )
#process.source = cms.Source("PoolSource", fileNames = qcdFiles )


#!
#! SERVICES
#!
process.load('FWCore.MessageLogger.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 5000
if not doProducer:
    process.load('CommonTools.UtilAlgos.TFileService_cfi')
    process.TFileService.fileName=cms.string('JRA.root')


#!
#! NEEDED FOR PFCHS
#!
process.load('CommonTools.ParticleFlow.pfNoPileUpJME_cff')
process.pfPileUp.checkClosestZVertex = False


#!
#! JET & REFERENCE KINEMATIC CUTS
#!
import JetMETAnalysis.JetAnalyzers.Defaults_cff as Defaults


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


#!
#! Check the keep and drop commands being added to the outputCommamnds
#!
printOC = False
if printOC:
    for oc in outCom:
        print oc


#!
#! Output
#!
if doProducer:
    process.out = cms.OutputModule("PoolOutputModule",
				   fileName = cms.untracked.string('JRAP.root'),
				   outputCommands = outCom
				   )
    process.e = cms.EndPath(process.out)


#!
#! THAT'S ALL! CAN YOU BELIEVE IT? :-D
#!

#Not sure what this does
#processDumpFile = open('runJRA.dump' , 'w')
#print >> processDumpFile, process.dumpPython()
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

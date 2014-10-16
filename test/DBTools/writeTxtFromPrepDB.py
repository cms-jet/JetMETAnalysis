import FWCore.ParameterSet.Config as cms

## import skeleton process
from PhysicsTools.PatAlgos.patTemplate_cfg import *
from PhysicsTools.PatAlgos.tools.coreTools import *
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import *

process = cms.Process("Test")

# Get the era we need to retrieve from the database
era = 'Summer12_V7_DATA'

#make sure that you end it with a '/'
outputDir = 'Summer12_V7_DATA_txts/'


from CondCore.DBCommon.CondDBSetup_cfi import *
# process.jec = cms.ESSource("PoolDBESSource",CondDBSetup,
#                            connect = cms.string("frontier://FrontierPrep/CMS_COND_PHYSICSTOOLS"),
#                            toGet =  cms.VPSet(
# 			    cms.PSet(
# 			            record = cms.string("JetCorrectionsRecord"),
#                                     tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK5Calo"),
#                                     label= cms.untracked.string("AK5Calo")),
#                             cms.PSet(
# 				    record = cms.string("JetCorrectionsRecord"),
#                                     tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK5PF"),
#                                     label=cms.untracked.string("AK5PF")),
#                             cms.PSet(
# 				    record = cms.string("JetCorrectionsRecord"),
#                                     tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK5PFchs"),
#                                     label=cms.untracked.string("AK5PFchs")),
#                             cms.PSet(
# 				    record = cms.string("JetCorrectionsRecord"),
# 				    tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK5JPT"),
# 				    label=cms.untracked.string("AK5JPT")),
# 			    cms.PSet(
# 				    record = cms.string('JetCorrectionsRecord'),
# 				    tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK7Calo'),
# 				    label  = cms.untracked.string('AK7Calo')),
# 			    cms.PSet(
# 				    record = cms.string('JetCorrectionsRecord'),
# 				    tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK7PF'),
# 				    label  = cms.untracked.string('AK7PF')),
# 			    cms.PSet(
# 				    record = cms.string('JetCorrectionsRecord'),
# 				    tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK7PFchs'),
# 				    label  = cms.untracked.string('AK7PFchs')),
# 			    cms.PSet(
# 				    record = cms.string('JetCorrectionsRecord'),
# 				    tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK7JPT'),
# 				    label  = cms.untracked.string('AK7JPT')),
# 			    cms.PSet(
# 				    record = cms.string("JetCorrectionsRecord"),
# 				    tag = cms.string("JetCorrectorParametersCollection_"+era+"_KT4Calo"),
# 				    label=cms.untracked.string("KT4Calo")),
#                             cms.PSet(
# 				    record = cms.string("JetCorrectionsRecord"),
# 				    tag = cms.string("JetCorrectorParametersCollection_"+era+"_KT4PF"),
# 				    label=cms.untracked.string("KT4PF"))
#                             )
#                            
#                         )
# 
# # Always prefer this set above the one in PoolDBESSource
# es_prefer_jec = cms.ESPrefer("PoolDBESSource","jec")

era = ''
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'GR_R_52_V9::All'#'START52_V9B::All'
  
# Need to run over at least 1 event to force the retrieval of payloads
process.maxEvents = cms.untracked.PSet(
        input = cms.untracked.int32(1)
        )

process.source = cms.Source("EmptySource")

process.demoPF = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK5PF'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputDir+era) 
                              )
process.demoPFchs = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK5PFchs'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputDir+era) 
                              )
process.demoCalo = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK5Calo'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputDir+era) 
                              )
process.demoJPT = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK5JPT'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputDir+era) 
                              )
process.demoKT4Calo = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('KT4Calo'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputDir+era) 
                              )
process.demoKT4PF = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('KT4PF'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputDir+era) 
                              )

process.demoAK7PF = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK7PF'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputDir+era) 
                              )
process.demoAK7PFchs = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK7PFchs'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputDir+era) 
                              )
process.demoAK7Calo = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK7Calo'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputDir+era) 
                              )
process.demoAK7JPT = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK7JPT'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputDir+era) 
                              )

# this one contains all of them
process.p  = cms.Path(process.demoPF * process.demoPFchs * process.demoCalo * process.demoJPT  * process.demoAK7PF * process.demoAK7PFchs * process.demoAK7Calo * process.demoAK7JPT * process.demoKT4Calo * process.demoKT4PF)





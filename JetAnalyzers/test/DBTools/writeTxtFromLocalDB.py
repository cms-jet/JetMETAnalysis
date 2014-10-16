import FWCore.ParameterSet.Config as cms

process = cms.Process("myprocess")
process.load("CondCore.DBCommon.CondDBCommon_cfi")

process.maxEvents = cms.untracked.PSet(
        input = cms.untracked.int32(1)
        )

process.source = cms.Source("EmptySource")

# The era
era = 'Winter14_V4_MC'
#era = 'ConeSizeTest_MC'
#era = 'Legacy11_V1_DATA'
#era = 'AK4_V2_MC'
#era = 'CSA14_V2_DATA'

# The database to connect to
localDB = 'sqlite:'+era+'.db'

# Prefix of the output files in directory with the era name_txt
outputPrefix = era+'_txts_fromDB/'+era

process.PoolDBESSource = cms.ESSource("PoolDBESSource",
      DBParameters = cms.PSet(
        messageLevel = cms.untracked.int32(0)
        ),
      timetype = cms.string('runnumber'),
      toGet = cms.VPSet(
#      cms.PSet(
#            record = cms.string('JetCorrectionsRecord'),
#            tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK5CaloHLT'),
#            label  = cms.untracked.string('AK5CaloHLT')
#            ),
#      cms.PSet(
#            record = cms.string('JetCorrectionsRecord'),
#            tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK5PFHLT'),
#            label  = cms.untracked.string('AK5PFHLT')
#            ),
#      cms.PSet(
#            record = cms.string('JetCorrectionsRecord'),
#            tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK5PFchsHLT'),
#            label  = cms.untracked.string('AK5PFchsHLT')
#            ),
      cms.PSet(
            record = cms.string('JetCorrectionsRecord'),
            tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK5Calo'),
            label  = cms.untracked.string('AK5Calo')
            ),
      cms.PSet(
            record = cms.string('JetCorrectionsRecord'),
            tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK5PF'),
            label  = cms.untracked.string('AK5PF')
            ),
      cms.PSet(
            record = cms.string('JetCorrectionsRecord'),
            tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK5PFchs'),
            label  = cms.untracked.string('AK5PFchs')
            ),
#      cms.PSet(
#            record = cms.string('JetCorrectionsRecord'),
#            tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK5JPT'),
#            label  = cms.untracked.string('AK5JPT')
#            ),
      cms.PSet(
            record = cms.string('JetCorrectionsRecord'),
            tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK7Calo'),
            label  = cms.untracked.string('AK7Calo')
            ),
      cms.PSet(
            record = cms.string('JetCorrectionsRecord'),
            tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK7PF'),
            label  = cms.untracked.string('AK7PF')
            ),
      cms.PSet(
            record = cms.string('JetCorrectionsRecord'),
            tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK7PFchs'),
            label  = cms.untracked.string('AK7PFchs')
            ),
#      cms.PSet(
#            record = cms.string('JetCorrectionsRecord'),
#            tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK7JPT'),
#            label  = cms.untracked.string('AK7JPT')
#            ),
#     cms.PSet(
#            record = cms.string('JetCorrectionsRecord'),
#            tag    = cms.string('JetCorrectorParametersCollection_'+era+'_KT4PF'),
#            label  = cms.untracked.string('KT4PF')
#            ),
#     cms.PSet(
#            record = cms.string('JetCorrectionsRecord'),
#            tag    = cms.string('JetCorrectorParametersCollection_'+era+'_KT4Calo'),
#            label  = cms.untracked.string('KT4Calo')
#            ),
#	  cms.PSet(
#            record = cms.string('JetCorrectionsRecord'),
#            tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK1PF'),
#            label  = cms.untracked.string('AK1PF')
#            ),
#	  cms.PSet(
#            record = cms.string('JetCorrectionsRecord'),
#            tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK2PF'),
#            label  = cms.untracked.string('AK2PF')
#            ),	  
#	  cms.PSet(
#            record = cms.string('JetCorrectionsRecord'),
#            tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK3PF'),
#            label  = cms.untracked.string('AK3PF')
#            ),
#	  cms.PSet(
#            record = cms.string('JetCorrectionsRecord'),
#            tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK4PF'),
#            label  = cms.untracked.string('AK4PF')
#            ),
#	  cms.PSet(
#            record = cms.string('JetCorrectionsRecord'),
#            tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK6PF'),
#            label  = cms.untracked.string('AK6PF')
#            ),
#	  cms.PSet(
#            record = cms.string('JetCorrectionsRecord'),
#            tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK8PF'),
#            label  = cms.untracked.string('AK8PF')
#            ),
#	  cms.PSet(
#            record = cms.string('JetCorrectionsRecord'),
#            tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK9PF'),
#            label  = cms.untracked.string('AK9PF')
#            ),
#	  cms.PSet(
#            record = cms.string('JetCorrectionsRecord'),
#            tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK10PF'),
#            label  = cms.untracked.string('AK10PF')
#            ),
#	  cms.PSet(
#            record = cms.string('JetCorrectionsRecord'),
#            tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK1PFchs'),
#            label  = cms.untracked.string('AK1PFchs')
#            ),
#	  cms.PSet(
#            record = cms.string('JetCorrectionsRecord'),
#            tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK2PFchs'),
#            label  = cms.untracked.string('AK2PFchs')
#            ),
#	  cms.PSet(
#            record = cms.string('JetCorrectionsRecord'),
#            tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK3PFchs'),
#            label  = cms.untracked.string('AK3PFchs')
#            ),
#	  cms.PSet(
#            record = cms.string('JetCorrectionsRecord'),
#            tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK4PFchs'),
#            label  = cms.untracked.string('AK4PFchs')
#            ),
#	  cms.PSet(
#            record = cms.string('JetCorrectionsRecord'),
#            tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK6PFchs'),
#            label  = cms.untracked.string('AK6PFchs')
#            ),
#	  cms.PSet(
#            record = cms.string('JetCorrectionsRecord'),
#            tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK8PFchs'),
#            label  = cms.untracked.string('AK8PFchs')
#            ),
#	  cms.PSet(
#            record = cms.string('JetCorrectionsRecord'),
#            tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK9PFchs'),
#            label  = cms.untracked.string('AK9PFchs')
#            ),
#	  cms.PSet(
#            record = cms.string('JetCorrectionsRecord'),
#            tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK10PFchs'),
#            label  = cms.untracked.string('AK10PFchs')
#            )
     ),
      connect = cms.string(localDB)
)



process.demoCaloHLT = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK5CaloHLT'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputPrefix) 
                             )
process.demoPFHLT = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK5PFHLT'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputPrefix) 
                             )
process.demoPFchsHLT = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK5PFchsHLT'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputPrefix) 
                             )
process.demoPF = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK5PF'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputPrefix) 
                             )
process.demoPFchs = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK5PFchs'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputPrefix) 
                             )
process.demoCalo = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK5Calo'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputPrefix) 
                             )
process.demoJPT = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK5JPT'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputPrefix) 
                             )
process.demo7PF = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK7PF'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputPrefix) 
                             )
process.demo7PFchs = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK7PFchs'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputPrefix) 
                             )
process.demo7Calo = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK7Calo'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputPrefix) 
                             )
process.demo7JPT = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK7JPT'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputPrefix) 
                             )
process.demoKT4Calo = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('KT4Calo'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputPrefix) 
                             )
process.demoKT4PF = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('KT4PF'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputPrefix) 
                             )
process.demoAK1PF = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK1PF'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputPrefix) 
                             )
process.demoAK2PF = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK2PF'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputPrefix) 
                             )
process.demoAK3PF = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK3PF'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputPrefix) 
                             )
process.demoAK4PF = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK4PF'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputPrefix) 
                             )
process.demoAK6PF = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK6PF'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputPrefix) 
                             )
process.demoAK8PF = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK8PF'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputPrefix) 
                             )
process.demoAK9PF = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK9PF'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputPrefix) 
                             )
process.demoAK10PF = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK10PF'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputPrefix) 
                             )
process.demoAK1PFchs = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK1PFchs'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputPrefix) 
                             )
process.demoAK2PFchs = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK2PFchs'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputPrefix) 
                             )
process.demoAK3PFchs = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK3PFchs'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputPrefix) 
                             )
process.demoAK4PFchs = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK4PFchs'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputPrefix) 
                             )
process.demoAK6PFchs = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK6PFchs'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputPrefix) 
                             )
process.demoAK8PFchs = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK8PFchs'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputPrefix) 
                             )
process.demoAK9PFchs = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK9PFchs'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputPrefix) 
                             )
process.demoAK10PFchs = cms.EDAnalyzer('JetCorrectorDBReader',
                              payloadName    = cms.untracked.string('AK10PFchs'),
                              printScreen    = cms.untracked.bool(False),
                              createTextFile = cms.untracked.bool(True),
                              globalTag      = cms.untracked.string(outputPrefix) 
                             )

#process.p = cms.Path(process.demoAK4PFchs)
#process.p = cms.Path(process.demoPFchs)
#process.p  = cms.Path(process.demoPFHLT * process.demoPFchsHLT * process.demoCaloHLT )
#process.p  = cms.Path(process.demoPF * process.demoPFchs * process.demoCalo * process.demoJPT )
process.p  = cms.Path(process.demoPF * process.demoPFchs * process.demoCalo * process.demo7PF * process.demo7PFchs * process.demo7Calo )
#process.p  = cms.Path(process.demoPF * process.demoPFchs * process.demoCalo * process.demoJPT * process.demo7PF * process.demo7PFchs * process.demo7Calo * process.demo7JPT )
#process.p  = cms.Path(process.demoPF * process.demoPFchs * process.demoCalo * process.demoJPT * process.demo7PF * process.demo7PFchs * process.demo7Calo * process.demo7JPT * process.demoKT4Calo * process.demoKT4PF )
#process.p = cms.Path(process.demoPF * process.demo7PF * process.demoAK3PF * process.demoAK4PF * process.demoAK6PF * process.demoAK8PF * process.demoAK9PF * process.demoAK10PF)
#process.p = cms.Path(process.demoPFchs * process.demo7PFchs * process.demoAK1PFchs * process.demoAK2PFchs * process.demoAK3PFchs * process.demoAK4PFchs * process.demoAK6PFchs * process.demoAK8PFchs * process.demoAK9PFchs * process.demoAK10PFchs)
#process.p = cms.Path(process.demoPF * process.demoPFchs * process.demoCalo * process.demo7PF * process.demo7PFchs * process.demo7Calo * process.demoAK1PF * process.demoAK2PF * process.demoAK3PF * process.demoAK4PF * process.demoAK6PF * process.demoAK8PF * process.demoAK9PF * process.demoAK10PF * process.demoAK1PFchs * process.demoAK2PFchs * process.demoAK3PFchs * process.demoAK4PFchs * process.demoAK6PFchs * process.demoAK8PFchs * process.demoAK9PFchs * process.demoAK10PFchs)

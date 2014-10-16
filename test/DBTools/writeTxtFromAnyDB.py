import FWCore.ParameterSet.Config as cms

process = cms.Process("myprocess")
process.load("CondCore.DBCommon.CondDBCommon_cfi")

process.maxEvents = cms.untracked.PSet(
        input = cms.untracked.int32(1)
        )

process.source = cms.Source("EmptySource")

# define here the algorithms to run over
algs = ( "AK5Calo" ,
         "AK5PF"   ,
         "AK5PFchs",
         "AK5JPT"  ,
         "AK7Calo" ,
         "AK7PF"   ,
         "AK7PFchs",
         "AK7JPT"  ,
         "KT4PF"   ,
         "KT4Calo")
      
for algo in algs:
  print "algorithm is ", algo
  

# The era
era = 'Jec11_V12'

# The database to connect to
localDB = 'sqlite:'+era+'.db'

# Prefix of the output files
outputPrefix = era+'_sqlfile/'+era


# Create the VPSet here
vpset = cms.VPSet()

# create one PSet here
pset = cms.PSet(
                record = cms.string('JetCorrectionsRecord'),
                tag    = cms.string('JetCorrectorParametersCollection_'+era+'_AK5Calo'),
                label  = cms.untracked.string('AK5Calo')
               )

print " pset conf value=",pset.configValue()
 

# add the PSet's into VPSet
pset.insertContentsInto(vpset)
# vpset._place("myprocess",pset)

##-------------------- Communicate with the DB -----------------------
# use these two lines for Global TAGS
# process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
# process.GlobalTag.globaltag = 'START42_V13::All'


process.PoolDBESSource = cms.ESSource("PoolDBESSource",
      DBParameters = cms.PSet(
          messageLevel = cms.untracked.int32(0)
          ),
      timetype = cms.string('runnumber'),
      toGet = vpset,
      connect = cms.string(localDB)
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

process.p  = cms.Path(process.demoPF * process.demoPFchs * process.demoCalo * process.demoJPT * process.demo7PF * process.demo7PFchs * process.demo7Calo * process.demo7JPT * process.demoKT4Calo * process.demoKT4PF )


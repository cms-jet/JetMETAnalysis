from WMCore.Configuration import Configuration
config = Configuration()
config.section_("General")
config.General.requestName = 'REQUESTNAME'

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'run_JRA_cfg.py'
config.JobType.allowUndistributedCMSSW = True
#config.JobType.inputFiles = ['']

config.section_("Data")
config.Data.inputDataset = 'INPUTDATASET'
config.Data.inputDBS = 'https://cmsweb.cern.ch/dbs/prod/DBSURL/DBSReader/'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = FILESPERJOB
config.Data.publication = False
config.Data.publishDBS = 'https://cmsweb.cern.ch/dbs/prod/phys03/DBSWriter/'
config.Data.outputDatasetTag = 'PUBLISHDATANAME'
config.Data.outLFNDirBase = '/store/user/aperloff/'
config.Data.ignoreLocality = True

config.section_("Site")
config.Site.storageSite = 'T3_US_TAMU'

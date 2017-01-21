from CRABClient.UserUtilities import config, getUsernameFromSiteDB

config = config()
config.General.requestName = 'QCDHLTJEC'
config.General.transferOutputs = True
config.General.transferLogs = False
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'run_JRA_hlt_cfg.py'
config.JobType.maxJobRuntimeMin = 2800
config.Data.inputDataset = '/QCD_Pt-15to3000_TuneCUETP8M1_Flat_13TeV_pythia8/fengwang-Fall16HLTJEC_80X_FlatPU0to70HcalNZSRAW_v1-afe01c608037eee01c6ee6bf3906eb29/USER'
config.Data.inputDBS = 'phys03'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 6
config.Data.outLFNDirBase = '/store/user/fengwang/QCDHLT/'
config.Data.publication = False
config.Site.storageSite = 'T2_CH_CERN'

from os import system

jobs=[#'REQUESTNAME','INPUTDATASET','PUBLISHDATANAME','FILESPERJOB','DBSURL','LUMIMASK'],
        ['hip0p6','/RelValQCD_FlatPt_15_3000HS_13/CMSSW_8_1_0_pre9-PU25ns_81X_mcRun2_asymptotic_v2_hip0p6-v1/GEN-SIM-RECO','hip0p6',5,'global',''],
        ['hip0p6_mtoff','/RelValQCD_FlatPt_15_3000HS_13/CMSSW_8_1_0_pre9-PU25ns_81X_mcRun2_asymptotic_v2_hip0p6_mtoff-v1/GEN-SIM-RECO','hip0p6_mtoff',5,'global',''],
        ['hip0p8','/RelValQCD_FlatPt_15_3000HS_13/CMSSW_8_1_0_pre9-PU25ns_81X_mcRun2_asymptotic_v2_hip0p8-v1/GEN-SIM-RECO','hip0p8',5,'global',''],
        ['hip0p8_mtoff','/RelValQCD_FlatPt_15_3000HS_13/CMSSW_8_1_0_pre9-PU25ns_81X_mcRun2_asymptotic_v2_hip0p8_mtoff-v1/GEN-SIM-RECO','hip0p8_mtoff',5,'global',''],
        ['hip1p2','/RelValQCD_FlatPt_15_3000HS_13/CMSSW_8_1_0_pre9-PU25ns_81X_mcRun2_asymptotic_v2_hip1p2-v1/GEN-SIM-RECO','hip1p2',5,'global',''],
        ['hip1p2_mtoff','/RelValQCD_FlatPt_15_3000HS_13/CMSSW_8_1_0_pre9-PU25ns_81X_mcRun2_asymptotic_v2_hip1p2_mtoff-v1/GEN-SIM-RECO','hip1p2_mtoff',5,'global',''],
    ]

MC = True
if MC:
    template='crabTemplateMC.py'
else:
    template='crabTemplateData.py'

#---------------------------------------------------------------------------------------------------------------------------------------------------

for job in jobs:
    requestName=job[0]
    inputDS=job[1]
    publishDN=job[2]
    Units=str(job[3])
    dbsUrl=job[4]
    Mask=job[5]
    
    config=requestName+'.py'
    system('cp '+template+' '+config)
    system('sed \"s%REQUESTNAME%'+requestName+'%g\" '+config+' --in-place')
    system('sed \"s%INPUTDATASET%'+inputDS+'%g\" '+config+' --in-place')
    system('sed \"s%PUBLISHDATANAME%'+publishDN+'%g\" '+config+' --in-place')
    if MC:
        system('sed \"s%FILESPERJOB%'+Units+'%g\" '+config+' --in-place')
    else:
        system('sed \"s%UNITSPERJOB%'+Units+'%g\" '+config+' --in-place')
        system('sed \"s%LUMIMASK%'+Mask+'%g\" '+config+' --in-place')
    system('sed \"s%DBSURL%'+dbsUrl+'%g\" '+config+' --in-place')

    system('crab submit '+config)

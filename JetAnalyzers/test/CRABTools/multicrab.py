#! /usr/bin/env python

import copy
import os
import time

from CRABAPI.RawCommand import crabCommand
from crab_config_data import config as config

config1 = copy.deepcopy(config)
config2 = copy.deepcopy(config)
del config

config1.Data.totalUnits = 100

tasks = {}

results = crabCommand('submit', config=config1)
request1 = results['uniquerequestname']

tasks[request1] = config1

config2.Data.inputDataset = '/DoubleMuParked/Run2012C-22Jan2013-v1/AOD'
config2.General.requestName = 'tutorial_Data_analysis_testC'
config2.Data.totalUnits = 100
del config2.Data.lumiMask
del config2.Data.runRange
print "Submitting with CRAB config"
print config2.pythonise_()

results = crabCommand('submit', config=config2)
request2 = results['uniquerequestname']

tasks[request2] = config2

while True:
    for request, config  in tasks.items():
        dirname = './%s/crab_%s' % (config.General.workArea, config.General.requestName)
        fulldir = os.path.abspath(dirname)
        try:
            results = crabCommand('status', dir=fulldir)
            print "For task", request, "the job states are", results['jobsPerStatus']
        except:
            pass
        time.sleep(20)

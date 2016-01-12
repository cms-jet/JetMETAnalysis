#! /usr/bin/env python

import copy
import os
import time
from CRABAPI.RawCommand import crabCommand

resubmitFailedJobs = False
 
'''
#from QCD_GEM2019Upg14DR_Age0CaloNoPU_hcal_DES19 import config as config
config = __import__("QCD_GEM2019Upg14DR_Age0CaloNoPU_hcal_DES19").config
print config
config1 = copy.deepcopy(config)
del config
tasks={}
tasks["QCD_GEM2019Upg14DR-Age0CaloNoPU_hcal_DES19"] = config1
for request, config  in tasks.items():
    dirname = './crab_%s' % (config.General.requestName)
    fulldir = os.path.abspath(dirname)
    try:
        results = crabCommand('status', dir=fulldir)
        print "For task", request, "the job states are", results['jobsPerStatus']
    except:
        pass
'''

modules = ["QCD_GEM2019Upg14DR-Age0CaloNoPU_hcal_DES19.py","QCD_GEM2019Upg14DR-Age0CaloNoPU_hcal_DES19_nonRec2Rec.py","QCD_GEM2019Upg14DR-Age0CaloPU50_hcal_DES19.py","QCD_GEM2019Upg14DR-Age0CaloPU50_hcal_DES19_nonRec2Rec.py","QCD_GEM2019Upg14DR-Age100CaloNoPU_hcal_U19_100FB.py","QCD_GEM2019Upg14DR-Age100CaloNoPU_hcal_U19_100FB_nonRec2Rec.py","QCD_GEM2019Upg14DR-Age100CaloPU50_hcal_U19_100FB.py","QCD_GEM2019Upg14DR-Age100CaloPU50_hcal_U19_100FB_nonRec2Rec.py","QCD_GEM2019Upg14DR-Age150CaloNoHENoPU_hcal_U19_150FB.py","QCD_GEM2019Upg14DR-Age150CaloNoHENoPU_hcal_U19_150FB_nonRec2Rec.py","QCD_GEM2019Upg14DR-Age150CaloNoHEPU50_hcal_U19_150FB.py","QCD_GEM2019Upg14DR-Age150CaloNoHEPU50_hcal_U19_150FB_nonRec2Rec.py","QCD_GEM2019Upg14DR-Age150CaloNoPU_hcal_U19_150FB.py","QCD_GEM2019Upg14DR-Age150CaloNoPU_hcal_U19_150FB_nonRec2Rec.py","QCD_GEM2019Upg14DR-Age150CaloPU50_hcal_U19_150FB.py","QCD_GEM2019Upg14DR-Age150CaloPU50_hcal_U19_150FB_nonRec2Rec.py","QCD_GEM2019Upg14DR-Age300CaloNoHENoPU_hcal_U19_300FB.py","QCD_GEM2019Upg14DR-Age300CaloNoHENoPU_hcal_U19_300FB_nonRec2Rec.py","QCD_GEM2019Upg14DR-Age300CaloNoHEPU50_hcal_U19_300FB.py","QCD_GEM2019Upg14DR-Age300CaloNoHEPU50_hcal_U19_300FB_nonRec2Rec.py","QCD_GEM2019Upg14DR-Age300CaloNoPU_hcal_U19_300FB.py","QCD_GEM2019Upg14DR-Age300CaloNoPU_hcal_U19_300FB_nonRec2Rec.py","QCD_GEM2019Upg14DR-Age300CaloPU50_hcal_U19_300FB.py","QCD_GEM2019Upg14DR-Age300CaloPU50_hcal_U19_300FB_nonRec2Rec.py","QCD_GEM2019Upg14DR-Age500CaloNoHENoPU_hcal_U19_500FB.py","QCD_GEM2019Upg14DR-Age500CaloNoHENoPU_hcal_U19_500FB_nonRec2Rec.py","QCD_GEM2019Upg14DR-Age500CaloNoHEPU50_hcal_U19_500FB.py","QCD_GEM2019Upg14DR-Age500CaloNoHEPU50_hcal_U19_500FB_nonRec2Rec.py","QCD_GEM2019Upg14DR-Age500CaloNoPU_hcal_U19_500FB.py","QCD_GEM2019Upg14DR-Age500CaloNoPU_hcal_U19_500FB_nonRec2Rec.py","QCD_GEM2019Upg14DR-Age500CaloPU50_hcal_U19_500FB.py","QCD_GEM2019Upg14DR-Age500CaloPU50_hcal_U19_500FB_nonRec2Rec.py","QCD_Upg2017Summer15DR-Age100CaloNoPU_U17_100FB.py","QCD_Upg2017Summer15DR-Age100CaloNoPU_U17_100FB_nonRec2Rec.py","QCD_Upg2017Summer15DR-Age100CaloPU50_U17_100FB.py","QCD_Upg2017Summer15DR-Age100CaloPU50_U17_100FB_nonRec2Rec.py","QCD_Upg2017Summer15DR-Age150CaloNoPU_U17_150FB.py","QCD_Upg2017Summer15DR-Age150CaloNoPU_U17_150FB_nonRec2Rec.py","QCD_Upg2017Summer15DR-Age150CaloPU50_U17_150FB.py","QCD_Upg2017Summer15DR-Age150CaloPU50_U17_150FB_nonRec2Rec.py","QCD_Upg2017Summer15DR-NoAgingNoPU_DES17.py","QCD_Upg2017Summer15DR-NoAgingNoPU_DES17_nonRec2Rec.py","QCD_Upg2017Summer15DR-NoAgingPU50_DES17.py","QCD_Upg2017Summer15DR-NoAgingPU50_DES17_nonRec2Rec.py"]
#modules = ["QCD_GEM2019Upg14DR-Age0CaloNoPU_hcal_DES19.py","QCD_GEM2019Upg14DR-Age0CaloNoPU_hcal_DES19_nonRec2Rec.py","QCD_Upg2017Summer15DR-Age150CaloNoPU_U17_150FB_nonRec2Rec.py"]
moduleNames = [x.strip('.py') for x in modules]
configs = []
for imodule in moduleNames:
    config = __import__(imodule).config
    configs.append(copy.deepcopy(config))
    del config

list_results = []
set_status_headers = set()
for config in configs:
    dirname = './crab_%s' % (config.General.requestName)
    fulldir = os.path.abspath(dirname)
    try:
        results = crabCommand('status', dir=fulldir)
        string_result = "For task", config.General.requestName, "the job states are", results['jobsPerStatus']
        set_status_headers.update(results['jobsPerStatus'].keys())
        list_results.append(string_result)

        if results['jobsPerStatus']['failed']!=0 and resubmitFailedJobs==True:
            print "****************************"
            print "* Resubmitting set of jobs *"
            print "****************************"
            crabCommand('resubmit', dir=fulldir)
    except:
        pass

print '\n\n\n'

list_status_headers = list(set_status_headers)
list_status_headers.append("Total Jobs")

row_format = "{0:>65}"
for i in range(1,len(list_status_headers)+1):
    row_format += "{"+str(i)+":>15}"
print row_format.format("Request Name", *list_status_headers)
for iresult in list_results:
    status_values = []
    for header in list_status_headers:
        if header in iresult[3]:
            status_values.append(iresult[3][header])
        elif header == "Total Jobs":
            pass
        else:
            status_values.append(0)
    status_values.append(sum(iresult[3].values()))
    print row_format.format(iresult[1], *status_values)

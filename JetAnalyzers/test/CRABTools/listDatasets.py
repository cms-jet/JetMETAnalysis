#! /usr/bin/env python

import copy, os, time, subprocess, shlex, sys
from CRABAPI.RawCommand import crabCommand

# update_progress() : Displays or updates a console progress bar
## Accepts a float between 0 and 1. Any int will be converted to a float.
## A value under 0 represents a 'halt'.
## A value at 1 or bigger represents 100%
def update_progress(progress):
    barLength = 10 # Modify this to change the length of the progress bar
    status = ""
    if isinstance(progress, int):
        progress = float(progress)
    if not isinstance(progress, float):
        progress = 0
        status = "error: progress var must be float\r\n"
    if progress < 0:
        progress = 0
        status = "Halt...\r\n"
    if progress >= 1:
        progress = 1
        status = "Done...\r\n"
    block = int(round(barLength*progress))
    text = "\rPercent: [{0}] {1}% {2}".format( "#"*block + "-"*(barLength-block), progress*100, status)
    sys.stdout.write(text)
    sys.stdout.flush()

modules = ["QCD_GEM2019Upg14DR-Age0CaloNoPU_hcal_DES19.py","QCD_GEM2019Upg14DR-Age0CaloNoPU_hcal_DES19_nonRec2Rec.py","QCD_GEM2019Upg14DR-Age0CaloPU50_hcal_DES19.py","QCD_GEM2019Upg14DR-Age0CaloPU50_hcal_DES19_nonRec2Rec.py","QCD_GEM2019Upg14DR-Age100CaloNoPU_hcal_U19_100FB.py","QCD_GEM2019Upg14DR-Age100CaloNoPU_hcal_U19_100FB_nonRec2Rec.py","QCD_GEM2019Upg14DR-Age100CaloPU50_hcal_U19_100FB.py","QCD_GEM2019Upg14DR-Age100CaloPU50_hcal_U19_100FB_nonRec2Rec.py","QCD_GEM2019Upg14DR-Age150CaloNoHENoPU_hcal_U19_150FB.py","QCD_GEM2019Upg14DR-Age150CaloNoHENoPU_hcal_U19_150FB_nonRec2Rec.py","QCD_GEM2019Upg14DR-Age150CaloNoHEPU50_hcal_U19_150FB.py","QCD_GEM2019Upg14DR-Age150CaloNoHEPU50_hcal_U19_150FB_nonRec2Rec.py","QCD_GEM2019Upg14DR-Age150CaloNoPU_hcal_U19_150FB.py","QCD_GEM2019Upg14DR-Age150CaloNoPU_hcal_U19_150FB_nonRec2Rec.py","QCD_GEM2019Upg14DR-Age150CaloPU50_hcal_U19_150FB.py","QCD_GEM2019Upg14DR-Age150CaloPU50_hcal_U19_150FB_nonRec2Rec.py","QCD_GEM2019Upg14DR-Age300CaloNoHENoPU_hcal_U19_300FB.py","QCD_GEM2019Upg14DR-Age300CaloNoHENoPU_hcal_U19_300FB_nonRec2Rec.py","QCD_GEM2019Upg14DR-Age300CaloNoHEPU50_hcal_U19_300FB.py","QCD_GEM2019Upg14DR-Age300CaloNoHEPU50_hcal_U19_300FB_nonRec2Rec.py","QCD_GEM2019Upg14DR-Age300CaloNoPU_hcal_U19_300FB.py","QCD_GEM2019Upg14DR-Age300CaloNoPU_hcal_U19_300FB_nonRec2Rec.py","QCD_GEM2019Upg14DR-Age300CaloPU50_hcal_U19_300FB.py","QCD_GEM2019Upg14DR-Age300CaloPU50_hcal_U19_300FB_nonRec2Rec.py","QCD_GEM2019Upg14DR-Age500CaloNoHENoPU_hcal_U19_500FB.py","QCD_GEM2019Upg14DR-Age500CaloNoHENoPU_hcal_U19_500FB_nonRec2Rec.py","QCD_GEM2019Upg14DR-Age500CaloNoHEPU50_hcal_U19_500FB.py","QCD_GEM2019Upg14DR-Age500CaloNoHEPU50_hcal_U19_500FB_nonRec2Rec.py","QCD_GEM2019Upg14DR-Age500CaloNoPU_hcal_U19_500FB.py","QCD_GEM2019Upg14DR-Age500CaloNoPU_hcal_U19_500FB_nonRec2Rec.py","QCD_GEM2019Upg14DR-Age500CaloPU50_hcal_U19_500FB.py","QCD_GEM2019Upg14DR-Age500CaloPU50_hcal_U19_500FB_nonRec2Rec.py","QCD_Upg2017Summer15DR-Age100CaloNoPU_U17_100FB.py","QCD_Upg2017Summer15DR-Age100CaloNoPU_U17_100FB_nonRec2Rec.py","QCD_Upg2017Summer15DR-Age100CaloPU50_U17_100FB.py","QCD_Upg2017Summer15DR-Age100CaloPU50_U17_100FB_nonRec2Rec.py","QCD_Upg2017Summer15DR-Age150CaloNoPU_U17_150FB.py","QCD_Upg2017Summer15DR-Age150CaloNoPU_U17_150FB_nonRec2Rec.py","QCD_Upg2017Summer15DR-Age150CaloPU50_U17_150FB.py","QCD_Upg2017Summer15DR-Age150CaloPU50_U17_150FB_nonRec2Rec.py","QCD_Upg2017Summer15DR-NoAgingNoPU_DES17.py","QCD_Upg2017Summer15DR-NoAgingNoPU_DES17_nonRec2Rec.py","QCD_Upg2017Summer15DR-NoAgingPU50_DES17.py","QCD_Upg2017Summer15DR-NoAgingPU50_DES17_nonRec2Rec.py"]
moduleNames = [x.strip('.py') for x in modules]
configs = []
for imodule in moduleNames:
    config = __import__(imodule).config
    configs.append(copy.deepcopy(config))
    del config

datasets = {}
for count, config in enumerate(configs):
    update_progress(float(count)/len(configs))
    try:
        command_line = '/uscms/home/aperloff/Scripts/das_cli.py --query "dataset='+config.Data.inputDataset+' | grep dataset.nevents"'
        args = shlex.split(command_line)
        p = subprocess.Popen(args,stdout=subprocess.PIPE)
        out, err = p.communicate()
        out = out.splitlines()
        datasets[config.General.requestName] = {"Dataset Name":config.Data.inputDataset,"Number of Events":out[3]}

    except:
        pass
print ""


row_format = "{0:<65}{1:<120}{2:<15}"
print row_format.format("Request Name",*datasets[datasets.keys()[0]].keys())
for key,val in datasets.iteritems():
    print row_format.format(key,*val.values())

#python ~/Scripts/das_cli.py --query "dataset=/QCD_Pt-15to3000_Tune4C_14TeV_pythia8/Upg2017Summer15DR-Age100CaloPU50_U17_100FB_V1A-v1/GEN-SIM-RECO | grep dataset.name, dataset.nevents"

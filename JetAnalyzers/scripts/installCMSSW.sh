#!/bin/bash
export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch                                        
export SCRAM_ARCH=slc6_amd64_gcc493                                            
export CMSSW_GIT_REFERENCE=/cvmfs/cms.cern.ch/cmssw.git/                       
source $VO_CMS_SW_DIR/cmsset_default.sh 

cmsrel CMSSW_8_0_1
cd CMSSW_8_0_1/src/
cmsenv
git-cms-init

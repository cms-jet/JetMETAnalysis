
import os
import re
import subprocess


#--------------------------------------------------------------------------------
#
# executable
#
execDir = "%s/bin/%s/" % (os.environ['CMSSW_BASE'], os.environ['SCRAM_ARCH'])
executable_jrAnalyzer     = execDir + 'jet_response_analyzer_x'
executable_fitResponse    = execDir + 'jet_response_fitter_x'
executable_fitL2param     = execDir + 'jet_l2_correction_x' # using option here to process L3 correction in one go!!
executable_applyL2L3param = execDir + 'jet_apply_jec_x'
executable_fitResolution  = execDir + 'jet_response_and_resolution_x'
executable_showGraphs     = execDir + 'jet_inspect_graphs_x'
executable_showHistos     = execDir + 'jet_inspect_histos_x'
#executable_showProfiles   = execDir + 'jet_inspect_profiles_x'
executable_hadd           = 'hadd -f -k' # -f(target != source), -k(skip corrupt files)
executable_haddFitHists   = 'hadd -f' 
executable_shell          = '/bin/csh'
executable_python         = 'python'
#--------------------------------------------------------------------------------

toCal = [
  'uncalibrated', 
  #'calibrated',
]


#--------------------------------------------------------------------------------
# correction file directory
#
jecTextFilePath = os.getcwd()
#--------------------------------------------------------------------------------


#--------------------------------------------------------------------------------
#
# define function used for fitting tau-jet response:
#   0 = Gaussian
#   1 = Crystal-Ball function
#   2 = Calpas Veelken CrystalBall 
#
fitOption = 2
#--------------------------------------------------------------------------------



ptBinning = [ 15., 20., 22.5, 25., 27.5, 30., 35., 40., 
              45., 50., 60., 80., 120., 200., 500., 3000. ] #15

etaBinning = [ -2.3, -2.1, -1.9, -1.7, -1.5, -1.3, 
	       -1.1, -0.9, -0.7, -0.5, -0.3, -0.1,
    	       +0.1, +0.3, +0.5, +0.7, +0.9, +1.1, 
	       +1.3, +1.5, +1.7, +1.9, +2.1, +2.3 ] #23


version = 'v1_2enRecoveryCBa'

era = 'TauJec11V1'
    

inputFilePaths = [
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/GluGluHToTauTauM125/',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/VBFHToTauTauM125',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/DYJetsToLLM50',
  #
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/ZprimeToTauTauM500',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/ZprimeToTauTauM1000', 
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/ZprimeToTauTauM1500',
  #'/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/ZprimeToTauTauM2000', # all jobs failed!!
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/ZprimeToTauTauM2500',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/ZprimeToTauTauM3500',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/ZprimeToTauTauM4000',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/ZprimeToTauTauM4500', 
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/ZprimeToTauTauM5000',
  #
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/WprimeToTauNuM1000',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/WprimeToTauNuM1200',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/WprimeToTauNuM1400',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/WprimeToTauNuM1600',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/WprimeToTauNuM1800',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/WprimeToTauNuM2000',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/WprimeToTauNuM2200',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/WprimeToTauNuM2400',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/WprimeToTauNuM2600',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/WprimeToTauNuM2800',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/WprimeToTauNuM3200',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/WprimeToTauNuM3400',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/WprimeToTauNuM3600',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/WprimeToTauNuM3800',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/WprimeToTauNuM4000',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/WprimeToTauNuM4200',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/WprimeToTauNuM4400',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/WprimeToTauNuM4600',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/WprimeToTauNuM4800',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/WprimeToTauNuM5000',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/WprimeToTauNuM5200',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/WprimeToTauNuM5400',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/WprimeToTauNuM5600',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/WprimeToTauNuM5800',
  #
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToHToTauTauM80',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToHToTauTauM100',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToHToTauTauM110',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToHToTauTauM120',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToHToTauTauM130',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToHToTauTauM140',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToHToTauTauM160',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToHToTauTauM180',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToHToTauTauM200',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToHToTauTauM250',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToHToTauTauM300',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToHToTauTauM350',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToHToTauTauM400',
  #'/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToHToTauTauM450', #crab:  WARNING: No sites are hosting any part of data for block:
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToHToTauTauM500',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToHToTauTauM800',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToHToTauTauM900',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToHToTauTauM1000',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToHToTauTauM1200',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToHToTauTauM1500',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToHToTauTauM1600',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToHToTauTauM1800',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToHToTauTauM2000',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToHToTauTauM2300',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToHToTauTauM2600',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToHToTauTauM2900',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToHToTauTauM3200',
  #
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToBBHToTauTauM80',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToBBHToTauTauM90',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToBBHToTauTauM120',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToBBHToTauTauM130',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToBBHToTauTauM140',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToBBHToTauTauM180',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToBBHToTauTauM200',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToBBHToTauTauM250',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToBBHToTauTauM300',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToBBHToTauTauM350',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToBBHToTauTauM400',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToBBHToTauTauM450',
  #'/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToBBHToTauTauM500', # job failed!!
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToBBHToTauTauM700',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToBBHToTauTauM800',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToBBHToTauTauM900',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToBBHToTauTauM1000',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToBBHToTauTauM1200',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToBBHToTauTauM1400',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToBBHToTauTauM1500',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToBBHToTauTauM1800',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToBBHToTauTauM2300',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToBBHToTauTauM2600',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToBBHToTauTauM2900',
  '/hdfs/cms/store/user/calpas/JRAtauNtuple/v4/SUSYGluGluToBBHToTauTauM3200',
]


#--------------------------------------------------------------------------------
#
# build shell script for running 'hadd' in order to "harvest" histograms
#
ntupleFileNames = []
for inputFilePath in inputFilePaths:
    ntupleFileNames.extend([ os.path.join(inputFilePath, file) for file in os.listdir(inputFilePath) ])

ntupleFile_regex = r"[a-zA-Z0-9_/:.]*ntupleJRAtau_(?P<sample>[a-zA-Z0-9]*)_(?P<gridJob>\d*)_(?P<gridTry>\d*)_(?P<gridId>[a-zA-Z0-9]*).root"
ntupleFile_matcher = re.compile(ntupleFile_regex)
#--------------------------------------------------------------------------------


samplesToAnalyze = [
  'VBFHToTauTauM125',
  'GluGluHToTauTauM125',
  'DYJetsToLLM50',
  'ZprimeToTauTauM500',
  'ZprimeToTauTauM1000',
  'ZprimeToTauTauM1500',
  #'ZprimeToTauTauM2000',
  'ZprimeToTauTauM2500',
  'ZprimeToTauTauM3500',
  'ZprimeToTauTauM4000',
  'ZprimeToTauTauM4500',
  'ZprimeToTauTauM5000',
  'WprimeToTauNuM1000',
  'WprimeToTauNuM1200',
  'WprimeToTauNuM1400',
  'WprimeToTauNuM1600',
  'WprimeToTauNuM1800',
  'WprimeToTauNuM2000',
  'WprimeToTauNuM2200',
  'WprimeToTauNuM2400',
  'WprimeToTauNuM2600',
  'WprimeToTauNuM2800',
  'WprimeToTauNuM3200',
  'WprimeToTauNuM3400',
  'WprimeToTauNuM3600',
  'WprimeToTauNuM3800',
  'WprimeToTauNuM4000',
  'WprimeToTauNuM4200',
  'WprimeToTauNuM4400',
  'WprimeToTauNuM4600',
  'WprimeToTauNuM4800',
  'WprimeToTauNuM5000',
  'WprimeToTauNuM5200',
  'WprimeToTauNuM5400',
  'WprimeToTauNuM5600',
  'WprimeToTauNuM5800',
  #
  'SUSYGluGluToHToTauTauM80',
  'SUSYGluGluToHToTauTauM100',
  'SUSYGluGluToHToTauTauM110',
  'SUSYGluGluToHToTauTauM120',
  'SUSYGluGluToHToTauTauM130',
  'SUSYGluGluToHToTauTauM140',
  'SUSYGluGluToHToTauTauM160',
  'SUSYGluGluToHToTauTauM180',
  'SUSYGluGluToHToTauTauM200',
  'SUSYGluGluToHToTauTauM250',
  'SUSYGluGluToHToTauTauM300',
  'SUSYGluGluToHToTauTauM350',
  'SUSYGluGluToHToTauTauM400',
  #'SUSYGluGluToHToTauTauM450',
  'SUSYGluGluToHToTauTauM500',
  'SUSYGluGluToHToTauTauM800',
  'SUSYGluGluToHToTauTauM900',
  'SUSYGluGluToHToTauTauM1000',
  'SUSYGluGluToHToTauTauM1200',
  'SUSYGluGluToHToTauTauM1500',
  'SUSYGluGluToHToTauTauM1600',
  'SUSYGluGluToHToTauTauM1800',
  'SUSYGluGluToHToTauTauM2000',
  'SUSYGluGluToHToTauTauM2300',
  'SUSYGluGluToHToTauTauM2600',
  'SUSYGluGluToHToTauTauM2900',
  'SUSYGluGluToHToTauTauM3200',
  #
  'SUSYGluGluToBBHToTauTauM80',
  'SUSYGluGluToBBHToTauTauM90',
  'SUSYGluGluToBBHToTauTauM120',
  'SUSYGluGluToBBHToTauTauM130',
  'SUSYGluGluToBBHToTauTauM140',
  'SUSYGluGluToBBHToTauTauM180',
  'SUSYGluGluToBBHToTauTauM200',
  'SUSYGluGluToBBHToTauTauM250',
  'SUSYGluGluToBBHToTauTauM300',
  'SUSYGluGluToBBHToTauTauM350',
  'SUSYGluGluToBBHToTauTauM400',
  'SUSYGluGluToBBHToTauTauM450',
  #'SUSYGluGluToBBHToTauTauM500',
  'SUSYGluGluToBBHToTauTauM700',
  'SUSYGluGluToBBHToTauTauM800',
  'SUSYGluGluToBBHToTauTauM900',
  'SUSYGluGluToBBHToTauTauM1000',
  'SUSYGluGluToBBHToTauTauM1200',
  'SUSYGluGluToBBHToTauTauM1400',
  'SUSYGluGluToBBHToTauTauM1500',
  'SUSYGluGluToBBHToTauTauM1800',
  'SUSYGluGluToBBHToTauTauM2300',
  'SUSYGluGluToBBHToTauTauM2600',
  'SUSYGluGluToBBHToTauTauM2900',
  'SUSYGluGluToBBHToTauTauM3200',
]



# for time reason rn parallel jobs for different eta for the crystalball fit
histNames = [
	     "RelRsp_JetEta-2.3to-2.1_RefPt", 
	     "RelRsp_JetEta-2.1to-1.9_RefPt", 
	     "RelRsp_JetEta-1.9to-1.7_RefPt", 
	     "RelRsp_JetEta-1.7to-1.5_RefPt", 
	     "RelRsp_JetEta-1.5to-1.3_RefPt", 
	     "RelRsp_JetEta-1.3to-1.1_RefPt", 
	     "RelRsp_JetEta-1.1to-0.9_RefPt", 
	     "RelRsp_JetEta-0.9to-0.7_RefPt", 
	     "RelRsp_JetEta-0.7to-0.5_RefPt", 
	     "RelRsp_JetEta-0.5to-0.3_RefPt", 
	     "RelRsp_JetEta-0.3to-0.1_RefPt", 
	     "RelRsp_JetEta-0.1to0.1_RefPt", 
	     "RelRsp_JetEta0.1to0.3_RefPt", 
	     "RelRsp_JetEta0.3to0.5_RefPt", 
	     "RelRsp_JetEta0.5to0.7_RefPt", 
	     "RelRsp_JetEta0.7to0.9_RefPt", 
	     "RelRsp_JetEta0.9to1.1_RefPt", 
	     "RelRsp_JetEta1.1to1.3_RefPt", 
	     "RelRsp_JetEta1.3to1.5_RefPt", 
	     "RelRsp_JetEta1.5to1.7_RefPt", 
	     "RelRsp_JetEta1.7to1.9_RefPt", 
	     "RelRsp_JetEta1.9to2.1_RefPt", 
	     "RelRsp_JetEta2.1to2.3_RefPt", 
	     ]



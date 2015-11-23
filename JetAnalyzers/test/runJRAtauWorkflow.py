#!/usr/bin/env python

import os
import re

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

outputFilePath = '%s/src/JetMETAnalysis/JetAnalyzers/test/JRAtau/%s' % (os.environ['CMSSW_BASE'], version)


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

algorithms = [
  'ak5tauHPSlooseCombDBcorrAll',
  'ak5tauHPSlooseCombDBcorrOneProng0Pi0',
  'ak5tauHPSlooseCombDBcorrOneProng1Pi0',
  #'ak5tauHPSlooseCombDBcorrOneProng2Pi0', # as we merged 1Prong1 and 1Prong2 in addAlgorithm.py they are equivalent!!
  'ak5tauHPSlooseCombDBcorrTwoProng0Pi0',
  'ak5tauHPSlooseCombDBcorrTwoProng1Pi0',
  'ak5tauHPSlooseCombDBcorrThreeProng0Pi0',
  'ak5tauHPSlooseCombDBcorrThreeProng1Pi0',
]

toCal = ["uncalibrated", "calibrated"]
cuts = ["pass", "failed"]

for cal in toCal:
     for alg in algorithms:
         for cut in cuts:
	      outputPlotPath = '%s/src/JetMETAnalysis/JetAnalyzers/test/fitPlots/%s/%s/%s' % (os.environ['CMSSW_BASE'], cal, alg, cut )
	      if not os.path.exists(outputPlotPath):
    	           os.makedirs(outputPlotPath)
	            	 	
execDir = "%s/bin/%s/" % (os.environ['CMSSW_BASE'], os.environ['SCRAM_ARCH'])

executable_jrAnalyzer     = execDir + 'jet_response_analyzer_x'
executable_fitResponse    = execDir + 'jet_response_fitter_x'
#executable_fitL3param     = execDir + 'jet_l3_correction_x'
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

# define function used for fitting tau-jet response:
#   0 = Gaussian
#   1 = Crystall-Ball function
#   2 = multi-fit (pol, ex, gaus) function 
fitOption = 2

jecTextFilePath = os.getcwd()

if not os.path.exists("JRAtau"):
    os.makedirs(outputFilePath)

outputFilePath_plots = os.path.join(outputFilePath, "plots")
if not os.path.exists(outputFilePath_plots):
    os.mkdir(outputFilePath_plots)

#--------------------------------------------------------------------------------
#
# build shell script for running 'hadd' in order to "harvest" histograms
# produced by FWLiteZllRecoilCorrectionAnalyzer macro
#
fileNames_hadd = {}

ntupleFileNames = []
for inputFilePath in inputFilePaths:
    ntupleFileNames.extend([ os.path.join(inputFilePath, file) for file in os.listdir(inputFilePath) ])

ntupleFile_regex = r"[a-zA-Z0-9_/:.]*ntupleJRAtau_(?P<sample>[a-zA-Z0-9]*)_(?P<gridJob>\d*)_(?P<gridTry>\d*)_(?P<gridId>[a-zA-Z0-9]*).root"
ntupleFile_matcher = re.compile(ntupleFile_regex)

####################
def buildConfigFile_hadd(haddCommand, shellFileName_full, inputFileNames, outputFileName_full):

    """Build shell script to run 'hadd' command in order to add all histograms
       in files specified by inputFileNames argument and write the sum to file outputFileName"""

    shellFile = open(shellFileName_full, "w")
    shellFile.write("#!/bin/csh -f\n")
    shellFile.write("\n")
    # CV: delete output file in case it exists 
    shellFile.write("rm -f %s\n" % outputFileName_full)
    shellFile.write("\n")
    haddCommandLine = "%s %s" % (haddCommand, outputFileName_full)
    for inputFileName in inputFileNames:
        haddCommandLine += " %s" % inputFileName
    shellFile.write("%s\n" % haddCommandLine)
    shellFile.close()

    logFileName_full = shellFileName_full.replace('.csh', '.log')

    retVal = {}
    retVal['shellFileName']  = shellFileName_full
    retVal['outputFileName'] = outputFileName_full
    retVal['logFileName']    = logFileName_full

    return retVal
####################

# add all samples files
haddInputFileNames = []
for sampleToAnalyze in samplesToAnalyze:
    nbOfSample = []
    for ntupleFileName in ntupleFileNames:
        if ntupleFile_matcher.match(ntupleFileName) and \
          ntupleFile_matcher.match(ntupleFileName).group('sample') == sampleToAnalyze:
            haddInputFileNames.append(ntupleFileName)
	    nbOfSample.append(ntupleFileName)
    print "sample = %s: found %i input files." % (sampleToAnalyze, len(nbOfSample))

print "found %i input files." % (len(haddInputFileNames))

haddShellFileName  = os.path.join(outputFilePath, 'harvestJRAtauNtuples.csh')
#haddOutputFileName = os.path.join(inputFilePath, 'ntupleJRAtau_all.root')
haddOutputFileName = os.path.join(outputFilePath, 'ntupleJRAtau_all.root') # copy where you can

retVal_hadd = buildConfigFile_hadd(executable_hadd, haddShellFileName, haddInputFileNames, haddOutputFileName)

fileNames_hadd = {}
fileNames_hadd['shellFileName']  = haddShellFileName
fileNames_hadd['inputFileNames'] = haddInputFileNames
fileNames_hadd['outputFileName'] = haddOutputFileName
fileNames_hadd['logFileName']    = retVal_hadd['logFileName']

#--------------------------------------------------------------------------------

def make_MakeFile_vstring(list_of_strings):
    retVal = ""
    for i, string_i in enumerate(list_of_strings):
        if i > 0:
            retVal += " "
        retVal += string_i
    return retVal

#--------------------------------------------------------------------------------
#
# initialize command-line parameters for analyzing "plain" ROOT Ntuples for uncalibrated tau-jets
# and filling tau-jet response and resolution histograms
#

ptBinning = [ 15., 20., 22.5, 25., 27.5, 30., 35., 40., 45., 50., 60., 80., 120., 200., 
    500., 3000. ] 

etaBinning = [ -2.3, -2.1, -1.9, -1.7, -1.5, -1.3, -1.1, -0.9, -0.7, -0.5, -0.3, -0.1,
    +0.1, +0.3, +0.5, +0.7, +0.9, +1.1, +1.3, +1.5, +1.7, +1.9, +2.1, +2.3 ] 
 
# run fit in parallel for different eta slides 
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
    
def make_MakeFile_vdouble(list_of_doubles):
    retVal = ""
    for i, double_i in enumerate(list_of_doubles):
        if i > 0:
            retVal += " "
        retVal += "%2.1f" % double_i
    return retVal

def make_jrAnalyzer_config(configFileName):
    configFile = open(configFileName, "w")
    configFile.write("drmax = 0.3\n")
    configFile.write("etabarrelmin = -1.3\n")
    configFile.write("etabarrelmax =  1.3\n")
    configFile.write("binspt = %s\n" % make_MakeFile_vdouble(ptBinning))
    configFile.write("binseta = %s\n" % make_MakeFile_vdouble(etaBinning))
    configFile.close()

#--------------------------------------------------------------------------------
#
# initialize command-line parameters for the jet response for uncalibrated tau-jets
#
fileNames_and_options_jrAnalyzer = {}    
fileNames_and_options_jrAnalyzer['inputFileNames'] = [fileNames_hadd['outputFileName']]
fileNames_and_options_jrAnalyzer['outputFileName'] = os.path.join(outputFilePath, "histogramsJRAtau.root")
fileNames_and_options_jrAnalyzer['configFileName'] = os.path.join(outputFilePath, "jet_response_analyzer.cfg")
make_jrAnalyzer_config(fileNames_and_options_jrAnalyzer['configFileName'])
fileNames_and_options_jrAnalyzer['logFileName']    = os.path.join(outputFilePath, "jet_response_analyzer.log")
fileNames_and_options_jrAnalyzer['commandLine']    = '%s -input %s -output %s -algs %s -nbinsrelrsp 250' % \
    (fileNames_and_options_jrAnalyzer['configFileName'],
     make_MakeFile_vstring(fileNames_and_options_jrAnalyzer['inputFileNames']),
     fileNames_and_options_jrAnalyzer['outputFileName'],
     "".join([ "%s:0.3 " % algorithm for algorithm in algorithms ]))
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# initialize command-line parameters for fitting jet response for uncalibrated tau-jets
#
fileNames_and_options_fitResponse_uncalibrated = {} 
haddFitHists = []
for histName in histNames:
	fileNames_and_options_fitResponse_uncalibrated[histName] = {} 
	fileNames_and_options_fitResponse_uncalibrated[histName]['inputFileNames'] = [fileNames_and_options_jrAnalyzer['outputFileName']]
	fileNames_and_options_fitResponse_uncalibrated[histName]['outputFileName'] = os.path.join(outputFilePath, "responseJRAtau_%s.root" % (histName))
	haddFitHists.append(fileNames_and_options_fitResponse_uncalibrated[histName]['outputFileName']) 
	fileNames_and_options_fitResponse_uncalibrated[histName]['logFileName']    = os.path.join(outputFilePath, "jet_response_fitter_%s.log" %histName)
	fileNames_and_options_fitResponse_uncalibrated[histName]['commandLine']    = '-input %s -output %s -algs %s -fittype %i -histName %s -calibrated false' % \
    	(make_MakeFile_vstring(fileNames_and_options_fitResponse_uncalibrated[histName]['inputFileNames']),
     	fileNames_and_options_fitResponse_uncalibrated[histName]['outputFileName'],
     	make_MakeFile_vstring(algorithms),
     	fitOption,
     	histName
     	)

# add the fit histograms
haddShellFitHistName  = os.path.join(outputFilePath, 'harvestFitHist.csh')
haddOutputFitHistName = os.path.join(outputFilePath, 'fitHist_all.root') 

retFitHist_hadd = buildConfigFile_hadd(executable_haddFitHists, haddShellFitHistName, haddFitHists, haddOutputFitHistName)

fitHists_hadd_uncalibrated = {}
fitHists_hadd_uncalibrated['shellFileName']  = haddShellFitHistName
fitHists_hadd_uncalibrated['inputFileNames'] = haddFitHists
fitHists_hadd_uncalibrated['outputFileName'] = haddOutputFitHistName
fitHists_hadd_uncalibrated['logFileName']    = retFitHist_hadd['logFileName']


#--------------------------------------------------------------------------------
#
# initialize command-line parameters for determining L2 and L3 correction parameters
# l2l3 is derived in on step by using -l2l3 true option in jet_l2_correction_x
#
fileNames_and_options_fitL2param = {}
fileNames_and_options_fitL2param['inputFileNames'] = [fitHists_hadd_uncalibrated['outputFileName']]
fileNames_and_options_fitL2param['outputFileName'] = os.path.join(outputFilePath, "fitL2param.root")
fileNames_and_options_fitL2param['logFileName']    = os.path.join(outputFilePath, "fitL2param.log")
fileNames_and_options_fitL2param['commandLine']    = \
  '-input %s -output %s -era %s -algs %s -formats png -batch true -l2l3 true -corr false -mpv true' % \
    (make_MakeFile_vstring(fileNames_and_options_fitL2param['inputFileNames']),
     fileNames_and_options_fitL2param['outputFileName'],
     era,
     make_MakeFile_vstring(algorithms))
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# initialize command-line parameters for applying L2/L3 correction parameters
# and producing new "plain" ROOT Ntuples for calibrated tau-jets
# l2l3 derived in 1 step: -levels 2
#
fileNames_and_options_applyL2L3param = {}
fileNames_and_options_applyL2L3param['inputFileNames'] = [ fileNames_and_options_jrAnalyzer['outputFileName'],
							   fileNames_and_options_fitL2param['outputFileName'] ]
fileNames_and_options_applyL2L3param['outputFileName'] = os.path.join(outputFilePath, "applyL2L3param.root")
fileNames_and_options_applyL2L3param['logFileName']    = os.path.join(outputFilePath, "applyL2L3param.log")
fileNames_and_options_applyL2L3param['commandLine']    = '-input %s -output %s -era %s -algs %s -jecpath %s -levels 2' % \
    (fileNames_hadd['outputFileName'],
     fileNames_and_options_applyL2L3param['outputFileName'],
     era,
     make_MakeFile_vstring(algorithms),
     jecTextFilePath)
#--------------------------------------------------------------------------------    

#--------------------------------------------------------------------------------
#
# initialize command-line parameters for analyzing "plain" ROOT Ntuples for calibrated tau-jets
# and filling tau-jet response and resolution histograms
#
fileNames_and_options_jrAnalyzer_calibrated = {}    
fileNames_and_options_jrAnalyzer_calibrated['inputFileNames'] = [fileNames_and_options_applyL2L3param['outputFileName']] 
fileNames_and_options_jrAnalyzer_calibrated['outputFileName'] = os.path.join(outputFilePath, "histogramsJRAtau_calibrated.root")
fileNames_and_options_jrAnalyzer_calibrated['configFileName'] = os.path.join(outputFilePath, "jet_response_analyzer_calibrated.cfg")
make_jrAnalyzer_config(fileNames_and_options_jrAnalyzer_calibrated['configFileName'])
fileNames_and_options_jrAnalyzer_calibrated['logFileName']    = \
  os.path.join(outputFilePath, "jet_response_analyzer_calibrated.log")
fileNames_and_options_jrAnalyzer_calibrated['commandLine']    = '%s -input %s -output %s -algs %s' % \
    (fileNames_and_options_jrAnalyzer_calibrated['configFileName'],
     make_MakeFile_vstring(fileNames_and_options_jrAnalyzer_calibrated['inputFileNames']),
     fileNames_and_options_jrAnalyzer_calibrated['outputFileName'],
     make_MakeFile_vstring([ "".join([ algorithm, suffix]) for algorithm in algorithms for suffix in [ "", "l2"] ]))
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# initialize command-line parameters for fitting jet response and resolution
# for calibrated as well as uncalibrated tau-jets
#
fileNames_and_options_fitResponse_calibrated = {} 
haddFitHists_calibrated = [] 
for histName in histNames:
	fileNames_and_options_fitResponse_calibrated[histName] = {} 
	fileNames_and_options_fitResponse_calibrated[histName]['inputFileNames'] = [fileNames_and_options_jrAnalyzer_calibrated['outputFileName']]
	fileNames_and_options_fitResponse_calibrated[histName]['outputFileName'] = os.path.join(outputFilePath, "responseJRAtau_%s_calibrated.root" % (histName))
	haddFitHists_calibrated.append(fileNames_and_options_fitResponse_calibrated[histName]['outputFileName']) 
	fileNames_and_options_fitResponse_calibrated[histName]['logFileName']    = os.path.join(outputFilePath, "jet_response_fitter_%s_calibrated.log" %histName)
	fileNames_and_options_fitResponse_calibrated[histName]['commandLine']    = '-input %s -output %s -algs %s -fittype %i -histName %s -calibrated true' % \
    	(make_MakeFile_vstring(fileNames_and_options_fitResponse_calibrated[histName]['inputFileNames']),
     	fileNames_and_options_fitResponse_calibrated[histName]['outputFileName'],
	make_MakeFile_vstring([ "".join([ algorithm, suffix]) for algorithm in algorithms for suffix in [ "", "l2"] ]),
     	fitOption,
     	histName
     	)

# add the fit histograms
haddShellFitHistName_calibrated  = os.path.join(outputFilePath, 'harvestFitHist_calibrated.csh')
haddOutputFitHistName_calibrated = os.path.join(outputFilePath, 'fitHist_all_calibrated.root') 

retFitHist_hadd_calibrated = buildConfigFile_hadd(executable_haddFitHists, haddShellFitHistName_calibrated, haddFitHists_calibrated, haddOutputFitHistName_calibrated)

fitHists_hadd_calibrated = {}
fitHists_hadd_calibrated['shellFileName']  = haddShellFitHistName_calibrated
fitHists_hadd_calibrated['inputFileNames'] = haddFitHists_calibrated
fitHists_hadd_calibrated['outputFileName'] = haddOutputFitHistName_calibrated
fitHists_hadd_calibrated['logFileName']    = retFitHist_hadd_calibrated['logFileName']

#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# initialize command-line parameters for making jet resolution plots 
fileNames_and_options_fitResolution = {}
fileNames_and_options_fitResolution['inputFileNames'] = [fitHists_hadd_calibrated['outputFileName']] 
fileNames_and_options_fitResolution['outputFileName'] = os.path.join(outputFilePath, "resolutionJRAtau.root")
fileNames_and_options_fitResolution['logFileName']    = os.path.join(outputFilePath, "jet_resolution_fitter.log")
fileNames_and_options_fitResolution['commandLine']    = '-input %s -output %s -algs %s -dorelrsp true -doetarsp true -docbfits true' % \
    (make_MakeFile_vstring(fileNames_and_options_fitResolution['inputFileNames']),
     fileNames_and_options_fitResolution['outputFileName'],
     make_MakeFile_vstring([ "".join([ algorithm, suffix]) for algorithm in algorithms for suffix in [ "", "l2"] ]))
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# initialize command-line parameters for making jet response plots for  calibrated tau-jets
# plus resolution plots for calibrated compared to uncalibrated tau-jets
#
fileNames_and_options_showHistos = {}
fileNames_and_options_showGraphs = {}

for algorithm in algorithms:

    outputFilePath_plots_algorithm = os.path.join(outputFilePath_plots, algorithm)
    if not os.path.exists(outputFilePath_plots_algorithm):
        os.mkdir(outputFilePath_plots_algorithm)
        
    fileNames_and_options_showHistos[algorithm] = {}
    for refVariable in [ "RefPt", "JetEta" ]:
        fileNames_and_options_showHistos[algorithm][refVariable] = {}
        fileNames_and_options_showHistos[algorithm][refVariable]['inputFileNames'] = [ fitHists_hadd_calibrated['outputFileName'] ]
        fileNames_and_options_showHistos[algorithm][refVariable]['outputFileName'] = "make_jet_inspect_histos_target_%s_%s_calibrated" % (algorithm, refVariable)
        fileNames_and_options_showHistos[algorithm][refVariable]['logFileName']    = \
	  os.path.join(outputFilePath, "jet_inspect_histos_%s_%s_calibrated.log" % (algorithm, refVariable))
        fileNames_and_options_showHistos[algorithm][refVariable]['commandLine']    = \
          '-inputs %s -algs %s -variables RelRsp:%s %s -formats png -batch true -opath %s -colors 1 4 -median true' % \
            (make_MakeFile_vstring(fileNames_and_options_showHistos[algorithm][refVariable]['inputFileNames']),
             make_MakeFile_vstring([ "".join([ algorithm, suffix]) for suffix in [ "", "l2"] ]),
             refVariable,
             "-norm true -npercanvas 1",
             outputFilePath_plots_algorithm)
    
    fileNames_and_options_showGraphs[algorithm] = {}
    for refVariable in [ "RefPt", "JetEta" ]:
        fileNames_and_options_showGraphs[algorithm][refVariable] = {}
        fileNames_and_options_showGraphs[algorithm][refVariable]['inputFileNames'] = [ fitHists_hadd_calibrated['outputFileName'] ]
        fileNames_and_options_showGraphs[algorithm][refVariable]['outputFileName'] = "make_jet_inspect_graphs_%s_%s_target" % (algorithm, refVariable)
        fileNames_and_options_showGraphs[algorithm][refVariable]['logFileName']    = \
          os.path.join(outputFilePath, "jet_inspect_graphs_%s_%s.log" % (algorithm, refVariable))
        fileNames_and_options_showGraphs[algorithm][refVariable]['commandLine']    = \
          '-inputs %s -algs %s -variables RelRspVs%s %s -formats png -batch true -opath %s -colors 1 4' % \
            (make_MakeFile_vstring(fileNames_and_options_showGraphs[algorithm][refVariable]['inputFileNames']),
             make_MakeFile_vstring([ "".join([ algorithm, suffix]) for suffix in [ "", "l2"] ]),
             refVariable,
             "-ymin 0.5 -ymax 1.5 -legx 0.20 -legy 0.35 -legw 0.60",
             outputFilePath_plots_algorithm)
#--------------------------------------------------------------------------------


#--------------------------------------------------------------------------------
#
# done building config files and initializing command-line parameters, now build Makefile...
#
makeFileName = "Makefile_runJRAtauworkflow_l2_%s" % version
makeFile = open(makeFileName, "w")
makeFile.write("\n")
outputFileNames_runJRAtauworkflow = []
outputFileNames_runJRAtauworkflow.extend([
    fileNames_and_options_jrAnalyzer['outputFileName'],
    fileNames_and_options_fitL2param['outputFileName'],
    fileNames_and_options_applyL2L3param['outputFileName'],
    fileNames_and_options_jrAnalyzer_calibrated['outputFileName'],
    fileNames_and_options_fitResolution['outputFileName']
])

for histName in histNames:
        outputFileNames_runJRAtauworkflow.extend([
	    fileNames_and_options_fitResponse_uncalibrated[histName]['outputFileName'],   
	    fileNames_and_options_fitResponse_calibrated[histName]['outputFileName']   
        ])

for algorithm in algorithms:
    for refVariable in [ "RefPt", "JetEta" ]:
        outputFileNames_runJRAtauworkflow.extend([
            fileNames_and_options_showHistos[algorithm][refVariable]['outputFileName'],
            fileNames_and_options_showGraphs[algorithm][refVariable]['outputFileName']
        ])

makeFile.write("all: %s\n" % make_MakeFile_vstring(outputFileNames_runJRAtauworkflow))
makeFile.write("\techo 'Finished running JRAtau Workflow.'\n")
makeFile.write("\n")

# add the ntuple
if len(fileNames_hadd['inputFileNames']) > 0:
    makeFile.write("%s: %s\n" %
      (fileNames_hadd['outputFileName'],
       make_MakeFile_vstring(fileNames_hadd['inputFileNames'])))
    makeFile.write("\t%s %s &> %s\n" %
      (executable_shell,
       fileNames_hadd['shellFileName'],
       fileNames_hadd['logFileName']))
makeFile.write("\n")

# response
makeFile.write("%s: %s\n" %
  (fileNames_and_options_jrAnalyzer['outputFileName'],
   make_MakeFile_vstring(fileNames_and_options_jrAnalyzer['inputFileNames'])))    
makeFile.write("\t%s %s &> %s\n" %
  (executable_jrAnalyzer,
   fileNames_and_options_jrAnalyzer['commandLine'],
   fileNames_and_options_jrAnalyzer['logFileName']))
makeFile.write("\n")

# fit uncalibrated
for histName in histNames:
	makeFile.write("%s: %s\n" %
  	(fileNames_and_options_fitResponse_uncalibrated[histName]['outputFileName'],
   	make_MakeFile_vstring(fileNames_and_options_fitResponse_uncalibrated[histName]['inputFileNames'])))    
	makeFile.write("\t%s %s &> %s\n" %
  	(executable_fitResponse,
   	fileNames_and_options_fitResponse_uncalibrated[histName]['commandLine'],
   	fileNames_and_options_fitResponse_uncalibrated[histName]['logFileName']))
	makeFile.write("\n")

# add the fit uncalibrated hist
if len(fitHists_hadd_uncalibrated['inputFileNames']) > 0:
    makeFile.write("%s: %s\n" %
      (fitHists_hadd_uncalibrated['outputFileName'],
       make_MakeFile_vstring(fitHists_hadd_uncalibrated['inputFileNames'])))
    makeFile.write("\t%s %s &> %s\n" %
      (executable_shell,
       fitHists_hadd_uncalibrated['shellFileName'],
       fitHists_hadd_uncalibrated['logFileName']))
makeFile.write("\n")

# fit param
makeFile.write("%s: %s\n" %
  (fileNames_and_options_fitL2param['outputFileName'],
   make_MakeFile_vstring(fileNames_and_options_fitL2param['inputFileNames'])))    
makeFile.write("\t%s %s &> %s\n" %
  (executable_fitL2param,
   fileNames_and_options_fitL2param['commandLine'],
   fileNames_and_options_fitL2param['logFileName']))
makeFile.write("\n")

# apply fit corr
makeFile.write("%s: %s\n" %
  (fileNames_and_options_applyL2L3param['outputFileName'],
   make_MakeFile_vstring(fileNames_and_options_applyL2L3param['inputFileNames'])))    
makeFile.write("\t%s %s &> %s\n" %
  (executable_applyL2L3param,
   fileNames_and_options_applyL2L3param['commandLine'],
   fileNames_and_options_applyL2L3param['logFileName']))
makeFile.write("\n")

# calibrated response
makeFile.write("%s: %s\n" %
  (fileNames_and_options_jrAnalyzer_calibrated['outputFileName'],
   make_MakeFile_vstring(fileNames_and_options_jrAnalyzer_calibrated['inputFileNames'])))    
makeFile.write("\t%s %s &> %s\n" %
  (executable_jrAnalyzer,
   fileNames_and_options_jrAnalyzer_calibrated['commandLine'],
   fileNames_and_options_jrAnalyzer_calibrated['logFileName']))    
makeFile.write("\n")

# fit calibrated response
for histName in histNames:
	makeFile.write("%s: %s\n" %
  	(fileNames_and_options_fitResponse_calibrated[histName]['outputFileName'],
   	make_MakeFile_vstring(fileNames_and_options_fitResponse_calibrated[histName]['inputFileNames'])))    
	makeFile.write("\t%s %s &> %s\n" %
  	(executable_fitResponse,
   	fileNames_and_options_fitResponse_calibrated[histName]['commandLine'],
   	fileNames_and_options_fitResponse_calibrated[histName]['logFileName']))
	makeFile.write("\n")

# add the calibrated fit hist
if len(fitHists_hadd_calibrated['inputFileNames']) > 0:
    makeFile.write("%s: %s\n" %
      (fitHists_hadd_calibrated['outputFileName'],
       make_MakeFile_vstring(fitHists_hadd_calibrated['inputFileNames'])))
    makeFile.write("\t%s %s &> %s\n" %
      (executable_shell,
       fitHists_hadd_calibrated['shellFileName'],
       fitHists_hadd_calibrated['logFileName']))
makeFile.write("\n")

# resolution
makeFile.write("%s: %s\n" %
  (fileNames_and_options_fitResolution['outputFileName'],
   make_MakeFile_vstring(fileNames_and_options_fitResolution['inputFileNames'])))    
makeFile.write("\t%s %s &> %s\n" %
  (executable_fitResolution,
   fileNames_and_options_fitResolution['commandLine'],
   fileNames_and_options_fitResolution['logFileName']))
makeFile.write("\n")

# histo graph
for algorithm in algorithms:
    for refVariable in [ "RefPt", "JetEta" ]:
        makeFile.write("%s: %s\n" %
          (fileNames_and_options_showHistos[algorithm][refVariable]['outputFileName'],
           make_MakeFile_vstring(fileNames_and_options_showHistos[algorithm][refVariable]['inputFileNames'])))    
        makeFile.write("\t%s %s &> %s\n" %
          (executable_showHistos,
           fileNames_and_options_showHistos[algorithm][refVariable]['commandLine'],
           fileNames_and_options_showHistos[algorithm][refVariable]['logFileName']))
        makeFile.write("%s: %s\n" %
          (fileNames_and_options_showGraphs[algorithm][refVariable]['outputFileName'],
           make_MakeFile_vstring(fileNames_and_options_showGraphs[algorithm][refVariable]['inputFileNames'])))    
        makeFile.write("\t%s %s &> %s\n" %
          (executable_showGraphs,
           fileNames_and_options_showGraphs[algorithm][refVariable]['commandLine'],
           fileNames_and_options_showGraphs[algorithm][refVariable]['logFileName']))       
makeFile.write("\n")  

makeFile.write(".PHONY: clean\n")
makeFile.write("clean:\n")
makeFile.write("\trm -f %s\n" % make_MakeFile_vstring(outputFileNames_runJRAtauworkflow))
makeFile.write("\techo 'Finished deleting old files.'\n")
makeFile.write("\n")
makeFile.close()

print("Finished building Makefile. Now execute 'make -j -f %s'." % makeFileName)

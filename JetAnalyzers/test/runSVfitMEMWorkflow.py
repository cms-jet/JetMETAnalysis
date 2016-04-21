#!/usr/bin/env python

from variable import *
from function import *

import os
import re
import subprocess

inputFilePath = '%s/src/JetMETAnalysis/JetAnalyzers/test/JRAtau' % (os.environ['CMSSW_BASE']) 
outputParPath = '/home/calpas/svfitMEM/CMSSW_8_0_0/src/TauAnalysis/SVfitTF/data/L2L3Corr'
print "plots path: %s" %(inputFilePath)
print "parameters path: %s " %(outputParPath)

algorithms = [
  'ak5tauHPSlooseCombDBcorrAll',
  'ak5tauHPSlooseCombDBcorrOneProng0Pi0',
  #'ak5tauHPSlooseCombDBcorrOneProng1Pi0',
  #'ak5tauHPSlooseCombDBcorrTwoProng0Pi0',
  #'ak5tauHPSlooseCombDBcorrTwoProng1Pi0',
  #'ak5tauHPSlooseCombDBcorrThreeProng0Pi0',
  #'ak5tauHPSlooseCombDBcorrThreeProng1Pi0',
  ##
  #'ak5tauHPSlooseCombDBcorrAlll2',
  #'ak5tauHPSlooseCombDBcorrOneProng0Pi0l2',
  #'ak5tauHPSlooseCombDBcorrOneProng1Pi0l2',
  #'ak5tauHPSlooseCombDBcorrTwoProng0Pi0l2',
  #'ak5tauHPSlooseCombDBcorrTwoProng1Pi0l2',
  #'ak5tauHPSlooseCombDBcorrThreeProng0Pi0l2',
  #'ak5tauHPSlooseCombDBcorrThreeProng1Pi0l2',
]

#--------------------------------------------------------------------------------
#
# create cristalBall fit directories
#
outputFilePath = '%s/src/JetMETAnalysis/JetAnalyzers/test/JRAtau/%s' % (os.environ['CMSSW_BASE'], version)
fitDir         = outputFilePath+"/plots"
cuts           = ["pass", "failed"]
for cut in cuts:
  for alg in algorithms:
    outputPlotPath = fitDir+'/%s/crystalBallFit/%s/' % (alg, cut)
    if not os.path.exists(outputPlotPath):
      os.makedirs(outputPlotPath)
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# build Makefile...
#
makeFileName = "Makefile_runSVfitMEMworkflow_l2_%s" % version
makeFile = open(makeFileName, "w")
makeFile.write("\n")
outputFileNames_runJRAtauworkflow = []
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# add all samples files
#
haddInputFileNames = []
#for sampleToAnalyze in samplesToAnalyze:
#    nbOfSample = []
#    for ntupleFileName in ntupleFileNames:
#        if ntupleFile_matcher.match(ntupleFileName) and \
#          ntupleFile_matcher.match(ntupleFileName).group('sample') == sampleToAnalyze:
#            haddInputFileNames.append(ntupleFileName)
#	    nbOfSample.append(ntupleFileName)
#    print "sample = %s: found %i input files." % (sampleToAnalyze, len(nbOfSample))
#print "found %i input files." % (len(haddInputFileNames))

haddShellFileName  = os.path.join(outputFilePath, 'harvestJRAtauNtuples.csh')
haddOutputFileName = os.path.join(outputFilePath, 'ntupleJRAtau_all.root') # copy where you can
retVal_hadd = buildConfigFile_hadd(executable_hadd, haddShellFileName, haddInputFileNames, haddOutputFileName)

fileNames_hadd = {}
fileNames_hadd['shellFileName']  = haddShellFileName
fileNames_hadd['inputFileNames'] = haddInputFileNames
fileNames_hadd['outputFileName'] = haddOutputFileName
fileNames_hadd['logFileName']    = retVal_hadd['logFileName']
#outputFileNames_runJRAtauworkflow.extend([ fileNames_hadd['outputFileName'],])


# makefile 
#if len(fileNames_hadd['inputFileNames']) > 0:
#    makeFile.write("%s: %s\n" %
#      (fileNames_hadd['outputFileName'],
#       make_MakeFile_vstring(fileNames_hadd['inputFileNames'])))
#    makeFile.write("\t%s %s &> %s\n" %
#      (executable_shell,
#       fileNames_hadd['shellFileName'],
#       fileNames_hadd['logFileName']))
#makeFile.write("\n")
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# jet response for uncalibrated tau-jets
#
fileNames_and_options_jrAnalyzer = {}    
fileNames_and_options_jrAnalyzer['inputFileNames'] = [fileNames_hadd['outputFileName']]
fileNames_and_options_jrAnalyzer['outputFileName'] = os.path.join(outputFilePath, "jet_response_analyzer_uncalibrated.root")
fileNames_and_options_jrAnalyzer['configFileName'] = os.path.join(outputFilePath, "jet_response_analyzer_uncalibrated.cfg")
make_jrAnalyzer_config(fileNames_and_options_jrAnalyzer['configFileName'])
fileNames_and_options_jrAnalyzer['logFileName']    = os.path.join(outputFilePath, "jet_response_analyzer_uncalibrated.log")
fileNames_and_options_jrAnalyzer['commandLine']    = '%s -input %s -output %s -algs %s -nbinsrelrsp 250' % \
    (fileNames_and_options_jrAnalyzer['configFileName'],
     make_MakeFile_vstring(fileNames_and_options_jrAnalyzer['inputFileNames']),
     fileNames_and_options_jrAnalyzer['outputFileName'],
     "".join([ "%s:0.3 " % algorithm for algorithm in algorithms ])
    )
#outputFileNames_runJRAtauworkflow.extend([ fileNames_and_options_jrAnalyzer['outputFileName'],])

# makefile
#makeFile.write("%s: %s\n" %
#  (fileNames_and_options_jrAnalyzer['outputFileName'],
#   make_MakeFile_vstring(fileNames_and_options_jrAnalyzer['inputFileNames'])))    
#makeFile.write("\t%s %s &> %s\n" %
#  (executable_jrAnalyzer,
#   fileNames_and_options_jrAnalyzer['commandLine'],
#   fileNames_and_options_jrAnalyzer['logFileName']))
#makeFile.write("\n")
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# round 1.1: fit uncalibrated jet response 
#
fileNames_and_options_fitResponse_uncalibrated = {} 
for alg in algorithms:
  haddFitHists = []
  for histName in histNames:
    fileNames_and_options_fitResponse_uncalibrated[histName] = {} 
    fileNames_and_options_fitResponse_uncalibrated[histName]['inputFileNames'] = [fileNames_and_options_jrAnalyzer['outputFileName']]
    fileNames_and_options_fitResponse_uncalibrated[histName]['outputFileName'] = os.path.join(fitDir, "%s/responseJRAtau_%s.root" % (alg, histName))
    haddFitHists.append(fileNames_and_options_fitResponse_uncalibrated[histName]['outputFileName']) 
    fileNames_and_options_fitResponse_uncalibrated[histName]['logFileName']    = os.path.join(fitDir, "%s/responseJRAtau_%s.log" % (alg, histName))
    fileNames_and_options_fitResponse_uncalibrated[histName]['commandLine']    = '-input %s -output %s -algs %s -fittype %i -histName %s -polDeg 1 -normalized true -fitDir %s' % \
    (make_MakeFile_vstring(fileNames_and_options_fitResponse_uncalibrated[histName]['inputFileNames']),
     fileNames_and_options_fitResponse_uncalibrated[histName]['outputFileName'],
     alg,
     fitOption,
     histName,
     fitDir
    )
    outputFileNames_runJRAtauworkflow.extend([ fileNames_and_options_fitResponse_uncalibrated[histName]['outputFileName'],])

    # makefile
    makeFile.write("%s: %s\n" %
    (fileNames_and_options_fitResponse_uncalibrated[histName]['outputFileName'],
    make_MakeFile_vstring(fileNames_and_options_fitResponse_uncalibrated[histName]['inputFileNames'])))    
    makeFile.write("\t%s %s &> %s\n" %
    (executable_fitResponse,
     fileNames_and_options_fitResponse_uncalibrated[histName]['commandLine'],
     fileNames_and_options_fitResponse_uncalibrated[histName]['logFileName']))
    makeFile.write("\n")
    #--------------------------------------------------------------------------------

  #--------------------------------------------------------------------------------
  #
  # round 1.2: add the uncalibrated fit histograms
  #
  haddShellFitHistName  = os.path.join(fitDir, '%s/harvestFitHist.csh' %(alg))
  haddOutputFitHistName = os.path.join(fitDir, '%s/fitHists.root' %(alg)) 

  retFitHist_hadd = buildConfigFile_hadd(executable_haddFitHists, haddShellFitHistName, haddFitHists, haddOutputFitHistName)

  fitHists_hadd_uncalibrated = {}
  fitHists_hadd_uncalibrated['shellFileName']  = haddShellFitHistName
  fitHists_hadd_uncalibrated['inputFileNames'] = haddFitHists
  fitHists_hadd_uncalibrated['outputFileName'] = haddOutputFitHistName
  fitHists_hadd_uncalibrated['logFileName']    = retFitHist_hadd['logFileName']
  outputFileNames_runJRAtauworkflow.extend([ fitHists_hadd_uncalibrated['outputFileName'],])

  # makefile
  if len(fitHists_hadd_uncalibrated['inputFileNames']) > 0:
      makeFile.write("%s: %s\n" %
        (fitHists_hadd_uncalibrated['outputFileName'],
         make_MakeFile_vstring(fitHists_hadd_uncalibrated['inputFileNames'])))
      makeFile.write("\t%s %s &> %s\n" %
        (executable_shell,
         fitHists_hadd_uncalibrated['shellFileName'],
         fitHists_hadd_uncalibrated['logFileName']))
  makeFile.write("\n")
  #--------------------------------------------------------------------------------


  #--------------------------------------------------------------------------------
  #
  # determine L2L3 corrections and fit them
  #
  #pars=[6, 17]
  #for par in pars:
  for par in range(0, 19): # from 0 to 18!!
    fileNames_and_options_fitL2param = {}
    fileNames_and_options_fitL2param['inputFileNames'] = [fitHists_hadd_uncalibrated['outputFileName']] 
	
    outdir = outputParPath+"/"+alg+"/"+str(par)
    subprocess.call(['rm', '-r', outdir]) 
    subprocess.check_output(['mkdir', '-p', outdir]) 
	
    fileNames_and_options_fitL2param['outputFileName'] = os.path.join(outdir, "fitL2param.root")
    outputFileNames_runJRAtauworkflow.extend([ fileNames_and_options_fitL2param['outputFileName'],])
    fileNames_and_options_fitL2param['logFileName']    = os.path.join(outdir, "fitL2param.log")
    fileNames_and_options_fitL2param['commandLine']    = \
    '-input %s -output %s -era %s -algs %s -formats png -batch true -l2l3 true -cbPar %i -outputDir %s' % \
    (make_MakeFile_vstring(fileNames_and_options_fitL2param['inputFileNames']),
     fileNames_and_options_fitL2param['outputFileName'],
     era,
     alg,
     par,
     outdir)

    makeFile.write("%s: %s\n" %
       (fileNames_and_options_fitL2param['outputFileName'],
       #fileNames_and_options_fitResponse_uncalibrated_pol1['outputFileName'],
        fitHists_hadd_uncalibrated['outputFileName']))
    makeFile.write("\t%s %s &> %s\n" %
       (executable_fitL2param,
        fileNames_and_options_fitL2param['commandLine'],
        fileNames_and_options_fitL2param['logFileName']))
    makeFile.write("\n")

#makeFile.write(".PHONY: clean\n")
#makeFile.write("clean:\n")
#makeFile.write("\trm -f %s\n" % make_MakeFile_vstring(outputFileNames_runJRAtauworkflow))
#makeFile.write("\techo 'Finished deleting old files.'\n")
#makeFile.write("\n")

makeFile.close()


#--------------------------------------------------------------------------------
#
# write the all target on top of the make file
#
append_copy = open(makeFileName, "r")
original_text = append_copy.read()
append_copy.close()
append_copy = open(makeFileName, "w")
append_copy.write("all: %s\n" % make_MakeFile_vstring(outputFileNames_runJRAtauworkflow))
append_copy.write(original_text)
append_copy.close()

#print("Finished building Makefile. Now execute 'make -j 16 -f %s'." % makeFileName)
print("Finished building Makefile. Now execute 'make -j 23 -f %s'." % makeFileName)


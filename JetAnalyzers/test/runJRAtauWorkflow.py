#!/usr/bin/env python

from variable import *
from function import *

import os
import re



#--------------------------------------------------------------------------------
#
# create cristalBall fit directories
#
outputFilePath = '%s/src/JetMETAnalysis/JetAnalyzers/test/JRAtau/%s' % (os.environ['CMSSW_BASE'], version)

fitDirPol1  = outputFilePath+"/plots/pol1/"
fitDirPol7  = outputFilePath+"/plots/pol7/"
cuts  = ["pass", "failed"]
for cut in cuts:
  for alg in algorithms:
    outputPlotPath = fitDirPol1+'%s/crystalBallFit/%s/' % (alg, cut)
    if not os.path.exists(outputPlotPath):
      os.makedirs(outputPlotPath)
    outputPlotPath = fitDirPol7+'%s/crystalBallFit/%s/' % (alg, cut)
    if not os.path.exists(outputPlotPath):
      os.makedirs(outputPlotPath)

    alg+="l2" # l2 correction
    outputPlotPath = fitDirPol1+'%s/crystalBallFit/%s/' % (alg, cut)
    if not os.path.exists(outputPlotPath):
      os.makedirs(outputPlotPath)
    outputPlotPath = fitDirPol7+'%s/crystalBallFit/%s/' % (alg, cut)
    if not os.path.exists(outputPlotPath):
      os.makedirs(outputPlotPath)
#--------------------------------------------------------------------------------

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

#--------------------------------------------------------------------------------
#
# add all samples files
#
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
haddOutputFileName = os.path.join(outputFilePath, 'ntupleJRAtau_all.root') # copy where you can
retVal_hadd = buildConfigFile_hadd(executable_hadd, haddShellFileName, haddInputFileNames, haddOutputFileName)

fileNames_hadd = {}
fileNames_hadd['shellFileName']  = haddShellFileName
fileNames_hadd['inputFileNames'] = haddInputFileNames
fileNames_hadd['outputFileName'] = haddOutputFileName
fileNames_hadd['logFileName']    = retVal_hadd['logFileName']
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
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# round 1.1: fit jet response for uncalibrated tau-jets
#
fileNames_and_options_fitResponse_uncalibrated = {} 
haddFitHists = []
for histName in histNames:
	fileNames_and_options_fitResponse_uncalibrated[histName] = {} 
	fileNames_and_options_fitResponse_uncalibrated[histName]['inputFileNames'] = [fileNames_and_options_jrAnalyzer['outputFileName']]
	fileNames_and_options_fitResponse_uncalibrated[histName]['outputFileName'] = os.path.join(outputFilePath, "responseJRAtau_%s_uncalibrated.root" % (histName))
	haddFitHists.append(fileNames_and_options_fitResponse_uncalibrated[histName]['outputFileName']) 
	fileNames_and_options_fitResponse_uncalibrated[histName]['logFileName']    = os.path.join(outputFilePath, "responseJRAtau_%s_uncalibrated.log" %histName)
	fileNames_and_options_fitResponse_uncalibrated[histName]['commandLine']    = '-input %s -output %s -algs %s -fittype %i -histName %s -isItCalibrated uncalibrated -pol1 false -normalized true -fitDirPol7 %s' % \
    	(make_MakeFile_vstring(fileNames_and_options_fitResponse_uncalibrated[histName]['inputFileNames']),
     	 fileNames_and_options_fitResponse_uncalibrated[histName]['outputFileName'],
     	 make_MakeFile_vstring(algorithms),
     	 fitOption,
     	 histName,
	 fitDirPol7
     	)
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# round 1.2: add the uncalibrated fit histograms
#
haddShellFitHistName  = os.path.join(outputFilePath, 'harvestFitHist_uncalibrated.csh')
haddOutputFitHistName = os.path.join(outputFilePath, 'fitHists_uncalibrated.root') 

retFitHist_hadd = buildConfigFile_hadd(executable_haddFitHists, haddShellFitHistName, haddFitHists, haddOutputFitHistName)

fitHists_hadd_uncalibrated = {}
fitHists_hadd_uncalibrated['shellFileName']  = haddShellFitHistName
fitHists_hadd_uncalibrated['inputFileNames'] = haddFitHists
fitHists_hadd_uncalibrated['outputFileName'] = haddOutputFitHistName
fitHists_hadd_uncalibrated['logFileName']    = retFitHist_hadd['logFileName']
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# round 2.1: fit jet response for uncalibrated tau-jets 
# Fixed the CB parameters determined above and use pol1 instead of pol7
#
fileNames_and_options_fitResponse_uncalibrated_pol1 = {} 
haddFitHists_pol1 = []
for histName in histNames:
	fileNames_and_options_fitResponse_uncalibrated_pol1[histName] = {} 
	fileNames_and_options_fitResponse_uncalibrated_pol1[histName]['inputFileNames'] = [fitHists_hadd_uncalibrated['outputFileName']]
	fileNames_and_options_fitResponse_uncalibrated_pol1[histName]['outputFileName'] = os.path.join(outputFilePath, "responseJRAtau_%s_uncalibrated_pol1.root" % (histName))
	haddFitHists_pol1.append(fileNames_and_options_fitResponse_uncalibrated_pol1[histName]['outputFileName']) 
	fileNames_and_options_fitResponse_uncalibrated_pol1[histName]['logFileName']    = os.path.join(outputFilePath, "responseJRAtau_%s_uncalibrated_plo1.log" %histName)
	fileNames_and_options_fitResponse_uncalibrated_pol1[histName]['commandLine']    = '-input %s -output %s -algs %s -fittype %i -histName %s -isItCalibrated uncalibrated -pol1 true -normalized true -fitDirPol1 %s' % \
    	(make_MakeFile_vstring(fileNames_and_options_fitResponse_uncalibrated_pol1[histName]['inputFileNames']),
     	 fileNames_and_options_fitResponse_uncalibrated_pol1[histName]['outputFileName'],
     	 make_MakeFile_vstring(algorithms),
     	 fitOption,
     	 histName,
	 fitDirPol1
     	)
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# round 2.2: add the uncalibrated fit histograms
#
haddShellFitHistName_pol1  = os.path.join(outputFilePath, 'harvestFitHist_uncalibrated_pol1.csh')
haddOutputFitHistName_pol1 = os.path.join(outputFilePath, 'fitHists_uncalibrated_pol1.root') 

retFitHist_hadd_pol1 = buildConfigFile_hadd(executable_haddFitHists, haddShellFitHistName_pol1, haddFitHists_pol1, haddOutputFitHistName_pol1)

fitHists_hadd_uncalibrated_pol1 = {}
fitHists_hadd_uncalibrated_pol1['shellFileName']  = haddShellFitHistName_pol1
fitHists_hadd_uncalibrated_pol1['inputFileNames'] = haddFitHists_pol1
fitHists_hadd_uncalibrated_pol1['outputFileName'] = haddOutputFitHistName_pol1
fitHists_hadd_uncalibrated_pol1['logFileName']    = retFitHist_hadd_pol1['logFileName']
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# determining L2L3 correction and fit them
#
fileNames_and_options_fitL2param = {}
fileNames_and_options_fitL2param['inputFileNames'] = [fitHists_hadd_uncalibrated_pol1['outputFileName']]
fileNames_and_options_fitL2param['outputFileName'] = os.path.join(outputFilePath, "fitL2param.root")
fileNames_and_options_fitL2param['logFileName']    = os.path.join(outputFilePath, "fitL2param.log")
fileNames_and_options_fitL2param['commandLine']    = \
  '-input %s -output %s -era %s -algs %s -formats png -batch true -l2l3 true -fitDirPol7 %s' % \
    (make_MakeFile_vstring(fileNames_and_options_fitL2param['inputFileNames']),
     fileNames_and_options_fitL2param['outputFileName'],
     era,
     make_MakeFile_vstring(algorithms),
     fitDirPol7
    )
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# apply L2/L3 correction (determined above) to the original tree 
#
fileNames_and_options_applyL2L3param = {}
fileNames_and_options_applyL2L3param['inputFileNames'] = [ fileNames_and_options_jrAnalyzer['outputFileName'],
							   fileNames_and_options_fitL2param['outputFileName'] ]
fileNames_and_options_applyL2L3param['outputFileName'] = os.path.join(outputFilePath, "applyL2L3param.root")
fileNames_and_options_applyL2L3param['logFileName']    = os.path.join(outputFilePath, "applyL2L3param.log")
fileNames_and_options_applyL2L3param['commandLine']    = '-input %s -output %s -era %s -algs %s -jecpath %s -levels 2 -saveitree false' % \
    (fileNames_hadd['outputFileName'],
     fileNames_and_options_applyL2L3param['outputFileName'],
     era,
     make_MakeFile_vstring(algorithms),
     jecTextFilePath)
#--------------------------------------------------------------------------------    

#--------------------------------------------------------------------------------
#
# jet response for calibrated tau-jets
#
fileNames_and_options_jrAnalyzer_calibrated = {}    
fileNames_and_options_jrAnalyzer_calibrated['inputFileNames'] = [fileNames_and_options_applyL2L3param['outputFileName']] 
fileNames_and_options_jrAnalyzer_calibrated['outputFileName'] = os.path.join(outputFilePath, "jet_response_analyzer_calibrated.root")
fileNames_and_options_jrAnalyzer_calibrated['configFileName'] = os.path.join(outputFilePath, "jet_response_analyzer_calibrated.cfg")
make_jrAnalyzer_config(fileNames_and_options_jrAnalyzer_calibrated['configFileName'])
fileNames_and_options_jrAnalyzer_calibrated['logFileName']    = os.path.join(outputFilePath, "jet_response_analyzer_calibrated.log")
fileNames_and_options_jrAnalyzer_calibrated['commandLine']    = '%s -input %s -output %s -algs %s -nbinsrelrsp 250' % \
    (fileNames_and_options_jrAnalyzer_calibrated['configFileName'],
     make_MakeFile_vstring(fileNames_and_options_jrAnalyzer_calibrated['inputFileNames']),
     fileNames_and_options_jrAnalyzer_calibrated['outputFileName'],
     make_MakeFile_vstring([ "".join([ algorithm, suffix]) for algorithm in algorithms for suffix in ["l2"] ]))
#--------------------------------------------------------------------------------


#--------------------------------------------------------------------------------
#
# fit jet response for calibrated tau-jets
#
fileNames_and_options_fitResponse_calibrated = {} 
haddFitHists_calibrated = [] 
for histName in histNames:
	fileNames_and_options_fitResponse_calibrated[histName] = {} 
	fileNames_and_options_fitResponse_calibrated[histName]['inputFileNames'] = [fileNames_and_options_jrAnalyzer_calibrated['outputFileName']]
	fileNames_and_options_fitResponse_calibrated[histName]['outputFileName'] = os.path.join(outputFilePath, "responseJRAtau_%s_calibrated.root" % (histName))
	haddFitHists_calibrated.append(fileNames_and_options_fitResponse_calibrated[histName]['outputFileName']) 
	fileNames_and_options_fitResponse_calibrated[histName]['logFileName']    = os.path.join(outputFilePath, "responseJRAtau_%s_calibrated.log" %histName)
	fileNames_and_options_fitResponse_calibrated[histName]['commandLine']    = '-input %s -output %s -algs %s -fittype %i -histName %s -isItCalibrated calibrated  -pol1 false -normalized true -fitDirPol7 %s' % \
    	(make_MakeFile_vstring(fileNames_and_options_fitResponse_calibrated[histName]['inputFileNames']),
     	fileNames_and_options_fitResponse_calibrated[histName]['outputFileName'],
	#make_MakeFile_vstring([ "".join([ algorithm, suffix]) for algorithm in algorithms for suffix in [ "", "l2"] ]),
	make_MakeFile_vstring([ "".join([ algorithm, suffix]) for algorithm in algorithms for suffix in [ "l2"] ]), # now fit only the calibrate
     	fitOption,
     	histName,
	fitDirPol7
     	)
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# add the calibrated fit histograms
#
haddShellFitHistName_calibrated  = os.path.join(outputFilePath, 'harvestFitHist_calibrated.csh')
haddOutputFitHistName_calibrated = os.path.join(outputFilePath, 'fitHists_calibrated.root') 

retFitHist_hadd_calibrated = buildConfigFile_hadd(executable_haddFitHists, haddShellFitHistName_calibrated, haddFitHists_calibrated, haddOutputFitHistName_calibrated)

fitHists_hadd_calibrated = {}
fitHists_hadd_calibrated['shellFileName']  = haddShellFitHistName_calibrated
fitHists_hadd_calibrated['inputFileNames'] = haddFitHists_calibrated
fitHists_hadd_calibrated['outputFileName'] = haddOutputFitHistName_calibrated
fitHists_hadd_calibrated['logFileName']    = retFitHist_hadd_calibrated['logFileName']
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# add the uncalibrated and calibrated fit histograms for the next step
#
haddShellFitHistName_all  = os.path.join(outputFilePath, 'harvestFitHist_all.csh')
haddOutputFitHistName_all = os.path.join(outputFilePath, 'fitHists_all.root') 
haddFitHists_all = [ fitHists_hadd_uncalibrated['outputFileName'], fitHists_hadd_calibrated['outputFileName']] 
retFitHist_hadd_all = buildConfigFile_hadd(executable_haddFitHists, haddShellFitHistName_all, haddFitHists_all, haddOutputFitHistName_all)
fitHists_hadd_all = {}
fitHists_hadd_all['shellFileName']  = haddShellFitHistName_all
fitHists_hadd_all['inputFileNames'] = haddFitHists_all
fitHists_hadd_all['outputFileName'] = haddOutputFitHistName_all
fitHists_hadd_all['logFileName']    = retFitHist_hadd_all['logFileName']
#--------------------------------------------------------------------------------


#--------------------------------------------------------------------------------
#
# jet response and resolution graphs 
fileNames_and_options_fitResolution = {}
fileNames_and_options_fitResolution['inputFileNames'] = [fitHists_hadd_all['outputFileName']] 
fileNames_and_options_fitResolution['outputFileName'] = os.path.join(outputFilePath, "resolutionJRAtau.root")
fileNames_and_options_fitResolution['logFileName']    = os.path.join(outputFilePath, "resolutionJRAtau.log")
fileNames_and_options_fitResolution['commandLine']    = '-input %s -output %s -algs %s -dorelrsp true' % \
    (make_MakeFile_vstring(fileNames_and_options_fitResolution['inputFileNames']),
     fileNames_and_options_fitResolution['outputFileName'],
     make_MakeFile_vstring([ "".join([ algorithm, suffix]) for algorithm in algorithms for suffix in [ "", "l2"] ]))
#--------------------------------------------------------------------------------


#--------------------------------------------------------------------------------
#
# hist:  response calibrated vs uncalibrated
# graph: resolution calibrated vs uncalibrated
#
fileNames_and_options_showHistos = {}
fileNames_and_options_showGraphs = {}

for algorithm in algorithms:
    outputFilePath_plots_algorithm = outputFilePath+"/plots/%s/tauRelRsp/" %(algorithm)
    if not os.path.exists(outputFilePath_plots_algorithm):
        os.mkdir(outputFilePath_plots_algorithm)
        
    fileNames_and_options_showHistos[algorithm] = {}
    fileNames_and_options_showHistos[algorithm]['inputFileNames'] = [ fitHists_hadd_all['outputFileName'] ]
    fileNames_and_options_showHistos[algorithm]['outputFileName'] = "make_jet_inspect_histos_target_%s_calibrated" % (algorithm)
    fileNames_and_options_showHistos[algorithm]['logFileName']    = \
       os.path.join(outputFilePath, "jet_inspect_histos_%s_calibrated.log" % (algorithm))
    fileNames_and_options_showHistos[algorithm]['commandLine']    = \
       '-inputs %s -algs %s -variables RelRsp:JetEta:RefPt %s -formats png -batch true -opath %s -colors 1 4 -peak false' % \
       (make_MakeFile_vstring(fileNames_and_options_showHistos[algorithm]['inputFileNames']),
        make_MakeFile_vstring([ "".join([ algorithm, suffix]) for suffix in [ "", "l2"] ]),
        "-norm true -npercanvas 1",
        outputFilePath_plots_algorithm)


    fileNames_and_options_showGraphs[algorithm] = {}
    for refVariable in [ "RefPt", "JetEta" ]:
    #for refVariable in [ "JetEta:RefPt" ]:
        fileNames_and_options_showGraphs[algorithm][refVariable] = {}
        fileNames_and_options_showGraphs[algorithm][refVariable]['inputFileNames'] = [ fileNames_and_options_fitResolution['outputFileName'] ]
        fileNames_and_options_showGraphs[algorithm][refVariable]['outputFileName'] = "make_jet_inspect_graphs_%s_%s_target" % (algorithm, refVariable)
        fileNames_and_options_showGraphs[algorithm][refVariable]['logFileName']    = \
          os.path.join(outputFilePath, "jet_inspect_graphs_%s_%s.log" % (algorithm, refVariable))
	if (refVariable == "RefPt"):
          fileNames_and_options_showGraphs[algorithm][refVariable]['commandLine']    = \
            '-inputs %s -algs %s -variables RelRspVs%s %s -formats png -batch true -opath %s -colors 1 4 -logx true' % \
              (make_MakeFile_vstring(fileNames_and_options_showGraphs[algorithm][refVariable]['inputFileNames']),
               make_MakeFile_vstring([ "".join([ algorithm, suffix]) for suffix in [ "", "l2"] ]),
               refVariable,
               "-ymin 0.5 -ymax 1.5 -legx 0.20 -legy 0.35 -legw 0.60",
               outputFilePath_plots_algorithm)
	else:
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
# build Makefile...
#
makeFileName = "Makefile_runJRAtauworkflow_l2_%s" % version
makeFile = open(makeFileName, "w")
makeFile.write("\n")
outputFileNames_runJRAtauworkflow = []
outputFileNames_runJRAtauworkflow.extend([
    fileNames_hadd['outputFileName'],
    fileNames_and_options_jrAnalyzer['outputFileName'],
    fitHists_hadd_uncalibrated['outputFileName'],
    fileNames_and_options_fitL2param['outputFileName'],
    fileNames_and_options_applyL2L3param['outputFileName'],
    fileNames_and_options_jrAnalyzer_calibrated['outputFileName'],
    fitHists_hadd_calibrated['outputFileName'],
    fitHists_hadd_all['outputFileName'],
    fileNames_and_options_fitResolution['outputFileName']
])

for histName in histNames:
        outputFileNames_runJRAtauworkflow.extend([
	    fileNames_and_options_fitResponse_uncalibrated[histName]['outputFileName'],   
	    fileNames_and_options_fitResponse_calibrated[histName]['outputFileName']   
        ])

for algorithm in algorithms:
    outputFileNames_runJRAtauworkflow.extend([
      fileNames_and_options_showHistos[algorithm]['outputFileName'],
    ])
    for refVariable in [ "RefPt", "JetEta" ]:
        outputFileNames_runJRAtauworkflow.extend([
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

# determine l2l3
makeFile.write("%s: %s\n" %
  (fileNames_and_options_fitL2param['outputFileName'],
   make_MakeFile_vstring(fileNames_and_options_fitL2param['inputFileNames'])))    
makeFile.write("\t%s %s &> %s\n" %
  (executable_fitL2param,
   fileNames_and_options_fitL2param['commandLine'],
   fileNames_and_options_fitL2param['logFileName']))
makeFile.write("\n")

# apply l2l3
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

#add uncalibrated and calibrated fit
if len(fitHists_hadd_all['inputFileNames']) > 0:
    makeFile.write("%s: %s\n" %
      (fitHists_hadd_all['outputFileName'],
       make_MakeFile_vstring(fitHists_hadd_all['inputFileNames'])))
    makeFile.write("\t%s %s &> %s\n" %
      (executable_shell,
       fitHists_hadd_all['shellFileName'],
       fitHists_hadd_all['logFileName']))
makeFile.write("\n")

# resolution
makeFile.write("%s: %s\n" %
  (fileNames_and_options_fitResolution['outputFileName'],
   #make_MakeFile_vstring(fitHists_hadd_calibrated['outputFileName'])))    
   fitHists_hadd_all['outputFileName']))    
makeFile.write("\t%s %s &> %s\n" %
  (executable_fitResolution,
   fileNames_and_options_fitResolution['commandLine'],
   fileNames_and_options_fitResolution['logFileName']))
makeFile.write("\n")

# histo graph
for algorithm in algorithms:
    makeFile.write("%s: %s\n" %
      (fileNames_and_options_showHistos[algorithm]['outputFileName'],
       fitHists_hadd_all['outputFileName']))    
    makeFile.write("\t%s %s &> %s\n" %
      (executable_showHistos,
       fileNames_and_options_showHistos[algorithm]['commandLine'],
       fileNames_and_options_showHistos[algorithm]['logFileName']))
    for refVariable in [ "RefPt", "JetEta" ]:
        makeFile.write("%s: %s\n" %
          (fileNames_and_options_showGraphs[algorithm][refVariable]['outputFileName'],
	   fileNames_and_options_fitResolution['outputFileName']))    
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
#--------------------------------------------------------------------------------

#print("Finished building Makefile. Now execute 'make -j 16 -f %s'." % makeFileName)
print("Finished building Makefile. Now execute 'make -j -f %s'." % makeFileName)


#!/usr/bin/env python

#from TauAnalysis.TauIdEfficiency.tools.buildConfigFilesTauIdEffAnalysis import buildConfigFile_hadd

import os
import re

version = 'v1_2enRecoveryCBa'
era = 'TauJec11V1'

#inputFilePath = '/data2/veelken/CMSSW_4_2_x/JRAtauNtuples/Ztautau/v1_2enRecovery/' \
#               + 'user/v/veelken/CMSSW_4_2_x/JRAtauNtuples/Ztautau/v1_2enRecovery'
inputFilePath  = '/hdfs/cms/store/user/calpas/DYJetsToLL_M-50_13TeV-madgraph-pythia8/DYJetsToLL_M-50_13TeV-madgraph-pythia8_TransferFunc_v1/a683f4f5fc0a3cbafdd5a17e17e4babe/'

#outputFilePath = '/data1/veelken/tmp/JRAtau/%s' % version
outputFilePath = '/home/calpas/TransferFunction/CMSSW_7_2_5/src/JetMETAnalysis/JetAnalyzers/test/JRAtau/%s' % version

samplesToAnalyze = [
    'Ztautau'
]

algorithms = [
  'ak5tauHPSlooseCombDBcorrAll',
  'ak5tauHPSlooseCombDBcorrOneProng0Pi0',
  'ak5tauHPSlooseCombDBcorrOneProng1Pi0',
  'ak5tauHPSlooseCombDBcorrOneProng2Pi0',
  'ak5tauHPSlooseCombDBcorrThreeProng0Pi0',
  'ak5tauHPSmediumCombDBcorrAll',
  'ak5tauHPSmediumCombDBcorrOneProng0Pi0',
  'ak5tauHPSmediumCombDBcorrOneProng1Pi0',
  'ak5tauHPSmediumCombDBcorrOneProng2Pi0',
  'ak5tauHPSmediumCombDBcorrThreeProng0Pi0',
  'ak5tauHPStightCombDBcorrAll',
  'ak5tauHPStightCombDBcorrOneProng0Pi0',
  'ak5tauHPStightCombDBcorrOneProng1Pi0',
  'ak5tauHPStightCombDBcorrOneProng2Pi0',
  'ak5tauHPStightCombDBcorrThreeProng0Pi0',
]

execDir = "%s/bin/%s/" % (os.environ['CMSSW_BASE'], os.environ['SCRAM_ARCH'])

executable_jrAnalyzer     = execDir + 'jet_response_analyzer_x'
executable_fitResponse    = execDir + 'jet_response_fitter_x'
executable_fitResolution  = execDir + 'jet_response_and_resolution_x'
executable_fitL3param     = execDir + 'jet_l3_correction_x'
executable_fitL2param     = execDir + 'jet_l2_correction_x'
executable_applyL2L3param = execDir + 'jet_apply_jec_x'
executable_showGraphs     = execDir + 'jet_inspect_graphs_x'
executable_showHistos     = execDir + 'jet_inspect_histos_x'
executable_showProfiles   = execDir + 'jet_inspect_profiles_x'
executable_hadd           = 'hadd -f'
executable_shell          = '/bin/csh'
executable_python         = 'python'

# define function used for fitting tau-jet response:
#   0 = Gaussian
#   1 = Crystall-Ball function
fitOption = 1

jecTextFilePath = os.getcwd()

if not os.path.exists(outputFilePath):
    os.mkdir(outputFilePath)

outputFilePath_plots = os.path.join(outputFilePath, "plots")
if not os.path.exists(outputFilePath_plots):
    os.mkdir(outputFilePath_plots)

#--------------------------------------------------------------------------------
#
# build shell script for running 'hadd' in order to "harvest" histograms
# produced by FWLiteZllRecoilCorrectionAnalyzer macro
#
fileNames_hadd = {}

ntupleFileNames = os.listdir(inputFilePath)
#print(ntupleFileNames)

ntupleFile_regex = r"ntupleJRAtau_(?P<sample>\w*)_(?P<gridJob>\d*)_(?P<gridTry>\d*)_(?P<gridId>[a-zA-Z0-9]*).root"
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


for sampleToAnalyze in samplesToAnalyze:
    haddInputFileNames = []
    for ntupleFileName in ntupleFileNames:
        if ntupleFile_matcher.match(ntupleFileName) and \
           ntupleFile_matcher.match(ntupleFileName).group('sample') == sampleToAnalyze:
            haddInputFileNames.append(os.path.join(inputFilePath, ntupleFileName))

    print "sample = %s: found %i input files." % (sampleToAnalyze, len(haddInputFileNames))

    haddShellFileName  = os.path.join(outputFilePath, 'harvestJRAtauNtuples_%s.csh' % sampleToAnalyze)
    haddOutputFileName = os.path.join(inputFilePath, 'ntupleJRAtau_%s_all.root' % sampleToAnalyze)

    retVal_hadd = \
      buildConfigFile_hadd(executable_hadd, haddShellFileName, haddInputFileNames, haddOutputFileName)

    fileNames_hadd[sampleToAnalyze] = {}
    fileNames_hadd[sampleToAnalyze]['shellFileName']  = haddShellFileName
    fileNames_hadd[sampleToAnalyze]['inputFileNames'] = haddInputFileNames
    fileNames_hadd[sampleToAnalyze]['outputFileName'] = haddOutputFileName
    fileNames_hadd[sampleToAnalyze]['logFileName']    = retVal_hadd['logFileName']
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
fileNames_and_options_jrAnalyzer = {}

ptBinning = [
    20., 22.5, 25., 27.5, 30., 35., 40., 45., 50., 60., 80., 120., 200.
]

etaBinning = [
    -2.5, -2.3, -2.1, -1.9, -1.7, -1.5, -1.3, -1.1, -0.9, -0.7, -0.5, -0.3, -0.1,
    +0.1, +0.3, +0.5, +0.7, +0.9, +1.1, +1.3, +1.5, +1.7, +1.9, +2.1, +2.3, +2.5
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

for sampleToAnalyze in samplesToAnalyze:
    fileNames_and_options_jrAnalyzer[sampleToAnalyze] = {}    
    fileNames_and_options_jrAnalyzer[sampleToAnalyze]['inputFileNames'] = \
      [ fileNames_hadd[sampleToAnalyze]['outputFileName'] ]
    fileNames_and_options_jrAnalyzer[sampleToAnalyze]['outputFileName'] = \
      os.path.join(outputFilePath, "histogramsJRAtau_%s.root" % sampleToAnalyze)
    fileNames_and_options_jrAnalyzer[sampleToAnalyze]['configFileName'] = \
      os.path.join(outputFilePath, "jet_response_analyzer_%s.cfg" % sampleToAnalyze)
    make_jrAnalyzer_config(fileNames_and_options_jrAnalyzer[sampleToAnalyze]['configFileName'])
    fileNames_and_options_jrAnalyzer[sampleToAnalyze]['logFileName']    = \
      os.path.join(outputFilePath, "jet_response_analyzer_%s.log" % sampleToAnalyze)
    fileNames_and_options_jrAnalyzer[sampleToAnalyze]['commandLine']    = \
      '%s -input %s -output %s -algs %s' % \
        (fileNames_and_options_jrAnalyzer[sampleToAnalyze]['configFileName'],
         make_MakeFile_vstring(fileNames_and_options_jrAnalyzer[sampleToAnalyze]['inputFileNames']),
         fileNames_and_options_jrAnalyzer[sampleToAnalyze]['outputFileName'],
         "".join([ "%s:0.3 " % algorithm for algorithm in algorithms ]))
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# initialize command-line parameters for fitting jet response for uncalibrated tau-jets
#
fileNames_and_options_fitResponse_uncalibrated = {}

for sampleToAnalyze in samplesToAnalyze:
    fileNames_and_options_fitResponse_uncalibrated[sampleToAnalyze] = {}
    fileNames_and_options_fitResponse_uncalibrated[sampleToAnalyze]['inputFileNames'] = \
      [ fileNames_and_options_jrAnalyzer[sampleToAnalyze]['outputFileName'] ]
    fileNames_and_options_fitResponse_uncalibrated[sampleToAnalyze]['outputFileName'] = \
      os.path.join(outputFilePath, "responseJRAtau_%s.root" % sampleToAnalyze) 
    fileNames_and_options_fitResponse_uncalibrated[sampleToAnalyze]['logFileName']    = \
      os.path.join(outputFilePath, "jet_response_fitter_%s.log" % sampleToAnalyze)
    fileNames_and_options_fitResponse_uncalibrated[sampleToAnalyze]['commandLine']    = \
      '-input %s -output %s -algs %s -fittype %i' % \
        (make_MakeFile_vstring(fileNames_and_options_fitResponse_uncalibrated[sampleToAnalyze]['inputFileNames']),
         fileNames_and_options_fitResponse_uncalibrated[sampleToAnalyze]['outputFileName'],
         make_MakeFile_vstring(algorithms),
         fitOption)
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# initialize command-line parameters for determining L2 and L3 correction parameters
#
fileNames_and_options_fitL3param = {}
fileNames_and_options_fitL2param = {}

for sampleToAnalyze in samplesToAnalyze:
    fileNames_and_options_fitL3param[sampleToAnalyze] = {}
    fileNames_and_options_fitL3param[sampleToAnalyze]['inputFileNames'] = \
      [ fileNames_and_options_fitResponse_uncalibrated[sampleToAnalyze]['outputFileName'] ]
    fileNames_and_options_fitL3param[sampleToAnalyze]['outputFileName'] = \
      os.path.join(outputFilePath, "fitL3param_%s.root" % sampleToAnalyze)
    fileNames_and_options_fitL3param[sampleToAnalyze]['logFileName']    = \
      os.path.join(outputFilePath, "fitL3param_%s.log" % sampleToAnalyze)
    fileNames_and_options_fitL3param[sampleToAnalyze]['commandLine']    = \
      '-input %s -output %s -era %s -algs %s -formats png -batch true' % \
        (make_MakeFile_vstring(fileNames_and_options_fitL3param[sampleToAnalyze]['inputFileNames']),
         fileNames_and_options_fitL3param[sampleToAnalyze]['outputFileName'],
         era,
         make_MakeFile_vstring(algorithms))
    
    fileNames_and_options_fitL2param[sampleToAnalyze] = {}
    fileNames_and_options_fitL2param[sampleToAnalyze]['inputFileNames'] = \
      [ fileNames_and_options_fitResponse_uncalibrated[sampleToAnalyze]['outputFileName'],
        fileNames_and_options_fitL3param[sampleToAnalyze]['outputFileName'] ]
    fileNames_and_options_fitL2param[sampleToAnalyze]['outputFileName'] = \
      os.path.join(outputFilePath, "fitL2param_%s.root" % sampleToAnalyze)
    fileNames_and_options_fitL2param[sampleToAnalyze]['logFileName']    = \
      os.path.join(outputFilePath, "fitL2param_%s.log" % sampleToAnalyze)
    fileNames_and_options_fitL2param[sampleToAnalyze]['commandLine']    = \
      '-input %s -l3input %s -output %s -era %s -algs %s -formats png -batch true' % \
        (fileNames_and_options_fitResponse_uncalibrated[sampleToAnalyze]['outputFileName'],
         fileNames_and_options_fitL3param[sampleToAnalyze]['outputFileName'],
         fileNames_and_options_fitL2param[sampleToAnalyze]['outputFileName'],
         era,
         make_MakeFile_vstring(algorithms))
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# initialize command-line parameters for applying L2/L3 correction parameters
# and producing new "plain" ROOT Ntuples for calibrated tau-jets
#
fileNames_and_options_applyL2L3param = {}

for sampleToAnalyze in samplesToAnalyze:
    fileNames_and_options_applyL2L3param[sampleToAnalyze] = {}
    fileNames_and_options_applyL2L3param[sampleToAnalyze]['inputFileNames'] = \
      [ fileNames_and_options_jrAnalyzer[sampleToAnalyze]['outputFileName'],
        fileNames_and_options_fitL3param[sampleToAnalyze]['outputFileName'],
        fileNames_and_options_fitL2param[sampleToAnalyze]['outputFileName'] ]
    fileNames_and_options_applyL2L3param[sampleToAnalyze]['outputFileName'] = \
      os.path.join(outputFilePath, "applyL2L3param_%s.root" % sampleToAnalyze)
    fileNames_and_options_applyL2L3param[sampleToAnalyze]['logFileName']    = \
      os.path.join(outputFilePath, "applyL2L3param_%s.log" % sampleToAnalyze)
    fileNames_and_options_applyL2L3param[sampleToAnalyze]['commandLine']    = \
      '-input %s -output %s -era %s -algs %s -jecpath %s -levels 2 3' % \
        (fileNames_hadd[sampleToAnalyze]['outputFileName'],
         fileNames_and_options_applyL2L3param[sampleToAnalyze]['outputFileName'],
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

for sampleToAnalyze in samplesToAnalyze:
    fileNames_and_options_jrAnalyzer_calibrated[sampleToAnalyze] = {}    
    fileNames_and_options_jrAnalyzer_calibrated[sampleToAnalyze]['inputFileNames'] = \
      [ fileNames_and_options_applyL2L3param[sampleToAnalyze]['outputFileName'] ]
    fileNames_and_options_jrAnalyzer_calibrated[sampleToAnalyze]['outputFileName'] = \
      os.path.join(outputFilePath, "histogramsJRAtau_%s_calibrated.root" % sampleToAnalyze)
    fileNames_and_options_jrAnalyzer_calibrated[sampleToAnalyze]['configFileName'] = \
      os.path.join(outputFilePath, "jet_response_analyzer_%s_calibrated.cfg" % sampleToAnalyze)
    make_jrAnalyzer_config(fileNames_and_options_jrAnalyzer_calibrated[sampleToAnalyze]['configFileName'])
    fileNames_and_options_jrAnalyzer_calibrated[sampleToAnalyze]['logFileName']    = \
      os.path.join(outputFilePath, "jet_response_analyzer_%s_calibrated.log" % sampleToAnalyze)
    fileNames_and_options_jrAnalyzer_calibrated[sampleToAnalyze]['commandLine']    = \
      '%s -input %s -output %s -algs %s' % \
        (fileNames_and_options_jrAnalyzer_calibrated[sampleToAnalyze]['configFileName'],
         make_MakeFile_vstring(fileNames_and_options_jrAnalyzer_calibrated[sampleToAnalyze]['inputFileNames']),
         fileNames_and_options_jrAnalyzer_calibrated[sampleToAnalyze]['outputFileName'],
         make_MakeFile_vstring([ "".join([ algorithm, suffix]) for algorithm in algorithms for suffix in [ "", "l2l3"] ]))
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# initialize command-line parameters for fitting jet response and resolution
# for calibrated as well as uncalibrated tau-jets
#
fileNames_and_options_fitResponse_calibrated = {}

fileNames_and_options_fitResolution = {}

for sampleToAnalyze in samplesToAnalyze:
    fileNames_and_options_fitResponse_calibrated[sampleToAnalyze] = {}
    fileNames_and_options_fitResponse_calibrated[sampleToAnalyze]['inputFileNames'] = \
      [ fileNames_and_options_jrAnalyzer_calibrated[sampleToAnalyze]['outputFileName'] ]
    fileNames_and_options_fitResponse_calibrated[sampleToAnalyze]['outputFileName'] = \
      os.path.join(outputFilePath, "responseJRAtau_%s_calibrated.root" % sampleToAnalyze) 
    fileNames_and_options_fitResponse_calibrated[sampleToAnalyze]['logFileName']    = \
      os.path.join(outputFilePath, "jet_response_fitter_%s_calibrated.log" % sampleToAnalyze)
    fileNames_and_options_fitResponse_calibrated[sampleToAnalyze]['commandLine']    = \
      '-input %s -output %s -algs %s -fittype %i' % \
        (make_MakeFile_vstring(fileNames_and_options_fitResponse_calibrated[sampleToAnalyze]['inputFileNames']),
         fileNames_and_options_fitResponse_calibrated[sampleToAnalyze]['outputFileName'],
         make_MakeFile_vstring([ "".join([ algorithm, suffix]) for algorithm in algorithms for suffix in [ "", "l2l3"] ]),
         fitOption)

    fileNames_and_options_fitResolution[sampleToAnalyze] = {}
    fileNames_and_options_fitResolution[sampleToAnalyze]['inputFileNames'] = \
      [ fileNames_and_options_fitResponse_calibrated[sampleToAnalyze]['outputFileName'] ]
    fileNames_and_options_fitResolution[sampleToAnalyze]['outputFileName'] = \
      os.path.join(outputFilePath, "resolutionJRAtau_%s.root" % sampleToAnalyze)
    fileNames_and_options_fitResolution[sampleToAnalyze]['logFileName']    = \
      os.path.join(outputFilePath, "jet_resolution_fitter_%s.log" % sampleToAnalyze)
    fileNames_and_options_fitResolution[sampleToAnalyze]['commandLine']    = \
      '-input %s -output %s -algs %s -dorelrsp true -doetarsp true -docbfits true' % \
        (make_MakeFile_vstring(fileNames_and_options_fitResolution[sampleToAnalyze]['inputFileNames']),
         fileNames_and_options_fitResolution[sampleToAnalyze]['outputFileName'],
         make_MakeFile_vstring([ "".join([ algorithm, suffix]) for algorithm in algorithms for suffix in [ "", "l2l3"] ]))
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# initialize command-line parameters for making jet response plots for  calibrated tau-jets
# plus resolution plots for calibrated compared to uncalibrated tau-jets
#
fileNames_and_options_showHistos = {}
fileNames_and_options_showGraphs = {}

for sampleToAnalyze in samplesToAnalyze:

    fileNames_and_options_showHistos[sampleToAnalyze] = {}
    fileNames_and_options_showGraphs[sampleToAnalyze] = {}

    for algorithm in algorithms:

        outputFilePath_plots_algorithm = os.path.join(outputFilePath_plots, algorithm)
        if not os.path.exists(outputFilePath_plots_algorithm):
            os.mkdir(outputFilePath_plots_algorithm)
        
        fileNames_and_options_showHistos[sampleToAnalyze][algorithm] = {}
        for refVariable in [ "RefPt", "JetEta" ]:
            fileNames_and_options_showHistos[sampleToAnalyze][algorithm][refVariable] = {}
            fileNames_and_options_showHistos[sampleToAnalyze][algorithm][refVariable]['inputFileNames'] = \
              [ fileNames_and_options_fitResponse_calibrated[sampleToAnalyze]['outputFileName'] ]
            fileNames_and_options_showHistos[sampleToAnalyze][algorithm][refVariable]['outputFileName'] = \
              "make_jet_inspect_histos_target_%s_%s_%s_calibrated" % (sampleToAnalyze, algorithm, refVariable)
            fileNames_and_options_showHistos[sampleToAnalyze][algorithm][refVariable]['logFileName']    = \
              os.path.join(outputFilePath, "jet_inspect_histos_%s_%s_%s_calibrated.log" % (sampleToAnalyze, algorithm, refVariable))
            fileNames_and_options_showHistos[sampleToAnalyze][algorithm][refVariable]['commandLine']    = \
              '-inputs %s -algs %s -variables RelRsp:%s %s -formats png -batch true -opath %s' % \
                (make_MakeFile_vstring(fileNames_and_options_showHistos[sampleToAnalyze][algorithm][refVariable]['inputFileNames']),
                 make_MakeFile_vstring([ "".join([ algorithm, suffix]) for suffix in [ "", "l2l3"] ]),
                 refVariable,
                 "-norm true -npercanvas 1",
                 outputFilePath_plots_algorithm)
    
        fileNames_and_options_showGraphs[sampleToAnalyze][algorithm] = {}
        for refVariable in [ "RefPt", "JetEta" ]:
            fileNames_and_options_showGraphs[sampleToAnalyze][algorithm][refVariable] = {}
            fileNames_and_options_showGraphs[sampleToAnalyze][algorithm][refVariable]['inputFileNames'] = \
              [ fileNames_and_options_fitResolution[sampleToAnalyze]['outputFileName'] ]
            fileNames_and_options_showGraphs[sampleToAnalyze][algorithm][refVariable]['outputFileName'] = \
              "make_jet_inspect_graphs_%s_%s_%s_target" % (sampleToAnalyze, algorithm, refVariable)
            fileNames_and_options_showGraphs[sampleToAnalyze][algorithm][refVariable]['logFileName']    = \
              os.path.join(outputFilePath, "jet_inspect_graphs_%s_%s_%s.log" % (sampleToAnalyze, algorithm, refVariable))
            fileNames_and_options_showGraphs[sampleToAnalyze][algorithm][refVariable]['commandLine']    = \
              '-inputs %s -algs %s -variables RelRspVs%s %s -formats png -batch true -opath %s' % \
                (make_MakeFile_vstring(fileNames_and_options_showGraphs[sampleToAnalyze][algorithm][refVariable]['inputFileNames']),
                 make_MakeFile_vstring([ "".join([ algorithm, suffix]) for suffix in [ "", "l2l3"] ]),
                 refVariable,
                 "-ymin 0.5 -ymax 1.5 -legx 0.20 -legy 0.35 -legw 0.60",
                 outputFilePath_plots_algorithm)
#--------------------------------------------------------------------------------

# done building config files and initializing command-line parameters, now build Makefile...
makeFileName = "Makefile_runJRAtauworkflow_%s" % version
makeFile = open(makeFileName, "w")
makeFile.write("\n")
outputFileNames_runJRAtauworkflow = []
for sampleName in samplesToAnalyze:
    outputFileNames_runJRAtauworkflow.extend([
        fileNames_and_options_jrAnalyzer[sampleName]['outputFileName'],
        fileNames_and_options_fitResponse_uncalibrated[sampleName]['outputFileName'],
        fileNames_and_options_fitL3param[sampleName]['outputFileName'],
        fileNames_and_options_fitL2param[sampleName]['outputFileName'],
        fileNames_and_options_applyL2L3param[sampleName]['outputFileName'],
        fileNames_and_options_jrAnalyzer_calibrated[sampleName]['outputFileName'],
        fileNames_and_options_fitResponse_calibrated[sampleName]['outputFileName'],
        fileNames_and_options_fitResolution[sampleName]['outputFileName']
    ])
    for algorithm in algorithms:
        for refVariable in [ "RefPt", "JetEta" ]:
            outputFileNames_runJRAtauworkflow.extend([
                fileNames_and_options_showHistos[sampleName][algorithm][refVariable]['outputFileName'],
                fileNames_and_options_showGraphs[sampleName][algorithm][refVariable]['outputFileName']
            ])
makeFile.write("all: %s\n" % make_MakeFile_vstring(outputFileNames_runJRAtauworkflow))
makeFile.write("\techo 'Finished running JRAtau Workflow.'\n")
makeFile.write("\n")
for sampleName in samplesToAnalyze:
    if len(fileNames_hadd[sampleName]['inputFileNames']) > 0:
        makeFile.write("%s: %s\n" %
          (fileNames_hadd[sampleName]['outputFileName'],
           make_MakeFile_vstring(fileNames_hadd[sampleName]['inputFileNames'])))
        makeFile.write("\t%s %s &> %s\n" %
          (executable_shell,
           fileNames_hadd[sampleName]['shellFileName'],
           fileNames_hadd[sampleName]['logFileName']))
makeFile.write("\n")
for sampleName in samplesToAnalyze:
    makeFile.write("%s: %s\n" %
      (fileNames_and_options_jrAnalyzer[sampleName]['outputFileName'],
       make_MakeFile_vstring(fileNames_and_options_jrAnalyzer[sampleName]['inputFileNames'])))    
    makeFile.write("\t%s %s &> %s\n" %
      (executable_jrAnalyzer,
       fileNames_and_options_jrAnalyzer[sampleName]['commandLine'],
       fileNames_and_options_jrAnalyzer[sampleName]['logFileName']))
makeFile.write("\n")
for sampleName in samplesToAnalyze:
    makeFile.write("%s: %s\n" %
      (fileNames_and_options_fitResponse_uncalibrated[sampleName]['outputFileName'],
       make_MakeFile_vstring(fileNames_and_options_fitResponse_uncalibrated[sampleName]['inputFileNames'])))    
    makeFile.write("\t%s %s &> %s\n" %
      (executable_fitResponse,
       fileNames_and_options_fitResponse_uncalibrated[sampleName]['commandLine'],
       fileNames_and_options_fitResponse_uncalibrated[sampleName]['logFileName']))
makeFile.write("\n")
for sampleName in samplesToAnalyze:
    makeFile.write("%s: %s\n" %
      (fileNames_and_options_fitL3param[sampleName]['outputFileName'],
       make_MakeFile_vstring(fileNames_and_options_fitL3param[sampleName]['inputFileNames'])))    
    makeFile.write("\t%s %s &> %s\n" %
      (executable_fitL3param,
       fileNames_and_options_fitL3param[sampleName]['commandLine'],
       fileNames_and_options_fitL3param[sampleName]['logFileName']))
    makeFile.write("%s: %s\n" %
      (fileNames_and_options_fitL2param[sampleName]['outputFileName'],
       make_MakeFile_vstring(fileNames_and_options_fitL2param[sampleName]['inputFileNames'])))    
    makeFile.write("\t%s %s &> %s\n" %
      (executable_fitL2param,
       fileNames_and_options_fitL2param[sampleName]['commandLine'],
       fileNames_and_options_fitL2param[sampleName]['logFileName']))
makeFile.write("\n")
for sampleName in samplesToAnalyze:
    makeFile.write("%s: %s\n" %
      (fileNames_and_options_applyL2L3param[sampleName]['outputFileName'],
       make_MakeFile_vstring(fileNames_and_options_applyL2L3param[sampleName]['inputFileNames'])))    
    makeFile.write("\t%s %s &> %s\n" %
      (executable_applyL2L3param,
       fileNames_and_options_applyL2L3param[sampleName]['commandLine'],
       fileNames_and_options_applyL2L3param[sampleName]['logFileName']))
makeFile.write("\n")
for sampleName in samplesToAnalyze:
    makeFile.write("%s: %s\n" %
      (fileNames_and_options_jrAnalyzer_calibrated[sampleName]['outputFileName'],
       make_MakeFile_vstring(fileNames_and_options_jrAnalyzer_calibrated[sampleName]['inputFileNames'])))    
    makeFile.write("\t%s %s &> %s\n" %
      (executable_jrAnalyzer,
       fileNames_and_options_jrAnalyzer_calibrated[sampleName]['commandLine'],
       fileNames_and_options_jrAnalyzer_calibrated[sampleName]['logFileName']))    
makeFile.write("\n")
for sampleName in samplesToAnalyze:
    makeFile.write("%s: %s\n" %
      (fileNames_and_options_fitResponse_calibrated[sampleName]['outputFileName'],
       make_MakeFile_vstring(fileNames_and_options_fitResponse_calibrated[sampleName]['inputFileNames'])))    
    makeFile.write("\t%s %s &> %s\n" %
      (executable_fitResponse,
       fileNames_and_options_fitResponse_calibrated[sampleName]['commandLine'],
       fileNames_and_options_fitResponse_calibrated[sampleName]['logFileName']))
    makeFile.write("%s: %s\n" %
      (fileNames_and_options_fitResolution[sampleName]['outputFileName'],
       make_MakeFile_vstring(fileNames_and_options_fitResolution[sampleName]['inputFileNames'])))    
    makeFile.write("\t%s %s &> %s\n" %
      (executable_fitResolution,
       fileNames_and_options_fitResolution[sampleName]['commandLine'],
       fileNames_and_options_fitResolution[sampleName]['logFileName']))
makeFile.write("\n")
for sampleName in samplesToAnalyze:
    for algorithm in algorithms:
        for refVariable in [ "RefPt", "JetEta" ]:
            makeFile.write("%s: %s\n" %
              (fileNames_and_options_showHistos[sampleName][algorithm][refVariable]['outputFileName'],
               make_MakeFile_vstring(fileNames_and_options_showHistos[sampleName][algorithm][refVariable]['inputFileNames'])))    
            makeFile.write("\t%s %s &> %s\n" %
              (executable_showHistos,
               fileNames_and_options_showHistos[sampleName][algorithm][refVariable]['commandLine'],
               fileNames_and_options_showHistos[sampleName][algorithm][refVariable]['logFileName']))
            makeFile.write("%s: %s\n" %
              (fileNames_and_options_showGraphs[sampleName][algorithm][refVariable]['outputFileName'],
               make_MakeFile_vstring(fileNames_and_options_showGraphs[sampleName][algorithm][refVariable]['inputFileNames'])))    
            makeFile.write("\t%s %s &> %s\n" %
              (executable_showGraphs,
               fileNames_and_options_showGraphs[sampleName][algorithm][refVariable]['commandLine'],
               fileNames_and_options_showGraphs[sampleName][algorithm][refVariable]['logFileName']))       
makeFile.write("\n")        
makeFile.write(".PHONY: clean\n")
makeFile.write("clean:\n")
makeFile.write("\trm -f %s\n" % make_MakeFile_vstring(outputFileNames_runJRAtauworkflow))
makeFile.write("\techo 'Finished deleting old files.'\n")
makeFile.write("\n")
makeFile.close()

print("Finished building Makefile. Now execute 'make -j 8 -f %s'." % makeFileName)

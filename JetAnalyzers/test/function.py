from variable import *

#--------------------------------------------------------------------------------
#
# function to add root files
#
def buildConfigFile_hadd(haddCommand, shellFileName_full, inputFileNames, outputFileName_full):

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
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# function to make seperate string from a vector of string
#
def make_MakeFile_vstring(list_of_strings):
    retVal = ""
    for i, string_i in enumerate(list_of_strings):
        if i > 0:
            retVal += " "
        retVal += string_i
    return retVal
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# function to make seperate double from a vector of double
#
def make_MakeFile_vdouble(list_of_doubles):
    retVal = ""
    for i, double_i in enumerate(list_of_doubles):
        if i > 0:
            retVal += " "
        retVal += "%2.1f" % double_i
    return retVal
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
#
# pt and eta binning 
#
def make_jrAnalyzer_config(configFileName):
    configFile = open(configFileName, "w")
    configFile.write("drmax = 0.3\n")
    configFile.write("etabarrelmin = -1.3\n")
    configFile.write("etabarrelmax =  1.3\n")
    configFile.write("binspt = %s\n" % make_MakeFile_vdouble(ptBinning))
    configFile.write("binseta = %s\n" % make_MakeFile_vdouble(etaBinning))
    configFile.close()
#--------------------------------------------------------------------------------


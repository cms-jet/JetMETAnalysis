JetMETAnalysis
==============
<!-- MarkdownTOC depth=0 -->

- [Overview](#overview)
- [Install & Run](#install--run)
	- [Standard](#standard)
	- [Standalone](#standalone)
- [Example Workflow](#example-workflow)
- [Beginner's Guide](#beginners-guide)
	- [Making an ntuple](#JetResponseAnalyzer.cc)
	- [Making the response histograms](#jet_response_analyzer_x.cc)
	- [Creating the L2Relative \(L2L3\) corrections](#jet_l2_correction_x.cc)
	- [Make validation plots](#make-validation-plots)
	- [Apply newly created JEC](#apply-newly-created-jec)
		- [On-the-fly](#jet_apply_jec_x.cc)
		- [Using the CMSSW framework](#using-the-cmssw-framework)
- [Presentations](#presentations)
- [Related Links](#related-links)
- [Status](#status)

<!-- /MarkdownTOC -->


<a name="overview"></a>
## Overview
The code contained in this package is used for creating and analyzing the L1FastJet, L2Relative, and L3Absolute MC truth jet energy corrections (JEC). The code is used by the Jet Energy Resolution and Corrections (JERC) subgroup.

[JetResponseAnalyzer](https://github.com/cms-jet/JetMETAnalysis) is a CMSSW framework analyzer module which records the information necessary for jet response and resolution measurements based on matching (to MC truth information) or balancing (applicable to data, e.g. Z+jet). The main goal of the package is to use MC truth information and jet matching techniques to create proper jet energy calibrations. The information from the main EDAnalyzer is saved in a ROOT tree, and can later be turned into a set of histograms using the [jet_response_analyzer_x](https://github.com/cms-jet/JetMETAnalysis/blob/master/JetAnalyzers/bin/jet_response_analyzer_x.cc) standalone program. [JetResponseAnalyzer](https://github.com/cms-jet/JetMETAnalysis) and all related tools can be found in this GitHub package, which includes the [JetAnalyzers](https://github.com/cms-jet/JetMETAnalysis/tree/master/JetAnalyzers) and [JetUtilities](https://github.com/cms-jet/JetMETAnalysis/tree/master/JetUtilities) subfolders. Many of the provided tools in [JetAnalyzers](https://github.com/cms-jet/JetMETAnalysis/tree/master/JetAnalyzers) rely on additional utilities provided by [JetUtilities](https://github.com/cms-jet/JetMETAnalysis/tree/master/JetUtilities). While these tools were originally written to support the [JetAnalyzers](https://github.com/cms-jet/JetMETAnalysis/tree/master/JetAnalyzers) code, some of them can be used outside of this framework (i.e. CommandLine tools).

Due to the switch between CVS and GitHub, the history of this package is somewhat fragmented. The ChangeLog files contained in each subfolder contain much of the early CVS history. Then the folders were included as separate GitHub repositories ([JetAnalyzers](https://github.com/cms-jet/JetAnalyzers) and [JetUtilities](https://github.com/cms-jet/JetUtilities)) before being combined in this repository. By merging the history of this repository with those of the separate GitHub repos, one will obtain a complete history of this software.

All related tools and utilities are described in the various README files ([JetAnalyzers](JetAnalyzers/README.md), [JetAnalyzers/bin](JetAnalyzers/bin/README.md), [JetUtilities/bin](JetUtilities/bin/README.md)). An older version of this information is contained in this [twiki](https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideJetResponseAnalyzer?redirectedfrom=CMS.SWGuideJetResponseAnalyzer) page. Additionally, an old, but still useful, analysis note ([AN-13-131](http://cms.cern.ch/iCMS/jsp/openfile.jsp?tp=draft&files=AN2013_131_v3.pdf)) was written which contains a step-by-step guide on how to run the software.

<a name="install--run"></a>
## Install & Run
Besides the standard installation using scram, the parts necessary to run the executables (in JetMETAnalysis/JetAnalyzers/bin) to evaluate the output of the analyzer can also be compiled standalone on any PC or Mac with a ROOT installation. Still, one would need to remove any part of the EDM framework which will depend on CMSSW.

<a name="standard"></a>
### Standard
The standard way to build the plugins, libraries, and executable programs provided by JetMETAnalysis/JetAnalyzers and JetMETAnalysis/JetUtilities is using scram, which was last tested to work with the CMSSW_9_2_X_2017-05-17-2300 IB, but it should also work with any of the 81X, 82X, 91X, or 92X CMSSW releases:

```
setenv SCRAM_ARCH slc6_amd64_gcc530
cmsrel CMSSW_9_2_X
cd CMSSW_9_2_X/src
cmsenv
git-cms-init
git clone git@github.com:cms-jet/JetMETAnalysis.git
scram b -j 4
```

To test your installation, run the standard configuration to produce and trees respectively, than create another file with histograms using [jet_response_analyzer_x](https://github.com/cms-jet/JetMETAnalysis/blob/master/JetAnalyzers/bin/jet_response_analyzer_x.cc), then inspect for both the relative response as a function of reference (GenJet) pT:

```
# run CMSSW job to create one tree per algorithm (NOTE: adjust global tag appropriately!!)
cmsRun JetMETAnalysis/JetAnalyzers/test/run_JRA_cfg.py

# make histograms from trees
jet_response_analyzer_x JetMETAnalysis/JetAnalyzers/config/jra_dr.config -input JRA.root

# inspect histograms created with jet_response_analyzer_x
jet_inspect_histos_x -input jra.root
```

<a name="standalone"></a>
### Standalone
Many of the tools to analyze the output of the [JetResponseAnalyzer](https://github.com/cms-jet/JetMETAnalysis) module, a ROOT file containing a tree for each analyzed jet collection, do not depend on CMSSW but only on ROOT. It is therefore possible to install, compile, and run the executable applications on any PC or Mac with a current ROOT installation (without CMSSW or SCRAM installed):

```
cd /path/to/work
mkdir STANDALONE; cd STANDALONE
setenv STANDALONE_DIR ${PWD}
setenv ROOTSYS /path/to/your/root
setenv PATH $ROOTSYS/bin:$STANDALONE_DIR/bin:${PATH}
setenv LD_LIBRARY_PATH $ROOTSYS/lib:$STANDALONE_DIR/lib
setenv CVSROOT user@cmscvs.cern.ch:/cvs_server/repositories/CMSSW
# this link should be the same as above under 'Standard', if not I probably forgot to adjust
git clone git@github.com:cms-jet/JetMETAnalysis.git
cd JetUtilities; make; cd ..
cd JetAnalyzers; make; cd ..
rehash
jet_inspect_histos_x -input /path/to/jra.root
```

**NOTE:** These standalone directions haven't been updated or tested in some time. Since this package is mainly used within a CMSSW environment we cannot guarantee that this recipe still works.

<a name="example-workflow"></a>
## Example Workflow
This is an example of a simple workflow. It will not cover the use of all tools nor every conceivable method of validating the JEC.

1. JetResponseAnalyzer.cc (to produce an ntuple)
2. jet\_synchtest\_x
3. jet\_synchplot\_x (validation plots) + jet\_synchfit\_x (L1FastJet corrections)
4. jet\_synchtets\_x (apply JEC on-the-fly)
5. jet\_synchplot\_x (validation plots after applying JEC)
6. JetResponseAnalyzer.cc or jet\_apply\_jec\_x (name a new L1 corrected ntuple)
7. jet\_response\_analyzer\_x
8. (optional) jet\_response\_fitter\_x
9. jet\_l2\_correction\_x
10. (optional) jet\_apply\_jec\_x
11. jet\_correction\_analyzer\_x
12. jet\_draw\_closure\_x
13. (optional) jet\_draw\_corrections\_x

<a name="beginners-guide"></a>
## Beginner's Guide
The purpose of this document is to act as a beginner’s guide to the JetMETAnalysis software package. By the end, the reader should be able to check out, compile, and run a basic workflow using this package. Please take into account that this guide is not mean to prepare the user for all eventualities and workflows. There will undoubtedly be cases, specific to each user, which cannot be accounted for. If you, the reader, have any corrections or suggestions on how to improve this document, please contact the author.

<a name="JetResponseAnalyzer.cc"></a>
### Making an ntuple [JetResponseAnalyzer.cc]
The piece of code responsible for making the ntuple is [JetResponseAnalyzer.cc](https://github.com/cms-jet/JetMETAnalysis/blob/master/JetAnalyzers/src/JetResponseAnalyzer.cc). It is a standard EDAnalyzer and is meant to be run in conjunction with cmsRun/CRAB. The example configuration files for this analyzer is [run_JRA_cfg.py](https://github.com/cms-jet/JetMETAnalysis/blob/master/JetAnalyzers/test/run_JRA_cfg.py). In this configuration file there are several options which one shoudl be aware of, all of which are discussed in another [README](JetAnalyzers/README.md) file. 

1. As a first step you will want to confirm all of the configurable parameters in [run_JRA_cfg.py](https://github.com/cms-jet/JetMETAnalysis/blob/master/JetAnalyzers/test/run_JRA_cfg.py), including the global tag, algorithms to be used, input pool files (if using cmsRun to test), maximum number of events (if using cmsRun to test), etc.

2. Next you will probably want to test that the python script and all the accompanying files work. Simply move into the test directory and use cmsRun to create a sample file using the EDAnalyzer. If all goes well, and the program completes successfully, you will have a new file called *JRA.root*. You can see a list of expected branches in [README](JetAnalyzers/README.md) along with the folder naming scheme.

```
cd JetMETAnalysis/JetAnalyzers/test/
cmsRun run_JRA_cfg.py
```

3. Once you are certain that the algorithms and corrections are behaving as you wish them to, you should submit a CRAB job in the usual fashion. CRAB3 templates are provided in the _test_ folder, along with a python script for making multiple configuration files in a single go.

	- *NOTE:* Many of the programs that access the ntuple have been updated to be xrootd compliant. The user is no longer required to hadd the CRAB output files together to make a single ntuple file. However, in some cases that may still be the prefered or more performant method.

<a name="jet_response_analyzer_x.cc"></a>
### Making the response histograms [jet_response_analyzer_x.cc]

This guide will explain, in the most basic terms, the necessary steps for deriving the L2Relative correction files. Since the L3Absolute corrections have been combined with the L2Relative corrections (single step), the L3Absolute text files don't change and only return a multiplicative factor of 1. Thus, they can be safely taken from a past set fo JEC.

The next step is to create another ROOT file named jra.root (notice that the file name is lowercase to distinguish it from the ntuple file). This file contains the histograms necessary for determining the relative response of the jets in a given eta and pTref bin. The output file will contain folders of algorithms, just like before. Each folder will contain many hundreds of histograms in 11 different types, but the most important one is the last one listed in the examples below.

| *Type* | *Variable*          | *Example name(s)*                         |
|:-------|:--------------------|:------------------------------------------|
| _TH1F_ | RefPt:RefPt         | RefPt_RefPt10to10.5                       |
| _TH1F_ | JetPt:RefPt         | JetPt_RefPt10to10.5                       |
| _TH1F_ | RefPt_Barrel:RefPt  | RefPt_Barrel_RefPt10to10.5                |
| _TH1F_ | JetPt_Barrel:RefPt  | JetPt_Barrel_RefPt10to10.5                |
| _TH1F_ | RelRsp:RefPt        | RelRsp_RefPt10to10.5                      |
| _TH1F_ | RelRsp_Barrel:RefPt | RelRsp_Barrel_RefPt10to10.5               |
| _TH1F_ | JetEta:JetEta       | JetEta_JetEta-5.191to-4.889               |
| _TH1F_ | RelRsp:JetEta       | RelRsp_JetEta-5.191to-4.889               |
| _TH1F_ | RefPt:JetEta:RefPt  | RefPt_JetEta-5.191to-4.889_RefPt10to10.5  |
| _TH1F_ | JetPt:JetEta:RefPt  | JetPt_JetEta-5.191to-4.889_RefPt10to10.5  |
| _TH1F_ | RelRsp:JetEta:RefPt | RelRsp_JetEta-5.191to-4.889_RefPt10to10.5 |

In order to create this file, follow these directions:
1. Move to your chosen working directory.
2. Run the [jet_response_analyzer_x](https://github.com/cms-jet/JetMETAnalysis/blob/master/JetAnalyzers/bin/jet_response_analyzer_x.cc) executable as described in the [JetAnalyzers/bin/README](JetAnalyzers/bin/README.md) file.

*NOTE:* If your CMSSW release cannot find the executable, it is most likely that it has simply not been linked (i.e. aliased) yet. Simply call _cmsenv_ again. If this does not work it is always possible to call the executable directly using the executable itself found in $CMSSW_RELEASE/bin/$SCRAM_ARCH/<executable name>.

<a name="jet_l2_correction_x.cc"></a>
### Creating the L2Relative (L2L3) corrections [jet_l2_correction_x.cc]
Traditionally, this stage would be used for creating the L2Relative corrections. However, as mentioned previously, the JEC group has moved to an un-factorized L2L3 MC truth correc- tion scheme. The L2 code, however, was easily modified to allow for this option. It is also backwards compatible, thus allowing for the JetResponseAnalyzer package to be adaptable.

Simply run the jet_l2_correction_x executable as described in the [JetAnalyzers/bin/README](JetAnalyzers/bin/README.md) file to create the L2L3 corrections. The first output of this executable will be a ROOT file named *l2.root*. It contains the actual TGraphErrors used for fitting the correction functions. This should be saved for later review. In addition, the program will make a text file for each algorithm following the naming scheme \<era\>_\<version\>\_MC\_L2Relative\_\<algorithm\>.txt.These are the text files which will be input into the SQLite file and ultimately uploaded into the global tags. These most certainly should be saved.

<a name="make-validation-plots"></a>
### Make validation plots
This section describes the programs used for validation and study of the previously created jet energy corrections. The purpose of this executable is to create and store all of the histograms necessary for the drawing and formatting executables. This program must be run once, and sometimes twice, in order to create all histograms for validation.

1. Run the [jet_correction_analyzer_x](https://github.com/cms-jet/JetMETAnalysis/blob/master/JetAnalyzers/bin/jet_correction_analyzer_x.cc) executable as described in the [JetAnalyzers/bin/README](JetAnalyzers/bin/README.md) file. The output of this executable will be a ROOT file named Closure\_\<algorithms\>.root (other output can be ignored for this simple guide). This file contains unformated response versus eta vs pT TH2Fs.
2. This next executable, [jet_draw_closure_x](https://github.com/cms-jet/JetMETAnalysis/blob/master/JetAnalyzers/bin/jet_draw_closure_x.cc), is responsible for creating nicely formatted Closure Vs. pTref plots. This will create a number of ROOT files, one for each algorithm, named ClosureVs\<independent variable\>\_\<algorithm\>.root. Each file contains a histogram for barrel, inner endcap, outer endcap, and forward eta region closure plots. if the _formats_ option is used then each of the canvases will also be turned into images.

<a name="apply-newly-created-jec"></a>
### Apply newly created JEC
At this point, the reader should now have a set of L2Relative and L3Absolute correction files. As these new corrections are not contained within a global tag, this section will explain to the reader how to make a new ntuple with these corrections applied to the jets. There are two methods to do this. One method corrects the jets on the fly while the other one uses the CMSSW framework to make a new ntuple with the corrected jets collection. Use of the second method is encouraged as it is closure in proceedure to how an analyzer will interact with the corrections. However, the first method is okay for quick checks and non-analysis level work.

<a name="jet_apply_jec_x.cc"></a>
#### On-the-fly [jet_apply_jec_x.cc]
This first method involves directly applying the corrections to the ROOT ntuple. We call this correcting the jets “on-the-fly”. The reader will need to collect the text files for any of the jet collections they wish to correct. The executable that will be run will create a new file containing folders and trees for the newly corrected algorithms. Simply run the [jet_apply_jec_x](https://github.com/cms-jet/JetMETAnalysis/blob/master/JetAnalyzers/bin/jet_apply_jec_x.cc) executable to create a new ROOT file with the input (uncorrected) and output (corrected) jet collections.

<a name="using-the-cmssw-framework"></a>
#### Using the CMSSW framework
The process discussed in this section will be almost identical to the one discussed when making the original ntuple. The first step will involve making an SQLite file. The next step will be to create a new ntuple with the fully corrected algorithms included. Really, the only new information will be how to create the SQLite file.

*Note:* The process described below makes use of some legacy code. The process is made much easier by the [DBUploadTools](https://github.com/cms-jet/DBUploadTools) repository and the [JECDatabase](https://github.com/cms-jet/JECDatabase) repository. The [DBUploadTools](https://github.com/cms-jet/DBUploadTools) repo has its own README which can guide you in making an identical SQLite file, but using a more automatic method.

1. Checkout the JetMETCorrections/Modules package:
```
git-cms-addpkg JetMETCorrections/Modules
git-cms-addpkg CondFormats/JetMETObjects
scram b -j 4
```

2. Place the newly created text files in the CondFormats/JetMETObjects/data folder. Make sure that all of the correction files that need to be in the SQLite file are in that folder and that they all have the naming scheme \<era\>\_\<version\>\_MC\_\<correction level\>\_\<algorithm\>.txt.
3. Go to the directory JetMETCorrections/Modules/test/ and open the file JetCorrectionDBWriter\_cfg.py for editing
4. Change the output SQLite filename
```
process.CondDBCommon.connect = ’sqlite_file:Fall12_V7_MC.db’
```
5. Every algorithm that has corrections in the SQLite file will need a set of lines like the following. Please note that the tag name needs the format JetCorrectorParametersCollection\_\<era\>\_MC\_\<algorithm\>.
```
cms.PSet(
    record = cms.string(’AK5Calo’),
    tag = cms.string(’
       JetCorrectorParametersCollection_Fall12_V7_MC_AK5Calo’),
    label = cms.string(’AK5Calo’)
),
```
6. Add each algorithm to the process.
```
process.dbWriterAK5Calo = cms.EDAnalyzer(’JetCorrectorDBWriter’,
    era = cms.untracked.string(’Fall12_V7_MC’),
    algo = cms.untracked.string(’AK5Calo’)
)
```
7. Make sure that only the algorithms desired in the SQL file are run. Comment out any other EDAnalyzer module. See below for an example.
```
process.p = cms.Path(
    process.dbWriterAK5Calo
    #process.dbWriterAK5PF *
    #process.dbWriterAK5PFchs *
    #process.dbWriterAK5JPT *
)
```
8. Run the EDAnalyzer using cmsRun. This will create the file Fall12\_V7\_MC.db and this is the SQLite file that you will need to move to JetMETAnalysis/JetAnalyzers/test to create the new ntuple.
9. (_Optional_) There are three ways in which to check which text files now reside within the SQL file. The first is by looking at the output of the cmsRun command. This is a good choice to see that the program picked up all of the necessary text files and no others. However, JetCorrectionDBWriter\_cfg.py will not overwrite an existing SQLite file, thus there is some room for mistakes to happen. Thus, it is recommended that the reader write out the text files from the newly created SQLite file. To do that, the reader will want to edit the file JetCorrectionLocalDBReader\_cfg.py with similar settings as were used in JetCorrectionDBWriter\_cfg.py. This should write out all of the text files contained in the SQLite file. Another option to simply check that all of the algorithms made it into the SQLite file is to use the _conddb_ tool provided by the AlCa group. However, this is beyond the scope of this README.

<a name="presentations"></a>
## Presentations
[First Look at Summer08 QCD Samples with JetResponseAnalyzer](https://indico.cern.ch/event/46778/contributions/1143300/attachments/951545/1350227/2008-12-04.jetalgs.jra.pdf)
- Author: Philipp Schieferdecker (KIT)
- December 4, 2008
- [Meeting Link](https://indico.cern.ch/event/46778/)

[Calibration, Resolution, Closure: JetResponseAnalyzer](https://indico.cern.ch/event/38581/contributions/1816062/attachments/770062/1056225/2008-07-29.jetalgs.jca.pdf)
- Author: Philipp Schieferdecker (KIT)
- July 29, 2008
- [Meeting Link](https://indico.cern.ch/event/38581/)

<a name="related-links"></a>
## Related Links
[JetMET](https://twiki.cern.ch/twiki/bin/view/CMS/JetMET)

[JetEnergyScale](https://twiki.cern.ch/twiki/bin/view/CMS/JetEnergyScale)

[JetAlgorithms](https://twiki.cern.ch/twiki/bin/view/CMS/JetMETAlgorithmsReconstruction)

[SWGuideTFileService](https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideTFileService)

<a name="status"></a>
## Status
Status of the master branch: 
[![Build Status](https://travis-ci.org/cms-jet/JetMETAnalysis.svg?branch=master)](https://travis-ci.org/cms-jet/JetMETAnalysis)

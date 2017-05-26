JetMETAnalysis
==============
<!-- MarkdownTOC depth=0-->

- [Overview](#overview)
- [Install & Run](#install--run)
	- [Standard](#standard)
	- [Standalone](#standalone)
- [Presentations](#presentations)
- [Related Links](#related-links)
- [Status](#status)

<!-- /MarkdownTOC -->


<a name="overview"></a>
## Overview
The code contained in this package is used for creating and analyzing the L1FastJet, L2Relative, and L3Absolute MC truth jet energy corrections (JEC). The code is used by the Jet Energy Resolution and Corrections (JERC) subgroup.

[JetResponseAnalyzer](https://github.com/cms-jet/JetMETAnalysis) is a CMSSW framework analyzer module which records the information necessary for jet response and resolution measurements based on matching (to MC truth information) or balancing (applicable to data, e.g. Z+jet). The information is saved in a ROOT tree, and can later be turned into a set of histograms using the [jet_response_analyzer_x](https://github.com/cms-jet/JetMETAnalysis/blob/master/JetAnalyzers/bin/jet_response_analyzer_x.cc) standalone program. [JetResponseAnalyzer](https://github.com/cms-jet/JetMETAnalysis) and all related tools can be found in this GitHub package, which includes the [JetAnalyzers](https://github.com/cms-jet/JetMETAnalysis/tree/master/JetAnalyzers) and [JetUtilities](https://github.com/cms-jet/JetMETAnalysis/tree/master/JetUtilities) subfolders. Many of the provided tools in [JetAnalyzers](https://github.com/cms-jet/JetMETAnalysis/tree/master/JetAnalyzers) rely on additional utilities provided by [JetUtilities](https://github.com/cms-jet/JetMETAnalysis/tree/master/JetUtilities). While these tools were originally written to support the [JetAnalyzers](https://github.com/cms-jet/JetMETAnalysis/tree/master/JetAnalyzers) code, some of them can be used outside of this framework (i.e. CommandLine tools).

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

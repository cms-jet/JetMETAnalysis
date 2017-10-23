JetResponseAnalyzer
===================
<!-- MarkdownTOC -->

- [Overview](#overview)
- [Configuration](#configuration)
	- [Setting Configuration Options for JetResponseAnalyzer Modules](#setting-configuration-options-for-jetresponseanalyzer-modules)
	- [Adding Several Jet Definitions to your Configuration](#adding-several-jet-definitions-to-your-configuration)
- [Output](#output)

<!-- /MarkdownTOC -->

<a name="overview"></a>
## Overview
These directions will cover the configuration and use of the [JetResponseAnalyzer](https://github.com/cms-jet/JetMETAnalysis/blob/master/JetAnalyzers/interface/JetResponseAnalyzer.hh) EDAnalyzer. A separate README file was written to explain the uses of the associate executables and tools.

<a name="configuration"></a>
## Configuration
Note that all of the details described below are handled by the [addAlgorithm.py](JetAnalyzers/python/addAlgorithm.py) function, see [test/run_JRA_cfg.py](JetAnalyzers/test/run_JRA_cfg.py) for the one line (!) necessary to add a specific algorithm, corrected to a specific level, to your configuration.

To configure the [JetResponseAnalyzer](https://github.com/cms-jet/JetMETAnalysis/blob/master/JetAnalyzers/interface/JetResponseAnalyzer.hh), you need to pick the input references and the input match-map associating these references with jets. You can run the analyzer in matching mode, requiring several references to be matched according to a deltaR requirement to several jets, or in balancing mode, requiring one reference per event to be back-to-back to one jet. The matching maps need to be constructed accordingly, and the respective mode is chosen by **either** providing the parameter *deltaRMax* (matching) or *deltaPhiMin* (balancing). The maps can be created with the plugins [MatchRecToGen](https://github.com/cms-jet/JetMETAnalysis/blob/master/JetUtilities/plugins/MatchRecToGen.cc) (matching) and [MatchBackToBack](https://github.com/cms-jet/JetMETAnalysis/blob/master/JetUtilities/plugins/MatchBackToBack.cc) (balancing) in the [JetMETAnalysis/JetUtilities](JetUtilities) package.

The flag *doFlavor* allows you to steer whether flavor should be considered. Some options only need to be provided if other parameters are set, see below. 

<a name="setting-configuration-options-for-jetresponseanalyzer-modules"></a>
### Setting Configuration Options for JetResponseAnalyzer Modules
[JetMETAnalysis/JetAnalyzers/test](JetAnalyzers/text) provides an example configuration, [run_JRA_cfg.py](JetAnalyzers/test/run_JRA_cfg.py), which may need to be modified based on your desired input files, jet algorithms, etc.

You override individual options towards the top of the run_JRA_cfg.py configuration file, these changes will then apply to all JRA configurations added below! Example:
```
Defaults.JetResponseParameters.doFlavor = False
```
will disable the parton matching.

The following table documents all available configuration options and their relations:

| **Parameter**             | **Type**   | **Description** |
|:--------------------------|:----------:|:----------------|
| _srcRef_                  | *InputTag* | Input collection containing the reference objects. Anything compatible with *edm::View< reco::Candidate>*. |
| _srcRefToJetMap_          | *InputTag* | Match map associating references to jets. It is up to the plugin creating this map how references are associated to jets: by matching or back-to-back balancing. For balancing, the map must only contain one entry per event, otherwise the event is skipped. |
| _nRefMax_                 | *uint32*   | By default (nRefMax=0), all references which are associated to jets via the map are considered. For nRefMax>0, only the nRefMax highest pT references are considered. |
| _deltaRMax_               | *double*   | Maximum deltaR separation of reference-jet pairs to be considered matched. **Provide this parameter only for MATCHING mode, don't provide deltaPhiMin!** |
| _deltaPhiMin_             | *double*   | Minimum deltaPhi separation of the reference-jet pair to be considered back-to-back. **Provide this parameter only for BALANCING mode, don't provide deltaRMax** |
| _etaBarrelMin_            | *double*   | Minimum eta for the barrel (central) calorimeter. **Provide only if doHistos=true!** |
| _etaBarrelMax_            | *double*   | Maximum eta for the barrel (central) calorimeter. **Provide only if doHistos=true!** |
| _doFlavor_                | *bool*     | Identify the flavor of references and create and additional set of histograms for uds, c, b, g each. Add =refpdgid= branch to the tree. |
| _srcRefToPartonMap_       | *InputTag* | By omitting / providing this parameter, the pdgId of the reference itself / the pdgId of the object (parton) associated with the reference via this map are considered to determine the reference flavor. **Only provide if doFlavor=true!** |
| _deltaRPartonMax_         | *double*   | Maximum separation in deltaR between the reference and the associated parton to consider the flavor of the reference. **Only provide if doFlavor=true AND srcRefToPartonMap is specified!**|
| _doJetPt_                 |  *bool*    | Distributions are only recorded in bins of pT(jet) if set to true. |
| _doRefPt_                 |  *bool*    | Distributions are only recorded in bins of pT(reference) if set to true. |

<a name="adding-several-jet-definitions-to-your-configuration"></a>
### Adding Several Jet Definitions to your Configuration 
You can add different jet definitions (e.g. different jet algorithms like AntiKt or kT, different reco types like CaloJets or PFJets, different jet correction levels like L2Absolute or L3Relative, etc.) by adding the line
```
addAlgorithm(process,'JETDEF',doReco)
```
to your configuration file, as demonstrated in [test/run_JRA_cfg.py](JetAnalyzers/test/run_JRA_cfg.py). 'doReco' is a boolean flag and indicates whether you would like to reconstruct (Gen)Jets yourself (True) or take them from the input file provided (False). It is recommended to set this to True, since e.g. PF particles will be appropriately preselected for PFJet reconstruction and MC particles will be appropriately filtered for e.g. neutrinos for GenJet reconstruction. If you want JPT jets, you have to provide True. The JETDEF string should be of the form AlgSizeType[Jec], where the following values are currently supported (perhaps not in all combinations):

  * *Alg*: ak (AntiKt), sc (SISCone), kt (kT), ca (Cambridge/Aachen), ic (IterativeCone)
  * *Size*: 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
  * *Type*: calo, pf, pfchs, puppi, jpt, trk, tau
  * *Jec*: \"\", l1, l2l3, l1l2l3, l1l2l3l5

All of the algorithm names start with the clustering algorithm abbreviation (i.e. “ak” for the Anti-KT algorithm or “kt” for the KT algorithm). The next part of the name should be a number, indicating the clustering cone size of the algorithm used (i.e. 4 indicates a cone size of R = 0.4). The third part of the name is the jet type. The possible jet types are calorimeter (calo), particle flow (pf), particle flow + charged hadron subtraction (pfchs), particle flow + pileup per particle identification (puppi), or jet plus track (jpt). Finally, the correction levels you want apply to each collection are added to the end of the name. For the L1 pileup corrections, you have the option of using l1 for the L1FastJet corrections or l1off for the L1Offset corrections. The rest of the labels should be self explanatory and for the rule l+ <level number> (i.e. l2l3 for L2Relative and L3Absolute MC truth corrections).

*NOTE:* All of this assumes that these corrections are either defined in the JetMETCorrections package or in the [JetCorrection_cff.py](https://github.com/cms-jet/JetMETAnalysis/blob/master/JetAnalyzers/python/JetCorrection_cff.py). A section on adding new correction may be written at another time if necessary.

[addAlgorithm.py](JetAnalyzers/python/addAlgorithm.py) will create the entire sequence (and the corresponding path) for each algorithm and add it to the *process*. You can inspect this sequence as follows (assuming that *ak5calo* is one of the jet definitions in your configuration):
```
python -i run_JRA_cfg.py
>>> process.ak5caloSequence
```
You can inspect every single component and its configuration that way using the python command line (don't forget to prepend *process.* every time!)

An alternative to specifying each AlgSizeType[Jec] combination individually is to provide a set of algorithms, jet cone sizes, jet types, and JEC levels you want to analyze as lists. As demonstrated in [test/run_JRA_cfg.py](JetAnalyzers/test/run_JRA_cfg.py), there will be a series of loops that will analyzer all the combinations of AlgSizeType, prepending the various JEC levels to this list of AlgSizeType. Each of these combinations will then get passes to the *addAlgorithm* function.

Another configurable parameter is the source of the JEC conditions. By default the option _conditionsSource_ is set to *"GT"*, which stands for _global tag_. However, one could also think about retrieving the conditions from an SQLite file (option *"SQLite"*) or a database (option *"DB"*). The later two options are useful when testing a set of JEC which have not been included in an official global tag. If using the SQLite option, make sure the era is set correctly as the era is the expected SQLite filename (without the extension). No matter where the code is retrieving the JEC conditions, the user will want to make sure a valid global tag is used because of the other CMS conditions used.

*WARNING:* If the user requests correction/algorithms which are not in the SQLite file, the corrections will be taken from the global tag.

*WARNING:* If the user list JetCorrectorParametersCollections which are not in the SQLite file, there will be an error.

<a name="output"></a>
## Output
[JetResponseAnalyzer](https://github.com/cms-jet/JetMETAnalysis/blob/master/JetAnalyzers/interface/JetResponseAnalyzer.hh) uses the [TFileService](https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideTFileService) to output a ROOT tree per algorithm. The name of the directory is the name of the module, e.g. 'ak5pfl2l3'.

The branches of the TTree defined by [JetResponseAnalyzer](https://github.com/cms-jet/JetMETAnalysis/blob/master/JetAnalyzers/interface/JetResponseAnalyzer.hh) are sufficient to create all necessary response histograms (using the tools described below) and are given in the following table:

<!--List of emoji: https://www.webpagefx.com/tools/emoji-cheat-sheet/-->
| **Branch** | **Definition** | **Type** | **Description** | **Requirement** |
|:-----------|:---------------|:---------|:----------------|:----------------|
| _npus_ :new: | *&npus_* | *vector&#8249;int&#8250;* | true number of admixed PU events | only for MC |
| _bxns_ :new: | *&bxns_* | *vector&#8249;int&#8250;* | bunch crossing numbers | only for MC |
| _sumpt_lowpt_ :new: | *&sumpt_lowpt_* | *vector&#8249;float&#8250;* | pileup pt sum with a lower pt threshold | only for MC |
| _sumpt_highpt_ :new: | *&sumpt_highpt_* | *vector&#8249;float&#8250;* | pileup pt sum with a higher pt threshold | only for MC |
| _ntrks_lowpt_ :new: | *&ntrks_lowpt_* | *vector&#8249;float&#8250;* | number of tracks with a lower pt threshold | only for MC |
| _ntrks_highpt_ :new: | *&ntrks_highpt_* | *vector&#8249;float&#8250;* | number of tracks with a higher pt threshold | only for MC |
| _rho_ :new: | *rho/F* | *float* | event rho | - |
| _weight_ | *weight/F* | *float* | individual event weight | only for MC |
| _pthat_ | *pthat/F* | *float* | pThat from MC | only for MC  |
| _nref_ | *nref/b* | *unsigned char* | Number of reference-jet pairs | -  |
| _refrank_ | *refrank[nref]/b* | *unsigned char[]* | rank of the reference | -  |
| _refpdgid_ | *refpdgid[nref]/I* | *int[]* | particle id of the reference | doFlavor=true |
| _refe_ | *refe[nref]/F* | *float[]* | reference energy  | -  | 
| _refpt_ | *refpt[nref]/F* | *float[]* | reference pT  | -  | 
| _refeta_ | *refeta[nref]/F* | *float[]* | reference pseudo-rapidity eta | -  |
| _refphi_ | *refphi[nref]/F* | *float[]* | reference azimuth phi | -  | 
| _refy_ | *refy[nref]/F* | *float[]* | reference rapidity y | -  | 
| _refdphijt_ | *refdphijt[nref]/F* | *float[]* | deltaPhi(reference,jet) | balancing mode (_deltaPhiMin_ provided) |
| _refdrjt_ | *refdrjt[nref]/F* | *float[]* | deltaR(reference,jet) | matching mode (_deltaRMax_ provided) |
| _jte_ | *jte[nref]/F* | *float[]* | jet energy | -  |
| _jtpt_ | *jtpt[nref]/F* | *float[]* | jet pT | -  |
| _jteta_ | *jteta[nref]/F* | *float[]* | jet pseudo-rapidty eta | -  |
| _jtphi_ | *jtphi[nref]/F* | *float[]* | jet azimuth phi | -  |
| _jty_ | *jty[nref]/F* | *float[]* | jet rapidity y | -  |
| _jtjec_ :new: | *jtjec[nref]/F* | *float[]* | jet energy correction factor | -  |
| _jtemf_ :new: | *jtemf[nref]/F* | *float[]* | electromagnetic energy fraction | only for CaloJets and doComposition=True  |
| _jtchf_ :new: | *jtchf[nref]/F* | *float[]* | charged hadron energy fraction | only for PFJets and doComposition=True  |
| _jtnhf_ :new: | *jtnhf[nref]/F* | *float[]* | neutral hadron energy fraction | only for PFJets and doComposition=True  |
| _jtnef_ :new: | *jtnef[nref]/F* | *float[]* | photon energy fraction | only for PFJets and doComposition=True  |
| _jtcef_ :new: | *jtcef[nref]/F* | *float[]* | electron energy fraction | only for PFJets and doComposition=True  |
| _jtmuf_ :new: | *jtmuf[nref]/F* | *float[]* | muon energy fraction | only for PFJets and doComposition=True  |
| _jthfhf_ :new: | *jthfhf[nref]/F* | *float[]* | HF hadronic energy fraction | only for PFJets and doComposition=True  |
| _jthfef_ :new: | *jthfef[nref]/F* | *float[]* | HF electromagnetic energy fraction | only for PFJets and doComposition=True  |

**NOTE:** While every effort is made to maintain and up-to-date list of branches, some branches might have been missed. If you are curious about a branch which is not listed here, please contact the current sofware maintainers.
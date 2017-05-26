# Description of the executables provided in [JetMETAnalysis/JetMETAnalyzers](https://github.com/cms-jet/JetAnalyzers)

<!-- MarkdownTOC depth=2 -->

- [Introduction](#introduction)
  - [Configuration](#configuration)
- [jet_response_analyzer_x](#jetresponseanalyzerx)
- [jet_response_fitter_x](#jetresponsefitterx)
- [jet_response_and_resolution_x](#jetresponseandresolutionx)
- [jet_synchtest_x](#jetsynchtestx)
- [jet_synchfit_x](#jetsynchfitx)
- [jet_synchplot_x](#jetsynchplotx)
- [jet_l1_correction_x](#jetl1correctionx)
- [jet_l3_correction_x](#jetl3correctionx)
- [jet_l2_correction_x](#jetl2correctionx)
- [jet_l5_correction_x](#jetl5correctionx)
- [jet_correction_analyzer_x](#jetcorrectionanalyzerx)
- [jet_draw_closure_x](#jetdrawclosurex)
- [jet_apply_jec_x](#jetapplyjecx)
- [jet_mass_analyzer_x](#jetmassanalyzerx)
- [jet_weighted_spectrum_x](#jetweightedspectrumx)

<!-- /MarkdownTOC -->

<a name="introduction"></a>
## Introduction
This README will introduce you to the tools used to evaluate the JetResponseAnalyzer output. Due to the complexity of the software package it might not be possible to list every tool or option available to the user. We will, however, try our best to list the most commonly used tools and options.

<a name="configuration"></a>
### Configuration
:new: The elements passed to a vector-type parameter must be separated by blanks, not commas!

All of the following tools use the *CommandLine* utility ([JetMETAnalysis/JetUtilities](https://github.com/cms-jet/JetUtilities)) to be configured. All declared parameters can be passed via the command line via *tool_x -param value*. Values in a vector **must** be separated by blanks: *tool_x -vector 10 20 30*. Parameters can also be declared in a configuration file, which must then be passed to the tool as the **first** parameter: *tool_x params.config* where params.config could look like this:
```
$ comment
param = value $ another comment
str = "This is a string with blanks in it" $ that wasn't so hard
vector = 10 20 30 $ no reason why this couldn't go on for lines and lines
40 50 60
```
Additional parameters can be passed via the command line, if they appear in the file, the value passed via the command-line will overwrite the value in the file:
```
tool_x params.config -input file.root -param othervalue
```

<a name="jetresponseanalyzerx"></a>
## jet_response_analyzer_x

<a name="purpose"></a>
##### PURPOSE:
This application processes the (ROOT) *tree* produced by the JetResponseAnalyzer (CMSSW fwk analyzer) and produces another file with the respective *histograms*. This allows you to only later decide on exact cuts or binning parameters. All other tools are based on histograms. Output and input files store the tree/histograms in one directory per jet algorithm.

<a name="parameters"></a>
##### PARAMETERS:
The following table summarizes the available options and their defaults. If a parameter has no default, it is mandatory. 

| *Parameter*    | *Type*    | *Default*  | *Description* |
|:---------------|:----------|:-----------|:--------------|
| _input_        | *string*  |            | Name of the input file, containing a directory for each algorithm with a tree in it |
| _binspt_       | *vector&lt;float&gt;*  | "" | pT bin boundaries |
| _binseta_      | *vector&lt;float&gt;*  | "" | eta bin boundaries |
| _binsphi_      | *vector&lt;float&gt;*  | "" | phi bin boundaries |
| _treename_     | *string*  | "t"        | name of the ROOT tree |
| _xsection_     | *float*   | 0.0        | if set >0.0, histogram entries will be filled with weight xsection/tree->GetEntries() |
| _useweight_    | *bool*    | true       | per-event weight from tree (branch: 'weight') will be considered unless xsection>0.0 (or branch not found) |
| _output_       | *string*  | "jra.root" | Name of the output file, containing a directory for each algorithm with histograms in it |
| _nrefmax_      | *int*     | 0          | if not set to 0, only the leading nrefmax references are considered |
| _nbinspt_      | *int*     | 50         | number of bins for pT distributions |
| _nbinseta_     | *int*     | 25         | number of bins for eta distributions | 
| _nbinsphi_     | *int*     | 25         | number of bins for phi distributions |
| _etabarrelmin_ | *float*   | -1.3       | minimum eta for barrel (central) calorimeters |
| _etabarrelmax_ | *float*   | +1.3       | maximum eta for barrel (central) calorimeters |
| _dobalance_    | *bool*    | false      | flag indicating balancing / matching mode |
| _doflavor_     | *bool*    | false      | flag indicating if additional histograms should be booked/filled for references with flavor uds/c/b/g |
| _drmax_        | *float*   | 0.3        | deltaR(ref,jet) requirement in matching mode |
| _dphimin_      | *float*   | 2.7        | deltaPhi(ref,jet) requirement in balancing mode |
| _dojetpt_      | *bool*    | false      | flag indicating if histograms related to pT(jet) are booked/filled |
| _dorefpt_      | *bool*    | true       | flag indicating if histograms related to pT(ref) are booked/filled |
| _nbinsrelrsp_  | *int*     | 50         | number of bins for relative response (pT(jet)/pT(ref)) distributions |
| _relrspmin_    | *float*   | 0.0        | xmin for relative response distributions |
| _relrspmax_    | *float*   | 2.0        | xmax for relative response distributions |
| _nbinsabsrsp_  | *int*     | 150        | number of bins for absolute response (pT(jet)-pT(ref)) distributions |
| _absrspmin_    | *float*   | -400.0     | xmin for absolute response distributions |
| _absrspmax_    | *float*   | 150.0      | xmax for absolute response distributions |
| _nbinsetarsp_  | *int*     | 100        | number of bins for eta response (abs(eta(jet))-abs(eta(ref))) distributions |
| _etarspmin_    | *float*   | -1.0       | xmin for eta response distributions |
| _etarspmax_    | *float*   | 1.0        | xmax for eta response distributions |
| _nbinsphirsp_  | *int*     | 100        | number of bins for phi response (phi(jet)-phi(ref)) distributions |
| _phirspmin_    | *float*   | -1.0       | xmin for phi response distributions |
| _phirspmax_    | *float*   | 1.0        | xmax for phi response distributions |
| _algs_         | *vstring* | ""         | list of algorithms to consider ("" -> all) |
| _presel_       | *vstring* | ""         | list of preselection cuts to print |
| _itlow_        | *int*     | 0          | minimum number of in-time pileup collisions (per event) |
| _ithigh_       | *int*     | 1000       | maximum number of in-time pileup collisions (per event) |
| _earlyootlow_  | *int*     | 0          | minimum number of out-of-time pileup collisions occurring before the current event (per event) |
| _earlyoothigh_ | *int*     | 1000       | maximum number of out-of-time pileup collisions occurring before the current event (per event) |
| _lateootlow_   | *int*     | 0          | minimum number of out-of-time pileup collisions occurring after the current event (per event) |
| _lateoothgih_  | *int*     | 1000       | maximum number of out-of-time pileup collisions occurring after the current event (per event) |
| _totalootlow_  | *int*     | 0          | minimum number of out-of-time pileup collisions occurring before and after the current event (per event) |
| _totaloothigh_ | *int*     | 1000       | maximum number of out-of-time pileup collisions occurring before and after the current event (per event) |
| _weightfile_   | *string*  | ""         | filename containing a histogram of weights to be applied to each distribution |

<a name="examples"></a>
##### EXAMPLES:
```
jet_resonse_analyzer_x config/binning.config -input JRA.root
jet_response_analyzer -input JRA.root -binspt 10 20 30 50 100 -binseta -2 -1 0 1 2
jet_response_analyzer jra.config -input JRA.root -doflavor true -algs kt4calo kt6calo -output jra_ktcalo_flv.root 
```

<a name="step-by-step"></a>
##### STEP-BY-STEP:
  * open input (read) and output (recreate) files
  * loop over directories found in the input file
  * load the tree (default name: 't', parameter 'treename') from the current directory / jet algorithm, set branch addresses
  * create directory in output file and book histograms
  * loop over events in tree and fill histograms
  * write and close output file, close input file

<a name="jetresponsefitterx"></a>
## jet_response_fitter_x

<a name="purpose-1"></a>
##### PURPOSE:
This application processes a file, produced by [jet_response_analyzer_x](https://github.com/cms-jet/JetMETAnalysis/blob/master/JetAnalyzers/bin/jet_response_analyzer_x.cc), containing (absolute or relative) response histograms for each jet algorithm (see [above](#jetresponseanalyzerx)). The response histograms in the input file are fitted with a Gaussian (default) or a double-sided crystal ball function, applying an iterative procedure developed by Kostas Kousouris. The resulting fits are stored in the output file together with the histograms. If the NDF comes out to be less or equal to five in the fit, no function is stored, and downstream algorithms are expected to consider mean and rms instead of the Gaussian fit parameters. Only those jet algorithms considered for fitting (option -algs) are transferred to the output. Algorithms which are omitted can be added later. Algorithms which are fitter already can not be overwritten for now, but are skipped.

<a name="parameters-1"></a>
##### PARAMETERS:
These are the available options:

| *Parameter* | *Type*    | *Default*      | *Description* |
|:------------|:----------|:---------------|:--------------|
| _input_     | *string*  |                | name of the ROOT input file, one directory per algorithm |
| _output_    | *string*  | "input_f.root" | name of the ROOT output file, one directory per algorithm |
| _nsigma_    | *double*  | 1.5            | the fit range for each fit iteration is chosen to be mean +/- nsigma * sigma |
| _jtptmin_   | *float*   | 1.0            | minimum jet pt for fits |
| _niter_     | *int*     | 3              | number of fit iterations. In each iteration, the result from the last fit are used as seeds. Mean/rms from the histo are used as seeds in the first iteration. |
| _ndfmin_    | *int*     | 5              | minimum ndf required for the fits |
| _algs_      | *vstring* |                | list of algorithms to be fitted. Per default, all algorithms found in the file are fitted. |
| _verbose_   | *int*     | 0              | the program will print more information to the screen if verbose>0 |
| _fittype_   | *int*     | 0              | type of fit to perform. 0 for Gaussian and 1 for DSCB. |


<a name="example"></a>
##### EXAMPLE:
```
jet_response_fitter_x -input jra.root
```

<a name="step-bystep"></a>
##### STEP-BY_STEP:
  * open input (read) and output (update!) files
  * loop over directories found in the input file
  * loop over all histograms and identify response histograms by their name, which must either begin with 'RelRsp' or 'AbsRsp'
  * fit the response histogram three times with a Gaussian, adjusting the starting parameters each time from the result of the last fit. Integral, mean, and rms are used as seed parameters for the first iteration.
  * if the histograms has no entries or the NDF comes out smaller or equal to five, no fit is stored, indicating downstream applications to consider mean and rms instead of the fit parameters of the Gaussian
  * save output, close input & output files

<a name="jetresponseandresolutionx"></a>
## jet_response_and_resolution_x

<a name="purpose-2"></a>
##### PURPOSE:
This application evaluates the response histograms for each algorithms over several bins of the specified list of variables and produces graphs, written to a ROOT file with the same directory structure as the input file (one directory per algorithm). It also fits the graphs if they correspond to resolutions. Useful variables are RelRsp, AbsRsp, EtaRsp, and PhiRsp.

<a name="parameters-2"></a>
##### PARAMETERS:
The following table summarizes all application parameters: 

| *Parameter* | *Type*    | *Default*      | *Description* |
|:------------|:----------|:---------------|:--------------|
| _input_     | *string*  |                | name of the ROOT input file, one directory per algorithm |
| _output_    | *string*  | "input_g.root" | name of the output ROOT file, one directory per algorithm |
| _dorelrsp_  | *bool*    | true           | indicate wether relative response distributions are to be turned into graphs |
| _doabsrsp_  | *bool*    | false          | indicate wether absolute response distributions are to be turned into graphs |
| _doetarsp_  | *bool*    | false          | indicate wether eta response distributions are to be turned into graphs |
| _dophirsp_  | *bool*    | false          | indicate wether phi response distributions are to be turned into graphs |
| _flavors_   | *vstring* |                | indicate wether dedicated graphs for flavors are requested. Possible values: all, uds, c, b, g, slc, slb |
| _algs_      | *vstring* |                | list of algorithms to be considered. By default, all algorithms found in the input file are considered |
| _fitres_    | *bool*    | true           | Specify wether or not the resolution graph(s) should be fitted (with a function which is probably suited for the resolution as a function of pT only) |

If e.g. dorelrsp is true, graphs *RelRspVsRefPt*, *RelRspVsJetEta*, *RelRspVsJetPhi*, *RelRspVsRefPt_JetEta\**, *RelRspVsJetEta_RefPt\** will be created for each algorithm. You need to know this naming convention in order to look at them using [jet_inspect_graphs_x](https://github.com/cms-jet/JetMETAnalysis/tree/master/JetUtilities/bin/jet_inspect_graphs_x.cc) , for which you need to set its variables parameter accordingly!

<a name="example-1"></a>
##### EXAMPLE:
jet_response_and_resolution_x -input JRA_f.root
jet_response_and_resolution_x -input jra.root -algs sc5calo,sc7calo

<a name="step-by-step-1"></a>
##### STEP-BY-STEP:
  * open the input (read) and output (recreate) ROOT files
  * loop over all directories in the input file, which correspond to the different jet algorithms
  * load the histograms relevant for the response plots according to the value of 'variable'
  * loop over the relevant response histograms for each algorithm and create response and resolution graphs. Query each histogram for its fit function and consider the Gaussian parameters. If a histogram does not have a fit function stored, use histogram mean and rms
  * fit the resolution if not otherwise requested
  * write the graphs to the output file (same directory structure: one dir per algorithm)
  * create canvas for each algorithm for response and resolution. If there is more than one variable, e.g. JetEta and RefPt, all JetEta graphs are overlaid in the same canvas. Create one file per 'formats' and canvas.
  * close input and output files

<a name="jetsynchtestx"></a>
## jet_synchtest_x
<a name="jetsynchfitx"></a>
## jet_synchfit_x
<a name="jetsynchplotx"></a>
## jet_synchplot_x
<a name="jetl1correctionx"></a>
## jet_l1_correction_x

<a name="jetl3correctionx"></a>
## jet_l3_correction_x

<a name="purpose-7"></a>
##### PURPOSE:
This application processes the response histograms created as described above and computes the absolute (l3) response (vs refpt) and correction (vs jetpt) in the 'barrel'. barrel hereby refers to the central eta region as configured in either [JetResponseAnalyzer](https://github.com/cms-jet/JetMETAnalysis/blob/master/JetAnalyzers/interface/JetResponseAnalyzer.hh) or [jet_response_analyzer_x](https://github.com/cms-jet/JetMETAnalysis/blob/master/JetAnalyzers/bin/jet_response_analyzer_x.cc). The resulting graphs and fits are stored in an output file, and the parameters of the response fit are stored in a text file.

<a name="parameters-7"></a>
##### PARAMETERS:
The application parameters are:

| *Parameter* | *Type*    | *Default* | *Description* |
|:------------|:----------|:----------|:--------------|
| _input_     | *string*  |           | name of the ROOT input file, one directory per algorithm |
| _era_       | *string*  |           | prefix for the output text file names (e.g. Jec11_V11 for Jec11_V11_L3Absolute_AK5Calo.txt) |
| _output_    | *string*  | "l3.root" | name of the output ROOT file, one directory per algorithm |
| _formats_   | *vstring* |           | specify a list of formats (e.g. pdf,jpg) to create one file per canvas and format |
| _algs_      | *vstring* |           | list of algorithms to be considered. By default, all algorithms found in the input file are considered |
| _logx_      | *bool*    | false     | Specify that all grapsh/histograms be displayed with logarithmic x axis |
| _logy_      | *bool*    | false     | Specify that all grapsh/histograms be displayed with logarithmic y axis |
| _batch_     | *bool*    | false     | Running in batch mode means that no plots are displayed on the screen, while all root and graphic files are still being produced.     |
| _flavor_    | *string*  |           | Specifies which flavor (if any) to examine from the input file (only one flavor can be specified) |
| _tpeak_     | *int*     | 1         | Specifies how the program chooses the peak of the histogram (1=fit mean, 2=average of the fit and histogram means, other=histogram mean) |


<a name="example-2"></a>
##### EXAMPLE:
```
jet_l3_correction_x -input jra.root
```

<a name="step-by-step-2"></a>
##### STEP-BY-STEP:
* instantiate a TApplication if not in batch mode (-batch 1), such that drawn canvases are being displayed
* open input (read) and output (recreate) files
* iterate over all directories in input file, each corresponding to a different jet algorithm
* create graphs for absolute response as a function of reference pT and correction as a function of jet pT
* loop over the relevant response histograms, extract mean/sigma and fill graphs
* fit both response and correction, draw a canvas each and create a file for each specified 'formats'
* write the parameters of the response fit to a text file "l3_<alg>.jer" (not needed, is it?)
* close input and output file

<a name="jetl2correctionx"></a>
## jet_l2_correction_x

<a name="purpose-8"></a>
##### PURPOSE:
This application calculates the relative energy correction (w.r.t. the barrel) as a function of jetpt in each etabin, and produces a text file with the results (parameters of the fits in each etabin). It may seem off that the L2Relative corrections are derived after the L3Absolute corrections, but this software needs the absolute response in the barrel as an additional input, see above! (NOTE: This program can now derive the L2 and L3 corrections in one step.)

<a name="parameters-8"></a>
##### PARAMETERS:
| *Parameter* | *Type*    | *Default* | *Description* |
|:------------|:----------|:----------|:--------------|
| _input_     | *string*  |           | name of the ROOT input file, one directory per algorithm |
| _era_       | *string*  |           | prefix for the output text file names (e.g. Jec11_V11 for Jec11_V11_L2Relative_AK5Calo.txt) |
| _l3input_   | *string*  | "l3.root" | name of the file containing the absolute response in the barrel (central) of the detector, see above. |
| _output_    | *string*  | "l2.root" | name of the output ROOT file, one directory per algorithm |
| _outputDir_ | *TString* | "./"      | path to the directory where the output files will be located |
| _formats_   | *vstring* |           | specify a list of formats (e.g. pdf,jpg) to create one file per canvas and format |
| _algs_      | *vstring* |           | list of algorithms to be considered. By default, all algorithms found in the input file are considered |
| _batch_     | *bool*    | false     | Running in batch mode means that no plots are displayed on the screen, while all root and graphic files are still being produced. |
| _l2l3_      | *bool*    | false     | if set to true, the option tells the program to derive the L2Relative and L3Absolute corrections together in one step, making the L3Absolute corrections unnecessary |
| _mpv_       | *bool*    | false     | if false, the program uses the mean from the input histograms, rather than the mean of the fits, for the relative responses |

<a name="example-3"></a>
##### EXAMPLE:
```
jet_l2_correction_x -input jra.root
```

<a name="step-by-step-3"></a>
##### STEP-BY-STEP:
* instantiate a TApplication if not in batch mode (-batch 1), such that drawn canvases are being displayed
* open input (read) and output (recreate) files, as well as the corresponding file with the absolute (l3) response (default: l3.root)
* loop over the directories in the input file, each corresponding to a different jet algorithm
* load and loop over histograms with absolute response, refpt, and jetpt as a function of eta and refpt
* for each eta bin, create graphs for absolute response and correction as a function of refpt. Fit absolute correction with a function in each eta bin.
* loop over all eta/pT bins again to calculate the relative response in each bin w.r.t. the response in the barrel in each etabin (-> graph&fit in each etabin)
* write the parameters of the fit to the relative (l2) correction to a text file, one line per etabin
* close input and output files

<a name="jetl5correctionx"></a>
## jet_l5_correction_x

<a name="purpose-6"></a>
##### PURPOSE:
The code creates the L5Flavor JEC.

<a name="parameters-6"></a>
##### PARAMETERS:
| *Parameter* | *Type*     | *Default* | *Description* |
|:------------|:-----------|:----------|:--------------|
| _input_     | *string*   |           | name of the ROOT input file, one directory per algorithm |
| _output_    | *string*   | "l5.root" | name of the output ROOT file, one directory per algorithm |
| _outputDir_ | *TString*  | "./"      | path to the directory where the output files will be located |
| _algs_      | *vstring*  |           | list of algorithms to be considered. By default, all algorithms found in the input file are considered |
| _batch_     | *bool*     | false     | Running in batch mode means that no plots are displayed on the screen, while all root and graphic files are still being produced. |
| _flavors_   | *vTString* |           | list of flavors to be considered. By default all of the flavors found in the input file are considered |
| _era_       | *string*   |           | prefix for the output text file names (e.g. Jec11_V11 for Jec11_V11_L5Flavor_qJ_AK5Calo.txt) |
| _mpv_       | *bool*     | false     | if false, the program uses the mean from the input histograms, rather than the mean of the fits, for the relative responses |

<a name="examples-3"></a>
##### EXAMPLES:
```
jet_l5_correction_x -input jra_f.root -era Jec11_V11
```

<a name="jetcorrectionanalyzerx"></a>
## jet_correction_analyzer_x
<a name="jetdrawclosurex"></a>
## jet_draw_closure_x

<a name="jetapplyjecx"></a>
## jet_apply_jec_x
<a name="purpose-3"></a>
##### PURPOSE:
This tool allows you to apply a set of jet energy corrections (JEC) in situ, without reprocessing the dataset. Every uncorrected jet collection in the input tree(s) will be corrected according to the *levels* parameter, while corrected jet collections will be skipped. The location of the text files specifying the JEC parameters can be passed via the *jecpath* option. If this parameter is not specified, the path is assumed to be *$CMSSW_BASE/src/CondFormats/JetMETObjects/data*. If that directory does not exist it is assumed to be *$CMSSW_RELEASE_BASE/src/CondFormats/JetMETObjects/data*. If that path doesn't exist either, the program will abort. 

<a name="parameters-3"></a>
##### PARAMETERS:
| *Parameter* | *Type*    | *Default* | *Description* |
|:------------|:----------|:----------|:--------------|
| _input_     | *string*  |           | name of the ROOT input file, one directory per algorithm |
| _era_       | *string*  |           | name of the JEC era (prefix of the paramter text files). |
| _output_    | *string*  | ""        | name of the output ROOT file, one directory per algorithm. [input]_jec.root if not specified. |
| _jecpath_   | *string*  | ""        | path to the parameter text files specifying the corrections. Will look in *$CMSSW_BASE* and *$CMSSW_RELEASE_BASE* if omitted. |
| _algs_      | *vstring* |           | list of algorithms to be considered. By default, all *uncorrected* algorithms found in the input file are considered, all corrected are skipped. |
| _levels_    | *vint32*  | 2,3       | list of correction levels to be applied, L2(Relative) and L3(Absolute) by default. |
| _useTags_   | *bool*    | true      | if true, the program will use properly formatted tags to set the FactorizedJetCorrector, rather than using a vector of JetCorrectorParameters |

<a name="examples-1"></a>
##### EXAMPLES:
```
jet_apply_jec_x -input JRA.root -era Spring10 -levels 2,3 -output JRA_jec.root
```

<a name="jetmassanalyzerx"></a>
## jet_mass_analyzer_x

<a name="purpose-4"></a>
##### PURPOSE:
This tool allows to easily create mass distributions based on the particle pdgids of the mass resonance decay products.

<a name="parameters-4"></a>
##### PARAMETERS:
The tool accepts the following parameters: 

| *Parameter* | *Type*    | *Default* | *Description* |
|:------------|:----------|:----------|:--------------|
| _input_     | *string*  |           | input ROOT file, containg JRA tree(s) |
| _particle_  | *string*  | "Z"       | name of the mass resonance (for x-axis labeling, might be auto-adjusted depending on 'legs' parameter below!) |
| _output_    | *string*  |"jma.root" | name of the output root file containing the created histograms |
| _useweight_ | *bool*    | false     | wether to use (individual event) weights or not |
| _xsection_  | *float*   | 0.0       | if >0.0, each (!) event will be weighted by xsecion / number of events |
| _drmax_     | *float*   | 0.3       | maximum !DeltaR between reference and jet |
| _nbinsmass_ | *int*     | 50        | number of bins for mass histograms |
| _massmin_   | *float*   | 50.0      | minimum x-value for mass histograms (might be auto-adjusted depending on 'legs' parameter below!) |
| _massmax_   | *float*   | 150.0     | maximum x-value for mass histograms (might be auto-adjusted depending on 'legs' parameter below!) |
| _jtptmin_   | *float*   | 1.0       | minimum jet pT |
| _algs_      | *vstring* | ""        | algorithms (= directory names within ROOT file) to be considered ("" means all) |
| _legs_      | *vstring* | "5:-5"    | pdgids of mass resonance decay products (see below) |

The key to the tool is the *legs* parameter: if you e.g. provide "5:-5", it will in each event look for two references, one with pdgid -5 and one with pdgid +5. If both are found, the mass of the sum of the corresponding reference and jet four vectors wil be entered in the reference ("RefMass") and jet ("JetMass") mass histograms respectively. several leg configurations can be provided, e.g. "1:-2,3:-4" in order to catch the corresponding W decays (the charge conjugates are taken into account automatically). There are some predefined string values which you can provide instead, the pdgids are then going to be filled in automatically. Currently these values are: "zbb", "zbb_oneslb", "zbb_bothslb", "wqq", "tbqq", "tbqq_nosl", "tbqq_slb". 

<a name="jetweightedspectrumx"></a>
## jet_weighted_spectrum_x

<a name="purpose-5"></a>
##### PURPOSE:
In case you deal with several samples which need to be properly weighted in order to correspond to the same integrated luminosity, this tool provides a quick and simple check of the weights you are using: it plots the combined pT spectrum of all reference jets before and after taking the weights into account. Each sample needs to be provided as part of a list, where each element has the format *sample:weight*, with *filename=sample+".root"*. 

<a name="parameters-5"></a>
##### PARAMETERS:
| *Parameter* | *Type*    | *Default* | *Description* |
|:------------|:----------|:----------|:--------------|
| _inputs_    | *vstring* |           | list of samples WITH weights, in the format sample:weight. The sample is the name of the file, without ".root" at the end. |
| _datapath_  | *string*  | ""        | path to the files referenced via the inputs parameter. |
| _algorithm_ | *string*  | "ktcalo"  | name of the algorithm |
| _logx_      | *bool*    | false     | log-scale for the x-axis |
| _logy_      | *bool*    | true      | log-scale for the y-axis |
| _batch_     | *bool*    | false     | Run the program in batch mode: no graphics to the screen, makes most sense if plots are saved via formats parameter. |
| _formats_   | *vstring* | ""        | List of formats in which each plot should be saved. example: pdf,gif,png |

<a name="examples-2"></a>
##### EXAMPLES:
```
jet_weighted_spectrum_x config/weighted_spectrum.config
jet_weighted_spectrum_x -datapath /path/to/my/files -inputs QCD0to15:10e10 QCD15to20:10E09 QCD20to30:10E08 
```
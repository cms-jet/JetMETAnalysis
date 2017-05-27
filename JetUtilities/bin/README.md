# Description of the executables provided in [JetMETAnalysis/JetUtilities](https://github.com/cms-jet/JetUtilities)
<!-- MarkdownTOC depth=1 -->

- [jet_inspect_histos_x](#jetinspecthistosx)
- [jet_inspect_graphs_x](#jetinspectgraphsx)
- [jet_inspect_profiles_x](#jetinspectprofilesx)

<!-- /MarkdownTOC -->

<a name="jetinspecthistosx"></a>
## jet_inspect_histos_x

<a name="purpose"></a>
##### PURPOSE:
This simple yet powerful tool allows you to plot histograms created by [jet_response_analyzer_x](https://github.com/cms-jet/JetMETAnalysis/blob/master/JetAnalyzers/bin/jet_response_analyzer_x.cc) or [jet_mass_analyzer_x](https://github.com/cms-jet/JetMETAnalysis/blob/master/JetAnalyzers/bin/jet_mass_analyzer_x.cc). The response for different files & algorithms can also be compared (will look good for maximum two though!).

Exploits ObjectLoader<TH1F> to display the response in different bins of variables to be specified. Different files (inputs), algorithms (algs), or variables (e.g. differen Eta / pT ranges) can be overlayed.

If different files are to be overlayed, specify a space-separated list as an argument to the -inputs option.

If different algorithms should be compared, specify a space-separated list as an arguments to the -algs option.

If different variables should be compared, specify a space-separated list as an argument to the -variables option.

A *variable* has the following syntax:
```
<quantity>:Var1:...:VarN
```
and the (only!) assumption is that histograms are stored with names as
```
quantity_<Var1><min>to<max>:...:<VarN><min>to<max>
```
e.g. "RelRsp_JetEta0to1.4_RefPt20to27"

<a name="example-variables"></a>
###### Example Variables:

  * RelRsp:RefPt
    * corresponds to the relative response as a function of reference (e.g. genJet) pT; one response histogram will be created for each found RefPt bin.
  * AbsRsp:JetEta:RefPt
    * corresponds to the absolute response as a function of jet eta and reference pT; one response histogram will be created for each found JetEta and RefPt bin. The response distributions belonging to the same eta bin will be drawn in a separate canvas by default.
  * RelRsp:JetEta@0:RefPt
    * corresponds to the relative response as a function of reference pT, in whichever eta bin contains '0'.
  * RelRsp:JetEta#1:RefPt@25
    * the '#1' indicates that JetEta should be considered the second variable (index 0->1): corresponds to the relative response *as a function of eta*, in whichever RefPt bin contains '25'.

###### PARAMETERS:
The options are summarized in the following table:

| *Parameter*  | *Type*    | *Default*      | *Description* |
|:-------------|:----------|:---------------|:--------------|
| _inputs_     | *vstring* |                | list of ROOT input files |
| _algs_       | *vstring* | "ktcalo"       | list of algorithms |
| _variables_  | *vstring* | "AbsRsp:RefPt" | variables to be plotted, in the format quantity:var1[:...[:varN]] |
| _npercanvas_ | *int*     | 0              | number of histograms per canvas, 0=all |
| _norm_       | *bool*    | 0              | normalize all histograms to 1.0 if set to true |
| _mean_       | *bool*    | 0              | draw a line for the histogram *mean* if set to true |
| _median_     | *bool*    | 0              | draw a line for the histogram *median* if set to true |
| _peak_       | *bool*    | 0              | draw a line for the histogram fit *peak* if set to true (and if the histogram has been fit) |
| _logx_       | *bool*    | 0              | display x-axis logarithmically if set to true |
| _logy_       | *bool*    | 0              | display y-axis logarithmically if set to true |
| _prefix_     | *string*  | ""             | prefix for the canvas / created plot file (constructed automatically if "") |
| _batch_      | *bool*    | 0              | execute in batch mode (no graphics are being displayed, but plot files are created!) |
| _formats_    | *vstring* | -              | indicate for which formats (ps, eps, pdf, png, jpg) each canvas should be saved. (if empty and batch is true, add pdf automatically!) |

<a name="examples"></a>
##### EXAMPLES:
```
jet_inspect_histos_x -inputs JRA_f.root -algs kt4calo -variables RelRsp:RefPt
```

<a name="jetinspectgraphsx"></a>
## jet_inspect_graphs_x

<a name="purpose-1"></a>
##### PURPOSE:
The tool allows you to plot *graphs* created by  [jet_response_and_resolution_x](https://github.com/cms-jet/JetMETAnalysis/blob/master/JetAnalyzers/bin/jet_response_and_resolution_x.cc) (actually, any application which conforms with the expected naming convention of the TGraphError objects!). Just like in [jet_inspect_histos_x](https://github.com/cms-jet/JetMETAnalysis/blob/master/JetUtilities/bin/jet_inspect_histos_x.cc), multiple graphs can be overlayed on the same canvas.

<a name="parameters"></a>
##### PARAMETERS:
The options are summarized in the following table:

| *Parameter* | *Type*    | *Default*       | *Description* |
|:------------|:----------|:----------------|:--------------|
| _inputs_    | *vstring* |                 | list of ROOT input files |
| _algs_      | *vstring* | "ak5calo"       | list of algorithms |
| _variables_ | *vstring* | "RelRspVsRefPt" | variables to be plotted, in the format quantityVsVar1[:...[:VarN]] |
| _labels_    | *vstring* | -               | Provide legend labels (constructed automatically if empty) |
| _text_      | *string*  | ""              | place text somwhere in the pad, format: "[x:y:]text" |
| _logx_      | *bool*    | 0               | display x-axis logarithmically if set to true |
| _logy_      | *bool*    | 0               | display y-axis logarithmically if set to true |
| _ymin_      | *float*   | -1.0            | y-axis minimum if larger than 0.0, 0.0/0.01 otherwise, based on logy*false/true |
| _ymax_      | *float*   | -1.0            | y-axis maximum if larger than 0.0, as determined by ROOT otherwise |
| _nocolor_   | *bool*    | false           | request plots to be drawn in black and white only (with adjusted marker styles) |
| _overlay_   | *bool*    | true            | indicate wether all graphs should be overlayed (default) or if each should be drawn in its own pad |
| _prefix_    | *string*  | ""              | prefix for the canvas / created plot file (constructed automatically if "") |
| _batch_     | *bool*    | 0               | execute in batch mode (no graphics are being displayed, but plot files are created!) |
| _formats_   | *vstring* | -               | indicate for which formats (ps, eps, pdf, png, jpg) each canvas should be saved. (if empty and batch is true, add pdf automatically!) |

<a name="examples-1"></a>
##### EXAMPLES:
```
jet_inspect_graphs_x -inputs JRA_g.root -algs sc5pf sc5calo
jet_inspect_graphs_x -inputs JRA_f_g.root -algs sc5pf sc7pf
jet_inspect_graphs_x -inputs JRA_g.root,JRA_fastsim_g.root -algs ak5pf -variables AbsRspVsRefPt:JetEta@0
jet_inspect_graphs_x -inputs JRA_f_g.root -variables RelRspVsRefPt:JetEta@0 RelRsVsRefPt:JetEta@2
```

<a name="name-mapping"></a>
##### NAME MAPPING:
This table will help you map the input variable names to the output graph names. These should just be used as examples as there are a great many options.

| *Variable*        | *TGraphErrors name(s)*    |
|:------------------|:--------------------------|
| RspVsRefPt        | RspVsRefPt                |
| ResVsRefPt:JetEta | ResVsRefPt_JetEta0to1.4   |
|                   |	ResVsRefPt_JetEta1.4to2.6 |
| RspVsJetEta:RefPt | RspVsJetEta_RefPt20to27   |
|                   |	RspVsJetEta_RefPt27to35   |

<a name="jetinspectprofilesx"></a>
## jet_inspect_profiles_x

<a name="purpose-2"></a>
##### PURPOSE:
Make profiles with this tool, e.g. of the response vs eta/pT. Useful for quick closure tests. You can supply different inputs, algorithms, and/or selections. E.g. you can compare different samples, different algorithms, or different kinematic regions.

<a name="parameters-1"></a>
##### PARAMETERS:
The complete list of parameters is the following: 

| *Parameter*  | *Type*    | *Default*           | *Description* |
|:-------------|:----------|:--------------------|:--------------|
| _inputs_     | *vstring* |                     | list of ROOT input files |
| _algs_       | *vstring* | "ak5pf"             | list of algorithms (e.g. 'ak5jptl2l3') |
| _selections_ | *vstring* | ""                  | list of selections (e.g. 'refpt>20.') |
| _varexpr_    | *string*  | "jtpt/refpt:jteta"  | variable expression describing 2D histo grom which profile is derived. |
| _treename_   | *string*  | "t"                 | name of the ROOT tree. |
| _nbinsx_     |  *int*    | 50                  | number of bins in x. |
| _xmin_       | *double*  | -5.0                | minimum value on x-axis. |
| _xmax_       | *double*  | 5.0                 | maximum value on x-axis. |
| _nbinsy_     |  *int*    | 50                  | number of bins in y. |
| _ymin_       | *double*  | 0.0                 | minimum value on y-axis. |
| _ymax_       | *double*  | 1.2                 | maximum value on y-axis. |
| _logx_       | *bool*    | false               | whether the x-axis should be displayed in log scale. |
| _logy_       | *bool*    | false               | whether the y-axis should be displayed in log scale. |
| _xtitle_     | *string*  | "jet #eta"          | x-axis title |
| _ytitle_     | *string*  | "p_{T}/p_{T}^{REF}" | y-axis title |
| _colors_     | *vint*    | ""                  | line colors for each of the profiles. |
| _labels_     | *vstring* | ""                  | legend labels for each of the profiles. |
| _hlines_     | *vstring* | ""                  | where to draw horizontal lines, specify <yvalue>[:<line_style>], e.g. "1.01:2" |
| _output_     | *string*  | "profile"           | name of canvas / output file(s). |
| _batch_      | *bool*    | 0                   | execute in batch mode (no graphics are being displayed, but plot files are created) |
| _formats_    | *vstring* | -                   | indicate for which formats (ps, eps, pdf, png, jpg) each canvas should be saved. (if empty and batch is true, add pdf automatically!) |

<a name="examples-2"></a>
##### EXAMPLES:
```
jet_inspect_profiles_x -inputs JRA.root
jet_inspect_profiles_x -inputs JRA.root -selections "refpt>20.&&refpt<50." "refpt>100.&refpt<300." -labels "20<p_{T}<50" "100<p_{T}<300"
jet_inspect_profiles_x config/jip_vseta.config config/jip_ptrsp.config -inputs JRA.root
```
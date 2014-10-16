----------------------
 jet_inspect_histos_x
----------------------

PURPOSE: exploits ObjectLoader<TH1F> to display the response in
different bins of variables to be specified. Different files (inputs),
algorithms (algs), or variables (e.g. differen Eta / pT ranges) can be
overlayed.

If different files are to be overlayed, specify a space- or
comma-separated list as an argument to the -inputs option.

If different algorithms should be compared, specify a space- or
comma-separated list as an arguments to the -algs option.

If different variables should be compared, specify a space- or
comma-separated list as an argument to the -variables option.

A *variable* has the following syntax:

<quantity>:Var1:...:VarN

and the (only!) assumption is that histograms are stored with names as

quantity_<Var1><min>to<max>:...:<VarN><min>to<max>

e.g. "RelRsp_JetEta0to1.4_RefPt20to27"

Examples:
---------
- RelRsp:RefPt

  corresponds to the relative response as a function of reference
  (e.g. genJet) pT; one response histogram will be created for each
  found RefPt bin.

- AbsRsp:JetEta:RefPt

  corresponds to the absolute response as a function of jet eta and
  reference pT; one response histogram will be created for each found
  JetEta and RefPt bin. The response distributions belonging to the
  same eta bin will be drawn in a separate canvas by default.

- RelRsp:JetEta@0:RefPt

  corresponds to the relative response as a function of reference pT,
  in whichever eta bin contains '0'.

- RelRsp:JetEta#1:RefPt@25

  the '#1' indicates that JetEta should be considered the second
  variable (index 0->1): corresponds to the relative response *as a
  function of eta*, in whichever RefPt bin contains '25'.

EXAMPLE:
jet_inspect_histos_x -inputs JRAh.root -algs kt4calo -variables RelRsp:RefPt


----------------------
 jet_inspect_graphs_x
----------------------

PURPOSE: allows to overlay TGraphError objects, with a very similar
naming convention (variable) as above:

variable	  	        TGraphErrors name(s)
--------------------------------------------------------
RspVsRefPt			RspVsRefPt
ResVsRefPt:JetEta               ResVsRefPt_JetEta0to1.4
				ResVsRefPt_JetEta1.4to2.6
RspVsJetEta:RefPt               RspVsJetEta_RefPt20to27
				RspVsJetEta_RefPt27to35
--------------------------------------------------------


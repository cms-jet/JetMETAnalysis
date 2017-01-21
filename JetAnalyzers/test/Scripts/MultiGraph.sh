#!/bin/bash
#--------------- draw the closure plots and resolution calibration ---------------
# Note: You need to change the path after -inputs in the command according to your path!

jet_inspect_graphs_x -inputs ./HLTBX25JEC/EcalMultifitHCALMethod3_jra_L1L2L3_MergePt_g.root  -algs ak4pfHLTl1l2l3 ak4caloHLTl1l2l3 -variables RelResVsJetEta:RefPt@10  -xmin -5 -xmax 5 -ymin 0.2 -ymax 0.8

jet_inspect_graphs_x -inputs ./HLTBX25JEC/EcalMultifitHCALMethod3_jra_L1L2L3_MergePt_g.root  -algs ak4pfHLTl1l2l3 ak4caloHLTl1l2l3 -variables RelResVsJetEta:RefPt@30  -xmin -5 -xmax 5 -ymin 0 -ymax 0.4

jet_inspect_graphs_x -inputs ./HLTBX25JEC/EcalMultifitHCALMethod3_jra_L1L2L3_MergePt_g.root  -algs ak4pfHLTl1l2l3 ak4caloHLTl1l2l3 -variables RelResVsJetEta:RefPt@50  -xmin -5 -xmax 5 -ymin 0 -ymax 0.4

jet_inspect_graphs_x -inputs ./HLTBX25JEC/EcalMultifitHCALMethod3_jra_L1L2L3_MergeEta_g.root  -algs ak4pfHLTl1l2l3 ak4caloHLTl1l2l3 -variables RelResVsRefPt:JetEta@0.0  -xmin 10 -xmax 1000 -ymin 0 -ymax 0.6 -logx true

jet_inspect_graphs_x -inputs ./HLTBX25JEC/EcalMultifitHCALMethod3_jra_L1L2L3_MergeEta_g.root  -algs ak4pfHLTl1l2l3 ak4caloHLTl1l2l3 -variables RelResVsRefPt:JetEta@1.3  -xmin 10 -xmax 500 -ymin 0 -ymax 0.6 -logx true

jet_inspect_graphs_x -inputs ./HLTBX25JEC/EcalMultifitHCALMethod3_jra_L1L2L3_MergeEta_g.root  -algs ak4pfHLTl1l2l3 ak4caloHLTl1l2l3 -variables RelResVsRefPt:JetEta@3.0  -xmin 10 -xmax 300 -ymin 0 -ymax 0.6 -logx true

jet_inspect_graphs_x -inputs ./HLTBX25JEC/EcalMultifitHCALMethod3_jra_L1L2L3_MergePt_g.root -algs ak4pfHLTl1l2l3 ak4caloHLTl1l2l3 -variables RelRspVsTrueNPU:RefPt@10 -xmin 0 -xmax 70 -ymin 0.95 -ymax 1.25

jet_inspect_graphs_x -inputs ./HLTBX25JEC/EcalMultifitHCALMethod3_jra_L1L2L3_MergePt_g.root -algs ak4pfHLTl1l2l3 ak4caloHLTl1l2l3 -variables RelRspVsTrueNPU:RefPt@30 -xmin 0 -xmax 70 -ymin 0.95 -ymax 1.25

jet_inspect_graphs_x -inputs ./HLTBX25JEC/EcalMultifitHCALMethod3_jra_L1L2L3_MergePt_g.root -algs ak4pfHLTl1l2l3 ak4caloHLTl1l2l3 -variables RelRspVsTrueNPU:RefPt@50 -xmin 0 -xmax 70 -ymin 0.95 -ymax 1.25

jet_inspect_graphs_x -inputs ./HLTBX25JEC/EcalMultifitHCALMethod3_jra_L1L2L3_MergePt_g.root  -algs ak4pfHLTl1l2l3 ak4caloHLTl1l2l3 -variables RelRspVsJetEta:RefPt@10  -xmin -5 -xmax 5 -ymin 0.95 -ymax 1.25

jet_inspect_graphs_x -inputs ./HLTBX25JEC/EcalMultifitHCALMethod3_jra_L1L2L3_MergePt_g.root  -algs ak4pfHLTl1l2l3 ak4caloHLTl1l2l3 -variables RelRspVsJetEta:RefPt@30  -xmin -5 -xmax 5 -ymin 0.95 -ymax 1.15

jet_inspect_graphs_x -inputs ./HLTBX25JEC/EcalMultifitHCALMethod3_jra_L1L2L3_MergePt_g.root  -algs ak4pfHLTl1l2l3 ak4caloHLTl1l2l3 -variables RelRspVsJetEta:RefPt@50  -xmin -5 -xmax 5 -ymin 0.95 -ymax 1.05

jet_inspect_graphs_x -inputs ./HLTBX25JEC/EcalMultifitHCALMethod3_jra_L1L2L3_MergeEta_g.root  -algs ak4pfHLTl1l2l3 ak4caloHLTl1l2l3 -variables RelRspVsRefPt:JetEta@0.0  -xmin 10 -xmax 1000 -ymin 0.95 -ymax 1.2 -logx true

jet_inspect_graphs_x -inputs ./HLTBX25JEC/EcalMultifitHCALMethod3_jra_L1L2L3_MergeEta_g.root  -algs ak4pfHLTl1l2l3 ak4caloHLTl1l2l3 -variables RelRspVsRefPt:JetEta@1.3  -xmin 10 -xmax 500 -ymin 0.95 -ymax 1.2 -logx true

jet_inspect_graphs_x -inputs ./HLTBX25JEC/EcalMultifitHCALMethod3_jra_L1L2L3_MergeEta_g.root  -algs ak4pfHLTl1l2l3 ak4caloHLTl1l2l3 -variables RelRspVsRefPt:JetEta@3.0  -xmin 10 -xmax 300 -ymin 0.95 -ymax 1.2 -logx true

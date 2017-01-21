jet_response_fitter_x -input ./HLTBX25JEC/jra_L1L2L3_MergeEta.root -output ./HLTBX25JEC/jra_L1L2L3_MergeEta_f.root

jet_response_and_resolution_x -input ./HLTBX25JEC/jra_L1L2L3_MergeEta_f.root -output ./HLTBX25JEC/EcalMultifitHCALMethod3_jra_L1L2L3_MergeEta_g.root -algs ak4pfHLTl1l2l3 ak4caloHLTl1l2l3 ak8pfHLTl1l2l3 ak8caloHLTl1l2l3

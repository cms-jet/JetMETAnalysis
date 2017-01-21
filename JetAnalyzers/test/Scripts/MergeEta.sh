#!/bin/bash
#------------- Fit all the response profiles with Gaussian function ------------------------
jet_response_fitter_x -input ./HLTBX25JEC/jra_L1L2L3_MergeEta.root -output ./HLTBX25JEC/jra_L1L2L3_MergeEta_f.root

#------------- Extract the jet response and resolution versus gen-jet pt in different jet eta regions ------
jet_response_and_resolution_x -input ./HLTBX25JEC/jra_L1L2L3_MergeEta_f.root -output ./HLTBX25JEC/EcalMultifitHCALMethod3_jra_L1L2L3_MergeEta_g.root -algs ak4pfHLTl1l2l3 ak4caloHLTl1l2l3 ak8pfHLTl1l2l3 ak8caloHLTl1l2l3

# Note: You need to change the path after -input and -output in the command according to your path!

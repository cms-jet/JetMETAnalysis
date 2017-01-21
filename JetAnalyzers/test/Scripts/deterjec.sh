jet_response_fitter_x -input ./HLTBX25JEC/jra_L1.root -output ./HLTBX25JEC/jra_L1_f.root

jet_l3_correction_x -input ./HLTBX25JEC/jra_L1_f.root -era EcalMultifitHCALMethod3 -algs ak4pfHLTl1 ak4caloHLTl1 ak8pfHLTl1 ak8caloHLTl1 -output ./HLTBX25JEC/l3.root

jet_l2_correction_x -input ./HLTBX25JEC/jra_L1_f.root -l3input ./HLTBX25JEC/l3.root -output ./HLTBX25JEC/l2.root -era EcalMultifitHCALMethod3 -algs ak4pfHLTl1 ak4caloHLTl1 ak8pfHLTl1 ak8caloHLTl1

rm EcalMultifitHCALMethod3_L3Absolute_AK4PFHLTl1.txt
rm EcalMultifitHCALMethod3_L3Absolute_AK4CaloHLTl1.txt
rm EcalMultifitHCALMethod3_L3Absolute_AK8PFHLTl1.txt
rm EcalMultifitHCALMethod3_L3Absolute_AK8CaloHLTl1.txt

mv EcalMultifitHCALMethod3_L2Relative_AK4PFHLTl1.txt JECTag_EcalMultifit_HCALMethod3/
mv EcalMultifitHCALMethod3_L2Relative_AK4CaloHLTl1.txt JECTag_EcalMultifit_HCALMethod3/
mv EcalMultifitHCALMethod3_L2Relative_AK8PFHLTl1.txt JECTag_EcalMultifit_HCALMethod3/
mv EcalMultifitHCALMethod3_L2Relative_AK8CaloHLTl1.txt JECTag_EcalMultifit_HCALMethod3/

rm -rf L3*.png

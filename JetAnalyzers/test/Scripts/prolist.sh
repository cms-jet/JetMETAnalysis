#!/bin/bash
source /afs/cern.ch/project/eos/installation/cms/etc/setup.sh

#----------- For PreL1batch.sh -------------------------
#eos ls store/user/fengwang/QCDHLT_NoPU/*.root > list.txt

#----------- For L1Hadd.sh -------------------------
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JECL1/output_ak4caloHLT*.root > list_ak4caloHLT.txt
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JECL1/output_ak4pfHLT*.root   > list_ak4pfHLT.txt
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JECL1/output_ak8caloHLT*.root > list_ak8caloHLT.txt
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JECL1/output_ak8pfHLT*.root   > list_ak8pfHLT.txt

#-------------- For L1batch.sh ---------------------------
#eos ls /store/user/fengwang/QCDHLT/JRA_1*.root > list1.txt
#eos ls /store/user/fengwang/QCDHLT/JRA_2*.root > list2.txt
#eos ls /store/user/fengwang/QCDHLT/JRA_3*.root > list3.txt
#eos ls /store/user/fengwang/QCDHLT/JRA_4*.root > list4.txt
#eos ls /store/user/fengwang/QCDHLT/JRA_5*.root > list5.txt
#eos ls /store/user/fengwang/QCDHLT/JRA_6*.root > list6.txt
#eos ls /store/user/fengwang/QCDHLT/JRA_7*.root > list7.txt
#eos ls /store/user/fengwang/QCDHLT/JRA_8*.root > list8.txt
#eos ls /store/user/fengwang/QCDHLT/JRA_9*.root > list9.txt

#-------------- For PrejraHadd.sh -------------------------
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/jra_L1_*.root          > list_fineBinning.txt
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/jra_L1-MergeEta_*.root > list_MergeEta.txt
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/jra_L1-MergePt_*.root  > list_MergePt.txt

#-------------- For L2batch.sh ---------------------------- 
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/JRA_L1_1*.root > list1.txt
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/JRA_L1_2*.root > list2.txt
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/JRA_L1_3*.root > list3.txt
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/JRA_L1_4*.root > list4.txt
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/JRA_L1_5*.root > list5.txt
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/JRA_L1_6*.root > list6.txt
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/JRA_L1_7*.root > list7.txt
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/JRA_L1_8*.root > list8.txt
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/JRA_L1_9*.root > list9.txt

#------------- For L3batch.sh -----------------------------
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/JRA_L1L2_1*.root > list1.txt
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/JRA_L1L2_2*.root > list2.txt
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/JRA_L1L2_3*.root > list3.txt
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/JRA_L1L2_4*.root > list4.txt
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/JRA_L1L2_5*.root > list5.txt
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/JRA_L1L2_6*.root > list6.txt
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/JRA_L1L2_7*.root > list7.txt
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/JRA_L1L2_8*.root > list8.txt
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/JRA_L1L2_9*.root > list9.txt

#------------- For jrabatch.sh -----------------------------
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/JRA_L1L2L3_1*.root > list1.txt
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/JRA_L1L2L3_2*.root > list2.txt
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/JRA_L1L2L3_3*.root > list3.txt
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/JRA_L1L2L3_4*.root > list4.txt
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/JRA_L1L2L3_5*.root > list5.txt
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/JRA_L1L2L3_6*.root > list6.txt
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/JRA_L1L2L3_7*.root > list7.txt
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/JRA_L1L2L3_8*.root > list8.txt
#eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/JRA_L1L2L3_9*.root > list9.txt

#------------- For jraHadd.sh -------------------------------
eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/jra_L1L2L3_*.root          > list_fineBinning.txt
eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/jra_L1L2L3-MergeEta_*.root > list_MergeEta.txt
eos ls /store/group/phys_jetmet/fengwang/HLTBX25JEC/jra_L1L2L3-MergePt_*.root  > list_MergePt.txt

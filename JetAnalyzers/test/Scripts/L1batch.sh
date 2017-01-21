#!/bin/bash
#set up the environment
export SCRAM_ARCH=slc6_amd64_gcc491
cd /afs/cern.ch/work/f/fengwang/OnlineJEC/CMSSW_8_0_0/src/ # modify this to your own path!
eval $(scramv1 runtime -sh)
cd JetMETAnalysis/JetAnalyzers/test/

#----------------Please modify these file paths to your own directories! ---------------------
confpath="/afs/cern.ch/work/f/fengwang/OnlineJEC/CMSSW_8_0_0/src/JetMETAnalysis/JetAnalyzers/config/"
inpath="/store/user/fengwang/QCDHLT"
midpath="/tmp/fengwang"
outpath="/store/group/phys_jetmet/fengwang/HLTBX25JEC"
#---------------------------------------------------------------------------------------------

num=$1
filelist="list$num.txt"

string="JRA_L1_"
string2="jra_L1_"
string3="jra_L1-MergeEta_"
string4="jra_L1-MergePt_"

confile="jra_dr_finebinning.config"
confile2="jra_QCDflat_MergeEta.config"
confile3="jra_Eta_finebinning.config"

iter=$(($num * 1000))

while read file
do
  echo $file
	label=`printf $iter`
	outfile="$string$label.root"
	outfile2="$string2$label.root"
    outfile3="$string3$label.root"
    outfile4="$string4$label.root"

	printf "Copy file from eos to tmp..."
	cmsStage $inpath/$file $midpath/

    jet_apply_jec_x -input $midpath/$file -output $midpath/$outfile -era EcalMultifitHCALMethod3 -levels 1 -jecpath ./JECTag_EcalMultifit_HCALMethod3/ -algs ak4pfHLT ak4caloHLT ak8pfHLT ak8caloHLT

	jet_response_analyzer_x $confpath/$confile -input $midpath/$outfile -output $midpath/$outfile2 -nbinsabsrsp 0 -nbinsetarsp 0 -nbinsphirsp 0 -algs ak4pfHLTl1 ak4caloHLTl1 ak8pfHLTl1 ak8caloHLTl1

	jet_response_analyzer_x $confpath/$confile2 -input $midpath/$outfile -output $midpath/$outfile3 -nbinsabsrsp 0 -nbinsetarsp 0 -nbinsphirsp 0 -algs ak4pfHLTl1 ak4caloHLTl1 ak8pfHLTl1 ak8caloHLTl1

	jet_response_analyzer_x $confpath/$confile3 -input $midpath/$outfile -output $midpath/$outfile4 -nbinsabsrsp 0 -nbinsetarsp 0 -nbinsphirsp 0 -algs ak4pfHLTl1 ak4caloHLTl1 ak8pfHLTl1 ak8caloHLTl1

	printf "Copy file from tmp back to eos..."
	cmsStage $midpath/$outfile $outpath/
	cmsStage $midpath/$outfile2 $outpath/
	cmsStage $midpath/$outfile3 $outpath/
	cmsStage $midpath/$outfile4 $outpath/

	rm -rf $midpath/$file
	rm -rf $midpath/$outfile
	rm -rf $midpath/$outfile2
	rm -rf $midpath/$outfile3
	rm -rf $midpath/$outfile4

	(( iter += 1 ))

done < $filelist

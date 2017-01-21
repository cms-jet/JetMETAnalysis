#!/bin/bash
export SCRAM_ARCH=slc6_amd64_gcc491
cd /afs/cern.ch/work/f/fengwang/OnlineJEC/CMSSW_8_0_0/src/ # modify this to your own directory!
eval $(scramv1 runtime -sh)
cd JetMETAnalysis/JetAnalyzers/test/

# -------------- please change these paths to your own ones -------------
inpath="/store/group/phys_jetmet/fengwang/HLTBX25JECL1/"
outpath="/afs/cern.ch/work/f/fengwang/OnlineJEC/CMSSW_8_0_0/src/JetMETAnalysis/JetAnalyzers/test/"
# -----------------------------------------------------------------------

num=$1
outfile="L1Prefile_"
list="list_"

if [ $num -eq 1 ]; then
    outfile=$outfile"ak4caloHLT_"
    list=$list"ak4caloHLT.txt"
elif [ $num -eq 2 ]; then
    outfile=$outfile"ak4pfHLT_"
    list=$list"ak4pfHLT.txt"
elif [ $num -eq 3 ]; then
    outfile=$outfile"ak8caloHLT_"
    list=$list"ak8caloHLT.txt"
else
    outfile=$outfile"ak8pfHLT_"
    list=$list"ak8pfHLT.txt"
fi

num1=0
num2=1

while read file
do
    label=`printf $num1`
    label2=`printf $num2`

    if [ $num1 -eq 0 ]; then
        infile=$inpath$file
        echo $infile
        printf "Copy file from eos to local directory ..."
        cmsStage $infile $outpath
        mv $outpath$file $outpath$outfile$label2".root" 
    else
        infile1=$outpath$outfile$label".root"
        infile2=$inpath$file
        echo $infile1
        echo $infile2
        printf "Copy file from eos to local directory ..."
        cmsStage $infile2 $outpath
        printf "Hadd source files ..."
        hadd $outpath$outfile$label2".root" $outpath$outfile$label".root" $outpath$file
        printf "Hadd finish!!!"
        rm $outpath$outfile$label".root"
        rm $outpath$file
    fi

    (( num1 += 1 ))
    (( num2 += 1 ))

done < $list

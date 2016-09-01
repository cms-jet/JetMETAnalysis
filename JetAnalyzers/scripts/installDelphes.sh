#!/bin/bash

if [ -z "$CMSSW_BASE" ]; then
    echo "you need to run \cmsenv\""
    exit 1
fi  

startDir=`pwd`
cd ${CMSSW_BASE}
export DELPHES_DIR=${CMSSW_BASE}/delphes
cd delphes/
make

cp ${CMSSW_BASE}/src/JetMETAnalysis/JetAnalyzers/scripts/delphes.xml ${CMSSW_BASE}/config/toolbox/${SCRAM_ARCH}/tools/selected
scram setup delphes

TEST=`scram tool list | grep delphes | wc -l`

if [ "$TEST" -eq "0" ]; then
    echo "delphes was not successfullly installed :-("
    cd $startDir
    exit 1
fi

echo "you can now link to the delphes libraries through scram!"

# Check if the Delphes folder/library exist and if the library is commented out of the JetMETAnalysis/JetAnalyzers/bin/BuildFile.xml. If so then uncomment the library line. Do the same for the DelphesNtupleToJRANtuple_x.cc executable.
export BUILD_FILE_PATH=${CMSSW_BASE}/src/JetMETAnalysis/JetAnalyzers/bin/BuildFile.xml
if [ -d "$DELPHES_DIR" ]; then
	if [ -f "$DELPHES_DIR/libDelphes.so" ];
	then
		export LIB_EXISTS=`grep -F "delphes" ${BUILD_FILE_PATH}`
		if [ -z "${LIB_EXISTS}" ]
		then
			echo "The line \"<use name=\"delphes\"/>\" does not exist in ${BUILD_FILE_PATH}"
			exit 1
		fi

		if [[ $LIB_EXISTS == *"-->"* ]]
		then 
			export LIB_EXISTS_UNCOMMENTED=${LIB_EXISTS#<\!--}
			export LIB_EXISTS_UNCOMMENTED=${LIB_EXISTS%-->}
			sed -i -e "s%LIB_EXISTS%LIB_EXISTS_UNCOMMENTED%g" ${BUILD_FILE_PATH}
			echo "The Delphes library line has been uncommented and ready to use in ${BUILD_FILE_PATH}"			
		else
			echo "The Delphes library line is already uncommented and ready to use in ${BUILD_FILE_PATH}"
		fi

		export LIB_EXISTS=`grep -F "DelphesNtupleToJRANtuple" ${BUILD_FILE_PATH}`
		if [ -z "${LIB_EXISTS}" ]
		then
            echo "The DelphesNtupleToJRANtuple executable line does not seem to exist in ${BUILD_FILE_PATH}"
            exit 1
        fi

		if [[ $LIB_EXISTS == *"-->"* ]]
        then
			export LIB_EXISTS_UNCOMMENTED=${LIB_EXISTS#<\!--}
            export LIB_EXISTS_UNCOMMENTED=${LIB_EXISTS%-->}
			sed-i -e "s%LIB_EXISTS%LIB_EXISTS_UNCOMMENTED%g" ${BUILD_FILE_PATH}
            echo "The DelphesNtupleToJRANtuple line has been uncommented and ready to use in ${BUILD_FILE_PATH}"
        else
            echo "The DelphesNtupleToJRANtuple line is already uncommented and ready to use in ${BUILD_FILE_PATH}"
        fi
	else
		echo "The file libDelphes.so does not exist where we expected. Cannot include library in the JetMETAnalysis/JetAnalyzers/bin/BuildFile.xml not can we compile DelphesNtupleToJRANtuple_x.cc."
		exit 1
	fi
fi

cd $startDir

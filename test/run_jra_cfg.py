import FWCore.ParameterSet.Config as cms
import JetMETAnalysis.JetAnalyzers.Defaults_cff as Defaults;

#!
#! PROCESS
#!
process = cms.Process("jetresponseanalyzerProcess")


#!
#! SERVICES
#!
#process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageLogger.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 2000
process.load('CommonTools.UtilAlgos.TFileService_cfi')
process.TFileService.fileName=cms.string('jra.root')


#!
#! INPUT
#!
inputFiles = cms.untracked.vstring(
#############################
# JRAP PATTuple (Published) #
#############################
#'/store/user/aperloff/QCD_Pt-15to3000_TuneZ2star_Flat_8TeV_pythia6/PAT_JRAP_PFchs_53X/ea497ea51a493fe0c576c422ffa32b60/JRAP_264_1_ivm.root',
#'/store/user/aperloff/QCD_Pt-15to3000_TuneZ2star_Flat_8TeV_pythia6/PAT_JRAP_PFchs_53X/ea497ea51a493fe0c576c422ffa32b60/JRAP_265_1_DUv.root',
#'/store/user/aperloff/QCD_Pt-15to3000_TuneZ2star_Flat_8TeV_pythia6/PAT_JRAP_PFchs_53X/ea497ea51a493fe0c576c422ffa32b60/JRAP_266_1_LHj.root',
###############################
# JRAP PATTuple (Unpublished) #
###############################
#'file:/fdata/hepx/store/user/aperloff/QCD_Pt-15to3000_TuneZ2star_Flat_8TeV_pythia6/PAT_JRAP_53X/bcedc6301a9ce8b39e91810d93214040/JRAP_287_1_0s2.root',
#'file:/fdata/hepx/store/user/aperloff/QCD_Pt-15to3000_TuneZ2star_Flat_8TeV_pythia6/PAT_JRAP_53X/bcedc6301a9ce8b39e91810d93214040/JRAP_288_1_AF4.root',
#'file:/fdata/hepx/store/user/aperloff/QCD_Pt-15to3000_TuneZ2star_Flat_8TeV_pythia6/PAT_JRAP_53X/bcedc6301a9ce8b39e91810d93214040/JRAP_289_1_T7V.root',
'file:JRAP.root'
    )

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(100))
process.source = cms.Source("PoolSource",
                            skipEvents = cms.untracked.uint32(0),
                            fileNames = inputFiles )


algsizetype = {'ak':[4]}
#algsizetype = {'ak':[3,4,5,6,7,8,9,10]}
jettype = ['pf','pfchs']
#jettype = ['calo','pf','pfchs']
corrs = ['']
#corrs = ['','l1','l2l3']
size = []

algorithms = []

for k, v in algsizetype.iteritems():
	for s in v:
		for j in jettype:
			for c in corrs:
				size.append(s/20.0)
				algorithms.append(str(k+str(s)+j+c))


#!
#! MODULES
#!
for (counter, algorithm) in enumerate(algorithms):

	sequence = cms.Sequence()

	#!
	#! MODULES/PARAMETERS
	#!
	jra = cms.EDAnalyzer('jet_response_analyzer',
		#-----Program Level Inputs
		Defaults.jet_response_parameters,
		#algs              = cms.vstring("ak3pfchs","ak4pfchs","ak5pfchs","ak6pfchs","ak7pfchs","ak8pfchs","ak9pfchs","ak10pfchs"),#,"ak3pfchsl1","ak4pfchsl1","ak5pfchsl1","ak6pfchsl1","ak7pfchsl1","ak8pfchsl1","ak9pfchsl1","ak10pfchsl1")
		algs = cms.vstring(algorithm),
		#drmaxs            = cms.vdouble(0.150,0.200,0.250,0.300,0.350,0.400,0.450,0.500),#,0.150,0.200,0.250,0.300,0.350,0.400,0.450,0.500)
		drmaxs = cms.vdouble(size[counter])
	)
	setattr(process,algorithm,jra)
	sequence = cms.Sequence(sequence * jra)
	setattr(process, algorithm + 'Sequence', sequence)

	path = cms.Path( sequence )
	setattr(process, algorithm + 'Path', path)
	print algorithm

#!
#! PATH
#!
#process.p = cms.Path(process.jra)

#process.options.wantSummary = True
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

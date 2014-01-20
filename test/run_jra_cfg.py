import FWCore.ParameterSet.Config as cms

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
#################
# JRAP PATTuple #
#################
'file:/fdata/hepx/store/user/aperloff/QCD_Pt-15to3000_TuneZ2star_Flat_8TeV_pythia6/PAT_JRAP_53X/bcedc6301a9ce8b39e91810d93214040/JRAP_287_1_0s2.root',
'file:/fdata/hepx/store/user/aperloff/QCD_Pt-15to3000_TuneZ2star_Flat_8TeV_pythia6/PAT_JRAP_53X/bcedc6301a9ce8b39e91810d93214040/JRAP_288_1_AF4.root',
'file:/fdata/hepx/store/user/aperloff/QCD_Pt-15to3000_TuneZ2star_Flat_8TeV_pythia6/PAT_JRAP_53X/bcedc6301a9ce8b39e91810d93214040/JRAP_289_1_T7V.root',
    )

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(30000))
process.source = cms.Source("PoolSource",
                            skipEvents = cms.untracked.uint32(0),
                            fileNames = inputFiles )


#!
#! PARAMETERS
#!
process.jra = cms.EDAnalyzer('jet_response_analyzer')
	#-----Program Level Inputs
process.jra.algs              = cms.vstring("ak3pf","ak4pf","ak5pf","ak6pf","ak7pf","ak8pf","ak9pf","ak10pf","ak3pfl1","ak4pfl1","ak5pfl1","ak6pfl1","ak7pfl1","ak8pfl1","ak9pfl1","ak10pfl1")
process.jra.drmaxs            = cms.vdouble(0.150,0.200,0.250,0.300,0.350,0.400,0.450,0.500,0.150,0.200,0.250,0.300,0.350,0.400,0.450,0.500)
process.jra.binspt            = cms.vdouble(10,10.5,11,11.5,12,12.5,13,13.5,14,15,17,20,23,27,30,35,40,45,57,72,90,120,150,200,300,400,550,750,1000,1500,2000,2500,3000,3500,4000,4500,5000,10000)
process.jra.binseta           = cms.vdouble(-5.191,-4.889,-4.716,-4.538,-4.363,-4.191,-4.013,-3.839,-3.664,-3.489,-3.314,-3.139,-2.964,-2.853,-2.65, -2.5,  -2.322,-2.172,-2.043,-1.93,-1.83,  -1.74, -1.653,-1.566,-1.479,-1.392,-1.305,-1.218,-1.131,-1.044,-0.957, -0.879,-0.783,-0.696,-0.609,-0.522,-0.435,-0.348,-0.261,-0.174,-0.087, 0,0.087, 0.174, 0.261, 0.348, 0.435 ,0.522, 0.609, 0.696,0.783,  0.879, 0.957, 1.044, 1.131, 1.218, 1.305 ,1.392, 1.479, 1.566,1.653,  1.74,  1.83,  1.93,  2.043, 2.172, 2.322 ,2.5,   2.65,  2.853,2.964,  3.139, 3.314, 3.489, 3.664, 3.839, 4.013 ,4.191, 4.363, 4.538,4.716,4.889,5.191)
process.jra.binsphi           = cms.vdouble(-3.141,-2.700,-2.100,-1.500,-0.900,-0.300,0.300,0.900,1.500,2.100,2.700,3.141)
process.jra.binsy             = cms.vdouble()
process.jra.treename          = cms.string("t")
process.jra.useweight         = cms.bool(False)
process.jra.xsection          = cms.double(0.0)
process.jra.nrefmax           = cms.int32(0)
process.jra.nbinspt           = cms.int32(50)
process.jra.nbinseta          = cms.int32(25)
process.jra.nbinsphi          = cms.int32(25)
process.jra.nbinsy            = cms.int32(25)
process.jra.etabarrelmin      = cms.double(-1.3)
process.jra.etabarrelmax      = cms.double(+1.3)
process.jra.dobalance         = cms.bool(False)
process.jra.doflavor          = cms.bool(False)
process.jra.noabsflavors      = cms.bool(False)
process.jra.drmax             = cms.double(0.3)
process.jra.dphimin           = cms.double(2.7)
process.jra.dojetpt           = cms.bool(False)
process.jra.dorefpt           = cms.bool(True)
process.jra.nbinsrelrsp       = cms.int32(50)
process.jra.relrspmin         = cms.double(0.0)
process.jra.relrspmax         = cms.double(2.0)
process.jra.nbinsabsrsp       = cms.int32(0)
process.jra.absrspmin         = cms.double(-250.0)
process.jra.absrspmax         = cms.double(100.0)
process.jra.nbinsetarsp       = cms.int32(100)
process.jra.etarspmin         = cms.double(-1.0)
process.jra.etarspmax         = cms.double(1.0)
process.jra.nbinsphirsp       = cms.int32(100)
process.jra.phirspmin         = cms.double(-1.0)
process.jra.phirspmax         = cms.double(1.0)
process.jra.jtptmin           = cms.double(1.0)
process.jra.presel            = cms.vstring("")
process.jra.itlow             = cms.int32(0)
process.jra.ithigh            = cms.int32(1000)
process.jra.earlyootlow       = cms.int32(0)
process.jra.earlyoothigh      = cms.int32(1000)
process.jra.lateootlow        = cms.int32(0)
process.jra.lateoothigh       = cms.int32(1000)
process.jra.totalootlow       = cms.int32(0)
process.jra.totaloothigh      = cms.int32(1000)
process.jra.weightfile        = cms.string("")
process.jra.MCPUReWeighting   = cms.string("")
process.jra.DataPUReWeighting = cms.string("")

#!
#! PATH
#!
process.p = cms.Path(process.jra)

#process.options.wantSummary = True
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
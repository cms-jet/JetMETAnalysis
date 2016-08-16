import FWCore.ParameterSet.Config as cms

################################################################################
## filter final state partons (define globaly)
################################################################################

#partons = cms.EDProducer('PartonSelector',
#    src = cms.InputTag('genParticles'),
#    withLeptons = cms.bool(False),
#    skipFirstN = cms.uint32(0)
#)


################################################################################
## jet reconstruction
################################################################################
from JetMETAnalysis.JetAnalyzers.JetReconstruction_cff import *
from JetMETAnalysis.JetAnalyzers.TauReconstruction_cff import *
#from JetMETAnalysis.JetAnalyzers.JPTReconstruction_cff import *
from JetMETAnalysis.JetAnalyzers.JetCorrection_cff     import *
from RecoTauTag.TauTagTools.tauDecayModes_cfi          import *
from CommonTools.PileupAlgos.Puppi_cff import *

tauDiscriminatorDict = {
    "ak5tauHPSlooseCombDBcorr"  : "hpsPFTauDiscriminationByLooseCombinedIsolationDBSumPtCorr3Hits",
    "ak5tauHPSmediumCombDBcorr" : "hpsPFTauDiscriminationByMediumCombinedIsolationDBSumPtCorr3Hits",
    "ak5tauHPStightCombDBcorr"  : "hpsPFTauDiscriminationByTightCombinedIsolationDBSumPtCorr3Hits",
}
#
tauDecayModeDict = {
    "All"                       : "*",
    "OneProng0Pi0"              : "%i" % tauToOneProng0PiZero,
    "OneProng1Pi0"              : "%i" % tauToOneProng1PiZero,
    "OneProng2Pi0"              : "%i" % tauToOneProng2PiZero,
    "ThreeProng0Pi0"            : "%i" % tauToThreeProng0PiZero
}
#
tauDiscriminators_and_DecayModes = {}
for tauDiscriminator in tauDiscriminatorDict:
    for tauDecayMode in tauDecayModeDict:
        key = tauDiscriminator
        if tauDecayModeDict[tauDecayMode] != "":
            key += tauDecayMode
        tauDiscriminators_and_DecayModes[key] = (tauDiscriminatorDict[tauDiscriminator], tauDecayModeDict[tauDecayMode])

stdGenJetsDict = {
#	'ak3calo'       : 'ak3GenJets',
#	'ak4calo'       : 'ak4GenJets',
    'ak4calo'       : 'ak4GenJets',
    'ak4caloHLT'    : 'ak4GenJets', # for HLT usage
    'ak5calo'       : 'ak5GenJets',
#	'ak6calo'       : 'ak6GenJets',
    'ak7calo'       : 'ak7GenJets',
#	'ak8calo'       : 'ak8GenJets',
    'ak8caloHLT'    : 'ak8GenJets', # for HLT usage
#	'ak9calo'       : 'ak9GenJets',
#	'ak10calo'      : 'ak10GenJets',
    'kt4calo'       : 'kt4GenJets',
    'kt6calo'       : 'kt6GenJets',
    'ak5caloHLT'    : 'ak5GenJets',
	'ak1pf'         : 'ak1GenJets',
	'ak2pf'         : 'ak2GenJets',
	'ak3pf'         : 'ak3GenJets',
	'ak4pf'         : 'ak4GenJets',
    'ak4pfHLT'      : 'ak4GenJets', # for HLT usage
    'ak5pf'         : 'ak5GenJets',
	'ak6pf'         : 'ak6GenJets',
    'ak7pf'         : 'ak7GenJets',
	'ak8pf'         : 'ak8GenJets',
    'ak8pfHLT'      : 'ak8GenJets', # for HLT usage
	'ak9pf'         : 'ak9GenJets',
	'ak10pf'        : 'ak10GenJets',
    'kt4pf'         : 'kt4GenJets',
    'kt6pf'         : 'kt6GenJets',
	'ak1pfchs'      : 'ak1GenJets',
	'ak2pfchs'      : 'ak2GenJets',
	'ak3pfchs'      : 'ak3GenJets',
	'ak4pfchs'      : 'ak4GenJets',
    'ak5pfchs'      : 'ak5GenJets',
	'ak6pfchs'      : 'ak6GenJets',
    'ak7pfchs'      : 'ak7GenJets',
	'ak8pfchs'      : 'ak8GenJets',
	'ak9pfchs'      : 'ak9GenJets',
	'ak10pfchs'     : 'ak10GenJets',
    'ak5pfHLT'      : 'ak5GenJets',
    'ak5pfchsHLT'   : 'ak5GenJets',
    'ak5trk'        : 'ak5GenJets',
    'ak7trk'        : 'ak7GenJets',
    'kt4trk'        : 'kt4GenJets',
    'kt6trk'        : 'kt6GenJets',
    'ak1puppi'      : 'ak1GenJets',
    'ak2puppi'      : 'ak2GenJets',
    'ak3puppi'      : 'ak3GenJets',
    'ak4puppi'      : 'ak4GenJets',
    'ak5puppi'      : 'ak5GenJets',
    'ak6puppi'      : 'ak6GenJets',
    'ak7puppi'      : 'ak7GenJets',
    'ak8puppi'      : 'ak8GenJets',
    'ak9puppi'      : 'ak9GenJets',
    'ak10puppi'     : 'ak10GenJets',
#    'ak5jpt'        : 'ak5GenJets',
#    'ak7jpt'        : 'ak7GenJets',
    'ak5tauHPSall'  : 'tauGenJetsSelectorAllHadrons',
#    'ak5tauTaNCall' : 'tauGenJetsSelectorAllHadrons'
}
for tauDiscriminator_and_DecayMode in tauDiscriminators_and_DecayModes:
    if   tauDiscriminator_and_DecayMode.find("HPS")  != -1:
        stdGenJetsDict[tauDiscriminator_and_DecayMode] = stdGenJetsDict["ak5tauHPSall"]
    elif tauDiscriminator_and_DecayMode.find("TaNC") != -1:
        stdGenJetsDict[tauDiscriminator_and_DecayMode] = stdGenJetsDict["ak5tauTaNCall"]

genJetsDict = {
#	'ak3calo'       : ('ak3GenJetsNoNu',               ak3GenJetsNoNu),
#	'ak4calo'       : ('ak4GenJetsNoNu',               ak4GenJetsNoNu),
    'ak4calo'       : ('ak4GenJetsNoNu',               ak4GenJetsNoNu), #chaned to NoNu from NoMuNoNu
    'ak4caloHLT'    : ('ak4GenJetsNoNu',               ak4GenJetsNoNu), # for HLT usage
    'ak5calo'       : ('ak5GenJetsNoNu',               ak5GenJetsNoNu),
#	'ak6calo'       : ('ak6GenJetsNoNu',               ak6GenJetsNoNu),
    'ak7calo'       : ('ak7GenJetsNoNu',               ak7GenJetsNoNu),
#	'ak8calo'       : ('ak8GenJetsNoNu',               ak8GenJetsNoNu),
    'ak8caloHLT'    : ('ak8GenJetsNoNu',               ak8GenJetsNoNu),
#	'ak9calo'       : ('ak9GenJetsNoNu',               ak9GenJetsNoNu),
#	'ak10calo'      : ('ak10GenJetsNoNu',              ak10GenJetsNoNu),
    'kt4calo'       : ('kt4GenJetsNoNu',               kt4GenJetsNoNu),
    'kt6calo'       : ('kt6GenJetsNoNu',               kt6GenJetsNoNu),
    'ak5caloHLT'    : ('ak5GenJetsNoNu',               ak5GenJetsNoNu),
	'ak1pf'         : ('ak1GenJetsNoNu',               ak1GenJetsNoNu),
	'ak2pf'         : ('ak2GenJetsNoNu',               ak2GenJetsNoNu),
	'ak3pf'         : ('ak3GenJetsNoNu',               ak3GenJetsNoNu),
	'ak4pf'         : ('ak4GenJetsNoNu',               ak4GenJetsNoNu),
    'ak4pfHLT'      : ('ak4GenJetsNoNu',               ak4GenJetsNoNu),
    'ak5pf'         : ('ak5GenJetsNoNu',               ak5GenJetsNoNu),
	'ak6pf'         : ('ak6GenJetsNoNu',               ak6GenJetsNoNu),
    'ak7pf'         : ('ak7GenJetsNoNu',               ak7GenJetsNoNu),
	'ak8pf'         : ('ak8GenJetsNoNu',               ak8GenJetsNoNu),
    'ak8pfHLT'      : ('ak8GenJetsNoNu',               ak8GenJetsNoNu),
	'ak9pf'         : ('ak9GenJetsNoNu',               ak9GenJetsNoNu),
	'ak10pf'        : ('ak10GenJetsNoNu',              ak10GenJetsNoNu),
    'kt4pf'         : ('kt4GenJetsNoNu',               kt4GenJetsNoNu),
    'kt6pf'         : ('kt6GenJetsNoNu',               kt6GenJetsNoNu),
	'ak1pfchs'      : ('ak1GenJetsNoNu',               ak1GenJetsNoNu),
	'ak2pfchs'      : ('ak2GenJetsNoNu',               ak2GenJetsNoNu),
	'ak3pfchs'      : ('ak3GenJetsNoNu',               ak3GenJetsNoNu),
	'ak4pfchs'      : ('ak4GenJetsNoNu',               ak4GenJetsNoNu),
    'ak5pfchs'      : ('ak5GenJetsNoNu',               ak5GenJetsNoNu),
	'ak6pfchs'      : ('ak6GenJetsNoNu',               ak6GenJetsNoNu),
    'ak7pfchs'      : ('ak7GenJetsNoNu',               ak7GenJetsNoNu),
	'ak8pfchs'      : ('ak8GenJetsNoNu',               ak8GenJetsNoNu),
	'ak9pfchs'      : ('ak9GenJetsNoNu',               ak9GenJetsNoNu),
	'ak10pfchs'     : ('ak10GenJetsNoNu',              ak10GenJetsNoNu),
    'ak5pfHLT'      : ('ak5GenJetsNoNu',               ak5GenJetsNoNu),
    'ak5pfchsHLT'   : ('ak5GenJetsNoNu',               ak5GenJetsNoNu),
    'ak5trk'        : ('ak5GenJetsNoNu',               ak5GenJetsNoNu),
    'ak7trk'        : ('ak7GenJetsNoNu',               ak7GenJetsNoNu),
    'kt4trk'        : ('kt4GenJetsNoNu',               kt4GenJetsNoNu),
    'kt6trk'        : ('kt6GenJetsNoNu',               kt6GenJetsNoNu),
    'ak1puppi'      : ('ak1GenJetsNoNu',               ak1GenJetsNoNu),
    'ak2puppi'      : ('ak2GenJetsNoNu',               ak2GenJetsNoNu),
    'ak3puppi'      : ('ak3GenJetsNoNu',               ak3GenJetsNoNu),
    'ak4puppi'      : ('ak4GenJetsNoNu',               ak4GenJetsNoNu),
    'ak5puppi'      : ('ak5GenJetsNoNu',               ak5GenJetsNoNu),
    'ak6puppi'      : ('ak6GenJetsNoNu',               ak6GenJetsNoNu),
    'ak7puppi'      : ('ak7GenJetsNoNu',               ak7GenJetsNoNu),
    'ak8puppi'      : ('ak8GenJetsNoNu',               ak8GenJetsNoNu),
    'ak9puppi'      : ('ak9GenJetsNoNu',               ak9GenJetsNoNu),
    'ak10puppi'     : ('ak10GenJetsNoNu',              ak10GenJetsNoNu),
#    'ak5jpt'        : ('ak5GenJetsNoNu',               ak5GenJetsNoNu),
#    'ak7jpt'        : ('ak7GenJetsNoNu',               ak7GenJetsNoNu),
    'ak5tauHPSall'  : ('tauGenJetsSelectorAllHadrons', tauGenJetsSelectorAllHadrons),
#    'ak5tauTaNCall' : ('tauGenJetsSelectorAllHadrons', tauGenJetsSelectorAllHadrons)
}
for tauDiscriminator_and_DecayMode in tauDiscriminators_and_DecayModes:
    if   tauDiscriminator_and_DecayMode.find("HPS")  != -1:
        genJetsDict[tauDiscriminator_and_DecayMode] = genJetsDict["ak5tauHPSall"]
    elif tauDiscriminator_and_DecayMode.find("TaNC") != -1:
        genJetsDict[tauDiscriminator_and_DecayMode] = genJetsDict["ak5tauTaNCall"]


stdRecJetsDict = {
#    'ak3calo'       : 'ak3CaloJets',
#    'ak4calo'       : 'ak4CaloJets',
    'ak4calo'       : 'ak4CaloJets',
    'ak4caloHLT'    : 'hltAK4CaloJets', # for HLT usage
    'ak5calo'       : 'ak5CaloJets',
#	'ak6calo'       : 'ak6CaloJets',
    'ak7calo'       : 'ak7CaloJets',
#	'ak8calo'       : 'ak8CaloJets',
    'ak8caloHLT'    : 'hltAK8CaloJets', # for HLT usage
#	'ak9calo'       : 'ak9CaloJets',
#	'ak10calo'      : 'ak10CaloJets',
    'kt4calo'       : 'kt4CaloJets',
    'kt6calo'       : 'kt6CaloJets',
    'ak5caloHLT'    : 'hltAntiKT5CaloJets',
    'ak1pf'         : 'ak1PFJets',
    'ak2pf'         : 'ak2PFJets',
    'ak3pf'         : 'ak3PFJets',
    'ak4pf'         : 'ak4PFJets',
    'ak4pfHLT'      : 'hltAK4PFJets', # for HLT usage
    'ak5pf'         : 'ak5PFJets',
	'ak6pf'         : 'ak6PFJets',
    'ak7pf'         : 'ak7PFJets',
    'ak8pf'         : 'ak8PFJets',
    'ak8pfHLT'      : 'hltAK8PFJets', # for HLT usage
    'ak9pf'         : 'ak9PFJets',
	'ak10pf'        : 'ak10PFJets',
    'kt4pf'         : 'kt4PFJets',
    'kt6pf'         : 'kt6PFJets',
    'ak1pfchs'      : 'ak1PFchsJets',
    'ak2pfchs'      : 'ak2PFchsJets',
    'ak3pfchs'      : 'ak3PFchsJets',
    'ak4pfchs'      : 'ak4PFchsJets',
    'ak5pfchs'      : 'ak5PFchsJets',
	'ak6pfchs'      : 'ak6PFchsJets',
    'ak7pfchs'      : 'ak7PFchsJets',
    'ak8pfchs'      : 'ak8PFchsJets',
    'ak9pfchs'      : 'ak9PFchsJets',
	'ak10pfchs'     : 'ak10PFchsJets',
    'ak5pfHLT'      : 'hltAntiKT5PFJets',
    'ak5pfchsHLT'   : 'hltAntiKT5PFJetsNoPUPixelVert',
    'ak5trk'        : 'ak5TrackJets',
    'kt4trk'        : 'kt4TrackJets',
    'ak1puppi'      : 'ak1PUPPIJets',
    'ak2puppi'      : 'ak2PUPPIJets',
    'ak3puppi'      : 'ak3PUPPIJets',
    'ak4puppi'      : 'ak4PUPPIJets',
    'ak5puppi'      : 'ak5PUPPIJets',
    'ak6puppi'      : 'ak6PUPPIJets',
    'ak7puppi'      : 'ak7PUPPIJets',
    'ak8puppi'      : 'ak8PUPPIJets',
    'ak9puppi'      : 'ak9PUPPIJets',
    'ak10puppi'     : 'ak10PUPPIJets',
#    'ak5jpt'        : 'ak5JPTJets',
#    'ak7jpt'        : 'ak7JPTJets',
    'ak5tauHPSall'  : 'hpsPFTauProducer',
#    'ak5tauTaNCall' : 'hpsTancTaus'
}
for tauDiscriminator_and_DecayMode in tauDiscriminators_and_DecayModes:
    if   tauDiscriminator_and_DecayMode.find("HPS")  != -1:
        stdRecJetsDict[tauDiscriminator_and_DecayMode] = stdRecJetsDict["ak5tauHPSall"]
    elif tauDiscriminator_and_DecayMode.find("TaNC") != -1:
        stdRecJetsDict[tauDiscriminator_and_DecayMode] = stdRecJetsDict["ak5tauTaNCall"]

recJetsDict = {
#    'ak3calo'       : ('ak3CaloJets',        ak3CaloJets),
#    'ak4calo'       : ('ak4CaloJets',        ak4CaloJets),
    'ak4calo'       : ('ak4CaloJets',        ak4CaloJets),
    'ak4caloHLT'    : ('ak4CaloHLTJets',     ak4CaloHLTJets), # for HLT usage
    'ak5calo'       : ('ak5CaloJets',        ak5CaloJets),
#	'ak6calo'       : ('ak6CaloJets',        ak6CaloJets),
    'ak7calo'       : ('ak7CaloJets',        ak7CaloJets),
#	'ak8calo'       : ('ak8CaloJets',        ak8CaloJets),
    'ak8caloHLT'    : ('ak8CaloHLTJets',     ak8CaloHLTJets), # for HLT usage
#	'ak9calo'       : ('ak9CaloJets',        ak9CaloJets),
#	'ak10calo'      : ('ak10CaloJets',       ak10CaloJets),
    'kt4calo'       : ('kt4CaloJets',        kt4CaloJets),
    'kt6calo'       : ('kt6CaloJets',        kt6CaloJets),
    'ak5caloHLT'    : ('ak5CaloHLTJets',     ak5CaloHLTJets),
    'ak1pf'         : ('ak1PFJets',          ak1PFJets),
    'ak2pf'         : ('ak2PFJets',          ak2PFJets),
    'ak3pf'         : ('ak3PFJets',          ak3PFJets),
    'ak4pf'         : ('ak4PFJets',          ak4PFJets),
    'ak4pfHLT'      : ('ak4PFHLTJets',       ak4PFHLTJets), # for HLT usage
    'ak5pf'         : ('ak5PFJets',          ak5PFJets),
	'ak6pf'         : ('ak6PFJets',          ak6PFJets),
    'ak7pf'         : ('ak7PFJets',          ak7PFJets),
	'ak8pf'         : ('ak8PFJets',          ak8PFJets),
    'ak8pfHLT'      : ('ak8PFHLTJets',       ak8PFHLTJets), # for HLT usage
	'ak9pf'         : ('ak9PFJets',          ak9PFJets),
	'ak10pf'        : ('ak10PFJets',         ak10PFJets),
    'kt4pf'         : ('kt4PFJets',          kt4PFJets),
    'kt6pf'         : ('kt6PFJets',          kt6PFJets),
    'ak1pfchs'      : ('ak1PFchsJets',       ak1PFchsJets),
    'ak2pfchs'      : ('ak2PFchsJets',       ak2PFchsJets),
    'ak3pfchs'      : ('ak3PFchsJets',       ak3PFchsJets),
    'ak4pfchs'      : ('ak4PFchsJets',       ak4PFchsJets),
    'ak5pfchs'      : ('ak5PFchsJets',       ak5PFchsJets),
	'ak6pfchs'      : ('ak6PFchsJets',       ak6PFchsJets),
    'ak7pfchs'      : ('ak7PFchsJets',       ak7PFchsJets),
	'ak8pfchs'      : ('ak8PFchsJets',       ak8PFchsJets),
	'ak9pfchs'      : ('ak9PFchsJets',       ak9PFchsJets),
	'ak10pfchs'     : ('ak10PFchsJets',      ak10PFchsJets),
    'ak5pfHLT'      : ('ak5PFHLTJets',       ak5PFHLTJets),
    'ak5pfchsHLT'   : ('ak5PFchsHLTJets',    ak5PFchsHLTJets),
    'ak5trk'        : ('ak5TrackJets',       ak5TrackJets),
    'kt4trk'        : ('kt4TrackJets',       kt4TrackJets),
    'ak1puppi'      : ('ak1PUPPIJets',       ak1PUPPIJets),
    'ak2puppi'      : ('ak2PUPPIJets',       ak2PUPPIJets),
    'ak3puppi'      : ('ak3PUPPIJets',       ak3PUPPIJets),
    'ak4puppi'      : ('ak4PUPPIJets',       ak4PUPPIJets),
    'ak5puppi'      : ('ak5PUPPIJets',       ak5PUPPIJets),
    'ak6puppi'      : ('ak6PUPPIJets',       ak6PUPPIJets),
    'ak7puppi'      : ('ak7PUPPIJets',       ak7PUPPIJets),
    'ak8puppi'      : ('ak8PUPPIJets',       ak8PUPPIJets),
    'ak9puppi'      : ('ak9PUPPIJets',       ak9PUPPIJets),
    'ak10puppi'     : ('ak10PUPPIJets',      ak10PUPPIJets),
#    'ak5jpt'        : ('ak5JPTJets',         ak5JPTJets),
#    'ak7jpt'        : ('ak7JPTJets',         ak7JPTJets),
    'ak5tauHPSall'  : ('hpsPFTauProducer',   hpsPFTauProducer),
#    'ak5tauTaNCall' : ('hpsTancTaus',        hpsTancTaus)
}
for tauDiscriminator_and_DecayMode in tauDiscriminators_and_DecayModes:
    if   tauDiscriminator_and_DecayMode.find("HPS")  != -1:
        recJetsDict[tauDiscriminator_and_DecayMode] = recJetsDict["ak5tauHPSall"]
    elif tauDiscriminator_and_DecayMode.find("TaNC") != -1:
        recJetsDict[tauDiscriminator_and_DecayMode] = recJetsDict["ak5tauTaNCall"]


corrJetsDict = {
    'ak4calol1off'         : ('ak4CaloJetsL1Off',          ak4CaloJetsL1Off),
    'ak7calol1off'         : ('ak7CaloJetsL1Off',          ak7CaloJetsL1Off),
    'ak4calol1'            : ('ak4CaloJetsL1Fast',         ak4CaloJetsL1Fast),
    'ak7calol1'            : ('ak7CaloJetsL1Fast',         ak7CaloJetsL1Fast),
    'kt4calol1'            : ('kt4CaloJetsL1Fast',         kt4CaloJetsL1Fast),
    'kt6calol1'            : ('kt6CaloJetsL1Fast',         kt6CaloJetsL1Fast),
    'ak5caloHLTl1'         : ('ak5CaloHLTJetsL1Fast',      ak5CaloHLTJetsL1Fast),
    'ak5caloHLTl1off'      : ('ak5CaloHLTJetsL1Off',       ak5CaloHLTJetsL1Off),
#	'ak5jptl1'             : ('ak5JPTJetsL1Fast',          ak5JPTJetsL1Fast),
#	'ak7jptl1'             : ('ak7JPTJetsL1Fast',          ak7JPTJetsL1Fast),
#    'ak5jptl1off'          : ('ak5JPTJetsL1Off',           ak5JPTJetsL1Off),
#    'ak7jptl1off'          : ('ak7JPTJetsL1Off',           ak7JPTJetsL1Off),
    'ak5pfl1off'           : ('ak5PFJetsL1Off',            ak5PFJetsL1Off),
    'ak7pfl1off'           : ('ak7PFJetsL1Off',            ak7PFJetsL1Off),
	'ak1pfl1'              : ('ak1PFJetsL1Fast',           ak1PFJetsL1Fast),
	'ak2pfl1'              : ('ak2PFJetsL1Fast',           ak2PFJetsL1Fast),
	'ak3pfl1'              : ('ak3PFJetsL1Fast',           ak3PFJetsL1Fast),
	'ak4pfl1'              : ('ak4PFJetsL1Fast',           ak4PFJetsL1Fast),
    'ak5pfl1'              : ('ak5PFJetsL1Fast',           ak5PFJetsL1Fast),
	'ak6pfl1'              : ('ak6PFJetsL1Fast',           ak6PFJetsL1Fast),
    'ak7pfl1'              : ('ak7PFJetsL1Fast',           ak7PFJetsL1Fast),
	'ak8pfl1'              : ('ak8PFJetsL1Fast',           ak8PFJetsL1Fast),
	'ak9pfl1'              : ('ak9PFJetsL1Fast',           ak9PFJetsL1Fast),
	'ak10pfl1'             : ('ak10PFJetsL1Fast',          ak10PFJetsL1Fast),
    'kt4pfl1'              : ('kt4PFJetsL1Fast',           kt4PFJetsL1Fast),
    'kt6pfl1'              : ('kt6PFJetsL1Fast',           kt6PFJetsL1Fast),
    'ak5pfchsl1off'        : ('ak5PFchsJetsL1Off',         ak5PFchsJetsL1Off),
    'ak7pfchsl1off'        : ('ak7PFchsJetsL1Off',         ak7PFchsJetsL1Off),
    'ak1pfchsl1'           : ('ak1PFchsJetsL1Fast',        ak1PFchsJetsL1Fast),
    'ak2pfchsl1'           : ('ak2PFchsJetsL1Fast',        ak2PFchsJetsL1Fast),
    'ak3pfchsl1'           : ('ak3PFchsJetsL1Fast',        ak3PFchsJetsL1Fast),
    'ak4pfchsl1'           : ('ak4PFchsJetsL1Fast',        ak4PFchsJetsL1Fast),
    'ak5pfchsl1'           : ('ak5PFchsJetsL1Fast',        ak5PFchsJetsL1Fast),
    'ak6pfchsl1'           : ('ak6PFchsJetsL1Fast',        ak6PFchsJetsL1Fast),
    'ak7pfchsl1'           : ('ak7PFchsJetsL1Fast',        ak7PFchsJetsL1Fast),
    'ak8pfchsl1'           : ('ak8PFchsJetsL1Fast',        ak8PFchsJetsL1Fast),
    'ak9pfchsl1'           : ('ak9PFchsJetsL1Fast',        ak9PFchsJetsL1Fast),
    'ak10pfchsl1'          : ('ak10PFchsJetsL1Fast',       ak10PFchsJetsL1Fast),
    'ak5pfHLTl1'           : ('ak5PFHLTJetsL1Fast',        ak5PFHLTJetsL1Fast),
    'ak5pfHLTl1off'        : ('ak5PFHLTJetsL1Off',         ak5PFHLTJetsL1Off),
    'ak5pfchsHLTl1'        : ('ak5PFchsHLTJetsL1Fast',     ak5PFchsHLTJetsL1Fast),
    'ak5pfchsHLTl1off'     : ('ak5PFchsHLTJetsL1Off',      ak5PFchsHLTJetsL1Off),
    'ak1puppil1'           : ('ak1PUPPIJetsL1Fast',       ak1PUPPIJetsL1Fast),
    'ak2puppil1'           : ('ak2PUPPIJetsL1Fast',       ak2PUPPIJetsL1Fast),
    'ak3puppil1'           : ('ak3PUPPIJetsL1Fast',       ak3PUPPIJetsL1Fast),
    'ak4puppil1'           : ('ak4PUPPIJetsL1Fast',       ak4PUPPIJetsL1Fast),
    'ak5puppil1'           : ('ak5PUPPIJetsL1Fast',       ak5PUPPIJetsL1Fast),
    'ak6puppil1'           : ('ak6PUPPIJetsL1Fast',       ak6PUPPIJetsL1Fast),
    'ak7puppil1'           : ('ak7PUPPIJetsL1Fast',       ak7PUPPIJetsL1Fast),
    'ak8puppil1'           : ('ak8PUPPIJetsL1Fast',       ak8PUPPIJetsL1Fast),
    'ak9puppil1'           : ('ak9PUPPIJetsL1Fast',       ak9PUPPIJetsL1Fast),
    'ak10puppil1'          : ('ak10PUPPIJetsL1Fast',      ak10PUPPIJetsL1Fast),
    'ak5calol2l3'          : ('ak5CaloJetsL2L3',           ak5CaloJetsL2L3),
    'ak4calol2l3'          : ('ak4CaloJetsL2L3',           ak4CaloJetsL2L3),
    'ak7calol2l3'          : ('ak7CaloJetsL2L3',           ak7CaloJetsL2L3),
    'kt4calol2l3'          : ('kt4CaloJetsL2L3',           kt4CaloJetsL2L3),
    'kt6calol2l3'          : ('kt6CaloJetsL2L3',           kt6CaloJetsL2L3),
    'ak5caloHLTl2l3'       : ('ak5CaloHLTJetsL2L3',        ak5CaloHLTJetsL2L3),
#    'ak5jptl2l3'           : ('ak5JPTJetsL2L3',            ak5JPTJetsL2L3),
#    'ak7jptl2l3'           : ('ak7JPTJetsL2L3',            ak7JPTJetsL2L3),
    'ak4pfl2l3'            : ('ak4PFJetsL2L3',             ak4PFJetsL2L3),
    'ak5pfl2l3'            : ('ak5PFJetsL2L3',             ak5PFJetsL2L3),
    'ak7pfl2l3'            : ('ak7PFJetsL2L3',             ak7PFJetsL2L3),
    'kt4pfl2l3'            : ('kt4PFJetsL2L3',             kt4PFJetsL2L3),
    'kt6pfl2l3'            : ('kt6PFJetsL2L3',             kt6PFJetsL2L3),
    'ak5pfchsl2l3'         : ('ak5PFchsJetsL2L3',          ak5PFchsJetsL2L3),
    'ak7pfchsl2l3'         : ('ak7PFchsJetsL2L3',          ak7PFchsJetsL2L3),
    'ak5pfHLTl2l3'         : ('ak5PFHLTJetsL2L3',          ak5PFHLTJetsL2L3),
    'ak5pfchsHLTl2l3'      : ('ak5PFchsHLTJetsL2L3',       ak5PFchsHLTJetsL2L3),
    'ak1puppil2l3'         : ('ak1PUPPIJetsL2L3',       ak1PUPPIJetsL2L3),
    'ak2puppil2l3'         : ('ak2PUPPIJetsL2L3',       ak2PUPPIJetsL2L3),
    'ak3puppil2l3'         : ('ak3PUPPIJetsL2L3',       ak3PUPPIJetsL2L3),
    'ak4puppil2l3'         : ('ak4PUPPIJetsL2L3',       ak4PUPPIJetsL2L3),
    'ak5puppil2l3'         : ('ak5PUPPIJetsL2L3',       ak5PUPPIJetsL2L3),
    'ak6puppil2l3'         : ('ak6PUPPIJetsL2L3',       ak6PUPPIJetsL2L3),
    'ak7puppil2l3'         : ('ak7PUPPIJetsL2L3',       ak7PUPPIJetsL2L3),
    'ak8puppil2l3'         : ('ak8PUPPIJetsL2L3',       ak8PUPPIJetsL2L3),
    'ak9puppil2l3'         : ('ak9PUPPIJetsL2L3',       ak9PUPPIJetsL2L3),
    'ak10puppil2l3'        : ('ak10PUPPIJetsL2L3',      ak10PUPPIJetsL2L3),
    'ak4calol1l2l3'        : ('ak4CaloJetsL1FastL2L3',     ak4CaloJetsL1FastL2L3),
    'ak4calol1offl2l3'     : ('ak4CaloJetsL1L2L3',         ak4CaloJetsL1L2L3),
    'ak7calol1l2l3'        : ('ak7CaloJetsL1FastL2L3',     ak7CaloJetsL1FastL2L3),
    'ak7calol1offl2l3'     : ('ak7CaloJetsL1L2L3',         ak7CaloJetsL1L2L3),
    'kt4calol1l2l3'        : ('kt4CaloJetsL1FastL2L3',     kt4CaloJetsL1FastL2L3),
    'kt6calol1l2l3'        : ('kt6CaloJetsL1FastL2L3',     kt6CaloJetsL1FastL2L3),
    'ak5caloHLTl1l2l3'     : ('ak5CaloHLTJetsL1FastL2L3',  ak5CaloHLTJetsL1FastL2L3),
    'ak5caloHLTl1offl2l3'  : ('ak5CaloHLTJetsL1L2L3',      ak5CaloHLTJetsL1L2L3),
#	'ak5jptl1l2l3'         : ('ak5JPTJetsL1FastL2L3',      ak7JPTJetsL1FastL2L3),
#	'ak7jptl1l2l3'         : ('ak5JPTJetsL1FastL2L3',      ak7JPTJetsL1FastL2L3),
#    'ak5jptl1offl2l3'      : ('ak5JPTJetsL1L2L3',          ak5JPTJetsL1L2L3),
#    'ak7jptl1offl2l3'      : ('ak7JPTJetsL1L2L3',          ak7JPTJetsL1L2L3),
    'ak4pfl1l2l3'          : ('ak4PFJetsL1FastL2L3',       ak4PFJetsL1FastL2L3),
    'ak8pfl1l2l3'          : ('ak8PFJetsL1FastL2L3',       ak8PFJetsL1FastL2L3),
    'ak5pfl1l2l3'          : ('ak5PFJetsL1FastL2L3',       ak5PFJetsL1FastL2L3),
    'ak5pfl1offl2l3'       : ('ak5PFJetsL1L2L3',           ak5PFJetsL1L2L3),
    'ak7pfl1l2l3'          : ('ak7PFJetsL1FastL2L3',       ak7PFJetsL1FastL2L3),
    'ak7pfl1offl2l3'       : ('ak7PFJetsL1L2L3',           ak7PFJetsL1L2L3),
    'kt4pfl1l2l3'          : ('kt4PFJetsL1FastL2L3',       kt4PFJetsL1FastL2L3),
    'kt6pfl1l2l3'          : ('kt6PFJetsL1FastL2L3',       kt6PFJetsL1FastL2L3),
    'ak4pfchsl1l2l3'       : ('ak4PFchsJetsL1FastL2L3',    ak4PFchsJetsL1FastL2L3),
    'ak8pfchsl1l2l3'       : ('ak8PFchsJetsL1FastL2L3',    ak8PFchsJetsL1FastL2L3),
    'ak5pfchsl1l2l3'       : ('ak5PFchsJetsL1FastL2L3',    ak5PFchsJetsL1FastL2L3),
    'ak5pfchsl1offl2l3'    : ('ak5PFchsJetsL1L2L3',        ak5PFchsJetsL1L2L3),
    'ak7pfchsl1l2l3'       : ('ak7PFchsJetsL1FastL2L3',    ak7PFchsJetsL1FastL2L3),
    'ak7pfchsl1offl2l3'    : ('ak7PFchsJetsL1L2L3',        ak7PFchsJetsL1L2L3),
    'ak5pfHLTl1l2l3'       : ('ak5PFHLTJetsL1FastL2L3',    ak5PFHLTJetsL1FastL2L3),
    'ak5pfHLTl1offl2l3'    : ('ak5PFHLTJetsL1L2L3',        ak5PFHLTJetsL1L2L3),
    'ak5pfchsHLTl1l2l3'    : ('ak5PFchsHLTJetsL1FastL2L3', ak5PFchsHLTJetsL1FastL2L3),
    'ak5pfchsHLTl1offl2l3' : ('ak5PFchsHLTJetsL1L2L3',     ak5PFchsHLTJetsL1L2L3),
    'ak1puppil1l2l3'       : ('ak1PUPPIJetsL1FastL2L3',    ak1PUPPIJetsL1FastL2L3),
    'ak2puppil1l2l3'       : ('ak2PUPPIJetsL1FastL2L3',    ak2PUPPIJetsL1FastL2L3),
    'ak3puppil1l2l3'       : ('ak3PUPPIJetsL1FastL2L3',    ak3PUPPIJetsL1FastL2L3),
    'ak4puppil1l2l3'       : ('ak4PUPPIJetsL1FastL2L3',    ak4PUPPIJetsL1FastL2L3),
    'ak5puppil1l2l3'       : ('ak5PUPPIJetsL1FastL2L3',    ak5PUPPIJetsL1FastL2L3),
    'ak6puppil1l2l3'       : ('ak6PUPPIJetsL1FastL2L3',    ak6PUPPIJetsL1FastL2L3),
    'ak7puppil1l2l3'       : ('ak7PUPPIJetsL1FastL2L3',    ak7PUPPIJetsL1FastL2L3),
    'ak8puppil1l2l3'       : ('ak8PUPPIJetsL1FastL2L3',    ak8PUPPIJetsL1FastL2L3),
    'ak9puppil1l2l3'       : ('ak9PUPPIJetsL1FastL2L3',    ak9PUPPIJetsL1FastL2L3),
    'ak10puppil1l2l3'      : ('ak10PUPPIJetsL1FastL2L3',   ak10PUPPIJetsL1FastL2L3),
}


################################################################################
## addAlgorithm
################################################################################
def addAlgorithm(process, alg_size_type_corr, Defaults, reco, doProducer):
    """
    addAlgorithm takes the following parameters:
    ============================================
      process:            the CMSSW process object
      alg_size_type_corr: a string, e.g. kt4calol2l3
                          alg=kt4, size=0.4, type=calo, corr=l2l3
      reco:               indicates wether the jets should be reconstructed
	  prod                indicates if the output ntuple should be in EDM format
	                      using an EDProducer or in the JRA Ntuple format unsing
						  the EDAnalyzer
    it will then create a complete sequence within an executable path
    to kinematically select references and jets, select partons and match
    them to the references, match references and jets, and finally execute
    the JetResponseAnalyzer.
    """
    ## deterine algorithm, size, type (Calo|PF|Track|JPT), and wether to apply jec
    alg_size      = ''
    type          = ''
    alg_size_type = ''
    correctl1     = False
    correctl1off  = False
    correctl2l3   = False
    if (alg_size_type_corr.find('caloHLT') > 0) :
        alg_size      = alg_size_type_corr[0:alg_size_type_corr.find('caloHLT')]
        type          = 'CaloHLT'
        alg_size_type = alg_size + 'caloHLT'
    elif (alg_size_type_corr.find('calo') > 0) :
        alg_size      = alg_size_type_corr[0:alg_size_type_corr.find('calo')]
        type          = 'Calo'
        alg_size_type = alg_size + 'calo'
    elif (alg_size_type_corr.find('pfHLT') > 0) :
        alg_size      = alg_size_type_corr[0:alg_size_type_corr.find('pfHLT')]
        type          = 'PFHLT'
        alg_size_type = alg_size + 'pfHLT'
    elif (alg_size_type_corr.find('pfchsHLT') > 0) :
        alg_size      = alg_size_type_corr[0:alg_size_type_corr.find('pfchsHLT')]
        type          = 'PFchsHLT'
        alg_size_type = alg_size + 'pfchsHLT'
    elif (alg_size_type_corr.find('pfchs') > 0) :
        alg_size      = alg_size_type_corr[0:alg_size_type_corr.find('pfchs')]
        type          = 'PFchs'
        alg_size_type = alg_size + 'pfchs'
    elif (alg_size_type_corr.find('pf') > 0) :
        alg_size      = alg_size_type_corr[0:alg_size_type_corr.find('pf')]
        type          = 'PF'
        alg_size_type = alg_size + 'pf'
    elif (alg_size_type_corr.find('puppi') > 0) :
        alg_size      = alg_size_type_corr[0:alg_size_type_corr.find('puppi')]
        type          = 'PUPPI'
        alg_size_type = alg_size + 'puppi'
    elif (alg_size_type_corr.find('tau') > 0) :
        alg_size      = alg_size_type_corr[0:alg_size_type_corr.find('tau')]
        type          = 'TAU'
        if "tauHPSall" in alg_size_type_corr:
            alg_size_type = alg_size + 'tauHPSall'
        elif "tauTaNCall" in alg_size_type_corr:
            alg_size_type = alg_size + 'tauTaNCall'
        else:
            for tauDiscriminator in tauDiscriminatorDict:
                if tauDiscriminator in alg_size_type_corr:
                    alg_size_type = tauDiscriminator
            for tauDecayMode in tauDecayModeDict:
                if tauDecayModeDict[tauDecayMode] != "" and \
                   tauDecayMode in alg_size_type_corr:
                    alg_size_type += tauDecayMode

    elif (alg_size_type_corr.find('jpt') > 0) :
        alg_size      = alg_size_type_corr[0:alg_size_type_corr.find('jpt')]
        type          = 'JPT'
        alg_size_type = alg_size + 'jpt'
    elif (alg_size_type_corr.find('trk') > 0) :
        alg_size      = alg_size_type_corr[0:alg_size_type_corr.find('trk')]
        type          = 'Track'
        alg_size_type = alg_size + 'trk'
    else:
        raise ValueError("Can't identify valid jet type: calo|caloHLT|pf|pfchs|pfHLT|jpt|trk|tau|puppi")

    if (alg_size_type_corr.find('l1') > 0):
        correctl1 = True
        if (alg_size_type_corr.find('l1off') > 0):
            correctl1off = True
        if not reco:
            raise ValueError("Can't subtract PU without reco!")

    if (alg_size_type_corr.find('l2l3') > 0):
        correctl2l3 = True


    ## check that alg_size_type_corr refers to valid jet configuration
    try:
        not reco and stdGenJetsDict.keys().index(alg_size_type)
        not reco and stdRecJetsDict.keys().index(alg_size_type)
    except ValueError:
        raise ValueError("Algorithm unavailable in standard format: " + alg_size_type)

    try:
        reco and genJetsDict.keys().index(alg_size_type)
        reco and recJetsDict.keys().index(alg_size_type)
    except ValueError:
        raise ValueError("Invalid jet configuration: " + alg_size_type)

    try:
        correctl2l3 and corrJetsDict.keys().index(alg_size_type_corr)
    except ValueError:
        raise ValueError("Invalid jet correction: " + alg_size_type_corr)

    ## reference (genjet) kinematic selection
    refPtEta = cms.EDFilter('EtaPtMinCandViewRefSelector',
        Defaults.RefPtEta,
        src = cms.InputTag(genJetsDict[alg_size_type][0])
    )
    if not reco:
        refPtEta.src = stdGenJetsDict[alg_size_type]
    setattr(process, alg_size_type + 'GenPtEta', refPtEta)

    ## reco jet kinematic selection
    jetPtEta = cms.EDFilter('EtaPtMinCandViewRefSelector',
        Defaults.JetPtEta,
        src = cms.InputTag(recJetsDict[alg_size_type][0])
    )
    if not reco:
        jetPtEta.src = stdRecJetsDict[alg_size_type]
    setattr(process, alg_size_type_corr + 'PtEta', jetPtEta)

    ## create the sequence
    sequence = cms.Sequence(refPtEta * jetPtEta)

	#############################
    jetPtEtaUncor = jetPtEta.clone()
    setattr(process, alg_size_type_corr + 'PtEtaUncor', jetPtEtaUncor)
    sequence = cms.Sequence(sequence * jetPtEtaUncor)
	#############################

    ## correct jets
    corrLabel = ''
    if correctl1 or correctl2l3:
        process.load('JetMETAnalysis.JetAnalyzers.JetCorrection_cff')
        (corrLabel, corrJets) = corrJetsDict[alg_size_type_corr]
        setattr(process, corrLabel, corrJets)
        sequence = cms.Sequence(corrJets * sequence)

    ## add pu density calculation
    if not correctl1 and not correctl1off:
#        if type == 'CaloHLT': #added 02/15/2012
#            process.kt6CaloJets = kt6CaloJets
#            process.kt6CaloJets.doRhoFastjet = True
#            process.kt6CaloJets.Ghost_EtaMax = Defaults.kt6CaloJetParameters.Ghost_EtaMax.value()
#            process.kt6CaloJets.Rho_EtaMax   = Defaults.kt6CaloJetParameters.Rho_EtaMax
#            sequence = cms.Sequence(process.kt6CaloJets * sequence)
        if type == 'PFchsHLT': # I have to keep this for compatibility while it makes no effect
            process.kt6PFJets = kt6PFJets
            process.kt6PFJets.doRhoFastjet = True
            process.kt6PFJets.Ghost_EtaMax = Defaults.kt6PFJetParameters.Ghost_EtaMax.value()
            process.kt6PFJets.Rho_EtaMax   = Defaults.kt6PFJetParameters.Rho_EtaMax
            sequence = cms.Sequence(process.kt6PFJets * sequence)
#        elif type == 'PFHLT':
#            process.kt6PFJets = kt6PFJets
#            process.kt6PFJets.doRhoFastjet = True
#            process.kt6PFJets.Ghost_EtaMax = Defaults.kt6PFJetParameters.Ghost_EtaMax.value()
#            process.kt6PFJets.Rho_EtaMax   = Defaults.kt6PFJetParameters.Rho_EtaMax
#            sequence = cms.Sequence(process.kt6PFJets * sequence)
    elif correctl1 and not correctl1off:  #modified 10/10/2011
        if type == 'CaloHLT': #added 02/15/2012
            process.kt6CaloJets = kt6CaloJets
            process.kt6CaloJets.doRhoFastjet = True
            process.kt6CaloJets.Ghost_EtaMax = Defaults.kt6PFJetParameters.Ghost_EtaMax.value()
            process.kt6CaloJets.Rho_EtaMax   = Defaults.kt6PFJetParameters.Rho_EtaMax
            sequence = cms.Sequence(process.kt6CaloJets * sequence)
        elif type == 'Calo':
            process.kt6CaloJets = kt6CaloJets
            process.kt6CaloJets.doRhoFastjet = True
            process.kt6CaloJets.Ghost_EtaMax = Defaults.kt6CaloJetParameters.Ghost_EtaMax.value()
            process.kt6CaloJets.Rho_EtaMax   = Defaults.kt6CaloJetParameters.Rho_EtaMax
            sequence = cms.Sequence(process.kt6CaloJets * sequence)
        elif type == 'PFchs':
            process.kt6PFJets = kt6PFJets
            process.kt6PFJets.doRhoFastjet = True
            process.kt6PFJets.Ghost_EtaMax = Defaults.kt6PFJetParameters.Ghost_EtaMax.value()
            process.kt6PFJets.Rho_EtaMax   = Defaults.kt6PFJetParameters.Rho_EtaMax
            sequence = cms.Sequence(process.kt6PFJets * sequence)
        elif type == 'PFHLT':
            process.kt6PFJets = kt6PFJets
            process.kt6PFJets.doRhoFastjet = True
            process.kt6PFJets.Ghost_EtaMax = Defaults.kt6PFJetParameters.Ghost_EtaMax.value()
            process.kt6PFJets.Rho_EtaMax   = Defaults.kt6PFJetParameters.Rho_EtaMax
            sequence = cms.Sequence(process.kt6PFJets * sequence)
        elif type == 'PFchsHLT':
            process.kt6PFJets = kt6PFJets
            process.kt6PFJets.doRhoFastjet = True
            process.kt6PFJets.Ghost_EtaMax = Defaults.kt6PFJetParameters.Ghost_EtaMax.value()
            process.kt6PFJets.Rho_EtaMax   = Defaults.kt6PFJetParameters.Rho_EtaMax
            sequence = cms.Sequence(process.kt6PFJets * sequence)
        elif type == 'PF':
            process.kt6PFJets = kt6PFJets
            process.kt6PFJets.doRhoFastjet = True
            process.kt6PFJets.Ghost_EtaMax = Defaults.kt6PFJetParameters.Ghost_EtaMax.value()
            process.kt6PFJets.Rho_EtaMax   = Defaults.kt6PFJetParameters.Rho_EtaMax
            sequence = cms.Sequence(process.kt6PFJets * sequence)

    ## reconstruct jets
    if type == 'JPT':
        process.load('Configuration.Geometry.GeometryIdeal_cff')
        process.load('Configuration.StandardSequences.MagneticField_cff')
#        process.load('JetMETAnalysis.JetAnalyzers.JPTReconstruction_cff')
#        if   alg_size == 'ak5':
#            sequence = cms.Sequence(process.ak5JPTJetsSequence * sequence)
#        elif alg_size == 'ak7':
#            sequence = cms.Sequence(process.ak7JPTJetsSequence * sequence)
#        else:
#            raise ValueError(alg_size + " not supported for JPT!")
        if correctl1 or correctl2l3:
            jetPtEta.src = corrLabel
    elif reco:
        process.load('Configuration.Geometry.GeometryIdeal_cff')
        process.load('Configuration.StandardSequences.MagneticField_cff')
        (recLabel, recJets) = recJetsDict[alg_size_type]
        if correctl1 or correctl2l3:
            corrJets.src = recLabel
            jetPtEta.src = corrLabel
        else:
            jetPtEta.src = recLabel
        if correctl1:
            recJets.doAreaFastjet = True #Should this be on for L1Offset
            recJets.Rho_EtaMax    = cms.double(4.4) # FIX LATER
        recJets.jetPtMin = cms.double(3.0)
        setattr(process, recLabel, recJets)
        sequence = cms.Sequence(recJets * sequence)
        if type == 'PUPPI':
            process.load('CommonTools.PileupAlgos.Puppi_cff')
            #puppi.candName = cms.InputTag("particleFlow")
            sequence = cms.Sequence(puppi * sequence)
        if type == 'Track':
            process.load('JetMETAnalysis.JetAnalyzers.TrackJetReconstruction_cff')
            sequence = cms.Sequence(trackJetSequence * sequence)
        elif type == 'TAU':
            from PhysicsTools.PatAlgos.tools.helpers import massSearchReplaceParam
            from RecoTauTag.TauTagTools.PFTauSelector_cfi import pfTauSelector
            #process.load("TrackingTools/TransientTrack/TransientTrackBuilder_cfi")
            process.load("RecoTauTag/Configuration/RecoPFTauTag_cff")

            tauRecoSequence = cms.Sequence(process.recoTauCommonSequence)
            if "HPS" in alg_size_type:
                tauRecoSequence += process.recoTauClassicHPSSequence
            elif "TaNC" in alg_size_type:
                tauRecoSequence += process.recoTauHPSTancSequence

            (tauIsoDiscriminator, tauDecayMode) = tauDiscriminators_and_DecayModes[alg_size_type]

            if not (("HPSall" in alg_size_type or "TaNCall" in alg_size_type) and tauDecayMode == ""):
                tauDiscriminators = []
                if "HPS" in alg_size_type:
                    tauDiscriminators = [
                        "hpsPFTauDiscriminationByLooseElectronRejection",
                        "hpsPFTauDiscriminationByTightMuonRejection",
                        "hpsPFTauDiscriminationByDecayModeFinding",
                    ]
#                elif "TaNC" in alg_size_type:
#                    tauDiscriminators = [
#                        "hpsTancTausDiscriminationByLooseElectronRejection",
#                        "hpsTancTausDiscriminationByTightMuonRejection",
#                        "hpsTancTausDiscriminationByDecayModeSelection"
#                    ]
                tauDiscriminators.append(tauIsoDiscriminator)
#
                tauDiscriminatorConfigs = []
                for tauDiscriminator in tauDiscriminators:
                    tauDiscriminatorConfigs.append(
                        cms.PSet(
                            discriminator = cms.InputTag(tauDiscriminator),
                            selectionCut = cms.double(0.5)
                        )
                    )
#
                selTauModule = pfTauSelector.clone(
                    src = cms.InputTag(recLabel),
                    discriminators = cms.VPSet(tauDiscriminatorConfigs)
                )
		# merge OneProg1Pi0 and OneProng2Pi0
                if (tauDecayMode == "OneProng1Pi0" or tauDecayMode == "OneProng2Pi0"):
                    setattr(selTauModule, "cut", cms.string("decayMode() == 1 || decayMode() == 2"))
		else:
                     if tauDecayMode != "*":
                          #setattr(selTauModule, "cut", cms.string("isDecayMode('%s')" % tauDecayMode))
                          setattr(selTauModule, "cut", cms.string("decayMode() == %s" % tauDecayMode))
                selTauModuleName = alg_size_type + "Selected"
                setattr(process, selTauModuleName, selTauModule)
                tauRecoSequence += getattr(process, selTauModuleName)
#
#                jetPtEta.src = cms.InputTag(selTauModuleName)
#
            process.load("PhysicsTools.JetMCAlgos.TauGenJets_cfi")
#
            sequence = cms.Sequence(tauRecoSequence * process.tauGenJets * sequence)

    # reconstruct genjets
    if reco:
        (genLabel, genJets) = genJetsDict[alg_size_type]
        setattr(process, genLabel, genJets)

        sequence.replace(refPtEta, genJets * refPtEta)

        if type == 'Calo':
            setattr(process, 'genParticlesForJetsNoNu', genParticlesForJetsNoNu) #chaned to NoNu from NoMuNoNu
            sequence = cms.Sequence(genParticlesForJetsNoNu * sequence)
        else:
            setattr(process,'genParticlesForJetsNoNu',genParticlesForJetsNoNu)
            sequence = cms.Sequence(genParticlesForJetsNoNu * sequence)
        refPtEta.src = genJets.label()

    ## filter / map partons only if flavor information is requested
#    if Defaults.JetResponseParameters.doFlavor.value() :
#        setattr(process, 'partons', partons)
#        if reco: #added 02/29/2012
#            jetsTag = cms.InputTag(genJetsDict[alg_size_type][0])
#        else:
#            jetsTag = cms.InputTag(stdGenJetsDict[alg_size_type])
#        genToParton = cms.EDProducer('JetPartonMatcher',
#            jets = jetsTag,
#            #jets = cms.InputTag(genJets.label()), #commented out on 02/29/2012 for line above.
#            coneSizeToAssociate = cms.double(0.3),
#            partons = cms.InputTag('partons')
#        )
#        setattr(process, alg_size_type + 'GenToParton', genToParton)
#        sequence = cms.Sequence(sequence * partons * genToParton)

    ## reference to jet matching
    jetToRef = cms.EDProducer('MatchRecToGen',
        srcGen = cms.InputTag(refPtEta.label()),
        srcRec = cms.InputTag(jetPtEta.label())
    )
    setattr(process,alg_size_type_corr + 'JetToRef', jetToRef)
    sequence = cms.Sequence(sequence * jetToRef)

	##############################
    jetToUncorJet = cms.EDProducer('MatchRecToGen',
        srcGen = cms.InputTag(jetPtEtaUncor.label()),
        srcRec = cms.InputTag(jetPtEta.label())
    )
    setattr(process,alg_size_type_corr + 'JetToUncorJet', jetToUncorJet)
    sequence = cms.Sequence(sequence * jetToUncorJet)
	##############################

    ## jet response analyzer
    jraAnalyzer = 'JetResponseAnalyzer'
    jra = cms.EDAnalyzer(jraAnalyzer,
                         Defaults.JetResponseParameters,
                         srcRefToJetMap    = cms.InputTag(jetToRef.label(), 'gen2rec'),
                         srcJetUnMatch     = cms.InputTag(jetToRef.label(), 'unmaprec'),
                         srcRef            = cms.InputTag(refPtEta.label()),
                         jecLabel          = cms.string(''),
                         srcRhos           = cms.InputTag(''),
                         srcRho            = cms.InputTag(''),
                         srcRhoHLT         = cms.InputTag(''),
                         srcVtx            = cms.InputTag('hltPixelVertices'),
                         srcJetToUncorJetMap = cms.InputTag(jetToUncorJet.label(), 'rec2gen'),
                         srcPFCandidates   = cms.InputTag('')
                        )
    if doProducer:
        jraAnalyzer = 'JetResponseAnalyzerProducer'
        jra = cms.EDProducer(jraAnalyzer,
                             Defaults.JetResponseParameters,
                             srcRefToJetMap    = cms.InputTag(jetToRef.label(), 'gen2rec'),
                             srcRef            = cms.InputTag(refPtEta.label()),
                             jecLabel          = cms.string(''),
                             srcRho            = cms.InputTag(''),
                             srcRhoHLT         = cms.InputTag(''),
                             srcVtx            = cms.InputTag('offlinePrimaryVertices'),
                             srcJetToUncorJetMap = cms.InputTag(jetToUncorJet.label(), 'rec2gen'),
                             srcPFCandidates   = cms.InputTag('')
                             )

    if type == 'CaloHLT':
        jra.srcRho = ak4CaloL1Fastjet.srcRho #added 02/15/2012
        #jra.srcRhoHLT = ak5CaloHLTL1Fastjet.srcRho
        jra.srcRhoHLT = cms.InputTag('hltFixedGridRhoFastjetAllCalo') # for HLT usage
    elif type == 'Calo':
        jra.srcRho = ak4CaloL1Fastjet.srcRho #added 10/14/2011
    elif type == 'PFchs':
        process.kt6PFchsJetsRhos = kt6PFJets.clone(src = 'pfNoPileUpJME',
                                                   doFastJetNonUniform = cms.bool(True),
                                                   puCenters = cms.vdouble(-5,-4,-3,-2,-1,0,1,2,3,4,5),
                                                   puWidth = cms.double(.8),
                                                   nExclude = cms.uint32(2))
        sequence = cms.Sequence(process.kt6PFchsJetsRhos * sequence)
        jra.srcRhos = cms.InputTag("kt6PFchsJetsRhos", "rhos")
        jra.srcRho = cms.InputTag("fixedGridRhoFastjetAll")#ak4PFchsL1Fastjet.srcRho #added 10/14/2011
        jra.srcPFCandidates = cms.InputTag('pfNoPileUpJME')
    elif type == 'PFHLT':
        jra.srcRho = ak4PFL1Fastjet.srcRho #added 02/15/2012
        #jra.srcRhoHLT = ak5PFHLTL1Fastjet.srcRho
        jra.srcRhoHLT = cms.InputTag('hltFixedGridRhoFastjetAll') # for HLT usage
    elif type == 'PFchsHLT':
        jra.srcRho = ak4PFchsL1Fastjet.srcRho #added 02/15/2012
        jra.srcRhoHLT = ak5PFchsHLTL1Fastjet.srcRho
    elif type == 'PF':
        process.kt6PFJetsRhos = kt6PFJets.clone(doFastJetNonUniform = cms.bool(True),
                                                puCenters = cms.vdouble(-5,-4,-3,-2,-1,0,1,2,3,4,5),
                                                puWidth = cms.double(.8),
                                                nExclude = cms.uint32(2))
        sequence = cms.Sequence(process.kt6PFJetsRhos * sequence)
        jra.srcRhos = cms.InputTag("kt6PFJetsRhos", "rhos")
        jra.srcRho = ak4PFL1Fastjet.srcRho #added 10/14/2011
        jra.srcPFCandidates = cms.InputTag('particleFlow')
    elif type == 'PUPPI':
        process.kt6PFJetsRhos = kt6PFJets.clone(doFastJetNonUniform = cms.bool(True),
                                                puCenters = cms.vdouble(-5,-4,-3,-2,-1,0,1,2,3,4,5),
                                                puWidth = cms.double(.8), nExclude = cms.uint32(2))
        sequence = cms.Sequence(process.kt6PFJetsRhos * sequence)
        jra.srcRhos = cms.InputTag("kt6PFJetsRhos", "rhos")
        jra.srcRho = cms.InputTag("fixedGridRhoFastjetAll")#ak4PFchsL1Fastjet.srcRho
        jra.srcPFCandidates = cms.InputTag('puppi')
    elif type == 'JPT':
        jra.srcRho = ak4CaloL1Fastjet.srcRho

    if correctl1 or correctl2l3:
        jra.jecLabel = corrJets.correctors[0].replace("Corrector","")

#    if Defaults.JetResponseParameters.doFlavor.value():
#		jra.srcRefToPartonMap = cms.InputTag(genToParton.label())

    setattr(process,alg_size_type_corr,jra)
    sequence = cms.Sequence(sequence * jra)

    ## add chs to path is needed
    if type == 'PFchs':
        sequence = cms.Sequence(process.pfNoPileUpJMESequence * sequence)

    ## create the path and put in the sequence
    sequence = cms.Sequence(sequence)
    setattr(process, alg_size_type_corr + 'Sequence', sequence)
    path = cms.Path( sequence )
    setattr(process, alg_size_type_corr + 'Path', path)
    print alg_size_type_corr

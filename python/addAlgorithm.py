import FWCore.ParameterSet.Config as cms

################################################################################
## filter final state partons (define globaly)
################################################################################

partons = cms.EDProducer('PartonSelector',
    src = cms.InputTag('genParticles'),
    withLeptons = cms.bool(False)
)


################################################################################
## jet reconstruction
################################################################################
from JetMETAnalysis.JetAnalyzers.JetReconstruction_cff import *
from JetMETAnalysis.JetAnalyzers.TauReconstruction_cff import *
from JetMETAnalysis.JetAnalyzers.JPTReconstruction_cff import *
from JetMETAnalysis.JetAnalyzers.JetCorrection_cff     import *
from RecoTauTag.TauTagTools.tauDecayModes_cfi          import *

tauDiscriminatorDict = {
    "ak5tauHPSloose"            : "hpsPFTauDiscriminationByLooseIsolation",
    "ak5tauHPSmedium"           : "hpsPFTauDiscriminationByMediumIsolation",
    "ak5tauHPStight"            : "hpsPFTauDiscriminationByTightIsolation",
    "ak5tauHPSlooseDBcorr"      : "hpsPFTauDiscriminationByLooseIsolation",
    "ak5tauHPSmediumDBcorr"     : "hpsPFTauDiscriminationByMediumIsolation",
    "ak5tauHPStightDBcorr"      : "hpsPFTauDiscriminationByTightIsolation",
    "ak5tauHPSlooseCombDBcorr"  : "hpsPFTauDiscriminationByLooseIsolation",
    "ak5tauHPSmediumCombDBcorr" : "hpsPFTauDiscriminationByMediumIsolation",
    "ak5tauHPStightCombDBcorr"  : "hpsPFTauDiscriminationByTightIsolation",
    "ak5tauTaNCloose"           : "hpsTancTausDiscriminationByTancLoose",    
    "ak5tauTaNCmedium"          : "hpsTancTausDiscriminationByTancMedium",
    "ak5tauTaNCtight"           : "hpsTancTausDiscriminationByTancTight"
}

tauDecayModeDict = {
    "All"                       : "*",
    "OneProng0Pi0"              : "%i" % tauToOneProng0PiZero,
    "OneProng1Pi0"              : "%i" % tauToOneProng1PiZero,
    "OneProng2Pi0"              : "%i" % tauToOneProng2PiZero,
    "ThreeProng0Pi0"            : "%i" % tauToThreeProng0PiZero
}

tauDiscriminators_and_DecayModes = {}
for tauDiscriminator in tauDiscriminatorDict:
    for tauDecayMode in tauDecayModeDict:
        key = tauDiscriminator
        if tauDecayModeDict[tauDecayMode] != "":
            key += tauDecayMode
        tauDiscriminators_and_DecayModes[key] = (tauDiscriminatorDict[tauDiscriminator], tauDecayModeDict[tauDecayMode])

stdGenJetsDict = {
    'ak5calo'       : 'ak5GenJets',
    'ak7calo'       : 'ak7GenJets',
    'kt4calo'       : 'kt4GenJets',
    'kt6calo'       : 'kt6GenJets',
    'ak5caloHLT'    : 'ak5GenJets', #added 02/14/2012
    'ak5pf'         : 'ak5GenJets',
    'ak7pf'         : 'ak7GenJets',
    'kt4pf'         : 'kt4GenJets',
    'kt6pf'         : 'kt6GenJets',
    'ak5pfchs'      : 'ak5GenJets', #added 10/07/2011
    'ak7pfchs'      : 'ak7GenJets', #added 10/10/2011
    'ak5pfHLT'      : 'ak5GenJets', #added 02/14/2012
    'ak5pfchsHLT'   : 'ak5GenJets', #added 03/20/2012
    'ak5trk'        : 'ak5GenJets',
    'ak7trk'        : 'ak7GenJets',
    'kt4trk'        : 'kt4GenJets',
    'kt6trk'        : 'kt6GenJets',
    'ak5jpt'        : 'ak5GenJets',
    'ak5tauHPSall'  : 'tauGenJetsSelectorAllHadrons',
    'ak5tauTaNCall' : 'tauGenJetsSelectorAllHadrons'
}
for tauDiscriminator_and_DecayMode in tauDiscriminators_and_DecayModes:
    if   tauDiscriminator_and_DecayMode.find("HPS")  != -1:
        stdGenJetsDict[tauDiscriminator_and_DecayMode] = stdGenJetsDict["ak5tauHPSall"]
    elif tauDiscriminator_and_DecayMode.find("TaNC") != -1:
        stdGenJetsDict[tauDiscriminator_and_DecayMode] = stdGenJetsDict["ak5tauTaNCall"]

genJetsDict = {
    'ak5calo'       : ('ak5GenJetsNoNu',               ak5GenJetsNoNu), #chaned to NoNu from NoMuNoNu
    'ak7calo'       : ('ak7GenJetsNoNu',               ak7GenJetsNoNu),
    'kt4calo'       : ('kt4GenJetsNoNu',               kt4GenJetsNoNu),
    'kt6calo'       : ('kt6GenJetsNoNu',               kt6GenJetsNoNu),
    'ak5caloHLT'    : ('ak5GenJetsNoNu',               ak5GenJetsNoNu),
    'ak5pf'         : ('ak5GenJetsNoNu',               ak5GenJetsNoNu),
    'ak7pf'         : ('ak7GenJetsNoNu',               ak7GenJetsNoNu),
    'kt4pf'         : ('kt4GenJetsNoNu',               kt4GenJetsNoNu),
    'kt6pf'         : ('kt6GenJetsNoNu',               kt6GenJetsNoNu),
    'ak5pfchs'      : ('ak5GenJetsNoNu',               ak5GenJetsNoNu),
    'ak7pfchs'      : ('ak7GenJetsNoNu',               ak7GenJetsNoNu),
    'ak5pfHLT'      : ('ak5GenJetsNoNu',               ak5GenJetsNoNu),
    'ak5pfchsHLT'   : ('ak5GenJetsNoNu',               ak5GenJetsNoNu),
    'ak5trk'        : ('ak5GenJetsNoNu',               ak5GenJetsNoNu),
    'ak7trk'        : ('ak7GenJetsNoNu',               ak7GenJetsNoNu),
    'kt4trk'        : ('kt4GenJetsNoNu',               kt4GenJetsNoNu),
    'kt6trk'        : ('kt6GenJetsNoNu',               kt6GenJetsNoNu),
    'ak5jpt'        : ('ak5GenJetsNoNu',               ak5GenJetsNoNu),
    'ak7jpt'        : ('ak7GenJetsNoNu',               ak7GenJetsNoNu),
    'ak5tauHPSall'  : ('tauGenJetsSelectorAllHadrons', tauGenJetsSelectorAllHadrons),
    'ak5tauTaNCall' : ('tauGenJetsSelectorAllHadrons', tauGenJetsSelectorAllHadrons)
}    
for tauDiscriminator_and_DecayMode in tauDiscriminators_and_DecayModes:
    if   tauDiscriminator_and_DecayMode.find("HPS")  != -1:
        genJetsDict[tauDiscriminator_and_DecayMode] = genJetsDict["ak5tauHPSall"]
    elif tauDiscriminator_and_DecayMode.find("TaNC") != -1:
        genJetsDict[tauDiscriminator_and_DecayMode] = genJetsDict["ak5tauTaNCall"]

    
stdRecJetsDict = {
    'ak5calo'       : 'ak5CaloJets',
    'ak7calo'       : 'ak7CaloJets',
    'kt4calo'       : 'kt4CaloJets',
    'kt6calo'       : 'kt6CaloJets',
    'ak5caloHLT'    : 'hltAntiKT5CaloJets', #added 02/14/2012
    'ak5pf'         : 'ak5PFJets',
    'ak7pf'         : 'ak7PFJets',
    'kt4pf'         : 'kt4PFJets',
    'kt6pf'         : 'kt6PFJets',
    'ak5pfchs'      : 'ak5PFchsJets', #added 10/07/2011
    'ak7pfchs'      : 'ak7PFchsJets', #added 10/10/2011
    'ak5pfHLT'      : 'hltAntiKT5PFJets', #added 02/14/2012
    'ak5pfchsHLT'   : 'hltAntiKT5PFJetsNoPUPixelVert', #added 03/20/2012
    'ak5trk'        : 'ak5TrackJets',
    'kt4trk'        : 'kt4TrackJets',
    'ak5jpt'        : 'ak5JPTJets',
    'ak7jpt'        : 'ak7JPTJets',
    'ak5tauHPSall'  : 'hpsPFTauProducer',
    'ak5tauTaNCall' : 'hpsTancTaus'
}
for tauDiscriminator_and_DecayMode in tauDiscriminators_and_DecayModes:
    if   tauDiscriminator_and_DecayMode.find("HPS")  != -1:
        stdRecJetsDict[tauDiscriminator_and_DecayMode] = stdRecJetsDict["ak5tauHPSall"]
    elif tauDiscriminator_and_DecayMode.find("TaNC") != -1:
        stdRecJetsDict[tauDiscriminator_and_DecayMode] = stdRecJetsDict["ak5tauTaNCall"]

recJetsDict = {
    'ak5calo'       : ('ak5CaloJets',        ak5CaloJets),
    'ak7calo'       : ('ak7CaloJets',        ak7CaloJets),
    'kt4calo'       : ('kt4CaloJets',        kt4CaloJets),
    'kt6calo'       : ('kt6CaloJets',        kt6CaloJets),
    'ak5caloHLT'    : ('ak5CaloHLTJets',     ak5CaloHLTJets), #added 02/14/2012
    'ak5pf'         : ('ak5PFJets',          ak5PFJets),
    'ak7pf'         : ('ak7PFJets',          ak7PFJets),
    'kt4pf'         : ('kt4PFJets',          kt4PFJets),
    'kt6pf'         : ('kt6PFJets',          kt6PFJets),
    'ak5pfchs'      : ('ak5PFchsJets',       ak5PFchsJets), #added 10/07/2011
    'ak7pfchs'      : ('ak7PFchsJets',       ak7PFchsJets), #added 10/10/2011
    'ak5pfHLT'      : ('ak5PFHLTJets',       ak5PFHLTJets), #added 02/14/2012
    'ak5pfchsHLT'   : ('ak5PFchsHLTJets',    ak5PFchsHLTJets), #added 03/20/2012
    'ak5trk'        : ('ak5TrackJets',       ak5TrackJets),
    'kt4trk'        : ('kt4TrackJets',       kt4TrackJets),
    'ak5jpt'        : ('ak5JPTJets',         ak5JPTJets),
    'ak7jpt'        : ('ak7JPTJets',         ak7JPTJets),
    'ak5tauHPSall'  : ('hpsPFTauProducer',   hpsPFTauProducer),
    'ak5tauTaNCall' : ('hpsTancTaus',        hpsTancTaus)
}
for tauDiscriminator_and_DecayMode in tauDiscriminators_and_DecayModes:
    if   tauDiscriminator_and_DecayMode.find("HPS")  != -1:
        recJetsDict[tauDiscriminator_and_DecayMode] = recJetsDict["ak5tauHPSall"]
    elif tauDiscriminator_and_DecayMode.find("TaNC") != -1:
        recJetsDict[tauDiscriminator_and_DecayMode] = recJetsDict["ak5tauTaNCall"]


corrJetsDict = {
    'ak5calol1'            : ('ak5CaloJetsL1Fast',         ak5CaloJetsL1Fast),
    'ak5calol1off'         : ('ak5CaloJetsL1Off',          ak5CaloJetsL1Off), #added 10/10/2011
    'ak7calol1'            : ('ak7CaloJetsL1Fast',         ak7CaloJetsL1Fast),
    'ak7calol1off'         : ('ak7CaloJetsL1Off',          ak7CaloJetsL1Off), #added 10/10/2011
    'kt4calol1'            : ('kt4CaloJetsL1Fast',         kt4CaloJetsL1Fast),
    'kt6calol1'            : ('kt6CaloJetsL1Fast',         kt6CaloJetsL1Fast),
    'ak5caloHLTl1'         : ('ak5CaloHLTJetsL1Fast',      ak5CaloHLTJetsL1Fast), #added 02/14/2012
    'ak5caloHLTl1off'      : ('ak5CaloHLTJetsL1Off',       ak5CaloHLTJetsL1Off), #added 02/14/2012
    'ak5jptl1'             : ('ak5JPTJetsL1',              ak5JPTJetsL1),
    'ak7jptl1'             : ('ak7JPTJetsL1',              ak7JPTJetsL1),
    'ak5pfl1'              : ('ak5PFJetsL1Fast',           ak5PFJetsL1Fast),
    'ak5pfl1off'           : ('ak5PFJetsL1Off',            ak5PFJetsL1Off), #added 10/10/2011
    'ak7pfl1'              : ('ak7PFJetsL1Fast',           ak7PFJetsL1Fast),
    'ak7pfl1off'           : ('ak7PFJetsL1Off',            ak7PFJetsL1Off), #added 10/10/2011
    'kt4pfl1'              : ('kt4PFJetsL1Fast',           kt4PFJetsL1Fast),
    'kt6pfl1'              : ('kt6PFJetsL1Fast',           kt6PFJetsL1Fast),
    'ak5pfchsl1'           : ('ak5PFchsJetsL1Fast',        ak5PFchsJetsL1Fast), #added 10/07/2011
    'ak5pfchsl1off'        : ('ak5PFchsJetsL1Off',         ak5PFchsJetsL1Off), #added 10/010/2011    
    'ak7pfchsl1'           : ('ak7PFchsJetsL1Fast',        ak7PFchsJetsL1Fast), #added 10/10/2011
    'ak7pfchsl1off'        : ('ak7PFchsJetsL1Off',         ak7PFchsJetsL1Off), #added 10/10/2011
    'ak5pfHLTl1'           : ('ak5PFHLTJetsL1Fast',        ak5PFHLTJetsL1Fast), #added 02/14/2012
    'ak5pfHLTl1off'        : ('ak5PFHLTJetsL1Off',         ak5PFHLTJetsL1Off), #added 02/14/2012
    'ak5pfchsHLTl1'        : ('ak5PFchsHLTJetsL1Fast',     ak5PFchsHLTJetsL1Fast), #added 03/20/2012
    'ak5pfchsHLTl1off'     : ('ak5PFchsHLTJetsL1Off',      ak5PFchsHLTJetsL1Off), #added 03/20/2012
    'ak5calol2l3'          : ('ak5CaloJetsL2L3',           ak5CaloJetsL2L3),
    'ak7calol2l3'          : ('ak7CaloJetsL2L3',           ak7CaloJetsL2L3),
    'kt4calol2l3'          : ('kt4CaloJetsL2L3',           kt4CaloJetsL2L3),
    'kt6calol2l3'          : ('kt6CaloJetsL2L3',           kt6CaloJetsL2L3),
    'ak5caloHLTl2l3'       : ('ak5CaloHLTJetsL2L3',        ak5CaloHLTJetsL2L3), #added 02/14/2012
    'ak5jptl2l3'           : ('ak5JPTJetsL2L3',            ak5JPTJetsL2L3),
    'ak7jptl2l3'           : ('ak7JPTJetsL2L3',            ak7JPTJetsL2L3),
    'ak5pfl2l3'            : ('ak5PFJetsL2L3',             ak5PFJetsL2L3),
    'ak7pfl2l3'            : ('ak7PFJetsL2L3',             ak7PFJetsL2L3),
    'kt4pfl2l3'            : ('kt4PFJetsL2L3',             kt4PFJetsL2L3),
    'kt6pfl2l3'            : ('kt6PFJetsL2L3',             kt6PFJetsL2L3),
    'ak5pfchsl2l3'         : ('ak5PFchsJetsL2L3',          ak5PFchsJetsL2L3), #added 10/07/2011
    'ak7pfchsl2l3'         : ('ak7PFchsJetsL2L3',          ak7PFchsJetsL2L3), #added 10/10/2011
    'ak5pfHLTl2l3'         : ('ak5PFHLTJetsL2L3',          ak5PFHLTJetsL2L3), #added 02/14/2012
    'ak5pfchsHLTl2l3'      : ('ak5PFchsHLTJetsL2L3',       ak5PFchsHLTJetsL2L3), #added 03/20/2012
    'ak5calol1l2l3'        : ('ak5CaloJetsL1FastL2L3',     ak5CaloJetsL1FastL2L3),
    'ak5calol1offl2l3'     : ('ak5CaloJetsL1L2L3',         ak5CaloJetsL1L2L3), #added 10/10/2011
    'ak7calol1l2l3'        : ('ak7CaloJetsL1FastL2L3',     ak7CaloJetsL1FastL2L3),
    'ak7calol1offl2l3'     : ('ak7CaloJetsL1L2L3',         ak7CaloJetsL1L2L3), #added 10/10/2011
    'kt4calol1l2l3'        : ('kt4CaloJetsL1FastL2L3',     kt4CaloJetsL1FastL2L3),
    'kt6calol1l2l3'        : ('kt6CaloJetsL1FastL2L3',     kt6CaloJetsL1FastL2L3),
    'ak5caloHLTl1l2l3'     : ('ak5CaloHLTJetsL1FastL2L3',  ak5CaloHLTJetsL1FastL2L3), #added 02/14/2012
    'ak5caloHLTl1offl2l3'  : ('ak5CaloHLTJetsL1L2L3',      ak5CaloHLTJetsL1L2L3), #added 02/14/2012
    'ak5jptl1l2l3'         : ('ak5JPTJetsL1L2L3',          ak5JPTJetsL1L2L3),
    'ak7jptl1l2l3'         : ('ak7JPTJetsL1L2L3',          ak7JPTJetsL1L2L3),
    'ak5pfl1l2l3'          : ('ak5PFJetsL1FastL2L3',       ak5PFJetsL1FastL2L3),
    'ak5pfl1offl2l3'       : ('ak5PFJetsL1L2L3',           ak5PFJetsL1L2L3), #added 10/10/2011
    'ak7pfl1l2l3'          : ('ak7PFJetsL1FastL2L3',       ak7PFJetsL1FastL2L3),
    'ak7pfl1offl2l3'       : ('ak7PFJetsL1L2L3',           ak7PFJetsL1L2L3), #added 10/10/2011
    'kt4pfl1l2l3'          : ('kt4PFJetsL1FastL2L3',       kt4PFJetsL1FastL2L3),
    'kt6pfl1l2l3'          : ('kt6PFJetsL1FastL2L3',       kt6PFJetsL1FastL2L3),
    'ak5pfchsl1l2l3'       : ('ak5PFchsJetsL1FastL2L3',    ak5PFchsJetsL1FastL2L3), #added 10/07/2011
    'ak5pfchsl1offl2l3'    : ('ak5PFchsJetsL1L2L3',        ak5PFchsJetsL1L2L3), #added 10/07/2011
    'ak7pfchsl1l2l3'       : ('ak7PFchsJetsL1FastL2L3',    ak7PFchsJetsL1FastL2L3), #added 10/10/2011
    'ak7pfchsl1offl2l3'    : ('ak7PFchsJetsL1L2L3',        ak7PFchsJetsL1L2L3), #added 10/10/2011
    'ak5pfHLTl1l2l3'       : ('ak5PFHLTJetsL1FastL2L3',    ak5PFHLTJetsL1FastL2L3), #added 02/14/2012
    'ak5pfHLTl1offl2l3'    : ('ak5PFHLTJetsL1L2L3',        ak5PFHLTJetsL1L2L3), #added 02/14/2012
    'ak5pfchsHLTl1l2l3'    : ('ak5PFchsHLTJetsL1FastL2L3', ak5PFchsHLTJetsL1FastL2L3), #added 03/20/2012
    'ak5pfchsHLTl1offl2l3' : ('ak5PFchsHLTJetsL1L2L3',     ak5PFchsHLTJetsL1L2L3) #added 03/20/2012
}


################################################################################
## addAlgorithm
################################################################################
def addAlgorithm(process, alg_size_type_corr, Defaults, reco):
    """ 
    addAlgorithm takes the following parameters:
    ============================================
      process:            the CMSSW process object
      alg_size_type_corr: a string, e.g. kt4calol2l3
                          alg=kt4, size=0.4, type=calo, corr=l2l3
      reco:               indicates wether the jets should be reconstructed

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
        raise ValueError("Can't identify valid jet type: calo|caloHLT|pf|pfchs|pfHLT|jpt|trk|tau")
        
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

    ## correct jets
    corrLabel = ''
    if correctl1 or correctl2l3:
        process.load('JetMETAnalysis.JetAnalyzers.JetCorrection_cff')
        (corrLabel, corrJets) = corrJetsDict[alg_size_type_corr]
        setattr(process, corrLabel, corrJets)
        sequence = cms.Sequence(corrJets * sequence)

    ## add pu density calculation
    if not correctl1 and not correctl1off:
        if type == 'CaloHLT': #added 02/15/2012
            process.kt6CaloJets50 = kt6CaloJets50
            process.kt6CaloJets50.doRhoFastjet = True
            process.kt6CaloJets50.Ghost_EtaMax = 6.0
            process.kt6CaloJets50.Rho_EtaMax   = 0.5
            sequence = cms.Sequence(process.kt6CaloJets50 * sequence)
            process.kt6CaloJets = kt6CaloJets 
            process.kt6CaloJets.doRhoFastjet = True
            process.kt6CaloJets.Ghost_EtaMax = Defaults.kt6CaloJetParameters.Ghost_EtaMax.value()
            process.kt6CaloJets.Rho_EtaMax   = Defaults.kt6CaloJetParameters.Rho_EtaMax
            sequence = cms.Sequence(process.kt6CaloJets * sequence)
        elif type == 'PFchsHLT':
            process.kt6PFJets = kt6PFJets
            process.kt6PFJets.doRhoFastjet = True
            process.kt6PFJets.Ghost_EtaMax = Defaults.kt6PFJetParameters.Ghost_EtaMax.value()
            process.kt6PFJets.Rho_EtaMax   = Defaults.kt6PFJetParameters.Rho_EtaMax
            sequence = cms.Sequence(process.kt6PFJets * sequence)
            process.kt6PFJets50 = kt6PFJets50
            process.kt6PFJets50.doRhoFastjet = True
            process.kt6PFJets50.Ghost_EtaMax = 6.0
            process.kt6PFJets50.Rho_EtaMax   = 0.5
            sequence = cms.Sequence(process.kt6PFJets50 * sequence) 
        elif type == 'PFHLT':
            process.kt6PFJets = kt6PFJets
            process.kt6PFJets.doRhoFastjet = True
            process.kt6PFJets.Ghost_EtaMax = Defaults.kt6PFJetParameters.Ghost_EtaMax.value()
            process.kt6PFJets.Rho_EtaMax   = Defaults.kt6PFJetParameters.Rho_EtaMax
            sequence = cms.Sequence(process.kt6PFJets * sequence)
            process.kt6PFJets50 = kt6PFJets50
            process.kt6PFJets50.doRhoFastjet = True
            process.kt6PFJets50.Ghost_EtaMax = 6.0
            process.kt6PFJets50.Rho_EtaMax   = 0.5
            sequence = cms.Sequence(process.kt6PFJets50 * sequence)  
    elif correctl1 and not correctl1off:  #modified 10/10/2011
        if type == 'CaloHLT': #added 02/15/2012
            process.kt6CaloJets = kt6CaloJets 
            process.kt6CaloJets.doRhoFastjet = True
            process.kt6CaloJets.Ghost_EtaMax = Defaults.kt6PFJetParameters.Ghost_EtaMax.value()
            process.kt6CaloJets.Rho_EtaMax   = Defaults.kt6PFJetParameters.Rho_EtaMax
            sequence = cms.Sequence(process.kt6CaloJets * sequence)
            process.kt6CaloJets50 = kt6CaloJets50
            process.kt6CaloJets50.doRhoFastjet = True
            process.kt6CaloJets50.Ghost_EtaMax = 6.0
            process.kt6CaloJets50.Rho_EtaMax   = 0.5
            sequence = cms.Sequence(process.kt6CaloJets50 * sequence)
        elif type == 'Calo':
            process.kt6CaloJets = kt6CaloJets
            process.kt6CaloJets.doRhoFastjet = True
            process.kt6CaloJets.Ghost_EtaMax = Defaults.kt6CaloJetParameters.Ghost_EtaMax.value()
            process.kt6CaloJets.Rho_EtaMax   = Defaults.kt6CaloJetParameters.Rho_EtaMax
            sequence = cms.Sequence(process.kt6CaloJets * sequence)
            process.kt6CaloJets50 = kt6CaloJets50
            process.kt6CaloJets50.doRhoFastjet = True
            process.kt6CaloJets50.Ghost_EtaMax = 6.0
            process.kt6CaloJets50.Rho_EtaMax   = 0.5
            sequence = cms.Sequence(process.kt6CaloJets50 * sequence)
            process.kt6PFJets50 = kt6PFJets50
            process.kt6PFJets50.doRhoFastjet = True
            process.kt6PFJets50.Ghost_EtaMax = 6.0
            process.kt6PFJets50.Rho_EtaMax   = 0.5
            sequence = cms.Sequence(process.kt6PFJets50 * sequence) 
        elif type == 'PFchs':
            process.kt6PFJets = kt6PFJets
            process.kt6PFJets.doRhoFastjet = True
            process.kt6PFJets.Ghost_EtaMax = Defaults.kt6PFJetParameters.Ghost_EtaMax.value()
            process.kt6PFJets.Rho_EtaMax   = Defaults.kt6PFJetParameters.Rho_EtaMax
        elif type == 'PFHLT':
            process.kt6PFJets = kt6PFJets
            process.kt6PFJets.doRhoFastjet = True
            process.kt6PFJets.Ghost_EtaMax = Defaults.kt6PFJetParameters.Ghost_EtaMax.value()
            process.kt6PFJets.Rho_EtaMax   = Defaults.kt6PFJetParameters.Rho_EtaMax
            sequence = cms.Sequence(process.kt6PFJets * sequence)
            process.kt6PFJets50 = kt6PFJets50
            process.kt6PFJets50.doRhoFastjet = True
            process.kt6PFJets50.Ghost_EtaMax = 6.0
            process.kt6PFJets50.Rho_EtaMax   = 0.5
            sequence = cms.Sequence(process.kt6PFJets50 * sequence)
        elif type == 'PFchsHLT':
            process.kt6PFJets = kt6PFJets
            process.kt6PFJets.doRhoFastjet = True
            process.kt6PFJets.Ghost_EtaMax = Defaults.kt6PFJetParameters.Ghost_EtaMax.value()
            process.kt6PFJets.Rho_EtaMax   = Defaults.kt6PFJetParameters.Rho_EtaMax
            sequence = cms.Sequence(process.kt6PFJets * sequence)
            process.kt6PFJets50 = kt6PFJets50
            process.kt6PFJets50.doRhoFastjet = True
            process.kt6PFJets50.Ghost_EtaMax = 6.0
            process.kt6PFJets50.Rho_EtaMax   = 0.5
            sequence = cms.Sequence(process.kt6PFJets50 * sequence)
        elif type == 'PF':
            process.kt6PFJets = kt6PFJets
            process.kt6PFJets.doRhoFastjet = True
            process.kt6PFJets.Ghost_EtaMax = Defaults.kt6PFJetParameters.Ghost_EtaMax.value()
            process.kt6PFJets.Rho_EtaMax   = Defaults.kt6PFJetParameters.Rho_EtaMax
            sequence = cms.Sequence(process.kt6PFJets * sequence)
            process.kt6PFJets50 = kt6PFJets50
            process.kt6PFJets50.doRhoFastjet = True
            process.kt6PFJets50.Ghost_EtaMax = 6.0
            process.kt6PFJets50.Rho_EtaMax   = 0.5
            sequence = cms.Sequence(process.kt6PFJets50 * sequence) 
    
    ## reconstruct jets
    if type == 'JPT':
        process.load('Configuration.StandardSequences.Geometry_cff')
        process.load('Configuration.StandardSequences.MagneticField_cff')
        process.load('JetMETAnalysis.JetAnalyzers.JPTReconstruction_cff')
        if   alg_size == 'ak5':
            sequence = cms.Sequence(process.ak5JPTJetsSequence * sequence)
        elif alg_size == 'ak7':
            sequence = cms.Sequence(process.ak7JPTJetsSequence * sequence)
        else:
            raise ValueError(alg_size + " not supported for JPT!")
        if correctl1 or correctl2l3:
            jetPtEta.src = corrLabel
    elif reco:
        process.load('Configuration.StandardSequences.Geometry_cff')
        process.load('Configuration.StandardSequences.MagneticField_cff')
        (recLabel, recJets) = recJetsDict[alg_size_type]
        if correctl1 or correctl2l3:
            corrJets.src = recLabel
            jetPtEta.src = corrLabel
        else:
            jetPtEta.src = recLabel
        if correctl1:
            recJets.doAreaFastjet = True #Should this be on for L1Offset
            recJets.Rho_EtaMax    = cms.double(5.0) # FIX LATER
        setattr(process, recLabel, recJets)
        sequence = cms.Sequence(recJets * sequence)
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
                elif "TaNC" in alg_size_type:
                    tauDiscriminators = [
                        "hpsTancTausDiscriminationByLooseElectronRejection",
                        "hpsTancTausDiscriminationByTightMuonRejection",
                        "hpsTancTausDiscriminationByDecayModeSelection"
                    ]
                tauDiscriminators.append(tauIsoDiscriminator)
                                
                tauDiscriminatorConfigs = []
                for tauDiscriminator in tauDiscriminators:
                    tauDiscriminatorConfigs.append(
                        cms.PSet(
                            discriminator = cms.InputTag(tauDiscriminator),
                            selectionCut = cms.double(0.5)
                        )
                    )
                
                selTauModule = pfTauSelector.clone(
                    src = cms.InputTag(recLabel),
                    discriminators = cms.VPSet(tauDiscriminatorConfigs)
                )
                if tauDecayMode != "*":
                    #setattr(selTauModule, "cut", cms.string("isDecayMode('%s')" % tauDecayMode))
                    setattr(selTauModule, "cut", cms.string("decayMode() == %s" % tauDecayMode))
                selTauModuleName = alg_size_type + "Selected"
                setattr(process, selTauModuleName, selTauModule)
                tauRecoSequence += getattr(process, selTauModuleName)
                        
                jetPtEta.src = cms.InputTag(selTauModuleName)
            
            process.load("PhysicsTools.JetMCAlgos.TauGenJets_cfi")

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
    if Defaults.JetResponseParameters.doFlavor.value() :
        setattr(process, 'partons', partons)
        if reco: #added 02/29/2012
            jetsTag = cms.InputTag(genJetsDict[alg_size_type][0])
        else:
            jetsTag = cms.InputTag(stdGenJetsDict[alg_size_type])
        genToParton = cms.EDProducer('JetPartonMatcher',
            jets = jetsTag,
            #jets = cms.InputTag(genJets.label()), #commented out on 02/29/2012 for line above.
            coneSizeToAssociate = cms.double(0.3),
            partons = cms.InputTag('partons')
        )
        setattr(process, alg_size_type + 'GenToParton', genToParton)
        sequence = cms.Sequence(sequence * partons * genToParton)
        
    ## reference to jet matching
    jetToRef = cms.EDProducer('MatchRecToGen',
        srcGen = cms.InputTag(refPtEta.label()),
        srcRec = cms.InputTag(jetPtEta.label())
    )
    setattr(process,alg_size_type_corr + 'JetToRef', jetToRef)
    sequence = cms.Sequence(sequence * jetToRef)
    
    ## jet response analyzer
    jra = cms.EDAnalyzer('JetResponseAnalyzer',
        Defaults.JetResponseParameters,
        srcRefToJetMap    = cms.InputTag(jetToRef.label(), 'gen2rec'),
        srcRef            = cms.InputTag(refPtEta.label()),
        jecLabel          = cms.string(''),
        srcRho            = cms.InputTag(''),
        srcRho50          = cms.InputTag(''),
        srcRhoHLT         = cms.InputTag(''),
        srcVtx            = cms.InputTag('offlinePrimaryVertices')
    )

    if type == 'CaloHLT':
        jra.srcRho = ak5CaloL1Fastjet.srcRho #added 02/15/2012
        jra.srcRho50 = cms.InputTag('kt6CaloJets50','rho')
        jra.srcRhoHLT = ak5CaloHLTL1Fastjet.srcRho
    elif type == 'Calo':
        jra.srcRho = ak5CaloL1Fastjet.srcRho #added 10/14/2011
        jra.srcRho50 = cms.InputTag('kt6CaloJets50','rho')
    elif type == 'PFchs':
        jra.srcRho = ak5PFchsL1Fastjet.srcRho #added 10/14/2011
        jra.srcRho50 = cms.InputTag('kt6PFJets50','rho')
    elif type == 'PFHLT':
        jra.srcRho = ak5PFL1Fastjet.srcRho #added 02/15/2012
        jra.srcRho50 = cms.InputTag('kt6PFJets50','rho')
        jra.srcRhoHLT = ak5PFHLTL1Fastjet.srcRho
    elif type == 'PFchsHLT':
        jra.srcRho = ak5PFchsL1Fastjet.srcRho #added 02/15/2012
        jra.srcRho50 = cms.InputTag('kt6PFJets50','rho')
        jra.srcRhoHLT = ak5PFchsHLTL1Fastjet.srcRho
    elif type == 'PF':
        jra.srcRho = ak5PFL1Fastjet.srcRho #added 10/14/2011
        jra.srcRho50 = cms.InputTag('kt6PFJets50','rho')
    elif type == 'JPT':
        jra.srcRho = ak5CaloL1Fastjet.srcRho
        jra.srcRho50 = cms.InputTag('kt6CaloJets50','rho')

    if correctl1 or correctl2l3:
        jra.jecLabel = corrJets.correctors[0]

    if Defaults.JetResponseParameters.doFlavor.value():
        jra.srcRefToPartonMap = cms.InputTag(genToParton.label())
    setattr(process,alg_size_type_corr,jra)
    sequence = cms.Sequence(sequence * jra)
    
    ## add chs to path is needed
    if type == 'PFchs':
        sequence = cms.Sequence(process.pfNoPileUpSequence * sequence)

    ## create the path and put in the sequence
    sequence = cms.Sequence(sequence)
    setattr(process, alg_size_type_corr + 'Sequence', sequence)
    path = cms.Path( sequence )
    setattr(process, alg_size_type_corr + 'Path', path)
    print alg_size_type_corr

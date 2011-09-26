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
    "ThreeProng0Pi0"            : "%i" % tauToThreeProng0PiZero,
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
    'ak5pf'         : 'ak5GenJets',
    'ak7pf'         : 'ak7GenJets',
    'kt4pf'         : 'kt4GenJets',
    'kt6pf'         : 'kt6GenJets',
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
    'ak5calo'       : ('ak5GenJetsNoMuNoNu',           ak5GenJetsNoMuNoNu),
    'ak7calo'       : ('ak7GenJetsNoMuNoNu',           ak7GenJetsNoMuNoNu),
    'kt4calo'       : ('kt4GenJetsNoMuNoNu',           kt4GenJetsNoMuNoNu),
    'kt6calo'       : ('kt6GenJetsNoMuNoNu',           kt6GenJetsNoMuNoNu),
    'ak5pf'         : ('ak5GenJetsNoNu',               ak5GenJetsNoNu),
    'ak7pf'         : ('ak7GenJetsNoNu',               ak7GenJetsNoNu),
    'kt4pf'         : ('kt4GenJetsNoNu',               kt4GenJetsNoNu),
    'kt6pf'         : ('kt6GenJetsNoNu',               kt6GenJetsNoNu),
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
    'ak5pf'         : 'ak5PFJets',
    'ak7pf'         : 'ak7PFJets',
    'kt4pf'         : 'kt4PFJets',
    'kt6pf'         : 'kt6PFJets',
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
    'ak5calo'       : ('ak5CaloJets',      ak5CaloJets),
    'ak7calo'       : ('ak7CaloJets',      ak7CaloJets),
    'kt4calo'       : ('kt4CaloJets',      kt4CaloJets),
    'kt6calo'       : ('kt6CaloJets',      kt6CaloJets),
    'ak5pf'         : ('ak5PFJets',        ak5PFJets),
    'ak7pf'         : ('ak7PFJets',        ak7PFJets),
    'kt4pf'         : ('kt4PFJets',        kt4PFJets),
    'kt6pf'         : ('kt6PFJets',        kt6PFJets),
    'ak5trk'        : ('ak5TrackJets',     ak5TrackJets),
    'kt4trk'        : ('kt4TrackJets',     kt4TrackJets),
    'ak5jpt'        : ('ak5JPTJets',       ak5JPTJets),
    'ak7jpt'        : ('ak7JPTJets',       ak7JPTJets),
    'ak5tauHPSall'  : ('hpsPFTauProducer', hpsPFTauProducer),
    'ak5tauTaNCall' : ('hpsTancTaus',      hpsTancTaus)
}
for tauDiscriminator_and_DecayMode in tauDiscriminators_and_DecayModes:
    if   tauDiscriminator_and_DecayMode.find("HPS")  != -1:
        recJetsDict[tauDiscriminator_and_DecayMode] = recJetsDict["ak5tauHPSall"]
    elif tauDiscriminator_and_DecayMode.find("TaNC") != -1:
        recJetsDict[tauDiscriminator_and_DecayMode] = recJetsDict["ak5tauTaNCall"]


corrJetsDict = {
    'ak5calol1'     : ('ak5CaloJetsL1Fast',     ak5CaloJetsL1Fast),
    'ak7calol1'     : ('ak7CaloJetsL1Fast',     ak7CaloJetsL1Fast),
    'kt4calol1'     : ('kt4CaloJetsL1Fast',     kt4CaloJetsL1Fast),
    'kt6calol1'     : ('kt6CaloJetsL1Fast',     kt6CaloJetsL1Fast),
    'ak5jptl1'      : ('ak5JPTJetsL1',          ak5JPTJetsL1),
    'ak7jptl1'      : ('ak7JPTJetsL1',          ak7JPTJetsL1),
    'ak5pfl1'       : ('ak5PFJetsL1Fast',       ak5PFJetsL1Fast),
    'ak7pfl1'       : ('ak7PFJetsL1Fast',       ak7PFJetsL1Fast),
    'kt4pfl1'       : ('kt4PFJetsL1Fast',       kt4PFJetsL1Fast),
    'kt6pfl1'       : ('kt6PFJetsL1Fast',       kt6PFJetsL1Fast),
    'ak5calol2l3'   : ('ak5CaloJetsL2L3',       ak5CaloJetsL2L3),
    'ak7calol2l3'   : ('ak7CaloJetsL2L3',       ak7CaloJetsL2L3),
    'kt4calol2l3'   : ('kt4CaloJetsL2L3',       kt4CaloJetsL2L3),
    'kt6calol2l3'   : ('kt6CaloJetsL2L3',       kt6CaloJetsL2L3),
    'ak5jptl2l3'    : ('ak5JPTJetsL2L3',        ak5JPTJetsL2L3),
    'ak7jptl2l3'    : ('ak7JPTJetsL2L3',        ak7JPTJetsL2L3),
    'ak5pfl2l3'     : ('ak5PFJetsL2L3',         ak5PFJetsL2L3),
    'ak7pfl2l3'     : ('ak7PFJetsL2L3',         ak7PFJetsL2L3),
    'kt4pfl2l3'     : ('kt4PFJetsL2L3',         kt4PFJetsL2L3),
    'kt6pfl2l3'     : ('kt6PFJetsL2L3',         kt6PFJetsL2L3),
    'ak5calol1l2l3' : ('ak5CaloJetsL1FastL2L3', ak5CaloJetsL1FastL2L3),
    'ak7calol1l2l3' : ('ak7CaloJetsL1FastL2L3', ak7CaloJetsL1FastL2L3),
    'kt4calol1l2l3' : ('kt4CaloJetsL1FastL2L3', kt4CaloJetsL1FastL2L3),
    'kt6calol1l2l3' : ('kt6CaloJetsL1FastL2L3', kt6CaloJetsL1FastL2L3),
    'ak5jptl1l2l3'  : ('ak5JPTJetsL1L2L3',      ak5JPTJetsL1L2L3),
    'ak7jptl1l2l3'  : ('ak7JPTJetsL1L2L3',      ak7JPTJetsL1L2L3),
    'ak5pfl1l2l3'   : ('ak5PFJetsL1FastL2L3',   ak5PFJetsL1FastL2L3),
    'ak7pfl1l2l3'   : ('ak7PFJetsL1FastL2L3',   ak7PFJetsL1FastL2L3),
    'kt4pfl1l2l3'   : ('kt4PFJetsL1FastL2L3',   kt4PFJetsL1FastL2L3),
    'kt6pfl1l2l3'   : ('kt6PFJetsL1FastL2L3',   kt6PFJetsL1FastL2L3)
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
    correctl2l3   = False
    if (alg_size_type_corr.find('calo') > 0) :
        alg_size      = alg_size_type_corr[0:alg_size_type_corr.find('calo')]
        type          = 'Calo'
        alg_size_type = alg_size + 'calo'
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
        raise ValueError("Can't identify valid jet type: calo|pf|jpt|trk|tau")
        
    if (alg_size_type_corr.find('l1') > 0):
        correctl1 = True
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
    if correctl1:
        if type == 'Calo':
            process.kt6CaloJets = kt6CaloJets
            process.kt6CaloJets.doRhoFastjet = True
            process.kt6CaloJets.Ghost_EtaMax = 5.0
            process.kt6CaloJets.Rho_EtaMax   = cms.double(4.4) # FIX LATER
            sequence = cms.Sequence(process.kt6CaloJets * sequence)
        elif type == 'PF':
            process.kt6PFJets = kt6PFJets
            process.kt6PFJets.doRhoFastjet = True
            process.kt6PFJets.Ghost_EtaMax = 5.0
            process.kt6PFJets.Rho_EtaMax   = cms.double(4.4) # FIX LATER
            sequence = cms.Sequence(process.kt6PFJets * sequence)
    
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
            recJets.doAreaFastjet = True
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
            setattr(process, 'genParticlesForJetsNoMuNoNu', genParticlesForJetsNoMuNoNu)
            sequence = cms.Sequence(genParticlesForJetsNoMuNoNu * sequence)
        else:
            setattr(process,'genParticlesForJetsNoNu',genParticlesForJetsNoNu)
            sequence = cms.Sequence(genParticlesForJetsNoNu * sequence)
        refPtEta.src = genJets.label()
        
    ## filter / map partons only if flavor information is requested
    if Defaults.JetResponseParameters.doFlavor.value() :
        setattr(process, 'partons', partons)
        genToParton = cms.EDProducer('JetPartonMatcher',
            jets = cms.InputTag(genJets.label()),
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
        jecLabel          = cms.string('')            
    )
    if correctl1 or correctl2l3:
        jra.jecLabel = corrJets.correctors[0]
    if Defaults.JetResponseParameters.doFlavor.value():
        jra.srcRefToPartonMap = cms.InputTag(genToParton.label())
    setattr(process,alg_size_type_corr,jra)
    sequence = cms.Sequence(sequence * jra)
    
    ## create the path and put in the sequence
    sequence = cms.Sequence(sequence)
    setattr(process, alg_size_type_corr + 'Sequence', sequence)
    path = cms.Path( sequence )
    setattr(process, alg_size_type_corr + 'Path', path)

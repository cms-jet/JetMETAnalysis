import FWCore.ParameterSet.Config as cms

################################################################################
## filter final state partons (define globaly)
################################################################################

partons = cms.EDProducer('PartonSelector',
    src = cms.InputTag('genParticles'),
    withLeptons = cms.bool(False),
    skipFirstN = cms.uint32(0)
)


################################################################################
## jet reconstruction
################################################################################
from JetMETAnalysis.JetAnalyzers.JetReconstruction_cff import *
from JetMETAnalysis.JetAnalyzers.TauReconstruction_cff import *
from JetMETAnalysis.JetAnalyzers.JPTReconstruction_cff import *
from JetMETAnalysis.JetAnalyzers.JetCorrection_cff     import *
from RecoTauTag.TauTagTools.tauDecayModes_cfi          import *
from CommonTools.PileupAlgos.Puppi_cff import *

stdClusteringAlgorithms = ['ak'] #Options: {ak,kt}
stdJetTypes = ['calo','pf','pfchs','puppi'] #Options: {'calo','pf','pfchs','puppi'}
stdCorrectionLevels = {
    'l1'     : 'L1',
    'l2l3'   : 'L2L3',
    'l1l2l3' : 'L1FastL2L3'
} #Options{l1,l2l3,l1l2l3}

stdGenJetsDict = {}
genJetsDict = {}
stdRecJetsDict = {}
recJetsDict = {}
corrJetsDict = {}

for ca in stdClusteringAlgorithms:
    for jt in stdJetTypes:
        for r in range(1,11):
            alg_size_type = str(ca)+str(r)+str(jt)

            ## Generator Jets
            tmpString = str(ca)+str(r)+"GenJets"
            stdGenJetsDict[alg_size_type] = tmpString
            tmpString = str(ca)+str(r)+"GenJetsNoNu"
            genJetsDict[alg_size_type] = (tmpString,eval(tmpString))

            ## Reconstructed Jets
            if jt == 'calo' :
                tmpString = str(ca)+str(r)+str(jt).capitalize()+'Jets'
            else :
                tmpString = str(ca)+str(r)+str(jt).upper()+'Jets'
            stdRecJetsDict[alg_size_type] = tmpString
            recJetsDict[alg_size_type] = (tmpString,eval(tmpString))

            ## Corrected Jets
            for cl in stdCorrectionLevels :
                if ca == 'kt': continue
                if jt == 'calo' and not r in [4,7]: continue
                alg_size_type_corr = alg_size_type+cl
                if jt == 'calo' :
                    tmpString = str(ca)+str(r)+str(jt).capitalize()+'Jets'+str(stdCorrectionLevels[cl])
                else :
                    tmpString = str(ca)+str(r)+str(jt).upper()+'Jets'+str(stdCorrectionLevels[cl])
                corrJetsDict[alg_size_type_corr] = (tmpString,eval(tmpString))

## Extra JPT Collections
stdGenJetsDict['ak4jpt']     = 'ak4GenJets'
genJetsDict['ak4jpt']        = ('ak4GenJetsNoNu',       ak4GenJetsNoNu)
stdRecJetsDict['ak4jpt']     = 'ak4JPTJets'
recJetsDict['ak4jpt']        = ('ak4JPTJets',           ak4JPTJets)
corrJetsDict['ak4jptl1']     = ('ak4JPTJetsL1',         ak4JPTJetsL1)
corrJetsDict['ak4jptl2l3']   = ('ak4JPTJetsL2L3',       ak4JPTJetsL2L3)
corrJetsDict['ak4jptl1l2l3'] = ('ak4JPTJetsL1FastL2L3', ak4JPTJetsL1FastL2L3)
'''
## Extra TRK Jet Collections
stdGenJetsDict['ak5trk'] = 'ak5GenJets'
stdGenJetsDict['ak7trk'] = 'ak7GenJets'
stdGenJetsDict['kt4trk'] = 'kt4GenJets'
stdGenJetsDict['kt6trk'] = 'kt6GenJets'
genJetsDict['ak5trk']    = ('ak5GenJetsNoNu', ak5GenJetsNoNu)
genJetsDict['ak7trk']    = ('ak7GenJetsNoNu', ak7GenJetsNoNu)
genJetsDict['kt4trk']    = ('kt4GenJetsNoNu', kt4GenJetsNoNu)
genJetsDict['kt6trk']    = ('kt6GenJetsNoNu', kt6GenJetsNoNu)
stdRecJetsDict['ak5trk'] = 'ak5TrackJets'
stdRecJetsDict['kt4trk'] = 'kt4TrackJets'
recJetsDict['ak5trk']    = ('ak5TrackJets',   ak5TrackJets)
recJetsDict['kt4trk']    = ('kt4TrackJets',   kt4TrackJets)

## Extra kT Jet Collections
corrJetsDict['kt4pfl1'    ] = ('kt4PFJetsL1',         kt4PFJetsL1),
corrJetsDict['kt6pfl1'    ] = ('kt6PFJetsL1',         kt6PFJetsL1),
corrJetsDict['kt4pfl2l3'  ] = ('kt4PFJetsL2L3',       kt4PFJetsL2L3),
corrJetsDict['kt6pfl2l3'  ] = ('kt6PFJetsL2L3',       kt6PFJetsL2L3),
corrJetsDict['kt4pfl1l2l3'] = ('kt4PFJetsL1FastL2L3', kt4PFJetsL1FastL2L3),
corrJetsDict['kt6pfl1l2l3'] = ('kt6PFJetsL1FastL2L3', kt6PFJetsL1FastL2L3),
'''
## Extra Tau Collections
stdGenJetsDict['ak5tauHPSall'] = 'tauGenJetsSelectorAllHadrons'
genJetsDict['ak5tauHPSall']    = ('tauGenJetsSelectorAllHadrons', tauGenJetsSelectorAllHadrons)
stdRecJetsDict['ak5tauHPSall'] = 'hpsPFTauProducer'
recJetsDict['ak5tauHPSall']    = ('hpsPFTauProducer',   hpsPFTauProducer)

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

for tauDiscriminator_and_DecayMode in tauDiscriminators_and_DecayModes:
    if   tauDiscriminator_and_DecayMode.find("HPS")  != -1:
        stdGenJetsDict[tauDiscriminator_and_DecayMode] = stdGenJetsDict["ak5tauHPSall"]
    elif tauDiscriminator_and_DecayMode.find("TaNC") != -1:
        stdGenJetsDict[tauDiscriminator_and_DecayMode] = stdGenJetsDict["ak5tauTaNCall"]

for tauDiscriminator_and_DecayMode in tauDiscriminators_and_DecayModes:
    if   tauDiscriminator_and_DecayMode.find("HPS")  != -1:
        genJetsDict[tauDiscriminator_and_DecayMode] = genJetsDict["ak5tauHPSall"]
    elif tauDiscriminator_and_DecayMode.find("TaNC") != -1:
        genJetsDict[tauDiscriminator_and_DecayMode] = genJetsDict["ak5tauTaNCall"]

for tauDiscriminator_and_DecayMode in tauDiscriminators_and_DecayModes:
    if   tauDiscriminator_and_DecayMode.find("HPS")  != -1:
        stdRecJetsDict[tauDiscriminator_and_DecayMode] = stdRecJetsDict["ak5tauHPSall"]
    elif tauDiscriminator_and_DecayMode.find("TaNC") != -1:
        stdRecJetsDict[tauDiscriminator_and_DecayMode] = stdRecJetsDict["ak5tauTaNCall"]

for tauDiscriminator_and_DecayMode in tauDiscriminators_and_DecayModes:
    if   tauDiscriminator_and_DecayMode.find("HPS")  != -1:
        recJetsDict[tauDiscriminator_and_DecayMode] = recJetsDict["ak5tauHPSall"]
    elif tauDiscriminator_and_DecayMode.find("TaNC") != -1:
        recJetsDict[tauDiscriminator_and_DecayMode] = recJetsDict["ak5tauTaNCall"]


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
        if not reco and type!='Calo':
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
        sequence = cms.Sequence(eval(corrLabel.replace("Jets","")+"CorrectorChain") * corrJets * sequence)

    ## add pu density calculation
    if not correctl1 and not correctl1off:
        if type == 'CaloHLT': #added 02/15/2012
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
        elif type == 'PFHLT':
            process.kt6PFJets = kt6PFJets
            process.kt6PFJets.doRhoFastjet = True
            process.kt6PFJets.Ghost_EtaMax = Defaults.kt6PFJetParameters.Ghost_EtaMax.value()
            process.kt6PFJets.Rho_EtaMax   = Defaults.kt6PFJetParameters.Rho_EtaMax
            sequence = cms.Sequence(process.kt6PFJets * sequence)
    elif correctl1 and not correctl1off:  #modified 10/10/2011
        if type == 'CaloHLT': #added 02/15/2012
            process.kt6CaloJets = kt6CaloJets 
            process.kt6CaloJets.doRhoFastjet = True
            process.kt6CaloJets.Ghost_EtaMax = Defaults.kt6PFJetParameters.Ghost_EtaMax.value()
            process.kt6CaloJets.Rho_EtaMax   = Defaults.kt6PFJetParameters.Rho_EtaMax
            sequence = cms.Sequence(process.kt6CaloJets * sequence)
        elif type == 'Calo' and reco:
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
        process.load('JetMETAnalysis.JetAnalyzers.JPTReconstruction_cff')
        if   alg_size == 'ak4':
            sequence = cms.Sequence(process.ak4JPTJetsSequence * sequence)
#        elif alg_size == 'ak7':
#            sequence = cms.Sequence(process.ak7JPTJetsSequence * sequence)
        else:
            raise ValueError(alg_size + " not supported for JPT!")
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
                         srcRef            = cms.InputTag(refPtEta.label()),
                         jecLabel          = cms.string(''),
                         srcRhos           = cms.InputTag(''),
                         srcRho            = cms.InputTag(''),
                         srcRhoHLT         = cms.InputTag(''),
                         srcVtx            = cms.InputTag('offlinePrimaryVertices'),
						 srcJetToUncorJetMap = cms.InputTag(jetToUncorJet.label(), 'rec2gen'),
                         srcPFCandidates   = cms.InputTag(''),
                         srcGenParticles   = cms.InputTag('genParticles')
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
                     srcPFCandidates   = cms.InputTag(''),
                     srcGenParticles   = cms.InputTag('genParticles')
                     )

    if type == 'CaloHLT':
        jra.srcRho = cms.InputTag("fixedGridRhoFastjetAllCalo")
        jra.srcRhoHLT = cms.InputTag("fixedGridRhoFastjetAllCalo")
    elif type == 'Calo':
        jra.srcRho = cms.InputTag("fixedGridRhoFastjetAllCalo")
    elif type == 'PFchs':
        process.kt6PFchsJetsRhos = kt6PFJets.clone(src = 'pfNoPileUpJME',
                                                   doFastJetNonUniform = cms.bool(True),
                                                   puCenters = cms.vdouble(-5,-4,-3,-2,-1,0,1,2,3,4,5), 
                                                   puWidth = cms.double(.8),
                                                   nExclude = cms.uint32(2))
        sequence = cms.Sequence(process.kt6PFchsJetsRhos * sequence)
        jra.srcRhos = cms.InputTag("kt6PFchsJetsRhos", "rhos")
        jra.srcRho = cms.InputTag("fixedGridRhoFastjetAll")
        jra.srcPFCandidates = cms.InputTag('pfNoPileUpJME')
    elif type == 'PFHLT':
        jra.srcRho = ak4PFL1Fastjet.srcRho #added 02/15/2012
        jra.srcRhoHLT = ak5PFHLTL1Fastjet.srcRho
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
        jra.srcRho = cms.InputTag("fixedGridRhoFastjetAll")
        jra.srcPFCandidates = cms.InputTag('particleFlow')
    elif type == 'PUPPI':
        process.kt6PFJetsRhos = kt6PFJets.clone(doFastJetNonUniform = cms.bool(True),
                                                puCenters = cms.vdouble(-5,-4,-3,-2,-1,0,1,2,3,4,5),
                                                puWidth = cms.double(.8), nExclude = cms.uint32(2))
        sequence = cms.Sequence(process.kt6PFJetsRhos * sequence)
        jra.srcRhos = cms.InputTag("kt6PFJetsRhos", "rhos")
        jra.srcRho = cms.InputTag("fixedGridRhoFastjetAll")
        jra.srcPFCandidates = cms.InputTag('puppi')
    elif type == 'JPT':
        jra.srcRho = cms.InputTag("fixedGridRhoFastjetAllCalo")

    if correctl1 or correctl2l3:
        jra.jecLabel = corrJets.correctors[0].replace("Corrector","")

    if Defaults.JetResponseParameters.doFlavor.value():
		jra.srcRefToPartonMap = cms.InputTag(genToParton.label())

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

import FWCore.ParameterSet.Config as cms

import JetMETAnalysis.JetAnalyzers.Defaults_cff as Defaults


################################################################################
## filter final state partons (define globaly)
################################################################################
partons = cms.EDFilter(
    'PdgIdAndStatusCandViewSelector',
    src    = cms.InputTag('genParticles'),
    status = cms.vint32(3),
    pdgId  = cms.vint32(1, 2, 3, 4, 5, 21)
    )


################################################################################
## jet reconstruction
################################################################################
from JetMETAnalysis.JetAnalyzers.JetReconstruction_cff import *
from JetMETAnalysis.JetAnalyzers.JPTReconstruction_cff import *
from JetMETAnalysis.JetAnalyzers.JetCorrection_cff     import *

stdGenJetsDict = {
    'ak5calo': 'ak5GenJets',
    'ak7calo': 'ak7GenJets',
    'sc5calo': 'sisCone5GenJets',
    'sc7calo': 'sisCone7GenJets',
    'ic5calo': 'iterativeCone5GenJets',
    'kt4calo': 'kt4GenJets',
    'kt6calo': 'kt6GenJets',
    'ak5pf':   'ak5GenJets',
    'ak7pf':   'ak7GenJets',
    'sc5pf':   'sisCone5GenJets',
    'sc7pf':   'sisCone7GenJets',
    'ic5pf':   'iterativeCone5GenJets',
    'kt4pf':   'kt4GenJets',
    'kt6pf':   'kt6GenJets',
    'ak5jpt':  'ak5GenJets',
    'sc5jpt':  'ak5GenJets',
    'ic5jpt':  'ak5GenJets'
    }

genJetsDict = {
    'ak5calo':     ('ak5GenJetsNoMusNoNus', ak5GenJetsNoMusNoNus),
    'ak7calo':     ('ak7GenJetsNoMusNoNus', ak7GenJetsNoMusNoNus),
    'sc5calo':     ('sc5GenJetsNoMusNoNus', sc5GenJetsNoMusNoNus),
    'sc7calo':     ('sc7GenJetsNoMusNoNus', sc7GenJetsNoMusNoNus),
    'ic5calo':     ('ic5GenJetsNoMusNoNus', ic5GenJetsNoMusNoNus),
    'kt4calo':     ('kt4GenJetsNoMusNoNus', kt4GenJetsNoMusNoNus),
    'kt5calo':     ('kt5GenJetsNoMusNoNus', kt5GenJetsNoMusNoNus),
    'kt6calo':     ('kt6GenJetsNoMusNoNus', kt6GenJetsNoMusNoNus),
    'kt7calo':     ('kt7GenJetsNoMusNoNus', kt7GenJetsNoMusNoNus),
    'ca4calo':     ('ca4GenJetsNoMusNoNus', ca4GenJetsNoMusNoNus),
    'ca5calo':     ('ca5GenJetsNoMusNoNus', ca5GenJetsNoMusNoNus),
    'ca6calo':     ('ca6GenJetsNoMusNoNus', ca6GenJetsNoMusNoNus),
    'ca7calo':     ('ca7GenJetsNoMusNoNus', ca7GenJetsNoMusNoNus),
    'ak5pf':       ('ak5GenJetsNoNus',      ak5GenJetsNoNus),
    'ak7pf':       ('ak7GenJetsNoNus',      ak7GenJetsNoNus),
    'sc5pf':       ('sc5GenJetsNoNus',      sc5GenJetsNoNus),
    'sc7pf':       ('sc7GenJetsNoNus',      sc7GenJetsNoNus),
    'ic5pf':       ('ic5GenJetsNoNus',      ic5GenJetsNoNus),
    'kt4pf':       ('kt4GenJetsNoNus',      kt4GenJetsNoNus),
    'kt5pf':       ('kt5GenJetsNoNus',      kt5GenJetsNoNus),
    'kt6pf':       ('kt6GenJetsNoNus',      kt6GenJetsNoNus),
    'kt7pf':       ('kt7GenJetsNoNus',      kt7GenJetsNoNus),
    'ca4pf':       ('ca4GenJetsNoNus',      ca4GenJetsNoNus),
    'ca5pf':       ('ca5GenJetsNoNus',      ca5GenJetsNoNus),
    'ca6pf':       ('ca6GenJetsNoNus',      ca6GenJetsNoNus),
    'ca7pf':       ('ca7GenJetsNoNus',      ca7GenJetsNoNus),
    'ak5trk':      ('ak5GenJetsNoNus',      ak5GenJetsNoNus),
    'ak7trk':      ('ak7GenJetsNoNus',      ak7GenJetsNoNus),
    'sc5trk':      ('sc5GenJetsNoNus',      sc5GenJetsNoNus),
    'sc7trk':      ('sc7GenJetsNoNus',      sc7GenJetsNoNus),
    'ic5trk':      ('ic5GenJetsNoNus',      ic5GenJetsNoNus),
    'kt4trk':      ('kt4GenJetsNoNus',      kt4GenJetsNoNus),
    'kt5trk':      ('kt5GenJetsNoNus',      kt5GenJetsNoNus),
    'kt6trk':      ('kt6GenJetsNoNus',      kt6GenJetsNoNus),
    'kt7trk':      ('kt7GenJetsNoNus',      kt7GenJetsNoNus),
    'ca4trk':      ('ca4GenJetsNoNus',      ca4GenJetsNoNus),
    'ca5trk':      ('ca5GenJetsNoNus',      ca5GenJetsNoNus),
    'ca6trk':      ('ca6GenJetsNoNus',      ca6GenJetsNoNus),
    'ca7trk':      ('ca7GenJetsNoNus',      ca7GenJetsNoNus),
    'ak5jpt':      ('ak5GenJetsNoNus',      ak5GenJetsNoNus),
    'sc5jpt':      ('sc5GenJetsNoNus',      sc5GenJetsNoNus),
    'ic5jpt':      ('ic5GenJetsNoNus',      ic5GenJetsNoNus)
    }

stdRecJetsDict = {
    'ak5calo':     'ak5CaloJets',
    'ak7calo':     'ak7CaloJets',
    'sc5calo':     'sisCone5CaloJets',
    'sc7calo':     'sisCone7CaloJets',
    'ic5calo':     'iterativeCone5CaloJets',
    'kt4calo':     'kt4CaloJets',
    'kt6calo':     'kt6CaloJets',
    'ak5pf':       'ak5PFJets',
    'ak7pf':       'ak7PFJets',
    'sc5pf':       'sisCone5PFJets',
    'sc7pf':       'sisCone7PFJets',
    'ic5pf':       'iterativeCone5PFJets',
    'kt4pf':       'kt4PFJets',
    'kt6pf':       'kt6PFJets',
    'ak5jpt':      'ak5JPTJets',
    'sc5jpt':      'sc5JPTJets',
    'ic5jpt':      'ic5JPTJets'
    }

recJetsDict = {
    'ak5calo':     ('ak5CaloJets',      ak5CaloJets),
    'ak7calo':     ('ak7CaloJets',      ak7CaloJets),
    'sc5calo':     ('sc5CaloJets',      sc5CaloJets),
    'sc7calo':     ('sc7CaloJets',      sc7CaloJets),
    'ic5calo':     ('ic5CaloJets',      ic5CaloJets),
    'kt4calo':     ('kt4CaloJets',      kt4CaloJets),
    'kt5calo':     ('kt5CaloJets',      kt5CaloJets),
    'kt6calo':     ('kt6CaloJets',      kt6CaloJets),
    'kt7calo':     ('kt7CaloJets',      kt7CaloJets),
    'ca4calo':     ('ca4CaloJets',      ca4CaloJets),
    'ca5calo':     ('ca5CaloJets',      ca5CaloJets),
    'ca6calo':     ('ca6CaloJets',      ca6CaloJets),
    'ca7calo':     ('ca7CaloJets',      ca7CaloJets),
    'ak5pf':       ('ak5PFJets',        ak5PFJets),
    'ak7pf':       ('ak7PFJets',        ak7PFJets),
    'sc5pf':       ('sc5PFJets',        sc5PFJets),
    'sc7pf':       ('sc7PFJets',        sc7PFJets),
    'ic5pf':       ('ic5PFJets',        ic5PFJets),
    'kt4pf':       ('kt4PFJets',        kt4PFJets),
    'kt5pf':       ('kt5PFJets',        kt5PFJets),
    'kt6pf':       ('kt6PFJets',        kt6PFJets),
    'kt7pf':       ('kt7PFJets',        kt7PFJets),
    'ca4pf':       ('ca4PFJets',        ca4PFJets),
    'ca5pf':       ('ca5PFJets',        ca5PFJets),
    'ca6pf':       ('ca6PFJets',        ca6PFJets),
    'ca7pf':       ('ca7PFJets',        ca7PFJets),
    'ak5trk':      ('ak5TrackJets',     ak5TrackJets),
    'ak7trk':      ('ak7TrackJets',     ak7TrackJets),
    'sc5trk':      ('sc5TrackJets',     sc5TrackJets),
    'sc7trk':      ('sc7TrackJets',     sc7TrackJets),
    'ic5trk':      ('ic5TrackJets',     ic5TrackJets),
    'kt4trk':      ('kt4TrackJets',     kt4TrackJets),
    'kt5trk':      ('kt5TrackJets',     kt5TrackJets),
    'kt6trk':      ('kt6TrackJets',     kt6TrackJets),
    'kt7trk':      ('kt7TrackJets',     kt7TrackJets),
    'ca4trk':      ('ca4TrackJets',     ca4TrackJets),
    'ca5trk':      ('ca5TrackJets',     ca5TrackJets),
    'ca6trk':      ('ca6TrackJets',     ca6TrackJets),
    'ca7trk':      ('ca7TrackJets',     ca7TrackJets),
    'ak5jpt':      ('ak5JPTJets',       ak5JPTJets),
    'sc5jpt':      ('sc5JPTJets',       sc5JPTJets),
    'ic5jpt':      ('ic5JPTJets',       ic5JPTJets)
    }

corrJetsDict = {
    'ak5calol2l3': ('ak5CaloJetsL2L3',  ak5CaloJetsL2L3),
    'ak7calol2l3': ('ak7CaloJetsL2L3',  ak7CaloJetsL2L3),
    'sc5calol2l3': ('sc5CaloJetsL2L3',  sc5CaloJetsL2L3),
    'sc7calol2l3': ('sc7CaloJetsL2L3',  sc7CaloJetsL2L3),
    'ic5calol2l3': ('ic5CaloJetsL2L3',  ic5CaloJetsL2L3),
    'kt4calol2l3': ('kt4CaloJetsL2L3',  kt4CaloJetsL2L3),
    'kt5calol2l3': ('kt5CaloJetsL2L3',  kt5CaloJetsL2L3),
    'kt6calol2l3': ('kt6CaloJetsL2L3',  kt6CaloJetsL2L3),
    'kt7calol2l3': ('kt7CaloJetsL2L3',  kt7CaloJetsL2L3),
    'ca4calol2l3': ('ca4CaloJetsL2L3',  ca4CaloJetsL2L3),
    'ca5calol2l3': ('ca5CaloJetsL2L3',  ca5CaloJetsL2L3),
    'ca6calol2l3': ('ca6CaloJetsL2L3',  ca6CaloJetsL2L3),
    'ca7calol2l3': ('ca7CaloJetsL2L3',  ca7CaloJetsL2L3),
    'ak5pfl2l3':   ('ak5PFJetsL2L3',    ak5PFJetsL2L3),
    'ak7pfl2l3':   ('ak7PFJetsL2L3',    ak7PFJetsL2L3),
    'sc5pfl2l3':   ('sc5PFJetsL2L3',    sc5PFJetsL2L3),
    'sc7pfl2l3':   ('sc7PFJetsL2L3',    sc7PFJetsL2L3),
    'ic5pfl2l3':   ('ic5PFJetsL2L3',    ic5PFJetsL2L3),
    'kt4pfl2l3':   ('kt4PFJetsL2L3',    kt4PFJetsL2L3),
    'kt5pfl2l3':   ('kt5PFJetsL2L3',    kt5PFJetsL2L3),
    'kt6pfl2l3':   ('kt6PFJetsL2L3',    kt6PFJetsL2L3),
    'kt7pfl2l3':   ('kt7PFJetsL2L3',    kt7PFJetsL2L3),
    'ca4pfl2l3':   ('ca4PFJetsL2L3',    ca4PFJetsL2L3),
    'ca5pfl2l3':   ('ca5PFJetsL2L3',    ca5PFJetsL2L3),
    'ca6pfl2l3':   ('ca6PFJetsL2L3',    ca6PFJetsL2L3),
    'ca7pfl2l3':   ('ca7PFJetsL2L3',    ca7PFJetsL2L3),
    }



################################################################################
## addAlgorithm
################################################################################
def addAlgorithm(process,alg_size_type_corr,reco):
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
    elif (alg_size_type_corr.find('jpt') > 0) :
        alg_size      = alg_size_type_corr[0:alg_size_type_corr.find('jpt')]
        type          = 'JPT'
        alg_size_type = alg_size + 'jpt'
    elif (alg_size_type_corr.find('trk') > 0) :
        alg_size      = alg_size_type_corr[0:alg_size_type_corr.find('trk')]
        type          = 'Track'
        alg_size_type = alg_size + 'trk'
    else:
        raise ValueError("Can't identify valid jet type: calo|pf|jpt|trk")

        
    if (alg_size_type_corr.find('l1') > 0):
        raise ValueError('PU subtraction not yet (!) supported, omit l1!')
        correctl1   = True
        
    if (alg_size_type_corr.find('l2l3') > 0):
        correctl2l3 = True


    ## check that alg_size_type_corr refers to valid jet configuration
    try:
        not reco and stdGenJetsDict.keys().index(alg_size_type)
        not reco and stdRecJetsDict.keys().index(alg_size_type)
    except ValueError:
        raise ValueError("Algorithm unavailable in standard format: "+alg_size_type)
    
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
    refPtEta = cms.EDFilter(
        'EtaPtMinCandViewRefSelector',
        Defaults.RefPtEta,
        src = cms.InputTag(genJetsDict[alg_size_type][0])
        )
    if not reco:
        refPtEta.src = stdGenJetsDict[alg_size_type]
    setattr(process, alg_size_type + 'GenPtEta', refPtEta)
    
    ## reco jet kinematic selection
    jetPtEta = cms.EDFilter(
        'EtaPtMinCandViewRefSelector',
        Defaults.JetPtEta,
        src = cms.InputTag(recJetsDict[alg_size_type][0])
        )
    if not reco:
        jetPtEta.src = stdRecJetsDict[alg_size_type]
    setattr(process, alg_size_type_corr + 'PtEta', jetPtEta)
    
    ## create the sequence
    sequence = refPtEta * jetPtEta
    
    ## correct jets
    corrLabel = ''
    if correctl2l3:
        process.load('JetMETAnalysis.JetAnalyzers.JetCorrection_cff')
        (corrLabel,corrJets) = corrJetsDict[alg_size_type_corr]
        setattr(process,corrLabel,corrJets)
        sequence = corrJets * sequence
        
    ## reconstruct jets
    if type == 'JPT':
        process.load('Configuration.StandardSequences.Geometry_cff')
        process.load('Configuration.StandardSequences.MagneticField_cff')
        process.load('JetMETAnalysis.JetAnalyzers.JPTReconstruction_cff')
        if   alg_size == 'ak5':
            sequence = process.ak5JPTJetsSequence * sequence
        elif alg_size == 'sc5':
            sequence = process.sc5JPTJetsSequence * sequence
        elif alg_size == 'ic5':
            sequence = process.ic5JPTJetsSequence * sequence
        else:
            raise ValueError(alg_size + " not supported for JPT!")
    elif reco:
        (recLabel,recJets) = recJetsDict[alg_size_type]
        if correctl2l3:
            corrJets.src = recLabel
            jetPtEta.src = corrLabel
        else:
            jetPtEta.src = recLabel
        setattr(process,recLabel,recJets)
        sequence = recJets * sequence
        if type == 'PF':
            process.load('JetMETAnalysis.JetAnalyzers.PFParticlesForJets_cff')
            recJets.src = 'pfNoElectron'
            sequence = process.pfParticlesForJets * sequence
        elif type =='Track':
            setattr(process,'tracksForJets',tracksForJets)
            sequence = tracksForJets * sequence
                
    # reconstruct genjets
    if reco:
        (genLabel,genJets) = genJetsDict[alg_size_type]
        setattr(process,genLabel,genJets)
        sequence = genJets * sequence
        if type == 'Calo':
            setattr(process,'genParticlesForJetsNoMusNoNus',
                    genParticlesForJetsNoMusNoNus)
            sequence = genParticlesForJetsNoMusNoNus * sequence
        else:
            setattr(process,'genParticlesForJetsNoNus',genParticlesForJetsNoNus)
            sequence = genParticlesForJetsNoNus * sequence
        refPtEta.src = genJets.label()
        
    ## filter / map partons only if flavor information is requested
    if Defaults.JetResponseParameters.doFlavor.value() :
        setattr(process,'partons',partons)
        genToParton = cms.EDFilter(
            'MatchRecToGen',
            srcRec = cms.InputTag(refPtEta.label()),
            srcGen = cms.InputTag(partons.label())
            )
        setattr(process,alg_size_type + 'GenToParton', genToParton)
        sequence = sequence * partons * genToParton
        
    ## reference to jet matching
    jetToRef = cms.EDFilter(
        'MatchRecToGen',
        srcGen = cms.InputTag(refPtEta.label()),
        srcRec = cms.InputTag(jetPtEta.label())
        )
    setattr(process,alg_size_type_corr + 'JetToRef', jetToRef)
    sequence = sequence * jetToRef
    
    ## jet response analyzer
    jra = cms.EDAnalyzer(
        'JetResponseAnalyzer',
        Defaults.JetResponseParameters,
        srcRefToJetMap    = cms.InputTag(jetToRef.label(),'gen2rec'),
        srcRef            = cms.InputTag(refPtEta.label()),
        )
    if Defaults.JetResponseParameters.doFlavor.value():
        jra.srcRefToPartonMap = cms.InputTag(genToParton.label(),'rec2gen')
    setattr(process,alg_size_type_corr,jra)
    sequence = sequence * jra
    
    ## create the path and put in the sequence
    sequence = cms.Sequence(sequence)
    setattr(process,alg_size_type_corr + 'Sequence',sequence)
    path = cms.Path( sequence )
    setattr(process,alg_size_type_corr + 'Path',path)

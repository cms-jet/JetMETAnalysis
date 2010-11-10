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
#    'sc5calo': 'sisCone5GenJets',
#    'sc7calo': 'sisCone7GenJets',
    'ic5calo': 'iterativeCone5GenJets',
    'kt4calo': 'kt4GenJets',
    'kt6calo': 'kt6GenJets',
    'ak5pf':   'ak5GenJets',
    'ak7pf':   'ak7GenJets',
#    'sc5pf':   'sisCone5GenJets',
#    'sc7pf':   'sisCone7GenJets',
    'ic5pf':   'iterativeCone5GenJets',
    'kt4pf':   'kt4GenJets',
    'kt6pf':   'kt6GenJets',
    'ak5trk': 'ak5GenJets',
    'ak7trk': 'ak7GenJets',
#    'sc5trk': 'sisCone5GenJets',
#    'sc7trk': 'sisCone7GenJets',
    'ic5trk': 'iterativeCone5GenJets',
    'kt4trk': 'kt4GenJets',
    'kt6trk': 'kt6GenJets',
    'ak5jpt':  'ak5GenJets',
    'ic5jpt':  'ak5GenJets'
    }

genJetsDict = {
    'ak5calo':     ('ak5GenJetsNoMuNoNu', ak5GenJetsNoMuNoNu),
    'ak7calo':     ('ak7GenJetsNoMuNoNu', ak7GenJetsNoMuNoNu),
#    'sc5calo':     ('sc5GenJetsNoMuNoNu', sc5GenJetsNoMuNoNu),
#    'sc7calo':     ('sc7GenJetsNoMuNoNu', sc7GenJetsNoMuNoNu),
    'ic5calo':     ('ic5GenJetsNoMuNoNu', ic5GenJetsNoMuNoNu),
    'kt4calo':     ('kt4GenJetsNoMuNoNu', kt4GenJetsNoMuNoNu),
    'kt5calo':     ('kt5GenJetsNoMuNoNu', kt5GenJetsNoMuNoNu),
    'kt6calo':     ('kt6GenJetsNoMuNoNu', kt6GenJetsNoMuNoNu),
    'kt7calo':     ('kt7GenJetsNoMuNoNu', kt7GenJetsNoMuNoNu),
    'ca4calo':     ('ca4GenJetsNoMuNoNu', ca4GenJetsNoMuNoNu),
    'ca5calo':     ('ca5GenJetsNoMuNoNu', ca5GenJetsNoMuNoNu),
    'ca6calo':     ('ca6GenJetsNoMuNoNu', ca6GenJetsNoMuNoNu),
    'ca7calo':     ('ca7GenJetsNoMuNoNu', ca7GenJetsNoMuNoNu),
    'ak5pf':       ('ak5GenJetsNoNu',     ak5GenJetsNoNu),
    'ak7pf':       ('ak7GenJetsNoNu',     ak7GenJetsNoNu),
#    'sc5pf':       ('sc5GenJetsNoNu',     sc5GenJetsNoNu),
#    'sc7pf':       ('sc7GenJetsNoNu',     sc7GenJetsNoNu),
    'ic5pf':       ('ic5GenJetsNoNu',     ic5GenJetsNoNu),
    'kt4pf':       ('kt4GenJetsNoNu',     kt4GenJetsNoNu),
    'kt5pf':       ('kt5GenJetsNoNu',     kt5GenJetsNoNu),
    'kt6pf':       ('kt6GenJetsNoNu',     kt6GenJetsNoNu),
    'kt7pf':       ('kt7GenJetsNoNu',     kt7GenJetsNoNu),
    'ca4pf':       ('ca4GenJetsNoNu',     ca4GenJetsNoNu),
    'ca5pf':       ('ca5GenJetsNoNu',     ca5GenJetsNoNu),
    'ca6pf':       ('ca6GenJetsNoNu',     ca6GenJetsNoNu),
    'ca7pf':       ('ca7GenJetsNoNu',     ca7GenJetsNoNu),
    'ak5trk':      ('ak5GenJetsNoNu',     ak5GenJetsNoNu),
    'ak7trk':      ('ak7GenJetsNoNu',     ak7GenJetsNoNu),
#    'sc5trk':      ('sc5GenJetsNoNu',     sc5GenJetsNoNu),
#    'sc7trk':      ('sc7GenJetsNoNu',     sc7GenJetsNoNu),
    'ic5trk':      ('ic5GenJetsNoNu',     ic5GenJetsNoNu),
    'kt4trk':      ('kt4GenJetsNoNu',     kt4GenJetsNoNu),
    'kt5trk':      ('kt5GenJetsNoNu',     kt5GenJetsNoNu),
    'kt6trk':      ('kt6GenJetsNoNu',     kt6GenJetsNoNu),
    'kt7trk':      ('kt7GenJetsNoNu',     kt7GenJetsNoNu),
    'ca4trk':      ('ca4GenJetsNoNu',     ca4GenJetsNoNu),
    'ca5trk':      ('ca5GenJetsNoNu',     ca5GenJetsNoNu),
    'ca6trk':      ('ca6GenJetsNoNu',     ca6GenJetsNoNu),
    'ca7trk':      ('ca7GenJetsNoNu',     ca7GenJetsNoNu),
    'ak5jpt':      ('ak5GenJetsNoNu',     ak5GenJetsNoNu),
    'ic5jpt':      ('ic5GenJetsNoNu',     ic5GenJetsNoNu)
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
 #   'sc5pf':       'sisCone5PFJets',
 #   'sc7pf':       'sisCone7PFJets',
    'ic5pf':       'iterativeCone5PFJets',
    'kt4pf':       'kt4PFJets',
    'kt6pf':       'kt6PFJets',
    'ak5trk':     'ak5TrackJets',
#    'ak7trk':     'ak7TrackJets',
#    'sc5trk':     'sisCone5TrackJets',
#    'sc7trk':     'sisCone7TrackJets',
#    'ic5trk':     'iterativeCone5TrackJets',
    'kt4trk':     'kt4TrackJets',
#    'kt6trk':     'kt6TrackJets',
    'ak5jpt':      'ak5JPTJets',
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
#    'sc5pf':       ('sc5PFJets',        sc5PFJets),
#    'sc7pf':       ('sc7PFJets',        sc7PFJets),
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
#    'ak7trk':      ('ak7TrackJets',     ak7TrackJets),
#    'sc5trk':      ('sisCone5TrackJets',     sisCone5TrackJets),
#    'sc7trk':      ('sc7TrackJets',     sc7TrackJets),
#    'ic5trk':      ('ic5TrackJets',     ic5TrackJets),
    'kt4trk':      ('kt4TrackJets',     kt4TrackJets),
#    'kt5trk':      ('kt5TrackJets',     kt5TrackJets),
#    'kt6trk':      ('kt6TrackJets',     kt6TrackJets),
#    'kt7trk':      ('kt7TrackJets',     kt7TrackJets),
#    'ca4trk':      ('ca4TrackJets',     ca4TrackJets),
#    'ca5trk':      ('ca5TrackJets',     ca5TrackJets),
#    'ca6trk':      ('ca6TrackJets',     ca6TrackJets),
#    'ca7trk':      ('ca7TrackJets',     ca7TrackJets),
    'ak5jpt':      ('ak5JPTJets',       ak5JPTJets),
    'ic5jpt':      ('ic5JPTJets',       ic5JPTJets)
    }

corrJetsDict = {
    'ak5calol1':     ('ak5CaloJetsL1',    ak5CaloJetsL1),
    'ak7calol1':     ('ak7CaloJetsL1',    ak7CaloJetsL1),
#    'sc5calol1':     ('sc5CaloJetsL1',    sc5CaloJetsL1),
#    'sc7calol1':     ('sc7CaloJetsL1',    sc7CaloJetsL1),
    'ic5calol1':     ('ic5CaloJetsL1',    ic5CaloJetsL1),
    'kt4calol1':     ('kt4CaloJetsL1',    kt4CaloJetsL1),
    'kt5calol1':     ('kt5CaloJetsL1',    kt5CaloJetsL1),
    'kt6calol1':     ('kt6CaloJetsL1',    kt6CaloJetsL1),
    'kt7calol1':     ('kt7CaloJetsL1',    kt7CaloJetsL1),
    'ca4calol1':     ('ca4CaloJetsL1',    ca4CaloJetsL1),
    'ca5calol1':     ('ca5CaloJetsL1',    ca5CaloJetsL1),
    'ca6calol1':     ('ca6CaloJetsL1',    ca6CaloJetsL1),
    'ca7calol1':     ('ca7CaloJetsL1',    ca7CaloJetsL1),
    'ak5jptl1':      ('ak5JPTJetsL1',     ak5JPTJetsL1), # *
    'ak5pfl1':       ('ak5PFJetsL1',      ak5PFJetsL1),
    'ak7pfl1':       ('ak7PFJetsL1',      ak7PFJetsL1),
#    'sc5pfl1':       ('sc5PFJetsL1',      sc5PFJetsL1),
#    'sc7pfl1':       ('sc7PFJetsL1',      sc7PFJetsL1),
    'ic5pfl1':       ('ic5PFJetsL1',      ic5PFJetsL1),
    'kt4pfl1':       ('kt4PFJetsL1',      kt4PFJetsL1),
    'kt5pfl1':       ('kt5PFJetsL1',      kt5PFJetsL1),
    'kt6pfl1':       ('kt6PFJetsL1',      kt6PFJetsL1),
    'kt7pfl1':       ('kt7PFJetsL1',      kt7PFJetsL1),
    'ca4pfl1':       ('ca4PFJetsL1',      ca4PFJetsL1),
    'ca5pfl1':       ('ca5PFJetsL1',      ca5PFJetsL1),
    'ca6pfl1':       ('ca6PFJetsL1',      ca6PFJetsL1),
    'ca7pfl1':       ('ca7PFJetsL1',      ca7PFJetsL1),
    'ak5calol2l3':   ('ak5CaloJetsL2L3',  ak5CaloJetsL2L3),
    'ak7calol2l3':   ('ak7CaloJetsL2L3',  ak7CaloJetsL2L3),
#    'sc5calol2l3':   ('sc5CaloJetsL2L3',  sc5CaloJetsL2L3),
#    'sc7calol2l3':   ('sc7CaloJetsL2L3',  sc7CaloJetsL2L3),
    'ic5calol2l3':   ('ic5CaloJetsL2L3',  ic5CaloJetsL2L3),
    'kt4calol2l3':   ('kt4CaloJetsL2L3',  kt4CaloJetsL2L3),
    'kt5calol2l3':   ('kt5CaloJetsL2L3',  kt5CaloJetsL2L3),
    'kt6calol2l3':   ('kt6CaloJetsL2L3',  kt6CaloJetsL2L3),
    'kt7calol2l3':   ('kt7CaloJetsL2L3',  kt7CaloJetsL2L3),
    'ca4calol2l3':   ('ca4CaloJetsL2L3',  ca4CaloJetsL2L3),
    'ca5calol2l3':   ('ca5CaloJetsL2L3',  ca5CaloJetsL2L3),
    'ca6calol2l3':   ('ca6CaloJetsL2L3',  ca6CaloJetsL2L3),
    'ca7calol2l3':   ('ca7CaloJetsL2L3',  ca7CaloJetsL2L3),
    'ak5jptl2l3':    ('ak5JPTJetsL2L3',   ak5JPTJetsL2L3),
    'ic5jptl2l3':    ('ic5JPTJetsL2L3',   ic5JPTJetsL2L3),
    'ak5pfl2l3':     ('ak5PFJetsL2L3',    ak5PFJetsL2L3),
    'ak7pfl2l3':     ('ak7PFJetsL2L3',    ak7PFJetsL2L3),
#    'sc5pfl2l3':     ('sc5PFJetsL2L3',    sc5PFJetsL2L3),
#    'sc7pfl2l3':     ('sc7PFJetsL2L3',    sc7PFJetsL2L3),
    'ic5pfl2l3':     ('ic5PFJetsL2L3',    ic5PFJetsL2L3),
    'kt4pfl2l3':     ('kt4PFJetsL2L3',    kt4PFJetsL2L3),
    'kt5pfl2l3':     ('kt5PFJetsL2L3',    kt5PFJetsL2L3),
    'kt6pfl2l3':     ('kt6PFJetsL2L3',    kt6PFJetsL2L3),
    'kt7pfl2l3':     ('kt7PFJetsL2L3',    kt7PFJetsL2L3),
    'ca4pfl2l3':     ('ca4PFJetsL2L3',    ca4PFJetsL2L3),
    'ca5pfl2l3':     ('ca5PFJetsL2L3',    ca5PFJetsL2L3),
    'ca6pfl2l3':     ('ca6PFJetsL2L3',    ca6PFJetsL2L3),
    'ca7pfl2l3':     ('ca7PFJetsL2L3',    ca7PFJetsL2L3),
#    'ak5trkl2l3':   ('ak5TrackJetsL2L3',  ak5TrackJetsL2L3),
#    'ak7trkl2l3':   ('ak7TrackJetsL2L3',  ak7TrackJetsL2L3),
#    'sc5trkl2l3':   ('sc5TrackJetsL2L3',  sc5TrackJetsL2L3),
#    'sc7trkl2l3':   ('sc7TrackJetsL2L3',  sc7TrackJetsL2L3),
#    'ic5trkl2l3':   ('ic5TrackJetsL2L3',  ic5TrackJetsL2L3),
#    'kt4trkl2l3':   ('kt4TrackJetsL2L3',  kt4TrackJetsL2L3),
#    'kt5trkl2l3':   ('kt5TrackJetsL2L3',  kt5TrackJetsL2L3),
#    'kt6trkl2l3':   ('kt6TrackJetsL2L3',  kt6TrackJetsL2L3),
#    'kt7trkl2l3':   ('kt7TrackJetsL2L3',  kt7TrackJetsL2L3),
#    'ca4trkl2l3':   ('ca4TrackJetsL2L3',  ca4TrackJetsL2L3),
#    'ca5trkl2l3':   ('ca5TrackJetsL2L3',  ca5TrackJetsL2L3),
#    'ca6trkl2l3':   ('ca6TrackJetsL2L3',  ca6TrackJetsL2L3),
#    'ca7trkl2l3':   ('ca7TrackJetsL2L3',  ca7TrackJetsL2L3),
    'ak5calol1l2l3': ('ak5CaloJetsL1L2L3',ak5CaloJetsL1L2L3),
    'ak7calol1l2l3': ('ak7CaloJetsL1L2L3',ak7CaloJetsL1L2L3),
#    'sc5calol1l2l3': ('sc5CaloJetsL1L2L3',sc5CaloJetsL1L2L3),
#    'sc7calol1l2l3': ('sc7CaloJetsL1L2L3',sc7CaloJetsL1L2L3),
    'ic5calol1l2l3': ('ic5CaloJetsL1L2L3',ic5CaloJetsL1L2L3),
    'kt4calol1l2l3': ('kt4CaloJetsL1L2L3',kt4CaloJetsL1L2L3),
    'kt5calol1l2l3': ('kt5CaloJetsL1L2L3',kt5CaloJetsL1L2L3),
    'kt6calol1l2l3': ('kt6CaloJetsL1L2L3',kt6CaloJetsL1L2L3),
    'kt7calol1l2l3': ('kt7CaloJetsL1L2L3',kt7CaloJetsL1L2L3),
    'ca4calol1l2l3': ('ca4CaloJetsL1L2L3',ca4CaloJetsL1L2L3),
    'ca5calol1l2l3': ('ca5CaloJetsL1L2L3',ca5CaloJetsL1L2L3),
    'ca6calol1l2l3': ('ca6CaloJetsL1L2L3',ca6CaloJetsL1L2L3),
    'ca7calol1l2l3': ('ca7CaloJetsL1L2L3',ca7CaloJetsL1L2L3),
    'ak5jptl1l2l3':   ('ak5JPTJetsL1L2L3',ak5JPTJetsL1L2L3), #*
    'ak5pfl1l2l3':   ('ak5PFJetsL1L2L3',  ak5PFJetsL1L2L3),
    'ak7pfl1l2l3':   ('ak7PFJetsL1L2L3',  ak7PFJetsL1L2L3),
#    'sc5pfl1l2l3':   ('sc5PFJetsL1L2L3',  sc5PFJetsL1L2L3),
#    'sc7pfl1l2l3':   ('sc7PFJetsL1L2L3',  sc7PFJetsL1L2L3),
    'ic5pfl1l2l3':   ('ic5PFJetsL1L2L3',  ic5PFJetsL1L2L3),
    'kt4pfl1l2l3':   ('kt4PFJetsL1L2L3',  kt4PFJetsL1L2L3),
    'kt5pfl1l2l3':   ('kt5PFJetsL1L2L3',  kt5PFJetsL1L2L3),
    'kt6pfl1l2l3':   ('kt6PFJetsL1L2L3',  kt6PFJetsL1L2L3),
    'kt7pfl1l2l3':   ('kt7PFJetsL1L2L3',  kt7PFJetsL1L2L3),
    'ca4pfl1l2l3':   ('ca4PFJetsL1L2L3',  ca4PFJetsL1L2L3),
    'ca5pfl1l2l3':   ('ca5PFJetsL1L2L3',  ca5PFJetsL1L2L3),
    'ca6pfl1l2l3':   ('ca6PFJetsL1L2L3',  ca6PFJetsL1L2L3),
    'ca7pfl1l2l3':   ('ca7PFJetsL1L2L3',  ca7PFJetsL1L2L3),
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
        #raise ValueError("TrackJets currently not supported, back soon!") # TEMP!
        alg_size      = alg_size_type_corr[0:alg_size_type_corr.find('trk')]
        type          = 'Track'
        alg_size_type = alg_size + 'trk'
    else:
        raise ValueError("Can't identify valid jet type: calo|pf|jpt|trk")

        
    if (alg_size_type_corr.find('l1') > 0):
        correctl1   = True
        if not reco:
            raise ValueError("Can't subtract PU without reco!")
        
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
    if correctl1 or correctl2l3:
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
        elif alg_size == 'ic5':
            sequence = process.ic5JPTJetsSequence * sequence
        else:
            raise ValueError(alg_size + " not supported for JPT!")
        if correctl1 or correctl2l3:
            jetPtEta.src = corrLabel
    elif reco:
        process.load('Configuration.StandardSequences.Geometry_cff')
        process.load('Configuration.StandardSequences.MagneticField_cff')
        (recLabel,recJets) = recJetsDict[alg_size_type]
        if correctl1 or correctl2l3:
            corrJets.src = recLabel
            jetPtEta.src = corrLabel
        else:
            jetPtEta.src = recLabel
        setattr(process,recLabel,recJets)
        sequence = recJets * sequence
        #if type == 'PF':
        #    process.load('JetMETAnalysis.JetAnalyzers.PFParticlesForJets_cff')
        #    recJets.src = 'pfNoElectron'
        #    sequence = process.pfParticlesForJets * sequence
        #elif type =='Track':
        if type =='Track':
            process.load('JetMETAnalysis.JetAnalyzers.TrackJetReconstruction_cff')
            sequence = trackJetSequence * sequence

    # reconstruct genjets
    if reco:
        (genLabel,genJets) = genJetsDict[alg_size_type]
        setattr(process,genLabel,genJets)
        sequence = genJets * sequence
        if type == 'Calo':
            setattr(process,'genParticlesForJetsNoMuNoNu',
                    genParticlesForJetsNoMuNoNu)
            sequence = genParticlesForJetsNoMuNoNu * sequence
        else:
            setattr(process,'genParticlesForJetsNoNu',genParticlesForJetsNoNu)
            sequence = genParticlesForJetsNoNu * sequence
        refPtEta.src = genJets.label()
        
    ## filter / map partons only if flavor information is requested
    if Defaults.JetResponseParameters.doFlavor.value() :
        setattr(process,'partons',partons)
        genToParton = cms.EDProducer(
            'MatchRecToGen',
            srcRec = cms.InputTag(refPtEta.label()),
            srcGen = cms.InputTag(partons.label())
            )
        setattr(process,alg_size_type + 'GenToParton', genToParton)
        sequence = sequence * partons * genToParton
        
    ## reference to jet matching
    jetToRef = cms.EDProducer(
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

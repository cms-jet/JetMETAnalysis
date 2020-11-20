import FWCore.ParameterSet.Config as cms

# event setup
from Configuration.Geometry.GeometryIdeal_cff import *
from Configuration.StandardSequences.MagneticField_cff import *

# jet reconstruction
from RecoJets.Configuration.GenJetParticles_cff import *
from RecoJets.Configuration.RecoGenJets_cff import *
from RecoJets.Configuration.RecoJets_cff import *
from RecoJets.Configuration.RecoPFJets_cff import *
from JetMETAnalysis.JetAnalyzers.TrackJetReconstruction_cff import *
#from JetMETAnalysis.JetAnalyzers.ak5CaloHLTJets_cff import *
#from JetMETAnalysis.JetAnalyzers.ak5PFHLTJets_cff import *

#########################
# ANTI-KT JET PRODUCERS #
#########################

#!
#! GEN JET PRODUCERS
#!
ak1GenJets      = ak4GenJets.clone  ( rParam=0.1 )
ak2GenJets      = ak4GenJets.clone  ( rParam=0.2 )
ak3GenJets      = ak4GenJets.clone  ( rParam=0.3 )
ak5GenJets      = ak4GenJets.clone  ( rParam=0.5 )
ak6GenJets      = ak4GenJets.clone  ( rParam=0.6 )
ak7GenJets      = ak4GenJets.clone  ( rParam=0.7 )
ak9GenJets      = ak4GenJets.clone  ( rParam=0.9 )
ak10GenJets     = ak4GenJets.clone  ( rParam=1.0 )

#!
#! GEN JETS WITHOUT NEUTRINOS
#!
ak1GenJetsNoNu  = ak1GenJets.clone ( src = 'genParticlesForJetsNoNu' )
ak2GenJetsNoNu  = ak2GenJets.clone ( src = 'genParticlesForJetsNoNu' )
ak3GenJetsNoNu  = ak3GenJets.clone ( src = 'genParticlesForJetsNoNu' )
ak5GenJetsNoNu  = ak5GenJets.clone ( src = 'genParticlesForJetsNoNu' )
ak6GenJetsNoNu  = ak6GenJets.clone ( src = 'genParticlesForJetsNoNu' )
ak7GenJetsNoNu  = ak7GenJets.clone ( src = 'genParticlesForJetsNoNu' )
ak9GenJetsNoNu  = ak9GenJets.clone ( src = 'genParticlesForJetsNoNu' )
ak10GenJetsNoNu = ak10GenJets.clone( src = 'genParticlesForJetsNoNu' )

#!
#! GENJETS WITHOUT MUONS & NEUTRINOS
#!
ak1GenJetsNoMuNoNu  = ak1GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
ak2GenJetsNoMuNoNu  = ak2GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
ak3GenJetsNoMuNoNu  = ak3GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
ak6GenJetsNoMuNoNu  = ak6GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
ak7GenJetsNoMuNoNu  = ak7GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
ak9GenJetsNoMuNoNu  = ak9GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
ak10GenJetsNoMuNoNu = ak10GenJets.clone( src = 'genParticlesForJetsNoMuNoNu' )

#!
#! HLT JETS
#!
ak5CaloHLTJets   = ak4CaloJets.clone( rParam=0.5, src = 'hltAntiKT5CaloJets' )
ak5PFHLTJets     = ak4PFJets.clone( rParam=0.5, src = 'hltAntiKT5PFJets' )
ak5PFCHSHLTJets  = ak4PFJets.clone( rParam=0.5, src = 'hltAntiKT5PFJetsNoPUPixelVert' )

#!
#! CALO JETS
#!
ak1CaloJets     = ak4CaloJets.clone ( rParam=0.1 )
ak2CaloJets     = ak4CaloJets.clone ( rParam=0.2 )
ak3CaloJets     = ak4CaloJets.clone ( rParam=0.3 )
ak5CaloJets     = ak4CaloJets.clone ( rParam=0.5 )
ak6CaloJets     = ak4CaloJets.clone ( rParam=0.6 )
ak7CaloJets     = ak4CaloJets.clone ( rParam=0.7 )
ak8CaloJets     = ak4CaloJets.clone ( rParam=0.8 )
ak9CaloJets     = ak4CaloJets.clone ( rParam=0.9 )
ak10CaloJets    = ak4CaloJets.clone ( rParam=1.0 )

#!
#! PF JETS
#!
ak1PFJets       = ak4PFJets.clone   ( rParam=0.1 )
ak2PFJets       = ak4PFJets.clone   ( rParam=0.2 )
ak3PFJets       = ak4PFJets.clone   ( rParam=0.3 )
ak5PFJets       = ak4PFJets.clone   ( rParam=0.5 )
ak6PFJets       = ak4PFJets.clone   ( rParam=0.6 )
ak7PFJets       = ak4PFJets.clone   ( rParam=0.7 )
ak9PFJets       = ak4PFJets.clone   ( rParam=0.9 )
ak10PFJets      = ak4PFJets.clone   ( rParam=1.0 )

#!
#! PF JETS CHS
#!
ak1PFCHSJets    = ak1PFJets.clone( src = 'pfCHS' )
ak2PFCHSJets    = ak1PFCHSJets.clone( rParam=0.2 )
ak3PFCHSJets    = ak1PFCHSJets.clone( rParam=0.3 )
ak4PFCHSJets    = ak1PFCHSJets.clone( rParam=0.4 )
ak5PFCHSJets    = ak1PFCHSJets.clone( rParam=0.5 )
ak6PFCHSJets    = ak1PFCHSJets.clone( rParam=0.6 )
ak7PFCHSJets    = ak1PFCHSJets.clone( rParam=0.7 )
ak8PFCHSJets    = ak1PFCHSJets.clone( rParam=0.8 )
ak9PFCHSJets    = ak1PFCHSJets.clone( rParam=0.9 )
ak10PFCHSJets   = ak1PFCHSJets.clone( rParam=1.0 )

#!
#! PF JETS PUPPI
#!
ak1PUPPIJets  = ak4PFJets.clone( rParam=0.1, src = cms.InputTag('puppi') )
ak2PUPPIJets  = ak4PFJets.clone( rParam=0.2, src = cms.InputTag('puppi') )
ak3PUPPIJets  = ak4PFJets.clone( rParam=0.3, src = cms.InputTag('puppi') )
ak4PUPPIJets  = ak4PFJets.clone( rParam=0.4, src = cms.InputTag('puppi') )
ak5PUPPIJets  = ak4PFJets.clone( rParam=0.5, src = cms.InputTag('puppi') )
ak6PUPPIJets  = ak4PFJets.clone( rParam=0.6, src = cms.InputTag('puppi') )
ak7PUPPIJets  = ak4PFJets.clone( rParam=0.7, src = cms.InputTag('puppi') )
ak8PUPPIJets  = ak4PFJets.clone( rParam=0.8, src = cms.InputTag('puppi') )
ak9PUPPIJets  = ak4PFJets.clone( rParam=0.9, src = cms.InputTag('puppi') )
ak10PUPPIJets = ak4PFJets.clone( rParam=1.0, src = cms.InputTag('puppi') )

#!
#! HLT JETS
#!
ak5CaloHLTJets   = ak5CaloJets.clone( src = 'hltAntiKT5CaloJets' )
ak5PFHLTJets     = ak5PFJets.clone( src = 'hltAntiKT5PFJets' )
ak5PFCHSHLTJets  = ak5PFJets.clone( src = 'hltAntiKT5PFJetsNoPUPixelVert' )

'''
####################
# KT JET PRODUCERS #
####################

#!
#! GEN JET PRODUCERS
#!
kt1GenJets    = kt4GenJets.clone( rParam=0.1 )
kt2GenJets    = kt4GenJets.clone( rParam=0.2 )
kt3GenJets    = kt4GenJets.clone( rParam=0.3 )
kt5GenJets    = kt4GenJets.clone( rParam=0.5 )
kt7GenJets    = kt4GenJets.clone( rParam=0.7 )
kt8GenJets    = kt4GenJets.clone( rParam=0.8 )
kt9GenJets    = kt4GenJets.clone( rParam=0.9 )
kt10GenJets   = kt4GenJets.clone( rParam=1.0 )

#!
#! GEN JETS WITHOUT NEUTRINOS
#!
kt1GenJetsNoNu  = kt1GenJets.clone ( src = 'genParticlesForJetsNoNu' )
kt2GenJetsNoNu  = kt2GenJets.clone ( src = 'genParticlesForJetsNoNu' )
kt3GenJetsNoNu  = kt3GenJets.clone ( src = 'genParticlesForJetsNoNu' )
kt5GenJetsNoNu  = kt5GenJets.clone ( src = 'genParticlesForJetsNoNu' )
kt7GenJetsNoNu  = kt7GenJets.clone ( src = 'genParticlesForJetsNoNu' )
kt8GenJetsNoNu  = kt8GenJets.clone ( src = 'genParticlesForJetsNoNu' )
kt9GenJetsNoNu  = kt9GenJets.clone ( src = 'genParticlesForJetsNoNu' )
kt10GenJetsNoNu = kt10GenJets.clone( src = 'genParticlesForJetsNoNu' )

#!
#! GENJETS WITHOUT MUONS & NEUTRINOS
#!
kt1GenJetsNoMuNoNu  = kt1GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
kt2GenJetsNoMuNoNu  = kt2GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
kt3GenJetsNoMuNoNu  = kt3GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
kt5GenJetsNoMuNoNu  = kt5GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
kt7GenJetsNoMuNoNu  = kt7GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
kt8GenJetsNoMuNoNu  = kt8GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
kt9GenJetsNoMuNoNu  = kt9GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
kt10GenJetsNoMuNoNu = kt10GenJets.clone( src = 'genParticlesForJetsNoMuNoNu' )

#!
#! CALO JETS
#!
kt1CaloJets   = kt4CaloJets.clone( rParam=0.1 )
kt2CaloJets   = kt4CaloJets.clone( rParam=0.2 )
kt3CaloJets   = kt4CaloJets.clone( rParam=0.3 )
kt5CaloJets   = kt4CaloJets.clone( rParam=0.5 )
kt7CaloJets   = kt4CaloJets.clone( rParam=0.7 )
kt8CaloJets   = kt4CaloJets.clone( rParam=0.8 )
kt9CaloJets   = kt4CaloJets.clone( rParam=0.9 )
kt10CaloJets  = kt4CaloJets.clone( rParam=1.0 )

#!
#! PF JETS
#!
kt1PFJets     = kt4PFJets.clone( rParam=0.1 )
kt2PFJets     = kt4PFJets.clone( rParam=0.2 )
kt3PFJets     = kt4PFJets.clone( rParam=0.3 )
kt5PFJets     = kt4PFJets.clone( rParam=0.5 )
kt7PFJets     = kt4PFJets.clone( rParam=0.7 )
kt8PFJets     = kt4PFJets.clone( rParam=0.8 )
kt9PFJets     = kt4PFJets.clone( rParam=0.9 )
kt10PFJets    = kt4PFJets.clone( rParam=1.0 )

#!
#! PF JETS CHS
#!
kt1PFCHSJets  = kt1PFJets.clone( src = 'pfNoPileUpJME' )
kt2PFCHSJets  = kt1PFCHSJets.clone( rParam=0.2 )
kt3PFCHSJets  = kt1PFCHSJets.clone( rParam=0.3 )
kt4PFCHSJets  = kt1PFCHSJets.clone( rParam=0.4 )
kt5PFCHSJets  = kt1PFCHSJets.clone( rParam=0.5 )
kt6PFCHSJets  = kt1PFCHSJets.clone( rParam=0.6 )
kt7PFCHSJets  = kt1PFCHSJets.clone( rParam=0.7 )
kt8PFCHSJets  = kt1PFCHSJets.clone( rParam=0.8 )
kt9PFCHSJets  = kt1PFCHSJets.clone( rParam=0.9 )
kt10PFCHSJets = kt1PFCHSJets.clone( rParam=1.0 )

#!
#! PF JETS PUPPI
#!
kt1PUPPIJets  = kt5PFJets.clone( rParam=0.1, src = cms.InputTag('puppi'))
kt2PUPPIJets  = kt5PFJets.clone( rParam=0.2, src = cms.InputTag('puppi'))
kt3PUPPIJets  = kt5PFJets.clone( rParam=0.3, src = cms.InputTag('puppi'))
kt4PUPPIJets  = kt5PFJets.clone( rParam=0.4, src = cms.InputTag('puppi'))
kt5PUPPIJets  = kt5PFJets.clone( rParam=0.5, src = cms.InputTag('puppi'))
kt6PUPPIJets  = kt5PFJets.clone( rParam=0.6, src = cms.InputTag('puppi'))
kt7PUPPIJets  = kt5PFJets.clone( rParam=0.7, src = cms.InputTag('puppi'))
kt8PUPPIJets  = kt5PFJets.clone( rParam=0.8, src = cms.InputTag('puppi'))
kt9PUPPIJets  = kt5PFJets.clone( rParam=0.9, src = cms.InputTag('puppi'))
kt10PUPPIJets = kt5PFJets.clone( rParam=1.0, src = cms.InputTag('puppi'))

####################
# CA JET PRODUCERS #
####################

#!
#! GEN JET PRODUCERS
#!
ca1GenJets   = ca4GenJets.clone( rParam=0.1 )
ca2GenJets   = ca4GenJets.clone( rParam=0.2 )
ca3GenJets   = ca4GenJets.clone( rParam=0.3 )
ca5GenJets   = ca4GenJets.clone( rParam=0.5 )
ca6GenJets   = ca4GenJets.clone( rParam=0.6 )
ca7GenJets   = ca4GenJets.clone( rParam=0.7 )
ca9GenJets   = ca4GenJets.clone( rParam=0.9 )
ca10GenJets  = ca4GenJets.clone( rParam=1.0 )

#!
#! GEN JETS WITHOUT NEUTRINOS
#!
ca1GenJetsNoNu  = ca1GenJets.clone ( src = 'genParticlesForJetsNoNu' )
ca2GenJetsNoNu  = ca2GenJets.clone ( src = 'genParticlesForJetsNoNu' )
ca3GenJetsNoNu  = ca3GenJets.clone ( src = 'genParticlesForJetsNoNu' )
ca5GenJetsNoNu  = ca5GenJets.clone ( src = 'genParticlesForJetsNoNu' )
ca6GenJetsNoNu  = ca6GenJets.clone ( src = 'genParticlesForJetsNoNu' )
ca7GenJetsNoNu  = ca7GenJets.clone ( src = 'genParticlesForJetsNoNu' )
ca9GenJetsNoNu  = ca9GenJets.clone ( src = 'genParticlesForJetsNoNu' )
ca10GenJetsNoNu = ca10GenJets.clone ( src = 'genParticlesForJetsNoNu' )

#!
#! GENJETS WITHOUT MUONS & NEUTRINOS
#!
ca1GenJetsNoMuNoNu  = ca1GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
ca2GenJetsNoMuNoNu  = ca2GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
ca3GenJetsNoMuNoNu  = ca3GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
ca5GenJetsNoMuNoNu  = ca5GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
ca6GenJetsNoMuNoNu  = ca6GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
ca7GenJetsNoMuNoNu  = ca7GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
ca9GenJetsNoMuNoNu  = ca9GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
ca10GenJetsNoMuNoNu = ca10GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )

#!
#! CALO JETS
#!
ca1CaloJets  = ca4CaloJets.clone( rParam=0.1 )
ca2CaloJets  = ca4CaloJets.clone( rParam=0.2 )
ca3CaloJets  = ca4CaloJets.clone( rParam=0.3 )
ca5CaloJets  = ca4CaloJets.clone( rParam=0.5 )
ca7CaloJets  = ca4CaloJets.clone( rParam=0.7 )
ca8CaloJets  = ca4CaloJets.clone( rParam=0.8 )
ca9CaloJets  = ca4CaloJets.clone( rParam=0.9 )
ca10CaloJets = ca4CaloJets.clone( rParam=1.0 )

#!
#! PF JETS
#!
ca1PFJets    = ca4PFJets.clone( rParam=0.1 )
ca2PFJets    = ca4PFJets.clone( rParam=0.2 )
ca3PFJets    = ca4PFJets.clone( rParam=0.3 )
ca5PFJets    = ca4PFJets.clone( rParam=0.5 )
ca6PFJets    = ca4PFJets.clone( rParam=0.6 )
ca7PFJets    = ca4PFJets.clone( rParam=0.7 )
ca9PFJets    = ca4PFJets.clone( rParam=0.9 )
ca10PFJets   = ca4PFJets.clone( rParam=1.0 )

#!
#! PF JETS CHS
#!
ca1PFchsJets  = ca1PFJets.clone( src = 'pfNoPileUpJME' )
ca2PFchsJets  = ca1PFchsJets.clone( rParam=0.2 )
ca3PFchsJets  = ca1PFchsJets.clone( rParam=0.3 )
ca4PFchsJets  = ca1PFchsJets.clone( rParam=0.4 )
ca5PFchsJets  = ca1PFchsJets.clone( rParam=0.5 )
ca6PFchsJets  = ca1PFchsJets.clone( rParam=0.6 )
ca7PFchsJets  = ca1PFchsJets.clone( rParam=0.7 )
ca8PFchsJets  = ca1PFchsJets.clone( rParam=0.8 )
ca9PFchsJets  = ca1PFchsJets.clone( rParam=0.9 )
ca10PFchsJets = ca1PFchsJets.clone( rParam=1.0 )

#!
#! PF JETS PUPPI
#!
ca1PUPPIJets  = ca5PFJets.clone( rParam=0.1, src = cms.InputTag('puppi'))
ca2PUPPIJets  = ca5PFJets.clone( rParam=0.2, src = cms.InputTag('puppi'))
ca3PUPPIJets  = ca5PFJets.clone( rParam=0.3, src = cms.InputTag('puppi'))
ca4PUPPIJets  = ca5PFJets.clone( rParam=0.4, src = cms.InputTag('puppi'))
ca5PUPPIJets  = ca5PFJets.clone( rParam=0.5, src = cms.InputTag('puppi'))
ca6PUPPIJets  = ca5PFJets.clone( rParam=0.6, src = cms.InputTag('puppi'))
ca7PUPPIJets  = ca5PFJets.clone( rParam=0.7, src = cms.InputTag('puppi'))
ca8PUPPIJets  = ca5PFJets.clone( rParam=0.8, src = cms.InputTag('puppi'))
ca9PUPPIJets  = ca5PFJets.clone( rParam=0.9, src = cms.InputTag('puppi'))
ca10PUPPIJets = ca5PFJets.clone( rParam=1.0, src = cms.InputTag('puppi'))

###############################
# SIS CONE & IC JET PRODUCERS #
###############################

#!
#! GEN JET PRODUCERS
#!
sc5GenJets = sisCone5GenJets.clone()
sisCone7GenJets = sisCone5GenJets.clone ( rParam=0.7 )
sc7GenJets = sisCone7GenJets.clone()
ic5GenJets = iterativeCone5GenJets.clone()

#!
#! GEN JETS WITHOUT NEUTRINOS
#!
sc5GenJetsNoNu  = sc5GenJets.clone ( src = 'genParticlesForJetsNoNu' )
sc7GenJetsNoNu  = sc7GenJets.clone ( src = 'genParticlesForJetsNoNu' )
ic5GenJetsNoNu  = ic5GenJets.clone ( src = 'genParticlesForJetsNoNu' )

#!
#! GENJETS WITHOUT MUONS & NEUTRINOS
#!
sc5GenJetsNoMuNoNu  = sc5GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
sc7GenJetsNoMuNoNu  = sc7GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
ic5GenJetsNoMuNoNu  = ic5GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )

#!
#! CALO JETS
#!
sc5CaloJets = sisCone5CaloJets.clone()
sc7CaloJets = sisCone7CaloJets.clone()
ic5CaloJets = iterativeCone5CaloJets.clone()

#!
#! PF JETS
#!
sc5PFJets = sisCone5PFJets.clone()
sc7PFJets = sisCone7PFJets.clone()
ic5PFJets = iterativeCone5PFJets.clone()
'''

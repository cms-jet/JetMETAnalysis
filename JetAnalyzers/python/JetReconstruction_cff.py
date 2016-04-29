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

# extra producerts for ak
ak5PFchsJets     = ak5PFJets.clone( src = 'pfNoPileUpJME' )
ak7PFchsJets     = ak5PFchsJets.clone( rParam=0.7 )
ak5CaloHLTJets   = ak5CaloJets.clone( src = 'hltAntiKT5CaloJets' )
ak5PFHLTJets     = ak5PFJets.clone( src = 'hltAntiKT5PFJets' )
ak5PFchsHLTJets  = ak5PFJets.clone( src = 'hltAntiKT5PFJetsNoPUPixelVert' )

ak1GenJets      = ak5GenJets.clone  ( rParam=0.1 )
ak1PFJets       = ak5PFJets.clone   ( rParam=0.1 )
ak1PFchsJets    = ak5PFchsJets.clone( rParam=0.1 )
ak1PUPPIJets    = ak5PFJets.clone   ( rParam=0.1, src = cms.InputTag('puppi'))
ak2GenJets      = ak5GenJets.clone  ( rParam=0.2 )
ak2PFJets       = ak5PFJets.clone   ( rParam=0.2 )
ak2PFchsJets    = ak5PFchsJets.clone( rParam=0.2 )
ak2PUPPIJets    = ak5PFJets.clone   ( rParam=0.2, src = cms.InputTag('puppi'))
ak3GenJets      = ak5GenJets.clone  ( rParam=0.3 )
ak3PFJets       = ak5PFJets.clone   ( rParam=0.3 )
ak3PFchsJets    = ak5PFchsJets.clone( rParam=0.3 )
ak3PUPPIJets    = ak5PFJets.clone   ( rParam=0.3, src = cms.InputTag('puppi'))
ak4GenJets      = ak5GenJets.clone  ( rParam=0.4 )
ak4PFJets       = ak5PFJets.clone   ( rParam=0.4 )
ak4PFchsJets    = ak5PFchsJets.clone( rParam=0.4 )
ak4PUPPIJets    = ak5PFJets.clone   ( rParam=0.4, src = cms.InputTag('puppi'))
ak5CaloJets     = ak4CaloJets.clone ( rParam=0.5 )
ak5PUPPIJets    = ak5PFJets.clone   ( rParam=0.5, src = cms.InputTag('puppi'))
ak6GenJets      = ak5GenJets.clone  ( rParam=0.6 )
ak6PFJets       = ak5PFJets.clone   ( rParam=0.6 )
ak6PFchsJets    = ak5PFchsJets.clone( rParam=0.6 )
ak6PUPPIJets    = ak5PFJets.clone   ( rParam=0.6, src = cms.InputTag('puppi'))
ak7GenJets      = ak5GenJets.clone  ( rParam=0.7 )
ak7PUPPIJets    = ak5PFJets.clone   ( rParam=0.7, src = cms.InputTag('puppi'))
ak8GenJets      = ak5GenJets.clone  ( rParam=0.8 )
ak8CaloJets     = ak5CaloJets.clone ( rParam=0.8 ) # for support HLT usage
ak8PFJets       = ak5PFJets.clone   ( rParam=0.8 )
ak8PFchsJets    = ak5PFchsJets.clone( rParam=0.8 )
ak8PUPPIJets    = ak5PFJets.clone   ( rParam=0.8, src = cms.InputTag('puppi'))
ak9GenJets      = ak5GenJets.clone  ( rParam=0.9 )
ak9PFJets       = ak5PFJets.clone   ( rParam=0.9 )
ak9PFchsJets    = ak5PFchsJets.clone( rParam=0.9 )
ak9PUPPIJets    = ak5PFJets.clone   ( rParam=0.9, src = cms.InputTag('puppi'))
ak10GenJets     = ak5GenJets.clone  ( rParam=1.0 )
ak10PFJets      = ak5PFJets.clone   ( rParam=1.0 )
ak10PFchsJets   = ak5PFchsJets.clone( rParam=1.0 )
ak10PUPPIJets   = ak5PFJets.clone   ( rParam=1.0, src = cms.InputTag('puppi'))

# objects for HLT usage
ak4CaloHLTJets = ak4CaloJets.clone( src = 'hltAK4CaloJets' )
ak4PFHLTJets = ak4PFJets.clone( src = 'hltAK4PFJets' )
ak8CaloHLTJets = ak8CaloJets.clone( src = 'hltAK8CaloJets' )
ak8PFHLTJets = ak8PFJets.clone( src = 'hltAK8PFJets' )

# sc & ic clones
sc5GenJets = sisCone5GenJets.clone()
sisCone7GenJets = sisCone5GenJets.clone ( rParam=0.7 )
sc7GenJets = sisCone7GenJets.clone()
ic5GenJets = iterativeCone5GenJets.clone()
sc5CaloJets = sisCone5CaloJets.clone()
sc7CaloJets = sisCone7CaloJets.clone()
ic5CaloJets = iterativeCone5CaloJets.clone()
sc5PFJets = sisCone5PFJets.clone()
sc7PFJets = sisCone7PFJets.clone()
ic5PFJets = iterativeCone5PFJets.clone()
#sc5TrackJets = sisCone5TrackJets.clone()
#sc7TrackJets = sisCone7TrackJets.clone()
#ic5TrackJets = iterativeCone5TrackJets.clone()

# extra producers for kt
kt5GenJets   = kt4GenJets.clone( rParam=0.5 )
kt7GenJets   = kt4GenJets.clone( rParam=0.7 )
kt5CaloJets  = kt4CaloJets.clone( rParam=0.5 )
kt7CaloJets  = kt4CaloJets.clone( rParam=0.7 )
kt5PFJets    = kt4PFJets.clone( rParam=0.5 )
kt7PFJets    = kt4PFJets.clone( rParam=0.7 )
#kt5TrackJets = kt4TrackJets.clone( rParam=0.5 )
#kt7TrackJets = kt4TrackJets.clone( rParam=0.7 )

# extra producers for ca
ca5GenJets   = ca4GenJets.clone( rParam=0.5 )
ca7GenJets   = ca4GenJets.clone( rParam=0.7 )
ca5CaloJets  = ca4CaloJets.clone( rParam=0.5 )
ca7CaloJets  = ca4CaloJets.clone( rParam=0.7 )
ca5PFJets    = ca4PFJets.clone( rParam=0.5 )
ca7PFJets    = ca4PFJets.clone( rParam=0.7 )
#ca5TrackJets = ca4TrackJets.clone( rParam=0.5 )
#ca7TrackJets = ca4TrackJets.clone( rParam=0.7 )

# genjets without muons & neutrinos
ak1GenJetsNoMuNoNu  = ak1GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
ak2GenJetsNoMuNoNu  = ak2GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
ak3GenJetsNoMuNoNu  = ak3GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
ak4GenJetsNoMuNoNu  = ak4GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
ak5GenJetsNoMuNoNu  = ak5GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
ak6GenJetsNoMuNoNu  = ak6GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
ak7GenJetsNoMuNoNu  = ak7GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
ak8GenJetsNoMuNoNu  = ak8GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
ak9GenJetsNoMuNoNu  = ak9GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
ak10GenJetsNoMuNoNu = ak10GenJets.clone( src = 'genParticlesForJetsNoMuNoNu' )
sc5GenJetsNoMuNoNu  = sc5GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
sc7GenJetsNoMuNoNu  = sc7GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
ic5GenJetsNoMuNoNu  = ic5GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
kt4GenJetsNoMuNoNu  = kt4GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
kt5GenJetsNoMuNoNu  = kt5GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
kt6GenJetsNoMuNoNu  = kt6GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
kt7GenJetsNoMuNoNu  = kt7GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
ca4GenJetsNoMuNoNu  = ca4GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
ca5GenJetsNoMuNoNu  = ca5GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
#ca6GenJetsNoMuNoNu  = ca6GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )
ca7GenJetsNoMuNoNu  = ca7GenJets.clone ( src = 'genParticlesForJetsNoMuNoNu' )

# genjets without neutrinos
ak1GenJetsNoNu  = ak1GenJets.clone ( src = 'genParticlesForJetsNoNu' )
ak2GenJetsNoNu  = ak2GenJets.clone ( src = 'genParticlesForJetsNoNu' )
ak3GenJetsNoNu  = ak3GenJets.clone ( src = 'genParticlesForJetsNoNu' )
ak4GenJetsNoNu  = ak4GenJets.clone ( src = 'genParticlesForJetsNoNu' )
ak5GenJetsNoNu  = ak5GenJets.clone ( src = 'genParticlesForJetsNoNu' )
ak6GenJetsNoNu  = ak6GenJets.clone ( src = 'genParticlesForJetsNoNu' )
ak7GenJetsNoNu  = ak7GenJets.clone ( src = 'genParticlesForJetsNoNu' )
ak8GenJetsNoNu  = ak8GenJets.clone ( src = 'genParticlesForJetsNoNu' )
ak9GenJetsNoNu  = ak9GenJets.clone ( src = 'genParticlesForJetsNoNu' )
ak10GenJetsNoNu = ak10GenJets.clone( src = 'genParticlesForJetsNoNu' )
sc5GenJetsNoNu  = sc5GenJets.clone ( src = 'genParticlesForJetsNoNu' )
sc7GenJetsNoNu  = sc7GenJets.clone ( src = 'genParticlesForJetsNoNu' )
ic5GenJetsNoNu  = ic5GenJets.clone ( src = 'genParticlesForJetsNoNu' )
kt4GenJetsNoNu  = kt4GenJets.clone ( src = 'genParticlesForJetsNoNu' )
kt5GenJetsNoNu  = kt5GenJets.clone ( src = 'genParticlesForJetsNoNu' )
kt6GenJetsNoNu  = kt6GenJets.clone ( src = 'genParticlesForJetsNoNu' )
kt7GenJetsNoNu  = kt7GenJets.clone ( src = 'genParticlesForJetsNoNu' )
ca4GenJetsNoNu  = ca4GenJets.clone ( src = 'genParticlesForJetsNoNu' )
ca5GenJetsNoNu  = ca5GenJets.clone ( src = 'genParticlesForJetsNoNu' )
#ca6GenJetsNoNu  = ca6GenJets.clone ( src = 'genParticlesForJetsNoNu' )
ca7GenJetsNoNu  = ca7GenJets.clone ( src = 'genParticlesForJetsNoNu' )

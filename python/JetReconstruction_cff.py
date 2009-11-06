import FWCore.ParameterSet.Config as cms

# event setup
from Configuration.StandardSequences.Geometry_cff import *
from Configuration.StandardSequences.MagneticField_cff import *

# jet reconstruction
from RecoJets.Configuration.GenJetParticles_cff import *
from RecoJets.Configuration.RecoGenJets_cff import *
from RecoJets.Configuration.RecoJets_cff import *
from RecoJets.Configuration.RecoPFJets_cff import *
from RecoJets.Configuration.RecoTrackJets_cff import *

# sc & ic clones
sc5GenJets = sisCone5GenJets.clone()
sc7GenJets = sisCone7GenJets.clone()
ic5GenJets = iterativeCone5GenJets.clone()
sc5CaloJets = sisCone5CaloJets.clone()
sc7CaloJets = sisCone7CaloJets.clone()
ic5CaloJets = iterativeCone5CaloJets.clone()
sc5PFJets = sisCone5PFJets.clone()
sc7PFJets = sisCone7PFJets.clone()
ic5PFJets = iterativeCone5PFJets.clone()
sc5TrackJets = sisCone5TrackJets.clone()
sc7TrackJets = sisCone7TrackJets.clone()
ic5TrackJets = iterativeCone5TrackJets.clone()

# extra producers for kt
kt5GenJets   = kt4GenJets.clone( rParam=0.5 )
kt7GenJets   = kt4GenJets.clone( rParam=0.7 )
kt5CaloJets  = kt4CaloJets.clone( rParam=0.5 )
kt7CaloJets  = kt4CaloJets.clone( rParam=0.7 )
kt5PFJets    = kt4PFJets.clone( rParam=0.5 )
kt7PFJets    = kt4PFJets.clone( rParam=0.7 )
kt5TrackJets = kt4TrackJets.clone( rParam=0.5 )
kt7TrackJets = kt4TrackJets.clone( rParam=0.7 )

# extra producers for ca
ca5GenJets   = ca4GenJets.clone( rParam=0.5 )
ca7GenJets   = ca4GenJets.clone( rParam=0.7 )
ca5CaloJets  = ca4CaloJets.clone( rParam=0.5 )
ca7CaloJets  = ca4CaloJets.clone( rParam=0.7 )
ca5PFJets    = ca4PFJets.clone( rParam=0.5 )
ca7PFJets    = ca4PFJets.clone( rParam=0.7 )
ca5TrackJets = ca4TrackJets.clone( rParam=0.5 )
ca7TrackJets = ca4TrackJets.clone( rParam=0.7 )

# gen particle filter for pf / trk / jpt jets
genParticlesForJetsNoNus = genParticlesForJets.clone()
genParticlesForJetsNoNus.ignoreParticleIDs.extend([12,14,16])

# gen particle filter for calo jets
genParticlesForJetsNoMusNoNus = genParticlesForJetsNoNus.clone()
genParticlesForJetsNoMusNoNus.ignoreParticleIDs.extend([13])

# genjets without muons & neutrinos
ak5GenJetsNoMusNoNus = ak5GenJets.clone( src = 'genParticlesForJetsNoMusNoNus' )
ak7GenJetsNoMusNoNus = ak7GenJets.clone( src = 'genParticlesForJetsNoMusNoNus' )
sc5GenJetsNoMusNoNus = sc5GenJets.clone( src = 'genParticlesForJetsNoMusNoNus' )
sc7GenJetsNoMusNoNus = sc7GenJets.clone( src = 'genParticlesForJetsNoMusNoNus' )
ic5GenJetsNoMusNoNus = ic5GenJets.clone( src = 'genParticlesForJetsNoMusNoNus' )
kt4GenJetsNoMusNoNus = kt4GenJets.clone( src = 'genParticlesForJetsNoMusNoNus' )
kt5GenJetsNoMusNoNus = kt5GenJets.clone( src = 'genParticlesForJetsNoMusNoNus' )
kt6GenJetsNoMusNoNus = kt6GenJets.clone( src = 'genParticlesForJetsNoMusNoNus' )
kt7GenJetsNoMusNoNus = kt7GenJets.clone( src = 'genParticlesForJetsNoMusNoNus' )
ca4GenJetsNoMusNoNus = ca4GenJets.clone( src = 'genParticlesForJetsNoMusNoNus' )
ca5GenJetsNoMusNoNus = ca5GenJets.clone( src = 'genParticlesForJetsNoMusNoNus' )
ca6GenJetsNoMusNoNus = ca6GenJets.clone( src = 'genParticlesForJetsNoMusNoNus' )
ca7GenJetsNoMusNoNus = ca7GenJets.clone( src = 'genParticlesForJetsNoMusNoNus' )

# genjets without neutrinos
ak5GenJetsNoNus = ak5GenJets.clone( src = 'genParticlesForJetsNoNus' )
ak7GenJetsNoNus = ak7GenJets.clone( src = 'genParticlesForJetsNoNus' )
sc5GenJetsNoNus = sc5GenJets.clone( src = 'genParticlesForJetsNoNus' )
sc7GenJetsNoNus = sc7GenJets.clone( src = 'genParticlesForJetsNoNus' )
ic5GenJetsNoNus = ic5GenJets.clone( src = 'genParticlesForJetsNoNus' )
kt4GenJetsNoNus = kt4GenJets.clone( src = 'genParticlesForJetsNoNus' )
kt5GenJetsNoNus = kt5GenJets.clone( src = 'genParticlesForJetsNoNus' )
kt6GenJetsNoNus = kt6GenJets.clone( src = 'genParticlesForJetsNoNus' )
kt7GenJetsNoNus = kt7GenJets.clone( src = 'genParticlesForJetsNoNus' )
ca4GenJetsNoNus = ca4GenJets.clone( src = 'genParticlesForJetsNoNus' )
ca5GenJetsNoNus = ca5GenJets.clone( src = 'genParticlesForJetsNoNus' )
ca6GenJetsNoNus = ca6GenJets.clone( src = 'genParticlesForJetsNoNus' )
ca7GenJetsNoNus = ca7GenJets.clone( src = 'genParticlesForJetsNoNus' )

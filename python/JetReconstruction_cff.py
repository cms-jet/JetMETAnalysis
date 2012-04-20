import FWCore.ParameterSet.Config as cms

# event setup
from Configuration.StandardSequences.Geometry_cff import *
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
ak5PFchsJets = ak5PFJets.clone( src = 'pfNoPileUp' )
ak7PFchsJets = ak5PFchsJets.clone( rParam=0.7 )
ak5CaloHLTJets = ak5CaloJets.clone( src = 'hltAntiKT5CaloJets' )
ak5PFHLTJets = ak5PFJets.clone( src = 'hltAntiKT5PFJets' )
ak5PFchsHLTJets = ak5PFJets.clone( src = 'hltAntiKT5PFJetsNoPUPixelVert' )
kt6CaloJets50 = kt6CaloJets.clone()
kt6PFJets50 = kt6PFJets.clone()

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
ak5GenJetsNoMuNoNu = ak5GenJets.clone( src = 'genParticlesForJetsNoMuNoNu' )
ak7GenJetsNoMuNoNu = ak7GenJets.clone( src = 'genParticlesForJetsNoMuNoNu' )
sc5GenJetsNoMuNoNu = sc5GenJets.clone( src = 'genParticlesForJetsNoMuNoNu' )
sc7GenJetsNoMuNoNu = sc7GenJets.clone( src = 'genParticlesForJetsNoMuNoNu' )
ic5GenJetsNoMuNoNu = ic5GenJets.clone( src = 'genParticlesForJetsNoMuNoNu' )
kt4GenJetsNoMuNoNu = kt4GenJets.clone( src = 'genParticlesForJetsNoMuNoNu' )
kt5GenJetsNoMuNoNu = kt5GenJets.clone( src = 'genParticlesForJetsNoMuNoNu' )
kt6GenJetsNoMuNoNu = kt6GenJets.clone( src = 'genParticlesForJetsNoMuNoNu' )
kt7GenJetsNoMuNoNu = kt7GenJets.clone( src = 'genParticlesForJetsNoMuNoNu' )
ca4GenJetsNoMuNoNu = ca4GenJets.clone( src = 'genParticlesForJetsNoMuNoNu' )
ca5GenJetsNoMuNoNu = ca5GenJets.clone( src = 'genParticlesForJetsNoMuNoNu' )
ca6GenJetsNoMuNoNu = ca6GenJets.clone( src = 'genParticlesForJetsNoMuNoNu' )
ca7GenJetsNoMuNoNu = ca7GenJets.clone( src = 'genParticlesForJetsNoMuNoNu' )

# genjets without neutrinos
ak5GenJetsNoNu = ak5GenJets.clone( src = 'genParticlesForJetsNoNu' )
ak7GenJetsNoNu = ak7GenJets.clone( src = 'genParticlesForJetsNoNu' )
sc5GenJetsNoNu = sc5GenJets.clone( src = 'genParticlesForJetsNoNu' )
sc7GenJetsNoNu = sc7GenJets.clone( src = 'genParticlesForJetsNoNu' )
ic5GenJetsNoNu = ic5GenJets.clone( src = 'genParticlesForJetsNoNu' )
kt4GenJetsNoNu = kt4GenJets.clone( src = 'genParticlesForJetsNoNu' )
kt5GenJetsNoNu = kt5GenJets.clone( src = 'genParticlesForJetsNoNu' )
kt6GenJetsNoNu = kt6GenJets.clone( src = 'genParticlesForJetsNoNu' )
kt7GenJetsNoNu = kt7GenJets.clone( src = 'genParticlesForJetsNoNu' )
ca4GenJetsNoNu = ca4GenJets.clone( src = 'genParticlesForJetsNoNu' )
ca5GenJetsNoNu = ca5GenJets.clone( src = 'genParticlesForJetsNoNu' )
ca6GenJetsNoNu = ca6GenJets.clone( src = 'genParticlesForJetsNoNu' )
ca7GenJetsNoNu = ca7GenJets.clone( src = 'genParticlesForJetsNoNu' )

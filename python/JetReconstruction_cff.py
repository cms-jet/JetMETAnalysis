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


antikt5GenJets   = ak5GenJets.clone()
antikt7GenJets   = ak7GenJets.clone()
antikt5CaloJets  = ak5CaloJets.clone()
antikt7CaloJets  = ak7CaloJets.clone()
antikt5PFJets    = ak5PFJets.clone()
antikt7PFJets    = ak7PFJets.clone()
antikt5TrackJets = ak5TrackJets.clone()
antikt7TrackJets = ak7TrackJets.clone()

kt5GenJets   = kt4GenJets.clone( rParam=0.5 )
kt7GenJets   = kt4GenJets.clone( rParam=0.7 )
kt5CaloJets  = kt4CaloJets.clone( rParam=0.5 )
kt7CaloJets  = kt4CaloJets.clone( rParam=0.7 )
kt5PFJets    = kt4PFJets.clone( rParam=0.5 )
kt7PFJets    = kt4PFJets.clone( rParam=0.7 )
kt5TrackJets = kt4TrackJets.clone( rParam=0.5 )
kt7TrackJets = kt4TrackJets.clone( rParam=0.7 )

ca5GenJets   = ca4GenJets.clone( rParam=0.5 )
ca7GenJets   = ca4GenJets.clone( rParam=0.7 )
ca5CaloJets  = ca4CaloJets.clone( rParam=0.5 )
ca7CaloJets  = ca4CaloJets.clone( rParam=0.7 )
ca5PFJets    = ca4PFJets.clone( rParam=0.5 )
ca7PFJets    = ca4PFJets.clone( rParam=0.7 )
ca5TrackJets = ca4TrackJets.clone( rParam=0.5 )
ca7TrackJets = ca4TrackJets.clone( rParam=0.7 )


# GENJETS
recoStandardGenJets = cms.Path(genJetParticles+
                               antikt5GenJets+
                               kt4GenJets+
                               kt6GenJets+
                               sisCone5GenJets+
                               sisCone7GenJets+
                               iterativeCone5GenJets
                               )

recoExtraGenJets = cms.Path(genJetParticles+
                            kt5GenJets+
                            kt7GenJets+
                            antikt7GenJets+
                            ca4GenJets+
                            ca5GenJets+
                            ca6GenJets+
                            ca7GenJets
                            )


# CALOJETS
recoStandardCaloJets = cms.Path(antikt5CaloJets+
                                kt4CaloJets+
                                kt6CaloJets+
                                sisCone5CaloJets+
                                sisCone7CaloJets+
                                iterativeCone5CaloJets
                                )


recoExtraCaloJets = cms.Path(kt5CaloJets+
                             kt7CaloJets+
                             antikt7CaloJets+
                             ca4CaloJets+
                             ca5CaloJets+
                             ca6CaloJets+
                             ca7CaloJets
                             )

# PFJETS
recoStandardPFJets = cms.Path(antikt5PFJets+
                              kt4PFJets+
                              kt6PFJets+
                              sisCone5PFJets+
                              sisCone7PFJets+
                              iterativeCone5PFJets
                              )

recoExtraPFJets = cms.Path(kt5PFJets+
                           kt7PFJets+
                           antikt7PFJets+
                           ca4PFJets+
                           ca5PFJets+
                           ca6PFJets+
                           ca7PFJets
                           )


# TRACKJETS
recoStandardTrackJets = cms.Path(tracksForJets+
                                 antikt5TrackJets+                                 
                                 kt4TrackJets+
                                 kt6TrackJets+
                                 sisCone5TrackJets+
                                 sisCone7TrackJets+
                                 iterativeCone5TrackJets
                               )

recoExtraTrackJets = cms.Path(tracksForJets+
                              kt5TrackJets+
                              kt7TrackJets+
                              antikt7TrackJets+
                              ca4TrackJets+
                              ca5TrackJets+
                              ca6TrackJets+
                              ca7TrackJets
                              )

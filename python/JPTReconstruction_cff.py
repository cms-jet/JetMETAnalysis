import FWCore.ParameterSet.Config as cms

# event setup
from Configuration.StandardSequences.Geometry_cff import *
from Configuration.StandardSequences.MagneticField_cff import *

# zsp & jpt
from JetMETCorrections.Configuration.JetPlusTrackCorrections_cff import *
from JetMETCorrections.Configuration.ZSPJetCorrections152_cff import *

recoStandardJPTJets = cms.Path(ZSPJetCorrections+JetPlusTrackCorrections)

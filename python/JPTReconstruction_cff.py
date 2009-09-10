import FWCore.ParameterSet.Config as cms

# event setup
#from Configuration.StandardSequences.Geometry_cff import *
#from Configuration.StandardSequences.MagneticField_cff import *

from JetMETCorrections.Configuration.ZSPJetCorrections219_cff import *
from JetMETCorrections.Configuration.JetPlusTrackCorrections_cff import *

recoStandardJPTJets = cms.Path(ZSPJetCorrections*JetPlusTrackCorrections)

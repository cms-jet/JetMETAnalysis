import FWCore.ParameterSet.Config as cms


from JetMETCorrections.Configuration.L2L3Corrections_Summer09_cff import *

# TEMPORARY
from JetMETCorrections.Configuration.L2L3Corrections_Summer09_7TeV_cff import L2JetCorrectorAK7Calo,L2JetCorrectorAK7PF,L3JetCorrectorAK7Calo,L3JetCorrectorAK7PF,L2L3JetCorrectorAK7Calo,L2L3JetCorrectorAK7PF,L2L3CorJetAK7Calo,L2L3CorJetAK7PF

# create properly named standard jet l2l3 producers
ak5CaloJetsL2L3 = L2L3CorJetAK5Calo.clone()
ak7CaloJetsL2L3 = L2L3CorJetAK7Calo.clone()
kt4CaloJetsL2L3 = L2L3CorJetKT4Calo.clone()
kt6CaloJetsL2L3 = L2L3CorJetKT6Calo.clone()
sc5CaloJetsL2L3 = L2L3CorJetSC5Calo.clone()
sc7CaloJetsL2L3 = L2L3CorJetSC7Calo.clone()
ic5CaloJetsL2L3 = L2L3CorJetIC5Calo.clone()

ak5PFJetsL2L3 = L2L3CorJetAK5PF.clone()
ak7PFJetsL2L3 = L2L3CorJetAK7PF.clone()
kt4PFJetsL2L3 = L2L3CorJetKT4PF.clone()
kt6PFJetsL2L3 = L2L3CorJetKT6PF.clone()
sc5PFJetsL2L3 = L2L3CorJetSC5PF.clone()
sc7PFJetsL2L3 = L2L3CorJetSC7PF.clone()
ic5PFJetsL2L3 = L2L3CorJetIC5PF.clone()

# extra l2l3 producers for kt jets
kt5CaloJetsL2L3 = kt4CaloJetsL2L3.clone( src = 'kt5CaloJets' )
kt7CaloJetsL2L3 = kt6CaloJetsL2L3.clone( src = 'kt7CaloJets' )
kt5PFJetsL2L3   = kt4PFJetsL2L3.clone( src  = 'kt5PFJets' )
kt7PFJetsL2L3   = kt6PFJetsL2L3.clone( src  = 'kt7PFJets' )

# extra l2l3 producers for ca jets
ca4CaloJetsL2L3 = kt4CaloJetsL2L3.clone( src = 'ca4CaloJets' )
ca5CaloJetsL2L3 = ca4CaloJetsL2L3.clone( src = 'ca5CaloJets' )
ca6CaloJetsL2L3 = ca4CaloJetsL2L3.clone( src = 'ca6CaloJets' )
ca7CaloJetsL2L3 = ca6CaloJetsL2L3.clone( src = 'ca7CaloJets' )

ca4PFJetsL2L3 = kt4PFJetsL2L3.clone( src = 'ca4PFJets' )
ca5PFJetsL2L3 = ca4PFJetsL2L3.clone( src = 'ca5PFJets' )
ca6PFJetsL2L3 = ca4PFJetsL2L3.clone( src = 'ca6PFJets' )
ca7PFJetsL2L3 = ca6PFJetsL2L3.clone( src = 'ca7PFJets' )

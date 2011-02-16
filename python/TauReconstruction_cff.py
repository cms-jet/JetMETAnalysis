import FWCore.ParameterSet.Config as cms

#from RecoTauTag.RecoTau.RecoTauHPSTancTauProdcuer_cfi import hpsTancRecoTausSequence

tauHadronicGenJets = cms.EDFilter("TauGenJetDecayModeSelector",
      src = cms.InputTag("tauGenJets"),
      select = cms.vstring('oneProng0Pi0', 
                           'oneProng1Pi0', 
                           'oneProng2Pi0', 
                           'oneProngOther',
                           'threeProng0Pi0',
                           'threeProng1Pi0', 
                           'threeProngOther', 
                           'rare'
                            ), 
)

#from PhysicsTools.JetMCAlgos.TauGenJets_cfi import tauGenJets

#tauPreparationSequence = cms.Sequence(hpsTancRecoTausSequence * tauGenJets )
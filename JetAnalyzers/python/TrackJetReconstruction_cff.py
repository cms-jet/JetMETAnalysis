from RecoJets.Configuration.RecoTrackJets_cff import *

tracksForJets = cms.EDFilter("TrackSelector",
                             src = cms.InputTag("generalTracks"),
                             cut = cms.string('pt > 0.3 && quality("highPurity")')
                            )

trackWithVertexRefSelector.nVertices = 0
trackWithVertexRefSelector.ptErrorCut = 0.2
trackWithVertexRefSelector.src = cms.InputTag("tracksForJets")

trackJetSequence = cms.Sequence(tracksForJets + trackWithVertexRefSelector + trackRefsForJets)

ak4TrackJets.UseOnlyVertexTracks = False
ak4TrackJets.UseOnlyOnePV = False
ak4TrackJets.DxyTrVtxMax = 0.2
ak4TrackJets.DzTrVtxMax = 1.0
'''
kt4TrackJets.UseOnlyVertexTracks = False
kt4TrackJets.UseOnlyOnePV = False
kt4TrackJets.DxyTrVtxMax = 0.2
kt4TrackJets.DzTrVtxMax = 1.0
sisCone5TrackJets.UseOnlyVertexTracks = False
sisCone5TrackJets.UseOnlyOnePV = False
sisCone5TrackJets.DxyTrVtxMax = 0.2
sisCone5TrackJets.DzTrVtxMax = 1.0
'''

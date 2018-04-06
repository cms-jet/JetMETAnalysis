////////////////////////////////////////////////////////////////////////////////
//
// JetResponseAnalyzer
// -------------------
//
//            07/04/2008 Kostas Kousouris       <kkousour@fnal.gov>
//                       Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
//            12/08/2011 Alexx Perloff          <alexx.stephen.perloff@cern.ch>
////////////////////////////////////////////////////////////////////////////////

#include "JetMETAnalysis/JetAnalyzers/interface/JetResponseAnalyzer.hh"

////////////////////////////////////////////////////////////////////////////////
// construction/destruction
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
JetResponseAnalyzer::JetResponseAnalyzer(const edm::ParameterSet& iConfig)
  : moduleLabel_            (iConfig.getParameter<std::string>            ("@module_label"))
  , srcRef_                 (consumes<reco::CandidateView>(iConfig.getParameter<edm::InputTag>                ("srcRef")))
  , srcJetToUncorJetMap_    (consumes<reco::CandViewMatchMap>(iConfig.getParameter<edm::InputTag>("srcJetToUncorJetMap")))
  , srcRefToJetMap_         (consumes<reco::CandViewMatchMap>(iConfig.getParameter<edm::InputTag>     ("srcRefToJetMap")))
  , srcRhos_                (consumes<vector<double> >(iConfig.getParameter<edm::InputTag>                   ("srcRhos")))
  , srcRho_                 (consumes<double>(iConfig.getParameter<edm::InputTag>                             ("srcRho")))
  , srcRhoHLT_              (consumes<double>(iConfig.getParameter<edm::InputTag>                          ("srcRhoHLT")))
  , srcVtx_                 (consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>             ("srcVtx")))
  , srcGenInfo_             (consumes<GenEventInfoProduct>(edm::InputTag("generator"))                                   )
  , srcPileupInfo_          (consumes<vector<PileupSummaryInfo> >(edm::InputTag("addPileupInfo"))                        )
  //, srcPFCandidates_      (consumes<vector<reco::PFCandidate> >(iConfig.getParameter<edm::InputTag>("srcPFCandidates")))
  , srcPFCandidates_        (consumes<PFCandidateView>(iConfig.getParameter<edm::InputTag>("srcPFCandidates")))
  , srcPFCandidatesAsFwdPtr_(consumes<std::vector<edm::FwdPtr<reco::PFCandidate> > >(iConfig.getParameter<edm::InputTag>("srcPFCandidates")))
  , srcGenParticles_        (consumes<vector<reco::GenParticle> >(iConfig.getParameter<edm::InputTag>("srcGenParticles")))
  , jecLabel_      (iConfig.getParameter<std::string>                 ("jecLabel"))
  , doComposition_ (iConfig.getParameter<bool>                   ("doComposition"))
  , doFlavor_      (iConfig.getParameter<bool>                        ("doFlavor"))
  , doJetPt_       (iConfig.getParameter<bool>                         ("doJetPt"))
  , doRefPt_       (iConfig.getParameter<bool>                         ("doRefPt"))
  , doHLT_         (iConfig.getParameter<bool>                           ("doHLT"))
  , saveCandidates_(iConfig.getParameter<bool>                  ("saveCandidates"))
  , nRefMax_       (iConfig.getParameter<unsigned int>                 ("nRefMax"))
  , deltaRMax_(0.0)
  , deltaPhiMin_(3.141)
  , deltaRPartonMax_(0.0)
  , doBalancing_(false)
  , getFlavorFromMap_(false)
  , jetCorrector_(0)
{
  if (iConfig.exists("deltaRMax")) {
    doBalancing_=false;
     deltaRMax_=iConfig.getParameter<double>("deltaRMax");
  }
  else if (iConfig.exists("deltaPhiMin")) {
    doBalancing_=true;
    deltaPhiMin_=iConfig.getParameter<double>("deltaPhiMin");
  }
  else
    throw cms::Exception("MissingParameter")<<"Set *either* deltaRMax (matching)"
					    <<" *or* deltaPhiMin (balancing)";
  
  if (doFlavor_&&iConfig.exists("srcRefToPartonMap")) {
     srcRefToPartonMap_=consumes<reco::JetMatchedPartonsCollection>(iConfig.getParameter<edm::InputTag>("srcRefToPartonMap"));
    deltaRPartonMax_  =iConfig.getParameter<double>       ("deltaRPartonMax");
    getFlavorFromMap_=true;
  }
  
  isCaloJet_  = (moduleLabel_.find("calo")!=string::npos);
  isJPTJet_   = (moduleLabel_.find("jpt") !=string::npos);
  isPFJet_    = (moduleLabel_.find("pf")  !=string::npos || moduleLabel_.find("puppi")  !=string::npos);
  isTrackJet_ = (moduleLabel_.find("trk") !=string::npos);
  isTauJet_   = (moduleLabel_.find("tau") !=string::npos);

  int check = isCaloJet_+isJPTJet_+isPFJet_+isTrackJet_+isTauJet_;
  assert(check<2);

  //if (isCaloJet_)  cout<<"These are CaloJets  ("<<moduleLabel_<<")"<<endl;
  //if (isJPTJet_)   cout<<"These are JPTJets   ("<<moduleLabel_<<")"<<endl;
  //if (isPFJet_)    cout<<"These are PFJets    ("<<moduleLabel_<<")"<<endl;
  //if (isTrackJet_) cout<<"These are TrackJets ("<<moduleLabel_<<")"<<endl;
  //if (isTauJet_)   cout<<"These are TauJets   ("<<moduleLabel_<<")"<<endl;

  //must state that we are using the TFileService
  usesResource("TFileService");
  //setupTree();
  //cout << "This is the thread id: " << std::this_thread::get_id() << endl;
}


//______________________________________________________________________________
JetResponseAnalyzer::~JetResponseAnalyzer()
{

}


////////////////////////////////////////////////////////////////////////////////
// implementation of member functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void JetResponseAnalyzer::beginJob()
//void JetResponseAnalyzer::setupTree()
{
  edm::Service<TFileService> fs;
  if (!fs) throw edm::Exception(edm::errors::Configuration,
				"TFileService missing from configuration!");
  
  // Configuration flags: Mapping in JRAEvent.h
  int flag_int = (saveCandidates_*pow(2,7)) + (isPFJet_*pow(2,6)) +
                 (isCaloJet_*pow(2,5)) + (doComposition_*pow(2,4)) +
                 (doBalancing_*pow(2,3)) + (doFlavor_*pow(2,2)) +
                 (doHLT_*pow(2,1)) + (1);
  bitset<8> flags(flag_int);
  tree_=fs->make<TTree>("t","t");
  JRAEvt_ = new JRAEvent(tree_,flags);
}


//______________________________________________________________________________
void JetResponseAnalyzer::analyze(const edm::Event& iEvent,
                                  const edm::EventSetup& iSetup)
{
  beginEvent();

  // EVENT DATA HANDLES
  edm::Handle<GenEventInfoProduct>               genInfo;
  edm::Handle<vector<PileupSummaryInfo> >        puInfos;  
  edm::Handle<reco::CandidateView>               refs;
  edm::Handle<reco::CandViewMatchMap>            jetToUncorJetMap;
  edm::Handle<reco::CandViewMatchMap>            refToJetMap;
  edm::Handle<reco::JetMatchedPartonsCollection> refToPartonMap;
  edm::Handle<vector<double> >                   rhos;
  edm::Handle<double>                            rho;
  edm::Handle<double>                            rho_hlt;
  edm::Handle<reco::VertexCollection>            vtx;
  edm::Handle<PFCandidateView>                   pfCandidates;
  edm::Handle<std::vector<edm::FwdPtr<reco::PFCandidate> > >  pfCandidatesAsFwdPtr;
  edm::Handle<vector<reco::GenParticle> >        genParticles;

  // Jet CORRECTOR
  jetCorrector_ = (jecLabel_.empty()) ? 0 : JetCorrector::getJetCorrector(jecLabel_,iSetup);

  // GENERATOR INFORMATION
  JRAEvt_->pthat  = 0.0;
  JRAEvt_->weight = 1.0;
  if (iEvent.getByToken(srcGenInfo_,genInfo)) {
    if (genInfo->hasBinningValues()) JRAEvt_->pthat = (Float_t)genInfo->binningValues()[0];
    JRAEvt_->weight = (Float_t)genInfo->weight();
  }
  
  //RHO INFORMATION
  JRAEvt_->rho = 0.0;
  if (iEvent.getByToken(srcRho_,rho)) {
    JRAEvt_->rho = *rho;
  }

  //HLT RHO INFORMATION
  JRAEvt_->rho_hlt = 0.0;
  if (doHLT_) {
     if (iEvent.getByToken(srcRhoHLT_,rho_hlt)) {
       JRAEvt_->rho_hlt = *rho_hlt;
     }
  }

  //ETA DEPENDENT RHO INFORMATION
  if(iEvent.getByToken(srcRhos_,rhos)) {
     for(unsigned int i=0; i<rhos->size(); i++) {
        JRAEvt_->rhos->push_back((*rhos)[i]);
     }
  }

  //NPV INFORMATION
  JRAEvt_->npv = 0;
  if (iEvent.getByToken(srcVtx_,vtx)) {
     const reco::VertexCollection::const_iterator vtxEnd = vtx->end();
     for (reco::VertexCollection::const_iterator vtxIter = vtx->begin(); vtxEnd != vtxIter; ++vtxIter) {
        if (!vtxIter->isFake() && vtxIter->ndof()>=4 && fabs(vtxIter->z())<=24) {
           ++(JRAEvt_->npv);
           JRAEvt_->refdzvtx->push_back(0);//fabs(vtxIter->z()-);
        }
     }
  }
 
  //EVENT INFORMATION
  JRAEvt_->run = iEvent.id().run();
  JRAEvt_->lumi = iEvent.id().luminosityBlock();
  JRAEvt_->evt = iEvent.id().event();

  // GENERATED PV INFORMATION & PU DENSITY
  JRAEvt_->refpvz = -1000.0;
  iEvent.getByToken(srcGenParticles_, genParticles);
  for (size_t i = 0; i < genParticles->size(); ++i) {
     const reco::GenParticle & genIt = (*genParticles)[i];
     if ( genIt.isHardProcess() ) {
        JRAEvt_->refpvz = genIt.vz();
        break;
     }
  }
  int zbin = getBin(abs(JRAEvt_->refpvz),&vz.at(0),vz.size()-1);

  // MC PILEUP INFORMATION
  if (iEvent.getByToken(srcPileupInfo_,puInfos)) {
     for(unsigned int i=0; i<puInfos->size(); i++) {
        JRAEvt_->bxns->push_back((*puInfos)[i].getBunchCrossing());
        JRAEvt_->npus->push_back((*puInfos)[i].getPU_NumInteractions());
        JRAEvt_->tnpus->push_back((*puInfos)[i].getTrueNumInteractions());
        if((*puInfos)[i].getBunchCrossing() == 0) {
           unsigned int nzpositions = (*puInfos)[i].getPU_zpositions().size();
           JRAEvt_->pudensity = 0;
           JRAEvt_->gpudensity = 0;
           for (unsigned int j=0; j<nzpositions; ++j) {
              JRAEvt_->zpositions->push_back((*puInfos)[i].getPU_zpositions()[j]);
              if (abs(JRAEvt_->zpositions->back()-JRAEvt_->refpvz)<0.1) JRAEvt_->pudensity++; //N_PU/mm
              if (getBin(abs(JRAEvt_->zpositions->back()),&vz.at(0),vz.size()-1)==zbin) JRAEvt_->gpudensity++;
           }
           JRAEvt_->gpudensity/=(20.0*(vz[zbin+1]-vz[zbin]));
        }
        int sumptlowpttemp = 0;
        int sumpthighpttemp = 0;
        int ntrkslowpttemp = 0;
        int ntrkshighpttemp = 0;
        for(unsigned int j=0; j<(*puInfos)[i].getPU_sumpT_lowpT().size(); j++) {
          sumptlowpttemp += ((*puInfos)[i].getPU_sumpT_lowpT())[j];
        }
        for(unsigned int j=0; j<(*puInfos)[i].getPU_sumpT_highpT().size(); j++) {
          sumpthighpttemp += ((*puInfos)[i].getPU_sumpT_highpT())[j];
        }
        for(unsigned int j=0; j<(*puInfos)[i].getPU_ntrks_lowpT().size(); j++) {
          ntrkslowpttemp += ((*puInfos)[i].getPU_ntrks_lowpT())[j];
        }
        for(unsigned int j=0; j<(*puInfos)[i].getPU_ntrks_highpT().size(); j++) {
          ntrkshighpttemp += ((*puInfos)[i].getPU_ntrks_highpT())[j];
        }
        JRAEvt_->sumpt_lowpt->push_back(sumptlowpttemp);
        JRAEvt_->sumpt_highpt->push_back(sumpthighpttemp);
        JRAEvt_->ntrks_lowpt->push_back(ntrkslowpttemp);
        JRAEvt_->ntrks_highpt->push_back(ntrkshighpttemp);
     }
  }

  // REFERENCES & RECOJETS
  iEvent.getByToken(srcRef_,               refs);
  iEvent.getByToken(srcJetToUncorJetMap_, jetToUncorJetMap); 
  iEvent.getByToken(srcRefToJetMap_,refToJetMap);
  if (getFlavorFromMap_) iEvent.getByToken(srcRefToPartonMap_,refToPartonMap);
  if (doBalancing_&&refToJetMap->size()!=1) return;
  JRAEvt_->nref = 0;
  size_t nRef=(nRefMax_==0) ? refs->size() : std::min(nRefMax_,refs->size());
  for (size_t iRef=0;iRef<nRef;iRef++) {
     
     reco::CandidateBaseRef ref=refs->refAt(iRef);
     
     reco::CandViewMatchMap::const_iterator itMatch=refToJetMap->find(ref);
     if (itMatch==refToJetMap->end()) continue;
     reco::CandidateBaseRef jet=itMatch->val;

     if(doBalancing_)
       JRAEvt_->refdphijt->push_back(reco::deltaPhi(jet->phi(),ref->phi()));
     else
       JRAEvt_->refdrjt->push_back(reco::deltaR(jet->eta(),jet->phi(),ref->eta(),ref->phi()));
 
     if ((!doBalancing_&&JRAEvt_->refdrjt->at(JRAEvt_->nref)>deltaRMax_)||
         (doBalancing_&&std::abs(JRAEvt_->refdphijt->at(JRAEvt_->nref))<deltaPhiMin_)) {
        if(doBalancing_) JRAEvt_->refdphijt->pop_back();
        else JRAEvt_->refdrjt->pop_back();
        continue;
     }
     
     JRAEvt_->refpdgid->push_back(0);
     JRAEvt_->refpdgid_algorithmicDef->push_back(0);
     JRAEvt_->refpdgid_physicsDef->push_back(0);
     if (getFlavorFromMap_) {
        reco::JetMatchedPartonsCollection::const_iterator itPartonMatch;
        itPartonMatch=refToPartonMap->begin();
        for (;itPartonMatch!=refToPartonMap->end();++itPartonMatch) {
           reco::JetBaseRef jetRef = itPartonMatch->first;
           const reco::MatchedPartons partonMatch = itPartonMatch->second;
           const reco::Candidate* cand = &(*jetRef);
           if (cand==&(*ref)) break;
        }
        
        if (itPartonMatch!=refToPartonMap->end()&&
            itPartonMatch->second.algoDefinitionParton().get()!=0&&
            itPartonMatch->second.physicsDefinitionParton().get()!=0) {
           
           double refdrparton_algo=
              reco::deltaR(ref->p4(),
                           itPartonMatch->second.algoDefinitionParton().get()->p4());
           double refdrparton_physics=
              reco::deltaR(ref->p4(),
                           itPartonMatch->second.physicsDefinitionParton().get()->p4());
           
           if (refdrparton_algo<deltaRPartonMax_) {
              JRAEvt_->refpdgid_algorithmicDef->at(JRAEvt_->nref)=itPartonMatch->second.algoDefinitionParton().get()->pdgId();
              int absid = std::abs(JRAEvt_->refpdgid_algorithmicDef->at(JRAEvt_->nref));
              if (absid==4||absid==5) {
                 GenJetLeptonFinder finder(*ref);
                 finder.run();
                 if (finder.foundLeptonAndNeutrino()) {
                    int sign  = (JRAEvt_->refpdgid_algorithmicDef->at(JRAEvt_->nref)>0) ? +1 : -1;
                    JRAEvt_->refpdgid_algorithmicDef->at(JRAEvt_->nref) = sign*(absid*100+std::abs(finder.leptonPdgId()));
                 }
              }
           }
           if (refdrparton_physics<deltaRPartonMax_) {
              JRAEvt_->refpdgid_physicsDef->at(JRAEvt_->nref)=itPartonMatch->second.physicsDefinitionParton().get()->pdgId();
              int absid = std::abs(JRAEvt_->refpdgid_physicsDef->at(JRAEvt_->nref));
              if (absid==4||absid==5) {
                 GenJetLeptonFinder finder(*ref);
                 finder.run();
                 if (finder.foundLeptonAndNeutrino()) {
                    int sign  = (JRAEvt_->refpdgid_physicsDef->at(JRAEvt_->nref)>0) ? +1 : -1;
                    JRAEvt_->refpdgid_physicsDef->at(JRAEvt_->nref) = sign*(absid*100+std::abs(finder.leptonPdgId()));
                 }
              }
           }
        }
     }
     else {
        JRAEvt_->refpdgid_algorithmicDef->at(JRAEvt_->nref)=0;
        JRAEvt_->refpdgid_physicsDef->at(JRAEvt_->nref)=0;
     }
     JRAEvt_->refpdgid->at(JRAEvt_->nref)=ref->pdgId();

     // Beta/Beta Star Calculation
     JRAEvt_->beta = 0.0;
     JRAEvt_->betaStar = 0.0;
//     if (isPFJet_) {
//        //---- vertex association -----------
//        //---- get the vector of tracks -----
//        reco::PFJetRef pfJetRef = jet.castTo<reco::PFJetRef>();
//        reco::TrackRefVector vTrks(pfJetRef->getTrackRefs());
//        float sumTrkPt(0.0),sumTrkPtBeta(0.0),sumTrkPtBetaStar(0.0);
//        //---- loop over the tracks of the jet ----
//        for(reco::TrackRefVector::const_iterator i_trk = vTrks.begin(); i_trk != vTrks.end(); i_trk++) {
//           //if (npv_ == 0) break;
//           if ((*vtx).size() == 0) break;
//           sumTrkPt += (*i_trk)->pt();
//           //---- loop over all vertices ----------------------------
//           for(unsigned ivtx = 0;ivtx < (*vtx).size();ivtx++) {
//              //---- loop over the tracks associated with the vertex ---
//              if (!((*vtx)[ivtx].isFake()) && (*vtx)[ivtx].ndof() >= 4 && fabs((*vtx)[ivtx].z()) <= 24) {
//                 for(reco::Vertex::trackRef_iterator i_vtxTrk = (*vtx)[ivtx].tracks_begin(); i_vtxTrk != (*vtx)[ivtx].tracks_end(); ++i_vtxTrk) {
//                    //---- match the jet track to the track from the vertex ----
//                    reco::TrackRef trkRef(i_vtxTrk->castTo<reco::TrackRef>());
//                    //---- check if the tracks match -------------------------
//                    if (trkRef == (*i_trk)) {
//                       if (ivtx == 0) {
//                          sumTrkPtBeta += (*i_trk)->pt();
//                       }
//                       else {
//                          sumTrkPtBetaStar += (*i_trk)->pt();
//                       }   
//                       break;
//                    }
//                 }
//              } 
//           }
//        }
//        if (sumTrkPt > 0) {
//           beta_     = sumTrkPtBeta/sumTrkPt;
//           betaStar_ = sumTrkPtBetaStar/sumTrkPt;
//        }
//        //qcdpfjet.setBeta(beta);
//        //qcdpfjet.setBetaStar(betaStar);
//     }

     JRAEvt_->refrank ->push_back(JRAEvt_->nref);
     JRAEvt_->refe    ->push_back(ref->energy());
     JRAEvt_->refpt   ->push_back(ref->pt());
     JRAEvt_->refeta  ->push_back(ref->eta());
     JRAEvt_->refphi  ->push_back(ref->phi());
     JRAEvt_->refy    ->push_back(ref->rapidity());
     JRAEvt_->refarea ->push_back(ref.castTo<reco::GenJetRef>()->jetArea());
     JRAEvt_->jte     ->push_back(jet->energy());
     JRAEvt_->jtpt    ->push_back(jet->pt());
     JRAEvt_->jteta   ->push_back(jet->eta());
     JRAEvt_->jtphi   ->push_back(jet->phi());
     JRAEvt_->jty     ->push_back(jet->rapidity());
     JRAEvt_->jtjec   ->push_back(1.0);
     JRAEvt_->jtarea  ->push_back(0.0);

     if (isCaloJet_) {
        JRAEvt_->jtarea->at(JRAEvt_->nref) = jet.castTo<reco::CaloJetRef>()->jetArea();
     }
     else if (isJPTJet_) {
        const reco::JPTJet& jptjet = dynamic_cast <const reco::JPTJet&> (*jet);
        edm::RefToBase<reco::Jet> jptjetRef = jptjet.getCaloJetRef();
        reco::CaloJet const * rawcalojet = dynamic_cast<reco::CaloJet const *>( &* jptjetRef);
        JRAEvt_->jtarea->at(JRAEvt_->nref) = rawcalojet->jetArea();
     }
     else if (isPFJet_) {
        JRAEvt_->jtarea->at(JRAEvt_->nref) = jet.castTo<reco::PFJetRef>()->jetArea();
     }

     if (0!=jetCorrector_) {
        if (!jetCorrector_->vectorialCorrection()) {
           if (jetCorrector_->eventRequired()||isJPTJet_) {
              if (isCaloJet_) {
                 reco::CaloJetRef caloJetRef;
                 caloJetRef = jet.castTo<reco::CaloJetRef>();
                 JRAEvt_->jtjec->at(JRAEvt_->nref) = jetCorrector_->correction(*caloJetRef,iEvent,iSetup);
              }
              else if (isJPTJet_) {
                 reco::JPTJetRef jptJetRef;
                 jptJetRef = jet.castTo<reco::JPTJetRef>();
                 JRAEvt_->jtjec->at(JRAEvt_->nref) = jetCorrector_->correction(*jptJetRef,iEvent,iSetup);
              }
              else if (isPFJet_) {
                 reco::CandViewMatchMap::const_iterator jetMatch=jetToUncorJetMap->find(jet);
                 if (jetMatch!=jetToUncorJetMap->end()) {
                    reco::CandidateBaseRef ujet = jetMatch->val;
                    reco::PFJetRef pfJetRef;
                    pfJetRef=ujet.castTo<reco::PFJetRef>();
                    JRAEvt_->jtjec->at(JRAEvt_->nref) = jetCorrector_->correction(*pfJetRef,iEvent,iSetup);
                 }
              }
           }
           else {
              JRAEvt_->jtjec->at(JRAEvt_->nref) = jetCorrector_->correction(jet->p4());
           }
        }
     }
     
     if (doComposition_) {
        
        if (isCaloJet_) {
           reco::CaloJetRef caloJetRef;
           caloJetRef=jet.castTo<reco::CaloJetRef>();
           JRAEvt_->jtemf->push_back(caloJetRef->emEnergyFraction());
        }
        
        else if (isPFJet_) {
           reco::PFJetRef pfJetRef;
           pfJetRef=jet.castTo<reco::PFJetRef>();
           JRAEvt_->jtchf ->push_back(pfJetRef->chargedHadronEnergyFraction()*JRAEvt_->jtjec->at(JRAEvt_->nref));
           JRAEvt_->jtnhf ->push_back(pfJetRef->neutralHadronEnergyFraction()*JRAEvt_->jtjec->at(JRAEvt_->nref));
           JRAEvt_->jtnef ->push_back(pfJetRef->photonEnergyFraction()       *JRAEvt_->jtjec->at(JRAEvt_->nref));
           JRAEvt_->jtcef ->push_back(pfJetRef->electronEnergyFraction()     *JRAEvt_->jtjec->at(JRAEvt_->nref));
           JRAEvt_->jtmuf ->push_back(pfJetRef->muonEnergyFraction()         *JRAEvt_->jtjec->at(JRAEvt_->nref));
           JRAEvt_->jthfhf->push_back(pfJetRef->HFHadronEnergyFraction()     *JRAEvt_->jtjec->at(JRAEvt_->nref));
           JRAEvt_->jthfef->push_back(pfJetRef->HFEMEnergyFraction()         *JRAEvt_->jtjec->at(JRAEvt_->nref));
        } 
     }
     
     JRAEvt_->nref++;
  }
     
  // PFCANDIDATE INFORMATION
  //Dual handle idea from https://github.com/aperloff/cmssw/blob/CMSSW_7_6_X/RecoJets/JetProducers/plugins/VirtualJetProducer.cc
  //Random-Cone algo from https://github.com/cihar29/OffsetAnalysis/blob/master/run_offset.py
  //                  and https://github.com/cihar29/OffsetAnalysis/blob/master/plugins/OffsetAnalysis.cc
  if (saveCandidates_ && isPFJet_) {
      bool isView = iEvent.getByToken(srcPFCandidates_, pfCandidates);
      if ( isView ) {
          for (auto i_pf=pfCandidates->begin(); i_pf != pfCandidates->end(); ++i_pf) {
              auto i_pfc = (i_pf);
              JRAEvent::Flavor pf_id = getFlavor( i_pfc->particleId() );
              if (pf_id == JRAEvent::X) continue;
              JRAEvt_->pfcand_px ->push_back(i_pfc->px());
              JRAEvt_->pfcand_py ->push_back(i_pfc->py());
              JRAEvt_->pfcand_pt ->push_back(i_pfc->pt());
              JRAEvt_->pfcand_eta->push_back(i_pfc->eta());
              JRAEvt_->pfcand_phi->push_back(i_pfc->phi());
              JRAEvt_->pfcand_e  ->push_back(i_pfc->energy());
              JRAEvt_->pfcand_id ->push_back(pf_id);
          }
      }
      else {
          bool isPF = iEvent.getByToken(srcPFCandidatesAsFwdPtr_, pfCandidatesAsFwdPtr);
          if ( isPF ) {
              for (auto i_pf=pfCandidatesAsFwdPtr->begin(); i_pf != pfCandidatesAsFwdPtr->end(); ++i_pf) {
                  auto i_pfc = (*i_pf);
                  JRAEvent::Flavor pf_id = getFlavor( i_pfc->particleId() );
                  if (pf_id == JRAEvent::X) continue;
                  JRAEvt_->pfcand_px ->push_back(i_pfc->px());
                  JRAEvt_->pfcand_py ->push_back(i_pfc->py());
                  JRAEvt_->pfcand_pt ->push_back(i_pfc->pt());
                  JRAEvt_->pfcand_eta->push_back(i_pfc->eta());
                  JRAEvt_->pfcand_phi->push_back(i_pfc->phi());
                  JRAEvt_->pfcand_e  ->push_back(i_pfc->energy());
                  JRAEvt_->pfcand_id ->push_back(pf_id);
              }
          }
      }
  }

  
  tree_->Fill();
  
  return;
  }

//______________________________________________________________________________
JRAEvent::Flavor JetResponseAnalyzer::getFlavor(reco::PFCandidate::ParticleType id) {
    if (id == reco::PFCandidate::h)
        return JRAEvent::h;
    else if (id == reco::PFCandidate::e)
        return JRAEvent::e;
    else if (id == reco::PFCandidate::mu)
        return JRAEvent::mu;
    else if (id == reco::PFCandidate::gamma)
        return JRAEvent::gamma;
    else if (id == reco::PFCandidate::h0)
        return JRAEvent::h0;
    else if (id == reco::PFCandidate::h_HF)
        return JRAEvent::h_HF;
    else if (id == reco::PFCandidate::egamma_HF)
        return JRAEvent::egamma_HF;
    else
        return JRAEvent::X;
}

//______________________________________________________________________________
int JetResponseAnalyzer::getBin(double x, const double boundaries[], int length)
{
   int i;
   int n = length;
   if (n<=0) return -1;
   if (x<boundaries[0] || x>=boundaries[n])
      return -1;
   for(i=0;i<n;i++)
   {
      if (x>=boundaries[i] && x<boundaries[i+1])
         return i;
   }
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
// define JetEfficiencyAnalyzer as a plugin
////////////////////////////////////////////////////////////////////////////////

DEFINE_FWK_MODULE(JetResponseAnalyzer);

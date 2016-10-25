////////////////////////////////////////////////////////////////////////////////
//
// JetResponseAnalyzerProducer
// ---------------------------
//
//                    01/28/2013 Alexx Perloff       <aperloff@physics.tamu.edu>
////////////////////////////////////////////////////////////////////////////////

#include "JetMETAnalysis/JetAnalyzers/interface/JetResponseAnalyzerProducer.hh"

////////////////////////////////////////////////////////////////////////////////
// construction/destruction
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
JetResponseAnalyzerProducer::JetResponseAnalyzerProducer(const edm::ParameterSet& iConfig)
  : moduleLabel_   (iConfig.getParameter<std::string>            ("@module_label"))
  , srcRef_        (iConfig.getParameter<edm::InputTag>                 ("srcRef"))
  , srcRefToJetMap_(iConfig.getParameter<edm::InputTag>         ("srcRefToJetMap"))
  , srcRho_        (iConfig.getParameter<edm::InputTag>                 ("srcRho"))
  , srcRhoHLT_     (iConfig.getParameter<edm::InputTag>              ("srcRhoHLT"))
  , srcVtx_        (iConfig.getParameter<edm::InputTag>                 ("srcVtx"))
  , jecLabel_      (iConfig.getParameter<std::string>                 ("jecLabel"))
  , doComposition_ (iConfig.getParameter<bool>                   ("doComposition"))
  , doFlavor_      (iConfig.getParameter<bool>                        ("doFlavor"))
  , doJetPt_       (iConfig.getParameter<bool>                         ("doJetPt"))
  , doRefPt_       (iConfig.getParameter<bool>                         ("doRefPt"))
  , doHLT_         (iConfig.getParameter<bool>                           ("doHLT"))
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
    srcRefToPartonMap_=iConfig.getParameter<edm::InputTag>("srcRefToPartonMap");
    deltaRPartonMax_  =iConfig.getParameter<double>       ("deltaRPartonMax");
    getFlavorFromMap_=true;
  }
  
  isCaloJet_  = (moduleLabel_.find("calo")!=string::npos);
  isJPTJet_   = (moduleLabel_.find("jpt") !=string::npos);
  isPFJet_    = (moduleLabel_.find("pf")  !=string::npos);
  isTrackJet_ = (moduleLabel_.find("trk") !=string::npos);
  isTauJet_   = (moduleLabel_.find("tau") !=string::npos);

  int check = isCaloJet_+isJPTJet_+isPFJet_+isTrackJet_+isTauJet_;
  assert(check<2);

  //if (isCaloJet_)  cout<<"These are CaloJets  ("<<moduleLabel_<<")"<<endl;
  //if (isJPTJet_)   cout<<"These are JPTJets   ("<<moduleLabel_<<")"<<endl;
  //if (isPFJet_)    cout<<"These are PFJets    ("<<moduleLabel_<<")"<<endl;
  //if (isTrackJet_) cout<<"These are TrackJets ("<<moduleLabel_<<")"<<endl;
  //if (isTauJet_)   cout<<"These are TauJets   ("<<moduleLabel_<<")"<<endl;

  //
  // Produces statements
  // Branches to be put into the PATTuple
  //
  produces<JRAEvent> ("JRAEvent").setBranchAlias("JRAEvent");
}


//______________________________________________________________________________
JetResponseAnalyzerProducer::~JetResponseAnalyzerProducer()
{

}


////////////////////////////////////////////////////////////////////////////////
// implementation of member functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void JetResponseAnalyzerProducer::beginJob()
{
}


//______________________________________________________________________________
void JetResponseAnalyzerProducer::beginEvent()
{
   JRAEvt_ = unique_ptr<JRAEvent> (new JRAEvent);
   JRAEvt_->clear();
}


//______________________________________________________________________________
void JetResponseAnalyzerProducer::produce(edm::Event& iEvent,
                                  const edm::EventSetup& iSetup)
{
   beginEvent();

  // EVENT DATA HANDLES
  (JRAEvt_->nref) = 0;
  edm::Handle<GenEventInfoProduct>               genInfo;
  edm::Handle<vector<PileupSummaryInfo> >        puInfos;  
  edm::Handle<reco::CandidateView>               refs;
  edm::Handle<reco::CandViewMatchMap>            refToJetMap;
  edm::Handle<reco::JetMatchedPartonsCollection> refToPartonMap;
  edm::Handle<double>                            rho;
  edm::Handle<double>                            rho_hlt;
  edm::Handle<reco::VertexCollection>            vtx;

  // JET CORRECTOR
  jetCorrector_ = (jecLabel_.empty()) ? 0 : JetCorrector::getJetCorrector(jecLabel_,iSetup);
  
  // GENERATOR INFORMATION
  JRAEvt_->pthat  = 0.0;
  JRAEvt_->weight = 1.0;
  if (iEvent.getByLabel("generator",genInfo)) {
    if (genInfo->hasBinningValues()) JRAEvt_->pthat = (Float_t)genInfo->binningValues()[0];
    JRAEvt_->weight = (Float_t)genInfo->weight();
  }
  
  //RHO INFORMATION
  JRAEvt_->rho = 0.0;
  if (iEvent.getByLabel(srcRho_,rho)) {
    JRAEvt_->rho = *rho;
  }

  //HLT RHO INFORMATION
  JRAEvt_->rho_hlt = 0.0;
  if (doHLT_) {
     if (iEvent.getByLabel(srcRhoHLT_,rho_hlt)) {
       JRAEvt_->rho_hlt = *rho_hlt;
     }
  }
 
  //NPV INFORMATION
  JRAEvt_->npv = 0;
  if (iEvent.getByLabel(srcVtx_,vtx)) {
     const reco::VertexCollection::const_iterator vtxEnd = vtx->end();
     for (reco::VertexCollection::const_iterator vtxIter = vtx->begin(); vtxEnd != vtxIter; ++vtxIter) {
        if (!vtxIter->isFake() && vtxIter->ndof()>=4 && fabs(vtxIter->z())<=24)
           ++(JRAEvt_->npv);
     }
     //npv_ = vtx->size();
  }
 
  //EVENT INFORMATION
  JRAEvt_->run = iEvent.id().run();
  JRAEvt_->lumi = iEvent.id().luminosityBlock();
  JRAEvt_->evt = iEvent.id().event();

  // MC PILEUP INFORMATION
  if (iEvent.getByLabel("addPileupInfo",puInfos)) {
     for(unsigned int i=0; i<puInfos->size(); i++) {
        JRAEvt_->npus->push_back((*puInfos)[i].getPU_NumInteractions());
        JRAEvt_->tnpus->push_back((*puInfos)[i].getTrueNumInteractions());
        JRAEvt_->bxns->push_back((*puInfos)[i].getBunchCrossing());
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
  iEvent.getByLabel(srcRef_,        refs);
  iEvent.getByLabel(srcRefToJetMap_,refToJetMap);
  if (getFlavorFromMap_) iEvent.getByLabel(srcRefToPartonMap_,refToPartonMap);
  if (doBalancing_&&refToJetMap->size()!=1) return;
  size_t nRef=(nRefMax_==0) ? refs->size() : std::min(nRefMax_,refs->size());
  for (size_t iRef=0;iRef<nRef;iRef++) {

     reco::CandidateBaseRef ref=refs->refAt(iRef);
     
     reco::CandViewMatchMap::const_iterator itMatch=refToJetMap->find(ref);
     if (itMatch==refToJetMap->end()) continue;
     reco::CandidateBaseRef jet=itMatch->val;
     
     JRAEvt_->refdrjt->push_back(reco::deltaR(jet->eta(),jet->phi(),ref->eta(),ref->phi()));
     JRAEvt_->refdphijt->push_back(reco::deltaPhi(jet->phi(),ref->phi()));
     
     if ((!doBalancing_&&JRAEvt_->refdrjt->at(JRAEvt_->nref)>deltaRMax_)||
         (doBalancing_&&std::abs(JRAEvt_->refdphijt->at(JRAEvt_->nref))<deltaPhiMin_)) continue;

     JRAEvt_->refpdgid->push_back(0);
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
            itPartonMatch->second.algoDefinitionParton().get()!=0) {
           
           double refdrparton=
              reco::deltaR(ref->p4(),
                           itPartonMatch->second.algoDefinitionParton().get()->p4());
           
           if (refdrparton<deltaRPartonMax_) {
              JRAEvt_->refpdgid->at(JRAEvt_->nref)=itPartonMatch->second.algoDefinitionParton().get()->pdgId();
              int absid = std::abs(JRAEvt_->refpdgid->at(JRAEvt_->nref));
              if (absid==4||absid==5) {
                 GenJetLeptonFinder finder(*ref);
                 finder.run();
                 if (finder.foundLeptonAndNeutrino()) {
                    int sign  = (JRAEvt_->refpdgid->at(JRAEvt_->nref)>0) ? +1 : -1;
                    JRAEvt_->refpdgid->at(JRAEvt_->nref) = sign*(absid*100+std::abs(finder.leptonPdgId()));
                 }
              }
           }
        }
     }
     else {
        JRAEvt_->refpdgid->at(JRAEvt_->nref)=ref->pdgId();
     }

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
        //jtarea_[*nref_] =jet.castTo<reco::JPTJetRef>()->jetArea(); //Doesn't work. Returns 0 instead of the jet area.
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
                 caloJetRef=jet.castTo<reco::CaloJetRef>();
                 JRAEvt_->jtjec->at(JRAEvt_->nref) = jetCorrector_->correction(*caloJetRef,iEvent,iSetup);
              }
              else if (isJPTJet_) {
                 reco::JPTJetRef jptJetRef;
                 jptJetRef=jet.castTo<reco::JPTJetRef>();
                 JRAEvt_->jtjec->at(JRAEvt_->nref) = jetCorrector_->correction(*jptJetRef,iEvent,iSetup);
              }
              else if (isPFJet_) {
                 reco::PFJetRef pfJetRef;
                 pfJetRef=jet.castTo<reco::PFJetRef>();
                 JRAEvt_->jtjec->at(JRAEvt_->nref) = jetCorrector_->correction(*pfJetRef,iEvent,iSetup);
              }
           }
           else {
              JRAEvt_->jtjec->at(JRAEvt_->nref) = jetCorrector_->correction(jet->p4());
           }
        }
     }

     if (doComposition_) {

        if (isCaloJet_) {
           JRAEvt_->jtemf->push_back(0.0);
           reco::CaloJetRef caloJetRef;
           caloJetRef=jet.castTo<reco::CaloJetRef>();
           JRAEvt_->jtemf->at(JRAEvt_->nref) = caloJetRef->emEnergyFraction();
        }
        
        else if (isPFJet_) {
           JRAEvt_->jtchf->push_back(0.0);
           JRAEvt_->jtnhf->push_back(0.0);
           JRAEvt_->jtnef->push_back(0.0);
           JRAEvt_->jtcef->push_back(0.0);
           JRAEvt_->jtmuf->push_back(0.0);
           JRAEvt_->jthfhf->push_back(0.0);
           JRAEvt_->jthfef->push_back(0.0);
           reco::PFJetRef pfJetRef;
           pfJetRef=jet.castTo<reco::PFJetRef>();
           JRAEvt_->jtchf->at(JRAEvt_->nref)  = pfJetRef->chargedHadronEnergyFraction()*JRAEvt_->jtjec->at(JRAEvt_->nref);
           JRAEvt_->jtnhf->at(JRAEvt_->nref)  = pfJetRef->neutralHadronEnergyFraction()*JRAEvt_->jtjec->at(JRAEvt_->nref);
           JRAEvt_->jtnef->at(JRAEvt_->nref)  = pfJetRef->photonEnergyFraction()       *JRAEvt_->jtjec->at(JRAEvt_->nref);
           JRAEvt_->jtcef->at(JRAEvt_->nref)  = pfJetRef->electronEnergyFraction()     *JRAEvt_->jtjec->at(JRAEvt_->nref);
           JRAEvt_->jtmuf->at(JRAEvt_->nref)  = pfJetRef->muonEnergyFraction()         *JRAEvt_->jtjec->at(JRAEvt_->nref);
           JRAEvt_->jthfhf->at(JRAEvt_->nref) = pfJetRef->HFHadronEnergyFraction()     *JRAEvt_->jtjec->at(JRAEvt_->nref);
           JRAEvt_->jthfef->at(JRAEvt_->nref) = pfJetRef->HFEMEnergyFraction()         *JRAEvt_->jtjec->at(JRAEvt_->nref);
        }
        
    }

     (JRAEvt_->nref)++;
  }

  iEvent.put(std::move(JRAEvt_),"JRAEvent");

  endEvent();

  return;
}


//______________________________________________________________________________
void JetResponseAnalyzerProducer::endEvent()
{
   JRAEvt_.reset();
}


////////////////////////////////////////////////////////////////////////////////
// define JetEfficiencyAnalyzer as a plugin
////////////////////////////////////////////////////////////////////////////////

DEFINE_FWK_MODULE(JetResponseAnalyzerProducer);

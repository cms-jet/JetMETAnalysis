////////////////////////////////////////////////////////////////////////////////
//
// JetResponseAnalyzer
// -------------------
//
//            07/04/2008 Kostas Kousouris       <kkousour@fnal.gov>
//                       Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/GenJetLeptonFinder.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Candidate/interface/CandMatchMap.h"
#include "DataFormats/JetReco/interface/JPTJet.h"
#include "DataFormats/JetReco/interface/CaloJet.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "JetMETCorrections/Objects/interface/JetCorrector.h"

#include "SimDataFormats/JetMatching/interface/JetMatchedPartons.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include <TH1F.h>
#include <TH2F.h>
#include <TTree.h>

#include <memory>
#include <vector>
#include <sstream>
#include <cmath>


using namespace std;


////////////////////////////////////////////////////////////////////////////////
// class definition
////////////////////////////////////////////////////////////////////////////////

//class JetResponseAnalyzer : public edm::EDAnalyzer
class JetResponseAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources>
{
	public:
		// construction/destruction
		explicit JetResponseAnalyzer(const edm::ParameterSet& iConfig);
		virtual ~JetResponseAnalyzer();

	private:
		// member functions
		void beginJob();
		void analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup);
		void endJob(){;}

	private:
		// member data
		std::string   moduleLabel_;

		edm::EDGetTokenT<reco::CandidateView> srcRef_;
		edm::EDGetTokenT<reco::CandViewMatchMap> srcJetToUncorJetMap_;
		edm::EDGetTokenT<reco::CandViewMatchMap> srcRefToJetMap_;
		edm::EDGetTokenT<reco::CandViewMatchMap> srcJetUnMatch_; //////////UnMatched Reco Jet!!!
		edm::EDGetTokenT<reco::JetMatchedPartonsCollection> srcRefToPartonMap_;
		edm::EDGetTokenT<vector<double>> srcRhos_;
		edm::EDGetTokenT<double> srcRho_;
		edm::EDGetTokenT<double> srcRhoHLT_;
		edm::EDGetTokenT<reco::VertexCollection> srcVtx_;
    edm::EDGetTokenT<GenEventInfoProduct> srcGenInfo_;
    edm::EDGetTokenT<vector<PileupSummaryInfo> > srcPileupInfo_;

		std::string   jecLabel_;

		bool          doComposition_;
		bool          doFlavor_;
		bool          doJetPt_;
		bool          doRefPt_;
		bool          doHLT_;
		unsigned int  nRefMax_;

		double        deltaRMax_;
		double        deltaPhiMin_;
		double        deltaRPartonMax_;

		bool          doBalancing_;
		bool          getFlavorFromMap_;
		bool          isCaloJet_;
		bool          isJPTJet_;
		bool          isPFJet_;
		bool          isTrackJet_;
		bool          isTauJet_;

		const JetCorrector* jetCorrector_;

		// tree & branches
		TTree*        tree_;

		vector<Int_t> npus_;
		vector<Float_t> tnpus_;
		vector<Float_t> zpositions_;
		vector<Int_t> bxns_;
		vector<Float_t> sumpt_lowpt_;
		vector<Float_t> sumpt_highpt_;
		vector<Int_t> ntrks_lowpt_;
		vector<Int_t> ntrks_highpt_;
		vector<Float_t> rhos_;
		Float_t       rho_;
		Float_t       rho_hlt_;
		Float_t       pthat_;
		Float_t       beta_;
		Float_t       betaStar_;
		Float_t       weight_;
		Long64_t      npv_;
		Long64_t      run_;
		Long64_t      lumi_;
		Long64_t      evt_;

		UChar_t       nref_;
		UChar_t       refrank_[100];
		Int_t         refpdgid_[100];
		Int_t         refpdgid_algorithmicDef_[100];
		Int_t         refpdgid_physicsDef_[100];
		Float_t       refe_[100];
		Float_t       refpt_[100];
		Float_t       refeta_[100];
		Float_t       refphi_[100];
		Float_t       refy_[100];
		Float_t       refdrjt_[100];
		Float_t       refdphijt_[100];
		Float_t       refarea_[100];

		Float_t       jte_[100];
		Float_t       jtpt_[100];
		Float_t       jteta_[100];
		Float_t       jtphi_[100];
		Float_t       jty_[100];
		Float_t       jtjec_[100];
		Float_t       jtarea_[100];

		Float_t       jtemf_[100];

		Float_t       jtchf_[100];
		Float_t       jtnhf_[100];
		Float_t       jtnef_[100];
		Float_t       jtcef_[100];
		Float_t       jtmuf_[100];
		Float_t       jthfhf_[100];
		Float_t       jthfef_[100];

		Float_t       refdzvtx_[100];
		//////////////////Branch for un-matched reco jets
		Long64_t      nUnMatchJet_;
		Float_t       unmapjteta_[100];
		Float_t       unmapjtpt_[100];

};


////////////////////////////////////////////////////////////////////////////////
// construction/destruction
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
JetResponseAnalyzer::JetResponseAnalyzer(const edm::ParameterSet& iConfig)
	: moduleLabel_        (iConfig.getParameter<std::string>            ("@module_label"))
	, srcRef_             (consumes<reco::CandidateView>(iConfig.getParameter<edm::InputTag>                ("srcRef")))
	, srcJetToUncorJetMap_(consumes<reco::CandViewMatchMap>(iConfig.getParameter<edm::InputTag>("srcJetToUncorJetMap")))
	, srcRefToJetMap_     (consumes<reco::CandViewMatchMap>(iConfig.getParameter<edm::InputTag>     ("srcRefToJetMap")))
	, srcJetUnMatch_      (consumes<reco::CandViewMatchMap>(iConfig.getParameter<edm::InputTag>      ("srcJetUnMatch")))
	, srcRhos_            (consumes<vector<double>>(iConfig.getParameter<edm::InputTag>                    ("srcRhos")))
	, srcRho_             (consumes<double>(iConfig.getParameter<edm::InputTag>                             ("srcRho")))
	, srcRhoHLT_          (consumes<double>(iConfig.getParameter<edm::InputTag>                          ("srcRhoHLT")))
	, srcVtx_             (consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>             ("srcVtx")))
  , srcGenInfo_         (consumes<GenEventInfoProduct>(edm::InputTag("generator"))                                   )
  , srcPileupInfo_      (consumes<vector<PileupSummaryInfo> >(edm::InputTag("addPileupInfo"))                        )
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
		srcRefToPartonMap_=consumes<reco::JetMatchedPartonsCollection>(iConfig.getParameter<edm::InputTag>("srcRefToPartonMap"));
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
  //must state that we are using the TFileService
  usesResource("TFileService");
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
{
	edm::Service<TFileService> fs;
	if (!fs) throw edm::Exception(edm::errors::Configuration,
			"TFileService missing from configuration!");

	tree_=fs->make<TTree>("t","t");

	tree_->Branch("npus", "vector<Int_t>",  &npus_);
	tree_->Branch("tnpus", "vector<Float_t>",  &tnpus_);
	tree_->Branch("zpositions", "vector<Float_t>",  &zpositions_);
	tree_->Branch("bxns", "vector<Int_t>", &bxns_);
	tree_->Branch("sumpt_lowpt", "vector<Float_t>", &sumpt_lowpt_);
	tree_->Branch("sumpt_highpt", "vector<Float_t>", &sumpt_highpt_);
	tree_->Branch("ntrks_lowpt", "vector<Int_t>", &ntrks_lowpt_);
	tree_->Branch("ntrks_highpt", "vector<Int_t>", &ntrks_highpt_);
	tree_->Branch("rhos", "vector<Float_t>", &rhos_);
	tree_->Branch("rho", &rho_, "rho/F");
	if (doHLT_) tree_->Branch("rho_hlt",&rho_hlt_, "rho_hlt/F");
	tree_->Branch("pthat", &pthat_,  "pthat/F");
	tree_->Branch("beta", &beta_,  "beta/F");
	tree_->Branch("betaStar", &betaStar_,  "betaStar/F");
	tree_->Branch("weight",&weight_, "weight/F");
	tree_->Branch("npv",&npv_, "npv/L");
	tree_->Branch("run",&run_, "run/L");
	tree_->Branch("lumi",&lumi_, "lumi/L");
	tree_->Branch("evt",&evt_, "evt/L");

	tree_->Branch("nref",  &nref_,   "nref/b");
	tree_->Branch("refrank",refrank_,"refrank[nref]/b");
	if (doFlavor_) {
		tree_->Branch("refpdgid",refpdgid_,"refpdgid[nref]/I");
		tree_->Branch("refpdgid_algorithmicDef",refpdgid_algorithmicDef_,"refpdgid_algorithmicDef[nref]/I");
		tree_->Branch("refpdgid_physicsDef",refpdgid_physicsDef_,"refpdgid_physicsDef[nref]/I");
	}
	tree_->Branch("refe",   refe_,   "refe[nref]/F");
	tree_->Branch("refpt",  refpt_,  "refpt[nref]/F");
	tree_->Branch("refeta", refeta_, "refeta[nref]/F");
	tree_->Branch("refphi", refphi_, "refphi[nref]/F");
	tree_->Branch("refy",   refy_,   "refy[nref]/F");
	if (doBalancing_) tree_->Branch("refdphijt",refdphijt_,"refdphijt[nref]/F");
	else              tree_->Branch("refdrjt",  refdrjt_,  "refdrjt[nref]/F");
	tree_->Branch("refarea", refarea_, "refarea[nref]/F");

	tree_->Branch("jte",    jte_,    "jte[nref]/F");
	tree_->Branch("jtpt",   jtpt_,   "jtpt[nref]/F");
	tree_->Branch("jteta",  jteta_,  "jteta[nref]/F");
	tree_->Branch("jtphi",  jtphi_,  "jtphi[nref]/F");
	tree_->Branch("jty",    jty_,    "jty[nref]/F");
	tree_->Branch("jtjec",  jtjec_,  "jtjec[nref]/F");
	tree_->Branch("jtarea", jtarea_, "jtarea[nref]/F");
	///////////////Unmatched Jet config!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	tree_->Branch("nUnMatchJet",  &nUnMatchJet_,   "nUnMatchJet/L");
	tree_->Branch("unmapjtpt",   unmapjtpt_,   "unmapjtpt[nUnMatchJet]/F");
	tree_->Branch("unmapjteta",  unmapjteta_,  "unmapjteta[nUnMatchJet]/F");
	/////////////////////////////////////////////////////////////////////////

	if (doComposition_) {

		if (isCaloJet_) {
			tree_->Branch("jtemf", jtemf_, "jtemf[nref]/F");
		}

		else if (isPFJet_) {
			tree_->Branch("jtchf",  jtchf_,  "jtchf[nref]/F");
			tree_->Branch("jtnhf",  jtnhf_,  "jtnhf[nref]/F");
			tree_->Branch("jtnef",  jtnef_,  "jtnef[nref]/F");
			tree_->Branch("jtcef",  jtcef_,  "jtcef[nref]/F");
			tree_->Branch("jtmuf",  jtmuf_,  "jtmuf[nref]/F");
			tree_->Branch("jthfhf", jthfhf_, "jthfhf[nref]/F");
			tree_->Branch("jthfef", jthfef_, "jthfef[nref]/F");
		}
	}

	tree_->Branch("refdzvtx",  refdzvtx_,  "refdzvtx[nref]/F");

}


//______________________________________________________________________________
void JetResponseAnalyzer::analyze(const edm::Event& iEvent,
		const edm::EventSetup& iSetup)
{
	// EVENT DATA HANDLES
	nref_=0;
	edm::Handle<GenEventInfoProduct>               genInfo;
	edm::Handle<vector<PileupSummaryInfo> >        puInfos;  
	edm::Handle<reco::CandidateView>               refs;
	edm::Handle<reco::CandViewMatchMap>            jetToUncorJetMap;
	edm::Handle<reco::CandViewMatchMap>            refToJetMap;
	edm::Handle<reco::CandViewMatchMap>            jetUnMatch;
	edm::Handle<reco::JetMatchedPartonsCollection> refToPartonMap;
	edm::Handle<vector<double> >                   rhos;
	edm::Handle<double>                            rho;
	edm::Handle<double>                            rho_hlt;
	edm::Handle<reco::VertexCollection>            vtx;

	// Jet CORRECTOR
	//std::cout<<" Before JetCorrector defined, jecLabel_ = "<<jecLabel_<<std::endl;
	jetCorrector_ = (jecLabel_.empty()) ? 0 : JetCorrector::getJetCorrector(jecLabel_,iSetup);
	//std::cout<<" After JetCorrector defined "<<std::endl;  

	// GENERATOR INFORMATION
	pthat_  = 0.0;
	weight_ = 1.0;
	if (iEvent.getByToken(srcGenInfo_,genInfo)) {
		if (genInfo->hasBinningValues()) pthat_ = (Float_t)genInfo->binningValues()[0];
		weight_ = (Float_t)genInfo->weight();
	}

	//RHO INFORMATION
	rho_ = 0.0;
	if (iEvent.getByToken(srcRho_,rho)) {
		rho_ = *rho;
	}

	//HLT RHO INFORMATION
	rho_hlt_ = 0.0;
	if (doHLT_) {
		if (iEvent.getByToken(srcRhoHLT_,rho_hlt)) {
			rho_hlt_ = *rho_hlt;
		}
	}

	//ETA DEPENDENT RHO INFORMATION
	rhos_.clear();
	if(iEvent.getByToken(srcRhos_,rhos)) {
		for(unsigned int i=0; i<rhos->size(); i++) {
			rhos_.push_back((*rhos)[i]);
		}
	}

	//refdrjt_[nref_]  =reco::deltaR(jet->eta(),jet->phi(),ref->eta(),ref->phi());
	//if (abs(muIter->innerTrack()->dz(vtxHandle->at(0).position()))<muPrim_dzMax && muIter->dB()<muPrim_dBMax) 
	//NPV INFORMATION
	npv_ = 0;

	if (iEvent.getByToken(srcVtx_,vtx)) {
		const reco::VertexCollection::const_iterator vtxEnd = vtx->end();
		for (reco::VertexCollection::const_iterator vtxIter = vtx->begin(); vtxEnd != vtxIter; ++vtxIter) {
			if (!vtxIter->isFake() && vtxIter->ndof()>=1 && fabs(vtxIter->z())<=24) {
				++npv_;
				refdzvtx_[nref_] = 0;//fabs(vtxIter->z()-);
			}
		}
		//npv_ = vtx->size();
	}

	//EVENT INFORMATION
	run_ = iEvent.id().run();
	lumi_ = iEvent.id().luminosityBlock();
	evt_ = iEvent.id().event();

	// MC PILEUP INFORMATION
	npus_.clear();
	tnpus_.clear();
	zpositions_.clear();
	bxns_.clear();
	sumpt_lowpt_.clear();
	sumpt_highpt_.clear();
	ntrks_lowpt_.clear();
	ntrks_highpt_.clear();
	if (iEvent.getByToken(srcPileupInfo_,puInfos)) {
		for(unsigned int i=0; i<puInfos->size(); i++) {
			npus_.push_back((*puInfos)[i].getPU_NumInteractions());
			tnpus_.push_back((*puInfos)[i].getTrueNumInteractions());
			//zpositions_.push_back((*puInfos)[i].getPU_zpositions());
			bxns_.push_back((*puInfos)[i].getBunchCrossing());
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
			sumpt_lowpt_.push_back(sumptlowpttemp);
			sumpt_highpt_.push_back(sumpthighpttemp);
			ntrks_lowpt_.push_back(ntrkslowpttemp);
			ntrks_highpt_.push_back(ntrkshighpttemp);
		}
	}

	// REFERENCES & RECOJETS
	iEvent.getByToken(srcRef_,               refs);
	iEvent.getByToken(srcJetToUncorJetMap_, jetToUncorJetMap); 
	iEvent.getByToken(srcRefToJetMap_,refToJetMap);
	iEvent.getByToken(srcJetUnMatch_, jetUnMatch); /////Unmatched reco jet config!!!
	if (getFlavorFromMap_) iEvent.getByToken(srcRefToPartonMap_,refToPartonMap);
	if (doBalancing_&&refToJetMap->size()!=1) return;
	size_t nRef=(nRefMax_==0) ? refs->size() : std::min(nRefMax_,refs->size());
	for (size_t iRef=0;iRef<nRef;iRef++) {

		reco::CandidateBaseRef ref=refs->refAt(iRef);

		reco::CandViewMatchMap::const_iterator itMatch=refToJetMap->find(ref);
		if (itMatch==refToJetMap->end()) continue;
		reco::CandidateBaseRef jet=itMatch->val;
		//std::cout<<" Analysis "<<jet->eta()<<" "<<jet->phi()<<" "<<jet->energy()<<" "<<jet->et()<<std::endl;     

		refdrjt_[nref_]  =reco::deltaR(jet->eta(),jet->phi(),ref->eta(),ref->phi());
		refdphijt_[nref_]=reco::deltaPhi(jet->phi(),ref->phi());

		if ((!doBalancing_&&refdrjt_[nref_]>deltaRMax_)||
				(doBalancing_&&std::abs(refdphijt_[nref_])<deltaPhiMin_)) continue;

		refpdgid_[nref_]=0;
		refpdgid_algorithmicDef_[nref_]=0;
		refpdgid_physicsDef_[nref_]=0;
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
					refpdgid_algorithmicDef_[nref_]=itPartonMatch->second.algoDefinitionParton().get()->pdgId();
					int absid = std::abs(refpdgid_algorithmicDef_[nref_]);
					if (absid==4||absid==5) {
						GenJetLeptonFinder finder(*ref);
						finder.run();
						if (finder.foundLeptonAndNeutrino()) {
							int sign  = (refpdgid_algorithmicDef_[nref_]>0) ? +1 : -1;
							refpdgid_algorithmicDef_[nref_] = sign*(absid*100+std::abs(finder.leptonPdgId()));
						}
					}
				}
				if (refdrparton_physics<deltaRPartonMax_) {
					refpdgid_physicsDef_[nref_]=itPartonMatch->second.physicsDefinitionParton().get()->pdgId();
					int absid = std::abs(refpdgid_physicsDef_[nref_]);
					if (absid==4||absid==5) {
						GenJetLeptonFinder finder(*ref);
						finder.run();
						if (finder.foundLeptonAndNeutrino()) {
							int sign  = (refpdgid_physicsDef_[nref_]>0) ? +1 : -1;
							refpdgid_physicsDef_[nref_] = sign*(absid*100+std::abs(finder.leptonPdgId()));
						}
					}
				}
			}
		}
		else {
			refpdgid_algorithmicDef_[nref_]=0;
			refpdgid_physicsDef_[nref_]=0;
		}
		refpdgid_[nref_]=ref->pdgId();

		/*     // Beta/Beta Star Calculation
					 beta_ = 0.0;
					 betaStar_ = 0.0;
					 if (isPFJet_) {
		//---- vertex association -----------
		//---- get the vector of tracks -----
		reco::PFJetRef pfJetRef = jet.castTo<reco::PFJetRef>();
		reco::TrackRefVector vTrks(pfJetRef->getTrackRefs());
		float sumTrkPt(0.0),sumTrkPtBeta(0.0),sumTrkPtBetaStar(0.0);
		//---- loop over the tracks of the jet ----
		for(reco::TrackRefVector::const_iterator i_trk = vTrks.begin(); i_trk != vTrks.end(); i_trk++) {
		//if (npv_ == 0) break;
		if ((*vtx).size() == 0) break;
		sumTrkPt += (*i_trk)->pt();
		//---- loop over all vertices ----------------------------
		for(unsigned ivtx = 0;ivtx < (*vtx).size();ivtx++) {
		//---- loop over the tracks associated with the vertex ---
		if (!((*vtx)[ivtx].isFake()) && (*vtx)[ivtx].ndof() >= 4 && fabs((*vtx)[ivtx].z()) <= 24) {
		for(reco::Vertex::trackRef_iterator i_vtxTrk = (*vtx)[ivtx].tracks_begin(); i_vtxTrk != (*vtx)[ivtx].tracks_end(); ++i_vtxTrk) {
		//---- match the jet track to the track from the vertex ----
		reco::TrackRef trkRef(i_vtxTrk->castTo<reco::TrackRef>());
		//---- check if the tracks match -------------------------
		if (trkRef == (*i_trk)) {
		if (ivtx == 0) {
		sumTrkPtBeta += (*i_trk)->pt();
		}
		else {
		sumTrkPtBetaStar += (*i_trk)->pt();
		}   
		break;
		}
		}
		} 
		}
		}
		if (sumTrkPt > 0) {
		beta_     = sumTrkPtBeta/sumTrkPt;
		betaStar_ = sumTrkPtBetaStar/sumTrkPt;
		}
		//qcdpfjet.setBeta(beta);
		//qcdpfjet.setBetaStar(betaStar);
		}*/

		refrank_[nref_]=nref_;
		refe_[nref_]   =ref->energy();
		refpt_[nref_]  =ref->pt();
		refeta_[nref_] =ref->eta();
		refphi_[nref_] =ref->phi();
		refy_[nref_]   =ref->rapidity();
		refarea_[nref_]=ref.castTo<reco::GenJetRef>()->jetArea();
		jte_[nref_]    =jet->energy();
		jtpt_[nref_]   =jet->pt();
		jteta_[nref_]  =jet->eta();
		jtphi_[nref_]  =jet->phi();
		jty_[nref_]    =jet->rapidity();
		jtjec_[nref_]  =1.0;

		if (isCaloJet_) {
			jtarea_[nref_] =jet.castTo<reco::CaloJetRef>()->jetArea();
		}
		else if (isJPTJet_) {
			//jtarea_[nref_] =jet.castTo<reco::JPTJetRef>()->jetArea(); //Doesn't work. Returns 0 instead of the jet area.
			const reco::JPTJet& jptjet = dynamic_cast <const reco::JPTJet&> (*jet);
			edm::RefToBase<reco::Jet> jptjetRef = jptjet.getCaloJetRef();
			reco::CaloJet const * rawcalojet = dynamic_cast<reco::CaloJet const *>( &* jptjetRef);
			jtarea_[nref_] = rawcalojet->jetArea();
		}
		else if (isPFJet_) {
			jtarea_[nref_] =jet.castTo<reco::PFJetRef>()->jetArea();
		}

		if (0!=jetCorrector_) {
			if (!jetCorrector_->vectorialCorrection()) {
				if (jetCorrector_->eventRequired()||isJPTJet_) {
					if (isCaloJet_) {
						reco::CaloJetRef caloJetRef;
						caloJetRef=jet.castTo<reco::CaloJetRef>();
						//jtjec_[nref_]=jetCorrector_->correction(*caloJetRef,
						//                                        edm::RefToBase<reco::Jet>(),
						//                                        iEvent,iSetup);
						jtjec_[nref_]=jetCorrector_->correction(*caloJetRef,
								iEvent,iSetup);
					}
					else if (isJPTJet_) {
						reco::JPTJetRef jptJetRef;
						jptJetRef=jet.castTo<reco::JPTJetRef>();
						//jtjec_[nref_]=jetCorrector_->correction(*jptJetRef,
						//                                        edm::RefToBase<reco::Jet>(),
						//                                        iEvent,iSetup);
						jtjec_[nref_]=jetCorrector_->correction(*jptJetRef,
								iEvent,iSetup);
					}
					else if (isPFJet_) {
						/*                 reco::CandViewMatchMap::const_iterator jetMatch=jetToUncorJetMap->find(jet);
															 if (jetMatch!=jetToUncorJetMap->end()) {
															 reco::CandidateBaseRef ujet=jetMatch->val;
															 reco::PFJetRef pfJetRef;
															 pfJetRef=ujet.castTo<reco::PFJetRef>();
						//std::cout<<" Do we start to correct "<<ujet->energy()<<" "<<ujet->eta()<<std::endl;
						jtjec_[nref_]=jetCorrector_->correction(*pfJetRef,
						iEvent,iSetup);
						//std::cout<<" Jet corrector end: after do we start to correct "<<std::endl;
						}*/

						reco::PFJetRef pfJetRef;
						pfJetRef=jet.castTo<reco::PFJetRef>();
						//jtjec_[nref_]=jetCorrector_->correction(*pfJetRef,
						//                                        edm::RefToBase<reco::Jet>(),
						//                                        iEvent,iSetup);
						//std::cout<<" Do we start to correct "<<jet->energy()<<" "<<jet->eta()<<std::endl;
						jtjec_[nref_]=jetCorrector_->correction(*pfJetRef,
								iEvent,iSetup);
						//std::cout<<" Jet corrector end: after do we start to correct "<<std::endl;

					}
				}
				else {
					jtjec_[nref_]=jetCorrector_->correction(jet->p4());
				}
			}
		}

		if (doComposition_) {

			if (isCaloJet_) {
				reco::CaloJetRef caloJetRef;
				caloJetRef=jet.castTo<reco::CaloJetRef>();
				jtemf_[nref_]=caloJetRef->emEnergyFraction();
			}

			else if (isPFJet_) {
				reco::PFJetRef pfJetRef;
				pfJetRef=jet.castTo<reco::PFJetRef>();
				jtchf_[nref_] =pfJetRef->chargedHadronEnergyFraction()*jtjec_[nref_];
				jtnhf_[nref_] =pfJetRef->neutralHadronEnergyFraction()*jtjec_[nref_];
				jtnef_[nref_] =pfJetRef->photonEnergyFraction()*jtjec_[nref_];
				jtcef_[nref_] =pfJetRef->electronEnergyFraction()*jtjec_[nref_];
				jtmuf_[nref_] =pfJetRef->muonEnergyFraction()*jtjec_[nref_];
				jthfhf_[nref_]=pfJetRef->HFHadronEnergyFraction()*jtjec_[nref_];
				jthfef_[nref_]=pfJetRef->HFEMEnergyFraction()*jtjec_[nref_];
			}

		}

		nref_++;
	}
	////////////Unmatched reco jet config!!!!!!!!!!!!!!
	reco::CandViewMatchMap::const_iterator itUnMatch;
	nUnMatchJet_ = 0;
	for(itUnMatch=jetUnMatch->begin();itUnMatch!=jetUnMatch->end();itUnMatch++)
	{
		reco::CandidateBaseRef unmatchrecojet = itUnMatch->val;
		unmapjteta_[nUnMatchJet_] = unmatchrecojet->eta();
		unmapjtpt_[nUnMatchJet_]  = unmatchrecojet->pt();
		nUnMatchJet_++;	
	}

	tree_->Fill();

	return;
}


////////////////////////////////////////////////////////////////////////////////
// define JetEfficiencyAnalyzer as a plugin
////////////////////////////////////////////////////////////////////////////////

DEFINE_FWK_MODULE(JetResponseAnalyzer);

////////////////////////////////////////////////////////////////////////////////
//
// JetResponseAnalyzer
// -------------------
//
//            07/04/2008 Kostas Kousouris       <kkousour@fnal.gov>
//                       Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////


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
#include "DataFormats/METReco/interface/MET.h"
#include "DataFormats/METReco/interface/METCollection.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

#include "PhysicsTools/UtilAlgos/interface/TFileService.h"

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

class JetResponseAnalyzer : public edm::EDAnalyzer
{
public:
  // construction/destruction
  explicit JetResponseAnalyzer(const edm::ParameterSet& iConfig);
  virtual ~JetResponseAnalyzer();

private:
  // member functions
  void beginJob(const edm::EventSetup& iSetup);
  void analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup);
  void endJob(){;}

private:
  // member data
  bool            doHistos_;
  bool            doTree_;
  bool            doFlavor_;
  bool            doJetPt_;
  bool            doRefPt_;
  bool            doRelRsp_;
  bool            doAbsRsp_;
  bool            doEtaRsp_;
  bool            doPhiRsp_;

  bool            doBalancing_;
  bool            doMET_;

  double          deltaRMax_;
  double          deltaPhiMin_;

  edm::InputTag   srcRef_;
  edm::InputTag   srcRefToJetMap_;
  edm::InputTag   srcRefToPartonMap_;
  edm::InputTag   srcMET_;
  
  bool            getFlavorFromMap_;
  double          deltaRPartonMax_;
  
  unsigned int    nRefMax_;
  unsigned int    nBinsRelRsp_;
  unsigned int    nBinsAbsRsp_;
  unsigned int    nBinsEtaRsp_;
  unsigned int    nBinsPhiRsp_;
  unsigned int    nBinsPt_;
  unsigned int    nBinsEta_;
  unsigned int    nBinsPhi_;

  double          etaBarrelMin_;
  double          etaBarrelMax_;

  double          relRspMin_;
  double          relRspMax_;
  double          absRspMin_;
  double          absRspMax_;
  double          etaRspMin_;
  double          etaRspMax_;
  double          phiRspMin_;
  double          phiRspMax_;

  vector<double>  binsPt_;
  vector<double>  binsEta_;
  vector<double>  binsPhi_;
  
  vector<TH1F**>  jetPtVsJetPt_;
  vector<TH1F**>  refPtVsRefPt_;
  vector<TH1F**>  jetPtVsRefPt_;
  vector<TH1F**>  refPtVsRefPtBarrel_;
  vector<TH1F**>  jetPtVsRefPtBarrel_;
  vector<TH1F**>  jetEtaVsJetEta_;
  vector<TH1F**>  jetPhiVsJetPhi_;
  vector<TH1F***> jetPtVsJetEtaJetPt_;
  vector<TH1F***> refPtVsJetEtaRefPt_;
  vector<TH1F***> jetPtVsJetEtaRefPt_;
  
  vector<TH1F**>  relRspVsJetPt_;
  vector<TH1F**>  relRspVsRefPt_;
  vector<TH1F**>  relRspVsRefPtBarrel_;
  vector<TH1F**>  relRspVsJetEta_;
  vector<TH1F**>  relRspVsJetPhi_;
  vector<TH1F***> relRspVsJetEtaJetPt_;
  vector<TH1F***> relRspVsJetEtaRefPt_;

  vector<TH1F**>  absRspVsJetPt_;
  vector<TH1F**>  absRspVsRefPt_;
  vector<TH1F**>  absRspVsRefPtBarrel_;
  vector<TH1F**>  absRspVsJetEta_;
  vector<TH1F**>  absRspVsJetPhi_;
  vector<TH1F***> absRspVsJetEtaJetPt_;
  vector<TH1F***> absRspVsJetEtaRefPt_;

  vector<TH1F**>  etaRspVsJetPt_;
  vector<TH1F**>  etaRspVsRefPt_;
  vector<TH1F**>  etaRspVsJetEta_;
  vector<TH1F**>  etaRspVsJetPhi_;
  vector<TH1F***> etaRspVsJetEtaJetPt_;
  vector<TH1F***> etaRspVsJetEtaRefPt_;
  
  vector<TH1F**>  phiRspVsJetPt_;
  vector<TH1F**>  phiRspVsRefPt_;
  vector<TH1F**>  phiRspVsJetEta_;
  vector<TH1F**>  phiRspVsJetPhi_;
  vector<TH1F***> phiRspVsJetEtaJetPt_;
  vector<TH1F***> phiRspVsJetEtaRefPt_;
  

  TTree*          tree_;
  float           pthat_;
  float           weight_;
  unsigned char   nref_;
  int             refpdgid_[100];
  float           refpt_[100];
  float           refeta_[100];
  float           refphi_[100];
  float           jtpt_[100];
  float           jteta_[100];
  float           jtphi_[100];
  float           refdrjt_[100];
  float           refdphijt_[100];
  float           metx_;
  float           mety_;
  
};


////////////////////////////////////////////////////////////////////////////////
// define local methods
////////////////////////////////////////////////////////////////////////////////

/// get the suffix for the histogram name, e.g. JetPt100to150
string getSuffix(const string& varname,int ibin,const vector<double>& bins);

/// get the index of the histogram corresponding to x
int getIndex(double x,const vector<double>& binsx);

/// fill the appropriate histogram (histos), based on x and binsx
void fillHisto(double value,double x,
	       const vector<double>& binsx,const vector<TH1F**>& histos);

/// fill the appropriate histogram (histos), based on pdgid, x and binsx
void fillHisto(int pdgid,double value,double x,
	       const vector<double>& binsx,const vector<TH1F**>& histos);

/// fill the appropriate histogram (histos), based on x, y, binsx, and binsy
void fillHisto(double value,double x,double y,
	       const vector<double>& binsx,const vector<double>& binsy,
	       const vector<TH1F***>& histos);

/// fill the appropriate histogram (histos), based on pdgid, x, y, binsx, and binsy
void fillHisto(int pdgid,double value,double x,double y,
	       const vector<double>& binsx,const vector<double>& binsy,
	       const vector<TH1F***>& histos);



////////////////////////////////////////////////////////////////////////////////
// construction/destruction
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
JetResponseAnalyzer::JetResponseAnalyzer(const edm::ParameterSet& iConfig)
  : doHistos_(iConfig.getParameter<bool>("doHistos"))
  , doTree_(iConfig.getParameter<bool>  ("doTree"))
  , doFlavor_(iConfig.getParameter<bool>("doFlavor"))
  , doJetPt_(iConfig.getParameter<bool> ("doJetPt"))
  , doRefPt_(iConfig.getParameter<bool> ("doRefPt"))
  , doBalancing_(false)
  , doMET_(false)
  , deltaRMax_(0.0)
  , deltaPhiMin_(3.141)
  , srcRef_(iConfig.getParameter<edm::InputTag>        ("srcRef"))
  , srcRefToJetMap_(iConfig.getParameter<edm::InputTag>("srcRefToJetMap"))
  , getFlavorFromMap_(false)
  , deltaRPartonMax_(0.0)
  , nRefMax_(iConfig.getParameter<unsigned int>("nRefMax"))
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
  
  if (iConfig.exists("srcMET")) {
    doMET_  = true;
    srcMET_ = iConfig.getParameter<edm::InputTag>("srcMET");
  }
  
  if (doHistos_) {

    nBinsPt_     =iConfig.getParameter<unsigned int>    ("nBinsPt");
    nBinsEta_    =iConfig.getParameter<unsigned int>    ("nBinsEta");
    nBinsPhi_    =iConfig.getParameter<unsigned int>    ("nBinsPhi");
    etaBarrelMin_=iConfig.getParameter<double>          ("etaBarrelMin");
    etaBarrelMax_=iConfig.getParameter<double>          ("etaBarrelMax");
    binsPt_      =iConfig.getParameter< vector<double> >("binsPt");
    binsEta_     =iConfig.getParameter< vector<double> >("binsEta");
    binsPhi_     =iConfig.getParameter< vector<double> >("binsPhi");
    
    nBinsRelRsp_ =iConfig.getParameter<unsigned int>    ("nBinsRelRsp");
    nBinsAbsRsp_ =iConfig.getParameter<unsigned int>    ("nBinsAbsRsp");
    nBinsEtaRsp_ =iConfig.getParameter<unsigned int>    ("nBinsEtaRsp");
    nBinsPhiRsp_ =iConfig.getParameter<unsigned int>    ("nBinsPhiRsp");
    
    doRelRsp_=(nBinsRelRsp_>0);
    if (doRelRsp_) {
      relRspMin_=iConfig.getParameter<double>("relRspMin");
      relRspMax_=iConfig.getParameter<double>("relRspMax");
    }
    doAbsRsp_=(nBinsAbsRsp_>0);
    if (doAbsRsp_) {
      absRspMin_=iConfig.getParameter<double>("absRspMin");
      absRspMax_=iConfig.getParameter<double>("absRspMax");
    }
    doEtaRsp_=(nBinsEtaRsp_>0);
    if (doEtaRsp_) {
      etaRspMin_=iConfig.getParameter<double>("etaRspMin");
      etaRspMax_=iConfig.getParameter<double>("etaRspMax");
    }
    doPhiRsp_=(nBinsPhiRsp_>0);
    if (doPhiRsp_) {
      phiRspMin_=iConfig.getParameter<double>("phiRspMin");
      phiRspMax_=iConfig.getParameter<double>("phiRspMax");
    }
  }
}


//______________________________________________________________________________
JetResponseAnalyzer::~JetResponseAnalyzer()
{

}


////////////////////////////////////////////////////////////////////////////////
// implementation of member functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void JetResponseAnalyzer::beginJob(const edm::EventSetup& iSetup)
{
  edm::Service<TFileService> fs;
  if (!fs) throw edm::Exception(edm::errors::Configuration,
				"TFileService missing from configuration!");
  
  if (doHistos_) {

    // define flavors
    vector<string> flavor;
    flavor.push_back("");
    if (doFlavor_) {
      flavor.push_back("uds_");
      flavor.push_back("c_");
      flavor.push_back("b_");
      flavor.push_back("g_");
    }

    // book pT histograms
    if (binsPt_.size()>=2) {
      for (unsigned int iPt=0;iPt<binsPt_.size()-1;++iPt) {

	string hname; double ptMin=binsPt_[iPt]; double ptMax=binsPt_[iPt+1];
	
	if (doJetPt_) {
	  jetPtVsJetPt_.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iFlv=0;iFlv<flavor.size();iFlv++) {
	    hname=flavor[iFlv]+"JetPt_"+getSuffix("JetPt",iPt,binsPt_);
	    jetPtVsJetPt_.back()[iFlv]=fs->make<TH1F>(hname.c_str(),
						      ";p_{T} [GeV]",
						      nBinsPt_,ptMin,ptMax);
	  }
	}
	
	if (doRefPt_) {
	  refPtVsRefPt_.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iFlv=0;iFlv<flavor.size();iFlv++) {
	    hname=flavor[iFlv]+"RefPt_"+getSuffix("RefPt",iPt,binsPt_);
	    refPtVsRefPt_.back()[iFlv]=fs->make<TH1F>(hname.c_str(),
						      ";p_{T}^{ref} [GeV]",
						      nBinsPt_,ptMin,ptMax);
	  }
	}
	
	if (doRefPt_) {
	  jetPtVsRefPt_.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iFlv=0;iFlv<flavor.size();iFlv++) {
	    hname=flavor[iFlv]+"JetPt_"+getSuffix("RefPt",iPt,binsPt_);
	    jetPtVsRefPt_.back()[iFlv]=fs->make<TH1F>(hname.c_str(),
						      ";p_{T} [GeV]",
						      2*nBinsPt_,
						      (ptMin>100.)*0.25*ptMin,
						      1.25*ptMax);
	  }
	}
	
	if (doRefPt_) {
	  refPtVsRefPtBarrel_.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iFlv=0;iFlv<flavor.size();iFlv++) {
	    hname=flavor[iFlv]+"RefPt_Barrel_"+getSuffix("RefPt",iPt,binsPt_);
	    refPtVsRefPtBarrel_.back()[iFlv]=fs->make<TH1F>(hname.c_str(),
							    ";p_{T}^{ref} [GeV]",
							    nBinsPt_,ptMin,ptMax);
	  }
	}
	
	if (doRefPt_) {
	  jetPtVsRefPtBarrel_.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iFlv=0;iFlv<flavor.size();iFlv++) {
	    hname=flavor[iFlv]+"JetPt_Barrel_"+getSuffix("RefPt",iPt,binsPt_);
	    jetPtVsRefPtBarrel_.back()[iFlv]=fs->make<TH1F>(hname.c_str(),
							    ";p_{T} [GeV]",
							    2*nBinsPt_,
							    (ptMin>100)*0.25*ptMin,
							    1.25*ptMax);
	  }
	}
	
	if (doRelRsp_&&doJetPt_) {
	  relRspVsJetPt_.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iFlv=0;iFlv<flavor.size();iFlv++) {
	    hname=flavor[iFlv]+"RelRsp_"+getSuffix("JetPt",iPt,binsPt_);
	    relRspVsJetPt_.back()[iFlv]=fs->make<TH1F>(hname.c_str(),
						       ";p_{T}/p_{T}^{ref}",
						       nBinsRelRsp_,
						       relRspMin_,relRspMax_);
	  }
	}
	
	if (doRelRsp_&&doRefPt_) {
	  relRspVsRefPt_.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iFlv=0;iFlv<flavor.size();iFlv++) {
	    hname=flavor[iFlv]+"RelRsp_"+getSuffix("RefPt",iPt,binsPt_);
	    relRspVsRefPt_.back()[iFlv]=fs->make<TH1F>(hname.c_str(),
						       ";p_{T}/p_{T}^{ref}",
						       nBinsRelRsp_,
						       relRspMin_,relRspMax_);
	  }
	}
	
	if (doRelRsp_&&doRefPt_) {
	  relRspVsRefPtBarrel_.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iFlv=0;iFlv<flavor.size();iFlv++) {
	    hname=flavor[iFlv]+"RelRsp_Barrel_"+getSuffix("RefPt",iPt,binsPt_);
	    relRspVsRefPtBarrel_.back()[iFlv]=fs->make<TH1F>(hname.c_str(),
							     ";p_{T}/p_{T}^{ref}",
							     nBinsRelRsp_,
							     relRspMin_,
							     relRspMax_);
	  }
	}
	
	if (doAbsRsp_&&doJetPt_) {
	  absRspVsJetPt_.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iFlv=0;iFlv<flavor.size();iFlv++) {
	    hname=flavor[iFlv]+"AbsRsp_"+getSuffix("JetPt",iPt,binsPt_);
	    absRspVsJetPt_.back()[iFlv]=fs->make<TH1F>(hname.c_str(),
						       ";p_{T}-p_{T}^{ref} [GeV]",
						       nBinsAbsRsp_,
						       absRspMin_,absRspMax_);
	  }
	}
	
	if (doAbsRsp_&&doRefPt_) {
	  absRspVsRefPt_.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iFlv=0;iFlv<flavor.size();iFlv++) {
	    hname=flavor[iFlv]+"AbsRsp_"+getSuffix("RefPt",iPt,binsPt_);
	    absRspVsRefPt_.back()[iFlv]=fs->make<TH1F>(hname.c_str(),
						       ";p_{T}-p_{T}^{ref} [GeV]",
						       nBinsAbsRsp_,
						       absRspMin_,absRspMax_);
	  }
	}
	
	if (doAbsRsp_&&doRefPt_) {
	  absRspVsRefPtBarrel_.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iFlv=0;iFlv<flavor.size();iFlv++) {
	    hname=flavor[iFlv]+"AbsRsp_Barrel_"+getSuffix("RefPt",iPt,binsPt_);
	    absRspVsRefPtBarrel_.back()[iFlv]=
	      fs->make<TH1F>(hname.c_str(),";p_{T}-p_{T}^{ref} [GeV]",
			     nBinsAbsRsp_,absRspMin_,absRspMax_);
	  }
	}

	if (doEtaRsp_&&doJetPt_) {
	  etaRspVsJetPt_.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iFlv=0;iFlv<flavor.size();iFlv++) {
	    hname=flavor[iFlv]+"EtaRsp_"+getSuffix("JetPt",iPt,binsPt_);
	    etaRspVsJetPt_.back()[iFlv]=fs->make<TH1F>(hname.c_str(),
						       ";|#eta|-|#eta^{ref}|",
						       nBinsEtaRsp_,
						       etaRspMin_,etaRspMax_);
	  }
	}
	
	if (doEtaRsp_&&doRefPt_) {
	  etaRspVsRefPt_.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iFlv=0;iFlv<flavor.size();iFlv++) {
	    hname=flavor[iFlv]+"EtaRsp_"+getSuffix("RefPt",iPt,binsPt_);
	    etaRspVsRefPt_.back()[iFlv]=fs->make<TH1F>(hname.c_str(),
						       ";|#eta|-|#eta^{ref}|",
						       nBinsEtaRsp_,
						       etaRspMin_,etaRspMax_);
	  }
	}
	
	if (doPhiRsp_&&doJetPt_) {
	  phiRspVsJetPt_.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iFlv=0;iFlv<flavor.size();iFlv++) {
	    hname=flavor[iFlv]+"PhiRsp_"+getSuffix("JetPt",iPt,binsPt_);
	    phiRspVsJetPt_.back()[iFlv]=fs->make<TH1F>(hname.c_str(),
						       ";#phi-#phi^{ref}",
						       nBinsPhiRsp_,
						       phiRspMin_,phiRspMax_);
	  }
	}
	
	if (doPhiRsp_&&doRefPt_) {
	  phiRspVsRefPt_.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iFlv=0;iFlv<flavor.size();iFlv++) {
	    hname=flavor[iFlv]+"PhiRsp_"+getSuffix("RefPt",iPt,binsPt_);
	    phiRspVsRefPt_.back()[iFlv]=fs->make<TH1F>(hname.c_str(),
						       ";#phi-#phi^{ref}",
						       nBinsPhiRsp_,
						       phiRspMin_,phiRspMax_);
	  }
	}
	
      }
    }
    
    // book eta histograms
    if (binsEta_.size()>=2) {
      for (unsigned int iEta=0;iEta<binsEta_.size()-1;++iEta) {
	
	string hname; double etaMin=binsEta_[iEta]; double etaMax=binsEta_[iEta+1];

	if (1) {
	  jetEtaVsJetEta_.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iFlv=0;iFlv<flavor.size();iFlv++) {
	    hname=flavor[iFlv]+"JetEta_"+getSuffix("JetEta",iEta,binsEta_);
	    jetEtaVsJetEta_.back()[iFlv]=fs->make<TH1F>(hname.c_str(),";#eta",
							nBinsEta_,etaMin,etaMax);
	  }
	}
	
	if (doRelRsp_) {
	  relRspVsJetEta_.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iFlv=0;iFlv<flavor.size();iFlv++) {
	    hname=flavor[iFlv]+"RelRsp_"+getSuffix("JetEta",iEta,binsEta_);
	    relRspVsJetEta_.back()[iFlv]=fs->make<TH1F>(hname.c_str(),
							";p_{T}/p_{T}^{ref}",
							nBinsRelRsp_,
							relRspMin_,relRspMax_);
	  }
	}
	
	if (doAbsRsp_) {
	  absRspVsJetEta_.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iFlv=0;iFlv<flavor.size();iFlv++) {
	    hname=flavor[iFlv]+"AbsRsp_"+getSuffix("JetEta",iEta,binsEta_);
	    absRspVsJetEta_.back()[iFlv]=fs->make<TH1F>(hname.c_str(),
							";p_{T}-p_{T}^{ref} [GeV]",
							nBinsAbsRsp_,
							absRspMin_,absRspMax_);
	  }
	}
	
	if (doEtaRsp_) {
	  etaRspVsJetEta_.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iFlv=0;iFlv<flavor.size();iFlv++) {
	    hname=flavor[iFlv]+"EtaRsp_"+getSuffix("JetEta",iEta,binsEta_);
	    etaRspVsJetEta_.back()[iFlv]=fs->make<TH1F>(hname.c_str(),
							";|#eta|-|#eta^{ref}|",
							nBinsEtaRsp_,
							etaRspMin_,etaRspMax_);
	  }
	}
	
	if (doPhiRsp_) {
	  phiRspVsJetEta_.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iFlv=0;iFlv<flavor.size();iFlv++) {
	    hname=flavor[iFlv]+"PhiRsp_"+getSuffix("JetEta",iEta,binsEta_);
	    phiRspVsJetEta_.back()[iFlv]=fs->make<TH1F>(hname.c_str(),
							";#phi-#phi^{ref}",
							nBinsPhiRsp_,
							phiRspMin_,phiRspMax_);
	  }
	}
	
      }
    }
    
    // book phi histograms
    if (binsPhi_.size()>=2) {
      for (unsigned int iPhi=0;iPhi<binsPhi_.size()-1;++iPhi) {
	
	string hname; double phiMin=binsPhi_[iPhi]; double phiMax=binsPhi_[iPhi+1];

	if (1) {
	  jetPhiVsJetPhi_.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iFlv=0;iFlv<flavor.size();iFlv++) {
	    hname=flavor[iFlv]+"JetPhi_"+getSuffix("JetPhi",iPhi,binsPhi_);
	    jetPhiVsJetPhi_.back()[iFlv]=fs->make<TH1F>(hname.c_str(),";#phi",
							nBinsPhi_,phiMin,phiMax);
	  }
	}
	
	if (doRelRsp_) {
	  relRspVsJetPhi_.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iFlv=0;iFlv<flavor.size();iFlv++) {
	    hname=flavor[iFlv]+"RelRsp_"+getSuffix("JetPhi",iPhi,binsPhi_);
	    relRspVsJetPhi_.back()[iFlv]=fs->make<TH1F>(hname.c_str(),
							";p_{T}/p_{T}^{ref}",
							nBinsRelRsp_,
							relRspMin_,relRspMax_);
	  }
	}
	
	if (doAbsRsp_) {
	  absRspVsJetPhi_.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iFlv=0;iFlv<flavor.size();iFlv++) {
	    hname=flavor[iFlv]+"AbsRsp_"+getSuffix("JetPhi",iPhi,binsPhi_);
	    absRspVsJetPhi_.back()[iFlv]=fs->make<TH1F>(hname.c_str(),
							";p_{T}-p_{T}^{ref} [GeV]",
							nBinsAbsRsp_,
							absRspMin_,absRspMax_);
	  }
	}
	
	if (doEtaRsp_) {
	  etaRspVsJetPhi_.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iFlv=0;iFlv<flavor.size();iFlv++) {
	    hname=flavor[iFlv]+"EtaRsp_"+getSuffix("JetPhi",iPhi,binsPhi_);
	    etaRspVsJetPhi_.back()[iFlv]=fs->make<TH1F>(hname.c_str(),
							";|#eta|-|#eta^{ref}|",
							nBinsEtaRsp_,
							etaRspMin_,etaRspMax_);
	  }
	}
	
	if (doPhiRsp_) {
	  phiRspVsJetPhi_.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iFlv=0;iFlv<flavor.size();iFlv++) {
	    hname=flavor[iFlv]+"PhiRsp_"+getSuffix("JetPhi",iPhi,binsPhi_);
	    phiRspVsJetPhi_.back()[iFlv]=fs->make<TH1F>(hname.c_str(),
							";#phi-#phi^{ref}",
							nBinsPhiRsp_,
							phiRspMin_,phiRspMax_);
	  }
	}
	
      }
    }
    
    // book eta/pT histograms
    if (binsPt_.size()>=2&&binsEta_.size()>=2) {
      for (unsigned int iEta=0;iEta<binsEta_.size()-1;++iEta) {

	TH1F*** jetPtJetPt(0);
	TH1F*** refPtRefPt(0);
	TH1F*** jetPtRefPt(0);
	TH1F*** relRspJetPt(0);
	TH1F*** relRspRefPt(0);
	TH1F*** absRspJetPt(0);
	TH1F*** absRspRefPt(0);
	TH1F*** etaRspJetPt(0);
	TH1F*** etaRspRefPt(0);
	TH1F*** phiRspJetPt(0);
	TH1F*** phiRspRefPt(0);
	
	if (doJetPt_) {
	  jetPtJetPt=new TH1F**[binsPt_.size()-1];
	  for (unsigned int iPt=0;iPt<binsPt_.size()-1;iPt++)
	    jetPtJetPt[iPt]=new TH1F*[flavor.size()];
	}
	
	if (doRefPt_) {
	  refPtRefPt =new TH1F**[binsPt_.size()];
	  for (unsigned int iPt=0;iPt<binsPt_.size()-1;iPt++)
	    refPtRefPt[iPt]=new TH1F*[flavor.size()];
	}
	
	if (doRefPt_) {
	  jetPtRefPt =new TH1F**[binsPt_.size()];
	  for (unsigned int iPt=0;iPt<binsPt_.size()-1;iPt++)
	    jetPtRefPt[iPt]=new TH1F*[flavor.size()];
	}
	
	if (doRelRsp_&&doJetPt_) {
	  relRspJetPt=new TH1F**[binsPt_.size()-1];
	  for (unsigned int iPt=0;iPt<binsPt_.size()-1;iPt++)
	    relRspJetPt[iPt]=new TH1F*[flavor.size()];
	}
	
	if (doRelRsp_&&doRefPt_) {
	  relRspRefPt=new TH1F**[binsPt_.size()-1];
	  for (unsigned int iPt=0;iPt<binsPt_.size()-1;iPt++)
	    relRspRefPt[iPt]=new TH1F*[flavor.size()];
	}
	
	if (doAbsRsp_&&doJetPt_) {
	  absRspJetPt=new TH1F**[binsPt_.size()-1];
	  for (unsigned int iPt=0;iPt<binsPt_.size()-1;iPt++)
	    absRspJetPt[iPt]=new TH1F*[flavor.size()];
	}
	
	if (doAbsRsp_&&doRefPt_) {
	  absRspRefPt=new TH1F**[binsPt_.size()-1];
	  for (unsigned int iPt=0;iPt<binsPt_.size()-1;iPt++)
	    absRspRefPt[iPt]=new TH1F*[flavor.size()];
	}
	
	if (doEtaRsp_&&doJetPt_) {
	  etaRspJetPt=new TH1F**[binsPt_.size()-1];
	  for (unsigned int iPt=0;iPt<binsPt_.size()-1;iPt++)
	    etaRspJetPt[iPt]=new TH1F*[flavor.size()];
	}
	
	if (doEtaRsp_&&doRefPt_) {
	  etaRspRefPt=new TH1F**[binsPt_.size()-1];
	  for (unsigned int iPt=0;iPt<binsPt_.size()-1;iPt++)
	    etaRspRefPt[iPt]=new TH1F*[flavor.size()];
	}
	
	if (doPhiRsp_&&doJetPt_) {
	  phiRspJetPt=new TH1F**[binsPt_.size()-1];
	  for (unsigned int iPt=0;iPt<binsPt_.size()-1;iPt++)
	    phiRspJetPt[iPt]=new TH1F*[flavor.size()];
	}
	
	if (doPhiRsp_&&doRefPt_) {
	  phiRspRefPt=new TH1F**[binsPt_.size()-1];
	  for (unsigned int iPt=0;iPt<binsPt_.size()-1;iPt++)
	    phiRspRefPt[iPt]=new TH1F*[flavor.size()];
	}
	
	string jetEtaSuffix=getSuffix("JetEta",iEta,binsEta_);
	
	for (unsigned int iPt=0;iPt<binsPt_.size()-1;iPt++) {	

	  string hname; double ptMin=binsPt_[iPt]; double ptMax=binsPt_[iPt+1];
	  
	  string jetPtSuffix=getSuffix("JetPt",iPt,binsPt_);
	  string refPtSuffix=getSuffix("RefPt",iPt,binsPt_);
	  
	  for (unsigned int iFlv=0;iFlv<flavor.size();iFlv++) {

	    if (doJetPt_) {
	      hname=flavor[iFlv]+"JetPt_"+jetEtaSuffix+"_"+jetPtSuffix;
	      jetPtJetPt[iPt][iFlv]=fs->make<TH1F>(hname.c_str(),";p_{T}",
						   nBinsPt_,ptMin,ptMax);
	    }
	    
	    if (doRefPt_) {
	      hname=flavor[iFlv]+"RefPt_"+jetEtaSuffix+"_"+refPtSuffix;
	      refPtRefPt[iPt][iFlv]=fs->make<TH1F>(hname.c_str(),";p_{T}^{ref}",
						   nBinsPt_,ptMin,ptMax);
	    }
	    
	    if (doRefPt_) {
	      hname=flavor[iFlv]+"JetPt_"+jetEtaSuffix+"_"+refPtSuffix;
	      jetPtRefPt[iPt][iFlv]=fs->make<TH1F>(hname.c_str(),";p_{T}",
						   2*nBinsPt_,
						   (ptMin>100.)*0.25*ptMin,
						   1.25*ptMax);
	    }
	    
	    if (doRelRsp_&&doJetPt_) {
	      hname=flavor[iFlv]+"RelRsp_"+jetEtaSuffix+"_"+jetPtSuffix;
	      relRspJetPt[iPt][iFlv]=fs->make<TH1F>(hname.c_str(),
						    ";p_{T}/p_{T}^{ref}",
						    nBinsRelRsp_,
						    relRspMin_,relRspMax_);
	    }
	    
	    if (doRelRsp_&&doRefPt_) {
	      hname=flavor[iFlv]+"RelRsp_"+jetEtaSuffix+"_"+refPtSuffix;
	      relRspRefPt[iPt][iFlv]=fs->make<TH1F>(hname.c_str(),
						    ";p_{T}/p_{T}^{ref}",
						    nBinsRelRsp_,
						    relRspMin_,relRspMax_);
	    }
	    
	    if (doAbsRsp_&&doJetPt_) {
	      hname=flavor[iFlv]+"AbsRsp_"+jetEtaSuffix+"_"+jetPtSuffix;
	      absRspJetPt[iPt][iFlv]=fs->make<TH1F>(hname.c_str(),
						    ";p_{T}-p_{T}^{ref} [GeV]",
						    nBinsAbsRsp_,
						    absRspMin_,absRspMax_);
	    }
	    
	    if (doAbsRsp_&&doRefPt_) {
	      hname=flavor[iFlv]+"AbsRsp_"+jetEtaSuffix+"_"+refPtSuffix;
	      absRspRefPt[iPt][iFlv]=fs->make<TH1F>(hname.c_str(),
						    ";p_{T}-p_{T}^{ref} [GeV]",
						    nBinsAbsRsp_,
						    absRspMin_,absRspMax_);
	    }
	    
	    if (doEtaRsp_&&doJetPt_) {
	      hname=flavor[iFlv]+"EtaRsp_"+jetEtaSuffix+"_"+jetPtSuffix;
	      etaRspJetPt[iPt][iFlv]=fs->make<TH1F>(hname.c_str(),
						    ";|#eta|-|#eta^{ref}|",
						    nBinsEtaRsp_,
						    etaRspMin_,etaRspMax_);
	    }
	    
	    if (doEtaRsp_&&doRefPt_) {
	      hname=flavor[iFlv]+"EtaRsp_"+jetEtaSuffix+"_"+refPtSuffix;
	      etaRspRefPt[iPt][iFlv]=fs->make<TH1F>(hname.c_str(),
						    ";|#eta|-|#eta^{ref}|",
						    nBinsEtaRsp_,
						    etaRspMin_,etaRspMax_);
	    }
	    
	    if (doPhiRsp_&&doJetPt_) {
	      hname=flavor[iFlv]+"PhiRsp_"+jetEtaSuffix+"_"+jetPtSuffix;
	      phiRspJetPt[iPt][iFlv]=fs->make<TH1F>(hname.c_str(),
						    ";#phi-#phi^{ref}",
						    nBinsPhiRsp_,
						    phiRspMin_,phiRspMax_);
	    }
	    
	    if (doPhiRsp_&&doRefPt_) {
	      hname=flavor[iFlv]+"PhiRsp_"+jetEtaSuffix+"_"+refPtSuffix;
	      phiRspRefPt[iPt][iFlv]=fs->make<TH1F>(hname.c_str(),
						    ";#phi-#phi^{ref}",
						    nBinsPhiRsp_,
						    phiRspMin_,phiRspMax_);
	    }
	    
	  }
	  
	}
	
	if (doJetPt_)            jetPtVsJetEtaJetPt_ .push_back(jetPtJetPt);
	if (doRefPt_)            refPtVsJetEtaRefPt_ .push_back(refPtRefPt);
	if (doRefPt_)            jetPtVsJetEtaRefPt_ .push_back(jetPtRefPt);
	if (doRelRsp_&&doJetPt_) relRspVsJetEtaJetPt_.push_back(relRspJetPt);
	if (doRelRsp_&&doRefPt_) relRspVsJetEtaRefPt_.push_back(relRspRefPt);
	if (doAbsRsp_&&doJetPt_) absRspVsJetEtaJetPt_.push_back(absRspJetPt);
	if (doAbsRsp_&&doRefPt_) absRspVsJetEtaRefPt_.push_back(absRspRefPt);
	if (doEtaRsp_&&doJetPt_) etaRspVsJetEtaJetPt_.push_back(etaRspJetPt);
	if (doEtaRsp_&&doRefPt_) etaRspVsJetEtaRefPt_.push_back(etaRspRefPt);
	if (doPhiRsp_&&doJetPt_) phiRspVsJetEtaJetPt_.push_back(phiRspJetPt);
	if (doPhiRsp_&&doRefPt_) phiRspVsJetEtaRefPt_.push_back(phiRspRefPt);
      }
    }
  } // doHistos_
  
  if (doTree_) {
    tree_=fs->make<TTree>("t","t");
    tree_->Branch("nref",  &nref_,  "nref/b");
    tree_->Branch("pthat", &pthat_, "pthat/F");
    tree_->Branch("weight",&weight_,"weight/F");
    if (doFlavor_) tree_->Branch("refpdgid",refpdgid_,"refpdgid[nref]/I");
    tree_->Branch("refpt",  refpt_, "refpt[nref]/F");
    tree_->Branch("refeta", refeta_,"refeta[nref]/F");
    tree_->Branch("refphi", refphi_,"refphi[nref]/F");
    tree_->Branch("jtpt",   jtpt_,  "jtpt[nref]/F");
    tree_->Branch("jteta",  jteta_, "jteta[nref]/F");
    tree_->Branch("jtphi",  jtphi_, "jtphi[nref]/F");
    if (doBalancing_) tree_->Branch("refdphijt",refdphijt_,"refdphijt[nref]/F");
    else    	      tree_->Branch("refdrjt",  refdrjt_,  "refdrjt[nref]/F");
    if (doMET_) {
      tree_->Branch("metx", &metx_, "metx/F");
      tree_->Branch("mety", &mety_, "mety/F");
    }
  }
  
}


//______________________________________________________________________________
void JetResponseAnalyzer::analyze(const edm::Event&      iEvent,
				  const edm::EventSetup& iSetup)
{
  nref_=0;
  edm::Handle<GenEventInfoProduct>    genInfo;
  edm::Handle<reco::CandidateView>    refs;
  edm::Handle<reco::CandViewMatchMap> refToJetMap;
  edm::Handle<reco::CandViewMatchMap> refToPartonMap;
  edm::Handle<reco::METCollection>    mets;
  
  pthat_  = 0.0;
  weight_ = 1.0;
  if (iEvent.getByLabel("generator",genInfo)) {
    if (genInfo->hasBinningValues())
      pthat_  = (float)genInfo->binningValues()[0];
    weight_ = (float)genInfo->weight();
  }
  

  iEvent.getByLabel(srcRef_,        refs);
  iEvent.getByLabel(srcRefToJetMap_,refToJetMap);
  if (getFlavorFromMap_) iEvent.getByLabel(srcRefToPartonMap_,refToPartonMap);

  if (doMET_) {
    iEvent.getByLabel(srcMET_,mets);
    metx_ = mets->at(0).px();
    mety_ = mets->at(0).py();
  }
  
  if (doBalancing_&&refToJetMap->size()!=1) return;
  
  unsigned int nRef=(nRefMax_==0) ? refs->size() : std::min(nRefMax_,refs->size());
  
  for (unsigned int iRef=0;iRef<nRef;iRef++) {

    reco::CandidateBaseRef ref=refs->refAt(iRef);
    reco::CandViewMatchMap::const_iterator itMatch=refToJetMap->find(ref);
    if (itMatch==refToJetMap->end()) continue;
    reco::CandidateBaseRef jet=itMatch->val;
    
    refdrjt_[nref_]  =reco::deltaR(jet->eta(),jet->phi(),ref->eta(),ref->phi());
    refdphijt_[nref_]=reco::deltaPhi(jet->phi(),ref->phi());
    
    if ((!doBalancing_&&refdrjt_[nref_]>deltaRMax_)||
	(doBalancing_&&std::abs(refdphijt_[nref_])<deltaPhiMin_)) continue;
    
    refpdgid_[nref_]=0;
    if (getFlavorFromMap_) {
      itMatch=refToPartonMap->find(ref);
      if (itMatch!=refToPartonMap->end()) {
	double refdrparton=reco::deltaR(*itMatch->key,*itMatch->val);
	if (refdrparton<deltaRPartonMax_)
	  refpdgid_[nref_]=itMatch->val->pdgId();
      }
    }
    else {
      refpdgid_[nref_]=ref->pdgId();
    }
    
    refpt_[nref_]   = ref->pt();
    refeta_[nref_]  = ref->eta();
    refphi_[nref_]  = ref->phi();
    jtpt_[nref_]    = jet->pt();
    jteta_[nref_]   = jet->eta();
    jtphi_[nref_]   = jet->phi();
    nref_++;

    double jetPt  =jet->pt();    
    double jetEta =
      (binsEta_.size()&&binsEta_.front()>=0.0) ? std::abs(jet->eta()) : jet->eta();
    double jetPhi = jet->phi();
    double refPt  = ref->pt();
    double absRsp = jetPt-refPt;
    double relRsp = jetPt/refPt;
    double etaRsp = std::abs(jet->eta()) - std::abs(ref->eta());
    double phiRsp = jet->phi() - ref->phi();

    if (doHistos_) {
      if (jetEta>=etaBarrelMin_&&jetEta<=etaBarrelMax_) {
	if (doRefPt_) {
	  fillHisto(refPt,refPt,binsPt_,refPtVsRefPtBarrel_);
	  fillHisto(jetPt,refPt,binsPt_,jetPtVsRefPtBarrel_);
	  if (doFlavor_) {
	    fillHisto(refpdgid_[nref_],
		      refPt,refPt,binsPt_,refPtVsRefPtBarrel_);
	    fillHisto(refpdgid_[nref_],
		      jetPt,refPt,binsPt_,jetPtVsRefPtBarrel_);
	  }
	}
	if (doRelRsp_&&doRefPt_) {
	  fillHisto(relRsp,refPt,binsPt_,relRspVsRefPtBarrel_);
	  if (doFlavor_)
	    fillHisto(refpdgid_[nref_],
		      relRsp,refPt,binsPt_,relRspVsRefPtBarrel_);
	}
	if (doAbsRsp_&&doRefPt_) {
	  fillHisto(absRsp,refPt,binsPt_,absRspVsRefPtBarrel_);
	  if (doFlavor_)
	    fillHisto(refpdgid_[nref_],
		      absRsp,refPt,binsPt_,absRspVsRefPtBarrel_);
	}
      }
      
      if (doJetPt_) {
	fillHisto(jetPt, jetPt, binsPt_, jetPtVsJetPt_);
	if (doFlavor_) fillHisto(refpdgid_[nref_],
				 jetPt, jetPt, binsPt_, jetPtVsJetPt_);
      }
      if (doRefPt_) {
	fillHisto(refPt, refPt, binsPt_, refPtVsRefPt_);
	fillHisto(jetPt, refPt, binsPt_, jetPtVsRefPt_);
	if (doFlavor_) {
	  fillHisto(refpdgid_[nref_],refPt, refPt, binsPt_, refPtVsRefPt_);
	  fillHisto(refpdgid_[nref_],jetPt, refPt, binsPt_, jetPtVsRefPt_);
	}
      }
      
      if (binsEta_.size()>=2) {
	fillHisto(jetEta,jetEta,binsEta_,jetEtaVsJetEta_);
	if (doFlavor_) fillHisto(refpdgid_[nref_],
				 jetEta,jetEta,binsEta_,jetEtaVsJetEta_);
      }
      
      if (binsPhi_.size()>=2) {
	fillHisto(jetPhi,jetPhi,binsPhi_,jetPhiVsJetPhi_);
	if (doFlavor_) fillHisto(refpdgid_[nref_],
				 jetPhi,jetPhi,binsPhi_,jetPhiVsJetPhi_);
      }
      
      if (doJetPt_&&binsEta_.size()>=2) {
	fillHisto(jetPt, jetEta,jetPt,
		  binsEta_,binsPt_,jetPtVsJetEtaJetPt_);
	if (doFlavor_) fillHisto(refpdgid_[nref_],
				 jetPt, jetEta,jetPt,
				 binsEta_,binsPt_,jetPtVsJetEtaJetPt_);
      }
      if (doRefPt_&&binsEta_.size()>=2) {
	fillHisto(refPt, jetEta,refPt,
		  binsEta_,binsPt_,refPtVsJetEtaRefPt_);
	fillHisto(jetPt, jetEta,refPt,
		  binsEta_,binsPt_,jetPtVsJetEtaRefPt_);
	if (doFlavor_) {
	  fillHisto(refpdgid_[nref_],refPt, jetEta,refPt,
		    binsEta_,binsPt_,refPtVsJetEtaRefPt_);
	  fillHisto(refpdgid_[nref_],jetPt, jetEta,refPt,
		    binsEta_,binsPt_,jetPtVsJetEtaRefPt_);
	}
      }
      
      if (doRelRsp_) {
	if (doJetPt_) {
	  fillHisto(relRsp,jetPt, binsPt_, relRspVsJetPt_);
	  if (doFlavor_) fillHisto(refpdgid_[nref_],
				   relRsp,jetPt, binsPt_, relRspVsJetPt_);
	}
	if (doRefPt_) {
	  fillHisto(relRsp,refPt, binsPt_, relRspVsRefPt_);
	  if (doFlavor_) fillHisto(refpdgid_[nref_],
				   relRsp,refPt, binsPt_, relRspVsRefPt_);
	}
	
	if (binsEta_.size()>=2) {
	  fillHisto(relRsp,jetEta,binsEta_,relRspVsJetEta_);
	  if (doFlavor_) fillHisto(refpdgid_[nref_],
				   relRsp,jetEta,binsEta_,relRspVsJetEta_);
	}
	
	if (binsPhi_.size()>=2) {
	  fillHisto(relRsp,jetPhi,binsPhi_,relRspVsJetPhi_);
	  if (doFlavor_) fillHisto(refpdgid_[nref_],
				   relRsp,jetPhi,binsPhi_,relRspVsJetPhi_);
	}
	
	if (doJetPt_&&binsEta_.size()>=2) {
	  fillHisto(relRsp,jetEta,jetPt,
		    binsEta_,binsPt_,relRspVsJetEtaJetPt_);
	  if (doFlavor_) fillHisto(refpdgid_[nref_],relRsp,jetEta,jetPt,
				   binsEta_,binsPt_,relRspVsJetEtaJetPt_);
	}
	if (doRefPt_&&binsEta_.size()>=2) {
	  fillHisto(relRsp,jetEta,refPt,
		    binsEta_,binsPt_,relRspVsJetEtaRefPt_);
	  if (doFlavor_) fillHisto(refpdgid_[nref_],relRsp,jetEta,refPt,
				   binsEta_,binsPt_,relRspVsJetEtaRefPt_);
	}
      }
      
      if (doAbsRsp_) {
	if (doJetPt_) {
	  fillHisto(absRsp,jetPt, binsPt_, absRspVsJetPt_);
	  if (doFlavor_) fillHisto(refpdgid_[nref_],
				   absRsp,jetPt, binsPt_, absRspVsJetPt_);
	}
	if (doRefPt_) {
	  fillHisto(absRsp,refPt, binsPt_, absRspVsRefPt_);
	  if (doFlavor_) fillHisto(refpdgid_[nref_],
				   absRsp,refPt, binsPt_, absRspVsRefPt_);
	}
	
	if (binsEta_.size()>=2) {
	  fillHisto(absRsp,jetEta,binsEta_,absRspVsJetEta_);
	  if (doFlavor_) fillHisto(refpdgid_[nref_],
				   absRsp,jetEta,binsEta_,absRspVsJetEta_);
	}
	
	if (binsPhi_.size()>=2) {
	  fillHisto(absRsp,jetPhi,binsPhi_,absRspVsJetPhi_);
	  if (doFlavor_) fillHisto(refpdgid_[nref_],
				   absRsp,jetPhi,binsPhi_,absRspVsJetPhi_);
	}
	
	if (doJetPt_&&binsEta_.size()>=2) {
	  fillHisto(absRsp,jetEta,jetPt,
		    binsEta_,binsPt_,absRspVsJetEtaJetPt_);
	  if (doFlavor_) fillHisto(refpdgid_[nref_],absRsp,jetEta,jetPt,
				   binsEta_,binsPt_,absRspVsJetEtaJetPt_);
	}
	if (doRefPt_&&binsEta_.size()>=2) {
	  fillHisto(absRsp,jetEta,refPt,
		    binsEta_,binsPt_,absRspVsJetEtaRefPt_);
	  if (doFlavor_) fillHisto(refpdgid_[nref_],absRsp,jetEta,refPt,
				   binsEta_,binsPt_,absRspVsJetEtaRefPt_);
	}
      }
      
      if (doEtaRsp_) {
	if (doJetPt_) {
	  fillHisto(etaRsp,jetPt, binsPt_, etaRspVsJetPt_);
	  if (doFlavor_) fillHisto(refpdgid_[nref_],
				   etaRsp,jetPt, binsPt_, etaRspVsJetPt_);
	}
	if (doRefPt_) {
	  fillHisto(etaRsp,refPt, binsPt_, etaRspVsRefPt_);
	  if (doFlavor_) fillHisto(refpdgid_[nref_],
				   etaRsp,refPt, binsPt_, etaRspVsRefPt_);
	}
	if (binsEta_.size()>=2) {
	  fillHisto(etaRsp,jetEta,binsEta_,etaRspVsJetEta_);
	  if (doFlavor_) fillHisto(refpdgid_[nref_],
				   etaRsp,jetEta,binsEta_,etaRspVsJetEta_);
	}
	if (binsPhi_.size()>=2) {
	  fillHisto(etaRsp,jetPhi,binsPhi_,etaRspVsJetPhi_);
	  if (doFlavor_) fillHisto(refpdgid_[nref_],
				   etaRsp,jetPhi,binsPhi_,etaRspVsJetPhi_);
	}
	if (doJetPt_&&binsEta_.size()>=2) {
	  fillHisto(etaRsp,jetEta,jetPt,
		    binsEta_,binsPt_,etaRspVsJetEtaJetPt_);
	  if (doFlavor_) fillHisto(refpdgid_[nref_],etaRsp,jetEta,jetPt,
				   binsEta_,binsPt_,etaRspVsJetEtaJetPt_);
	}
	if (doRefPt_&&binsEta_.size()>=2) {
	  fillHisto(etaRsp,jetEta,refPt,
		    binsEta_,binsPt_,etaRspVsJetEtaRefPt_);
	  if (doFlavor_) fillHisto(refpdgid_[nref_],etaRsp,jetEta,refPt,
				   binsEta_,binsPt_,etaRspVsJetEtaRefPt_);
	}
      }
      
      if (doPhiRsp_) {
	if (doJetPt_) {
	  fillHisto(phiRsp,jetPt, binsPt_, phiRspVsJetPt_);
	  if (doFlavor_) fillHisto(refpdgid_[nref_],
				   phiRsp,jetPt, binsPt_, phiRspVsJetPt_);
	}
	if (doRefPt_) {
	  fillHisto(phiRsp,refPt, binsPt_, phiRspVsRefPt_);
	  if (doFlavor_) fillHisto(refpdgid_[nref_],
				   phiRsp,refPt, binsPt_, phiRspVsRefPt_);
	}
	if (binsEta_.size()>=2) {
	  fillHisto(phiRsp,jetEta,binsEta_,phiRspVsJetEta_);
	  if (doFlavor_) fillHisto(refpdgid_[nref_],
				   phiRsp,jetEta,binsEta_,phiRspVsJetEta_);
	}
	if (binsPhi_.size()>=2) {
	  fillHisto(phiRsp,jetPhi,binsPhi_,phiRspVsJetPhi_);
	  if (doFlavor_) fillHisto(refpdgid_[nref_],
				   phiRsp,jetPhi,binsPhi_,phiRspVsJetPhi_);
	}
	if (doJetPt_&&binsEta_.size()>=2) {
	  fillHisto(phiRsp,jetEta,jetPt,
		    binsEta_,binsPt_,phiRspVsJetEtaJetPt_);
	  if (doFlavor_) fillHisto(refpdgid_[nref_],phiRsp,jetEta,jetPt,
				   binsEta_,binsPt_,phiRspVsJetEtaJetPt_);
	}
	if (doRefPt_&&binsEta_.size()>=2) {
	  fillHisto(phiRsp,jetEta,refPt,
		    binsEta_,binsPt_,phiRspVsJetEtaRefPt_);
	  if (doFlavor_) fillHisto(refpdgid_[nref_],phiRsp,jetEta,refPt,
				   binsEta_,binsPt_,phiRspVsJetEtaRefPt_);
	}
      }
      
    }
  }
  
  if (doTree_) tree_->Fill();
}


//______________________________________________________________________________
string getSuffix(const string& varname,int ibin,const vector<double>& bins)
{
  stringstream ss; ss<<varname<<bins[ibin]<<"to"<<bins[ibin+1];
  return ss.str();
}

//______________________________________________________________________________
int getIndex(double x,const vector<double>& binsx)
{
  for (unsigned int ix=0;ix<binsx.size()-1;ix++)
    if (x>=binsx[ix]&&x<binsx[ix+1]) return ix;
  return -1;
}

//______________________________________________________________________________
void fillHisto(double value,double x,
	       const vector<double>& binsx,const vector<TH1F**>& histos)
{
  int ix=getIndex(x,binsx);
  if (ix>=0) histos[ix][0]->Fill(value);
}

//______________________________________________________________________________
void fillHisto(int pdgid,double value,double x,
	       const vector<double>& binsx,const vector<TH1F**>& histos)
{
  int abspdgid=std::abs(pdgid);
  int iflv(-1);
  if (abspdgid>=1&&abspdgid<=3) iflv=1;
  else if (abspdgid== 4)        iflv=2;
  else if (abspdgid== 5)        iflv=3;
  else if (abspdgid==21)        iflv=4;
  else return;

  int ix=getIndex(x,binsx);
  if (ix>=0) histos[ix][iflv]->Fill(value);
}


//______________________________________________________________________________
void fillHisto(double value,double x,double y,
	       const vector<double>& binsx,const vector<double>& binsy,
	       const vector<TH1F***>& histos)
{
  int ix=getIndex(x,binsx);
  int iy=getIndex(y,binsy);
  if (ix>=0&&iy>=0) histos[ix][iy][0]->Fill(value);
}

//______________________________________________________________________________
void fillHisto(int pdgid,double value,double x,double y,
	       const vector<double>& binsx,const vector<double>& binsy,
	       const vector<TH1F***>& histos)
{
  int abspdgid=std::abs(pdgid);
  int iflv(-1);
  if (abspdgid>=1&&abspdgid<=3) iflv=1;
  else if (abspdgid== 4)        iflv=2;
  else if (abspdgid== 5)        iflv=3;
  else if (abspdgid==21)        iflv=4;
  else return;
  
  int ix=getIndex(x,binsx);
  int iy=getIndex(y,binsy);
  if (ix>=0&&iy>=0) histos[ix][iy][iflv]->Fill(value);
}



////////////////////////////////////////////////////////////////////////////////
// define JetEfficiencyAnalyzer as a plugin
////////////////////////////////////////////////////////////////////////////////

DEFINE_FWK_MODULE(JetResponseAnalyzer);

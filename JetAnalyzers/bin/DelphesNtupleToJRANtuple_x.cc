///////////////////////////////////////////////////////////////////
//
// DelphesNtupleToJRANtuple_x
// --------------------------
//
//            01/16/2014 Alexx Perloff  <aperloff@physics.tamu.edu>
///////////////////////////////////////////////////////////////////

#include "JetMETAnalysis/JetAnalyzers/interface/Settings.h"
#include "JetMETAnalysis/JetAnalyzers/interface/VectorWrapper.h"
#include "JetMETAnalysis/JetAnalyzers/interface/VectorWrapper2D.h"
#include "Delphes/classes/DelphesClasses.h"
#include "Delphes/external/ExRootAnalysis/ExRootTreeReader.h"
#include "JetMETAnalysis/JetUtilities/interface/TProfileMDF.h"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/JRANtuple.h"

#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "TROOT.h"
#include "TSystem.h"
#include <TObjectTable.h>
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2.h"
#include "TH2F.h"
#include "TH3D.h"
#include "TF1.h"
#include "TString.h"
#include "TMath.h"
#include "TFitResult.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TProfile3D.h"
#include "TBenchmark.h"
#include "TLorentzVector.h"
#include "TClonesArray.h"

#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdarg.h>
#include <cstring>
#include <map>
#include <utility>
#include <set>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// class definition
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
typedef std::pair<double,std::pair<unsigned int,unsigned int> > Match_t;

//______________________________________________________________________________
struct MatchLtComp
{
  bool operator()(const Match_t& m1,const Match_t& m2) const
  {
    return m1.first<m2.first;
  }
};

////////////////////////////////////////////////////////////////////////////////
// define typedefs
////////////////////////////////////////////////////////////////////////////////
typedef set<unsigned int>                                       IndexSet_t;
typedef IndexSet_t::const_iterator                              IndexIter_t;
typedef set<Match_t,MatchLtComp>                                MatchSet_t;
typedef MatchSet_t::const_iterator                              MatchIter_t;
typedef map<Jet*, Jet*>                                         JetToJet_t;
typedef map<string,pair<TClonesArray*,JRANtuple*> >             NtupleMap_t;

////////////////////////////////////////////////////////////////////////////////
// define global variables
////////////////////////////////////////////////////////////////////////////////
map<string,unsigned int> nMatchedTot_;
map<string,unsigned int> nRecTot_;
map<string,unsigned int> nGenTot_;

////////////////////////////////////////////////////////////////////////////////
// define local functions
////////////////////////////////////////////////////////////////////////////////

//Get a complete gen2rec map of the jets
//Fill the dR based matching between the jets and the gen jets
map<Jet*, Jet*> matchRecToGen(TClonesArray *branchGenJet, TClonesArray *branchRecJet,
							  bool jets, string moduleName_);

//Print out some usefull information after the matching
void endMatching(string moduleName_);

//Initializes the counters used during the matching process
void initMatchingMaps(NtupleMap_t& recoJetMap);

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv) {
	gSystem->Load("libFWCoreFWLite.so");

   //
   // evaluate command-line / configuration file options
   // 
	CommandLine cl;
	if (!cl.parse(argc,argv)) return 0;

    TString         inputFilePath     = cl.getValue<TString>       ("inputFilePath");
	TString 	    inputFilename     = cl.getValue<TString> 	   ("inputFilename",        "");
	TString 	    outputDir         = cl.getValue<TString> 	   ("outputDir",          "./");
    //int             tnpu              = cl.getValue<int>           ("tnpu",                 20);
	int             maxEvts           = cl.getValue<int>           ("maxEvts",               0);
	//vector<TString> collections       = cl.getVector<TString>      ("collections", "GenJet:::CAJet:::Jet")    
	//bool    	 	doComposition_    = cl.getValue<bool>    	   ("doComposition",      true);
	//bool    	 	doFlavor_         = cl.getValue<bool>    	   ("doFlavor",           true);
	//bool    	 	doJetPt_          = cl.getValue<bool>    	   ("doJetPt",            true);
	//bool 		 	doRefPt_          = cl.getValue<bool>    	   ("doRefPt",            true);
	//bool 		 	doHLT_            = cl.getValue<bool>    	   ("doHLT",              true);
	unsigned int 	nRefMax_          = cl.getValue<unsigned int>  ("nRefMax",               0);
	double       	deltaRMax_        = cl.getValue<double>  	   ("deltaRMax",           0.0);
	double       	deltaPhiMin_      = cl.getValue<double>  	   ("deltaPhiMin",       3.141);
	//double       	deltaRPartonMax_  = cl.getValue<double>  	   ("deltaRPartonMax",     0.0);
 	bool         	doBalancing_      = cl.getValue<bool>    	   ("doBalancing",       false);
 	//bool         	getFlavorFromMap_ = cl.getValue<bool>   	   ("getFlavorFromMap_", false);
	//bool         	isCaloJet_;
	//bool         	isJPTJet_;
	//bool         	isPFJet_;
	//bool         	isTrackJet_;
	//bool         	isTauJet_;


	if (!cl.check()) return 0;
	cl.print();

	TBenchmark* m_benchmark = new TBenchmark();
	m_benchmark->Reset();
	m_benchmark->Start("event");

	if (deltaRMax_!=0.0) {
    	doBalancing_=false;
  	}
  	else if (deltaPhiMin_>=0) {
    	doBalancing_=true;
  	}
    doBalancing_ = doBalancing_;

  	// Create chain of root trees
  	TChain chain("Delphes");
  	//chain.Add(inputFilename);
    cout<<"\tAdding "<<inputFilePath+"/"+inputFilename+"*.root"<<endl;
    int file_count = chain.Add(inputFilePath+"/"+inputFilename+"*.root");
    if (file_count==0){
       cout << "\tNo files found!  Aborting.\n";
       return 0;
    }

	// Create object of class ExRootTreeReader
  	ExRootTreeReader *treeReader = new ExRootTreeReader(&chain);
  	Long64_t numberOfEntries = treeReader->GetEntries();

    // Turn off unwanted all branches and turn on only the ones we want
    treeReader->SetBranchStatus("*",0);
    treeReader->SetBranchStatus("RawJetNoPU*",1);
    treeReader->SetBranchStatus("RawJet*",1);
    treeReader->SetBranchStatus("GenJet*",1);
    treeReader->SetBranchStatus("Jet*",1);
    treeReader->SetBranchStatus("Rho*",1);
    treeReader->SetBranchStatus("NPU*",1);
    //treeReader->SetBranchStatus("EFlowTrack*",1);
    //treeReader->SetBranchStatus("EFlowTower*",1);
    //treeReader->SetBranchStatus("EFlowMuon*",1);

	// Get pointers to branches used in this analysis
	TClonesArray *branchRawJetNoPU = treeReader->UseBranch("RawJetNoPU");
  	TClonesArray *branchRawJet = treeReader->UseBranch("RawJet");
  	TClonesArray *branchGenJet = treeReader->UseBranch("GenJet");
  	TClonesArray *branchJet = treeReader->UseBranch("Jet");
	//TClonesArray *branchCAJet = treeReader->UseBranch("CAJet");
  	TClonesArray *branchRho = treeReader->UseBranch("Rho");
  	TClonesArray *branchNPU = treeReader->UseBranch("NPU");

  	// Constituents will be 0 otherwise
  	//TClonesArray *branchEFlowTrack = treeReader->UseBranch("EFlowTrack");
  	//TClonesArray *branchEFlowTower = treeReader->UseBranch("EFlowTower");
  	//TClonesArray *branchEFlowMuon = treeReader->UseBranch("EFlowMuon");

    // output file, tree, and branches
  	TFile* ofile = new TFile(outputDir+"/DelphesJRA.root","RECREATE");
  	ofile->mkdir("ak4pfchs","genRawJet");
  	ofile->mkdir("ak4pfchsl1","genRawNoPUJet");
  	ofile->mkdir("ak4pfchsrhocorrected","genJet");
  	//ofile->mkdir("ca8pf","genCAJet");
  	TTree* genRawJet_     = new TTree("t","t");
  	TTree* genRawNoPUJet_ = new TTree("t","t");
  	TTree* genJet_        = new TTree("t","t");
    //TTree* genCAJet_      = new TTree("t","t");
  	JRANtuple* genRawJet     = new JRANtuple(genRawJet_,true);
  	JRANtuple* genRawNoPUJet = new JRANtuple(genRawNoPUJet_,true);
  	JRANtuple* genJet        = new JRANtuple(genJet_,true);
  	//JRANtuple* genCAJet      = new JRANtuple(genCAJet_,true);

  	//Set vector of reco jet collections
  	NtupleMap_t recoJetMap;
  	recoJetMap["ak4pfchs"]       = make_pair(branchRawJet,     genRawJet);
  	recoJetMap["ak4pfchsl1"]     = make_pair(branchRawJetNoPU, genRawNoPUJet);
  	recoJetMap["ak4pfchsrhocorrected"] = make_pair(branchJet,        genJet);
  	//recoJetMap["ca8pf"]       = make_pair(branchCAJet,      genCAJet);

  	//Set up the matching maps
  	initMatchingMaps(recoJetMap);

	for(Int_t iEntry=0; iEntry<numberOfEntries; iEntry++) {
		if(maxEvts>0 && iEntry>=maxEvts) continue;
		treeReader->ReadEntry(iEntry);
		if(iEntry%10000==0)
			cout << "Doing entry " << iEntry << " ..." << endl;

		for (NtupleMap_t::iterator ialg=recoJetMap.begin(); ialg!=recoJetMap.end(); ++ialg) {
		//for(unsigned int ialg=0; ialg<recoJetMap.size(); ialg++) {

			//Do the gen2rec matching
			JetToJet_t refToJetMap = matchRecToGen(branchGenJet,ialg->second.first,
			                                       false,ialg->first);

			//Set values for genRawJet collection
	   		//size_t nRef=(nRefMax_==0) ? refToJetMap.size() : std::min(nRefMax_,(unsigned int)refToJetMap.size());
	   		size_t nRef=(nRefMax_==0) ? branchGenJet->GetEntries() : std::min(nRefMax_,(unsigned int)branchGenJet->GetEntries());
	   		//ialg->second.second->nref = branchGenJet->GetEntries();
	   		size_t nref_ = 0;
	   		for(size_t iRef=0; iRef<nRef; iRef++) {

	   			//if(iEntry==0)cout << "loop " << iRef << " in the gen jet section" << endl; 

	   			Jet* ref = (Jet*)branchGenJet->At(iRef);
	   			JetToJet_t::const_iterator itMatch = refToJetMap.find(ref);
	   			if (itMatch == refToJetMap.end()) continue;
	   			Jet* jet = itMatch->second;
	
	   			//Set values for the GenJet collection
	   			ialg->second.second->refrank[nref_]  = nref_;
				ialg->second.second->refpt[nref_]    = ref->PT;
				ialg->second.second->refeta[nref_]   = ref->Eta;
				ialg->second.second->refphi[nref_]   = ref->Phi;
				ialg->second.second->refe[nref_]     = ref->P4().E();
				ialg->second.second->refy[nref_]     = ref->P4().Rapidity();
				//ialg->second.second->refdrjt[nref_]  = TMath::Sqrt(TMath::Power(iDelphes.GenJet_DeltaEta[nref_],2)+
				//                                        TMath::Power(iDelphes.GenJet_DeltaPhi[nref_],2));
				ialg->second.second->refdrjt[nref_]  = ref->P4().DeltaR(jet->P4());
				ialg->second.second->refarea[nref_]  = ref->AreaP4().Pt();
				ialg->second.second->refpdgid[nref_] = 0;
				ialg->second.second->beta            = 0.0;//gen->Beta;
				ialg->second.second->betaStar        = 0.0;//gen->BetaStar;

				//Set values for the Jet collection
				ialg->second.second->jtpt[nref_]    = jet->PT;
				ialg->second.second->jteta[nref_]   = jet->Eta;
				ialg->second.second->jtphi[nref_]   = jet->Phi;
				ialg->second.second->jte[nref_]     = jet->P4().E();
				ialg->second.second->jty[nref_]     = jet->P4().Rapidity();
				ialg->second.second->jtjec[nref_]   = 0.0;
				ialg->second.second->jtarea[nref_]  = jet->AreaP4().Pt();
				ialg->second.second->jtchf[nref_]   = 0.0;
				ialg->second.second->jtnhf[nref_]   = 0.0;
				ialg->second.second->jtnef[nref_]   = 0.0;
				ialg->second.second->jtcef[nref_]   = 0.0;
				ialg->second.second->jtmuf[nref_]   = 0.0;
				ialg->second.second->jthfhf[nref_]  = 0.0;
				ialg->second.second->jthfef[nref_]  = 0.0;

				nref_++;
				ialg->second.second->nref = nref_;
	   		}

	   		//Set event based values
			ialg->second.second->evt     = iEntry;

			//Set values for the Rho Collection
	   		size_t nRho = branchRho->GetEntries();
            double avgRho = 0;
	   		for(size_t iRho=0; iRho<nRho; iRho++) {
               ScalarHT *rho = (ScalarHT*) branchRho->At(iRho);
               avgRho+=rho->HT;
            }
			ialg->second.second->rho     = avgRho/nRho;
			ialg->second.second->rho_hlt = 0;

			//Set values for the NPU Collection
			ScalarHT *NPU = (ScalarHT*) branchNPU->At(0);
			int nPUvertices_true = (int)NPU->HT;
            //ialg->second.second->tnpus = new vector<float>(3,0);
            //(*ialg->second.second->tnpus)[0] = nPUvertices_true;
            //(*ialg->second.second->tnpus)[1] = nPUvertices_true;
            //(*ialg->second.second->tnpus)[2] = nPUvertices_true;
            ialg->second.second->bxns->clear();
            ialg->second.second->bxns->push_back(-1);
            ialg->second.second->bxns->push_back(0);
            ialg->second.second->bxns->push_back(1);
            ialg->second.second->npus->clear();
            ialg->second.second->npus->push_back(nPUvertices_true);
            ialg->second.second->npus->push_back(nPUvertices_true);
            ialg->second.second->npus->push_back(nPUvertices_true);
            ialg->second.second->tnpus->clear();
            //The bins in a poison distribution used to determine the pileup added to an event
            ialg->second.second->tnpus->push_back(nPUvertices_true);
            ialg->second.second->tnpus->push_back(nPUvertices_true);
            ialg->second.second->tnpus->push_back(nPUvertices_true);
            //ialg->second.second->tnpus.push_back(tnpu);
            //ialg->second.second->tnpus.push_back(tnpu);
            //ialg->second.second->tnpus.push_back(tnpu);

            //Set values for sumpt
            ialg->second.second->sumpt_lowpt->clear();
            ialg->second.second->sumpt_lowpt->push_back(0);
            ialg->second.second->sumpt_lowpt->push_back(0);
            ialg->second.second->sumpt_lowpt->push_back(0);
            ialg->second.second->sumpt_highpt->clear();
            ialg->second.second->sumpt_highpt->push_back(0);
            ialg->second.second->sumpt_highpt->push_back(0);
            ialg->second.second->sumpt_highpt->push_back(0);

            //Set values for ntrks
            ialg->second.second->ntrks_lowpt->clear();
            ialg->second.second->ntrks_lowpt->push_back(0);
            ialg->second.second->ntrks_lowpt->push_back(0);
            ialg->second.second->ntrks_lowpt->push_back(0);
            ialg->second.second->ntrks_highpt->clear();
            ialg->second.second->ntrks_highpt->push_back(0);
            ialg->second.second->ntrks_highpt->push_back(0);
            ialg->second.second->ntrks_highpt->push_back(0);

			//Set values for missing collections
			ialg->second.second->pthat  = 0;
			ialg->second.second->weight = 0;
            if(ialg->first == "ak4pfchsl1")
               ialg->second.second->npv    = (int)(1);
            else
               ialg->second.second->npv    = (int)(nPUvertices_true/0.74);
			ialg->second.second->run    = 1;

			ofile->cd(ialg->first.c_str());
			ialg->second.second->fChain->Fill();
		}
	}

	for (NtupleMap_t::iterator ialg=recoJetMap.begin(); ialg!=recoJetMap.end(); ++ialg) {
		ofile->cd(ialg->first.c_str());
		ialg->second.second->fChain->Write();
		endMatching(ialg->first);
	}

	ofile->cd();
	ofile->Close();

	m_benchmark->Stop("event"); 
	cout << "jet_correction_analyzer_x" << endl << "\tCPU time = " << m_benchmark->GetCpuTime("event") << " s" << endl
	<< "\tReal time = " << m_benchmark->GetRealTime("event") << " s" << endl;
	delete m_benchmark;

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
map<Jet*, Jet*> matchRecToGen(TClonesArray *branchGenJet, TClonesArray *branchRecJet,
							  bool jets, string moduleName_) {

	//Can add more sophistication by checking that:
	// 1) the particle status==3
	// 2) the gen jet pt > 0
	// 3) if (jets && (fabs(genParticle->pdgId())>6 && fabs(genParticle->pdgId())!=21)) continue;

  	unsigned int nRec(0);
	unsigned int nGen(0);
	unsigned int nMatched(0);

	nRec = std::min((size_t)branchGenJet->GetEntries(),(size_t)100);
  	nGen = std::min((size_t)branchRecJet->GetEntries(),(size_t)100);  

	IndexSet_t iRecSet;
  	IndexSet_t iGenSet;
  	MatchSet_t matchSet;

	for (unsigned int i=0;i<nRec;i++) iRecSet.insert(i);
  	for (unsigned int i=0;i<nGen;i++) iGenSet.insert(i);

	for(Int_t iRec=0; iRec<branchRecJet->GetEntries(); iRec++) {

		Jet* rec = (Jet*) branchRecJet->At(iRec);

		for(Int_t iGen=0; iGen<branchGenJet->GetEntries(); iGen++) {

			Jet* gen = (Jet*) branchGenJet->At(iGen);

			//double deltaR = reco::deltaR(rec,gen);
			double deltaR = reco::deltaR(rec->Eta,rec->Phi,gen->Eta,gen->Phi);
			matchSet.insert(make_pair(deltaR,make_pair(iRec,iGen)));
		}
	}

	JetToJet_t recToGenMap;
	JetToJet_t genToRecMap;

	MatchIter_t it=matchSet.begin();
	while (it!=matchSet.end()&&iRecSet.size()>0&&iGenSet.size()>0) {
    	unsigned int iRec  = it->second.first;
    	unsigned int iGen  = it->second.second;
    	IndexIter_t  itRec = iRecSet.find(iRec);
    	IndexIter_t  itGen = iGenSet.find(iGen);
    
    	if (itRec!=iRecSet.end()&&itGen!=iGenSet.end()) {
      		nMatched++;
      		//recToGenMap.insert((Jet*)branchRecJet->At(iRec),(Jet*)branchGenJet->At(iGen));
      		//genToRecMap.insert((Jet*)branchGenJet->At(iGen),(Jet*)branchRecJet->At(iRec));
      		recToGenMap[(Jet*)branchRecJet->At(iRec)] = (Jet*)branchGenJet->At(iGen);
      		genToRecMap[(Jet*)branchGenJet->At(iGen)] = (Jet*)branchRecJet->At(iRec);
      		iRecSet.erase(itRec);
      		iGenSet.erase(itGen);
    	}
    
    	++it;
  	}

  	nRecTot_[moduleName_] += nRec;
  	nGenTot_[moduleName_] += nGen;
  	nMatchedTot_[moduleName_] += nMatched;

  	/*
    //Double check that each rec jet has a gen jet match. If not, put a placeholder in.
   	for (unsigned int iRec = 0; iRec<jp4.size(); iRec++) {
    	if (jetMap.find(iRec)!=jetMap.end())
        	continue;
      	else {
        	cout << "matchRecToGen::WARNING Did not find gen particle match for reco jet index " << iRec << endl;
        	jetMap[iRec] = -1-iRec;
      	}
   	}
   	*/

   return genToRecMap;

}//matchRecToGen

//______________________________________________________________________________
void endMatching(string moduleName_) {
	stringstream ss;
  	ss<<"nMatched = "<<nMatchedTot_[moduleName_]<<"\n"
    	<<"nRec = "<<nRecTot_[moduleName_]<<" "
    	<<"fRec = "<<100.*(nMatchedTot_[moduleName_]/(double)nRecTot_[moduleName_])<<" %\n"
    	<<"nGen = "<<nGenTot_[moduleName_]<<" "
    	<<"fGen = "<<100.*(nMatchedTot_[moduleName_]/(double)nGenTot_[moduleName_])<<" %\n";
  	edm::LogPrint("Summary")
    	<<"++++++++++++++++++++++++++++++++++++++++++++++++++"
    	<<"\n"<<moduleName_<<"(matchRecToGen) SUMMARY:\n"<<ss.str()
    	<<"++++++++++++++++++++++++++++++++++++++++++++++++++";
}

//______________________________________________________________________________
void initMatchingMaps(NtupleMap_t& recoJetMap) {
	for (NtupleMap_t::iterator ialg=recoJetMap.begin(); ialg!=recoJetMap.end(); ++ialg) {
		nMatchedTot_[ialg->first] = 0;
		nRecTot_[ialg->first] = 0;
		nGenTot_[ialg->first] = 0;	
	}
}

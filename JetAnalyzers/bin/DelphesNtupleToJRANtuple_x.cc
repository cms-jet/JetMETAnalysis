///////////////////////////////////////////////////////////////////
//
// DelphesNtupleToJRANtuple_x
// --------------------------
//
//            01/16/2014 Alexx Perloff  <aperloff@physics.tamu.edu>
///////////////////////////////////////////////////////////////////

#include "JetMETAnalysis/JetAnalyzers/interface/Settings.h"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/JRAEvent.h"
#include "JetMETAnalysis/JetUtilities/interface/ProgressBar.hh"

#include "classes/DelphesClasses.h"
#include "external/ExRootAnalysis/ExRootTreeReader.h"

#include "DataFormats/Math/interface/deltaR.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
//https://github.com/xrootd/xrootd/blob/master/src/XrdCl/XrdClFileSystem.hh#L621
//https://github.com/xrootd/xrootd/blob/master/src/XrdCl/XrdClXRootDResponses.hh
#include "xrootd/XrdCl/XrdClFileSystem.hh"

#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TFileCollection.h"
#include "THashList.h"
#include "TTree.h"
#include "TChain.h"
#include "TMath.h"
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
typedef map<string,pair<TClonesArray*,JRAEvent*> >             NtupleMap_t;

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

    string       inputFilePath  = cl.getValue<string>       ("inputFilePath");
	string 	 	 inputFilename  = cl.getValue<string> 	    ("inputFilename",                 "");
	string       fileList       = cl.getValue<string>       ("fileList",                      "");
    string       url_string     = cl.getValue<string>       ("url",                           "");
	string 	     outputDir      = cl.getValue<string> 	    ("outputDir",                   "./");
	string       outputFilename = cl.getValue<string>       ("outputFilename", "DelphesJRA.root");
	int          maxEvts        = cl.getValue<int>          ("maxEvts",                        0);
	unsigned int nRefMax_       = cl.getValue<unsigned int> ("nRefMax",                        0);

	if (!cl.check()) return 0;
	cl.print();

	TBenchmark* m_benchmark = new TBenchmark();
	m_benchmark->Reset();
	m_benchmark->Start("event");

  	// Create chain of root trees
  	int file_count(0);
  	TChain chain("Delphes");
  	if(!fileList.empty()) {
  		cout<<"\tAdding files from the list " << inputFilePath << "/" << fileList<<endl;
  		TFileCollection fc("fc","",(inputFilePath+"/"+fileList).c_str());
  		chain.AddFileInfoList((TCollection*)fc.GetList());
  		if(chain.GetListOfFiles()->GetEntries()!=fc.GetNFiles()) {
  			cout << "ERROR::DelphesNtupleToJRANtuple_x::main Something went wrong and the number of files in the filesList doesn't equal the number of files in the chain." << endl;
			return -1;  			
  		}
  		file_count = chain.GetListOfFiles()->GetEntries();
  	}
    else if(!url_string.empty()) {
       XrdCl::DirectoryList *response;
       XrdCl::DirListFlags::Flags flags = XrdCl::DirListFlags::None;
       XrdCl::URL url(url_string);
       XrdCl::FileSystem fs(url);
       fs.DirList(inputFilePath,flags,response);
       for(auto iresp=response->Begin(); iresp!=response->End(); iresp++) {
          if((*iresp)->GetName().find(".root")!=std::string::npos) {
             cout << "\tAdding " << url_string << inputFilePath << (*iresp)->GetName() << endl;
             file_count = chain.Add((url_string+inputFilePath+(*iresp)->GetName()).c_str());
          }
       }
    }
  	else {
	    cout<<"\tAdding "<<inputFilePath+"/"+inputFilename+"*.root"<<endl;
    	file_count = chain.Add((inputFilePath+"/"+inputFilename+"*.root").c_str());
    }
    if (file_count==0){
       cout << "\tNo files found!  Aborting.\n";
       return 0;
    }

    // Turn off unwanted all branches and turn on only the ones we want
    chain.SetBranchStatus("*",0);
    chain.SetBranchStatus("Event.*",1);
    chain.SetBranchStatus("GenJet.*",1);
    chain.SetBranchStatus("Jet.*",1);
    chain.SetBranchStatus("JetPF.*",1);
    chain.SetBranchStatus("JetPUPPI.*",1);
    chain.SetBranchStatus("Vertex.*",1);
    if(chain.FindBranch("Rho"))
	    chain.SetBranchStatus("Rho.*",1);

  	// Create object of class ExRootTreeReader
  	ExRootTreeReader *treeReader = new ExRootTreeReader(&chain);
  	Long64_t numberOfEntries = (maxEvts>0) ? maxEvts : treeReader->GetEntries();

	// Get pointers to branches used in this analysis
	TClonesArray *branchEvent    = treeReader->UseBranch("Event");
  	TClonesArray *branchGenJet   = treeReader->UseBranch("GenJet");
  	TClonesArray *branchJet      = treeReader->UseBranch("Jet");
  	TClonesArray *branchJetPF    = treeReader->UseBranch("JetPF");
  	TClonesArray *branchJetPUPPI = treeReader->UseBranch("JetPUPPI");
  	TClonesArray *branchVertex   = treeReader->UseBranch("Vertex");
  	TClonesArray *branchRho      = treeReader->UseBranch("Rho");

    // open the output file
  	TFile* ofile = TFile::Open((outputDir+"/"+outputFilename).c_str(),"RECREATE");

  	//Make output directories and ntuples and setup the map of reco jet collections
  	NtupleMap_t recoJetMap;
  	if(branchJetPF) {
	  	ofile->mkdir("ak4pf", "genJetPF");
	  	JRAEvent* genJetPF      = new JRAEvent(0,85);
	  	recoJetMap["ak4pf"] = make_pair(branchJetPF, genJetPF);
	}
  	if(branchJet) {
	  	ofile->mkdir("ak4pfchs", "genJet");
	  	JRAEvent* genJetPFchs   = new JRAEvent(0,85);
	  	recoJetMap["ak4pfchs"] = make_pair(branchJet, genJetPFchs);
	}
  	if(branchJetPUPPI) {
  		ofile->mkdir("ak4puppi", "genJetPUPPI");
  		JRAEvent* genJetPFPuppi = new JRAEvent(0,85);
	  	recoJetMap["ak4puppi"] = make_pair(branchJetPUPPI, genJetPFPuppi);
	}

  	//Set up the matching maps
  	initMatchingMaps(recoJetMap);

	for(Int_t iEntry=0; iEntry<numberOfEntries; iEntry++) {
		treeReader->ReadEntry(iEntry);
		loadbar2(iEntry+1,numberOfEntries,50,"\t");

		for (NtupleMap_t::iterator ialg=recoJetMap.begin(); ialg!=recoJetMap.end(); ++ialg) {
			//Clear the JRAEvent from the previous event
			ialg->second.second->clear();

			//Do the gen2rec matching
			JetToJet_t refToJetMap = matchRecToGen(branchGenJet,ialg->second.first,false,ialg->first);

			//Set values for genRawJet collection
	   		size_t nRef=(nRefMax_==0) ? branchGenJet->GetEntries() : std::min(nRefMax_,(unsigned int)branchGenJet->GetEntries());
	   		size_t nref_ = 0;
	   		for(size_t iRef=0; iRef<nRef; iRef++) {

	   			Jet* ref = (Jet*)branchGenJet->At(iRef);
	   			JetToJet_t::const_iterator itMatch = refToJetMap.find(ref);
	   			if (itMatch == refToJetMap.end()) continue;
	   			Jet* jet = itMatch->second;
	
	   			//Set values for the GenJet collection
	   			ialg->second.second->refrank->push_back(nref_);
				ialg->second.second->refpt->push_back(ref->PT);
				ialg->second.second->refeta->push_back(ref->Eta);
				ialg->second.second->refphi->push_back(ref->Phi);
				ialg->second.second->refe->push_back(ref->P4().E());
				ialg->second.second->refy->push_back(ref->P4().Rapidity());
				ialg->second.second->refdrjt->push_back(ref->P4().DeltaR(jet->P4()));
				ialg->second.second->refarea->push_back(ref->Area.Pt());
				ialg->second.second->refpdgid->push_back(0);
				ialg->second.second->beta            = 0.0;//gen->Beta;
				ialg->second.second->betaStar        = 0.0;//gen->BetaStar;

				//Set values for the Jet collection
				ialg->second.second->jtpt->push_back(jet->PT);
				ialg->second.second->jteta->push_back(jet->Eta);
				ialg->second.second->jtphi->push_back(jet->Phi);
				ialg->second.second->jte->push_back(jet->P4().E());
				ialg->second.second->jty->push_back(jet->P4().Rapidity());
				ialg->second.second->jtjec->push_back(0.0);
				ialg->second.second->jtarea->push_back(jet->Area.Pt());
				ialg->second.second->jtchf->push_back(0.0);
				ialg->second.second->jtnhf->push_back(0.0);
				ialg->second.second->jtnef->push_back(0.0);
				ialg->second.second->jtcef->push_back(0.0);
				ialg->second.second->jtmuf->push_back(0.0);
				ialg->second.second->jthfhf->push_back(0.0);
				ialg->second.second->jthfef->push_back(0.0);

				nref_++;
				ialg->second.second->nref = nref_;
	   		}
	   		if(ialg->second.second->refrank->size()==0) ialg->second.second->nref = 0;

			//Set values for the Rho Collection
			if(branchRho) {
		   		size_t nRho = branchRho->GetEntries();
	            double avgRho = 0;
		   		for(size_t iRho=0; iRho<nRho; iRho++) {
	               ScalarHT *rho = (ScalarHT*) branchRho->At(iRho);
	               avgRho+=rho->HT;
	            }
				ialg->second.second->rho     = avgRho/nRho;
			}

			//Set values for the NPU Collection
			int nPUvertices_true = (int)branchVertex->GetEntries();
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
            ialg->second.second->npv    = std::max(1,(int)(nPUvertices_true*0.74));

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

			//Set event based values
			HepMCEvent* event = (HepMCEvent*)branchEvent->At(0);
			ialg->second.second->run    = 1;
			ialg->second.second->lumi   = 1;
			ialg->second.second->evt    = event->Number;
			ialg->second.second->pthat  = 0;
			ialg->second.second->weight = event->Weight;

			ofile->cd(ialg->first.c_str());
			ialg->second.second->fChain->Fill();
		}
	}

	cout << endl;
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

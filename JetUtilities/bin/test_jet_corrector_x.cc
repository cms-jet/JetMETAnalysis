#include <map>
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <assert.h>
#include <algorithm>
#include <cstdio>

#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TBenchmark.h"

#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"

using namespace std;

void benchmarkJCP(unsigned int nBenchmarkJCP, string theBenchmark) {
	bool old = false;
	TString basepath = "/home/aperloff/fdata/JEC/81X_StudyHighPtClosure/";
	TString filename = (old) ? basepath+"/textfiles/StudyHighPtClosure_L1FastJet_AK4PFchs.txt" : basepath+"beforeL1/THnSparseSpline/THnSparseTest_25nsV1_MC_L1FastJet_ak4pfchs.txt";
	JetCorrectorParameters* L1JetPar = new JetCorrectorParameters(filename.Data());
	vector<float> fX = {5.0,1.5,4.0};
	//L1JetPar->binIndex(fX);
	//L1JetPar->binIndex3(fX);

	float oldCPU = 0, newCPU = 0, oldReal = 0, newReal = 0;

	TBenchmark* m_benchmark = new TBenchmark();
	m_benchmark->Reset();
	unsigned int ntests = nBenchmarkJCP;
	if(theBenchmark == "old" || theBenchmark == "both") {
		m_benchmark->Start("event");
		for(unsigned int i=0; i<ntests; i++) {
			L1JetPar->binIndex(fX);
		}
		m_benchmark->Stop("event");
		cout << "test_jetCorrector" << endl << "\tCPU time = " << m_benchmark->GetCpuTime("event")/double(ntests) << " s" << endl << "\tReal time = " << m_benchmark->GetRealTime("event")/double(ntests) << " s" << endl;;
		oldCPU = m_benchmark->GetCpuTime("event")/double(ntests);
		oldReal = m_benchmark->GetRealTime("event")/double(ntests);
		m_benchmark->Reset();
	}
	if(theBenchmark == "new" || theBenchmark == "both") {
		m_benchmark->Start("event");
		for(unsigned int i=0; i<ntests; i++) {
			L1JetPar->binIndexN(fX);
		}
		m_benchmark->Stop("event");
		cout << "test_jetCorrector" << endl << "\tCPU time = " << m_benchmark->GetCpuTime("event")/double(ntests) << " s" << endl << "\tReal time = " << m_benchmark->GetRealTime("event")/double(ntests) << " s" << endl;
		newCPU = m_benchmark->GetCpuTime("event")/double(ntests);
		newReal = m_benchmark->GetRealTime("event")/double(ntests);
	}
	if(theBenchmark == "both") {
		cout << "test_jetCorrector" << endl << "\tCPU speedup = " << oldCPU/newCPU << endl << "\tReal speedup = " << oldReal/newReal << endl;
	}
}

void compareJCP() {
	bool old = false;
	TString basepath = "/home/aperloff/fdata/JEC/81X_StudyHighPtClosure/";
	TString filename = (old) ? basepath+"/textfiles/StudyHighPtClosure_L1FastJet_AK4PFchs.txt" : basepath+"beforeL1/THnSparseSpline/THnSparseTest_25nsV1_MC_L1FastJet_ak4pfchs.txt";
	JetCorrectorParameters* L1JetPar = new JetCorrectorParameters(filename.Data());
	vector<float> fX = {0.0,0.0,0.0};

	//for(float i=-5.191+0.091; i<5.191; i+=0.2) {
    for(float i=5.0; i<5.191; i+=0.1) {
		for(float j=1.5; j<36; j++) {
			for(float k=4.0; k<4000; k++) {
				fX = {i,j,k};
				int oldBin = L1JetPar->binIndex(fX);
				int newBin = L1JetPar->binIndexN(fX);
				if((oldBin < 0 && newBin >= 0) || (oldBin >= 0 && newBin < 0)) {
					cout << "ERROR::compareJCP Unable to find the right bin for (eta,rho,pt)=(" << i << "," << j << "," << k << ")" << endl
						 << "\t(oldBin,newBin)=(" << oldBin << "," << newBin << ")" << endl;
					return;
				}
				else if(oldBin!=newBin) {
					cout << "ERROR::compareJCP oldBin!=newBin (" << oldBin << "!=" << newBin << ") for (eta,rho,pt)=("
					     << i << "," << j << "," << k << ")" << endl;
					return;
				}
				else {
					if(int(k)%100==0)
						cout << "(eta,rho,pt)=(" << i << "," << j << "," << k << ")" << endl;
				}
			}
		}
	}
	cout << "compareJCP All bins match between the linear and non-linear search algorithms." << endl;
}

void compareJC(string ifilename) {
	bool old = false;
	TString basepath = "/home/aperloff/fdata/JEC/81X_StudyHighPtClosure/";
	TString filename = (old) ? basepath+"/textfiles/StudyHighPtClosure_L1FastJet_AK4PFchs.txt" : basepath+"beforeL1/THnSparseSpline/THnSparseTest_25nsV1_MC_L1FastJet_ak4pfchs.txt";
	JetCorrectorParameters* L1JetPar = new JetCorrectorParameters(filename.Data());
	vector<JetCorrectorParameters> vPar;
	vPar.push_back(*L1JetPar);
	FactorizedJetCorrector* jetCorrector = new FactorizedJetCorrector(vPar);
	jetCorrector->setJetEta(-5.1);
	jetCorrector->setRho(1.1);
	jetCorrector->setJetA(0.5);
	jetCorrector->setJetPt(4.0);
	jetCorrector->getCorrection();
}

//______________________________________________________________________________
//void testJetCorrector(unsigned int nBenchmarkJCP = 0, bool doCompareJCP = false, bool doCompareJC = false, string ifilename = "") {
int main(int argc,char**argv){

	CommandLine cl;
    if (!cl.parse(argc,argv)) return 0;
    unsigned int nBenchmarkJCP = cl.getValue<unsigned int> ("nBenchmarkJCP",     0);
    string       theBenchmark  = cl.getValue<string>       ("theBenchmark", "both");
    bool         doCompareJCP  = cl.getValue<bool>         ("doCompareJCP",  false);
    bool         doCompareJC   = cl.getValue<bool>         ("doCompareJC",   false);
    string       ifilename     = cl.getValue<string>       ("ifilename",        "");
    if (!cl.check()) return 0;
    cl.print();

	if(nBenchmarkJCP > 0) benchmarkJCP(nBenchmarkJCP, theBenchmark);
	if(doCompareJCP) compareJCP();
	if(doCompareJC && !ifilename.empty()) compareJC(ifilename);

	return 0;
}

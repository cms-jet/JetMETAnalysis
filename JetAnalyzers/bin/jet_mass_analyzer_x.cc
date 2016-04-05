////////////////////////////////////////////////////////////////////////////////
//
// jet_mass_analyzer_x
// -------------------
//
//            22/11/2009 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/JRAEvent.h"

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TKey.h>
#include <TLorentzVector.h>

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <cmath>


using namespace std;


////////////////////////////////////////////////////////////////////////////////
// define local functions
////////////////////////////////////////////////////////////////////////////////

/// get the suffix for the histogram name, e.g. JetPt100to150
string get_suffix(const string& varname,int ibin,const vector<float>& bins);

/// get the index of the histogram corresponding to x
int get_index(float x,const vector<float>& binsx);

/// fill the appropriate histogram (histos), based on x and binsx
void fill_histo(float value,float weight,float x,
		const vector<float>& binsx,const vector<TH1F**>& histos);

/// check if a vector of strings contains a certain element
bool contains(const vector<string>& collection,const string& element);

/// interpret legs_as_vstr parameter, translating values like 'zbb','top',etc.
void interpret_legs(vector<string>& legs_as_vstr,
		    string& particle, float& massmin,float& massmax);

/// convert legs from string to vint
bool legs_from_string(const string& legs_as_string,vector<int>& legs);

/// charge-conjugate vint of signed pdgids (legs)
vector<int> charge_conjugate(const vector<int>& legs);

/// check if the pdgid corresponds to something valid
bool is_valid(int pdgid);


//______________________________________________________________________________
int main(int argc,char**argv)
{
  //
  // evaluate command-line / configuration file options
  //
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;

  string         input        = cl.getValue<string> ("input");
  string         particle     = cl.getValue<string> ("particle",     "Z");
  string         output       = cl.getValue<string> ("output","jma.root");
  bool           useweight    = cl.getValue<bool>   ("useweight",  false);
  float          xsection     = cl.getValue<float>  ("xsection",     0.0);
  float          drmax        = cl.getValue<float>  ("drmax",        0.3);
  int            nbinsmass    = cl.getValue<int>    ("nbinsmass",     50);
  float          massmin      = cl.getValue<float>  ("massmin",     50.0);
  float          massmax      = cl.getValue<float>  ("massmax",    150.0);
  float          jtptmin      = cl.getValue<float>  ("jtptmin",      1.0);
  vector<string> algs         = cl.getVector<string>("algs",          "");
  vector<string> legs_as_vstr = cl.getVector<string>("legs",      "5:-5");
  
  if (!cl.check()) return 0;
  cl.print();
  
  TH1::SetDefaultSumw2(true);
  
  
  //
  // evaluate drmin requirements for individual algorithms if provided
  //
  map<std::string,float> alg2drmax;
  for (unsigned int ialg=0;ialg<algs.size();ialg++) {
    string alg=algs[ialg];
    size_t pos=alg.find(':');
    if (pos!=string::npos) {
      float drmax_alg; stringstream ss; ss<<alg.substr(pos+1); ss>>drmax_alg;
      alg=alg.substr(0,pos);
      alg2drmax[alg]=drmax_alg;
      algs[ialg]=alg;
    }
  }
  

  //
  // evaluate legs (=jets to construct mass resonance from)
  //
  interpret_legs(legs_as_vstr,particle,massmin,massmax);
  vector< vector<int> > legs;
  for (unsigned ileg=0;ileg<legs_as_vstr.size();ileg++) {
    string legs_as_str = legs_as_vstr[ileg];
    vector<int> ilegs;
    if (!legs_from_string(legs_as_str,ilegs)) {cout<<"Buh!"<<endl; return 0;}
    legs.push_back(ilegs);
    if (ilegs.size()>2||ilegs[0]!=-ilegs[1])
      legs.push_back(charge_conjugate(ilegs));
  }
  
  
  //
  // open input/output files and loop over input directories/trees (=algorithms!)
  //
  TFile* ifile = new TFile(input.c_str(),"READ");
  if (!ifile->IsOpen()) {  cout<<"Can't open "<<input<<endl; return 0; }
  
  TFile* ofile = new TFile(output.c_str(),"RECREATE");
  if (!ofile->IsOpen()) { cout<<"Can't create "<<output<<endl; return 0; }
  
  TIter next(ifile->GetListOfKeys());
  TKey* key(0);
  while ((key=(TKey*)next())) {
    if (strcmp(key->GetClassName(),"TDirectoryFile")!=0) continue;
    
    TDirectoryFile* idir = (TDirectoryFile*)key->ReadObj();
    string alg(idir->GetName());
    if (algs.size()>0&&!contains(algs,alg)) continue;
    
    cout<<alg<<" ... "<<endl;

    TTree* tree = (TTree*)idir->Get("t");
    if (0==tree) { cout<<"no tree found."<<endl; continue; }
  
    float drmax_alg = drmax;
    if (alg2drmax.find(alg)!=alg2drmax.end()) drmax_alg=alg2drmax[alg];
    

    //
    // setup the tree for reading
    //
    float weight(1.0);
    JRAEvent* JRAEvt = new JRAEvent(tree,85);
    tree->SetBranchStatus("*",0);
    vector<string> branch_names = {"nref","weight","refpdgid","refe","refpt","refeta",
                                   "refphi","jte","jtpt","jteta","jtphi","refdrjt"};
    for(auto n : branch_names) {
      if(n=="weight") {
        if (xsection>0.0) { weight = xsection/tree->GetEntries(); useweight = false; }
        if (useweight) {
            if (0==tree->GetBranch(n.c_str()))
                cout<<"branch 'weight' not found, events will NOT be weighted!"<<endl;
            else
                tree->SetBranchStatus(n.c_str(),1);
        }
        continue;
      }
      tree->SetBranchStatus(n.c_str(),1);
    }
    
    //
    // create directory in output file and book histograms
    //
    TDirectoryFile* odir = (TDirectoryFile*)ofile->mkdir(alg.c_str());
    if (0==odir) { cout<<"failed to create directory."<<endl; continue; }
    odir->cd();
    
    // declare histograms
    vector<TH1F*> jetMass;
    vector<TH1F*> refMass;
    
    
    // book mass histograms
    jetMass.push_back(new TH1F("JetMass",(";m_{"+particle+"} [GeV]").c_str(),
			       nbinsmass,massmin,massmax));
    refMass.push_back(new TH1F("RefMass",(";m^{gen}_{"+particle+"} [GeV]").c_str(),
			       nbinsmass,massmin,massmax));

    
    //
    // fill histograms
    //
    unsigned nevt = (unsigned)tree->GetEntries();
    for (unsigned ievt=0;ievt<nevt;ievt++) {
      tree->GetEntry(ievt);
      if(useweight) weight = JRAEvt->weight;

      for (unsigned ileg=0;ileg<legs.size();ileg++) {
	vector<int> ilegs = legs[ileg];
	vector<TLorentzVector> refs;
	vector<TLorentzVector> jets;
	for (unsigned char iref=0;iref<JRAEvt->nref;iref++) {
	  if (JRAEvt->refdrjt->at(iref)>drmax_alg) continue;
	  if (JRAEvt->jtpt->at(iref)<jtptmin) continue;
	  int id = JRAEvt->refpdgid->at(iref);
	  vector<int>::iterator itid = find(ilegs.begin(),ilegs.end(),id);
	  if (itid!=ilegs.end()) {
	    TLorentzVector ref;
	    ref.SetPtEtaPhiE(JRAEvt->refpt->at(iref),JRAEvt->refeta->at(iref),JRAEvt->refphi->at(iref),JRAEvt->refe->at(iref));
	    refs.push_back(ref);
	    TLorentzVector jet;
	    jet.SetPtEtaPhiE(JRAEvt->jtpt->at(iref),JRAEvt->jteta->at(iref),JRAEvt->jtphi->at(iref),JRAEvt->jte->at(iref));
	    jets.push_back(jet);
	    ilegs.erase(itid);
	  }
	}
	if (refs.size()==legs[ileg].size()) {
	  TLorentzVector refSum;
	  for (unsigned i=0;i<refs.size();i++) refSum += refs[i];
	  refMass.back()->Fill(refSum.M(),weight);
	  TLorentzVector jetSum;
	  for (unsigned i=0;i<jets.size();i++) jetSum += jets[i];
	  jetMass.back()->Fill(jetSum.M(),weight);
	}
      }
    }
    
    cout<<" DONE."<<endl;
  }


  //
  // close files
  //
  cout<<"close output file "<<output<<" ... "<<flush;
  ofile->Write();
  gROOT->GetListOfFiles()->Remove(ofile);
  ofile->Close();
  delete ofile;
  cout<<"DONE."<<endl;
  
  ifile->Close();
  delete ifile;

  return 0;
}



////////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
string get_suffix(const string& varname,int ibin,const vector<float>& bins)
{
  stringstream ss; ss<<varname<<bins[ibin]<<"to"<<bins[ibin+1];
  return ss.str();
}


//______________________________________________________________________________
int get_index(float x,const vector<float>& binsx)
{
  for (unsigned int ix=0;ix<binsx.size()-1;ix++)
    if (x>=binsx[ix]&&x<binsx[ix+1]) return ix;
  return -1;
}


//______________________________________________________________________________
void fill_histo(float value,float weight,float x,
		const vector<float>& binsx,const vector<TH1F**>& histos)
{
  if (binsx.size()==0) return;
  int ix=get_index(x,binsx);
  if (ix>=0) histos[ix][0]->Fill(value,weight);
}

//______________________________________________________________________________
bool contains(const vector<string>& collection,const string& element)
{
  vector<string>::const_iterator it;
  for (it=collection.begin();it!=collection.end();++it)
    if ((*it)==element) return true;
  return false;
}


//______________________________________________________________________________
void interpret_legs(vector<string>& legs_as_vstr,
		    string& particle,float& massmin, float& massmax)
{
  if (legs_as_vstr[0]=="zbb") {
    legs_as_vstr.clear();
    particle = "Z";
    massmin = 50;
    massmax = 150;
    legs_as_vstr.push_back("-5:5");
    legs_as_vstr.push_back("-511:5");
    legs_as_vstr.push_back("-513:5");
    legs_as_vstr.push_back("-511:511");
    legs_as_vstr.push_back("-511:513");
    legs_as_vstr.push_back("-513:513");
  }
  else if (legs_as_vstr[0]=="zbb_oneslb") {
    legs_as_vstr.clear();
    particle = "Z";
    massmin = 50;
    massmax = 150;
    legs_as_vstr.push_back("-511:5");
    legs_as_vstr.push_back("-513:5");
    legs_as_vstr.push_back("-511:511");
    legs_as_vstr.push_back("-511:513");
    legs_as_vstr.push_back("-513:513");
  }
  else if (legs_as_vstr[0]=="zbb_bothslb") {
    legs_as_vstr.clear();
    particle = "Z";
    massmin = 50;
    massmax = 150;
    legs_as_vstr.push_back("-511:511");
    legs_as_vstr.push_back("-511:513");
    legs_as_vstr.push_back("-513:513");
  }
  else if (legs_as_vstr[0]=="wqq") {
    legs_as_vstr.clear();
    particle = "W";
    massmin = 40;
    massmax = 140;
    legs_as_vstr.push_back("1:-2");
    legs_as_vstr.push_back("3:-4");
    legs_as_vstr.push_back("3:-411");
    legs_as_vstr.push_back("3:-413");
  }
  else if (legs_as_vstr[0]=="tbqq") {
    legs_as_vstr.clear();
    particle = "top";
    massmin = 120;
    massmax = 220;
    legs_as_vstr.push_back("1:-2:-5");
    legs_as_vstr.push_back("3:-4:-5");
    legs_as_vstr.push_back("3:-411:-5");
    legs_as_vstr.push_back("3:-413:-5");
    legs_as_vstr.push_back("1:-2:-511");
    legs_as_vstr.push_back("3:-4:-511");
    legs_as_vstr.push_back("3:-411:-511");
    legs_as_vstr.push_back("3:-413:-511");
    legs_as_vstr.push_back("1:-2:-513");
    legs_as_vstr.push_back("3:-4:-513");
    legs_as_vstr.push_back("3:-411:-513");
    legs_as_vstr.push_back("3:-413:-513");
  }
  else if (legs_as_vstr[0]=="tbqq_nosl") {
    legs_as_vstr.clear();
    particle = "top";
    massmin = 120;
    massmax = 220;
    legs_as_vstr.push_back("1:-2:-5");
    legs_as_vstr.push_back("3:-4:-5");
  }
  else if (legs_as_vstr[0]=="tbqq_slb") {
    legs_as_vstr.clear();
    particle = "top";
    massmin = 120;
    massmax = 220;
    legs_as_vstr.push_back("1:-2:-511");
    legs_as_vstr.push_back("3:-4:-511");
    legs_as_vstr.push_back("3:-411:-511");
    legs_as_vstr.push_back("3:-413:-511");
    legs_as_vstr.push_back("1:-2:-513");
    legs_as_vstr.push_back("3:-4:-513");
    legs_as_vstr.push_back("3:-411:-513");
    legs_as_vstr.push_back("3:-413:-513");
  }
  
  return;
}


//______________________________________________________________________________
bool legs_from_string(const string& legs_as_str, vector<int>& legs)
{
  legs.clear();
  string tmp(legs_as_str);

  while (!tmp.empty()) {
    size_t pos = tmp.find(':');
    stringstream ss; ss<<tmp.substr(0,pos); int id; ss>>id;
    if (!is_valid(id)) {
      cout<<"invalid pdgid: "<<id<<endl;
      return false;
    }
    legs.push_back(id);
    tmp = (pos==string::npos) ? "" : tmp.substr(pos+1);
  }
  
  return (legs.size()>1) ? true : false;
}


//______________________________________________________________________________
vector<int> charge_conjugate(const vector<int>& legs)
{
  vector<int> result;
  for (unsigned i=0;i<legs.size();i++) result.push_back(-legs[i]);
  return result;
}


//______________________________________________________________________________
bool is_valid(int pdgid)
{
  int absid = std::abs(pdgid);
  if (absid==0)  return false;
  if (absid<6)   return true;
  if (absid==21) return true;
  if (absid==411||absid==413||absid==511||absid==513) return true;
  return false;
}

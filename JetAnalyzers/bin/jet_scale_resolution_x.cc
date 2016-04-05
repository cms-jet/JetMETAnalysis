////////////////////////////////////////////////////////////////////////////////
//
// jet_scale_resolution_x
// ----------------------
//
//            05/05/2010 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"

#include <TSystem.h>
#include <TFile.h>
#include <TDirectory.h>
#include <TTree.h>
#include <TKey.h>
#include <TF2.h>

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <cassert>
#include <algorithm>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// helpers
////////////////////////////////////////////////////////////////////////////////

/// order function for sort algorithm
bool largerthan (Float_t i,Float_t j) { return (i>j); }
bool smallerthan (Float_t i,Float_t j) { return (i<j); }

/// setup a map to specified eta bins (or one large dummy bin)
bool setup_mapEtaToFnc(const string& dir,const vector<string>& vv,
		       const string& globalfnc,const vector<float>& globalpars,
		       map<string,TF2*>& map);

/// print the setup of this map
void print_map_setting(const map<string,TF2*>& mm);

/// setup a vector for the eta bins - to speed up map matching later
bool setup_etabins(const map<string,TF2*>& mapfnc, vector<float>& etabins);

/// retrieve the name of a eta bin associated with jteta according to etabins
string get_etabin(const vector<float>& etabins,const float jteta);

/// helper to decode dirname information
bool decode_dirinfo(const string& dirinfo,string& fnc,vector<Float_t>& pars);

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv)
{
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;
  
  vector<string> inputs    = cl.getVector<string>("inputs");
  string         scalefnc  = cl.getValue<string> ("scalefnc", "[0]");
  vector<float>  scalepars = cl.getVector<float> ("scalepars","0.1");  
  string         treename  = cl.getValue<string> ("treename",   "t");
  vector<string> dirname   = cl.getVector<string>("dirname",     "");
  string         datapath  = cl.getValue<string> ("datapath",   ".");
 
  if(!cl.check()) return 0;
  cl.print();
  
  //
  // loop over all input files
  //

  for (unsigned iinput=0;iinput<inputs.size();++iinput) {

    // open input file
    string input = inputs[iinput]; input = datapath + "/" + input;
    TFile* ifile = new TFile(input.c_str(),"READ");
    if (!ifile->IsOpen()) { cout<<"Failed to open "<<input<<endl; return 0; }

    // create temporary output file
    size_t pos = input.find(".root");
    string output = input.substr(0,pos);
    output = output + ".jer.root";
    TFile* ofile = new TFile(output.c_str(),"RECREATE");
    if (!ofile->IsOpen()) { cout<<"Failed to create "<<output<<endl; return 0; }

    // loop over input directories
    TIter next(ifile->GetListOfKeys());
    TKey* key(0);
    while ((key=(TKey*)next())) {

      if (strcmp(key->GetClassName(),"TDirectoryFile")!=0) continue;

      TDirectory* idir    = (TDirectory*)key->ReadObj();
      string      idirname = idir->GetName();

      // fnc / pars for this dir or global; maybe in bins(!) of eta?

      map<string,TF2*> mapEtaToFnc;
      if (!setup_mapEtaToFnc(idirname,dirname,scalefnc,scalepars,mapEtaToFnc)) continue; 

      cout<<input<<"/"<<idirname<<":"<<endl;
      print_map_setting(mapEtaToFnc);

      vector<float> etabins;
      if (!setup_etabins(mapEtaToFnc,etabins)) {cout<<"Error while building eta bins\n";continue;}
      cout<<"Please verify eta binning: ";
      for (unsigned i=0;i<etabins.size();++i) cout<<etabins[i]<<"  ";
      cout<<endl;

      // create output directory
      TDirectory* odir = (TDirectory*)ofile->mkdir(idirname.c_str());
      if(0==odir){cout<<"Failed to make "<<idirname<<" in "<<output<<endl;return 0;}

      // open input tree
      TTree* itree = (TTree*)idir->Get(treename.c_str());
      if(0==itree){cout<<"No "<<treename<<" in "<<output<<":"<<idirname<<endl;return 0;}

      // don't copy dijets branches if they already exist
      itree->SetBranchStatus("jtpt",0);
      itree->SetBranchStatus("jte", 0);

      // clone input tree into output directory
      odir->cd();
      TTree* otree = itree->CloneTree();
      cout<<output<<"/"<<idirname<<"/"<<treename<<" created (temporary!)"<<endl;

      itree->SetBranchStatus("jtpt",1);
      itree->SetBranchStatus("jte", 1);
    
      // create dijet branches in output tree
      vector<Float_t>*  ojtpt = new vector<float>;
      vector<Float_t>*  ojte = new vector<float>;

      TBranch* b_jtpt=otree->Branch("jtpt", "vector<Float_t>", &ojtpt);
      TBranch* b_jte =otree->Branch("jte", "vector<Float_t>", &ojte);

      // setup input branches needed

      UChar_t           nref;
      vector<Float_t>*  refpt;
      vector<Float_t>*  jtpt;
      vector<Float_t>*  jteta;
      vector<Float_t>*  jte;

      itree->SetBranchAddress("nref", &nref);
      itree->SetBranchAddress("refpt", &refpt);
      itree->SetBranchAddress("jtpt",  &jtpt);
      itree->SetBranchAddress("jteta", &jteta);
      itree->SetBranchAddress("jte", &jte);
    
      unsigned nevt= static_cast<unsigned>(itree->GetEntries());
      
      for (unsigned ievt=0;ievt<nevt;ievt++) {
	itree->GetEntry(ievt);

	for (unsigned int ijt=0;ijt<nref;ijt++) {
	  
	  float scale   = 1.0;

	  map<string,TF2*>::const_iterator itmm = 
	    mapEtaToFnc.find(get_etabin(etabins,jteta->at(ijt)));
	  if (itmm!=mapEtaToFnc.end()) 
	    scale = itmm->second->Eval(jtpt->at(ijt),jteta->at(ijt));
	  else {
	      cout<<"Error; scaling 1. - evt: "<<ievt<<" ijt: "<<(int)ijt
		  <<" eta: "<<jteta->at(ijt)<<endl;
	  }

	  float deltapt = (jtpt->at(ijt)-refpt->at(ijt))*scale;
	  float ptscale = std::max( (float)0.0 , (jtpt->at(ijt)+deltapt)/jtpt->at(ijt) );
	  ojtpt->push_back(jtpt->at(ijt)*ptscale);
	  ojte ->push_back(jte->at(ijt)*ptscale);

	} //ijt

	b_jtpt->Fill();
	b_jte ->Fill();

      } // ievt...

    } // while key=nextDir
  
    cout<<"Save resolution-scaled tree to "<<output<<" ... "<<flush;
    ifile->Close();
    delete ifile;
    ofile->Write();
    ofile->Close();
    delete ofile;
    cout<<"DONE!\n\n";

  } // ifile

  return 0;
}


//______________________________________________________________________________
void print_map_setting(const map<string,TF2*>& mm)
{
  std::map<string,TF2*>::const_iterator itmap = mm.begin();
  for (;itmap!=mm.end();++itmap) {
    cout<<itmap->first<<" : "<<itmap->second->GetExpFormula();
    for (int i=0;i<itmap->second->GetNumberFreeParameters();++i)
      cout<<" : "<<itmap->second->GetParameter(i);
    cout<<endl;
  }
}

//______________________________________________________________________________
bool setup_mapEtaToFnc(const string& dir,const vector<string>& vv,
		       const string& globalfnc,const vector<float>& globalpars,
		       map<string,TF2*>& map)
{
  // if nothing is specified in dirname take global and go on
  if (vv.size()==0) {
    cout<<"No dirspecific inputs - taking global function for one large eta bin\n";
    map["JetEta0to999"] = new TF2("fscale_JetEta0to999",globalfnc.c_str(),1.0,1000.,-5.0,5.0);
    TF2* fnc = map.find("JetEta0to999")->second; assert (fnc);

    if (fnc->GetNumberFreeParameters()!=(int)globalpars.size()) {
	cout<<"Scale fnc par mismatch; expected: "<<fnc->GetNumberFreeParameters()
	    <<" and got: "<<globalpars.size()<<endl; return false;
    }
    for (unsigned ipar=0;ipar<globalpars.size();++ipar)
      fnc->SetParameter(ipar,globalpars[ipar]);

    return true;
  } //vv.size==0

  // if there are dir specifics fill appropriately
  for (unsigned ivv=0;ivv<vv.size();++ivv) {
    string tmp(vv[ivv]);

    if (tmp.find(dir.c_str())==string::npos) continue;

    size_t pos = tmp.find(":");
    if (pos==string::npos) {
      cout<<"Scale fnc setup failed: dirname specified, but no details found\n";
      return false;
    }

    tmp = tmp.substr(pos+1);
    pos = tmp.find(":");

    string etabin = "";

    if (map.size()==0&&tmp.find("JetEta")==string::npos) 
      {etabin = "JetEta0to999";}
    else if (map.size()!=0&&tmp.find("JetEta")==string::npos)
      {cout<<"Scale fnc setup error\n";return false;}
    else {etabin = tmp.substr(0,pos); tmp = tmp.substr(pos+1); pos = tmp.find(":");}

    string dirfnc(""); vector<float> dirpars;
    if (!decode_dirinfo(tmp,dirfnc,dirpars)) {
      cout<<"Scale fnc setup error while retrieving fnc/pars!\n"; return false;
    }

    map[etabin] =  new TF2(("fscale_"+etabin).c_str(),dirfnc.c_str(),1.0,1000.,-5.0,5.0);
    TF2* fnc = map.find(etabin)->second; assert (fnc);

    if (fnc->GetNumberFreeParameters()!=(int)dirpars.size()) {
	cout<<"Scale fnc par mismatch; expected: "<<fnc->GetNumberFreeParameters()
	    <<" and got: "<<dirpars.size()<<endl; return false;
    }
    for (unsigned ipar=0;ipar<dirpars.size();++ipar)
      fnc->SetParameter(ipar,dirpars[ipar]);

  }

  if (map.size()==0) return false;
  return true;
}

//______________________________________________________________________________
string get_etabin(const vector<float>& etabins,const float jteta)
{
  const float eta = (etabins[0]>=0.0) ? std::abs(jteta) : jteta;
  string etabin   = "";
  for (unsigned i=0;i<etabins.size()-1;++i) {
    if (etabins[i]<=eta && etabins[i+1]>eta) {
      stringstream sstmp; sstmp<<"JetEta"<<etabins[i]<<"to"<<etabins[i+1];
      etabin = sstmp.str(); break;
    }
  }
  return etabin;
}

//______________________________________________________________________________
bool setup_etabins(const map<string,TF2*>& mapfnc, vector<float>& etabins)
{
  std::map<string,TF2*>::const_iterator itmap = mapfnc.begin();
  etabins.push_back(0.0);

  for (;itmap!=mapfnc.end();++itmap) {
    string tmp(itmap->first);
    tmp = tmp.substr(tmp.find("to")+2);
    stringstream sstmp;sstmp<<tmp;float ftmp; sstmp>>ftmp;assert (!isnan(ftmp));
    etabins.push_back(ftmp);
  }

  std::sort (etabins.begin(),etabins.end(),smallerthan);
  return true;
}

//______________________________________________________________________________
bool decode_dirinfo(const string& dirinfo,string& fnc,vector<Float_t>& pars)
{
  if (dirinfo.empty()) {cout<<"Error; no dirinfo\n";return false;}

  string tmp(dirinfo);
  size_t pos = tmp.find(":");

  // assert that TMath::Sign goes through in function string...:/
  while (tmp.substr(pos,pos+2).find("::")!=string::npos) pos = tmp.find(":",pos+2);

  if (pos==string::npos) return false;

  fnc = tmp.substr(0,pos);
  if (pos==string::npos) return true;

  while (pos!=string::npos) {
    tmp = tmp.substr(pos+1);
    pos = tmp.find(":");
    stringstream ss; ss<<tmp.substr(0,pos); 
    Float_t par;     ss>>par; assert (!isnan(par));
    pars.push_back(par);
  }
  return true;
}

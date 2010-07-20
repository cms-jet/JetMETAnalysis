////////////////////////////////////////////////////////////////////////////////
//
// jet_response_analyzer_x
// -----------------------
//
// DESCRIPTION: jet_response_analyzer_x takes a file written with
// the CMSSW fwk analyzer JetResponseAnalyzer, and turns the trees
// in each directory (for each algorithm) into histograms. The exact
// same result can be achieved with the fwk analyzer directly! But if
// you decide to operate the fwk analyzer with doTree=true,
// doHistos=false, you can fit the output e.g. on a local disk and
// decide on the cuts and binning now. A lot more flexibility, for a
// lot more disk space ...
//
//            07/23/2008 Kostas Kousouris                    <kkousour@fnal.gov>
//                       Vasundhara Chetluru                 <vasurang@fnal.gov>
//                       Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TKey.h>

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <cmath>
#include <cassert>


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

/// fill the appropriate histogram (histos), based on pdgid, x and binsx
void fill_histo(int pdgid,float value,float weight,float x,
		const vector<float>& binsx,const vector<TH1F**>& histos);

/// fill the appropriate histogram (histos), based on x, y, binsx, and binsy
void fill_histo(float value,float weight,float x,float y,
		const vector<float>& binsx,const vector<float>& binsy,
		const vector<TH1F***>& histos);

/// fill the appropriate histogram (histos), based on pdgid, x, y, binsx, and binsy
void fill_histo(int pdgid,float value,float weight,float x,float y,
		const vector<float>& binsx,const vector<float>& binsy,
		const vector<TH1F***>& histos);

/// check if a vector of strings contains a certain element
bool contains(const vector<string>& collection,const string& element);

/// compute deltaPhi between two jets - copied from DataFormats/Math/interface/
inline float deltaPhi(float phi1, float phi2) { 
  float result = phi1 - phi2;
  while (result > float(M_PI)) result -= float(2*M_PI);
  while (result <= -float(M_PI)) result += float(2*M_PI);
  return result;
}


//______________________________________________________________________________
int main(int argc,char**argv)
{
  //
  // evaluate command-line / configuration file options
  //
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;

  string         input        = cl.getValue<string> ("input");
  vector<float>  binspt       = cl.getVector<float> ("binspt",        "");
  vector<float>  binseta      = cl.getVector<float> ("binseta",       "");
  vector<float>  binsphi      = cl.getVector<float> ("binsphi",       "");
  vector<float>  binsy        = cl.getVector<float> ("binsy",         "");
  vector<float>  binsdipt     = cl.getVector<float> ("binsdipt",      "");
  string         treename     = cl.getValue<string> ("treename",     "t");
  string         output       = cl.getValue<string> ("output","jra.root");
  bool           useweight    = cl.getValue<bool>   ("useweight",  false);
  float          xsection     = cl.getValue<float>  ("xsection",     0.0);
  int            nrefmax      = cl.getValue<int>    ("nrefmax",        0);
  int            nbinspt      = cl.getValue<int>    ("nbinspt",       50);
  int            nbinsdipt    = cl.getValue<int>    ("nbinsdipt",     50);
  int            nbinseta     = cl.getValue<int>    ("nbinseta",      25);
  int            nbinsphi     = cl.getValue<int>    ("nbinsphi",      25);
  int            nbinsy       = cl.getValue<int>    ("nbinsy",        25);
  float          etabarrelmin = cl.getValue<float>  ("etabarrelmin",-1.3);
  float          etabarrelmax = cl.getValue<float>  ("etabarrelmax",+1.3);
  bool           dobalance    = cl.getValue<bool>   ("dobalance",  false);
  bool           doflavor     = cl.getValue<bool>   ("doflavor",   false);
  float          drmax        = cl.getValue<float>  ("drmax",        0.3);
  float          dphimin      = cl.getValue<float>  ("dphimin",      2.7);
  float          jetrecdidphimin = cl.getValue<float>  ("jetrecdidphimin",-1.0);
  bool           dojetpt      = cl.getValue<bool>   ("dojetpt",    false);
  bool           dorefpt      = cl.getValue<bool>   ("dorefpt",     true);
  bool           dojetdipt    = cl.getValue<bool>   ("dojetdipt",  false);
  bool           dorefdipt    = cl.getValue<bool>   ("dorefdipt",  false);
  int            nbinsrelrsp  = cl.getValue<int>    ("nbinsrelrsp",   50);
  float          relrspmin    = cl.getValue<float>  ("relrspmin",    0.0);
  float          relrspmax    = cl.getValue<float>  ("relrspmax",    2.0);
  int            nbinsabsrsp  = cl.getValue<int>    ("nbinsabsrsp",    0);
  float          absrspmin    = cl.getValue<float>  ("absrspmin", -250.0);
  float          absrspmax    = cl.getValue<float>  ("absrspmax",  100.0);
  int            nbinsetarsp  = cl.getValue<int>    ("nbinsetarsp",  100);
  float          etarspmin    = cl.getValue<float>  ("etarspmin",   -1.0);
  float          etarspmax    = cl.getValue<float>  ("etarspmax",    1.0);
  int            nbinsphirsp  = cl.getValue<int>    ("nbinsphirsp",  100);
  float          phirspmin    = cl.getValue<float>  ("phirspmin",   -1.0);
  float          phirspmax    = cl.getValue<float>  ("phirspmax",    1.0);
  float          jtptmin      = cl.getValue<float>  ("jtptmin",      1.0);
  vector<string> algs         = cl.getVector<string>("algs",          "");
  float          maxextrarefpt= cl.getValue<float>  ("maxextrarefpt",-1.0);
  float          minextrarefpt= cl.getValue<float>  ("minextrarefpt",-1.0);
  float          maxextrajetpt= cl.getValue<float>  ("maxextrajetpt",-1.0);
  float          minextrajetpt= cl.getValue<float>  ("minextrajetpt",-1.0);
  float          mindijetpt   = cl.getValue<float>  ("mindijetpt",  -1.0);

  if (!cl.check()) return 0;
  cl.print();

  if (maxextrarefpt!=-1.0 && minextrarefpt!=-1.0) {cout<<"Error; min & max extrapt are set"<<endl;return 0;}

  if (maxextrajetpt!=-1. && nrefmax!=2) {cout<<"Error; maxextrajetpt option so far only for dijets!"<<endl; return 0;}
  if (minextrajetpt!=-1. && nrefmax!=2) {cout<<"Error; minextrajetpt option so far only for dijets!"<<endl; return 0;}

  bool dorelrsp=(nbinsrelrsp>0);
  bool doabsrsp=(nbinsabsrsp>0);
  bool doetarsp=(nbinsetarsp>0);
  bool dophirsp=(nbinsphirsp>0);

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
      //cout<<"drmax("<<alg<<") = "<<alg2drmax[alg]<<endl;
    }
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
    unsigned char nref;
    float weight(1.0);
    int   refpdgid[100];
    float refpt[100];
    float refeta[100];
    float refphi[100];
    //float refy[100];
    float jtpt[100];
    float jteta[100];
    float jtphi[100];
    float jty[100];
    float refdrjt[100];
    float refdphijt[100];
    
    tree->SetBranchAddress("nref",   &nref);
    if (doflavor) tree->SetBranchAddress("refpdgid",refpdgid);
    tree->SetBranchAddress("refpt",   refpt);
    tree->SetBranchAddress("refeta",  refeta);
    tree->SetBranchAddress("refphi",  refphi);
    //tree->SetBranchAddress("refy",    refy);
    tree->SetBranchAddress("jtpt",    jtpt);
    tree->SetBranchAddress("jteta",   jteta);
    tree->SetBranchAddress("jtphi",   jtphi);
    tree->SetBranchAddress("jty",     jty);
    
    if (xsection>0.0) { weight = xsection/tree->GetEntries(); useweight = false; }
    if (useweight) {
      if (0==tree->GetBranch("weight"))
	cout<<"branch 'weight' not found, events will NOT be weighted!"<<endl;
      else
	tree->SetBranchAddress("weight",&weight);
    }
    
    if (dobalance) {
      if (0==tree->GetBranch("refdphijt")) {
	cout<<"dobalance, but no branch 'refdphijt' in tree, skip!"<<endl;
	continue;
      }
      else tree->SetBranchAddress("refdphijt",refdphijt);
    }
    else {
      if (0==tree->GetBranch("refdrjt")) {
	cout<<"!dobalance, but no branch 'refdrjt' in tree, skip!"<<endl;
	continue;
      }
      else tree->SetBranchAddress("refdrjt",refdrjt);
    }
    
    
    //
    // create directory in output file and book histograms
    //
    TDirectoryFile* odir = (TDirectoryFile*)ofile->mkdir(alg.c_str());
    if (0==odir) { cout<<"failed to create directory."<<endl; continue; }
    odir->cd();
    


    // declare histograms
    vector<TH1F**>  jetPtVsJetPt;
    vector<TH1F**>  refPtVsRefPt;
    vector<TH1F**>  jetPtVsRefPt;
    //hh
    vector<TH1F**>  jetDiPtVsJetDiPt;
    vector<TH1F**>  refDiPtVsRefDiPt;
    vector<TH1F**>  jetDiPtVsRefDiPt;

    vector<TH1F**>  refPtVsRefPtBarrel;
    vector<TH1F**>  jetPtVsRefPtBarrel;
    //hh
    vector<TH1F**>  refDiPtVsRefDiPtBarrel;
    vector<TH1F**>  jetDiPtVsRefDiPtBarrel;

    vector<TH1F**>  jetEtaVsJetEta;
    vector<TH1F**>  jetPhiVsJetPhi;
    vector<TH1F**>  jetYVsJetY;


    vector<TH1F***> jetPtVsJetEtaJetPt;
    vector<TH1F***> refPtVsJetEtaRefPt;
    vector<TH1F***> jetPtVsJetEtaRefPt;
    //hh
    vector<TH1F***> jetDiPtVsJetEtaJetDiPt;
    vector<TH1F***> refDiPtVsJetEtaRefDiPt;
    vector<TH1F***> jetDiPtVsJetEtaRefDiPt;

    vector<TH1F***> jetPtVsJetYJetPt;
    vector<TH1F***> refPtVsJetYRefPt;
    vector<TH1F***> jetPtVsJetYRefPt;
    //hh
    vector<TH1F***> jetDiPtVsJetYJetDiPt;
    vector<TH1F***> refDiPtVsJetYRefDiPt;
    vector<TH1F***> jetDiPtVsJetYRefDiPt;


    vector<TH1F**>  relRspVsJetPt;
    vector<TH1F**>  relRspVsRefPt;
    vector<TH1F**>  relRspVsRefPtBarrel;
    //hh
    vector<TH1F**>  relRspVsJetDiPt;
    vector<TH1F**>  relRspVsRefDiPt;
    vector<TH1F**>  relRspVsRefDiPtBarrel;

    vector<TH1F**>  relRspVsJetEta;
    vector<TH1F**>  relRspVsJetPhi;
    vector<TH1F**>  relRspVsJetY;

    vector<TH1F***> relRspVsJetEtaJetPt;
    vector<TH1F***> relRspVsJetEtaRefPt;
    //hh
    vector<TH1F***> relRspVsJetEtaJetDiPt;
    vector<TH1F***> relRspVsJetEtaRefDiPt;

    vector<TH1F***> relRspVsJetYJetPt;
    vector<TH1F***> relRspVsJetYRefPt;
    //hh
    vector<TH1F***> relRspVsJetYJetDiPt;
    vector<TH1F***> relRspVsJetYRefDiPt;

    
    vector<TH1F**>  absRspVsJetPt;
    vector<TH1F**>  absRspVsRefPt;
    vector<TH1F**>  absRspVsRefPtBarrel;
    //hh
    vector<TH1F**>  absRspVsJetDiPt;
    vector<TH1F**>  absRspVsRefDiPt;
    vector<TH1F**>  absRspVsRefDiPtBarrel;

    vector<TH1F**>  absRspVsJetEta;
    vector<TH1F**>  absRspVsJetPhi;
    vector<TH1F**>  absRspVsJetY;

    vector<TH1F***> absRspVsJetEtaJetPt;
    vector<TH1F***> absRspVsJetEtaRefPt;
    //hh
    vector<TH1F***> absRspVsJetEtaJetDiPt;
    vector<TH1F***> absRspVsJetEtaRefDiPt;

    vector<TH1F***> absRspVsJetYJetPt;
    vector<TH1F***> absRspVsJetYRefPt;
    //hh
    vector<TH1F***> absRspVsJetYJetDiPt;
    vector<TH1F***> absRspVsJetYRefDiPt;


    vector<TH1F**>  etaRspVsJetPt;
    vector<TH1F**>  etaRspVsRefPt;
    //hh
    vector<TH1F**>  etaRspVsJetDiPt;
    vector<TH1F**>  etaRspVsRefDiPt;

    vector<TH1F**>  etaRspVsJetEta;
    vector<TH1F**>  etaRspVsJetPhi;
    vector<TH1F***> etaRspVsJetEtaJetPt;
    vector<TH1F***> etaRspVsJetEtaRefPt;
    //hh
    vector<TH1F***> etaRspVsJetEtaJetDiPt;
    vector<TH1F***> etaRspVsJetEtaRefDiPt;

    vector<TH1F**>  phiRspVsJetPt;
    vector<TH1F**>  phiRspVsRefPt;
    //hh
    vector<TH1F**>  phiRspVsJetDiPt;
    vector<TH1F**>  phiRspVsRefDiPt;

    vector<TH1F**>  phiRspVsJetEta;
    vector<TH1F**>  phiRspVsJetPhi;
    vector<TH1F***> phiRspVsJetEtaJetPt;
    vector<TH1F***> phiRspVsJetEtaRefPt;
    //hh
    vector<TH1F***> phiRspVsJetEtaJetDiPt;
    vector<TH1F***> phiRspVsJetEtaRefDiPt;


    // define flavors
    vector<string> flavor;
    flavor.push_back("");
    if (doflavor) {
      flavor.push_back("uds_");
      flavor.push_back("c_");
      flavor.push_back("b_");
      flavor.push_back("g_");
      flavor.push_back("slc_");
      flavor.push_back("slb_");
    }
    

    // book pT histograms
    if (binspt.size()>=2) {
      for (unsigned int ipt=0;ipt<binspt.size()-1;++ipt) {
	
	string hname; float ptmin=binspt[ipt]; float ptmax=binspt[ipt+1];
	
	if (dojetpt) {
	  jetPtVsJetPt.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"JetPt_"+get_suffix("JetPt",ipt,binspt);
	    jetPtVsJetPt.back()[iflv]=new TH1F(hname.c_str(),";p_{T} [GeV]",
					       nbinspt,ptmin,ptmax);
	  }
	}
	
	if (dorefpt) {
	  refPtVsRefPt.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"RefPt_"+get_suffix("RefPt",ipt,binspt);
	    refPtVsRefPt.back()[iflv]=new TH1F(hname.c_str(),";p_{T}^{ref} [GeV]",
					       nbinspt,ptmin,ptmax);
	  }
	}
	
	if (dorefpt) {
	  jetPtVsRefPt.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"JetPt_"+get_suffix("RefPt",ipt,binspt);
	    jetPtVsRefPt.back()[iflv]=new TH1F(hname.c_str(),";p_{T} [GeV]",
					       3*nbinspt,
					       0,
					       3.0*ptmax);
	  }
	}
	
	if (dorefpt) {
	  refPtVsRefPtBarrel.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"RefPt_Barrel_"+get_suffix("RefPt",ipt,binspt);
	    refPtVsRefPtBarrel.back()[iflv]=new TH1F(hname.c_str(),
						     ";p_{T}^{ref} [GeV]",
						     nbinspt,ptmin,ptmax);
	  }
	}
	
	if (dorefpt) {
	  jetPtVsRefPtBarrel.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"JetPt_Barrel_"+get_suffix("RefPt",ipt,binspt);
	    jetPtVsRefPtBarrel.back()[iflv]=new TH1F(hname.c_str(),
						     ";p_{T} [GeV]",
						     3*nbinspt,
						     0,
						     3.0*ptmax);
	  }
	}
	
	if (dorelrsp&&dojetpt) {
	  relRspVsJetPt.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"RelRsp_"+get_suffix("JetPt",ipt,binspt);
	    relRspVsJetPt.back()[iflv]=new TH1F(hname.c_str(),";p_{T}/p_{T}^{ref}",
						nbinsrelrsp,relrspmin,relrspmax);
	  }
	}

	if (dorelrsp&&dorefpt) {
	  relRspVsRefPt.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"RelRsp_"+get_suffix("RefPt",ipt,binspt);
	    relRspVsRefPt.back()[iflv]=new TH1F(hname.c_str(),";p_{T}/p_{T}^{ref}",
						nbinsrelrsp,relrspmin,relrspmax);
	  }
	}

	if (dorelrsp&&dorefpt) {
	  relRspVsRefPtBarrel.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"RelRsp_Barrel_"+get_suffix("RefPt",ipt,binspt);
	    relRspVsRefPtBarrel.back()[iflv]=new TH1F(hname.c_str(),
						      ";p_{T}/p_{T}^{ref}",
						      nbinsrelrsp,
						      relrspmin,relrspmax);
	  }
	}
	
	if (doabsrsp&&dojetpt) {
	  absRspVsJetPt.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"AbsRsp_"+get_suffix("JetPt",ipt,binspt);
	    absRspVsJetPt.back()[iflv]=new TH1F(hname.c_str(),
						";p_{T}-p_{T}^{ref} [GeV]",
						nbinsabsrsp,absrspmin,absrspmax);
	  }
	}
	
	if (doabsrsp&&dorefpt) {
	  absRspVsRefPt.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"AbsRsp_"+get_suffix("RefPt",ipt,binspt);
	    absRspVsRefPt.back()[iflv]=new TH1F(hname.c_str(),
						";p_{T}-p_{T}^{ref} [GeV]",
						nbinsabsrsp,absrspmin,absrspmax);
	  }
	}
	
	if (doabsrsp&&dorefpt) {
	  absRspVsRefPtBarrel.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"AbsRsp_Barrel_"+get_suffix("RefPt",ipt,binspt);
	    absRspVsRefPtBarrel.back()[iflv]=new TH1F(hname.c_str(),
						      ";p_{T}-p_{T}^{ref} [GeV]",
						      nbinsabsrsp,
						      absrspmin,absrspmax);
	  }
	}

	if (doetarsp&&dojetpt) {
	  etaRspVsJetPt.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"EtaRsp_"+get_suffix("JetPt",ipt,binspt);
	    etaRspVsJetPt.back()[iflv]=new TH1F(hname.c_str(),
						";|#eta|-|#eta^{ref}|",
						nbinsetarsp,etarspmin,etarspmax);
	  }
	}
	
	if (doetarsp&&dorefpt) {
	  etaRspVsRefPt.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"EtaRsp_"+get_suffix("RefPt",ipt,binspt);
	    etaRspVsRefPt.back()[iflv]=new TH1F(hname.c_str(),
						";|#eta|-|#eta^{ref}|",
						nbinsetarsp,etarspmin,etarspmax);
	  }
	}
	
	if (dophirsp&&dojetpt) {
	  phiRspVsJetPt.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"PhiRsp_"+get_suffix("JetPt",ipt,binspt);
	    phiRspVsJetPt.back()[iflv]=new TH1F(hname.c_str(),
						";#phi-#phi^{ref}",
						nbinsphirsp,phirspmin,phirspmax);
	  }
	}
	
	if (dophirsp&&dorefpt) {
	  phiRspVsRefPt.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"PhiRsp_"+get_suffix("RefPt",ipt,binspt);
	    phiRspVsRefPt.back()[iflv]=new TH1F(hname.c_str(),
						";#phi-#phi^{ref}",
						nbinsphirsp,phirspmin,phirspmax);
	  }
	}
	
	
      }
    }



    // book DI-pT histograms
    if (binsdipt.size()>=2) {

      for (unsigned int idipt=0;idipt<binsdipt.size()-1;++idipt) {
	
	string hname; float diptmin=binsdipt[idipt]; float diptmax=binsdipt[idipt+1];
	
	if (dojetdipt) {jetDiPtVsJetDiPt.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"JetDiPt_"+get_suffix("JetDiPt",idipt,binsdipt);
	    jetDiPtVsJetDiPt.back()[iflv]=new TH1F(hname.c_str(),";p_{T}^{ave} [GeV]",
						   nbinsdipt,diptmin,diptmax);
	  }
	}
	
	if (dorefdipt) {
	  refDiPtVsRefDiPt.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"RefDiPt_"+get_suffix("RefDiPt",idipt,binsdipt);
	    refDiPtVsRefDiPt.back()[iflv]=new TH1F(hname.c_str(),";p_{T}^{ave,ref} [GeV]",
						   nbinsdipt,diptmin,diptmax);
	  }
	}
	
	if (dorefdipt) {
	  jetDiPtVsRefDiPt.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"JetDiPt_"+get_suffix("RefDiPt",idipt,binsdipt);
	    jetDiPtVsRefDiPt.back()[iflv]=new TH1F(hname.c_str(),";p_{T}^{ave} [GeV]",
						   3*nbinsdipt,
						   0,
						   3.0*diptmax);
	  }
	}
	
	if (dorefdipt) {
	  refDiPtVsRefDiPtBarrel.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"RefDiPt_Barrel_"+get_suffix("RefDiPt",idipt,binsdipt);
	    refDiPtVsRefDiPtBarrel.back()[iflv]=new TH1F(hname.c_str(),
							 ";p_{T}^{ave,ref} [GeV]",
							 nbinsdipt,diptmin,diptmax);
	  }
	}
	
	if (dorefdipt) {
	  jetDiPtVsRefDiPtBarrel.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"JetPt_Barrel_"+get_suffix("RefDiPt",idipt,binsdipt);
	    jetDiPtVsRefDiPtBarrel.back()[iflv]=new TH1F(hname.c_str(),
							 ";p_{T}^{ave} [GeV]",
							 3*nbinsdipt,
							 0,
							 3.0*diptmax);
	  }
	}
	
	if (dorelrsp&&dojetdipt) {
	  relRspVsJetDiPt.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"RelRsp_"+get_suffix("JetDiPt",idipt,binsdipt);
	    relRspVsJetDiPt.back()[iflv]=new TH1F(hname.c_str(),";p_{T}/p_{T}^{ref}",
						  nbinsrelrsp,relrspmin,relrspmax);
	  }
	}

	if (dorelrsp&&dorefdipt) {
	  relRspVsRefDiPt.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"RelRsp_"+get_suffix("RefDiPt",idipt,binsdipt);
	    relRspVsRefDiPt.back()[iflv]=new TH1F(hname.c_str(),";p_{T}/p_{T}^{ref}",
						  nbinsrelrsp,relrspmin,relrspmax);
	  }
	}

	if (dorelrsp&&dorefdipt) {
	  relRspVsRefDiPtBarrel.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"RelRsp_Barrel_"+get_suffix("RefDiPt",idipt,binsdipt);
	    relRspVsRefDiPtBarrel.back()[iflv]=new TH1F(hname.c_str(),
							";p_{T}/p_{T}^{ref}",
							nbinsrelrsp,
							relrspmin,relrspmax);
	  }
	}
	
	if (doabsrsp&&dojetdipt) {
	  absRspVsJetDiPt.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"AbsRsp_"+get_suffix("JetDiPt",idipt,binsdipt);
	    absRspVsJetDiPt.back()[iflv]=new TH1F(hname.c_str(),
						  ";p_{T}-p_{T}^{ref} [GeV]",
						  nbinsabsrsp,absrspmin,absrspmax);
	  }
	}
	
	if (doabsrsp&&dorefdipt) {
	  absRspVsRefDiPt.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"AbsRsp_"+get_suffix("RefDiPt",idipt,binsdipt);
	    absRspVsRefDiPt.back()[iflv]=new TH1F(hname.c_str(),
						  ";p_{T}-p_{T}^{ref} [GeV]",
						  nbinsabsrsp,absrspmin,absrspmax);
	  }
	}
	
	if (doabsrsp&&dorefdipt) {
	  absRspVsRefDiPtBarrel.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"AbsRsp_Barrel_"+get_suffix("RefDiPt",idipt,binsdipt);
	    absRspVsRefDiPtBarrel.back()[iflv]=new TH1F(hname.c_str(),
							";p_{T}-p_{T}^{ref} [GeV]",
							nbinsabsrsp,
							absrspmin,absrspmax);
	  }
	}

	if (doetarsp&&dojetdipt) {
	  etaRspVsJetDiPt.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"EtaRsp_"+get_suffix("JetDiPt",idipt,binsdipt);
	    etaRspVsJetDiPt.back()[iflv]=new TH1F(hname.c_str(),
						  ";|#eta|-|#eta^{ref}|",
						  nbinsetarsp,etarspmin,etarspmax);
	  }
	}
	
	if (doetarsp&&dorefdipt) {
	  etaRspVsRefDiPt.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"EtaRsp_"+get_suffix("RefDiPt",idipt,binsdipt);
	    etaRspVsRefDiPt.back()[iflv]=new TH1F(hname.c_str(),
						  ";|#eta|-|#eta^{ref}|",
						  nbinsetarsp,etarspmin,etarspmax);
	  }
	}
	
	if (dophirsp&&dojetdipt) {
	  phiRspVsJetDiPt.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"PhiRsp_"+get_suffix("JetDiPt",idipt,binsdipt);
	    phiRspVsJetDiPt.back()[iflv]=new TH1F(hname.c_str(),
						  ";#phi-#phi^{ref}",
						  nbinsphirsp,phirspmin,phirspmax);
	  }
	}
	
	if (dophirsp&&dorefdipt) {
	  phiRspVsRefDiPt.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"PhiRsp_"+get_suffix("RefDiPt",idipt,binsdipt);
	    phiRspVsRefDiPt.back()[iflv]=new TH1F(hname.c_str(),
						  ";#phi-#phi^{ref}",
						  nbinsphirsp,phirspmin,phirspmax);
	  }
	}
	
	
      }
    }



    

    
    // book eta histograms
    if (binseta.size()>=2) {
      for (unsigned int ieta=0;ieta<binseta.size()-1;++ieta) {

	string hname; float etamin=binseta[ieta]; float etamax=binseta[ieta+1];
	
	if (1) {
	  jetEtaVsJetEta.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"JetEta_"+get_suffix("JetEta",ieta,binseta);
	    jetEtaVsJetEta.back()[iflv]=new TH1F(hname.c_str(),";#eta",
						 nbinseta,etamin,etamax);
	  }
	}
	
	if (dorelrsp) {
	  relRspVsJetEta.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"RelRsp_"+get_suffix("JetEta",ieta,binseta);
	    relRspVsJetEta.back()[iflv]=new TH1F(hname.c_str(),";p_{T}/p_{T}^{ref}",
						 nbinsrelrsp,relrspmin,relrspmax);
	  }
	}
	
	if (doabsrsp) {
	  absRspVsJetEta.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"AbsRsp_"+get_suffix("JetEta",ieta,binseta);
	    absRspVsJetEta.back()[iflv]=new TH1F(hname.c_str(),
						 ";p_{T}-p_{T}^{ref} [GeV]",
						 nbinsabsrsp,absrspmin,absrspmax);
	  }
	}
	
	if (doetarsp) {
	  etaRspVsJetEta.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"EtaRsp_"+get_suffix("JetEta",ieta,binseta);
	    etaRspVsJetEta.back()[iflv]=new TH1F(hname.c_str(),
						 ";|#eta|-|#eta^{ref}|",
						 nbinsetarsp,etarspmin,etarspmax);
	  }
	}
	
	if (dophirsp) {
	  phiRspVsJetEta.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"PhiRsp_"+get_suffix("JetEta",ieta,binseta);
	    phiRspVsJetEta.back()[iflv]=new TH1F(hname.c_str(),
						 ";#phi-#phi^{ref}",
						 nbinsphirsp,phirspmin,phirspmax);
	  }
	}

      }
    }
    
    // book phi histograms
    if (binsphi.size()>=2) {
      for (unsigned int iphi=0;iphi<binsphi.size()-1;++iphi) {

	string hname; float phimin=binsphi[iphi]; float phimax=binsphi[iphi+1];

	if (1) {
	  jetPhiVsJetPhi.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"JetPhi_"+get_suffix("JetPhi",iphi,binsphi);
	    jetPhiVsJetPhi.back()[iflv]=new TH1F(hname.c_str(),";#phi",
						 nbinsphi,phimin,phimax);
	  }
	}
	
	if (dorelrsp) {
	  relRspVsJetPhi.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"RelRsp_"+get_suffix("JetPhi",iphi,binsphi);
	    relRspVsJetPhi.back()[iflv]=new TH1F(hname.c_str(),";p_{T}/p_{T}^{ref}",
						 nbinsrelrsp,relrspmin,relrspmax);
	  }
	}
	
	if (doabsrsp) {
	  absRspVsJetPhi.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"AbsRsp_"+get_suffix("JetPhi",iphi,binsphi);
	    absRspVsJetPhi.back()[iflv]=new TH1F(hname.c_str(),
						 ";p_{T}-p_{T}^{ref} [GeV]",
						 nbinsabsrsp,absrspmin,absrspmax);
	  }
	}
	
	if (doetarsp) {
	  etaRspVsJetPhi.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"EtaRsp_"+get_suffix("JetPhi",iphi,binsphi);
	    etaRspVsJetPhi.back()[iflv]=new TH1F(hname.c_str(),
						 ";|#eta|-|#eta^{ref}|",
						 nbinsetarsp,etarspmin,etarspmax);
	  }
	}
	
	if (dophirsp) {
	  phiRspVsJetPhi.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"PhiRsp_"+get_suffix("JetPhi",iphi,binsphi);
	    phiRspVsJetPhi.back()[iflv]=new TH1F(hname.c_str(),
						 ";#phi-#phi^{ref}",
						 nbinsphirsp,phirspmin,phirspmax);
	  }
	}
      }
    }
	
    // book y histograms (rapidity)
    if (binsy.size()>=2) {
      for (unsigned int iy=0;iy<binsy.size()-1;++iy) {
	
	string hname; float ymin=binsy[iy]; float ymax=binsy[iy+1];
	
	if (1) {
	  jetYVsJetY.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"JetY_"+get_suffix("JetY",iy,binsy);
	    jetYVsJetY.back()[iflv]=new TH1F(hname.c_str(),";#y",nbinsy,ymin,ymax);
	  }
	}
	
	if (dorelrsp) {
	  relRspVsJetY.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"RelRsp_"+get_suffix("JetY",iy,binsy);
	    relRspVsJetY.back()[iflv]=new TH1F(hname.c_str(),";p_{T}/p_{T}^{ref}",
					       nbinsrelrsp,relrspmin,relrspmax);
	  }
	}
	
	if (doabsrsp) {
	  absRspVsJetY.push_back(new TH1F*[flavor.size()]);
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    hname=flavor[iflv]+"AbsRsp_"+get_suffix("JetY",iy,binsy);
	    absRspVsJetY.back()[iflv]=new TH1F(hname.c_str(),
					       ";p_{T}-p_{T}^{ref} [GeV]",
					       nbinsabsrsp,absrspmin,absrspmax);
	  }
	}
	
      }
    }

    // book eta/pT histograms
    if (binspt.size()>=2&&binseta.size()>=2) {
      for (unsigned int ieta=0;ieta<binseta.size()-1;++ieta) {
	
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
	
	if (dojetpt) {
	  jetPtJetPt=new TH1F**[binspt.size()-1];
	  for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
	    jetPtJetPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (dorefpt) {
	  refPtRefPt =new TH1F**[binspt.size()];
	  for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
	    refPtRefPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (dorefpt) {
	  jetPtRefPt =new TH1F**[binspt.size()];
	  for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
	    jetPtRefPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (dorelrsp&&dojetpt) {
	  relRspJetPt=new TH1F**[binspt.size()-1];
	  for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
	    relRspJetPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (dorelrsp&&dorefpt) {
	  relRspRefPt=new TH1F**[binspt.size()-1];
	  for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
	    relRspRefPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (doabsrsp&&dojetpt) {
	  absRspJetPt=new TH1F**[binspt.size()-1];
	  for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
	    absRspJetPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (doabsrsp&&dorefpt) {
	  absRspRefPt=new TH1F**[binspt.size()-1];
	  for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
	    absRspRefPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (doetarsp&&dojetpt) {
	  etaRspJetPt=new TH1F**[binspt.size()-1];
	  for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
	    etaRspJetPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (doetarsp&&dorefpt) {
	  etaRspRefPt=new TH1F**[binspt.size()-1];
	  for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
	    etaRspRefPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (dophirsp&&dojetpt) {
	  phiRspJetPt=new TH1F**[binspt.size()-1];
	  for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
	    phiRspJetPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (dophirsp&&dorefpt) {
	  phiRspRefPt=new TH1F**[binspt.size()-1];
	  for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
	    phiRspRefPt[ipt]=new TH1F*[flavor.size()];
	}
	
	string jetEtaSuffix=get_suffix("JetEta",ieta,binseta);
	
	for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++) {

	  string hname; float ptmin=binspt[ipt]; float ptmax=binspt[ipt+1];

	  string jetPtSuffix=get_suffix("JetPt",ipt,binspt);
	  string refPtSuffix=get_suffix("RefPt",ipt,binspt);
	  
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    
	    if (dojetpt) {
	      hname=flavor[iflv]+"JetPt_"+jetEtaSuffix+"_"+jetPtSuffix;
	      jetPtJetPt[ipt][iflv]=new TH1F(hname.c_str(),";p_{T}",
					     nbinspt,ptmin,ptmax);
	    }
	    
	    if (dorefpt) {
	      hname=flavor[iflv]+"RefPt_"+jetEtaSuffix+"_"+refPtSuffix;
	      refPtRefPt[ipt][iflv]=new TH1F(hname.c_str(),";p_{T}^{ref}",
					     nbinspt,ptmin,ptmax);
	    }
	    
	    if (dorefpt) {
	      hname=flavor[iflv]+"JetPt_"+jetEtaSuffix+"_"+refPtSuffix;
	      jetPtRefPt[ipt][iflv]=new TH1F(hname.c_str(),";p_{T}",
					     3*nbinspt,
					     0,
					     3.0*ptmax);
	    }
	    
	    if (dorelrsp&&dojetpt) {
	      hname=flavor[iflv]+"RelRsp_"+jetEtaSuffix+"_"+jetPtSuffix;
	      relRspJetPt[ipt][iflv]=new TH1F(hname.c_str(),";p_{T}/p_{T}^{ref}",
					      nbinsrelrsp,relrspmin,relrspmax);
	    }
	    
	    if (dorelrsp&&dorefpt) {
	      hname=flavor[iflv]+"RelRsp_"+jetEtaSuffix+"_"+refPtSuffix;
	      relRspRefPt[ipt][iflv]=new TH1F(hname.c_str(),";p_{T}/p_{T}^{ref}",
					      nbinsrelrsp,relrspmin,relrspmax);
	    }
	    
	    if (doabsrsp&&dojetpt) {
	      hname=flavor[iflv]+"AbsRsp_"+jetEtaSuffix+"_"+jetPtSuffix;
	      absRspJetPt[ipt][iflv]=new TH1F(hname.c_str(),
					      ";p_{T}-p_{T}^{ref} [GeV]",
					      nbinsabsrsp,absrspmin,absrspmax);
	    }
	    
	    if (doabsrsp&&dorefpt) {
	      hname=flavor[iflv]+"AbsRsp_"+jetEtaSuffix+"_"+refPtSuffix;
	      absRspRefPt[ipt][iflv]=new TH1F(hname.c_str(),
					      ";p_{T}-p_{T}^{ref} [GeV]",
					      nbinsabsrsp,absrspmin,absrspmax);
	    }
	    if (doabsrsp&&dojetpt) {
	      hname=flavor[iflv]+"AbsRsp_"+jetEtaSuffix+"_"+jetPtSuffix;
	      absRspJetPt[ipt][iflv]=new TH1F(hname.c_str(),
					      ";|#eta|-|#eta^{ref}|",
					      nbinsabsrsp,absrspmin,absrspmax);
	    }
	    
	    if (doetarsp&&dorefpt) {
	      hname=flavor[iflv]+"EtaRsp_"+jetEtaSuffix+"_"+refPtSuffix;
	      etaRspRefPt[ipt][iflv]=new TH1F(hname.c_str(),
					      ";|#eta|-|#eta^{ref}|",
					      nbinsetarsp,etarspmin,etarspmax);
	    }

	    if (doetarsp&&dojetpt) {
	      hname=flavor[iflv]+"EtaRsp_"+jetEtaSuffix+"_"+jetPtSuffix;
	      etaRspJetPt[ipt][iflv]=new TH1F(hname.c_str(),
					      ";#phi-#phi^{ref}",
					      nbinsetarsp,etarspmin,etarspmax);
	    }
	    
	    if (dophirsp&&dorefpt) {
	      hname=flavor[iflv]+"PhiRsp_"+jetEtaSuffix+"_"+refPtSuffix;
	      phiRspRefPt[ipt][iflv]=new TH1F(hname.c_str(),
					      ";#phi-#phi^{ref}",
					      nbinsphirsp,phirspmin,phirspmax);
	    }

	    if (dophirsp&&dojetpt) {
	      hname=flavor[iflv]+"PhiRsp_"+jetEtaSuffix+"_"+jetPtSuffix;
	      phiRspJetPt[ipt][iflv]=new TH1F(hname.c_str(),
					      ";#phi-#phi^{ref}",
					      nbinsphirsp,phirspmin,phirspmax);
	    }
	  }
	}
	if (dojetpt)           jetPtVsJetEtaJetPt .push_back(jetPtJetPt);
	if (dorefpt)           refPtVsJetEtaRefPt .push_back(refPtRefPt);
	if (dorefpt)           jetPtVsJetEtaRefPt .push_back(jetPtRefPt);
	if (dorelrsp&&dojetpt) relRspVsJetEtaJetPt.push_back(relRspJetPt);
	if (dorelrsp&&dorefpt) relRspVsJetEtaRefPt.push_back(relRspRefPt);
	if (doabsrsp&&dojetpt) absRspVsJetEtaJetPt.push_back(absRspJetPt);
	if (doabsrsp&&dorefpt) absRspVsJetEtaRefPt.push_back(absRspRefPt);
	if (doetarsp&&dojetpt) etaRspVsJetEtaJetPt.push_back(etaRspJetPt);
	if (doetarsp&&dorefpt) etaRspVsJetEtaRefPt.push_back(etaRspRefPt);
	if (dophirsp&&dojetpt) phiRspVsJetEtaJetPt.push_back(phiRspJetPt);
	if (dophirsp&&dorefpt) phiRspVsJetEtaRefPt.push_back(phiRspRefPt);
      }
    }
    



    // book eta/DI-pT histograms
    if (binsdipt.size()>=2&&binseta.size()>=2) {
      for (unsigned int ieta=0;ieta<binseta.size()-1;++ieta) {
	
	TH1F*** jetDiPtJetDiPt(0);
	TH1F*** refDiPtRefDiPt(0);
	TH1F*** jetDiPtRefDiPt(0);
	TH1F*** relRspJetDiPt(0);
	TH1F*** relRspRefDiPt(0);
	TH1F*** absRspJetDiPt(0);
	TH1F*** absRspRefDiPt(0);
	TH1F*** etaRspJetDiPt(0);
	TH1F*** etaRspRefDiPt(0);	
	TH1F*** phiRspJetDiPt(0);
	TH1F*** phiRspRefDiPt(0);
	
	if (dojetdipt) {
	  jetDiPtJetDiPt=new TH1F**[binsdipt.size()-1];
	  for (unsigned int ipt=0;ipt<binsdipt.size()-1;ipt++)
	    jetDiPtJetDiPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (dorefdipt) {
	  refDiPtRefDiPt =new TH1F**[binsdipt.size()];
	  for (unsigned int ipt=0;ipt<binsdipt.size()-1;ipt++)
	    refDiPtRefDiPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (dorefdipt) {
	  jetDiPtRefDiPt =new TH1F**[binsdipt.size()];
	  for (unsigned int ipt=0;ipt<binsdipt.size()-1;ipt++)
	    jetDiPtRefDiPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (dorelrsp&&dojetdipt) {
	  relRspJetDiPt=new TH1F**[binsdipt.size()-1];
	  for (unsigned int ipt=0;ipt<binsdipt.size()-1;ipt++)
	    relRspJetDiPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (dorelrsp&&dorefdipt) {
	  relRspRefDiPt=new TH1F**[binsdipt.size()-1];
	  for (unsigned int ipt=0;ipt<binsdipt.size()-1;ipt++)
	    relRspRefDiPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (doabsrsp&&dojetdipt) {
	  absRspJetDiPt=new TH1F**[binsdipt.size()-1];
	  for (unsigned int ipt=0;ipt<binsdipt.size()-1;ipt++)
	    absRspJetDiPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (doabsrsp&&dorefdipt) {
	  absRspRefDiPt=new TH1F**[binsdipt.size()-1];
	  for (unsigned int ipt=0;ipt<binsdipt.size()-1;ipt++)
	    absRspRefDiPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (doetarsp&&dojetdipt) {
	  etaRspJetDiPt=new TH1F**[binsdipt.size()-1];
	  for (unsigned int ipt=0;ipt<binsdipt.size()-1;ipt++)
	    etaRspJetDiPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (doetarsp&&dorefdipt) {
	  etaRspRefDiPt=new TH1F**[binsdipt.size()-1];
	  for (unsigned int ipt=0;ipt<binsdipt.size()-1;ipt++)
	    etaRspRefDiPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (dophirsp&&dojetdipt) {
	  phiRspJetDiPt=new TH1F**[binsdipt.size()-1];
	  for (unsigned int ipt=0;ipt<binsdipt.size()-1;ipt++)
	    phiRspJetDiPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (dophirsp&&dorefdipt) {
	  phiRspRefDiPt=new TH1F**[binsdipt.size()-1];
	  for (unsigned int ipt=0;ipt<binsdipt.size()-1;ipt++)
	    phiRspRefDiPt[ipt]=new TH1F*[flavor.size()];
	}
	
	string jetEtaSuffix=get_suffix("JetEta",ieta,binseta);
	
	for (unsigned int ipt=0;ipt<binsdipt.size()-1;ipt++) {

	  string hname; float ptmin=binsdipt[ipt]; float ptmax=binsdipt[ipt+1];

	  string jetDiPtSuffix=get_suffix("JetDiPt",ipt,binsdipt);
	  string refDiPtSuffix=get_suffix("RefDiPt",ipt,binsdipt);
	  
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    
	    if (dojetdipt) {
	      hname=flavor[iflv]+"JetDiPt_"+jetEtaSuffix+"_"+jetDiPtSuffix;
	      jetDiPtJetDiPt[ipt][iflv]=new TH1F(hname.c_str(),";p_{T}",
						 nbinsdipt,ptmin,ptmax);
	    }
	    
	    if (dorefdipt) {
	      hname=flavor[iflv]+"RefDiPt_"+jetEtaSuffix+"_"+refDiPtSuffix;
	      refDiPtRefDiPt[ipt][iflv]=new TH1F(hname.c_str(),";p_{T}^{ref}",
						 nbinsdipt,ptmin,ptmax);
	    }
	    
	    if (dorefdipt) {
	      hname=flavor[iflv]+"JetDiPt_"+jetEtaSuffix+"_"+refDiPtSuffix;
	      jetDiPtRefDiPt[ipt][iflv]=new TH1F(hname.c_str(),";p_{T}",
						 3*nbinsdipt,
						 0,
						 3.0*ptmax);
	    }
	    
	    if (dorelrsp&&dojetdipt) {
	      hname=flavor[iflv]+"RelRsp_"+jetEtaSuffix+"_"+jetDiPtSuffix;
	      relRspJetDiPt[ipt][iflv]=new TH1F(hname.c_str(),";p_{T}/p_{T}^{ref}",
						nbinsrelrsp,relrspmin,relrspmax);
	    }
	    
	    if (dorelrsp&&dorefdipt) {
	      hname=flavor[iflv]+"RelRsp_"+jetEtaSuffix+"_"+refDiPtSuffix;
	      relRspRefDiPt[ipt][iflv]=new TH1F(hname.c_str(),";p_{T}/p_{T}^{ref}",
						nbinsrelrsp,relrspmin,relrspmax);
	    }
	    
	    if (doabsrsp&&dojetdipt) {
	      hname=flavor[iflv]+"AbsRsp_"+jetEtaSuffix+"_"+jetDiPtSuffix;
	      absRspJetDiPt[ipt][iflv]=new TH1F(hname.c_str(),
						";p_{T}-p_{T}^{ref} [GeV]",
						nbinsabsrsp,absrspmin,absrspmax);
	    }
	    
	    if (doabsrsp&&dorefdipt) {
	      hname=flavor[iflv]+"AbsRsp_"+jetEtaSuffix+"_"+refDiPtSuffix;
	      absRspRefDiPt[ipt][iflv]=new TH1F(hname.c_str(),
						";p_{T}-p_{T}^{ref} [GeV]",
						nbinsabsrsp,absrspmin,absrspmax);
	    }
	    if (doabsrsp&&dojetdipt) {
	      hname=flavor[iflv]+"AbsRsp_"+jetEtaSuffix+"_"+jetDiPtSuffix;
	      absRspJetDiPt[ipt][iflv]=new TH1F(hname.c_str(),
						";|#eta|-|#eta^{ref}|",
						nbinsabsrsp,absrspmin,absrspmax);
	    }
	    
	    if (doetarsp&&dorefdipt) {
	      hname=flavor[iflv]+"EtaRsp_"+jetEtaSuffix+"_"+refDiPtSuffix;
	      etaRspRefDiPt[ipt][iflv]=new TH1F(hname.c_str(),
						";|#eta|-|#eta^{ref}|",
						nbinsetarsp,etarspmin,etarspmax);
	    }

	    if (doetarsp&&dojetdipt) {
	      hname=flavor[iflv]+"EtaRsp_"+jetEtaSuffix+"_"+jetDiPtSuffix;
	      etaRspJetDiPt[ipt][iflv]=new TH1F(hname.c_str(),
						";#phi-#phi^{ref}",
						nbinsetarsp,etarspmin,etarspmax);
	    }
	    
	    if (dophirsp&&dorefdipt) {
	      hname=flavor[iflv]+"PhiRsp_"+jetEtaSuffix+"_"+refDiPtSuffix;
	      phiRspRefDiPt[ipt][iflv]=new TH1F(hname.c_str(),
						";#phi-#phi^{ref}",
						nbinsphirsp,phirspmin,phirspmax);
	    }

	    if (dophirsp&&dojetdipt) {
	      hname=flavor[iflv]+"PhiRsp_"+jetEtaSuffix+"_"+jetDiPtSuffix;
	      phiRspJetDiPt[ipt][iflv]=new TH1F(hname.c_str(),
						";#phi-#phi^{ref}",
						nbinsphirsp,phirspmin,phirspmax);
	    }

	  }
	}
	if (dojetdipt)           jetDiPtVsJetEtaJetDiPt .push_back(jetDiPtJetDiPt);
	if (dorefdipt)           refDiPtVsJetEtaRefDiPt .push_back(refDiPtRefDiPt);
	if (dorefdipt)           jetDiPtVsJetEtaRefDiPt .push_back(jetDiPtRefDiPt);
	if (dorelrsp&&dojetdipt) relRspVsJetEtaJetDiPt.push_back(relRspJetDiPt);
	if (dorelrsp&&dorefdipt) relRspVsJetEtaRefDiPt.push_back(relRspRefDiPt);
	if (doabsrsp&&dojetdipt) absRspVsJetEtaJetDiPt.push_back(absRspJetDiPt);
	if (doabsrsp&&dorefdipt) absRspVsJetEtaRefDiPt.push_back(absRspRefDiPt);
	if (doetarsp&&dojetdipt) etaRspVsJetEtaJetDiPt.push_back(etaRspJetDiPt);
	if (doetarsp&&dorefdipt) etaRspVsJetEtaRefDiPt.push_back(etaRspRefDiPt);
	if (dophirsp&&dojetdipt) phiRspVsJetEtaJetDiPt.push_back(phiRspJetDiPt);
	if (dophirsp&&dorefdipt) phiRspVsJetEtaRefDiPt.push_back(phiRspRefDiPt);
      }
    }
    












    
    // book y/pT histograms
    if (binspt.size()>=2&&binsy.size()>=2) {
      for (unsigned int iy=0;iy<binsy.size()-1;++iy) {
	
	TH1F*** jetPtJetPt(0);
	TH1F*** refPtRefPt(0);
	TH1F*** jetPtRefPt(0);
	TH1F*** relRspJetPt(0);
	TH1F*** relRspRefPt(0);
	TH1F*** absRspJetPt(0);
	TH1F*** absRspRefPt(0);
	
	if (dojetpt) {
	  jetPtJetPt=new TH1F**[binspt.size()-1];
	  for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
	    jetPtJetPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (dorefpt) {
	  refPtRefPt =new TH1F**[binspt.size()];
	  for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
	    refPtRefPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (dorefpt) {
	  jetPtRefPt =new TH1F**[binspt.size()];
	  for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
	    jetPtRefPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (dorelrsp&&dojetpt) {
	  relRspJetPt=new TH1F**[binspt.size()-1];
	  for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
	    relRspJetPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (dorelrsp&&dorefpt) {
	  relRspRefPt=new TH1F**[binspt.size()-1];
	  for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
	    relRspRefPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (doabsrsp&&dojetpt) {
	  absRspJetPt=new TH1F**[binspt.size()-1];
	  for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
	    absRspJetPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (doabsrsp&&dorefpt) {
	  absRspRefPt=new TH1F**[binspt.size()-1];
	  for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
	    absRspRefPt[ipt]=new TH1F*[flavor.size()];
	}
	
	string jetYSuffix=get_suffix("JetY",iy,binsy);
	
	for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++) {
	  
	  string hname; float ptmin=binspt[ipt]; float ptmax=binspt[ipt+1];
	  
	  string jetPtSuffix=get_suffix("JetPt",ipt,binspt);
	  string refPtSuffix=get_suffix("RefPt",ipt,binspt);
	  
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    
	    if (dojetpt) {
	      hname=flavor[iflv]+"JetPt_"+jetYSuffix+"_"+jetPtSuffix;
	      jetPtJetPt[ipt][iflv]=new TH1F(hname.c_str(),";p_{T}",
					     nbinspt,ptmin,ptmax);
	    }
	    
	    if (dorefpt) {
	      hname=flavor[iflv]+"RefPt_"+jetYSuffix+"_"+refPtSuffix;
	      refPtRefPt[ipt][iflv]=new TH1F(hname.c_str(),";p_{T}^{ref}",
					     nbinspt,ptmin,ptmax);
	    }
	    
	    if (dorefpt) {
	      hname=flavor[iflv]+"JetPt_"+jetYSuffix+"_"+refPtSuffix;
	      jetPtRefPt[ipt][iflv]=new TH1F(hname.c_str(),";p_{T}",
					     3*nbinspt,0,3.0*ptmax);
	    }
	    
	    if (dorelrsp&&dojetpt) {
	      hname=flavor[iflv]+"RelRsp_"+jetYSuffix+"_"+jetPtSuffix;
	      relRspJetPt[ipt][iflv]=new TH1F(hname.c_str(),";p_{T}/p_{T}^{ref}",
					      nbinsrelrsp,relrspmin,relrspmax);
	    }
	    
	    if (dorelrsp&&dorefpt) {
	      hname=flavor[iflv]+"RelRsp_"+jetYSuffix+"_"+refPtSuffix;
	      relRspRefPt[ipt][iflv]=new TH1F(hname.c_str(),";p_{T}/p_{T}^{ref}",
					      nbinsrelrsp,relrspmin,relrspmax);
	    }
	    
	    if (doabsrsp&&dojetpt) {
	      hname=flavor[iflv]+"AbsRsp_"+jetYSuffix+"_"+jetPtSuffix;
	      absRspJetPt[ipt][iflv]=new TH1F(hname.c_str(),
					      ";p_{T}-p_{T}^{ref} [GeV]",
					      nbinsabsrsp,absrspmin,absrspmax);
	    }
	    
	    if (doabsrsp&&dorefpt) {
	      hname=flavor[iflv]+"AbsRsp_"+jetYSuffix+"_"+refPtSuffix;
	      absRspRefPt[ipt][iflv]=new TH1F(hname.c_str(),
					      ";p_{T}-p_{T}^{ref} [GeV]",
					      nbinsabsrsp,absrspmin,absrspmax);
	    }
	    if (doabsrsp&&dojetpt) {
	      hname=flavor[iflv]+"AbsRsp_"+jetYSuffix+"_"+jetPtSuffix;
	      absRspJetPt[ipt][iflv]=new TH1F(hname.c_str(),
					      ";|#y|-|#y^{ref}|",
					      nbinsabsrsp,absrspmin,absrspmax);
	    }

	  }
	}
	if (dojetpt)           jetPtVsJetYJetPt .push_back(jetPtJetPt);
	if (dorefpt)           refPtVsJetYRefPt .push_back(refPtRefPt);
	if (dorefpt)           jetPtVsJetYRefPt .push_back(jetPtRefPt);
	if (dorelrsp&&dojetpt) relRspVsJetYJetPt.push_back(relRspJetPt);
	if (dorelrsp&&dorefpt) relRspVsJetYRefPt.push_back(relRspRefPt);
	if (doabsrsp&&dojetpt) absRspVsJetYJetPt.push_back(absRspJetPt);
	if (doabsrsp&&dorefpt) absRspVsJetYRefPt.push_back(absRspRefPt);
      }
    }










    
    // book y/DI-pT histograms
    if (binsdipt.size()>=2&&binsy.size()>=2) {
      for (unsigned int iy=0;iy<binsy.size()-1;++iy) {
	
	TH1F*** jetDiPtJetDiPt(0);
	TH1F*** refDiPtRefDiPt(0);
	TH1F*** jetDiPtRefDiPt(0);
	TH1F*** relRspJetDiPt(0);
	TH1F*** relRspRefDiPt(0);
	TH1F*** absRspJetDiPt(0);
	TH1F*** absRspRefDiPt(0);
	
	if (dojetpt) {
	  jetDiPtJetDiPt=new TH1F**[binsdipt.size()-1];
	  for (unsigned int ipt=0;ipt<binsdipt.size()-1;ipt++)
	    jetDiPtJetDiPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (dorefpt) {
	  refDiPtRefDiPt =new TH1F**[binsdipt.size()];
	  for (unsigned int ipt=0;ipt<binsdipt.size()-1;ipt++)
	    refDiPtRefDiPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (dorefpt) {
	  jetDiPtRefDiPt =new TH1F**[binsdipt.size()];
	  for (unsigned int ipt=0;ipt<binsdipt.size()-1;ipt++)
	    jetDiPtRefDiPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (dorelrsp&&dojetpt) {
	  relRspJetDiPt=new TH1F**[binsdipt.size()-1];
	  for (unsigned int ipt=0;ipt<binsdipt.size()-1;ipt++)
	    relRspJetDiPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (dorelrsp&&dorefpt) {
	  relRspRefDiPt=new TH1F**[binsdipt.size()-1];
	  for (unsigned int ipt=0;ipt<binsdipt.size()-1;ipt++)
	    relRspRefDiPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (doabsrsp&&dojetpt) {
	  absRspJetDiPt=new TH1F**[binsdipt.size()-1];
	  for (unsigned int ipt=0;ipt<binsdipt.size()-1;ipt++)
	    absRspJetDiPt[ipt]=new TH1F*[flavor.size()];
	}
	
	if (doabsrsp&&dorefpt) {
	  absRspRefDiPt=new TH1F**[binsdipt.size()-1];
	  for (unsigned int ipt=0;ipt<binsdipt.size()-1;ipt++)
	    absRspRefDiPt[ipt]=new TH1F*[flavor.size()];
	}
	
	string jetYSuffix=get_suffix("JetY",iy,binsy);
	
	for (unsigned int ipt=0;ipt<binsdipt.size()-1;ipt++) {
	  
	  string hname; float ptmin=binsdipt[ipt]; float ptmax=binsdipt[ipt+1];
	  
	  string jetDiPtSuffix=get_suffix("JetDiPt",ipt,binsdipt);
	  string refDiPtSuffix=get_suffix("RefDiPt",ipt,binsdipt);
	  
	  for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
	    
	    if (dojetpt) {
	      hname=flavor[iflv]+"JetDiPt_"+jetYSuffix+"_"+jetDiPtSuffix;
	      jetDiPtJetDiPt[ipt][iflv]=new TH1F(hname.c_str(),";p_{T}",
						 nbinsdipt,ptmin,ptmax);
	    }
	    
	    if (dorefpt) {
	      hname=flavor[iflv]+"RefDiPt_"+jetYSuffix+"_"+refDiPtSuffix;
	      refDiPtRefDiPt[ipt][iflv]=new TH1F(hname.c_str(),";p_{T}^{ref}",
						 nbinsdipt,ptmin,ptmax);
	    }
	    
	    if (dorefpt) {
	      hname=flavor[iflv]+"JetDiPt_"+jetYSuffix+"_"+refDiPtSuffix;
	      jetDiPtRefDiPt[ipt][iflv]=new TH1F(hname.c_str(),";p_{T}",
						 3*nbinsdipt,0,3.0*ptmax);
	    }
	    
	    if (dorelrsp&&dojetpt) {
	      hname=flavor[iflv]+"RelRsp_"+jetYSuffix+"_"+jetDiPtSuffix;
	      relRspJetDiPt[ipt][iflv]=new TH1F(hname.c_str(),";p_{T}/p_{T}^{ref}",
						nbinsrelrsp,relrspmin,relrspmax);
	    }
	    
	    if (dorelrsp&&dorefpt) {
	      hname=flavor[iflv]+"RelRsp_"+jetYSuffix+"_"+refDiPtSuffix;
	      relRspRefDiPt[ipt][iflv]=new TH1F(hname.c_str(),";p_{T}/p_{T}^{ref}",
						nbinsrelrsp,relrspmin,relrspmax);
	    }
	    
	    if (doabsrsp&&dojetpt) {
	      hname=flavor[iflv]+"AbsRsp_"+jetYSuffix+"_"+jetDiPtSuffix;
	      absRspJetDiPt[ipt][iflv]=new TH1F(hname.c_str(),
						";p_{T}-p_{T}^{ref} [GeV]",
						nbinsabsrsp,absrspmin,absrspmax);
	    }
	    
	    if (doabsrsp&&dorefpt) {
	      hname=flavor[iflv]+"AbsRsp_"+jetYSuffix+"_"+refDiPtSuffix;
	      absRspRefDiPt[ipt][iflv]=new TH1F(hname.c_str(),
						";p_{T}-p_{T}^{ref} [GeV]",
						nbinsabsrsp,absrspmin,absrspmax);
	    }
	    if (doabsrsp&&dojetpt) {
	      hname=flavor[iflv]+"AbsRsp_"+jetYSuffix+"_"+jetDiPtSuffix;
	      absRspJetDiPt[ipt][iflv]=new TH1F(hname.c_str(),
						";|#y|-|#y^{ref}|",
						nbinsabsrsp,absrspmin,absrspmax);
	    }

	  }
	}
	if (dojetpt)           jetDiPtVsJetYJetDiPt .push_back(jetDiPtJetDiPt);
	if (dorefpt)           refDiPtVsJetYRefDiPt .push_back(refDiPtRefDiPt);
	if (dorefpt)           jetDiPtVsJetYRefDiPt .push_back(jetDiPtRefDiPt);
	if (dorelrsp&&dojetpt) relRspVsJetYJetDiPt.push_back(relRspJetDiPt);
	if (dorelrsp&&dorefpt) relRspVsJetYRefDiPt.push_back(relRspRefDiPt);
	if (doabsrsp&&dojetpt) absRspVsJetYJetDiPt.push_back(absRspJetDiPt);
	if (doabsrsp&&dorefpt) absRspVsJetYRefDiPt.push_back(absRspRefDiPt);
      }
    }



    //
    // fill histograms
    //
    unsigned int nevt = (unsigned int)tree->GetEntries();
    for (unsigned int ievt=0;ievt<nevt;ievt++) {
      tree->GetEntry(ievt);

      if (0==nref) continue;


      //find the 3 leading reco jets and index

      unsigned char recjt0=0; float jt0pt = 0.0; bool hasfirstjet = false;
      unsigned char recjt1=0; float jt1pt = 0.0; bool hassecondjet= false;
      unsigned char recjt2=0; float jt2pt = 0.0; bool hasthirdjet = false;

      if (!dobalance) {
	for (unsigned char ij0=0;ij0<nref;ij0++) 
	  if (jt0pt<jtpt[ij0]&&refdrjt[ij0]<=drmax_alg&&jtpt[ij0]>=jtptmin) 
	    {hasfirstjet=true;recjt0=ij0;jt0pt=jtpt[recjt0];}
	for (unsigned char ij1=0;ij1<nref;ij1++) {
	  if (recjt0==ij1) continue;
	  if (jt1pt<jtpt[ij1]&&refdrjt[ij1]<=drmax_alg&&jtpt[ij1]>=jtptmin) 
	    {hassecondjet=true;recjt1=ij1;jt1pt=jtpt[recjt1];}
	}
	for (unsigned char ij2=0;ij2<nref;ij2++) {
	  if (recjt0==ij2 || recjt1==ij2) continue;
	  if (jt2pt<jtpt[ij2]&&refdrjt[ij2]<=drmax_alg) 
	    {hasthirdjet=true;recjt2=ij2;jt2pt=jtpt[recjt2];}
	}
      }

      // constrain extra jet activity - first for ref jets
      if ( (int) nref>0 ) {
	unsigned char nprobe = nrefmax;
	if ( maxextrarefpt>=0.0&&nrefmax>0&&nprobe<nref ) {
	  if (refpt[nprobe]>maxextrarefpt) continue;
	}
	if ( minextrarefpt>=0.0&&nrefmax>0 ) {
	  if (nprobe>=nref) continue;
	  else if (nprobe<nref ) {
	    if (refpt[nprobe]<minextrarefpt) continue;
	    assert (jtpt[nprobe]>0.0);
	  }
	}
	// then for reco jets; but here only the dijet case...!
	if ( maxextrajetpt>=0.0&&nrefmax>0&&nprobe<nref ) {
	  if (nprobe!=2) continue;
	  if (hasthirdjet && jtpt[recjt2]>maxextrajetpt) continue;
	}
	if ( minextrajetpt>=0.0&&nrefmax>0 ) {
	  if (nprobe!=2) continue;
	  if (!hasthirdjet) continue;
	  if (jtpt[recjt2]<minextrajetpt) continue;
	}
      }

      

      bool isdijet = (hasfirstjet&&hassecondjet);
      bool isdiref = (nref>1&&refdrjt[0]<=drmax_alg&&refdrjt[1]<=drmax_alg
		      &&jtpt[0]>=jtptmin&&jtpt[1]>=jtptmin);


      // dijet values ordered in REC jet pt
      float jtrecdipt  = (isdijet) ? .5*(jtpt[recjt0]+jtpt[recjt1]) : 0.0;
      float jtrecdidphi= (isdijet) ? deltaPhi(jtphi[recjt0],jtphi[recjt1]) :0.0;

      // dijet values ordered in GEN jet pt
      float refgendipt = (isdiref) ? (.5*(refpt[0]+refpt[1]))     : 0.0;

      float diabsrspgen[2];
      diabsrspgen[0] = (isdiref) ? jtpt[0]-refpt[0]           : 0.0;
      diabsrspgen[1] = (isdiref) ? jtpt[1]-refpt[1]           : 0.0;
      float diabsrsprec[2];
      diabsrsprec[0] = (isdijet) ? jtpt[recjt0]-refpt[recjt0] : 0.0;
      diabsrsprec[1] = (isdijet) ? jtpt[recjt1]-refpt[recjt1] : 0.0;

      float direlrspgen[2];
      direlrspgen[0] = (isdiref) ? jtpt[0]/refpt[0]           : 0.0;
      direlrspgen[1] = (isdiref) ? jtpt[1]/refpt[1]           : 0.0;
      float direlrsprec[0];
      direlrsprec[0] = (isdijet) ? jtpt[recjt0]/refpt[recjt0] : 0.0;
      direlrsprec[1] = (isdijet) ? jtpt[recjt1]/refpt[recjt1] : 0.0;

      float dietarspgen[2];
      dietarspgen[0] = (isdiref) ? jteta[0]-refeta[0]           : 0.0;
      dietarspgen[1] = (isdiref) ? jteta[1]-refeta[1]           : 0.0;
      float dietarsprec[2];
      dietarsprec[0] = (isdijet) ? jteta[recjt0]-refeta[recjt0] : 0.0;
      dietarsprec[1] = (isdijet) ? jteta[recjt1]-refeta[recjt1] : 0.0;

      float diphirspgen[2];
      diphirspgen[0] = (isdiref) ? fmod(jtphi[0]-refphi[0]+3*M_PI,2*M_PI)-M_PI : 0.0;
      diphirspgen[1] = (isdiref) ? fmod(jtphi[1]-refphi[1]+3*M_PI,2*M_PI)-M_PI : 0.0;
      float diphirsprec[2];
      diphirsprec[0] = (isdijet) ? fmod(jtphi[recjt0]-refphi[recjt0]+3*M_PI,2*M_PI)-M_PI : 0.0;
      diphirsprec[1] = (isdijet) ? fmod(jtphi[recjt1]-refphi[recjt1]+3*M_PI,2*M_PI)-M_PI : 0.0;

      int refrecpdgid[2];
      refrecpdgid[0] = (isdijet) ? refpdgid[recjt0] : 0;
      refrecpdgid[1] = (isdijet) ? refpdgid[recjt1] : 0;
      
      if (jetrecdidphimin!=-1. && (jtrecdidphi < jetrecdidphimin)) continue;
      if (mindijetpt!=-1. && (jtrecdipt < mindijetpt)) continue;

      if (nrefmax>0) nref = std::min((int)nref,nrefmax);

      for (unsigned char iref=0;iref<nref;iref++) {

	if (( dobalance&&refdphijt[iref]<dphimin)||
	    (!dobalance&&refdrjt[iref]>drmax_alg)) continue;

	if (jtpt[iref]<jtptmin) continue;
	
	float eta    =
	  (binseta.size()&&binseta.front()>=0.)?std::abs(jteta[iref]):jteta[iref];
	float y      =
	  (binsy.size()&&binsy.front()>=0.)?std::abs(jty[iref]):jty[iref];
	
	float absrsp = jtpt[iref]-refpt[iref];
	float relrsp = jtpt[iref]/refpt[iref];
	float etarsp = jteta[iref]-refeta[iref];
	float phirsp = fmod(jtphi[iref]-refphi[iref]+3*M_PI,2*M_PI)-M_PI;




	if (eta>=etabarrelmin&&eta<=etabarrelmax) {
	  if (dorefpt) {
	    fill_histo(refpt[iref],weight,refpt[iref],binspt,refPtVsRefPtBarrel);
	    fill_histo(jtpt [iref],weight,refpt[iref],binspt,jetPtVsRefPtBarrel);
	    if (doflavor) {
	      fill_histo(refpdgid[iref],refpt[iref],weight,
			 refpt[iref],binspt,refPtVsRefPtBarrel);
	      fill_histo(refpdgid[iref],jtpt [iref],weight,
			 refpt[iref],binspt,jetPtVsRefPtBarrel);
	    }
	  }
	  if (dorelrsp&&dorefpt) {
	    fill_histo(relrsp,weight,refpt[iref],binspt,relRspVsRefPtBarrel);
	    if (doflavor) fill_histo(refpdgid[iref],relrsp,weight,
				     refpt[iref],binspt,relRspVsRefPtBarrel);
	  }
	  if (doabsrsp&&dorefpt) {
	    fill_histo(absrsp,weight,refpt[iref],binspt,absRspVsRefPtBarrel);
	    if (doflavor) fill_histo(refpdgid[iref],absrsp,weight,
				     refpt[iref],binspt,absRspVsRefPtBarrel);
	  }
	}
	
	if (dojetpt) {
	  fill_histo(jtpt[iref],weight,jtpt[iref], binspt,jetPtVsJetPt);
	  if (doflavor)
	    fill_histo(refpdgid[iref],jtpt[iref],weight,
		       jtpt[iref],binspt,jetPtVsJetPt);
	}

	if (dorefpt) {
	  fill_histo(refpt[iref],weight,refpt[iref],binspt,refPtVsRefPt);
	  fill_histo(jtpt [iref],weight,refpt[iref],binspt,jetPtVsRefPt);
	  if (doflavor) {
	    fill_histo(refpdgid[iref],refpt[iref],weight,
		       refpt[iref],binspt,refPtVsRefPt);
	    fill_histo(refpdgid[iref],jtpt[iref],weight,
		       refpt[iref],binspt,jetPtVsRefPt);
	  }
	}
	
	fill_histo(eta,weight,eta,binseta,jetEtaVsJetEta);
	if (doflavor) fill_histo(refpdgid[iref],eta,weight,
				 eta,binseta,jetEtaVsJetEta);
	
	fill_histo(jtphi[iref],weight,jtphi[iref],binsphi,jetPhiVsJetPhi);
	if (doflavor) fill_histo(refpdgid[iref],jtphi[iref],weight,
				 jtphi[iref],binsphi,jetPhiVsJetPhi);
 
	fill_histo(jty[iref],weight,jty[iref],binsy,jetYVsJetY);
	if (doflavor) fill_histo(refpdgid[iref],jty[iref],weight,
				 jty[iref],binsy,jetYVsJetY);
 
	if (dojetpt) {
	  fill_histo(jtpt[iref],weight,eta,jtpt[iref],
		     binseta,binspt,jetPtVsJetEtaJetPt);
	  fill_histo(jtpt[iref],weight,y,jtpt[iref],
		     binsy,binspt,jetPtVsJetYJetPt);

	  if (doflavor) {
	    fill_histo(refpdgid[iref],jtpt[iref],weight,
		       eta,jtpt[iref],binseta,binspt,jetPtVsJetEtaJetPt);
	    fill_histo(refpdgid[iref],jtpt[iref],weight,
		       y,jtpt[iref],binsy,binspt,jetPtVsJetYJetPt);
	  }
	}

	if (dorefpt) {
	  fill_histo(refpt[iref],weight,eta,refpt[iref],
		     binseta,binspt,refPtVsJetEtaRefPt);
	  fill_histo(jtpt [iref],weight,eta,refpt[iref],
		     binseta,binspt,jetPtVsJetEtaRefPt);
	  fill_histo(refpt[iref],weight,y,refpt[iref],
		     binsy,binspt,refPtVsJetYRefPt);
	  fill_histo(jtpt [iref],weight,y,refpt[iref],
		     binsy,binspt,jetPtVsJetYRefPt);
	  if (doflavor) {
	    fill_histo(refpdgid[iref],refpt[iref],weight,
		       eta,refpt[iref],binseta,binspt,refPtVsJetEtaRefPt);
	    fill_histo(refpdgid[iref],jtpt[iref],weight,
		       eta,refpt[iref],binseta,binspt,jetPtVsJetEtaRefPt);
	    fill_histo(refpdgid[iref],refpt[iref],weight,
		       y,refpt[iref],binsy,binspt,refPtVsJetYRefPt);
	    fill_histo(refpdgid[iref],jtpt[iref],weight,
		       y,refpt[iref],binsy,binspt,jetPtVsJetYRefPt);
	  }
	}
	
	if (dorelrsp) {
	  if (dojetpt) {
	    fill_histo(relrsp,weight,jtpt[iref],binspt,relRspVsJetPt);
	    if (doflavor) fill_histo(refpdgid[iref],relrsp,weight,
				     jtpt[iref], binspt,relRspVsJetPt);
	  }
	  if (dorefpt) {
	    fill_histo(relrsp,weight,refpt[iref],binspt,relRspVsRefPt);
	    if (doflavor) fill_histo(refpdgid[iref],relrsp,weight,
				     refpt[iref],binspt,relRspVsRefPt);
	  }

	  fill_histo(relrsp,weight,eta,binseta,relRspVsJetEta);
	  if (doflavor) fill_histo(refpdgid[iref],relrsp,weight,
				   eta,binseta,relRspVsJetEta);
	  
	  fill_histo(relrsp,weight,jtphi[iref],binsphi,relRspVsJetPhi);
	  if (doflavor) fill_histo(refpdgid[iref],relrsp,weight,
				   jtphi[iref],binsphi,relRspVsJetPhi);
	  
	  fill_histo(relrsp,weight,jty[iref],binsy,relRspVsJetY);
	  if (doflavor) fill_histo(refpdgid[iref],relrsp,weight,
				   jty[iref],binsy,relRspVsJetY);
	  
	  if (dojetpt) {
	    fill_histo(relrsp,weight,eta,jtpt[iref],
		       binseta,binspt,relRspVsJetEtaJetPt);
	    fill_histo(relrsp,weight,y,jtpt[iref],
		       binsy,binspt,relRspVsJetYJetPt);
	    if (doflavor) {
	      fill_histo(refpdgid[iref],relrsp,weight,eta,jtpt[iref],
			 binseta,binspt,relRspVsJetEtaJetPt);
	      fill_histo(refpdgid[iref],relrsp,weight,y,jtpt[iref],
			 binsy,binspt,relRspVsJetYJetPt);
	    }
	  }
	  if (dorefpt) {
	    fill_histo(relrsp,weight,eta,refpt[iref],
		       binseta,binspt,relRspVsJetEtaRefPt);
	    fill_histo(relrsp,weight,y,refpt[iref],
		       binsy,binspt,relRspVsJetYRefPt);
	    if (doflavor) {
	      fill_histo(refpdgid[iref],relrsp,weight,eta,refpt[iref],
			 binseta,binspt,relRspVsJetEtaRefPt);
	      fill_histo(refpdgid[iref],relrsp,weight,y,refpt[iref],
			 binsy,binspt,relRspVsJetYRefPt);
	    }
	  }
	}
	
	if (doabsrsp) {
	  if (dojetpt) {
	    fill_histo(absrsp,weight,jtpt[iref], binspt,absRspVsJetPt);
	    if (doflavor) fill_histo(refpdgid[iref],absrsp,weight,
				     jtpt[iref],binspt,absRspVsJetPt);
	  }
	  if (dorefpt) {
	    fill_histo(absrsp,weight,refpt[iref],binspt,absRspVsRefPt);
	    if (doflavor) fill_histo(refpdgid[iref],absrsp,weight,
				     refpt[iref],binspt,absRspVsRefPt);
	  }
	  
	  fill_histo(absrsp,weight,eta,binseta,absRspVsJetEta);
	  if (doflavor) fill_histo(refpdgid[iref],absrsp,weight,
				   eta,binseta,absRspVsJetEta);

	  fill_histo(absrsp,weight,jtphi[iref],binsphi,absRspVsJetPhi);
	  if (doflavor) fill_histo(refpdgid[iref],absrsp,weight,
				   jtphi[iref],binsphi,absRspVsJetPhi);
	  
	  fill_histo(absrsp,weight,jty[iref],binsy,absRspVsJetY);
	  if (doflavor) fill_histo(refpdgid[iref],absrsp,weight,
				   jty[iref],binsy,absRspVsJetY);
	  
	  if (dojetpt) {
	    fill_histo(absrsp,weight,eta,jtpt[iref],
		       binseta,binspt,absRspVsJetEtaJetPt);
	    fill_histo(absrsp,weight,y,jtpt[iref],
		       binsy,binspt,absRspVsJetYJetPt);
	    if (doflavor) {
	      fill_histo(refpdgid[iref],absrsp,weight,eta,jtpt[iref],
			 binseta,binspt,absRspVsJetEtaJetPt);
	      fill_histo(refpdgid[iref],absrsp,weight,y,jtpt[iref],
			 binsy,binspt,absRspVsJetYJetPt);
	    }
	  }
	  if (dorefpt) {
	    fill_histo(absrsp,weight,eta,refpt[iref],
		       binseta,binspt,absRspVsJetEtaRefPt);
	    fill_histo(absrsp,weight,y,refpt[iref],
		       binsy,binspt,absRspVsJetYRefPt);
	    if (doflavor) {
	      fill_histo(refpdgid[iref],absrsp,weight,eta,refpt[iref],
			 binseta,binspt,absRspVsJetEtaRefPt);
	      fill_histo(refpdgid[iref],absrsp,weight,y,refpt[iref],
			 binsy,binspt,absRspVsJetYRefPt);
	    }
	  }
	}
	
	if (doetarsp) {
	  if (dojetpt) {
	    fill_histo(etarsp,weight,jtpt[iref], binspt,etaRspVsJetPt);
	    if (doflavor) fill_histo(refpdgid[iref],etarsp,weight,
				     jtpt[iref],binspt,etaRspVsJetPt);
	  }
	  if (dorefpt) {
	    fill_histo(etarsp,weight,refpt[iref],binspt,etaRspVsRefPt);
	    if (doflavor) fill_histo(refpdgid[iref],etarsp,weight,
				     refpt[iref],binspt,etaRspVsRefPt);
	  }
	  
	  fill_histo(etarsp,weight,eta,binseta,etaRspVsJetEta);
	  if (doflavor) fill_histo(refpdgid[iref],etarsp,weight,
				   eta,binseta,etaRspVsJetEta);
	  
	  fill_histo(etarsp,weight,jtphi[iref],binsphi,etaRspVsJetPhi);
	  if (doflavor) fill_histo(refpdgid[iref],etarsp,weight,
				   jtphi[iref],binsphi,etaRspVsJetPhi);
	  
	  if (dojetpt) {
	    fill_histo(etarsp,weight,eta,jtpt[iref],
		       binseta,binspt,etaRspVsJetEtaJetPt);
	    if (doflavor) fill_histo(refpdgid[iref],etarsp,weight,
				     eta,jtpt[iref],
				     binseta,binspt,etaRspVsJetEtaJetPt);
	  }
	  if (dorefpt) {
	    fill_histo(etarsp,weight,eta,refpt[iref],
		       binseta,binspt,etaRspVsJetEtaRefPt);
	    if (doflavor) fill_histo(refpdgid[iref],etarsp,weight,
				     eta,refpt[iref],
				     binseta,binspt,etaRspVsJetEtaRefPt);
	  }
	}
	
	if (dophirsp) {
	  if (dojetpt) {
	    fill_histo(phirsp,weight,jtpt[iref], binspt,phiRspVsJetPt);
	    if (doflavor) fill_histo(refpdgid[iref],phirsp,weight,
				     jtpt[iref],binspt,phiRspVsJetPt);
	  }
	  if (dorefpt) {
	    fill_histo(phirsp,weight,refpt[iref],binspt,phiRspVsRefPt);
	    if (doflavor) fill_histo(refpdgid[iref],phirsp,weight,
				     refpt[iref],binspt,phiRspVsRefPt);
	  }
	  
	  fill_histo(phirsp,weight,eta,binseta,phiRspVsJetEta);
	  if (doflavor) fill_histo(refpdgid[iref],phirsp,weight,
				   eta,binseta,phiRspVsJetEta);
	  
	  fill_histo(phirsp,weight,jtphi[iref],binsphi,phiRspVsJetPhi);
	  if (doflavor) fill_histo(refpdgid[iref],phirsp,weight,
				   jtphi[iref],binsphi,phiRspVsJetPhi);
	  
	  if (dojetpt) {
	    fill_histo(phirsp,weight,eta,jtpt[iref],
		       binseta,binspt,phiRspVsJetEtaJetPt);
	    if (doflavor) fill_histo(refpdgid[iref],phirsp,weight,
				     eta,jtpt[iref],
				     binseta,binspt,phiRspVsJetEtaJetPt);
	  }
	  if (dorefpt) {
	    fill_histo(phirsp,weight,eta,refpt[iref],
		       binseta,binspt,phiRspVsJetEtaRefPt);
	    if (doflavor) fill_histo(refpdgid[iref],phirsp,weight,
				     eta,refpt[iref],
				     binseta,binspt,phiRspVsJetEtaRefPt);
	  }
	}
	

	//hh

	if (iref<2&&nref>=2&&(dojetdipt||dorefdipt)) {

	  if (eta>=etabarrelmin&&eta<=etabarrelmax) {
	    if (dorefdipt&&isdiref) {
	      fill_histo(refgendipt,weight,refgendipt,binsdipt,refDiPtVsRefDiPtBarrel);
	      fill_histo(jtrecdipt,weight,refgendipt,binsdipt,jetDiPtVsRefDiPtBarrel);
	      if (doflavor) {
		fill_histo(refpdgid[iref],refgendipt,weight,
			   refgendipt,binsdipt,refDiPtVsRefDiPtBarrel);
		fill_histo(refpdgid[iref],jtrecdipt,weight,
			   refgendipt,binsdipt,jetDiPtVsRefDiPtBarrel);
	      }
	    }
	    if (dorelrsp&&dorefdipt&&isdiref) {
	      fill_histo(direlrspgen[iref],weight,refgendipt,binsdipt,relRspVsRefDiPtBarrel);
	      if (doflavor) fill_histo(refpdgid[iref],direlrspgen[iref],weight,
				       refgendipt,binsdipt,relRspVsRefDiPtBarrel);
	    }
	    if (doabsrsp&&dorefdipt&&isdiref) {
	      fill_histo(diabsrspgen[iref],weight,refgendipt,binsdipt,absRspVsRefDiPtBarrel);
	      if (doflavor) fill_histo(refpdgid[iref],diabsrspgen[iref],weight,
				       refgendipt,binsdipt,absRspVsRefDiPtBarrel);
	    }
	  }
	
	  if (dojetdipt&&isdijet) {
	    fill_histo(jtrecdipt,weight,jtrecdipt, binsdipt,jetDiPtVsJetDiPt);
	    if (doflavor)
	      fill_histo(refrecpdgid[iref],jtrecdipt,weight,
			 jtrecdipt,binsdipt,jetDiPtVsJetDiPt);
	  }

	  if (dorefdipt&&isdiref) {
	    fill_histo(refgendipt,weight,refgendipt,binsdipt,refDiPtVsRefDiPt);
	    fill_histo(jtrecdipt,weight,refgendipt,binsdipt,jetDiPtVsRefDiPt);
	    if (doflavor) {
	      fill_histo(refpdgid[iref],refgendipt,weight,
			 refgendipt,binsdipt,refDiPtVsRefDiPt);
	      fill_histo(refpdgid[iref],jtrecdipt,weight,
			 refgendipt,binsdipt,jetDiPtVsRefDiPt);
	    }
	  }
	
	  //fill_histo(eta,weight,eta,binseta,jetEtaVsJetEta);
	  //if (doflavor) fill_histo(refpdgid[iref],eta,weight,
	  //			   eta,binseta,jetEtaVsJetEta);
	
	  //fill_histo(jtphi[iref],weight,jtphi[iref],binsphi,jetPhiVsJetPhi);
	  //if (doflavor) fill_histo(refpdgid[iref],jtphi[iref],weight,
	  //			   jtphi[iref],binsphi,jetPhiVsJetPhi);
 
	  //fill_histo(jty[iref],weight,jty[iref],binsy,jetYVsJetY);
	  //if (doflavor) fill_histo(refpdgid[iref],jty[iref],weight,
	  //			   jty[iref],binsy,jetYVsJetY);
 
	  if (dojetdipt&&isdijet) {
	    fill_histo(jtrecdipt,weight,eta,jtrecdipt,
		       binseta,binsdipt,jetDiPtVsJetEtaJetDiPt);
	    fill_histo(jtrecdipt,weight,y,jtrecdipt,
		       binsy,binsdipt,jetDiPtVsJetYJetDiPt);

	    if (doflavor) {
	      fill_histo(refrecpdgid[iref],jtrecdipt,weight,
			 eta,jtrecdipt,binseta,binsdipt,jetDiPtVsJetEtaJetDiPt);
	      fill_histo(refrecpdgid[iref],jtrecdipt,weight,
			 y,jtrecdipt,binsy,binsdipt,jetDiPtVsJetYJetDiPt);
	    }
	  }

	  if (dorefdipt&&isdiref) {
	    fill_histo(refgendipt,weight,eta,refgendipt,
		       binseta,binsdipt,refDiPtVsJetEtaRefDiPt);
	    fill_histo(jtrecdipt,weight,eta,refgendipt,
		       binseta,binsdipt,jetDiPtVsJetEtaRefDiPt);
	    fill_histo(refgendipt,weight,y,refgendipt,
		       binsy,binsdipt,refDiPtVsJetYRefDiPt);
	    fill_histo(jtrecdipt,weight,y,refgendipt,
		       binsy,binsdipt,jetDiPtVsJetYRefDiPt);
	    if (doflavor) {
	      fill_histo(refpdgid[iref],refgendipt,weight,
			 eta,refgendipt,binseta,binsdipt,refDiPtVsJetEtaRefDiPt);
	      fill_histo(refpdgid[iref],jtrecdipt,weight,
			 eta,refgendipt,binseta,binsdipt,jetDiPtVsJetEtaRefDiPt);
	      fill_histo(refpdgid[iref],refgendipt,weight,
			 y,refgendipt,binsy,binsdipt,refDiPtVsJetYRefDiPt);
	      fill_histo(refpdgid[iref],jtrecdipt,weight,
			 y,refgendipt,binsy,binsdipt,jetDiPtVsJetYRefDiPt);
	    }
	  }
	
	  if (dorelrsp) {
	    if (dojetdipt&&isdijet) {
	      fill_histo(direlrsprec[iref],weight,jtrecdipt,binsdipt,relRspVsJetDiPt);
	      if (doflavor) fill_histo(refrecpdgid[iref],direlrsprec[iref],weight,
				       jtrecdipt, binsdipt,relRspVsJetDiPt);
	    }
	    if (dorefdipt&&isdiref) {
	      fill_histo(direlrspgen[iref],weight,refgendipt,binsdipt,relRspVsRefDiPt);
	      if (doflavor) fill_histo(refpdgid[iref],direlrspgen[iref],weight,
				       refgendipt,binsdipt,relRspVsRefDiPt);
	    }

	    //fill_histo(direlrspgen[iref],weight,eta,binseta,relRspVsJetEta);
	    //if (doflavor) fill_histo(refpdgid[iref],direlrspgen[iref],weight,
	    //			     eta,binseta,relRspVsJetEta);
	  
	    //fill_histo(direlrspgen[iref],weight,jtphi[iref],binsphi,relRspVsJetPhi);
	    //if (doflavor) fill_histo(refpdgid[iref],direlrspgen[iref],weight,
	    //			     jtphi[iref],binsphi,relRspVsJetPhi);
	  
	    //fill_histo(direlrspgen[iref],weight,jty[iref],binsy,relRspVsJetY);
	    //if (doflavor) fill_histo(refpdgid[iref],direlrspgen[iref],weight,
	    //			     jty[iref],binsy,relRspVsJetY);
	  
	    if (dojetdipt&&isdijet) {
	      fill_histo(direlrsprec[iref],weight,eta,jtrecdipt,
			 binseta,binsdipt,relRspVsJetEtaJetDiPt);
	      fill_histo(direlrsprec[iref],weight,y,jtrecdipt,
			 binsy,binsdipt,relRspVsJetYJetDiPt);
	      if (doflavor) {
		fill_histo(refrecpdgid[iref],direlrsprec[iref],weight,eta,jtrecdipt,
			   binseta,binsdipt,relRspVsJetEtaJetDiPt);
		fill_histo(refrecpdgid[iref],direlrsprec[iref],weight,y,jtrecdipt,
			   binsy,binsdipt,relRspVsJetYJetDiPt);
	      }
	    }
	    if (dorefdipt&&isdiref) {
	      fill_histo(direlrspgen[iref],weight,eta,refgendipt,
			 binseta,binsdipt,relRspVsJetEtaRefDiPt);
	      fill_histo(direlrspgen[iref],weight,y,refgendipt,
			 binsy,binsdipt,relRspVsJetYRefDiPt);
	      if (doflavor) {
		fill_histo(refpdgid[iref],direlrspgen[iref],weight,eta,refgendipt,
			   binseta,binsdipt,relRspVsJetEtaRefDiPt);
		fill_histo(refpdgid[iref],direlrspgen[iref],weight,y,refgendipt,
			   binsy,binsdipt,relRspVsJetYRefDiPt);
	      }
	    }
	  }
	
	  if (doabsrsp) {
	    if (dojetdipt&&isdijet) {
	      fill_histo(diabsrsprec[iref],weight,jtrecdipt, binsdipt,absRspVsJetDiPt);
	      if (doflavor) fill_histo(refrecpdgid[iref],diabsrsprec[iref],weight,
				       jtrecdipt,binsdipt,absRspVsJetDiPt);
	    }
	    if (dorefdipt&&isdiref) {
	      fill_histo(diabsrspgen[iref],weight,refgendipt,binsdipt,absRspVsRefDiPt);
	      if (doflavor) fill_histo(refpdgid[iref],diabsrspgen[iref],weight,
				       refgendipt,binsdipt,absRspVsRefDiPt);
	    }
	  
	    //fill_histo(diabsrspgen[iref],weight,eta,binseta,absRspVsJetEta);
	    //if (doflavor) fill_histo(refpdgid[iref],diabsrspgen[iref],weight,
	    //			     eta,binseta,absRspVsJetEta);

	    //fill_histo(diabsrspgen[iref],weight,jtphi[iref],binsphi,absRspVsJetPhi);
	    //if (doflavor) fill_histo(refpdgid[iref],diabsrspgen[iref],weight,
	    //			     jtphi[iref],binsphi,absRspVsJetPhi);
	  
	    //fill_histo(diabsrspgen[iref],weight,jty[iref],binsy,absRspVsJetY);
	    //if (doflavor) fill_histo(refpdgid[iref],diabsrspgen[iref],weight,
	    //			     jty[iref],binsy,absRspVsJetY);
	  
	    if (dojetdipt&&isdijet) {
	      fill_histo(diabsrsprec[iref],weight,eta,jtrecdipt,
			 binseta,binsdipt,absRspVsJetEtaJetDiPt);
	      fill_histo(diabsrsprec[iref],weight,y,jtrecdipt,
			 binsy,binsdipt,absRspVsJetYJetDiPt);
	      if (doflavor) {
		fill_histo(refrecpdgid[iref],diabsrsprec[iref],weight,eta,jtrecdipt,
			   binseta,binsdipt,absRspVsJetEtaJetDiPt);
		fill_histo(refrecpdgid[iref],diabsrsprec[iref],weight,y,jtrecdipt,
			   binsy,binsdipt,absRspVsJetYJetDiPt);
	      }
	    }
	    if (dorefdipt&&isdiref) {
	      fill_histo(diabsrspgen[iref],weight,eta,refgendipt,
			 binseta,binsdipt,absRspVsJetEtaRefDiPt);
	      fill_histo(diabsrspgen[iref],weight,y,refgendipt,
			 binsy,binsdipt,absRspVsJetYRefDiPt);
	      if (doflavor) {
		fill_histo(refpdgid[iref],diabsrspgen[iref],weight,eta,refgendipt,
			   binseta,binsdipt,absRspVsJetEtaRefDiPt);
		fill_histo(refpdgid[iref],diabsrspgen[iref],weight,y,refgendipt,
			   binsy,binsdipt,absRspVsJetYRefDiPt);
	      }
	    }
	  }
	
	  if (doetarsp) {
	    if (dojetdipt&&isdijet) {
	      fill_histo(dietarsprec[iref],weight,jtrecdipt, binsdipt,etaRspVsJetDiPt);
	      if (doflavor) fill_histo(refrecpdgid[iref],dietarsprec[iref],weight,
				       jtrecdipt,binsdipt,etaRspVsJetDiPt);
	    }
	    if (dorefdipt&&isdiref) {
	      fill_histo(dietarspgen[iref],weight,refgendipt,binsdipt,etaRspVsRefDiPt);
	      if (doflavor) fill_histo(refpdgid[iref],dietarspgen[iref],weight,
				       refgendipt,binsdipt,etaRspVsRefDiPt);
	    }
	  
	    //fill_histo(dietarspgen[iref],weight,eta,binseta,etaRspVsJetEta);
	    //if (doflavor) fill_histo(refpdgid[iref],dietarspgen[iref],weight,
	    //			     eta,binseta,etaRspVsJetEta);
	  
	    //fill_histo(dietarspgen[iref],weight,jtphi[iref],binsphi,etaRspVsJetPhi);
	    //if (doflavor) fill_histo(refpdgid[iref],dietarspgen[iref],weight,
	    //			     jtphi[iref],binsphi,etaRspVsJetPhi);
	  
	    if (dojetdipt&&isdijet) {
	      fill_histo(dietarsprec[iref],weight,eta,jtrecdipt,
			 binseta,binsdipt,etaRspVsJetEtaJetDiPt);
	      if (doflavor) fill_histo(refrecpdgid[iref],dietarsprec[iref],weight,
				       eta,jtrecdipt,
				       binseta,binsdipt,etaRspVsJetEtaJetDiPt);
	    }
	    if (dorefdipt&&isdiref) {
	      fill_histo(dietarspgen[iref],weight,eta,refgendipt,
			 binseta,binsdipt,etaRspVsJetEtaRefDiPt);
	      if (doflavor) fill_histo(refpdgid[iref],dietarspgen[iref],weight,
				       eta,refgendipt,
				       binseta,binsdipt,etaRspVsJetEtaRefDiPt);
	    }
	  }
	
	  if (dophirsp) {
	    if (dojetdipt&&isdijet) {
	      fill_histo(diphirsprec[iref],weight,jtrecdipt, binsdipt,phiRspVsJetDiPt);
	      if (doflavor) fill_histo(refrecpdgid[iref],diphirsprec[iref],weight,
				       jtrecdipt,binsdipt,phiRspVsJetDiPt);
	    }
	    if (dorefdipt&&isdiref) {
	      fill_histo(diphirspgen[iref],weight,refgendipt,binsdipt,phiRspVsRefDiPt);
	      if (doflavor) fill_histo(refpdgid[iref],diphirspgen[iref],weight,
				       refgendipt,binsdipt,phiRspVsRefDiPt);
	    }
	  
	    //fill_histo(diphirspgen[iref],weight,eta,binseta,phiRspVsJetEta);
	    //if (doflavor) fill_histo(refpdgid[iref],diphirspgen[iref],weight,
	    //			     eta,binseta,phiRspVsJetEta);
	  
	    //fill_histo(diphirspgen[iref],weight,jtphi[iref],binsphi,phiRspVsJetPhi);
	    //if (doflavor) fill_histo(refpdgid[iref],diphirspgen[iref],weight,
	    //			     jtphi[iref],binsphi,phiRspVsJetPhi);
	  
	    if (dojetdipt&&isdijet) {
	      fill_histo(diphirsprec[iref],weight,eta,jtrecdipt,
			 binseta,binsdipt,phiRspVsJetEtaJetDiPt);
	      if (doflavor) fill_histo(refrecpdgid[iref],diphirsprec[iref],weight,
				       eta,jtrecdipt,
				       binseta,binsdipt,phiRspVsJetEtaJetDiPt);
	    }
	    if (dorefdipt&&isdiref) {
	      fill_histo(diphirspgen[iref],weight,eta,refgendipt,
			 binseta,binsdipt,phiRspVsJetEtaRefDiPt);
	      if (doflavor) fill_histo(refpdgid[iref],diphirspgen[iref],weight,
				       eta,refgendipt,
				       binseta,binsdipt,phiRspVsJetEtaRefDiPt);
	    }
	  }

	} //hh

      } // for iref

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
void fill_histo(int pdgid,float value,float weight,float x,
		const vector<float>& binsx,const vector<TH1F**>& histos)
{
  if (binsx.size()==0) return;
  int abspdgid=fabs(pdgid);
  int iflv(-1);
  if (abspdgid>=1&&abspdgid<=3) iflv=1;
  else if (abspdgid== 4)        iflv=2;
  else if (abspdgid== 5)        iflv=3;
  else if (abspdgid==21)        iflv=4;
  else if (abspdgid==411||
	   abspdgid==413)       iflv=5;
  else if (abspdgid==511||
	   abspdgid==513)       iflv=6;
  else return;

  int ix=get_index(x,binsx);
  if (ix>=0) histos[ix][iflv]->Fill(value,weight);
}


//______________________________________________________________________________
void fill_histo(float value,float weight,float x,float y,
		const vector<float>& binsx,const vector<float>& binsy,
		const vector<TH1F***>& histos)
{
  if (binsx.size()==0||binsy.size()==0) return;
  int ix=get_index(x,binsx);
  int iy=get_index(y,binsy);
  if (ix>=0&&iy>=0) histos[ix][iy][0]->Fill(value,weight);
}


//______________________________________________________________________________
void fill_histo(int pdgid,float value,float weight,float x,float y,
		const vector<float>& binsx,const vector<float>& binsy,
		const vector<TH1F***>& histos)
{
  if (binsx.size()==0||binsy.size()==0) return;
  int abspdgid=fabs(pdgid);
  int iflv(-1);
  if (abspdgid>=1&&abspdgid<=3) iflv=1;
  else if (abspdgid== 4)        iflv=2;
  else if (abspdgid== 5)        iflv=3;
  else if (abspdgid==21)        iflv=4;
  else if (abspdgid==411||
	   abspdgid==413)       iflv=5;
  else if (abspdgid==511||
	   abspdgid==513)       iflv=6;
  else return;
  
  int ix=get_index(x,binsx);
  int iy=get_index(y,binsy);
  if (ix>=0&&iy>=0) histos[ix][iy][iflv]->Fill(value,weight);
}


//______________________________________________________________________________
bool contains(const vector<string>& collection,const string& element)
{
  vector<string>::const_iterator it;
  for (it=collection.begin();it!=collection.end();++it)
    if ((*it)==element) return true;
  return false;
}

//_______________________________________________________________________________
//inline float deltaPhi(float phi1, float phi2) { 
//    float result = phi1 - phi2;
//    while (result > float(M_PI)) result -= float(2*M_PI);
//    while (result <= -float(M_PI)) result += float(2*M_PI);
//    return result;
//}

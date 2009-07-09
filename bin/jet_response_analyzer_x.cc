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
  string         treename     = cl.getValue<string> ("treename",     "t");
  string         output       = cl.getValue<string> ("output","jra.root");
  float          xsection     = cl.getValue<float>  ("xsection",     0.0);
  int            nrefmax      = cl.getValue<int>    ("nrefmax",        0);
  int            nbinspt      = cl.getValue<int>    ("nbinspt",       50);
  int            nbinseta     = cl.getValue<int>    ("nbinseta",      25);
  int            nbinsphi     = cl.getValue<int>    ("nbinsphi",      25);
  float          etabarrelmin = cl.getValue<float>  ("etabarrelmin",-1.3);
  float          etabarrelmax = cl.getValue<float>  ("etabarrelmax",+1.3);
  bool           dobalance    = cl.getValue<bool>   ("dobalance",  false);
  bool           doflavor     = cl.getValue<bool>   ("doflavor",   false);
  float          drmax        = cl.getValue<float>  ("drmax",        0.3);
  float          dphimin      = cl.getValue<float>  ("dphimin",      2.7);
  bool           dojetpt      = cl.getValue<bool>   ("dojetpt",    false);
  bool           dorefpt      = cl.getValue<bool>   ("dorefpt",     true);
  int            nbinsrelrsp  = cl.getValue<int>    ("nbinsrelrsp",   50);
  float          relrspmin    = cl.getValue<float>  ("relrspmin",    0.0);
  float          relrspmax    = cl.getValue<float>  ("relrspmax",    2.0);
  int            nbinsabsrsp  = cl.getValue<int>    ("nbinsabsrsp",  150);
  float          absrspmin    = cl.getValue<float>  ("absrspmin", -250.0);
  float          absrspmax    = cl.getValue<float>  ("absrspmax",  100.0);
  int            nbinsetarsp  = cl.getValue<int>    ("nbinsetarsp",  100);
  float          etarspmin    = cl.getValue<float>  ("etarspmin",   -1.0);
  float          etarspmax    = cl.getValue<float>  ("etarspmax",    1.0);
  int            nbinsphirsp  = cl.getValue<int>    ("nbinsphirsp",  100);
  float          phirspmin    = cl.getValue<float>  ("phirspmin",   -1.0);
  float          phirspmax    = cl.getValue<float>  ("phirspmax",    1.0);
  vector<string> algs         = cl.getVector<string>("algs",          "");
  
  if (!cl.check()) return 0;
  cl.print();

  bool dorelrsp=(nbinsrelrsp>0);
  bool doabsrsp=(nbinsabsrsp>0);
  bool doetarsp=(nbinsetarsp>0);
  bool dophirsp=(nbinsphirsp>0);

  if (xsection>0.0) TH1::SetDefaultSumw2(true);


  //
  // evaluate drmin requirements for individual algorithms if provided
  //
  map<std::string,float> alg2drmax;
  for (unsigned int ialg=0;ialg<algs.size();ialg++) {
    string alg=algs[ialg];
    unsigned pos = alg.find(':');
    if (pos!=string::npos) {
      float drmax_alg; stringstream ss; ss<<alg.substr(pos+1); ss>>drmax_alg;
      alg=alg.substr(0,pos);
      alg2drmax[alg]=drmax_alg;
      algs[ialg]=alg;
      cout<<"drmax("<<alg<<") = "<<alg2drmax[alg]<<endl;
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
  
    float weight = (xsection>0.0) ? xsection/tree->GetEntries() : 1.0;
    float drmax_alg = drmax;
    if (alg2drmax.find(alg)!=alg2drmax.end()) drmax_alg=alg2drmax[alg];
    //cout<<"weight="<<weight<<", drmax_alg="<<drmax_alg<<endl;
    

    //
    // setup the tree for reading
    //
    unsigned char nref;
    int   refpdgid[100];
    float refpt[100];
    float refeta[100];
    float refphi[100];
    float jtpt[100];
    float jteta[100];
    float jtphi[100];
    float refdrjt[100];
    float refdphijt[100];
    
    tree->SetBranchAddress("nref",   &nref);
    if (doflavor) tree->SetBranchAddress("refpdgid",refpdgid);
    tree->SetBranchAddress("refpt",   refpt);
    tree->SetBranchAddress("refeta",  refeta);
    tree->SetBranchAddress("refphi",  refphi);
    tree->SetBranchAddress("jtpt",    jtpt);
    tree->SetBranchAddress("jteta",   jteta);
    tree->SetBranchAddress("jtphi",   jtphi);
    
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
    vector<TH1F**>  refPtVsRefPtBarrel;
    vector<TH1F**>  jetPtVsRefPtBarrel;
    vector<TH1F**>  jetEtaVsJetEta;
    vector<TH1F**>  jetPhiVsJetPhi;
    vector<TH1F***> jetPtVsJetEtaJetPt;
    vector<TH1F***> refPtVsJetEtaRefPt;
    vector<TH1F***> jetPtVsJetEtaRefPt;
    
    vector<TH1F**>  relRspVsJetPt;
    vector<TH1F**>  relRspVsRefPt;
    vector<TH1F**>  relRspVsRefPtBarrel;
    vector<TH1F**>  relRspVsJetEta;
    vector<TH1F**>  relRspVsJetPhi;
    vector<TH1F***> relRspVsJetEtaJetPt;
    vector<TH1F***> relRspVsJetEtaRefPt;
    
    vector<TH1F**>  absRspVsJetPt;
    vector<TH1F**>  absRspVsRefPt;
    vector<TH1F**>  absRspVsRefPtBarrel;
    vector<TH1F**>  absRspVsJetEta;
    vector<TH1F**>  absRspVsJetPhi;
    vector<TH1F***> absRspVsJetEtaJetPt;
    vector<TH1F***> absRspVsJetEtaRefPt;

    vector<TH1F**>  etaRspVsJetPt;
    vector<TH1F**>  etaRspVsRefPt;
    vector<TH1F**>  etaRspVsJetEta;
    vector<TH1F**>  etaRspVsJetPhi;
    vector<TH1F***> etaRspVsJetEtaJetPt;
    vector<TH1F***> etaRspVsJetEtaRefPt;

    vector<TH1F**>  phiRspVsJetPt;
    vector<TH1F**>  phiRspVsRefPt;
    vector<TH1F**>  phiRspVsJetEta;
    vector<TH1F**>  phiRspVsJetPhi;
    vector<TH1F***> phiRspVsJetEtaJetPt;
    vector<TH1F***> phiRspVsJetEtaRefPt;
    

    // define flavors
    vector<string> flavor;
    flavor.push_back("");
    if (doflavor) {
      flavor.push_back("uds_");
      flavor.push_back("c_");
      flavor.push_back("b_");
      flavor.push_back("g_");
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
					       2*nbinspt,
					       (ptmin>100.)*0.25*ptmin,
					       1.25*ptmax);
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
						     2*nbinspt,
						     (ptmin>100.)*0.25*ptmin,
						     1.25*ptmax);
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
					     2*nbinspt,
					     (ptmin>100.)*0.25*ptmin,
					     1.25*ptmax);
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
    
    //
    // fill histograms
    //
    unsigned int nevt = (unsigned int)tree->GetEntries();
    for (unsigned int ievt=0;ievt<nevt;ievt++) {
      tree->GetEntry(ievt);
      if (nrefmax>0) nref = std::min((int)nref,nrefmax);
      for (unsigned char iref=0;iref<nref;iref++) {
	if (( dobalance&&refdphijt[iref]<dphimin)||
	    (!dobalance&&refdrjt[iref]>drmax)) continue;
	
	float eta    =
	  (binseta.size()&&binseta.front()>=0.)?std::abs(jteta[iref]):jteta[iref];
	float absrsp = jtpt[iref]-refpt[iref];
	float relrsp = jtpt[iref]/refpt[iref];
	float etarsp = std::abs(jteta[iref])-std::abs(refeta[iref]);
	float phirsp = jtphi[iref]-refphi[iref];
	
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
 
	if (dojetpt) {
	  fill_histo(jtpt[iref],weight,eta,jtpt[iref],
		     binseta,binspt,jetPtVsJetEtaJetPt);
	  if (doflavor)
	    fill_histo(refpdgid[iref],jtpt[iref],weight,
		       eta,jtpt[iref],binseta,binspt,jetPtVsJetEtaJetPt);
	}

	if (dorefpt) {
	  fill_histo(refpt[iref],weight,eta,refpt[iref],
		     binseta,binspt,refPtVsJetEtaRefPt);
	  fill_histo(jtpt [iref],weight,eta,refpt[iref],
		     binseta,binspt,jetPtVsJetEtaRefPt);
	  if (doflavor) {
	    fill_histo(refpdgid[iref],refpt[iref],weight,
		       eta,refpt[iref],binseta,binspt,refPtVsJetEtaRefPt);
	    fill_histo(refpdgid[iref],jtpt[iref],weight,
		       eta,refpt[iref],binseta,binspt,jetPtVsJetEtaRefPt);
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
	  
	  if (dojetpt) {
	    fill_histo(relrsp,weight,eta,jtpt[iref],
		       binseta,binspt,relRspVsJetEtaJetPt);
	    if (doflavor) fill_histo(refpdgid[iref],relrsp,weight,
				     eta,jtpt[iref],
				     binseta,binspt,relRspVsJetEtaJetPt);
	  }
	  if (dorefpt) {
	    fill_histo(relrsp,weight,eta,refpt[iref],
		       binseta,binspt,relRspVsJetEtaRefPt);
	    if (doflavor) fill_histo(refpdgid[iref],relrsp,weight,
				     eta,refpt[iref],
				     binseta,binspt,relRspVsJetEtaRefPt);
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
	  
	  if (dojetpt) {
	    fill_histo(absrsp,weight,eta,jtpt[iref],
		       binseta,binspt,absRspVsJetEtaJetPt);
	    if (doflavor) fill_histo(refpdgid[iref],absrsp,weight,
				     eta,jtpt[iref],
				     binseta,binspt,absRspVsJetEtaJetPt);
	  }
	  if (dorefpt) {
	    fill_histo(absrsp,weight,eta,refpt[iref],
		       binseta,binspt,absRspVsJetEtaRefPt);
	    if (doflavor) fill_histo(refpdgid[iref],absrsp,weight,
				     eta,refpt[iref],
				     binseta,binspt,absRspVsJetEtaRefPt);
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
void fill_histo(int pdgid,float value,float weight,float x,
		const vector<float>& binsx,const vector<TH1F**>& histos)
{
  if (binsx.size()==0) return;
  int abspdgid=std::abs(pdgid);
  int iflv(-1);
  if (abspdgid>=1&&abspdgid<=3) iflv=1;
  else if (abspdgid== 4)        iflv=2;
  else if (abspdgid== 5)        iflv=3;
  else if (abspdgid==21)        iflv=4;
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
  int abspdgid=std::abs(pdgid);
  int iflv(-1);
  if (abspdgid>=1&&abspdgid<=3) iflv=1;
  else if (abspdgid== 4)        iflv=2;
  else if (abspdgid== 5)        iflv=3;
  else if (abspdgid==21)        iflv=4;
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

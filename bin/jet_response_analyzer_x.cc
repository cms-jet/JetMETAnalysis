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
#include <TH2F.h>
#include <TKey.h>
#include <TEventList.h>

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

/// get the flavors if the option doflavors is specified
vector<string> get_flavors(bool noabsflavors);

/// return the flavor name (string) given a specific pdgid
TString pdgid_to_flavor_name(int pdgid);

/// fill the appropriate histogram (histos), based on x and binsx
void fill_histo(float value,float weight,float x,
                const vector<float>& binsx,const vector<TH1F**>& histos);

/// fill the appropriate histogram (histos), based on pdgid, x and binsx
void fill_histo(int pdgid,float value,float weight,float x,
                const vector<float>& binsx,const vector<TH1F**>& histos,
                bool noabsflavors);

/// fill the appropriate histogram (histos), based on x, y, binsx, and binsy
void fill_histo(float value,float weight,float x,float y,
                const vector<float>& binsx,const vector<float>& binsy,
                const vector<TH1F***>& histos);

/// fill the appropriate histogram (histos), based on pdgid, x, y, binsx, and binsy
void fill_histo(int pdgid,float value,float weight,float x,float y,
                const vector<float>& binsx,const vector<float>& binsy,
                const vector<TH1F***>& histos, bool noabsflavors);

/// check if a vector of strings contains a certain element
bool contains(const vector<string>& collection,const string& element);

/// check the amount of IT pileup and see if it is in the specified range
bool it_pileup(int itlow, int ithigh, vector<int>* npus);

/// check the amount of OOT pileup before nad after the event and see if it is in the specified range
bool oot_pileup(int earlyootlow, int earlyoothigh, int lateootlow, int lateoothigh,
                vector<int>* npus);

/// check the sum of the OOT pileup before and after the event and see if it is in the specified range
bool total_oot_pileup(int totalootlow, int totaloothigh, vector<int>* npus);

/// combines the booleans from the IT, OOT, and TotalOOT functions into one boolean
bool pileup_cut(int itlow, int ithigh, int earlyootlow, int earlyoothigh, 
                int lateootlow, int lateoothigh, int totalootlow, int totaloothigh, 
                vector<int>* npus);

//______________________________________________________________________________
int main(int argc,char**argv)
{
  //
  // evaluate command-line / configuration file options
  //
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;

  string         input             = cl.getValue<string> ("input");
  vector<float>  binspt            = cl.getVector<float> ("binspt",               "");
  vector<float>  binseta           = cl.getVector<float> ("binseta",              "");
  vector<float>  binsphi           = cl.getVector<float> ("binsphi",              "");
  vector<float>  binsy             = cl.getVector<float> ("binsy",                "");
  string         treename          = cl.getValue<string> ("treename",            "t");
  string         output            = cl.getValue<string> ("output",       "jra.root");
  bool           useweight         = cl.getValue<bool>   ("useweight",         false);
  float          xsection          = cl.getValue<float>  ("xsection",            0.0);
  int            nrefmax           = cl.getValue<int>    ("nrefmax",               0);
  int            nbinspt           = cl.getValue<int>    ("nbinspt",              50);
  int            nbinseta          = cl.getValue<int>    ("nbinseta",             25);
  int            nbinsphi          = cl.getValue<int>    ("nbinsphi",             25);
  int            nbinsy            = cl.getValue<int>    ("nbinsy",               25);
  float          etabarrelmin      = cl.getValue<float>  ("etabarrelmin",       -1.3);
  float          etabarrelmax      = cl.getValue<float>  ("etabarrelmax",       +1.3);
  bool           dobalance         = cl.getValue<bool>   ("dobalance",         false);
  bool           doflavor          = cl.getValue<bool>   ("doflavor",          false);
  bool           noabsflavors      = cl.getValue<bool>   ("noabsflavors",      false);
  float          drmax             = cl.getValue<float>  ("drmax",               0.3);
  float          dphimin           = cl.getValue<float>  ("dphimin",             2.7);
  bool           dojetpt           = cl.getValue<bool>   ("dojetpt",           false);
  bool           dorefpt           = cl.getValue<bool>   ("dorefpt",            true);
  int            nbinsrelrsp       = cl.getValue<int>    ("nbinsrelrsp",          50);
  float          relrspmin         = cl.getValue<float>  ("relrspmin",           0.0);
  float          relrspmax         = cl.getValue<float>  ("relrspmax",           2.0);
  int            nbinsabsrsp       = cl.getValue<int>    ("nbinsabsrsp",           0);
  float          absrspmin         = cl.getValue<float>  ("absrspmin",        -250.0);
  float          absrspmax         = cl.getValue<float>  ("absrspmax",         100.0);
  int            nbinsetarsp       = cl.getValue<int>    ("nbinsetarsp",         100);
  float          etarspmin         = cl.getValue<float>  ("etarspmin",          -1.0);
  float          etarspmax         = cl.getValue<float>  ("etarspmax",           1.0);
  int            nbinsphirsp       = cl.getValue<int>    ("nbinsphirsp",         100);
  float          phirspmin         = cl.getValue<float>  ("phirspmin",          -1.0);
  float          phirspmax         = cl.getValue<float>  ("phirspmax",           1.0);
  float          jtptmin           = cl.getValue<float>  ("jtptmin",             1.0);
  vector<string> algs              = cl.getVector<string>("algs",                 "");
  vector<string> presel            = cl.getVector<string>("presel",               "");
  int            itlow             = cl.getValue<int>    ("itlow",                 0);
  int            ithigh            = cl.getValue<int>    ("ithigh",             1000);
  int            earlyootlow       = cl.getValue<int>    ("earlyootlow",           0);
  int            earlyoothigh      = cl.getValue<int>    ("earlyoothigh",       1000);
  int            lateootlow        = cl.getValue<int>    ("lateootlow",            0);
  int            lateoothigh       = cl.getValue<int>    ("lateoothigh",        1000);
  int            totalootlow       = cl.getValue<int>    ("totalootlow",           0);
  int            totaloothigh      = cl.getValue<int>    ("totaloothigh",       1000);
  TString         weightfile       = cl.getValue<TString>("weightfile",           "");

  if (!cl.check()) return 0;
  cl.print();

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
    else {
       cout << "WARNING::drmax not specified for algorithm " << alg << "." << endl
            << " Using default drmax = " << drmax << endl
            << " To specify drmax for a given algorithm, use \"alg:drmax\"." << endl;
    }
  }
  
  // define flavors
  vector<string> flavor;
  if(doflavor) flavor = get_flavors(noabsflavors);
  else flavor.push_back("");

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
    // get weights
    //
    TH1D* weightHist = 0;
    map<TString,TH2D*> weightMap;
    if(!weightfile.IsNull())
    {
       TFile* wfile = new TFile(weightfile,"READ");
       if (!wfile->IsOpen()) { cout<<"Can't open "<<weightfile<<endl; }
       gDirectory->cd(alg.c_str());
       if(!doflavor)
       {
          weightHist = (TH1D*)wfile->Get("we");
          if (weightHist==0) { cout<<"weightHist named \"we\" was not in file "<<weightfile<<endl; return 0; } 
          weightHist->Scale(1./weightHist->Integral(1,weightHist->FindBin(3)));
       }
       else
       {
          for(unsigned int f=0; f<flavor.size(); f++)
          {
             if(flavor[f]!="")
                weightMap[flavor[f]] = (TH2D*)gDirectory->Get(flavor[f].c_str());
             else
                weightMap[flavor[f]] = (TH2D*)gDirectory->Get("all_");
             if(weightMap[flavor[f]]==0) { cout<<"weightHist named \""<< flavor[f]<< "\" was not in file "<<weightfile<<endl; return 0; } 
          }
       }
    }

    //
    // setup the tree for reading
    //
    unsigned char nref;
    float weight(1.0);
    float flavorWeight(1.0);
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
    vector<int>* npus = new vector<int>;
    
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
    tree->SetBranchAddress("npus",    &npus);
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
    vector<TH1F**>  refPtVsRefPtBarrel;
    vector<TH1F**>  jetPtVsRefPtBarrel;
    vector<TH1F**>  jetEtaVsJetEta;
    vector<TH1F**>  jetPhiVsJetPhi;
    vector<TH1F**>  jetYVsJetY;
    vector<TH1F***> jetPtVsJetEtaJetPt;
    vector<TH1F***> refPtVsJetEtaRefPt;
    vector<TH1F***> jetPtVsJetEtaRefPt;
    vector<TH1F***> jetPtVsJetYJetPt;
    vector<TH1F***> refPtVsJetYRefPt;
    vector<TH1F***> jetPtVsJetYRefPt;
    
    vector<TH1F**>  relRspVsJetPt;
    vector<TH1F**>  relRspVsRefPt;
    vector<TH1F**>  relRspVsRefPtBarrel;
    vector<TH1F**>  relRspVsJetEta;
    vector<TH1F**>  relRspVsJetPhi;
    vector<TH1F**>  relRspVsJetY;
    vector<TH1F***> relRspVsJetEtaJetPt;
    vector<TH1F***> relRspVsJetEtaRefPt;
    vector<TH1F***> relRspVsJetYJetPt;
    vector<TH1F***> relRspVsJetYRefPt;
    
    vector<TH1F**>  absRspVsJetPt;
    vector<TH1F**>  absRspVsRefPt;
    vector<TH1F**>  absRspVsRefPtBarrel;
    vector<TH1F**>  absRspVsJetEta;
    vector<TH1F**>  absRspVsJetPhi;
    vector<TH1F**>  absRspVsJetY;
    vector<TH1F***> absRspVsJetEtaJetPt;
    vector<TH1F***> absRspVsJetEtaRefPt;
    vector<TH1F***> absRspVsJetYJetPt;
    vector<TH1F***> absRspVsJetYRefPt;

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
    

    //
    // fill histograms
    //
    TEventList* el = new TEventList("el","el");
    stringstream selection; selection<<"1";
    for (unsigned icut=0;icut<presel.size();icut++) selection<<"&&("<<presel[icut]<<")";
    if (presel.size()>0) cout<<"Selection: "<<selection.str()<<endl;
    tree->Draw(">>el",selection.str().c_str());
    cout<<"tree entries: "<<tree->GetEntries()<<" elist: "<<el->GetN()<<endl;
    
    unsigned int nevt = (unsigned) el->GetN();
    for (unsigned int ievt=0;ievt<nevt;ievt++)
      {
        const Long64_t ientry = el->GetEntry(ievt);
        tree->GetEntry(ientry);
        if (nrefmax>0) nref = std::min((int)nref,nrefmax);
        for (unsigned char iref=0;iref<nref;iref++) {
   
          if (( dobalance&&refdphijt[iref]<dphimin)||
              (!dobalance&&refdrjt[iref]>drmax_alg)) continue;
        
          if (jtpt[iref]<jtptmin) continue;

          if (!pileup_cut(itlow,ithigh,earlyootlow,earlyoothigh,lateootlow,lateoothigh,
                             totalootlow,totaloothigh,npus)) continue;

          float eta    =
            (binseta.size()&&binseta.front()>=0.)?std::abs(jteta[iref]):jteta[iref];
          float y      =
            (binsy.size()&&binsy.front()>=0.)?std::abs(jty[iref]):jty[iref];
          float pt     = jtpt[iref];
          float pdgid  = refpdgid[iref];

          float absrsp = jtpt[iref]-refpt[iref];
          float relrsp = jtpt[iref]/refpt[iref];
          float etarsp = jteta[iref]-refeta[iref];
          float phirsp = fmod(jtphi[iref]-refphi[iref]+3*M_PI,2*M_PI)-M_PI;

          //
          // retrieve the correct weight
          //
          if(!weightfile.IsNull())
          {
             if(!doflavor && log10(refpt[iref])<3)
             {
                weight = weightHist->GetBinContent(weightHist->FindBin(log10(refpt[iref])));
             }
             else if(doflavor)
             {
                weight = weightMap["all_"]->
                   GetBinContent(weightMap["all_"]->FindBin(pt,eta));
                if(noabsflavors)
                   flavorWeight = weightMap[pdgid_to_flavor_name(pdgid)]->
                      GetBinContent(weightMap[pdgid_to_flavor_name(pdgid)]->FindBin(pt,eta));
                else
                   flavorWeight = weightMap[pdgid_to_flavor_name(fabs(pdgid))]->
                      GetBinContent(weightMap[pdgid_to_flavor_name(fabs(pdgid))]->FindBin(pt,eta));
             }
          }
          else
             flavorWeight = weight;

          if (eta>=etabarrelmin&&eta<=etabarrelmax) {
            if (dorefpt) {
               fill_histo(refpt[iref],weight,refpt[iref],binspt,refPtVsRefPtBarrel);
               fill_histo(jtpt [iref],weight,refpt[iref],binspt,jetPtVsRefPtBarrel);
              if (doflavor) {
                fill_histo(refpdgid[iref],refpt[iref],flavorWeight,
                           refpt[iref],binspt,refPtVsRefPtBarrel,noabsflavors);
                fill_histo(refpdgid[iref],jtpt [iref],flavorWeight,
                           refpt[iref],binspt,jetPtVsRefPtBarrel,noabsflavors);
              }
            }
            if (dorelrsp&&dorefpt) {
               fill_histo(relrsp,weight,refpt[iref],binspt,relRspVsRefPtBarrel);
              if (doflavor) fill_histo(refpdgid[iref],relrsp,flavorWeight,
                                       refpt[iref],binspt,relRspVsRefPtBarrel,
                                       noabsflavors);
            }
            if (doabsrsp&&dorefpt) {
               fill_histo(absrsp,weight,refpt[iref],binspt,absRspVsRefPtBarrel);
              if (doflavor) fill_histo(refpdgid[iref],absrsp,flavorWeight,
                                       refpt[iref],binspt,absRspVsRefPtBarrel,
                                       noabsflavors);
            }
          }

          if (dojetpt) {
            fill_histo(jtpt[iref],weight,jtpt[iref], binspt,jetPtVsJetPt);
            if (doflavor)
              fill_histo(refpdgid[iref],jtpt[iref],flavorWeight,
                         jtpt[iref],binspt,jetPtVsJetPt,noabsflavors);
          }
          if (dorefpt) {
            fill_histo(refpt[iref],weight,refpt[iref],binspt,refPtVsRefPt);
            fill_histo(jtpt [iref],weight,refpt[iref],binspt,jetPtVsRefPt);
            if (doflavor) {
              fill_histo(refpdgid[iref],refpt[iref],flavorWeight,
                         refpt[iref],binspt,refPtVsRefPt,noabsflavors);
              fill_histo(refpdgid[iref],jtpt[iref],flavorWeight,
                         refpt[iref],binspt,jetPtVsRefPt,noabsflavors);
            }
          }
	
          fill_histo(eta,weight,eta,binseta,jetEtaVsJetEta);
          if (doflavor) fill_histo(refpdgid[iref],eta,flavorWeight,
                                   eta,binseta,jetEtaVsJetEta,noabsflavors);
	
          fill_histo(jtphi[iref],weight,jtphi[iref],binsphi,jetPhiVsJetPhi);
          if (doflavor) fill_histo(refpdgid[iref],jtphi[iref],flavorWeight,
                                   jtphi[iref],binsphi,jetPhiVsJetPhi,
                                   noabsflavors);
 
          fill_histo(jty[iref],weight,jty[iref],binsy,jetYVsJetY);
          if (doflavor) fill_histo(refpdgid[iref],jty[iref],flavorWeight,
                                   jty[iref],binsy,jetYVsJetY,noabsflavors);

          if (dojetpt) {
            fill_histo(jtpt[iref],weight,eta,jtpt[iref],
                       binseta,binspt,jetPtVsJetEtaJetPt);
            fill_histo(jtpt[iref],weight,y,jtpt[iref],
                       binsy,binspt,jetPtVsJetYJetPt);

            if (doflavor) {
              fill_histo(refpdgid[iref],jtpt[iref],flavorWeight,
                         eta,jtpt[iref],binseta,binspt,jetPtVsJetEtaJetPt,
                         noabsflavors);
              fill_histo(refpdgid[iref],jtpt[iref],flavorWeight,
                         y,jtpt[iref],binsy,binspt,jetPtVsJetYJetPt,
                         noabsflavors);
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
              fill_histo(refpdgid[iref],refpt[iref],flavorWeight,
                         eta,refpt[iref],binseta,binspt,refPtVsJetEtaRefPt,
                         noabsflavors);
              fill_histo(refpdgid[iref],jtpt[iref],flavorWeight,
                         eta,refpt[iref],binseta,binspt,jetPtVsJetEtaRefPt,
                         noabsflavors);
              fill_histo(refpdgid[iref],refpt[iref],flavorWeight,
                         y,refpt[iref],binsy,binspt,refPtVsJetYRefPt,
                         noabsflavors);
              fill_histo(refpdgid[iref],jtpt[iref],flavorWeight,
                         y,refpt[iref],binsy,binspt,jetPtVsJetYRefPt,
                         noabsflavors);
            }
          }
	
          if (dorelrsp) {
            if (dojetpt) {
              fill_histo(relrsp,weight,jtpt[iref],binspt,relRspVsJetPt);
              if (doflavor) fill_histo(refpdgid[iref],relrsp,flavorWeight,
                                       jtpt[iref], binspt,relRspVsJetPt,
                                       noabsflavors);
            }
            if (dorefpt) {
              fill_histo(relrsp,weight,refpt[iref],binspt,relRspVsRefPt);
              if (doflavor) fill_histo(refpdgid[iref],relrsp,flavorWeight,
                                       refpt[iref],binspt,relRspVsRefPt,
                                       noabsflavors);
            }

            fill_histo(relrsp,weight,eta,binseta,relRspVsJetEta);
            if (doflavor) fill_histo(refpdgid[iref],relrsp,flavorWeight,
                                     eta,binseta,relRspVsJetEta,
                                     noabsflavors);
	  
            fill_histo(relrsp,weight,jtphi[iref],binsphi,relRspVsJetPhi);
            if (doflavor) fill_histo(refpdgid[iref],relrsp,flavorWeight,
                                     jtphi[iref],binsphi,relRspVsJetPhi,
                                     noabsflavors);
	  
            fill_histo(relrsp,weight,jty[iref],binsy,relRspVsJetY);
            if (doflavor) fill_histo(refpdgid[iref],relrsp,flavorWeight,
                                     jty[iref],binsy,relRspVsJetY,
                                     noabsflavors);
	  
            if (dojetpt) {
              fill_histo(relrsp,weight,eta,jtpt[iref],
                         binseta,binspt,relRspVsJetEtaJetPt);
              fill_histo(relrsp,weight,y,jtpt[iref],
                         binsy,binspt,relRspVsJetYJetPt);
              if (doflavor) {
                fill_histo(refpdgid[iref],relrsp,flavorWeight,eta,jtpt[iref],
                           binseta,binspt,relRspVsJetEtaJetPt,noabsflavors);
                fill_histo(refpdgid[iref],relrsp,flavorWeight,y,jtpt[iref],
                           binsy,binspt,relRspVsJetYJetPt,noabsflavors);
              }
            }
            if (dorefpt) {
              fill_histo(relrsp,weight,eta,refpt[iref],
                         binseta,binspt,relRspVsJetEtaRefPt);
              fill_histo(relrsp,weight,y,refpt[iref],
                         binsy,binspt,relRspVsJetYRefPt);
              if (doflavor) {
                fill_histo(refpdgid[iref],relrsp,flavorWeight,eta,refpt[iref],
                           binseta,binspt,relRspVsJetEtaRefPt,noabsflavors);
                fill_histo(refpdgid[iref],relrsp,flavorWeight,y,refpt[iref],
                           binsy,binspt,relRspVsJetYRefPt,noabsflavors);
              }
            }
          }
	
          if (doabsrsp) {
            if (dojetpt) {
              fill_histo(absrsp,weight,jtpt[iref], binspt,absRspVsJetPt);
              if (doflavor) fill_histo(refpdgid[iref],absrsp,flavorWeight,
                                       jtpt[iref],binspt,absRspVsJetPt,
                                       noabsflavors);
            }
            if (dorefpt) {
              fill_histo(absrsp,weight,refpt[iref],binspt,absRspVsRefPt);
              if (doflavor) fill_histo(refpdgid[iref],absrsp,flavorWeight,
                                       refpt[iref],binspt,absRspVsRefPt,
                                       noabsflavors);
            }
	  
            fill_histo(absrsp,weight,eta,binseta,absRspVsJetEta);
            if (doflavor) fill_histo(refpdgid[iref],absrsp,flavorWeight,
                                     eta,binseta,absRspVsJetEta,
                                     noabsflavors);

            fill_histo(absrsp,weight,jtphi[iref],binsphi,absRspVsJetPhi);
            if (doflavor) fill_histo(refpdgid[iref],absrsp,flavorWeight,
                                     jtphi[iref],binsphi,absRspVsJetPhi,
                                     noabsflavors);
	  
            fill_histo(absrsp,weight,jty[iref],binsy,absRspVsJetY);
            if (doflavor) fill_histo(refpdgid[iref],absrsp,flavorWeight,
                                     jty[iref],binsy,absRspVsJetY,
                                     noabsflavors);
	  
            if (dojetpt) {
              fill_histo(absrsp,weight,eta,jtpt[iref],
                         binseta,binspt,absRspVsJetEtaJetPt);
              fill_histo(absrsp,weight,y,jtpt[iref],
                         binsy,binspt,absRspVsJetYJetPt);
              if (doflavor) {
                fill_histo(refpdgid[iref],absrsp,flavorWeight,eta,jtpt[iref],
                           binseta,binspt,absRspVsJetEtaJetPt,noabsflavors);
                fill_histo(refpdgid[iref],absrsp,flavorWeight,y,jtpt[iref],
                           binsy,binspt,absRspVsJetYJetPt,noabsflavors);
              }
            }
            if (dorefpt) {
              fill_histo(absrsp,weight,eta,refpt[iref],
                         binseta,binspt,absRspVsJetEtaRefPt);
              fill_histo(absrsp,weight,y,refpt[iref],
                         binsy,binspt,absRspVsJetYRefPt);
              if (doflavor) {
                fill_histo(refpdgid[iref],absrsp,flavorWeight,eta,refpt[iref],
                           binseta,binspt,absRspVsJetEtaRefPt,noabsflavors);
                fill_histo(refpdgid[iref],absrsp,flavorWeight,y,refpt[iref],
                           binsy,binspt,absRspVsJetYRefPt,noabsflavors);
              }
            }
          }
	
          if (doetarsp) {
            if (dojetpt) {
              fill_histo(etarsp,weight,jtpt[iref], binspt,etaRspVsJetPt);
              if (doflavor) fill_histo(refpdgid[iref],etarsp,flavorWeight,
                                       jtpt[iref],binspt,etaRspVsJetPt,
                                       noabsflavors);
            }
            if (dorefpt) {
              fill_histo(etarsp,weight,refpt[iref],binspt,etaRspVsRefPt);
              if (doflavor) fill_histo(refpdgid[iref],etarsp,flavorWeight,
                                       refpt[iref],binspt,etaRspVsRefPt,
                                       noabsflavors);
            }
	  
            fill_histo(etarsp,weight,eta,binseta,etaRspVsJetEta);
            if (doflavor) fill_histo(refpdgid[iref],etarsp,flavorWeight,
                                     eta,binseta,etaRspVsJetEta,
                                     noabsflavors);
	  
            fill_histo(etarsp,weight,jtphi[iref],binsphi,etaRspVsJetPhi);
            if (doflavor) fill_histo(refpdgid[iref],etarsp,flavorWeight,
                                     jtphi[iref],binsphi,etaRspVsJetPhi,
                                     noabsflavors);
	  
            if (dojetpt) {
              fill_histo(etarsp,weight,eta,jtpt[iref],
                         binseta,binspt,etaRspVsJetEtaJetPt);
              if (doflavor) fill_histo(refpdgid[iref],etarsp,flavorWeight,
                                       eta,jtpt[iref],
                                       binseta,binspt,etaRspVsJetEtaJetPt,
                                       noabsflavors);
            }
            if (dorefpt) {
              fill_histo(etarsp,weight,eta,refpt[iref],
                         binseta,binspt,etaRspVsJetEtaRefPt);
              if (doflavor) fill_histo(refpdgid[iref],etarsp,flavorWeight,
                                       eta,refpt[iref],
                                       binseta,binspt,etaRspVsJetEtaRefPt,
                                       noabsflavors);
            }
          }
	
          if (dophirsp) {
            if (dojetpt) {
              fill_histo(phirsp,weight,jtpt[iref], binspt,phiRspVsJetPt);
              if (doflavor) fill_histo(refpdgid[iref],phirsp,flavorWeight,
                                       jtpt[iref],binspt,phiRspVsJetPt,
                                       noabsflavors);
            }
            if (dorefpt) {
              fill_histo(phirsp,weight,refpt[iref],binspt,phiRspVsRefPt);
              if (doflavor) fill_histo(refpdgid[iref],phirsp,flavorWeight,
                                       refpt[iref],binspt,phiRspVsRefPt,
                                       noabsflavors);
            }
	  
            fill_histo(phirsp,weight,eta,binseta,phiRspVsJetEta);
            if (doflavor) fill_histo(refpdgid[iref],phirsp,flavorWeight,
                                     eta,binseta,phiRspVsJetEta,
                                     noabsflavors);
	  
            fill_histo(phirsp,weight,jtphi[iref],binsphi,phiRspVsJetPhi);
            if (doflavor) fill_histo(refpdgid[iref],phirsp,flavorWeight,
                                     jtphi[iref],binsphi,phiRspVsJetPhi,
                                     noabsflavors);
	  
            if (dojetpt) {
              fill_histo(phirsp,weight,eta,jtpt[iref],
                         binseta,binspt,phiRspVsJetEtaJetPt);
              if (doflavor) fill_histo(refpdgid[iref],phirsp,flavorWeight,
                                       eta,jtpt[iref],
                                       binseta,binspt,phiRspVsJetEtaJetPt,
                                       noabsflavors);
            }
            if (dorefpt) {
              fill_histo(phirsp,weight,eta,refpt[iref],
                         binseta,binspt,phiRspVsJetEtaRefPt);
              if (doflavor) fill_histo(refpdgid[iref],phirsp,flavorWeight,
                                       eta,refpt[iref],
                                       binseta,binspt,phiRspVsJetEtaRefPt,
                                       noabsflavors);
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
vector<string> get_flavors(bool noabsflavors)
{
   vector<string> flavor;
   flavor.push_back("");
   if (!noabsflavors) {
      flavor.push_back("uds_");
      flavor.push_back("c_");
      flavor.push_back("b_");
      flavor.push_back("g_");
      flavor.push_back("slc_");
      flavor.push_back("slb_");
   }
   else {
      flavor.push_back("uds_");
      flavor.push_back("udsbar_");
      flavor.push_back("c_");
      flavor.push_back("cbar_");
      flavor.push_back("b_");
      flavor.push_back("bbar_");
      flavor.push_back("g_");
      flavor.push_back("slc_");
      flavor.push_back("slb_");
      flavor.push_back("udscbg_");
   }
   return flavor;
}


//______________________________________________________________________________
TString pdgid_to_flavor_name(int pdgid)
{
   if(pdgid<=3 && pdgid>=1)
      return "uds_";
   else if(pdgid>=-3 && pdgid<=-1)
      return "udsbar_";
   else if(pdgid==4)
      return "c_";
   else if(pdgid==-4)
      return "cbar_";
   else if(pdgid==5)
      return "b_";
   else if(pdgid==-5)
      return "bbar_";
   else if(fabs(pdgid)==21)
      return "g_";
   else if(fabs(pdgid)==411 || fabs(pdgid)==413)
      return "slc_";
   else if(fabs(pdgid)==511 || fabs(pdgid)==513)
      return "slb_";
   else
      return "all_";
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
void fill_histo(int pdgid,float value,float weight,float x,const vector<float>& binsx,
                const vector<TH1F**>& histos, bool noabsflavors)
{
  if (binsx.size()==0) return;
  int abspdgid=fabs(pdgid);
  int iflv(-1);
  if(noabsflavors)
    {
      if (pdgid>=1&&pdgid<=3)        iflv=1;
      else if (pdgid<=-1&&pdgid>=-3) iflv=2;
      else if (pdgid==4)             iflv=3;
      else if (pdgid==-4)            iflv=4;
      else if (pdgid==5)             iflv=5;
      else if (pdgid==-5)            iflv=6;
      else if (abspdgid==21)         iflv=7;
      else if (abspdgid==411||
               abspdgid==413)        iflv=8;
      else if (abspdgid==511||
               abspdgid==513)        iflv=9;
      else return;
      
      int ix=get_index(x,binsx);
      if (ix>=0)
        {
          histos[ix][iflv]->Fill(value,weight);
          if((abspdgid>=1&&abspdgid<=5)||abspdgid==21)histos[ix][10]->Fill(value,weight);
        }
    }
  else
    {
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
                const vector<TH1F***>& histos, bool noabsflavors)
{
  if (binsx.size()==0||binsy.size()==0) return;
  int abspdgid=fabs(pdgid);
  int iflv(-1);
  if (noabsflavors)
    {
      if (pdgid>=1&&pdgid<=3)        iflv=1;
      else if (pdgid<=-1&&pdgid>=-3) iflv=2;
      else if (pdgid==4)             iflv=3;
      else if (pdgid==-4)            iflv=4;
      else if (pdgid==5)             iflv=5;
      else if (pdgid==-5)            iflv=6;
      else if (abspdgid==21)         iflv=7;
      else if (abspdgid==411||
               abspdgid==413)        iflv=8;
      else if (abspdgid==511||
               abspdgid==513)        iflv=9;
      else return;
      
      int ix=get_index(x,binsx);
      int iy=get_index(y,binsy);
      if (ix>=0&&iy>=0) 
        {
          histos[ix][iy][iflv]->Fill(value,weight);
          if((abspdgid>=1&&abspdgid<=5)||abspdgid==21)histos[ix][iy][10]->Fill(value,weight);
        }
    }
  else
    {
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
bool it_pileup(int itlow, int ithigh, vector<int>* npus)
{
  if((*npus)[1]>=itlow && (*npus)[1]<=ithigh) return true;
  return false;
}


//______________________________________________________________________________
bool oot_pileup(int earlyootlow, int earlyoothigh, int lateootlow, int lateoothigh,
                vector<int>* npus)
{
  if((*npus)[0]>=earlyootlow && (*npus)[0]<=earlyoothigh && 
     (*npus)[2]>=lateootlow && (*npus)[2]<=lateoothigh) return true;
  return false;
}


//______________________________________________________________________________
bool total_oot_pileup(int totalootlow, int totaloothigh, vector<int>* npus)
{
  if((*npus)[0]+(*npus)[2]>=totalootlow && (*npus)[0]+(*npus)[2]<=totaloothigh) return true;
  return false;
}


//______________________________________________________________________________
bool pileup_cut(int itlow, int ithigh, int earlyootlow, int earlyoothigh, 
                int lateootlow, int lateoothigh, int totalootlow, int totaloothigh, 
                vector<int>* npus)
{
  if(it_pileup(itlow,ithigh,npus) && 
     total_oot_pileup(totalootlow,totaloothigh,npus) && 
     oot_pileup(earlyootlow,earlyoothigh,lateootlow,lateoothigh,npus)) return true;
  return false;
}

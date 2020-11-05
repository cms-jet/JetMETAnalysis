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
#include "JetMETAnalysis/JetUtilities/interface/JRAEvent.h"
#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"

#include <TROOT.h>
#include <TSystem.h>
#include <TEnv.h>
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
#include <vector>

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

/// fill the appropriate histogram (histos), based on x, y, z, binsx, binsy, and binsz
void fill_histo(float value,float weight,float x,float y, float z,
                const vector<float>& binsx,const vector<float>& binsy,const vector<float>& binsz,
                const vector<TH1F****>& histos);

/// fill the appropriate histogram (histos), based on pdgid, x, y, z, binsx, binsy, and binsz
void fill_histo(int pdgid,float value,float weight,float x,float y,float z,
                const vector<float>& binsx,const vector<float>& binsy,const vector<float>& binsz,
                const vector<TH1F****>& histos, bool noabsflavors);

/// check if a vector of strings contains a certain element
bool contains(const vector<string>& collection,const string& element);

/// check the amount of IT pileup and see if it is in the specified range
bool it_pileup(int itlow, int ithigh, vector<int>* npus, int iIT);

/// check the amount of OOT pileup before nad after the event and see if it is in the specified range
bool oot_pileup(int earlyootlow, int earlyoothigh, int lateootlow, int lateoothigh,
                vector<int>* npus, int iIT);

/// check the sum of the OOT pileup before and after the event and see if it is in the specified range
bool total_oot_pileup(int totalootlow, int totaloothigh, vector<int>* npus, int iIT);

/// combines the booleans from the IT, OOT, and TotalOOT functions into one boolean
bool pileup_cut(int itlow, int ithigh, int earlyootlow, int earlyoothigh, 
                int lateootlow, int lateoothigh, int totalootlow, int totaloothigh, 
                vector<int>* npus, vector<int>* bxns);

/// returns the index in bxns, npus, and tnpus that corresponds to the IT PU
int itIndex(vector<int>* bxns);

/// returns the number of PUs before the index iIT (i.e. the current BX index)
double sumEOOT(vector<int>* npus, unsigned int iIT);

/// returns the number of PUs after the index iIT (i.e. the current BX index)
double sumLOOT(vector<int>* npus, unsigned int iIT);

//______________________________________________________________________________
int main(int argc,char**argv)
{
  //
  // evaluate command-line / configuration file options
  //
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;

  string         input             = cl.getValue<string> ("input");
  vector<float>  binspt            = cl.getVector<float> ("binspt",                     "");
  vector<float>  binseta           = cl.getVector<float> ("binseta",                    "");
  vector<float>  binsphi           = cl.getVector<float> ("binsphi",                    "");
  vector<float>  binsy             = cl.getVector<float> ("binsy",                      "");
  vector<float>  binsmu            = cl.getVector<float> ("binsmu",                     "");
  vector<float>  binsrho           = cl.getVector<float> ("binsrho",                    "");
  vector<float>  binspudensity     = cl.getVector<float> ("binspudensity",              "");
  string         treename          = cl.getValue<string> ("treename",                  "t");
  string         output            = cl.getValue<string> ("output",             "jra.root");
  bool           useweight         = cl.getValue<bool>   ("useweight",               false);
  float          xsection          = cl.getValue<float>  ("xsection",                  0.0);
  float          luminosity        = cl.getValue<float>  ("luminosity",                1.0);
  int            nrefmax           = cl.getValue<int>    ("nrefmax",                     0);
  int            nbinspt           = cl.getValue<int>    ("nbinspt",                    50);
  int            nbinseta          = cl.getValue<int>    ("nbinseta",                   25);
  int            nbinsphi          = cl.getValue<int>    ("nbinsphi",                   25);
  int            nbinsy            = cl.getValue<int>    ("nbinsy",                     25);
  float          etabarrelmin      = cl.getValue<float>  ("etabarrelmin",             -1.3);
  float          etabarrelmax      = cl.getValue<float>  ("etabarrelmax",             +1.3);
  float          etaiendcapmin     = cl.getValue<float>  ("etaiendcapmin",            -2.5);
  float          etaiendcapmax     = cl.getValue<float>  ("etaiendcapmax",            +2.5);
  float          etaoendcapmin     = cl.getValue<float>  ("etaoendcapmin",            -3.0);
  float          etaoendcapmax     = cl.getValue<float>  ("etaoendcapmax",            +3.0);
  float          etaforwardmin     = cl.getValue<float>  ("etaforwardmin",            -5.0);
  float          etaforwardmax     = cl.getValue<float>  ("etaforwardmax",            +5.0);
  bool           dobalance         = cl.getValue<bool>   ("dobalance",               false);
  bool           doflavor          = cl.getValue<bool>   ("doflavor",                false);
  vector<string> flavors           = cl.getVector<string>("flavors",                    "");
  TString        flavorDefinition  = cl.getValue<TString>("flavorDefinition",       "phys");
  bool           noabsflavors      = cl.getValue<bool>   ("noabsflavors",            false);
  float          drmax             = cl.getValue<float>  ("drmax",                     0.3);
  float          dphimin           = cl.getValue<float>  ("dphimin",                   2.7);
  bool           dojetpt           = cl.getValue<bool>   ("dojetpt",                 false);
  bool           dorefpt           = cl.getValue<bool>   ("dorefpt",                  true);
  bool           domu              = cl.getValue<bool>   ("domu",                    false);
  bool           dorho             = cl.getValue<bool>   ("dorho",                   false);
  bool           dopudensity       = cl.getValue<bool>   ("dopudensity",             false);
  bool           NpuNotMu          = cl.getValue<bool>   ("NpuNotMu",                false);
  bool           GRhoPU            = cl.getValue<bool>   ("GRhoPU",                  false);
  int            nbinsrelrsp       = cl.getValue<int>    ("nbinsrelrsp",                50);
  float          relrspmin         = cl.getValue<float>  ("relrspmin",                 0.0);
  float          relrspmax         = cl.getValue<float>  ("relrspmax",                 2.0);
  int            nbinsabsrsp       = cl.getValue<int>    ("nbinsabsrsp",                 0);
  float          absrspmin         = cl.getValue<float>  ("absrspmin",              -250.0);
  float          absrspmax         = cl.getValue<float>  ("absrspmax",               100.0);
  int            nbinsetarsp       = cl.getValue<int>    ("nbinsetarsp",               100);
  float          etarspmin         = cl.getValue<float>  ("etarspmin",                -1.0);
  float          etarspmax         = cl.getValue<float>  ("etarspmax",                 1.0);
  int            nbinsphirsp       = cl.getValue<int>    ("nbinsphirsp",               100);
  float          phirspmin         = cl.getValue<float>  ("phirspmin",                -1.0);
  float          phirspmax         = cl.getValue<float>  ("phirspmax",                 1.0);
  float          jtptmin           = cl.getValue<float>  ("jtptmin",                   1.0);
  float          pthatmin          = cl.getValue<float>  ("pthatmin",                  0.0);
  float          pthatmax          = cl.getValue<float>  ("pthatmax",                 -1.0);
  vector<string> algs              = cl.getVector<string>("algs",                       "");
  vector<string> presel            = cl.getVector<string>("presel",                     "");
  int            itlow             = cl.getValue<int>    ("itlow",                       0);
  int            ithigh            = cl.getValue<int>    ("ithigh",                 100000);
  int            earlyootlow       = cl.getValue<int>    ("earlyootlow",                 0);
  int            earlyoothigh      = cl.getValue<int>    ("earlyoothigh",           100000);
  int            lateootlow        = cl.getValue<int>    ("lateootlow",                  0);
  int            lateoothigh       = cl.getValue<int>    ("lateoothigh",            100000);
  int            totalootlow       = cl.getValue<int>    ("totalootlow",                 0);
  int            totaloothigh      = cl.getValue<int>    ("totaloothigh",           100000);
  TString        weightfile        = cl.getValue<TString>("weightfile",                 "");
  TString        MCPUReWeighting   = cl.getValue<TString>("MCPUReWeighting",            "");
  TString        MCPUHistoName     = cl.getValue<TString>("MCPUHistoName",        "h_pileup");
  TString        DataPUReWeighting = cl.getValue<TString>("DataPUReWeighting",          "");
  TString        DataPUHistoName   = cl.getValue<TString>("DataPUHistoName","pileup_jt400");
  bool           verbose           = cl.getValue<bool>   ("verbose",                 false);

  if (!cl.check()) return 0;
  cl.print();

  gEnv->SetValue("TFile.AsyncPrefetching", 1);

  bool dorelrsp=(nbinsrelrsp>0);
  if(domu)        domu        = binsmu.size()>=2;
  if(dorho)       dorho       = binsrho.size()>=2;
  if(dopudensity) dopudensity = binspudensity.size()>=2;
  bool doabsrsp=false; //(nbinsabsrsp>0);
  bool doetarsp=false; //(nbinsetarsp>0);
  bool dophirsp=false; //(nbinsphirsp>0);

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
  if      (doflavor && flavors.size()==0)     {flavors = get_flavors(noabsflavors);}
  else if (doflavor && !contains(flavors,"")) {flavors.insert(flavors.begin(),"");}
  else if (!doflavor && flavors.size()>0)     {flavors.clear(); flavors.push_back("");}
  else                                        {flavors.push_back("");}

  //
  // open input/output files and loop over input directories/trees (=algorithms!)
  //
  TFile* ifile = TFile::Open(input.c_str(),"READ");
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
    map<TString,TH2D*> weightMap;
    if(!weightfile.IsNull())
    {
       TFile* wfile = new TFile(weightfile,"READ");
       if (!wfile->IsOpen()) { cout<<"Can't open "<<weightfile<<endl; }
       gDirectory->cd(alg.c_str());
       if(!doflavor)
       {
          cout << "Getting the weight histogram all_ ... " << flush; 
          weightMap["all_"] = (TH2D*)gDirectory->Get("all_");
          if(weightMap["all_"]==0) { cout<<"FAIL!"<<endl<<"Histogram of weights named \"all_\" was not in file "<<weightfile<<endl; return 0; } 
          cout << "DONE" << endl;
       }
       else
       {
          for(unsigned int f=0; f<flavors.size(); f++)
          {
             cout << "Getting the weight histogram " << flavors[f] << " ... " << flush; 
             if(flavors[f]!="") {
                weightMap[flavors[f]] = (TH2D*)gDirectory->Get(flavors[f].c_str());
              }
             else
                weightMap[flavors[f]] = (TH2D*)gDirectory->Get("all_");
             if(weightMap[flavors[f]]==0) { cout<<"FAIL!"<<endl<<"Histogram of weights named \""<< flavors[f]<< "\" was not in file "<<weightfile<<endl; return 0; } 
             cout << "DONE" << endl;
          }
       }
    }
    edm::LumiReWeighting LumiWeights_;
    if(!MCPUReWeighting.IsNull() && !DataPUReWeighting.IsNull()) { 
       LumiWeights_ = edm::LumiReWeighting(string(MCPUReWeighting),string(DataPUReWeighting),string(MCPUHistoName),string(DataPUHistoName));
    }

    //
    // setup the tree for reading
    //
    float weight(1.0);
    float flavorWeight(1.0);
    JRAEvent* JRAEvt = new JRAEvent(tree,85);
    tree->SetBranchStatus("*",0);
    vector<string> branch_names = {"nref","weight","rho","refpdgid","refpt",
                                   "refeta","refphi","jtpt","jteta","jtphi",
                                   "jty","refdxjt","bxns","npus","tnpus",
                                   "pthat","pudensity","gpudensity"};
    for(auto n : branch_names) {
        if(n=="refpdgid") {
            if(!doflavor) continue;
            else if(doflavor) {
                flavorDefinition.ToUpper();
                if(flavorDefinition.CompareTo("ALGO")==0)
                    n = "refpdgid_algorithmicDef";
                else if(flavorDefinition.CompareTo("PHYS")==0)
                    n = "refpdgid_physicsDef";
            }
        }
        if(n=="weight") {
            if (xsection>0.0) { 
                useweight = false;
            }
            if (useweight) {
                if (0==tree->GetBranch(n.c_str()))
                    cout<<"branch 'weight' not found, events will NOT be weighted!"<<endl;
                else
                    tree->SetBranchStatus(n.c_str(),1);
            }
            continue;
        }
        if(n=="refdxjt") {
            if(dobalance){
                if (0==tree->GetBranch("refdphijt")) {
                    cout<<"dobalance, but no branch 'refdphijt' in tree, skip!"<<endl;
                    continue;
                }
                else {
                    n="refdphijt";
                }
            }
            else {
                if (0==tree->GetBranch("refdrjt")) {
                    cout<<"!dobalance, but no branch 'refdrjt' in tree, skip!"<<endl;
                    continue;
                }
                else {
                    n="refdrjt";
                }
            }
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
    vector<TH1F**>   jetPtVsJetPt;
    vector<TH1F**>   refPtVsRefPt;
    vector<TH1F**>   jetPtVsRefPt;
    vector<TH1F**>   refPtVsRefPtBarrel;
    vector<TH1F**>   jetPtVsRefPtBarrel;
    vector<TH1F**>   refPtVsRefPtInnerEndcap;
    vector<TH1F**>   jetPtVsRefPtInnerEndcap;
    vector<TH1F**>   refPtVsRefPtOuterEndcap;
    vector<TH1F**>   jetPtVsRefPtOuterEndcap;
    vector<TH1F**>   refPtVsRefPtForward;
    vector<TH1F**>   jetPtVsRefPtForward;
    vector<TH1F**>   jetEtaVsJetEta;
    vector<TH1F**>   jetPhiVsJetPhi;
    vector<TH1F**>   jetYVsJetY;
    vector<TH1F***>  jetPtVsJetEtaJetPt;
    vector<TH1F****> jetPtVsJetEtaMuJetPt;
    vector<TH1F****> jetPtVsJetEtaRhoJetPt;
    vector<TH1F****> jetPtVsJetEtaRhoPUJetPt;
    vector<TH1F***>  refPtVsJetEtaRefPt;
    vector<TH1F****> refPtVsJetEtaMuRefPt;
    vector<TH1F****> refPtVsJetEtaRhoRefPt;
    vector<TH1F****> refPtVsJetEtaRhoPURefPt;
    vector<TH1F***>  jetPtVsJetEtaRefPt;
    vector<TH1F****> jetPtVsJetEtaMuRefPt;
    vector<TH1F****> jetPtVsJetEtaRhoRefPt;
    vector<TH1F****> jetPtVsJetEtaRhoPURefPt;
    vector<TH1F***>  jetPtVsJetYJetPt;
    vector<TH1F***>  refPtVsJetYRefPt;
    vector<TH1F***>  jetPtVsJetYRefPt;

    vector<TH1F**>   pThatVsRefPt;
    vector<TH1F***>  pThatVsJetEtaRefPt;
    
    vector<TH1F**>   relRspVsJetPt;
    vector<TH1F**>   relRspVsRefPt;
    vector<TH1F**>   relRspVsRefPtBarrel;
    vector<TH1F**>   relRspVsRefPtInnerEndcap;
    vector<TH1F**>   relRspVsRefPtOuterEndcap;
    vector<TH1F**>   relRspVsRefPtForward;
    vector<TH1F**>   relRspVsJetEta;
    vector<TH1F**>   relRspVsJetPhi;
    vector<TH1F**>   relRspVsJetY;
    vector<TH1F***>  relRspVsJetEtaJetPt;
    vector<TH1F****> relRspVsJetEtaMuJetPt;
    vector<TH1F****> relRspVsJetEtaRhoJetPt;
    vector<TH1F****> relRspVsJetEtaRhoPUJetPt;
    vector<TH1F***>  relRspVsJetEtaRefPt;
    vector<TH1F****> relRspVsJetEtaMuRefPt;
    vector<TH1F****> relRspVsJetEtaRhoRefPt;
    vector<TH1F****> relRspVsJetEtaRhoPURefPt;
    vector<TH1F***>  relRspVsJetYJetPt;
    vector<TH1F***>  relRspVsJetYRefPt;
    
    vector<TH1F**>   absRspVsJetPt;
    vector<TH1F**>   absRspVsRefPt;
    vector<TH1F**>   absRspVsRefPtBarrel;
    vector<TH1F**>   absRspVsJetEta;
    vector<TH1F**>   absRspVsJetPhi;
    vector<TH1F**>   absRspVsJetY;
    vector<TH1F***>  absRspVsJetEtaJetPt;
    vector<TH1F***>  absRspVsJetEtaRefPt;
    vector<TH1F***>  absRspVsJetYJetPt;
    vector<TH1F***>  absRspVsJetYRefPt;

    vector<TH1F**>   etaRspVsJetPt;
    vector<TH1F**>   etaRspVsRefPt;
    vector<TH1F**>   etaRspVsJetEta;
    vector<TH1F**>   etaRspVsJetPhi;
    vector<TH1F***>  etaRspVsJetEtaJetPt;
    vector<TH1F***>  etaRspVsJetEtaRefPt;

    vector<TH1F**>   phiRspVsJetPt;
    vector<TH1F**>   phiRspVsRefPt;
    vector<TH1F**>   phiRspVsJetEta;
    vector<TH1F**>   phiRspVsJetPhi;
    vector<TH1F***>  phiRspVsJetEtaJetPt;
    vector<TH1F***>  phiRspVsJetEtaRefPt;

    // book pT histograms
    if (binspt.size()>=2) {
      for (unsigned int ipt=0;ipt<binspt.size()-1;++ipt) {
	
        string hname; float ptmin=binspt[ipt]; float ptmax=binspt[ipt+1];
	
        if (dojetpt) {
          jetPtVsJetPt.push_back(new TH1F*[flavors.size()]);
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
            hname=flavors[iflv]+"JetPt_"+get_suffix("JetPt",ipt,binspt);
            jetPtVsJetPt.back()[iflv]=new TH1F(hname.c_str(),";p_{T} [GeV]",
                                               nbinspt,ptmin,ptmax);
          }
        }
	
        if (dorefpt) {
          refPtVsRefPt.push_back(new TH1F*[flavors.size()]);
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
            hname=flavors[iflv]+"RefPt_"+get_suffix("RefPt",ipt,binspt);
            refPtVsRefPt.back()[iflv]=new TH1F(hname.c_str(),";p_{T}^{ref} [GeV]",
                                               nbinspt,ptmin,ptmax);
          }
        }
	
        if (dorefpt) {
          jetPtVsRefPt.push_back(new TH1F*[flavors.size()]);
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
            hname=flavors[iflv]+"JetPt_"+get_suffix("RefPt",ipt,binspt);
            jetPtVsRefPt.back()[iflv]=new TH1F(hname.c_str(),";p_{T} [GeV]",
                                               3*nbinspt,
                                               0,
                                               3.0*ptmax);
          }
        }

        if (dorefpt) {
          pThatVsRefPt.push_back(new TH1F*[flavors.size()]);
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
            hname=flavors[iflv]+"pThat_"+get_suffix("RefPt",ipt,binspt);
            pThatVsRefPt.back()[iflv]=new TH1F(hname.c_str(),";#hat{p}_{T} [GeV]",
                                               (int)binspt[binspt.size()-1]/10.0,
                                               0,
                                               binspt[binspt.size()-1]);
          }
        }
	
        if (dorefpt) {
          refPtVsRefPtBarrel.push_back(new TH1F*[flavors.size()]);
          refPtVsRefPtInnerEndcap.push_back(new TH1F*[flavors.size()]);
          refPtVsRefPtOuterEndcap.push_back(new TH1F*[flavors.size()]);
          refPtVsRefPtForward.push_back(new TH1F*[flavors.size()]);
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
            hname=flavors[iflv]+"RefPt_Barrel_"+get_suffix("RefPt",ipt,binspt);
            refPtVsRefPtBarrel.back()[iflv]=new TH1F(hname.c_str(),
                                                     ";p_{T}^{ref} [GeV]",
                                                     nbinspt,ptmin,ptmax);
            hname=flavors[iflv]+"RefPt_InnerEndcap_"+get_suffix("RefPt",ipt,binspt);
            refPtVsRefPtInnerEndcap.back()[iflv]=new TH1F(hname.c_str(),
                                                     ";p_{T}^{ref} [GeV]",
                                                     nbinspt,ptmin,ptmax);
            hname=flavors[iflv]+"RefPt_OuterEndcap_"+get_suffix("RefPt",ipt,binspt);
            refPtVsRefPtOuterEndcap.back()[iflv]=new TH1F(hname.c_str(),
                                                     ";p_{T}^{ref} [GeV]",
                                                     nbinspt,ptmin,ptmax);
            hname=flavors[iflv]+"RefPt_Forward_"+get_suffix("RefPt",ipt,binspt);
            refPtVsRefPtForward.back()[iflv]=new TH1F(hname.c_str(),
                                                      ";p_{T}^{ref} [GeV]",
                                                      nbinspt,ptmin,ptmax);
          }
        }
	
        if (dorefpt) {
          jetPtVsRefPtBarrel.push_back(new TH1F*[flavors.size()]);
          jetPtVsRefPtInnerEndcap.push_back(new TH1F*[flavors.size()]);
          jetPtVsRefPtOuterEndcap.push_back(new TH1F*[flavors.size()]);
          jetPtVsRefPtForward.push_back(new TH1F*[flavors.size()]);
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
            hname=flavors[iflv]+"JetPt_Barrel_"+get_suffix("RefPt",ipt,binspt);
            jetPtVsRefPtBarrel.back()[iflv]=new TH1F(hname.c_str(),
                                                     ";p_{T} [GeV]",
                                                     3*nbinspt,
                                                     0,
                                                     3.0*ptmax);
            hname=flavors[iflv]+"JetPt_InnerEndcap_"+get_suffix("RefPt",ipt,binspt);
            jetPtVsRefPtInnerEndcap.back()[iflv]=new TH1F(hname.c_str(),
                                                     ";p_{T} [GeV]",
                                                     3*nbinspt,
                                                     0,
                                                     3.0*ptmax);
            hname=flavors[iflv]+"JetPt_OuterEndcap_"+get_suffix("RefPt",ipt,binspt);
            jetPtVsRefPtOuterEndcap.back()[iflv]=new TH1F(hname.c_str(),
                                                     ";p_{T} [GeV]",
                                                     3*nbinspt,
                                                     0,
                                                     3.0*ptmax);
            hname=flavors[iflv]+"JetPt_Forward_"+get_suffix("RefPt",ipt,binspt);
            jetPtVsRefPtForward.back()[iflv]=new TH1F(hname.c_str(),
                                                      ";p_{T} [GeV]",
                                                      3*nbinspt,
                                                      0,
                                                      3.0*ptmax);
          }
        }
	
        if (dorelrsp&&dojetpt) {
          relRspVsJetPt.push_back(new TH1F*[flavors.size()]);
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
            hname=flavors[iflv]+"RelRsp_"+get_suffix("JetPt",ipt,binspt);
            relRspVsJetPt.back()[iflv]=new TH1F(hname.c_str(),";p_{T}/p_{T}^{ref}",
                                                nbinsrelrsp,relrspmin,relrspmax);
          }
        }

        if (dorelrsp&&dorefpt) {
          relRspVsRefPt.push_back(new TH1F*[flavors.size()]);
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
            hname=flavors[iflv]+"RelRsp_"+get_suffix("RefPt",ipt,binspt);
            relRspVsRefPt.back()[iflv]=new TH1F(hname.c_str(),";p_{T}/p_{T}^{ref}",
                                                nbinsrelrsp,relrspmin,relrspmax);
          }
        }

        if (dorelrsp&&dorefpt) {
          relRspVsRefPtBarrel.push_back(new TH1F*[flavors.size()]);
          relRspVsRefPtInnerEndcap.push_back(new TH1F*[flavors.size()]);
          relRspVsRefPtOuterEndcap.push_back(new TH1F*[flavors.size()]);
          relRspVsRefPtForward.push_back(new TH1F*[flavors.size()]);
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
            hname=flavors[iflv]+"RelRsp_Barrel_"+get_suffix("RefPt",ipt,binspt);
            relRspVsRefPtBarrel.back()[iflv]=new TH1F(hname.c_str(),
                                                      ";p_{T}/p_{T}^{ref}",
                                                      nbinsrelrsp,
                                                      relrspmin,relrspmax);
            hname=flavors[iflv]+"RelRsp_InnerEndcap_"+get_suffix("RefPt",ipt,binspt);
            relRspVsRefPtInnerEndcap.back()[iflv]=new TH1F(hname.c_str(),
                                                      ";p_{T}/p_{T}^{ref}",
                                                      nbinsrelrsp,
                                                      relrspmin,relrspmax);
            hname=flavors[iflv]+"RelRsp_OuterEndcap_"+get_suffix("RefPt",ipt,binspt);
            relRspVsRefPtOuterEndcap.back()[iflv]=new TH1F(hname.c_str(),
                                                      ";p_{T}/p_{T}^{ref}",
                                                      nbinsrelrsp,
                                                      relrspmin,relrspmax);
            hname=flavors[iflv]+"RelRsp_Forward_"+get_suffix("RefPt",ipt,binspt);
            relRspVsRefPtForward.back()[iflv]=new TH1F(hname.c_str(),
                                                       ";p_{T}/p_{T}^{ref}",
                                                       nbinsrelrsp,
                                                       relrspmin,relrspmax);
          }
        }
	
        if (doabsrsp&&dojetpt) {
          absRspVsJetPt.push_back(new TH1F*[flavors.size()]);
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
            hname=flavors[iflv]+"AbsRsp_"+get_suffix("JetPt",ipt,binspt);
            absRspVsJetPt.back()[iflv]=new TH1F(hname.c_str(),
                                                ";p_{T}-p_{T}^{ref} [GeV]",
                                                nbinsabsrsp,absrspmin,absrspmax);
          }
        }
	
        if (doabsrsp&&dorefpt) {
          absRspVsRefPt.push_back(new TH1F*[flavors.size()]);
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
            hname=flavors[iflv]+"AbsRsp_"+get_suffix("RefPt",ipt,binspt);
            absRspVsRefPt.back()[iflv]=new TH1F(hname.c_str(),
                                                ";p_{T}-p_{T}^{ref} [GeV]",
                                                nbinsabsrsp,absrspmin,absrspmax);
          }
        }
	
        if (doabsrsp&&dorefpt) {
          absRspVsRefPtBarrel.push_back(new TH1F*[flavors.size()]);
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
            hname=flavors[iflv]+"AbsRsp_Barrel_"+get_suffix("RefPt",ipt,binspt);
            absRspVsRefPtBarrel.back()[iflv]=new TH1F(hname.c_str(),
                                                      ";p_{T}-p_{T}^{ref} [GeV]",
                                                      nbinsabsrsp,
                                                      absrspmin,absrspmax);
          }
        }

        if (doetarsp&&dojetpt) {
          etaRspVsJetPt.push_back(new TH1F*[flavors.size()]);
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
            hname=flavors[iflv]+"EtaRsp_"+get_suffix("JetPt",ipt,binspt);
            etaRspVsJetPt.back()[iflv]=new TH1F(hname.c_str(),
                                                ";|#eta|-|#eta^{ref}|",
                                                nbinsetarsp,etarspmin,etarspmax);
          }
        }
	
        if (doetarsp&&dorefpt) {
          etaRspVsRefPt.push_back(new TH1F*[flavors.size()]);
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
            hname=flavors[iflv]+"EtaRsp_"+get_suffix("RefPt",ipt,binspt);
            etaRspVsRefPt.back()[iflv]=new TH1F(hname.c_str(),
                                                ";|#eta|-|#eta^{ref}|",
                                                nbinsetarsp,etarspmin,etarspmax);
          }
        }
	
        if (dophirsp&&dojetpt) {
          phiRspVsJetPt.push_back(new TH1F*[flavors.size()]);
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
            hname=flavors[iflv]+"PhiRsp_"+get_suffix("JetPt",ipt,binspt);
            phiRspVsJetPt.back()[iflv]=new TH1F(hname.c_str(),
                                                ";#phi-#phi^{ref}",
                                                nbinsphirsp,phirspmin,phirspmax);
          }
        }
	
        if (dophirsp&&dorefpt) {
          phiRspVsRefPt.push_back(new TH1F*[flavors.size()]);
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
            hname=flavors[iflv]+"PhiRsp_"+get_suffix("RefPt",ipt,binspt);
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
          jetEtaVsJetEta.push_back(new TH1F*[flavors.size()]);
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
            hname=flavors[iflv]+"JetEta_"+get_suffix("JetEta",ieta,binseta);
            jetEtaVsJetEta.back()[iflv]=new TH1F(hname.c_str(),";#eta",
                                                 nbinseta,etamin,etamax);
          }
        }
	
        if (dorelrsp) {
          relRspVsJetEta.push_back(new TH1F*[flavors.size()]);
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
            hname=flavors[iflv]+"RelRsp_"+get_suffix("JetEta",ieta,binseta);
            relRspVsJetEta.back()[iflv]=new TH1F(hname.c_str(),";p_{T}/p_{T}^{ref}",
                                                 nbinsrelrsp,relrspmin,relrspmax);
          }
        }
	
        if (doabsrsp) {
          absRspVsJetEta.push_back(new TH1F*[flavors.size()]);
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
            hname=flavors[iflv]+"AbsRsp_"+get_suffix("JetEta",ieta,binseta);
            absRspVsJetEta.back()[iflv]=new TH1F(hname.c_str(),
                                                 ";p_{T}-p_{T}^{ref} [GeV]",
                                                 nbinsabsrsp,absrspmin,absrspmax);
          }
        }
	
        if (doetarsp) {
          etaRspVsJetEta.push_back(new TH1F*[flavors.size()]);
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
            hname=flavors[iflv]+"EtaRsp_"+get_suffix("JetEta",ieta,binseta);
            etaRspVsJetEta.back()[iflv]=new TH1F(hname.c_str(),
                                                 ";|#eta|-|#eta^{ref}|",
                                                 nbinsetarsp,etarspmin,etarspmax);
          }
        }
	
        if (dophirsp) {
          phiRspVsJetEta.push_back(new TH1F*[flavors.size()]);
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
            hname=flavors[iflv]+"PhiRsp_"+get_suffix("JetEta",ieta,binseta);
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
          jetPhiVsJetPhi.push_back(new TH1F*[flavors.size()]);
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
            hname=flavors[iflv]+"JetPhi_"+get_suffix("JetPhi",iphi,binsphi);
            jetPhiVsJetPhi.back()[iflv]=new TH1F(hname.c_str(),";#phi",
                                                 nbinsphi,phimin,phimax);
          }
        }
	
        if (dorelrsp) {
          relRspVsJetPhi.push_back(new TH1F*[flavors.size()]);
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
            hname=flavors[iflv]+"RelRsp_"+get_suffix("JetPhi",iphi,binsphi);
            relRspVsJetPhi.back()[iflv]=new TH1F(hname.c_str(),";p_{T}/p_{T}^{ref}",
                                                 nbinsrelrsp,relrspmin,relrspmax);
          }
        }
	
        if (doabsrsp) {
          absRspVsJetPhi.push_back(new TH1F*[flavors.size()]);
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
            hname=flavors[iflv]+"AbsRsp_"+get_suffix("JetPhi",iphi,binsphi);
            absRspVsJetPhi.back()[iflv]=new TH1F(hname.c_str(),
                                                 ";p_{T}-p_{T}^{ref} [GeV]",
                                                 nbinsabsrsp,absrspmin,absrspmax);
          }
        }
	
        if (doetarsp) {
          etaRspVsJetPhi.push_back(new TH1F*[flavors.size()]);
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
            hname=flavors[iflv]+"EtaRsp_"+get_suffix("JetPhi",iphi,binsphi);
            etaRspVsJetPhi.back()[iflv]=new TH1F(hname.c_str(),
                                                 ";|#eta|-|#eta^{ref}|",
                                                 nbinsetarsp,etarspmin,etarspmax);
          }
        }
	
        if (dophirsp) {
          phiRspVsJetPhi.push_back(new TH1F*[flavors.size()]);
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
            hname=flavors[iflv]+"PhiRsp_"+get_suffix("JetPhi",iphi,binsphi);
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
          jetYVsJetY.push_back(new TH1F*[flavors.size()]);
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
            hname=flavors[iflv]+"JetY_"+get_suffix("JetY",iy,binsy);
            jetYVsJetY.back()[iflv]=new TH1F(hname.c_str(),";#y",nbinsy,ymin,ymax);
          }
        }
	
        if (dorelrsp) {
          relRspVsJetY.push_back(new TH1F*[flavors.size()]);
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
            hname=flavors[iflv]+"RelRsp_"+get_suffix("JetY",iy,binsy);
            relRspVsJetY.back()[iflv]=new TH1F(hname.c_str(),";p_{T}/p_{T}^{ref}",
                                               nbinsrelrsp,relrspmin,relrspmax);
          }
        }
	
        if (doabsrsp) {
          absRspVsJetY.push_back(new TH1F*[flavors.size()]);
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
            hname=flavors[iflv]+"AbsRsp_"+get_suffix("JetY",iy,binsy);
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
	
        TH1F***  jetPtJetPt(0);
        TH1F**** jetPtMuJetPt(0);
        TH1F**** jetPtRhoJetPt(0);
        TH1F**** jetPtRhoPUJetPt(0);
        TH1F***  refPtRefPt(0);
        TH1F**** refPtMuRefPt(0);
        TH1F**** refPtRhoRefPt(0);
        TH1F**** refPtRhoPURefPt(0);
        TH1F***  jetPtRefPt(0);
        TH1F**** jetPtMuRefPt(0);
        TH1F**** jetPtRhoRefPt(0);
        TH1F**** jetPtRhoPURefPt(0);
        TH1F***  pThatRefPt(0);
        TH1F***  relRspJetPt(0);
        TH1F**** relRspMuJetPt(0);
        TH1F**** relRspRhoJetPt(0);
        TH1F**** relRspRhoPUJetPt(0);
        TH1F***  relRspRefPt(0);
        TH1F**** relRspMuRefPt(0);
        TH1F**** relRspRhoRefPt(0);
        TH1F**** relRspRhoPURefPt(0);
        TH1F***  absRspJetPt(0);
        TH1F***  absRspRefPt(0);
        TH1F***  etaRspJetPt(0);
        TH1F***  etaRspRefPt(0);	
        TH1F***  phiRspJetPt(0);
        TH1F***  phiRspRefPt(0);
	
        if (dojetpt) {
          jetPtJetPt=new TH1F**[binspt.size()-1];
          for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
            jetPtJetPt[ipt]=new TH1F*[flavors.size()];

          if (domu) {
            jetPtMuJetPt = new TH1F***[binsmu.size()-1];
            for (unsigned int imu=0;imu<binsmu.size()-1;imu++){

              jetPtMuJetPt[imu] = new TH1F**[binspt.size()];
              for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
                jetPtMuJetPt[imu][ipt]=new TH1F*[flavors.size()];
            }
          }
          if (dorho) {
            jetPtRhoJetPt = new TH1F***[binsrho.size()-1];
            for (unsigned int irho=0;irho<binsrho.size()-1;irho++){

              jetPtRhoJetPt[irho] = new TH1F**[binspt.size()];
              for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
                jetPtRhoJetPt[irho][ipt]=new TH1F*[flavors.size()];
            }
          }
          if (dopudensity) {
            jetPtRhoPUJetPt = new TH1F***[binspudensity.size()-1];
            for (unsigned int irhopu=0;irhopu<binspudensity.size()-1;irhopu++){

              jetPtRhoPUJetPt[irhopu] = new TH1F**[binspt.size()];
              for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
                jetPtRhoPUJetPt[irhopu][ipt]=new TH1F*[flavors.size()];
            }
          }
        }
	
        if (dorefpt) {
          refPtRefPt =new TH1F**[binspt.size()];
          for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
            refPtRefPt[ipt]=new TH1F*[flavors.size()];

          if (domu) {
            refPtMuRefPt = new TH1F***[binsmu.size()-1];
            for (unsigned int imu=0;imu<binsmu.size()-1;imu++){

              refPtMuRefPt[imu] = new TH1F**[binspt.size()];
              for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
                refPtMuRefPt[imu][ipt]=new TH1F*[flavors.size()];
            }
          }
          if (dorho) {
            refPtRhoRefPt = new TH1F***[binsrho.size()-1];
            for (unsigned int irho=0;irho<binsrho.size()-1;irho++){

              refPtRhoRefPt[irho] = new TH1F**[binspt.size()];
              for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
                refPtRhoRefPt[irho][ipt]=new TH1F*[flavors.size()];
            }
          }
          if (dopudensity) {
            refPtRhoPURefPt = new TH1F***[binspudensity.size()-1];
            for (unsigned int irhopu=0;irhopu<binspudensity.size()-1;irhopu++){

              refPtRhoPURefPt[irhopu] = new TH1F**[binspt.size()];
              for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
                refPtRhoPURefPt[irhopu][ipt]=new TH1F*[flavors.size()];
            }
          }
        }
	
        if (dorefpt) {
          jetPtRefPt =new TH1F**[binspt.size()];
          for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
            jetPtRefPt[ipt]=new TH1F*[flavors.size()];

          if (domu) {
            jetPtMuRefPt = new TH1F***[binsmu.size()-1];
            for (unsigned int imu=0;imu<binsmu.size()-1;imu++){

              jetPtMuRefPt[imu] = new TH1F**[binspt.size()];
              for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
                jetPtMuRefPt[imu][ipt]=new TH1F*[flavors.size()];
            }
          }
          if (dorho) {
            jetPtRhoRefPt = new TH1F***[binsrho.size()-1];
            for (unsigned int irho=0;irho<binsrho.size()-1;irho++){

              jetPtRhoRefPt[irho] = new TH1F**[binspt.size()];
              for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
                jetPtRhoRefPt[irho][ipt]=new TH1F*[flavors.size()];
            }
          }
          if (dopudensity) {
            jetPtRhoPURefPt = new TH1F***[binspudensity.size()-1];
            for (unsigned int irhopu=0;irhopu<binspudensity.size()-1;irhopu++){

              jetPtRhoPURefPt[irhopu] = new TH1F**[binspt.size()];
              for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
                jetPtRhoPURefPt[irhopu][ipt]=new TH1F*[flavors.size()];
            }
          }
        }

        if (dorefpt) {
          pThatRefPt =new TH1F**[binspt.size()];
          for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
            pThatRefPt[ipt]=new TH1F*[flavors.size()];
        }
	
        if (dorelrsp&&dojetpt) {
          relRspJetPt=new TH1F**[binspt.size()-1];
          for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
            relRspJetPt[ipt]=new TH1F*[flavors.size()];

          if (domu) {
            relRspMuJetPt=new TH1F***[binsmu.size()-1];
            for (unsigned int imu=0;imu<binsmu.size()-1;imu++){

              relRspMuJetPt[imu] = new TH1F**[binspt.size()-1];
              for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
                relRspMuJetPt[imu][ipt]=new TH1F*[flavors.size()];
            }
          }
          if (dorho) {
            relRspRhoJetPt=new TH1F***[binsrho.size()-1];
            for (unsigned int irho=0;irho<binsrho.size()-1;irho++){

              relRspRhoJetPt[irho] = new TH1F**[binspt.size()-1];
              for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
                relRspRhoJetPt[irho][ipt]=new TH1F*[flavors.size()];
            }
          }
          if (dopudensity) {
            relRspRhoPUJetPt=new TH1F***[binspudensity.size()-1];
            for (unsigned int irhopu=0;irhopu<binspudensity.size()-1;irhopu++){

              relRspRhoPUJetPt[irhopu] = new TH1F**[binspt.size()-1];
              for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
                relRspRhoPUJetPt[irhopu][ipt]=new TH1F*[flavors.size()];
            }
          }
        }
	
        if (dorelrsp&&dorefpt) {
          relRspRefPt=new TH1F**[binspt.size()-1];
          for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
            relRspRefPt[ipt]=new TH1F*[flavors.size()];

          if (domu) {
            relRspMuRefPt=new TH1F***[binsmu.size()-1];
            for (unsigned int imu=0;imu<binsmu.size()-1;imu++){

              relRspMuRefPt[imu] = new TH1F**[binspt.size()-1];
              for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
                relRspMuRefPt[imu][ipt]=new TH1F*[flavors.size()];
            }
          }
          if (dorho) {
            relRspRhoRefPt=new TH1F***[binsrho.size()-1];
            for (unsigned int irho=0;irho<binsrho.size()-1;irho++){

              relRspRhoRefPt[irho] = new TH1F**[binspt.size()-1];
              for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
                relRspRhoRefPt[irho][ipt]=new TH1F*[flavors.size()];
            }
          }
          if (dopudensity) {
            relRspRhoPURefPt=new TH1F***[binspudensity.size()-1];
            for (unsigned int irhopu=0;irhopu<binspudensity.size()-1;irhopu++){

              relRspRhoPURefPt[irhopu] = new TH1F**[binspt.size()-1];
              for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
                relRspRhoPURefPt[irhopu][ipt]=new TH1F*[flavors.size()];
            }
          }
        }
	
        if (doabsrsp&&dojetpt) {
          absRspJetPt=new TH1F**[binspt.size()-1];
          for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
            absRspJetPt[ipt]=new TH1F*[flavors.size()];
        }
	
        if (doabsrsp&&dorefpt) {
          absRspRefPt=new TH1F**[binspt.size()-1];
          for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
            absRspRefPt[ipt]=new TH1F*[flavors.size()];
        }
	
        if (doetarsp&&dojetpt) {
          etaRspJetPt=new TH1F**[binspt.size()-1];
          for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
            etaRspJetPt[ipt]=new TH1F*[flavors.size()];
        }
	
        if (doetarsp&&dorefpt) {
          etaRspRefPt=new TH1F**[binspt.size()-1];
          for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
            etaRspRefPt[ipt]=new TH1F*[flavors.size()];
        }
	
        if (dophirsp&&dojetpt) {
          phiRspJetPt=new TH1F**[binspt.size()-1];
          for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
            phiRspJetPt[ipt]=new TH1F*[flavors.size()];
        }
	
        if (dophirsp&&dorefpt) {
          phiRspRefPt=new TH1F**[binspt.size()-1];
          for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
            phiRspRefPt[ipt]=new TH1F*[flavors.size()];
        }
	
        string jetEtaSuffix=get_suffix("JetEta",ieta,binseta);

//mu loop needs to be OUTSIDE PT loop
        if (domu){

          for (unsigned int imu=0;imu<binsmu.size()-1;imu++){
            string muSuffix=get_suffix("Mu",imu,binsmu);

            for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++) {

              string hname; float ptmin=binspt[ipt]; float ptmax=binspt[ipt+1];

              string jetPtSuffix=get_suffix("JetPt",ipt,binspt);
              string refPtSuffix=get_suffix("RefPt",ipt,binspt);

              for (unsigned int iflv=0;iflv<flavors.size();iflv++) {

                if (dorefpt){
                  hname=flavors[iflv]+"JetPt_"+jetEtaSuffix+"_"+muSuffix+"_"+refPtSuffix;
                  jetPtMuRefPt[imu][ipt][iflv]=new TH1F(hname.c_str(),";p_{T}",3*nbinspt, 0, 3.0*ptmax);

                  hname=flavors[iflv]+"RefPt_"+jetEtaSuffix+"_"+muSuffix+"_"+refPtSuffix;
                  refPtMuRefPt[imu][ipt][iflv]=new TH1F(hname.c_str(),";p_{T}^{ref}",nbinspt,ptmin,ptmax);

                  if (dorelrsp){
                    hname=flavors[iflv]+"RelRsp_"+jetEtaSuffix+"_"+muSuffix+"_"+refPtSuffix;
                    relRspMuRefPt[imu][ipt][iflv]=new TH1F(hname.c_str(),";p_{T}/p_{T}^{ref}",nbinsrelrsp,relrspmin,relrspmax);
                  }
                }
                if (dojetpt){
                  hname=flavors[iflv]+"JetPt_"+jetEtaSuffix+"_"+muSuffix+"_"+jetPtSuffix;
                  jetPtMuJetPt[imu][ipt][iflv]=new TH1F(hname.c_str(),";p_{T}",nbinspt, ptmin, ptmax);

                  if (dorelrsp){
                    hname=flavors[iflv]+"RelRsp_"+jetEtaSuffix+"_"+muSuffix+"_"+jetPtSuffix;
                    relRspMuJetPt[imu][ipt][iflv]=new TH1F(hname.c_str(),";p_{T}/p_{T}^{ref}",nbinsrelrsp,relrspmin,relrspmax);
                  }
                }
              }
            }
          }
        }
//rho loop needs to be OUTSIDE PT loop
        if (dorho){

          for (unsigned int irho=0;irho<binsrho.size()-1;irho++){
            string rhoSuffix=get_suffix("Rho",irho,binsrho);

            for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++) {

              string hname; float ptmin=binspt[ipt]; float ptmax=binspt[ipt+1];

              string jetPtSuffix=get_suffix("JetPt",ipt,binspt);
              string refPtSuffix=get_suffix("RefPt",ipt,binspt);

              for (unsigned int iflv=0;iflv<flavors.size();iflv++) {

                if (dorefpt){
                  hname=flavors[iflv]+"JetPt_"+jetEtaSuffix+"_"+rhoSuffix+"_"+refPtSuffix;
                  jetPtRhoRefPt[irho][ipt][iflv]=new TH1F(hname.c_str(),";p_{T}",3*nbinspt, 0, 3.0*ptmax);

                  hname=flavors[iflv]+"RefPt_"+jetEtaSuffix+"_"+rhoSuffix+"_"+refPtSuffix;
                  refPtRhoRefPt[irho][ipt][iflv]=new TH1F(hname.c_str(),";p_{T}^{ref}",nbinspt,ptmin,ptmax);

                  if (dorelrsp){
                    hname=flavors[iflv]+"RelRsp_"+jetEtaSuffix+"_"+rhoSuffix+"_"+refPtSuffix;
                    relRspRhoRefPt[irho][ipt][iflv]=new TH1F(hname.c_str(),";p_{T}/p_{T}^{ref}",nbinsrelrsp,relrspmin,relrspmax);
                  }
                }
                if (dojetpt){
                  hname=flavors[iflv]+"JetPt_"+jetEtaSuffix+"_"+rhoSuffix+"_"+jetPtSuffix;
                  jetPtRhoJetPt[irho][ipt][iflv]=new TH1F(hname.c_str(),";p_{T}",nbinspt, ptmin, ptmax);

                  if (dorelrsp){
                    hname=flavors[iflv]+"RelRsp_"+jetEtaSuffix+"_"+rhoSuffix+"_"+jetPtSuffix;
                    relRspRhoJetPt[irho][ipt][iflv]=new TH1F(hname.c_str(),";p_{T}/p_{T}^{ref}",nbinsrelrsp,relrspmin,relrspmax);
                  }
                }
              }
            }
          }
        }
        if (dopudensity){

          for (unsigned int irhopu=0;irhopu<binspudensity.size()-1;irhopu++){
            string rhoPUSuffix=get_suffix("RhoPU",irhopu,binspudensity);

            for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++) {

              string hname; float ptmin=binspt[ipt]; float ptmax=binspt[ipt+1];

              string jetPtSuffix=get_suffix("JetPt",ipt,binspt);
              string refPtSuffix=get_suffix("RefPt",ipt,binspt);

              for (unsigned int iflv=0;iflv<flavors.size();iflv++) {

                if (dorefpt){
                  hname=flavors[iflv]+"JetPt_"+jetEtaSuffix+"_"+rhoPUSuffix+"_"+refPtSuffix;
                  jetPtRhoPURefPt[irhopu][ipt][iflv]=new TH1F(hname.c_str(),";p_{T}",3*nbinspt, 0, 3.0*ptmax);

                  hname=flavors[iflv]+"RefPt_"+jetEtaSuffix+"_"+rhoPUSuffix+"_"+refPtSuffix;
                  refPtRhoPURefPt[irhopu][ipt][iflv]=new TH1F(hname.c_str(),";p_{T}^{ref}",nbinspt,ptmin,ptmax);

                  if (dorelrsp){
                    hname=flavors[iflv]+"RelRsp_"+jetEtaSuffix+"_"+rhoPUSuffix+"_"+refPtSuffix;
                    relRspRhoPURefPt[irhopu][ipt][iflv]=new TH1F(hname.c_str(),";p_{T}/p_{T}^{ref}",nbinsrelrsp,relrspmin,relrspmax);
                  }
                }
                if (dojetpt){
                  hname=flavors[iflv]+"JetPt_"+jetEtaSuffix+"_"+rhoPUSuffix+"_"+jetPtSuffix;
                  jetPtRhoPUJetPt[irhopu][ipt][iflv]=new TH1F(hname.c_str(),";p_{T}",nbinspt, ptmin, ptmax);

                  if (dorelrsp){
                    hname=flavors[iflv]+"RelRsp_"+jetEtaSuffix+"_"+rhoPUSuffix+"_"+jetPtSuffix;
                    relRspRhoPUJetPt[irhopu][ipt][iflv]=new TH1F(hname.c_str(),";p_{T}/p_{T}^{ref}",nbinsrelrsp,relrspmin,relrspmax);
                  }
                }
              }
            }
          }
        }

        for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++) {

          string hname; float ptmin=binspt[ipt]; float ptmax=binspt[ipt+1];

          string jetPtSuffix=get_suffix("JetPt",ipt,binspt);
          string refPtSuffix=get_suffix("RefPt",ipt,binspt);
	  
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
	    
            if (dojetpt) {
              hname=flavors[iflv]+"JetPt_"+jetEtaSuffix+"_"+jetPtSuffix;
              jetPtJetPt[ipt][iflv]=new TH1F(hname.c_str(),";p_{T}",
                                             nbinspt,ptmin,ptmax);
            }
	    
            if (dorefpt) {
              hname=flavors[iflv]+"RefPt_"+jetEtaSuffix+"_"+refPtSuffix;
              refPtRefPt[ipt][iflv]=new TH1F(hname.c_str(),";p_{T}^{ref}",
                                             nbinspt,ptmin,ptmax);
            }
	    
            if (dorefpt) {             
              hname=flavors[iflv]+"JetPt_"+jetEtaSuffix+"_"+refPtSuffix;
              jetPtRefPt[ipt][iflv]=new TH1F(hname.c_str(),";p_{T}",
                                             3*nbinspt,
                                             0,
                                             3.0*ptmax);
            }

            if (dorefpt) {             
              hname=flavors[iflv]+"pThat_"+jetEtaSuffix+"_"+refPtSuffix;
              pThatRefPt[ipt][iflv]=new TH1F(hname.c_str(),";#hat{p}_{T}",
                                            (int)binspt[binspt.size()-1]/10.0,
                                            0,
                                            binspt[binspt.size()-1]);
            }

            if (dorelrsp&&dojetpt) {
              hname=flavors[iflv]+"RelRsp_"+jetEtaSuffix+"_"+jetPtSuffix;
              relRspJetPt[ipt][iflv]=new TH1F(hname.c_str(),";p_{T}/p_{T}^{ref}",
                                              nbinsrelrsp,relrspmin,relrspmax);
            }
	    
            if (dorelrsp&&dorefpt) {
              hname=flavors[iflv]+"RelRsp_"+jetEtaSuffix+"_"+refPtSuffix;
              relRspRefPt[ipt][iflv]=new TH1F(hname.c_str(),";p_{T}/p_{T}^{ref}",
                                              nbinsrelrsp,relrspmin,relrspmax);
            }
	    
            if (doabsrsp&&dojetpt) {
              hname=flavors[iflv]+"AbsRsp_"+jetEtaSuffix+"_"+jetPtSuffix;
              absRspJetPt[ipt][iflv]=new TH1F(hname.c_str(),
                                              ";p_{T}-p_{T}^{ref} [GeV]",
                                              nbinsabsrsp,absrspmin,absrspmax);
            }
	    
            if (doabsrsp&&dorefpt) {
              hname=flavors[iflv]+"AbsRsp_"+jetEtaSuffix+"_"+refPtSuffix;
              absRspRefPt[ipt][iflv]=new TH1F(hname.c_str(),
                                              ";p_{T}-p_{T}^{ref} [GeV]",
                                              nbinsabsrsp,absrspmin,absrspmax);
            }
            if (doabsrsp&&dojetpt) {
              hname=flavors[iflv]+"AbsRsp_"+jetEtaSuffix+"_"+jetPtSuffix;
              absRspJetPt[ipt][iflv]=new TH1F(hname.c_str(),
                                              ";|#eta|-|#eta^{ref}|",
                                              nbinsabsrsp,absrspmin,absrspmax);
            }
	    
            if (doetarsp&&dorefpt) {
              hname=flavors[iflv]+"EtaRsp_"+jetEtaSuffix+"_"+refPtSuffix;
              etaRspRefPt[ipt][iflv]=new TH1F(hname.c_str(),
                                              ";|#eta|-|#eta^{ref}|",
                                              nbinsetarsp,etarspmin,etarspmax);
            }

            if (doetarsp&&dojetpt) {
              hname=flavors[iflv]+"EtaRsp_"+jetEtaSuffix+"_"+jetPtSuffix;
              etaRspJetPt[ipt][iflv]=new TH1F(hname.c_str(),
                                              ";#phi-#phi^{ref}",
                                              nbinsetarsp,etarspmin,etarspmax);
            }
	    
            if (dophirsp&&dorefpt) {
              hname=flavors[iflv]+"PhiRsp_"+jetEtaSuffix+"_"+refPtSuffix;
              phiRspRefPt[ipt][iflv]=new TH1F(hname.c_str(),
                                              ";#phi-#phi^{ref}",
                                              nbinsphirsp,phirspmin,phirspmax);
            }
          }
        }
        if (dojetpt)                        jetPtVsJetEtaJetPt      .push_back(jetPtJetPt);
        if (dojetpt&&domu)                  jetPtVsJetEtaMuJetPt    .push_back(jetPtMuJetPt);
        if (dojetpt&&dorho)                 jetPtVsJetEtaRhoJetPt   .push_back(jetPtRhoJetPt);
        if (dojetpt&&dopudensity)           jetPtVsJetEtaRhoPUJetPt .push_back(jetPtRhoPUJetPt);
        if (dorefpt)                        refPtVsJetEtaRefPt      .push_back(refPtRefPt);
        if (dorefpt&&domu)                  refPtVsJetEtaMuRefPt    .push_back(refPtMuRefPt);
        if (dorefpt&&dorho)                 refPtVsJetEtaRhoRefPt   .push_back(refPtRhoRefPt);
        if (dorefpt&&dopudensity)           refPtVsJetEtaRhoPURefPt .push_back(refPtRhoPURefPt);
        if (dorefpt)                        jetPtVsJetEtaRefPt      .push_back(jetPtRefPt);
        if (dorefpt&&domu)                  jetPtVsJetEtaMuRefPt    .push_back(jetPtMuRefPt);
        if (dorefpt&&dorho)                 jetPtVsJetEtaRhoRefPt   .push_back(jetPtRhoRefPt);
        if (dorefpt&&dopudensity)           jetPtVsJetEtaRhoPURefPt .push_back(jetPtRhoPURefPt);
        if (dorefpt)                        pThatVsJetEtaRefPt      .push_back(pThatRefPt);
        if (dorelrsp&&dojetpt)              relRspVsJetEtaJetPt     .push_back(relRspJetPt);
        if (dorelrsp&&dojetpt&&domu)        relRspVsJetEtaMuJetPt   .push_back(relRspMuJetPt);
        if (dorelrsp&&dojetpt&&dorho)       relRspVsJetEtaRhoJetPt  .push_back(relRspRhoJetPt);
        if (dorelrsp&&dojetpt&&dopudensity) relRspVsJetEtaRhoPUJetPt.push_back(relRspRhoPUJetPt);
        if (dorelrsp&&dorefpt)              relRspVsJetEtaRefPt     .push_back(relRspRefPt);
        if (dorelrsp&&dorefpt&&domu)        relRspVsJetEtaMuRefPt   .push_back(relRspMuRefPt);
        if (dorelrsp&&dorefpt&&dorho)       relRspVsJetEtaRhoRefPt  .push_back(relRspRhoRefPt);
        if (dorelrsp&&dorefpt&&dopudensity) relRspVsJetEtaRhoPURefPt.push_back(relRspRhoPURefPt);
        if (doabsrsp&&dojetpt)              absRspVsJetEtaJetPt     .push_back(absRspJetPt);
        if (doabsrsp&&dorefpt)              absRspVsJetEtaRefPt     .push_back(absRspRefPt);
        if (doetarsp&&dojetpt)              etaRspVsJetEtaJetPt     .push_back(etaRspJetPt);
        if (doetarsp&&dorefpt)              etaRspVsJetEtaRefPt     .push_back(etaRspRefPt);
        if (dophirsp&&dojetpt)              phiRspVsJetEtaJetPt     .push_back(phiRspJetPt);
        if (dophirsp&&dorefpt)              phiRspVsJetEtaRefPt     .push_back(phiRspRefPt);
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
            jetPtJetPt[ipt]=new TH1F*[flavors.size()];
        }
	
        if (dorefpt) {
          refPtRefPt =new TH1F**[binspt.size()];
          for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
            refPtRefPt[ipt]=new TH1F*[flavors.size()];
        }
	
        if (dorefpt) {
          jetPtRefPt =new TH1F**[binspt.size()];
          for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
            jetPtRefPt[ipt]=new TH1F*[flavors.size()];
        }
	
        if (dorelrsp&&dojetpt) {
          relRspJetPt=new TH1F**[binspt.size()-1];
          for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
            relRspJetPt[ipt]=new TH1F*[flavors.size()];
        }
	
        if (dorelrsp&&dorefpt) {
          relRspRefPt=new TH1F**[binspt.size()-1];
          for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
            relRspRefPt[ipt]=new TH1F*[flavors.size()];
        }
	
        if (doabsrsp&&dojetpt) {
          absRspJetPt=new TH1F**[binspt.size()-1];
          for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
            absRspJetPt[ipt]=new TH1F*[flavors.size()];
        }
	
        if (doabsrsp&&dorefpt) {
          absRspRefPt=new TH1F**[binspt.size()-1];
          for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++)
            absRspRefPt[ipt]=new TH1F*[flavors.size()];
        }
	
        string jetYSuffix=get_suffix("JetY",iy,binsy);
	
        for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++) {
	  
          string hname; float ptmin=binspt[ipt]; float ptmax=binspt[ipt+1];
	  
          string jetPtSuffix=get_suffix("JetPt",ipt,binspt);
          string refPtSuffix=get_suffix("RefPt",ipt,binspt);
	  
          for (unsigned int iflv=0;iflv<flavors.size();iflv++) {
	    
            if (dojetpt) {
              hname=flavors[iflv]+"JetPt_"+jetYSuffix+"_"+jetPtSuffix;
              jetPtJetPt[ipt][iflv]=new TH1F(hname.c_str(),";p_{T}",
                                             nbinspt,ptmin,ptmax);
            }
	    
            if (dorefpt) {
              hname=flavors[iflv]+"RefPt_"+jetYSuffix+"_"+refPtSuffix;
              refPtRefPt[ipt][iflv]=new TH1F(hname.c_str(),";p_{T}^{ref}",
                                             nbinspt,ptmin,ptmax);
            }
	    
            if (dorefpt) {
              hname=flavors[iflv]+"JetPt_"+jetYSuffix+"_"+refPtSuffix;
              jetPtRefPt[ipt][iflv]=new TH1F(hname.c_str(),";p_{T}",
                                             3*nbinspt,0,3.0*ptmax);
            }
	    
            if (dorelrsp&&dojetpt) {
              hname=flavors[iflv]+"RelRsp_"+jetYSuffix+"_"+jetPtSuffix;
              relRspJetPt[ipt][iflv]=new TH1F(hname.c_str(),";p_{T}/p_{T}^{ref}",
                                              nbinsrelrsp,relrspmin,relrspmax);
            }
	    
            if (dorelrsp&&dorefpt) {
              hname=flavors[iflv]+"RelRsp_"+jetYSuffix+"_"+refPtSuffix;
              relRspRefPt[ipt][iflv]=new TH1F(hname.c_str(),";p_{T}/p_{T}^{ref}",
                                              nbinsrelrsp,relrspmin,relrspmax);
            }
	    
            if (doabsrsp&&dojetpt) {
              hname=flavors[iflv]+"AbsRsp_"+jetYSuffix+"_"+jetPtSuffix;
              absRspJetPt[ipt][iflv]=new TH1F(hname.c_str(),
                                              ";p_{T}-p_{T}^{ref} [GeV]",
                                              nbinsabsrsp,absrspmin,absrspmax);
            }
	    
            if (doabsrsp&&dorefpt) {
              hname=flavors[iflv]+"AbsRsp_"+jetYSuffix+"_"+refPtSuffix;
              absRspRefPt[ipt][iflv]=new TH1F(hname.c_str(),
                                              ";p_{T}-p_{T}^{ref} [GeV]",
                                              nbinsabsrsp,absrspmin,absrspmax);
            }
            if (doabsrsp&&dojetpt) {
              hname=flavors[iflv]+"AbsRsp_"+jetYSuffix+"_"+jetPtSuffix;
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
    
    tree->GetEntry( el->GetEntry(0) );         
    int itInd = itIndex(JRAEvt->bxns);

    unsigned int nevt = (unsigned) el->GetN();
    for (unsigned int ievt=0;ievt<nevt;ievt++)
      {
        if(ievt%10000==0)
          cout << "\tDoing event " << ievt << " ";
        const Long64_t ientry = el->GetEntry(ievt);
        tree->GetEntry(ientry);

        float mu = (NpuNotMu) ? JRAEvt->npus->at(itInd) : JRAEvt->tnpus->at(itInd);
        float rho = JRAEvt->rho;
        float rhoPU = (GRhoPU) ? JRAEvt->gpudensity : JRAEvt->pudensity;
        float pthat = JRAEvt->pthat;
        bool evt_fill = true;

        if(JRAEvt->refdrjt->size()!=(unsigned int)JRAEvt->nref) {
          if(verbose) cout << "WARNING::The number of reference jets doesn't match the number of stored values!" << endl;
          continue;
        }
        if(pthatmin>0.0 && pthat<pthatmin) {
           if(verbose) cout << "WARNING::The pthat of this event is less than the minimum pthat!" << endl;
           continue;
        }
        if(pthatmax!=-1.0 && pthat>pthatmax) {
           if(verbose) cout << "WARNING::The pthat of this event is greater than the maximum pthat!" << endl;
           continue;
        }

        if (nrefmax>0) JRAEvt->nref = std::min((int)JRAEvt->nref,nrefmax);
        for (unsigned char iref=0;iref<JRAEvt->nref;iref++) {
          if(ievt%10000==0 && iref<JRAEvt->nref-1)
            cout << ".";
          else if(ievt%10000==0 && iref==JRAEvt->nref-1)
            cout << ". DONE" << endl;
   
          if (( dobalance&&JRAEvt->refdphijt->at(iref)<dphimin)||
              (!dobalance&&JRAEvt->refdrjt->at(iref)>drmax_alg)) {
            if(verbose) cout << "WARNING::Failed refdrjt or refdphijt cut!" << endl;
            continue;
          }
        
          if (JRAEvt->jtpt->at(iref)<jtptmin) {
            if(verbose) cout << "WARNING::jtpt[" << iref << "]<" << jtptmin << endl;
            continue;
          }

          if (!pileup_cut(itlow,ithigh,earlyootlow,earlyoothigh,lateootlow,lateoothigh,
                          totalootlow,totaloothigh,JRAEvt->npus,JRAEvt->bxns)) {
            if(verbose) cout << "WARNING::Failed pileup cut!" << endl;
            continue;
          }

          float eta    =
            (binseta.size()&&binseta.front()>=0.)?std::abs(JRAEvt->jteta->at(iref)):JRAEvt->jteta->at(iref);
          float y      =
            (binsy.size()&&binsy.front()>=0.)?std::abs(JRAEvt->jty->at(iref)):JRAEvt->jty->at(iref);
          float pt     = JRAEvt->jtpt->at(iref);
          float refpt  = JRAEvt->refpt->at(iref);
          float phi    = JRAEvt->jtphi->at(iref);
          float pdgid(0);
          if(doflavor) {
            flavorDefinition.ToUpper();
            if(flavorDefinition.CompareTo("ALGO")==0)
               pdgid = JRAEvt->refpdgid_algorithmicDef->at(iref);
            else if(flavorDefinition.CompareTo("PHYS")==0)
               pdgid = JRAEvt->refpdgid_physicsDef->at(iref);
            else
               pdgid = JRAEvt->refpdgid->at(iref);
          }
          float absrsp = pt-refpt;
          float relrsp = pt/refpt;
          float etarsp = JRAEvt->jteta->at(iref)-JRAEvt->refeta->at(iref);
          float phirsp = fmod(JRAEvt->jtphi->at(iref)-JRAEvt->refphi->at(iref)+3*M_PI,2*M_PI)-M_PI;

          //
          // retrieve the correct weight
          //
          if(xsection>0.0) weight = (xsection*luminosity)/nevt;
          if(useweight) weight = JRAEvt->weight;
          if(!(xsection>0.0) && !useweight) weight = 1.0;
          if(!weightfile.IsNull())
          {
             if(!doflavor)
             {
                weight *= weightMap["all_"]->GetBinContent(weightMap["all_"]->FindBin(refpt,eta));
             }
             else if(doflavor)
             {
                weight *= weightMap["all_"]->GetBinContent(weightMap["all_"]->FindBin(refpt,eta));
                if(noabsflavors)
                   flavorWeight *= weightMap[pdgid_to_flavor_name(pdgid)]->GetBinContent(weightMap[pdgid_to_flavor_name(pdgid)]->FindBin(refpt,eta));
                else
                   flavorWeight *= weightMap[pdgid_to_flavor_name(fabs(pdgid))]->GetBinContent(weightMap[pdgid_to_flavor_name(fabs(pdgid))]->FindBin(refpt,eta));
             }
          }
          else
             flavorWeight *= weight;
          if(!MCPUReWeighting.IsNull() && !DataPUReWeighting.IsNull()) {
             double LumiWeight = LumiWeights_.weight(JRAEvt->tnpus->at(itIndex(JRAEvt->bxns)));
             weight *= LumiWeight;
          }

          if (eta>=etabarrelmin&&eta<=etabarrelmax) {
            if (dorefpt) {
               fill_histo(refpt,weight,refpt,binspt,refPtVsRefPtBarrel);
               fill_histo(pt,weight,refpt,binspt,jetPtVsRefPtBarrel);
              if (doflavor) {
                fill_histo(pdgid,refpt,flavorWeight,
                           refpt,binspt,refPtVsRefPtBarrel,noabsflavors);
                fill_histo(pdgid,pt,flavorWeight,
                           refpt,binspt,jetPtVsRefPtBarrel,noabsflavors);
              }
            }
            if (dorelrsp&&dorefpt) {
               fill_histo(relrsp,weight,refpt,binspt,relRspVsRefPtBarrel);
              if (doflavor) fill_histo(pdgid,relrsp,flavorWeight,
                                       refpt,binspt,relRspVsRefPtBarrel,
                                       noabsflavors);
            }
            if (doabsrsp&&dorefpt) {
               fill_histo(absrsp,weight,refpt,binspt,absRspVsRefPtBarrel);
              if (doflavor) fill_histo(pdgid,absrsp,flavorWeight,
                                       refpt,binspt,absRspVsRefPtBarrel,
                                       noabsflavors);
            }
          }
          if ((eta>=etaiendcapmin&&eta<etabarrelmin)||(eta>etabarrelmax&&eta<=etaiendcapmax)) {
            if (dorefpt) {
               fill_histo(refpt,weight,refpt,binspt,refPtVsRefPtInnerEndcap);
               fill_histo(pt,weight,refpt,binspt,jetPtVsRefPtInnerEndcap);
              if (doflavor) {
                fill_histo(pdgid,refpt,flavorWeight,
                           refpt,binspt,refPtVsRefPtInnerEndcap,noabsflavors);
                fill_histo(pdgid,pt,flavorWeight,
                           refpt,binspt,jetPtVsRefPtInnerEndcap,noabsflavors);
              }
            }
            if (dorelrsp&&dorefpt) {
               fill_histo(relrsp,weight,refpt,binspt,relRspVsRefPtInnerEndcap);
              if (doflavor) fill_histo(pdgid,relrsp,flavorWeight,
                                       refpt,binspt,relRspVsRefPtInnerEndcap,
                                       noabsflavors);
            }
          }

          if ((eta>=etaoendcapmin&&eta<etaiendcapmin)||(eta>etaiendcapmax&&eta<=etaoendcapmax)) {
            if (dorefpt) {
               fill_histo(refpt,weight,refpt,binspt,refPtVsRefPtOuterEndcap);
               fill_histo(pt,weight,refpt,binspt,jetPtVsRefPtOuterEndcap);
              if (doflavor) {
                fill_histo(pdgid,refpt,flavorWeight,
                           refpt,binspt,refPtVsRefPtOuterEndcap,noabsflavors);
                fill_histo(pdgid,pt,flavorWeight,
                           refpt,binspt,jetPtVsRefPtOuterEndcap,noabsflavors);
              }
            }
            if (dorelrsp&&dorefpt) {
               fill_histo(relrsp,weight,refpt,binspt,relRspVsRefPtOuterEndcap);
              if (doflavor) fill_histo(pdgid,relrsp,flavorWeight,
                                       refpt,binspt,relRspVsRefPtOuterEndcap,
                                       noabsflavors);
            }
          }
          if ((eta>=etaforwardmin&&eta<etaoendcapmin)||(eta>etaoendcapmax&&eta<=etaforwardmax)) {
            if (dorefpt) {
               fill_histo(refpt,weight,refpt,binspt,refPtVsRefPtForward);
               fill_histo(pt,weight,refpt,binspt,jetPtVsRefPtForward);
              if (doflavor) {
                fill_histo(pdgid,refpt,flavorWeight,
                           refpt,binspt,refPtVsRefPtForward,noabsflavors);
                fill_histo(pdgid,pt,flavorWeight,
                           refpt,binspt,jetPtVsRefPtForward,noabsflavors);
              }
            }
            if (dorelrsp&&dorefpt) {
               fill_histo(relrsp,weight,refpt,binspt,relRspVsRefPtForward);
              if (doflavor) fill_histo(pdgid,relrsp,flavorWeight,
                                       refpt,binspt,relRspVsRefPtForward,
                                       noabsflavors);
            }
          }

          if (dojetpt) {
            fill_histo(pt,weight,pt, binspt,jetPtVsJetPt);
            if (doflavor)
              fill_histo(pdgid,pt,flavorWeight,
                         pt,binspt,jetPtVsJetPt,noabsflavors);
          }
          if (dorefpt) {
            fill_histo(refpt,weight,refpt,binspt,refPtVsRefPt);
            fill_histo(pt,weight,refpt,binspt,jetPtVsRefPt);
            if (doflavor) {
              fill_histo(pdgid,refpt,flavorWeight,
                         refpt,binspt,refPtVsRefPt,noabsflavors);
              fill_histo(pdgid,pt,flavorWeight,
                         refpt,binspt,jetPtVsRefPt,noabsflavors);
            }
          }
	
          fill_histo(eta,weight,eta,binseta,jetEtaVsJetEta);
          if (doflavor) fill_histo(pdgid,eta,flavorWeight,
                                   eta,binseta,jetEtaVsJetEta,noabsflavors);
	
          fill_histo(phi,weight,phi,binsphi,jetPhiVsJetPhi);
          if (doflavor) fill_histo(pdgid,phi,flavorWeight,
                                   phi,binsphi,jetPhiVsJetPhi,
                                   noabsflavors);
 
          fill_histo(y,weight,y,binsy,jetYVsJetY);
          if (doflavor) fill_histo(pdgid,y,flavorWeight,
                                   y,binsy,jetYVsJetY,noabsflavors);

          if (dojetpt) {
            fill_histo(pt,weight,eta,pt,binseta,binspt,jetPtVsJetEtaJetPt);
            if (domu)        fill_histo(pt, weight, eta, mu,  pt, binseta, binsmu,  binspt, jetPtVsJetEtaMuJetPt);
            if (dorho)       fill_histo(pt, weight, eta, rho, pt, binseta, binsrho, binspt, jetPtVsJetEtaRhoJetPt);
            if (dopudensity) fill_histo(pt, weight, eta, rhoPU, pt, binseta, binspudensity, binspt, jetPtVsJetEtaRhoPUJetPt);

            fill_histo(pt,weight,y,pt,
                       binsy,binspt,jetPtVsJetYJetPt);

            if (doflavor) {
              fill_histo(pdgid,pt,flavorWeight,
                         eta,pt,binseta,binspt,jetPtVsJetEtaJetPt,
                         noabsflavors);
              if (domu)        fill_histo(pdgid,pt, flavorWeight, eta, mu,  pt, binseta, binsmu,  binspt, jetPtVsJetEtaMuJetPt,  noabsflavors);
              if (dorho)       fill_histo(pdgid,pt, flavorWeight, eta, rho, pt, binseta, binsrho, binspt, jetPtVsJetEtaRhoJetPt, noabsflavors);
              if (dopudensity) fill_histo(pdgid,pt, flavorWeight, eta, rhoPU, pt, binseta, binspudensity, binspt, jetPtVsJetEtaRhoPUJetPt, noabsflavors);

              fill_histo(pdgid,pt,flavorWeight,
                         y,pt,binsy,binspt,jetPtVsJetYJetPt,
                         noabsflavors);
            }
          }

          if (dorefpt) {
            fill_histo(refpt,weight,eta,refpt,binseta,binspt,refPtVsJetEtaRefPt);
            if (domu)        fill_histo(refpt, weight, eta, mu,  refpt, binseta, binsmu,  binspt, refPtVsJetEtaMuRefPt);
            if (dorho)       fill_histo(refpt, weight, eta, rho, refpt, binseta, binsrho, binspt, refPtVsJetEtaRhoRefPt);
            if (dopudensity) fill_histo(refpt, weight, eta, rhoPU, refpt, binseta, binspudensity, binspt, refPtVsJetEtaRhoPURefPt);

            fill_histo(pt,weight,eta,refpt,binseta,binspt,jetPtVsJetEtaRefPt);
            if (domu)        fill_histo(pt, weight, eta, mu,  refpt, binseta, binsmu,  binspt, jetPtVsJetEtaMuRefPt);
            if (dorho)       fill_histo(pt, weight, eta, rho, refpt, binseta, binsrho, binspt, jetPtVsJetEtaRhoRefPt);
            if (dopudensity) fill_histo(pt, weight, eta, rhoPU, refpt, binseta, binspudensity, binspt, jetPtVsJetEtaRhoPURefPt);

            if(evt_fill) {fill_histo(pthat,weight,eta,refpt,binseta,binspt,pThatVsJetEtaRefPt); if(!doflavor) {evt_fill=false;}}
            fill_histo(refpt,weight,y,refpt,binsy,binspt,refPtVsJetYRefPt);
            fill_histo(pt,weight,y,refpt,binsy,binspt,jetPtVsJetYRefPt);
            if (doflavor) {
              fill_histo(pdgid,refpt,flavorWeight,eta,refpt,binseta,binspt,refPtVsJetEtaRefPt,noabsflavors);
              if (domu)       fill_histo(pdgid, refpt, flavorWeight, eta, mu,  refpt, binseta, binsmu,  binspt, refPtVsJetEtaMuRefPt,  noabsflavors);
              if (dorho)      fill_histo(pdgid, refpt, flavorWeight, eta, rho, refpt, binseta, binsrho, binspt, refPtVsJetEtaRhoRefPt, noabsflavors);
              if (dopudensity) fill_histo(pdgid, refpt, flavorWeight, eta, rhoPU, refpt, binseta, binspudensity, binspt, refPtVsJetEtaRhoPURefPt, noabsflavors);

              fill_histo(pdgid,pt,flavorWeight,eta,refpt,binseta,binspt,jetPtVsJetEtaRefPt,noabsflavors);
              if (domu)        fill_histo(pdgid, pt, flavorWeight, eta, mu,  refpt, binseta, binsmu,  binspt, jetPtVsJetEtaMuRefPt,  noabsflavors);
              if (dorho)       fill_histo(pdgid, pt, flavorWeight, eta, rho, refpt, binseta, binsrho, binspt, jetPtVsJetEtaRhoRefPt, noabsflavors);
              if (dopudensity) fill_histo(pdgid, pt, flavorWeight, eta, rhoPU, refpt, binseta, binspudensity, binspt, jetPtVsJetEtaRhoPURefPt, noabsflavors);

              fill_histo(pdgid,refpt,flavorWeight,y,refpt,binsy,binspt,refPtVsJetYRefPt,noabsflavors);
              fill_histo(pdgid,pt,flavorWeight,y,refpt,binsy,binspt,jetPtVsJetYRefPt,noabsflavors);
              if(evt_fill) {fill_histo(pdgid,pthat,flavorWeight,eta,refpt,binseta,binspt,pThatVsJetEtaRefPt,noabsflavors); evt_fill=false;}
            }
          }
	
          if (dorelrsp) {
            if (dojetpt) {
              fill_histo(relrsp,weight,pt,binspt,relRspVsJetPt);
              if (doflavor) fill_histo(pdgid,relrsp,flavorWeight,
                                       pt, binspt,relRspVsJetPt,
                                       noabsflavors);
            }
            if (dorefpt) {
              fill_histo(relrsp,weight,refpt,binspt,relRspVsRefPt);
              if (doflavor) fill_histo(pdgid,relrsp,flavorWeight,
                                       refpt,binspt,relRspVsRefPt,
                                       noabsflavors);
            }

            fill_histo(relrsp,weight,eta,binseta,relRspVsJetEta);
            if (doflavor) fill_histo(pdgid,relrsp,flavorWeight,
                                     eta,binseta,relRspVsJetEta,
                                     noabsflavors);
	  
            fill_histo(relrsp,weight,phi,binsphi,relRspVsJetPhi);
            if (doflavor) fill_histo(pdgid,relrsp,flavorWeight,
                                     phi,binsphi,relRspVsJetPhi,
                                     noabsflavors);
	  
            fill_histo(relrsp,weight,y,binsy,relRspVsJetY);
            if (doflavor) fill_histo(pdgid,relrsp,flavorWeight,
                                     y,binsy,relRspVsJetY,
                                     noabsflavors);
	  
            if (dojetpt) {
              fill_histo(relrsp,weight,eta,pt,binseta,binspt,relRspVsJetEtaJetPt);
              if (domu)        fill_histo(relrsp, weight, eta, mu,  pt, binseta, binsmu,  binspt, relRspVsJetEtaMuJetPt);
              if (dorho)       fill_histo(relrsp, weight, eta, rho, pt, binseta, binsrho, binspt, relRspVsJetEtaRhoJetPt);
              if (dopudensity) fill_histo(relrsp, weight, eta, rhoPU, pt, binseta, binspudensity, binspt, relRspVsJetEtaRhoPUJetPt);

              fill_histo(relrsp,weight,y,pt,
                         binsy,binspt,relRspVsJetYJetPt);
              if (doflavor) {
                fill_histo(pdgid,relrsp,flavorWeight,eta,pt,
                           binseta,binspt,relRspVsJetEtaJetPt,noabsflavors);
                if (domu)        fill_histo(pdgid,relrsp, flavorWeight, eta, mu,  pt, binseta, binsmu,  binspt, relRspVsJetEtaMuJetPt,  noabsflavors);
                if (dorho)       fill_histo(pdgid,relrsp, flavorWeight, eta, rho, pt, binseta, binsrho, binspt, relRspVsJetEtaRhoJetPt, noabsflavors);
                if (dopudensity) fill_histo(pdgid,relrsp, flavorWeight, eta, rhoPU, pt, binseta, binspudensity, binspt, relRspVsJetEtaRhoPUJetPt, noabsflavors);

                fill_histo(pdgid,relrsp,flavorWeight,y,pt,
                           binsy,binspt,relRspVsJetYJetPt,noabsflavors);
              }
            }
            if (dorefpt) {
              fill_histo(relrsp,weight,eta,refpt,binseta,binspt,relRspVsJetEtaRefPt);
              if (domu)        fill_histo(relrsp, weight, eta, mu,  refpt, binseta, binsmu,  binspt, relRspVsJetEtaMuRefPt);
              if (dorho)       fill_histo(relrsp, weight, eta, rho, refpt, binseta, binsrho, binspt, relRspVsJetEtaRhoRefPt);
              if (dopudensity) fill_histo(relrsp, weight, eta, rhoPU, refpt, binseta, binspudensity, binspt, relRspVsJetEtaRhoPURefPt);

              fill_histo(relrsp,weight,y,refpt,
                         binsy,binspt,relRspVsJetYRefPt);
              if (doflavor) {
                fill_histo(pdgid,relrsp,flavorWeight,eta,refpt,
                           binseta,binspt,relRspVsJetEtaRefPt,noabsflavors);
                if (domu)        fill_histo(pdgid,relrsp, flavorWeight, eta, mu,  refpt, binseta, binsmu,  binspt, relRspVsJetEtaMuRefPt,  noabsflavors);
                if (dorho)       fill_histo(pdgid,relrsp, flavorWeight, eta, rho, refpt, binseta, binsrho, binspt, relRspVsJetEtaRhoRefPt, noabsflavors);
                if (dopudensity) fill_histo(pdgid,relrsp, flavorWeight, eta, rhoPU, refpt, binseta, binspudensity, binspt, relRspVsJetEtaRhoPURefPt, noabsflavors);

                fill_histo(pdgid,relrsp,flavorWeight,y,refpt,
                           binsy,binspt,relRspVsJetYRefPt,noabsflavors);
              }
            }
          }
	
          if (doabsrsp) {
            if (dojetpt) {
              fill_histo(absrsp,weight,pt, binspt,absRspVsJetPt);
              if (doflavor) fill_histo(pdgid,absrsp,flavorWeight,
                                       pt,binspt,absRspVsJetPt,
                                       noabsflavors);
            }
            if (dorefpt) {
              fill_histo(absrsp,weight,refpt,binspt,absRspVsRefPt);
              if (doflavor) fill_histo(pdgid,absrsp,flavorWeight,
                                       refpt,binspt,absRspVsRefPt,
                                       noabsflavors);
            }
	  
            fill_histo(absrsp,weight,eta,binseta,absRspVsJetEta);
            if (doflavor) fill_histo(pdgid,absrsp,flavorWeight,
                                     eta,binseta,absRspVsJetEta,
                                     noabsflavors);

            fill_histo(absrsp,weight,phi,binsphi,absRspVsJetPhi);
            if (doflavor) fill_histo(pdgid,absrsp,flavorWeight,
                                     phi,binsphi,absRspVsJetPhi,
                                     noabsflavors);
	  
            fill_histo(absrsp,weight,y,binsy,absRspVsJetY);
            if (doflavor) fill_histo(pdgid,absrsp,flavorWeight,
                                     y,binsy,absRspVsJetY,
                                     noabsflavors);
	  
            if (dojetpt) {
              fill_histo(absrsp,weight,eta,pt,
                         binseta,binspt,absRspVsJetEtaJetPt);
              fill_histo(absrsp,weight,y,pt,
                         binsy,binspt,absRspVsJetYJetPt);
              if (doflavor) {
                fill_histo(pdgid,absrsp,flavorWeight,eta,pt,
                           binseta,binspt,absRspVsJetEtaJetPt,noabsflavors);
                fill_histo(pdgid,absrsp,flavorWeight,y,pt,
                           binsy,binspt,absRspVsJetYJetPt,noabsflavors);
              }
            }
            if (dorefpt) {
              fill_histo(absrsp,weight,eta,refpt,
                         binseta,binspt,absRspVsJetEtaRefPt);
              fill_histo(absrsp,weight,y,refpt,
                         binsy,binspt,absRspVsJetYRefPt);
              if (doflavor) {
                fill_histo(pdgid,absrsp,flavorWeight,eta,refpt,
                           binseta,binspt,absRspVsJetEtaRefPt,noabsflavors);
                fill_histo(pdgid,absrsp,flavorWeight,y,refpt,
                           binsy,binspt,absRspVsJetYRefPt,noabsflavors);
              }
            }
          }
	
          if (doetarsp) {
            if (dojetpt) {
              fill_histo(etarsp,weight,pt, binspt,etaRspVsJetPt);
              if (doflavor) fill_histo(pdgid,etarsp,flavorWeight,
                                       pt,binspt,etaRspVsJetPt,
                                       noabsflavors);
            }
            if (dorefpt) {
              fill_histo(etarsp,weight,refpt,binspt,etaRspVsRefPt);
              if (doflavor) fill_histo(pdgid,etarsp,flavorWeight,
                                       refpt,binspt,etaRspVsRefPt,
                                       noabsflavors);
            }
	  
            fill_histo(etarsp,weight,eta,binseta,etaRspVsJetEta);
            if (doflavor) fill_histo(pdgid,etarsp,flavorWeight,
                                     eta,binseta,etaRspVsJetEta,
                                     noabsflavors);
	  
            fill_histo(etarsp,weight,phi,binsphi,etaRspVsJetPhi);
            if (doflavor) fill_histo(pdgid,etarsp,flavorWeight,
                                     phi,binsphi,etaRspVsJetPhi,
                                     noabsflavors);
	  
            if (dojetpt) {
              fill_histo(etarsp,weight,eta,pt,
                         binseta,binspt,etaRspVsJetEtaJetPt);
              if (doflavor) fill_histo(pdgid,etarsp,flavorWeight,
                                       eta,pt,
                                       binseta,binspt,etaRspVsJetEtaJetPt,
                                       noabsflavors);
            }
            if (dorefpt) {
              fill_histo(etarsp,weight,eta,refpt,
                         binseta,binspt,etaRspVsJetEtaRefPt);
              if (doflavor) fill_histo(pdgid,etarsp,flavorWeight,
                                       eta,refpt,
                                       binseta,binspt,etaRspVsJetEtaRefPt,
                                       noabsflavors);
            }
          }
	
          if (dophirsp) {
            if (dojetpt) {
              fill_histo(phirsp,weight,pt, binspt,phiRspVsJetPt);
              if (doflavor) fill_histo(pdgid,phirsp,flavorWeight,
                                       pt,binspt,phiRspVsJetPt,
                                       noabsflavors);
            }
            if (dorefpt) {
              fill_histo(phirsp,weight,refpt,binspt,phiRspVsRefPt);
              if (doflavor) fill_histo(pdgid,phirsp,flavorWeight,
                                       refpt,binspt,phiRspVsRefPt,
                                       noabsflavors);
            }
	  
            fill_histo(phirsp,weight,eta,binseta,phiRspVsJetEta);
            if (doflavor) fill_histo(pdgid,phirsp,flavorWeight,
                                     eta,binseta,phiRspVsJetEta,
                                     noabsflavors);
	  
            fill_histo(phirsp,weight,phi,binsphi,phiRspVsJetPhi);
            if (doflavor) fill_histo(pdgid,phirsp,flavorWeight,
                                     phi,binsphi,phiRspVsJetPhi,
                                     noabsflavors);
	  
            if (dojetpt) {
              fill_histo(phirsp,weight,eta,pt,
                         binseta,binspt,phiRspVsJetEtaJetPt);
              if (doflavor) fill_histo(pdgid,phirsp,flavorWeight,
                                       eta,pt,
                                       binseta,binspt,phiRspVsJetEtaJetPt,
                                       noabsflavors);
            }
            if (dorefpt) {
              fill_histo(phirsp,weight,eta,refpt,
                         binseta,binspt,phiRspVsJetEtaRefPt);
              if (doflavor) fill_histo(pdgid,phirsp,flavorWeight,
                                       eta,refpt,
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
      flavor.push_back("ud_");
      flavor.push_back("s_");
      flavor.push_back("c_");
      flavor.push_back("b_");
      flavor.push_back("g_");
      flavor.push_back("slc_");
      flavor.push_back("slb_");
      flavor.push_back("q_");
      flavor.push_back("uds_");
   }
   else {
      flavor.push_back("ud_");
      flavor.push_back("udbar_");
      flavor.push_back("s_");
      flavor.push_back("sbar_");
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
   if(pdgid<=2 && pdgid>=1)
      return "ud_";
   else if(pdgid>=-2 && pdgid<=-1)
      return "udbar_";
   else if(pdgid==3)
      return "s_";
   else if(pdgid==-3)
      return "sbar_";
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
      if (pdgid>=1&&pdgid<=2)        iflv=1;
      else if (pdgid<=-1&&pdgid>=-2) iflv=2;
      else if (pdgid==3)             iflv=3;
      else if (pdgid==-3)            iflv=4;
      else if (pdgid==4)             iflv=5;
      else if (pdgid==-4)            iflv=6;
      else if (pdgid==5)             iflv=7;
      else if (pdgid==-5)            iflv=8;
      else if (abspdgid==21)         iflv=9;
      else if (abspdgid==411||
               abspdgid==413)        iflv=10;
      else if (abspdgid==511||
               abspdgid==513)        iflv=11;
      else return;
      
      int ix=get_index(x,binsx);
      if (ix>=0)
        {
          histos[ix][iflv]->Fill(value,weight);
          if((abspdgid>=1&&abspdgid<=5)||abspdgid==21)histos[ix][12]->Fill(value,weight);
        }
    }
  else
    {
      if (abspdgid>=1&&abspdgid<=2) iflv=1;
      else if (abspdgid== 3)        iflv=2;
      else if (abspdgid== 4)        iflv=3;
      else if (abspdgid== 5)        iflv=4;
      else if (abspdgid==21)        iflv=5;
      else if (abspdgid==411||
               abspdgid==413)       iflv=6;
      else if (abspdgid==511||
               abspdgid==513)       iflv=7;
      else return;
      
      int ix=get_index(x,binsx);
      if (ix>=0) histos[ix][iflv]->Fill(value,weight);
      if (ix>=0 && abspdgid>0 && abspdgid<=5) histos[ix][8]->Fill(value,weight);
      if (ix>=0 && abspdgid>0 && abspdgid<=3) histos[ix][9]->Fill(value,weight);
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
      if (pdgid>=1&&pdgid<=2)        iflv=1;
      else if (pdgid<=-1&&pdgid>=-2) iflv=2;
      else if (pdgid==3)             iflv=3;
      else if (pdgid==-3)            iflv=4;
      else if (pdgid==4)             iflv=5;
      else if (pdgid==-4)            iflv=6;
      else if (pdgid==5)             iflv=7;
      else if (pdgid==-5)            iflv=8;
      else if (abspdgid==21)         iflv=9;
      else if (abspdgid==411||
               abspdgid==413)        iflv=10;
      else if (abspdgid==511||
               abspdgid==513)        iflv=11;
      else return;
      
      int ix=get_index(x,binsx);
      int iy=get_index(y,binsy);
      if (ix>=0&&iy>=0) 
        {
          histos[ix][iy][iflv]->Fill(value,weight);
          if((abspdgid>=1&&abspdgid<=5)||abspdgid==21)histos[ix][iy][12]->Fill(value,weight);
        }
    }
  else
    {
      if (abspdgid>=1&&abspdgid<=2) iflv=1;
      else if (abspdgid== 3)        iflv=2;
      else if (abspdgid== 4)        iflv=3;
      else if (abspdgid== 5)        iflv=4;
      else if (abspdgid==21)        iflv=5;
      else if (abspdgid==411||
               abspdgid==413)       iflv=6;
      else if (abspdgid==511||
               abspdgid==513)       iflv=7;
      else return;
      
      int ix=get_index(x,binsx);
      int iy=get_index(y,binsy);
      if (ix>=0&&iy>=0) histos[ix][iy][iflv]->Fill(value,weight);
      if (ix>=0&&iy>=0 && abspdgid>0 && abspdgid<=5) histos[ix][iy][8]->Fill(value,weight);
      if (ix>=0&&iy>=0 && abspdgid>0 && abspdgid<=3) histos[ix][iy][9]->Fill(value,weight);
    }
}

//______________________________________________________________________________
void fill_histo(float value,float weight,float x,float y,float z,
                const vector<float>& binsx,const vector<float>& binsy,const vector<float>& binsz,
                const vector<TH1F****>& histos)
{
  if (binsx.size()==0||binsy.size()==0||binsz.size()==0) return;
  int ix=get_index(x,binsx);
  int iy=get_index(y,binsy);
  int iz=get_index(z,binsz);  
  if (ix>=0&&iy>=0&&iz>=0) histos[ix][iy][iz][0]->Fill(value,weight);
}

//______________________________________________________________________________
void fill_histo(int pdgid,float value,float weight,float x,float y,float z,
                const vector<float>& binsx,const vector<float>& binsy,const vector<float>& binsz,
                const vector<TH1F****>& histos, bool noabsflavors)
{
  if (binsx.size()==0||binsy.size()==0||binsz.size()==0) return;
  int abspdgid=fabs(pdgid);
  int iflv(-1);
  if (noabsflavors)
    {
      if (pdgid>=1&&pdgid<=2)        iflv=1;
      else if (pdgid<=-1&&pdgid>=-2) iflv=2;
      else if (pdgid==3)             iflv=3;
      else if (pdgid==-3)            iflv=4;
      else if (pdgid==4)             iflv=5;
      else if (pdgid==-4)            iflv=6;
      else if (pdgid==5)             iflv=7;
      else if (pdgid==-5)            iflv=8;
      else if (abspdgid==21)         iflv=9;
      else if (abspdgid==411||
               abspdgid==413)        iflv=10;
      else if (abspdgid==511||
               abspdgid==513)        iflv=11;
      else return;

      int ix=get_index(x,binsx);
      int iy=get_index(y,binsy);
      int iz=get_index(z,binsz);
      if (ix>=0&&iy>=0&&iz>=0)
        {
          histos[ix][iy][iz][iflv]->Fill(value,weight);
          if((abspdgid>=1&&abspdgid<=5)||abspdgid==21) histos[ix][iy][iz][12]->Fill(value,weight);
        }
    }
  else
    {
      if (abspdgid>=1&&abspdgid<=2) iflv=1;
      else if (abspdgid== 3)        iflv=2;
      else if (abspdgid== 4)        iflv=3;
      else if (abspdgid== 5)        iflv=4;
      else if (abspdgid==21)        iflv=5;
      else if (abspdgid==411||
               abspdgid==413)       iflv=6;
      else if (abspdgid==511||
               abspdgid==513)       iflv=7;
      else return;

      int ix=get_index(x,binsx);
      int iy=get_index(y,binsy);
      int iz=get_index(z,binsz);
      if (ix>=0&&iy>=0&&iz>=0) histos[ix][iy][iz][iflv]->Fill(value,weight);
      if (ix>=0&&iy>=0&&iz>=0 && abspdgid>0 && abspdgid<=5) histos[ix][iy][iz][8]->Fill(value,weight);
      if (ix>=0&&iy>=0&&iz>=0 && abspdgid>0 && abspdgid<=3) histos[ix][iy][iz][9]->Fill(value,weight);
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
bool it_pileup(int itlow, int ithigh, vector<int>* npus, int iIT)
{
   if((*npus)[iIT]>=itlow && (*npus)[iIT]<=ithigh) return true;
  return false;
}


//______________________________________________________________________________
bool oot_pileup(int earlyootlow, int earlyoothigh, int lateootlow, int lateoothigh,
                vector<int>* npus, int iIT)
{
  if(sumEOOT(npus,iIT)>=earlyootlow && sumEOOT(npus,iIT)<=earlyoothigh && 
     sumLOOT(npus,iIT)>=lateootlow && sumLOOT(npus,iIT)<=lateoothigh) return true;
  return false;
}


//______________________________________________________________________________
bool total_oot_pileup(int totalootlow, int totaloothigh, vector<int>* npus, int iIT)
{
  double sumOOT = sumEOOT(npus,iIT)+sumLOOT(npus,iIT);
  if(sumOOT>=totalootlow && sumOOT<=totaloothigh) return true;
  return false;
}


//______________________________________________________________________________
bool pileup_cut(int itlow, int ithigh, int earlyootlow, int earlyoothigh, 
                int lateootlow, int lateoothigh, int totalootlow, int totaloothigh, 
                vector<int>* npus, vector<int>* bxns)
{
  int iIT = itIndex(bxns);
  if(it_pileup(itlow,ithigh,npus,iIT) && 
     total_oot_pileup(totalootlow,totaloothigh,npus,iIT) && 
     oot_pileup(earlyootlow,earlyoothigh,lateootlow,lateoothigh,npus,iIT)) return true;
  return false;
}

//______________________________________________________________________________
int itIndex(vector<int>* bxns) {
   for(unsigned int ibx=0; ibx<(*bxns).size(); ibx++) {
      if((*bxns)[ibx]==0) return ibx;
   }
   return -1;
}

//______________________________________________________________________________
double sumEOOT(vector<int>* npus, unsigned int iIT) {
   if(iIT>(*npus).size()-1) return 0;
   double sum = 0;
   for(unsigned int ipu=0; ipu<iIT; ipu++) {
      sum+=(*npus)[ipu];
   }
   return sum;
}

//______________________________________________________________________________
double sumLOOT(vector<int>* npus, unsigned int iIT) {
   if(iIT>(*npus).size()-1) return 0;
   double sum = 0;
   for(unsigned int ipu=(*npus).size()-1; ipu>iIT; ipu--) {
      sum+=(*npus)[ipu];
   }
   return sum;
}

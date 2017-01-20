#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TLegend.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TLine.h"
#include "TPaveText.h"
#include "THStack.h"

#include "TLatex.h"
//#include "/home/aperloff/Scripts/tdrstyle_mod14.C"
//#include "tdrstyle_mod14_ia.C"
//#include "settings12.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
//#include "CondFormats/JetMETObjects/interface/Utilities.h"
//#include "CondFormats/JetMETObjects/interface/SimpleJetCorrector.h"
//#include "CondFormats/JetMETObjects/interface/SimpleJetCorrectionUncertainty.h"

#include "FWCore/ParameterSet/interface/FileInPath.h"

#include "JetMETAnalysis/JetUtilities/interface/Style.h"

using namespace std;

// symmetric if both below false
// positive true overrides negative
bool _usenegative = false;
bool _usepositive = false;

bool _useptgen = true ; // iterate to produce JEC vs pTgen
bool _paper    = true ; // graphical settings for the paper (e.g. y-axis range)

const double _mu = 25; //19.83; // 20/fb at 8 TeV (htrpu)
const double _pt = 15. ;
const int _ieta  = 40. ;  // eta=_ieta*0.1

//const double _lumi = 19800.;

string _payld("Winter14_VX");

bool _pdf = true; // save .pdf
bool _C   = false; // save .C
string _alg("");

// ------------------------------------------------------------
double getRho(double mu) {

  double ue = 1.519 ; // 8 TeV
/// check later  if( TString(_payld).Contains("Fall15_25ns") )  ue = 2.;  // 13 TeV

  double p[3] = {1.009 + ue, 0.5515, 0.0003597}; // DATA // ???

  if(TString(_payld).Contains("RunISummer12") || TString(_payld).Contains("Winter14_") )  // Run1 53X MC
  {p[0]=-0.426 + ue; p[1]=0.504; p[2]=0.0005;}

  if(TString(_payld).Contains("Run1Sub53X")) 
  {p[0]=-0.350 + ue; p[1]=0.526; p[2]=0.001;} 

  if(TString(_payld).Contains("Run1Sub742")) 
  {p[0]=-0.855 + ue; p[1]=0.461; p[2]=-0.001;}

  if(TString(_payld).Contains("50nsRunIISpring15") || TString(_payld).Contains("Summer15") || TString(_payld).Contains("RunIISpring15DR74_bx50") )
  {p[0]=-1.035 + ue; p[1]=0.617; p[2]=-0.0005;}

  if(TString(_payld).Contains("25nsRunIISpring15") || TString(_payld).Contains("RunIISpring15DR74_bx25") )
  {p[0]=-0.837 + ue; p[1]=0.658; p[2]=-0.0012;}

//  if( TString(_payld).Contains("Summer15_25nsV5_") )
 // {p[0]=1.462 + ue; p[1]=0.220; p[2]=0.0195;}

  if( TString(_payld).Contains("Summer15_25nsV5")  || TString(_payld).Contains("Summer15_25nsV6") || TString(_payld).Contains("Summer15_25nsV7") )
  {p[0]=-1.036 + ue; p[1]=0.705; p[2]=-0.0016;}

  if( TString(_payld).Contains("Fall15_25ns") )
//  {p[0]=0.404 + ue; p[1]=0.374; p[2]=0.0117;} ??
//  {p[0]=-1.036 + ue; p[1]=0.705; p[2]=-0.0016;} ??
      
  {ue=0.; p[0]=1.084 + ue; p[1]=0.6075; p[2]=0.;}  // Alexx; 

//  if( TString(_payld).Contains("Summer15_25nsV5ch2") )
//  {p[0]=-1.036 + ue; p[1]=0.705; p[2]=-0.0016;}


//  if( TString(_payld).Contains("Summer15_25nsV5Silver1") )
//  {p[0]=0.979 + ue; p[1]=0.340; p[2]=0.0148;}


  if( TString(_payld).Contains("16_25ns") )
  {ue=0.; p[0]=0.929 + ue; p[1]=0.5841; p[2]=-0.0001516;}  // Alexx;



  return (p[0]+p[1]*mu+p[2]*mu*mu);

}


double getNPV(double mu) {

  double p[3] = {1.032, 0.7039, -0.001319}; // DATA

  return (p[0]+p[1]*mu+p[2]*mu*mu);
}


// ------------------------------------------------------------
void setEtaPtE(FactorizedJetCorrector *jec, double eta, double pt, double e,
               int mu) {
  
  assert(jec);
    
  int npv = getNPV(mu);
  double rho = getRho(mu);

  jec->setJetEta(eta);
  jec->setJetPt(pt);
  // L1Offset
  jec->setJetE(e);
  jec->setNPV(npv);
  // L1FastJet
  double r=0.5 ;
  string cone = _alg.substr(2, 1);
  if(cone == "1") r=0.1 ;
  if(cone == "2") r=0.2 ;
  if(cone == "3") r=0.3 ;
  if(cone == "4") r=0.4 ;
  if(cone == "5") r=0.5 ;
  if(cone == "6") r=0.6 ;
  if(cone == "7") r=0.7 ;
  if(cone == "8") r=0.8 ;
  if(cone == "9") r=0.9 ;
  double jeta = TMath::Pi()*r*r;                    

  jec->setJetA(jeta);
  jec->setRho(rho);
                      
  return;
}

// ------------------------------------------------------------

FactorizedJetCorrector *_thejec(0);
TF1 *fCorrPt(0);

Double_t funcCorrPt(Double_t *x, Double_t *p) {

  double eta = p[0];
  double pt = x[0];
  double e = pt * cosh(eta);
  double mu = p[1];
  setEtaPtE(_thejec, eta, pt, e, mu);
               
  return (_thejec->getCorrection() * pt);
}


// ------------------------------------------------------------
double getEtaPtE(FactorizedJetCorrector *jec, double eta, double pt, double e,
                 int mu = _mu) {

  setEtaPtE(jec, eta, pt, e, mu);

  // if using pTgen, need to iterate to solve ptreco
  if (_useptgen) {  

    double ptgen = pt;
    _thejec = jec; 
    fCorrPt->SetParameters(eta, mu);

    // Find ptreco that gives pTreco*JEC = pTgen
    double ptreco = fCorrPt->GetX(ptgen,5,6500);
    
    setEtaPtE(jec, eta, ptreco, e, mu);

  }

  return (jec->getCorrection());
} // getEtaPtE


// ------------------------------------------------------------ //

double getEtaPtUncert(JetCorrectionUncertainty *unc,
                      FactorizedJetCorrector *jec,
                      double eta, double pt, double mu = _mu) {

  assert(unc);

  unc->setJetEta(eta);
  unc->setJetPt(pt);

  // if not using pTgen, need to solve it   
  if (!_useptgen) {

    assert(jec);
    double ptreco = pt;
    _thejec = jec;
    fCorrPt->SetParameters(eta, mu);
    double ptgen = fCorrPt->Eval(ptreco);
  
    unc->setJetEta(eta);
    unc->setJetPt(ptgen);
  }

  return (unc->getUncertainty(true));
} // getEtaPtUncert


// ------------------------------------------------------------ //

void compareJEC(string payld1="Winter14_V8", string payld2="",         string payld3="",  
                string  algo1="AK5PFchs",    string  algo2="AK5PFchs", string algo3 ="AK5PFchs",
                string  type1="DATA",        string  type2="DATA",     string type3 ="DATA",
			bool l1=true, bool l2l3=true, bool res=true) {

  //gROOT->ProcessLine(".L tdrstyle_mod14_ia.C");
  setTDRStyle();

//  cout << " mu and rho " << _mu  << " " << getRho(_mu) << endl ;

  //const char *str;
  string sgen = (_useptgen ? "corr" : "raw");
  const char *cgen = sgen.c_str();

  // Initialize function used to invert JEC
  fCorrPt = new TF1("fCorrPt",funcCorrPt,1,6500,2); //4000.

  // JEC1
  string sid1 =  (payld1=="PHYS14_25_V2") ? payld1 : payld1+"_"+type1 ;
  const char *cid1 = sid1.c_str();
  const char *a1 = algo1.c_str();
  // JEC2
  string sid2 = (payld2=="PHYS14_25_V2") ? payld2 : payld2+"_"+type2 ;
  const char *cid2 = sid2.c_str();
  const char *a2 = algo2.c_str();
  // JEC3
  string sid3 = (payld3=="PHYS14_25_V2") ? payld3 : payld3+"_"+type3 ;
  const char *cid3 = sid3.c_str();
  const char *a3 = algo3.c_str();

  int maxTries = 7;
  string strPath;
  vector<string> paths = {"CondFormats/JetMETObjects/data/"};
  paths.push_back(string("/fdata/hepx/store/user/aperloff/JEC/80X_Summer16/")+cid1+"/");
  paths.push_back(string("/fdata/hepx/store/user/aperloff/JEC/80X_Summer16/")+cid2+"/");
  paths.push_back(string("/fdata/hepx/store/user/aperloff/JEC/80X_Summer16/")+cid3+"/");
  paths.push_back(string("/home/aperloff/JEC/CMSSW_8_0_20/src/JetMETCorrections/JECDatabase/textFiles/")+cid1+"/");
  paths.push_back(string("/home/aperloff/JEC/CMSSW_8_0_20/src/JetMETCorrections/JECDatabase/textFiles/")+cid2+"/");
  paths.push_back(string("/home/aperloff/JEC/CMSSW_8_0_20/src/JetMETCorrections/JECDatabase/textFiles/")+cid3+"/");

  // JEC1
  bool noL2L3_p1 = TString(payld1).Contains("Run1Sub") || TString(payld1).Contains("nsRunIISpring15") || TString(payld1).Contains("RC")  ;
  // 
  //  noL2L3_p1 = true ; //ctmp

  JetCorrectionUncertainty  *jecUnc1   = 0;
  JetCorrectorParameters *JetCorPar1L1 = 0;
  JetCorrectorParameters *JetCorPar1L2 = 0;
  JetCorrectorParameters *JetCorPar1L3 = 0;
  JetCorrectorParameters *JetCorPar1   = 0;
  vector<const char*> strVec;
  strVec.push_back(Form("%s_L1FastJet_%s.txt",cid1,a1));
  if(TString(payld1).Contains("Run1Sub") || TString(payld1).Contains("nsRunIISpring15")) strVec.push_back(Form("%s_RC_%s.txt",cid1,a1));
  strVec.push_back(Form("%s_L2Relative_%s.txt",cid1,a1));
  strVec.push_back(Form("%s_L3Absolute_%s.txt",cid1,a1));
  //strVec.push_back(Form("%s_L2L3Residual_%s.txt",cid1,a1));
  //strVec.push_back(Form("%s_Uncertainty_%s.txt",cid1,a1));

  for(unsigned int istr=0;istr<strVec.size(); istr++) {
     for(int count=0; count<maxTries; count++) {
        try {
           edm::FileInPath strFIP(paths[count]+strVec[istr]);
           strPath = strFIP.fullPath();
           break;
        }
        catch (edm::Exception ex) {
           if(count==maxTries-1) throw ex;
        }
     }
     if((type1=="MC" && istr!=3) || type1=="DATA")cout << strPath << endl << flush;
     if(istr==0)
        JetCorPar1L1 = new JetCorrectorParameters(strPath);
     else if(istr==1)
        JetCorPar1L2 = noL2L3_p1 ? 0 : new JetCorrectorParameters(strPath);
     else if(istr==2)
        JetCorPar1L3 = noL2L3_p1 ? 0 : new JetCorrectorParameters(strPath);
     else if(istr==3)
        JetCorPar1 = (type1 == "MC" || noL2L3_p1 ) ? 0 : new JetCorrectorParameters(strPath);
     else if(istr==4)
        jecUnc1 = TString(payld1).Contains("PHYS14") || noL2L3_p1 ? 0 : new JetCorrectionUncertainty(strPath);
  }

  // JEC2
  JetCorrectorParameters *JetCorPar2L1=0;
  JetCorrectorParameters *JetCorPar2L2=0;
  JetCorrectorParameters *JetCorPar2L3=0;
  JetCorrectorParameters *JetCorPar2  =0;
  JetCorrectionUncertainty  *jecUnc2  =0;
  if(payld2 != ""){
   bool noL2L3_p2 = TString(payld2).Contains("Run1Sub") || TString(payld2).Contains("nsRunIISpring15") || TString(payld2).Contains("RC") ;  

   strVec.erase(strVec.begin(),strVec.end());
   strVec.push_back(Form("%s_L1FastJet_%s.txt",cid2,a2));
   if(TString(payld2).Contains("Run1Sub") || TString(payld2).Contains("nsRunIISpring15")) strVec.push_back(Form("%s_RC_%s.txt",cid2,a2));
   strVec.push_back(Form("%s_L2Relative_%s.txt",cid2,a2));
   strVec.push_back(Form("%s_L3Absolute_%s.txt",cid2,a2));
   //strVec.push_back(Form("%s_L2L3Residual_%s.txt",cid2,a2));
   //strVec.push_back(Form("%s_Uncertainty_%s.txt",cid2,a2));

   for(unsigned int istr=0;istr<strVec.size(); istr++) {
     for(int count=0; count<maxTries; count++) {
        try {
           edm::FileInPath strFIP(paths[count]+strVec[istr]);
           strPath = strFIP.fullPath();
           break;
        }
        catch (edm::Exception ex) {
           if(count==maxTries-1) throw ex;
        }
     }
     if((type2=="MC" && istr!=3) || type2=="DATA")cout << strPath << endl << flush;
     if(istr==0)
        JetCorPar2L1 = new JetCorrectorParameters(strPath);
     else if(istr==1)
        JetCorPar2L2 = noL2L3_p2 ? 0 : new JetCorrectorParameters(strPath);
     else if(istr==2)
        JetCorPar2L3 = noL2L3_p2 ? 0 : new JetCorrectorParameters(strPath);
     else if(istr==3)
        JetCorPar2 = (type2 == "MC" || noL2L3_p2 ) ? 0 : new JetCorrectorParameters(strPath);
     else if(istr==4)
        jecUnc2 = TString(payld2).Contains("PHYS14") || noL2L3_p2 ? 0 : new JetCorrectionUncertainty(strPath);
   }
  }

  // JEC3
  JetCorrectorParameters *JetCorPar3L1=0;
  JetCorrectorParameters *JetCorPar3L2=0;
  JetCorrectorParameters *JetCorPar3L3=0;
  JetCorrectorParameters *JetCorPar3  =0;
  JetCorrectionUncertainty  *jecUnc3  =0;
  if(payld3 != ""){

    strVec.erase(strVec.begin(),strVec.end());
    strVec.push_back(Form("%s_L1FastJet_%s.txt",cid3,a3));
    if(TString(payld1).Contains("Run1Sub")|| TString(payld3).Contains("nsRunIISpring15")) strVec.push_back(Form("%s_RC_%s.txt",cid3,a3));
    strVec.push_back(Form("%s_L2Relative_%s.txt",cid3,a3));
    strVec.push_back(Form("%s_L3Absolute_%s.txt",cid3,a3));
    //strVec.push_back(Form("%s_L2L3Residual_%s.txt",cid3,a3));
    //strVec.push_back(Form("%s_Uncertainty_%s.txt",cid3,a3));

    for(unsigned int istr=0;istr<strVec.size(); istr++) {
     for(int count=0; count<maxTries; count++) {
        try {
           edm::FileInPath strFIP(paths[count]+strVec[istr]);
           strPath = strFIP.fullPath();
           break;
        }
        catch (edm::Exception ex) {
           if(count==maxTries-1) throw ex;
        }
     }
     if((type2=="MC" && istr!=3) || type2=="DATA")cout << strPath << endl << flush;
     if(istr==0)
        JetCorPar3L1 = new JetCorrectorParameters(strPath);      
     else if(istr==1)
        JetCorPar3L2 = TString(payld3).Contains("Run1Sub") ? 0 : new JetCorrectorParameters(strPath); 
     else if(istr==2)
        JetCorPar3L3 = TString(payld3).Contains("Run1Sub") ? 0 : new JetCorrectorParameters(strPath);   
     else if(istr==3)
        JetCorPar3 = (type3 == "MC" || TString(payld3).Contains("Run1Sub") ? 0 : new JetCorrectorParameters(strPath));
     else if(istr==4)
        jecUnc3 = TString(payld3).Contains("PHYS14") || TString(payld3).Contains("Run1Sub") ? 0 : new JetCorrectionUncertainty(strPath);
    }
  }

  // JEC1
  vector<JetCorrectorParameters> vParam1;
  if (l1)   vParam1.push_back(*JetCorPar1L1);
  if (l2l3) vParam1.push_back(*JetCorPar1L2);
  if (l2l3) vParam1.push_back(*JetCorPar1L3);
  if (res && type1 == "DATA")  vParam1.push_back(*JetCorPar1);
  FactorizedJetCorrector *JEC1 = new FactorizedJetCorrector(vParam1);

  // JEC2
  vector<JetCorrectorParameters> vParam2;
  FactorizedJetCorrector *JEC2 = 0 ; 
  if(payld2 != ""){
   if (l1)   vParam2.push_back(*JetCorPar2L1);
   if (l2l3) vParam2.push_back(*JetCorPar2L2);
   if (l2l3) vParam2.push_back(*JetCorPar2L3);
   if (res && type2 == "DATA")  vParam2.push_back(*JetCorPar2);
   JEC2 = new FactorizedJetCorrector(vParam2);
  } 
  // JEC3
  vector<JetCorrectorParameters> vParam3;    
  FactorizedJetCorrector *JEC3 = 0 ;
  if(payld3 != ""){
   if (l1)   vParam3.push_back(*JetCorPar3L1);
   if (l2l3) vParam3.push_back(*JetCorPar3L2);
   if (l2l3) vParam3.push_back(*JetCorPar3L3);
   if (res && type3 == "DATA")  vParam3.push_back(*JetCorPar3);
   JEC3 = new FactorizedJetCorrector(vParam3);
  }


// ===
  const char *cl1   = (l1 ? "L1" : "");
  const char *cl2l3 = (l2l3 ? "L2L3" : "");
  const char *cpl   = (res&&(l1||l2l3) ? "+" : "");
  const char *cplus = (res&&(l1||l2l3) ? "Plus" : "");
  const char *cres  = (res ? "L2L3res" : "");


// ... versus eta plots ...
  TH1D *h = new TH1D("h",";|#eta|; L2L3 residual", 50,0,5);
  if (_usenegative) h->GetXaxis()->SetTitle("-#eta");
  if (_usepositive) h->GetXaxis()->SetTitle("+#eta");
  h->GetYaxis()->SetTitle(Form("%s%s%s%s",cl1,cl2l3,cpl,cres));

// ... versus pt plots ...
  // Create suitable binning
  const double x_pt[] =
//    {1, 3, 5, 7, 9, 11, 12, 15, 18, 21, 24, 28, 32, 37, 43, 49, 56, 64, 74, 84,
//     97, 114, 133, 153, 174, 196, 220, 245, 272, 300, 362, 430,
//     507, 592, 686, 790, 905, 1032, 1172, 1327, 1497, 1684, 1890, //1999};
//     2000, 2238, 2500};//, 2787, 3103, 3450};

    {10, 12, 15, 18, 21, 24, 
     28, 32, 37, 43, 49, 56, 64, 74, 84,
     97, 114, 133, 153, 174, 196, 220, 245, 272, 300, 362, 430,
     507, 592, 686, 790, 905, 1032, 1172, 1327, 1497, 1684, 1890, //1999};
     2000, 2238, 2500, 2900, 3000, 3100,  3500, 4000, 4500, 5000, 5500, 6000, 6500};

  const int ndiv_pt = sizeof(x_pt)/sizeof(x_pt[0])-1;
  TH1D *hpt = new TH1D("hpt", Form(";p_{T,%s} (GeV); L2L3 residual",cgen),
                       ndiv_pt, x_pt);
  hpt->GetYaxis()->SetTitle(Form("%s%s%s%s",cl1,cl2l3,cpl,cres));
  TH1D *h10 = (TH1D*)hpt->Clone("h10");

// ...
  h->SetMinimum(0.3);
  h->SetMaximum(2.0);

  hpt->GetXaxis()->SetMoreLogLabels();
  hpt->GetXaxis()->SetNoExponent();
  hpt->SetMinimum(0.);
  hpt->SetMaximum(2.);

  if (_paper) {        
    if (l1 && !l2l3 && !res) h->SetYTitle("Pileup offset correction");
    if (!l1 && l2l3 && !res) h->SetYTitle("Simulated response correction");
    if (!l1 && !l2l3 && res) h->SetYTitle("Residual response correction");
    //
    if (l1 && !l2l3 && !res) hpt->SetYTitle("Pileup offset correction");
    if (!l1 && l2l3 && !res) hpt->SetYTitle("Simulated response correction");
    if (!l1 && !l2l3 && res) hpt->SetYTitle("Residual response correction");
    //
    if (l1 && !l2l3 && !res) h->GetYaxis()->SetRangeUser(0.5,1.4); 
    if (!l1 && l2l3 && !res) h->GetYaxis()->SetRangeUser(0.85,1.8); // 2.2

    if (!l1 && !l2l3 && res) h->GetYaxis()->SetRangeUser(0.85,1.4);

    if(_pt<20.){
      if (l1 && !l2l3 && !res) h->GetYaxis()->SetRangeUser(0.0,1.4);
      if (!l1 && l2l3 && !res) h->GetYaxis()->SetRangeUser(0.85,1.8); //5.0
      if (!l1 && !l2l3 && res) h->GetYaxis()->SetRangeUser(0.85,1.4);
    }

    //
    if (l1 && !l2l3 && !res) hpt->GetYaxis()->SetRangeUser(0.5,1.4);
    if (!l1 && l2l3 && !res) hpt->GetYaxis()->SetRangeUser(0.85,2.2); // 1.4
 
    if (!l1 && !l2l3 && res) hpt->GetYaxis()->SetRangeUser(0.85,1.4);

    if(_pt<20.){
      if (l1 && !l2l3 && !res) hpt->GetYaxis()->SetRangeUser(0.0,1.4);
      if (!l1 && l2l3 && !res) hpt->GetYaxis()->SetRangeUser(0.85,1.8); //5.0
      if (!l1 && !l2l3 && res) hpt->GetYaxis()->SetRangeUser(0.85,1.4);
    }

  }

  // a: pT=30 GeV,    b: pT=100 GeV,   c: pT=1000 GeV,  d: vs. pT
  TH1D *h1a  = (TH1D*)h->Clone("h1a");
  TH1D *h1ar = (TH1D*)h->Clone("h1ar");
  TCanvas *c1a  = tdrCanvas("c1a", h1a, 0,11,kSquare);
  TCanvas *c1d  = tdrCanvas("c1d", hpt, 0,11,kSquare);
  TCanvas *c1ar = tdrCanvas("c1ar",h1ar,0,11,kSquare);
  TCanvas *c10  = tdrCanvas("c10", hpt, 0,11,kSquare);

  TGraph *g1a = new TGraph(0);
  TGraph *g1d = new TGraph(0);
  //
  TGraph *g2a = new TGraph(0);
  TGraph *g2d = new TGraph(0);
  //
  TGraph *g3a = new TGraph(0);
  TGraph *g3d = new TGraph(0);

  TGraphErrors *g1a_e = new TGraphErrors(0);
  TGraphErrors *g1d_e = new TGraphErrors(0);
  TGraph *g1a_pl = new TGraph(0);
  TGraph *g1a_mn = new TGraph(0);
  TGraph *g1d_pl = new TGraph(0);
  TGraph *g1d_mn = new TGraph(0);

  TGraphErrors *g2a_e = new TGraphErrors(0);
  TGraphErrors *g2d_e = new TGraphErrors(0);
  TGraph *g2a_pl = new TGraph(0);
  TGraph *g2a_mn = new TGraph(0);
  TGraph *g2d_pl = new TGraph(0);
  TGraph *g2d_mn = new TGraph(0);
    
  TGraphErrors *g3a_e = new TGraphErrors(0);
  TGraphErrors *g3d_e = new TGraphErrors(0);
  TGraph *g3a_pl = new TGraph(0);
  TGraph *g3a_mn = new TGraph(0);
  TGraph *g3d_pl = new TGraph(0);
  TGraph *g3d_mn = new TGraph(0);

    double ptbins[] = {15., 30, 100, 1000, 4000};
//  double ptbins[] = {1000, 2000, 3000, 3500, 4000};

  const int npt = sizeof(ptbins)/sizeof(ptbins[0]); 
  TGraphErrors *g21s[npt];

  TMultiGraph *mg = new TMultiGraph();
  TLegend *leg10 = tdrLeg(0.50, 0.56, 0.90, 0.82);

  for (int i = 0; i != npt; ++i) {
    g21s[i] = new TGraphErrors(0);
  }

  for (int i = 1; i != h->GetNbinsX()+1; ++i) {  // loop over eta bins
    double eta = h->GetBinCenter(i);
    if (fabs(eta)>4.7) continue;

    if(JEC2){
    // ***** Various pt bins, versus eta, 2/1 ratios *****
      for (int j = 0; j != npt; ++j) {
        TGraphErrors *g21 = g21s[j];
        double pt = ptbins[j];
        double energy = pt*cosh(eta); 
          if (energy < 6500.) {
//        if (energy < 3000.) {

         _alg  = algo1;
         _payld=payld1;
          if(i==1 && j==0) cout << _payld << ":  mu and rho " << _mu  << " " << getRho(_mu) << endl ;
          double y1 = 0.5*(getEtaPtE(JEC1, +eta, pt, energy)
                         + getEtaPtE(JEC1, -eta, pt, energy));
         _alg  = algo2;
         _payld=payld2;
          if(i==1 && j==0) cout << _payld << ":  mu and rho " << _mu  << " " << getRho(_mu) << endl ; 
          double y2 = 0.5*(getEtaPtE(JEC2, +eta, pt, energy)
                         + getEtaPtE(JEC2, -eta, pt, energy));

          g21->SetPoint(g21->GetN(), eta, y2/y1);
        } // energy < 6500
      } // for j
    } 

    // ***** Fixed pT, versus eta ******
    { 
      double pt = _pt ;
      double energy = pt*cosh(eta);   
        
      if (energy < 6500.) {
//      if (energy < 3000.) {

        // Asymmetric corrections now
        _alg  = algo1;
        _payld=payld1;
        double y1 = 0.5*(getEtaPtE(JEC1, +eta, pt, energy)
                       + getEtaPtE(JEC1, -eta, pt, energy));
        //  cout << "  == 1 " << y1 << endl ;
        double y2(0);
        _alg  = algo2;
        _payld=payld2;
        if(JEC2) y2 = 0.5*(getEtaPtE(JEC2, +eta, pt, energy)
                         + getEtaPtE(JEC2, -eta, pt, energy));
        //  cout << "  == 2 " << y2 << endl ;



        double y3(0);
        _alg  = algo3;
        _payld=payld3;
        if(JEC3) y3 = 0.5*(getEtaPtE(JEC3, +eta, pt, energy)
                         + getEtaPtE(JEC3, -eta, pt, energy));
        // cout << "  == 3 " << y3 << endl ;

        if (_usenegative) { // negative side
          _alg = algo1;
         _payld=payld1;
          y1 = getEtaPtE(JEC1, -eta, pt, energy);
          _alg = algo2;
         _payld=payld2;
          y2 = JEC2 ? getEtaPtE(JEC2, -eta, pt, energy) : 0;
          _alg = algo3;
         _payld=payld3;
          y3 = JEC3 ? getEtaPtE(JEC3, -eta, pt, energy) : 0;
        }

        if (_usepositive) { // positive side
          _alg = algo1;
         _payld=payld1;
          y1 = getEtaPtE(JEC1, +eta, pt, energy);
          _alg = algo2;
         _payld=payld2;
          y2 = JEC2 ? getEtaPtE(JEC2, +eta, pt, energy) : 0;
          _alg = algo3;
         _payld=payld3;
          y3 = JEC3 ? getEtaPtE(JEC3, +eta, pt, energy) : 0; 
        }

        double e1 = jecUnc1 ? getEtaPtUncert(jecUnc1, JEC1, eta, pt) : 0;
        double e2 = jecUnc2 ? getEtaPtUncert(jecUnc2, JEC2, eta, pt) : 0;
        double e3 = jecUnc3 ? getEtaPtUncert(jecUnc3, JEC3, eta, pt) : 0;

        g1a->SetPoint(g1a->GetN(), eta, y1);
        g2a->SetPoint(g2a->GetN(), eta, y2);
        g3a->SetPoint(g3a->GetN(), eta, y3);
        g1a_pl->SetPoint(g1a_pl->GetN(), eta, y1*(1+e1));
        g1a_mn->SetPoint(g1a_mn->GetN(), eta, y1*(1-e1));
        g1a_e->SetPoint(i-1, eta, y1);
        g1a_e->SetPointError(i-1, 0., y1*e1);
        //
        g2a_pl->SetPoint(g2a_pl->GetN(), eta, y2*(1+e2));
        g2a_mn->SetPoint(g2a_mn->GetN(), eta, y2*(1-e2));
        g2a_e->SetPoint(i-1, eta, y2);
        g2a_e->SetPointError(i-1, 0., y2*e2);
        //
        g3a_pl->SetPoint(g3a_pl->GetN(), eta, y3*(1+e3));
        g3a_mn->SetPoint(g3a_mn->GetN(), eta, y3*(1-e3));
        g3a_e->SetPoint(i-1, eta, y3);
        g3a_e->SetPointError(i-1, 0., y3*e3);
      }
    }
  }

//  ......

  for (int ieta = -49; ieta<=49; ieta++){ // Loop over eta bins: 
                                          // Only one eta value in the main plot
                                          // Many eta curves for the _validation_ plot
    double eta = ieta*0.1 ; 
    TGraph *gtmp = new TGraph(0);

    for (int i = 1; i != hpt->GetNbinsX()+1; ++i) { // Loop over pt-bins
        
      double pt = hpt->GetBinCenter(i);
      double energy = pt*cosh(eta);
        
      if (pt>0. && energy < 6500.) {
//      if (pt>0. && energy < 3000.) {

       
        // Asymmetric corrections now
        _alg  = algo1;
        _payld=payld1;
        double y1 = 0.5*(getEtaPtE(JEC1, +eta, pt, energy)
                       + getEtaPtE(JEC1, -eta, pt, energy));  
        double y2(0);
        _alg  = algo2;
        _payld=payld2;
        if(JEC2) y2 = 0.5*(getEtaPtE(JEC2, +eta, pt, energy)
                         + getEtaPtE(JEC2, -eta, pt, energy));

        double y3(0);
        _alg  = algo3;
        _payld=payld3;
        if(JEC3) y3 = 0.5*(getEtaPtE(JEC3, +eta, pt, energy)
                         + getEtaPtE(JEC3, -eta, pt, energy));

        // negative side
        if (_usenegative) {
          _alg = algo1;
         _payld=payld1;
          y1 = getEtaPtE(JEC1, -eta, pt, energy);
          _alg = algo2;
         _payld=payld2;
          if(JEC2) y2 = getEtaPtE(JEC2, -eta, pt, energy);
          _alg = algo3;
         _payld=payld3;
          if(JEC3) y3 = getEtaPtE(JEC3, -eta, pt, energy);
        }
        // positive side
        if (_usepositive) {
          _alg = algo1;
         _payld=payld1;
          y1 = getEtaPtE(JEC1, +eta, pt, energy);
          _alg = algo2;
         _payld=payld2;
          if(JEC2) y2 = getEtaPtE(JEC2, +eta, pt, energy);
          _alg = algo3;
         _payld=payld3;
          if(JEC3) y3 = getEtaPtE(JEC3, +eta, pt, energy);
        }

        double e1 = jecUnc1 ? getEtaPtUncert(jecUnc1, JEC1, eta, pt) : 0;
        double e2 = jecUnc2 ? getEtaPtUncert(jecUnc2, JEC2, eta, pt) : 0;
//        double e3 = jecUnc3 ? getEtaPtUncert(jecUnc3, JEC3, eta, pt) : 0;
        if(ieta == _ieta){  
//         cout << " pt y1 y2 " << pt << "     "   << y1 << "  " << y2 << endl ;
         g1d->SetPoint(g1d->GetN(), pt, y1);
         g2d->SetPoint(g2d->GetN(), pt, y2);
         g3d->SetPoint(g3d->GetN(), pt, y3);
         //
         g1d_pl->SetPoint(g1d_pl->GetN(), pt, y1*(1+e1));
         g1d_mn->SetPoint(g1d_mn->GetN(), pt, y1*(1-e1));
         g1d_e->SetPoint(i-1, pt, y1);
         g1d_e->SetPointError(i-1, 0., y1*e1);
         //
         g2d_pl->SetPoint(g2d_pl->GetN(), pt, y2*(1+e2));
         g2d_mn->SetPoint(g2d_mn->GetN(), pt, y2*(1-e2));
         g2d_e->SetPoint(i-1, pt, y2);
         g2d_e->SetPointError(i-1, 0., y2*e2);
        }
        gtmp->SetPoint(gtmp->GetN(), pt, y1);
        if(l1 && !l2l3 && ! res) {
          if(y1>1.) {
           cout << endl ;  
               cout << " L1: pt eta cor " << std::setw(7) << pt << "  " << std::setw(7) << eta << "     " << y1  ;
           if(y1 > 1.1)
           cout << " < ============ " ; 
           cout << endl ;
          }
        }
      }
    } // END: loop over pt bins
    gtmp->SetLineWidth(2);
//    gtmp->SetLineStyle( ieta>=0 ? 2 : 1); 
    gtmp->SetLineColor(abs(ieta)/10+1);
    if(ieta%10 ==0 && ieta>=0 && ieta<50) 
    leg10->AddEntry(gtmp,Form("%2.0f<|#eta|<%2.0f",0.1*abs(ieta),0.1*abs(ieta+10)),"l");
    mg->Add(gtmp,"l");
  } // END: loop over eta bins


// .......

  g1a->SetFillStyle(3003);
  g1a->SetFillColor(kBlue);
  g1d->SetFillStyle(3003);
  g1d->SetFillColor(kBlue);

  g2a->SetFillStyle(3003);
  g2a->SetFillColor(kRed);
  g2d->SetFillStyle(3003);
  g2d->SetFillColor(kRed);

  g3a->SetFillStyle(3003);
  g3a->SetFillColor(kGreen+2);
  g3d->SetFillStyle(3003);
  g3d->SetFillColor(kGreen+2);

  c1a->cd();

  g1a_e->SetFillStyle(3003);
  g1a_e->SetFillColor(kBlue);
  g1a_e->Draw("SAME E3");
  g1a_pl->SetLineColor(kBlue-9);
  g1a_pl->SetLineStyle(kSolid);//kDotted);
  g1a_pl->Draw("SAMEL");
  g1a_mn->SetLineColor(kBlue-9);
  g1a_mn->SetLineStyle(kSolid);//kDotted);
  g1a_mn->Draw("SAMEL");

  if(JEC2){
   g2a_e->SetFillStyle(3003);
   g2a_e->SetFillColor(kRed);
   g2a_e->Draw("SAME E3");
   g2a_pl->SetLineColor(kRed-9);
   g2a_pl->SetLineStyle(kSolid);//kDotted);
   g2a_pl->Draw("SAMEL");
   g2a_mn->SetLineColor(kRed-9);
   g2a_mn->SetLineStyle(kSolid);//kDotted);
   g2a_mn->Draw("SAMEL"); 
  }

  if(JEC3){
   g3a_e->SetFillStyle(3003);    
   g3a_e->SetFillColor(kGreen+2);
   g3a_e->Draw("SAME E3");
   g3a_pl->SetLineColor(kGreen-9);
   g3a_pl->SetLineStyle(kSolid);//kDotted);
   g3a_pl->Draw("SAMEL");  
   g3a_mn->SetLineColor(kGreen-9);
   g3a_mn->SetLineStyle(kSolid);//kDotted);
   g3a_mn->Draw("SAMEL");
  }                  

  g1a->SetMarkerStyle(kFullSquare);
  g1a->SetMarkerColor(kBlue);
  g1a->SetLineColor(kBlue);
  g1a->Draw("SAMEPL");

  if(JEC2){
   g2a->SetMarkerStyle(kFullCircle);
   g2a->SetMarkerColor(kRed);
   g2a->SetLineColor(kRed);
   g2a->Draw("SAMEPL");
  }

  if (JEC3){
   g3a->SetMarkerStyle(kOpenSquare);
   g3a->SetMarkerColor(kGreen+2);   
   g3a->SetLineColor(kGreen+2);
   g3a->Draw("SAMEPL");
  }

  TLatex *tex = new TLatex();
  tex->SetNDC();
  tex->SetTextSize(0.045);   
  tex->DrawLatex(0.19,0.75,Form("p_{T,%s} = %2.0f GeV",cgen,_pt));
  if (l1) tex->DrawLatex(0.19,0.68,Form("#LT#mu#GT = %1.1f",_mu));

  TLegend *leg1a ;
  string cone1 = algo1.substr(2, 1);
  string pf1   = algo1.substr(3, 2);
  string chs1  = algo1.substr(5, 3);
//  chs1 = chs1=="chs" ? "+CHS" : "";
  if(chs1=="chs") chs1 = "+CHS" ;
  if(chs1=="Pup") chs1 = "+Puppi" ;
  if(pf1=="Ca")   {pf1=""; chs1 = "Calo" ;}

  string cone2 = algo2.substr(2, 1);
  string pf2   = algo2.substr(3, 2);
  string chs2  = algo2.substr(5, 3);
//  chs2 = chs2=="chs" ? "+CHS" : "";
  if(chs2=="chs") chs2 = "+CHS" ;
  if(chs2=="Pup") chs2 = "+Puppi" ;
  if(pf2=="Ca")   {pf2=""; chs2 = "Calo" ;}
  
   
  string cone3 = algo3.substr(2, 1);
  string pf3   = algo3.substr(3, 2);
  string chs3  = algo3.substr(5, 3);
//  chs3 = chs3=="chs" ? "+CHS" : "";
  if(chs3=="chs") chs3 = "+CHS" ;
  if(chs3=="Pup") chs3 = "+Puppi" ;
  if(pf3=="Ca")   {pf3=""; chs3 = "Calo" ;}



  string sheader="";
  string sheader_pdf="";
  string slg1="" ;
  string slg2="" ;
  string slg3="" ;


  if(!JEC2 && !JEC3){
    leg1a = tdrLeg(0.45, 0.66, 0.90, 0.82);
    leg1a->AddEntry(g1a,Form("R = 0.%s, %s%s",  cone1.c_str(), pf1.c_str(), chs1.c_str()),"");
    leg1a->AddEntry(g1a,Form("%s, %s",payld1.c_str(), type1.c_str()),"");

    sheader_pdf = "_"+payld1+"_AK"+cone1+pf1+chs1+"_"+type1;
  } else {
    leg1a = tdrLeg(0.51, 0.66, 0.90, 0.90);
    if( (JEC2 && payld2 != payld1) || (JEC3 && payld3 !=payld1) ){
      slg1 = payld1 ;
      slg2 = payld2 ;
      slg3 = payld3 ;
     } else {
      sheader     =payld1 ;
      sheader_pdf ="_"+payld1; 
     }
     if( (JEC2 && cone2 != cone1) || (JEC3 && cone3 !=cone1) ){
       if(slg1 != "") slg1 +=",  ";
       if(slg2 != "") slg2 +=",  ";
       if(slg3 != "") slg3 +=",  ";
       slg1 += "R = 0." + cone1 ;
       slg2 += "R = 0." + cone2 ; 
       slg3 += "R = 0." + cone3 ; 
      } else {
       if(sheader != "") sheader +=",  ";
       sheader  += "R = 0." + cone1 ;
       sheader_pdf += "_AK" + cone1 ;
      }
      if( (JEC2 && chs2 != chs1) || (JEC3 && chs3 !=chs1) ){
       if(slg1 != "") slg1 +=",  ";
       if(slg2 != "") slg2 +=",  ";
       if(slg3 != "") slg3 +=",  ";
       slg1 += pf1 + chs1 ;  
       slg2 += pf2 + chs2 ;
       slg3 += pf3 + chs3 ;
      } else {
       if(sheader != "") sheader +=",  ";
       sheader     +=  "PF" + chs1 ;
       sheader_pdf += "_PF" + chs1 ;
      }
      if( (JEC2 && type2 != type1) || (JEC3 && type3 !=type1) ){
       if(slg1 != "") slg1 +=",  ";
       if(slg2 != "") slg2 +=",  ";
       if(slg3 != "") slg3 +=",  ";
       slg1 += type1 ;
       slg2 += type2 ;
       slg3 += type3 ;
      } else {
       if(sheader != "") sheader +=",  ";
       sheader += type1 ;
       sheader_pdf += "_" + type1 ;
      }
      cout << "string size " << sheader.size() << "  " << slg1.size() << " " << slg2.size() << endl ; 

      if(sheader.size()>25 || slg1.size()>20 || slg2.size()>20 || slg3.size()>20 ) leg1a->SetTextSize(0.031);
      leg1a->SetTextSize(0.029);
      if(sheader !="") leg1a->SetHeader(sheader.c_str());
      if(slg1 !="") leg1a->AddEntry(g1a,slg1.c_str(),"LPF");
      if(slg2 !="" && JEC2) leg1a->AddEntry(g2a,slg2.c_str(),"LPF");
      if(slg3 !="" && JEC3) leg1a->AddEntry(g3a,slg3.c_str(),"LPF");
/////      leg1a->AddEntry(g1a,"2015","LPF");
/////      leg1a->AddEntry(g2a,"2016","LPF");

    }
   
  gPad->RedrawAxis();


  // ***** Eta = 0
  {
    c1d->cd();
    c1d->SetLogx();

    g1d_e->SetFillStyle(3003);
    g1d_e->SetFillColor(kBlue);
    g1d_e->Draw("SAME E3");
    g1d_pl->SetLineColor(kBlue-9);
    g1d_pl->SetLineStyle(kSolid);
    g1d_pl->Draw("SAMEL");
    g1d_mn->SetLineColor(kBlue-9);   
    g1d_mn->SetLineStyle(kSolid);
    g1d_mn->Draw("SAMEL");
      
    if (JEC2) {
     g2d_e->SetFillStyle(3003);   
     g2d_e->SetFillColor(kRed);
     g2d_e->Draw("SAME E3");
     g2d_pl->SetLineColor(kRed-9);
     g2d_pl->SetLineStyle(kSolid);
     g2d_pl->Draw("SAMEL");
     g2d_mn->SetLineColor(kRed-9);
     g2d_mn->SetLineStyle(kSolid);
     g2d_mn->Draw("SAMEL"); 
    }

    if (JEC3) {
     g3d_e->SetFillStyle(3003);
     g3d_e->SetFillColor(kGreen+2);
     g3d_e->Draw("SAME E3");
     g3d_pl->SetLineColor(kGreen-9);  
     g3d_pl->SetLineStyle(kSolid); 
     g3d_pl->Draw("SAMEL");
     g3d_mn->SetLineColor(kGreen-9);
     g3d_mn->SetLineStyle(kSolid);
     g3d_mn->Draw("SAMEL");
    }

    g1d->SetMarkerStyle(kFullSquare);
    g1d->SetMarkerColor(kBlue);
    g1d->SetLineColor(kBlue);
    g1d->Draw("SAMEPL");
    
    if (JEC2) {
     g2d->SetMarkerStyle(kFullCircle);
     g2d->SetMarkerColor(kRed);   
     g2d->SetLineColor(kRed);
     g2d->Draw("SAMEPL");
    }

    if (JEC3) {
     g3d->SetMarkerStyle(kOpenSquare);
     g3d->SetMarkerColor(kGreen+2); 
     g3d->SetLineColor(kGreen+2);
     g3d->Draw("SAMEPL");
    } 


//    tex->DrawLatex(0.19,0.75,"|#eta| = 0");
    tex->DrawLatex(0.19,0.75,Form("|#eta| = %1.1f",_ieta*0.1));
    if (l1) tex->DrawLatex(0.19,0.68,Form("#LT#mu#GT = %1.1f",_mu));

    leg1a->Draw();
    gPad->RedrawAxis();
  }


  string ctype = string(cl1)+string(cl2l3)+string(cplus)+string(cres);
  const char *cs = ctype.c_str();
  if (_pdf) {
    c1a->SaveAs(Form("pdf/compareJECversions_%s%s_Pt%2.0f_%s.pdf",cs,sheader_pdf.c_str(),_pt,cgen));
    c1d->SaveAs(Form("pdf/compareJECversions_%s%s_Eta%d_%s.pdf",  cs,sheader_pdf.c_str(),_ieta,cgen));
  }
  if (_C) {
  }

  // ***** Multiple pT bins for ratio only
  if(JEC2){
    int colors[] = {kBlack, kBlue, kCyan+2, kGreen+2, kOrange+2, kRed};
    int styles[] = {kSolid, kDashed, kDotted, kDashDotted, kDashed, kSolid};
    c1ar->cd();
    h1ar->SetMinimum(0.85);//0.7);
    h1ar->SetMaximum(1.55);
    h1ar->GetYaxis()->SetTitle(Form("%s%s%s%s (%s / %s)",cl1,cl2l3,cpl,cres,
                                   slg2.c_str(), slg1.c_str()));
////    h1ar->GetYaxis()->SetTitle(Form("%s%s%s%s (80X / 76X)",cl1,cl2l3,cpl,cres ));              // this is tmp 

    h1ar->GetYaxis()->SetTitleSize(0.045);
    h1ar->GetYaxis()->SetTitleOffset(1.7);   
    

    TLine *l = new TLine();
    l->SetLineStyle(kDashed);
    l->DrawLine(0,1,5,1);
    l->DrawLine(0,1.05,5,1.05);
    l->SetLineStyle(kDotted);
    l->DrawLine(0,1.02,5,1.02);
    l->DrawLine(0,0.98,5,0.98);

    if (l1) tex->DrawLatex(0.19,0.68,Form("#LT#mu#GT = %1.1f",_mu));  

    
    TLegend *leg = tdrLeg(0.46, 0.57, 0.90, 0.88);
    if(sheader.size()>25) leg->SetTextSize(0.031);

    for (int i = 0; i != npt; ++i) {
      TGraphErrors *gr = g21s[i];
      gr->SetLineColor(colors[i]);//i+1);
      gr->SetLineStyle(styles[i]);//i+1);
      gr->SetLineWidth(3);
      gr->Draw("SAME L");
    
      leg->SetHeader(sheader.c_str());
      leg->AddEntry(gr,Form("p_{T,%s} = %1.0f GeV",
                            cgen,ptbins[i]),"L");
    } // for i

   gPad->RedrawAxis();
   c1ar->SaveAs(Form("pdf/compareJECversions_%s%s_2over1_%s.pdf",cs,sheader_pdf.c_str(),cgen));

  }                               

  {
    c10->cd();
    c10->SetLogx(); c10->SetGridx(); c10->SetGridy();
    h10->SetMinimum(0.7);//0.85);
    h10->SetMaximum(1.4);
    h10->GetYaxis()->SetTitle(Form("%s%s%s%s",cl1,cl2l3,cpl,cres));

    mg->Draw("AL");
    mg->GetXaxis()->SetTitle(Form("p_{T,%s} (GeV)",cgen));
    mg->GetYaxis()->SetTitle(Form("%s%s%s%s",cl1,cl2l3,cpl,cres));
    mg->GetXaxis()->SetMoreLogLabels();
    mg->GetXaxis()->SetNoExponent();

   // Change the axis limits
    gPad->Modified();
    mg->GetXaxis()->SetLimits(3.,3000.);
    mg->SetMinimum(0.);
    mg->SetMaximum(5.);
    if (l1 && !l2l3 && !res ) mg->SetMaximum(2.);

    tex->SetTextSize(0.040);
    tex->DrawLatex(0.19,0.89,
    Form("%s  %s   R = 0.%s, %s%s", payld1.c_str(), type1.c_str(),  cone1.c_str(), pf1.c_str(), chs1.c_str()));    

    if (l1) tex->DrawLatex(0.19,0.68,Form("#LT#mu#GT = %1.1f",_mu));

    leg10->Draw();



//    gPad->RedrawAxis();
    c10->Update();
    c10->SaveAs(Form("pdf/%s_%s_%s_validation_%s.pdf", payld1.c_str(), type1.c_str(), cs, cgen));

  }

} // compareJEC


////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////
//______________________________________________________________________________
int main(int argc,char**argv)
{
  compareJEC("Summer16_25nsV5", "Summer16_25nsV4", "Spring16_25nsV6", "AK4PFchs", "AK4PFchs", "AK4PFchs", "MC","MC","MC",    false, true,  false);
  compareJEC("Summer16_25nsV5", "Summer16_25nsV4", "Spring16_25nsV6", "AK4PFchs", "AK4PFchs", "AK4PFchs", "MC","MC","MC",    true, false,  false);

  compareJEC("Summer16_25nsV5", "Summer16_25nsV4", "Spring16_25nsV6", "AK8PFchs", "AK8PFchs", "AK8PFchs", "MC","MC","MC",    false, true,  false);
  compareJEC("Summer16_25nsV5", "Summer16_25nsV4", "Spring16_25nsV6", "AK8PFchs", "AK8PFchs", "AK8PFchs", "MC","MC","MC",    true, false,  false);

  compareJEC("Summer16_25nsV5", "Summer16_25nsV4", "Spring16_25nsV6", "AK4PFPuppi", "AK4PFPuppi", "AK4PFPuppi", "MC","MC","MC",    false, true,  false);
  compareJEC("Summer16_25nsV5", "Summer16_25nsV4", "Spring16_25nsV6", "AK4PFPuppi", "AK4PFPuppi", "AK4PFPuppi", "MC","MC","MC",    true, false,  false);

  compareJEC("Summer16_25nsV5", "Summer16_25nsV4", "Spring16_25nsV6", "AK8PFPuppi", "AK8PFPuppi", "AK8PFPuppi", "MC","MC","MC",    false, true,  false);
  compareJEC("Summer16_25nsV5", "Summer16_25nsV4", "Spring16_25nsV6", "AK8PFPuppi", "AK8PFPuppi", "AK8PFPuppi", "MC","MC","MC",    true, false,  false);
}


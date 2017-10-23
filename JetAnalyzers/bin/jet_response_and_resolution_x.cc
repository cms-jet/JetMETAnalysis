////////////////////////////////////////////////////////////////////////////////
//
// jet_response_and_resolution_x
// -----------------------------
//
//            07/21/2009 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/ObjectLoader.h"

#include "JERWriter.h"

#include <TROOT.h>
#include <TApplication.h>
#include <TFile.h>
#include <TKey.h>
#include <TH1F.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TVirtualFitter.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <cmath>
#include <cassert>

using namespace std;

/// set histo range to correspond to a specific fraction of events
void set_range_truncatedRMS(TH1* hist,float frac);

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv)
{
  //
  // evaluate command-line / configuration file options
  // 
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;

  string         input         = cl.getValue<string> ("input");
  string         output        = cl.getValue<string> ("output",               "");
  bool           dorelrsp      = cl.getValue<bool>   ("dorelrsp",           true);
  bool           doabsrsp      = cl.getValue<bool>   ("doabsrsp",          false);
  bool           doetarsp      = cl.getValue<bool>   ("doetarsp",          false);
  bool           dophirsp      = cl.getValue<bool>   ("dophirsp",          false);
  bool           domu          = cl.getValue<bool>   ("domu",              false);
  bool           dorho         = cl.getValue<bool>   ("dorho",             false);
  bool           dopudensity   = cl.getValue<bool>   ("dopudensity",       false);
  vector<string> flavors       = cl.getVector<string>("flavors",              "");
  vector<string> algs          = cl.getVector<string>("algs",                 "");
  bool           fitres        = cl.getValue<bool>   ("fitres",             true);
  bool           unweighted    = cl.getValue<bool>   ("unweighted",        false);
  bool           verbose       = cl.getValue<bool>   ("verbose",           false);
  bool           forcefit      = cl.getValue<bool>   ("forcefit",          false);
  int            minentries    = cl.getValue<int>    ("minentries",           -1);
  bool           addminerr     = cl.getValue<bool>   ("addminerr",         false);
  bool           docbfits      = cl.getValue<bool>   ("docbfits",          false);
  bool           dowrite       = cl.getValue<bool>   ("dowrite",           false);
  string         fera          = cl.getValue<string> ("fera",                 "");
  string         fprefix       = cl.getValue<string> ("fprefix",              "");
  bool           writeHeader   = cl.getValue<bool>   ("writeHeader",       false);

  float          fractionRMS   = cl.getValue<float> ("fractionRMS",           1.);
  float          fracRMSpf     = cl.getValue<float> ("fracRMSpf",             1.);
  float          fracRMSjpt    = cl.getValue<float> ("fracRMSjpt",            1.);
  float          fracRMScalo   = cl.getValue<float> ("fracRMScalo",           1.);

  float          calomin       = cl.getValue<float>  ("calomin",             -1.);
  float          jptmin        = cl.getValue<float>  ("jptmin",              -1.);
  float          pfmin         = cl.getValue<float>  ("pfmin",               -1.);
  float          puppimin      = cl.getValue<float>  ("puppimin",            -1.);

  float          calofitmin    = cl.getValue<float>  ("calofitmin",          -1.);
  float          jptfitmin     = cl.getValue<float>  ("jptfitmin",           -1.);
  float          pffitmin      = cl.getValue<float>  ("pffitmin",            -1.);
  float          pfchsfitmin   = cl.getValue<float>  ("pfchsfitmin",         -1.);
  float          puppifitmin   = cl.getValue<float>  ("puppifitmin",         -1.);

  float          calofitmax    = cl.getValue<float>  ("calofitmax",          -1.);
  float          jptfitmax     = cl.getValue<float>  ("jptfitmax",           -1.);
  float          pffitmax      = cl.getValue<float>  ("pffitmax",            -1.);
  float          puppifitmax   = cl.getValue<float>  ("puppifitmax",         -1.);

  bool           semifitted    = cl.getValue<bool>  ("semifitted",         false);
  float          addUnc        = cl.getValue<float> ("addUnc",               0.0);
  vector<float>  fixCTerm      = cl.getVector<float>("fixCTerm",       "-9999.0");
  int            nfititer      = cl.getValue<int>   ("nfititer",              10);
  string         xvarOverride  = cl.getValue<string>("xvarOverride",          "");
  string         indirectProf  = cl.getValue<string>("indirectProf",          "");

  if (!cl.check()) return 0;
  cl.print();
  
  //
  // The stochastic ([1]*[1]/x) and constant ([2]*[2]) terms are effective descriptions
  //  that work over a limited energy range only. Tweaked to be more like the 2010 JER
  //  description and made the stochastic exponent a parameter.
  //
  //const string s_sigma_calo="sqrt([0]*[0]/(x*x) + [1]*[1]/x + [2]*[2])";
  //const string s_sigma="sqrt([0]*abs([0])/(x*x) + [1]*[1]/x + [2]*[2])";
  const string s_sigma="sqrt([0]*abs([0])/(x*x) + [1]*[1]*pow(x,[3]) + [2]*[2])";
  const string s_sigma_calo="sqrt([0]*[0]/(x*x) + [1]*[1]*pow(x,[3]) + [2]*[2])";
  const string s_sigma_angle="[0]+[1]*exp(-x/[2])";
  const string s_sigma_density="[0]+([1]*x)";
  //const string s_sigma="sqrt(((TMath::Sign(1,[0])*sq([0]/x))+(sq([1])*(x^([3]-1))))+sq([2]))";
  const string s_aone ="[0]";
  const string s_atwo ="[0]*x**[1]";
  const string s_pone ="TMath::Max(0.0,([0]-[3])/(1.+exp([1]*(x-[2])))+[3])";
  const string s_ptwo ="TMath::Max(0.0,([0]-[3])/(1.+exp([1]*(x-[2])))+[3])";

  
  //
  // construct variables vector
  //
  vector<string> variables;
  if (dorelrsp) {
    if (domu) {
        variables.push_back("RelRsp:JetEta:Mu:RefPt");
        variables.push_back("RelRsp:JetEta@0:Mu#2:RefPt");
        variables.push_back("RelRsp:JetEta@1.3:Mu#2:RefPt");
        variables.push_back("RelRsp:JetEta@2.5:Mu#2:RefPt");
        variables.push_back("RelRsp:JetEta@3:Mu#2:RefPt");
    }
    else if (dorho) {
       variables.push_back("RelRsp:JetEta:Rho:RefPt");
       variables.push_back("RelRsp:JetEta@0:Rho#2:RefPt");
       variables.push_back("RelRsp:JetEta@1.3:Rho#2:RefPt");
       variables.push_back("RelRsp:JetEta@2.5:Rho#2:RefPt");
       variables.push_back("RelRsp:JetEta@3:Rho#2:RefPt");
    }
    else if (dopudensity) {
       variables.push_back("RelRsp:JetEta:RhoPU:RefPt");
       variables.push_back("RelRsp:JetEta@0:RhoPU#2:RefPt");
       variables.push_back("RelRsp:JetEta@1.3:RhoPU#2:RefPt");
       variables.push_back("RelRsp:JetEta@2.5:RhoPU#2:RefPt");
       variables.push_back("RelRsp:JetEta@3:RhoPU#2:RefPt");
    }
    else{
      variables.push_back("RelRsp:RefPt");
      variables.push_back("RelRsp:JetEta");
      variables.push_back("RelRsp:JetPhi");
      //variables.push_back("RelRsp:JetY");
      variables.push_back("RelRsp:JetEta:RefPt");
      variables.push_back("RelRsp:JetEta#1:RefPt");  //fitmin and min options affect JetEta#1 (vs. JetEta), seems to try to fit res but not rsp
      //variables.push_back("RelRsp:JetY:RefPt");
      //variables.push_back("RelRsp:JetY#1:RefPt");
      //variables.push_back("RelRsp_Barrel:RefPt");
      //variables.push_back("RelRsp_InnerEndcap:RefPt");
      //variables.push_back("RelRsp_OuterEndcap:RefPt");
      //variables.push_back("RelRsp_Forward:RefPt");
    }
  }
  if (doabsrsp) {
    variables.push_back("AbsRsp:RefPt");
    variables.push_back("AbsRsp:JetEta");
    variables.push_back("AbsRsp:JetPhi");
    variables.push_back("AbsRsp:JetY");
    variables.push_back("AbsRsp:JetEta:RefPt");
    variables.push_back("AbsRsp:JetEta#1:RefPt");
    variables.push_back("AbsRsp:JetY:RefPt");
    variables.push_back("AbsRsp:JetY#1:RefPt");
  }
  if (doetarsp) {
    variables.push_back("EtaRsp:RefPt");
    variables.push_back("EtaRsp:JetEta");
    variables.push_back("EtaRsp:JetEta:RefPt");
    variables.push_back("EtaRsp:JetEta#1:RefPt");
  }
  if (dophirsp) {
    variables.push_back("PhiRsp:RefPt");
    variables.push_back("PhiRsp:JetEta");
    variables.push_back("PhiRsp:JetEta:RefPt");
    variables.push_back("PhiRsp:JetEta#1:RefPt");
  }

  if (flavors.size()>0) {
    if (flavors.front()=="all") {
      flavors.clear();
      flavors.push_back("uds");
      flavors.push_back("ud");
      flavors.push_back("s");
      flavors.push_back("c");
      flavors.push_back("b");
      flavors.push_back("g");
      flavors.push_back("slc");
      flavors.push_back("slb");
    }
    vector<string> vtmp(variables.begin(),variables.end());
    for (unsigned iflv=0;iflv<flavors.size();iflv++)
      for (unsigned iv=0;iv<vtmp.size();iv++)
    variables.push_back(flavors[iflv]+"_"+vtmp[iv]);
  }
  cout<<"List of variables: "<<endl;
  for (unsigned iv=0;iv<variables.size();iv++) cout<<variables[iv]<<endl;
  

  //
  // open output file and declare vectors for response & resolution graphs
  //
  if (output.empty()) {
    size_t pos=input.find(".root");
    output=input.substr(0,pos)+"_g.root";
    cout<<"*** write output to "<<output<<endl;
  }
  
  TFile* ofile = new TFile(output.c_str(),"RECREATE");
  if (!ofile->IsOpen()) { cout<<"Can't create "<<output<<endl; return 0; }
  vector<TGraphErrors*> vrsp;
  vector<TGraphErrors*> vres;
  vector<TGraphErrors*> vaone;
  vector<TGraphErrors*> vatwo;
  vector<TGraphErrors*> vpone;
  vector<TGraphErrors*> vptwo;
  
  //
  // open input file and loop over input directories (=algorithms)
  //
  TFile* ifile = new TFile(input.c_str(),"READ");
  if (!ifile->IsOpen()) { cout<<"Can't open "<<input<<endl; return 0; }
  
  if (algs.size()==0) {
    TIter nextDir(ifile->GetListOfKeys());
    TKey* dirKey(0);
    while ((dirKey=(TKey*)nextDir())) {
      if (strcmp(dirKey->GetClassName(),"TDirectoryFile")!=0) continue;
      algs.push_back(dirKey->GetName());
    }
  }
  
  for (unsigned int ialg=0;ialg<algs.size();++ialg) {
    
    string alg = algs[ialg];

    // truncation of RMS is algo specific...

    if (fracRMSpf<1. && alg.find("pf")!=string::npos) {
      fractionRMS = fracRMSpf;
      cout<<" Overriding fractionRMS with PF specific: "<<fracRMSpf<<endl;
    }
    else if (fracRMSjpt<1. && alg.find("jpt")!=string::npos) {
      fractionRMS = fracRMSjpt;
    cout<<" Overriding fractionRMS with JPT specific: "<<fracRMSjpt<<endl;
    }
    else if (fracRMScalo<1. && alg.find("calo")!=string::npos) {
      fractionRMS = fracRMScalo;
      cout<<" Overriding fractionRMS with CALO specific: "<<fracRMScalo<<endl;
    }

    // for each algorithm use a JERWriter (PtResolution=true)
    JERWriter resolutions(alg,fera,fprefix,true, domu || dorho || dopudensity, writeHeader, indirectProf);

    TDirectory* idir = (TDirectory*)ifile->Get(alg.c_str());
    if (0==idir) { cout<<"No dir "<<alg<<" found"<<endl; return 0; }
    
    cout<<alg<<" ... "<<flush;
    
    TDirectory* odir = (TDirectory*)ofile->mkdir(alg.c_str());
    gROOT->cd();
    
    for (unsigned int ivar=0;ivar<variables.size();ivar++) {
      string variable = variables[ivar];

      ObjectLoader<TH1F> hlrsp;
      hlrsp.load_objects(idir,variable);

      string xvar = hlrsp.variable(hlrsp.nvariables()-1);
      if (xvarOverride!="" && xvar==xvarOverride.substr(0,xvarOverride.find(":")))
         xvar = xvarOverride.substr(xvarOverride.find(":")+1,xvarOverride.rfind(":")-xvarOverride.find(":")-1);
      //string varexp=hlrsp.variable(hlrsp.nvariables()-1)+
      //variable.substr(variable.find(':'));
      string varexp=xvar+variable.substr(variable.find(':'));

      ObjectLoader<TH1F> hlvar;
      hlvar.load_objects(idir,varexp);

      vector<unsigned int> indices;
      // first the std resolution
      TH1F* hrsp(0); TGraphErrors* grsp(0); TGraphErrors* gres(0);
      // then the other stuff
      TGraphErrors* gaone(0);
      TGraphErrors* gatwo(0);
      TGraphErrors* gpone(0);
      TGraphErrors* gptwo(0);

      hlrsp.begin_loop();
      while ((hrsp=hlrsp.next_object(indices))) {

        //cout << "Quantity = " << hlrsp.quantity() << endl;
    // create new graphs for response & resolution
    if (indices.back()==0) {
      grsp = new TGraphErrors(0);  vrsp.push_back(grsp);
      gres = new TGraphErrors(0);  vres.push_back(gres);

      gaone = new TGraphErrors(0); vaone.push_back(gaone);
      gatwo = new TGraphErrors(0); vatwo.push_back(gatwo);
      gpone = new TGraphErrors(0); vpone.push_back(gpone);
      gptwo = new TGraphErrors(0); vptwo.push_back(gptwo);

      // this is where the magic happens...
      string prefix = hlrsp.quantity().substr(0,3);
      string suffix;
      if(hlrsp.quantity().size()>6) suffix = hlrsp.quantity().substr(hlrsp.quantity().find("_"),hlrsp.quantity().size()-hlrsp.quantity().find("_"));
      //string grsp_name=prefix+"RspVs"+hlrsp.variable(hlrsp.nvariables()-1)+suffix;
      //string gres_name=prefix+"ResVs"+hlrsp.variable(hlrsp.nvariables()-1)+suffix;
      //string gaone_name="AoneVs"+hlrsp.variable(hlrsp.nvariables()-1);
      //string gatwo_name="AtwoVs"+hlrsp.variable(hlrsp.nvariables()-1);
      //string gpone_name="PoneVs"+hlrsp.variable(hlrsp.nvariables()-1);
      //string gptwo_name="PtwoVs"+hlrsp.variable(hlrsp.nvariables()-1);
      string grsp_name=prefix+"RspVs"+xvar+suffix;
      string gres_name=prefix+"ResVs"+xvar+suffix;
      string gaone_name="AoneVs"+xvar;
      string gatwo_name="AtwoVs"+xvar;
      string gpone_name="PoneVs"+xvar;
      string gptwo_name="PtwoVs"+xvar;

      if (hlrsp.nvariables()>1) {
        for (unsigned int i=0;i<hlrsp.nvariables()-1;i++) {

          stringstream suffix;
          suffix<<"_"<<hlrsp.variable(i)
            <<hlrsp.minimum(i,indices[i])<<"to"
            <<hlrsp.maximum(i,indices[i]);
          grsp_name += suffix.str();
          gres_name += suffix.str();

          gaone_name += suffix.str();
          gatwo_name += suffix.str();
          gpone_name += suffix.str();
          gptwo_name += suffix.str();
        }
      }
      grsp->SetName(grsp_name.c_str());
      gres->SetName(gres_name.c_str());

      gaone->SetName(gaone_name.c_str());
      gatwo->SetName(gatwo_name.c_str());
      gpone->SetName(gpone_name.c_str());
      gptwo->SetName(gptwo_name.c_str());
    }
    
    // add new points to current response & resolution graphs
    if (hrsp->Integral()==0) continue;
    
    double x(0.0),ex(0.0);
    if (hlvar.nobjects()>0) {
      TH1F*  hvar = hlvar.object(indices);
      assert(hvar->GetEntries()>0);
      x  = hvar->GetMean();
      ex = hvar->GetMeanError();
    }
    else {
      double min = hlrsp.minimum(hlrsp.nvariables()-1,indices.back());
      double max = hlrsp.maximum(hlrsp.nvariables()-1,indices.back());
      x=0.5*(min+max);
    }

    //exclude points outside the physics range of the reco-algs

    if      ( (alg.find("calo")!=string::npos)&&(x<calomin) )   continue;
    else if ( (alg.find("jpt")!=string::npos)&&(x<jptmin) )     continue;
    else if ( (alg.find("puppi")!=string::npos)&&(x<puppimin) ) continue;
    else if ( (alg.find("pf")!=string::npos)&&(x<pfmin) )       continue;
    

    TF1*   frsp    = (TF1*)hrsp->GetListOfFunctions()->Last();
    bool   isFDSCB = (0==frsp) ? false : ("fdscb"==(string)frsp->GetName());
    
    if (minentries>0 && hrsp->GetEffectiveEntries()<minentries) continue;
    if (forcefit && frsp==0) continue;
    
    if (fractionRMS<1.) set_range_truncatedRMS(hrsp,fractionRMS);

    double y  = (frsp==0 || (semifitted && x<40.0)) ? hrsp->GetMean()      : frsp->GetParameter(1);
    double ey = (frsp==0 || (semifitted && x<40.0)) ? hrsp->GetMeanError() : frsp->GetParError(1);
    double e  = (frsp==0 || (semifitted && x<40.0)) ? hrsp->GetRMS()       : frsp->GetParameter(2);
    double ee = (frsp==0 || (semifitted && x<40.0)) ? hrsp->GetRMSError()  : frsp->GetParError(2);

    // declare the addtional pars for the CB function

    double aone(0.0),eaone(.25),atwo(0.0),eatwo(.25);
    double pone(0.0),epone(2.5),ptwo(0.0),eptwo(2.5);

    if (isFDSCB) {
      aone  = frsp->GetParameter(3);
      //eaone = frsp->GetParError(3);
      atwo  = frsp->GetParameter(5);
      //eatwo = frsp->GetParError(5);

      pone  = frsp->GetParameter(4);
      //epone = frsp->GetParError(4);
      ptwo  = frsp->GetParameter(6);
      //eptwo = frsp->GetParError(6);
    }
    
    if (hlrsp.quantity().find("AbsRsp")!=string::npos) {
      
      double yabs  = (x+y)/x;
      double eyabs = std::abs(yabs-1)*std::sqrt(ey*ey/y/y+ex*ex/x/x);
      double eabs  = e/x;
      double eeabs = eabs*std::sqrt(ee*ee/e/e+ex*ex/x/x);
        
      y  = yabs;
      ey = eyabs;
      e  = eabs;
      ee = eeabs;
    }
    else if (hlrsp.quantity().find("RelRsp")!=string::npos) {
      
      double erel  = e/y;
      double eerel = erel*std::sqrt(ee*ee/e/e+ey*ey/y/y);
      
      e  = erel;
      ee = eerel;
    }
        
    if (addminerr) {
      // add a minimal uncertainty of .5% in quadrature to each point
      ee    = std::sqrt(ee*ee+.0005*.0005);
      //eaone = std::sqrt(eaone*eaone+.0005*.0005);
      //eatwo = std::sqrt(eatwo*eatwo+.0005*.0005);
      //epone = std::sqrt(epone*epone+.0005*.0005);
      //eptwo = std::sqrt(eptwo*eptwo+.0005*.0005);
    }
    if(addUnc>0.0) {
        // add an additional uncertainty in quadrature to each point
        addUnc *= e;
        ee  = std::sqrt(ee*ee+addUnc*addUnc);
    }

    int n = grsp->GetN();
    grsp->SetPoint(n,x,y);
    grsp->SetPointError(n,ex,ey);
    gres->SetPoint(n,x,e);
    gres->SetPointError(n,ex,ee);

    if (isFDSCB) {
      n = gaone->GetN();
      gaone->SetPoint(n,x,aone);
      gaone->SetPointError(n,ex,eaone);

      n = gatwo->GetN();
      gatwo->SetPoint(n,x,atwo);
      gatwo->SetPointError(n,ex,eatwo);
          
      n = gpone->GetN();
      gpone->SetPoint(n,x,pone);
      gpone->SetPointError(n,ex,epone);
      
      n = gptwo->GetN();
      gptwo->SetPoint(n,x,ptwo);
      gptwo->SetPointError(n,ex,eptwo);
      
    }

      } //while hrsp iteration 

      
      // ---------------------------
      // fit resolution if requested
      // ---------------------------
      if (fitres) {
    
    //TVirtualFitter::SetDefaultFitter("Minuit2");

    double fitmin(0.0);
    double fitmax(0.0);

    if (calofitmin!=-1. && alg.find("calo")!=string::npos)   fitmin = calofitmin;
    if (jptfitmin!=-1. && alg.find("jpt")!=string::npos)     fitmin = jptfitmin;    
    if (pfchsfitmin!=-1. && alg.find("pfchs")!=string::npos) fitmin = pfchsfitmin;
    else if (pffitmin!=-1. && alg.find("pf")!=string::npos)  fitmin = pffitmin;
    if (puppifitmin!=-1. && alg.find("puppi")!=string::npos) fitmin = puppifitmin;

    if (calofitmax!=-1. && alg.find("calo")!=string::npos)   fitmax = calofitmax;
    if (jptfitmax!=-1. && alg.find("jpt")!=string::npos)     fitmax = jptfitmax;
    if (pffitmax!=-1. && alg.find("pf")!=string::npos)       fitmax = pffitmax;
    if (puppifitmax!=-1. && alg.find("puppi")!=string::npos) fitmax = puppifitmax;

    // SIGMA
    for (unsigned int igraph=0;igraph<vres.size();igraph++) {
      
      TGraphErrors* g = vres[igraph];
      //cout << g->GetName() << endl;

      if (g->GetN()==0) continue;
      double xmin(g->GetX()[0]);
      double xmax(-1e100);
      for (int ipoint=0;ipoint<g->GetN();ipoint++)
        if (g->GetX()[ipoint]>xmax) xmax = g->GetX()[ipoint];

      if (fitmin!=0.0 && fitmin>xmin) xmin = fitmin;
      if (fitmax!=0.0 && fitmax<xmax) xmax = fitmax;

      //TF1* fnc=new TF1("fit",
        //     s_sigma.c_str(),
        //     xmin,xmax);
      TF1* fnc = 0;

      if(hlrsp.quantity().find("EtaRsp")!=string::npos ||
         hlrsp.quantity().find("PhiRsp")!=string::npos) {
         fnc = new TF1("fit",s_sigma_angle.c_str(),xmin,xmax);
      }
      else if(xvar.find("RhoPU")!=string::npos) {
         fnc = new TF1("fit",s_sigma_density.c_str(),xmin,xmax);
      }
      else if(alg.find("calo")!=string::npos) {
         fnc = new TF1("fit",s_sigma_calo.c_str(),xmin,xmax);
      }
      else {
         fnc = new TF1("fit",s_sigma.c_str(),xmin,xmax);
      }
               
      fnc->SetLineWidth(2);
      fnc->SetLineColor(g->GetLineColor());
      //fnc->SetParameter(0,2.0);
      //fnc->SetParameter(1,0.5);
      //fnc->SetParameter(2,0.1);
      //fnc->SetParameter(3,0.2);

      //fnc->FixParameter(2,0.);

      if(hlrsp.quantity().find("EtaRsp")!=string::npos ||
         hlrsp.quantity().find("PhiRsp")!=string::npos) {
         fnc->SetParameters(0.005,0.02,150.0);
      }
      else if(alg.find("calo")!=string::npos) {
        fnc->SetParameters(+1,1,0.05,-0.8);
      }
      else if(alg.find("pf")!=string::npos) {
        fnc->SetParameters(3.5,0.5,0.03,-1);
      }
      else if(alg.find("puppi")!=string::npos) {
        fnc->SetParameters(3.5,0.5,0.03,-1);
      }
      else{
        fnc->SetParameters(-1,1,0.05,-0.8);
      }

      if(fixCTerm.size()==algs.size()) {
        if(fixCTerm[ialg]>-9999.0) {
            fnc->FixParameter(2,fixCTerm[ialg]);
        }
      }

      TString fit_options = "QR+";
      if(unweighted) fit_options = "W" + fit_options;
      //int fitstatus = g->Fit(fnc,fit_options);
      int fitstatus = -1;
      for(int i=0; i<nfititer; i++) {
        if(fitstatus==0)
            i=nfititer+1;
        else {
            if(g->GetListOfFunctions()->Last())
                g->GetListOfFunctions()->Last()->Delete();
            fitstatus = g->Fit(fnc,fit_options);
        }
      }

      if (0==fitstatus) resolutions.addEntry(g->GetName(),fnc);

      if (0!=fitstatus) {
        if (verbose)
          cout<<"Fit failed: "<<fitstatus
          <<" alg: "<<alg
          <<" var: "<<variable
          <<" name: "<<g->GetName()<<endl;
        if(g->GetListOfFunctions()->Last())
          g->GetListOfFunctions()->Last()->Delete();
        if (verbose)
          cout<<"...fnc deleted!"<<endl;
      }
      /*
      else {
        if (verbose)
        cout<<"Fit successful: "<<fitstatus<<" chi2/NDF: "<<fnc->GetChisquare()/fnc->GetNDF()
            <<" alg: "<<alg<<" var: "<<variable<<" name: "<<g->GetName()<<endl;
      }
      */
    } // SIGMA

    if (docbfits) {
      // AONE
      for (unsigned int igraph=0;igraph<vaone.size();igraph++) {
      
        TGraphErrors* g = vaone[igraph];

        if (g->GetN()==0) continue;
        double xmin(g->GetX()[0]);
        double xmax(-1e100);
        for (int ipoint=0;ipoint<g->GetN();ipoint++)
          if (g->GetX()[ipoint]>xmax) xmax = g->GetX()[ipoint];

        TF1* fnc=new TF1("fit",
                 s_aone.c_str(),
                 xmin,xmax);

        fnc->SetLineWidth(2);
        fnc->SetLineColor(g->GetLineColor());
        fnc->SetParameter(0,2.);

        int fitstatus = g->Fit(fnc,"QR");

        if (0==fitstatus) resolutions.addEntry(g->GetName(),fnc);

        if (0!=fitstatus) {
          if (verbose)
        cout<<"Fit failed: "<<fitstatus
            <<" alg: "<<alg
            <<" var: "<<variable
            <<" name: "<<g->GetName()<<endl;
          g->GetListOfFunctions()->Last()->Delete();
          if (verbose)
        cout<<"...fnc deleted!"<<endl;
        }
      } // AONE

      // ATWO
      for (unsigned int igraph=0;igraph<vatwo.size();igraph++) {
      
        TGraphErrors* g = vatwo[igraph];

        if (g->GetN()==0) continue;
        double xmin(g->GetX()[0]);
        double xmax(-1e100);
        for (int ipoint=0;ipoint<g->GetN();ipoint++)
          if (g->GetX()[ipoint]>xmax) xmax = g->GetX()[ipoint];

        TF1* fnc=new TF1("fit",
                 s_atwo.c_str(),
                 xmin,xmax);

        fnc->SetLineWidth(2);
        fnc->SetLineColor(g->GetLineColor());
        fnc->SetParameter(0,2.);
        fnc->SetParameter(1,0.7);

        int fitstatus = g->Fit(fnc,"QR");

        if (0==fitstatus) resolutions.addEntry(g->GetName(),fnc);

        if (0!=fitstatus) {
          if (verbose)
        cout<<"Fit failed: "<<fitstatus
            <<" alg: "<<alg
            <<" var: "<<variable
            <<" name: "<<g->GetName()<<endl;
          g->GetListOfFunctions()->Last()->Delete();
          if (verbose)
        cout<<"...fnc deleted!"<<endl;
        }
      } // ATWO

      // PONE
      for (unsigned int igraph=0;igraph<vpone.size();igraph++) {
      
        TGraphErrors* g = vpone[igraph];

        if (g->GetN()==0) continue;
        double xmin(g->GetX()[0]);
        double xmax(-1e100);
        for (int ipoint=0;ipoint<g->GetN();ipoint++)
          if (g->GetX()[ipoint]>xmax) xmax = g->GetX()[ipoint];

        TF1* fnc=new TF1("fit",
                 s_pone.c_str(),
                 xmin,xmax);

        fnc->SetLineWidth(2);
        fnc->SetLineColor(g->GetLineColor());

        fnc->FixParameter(0,25);
        fnc->SetParameter(1,.05);
        fnc->SetParameter(2,100);
        fnc->SetParameter(3,3);
        
        int fitstatus = g->Fit(fnc,"QR");

        if (0==fitstatus) resolutions.addEntry(g->GetName(),fnc);

        if (0!=fitstatus) {
          if (verbose)
        cout<<"Fit failed: "<<fitstatus
            <<" alg: "<<alg
            <<" var: "<<variable
            <<" name: "<<g->GetName()<<endl;
          g->GetListOfFunctions()->Last()->Delete();
          if (verbose)
        cout<<"...fnc deleted!"<<endl;
        }
      } // PONE

      // PTWO
      for (unsigned int igraph=0;igraph<vptwo.size();igraph++) {
      
        TGraphErrors* g = vptwo[igraph];

        if (g->GetN()==0) continue;
        double xmin(g->GetX()[0]);
        double xmax(-1e100);
        for (int ipoint=0;ipoint<g->GetN();ipoint++)
          if (g->GetX()[ipoint]>xmax) xmax = g->GetX()[ipoint];

        TF1* fnc=new TF1("fit",
                 s_ptwo.c_str(),
                 xmin,xmax);

        fnc->SetLineWidth(2);
        fnc->SetLineColor(g->GetLineColor());

        fnc->FixParameter(0,25);
        fnc->SetParameter(1,.05);
        fnc->SetParameter(2,100);
        fnc->SetParameter(3,3);

        int fitstatus = g->Fit(fnc,"QR");

        if (0==fitstatus) resolutions.addEntry(g->GetName(),fnc);

        if (0!=fitstatus) {
          if (verbose)
        cout<<"Fit failed: "<<fitstatus
            <<" alg: "<<alg
            <<" var: "<<variable
            <<" name: "<<g->GetName()<<endl;
          g->GetListOfFunctions()->Last()->Delete();
          if (verbose)
        cout<<"...fnc deleted!"<<endl;
        }
      } // PTWO
    } 
 
      } // if (fitres)

      // ------------------
      // fit resolution end
      // ------------------

    } // variables


    cout<<" DONE."<<endl;
    
    if (dowrite) resolutions.writeJER();

    // write response & resolution graphs to output root file
    odir->cd();

    for (unsigned int igraph=0;igraph<vrsp.size();igraph++)
      vrsp[igraph]->Write();
    for (unsigned int igraph=0;igraph<vres.size();igraph++)
      vres[igraph]->Write();

    for (unsigned int igraph=0;igraph<vaone.size();igraph++)
      if (vaone[igraph]->GetN()>0) vaone[igraph]->Write();
    for (unsigned int igraph=0;igraph<vatwo.size();igraph++)
      if (vatwo[igraph]->GetN()>0) vatwo[igraph]->Write();
    for (unsigned int igraph=0;igraph<vpone.size();igraph++)
      if (vpone[igraph]->GetN()>0) vpone[igraph]->Write();
    for (unsigned int igraph=0;igraph<vptwo.size();igraph++)
      if (vptwo[igraph]->GetN()>0) vptwo[igraph]->Write();
    
    vrsp.clear(); vres.clear();
    vaone.clear();vatwo.clear();vpone.clear();vptwo.clear();

  }
  
  //
  // close input & output files
  //
  gROOT->GetListOfFiles()->Remove(ofile);
  ofile->Close();
  delete ofile;

  gROOT->GetListOfFiles()->Remove(ifile);
  ifile->Close();
  delete ifile;

  return 0;
}

//______________________________________________________________________________
void set_range_truncatedRMS(TH1* hist,float frac)
{
  if (0==hist) return;

  const float nevts = hist->Integral(); if (0==nevts) return;
  const int   nbins = hist->GetNbinsX();

  if (frac<=0.0 || frac==1.) return;

  for (int ibin=1;ibin<nbins;++ibin) {
    int binx1   = ibin;
    int binx2   = nbins+1-ibin;
    float ievts = hist->Integral(binx1,binx2);

    if ( (ievts/nevts)>frac ) continue;
    else { hist->GetXaxis()->SetRange(binx1,binx2); break; }
  }
  return;
}

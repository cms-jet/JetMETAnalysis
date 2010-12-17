////////////////////////////////////////////////////////////////////////////////
//
// jet_l2_correction_x
// -------------------
//
//            08/16/2008 Kostas Kousouris                    <kkousour@fnal.gov>
//                       Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/ObjectLoader.h"
#include "JetMETAnalysis/JetUtilities/interface/RootStyle.h"


#include <TApplication.h>
#include <TFile.h>
#include <TKey.h>
#include <TH1F.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TLatex.h>


#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>


using namespace std;


////////////////////////////////////////////////////////////////////////////////
// define local functions
////////////////////////////////////////////////////////////////////////////////

/// check if a vector of strings contains a certain element
bool   contains(const vector<string>& collection,const string& element);

/// transform the alg label into a title, e.g.: kt4calo -> k_{T}, D=0.4 (Calo)
string get_legend_title(const string& alg);

/// get the suffix to the parmeter text file for each algorithm
string get_algorithm_suffix(const string& alg);


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

  string         input   = cl.getValue<string> ("input");
  string         era     = cl.getValue<string> ("era");
  string         l3input = cl.getValue<string> ("l3input","l3.root");
  string         output  = cl.getValue<string> ("output", "l2.root");
  vector<string> formats = cl.getVector<string>("formats",       "");
  vector<string> algs    = cl.getVector<string>("algs",          "");
  bool           batch   = cl.getValue<bool>   ("batch",      false);

  if (!cl.check()) return 0;
  cl.print();

  //
  // run a tapplication if not in batch mode
  //
  argc = (batch) ? 2 : 1; if (batch) argv[1] = (char*)"-b";
  TApplication* app = new TApplication("jet_l2_correction_x",&argc,argv);
  
  
  //
  // open output file
  //
  TFile* ofile = new TFile(output.c_str(),"RECREATE");
  if (!ofile->IsOpen()) { cout<<"Can't create "<<output<<endl; return 0; }
  
  
  //
  // open input & l3input files and loop over directories (algorithms)
  //
  TFile* ifile = new TFile(input.c_str(),"READ");
  if (!ifile->IsOpen()) { cout<<"Can't open "<<input<<endl; return 0; }
  
  TFile* l3file = new TFile(l3input.c_str(),"READ");
  if (!l3file->IsOpen()) { cout<<"Can't open "<<l3input<<endl; return 0; }
  
  if (algs.size()==0) {
    TIter nextDir(ifile->GetListOfKeys());
    TKey* dirKey(0);
    while ((dirKey=(TKey*)nextDir())) {
      if (strcmp(dirKey->GetClassName(),"TDirectoryFile")!=0) continue;
      algs.push_back(dirKey->GetName());
    }
  }
  
  TIter nextDir(ifile->GetListOfKeys());
  TKey* dirKey(0);
  while ((dirKey=(TKey*)nextDir())) {
    if (strcmp(dirKey->GetClassName(),"TDirectoryFile")!=0) continue;
    TDirectoryFile* idir = (TDirectoryFile*)dirKey->ReadObj();
    string alg(idir->GetName()); if (!contains(algs,alg)) continue;
    
    cout<<alg<<" ... "<<flush;
  
    TDirectoryFile* l3dir = (TDirectoryFile*)l3file->Get(alg.c_str());
    if (l3dir==0) {
      cout<<"Failed to rerieve L3 correction for "<<alg<<", skip"<<endl;
      continue;
    }
    
    TGraphErrors* gl3rsp = (TGraphErrors*)l3dir->Get("L3RspVsRefPt");
    TF1*          fl3rsp = (TF1*)gl3rsp->GetListOfFunctions()->First();
    if (0==fl3rsp) {
      cout<<"Failed to retrieve L3 correction for "<<alg<<", skip"<<endl;
      continue;
    }
    TDirectoryFile* odir = (TDirectoryFile*)ofile->mkdir(alg.c_str());
    odir->cd();
    
    ObjectLoader<TH1F> hl_rsp;
    
    hl_rsp.load_objects(idir,"RelRsp:JetEta:RefPt");
    
    ObjectLoader<TH1F> hl_refpt;
    hl_refpt.load_objects(idir,"RefPt:JetEta:RefPt");
    
    ObjectLoader<TH1F> hl_jetpt;
    hl_jetpt.load_objects(idir,"JetPt:JetEta:RefPt");
    
    
    //
    // absolute response/correction as a function of pT for each eta bin
    //
    vector<TGraphErrors*> vabsrsp_eta;
    vector<TGraphErrors*> vabscor_eta;
    
    vector<unsigned int> indices; TH1F* hrsp(0);
    hl_rsp.begin_loop();
    
    while ((hrsp=hl_rsp.next_object(indices))) {

      unsigned int ieta=indices[0];
      unsigned int ipt =indices[1];
      
      // create new graphs if a new eta bin comes around
      if (ipt==0) {
	vabsrsp_eta.push_back(new TGraphErrors());
	vabscor_eta.push_back(new TGraphErrors());
	stringstream ss;
	ss<<hl_rsp.minimum(0,ieta)<<"to"<<hl_rsp.maximum(0,ieta);
	vabsrsp_eta.back()->SetName(("AbsRspVsRefPt_JetEta"+ss.str()).c_str());
	vabscor_eta.back()->SetName(("AbsCorVsJetPt_JetEta"+ss.str()).c_str());
      }
      
      // only add points to the graphs if the current histo is not empty
      if (hrsp->Integral()!=0) {
	
	TF1*  frsp    = hrsp->GetFunction("fit");
	TH1F* hrefpt  = hl_refpt.object(indices);
	TH1F* hjetpt  = hl_jetpt.object(indices);
	
	assert(hrefpt->GetEntries()>0&&hjetpt->GetEntries()>0);
	
	double refpt  =hrefpt->GetMean();
	double erefpt =hrefpt->GetMeanError();
	double jetpt  =hjetpt->GetMean();
	double ejetpt =hjetpt->GetMeanError();
	
	double peak   =(frsp==0)?hrsp->GetMean()     :frsp->GetParameter(1);
	double epeak  =(frsp==0)?hrsp->GetMeanError():frsp->GetParError(1);
	
        double absrsp = peak;
        double eabsrsp = epeak;
        double abscor = 0.0;
        double eabscor = 0.0;

        if (absrsp > 0) 
          {  
            abscor  =1.0/absrsp;
            eabscor = abscor*abscor*epeak;
          } 
	if ((abscor>0) && (absrsp>0) && (eabscor>1e-3) && (eabscor/abscor<0.5) && (eabsrsp>1e-3) && (eabsrsp/absrsp<0.5)) { 
	  int n = vabsrsp_eta.back()->GetN();
	  vabsrsp_eta.back()->SetPoint     (n,refpt, absrsp);
	  vabsrsp_eta.back()->SetPointError(n,erefpt,eabsrsp);
	  vabscor_eta.back()->SetPoint     (n,jetpt, abscor);
	  vabscor_eta.back()->SetPointError(n,ejetpt,eabscor);      
        }
      }

      // fit graphs if last pt of the current eta bin comes around
      if (ipt==hl_jetpt.nobjects(1)-1) {
	TGraphErrors* gabsrsp = vabsrsp_eta.back();
	TGraphErrors* gabscor = vabscor_eta.back();
	TF1*          fabscor(0);
	int npoints = gabscor->GetN(); 
        double xmin(1.0),xmax(100.0);
        if (npoints > 0)
          {
            xmin = gabscor->GetX()[0];
            xmax = gabscor->GetX()[gabscor->GetN()-1];
          }
        if (npoints<3) {
          gabscor->SetPoint     (0, 10.0,1.0);
          gabscor->SetPointError(0,  0.0,0.0);
          gabscor->SetPoint     (1,100.0,1.0);
          gabscor->SetPointError(1,  0.0,0.0);
          fabscor = new TF1("fit","[0]",10.0,100.0);
          fabscor->FixParameter(0,1.0);
        }
        else if (npoints > 2 && gabscor->GetN()<10) {
          fabscor=new TF1("fit","[0]+[1]*log10(x)+[2]*pow(log10(x),2)",xmin,xmax);
          fabscor->SetParameter(0,1.0);
          fabscor->SetParameter(1,0.0);
          fabscor->SetParameter(2,0.0);
        } 
	else {
          if (alg.find("pf")!=string::npos) {
            fabscor=new TF1("fit","[0]+[1]/(pow(log10(x),2)+[2])+[3]*exp(-[4]*(log10(x)-[5])*(log10(x)-[5]))",xmin,xmax);
	    fabscor->SetParameter(0,0.5);
	    fabscor->SetParameter(1,9.0);
	    fabscor->SetParameter(2,8.0);
	    fabscor->SetParameter(3,-0.3);
            fabscor->SetParameter(4,0.6);
	    fabscor->SetParameter(5,1.0);
          }
          else if (alg.find("trk")!=string::npos) {
            fabscor=new TF1("fit","[0]+[1]*pow(x/500.0,[2])+[3]/log10(x)+[4]*log10(x)",xmin,xmax);
	    fabscor->SetParameter(0,1.7);
	    fabscor->SetParameter(1,0.7);
	    fabscor->SetParameter(2,3.0);
	    fabscor->SetParLimits(2,1,10);
	    fabscor->SetParameter(3,0.0);
	    fabscor->SetParameter(4,0.0);
          }
          else {
            fabscor=new TF1("fit","[0]+[1]/(pow(log10(x),[2])+[3])",xmin,xmax);
	    fabscor->SetParameter(0,1.0);
	    fabscor->SetParameter(1,5.0);
	    fabscor->SetParameter(2,3.0);
	    fabscor->SetParameter(3,3.0);
          }
	}
	
	gabscor->Fit(fabscor,"QR0");
	gabscor->GetListOfFunctions()->First()->ResetBit(TF1::kNotDraw);
        gabsrsp->SetMarkerStyle(20); 
        gabscor->SetMarkerStyle(20); 
	gabsrsp->Write();
	gabscor->Write();
      }

    }

    
    //
    // relative (L2) response/correction as a function of pT for each eta bin
    //
    string fnc_as_str = (alg.find("trk")>0) ? 
      "[0]+[1]*log10(x)+[2]*pow(log10(x),2)+[3]*pow(log10(x),3)+[4]*pow(x/500.0,3)" :
      "[0]+[1]*log10(x)+[2]*pow(log10(x),2)+[3]*pow(log10(x),3)+[4]*pow(log10(x),4)";
    
    vector<TGraph*> vrelcor_eta;
    TH1F*           hjetpt(0);
    hl_jetpt.begin_loop();
    while ((hjetpt=hl_jetpt.next_object(indices))) {
      
      unsigned int ieta = indices[0];
      unsigned int ipt  = indices[1];
      
      // create a new graph if a new eta bin comes around
      if (ipt==0) {
	vrelcor_eta.push_back(new TGraphErrors());
	stringstream ss;
	ss<<hl_jetpt.minimum(0,ieta)<<"to"<<hl_jetpt.maximum(0,ieta);
	vrelcor_eta.back()->SetName(("RelCorVsJetPt_JetEta"+ss.str()).c_str());
      }
      
      // only add a point to the graph if the current histo is not empty
      if (hjetpt->Integral()!=0) {
	TF1*   fabscor  =vabscor_eta[ieta]->GetFunction("fit");
	double jetpt    =hjetpt->GetMean();
	double refpt    =jetpt*fabscor->Eval(jetpt);
	double controlpt=refpt*fl3rsp->Eval(refpt);
	double relcor   =controlpt/jetpt;
        if (relcor > 5)
          cout<<"WARNING !!! suspicious point: "<<hjetpt->GetName()
	      <<", jet pt = "<<jetpt<<", ref pt = "<<refpt<<" "<<endl;
        else { 
	  int n=vrelcor_eta.back()->GetN();
	  vrelcor_eta.back()->SetPoint(n,jetpt,relcor);
        }
      }
      
      // fit the graph if the last pt of the current eta bin comes around
      if (ipt==hl_jetpt.nobjects(1)-1) {
	TGraph* grelcor = vrelcor_eta.back();
	double  xmin    = grelcor->GetX()[0];
	double  xmax    = grelcor->GetX()[grelcor->GetN()-1];
	TF1*    frelcor = new TF1("fit",fnc_as_str.c_str(),xmin,xmax);
	
	frelcor->SetParameter(0,0.0);
	frelcor->SetParameter(1,0.0);
	frelcor->SetParameter(2,0.0);
	frelcor->SetParameter(3,0.0);
	frelcor->SetParameter(4,0.0);
	frelcor->SetParameter(5,0.0);
	
	if (grelcor->GetN()<2) {
	  grelcor->SetPoint(0,10,1.0);
	  grelcor->SetPoint(1,100,1.0);
	  frelcor->FixParameter(1,0.0);
	  frelcor->FixParameter(2,0.0);
	  frelcor->FixParameter(3,0.0);
	  frelcor->FixParameter(4,0.0);
	  frelcor->FixParameter(5,0.0);
	}
	else if (grelcor->GetN()==2) {
	  frelcor->FixParameter(2,0.0);
	  frelcor->FixParameter(3,0.0);
	  frelcor->FixParameter(4,0.0);
	  frelcor->FixParameter(5,0.0);
	}
	
	grelcor->Fit(frelcor,"QRB0");
	grelcor->GetListOfFunctions()->First()->ResetBit(TF1::kNotDraw);
        grelcor->SetMarkerStyle(20); 
	grelcor->Write();
      }
    }
    
    
    //
    // write the L2 correction text file for the current algorithm
    //
    string txtfilename = era+"_L2Relative_"+get_algorithm_suffix(alg)+".txt";
    ofstream fout(txtfilename.c_str());
    fout.setf(ios::right);
    fout<<"{1 JetEta 1 JetPt "<<fnc_as_str<<" Correction L2Relative}"<<endl;
    for (unsigned int ieta=0;ieta<vrelcor_eta.size();ieta++) {
      TGraph* grelcor = vrelcor_eta[ieta];
      TF1*    frelcor = (TF1*)grelcor->GetListOfFunctions()->First();
      double  etamin  = hl_jetpt.minimum(0,ieta);
      double  etamax  = hl_jetpt.maximum(0,ieta);
      double  ptmin = grelcor->GetX()[0];
      double  ptmax = grelcor->GetX()[grelcor->GetN()-1];
      fout<<setw(11)<<etamin
	  <<setw(11)<<etamax
	  <<setw(11)<<(int)8
	  <<setw(12)<<ptmin
	  <<setw(12)<<ptmax
	  <<setw(13)<<frelcor->GetParameter(0)
	  <<setw(13)<<frelcor->GetParameter(1)
	  <<setw(13)<<frelcor->GetParameter(2)
	  <<setw(13)<<frelcor->GetParameter(3)
	  <<setw(13)<<frelcor->GetParameter(4)
	  <<setw(13)<<frelcor->GetParameter(5)
	  <<endl;
    }
    fout.close();
    
    
    cout<<"DONE"<<endl;
  }


  //
  // close output file
  //
  cout<<"Write "<<output<<" ... "<<flush;
  ofile->Close();
  delete ofile;
  ifile->Close();
  delete ifile;
  cout<<"DONE"<<endl;
  
  
  if (!batch) app->Run();
  
  return 0;
}


////////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
bool contains(const vector<string>& collection,const string& element)
{
  vector<string>::const_iterator it;
  for (it=collection.begin();it!=collection.end();++it)
    if ((*it)==element) return true;
  return false;
}


//______________________________________________________________________________
string get_legend_title(const string& alg)
{
  string title;
  string tmp(alg);
  if      (alg.find("kt")==0) { title = "k_{T}, D=";      tmp = tmp.substr(2); }
  else if (alg.find("sc")==0) { title = "SISCone, R=";    tmp = tmp.substr(2); }
  else if (alg.find("ic")==0) { title = "ItCone, R=";     tmp = tmp.substr(2); }
  else if (alg.find("mc")==0) { title = "MidCone. R=";    tmp = tmp.substr(2); }
  else if (alg.find("ca")==0) { title = "Cam/Aachen, D="; tmp = tmp.substr(2); }
  else if (alg.find("ak")==0) { title = "Anti k_{T}, D="; tmp = tmp.substr(2); }
  else return alg;
  
  string reco[5] = { "gen",  "calo",   "pf",      "trk",      "jpt" };
  string RECO[5] = { "(Gen)","(Calo)", "(PFlow)", "(Tracks)", "(JPT)" };

  size_t pos=string::npos; int ireco=-1;
  while (pos==string::npos&&ireco<4) { pos = tmp.find(reco[++ireco]); }
  if (pos==string::npos) return alg;
  
  double jet_size; stringstream ss1; ss1<<tmp.substr(0,pos); ss1>>jet_size;
  jet_size/=10.0;  stringstream ss2; ss2<<jet_size;

  title += ss2.str() + " " + RECO[ireco];

  return title;
}


//______________________________________________________________________________
string get_algorithm_suffix(const string& alg)
{
  string result;
  result += std::toupper(alg[0]);
  result += std::toupper(alg[1]);
  result += alg[2];
  if      (alg.find("calo")==3) result += "Calo";
  else if (alg.find("jpt") ==3) result += "JPT";
  else if (alg.find("pf")  ==3) result += "PF";
  else if (alg.find("trk") ==3) result += "TRK";
  cout<<"get_algorithm_suffix: result = "<<result;
  return result;
}

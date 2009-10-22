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
  string         l3input = cl.getValue<string> ("l3input","l3.root");
  string         output  = cl.getValue<string> ("output", "l2.root");
  string         tag     = cl.getValue<string> ("tag",           "");
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
  //set_root_style();
  
  
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
    
    ObjectLoader<TH1F> hl_absrsp;
    
    hl_absrsp.load_objects(idir,"AbsRsp:JetEta:RefPt");
    
    ObjectLoader<TH1F> hl_refpt;
    hl_refpt.load_objects(idir,"RefPt:JetEta:RefPt");
    
    ObjectLoader<TH1F> hl_jetpt;
    hl_jetpt.load_objects(idir,"JetPt:JetEta:RefPt");
    
    
    //
    // absolute response/correction as a function of pT for each eta bin
    //
    vector<TGraphErrors*> vabsrsp_eta;
    vector<TGraphErrors*> vabscor_eta;
    
    vector<unsigned int> indices; TH1F* habsrsp(0);
    hl_absrsp.begin_loop();
    
    while ((habsrsp=hl_absrsp.next_object(indices))) {

      unsigned int ieta=indices[0];
      unsigned int ipt =indices[1];
      
      // create new graphs if a new eta bin comes around
      if (ipt==0) {
	vabsrsp_eta.push_back(new TGraphErrors());
	vabscor_eta.push_back(new TGraphErrors());
	stringstream ss;
	ss<<hl_absrsp.minimum(0,ieta)<<"to"<<hl_absrsp.maximum(0,ieta);
	vabsrsp_eta.back()->SetName(("AbsRspVsRefPt_JetEta"+ss.str()).c_str());
	vabscor_eta.back()->SetName(("AbsCorVsJetPt_JetEta"+ss.str()).c_str());
      }
      
      // only add points to the graphs if the current histo is not empty
      if (habsrsp->Integral()!=0) {
	
	TF1*  fabsrsp = habsrsp->GetFunction("fit");
	TH1F* hrefpt  = hl_refpt.object(indices);
	TH1F* hjetpt  = hl_jetpt.object(indices);
	
	assert(hrefpt->GetEntries()>0&&hjetpt->GetEntries()>0);
	
	double refpt  =hrefpt->GetMean();
	double erefpt =hrefpt->GetMeanError();
	double jetpt  =hjetpt->GetMean();
	double ejetpt =hjetpt->GetMeanError();
	
	double peak   =(fabsrsp==0)?habsrsp->GetMean():fabsrsp->GetParameter(1);
	double epeak  =(fabsrsp==0)?habsrsp->GetMeanError():fabsrsp->GetParError(1);
	
	double refptsq =refpt*refpt;
	double erefptsq=erefpt*erefpt;
	double peaksq  =peak*peak;
	double epeaksq =epeak*epeak;
	
	double absrsp =(refpt+peak)/refpt;
	double eabsrsp=std::abs(absrsp-1.)*std::sqrt(epeaksq/peaksq+erefptsq/refptsq);
	double abscor  =1.0/absrsp;
	double eabscor =
	  std::abs(refpt*peak)/(refpt+peak)/(refpt+peak)*
	  std::sqrt(epeaksq/peaksq+erefptsq/refptsq);
	if ((abscor>0) && (absrsp>0) && (eabscor>1e-3) && (eabscor<0.5) && (eabsrsp>1e-3) && (eabsrsp<0.5)) { 
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
	
	double xmin = gabscor->GetX()[0];
	double xmax = gabscor->GetX()[gabscor->GetN()-1];
      
	if (gabscor->GetN()==0) {
	  gabscor->SetPoint     (0, 10.0,1.0);
	  gabscor->SetPointError(0,  0.0,0.0);
	  gabscor->SetPoint     (1,100.0,1.0);
	  gabscor->SetPointError(1,  0.0,0.0);
	  fabscor = new TF1("fit","[0]",10.0,100.0);
	}
	else if (gabscor->GetN()<10) {
	  fabscor=new TF1("fit","[0]+[1]*log10(x)+[2]*pow(log10(x),2)",xmin,xmax);
	  fabscor->SetParameter(0,0.0);
	  fabscor->SetParameter(1,0.0);
	  fabscor->SetParameter(2,0.0);
	}
	else {
          if ((int)alg.find("pf")>0) {
            fabscor=new TF1("fit","[0]+[1]/(pow(log10(x),2)+[2])+[3]*exp(-[4]*(log10(x)-[5])*(log10(x)-[5]))",xmin,xmax);
	    fabscor->SetParameter(0,0.5);
	    fabscor->SetParameter(1,9.0);
	    fabscor->SetParameter(2,8.0);
	    fabscor->SetParameter(3,-0.3);
            fabscor->SetParameter(4,0.6);
	    fabscor->SetParameter(5,1.0);
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
    vector<TGraph*> vrelcor_eta;
    TH1F* hjetpt(0);
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
          cout<<"WARNING !!! suspicious point: "<<hjetpt->GetName()<<", jet pt = "<<jetpt<<", ref pt = "<<refpt<<" "<<endl;
        else { 
	  int n=vrelcor_eta.back()->GetN();
	  vrelcor_eta.back()->SetPoint(n,jetpt,relcor);
        }
      }
      
      // fit the graph if the last pt of the current eta bin comes around
      if (ipt==hl_jetpt.nobjects(1)-1) {
	TGraph* grelcor = vrelcor_eta.back();
	TF1*    frelcor(0);

	double xmin = grelcor->GetX()[0];
	double xmax = grelcor->GetX()[grelcor->GetN()-1];
      
	if (grelcor->GetN()<2) {
	  grelcor->SetPoint(0,10,1.0);
	  grelcor->SetPoint(1,100,1.0);
	  frelcor=new TF1("fit","[0]",10,100);
	}
	else if (grelcor->GetN()==2) {
	  frelcor=new TF1("fit","[0]+[1]*log10(x)",xmin,xmax);
	}
	else {
	  frelcor=new TF1("fit","[0]+[1]*log10(x)+[2]*pow(log10(x),2)+[3]*pow(log10(x),3)+[4]*pow(log10(x),4)",xmin,xmax);
	}
	
	frelcor->SetParameter(0,0.0);
	frelcor->SetParameter(1,0.0);
	frelcor->SetParameter(2,0.0);
	frelcor->SetParameter(3,0.0);
	frelcor->SetParameter(4,0.0);
	frelcor->SetParameter(5,0.0);
	
	grelcor->Fit(frelcor,"QR0");
	grelcor->GetListOfFunctions()->First()->ResetBit(TF1::kNotDraw);
        grelcor->SetMarkerStyle(20); 
	grelcor->Write();
      }
    }
    
    
    //
    // write the L2 correction text file for the current algorithm
    //
    string txtfilename = "l2_"+alg;
    if (!tag.empty())
      txtfilename+="_"+tag;
    txtfilename+=".jec";
    ofstream fout(txtfilename.c_str());
    fout.setf(ios::right);
    for (unsigned int ieta=0;ieta<vrelcor_eta.size();ieta++) {
      TGraph* grelcor = vrelcor_eta[ieta];
      TF1*    frelcor = (TF1*)grelcor->GetListOfFunctions()->First();
      double  etamin  = hl_jetpt.minimum(0,ieta);
      double  etamax  = hl_jetpt.maximum(0,ieta);
      double  ptmin = grelcor->GetX()[0];
      double  ptmax = grelcor->GetX()[grelcor->GetN()-1];
      //frelcor()->GetRange(ptmin,ptmax); 
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

  string::size_type pos=string::npos; int ireco=-1;
  while (pos==string::npos&&ireco<4) { pos = tmp.find(reco[++ireco]); }
  if (pos==string::npos) return alg;
  
  double jet_size; stringstream ss1; ss1<<tmp.substr(0,pos); ss1>>jet_size;
  jet_size/=10.0;  stringstream ss2; ss2<<jet_size;

  title += ss2.str() + " " + RECO[ireco];

  return title;
}

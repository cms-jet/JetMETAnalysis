////////////////////////////////////////////////////////////////////////////////
//
// jet_matching_efficiency_x
// -------------------------
//
//            05/13/2009 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/RootStyle.h"

#include <TApplication.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TColor.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TTree.h>
#include <TH1F.h>
#include <TKey.h>

#include <iostream>
#include <sstream>
#include <string>
#include <cmath>


using namespace std;


////////////////////////////////////////////////////////////////////////////////
// define local functions
////////////////////////////////////////////////////////////////////////////////

/// set binomial errors for efficiency from two historgrams
void setBinomialErrors(TH1F* hEff,const TH1F* hEnum, const TH1F* hDenom);

/// transform the alg label into a title, e.g.: kt4calo -> k_{T}, D=0.4 (Calo)
string get_legend_title(const string& alg);


//______________________________________________________________________________
int main(int argc,char**argv)
{
  //
  // evaluate command-line / configuration file options
  //
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;

  vector<string> inputs   = cl.getVector<string>("inputs");
  vector<string> algs     = cl.getVector<string>("algs", "kt4calo");
  vector<double> drcuts   = cl.getVector<double>("drcuts",  "0.25");
  string         datapath = cl.getValue<string> ("datapath",    "");
  int            nbinspt  = cl.getValue<int>    ("nbinspt",    100);
  double         ptmin    = cl.getValue<double> ("ptmin",     10.0);
  double         ptmax    = cl.getValue<double> ("ptmax",    150.0);
  int            nbinseta = cl.getValue<int>    ("nbinseta",    50);
  double         etamin   = cl.getValue<double> ("etamin",    -5.0);
  double         etamax   = cl.getValue<double> ("etamax",     5.0);
  int            nbinsphi = cl.getValue<int>    ("nbinsphi",    50);
  double         phimin   = cl.getValue<double> ("phimin",   -3.14);
  double         phimax   = cl.getValue<double> ("phimax",    3.14);
  bool           logy     = cl.getValue<bool>   ("logy",      true);
  bool           batch    = cl.getValue<bool>   ("batch",    false);
  vector<string> formats  = cl.getVector<string>("formats",     "");
  
  if (!cl.check()) return 0;
  cl.print();
  
  vector<string> drcuts_as_str;
  for (unsigned int idr=0;idr<drcuts.size();idr++) {
    stringstream ssdr; ssdr<<drcuts[idr]*100.0;
    string sdr(ssdr.str());
    drcuts_as_str.push_back("dr0"+sdr.substr(0,sdr.find('.')));
  }
  
  TH1::SetDefaultSumw2();
  set_root_style();
  
  vector<TH1F**> hPtAll;
  vector<TH1F**> hPtSel;
  vector<TH1F**> hEtaAll;
  vector<TH1F**> hEtaSel;
  vector<TH1F**> hPhiAll;
  vector<TH1F**> hPhiSel;
  
  
  //
  // open input files and loop over input directories/trees (=algorithms!)
  //
  for (unsigned int ifile=0;ifile<inputs.size();ifile++) {
    
    unsigned pos    = inputs[ifile].find(":");
    string sample   = inputs[ifile].substr(0,pos);
    string filename = datapath+"/"+sample;
    if (filename.find(".root")==string::npos) filename += ".root";
    float  weight   = 1.0;
    if (pos!=string::npos){
      stringstream ss;
      ss<<inputs[ifile].substr(pos+1);
      ss>>weight;
    }
    cout<<"filename="<<filename<<", weight="<<weight<<endl;
    
    
    TFile* file = new TFile(filename.c_str(),"READ");
    if (!file->IsOpen()) {
      cout<<"Can't open "<<filename<<endl;
      continue;
    }
    
    for (unsigned int ialg=0;ialg<algs.size();ialg++) {
      
      string alg = algs[ialg];

      TDirectory* dir = (TDirectory*)file->Get(alg.c_str());
      if (0==dir) {
	cout<<"file "<<filename<<" does not contain dir "<<alg<<endl;
	continue;
      }
      
      TTree* tree = (TTree*)dir->Get("t");
      if (0==tree) {
	cout<<"dir "<<alg<<" does not contain tree t"<<endl;
	continue;
      }
      
      cout<<alg<<" ... "<<flush;      

      unsigned char nref(0);
      float refpt[100];
      float refeta[100];
      float refphi[100];
      float refdrjt[100];
      tree->SetBranchAddress("nref",     &nref);
      tree->SetBranchAddress("refpt",    refpt);
      tree->SetBranchAddress("refeta",  refeta);
      tree->SetBranchAddress("refphi",  refphi);
      tree->SetBranchAddress("refdrjt",refdrjt);

      if (ifile==0) {
	hPtAll.push_back(new TH1F*[drcuts.size()]);
	hPtSel.push_back(new TH1F*[drcuts.size()]);
	hEtaAll.push_back(new TH1F*[drcuts.size()]);
	hEtaSel.push_back(new TH1F*[drcuts.size()]);
	hPhiAll.push_back(new TH1F*[drcuts.size()]);
	hPhiSel.push_back(new TH1F*[drcuts.size()]);

	for (unsigned int i=0;i<drcuts.size();i++) {
	  string sdr = drcuts_as_str[i];
	  hPtAll.back()[i] = new TH1F(("PtAll_"+alg+"_dr0"+sdr).c_str(),"",
				      nbinspt,ptmin,ptmax);
	  hPtSel.back()[i] = new TH1F(("PtSel_"+alg+"_dr0"+sdr).c_str(),"",
				      nbinspt,ptmin,ptmax);
	  
	  hEtaAll.back()[i] = new TH1F(("EtaAll_"+alg+"_dr0"+sdr).c_str(),"",
				       nbinseta,etamin,etamax);
	  hEtaSel.back()[i] = new TH1F(("EtaSel_"+alg+"_dr0"+sdr).c_str(),"",
				      nbinseta,etamin,etamax);
	  
	  hPhiAll.back()[i] = new TH1F(("PhiAll_"+alg+"_dr0"+sdr).c_str(),"",
				       nbinsphi,phimin,phimax);
	  hPhiSel.back()[i] = new TH1F(("PhiSel_"+alg+"_dr0"+sdr).c_str(),"",
				       nbinsphi,phimin,phimax);
	}
      }
      
      
      // fill histograms
      unsigned int nevt = (unsigned int)tree->GetEntries();
      for (unsigned int ievt=0;ievt<nevt;ievt++) {
	tree->GetEntry(ievt);
	for (unsigned int iref=0;iref<nref;iref++) {
	  for (unsigned int idr=0;idr<drcuts.size();idr++) {
	    hPtAll[ialg][idr] ->Fill(refpt[iref], weight);
	    hEtaAll[ialg][idr]->Fill(refeta[iref],weight);
	    hPhiAll[ialg][idr]->Fill(refphi[iref],weight);
	    if (refdrjt[iref]<drcuts[idr]) {
	      hPtSel[ialg][idr] ->Fill(refpt[iref], weight);
	      hEtaSel[ialg][idr]->Fill(refeta[iref],weight);
	      hPhiSel[ialg][idr]->Fill(refphi[iref],weight);
	    }
	  }
	}
      }
      
      cout<<" DONE."<<endl;
    }
  }
  
  
  // instantiate root application object, enable batch mode if requested
  argc= (batch) ? 2 : 1; if (batch) argv[1] = "-b";
  TApplication* app = new TApplication("jet_matching_efficiency_x",&argc,argv);
  
  
  gStyle->SetOptStat(0);
  TColor::SetPalette(1,0);
  Color_t colors[7] = {kRed,kBlue,kMagenta,kCyan,kGreen+1,kBlue+4,kRed+4};
  
  // one canvas per algorithm, compare several dr cuts
  for (unsigned int ialg=0;ialg<algs.size();ialg++) {
    
    string cptname  = "EffVsPt_"+algs[ialg];
    string cetaname = "EffVsEta_"+algs[ialg];
    string cphiname = "EffVsPhi_"+algs[ialg];

    TCanvas* cPt = new TCanvas(cptname.c_str(),cptname.c_str(),
			       (ialg%2)*380,0,365,365);
    cPt->cd(); if (logy) gPad->SetLogy();
    gPad->SetLeftMargin(0.2); gPad->SetTopMargin(0.08);
    TCanvas* cEta = new TCanvas(cetaname.c_str(),cetaname.c_str(),
				(ialg%2)*380,400,365,365);
    cEta->cd(); if (logy) gPad->SetLogy();
    gPad->SetLeftMargin(0.2); gPad->SetTopMargin(0.08);
    TCanvas* cPhi = new TCanvas(cphiname.c_str(),cphiname.c_str(),
				(ialg%2)*380,800,365,365);
    cPhi->cd(); if (logy) gPad->SetLogy();
    gPad->SetLeftMargin(0.2); gPad->SetTopMargin(0.08);
    
    TLegend* legPt = new TLegend(0.6,0.2,0.9,0.2+drcuts.size()*0.065);
    legPt->SetLineColor(10); legPt->SetFillColor(10); legPt->SetShadowColor(10);
    TLegend* legEta = new TLegend(0.6,0.2,0.9,0.2+drcuts.size()*0.065);
    legEta->SetLineColor(10); legEta->SetFillColor(10); legEta->SetShadowColor(10);
    TLegend* legPhi = new TLegend(0.6,0.2,0.9,0.2+drcuts.size()*0.065);
    legPhi->SetLineColor(10); legPhi->SetFillColor(10); legPhi->SetShadowColor(10);

    for (unsigned int idr=0;idr<drcuts.size();idr++) {
      string hptname  = "EffVsPt_"+algs[ialg]+"_"+drcuts_as_str[idr];
      string hetaname = "EffVsEta_"+algs[ialg]+"_"+drcuts_as_str[idr];
      string hphiname = "EffVsPhi_"+algs[ialg]+"_"+drcuts_as_str[idr];

      cPt->cd();
      TH1F* hEffVsPt = new TH1F(hptname.c_str(),"",nbinspt,ptmin,ptmax);
      hEffVsPt->Divide(hPtSel[ialg][idr],hPtAll[ialg][idr],1.0,1.0,"B");
      setBinomialErrors(hEffVsPt,hPtSel[ialg][idr],hPtAll[ialg][idr]);
      hEffVsPt->SetXTitle("p_{T}^{REF} [GeV]");
      hEffVsPt->SetYTitle("Matching Efficiency");
      hEffVsPt->GetYaxis()->CenterTitle();
      hEffVsPt->SetLineColor(colors[idr]);

      cEta->cd();
      TH1F* hEffVsEta = new TH1F(hetaname.c_str(),"",nbinseta,etamin,etamax);
      hEffVsEta->Divide(hEtaSel[ialg][idr],hEtaAll[ialg][idr],1.0,1.0,"B");
      setBinomialErrors(hEffVsEta,hEtaSel[ialg][idr],hEtaAll[ialg][idr]);
      hEffVsEta->SetXTitle("#eta^{REF}");
      hEffVsEta->SetYTitle("Matching Efficiency");
      hEffVsEta->GetYaxis()->CenterTitle();
      hEffVsEta->SetLineColor(colors[idr]);

      cPhi->cd();
      TH1F* hEffVsPhi = new TH1F(hphiname.c_str(),"",nbinsphi,phimin,phimax);
      hEffVsPhi->Divide(hPhiSel[ialg][idr],hPhiAll[ialg][idr],1.0,1.0,"B");
      setBinomialErrors(hEffVsPhi,hPhiSel[ialg][idr],hPhiAll[ialg][idr]);
      hEffVsPhi->SetXTitle("#phi^{REF}");
      hEffVsPhi->SetYTitle("Matching Efficiency");
      hEffVsPhi->GetYaxis()->CenterTitle();
      hEffVsPhi->SetLineColor(colors[idr]);      

      stringstream ssdr; ssdr<<"#DeltaR<"<<drcuts[idr];
      legPt ->AddEntry(hEffVsPt, ssdr.str().c_str(),"l");
      legEta->AddEntry(hEffVsEta,ssdr.str().c_str(),"l");
      legPhi->AddEntry(hEffVsPhi,ssdr.str().c_str(),"l");

      TLatex txt; txt.SetNDC();
      cPt->cd();  txt.DrawLatex(0.2,0.93,get_legend_title(algs[ialg]).c_str());
      cEta->cd(); txt.DrawLatex(0.2,0.93,get_legend_title(algs[ialg]).c_str());
      cPhi->cd(); txt.DrawLatex(0.2,0.93,get_legend_title(algs[ialg]).c_str());
      
      string drawopt = (idr==0) ? "E" : "ESAME";
      cPt ->cd(); hEffVsPt ->Draw(drawopt.c_str());
      cEta->cd(); hEffVsEta->Draw(drawopt.c_str());
      cPhi->cd(); hEffVsPhi->Draw(drawopt.c_str());
    }
    cPt->cd();  legPt->Draw();
    cEta->cd(); legEta->Draw();
    cPhi->cd(); legPhi->Draw();

    for (unsigned int iformat=0;iformat<formats.size();iformat++) {
      cPt ->Print((string(cPt ->GetName())+"."+formats[iformat]).c_str());
      cEta->Print((string(cEta->GetName())+"."+formats[iformat]).c_str());
      cPhi->Print((string(cPhi->GetName())+"."+formats[iformat]).c_str());
    }
  }
  
  // one canvas per dr cut, compare several algorithms
  for (unsigned int idr=0;idr<drcuts.size();idr++) {
    
    string cptname  = "EffVsPt_"+drcuts_as_str[idr];
    string cetaname = "EffVsEta_"+drcuts_as_str[idr];
    string cphiname = "EffVsPhi_"+drcuts_as_str[idr];
    
    TCanvas* cPt = new TCanvas(cptname.c_str(),cptname.c_str(),
			       800+(idr%2)*380,0,365,365);
    cPt->cd(); if (logy) gPad->SetLogy();
    gPad->SetLeftMargin(0.2); gPad->SetTopMargin(0.08);
    TCanvas* cEta = new TCanvas(cetaname.c_str(),cetaname.c_str(),
				800+(idr%2)*380,400,365,365);
    cEta->cd(); if (logy) gPad->SetLogy();
    gPad->SetLeftMargin(0.2); gPad->SetTopMargin(0.08);
    TCanvas* cPhi = new TCanvas(cphiname.c_str(),cphiname.c_str(),
				800+(idr%2)*380,800,365,365);
    cPhi->cd(); if (logy) gPad->SetLogy();
    gPad->SetLeftMargin(0.2); gPad->SetTopMargin(0.08);
    
    TLegend* legPt = new TLegend(0.5,0.2,0.9,0.2+drcuts.size()*0.065);
    legPt->SetLineColor(10); legPt->SetFillColor(10); legPt->SetShadowColor(10);
    TLegend* legEta = new TLegend(0.5,0.2,0.9,0.2+drcuts.size()*0.065);
    legEta->SetLineColor(10); legEta->SetFillColor(10); legEta->SetShadowColor(10);
    TLegend* legPhi = new TLegend(0.5,0.2,0.9,0.2+drcuts.size()*0.065);
    legPhi->SetLineColor(10); legPhi->SetFillColor(10); legPhi->SetShadowColor(10);
    
    for (unsigned int ialg=0;ialg<algs.size();ialg++) {
      string hptname  = "EffVsPt_" +drcuts_as_str[idr]+"_"+algs[ialg];
      string hetaname = "EffVsEta_"+drcuts_as_str[idr]+"_"+algs[ialg];
      string hphiname = "EffVsPhi_"+drcuts_as_str[idr]+"_"+algs[ialg];

      cPt->cd();
      TH1F* hEffVsPt = new TH1F(hptname.c_str(),"",nbinspt,ptmin,ptmax);
      hEffVsPt->Divide(hPtSel[ialg][idr],hPtAll[ialg][idr],1.0,1.0,"B");
      setBinomialErrors(hEffVsPt,hPtSel[ialg][idr],hPtAll[ialg][idr]);
      hEffVsPt->SetXTitle("p_{T}^{REF} [GeV]");
      hEffVsPt->SetYTitle("Matching Efficiency");
      hEffVsPt->GetYaxis()->CenterTitle();
      hEffVsPt->SetLineColor(colors[ialg]);

      cEta->cd();
      TH1F* hEffVsEta = new TH1F(hetaname.c_str(),"",nbinseta,etamin,etamax);
      hEffVsEta->Divide(hEtaSel[ialg][idr],hEtaAll[ialg][idr],1.0,1.0,"B");
      setBinomialErrors(hEffVsEta,hEtaSel[ialg][idr],hEtaAll[ialg][idr]);
      hEffVsEta->SetXTitle("#eta^{REF}");
      hEffVsEta->SetYTitle("Matching Efficiency");
      hEffVsEta->GetYaxis()->CenterTitle();
      hEffVsEta->SetLineColor(colors[ialg]);

      cPhi->cd();
      TH1F* hEffVsPhi = new TH1F(hphiname.c_str(),"",nbinsphi,phimin,phimax);
      hEffVsPhi->Divide(hPhiSel[ialg][idr],hPhiAll[ialg][idr],1.0,1.0,"B");
      setBinomialErrors(hEffVsPhi,hPhiSel[ialg][idr],hPhiAll[ialg][idr]);
      hEffVsPhi->SetXTitle("#phi^{REF}");
      hEffVsPhi->SetYTitle("Matching Efficiency");
      hEffVsPhi->GetYaxis()->CenterTitle();
      hEffVsPhi->SetLineColor(colors[ialg]);      

      legPt ->AddEntry(hEffVsPt, get_legend_title(algs[ialg]).c_str(),"l");
      legEta->AddEntry(hEffVsEta,get_legend_title(algs[ialg]).c_str(),"l");
      legPhi->AddEntry(hEffVsPhi,get_legend_title(algs[ialg]).c_str(),"l");

      TLatex txt; txt.SetNDC();
      stringstream ssdr; ssdr<<"#DeltaR<"<<drcuts[idr];
      cPt->cd();  txt.DrawLatex(0.2,0.93,ssdr.str().c_str());
      cEta->cd(); txt.DrawLatex(0.2,0.93,ssdr.str().c_str());
      cPhi->cd(); txt.DrawLatex(0.2,0.93,ssdr.str().c_str());
      
      string drawopt = (ialg==0) ? "E" : "ESAME";
      cPt ->cd(); hEffVsPt ->Draw(drawopt.c_str());
      cEta->cd(); hEffVsEta->Draw(drawopt.c_str());
      cPhi->cd(); hEffVsPhi->Draw(drawopt.c_str());
    }
    cPt->cd();  legPt->Draw();
    cEta->cd(); legEta->Draw();
    cPhi->cd(); legPhi->Draw();

    for (unsigned int iformat=0;iformat<formats.size();iformat++) {
      cPt ->Print((string(cPt ->GetName())+"."+formats[iformat]).c_str());
      cEta->Print((string(cEta->GetName())+"."+formats[iformat]).c_str());
      cPhi->Print((string(cPhi->GetName())+"."+formats[iformat]).c_str());
    }
  }
  
  
  
  // run application
  if (!batch) app->Run();

  return 0;
}



////////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void setBinomialErrors(TH1F* hEff,const TH1F* hEnum, const TH1F* hDenom)
{
  for (int i=1;i<=hEff->GetNbinsX();i++) {
    float nenum =hEnum ->GetBinContent(i);
    float ndenom=hDenom->GetBinContent(i);
    float eeff=(ndenom>0.0) ? std::sqrt(nenum/(ndenom*ndenom)*(1-nenum/ndenom)):0.0;
    hEff->SetBinError(i,eeff);
  }
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
  
  assert(!title.empty());
  
  string            reco[4] = { "calo","pf","trk","jpt" };
  string            RECO[4] = { "(Calo)", "(PFlow)", "(Tracks)", "(JPT)" };

  string::size_type pos=string::npos; int ireco=-1;
  while (pos==string::npos&&ireco<3) { pos = tmp.find(reco[++ireco]); }
  assert(pos!=string::npos);
  
  double jet_size; stringstream ss1; ss1<<tmp.substr(0,pos); ss1>>jet_size;
  jet_size/=10.0;  stringstream ss2; ss2<<jet_size;
  title += ss2.str() + " " + RECO[ireco];

  return title;
}

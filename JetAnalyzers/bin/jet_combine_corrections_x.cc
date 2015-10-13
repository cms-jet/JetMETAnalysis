///////////////////////////////////////////////////////////////////
//
// jet_combine_corrections_x
// -------------------------
//
//            01/20/2014 Alexx Perloff aperloff@physics.tamu.edu
//
// This executable reads the relevant root files which have drawn
// either the corrections vs eta or the corrections vs pt. It will
// then loop over multiple files/algorithms and draw them to the
// same canvas.
///////////////////////////////////////////////////////////////////

#include "JetMETAnalysis/JetUtilities/interface/JetInfo.hh"
#include "JetMETAnalysis/JetUtilities/interface/Style.h"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"

#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TObject.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TF1.h"
#include "TString.h"
#include "TPaveText.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TMath.h"

#include <fstream>
#include <string>
#include <cmath>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

vector<Int_t> getColors();

vector<Int_t> getMarkerNumbers();

string getAlias(TString s);

TString getAlgNameLong(TString algo, int coneSize = 0);

//______________________________________________________________________________
int main(int argc,char**argv)
{
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);

  gSystem->Load("libFWCoreFWLite.so");

  // evaluate command-line / configuration file options
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;

  vector<TString> algs         = cl.getVector<TString> ("algs");
  int             ptBin        = cl.getValue<int>      ("ptBin");
  double          ptValue      = cl.getValue<double>   ("ptValue");
  TString         path         = cl.getValue<TString>  ("path",                    "");
  TString         outputDir    = cl.getValue<TString>  ("outputDir",             "./");
  vector<TString> outputFormat = cl.getVector<TString> ("outputFormat", ".png:::.pdf");

  if (algs.size()==0){
    cout<<"ERROR you must use the flag -algs to select one or more algorithms!"<<endl;
  }

  if(!path.EndsWith("/")) path+="/";
  if(!outputDir.EndsWith("/")) outputDir+="/";

  vector<Int_t> colors = getColors();
  vector<Int_t> markers = getMarkerNumbers();
  
  TString allAlgs;
  for(unsigned int ialg=0; ialg<algs.size(); ialg++) {
    allAlgs+=algs[ialg];
    if(ialg<algs.size()-1)
      allAlgs+="_";
  }

  TString ss("CorrectionVsEta_Comparison_TDR");
  ss = Form("%s_%i_%s",ss.Data(),ptBin,allAlgs.Data());

  TLegend* leg = tdrLeg(0.25,0.51,0.9,0.71);
  leg->SetNColumns(2);
  leg->SetBorderSize(0);

  TString ptstr;
  if (ptValue<0.1)
     ptstr.Form("p_{T} = %f GeV",ptValue);
  else 
     ptstr.Form("p_{T} = %.0f GeV",ptValue);

  TPaveText* pave;
  pave = tdrText(0.5,0.71,0.93,1-gPad->GetTopMargin()-0.045*(1-gPad->GetTopMargin()-gPad->GetBottomMargin())+0.01,31);
  pave->AddText("QCD Monte Carlo");
  pave->AddText(JetInfo::get_legend_title(string(algs[0]),false).c_str());
  pave->AddText(ptstr);

  map<TString,TH1F*> histos;
  map<TString,TH1F*> histos_norm;
  int normIndex = 0;

  for (unsigned int ialg=0; ialg<algs.size(); ialg++) {
    TString filename = path+"/Corrections_Overview_"+algs[ialg]+".root";
    cout << "jet_combine_corrections_x::main opening file " << filename << " ... ";
    TFile* f = new TFile(filename,"READ");
    cout << "DONE" << endl;

    TString hstr = Form("EtaSF_TDR_%d_%s",ptBin,algs[ialg].Data());
    cout << "jet_combine_corrections_x::main getting and cloning histogram " << hstr << " ... ";
    histos[hstr] = (TH1F*)(f->Get(Form("EtaSF_TDR_%d",ptBin))->Clone(hstr));
    histos[hstr]->SetDirectory(0);
    histos_norm[hstr+"_norm"] = (TH1F*)(f->Get(Form("EtaSF_TDR_%d",ptBin))->Clone(hstr+"_norm"));
    histos_norm[hstr+"_norm"]->SetDirectory(0);
    cout << "DONE" << endl;

    if(algs[ialg].Contains("ak5")) {
      normIndex = ialg;
    }

    leg->AddEntry(histos[hstr],getAlgNameLong(algs[ialg],2),"p");

    cout << "jet_combine_corrections_x::main closing file " << filename << " ... ";
    f->Close();
    cout << "DONE" << endl;
  }
  for (unsigned int ialg=0; ialg<algs.size(); ialg++) {
    TString numstr = Form("EtaSF_TDR_%d_%s_norm",ptBin,algs[ialg].Data());
    TString denstr = Form("EtaSF_TDR_%d_%s",ptBin,algs[normIndex].Data());
    cout << "jet_combine_corrections_x::main dividing histogram " << numstr << " by " << denstr << " ... ";
    histos_norm[numstr]->Divide(histos[denstr]);
    cout << "DONE" << endl;
  }

  setTDRStyle();

  TH1D* frame = new TH1D();
  frame->GetXaxis()->SetLimits(-5.0,5.0);
  frame->GetYaxis()->SetRangeUser(0.95,1.80);
  frame->GetXaxis()->SetTitle("#eta");
  frame->GetYaxis()->SetTitle("Corr. Factor");
  TH1D* frameRatio = new TH1D();
  frameRatio->GetXaxis()->SetLimits(-5.0,5.0);
  frameRatio->GetYaxis()->SetRangeUser(0.9,1.2);
  frameRatio->GetXaxis()->SetTitle("#eta");
  frameRatio->GetYaxis()->SetTitle("Ratio to R=0.5");
  TCanvas* can = tdrDiCanvas(ss,frame,frameRatio,2,11);

  can->cd(1);
  for (unsigned int ialg=0; ialg<algs.size(); ialg++) {
    TString hstr = Form("EtaSF_TDR_%d_%s",ptBin,algs[ialg].Data());
    tdrDraw(histos[hstr],"P",markers[ialg],colors[ialg],kSolid,colors[ialg],kNone,0);
  }
  leg->Draw("same");
  pave->Draw("same");

  can->cd(2);
  for (unsigned int ialg=0; ialg<algs.size(); ialg++) {
    TString hstr = Form("EtaSF_TDR_%d_%s_norm",ptBin,algs[ialg].Data());
    tdrDraw(histos_norm[hstr],"P",markers[ialg],colors[ialg],kSolid,colors[ialg],kNone,0);
  }

 for(unsigned int of=0; of<outputFormat.size(); of++) {
    can->SaveAs(outputDir+can->GetName()+outputFormat[of]);
 }

 return 0;
}

//______________________________________________________________________________
string getAlias(TString s)
{
   if (s=="ic5calo")
      return "IC5Calo";
   else if (s=="ic5pf")
      return "IC5PF";
   else if (s=="ak5calo")
      return "AK5Calo";  
   else if (s=="ak5calol1")
      return "AK5Calol1";
   else if (s=="ak5calol1off")
      return "AK5Calol1off";
   else if (s=="ak5calol1offl2l3")
      return "AK5Calol1off";
   else if (s=="ak7calo")
      return "AK7Calo";
   else if (s=="ak7calol1")
      return "AK7Calol1";
   else if (s=="ak7calol1off")
      return "AK7Calol1off";
   else if (s=="ak5caloHLT")
      return "AK5CaloHLT";
   else if (s=="ak5caloHLTl1")
      return "AK5CaloHLTl1";
   else if (s=="ak1pf")
      return "AK1PF";
   else if (s=="ak1pfl1")
      return "AK1PFl1";
   else if (s=="ak2pf")
      return "AK2PF";
   else if (s=="ak2pfl1")
      return "AK2PFl1";
   else if (s=="ak3pf")
      return "AK3PF";
   else if (s=="ak3pfl1")
      return "AK3PFl1";
   else if (s=="ak4pf")
      return "AK4PF";
   else if (s=="ak4pfl1")
      return "AK4PFl1";
   else if (s=="ak5pf")
      return "AK5PF";
   else if (s=="ak5pfl1")
      return "AK5PFl1";
   else if (s=="ak5pfl1l2l3")
      return "AK5PFl1";
   else if (s=="ak5pfl1off")
      return "AK5PFl1off";
   else if (s=="ak6pf")
      return "AK6PF";
   else if (s=="ak6pfl1")
      return "AK6PFl1";
   else if (s=="ak7pf")
      return "AK7PF";
   else if (s=="ak7pfl1")
      return "AK7PFl1";
   else if (s=="ak7pfl1off")
      return "AK7PFl1off";
   else if (s=="ak8pf")
      return "AK8PF";
   else if (s=="ak8pfl1")
      return "AK8PFl1";
   else if (s=="ak9pf")
      return "AK9PF";
   else if (s=="ak9pfl1")
      return "AK9PFl1";
   else if (s=="ak10pf")
      return "AK10PF";
   else if (s=="ak10pfl1")
      return "AK10PFl1";
   else if (s=="ak1pfchs")
      return "AK1PFchs";
   else if (s=="ak1pfchsl1")
      return "AK1PFchsl1";
   else if (s=="ak2pfchs")
      return "AK2PFchs";
   else if (s=="ak2pfchsl1")
      return "AK2PFchsl1";
   else if (s=="ak3pfchs")
      return "AK3PFchs";
   else if (s=="ak3pfchsl1")
      return "AK3PFchsl1";
   else if (s=="ak4pfchs")
      return "AK4PFchs";
   else if (s=="ak4pfchsl1")
      return "AK4PFchsl1";
   else if (s=="ak5pfchs")
      return "AK5PFchs";
   else if (s=="ak5pfchsl1")
      return "AK5PFchsl1";
   else if (s=="ak5pfchsl1l2l3")
      return "AK5PFchsl1";
   else if (s=="ak5pfchsl1off")
      return "AK5PFchsl1off";
   else if (s=="ak6pfchs")
      return "AK6PFchs";
   else if (s=="ak6pfchsl1")
      return "AK6PFchsl1";
   else if (s=="ak7pfchs")
      return "AK7PFchs";
   else if (s=="ak7pfchsl1")
      return "AK7PFchsl1";
   else if (s=="ak7pfchsl1off")
      return "AK7PFchsl1off";
   else if (s=="ak8pfchs")
      return "AK8PFchs";
   else if (s=="ak8pfchsl1")
      return "AK8PFchsl1";
   else if (s=="ak9pfchs")
      return "AK9PFchs";
   else if (s=="ak9pfchsl1")
      return "AK9PFchsl1";
   else if (s=="ak10pfchs")
      return "AK10PFchs";
   else if (s=="ak10pfchsl1")
      return "AK10PFchsl1";
   else if (s=="ak5pfHLT")
      return "AK5PFHLT";
  else if (s=="ak5pfHLTl1")
      return "AK5PFHLTl1";
   else if (s=="ak5pfchsHLT")
      return "AK5PFchsHLT";
   else if (s=="ak5pfchsHLTl1")
      return "AK5PFchsHLTl1";
   else if (s=="ak5jpt")
      return "AK5JPT";
   else if (s=="ak5jptl1")
      return "AK5JPTl1";
   else if (s=="ak5jptl1off")
      return "AK5JPTl1off";
   else if (s=="ak5jptl1l2l3")
      return "AK5JPTl1";
   else if (s=="ak5jptl1offl2l3")
      return "AK5JPTl1off";
   else if (s=="ak7jpt")
      return "AK7JPT";
   else if (s=="ak7jptl1")
      return "AK7JPTl1";
   else if (s=="ak7jptl1off")
      return "AK7JPTl1off";
   else if (s=="sc5calo")
      return "SC5Calo";
   else if (s=="sc5pf")
      return "SC5PF";
   else if (s=="sc7calo")
      return "SC5Calo";
   else if (s=="sc7pf")
      return "SC5PF";
   else if (s=="kt4calo")
      return "KT4Calo";
   else if (s=="kt4pf")
      return "KT4PF";
   else if (s=="kt6calo")
      return "KT6Calo";
   else if (s=="kt6pf")
      return "KT6PF";
   else if (s=="ak5calordl1")
      return "AK5CaloRDl1";
   else if (s=="ak5pfrdl1")
      return "AK5PFRDl1";
   else if (s=="ak5pfchsrdl1")
      return "AK5PFchsRDl1";
   else if (s=="ak7calordl1")
      return "AK7CaloRDl1";
   else if (s=="ak7pfrdl1")
      return "AK7PFRDl1";
   else if (s=="ak7pfchsrdl1")
      return "AK7PFchsRDl1";
   else
      return "unknown";
}

//______________________________________________________________________________
//coneSize = 0 the entire name
//coneSize = 1 no cone size
//coneSize = 2 only cone size
TString getAlgNameLong(TString algo, int coneSize) {
  TString algNameLong;

  if (coneSize<2)
    if(algo.Contains("ak"))        algNameLong += "Anti-kT";
  if(coneSize==0 || coneSize==2) {
     if(algo.Contains("ak1") && !algo.Contains("ak10")) algNameLong += " R=0.1";
    else if(algo.Contains("2"))    algNameLong += " R=0.2";
    else if(algo.Contains("3"))    algNameLong += " R=0.3";
    else if(algo.Contains("4"))    algNameLong += " R=0.4";
    else if(algo.Contains("5"))    algNameLong += " R=0.5";
    else if(algo.Contains("6"))    algNameLong += " R=0.6";
    else if(algo.Contains("7"))    algNameLong += " R=0.7";
    else if(algo.Contains("8"))    algNameLong += " R=0.8";
    else if(algo.Contains("9"))    algNameLong += " R=0.9";
    else if(algo.Contains("10"))   algNameLong += " R=1.0";
  }
  if(coneSize<2) {
    if(algo.Contains("pfchs"))     algNameLong += ", PF+CHS";
    //else if(algo.Contains("pf"))   algNameLong += ", PFlow";
    else if(algo.Contains("pf"))   algNameLong += ", Particle-Flow Jets";
    else if(algo.Contains("calo")) algNameLong += ", Calo";
    else if(algo.Contains("jpt"))  algNameLong += ", JPT";

    if(algo.Contains("rd")) algNameLong += ", (RD)";
  }

  return algNameLong;
}

//______________________________________________________________________________
vector<Int_t> getColors() {
  vector<Int_t> ret;
  ret.push_back(kBlack);
  ret.push_back(kRed);
  ret.push_back(kOrange+1);
  ret.push_back(kYellow+2);
  ret.push_back(kGreen);
  ret.push_back(kCyan);
  ret.push_back(kAzure+1);
  ret.push_back(kViolet+1);
  ret.push_back(kMagenta);
  ret.push_back(kGray+2);
  return ret;
}

//______________________________________________________________________________
vector<Int_t> getMarkerNumbers() {
  vector<Int_t> ret;
  ret.push_back(20);
  ret.push_back(33);
  ret.push_back(22);
  ret.push_back(21);
  ret.push_back(24);
  ret.push_back(27);
  ret.push_back(26);
  ret.push_back(25);
  ret.push_back(28);
  ret.push_back(34);
  return ret;
}

///////////////////////////////////////////////////////////////////
//
// jet_draw_closure_eta_ratio_x
// ----------------------------
//
//            09/01/2011 Alexx Perloff  <aperloff@physics.tamu.edu>
///////////////////////////////////////////////////////////////////

#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetAnalyzers/interface/Settings.h"
#include "JetMETAnalysis/JetUtilities/interface/Style.h"

#include "TROOT.h"
#include "TSystem.h"
#include "TMath.h"
#include "TStyle.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2.h"
#include "TH2F.h"
#include "TF1.h"
#include "TString.h"
#include "TPaveText.h"
#include "TLatex.h"
#include "TLegend.h"

#include <vector>
#include <string>
#include <iomanip>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// define local functions
////////////////////////////////////////////////////////////////////////////////

///CMS Preliminary label;
void cmsPrelim(double intLUMI = 0);

/// get the uppercase version of the algorithm name
TString getAlias(TString s);

/// transform the alg label into a title, e.g.: kt4calo -> k_{T}, D=0.4 (Calo)
string get_legend_title(const string& alg);

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv)
{
   gROOT->SetStyle("Plain");
   gStyle->SetOptStat(0);

   gSystem->Load("libFWCoreFWLite.so");
  
   //
   // evaluate command-line / configuration file options
   // 
   CommandLine cl;
   if (!cl.parse(argc,argv)) return 0;
  
   string          filepath1    = cl.getValue<string>   ("filepath1");
   string          filepath2    = cl.getValue<string>   ("filepath2");
   string          algo         = cl.getValue<string>   ("algo");
   string          numerator    = cl.getValue<string>   ("numerator",            "53X");
   string          denominator  = cl.getValue<string>   ("denominator",          "52X");
   bool            doflavor     = cl.getValue<bool>     ("doflavor",             false);
   TString         outputDir    = cl.getValue<TString>  ("outputDir",         "images");
   vector<TString> outputFormat = cl.getVector<TString> ("outputFormat", ".png:::.eps");
   bool            tdr          = cl.getValue<bool>     ("tdr",                  false);

   if (!cl.check()) return 0;
   cl.print();

   if (tdr) {
      setTDRStyle();
   }

   bool algDiv = false;
   if(getAlias(TString(numerator)).CompareTo("unknown") && getAlias(TString(denominator)).CompareTo("unknown")) {
      algo = numerator+"Over"+denominator;
      algDiv = true;
   }

   //
   // Open the files containing the original closure plots and retrieve the histograms
   //
   char name[1024];
   TFile* file1 = new TFile(filepath1.c_str(),"READ");
   vector<TH1F*> h1;
   vector<TH1F*> h1copy;
   for(int i=0;i<NPtBins;i++)                                                                            
   {                                                                                                   
      sprintf(name,"ClosureVsEta_RefPt%sto%s",Pt[i],Pt[i+1]);
      h1.push_back((TH1F*)gDirectory->Get(name));
      if(h1.back()==0)
         cout << "One or more of the histogram pointers from file " << filepath1 << " is NULL." << endl;
      h1copy.push_back((TH1F*)h1.back()->Clone());
   }
   sprintf(name,"ClosureVsEta_RefPt%sto%s",Pt[0],Pt[NPtBins]);
   h1.push_back((TH1F*)gDirectory->Get(name));
   h1copy.push_back((TH1F*)h1.back()->Clone());

   TFile* file2 = new TFile(filepath2.c_str(),"READ");
   vector<TH1F*> h2;
   for(int i=0;i<NPtBins;i++)                                                                            
   {                                                                                                   
      sprintf(name,"ClosureVsEta_RefPt%sto%s",Pt[i],Pt[i+1]);
      h2.push_back((TH1F*)gDirectory->Get(name));
      if(h2.back()==0)
         cout << "One or more of the histogram pointers from file " << filepath2 << " is NULL." << endl;
   }
   sprintf(name,"ClosureVsEta_RefPt%sto%s",Pt[0],Pt[NPtBins]);
   h2.push_back((TH1F*)gDirectory->Get(name));

   //
   // book histograms
   //
   vector<TH1F*> ratioHist;
   for(int i=0; i<NPtBins; i++) {
      sprintf(name,"ClosureVsEtaRatio_RefPt%sto%s",Pt[i],Pt[i+1]);
      ratioHist.push_back(new TH1F(name,name,h1[i]->GetNbinsX(),h1[i]->GetXaxis()->GetXmin(),h1[i]->GetXaxis()->GetXmax()));
   }
   sprintf(name,"ClosureVsEtaRatio_RefPt%sto%s",Pt[0],Pt[NPtBins]);
   ratioHist.push_back(new TH1F(name,name,h1.back()->GetNbinsX(),h1.back()->GetXaxis()->GetXmin(),h1.back()->GetXaxis()->GetXmax()));

   //
   // divide histograms
   //
   for(unsigned int i=0; i<h1copy.size(); i++) {
      h1copy[i]->Divide(h2[i]);
      ratioHist[i] = h1copy[i];
   }

   //
   // Create guides (lines) for the output histograms
   //
   TF1 *line = new TF1("line","0*x+1",-6,6);
   line->SetLineColor(1);
   line->SetLineWidth(1);
   line->SetLineStyle(2);
   TF1 *linePlus;
   if (tdr)
      linePlus = new TF1("linePlus","0*x+1.01",-6,6);
   else
      linePlus = new TF1("linePlus","0*x+1.02",-6,6);
   linePlus->SetLineColor(1);
   linePlus->SetLineWidth(1);
   linePlus->SetLineStyle(2);
   TF1 *lineMinus;
   if (tdr)
      lineMinus = new TF1("lineMinus","0*x+0.99",-6,6);
   else
      lineMinus = new TF1("lineMinus","0*x+0.98",-6,6);
   lineMinus->SetLineColor(1);
   lineMinus->SetLineWidth(1);
   lineMinus->SetLineStyle(2);
  
   TCanvas* can[NPtBins+1];
   TPaveText *pave[NPtBins+1];
   //string algo = filepath1.substr(int(filepath1.find("_")+1),int(filepath1.rfind("_")-filepath1.find("_")-1));
   TString flavor1, flavor2;
   if(doflavor)
   {
      flavor1 = filepath1.substr(int(filepath1.rfind("_")+1),int(filepath1.rfind(".root")-filepath1.rfind("_")-1));
      flavor2 = filepath2.substr(int(filepath2.rfind("_")+1),int(filepath2.rfind(".root")-filepath2.rfind("_")-1));
   }

   //
   // Open/create the output directory and file
   //
   if(!gSystem->OpenDirectory(outputDir)) gSystem->mkdir(outputDir);
   TString ofname = outputDir+"/ClosureVsEtaRatio_"+algo+".root";
   if(doflavor) ofname = outputDir+"/ClosureVsEtaRatio_"+algo+"_"+flavor1+"over"+flavor2+".root";
   TFile* outf = new TFile(ofname,"RECREATE");

   //
   // Format and save the output
   //
   for(int i=0; i<NPtBins+1; i++)
   {
      TString ss;
      if(!algDiv)
         pave[i] = new TPaveText(0.3,0.75,0.8,0.9,"NDC");
      else
         pave[i] = new TPaveText(0.3,0.65,0.8,0.9,"NDC");
      if (tdr) {
         pave[i]->AddText("QCD Monte Carlo");
         if(!algDiv) {
            //if(ss.Contains("pfchs"))
            //   pave[i]->AddText("Anti-kT R=0.5, PFlow+CHS");
            //else if(ss.Contains("pf"))
            //   pave[i]->AddText("Anti-kT R=0.5, PFlow");
            //else if(ss.Contains("calo"))
            //   pave[i]->AddText("Anti-kT R=0.5, Calo");
            //else if(ss.Contains("jpt"))
            //   pave[i]->AddText("Anti-kT R=0.5, JPT");
            pave[i]->AddText(get_legend_title(algo).c_str());
         }
         else {
            pave[i]->AddText(get_legend_title(numerator).c_str());
            pave[i]->AddText("Over");
            pave[i]->AddText(get_legend_title(denominator).c_str());
         }
      }
      else {
         pave[i]->AddText(algo.c_str());
      }
      if(i!=NPtBins)
      {
         pave[i]->AddText(TString(Pt[i])+" < p_{T}^{gen} < "+TString(Pt[i+1])+" GeV");
         ss = "ClosureVsEtaRatio_RefPt"+TString(Pt[i])+"to"+TString(Pt[i+1])+"_"+algo.c_str();
      }
      else
      {
         pave[i]->AddText(TString(Pt[0])+" < p_{T}^{gen} < "+TString(Pt[NPtBins])+" GeV");
         ss = "ClosureVsEtaRatio_RefPt"+TString(Pt[0])+"to"+TString(Pt[NPtBins])+"_"+algo.c_str();
      }
      if (doflavor) ss += "_"+flavor1+"over"+flavor2;

      can[i] = new TCanvas(ss,ss,800,800);

      ratioHist[i]->SetTitle(ss);
      ratioHist[i]->Draw();
      ratioHist[i]->GetXaxis()->SetTitle("#eta");
      //ratioHist[i]->GetYaxis()->SetTitle("Particle Over Anti-Particle Closure");
      ratioHist[i]->GetYaxis()->SetTitle("Response/Response Ratio");
      float etaMax = TMath::ACosH(3500./vpt[i]);
      if(i!=NPtBins)
         ratioHist[i]->GetXaxis()->SetRangeUser(-etaMax,etaMax);
      else
         ratioHist[i]->GetXaxis()->SetRangeUser(veta[0],veta[NETA]);
      ratioHist[i]->SetMaximum(1.1);
      ratioHist[i]->SetMinimum(0.9);
      ratioHist[i]->SetMarkerColor(kBlue);
      ratioHist[i]->SetLineColor(kBlue);
      ratioHist[i]->SetMarkerSize(1.5);
      ratioHist[i]->Draw();
      line->Draw("same");
      linePlus->Draw("same");
      lineMinus->Draw("same");
      ratioHist[i]->GetYaxis()->SetTitleOffset(1.5);
      if (tdr) {
         ratioHist[i]->GetXaxis()->SetTitleSize(0.058);
         ratioHist[i]->GetXaxis()->SetTitleOffset(0.95);
         ratioHist[i]->SetMarkerStyle(20);
         ratioHist[i]->SetMarkerSize(0.5);
      }
      else {
         ratioHist[i]->GetXaxis()->SetLabelSize(0.04);
         ratioHist[i]->GetYaxis()->SetLabelSize(0.04);             
         ratioHist[i]->SetMarkerSize(2.0);
          }      
      pave[i]->SetFillColor(0);
      pave[i]->SetBorderSize(0);
      pave[i]->SetTextFont(42);
      if(!algDiv)
         pave[i]->SetTextSize(0.05);
      else
         pave[i]->SetTextSize(0.04);
      pave[i]->Draw("EP");
      if (tdr) cmsPrelim();
      for(unsigned int f=0; f<outputFormat.size(); f++) {
        can[i]->SaveAs(outputDir+"/"+ss+outputFormat[f]);
      }
      ratioHist[i]->Write();
      can[i]->Write();
   }
   //
   // create overview canvas
   //
   TLegend* leg[6];
   TString ss("ClosureVsEtaRatio_Overview");
   ss+="_"+algo;
   if(doflavor) ss+="_"+flavor1+"over"+flavor2;
   
   TCanvas *ove = new TCanvas(ss,ss,1200,800);
   ove->Divide(3,2);
   int ca[6] = {1,11,15,19,23,27};
   for (int c=0;c<6;c++) {
      if (ca[c] > NPtBins-1) {
         cout<<"\tWRONG PT requested"<<endl;
         continue;
      }
      ove->cd(c+1);
      leg[c] = new TLegend(0.40,0.20,0.80,0.40);
      leg[c]->SetTextSize(0.04);//0.03);
      leg[c]->SetBorderSize(0);
      leg[c]->SetFillColor(0);
      ratioHist[ca[c]]->Draw("EP");
      ratioHist[ca[c]]->SetMarkerColor(kBlack);
      ratioHist[ca[c]]->SetLineColor(kBlack);

      h1[ca[c]]->SetMarkerColor(kBlue);
      h1[ca[c]]->SetLineColor(kBlue);
      h2[ca[c]]->SetMarkerColor(kRed);
      h2[ca[c]]->SetLineColor(kRed);
      h1[ca[c]]->Draw("EPsame");
      h2[ca[c]]->Draw("EPsame");
      leg[c]->AddEntry(h1[ca[c]],numerator.c_str(),"lep");
      leg[c]->AddEntry(h2[ca[c]],denominator.c_str(),"lep");

      leg[c]->AddEntry(ratioHist[ca[c]],"Ratio = #frac{"+TString(numerator)+"}{"+TString(denominator)+"}","lep");
      line->Draw("same");
      linePlus->Draw("same");
      lineMinus->Draw("same");
      pave[ca[c]]->Draw();
      leg[c]->Draw("same");
      if (tdr) cmsPrelim();
   }
   for(unsigned int f=0; f<outputFormat.size(); f++) {
     ove->SaveAs(outputDir+"/"+ss+outputFormat[f]);
   }
   ove->Write();
   outf->Close();
   file1->Close();
   file2->Close();
}

////////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void cmsPrelim(double intLUMI)
{
   const float LUMINOSITY = intLUMI;
   TLatex latex;
   latex.SetNDC();
   latex.SetTextSize(0.045);

   latex.SetTextAlign(31); // align right
   latex.DrawLatex(0.93,0.96,"#sqrt{s} = 8 TeV");
   if (LUMINOSITY > 0.) {
      latex.SetTextAlign(31); // align right
      //latex.DrawLatex(0.82,0.7,Form("#int #font[12]{L} dt = %d pb^{-1}", (int) LUMINOSITY)); //Original
      latex.DrawLatex(0.65,0.85,Form("#int #font[12]{L} dt = %d pb^{-1}", (int) LUMINOSITY)); //29/07/2011
   }
   latex.SetTextAlign(11); // align left
   latex.DrawLatex(0.16,0.96,"CMS preliminary");// 2012");
}

//______________________________________________________________________________
TString getAlias(TString s)
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
   else if (s=="ak5pfchs")
      return "AK5PFchs";
   else if (s=="ak5pfchsl1")
      return "AK5PFchsl1";
   else if (s=="ak5pfchsl1l2l3")
      return "AK5PFchsl1";
   else if (s=="ak5pfchsl1off")
      return "AK5PFchsl1off";
   else if (s=="ak7pfchs")
      return "AK7PFchs";
   else if (s=="ak7pfchsl1")
      return "AK7PFchsl1";
   else if (s=="ak7pfchsl1off")
      return "AK7PFchsl1off";
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
   else if (s=="ak5jptl1l2l3")
      return "AK5JPTl1";
   else if (s=="ak7jpt")
      return "AK7JPT";
   else if (s=="ak7jptl1")
      return "AK7JPTl1";
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
   else
      return "unknown";
}

//______________________________________________________________________________
string get_legend_title(const string& alg)
{
  string title;
  string tmp(alg);
  if      (alg.find("kt")==0) { title = "k_{T}, R=";      tmp = tmp.substr(2); }
  else if (alg.find("sc")==0) { title = "SISCone, R=";    tmp = tmp.substr(2); }
  else if (alg.find("ic")==0) { title = "ItCone, R=";     tmp = tmp.substr(2); }
  else if (alg.find("mc")==0) { title = "MidCone. R=";    tmp = tmp.substr(2); }
  else if (alg.find("ca")==0) { title = "Cam/Aachen, R="; tmp = tmp.substr(2); }
  else if (alg.find("ak")==0) { title = "Anti k_{T}, R="; tmp = tmp.substr(2); }
  else return alg;
  
  string reco[9] = { "gen", "caloHLT", "calo", "pfHLT", "pfchsHLT", "pfchs", "pf", "trk", "jpt" };
  string RECO[9] = { "(Gen)", "(Calo@HLT)", "(Calo)", "(PFlow@HLT)", "(PFlow+CHS@HLT)", "(PFlow+CHS)", "(PFlow)", "(Tracks)", "(JPT)" };

  string::size_type pos=string::npos; int ireco=-1;
  while (pos==string::npos&&ireco<8) { pos = tmp.find(reco[++ireco]); }
  if (pos==string::npos) return alg;
  
  double jet_size; stringstream ss1; ss1<<tmp.substr(0,pos); ss1>>jet_size;
  jet_size/=10.0;  stringstream ss2; ss2<<jet_size;

  title += ss2.str() + " " + RECO[ireco];

  return title;
}

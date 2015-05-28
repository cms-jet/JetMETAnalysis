/*
From Delphes_V1_MC

L1
eta = 0 to 0.087
pt = 1 to 3500
jetA = 0 to 10
rho = 0 to 200
Function = max(0.0001,1-y*([0]+([1]*z)*(1+[2]*log(x)))/x)
[0] = -5.0
[1] = 1.83984
[2] = -0.0618013

L2L3 
eta = 0 to 0.087
pt = 16.3013 to 1299.54
Function = (((([0]+([1]*log10(x)))+([2]*(log10(x)^2)))+([3]/(log10(x)^3)))+([4]/(log10(x)^4)))+([5]/(log10(x)^5))
[0] = 2.60518
[1] = -0.719682
[2] = 0.1
[3] = -24.1567
[4] = 57.0641
[5] = -36.5885


QCD Mean = 1.337
TT Mean = 1.29

TF3* L1 = new TF3("L1","max(0.0001,1-y*([0]+([1]*z)*(1+[2]*log(x)))/x)",1,3500,0,10,0,200)
L1->SetParameters(-5.0,1.83984,-0.0618013)
TF1* L2L3 = new TF1("L2L3","(((([0]+([1]*log10(x)))+([2]*(log10(x)^2)))+([3]/(log10(x)^3)))+([4]/(log10(x)^4)))+([5]/(log10(x)^5))",16.3013,1299.54)
L2L3->SetParameters(2.60518,-0.719682,0.1,-24.1567,57.0641,-36.5885)

*/

#include "TROOT.h"
#include "TSystem.h"
#include "TString.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TFile.h"
#include "TPad.h"
#include "TAxis.h"
#include "TLine.h"
#include "TH1F.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TPaveText.h"

#include <iostream>
#include <utility>
#include <assert.h>

using std::cout;
using std::endl;
using std::pair;
using std::make_pair;
using std::assert;

using namespace std;

void compareErasNoPU(bool PU = false, TString algo = "ak4pfchs", TString var = "resp", int ptlow = 20, int pthigh = 40,bool debug = false){
    TList* loc = (TList*)gROOT->GetListOfCanvases();
    TListIter itc(loc);
    TObject *o(0);
    while ((o = itc())) delete o;

    TList* loc2 = (TList*)gROOT->GetListOfFiles();
    TListIter itc2(loc2);
    while ((o = itc2())) ((TFile*)o)->Close();
   
    gROOT->LoadMacro("$CMSSW_BASE/src/JetMETAnalysis/JetAnalyzers/interface/Settings.h");
    setTDRStyle();

    cout << "Setting up strings ... " << endl;
    TString pu;
    if(PU)
       pu = "PU";
    else
       pu = "NoPU";
    pair<int,int> pt = make_pair(ptlow,pthigh); //20to40 and 30to50 and 200to300
    TString ptCut = Form("refpt>%i && refpt<%i",pt.first,pt.second);
    //TString ptCut = "jtpt<50 && jtpt>30";
    TString etaCut = "TMath::Abs(jteta)>0.00 && TMath::Abs(jteta)<0.087";
    //TString etaCut = "TMath::Abs(jteta)>1.83 && TMath::Abs(jteta)<1.93";
    //TString etaCut = "TMath::Abs(jteta)>2.65 && TMath::Abs(jteta)<2.853";
    TString refdrjtCut = "refdrjt<0.200";
    TString responseCut;
    if(var.CompareTo("mass")==0)
       responseCut = "";
    else
       responseCut = " && jtpt/refpt<4.0";
    TString debugCut;
    if(debug)
       debugCut = " && Entry$<1000";
    else
       debugCut = " && Entry$<1000000";
    TString toDraw;
    if(var.CompareTo("mass")==0)
       toDraw = "TMath::Sqrt(TMath::Abs(TMath::Power(jte,2)-(TMath::Power(jtpt*TMath::Cos(jtphi),2)+TMath::Power(jtpt*TMath::Sin(jtphi),2)+TMath::Power(jtpt*TMath::SinH(jteta),2))))-TMath::Sqrt(TMath::Abs(TMath::Power(refe,2)-(TMath::Power(refpt*TMath::Cos(refphi),2)+TMath::Power(refpt*TMath::Sin(refphi),2)+TMath::Power(refpt*TMath::SinH(refeta),2))))";
    else if(var.CompareTo("resp")==0)
       toDraw = "jtpt/refpt";
    else if(var.CompareTo("CHF")==0)
       toDraw = "jtchf*jtpt/refpt";
    else if(var.CompareTo("NHF")==0)
       toDraw = "jtnhf*jtpt/refpt";
    else if(var.CompareTo("NEF")==0)
       toDraw = "jtnef*jtpt/refpt";
    else if(var.CompareTo("respCrossCheck")==0)
       toDraw = "((jtchf+jtnhf+jtnef)*jtpt)/refpt";
    else {
       cout << "Unknown variable!!!" << endl;
       return;
    }

    cout << "Doing 74X ... " << endl;
    TFile* f74;
    if(!PU)
       f74 = TFile::Open("/home/aperloff/JEC/CMSSW_7_4_0_pre9/src/JetMETAnalysis/JetAnalyzers/test/JRA_HS.root","READ");
    else
       return;
       //f74 = TFile::Open("/home/aperloff/JEC/CMSSW_7_4_0_pre9/src/JetMETAnalysis/JetAnalyzers/test/JRA_HS.root","READ");
    f74->cd(algo);
    TTree* t74 = (TTree*)gDirectory->Get("t");
    if(!t74) return;
    TH1F* raw74;
    if(var.CompareTo("mass")==0)
       raw74 = new TH1F("raw74","raw74",40,-20,20);
    else if(var.CompareTo("resp")==0)
       raw74 = new TH1F("raw74","raw74",200,0,4);
    else if(var.CompareTo("CHF")==0)
       raw74 = new TH1F("raw74","raw74",100,0,2);
    else if(var.CompareTo("NHF")==0)
       raw74 = new TH1F("raw74","raw74",100,0,2);
    else if(var.CompareTo("NEF")==0)
       raw74 = new TH1F("raw74","raw74",100,0,2);
    else if(var.CompareTo("respCrossCheck")==0)
       raw74 = new TH1F("raw74","raw74",100,0,2);
    raw74->Sumw2();
    t74->Draw(toDraw+">>raw74",etaCut+" && "+ptCut+" && "+refdrjtCut+responseCut+debugCut,"geoff");

    cout << "Doing 73X ... " << endl;
    TFile* f73;
    if(!PU)
       f73 = TFile::Open("/fdata/hepx/store/user/aperloff/JRA_outfiles_73X_20150406_QCD_Pt15To7000_Fall14DR73_NoPU/QCD_Pt-15TTo7000_TuneZ2star-Flat_13TeV_pythia6/QCD_Pt15To7000_Fall14DR73_NoPU/150406_214503/0000/JRA/JRA.root","READ");
    else
       return;
       //f73 = TFile::Open("/home/aperloff/JEC/CMSSW_7_4_0_pre9/src/JetMETAnalysis/JetAnalyzers/test/JRA_HS.root","READ");
    f73->cd(algo);
    TTree* t73 = (TTree*)gDirectory->Get("t");
    if(!t73) return;
    TH1F* raw73;
    if(var.CompareTo("mass")==0)
       raw73 = new TH1F("raw73","raw73",40,-20,20);
    else if(var.CompareTo("resp")==0)
       raw73 = new TH1F("raw73","raw73",200,0,4);
    else if(var.CompareTo("CHF")==0)
       raw73 = new TH1F("raw73","raw73",100,0,2);
    else if(var.CompareTo("NHF")==0)
       raw73 = new TH1F("raw73","raw73",100,0,2);
    else if(var.CompareTo("NEF")==0)
       raw73 = new TH1F("raw73","raw73",100,0,2);
    else if(var.CompareTo("respCrossCheck")==0)
       raw73 = new TH1F("raw73","raw73",100,0,2);
    raw73->Sumw2();
    t73->Draw(toDraw+">>raw73",etaCut+" && "+ptCut+" && "+refdrjtCut+responseCut+debugCut,"geoff");

    cout << "Doing 72X ... " << endl;
    TFile* f72;
    if(!PU)
       f72 = TFile::Open("/home/aperloff/fdata/JRA_outfiles_72X_20141202_QCD_Pt15to3000_NoPU_PHYS14_beforeL1/JRA/JRA.root","READ");
    else
       f72 = TFile::Open("/home/aperloff/fdata/JRA_outfiles_72X_20141202_QCD_Pt15to3000_PU20bx25_PHYS14_beforeL1/JRA/JRA.root","READ");
    f72->cd(algo);
    TTree* t72 = (TTree*)gDirectory->Get("t");
    if(!t72) return;
    TH1F* raw72;
    if(var.CompareTo("mass")==0)
       raw72 = new TH1F("raw72","raw72",40,-20,20);
    else if(var.CompareTo("resp")==0)
       raw72 = new TH1F("raw72","raw72",200,0,4);
    else if(var.CompareTo("CHF")==0)
       raw72 = new TH1F("raw72","raw72",100,0,2);
    else if(var.CompareTo("NHF")==0)
       raw72 = new TH1F("raw72","raw72",100,0,2);
    else if(var.CompareTo("NEF")==0)
       raw72 = new TH1F("raw72","raw72",100,0,2);
    else if(var.CompareTo("respCrossCheck")==0)
       raw72 = new TH1F("raw72","raw72",100,0,2);
    raw72->Sumw2();
    t72->Draw(toDraw+">>raw72",etaCut+" && "+ptCut+" && "+refdrjtCut+responseCut+debugCut,"geoff");
    
    cout << "Doing 53X ... " << endl;
    TFile* f53;
    if(!PU && algo.Contains("chs"))
       f53 = TFile::Open("/home/aperloff/fdata/JRA_outfiles_53X_20140526_NoPileup_pbs/JRA/JRA.root","READ");
    else if(!PU)
       f53 = TFile::Open("/home/aperloff/fdata/JRA_outfiles_53X_20140529_NoPileup_pbs/JRA/JRA.root","READ");
    else if(PU && algo.Contains("chs"))
       f53 = TFile::Open("/home/aperloff/fdata/JRA_outfiles_53X_20140526_pbs/JRA/JRA.root","READ");
    else if()
       f53 = TFile::Open("/home/aperloff/fdata/JRA_outfiles_53X_20140529_pbs/JRA/JRA.root","READ");
    else 
       return;
    f53->cd(algo);
    TTree* t53 = (TTree*)gDirectory->Get("t");
    TH1F* raw53;
    if(var.CompareTo("mass")==0)
       raw53 = new TH1F("raw53","raw53",40,-20,20);
    else if(var.CompareTo("resp")==0)
       raw53 = new TH1F("raw53","raw53",200,0,4);
    else if(var.CompareTo("CHF")==0)
       raw53 = new TH1F("raw53","raw53",100,0,2);
    else if(var.CompareTo("NHF")==0)
       raw53 = new TH1F("raw53","raw53",100,0,2);
    else if(var.CompareTo("NEF")==0)
       raw53 = new TH1F("raw53","raw53",100,0,2);
    else if(var.CompareTo("respCrossCheck")==0)
       raw53 = new TH1F("raw53","raw53",100,0,2);
    raw53->Sumw2();
    t53->Draw(toDraw+">>raw53",etaCut+" && "+ptCut+" && "+refdrjtCut+responseCut+debugCut,"geoff");

    cout << "Drawing canvas ... " << endl;
    TH1D* frame = new TH1D();
    if(var.CompareTo("mass")==0) {
       frame->GetXaxis()->SetLimits(-20,20);
       frame->GetYaxis()->SetRangeUser(0,0.4);
       frame->GetXaxis()->SetTitle("m^{RAW}-m^{GEN}");
    }
    else if(var.CompareTo("resp")==0){
       frame->GetXaxis()->SetLimits(0.4,1.4);
       frame->GetYaxis()->SetRangeUser(0,0.25);
       frame->GetXaxis()->SetTitle("Response = p_{T}^{RAW}/p_{T}^{GEN}");
    }
    else if(var.CompareTo("CHF")==0) {
       frame->GetXaxis()->SetLimits(0,1.4);
       frame->GetYaxis()->SetRangeUser(0,0.25);
       frame->GetXaxis()->SetTitle("CHF^{RAW}*p_{T}^{RAW}/p_{T}^{GEN}");
    }
    else if(var.CompareTo("NHF")==0) {
       frame->GetXaxis()->SetLimits(0,1.4);
       frame->GetYaxis()->SetRangeUser(0,0.6);
       frame->GetXaxis()->SetTitle("NHF^{RAW}*p_{T}^{RAW}/p_{T}^{GEN}");
    }
    else if(var.CompareTo("NEF")==0) {
       frame->GetXaxis()->SetLimits(0,1.4);
       frame->GetYaxis()->SetRangeUser(0,0.25);
       frame->GetXaxis()->SetTitle("NEF^{RAW}*p_{T}^{RAW}/p_{T}^{GEN}");
    }
    else if(var.CompareTo("respCrossCheck")==0) {
       frame->GetXaxis()->SetLimits(0,1.4);
       frame->GetYaxis()->SetRangeUser(0,0.25);
       frame->GetXaxis()->SetTitle("(CHF^{RAW}+NHF^{RAW}+NEF^{RAW})*p_{T}^{RAW}/p_{T}^{GEN}");
    }
    frame->GetYaxis()->SetTitle("a.u.");
    TCanvas* c = tdrCanvas("c",frame,4,11,true);

    f74->cd(algo);
    raw74->Scale(1.0/raw74->Integral());
    tdrDraw(raw74,"hist",kDot,kOrange,kSolid,kOrange,0,0);
    TLine* rawline74 = new TLine(raw74->GetMean(),gPad->GetUymin(),raw74->GetMean(),gPad->GetUymax());
    rawline74->SetLineColor(kOrange);
    rawline74->SetLineWidth(3);
    pair<double,double> meanRMS74 = make_pair(raw74->GetMean(),raw74->GetRMS());

    f73->cd(algo);
    raw73->Scale(1.0/raw73->Integral());
    tdrDraw(raw73,"hist",kDot,kGreen,kSolid,kGreen,0,0);
    TLine* rawline73 = new TLine(raw73->GetMean(),gPad->GetUymin(),raw73->GetMean(),gPad->GetUymax());
    rawline73->SetLineColor(kGreen);
    rawline73->SetLineWidth(3);
    pair<double,double> meanRMS73 = make_pair(raw73->GetMean(),raw73->GetRMS());

    f72->cd(algo);
    raw72->Scale(1.0/raw72->Integral());
    tdrDraw(raw72,"hist",kDot,kRed,kSolid,kRed,0,0);
    TLine* rawline72 = new TLine(raw72->GetMean(),gPad->GetUymin(),raw72->GetMean(),gPad->GetUymax());
    rawline72->SetLineColor(kRed);
    rawline72->SetLineWidth(3);
    pair<double,double> meanRMS72 = make_pair(raw72->GetMean(),raw72->GetRMS());

    f53->cd(algo);
    raw53->Scale(1.0/raw53->Integral());
    tdrDraw(raw53,"hist",kDot,kBlue,kSolid,kBlue,0,0);
    TLine* rawline53 = new TLine(raw53->GetMean(),gPad->GetUymin(),raw53->GetMean(),gPad->GetUymax());
    rawline53->SetLineColor(kBlue);
    rawline53->SetLineWidth(3);
    pair<double,double> meanRMS53 = make_pair(raw53->GetMean(),raw53->GetRMS());

    rawline74->Draw("same");
    rawline73->Draw("same");
    rawline72->Draw("same");
    rawline53->Draw("same");

    TLegend* leg = tdrLeg(0.33,0.55,0.78,1-gPad->GetTopMargin()-0.045*(1-gPad->GetTopMargin()-gPad->GetBottomMargin())+0.01);
    if(algo.Contains("chs"))
       leg->AddEntry((TObject*)0,"Anti-k_{T} R=0.4, PF+CHS","");
    else
       leg->AddEntry((TObject*)0,"Anti-k_{T} R=0.4, PF","");
    leg->AddEntry((TObject*)0,"|#eta|<0.087","");
    //leg->AddEntry((TObject*)0,"200 GeV < p_{T}^{RAW} < 300 GeV","");
    //leg->AddEntry((TObject*)0,"30 GeV < p_{T}^{RAW} < 50 GeV","");
    leg->AddEntry((TObject*)0,Form("%i GeV < p_{T}^{GEN} < %i GeV",pt.first,pt.second),"");
    leg->AddEntry(raw53,"53X "+pu+" Sample","l");
    leg->AddEntry(raw72,"72X "+pu+" Sample","l");
    leg->AddEntry(raw73,"73X "+pu+" Sample","l");
    leg->AddEntry(raw74,"74X "+pu+" Sample","l");
    leg->Draw("same");

    TPaveText* pave = tdrText(0.7,0.35,0.85,0.55);
    pave->AddText("Sample = <R>#pm#sigma");
    cout << "Mean 53 = " << meanRMS53.first << endl;
    cout << "RMS 53 = " << meanRMS53.second << endl;
    cout << "Mean 72 = " << meanRMS72.first << endl;
    cout << "RMS 72 = " << meanRMS72.second << endl;
    cout << "Mean 73 = " << meanRMS73.first << endl;
    cout << "RMS 73 = " << meanRMS73.second << endl;
    cout << "Mean 74 = " << meanRMS74.first << endl;
    cout << "RMS 74 = " << meanRMS74.second << endl;
    pave->AddText(Form("53X = %.3f#pm%.3f",meanRMS53.first,meanRMS53.second));
    pave->AddText(Form("72X = %.3f#pm%.3f",meanRMS72.first,meanRMS72.second));
    pave->AddText(Form("73X = %.3f#pm%.3f",meanRMS73.first,meanRMS73.second));
    pave->AddText(Form("74X = %.3f#pm%.3f",meanRMS74.first,meanRMS74.second));
    pave->Draw("same");

    cout << "Saving canvas ... " << endl;
    if(var.CompareTo("mass")==0)
       c->SaveAs(Form("%sMass_7XYVs53X_pTGen%ito%i_%s.pdf",pu.Data(),pt.first,pt.second,algo.Data()));
    else if(var.CompareTo("resp")==0)
       c->SaveAs(Form("%sResponse_7XYVs53X_pTGen%ito%i_%s.pdf",pu.Data(),pt.first,pt.second,algo.Data()));
    else if(var.CompareTo("CHF")==0)
       c->SaveAs(Form("%sCHF_7XYVs53X_pTGen%ito%i_%s.pdf",pu.Data(),pt.first,pt.second,algo.Data()));
    else if(var.CompareTo("NHF")==0)
       c->SaveAs(Form("NHF_7XYVs53X_pTGen%ito%i_%s.pdf",pu.Data(),pt.first,pt.second,algo.Data()));
    else if(var.CompareTo("NEF")==0)
       c->SaveAs(Form("NEF_7XYVs53X_pTGen%ito%i_%s.pdf",pu.Data(),pt.first,pt.second,algo.Data()));
    else if(var.CompareTo("respCrossCheck")==0)
       c->SaveAs(Form("RespCrossCheck_7XYVs53X_pTGen%ito%i_%s.pdf",pu.Data(),pt.first,pt.second,algo.Data()));
}

void responseByHand() {
    TString ptCut = "jtpt<300 && jtpt>200";
//TString ptCut = "jtpt<2500 && jtpt>1500";
    gStyle->SetOptStat(2222);
    gDirectory->cd("ak4pf");
    //ak4pf->cd();
    TCanvas* c = new TCanvas("c","c",1800,600);
    c->Divide(3,1);
//Uncorrected
    c->cd(1);
    TH1F* raw = new TH1F("raw","",100,0,2);
    TTree* t = (TTree*)gDirectory->Get("t");
    t->Draw("jtpt/refpt>>raw","jteta<0.087 && jteta>0.0 && "+ptCut+" && refdrjt<0.200 && jtpt/refpt<2.0","n");
    raw->Draw();
    raw->GetXaxis()->SetTitleOffset(1.2);
    raw->GetXaxis()->SetTitle("Response = p_{T}^{RAW}/p_{T}^{GEN}");
    raw->GetYaxis()->SetTitleOffset(1.5);
    raw->GetYaxis()->SetTitle("Jets");
    TLine* rawline = new TLine(raw->GetMean(),gPad->GetUymin(),raw->GetMean(),gPad->GetUymax());
    rawline->SetLineColor(kRed);
    rawline->SetLineWidth(3);
    rawline->Draw("same");
//L1 Corrected
    c->cd(2);
    TH1F *l1cor = new TH1F("l1cor","",100,0,2);
    t->Draw("(max(0.0001,1-jtarea*(-5.0+(1.83984*rho)*(1+-0.0618013*log(jtpt)))/jtpt)*jtpt)/refpt>>l1cor","jteta<0.087 && jteta>0.0 && "+ptCut+" && refdrjt<0.200 && jtpt/refpt<2.0");
    l1cor->GetXaxis()->SetTitleOffset(1.2);
    l1cor->GetXaxis()->SetTitle("Response (L1 Corrected)");
    l1cor->GetYaxis()->SetTitleOffset(1.5);
    l1cor->GetYaxis()->SetTitle("Jets");
    TLine* l1line = new TLine(l1cor->GetMean(),gPad->GetUymin(),l1cor->GetMean(),gPad->GetUymax());
    l1line->SetLineColor(kRed);
    l1line->SetLineWidth(3);
    l1line->Draw("same");
//L1L2L3 corrected
    c->cd(3);
    TH1F *l1l2l3cor = new TH1F("l1l2l3cor","",100,0,2);
    t->Draw("((((((2.60518+(-0.719682*log10((max(0.0001,1-jtarea*(-5.0+(1.83984*rho)*(1+-0.0618013*log(jtpt)))/jtpt)*jtpt))))+(0.1*(log10((max(0.0001,1-jtarea*(-5.0+(1.83984*rho)*(1+-0.0618013*log(jtpt)))/jtpt)*jtpt))^2)))+(-24.1567/(log10((max(0.0001,1-jtarea*(-5.0+(1.83984*rho)*(1+-0.0618013*log(jtpt)))/jtpt)*jtpt))^3)))+(57.0641/(log10((max(0.0001,1-jtarea*(-5.0+(1.83984*rho)*(1+-0.0618013*log(jtpt)))/jtpt)*jtpt))^4)))+(-36.5855/(log10((max(0.0001,1-jtarea*(-5.0+(1.83984*rho)*(1+-0.0618013*log(jtpt)))/jtpt)*jtpt))^5)))*(max(0.0001,1-jtarea*(-5.0+(1.83984*rho)*(1+-0.0618013*log(jtpt)))/jtpt)*jtpt))/refpt>>l1l2l3cor","jteta<0.087 && jteta>0.0 && "+ptCut+" && refdrjt<0.200 && jtpt/refpt<2.0");
    l1l2l3cor->GetXaxis()->SetTitleOffset(1.2);
    l1l2l3cor->GetXaxis()->SetTitle("Response (L1L2L3 Corrected)");
    l1l2l3cor->GetYaxis()->SetTitleOffset(1.5);
    l1l2l3cor->GetYaxis()->SetTitle("Jets");
    TLine* l1l2l3line = new TLine(l1l2l3cor->GetMean(),gPad->GetUymin(),l1l2l3cor->GetMean(),gPad->GetUymax());
    l1l2l3line->SetLineColor(kRed);
    l1l2l3line->SetLineWidth(3);
    l1l2l3line->Draw("same");
    c->SaveAs("QCD.pdf");
//c->SaveAs("tt_HT1.pdf")
//c->SaveAs("tt_HT5_lowPt.pdf")
//c->SaveAs("tt_HT5_highPt.pdf")
}

#include "TFile.h"
#include "TCanvas.h"
#include "TProfile.h"
#include "TLegend.h"
#include "TTree.h"
#include "TLatex.h"
#include "TPaveText.h"

#include "JetMETAnalysis/JetUtilities/interface/JRAEvent.h"
#include "JetMETAnalysis/JetUtilities/interface/Style.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

using std::cout;
using std::endl;
using std::vector;
using std::min;
using std::setw;

static int canvasNumber = 0;

void loadbar2(unsigned int x, unsigned int n, unsigned int w = 50) {
  if ( (x != n) && (x % (n/100) != 0) ) return;

  float ratio  =  x/(float)n;
  int   c      =  ratio * w;

  cout << setw(3) << (int)(ratio*100) << "% [";
  for (int x=0; x<c; x++) cout << "=";
  for (unsigned int x=c; x<w; x++) cout << " ";
  cout << "] (" << x << "/" << n << ")\r" << flush;
}

void getPtBinning(double ptbins[],int nbins){

  // create the ptbinning array.
  int i_i=0, i_f=nbins-1;
  double pt_i=20, pt_f=2000;
  double b = (i_i-i_f)/log(pt_i/pt_f);
  double a = i_f - b*log(pt_f);
  for(int i_pt = i_i ; i_pt <= i_f; i_pt++)
    ptbins[i_pt] = exp((i_pt-a)/b);

}//getPtBinning



void cmsPrelim(double intLUMI=0) {
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
  latex.DrawLatex(0.16,0.96,"CMS Simulation");

  latex.SetTextSize(0.035);
  latex.DrawLatex(0.424,0.879,"QCD Monte Carlo");
  latex.DrawLatex(0.397,0.835,"Anti-k_{T} R=0.5, PF+CHS");
  latex.SetTextSize(0.035);
  latex.DrawLatex(0.493,0.793,"|#eta| < 1.3");
}

void setTreeBranches(TTree * t, JRAEvent * ev) {

  t->SetBranchAddress("npus", &ev->npus);
  t->SetBranchAddress("tnpus", &ev->tnpus);
  t->SetBranchAddress("bxns", &ev->bxns);
  t->SetBranchAddress("sumpt_lowpt", &ev->sumpt_lowpt);
  t->SetBranchAddress("sumpt_highpt", &ev->sumpt_highpt);
  t->SetBranchAddress("ntrks_lowpt", &ev->ntrks_lowpt);
  t->SetBranchAddress("ntrks_highpt", &ev->ntrks_highpt);
  t->SetBranchAddress("rho", &ev->rho);
  t->SetBranchAddress("rho50", &ev->rho50);
  t->SetBranchAddress("rho_hlt", &ev->rho_hlt);
  t->SetBranchAddress("pthat", &ev->pthat);
  t->SetBranchAddress("weight", &ev->weight);
  t->SetBranchAddress("npv", &ev->npv);
  t->SetBranchAddress("evt", &ev->evt);
  t->SetBranchAddress("run", &ev->run);
  t->SetBranchAddress("nref", &ev->nref);
  t->SetBranchAddress("refrank", &ev->refrank);
  t->SetBranchAddress("refpdgid", &ev->refpdgid);
  t->SetBranchAddress("refe", &ev->refe);
  t->SetBranchAddress("refpt", &ev->refpt);
  t->SetBranchAddress("refeta", &ev->refeta);
  t->SetBranchAddress("refphi", &ev->refphi);
  t->SetBranchAddress("refy", &ev->refy);
  t->SetBranchAddress("refdrjt", &ev->refdrjt);
  t->SetBranchAddress("refarea", &ev->refarea);
  t->SetBranchAddress("jte", &ev->jte);
  t->SetBranchAddress("jtpt", &ev->jtpt);
  t->SetBranchAddress("jteta", &ev->jteta);
  t->SetBranchAddress("jtphi", &ev->jtphi);
  t->SetBranchAddress("jty", &ev->jty);
  t->SetBranchAddress("jtjec", &ev->jtjec);
  t->SetBranchAddress("jtarea", &ev->jtarea);
  t->SetBranchAddress("jtchf", &ev->jtchf);
  t->SetBranchAddress("jtnhf", &ev->jtnhf);
  t->SetBranchAddress("jtnef", &ev->jtnef);
  t->SetBranchAddress("jtcef", &ev->jtcef);
  t->SetBranchAddress("jtmuf", &ev->jtmuf);
  t->SetBranchAddress("jthfhf", &ev->jthfhf);
  t->SetBranchAddress("jthfef", &ev->jthfef);

}// setTreeBranches;


void plotProfiles(vector<TProfile*> profs, TString yAxisTitle, TProfile* pNoPU){

  // Create the canvas
  TString cn = Form("Can%i",canvasNumber);
  //TCanvas * c = new TCanvas(cn,cn,600,600);
  TH1D* frame = new TH1D();
  frame->GetXaxis()->SetLimits(20,2000);
  frame->GetXaxis()->SetTitle("p_{T}^{GEN} [GeV]");
  frame->GetXaxis()->SetMoreLogLabels();
  frame->GetXaxis()->SetNoExponent();
  frame->GetYaxis()->SetRangeUser(0.8,1.7);
  frame->GetYaxis()->SetTitle(yAxisTitle);
  TCanvas * c = tdrCanvas(cn,frame,2,11,true);

  //Format the noPU profile
  if(pNoPU) {
     pNoPU->SetLineColor(kBlack);
     pNoPU->SetMarkerColor(kBlack);
     pNoPU->SetMarkerStyle(kOpenCircle);
     pNoPU->SetMarkerSize(1.0);
  }

  //Plot the profiles
  //TLegend * leg = new TLegend(0.5,0.437,0.875,0.762);
  TLegend * leg = tdrLeg(0.345,0.427,0.825,0.91);
  leg->AddEntry((TObject*)0,"QCD Monte Carlo","");
  leg->AddEntry((TObject*)0,"Anti-k_{T} R=0.5, PF+CHS","");
  leg->AddEntry((TObject*)0,"|#eta| < 1.3","");
  if(pNoPU) leg->AddEntry(pNoPU,pNoPU->GetTitle(),"lpe");
  for (int p = 0 ; p < (int) profs.size();p++){
    int col = p+1;
    if (col==5 ) col = 38; // skip the yellow color
    profs[p]->SetLineColor(col);
    profs[p]->SetMarkerColor(col);
    profs[p]->SetMarkerStyle(kFullCircle);
    profs[p]->SetMarkerSize(1.0);
    /*
    if (p == 0){
      profs[p]->GetYaxis()->SetRangeUser(0.7,2);
      profs[p]->GetYaxis()->SetTitle(yAxisTitle);
      profs[p]->GetXaxis()->SetTitle("p_{T}^{GEN}");
      profs[p]->Draw();
    }else
      profs[p]->Draw("same");
    */
    tdrDraw(profs[p],"",kFullCircle,col,kSolid,col);

    leg->AddEntry(profs[p],profs[p]->GetTitle(),"lpe");
  }//for
  //pNoPU->Draw("same");
  if(pNoPU) tdrDraw(pNoPU,"",kOpenCircle,kBlack,kSolid,kBlack);

  leg->SetFillColor(0);
  leg->SetBorderSize(0);
  leg->Draw("same");
  
  // Plot the "CMS preliminary 8TeV", etc. 
  //cmsPrelim();

  c->SetLogx();
  c->Print(cn+".eps");
  c->SaveAs(cn+".C");
  c->SaveAs(cn+".pdf");
  c->SaveAs(cn+".png");
  c->Write();

  canvasNumber ++;


}//plotProfiles


void fillProfiles(TFile * file, TString dir, const vector<int> & npu, vector<TProfile*> profs){

  cout<<" Filling profiles "<<dir<<endl;

  // cd to the proper dir
  if (!file->cd(dir)){
    cout<<"ERROR Could not cd into directory "<<dir<<" in file "<<file->GetName()<<endl;
    return;
  }

  //Get the tree and fill the profiles
  TTree * t = (TTree *) gDirectory->Get("t");
  JRANtuple * ev = new JRANtuple();
  setTreeBranches(t,ev);
  
  // Fill the Tree, looping over events
  unsigned int nev = t->GetEntries();
  //nev = min(nev, (unsigned int) 10000);
  for (unsigned int e=0 ; e < nev ;e++){
    loadbar2(e+1,nev);
    t->GetEntry(e);
    int iIT = ev->itIndex();
    for (int n=0 ; n< (int) npu.size()-1; n++){

      // Report
      //cout<<"\t "<<npu[n]<<" < npv/rho < "<<npu[n+1]<<endl;

      // Loop over jets in the event
      for (int j=0;j<ev->nref;j++){
       if (ev->refpt[j] >10 && ev->refdrjt[j] < 0.25 && fabs(ev->jteta[j]) < 1.3 
//           && ev->npv >= npu[n] && ev->npv < npu[n+1])
           && ev->tnpus->at(iIT) >= npu[n] && ev->tnpus->at(iIT) < npu[n+1])
         profs[n]->Fill(ev->refpt[j], ev->jtpt[j]/ev->refpt[j]);
      }// for jets
    }// for entries

  }// for profiles
  delete ev;
  
}//fillProfiles

 
void simpleResponsePlots(){

  setTDRStyle();

  TFile *file0 = TFile::Open("/fdata/hepx/store/user/aperloff/JRA_outfiles_53X_20140526_pbs/JRA/JRA.root"); // ak5pfchs RD
  TFile *file1 = TFile::Open("/fdata/hepx/store/user/aperloff/JRA_outfiles_53X_20140531_pbs/JRA/JRA.root"); //ak5pfchsl1 RD
  TFile *file2 = TFile::Open("/fdata/hepx/store/user/aperloff/JRA_outfiles_53X_20140526_NoPileup_pbs/JRA/JRA.root"); // ak5pfchs NoPU RD
  TFile *file3 = TFile::Open("/fdata/hepx/store/user/aperloff/JRA_outfiles_53X_20140531_pbs/JRA/JRA_jec.root"); // ak5pfchsl1l2l3 RD
  TFile *file4 = TFile::Open("/fdata/hepx/store/user/aperloff/JRA_outfiles_53X_20140526_NoPileup_pbs/JRA/JRA_jec.root"); // ak5pfchsl2l3 NoPU RD
  TFile *outFile = new TFile("outputA.root","RECREATE");

  // Vector of npu
  vector<int> npu;
  for (int n=0; n<41; n = n+10)  
    npu.push_back(n);
  vector<int> npuNoPU;
  //npuNoPU.push_back(1);
  //npuNoPU.push_back(2);
  npuNoPU.push_back(0);
  npuNoPU.push_back(1);


  // create the ptbinning array.
  const int nbins=40;
  double ptbins[nbins+1];
  getPtBinning(ptbins,nbins+1);

  // Create the profiles
  vector<TProfile *> profs;
  vector<TProfile *> profsl1;
  vector<TProfile *> profsl123;
  vector<TProfile *> profs_NoPU;
  vector<TProfile *> profsl2l3_NoPU;
  for (int n=0 ; n< (int) npu.size()-1; n++){
    TString pn= Form("Prof%ito%i",npu[n],npu[n+1]);
    //TString pt= Form("%i < N_{PV} < %i",npu[n],npu[n+1]);
    TString pt= Form("%i < #mu < %i",npu[n],npu[n+1]);
    //TString pt= Form("%i < #rho < %i",npu[n],npu[n+1]);
    TProfile * aux0 = new TProfile(pn,pt,nbins,ptbins);
    profs.push_back(aux0);
    TProfile * aux1 = new TProfile(pn+"l1",pt,nbins,ptbins);
    profsl1.push_back(aux1);
    TProfile * aux123 = new TProfile(pn+"l123",pt,nbins,ptbins);
    profsl123.push_back(aux123);
  }//for
  for (int n=0 ; n< (int) npuNoPU.size()-1; n++){
    TString pn= Form("Prof%ito%i",npuNoPU[n],npuNoPU[n+1]);
    //TString pt= "No Pileup (N_{PV} = 1)";
    TString pt= "No Pileup (#mu = 0)";
    //TString pt= Form("%i < #rho < %i",npu[n],npu[n+1]);
    TProfile * aux0 = new TProfile(pn,pt,nbins,ptbins);
    profs_NoPU.push_back(aux0);
    TProfile * aux1 = new TProfile(pn+"l2l3",pt,nbins,ptbins);
    profsl2l3_NoPU.push_back(aux1);
  }//for

  fillProfiles(file0, "/ak5pfchs", npu, profs);
  fillProfiles(file1, "/ak5pfchsl1", npu, profsl1);
  fillProfiles(file3, "/ak5pfchsl1l2l3", npu, profsl123);
  fillProfiles(file2, "/ak5pfchs", npuNoPU, profs_NoPU);
  fillProfiles(file4, "/ak5pfchsl2l3", npuNoPU, profsl2l3_NoPU);
  
  // Plot the profiles
  outFile->cd();
  plotProfiles(profs,"Response",profs_NoPU[0]);
  plotProfiles(profsl1,"Pileup-Corrected Response",profs_NoPU[0]);
  plotProfiles(profsl123,"Corrected Response",profsl2l3_NoPU[0]);

  //Close the file
  outFile->Write();
  outFile->Close();

}//a 

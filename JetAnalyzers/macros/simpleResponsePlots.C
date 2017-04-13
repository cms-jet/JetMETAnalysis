#include "TFile.h"
#include "TCanvas.h"
#include "TProfile.h"
#include "TLegend.h"
#include "TTree.h"
#include "TLatex.h"
#include "TPaveText.h"
#include "TLine.h"

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

void eraseBins(TProfile* p, double threshold) {
   for(int iBin=1; iBin<=p->GetNbinsX(); iBin++) {
      if(p->GetBinContent(iBin)>threshold)
         p->SetBinContent(iBin,-9999);
   }
}

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

void plotProfiles(vector<TProfile*> profs, TString yAxisTitle, TProfile* pNoPU, TString suffix){

  // Create the canvas
   TString cn = Form("Can%i_%s",canvasNumber,suffix.Data());
  //TCanvas * c = new TCanvas(cn,cn,600,600);
  TH1D* frame = new TH1D();
  frame->GetXaxis()->SetLimits(20,2000);
  frame->GetXaxis()->SetTitle("p_{T}^{GEN} (GeV)");
  frame->GetXaxis()->SetMoreLogLabels();
  frame->GetXaxis()->SetNoExponent();
  frame->GetYaxis()->SetRangeUser(0.7,2.0);
  frame->GetYaxis()->SetTitle(yAxisTitle);
  TCanvas * c = tdrCanvas(cn,frame,15,11,true);
  TLine *l = new TLine(20.0,1.0,2000.0,1.0);
  l->SetLineStyle(kDashed);
  l->SetLineColor(kBlack);
  l->Draw("same");

  //Format the noPU profile
  if(pNoPU) {
     pNoPU->SetLineColor(kBlack);
     pNoPU->SetMarkerColor(kBlack);
     pNoPU->SetMarkerStyle(kOpenCircle);
     pNoPU->SetMarkerSize(1.0);
  }

  //Plot the profiles
  //TLegend * leg = new TLegend(0.5,0.437,0.875,0.762);
  //TLegend * leg = tdrLeg(0.345,0.427,0.825,0.91);
  //TLegend * leg = tdrLeg(0.455,0.427,0.885,0.91);
  TLegend * leg;
  if(suffix.Contains("puppi"))
     leg = tdrLeg(0.32,0.427,0.885,0.91);
  else if(suffix.Contains("chs"))
     leg = tdrLeg(0.37,0.427,0.885,0.91);
  else
     leg = tdrLeg(0.49,0.427,0.885,0.91);
  leg->AddEntry((TObject*)0,"QCD Monte Carlo","");
  if(suffix.Contains("puppi"))
     leg->AddEntry((TObject*)0,"Anti-k_{T} R=0.4, PF+PUPPI","");
  else if(suffix.Contains("chs"))
     leg->AddEntry((TObject*)0,"Anti-k_{T} R=0.4, PF+CHS","");
  else
     leg->AddEntry((TObject*)0,"Anti-k_{T} R=0.4, PF","");
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
    eraseBins(profs[p],1.73);
    tdrDraw(profs[p],"",kFullCircle,col,kSolid,col);

    leg->AddEntry(profs[p],profs[p]->GetTitle(),"lpe");
  }//for
  //pNoPU->Draw("same");
  if(pNoPU) tdrDraw(pNoPU,"",kOpenCircle,kBlack,kSolid,kBlack);

  leg->SetFillColor(0);
  leg->SetBorderSize(0);
  leg->Draw("same");
  
  // Plot the "CMS preliminary 8TeV", etc. 

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
  JRAEvent * ev = new JRAEvent(t);
  
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
         if (ev->refpt->at(j)>10 && ev->refdrjt->at(j)<0.2 && fabs(ev->jteta->at(j))<1.3 
//           && ev->npv >= npu[n] && ev->npv < npu[n+1])
//           && ev->tnpus->at(iIT) >= npu[n] && ev->tnpus->at(iIT) < npu[n+1])
           && ev->npus->at(iIT) >= npu[n] && ev->npus->at(iIT) < npu[n+1])
            profs[n]->Fill(ev->refpt->at(j), ev->jtpt->at(j)/ev->refpt->at(j));
      }// for jets
    }// for entries

  }// for profiles
  delete ev;
  
}//fillProfiles

void simpleResponsePlots(TString algo = "ak4pf", TString PU = "", TString sample = "QCD"){

  canvasNumber = 0;
  setTDRStyle();
  
  TString basepath = "root://cmseos.fnal.gov//store/user/lpcjme/noreplica/QCD_Flat_Pt-15to7000_TuneCUETP8M1_14TeV_pythia8/";

  TFile *file0 = TFile::Open(basepath+"PU200/170403_040528/JRA.root"); // ak5pfchs RD
  TFile *file1 = TFile::Open(basepath+"PU200/170403_040528/JRA_jecl1.root"); //ak5pfchsl1 RD
  TFile *file2 = TFile::Open(basepath+"NoPU/170403_040452/JRA.root"); // ak5pfchs NoPU RD
  TFile *file3 = TFile::Open(basepath+"PU200/170403_040528/JRA_jecl1l2l3.root"); // ak5pfchsl1l2l3 RD
  TFile *file4 = TFile::Open(basepath+"NoPU/170403_040452/JRA_jecl2l3.root"); // ak5pfchsl2l3 NoPU RD
  TFile *outFile = new TFile("simpleResponsePlots_"+algo+".root","RECREATE");

  // Vector of npu
  vector<int> npu;
  pair<int,int> puBounds;
  if(PU.Contains("200")) puBounds = make_pair(160,250);
  else if(PU.Contains("140")) puBounds = make_pair(100,190);
  else puBounds = make_pair(0,50);
  for (int n=puBounds.first; n<puBounds.second; n+=20)  
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

  fillProfiles(file0, "/"+algo, npu, profs);
  fillProfiles(file1, "/"+algo+"l1", npu, profsl1);
  fillProfiles(file3, "/"+algo+"l1l2l3", npu, profsl123);
  fillProfiles(file2, "/"+algo, npuNoPU, profs_NoPU);
  fillProfiles(file4, "/"+algo+"l2l3", npuNoPU, profsl2l3_NoPU);
  
  // Plot the profiles
  outFile->cd();
  plotProfiles(profs,"Uncorrected Response",profs_NoPU[0],algo);
  plotProfiles(profsl1,"Pileup-Corrected Response",profs_NoPU[0],algo);
  plotProfiles(profsl123,"Corrected Response",profsl2l3_NoPU[0],algo);

  //Close the file
  outFile->Write();
  outFile->Close();

}//a 

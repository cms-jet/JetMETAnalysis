#include "TObject.h"
#include "TFile.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TProfile.h"
#include "TMath.h"
#include "TLatex.h"
#include "TMarker.h"

#include "JetMETAnalysis/JetAnalyzers/interface/Settings.h"
#include "JetMETAnalysis/JetUtilities/interface/Style.h"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"

#include <iostream>
#include <assert.h>
#include <string>
#include <vector>

using namespace std;

Double_t jerfit(Double_t *xx, Double_t *p) {

  double x = xx[0];
  //double pt = (int(x) % 2000);
  //double npu = 0.1*int(x / 10000);
  //double pt = x - npu*100000;
  double r = 0.1*int(x / (1000*100000)); assert(r>0.0 && r<1.1);//assert(r>0.2 && r<1.1);
  double xb = x - r*10000*100000;
  double rhoa = 0.1*int(xb / 10000); assert(rhoa>0 && rhoa<110);
  double pt = xb - rhoa*100000; assert(pt>20 && pt < 1900);

  double c2 = p[2]*p[2] + p[4]*p[4]/(r*r);
  double jer = sqrt(max(p[0]*fabs(p[0])/(pt*pt) + p[1]*p[1]/pt + c2
			+ rhoa*p[3]*p[3]/(pt*pt), 0.));
  
  //cout << "pt="<<pt<<" npu="<<npu<<" jer="<<jer << endl;
  //cout << "  p[x]: " << p[0] << " " << p[1] << " " << p[2] << " " << p[3] << endl;
  //cout << flush;
  //assert(pt>19 && pt<2000);
  //assert(npu>0 && npu<30);

  return jer;
} // jerfit

//void jerplots(string seta="BB") {
int main(int argc,char**argv) {

  //
  // evaluate command-line / configuration file options
  //
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;

  TString         path          = cl.getValue<TString>  ("path");
  TString         odir          = cl.getValue<TString>  ("odir",            "./");
  string          seta          = cl.getValue<string>   ("seta",            "BB");
  bool            chs           = cl.getValue<bool>     ("chs",            false);
  const bool      _noaeff       = cl.getValue<bool>     ("noaeff",          true);
  bool            pfAndPfchs    = cl.getValue<bool>     ("pfAndPfchs",     false);
  bool            minimalist    = cl.getValue<bool>     ("minimalist",     false);
  bool            modifiedError = cl.getValue<bool>     ("modifiedError",  false);
  vector<TString> outputFormats = cl.getVector<TString> ("outputFormats", ".pdf");

  if (!cl.check()) return 0;
  cl.print();

  if(modifiedError && !minimalist)
    modifiedError = false;

  const char *ceta = seta.c_str();
  assert(seta=="BB"||seta=="EI"||seta=="EO"||seta=="FF");
  TString seta_expanded;
  if(seta=="BB")
    seta_expanded = "|#eta| < 1.3";
  else if(seta=="EI")
    seta_expanded = "1.3 < |#eta| < 2.5";
  else if(seta=="EO")
    seta_expanded = "2.5 < |#eta| < 3.0";
  else if(seta=="FF")
    seta_expanded = "3.0 < |#eta| < 5.0";

  TDirectory *curdir = gDirectory;
  
  setTDRStyle();
  gStyle->SetErrorX(0.);
  
  const int ncone = 9;
  double cones[ncone] = {0.2,0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
  
  //const int npu = 6;
  const int npu = 4;
  const int npuWithNoPU = 5;
  //const double rhos[npu] = {3.8, 7.8, 12.4, 17.3, 22.2, 28.9};//27.1};
  const double rhos[npuWithNoPU] = {8.794,15.47,24.12,32.11,0};
  //for minimalist plots
  const double rhoabins[] = {0,3,6,10,15,22,30,40,50,65,80,100};
  int nrhoabins = sizeof(rhoabins)/sizeof(rhoabins[0])-1;
  //const int jpus[npu] = {0, 1, 2, 3, 4, 5};
  const int jpus[npuWithNoPU] = {0, 1, 2, 3, 4};
  
  TGraphErrors *gallX = new TGraphErrors(0);
  TF1 *fall = new TF1("fall",jerfit,0,100.0*10000.0*100000.0,5);
  fall->SetParameters(-2.6, 0.90, 0.042, 1.4, 0.0071);
  fall->FixParameter(4,0.0071);
  TGraphErrors *gallXchs = new TGraphErrors(0);
  TF1 *fallchs = new TF1("fallchs",jerfit,0,100.0*10000.0*100000.0,5);
  fallchs->SetParameters(-2.6, 0.90, 0.042, 1.4, 0.0071);
  fallchs->FixParameter(4,0.0071);

  TProfile *pgna = new TProfile(Form("pgna"),"",nrhoabins,rhoabins,"s");
  pgna->SetLineColor(kBlack); pgna->SetMarkerColor(kBlack); pgna->SetMarkerSize(1.0); pgna->SetMarkerStyle(kFullCircle);
  TProfile *pgsa = new TProfile(Form("pgsa"),"",nrhoabins,rhoabins,"s");
  pgsa->SetLineColor(kRed); pgsa->SetMarkerColor(kRed); pgsa->SetMarkerSize(1.0); pgsa->SetMarkerStyle(kFullCircle);
  TProfile *pgca = new TProfile(Form("pgca"),"",nrhoabins,rhoabins,"s");
  pgca->SetLineColor(kBlue); pgca->SetMarkerColor(kBlue); pgca->SetMarkerSize(1.0); pgca->SetMarkerStyle(kFullCircle);

  TProfile *pgnachs = new TProfile(Form("pgnachs"),"",nrhoabins,rhoabins,"s");
  pgnachs->SetLineColor(kBlack); pgnachs->SetMarkerColor(kBlack); pgnachs->SetMarkerSize(1.0); pgnachs->SetMarkerStyle(kOpenCircle);
  TProfile *pgsachs = new TProfile(Form("pgsachs"),"",nrhoabins,rhoabins,"s");
  pgsachs->SetLineColor(kRed); pgsachs->SetMarkerColor(kRed); pgsachs->SetMarkerSize(1.0); pgsachs->SetMarkerStyle(kOpenCircle);
  TProfile *pgcachs = new TProfile(Form("pgcachs"),"",nrhoabins,rhoabins,"s");
  pgcachs->SetLineColor(kBlue); pgcachs->SetMarkerColor(kBlue); pgcachs->SetMarkerSize(1.0); pgcachs->SetMarkerStyle(kOpenCircle);

  //Stores the min and max values for each point in the TProfile so that we can calculate the errors.
  map<double,double> minPGNA, minPGSA, minPGCA, minPGNAchs, minPGSAchs, minPGCAchs;
  map<double,double> maxPGNA, maxPGSA, maxPGCA, maxPGNAchs, maxPGSAchs, maxPGCAchs;

  //Graphs to convert the TProfiles into graphs
  TGraphErrors *agna = new TGraphErrors();
  agna->SetLineColor(kBlack); agna->SetMarkerColor(kBlack); agna->SetMarkerSize(1.0); agna->SetMarkerStyle(kFullCircle);
  TGraphErrors *agsa = new TGraphErrors();
  agsa->SetLineColor(kRed); agsa->SetMarkerColor(kRed); agsa->SetMarkerSize(1.0); agsa->SetMarkerStyle(kFullCircle);
  TGraphErrors *agca = new TGraphErrors();
  agca->SetLineColor(kBlue); agca->SetMarkerColor(kBlue); agca->SetMarkerSize(1.0); agca->SetMarkerStyle(kFullCircle);

  TGraphErrors *agnachs = new TGraphErrors();
  agnachs->SetLineColor(kBlack); agnachs->SetMarkerColor(kBlack); agnachs->SetMarkerSize(1.0); agnachs->SetMarkerStyle(kOpenCircle);
  TGraphErrors *agsachs = new TGraphErrors();
  agsachs->SetLineColor(kRed); agsachs->SetMarkerColor(kRed); agsachs->SetMarkerSize(1.0); agsachs->SetMarkerStyle(kOpenCircle);
  TGraphErrors *agcachs = new TGraphErrors();
  agcachs->SetLineColor(kBlue); agcachs->SetMarkerColor(kBlue); agcachs->SetMarkerSize(1.0); agcachs->SetMarkerStyle(kOpenCircle);

  vector<TCanvas*> c1s(2*ncone);
  vector<TCanvas*> c2s(2*ncone);
  double rhoas[2*ncone][npu];
  
  TLegend *legcs = tdrLeg(0.30,0.15,0.60,0.38);
  legcs->SetNColumns(2);
  legcs->SetTextSize(0.026);

  TH1D* frame = new TH1D();
  frame->GetXaxis()->SetLimits(0.0,100.0);
  frame->GetYaxis()->SetRangeUser(0.0,14.0);
  frame->GetXaxis()->SetTitle("#mu#timesA");
  frame->GetYaxis()->SetTitle("Resolution Fit Parameter");
  TCanvas *c2a = tdrCanvas("c2a",frame,2,0,true);
  //TCanvas *c2a = new TCanvas("c2a","c2a");

  vector<pair<TString,double> > algs;
  for(int icone = 0; icone != ncone; ++icone) {
    double cone = cones[icone];
    if(chs==false && pfAndPfchs==false)
      algs.push_back(make_pair(Form("ak%1.0fpf",cone*10),cones[icone]));
    else if(chs==true && pfAndPfchs==false)
      algs.push_back(make_pair(Form("ak%1.0fpfchs",cone*10),cones[icone]));
    else if(pfAndPfchs==true) {
      algs.push_back(make_pair(Form("ak%1.0fpf",cone*10),cones[icone]));
      algs.push_back(make_pair(Form("ak%1.0fpfchs",cone*10),cones[icone]));
    }
  }

for (unsigned int ialg = 0; ialg<algs.size(); ++ialg) {

    double cone = algs[ialg].second;
    //double ajet = TMath::Pi()*cone*cone;
    const char *a = algs[ialg].first.Data();

    // effective area to account for fluctuations in jet direction
    // by an approxima sigma_dR of 0.1
    double aeff = (chs ? TMath::Pi()*(cone*cone + 0.4*0.4) :
       TMath::Pi()*(cone*cone + 0.2*0.2));
    if (_noaeff) aeff = TMath::Pi()*cone*cone;

    TFile *f = new TFile(Form("%s/canvases_synchplot_%s.root",
                         path.Data(),algs[ialg].first.Data()));
    assert(f && !f->IsZombie());
    assert(f->cd("histograms"));
    TDirectory *d = gDirectory;

    curdir->cd();
    
    TCanvas *c1 = new TCanvas(Form("c1_%s",a),Form("c1_%s",a),600,600);
    gPad->SetLogx();
    c1s[ialg] = c1;

    TH1D *h = new TH1D(Form("h_%s",a),";p_{T} (GeV);"
           "#sigma(p_{T}/p_{T,ref}) / #LTp_{T}/p_{T,ref}#GT",
           1989,10,1999);
    h->SetMinimum(0);
    h->SetMaximum(0.5-0.0001);
    h->GetXaxis()->SetMoreLogLabels();
    h->GetXaxis()->SetNoExponent();
    h->Draw("AXIS");

    TLegend *leg1 = tdrLeg(0.5,0.6,0.8,0.9);
    //TLegend *leg1 = tdrLeg(0.46,0.6,0.8,0.9);
    leg1->Draw();
    
    TF1 *f1 = new TF1(Form("f1_%s",a),
          "sqrt([0]*abs([0])/(x*x) + [1]*[1]/x + [2]*[2])",10,2000);
    f1->SetParameters(1,1,0.05);

    TGraphErrors *gn = new TGraphErrors(npu);
    TGraphErrors *gs = new TGraphErrors(npu);
    TGraphErrors *gc = new TGraphErrors(npu);
    TGraphErrors *gx = new TGraphErrors(npu);

    TGraphErrors *gall = new TGraphErrors(0);
    TGraphErrors *gallchs = new TGraphErrors(0);
    //for (int ipu = 0; ipu != npuWithNoPU; ++ipu) {
    for (int ipu = 0; ipu != npu; ++ipu) {

      TDirectory *d1 = d;
      int jpu = jpus[ipu];

      //TH1D *hjer = (TH1D*)d1->Get(Form("ResolutionRhoRef_%s_%d",ceta,jpu));
      //TH1D *hjer = (TH1D*)d1->Get(Form("ResolutionTnpuRef_%s_PU_NoPU_%d",ceta,jpu));
      TH1D *hjer = (TH1D*)d1->Get(Form("ResolutionTnpuRef_%s_%d",ceta,jpu));
      assert(hjer);
      curdir->cd();

      TH1D *hjerc = (TH1D*)hjer->DrawClone("SAME");
      
      double rho = max(1.,rhos[ipu]);
      //double rhoa = rho*ajet;
      double rhoaeff = rho*aeff;
      rhoas[ialg][ipu] = rhoaeff;
      for (int i = 1; i != hjer->GetNbinsX()+1; ++i) {
        double pt = hjer->GetBinCenter(i);
        double jer = hjer->GetBinContent(i);
        if (pt>20 && pt<1900 && jer<0.5) {
          int n = gall->GetN();
          Double_t x = pt + int(rhoaeff*10+0.5)*10000.
          + int(cone*10+0.5)*1000*100000.;
          assert(rho>0);
          //assert(rho<30.);
          assert(rho<50.);
          assert(pt>20);
          assert(pt<1900);
          if(!algs[ialg].first.Contains("chs")) {
            gall->SetPoint(n, x, hjer->GetBinContent(i));
            gall->SetPointError(n, 0, hjer->GetBinError(i));

            // also all Rcone
            int m = gallX->GetN();
            gallX->SetPoint(m, x, hjer->GetBinContent(i));
            gallX->SetPointError(m, 0, hjer->GetBinError(i));
          }
          else {
            int p = gallchs->GetN();
            gallchs->SetPoint(p, x, hjer->GetBinContent(i));
            gallchs->SetPointError(p, 0, hjer->GetBinError(i));

            int o = gallXchs->GetN();
            gallXchs->SetPoint(o, x, hjer->GetBinContent(i));
            gallXchs->SetPointError(o, 0, hjer->GetBinError(i));
          }
        }
      }
      if (rhos[ipu]!=0)
        //leg1->AddEntry(hjerc,Form("%d < #rho < %d GeV",jpu*5,(jpu+1)*5),"PL");
        leg1->AddEntry(hjerc,Form("%d < #mu < %d GeV",jpu*10,(jpu+1)*10),"PL");
      else {
        hjerc->SetMarkerColor(kBlack);
        hjerc->SetLineColor(kBlack);
        hjerc->SetMarkerStyle(kOpenCircle);
        //leg1->AddEntry(hjerc,"No PU (#LT#rho#GT=1 GeV)","PL");
        leg1->AddEntry(hjerc,"No PU (#LT#mu#GT=1 GeV)","PL");
      }

      f1->SetLineStyle(kSolid);
      f1->SetRange(20,2000);
      hjerc->Fit(f1,"QRN");
      f1->SetLineColor(hjerc->GetMarkerColor());
      f1->DrawClone("SAME");
      f1->SetLineStyle(kDashed);
      f1->SetRange(10,2000);
      f1->DrawClone("SAME");
      
      double x = sqrt(f1->GetChisquare()/f1->GetNDF());
      gn->SetPoint(ipu, rho, f1->GetParameter(0));
      gn->SetPointError(ipu, 0, f1->GetParError(0)*x);
      gs->SetPoint(ipu, rho, f1->GetParameter(1));
      gs->SetPointError(ipu, 0, f1->GetParError(1)*x);
      gc->SetPoint(ipu, rho, f1->GetParameter(2)*100);
      gc->SetPointError(ipu, 0, f1->GetParError(2)*100*x);
      gx->SetPoint(ipu, rho, f1->GetChisquare()/f1->GetNDF());// *0.1);
      gx->SetPointError(ipu, 0, 1./sqrt(f1->GetNDF()));// * 0.1);
    }       

    if(!algs[ialg].first.Contains("chs")) gall->Fit(fall,"RNW");
    else gallchs->Fit(fallchs,"RNW");
    for (int ipu = 0; ipu != npu; ++ipu) {
      
      TDirectory *d1 = d;
      int jpu = jpus[ipu];
      TH1D *hjer = (TH1D*)d1->Get(Form("ResolutionTnpuRef_%s_%d",ceta,jpu));
      assert(hjer);
      double rho = max(1.,rhos[ipu]);
      //double rhoa = rho*ajet;
      double rhoaeff = rho*aeff;
      f1->SetLineStyle(kDotted);
      f1->SetLineWidth(2);
      f1->SetLineColor(hjer->GetLineColor());
      if(!algs[ialg].first.Contains("chs")) {
        f1->SetParameters(sqrt(fabs(fall->GetParameter(0)
            *fabs(fall->GetParameter(0))
            + rhoaeff*pow(fall->GetParameter(3),2))),
        fall->GetParameter(1),
        sqrt(pow(fall->GetParameter(2),2)
             + pow(fall->GetParameter(4)/cone,2)));
      }
      else {
        f1->SetParameters(sqrt(fabs(fallchs->GetParameter(0)
            *fabs(fallchs->GetParameter(0))
            + rhoaeff*pow(fallchs->GetParameter(3),2))),
        fall->GetParameter(1),
        sqrt(pow(fallchs->GetParameter(2),2)
             + pow(fallchs->GetParameter(4)/cone,2)));
      }
      f1->DrawClone("SAME");
    }

    gPad->RedrawAxis();
    cmsPrel(-1);
    
    TLatex *tex = new TLatex();
    tex->SetNDC();
    tex->SetTextSize(0.045);
    if(algs[ialg].first.Contains("chs"))
      tex->DrawLatex(0.41,0.50,Form("PF+CHS, R=%1.1f, %s",cone,seta_expanded.Data()));
    else
      tex->DrawLatex(0.50,0.50,Form("PF, R=%1.1f, %s",cone,seta_expanded.Data()));

    TCanvas *c2 = new TCanvas(Form("c2_%s",a),Form("c2_%s",a),600,600);
    c2s[ialg] = c2;

    TH1D *h2 = new TH1D(Form("h2_%s",a),";#mu (GeV/rad^{2});JER parameter",6,0,30);
    //TH1D *h2 = new TH1D(Form("h2_%s",a),";#rho (GeV/rad^{2});JER parameter",6,0,30);
    h2->SetMaximum(8);
    h2->SetMinimum(0);//-0.5);
    h2->DrawClone("AXIS");

    gn->SetMarkerStyle(kFullCircle);
    gn->Draw("SAMEP");
    gs->SetLineColor(kRed);
    gs->SetMarkerColor(kRed);
    gs->SetMarkerStyle(kOpenCircle);
    gs->Draw("SAMEP");
    gc->SetLineColor(kBlue);
    gc->SetMarkerColor(kBlue);
    gc->SetMarkerStyle(kOpenSquare);
    gc->Draw("SAMEP");

    TF1 *f2 = new TF1(Form("f2_%s",a),"sqrt(abs([0]*abs([0]) + x*[1]*[1]))",
          0,30);
    gn->Fit(f2,"RNW");
    f2->SetLineColor(kBlack);
    f2->DrawClone("SAME");
    f2->SetLineWidth(2);
    f2->SetLineStyle(kDotted);
    if(!algs[ialg].first.Contains("chs"))
      f2->SetParameters(fall->GetParameter(0),fall->GetParameter(3)*sqrt(aeff));
    else
      f2->SetParameters(fallchs->GetParameter(0),fallchs->GetParameter(3)*sqrt(aeff));
    f2->DrawClone("SAME");

    TF1 *fc = new TF1(Form("fc_%s",a),"sqrt([0]*[0]+[1]*[1]/([2]*[2])",
          3.*aeff, 30.*aeff);
    gc->Fit(fc,"QRNW");
    fc->SetLineColor(kBlue);
    fc->DrawClone("SAME");
    fc->SetLineWidth(2);
    fc->SetLineStyle(kDotted);
    if(!algs[ialg].first.Contains("chs"))
      fc->SetParameters(fall->GetParameter(2)*100, fall->GetParameter(4)*100,cone);
    else
      fc->SetParameters(fallchs->GetParameter(2)*100, fallchs->GetParameter(4)*100,cone);
    fc->DrawClone("SAME");
    
    TF1 *fs = new TF1(Form("fs_%s",a),"pol1",0,30);
    gs->Fit(fs,"QRNW");
    fs->DrawClone("SAME");
    fs->SetLineWidth(2);
    fs->SetLineStyle(kDotted);
    if(!algs[ialg].first.Contains("chs"))
      fs->SetParameters(fall->GetParameter(1),0);
    else
      fs->SetParameters(fallchs->GetParameter(1),0);
    fs->DrawClone("SAME");
    
    cmsPrel(-1);
    
    TLegend *leg2 = tdrLeg(0.2,0.75,0.5,0.9);
    leg2->AddEntry(gn,"Noise+PU","PL");
    leg2->AddEntry(gs,"Stochastic","PL");
    leg2->AddEntry(gc,"Constant #times 100","PL");
    leg2->Draw();
    
    tex->SetTextSize(0.035);
    tex->DrawLatex(0.38,0.40,"#frac{#sigma}{p_{T}}  =   #sqrt{#frac{N|N| +"
                   " #sigma_{PU}^{2}#muA_{jet}}{p_{T}^{2}}  +  #frac{S^{2}}{p_{T}}  +  C^{2}}");
                   //" #sigma_{PU}^{2}#rhoA_{jet}}{p_{T}^{2}}  +  #frac{S^{2}}{p_{T}}  +  C^{2}}");
    if(!algs[ialg].first.Contains("chs")) {
      tex->DrawLatex(0.35,0.30,Form("N = %1.2f #pm %1.2f",
            fall->GetParameter(0), fall->GetParError(0)));
      tex->DrawLatex(0.35,0.25,Form("S = %1.3f #pm %1.3f",
            fall->GetParameter(1), fall->GetParError(1)));
      tex->DrawLatex(0.63,0.25,Form("C = %1.4f #pm %1.4f",
            fall->GetParameter(2), fall->GetParError(2)));
      tex->DrawLatex(0.63,0.30,Form("#sigma_{PU} = %1.3f #pm %1.3f",
            fall->GetParameter(3),
            fall->GetParError(3)));
      tex->DrawLatex(0.63,0.25,Form("Cx = %1.4f #pm %1.4f",
            fall->GetParameter(4), fall->GetParError(4)));
    }
    else {
      tex->DrawLatex(0.35,0.30,Form("N = %1.2f #pm %1.2f",
            fallchs->GetParameter(0), fallchs->GetParError(0)));
      tex->DrawLatex(0.35,0.25,Form("S = %1.3f #pm %1.3f",
            fallchs->GetParameter(1), fallchs->GetParError(1)));
      tex->DrawLatex(0.63,0.25,Form("C = %1.4f #pm %1.4f",
            fallchs->GetParameter(2), fallchs->GetParError(2)));
      tex->DrawLatex(0.63,0.30,Form("#sigma_{PU} = %1.3f #pm %1.3f",
            fallchs->GetParameter(3),
            fallchs->GetParError(3)));
      tex->DrawLatex(0.63,0.25,Form("Cx = %1.4f #pm %1.4f",
            fallchs->GetParameter(4), fallchs->GetParError(4)));      
    }

    tex->SetTextSize(0.045);
    if(algs[ialg].first.Contains("chs"))
      tex->DrawLatex(0.51,0.85,Form("PF+CHS, R=%1.1f, %s",cone,seta_expanded.Data()));
    else
      tex->DrawLatex(0.60,0.85,Form("PF, R=%1.1f, %s",cone,seta_expanded.Data()));

    
    TCanvas *c3 = new TCanvas(Form("c3_%s",a),Form("c3_%s",a),600,600);
    if(!algs[ialg].first.Contains("chs")) {
      gall->Draw("AP");
      fall->Draw("SAME");
    }
    else {
      gallchs->Draw("P SAME");
      fallchs->Draw("SAME");
    }
    
    delete c3;

    f->Close();
    curdir->cd();


    // All cone sizes summarized
    c2a->cd();
    if (ialg==0) {
      //TString axes_titles = ";#rho#timesA_{eff} (GeV);Fit parameter";
      //if (_noaeff) axes_titles = ";#rho#timesA (GeV);Fit parameter";
      TString axes_titles = ";#mu#timesA_{eff};Resolution Fit Parameter";
      if (_noaeff) axes_titles = ";#mu#timesA;Resolution Fit Parameter";
      TH1D *h2a = new TH1D("h2a", axes_titles,
         110,0,110);
      h2a->SetMaximum(14-0.001);
      h2a->SetMinimum(-2+0.001);//0);
      if(minimalist)
        h2a->SetMinimum(0+0.001);
      //h2a->Draw("AXIS");
    }
    double size = 0.5 + (cone-0.3)/0.7;
    TGraphErrors *gna = (TGraphErrors*)gn->Clone(Form("gna_%d",ialg));
    gna->SetMarkerColor(gn->GetMarkerColor());
    gna->SetMarkerSize(size);
    if(algs[ialg].first.Contains("chs")) gna->SetMarkerStyle(kOpenCircle);
    TGraphErrors *gsa = (TGraphErrors*)gn->Clone(Form("gsa_%d",ialg));
    gsa->SetMarkerColor(gs->GetMarkerColor());
    gsa->SetMarkerSize(size);
    if(algs[ialg].first.Contains("chs")) gsa->SetMarkerStyle(kOpenCircle);
    TGraphErrors *gca = (TGraphErrors*)gn->Clone(Form("gca_%d",ialg));
    gca->SetMarkerColor(gc->GetMarkerColor());
    gca->SetMarkerSize(size);
    if(algs[ialg].first.Contains("chs")) gca->SetMarkerStyle(kOpenCircle);

    for (int i = 0; i != gna->GetN(); ++i) {
      gna->SetPoint(i, gn->GetX()[i]*aeff, gn->GetY()[i]);
      gsa->SetPoint(i, gs->GetX()[i]*aeff, gs->GetY()[i]*10);
      gca->SetPoint(i, gc->GetX()[i]*aeff, gc->GetY()[i]);

      if(algs[ialg].first.Contains("chs")) {
        pgnachs->Fill(gna->GetX()[i], gna->GetY()[i]);
        pgsachs->Fill(gsa->GetX()[i], gsa->GetY()[i]);
        pgcachs->Fill(gca->GetX()[i], gca->GetY()[i]);

        if(cone>=0.1&&cone<=1.0) {
          int binPGNAchs = pgnachs->FindBin(gna->GetX()[i]);
          int binPGSAchs = pgsachs->FindBin(gsa->GetX()[i]);
          int binPGCAchs = pgcachs->FindBin(gca->GetX()[i]);
          //Get the minimum y value for each bin
          if(minPGNAchs.find(binPGNAchs)==minPGNAchs.end() || gna->GetY()[i]<minPGNAchs[binPGNAchs])
            minPGNAchs[binPGNAchs] = gna->GetY()[i];
          if(minPGSAchs.find(binPGSAchs)==minPGSAchs.end() || gsa->GetY()[i]<minPGSAchs[binPGSAchs])
            minPGSAchs[binPGSAchs] = gsa->GetY()[i];
          if(minPGCAchs.find(binPGCAchs)==minPGCAchs.end() || gca->GetY()[i]<minPGCAchs[binPGCAchs])
            minPGCAchs[binPGCAchs] = gca->GetY()[i];

          //Get the maximum y value for each bin
          if(maxPGNAchs.find(binPGNAchs)==maxPGNAchs.end() || gna->GetY()[i]>maxPGNAchs[binPGNAchs])
            maxPGNAchs[binPGNAchs] = gna->GetY()[i];
          if(maxPGSAchs.find(binPGSAchs)==maxPGSAchs.end() || gsa->GetY()[i]>maxPGSAchs[binPGSAchs])
            maxPGSAchs[binPGSAchs] = gsa->GetY()[i];
          if(maxPGCAchs.find(binPGCAchs)==maxPGCAchs.end() || gca->GetY()[i]>maxPGCAchs[binPGCAchs])
            maxPGCAchs[binPGCAchs] = gca->GetY()[i];
        }
      }
      else {
        pgna->Fill(gna->GetX()[i], gna->GetY()[i]);
        pgsa->Fill(gsa->GetX()[i], gsa->GetY()[i]);
        pgca->Fill(gca->GetX()[i], gca->GetY()[i]);

        if(cone>=0.1&&cone<=1.0) {
          int binPGNA = pgna->FindBin(gna->GetX()[i]);
          int binPGSA = pgsa->FindBin(gsa->GetX()[i]);
          int binPGCA = pgca->FindBin(gca->GetX()[i]);
          //Get the minimum y value for each bin
          if(minPGNA.find(binPGNA)==minPGNA.end() || gna->GetY()[i]<minPGNA[binPGNA])
            minPGNA[binPGNA] = gna->GetY()[i];
          if(minPGSA.find(binPGSA)==minPGSA.end() || gsa->GetY()[i]<minPGSA[binPGSA])
            minPGSA[binPGSA] = gsa->GetY()[i];
          if(minPGCA.find(binPGCA)==minPGCA.end() || gca->GetY()[i]<minPGCA[binPGCA])
            minPGCA[binPGCA] = gca->GetY()[i];
  
          //Get the maximum y value for each bin
          if(maxPGNA.find(binPGNA)==maxPGNA.end() || gna->GetY()[i]>maxPGNA[binPGNA])
            maxPGNA[binPGNA] = gna->GetY()[i];
          if(maxPGSA.find(binPGSA)==maxPGSA.end() || gsa->GetY()[i]>maxPGSA[binPGSA])
            maxPGSA[binPGSA] = gsa->GetY()[i];
          if(maxPGCA.find(binPGCA)==maxPGCA.end() || gca->GetY()[i]>maxPGCA[binPGCA])
            maxPGCA[binPGCA] = gca->GetY()[i];
        }
      }
    }

    if(!minimalist) {
      gna->DrawClone("SAMEPz");
      gsa->DrawClone("SAMEPz");
      gca->DrawClone("SAMEPz");
    }

    if(ialg==0) {
      legcs->AddEntry((TObject*)0,"PF","");
      legcs->AddEntry((TObject*)0,"PFchs","");
    }
    legcs->AddEntry(gna,Form("R=%.1f",cone),"P");
    if(ialg == algs.size()-1 && !minimalist) {
      legcs->Draw();
    }

  } // for ialg
  if(minimalist) {
    if(modifiedError) {
      //Set the bin errors
      if(pgna->GetNbinsX() != (int)minPGNA.size())
        cout << "WARNING::the number of bins in pgna (" << pgna->GetNbinsX() << ")and the number of min/max values ("<< minPGNA.size() << ") for that bin are not the same" << endl;
      for(int ibin=1; ibin<=pgna->GetNbinsX(); ibin++) {
        agna->SetPoint(ibin-1,pgna->GetBinCenter(ibin),pgna->GetBinContent(ibin));
        agsa->SetPoint(ibin-1,pgsa->GetBinCenter(ibin),pgsa->GetBinContent(ibin));
        agca->SetPoint(ibin-1,pgca->GetBinCenter(ibin),pgca->GetBinContent(ibin));
        agnachs->SetPoint(ibin-1,pgnachs->GetBinCenter(ibin),pgnachs->GetBinContent(ibin));
        agsachs->SetPoint(ibin-1,pgsachs->GetBinCenter(ibin),pgsachs->GetBinContent(ibin));
        agcachs->SetPoint(ibin-1,pgcachs->GetBinCenter(ibin),pgcachs->GetBinContent(ibin));
        agna->SetPointError(ibin-1,0.0,(maxPGNA[ibin]-minPGNA[ibin])/2.0);
        agsa->SetPointError(ibin-1,0.0,(maxPGSA[ibin]-minPGSA[ibin])/2.0);
        agca->SetPointError(ibin-1,0.0,(maxPGCA[ibin]-minPGCA[ibin])/2.0);
        agnachs->SetPointError(ibin-1,0.0,(maxPGNAchs[ibin]-minPGNAchs[ibin])/2.0);
        agsachs->SetPointError(ibin-1,0.0,(maxPGSAchs[ibin]-minPGSAchs[ibin])/2.0);
        agcachs->SetPointError(ibin-1,0.0,(maxPGCAchs[ibin]-minPGCAchs[ibin])/2.0);
        //cout << "bin = "<< ibin << "\tmin = " << minPGCAchs[ibin] << "\tmax = " << maxPGCAchs[ibin] 
        //     << "\terror(set) = " << (maxPGCAchs[ibin]-minPGCAchs[ibin])/2.0 << "\terror(found) = " << agcachs->GetErrorY(ibin-1) << endl;

        //agna->Draw("SAMEPz");
        //agsa->Draw("SAMEPz");
        //agca->Draw("SAMEPz");
        //agnachs->Draw("SAMEPz");
        //agsachs->Draw("SAMEPz");
        //agcachs->Draw("SAMEPz");
        tdrDraw(agna,"Pz",kFullCircle,kBlack);
        tdrDraw(agsa,"Pz",kFullCircle,kRed);
        tdrDraw(agca,"Pz",kFullCircle,kBlue);
        tdrDraw(agnachs,"Pz",kOpenCircle,kBlack);
        tdrDraw(agsachs,"Pz",kOpenCircle,kRed);
        tdrDraw(agcachs,"Pz",kOpenCircle,kBlue);
      }
    }
    else {
      tdrDraw(pgna,"PEX0",kFullCircle,kBlack);
      tdrDraw(pgsa,"PEX0",kFullCircle,kRed);
      tdrDraw(pgca,"PEX0",kFullCircle,kBlue);
      tdrDraw(pgnachs,"PEX0",kOpenCircle,kBlack);
      tdrDraw(pgsachs,"PEX0",kOpenCircle,kRed);
      tdrDraw(pgcachs,"PEX0",kOpenCircle,kBlue);
      //pgna->Draw("SAMEPz");
      //pgsa->Draw("SAMEPz");
      //pgca->Draw("SAMEPz");
      //pgnachs->Draw("SAMEPz");
      //pgsachs->Draw("SAMEPz");
      //pgcachs->Draw("SAMEPz");
    }
  }

  // Fit all cones, all rhos at once
  gallX->Fit(fall,"RNW");
  gallXchs->Fit(fallchs,"RNW");
  for (unsigned int ialg = 0; ialg<algs.size(); ++ialg) {

    TCanvas *c1 = c1s[ialg];
    c1->cd();

    double cone = algs[ialg].second;
    double aeff = (chs ? TMath::Pi()*(cone*cone+0.4*0.4) :
       TMath::Pi()*(cone*cone+0.2*0.2));
    if (_noaeff) aeff = TMath::Pi()*cone*cone;

    TF1 *f1 = new TF1(Form("f1X_%d",ialg),
          "sqrt([0]*[0]/(x*x) + [1]*[1]/x + [2]*[2])",10,2000);
    
    for (int ipu = 0; ipu != npu; ++ipu) {
      
      double rhoaeff = rhoas[ialg][ipu];
      f1->SetLineStyle(kDashDotted);
      f1->SetLineColor(kBlack);
      if(!algs[ialg].first.Contains("chs")) {
        f1->SetParameters(sqrt(fabs(fall->GetParameter(0)
            *fabs(fall->GetParameter(0))
            + rhoaeff*pow(fall->GetParameter(3),2))),
        fall->GetParameter(1),
        sqrt(pow(fall->GetParameter(2),2)+
             pow(fall->GetParameter(4)/cone,2)));
      }
      else {
        f1->SetParameters(sqrt(fabs(fallchs->GetParameter(0)
            *fabs(fallchs->GetParameter(0))
            + rhoaeff*pow(fallchs->GetParameter(3),2))),
        fallchs->GetParameter(1),
        sqrt(pow(fallchs->GetParameter(2),2)+
             pow(fallchs->GetParameter(4)/cone,2)));
      }
      f1->DrawClone("SAME");
    } 

    TCanvas *c2 = c2s[ialg];
    c2->cd();

    TF1 *f2 = new TF1(Form("f2X_%d",ialg),"sqrt(abs([0]*abs([0])+x*[1]*[1]))",
          0,110);
    f2->SetLineColor(kBlack);
    if(algs[ialg].first.Contains("chs")) f2->SetLineStyle(kDashDotted);
    else f2->SetLineStyle(kDotted);//DashDotted);
    f2->SetLineWidth(2);
    if(!algs[ialg].first.Contains("chs"))
      f2->SetParameters(fall->GetParameter(0),fall->GetParameter(3));
    else
      f2->SetParameters(fallchs->GetParameter(0),fallchs->GetParameter(3));
    f2->DrawClone("SAME");

    TF1 *fc = new TF1(Form("fcX_%d",ialg),"sqrt([0]*[0]+[1]*[1]/([2]*[2]))",
          //0,110);
          3.*aeff, 30.*aeff);
    fc->SetLineColor(kBlue);
    if(algs[ialg].first.Contains("chs")) fc->SetLineStyle(kDashDotted);
    else fc->SetLineStyle(kDotted);//DashDotted);
    fc->SetLineWidth(2);
    if(!algs[ialg].first.Contains("chs"))
      fc->SetParameters(fall->GetParameter(2)*100, fall->GetParameter(4)*100,cone);
    else
      fc->SetParameters(fallchs->GetParameter(2)*100, fallchs->GetParameter(4)*100,cone);
    fc->DrawClone("SAME");

    TF1 *fs = new TF1(Form("fsX_%d",ialg),"pol1",0,110);
    fs->SetLineColor(kRed);
    if(algs[ialg].first.Contains("chs")) fs->SetLineStyle(kDashDotted);
    else fs->SetLineStyle(kDotted);//DashDotted);
    fs->SetLineWidth(2);
    if(!algs[ialg].first.Contains("chs"))
      fs->SetParameters(fall->GetParameter(1)*10,0);
    else
      fs->SetParameters(fallchs->GetParameter(1)*10,0);

    c2a->cd();
    if(!minimalist) {
      f2->DrawClone("SAME");
      fc->DrawClone("SAME");
      fs->DrawClone("SAME");
    }
    
    if (ialg==0) {
      if(minimalist) {
        TF1 *fn = new TF1("fn","sqrt(abs([0]*abs([0])+x*[1]*[1]))",0,110);
        fn->SetLineColor(kBlack); fn->SetLineStyle(kDotted); fn->SetLineWidth(2);
        pgna->Fit(fn,"QRNW"); fn->DrawClone("SAME");

        //TF1 *fnchs = new TF1("fnchs","sqrt([0]*x)",0,110);
        TF1 *fnchs = new TF1("fnchs","sqrt(abs([0]*abs([0])+x*[1]*[1]))",0,110);
        fnchs->SetLineColor(kBlack); fnchs->SetLineStyle(kDashDotted); fnchs->SetLineWidth(2);
        pgnachs->Fit(fnchs,"QRNW"); fnchs->DrawClone("SAME");
      }
      
      TLegend *leg;
      if(minimalist)
        leg = tdrLeg(0.28,0.73,0.48,0.91);
      else
        leg = tdrLeg(0.20,0.75,0.40,0.90);
      if(minimalist) {
        leg->SetHeader("PF+CHS");
        f2->SetMarkerStyle(kOpenCircle); f2->SetMarkerColor(kBlack);
        fs->SetMarkerStyle(kOpenCircle); fs->SetMarkerColor(kRed);
        fc->SetMarkerStyle(kOpenCircle); fc->SetMarkerColor(kBlue);
      }
      else {
        f2->SetMarkerStyle(kFullCircle); f2->SetMarkerColor(kBlack);
        fs->SetMarkerStyle(kFullCircle); fs->SetMarkerColor(kRed);
        fc->SetMarkerStyle(kFullCircle); fc->SetMarkerColor(kBlue);
      }
      leg->AddEntry(f2,"N (GeV)","LP");
      leg->AddEntry(fs,"S (#sqrt{GeV}) #times 10","LP");
      leg->AddEntry(fc,"C (1) #times 100","LP");
      leg->Draw("SAME");

      TLegend* leg2;
      if(minimalist) {
        leg2 = tdrLeg(0.20,0.73,0.40,0.91);
        TF1* f2clone = (TF1*)f2->Clone(); f2clone->SetMarkerStyle(kFullCircle);
        leg2->AddEntry(f2clone," ","LP");
        TF1* fsclone = (TF1*)fs->Clone(); fsclone->SetMarkerStyle(kFullCircle);
        leg2->AddEntry(fsclone," ","LP");
        TF1* fcclone = (TF1*)fc->Clone(); fcclone->SetMarkerStyle(kFullCircle);
        leg2->AddEntry(fcclone," ","LP");
        leg2->SetHeader("PF");
        leg2->Draw("SAME");
      }

      TLatex *tex = new TLatex();
      tex->SetNDC(); tex->SetTextSize(0.045);
      tex->DrawLatex(0.70, 0.87, seta_expanded);
      if(minimalist) {
        tex->DrawLatex(0.70, 0.82, "R=0.2-1.0");
        tex->SetTextSize(0.027);
        //tex->DrawLatex(0.535, 0.21,"#frac{#sigma}{#LT#frac{p_{T}}{p_{T}^{GEN}}#GT} =   #sqrt{#frac{N#upoint|N|+#sigma_{PU}^{2}#muA}{p_{T}^{2}} + #frac{S^{2}}{p_{T}} + C^{2}}");
        tex->DrawLatex(0.54, 0.19,"#frac{#sigma}{p_{T}} =   #sqrt{#frac{N_{0}#upoint|N_{0}|+#sigma_{PU}^{2}#muA}{p_{T}^{2}} + #frac{S^{2}}{p_{T}} + C^{2}}");
        tex->SetTextSize(0.045);
        tex->DrawLatex(0.20, 0.19,
         _noaeff ? "(A = #LTA_{jet}#GT = #piR^{2})" :
         (chs ? "A_{eff} = #pi(R^{2}+0.4^{2})" :
          "A_{eff} = #pi(R^{2}+0.2^{2})"));
      }
      else {
         tex->DrawLatex(0.60, 0.20,
         _noaeff ? "(A = #LTA_{jet}#GT = #piR^{2})" :
         (chs ? "A_{eff} = #pi(R^{2}+0.4^{2})" :
          "A_{eff} = #pi(R^{2}+0.2^{2})"));
      }
      if(!pfAndPfchs) tex->DrawLatex(0.70, 0.82, chs ? "PFchs" : "PF");

      //cmsPrel(0);
    }

    // Save with global fit drawn on top
    const char *a = algs[ialg].first.Data();
    for(unsigned int f=0; f<outputFormats.size(); f++) {
      if (algs[ialg].first.Contains("chs")) {
        c1->SaveAs(Form("%s/jerplots_%s_vsRho_CHS%s",odir.Data(),a,outputFormats[f].Data()));
        c2->SaveAs(Form("%s/jerplots_%s_ParFits_CHS%s",odir.Data(),a,outputFormats[f].Data()));
      }
      else {
        c1->SaveAs(Form("%s/jerplots_%s_vsRho%s",odir.Data(),a,outputFormats[f].Data()));
        c2->SaveAs(Form("%s/jerplots_%s_ParFits%s",odir.Data(),a,outputFormats[f].Data()));
      }    
    }
  } // for ialg (2nd)

  cout << "Etabin " << ceta << (chs ? " PFchs" : " plain PF") << endl;
  cout << "// sigma_pT/pT = sqrt( (N*|N| + sPU^2*rho*Aeff)/pT^2 " << endl
       << "// + S^2/pT + (C_0^2 + C_1^2/R^2)), where" << endl
       << (chs ? "// Aeff = pi*sqrt(R^2 + 0.4^2) for CHS" :
     "// Aeff = pi*sqrt(R^2 + 0.2^2) for plain PF") << endl
       << "// [0]=N, [1]=S, [2]=C_0, [3]=sPU, [4]=C_1" << endl;
  cout << Form("{%1.2f, %1.2f, %1.3f, %1.4f, %1.4f}",
         fall->GetParameter(0), fall->GetParameter(1),
         fall->GetParameter(2), fall->GetParameter(3),
         fall->GetParameter(4)) << endl;
  
  for(unsigned int f=0; f<outputFormats.size(); f++) {
    if(pfAndPfchs) c2a->SaveAs(odir+"/jerplots_ParFits_Combined"+outputFormats[f]);
    else if  (chs) c2a->SaveAs(odir+"/jerplots_ParFits_CHS"+outputFormats[f]);
    else           c2a->SaveAs(odir+"/jerplots_ParFits"+outputFormats[f]);
  }

  TFile* ofile;
  if(pfAndPfchs) ofile = new TFile(odir+"/jerplots_ParFits_Combined.root","RECREATE");
  else if (chs)  ofile = new TFile(odir+"/jerplots_ParFits_CHS.root","RECREATE");
  else           ofile = new TFile(odir+"/jerplots_ParFits.root","RECREATE");
  c2a->Write();
  for(unsigned int c=0; c<c1s.size(); c++) {
    c1s[c]->Write();
  }
  for(unsigned int c=0; c<c2s.size(); c++) {
    c2s[c]->Write();
  }
  ofile->Close();

}




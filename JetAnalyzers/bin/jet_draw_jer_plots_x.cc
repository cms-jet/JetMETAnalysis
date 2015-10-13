#include "TFile.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TMath.h"
#include "TLatex.h"

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

  TString    path       = cl.getValue<TString> ("path");
  string     seta       = cl.getValue<string>  ("seta",       "BB");
  bool chs              = cl.getValue<bool>    ("chs",        false);
  const bool _noaeff    = cl.getValue<bool>    ("noaeff",     true);
  bool       pfAndPfchs = cl.getValue<bool>    ("pfAndPfchs", false);

  if (!cl.check()) return 0;
  cl.print();
  
  const char *ceta = seta.c_str();

  TDirectory *curdir = gDirectory;
  
  setTDRStyle();
  
  //const int ncone = 1;
  //double cones[ncone] = {0.4};
  //const int ncone = 5;
  //double cones[ncone] = {0.3, 0.4, 0.5, 0.6, 0.7};
  const int ncone = 8;
  double cones[ncone] = {0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
  
  const int npu = 6;
  const double rhos[npu] = {3.8, 7.8, 12.4, 17.3, 22.2, 28.9};//27.1};
  const int jpus[npu] = {0, 1, 2, 3, 4, 5};
  //const int npu = 7;
  //const double rhos[npu] = {0, 3.8, 7.8, 12.4, 17.3, 22.2, 28.9};//27.1};
  //const int jpus[npu] = {0, 0, 1, 2, 3, 4, 5};
  
  TGraphErrors *gallX = new TGraphErrors(0);
  TGraphErrors *gallCB = new TGraphErrors(0);
  //TF1 *fall = new TF1(Form("fall_%s",a),jerfit,0,100*10*20000,4);
  //TF1 *fall = new TF1("fall",jerfit,0,100*10*20000,4);
  //TF1 *fall = new TF1("fall",jerfit,0,110*100000,4);
  //TF1 *fall = new TF1("fall",jerfit,0,100*10000*100000,5); //Mikko's version which has the error integer overflow in expression [-Werror=overflow]
  TF1 *fall = new TF1("fall",jerfit,0,100.0*10000.0*100000.0,5); //Alexx's Version
  fall->SetParameters(-2.6, 0.90, 0.042, 1.4, 0.0071);
  fall->FixParameter(4,0.0071);
  vector<TCanvas*> c1s(ncone);
  vector<TCanvas*> c2s(ncone);
  double rhoas[ncone][npu];
  
  TCanvas *c2a = new TCanvas("c2a","c2a");
  TCanvas *c2b = new TCanvas("c2b","c2b");
  
  TLegend *legcs = tdrLeg(0.20,0.15,0.50,0.30); //0.4 -> 0.5
  legcs->SetNColumns(2);

  //int used to keep track of if both PF and PFchs have already been included
  //counts the number of time through the same cone size
  int ccounter = 1;

  for (int icone = 0; icone != ncone; ++icone) {

    double cone = cones[icone];//0.7;
    double ajet = TMath::Pi()*cone*cone;
    string sa = Form("ak%1.0fpf",cone*10);
    const char *a = sa.c_str();

    // effective area to account for fluctuations in jet direction
    // by an approxima sigma_dR of 0.1
    double aeff = (chs ? TMath::Pi()*(cone*cone + 0.4*0.4) :
		   TMath::Pi()*(cone*cone + 0.2*0.2));
    if (_noaeff) aeff = TMath::Pi()*cone*cone;

    //TFile *f = new TFile(Form("files/canvases_synchplot_%s.root",a));
    TFile *f = new TFile(Form("%s/canvases_synchplot_%s%s.root",path.Data(),a,
			      chs ? "chs" : ""));
    assert(f && !f->IsZombie());
    assert(f->cd("histograms"));
    TDirectory *d = gDirectory;

    // for NoPU files, when available
    TFile *f0 = new TFile(Form("%s/canvases_synchplot_%s%s_nopu.root",path.Data(),a,
			       chs ? "chs" : ""));
    //assert(f0 && !f0->IsZombie());
    assert(f0->IsZombie() || f0->cd("histograms"));
    TDirectory *d0 = gDirectory;
    
    // OffResolutionRhoRef_<detector section>_ak<cone size><pf,pfchs>.pdf
    // ResolutionRhoRef_<detector section>_ak<cone size><pf,pfchs>.pdf
    // <cone size> must really be NPU bin 0-5?
    
    curdir->cd();
    
    TCanvas *c1 = new TCanvas(Form("c1_%s",a),Form("c1_%s",a),600,600);
    gPad->SetLogx();
    c1s[icone] = c1;

    TH1D *h = new TH1D(Form("h_%s",a),";p_{T} (GeV);"
		       "#sigma(p_{T}/p_{T,ref}) / #LTp_{T}/p_{T,ref}#GT",
		       1989,10,1999);
    h->SetMinimum(0);
    h->SetMaximum(0.5-0.0001);
    h->GetXaxis()->SetMoreLogLabels();
    h->GetXaxis()->SetNoExponent();
    h->Draw("AXIS");
    
    TLegend *leg1 = tdrLeg(0.5,0.6,0.8,0.9);
    leg1->Draw();
    
    TF1 *f1 = new TF1(Form("f1_%s",a),
		      "sqrt([0]*abs([0])/(x*x) + [1]*[1]/x + [2]*[2])",10,2000);
    f1->SetParameters(1,1,0.05);
    //TF1 *fall = new TF1(Form("fall_%s",a),jerfit,0,100*10*20000,4);
    //fall->SetParameters(-2.6, 0.90, 0.042, 1.4);
    //fall->FixParameter(0, -2);
    //fall->FixParameter(1, 0.89);
    //fall->FixParameter(2, 0.040);
    //fall->FixParameter(3, 1.3);
    
    TGraphErrors *gn = new TGraphErrors(npu);
    TGraphErrors *gs = new TGraphErrors(npu);
    TGraphErrors *gc = new TGraphErrors(npu);
    TGraphErrors *gx = new TGraphErrors(npu);
    
    TGraphErrors *gall = new TGraphErrors(0);
    for (int ipu = 0; ipu != npu; ++ipu) {

      TDirectory *d1 = (rhos[ipu]==0 && f0 ? d0 : d);
      int jpu = jpus[ipu];

      TH1D *hjer = (TH1D*)d1->Get(Form("ResolutionRhoRef_%s_%d",ceta,jpu));
      assert(hjer);
      curdir->cd();

      TH1D *hjerc = (TH1D*)hjer->DrawClone("SAME");
      
      double rho = max(1.,rhos[ipu]);
      //double rhoa = rho*ajet;
      double rhoaeff = rho*aeff;
      rhoas[icone][ipu] = rhoaeff;
      for (int i = 1; i != hjer->GetNbinsX()+1; ++i) {
	double pt = hjer->GetBinCenter(i);
	double jer = hjer->GetBinContent(i);
	//if (pt>20 && pt<1900) {
	if (pt>20 && pt<1900 && jer<0.5) {
	  int n = gall->GetN();
	  // X = (PT) + (NPU*10)*20000
	  //Double_t x = pt + int(mpu*10+0.5)*10000.;
	  //Double_t x = pt + int(rhoaeff*10+0.5)*10000.;
	  Double_t x = pt + int(rhoaeff*10+0.5)*10000.
			+ int(cone*10+0.5)*1000*100000.;
	  assert(rho>0);
	  assert(rho<30.);
	  assert(pt>20);
	  assert(pt<1900);
	  gall->SetPoint(n, x, hjer->GetBinContent(i));
	  gall->SetPointError(n, 0, hjer->GetBinError(i));

	  // also all Rcone
	  int m = gallX->GetN();
	  gallX->SetPoint(m, x, hjer->GetBinContent(i));
	  gallX->SetPointError(m, 0, hjer->GetBinError(i));
	}
      }
      
      if (rhos[ipu]!=0)
	leg1->AddEntry(hjerc,Form("%d < #rho < %d GeV",jpu*5,(jpu+1)*5),
		       "PL");
      else {
	hjerc->SetMarkerColor(kBlack);
	hjerc->SetLineColor(kBlack);
	leg1->AddEntry(hjerc,"No PU (#LT#rho#GT=1 GeV)","PL");
      }

      f1->SetLineStyle(kSolid);
      f1->SetRange(20,2000);
      hjerc->Fit(f1,"QRN");
      f1->SetLineColor(hjerc->GetMarkerColor());
      f1->DrawClone("SAME");
      f1->SetLineStyle(kDashed);
      f1->SetRange(10,2000);
      f1->DrawClone("SAME");
      

      
      //if (ipu==2) fall->SetParameters(-2, f1->GetParameter(1), f1->GetParameter(2),
      //			    f1->GetParameter(0));
      
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
    
    gall->Fit(fall,"RN");
    for (int ipu = 0; ipu != npu; ++ipu) {
      
      TDirectory *d1 = (rhos[ipu]==0 && f0 ? d0 : d);
      int jpu = jpus[ipu];
      TH1D *hjer = (TH1D*)d1->Get(Form("ResolutionRhoRef_%s_%d",ceta,jpu));
      assert(hjer);
      double rho = max(1.,rhos[ipu]);
      //double rhoa = rho*ajet;
      double rhoaeff = rho*aeff;
      f1->SetLineStyle(kDotted);
      f1->SetLineWidth(2);
      f1->SetLineColor(hjer->GetLineColor());
      f1->SetParameters(sqrt(fabs(fall->GetParameter(0)
				  *fabs(fall->GetParameter(0))
				  + rhoaeff*pow(fall->GetParameter(3),2))),
			fall->GetParameter(1),
			sqrt(pow(fall->GetParameter(2),2)
			     + pow(fall->GetParameter(4)/cone,2)));
      f1->DrawClone("SAME");
    }
    
    gPad->RedrawAxis();
    cmsPrel(-1);
    
    TLatex *tex = new TLatex();
    tex->SetNDC();
    tex->SetTextSize(0.045);
    tex->DrawLatex(0.50,0.50,Form("PF, R=%1.1f, |#eta|<1.3",cone));

    TCanvas *c2 = new TCanvas(Form("c2_%s",a),Form("c2_%s",a),600,600);
    //TH1D *h2 = new TH1D("h2",";N_{PU};Parameter",6,0,30);//-0.5,5.5);
    c2s[icone] = c2;

    TH1D *h2 = new TH1D(Form("h2_%s",a),
			";#rho (GeV/rad^{2});JER parameter",6,0,30);
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
    //gx->SetMarkerStyle(kOpenStar);
    //gx->Draw("SAMEP");
    
    TF1 *f2 = new TF1(Form("f2_%s",a),"sqrt(abs([0]*abs([0]) + x*[1]*[1]))",
		      0,30);
    //f2->FixParameter(0,0);
    gn->Fit(f2,"RN");
    f2->SetLineColor(kBlack);
    f2->DrawClone("SAME");
    f2->SetLineWidth(2);
    f2->SetLineStyle(kDotted);
    //f2->SetParameters(fall->GetParameter(0),fall->GetParameter(3)*sqrt(ajet));
    f2->SetParameters(fall->GetParameter(0),fall->GetParameter(3)*sqrt(aeff));
    f2->DrawClone("SAME");

    TF1 *fc = new TF1(Form("fc_%s",a),"sqrt([0]*[0]+[1]*[1]/([2]*[2])",
		      3.*aeff, 30.*aeff);
    gc->Fit(fc,"QRN");
    fc->SetLineColor(kBlue);
    fc->DrawClone("SAME");
    fc->SetLineWidth(2);
    fc->SetLineStyle(kDotted);
    //fc->SetParameters(fall->GetParameter(2)*100,0);
    fc->SetParameters(fall->GetParameter(2)*100, fall->GetParameter(4)*100,
		      cone);

    fc->DrawClone("SAME");
    
    TF1 *fs = new TF1(Form("fs_%s",a),"pol1",0,30);
    gs->Fit(fs,"QRN");
    fs->DrawClone("SAME");
    fs->SetLineWidth(2);
    fs->SetLineStyle(kDotted);
    fs->SetParameters(fall->GetParameter(1),0);
    fs->DrawClone("SAME");
    
    cmsPrel(-1);
    
    TLegend *leg2 = tdrLeg(0.2,0.75,0.5,0.9);
    leg2->AddEntry(gn,"Noise+PU","PL");
    leg2->AddEntry(gs,"Stochastic","PL");
    leg2->AddEntry(gc,"Constant #times 100","PL");
    leg2->Draw();
    
    tex->SetTextSize(0.035);
    tex->DrawLatex(0.38,0.40,
		   "#frac{#sigma}{p_{T}}  =   #sqrt{#frac{N|N| + "
		   "#sigma_{PU}^{2}#rhoA_{jet}}{p_{T}^{2}}  +  "
		   "#frac{S^{2}}{p_{T}}  +  C^{2}}");
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

    tex->SetTextSize(0.045);
    tex->DrawLatex(0.60,0.85,Form("PF, R=%1.1f, |#eta|<1.3",cone));

    
    TCanvas *c3 = new TCanvas(Form("c3_%s",a),Form("c3_%s",a),600,600);
    gall->Draw("AP");
    fall->Draw("SAME");
    
    delete c3;

    f->Close();
    curdir->cd();

    /*
    if (chs) {
      c1->SaveAs(Form("pdf/jerplots_%s_vsRho_CHS.pdf",a));
      c2->SaveAs(Form("pdf/jerplots_%s_ParFits_CHS.pdf",a));
    }
    else {
      c1->SaveAs(Form("pdf/jerplots_%s_vsRho.pdf",a));
      c2->SaveAs(Form("pdf/jerplots_%s_ParFits.pdf",a));
    }
    */

    // All cone sizes summarized
    c2a->cd();
    if (icone==0) {
       TString axes_titles = ";#rho#timesA_{eff} (GeV);Fit parameter";
       if (_noaeff) axes_titles = ";#rho#timesA (GeV);Fit parameter";
      TH1D *h2a = new TH1D("h2a", axes_titles,
			   110,0,110);
      h2a->SetMaximum(18-0.001);
      h2a->SetMinimum(-2+0.001);//0);
      h2a->Draw("AXIS");
    }
    double size = 0.5 + (cone-0.3)/0.7;
    TGraphErrors *gna = (TGraphErrors*)gn->Clone(Form("gna_%d",icone));
    gna->SetMarkerColor(gn->GetMarkerColor());
    gna->SetMarkerSize(size);
    if(pfAndPfchs&&chs) gna->SetMarkerStyle(kOpenCircle);
    TGraphErrors *gsa = (TGraphErrors*)gn->Clone(Form("gsa_%d",icone));
    gsa->SetMarkerColor(gs->GetMarkerColor());
    gsa->SetMarkerSize(size);
    if(pfAndPfchs&&chs) gsa->SetMarkerStyle(kOpenCircle);
    TGraphErrors *gca = (TGraphErrors*)gn->Clone(Form("gca_%d",icone));
    gca->SetMarkerColor(gc->GetMarkerColor());
    gca->SetMarkerSize(size);
    if(pfAndPfchs&&chs) gca->SetMarkerStyle(kOpenCircle);
    for (int i = 0; i != gna->GetN(); ++i) {
      gna->SetPoint(i, gn->GetX()[i]*aeff, gn->GetY()[i]);
      gsa->SetPoint(i, gs->GetX()[i]*aeff, gs->GetY()[i]*10);
      gca->SetPoint(i, gc->GetX()[i]*aeff, gc->GetY()[i]);
    }
    gna->DrawClone("SAMEPz");
    gsa->DrawClone("SAMEPz");
    gca->DrawClone("SAMEPz");

    legcs->AddEntry(gna,Form("R=%.1f",cone),"LP");
    if(icone == ncone-1)
       legcs->Draw();

    // All cone sizes summarized
    c2b->cd();
    if (icone==0) {
      TH1D *h2b = new TH1D("h2b", ";A_{jet} (rad^{2});Fit parameter",
			   100,0,3);
      h2b->SetMaximum(15-0.001);
      h2b->SetMinimum(0);
      h2b->Draw("AXIS");
    }
    //double size = 0.5 + (cone-0.3)/0.7;
    TGraphErrors *gnb = (TGraphErrors*)gn->Clone(Form("gnb_%d",icone));
    gnb->SetMarkerColor(gn->GetMarkerColor());
    gnb->SetMarkerSize(size);
    TGraphErrors *gsb = (TGraphErrors*)gn->Clone(Form("gsb_%d",icone));
    gsb->SetMarkerColor(gs->GetMarkerColor());
    gsb->SetMarkerSize(size);
    TGraphErrors *gcb = (TGraphErrors*)gn->Clone(Form("gcb_%d",icone));
    gcb->SetMarkerColor(gc->GetMarkerColor());
    gcb->SetMarkerSize(size);
    for (int i = 0; i != gna->GetN(); ++i) {
      gnb->SetPoint(i, ajet, gn->GetY()[i]);
      gsb->SetPoint(i, ajet, gs->GetY()[i]*10);
      gcb->SetPoint(i, ajet, gc->GetY()[i]);
      int m = gallCB->GetN();
      gallCB->SetPoint(m, ajet, gc->GetY()[i]);
    }
    //gnb->DrawClone("SAMEPz");
    gsb->DrawClone("SAMEPz");
    gcb->DrawClone("SAMEPz");

    //Make the loop go through each cone size twice to pick up PF and PFchs
    //cout << ccounter << "\t" << icone << "\t" << chs << endl;
    if(pfAndPfchs && ccounter%2==0) {
      ccounter = 1;
    }
    else if(pfAndPfchs && ccounter%2!=0) {
      icone--;
      chs = !chs;
      ccounter++;
    }

  } // for icone


  // Fit all cones, all rhos at once
  gallX->Fit(fall,"RN");
  for (int icone = 0; icone != ncone; ++icone) {

    TCanvas *c1 = c1s[icone];
    c1->cd();

    double cone = cones[icone];
    double aeff = (chs ? TMath::Pi()*(cone*cone+0.4*0.4) :
		   TMath::Pi()*(cone*cone+0.2*0.2));
    if (_noaeff) aeff = TMath::Pi()*cone*cone;

    TF1 *f1 = new TF1(Form("f1X_%d",icone),
		      "sqrt([0]*[0]/(x*x) + [1]*[1]/x + [2]*[2])",10,2000);
    
    for (int ipu = 0; ipu != npu; ++ipu) {
      
      double rhoaeff = rhoas[icone][ipu];
      f1->SetLineStyle(kDashDotted);
      f1->SetLineColor(kBlack);
      f1->SetParameters(sqrt(fabs(fall->GetParameter(0)
				  *fabs(fall->GetParameter(0))
				  + rhoaeff*pow(fall->GetParameter(3),2))),
			fall->GetParameter(1),
			sqrt(pow(fall->GetParameter(2),2)+
			     pow(fall->GetParameter(4)/cone,2)));
      f1->DrawClone("SAME");
    } 

    TCanvas *c2 = c2s[icone];
    c2->cd();

    TF1 *f2 = new TF1(Form("f2X_%d",icone),"sqrt(abs([0]*abs([0])+x*[1]*[1]))",
		      0,110);
    f2->SetLineColor(kBlack);
    if(pfAndPfchs&&chs) f2->SetLineStyle(kDashDotted);
    else f2->SetLineStyle(kDotted);//DashDotted);
    f2->SetLineWidth(2);
    f2->SetParameters(fall->GetParameter(0),fall->GetParameter(3));
    f2->DrawClone("SAME");

    TF1 *fc = new TF1(Form("fcX_%d",icone),"sqrt([0]*[0]+[1]*[1]/([2]*[2]))",
		      //0,110);
		      3.*aeff, 30.*aeff);
    fc->SetLineColor(kBlue);
    if(pfAndPfchs&&chs) fc->SetLineStyle(kDashDotted);
    else fc->SetLineStyle(kDotted);//DashDotted);
    fc->SetLineWidth(2);
    fc->SetParameters(fall->GetParameter(2)*100, fall->GetParameter(4)*100,
		      cone);
    fc->DrawClone("SAME");

    TF1 *fs = new TF1(Form("fsX_%d",icone),"pol1",0,110);
    fs->SetLineColor(kRed);
    if(pfAndPfchs&&chs) fs->SetLineStyle(kDashDotted);
    else fs->SetLineStyle(kDotted);//DashDotted);
    fs->SetLineWidth(2);
    fs->SetParameters(fall->GetParameter(1)*10,0);

    c2a->cd();
    f2->DrawClone("SAME");
    fc->DrawClone("SAME");
    fs->DrawClone("SAME");
    
    if (icone==0) {
      
      TLegend *leg = tdrLeg(0.20,0.75,0.40,0.90);
      f2->SetMarkerStyle(kFullCircle); f2->SetMarkerColor(kBlack);
      leg->AddEntry(f2,"N (GeV)","LP");
      fs->SetMarkerStyle(kFullCircle); fs->SetMarkerColor(kRed);
      leg->AddEntry(fs,"S (1/#sqrt{GeV}) #times 10","LP");
      fc->SetMarkerStyle(kFullCircle); fc->SetMarkerColor(kBlue);
      leg->AddEntry(fc,"C (1/GeV) #times 100","LP");
      leg->Draw();

      TLatex *tex = new TLatex();
      tex->SetNDC(); tex->SetTextSize(0.045);
      tex->DrawLatex(0.70, 0.87, "|#eta| < 1.3");
      tex->DrawLatex(0.70, 0.82, chs ? "PFchs" : "PF");
      tex->DrawLatex(0.60, 0.20,
		     _noaeff ? "(A = #LTA_{jet}#GT = #piR^{2})" :
		     (chs ? "A_{eff} = #pi(R^{2}+0.4^{2})" :
		      "A_{eff} = #pi(R^{2}+0.2^{2})"));

      cmsPrel(0);
    }

    // Save with global fit drawn on top
    string sa = Form("ak%1.0fpf",cone*10);
    const char *a = sa.c_str();
    if (chs) {
      c1->SaveAs(Form("pdf/jerplots_%s_vsRho_CHS.pdf",a));
      c2->SaveAs(Form("pdf/jerplots_%s_ParFits_CHS.pdf",a));
    }
    else {
      c1->SaveAs(Form("pdf/jerplots_%s_vsRho.pdf",a));
      c2->SaveAs(Form("pdf/jerplots_%s_ParFits.pdf",a));
    }    
  } // for icone (2nd)

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
  
  if (chs) c2a->SaveAs("pdf/jerplots_ParFits_CHS.pdf");
  else     c2a->SaveAs("pdf/jerplots_ParFits.pdf");

  TFile* ofile;
  if (chs) ofile = new TFile("jerplots_ParFits_CHS.root","RECREATE");
  else     ofile = new TFile("jerplots_ParFits.root","RECREATE");
  c2a->Write();
  c2b->Write();
  for(unsigned int c=0; c<c1s.size(); c++) {
    c1s[c]->Write();
  }
  for(unsigned int c=0; c<c2s.size(); c++) {
    c2s[c]->Write();
  }
  ofile->Close();

  if (true) {
    c2b->cd();
    TF1 *fb = new TF1("fb","sqrt([0]*[0]+[1]*[1]/x)",0,3);
    gallCB->Fit(fb,"RN");
    fb->SetLineColor(kBlue);
    fb->DrawClone("SAME");

    fb->SetParameters(100*fall->GetParameter(2),
		      100*sqrt(TMath::Pi())*fall->GetParameter(4));
    fb->SetLineStyle(kDotted);
    fb->SetLineWidth(2);
    fb->DrawClone("SAME");

    cmsPrel(-1);
  }
  if (chs) c2b->SaveAs("pdf/jerplots_ParFits_vsAjet_CHS.pdf");
  else     c2b->SaveAs("pdf/jerplots_ParFits_vsAjet.pdf");

}

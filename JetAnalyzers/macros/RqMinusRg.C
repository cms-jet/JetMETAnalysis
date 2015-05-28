void RqMinusRg() {

   setTDRStyle();
   gROOT->LoadMacro("$CMSSW_BASE/src/JetMETAnalysis/JetAnalyzers/interface/Settings.h");
   TH1D* dummy = new TH1D();
   dummy->GetXaxis()->SetLimits(20.0,1000.0);
   dummy->GetXaxis()->SetMoreLogLabels();
   dummy->GetXaxis()->SetNoExponent();
   //dummy->GetYaxis()->SetRangeUser(0.0,0.13);
   dummy->GetYaxis()->SetRangeUser(0.0,0.08);
   dummy->GetXaxis()->SetTitle("p_{T}^{ptcl} [GeV]");
   //dummy->GetYaxis()->SetTitle("#Rgothic_{q}-#Rgothic_{g}");
   //dummy->GetYaxis()->SetTitle("R_{q}-R_{g}");
   dummy->GetYaxis()->SetTitle("R_{ud}-R_{g}");
   TCanvas* c = tdrCanvas("gqResp",dummy,2,11,true);
   c->GetPad(0)->SetLogx();

   TFile* fPythia = new TFile("l5_physDef_pythia.root");
   ak5pfchsl1l2l3->cd();
   //TGraphErrors* quark = (TGraphErrors*)gDirectory->Get("qAbsRspVsRefPt_JetEta0to1.3");
   TGraphErrors* quark = (TGraphErrors*)gDirectory->Get("udAbsRspVsRefPt_JetEta0to1.3");
   TGraphErrors* gluon = (TGraphErrors*)gDirectory->Get("gAbsRspVsRefPt_JetEta0to1.3");
   int N_ = quark->GetN();
   vector<double> x(N_,0.0), y(N_,0.0), exl(N_,0.0), exh(N_,0.0), ey(N_,0.0);
   for(int i=0; i<N_; i++) {
      x[i]=(quark->GetX()[i]);y[i] = (quark->GetY()[i]-gluon->GetY()[i]);ey[i] = (TMath::Sqrt(TMath::Power(quark->GetEY()[i],2)+TMath::Power(gluon->GetEY()[i],2)));exl[i] = x[i]-vpt[i];exh[i] = vpt[i+1]-x[i];
   }
   TGraphAsymmErrors* diff = new TGraphAsymmErrors(N_,&x.at(0),&y.at(0),&exl.at(0),&exh.at(0),&ey.at(0),&ey.at(0));
   diff->SetMarkerStyle(kFullCircle);
   diff->SetMarkerColor(kRed);
   tdrDraw(diff,"P",kFullCircle,kRed);

   TFile* fHerwig = new TFile("l5_physDef_herwig.root");
   ak5pfchsl1l2l3->cd();
   //TGraphErrors* hquark = (TGraphErrors*)gDirectory->Get("qAbsRspVsRefPt_JetEta0to1.3");
   TGraphErrors* hquark = (TGraphErrors*)gDirectory->Get("udAbsRspVsRefPt_JetEta0to1.3");
   TGraphErrors* hgluon = (TGraphErrors*)gDirectory->Get("gAbsRspVsRefPt_JetEta0to1.3");
   int hN_ = hquark->GetN();
   vector<double> hx(hN_,0.0), hy(hN_,0.0), hexl(hN_,0.0), hexh(hN_,0.0), hey(hN_,0.0);
   for(int i=0; i<hN_; i++) {
      hx[i]=(hquark->GetX()[i]);hy[i] = (hquark->GetY()[i]-hgluon->GetY()[i]);hey[i] = (TMath::Sqrt(TMath::Power(hquark->GetEY()[i],2)+TMath::Power(hgluon->GetEY()[i],2)));hexl[i] = hx[i]-vpt[i];hexh[i] = vpt[i+1]-hx[i];
   }
   TGraphAsymmErrors* hdiff = new TGraphAsymmErrors(hN_,&hx.at(0),&hy.at(0),&hexl.at(0),&hexh.at(0),&hey.at(0),&hey.at(0));
   hdiff->SetMarkerStyle(kOpenCircle);
   hdiff->SetMarkerColor(kBlue);
   tdrDraw(hdiff,"P",kOpenCircle,kBlue);

   TLegend* leg = tdrLeg(0.38,0.6,0.78,1-gPad->GetTopMargin()-0.045*(1-gPad->GetTopMargin()-gPad->GetBottomMargin())+0.01);
   leg->AddEntry((TObject*)0,"Anti-k_{T} R=0.5, PF+CHS","");
   //leg->AddEntry((TObject*)0,"L1L2L3 Corrected","");
   leg->AddEntry((TObject*)0,"|#eta|<1.3","");
   //leg->AddEntry((TObject*)0,"Physics Definition","");
   leg->AddEntry((TObject*)0,"Parton Flavor","");
   leg->AddEntry(diff,"PYTHIA Z2*","pl");
   leg->AddEntry(hdiff,"HERWIG++ EE3C","pl");
   leg->Draw("SAME");

   c->SaveAs("qgResponse_PhysicsDef.png");
   c->SaveAs("qgResponse_PhysicsDef.pdf");
   c->SaveAs("qgResponse_PhysicsDef.C");

}



void RqMinusRg_caloVsPF() {

   setTDRStyle();
   gROOT->LoadMacro("$CMSSW_BASE/src/JetMETAnalysis/JetAnalyzers/interface/Settings.h");
   TH1D* dummy = new TH1D();
   dummy->GetXaxis()->SetLimits(20.0,1000.0);
   dummy->GetXaxis()->SetMoreLogLabels();
   dummy->GetXaxis()->SetNoExponent();
   dummy->GetYaxis()->SetRangeUser(0.0,0.08);
   dummy->GetXaxis()->SetTitle("p_{T}^{ptcl} [GeV]");
   dummy->GetYaxis()->SetTitle("R_{ud}-R_{g}");
   TCanvas* c = tdrCanvas("gqResp",dummy,2,11,true);
   c->GetPad(0)->SetLogx();

   TFile* fCalo = new TFile("l5.root");
   ak5calo->cd();
   TGraphErrors* quark = (TGraphErrors*)gDirectory->Get("udAbsRspVsRefPt_JetEta0to1.3");
   TGraphErrors* gluon = (TGraphErrors*)gDirectory->Get("gAbsRspVsRefPt_JetEta0to1.3");
   int N_ = quark->GetN();
   vector<double> x(N_,0.0), y(N_,0.0), exl(N_,0.0), exh(N_,0.0), ey(N_,0.0);
   for(int i=0; i<N_; i++) {
      x[i]=(quark->GetX()[i]);y[i] = (quark->GetY()[i]-gluon->GetY()[i]);ey[i] = (TMath::Sqrt(TMath::Power(quark->GetEY()[i],2)+TMath::Power(gluon->GetEY()[i],2)));exl[i] = x[i]-vpt[i];exh[i] = vpt[i+1]-x[i];
   }
   TGraphAsymmErrors* diff = new TGraphAsymmErrors(N_,&x.at(0),&y.at(0),&exl.at(0),&exh.at(0),&ey.at(0),&ey.at(0));
   diff->SetMarkerStyle(kFullCircle);
   diff->SetMarkerColor(kRed);
   tdrDraw(diff,"P",kFullCircle,kRed);

   TFile* fPF = new TFile("l5.root");
   ak5pf->cd();
   TGraphErrors* hquark = (TGraphErrors*)gDirectory->Get("udAbsRspVsRefPt_JetEta0to1.3");
   TGraphErrors* hgluon = (TGraphErrors*)gDirectory->Get("gAbsRspVsRefPt_JetEta0to1.3");
   int hN_ = hquark->GetN();
   vector<double> hx(hN_,0.0), hy(hN_,0.0), hexl(hN_,0.0), hexh(hN_,0.0), hey(hN_,0.0);
   for(int i=0; i<hN_; i++) {
      hx[i]=(hquark->GetX()[i]);hy[i] = (hquark->GetY()[i]-hgluon->GetY()[i]);hey[i] = (TMath::Sqrt(TMath::Power(hquark->GetEY()[i],2)+TMath::Power(hgluon->GetEY()[i],2)));hexl[i] = hx[i]-vpt[i];hexh[i] = vpt[i+1]-hx[i];
   }
   TGraphAsymmErrors* hdiff = new TGraphAsymmErrors(hN_,&hx.at(0),&hy.at(0),&hexl.at(0),&hexh.at(0),&hey.at(0),&hey.at(0));
   hdiff->SetMarkerStyle(kOpenCircle);
   hdiff->SetMarkerColor(kBlue);
   tdrDraw(hdiff,"P",kOpenCircle,kBlue);

   TLegend* leg = tdrLeg(0.38,0.6,0.78,1-gPad->GetTopMargin()-0.045*(1-gPad->GetTopMargin()-gPad->GetBottomMargin())+0.01);
   //leg->AddEntry((TObject*)0,"Anti-k_{T} R=0.5, PF+CHS","");
   leg->AddEntry((TObject*)0,"|#eta|<1.3","");
   //leg->AddEntry((TObject*)0,"Parton Flavor",""); //A.K.A Physics Definition
   leg->AddEntry((TObject*)0,"Algorithmic Flavor","");
   leg->AddEntry(diff,"Anti-k_{T} R=0.5, Calo","pl");
   leg->AddEntry(hdiff,"Anti-k_{T} R=0.5, PF","pl");
   leg->Draw("SAME");

   c->SaveAs("qgResponse.png");
   c->SaveAs("qgResponse.pdf");
   //c->SaveAs("qgResponse.C");
}

void DrawL2(TString l2filename, TString alg, TString etaname)
{
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0000);
  gStyle->SetOptFit(111); 
  gStyle->SetPalette(1);
  
  TGraphErrors *g_AbsEtaCorrection;
  TGraphErrors *g_RelEtaCorrection;
  TCanvas *c_Resp;
  TCanvas *c_L2Cor;
  TF1 *RelFit;
  TF1 *AbsFit;
  TFile *l2file;
  char filename[100];
  TString name;
  l2file = new TFile(l2filename,"r");
  if (!l2file->IsOpen()) break;
  TDirectoryFile *dir = (TDirectoryFile*)l2file->Get(alg);
  /////////////////////////////// Correction /////////////////////////
  name = "AbsCorrection_JetEta"+etaname;
  c_Cor = new TCanvas(name,name,900,700);
  name = "AbsCorVsJetPt_JetEta"+etaname;
  g_AbsEtaCorrection = (TGraphErrors*)dir->Get(name);    
  AbsFit = (TF1*)g_AbsEtaCorrection->GetFunction("fit"); 
  if (AbsFit->GetXmax()>200) 
    gPad->SetLogx();
  AbsFit->SetLineColor(2);
  g_AbsEtaCorrection->GetXaxis()->SetTitle("Uncorrected jet p_{T} (GeV)");
  g_AbsEtaCorrection->GetYaxis()->SetTitle("Absolute Correction"); 
  g_AbsEtaCorrection->SetTitle(etaname); 
  g_AbsEtaCorrection->SetMarkerStyle(20);
  g_AbsEtaCorrection->Draw("AP"); 
  
  /////////////////////////////// L2 correction ///////////////////////// 
  name = "RelCorrection_JetEta"+etaname;
  c_L2Cor = new TCanvas(name,name,900,700);
  name = "RelCorVsJetPt_JetEta"+etaname;
  g_RelEtaCorrection = (TGraphErrors*)dir->Get(name);   
  RelFit = (TF1*)g_RelEtaCorrection->GetFunction("fit");
  if (RelFit->GetXmax()>200) 
    gPad->SetLogx(); 
  g_RelEtaCorrection->SetMinimum(0.3);
  g_RelEtaCorrection->SetMaximum(1.4);
  g_RelEtaCorrection->GetXaxis()->SetTitle("Uncorrected jet p_{T} (GeV)");
  g_RelEtaCorrection->GetYaxis()->SetTitle("Relative Correction");  
  g_RelEtaCorrection->SetTitle(etaname); 
  g_RelEtaCorrection->Draw("AP");
  g_RelEtaCorrection->SetMarkerStyle(20);
  RelFit->SetLineColor(2);
}

void MakeMCPileupDistribution(TString ifilename = "", TString ofilename = "", TString alg = "") {
   if(ifilename.IsNull())
      ifilename = "/fdata/hepx/store/user/aperloff/JRA_outfiles_53X_20140212_pbs/JRA/JRA.root";
   if(ofilename.IsNull())
      ofilename = "pileup14_RD_March13_finebin.root";
      //ofilename = "/fdata/hepx/store/user/aperloff/JEC_532/53X/pileup14_RD_March13_finebin.root";
   if(alg.IsNull())
      alg = "ak5pf";
   TFile* ifile = new TFile(ifilename,"READ");
   TFile* ofile = new TFile(ofilename,"RECREATE");
   ifile->cd(alg);
   TH1D* pileup = new TH1D("pileup","pileup",600,0,60);
   t->Draw("tnpus[1]>>pileup");
   pileup->GetXaxis()->SetTitle("tnpus[1]");
   ofile->cd();
   pileup->Write();
   ofile->Close();
}

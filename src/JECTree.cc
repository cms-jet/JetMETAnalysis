#define JECTree_cxx
#include "JetMETAnalysis/JetUtilities/interface/JECTree.h"

//   In a ROOT session, you can do:
//      Root > .L JECTree.C
//      Root > JECTree t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16



JECTree::JECTree(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/uscms_data/d2/aperloff/JRA_outfiles_53X_20120911/JRA/JRA.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("/uscms_data/d2/aperloff/JRA_outfiles_53X_20120911/JRA/JRA.root");
      }
      TDirectory * dir = (TDirectory*)f->Get("/uscms_data/d2/aperloff/JRA_outfiles_53X_2012s0911/JRA/JRA.root:/ak5pf");
      dir->GetObject("t",tree);

   }
   Init(tree);
}

JECTree::~JECTree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t JECTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t JECTree::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void JECTree::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   npus = 0;
   tnpus = 0;
   bxns = 0;
   sumpt_lowpt = 0;
   sumpt_highpt = 0;
   ntrks_lowpt = 0;
   ntrks_highpt = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("npus", &npus, &b_npus);
   fChain->SetBranchAddress("tnpus", &tnpus, &b_tnpus);
   fChain->SetBranchAddress("bxns", &bxns, &b_bxns);
   fChain->SetBranchAddress("sumpt_lowpt", &sumpt_lowpt, &b_sumpt_lowpt);
   fChain->SetBranchAddress("sumpt_highpt", &sumpt_highpt, &b_sumpt_highpt);
   fChain->SetBranchAddress("ntrks_lowpt", &ntrks_lowpt, &b_ntrks_lowpt);
   fChain->SetBranchAddress("ntrks_highpt", &ntrks_highpt, &b_ntrks_highpt);
   fChain->SetBranchAddress("rho", &rho, &b_rho);
   fChain->SetBranchAddress("rho50", &rho50, &b_rho50);
   fChain->SetBranchAddress("rho_hlt", &rho_hlt, &b_rho_hlt);
   fChain->SetBranchAddress("pthat", &pthat, &b_pthat);
   fChain->SetBranchAddress("weight", &weight, &b_weight);
   fChain->SetBranchAddress("npv", &npv, &b_npv);
   fChain->SetBranchAddress("evt", &evt, &b_evt);
   fChain->SetBranchAddress("run", &run, &b_run);
   fChain->SetBranchAddress("nref", &nref, &b_nref);
   fChain->SetBranchAddress("refrank", refrank, &b_refrank);
   fChain->SetBranchAddress("refpdgid", refpdgid, &b_refpdgid);
   fChain->SetBranchAddress("refe", refe, &b_refe);
   fChain->SetBranchAddress("refpt", refpt, &b_refpt);
   fChain->SetBranchAddress("refeta", refeta, &b_refeta);
   fChain->SetBranchAddress("refphi", refphi, &b_refphi);
   fChain->SetBranchAddress("refy", refy, &b_refy);
   fChain->SetBranchAddress("refdrjt", refdrjt, &b_refdrjt);
   fChain->SetBranchAddress("refarea", refarea, &b_refarea);
   fChain->SetBranchAddress("jte", jte, &b_jte);
   fChain->SetBranchAddress("jtpt", jtpt, &b_jtpt);
   fChain->SetBranchAddress("jteta", jteta, &b_jteta);
   fChain->SetBranchAddress("jtphi", jtphi, &b_jtphi);
   fChain->SetBranchAddress("jty", jty, &b_jty);
   fChain->SetBranchAddress("jtjec", jtjec, &b_jtjec);
   fChain->SetBranchAddress("jtarea", jtarea, &b_jtarea);
   fChain->SetBranchAddress("jtchf", jtchf, &b_jtchf);
   fChain->SetBranchAddress("jtnhf", jtnhf, &b_jtnhf);
   fChain->SetBranchAddress("jtnef", jtnef, &b_jtnef);
   fChain->SetBranchAddress("jtcef", jtcef, &b_jtcef);
   fChain->SetBranchAddress("jtmuf", jtmuf, &b_jtmuf);
   fChain->SetBranchAddress("jthfhf", jthfhf, &b_jthfhf);
   fChain->SetBranchAddress("jthfef", jthfef, &b_jthfef);
   Notify();
}

Bool_t JECTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void JECTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t JECTree::Cut(Long64_t )
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}


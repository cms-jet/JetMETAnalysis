//#ifdef JetUtilities_JRAEvent_cxx
//#define JetUtilities_JRAEvent_cxx

#include "JetMETAnalysis/JetUtilities/interface/JRAEvent.h"

#include "TH2.h"
#include "TStyle.h"
#include "TCanvas.h"

JRAEvent::JRAEvent(TTree *tree, bitset<8> b) : fChain(0), flags(b)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if(!tree || tree->GetNbranches()==0) {
      MakeTree(tree);
   }
   else {
      Init(tree);
   }
}

JRAEvent::~JRAEvent()
{
   if (!fChain) return;
   delete fChain;
}

Int_t JRAEvent::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}

Long64_t JRAEvent::LoadTree(Long64_t entry)
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

void JRAEvent::MakeTree(TTree* tree)
{
   gROOT->ProcessLine("#include <vector>");

   // Set object pointer
   MakeVectors();

   // Set branch addresses and branch pointers
   if (!tree) tree = new TTree("t","t");
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->Branch("npus", "vector<Int_t>", &npus);
   fChain->Branch("tnpus", "vector<Float_t>", &tnpus);
   fChain->Branch("zpositions", "vector<Float_t>", &zpositions);
   fChain->Branch("bxns", "vector<Int_t>", &bxns);
   fChain->Branch("sumpt_lowpt", "vector<Float_t>", &sumpt_lowpt);
   fChain->Branch("sumpt_highpt", "vector<Float_t>", &sumpt_highpt);
   fChain->Branch("ntrks_lowpt", "vector<Int_t>", &ntrks_lowpt);
   fChain->Branch("ntrks_highpt", "vector<Int_t>", &ntrks_highpt);
   fChain->Branch("rhos", "vector<Float_t>", &rhos);
   fChain->Branch("rho", &rho, "rho/F");
   if(!flags.test(0) || (flags.test(0) && flags.test(1))) {
      fChain->Branch("rho_hlt", &rho_hlt, "rho_hlt/F");
   }
   fChain->Branch("pthat", &pthat, "pthat/F");
   fChain->Branch("beta", &beta, "beta/F");
   fChain->Branch("betaStar", &betaStar, "betaStar/F");
   fChain->Branch("weight", &weight, "weight/F");
   fChain->Branch("refpvz", &refpvz, "refpvz/F");
   fChain->Branch("pudensity", &pudensity, "pudensity/F");
   fChain->Branch("gpudensity", &gpudensity, "gpudensity/F");
   fChain->Branch("npv", &npv, "npv/L");
   fChain->Branch("run", &run, "run/L");
   fChain->Branch("lumi", &lumi, "lumi/L");
   fChain->Branch("evt", &evt, "evt/L");
   fChain->Branch("nref", &nref, "nref/b");
   fChain->Branch("refrank", "vector<UChar_t>", &refrank);
   fChain->Branch("refpdgid", "vector<Int_t>", &refpdgid);
   if(!flags.test(0) || (flags.test(0) && flags.test(2))) {
      fChain->Branch("refpdgid_algorithmicDef", "vector<Int_t>", &refpdgid_algorithmicDef);
      fChain->Branch("refpdgid_physicsDef", "vector<Int_t>", &refpdgid_physicsDef);
   }
   fChain->Branch("refe", "vector<Float_t>", &refe);
   fChain->Branch("refpt", "vector<Float_t>", &refpt);
   fChain->Branch("refeta", "vector<Float_t>", &refeta);
   fChain->Branch("refphi", "vector<Float_t>", &refphi);
   fChain->Branch("refy", "vector<Float_t>", &refy);
   if(!flags.test(0) || (flags.test(0) && flags.test(3))) {
      fChain->Branch("refdphijt", "vector<Float_t>", &refdphijt);
   }
   if(!flags.test(0) || (flags.test(0) && !flags.test(3))) {
      fChain->Branch("refdrjt", "vector<Float_t>", &refdrjt);
   }
   fChain->Branch("refarea", "vector<Float_t>", &refarea);
   fChain->Branch("jte", "vector<Float_t>", &jte);
   fChain->Branch("jtpt", "vector<Float_t>", &jtpt);
   fChain->Branch("jteta", "vector<Float_t>", &jteta);
   fChain->Branch("jtphi", "vector<Float_t>", &jtphi);
   fChain->Branch("jty", "vector<Float_t>", &jty);
   fChain->Branch("jtjec", "vector<Float_t>", &jtjec);
   fChain->Branch("jtarea", "vector<Float_t>", &jtarea);
   if(!flags.test(0) || (flags.test(0) && flags.test(4))) {
      if (!flags.test(0) || (flags.test(0) && flags.test(5))) {
         fChain->Branch("jtemf", "vector<Float_t>", &jtemf);
      }    
      if (!flags.test(0) || (flags.test(0) && flags.test(6))) {
         fChain->Branch("jtchf", "vector<Float_t>", &jtchf);
         fChain->Branch("jtnhf", "vector<Float_t>", &jtnhf);
         fChain->Branch("jtnef", "vector<Float_t>", &jtnef);
         fChain->Branch("jtcef", "vector<Float_t>", &jtcef);
         fChain->Branch("jtmuf", "vector<Float_t>", &jtmuf);
         fChain->Branch("jthfhf", "vector<Float_t>", &jthfhf);
         fChain->Branch("jthfef", "vector<Float_t>", &jthfef);

         fChain->Branch("refnMult", "vector<Int_t>", &refnMult);
         fChain->Branch("refchMult", "vector<Int_t>", &refchMult);
         fChain->Branch("jtnMult", "vector<Int_t>", &jtnMult);
         fChain->Branch("jtchMult", "vector<Int_t>", &jtchMult);
      }
   }
   if (!flags.test(0) || (flags.test(0) && flags.test(6) && flags.test(7))) {
      fChain->Branch("pfcand_px", "vector<Float_t>", &pfcand_px);
      fChain->Branch("pfcand_py", "vector<Float_t>", &pfcand_py);
      fChain->Branch("pfcand_pt", "vector<Float_t>", &pfcand_pt);
      fChain->Branch("pfcand_eta", "vector<Float_t>", &pfcand_eta);
      fChain->Branch("pfcand_phi", "vector<Float_t>", &pfcand_phi);
      fChain->Branch("pfcand_e", "vector<Float_t>", &pfcand_e);
      fChain->Branch("pfcand_id", "vector<JRAEvent::Flavor>", &pfcand_id);
   }
   fChain->Branch("refdzvtx", "vector<Float_t>", &refdzvtx);
   Notify();
}

void JRAEvent::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set vector pointers
   MakeVectors();

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   // Default values in case branch does not exist in tree
   weight = 1.0;

   // Set branch addresses
   fChain->SetBranchAddress("npus", &npus, &b_npus);
   fChain->SetBranchAddress("tnpus", &tnpus, &b_tnpus);
   fChain->SetBranchAddress("zpositions", &zpositions, &b_zpositions);
   fChain->SetBranchAddress("bxns", &bxns, &b_bxns);
   fChain->SetBranchAddress("sumpt_lowpt", &sumpt_lowpt, &b_sumpt_lowpt);
   fChain->SetBranchAddress("sumpt_highpt", &sumpt_highpt, &b_sumpt_highpt);
   fChain->SetBranchAddress("ntrks_lowpt", &ntrks_lowpt, &b_ntrks_lowpt);
   fChain->SetBranchAddress("ntrks_highpt", &ntrks_highpt, &b_ntrks_highpt);
   fChain->SetBranchAddress("rhos", &rhos, &b_rhos);
   fChain->SetBranchAddress("rho", &rho, &b_rho);
   if(!flags.test(0) || (flags.test(0) && flags.test(1))) {
      fChain->SetBranchAddress("rho_hlt", &rho_hlt, &b_rho_hlt);
   }
   fChain->SetBranchAddress("pthat", &pthat, &b_pthat);
   fChain->SetBranchAddress("beta", &beta, &b_beta);
   fChain->SetBranchAddress("betaStar", &betaStar, &b_betaStar);
   fChain->SetBranchAddress("weight", &weight, &b_weight);
   fChain->SetBranchAddress("refpvz", &refpvz, &b_refpvz);
   fChain->SetBranchAddress("pudensity", &pudensity, &b_pudensity);
   fChain->SetBranchAddress("gpudensity", &gpudensity, &b_gpudensity);
   fChain->SetBranchAddress("npv", &npv, &b_npv);
   fChain->SetBranchAddress("run", &run, &b_run);
   fChain->SetBranchAddress("lumi", &lumi, &b_lumi);
   fChain->SetBranchAddress("evt", &evt, &b_evt);
   fChain->SetBranchAddress("nref", &nref, &b_nref);
   fChain->SetBranchAddress("refrank", &refrank, &b_refrank);
   fChain->SetBranchAddress("refpdgid", &refpdgid, &b_refpdgid);
   if(!flags.test(0) || (flags.test(0) && flags.test(2))) {
      fChain->SetBranchAddress("refpdgid_algorithmicDef", &refpdgid_algorithmicDef, &b_refpdgid_algorithmicDef);
      fChain->SetBranchAddress("refpdgid_physicsDef", &refpdgid_physicsDef, &b_refpdgid_physicsDef);
   }
   fChain->SetBranchAddress("refe", &refe, &b_refe);
   fChain->SetBranchAddress("refpt", &refpt, &b_refpt);
   fChain->SetBranchAddress("refeta", &refeta, &b_refeta);
   fChain->SetBranchAddress("refphi", &refphi, &b_refphi);
   fChain->SetBranchAddress("refy", &refy, &b_refy);
   if(!flags.test(0) || (flags.test(0) && flags.test(3))) {
      fChain->SetBranchAddress("refdphijt", &refdphijt, &b_refdphijt);
   }
   if(!flags.test(0) || (flags.test(0) && !flags.test(3))) {
     fChain->SetBranchAddress("refdrjt", &refdrjt, &b_refdrjt);
   }
   fChain->SetBranchAddress("refarea", &refarea, &b_refarea);
   fChain->SetBranchAddress("jte", &jte, &b_jte);
   fChain->SetBranchAddress("jtpt", &jtpt, &b_jtpt);
   fChain->SetBranchAddress("jteta", &jteta, &b_jteta);
   fChain->SetBranchAddress("jtphi", &jtphi, &b_jtphi);
   fChain->SetBranchAddress("jty", &jty, &b_jty);
   fChain->SetBranchAddress("jtjec", &jtjec, &b_jtjec);
   fChain->SetBranchAddress("jtarea", &jtarea, &b_jtarea);
   if(!flags.test(0) || (flags.test(0) && flags.test(4))) {
      if (!flags.test(0) || (flags.test(0) && flags.test(5))) {
         fChain->SetBranchAddress("jtemf", &jtemf, &b_jtemf);
      }
      if (!flags.test(0) || (flags.test(0) && flags.test(6))) {
         fChain->SetBranchAddress("jtchf", &jtchf, &b_jtchf);
         fChain->SetBranchAddress("jtnhf", &jtnhf, &b_jtnhf);
         fChain->SetBranchAddress("jtnef", &jtnef, &b_jtnef);
         fChain->SetBranchAddress("jtcef", &jtcef, &b_jtcef);
         fChain->SetBranchAddress("jtmuf", &jtmuf, &b_jtmuf);
         fChain->SetBranchAddress("jthfhf", &jthfhf, &b_jthfhf);
         fChain->SetBranchAddress("jthfef", &jthfef, &b_jthfef);

         fChain->SetBranchAddress("refnMult", &refnMult, &b_refnMult);
         fChain->SetBranchAddress("refchMult", &refchMult, &b_refchMult);
         fChain->SetBranchAddress("jtnMult", &jtnMult, &b_jtnMult);
         fChain->SetBranchAddress("jtchMult", &jtchMult, &b_jtchMult);
      }
   }
   if (!flags.test(0) || (flags.test(0) && flags.test(6) && flags.test(7))) {
      fChain->SetBranchAddress("pfcand_px", &pfcand_px, &b_pfcand_px);
      fChain->SetBranchAddress("pfcand_py", &pfcand_py, &b_pfcand_py);
      fChain->SetBranchAddress("pfcand_pt", &pfcand_pt, &b_pfcand_pt);
      fChain->SetBranchAddress("pfcand_eta", &pfcand_eta, &b_pfcand_eta);
      fChain->SetBranchAddress("pfcand_phi", &pfcand_phi, &b_pfcand_phi);
      fChain->SetBranchAddress("pfcand_e", &pfcand_e, &b_pfcand_e);
      fChain->SetBranchAddress("pfcand_id", &pfcand_id, &b_pfcand_id);
   }
   fChain->SetBranchAddress("refdzvtx", &refdzvtx, &b_refdzvtx);
   Notify();
}

Bool_t JRAEvent::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void JRAEvent::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}

Int_t JRAEvent::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}

void JRAEvent::MakeVectors()
{
   npus                    = new vector<int>;
   tnpus                   = new vector<float>;
   zpositions              = new vector<float>;
   bxns                    = new vector<int>;
   sumpt_lowpt             = new vector<float>;
   sumpt_highpt            = new vector<float>;
   ntrks_lowpt             = new vector<int>;
   ntrks_highpt            = new vector<int>;
   rhos                    = new vector<float>;
   refrank                 = new vector<UChar_t>;
   refpdgid                = new vector<int>;
   if(!flags.test(0) || (flags.test(0) && flags.test(2))) {
      refpdgid_algorithmicDef = new vector<int>;
      refpdgid_physicsDef     = new vector<int>;
   }
   refe                    = new vector<float>;
   refpt                   = new vector<float>;
   refeta                  = new vector<float>;
   refphi                  = new vector<float>;
   refy                    = new vector<float>;
   if(!flags.test(0) || (flags.test(0) && flags.test(3))) {
      refdphijt               = new vector<float>;
   }
   if(!flags.test(0) || (flags.test(0) && !flags.test(3))) {
      refdrjt                 = new vector<float>;
   }
   refarea                 = new vector<float>;
   jte                     = new vector<float>;
   jtpt                    = new vector<float>;
   jteta                   = new vector<float>;
   jtphi                   = new vector<float>;
   jty                     = new vector<float>;
   jtjec                   = new vector<float>;
   jtarea                  = new vector<float>;
   if(!flags.test(0) || (flags.test(0) && flags.test(4))) {
      if (!flags.test(0) || (flags.test(0) && flags.test(5))) {
         jtemf                   = new vector<float>;
      }
      if (!flags.test(0) || (flags.test(0) && flags.test(6))) {
         jtchf                   = new vector<float>;
         jtnhf                   = new vector<float>;
         jtnef                   = new vector<float>;
         jtcef                   = new vector<float>;
         jtmuf                   = new vector<float>;
         jthfhf                  = new vector<float>;
         jthfef                  = new vector<float>;

         refnMult                = new vector<int>;
         refchMult               = new vector<int>;
         jtnMult                 = new vector<int>;
         jtchMult                = new vector<int>;
      }
   }
   if (!flags.test(0) || (flags.test(0) && flags.test(6) && flags.test(7))) {
      pfcand_px               = new vector<float>;
      pfcand_py               = new vector<float>;
      pfcand_pt               = new vector<float>;
      pfcand_eta              = new vector<float>;
      pfcand_phi              = new vector<float>;
      pfcand_e                = new vector<float>;
      pfcand_id               = new vector<Flavor>;
   }
   refdzvtx                = new vector<float>;
}

void JRAEvent::clear()
{
   npus->clear();
   tnpus->clear();
   zpositions->clear();
   bxns->clear();
   sumpt_lowpt->clear();
   sumpt_highpt->clear();
   ntrks_lowpt->clear();
   ntrks_highpt->clear();
   rhos->clear();
   refrank->clear();
   refpdgid->clear();
   if(!flags.test(0) || (flags.test(0) && flags.test(2))) {
      refpdgid_algorithmicDef->clear();
      refpdgid_physicsDef->clear();
   }
   refe->clear();
   refpt->clear();
   refeta->clear();
   refphi->clear();
   refy->clear();
   if(!flags.test(0) || (flags.test(0) && flags.test(3))) {
      refdphijt->clear();
   }
   if(!flags.test(0) || (flags.test(0) && !flags.test(3))) {
      refdrjt->clear();
   }
   refarea->clear();
   jte->clear();
   jtpt->clear();
   jteta->clear();
   jtphi->clear();
   jty->clear();
   jtjec->clear();
   jtarea->clear();
   if(!flags.test(0) || (flags.test(0) && flags.test(4))) {
      if (!flags.test(0) || (flags.test(0) && flags.test(5))) {
         jtemf->clear();
      }
      if (!flags.test(0) || (flags.test(0) && flags.test(6))) {
         jtchf->clear();
         jtnhf->clear();
         jtnef->clear();
         jtcef->clear();
         jtmuf->clear();
         jthfhf->clear();
         jthfef->clear();

         refnMult->clear();
         refchMult->clear();
         jtnMult->clear();
         jtchMult->clear();
      }
   }
   if (!flags.test(0) || (flags.test(0) && flags.test(6) && flags.test(7))) {
      pfcand_px->clear();
      pfcand_py->clear();
      pfcand_pt->clear();
      pfcand_eta->clear();
      pfcand_phi->clear();
      pfcand_e->clear();
      pfcand_id->clear();
   }
   refdzvtx->clear();
}

void JRAEvent::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L JRAEvent.C
//      Root > JRAEvent t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
   }
}

//______________________________________________________________________________
int JRAEvent::itIndex() {
   for(unsigned int ibx=0; ibx<(*bxns).size(); ibx++) {
      if((*bxns)[ibx]==0) return ibx;
   }
   return -1;
}

//______________________________________________________________________________
double JRAEvent::sumEOOT() {
   int iIT = itIndex();
   if(iIT>(int)(*npus).size()-1) return 0;
   double sum = 0;
   for(int ipu=0; ipu<iIT; ipu++) {
      sum+=(*npus)[ipu];
   }
   return sum;
}

//______________________________________________________________________________
double JRAEvent::sumLOOT() {
   int iIT = itIndex();
   if(iIT>(int)(*npus).size()-1) return 0;
   double sum = 0;
   for(int ipu=(*npus).size()-1; ipu>iIT; ipu--) {
      sum+=(*npus)[ipu];
   }
   return sum;
}

//______________________________________________________________________________
bool JRAEvent::CheckValue(ROOT::Internal::TTreeReaderValueBase* value) {
   if (value->GetSetupStatus() < 0) {
      std::cerr << "Error " << value->GetSetupStatus()
                << "setting up reader for " << value->GetBranchName() << '\n';
      return false;
   }
   return true;
}
//#endif // #ifdef JetUtilities_JRAEvent_cxx

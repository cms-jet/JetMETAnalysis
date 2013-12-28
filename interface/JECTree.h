//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Oct 11 13:09:34 2012 by ROOT version 5.32/00
// from TTree t/t
// found on file: /uscms_data/d2/aperloff/JRA_outfiles_53X_20120911/JRA/JRA.root
//////////////////////////////////////////////////////////

#ifndef JECTree_h
#define JECTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include <vector>
using namespace std;

// Fixed size dimensions of array or collections stored in the TTree if any.

class JECTree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   vector<int>     *npus;
   vector<float>   *tnpus;
   vector<int>     *bxns;
   vector<float>   *sumpt_lowpt;
   vector<float>   *sumpt_highpt;
   vector<int>     *ntrks_lowpt;
   vector<int>     *ntrks_highpt;
   Float_t         rho;
   Float_t         rho50;
   Float_t         rho_hlt;
   Float_t         pthat;
   Float_t         weight;
   Long64_t        npv;
   Long64_t        evt;
   Long64_t        run;
   UChar_t         nref;
   UChar_t         refrank[61];   //[nref]
   Int_t           refpdgid[61];   //[nref]
   Float_t         refe[61];   //[nref]
   Float_t         refpt[61];   //[nref]
   Float_t         refeta[61];   //[nref]
   Float_t         refphi[61];   //[nref]
   Float_t         refy[61];   //[nref]
   Float_t         refdrjt[61];   //[nref]
   Float_t         refarea[61];   //[nref]
   Float_t         jte[61];   //[nref]
   Float_t         jtpt[61];   //[nref]
   Float_t         jteta[61];   //[nref]
   Float_t         jtphi[61];   //[nref]
   Float_t         jty[61];   //[nref]
   Float_t         jtjec[61];   //[nref]
   Float_t         jtarea[61];   //[nref]
   Float_t         jtchf[61];   //[nref]
   Float_t         jtnhf[61];   //[nref]
   Float_t         jtnef[61];   //[nref]
   Float_t         jtcef[61];   //[nref]
   Float_t         jtmuf[61];   //[nref]
   Float_t         jthfhf[61];   //[nref]
   Float_t         jthfef[61];   //[nref]

   // List of branches
   TBranch        *b_npus;   //!
   TBranch        *b_tnpus;   //!
   TBranch        *b_bxns;   //!
   TBranch        *b_sumpt_lowpt;   //!
   TBranch        *b_sumpt_highpt;   //!
   TBranch        *b_ntrks_lowpt;   //!
   TBranch        *b_ntrks_highpt;   //!
   TBranch        *b_rho;   //!
   TBranch        *b_rho50;   //!
   TBranch        *b_rho_hlt;   //!
   TBranch        *b_pthat;   //!
   TBranch        *b_weight;   //!
   TBranch        *b_npv;   //!
   TBranch        *b_evt;   //!
   TBranch        *b_run;   //!
   TBranch        *b_nref;   //!
   TBranch        *b_refrank;   //!
   TBranch        *b_refpdgid;   //!
   TBranch        *b_refe;   //!
   TBranch        *b_refpt;   //!
   TBranch        *b_refeta;   //!
   TBranch        *b_refphi;   //!
   TBranch        *b_refy;   //!
   TBranch        *b_refdrjt;   //!
   TBranch        *b_refarea;   //!
   TBranch        *b_jte;   //!
   TBranch        *b_jtpt;   //!
   TBranch        *b_jteta;   //!
   TBranch        *b_jtphi;   //!
   TBranch        *b_jty;   //!
   TBranch        *b_jtjec;   //!
   TBranch        *b_jtarea;   //!
   TBranch        *b_jtchf;   //!
   TBranch        *b_jtnhf;   //!
   TBranch        *b_jtnef;   //!
   TBranch        *b_jtcef;   //!
   TBranch        *b_jtmuf;   //!
   TBranch        *b_jthfhf;   //!
   TBranch        *b_jthfef;   //!

   JECTree(TTree *tree=0);
   virtual ~JECTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   //   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif


////////////////////////////////////////////////////////////////////////////////
//
// JetResponseAnalyzer
// -------------------
//
//            12/28/2013 Alexx Perloff          <alexx.stephen.perloff@cern.ch>
////////////////////////////////////////////////////////////////////////////////

#ifndef JetUtilities_JRAEvent_h
#define JetUtilities_JRAEvent_h

#include "TROOT.h"
#include "TChain.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"

// Header file for the classes stored in the TTree if any.
#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <bitset>

// Fixed size dimensions of array or collections stored in the TTree if any.

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::pair;
using std::bitset;

class JRAEvent {
public :
   enum Flavor{all=0, h, e, mu, gamma, h0, h_HF, egamma_HF, chs, numFlavors, X}; //X=undefined

   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Configuration flags
   // Bit 0: useFlags
   // Bit 1: doHLT_
   // Bit 2: doFlavor_
   // Bit 3: doBalancing_
   // Bit 4: doComposition_
   // Bit 5: isCaloJet_
   // Bit 6: isPFJet_
   // Bit 7: saveCandidates_
   bitset<8> flags;   //!

   // Declaration of leaf types
   vector<int>*     npus;
   vector<float>*   tnpus;
   vector<float>*   zpositions;
   vector<int>*     bxns;
   vector<float>*   sumpt_lowpt;
   vector<float>*   sumpt_highpt;
   vector<int>*     ntrks_lowpt;
   vector<int>*     ntrks_highpt;
   vector<float>*   rhos;
   Float_t          rho;
   Float_t          rho_hlt;
   Float_t          pthat;
   Float_t          beta;
   Float_t          betaStar;
   Float_t          weight;
   Float_t          refpvz;
   Float_t          pudensity;
   Float_t          gpudensity;
   Long64_t         npv;
   Long64_t         run;
   Long64_t         lumi;
   Long64_t         evt;
   UChar_t          nref;
   vector<UChar_t>* refrank;
   vector<Int_t>*   refpdgid;
   vector<Int_t>*   refpdgid_algorithmicDef;
   vector<Int_t>*   refpdgid_physicsDef;
   vector<Float_t>* refe;
   vector<Float_t>* refpt;
   vector<Int_t>*   refnMult;
   vector<Int_t>*   refchMult;
   vector<Float_t>* refeta;
   vector<Float_t>* refphi;
   vector<Float_t>* refy;
   vector<Float_t>* refdphijt;
   vector<Float_t>* refdrjt;
   vector<Float_t>* refarea;
   vector<Float_t>* jte;
   vector<Float_t>* jtpt;
   vector<Int_t>*   jtnMult;
   vector<Int_t>*   jtchMult;
   vector<Float_t>* jteta;
   vector<Float_t>* jtphi;
   vector<Float_t>* jty;
   vector<Float_t>* jtjec;
   vector<Float_t>* jtarea;
   vector<Float_t>* jtemf;
   vector<Float_t>* jtchf;
   vector<Float_t>* jtnhf;
   vector<Float_t>* jtnef;
   vector<Float_t>* jtcef;
   vector<Float_t>* jtmuf;
   vector<Float_t>* jthfhf;
   vector<Float_t>* jthfef;
   vector<Float_t>* pfcand_px;
   vector<Float_t>* pfcand_py;
   vector<Float_t>* pfcand_pt;
   vector<Float_t>* pfcand_eta;
   vector<Float_t>* pfcand_phi;
   vector<Float_t>* pfcand_e;
   vector<Flavor>*  pfcand_id;
   vector<Float_t>* refdzvtx;

   // List of branches
   TBranch        *b_npus;   //!
   TBranch        *b_tnpus;   //!
   TBranch        *b_zpositions;   //!
   TBranch        *b_bxns;   //!
   TBranch        *b_sumpt_lowpt;   //!
   TBranch        *b_sumpt_highpt;   //!
   TBranch        *b_ntrks_lowpt;   //!
   TBranch        *b_ntrks_highpt;   //!
   TBranch        *b_rhos;   //!
   TBranch        *b_rho;   //!
   TBranch        *b_rho_hlt;   //!
   TBranch        *b_pthat;   //!
   TBranch        *b_beta;   //!
   TBranch        *b_betaStar;   //!
   TBranch        *b_weight;   //!
   TBranch        *b_refpvz;   //!
   TBranch        *b_pudensity;   //!
   TBranch        *b_gpudensity;   //!
   TBranch        *b_npv;   //!
   TBranch        *b_run;   //!
   TBranch        *b_lumi;   //!
   TBranch        *b_evt;   //!
   TBranch        *b_nref;   //!
   TBranch        *b_refrank;   //!
   TBranch        *b_refpdgid;   //!
   TBranch        *b_refpdgid_algorithmicDef;   //!
   TBranch        *b_refpdgid_physicsDef;   //!
   TBranch        *b_refe;   //!
   TBranch        *b_refpt;   //!
   TBranch        *b_refnMult;   //!
   TBranch        *b_refchMult;   //!
   TBranch        *b_refeta;   //!
   TBranch        *b_refphi;   //!
   TBranch        *b_refy;   //!
   TBranch        *b_refdphijt;   //!
   TBranch        *b_refdrjt;   //!
   TBranch        *b_refarea;   //!
   TBranch        *b_jte;   //!
   TBranch        *b_jtpt;   //!
   TBranch        *b_jtnMult;   //!
   TBranch        *b_jtchMult;   //!
   TBranch        *b_jteta;   //!
   TBranch        *b_jtphi;   //!
   TBranch        *b_jty;   //!
   TBranch        *b_jtjec;   //!
   TBranch        *b_jtarea;   //!
   TBranch        *b_jtemf;   //!
   TBranch        *b_jtchf;   //!
   TBranch        *b_jtnhf;   //!
   TBranch        *b_jtnef;   //!
   TBranch        *b_jtcef;   //!
   TBranch        *b_jtmuf;   //!
   TBranch        *b_jthfhf;   //!
   TBranch        *b_jthfef;   //!
   TBranch        *b_pfcand_px;   //!
   TBranch        *b_pfcand_py;   //!
   TBranch        *b_pfcand_pt;   //!
   TBranch        *b_pfcand_eta;   //!
   TBranch        *b_pfcand_phi;   //!
   TBranch        *b_pfcand_e;   //!
   TBranch        *b_pfcand_id;   //!
   TBranch        *b_refdzvtx;   //!

   JRAEvent(TTree *tree = 0, bitset<8> b = 0);
   virtual ~JRAEvent();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
   virtual void     MakeTree(TTree *tree);
   virtual void     MakeVectors();
   virtual void     clear();
   int              itIndex();
   double           sumEOOT();
   double           sumLOOT();
   bool             CheckValue(ROOT::Internal::TTreeReaderValueBase* value);
};

// this is our new product, it is simply a 
// collection of SampleProd held in an std::vector
typedef std::vector<JRAEvent> JRAEventCollection;

#endif

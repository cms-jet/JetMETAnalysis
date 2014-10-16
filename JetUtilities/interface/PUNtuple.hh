#ifndef PUNTUPLE_HH
#define PUNTUPLE_HH

#include <TObject.h>

#include <vector>

using std::vector;

class PUNtuple : public TObject
{
public: 
      //
      // Construction/Destruction
      //
      PUNtuple();
      ~PUNtuple();

      Long64_t        run;
      Long64_t        lumi;
      Long64_t        evt;
      Long64_t        npv;
      Float_t         rho;
      Float_t         refpt[92];
      Float_t         refeta[92];
      Float_t         jtpt[92];
      Float_t         jteta[92];
      Float_t         jtptNoPU[92];
      Float_t         jtetaNoPU[92];
      Float_t         offset[92];
      vector<int>*    npus;
      vector<float>*  tnpus;
      vector<int>*    bxns;

      ClassDef(PUNtuple,1)
};

#endif

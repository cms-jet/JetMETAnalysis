#ifndef TProfileMDF_HH
#define TProfileMDF_HH

#include "TH1F.h"
#include "TClass.h"
#include "TAxis.h"

#include <vector>

class TProfileHelper;

class TProfileMDF : public TH1F{

enum EErrorType { kERRORMEAN = 0, kERRORSPREAD, kERRORSPREADI, kERRORSPREADG };

public:
   TProfileMDF();
   TProfileMDF(const char*name, const char *title);
   ~TProfileMDF();
 
   // Member Functions
   void               AddAxis(TString axisTitle, Int_t nbins, Double_t xlow, Double_t xup);
   void               AddAxis(TString axisTitle, Int_t nbins, const Double_t* xbins);
   void               AddAxis(TAxis* faxis);
   void               AddAxes(std::vector<TAxis*> faxes);
   TAxis*             GetAxis(Int_t axisNumber);
   Double_t           GetBinContent(const std::vector<Int_t> & binCoord);
   virtual Double_t   GetBinContent(Int_t bin) const;
   Double_t           GetBinEffectiveEntries(const std::vector<Int_t> & binCoord);
   virtual Double_t   GetBinEffectiveEntries(Int_t globalBin) const;
   Double_t           GetBinError(const std::vector<Int_t> & binCoord);
   virtual Double_t   GetBinError(Int_t globalBin) const;
   std::vector<Int_t> GetBins(Int_t binglobal);
   Int_t              GetGlobalBin(const std::vector<Int_t> & binCoord);
   Int_t              GetNaxis() { return fAxes.size(); }
   Int_t              GetNbins() { return fNcells; }
   Int_t              GetNGlobalBins() { return fNcells; }
   Int_t              FindBin(const std::vector<Double_t> & xcoor);
   void               Fill(const std::vector<Double_t> & xcoor, Double_t val, Double_t weight=1);
   void               LoopOverBins();
   void               LoopOverBinsRaw();
   void               ReadFromFile(TString filename, TString treeName); 
   TProfileMDF*       ReduceDimensions(TString name, UInt_t axisNumber, Int_t firstbin, Int_t lastbin);
   void               SetBinContentError(Int_t bin, Double_t cont, Double_t sw2, Double_t bent, Double_t bsw2);
   void               SetErrorOption(Option_t * option);
   void               Test();
   void               TestRD();
   void               WriteToFile(TString filename, TString writeFlag = "RECREATE");

protected :

   std::vector<TAxis*> fAxes;    //vector holding all axes

   TArrayF     fBinEntries;      //number of entries per bin
   EErrorType  fErrorMode;       //Option to compute errors
   Bool_t      fScaling;         //!True when TProfile::Scale is called
   TArrayF     fBinSumw2;        //Array of sum of squares of weights per bin

   static Bool_t   fgApproximate;    //bin error approximation option

   ClassDef(TProfileMDF,1)  //class definition

};

#endif

#ifndef TProfileMDF_HH
#define TProfileMDF_HH

#include "TH1F.h"
#include "TClass.h"
#include "TAxis.h"

#include <vector>
#include <utility>
#include <string>

class TProfileHelper;

class TProfileMDF : public TH1F{

enum EErrorType { kERRORMEAN = 0, kERRORSPREAD, kERRORSPREADI, kERRORSPREADG };

public:
   TProfileMDF();
   TProfileMDF(const char*name, const char *title);
   ~TProfileMDF();
 
   // Member Functions
   virtual void       Add(TProfileMDF* h1, Double_t c1);
   virtual void   	  Add(TProfileMDF* h1, TProfileMDF* h2, Double_t c1 = 1, Double_t c2 = 1, Bool_t vetoAddAxis = false);
   void               AddAxis(TString axisTitle, Int_t nbins, Double_t xlow, Double_t xup);
   void               AddAxis(TString axisTitle, Int_t nbins, const Double_t* xbins);
   void               AddAxis(TAxis* faxis);
   void               AddAxes(std::vector<TAxis*> faxes);
   TH1F*              Get1DProjection(Int_t axisNumber, std::vector<Int_t> binCoord);
   TAxis*             GetAxis(Int_t axisNumber);
   Double_t           GetBinContent(const std::vector<Int_t> & binCoord);
   virtual Double_t   GetBinContent(Int_t bin) const;
   Double_t           GetBinEffectiveEntries(const std::vector<Int_t> & binCoord);
   virtual Double_t   GetBinEffectiveEntries(Int_t globalBin) const;
   Double_t           GetBinError(const std::vector<Int_t> & binCoord);
   virtual Double_t   GetBinError(Int_t globalBin) const;
   std::vector<Int_t> GetBins(Int_t binglobal) const;
   Int_t              GetGlobalBin(const std::vector<Int_t> & binCoord);
   Int_t              GetMaximumBin();
   Int_t              GetMaximumBin(std::vector<Int_t>& binCoord);
   Int_t              GetNaxis() { return fAxes.size(); }
   Int_t              GetNbins() { return fNcells; }
   Int_t              GetNGlobalBins() { return fNcells; }
   Int_t              FindBin(const std::vector<Double_t> & xcoor);
   void               Fill(const std::vector<Double_t> & xcoor, Double_t val, Double_t weight=1);
   Double_t           Integral(TString option = "");
   Double_t           Integral(std::vector<std::pair<Int_t,Int_t> > ranges, TString option = "");
   void               IntegralRecursive(Double_t& integral, std::vector<std::pair<Int_t,Int_t> > ranges, std::vector<Int_t>& coord, Int_t depth, TString option = "");
   virtual Bool_t     IsBinOverflow(Int_t bin) const;
   virtual Bool_t     IsBinUnderflow(Int_t bin) const;
   void               LoopOverBins();
   void               LoopOverBinsRaw();
   void               LoopOverBinsCoordinates();
   void               ReadFromFile(TString filename, TString treeName); 
   TProfileMDF*       ReduceDimensions(TString name, UInt_t axisNumber, Int_t firstbin, Int_t lastbin);
   virtual void	      Scale(Double_t c1 = 1, Option_t* option = "");
   void               SetBinContentError(Int_t bin, Double_t cont, Double_t sw2, Double_t bent, Double_t bsw2);
   void               SetErrorOption(Option_t * option);
   void               Test();
   void               TestRD();
   void               Test2D();
   void               Test3D();
   void               WriteToFile(TString filename, TString writeFlag = "RECREATE");

protected :

   std::vector<TAxis*> fAxes;    //vector holding all axes

   TArrayF     fBinEntries;      //number of entries per bin
   EErrorType  fErrorMode;       //Option to compute errors
   Bool_t      fScaling;         //!True when TProfile::Scale is called
   TArrayF     fBinSumw2;        //Array of sum of squares of weights per bin

   static Bool_t   fgApproximate;    //bin error approximation option

   Float_t * GetW() {return &fArray[0];}
   Double_t * GetW2() {return &fSumw2.fArray[0];}
   Float_t * GetB() {return &fBinEntries.fArray[0];}
   Float_t * GetB2() {return (fBinSumw2.fN ? &fBinSumw2.fArray[0] : 0 ); }

   ClassDef(TProfileMDF,1)  //class definition

};

#endif

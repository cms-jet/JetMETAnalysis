#ifndef TProfileMDD_HH
#define TProfileMDD_HH

#include "TH1D.h"
#include "TClass.h"
#include "TAxis.h"

#include <vector>

class TProfileHelper;

class TProfileMDD : public TH1D{

enum EErrorType { kERRORMEAN = 0, kERRORSPREAD, kERRORSPREADI, kERRORSPREADG };

public:
   TProfileMDD();
   TProfileMDD(const char*name, const char *title);
   ~TProfileMDD();
 
   // Member Methods
   void    AddAxis(TString axisTitle, Int_t nbins, Double_t xlow, Double_t xup);
   void    AddAxis(TString axisTitle, Int_t nbins, const Double_t* xbins);
   Int_t   GetNaxis() { return fAxes.size();}
   TAxis * GetAxis(Int_t axisNumber);
   Int_t   GetGlobalBin(const std::vector<Int_t> & binCoord);
   Int_t   FindBin(const std::vector<Double_t> & xcoor);
   void    Fill(const std::vector<Double_t> & xcoor, Double_t val, Double_t weight=1);

   Double_t GetBinContent(const std::vector<Int_t> & binCoord);
   virtual Double_t GetBinContent(Int_t bin) const;
   
   Double_t GetBinError(const std::vector<Int_t> & binCoord);
   virtual Double_t GetBinError(Int_t globalBin) const;

   virtual Double_t GetBinEffectiveEntries(Int_t globalBin) const;

   void Test();
   void LoopOverBins();

   TProfileMDD* ReadFromFile(TString filename, TString objectName);
   void WriteToFile(TString filename, TString writeFlag = "RECREATE");

protected :

   // Vector holding all axes
   std::vector<TAxis*> fAxes;
   //std::vector<Double_t> sumws;
   //std::vector<Double_t> sumws2;

   TArrayD     fBinEntries;      //number of entries per bin
   EErrorType  fErrorMode;       //Option to compute errors
   Bool_t      fScaling;         //!True when TProfile::Scale is called
   TArrayD     fBinSumw2;        //Array of sum of squares of weights per bin

   static Bool_t   fgApproximate;    //bin error approximation option

   ClassDef(TProfileMDD,1)  //

};

#endif

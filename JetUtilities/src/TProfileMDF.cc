#include "JetMETAnalysis/JetUtilities/interface/TProfileMDF.h"

#include "TROOT.h"
#include "TMath.h"
#include "Riostream.h"
#include "TRandom3.h"
#include "TFile.h"
#include "TKey.h"
#include "TTree.h"
#include "TAxis.h"
#include "TH1.h"
#include <iostream>
#include <assert.h>
#include <sstream>
#include <algorithm>
#include <iomanip>

using namespace std;
ClassImp(TProfileMDF)

using std::cout;
using std::endl;
using std::vector;
using std::stringstream;

Bool_t TProfileMDF::fgApproximate = kFALSE;

bool axisComparison (TAxis* i,TAxis* j) { return (TString(i->GetName()).CompareTo(j->GetName())<0); }


/* TO TEST

root -l TProfileMDF.c+ // Ignore the warnings during compilation
TFile * f = new TFile("a.root","RECREATE")
TProfileMDF j("H","k")
j.Test()
j->Write()
f->Close()
.q

root -l TProfileMDF.c+
TFile *_file0 = TFile::Open("a.root")

*/

/* TO TEST 2
.L ~/JEC/CMSSW_4_2_4/lib/slc5_amd64_gcc434/libJetMETAnalysisJetUtilities.so
TProfileMDF test("test","test")
test.Test()
test.WriteToFile("test.root")

TProfileMDF test2("test2","test2")
test2.ReadFromFile("test.root","test")

test.LoopOverBinsRaw()
test2.LoopOverBinsRaw()

test.LoopOverBins()
test2.LoopOverBins()
*/

/* TO USE

root -l TProfileMDF.cc+ // Ignore the warnings during compilation

TProfileMDF * h = new TProfileMDF("H","K")
h->AddAxis("x",30,0,130);
h->AddAxis("y",20,0,3500);
h->AddAxis("z",100,0,800);

// Vector of coordinates initialized to have h->GetNaxis() elements
// coord[0]=x, coord[1]=y, and so on...
vector<Double_t> coord (3,0);

// Fill the coord vector with the (x,y,z,...) coordinates
coord[0] = x ;// x
coord[0] = y ;// y

// fill the x,y coordinates with the value 56 (can use weights too)
h->Fill(coord, 56);

// Bin -> Vector of bins
vector<Int_t> bin (h->GetNaxis(),0);
bin[0] = 2;
bin[1] = 2;

h->GetBinContent(bin)
h->GetBinError(bin)

*/


// --------------------------------------------------------------
TProfileMDF::TProfileMDF() : TH1F() { }

// --------------------------------------------------------------
TProfileMDF::~TProfileMDF(){ }

// --------------------------------------------------------------
TProfileMDF::TProfileMDF(const char*name, const char *title) : TH1F() {
  
  // Set the number of cells to zero
  fNcells = 0;

  fErrorMode = kERRORMEAN;
  TH1F::SetName(name);
  TH1F::SetTitle(title);

}

// --------------------------------------------------------------
void TProfileMDF::Add(TProfileMDF* h1, Double_t c1) {
   // Performs the operation: this = this + c1*h1

   if (!h1) {
      Error("Add","Attempt to add a non-existing TProfileMDF");
      return;
   }
   
   Add(this, h1, 1, c1);
}

// --------------------------------------------------------------
void TProfileMDF::Add(TProfileMDF *h1, TProfileMDF *h2, Double_t c1, Double_t c2, Bool_t vetoAddAxis) {
//   -*-*-*Replace contents of this histogram by the addition of h1 and h2*-*-*
//         ===============================================================
//
//   this = c1*h1 + c2*h2
//   if errors are defined (see TH1::Sumw2), errors are also recalculated
//   Note that if h1 or h2 have Sumw2 set, Sumw2 is automatically called for this
//   if not already set.
//
// SPECIAL CASE (Average/Efficiency histograms)
// For histograms representing averages or efficiencies, one should compute the average
// of the two histograms and not the sum. One can mark a histogram to be an average
// histogram by setting its bit kIsAverage with
//    myhist.SetBit(TH1::kIsAverage);
// Note that the two histograms must have their kIsAverage bit set
//
// IMPORTANT NOTE: If you intend to use the errors of this histogram later
// you should call Sumw2 before making this operation.
// This is particularly important if you fit the histogram after TH1::Add
   
   if (!h1 || !h2) {
      Error("Add","Attempt to add a non-existing histogram");
      return;
   }

   Bool_t normWidth = kFALSE;
   if (h1 == h2 && c2 < 0) {
      Warning("Add","Normalizing h1 by the width of each cell.");
      c2 = 0; normWidth = kTRUE;
   }
   
   Int_t naxis = 0;
   TProfileMDF* atemp;
   if(GetNaxis()==0) {
      Warning("Add","Attempt to add to a TProfileMDF without any axes. Setting vetoAddAxis to false.");
      vetoAddAxis=false;
      if(h1!=this) {
         naxis = h1->GetNaxis();
         atemp = h1;
      }
      else if(h2!=this) {
         naxis = h2->GetNaxis();
         atemp = h2;
      }
      else {
         Error("Add","Attempt to add histogram to itself when \"this\", \"h1\", and \"h2\" have no axes.");
         return;
      }
   }

//   - Add Axes
   vector<Int_t> nbins;
   for(Int_t a=0; a<naxis; a++) {
      if(!vetoAddAxis) {
         cout << " TProfileMDF::Add::Adding an axis with nBins=" << atemp->GetAxis(a)->GetNbins() << endl;
         AddAxis(atemp->GetAxis(a));
      }
      nbins.push_back(fAxes[a]->GetNbins());
//   - Check histogram compatibility
      if(nbins.back() != h1->GetAxis(a)->GetNbins() || nbins.back() != h2->GetAxis(a)->GetNbins()) {
         Error("Add","Attempt to add histograms with different number of bins");
         return;
      }
//   - Issue a Warning if histogram limits are different
      if(fAxes[a]->GetXmin() != h1->GetAxis(a)->GetXmin() ||
         fAxes[a]->GetXmax() != h1->GetAxis(a)->GetXmax()) {
         Warning("Add",Form("Attempt to add histograms (this=[%f,%f],h1=[%f,%f]) with different axis limits",fAxes[a]->GetXmin(),fAxes[a]->GetXmax(),h1->GetAxis(a)->GetXmin(),h1->GetAxis(a)->GetXmax()));
      }
      if(fAxes[a]->GetXmin() != h2->GetAxis(a)->GetXmin() ||
         fAxes[a]->GetXmax() != h2->GetAxis(a)->GetXmax()) {
         Warning("Add",Form("Attempt to add histograms (this=[%f,%f],h2=[%f,%f]) with different axis limits",fAxes[a]->GetXmin(),fAxes[a]->GetXmax(),h2->GetAxis(a)->GetXmin(),h2->GetAxis(a)->GetXmax()));
      }      
   }

//   - Create Sumw2 if h1 or h2 have Sumw2 set
   if (fSumw2.fN == 0 && (h1->GetSumw2N() != 0 || h2->GetSumw2N() != 0)) Sumw2();

//   - Add statistics
   Double_t nEntries = c1*h1->GetEntries() + c2*h2->GetEntries();
   Double_t s1[kNstat], s2[kNstat], s3[kNstat];
   Int_t i;
   for (i=0;i<kNstat;i++) {s1[i] = s2[i] = s3[i] = 0;}
   h1->GetStats(s1);
   h2->GetStats(s2);
   for (i=0;i<kNstat;i++) {
      if (i == 1) s3[i] = c1*c1*s1[i] + c2*c2*s2[i];
      else        s3[i] = TMath::Abs(c1)*s1[i] + TMath::Abs(c2)*s2[i];
   }

   SetMinimum();
   SetMaximum();

//    Reset the kCanRebin option. Otherwise SetBinContent on the overflow bin
//    would resize the axis limits!
   //ResetBit(TAxis::kCanExtend);
	 TH1::SetCanExtend(kNoAxis);

//   - Loop on bins (including underflows/overflows)
   vector<Int_t> coord(fAxes.size(),0);
   vector<Double_t> widths(fAxes.size(),0.0);
   Float_t cu;
   Float_t *cu1 = h1->GetW();    Float_t *cu2 = h2->GetW();
   Double_t *er1 = h1->GetW2();  Double_t *er2 = h2->GetW2();
   Float_t *en1 = h1->GetB();    Float_t *en2 = h2->GetB();
   Float_t *ew1 = h1->GetB2();   Float_t *ew2 = h2->GetB2();
   // if p1 has not the sum of weight squared/bin stored use just the sum of weights  
   if (ew1 == 0) ew1 = en1;
   if (ew2 == 0) ew2 = en2;
   for (Int_t globalBin=0; globalBin < fNcells ; globalBin++){
      coord = GetBins(globalBin);
      Double_t width = 1.0;
      for(Int_t a=0; a<Int_t(fAxes.size()); a++) {
         widths[a] = h1->GetAxis(a)->GetBinWidth(coord[a]);
         width*=widths.back();
      }
      if (normWidth) {
         cu  = c1*cu1[globalBin]/width;
         TH1F::SetBinContent(globalBin,cu);
         if (fSumw2.fN) {
            Double_t e1 = h1->GetBinError(globalBin)/width;
            e1 = e1;
            fSumw2.fArray[globalBin] = TMath::Abs(c1)*er1[globalBin];
         }
         fBinEntries.fArray[globalBin] = TMath::Abs(c1)*en2[globalBin]/width;
         if (fBinSumw2.fN) fBinSumw2.fArray[globalBin]  = TMath::Power(TMath::Abs(c1),2)*ew1[globalBin]/width;
      } else {
         cu  = c1*cu1[globalBin] + c2*cu2[globalBin];
         //if(cu!=0)
         //   cout << "Setting content of bin " << globalBin << " to " << cu << endl;
         fArray[globalBin] = cu;
         //TH1F::SetBinContent(globalBin,cu);
         if (fSumw2.fN) {
            Double_t e1 = h1->GetBinError(globalBin);
            Double_t e2 = h2->GetBinError(globalBin);
            e1 = e1; e2 = e2;
            fSumw2.fArray[globalBin] = TMath::Abs(c1)*er1[globalBin] + TMath::Abs(c2)*er2[globalBin];
         }
         fBinEntries.fArray[globalBin] = TMath::Abs(c1)*en1[globalBin] + TMath::Abs(c2)*en2[globalBin];
         if (fBinSumw2.fN) fBinSumw2.fArray[globalBin]  = TMath::Power(TMath::Abs(c1),2)*ew1[globalBin] + TMath::Power(TMath::Abs(c2),2)*ew2[globalBin];
      }
   }

   // update statistics (do here to avoid changes by SetBinContent)
   PutStats(s3);
   SetEntries(nEntries);

}

// --------------------------------------------------------------
void TProfileMDF::AddAxis(TString axisTitle, Int_t nbins, Double_t xlow, Double_t xup){
  TAxis * faxis = new TAxis(nbins, xlow, xup);

  stringstream ss;
  ss << "Axis_" << fAxes.size();
  faxis->SetNameTitle(ss.str().c_str(),axisTitle);
  fAxes.push_back(faxis);

  // Set the total number of cells.
  if (nbins <= 0) {
    cout<<"ERROR TProfileMDF::AddDimension called with"
      " nbins <= 0. Not adding axis !!"<<endl;
    return ;
  }

  if (fNcells == 0) 
    fNcells  = (nbins+2);
  else
    fNcells  *= (nbins+2); 

  if (fNcells <= 0) {
     cout<<"ERROR TProfileMDF::Number of bins greater than limit of 2.147 billion."<<endl
         <<"Ending program !!"<<endl;
     assert(fNcells > 0);
  }

  // Skip this as it would not allow me to set the bins
  // later on
  // fDimension = fAxes.size();
  
  // set the number cells 
  fBinSumw2.Set(fNcells);
  TArrayF::Set(fNcells);
  fBinEntries.Set(fNcells); 

  // Set the underlying histo to 
  cout<<" fNcells="<<fNcells<<endl;
  SetBins(fNcells-2,0,fNcells);

}//AddAxis

// --------------------------------------------------------------
void TProfileMDF::AddAxis(TString axisTitle, Int_t nbins, const Double_t* xbins){
   TAxis * faxis = new TAxis(nbins, xbins);

   stringstream ss;
   ss << "Axis_" << fAxes.size();
   faxis->SetNameTitle(ss.str().c_str(),axisTitle);
   fAxes.push_back(faxis);

   // Set the total number of cells.
   if (nbins <= 0) {
      cout<<"ERROR TProfileMDF::AddDimension called with"
         " nbins <= 0. Not adding axis !!"<<endl;
      return ;
   }

   if (fNcells == 0) 
      fNcells  = (nbins+2);
   else
      fNcells  *= (nbins+2);

   if (fNcells <= 0) {
      cout<<"ERROR TProfileMDF::Number of bins greater than limit of 2.147 billion."<<endl
          <<"Ending program !!"<<endl;
      assert(fNcells > 0);
   }

   // set the number cells 
   fBinSumw2.Set(fNcells);
   TArrayF::Set(fNcells);
   fBinEntries.Set(fNcells); 

   // Set the underlying histo to 
   cout<<" fNcells="<<fNcells<<endl;
   SetBins(fNcells-2,0,fNcells);

}//AddAxis

// --------------------------------------------------------------
void TProfileMDF::AddAxis(TAxis* faxis){

   if (!faxis) {
      cout<<"ERROR TProfileMDF::The pointer to faxis does not exist."<<endl
          <<"Ending program !!"<<endl;
      assert(faxis);
   }

   fAxes.push_back(faxis);

   // Set the total number of cells.
   if (faxis->GetNbins() <= 0) {
      cout<<"ERROR TProfileMDF::AddDimension called with"
         " nbins <= 0. Not adding axis !!"<<endl;
      return ;
   }

   if (fNcells == 0) 
      fNcells  = (faxis->GetNbins()+2);
   else
      fNcells  *= (faxis->GetNbins()+2);

   if (fNcells <= 0) {
      cout<<"ERROR TProfileMDF::Number of bins greater than limit of 2.147 billion."<<endl
          <<"Ending program !!"<<endl;
      assert(fNcells > 0);
   }

   // set the number cells 
   fBinSumw2.Set(fNcells);
   TArrayF::Set(fNcells);
   fBinEntries.Set(fNcells); 

   // Set the underlying histo to 
   cout<<" fNcells="<<fNcells<<endl;
   SetBins(fNcells-2,0,fNcells);

}//AddAxis

// --------------------------------------------------------------
void TProfileMDF::AddAxes(vector<TAxis*> faxes){
   for (unsigned int i=0; i<faxes.size(); i++) {
      AddAxis(faxes[i]);
   }
}//AddAxes

// --------------------------------------------------------------
TH1F* TProfileMDF::Get1DProjection(Int_t axisNumber, std::vector<Int_t> binCoord) {
   TString Name = Form("projection_axis%d",axisNumber);
   TString Title = Form("1D Projection Along The %s Axis",GetAxis(axisNumber)->GetTitle());
   //TH1F* projection = new TH1F(NameTitle,NameTitle,GetAxis(axisNumber)->GetNbins(),GetAxis(axisNumber)->GetXmin(),GetAxis(axisNumber)->GetXmax());
   TH1F* projection = new TH1F(Name,Title,GetAxis(axisNumber)->GetNbins(),GetAxis(axisNumber)->GetXbins()->GetArray());
   projection->GetXaxis()->SetTitle(GetAxis(axisNumber)->GetTitle());
   projection->GetXaxis()->SetTitleOffset(1.4); 
   projection->GetYaxis()->SetTitle("a.u.");
   projection->GetYaxis()->SetTitleOffset(1.4); 

   for(int i=0; i<=GetAxis(axisNumber)->GetNbins()+1; i++) {
      binCoord[axisNumber] = i;
      projection->SetBinContent(i,GetBinContent(binCoord));
      projection->SetBinError(i,GetBinError(binCoord));
   }
   
   return projection;
}

// --------------------------------------------------------------
TAxis * TProfileMDF::GetAxis(Int_t axisNumber){
  
  if (axisNumber >= (Int_t) fAxes.size() ) {
    cout<<"WARNING TProfileMDF::GetAxis requested with axisNumber"
      " out of range."<<endl;
    return 0;
  }
  
  return fAxes[axisNumber];
  
} // GetAxis

// --------------------------------------------------------------
vector<Int_t> TProfileMDF::GetBins(Int_t binglobal) const {
   
   // return binx, biny, binz corresponding to the global bin number globalbin
   vector<Int_t> cellsPerAxis;
   for (unsigned int a=0; a<fAxes.size(); a++) {
      cellsPerAxis.push_back(fAxes[a]->GetNbins()+2);
   }
   
   vector<Int_t> bins;
   for (unsigned int a=0; a<fAxes.size(); a++) {
      Int_t bin = binglobal;
      for (unsigned int b=0; b<a; b++) {
         bin = (bin - bins[b])/cellsPerAxis[b];
      }
      
      bin = bin % cellsPerAxis[a];

      bins.push_back(bin);
   }

   return bins;

}//GetBin

// --------------------------------------------------------------
// Find the global bin given the bin position
Int_t TProfileMDF::GetGlobalBin(const vector<Int_t> & bins){

  if (bins.size() != fAxes.size()){
    cout<<"ERROR TProfileMDF::GetGlobalBin called but #binCoordinates != #Axes"<<endl;
    return -1;
  }

  // the returning value
  Int_t bin     = 0;
  // aux vars
  Int_t binAxis = 0;
  Int_t binProd = 1;
  //loop over coordinates
  for (unsigned int axis = 0 ; axis < bins.size() ; axis ++ ){
    Int_t nx = fAxes[axis]->GetNbins() + 2;
    binAxis = bins[axis];
    if (binAxis < 0)   binAxis = 0;
    if (binAxis >= nx) binAxis = nx-1;
    bin += binProd * binAxis;
    binProd *=  nx ; // do this after adding it to bin
  }

  return bin;

}//GetGlobalBin

// --------------------------------------------------------------
Int_t TProfileMDF::GetMaximumBin() {
   vector<Int_t> binCoord(GetNaxis(),0);
   return GetMaximumBin(binCoord);
}

// --------------------------------------------------------------
Int_t TProfileMDF::GetMaximumBin(std::vector<Int_t>& binCoord) {
   //   -*-*-*-*-*Return location of bin with maximum value in the range*-*
   //             ======================================================
   Int_t locm;
   vector<Int_t> low;
   vector<Int_t> high;
   for(int a=0; a<GetNaxis(); a++) {
      low.push_back(fAxes[a]->GetFirst());
      high.push_back(fAxes[a]->GetLast());
   }
   Double_t maximum = -FLT_MAX, value;
   locm = 0;
   for (Int_t b=0; b < fNcells ; b++){
      value = GetBinContent(b);
      if(value>maximum && !IsBinOverflow(b) && !IsBinUnderflow(b)) {
         maximum = value;
         locm = b;
         binCoord = GetBins(b);
      }
   }
   return locm;
}

// --------------------------------------------------------------
Int_t TProfileMDF::FindBin(const vector<Double_t> & xcoor){

  if (xcoor.size() != fAxes.size()){
    cout<<"ERROR TProfileMDF::FindBin called but #Coordinates != #Axes"<<endl;
    return -1;
  }

  /* Example from TH3D::Fill
    Int_t bin  =  binx 
    + (fXaxis.GetNbins()+2) * biny 
    + (fXaxis.GetNbins()+2) * (fYaxis.GetNbins()+2) *binz
  */

  // the returning value
  Int_t bin     = 0;
  // aux vars
  Int_t binAxis = 0;
  Int_t binProd = 1;
  //loop over coordinates
  for (unsigned int axis = 0 ; axis < xcoor.size() ; axis ++ ){
    Int_t nx = fAxes[axis]->GetNbins() + 2;
    binAxis = fAxes[axis]->FindBin(xcoor[axis]);
    //cout<<" \t ::FindBin binAxis["<<axis<<"]("<<xcoor[axis]<<")="<<binAxis<<" nx="<<nx<<" binProd="<<binProd<<endl;
    if (binAxis < 0)   binAxis = 0;
    if (binAxis >= nx) binAxis = nx-1;
    bin += binProd * binAxis;
    binProd *=  nx ; // do this after adding it to bin
  }

  return bin;

}//FindBin

// --------------------------------------------------------------
void TProfileMDF::Fill(const vector<Double_t> & xcoor, Double_t val, Double_t weight){

  // Find the bin
  Int_t bin = FindBin(xcoor);
  //cout<<" Filling global bin="<<bin<<" with "<<val*weight<<endl;

  //Add the content
  AddBinContent(bin, val*weight);
  fSumw2.fArray[bin] += val*val;
  fBinEntries.fArray[bin] += 1;
  if (fBinSumw2.fN)  fBinSumw2.fArray[bin] += 1;

}//fill

// --------------------------------------------------------------
Double_t TProfileMDF::GetBinContent(Int_t bin) const {

  if (bin < 0 || bin >= fNcells) return 0;
  if (fBinEntries.fArray[bin] == 0) return 0;
  if (!fArray) return 0;
  return fArray[bin]/fBinEntries.fArray[bin];

}//GetBinContent

// --------------------------------------------------------------
Double_t TProfileMDF::GetBinContent(const vector<Int_t> & binCoord){

  // Find the bin and return the content
  Int_t bin = GetGlobalBin(binCoord);
  return GetBinContent(bin);

}//GetBinContent

// --------------------------------------------------------------
// the following is extracted from TProfileHelper::GetBinEffectiveEntries
Double_t TProfileMDF::GetBinEffectiveEntries(Int_t bin) const {
  //if (fBuffer) BufferEmpty();

   if (bin < 0 || bin >= fNcells) return 0;
   double sumOfWeights = fBinEntries.fArray[bin];
   /* This breaks the const requirement so I take it out. 
   if ( fBinSumw2.fN == 0 || fBinSumw2.fN != fNcells) { 
      // this can happen  when reading an old file 
      fBinSumw2.Set(0);
      return sumOfWeights;
   }
   */
   double sumOfWeightsSquare = fBinSumw2.fArray[bin]; 
   return ( sumOfWeightsSquare > 0 ?  sumOfWeights * sumOfWeights /   sumOfWeightsSquare : 0 ); 

}//GetBinEffectiveEntries

// --------------------------------------------------------------
Double_t TProfileMDF::GetBinEffectiveEntries(const std::vector<Int_t> & binCoord){

   // Find the bin and return the effective entries
   Int_t bin = GetGlobalBin(binCoord);
   return GetBinEffectiveEntries(bin);

}//GetBinEffectiveEntries

// --------------------------------------------------------------
Double_t TProfileMDF::GetBinError(Int_t bin) const{

  // The following cryptic piece of code is of course 
  // extracted from TProfileHelper::GetBinError
  //if (fBuffer) BufferEmpty();

   if (bin < 0 || bin >= fNcells) return 0;
   Double_t cont = fArray[bin];
   Double_t sum  = fBinEntries.fArray[bin];
   Double_t err2 = fSumw2.fArray[bin];
   Double_t neff = GetBinEffectiveEntries(bin);
   //cout<<setprecision(9)<<endl<<" neff="<<neff<<" err2="<<err2<<" cont="<<cont<<" sum="<<sum<<endl;
   if (sum == 0) return 0;
   Double_t contsum = cont/sum;
   Double_t eprim2  = TMath::Abs(err2/sum - contsum*contsum);
   Double_t eprim   = TMath::Sqrt(eprim2);
   Double_t test = 1;
   if (err2 != 0 && neff < 5) test = eprim2*sum/err2;
   //cout<<setprecision(9)<<" contsum="<<contsum<<" eprim2="<<eprim2<<" eprim="<<eprim<<" test="<<test<<endl;

   if (fgApproximate && fNcells <=1000404 && (test < 1.e-4 || eprim2 < 1e-6)) { //3.04
      Double_t scont, ssum, serr2;
      scont = ssum = serr2 = 0;
      for (Int_t i=1;i<fNcells;i++) {
         if (fSumw2.fArray[i] <= 0) continue; //added in 3.10/02
         scont += fArray[i];
         ssum  += fBinEntries.fArray[i];
         serr2 += fSumw2.fArray[i];
      }
      Double_t scontsum = scont/ssum;
      Double_t seprim2  = TMath::Abs(serr2/ssum - scontsum*scontsum);
      eprim           = 2*TMath::Sqrt(seprim2);
      sum = ssum;
   }

   sum = TMath::Abs(sum);
   if (fErrorMode == kERRORMEAN) return eprim/TMath::Sqrt(neff);
   else if (fErrorMode == kERRORSPREAD) return eprim;
   else if (fErrorMode == kERRORSPREADI) {
      if (eprim != 0) return eprim/TMath::Sqrt(neff);
      return 1/TMath::Sqrt(12*neff);
   }

   else if (fErrorMode == kERRORSPREADG) {
      // it is supposed the values y are gaussian distributed y +/- dy
      return 1./TMath::Sqrt(sum);
   }
   else return eprim;
   
}// GetBinError

// --------------------------------------------------------------
Double_t TProfileMDF::GetBinError(const vector<Int_t> & binCoord){

  // Find the bin and return the content
  Int_t globalBin = GetGlobalBin(binCoord);
  return GetBinError(globalBin);
}// GetBinError

// --------------------------------------------------------------
Double_t TProfileMDF::Integral(TString option){

   option.ToLower();

   vector<pair<int,int> > ranges;
   for(int fDimension=0; fDimension<GetNaxis(); fDimension++) {
      ranges.push_back(make_pair(0,GetAxis(fDimension)->GetNbins()+1));
      if(option.CompareTo("debug")==0)
         cout << "Integral::fDimension = " << fDimension << "\tlow = " << 0 << "\thight = " << GetAxis(fDimension)->GetNbins()+1 << endl;
   }
   return Integral(ranges,option);

}// Integral

// --------------------------------------------------------------
Double_t TProfileMDF::Integral(vector<pair<Int_t,Int_t> > ranges, TString option){
   vector<Int_t> coord(GetNaxis(),0);
   Double_t integral = 0.0;

   option.ToLower();
   
   if(option.Contains("debug"))
      cout << "Integral::Before IntegralRecursive with integral = " << integral << "\tranges.size() = " << ranges.size() << endl;
   IntegralRecursive(integral, ranges, coord, GetNaxis(), option);
   if(option.Contains("debug"))
      cout << "Integral::After IntegralRecursive with integral = " << integral << endl;

   return integral;

}// Integral

// --------------------------------------------------------------
void TProfileMDF::IntegralRecursive(Double_t& integral, vector<pair<Int_t,Int_t> > ranges, vector<Int_t>& coord, Int_t depth, TString option){

   option.ToLower();
   
   if(option.Contains("debug")){
      for(int i=ranges.size()-depth; i>-1;i--)
         cout << "\t";
      cout << "IntegralRecursive::At depth = " << depth << endl;
   }

   if (depth==0) {
      Int_t bin = GetGlobalBin(coord);
      if(option.Contains("width")) {
         Double_t width = 0.0;
         for(unsigned int w=0; w< fAxes.size();w++) {
            width*=fAxes[w]->GetBinWidth(coord[w]);
         }
         integral += GetBinContent(bin)*fBinEntries.fArray[bin]*width;
      }
      else
         integral += GetBinContent(bin)*fBinEntries.fArray[bin];
   }		
   else {
      for (int fRange = ranges[depth-1].first; fRange<=ranges[depth-1].second; fRange++) {
         if(option.Contains("debug")){
            for(int i=ranges.size()-depth; i>-1;i--)
               cout << "\t";
            cout << "IntegralRecursive::At index = " << fRange << endl;
         }
         coord[depth-1] = fRange;
         IntegralRecursive(integral,ranges,coord,depth-1,option);
      }
   }

}

// --------------------------------------------------------------
Bool_t TProfileMDF::IsBinOverflow(Int_t bin) const {
   // Return true if the bin is overflow.
   vector<Int_t> bins = GetBins(bin);
   bool overflow = false;
   for(unsigned int fDimension=1; fDimension<=bins.size(); fDimension++) {
      if(bins[fDimension-1] >= fAxes[fDimension-1]->GetNbins()+1)
         overflow = true;
   }
   
   return overflow;
}// IsBinOverflow

// --------------------------------------------------------------
Bool_t TProfileMDF::IsBinUnderflow(Int_t bin) const {
   // Return true if the bin is overflow.
   vector<Int_t> bins = GetBins(bin);
   bool underflow = false;

   for(unsigned int fDimension=1; fDimension<=bins.size(); fDimension++) {
      if(bins[fDimension-1] <= 0)
         underflow = true;
   }
   
   return underflow;
}// IsBinUnderflow

// --------------------------------------------------------------
void TProfileMDF::LoopOverBins(){
   
  for (Int_t b=0; b < fNcells ; b++){
     if(GetBinEffectiveEntries(b)==0) 
        continue;
     cout<<"\tGlobal bin="<<b
         <<" content="<< GetBinContent(b)
         <<" +/- "<< GetBinError(b)
         <<" entries "<< GetBinEffectiveEntries(b)
         <<endl;
  }//for

}//LoopOverBins

// --------------------------------------------------------------
void TProfileMDF::LoopOverBinsRaw(){

   Double_t integral = 0.0;

   for (Int_t b=0; b < fNcells ; b++){
      if(GetBinEffectiveEntries(b)==0) 
         continue;
      cout<<"\tGlobal bin="<<b
          <<" content="<< fArray[b]
          <<" fBinEntries="<< fBinEntries.fArray[b]
          <<" fBinSumw2="<< fBinSumw2.fArray[b]
          <<" fSumw2="<< fSumw2.fArray[b]
         <<endl;
      integral += fBinEntries.fArray[b]*GetBinContent(b);
   }//for
   cout<<"\tfNcells="<<fNcells<<endl
       <<"\tfErrorMode="<<fErrorMode<<endl
       <<"\tfgApproximate="<<fgApproximate<<endl;
   for (unsigned int a=0; a<fAxes.size(); a++){
      cout<<"\tAxis"<<a<<" \tname="<<fAxes[a]->GetName()
          <<" \ttitle="<<fAxes[a]->GetTitle()<<endl;
   }
   cout<<"\tIntegral="<<integral<<endl;

}//LoopOverBinsRaw

// --------------------------------------------------------------
void TProfileMDF::LoopOverBinsCoordinates(){

  for (Int_t b=0; b < fNcells ; b++){
     cout<<"\tGlobal bin="<<b;
     vector<Int_t> bins = GetBins(b);
     for(unsigned int fDimension=1; fDimension<=bins.size(); fDimension++) {
        cout<<" Axis" << fDimension << "Bin=" << bins[fDimension-1];
     }
     cout<<" IsUnderflow="<< IsBinUnderflow(b)
         <<" IsOverflow="<< IsBinOverflow(b)
         <<endl;
  }//for

}

// --------------------------------------------------------------
void TProfileMDF::ReadFromFile(TString filename, TString treeName){

   TFile *f = new TFile(filename);
   if (!f->IsOpen()) return;

   gDirectory->cd(treeName);
   if(TString(gDirectory->GetName()).CompareTo(treeName)!=0) return;

   TTree* t = (TTree*)gDirectory->Get(treeName);
   if (!t) return;

   Float_t content_ = 0.0;
   Float_t fBinEntries_ = 0.0;
   Float_t fBinSumw2_ = 0.0;
   Double_t fSumw2_ = 0.0;
   Double_t fEntries_ = 0.0;
   vector<TAxis*> axes;

   t->SetBranchAddress("content",     &content_);
   t->SetBranchAddress("fBinEntries", &fBinEntries_);
   t->SetBranchAddress("fBinSumw2",   &fBinSumw2_);
   t->SetBranchAddress("fSumw2",      &fSumw2_);

   TList* list = (TList*)gDirectory->GetListOfKeys();
   TIter next(list);
   while (TObject* obj = next())
   {
      if (TString(((TKey*)obj)->GetClassName()).CompareTo("TAxis")==0){
         TAxis* tempAxis = (TAxis*)gDirectory->Get(obj->GetName());
         if (tempAxis){
            axes.push_back(tempAxis);
         }
      }
   }
   //Sort axes
   sort(axes.begin(),axes.end(),axisComparison);
   //Add axes
   AddAxes(axes);

   Sumw2();
   
   int nent = t->GetEntries();
   if (fNcells!=nent) {
      cout << "ERROR TProfileMDF::Number of entries in input tree is not consistent with the number of cells specified by the TAxes." << endl
           << "Program will now exit!!" << endl;
      assert(fNcells!=nent);
   }

   for (int bin=0; bin<nent; bin++){
      t->GetEntry(bin);
      if (fBinEntries_<1) {
         //TH1F::SetBinContent(bin,0.0);
         fBinEntries.fArray[bin] = 0.0;      
         fBinSumw2.fArray[bin] = 0.0;
         fSumw2.fArray[bin] = 0.0;
      }
      else {
         TH1F::SetBinContent(bin,content_);
         fBinEntries.fArray[bin] = fBinEntries_;      
         fBinSumw2.fArray[bin] = fBinSumw2_;
         fSumw2.fArray[bin] = fSumw2_;
         fEntries_+=fBinEntries_;
      }
   }
   fEntries = fEntries_;

}

// --------------------------------------------------------------
TProfileMDF* TProfileMDF::ReduceDimensions(TString name, UInt_t axisNumber, Int_t firstbin = 0, Int_t lastbin = -1){

   Int_t inNbin;
   Int_t firstOutBin, lastOutBin;
   vector<TAxis*> fAxesTemp = fAxes;
   vector<TAxis*> outAxes;
   TAxis* inAxis;

   inNbin = fAxesTemp[axisNumber]->GetNbins();
   inAxis = fAxesTemp[axisNumber];
   fAxesTemp.erase(fAxesTemp.begin()+axisNumber);
   outAxes = fAxesTemp;

   if ( lastbin < firstbin && inAxis->TestBit(TAxis::kAxisRange) ) {
      firstbin = inAxis->GetFirst();
      lastbin = inAxis->GetLast();
      // For special case of TAxis::SetRange, when first == 1 and last
      // = N and the range bit has been set, the TAxis will return 0
      // for both.
      if (firstbin == 0 && lastbin == 0)
      {
         firstbin = 1;
         lastbin = inAxis->GetNbins();
      }
   }
   if (firstbin < 0) firstbin = 0;
   if (lastbin  < 0) lastbin  = inNbin + 1;
   if (lastbin  > inNbin+1) lastbin  = inNbin + 1;

   // Create the projection histogram
   if(name.IsNull()) {
      stringstream ss;
      ss << axisNumber;
      name = TString(GetName() + TString(ss.str()));
   }

   //check if histogram with identical name exist
   // if compatible reset and re-use previous histogram
   // (see https://savannah.cern.ch/bugs/?54340)
   TObject *h1obj = gROOT->FindObject(name);
   if (h1obj && h1obj->InheritsFrom(TH1::Class())) {
      Error("ReduceDimensions","Histogram with name %s already exists",string(name).c_str());
      return 0;
   }
   
   TProfileMDF *h1 = new TProfileMDF(name,GetTitle());
   h1->AddAxes(outAxes);
   h1->Sumw2();
   
   h1->SetLineColor(this->GetLineColor());
   h1->SetFillColor(this->GetFillColor());
   h1->SetMarkerColor(this->GetMarkerColor());
   h1->SetMarkerStyle(this->GetMarkerStyle());

   // Fill the projected histogram
   Double_t cont,sw2,bent,bsw2;
   Double_t totcont = 0;
   Bool_t  computeErrors = h1->GetSumw2N();
   computeErrors = computeErrors;
   vector<Int_t> inbins;
   inbins.assign(fAxes.size(),0);
   vector<Int_t> outbins;
   outbins.assign(h1->GetNaxis(),0);

   // implement filling of projected histogram
   // outbin is bin number of outAxis (the projected axis). Loop is done on all bin of TH2 histograms
   // inbin is the axis being integrated. Loop is done only on the selected bins
   firstOutBin = 0;
   lastOutBin = h1->GetNbins();
   firstOutBin = firstOutBin; lastOutBin = lastOutBin;

   for (Int_t outbin = 0; outbin < h1->GetNbins(); ++outbin) {
      cont = 0;
      sw2  = 0;
      bent = 0;
      bsw2 = 0;
      outbins = h1->GetBins(outbin);
      
      /*
      cout << "outbin=" << outbin << "  outbins (";
      for(unsigned int i=0; i<outbins.size(); i++){
         cout << outbins[i];
         if(i!=outbins.size()-1)
            cout <<",";
         else
            cout << ")" << endl;
      }
      */

      for (Int_t inbin = firstbin ; inbin <= lastbin ; ++inbin) {
         // sum bin content and error if needed
         inbins.clear();
         inbins = outbins;
         for (UInt_t b=0; b<outbins.size(); b++) {
            if (b == axisNumber)
               inbins.insert(inbins.begin()+b,inbin);
            }

         /*
         cout << "Getting content from bin (";
         for(unsigned int i=0; i<inbins.size(); i++){
            cout << inbins[i];
            if(i!=inbins.size()-1)
               cout <<",";
            else
               cout << ")" << endl;
         }
         */
       
         cont += GetBinContent(inbins);
         sw2  += fSumw2.fArray[GetGlobalBin(inbins)];
         bent += fBinEntries.fArray[GetGlobalBin(inbins)];
         bsw2 += (fBinSumw2.fN ? fBinSumw2.fArray[GetGlobalBin(inbins)] : 0);
      }//for inbin

      // find corresponding bin number in h1 for outbin and set the bin content and error
      h1->SetBinContentError(h1->GetGlobalBin(outbins),cont,sw2,bent,bsw2);

      // For debugging purposes
      //if(cont!=0) cout << "cont=" << cont << " at global bin " << h1->GetGlobalBin(outbins) << endl;
      //if(cont!=0) cout << "h1->GetBinContent(6)=" << h1->GetBinContent(6) << " at global bin " << h1->GetGlobalBin(outbins) << endl;

      // sum  all content
      totcont += cont;
   }//for outbin

   // For debugging purposes
   //h1->LoopOverBins();

   // the statistics is automatically recalulated since it is reset by the call to SetBinContent
   // we just need to set the entries since they have not been correctly calculated during the projection
   // we can only set them to the effective entries
   h1->SetEntries( h1->GetEffectiveEntries() );

   // re-compute the entries
   // in case of error calculation (i.e. when Sumw2() is set)
   // use the effective entries for the entries
   // since this  is the only way to estimate them
   Double_t entries =  TMath::Floor( totcont + 0.5); // to avoid numerical rounding
   if (h1->GetSumw2N()) entries = h1->GetEffectiveEntries();
   h1->SetEntries( entries );

   return h1;

}//ReduceDimensions

// --------------------------------------------------------------
void TProfileMDF::Scale(Double_t c1, Option_t* option){
   //   -*-*-*Multiply this histogram by a constant c1*-*-*-*-*-*-*-*-*
   //         ========================================
   //
   //   this = c1*this
   //
   // Note that both contents and errors(if any) are scaled.
   // This function uses the services of TH1::Add
   //
   // IMPORTANT NOTE: If you intend to use the errors of this histogram later
   // you should call Sumw2 before making this operation.
   // This is particularly important if you fit the histogram after TH1::Scale
   //
   // One can scale an histogram such that the bins integral is equal to
   // the normalization parameter via TH1::Scale(Double_t norm), where norm
   // is the desired normalization divided by the integral of the histogram.
   //
   // If option contains "width" the bin contents and errors are divided
   // by the bin width.
/*
//For TH1

   TString opt = option;
   opt.ToLower();
   Double_t ent = 0.0;
   for (Int_t bin=0; bin < fNcells ; bin++){
      ent += fBinEntries.fArray[bin];
   }
   if (opt.Contains("width")) Add(this,this,c1,-1);
   else                       Add(this,this,c1,0);
   fEntries = ent;
*/
//For TProfile
   Double_t ac1 = TMath::Abs(c1);

   // Make the loop over the bins to calculate the Addition
   Int_t bin;
   Float_t *cu1 = GetW();
   Double_t *er1 = GetW2();
   Float_t *en1 = GetB();
   Float_t *ew1 = GetB2();
   for (bin=0;bin<fNcells;bin++) {
      fArray[bin]             = c1*cu1[bin];
      fSumw2.fArray[bin]      = ac1*ac1*er1[bin];
      fBinEntries.fArray[bin] = en1[bin];
      fBinSumw2.fArray[bin]   = ac1*ac1*ew1[bin];
   }

}//Scale

// --------------------------------------------------------------
void TProfileMDF::SetBinContentError(Int_t bin, Double_t cont, Double_t sw2, Double_t bent, Double_t bsw2){

   if (bin < 0 || bin > fNcells-1) return;
   // delete buffer if it is there since it will become invalid
   if (fBuffer) BufferEmpty(1);
   // create sumw2 per bin if not set
   if (fBinSumw2.fN == 0) Sumw2();
   
   fEntries+=bent;
   fTsumw = 0;
   fArray[bin] = cont;
   fSumw2.fArray[bin] = sw2;
   fBinEntries.fArray[bin] = bent;
   if (fBinSumw2.fN)
      fBinSumw2.fArray[bin] = bsw2; 
   //cout<<"bin="<<bin<<"\t fNcells="<<fNcells<<"\tcontent="<<cont<<"\tfArray[bin]="<<fArray[bin]<<"\tfBinEntries.fArray[bin]="<<fBinEntries.fArray[bin]<<"\tfBinSumw2.fArray[bin]="<<fBinSumw2.fArray[bin]<<endl;

}

// --------------------------------------------------------------
void TProfileMDF::SetErrorOption(Option_t * option){

//*-*-*-*-*-*-*-*-*-*Set option to compute profile2D errors*-*-*-*-*-*-*-*
//*-*                =======================================
//
//    The computation of errors is based on the parameter option:
//    option:
//     ' '  (Default) Errors are Spread/SQRT(N) for Spread.ne.0. ,
//                      "     "  SQRT(T)/SQRT(N) for Spread.eq.0,N.gt.0 ,
//                      "     "  0.  for N.eq.0
//     's'            Errors are Spread  for Spread.ne.0. ,
//                      "     "  SQRT(T)  for Spread.eq.0,N.gt.0 ,
//                      "     "  0.  for N.eq.0
//     'i'            Errors are Spread/SQRT(N) for Spread.ne.0. ,
//                      "     "  1./SQRT(12.*N) for Spread.eq.0,N.gt.0 ,
//                      "     "  0.  for N.eq.0
//   See TProfile3D::BuildOptions for explanation of all options

   TString opt = option;
   opt.ToLower();
   fErrorMode = kERRORMEAN;
   if (opt.Contains("s")) fErrorMode = kERRORSPREAD;
   if (opt.Contains("i")) fErrorMode = kERRORSPREADI;
   if (opt.Contains("g")) fErrorMode = kERRORSPREADG;

}//SetErrorOption

// --------------------------------------------------------------
void TProfileMDF::Test(){

  AddAxis("x",30,0,130);
  AddAxis("y",20,0,3500);
  AddAxis("z",10,0,10);
  
  Sumw2();

  // Vector of coordinates initialized to 
  // have h->GetNaxis() elements
  // coord[0]=x, coord[1]=y, and so on...
  vector<Double_t> coord (GetNaxis(),0);

  // Throw some random values
  TRandom3 rn;

  for (int pse = 0; pse < 1000; pse ++) {

     coord[0] = rn.Gaus(60,10);
     coord[1] = rn.Gaus(1000,100);
     coord[2] = rn.Gaus(5,2);

     Fill(coord,rn.Gaus(13, 0.2));

  }//for pse's
  
  // Bin -> Vector of bins
  vector<Int_t> bin (GetNaxis(),0);
  bin[0] = 2;
  bin[1] = 2;
  bin[2] = 2;
  
  cout<<"TEST()"<<endl;
  cout<<" \tfNcells="<<fNcells<<endl;
  cout<<" \tbin[0]="<<bin[0]<<" bin[1]="<<bin[1]<<" bin[2]="<<bin[2]<<endl;
  cout<<" \tGlobal bin        ="<< GetGlobalBin(bin)<<endl;
  cout<<" \tBin Content       ="<< GetBinContent(bin)<<endl;
  cout<<" \tBin Error         ="<< GetBinError(bin)<<endl;
  cout<<" \tEntries           ="<< GetEntries()<<endl;
  cout<<" \tEffective Entries ="<< GetEffectiveEntries()<<endl;

}//Test

// --------------------------------------------------------------
void TProfileMDF::TestRD(){
   Test3D();

   cout<<"LoopOverBins1"<< endl;
   LoopOverBins();
   TProfileMDF* p = ReduceDimensions("p",0,0,5);
   cout<<"ReducedDimensionsBy1"<< endl;
   cout<<"LoopOverBins2"<< endl;
   p->LoopOverBins();
   //cout<<"p->GetBinContent(6)="<<p->GetBinContent(6)<<endl;

}//TestRD

// --------------------------------------------------------------
void TProfileMDF::Test2D(){

   AddAxis("x",3,0,3);
   AddAxis("y",3,0,3);

   Sumw2();

   vector<Double_t> c (GetNaxis(),0);

   c[0]=0.5;
   c[1]=0.5;
   Fill(c,1);
   c[0]=1.5;
   c[1]=0.5;
   Fill(c,1);
   c[0]=2.5;
   c[1]=0.5;
   Fill(c,4);
/*
   vector<pair<Int_t,Int_t> > ranges;
   ranges.push_back(make_pair(0,1));
   ranges.push_back(make_pair(0,3));
   cout << "Integral=" << Integral(ranges) << endl;
*/
}//Test2D

// --------------------------------------------------------------
void TProfileMDF::Test3D(){

   AddAxis("x",3,0,3);
   AddAxis("y",3,0,3);
   AddAxis("z",3,0,3);

   Sumw2();

   vector<Double_t> c (GetNaxis(),0);

   c[0]=0.5;
   c[1]=0.5;
   c[2]=0.5;
   Fill(c,1);
   c[0]=1.5;
   c[1]=0.5;
   c[2]=0.5;
   Fill(c,1);
   c[0]=2.5;
   c[1]=0.5;
   c[2]=0.5;
   Fill(c,4);
   c[0]=2.5;
   c[1]=0.5;
   c[2]=0.5;
   Fill(c,4);
/*
   vector<pair<Int_t,Int_t> > ranges;
   ranges.push_back(make_pair(3,3));
   ranges.push_back(make_pair(0,1));
   ranges.push_back(make_pair(0,1));
   cout << "Integral=" << Integral(ranges) << endl;
*/
}//Test3D

// --------------------------------------------------------------
void TProfileMDF::WriteToFile(TString filename, TString writeFlag){

   TFile* f = new TFile(filename,writeFlag);
   gDirectory->mkdir(this->GetName());
   gDirectory->cd(this->GetName());
   TTree* t = new TTree(this->GetName(),this->GetTitle());

   Float_t content_;
   Float_t fBinEntries_;
   Float_t fBinSumw2_;
   Double_t fSumw2_;

   t->Branch("content",     &content_,     "content/F");
   t->Branch("fBinEntries", &fBinEntries_, "fBinEntries/F");
   t->Branch("fBinSumw2",   &fBinSumw2_,   "fBinSumw2/F");
   t->Branch("fSumw2",      &fSumw2_,      "fSumw2/D");

   for (int bin=0; bin<fNcells; bin++){
      content_ = 0.0;
      fBinEntries_ = 0.0;
      fBinSumw2_ = 0.0;
      fSumw2_ = 0.0;
      
      content_ = TH1F::GetBinContent(bin);
      fBinEntries_ = fBinEntries.fArray[bin];
      fBinSumw2_ = fBinSumw2.fArray[bin];
      fSumw2_ = fSumw2.fArray[bin];

      t->Fill();
   }
   t->Write();

   for(unsigned int a=0; a<fAxes.size(); a++){
      fAxes[a]->Write();
   }

   f->Close();
   delete f;

}

#include "JetMETAnalysis/JetUtilities/interface/TProfileMDF.h"

#include "TROOT.h"
#include "TMath.h"
#include "Riostream.h"
#include "TRandom3.h"
#include "TFile.h"
#include "TKey.h"
#include "TTree.h"

#include <iostream>
#include <assert.h>
#include <sstream>
#include <algorithm>
#include <iomanip>


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
TAxis * TProfileMDF::GetAxis(Int_t axisNumber){
  
  if (axisNumber >= (Int_t) fAxes.size() ) {
    cout<<"WARNING TProfileMDF::GetAxis requested with axisNumber"
      " out of range."<<endl;
    return 0;
  }
  
  return fAxes[axisNumber];
  
} // GetAxis

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
vector<Int_t> TProfileMDF::GetBins(Int_t binglobal){
   
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

}//FindBin


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

} // GetBinEffectiveEntries


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
}

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
  cout<<" \tGlobal bin  ="<< GetGlobalBin(bin)<<endl;
  cout<<" \tBin Content ="<< GetBinContent(bin)<<endl;
  cout<<" \tBin Error   ="<< GetBinError(bin)<<endl;

}//Test

// --------------------------------------------------------------
void TProfileMDF::TestRD(){

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
   Fill(c,1);

   LoopOverBins();
   cout<<"LoopOverBins1"<< endl;
   TProfileMDF* p = ReduceDimensions("p",0,0,5);
   cout<<"ReducedDimensionsBy1"<< endl;
   p->LoopOverBins();
   cout<<"LoopOverBins2"<< endl;

}//TestRD

// --------------------------------------------------------------
void TProfileMDF::LoopOverBins(){

  for (Int_t b=0; b < fNcells ; b++){
     if(GetBinEffectiveEntries(b)==0) 
        continue;
     cout<<"\tGlobal bin ="<<b
         <<" content="<< GetBinContent(b)
         <<" +/- "<< GetBinError(b)
         <<" entries "<< GetBinEffectiveEntries(b)
         <<endl;
  }//for

}//LoopOverBins

// --------------------------------------------------------------
void TProfileMDF::LoopOverBinsRaw(){

   for (Int_t b=0; b < fNcells ; b++){
      if(GetBinEffectiveEntries(b)==0) 
         continue;
      cout<<"\tGlobal bin ="<<b
          <<" content="<< fArray[b]
          <<" fBinEntries="<< fBinEntries.fArray[b]
          <<" fBinSumw2="<< fBinSumw2.fArray[b]
          <<" fSumw2="<< fSumw2.fArray[b]
         <<endl;
   }//for
   cout<<"\tfNcells="<<fNcells<<endl
       <<"\tfErrorMode="<<fErrorMode<<endl
       <<"\tfgApproximate="<<fgApproximate<<endl;
   for (unsigned int a=0; a<fAxes.size(); a++){
      cout<<"\tAxis"<<a<<" name="<<fAxes[a]->GetName()
          <<"Axis"<<a<<" title="<<fAxes[a]->GetTitle()<<endl;
   }

}//LoopOverBinsRaw

// --------------------------------------------------------------
void TProfileMDF::ReadFromFile(TString filename, TString treeName){

   TFile *f = new TFile(filename);
   if (!f->IsOpen()) return;
 
   TTree* t = (TTree*)gDirectory->Get(treeName);
   if (!t) return;

   Float_t content_ = 0.0;
   Float_t fBinEntries_ = 0.0;
   Float_t fBinSumw2_ = 0.0;
   Double_t fSumw2_ = 0.0;
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
         TH1F::SetBinContent(bin,0.0);
         fBinEntries.fArray[bin] = 0.0;      
         fBinSumw2.fArray[bin] = 0.0;
         fSumw2.fArray[bin] = 0.0;
      }
      else {
         TH1F::SetBinContent(bin,content_);
         fBinEntries.fArray[bin] = fBinEntries_;      
         fBinSumw2.fArray[bin] = fBinSumw2_;
         fSumw2.fArray[bin] = fSumw2_;
      }
   }

}

// --------------------------------------------------------------
TProfileMDF* TProfileMDF::ReduceDimensions(TString name, UInt_t axisNumber, Int_t firstbin = 0, Int_t lastbin = -1){

   Int_t inNbin;
   Int_t firstOutBin, lastOutBin;
   vector<TAxis*> fAxesTemp = fAxes;
   vector<TAxis*> outAxes;
   TAxis* inAxis;
   cout << "sfsg0" << endl;
   inNbin = fAxesTemp[axisNumber]->GetNbins();
   inAxis = fAxesTemp[axisNumber];
   fAxesTemp.erase(fAxesTemp.begin()+axisNumber);
   outAxes = fAxesTemp;
   cout << "sfsg1" << endl;
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
   cout << "sfsg2" << endl;
   // Create the projection histogram
   if(name.IsNull()) {
      stringstream ss;
      ss << axisNumber;
      name = TString(GetName() + TString(ss.str()));
   }
   cout << "sfsg3" << endl;
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
   cout << "sfsg4" << endl;
   // Fill the projected histogram
   Double_t cont,err2;
   Double_t totcont = 0;
   Bool_t  computeErrors = h1->GetSumw2N();
   vector<Int_t> inbins;
   inbins.assign(fAxes.size(),0);
   vector<Int_t> outbins;
   outbins.assign(h1->GetNaxis(),0);
   cout << "sfsg5" << endl;
   // implement filling of projected histogram
   // outbin is bin number of outAxis (the projected axis). Loop is done on all bin of TH2 histograms
   // inbin is the axis being integrated. Loop is done only on the selected bins
   firstOutBin = 0;
   lastOutBin = h1->GetNbins();
   cout << "sfsg6" << endl;
   for (Int_t outbin = 0; outbin < h1->GetNbins(); ++outbin) {
      err2 = 0;
      cont = 0;
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
         Double_t err = GetBinError(inbins);
         err2 += err*err;
      }//for inbin

      // find corresponding bin number in h1 for outbin
      h1->TH1F::SetBinContent(h1->GetGlobalBin(outbins),cont);
      if(cont!=0) cout << "cont=" << cont << " at global bin " << h1->GetGlobalBin(outbins) << endl;
      h1->TH1F::SetBinError(h1->GetGlobalBin(outbins),TMath::Sqrt(err2));
      // sum  all content
      totcont += cont;
   }//for outbin
   cout << "sfsg7" << endl;
   h1->LoopOverBins();
   // the statistics is automatically recalulated since it is reset by the call to SetBinContent
   // we just need to set the entries since they have not been correctly calculated during the projection
   // we can only set them to the effective entries
   h1->SetEntries( h1->GetEffectiveEntries() );
   cout << "sfsg8" << endl;
   // re-compute the entries
   // in case of error calculation (i.e. when Sumw2() is set)
   // use the effective entries for the entries
   // since this  is the only way to estimate them
   Double_t entries =  TMath::Floor( totcont + 0.5); // to avoid numerical rounding
   if (h1->GetSumw2N()) entries = h1->GetEffectiveEntries();
   h1->SetEntries( entries );
   cout << "sfsg9" << endl;
   return h1;

}//ReduceDimensions

// --------------------------------------------------------------
void TProfileMDF::WriteToFile(TString filename, TString writeFlag){

   TFile* f = new TFile(filename,writeFlag);
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

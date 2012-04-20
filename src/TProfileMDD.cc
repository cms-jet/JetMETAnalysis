#include "JetMETAnalysis/JetUtilities/interface/TProfileMDD.h"

#include "TMath.h"
#include "Riostream.h"
#include "TRandom3.h"
#include "TFile.h"

#include <iostream>
#include <assert.h>
#include <sstream>

ClassImp(TProfileMDD)

using std::cout;
using std::endl;
using std::vector;
using std::stringstream;

Bool_t TProfileMDD::fgApproximate = kFALSE;


/* TO TEST

root -l TProfileMDD.c+ // Ignore the warnings during compilation
TFile * f = new TFile("a.root","RECREATE")
TProfileMDD j("H","k")
j.Test()
j->Write()
f->Close()
.q

root -l TProfileMDD.c+
TFile *_file0 = TFile::Open("a.root")

*/

/* TO USE

root -l TProfileMDD.cc+ // Ignore the warnings during compilation

TProfileMDD * h = new TProfileMDD("H","K")
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
TProfileMDD::TProfileMDD() : TH1D() { }

// --------------------------------------------------------------
TProfileMDD::~TProfileMDD(){ 

  /*
  for (unsigned int axis = 0 ; axis < fAxes.size() ; axis ++ )
  delete fAxes[axis];

  fAxes.clear();
  */
}

// --------------------------------------------------------------
TProfileMDD::TProfileMDD(const char*name, const char *title) : TH1D() {
  // TH1D(name,title,100,0,1){
  
  // Set the number of cells to zero
  fNcells = 0;

  fErrorMode = kERRORMEAN;
  TH1D::SetName(name);
  TH1D::SetTitle(title);

}

// --------------------------------------------------------------
TAxis * TProfileMDD::GetAxis(Int_t axisNumber){
  
  if (axisNumber >= (Int_t) fAxes.size() ) {
    cout<<"WARNING TProfileMDD::GetAxis requested with axisNumber"
      " out of range."<<endl;
    return 0;
  }
  
  return fAxes[axisNumber];
  
} // GetAxis

// --------------------------------------------------------------
void TProfileMDD::AddAxis(TString axisTitle, Int_t nbins, Double_t xlow, Double_t xup){
  TAxis * faxis = new TAxis(nbins, xlow, xup);
  //TAxis faxis(nbins, xlow, xup);

  stringstream ss;
  ss << fAxes.size();
  faxis->SetName(ss.str().c_str());
  faxis->SetTitle(axisTitle);
  fAxes.push_back(faxis);

  // Set the total number of cells.
  if (nbins <= 0) {
    cout<<"ERROR TProfileMDD::AddDimension called with"
      " nbins <= 0. Not adding axis !!"<<endl;
    return ;
  }
 
  if (fNcells == 0) 
    fNcells  = (nbins+2);
  else
    fNcells  *= (nbins+2); 
  
  if (fNcells <= 0) {
     cout<<"ERROR TProfileMDD::Number of bins greater than limit of 2.147 billion."<<endl
         <<"Ending program !!"<<endl;
     assert(fNcells > 0);
  }
  
  // Create the sumW'2
  //sumws.push_back((Double_t) 0);
  //sumws2.push_back((Double_t) 0);

  // Skip this as it would not allow me to set the bins
  // later on
  // fDimension = fAxes.size();
  
  // set the number cells 
  fBinSumw2.Set(fNcells);
  TArrayD::Set(fNcells);
  fBinEntries.Set(fNcells); 

  // Set the underlying histo to 
  cout<<" fNcells="<<fNcells<<endl;
  SetBins(fNcells,0,fNcells);

}//AddAxis

// --------------------------------------------------------------
void TProfileMDD::AddAxis(TString axisTitle, Int_t nbins, const Double_t* xbins){
   TAxis * faxis = new TAxis(nbins, xbins);

   stringstream ss;
   ss << fAxes.size();
   faxis->SetName(ss.str().c_str());
   faxis->SetTitle(axisTitle);
   fAxes.push_back(faxis);

   // Set the total number of cells.
   if (nbins <= 0) {
      cout<<"ERROR TProfileMDD::AddDimension called with"
         " nbins <= 0. Not adding axis !!"<<endl;
      return ;
   }

   if (fNcells == 0) 
      fNcells  = (nbins+2);
   else
      fNcells  *= (nbins+2);

   // set the number cells 
   fBinSumw2.Set(fNcells);
   TArrayD::Set(fNcells);
   fBinEntries.Set(fNcells); 

   // Set the underlying histo to 
   cout<<" fNcells="<<fNcells<<endl;
   SetBins(fNcells,0,fNcells);

}//AddAxis

// --------------------------------------------------------------
// Find the global bin given the bin position
Int_t TProfileMDD::GetGlobalBin(const vector<Int_t> & bins){

  if (bins.size() != fAxes.size()){
    cout<<"ERROR TProfileMDD::GetGlobalBin called but #binCoordinates != #Axes"<<endl;
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
Int_t TProfileMDD::FindBin(const vector<Double_t> & xcoor){

  if (xcoor.size() != fAxes.size()){
    cout<<"ERROR TProfileMDD::FindBin called but #Coordinates != #Axes"<<endl;
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
    //    cout<<" \t ::FindBin binAxis["<<axis<<"]("<<xcoor[axis]<<")="<<binAxis<<" nx="<<nx<<" binProd="<<binProd<<endl;
    if (binAxis < 0)   binAxis = 0;
    if (binAxis >= nx) binAxis = nx-1;
    bin += binProd * binAxis;
    binProd *=  nx ; // do this after adding it to bin
  }

  return bin;

}//FindBin

// --------------------------------------------------------------
void TProfileMDD::Fill(const vector<Double_t> & xcoor, Double_t val, Double_t weight){

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
Double_t TProfileMDD::GetBinContent(Int_t bin) const {

  if (bin < 0 || bin >= fNcells) return 0;
  if (fBinEntries.fArray[bin] == 0) return 0;
  if (!fArray) return 0;
  return fArray[bin]/fBinEntries.fArray[bin];

}//GetBinContent

// --------------------------------------------------------------
Double_t TProfileMDD::GetBinContent(const vector<Int_t> & binCoord){

  // Find the bin and return the content
  Int_t bin = GetGlobalBin(binCoord);
  return GetBinContent(bin);

}//GetBinContent

// --------------------------------------------------------------
// the following is extracted from TProfileHelper::GetBinEffectiveEntries
Double_t TProfileMDD::GetBinEffectiveEntries(Int_t bin) const {
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
Double_t TProfileMDD::GetBinError(Int_t bin) const{

  // The following cryptic piece of code is of course 
  // extracted from TProfileHelper::GetBinError
  //if (fBuffer) BufferEmpty();

   if (bin < 0 || bin >= fNcells) return 0;
   Double_t cont = fArray[bin];
   Double_t sum  = fBinEntries.fArray[bin];
   Double_t err2 = fSumw2.fArray[bin];
   Double_t neff = GetBinEffectiveEntries(bin);
   //cout<<" neff="<<neff<<" err2="<<err2<<" cont="<<cont<<" sum="<<sum<<endl;
   if (sum == 0) return 0;
   Double_t contsum = cont/sum;
   Double_t eprim2  = TMath::Abs(err2/sum - contsum*contsum);
   Double_t eprim   = TMath::Sqrt(eprim2);
   Double_t test = 1;
   if (err2 != 0 && neff < 5) test = eprim2*sum/err2;
   
  //Int_t cellLimit = (p->GetDimension() == 3)?1000404:10404;
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
Double_t TProfileMDD::GetBinError(const vector<Int_t> & binCoord){

  // Find the bin and return the content
  Int_t globalBin = GetGlobalBin(binCoord);
  return GetBinError(globalBin);
}

// --------------------------------------------------------------
void TProfileMDD::Test(){

  AddAxis("x",30,0,130);
  AddAxis("y",20,0,3500);


  // Vector of coordinates initialized to 
  // have h->GetNaxis() elements
  // coord[0]=x, coord[1]=y, and so on...
  vector<Double_t> coord (GetNaxis(),0);

  // Throw some random values
  TRandom3 rn;
  for (int pse = 0; pse < 1000; pse ++) {
      
    coord[0] = rn.Gaus(60,10);
    coord[1] = rn.Gaus(1000,100);
    
    Fill(coord,rn.Gaus(13, 0.2));

  }//for pse's
  
  // Bin -> Vector of bins
  vector<Int_t> bin (GetNaxis(),0);
  bin[0] = 2;
  bin[1] = 2;
  
  cout<<"TEST()"<<endl;
  cout<<" \tfNcells="<<fNcells<<endl;
  cout<<" \tbin[0]="<<bin[0]<<" bin[1]="<<bin[1]<<endl;
  cout<<" \tGlobal bin  ="<< GetGlobalBin(bin)<<endl;
  cout<<" \tBin Content ="<< GetBinContent(bin)<<endl;
  cout<<" \tBin Error   ="<< GetBinError(bin)<<endl;

}//Test

// --------------------------------------------------------------
void TProfileMDD::LoopOverBins(){

  for (Int_t b=0; b < fNcells ; b++){
     if(GetBinEffectiveEntries(b)==0) 
        continue;
     cout<<"\tGlobal bin ="<<b
         <<" content="<< GetBinContent(b)
         <<" +/- "<< GetBinError(b)
         <<" entries "<< GetBinEffectiveEntries(b)
         <<endl;
  }//for
}

// --------------------------------------------------------------
TProfileMDD* TProfileMDD::ReadFromFile(TString filename, TString objectName){
   TFile *f = new TFile(filename);
   return (TProfileMDD*)gDirectory->Get("RespVsPileup");
}

// --------------------------------------------------------------
void TProfileMDD::WriteToFile(TString filename, TString writeFlag){
   TFile* f = new TFile (filename,writeFlag);
   this->Write();
   f->Close();
   delete f;
}

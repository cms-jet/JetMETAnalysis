#include "TROOT.h"
#include "TSpline.h"
#include "TVirtualPad.h"
#include "TH1.h"
#include "TF1.h"
#include "TSystem.h"
#include "Riostream.h"
#include "TClass.h"
#include "TMath.h"

#include "JetMETAnalysis/JetUtilities/interface/TSplineSteffen.hh"

// Based on: https://root.cern.ch/doc/master/TSpline_8cxx_source.html

ClassImp(TSplinePolySteffen)
ClassImp(TSplineSteffen)

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TSplinePolySteffen                                                   //
//                                                                      //
// Class for TSplineSteffen knot                                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
///assignment operator

TSplinePolySteffen &TSplinePolySteffen::operator=(TSplinePolySteffen const &other)
{
   if(this != &other) {
      TSplinePoly::operator=(other);
      CopyPoly(other);
   }
   return *this;
}

////////////////////////////////////////////////////////////////////////////////
///utility called by the copy constructors and = operator

void TSplinePolySteffen::CopyPoly(TSplinePolySteffen const &other)
{
   fA = other.fA;
   fB = other.fB;
   fC = other.fC;
   fD = other.fD;
}


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TSplineSteffen                                                       //
//                                                                      //
// Class to create third splines to interpolate knots                   //
// Arbitrary conditions can be introduced for first and second          //
// derivatives at beginning and ending points                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
/// Third spline creator given an array of
/// arbitrary knots in increasing abscissa order and
/// possibly end point conditions

TSplineSteffen::TSplineSteffen(const char *title, Double_t x[], Double_t y[], Int_t n, const char *opt, Double_t valbeg, Double_t valend) :
  TSpline(title,-1,x[0],x[n-1],n,kFALSE), fValBeg(valbeg), fValEnd(valend), fBegCond(0), fEndCond(0)
{
   fName="SplineSteffen";

   // Set endpoint conditions
   if(opt) SetCond(opt);

   // Create the plynomial terms and fill
   // them with node information
   fPoly = new TSplinePolySteffen[n];
   for (Int_t i=0; i<n; ++i) {
      fPoly[i].X() = x[i];
      fPoly[i].Y() = y[i];
   }

   // Build the spline coefficients
   BuildCoeff();
}

////////////////////////////////////////////////////////////////////////////////
/// Third spline creator given an array of
/// arbitrary function values on equidistant n abscissa
/// values from xmin to xmax and possibly end point conditions

TSplineSteffen::TSplineSteffen(const char *title, Double_t xmin, Double_t xmax, Double_t y[], Int_t n, const char *opt, Double_t valbeg, Double_t valend) :
  TSpline(title,(xmax-xmin)/(n-1), xmin, xmax, n, kTRUE), fValBeg(valbeg), fValEnd(valend), fBegCond(0), fEndCond(0)
{
   fName="SplineSteffen";

   // Set endpoint conditions
   if(opt) SetCond(opt);

   // Create the plynomial terms and fill
   // them with node information
   fPoly = new TSplinePolySteffen[n];
   for (Int_t i=0; i<n; ++i) {
      fPoly[i].X() = fXmin+i*fDelta;
      fPoly[i].Y() = y[i];
   }

   // Build the spline coefficients
   BuildCoeff();
}


////////////////////////////////////////////////////////////////////////////////
/// Third spline creator given an array of
/// arbitrary abscissas in increasing order and a function
/// to interpolate and possibly end point conditions

TSplineSteffen::TSplineSteffen(const char *title, Double_t x[], const TF1 *func, Int_t n, const char *opt, Double_t valbeg, Double_t valend) :
  TSpline(title,-1, x[0], x[n-1], n, kFALSE), fValBeg(valbeg), fValEnd(valend), fBegCond(0), fEndCond(0)
{
   fName="SplineSteffen";

   // Set endpoint conditions
   if(opt) SetCond(opt);

   // Create the plynomial terms and fill
   // them with node information
   fPoly = new TSplinePolySteffen[n];
   for (Int_t i=0; i<n; ++i) {
      fPoly[i].X() = x[i];
      fPoly[i].Y() = ((TF1*)func)->Eval(x[i]);
   }

   // Build the spline coefficients
   BuildCoeff();
}


////////////////////////////////////////////////////////////////////////////////
/// Third spline creator given a function to be
/// evaluated on n equidistand abscissa points between xmin
/// and xmax and possibly end point conditions

TSplineSteffen::TSplineSteffen(const char *title, Double_t xmin, Double_t xmax, const TF1 *func, Int_t n, const char *opt, Double_t valbeg, Double_t valend) :
  TSpline(title,(xmax-xmin)/(n-1), xmin, xmax, n, kTRUE), fValBeg(valbeg), fValEnd(valend), fBegCond(0), fEndCond(0)
{
   fName="SplineSteffen";

   // Set endpoint conditions
   if(opt) SetCond(opt);

   // Create the plynomial terms and fill
   // them with node information
   fPoly = new TSplinePolySteffen[n];
   //when func is null we return. In this case it is assumed that the spline
   //points will be given later via SetPoint and SetPointCoeff
   if (!func) {fKstep = kFALSE; fDelta = -1; return;}
   for (Int_t i=0; i<n; ++i) {
      Double_t x=fXmin+i*fDelta;
      fPoly[i].X() = x;
      fPoly[i].Y() = ((TF1*)func)->Eval(x);
   }

   // Build the spline coefficients
   BuildCoeff();
}


////////////////////////////////////////////////////////////////////////////////
/// Third spline creator given a TGraph with
/// abscissa in increasing order and possibly end
/// point conditions

TSplineSteffen::TSplineSteffen(const char *title, const TGraph *g, const char *opt, Double_t valbeg, Double_t valend) :
  TSpline(title,-1,0,0,g->GetN(),kFALSE), fValBeg(valbeg), fValEnd(valend), fBegCond(0), fEndCond(0)
{
   fName="SplineSteffen";

   // Set endpoint conditions
   if(opt) SetCond(opt);

   // Create the plynomial terms and fill
   // them with node information
   fPoly = new TSplinePolySteffen[fNp];
   for (Int_t i=0; i<fNp; ++i) {
      Double_t xx, yy;
      g->GetPoint(i,xx,yy);
      fPoly[i].X()=xx;
      fPoly[i].Y()=yy;
   }
   fXmin = fPoly[0].X();
   fXmax = fPoly[fNp-1].X();

   // Build the spline coefficients
   BuildCoeff();
}


////////////////////////////////////////////////////////////////////////////////
/// Third spline creator given a TH1

TSplineSteffen::TSplineSteffen(const TH1 *h, const char *opt, Double_t valbeg, Double_t valend) :
  TSpline(h->GetTitle(),-1,0,0,h->GetNbinsX(),kFALSE), fValBeg(valbeg), fValEnd(valend), fBegCond(0), fEndCond(0)
{
   fName=h->GetName();

   // Set endpoint conditions
   if(opt) SetCond(opt);

   // Create the plynomial terms and fill
   // them with node information
   fPoly = new TSplinePolySteffen[fNp];
   for (Int_t i=0; i<fNp; ++i) {
      fPoly[i].X()=h->GetXaxis()->GetBinCenter(i+1);
      fPoly[i].Y()=h->GetBinContent(i+1);
   }
   fXmin = fPoly[0].X();
   fXmax = fPoly[fNp-1].X();

   // Build the spline coefficients
   BuildCoeff();
}


////////////////////////////////////////////////////////////////////////////////
///copy constructor

TSplineSteffen::TSplineSteffen(const TSplineSteffen& sp3) :
  TSpline(sp3),
  fPoly(0),
  fValBeg(sp3.fValBeg),
  fValEnd(sp3.fValEnd),
  fBegCond(sp3.fBegCond),
  fEndCond(sp3.fEndCond)
{
   if (fNp > 0) fPoly = new TSplinePolySteffen[fNp];
   for (Int_t i=0; i<fNp; ++i)
      fPoly[i] = sp3.fPoly[i];
}


////////////////////////////////////////////////////////////////////////////////
///assignment operator

TSplineSteffen& TSplineSteffen::operator=(const TSplineSteffen& sp3)
{
   if(this!=&sp3) {
      TSpline::operator=(sp3);
      fPoly= 0;
      if (fNp > 0) fPoly = new TSplinePolySteffen[fNp];
      for (Int_t i=0; i<fNp; ++i)
         fPoly[i] = sp3.fPoly[i];

      fValBeg=sp3.fValBeg;
      fValEnd=sp3.fValEnd;
      fBegCond=sp3.fBegCond;
      fEndCond=sp3.fEndCond;
   }
   return *this;
}


////////////////////////////////////////////////////////////////////////////////
/// Check the boundary conditions

void TSplineSteffen::SetCond(const char *opt)
{
   const char *b1 = strstr(opt,"b1");
   const char *e1 = strstr(opt,"e1");
   const char *b2 = strstr(opt,"b2");
   const char *e2 = strstr(opt,"e2");
   if (b1 && b2)
      Error("SetCond","Cannot specify first and second derivative at first point");
   if (e1 && e2)
      Error("SetCond","Cannot specify first and second derivative at last point");
   if (b1) fBegCond=1;
   else if (b2) fBegCond=2;
   if (e1) fEndCond=1;
   else if (e2) fEndCond=2;
}


////////////////////////////////////////////////////////////////////////////////
/// Test method for TSplineSteffen
///
///   n          number of data points.
///   m          2*m-1 is order of spline.
///                 m = 2 always for third spline.
///   nn,nm1,mm,
///   mm1,i,k,
///   j,jj       temporary integer variables.
///   z,p        temporary double precision variables.
///   x[n]       the sequence of knots.
///   y[n]       the prescribed function values at the knots.
///   a[200][4]  two dimensional array whose columns are
///                 the computed spline coefficients
///   diff[3]    maximum values of differences of values and
///                 derivatives to right and left of knots.
///   com[3]     maximum values of coefficients.
///
///
///   test of TSplineSteffen with nonequidistant knots and
///      equidistant knots follows.

void TSplineSteffen::Test()
{
   Double_t hx;
   Double_t diff[3];
   Double_t a[1000], c[4];
   Int_t i, j, k, m, n;
   Double_t x[200], y[200], z;
   Int_t jj, mm;
   Int_t mm1, nm1;
   Double_t com[3];
   printf("1         TEST OF TSplineSteffen WITH NONEQUIDISTANT KNOTS\n");
   n = 5;
   x[0] = -3;
   x[1] = -1;
   x[2] = 0;
   x[3] = 3;
   x[4] = 4;
   y[0] = 7;
   y[1] = 11;
   y[2] = 26;
   y[3] = 56;
   y[4] = 29;
   m = 2;
   mm = m << 1;
   mm1 = mm-1;
   printf("\n-N = %3d    M =%2d\n",n,m);
   TSplineSteffen *spline = new TSplineSteffen("Test",x,y,n);
   for (i = 0; i < n; ++i)
      spline->GetCoeff(i,hx,a[i+800],a[i],a[i+200],a[i+400],a[i+600]);
   delete spline;
   for (i = 0; i < mm1; ++i) diff[i] = com[i] = 0;
   for (k = 0; k < n; ++k) {
      for (i = 0; i < mm; ++i) c[i] = a[k+i*200];
      printf(" ---------------------------------------%3d --------------------------------------------\n",k+1);
      printf("%12.8f\n",x[k]);
      if (k == n-1) {
         printf("%16.8f\n",c[0]);
      } else {
         for (i = 0; i < mm; ++i) printf("%16.8f",c[i]);
         printf("\n");
         for (i = 0; i < mm1; ++i)
            if ((z=TMath::Abs(a[k+i*200])) > com[i]) com[i] = z;
         z = x[k+1]-x[k];
         for (i = 1; i < mm; ++i)
            for (jj = i; jj < mm; ++jj) {
               j = mm+i-jj;
               c[j-2] = c[j-1]*z+c[j-2];
            }
         for (i = 0; i < mm; ++i) printf("%16.8f",c[i]);
         printf("\n");
         for (i = 0; i < mm1; ++i)
            if (!(k >= n-2 && i != 0))
               if((z = TMath::Abs(c[i]-a[k+1+i*200]))
                  > diff[i]) diff[i] = z;
      }
   }
   printf("  MAXIMUM ABSOLUTE VALUES OF DIFFERENCES \n");
   for (i = 0; i < mm1; ++i) printf("%18.9E",diff[i]);
   printf("\n");
   printf("  MAXIMUM ABSOLUTE VALUES OF COEFFICIENTS \n");
   if (TMath::Abs(c[0]) > com[0])
      com[0] = TMath::Abs(c[0]);
   for (i = 0; i < mm1; ++i) printf("%16.8f",com[i]);
   printf("\n");
   m = 2;
   for (n = 10; n <= 100; n += 10) {
      mm = m << 1;
      mm1 = mm-1;
      nm1 = n-1;
      for (i = 0; i < nm1; i += 2) {
         x[i] = i+1;
         x[i+1] = i+2;
         y[i] = 1;
         y[i+1] = 0;
      }
      if (n % 2 != 0) {
         x[n-1] = n;
         y[n-1] = 1;
      }
      printf("\n-N = %3d    M =%2d\n",n,m);
      spline = new TSplineSteffen("Test",x,y,n);
      for (i = 0; i < n; ++i)
         spline->GetCoeff(i,hx,a[i+800],a[i],a[i+200],a[i+400],a[i+600]);
      delete spline;
      for (i = 0; i < mm1; ++i)
         diff[i] = com[i] = 0;
      for (k = 0; k < n; ++k) {
         for (i = 0; i < mm; ++i)
            c[i] = a[k+i*200];
         if (n < 11) {
            printf(" ---------------------------------------%3d --------------------------------------------\n",k+1);
            printf("%12.8f\n",x[k]);
            if (k == n-1) printf("%16.8f\n",c[0]);
         }
         if (k == n-1) break;
         if (n <= 10) {
            for (i = 0; i < mm; ++i) printf("%16.8f",c[i]);
            printf("\n");
         }
         for (i = 0; i < mm1; ++i)
         if ((z=TMath::Abs(a[k+i*200])) > com[i])
            com[i] = z;
         z = x[k+1]-x[k];
         for (i = 1; i < mm; ++i)
            for (jj = i; jj < mm; ++jj) {
               j = mm+i-jj;
               c[j-2] = c[j-1]*z+c[j-2];
            }
         if (n <= 10) {
            for (i = 0; i < mm; ++i) printf("%16.8f",c[i]);
            printf("\n");
         }
         for (i = 0; i < mm1; ++i)
         if (!(k >= n-2 && i != 0))
            if ((z = TMath::Abs(c[i]-a[k+1+i*200]))
               > diff[i]) diff[i] = z;
      }
      printf("  MAXIMUM ABSOLUTE VALUES OF DIFFERENCES \n");
      for (i = 0; i < mm1; ++i) printf("%18.9E",diff[i]);
      printf("\n");
      printf("  MAXIMUM ABSOLUTE VALUES OF COEFFICIENTS \n");
      if (TMath::Abs(c[0]) > com[0])
         com[0] = TMath::Abs(c[0]);
      for (i = 0; i < mm1; ++i) printf("%16.8E",com[i]);
         printf("\n");
   }
}


////////////////////////////////////////////////////////////////////////////////
/// Find X

Int_t TSplineSteffen::FindX(Double_t x) const
{
   Int_t klow=0, khig=fNp-1;
   //
   // If out of boundaries, extrapolate
   // It may be badly wrong
   if(x<=fXmin) klow=0;
   else if(x>=fXmax) klow=khig;
   else {
      if(fKstep) {
         //
         // Equidistant knots, use histogramming
         klow = TMath::FloorNint((x-fXmin)/fDelta);
         // Correction for rounding errors
         if (x < fPoly[klow].X())
            klow = TMath::Max(klow-1,0);
         else if (klow < khig) {
            if (x > fPoly[klow+1].X()) ++klow;
         }
      } else {
         Int_t khalf;
         //
         // Non equidistant knots, binary search
         while(khig-klow>1)
            if(x>fPoly[khalf=(klow+khig)/2].X())
               klow=khalf;
            else
               khig=khalf;
         //
         // This could be removed, sanity check
         if(!(fPoly[klow].X()<=x && x<=fPoly[klow+1].X()))
            Error("Eval",
                  "Binary search failed x(%d) = %f < x= %f < x(%d) = %f\n",
                  klow,fPoly[klow].X(),x,klow+1,fPoly[klow+1].X());
      }
   }
   return klow;
}


////////////////////////////////////////////////////////////////////////////////
/// Eval this spline at x

Double_t TSplineSteffen::Eval(Double_t x) const
{
   Int_t klow=FindX(x);
   if (klow >= fNp-1 && fNp > 1) klow = fNp-2; //see: https://savannah.cern.ch/bugs/?71651
   return fPoly[klow].Eval(x);
}


////////////////////////////////////////////////////////////////////////////////
/// Derivative

Double_t TSplineSteffen::Derivative(Double_t x) const
{
   Int_t klow=FindX(x);
   if (klow >= fNp-1) klow = fNp-2; //see: https://savannah.cern.ch/bugs/?71651
   return fPoly[klow].Derivative(x);
}


////////////////////////////////////////////////////////////////////////////////
/// write this spline as a C++ function that can be executed without ROOT
/// the name of the function is the name of the file up to the "." if any

void TSplineSteffen::SaveAs(const char *filename, Option_t * /*option*/) const
{
   //open the file
   std::ofstream *f = new std::ofstream(filename,std::ios::out);
   if (f == 0 || gSystem->AccessPathName(filename,kWritePermission)) {
      Error("SaveAs","Cannot open file:%s\n",filename);
      return;
   }

   //write the function name and the spline constants
   char buffer[512];
   Int_t nch = strlen(filename);
   snprintf(buffer,512,"double %s",filename);
   char *dot = strstr(buffer,".");
   if (dot) *dot = 0;
   strlcat(buffer,"(double x) {\n",512);
   nch = strlen(buffer); f->write(buffer,nch);
   snprintf(buffer,512,"   const int fNp = %d, fKstep = %d;\n",fNp,fKstep);
   nch = strlen(buffer); f->write(buffer,nch);
   snprintf(buffer,512,"   const double fDelta = %g, fXmin = %g, fXmax = %g;\n",fDelta,fXmin,fXmax);
   nch = strlen(buffer); f->write(buffer,nch);

   //write the spline coefficients
   //array fX
   snprintf(buffer,512,"   const double fX[%d] = {",fNp);
   nch = strlen(buffer); f->write(buffer,nch);
   buffer[0] = 0;
   Int_t i;
   char numb[20];
   for (i=0;i<fNp;i++) {
      snprintf(numb,20," %g,",fPoly[i].X());
      nch = strlen(numb);
      if (i == fNp-1) numb[nch-1]=0;
      strlcat(buffer,numb,512);
      if (i%5 == 4 || i == fNp-1) {
         nch = strlen(buffer); f->write(buffer,nch);
         if (i != fNp-1) snprintf(buffer,512,"\n                       ");
      }
   }
   snprintf(buffer,512," };\n");
   nch = strlen(buffer); f->write(buffer,nch);
   //array fY
   snprintf(buffer,512,"   const double fY[%d] = {",fNp);
   nch = strlen(buffer); f->write(buffer,nch);
   buffer[0] = 0;
   for (i=0;i<fNp;i++) {
      snprintf(numb,20," %g,",fPoly[i].Y());
      nch = strlen(numb);
      if (i == fNp-1) numb[nch-1]=0;
      strlcat(buffer,numb,512);
      if (i%5 == 4 || i == fNp-1) {
         nch = strlen(buffer); f->write(buffer,nch);
         if (i != fNp-1) snprintf(buffer,512,"\n                       ");
      }
   }
   snprintf(buffer,512," };\n");
   nch = strlen(buffer); f->write(buffer,nch);
   //array fA
   snprintf(buffer,512,"   const double fA[%d] = {",fNp);
   nch = strlen(buffer); f->write(buffer,nch);
   buffer[0] = 0;
   for (i=0;i<fNp;i++) {
      snprintf(numb,20," %g,",fPoly[i].A());
      nch = strlen(numb);
      if (i == fNp-1) numb[nch-1]=0;
      strlcat(buffer,numb,512);
      if (i%5 == 4 || i == fNp-1) {
         nch = strlen(buffer); f->write(buffer,nch);
         if (i != fNp-1) snprintf(buffer,512,"\n                       ");
      }
   }
   snprintf(buffer,512," };\n");
   nch = strlen(buffer); f->write(buffer,nch);
   //array fB
   snprintf(buffer,512,"   const double fB[%d] = {",fNp);
   nch = strlen(buffer); f->write(buffer,nch);
   buffer[0] = 0;
   for (i=0;i<fNp;i++) {
      snprintf(numb,20," %g,",fPoly[i].B());
      nch = strlen(numb);
      if (i == fNp-1) numb[nch-1]=0;
      strlcat(buffer,numb,512);
      if (i%5 == 4 || i == fNp-1) {
         nch = strlen(buffer); f->write(buffer,nch);
         if (i != fNp-1) snprintf(buffer,512,"\n                       ");
      }
   }
   snprintf(buffer,512," };\n");
   nch = strlen(buffer); f->write(buffer,nch);
   //array fC
   snprintf(buffer,512,"   const double fC[%d] = {",fNp);
   nch = strlen(buffer); f->write(buffer,nch);
   buffer[0] = 0;
   for (i=0;i<fNp;i++) {
      snprintf(numb,20," %g,",fPoly[i].C());
      nch = strlen(numb);
      if (i == fNp-1) numb[nch-1]=0;
      strlcat(buffer,numb,512);
      if (i%5 == 4 || i == fNp-1) {
         nch = strlen(buffer); f->write(buffer,nch);
         if (i != fNp-1) snprintf(buffer,512,"\n                       ");
      }
   }
   snprintf(buffer,512," };\n");
   nch = strlen(buffer); f->write(buffer,nch);
    //array fD
   snprintf(buffer,512,"   const double fD[%d] = {",fNp);
   nch = strlen(buffer); f->write(buffer,nch);
   buffer[0] = 0;
   for (i=0;i<fNp;i++) {
      snprintf(numb,20," %g,",fPoly[i].D());
      nch = strlen(numb);
      if (i == fNp-1) numb[nch-1]=0;
      strlcat(buffer,numb,512);
      if (i%5 == 4 || i == fNp-1) {
         nch = strlen(buffer); f->write(buffer,nch);
         if (i != fNp-1) snprintf(buffer,512,"\n                       ");
      }
   }
   snprintf(buffer,512," };\n");
   nch = strlen(buffer); f->write(buffer,nch);

   //generate code for the spline evaluation
   snprintf(buffer,512,"   int klow=0;\n");
   nch = strlen(buffer); f->write(buffer,nch);

   snprintf(buffer,512,"   // If out of boundaries, extrapolate. It may be badly wrong\n");
   snprintf(buffer,512,"   if(x<=fXmin) klow=0;\n");
   nch = strlen(buffer); f->write(buffer,nch);
   snprintf(buffer,512,"   else if(x>=fXmax) klow=fNp-1;\n");
   nch = strlen(buffer); f->write(buffer,nch);
   snprintf(buffer,512,"   else {\n");
   nch = strlen(buffer); f->write(buffer,nch);
   snprintf(buffer,512,"     if(fKstep) {\n");
   nch = strlen(buffer); f->write(buffer,nch);

   snprintf(buffer,512,"       // Equidistant knots, use histogramming\n");
   nch = strlen(buffer); f->write(buffer,nch);
   snprintf(buffer,512,"       klow = int((x-fXmin)/fDelta);\n");
   nch = strlen(buffer); f->write(buffer,nch);
   snprintf(buffer,512,"       if (klow < fNp-1) klow = fNp-1;\n");
   nch = strlen(buffer); f->write(buffer,nch);
   snprintf(buffer,512,"     } else {\n");
   nch = strlen(buffer); f->write(buffer,nch);
   snprintf(buffer,512,"       int khig=fNp-1, khalf;\n");
   nch = strlen(buffer); f->write(buffer,nch);

   snprintf(buffer,512,"       // Non equidistant knots, binary search\n");
   nch = strlen(buffer); f->write(buffer,nch);
   snprintf(buffer,512,"       while(khig-klow>1)\n");
   nch = strlen(buffer); f->write(buffer,nch);
   snprintf(buffer,512,"         if(x>fX[khalf=(klow+khig)/2]) klow=khalf;\n");
   nch = strlen(buffer); f->write(buffer,nch);
   snprintf(buffer,512,"         else khig=khalf;\n");
   nch = strlen(buffer); f->write(buffer,nch);
   snprintf(buffer,512,"     }\n");
   nch = strlen(buffer); f->write(buffer,nch);
   snprintf(buffer,512,"   }\n");
   nch = strlen(buffer); f->write(buffer,nch);
   snprintf(buffer,512,"   // Evaluate now\n");
   nch = strlen(buffer); f->write(buffer,nch);
   snprintf(buffer,512,"   double dx=x-fX[klow];\n");
   nch = strlen(buffer); f->write(buffer,nch);
   snprintf(buffer,512,"   return (fD[klow]+dx*(fC[klow]+dx*(fB[klow]+dx*fA[klow])));\n");
   nch = strlen(buffer); f->write(buffer,nch);

   //close file
   f->write("}\n",2);

   if (f) { f->close(); delete f;}
}


////////////////////////////////////////////////////////////////////////////////
/// Save primitive as a C++ statement(s) on output stream out

void TSplineSteffen::SavePrimitive(std::ostream &out, Option_t *option /*= ""*/)
{
   char quote = '"';
   out<<"   "<<std::endl;
   if (gROOT->ClassSaved(TSplineSteffen::Class())) {
      out<<"   ";
   } else {
      out<<"   TSplineSteffen *";
   }
   out<<"splineSteffen = new TSplineSteffen("<<quote<<GetTitle()<<quote<<","
      <<fXmin<<","<<fXmax<<",(TF1*)0,"<<fNp<<","<<quote<<quote<<","
      <<fValBeg<<","<<fValEnd<<");"<<std::endl;
   out<<"   splineSteffen->SetName("<<quote<<GetName()<<quote<<");"<<std::endl;

   SaveFillAttributes(out,"splineSteffen",0,1001);
   SaveLineAttributes(out,"splineSteffen",1,1,1);
   SaveMarkerAttributes(out,"splineSteffen",1,1,1);
   if (fNpx != 100) out<<"   splineSteffen->SetNpx("<<fNpx<<");"<<std::endl;

   for (Int_t i=0;i<fNp;i++) {
      out<<"   splineSteffen->SetPoint("<<i<<","<<fPoly[i].X()<<","<<fPoly[i].Y()<<");"<<std::endl;
      out<<"   splineSteffen->SetPointCoeff("<<i<<","<<fPoly[i].A()<<","<<fPoly[i].B()<<","<<fPoly[i].C()<<","<<fPoly[i].D()<<");"<<std::endl;
   }
   out<<"   splineSteffen->Draw("<<quote<<option<<quote<<");"<<std::endl;
}


////////////////////////////////////////////////////////////////////////////////
///set point number i.

void TSplineSteffen::SetPoint(Int_t i, Double_t x, Double_t y)
{
   if (i < 0 || i >= fNp) return;
   fPoly[i].X()= x;
   fPoly[i].Y()= y;
}

////////////////////////////////////////////////////////////////////////////////
/// set point coefficient number i

void TSplineSteffen::SetPointCoeff(Int_t i, Double_t a, Double_t b, Double_t c, Double_t d)
{
   if (i < 0 || i >= fNp) return;
   fPoly[i].A()= a;
   fPoly[i].B()= b;
   fPoly[i].C()= c;
   fPoly[i].D()= d;
}

////////////////////////////////////////////////////////////////////////////////
///      subroutine taken from the GSL Steffen spline code
///      https://github.com/ampl/gsl/blob/master/interpolation/steffen.c#L110-L179

void TSplineSteffen::BuildCoeff()
{

   double y_prime[fNp];

   /*
   * first assign the interval and slopes for the left boundary.
   * We use the "simplest possibility" method described in the paper
   * in section 2.2
   */
   double h0 = fPoly[1].X() - fPoly[0].X();
   double s0 = fPoly[1].Y() - fPoly[0].Y() / h0;

   y_prime[0] = s0;

   /* Now we calculate all the necessary s, h, p, and y' variables 
     from 1 to N-2 (0 to size - 2 inclusive) */
   for (Int_t i = 1; i < (fNp - 1); i++)
   {
      double pi;

      /* equation 6 in the paper */
      double hi = (fPoly[i+1].X() - fPoly[i].X());
      double him1 = (fPoly[i].X() - fPoly[i - 1].X());

      /* equation 7 in the paper */
      double si = (fPoly[i+1].Y() - fPoly[i].Y()) / hi;
      double sim1 = (fPoly[i].Y() - fPoly[i - 1].Y()) / him1;

      /* equation 8 in the paper */
      pi = (sim1*hi + si*him1) / (him1 + hi);

      /* This is a C equivalent of the FORTRAN statement below eqn 11 */
      y_prime[i] = (steffen_copysign(1.0,sim1) + steffen_copysign(1.0,si)) * std::min(fabs(sim1),std::min(fabs(si), 0.5*fabs(pi))); 
   }

   /*
   * we also need y' for the rightmost boundary; we use the
   * "simplest possibility" method described in the paper in
   * section 2.2
   *
   * y' = s_{n-1}
   */
   y_prime[fNp-1] = (fPoly[fNp - 1].Y() - fPoly[fNp - 2].Y()) / (fPoly[fNp - 1].X() - fPoly[fNp - 2].X());

  /* Now we can calculate all the coefficients for the whole range. */
   for (Int_t i = 0; i < (fNp - 1); i++)
   {
      double hi = (fPoly[i+1].X() - fPoly[i].X());
      double si = (fPoly[i+1].Y() - fPoly[i].Y()) / hi;

      /* These are from equations 2-5 in the paper. */
      fPoly[i].A() = (y_prime[i] + y_prime[i+1] - 2*si) / hi / hi;
      fPoly[i].B() = (3*si - 2*y_prime[i] - y_prime[i+1]) / hi;
      fPoly[i].C() = y_prime[i];
      fPoly[i].D() = fPoly[i].Y();
   }
}


////////////////////////////////////////////////////////////////////////////////
/// Stream an object of class TSplineSteffen.
/*
//https://root.cern.ch/root/SchemaEvolution.html
void TSplineSteffen::Streamer(TBuffer &R__b)
{
   if (R__b.IsReading()) {
      UInt_t R__s, R__c;
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
      if (R__v > 1) {
         R__b.ReadClassBuffer(TSplineSteffen::Class(), this, R__v, R__s, R__c);
         return;
      }
      //====process old versions before automatic schema evolution
      TSpline::Streamer(R__b);
      if (fNp > 0) {
         fPoly = new TSplinePolySteffen[fNp];
         for(Int_t i=0; i<fNp; ++i) {
            fPoly[i].Streamer(R__b);
         }
      }
      //      R__b >> fPoly;
      R__b >> fValBeg;
      R__b >> fValEnd;
      R__b >> fBegCond;
      R__b >> fEndCond;
   } else {
      R__b.WriteClassBuffer(TSplineSteffen::Class(),this);
   }
}
*/



#ifndef ROOT_TSplineSteffen
#define ROOT_TSplineSteffen

// Based on: https://root.cern.ch/doc/master/TSpline_8h_source.html

#ifndef ROOT_TGraph
#include "TGraph.h"
#endif

#ifndef ROOT_TSpline
#include "TSpline.h"
#endif

class TH1;
class TF1;

//______________________________________________________________________________
class TSplinePolySteffen : public TSplinePoly
{
private:
   Double_t fA; //
   Double_t fB; // 
   Double_t fC; // 
   Double_t fD; //

public:
   TSplinePolySteffen() :
      fA(0), fB(0), fC(0), fD(0) {}
   TSplinePolySteffen(Double_t x, Double_t y, Double_t a, Double_t b, Double_t c, Double_t d) :
      TSplinePoly(x,y), fA(a), fB(b), fC(c), fD(d) {}
   TSplinePolySteffen(TSplinePolySteffen const &other);
   TSplinePolySteffen &operator=(TSplinePolySteffen const &other);

   Double_t &A() {return fA;}
   Double_t &B() {return fB;}
   Double_t &C() {return fC;}
   Double_t &D() {return fD;}
   Double_t Eval(Double_t x) const {
      Double_t dx=x-fX;
      return (fD+dx*(fC+dx*(fB+dx*fA)));
   }
   Double_t Derivative(Double_t x) const {
      Double_t dx=x-fX;
      return (fC+dx*(2*fB+3*fA*dx));      
   }

private:
   void CopyPoly(TSplinePolySteffen const &other);

   ClassDef(TSplinePolySteffen,1)  // Third spline polynomial terms
};

inline TSplinePolySteffen::TSplinePolySteffen(TSplinePolySteffen const &other)
   :
  TSplinePoly(other), fA(0), fB(0), fC(0), fD(0)
{
   CopyPoly(other);
}

//______________________________________________________________________________
class TSplineSteffen : public TSpline
{
protected:
   TSplinePolySteffen  *fPoly;       //[fNp] Array of polynomial terms
   Double_t       		fValBeg;     // Initial value of first or second derivative
   Double_t       		fValEnd;     // End value of first or second derivative
   Int_t          		fBegCond;    // 0=no beg cond, 1=first derivative, 2=second derivative
   Int_t          		fEndCond;    // 0=no end cond, 1=first derivative, 2=second derivative

   void   BuildCoeff();
   void   SetCond(const char *opt);

public:
   TSplineSteffen() : TSpline(), fPoly(0), fValBeg(0), fValEnd(0), fBegCond(-1), fEndCond(-1) {}
   TSplineSteffen(const char *title, Double_t x[], Double_t y[], Int_t n, const char *opt=0, Double_t valbeg=0, Double_t valend=0);
   TSplineSteffen(const char *title, Double_t xmin, Double_t xmax, Double_t y[], Int_t n, const char *opt=0, Double_t valbeg=0, Double_t valend=0);
   TSplineSteffen(const char *title, Double_t x[], const TF1 *func, Int_t n, const char *opt=0, Double_t valbeg=0, Double_t valend=0);
   TSplineSteffen(const char *title, Double_t xmin, Double_t xmax, const TF1 *func, Int_t n, const char *opt=0, Double_t valbeg=0, Double_t valend=0);
   TSplineSteffen(const char *title, const TGraph *g, const char *opt=0, Double_t valbeg=0, Double_t valend=0);
   TSplineSteffen(const TH1 *h, const char *opt=0, Double_t valbeg=0, Double_t valend=0);
   TSplineSteffen(const TSplineSteffen&);
   TSplineSteffen& operator=(const TSplineSteffen&);
   Int_t    FindX(Double_t x) const;
   Double_t Eval(Double_t x) const;
   Double_t Derivative(Double_t x) const;
   virtual ~TSplineSteffen() {if (fPoly) delete [] fPoly;}
   void GetCoeff(Int_t i, Double_t &x, Double_t &y, Double_t&a, Double_t &b,
                 Double_t &c, Double_t &d) {x=fPoly[i].X();y=fPoly[i].Y();
                  a=fPoly[i].A();b=fPoly[i].B();c=fPoly[i].C();d=fPoly[i].D();}
   void GetKnot(Int_t i, Double_t &x, Double_t &y) const
      {x=fPoly[i].X(); y=fPoly[i].Y();}
   virtual  void     SaveAs(const char *filename,Option_t *option="") const;
   virtual  void     SavePrimitive(std::ostream &out, Option_t *option = "");
   virtual  void     SetPoint(Int_t i, Double_t x, Double_t y);
   virtual  void     SetPointCoeff(Int_t i, Double_t a, Double_t b, Double_t c, Double_t d);
   static   double   steffen_copysign(const double x, const double y)
	{
  	if ((x < 0 && y > 0) || (x > 0 && y < 0))
    	return -x;

	  return x;
	}
   static void Test();

   ClassDef (TSplineSteffen,1)  // Class to create third natural splines
};

#endif
#ifndef CRYSTALBALLRES_HH
#define CRYSTALBALLRES_HH

#include <TH1.h>
#include <TF1.h>
#include "TCanvas.h"
#include <vector>

typedef std::vector<double> vdouble; 
typedef std::vector<bool> vbool; 
typedef std::vector<int> vint; 

void crystalBallRes( TH1 *hrsp, bool drawCrystalBall, vdouble &vintegral, vdouble &vnorm, 
   		     vdouble &vrange, vdouble &vmean, vdouble &vsigma,
  	             vdouble &vexp, vdouble &vpolLeft, vdouble &vpolRight, 
  	             vint &voption, int polDeg=1, TCanvas *canvas=0);

#endif

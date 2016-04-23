#ifndef FITCRYSTALBALL_HH
#define FITCRYSTALBALL_HH

#include <TH1F.h>
#include <string>

void fitCrystalBall(TH1F*& hrspOrig, std::string alg, std::string histName, int polDeg, bool normalized, std::string fitDir);

#endif

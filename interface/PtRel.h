#ifndef JETMETANALYSIS_JETUTILITIES_PTREL_H
#define JETMETANALYSIS_JETUTILITIES_PTREL_H 1


#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/Vector3D.h"


#include <cmath>


namespace jetmet
{
  // BASED ON DOUBLE-PRECISION VECTOR CLASSES
  inline double getPtRel(const math::XYZTLorentzVectorD& jet,
			 const math::XYZVectorD& lep,
			 bool addLepToJet)
  {
    float lj_x = (addLepToJet) ? lep.X()+jet.Px() : jet.Px();
    float lj_y = (addLepToJet) ? lep.Y()+jet.Py() : jet.Py();
    float lj_z = (addLepToJet) ? lep.Z()+jet.Pz() : jet.Pz();
    
    // absolute values squared
    float lj2  = lj_x*lj_x+lj_y*lj_y+lj_z*lj_z;
    float lep2 = lep.X()*lep.X()+lep.Y()*lep.Y()+lep.Z()*lep.Z();

    // projection vec(mu) to lepjet axis
    float lepXlj = lep.X()*lj_x+lep.Y()*lj_y+lep.Z()*lj_z;
    
    // absolute value squared and normalized
    float pLrel2 = lepXlj*lepXlj/lj2;
    
    // lep2 = pTrel2 + pLrel2
    float pTrel2 = lep2-pLrel2;
    
    return (pTrel2 > 0) ? std::sqrt(pTrel2) : 0.0;
  }

  
  // BASED ON DOUBLE-PRECISION VALUES
  inline double getPtRel(double jetPt, double jetEta, double jetPhi, double jetE,
			 double lepPx, double lepPy,  double lepPz,
			 bool   addLepToJet)
  {
    
    math::PtEtaPhiELorentzVectorD tmp;
    tmp.SetPt(jetPt);
    tmp.SetEta(jetEta);
    tmp.SetPhi(jetPhi);
    tmp.SetE(jetE);
    math::XYZTLorentzVectorD jet(tmp);

    math::XYZVectorD lep;
    lep.SetXYZ(lepPx,lepPy,lepPz);
    
    return getPtRel(jet,lep,addLepToJet);
  }
  
  
  // BASED ON SINGLE-PRECISION VALUES
  inline float getPtRel(float jetPt, float jetEta, float jetPhi, float jetE,
			float lepPx, float lepPy,  float lepPz,
			bool   addLepToJet)
  {
    return static_cast<float>(getPtRel(static_cast<double>(jetPt),
				       static_cast<double>(jetEta),
				       static_cast<double>(jetPhi),
				       static_cast<double>(jetE),
				       static_cast<double>(lepPx),
				       static_cast<double>(lepPy),
				       static_cast<double>(lepPz),
				       addLepToJet));
  }
  
}


#endif

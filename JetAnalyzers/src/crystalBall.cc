#include "JetMETAnalysis/JetAnalyzers/interface/crystalBall.h"

#include "TMath.h"

double sum_gaus_fnc(double *x, double *par); // sum of 3gaus+pol7

double sum_gaus_pol_fnc(double *x, double *par); // sum of 3gaus+pol7

double exp_fcn(double x, double norm, double slope); // exp 

double gauss_fcn(double x, double norm, double mean, double sigma); // gauss 

double pol_fcn(double x, double p0, double p1=0, double p2=0, double p3=0, double p4=0, double p5=0, double p6=0, double p7=0); // pol7


// global fit
double crystalBall(double *xx, double *pp)
{
  double x = xx[0];

  // left
  double normGaus1     = pp[0];  
  double meanGaus1     = pp[1];  
  double sigGaus1_left = pp[2];   
  double slopeExp_left = pp[4];  
  double p1_left       = pp[6];
  double p2_left       = pp[7]; 
  double p3_left       = pp[8];
  double p4_left       = pp[9]; 
  double p5_left       = pp[10]; 
  double p6_left       = pp[11]; 
  double p7_left       = pp[12]; 
  double R2            = meanGaus1 - 0.5*sigGaus1_left - (pp[14]); //??
  double R1            = R2 - pp[13]; // min exp
  bool drawPolLeft     = pp[30];
  bool drawExp         = pp[31];
  bool drawGaus1       = pp[32];
  bool drawGaus2       = pp[33];
  bool drawGaus3       = pp[34];
  bool drawPolRight    = pp[35];
  bool drawAnyRight    = drawGaus1 || drawGaus2 || drawGaus3 || drawPolRight;

  // remove discontinuity at x=R2: solve exp=gauss for norm exp and replace R2 by (meanGaus1-R2*R2)
  double normExp_left = gauss_fcn(R2, normGaus1, meanGaus1, sigGaus1_left)/TMath::Exp(slopeExp_left*R2);

  // remove discontinuity at x=R1: solve pol7=exp for p0 left 
  double p0_left = exp_fcn(R1, normExp_left, slopeExp_left) -
    ( p1_left*R1 + p2_left*pow(R1,2) +
      p3_left*pow(R1,3) + p4_left*pow(R1,4) +
      p5_left*pow(R1,5) + p6_left*pow(R1,6) +
      p7_left*pow(R1,7) ) ;

  double result = 0.;
  if (drawPolLeft  && x >= 0.        && x < R1       ) result += pol_fcn(x, p0_left, p1_left, p2_left, p3_left, p4_left, p5_left, p6_left, p7_left);
  if (drawExp      && x >= R1        && x < R2       ) result += exp_fcn(x, normExp_left, slopeExp_left);
  if (drawGaus1    && x >= R2        && x < meanGaus1) result += gauss_fcn(x, normGaus1, meanGaus1, sigGaus1_left);
  if (drawAnyRight && x >= meanGaus1 && x < 2.       ) result += sum_gaus_pol_fnc(xx, pp);
  return result;
} // crystalBall


double crystalBall_1(double *xx, double *pp)
{
  double x = xx[0];

  // left
  double normGaus1     = pp[0];  
  double meanGaus1     = pp[1];  
  double sigGaus1_left = pp[2];   

  double slopeExp_left = pp[4];  

  double p1_left       = pp[6];

  double p1_right      = pp[17]; 

  double R2            = meanGaus1 - 0.5*sigGaus1_left - pp[8]; 
  double R1            = R2 - pp[7]; // min exp
  double R3            = pp[18];

  bool drawPolLeft     = pp[19];
  bool drawExp         = pp[20];
  bool drawGaus1       = pp[21];
  bool drawGaus2       = pp[22];
  bool drawGaus3       = pp[23];
  bool drawPolRight    = pp[24];
  bool drawAnyRight    = drawGaus1 || drawGaus2 || drawGaus3;

  // remove discontinuity at x=R2: solve exp=gauss for norm exp and replace R2 by (meanGaus1-R2*R2)
  double normExp_left = gauss_fcn(R2, normGaus1, meanGaus1, sigGaus1_left)/TMath::Exp(slopeExp_left*R2);

  // remove discontinuity at x=R1: solve pol1=exp for p0 left 
  double p0_left = exp_fcn(R1, normExp_left, slopeExp_left) - p1_left*R1 ;

  // remove discontinuity at x=R3: solve pol1=sumGaus for p0 right
  double r3[]{R3};
  double p0_right = sum_gaus_fnc(r3, pp)-p1_right*R3;

  // compute probability
  double result = 0.;
  if (drawPolLeft  && x >= 0.        && x < R1       ) result += pol_fcn  (x, p0_left, p1_left);
  if (drawExp      && x >= R1        && x < R2       ) result += exp_fcn  (x, normExp_left, slopeExp_left);
  if (drawGaus1    && x >= R2        && x < meanGaus1) result += gauss_fcn(x, normGaus1, meanGaus1, sigGaus1_left);
  if (drawAnyRight && x >= meanGaus1 && x < R3       ) result += sum_gaus_fnc(xx, pp); 
  if (drawPolRight && x >= R3        && x < 2.       ) result += pol_fcn  (x, p0_right, p1_right);

  return result;
} // crystalBall_1



//////////////////////
double exp_fcn(double x, double norm, double slope){
  return norm*TMath::Exp(slope*x);
}

double gauss_fcn(double x, double norm, double mean, double sigma){
  return norm*TMath::Exp(-0.5*TMath::Power((x - mean)/sigma, 2.));
}

double pol_fcn(double x, double p0, double p1, double p2, double p3, double p4, double p5, double p6, double p7){
  return p0 + p1*x + p2*TMath::Power(x,2) +
    p3*TMath::Power(x,3) + p4*TMath::Power(x,4) +
    p5*TMath::Power(x,5) + p6*TMath::Power(x,6) +
    p7*TMath::Power(x,7);
}

double sum_gaus_pol_fnc(double *xx, double *pp)
{
  double x = xx[0];

  double normGaus1Left  = pp[0];
  double meanGaus1      = pp[1];
  double valGaus1_at_meanGaus1 = normGaus1Left;

  double normGaus2      = pp[16]; 	        
  double meanGaus2      = meanGaus1 + pp[17];   
  double sigGaus2       = pp[18]; 	        
  double valGaus2_at_meanGaus1 = gauss_fcn(meanGaus1, normGaus2, meanGaus2, sigGaus2);
  double valGaus2       = gauss_fcn(x, normGaus2, meanGaus2, sigGaus2);

  double normGaus3      = pp[19]; 	
  double meanGaus3      = meanGaus2 + pp[20]; 
  double sigGaus3       = pp[21];  		
  double valGaus3_at_meanGaus1 = gauss_fcn(meanGaus1, normGaus3, meanGaus3, sigGaus3);
  double valGaus3       = gauss_fcn(x, normGaus3, meanGaus3, sigGaus3);

  double p0             = pp[22];
  double p1             = pp[23];
  double p2             = pp[24]; 
  double p3             = pp[25];
  double p4             = pp[26]; 
  double p5             = pp[27]; 
  double p6             = pp[28]; 
  double p7             = pp[29]; 
  double valPol_at_meanGaus1 = pol_fcn(meanGaus1, p0, p1, p2, p3, p4, p5, p6, p7);
  double valPol         = pol_fcn(x, p0, p1, p2, p3, p4, p5, p6, p7);

  double normGaus1Right = valGaus1_at_meanGaus1 - (valGaus2_at_meanGaus1 + valGaus3_at_meanGaus1 + valPol_at_meanGaus1);
  double sigGaus1_right = pp[15]; 
  double valGaus1_right = gauss_fcn(x, normGaus1Right, meanGaus1, sigGaus1_right);

  bool drawGaus1        = pp[32];
  bool drawGaus2        = pp[33];
  bool drawGaus3        = pp[34];
  bool drawPolRight     = pp[35];

  //if ( drawGaus1 && drawGaus2 && drawGaus3 && drawPolRight && x > 0.99 && x < 1.01 ) {
  //  std::cout << "<sum_gaus_pol_fnc>:" << std::endl;
  //  std::cout << " x = " << x << ": g1 = " << valGaus1_right << ", g2 = " << valGaus2 << ", g3 = " << valGaus3 << ", pol = " << valPol << std::endl;
  //}

  double sum = 0.;
  if ( drawGaus1       ) sum += valGaus1_right;    
  if ( drawGaus2       ) sum += valGaus2;
  if ( drawGaus3       ) sum += valGaus3;    
  if ( drawPolRight ) sum += valPol;
  if ( normGaus1Right < 0. ) sum /= (1. + normGaus1Right*normGaus1Right);
  return sum;
}


double sum_gaus_fnc(double *xx, double *pp)
{
  double x = xx[0];

  double normGaus1Left  = pp[0];
  double meanGaus1      = pp[1];
  double valGaus1_at_meanGaus1 = normGaus1Left;

  double normGaus2      = pp[10]; 	        
  double meanGaus2      = meanGaus1 + pp[11];   
  double sigGaus2       = pp[12]; 	        
  double valGaus2_at_meanGaus1 = gauss_fcn(meanGaus1, normGaus2, meanGaus2, sigGaus2);
  double valGaus2       = gauss_fcn(x, normGaus2, meanGaus2, sigGaus2);

  double normGaus3      = pp[13]; 	
  double meanGaus3      = meanGaus2 + pp[14]; 
  double sigGaus3       = pp[15];  		
  double valGaus3_at_meanGaus1 = gauss_fcn(meanGaus1, normGaus3, meanGaus3, sigGaus3);
  double valGaus3       = gauss_fcn(x, normGaus3, meanGaus3, sigGaus3);

  double normGaus1Right = valGaus1_at_meanGaus1 - (valGaus2_at_meanGaus1 + valGaus3_at_meanGaus1);
  double sigGaus1_right = pp[9]; 
  double valGaus1_right = gauss_fcn(x, normGaus1Right, meanGaus1, sigGaus1_right);

  bool drawGaus1        = pp[21];
  bool drawGaus2        = pp[22];
  bool drawGaus3        = pp[23];

  //if ( drawGaus1 && drawGaus2 && drawGaus3 && x > 0.99 && x < 1.01 ) {
  //  std::cout << "<sum_gaus_fnc>:" << std::endl;
  //  std::cout << " x = " << x << ": g1 = " << valGaus1_right << ", g2 = " << valGaus2 << ", g3 = " << valGaus3 << std::endl;
  //}

  double sum = 0.;
  if ( drawGaus1       ) sum += valGaus1_right;    
  if ( drawGaus2       ) sum += valGaus2;
  if ( drawGaus3       ) sum += valGaus3;    
  if ( normGaus1Right < 0. ) sum /= (1. + normGaus1Right*normGaus1Right);
  return sum;
}






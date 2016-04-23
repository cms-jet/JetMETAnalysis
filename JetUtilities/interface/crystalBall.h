#ifndef CRYSTALBALL_HH
#define CRYSTALBALL_HH

// sum of 3gaus+pol7
double sum_gaus_fnc(double *x, double *par); 
// sum of 3gaus+pol7
double sum_gaus_pol_fnc(double *x, double *par); 
// exp
double exp_fcn(double x, double norm, double slope); 
// gauss
double gauss_fcn(double x, double norm, double mean, double sigma); 
// pol7
double pol_fcn(double x, double p0, double p1=0, double p2=0, double p3=0, double p4=0, double p5=0, double p6=0, double p7=0); 
// global fits
double crystalBall(double *xx, double *pp); 
double crystalBall_1(double *xx, double *pp);


#endif

#include "JetMETAnalysis/JetAnalyzers/interface/crystalBall.h"
#include "JetMETAnalysis/JetAnalyzers/interface/crystalBallRes.h"

#include <TROOT.h>
#include <TSystem.h>
#include <TFile.h>
#include <TKey.h>
#include <TH1F.h>
#include <TF1.h>
#include <TTree.h>
#include "TVirtualFitter.h"
#include "TMath.h"
#include "TSpectrum.h"
#include "TRandom3.h"
#include "TCanvas.h"

#include "TMath.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <math.h>
#include <map>
#include <algorithm>

TH1F *hXaxis( TH1F* hist, double minX, double maxX);

typedef std::vector<std::string> vstring;

using namespace std;

void fitCrystalBall(TH1F*& hrspOrig, string alg, string histName, int polDeg, bool normalized, string fitDir)
{

  //
  // do no write all the histogram in the output file
  //
  TH1::AddDirectory(kFALSE);

  //
  // select a particular algorithm
  //
  //vstring valg;
  //valg.push_back("ak5tauHPSlooseCombDBcorrAll");
  //valg.push_back("ak5tauHPSlooseCombDBcorrOneProng0Pi0");
  //valg.push_back("ak5tauHPSlooseCombDBcorrOneProng1Pi0");
  //valg.push_back("ak5tauHPSlooseCombDBcorrTwoProng0Pi0");
  //valg.push_back("ak5tauHPSlooseCombDBcorrTwoProng1Pi0");
  //valg.push_back("ak5tauHPSlooseCombDBcorrThreeProng0Pi0");
  //valg.push_back("ak5tauHPSlooseCombDBcorrThreeProng1Pi0");
  //valg.push_back("ak5tauHPSlooseCombDBcorrAlll2");
  //valg.push_back("ak5tauHPSlooseCombDBcorrOneProng0Pi0l2");
  //valg.push_back("ak5tauHPSlooseCombDBcorrOneProng1Pi0l2");
  //valg.push_back("ak5tauHPSlooseCombDBcorrTwoProng0Pi0l2");
  //valg.push_back("ak5tauHPSlooseCombDBcorrTwoProng1Pi0l2");
  //valg.push_back("ak5tauHPSlooseCombDBcorrThreeProng0Pi0l2");
  //valg.push_back("ak5tauHPSlooseCombDBcorrThreeProng1Pi0l2");
  //bool findAlg{false};
  //if( find(valg.begin(), valg.end(), alg ) != valg.end() ) findAlg=true;
  //if(!findAlg) return;
  ///////////////////////////////////////

  //
  // current histogram name
  //
  string histname{hrspOrig->GetName()};
  ///////////////////////////////////////

  //
  // select the relative response histograms for an eta range.
  // For time reason, we split the jobs in section of eta 
  //
  if ( histname.find(histName) == string::npos) return;
  ///////////////////////////////////////

  //
  // select an histogram
  //
  //vstring vhistname;
  //vhistname.push_back("RelRsp_JetEta-0.5to-0.3_RefPt20to22.5");
  //bool findHistname{false};
  //if( find(vhistname.begin(), vhistname.end(),  histname) != vhistname.end() ) findHistname=true;
  //if(!findHistname) return;
  ///////////////////////////////////////

  //
  // start fit procedure
  //
  cout << "\nCalpas Veeken crystalBallFit processing "<<alg<<": " << histname << endl;

  // normalized hist to 1
  if(normalized && hrspOrig->Integral("width")>0) hrspOrig->Scale(1/hrspOrig->Integral("width"));
  //cout<<"hrsp integral after scaling: "<<hrspOrig->Integral("width")<<endl;

  TH1F* hrsp=(TH1F*)hrspOrig->Clone(); 

  // to avoid pol divergence at low value, set empty bin to 0.5+/-0.5
  for(int i=1; i<=hrsp->GetNbinsX(); i++) if(hrsp->GetBinContent(i)==0){
    if(normalized){hrsp->SetBinContent(i, 0.005); hrsp->SetBinError(i, 0.005);}
    else if(hrsp->Integral()<300000){hrsp->SetBinContent(i, 0.5);   hrsp->SetBinError(i, 0.5);}
  }

  // find the max of the hist that correspond to the tau pic btw [0, 1.05]   
  double maxBin{0};
  int binmax = -1;
  for ( int i=0; i <= hrsp->FindBin(1.05); ++i ){
    double newMaxBin = hrsp->GetBinContent(i);
    if ( newMaxBin > maxBin ) { 
      maxBin = newMaxBin; binmax = i; 
    }
  } 

  // pre-fit function 
  TF1 *testfit {0}; 
  TF1 *gausLeft{0};
  TF1 *expLeft {0};
  TF1 *polLeft {0};
  TF1 *sumGaus {0};
  // global fit function
  TF1 *crystalBallFit{0};

  // pre-fit definition
  string gaus = "[0]*TMath::Exp(-0.5*TMath::Power((x-[1])/[2],2.))"; 
  string expo = "[0]*TMath::Exp([1]*(x-[2]))";
  string pol;

  // reduce chisquare and Kolmogorov-Smir test
  double chi2, ks;

  // map chi2/KS->fit
  map<double, TH1F*> mapKsFit;  // the best out of the 4 cases
  map<double, TH1F*> mapKsFitB; // best from Blue   (case 4: Gauss1+Gauss2+Gauss3+pol+exp_left)
  map<double, TH1F*> mapKsFitO; // best from Orange (case 3: Gauss1+Gauss2+Gauss3+pol)
  map<double, TH1F*> mapKsFitM; // best from Magenta(case 2: Gauss1+Gaus2+pol)
  map<double, TH1F*> mapKsFitG; // best from Green  (case 1: Gaus1+pol)
  map<double, TH1F*> mapChi2Fit;
  map<double, TH1F*> mapChi2FitB;
  map<double, TH1F*> mapChi2FitO;
  map<double, TH1F*> mapChi2FitM;
  map<double, TH1F*> mapChi2FitG;

  bool goodFit{false}, nofit{false};

  if(polDeg==1){

    TF1 *polRight{0};
    pol = "[0]+[1]*x";

    int maxPar{25};
    vstring parName {"norm G1", "mean G1", "sigma G1 left", 
    		     "!norm Exp left", "slope Exp left", 
     		     "!p0 left", "p1 left", "!R1", "!R2",
     		     "sigma G1 right", "norm G2", "!mean G2", "sigma G2", 
     		     "norm G3", "!mean G3", "sigma G3", "p0 right", "p1 right", "R3", 
     		     "drawPolLeft", "drawExp", "drawG1", "drawG2", "drawG3", "drawPolRight"};

    double xbinmax = hrsp->GetXaxis()->GetBinUpEdge(binmax); // x position of the max bin
    double ybinmax = hrsp->GetBinContent(binmax); 
    int    bindown = binmax-4;
    double minGaus = hrsp->GetXaxis()->GetBinLowEdge(bindown); // x_min position of Gauss1 (=R2)



    //////////////////////////////////////////////////////////////////////////
    // left side Gauss1 pre-fit
    //////////////////////////////////////////////////////////////////////////
    cout<<"\nProcessing fit Gauss1 left..."<< endl;
    testfit = new TF1("testfit", gaus.data(), 0., 2.);
    testfit->SetParameter(0, hrsp->GetBinContent(binmax));
    testfit->FixParameter(0, hrsp->GetBinContent(binmax)); //!!!! was not fixed before!!!!
    testfit->SetParameter(1, xbinmax);
    testfit->FixParameter(1, xbinmax);
    testfit->SetParameter(2, 0.10);
    testfit->SetParLimits(2, 0., 1.e+1); 
    gausLeft = new TF1("gausLeft", gaus.data(), 0., 2.);
    do {
      hrsp->Fit(testfit, "Q0", "", minGaus, xbinmax); // Q: quiet mode; 0: do not draw default canvas
      // change the current hist axis to match the fit hist axis
      TH1F * hrspXaxis = hXaxis(hrsp, minGaus, xbinmax); 
      TF1 *fit=hrsp->GetFunction("testfit");
      // ensure that the current hist and the fit hist have the same number of point 
      fit->SetNpx(hrspXaxis->GetNbinsX());
      // compute Kolmogorov and reduce Chi2 test
      ks = hrspXaxis->KolmogorovTest((TH1D*)fit->GetHistogram()); 
      chi2 = testfit->GetChisquare()/max(1, testfit->GetNDF()); 
      // add a new bin in the fit toward the left for the next fit iteration
      --bindown;
      minGaus = hrsp->GetXaxis()->GetBinLowEdge(bindown);
      if ( chi2 < 5. && ks > 0.01) { // Calpas and Veeken guess
	gausLeft->SetParameter(0, testfit->GetParameter(0));
	gausLeft->SetParameter(1, testfit->GetParameter(1));
	gausLeft->SetParameter(2, testfit->GetParameter(2));	  
      }
    } while ( chi2 < 5. && minGaus > 0. && ks > 0.01 );
    // sometimes chi2 may exceed 5 in 1st interation->set default value
    if(gausLeft->GetParameter(0)==0) gausLeft->SetParameter(0, testfit->GetParameter(0));
    if(gausLeft->GetParameter(1)==0) gausLeft->SetParameter(1, testfit->GetParameter(1));
    if(gausLeft->GetParameter(2)==0) gausLeft->SetParameter(2, testfit->GetParameter(2));
    //std::cout << "prefit for Gaus1: norm = " << testfit->GetParameter(0) << ", mean = " << testfit->GetParameter(1) << ", sigma = " << testfit->GetParameter(2) << std::endl;
    //std::cout << " (minGaus = " << minGaus << ")" << std::endl;

    // go back on bin as it was not take into account
    ++bindown;
    minGaus = hrsp->GetXaxis()->GetBinCenter(bindown);



    //////////////////////////////////////////////////////////////////////////
    // left side expo pre-fit
    //////////////////////////////////////////////////////////////////////////
    cout<<"\nProcessing fit expo left..."<< endl;
    double minExpLeft = hrsp->GetXaxis()->GetBinLowEdge(bindown-3); // x_min expo position (=R1)
    expLeft = new TF1("expLeft", expo.data(), 0., 2.);
    if ( minGaus > 0 && hrsp->Integral(hrsp->FindBin(0), hrsp->FindBin(minGaus)) > 0. ){
      delete testfit;
      testfit = new TF1("testfit", expo.data(), 0., 2.);
      testfit->SetParameter(0, gausLeft->Eval(minGaus));
      testfit->SetParameter(1, 0.1);
      testfit->SetParameter(2, minGaus);
      testfit->FixParameter(2, minGaus);
      do {
	hrsp->Fit(testfit, "Q0", "", minExpLeft, minGaus);
	--bindown;
	minExpLeft = hrsp->GetXaxis()->GetBinCenter(bindown);
	chi2 = testfit->GetChisquare()/max(1, testfit->GetNDF()); 
	if ( chi2 < 5. ) {
	  expLeft->SetParameter(0, testfit->GetParameter(0));
	  expLeft->SetParameter(1, testfit->GetParameter(1));
	  expLeft->SetParameter(2, testfit->GetParameter(2));
	}
      }
      while ( chi2 < 5. && minExpLeft > 0 );
      if(expLeft->GetParameter(0)==0) expLeft->SetParameter(0, testfit->GetParameter(0));
      if(expLeft->GetParameter(1)==0) expLeft->SetParameter(1, testfit->GetParameter(1));
      if(expLeft->GetParameter(2)==0) expLeft->SetParameter(2, testfit->GetParameter(2));

      ++bindown;
      minExpLeft = hrsp->GetXaxis()->GetBinCenter(bindown);
    } 
    else cout<<"no point or and/or range for the expo left fit\n!!";
    
    //std::cout << "prefit for Exp: norm = " << testfit->GetParameter(0) << ", slope = " << testfit->GetParameter(1) << ", offset = " << testfit->GetParameter(2) << std::endl;



    //////////////////////////////////////////////////////////////////////////
    // left side Pol pre-fit
    //////////////////////////////////////////////////////////////////////////
    cout<<"\nProcessing fit pol left...\n";
    if( minExpLeft > 0 && hrsp->Integral(hrsp->FindBin(0), hrsp->FindBin(minExpLeft)) > 0. ){
      polLeft = new TF1("polLeft", pol.c_str(), 0, minExpLeft);
    } 
    else cout<<"no point or and/or range for the pol left fit!!\n"; 
    
    //std::cout << "prefit for Pol(left):" 
    //for(int i=0; i<2; i++) cout<<"p["<<i<<"] = "<< polLeft->GetParameter(i) <<endl;



    //////////////////////////////////////////////////////////////////////////
    // right side sumGauss pre-fit
    //////////////////////////////////////////////////////////////////////////
    cout<<"\nProcessing fit sumGaus right..."<< endl;
    int    binup = binmax+4;
    double maxSumGaus = hrsp->GetXaxis()->GetBinUpEdge(binup); 
    delete testfit;
    testfit = new TF1("sumGaus", crystalBall_1, 0., 2., maxPar);
    testfit->SetParameter(0, 0.70*hrsp->GetBinContent(binmax));
    testfit->SetParameter(1, xbinmax);
    testfit->SetParameter(2, 0.1);
    testfit->SetParameter(3, 0.2*hrsp->GetBinContent(binmax));
    testfit->SetParameter(4, xbinmax+0.1);
    testfit->SetParameter(5, 0.15);
    testfit->SetParameter(6, 0.1*hrsp->GetBinContent(binmax));
    testfit->SetParameter(7, xbinmax+0.2);
    testfit->SetParameter(8, 0.15);
    sumGaus = new TF1("sumGaus", crystalBall_1, 0., 2., maxPar);
    do {
      hrsp->Fit(testfit, "Q0", "", xbinmax, maxSumGaus);
      ++binup;
      maxSumGaus = hrsp->GetXaxis()->GetBinCenter(binup);
      chi2 = testfit->GetChisquare()/max(1, testfit->GetNDF()); 
      if ( chi2 < 5. ) for(int i=9; i<16; i++) sumGaus->SetParameter(i, testfit->GetParameter(i));
    }
    while ( chi2 < 5. && maxSumGaus < 2. );
    for(int i=9; i<16; i++) if(sumGaus->GetParameter(i)==0) sumGaus->SetParameter(i, testfit->GetParameter(i-9));
    --binup;
    maxSumGaus = hrsp->GetXaxis()->GetBinCenter(binup);



    //////////////////////////////////////////////////////////////////////////
    // Right side Pol pre-fit
    //////////////////////////////////////////////////////////////////////////
    cout<<"\nProcessing fit pol right...\n";
    if( maxSumGaus < 2 && hrsp->Integral(hrsp->FindBin(maxSumGaus), hrsp->FindBin(2)) > 0. ){
      polRight = new TF1("polRight", pol.c_str(), maxSumGaus, 2);
    } 
    else cout<<"no point or and/or range for the pol right fit!!\n"; 
    //std::cout << "prefit for Pol(right):" 
    //for(int i=0; i<2; i++) cout<<"p["<<i<<"] = "<< polRight->GetParameter(i) <<endl;


    //
    // initalise the global fit
    //
    double mu1=gausLeft->GetParameter(1); 
    double mu2=0.15; 
    double mu3=0.15; 
    double r1=minGaus-minExpLeft; 
    double r2=xbinmax-minGaus; 
    //double r3=maxSumGaus-xbinmax;
    double r3=maxSumGaus;
    //cout<<"maxsumGaus: "<<maxSumGaus<<endl;
    //cout<<"xbinmax: "<<xbinmax<<endl;
    cout<<"r3 prefit: "<<r3<<endl;

    //
    // ideal values to be used with flag to fixed or unfixed them
    //
    vdouble rd_unfix {mu1, mu2, mu3, r1, r2, r3, 0., 0., 0., 0., 0., 0.};
    vdouble rd_fixG1 {mu1, mu2, mu3, r1, r2, r3, 1., 0., 0., 0., 0., 0.};
    vdouble rd_fixG2 {mu1, mu2, mu3, r1, r2, r3, 0 , 1., 0., 0., 0., 0.};
    vdouble rd_fixG3 {mu1, mu2, mu3, r1, r2, r3, 0., 0., 1., 0., 0., 0.};
    vdouble rd_fixR1 {mu1, mu2, mu3, r1, r2, r3, 0., 0., 0., 1., 0., 0.};
    vdouble rd_fixR2 {mu1, mu2, mu3, r1, r2, r3, 0., 0., 0., 0., 1., 0.};
    vdouble rd_fixR3 {mu1, mu2, mu3, r1, r2, r3, 0., 0., 0., 0., 0., 1.};
    vector<vdouble> mrd {rd_unfix, rd_fixG1, rd_fixG2, rd_fixG3, rd_fixR1, rd_fixR2, rd_fixR3}; //7*12

    //
    // try ideal + Random value and after 100 try, select the best fit
    //
    TH1F* hrspClone;

    TRandom *random = new TRandom3();
    cout<<"\nStart Random...\n";
    for(int i=0; i<300; i++){
    //for(int i=1; i<2; i++){
      cout<<"\ntest number: "<<i<<"/299"<<endl;

      crystalBallFit = new TF1("crystalBallFit", crystalBall_1, 0., 2., maxPar);
      crystalBallFit->SetLineWidth(2);
      for(int par=0; par<maxPar; par++){ 
	crystalBallFit->SetParameter(par, 0);
	crystalBallFit->SetParName(par, parName[par].c_str()); 
      }



      ////////////////
      // fit case 4
      ////////////////
      crystalBallFit->SetLineColor(kBlue); 

      if(gausLeft){
	double stepG1{0.1*random->Rndm()}; // btw [0, 1]
	double stepR2{0.1*random->Rndm()};

	if(i<7){ // "optimal value = 6 1st vector of the matrix"
	  if(mrd[i][6]==0.) crystalBallFit->SetParameter(1, mrd[i][0]); // mean
	  else crystalBallFit->FixParameter(1, mrd[i][0]); 
	  if(mrd[i][10]==0.) crystalBallFit->SetParameter(8, mrd[i][4]); // R2
	  else crystalBallFit->FixParameter(8, mrd[i][4]);     
	}
	//else if(i>=7 && i<53){ // random up from optimal value
	else if(i>=7 && i<146){ // random up from optimal value
	  if(mrd[0][6]==0.) crystalBallFit->SetParameter(1, mrd[0][0]+stepG1); //mean
	  else crystalBallFit->FixParameter(1, mrd[0][0]+stepG1); 
	  if(mrd[0][10]==0.) crystalBallFit->SetParameter(8, mrd[0][4]+stepR2); // R2
	  else crystalBallFit->FixParameter(8, mrd[0][4]+stepR2);     
	}
	else{ // random down from optimal value
	  if(mrd[0][6]==0.) crystalBallFit->SetParameter(1, mrd[0][0]-stepG1); //mean
	  else crystalBallFit->FixParameter(1, mrd[0][0]-stepG1); 
	  if(mrd[0][10]==0.) crystalBallFit->SetParameter(8, mrd[0][4]-stepR2); // R2
	  else crystalBallFit->FixParameter(8, mrd[0][4]-stepR2);     
	}
	crystalBallFit->SetParameter(0, gausLeft->GetParameter(0)); // norm
	//crystalBallFit->SetParLimits(0, 0.5*ybinmax, ybinmax*2); // was not set!!
	crystalBallFit->SetParLimits(0, 0., 1000); // was not set!!
	crystalBallFit->SetParLimits(1, 0., xbinmax);
	crystalBallFit->SetParameter(2, gausLeft->GetParameter(2)); // sigma Gauss1 left
	//crystalBallFit->SetParLimits(2, 0.02, 10.);
	crystalBallFit->SetParLimits(2, 0.02, 1.);
	crystalBallFit->SetParLimits(8, 0., xbinmax); // R2
      }else{
	for(int i=0; i<9; i++) crystalBallFit->FixParameter(i, 0); 
      }	
      if(expLeft){  
	double stepR1{0.1*random->Rndm()};
	if(i<7){ // "optimal value"
	  if(mrd[i][9]==0.) crystalBallFit->SetParameter(7, mrd[i][3]); // R1
	  else crystalBallFit->FixParameter(7, mrd[i][3]); 
	}
	else if (i>=7 && i<53){ // random up from optimal value
	  if(mrd[0][9]==0.) crystalBallFit->SetParameter(7, mrd[0][3]+stepR1); // R1
	  else crystalBallFit->FixParameter(7, mrd[0][3]+stepR1); 
	}
	else{ // random down from optimal value
	  if(mrd[0][9]==0.) crystalBallFit->SetParameter(7, mrd[0][3]-stepR1); // R1
	  else crystalBallFit->FixParameter(7, mrd[0][3]-stepR1); 
	}
	crystalBallFit->FixParameter(3, 0.); // norm
	crystalBallFit->SetParameter(4, expLeft->GetParameter(1)); // slope
	//crystalBallFit->SetParLimits(4, 0., 1.e+4); 
	crystalBallFit->SetParLimits(4, 0.01, 20); 
	crystalBallFit->SetParLimits(7, 0., xbinmax);
      }else{
	for(int i=3; i<9; i++) crystalBallFit->FixParameter(i, 0); 
      }	
      if(polLeft){
	crystalBallFit->FixParameter(5, 0);  // P0 left
	crystalBallFit->SetParameter(6, polLeft ->GetParameter(1));
      }else{
	for(int i=5; i<8; i++) crystalBallFit->FixParameter(i, 0); 
      }
      //
      if(polRight){
	crystalBallFit->FixParameter(16, 0);  // P0 right
	crystalBallFit->SetParameter(17, polRight ->GetParameter(1));
      }else{
	for(int i=16; i<19; i++) crystalBallFit->FixParameter(i, 0); 
      }

      double stepG2{0.1*random->Rndm()};
      double stepG3{0.1*random->Rndm()};
      double stepR3{0.1*random->Rndm()};

      //if(i!=269) continue;//!!!!!!!!!! to select only the problematic 

      if(i<7){ // "optimal value"
	if(mrd[i][7]==0.) crystalBallFit->SetParameter(11, mrd[i][1]); // Gauss2 mean
	else crystalBallFit->FixParameter(11, mrd[i][1]); 
	if(mrd[i][8]==0.) crystalBallFit->SetParameter(14, mrd[i][2]); // Gauss3 mean
	else crystalBallFit->FixParameter(14, mrd[i][2]);
	if(mrd[i][11]==0.) crystalBallFit->SetParameter(18, mrd[i][5]); // R3
	else crystalBallFit->FixParameter(18, mrd[i][5]);
      }
      //else if(i>=7 && i<53){ // random up from optimal value
      else if(i>=7 && i<146){ // random up from optimal value
	if(mrd[0][7]==0.) crystalBallFit->SetParameter(11, mrd[0][1]+stepG2); 
	else crystalBallFit->FixParameter(11, mrd[0][1]+stepG2); 
	if(mrd[0][8]==0.) crystalBallFit->SetParameter(14, mrd[0][2]+stepG3);
	else crystalBallFit->FixParameter(14, mrd[0][2]+stepG3);
	if(mrd[0][11]==0.) crystalBallFit->SetParameter(18, mrd[0][5]+stepR3);
	else crystalBallFit->FixParameter(18, mrd[0][5]+stepR3);
      }
      else{ // random down from optimal value
	if(mrd[0][7]==0.) crystalBallFit->SetParameter(11, mrd[0][1]-stepG2); 
	else crystalBallFit->FixParameter(11, mrd[0][1]+stepG2); 
	if(mrd[0][8]==0.) crystalBallFit->SetParameter(14, mrd[0][2]-stepG3);
	else crystalBallFit->FixParameter(14, mrd[0][2]+stepG3);
	if(mrd[0][11]==0.) crystalBallFit->SetParameter(18, mrd[0][5]-stepR3);
	else crystalBallFit->FixParameter(18, mrd[0][5]-stepR3);
      }
      crystalBallFit->SetParameter(9, 0.05);  // sigma Gauss 1 right
      //crystalBallFit->SetParLimits(9, 0.01, 10);  
      crystalBallFit->SetParLimits(9, 0.01, 1);  
      crystalBallFit->SetParameter(10, 0.5*ybinmax); // Gauss 2 norm
      //crystalBallFit->SetParLimits(10, 0., ybinmax); 
      crystalBallFit->SetParLimits(10, 0., 1000); 
      crystalBallFit->SetParLimits(11, 0., 2.); 
      crystalBallFit->SetParameter(12, 0.2);  // Gauss 2 sigma
      //crystalBallFit->SetParLimits(12, 0., 10.);  
      crystalBallFit->SetParLimits(12, 0., 1.);  
      crystalBallFit->SetParameter(13, 0.25*ybinmax); // gauss 3 norm
      //crystalBallFit->SetParLimits(13, 0., ybinmax);  
      crystalBallFit->SetParLimits(13, 0., 1000);  
      crystalBallFit->SetParLimits(14, 0., 2.); 
      crystalBallFit->SetParameter(15, 0.1); // Gauss 3 sigma
      //crystalBallFit->SetParLimits(15, 0., 10.);  
      crystalBallFit->SetParLimits(15, 0., 1.);  
      // bool parameter to draw each function separatly
      crystalBallFit->FixParameter(19, 1); // draw pol left
      crystalBallFit->FixParameter(20, 1); // draw exp
      crystalBallFit->FixParameter(21, 1); // draw g1
      crystalBallFit->FixParameter(22, 1); // draw g2
      crystalBallFit->FixParameter(23, 1); // draw g3
      crystalBallFit->FixParameter(24, 1); // draw pol right
      // 
      hrsp->Fit("crystalBallFit", "R"); 
      testfit = (TF1*) hrsp->GetListOfFunctions()->Last();
      testfit->SetNpx(hrsp->GetNbinsX());
      ks=hrsp->KolmogorovTest(testfit->GetHistogram()); 
      chi2=testfit->GetChisquare()/max(1, testfit->GetNDF()); 
      cout<<"ks/chi2 case4: "<<ks<<"/"<<chi2<<endl<<endl;
      //
      hrspClone = (TH1F*) hrsp->Clone(); 
      if( ks>1e-4 && fabs(ks-1)>0.1 && testfit->Eval(1)>0) mapKsFitB[ks]=hrspClone; // 0 and 1 are bad ks values 1e-7!!
      if(chi2<50 && testfit->Eval(1)>0) mapChi2FitB[chi2]=hrspClone;

      
      ////////////////
      // fit case 3
      ////////////////
      crystalBallFit->SetLineColor(kOrange); 
      crystalBallFit->FixParameter(6, 0); // pol7/range left=cst p0
      hrsp->Fit("crystalBallFit", "R"); 
      testfit = (TF1*) hrsp->GetListOfFunctions()->Last();
      testfit->SetNpx(hrsp->GetNbinsX());
      ks=hrsp->KolmogorovTest(testfit->GetHistogram()); 
      chi2=testfit->GetChisquare()/max(1, testfit->GetNDF()); 
      cout<<"ks/chi2 case3: "<<ks<<"/"<<chi2<<endl<<endl;
      //
      hrspClone = (TH1F*) hrsp->Clone(); 
      if( ks>1e-4 && fabs(ks-1)>0.1 && testfit->Eval(1)>0) mapKsFitO[ks]=hrspClone; 
      if(chi2<50 && testfit->Eval(1)>0) mapChi2FitO[chi2]=hrspClone;
      


      ////////////////
      // fit case 2
      ////////////////
      crystalBallFit->SetLineColor(kMagenta); 
      for(int i=13; i<15; i++) crystalBallFit->FixParameter(i, 0); // gaus3 right
      crystalBallFit->FixParameter(15, 1); // sigma !=0 
      hrsp->Fit("crystalBallFit", "R"); 
      testfit = (TF1*) hrsp->GetListOfFunctions()->Last();
      testfit->SetNpx(hrsp->GetNbinsX());
      ks=hrsp->KolmogorovTest(testfit->GetHistogram()); 
      chi2=testfit->GetChisquare()/max(1, testfit->GetNDF()); 
      cout<<"ks/chi2 case2 "<<ks<<"/"<<chi2<<endl<<endl;
      //
      hrspClone = (TH1F*) hrsp->Clone(); 
      if(ks>1e-4 && fabs(ks-1)>0.1 && testfit->Eval(1)>0) mapKsFitM[ks]=hrspClone; 
      if(chi2<50 && testfit->Eval(1)>0) mapChi2FitM[chi2]=hrspClone;



      ////////////////
      // fit case 1
      ////////////////
      crystalBallFit->SetLineColor(kGreen); 
      for(int i=10; i<12; i++) crystalBallFit->FixParameter(i, 0); // gaus2 right
      crystalBallFit->FixParameter(12, 1); // sigma !=0 
      hrsp->Fit("crystalBallFit", "R"); 
      testfit = (TF1*) hrsp->GetListOfFunctions()->Last();
      testfit->SetNpx(hrsp->GetNbinsX());
      ks=hrsp->KolmogorovTest(testfit->GetHistogram()); 
      chi2=testfit->GetChisquare()/max(1, testfit->GetNDF()); 
      cout<<"ks/chi2 case1 "<<ks<<"/"<<chi2<<endl<<endl;
      //
      hrspClone = (TH1F*) hrsp->Clone(); 
      if( ks>1e-4 && fabs(ks-1)>0.1 && testfit->Eval(1)>0) mapKsFitG[ks]=hrspClone; 
      if(chi2<50 && testfit->Eval(1)>0) mapChi2FitG[chi2]=hrspClone;
    } // random
    cout<<"endRandom\n";
    cout<<"\nretriving best fit from ks or chi2 map..."<<endl;

    if(mapKsFitB.size()!=0)mapKsFit[mapKsFitB.rbegin()->first]=mapKsFitB.rbegin()->second; 
    if(mapKsFitO.size()!=0)mapKsFit[mapKsFitO.rbegin()->first]=mapKsFitO.rbegin()->second; 
    if(mapKsFitM.size()!=0)mapKsFit[mapKsFitM.rbegin()->first]=mapKsFitM.rbegin()->second; 
    if(mapKsFitG.size()!=0)mapKsFit[mapKsFitG.rbegin()->first]=mapKsFitG.rbegin()->second; 
    //
    if(mapChi2FitB.size()!=0)mapChi2Fit[mapChi2FitB.begin()->first]=mapChi2FitB.begin()->second; 
    if(mapChi2FitO.size()!=0)mapChi2Fit[mapChi2FitO.begin()->first]=mapChi2FitO.begin()->second; 
    if(mapChi2FitM.size()!=0)mapChi2Fit[mapChi2FitM.begin()->first]=mapChi2FitM.begin()->second; 
    if(mapChi2FitG.size()!=0)mapChi2Fit[mapChi2FitG.begin()->first]=mapChi2FitG.begin()->second; 

    // select best fit from ks then chi2, if none best failed ks then chi2, if none keep original hist
    bool selKs{false}, selChi2{false};

    if(mapKsFit.size()!=0 || mapChi2Fit.size()!=0){
      if(mapChi2Fit.size()!=0){
	double minChi2 {mapChi2Fit.begin()->first}; 
	if(minChi2<5){
	  goodFit=true; hrspOrig= (TH1F*) mapChi2Fit.begin()->second->Clone();
	  cout<<"pass Chi2: "<<mapChi2Fit.begin()->first<<endl;
	  selChi2=true;
	}
      }
      else if(mapKsFit.size()!=0){
	double maxKs {mapKsFit.rbegin()->first};
	if(maxKs>0.5){
	  goodFit=true; hrspOrig= (TH1F*) mapKsFit.rbegin()->second->Clone();
	  cout<<"pass Ks: "<<mapKsFit.rbegin()->first<<endl;
	  selKs=true;
	}
      }
      if(mapChi2Fit.size()!=0 && !selChi2 && !selKs){
	goodFit=false; hrspOrig= (TH1F*) mapChi2Fit.begin()->second->Clone();
	cout<<"failed Chi2: "<<mapChi2Fit.begin()->first<<endl;
      }
      else if(mapKsFit.size()!=0 && !selKs && !selChi2){
	goodFit=false; hrspOrig= (TH1F*) mapKsFit.rbegin()->second->Clone();
	cout<<"failed Ks: "<<mapKsFit.rbegin()->first<<endl;
      }
    }
    else{ goodFit=false; nofit=true; cout<<"no fit at all, keeping original histogram!!\n"; }

    hrspOrig->Write();
  }
  else{

    pol = "[0]+[1]*x+[2]*TMath::Power(x,2)+[3]*TMath::Power(x,3)+"
  	  "[4]*TMath::Power(x,4)+[5]*TMath::Power(x,5)+"
	  "[6]*TMath::Power(x,6)+[7]*TMath::Power(x,7)";

    // global fit maximun parameters and name
    int maxPar{36};
    vstring parName {"norm G1", "mean G1", "sigma G1 left", "!norm Exp left", "slope Exp left", 
      "!p0 left", "p1 left", "p2 left", "p3 left", "p4 left", "p5 left", "p6 left", "p7 left", 
      "!R1", "!R2", "sigma G1 right", "norm G2", "!mean G2", "sigma G2", "norm G3", "!mean G3", "sigma G3",
      "p0 right", "p1 right", "p2 right", "p3 right", "p4 right", "p5 right", "p6 right", "p7 right", 
      "drawPolLeft", "drawExp", "drawG1", "drawG2", "drawG3", "drawPolRight"};


    double xbinmax = hrsp->GetXaxis()->GetBinUpEdge(binmax); // x position of the max bin
    double ybinmax = hrsp->GetBinContent(binmax); 
    int    bindown = binmax-4;
    double minGaus = hrsp->GetXaxis()->GetBinLowEdge(bindown); // x_min position of Gauss1 (=R2)


    //////////////////////////////////////////////////////////////////////////
    // left side Gauss1 pre-fit
    //////////////////////////////////////////////////////////////////////////
    cout<<"\nProcessing fit Gauss1 left..."<< endl;
    testfit = new TF1("testfit", gaus.data(), 0., 2.);
    testfit->SetParameter(0, hrsp->GetBinContent(binmax));
    testfit->FixParameter(0, hrsp->GetBinContent(binmax)); //!!!! was not fixed before!!!!
    testfit->SetParameter(1, xbinmax);
    testfit->FixParameter(1, xbinmax);
    testfit->SetParameter(2, 0.10);
    testfit->SetParLimits(2, 0., 1.e+1); 
    gausLeft = new TF1("gausLeft", gaus.data(), 0., 2.);
    do {
      hrsp->Fit(testfit, "Q0", "", minGaus, xbinmax); // Q: quiet mode; 0: do not draw default canvas
      // change the current hist axis to match the fit hist axis
      TH1F * hrspXaxis = hXaxis(hrsp, minGaus, xbinmax); 
      TF1 *fit=hrsp->GetFunction("testfit");
      // ensure that the current hist and the fit hist have the same number of point 
      fit->SetNpx(hrspXaxis->GetNbinsX());
      // compute Kolmogorov and reduce Chi2 test
      ks = hrspXaxis->KolmogorovTest((TH1D*)fit->GetHistogram()); 
      chi2 = testfit->GetChisquare()/max(1, testfit->GetNDF()); 
      // add a new bin in the fit toward the left for the next fit iteration
      --bindown;
      minGaus = hrsp->GetXaxis()->GetBinLowEdge(bindown);
      if ( chi2 < 5. && ks > 0.01) { // Calpas and Veeken guess
	gausLeft->SetParameter(0, testfit->GetParameter(0));
	gausLeft->SetParameter(1, testfit->GetParameter(1));
	gausLeft->SetParameter(2, testfit->GetParameter(2));	  
      }
    } while ( chi2 < 5. && minGaus > 0. && ks > 0.01 );
    // sometimes chi2 may exceed 5 in 1st interation->set default value
    if(gausLeft->GetParameter(0)==0) gausLeft->SetParameter(0, testfit->GetParameter(0));
    if(gausLeft->GetParameter(1)==0) gausLeft->SetParameter(1, testfit->GetParameter(1));
    if(gausLeft->GetParameter(2)==0) gausLeft->SetParameter(2, testfit->GetParameter(2));
    //std::cout << "prefit for Gaus1: norm = " << testfit->GetParameter(0) << ", mean = " << testfit->GetParameter(1) << ", sigma = " << testfit->GetParameter(2) << std::endl;
    //std::cout << " (minGaus = " << minGaus << ")" << std::endl;

    // go back on bin as it was not take into account
    ++bindown;
    minGaus = hrsp->GetXaxis()->GetBinCenter(bindown);


    //////////////////////////////////////////////////////////////////////////
    // left side expo pre-fit
    //////////////////////////////////////////////////////////////////////////
    cout<<"\nProcessing fit expo left..."<< endl;
    double minExpLeft = hrsp->GetXaxis()->GetBinLowEdge(bindown-3); // x_min expo position (=R1)
    expLeft = new TF1("expLeft", expo.data(), 0., 2.);
    if ( minGaus > 0 && hrsp->Integral(hrsp->FindBin(0), hrsp->FindBin(minGaus)) > 0. ){
      delete testfit;
      testfit = new TF1("testfit", expo.data(), 0., 2.);
      testfit->SetParameter(0, gausLeft->Eval(minGaus));
      testfit->SetParameter(1, 0.1);
      testfit->SetParameter(2, minGaus);
      testfit->FixParameter(2, minGaus);
      do {
	hrsp->Fit(testfit, "Q0", "", minExpLeft, minGaus);
	--bindown;
	minExpLeft = hrsp->GetXaxis()->GetBinCenter(bindown);
	chi2 = testfit->GetChisquare()/max(1, testfit->GetNDF()); 
	if ( chi2 < 5. ) {
	  expLeft->SetParameter(0, testfit->GetParameter(0));
	  expLeft->SetParameter(1, testfit->GetParameter(1));
	  expLeft->SetParameter(2, testfit->GetParameter(2));
	}
      }
      while ( chi2 < 5. && minExpLeft > 0 );
      if(expLeft->GetParameter(0)==0) expLeft->SetParameter(0, testfit->GetParameter(0));
      if(expLeft->GetParameter(1)==0) expLeft->SetParameter(1, testfit->GetParameter(1));
      if(expLeft->GetParameter(2)==0) expLeft->SetParameter(2, testfit->GetParameter(2));

      ++bindown;
      minExpLeft = hrsp->GetXaxis()->GetBinCenter(bindown);
    } 
    else {
      cout<<"no point or and/or range for the expo left fit\n!!";
    }
    //std::cout << "prefit for Exp: norm = " << testfit->GetParameter(0) << ", slope = " << testfit->GetParameter(1) << ", offset = " << testfit->GetParameter(2) << std::endl;


    //////////////////////////////////////////////////////////////////////////
    // left side Pol pre-fit
    //////////////////////////////////////////////////////////////////////////
    cout<<"\nProcessing fit pol left...\n";
    if( minExpLeft > 0 && hrsp->Integral(hrsp->FindBin(0), hrsp->FindBin(minExpLeft)) > 0. ){
      polLeft = new TF1("polLeft", pol.c_str(), 0, minExpLeft);
    } 
    else { 
      cout<<"no point or and/or range for the pol7 left fit!!\n"; 
    }
    //std::cout << "prefit for Pol(left):" 
    //if(pol1) for(int i=0; i<8; i++) cout<<"p["<<i<<"] = "<< polLeft->GetParameter(i) <<endl;
    //else         for(int i=0; i<2; i++) cout<<"p["<<i<<"] = "<< polLeft->GetParameter(i) <<endl;


    //
    // initalise the global fit
    //
    double mu1=gausLeft->GetParameter(1); 
    double mu2=0.15; 
    double mu3=0.15; 
    double r1=minGaus-minExpLeft; 
    double r2=xbinmax-minGaus; 

    //
    // ideal values to be used with flag to fixed or unfixed them
    //
    vdouble rd_unfix {mu1, mu2, mu3, r1, r2, 0., 0., 0., 0., 0.};
    vdouble rd_fixG1 {mu1, mu2, mu3, r1, r2, 1., 0., 0., 0., 0.};
    vdouble rd_fixG2 {mu1, mu2, mu3, r1, r2, 0., 1., 0., 0., 0.};
    vdouble rd_fixG3 {mu1, mu2, mu3, r1, r2, 0., 0., 1., 0., 0.};
    vdouble rd_fixR1 {mu1, mu2, mu3, r1, r2, 0., 0., 0., 1., 0.};
    vdouble rd_fixR2 {mu1, mu2, mu3, r1, r2, 0., 0., 0., 0., 1.};
    vector<vdouble> mrd {rd_unfix, rd_fixG1, rd_fixG2, rd_fixG3, rd_fixR1, rd_fixR2};

    //
    // try ideal + Random value and after 100 try, select the best fit
    //
    TH1F* hrspClone;

    TRandom *random = new TRandom3();
    cout<<"\nStart Random...\n";
    for(int i=0; i<100; i++){
      cout<<"\ntest number: "<<i<<"/99"<<endl;

      crystalBallFit = new TF1("crystalBallFit", crystalBall, 0., 2., maxPar);
      crystalBallFit->SetLineWidth(2);
      for(int par=0; par<maxPar; par++){ 
	crystalBallFit->SetParameter(par, 0);
	crystalBallFit->SetParName(par, parName[par].c_str()); 
      }

      ////////////////
      // fit case 4
      ////////////////
      crystalBallFit->SetLineColor(kBlue); 

      if(gausLeft){
	double stepG1{0.1*random->Rndm()};
	double stepR2{0.1*random->Rndm()};

	crystalBallFit->SetParameter(0, gausLeft->GetParameter(0)); // norm
	//crystalBallFit->SetParLimits(0, 0., ybinmax*2); // was not set!!
	if(i<6){ // "optimal value = 6 1st vector of the matrix"
	  if(mrd[i][5]==0.) { crystalBallFit->SetParameter(1, mrd[i][0]); crystalBallFit->SetParLimits(1, 0, xbinmax);} // mean
	  else crystalBallFit->FixParameter(1, mrd[i][0]); 
	  if(mrd[i][9]==0.) crystalBallFit->SetParameter(14, mrd[i][4]); // R2
	  else crystalBallFit->FixParameter(14, mrd[i][4]);     
	}
	else if(i>=6 && i<47){ // random up from optimal value
	  if(mrd[0][5]==0.) { crystalBallFit->SetParameter(1, mrd[0][0]+stepG1); crystalBallFit->SetParLimits(1, 0, xbinmax);} //mean
	  else crystalBallFit->FixParameter(1, mrd[0][0]+stepG1); 
	  if(mrd[0][9]==0.) crystalBallFit->SetParameter(14, mrd[0][4]+stepR2); // R2
	  else crystalBallFit->FixParameter(14, mrd[0][4]+stepR2);     
	}
	else{ // random down from optimal value
	  if(mrd[0][5]==0.) { crystalBallFit->SetParameter(1, mrd[0][0]-stepG1); crystalBallFit->SetParLimits(1, 0, xbinmax);} //mean
	  else crystalBallFit->FixParameter(1, mrd[0][0]-stepG1); 
	  if(mrd[0][9]==0.) crystalBallFit->SetParameter(14, mrd[0][4]-stepR2); // R2
	  else crystalBallFit->FixParameter(14, mrd[0][4]-stepR2);     
	}
	crystalBallFit->SetParameter(2, gausLeft->GetParameter(2)); // sigma Gauss1 left
	//crystalBallFit->SetParLimits(2, 0.02, 10.);
	crystalBallFit->SetParLimits(2, 0.02, 1.);
	crystalBallFit->SetParLimits(14, 0., xbinmax);
      }else{
	for(int i=0; i<15; i++) crystalBallFit->FixParameter(i, 0); 
      }	
      if(expLeft){  
	double stepR1{0.1*random->Rndm()};
	crystalBallFit->FixParameter(3, 0.); // norm
	crystalBallFit->SetParameter(4, expLeft->GetParameter(1)); // slope
	//crystalBallFit->SetParLimits(4, 0., 1.e+4); 
	crystalBallFit->SetParLimits(4, 0.01, 20); 
	if(i<6){ // "optimal value"
	  if(mrd[i][8]==0.) crystalBallFit->SetParameter(13, mrd[i][3]); // R1
	  else crystalBallFit->FixParameter(13, mrd[i][3]); 
	}
	else if (i>=6 && i<47){ // random up from optimal value
	  if(mrd[0][8]==0.) crystalBallFit->SetParameter(13, mrd[0][3]+stepR1); // R1
	  else crystalBallFit->FixParameter(13, mrd[0][3]+stepR1); 
	}
	else{ // random down from optimal value
	  if(mrd[0][8]==0.) crystalBallFit->SetParameter(13, mrd[0][3]-stepR1); // R1
	  else crystalBallFit->FixParameter(13, mrd[0][3]-stepR1); 
	}
	crystalBallFit->SetParLimits(13, 0., 1.);
      }else{
	for(int i=3; i<15; i++) crystalBallFit->FixParameter(i, 0); 
      }	
      if(polLeft){
	for(int i=1; i<8; i++)  crystalBallFit->SetParameter(i+5, polLeft ->GetParameter(i)); 
	crystalBallFit->FixParameter(5, 0);  // P0 left
	for(int i=0; i<7; i++) crystalBallFit->SetParLimits(i+6, -100, 100 ); //!!!!??
      }else{
	for(int i=5; i<=13; i++) crystalBallFit->FixParameter(i, 0); 
      }
      //	
      double stepG2{0.1*random->Rndm()};
      double stepG3{0.1*random->Rndm()};

      //if(i!=96) continue;//!!!!!!!!!! to select only the problematic 

      if(i<6){ // "optimal value"
	if(mrd[i][6]==0.) crystalBallFit->SetParameter(17, mrd[i][1]); // Gauss2 mean
	else crystalBallFit->FixParameter(17, mrd[i][1]); 
	if(mrd[i][7]==0.) crystalBallFit->SetParameter(20, mrd[i][2]); // Gauss3 mean
	else crystalBallFit->FixParameter(20, mrd[i][2]);
      }
      else if(i>=6 && i<47){ // random up from optimal value
	if(mrd[0][6]==0.) crystalBallFit->SetParameter(17, mrd[0][1]+stepG2); 
	else crystalBallFit->FixParameter(17, mrd[0][1]+stepG2); 
	if(mrd[0][7]==0.) crystalBallFit->SetParameter(20, mrd[0][2]+stepG3);
	else crystalBallFit->FixParameter(20, mrd[0][2]+stepG3);
      }
      else{ // random down from optimal value
	if(mrd[0][6]==0.) crystalBallFit->SetParameter(17, mrd[0][1]-stepG2); 
	else crystalBallFit->FixParameter(17, mrd[0][1]+stepG2); 
	if(mrd[0][7]==0.) crystalBallFit->SetParameter(20, mrd[0][2]-stepG3);
	else crystalBallFit->FixParameter(20, mrd[0][2]+stepG3);
      }
      crystalBallFit->SetParameter(15, 0.05);  // sigma Gauss 1 right
      //crystalBallFit->SetParLimits(15, 0.01, 10);  
      crystalBallFit->SetParLimits(15, 0.01, 1);  
      crystalBallFit->SetParameter(16, 0.01*ybinmax); // Gauss 2 norm
      crystalBallFit->SetParLimits(16, 0., ybinmax); 
      crystalBallFit->SetParLimits(17, 0., 2.); 
      crystalBallFit->SetParameter(18, 0.2);  // Gauss 2 sigma
      //crystalBallFit->SetParLimits(18, 0., 10.);  
      crystalBallFit->SetParLimits(18, 0., 1.);  
      crystalBallFit->SetParameter(19, 0.10*ybinmax); // gauss 3 norm
      crystalBallFit->SetParLimits(19, 0., ybinmax);  
      crystalBallFit->SetParLimits(20, 0., 2.); 
      crystalBallFit->SetParameter(21, 0.1); // Gauss 3 sigma
      //crystalBallFit->SetParLimits(21, 0., 10.);  
      crystalBallFit->SetParLimits(21, 0., 1.);  
      crystalBallFit->SetParameter(22, 0.5);   // P0 right
      crystalBallFit->SetParLimits(22, 0., ybinmax);  
      for(int i=0; i<8; i++) crystalBallFit->SetParLimits(i+22, -100, 100 ); //!!!!??
      if(polDeg==1){
	crystalBallFit->SetParameter(23, -0.1);
	crystalBallFit->SetParLimits(23, -1, 0);
	for(int i=0; i<6; i++) crystalBallFit->FixParameter(i+24, 0);
      }
      // bool parameter to draw each function separatly
      crystalBallFit->FixParameter(30, 1); // draw pol left
      crystalBallFit->FixParameter(31, 1); // draw exp
      crystalBallFit->FixParameter(32, 1); // draw g1
      crystalBallFit->FixParameter(33, 1); // draw g2
      crystalBallFit->FixParameter(34, 1); // draw g3
      crystalBallFit->FixParameter(35, 1); // draw pol right
      // 
      hrsp->Fit("crystalBallFit", "R"); 
      testfit = (TF1*) hrsp->GetListOfFunctions()->Last();
      testfit->SetNpx(hrsp->GetNbinsX());
      ks=hrsp->KolmogorovTest(testfit->GetHistogram()); 
      chi2=testfit->GetChisquare()/max(1, testfit->GetNDF()); 
      cout<<"ks/chi2 case4: "<<ks<<"/"<<chi2<<endl<<endl;
      //
      hrspClone = (TH1F*) hrsp->Clone(); 
      if( ks>1e-4 && fabs(ks-1)>0.1 && testfit->Eval(1)>0) mapKsFitB[ks]=hrspClone; // 0 and 1 are bad ks values 1e-7!!
      if(chi2<50 && testfit->Eval(1)>0) mapChi2FitB[chi2]=hrspClone;


      ////////////////
      // fit case 3
      ////////////////
      crystalBallFit->SetLineColor(kOrange); 
      if(polLeft)for(int i=6; i<=12; i++) crystalBallFit->FixParameter(i, 0); // pol7/range left=cst p0
      for(int i=23; i<=29; i++) crystalBallFit->FixParameter(i, 0); // pol7 right= cst p0 
      hrsp->Fit("crystalBallFit", "R"); 
      testfit = (TF1*) hrsp->GetListOfFunctions()->Last();
      testfit->SetNpx(hrsp->GetNbinsX());
      ks=hrsp->KolmogorovTest(testfit->GetHistogram()); 
      chi2=testfit->GetChisquare()/max(1, testfit->GetNDF()); 
      cout<<"ks/chi2 case3: "<<ks<<"/"<<chi2<<endl<<endl;
      //
      hrspClone = (TH1F*) hrsp->Clone(); 
      if( ks>1e-4 && fabs(ks-1)>0.1 && testfit->Eval(1)>0) mapKsFitO[ks]=hrspClone; 
      if(chi2<50 && testfit->Eval(1)>0) mapChi2FitO[chi2]=hrspClone;



      ////////////////
      // fit case 2
      ////////////////
      crystalBallFit->SetLineColor(kMagenta); 
      for(int i=19; i<=20; i++) crystalBallFit->FixParameter(i, 0); // gaus3 right
      crystalBallFit->FixParameter(21, 1); // sigma !=0 
      hrsp->Fit("crystalBallFit", "R"); 
      testfit = (TF1*) hrsp->GetListOfFunctions()->Last();
      testfit->SetNpx(hrsp->GetNbinsX());
      ks=hrsp->KolmogorovTest(testfit->GetHistogram()); 
      chi2=testfit->GetChisquare()/max(1, testfit->GetNDF()); 
      cout<<"ks/chi2 case2 "<<ks<<"/"<<chi2<<endl<<endl;
      //
      hrspClone = (TH1F*) hrsp->Clone(); 
      if(ks>1e-4 && fabs(ks-1)>0.1 && testfit->Eval(1)>0) mapKsFitM[ks]=hrspClone; 
      if(chi2<50 && testfit->Eval(1)>0) mapChi2FitM[chi2]=hrspClone;



      ////////////////
      // fit case 1
      ////////////////
      crystalBallFit->SetLineColor(kGreen); 
      for(int i=16; i<=17; i++) crystalBallFit->FixParameter(i, 0); // gaus2 right
      crystalBallFit->FixParameter(18, 1); // sigma !=0 
      hrsp->Fit("crystalBallFit", "R"); 
      testfit = (TF1*) hrsp->GetListOfFunctions()->Last();
      testfit->SetNpx(hrsp->GetNbinsX());
      ks=hrsp->KolmogorovTest(testfit->GetHistogram()); 
      chi2=testfit->GetChisquare()/max(1, testfit->GetNDF()); 
      cout<<"ks/chi2 case1 "<<ks<<"/"<<chi2<<endl<<endl;
      //
      hrspClone = (TH1F*) hrsp->Clone(); 
      if( ks>1e-4 && fabs(ks-1)>0.1 && testfit->Eval(1)>0) mapKsFitG[ks]=hrspClone; 
      if(chi2<50 && testfit->Eval(1)>0) mapChi2FitG[chi2]=hrspClone;

    } // random
    cout<<"endRandom\n";
    cout<<"\nretriving best fit from ks or chi2 map..."<<endl;

    if(mapKsFitB.size()!=0)mapKsFit[mapKsFitB.rbegin()->first]=mapKsFitB.rbegin()->second; 
    if(mapKsFitO.size()!=0)mapKsFit[mapKsFitO.rbegin()->first]=mapKsFitO.rbegin()->second; 
    if(mapKsFitM.size()!=0)mapKsFit[mapKsFitM.rbegin()->first]=mapKsFitM.rbegin()->second; 
    if(mapKsFitG.size()!=0)mapKsFit[mapKsFitG.rbegin()->first]=mapKsFitG.rbegin()->second; 
    //
    if(mapChi2FitB.size()!=0)mapChi2Fit[mapChi2FitB.begin()->first]=mapChi2FitB.begin()->second; 
    if(mapChi2FitO.size()!=0)mapChi2Fit[mapChi2FitO.begin()->first]=mapChi2FitO.begin()->second; 
    if(mapChi2FitM.size()!=0)mapChi2Fit[mapChi2FitM.begin()->first]=mapChi2FitM.begin()->second; 
    if(mapChi2FitG.size()!=0)mapChi2Fit[mapChi2FitG.begin()->first]=mapChi2FitG.begin()->second; 

    // select best fit from ks then chi2, if none best failed ks then chi2, if none keep original hist
    bool selKs{false}, selChi2{false};

    if(mapKsFit.size()!=0 || mapChi2Fit.size()!=0){
      if(mapKsFit.size()!=0){
	double maxKs {mapKsFit.rbegin()->first};
	if(maxKs>0.5){
	  goodFit=true; hrspOrig= (TH1F*) mapKsFit.rbegin()->second->Clone();
	  cout<<"pass Ks: "<<mapKsFit.rbegin()->first<<endl;
	  selKs=true;
	}
      }
      else if(mapChi2Fit.size()!=0){
	double minChi2 {mapChi2Fit.begin()->first}; 
	if(minChi2<5){
	  goodFit=true; hrspOrig= (TH1F*) mapChi2Fit.begin()->second->Clone();
	  cout<<"pass Chi2: "<<mapChi2Fit.begin()->first<<endl;
	  selChi2=true;
	}
      }
      if(!selKs && !selChi2){
	if(mapKsFit.size()!=0){
	  goodFit=false; hrspOrig= (TH1F*) mapKsFit.rbegin()->second->Clone();
	  cout<<"failed Ks: "<<mapKsFit.rbegin()->first<<endl;
	}
	else if(mapChi2Fit.size()!=0) {
	  goodFit=false; hrspOrig= (TH1F*) mapChi2Fit.begin()->second->Clone();
	  cout<<"failed Chi2: "<<mapChi2Fit.begin()->first<<endl;
	}
      }
    }
    else{ goodFit=false; nofit=true; cout<<"no fit at all, keeping original histogram!!\n"; }

    hrspOrig->Write();

  } // else polDeg=7

  //
  // save fit plot
  //
  TCanvas *canvas = new TCanvas("canvas", "", 700, 700);
  canvas->Divide(1,2);
  canvas->cd(1);
  TH1F *hrspOrigClone=(TH1F*)hrspOrig->Clone();
  hrspOrigClone->GetXaxis()->SetRangeUser(0.8, 1.2);
  hrspOrigClone->Draw();
  canvas->cd(2); 
  gPad->SetLogy();
  hrspOrig->Draw();

  //
  // print CB fit result if fit exist
  // 
  if(!nofit){
    cout<<"\nretrieving parameters from the selected fit... "<<endl;

    vector<double> vintegral(3), vnorm(3), vrange(3), vmean(3), vsigma(4), vexp(2), vpolLeft(8), vpolRight(8);
    vector<int>    voption(6);

    crystalBallRes(hrspOrig, true, vintegral, vnorm, vrange, vmean, vsigma, vexp, vpolLeft, vpolRight, voption, polDeg, canvas); // hist fit, drawfit, intg2, g3, g3, canvas(defaut)

  }

  cout<<"saving fit plot...\n";
  string savePath, dir, hrspOrigName{hrspOrig->GetName()};
  if(goodFit) dir="pass"; else dir="failed";
  savePath=fitDir+"/"+alg+"/crystalBallFit/"+dir+"/"+hrspOrigName+".png";

  canvas->Print((savePath).c_str()); 
}


TH1F *hXaxis( TH1F* hist, double minX, double maxX)
{
  double *xbins = new double [hist->GetNbinsX()+1];
  int nbins{0};
  for(int i=0; i<hist->GetNbinsX(); i++){
    double x {hist->GetXaxis()->GetBinCenter(i+1)};
    if(x>=minX && x<=maxX){
      xbins[nbins]  = hist->GetXaxis()->GetBinLowEdge(i+1);
      xbins[nbins+1]= hist->GetXaxis()->GetBinUpEdge(i+1);
      ++nbins;
    }
  }

  TH1F *hnew = new TH1F ("hnew", "", nbins, xbins);
  for(int i=0; i<nbins+1; i++){
    hnew->SetBinContent(i+1, hist->GetBinContent(hist->FindBin(hnew->GetBinCenter(i+1))));
    hnew->SetBinError  (i+1, hist->GetBinError  (hist->FindBin(hnew->GetBinCenter(i+1))));
  }
  return hnew;
}





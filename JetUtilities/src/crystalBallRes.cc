#include "JetMETAnalysis/JetUtilities/interface/crystalBall.h"
#include "JetMETAnalysis/JetUtilities/interface/crystalBallRes.h"

#include <TH1.h>
#include <TF1.h>
#include <TCanvas.h>

#include <iostream>
#include <vector>


using namespace std;

void crystalBallRes( TH1 *hrsp, bool drawCrystalBall, vdouble &vintegral, vdouble &vnorm, 
   		     vdouble &vrange, vdouble &vmean, vdouble &vsigma,
  	             vdouble &vexp, vdouble &vpolLeft, vdouble &vpolRight, 
  	             vint &voption, int polDeg, TCanvas *canvas)
{
  //
  // retrieve the fit
  //
  TF1 *crystalBallFit=(TF1*)hrsp->GetListOfFunctions()->Last();
  if(!crystalBallFit) {cout<<"crystalBallRes: pointor to CB fit failed!!"; return;}

  //
  // print fit parameters
  //
  cout<<"\n"<<crystalBallFit->GetNpar()<<" fit parameters:\n";
  for(int par=0; par<crystalBallFit->GetNpar(); par++) cout<<crystalBallFit->GetParName(par)<<": "<<crystalBallFit->GetParameter(par)<<endl;

  //
  // set fit result in vectors
  //
  if(polDeg==7){
    cout<<"\nfit results (pol7 was used): "<<endl;
    vmean[0]  = crystalBallFit->GetParameter(1);      // mean gauss1
    vrange[1] = vmean[0]-0.5*crystalBallFit->GetParameter(2)-crystalBallFit->GetParameter(14); // min gauss1
    vrange[0] = vrange[1]-crystalBallFit->GetParameter(13);  // min exp
    vmean[1]  = vmean[0]+crystalBallFit->GetParameter(17); // mean gauss2
    vmean[2]  = vmean[1]+crystalBallFit->GetParameter(20); // mean gauss3
    //
    vsigma[0] = crystalBallFit->GetParameter(2);  // sigma gauss1 right
    vsigma[1] = crystalBallFit->GetParameter(15); // sigma gauss1 left
    vsigma[2] = crystalBallFit->GetParameter(18); // sigma gauss2
    vsigma[3] = crystalBallFit->GetParameter(21); // sigma gauss3
    //
    vnorm[0] = crystalBallFit->GetParameter(0);   // norm gauss1
    vnorm[1] = crystalBallFit->GetParameter(16);  // norm gauss2
    vnorm[2] = crystalBallFit->GetParameter(19);  // norm gauss3
    //
    vexp[0] = crystalBallFit->GetParameter(3);    // norm exp
    vexp[1] = crystalBallFit->GetParameter(4);    // slope exp
    //
    for(int i=0; i<8; i++) vpolLeft[i]  = crystalBallFit->GetParameter(i+5);
    //
    for(int i=0; i<8; i++) vpolRight[i] = crystalBallFit->GetParameter(i+22);
    //
    for(int i=0; i<6; i++) voption[i]   = crystalBallFit->GetParameter(i+30);

    // will draw fit in different color for each range if option set
    vbool sum  { 1, 1, 1, 1, 1, 1 };
    vbool polL { 1, 0, 0, 0, 0, 0 };
    vbool exp  { 0, 1, 0, 0, 0, 0 };
    vbool g1   { 0, 0, 1, 0, 0, 0 };
    vbool g2   { 0, 0, 0, 1, 0, 0 };
    vbool g3   { 0, 0, 0, 0, 1, 0 };
    vbool polR { 0, 0, 0, 0, 0, 1 };
    vector<vbool> drawOpt{sum, polL, exp, g1, g2, g3, polR};

    unsigned long nfnc{drawOpt.size()};

    TF1 *fnc[nfnc];

    for(unsigned  i=0; i<nfnc; i++){
      fnc[i] = new TF1("fnc", crystalBall, 0., 2., crystalBallFit->GetNpar());

      for( int par=0; par<crystalBallFit->GetNpar(); par++ ){
	fnc[i]->SetParName  (par, crystalBallFit->GetParName(par)); 
	fnc[i]->SetParameter(par, crystalBallFit->GetParameter(par)); 
      }
      for(unsigned j=0; j<drawOpt[i].size(); j++) fnc[i]->SetParameter(j+30, drawOpt[i][j]); 

      //if(i==0) fnc[i]->SetLineColor(kBlack);  // sum
      if(i==1) fnc[i]->SetLineColor(kGray+2); // polL
      if(i==2) fnc[i]->SetLineColor(kCyan);   // exp
      if(i==3){fnc[i]->SetLineColor(kRed);      vintegral[0]=fnc[i]->Integral(0,2)/hrsp->GetBinWidth(1);} // g1 
      if(i==4){fnc[i]->SetLineColor(kMagenta);  vintegral[1]=fnc[i]->Integral(0,2)/hrsp->GetBinWidth(1);} // g2
      if(i==5){fnc[i]->SetLineColor(kOrange+4); vintegral[2]=fnc[i]->Integral(0,2)/hrsp->GetBinWidth(1);} // g3
      if(i==6) fnc[i]->SetLineColor(kGray+2); // polR
      if(drawCrystalBall && canvas!=0){ canvas->cd(2); fnc[i]->Draw("same"); }
    }


    //
    // print fit info
    //
    cout<<"\norder after fit: "<<endl;
    cout<<"0 < r1: "<<vrange[0]<<" < r2: "<<vrange[1]<<" < mu1: "<<vmean[0]<<" < mu2: "<<vmean[1]<<" < mu3: "<<vmean[2]<<" < 2"<<endl;

    cout<<"\nclosest mean to 1: "<<endl;
    cout<<"|mu1-1|: "<<fabs(vmean[0]-1)<<", |mu2-1|: "<<fabs(vmean[1]-1)<<", |mu3-1|: "<<fabs(vmean[2]-1)<<endl;

    cout<<"\nclosest mean to histpeak: "<<endl;
    double histPeak{hrsp->GetXaxis()->GetBinCenter(hrsp->GetMaximumBin())};		 
    cout<<"|mu1-peak|: "<<fabs(vmean[0]-histPeak)<<", |mu2-peak|: "<<fabs(vmean[1]-histPeak)<<", |mu3-peak|: "<<fabs(vmean[2]-histPeak)<<endl;

    cout<<"\nnorm: "<<endl;
    cout<<"Gauss1: "<<vnorm[0]<< ", Gauss2: "<<vnorm[1]<<", Gauss3: "<<vnorm[2]<<endl;
    double sumNormG{0}; for( auto i : vnorm) sumNormG+=i;
    cout<<"sum: "<<sumNormG<< endl;
    cout<<"25%: "<< 0.25*sumNormG<<endl;

    cout<<"\nintegral:"<<endl;
    cout<<"Gauss1: "<<vintegral[0]<< ", Gauss2: "<<vintegral[1]<<", Gauss3: "<<vintegral[2]<<endl;
    double sumIntG{0}; for( auto i : vintegral) sumIntG+=i;
    cout<<"sum: "<<sumIntG<< endl;
    cout<<"25%: "<< 0.25*sumIntG<<endl<<endl;

  }
  else{
    cout<<"\nfit results (pol1 was used): "<<endl;
    vmean[0]  = crystalBallFit->GetParameter(1);      // mean gauss1
    vrange[1] = vmean[0]-0.5*crystalBallFit->GetParameter(2)-crystalBallFit->GetParameter(8); // min gauss1
    vrange[0] = vrange[1]-crystalBallFit->GetParameter(7); // min exp
    vrange[2] = crystalBallFit->GetParameter(18); // maxSumGaus
    vmean[1]  = vmean[0]+crystalBallFit->GetParameter(11); // mean gauss2
    vmean[2]  = vmean[1]+crystalBallFit->GetParameter(14); // mean gauss3
    //
    vsigma[0] = crystalBallFit->GetParameter(2);  // sigma gauss1 left
    vsigma[1] = crystalBallFit->GetParameter(9);  // sigma gauss1 right
    vsigma[2] = crystalBallFit->GetParameter(12); // sigma gauss2
    vsigma[3] = crystalBallFit->GetParameter(15); // sigma gauss3
    //
    vnorm[0] = crystalBallFit->GetParameter(0);   // norm gauss1
    vnorm[1] = crystalBallFit->GetParameter(10);  // norm gauss2
    vnorm[2] = crystalBallFit->GetParameter(13);  // norm gauss3
    //
    vexp[0] = crystalBallFit->GetParameter(3);    // norm exp
    vexp[1] = crystalBallFit->GetParameter(4);    // slope exp
    //
    for(int i=0; i<2; i++) vpolLeft[i]  = crystalBallFit->GetParameter(i+5);
    //
    for(int i=0; i<2; i++) vpolRight[i] = crystalBallFit->GetParameter(i+16);
    //
    for(int i=0; i<6; i++) voption[i]   = crystalBallFit->GetParameter(i+19);

    cout<<"\nfit integral           [0, 2]    : "<<crystalBallFit->Integral(0., 2.)              <<endl;
    cout<<"\nfit integral pol left  [0, r1]   : "<<crystalBallFit->Integral(0., vrange[0])       <<endl;
    cout<<"\nfit integral exp       [r1, r2]  : "<<crystalBallFit->Integral(vrange[0], vrange[1])<<endl;
    cout<<"\nfit integral gaus      [r2, mg1] : "<<crystalBallFit->Integral(vrange[1], vmean[0]) <<endl;
    cout<<"\nfit integral sumgaus   [mg1, r3] : "<<crystalBallFit->Integral(vmean[0], vrange[2]) <<endl;
    cout<<"\nfit integral pol right [r3, 2]   : "<<crystalBallFit->Integral(vrange[2], 2.)       <<endl;

    // will draw fit in different color for each range if option set
    vbool sum  { 1, 1, 1, 1, 1, 1 };
    vbool polL { 1, 0, 0, 0, 0, 0 };
    vbool exp  { 0, 1, 0, 0, 0, 0 };
    vbool g1   { 0, 0, 1, 0, 0, 0 };
    vbool g2   { 0, 0, 0, 1, 0, 0 };
    vbool g3   { 0, 0, 0, 0, 1, 0 };
    vbool polR { 0, 0, 0, 0, 0, 1 };
    vector<vbool> drawOpt{sum, polL, exp, g1, g2, g3, polR};

    unsigned long nfnc{drawOpt.size()};

    TF1 *fnc[nfnc];

    for(unsigned  i=0; i<nfnc; i++){
      fnc[i] = new TF1("fnc", crystalBall_1, 0., 2., crystalBallFit->GetNpar());

      for( int par=0; par<crystalBallFit->GetNpar(); par++ ){
	fnc[i]->SetParName  (par, crystalBallFit->GetParName(par)); 
	fnc[i]->SetParameter(par, crystalBallFit->GetParameter(par)); 
      }
      for(unsigned j=0; j<drawOpt[i].size(); j++) fnc[i]->SetParameter(j+19, drawOpt[i][j]); 

      if(i==0) fnc[i]->SetLineColor(kBlack);  // sum
      if(i==1) fnc[i]->SetLineColor(kGray+2); // polL
      if(i==2) fnc[i]->SetLineColor(kCyan);   // exp
      if(i==3){fnc[i]->SetLineColor(kRed);      vintegral[0]=fnc[i]->Integral(0,2)/hrsp->GetBinWidth(1);} // g1 
      if(i==4){fnc[i]->SetLineColor(kMagenta);  vintegral[1]=fnc[i]->Integral(0,2)/hrsp->GetBinWidth(1);} // g2
      if(i==5){fnc[i]->SetLineColor(kOrange+4); vintegral[2]=fnc[i]->Integral(0,2)/hrsp->GetBinWidth(1);} // g3
      if(i==6) fnc[i]->SetLineColor(kGray+2); // polR
      if(drawCrystalBall && canvas!=0){ canvas->cd(2); fnc[i]->Draw("same"); }
    }


    //
    // print fit info
    //
    cout<<"\norder after fit: "<<endl;
    cout<<"0 < r1: "<<vrange[0]<<" < r2: "<<vrange[1]<<" < mu1: "<<vmean[0]<<" < mu2: "<<vmean[1]<<" < mu3: "<<vmean[2]<<" < r3: "<<vrange[2]<<" < 2"<<endl;

    cout<<"\nclosest mean to 1: "<<endl;
    cout<<"|mu1-1|: "<<fabs(vmean[0]-1)<<", |mu2-1|: "<<fabs(vmean[1]-1)<<", |mu3-1|: "<<fabs(vmean[2]-1)<<endl;

    cout<<"\nclosest mean to histpeak: "<<endl;
    double histPeak{hrsp->GetXaxis()->GetBinCenter(hrsp->GetMaximumBin())};		 
    cout<<"|mu1-peak|: "<<fabs(vmean[0]-histPeak)<<", |mu2-peak|: "<<fabs(vmean[1]-histPeak)<<", |mu3-peak|: "<<fabs(vmean[2]-histPeak)<<endl;

    cout<<"\nnorm: "<<endl;
    cout<<"Gauss1: "<<vnorm[0]<< ", Gauss2: "<<vnorm[1]<<", Gauss3: "<<vnorm[2]<<endl;
    double sumNormG{0}; for( auto i : vnorm) sumNormG+=i;
    cout<<"sum: "<<sumNormG<< endl;
    cout<<"25%: "<< 0.25*sumNormG<<endl;

    cout<<"\nintegral:"<<endl;
    cout<<"Gauss1: "<<vintegral[0]<< ", Gauss2: "<<vintegral[1]<<", Gauss3: "<<vintegral[2]<<endl;
    double sumIntG{0}; for( auto i : vintegral) sumIntG+=i;
    cout<<"sum: "<<sumIntG<< endl;
    cout<<"25%: "<< 0.25*sumIntG<<endl<<endl;
  }
}

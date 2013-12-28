#include <map>
#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>

#include "TFile.h"
#include "TChain.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TProfile3D.h"
#include "TGraph2DErrors.h"
#include "TSystem.h"
#include "TF2.h"
#include "TH2.h"
#include "TCutG.h"
#include "TMath.h"
#include "TH1.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TCanvas.h"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetAnalyzers/interface/REStyle.h"


using namespace std;

void ExtractGraph(TString calgo1="ak5pf",TString calgo2="ak5pf")
{
	TString algo1(calgo1);
	TString algo2(calgo2);
	TString algo12 = algo1+"_"+algo2;
	if (algo1.EqualTo(algo2)) 
		algo12 = algo1;
   TString inputFilename = "output_"+algo12+".root";
   TFile *fin= new TFile(inputFilename);
	TProfile3D *prof = (TProfile3D*)fin->Get("p_offOverA_etaVsTnpusVsJetPt");
	TProfile3D *profPt = (TProfile3D*)fin->Get("p_PtAve_etaVsTnpusVsJetPt");
	TProfile3D *profRho = (TProfile3D*)fin->Get("p_RhoAve_etaVsTnpusVsJetPt");
	TString outputFilename = "p_offOverA_EtaVsRhoVsJetPt_"+algo12+".root";
   TFile *fout = new TFile (outputFilename,"RECREATE");
   fout->cd();

   const int NPtBins    = 30;//37
   const int NETA = 82;
   const int NRHO = 50;
	TGraph2DErrors *ptemp;
	char buff[400];
	char bufftitle[400];
	for (int iEta = 1;iEta<=NETA;iEta++)
	{
		sprintf(buff,"p_offOverA_RhoVsJetPt_%d",iEta);
		sprintf(bufftitle,"p_offOverA_RhoVsJetPt_%d;Rho;p_{T}^{pu};OffsetOverArea",iEta);
		double x[NRHO*NPtBins],xe[NRHO*NPtBins],y[NRHO*NPtBins],ye[NRHO*NPtBins],z[NRHO*NPtBins],ze[NRHO*NPtBins];
		int n_count = 0;
      for (int i = 1;i<=prof->GetYaxis()->GetNbins();i++)
      {
      	for (int j=1;j<=prof->GetZaxis()->GetNbins();j++)
      	{
      		//if ((profPt->GetBinContent(iEta,i,j)!=0) && (prof->GetBinContent(iEta,i,j)>0) && (prof->GetBinError(iEta,i,j)>0.000001) && ! TMath::IsNaN(prof->GetBinError(iEta,i,j)))
        		if ((profPt->GetBinContent(iEta,i,j)!=0) && (prof->GetBinContent(iEta,i,j)>0) && (prof->GetBinError(iEta,i,j)>0.000001))
      		{
        			x[n_count]=profRho->GetBinContent(iEta,i,j);
        			xe[n_count]=profRho->GetBinError(iEta,i,j);
      			y[n_count]=profPt->GetBinContent(iEta,i,j);
      			ye[n_count]=profPt->GetBinError(iEta,i,j);
         		z[n_count]=prof->GetBinContent(iEta,i,j);
         		ze[n_count]=prof->GetBinError(iEta,i,j);
   				n_count++;
				}
      	}
      }
      ptemp = new TGraph2DErrors(n_count,x,y,z,xe,ye,ze);
      ptemp->SetNameTitle(buff,bufftitle);
      ptemp->Write();
      delete ptemp;
	}
	fout->Close();
	delete fout;
}

void Fit2Doffset(TString calgo1="ak5pf",TString calgo2="ak5pf")
{
	//const int NPtBins    = 30;//37
   //const double vpt[NPtBins + 1] = {10,10.5,11,11.5,12,12.5,13,13.5,14,15,17,20,23,27,30,35,40,45,57,72,90,120,150,200,300,400,550,750,1000,1500,2000};//,2500,3000,3500,4000};//,4500,5000,10000};
   const int NETA = 82;
   const double veta[NETA+1] = {-5.191,-4.889,-4.716,-4.538,-4.363,-4.191,-4.013,-3.839,-3.664,-3.489,
   -3.314, -3.139,-2.964,-2.853,-2.65, -2.5,  -2.322,-2.172,-2.043,-1.93,
   -1.83,  -1.74, -1.653,-1.566,-1.479,-1.392,-1.305,-1.218,-1.131,-1.044,
   -0.957, -0.879,-0.783,-0.696,-0.609,-0.522,-0.435,-0.348,-0.261,-0.174,
   -0.087, 0,     0.087, 0.174, 0.261, 0.348, 0.435 ,0.522, 0.609, 0.696,
   0.783,  0.879, 0.957, 1.044, 1.131, 1.218, 1.305 ,1.392, 1.479, 1.566,
   1.653,  1.74,  1.83,  1.93,  2.043, 2.172, 2.322 ,2.5,   2.65,  2.853,
   2.964,  3.139, 3.314, 3.489, 3.664, 3.839, 4.013 ,4.191, 4.363, 4.538,
   4.716,4.889,5.191};
	const unsigned int NPARS = 4;
	TString algo1(calgo1);
	TString algo2(calgo2);
	TString algo12 = algo1+"_"+algo2;
	if (algo1.EqualTo(algo2)) 
		algo12 = algo1;
	TString inputFilename = "p_offOverA_EtaVsRhoVsJetPt_"+algo12+".root";
  	TFile *fin = new TFile(inputFilename);

	TH1 * aux[NPARS];
	TString cname = "OffsetOverAreaVsRhoVsPt_"+algo12;
	for (unsigned int h=0;h<NPARS;h++){
    TString hname = cname + Form("_Par%i",h);
    TString htitle = cname + Form("_Par%i",h);
    if (h==NPARS-1) htitle = cname + Form("_CHI2NDF");
    TString suffix = Form(";#eta;Par%i;",h);
    if (h==NPARS-1) suffix = ";#eta;#chi^{2}/ndf;";
    aux[h] = new TH1D(hname,htitle+suffix, NETA,veta);
  	}
  	
  	
   TCanvas *cPar = new TCanvas("Par_"+algo12,"Par_"+algo12,400*NPARS,400);
   cPar->cd();
   cPar->Divide(NPARS,1);
   
  	char buff[400];
  	TString outputFilename = "parameters_"+algo12+".txt";
  	ofstream outF(outputFilename.Data());
  	outF <<"{1 JetEta 3 JetPt JetA Rho max(0.0001,1-y*([0]+([1]*z)*(1+[2]*log(x)))/x) Correction L1FastJet}"<<endl;
  	double par0i=0,par1i=0.5,par2i=0;
	for (int iEta =1;iEta<=82;iEta++)
	{
//   	TF2 * f4  = new TF2("f4","[0] + ([1] * x ) *(1 + [2] * log(y))",0,50,0,1800);
   	TF2 * f4  = new TF2("f4","[0] + ([1] * x ) *(1 + [2] * log(y))");
		sprintf(buff,"p_offOverA_RhoVsJetPt_%d",iEta);
   	TGraph2DErrors *prof = (TGraph2DErrors*)fin->Get(buff);
   	f4->SetParameter(0,par0i);
   	f4->SetParameter(1,par1i);
   	f4->SetParameter(2,par2i);
   	prof->Fit("f4","0QM");
   	cout <<std::setw(12)<<iEta<<std::setw(12)<<f4->GetChisquare()<<std::setw(12)<<f4->GetNDF()<<std::setw(12)<<f4->GetChisquare()/f4->GetNDF()<<endl;
   	double par0 = f4->GetParameter(0);
   	double par1 = f4->GetParameter(1);
   	double par2 = f4->GetParameter(2);
   	par0i = par0; par1i = par1; par2i = par2;
   	outF<<std::setw(11)<<veta[iEta-1]<<std::setw(11)<<veta[iEta]<<std::setw(11)<<9<<std::setw(12)<<1<<std::setw(12)<<3500<<std::setw(12)<<0<<std::setw(12)<<10<<std::setw(12)<<0<<std::setw(12)<<200<<std::setw(13)<<par0<<std::setw(13)<<par1<<std::setw(13)<<par2<<std::endl;//<<std::setw(13)<<par3<<std::endl;//<<std::setw(13)<<par4<<std::setw(13)<<par5<<std::setw(13)<<par6<<std::setw(13)<<par7<<std::setw(13)<<par8<<std::setw(13)<<par9<<std::setw(13)<<par10<<endl;
   	for (unsigned int p=0;p<NPARS-1;p++){
      aux[p]->SetBinContent(iEta,f4->GetParameter(p));
      aux[p]->SetBinError(iEta,f4->GetParError(p));
    }
        aux[NPARS-1]->SetBinContent(iEta, f4->GetChisquare()/f4->GetNDF());
        aux[NPARS-1]->SetBinError(iEta, 0);
	}
	  for (int unsigned h=0;h<NPARS;h++){
    cPar->cd(h+1);
    if (h!=NPARS) aux[h]->Draw("E");
    else aux[h]->Draw("P");
  }
	cPar->SaveAs("Parameter_"+algo12+".pdf");
	outF.close();
}



int main(int argc,char**argv)
{
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;
  string         algo1     = cl.getValue<string>  ("algo1",   "ak5pf");
  string         algo2     = cl.getValue<string>  ("algo2",   "ak5pf");

  setREStyle();
  ExtractGraph(algo1,algo2);
  Fit2Doffset(algo1,algo2);
}

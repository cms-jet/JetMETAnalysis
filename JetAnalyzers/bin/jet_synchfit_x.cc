#include <map>
#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include <assert.h>

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

struct FitRes {
      double etalowedge;
      double etaupedge;
      TF2 * fit;
};


//===========================================================================
// This method returns the graph from all the TProfile3D's
TGraph2DErrors * getGraph2D(int iEta, const TProfile3D * prof, 
                            const TProfile3D * profPt, 
                            const TProfile3D * profRho){
   
   // The returning graph, with its name and title
   TGraph2DErrors *graph  = new TGraph2DErrors();
   graph->SetName(Form("p_offOverA_RhoVsJetPt_%d",iEta));
   graph->SetTitle(Form("p_offOverA_RhoVsJetPt_%d;Rho;p_{T}^{pu};OffsetOverArea",iEta));
   int nEvt = 0;
   cout << "\tNBins (rho,pT):  (" << prof->GetYaxis()->GetNbins() << "," << prof->GetZaxis()->GetNbins() << ")" << endl;
   //cout << "NBins rho:  " << prof->GetYaxis()->GetNbins() << endl;
   //cout << "NBins pT:  " << prof->GetZaxis()->GetNbins() << endl; 
   
   for (int irho = 1 ; irho <= prof->GetYaxis()->GetNbins() ; irho ++){      
      for (int irefpt = 1; irefpt <= prof->GetZaxis()->GetNbins() ; irefpt++){
         
         int Gbin = prof->GetBin(iEta, irho, irefpt);
         nEvt += prof->GetBinEntries(Gbin);
         // avoid points with empty content or too small error
         if (prof   ->GetBinError  (iEta,irho,irefpt)  > 0.000001 &&
             (fabs(prof->GetBinError(iEta,irho,irefpt)/prof->GetBinContent(iEta,irho,irefpt)))<0.3 &&
             (fabs(prof->GetBinError(iEta,irho,irefpt)/prof->GetBinContent(iEta,irho,irefpt)))>0.05 &&
             profPt ->GetBinContent(iEta,irho,irefpt)  > 0 &&
             profPt ->GetBinError  (iEta,irho,irefpt)  > 0.1 &&
             profRho->GetBinContent(iEta,irho,irefpt)  > 0 &&
             profRho->GetBinError  (iEta,irho,irefpt)  > 0.1 ) {
            
            // get the relevant values
            double rho  = profRho->GetBinContent(iEta, irho, irefpt); 
            double rhoe = profRho->GetBinError  (iEta, irho, irefpt);
            double pt   = profPt ->GetBinContent(iEta, irho, irefpt);
            double pte  = profPt ->GetBinError  (iEta, irho, irefpt);
            double ooa  = prof   ->GetBinContent(iEta, irho, irefpt);
            double ooae = prof   ->GetBinError  (iEta, irho, irefpt);
            
            // Store the values
            double n = graph->GetN(); 
            graph->SetPoint(n, rho, pt, ooa);
            graph->SetPointError(n, rhoe, pte, ooae);
            
         }//if
         
      } // irefpt
      
   }// irho
   
   cout << "\tgraph has (Entries,Evts):  (" << graph->GetN() << "," << nEvt << ")" << endl;
   //cout<<"\tgraph has "<<graph->GetN()<<" entries"<<"& # of Evts: " << nEvt<< endl;
   
   // return it
   return graph;
   
} // getGraph2D



//===========================================================================
// This method tries to obtain all the relevant TProfiles from the inputFilename
// It returns true if successfull
bool getInputProfiles(TString inputFilename, TProfile3D *& prof, 
                      TProfile3D *& profPt, 
                      TProfile3D *& profRho, bool useNPU){
   
   // Open the input file
   TFile *fin= new TFile(inputFilename);
   if(!fin->IsOpen()) {
      cout << "ERROR jet_synchfit_xx::getInputProfiles() could not open file " 
           <<inputFilename<< endl;
      return false;
   }
   
   // Get the histos
   if(useNPU) {
      prof    = (TProfile3D*) fin->Get("p_offOverA_etaVsNpusVsJetPt"); //offOverA(eta, rho, refpt) 
      profPt  = (TProfile3D*) fin->Get("p_PtAve_etaVsNpusVsJetPt"); // pt(eta, rho, refpt) 
      profRho = (TProfile3D*) fin->Get("p_RhoAve_etaVsNpusVsJetPt");// rho(eta, rho, refpt) 
   
      if (!prof || !profPt  || !profRho) {
         cout<<"ERROR jet_synchfit_xx::getInputProfiles() could not retrieve TProfile3D named "
             <<"either of  p_offOverA_etaVsNpusVsJetPt, p_PtAve_etaVsNpusVsJetPt, "
             <<" or p_RhoAve_etaVsNpusVsJetPt"<<endl;
         return false;
      }
   }
   else {
      prof    = (TProfile3D*) fin->Get("p_offOverA_etaVsTnpusVsJetPt"); //offOverA(eta, rho, refpt) 
      profPt  = (TProfile3D*) fin->Get("p_PtAve_etaVsTnpusVsJetPt"); // pt(eta, rho, refpt) 
      profRho = (TProfile3D*) fin->Get("p_RhoAve_etaVsTnpusVsJetPt");// rho(eta, rho, refpt) 
   
      if (!prof || !profPt  || !profRho) {
         cout<<"ERROR jet_synchfit_xx::getInputProfiles() could not retrieve TProfile3D named "
             <<"either of  p_offOverA_etaVsTnpusVsJetPt, p_PtAve_etaVsTnpusVsJetPt, "
             <<" or p_RhoAve_etaVsTnpusVsJetPt"<<endl;
         return false;
      }
   }
   
   // if everything went well just return true.
   return true;
   
}//getInputProfiles



//===========================================================================
// This method creates a new fit function and fits it to the graph
TF2 * doGraphFitting(TGraph2DErrors * graph, bool highPU, bool logPol, bool PUPPIFunc, int iEta, const TProfile3D * prof){
   
   
   static double par0i = -0.5;
   static double par1i = 0.5;
   static double par2i = 0.1;
   static double par3i = 0.0;
   static double par4i = 1.0;
   if(PUPPIFunc) {
      par0i = 18;
      par1i = -1.5;
      par2i = -0.35;
      par3i = 0.0;
      par4i = 1.0;
   }

   TF2* f4 = 0;
   TString function;
   if(PUPPIFunc)
      function = "[0]+([1]*x)*(1+[2]*log(y))+(1/([3]*log(y)+[4]))";
   else if(logPol)
      //Non-Taylor expanded version
      function = "[0]+([1]*x)*(1+[2]*log(y))";
   else
      //Taylor expanded version
      function = "[0]+([1]*(x-11))*(1+[2]*(log(y)-1.47))";

   cout << "\t(par01,par1i,par2i):  (" << par0i << "," << par1i << "," << par2i << ")" << endl;

   if(highPU)
      f4 = new TF2("f4",function, 0,200,1,3000);
      //f4 = new TF2("f4",function, 5,200,10,3000);
   else
      f4 = new TF2("f4",function, 0,50,1,3000);
      //f4 = new TF2("f4",function, 5,50,10,3000);

   if(PUPPIFunc) {
      f4->SetParameter(0,par0i);
      f4->SetParameter(1,par1i);
      f4->SetParameter(2,par2i);
      f4->SetParameter(3,par3i);
      f4->SetParameter(4,par4i);
      f4->SetParLimits(0,-50,50);
      f4->SetParLimits(1,0,10);
      f4->SetParLimits(2,-2,5);
   }
   else { 
      f4->SetParameter(0,par0i);
      f4->SetParameter(1,par1i);
      f4->SetParameter(2,par2i);
      f4->SetParLimits(0,-5,25);
      f4->SetParLimits(1,0,10);
      f4->SetParLimits(2,-2,5);
   }

   //if (graph->GetN()<500)
   //f4->FixParameter(2,0.05);
   
/*   if(fabs((prof->GetXaxis()->GetBinLowEdge(iEta)+ prof->GetXaxis()->GetBinLowEdge(iEta + 1))/2)>4.2){
      f4->FixParameter(2,0);
      }*/

   int counter=0;
   double rchi2;
   do{
      graph->Fit(f4,"0QMR");
      rchi2 = f4->GetChisquare()/ f4->GetNDF();
      counter++;
      if (rchi2>9 && counter <10) f4->SetParameters(18,-1.5,-0.35);
   } while (rchi2>9 && counter < 10);
   cout <<"N. of fits: " << counter << endl;
   
   cout << "\t(par0,par1,par2):  (" << f4->GetParameter(0) << "," << f4->GetParameter(1) << "," << f4->GetParameter(2) << ")" << endl;
   cout << "\tchi2/NDF=" << rchi2 << endl;
   
   par0i = f4->GetParameter(0);
   par1i = f4->GetParameter(1);
   par2i = f4->GetParameter(2);

   return f4;
   
}//doGraphFitting


//===========================================================================
// This method creates the txt file for the corrections
void createTxtFile(TString txtFilename, const vector<FitRes> & fitResults){
   
   // Create the stream 
   ofstream outF(txtFilename.Data());
   
   // Produce the first line
   //TString fname = Form("{1 JetEta 3 JetPt JetA Rho max(0.0001,1-y*(%s)/x)",fitResults[0].fit->GetTitle());
   TString fname = Form("{1 JetEta 3 Rho JetPt JetA max(0.0001,1-z*(%s)/y)",fitResults[0].fit->GetTitle());
   //outF <<"{1 JetEta 3 JetPt JetA Rho max(0.0001,1-y*([0]+([1]*z)*(1+[2]*log(x)))/x)"
   outF << fname <<" Correction L1FastJet}"<<endl;
   
   // loop over the vector producing the eta lines
   for (unsigned int l=0; l<fitResults.size() ; l++){
      
      // for each fit print this header ...
      outF<<std::setw(11)<<fitResults[l].etalowedge
          <<std::setw(11)<<fitResults[l].etaupedge
         //<<std::setw(11)<<9
          <<std::setw(11)<<(int)(fitResults[l].fit->GetNpar()+6)
         //<<std::setw(12)<<fitResults[l].fit->GetYmin()<<std::setw(12)<<fitResults[l].fit->GetYmax()
         //<<std::setw(12)<<0<<std::setw(12)<<10
         //<<std::setw(12)<<fitResults[l].fit->GetXmin()<<std::setw(12)<<fitResults[l].fit->GetXmax();
          <<std::setw(12)<<fitResults[l].fit->GetXmin()<<std::setw(12)<<fitResults[l].fit->GetXmax()
          <<std::setw(12)<<fitResults[l].fit->GetYmin()<<std::setw(12)<<fitResults[l].fit->GetYmax()
          <<std::setw(12)<<0<<std::setw(12)<<10;
      
      
      // ... followed by the parameters
      for(int p=0; p<fitResults[l].fit->GetNpar(); p++) {
         outF<<std::setw(13)<<fitResults[l].fit->GetParameter(p);
      }
      outF<<std::endl;
      //outF<<std::setw(13)<<fitResults[l].fit->GetParameter(0)
      //    <<std::setw(13)<<fitResults[l].fit->GetParameter(1)
      //    <<std::setw(13)<<fitResults[l].fit->GetParameter(2)<<std::endl;
      
   }//for fit results
   
   // Close the stream
   outF.close();
   
}//createTxtFile



//===========================================================================
void createPDFFile(TString pdfFilename, TString rootFilename, const vector<FitRes> & fitResults){
   
   // The number of parameters in the fit. 
   // Assume all fitResults element have the same number
   unsigned int nfitpars = fitResults[0].fit->GetNpar();
   
   // Create the vectors veta from the histo, no hardcoding here
   double veta[fitResults.size()+1];
   for (unsigned int l=0;l<fitResults.size();l++)
      veta[l] = fitResults[l].etalowedge;
   veta[fitResults.size()] = fitResults[fitResults.size()-1].etaupedge;
   
   // The vector holding all histos
   vector<TH1 *> histos;
   TString cname = "OffsetOverAreaVsRhoVsPt_";
   for (unsigned int h = 0 ; h < nfitpars ; h++){
      TString hname = cname + Form("_Par%i",h);
      histos.push_back(new TH1D(hname,hname+ Form(";#eta;Par%i;",h), 
                                fitResults.size() ,veta));
   }
   // add the chi2/ndf line
   histos.push_back(new TH1D(cname+"_CHI2NDF",cname+"_CHI2NDF"+
                             ";#eta;#chi^{2}/ndf;", fitResults.size() ,veta));
   
   // loop over the fits 
   for (unsigned int l=0; l<fitResults.size() ; l++){
      
      // for each line fill the histos
      for (unsigned int p=0 ; p < nfitpars  ; p++){
         histos[p]->SetBinContent(l+1, fitResults[l].fit->GetParameter(p));
         histos[p]->SetBinError  (l+1, fitResults[l].fit->GetParError(p));
      }
      
      // add the red chi2 info
      double redchi2 =  fitResults[l].fit->GetChisquare()/ fitResults[l].fit->GetNDF();
      histos[nfitpars]->SetBinContent(l+1, redchi2);
      histos[nfitpars]->SetBinError(l+1,2/sqrt(fitResults[l].fit->GetNDF()));
      // for large NDF's the error on the mean of the redchi2 is 2/sqrt(n) ??, not sure...
      
   }// loop over fits
   
   // Create the canvas
   TCanvas *cPar = new TCanvas("Par_","Par_",400*(nfitpars+1),400);
   cPar->cd();
   cPar->Divide(nfitpars+1,1);
   
   // fill the pads
   for (int unsigned h = 0 ; h < histos.size() ; h++){
      cPar->cd(h+1);
      histos[h]->Draw("E");
   }
   
   // Save the Canvas as pdf
   cPar->SaveAs(pdfFilename);
   cPar->SaveAs(rootFilename);
}// createPDFFile(pdfFilename, fitResults);


//===========================================================================
int main(int argc,char**argv){
   
   CommandLine cl;
   if (!cl.parse(argc,argv)) return 0;
   string         aalgo1     = cl.getValue<string>  ("algo1", "ak4pfHLT");
   string         aalgo2     = cl.getValue<string>  ("algo2", "ak4pfHLT");
   bool           highPU     = cl.getValue<bool>    ("highPU",    false);
   bool           logPol     = cl.getValue<bool>    ("logPol",     true);
   bool           useNPU     = cl.getValue<bool>    ("useNPU",    false);
   bool           PUPPIFunc  = cl.getValue<bool>    ("PUPPIFunc", false);

   if (!cl.check()) return 0;
   cl.print();

   TString algo1(aalgo1);
   TString algo2(aalgo2);
   
/*   TString algo1("ak5pf");
     TString algo2("ak5pf");*/
   
   setREStyle();
   
   TString algo12 = algo1+"_"+algo2;
   if (algo1.EqualTo(algo2)) 
      algo12 = algo1;
   
// Open the input file and retrieve all relevant TProfile3D's
   TString inputFilename = "output_"+algo12+".root";
   TProfile3D *prof=0, *profPt=0, *profRho=0;
   if (!getInputProfiles(inputFilename, prof, profPt, profRho, useNPU))
      return 1;
   
// Create the output file to store the graphs
   TString outputFilename = "p_offOverA_EtaVsRhoVsJetPt_"+algo12+".root";
   TFile *fout = new TFile (outputFilename,"RECREATE");
   fout->cd();
   
// The vector to save the results of all fits
   vector<FitRes> fitResults;
   
// Loop over all etas
   for (int iEta = 1; iEta <= prof->GetXaxis()->GetNbins(); iEta++){
      
      // Report to Screen
      cout<< "Analyzing ieta="<<iEta<<" eta="<<prof->GetXaxis()->GetBinCenter(iEta) << endl
          << "****************************" << endl;
      
      // Create the graph 
      TGraph2DErrors *graph  = getGraph2D(iEta, prof, profPt, profRho);
      cout << "Graph for pT, Eta, Rho created successfully" << endl;

      // Needs to be at least 4 entries
      if(graph->GetN()<4){
         cout<<"\t WARNING: Graph has only " << graph->GetN() << " entries. Skipping eta" << endl;
         continue;
      }
      
      // Do the fitting
      TF2 * fitfunc = doGraphFitting(graph, highPU, logPol, PUPPIFunc, iEta, prof);
      cout << "Fitted function" << endl << endl;
      
      // Put this fit result in the vector fitResults
      FitRes fitres;
      fitres.etalowedge = prof->GetXaxis()->GetBinLowEdge(iEta);
      fitres.etaupedge  = prof->GetXaxis()->GetBinUpEdge(iEta);
      fitres.fit        = fitfunc;
      fitResults.push_back(fitres);
      
      // Save the graph to file   
      graph->Write();
   }// eta bins
   
// close the file and clean up
   fout->Close();
   delete fout;
   
// Create the txt file from the fitResults vector
   //TString txtFilename = "parameters_"+algo12+"VR10GPT30"+".txt";
   TString txtFilename = "parameters_"+algo12+".txt";
   createTxtFile(txtFilename, fitResults);
   
// Create the canvas with all parameters vs eta.
//   TString pdfFilename = "Parameter_"+algo12+"VR10GPT30"+".pdf";
   TString pdfFilename = "Parameter_"+algo12+".pdf";
//   TString rootFilename = "Parameter_"+algo12+"VR10GPT30"+".root";
   TString rootFilename = "Parameter_"+algo12+".root";
   createPDFFile(pdfFilename, rootFilename, fitResults);
   
   return 0;
   
} // jet_synchfit_x


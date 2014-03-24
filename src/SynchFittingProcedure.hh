#include "TCanvas.h"
#include "TFile.h"
#include "TProfile2D.h"
#include "TString.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"

#include <iostream>

using namespace std;

// Forward decleration
TCanvas * getCanvasFromFittingProcedure(TString cname , TProfile2D * prof, TString fname);
TH1 * getResolutionHistoFromHisto(TString cname, TString title, TH2 * histo_in);
TH1 * getResolutionHistoFromHisto_v3(TString cname, TString title, TH2 * histo_in);
TH1 * getResolutionHistoFromHisto_v2(TString cname, TString title, TH2 * histo_in, TH2 *off_in);
TH1 * getMeanHistoFromHisto(TString cname, TString title, TH2 *off_in,double & maxy);
TCanvas * getCanvasResponseResolution(TString cname, TString algo, TString title, TH2 * prof[4]);
TCanvas * getCanvasResolution(TString cname, TString algo, TString title, TH2 * prof[6],int modeNo);
TCanvas * getResolutionNumDenom(TString cname, TString ctitle, TString algo, TH2 * prof, TH2 * off);
TCanvas * getGausMeanOffset(TString cname, TString ctitle, TString algo, TH2 * off[6], bool fixedRange);
TCanvas * getGausMeanOffsetWithSum(TString cname, TString ctitle, TString algo, TH2 * off[6], TH2* sum, bool fixedRange);
TCanvas * getGausMeanOffsetOverPtref(TString cname, TString ctitle, TString algo, TH2 * off[6], bool fixedRange);
TCanvas * getGausMeanOffsetScale(TString cname, TString ctitle, TString algo, TH2 * off[6],int scaleNo, bool fixedRange);
TCanvas * getCanvasResolution_v2(TString cname, TString algo, TString title, TH2 * prof[6], TH2 * off[6]);
TCanvas * getCanvasIntegral(TString cname, TString algo, TString title, TProfile * prof[6]);
TH1 * getIntegralHistoFromHisto(TString cname, TString title,TProfile *off_in);
TCanvas * getCanvasAverage(TString cname, TString algo, TString title, TProfile * prof[6]);
TH1 * getAverageHistoFromHisto(TString cname, TString title,TProfile *off_in);


// ------------------------------------------------------------------
TCanvas * getCanvasFromFittingProcedure(TString cname , TProfile2D * prof, TString fname){

   const unsigned int NPARS = 4;

   cout<<"\t Fitting profile named "<<prof->GetName()<<endl;

   // Create the canvas and all it's histos
   TCanvas * c = new TCanvas(cname,cname,1600,400);
   c->cd();
   c->Divide(NPARS,1);
   TH1 * aux[NPARS];
   for (unsigned int h=0;h<NPARS;h++){
      TString hname = cname + Form("_Par%i",h);
      TString htitle = cname + Form("_Par%i",h);
      TString suffix = Form(";#eta;Par%i;",h);
      if (h==NPARS-1) suffix = ";#eta;#chi^{2}/ndf;";
      aux[h] = new TH1D(hname,htitle+suffix, prof->GetXaxis()->GetNbins(), prof->GetXaxis()->GetXbins()->GetArray());
   }

   // open the filename
   TFile * fout = new TFile(fname,"RECREATE");

   // Loop over all eta bins of profile
   for (int eb = 1 ; eb <= prof->GetXaxis()->GetNbins();eb++){

      // create histo of offset vs npv for that eta bin and fit it
      TH1 * hoff = prof->ProjectionY("_py",eb,eb);
      int S_comp_ = cname.CompareTo("ParametersVsGenSumPtOA");	//cout <<S_comp_<<endl;
      int S_comp_2 = cname.CompareTo("ParametersOffOverAVsJetPt");// cout <<cname<<" "<<S_comp_2<<endl;
      TFitResultPtr fr;
      if (! S_comp_) 
      {
         fr = hoff->Fit("pol2","0QS","",50,1800);
      }
      else if (! S_comp_2)
      {
         fr = hoff->Fit("pol2","0QS","",10,2000);
      }
      else
      {
         fr = hoff->Fit("pol2","0QS","",5,35);
      }
    

      // Skip if fit failed
      if (fr->Status()){
         cout<<" Fit FAILED for eta bin="<<eb<<endl;
         delete hoff;
         continue;
      }
      if (fr->NPar()+1 != NPARS){
         cout<<" ERROR getCanvasFromFittingProcedure() NPAR != than fr->NPar()"<<endl;
         return 0;
      }
    
      // plot the results
      for (unsigned int p=0;p<fr->NPar();p++){
         aux[p]->SetBinContent(eb,fr->Parameter(p));
         aux[p]->SetBinError(eb,fr->ParError(p));
      }
      //cout <<eb<<" "<<fr->Chi2()<<" "<<fr->Ndf()<<endl;
      aux[fr->NPar()]->SetBinContent(eb, fr->Chi2()/fr->Ndf());

      // Save the function to file
      TF1 * faux = hoff->GetFunction("pol2");
      if (!faux){
         cout<<" ERROR, getCanvasFromFittingProcedure could not retrieve function"<<endl;
         return 0;
      }
      faux->SetName(Form("pol2_%i",eb));
      faux->Write();

      //clean up
      delete hoff;

   }//for 

   // Close the file
   fout->Write();
   fout->Close();
  
   // Put all histos into the canvas
   aux[0]->GetYaxis()->SetRangeUser(-8,8);
   aux[1]->GetYaxis()->SetRangeUser(-1.5,5);
   aux[2]->GetYaxis()->SetRangeUser(-.05,.05);
   aux[3]->GetYaxis()->SetRangeUser(0,6);
   for (int unsigned h=0;h<NPARS;h++){
      c->cd(h+1);
      aux[h]->Draw("E");
   }

   return c;
  
}//getCanvasFromFittingProcedure


// ------------------------------------------------------------------
// get the canvas from the resolution histo. Return rms/mean
TH1 * getResolutionHistoFromHisto(TString cname, TString title, TH2 * histo_in){

   // make an empty copy to fill and return
   TH1 * histo = histo_in->ProjectionX(cname);
   histo->Reset();
   //histo->Clear();
   histo->GetYaxis()->SetTitle(title);

   // Now loop over the entries of prof and set the histo
   for (int nb = 1 ; nb <= histo->GetXaxis()->GetNbins() ; nb++){

      //double ptref = histo->GetXaxis()->GetBinCenter(nb);
      //double ptreferr =  0.5*(histo->GetXaxis()->GetBinLowEdge(nb)+histo->GetXaxis()->GetBinUpEdge(nb));
      double val = 0;
      double valerr = 0;
    
      TH1 * aux = histo_in->ProjectionY("_py",nb,nb);
      if (aux->GetEntries() > 0) {

         TFitResultPtr fr = aux->Fit("gaus","0qS");
           
         // Skip if fit failed
         if (!fr->Status()){
            double mean    = fr->Parameter(1);
            double meanerr = fr->ParError(1);
            double rms     = fr->Parameter(2);
            double rmserr  = fr->ParError(2);
            val = rms / mean ;
            valerr = val * sqrt( pow(rmserr/rms,2) + pow(meanerr/mean,2));

         }

      }

      histo->SetBinContent(nb,val);
      histo->SetBinError(nb,valerr);

      // clean up
      delete aux;

   }

   // return
   return histo;

}//getResolutionHistoFromHisto


// ------------------------------------------------------------------
// get the canvas from the resolution histo. Return rms
TH1 * getResolutionHistoFromHisto_v3(TString cname, TString title, TH2 * histo_in){

   // make an empty copy to fill and return
   TH1 * histo = histo_in->ProjectionX(cname);
   histo->Reset();
   //histo->Clear();
   histo->GetYaxis()->SetTitle(title);

   // Now loop over the entries of prof and set the histo
   for (int nb = 1 ; nb <= histo->GetXaxis()->GetNbins() ; nb++){

      //double ptref = histo->GetXaxis()->GetBinCenter(nb);
      //double ptreferr =  0.5*(histo->GetXaxis()->GetBinLowEdge(nb)+histo->GetXaxis()->GetBinUpEdge(nb));
      double val = 0;
      double valerr = 0;
    
      TH1 * aux = histo_in->ProjectionY("_py",nb,nb);
      if (aux->GetEntries() > 0) {

         TFitResultPtr fr = aux->Fit("gaus","0qS");
           
         // Skip if fit failed
         if (!fr->Status()){
            //double mean    = fr->Parameter(1);
            //double meanerr = fr->ParError(1);
            double rms     = fr->Parameter(2);
            double rmserr  = fr->ParError(2);
            val = rms  ;
            valerr = rmserr;

         }

      }

      histo->SetBinContent(nb,val);
      histo->SetBinError(nb,valerr);

      // clean up
      delete aux;

   }

   // return
   return histo;

}//getResolutionHistoFromHisto_v3


// ------------------------------------------------------------------
// get the canvas from the resolution histo. Return rms(off_in)/mean(histo_in)
TH1 * getResolutionHistoFromHisto_v2(TString cname, TString title, TH2 * histo_in, TH2 *off_in){

   // make an empty copy to fill and return
   TH1 * histo = off_in->ProjectionX(cname);
   histo->Reset();
   //histo->Clear();
   histo->GetYaxis()->SetTitle(title);
   double maxy=0;
   // Now loop over the entries of prof and set the histo
   for (int nb = 1 ; nb <= histo->GetXaxis()->GetNbins() ; nb++){

      //double ptref = histo->GetXaxis()->GetBinCenter(nb);
      //double ptreferr =  0.5*(histo->GetXaxis()->GetBinLowEdge(nb)+histo->GetXaxis()->GetBinUpEdge(nb));
      double val = 0;
      double valerr = 0;
    
      TH1 * aux = histo_in->ProjectionY("_py",nb,nb);
      TH1 * aux2= off_in->ProjectionY("_py",nb,nb);
      if (aux->GetEntries() > 0 && aux2->GetEntries()>0) {

         TFitResultPtr fr = aux->Fit("gaus","0qS");
         //cout <<cname<<"Here1"<<endl;
         TFitResultPtr fr2= aux2->Fit("gaus","0qS");
         //cout <<cname<<"Here2"<<endl;
         // Skip if fit failed
         if (!fr->Status() && !fr2->Status()){
            double mean    = fr->Parameter(1);
            double meanerr = fr->ParError(1);
            //double rms     = fr->Parameter(2);
            //double rmserr  = fr->ParError(2);
            //double mean2    = fr2->Parameter(1);
            //double meanerr2 = fr2->ParError(1);
            double rms2     = fr2->Parameter(2);
            double rmserr2  = fr2->ParError(2);
            val = rms2 / mean ;//cout <<val<<" ";
            if (val>maxy) maxy=val;
            valerr = val * sqrt( pow(rmserr2/rms2,2) + pow(meanerr/mean,2));

         }

      }

      histo->SetBinContent(nb,val);
      histo->SetBinError(nb,valerr);
      histo->GetYaxis()->SetRangeUser(0,maxy);

      // clean up
      delete aux;
      delete aux2;

   }

   // return
   return histo;

}//getResolutionHistoFromHisto_v2


// ------------------------------------------------------------------
// get mean from histo. Return mean.
TH1 * getMeanHistoFromHisto(TString cname, TString title, TH2 *off_in,double & maxy){

   // make an empty copy to fill and return
   TH1 * histo = off_in->ProjectionX(cname);
   histo->Reset();
   //histo->Clear();
   histo->GetYaxis()->SetTitle(title);
  
   // Now loop over the entries of prof and set the histo
   for (int nb = 1 ; nb <= histo->GetXaxis()->GetNbins() ; nb++){

      //double ptref = histo->GetXaxis()->GetBinCenter(nb);
      //double ptreferr =  0.5*(histo->GetXaxis()->GetBinLowEdge(nb)+histo->GetXaxis()->GetBinUpEdge(nb));
      double val = 0;
      double valerr = 0;
    

      TH1 * aux= off_in->ProjectionY("_py",nb,nb);
      if (aux->GetEntries() > 0) {

         TFitResultPtr fr = aux->Fit("gaus","0qS");
         //cout <<cname<<"Here1"<<endl;

         // Skip if fit failed
         if (!fr->Status()){
            double mean    = fr->Parameter(1);
            double meanerr = fr->ParError(1);
            //double rms     = fr->Parameter(2);
            //double rmserr  = fr->ParError(2);

            val = mean ;//cout <<val<<" ";
            if (val>maxy) maxy=val;
            valerr = meanerr;

         }

      }

      histo->SetBinContent(nb,val);
      histo->SetBinError(nb,valerr);


      // clean up
      delete aux;

   }
   histo->GetYaxis()->SetRangeUser(0,maxy);
   // return
   return histo;

}//getMeanHistoFromHisto


// ------------------------------------------------------------------
// get mean over bin center from histo. Return mean over bin center.
TH1 * getMeanOverBinCenterHistoFromHisto(TString cname, TString title, TH2 *off_in,double & maxy){

   // make an empty copy to fill and return
   TH1 * histo = off_in->ProjectionX(cname);
   histo->Reset();
   //histo->Clear();
   histo->GetYaxis()->SetTitle(title);
  
   // Now loop over the entries of prof and set the histo
   for (int nb = 1 ; nb <= histo->GetXaxis()->GetNbins() ; nb++){

      double ptref = histo->GetXaxis()->GetBinCenter(nb);
      double ptreferr =  0.5*(histo->GetXaxis()->GetBinLowEdge(nb)+histo->GetXaxis()->GetBinUpEdge(nb));
      double val = 0;
      double valerr = 0;
    

      TH1 * aux= off_in->ProjectionY("_py",nb,nb);
      if (aux->GetEntries() > 0) {

         TFitResultPtr fr = aux->Fit("gaus","0qS");
         //cout <<cname<<"Here1"<<endl;

         // Skip if fit failed
         if (!fr->Status()){
            double mean    = fr->Parameter(1);
            double meanerr = fr->ParError(1);
            //double rms     = fr->Parameter(2);
            //double rmserr  = fr->ParError(2);

            val = mean/ptref ;//cout <<val<<" ";
            if (val>maxy) maxy=val;
            valerr = val * sqrt( pow(ptreferr/ptref,2) + pow(meanerr/mean,2));
         }

      }

      histo->SetBinContent(nb,val);
      histo->SetBinError(nb,valerr);


      // clean up
      delete aux;

   }
   histo->GetYaxis()->SetRangeUser(0,maxy);
   // return
   return histo;

}//getMeanOverBinCenterHistoFromHisto


// ------------------------------------------------------------------
// get the canvas from the resolution
TCanvas * getCanvasResponseResolution(TString cname, TString algo, TString title, TH2 * prof[4]){

   cout<<"\t Doing fits for Response Resolution "<<cname<<endl;
   algo.ToUpper();
   TCanvas * c = new TCanvas(cname,cname);
   c->SetLogx();
   TH1 * hh[4];
   for (int j=0;j<4;j++){
      TString hname = cname;
      hname += Form("_%i",j);
      hh[j] = getResolutionHistoFromHisto(hname, title, prof[j]);

   }
   setHistoColor(hh[0],colDet[0]);
   setHistoColor(hh[1],colDet[1]);
   setHistoColor(hh[2],colDet[2]);
   setHistoColor(hh[3],colDet[3]);

   hh[0]->GetYaxis()->SetRangeUser(0,0.4);
   hh[0]->Draw("E");
   for (int j=1;j<4;j++)
      hh[j]->Draw("sameE");

   TLegend *leg = new TLegend(0.7,0.72,0.9,0.92);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   for (int det=0;det<4;det++)
   {
      leg->AddEntry(hh[det],detName[det],"lep");
   }
   leg->Draw();

   return c;

}//getCanvasFromResolution


// ------------------------------------------------------------------
// get the canvas from the resolution
// input: one set of 2D histograms
// output:
//      modeNo == 0: RMS/mean of each x slice
//      modeNo == 1: RMS of each x slice
// Legend depends on cname. If cname contains "rho", output rho legend. If cname contains "npv", output npv legend. Otherwise, output PF legend.
TCanvas * getCanvasResolution(TString cname, TString algo, TString title, TH2 * prof[6],int modeNo){

   cout<<"\t Doing fits for Resolution "<<cname<<endl;
   algo.ToUpper();
   TCanvas * c = new TCanvas(cname,cname);
   c->SetLogx();
   TH1 * hh[6];
   for (int j=0;j<6;j++){
      TString hname = cname;
      hname += Form("_%i",j);
      if (modeNo==0)
         hh[j] = getResolutionHistoFromHisto(hname, title, prof[j]);
      else
         hh[j] = getResolutionHistoFromHisto_v3(hname, title, prof[j]);
   }
   setHistoColor(hh[0],colNpv0);
   setHistoColor(hh[1],colNpv5);
   setHistoColor(hh[2],colNpv10);
   setHistoColor(hh[3],colNpv15);
   setHistoColor(hh[4],colNpv20);
   setHistoColor(hh[5],colNpv25);

   if (modeNo==0)
      hh[0]->GetYaxis()->SetRangeUser(0,0.5);
   else
      hh[0]->GetYaxis()->SetRangeUser(0,20);
   hh[0]->Draw("E");
   for (int j=1;j<6;j++)
      hh[j]->Draw("sameE");

   TLegend * leg = new TLegend(0.65,0.56,0.9,0.85);
   leg->SetHeader(algo);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   int NPV_Rho;
   if (cname.Contains("npv",TString::kIgnoreCase))
      NPV_Rho = 1;
   else if (cname.Contains("rho",TString::kIgnoreCase))
      NPV_Rho = 2;
   else if (cname.Contains("tnpu",TString::kIgnoreCase))
      NPV_Rho = 3;
   else
      NPV_Rho = 0;
   if (NPV_Rho == 1)
   {
      leg->AddEntry(hh[0]," 0 <= N_{PV} < 5","lep");
      leg->AddEntry(hh[1]," 5 <= N_{PV} < 10","lep");
      leg->AddEntry(hh[2],"10 <= N_{PV} < 15","lep");
      leg->AddEntry(hh[3],"15 <= N_{PV} < 20","lep");
      leg->AddEntry(hh[4],"20 <= N_{PV} < 25","lep");
      leg->AddEntry(hh[5],"25 <= N_{PV} < 30","lep");
   }
   else if (NPV_Rho == 2)
   {
      leg->AddEntry(hh[0]," 0 <= Rho < 5","lep");
      leg->AddEntry(hh[1]," 5 <= Rho < 10","lep");
      leg->AddEntry(hh[2],"10 <= Rho < 15","lep");
      leg->AddEntry(hh[3],"15 <= Rho < 20","lep");
      leg->AddEntry(hh[4],"20 <= Rho < 25","lep");
      leg->AddEntry(hh[5],"25 <= Rho < 30","lep");
   }
   else if (NPV_Rho == 3)
   {
      leg->AddEntry(hh[0]," 0 <= True NPU < 5","lep");
      leg->AddEntry(hh[1]," 5 <= True NPU < 10","lep");
      leg->AddEntry(hh[2],"10 <= True NPU < 15","lep");
      leg->AddEntry(hh[3],"15 <= True NPU < 20","lep");
      leg->AddEntry(hh[4],"20 <= True NPU < 25","lep");
      leg->AddEntry(hh[5],"25 <= True NPU < 30","lep");
   }
   else
   {
      leg->AddEntry(hh[0],"chf","lep");
      leg->AddEntry(hh[1],"nhf","lep");
      leg->AddEntry(hh[2],"nef","lep");
      leg->AddEntry(hh[3],"cef","lep");
      leg->AddEntry(hh[4],"hfhf","lep");
      leg->AddEntry(hh[5],"hfef","lep");
   }
   leg->Draw();

   return c;

}//getCanvasFromResolution


//---------------------------------------------
// getResolutionNumDenom
// output rms(prof) and mean(off) of each X slice, and rms(prof)/mean(off) for sanity check.
TCanvas * getResolutionNumDenom(TString cname, TString ctitle, TString algo, TH2 * prof, TH2 * off){
   TCanvas *c = new TCanvas(cname,cname,1600,600);
   c->Divide(2);
   TPad *c_1 = ((TPad*)(c->GetPad(1)));
   TPad *c_2 = ((TPad*)(c->GetPad(2)));
   c_1->SetLogx();c_2->SetLogx();
   // make an empty copy to fill and return
   TH1 * histon = off->ProjectionX(cname);
   histon->Reset();
   histon->Clear();
   histon->GetYaxis()->SetTitle("#sigma(p_{T}^{PU}-p_{T}^{noPU})");
   histon->SetTitle(ctitle+"#sigma(p_{T}^{PU}-p_{T}^{noPU}) vs. p_{T}^{ref}");
   TH1 * histod = off->ProjectionX(cname);
   histod->Reset();
   histod->Clear();
   histod->GetYaxis()->SetTitle("<p_{T}^{noPU}/p_{T}^{ref}>");
   histod->SetTitle(ctitle+" <p_{T}^{noPU}/p_{T}^{ref}> vs. p_{T}^{ref}");
  
   //---to check, produce n/d
  
   TH1 * histocheck = off->ProjectionX(cname);
   histocheck->Reset();
   histocheck->Clear();
   histocheck->GetYaxis()->SetTitle("#sigma(p_{T}^{PU}-p_{T}^{noPU})/<p_{T}^{noPU}/p_{T}^{ref}>");
   histocheck->SetTitle(ctitle+" #sigma(p_{T}^{PU}-p_{T}^{noPU})/<p_{T}^{noPU}/p_{T}^{ref}> vs. p_{T}^{ref}");
  
   double maxy1=0;
   double maxy2=0;
   double maxycheck=0;
   //Now loop over the entries of prof and set the histo
   //cout <<histon->GetXaxis()->GetNbins()<<endl;
   for (int nb = 1 ; nb <= histon->GetXaxis()->GetNbins() ; nb++){

      //double ptref = histon->GetXaxis()->GetBinCenter(nb);
      //double ptreferr =  0.5*(histon->GetXaxis()->GetBinLowEdge(nb)+histon->GetXaxis()->GetBinUpEdge(nb));
      double val = 0;
      double valerr = 0;
      double mean=0, meanerr=0;
      //double rms, rmserr;
      //double mean2, meanerr2;
      double rms2=0, rmserr2=0;
      TH1 * aux = prof->ProjectionY("_py",nb,nb);
      TH1 * aux2= off->ProjectionY("_py",nb,nb);
      if (aux->GetEntries() > 0) {

         TFitResultPtr fr = aux->Fit("gaus","0qS");
         //cout <<cname<<"Here1"<<endl;
         TFitResultPtr fr2= aux2->Fit("gaus","0qS");
         //cout <<cname<<"Here2"<<endl;
         // Skip if fit failed
         if (!fr->Status() && !fr2->Status()){
            mean    = fr->Parameter(1);
            meanerr = fr->ParError(1);
        	//rms     = fr->Parameter(2);
        	//rmserr  = fr->ParError(2);
        	//mean2    = fr2->Parameter(1);
        	//meanerr2 = fr2->ParError(1);
            rms2     = fr2->Parameter(2);
            rmserr2  = fr2->ParError(2);
            val = rms2/mean;
            valerr = val * sqrt( pow(rmserr2/rms2,2) + pow(meanerr/mean,2));
   		
            if (maxy1<mean)	maxy1=mean;
            if (maxy2<rms2)	maxy2=rms2;
            if (maxycheck<val) maxycheck = val;
         }
      }

      histon->SetBinContent(nb,rms2);
      histon->SetBinError(nb,rmserr2);
      histod->SetBinContent(nb,mean);
      histod->SetBinError(nb,meanerr);
      histocheck->SetBinContent(nb,val);
      histocheck->SetBinError(nb,valerr);
      // clean up
      delete aux;
      delete aux2;

   }
   histocheck->GetYaxis()->SetRangeUser(0,30);
   TCanvas *ccheck = new TCanvas("ccheckoffreso","ccheckoffreso",1);
   ccheck->cd();
   ccheck->SetLogx();
   histocheck->Draw("E");
		

   histon->GetYaxis()->SetRangeUser(0,30);
   histod->GetYaxis()->SetRangeUser(0,maxy1*1.2);
   c->cd(1);
   histon->Draw("E");
   c->cd(2);
   histod->Draw("E");
   c->cd();
    
   return c;
}//getResolutionNumDenom


//-----------------------------------------------
//getGausMeanOffset
// output mean of each x slice
// Legend depends on cname. If cname contains "rho", output rho legend. If cname contains "npv", output npv legend. Otherwise, output PF legend.
TCanvas * getGausMeanOffset(TString cname, TString ctitle, TString algo, TH2 * off[7], bool fixedRange){

   cout<<"\t Doing fits for Mean "<<cname<<endl;
   algo.ToUpper();
  
   int NPV_Rho;
   if (cname.Contains("npv",TString::kIgnoreCase))
      NPV_Rho = 1;
   else if (cname.Contains("rho",TString::kIgnoreCase))
      NPV_Rho = 2;
   else if (cname.Contains("tnpu",TString::kIgnoreCase))
      NPV_Rho = 3;
   else if (cname.Contains("pdgid",TString::kIgnoreCase))
      NPV_Rho = 4;
   else
      NPV_Rho = 0;

   TCanvas * c = new TCanvas(cname,cname);
   c->SetLogx();
   TH1 * hh[7];
   double maxy = 0;
   int upper_limit = 0;
   if (NPV_Rho == 4)
      upper_limit = 7;
   else
      upper_limit = 6;
   for (int j=0;j<upper_limit;j++){
      TString hname = cname;
      hname += Form("_%i",j);
      hh[j] = getMeanHistoFromHisto(hname, ctitle, off[j],maxy);
   }
   setHistoColor(hh[0],colNpv0);
   setHistoColor(hh[1],colNpv5);
   setHistoColor(hh[2],colNpv10);
   setHistoColor(hh[3],colNpv15);
   setHistoColor(hh[4],colNpv20);
   setHistoColor(hh[5],colNpv25);
   if(NPV_Rho == 4) {
      setHistoColor(hh[0],colnJ);
      setHistoColor(hh[1],colqJ);
      setHistoColor(hh[2],colcJ);
      setHistoColor(hh[3],colbJ);
      setHistoColor(hh[4],colgJ);
      setHistoColor(hh[5],colaJ);
      setHistoColor(hh[6],colaqJ);
   }

   if(fixedRange)
      hh[0]->GetYaxis()->SetRangeUser(-3,3);
   else
      hh[0]->GetYaxis()->SetRangeUser(0,1.25*maxy);
   hh[0]->Draw("E");
   for (int j=1;j<upper_limit;j++)
      hh[j]->Draw("sameE");

   TLegend * leg = new TLegend(0.2,0.72,0.45,0.99);
   leg->SetHeader(algo);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   leg->SetName(cname+"_leg");

   if (NPV_Rho == 1)
   {
      leg->AddEntry(hh[0]," 0 <= N_{PV} < 5","lep");
      leg->AddEntry(hh[1]," 5 <= N_{PV} < 10","lep");
      leg->AddEntry(hh[2],"10 <= N_{PV} < 15","lep");
      leg->AddEntry(hh[3],"15 <= N_{PV} < 20","lep");
      leg->AddEntry(hh[4],"20 <= N_{PV} < 25","lep");
      leg->AddEntry(hh[5],"25 <= N_{PV} < 30","lep");
   }
   else if (NPV_Rho == 2)
   {
      leg->AddEntry(hh[0]," 0 <= Rho < 5","lep");
      leg->AddEntry(hh[1]," 5 <= Rho < 10","lep");
      leg->AddEntry(hh[2],"10 <= Rho < 15","lep");
      leg->AddEntry(hh[3],"15 <= Rho < 20","lep");
      leg->AddEntry(hh[4],"20 <= Rho < 25","lep");
      leg->AddEntry(hh[5],"25 <= Rho < 30","lep");
   }
   else if (NPV_Rho == 3)
   {
      leg->AddEntry(hh[0]," 0 <= True NPU < 5","lep");
      leg->AddEntry(hh[1]," 5 <= True NPU < 10","lep");
      leg->AddEntry(hh[2],"10 <= True NPU < 15","lep");
      leg->AddEntry(hh[3],"15 <= True NPU < 20","lep");
      leg->AddEntry(hh[4],"20 <= True NPU < 25","lep");
      leg->AddEntry(hh[5],"25 <= True NPU < 30","lep");
   }
   else if (NPV_Rho == 4)
   {
      leg->AddEntry(hh[0],"nJ (unknown PDGID)","lep");
      leg->AddEntry(hh[1],"qJ","lep");
      leg->AddEntry(hh[2],"cJ","lep");
      leg->AddEntry(hh[3],"bJ","lep");
      leg->AddEntry(hh[4],"gJ","lep");
      leg->AddEntry(hh[5],"aJ (all jets)","lep");
      leg->AddEntry(hh[6],"aqJ (quark jets)","lep");
   }
   else
   {
      leg->AddEntry(hh[0],"chf","lep");
      leg->AddEntry(hh[1],"nhf","lep");
      leg->AddEntry(hh[2],"nef","lep");
      leg->AddEntry(hh[3],"cef","lep");
      leg->AddEntry(hh[4],"hfhf","lep");
      leg->AddEntry(hh[5],"hfef","lep");
   }
   leg->Draw();

   return c;

}//getGausMeanOffset

TCanvas * getGausMeanOffsetWithSum(TString cname, TString ctitle, TString algo, TH2 * off[6], TH2 * sum, bool fixedRange){

   TCanvas * baseCanvas = getGausMeanOffset(cname,ctitle,algo,off,fixedRange);
   baseCanvas->cd();

   TH1 * hh;
   double maxy = 0;
   TString hname = cname;
   hname += Form("_6");
   hh = getMeanHistoFromHisto(hname, ctitle, sum, maxy);
   setHistoColor(hh,1);

   hh->Draw("sameE");
   
   TLegend* leg = (TLegend*)baseCanvas->GetPrimitive(cname+"_leg");
   int NPV_Rho;
   if (cname.Contains("npv",TString::kIgnoreCase))
      NPV_Rho = 1;
   else if (cname.Contains("rho",TString::kIgnoreCase))
      NPV_Rho = 2;
   else if (cname.Contains("tnpu",TString::kIgnoreCase))
      NPV_Rho = 3;
   else
      NPV_Rho = 0;
   if (NPV_Rho == 1)
      leg->AddEntry(hh," 0 <= N_{PV} < 30","lep");
   else if (NPV_Rho == 2)
      leg->AddEntry(hh,"0 <= Rho < 30","lep");
   else if (NPV_Rho == 3)
      leg->AddEntry(hh,"0 <= True NPU < 30","lep");
   else
      leg->AddEntry(hh,"Total <offset>","lep");

   leg->Draw();
   //baseCanvas->Update();

   return baseCanvas;
}//getGausMeanOffsetWithSum

//-----------------------------------------------
//getGausMeanOffsetOverPtref
// output mean of each x slice
// Legend depends on cname. If cname contains "rho", output rho legend. If cname contains "npv", output npv legend. Otherwise, output PF legend.
TCanvas * getGausMeanOffsetOverPtref(TString cname, TString ctitle, TString algo, TH2 * off[6],bool fixedRange){

   cout<<"\t Doing fits for Mean "<<cname<<endl;
   algo.ToUpper();
  
  
   TCanvas * c = new TCanvas(cname,cname);
   c->SetLogx();
   TH1 * hh[6];
   double maxy = 0;
   for (int j=0;j<6;j++){
      TString hname = cname;
      hname += Form("_%i",j);
      hh[j] = getMeanOverBinCenterHistoFromHisto(hname, ctitle, off[j],maxy);

   }
   setHistoColor(hh[0],colNpv0);
   setHistoColor(hh[1],colNpv5);
   setHistoColor(hh[2],colNpv10);
   setHistoColor(hh[3],colNpv15);
   setHistoColor(hh[4],colNpv20);
   setHistoColor(hh[5],colNpv25);

   if(fixedRange) {
      hh[0]->GetYaxis()->SetRangeUser(-0.1,0.1);
   }
   else {
      hh[0]->GetYaxis()->SetRangeUser(0,1.25*maxy);
   }
   hh[0]->Draw("E");
   for (int j=1;j<6;j++)
      hh[j]->Draw("sameE");

   TLegend * leg = new TLegend(0.2,0.72,0.45,0.99);
   leg->SetHeader(algo);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   int NPV_Rho;
   if (cname.Contains("npv",TString::kIgnoreCase))
      NPV_Rho = 1;
   else if (cname.Contains("rho",TString::kIgnoreCase))
      NPV_Rho = 2;
   else if (cname.Contains("tnpu",TString::kIgnoreCase))
      NPV_Rho = 3;
   else
      NPV_Rho = 0;
   if (NPV_Rho == 1)
   {
      leg->AddEntry(hh[0]," 0 <= N_{PV} < 5","lep");
      leg->AddEntry(hh[1]," 5 <= N_{PV} < 10","lep");
      leg->AddEntry(hh[2],"10 <= N_{PV} < 15","lep");
      leg->AddEntry(hh[3],"15 <= N_{PV} < 20","lep");
      leg->AddEntry(hh[4],"20 <= N_{PV} < 25","lep");
      leg->AddEntry(hh[5],"25 <= N_{PV} < 30","lep");
   }
   else if (NPV_Rho == 2)
   {
      leg->AddEntry(hh[0]," 0 <= Rho < 5","lep");
      leg->AddEntry(hh[1]," 5 <= Rho < 10","lep");
      leg->AddEntry(hh[2],"10 <= Rho < 15","lep");
      leg->AddEntry(hh[3],"15 <= Rho < 20","lep");
      leg->AddEntry(hh[4],"20 <= Rho < 25","lep");
      leg->AddEntry(hh[5],"25 <= Rho < 30","lep");
   }
   else if (NPV_Rho == 3)
   {
      leg->AddEntry(hh[0]," 0 <= True NPU < 5","lep");
      leg->AddEntry(hh[1]," 5 <= True NPU < 10","lep");
      leg->AddEntry(hh[2],"10 <= True NPU < 15","lep");
      leg->AddEntry(hh[3],"15 <= True NPU < 20","lep");
      leg->AddEntry(hh[4],"20 <= True NPU < 25","lep");
      leg->AddEntry(hh[5],"25 <= True NPU < 30","lep");
   }
   else
   {
      leg->AddEntry(hh[0],"chf","lep");
      leg->AddEntry(hh[1],"nhf","lep");
      leg->AddEntry(hh[2],"nef","lep");
      leg->AddEntry(hh[3],"cef","lep");
      leg->AddEntry(hh[4],"hfhf","lep");
      leg->AddEntry(hh[5],"hfef","lep");
   }
   leg->Draw();

   return c;

}//getGausMeanOffsetOverPtref



//--------------------------------------------------------
// getGausMeanOffsetScale
// output mean of each x slice and scale so that the *scaleNo* bin is 1.
// Legend depends on cname. If cname contains "rho", output rho legend. If cname contains "npv", output npv legend. Otherwise, output PF legend.
TCanvas * getGausMeanOffsetScale(TString cname, TString ctitle, TString algo, TH2 * off[6],int scaleNo, bool fixedRange){

   cout<<"\t Doing fits for Mean "<<cname<<endl;
   algo.ToUpper();
  
  
   TCanvas * c = new TCanvas(cname,cname);
   c->SetLogx();
   TH1 * hh[6];
   TH1 * histo = off[0]->ProjectionX(cname);
   histo->Reset();
   //histo->Clear();
   histo->Sumw2();
  

   double maxy = 0;
   //double scaleFactor;
   for (int j=0;j<6;j++){
      TString hname = cname;
      hname += Form("_%i",j);
      hh[j] = getMeanHistoFromHisto(hname, ctitle, off[j],maxy);
      //hh[j]->Sumw2();    
      for (int nb = 1 ; nb <= histo->GetXaxis()->GetNbins() ; nb++)
      {

         histo->SetBinContent(nb,hh[j]->GetBinContent(scaleNo));
         histo->SetBinError(nb,hh[j]->GetBinError(scaleNo));

      }    

      hh[j]->Divide(histo);
   }
   setHistoColor(hh[0],colNpv0);
   setHistoColor(hh[1],colNpv5);
   setHistoColor(hh[2],colNpv10);
   setHistoColor(hh[3],colNpv15);
   setHistoColor(hh[4],colNpv20);
   setHistoColor(hh[5],colNpv25);

   if(fixedRange){
      hh[0]->GetYaxis()->SetRangeUser(0.2,3);
   }
   else {
      hh[0]->GetYaxis()->SetRangeUser(0,1.25*maxy);
   }
   hh[0]->Draw("E");
   for (int j=1;j<6;j++)
      hh[j]->Draw("sameE");

   TLegend * leg = new TLegend(0.15,0.72,0.4,0.99);
   leg->SetHeader(algo);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   int NPV_Rho;
   if (cname.Contains("npv",TString::kIgnoreCase))
      NPV_Rho = 1;
   else if (cname.Contains("rho",TString::kIgnoreCase))
      NPV_Rho = 2;
   else if (cname.Contains("tnpu",TString::kIgnoreCase))
      NPV_Rho = 3;
   else
      NPV_Rho = 0;
   if (NPV_Rho == 1)
   {
      leg->AddEntry(hh[0]," 0 <= N_{PV} < 5","lep");
      leg->AddEntry(hh[1]," 5 <= N_{PV} < 10","lep");
      leg->AddEntry(hh[2],"10 <= N_{PV} < 15","lep");
      leg->AddEntry(hh[3],"15 <= N_{PV} < 20","lep");
      leg->AddEntry(hh[4],"20 <= N_{PV} < 25","lep");
      leg->AddEntry(hh[5],"25 <= N_{PV} < 30","lep");
   }
   else if (NPV_Rho == 2)
   {
      leg->AddEntry(hh[0]," 0 <= Rho < 5","lep");
      leg->AddEntry(hh[1]," 5 <= Rho < 10","lep");
      leg->AddEntry(hh[2],"10 <= Rho < 15","lep");
      leg->AddEntry(hh[3],"15 <= Rho < 20","lep");
      leg->AddEntry(hh[4],"20 <= Rho < 25","lep");
      leg->AddEntry(hh[5],"25 <= Rho < 30","lep");
   }
   else if (NPV_Rho == 3)
   {
      leg->AddEntry(hh[0]," 0 <= True NPU < 5","lep");
      leg->AddEntry(hh[1]," 5 <= True NPU < 10","lep");
      leg->AddEntry(hh[2],"10 <= True NPU < 15","lep");
      leg->AddEntry(hh[3],"15 <= True NPU < 20","lep");
      leg->AddEntry(hh[4],"20 <= True NPU < 25","lep");
      leg->AddEntry(hh[5],"25 <= True NPU < 30","lep");
   }
   else
   {
      leg->AddEntry(hh[0],"chf","lep");
      leg->AddEntry(hh[1],"nhf","lep");
      leg->AddEntry(hh[2],"nef","lep");
      leg->AddEntry(hh[3],"cef","lep");
      leg->AddEntry(hh[4],"hfhf","lep");
      leg->AddEntry(hh[5],"hfef","lep");
   }
   leg->Draw();

   return c;

}


//-----------------------------------------------------------
// get the canvas from the resolution_v2
// output Return rms(off)/mean(prof)
// Legend depends on cname. If cname contains "rho", output rho legend. If cname contains "npv", output npv legend. Otherwise, output PF legend.
TCanvas * getCanvasResolution_v2(TString cname, TString algo, TString title, TH2 * prof[6], TH2 * off[6]){

   cout<<"\t Doing fits for Resolution "<<cname<<endl;
   algo.ToUpper();
  
  
   TCanvas * c = new TCanvas(cname,cname);
   c->SetLogx();
   TH1 * hh[6];
   for (int j=0;j<6;j++){
      TString hname = cname;
      hname += Form("_%i",j);
      hh[j] = getResolutionHistoFromHisto_v2(hname, title, prof[j] , off[j]);

   }
   setHistoColor(hh[0],colNpv0);
   setHistoColor(hh[1],colNpv5);
   setHistoColor(hh[2],colNpv10);
   setHistoColor(hh[3],colNpv15);
   setHistoColor(hh[4],colNpv20);
   setHistoColor(hh[5],colNpv25);

   hh[0]->GetYaxis()->SetRangeUser(0,30);
   hh[0]->Draw("E");
   for (int j=1;j<6;j++)
      hh[j]->Draw("sameE");

   TLegend * leg = new TLegend(0.2,0.56,0.45,0.85);
   leg->SetHeader(algo);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   int NPV_Rho;
   if (cname.Contains("npv",TString::kIgnoreCase))
      NPV_Rho = 1;
   else if (cname.Contains("rho",TString::kIgnoreCase))
      NPV_Rho = 2;
   else if (cname.Contains("tnpu",TString::kIgnoreCase))
      NPV_Rho = 3;
   else
      NPV_Rho = 0;
   if (NPV_Rho == 1)
   {
      leg->AddEntry(hh[0]," 0 <= N_{PV} < 5","lep");
      leg->AddEntry(hh[1]," 5 <= N_{PV} < 10","lep");
      leg->AddEntry(hh[2],"10 <= N_{PV} < 15","lep");
      leg->AddEntry(hh[3],"15 <= N_{PV} < 20","lep");
      leg->AddEntry(hh[4],"20 <= N_{PV} < 25","lep");
      leg->AddEntry(hh[5],"25 <= N_{PV} < 30","lep");
   }
   else if (NPV_Rho == 2)
   {
      leg->AddEntry(hh[0]," 0 <= Rho < 5","lep");
      leg->AddEntry(hh[1]," 5 <= Rho < 10","lep");
      leg->AddEntry(hh[2],"10 <= Rho < 15","lep");
      leg->AddEntry(hh[3],"15 <= Rho < 20","lep");
      leg->AddEntry(hh[4],"20 <= Rho < 25","lep");
      leg->AddEntry(hh[5],"25 <= Rho < 30","lep");
   }
   else if (NPV_Rho == 3)
   {
      leg->AddEntry(hh[0]," 0 <= True NPU < 5","lep");
      leg->AddEntry(hh[1]," 5 <= True NPU < 10","lep");
      leg->AddEntry(hh[2],"10 <= True NPU < 15","lep");
      leg->AddEntry(hh[3],"15 <= True NPU < 20","lep");
      leg->AddEntry(hh[4],"20 <= True NPU < 25","lep");
      leg->AddEntry(hh[5],"25 <= True NPU < 30","lep");
   }
   else
   {
      leg->AddEntry(hh[0],"chf","lep");
      leg->AddEntry(hh[1],"nhf","lep");
      leg->AddEntry(hh[2],"nef","lep");
      leg->AddEntry(hh[3],"cef","lep");
      leg->AddEntry(hh[4],"hfhf","lep");
      leg->AddEntry(hh[5],"hfef","lep");
   }
   leg->Draw();

   return c;

}//getCanvasFromResolution_v2



// ------------------------------------------------------------------
// get the canvas from integral
// output integral of each X slice
// Legend is NPV legend
TCanvas * getCanvasIntegral(TString cname, TString algo, TString title, TProfile * prof[6]){
   cout<<"\t Doing integration "<<cname<<endl;
   algo.ToUpper();
   TCanvas * c = new TCanvas(cname,cname);
   c->SetLogx();
   TH1 * hh[6];
  
  
  
   for (int j=0;j<6;j++){
      TString hname = cname;
      hname += Form("_%i",j);
      hh[j] = getIntegralHistoFromHisto(hname, title, prof[j]);

   }
   setHistoColor(hh[0],colNpv0);
   setHistoColor(hh[1],colNpv5);
   setHistoColor(hh[2],colNpv10);
   setHistoColor(hh[3],colNpv15);
   setHistoColor(hh[4],colNpv20);
   setHistoColor(hh[5],colNpv25);

   hh[0]->GetYaxis()->SetRangeUser(0,22);
   hh[0]->Draw("E");
   for (int j=1;j<6;j++)
      hh[j]->Draw("sameE");

   TLegend * leg = new TLegend(0.65,0.56,0.9,0.85);
   leg->SetHeader(algo);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   leg->AddEntry(hh[0]," 0 <= N_{PV} < 5","lep");
   leg->AddEntry(hh[1]," 5 <= N_{PV} < 10","lep");
   leg->AddEntry(hh[2],"10 <= N_{PV} < 15","lep");
   leg->AddEntry(hh[3],"15 <= N_{PV} < 20","lep");
   leg->AddEntry(hh[4],"20 <= N_{PV} < 25","lep");
   leg->AddEntry(hh[5],"25 <= N_{PV}   ","lep");
   leg->Draw();

   return c;

}//getCanvasIntegral

// ------------------------------------------------------------------
// get the histo integral from the histo
// output integral of each X slice
//   Legend is NPV legend
TH1 * getIntegralHistoFromHisto(TString cname, TString title,TProfile *off_in){

   // make an empty copy to fill and return
   TH1 * histo = off_in->ProjectionX(cname);
   histo->Reset();
   //histo->Clear();
   histo->GetYaxis()->SetTitle(title);
   int binmax = histo->GetXaxis()->GetNbins();
   double maxy=0;
   // Now loop over the entries of prof and set the histo
   for (int nb = 1 ; nb <= histo->GetXaxis()->GetNbins() ; nb++){
      double valerr;
      double val = off_in->IntegralAndError(nb,binmax,valerr);
    
      //cout <<nb<<" "<<off_in->GetXaxis()->GetBinCenter(nb)<<" "<<off_in->GetBinContent(nb)<<" "<<val<<endl;
    
      histo->SetBinContent(nb,val);
      histo->SetBinError(nb,valerr);
      if (val>maxy) maxy = val;

   }
   histo->GetYaxis()->SetRangeUser(0,maxy*1.2);
   // return
   return histo;

}//getIntegralHistoFromHisto


// ------------------------------------------------------------------
// get the canvas from average
// output average of each X slice
TCanvas * getCanvasAverage(TString cname, TString algo, TString title, TProfile * prof[6]){

   cout<<"\t Doing Averaging "<<cname<<endl;
   algo.ToUpper();
   TCanvas * c = new TCanvas(cname,cname);
   c->SetLogx();
   TH1 * hh[6];
  
  
  
   for (int j=0;j<6;j++){
      TString hname = cname;
      hname += Form("_%i",j);
      hh[j] = getAverageHistoFromHisto(hname, title, prof[j]);

   }
   setHistoColor(hh[0],colNpv0);
   setHistoColor(hh[1],colNpv5);
   setHistoColor(hh[2],colNpv10);
   setHistoColor(hh[3],colNpv15);
   setHistoColor(hh[4],colNpv20);
   setHistoColor(hh[5],colNpv25);

   hh[0]->GetYaxis()->SetRangeUser(0,300);
   hh[0]->Draw("E");
   for (int j=1;j<6;j++)
      hh[j]->Draw("sameE");

   TLegend * leg = new TLegend(0.65,0.56,0.9,0.85);
   leg->SetHeader(algo);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   leg->AddEntry(hh[0]," 0 <= N_{PV} < 5","lep");
   leg->AddEntry(hh[1]," 5 <= N_{PV} < 10","lep");
   leg->AddEntry(hh[2],"10 <= N_{PV} < 15","lep");
   leg->AddEntry(hh[3],"15 <= N_{PV} < 20","lep");
   leg->AddEntry(hh[4],"20 <= N_{PV} < 25","lep");
   leg->AddEntry(hh[5],"25 <= N_{PV}   ","lep");
   leg->Draw();

   return c;

}//getCanvasAverage

// ------------------------------------------------------------------
// get the histo average from the histo
// output integral of each X slice
TH1 * getAverageHistoFromHisto(TString cname, TString title,TProfile *off_in){

   // make an empty copy to fill and return
   TH1 * histo = off_in->ProjectionX(cname);
   histo->Reset();
   //histo->Clear();
   histo->GetYaxis()->SetTitle(title);
   int binmax = histo->GetXaxis()->GetNbins();
   double maxy=0;
   // Now loop over the entries of prof and set the histo
   for (int nb = 1 ; nb <= histo->GetXaxis()->GetNbins() ; nb++){
      double inteErr;
      double inte = off_in->IntegralAndError(nb,binmax,inteErr);
      double jetEneAvg = 0;
      double jetEneAvgErr = 0;
    
      for (int nb2 = nb; nb2<=binmax;nb2++)
      {
         jetEneAvg += off_in->GetXaxis()->GetBinCenter(nb)*off_in->GetBinContent(nb);
         jetEneAvgErr += off_in->GetBinError(nb)*off_in->GetXaxis()->GetBinCenter(nb) + off_in->GetBinContent(nb)* 0.5*(off_in->GetXaxis()->GetBinWidth(nb));
      }
    
      if (inte>0)
      {
         jetEneAvgErr = sqrt(inte*inte*jetEneAvgErr*jetEneAvgErr+jetEneAvg*jetEneAvg*inteErr*inteErr)/inteErr/inteErr;
    
         jetEneAvg /= inte;
    
         histo->SetBinContent(nb,jetEneAvg);
         histo->SetBinError(nb,jetEneAvgErr);
         if (jetEneAvg>maxy) maxy = jetEneAvg;
      }

   }
   histo->GetYaxis()->SetRangeUser(0,maxy*1.2);
   // return
   return histo;

}//getAverageHistoFromHisto

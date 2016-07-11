#include "TCanvas.h"
#include "TFile.h"
#include "TProfile2D.h"
#include "TString.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TLatex.h"
#include "TPaveText.h"
#include "THStack.h"

#include <iostream>

#include "JetMETAnalysis/JetUtilities/interface/Style.h"

using namespace std;

///Scan the histogram for bin error being to high and remove points
void scanHistoBinError(TH1* histo, double maxBinError);
//Check is any of the bins in the histo are filled
bool isHistoEmpty(TH1* histo);
//Divide each bin of a histogram by a certain value and reset its content and error
void divideHistoBy(TH1* histo, double den, double denE = -1);
//Find the ymax such that it meets the previous criteria and no histogram overlaps with the legend
double findNonOverlappingYmax(TCanvas* c, vector<TH1*> hists, TLegend* leg, bool checkerr = true, pair<bool,bool> logxy = make_pair(false,false), bool debug = false);

// Forward decleration
void setHistoColor(TH1* h, int c);
void setHistoColorFill(TH1* h, int c);

//Returns a histogram through some proceedure
TH1 * getResolutionHistoFromHisto(TString cname, TString title, TH2 * histo_in);
TH1 * getResolutionHistoFromHisto_v3(TString cname, TString title, TH2 * histo_in);
TH1 * getResolutionHistoFromHisto_v2(TString cname, TString title, TH2 * histo_in, TH2 *off_in);
TH1 * getMeanHistoFromHisto(TString cname, TString title, TH2 *off_in,double & miny, double & maxy);
TH1 * getIntegralHistoFromHisto(TString cname, TString title,TProfile *off_in);
TH1 * getAverageHistoFromHisto(TString cname, TString title,TProfile *off_in);

//Formats a canvas in a certain fashion
TCanvas * getCanvasResponseResolution(TString cname, TString algo, TString title, vector<TH2*> prof);
TCanvas * getCanvasResolution(TString cname, TString algo, TString title, vector<TH2*> prof, int modeNo, vector<pair<int,int> > npvRhoNpuBins);
TCanvas * getResolutionNumDenom(TString cname, TString ctitle, TString algo, TH2 * prof, TH2 * off);
TCanvas * getGausMeanOffset(TString cname, TString ctitle, TString algo, vector<TH2*> off, bool fixedRange, vector<pair<int,int> > npvRhoNpuBins);
TCanvas * getGausMeanOffsetWithSum(TString cname, TString ctitle, TString algo, vector<TH2*> off, TH2* sum, bool fixedRange, vector<pair<int,int> > npvRhoNpuBins, pair<int,int> minmaxNpvRhoNpu);
TCanvas * getGausMeanOffsetOverPtref(TString cname, TString ctitle, TString algo, vector<TH2*> off, bool fixedRange, vector<pair<int,int> > npvRhoNpuBins);
TCanvas * getGausMeanOffsetScale(TString cname, TString ctitle, TString algo, vector<TH2*> off,int scaleNo, bool fixedRange, vector<pair<int,int> > npvRhoNpuBins);
TCanvas * getOffsetStack(TString cname, TString ctitle, TString algo, vector<TProfile*> off, bool fixedRange, vector<pair<int,int> > npvRhoNpuBins, double avgMu = 19.41, double avgMuE = 0.00179);
TCanvas * getOffsetStackWithSum(TString cname, TString ctitle, TString algo, vector<TProfile*> off, TProfile* sum, bool fixedRange, vector<pair<int,int> > npvRhoNpuBins, pair<int,int> minmaxNpvRhoNpu, double avgMu = 19.41, double avgMuE = 0.00179);
TCanvas * getCanvasResolution_v2(TString cname, TString algo, TString title, vector<TH2*> prof, vector<TH2*> off, vector<pair<int,int> > npvRhoNpuBins);
TCanvas * getCanvasIntegral(TString cname, TString algo, TString title, vector<TProfile*> prof, vector<pair<int,int> > npvRhoNpuBins);
TCanvas * getCanvasAverage(TString cname, TString algo, TString title, vector<TProfile*> prof, vector<pair<int,int> > npvRhoNpuBins);
TCanvas * getCanvasFromFittingProcedure(TString cname , TProfile2D * prof, TString fname);

// ------------------------------------------------------------------
void setHistoColor(TH1* h, int c){
   h->SetMarkerColor(c);
   h->SetLineColor(c);
}

// ------------------------------------------------------------------
void setHistoColorFill(TH1* h, int c){
   h->SetMarkerColor(c-1);
   h->SetLineColor(c);
   h->SetFillColor(c-1);
}

// ------------------------------------------------------------------
TCanvas * getCanvasFromFittingProcedure(TString cname , TProfile2D * prof, TString fname){

   const unsigned int NPARS = 4;

   // Create the canvas and all it's histos
   TCanvas * c = new TCanvas(cname,cname,1600,400);
   c->cd();
   c->Divide(NPARS,1);


   if(!prof) {
      cout << "WARNING::getCanvasFromFittingProcedure histogram prof was not set." << endl
           << "Returning blank histogram." << endl;
      return c;
   }
   cout<<"\t Fitting profile named "<<prof->GetName()<<endl;

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
   TH1 * histo = 0;
   if(histo_in)
      histo = histo_in->ProjectionX(cname);
   else
      return histo;
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
         if (fr.Get() && !fr->Status()){
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
         if (fr.Get() && fr2.Get() && !fr->Status() && !fr2->Status()){
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
TH1 * getMeanHistoFromHisto(TString cname, TString title, TH2 *off_in, double & miny, double & maxy){

   // make an empty copy to fill and return
   TH1 * histo = 0;
   if(off_in)
      histo = off_in->ProjectionX(cname);
   else
      return histo;
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
         //cout << cname << "sfsg1\tnb=" << nb << endl;

         // Skip if fit failed
         if (fr.Get() && !fr->Status()){
            double mean    = fr->Parameter(1);
            double meanerr = fr->ParError(1);
            //double rms     = fr->Parameter(2);
            //double rmserr  = fr->ParError(2);

            val = mean ;//cout <<val<<" ";
            if (val>maxy && meanerr<0.4) maxy=val;
            if (val<miny && meanerr<0.4) miny=val;
            valerr = meanerr;

         }

      }

      histo->SetBinContent(nb,val);
      histo->SetBinError(nb,valerr);


      // clean up
      delete aux;

   }
   //histo->GetYaxis()->SetRangeUser(0,maxy);
   histo->GetYaxis()->SetRangeUser(miny,maxy);
   // return
   return histo;

}//getMeanHistoFromHisto


// ------------------------------------------------------------------
// get mean over bin center from histo. Return mean over bin center.
TH1 * getMeanOverBinCenterHistoFromHisto(TString cname, TString title, TH2 *off_in, double & maxy){

   // make an empty copy to fill and return
   TH1 * histo = 0;
   if(off_in)
      histo = off_in->ProjectionX(cname);
   else
      return histo;
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
         if (fr.Get() && !fr->Status()){
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
TCanvas * getCanvasResponseResolution(TString cname, TString algo, TString title, vector<TH2*> prof){

   cout<<"\t Doing fits for Response Resolution "<<cname<<endl;
   algo.ToUpper();
   TCanvas * c = new TCanvas(cname,cname);
   c->SetLogx();
   vector<TH1*> hh(prof.size(),(TH1*)0);
   for (unsigned int j=0;j<prof.size();j++){
      TString hname = cname;
      hname += Form("_%i",j);
      hh[j] = getResolutionHistoFromHisto(hname, title, prof[j]);
      setHistoColor(hh[j],colDet[j]);
   }

   hh[0]->GetYaxis()->SetRangeUser(0,0.4);


   hh[0]->Draw("E");
   for (unsigned int j=1;j<hh.size();j++)
      hh[j]->Draw("sameE");

   TLegend *leg = new TLegend(0.7,0.72,0.9,0.92);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);
   for (int det=0;det<NDetectorNames;det++)
   {
      leg->AddEntry(hh[det],detector_names[det],"lep");
   }
   leg->Draw();
   cmsPrel(13,0);
   gPad->RedrawAxis();

   return c;

}//getCanvasFromResolution


// ------------------------------------------------------------------
// get the canvas from the resolution
// input: one set of 2D histograms
// output:
//      modeNo == 0: RMS/mean of each x slice
//      modeNo == 1: RMS of each x slice
// Legend depends on cname. If cname contains "rho", output rho legend. If cname contains "npv", output npv legend. Otherwise, output PF legend.
TCanvas * getCanvasResolution(TString cname, TString algo, TString title, vector<TH2*> prof, int modeNo, vector<pair<int,int> > npvRhoNpuBins){
   setTDRStyle();

   cout<<"\t Doing fits for Resolution "<<cname<<endl;
   JetInfo ji(algo);
   algo.ToUpper();
   
   int NPV_Rho;
   if (cname.Contains("npv",TString::kIgnoreCase))
      NPV_Rho = 1;
   else if (cname.Contains("rho",TString::kIgnoreCase))
      NPV_Rho = 2;
   else if (cname.Contains("tnpu",TString::kIgnoreCase))
      NPV_Rho = 3;
   else if (cname.Contains("npu",TString::kIgnoreCase))
      NPV_Rho = 4;
   else
      NPV_Rho = 0;

   TH1D* hbin = new TH1D(Form("hbin_%s",cname.Data()),Form("hbin_%s",cname.Data()), 10000, 0.,10000.);
   hbin->GetXaxis()->SetLimits(10.0,1000.0);
   hbin->GetXaxis()->SetMoreLogLabels();
   hbin->GetXaxis()->SetNoExponent();
   hbin->GetXaxis()->SetTitle("p_{T}^{ptcl} (GeV)");
   hbin->GetYaxis()->SetTitle(title);
   if (modeNo==0)
      hbin->GetYaxis()->SetRangeUser(0,0.5);
   else
      hbin->GetYaxis()->SetRangeUser(0,20);
   TCanvas* c = tdrCanvas(cname,hbin,14,0,true);
   c->GetPad(0)->SetLogx();

   TLegend* leg = tdrLeg(0.38,0.6,0.78,1-gPad->GetTopMargin()-0.045*(1-gPad->GetTopMargin()-gPad->GetBottomMargin())+0.01);
   leg->AddEntry((TObject*)0,ji.get_legend_title(string(ji.abbreviation),true).c_str(),"");
   TString eta;
   if(cname.Contains("BB"))
      eta = "|#eta|<1.3";
   else if(cname.Contains("EI"))
      eta = "1.3<|#eta|<2.5";
   else if(cname.Contains("EO"))
      eta = "2.5<|#eta|<3.0";
   else if(cname.Contains("FF"))
      eta = "3.0<|#eta|<5.0";
   leg->AddEntry((TObject*)0,eta,"");
   leg->SetName(cname+"_leg");
   //if(cname.Contains("PU_NoPU"))
   //   leg = new TLegend(0.65,0.48,0.90,0.75);
   //else
   //   leg = new TLegend(0.65,0.48,0.90,0.75);
   /*leg->AddEntry((TObject*)0,ji.get_legend_title(string(ji.abbreviation),true).c_str(),"");
   if(cname.Contains("BB"))
      leg->AddEntry((TObject*)0,ji.getDetRegion("Barrel"),"");
   else if(cname.Contains("EI"))
      leg->AddEntry((TObject*)0,ji.getDetRegion("Inner Endcap"),"");
   else if(cname.Contains("EO"))
      leg->AddEntry((TObject*)0,ji.getDetRegion("Outer Endcap"),"");
   else if(cname.Contains("FF"))
   leg->AddEntry((TObject*)0,ji.getDetRegion("Forward"),"");*/

   vector<TH1*> hh;
   for (unsigned int j=0;j<prof.size();j++){
      TString hname = cname;
      hname += Form("_%i",j);
      if (modeNo==0)
         hh.push_back(getResolutionHistoFromHisto(hname, title, prof[j]));
      else
         hh.push_back(getResolutionHistoFromHisto_v3(hname, title, prof[j]));
      hh.back()->GetXaxis()->SetRangeUser(10.0,1000.0);
   }
   if(!hh[0]) {
      cout << "WARNING::getCanvasResolution histogram hh[0] was not set by getResolutionHistoFromHisto." << endl
           << "Returning blank histogram." << endl;
      return c;
   }

   bool drawNoPU = false;
   bool drawNoPULeg = false;
   for (unsigned int j=0;j<hh.size();j++) {
      scanHistoBinError(hh[j],0.05);
      if(isHistoEmpty(hh[j])) {
         cout << "WARNING::getCanvasResolution histogram hh[j] = " << hh[j]->GetName() << " is empty an will be skipped." << endl;
         continue;
      }

      TString var;
      if (NPV_Rho == 1)
         var = "N_{PV}";
      else if (NPV_Rho == 2)
         var = "#rho";
      else if (NPV_Rho == 3)
         var = "#mu";//True NPU->#mu
      else if (NPV_Rho == 4)
         var = "N_{PU}";
      else
         var = PFstr[j];

      if((NPV_Rho==3 || NPV_Rho==4) && cname.Contains("PU_NoPU")) {
         if(drawNoPULeg == false) {
            TH1D* obj = new TH1D();
            obj->SetMarkerStyle(kOpenCircle);
            obj->SetLineStyle(kSolid);
            if(NPV_Rho == 3)
               leg->AddEntry(obj,"#mu=0","lep");
            else if(NPV_Rho == 4)
               leg->AddEntry(obj,"N_{PU}=0","lep");
            drawNoPULeg = true;
         }
         if(j>=(prof.size()/2) && drawNoPU == false) {
            tdrDraw(hh[j],"E",kOpenCircle,kBlack,kSolid,kBlack);
            drawNoPU = true;
         }
         else if(j<(prof.size()/2)) {
            tdrDraw(hh[j],"E",kFullCircle,colNpv[j%npvRhoNpuBins.size()],kSolid,colNpv[j%npvRhoNpuBins.size()]);
            leg->AddEntry(hh[j],JetInfo::getBinLegendEntry(var,npvRhoNpuBins[j%(prof.size()/2)].first,npvRhoNpuBins[j%(prof.size()/2)].second+1),"lep");
         }
      }
      else {
         if(j>=(prof.size()/2) && cname.Contains("PU_NoPU")) {
            tdrDraw(hh[j],"E",kOpenCircle,kBlack,kSolid,kBlack);
         }
         else {
            tdrDraw(hh[j],"E",kFullCircle,colNpv[j%npvRhoNpuBins.size()],kSolid,colNpv[j%npvRhoNpuBins.size()]);
         }
         if(j<hh.size()-1)
            leg->AddEntry(hh[j],JetInfo::getBinLegendEntry(var,npvRhoNpuBins[j].first,npvRhoNpuBins[j].second+1),"lep");
         else
            leg->AddEntry(hh[j],JetInfo::getBinLegendEntry(var,npvRhoNpuBins[j].first),"lep");
      }
   }
   leg->Draw("SAME");

/*
   //Draw Algo name and detector region eta using TLatex.
   TPaveText* pave = new TPaveText(0.40,0.75,0.92,0.92,"NDC TR");
   pave->SetFillColor(0);
   pave->SetShadowColor(0);
   pave->SetTextSize(0.045);
   pave->SetTextAlign(31);
   pave->AddText(ji.get_legend_title(string(ji.abbreviation),true).c_str());
   if(cname.Contains("BB"))
      pave->AddText(ji.getDetRegion("Barrel"));
   else if(cname.Contains("EI"))
      pave->AddText(ji.getDetRegion("Inner Endcap"));
   else if(cname.Contains("EO"))
      pave->AddText(ji.getDetRegion("Outer Endcap"));
   else if(cname.Contains("FF"))
      pave->AddText(ji.getDetRegion("Forward"));
   pave->Draw("same");
*/
   gPad->RedrawAxis();

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
   histon->SetTitle(ctitle+"#sigma(p_{T}^{PU}-p_{T}^{noPU}) vs. p_{T}^{GEN}");
   TH1 * histod = off->ProjectionX(cname);
   histod->Reset();
   histod->Clear();
   histod->GetYaxis()->SetTitle("<p_{T}^{noPU}/p_{T}^{GEN}>");
   histod->SetTitle(ctitle+" <p_{T}^{noPU}/p_{T}^{GEN}> vs. p_{T}^{GEN}");
  
   //---to check, produce n/d
  
   TH1 * histocheck = off->ProjectionX(cname);
   histocheck->Reset();
   histocheck->Clear();
   histocheck->GetYaxis()->SetTitle("#sigma(p_{T}^{PU}-p_{T}^{noPU})/<p_{T}^{noPU}/p_{T}^{GEN}>");
   histocheck->SetTitle(ctitle+" #sigma(p_{T}^{PU}-p_{T}^{noPU})/<p_{T}^{noPU}/p_{T}^{GEN}> vs. p_{T}^{GEN}");
  
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
TCanvas * getGausMeanOffset(TString cname, TString ctitle, TString algo, vector<TH2*> off, bool fixedRange, vector<pair<int,int> > npvRhoNpuBins){
   setTDRStyle();

   cout<<"\t Doing fits for Mean "<<cname<<endl;
   JetInfo ji(algo);
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
   else if (cname.Contains("npu",TString::kIgnoreCase))
      NPV_Rho = 5;
   else
      NPV_Rho = 0;

   TH1D* hbin = new TH1D(Form("hbin_%s",cname.Data()),Form("hbin_%s",cname.Data()), 10000, 0.,10000.);
   hbin->GetXaxis()->SetLimits(6.0,4000.0);
   hbin->GetXaxis()->SetMoreLogLabels();
   hbin->GetXaxis()->SetNoExponent();
   hbin->GetXaxis()->SetTitle("p_{T}^{ptcl} (GeV)");
   hbin->GetYaxis()->SetTitle(ctitle);
   TCanvas* c = tdrCanvas(cname,hbin,14,11,true);
   c->GetPad(0)->SetLogx();

   vector<TH1*> hh(off.size(),(TH1*)0);
   double maxy = 0;
   double miny = 0;
   for (unsigned int j=0;j<off.size();j++){
      TString hname = cname;
      hname += Form("_%i",j);
      hh[j] = getMeanHistoFromHisto(hname, ctitle, off[j], miny, maxy);
      //cout << hh[j] << " Mean for histo: " << hname << " " << ctitle << " " << off[j] << endl;
   }
   if(!hh[0]) {
      cout << "WARNING::getGausMeanOffset histogram hh[0] was not set by getMeanHistoFromHisto." << endl
           << "Returning blank histogram." << endl;
      return c;
   }

   //Set up legend here (before resetting yaxis range) so that we can determine ymax such that the histograms and the legend don't overlap
   TLegend* leg = tdrLeg(0.38,0.55,0.78,1-gPad->GetTopMargin()-0.045*(1-gPad->GetTopMargin()-gPad->GetBottomMargin())+0.005);
   leg->AddEntry((TObject*)0,ji.get_legend_title(string(ji.abbreviation),true).c_str(),"");
   TString eta;
   if(cname.Contains("BB"))
      eta = "|#eta|<1.3";
   else if(cname.Contains("EI"))
      eta = "1.3<|#eta|<2.5";
   else if(cname.Contains("EO"))
      eta = "2.5<|#eta|<3.0";
   else if(cname.Contains("FF"))
      eta = "3.0<|#eta|<5.0";
   leg->AddEntry((TObject*)0,eta,"");
   leg->SetName(cname+"_leg");

   /*
   if(fixedRange) {
      hbin->GetYaxis()->SetRangeUser(-3.0,3.0);
      if(NPV_Rho == 3)
         hbin->GetYaxis()->SetRangeUser(-0.6,1.4);
   }
   else {
      hbin->GetYaxis()->SetRangeUser(((miny >= 0) - (miny < 0))*1.25*fabs(miny),1.6*maxy);
      if(NPV_Rho == 0)
         hbin->GetYaxis()->SetRangeUser(((miny >= 0) - (miny < 0))*1.25*fabs(miny),1.6*maxy);
   }
   */

   for (unsigned int j=0;j<hh.size();j++) {
      scanHistoBinError(hh[j],0.4);
      if(isHistoEmpty(hh[j])) {
         cout << "WARNING::getGausMeanOffset histogram hh[j] = " << hh[j]->GetName() << " is empty and will be skipped." << endl;
         continue;
      }
      hh[j]->GetXaxis()->SetRangeUser(6.0,4000.0);
      hh[j]->GetXaxis()->SetLimits(6.0,4000.0);

      if(NPV_Rho == 4)
         tdrDraw(hh[j],"E",kFullCircle,colPDGID[j],kSolid,colPDGID[j]);
      else
         tdrDraw(hh[j],"E",kFullCircle,colNpv[j],kSolid,colNpv[j]);

      TString var;

      if (NPV_Rho == 1)
         var = "N_{PV}";
      else if (NPV_Rho == 2)
         var = "#rho";
      else if (NPV_Rho == 3)
         var = "#mu";//True NPU->#mu
      else if (NPV_Rho == 4)
         var = pdgidstrLegend[j];
      else if (NPV_Rho == 5)
         var = "N_{PU}";
      else {
         var = PFstr[j];
         var.ToUpper();
      }

      if(NPV_Rho == 0)
         leg->AddEntry(hh[j],var,"lep");
      else if(j<hh.size()-1)
         leg->AddEntry(hh[j],JetInfo::getBinLegendEntry(var,npvRhoNpuBins[j].first,npvRhoNpuBins[j].second+1),"lep");
      else if(NPV_Rho == 3)
         leg->AddEntry(hh[j],JetInfo::getBinLegendEntry(var,npvRhoNpuBins[j].first,npvRhoNpuBins[j].second+1),"lep");
      else
         leg->AddEntry(hh[j],JetInfo::getBinLegendEntry(var,npvRhoNpuBins[j].first),"lep");
   }
   leg->Draw("SAME");
   //c->Update();
   if(fixedRange) {
      hbin->GetYaxis()->SetRangeUser(-3.0,3.0);
      if(NPV_Rho == 3)
         hbin->GetYaxis()->SetRangeUser(-0.6,1.4);
   }
   else {
      //Need to call SetRangeUser eat least once to set the minimum. Otherwise findNonOverlappingYmax will not calculate the maximum correctly.
      hbin->GetYaxis()->SetRangeUser(((miny >= 0) - (miny < 0))*1.25*fabs(miny),1.6*maxy);
      c->Update();
      hbin->GetYaxis()->SetRangeUser(((miny >= 0) - (miny < 0))*1.25*fabs(miny),findNonOverlappingYmax(c,hh,leg));
      cout << "This is the value returned by findNonOverlappingYmax(c,hh,leg): " << findNonOverlappingYmax(c,hh,leg,true,make_pair(false,false),true) << endl;
   }


/*
   //Draw Algo name and detector region eta using TLatex.
   TPaveText* pave = new TPaveText(0.52,0.79,0.954,0.91,"NDC tr");
   pave->SetFillColor(0);
   pave->SetShadowColor(0);
   pave->SetBorderSize(0);
   pave->SetTextSize(0.045);
   pave->SetTextAlign(31);
   pave->AddText(ji.get_legend_title(string(ji.abbreviation),true).c_str());
   if(cname.Contains("BB"))
      pave->AddText("|#eta|<1.3");
   else if(cname.Contains("EI"))
      pave->AddText("1.3<|#eta|<2.5");
   else if(cname.Contains("EO"))
      pave->AddText("2.5<|#eta|<3.0");
   else if(cname.Contains("FF"))
      pave->AddText("3.0<|#eta|<5.0");
   pave->Draw("same");
*/
   gPad->RedrawAxis();

   return c;

}//getGausMeanOffset

TCanvas * getGausMeanOffsetWithSum(TString cname, TString ctitle, TString algo, vector<TH2*> off, TH2 * sum, bool fixedRange, vector<pair<int,int> > npvRhoNpuBins, pair<int,int> minmaxNpvRhoNpu){

   TCanvas * baseCanvas = getGausMeanOffset(cname,ctitle,algo,off,fixedRange,npvRhoNpuBins);
   baseCanvas->cd();

   TH1 * hh;
   double maxy = 0;
   double miny = 0;
   TString hname = cname;
   hname += Form("_%i",(unsigned int)off.size());
   hh = getMeanHistoFromHisto(hname, ctitle, sum, miny, maxy);
   if(!hh) {
      cout << "WARNING::getGausMeanOffsetWithSum histogram hh was not set by getMeanHistoFromHisto." << endl
           << "Returning basic canvas." << endl;
      return baseCanvas;
   }
   //setHistoColor(hh,1);
   //hh->Draw("sameE");
   tdrDraw(hh,"E",kOpenCircle,1,kSolid,1);
   
   TLegend* leg = (TLegend*)baseCanvas->GetPrimitive(cname+"_leg");
   int NPV_Rho;
   if (cname.Contains("npv",TString::kIgnoreCase))
      NPV_Rho = 1;
   else if (cname.Contains("rho",TString::kIgnoreCase))
      NPV_Rho = 2;
   else if (cname.Contains("tnpu",TString::kIgnoreCase))
      NPV_Rho = 3;
   else if (cname.Contains("npu",TString::kIgnoreCase))
      NPV_Rho = 4;
   else
      NPV_Rho = 0;

   if (NPV_Rho == 1)
      leg->AddEntry(hh,JetInfo::getBinLegendEntry("N_{PV}",minmaxNpvRhoNpu.first,minmaxNpvRhoNpu.second),"lep");
   else if (NPV_Rho == 2)
      leg->AddEntry(hh,JetInfo::getBinLegendEntry("#rho",minmaxNpvRhoNpu.first,minmaxNpvRhoNpu.second),"lep");
   else if (NPV_Rho == 3)
      leg->AddEntry(hh,JetInfo::getBinLegendEntry("#mu",minmaxNpvRhoNpu.first,minmaxNpvRhoNpu.second),"lep");//True NPU->#mu
   else if (NPV_Rho == 4)
      leg->AddEntry(hh,JetInfo::getBinLegendEntry("N_{PU}",minmaxNpvRhoNpu.first,minmaxNpvRhoNpu.second),"lep");//NPU
   else
      leg->AddEntry(hh,"Total <offset>","lep");

   leg->Draw();
   //baseCanvas->Update();
   //cmsPrel(13,0);

   return baseCanvas;
}//getGausMeanOffsetWithSum

//-----------------------------------------------
//getGausMeanOffsetOverPtref
// output mean of each x slice
// Legend depends on cname. If cname contains "rho", output rho legend. If cname contains "npv", output npv legend. Otherwise, output PF legend.
TCanvas * getGausMeanOffsetOverPtref(TString cname, TString ctitle, TString algo, vector<TH2*> off, bool fixedRange, vector<pair<int,int> > npvRhoNpuBins){

   cout<<"\t Doing fits for Mean "<<cname<<endl;
   algo.ToUpper();
  
  
   TCanvas * c = new TCanvas(cname,cname);
   c->SetLogx();

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
   else if (cname.Contains("npu",TString::kIgnoreCase))
      NPV_Rho = 4;
   else
      NPV_Rho = 0;

   vector<TH1*> hh(off.size(),(TH1*)0);
   double maxy = 0;
   for (unsigned int j=0;j<off.size();j++){
      TString hname = cname;
      hname += Form("_%i",j);
      hh.push_back(getMeanOverBinCenterHistoFromHisto(hname, ctitle, off[j], maxy));

   }
   if(!hh[0]) {
      cout << "WARNING::getGausMeanOffsetOverPtref histogram hh[0] was not set by getMeanOverBinCenterHistoFromHisto." << endl
           << "Returning blank canvas." << endl;
      return c;
   }
   for (unsigned int j=0;j<hh.size();j++){
      setHistoColor(hh[j],colNpv[j]);
   }

   if(fixedRange) {
      hh[0]->GetYaxis()->SetRangeUser(-0.1,0.1);
   }
   else {
      hh[0]->GetYaxis()->SetRangeUser(0,1.25*maxy);
   }
   hh[0]->GetXaxis()->SetRangeUser(0,1000);
   hh[0]->GetXaxis()->SetMoreLogLabels();
   hh[0]->GetXaxis()->SetNoExponent();

   for (unsigned int j=0;j<hh.size();j++) {
      if(j==0)
         hh[0]->Draw("E");
      else
         hh[j]->Draw("sameE");
      TString var;

      if (NPV_Rho == 1)
         var = "N_{PV}";
      else if (NPV_Rho == 2)
         var = "#rho";
      else if (NPV_Rho == 3)
         var = "#mu";//True NPU->#mu
      else if (NPV_Rho == 4)
         var = "N_{PU}";
      else
         var = PFstr[j];

      if(j<hh.size()-1)
         leg->AddEntry(hh[j],JetInfo::getBinLegendEntry(var,npvRhoNpuBins[j].first,npvRhoNpuBins[j].second+1),"lep");
      else
         leg->AddEntry(hh[j],JetInfo::getBinLegendEntry(var,npvRhoNpuBins[j].first),"lep");
   }
   leg->Draw();
   cmsPrel(13,0);
   gPad->RedrawAxis();

   return c;

}//getGausMeanOffsetOverPtref



//--------------------------------------------------------
// getGausMeanOffsetScale
// output mean of each x slice and scale so that the *scaleNo* bin is 1.
// Legend depends on cname. If cname contains "rho", output rho legend. If cname contains "npv", output npv legend. Otherwise, output PF legend.
TCanvas * getGausMeanOffsetScale(TString cname, TString ctitle, TString algo, vector<TH2*> off, int scaleNo, bool fixedRange, vector<pair<int,int> > npvRhoNpuBins){

   cout<<"\t Doing fits for Mean "<<cname<<endl;
   algo.ToUpper();
  
  
   TCanvas * c = new TCanvas(cname,cname);
   c->SetLogx();
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
   else if (cname.Contains("npu",TString::kIgnoreCase))
      NPV_Rho = 4;
   else
      NPV_Rho = 0;

   vector<TH1*> hh(off.size(),(TH1*)0);
   TH1 * histo = off[0]->ProjectionX(cname);
   histo->Reset();
   //histo->Clear();
   histo->Sumw2();
  

   double maxy = 0;
   double miny = 0;
   //double scaleFactor;
   for (unsigned int j=0;j<off.size();j++){
      TString hname = cname;
      hname += Form("_%i",j);
      hh[j] = getMeanHistoFromHisto(hname, ctitle, off[j], miny, maxy);
      //hh[j]->Sumw2();    
      for (int nb = 1 ; nb <= histo->GetXaxis()->GetNbins() ; nb++)
      {

         histo->SetBinContent(nb,hh[j]->GetBinContent(scaleNo));
         histo->SetBinError(nb,hh[j]->GetBinError(scaleNo));

      }    

      hh[j]->Divide(histo);
   }
   for (unsigned int j=0;j<hh.size();j++){
      setHistoColor(hh[j],colNpv[j]);
   }

   if(fixedRange){
      hh[0]->GetYaxis()->SetRangeUser(0.2,3);
   }
   else {
      hh[0]->GetYaxis()->SetRangeUser(((miny >= 0) - (miny < 0))*1.25*fabs(miny),1.25*maxy);
      //hh[0]->GetYaxis()->SetRangeUser(0,1.25*maxy);
   }
   hh[0]->GetXaxis()->SetRangeUser(0,1000);
   hh[0]->GetXaxis()->SetMoreLogLabels();
   hh[0]->GetXaxis()->SetNoExponent();

   for (unsigned int j=0;j<hh.size();j++) {
      if(j==0)
         hh[0]->Draw("E");
      else
         hh[j]->Draw("sameE");
      TString var;

      if (NPV_Rho == 1)
         var = "N_{PV}";
      else if (NPV_Rho == 2)
         var = "#rho";
      else if (NPV_Rho == 3)
         var = "#mu";//True NPU->#mu
      else if (NPV_Rho == 4)
         var = "N_{PU}";
      else
         var = PFstr[j];

      if(j<hh.size()-1)
         leg->AddEntry(hh[j],JetInfo::getBinLegendEntry(var,npvRhoNpuBins[j].first,npvRhoNpuBins[j].second+1),"lep");
      else
         leg->AddEntry(hh[j],JetInfo::getBinLegendEntry(var,npvRhoNpuBins[j].first),"lep");
   }
   leg->Draw();
   cmsPrel(13,0);
   gPad->RedrawAxis();

   return c;

}

//-----------------------------------------------
//getGausMeanOffset
// output mean of each x slice
// Legend depends on cname. If cname contains "rho", output rho legend. If cname contains "npv", output npv legend. Otherwise, output PF legend.
TCanvas * getOffsetStack(TString cname, TString ctitle, TString algo, vector<TProfile*> off, bool fixedRange, vector<pair<int,int> > npvRhoNpuBins, double avgMu, double avgMuE){
   //gROOT->LoadMacro("~aperloff/Scripts/tdrstyle_mod14.C"); setTDRStyle();
   setTDRStyle();

   cout<<"\t Doing fits for Mean "<<cname<<endl;
   JetInfo ji(algo);
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
   else if (cname.Contains("npu",TString::kIgnoreCase))
      NPV_Rho = 5;
   else
      NPV_Rho = 0;

   TH1D* hbin = new TH1D("hbin", "hbin", 10000, 0.,10000.);
   hbin->GetXaxis()->SetLimits(10.0,300.0);
   hbin->GetYaxis()->SetRangeUser(0.0,1.0);
   hbin->GetXaxis()->SetMoreLogLabels();
   hbin->GetXaxis()->SetNoExponent();
   hbin->GetXaxis()->SetTitle("p_{T}^{ptcl} (GeV}");
   hbin->GetYaxis()->SetTitle(ctitle);
   TCanvas* c = tdrCanvas(cname,hbin,14,11,true);
   c->GetPad(0)->SetLogx();

   //TCanvas * c = new TCanvas(cname,cname);
   //c->SetLogx();
   THStack* stack = new THStack(Form("%s_Stack",cname.Data()),ctitle);
   //stack->SetHistogram(hbin);
   //TLegend * leg = new TLegend(0.19,0.65,0.45,0.92);
   //leg->SetFillColor(0);
   //leg->SetFillStyle(0);
   //leg->SetBorderSize(0);
   //leg->SetTextSize(0.045);
   TLegend* leg = tdrLeg(0.38,0.6,0.78,1-gPad->GetTopMargin()-0.045*(1-gPad->GetTopMargin()-gPad->GetBottomMargin())+0.01);
   leg->AddEntry((TObject*)0,ji.get_legend_title(string(ji.abbreviation),true).c_str(),"");
   TString eta;
   if(cname.Contains("BB"))
      eta = "|#eta|<1.3";
   else if(cname.Contains("EI"))
      eta = "1.3<|#eta|<2.5";
   else if(cname.Contains("EO"))
      eta = "2.5<|#eta|<3.0";
   else if(cname.Contains("FF"))
      eta = "3.0<|#eta|<5.0";
   leg->AddEntry((TObject*)0,eta,"");
   leg->SetName(cname+"_leg");

   vector<TH1*> hh(off.size(),(TH1*)0);
   //double maxy = 0;
   //double miny = 0;
   for (unsigned int j=0;j<off.size();j++){
      TString hname = cname;
      hname += Form("_%i",j);
      hh[j] = off[j]->ProjectionX(hname);
      hh[j]->GetYaxis()->SetTitle(ctitle);
   //   hh[j] = getMeanHistoFromHisto(hname, ctitle, off[j], miny, maxy);
   //   //cout << hh[j] << " Mean for histo: " << hname << " " << ctitle << " " << off[j] << endl;
   }
   if(!hh[0]) {
      cout << "WARNING::getOffsetStack histogram off[0] was not set by getMeanHistoFromHisto." << endl
           << "Returning blank histogram." << endl;
      return c;
   }

   TH1D* background = (TH1D*)hh[0]->Clone("background");
   background->Reset();
   background->SetFillColor(kWhite);
   background->SetFillStyle(1001);

   for (unsigned int j=0;j<hh.size();j++) {
      //if(NPV_Rho == 4)
      //   setHistoColorFill(hh[j],colPDGID[j]);
      //else
      //   setHistoColorFill(hh[j],colNpv[j]);
      TString offName = off[j]->GetName();
      hh[j]->SetLineColor(kBlack);
      if(offName.Contains("PFCHF")) {
         hh[j]->SetFillColor(kRed);
         hh[j]->SetMarkerColor(kRed);
      }
      else if(offName.Contains("unassociatedchf")) {
         hh[j]->SetFillColor(kRed-9);
         hh[j]->SetMarkerColor(kRed-9);
         //hh[j]->SetFillStyle(3013);
      }
      else if(offName.Contains("chf")) {
         hh[j]->SetFillColor(kRed);
         hh[j]->SetMarkerColor(kRed);
      }
      else if(offName.Contains("nef")) {
         hh[j]->SetFillColor(kBlue);
         hh[j]->SetMarkerColor(kBlue);
      }
      else if(offName.Contains("cef")) {
         hh[j]->SetFillColor(kViolet+2);
         hh[j]->SetMarkerColor(kViolet+2);
      }
      else if(offName.Contains("nhf")) {
         hh[j]->SetFillColor(kGreen);
         hh[j]->SetMarkerColor(kGreen);
      }
      else if(offName.Contains("muf")) {
         hh[j]->SetFillColor(kViolet+2);
         hh[j]->SetMarkerColor(kViolet+2);
      }
      else if(offName.Contains("hfhf")) {
         hh[j]->SetFillColor(kPink+9);
         hh[j]->SetMarkerColor(kPink+9);
      }
      else if(offName.Contains("hfef")) {
         hh[j]->SetFillColor(kPink+9);
         hh[j]->SetMarkerColor(kPink+9);
      }

      scanHistoBinError(hh[j],1.0);
      divideHistoBy(hh[j],avgMu,avgMuE);
      if(isHistoEmpty(hh[j])) continue;

      //if(j==0)
      //   off[0]->Draw("E");
      //else
      //   off[j]->Draw("sameE");
      if((offName.Contains("hfhf")||offName.Contains("hfef"))&&(cname.Contains("BB")||cname.Contains("EI")||cname.Contains("EO")))
         continue;
      else if((offName.Contains("cef")||offName.Contains("muf"))&&(cname.Contains("BB")||cname.Contains("EI")||cname.Contains("EO")))
         continue;
      else if(offName.Contains("muf") && stack->GetHists()->FindObject("MeanOffRefPFStackWithSum_BB_1")!=0) {
         ((TH1D*)stack->GetHists()->FindObject("MeanOffRefPFStackWithSum_BB_1"))->Add(hh[j]);
      }
      else if(offName.Contains("muf") && stack->GetHists()->FindObject("MeanOffRefPFStack_BB_1")!=0) {
         ((TH1D*)stack->GetHists()->FindObject("MeanOffRefPFStack_BB_1"))->Add(hh[j]);
      }
      else if(offName.Contains("PFCHF")) {
         tdrDraw(hh[j],"HIST",kFullCircle,kRed,kSolid,kBlack,1001,kRed);
      }
      else {
         stack->Add(hh[j],"HIST");
         background->Add(hh[j]);
      }

      TString var, legVar;

      if (NPV_Rho == 1)
         var = "N_{PV}";
      else if (NPV_Rho == 2)
         var = "#rho";
      else if (NPV_Rho == 3)
         var = "#mu";//True NPU->#mu
      else if (NPV_Rho == 4)
         var = pdgidstrLegend[j];
      else if (NPV_Rho == 5)
         var = "N_{PU}";
      else {
         if(offName.Contains("PFCHF")) {
            var = "CHF";
            legVar = "Charged hadrons";
         }
         else if(offName.Contains("unassociatedchf")) {
            var = "CHF";
            legVar = "Unassoc. ch. hadrons";
         }
         else {
            var = PFstr[j];
            var.ToUpper();
            legVar = PFstr_long[j];
         }
      }

      if(NPV_Rho == 0) {
         if(var.CompareTo("CEF")==0)
            var = "CEF+MUF";
         else if(var.CompareTo("MUF")==0)
            continue;
         leg->AddEntry(hh[j],legVar,"f");
      }
      else if(j<hh.size()-1)
         leg->AddEntry(hh[j],JetInfo::getBinLegendEntry(var,npvRhoNpuBins[j].first,npvRhoNpuBins[j].second+1),"f");
      else if(NPV_Rho == 3)
         leg->AddEntry(hh[j],JetInfo::getBinLegendEntry(var,npvRhoNpuBins[j].first,npvRhoNpuBins[j].second+1),"f");
      else
         leg->AddEntry(hh[j],JetInfo::getBinLegendEntry(var,npvRhoNpuBins[j].first),"f");
   }
   tdrDraw(background,"HIST",kFullCircle,10,kSolid,kBlack,1001,10);
   tdrDraw(stack,"");
   //stack->Draw();
   //stack->GetXaxis()->SetTitle("p_{T}^{GEN} (GeV)");
   //stack->GetXaxis()->SetRangeUser(10.0,300.0);
   //stack->GetXaxis()->SetMoreLogLabels();
   //stack->GetXaxis()->SetNoExponent();
   //stack->GetYaxis()->SetTitle(ctitle);
   //stack->GetYaxis()->SetRangeUser(0.0,1.0);
   //stack->Draw();
   leg->Draw("SAME");
   //cmsPrel(13,0);

   //Draw Algo name and detector region eta using TLatex.
   //TPaveText* pave = new TPaveText(0.52,0.79,0.954,0.91,"NDC tr");
   //pave->SetFillColor(0);
   //pave->SetFillStyle(0);
   //pave->SetShadowColor(0);
   //pave->SetBorderSize(0);
   //pave->SetTextSize(0.045);
   //pave->SetTextAlign(31);
   //pave->AddText(ji.get_legend_title(string(ji.abbreviation),true).c_str());
   //if(cname.Contains("BB"))
   //   pave->AddText("|#eta|<1.3");
   //else if(cname.Contains("EI"))
   //   pave->AddText("1.3<|#eta|<2.5");
   //else if(cname.Contains("EO"))
   //   pave->AddText("2.5<|#eta|<3.0");
   //else if(cname.Contains("FF"))
   //   pave->AddText("3.0<|#eta|<5.0");
   //pave->Draw("same");

   //hbin->GetYaxis()->SetRangeUser(0.0,1.0);
   gPad->RedrawAxis();

   return c;

}//getGausMeanOffsetStack

TCanvas * getOffsetStackWithSum(TString cname, TString ctitle, TString algo, vector<TProfile*> off, TProfile * sum, bool fixedRange, vector<pair<int,int> > npvRhoNpuBins, pair<int,int> minmaxNpvRhoNpu, double avgMu, double avgMuE){

   TCanvas * baseCanvas = getOffsetStack(cname,ctitle,algo,off,fixedRange,npvRhoNpuBins);
   baseCanvas->cd();

   TH1 * hh;
   //double maxy = 0;
   //double miny = 0;
   TString hname = cname;
   hname += Form("_%i",(unsigned int)off.size());
   //hh = getMeanHistoFromHisto(hname, ctitle, sum, miny, maxy);
   hh = sum->ProjectionX(hname);
   hh->GetYaxis()->SetTitle(ctitle);
   if(!hh) {
      cout << "WARNING::getGausMeanOffsetStackWithSum histogram hh was not set by getMeanHistoFromHisto." << endl
           << "Returning basic canvas." << endl;
      return baseCanvas;
   }
   setHistoColor(hh,1);
   divideHistoBy(hh,avgMu,avgMuE);
   hh->GetXaxis()->SetRangeUser(10.0,300.0);
   hh->GetYaxis()->SetRangeUser(0,0.8);
   hh->Draw("sameEX0");
   
   TLegend* leg = (TLegend*)baseCanvas->GetPrimitive(cname+"_leg");
   int NPV_Rho;
   if (cname.Contains("npv",TString::kIgnoreCase))
      NPV_Rho = 1;
   else if (cname.Contains("rho",TString::kIgnoreCase))
      NPV_Rho = 2;
   else if (cname.Contains("tnpu",TString::kIgnoreCase))
      NPV_Rho = 3;
   else if (cname.Contains("npu",TString::kIgnoreCase))
      NPV_Rho = 4;
   else
      NPV_Rho = 0;

   if (NPV_Rho == 1)
      leg->AddEntry(hh,JetInfo::getBinLegendEntry("N_{PV}",minmaxNpvRhoNpu.first,minmaxNpvRhoNpu.second),"lep");
   else if (NPV_Rho == 2)
      leg->AddEntry(hh,JetInfo::getBinLegendEntry("#rho",minmaxNpvRhoNpu.first,minmaxNpvRhoNpu.second),"lep");
   else if (NPV_Rho == 3)
      leg->AddEntry(hh,JetInfo::getBinLegendEntry("#mu",minmaxNpvRhoNpu.first,minmaxNpvRhoNpu.second),"lep");//True NPU->#mu
   else if (NPV_Rho == 4)
      leg->AddEntry(hh,JetInfo::getBinLegendEntry("N_{PU}",minmaxNpvRhoNpu.first,minmaxNpvRhoNpu.second),"lep");//NPU
   else
      leg->AddEntry(hh,"Total <offset>","lep");

   leg->Draw("SAME");
   gPad->RedrawAxis();
   //baseCanvas->Update();
   //cmsPrel(13,0);

   return baseCanvas;
}//getGausMeanOffsetStackWithSum


//-----------------------------------------------------------
// get the canvas from the resolution_v2
// output Return rms(off)/mean(prof)
// Legend depends on cname. If cname contains "rho", output rho legend. If cname contains "npv", output npv legend. Otherwise, output PF legend.
TCanvas * getCanvasResolution_v2(TString cname, TString algo, TString title, vector<TH2*> prof, vector<TH2*> off, vector<pair<int,int> > npvRhoNpuBins){

   cout<<"\t Doing fits for Resolution "<<cname<<endl;
   JetInfo ji(algo);
   algo.ToUpper();
  
  
   TCanvas * c = new TCanvas(cname,cname);
   c->SetLogx();
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
   else if (cname.Contains("npu",TString::kIgnoreCase))
      NPV_Rho = 4;
   else
      NPV_Rho = 0;

   vector<TH1*> hh(prof.size(),(TH1*)0);
   for (unsigned int j=0;j<prof.size();j++){
      TString hname = cname;
      hname += Form("_%i",j);
      hh[j] = getResolutionHistoFromHisto_v2(hname, title, prof[j] , off[j]);

   }
   for (unsigned int j=0;j<hh.size();j++) {
      setHistoColor(hh[j],colNpv[j]);
      if(j==0) {
         hh[0]->GetYaxis()->SetLabelSize(0.04);
         hh[0]->GetYaxis()->SetRangeUser(0,30);
         hh[0]->GetXaxis()->SetRangeUser(0,1000);
         hh[0]->GetXaxis()->SetTitle("p_{T}^{ptcl} (GeV)");
         hh[0]->GetXaxis()->SetLabelSize(0.04);
         hh[0]->GetXaxis()->SetMoreLogLabels();
         hh[0]->GetXaxis()->SetNoExponent();
         hh[0]->Draw("E");
      }
      else
         hh[j]->Draw("sameE");
      TString var;

      if (NPV_Rho == 1)
         var = "N_{PV}";
      else if (NPV_Rho == 2)
         var = "#rho";
      else if (NPV_Rho == 3)
         var = "#mu";//True NPU->#mu
      else if (NPV_Rho == 4)
         var = "N_{PU}";
      else
         var = PFstr[j];

      if(j<hh.size()-1)
         leg->AddEntry(hh[j],JetInfo::getBinLegendEntry(var,npvRhoNpuBins[j].first,npvRhoNpuBins[j].second+1),"lep");
      else if(NPV_Rho == 3)
         leg->AddEntry(hh[j],JetInfo::getBinLegendEntry(var,npvRhoNpuBins[j].first,npvRhoNpuBins[j].second+1),"lep");
      else
         leg->AddEntry(hh[j],JetInfo::getBinLegendEntry(var,npvRhoNpuBins[j].first),"lep");
   }
   leg->Draw();
   cmsPrel(13,0);

   //Draw Algo name and detector region eta using TLatex.
   TPaveText* pave = new TPaveText(0.70,0.70,0.92,0.92,"NDC");
   pave->SetFillColor(0);
   pave->SetShadowColor(0);
   pave->AddText(ji.getAlias());
   if(cname.Contains("BB"))
      pave->AddText("|#eta|<1.3");
   else if(cname.Contains("EI"))
      pave->AddText("1.3<|#eta|<2.5");
   else if(cname.Contains("EO"))
      pave->AddText("2.5<|#eta|<3.0");
   else if(cname.Contains("FF"))
      pave->AddText("3.0<|#eta|<5.0");

   gPad->RedrawAxis();

   return c;

}//getCanvasFromResolution_v2



// ------------------------------------------------------------------
// get the canvas from integral
// output integral of each X slice
// Legend is NPV legend
TCanvas * getCanvasIntegral(TString cname, TString algo, TString title, vector<TProfile*> prof, vector<pair<int,int> > npvRhoNpuBins){
   cout<<"\t Doing integration "<<cname<<endl;
   algo.ToUpper();
   TCanvas * c = new TCanvas(cname,cname);
   c->SetLogx();
   TLegend * leg = new TLegend(0.65,0.56,0.9,0.85);
   leg->SetHeader(algo);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);

   //vector<TH1*> hh(prof.size(),(TH1*)0);
   vector<TH1*> hh;
   for (unsigned int j=0;j<prof.size();j++){
      TString hname = cname;
      hname += Form("_%i",j);
      //hh[j] = getIntegralHistoFromHisto(hname, title, prof[j]);
      hh.push_back(getIntegralHistoFromHisto(hname, title, prof[j]));
      if(hh.back())
         setHistoColor(hh[j],colNpv[j]);
   }
   if(!hh[0]) {
      cout << "WARNING::getCanvasIntegral histogram hh[0] was not set by getIntegralHistoFromHisto." << endl
           << "Returning blank canvas." << endl;
      return c;
   }

   for (unsigned int j=0;j<hh.size();j++) {
      if(j==0) {
         hh[j]->GetYaxis()->SetRangeUser(0,22);
         hh[j]->Draw("E");
      }
      else
         hh[j]->Draw("sameE");
      if(j<hh.size()-1)
         leg->AddEntry(hh[j],JetInfo::getBinLegendEntry("N_{PV}",npvRhoNpuBins[j].first,npvRhoNpuBins[j].second+1),"lep");
      else
         leg->AddEntry(hh[j],JetInfo::getBinLegendEntry("N_{PV}",npvRhoNpuBins[j].first),"lep");
   }
   leg->Draw();
   cmsPrel(13,0);

   return c;

}//getCanvasIntegral

// ------------------------------------------------------------------
// get the histo integral from the histo
// output integral of each X slice
//   Legend is NPV legend
TH1 * getIntegralHistoFromHisto(TString cname, TString title,TProfile *off_in){

   // make an empty copy to fill and return
   TH1 * histo = 0;
   if(off_in)
      histo = off_in->ProjectionX(cname);
   else
      return histo;
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
TCanvas * getCanvasAverage(TString cname, TString algo, TString title, vector<TProfile*> prof, vector<pair<int,int> > npvRhoNpuBins){

   cout<<"\t Doing Averaging "<<cname<<endl;
   algo.ToUpper();
   TCanvas * c = new TCanvas(cname,cname);
   c->SetLogx();
   TLegend * leg = new TLegend(0.65,0.56,0.9,0.85);
   leg->SetHeader(algo);
   leg->SetFillColor(0);
   leg->SetBorderSize(0);

   //vector<TH1*> hh(prof.size(),(TH1*)0);  
   vector<TH1*> hh;  
   for (unsigned int j=0;j<prof.size();j++){
      TString hname = cname;
      hname += Form("_%i",j);
      //hh[j] = getAverageHistoFromHisto(hname, title, prof[j]);
      hh.push_back(getAverageHistoFromHisto(hname, title, prof[j]));
      if(hh.back())
         setHistoColor(hh[j],colNpv[j]);
   }
   if(!hh[0]) {
      cout << "WARNING::getCanvasAverage histogram hh[0] was not set by getAverageHistoFromHisto." << endl
           << "Returning blank canvas." << endl;
      return c;
   }
   for (unsigned int j=0;j<hh.size();j++) {
      if(j==0) {
         hh[j]->GetYaxis()->SetRangeUser(0,300);
         hh[j]->Draw("E");
      }
      else
         hh[j]->Draw("sameE");
      if(j<hh.size()-1)
         leg->AddEntry(hh[j],JetInfo::getBinLegendEntry("N_{PV}",npvRhoNpuBins[j].first,npvRhoNpuBins[j].second+1),"lep");
      else
         leg->AddEntry(hh[j],JetInfo::getBinLegendEntry("N_{PV}",npvRhoNpuBins[j].first),"lep");
   }
   leg->Draw();
   cmsPrel(13,0);

   return c;

}//getCanvasAverage

// ------------------------------------------------------------------
// get the histo average from the histo
// output integral of each X slice
TH1 * getAverageHistoFromHisto(TString cname, TString title,TProfile *off_in){

   // make an empty copy to fill and return
   TH1 * histo = 0;
   if(off_in)
      histo = off_in->ProjectionX(cname);
   else
      return histo;

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

////////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void scanHistoBinError(TH1* histo, double maxBinError) {
   for(int ibin=1; ibin<=histo->GetNbinsX(); ibin++) {
      if(histo->GetBinError(ibin)>=maxBinError) {
         histo->SetBinContent(ibin,0);
         histo->SetBinError(ibin,0);
      }
   }
}

//______________________________________________________________________________
bool isHistoEmpty(TH1* histo) {
   bool isEmpty = true;
   for(int ibin=1; ibin<=histo->GetNbinsX(); ibin++) {
      if(histo->GetBinContent(ibin)!=0)
         isEmpty = false;
   }
   return isEmpty;
}

//______________________________________________________________________________
void divideHistoBy(TH1* histo, double den, double denE) {
   for(int ibin=1; ibin<=histo->GetNbinsX(); ibin++) {
      double num = histo->GetBinContent(ibin);
      double numE = histo->GetBinError(ibin);
      histo->SetBinContent(ibin,num/den);
      if(denE>-1)
         histo->SetBinError(ibin,TMath::Sqrt((TMath::Power(1.0/den,2)*TMath::Power(numE,2))+
                            ((TMath::Power(-1.0*num/TMath::Power(den,2),2)*TMath::Power(denE,2)))));
   }
}

//______________________________________________________________________________
double findNonOverlappingYmax(TCanvas* c, vector<TH1*> hists, TLegend* leg, bool checkerr, pair<bool,bool> logxy, bool debug) {
   TVirtualPad* pad = c->GetPad(0);

   //step 3: find highest bin that could overlap with legend, and set ymax to prevent overlap
   double gy = 1 - (pad->GetBottomMargin() + pad->GetTopMargin());
   double gx = 1 - (pad->GetLeftMargin() + pad->GetRightMargin());
   double eps = 0.075;//0.05; //small separation between legend and histos
   double ytick = (hists.size()>0) ? hists[0]->GetYaxis()->GetTickLength() : 0;
   double xtick = (hists.size()>0) ? hists[0]->GetXaxis()->GetTickLength() : 0;
   double lbound = pad->GetLeftMargin() + ytick;
   //double rbound = 1 - (pad->GetRightMargin() + ytick);
   double tbound = 1 - (pad->GetTopMargin() + xtick);
   //double bbound = 1 - (pad->GetTopMargin() + xtick);

   //bounds to check
   double ucmin[2], ucmax[2], vcmin[2]; //[0] is legend side, [1] is other side
   ucmin[0] = leg->GetX1NDC();
   ucmax[0] = leg->GetX2NDC();
   ucmin[1] = lbound;
   ucmax[1] = ucmin[0];

   vcmin[0] = leg->GetY1NDC(); //legend always at the bottom
   vcmin[1] = tbound; //just compare to top of plot (margin + ticks) on the other side
         
   //loop over histos
   double bh[2]; //height of highest bin + error (legend)
   bh[0] = bh[1] = 0;
   for(unsigned s = 0; s < hists.size(); s++){
      TAxis* x1 = hists[s]->GetXaxis();
      int xomin, xomax; //original xmin and xmax bin #s, to reset range at the end
      xomin = x1->GetFirst();
      xomax = x1->GetLast();

      //count bins not shown in this histogram due to a new range
      int unseenBins = 0;
      for(int ibin = 1; ibin <= hists[s]->GetNbinsX(); ibin++) {
         if(hists[s]->GetBinLowEdge(ibin)<x1->GetXmin() || hists[s]->GetBinLowEdge(ibin)>=x1->GetXmax()) unseenBins++;
      }
            
      for(int i = 0; i < 2; i++){ //check each side of plot
         //new bin #s for limited range
         int xbmin, xbmax;
            
         xbmin = logxy.first
               ? x1->FindBin(x1->GetXmin()*pow(x1->GetXmax()/x1->GetXmin(), (ucmin[i] - pad->GetLeftMargin())/gx))
               : x1->FindBin((ucmin[i] - pad->GetLeftMargin())*(x1->GetXmax() - x1->GetXmin())/gx + x1->GetXmin());
         if(xbmin > 1) xbmin -= 1; //include partial overlap
         xbmax = logxy.first
               ? x1->FindBin(x1->GetXmin()*pow(x1->GetXmax()/x1->GetXmin(), (ucmax[i] - pad->GetLeftMargin())/gx))
               : x1->FindBin((ucmax[i] - pad->GetLeftMargin())*(x1->GetXmax() - x1->GetXmin())/gx + x1->GetXmin());
         if(xbmax < hists[s]->GetNbinsX()) xbmax += 1; //include partial overlap
         
         if(debug) {
            cout << "hist name = " << hists[s]->GetName() << endl
                 << "\tucmin[" << i << "] = " << ucmin[i] << endl
                 << "\tucmax[" << i << "] = " << ucmax[i] << endl
                 << "\tpad->GetLeftMargin() = " << pad->GetLeftMargin() << endl
                 << "\tx1->GetXmin() = " << x1->GetXmin() << endl
                 << "\tx1->GetXmax() = " << x1->GetXmax() << endl
                 << "\tgx = " << gx << endl
                 << "\txbmin[" << i << "] = " << xbmin << endl
                 << "\txbmax[" << i << "] = " << xbmax << endl;
         }

         //set range for search
         //might need to remove the -unseenBins from the xbmax. It could be cutting off too many of the bins that need to be checked.
         x1->SetRange(xbmin-unseenBins,xbmax-unseenBins);
         int b_ = hists[s]->GetMaximumBin();
         double bh_ = hists[s]->GetBinContent(b_);
         if(checkerr) bh_ += hists[s]->GetBinError(b_);
         //check height
         if(bh_ > bh[i]) bh[i] = bh_;

         //reset to original range
         x1->SetRange(xomin,xomax);
      }
   }

   double ymin = pad->GetUymin();

   //print out some debugging information
   if(debug) {
      cout << "gy = " << gy << endl;
      cout << "eps = " << eps << endl;
      cout << "leg->GetY1NDC() = " << leg->GetY1NDC() << "\tvcmin[0] = " << vcmin[0] << endl;
      cout << "leg->GetY2NDC() = " << leg->GetY2NDC() << "\tvcmin[1] = " << vcmin[1] << endl;
      cout << "ymin = " << ymin << endl;
      cout << "pad->GetBottomMargin() = " << pad->GetBottomMargin() << endl;
   }

   double ymax_[2];
   for(int i = 0; i < 2; i++){
      //account for log scale if enabled
      ymax_[i] = logxy.second ? ymin*pow(bh[i]/ymin, gy/(vcmin[i] - pad->GetBottomMargin() - eps)) : ymin + gy*(bh[i] - ymin)/(vcmin[i] - pad->GetBottomMargin() - eps);
      if(debug) {
         cout << "bh[" << i << "] = " << bh[i] << endl;
         cout << "vcmin[" << i << "] = " << vcmin[i] << endl;
         cout << "ymax_[" << i << "] = " << ymax_[i] << endl;
      }
   }

   if(debug) {
      cout << "New ymax should be: " << max(ymax_[0],ymax_[1]) << endl;
   }
   //return ymax
   return max(ymax_[0],ymax_[1]);
}

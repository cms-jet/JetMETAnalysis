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
#include "TLegend.h"

#include <iostream>

#include <vector>
#include <utility>

using namespace std;

double findNonOverlappingYmax(TCanvas* c, vector<TH1*> hists, TLegend* leg, bool checkerr = true, pair<bool,bool> logxy = make_pair(false,false)) {
   TVirtualPad* pad = c->GetPad(0);

   //step 3: find highest bin that could overlap with legend, and set ymax to prevent overlap
   double gy = 1 - (pad->GetBottomMargin() + pad->GetTopMargin());
   cout << "gy = " << gy << endl;
   double gx = 1 - (pad->GetLeftMargin() + pad->GetRightMargin());
   double eps = 0.03; //small separation between legend and histos
   cout << "eps = " << eps << endl;
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
   vcmin[1] = tbound;//leg->GetY2NDC(); //just compare to top of plot (margin + ticks) on the other side
   cout << "leg->GetY1NDC() = " << leg->GetY1NDC() << "\tvcmin[0] = " << vcmin[0] << endl;
   cout << "leg->GetY2NDC() = " << leg->GetY2NDC() << "\tvcmin[1] = " << vcmin[1] << endl;            

   //loop over histos
   double bh[2]; //height of highest bin + error (legend)
   bh[0] = bh[1] = 0;
   for(unsigned s = 0; s < hists.size(); s++){
      TAxis* x1 = hists[s]->GetXaxis();
      int xomin, xomax; //original xmin and xmax bin #s, to reset range at the end
      xomin = x1->GetFirst();
      xomax = x1->GetLast();
            
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
         
         cout << "xbmin[" << i << "] = " << xbmin << endl;
         cout << "xbmax[" << i << "] = " << xbmax << endl;

         //set range for search
         x1->SetRange(xbmin,xbmax);
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
   cout << "ymin = " << ymin << endl;
   cout << "pad->GetBottomMargin() = " << pad->GetBottomMargin() << endl;

   double ymax_[2];
   for(int i = 0; i < 2; i++){
      //account for log scale if enabled
      ymax_[i] = logxy.second ? ymin*pow(bh[i]/ymin, gy/(vcmin[i] - pad->GetBottomMargin() - eps)) : ymin + gy*(bh[i] - ymin)/(vcmin[i] - pad->GetBottomMargin() - eps);
      cout << "bh_[" << i << "] = " << bh[i] << endl;
      cout << "vcmin[" << i << "] = " << vcmin[i] << endl;
      cout << "ymax_[" << i << "] = " << ymax_[i] << endl;
   }

   //return ymax
   return max(ymax_[0],ymax_[1]);
}

void testFunc() {
	TCanvas* c = new TCanvas();
	TF1* ff = new TF1("ff","TMath::Gaus(x,5,2)",0,10);
   ff=ff;
	TF1* fff = new TF1("fff","TMath::Gaus(x,3,1)",0,10);
   fff=fff;
	TH1D* h = new TH1D("h","h",10,0,10);
	h->FillRandom("gaus");
	TH1D* hh = new TH1D("hh","hh",10,0,10);
	hh->FillRandom("ff");
	TH1D* hhh = new TH1D("hhh","hhh",10,0,10);
	hhh->FillRandom("fff");
	h->Draw();
	hh->Draw("same");
	hhh->Draw("same");
	vector<TH1*> v = {h,hh,hhh};
	TLegend* leg = new TLegend(0.3,0.4,0.5,0.6);
	leg->AddEntry(h,"h","l");
	leg->AddEntry(hh,"hh","l");
	leg->AddEntry(hhh,"hhh","l");
	leg->Draw("same");
	cout << "leg->GetY1NDC() = " << leg->GetY1NDC() << endl;
   cout << "leg->GetY2NDC() = " << leg->GetY2NDC() << endl; 
   c->Update(); 
   cout << "leg->GetY1NDC() = " << leg->GetY1NDC() << endl;
   cout << "leg->GetY2NDC() = " << leg->GetY2NDC() << endl; 

	cout << "New ymax should be: " << findNonOverlappingYmax(c,v,leg) << endl;
}
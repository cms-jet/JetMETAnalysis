#include "TStyle.h"
#include "TROOT.h"
#include "TPad.h"
#include "TLatex.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TGraph.h"

#include <string>
#include <assert.h>

using std::string;

// colors to use
EColor tdrColors[13] = {kBlack, kBlue, kRed, kGreen, kYellow, kMagenta, kCyan,
                        kOrange, kSpring, kTeal, kAzure, kViolet, kPink};

// Add useful short-hands
void tdrDraw(TH1* h, string opt,
       int marker=kFullCircle, int mcolor = kBlack,
       int lstyle=kSolid, int lcolor=-1,
       int fstyle=1001, int fcolor=kYellow+1) {
  h->SetMarkerStyle(marker);
  h->SetMarkerColor(mcolor);
  h->SetLineStyle(lstyle);
  h->SetLineColor(lcolor==-1 ? mcolor : lcolor);
  h->SetFillStyle(fstyle);
  h->SetFillColor(fcolor);
  h->Draw((opt+"SAME").c_str());
}

void tdrDraw(TGraph* g, string opt,
       int marker=kFullCircle, int mcolor = kBlack,
       int lstyle=kSolid, int lcolor=-1,
       int fstyle=1001, int fcolor=kYellow+1) {
  g->SetMarkerStyle(marker);
  g->SetMarkerColor(mcolor);
  g->SetLineStyle(lstyle);
  g->SetLineColor(lcolor==-1 ? mcolor : lcolor);
  g->SetFillStyle(fstyle);
  g->SetFillColor(fcolor);
  g->Draw((opt+"SAME").c_str());
}

TLegend *tdrLeg(double x1, double y1, double x2, double y2) {
  TLegend *leg = new TLegend(x1, y1, x2, y2, "", "brNDC");
  leg->SetFillStyle(kNone);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.045);
  leg->Draw();
  return leg;
}

// tdrGrid: Turns the grid lines on (true) or off (false)

void tdrGrid(TStyle* tdrStyle, bool gridOn) {
  tdrStyle->SetPadGridX(gridOn);
  tdrStyle->SetPadGridY(gridOn);
}

// fixOverlay: Redraws the axis

void fixOverlay() {
   gPad->RedrawAxis();
}

TStyle* getTDRStyle() {

  TStyle *tdrStyle = new TStyle("tdrStyle","Style for P-TDR");

// For the canvas:
  tdrStyle->SetCanvasBorderMode(0);
  tdrStyle->SetCanvasColor(kWhite);
  tdrStyle->SetCanvasDefH(600); //Height of canvas
  tdrStyle->SetCanvasDefW(600); //Width of canvas
  tdrStyle->SetCanvasDefX(0);   //Position on screen
  tdrStyle->SetCanvasDefY(0);

// For the Pad:
  tdrStyle->SetPadBorderMode(0);
  // tdrStyle->SetPadBorderSize(Width_t size = 1);
  tdrStyle->SetPadColor(kWhite);
  tdrStyle->SetPadGridX(false);
  tdrStyle->SetPadGridY(false);
  tdrStyle->SetGridColor(0);
  tdrStyle->SetGridStyle(3);
  tdrStyle->SetGridWidth(1);

// For the frame:
  tdrStyle->SetFrameBorderMode(0);
  tdrStyle->SetFrameBorderSize(1);
  tdrStyle->SetFrameFillColor(0);
  tdrStyle->SetFrameFillStyle(0);
  tdrStyle->SetFrameLineColor(1);
  tdrStyle->SetFrameLineStyle(1);
  tdrStyle->SetFrameLineWidth(1);

// For the histo:
  // tdrStyle->SetHistFillColor(1);
  // tdrStyle->SetHistFillStyle(0);
  tdrStyle->SetHistLineColor(1);
  tdrStyle->SetHistLineStyle(0);
  tdrStyle->SetHistLineWidth(1);
  // tdrStyle->SetLegoInnerR(Float_t rad = 0.5);
  // tdrStyle->SetNumberContours(Int_t number = 20);

  tdrStyle->SetEndErrorSize(2);
  //  tdrStyle->SetErrorMarker(20);
  tdrStyle->SetErrorX(0.);
  
  //tdrStyle->SetMarkerStyle(20);
  //tdrStyle->SetMarkerStyle(7);

//For the fit/function:
  tdrStyle->SetOptFit(1); // To turn of the fit statistics: SetOptFit(0);
  // tdrStyle->SetOptFit(2211);
  tdrStyle->SetFitFormat("5.4g");
  tdrStyle->SetFuncColor(2);
  tdrStyle->SetFuncStyle(1);
  tdrStyle->SetFuncWidth(1);

//For the date:
  tdrStyle->SetOptDate(0);
  // tdrStyle->SetDateX(Float_t x = 0.01);
  // tdrStyle->SetDateY(Float_t y = 0.01);

// For the statistics box:
  tdrStyle->SetOptFile(0);
  tdrStyle->SetOptStat(0); // To display the mean and RMS:   SetOptStat("mr");
  // tdrStyle->SetOptStat("neMR"); // To display without the name:  SetOptstat("eMR");
  // tdrStyle->SetStatColor(kWhite);
  tdrStyle->SetStatColor(0);
  tdrStyle->SetStatFont(42);
  tdrStyle->SetStatFontSize(0.025);
  tdrStyle->SetStatTextColor(1);
  tdrStyle->SetStatFormat("6.4g");
  tdrStyle->SetStatBorderSize(1);
  tdrStyle->SetStatH(0.1);
  tdrStyle->SetStatW(0.15);
  // tdrStyle->SetStatStyle(Style_t style = 1001);
  // tdrStyle->SetStatX(Float_t x = 0);
  // tdrStyle->SetStatY(Float_t y = 0);

// for the legend:
  tdrStyle->SetLegendBorderSize(0);

// Margins:
  tdrStyle->SetPadTopMargin(0.05);
  tdrStyle->SetPadBottomMargin(0.13);
  tdrStyle->SetPadLeftMargin(0.16);
  // tdrStyle->SetPadRightMargin(0.02);
  tdrStyle->SetPadRightMargin(0.08);

// For the Global title:

  tdrStyle->SetOptTitle(0);
  tdrStyle->SetTitleFont(42);
  // tdrStyle->SetTitleColor(1);
  tdrStyle->SetTitleColor(0,"c");
  tdrStyle->SetTitleTextColor(1);
  // tdrStyle->SetTitleFillColor(10);
  tdrStyle->SetTitleFillColor(0);
  tdrStyle->SetTitleFontSize(0.05);
  // tdrStyle->SetTitleH(0); // Set the height of the title box
  // tdrStyle->SetTitleW(0); // Set the width of the title box
  // tdrStyle->SetTitleX(0); // Set the position of the title box
  // tdrStyle->SetTitleY(0.985); // Set the position of the title box
  // tdrStyle->SetTitleStyle(Style_t style = 1001);
  // tdrStyle->SetTitleBorderSize(2);

// For the axis titles:

  tdrStyle->SetTitleColor(1, "XYZ");
  tdrStyle->SetTitleFont(42, "XYZ");
  tdrStyle->SetTitleSize(0.06, "XYZ");
  // tdrStyle->SetTitleXSize(Float_t size = 0.02); // Another way to set the size?
  // tdrStyle->SetTitleYSize(Float_t size = 0.02);
  tdrStyle->SetTitleXOffset(1.0);
  tdrStyle->SetTitleYOffset(1.25);
  // tdrStyle->SetTitleOffset(1.3, "X"); // Another way to set the Offset
  // tdrStyle->SetTitleOffset(1.3, "Y"); // Another way to set the Offset

// For the axis labels:

  tdrStyle->SetLabelColor(1, "XYZ");
  tdrStyle->SetLabelFont(42, "XYZ");
  tdrStyle->SetLabelOffset(0.007, "XYZ");
  tdrStyle->SetLabelSize(0.05, "XYZ");

// For the axis:

  tdrStyle->SetAxisColor(1, "XYZ");
  tdrStyle->SetStripDecimals(kTRUE);
  tdrStyle->SetTickLength(0.03, "XYZ");
  tdrStyle->SetNdivisions(510, "XYZ");
  tdrStyle->SetPadTickX(1);  // To get tick marks on the opposite side of the frame
  tdrStyle->SetPadTickY(1);

// Change for log plots:
  tdrStyle->SetOptLogx(0);
  tdrStyle->SetOptLogy(0);
  tdrStyle->SetOptLogz(0);

// Postscript options:
  tdrStyle->SetPaperSize(20.,20.);
  // tdrStyle->SetLineScalePS(Float_t scale = 3);
  // tdrStyle->SetLineStyleString(Int_t i, const char* text);
  // tdrStyle->SetHeaderPS(const char* header);
  // tdrStyle->SetTitlePS(const char* pstitle);

  // tdrStyle->SetBarOffset(Float_t baroff = 0.5);
  // tdrStyle->SetBarWidth(Float_t barwidth = 0.5);
  // tdrStyle->SetPaintTextFormat(const char* format = "g");
  // tdrStyle->SetPalette(Int_t ncolors = 0, Int_t* colors = 0);
  tdrStyle->SetPalette(1);
  // tdrStyle->SetTimeOffset(Double_t toffset);
  // tdrStyle->SetHistMinimumZero(kTRUE);

  tdrStyle->cd();
  
  return tdrStyle;
}

// sets the style for the canvas if the plots need to be in tdr style
void setStyle() {
   TStyle* tdrStyle = getTDRStyle();
   gROOT->SetStyle(tdrStyle->GetName());
}

void cmsPrel(double intLumi=-1, bool wide = false) {

  TLatex *latex = new TLatex();
  latex->SetNDC();
  latex->SetTextSize(0.045);
  
  latex->SetTextAlign(31); // align right
  latex->DrawLatex(wide ? 0.98 : 0.95, 0.96, "#sqrt{s} = 8 TeV");
  if (intLumi > 0.) {
    latex->SetTextAlign(11); // align left
    latex->DrawLatex(wide ? 0.06 : 0.15, 0.96,
         Form("CMS preliminary, L = %.3g fb^{-1}",intLumi*0.001));
  }
  else if (intLumi==0) { // simulation
    latex->SetTextAlign(11); // align left
    //latex->DrawLatex(wide ? 0.06 : 0.15, 0.96, "CMS simulation (Summer12)");
    latex->DrawLatex(wide ? 0.06 : 0.15, 0.96, "CMS Simulation");
  }
  else {
    latex->SetTextAlign(11); // align left
    latex->DrawLatex(0.15,0.96,"CMS preliminary 2012");
  }
} // cmsPrel

void cmsFinal(double intLumi=-1, bool wide = false) {

  TLatex *latex = new TLatex();
  latex->SetNDC();
  latex->SetTextSize(0.045);
  
  latex->SetTextAlign(31); // align right
  latex->DrawLatex(wide ? 0.98 : 0.95, 0.96, "#sqrt{s} = 8 TeV");
  if (intLumi > 0.) {
    latex->SetTextAlign(11); // align left
    latex->DrawLatex(wide ? 0.06 : 0.15, 0.96,
         Form("CMS, L = %.2g fb^{-1}",intLumi*0.001));
  }
  else if (intLumi==0) { // simulation
    latex->SetTextAlign(11); // align left
    latex->DrawLatex(wide ? 0.06 : 0.15, 0.96, "CMS simulation (Pythia Z2*)");
  }
  else {
    latex->SetTextAlign(11); // align left
    latex->DrawLatex(0.15,0.96,"CMS 2012");
  }
} // cmsPrel

//cmsPrel(); // to print just CMS and \sqrt{s}
//cmsPrel(400);  // to print also the integrated luminosity.

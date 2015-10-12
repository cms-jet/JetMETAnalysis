////////////////////////////////////////////////////////////////////////////////
//
// RootStyle
// ---------
//            07/29/2008 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
//            09/24/2015 Alexx Perloff           <alexx.stephen.perloff@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/RootStyle.h"

#include <TROOT.h>
#include <TStyle.h>


//______________________________________________________________________________
void set_root_style()
{
  gStyle->SetTextFont        (42);
  
  // Canvas
  gStyle->SetCanvasColor     (0);
  gStyle->SetCanvasBorderSize(10);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasDefH      (800);
  gStyle->SetCanvasDefW      (1000);
  gStyle->SetCanvasDefX      (10);
  gStyle->SetCanvasDefY      (10);
  
  // Pads
  gStyle->SetPadColor       (0);
  gStyle->SetPadBorderSize  (10);
  gStyle->SetPadBorderMode  (0);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetPadTopMargin   (0.05);
  gStyle->SetPadLeftMargin  (0.15);
  gStyle->SetPadRightMargin (0.07);
  gStyle->SetPadGridX       (0);
  gStyle->SetPadGridY       (0);
  gStyle->SetPadTickX       (1);
  gStyle->SetPadTickY       (1);
  
  
  // Frames
  gStyle->SetFrameFillStyle (0);
  gStyle->SetFrameFillColor (kWhite);
  gStyle->SetFrameLineColor (kBlack);
  gStyle->SetFrameLineStyle (0);
  gStyle->SetFrameLineWidth (1);
  gStyle->SetFrameBorderSize(10);
  gStyle->SetFrameBorderMode(0);
  
  // Histograms
  gStyle->SetHistFillColor(kWhite);
  gStyle->SetHistFillStyle(0);
  gStyle->SetHistLineColor(kBlack);
  gStyle->SetHistLineStyle(0);
  gStyle->SetHistLineWidth(1);
  gStyle->SetEndErrorSize(0);
  
  // Functions
  gStyle->SetFuncColor(1);
  gStyle->SetFuncStyle(0);
  gStyle->SetFuncWidth(1);
  
  // Markers
  gStyle->SetMarkerStyle(0); 
  gStyle->SetMarkerSize(1);
  
  // Various
  gStyle->SetTickLength (0.03 ,"XYZ");
  gStyle->SetTitleSize  (0.055,"XYZ");
  gStyle->SetTitleFont  (42   ,"XYZ");
  gStyle->SetLabelFont  (42   ,"XYZ");
  gStyle->SetNdivisions (405  ,"XYZ");
  
  gStyle->SetTitleOffset(1.15,"X");
  gStyle->SetTitleOffset(1.25,"Y");
  gStyle->SetTitleOffset(1.15,"Z");
  
  gStyle->SetLabelOffset(0.004,"X");
  gStyle->SetLabelOffset(0.015,"Y");
  gStyle->SetLabelOffset(0.015,"Z");
  
  gStyle->SetLabelSize  (0.055,"X");
  gStyle->SetLabelSize  (0.055,"Y");
  gStyle->SetLabelSize  (0.055,"Z");
  
  gStyle->SetStatFont   (42);
  gStyle->SetTitleFont  (42);
  
  // Options
  gStyle->SetOptFit     (0);
  gStyle->SetOptStat    (1111);
  
  gROOT->ForceStyle();
  
  return;
}


//______________________________________________________________________________
void set_tdr_style()
{
  // For the canvas:
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasColor(kWhite);
  gStyle->SetCanvasDefH(600);
  gStyle->SetCanvasDefW(600);
  gStyle->SetCanvasDefX(0);
  gStyle->SetCanvasDefY(0);
  
  // For the Pad:
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadColor(kWhite);
  gStyle->SetPadGridX(false);
  gStyle->SetPadGridY(false);
  gStyle->SetGridColor(0);
  gStyle->SetGridStyle(3);
  gStyle->SetGridWidth(1);

  // For the frame:
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameBorderSize(1);
  gStyle->SetFrameFillColor(0);
  gStyle->SetFrameFillStyle(0);
  gStyle->SetFrameLineColor(1);
  gStyle->SetFrameLineStyle(1);
  gStyle->SetFrameLineWidth(1);
  
  // For the histo:
  gStyle->SetHistLineColor(1);
  gStyle->SetHistLineStyle(0);
  gStyle->SetHistLineWidth(1);

  
  gStyle->SetEndErrorSize(2);
  gStyle->SetErrorX(0.);
  
  gStyle->SetMarkerStyle(20);
  
  //For the fit/function:
  gStyle->SetOptFit(1);
  gStyle->SetFitFormat("5.4g");
  gStyle->SetFuncColor(2);
  gStyle->SetFuncStyle(1);
  gStyle->SetFuncWidth(1);
  
  //For the date:
  gStyle->SetOptDate(0);

  // For the statistics box:
  gStyle->SetOptFile(0);
  gStyle->SetOptStat(0); // To display the mean and RMS:   SetOptStat("mr");
  gStyle->SetStatColor(kWhite);
  gStyle->SetStatFont(42);
  gStyle->SetStatFontSize(0.025);
  gStyle->SetStatTextColor(1);
  gStyle->SetStatFormat("6.4g");
  gStyle->SetStatBorderSize(1);
  gStyle->SetStatH(0.1);
  gStyle->SetStatW(0.15);
  // gStyle->SetStatStyle(Style_t style = 1001);
  // gStyle->SetStatX(Float_t x = 0);
  // gStyle->SetStatY(Float_t y = 0);
  
  // Margins:
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.13);
  gStyle->SetPadLeftMargin(0.16);
  gStyle->SetPadRightMargin(0.02);
  
  // For the Global title:
  gStyle->SetOptTitle(0);
  gStyle->SetTitleFont(42);
  gStyle->SetTitleColor(1);
  gStyle->SetTitleTextColor(1);
  gStyle->SetTitleFillColor(10);
  gStyle->SetTitleFontSize(0.05);
  // gStyle->SetTitleH(0); // Set the height of the title box
  // gStyle->SetTitleW(0); // Set the width of the title box
  // gStyle->SetTitleX(0); // Set the position of the title box
  // gStyle->SetTitleY(0.985); // Set the position of the title box
  // gStyle->SetTitleStyle(Style_t style = 1001);
  // gStyle->SetTitleBorderSize(2);
  
  // For the axis titles:
  
  gStyle->SetTitleColor(1, "XYZ");
  gStyle->SetTitleFont(42, "XYZ");
  gStyle->SetTitleSize(0.06, "XYZ");
  // gStyle->SetTitleXSize(Float_t size = 0.02); // Another way to set the size?
  // gStyle->SetTitleYSize(Float_t size = 0.02);
  gStyle->SetTitleXOffset(0.9);
  gStyle->SetTitleYOffset(1.25);
  // gStyle->SetTitleOffset(1.1, "Y"); // Another way to set the Offset
  
  // For the axis labels:
  
  gStyle->SetLabelColor(1, "XYZ");
  gStyle->SetLabelFont(42, "XYZ");
  gStyle->SetLabelOffset(0.007, "XYZ");
  gStyle->SetLabelSize(0.05, "XYZ");
  
  // For the axis:
  
  gStyle->SetAxisColor(1, "XYZ");
  gStyle->SetStripDecimals(kTRUE);
  gStyle->SetTickLength(0.03, "XYZ");
  gStyle->SetNdivisions(510, "XYZ");
  gStyle->SetPadTickX(1);  // To get tick marks on the opposite side of the frame
  gStyle->SetPadTickY(1);
  
  // Change for log plots:
  gStyle->SetOptLogx(0);
  gStyle->SetOptLogy(0);
  gStyle->SetOptLogz(0);
  
  // Postscript options:
  gStyle->SetPaperSize(20.,20.);
  // gStyle->SetLineScalePS(Float_t scale = 3);
  // gStyle->SetLineStyleString(Int_t i, const char* text);
  // gStyle->SetHeaderPS(const char* header);
  // gStyle->SetTitlePS(const char* pstitle);
  
  // gStyle->SetBarOffset(Float_t baroff = 0.5);
  // gStyle->SetBarWidth(Float_t barwidth = 0.5);
  // gStyle->SetPaintTextFormat(const char* format = "g");
  // gStyle->SetPalette(Int_t ncolors = 0, Int_t* colors = 0);
  // gStyle->SetTimeOffset(Double_t toffset);
  // gStyle->SetHistMinimumZero(kTRUE);
  
  gROOT->ForceStyle();
}


//______________________________________________________________________________
void set_vasu_style()
{
  gROOT->SetStyle("Plain");

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  gStyle->SetPalette(1);
  gStyle->SetTextFont(42);
  
  // Canvas
  gStyle->SetCanvasColor     (0);
  gStyle->SetCanvasBorderSize(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasDefH      (800);
  gStyle->SetCanvasDefW      (1000);
  gStyle->SetCanvasDefX      (10);
  gStyle->SetCanvasDefY      (10);
  
  // Pads
  gStyle->SetPadColor       (0);
  gStyle->SetPadBorderSize  (10);
  gStyle->SetPadBorderMode  (0);
  gStyle->SetPadTopMargin   (0.04);
  gStyle->SetPadBottomMargin(0.16);
  gStyle->SetPadLeftMargin  (0.15);
  gStyle->SetPadRightMargin (0.04);
  gStyle->SetPadGridX       (0);
  gStyle->SetPadGridY       (0);
  gStyle->SetPadTickX       (1);
  gStyle->SetPadTickY       (1);
  
  // Frames
  gStyle->SetFrameFillStyle (0);
  gStyle->SetFrameFillColor (kWhite);
  gStyle->SetFrameLineColor (kBlack);
  gStyle->SetFrameLineStyle (0);
  gStyle->SetFrameLineWidth (1);
  gStyle->SetFrameBorderSize(0);
  gStyle->SetFrameBorderMode(0);
  
  // Histograms
  gStyle->SetHistFillColor(kWhite);
  gStyle->SetHistFillStyle(0);
  gStyle->SetHistLineColor(kBlack);
  gStyle->SetHistLineStyle(0);
  gStyle->SetHistLineWidth(1);
  
  // Functions
  gStyle->SetFuncColor(1);
  gStyle->SetFuncStyle(0);
  gStyle->SetFuncWidth(1);
  
  // Markers
  gStyle->SetMarkerStyle(0); 
  gStyle->SetMarkerSize(1);
  
  // Various
  gStyle->SetStripDecimals(false);
  gStyle->SetTickLength (0.03 ,"XYZ");
  gStyle->SetTitleFont  (42   ,"XYZ");
  gStyle->SetLabelFont  (42   ,"XYZ");
  gStyle->SetNdivisions (505  ,"XYZ");
  
  gStyle->SetTitleOffset(1.1,"X");
  gStyle->SetTitleOffset(0.9,"Y");
  gStyle->SetTitleOffset(0.9,"Z");
  
  gStyle->SetLabelOffset(0.004,"X");
  gStyle->SetLabelOffset(0.015,"Y");
  gStyle->SetLabelOffset(0.015,"Z");
  
  gStyle->SetLabelSize  (0.06,"X");
  gStyle->SetLabelSize  (0.06,"Y");
  gStyle->SetLabelSize  (0.06,"Z");

  gStyle->SetTitleSize  (0.065,"X");
  gStyle->SetTitleSize  (0.080,"Y");
  gStyle->SetTitleSize  (0.080,"Z");
  
  gStyle->SetStatFont   (42);
  gStyle->SetTitleFont  (42);
  
  gROOT->ForceStyle();
  
  return;
}


//______________________________________________________________________________
void setTDRStyle()
{
  TStyle *tdrStyle = new TStyle("tdrStyle","Style for P-TDR");

// For the canvas:
  tdrStyle->SetCanvasBorderMode(0);
  tdrStyle->SetCanvasColor(kWhite);
  tdrStyle->SetCanvasDefH(600); //Height of canvas
  tdrStyle->SetCanvasDefW(600); //Width of canvas
  tdrStyle->SetCanvasDefX(0);   //POsition on screen
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
  // tdrStyle->SetErrorMarker(20);
  //tdrStyle->SetErrorX(0.);
  
  tdrStyle->SetMarkerStyle(20);
  
//For the fit/function:
  tdrStyle->SetOptFit(1);
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
  tdrStyle->SetStatColor(kWhite);
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

// Margins:
  tdrStyle->SetPadTopMargin(0.05);
  tdrStyle->SetPadBottomMargin(0.13);
  tdrStyle->SetPadLeftMargin(0.16);
  tdrStyle->SetPadRightMargin(0.02);

// For the Global title:

  tdrStyle->SetOptTitle(0);
  tdrStyle->SetTitleFont(42);
  tdrStyle->SetTitleColor(1);
  tdrStyle->SetTitleTextColor(1);
  tdrStyle->SetTitleFillColor(10);
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
  tdrStyle->SetTitleXOffset(0.9);
  tdrStyle->SetTitleYOffset(1.25);
  // tdrStyle->SetTitleOffset(1.1, "Y"); // Another way to set the Offset

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
  // tdrStyle->SetTimeOffset(Double_t toffset);
  // tdrStyle->SetHistMinimumZero(kTRUE);

  tdrStyle->SetHatchesLineWidth(5);
  tdrStyle->SetHatchesSpacing(0.05);

  tdrStyle->cd();

  return;
}

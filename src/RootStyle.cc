////////////////////////////////////////////////////////////////////////////////
//
// RootStyle
// ---------
//
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/RootStyle.h"

#include <TStyle.h>


//______________________________________________________________________________
void set_root_style()
{
  gStyle->SetTextFont        (52);
  
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
  gStyle->SetTitleFont  (52   ,"XYZ");
  gStyle->SetLabelFont  (52   ,"XYZ");
  gStyle->SetNdivisions (405  ,"XYZ");
  
  gStyle->SetTitleOffset(1.150,"X");
  gStyle->SetTitleOffset(1.500,"Y");
  gStyle->SetTitleOffset(1.150,"Z");
  
  gStyle->SetLabelOffset(0.004,"X");
  gStyle->SetLabelOffset(0.015,"Y");
  gStyle->SetLabelOffset(0.015,"Z");
  
  gStyle->SetLabelSize  (0.055,"X");
  gStyle->SetLabelSize  (0.055,"Y");
  gStyle->SetLabelSize  (0.055,"Z");
  
  gStyle->SetStatFont   (52);
  gStyle->SetTitleFont  (52);
  
  // Options
  gStyle->SetOptFit     (0);
  gStyle->SetOptStat    (1111);

  return;
}

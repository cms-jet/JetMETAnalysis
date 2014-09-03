///////////////////////////////////////////////////////////////////
//
// jet_draw_L3Rsp_RefPt_Ratio_x
// ----------------------------
//
//            09/01/2011 Alexx Perloff  <aperloff@physics.tamu.edu>
///////////////////////////////////////////////////////////////////

#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"

#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TMath.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TAxis.h"
#include "TLegend.h"

#include <vector>
#include <string>
#include <iomanip>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv)
{
  gSystem->Load("libFWCoreFWLite.so");
  
  //
  // evaluate command-line / configuration file options
  // 
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;
  
  TString        algo         = cl.getValue<TString> ("algo");
  vector<string> filenameNum  = cl.getVector<string> ("filenameNum");
  vector<string> filenameDen  = cl.getVector<string> ("filenameDen");
  string         path         = cl.getValue<string>  ("path",             "");
  TString        outputDir    = cl.getValue<TString> ("outputDir",  "images");
  TString        outputFormat = cl.getValue<TString> ("outputFormat", ".png");
  bool           errOverSq2   = cl.getValue<bool>    ("errOverSq2",    false);

  if (!cl.check()) return 0;
  cl.print();

  if(filenameNum.size()!=filenameDen.size())
    {
      cout << "The number of numerator files is not equal to the number of denominator files" << endl
           << "Please make sure there are an equal number of files in each and try again." << endl
           << "The program will now exit" << endl;
      return 0;
    }

  //
  // set path to input files
  //
  if(path.empty()) path = string (gSystem->pwd())+"/";

  //
  // open/create the output directory and file
  //
  if(!gSystem->OpenDirectory(outputDir)) gSystem->mkdir(outputDir);
  TString ss = "L3RspVsRefPtRatio_"+algo;
  TString ofname = outputDir+ss+".root";
  TFile* outf = new TFile(ofname,"UPDATE");

  //
  // book containers and canvases
  //
  vector<TGraphErrors*> ratioGraphs;
  TCanvas* can = new TCanvas("can","can",800,800);
  TLegend* leg = new TLegend(0.65,0.6,1.0,0.8);

  for(unsigned int f=0; f<filenameNum.size(); f++)
    {
      //
      // read in original L3RspVsRefPt graphs
      //
      TFile* file1 = new TFile((path+filenameNum[f]).c_str(),"READ");
      TDirectoryFile *idir = (TDirectoryFile*)file1->Get(algo);
      TGraphErrors* graphNum = (TGraphErrors*)idir->Get("L3RspVsRefPt");

      TFile* file2 = new TFile((path+filenameDen[f]).c_str(),"READ");
      idir = (TDirectoryFile*)file2->Get(algo);
      TGraphErrors* graphDen = (TGraphErrors*)gDirectory->Get("L3RspVsRefPt");

      if(graphNum==0 || graphDen==0)
        {
          cout << "One or more of the graph pointers from files " << filenameNum[f] << "and" << filenameDen[f] 
               << " is NULL." << endl << "Please correct this and try again" << endl 
               << "The program will now exit" << endl;
          return 0;
        }

      ratioGraphs.push_back(new TGraphErrors(graphDen->GetN()));

      //
      // setup the arrays of point values
      //
      Double_t* numX = graphNum->GetX();
      Double_t* numY = graphNum->GetY();
      Double_t* numEX = graphNum->GetEX();
      Double_t* numEY = graphNum->GetEY();
      //Double_t* denX = graphDen->GetX();
      Double_t* denY = graphDen->GetY();
      //Double_t* denEX = graphDen->GetEX();
      Double_t* denEY = graphDen->GetEY();

      //
      // set graph points
      //
      for(int i=0; i<graphDen->GetN(); i++)
        {
          //
          // calculate the y error of the ratio
          //
          double ptyerr = 0;
          ptyerr = (TMath::Power(1.0/denY[i],2)*TMath::Power(numEY[i],2));
          ptyerr += (TMath::Power(-numY[i]/TMath::Power(denY[i],2),2)*TMath::Power(denEY[i],2));
          ptyerr = TMath::Sqrt(ptyerr);
          //
          //Correction for not having both particle and antiparticle statistics
          //
          if(errOverSq2) ptyerr /= TMath::Sqrt(2);

          ratioGraphs.back()->SetPoint(i,numX[i],numY[i]/denY[i]);
          ratioGraphs.back()->SetPointError(i,numEX[i],ptyerr);
          cout << "Point: " << i << "\tX: " << numX[i] << " +- " << numEX[i];
          cout << "\tY: " << numY[i]/denY[i] << " +- " << ptyerr;
          cout << endl;
        }

      //
      // find naming identifiers (ex: flavor, algo, etc.)
      //
      TString num = filenameNum[f].substr(int(filenameNum[f].rfind("_")+1),
                                          int(filenameNum[f].rfind(".root")-filenameNum[f].rfind("_")-1));
      TString den = filenameDen[f].substr(int(filenameDen[f].rfind("_")+1),
                                          int(filenameDen[f].rfind(".root")-filenameDen[f].rfind("_")-1));

      //
      // format graphs
      //
      can->cd();
      ratioGraphs.back()->SetTitle("Ratio of L3RspVsRefPt for #frac{"+num+"}{"+den+"} ("+algo+")");
      gStyle->SetTitleW(0.7);  
      gStyle->SetTitleH(0.08);
      ratioGraphs.back()->GetXaxis()->SetTitle("RefPt (GeV)");
      ratioGraphs.back()->GetXaxis()->SetLabelSize(0.03);
      ratioGraphs.back()->GetXaxis()->SetTitleOffset(1.2);
      ratioGraphs.back()->GetYaxis()->SetTitle("Response Ratio");
      ratioGraphs.back()->GetYaxis()->SetLabelSize(0.03);
      ratioGraphs.back()->GetYaxis()->SetTitleOffset(1.2);
      ratioGraphs.back()->SetName(ss+"_"+num+"over"+den);

      if(f==0)
        {
          ratioGraphs.back()->SetTitle("Ratio of L3RspVsRefPt ("+algo+")");
          ratioGraphs.back()->SetMinimum(0.9);
          ratioGraphs.back()->SetMaximum(1.15);
          ratioGraphs.back()->Draw("AP");
        }
      else
        {
          ratioGraphs.back()->SetMarkerColor(f+1);
          ratioGraphs.back()->SetLineColor(f+1);
          ratioGraphs.back()->Draw("P");
        }
      leg->AddEntry(ratioGraphs.back(),ratioGraphs.back()->GetName(),"le");

      outf->cd();
      ratioGraphs.back()->Write();
    }
  can->SetLogx();
  leg->SetFillColor(0);
  leg->Draw();

  //
  // save output
  //
  outf->cd();
  can->Write();
  can->SaveAs(outputDir+"/"+ss+outputFormat);

  outf->Close();
}

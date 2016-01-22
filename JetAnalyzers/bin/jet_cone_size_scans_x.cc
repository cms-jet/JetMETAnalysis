///////////////////////////////////////////////////////////////////
//
// jet_cone_size_scans_x
// ---------------------
//
//            12/09/2011 Alexx Perloff  <aperloff@physics.tamu.edu>
///////////////////////////////////////////////////////////////////

#include "JetMETAnalysis/JetUtilities/interface/Style.h"
#include "JetMETAnalysis/JetUtilities/interface/JetInfo.hh"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetAnalyzers/interface/REStyle.h"

#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TMultiGraph.h"
#include "TF1.h"
#include "TString.h"
#include "TObjString.h"
#include "TObjArray.h"
#include "TPaveText.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TRegexp.h"
#include "TAxis.h"
#include "TGaxis.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <stdlib.h>
#include <utility>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// define local functions
////////////////////////////////////////////////////////////////////////////////

/// Open the input files/cone sizes created by jet_synchplot_x
TString openInputFiles(map<JetInfo,TFile*>& ifiles, TString path, vector<TString> algs, TString flavor);

/// Write to the output file and save the output canvases
void writeToOutputFileAndSaveCanvases(map<TString,TCanvas*>& ocanvases, TString ioutputDir, vector<TString> outputFormat);

/// Get the input histograms from jet_synchplot_x
void getInputHistograms(map<JetInfo,TFile*>& ifiles, vector<TString> plot, map<TString,TH1D*>& ihistograms);
void getInputHistograms(map<JetInfo,TFile*>& ifiles, TString plot, vector<TString> hname, map<TString,TH1D*>& ihistograms);

/// Find the optimal cone size
void doOptimalConeSizeScans(map<JetInfo,TFile*>& ifiles, vector<double> scanPts, vector<int> npvRhoNpuBins, double binWidth,
                            TString outputDir, vector<TString> outputFormat, TString algsString, bool versionB);

/// Combine the DeltaR plots
void combineCSPlots(map<JetInfo,TFile*>& ifiles, TString outputDir, vector<TString> outputFormat, TString algsString);

/// get the colors for the 8 cone sizes
vector<Int_t> getColors();

/// get the marker styles for the 8 cone sizes
vector<Int_t> getMarkerNumbers();

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv)
{
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);

  gSystem->Load("libFWCoreFWLite.so");
  
  //
  // evaluate command-line / configuration file options
  // 
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;
  
  vector<TString> algs                = cl.getVector<TString> ("algs");
  TString         flavor              = cl.getValue<TString>  ("flavor",                                              "");
  TString         path                = cl.getValue<TString>  ("path",                                                "");
  TString         outputDir           = cl.getValue<TString>  ("outputDir",                                         "./");
  vector<TString> outputFormat        = cl.getVector<TString> ("outputFormat",                             ".png:::.eps");
  bool            tdr                 = cl.getValue<bool>     ("tdr",                                              false);
  vector<int>     npvRhoNpuBins       = cl.getVector<int>     ("npvRhoNpuBin",                       "0:::1:::2:::3:::4");
  double          binWidth            = cl.getValue<double>   ("binWidth",                                             5);
  vector<double>  scanPts             = cl.getVector<double>  ("scanPts", "30.0:::60.0:::100.0:::250.0:::500.0:::1000.0");
  vector<TString> plots               = cl.getVector<TString> ("plots", "OffMeanrhoRef:::"//"OffMeanOverPtrhoRef:::"
                                                              "ResolutionRho:::ResolutionRhoRef:::OffResolutionRhoRef:::"
                                                              "OffMeannpvRef:::"//"OffMeanOverPtnpvRef:::"
                                                              "OffResolutionnpvRef");
  bool            findOptimalConeSize = cl.getValue<bool>     ("findOptimalConeSize",                              false);
  bool            versionB            = cl.getValue<bool>     ("versionB",                                         false);
  bool            combineCS           = cl.getValue<bool>     ("combineCS",                                        false);

  if (!cl.check()) return 0;
  cl.print();

  if (tdr) {
     setTDRStyle();
  }

  int colDet[4] = {kGreen+3,kBlue+1,kYellow+2,kRed+1}; //bb,ei,eo,ff

  //
  // Open the input files
  //
  map<JetInfo,TFile*> ifiles;
  if(path.IsNull()) path = string (gSystem->pwd())+"/";
  if(!path.EndsWith("/")) path+="/";
  TString algsString = openInputFiles(ifiles,path,algs,flavor);

  if(findOptimalConeSize) {
    doOptimalConeSizeScans(ifiles,scanPts,npvRhoNpuBins,binWidth,outputDir,outputFormat,algsString,versionB);
    return 0;
  }
  if(combineCS) {
    combineCSPlots(ifiles,outputDir,outputFormat,algsString);
    return 0;
  }

  map<TString,TH1D*> ihistograms;
  map<TString, TCanvas*> ocanvases;
  map<TString, TGraphErrors*> ographs;
  //
  // loop over the pts to scan
  //
  for(unsigned int iscanPt=0; iscanPt<scanPts.size(); iscanPt++) {
    //
    // loop over the npv/rho/npu bins  
    //
    for(unsigned int inpvRhoNpuBin=0; inpvRhoNpuBin<npvRhoNpuBins.size(); inpvRhoNpuBin++) {
      //
      // loop over the plots
      //
      for(unsigned int iplot=0; iplot<plots.size(); iplot++) {
        cout << "\t" << string(13+plots[iplot].Length(),'*') << endl
        << "\t*Doing plot " << plots[iplot] << "*" << endl
        << "\t" << string(13+plots[iplot].Length(),'*') << endl;
        //
        // Grab the necessary histograms
        //
        cout << "\t\tGetting the input histograms ... " << endl;
        cout << "\t\t\tBooking the graphs ... ";
        for(map<JetInfo,TFile*>::iterator it=ifiles.begin(); it!=ifiles.end(); ++it) {
          it->second->cd();
  
          //
          // Booking based on detector sections
          //
          for(int dn=0; dn<NDetectorNames; dn++) {
            TString icanvasName = Form(plots[iplot]+"_%s",JetInfo::get_detector_abbreviation(detector_names[dn]).Data());
            //cout << canvasName << endl;
            TString ihistogramName = Form(icanvasName+"_%i",npvRhoNpuBins[inpvRhoNpuBin]);
            TString mapName = Form(ihistogramName+"_%s",it->first.abbreviation.Data());
            //cout << "mapName = " << mapName << "\ticanvasName = " << icanvasName << "\tihistogramName = " << ihistogramName << endl;
            ihistograms[mapName] = (TH1D*)((TCanvas*)gDirectory->Get(icanvasName))->GetPrimitive(ihistogramName);
            TString ihistogramTitle = ihistograms[mapName]->GetTitle();

            if(it==ifiles.begin()) {
              //
              // book the necessary output canvases & histograms
              //
              if(dn==0) {
                TString ocanvasName = plots[iplot];
                TString ocanvasTitle = ihistogramTitle+" Vs. Cone Size";
                ocanvases[ocanvasName] = new TCanvas(ocanvasName,ocanvasTitle,700,500);
              }
              TString histogramName = Form(plots[iplot]+"_%s",JetInfo::get_detector_abbreviation(detector_names[dn]).Data());
              TString histogramTitle = Form(ihistogramTitle+" Vs. Cone Size (%s)",detector_names[dn].Data());
              //ographs[histogramName] = new TH1D(histogramName,histogramTitle,algs.size(),ifiles.begin()->first.coneSize,ifiles.rbegin()->first.coneSize);
              ographs[histogramName] = new TGraphErrors();
              ographs[histogramName]->SetNameTitle(histogramName,histogramTitle);

              //histogramName = Form(plots[iplot]+"_optimalConeSize_%s"JetInfo::get_detector_abbreviation(detector_names[dn]).Data());
              //TString rhoNpv;
              //if(plots[iplot].Contains("npv")) rhoNpv = "NPV";
              //else if(plots[iplot].Contains("Rho")) rhoNpv = "#rho";
              //histogramTitle = Form("Optimal Cone Size Vs. "+rhoNpv+" (ihistogramTitle, %s)",detector_names[dn].Data());
              //ographs[histogramName] = new TGraphErrors();
              //ographs[histogramName]->SetNameTitle(histogramName,histogramTitle);

              if(dn==NDetectorNames-1)
                cout << "DONE";
            }
          }
        }
        cout << "\x1b[A" << "\r\t\tGetting the input histograms ... DONE" << endl << endl;
  
        //
        // perform a scan on the requested parameter
        //
        TString canvasName = plots[iplot];
        cout << "\t\tDoing the scan ... ";
        TMultiGraph* mg = new TMultiGraph();
        TLegend* leg;
        //TLegend* leg = new TLegend(0.15,0.15,0.5,0.4);
        if(canvasName.Contains("Resolution"))
          //leg = new TLegend(0.68,0.15,0.88,0.5);
          leg = new TLegend(0.2,0.6,0.75,0.92);
        else
          leg = new TLegend(0.18,0.15,0.38,0.5);
        leg->SetFillColor(0);
        leg->SetBorderSize(0);
        leg->SetFillStyle(0);
        leg->SetNColumns(2);
        leg->SetTextSize(0.035);
        TGraph* dummy = 0;
        ocanvases[canvasName]->cd();
        TString imapName;
        for(int dn=0; dn<NDetectorNames; dn++) {
          if(dn==0)      
            leg->AddEntry(dummy,"QCD Multijet","");
          else if(dn==1) 
            leg->AddEntry(dummy,JetInfo::get_legend_title(string(ifiles.begin()->first.abbreviation),false).c_str(),"");
          /*else if(dn==2) {
            std::ostringstream ss;
            ss.precision(1);
            ss << fixed << scanPts[iscanPt];
            leg->AddEntry(dummy,TString("p_{T}^{GEN} = ") + ss.str().c_str() + " [GeV]","");
          }*/
          else if (dn==2) {
            ;
          }
          else if(dn==3) {
            if(plots[iplot].Contains("rho") || plots[iplot].Contains("Rho"))
              leg->AddEntry(dummy,JetInfo::getBinLegendEntry(npvRhoNpuBins[inpvRhoNpuBin],"#rho",npvRhoNpuBins.size()),"");
            else if(plots[iplot].Contains("npv") || plots[iplot].Contains("Npv"))
              leg->AddEntry(dummy,JetInfo::getBinLegendEntry(npvRhoNpuBins[inpvRhoNpuBin],"N_{PV}",npvRhoNpuBins.size()),"");
            else if(plots[iplot].Contains("tnpu") || plots[iplot].Contains("Tnpu"))
              leg->AddEntry(dummy,JetInfo::getBinLegendEntry(npvRhoNpuBins[inpvRhoNpuBin],"#mu",npvRhoNpuBins.size(),10),"");
          }
          else
            leg->AddEntry(dummy,"","");

          if(scanPts[iscanPt]>=500.0 && dn>2)  continue;
          if(scanPts[iscanPt]>=1000.0 && dn>1) continue;

          TString histogramName = Form(plots[iplot]+"_%s",JetInfo::get_detector_abbreviation(detector_names[dn]).Data());
          int point = 0;
          for(map<JetInfo,TFile*>::iterator it=ifiles.begin(); it!=ifiles.end(); ++it) {
            imapName = Form(plots[iplot]+"_%s_%i_%s",JetInfo::get_detector_abbreviation(detector_names[dn]).Data(),npvRhoNpuBins[inpvRhoNpuBin],it->first.abbreviation.Data());
            if(dn==2 && it==ifiles.begin()) {
              leg->AddEntry(dummy,Form("%.0f GeV < p_{T}^{GEN} < %.0f GeV",
                            ihistograms[imapName]->GetBinLowEdge(ihistograms[imapName]->FindBin(scanPts[iscanPt])),
                            ihistograms[imapName]->GetBinLowEdge(ihistograms[imapName]->FindBin(scanPts[iscanPt])+1)),"");
            }
            ographs[histogramName]->SetPoint(point,it->first.coneSize/10.0,ihistograms[imapName]->GetBinContent(ihistograms[imapName]->FindBin(scanPts[iscanPt])));
            ographs[histogramName]->SetPointError(point,0.05,ihistograms[imapName]->GetBinError(ihistograms[imapName]->FindBin(scanPts[iscanPt])));
            ographs[histogramName]->SetMarkerSize(ihistograms[imapName]->GetMarkerSize());
            ographs[histogramName]->SetMarkerStyle(ihistograms[imapName]->GetMarkerStyle());
            ographs[histogramName]->SetMarkerColor(colDet[dn]);
            ographs[histogramName]->SetLineColor(colDet[dn]);
            ographs[histogramName]->GetXaxis()->SetTitle("Cone Size");
            //ographs[histogramName]->GetYaxis()->SetTitle(ihistograms[imapName]->GetYaxis()->GetTitle() + TString(" [GeV]"));
            ographs[histogramName]->GetYaxis()->SetTitle(ihistograms[imapName]->GetYaxis()->GetTitle());
            ographs[histogramName]->GetYaxis()->SetRangeUser(0.05,0.4);
            point++;
          }
          mg->Add(ographs[histogramName]);
          leg->AddEntry(ographs[histogramName],detector_regions_eta[dn],"ep");
          if(scanPts[iscanPt]>=1000.0 && dn==1) {
            leg->AddEntry(dummy,Form("%.0f GeV < p_{T}^{GEN} < %.0f GeV",
                            ihistograms[imapName]->GetBinLowEdge(ihistograms[imapName]->FindBin(scanPts[iscanPt])),
                            ihistograms[imapName]->GetBinLowEdge(ihistograms[imapName]->FindBin(scanPts[iscanPt])+1)),"");
            leg->AddEntry(dummy,"","");
            if(scanPts[iscanPt]>=500.0 && dn==2) {
              if(plots[iplot].Contains("rho") || plots[iplot].Contains("Rho"))
                leg->AddEntry(dummy,JetInfo::getBinLegendEntry(npvRhoNpuBins[inpvRhoNpuBin],"#rho",npvRhoNpuBins.size()),"");
              else if(plots[iplot].Contains("npv"))
                leg->AddEntry(dummy,JetInfo::getBinLegendEntry(npvRhoNpuBins[inpvRhoNpuBin],"N_{PV}",npvRhoNpuBins.size()),"");
              else if(plots[iplot].Contains("tnpu") || plots[iplot].Contains("Tnpu"))
              leg->AddEntry(dummy,JetInfo::getBinLegendEntry(npvRhoNpuBins[inpvRhoNpuBin],"#mu",npvRhoNpuBins.size(),10),"");
            }
          }
        }
        TGraphErrors* dummy2 = new TGraphErrors();
        dummy2->SetMarkerSize(0);
        dummy2->SetMarkerColor(kWhite);
        dummy2->SetPoint(0,0.1,0.055);
        dummy2->SetPoint(1,1.0,0.45);//0.5->0.45
        if((canvasName.Contains("ResolutionRhoRef") || canvasName.Contains("ResolutionTnpuRef"))&& tdr)
          mg->Add(dummy2);
        mg->Draw("AP");
        mg->GetXaxis()->SetTitle("Cone Size");
        mg->GetYaxis()->SetTitle(ihistograms[imapName]->GetYaxis()->GetTitle());
        mg->GetYaxis()->SetTitleOffset(1.2);
        leg->Draw("same");
        if(tdr)
          cmsPrelim(19.2);
        cout << "DONE" << endl;
      }
  
      //
      // Open/create the output directory and file
      //
      cout << endl << "\tCreating the output ROOT file ... ";
      TString ioutputDir = outputDir+Form("/%.0fGeV_%inpvRhoNpu/",scanPts[iscanPt],npvRhoNpuBins[inpvRhoNpuBin]);
      if(!gSystem->OpenDirectory(ioutputDir)) gSystem->mkdir(ioutputDir);
  
      TString ofname = ioutputDir+"/ConeSizeScans_"+algsString+".root";
      if(!flavor.IsNull()) ofname = ioutputDir+"/ConeSizeScans_"+algsString+"_"+flavor+".root";
      TFile* ofile = new TFile(ofname,"RECREATE");
      cout << "DONE" << endl;
  
      //
      // Write the output canvases to a file
      //
      writeToOutputFileAndSaveCanvases(ocanvases,ioutputDir,outputFormat);
  
      cout << "\tClosing the output file ... ";
      ofile->Close();
      cout << "DONE" << endl;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
TString openInputFiles(map<JetInfo,TFile*>& ifiles, TString path, vector<TString> algs, TString flavor) {
  cout << "Opening the input files ... ";
  TString algsString;
  for(unsigned int ialg=0; ialg<algs.size(); ialg++) {
    if (!flavor.IsNull()) ifiles[JetInfo(algs[ialg])] = new TFile(path+"canvases_synchplot_"+algs[ialg]+"_"+flavor+".root","READ");
    else ifiles[JetInfo(algs[ialg])] = new TFile(path+"canvases_synchplot_"+algs[ialg]+".root","READ");

    if(!ifiles[JetInfo(algs[ialg])]) {
      cout << endl << "ERROR::main() Could not open the file " << path+"canvases_synchplot_"+algs[ialg]+".root" << endl;
      assert(ifiles[JetInfo(algs[ialg])]);
    }

    algsString+=algs[ialg];
    if(ialg!=algs.size()-1)
      algsString+="_";
  }
  cout << "DONE" << endl << endl;
  return algsString;
}//openInputFiles

//______________________________________________________________________________
void writeToOutputFileAndSaveCanvases(map<TString,TCanvas*>& ocanvases, TString ioutputDir, vector<TString> outputFormat) {
  cout << "\tSaving the output canvases ... ";
  for(map<TString,TCanvas*>::iterator it=ocanvases.begin(); it!=ocanvases.end(); ++it) {
    it->second->Write();
    for(unsigned int iformat=0; iformat<outputFormat.size(); iformat++) {
      it->second->SaveAs(ioutputDir+"/"+it->second->GetName()+outputFormat[iformat]);
    }
  }
  cout << "DONE" << endl;
}//writeToOutputFileAndSaveCanvases

//______________________________________________________________________________
void getInputHistograms(map<JetInfo,TFile*>& ifiles, vector<TString> plot, vector<int> npvRhoNpuBins, map<TString,TH1D*>& ihistograms) {
  //
  // Grab the necessary histograms
  //
  cout << "Getting the input histograms ... ";
  for(map<JetInfo,TFile*>::iterator it=ifiles.begin(); it!=ifiles.end(); ++it) {
    it->second->cd();

    //
    // Booking based on detector sections
    //
    for(int dn=0; dn<NDetectorNames; dn++) {
      for(unsigned int p=0; p<plot.size(); p++) {
        TString icanvasName = Form(plot[p]+"_%s",JetInfo::get_detector_abbreviation(detector_names[dn]).Data());
        for(unsigned int inpvRhoNpuBin=0; inpvRhoNpuBin<npvRhoNpuBins.size(); inpvRhoNpuBin++) {
          TString ihistogramName = Form(icanvasName+"_%i",npvRhoNpuBins[inpvRhoNpuBin]);
          TString mapName = Form(ihistogramName+"_%s",it->first.abbreviation.Data());
          ihistograms[mapName] = (TH1D*)((TCanvas*)gDirectory->Get(icanvasName))->GetPrimitive(ihistogramName);
        }
      }
    }
  }
  cout << "DONE" << endl;

}//getInputHistograms

//______________________________________________________________________________
void getInputHistograms(map<JetInfo,TFile*>& ifiles, TString plot, vector<TString> hname, map<TString,TH1D*>& ihistograms) {
  //
  // Grab the necessary histograms
  //
  cout << "Getting the input histograms for (Canvas,Histogram)=(" << plot << ",{";
  for(map<JetInfo,TFile*>::iterator it=ifiles.begin(); it!=ifiles.end(); ++it) {
    it->second->cd();

    //
    // Booking based on detector sections
    //
    for(unsigned int hh=0; hh<hname.size(); hh++){
      if(it==ifiles.begin()) {
        cout << hname[hh];
        if(hh==hname.size()-1) cout << "}";
        else cout << ",";
      }
      TString icanvasName = plot;
      TString ihistogramName = hname[hh];
      TString mapName = Form(ihistogramName+"_%s",it->first.abbreviation.Data());
      TCanvas* c = (TCanvas*)gDirectory->Get(icanvasName);
      assert(c);
      TH1D* h = (TH1D*)c->GetPrimitive(ihistogramName);
      assert(h);
      /*cout << icanvasName << endl;
      cout << ihistogramName <<endl;
      cout << "sfsg1" << endl;
      cout << h->GetName() << endl;
      cout << "sfsg2" << endl;
      cout << h->GetEntries() << endl;
      cout << "sfsg3" << endl;*/
      ihistograms[mapName] = h;
    }
  }
  cout << ") ... DONE" << endl;

}//getInputHistograms

//______________________________________________________________________________
void doOptimalConeSizeScans(map<JetInfo,TFile*>& ifiles, vector<double> scanPts, vector<int> npvRhoNpuBins,
                            double binWidth, TString outputDir, vector<TString> outputFormat, TString algsString,
                            bool versionB) {
  setREStyle();
  int colDet[4] = {kGreen+3,kBlue+1,kYellow+2,kRed+1}; //bb,ei,eo,ff

  map<TString,TH1D*> ihistograms;
  map<TString, TCanvas*> ocanvases;
  map<TString, TGraphAsymmErrors*> ographs;
  //TString plot = "ResolutionRhoRef";
  TString plot = "ResolutionTnpuRef";
  vector<TString> mp; mp.push_back(plot);
  TString rhoNpv;
  if(plot.Contains("npv")) rhoNpv = "N_{PV}";
  else if(plot.Contains("Rho")) rhoNpv = "#rho [GeV]";
  else if(plot.Contains("Tnpu")) rhoNpv = "#mu";

  getInputHistograms(ifiles,mp,npvRhoNpuBins,ihistograms);

  cout << "\tDoing the scan ... ";
  //
  // loop over the pts to scan
  //
  for(unsigned int iscanPt=0; iscanPt<scanPts.size(); iscanPt++) {
    TMultiGraph* mg = new TMultiGraph();
    TLegend* leg;
    if(plot.Contains("Resolution"))
      if(scanPts[iscanPt]>=1000.0)
        leg = new TLegend(0.38,0.67,0.94,0.92);
      else
        leg = new TLegend(0.38,0.60,0.94,0.92);
      //leg = new TLegend(0.68,0.48,0.88,0.94);
    else
      leg = new TLegend(0.18,0.15,0.38,0.5);
    leg->SetFillColor(0);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.035);
    leg->SetNColumns(2);
    TGraph* dummy = 0;    
    TString imapName;
    TString ocanvasName;
    if(versionB)
      ocanvasName = Form(plot+"_versionB_%.0fGeV",scanPts[iscanPt]);
    else
      ocanvasName = Form(plot+"_%.0fGeV",scanPts[iscanPt]);
    TString ocanvasTitle = "Optimal Cone Size Vs. "+rhoNpv;
    ocanvases[ocanvasName] = new TCanvas(ocanvasName,ocanvasTitle,700,500);
    //
    // loop over the detector regions
    //
    for(int dn=0; dn<NDetectorNames; dn++) {
      if(dn==0)
        leg->AddEntry(dummy,"QCD Multijet","");
      else if(dn==1)
        leg->AddEntry(dummy,JetInfo::get_legend_title(string(ifiles.begin()->first.abbreviation),false).c_str(),"");
      else if(dn==2) {
        /*std::ostringstream ss;
        ss.precision(1);
        ss << fixed << scanPts[iscanPt];
        leg->AddEntry(dummy,TString("p_{T}^{GEN} = ") + ss.str().c_str() + " [GeV]","");*/
        ;
      }
      else
        leg->AddEntry(dummy,"","");

      if(scanPts[iscanPt]>=500.0 && dn>2)  continue;
      if(scanPts[iscanPt]>=1000.0 && dn>1) continue;

      //
      // book the necessary output canvases & histograms
      //
      TString histogramName = Form(plot+"_optimalConeSize_%s"+JetInfo::get_detector_abbreviation(detector_names[dn]).Data());
      TString histogramTitle = Form("Optimal Cone Size Vs. "+rhoNpv+" (%s,%.1f GeV)",detector_names[dn].Data(),scanPts[iscanPt]);
      ographs[histogramName] = new TGraphAsymmErrors();
      ographs[histogramName]->SetNameTitle(histogramName,histogramTitle);
      //ographs[histogramName]->SetMarkerSize(ihistograms[imapName]->GetMarkerSize());
      //ographs[histogramName]->SetMarkerStyle(ihistograms[imapName]->GetMarkerStyle());
      ographs[histogramName]->SetMarkerColor(colDet[dn]);
      //ographs[histogramName]->SetMarkerSize(1.0+((3.0-dn)/2.0));
      if(!versionB) {
        ographs[histogramName]->SetMarkerSize(NDetectorNames-dn);
        ographs[histogramName]->SetLineWidth(2*(NDetectorNames-dn));
      }
      else
        ographs[histogramName]->SetMarkerSize(2);
      ographs[histogramName]->SetLineColor(colDet[dn]);
      ographs[histogramName]->GetXaxis()->SetTitle(rhoNpv);
      ographs[histogramName]->GetYaxis()->SetTitle("Optimal Cone Size");
      /*TAxis *ax = ographs[histogramName]->GetHistogram()->GetXaxis();
      Double_t x1 = ax->GetBinLowEdge(1);
      Double_t x2 = ax->GetBinUpEdge(ax->GetNbins());
      ographs[histogramName]->GetHistogram()->GetXaxis()->Set(4,x1,x2);
      for(unsigned int k=0;k<npvRhoNpuBins.size();k++){
        ographs[histogramName]->GetHistogram()->GetXaxis()->SetBinLabel(k+1,JetInfo::getBinLegendEntry(npvRhoNpuBins[k],"#mu",npvRhoNpuBins.size(),10));
      }*/

      //
      // loop over the npv/rho/npu bins  
      //
      int point = 0;
      for(unsigned int inpvRhoNpuBin=0; inpvRhoNpuBin<npvRhoNpuBins.size(); inpvRhoNpuBin++) {
        double minPointValue = 9999.0;
        for(map<JetInfo,TFile*>::iterator it=ifiles.begin(); it!=ifiles.end(); ++it) {
          imapName = Form(plot+"_%s_%i_%s",JetInfo::get_detector_abbreviation(detector_names[dn]).Data(),
                          npvRhoNpuBins[inpvRhoNpuBin],it->first.abbreviation.Data());
          //map of all the cone sizes and their values.
          double fivePercentCSUp = 0, fivePercentCSDown = 0, tenPercentCSUp = 0, tenPercentCSDown = 0;
          if(inpvRhoNpuBin==0 && dn==2 && it==ifiles.begin()) {
              leg->AddEntry(dummy,Form("%.0f GeV < p_{T}^{GEN} < %.0f GeV",
                            ihistograms[imapName]->GetBinLowEdge(ihistograms[imapName]->FindBin(scanPts[iscanPt])),
                            ihistograms[imapName]->GetBinLowEdge(ihistograms[imapName]->FindBin(scanPts[iscanPt])+1)),"");
          }
          double effectiveMinPointValue = 0;
          if(scanPts[iscanPt]<1000) effectiveMinPointValue = minPointValue;//0.99*minPointValue;
          else effectiveMinPointValue = 0.99*minPointValue;
          if(ihistograms[imapName]->GetBinContent(ihistograms[imapName]->FindBin(scanPts[iscanPt]))<(effectiveMinPointValue) &&
             ihistograms[imapName]->GetBinContent(ihistograms[imapName]->FindBin(scanPts[iscanPt]))!=0) {
            minPointValue = ihistograms[imapName]->GetBinContent(ihistograms[imapName]->FindBin(scanPts[iscanPt]));
            double avgNpvRho = ((npvRhoNpuBins[inpvRhoNpuBin]*binWidth)+(npvRhoNpuBins[inpvRhoNpuBin]*binWidth+binWidth))/2.0;

            //Find the uper error bounds
            for(map<JetInfo,TFile*>::iterator it2=it; it2!=ifiles.end(); it2++) {
              TString upMapName = Form(plot+"_%s_%i_%s",JetInfo::get_detector_abbreviation(detector_names[dn]).Data(),
                                       npvRhoNpuBins[inpvRhoNpuBin],it2->first.abbreviation.Data());
              if(ihistograms[upMapName]->GetBinContent(ihistograms[upMapName]->FindBin(scanPts[iscanPt]))<=(minPointValue*1.05) &&
                 ihistograms[upMapName]->GetBinContent(ihistograms[upMapName]->FindBin(scanPts[iscanPt]))!=0) {
                fivePercentCSUp = it2->first.coneSize/10.0;
              }
              if(ihistograms[upMapName]->GetBinContent(ihistograms[upMapName]->FindBin(scanPts[iscanPt]))<=(minPointValue*1.10) &&
                 ihistograms[upMapName]->GetBinContent(ihistograms[upMapName]->FindBin(scanPts[iscanPt]))!=0) {
                tenPercentCSUp = it2->first.coneSize/10.0;
                tenPercentCSUp = tenPercentCSUp;
              }
            }
            //Find the lower error bounds
            for(map<JetInfo,TFile*>::reverse_iterator it2(it); it2!=ifiles.rend(); it2++) {
              TString downMapName = Form(plot+"_%s_%i_%s",JetInfo::get_detector_abbreviation(detector_names[dn]).Data(),
                                         npvRhoNpuBins[inpvRhoNpuBin],it2->first.abbreviation.Data());
              if(ihistograms[downMapName]->GetBinContent(ihistograms[downMapName]->FindBin(scanPts[iscanPt]))<=(minPointValue*1.05) &&
                 ihistograms[downMapName]->GetBinContent(ihistograms[downMapName]->FindBin(scanPts[iscanPt]))!=0) {
                fivePercentCSDown = it2->first.coneSize/10.0;
              }
              if(ihistograms[downMapName]->GetBinContent(ihistograms[downMapName]->FindBin(scanPts[iscanPt]))<=(minPointValue*1.10) &&
                 ihistograms[downMapName]->GetBinContent(ihistograms[downMapName]->FindBin(scanPts[iscanPt]))!=0) {
                tenPercentCSDown = it2->first.coneSize/10.0;
                tenPercentCSDown = tenPercentCSDown;
              }
            }

            //Set the points on the graph for the minimum and its errors.
            if(!versionB) {
              ographs[histogramName]->SetPoint(point,avgNpvRho,it->first.coneSize/10.0);
              ographs[histogramName]->SetPointError(point,0.0,0.0,(it->first.coneSize/10.0)-fivePercentCSDown,fivePercentCSUp-(it->first.coneSize/10.0));
            }
            else {
              ographs[histogramName]->SetPoint(point,(npvRhoNpuBins[inpvRhoNpuBin]*binWidth)+(2*(dn+1)),it->first.coneSize/10.0);
              //ographs[histogramName]->SetPointError(point,2.5,0.05);
              ographs[histogramName]->SetPointError(point,0.0,0.0,(it->first.coneSize/10.0)-fivePercentCSDown,fivePercentCSUp-(it->first.coneSize/10.0));
            }
          }
        }
        point++;
      }

      //Set the hitograms for each graph
      /*TH1F *h1 = new TH1F("h1","h1",5,0,40);
      h1->SetMaximum(40);
      ographs[histogramName]->SetHistogram(h1);
      ographs[histogramName]->GetXaxis()->SetBinLabel(1,"0#leq#mu<10");
      ographs[histogramName]->GetXaxis()->SetBinLabel(2,"10#leq#mu<20");
      ographs[histogramName]->GetXaxis()->SetBinLabel(3,"20#leq#mu<30");
      ographs[histogramName]->GetXaxis()->SetBinLabel(4,"30#leq#mu<40");
      TAxis *ax = ographs[histogramName]->GetXaxis();
      ax->LabelsOption("d");*/

      mg->Add(ographs[histogramName]);
      leg->AddEntry(ographs[histogramName],detector_regions_eta[dn],"ep");
      if(scanPts[iscanPt]>=1000.0 && dn==1) {
        leg->AddEntry(dummy,Form("%.0f GeV < p_{T}^{GEN} < %.0f GeV",
                      ihistograms[imapName]->GetBinLowEdge(ihistograms[imapName]->FindBin(scanPts[iscanPt])),
                      ihistograms[imapName]->GetBinLowEdge(ihistograms[imapName]->FindBin(scanPts[iscanPt])+1)),"");
      }
    }
    TGraphErrors* dummy2 = new TGraphErrors();
    dummy2->SetMarkerSize(0);
    dummy2->SetMarkerColor(kWhite);
    dummy2->SetPoint(0,0.0,0.2); //0.0->0.2
    if(plot.Contains("Rho")|| plot.Contains("npv"))
      dummy2->SetPoint(1,25,1.4);
    else if(plot.Contains("Tnpu"))
      dummy2->SetPoint(1,40,1.4);
    mg->Add(dummy2);

    //Draw the graph(s)
    mg->Draw("AP");

    //Set axis settings
    if(plot.Contains("Rho")|| plot.Contains("npv")) {
      mg->GetXaxis()->SetRangeUser(0.0,25.0);
      mg->GetXaxis()->SetTitle(rhoNpv);
    }
    else if(plot.Contains("Tnpu")) {
      //Rebin the histogram used to draw the axes
      gPad->Update();
      TH1 *h1 = mg->GetHistogram();
      h1->Rebin(25);
      h1->GetXaxis()->Set(4,0.0,40.0);
      mg->GetXaxis()->SetNdivisions(5);
      //mg->GetXaxis()->LabelsOption("d");
      mg->GetXaxis()->SetBinLabel(1, "0#leq#mu<10");
      //mg->GetXaxis()->LabelsOption("d");
      mg->GetXaxis()->SetBinLabel(2, "10#leq#mu<20");
      //mg->GetXaxis()->LabelsOption("d");
      mg->GetXaxis()->SetBinLabel(3, "20#leq#mu<30");
      //mg->GetXaxis()->LabelsOption("d");
      mg->GetXaxis()->SetBinLabel(4, "30#leq#mu<40");
      mg->GetXaxis()->SetRangeUser(0.0,40.0);
      mg->GetXaxis()->SetLabelSize(0.07);
    }
    mg->GetYaxis()->SetTitle("Optimal Cone Size");
    mg->GetYaxis()->SetTitleOffset(1.2);
    mg->Draw("AP");
    leg->Draw("same");

    TPaveText* pave = new TPaveText(0.20,0.76,0.37,0.90,"NDC TR");
    pave->SetFillColor(0);
    pave->SetShadowColor(0);
    pave->SetTextSize(0.035);
    pave->SetTextAlign(11);
    pave->AddText("5% Uncertainty");
    pave->AddText("Error Bars");
    pave->Draw("same");

    bool tdr = true;
    if(tdr)
      cmsPrelim(19.2);
  }
  cout << "DONE" << endl;
  
  //
  // Open/create the output directory and file
  //
  cout << endl << "\tCreating the output ROOT file ... ";
  if(!gSystem->OpenDirectory(outputDir)) gSystem->mkdir(outputDir);  
  TString ofname;
  if(versionB)
    ofname = outputDir+"/ConeSizeScans_optimalConeSize_versionB_"+algsString+".root";
  else
    ofname = outputDir+"/ConeSizeScans_optimalConeSize_"+algsString+".root";
  //if(!flavor.IsNull()) ofname = outputDir+"/ConeSizeScans_optimalConeSize_"+algsString+"_"+flavor+".root";
  TFile* ofile = new TFile(ofname,"RECREATE");
  cout << "DONE" << endl;
  
  //
  // Write the output canvases to a file
  //
  writeToOutputFileAndSaveCanvases(ocanvases,outputDir,outputFormat);
  
  cout << "\tClosing the output file ... ";
  ofile->Close();
  cout << "DONE" << endl;

}//doOptimalConeSizeScans

//______________________________________________________________________________
void combineCSPlots(map<JetInfo,TFile*>& ifiles, TString outputDir, vector<TString> outputFormat, TString algsString) {
  setREStyle();

  vector<Int_t> colors = getColors();
  vector<Int_t> markers = getMarkerNumbers();

  map<TString,TH1D*> ihistograms;
  map<TString, TCanvas*> ocanvases;
  map<TString, TGraph*> ographs;
  TMultiGraph* mg;
  vector<pair<TString,vector<TString> > > plots;
  vector<TString> mp; mp.push_back("p_drVsrefpt");
  plots.push_back(make_pair("drVsrefptMatchedJets",mp));
  mp.clear(); mp.push_back("NJetsVsPt_Ratio_GenPtCut");
  plots.push_back(make_pair("NJetsVsPt_Ratio_GenPtCut",mp));
  //mp.clear(); mp.push_back("ResolutionRhoRef"); mp.push_back("ResolutionRhoRefNoPU");
  //plots.push_back(make_pair("ResolutionRhoRef_PU_noPU",mp));

  cout << "\tDoing the scan ... ";
  for(unsigned int iplot=0; iplot<plots.size(); iplot++) {
    getInputHistograms(ifiles,plots[iplot].first,plots[iplot].second,ihistograms);

    TLegend* leg = new TLegend(0.6,0.53,0.9,0.9);
    leg->SetFillColor(0);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.045);
    //leg->AddEntry((TObject*)0,JetInfo::get_legend_title(string(ifiles.begin()->first.abbreviation),false).c_str(),"");
    TString imapName;
    TString ocanvasName = plots[iplot].first+"_"+algsString;
    TString ocanvasTitle = "{#Delta}R of RECO jet w.r.t. the GEN Jet";
    ocanvases[ocanvasName] = new TCanvas(ocanvasName,ocanvasTitle,700,500);
    ocanvases[ocanvasName]->cd()->SetLogx();

    int counter = 0;
    mg = new TMultiGraph();
    for(map<JetInfo,TFile*>::iterator it=ifiles.begin(); it!=ifiles.end(); ++it) {
      imapName = Form(plots[iplot].second[0]+"_%s",it->first.abbreviation.Data());
      //cout << ihistograms[imapName]->GetName() << "\t" << ihistograms[imapName]->GetEntries() << endl;
      ographs[imapName] = new TGraph(ihistograms[imapName]);
      if(counter==0) {
        if(plots[iplot].first.Contains("drVsrefptMatchedJets")) {
          mg->Add(ographs[imapName]);
          leg->AddEntry(ographs[imapName],it->first.title,"p");
          //ihistograms[imapName]->GetYaxis()->SetTitle("<#DeltaR> #pm #sigma(#DeltaR)");
          //ographs[imapName]->Draw("AP");
        }
        else if(plots[iplot].first.Contains("NJetsVsPt_Ratio")) {
          ihistograms[imapName]->GetXaxis()->SetTitle("p_{T}^{RECO}   ");
          ihistograms[imapName]->GetXaxis()->SetMoreLogLabels();
          ihistograms[imapName]->GetXaxis()->SetNoExponent();
          ihistograms[imapName]->GetXaxis()->SetRangeUser(0,1000);
          ihistograms[imapName]->GetYaxis()->SetTitle("N_{Jets}^{PU Sample}/N_{Jets}^{NoPU Sample}");
          ihistograms[imapName]->GetYaxis()->SetRangeUser(0,6);
          ihistograms[imapName]->Draw();
          leg->SetHeader(JetInfo::get_legend_title(string(it->first.abbreviation),false).c_str());
          leg->SetNColumns(2);
          leg->AddEntry(ihistograms[imapName],Form("R=%.1f",it->first.coneSize/10.0),"ep");
        }
      }
      else {
        if(plots[iplot].first.Contains("drVsrefptMatchedJets")) {
          //ographs[imapName]->Draw("sameP");
          ographs[imapName]->SetMarkerColor(colors[counter]);
          ographs[imapName]->SetMarkerStyle(markers[counter]);
          ographs[imapName]->SetLineColor(colors[counter]);
          mg->Add(ographs[imapName]);
          leg->AddEntry(ographs[imapName],it->first.title,"p");
        }
        else if(plots[iplot].first.Contains("NJetsVsPt_Ratio")) {
          ihistograms[imapName]->Draw("same");
          ihistograms[imapName]->SetMarkerColor(colors[counter]);
          ihistograms[imapName]->SetMarkerStyle(markers[counter]);
          ihistograms[imapName]->SetLineColor(colors[counter]);
          //leg->AddEntry(ihistograms[imapName],it->first.title,"ep");
          JetInfo ji(it->first.abbreviation);
          leg->AddEntry(ihistograms[imapName],Form("R=%.1f",ji.coneSize/10.0),"ep");
        }
      }
      counter++;
    }
    if(plots[iplot].first.Contains("drVsrefptMatchedJets")) {
      mg->Draw("AP");
      mg->GetXaxis()->SetTitle("p_{T}^{GEN}");
      mg->GetYaxis()->SetTitle("<#DeltaR>");
      mg->GetYaxis()->SetRangeUser(0,0.3);
    }
    leg->Draw("same");
    bool tdr = true;
    if(tdr)
      cmsPrelim(19.2);
  }
  cout << "DONE" << endl;

  //
  // Open/create the output directory and file
  //
  cout << endl << "\tCreating the output ROOT file ... ";
  if(!gSystem->OpenDirectory(outputDir)) gSystem->mkdir(outputDir);  
  TString ofname = outputDir+"/ConeSizeScans_combinations_"+algsString+".root";
  //if(!flavor.IsNull()) ofname = outputDir+"/ConeSizeScans_optimalConeSize_"+algsString+"_"+flavor+".root";
  TFile* ofile = new TFile(ofname,"RECREATE");
  cout << "DONE" << endl;
  
  //
  // Write the output canvases to a file
  //
  writeToOutputFileAndSaveCanvases(ocanvases,outputDir,outputFormat);

  cout << "\tClosing the output file ... ";
  ofile->Close();
  cout << "DONE" << endl;


}//combineCSPlots

//______________________________________________________________________________
vector<Int_t> getColors() {
  vector<Int_t> ret;
  ret.push_back(kBlack);
  ret.push_back(kRed);
  ret.push_back(kOrange+1);
  ret.push_back(kYellow);
  ret.push_back(kGreen);
  ret.push_back(kCyan);
  ret.push_back(kAzure+1);
  ret.push_back(kViolet+1);
  ret.push_back(kMagenta);
  ret.push_back(kGray+2);
  return ret;
}

//______________________________________________________________________________
vector<Int_t> getMarkerNumbers() {
  vector<Int_t> ret;
  ret.push_back(20);
  ret.push_back(33);
  ret.push_back(22);
  ret.push_back(21);
  ret.push_back(24);
  ret.push_back(27);
  ret.push_back(26);
  ret.push_back(25);
  ret.push_back(28);
  ret.push_back(34);
  return ret;
}

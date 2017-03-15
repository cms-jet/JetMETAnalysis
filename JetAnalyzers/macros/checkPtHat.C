//https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideDataFormatGeneratorInterface
//https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuidePythia6Interface#Example_9_CSA_mode_with_Event_Re
//https://cmsweb.cern.ch/couchdb/reqmgr_config_cache/1e816cad3c72514eb948a2551a16f00f/configFile
//https://cmsweb.cern.ch/das/request?view=list&limit=150&instance=prod%2Fglobal&input=dataset%3D%2FQCD_Pt_*0_TuneCUETP8M1_13TeV_pythia8%2FRunIISpring16DR80-PUSpring16_80X_mcRun2_asymptotic_2016_v3-v1%2FAODSIM
#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TLegend.h"

#include "JetMETAnalysis/JetAnalyzers/interface/Settings.h"
#include "JetMETAnalysis/JetUtilities/interface/JRAEvent.h"
#include "JetMETAnalysis/JetUtilities/interface/ProgressBar.hh"

#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <utility>
#include <sstream>
#include <assert.h>

using namespace std;

typedef struct
{
   string path_name;
   double pthat_min;
   double pthat_max;
   double xsection;
   TFile* file;
   TTree* tree;
   TH1D* histogram;
   bool flat;
   string legend_title;
} file_info;

vector<file_info> getFileInfo(string algo) {
   cout << "GetFileInfo::Retrieving file information ... " << endl;

   vector<file_info> ret;
   string basepath = "/fdata/hepx/store/user/siluo/closure/";   
   //Cross section in pb
   //Flat sample should come first, especially if fitFlatten == true
   vector<pair<string,double> > filenames = {
      //make_pair("JRA_QCD_Pt_15to30_TuneCUETP8M1_13TeV_pythia8.root",            1837410000),
      make_pair("../../aperloff/JRA_QCD_Pt_15to30_TuneCUETP8M1_13TeV_pythia8.root",1837410000),
      make_pair("JRA_QCD_Pt_30to50_TuneCUETP8M1_13TeV_pythia8_partial.root",    140932000),
      make_pair("JRA_QCD_Pt_50to80_TuneCUETP8M1_13TeV_pythia8.root",            19204300),
      make_pair("JRA_QCD_Pt_80to120_TuneCUETP8M1_13TeV_pythia8.root",           2762530),
      make_pair("JRA_QCD_Pt_120to170_TuneCUETP8M1_13TeV_pythia8.root",          471100),
      make_pair("JRA_QCD_Pt_170to300_TuneCUETP8M1_13TeV_pythia8.root",          117276),
      make_pair("JRA_QCD_Pt_300to470_TuneCUETP8M1_13TeV_pythia8.root",          7823),
      make_pair("JRA_QCD_Pt_470to600_TuneCUETP8M1_13TeV_pythia8.root",          648.2),
      make_pair("JRA_QCD_Pt_600to800_TuneCUETP8M1_13TeV_pythia8.root",          186.9),
      make_pair("JRA_QCD_Pt_800to1000_TuneCUETP8M1_13TeV_pythia8_partial.root", 32.293),
      make_pair("JRA_QCD_Pt_1000to1400_TuneCUETP8M1_13TeV_pythia8.root",        9.4183),
      make_pair("JRA_QCD_Pt_1400to1800_TuneCUETP8M1_13TeV_pythia8_partial.root",0.84265),
      make_pair("JRA_QCD_Pt_1800to2400_TuneCUETP8M1_13TeV_pythia8.root",        0.114943),
      make_pair("JRA_QCD_Pt_2400to3200_TuneCUETP8M1_13TeV_pythia8.root",        0.00682981),
      make_pair("JRA_QCD_Pt_3200toInf_TuneCUETP8M1_13TeV_pythia8.root",         0.000165445)
   };

   for(auto fn : filenames) {
      file_info tmp;
      string abbreviation = fn.first.substr(0,fn.first.find("_Tune"));
      tmp.path_name = basepath + fn.first;
      cout << "\tFile " << ret.size() << ": " << tmp.path_name << endl;
      string range = fn.first.substr(fn.first.find("QCD_Pt")+7,fn.first.find("_Tune")-7);
      tmp.pthat_min = atof(range.substr(0,range.find("to")).c_str());
      tmp.pthat_max = atof(range.substr(range.find("to")+2,range.length()-range.find("to")+2).c_str());
      tmp.xsection = fn.second;
      tmp.file = TFile::Open(tmp.path_name.c_str(),"READ");
      tmp.tree = (TTree*)tmp.file->Get((algo+"/t").c_str());
      assert(tmp.tree);
      tmp.histogram = new TH1D(abbreviation.c_str(),"",1000,0,10000);
      tmp.histogram->SetDirectory(0);
      if(tmp.pthat_max==7000) tmp.flat = true;
      else                    tmp.flat = false;
      ret.push_back(tmp);
   }

   return ret;
}

void makeAndSaveFlatWeightHistogram(string algo, double normalization) {
   TH2F* weights = new TH2F("all_","all",NPtBins,vpt,NETA,veta);
   weights->SetDirectory(0);
   weights->Sumw2();
   for(int ibin=0; ibin<=(weights->GetNbinsX()+2)*(weights->GetNbinsY()+2); ibin++) {
      weights->SetBinContent(ibin,normalization);
   }
   TFile* outf = TFile::Open("NormalizationWeight.root","RECREATE");
   outf->mkdir(algo.c_str());
   outf->cd(algo.c_str());
   weights->Write();
   outf->Close();
}

/*
Run Using:
.L checkPtHat.C+
checkPtHat("ak4pfchsl1",false,0,3200,12104.3,true)
 */

void checkPtHat(string algo = "ak4pfchs", bool fitFlatten = false, int maxEntries = 0, double mergePoint = 1800.0, double flatNormalization = 1.0, bool makeWeights = false, bool save = true) {
   vector<file_info> files = getFileInfo(algo);
   TH1D* pthat_distribution = new TH1D("pThat","pThat Distribution",1000,0,10000);
   pthat_distribution->SetDirectory(0);
   pthat_distribution->Sumw2();
   TH1D* flat = new TH1D("flat","flat",1000,0,10000);
   flat->SetDirectory(0);
   flat->Sumw2();
   double flatTailCrossSection = 0.0;
   TFitResultPtr fitResult;
   TF1* fit(nullptr);
   TH1D* weights = new TH1D("weights","Weights for flattening non-flat samples",1000,0,10000);;

   cout << "Looping through files ... " << endl;
   for(auto fi : files) {
      cout << "\tDoing file " << fi.path_name << " ... " << endl;
      JRAEvent* JRAEvt = new JRAEvent(fi.tree,85);
      fi.tree->SetBranchStatus("*",0);
      vector<string> branch_names = {"pthat","weight"};
      for(auto n : branch_names) {
         fi.tree->SetBranchStatus(n.c_str(),1);
      }
      
      cout << "\tLooping through entries ... " << endl;
      Long64_t nentries = (maxEntries>0) ? maxEntries : fi.tree->GetEntries();
      int entry_counter = 0;
      for (Long64_t ientry=0;ientry<nentries; ientry++) {
         loadbar2(ientry+1,nentries,50,"\t\t");

         fi.tree->GetEntry(ientry);
         
         double weight(JRAEvt->weight);

         if(fi.flat) {
            weight*=(fitFlatten) ? 1.0 : flatNormalization;
         }
         else {
            //6E8 fudge factor
            weight*= (fi.xsection/nentries) * ((fitFlatten) ? weights->GetBinContent(weights->FindBin(JRAEvt->pthat)) : 1.0);
         }

         if(JRAEvt->pthat<fi.pthat_min) continue;
         if(JRAEvt->pthat>fi.pthat_max) continue;
         if(JRAEvt->pthat>mergePoint) entry_counter++;
         if(fi.flat && JRAEvt->pthat>mergePoint) flat->Fill(JRAEvt->pthat, weight);
         if(fi.flat && JRAEvt->pthat>mergePoint) continue;
         fi.histogram->Fill(JRAEvt->pthat, weight);

         if(ientry==nentries-1) cout << endl;
      }
      cout << "\t\tEntries in pThat>" << mergePoint << " section: " << entry_counter << endl;

      if(fitFlatten) {
         if(fi.flat) {
            cout << "Fitting \"flat\" sample tail and making weights ... " << endl;
            fit = new TF1("fit","pow(10.0,[0]+([1]*x))",mergePoint-1000,mergePoint);
            fitResult = fi.histogram->Fit("fit","RS","goff");
            for(int ibin=1; ibin<=weights->GetNbinsX(); ibin++) {
               if(weights->GetBinCenter(ibin)<mergePoint || fi.histogram->GetBinContent(ibin)==0)
                  weights->SetBinContent(ibin,1.0);
               else
                  weights->SetBinContent(ibin,fit->Eval(weights->GetBinCenter(ibin))/fi.histogram->GetBinContent(ibin));
            }
         }
      }      

      if(fi.flat) {
         if(flatNormalization!=1.0) {
            cout << "\t\tWARNING::checkPtHat if the overall normalization of the flat sample is not right try setting the"
                 << "flatNormalization value to 1.0 and recomputing the normalization. It never hurts to confirm the value."
                 << " For right now, unfortunately, this is an iterative process." << endl; 
         }
         else {
            cout << "\t\tTotal scale factor applied to flat sample: " << fi.xsection/fi.histogram->Integral() << endl;
            cout << "\t\tMake sure to double check that the normalization looks right." << endl;
            fi.histogram->Scale(fi.xsection/fi.histogram->Integral());
         }
         if(makeWeights) {
            makeAndSaveFlatWeightHistogram(algo, (flatNormalization!=1.0) ? flatNormalization : fi.xsection/fi.histogram->Integral());
         }
      }

      if(!fi.flat) flatTailCrossSection+=fi.xsection;
      pthat_distribution->Add(fi.histogram);

      fi.file->Close();
   }

   TCanvas * c = new TCanvas();
   c->SetName("NormalizationValidation");
   c->SetLogy();
   pthat_distribution->GetXaxis()->SetRangeUser(0.0,6000.0);
   pthat_distribution->GetXaxis()->SetTitle("#hat{p}_{T} (GeV)");
   pthat_distribution->GetYaxis()->SetTitle("Events / fb^{-1}");
   pthat_distribution->Draw();
   flat->Scale(flatTailCrossSection/flat->Integral());
   flat->SetMarkerColor(kBlue);
   flat->SetLineColor(kBlue);
   flat->SetMarkerStyle(kOpenCircle);
   flat->Draw("same");
   TLegend *l = new TLegend(0.5,0.5,0.88,0.88);
   l->AddEntry(pthat_distribution,"Full (Merged) QCD","ep");
   l->AddEntry(flat,"Unused QCD Flat","ep");
   int color_counter = 2;
   for(unsigned int ifile=0; ifile<files.size(); ifile++) {
      color_counter = (ifile!=0 && ifile%2==0) ? (ifile/2)+2 : color_counter;
      files[ifile].histogram->SetMarkerColor(color_counter);
      files[ifile].histogram->SetLineColor(color_counter);
      (ifile%2==0) ? files[ifile].histogram->SetMarkerStyle(kOpenTriangleUp) : files[ifile].histogram->SetMarkerStyle(kOpenTriangleDown);
      files[ifile].histogram->Draw("same");
      if(files[ifile].flat)
         l->AddEntry(files[ifile].histogram,
                     Form("QCD Flat [%.0f,%.0f]",files[ifile].pthat_min,files[ifile].pthat_max),
                     "ep");
      else
         l->AddEntry(files[ifile].histogram,
                     Form("QCD [%.0f,%.0f]",files[ifile].pthat_min,files[ifile].pthat_max),"ep");
   }
   l->Draw("same");
   if(save)
      c->SaveAs("NormalizationValidation.eps");
   TFile* outf = TFile::Open("NormalizationWeight.root","Update");
   outf->cd(algo.c_str());
   c->Write();
   outf->Close();
}

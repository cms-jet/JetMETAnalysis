///////////////////////////////////////////////////////////////////
//
// jet_create_flavor_weights_x
// ---------------------------
//
//            01/20/2011 Alexx Perloff  <aperloff@physics.tamu.edu>
///////////////////////////////////////////////////////////////////

#include "JetMETAnalysis/JetAnalyzers/interface/Settings.h"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"

#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2.h"
#include "TH2F.h"
#include "TH3D.h"
#include "TF1.h"
#include "TLegend.h"
#include "TString.h"
#include "TMath.h"

#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdarg.h>
#include <cstring>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// define local functions
////////////////////////////////////////////////////////////////////////////////

///returns the appropriate maximum deltaR for a given algorithm
double getDRmax(TString s);

///returns the appropriate minimum jtpt for a given algorithm
double getJTPTmin(TString s);

/// get the flavor name used to index the flavors
TString get_flavor_name(TString flavor);

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv)
{
  gROOT->ProcessLine("#include<vector>");
  gSystem->Load("libFWCoreFWLite.so");
  
  //
  // evaluate command-line / configuration file options
  // 
  CommandLine cl;
  if (!cl.parse(argc,argv)) return 0;

  TString         input          = cl.getValue<TString>  ("input");
  vector<TString> algs           = cl.getVector<TString> ("algs");
  vector<TString> flavors        = cl.getVector<TString> ("flavors");
  TString         outputDir      = cl.getValue<TString>  ("outputDir",             "");
  TString         outputFilename = cl.getValue<TString>  ("outputFilename", "wf.root");
  
  if (!cl.check()) return 0;
  cl.print();

  //
  // more checks
  //
  vector<TString>::iterator it;
  it = flavors.end();
  flavors.insert(it,"all");
  cout << "Adding \"all\" to flavors" << endl;

  //
  // Extra settings
  //
  gStyle->SetPalette(1);

  TFile *inf = new TFile(input);
  if (!inf->IsOpen()) {  cout<<"Can't open "<<input<<endl; return 0; }
  if(!outputDir.IsNull() && !outputDir.EndsWith("/")) outputDir += "/";
  TFile *outf = new TFile(outputDir+outputFilename,"RECREATE");
  if (!outf->IsOpen()) { cout<<"Can't create "<<outputDir+outputFilename<<endl; return 0; }

  //
  // Loop over the algorithms
  //
  for(unsigned int a=0; a<algs.size(); a++)
    {
       int    refpdgid[100];
       float  jtpt[100];
       float  jteta[100];
       unsigned char nref;
       float  refdrjt[100];
       double drmax = getDRmax(algs[a]);
       double jtptmin = getJTPTmin(algs[a]);

       //
       // setup the tree for reading
       //
       TDirectoryFile *idir = (TDirectoryFile*)inf->Get(algs[a]);
       if (idir) 
          cout << "The directory is " << idir->GetName() << endl;
       else {
          cout << "ERROR::Directory " << algs[a] <<" could not be found in file " << inf->GetName() << endl;
          cout << " SKIPPING ALGO " << algs[a] << endl;
          continue;
       }
       
       TTree *tree = (TTree*)idir->Get("t");
       tree->SetBranchAddress("refpdgid", refpdgid);
       tree->SetBranchAddress("jtpt",         jtpt);
       tree->SetBranchAddress("jteta",       jteta);
       tree->SetBranchAddress("nref",        &nref);
       tree->SetBranchAddress("refdrjt",   refdrjt);

       //
       // create directory in output file and book histograms
       //
       TDirectoryFile* odir = (TDirectoryFile*)outf->mkdir(algs[a]);
       if (odir==0) { cout<<"failed to create directory."<<endl; continue; }
       odir->cd();

       map<TString,TH2F*> m;
       for(unsigned int f=0; f<flavors.size(); f++)
       {
          //CHECK TO SEE IF THESE BIN SIZES ARE OKAY!!!
          m[flavors[f]] = new TH2F(flavors[f]+"_",flavors[f],200,0,2000,100,-5,5);
       }
       
       //
       // fill histograms
       //
       unsigned int nevt = (unsigned int)tree->GetEntries();
       cout<<algs[a]<<"......"<<nevt<<" entries:"<<endl;
       for (unsigned int ievt=0;ievt<nevt;ievt++) 
       {
          if (ievt % 100000 == 0) 
             cout<<ievt<<endl;
          tree->GetEntry(ievt);
          for (unsigned char iref=0;iref<nref;iref++) 
            {
               int   pdgid  = refpdgid[iref];
               float pt     = jtpt[iref];
               float eta    = jteta[iref];
               float dr     = refdrjt[iref];

               if(dr>drmax) continue;
               if(pt<jtptmin) continue;
               //if(((abs(pdgid)<=5 && abs(pdgid)!=0) || abs(pdgid)==21) && m["all"])
               m["all"]->Fill(pt,eta);
               if(abs(pdgid)==5 && m["b"])
                  m["b"]->Fill(pt,eta);
               else if(abs(pdgid)==4 && m["c"])
                  m["c"]->Fill(pt,eta);
               else if(abs(pdgid)==21 && m["g"])
                  m["g"]->Fill(pt,eta);
               else if(abs(pdgid)<=3 && abs(pdgid)!=0 && m["uds"])
                  m["uds"]->Fill(pt,eta);
               else if((abs(pdgid)==411 || abs(pdgid)==413) && m["slc"])
                  m["slc"]->Fill(pt,eta);
               else if((abs(pdgid)==511 || abs(pdgid)==513) && m["slb"])
                  m["slb"]->Fill(pt,eta);
            }//for (unsigned char iref=0;iref<nref;iref++)
       }//for (unsigned int ievt=0;ievt<nevt;ievt++) 

       TLegend leg(0.9,0.8,1.0,0.9);
       for(unsigned int f=0; f<flavors.size(); f++)
       {
          cout << "\tDoing flavor " << flavors[f] << " ... ";
          //
          // Normalize to "all"
          //
          m[flavors[f]]->Divide(m["all"]);

          double meanx = m[flavors[f]]->GetMean(1);
          double meany = m[flavors[f]]->GetMean(2);
          int meanbin = m[flavors[f]]->FindBin(meanx,meany);
          double meanbinvalue = m[flavors[f]]->GetBinContent(meanbin);
          for(int i=1; i<=m[flavors[f]]->GetNbinsX(); i++)
          {
             for(int j=1;j<=m[flavors[f]]->GetNbinsY(); j++)
             {
                if(m[flavors[f]]->GetBinContent(i,j)!=0)
                {
                   m[flavors[f]]->SetBinContent(i,j,meanbinvalue/m[flavors[f]]->GetBinContent(i,j));
                }
             }
          }
          
          if(f==0)
          {
              m[flavors[f]]->GetXaxis()->SetTitle("p_{T}");
              m[flavors[f]]->GetYaxis()->SetTitle("#eta");
              m[flavors[f]]->GetZaxis()->SetTitle("Contributions (Relative to All Contribution)");
              m[flavors[f]]->SetTitle("Flavor Composition of QCD Sample");
              m[flavors[f]]->Draw("colz");
          }
          else
          {
             m[flavors[f]]->SetLineColor(f+1);
             m[flavors[f]]->Draw("colz same");
          }

          leg.AddEntry(m[flavors[f]],flavors[f],"l");
          if(f==flavors.size()-1)
          {
             leg.SetBorderSize(0);
             leg.SetFillColor(0);    
             leg.Draw("same");
          }

          m[flavors[f]]->Write();
          cout << "DONE" << endl;
       }//for(unsigned int f=0; f<flavors.size(); f++)
    }//for(unsigned int a=0; a<algs.size(); a++)
}

//////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
double getDRmax(TString s)
{
   if (s=="ic5calo")
      return 0.25;
   else if (s=="ic5pf")
      return 0.25;
   else if (s=="ak5calo")
      return 0.25;
   else if (s=="ak5calol1")
      return 0.25;
   else if (s=="ak5calol1off")
      return 0.25;
   else if (s=="ak5calol1offl2l3")
      return 0.25;
   else if (s=="ak7calo")
      return 0.35;
   else if (s=="ak7calol1")
      return 0.35;
   else if (s=="ak7calol1off")
      return 0.35;
   else if (s=="ak5pf")
      return 0.25;
   else if (s=="ak5pfl1")
      return 0.25;
   else if (s=="ak5pfl1l2l3")
      return 0.25;
   else if (s=="ak5pfl1off")
      return 0.25;
   else if (s=="ak7pf")
      return 0.35;
   else if (s=="ak7pfl1")
      return 0.35;
   else if (s=="ak7pfl1off")
      return 0.35;
   else if (s=="ak5pfchs")
      return 0.25;
   else if (s=="ak5pfchsl1")
      return 0.25;
   else if (s=="ak5pfchsl1l2l3")
      return 0.25;
   else if (s=="ak5pfchsl1off")
      return 0.25;
   else if (s=="ak7pfchs")
      return 0.35;
   else if (s=="ak7pfchsl1")
      return 0.35;
   else if (s=="ak7pfchsl1off")
      return 0.35;
   else if (s=="ak5jpt")
      return 0.25;
   else if (s=="ak5jptl1")
      return 0.25;
   else if (s=="ak5jptl1l2l3")
      return 0.25;
   else if (s=="ak7jpt")
      return 0.35;
   else if (s=="ak7jptl1")
      return 0.35;
   else if (s=="sc5calo")
      return 0.25;
   else if (s=="sc5pf")
      return 0.25;
   else if (s=="sc7calo")
      return 0.25;
   else if (s=="sc7pf")
      return 0.25;
   else if (s=="kt4calo")
      return 0.20;
   else if (s=="kt4pf")
      return 0.20;
   else if (s=="kt6calo")
      return 0.30;
   else if (s=="kt6pf")
      return 0.30;
   else
    return 0.0;
}

//______________________________________________________________________________
double getJTPTmin(TString s)
{
   if(s.Contains("pf"))
      return 10.0;
   else if(s.Contains("calo"))
      return 30.0;
   else if(s.Contains("jpt"))
      return 20.0;
   else
      return 0.0;
}

//______________________________________________________________________________
TString get_flavor_name(TString flavor)
{
   TString result;
   if      (flavor.CompareTo("uds")==0)   result = "qJ";
   else if (flavor.CompareTo("c")==0)     result = "cJ";
   else if (flavor.CompareTo("b")==0)     result = "bJ";
   else if (flavor.CompareTo("g")==0)     result = "gJ";
   else if (flavor.CompareTo("slc")==0)   result = "slcJ";
   else if (flavor.CompareTo("slb")==0)   result = "slbJ";
   else if (flavor.CompareTo("all")==0)   result = "aJ";
   else {
      cout << "***ERROR***get_flavor_name::flavor "<<flavor<<" is not known"<<endl;
   } 
   return result;
}

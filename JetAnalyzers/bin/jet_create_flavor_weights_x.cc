///////////////////////////////////////////////////////////////////
//
// jet_create_flavor_weights_x
// ---------------------------
//
//            01/20/2011 Alexx Perloff  <aperloff@physics.tamu.edu>
///////////////////////////////////////////////////////////////////

#include "JetMETAnalysis/JetAnalyzers/interface/Settings.h"
#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/JRAEvent.h"
#include "JetMETAnalysis/JetUtilities/interface/ProgressBar.hh"

#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TFile.h"
#include "TKey.h"
#include "TDirectory.h"
#include "TTree.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2.h"
#include "TH2F.h"
#include "TH3D.h"
#include "TF1.h"
#include "TLegend.h"
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

/// returns the appropriate maximum deltaR for a set of algorithms
map<std::string,float> getDRmax(vector<string>& algs, double drmax);

/// get the flavor name used to index the flavors
string get_flavor_name(string flavor);

/// returns the name of the jet flavor based on the abs(pdgid)
string pdgid_to_name(int pdgid);

/// check if a vector of strings contains a certain element
bool contains(const vector<string>& collection,const string& element);

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv) {
    gROOT->ProcessLine("#include<vector>");
    gSystem->Load("libFWCoreFWLite.so");
  
    //
    // evaluate command-line / configuration file options
    // 
    CommandLine cl;
    if (!cl.parse(argc,argv)) return 0;

    string         input          = cl.getValue<string>  ("input");
    vector<string> flavors        = cl.getVector<string> ("flavors");
    vector<float>  binspt         = cl.getVector<float>  ("binspt",                "");
    vector<float>  binseta        = cl.getVector<float>  ("binseta",               "");
    vector<float>  binsphi        = cl.getVector<float>  ("binsphi",               "");
    vector<string> algs           = cl.getVector<string> ("algs",                  "");
    string         outputDir      = cl.getValue<string>  ("outputDir",             "");
    string         outputFilename = cl.getValue<string>  ("outputFilename", "wf.root");
    float          drmax          = cl.getValue<float>   ("drmax",                0.2);
    float          ptmin          = cl.getValue<float>   ("ptmin",                0.0);
    bool           normToAll      = cl.getValue<bool>    ("normToAll",           true);
    bool           refOrJetPt     = cl.getValue<bool>    ("refOrJetPt",             0);

    if (!cl.check()) return 0;
    cl.print();

    //
    // more checks
    //
    vector<string>::iterator it;
    it = flavors.end();
    flavors.insert(it,"all");
    cout << "Adding \"all\" to flavors" << endl;

    //
    // Extra settings
    //
    gStyle->SetPalette(1);

    TFile *inf = TFile::Open(input.c_str(),"READ");
    if (!inf->IsOpen()) {  cout<<"Can't open "<<input<<endl; return 0; }
    if(!outputDir.empty() && outputDir.back()!='/') outputDir += "/";
    TFile *outf = TFile::Open((outputDir+outputFilename).c_str(),"RECREATE");
    if (!outf->IsOpen()) { cout<<"Can't create "<<outputDir+outputFilename<<endl; return 0; }

    //
    // Split the DRmax from the algorithms
    //
    map<std::string,float> alg2drmax = getDRmax(algs, drmax);

    //
    // Loop over the algorithms
    //
    TIter next(inf->GetListOfKeys());
    TKey* key(0);
    while ((key=(TKey*)next())) {
        if (strcmp(key->GetClassName(),"TDirectoryFile")!=0) continue;

        TDirectoryFile* idir = (TDirectoryFile*)key->ReadObj();
        string alg(idir->GetName());
        if (algs.size()>0&&!contains(algs,alg)) continue;
        if (!idir) {
            cout << "ERROR::Directory " << alg <<" could not be found in file "
                 << inf->GetName() << endl << " SKIPPING ALGO " << alg << endl;
            continue;
        }

        float drmax_alg = drmax;
        if (alg2drmax.find(alg)!=alg2drmax.end()) drmax_alg=alg2drmax[alg];

        //
        // setup the tree for reading
        //
        TTree *tree = (TTree*)idir->Get("t");
        if (0==tree) { cout<<"no tree found."<<endl; continue; }
        JRAEvent* JRAEvt = new JRAEvent(tree,85);
        tree->SetBranchStatus("*",0);
        vector<string> branch_names = {"nref","refdrjt","refpdgid","refpt","jtpt","jteta"};
        for(auto n : branch_names) {
            tree->SetBranchStatus(n.c_str(),1);
        }

        //
        // create directory in output file and book histograms
        //
        TDirectoryFile* odir = (TDirectoryFile*)outf->mkdir(alg.c_str());
        if (odir==0) { cout<<"failed to create directory."<<endl; continue; }
        odir->cd();

        map<string,TH2F*> m;
        for(unsigned int f=0; f<flavors.size(); f++) {
            m[flavors[f]] = new TH2F((flavors[f]+"_").c_str(),flavors[f].c_str(),binspt.size()-1,&binspt[0],binseta.size()-1,&binseta[0]);
        }
       
        //
        // fill histograms
        //
        unsigned int nevt = (unsigned int)tree->GetEntries();
        cout<<alg<<" ...... "<<nevt<<" entries:"<<endl;
        for (unsigned int ievt=0;ievt<nevt;ievt++) {
            loadbar2(ievt+1,nevt,50,"\t");
            tree->GetEntry(ievt);
            for (unsigned char iref=0;iref<JRAEvt->nref;iref++) {
               int   pdgid  = JRAEvt->refpdgid->at(iref);
               float pt     = (refOrJetPt==0) ? JRAEvt->refpt->at(iref) : JRAEvt->jtpt->at(iref);
               float eta    = JRAEvt->jteta->at(iref);
               float dr     = JRAEvt->refdrjt->at(iref);

               if(dr>drmax_alg) continue;
               if(pt<ptmin) continue;
               //if(((abs(pdgid)<=5 && abs(pdgid)!=0) || abs(pdgid)==21) && m["all"])
               m["all"]->Fill(pt,eta);
                if(abs(pdgid)==5 && m["b"])
                    m["b"]->Fill(pt,eta);
                else if(abs(pdgid)==4 && m["c"])
                    m["c"]->Fill(pt,eta);
                else if(abs(pdgid)==3 && m["s"])
                    m["s"]->Fill(pt,eta);
                else if(abs(pdgid)<=3 && abs(pdgid)!=0 && m["ud"])
                    m["ud"]->Fill(pt,eta);
                else if(abs(pdgid)==21 && m["g"])
                    m["g"]->Fill(pt,eta);
                else if((abs(pdgid)==411 || abs(pdgid)==413) && m["slc"])
                    m["slc"]->Fill(pt,eta);
                else if((abs(pdgid)==511 || abs(pdgid)==513) && m["slb"])
                    m["slb"]->Fill(pt,eta);
            }//for (unsigned char iref=0;iref<nref;iref++)
        }//for (unsigned int ievt=0;ievt<nevt;ievt++) 

        TLegend leg(0.9,0.8,1.0,0.9);
        for(unsigned int f=0; f<flavors.size(); f++) {
            cout << "\tDoing flavor " << flavors[f] << " ... ";

            //
            // Save a reference
            //
            TH2F* reference = dynamic_cast<TH2F*>(m[flavors[f]]->Clone((flavors[f]+"_Reference").c_str()));
            TH2F* tmp       = dynamic_cast<TH2F*>(m[flavors[f]]->Clone((flavors[f]+"_RefXWeight").c_str()));
            reference->Write();

            //
            // Normalize to "all"
            //
            if(normToAll)
                m[flavors[f]]->Divide(m["all"]);

            double meanx = m[flavors[f]]->GetMean(1);
            double meany = m[flavors[f]]->GetMean(2);
            int meanbin = m[flavors[f]]->FindBin(meanx,meany);
            double meanbinvalue = m[flavors[f]]->GetBinContent(meanbin);
            for(int i=1; i<=m[flavors[f]]->GetNbinsX(); i++) {
                for(int j=1;j<=m[flavors[f]]->GetNbinsY(); j++) {
                    if(m[flavors[f]]->GetBinContent(i,j)!=0) {
                        m[flavors[f]]->SetBinContent(i,j,meanbinvalue/m[flavors[f]]->GetBinContent(i,j));
                    }
                }
            }

            //
            // Set Overall Normalization
            //
            tmp->Multiply(m[flavors[f]]);
            double k = reference->Integral()/tmp->Integral();
            m[flavors[f]]->Scale(k);

            string xtitle = ((refOrJetPt==0) ? "p_{T}^{ref}" : "p_{T}^{reco}");
            string ytitle = "#eta^{reco}";
            string ztitle = ((normToAll) ? "Contributions (Relative to All Contribution)": "Contributions");
            m[flavors[f]]->GetXaxis()->SetTitle(xtitle.c_str());
            m[flavors[f]]->GetYaxis()->SetTitle(ytitle.c_str());
            m[flavors[f]]->GetZaxis()->SetTitle(ztitle.c_str());
            m[flavors[f]]->SetTitle("Flavor Composition of QCD Sample");
            m[flavors[f]]->SetLineColor(f+1);
            if(f==0)
                m[flavors[f]]->Draw("colz");
            else
                m[flavors[f]]->Draw("colz same");

            leg.AddEntry(m[flavors[f]],flavors[f].c_str(),"l");
            if(f==flavors.size()-1) {
                leg.SetBorderSize(0);
                leg.SetFillColor(0);    
                leg.Draw("same");
            }

            m[flavors[f]]->Write();
            cout << "DONE" << endl;
        }//for(unsigned int f=0; f<flavors.size(); f++)
    }//while
}

//////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
map<std::string,float> getDRmax(vector<string>& algs, double drmax)
{
   map<std::string,float> alg2drmax;
   for (unsigned int ialg=0;ialg<algs.size();ialg++) {
      string alg=algs[ialg];
      size_t pos=alg.find(':');
      if (pos!=string::npos) {
         float drmax_alg; stringstream ss; ss<<alg.substr(pos+1); ss>>drmax_alg;
         alg=alg.substr(0,pos);
         alg2drmax[alg]=drmax_alg;
         algs[ialg]=alg;
         //cout<<"drmax("<<alg<<") = "<<alg2drmax[alg]<<endl;
      }
      else {
         cout << "WARNING::drmax not specified for algorithm " << alg << "." << endl
              << " Using default drmax = " << drmax << endl
              << " To specify drmax for a given algorithm, use \"alg:drmax\"." << endl;
      }
   }
   return alg2drmax;
}

//______________________________________________________________________________
string get_flavor_name(string flavor) {
   string result;
   if      (flavor == "ud")    result = "qJ";
   else if (flavor == "s")     result = "sJ";
   else if (flavor == "c")     result = "cJ";
   else if (flavor == "b")     result = "bJ";
   else if (flavor == "g")     result = "gJ";
   else if (flavor == "slc")   result = "slcJ";
   else if (flavor == "slb")   result = "slbJ";
   else if (flavor == "all")   result = "aJ";
   else {
      cout << "***ERROR***get_flavor_name::flavor "<<flavor<<" is not known"<<endl;
   } 
   return result;
}

//______________________________________________________________________________
string pdgid_to_name(int pdgid) {
   int abspdgid = abs(pdgid);
   string result;
   if      (abspdgid <= 2 && abspdgid >= 1)     result = "ud";
   else if (abspdgid == 3)                      result = "s";
   else if (abspdgid == 4)                      result = "c";
   else if (abspdgid == 5)                      result = "b";
   else if (abspdgid == 6)                      result = "g";
   else if (abspdgid == 411 || abspdgid == 413) result = "slcJ";
   else if (abspdgid == 511 || abspdgid == 513) result = "slbJ";
   else {
      cout << "***ERROR***pdgid_to_name::pdgid "<<pdgid<<" is not known"<<endl;
   } 
   return result;
}

//______________________________________________________________________________
bool contains(const vector<string>& collection,const string& element) {
   vector<string>::const_iterator it;
   for (it=collection.begin();it!=collection.end();++it)
      if ((*it)==element) return true;
   return false;
}

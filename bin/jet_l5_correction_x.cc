////////////////////////////////////////////////////////////////////////////////
//
// jet_l5_correction_x
// -------------------
//
//                         12/15/2008 Alexx Perloff  <aperloff@physics.tamu.edu>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/ObjectLoader.h"
#include "JetMETAnalysis/JetUtilities/interface/RootStyle.h"

#include "TROOT.h"
#include "TApplication.h"
#include "TMath.h"
#include "TFile.h"
#include "TList.h"
#include "TKey.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TObjString.h"
#include "TString.h"
#include "TLatex.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <map>
#include <stdlib.h>


using namespace std;


////////////////////////////////////////////////////////////////////////////////
// define local functions
////////////////////////////////////////////////////////////////////////////////

/// check if a vector of strings contains a certain element
bool contains(const vector<string>& collection,const string& element);

/// transform the alg label into a title, e.g.: kt4calo -> k_{T}, D=0.4 (Calo)
string get_legend_title(const string& alg);

/// get the suffix to the parmeter text file for each algorithm
string get_algorithm_suffix(const string& alg);

/// get the flavor name used to name the .txt files
TString get_flavor_name(TString flavor);

/// get a list of the flavors to analyze
vector<TString> get_flavors();

/// analyze a single algorithm
void analyze_algo(TString algo, TFile* ifile, TFile* ofile, CommandLine & cl);

/// analyze a single flavor
void analyze_flavor(TString algo, TString flavor, TDirectoryFile* idir, CommandLine & cl,
                    vector<TGraphErrors*>& vabsrsp_eta, vector<TGraphErrors*>& vabscor_eta,
                    ObjectLoader<TH1F>& hl_eta);

/// interpolates where the (x,y) points for a given flavor should be given the abscissa of the "all" flavor
void interpolate_flavor(vector<TGraphErrors*>& v, vector<TGraphErrors*>& vint, vector<TGraphErrors*>& vall);

/// normalize the rsp and cor graphs to the "all" flavor
void normalize_flavor(vector<TGraphErrors*>& vnum,vector<TGraphErrors*>& vden);

/// perform all of the fitting for the corrections
void perform_correction_fits(vector<TGraphErrors*>& vabsrsp_eta, vector<TGraphErrors*>& vabscor_eta,
                             TString algo);

/// this method performs consecutive fittings (up to maxFitIter) but uses the
/// parameters from the one with the lowest chi2 that has not failed.
void perform_smart_fit(TGraphErrors * gabscor, TF1 * fabscor);

/// split string into vector with user defined delimiter
vector<string> split(const string& str,const string& delim);

/// print parameters to .txt file
void print_text_file(TString filename, TString flavor, vector<TGraphErrors*> & vabscor_eta, 
                     ObjectLoader<TH1F> & hl_eta, bool print_header);

/// evaluates/interpolates the error for any value in a TGraphErrors object
/// this function assumes that the x values of the graph are sorted and that the errors are symmetric
double evalError(TGraphErrors* g, double x);

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv)
{
   //
   // evaluate command-line / configuration file options
   // 
   CommandLine cl;
   if (!cl.parse(argc,argv)) return 0;

   string         input     = cl.getValue<string>  ("input");
   string         output    = cl.getValue<string>  ("output", "l5.root");
   TString        outputDir = cl.getValue<TString> ("outputDir",   "./");
   vector<string> algs      = cl.getVector<string> ("algs",          "");
   bool           batch     = cl.getValue<bool>    ("batch",      false);

   if (!cl.partialCheck()) return 0;
   cl.print();

   //
   // more checks
   //
   if(outputDir.Length() > 0 && !outputDir.EndsWith("/")) outputDir+="/";

   //
   // run a tapplication if not in batch mode
   //
   argc = (batch) ? 2 : 1; if (batch) argv[1] = (char*)"-b";
   TApplication* app = new TApplication("jet_l5_correction_x",&argc,argv);


   //
   // create the output file
   //
   TFile *ofile = new TFile(outputDir+output.c_str(),"RECREATE");
   if (!ofile) {
      cout<<" Output file="<<ofile<<" could not be created"<<endl;
      return 0;
   }

   //
   // open input file and loop over directories (algorithms)
   //
   TFile* ifile = new TFile(input.c_str(),"READ");
   if (!ifile->IsOpen()) { cout<<"Can't open "<<input<<endl; return 0; }
  
   if (algs.size()==0) {
      TIter nextDir(ifile->GetListOfKeys());
      TKey* dirKey(0);
      while ((dirKey=(TKey*)nextDir())) {
         if (strcmp(dirKey->GetClassName(),"TDirectoryFile")!=0) continue;
         algs.push_back(dirKey->GetName());
      }
   }

   // Loop over the algorithms 
   for(unsigned int a=0; a<algs.size(); a++) {
      // analyze that algorithm.
      analyze_algo(algs[a], ifile, ofile, cl);
   }// for algos

   //
   // close output file
   //
   cout<<"Writing "<<output<<" ... "<<flush;
   ofile->Close();
   delete ofile;
   ifile->Close();
   delete ifile;
   cout<<"DONE"<<endl;
  
   if (!batch) app->Run();
  
   return 0;
  
}//main  

  
////////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
vector<TString> get_flavors(TList* list)
{
   cout << "\tGetting flavors ... " << endl;
   TIter nextDir(list);
   TKey* dirKey(0);
   vector<string> vec;
   map<string,int> mymap;
   while ((dirKey=(TKey*)nextDir())) {
      if (strcmp(dirKey->GetClassName(),"TH1F")!=0) continue;
      TString name = dirKey->GetName();
      vec = split(string(name),"_");
      if(vec.size()<4) continue;
      string flavor = vec[0];
      if(mymap.find(flavor)==mymap.end()) mymap[flavor]=1;
      else mymap[flavor]++;
   }
   
   vector<TString> result;
   for(map<string,int>::iterator it = mymap.begin(); it != mymap.end(); it++) {
         if(it->first.compare("slc")!=0 && it->first.compare("slb")!=0) {
            result.push_back(it->first);
            cout << "\t\t" << it->first << endl;
         }
         else {
            cout << "\t\tSkipping flavor " << it->first << endl;
         }
      }

   vector<TString>::iterator it;
   it = result.begin();
   result.insert(it,"all");
   cout << "\t\tAdding \"all\" to flavors (all jets)" << endl;
          
   cout << "\tDONE" << endl; 
   return result;
}

//______________________________________________________________________________
void analyze_algo(TString algo, TFile* ifile, TFile* ofile, CommandLine & cl)
{
   //
   // Report
   //
   cout<<"Doing algorithm "<<algo<<endl;

   string          era                        = cl.getValue<string>   ("era");
   vector<TString> flavors                    = cl.getVector<TString> ("flavors",                      "");
   TString         outputDir                  = cl.getValue<TString>  ("outputDir",                  "./");
   bool            printNormalizedCorrections = cl.getValue<bool>     ("printNormalizedCorrections", true);
   if (!cl.partialCheck()) return;

   //
   // more checks
   //
   if(!outputDir.EndsWith("/")) outputDir+="/";

   ifile->cd();
   TKey* key = gDirectory->GetKey(algo);
   if (!key) {
      cout << "***ERROR***analyze_algo::Could not retrieve key from " << algo << endl;
      return;
   }
   TDirectoryFile* idir = (TDirectoryFile*)key->ReadObj();
   gDirectory->cd(algo);

   if (flavors.size()==0) {
      flavors = get_flavors(gDirectory->GetListOfKeys());
   }

   TDirectoryFile* odir = (TDirectoryFile*)ofile->mkdir(algo);
   odir->cd();

   //
   // storage containers for normalized and un-normalized vabsrsp_eta and vabscor_eta
   //
   map<TString,vector<TGraphErrors*> > graphMap;
   //map<TString,vector<pair<double,double> > > etaMap;
   map<TString,ObjectLoader<TH1F> > hlEtaMap;

   //
   // create un-normalized abscor and absrsp graphs
   //
   for(unsigned int i=0; i<flavors.size(); i++)
   {
      vector<TGraphErrors*> v;
      graphMap[flavors[i]+"_vabsrsp_eta"] = v;
      graphMap[flavors[i]+"_vabscor_eta"] = v;
      graphMap[flavors[i]+"_vabsrsp_eta_norm"] = v;
      graphMap[flavors[i]+"_vabscor_eta_norm"] = v;
      ObjectLoader<TH1F> hl_eta;
      hlEtaMap[flavors[i]] = hl_eta;
      analyze_flavor(algo, flavors[i], idir, cl, graphMap[flavors[i]+"_vabsrsp_eta"],
                     graphMap[flavors[i]+"_vabscor_eta"], hlEtaMap[flavors[i]]);
   }

   for(unsigned int i=0; i<flavors.size(); i++)
   {
      //
      // perform the interpolation and normalization for the vabsrsp_eta_norm and vabscor_eta_norm graphs
      //
      interpolate_flavor(graphMap[flavors[i]+"_vabsrsp_eta"], graphMap[flavors[i]+"_vabsrsp_eta_norm"],
                         graphMap["all_vabsrsp_eta"]);
      interpolate_flavor(graphMap[flavors[i]+"_vabscor_eta"], graphMap[flavors[i]+"_vabscor_eta_norm"],
                         graphMap["all_vabscor_eta"]); 
      normalize_flavor(graphMap[flavors[i]+"_vabsrsp_eta_norm"],graphMap["all_vabsrsp_eta"]);
      normalize_flavor(graphMap[flavors[i]+"_vabscor_eta_norm"],graphMap["all_vabscor_eta"]);

      //
      // perform fit to the vabscor_eta and vabscor_eta_norm graphs
      //
      cout << "\tFitting flavor " << flavors[i] << " ... " << endl;
      perform_correction_fits(graphMap[flavors[i]+"_vabsrsp_eta"], graphMap[flavors[i]+"_vabscor_eta"], algo);
      cout << "\tFitting normalized flavor " << flavors[i] << " ... " << endl;
      perform_correction_fits(graphMap[flavors[i]+"_vabsrsp_eta_norm"], graphMap[flavors[i]+"_vabscor_eta_norm"],
                              algo);

      //
      // write the L5 correction text file for the current algorithm
      //
      TString txtfilename = outputDir + era + "_L5Flavor_" + get_algorithm_suffix(string(algo)) + ".txt";
      if(printNormalizedCorrections)
         print_text_file(txtfilename, flavors[i], graphMap[flavors[i]+"_vabscor_eta_norm"], hlEtaMap[flavors[i]],
                         i==0);
      else
         print_text_file(txtfilename, flavors[i], graphMap[flavors[i]+"_vabscor_eta"], hlEtaMap[flavors[i]],
                         i==0);
   }
   
   cout<<"\t"<<algo<<" is DONE."<<endl;
}

//______________________________________________________________________________
void analyze_flavor(TString algo, TString flavor, TDirectoryFile* idir, CommandLine & cl,
                    vector<TGraphErrors*>& vabsrsp_eta, vector<TGraphErrors*>& vabscor_eta,
                    ObjectLoader<TH1F>& hl_eta)
{
   // Report
   cout << "\tFilling flavor " << flavor << " ... ";

   bool mpv = cl.getValue<bool> ("mpv", false);
   if (!cl.partialCheck()) return;

   //
   // load the histograms
   //
   ObjectLoader<TH1F> hl_rsp;
   ObjectLoader<TH1F> hl_refpt;
   ObjectLoader<TH1F> hl_jetpt;
   if(flavor.CompareTo("all")==0) {
      hl_rsp.load_objects(idir,"RelRsp:JetEta:RefPt");
      hl_refpt.load_objects(idir,"RefPt:JetEta:RefPt");  
      hl_jetpt.load_objects(idir,"JetPt:JetEta:RefPt");
      hl_eta.load_objects(idir,"JetPt:JetEta:RefPt");
   }
   else {
      hl_rsp.load_objects(idir,string(flavor)+"_RelRsp:JetEta:RefPt");
      hl_refpt.load_objects(idir,string(flavor)+"_RefPt:JetEta:RefPt");  
      hl_jetpt.load_objects(idir,string(flavor)+"_JetPt:JetEta:RefPt");
      hl_eta.load_objects(idir,string(flavor)+"_JetPt:JetEta:RefPt");
   }
  
   //
   // absolute response/correction as a function of pT for each eta bin
   //
   vector<unsigned int> indices; TH1F* hrsp(0);
   hl_rsp.begin_loop();
   
   while ((hrsp=hl_rsp.next_object(indices))) {
      
      unsigned int ieta=indices[0];
      unsigned int ipt =indices[1];
        
      //
      // create new graphs if a new eta bin comes around
      //
      if (ipt==0) {
         vabsrsp_eta.push_back(new TGraphErrors());
         vabscor_eta.push_back(new TGraphErrors());
         stringstream ss;
         ss<<hl_rsp.minimum(0,ieta)<<"to"<<hl_rsp.maximum(0,ieta);
         if(flavor.CompareTo("all")==0) {
            vabsrsp_eta.back()->SetName(("AbsRspVsRefPt_JetEta"+ss.str()).c_str());
            vabscor_eta.back()->SetName(("AbsCorVsJetPt_JetEta"+ss.str()).c_str());
         }
         else {
            vabsrsp_eta.back()->SetName((flavor+"AbsRspVsRefPt_JetEta"+ss.str()));
            vabscor_eta.back()->SetName((flavor+"AbsCorVsJetPt_JetEta"+ss.str()));
         }
      }

      //
      // only add points to the graphs if the current histo is not empty
      // the current setting might be a little high
      //
      if (hrsp->GetEntries() > 10) {//hrsp->Integral()!=0) { 
           
         TF1*  frsp    = (TF1*)hrsp->GetListOfFunctions()->Last();
         //std::cout << "hrspName = " << hrsp->GetName() << ": frsp = " << frsp << std::endl;
         TH1F* hrefpt  = hl_refpt.object(indices);
         TH1F* hjetpt  = hl_jetpt.object(indices);

         assert(hrefpt->GetEntries()>0&&hjetpt->GetEntries()>0);
           
         double refpt  =hrefpt->GetMean();
         double erefpt =hrefpt->GetMeanError();
         double jetpt  =hjetpt->GetMean();
         double ejetpt =hjetpt->GetMeanError();

         double peak;
         double epeak;
         if(algo.Contains("calo"))
         {
            peak = (frsp==0 || !mpv)?hrsp->GetMean():frsp->GetParameter(1);
            epeak = (frsp==0 || !mpv)?hrsp->GetMeanError():frsp->GetParError(1);
         }
         else if(algo.Contains("pf"))
         {
            peak = (frsp==0 || !mpv)?hrsp->GetMean():frsp->GetParameter(1);
            epeak = (frsp==0 || !mpv)?hrsp->GetMeanError():frsp->GetParError(1);
         }
         else
         {
            peak = (frsp==0 || !mpv)?hrsp->GetMean():frsp->GetParameter(1);
            epeak = (frsp==0 || !mpv)?hrsp->GetMeanError():frsp->GetParError(1);
         }

         double absrsp = peak;
         double eabsrsp = epeak;
         double abscor = 0.0;
         double eabscor = 0.0;
           
         if (absrsp > 0) 
         {  
            abscor  =1.0/absrsp;
            eabscor = abscor*abscor*epeak;
         } 
         if ((abscor>0) && (absrsp>0) && (eabscor>1e-6) && (eabscor/abscor<0.5) && (eabsrsp>1e-6) 
             && (eabsrsp/absrsp<0.5)) { 
            int n = vabsrsp_eta.back()->GetN();
            vabsrsp_eta.back()->SetPoint     (n,refpt, absrsp);
            vabsrsp_eta.back()->SetPointError(n,erefpt,eabsrsp);
            vabscor_eta.back()->SetPoint     (n,jetpt, abscor);
            vabscor_eta.back()->SetPointError(n,ejetpt,eabscor);      
         }
         else cout << "\t\tabsrsp " << absrsp << " and eabsrsp " << eabsrsp << " and abscor " 
                   << abscor << " and eabscor " << eabscor << endl;
      }  
   }
   // Report when done
   cout << "DONE" << endl;
}

//______________________________________________________________________________
bool contains(const vector<string>& collection,const string& element)
{
   vector<string>::const_iterator it;
   for (it=collection.begin();it!=collection.end();++it)
      if ((*it)==element) return true;
   return false;
}

//______________________________________________________________________________
string get_legend_title(const string& alg)
{
   string title;
   string tmp(alg);
   if      (alg.find("kt")==0) { title = "k_{T}, D=";      tmp = tmp.substr(2); }
   else if (alg.find("sc")==0) { title = "SISCone, R=";    tmp = tmp.substr(2); }
   else if (alg.find("ic")==0) { title = "ItCone, R=";     tmp = tmp.substr(2); }
   else if (alg.find("mc")==0) { title = "MidCone. R=";    tmp = tmp.substr(2); }
   else if (alg.find("ca")==0) { title = "Cam/Aachen, D="; tmp = tmp.substr(2); }
   else if (alg.find("ak")==0) { title = "Anti k_{T}, D="; tmp = tmp.substr(2); }
   else return alg;
  
   string reco[6] = { "gen",  "calo",   "pfchs",       "pf",      "trk",      "jpt" };
   string RECO[6] = { "(Gen)","(Calo)", "(PFlow+CHS)", "(PFlow)", "(Tracks)", "(JPT)" };

   string::size_type pos=string::npos; int ireco=-1;
   while (pos==string::npos&&ireco<5) { pos = tmp.find(reco[++ireco]); }
   if (pos==string::npos) return alg;
  
   double jet_size; stringstream ss1; ss1<<tmp.substr(0,pos); ss1>>jet_size;
   jet_size/=10.0;  stringstream ss2; ss2<<jet_size;

   title += ss2.str() + " " + RECO[ireco];

   return title;
}

//______________________________________________________________________________
string get_algorithm_suffix(const string& alg)
{
   string result;
   result += std::toupper(alg[0]);
   result += std::toupper(alg[1]);
   result += alg[2];
   if      (alg.find("calol1off")  ==3) result += "Calol1off";
   else if (alg.find("calol1")     ==3) result += "Calol1";
   else if (alg.find("calo")       ==3) result += "Calo";
   else if (alg.find("jptl1")      ==3) result += "JPTl1";
   else if (alg.find("jpt")        ==3) result += "JPT";
   else if (alg.find("pfchsl1off") ==3) result += "PFchsl1off";
   else if (alg.find("pfchsl1")    ==3) result += "PFchsl1";
   else if (alg.find("pfchs")      ==3) result += "PFchs";
   else if (alg.find("pfl1off")    ==3) result += "PFl1off";
   else if (alg.find("pfl1")       ==3) result += "PFl1";
   else if (alg.find("pf")         ==3) result += "PF";
   else if (alg.find("trk")        ==3) result += "TRK";
   else if (alg.find("tau")        ==3) result += std::string(alg, 3);
   else {
      cout << "***ERROR***get_algorithm_suffix::algorithm "<<alg<<" is not known"<<endl;
   } 
   return result;
}

//______________________________________________________________________________
void interpolate_flavor(vector<TGraphErrors*>& v, vector<TGraphErrors*>& vint, vector<TGraphErrors*>& vall)
{
   for(unsigned int i=0; i<vall.size(); i++)
   {
      vint.push_back(new TGraphErrors());
      vint.back()->SetName(TString(v[i]->GetName())+"_norm");
      
      for(int j=0; j<vall[i]->GetN(); j++)
      {
         double allx = 0;
         double ally = 0;
         vall[i]->GetPoint(j,allx,ally);
         double ex = v[i]->GetErrorX(j);
         double inty = v[i]->Eval(allx);
         double intey = evalError(v[i],allx);
/*
         if(i==4 && j==0){
            double x;
            double y;
            v[i]->GetPoint(j,x,y);
            cout << "interpolate_flavor::" << v[i]->GetName() << endl;
            cout << "interpolate_flavor::allx="<<allx<<"\tinty="<<inty<<"\ty="<<y<<endl;
         }
*/
         vint[i]->SetPoint(j,allx,inty);
         vint[i]->SetPointError(j,ex,intey);
      }
   }
}

//______________________________________________________________________________
void normalize_flavor(vector<TGraphErrors*>& vnum,vector<TGraphErrors*>& vden)
{
   for(unsigned int i=0; i<vnum.size(); i++)
   {
      assert(vnum[i]->GetN()==vden[i]->GetN());
      for(int j=0; j<vnum[i]->GetN(); j++)
      {
         double numx = 0;
         double numy = 0;
         vnum[i]->GetPoint(j,numx,numy);
         double numex = vnum[i]->GetErrorX(j);
         double numey = vnum[i]->GetErrorY(j);
         double denx = 0;
         double deny = 0;
         vden[i]->GetPoint(j,denx,deny);
         double deney = vden[i]->GetErrorY(j);
         double newey = TMath::Sqrt((TMath::Power((1/deny),2)*TMath::Power(numey,2))+
                                    (TMath::Power((numy/TMath::Power(deny,2)),2)*TMath::Power(deney,2)));
/*
         if(i==4 && j==0)
            cout << "normalize_flavor::numx="<<numx<<"\tinty=numy="<<numy<<"\tdeny=ally="<<deny<<"\tnumy/deny"<<numy/deny<<endl;
*/
         vnum[i]->SetPoint(j,numx,numy/deny);
         vnum[i]->SetPointError(j,numex,newey);
      }
   }
}

//______________________________________________________________________________
void perform_correction_fits(vector<TGraphErrors*>& vabsrsp_eta, vector<TGraphErrors*>& vabscor_eta,
                             TString algo)
{
   assert(vabsrsp_eta.size()==vabscor_eta.size());
   for(unsigned int ieta=0; ieta<vabsrsp_eta.size(); ieta++)
   {
      TGraphErrors* gabsrsp = vabsrsp_eta[ieta];
      TGraphErrors* gabscor = vabscor_eta[ieta];
      if(!gabsrsp || !gabscor) {
         cout<<"\t***ERROR***perform_correction_fits::Either gabsrsp or gabscor does not exist"<<endl
             <<"\tSkipping this gabscor graph"<<endl;
         continue;
      }
      if (gabsrsp->GetN()==0 || gabscor->GetN()==0) {
         cout << "\t***ERROR***perform_correction_fits::gabsrsp has " << gabsrsp->GetN()
              << " points and gabscor has " << gabscor->GetN() << " points."<<endl
              << "\tGraph name = " << vabscor_eta[ieta]->GetName() << endl;
         cout << "\tExiting the program." << endl; 
         exit(0);
         //continue;
      }
      TF1* fabscor(0);
      int npoints = gabscor->GetN(); 
      double xmin(1.0),xmax(100.0);
      if (npoints > 0)
      {
         xmin = gabscor->GetX()[0];
         xmax = gabscor->GetX()[gabscor->GetN()-1];
      }

      //
      // We don't want to fit for pt less than 10 GeV for PF and PFchs, 20 GeV for JPT, or 30 GeV for calo  
      // These are the recommended limits for everyone. Thus, improving the fits for high pt by setting a
      // limit on the fit range is okay. None of the corrections below these cuts will be of much use.
      //
      if (algo.Contains("pf")) {
//my test fit
//[2]*log10([0]*x)-[1]
//Ricardo fit
//[0]+log10((x-[3])/[4])*([1]+[2]*log10((x-[3])/[4]))
         xmin = 10;
         fabscor=new TF1("fit","[0]+[1]/(pow(log10(x),2)+[2])+[3]*exp(-[4]*(log10(x)-[5])*(log10(x)-[5]))",xmin,xmax);
         fabscor->SetParameter(0,0.5);
         fabscor->SetParameter(1,9.0);
         fabscor->SetParameter(2,8.0);
         fabscor->SetParameter(3,-0.3);
         fabscor->SetParameter(4,0.6);
         fabscor->SetParameter(5,1.0);
      }
      else if (algo.Contains("trk")) {
         fabscor=new TF1("fit","[0]+[1]*pow(x/500.0,[2])+[3]/log10(x)+[4]*log10(x)",xmin,xmax);
         fabscor->SetParameter(0,1.7);
         fabscor->SetParameter(1,0.7);
         fabscor->SetParameter(2,3.0);
         fabscor->SetParLimits(2,1,10);
         fabscor->SetParameter(3,0.0);
         fabscor->SetParameter(4,0.0);
      }
      else if (algo.Contains("jpt")) {
         xmin = 20;
         fabscor=new TF1("fit","[0]+[1]/(pow(log10(x),2)+[2])+[3]*exp(-[4]*(log10(x)-[5])*(log10(x)-[5]))",xmin,xmax);
         //
         // INITIAL PARAMS: These are the fitted parameters that work for JetEta0.261to0.348
         //
         fabscor->SetParameter(0,-8.9);
         fabscor->SetParameter(1,530);
         fabscor->SetParameter(2,16);
         fabscor->SetParameter(3,-22);
         fabscor->SetParameter(4,0.06);
         fabscor->SetParameter(5,-0.28);
      }
      else if (algo.Contains("calo")) {
         xmin = 30;
         /*
         fabscor=new TF1("fit","[0]+[1]/(pow(log10(x),[2])+[3])",xmin,xmax);
         fabscor->SetParameter(0,1.0);
         fabscor->SetParameter(1,5.0);
         fabscor->SetParameter(2,3.0);
         fabscor->SetParameter(3,3.0);
         */
         fabscor=new TF1("fit","[0]+[1]/(pow(log10(x),2)+[2])+[3]*exp(-[4]*(log10(x)-[5])*(log10(x)-[5]))",xmin,xmax);
         fabscor->SetParameter(0,0.5);
         fabscor->SetParameter(1,9.0);
         fabscor->SetParameter(2,8.0);
         fabscor->SetParameter(3,-0.3);
         fabscor->SetParameter(4,0.6);
         fabscor->SetParameter(5,1.0);

      }
      else {
         cout << "WARNING::Cannot determine fit function for " << algo << "." << endl;
      }
      //
      // This warning is obsolete and incorrect
      //
      //cout << "\tWARNING::Setting xmin = " << xmin << endl
      //     << "\tOnly corrections after this point should be used." << endl;
           
      //
      // obtain the best fit of the function fabscor to the histo gabscor
      //
      perform_smart_fit(gabscor,fabscor);

      //
      // format the graphs
      //
      gabscor->GetListOfFunctions()->First()->ResetBit(TF1::kNotDraw);
      gabsrsp->SetMarkerStyle(20); 
      gabscor->SetMarkerStyle(20); 
      gabsrsp->Write();
      gabscor->Write();
   }
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

//______________________________________________________________________________
void perform_smart_fit(TGraphErrors * gabscor, TF1 * fabscor) {

   int maxFitIter = 30;
   int fitIter = 0;
   vector<double> bestPars;
   double bestRChi2 = 0;
   do {	       

      //
      // do the fit, get the results and the parameters of the fitted function
      //
      TFitResultPtr fitResPtr = gabscor->Fit(fabscor,"RQ0S");
      vector<double> auxPars = fitResPtr.Get()->Parameters();

      //
      // compute the reduced chi2 of this fit and if it is the best fit so far
      // then save the parameters
      //
      double rchi2 = fitResPtr.Get()->Chi2()/ fitResPtr.Get()->Ndf();
      if (rchi2 > 0 && (rchi2<bestRChi2 || bestRChi2==0)){
         bestRChi2 = rchi2;
         bestPars  = auxPars;
      } 

      //
      // increment the counter
      //
      fitIter++;
   }while(( bestRChi2 > 2 || bestRChi2 == 0 ) && fitIter < maxFitIter);
 
   //
   // set the best parameters and chi2 to the fit function
   //
   TF1 * ffh = gabscor->GetFunction("fit");
   for (unsigned int np=0;np < bestPars.size() ; np++){
      ffh->SetParameter(np,bestPars[np]);
      fabscor->SetParameter(np,bestPars[np]);
   }
   fabscor->SetChisquare(bestRChi2 * fabscor->GetNDF());
   ffh->SetChisquare(bestRChi2 * fabscor->GetNDF());
		
   //   
   // check for failed fits
   // a chi2 of zero is symptomatic of a failed fit.
   //
   if (bestRChi2 < 0.001){
      cout<<"\t\t***ERROR***, FIT HAS FAILED for histo "<<gabscor->GetName()
          <<" which has a reduced chi2="<<bestRChi2
          <<" after "<<fitIter<<" iterations. "<<endl;
   }

   //
   // check for large reduced chi2's
   // above 10 is a plain error; between 5 and 10 is a warning
   //
   if (bestRChi2 > 5){
      if (bestRChi2 > 10)
         cout<<"\t\t***ERROR***,";
      else
         cout<<"\t\tWARNING,";

      cout<<" fit for histo "<<gabscor->GetName()
          <<" has a reduced chi2="<<bestRChi2
          <<" after "<<fitIter<<" iterations"<<endl;
   }
}

//______________________________________________________________________________
vector<string> split(const string& str,const string& delim)
{
   vector<string> result;                             
   string tmp=str;                                    
   while (tmp.length()>0) {                           
      size_t pos=tmp.find(delim);                      
      if (pos==string::npos) {                         
         result.push_back(tmp);                         
         tmp="";                                        
      }                                                
      else {                                           
         result.push_back(tmp.substr(0,pos));           
         tmp=tmp.substr(pos+delim.length());            
      }                                                
   }                                                  
   return result;                                     
}

//______________________________________________________________________________
double evalError(TGraphErrors* g, double x)
{
   if (g->GetN() == 0) return 0;
   if (g->GetN() == 1) return g->GetEY()[0];

   //
   //linear interpolation
   //In case x is < g->GetX()[0] or x > g->GetX()[g->GetN()-1] return the extrapolated point
   
   //find points in graph around x assuming points are not sorted
   // (if point are sorted could use binary search)
   
   // find neighbours simply looping  all points
   // and find also the 2 adjacent points: (low2 < low < x < up < up2 )
   // needed in case x is outside the graph ascissa interval
   //
   Int_t low  = -1;
   Int_t up  = -1;
   Int_t low2 = -1;
   Int_t up2 = -1;
   
   for (Int_t i = 0; i < g->GetN(); ++i)
   {
      if (g->GetX()[i] < x) {
         if (low == -1 || g->GetX()[i] > g->GetX()[low])
         {
            low2 = low;
            low = i;
         }
         else if (low2 == -1)
            low2 = i;
      } 
      else if (g->GetX()[i] > x) {
         if (up  == -1 || g->GetX()[i] < g->GetX()[up])
         {
            up2 = up;
            up = i;
         }
         else if (up2 == -1) 
            up2 = i;
      }
      //
      // case x == fX[i]
      // no interpolation needed
      //
      else
         return g->GetEY()[i];
   }
   
   //
   // treat cases when x is outside graph min max abscissa
   //
   if (up == -1)  {
      up  = low;
      low = low2;
   }
   if (low == -1) {
      low = up;
      up  = up2;
   }
   
   assert(low != -1 && up != -1);
   
   if (g->GetX()[low] == g->GetX()[up]) return g->GetEY()[low];
   Double_t eyn = g->GetEY()[up] + (x - g->GetX()[up]) * (g->GetEY()[low] - g->GetEY()[up]) / (g->GetX()[low] - g->GetX()[up]);
   return eyn;
}               

//______________________________________________________________________________
void print_text_file(TString filename, TString flavor,vector<TGraphErrors*> & vabscor_eta,
                     ObjectLoader<TH1F> & hl_eta, bool print_header)
{
   //
   // Report
   //
   cout<<"\tPrinting text file "<<filename<<" ... "<<flush;

   ofstream fout(filename,print_header ? ios_base::out : ios_base::app);
   fout.setf(ios::right); 

   if(print_header) {
      fout << "# L5 flavor corrections for IC5 algorithm" << endl;
      fout << "# [gJ] (gluons from diJet mixture)" << endl;
      fout << "# [qJ] (uds quarks from diJet mixture)" << endl;
      fout << "# [cJ] (c quark from diJet mixture)" << endl;
      fout << "# [bJ] (b quark from diJet mixture)" << endl;
      fout << "# [gT] (gluons from ttbar events)" << endl;
      fout << "# [qT] (uds quarks from ttbar events)" << endl;
      fout << "# [cT] (c quark from ttbar events)" << endl;
      fout << "# [bT] (b quark from ttbar events)" << endl;
      fout << "# energy mapping: ptGen = (pt - p5)/p6" << endl;
      fout << "# parametrization: p2+p3*logPt+p4*logPt^2, constant if Pt<p0 or Pt>p1" << endl;
      fout << "#etamin  etamax  #ofparameters  ptmin  ptmax    p2         p3        p4     mapping: p5        p6" << endl;
   }

   bool formula_printed = false;
   for (unsigned int ieta=0;ieta<vabscor_eta.size();ieta++) { 
      if(ieta==0) fout<<"["<<get_flavor_name(flavor)<<"]"<<endl;
      TGraph* grelcor;
      grelcor = vabscor_eta[ieta];
      TF1* frelcor = (TF1*)grelcor->GetListOfFunctions()->First();
      if(frelcor!=0) {
         if(!formula_printed) {
            fout<<"{1 JetEta 1 JetPt "<<frelcor->GetExpFormula()<<" Correction L5Flavor}"<<endl;
            formula_printed = true;
         }
         double etamin  = hl_eta.minimum(0,ieta);
         double etamax  = hl_eta.maximum(0,ieta);
         double ptmin = grelcor->GetX()[0];
         double ptmax = grelcor->GetX()[grelcor->GetN()-1];
         fout<<setw(11)<<etamin
             <<setw(11)<<etamax
             <<setw(11)<<(int)(frelcor->GetNpar()+2) //Number of parameters + 2 
             <<setw(12)<<ptmin
             <<setw(12)<<ptmax;
         for(int p=0; p<frelcor->GetNpar(); p++)
         {
            fout<<setw(13)<<frelcor->GetParameter(p);
         }
         fout<<endl;
      }
      else {
         cout << "\t\tWARNING::frelcor is missing for graph named " << grelcor->GetName() << endl
              << "\t\tSkipping this eta region" << endl;
      }
   }
   fout.close();
   cout << "DONE" << endl;
}

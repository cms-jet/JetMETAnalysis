////////////////////////////////////////////////////////////////////////////////
//
// jet_l2_correction_x
// -------------------
//
//            08/16/2008 Kostas Kousouris                    <kkousour@fnal.gov>
//                       Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/ObjectLoader.h"
#include "JetMETAnalysis/JetUtilities/interface/RootStyle.h"

#include "TApplication.h"
#include "TFile.h"
#include "TKey.h"
#include "TObjArray.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TString.h"
#include "TLatex.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>


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

/// this method performs consecutive fittings (up to maxFitIter) but uses the
/// parameters from the one with the lowest chi2 that has not failed.
void perform_smart_fit(TGraphErrors * gabscor, TF1 * fabscor);

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
  string         era       = cl.getValue<string>  ("era");
  string         l3input   = cl.getValue<string>  ("l3input","l3.root");
  TString        output    = cl.getValue<TString> ("output", "l2.root");
  TString        outputDir = cl.getValue<TString> ("outputDir",   "./");
  vector<string> formats   = cl.getVector<string> ("formats",       "");
  vector<string> algs      = cl.getVector<string> ("algs",          "");
  bool           batch     = cl.getValue<bool>    ("batch",      false);
  bool           l2l3      = cl.getValue<bool>    ("l2l3",       false);
  bool           mpv       = cl.getValue<bool>    ("mpv",        false);

  if (!cl.check()) return 0;
  cl.print();

  //
  // run a tapplication if not in batch mode
  //
  argc = (batch) ? 2 : 1; if (batch) argv[1] = (char*)"-b";
  TApplication* app = new TApplication("jet_l2_correction_x",&argc,argv);
  
  
  //
  // open output file
  //
  if(!outputDir.EndsWith("/")) outputDir+="/";
  TFile* ofile = new TFile(outputDir+output,"RECREATE");
  if (!ofile->IsOpen()) { cout<<"Can't create "<<output<<endl; return 0; }
  
  
  //
  // open input & l3input files and loop over directories (algorithms)
  //
  TFile* ifile = new TFile(input.c_str(),"READ");
  if (!ifile->IsOpen()) { cout<<"Can't open "<<input<<endl; return 0; }
  
  TFile* l3file = 0;
  if (!l2l3) {
    l3file = new TFile(l3input.c_str(),"READ");
    if (!l3file->IsOpen()) {
      cout<<"Can't open "<<l3input<<endl;
      return 0;
    }
  }
  
  if (algs.size()==0) {
    TIter nextDir(ifile->GetListOfKeys());
    TKey* dirKey(0);
    while ((dirKey=(TKey*)nextDir())) {
      if (strcmp(dirKey->GetClassName(),"TDirectoryFile")!=0) continue;
      algs.push_back(dirKey->GetName());
    }
  }
  
  TIter nextDir(ifile->GetListOfKeys());
  TKey* dirKey(0);
  while ((dirKey=(TKey*)nextDir())) {
    if (strcmp(dirKey->GetClassName(),"TDirectoryFile")!=0) continue;
    TDirectoryFile* idir = (TDirectoryFile*)dirKey->ReadObj();
    string alg(idir->GetName()); if (!contains(algs,alg)) continue;
    
     cout << alg << " ... " << endl;
     
     //
     // Get the response from the l3 file only if l2l3 is set to false;
     //
     TF1* fl3rsp = 0;
     if (!l2l3) {
       TDirectoryFile* l3dir = (TDirectoryFile*)l3file->Get(alg.c_str());
       if (l3dir==0) {
         cout<<"Failed to rerieve L3 correction for "<<alg<<", skip"<<endl;
         continue;
       }
       
       TGraphErrors* gl3rsp = (TGraphErrors*)l3dir->Get("L3RspVsRefPt");
       fl3rsp = (TF1*)gl3rsp->GetListOfFunctions()->First();
       if (0==fl3rsp) {
         cout<<"Failed to retrieve L3 correction for "<<alg<<", skip"<<endl;
         continue;
       }
     }//if (!l2l3)

     TDirectoryFile* odir = (TDirectoryFile*)ofile->mkdir(alg.c_str());
     odir->cd();
     
     ObjectLoader<TH1F> hl_rsp;
     
     hl_rsp.load_objects(idir,"RelRsp:JetEta:RefPt");
     
     ObjectLoader<TH1F> hl_refpt;
     hl_refpt.load_objects(idir,"RefPt:JetEta:RefPt");
     
     ObjectLoader<TH1F> hl_jetpt;
     hl_jetpt.load_objects(idir,"JetPt:JetEta:RefPt");
     
     
     //
     // absolute response/correction as a function of pT for each eta bin
     //
     vector<TGraphErrors*> vabsrsp_eta;
     vector<TGraphErrors*> vabscor_eta;
     
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
           vabsrsp_eta.back()->SetName(("AbsRspVsRefPt_JetEta"+ss.str()).c_str());
           vabscor_eta.back()->SetName(("AbsCorVsJetPt_JetEta"+ss.str()).c_str());
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
           if(alg.find("calo")!=string::npos)
           {
             peak = (frsp==0 || !mpv)?hrsp->GetMean():frsp->GetParameter(1);
             epeak = (frsp==0 || !mpv)?hrsp->GetMeanError():frsp->GetParError(1);
           }
           else if(alg.find("pf")!=string::npos)
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
           if ((abscor>0) && (absrsp>0) && (eabscor>1e-5) && (eabscor/abscor<0.5) && (eabsrsp>1e-4) && (eabsrsp/absrsp<0.5)) { 
              int n = vabsrsp_eta.back()->GetN();
              vabsrsp_eta.back()->SetPoint     (n,refpt, absrsp);
              vabsrsp_eta.back()->SetPointError(n,erefpt,eabsrsp);
              vabscor_eta.back()->SetPoint     (n,jetpt, abscor);
              vabscor_eta.back()->SetPointError(n,ejetpt,eabscor);      
           }
           else cout << "absrsp " << absrsp << " and eabsrsp " << eabsrsp << " and abscor " << abscor << " and eabscor " << eabscor << endl;
        }
        
        //
        // fit graphs if last pt of the current eta bin comes around
        //
        if (ipt==hl_jetpt.nobjects(1)-1 && (vabsrsp_eta.back())->GetN()!=0 && (vabscor_eta.back())->GetN()!=0) {
           TGraphErrors* gabsrsp = vabsrsp_eta.back();
           TGraphErrors* gabscor = vabscor_eta.back();
           TF1*          fabscor(0);
           int npoints = gabscor->GetN(); 
           double xmin(1.0),xmax(100.0);
           if (npoints > 0)
           {
              xmin = gabscor->GetX()[0];
              xmax = gabscor->GetX()[gabscor->GetN()-1];
           }

           //
           // we don't want to fit for pt less than 5 GeV as even a corrected calo jet of 10 
           // will be at least 5 Gev in raw energy.
           //
           if (xmin<7) xmin=7;
           if (npoints<3) {
              gabscor->SetPoint     (0, 10.0,1.0);
              gabscor->SetPointError(0,  0.0,0.0);
              gabscor->SetPoint     (1,100.0,1.0);
              gabscor->SetPointError(1,  0.0,0.0);
              fabscor = new TF1("fit","[0]",10.0,100.0);
              fabscor->FixParameter(0,1.0);
           }
           else if (!l2l3 && npoints > 2 && gabscor->GetN()<10) {
             fabscor=new TF1("fit","[0]+[1]*log10(x)+[2]*pow(log10(x),2)",xmin,xmax);
             fabscor->SetParameter(0,1.0);
             fabscor->SetParameter(1,0.0);
             fabscor->SetParameter(2,0.0);
           }
           else {
             if (alg.find("pf")!=string::npos) {
                //
                // online
                //
                if(alg.find("HLT")!=string::npos){
                   fabscor=new TF1("fit","(x>=[6])*([0]+[1]/(pow(log10(x),2)+[2])+[3]*exp(-[4]*(log10(x)-[5])*(log10(x)-[5])))+(x<[6])*[7]",xmin,xmax);
                   fabscor->FixParameter(6,xmin);
                   fabscor->FixParameter(7,0.0);
                }
                //
                // offline
                //
                else
                   fabscor=new TF1("fit","[0]+[1]/(pow(log10(x),2)+[2])+[3]*exp(-[4]*(log10(x)-[5])*(log10(x)-[5]))",xmin,xmax);
                
                fabscor->SetParameter(0,0.5);
                fabscor->SetParameter(1,9.0);
                fabscor->SetParameter(2,8.0);
                fabscor->SetParameter(3,-0.3);
                fabscor->SetParameter(4,0.6);
                fabscor->SetParameter(5,1.0);
               
                fabscor->SetParLimits(2,0.1,100);
                fabscor->SetParLimits(3,-100,0);
                fabscor->SetParLimits(4,0,100);

               if (xmax < 15) {
                  fabscor->FixParameter(1,0.0);
                  fabscor->FixParameter(2,0.0);
                  fabscor->FixParameter(3,0.0);
                  fabscor->FixParameter(4,0.0);
                  fabscor->FixParameter(5,0.0);
               }
             }
             else if (alg.find("trk")!=string::npos) {
               fabscor=new TF1("fit","[0]+[1]*pow(x/500.0,[2])+[3]/log10(x)+[4]*log10(x)",xmin,xmax);
               fabscor->SetParameter(0,1.7);
               fabscor->SetParameter(1,0.7);
               fabscor->SetParameter(2,3.0);
               fabscor->SetParLimits(2,1,10);
               fabscor->SetParameter(3,0.0);
               fabscor->SetParameter(4,0.0);
             }
             else if (alg.find("jpt")!=string::npos) {
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
             else if (alg.find("calo")!=string::npos) {
               if (xmin<6) xmin=6;
               fabscor=new TF1("fit","[0]+[1]/(pow(log10(x),[2])+[3])",xmin,xmax);
               fabscor->SetParameter(0,1.0);
               fabscor->SetParameter(1,5.0);
               fabscor->SetParameter(2,3.0);
               fabscor->SetParameter(3,3.0);

               fabscor->SetParLimits(3,0,100);
             }
             else {
               cout << "WARNING::Cannot determine fit function for " << alg << "." << endl;
             }
           }
           
           //
           // obtain the best fit of the function fabscor to the histo gabscor
           //
           perform_smart_fit(gabscor,fabscor);
           if (alg.find("pf")!=string::npos)
              if (alg.find("HLT")!=string::npos) {
                 ((TF1*)gabscor->GetListOfFunctions()->First())->FixParameter(7,fabscor->Eval(fabscor->GetParameter(6)));
                 fabscor->FixParameter(7,fabscor->Eval(fabscor->GetParameter(6)));
              }

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
     
     
     //
     // relative (L2) response/correction as a function of pT for each eta bin
     //
     string fnc_as_str = (alg.find("trk")!=string::npos) ? 
       "[0]+[1]*log10(x)+[2]*pow(log10(x),2)+[3]*pow(log10(x),3)+[4]*pow(x/500.0,3)" :
       "[0]+[1]*log10(x)+[2]*pow(log10(x),2)+[3]*pow(log10(x),3)+[4]*pow(log10(x),4)";
     
     vector<TGraph*> vrelcor_eta;
     TH1F*           hjetpt(0);
     hl_jetpt.begin_loop();
     while ((hjetpt=hl_jetpt.next_object(indices))) {
        
        unsigned int ieta = indices[0];
        unsigned int ipt  = indices[1];
        
        // create a new graph if a new eta bin comes around
        if (ipt==0) {
           vrelcor_eta.push_back(new TGraphErrors());
           stringstream ss;
           ss<<hl_jetpt.minimum(0,ieta)<<"to"<<hl_jetpt.maximum(0,ieta);
           vrelcor_eta.back()->SetName(("RelCorVsJetPt_JetEta"+ss.str()).c_str());
        }
        
        // only add a point to the graph if the current histo is not empty
        if (hjetpt->Integral()!=0) {
           TF1*   fabscor  =vabscor_eta[ieta]->GetFunction("fit");
           double jetpt    =hjetpt->GetMean();
           double refpt    =jetpt*fabscor->Eval(jetpt);
           double l3cor    = 1;
           if (!l2l3) l3cor = fl3rsp->Eval(refpt);
           double controlpt=refpt*l3cor;
           double relcor   =controlpt/jetpt;
           if (relcor > 5)
              cout<<"WARNING !!! suspicious point: "<<hjetpt->GetName()
                  <<", jet pt = "<<jetpt<<", ref pt = "<<refpt<<" "<<endl;
           else { 
              int n=vrelcor_eta.back()->GetN();
              vrelcor_eta.back()->SetPoint(n,jetpt,relcor);
           }
        }
        
        // fit the graph if the last pt of the current eta bin comes around
        if (ipt==hl_jetpt.nobjects(1)-1 && (vrelcor_eta.back())->GetN()!=0) {
           TGraph* grelcor = vrelcor_eta.back();
           double  xmin    = grelcor->GetX()[0];
           double  xmax    = grelcor->GetX()[grelcor->GetN()-1];
           TF1*    frelcor = new TF1("fit",fnc_as_str.c_str(),xmin,xmax);
                     
           frelcor->SetParameter(0,0.0);
           frelcor->SetParameter(1,0.0);
           frelcor->SetParameter(2,0.0);
           frelcor->SetParameter(3,0.0);
           frelcor->SetParameter(4,0.0);
           frelcor->SetParameter(5,0.0);
           
           if (grelcor->GetN()<2) {
              grelcor->SetPoint(0,10,1.0);
              grelcor->SetPoint(1,100,1.0);
              frelcor->FixParameter(1,0.0);
              frelcor->FixParameter(2,0.0);
              frelcor->FixParameter(3,0.0);
              frelcor->FixParameter(4,0.0);
              frelcor->FixParameter(5,0.0);
           }
           else if (grelcor->GetN()==2) {
              frelcor->FixParameter(2,0.0);
              frelcor->FixParameter(3,0.0);
              frelcor->FixParameter(4,0.0);
              frelcor->FixParameter(5,0.0);
           }
           
           grelcor->Fit(frelcor,"QRB0");
           grelcor->GetListOfFunctions()->First()->ResetBit(TF1::kNotDraw);
        grelcor->SetMarkerStyle(20); 
        grelcor->Write();
        }
     }
    
     //
     // write the L2 correction text file for the current algorithm
     //
     TString txtfilename = outputDir + era + "_L2Relative_" + get_algorithm_suffix(alg) + ".txt";
     ofstream fout(txtfilename);
     fout.setf(ios::right); 

     unsigned int vector_size = 0;
     if(l2l3) vector_size = vabscor_eta.size(); //For L2L3 Corrections Together
     else vector_size = vrelcor_eta.size(); //For L2 & L3 Corrections Separate
     for (unsigned int ieta=0;ieta<vector_size;ieta++) { 
       TGraph* grelcor;
       if(l2l3) grelcor = vabscor_eta[ieta]; //For L2L3 Corrections Together
       else grelcor = vrelcor_eta[ieta]; //For L2 & L3 Corrections Separate
       TF1* frelcor = (TF1*)grelcor->GetListOfFunctions()->First();
       if(frelcor!=0) {
         if(ieta==0) fout<<"{1 JetEta 1 JetPt "<<frelcor->GetExpFormula()<<" Correction L2Relative}"<<endl;
         double  etamin  = hl_jetpt.minimum(0,ieta);
         double  etamax  = hl_jetpt.maximum(0,ieta);
         double  ptmin = grelcor->GetX()[0];
         double  ptmax = grelcor->GetX()[grelcor->GetN()-1];
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
     }
     fout.close();
     
     cout<<alg<<" is DONE."<<endl;
  }
  
  //
  // close output file
  //
  cout<<"Write "<<output<<" ... "<<flush;
  ofile->Close();
  delete ofile;
  ifile->Close();
  delete ifile;
  cout<<"DONE"<<endl;
  
  
  if (!batch) app->Run();
  
  return 0;
}


////////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

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
  
  string reco[9] = { "gen", "caloHLT", "calo", "pfHLT", "pfchsHLT", "pfchs", "pf", "trk", "jpt" };
  string RECO[9] = { "(Gen)", "(Calo@HLT)", "(Calo)", "(PFlow@HLT)", "(PFlow+CHS@HLT)", "(PFlow+CHS)", "(PFlow)", "(Tracks)", "(JPT)" };

  string::size_type pos=string::npos; int ireco=-1;
  while (pos==string::npos&&ireco<8) { pos = tmp.find(reco[++ireco]); }
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
  else if (alg.find("caloHLTl1")  ==3) result += "CaloHLTl1";
  else if (alg.find("caloHLT")    ==3) result += "CaloHLT";
  else if (alg.find("calo")       ==3) result += "Calo";
  else if (alg.find("jptl1")      ==3) result += "JPTl1";
  else if (alg.find("jpt")        ==3) result += "JPT";
  else if (alg.find("pfchsHLTl1") ==3) result += "PFchsHLTl1";
  else if (alg.find("pfchsHLT")   ==3) result += "PFchsHLT";
  else if (alg.find("pfchsl1off") ==3) result += "PFchsl1off";
  else if (alg.find("pfchsl1")    ==3) result += "PFchsl1";
  else if (alg.find("pfchs")      ==3) result += "PFchs";
  else if (alg.find("pfHLTl1")    ==3) result += "PFHLTl1";
  else if (alg.find("pfHLT")      ==3) result += "PFHLT";
  else if (alg.find("pfl1off")    ==3) result += "PFl1off";
  else if (alg.find("pfl1")       ==3) result += "PFl1";
  else if (alg.find("pf")         ==3) result += "PF";
  else if (alg.find("trk")        ==3) result += "TRK";
  else if (alg.find("tau")        ==3) result += std::string(alg, 3);
  cout<<"get_algorithm_suffix: result = "<<result<<" from algo = "<<alg<<endl;
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
    if (fitResPtr.Get()->Ndf() == 0) rchi2 = 0;
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
  // warn if the fit diverges at low pt
  //
  if (fabscor->Integral(0,10) > 25)
     cout << "\t***ERROR***, fit for histo " << gabscor->GetName() << " diverges at low pt" << endl;

  //   
  // check for failed fits
  // a chi2 of zero is symptomatic of a failed fit.
  //
  if (bestRChi2 < 0.001){
    cout<<"\t***ERROR***, FIT HAS FAILED for histo "<<gabscor->GetName()
	<<" which has a reduced chi2="<<bestRChi2
	<<" after "<<fitIter<<" iterations. "<<endl;
  }

  //
  // check for large reduced chi2's
  // above 10 is a plain error; between 5 and 10 is a warning
  //
  if (bestRChi2 > 5){
    if (bestRChi2 > 10)
      cout<<"\t***ERROR***,";
    else
      cout<<"\tWARNING,";

    cout<<" fit for histo "<<gabscor->GetName()
	<<" has a reduced chi2="<<bestRChi2
	<<" after "<<fitIter<<" iterations"<<endl;
  }
}

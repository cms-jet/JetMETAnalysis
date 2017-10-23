////////////////////////////////////////////////////////////////////////////////
//
// jet_l1_correction_x
// -------------------
//
//            10/31/2016 Alexx Perloff           <alexx.stephen.perloff@cern.ch>
////////////////////////////////////////////////////////////////////////////////

#include <map>
#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <algorithm>
#include <cstdio>

#include "TSystem.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TParallelCoord.h"
#include "TParallelCoordVar.h"
#include "TAxis.h"
#include "TH2.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TProfile3D.h"
#include "THnSparse.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TSpline.h"
#include "TMath.h"
#include "TH1.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TError.h"
#include "TPaveStats.h"
#include "TBenchmark.h"

#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/ProgressBar.hh"
#include "JetMETAnalysis/JetUtilities/interface/PiecewiseSpline.hh"
#include "JetMETAnalysis/JetUtilities/interface/RootStyle.h"
#include "JetMETAnalysis/JetUtilities/interface/Style.h"

#include <gsl/gsl_math.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_interp.h>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// declare global variables
////////////////////////////////////////////////////////////////////////////////
bool projThenDiv;
const float ABS_ERROR_MIN = 0.000001;
const float REL_ERROR_MAX = 0.5;
const float REL_ERROR_MIN = 0.01;
const float PT_CONT_MIN = 0;
const float PT_ERROR_MIN = 0.1;


////////////////////////////////////////////////////////////////////////////////
// declare classes and structs
////////////////////////////////////////////////////////////////////////////////

struct FitRes {
      double etalowedge;
      double etaupedge;
      double rholowedge;
      double rhoupedge;
      PiecewiseSpline * pspline;
      unsigned int igraph;
};

////////////////////////////////////////////////////////////////////////////////
// declare local functions
////////////////////////////////////////////////////////////////////////////////

// This method tries to obtain all the relevant TProfiles from the inputFilename
// It returns true if successfull
bool getInputHistograms(TString inputFilename, THnSparseF *& prof, THnSparseF *& profPt, THnSparseF *& profRho, bool useNPU);

// This method returns the maximum rho value for all eta by looping through the barrel region and checking the minimum requirements
// to add a point to the graphs. If at least 4 entries will be added to a graph, then this becomes the next highest rho.
pair<float,float> find_rho_bounds(THnSparseF *& prof, THnSparseF *& profPt, THnSparseF *& profEntries, int rebinEta, int rebinRho);

// This is a helper function to the divideHistogram function
// It tests if certain conditions are met before division can occur
bool checkConsistency(const THnSparseF *h1, const THnSparseF *h2, const char *tag);

// This is a reimplementation of the THnSparse divide function. The original function loops over all of the bins in the
//  sparse histogram. This is incredibly inefficient if very few bins are filled, but necessary if one doesn't know that
//  the filled bins have the same coordinates in each histogram. Since we can guarantee a one-to-one mapping between the
//  filled bins in one histogram and the filled bins in the other, we can simply loop over the filled bins.
//void divideHistograms(THnSparse *&numerator, THnSparse *&denominator, int depth, string resultantDescription = "histogram", string option = "");
void divideHistograms(THnSparseF *&numerator, THnSparseF *&denominator, string resultantDescription = "histogram");

// This method returns the graph from all the THnSparse's
TGraphErrors * getGraph(int iEta, int iRho, const THnSparseF * prof, const THnSparseF * profPt, const THnSparseF * profEntries);
TGraphErrors * getGraph(string nameTitle, const TH2D * prof, const TH2D * profPt, const TH2D * profEntries);
TGraphErrors * getGraph(string nameTitle, TH1D * prof, TH1D * profPt, const TH1D * profEntries);

// This method creates the txt file for the corrections
ofstream createTxtFile(string txtFilename, string function, bool forTesting = false);
void writeToTxtFile(ofstream& outF, const FitRes& fitResult,
                    bool forTesting = false, bool verbose = false);

// This method creates canvases showing the spline fits
TCanvas* createCanvas(const vector<TGraphErrors*>& graphs, const vector<FitRes>& fitResults,
                      string outputDir, string algo12, const vector<string>& formats);

// Convert a THnSparse to a TTree using efficient iteration through the THnSparse
// and draw a THnSparse using TParallelCoord.
// The plot will contain one line for each filled bin,
// with the bin's coordinates on each axis, and the bin's content on
// the rightmost axis.
TCanvas* drawSparse(THnSparseF* hs);
void drawsparse_draw(THnSparseF* h);
TTree* toTree(THnSparseF* h);

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char**argv){
   
    CommandLine cl;
    if (!cl.parse(argc,argv)) return 0;
    string         inputDir      = cl.getValue<string> ("inputDir",           "./");
    string         outputDir     = cl.getValue<string> ("outputDir",          "./");
    string         algo1         = cl.getValue<string> ("algo1",           "ak5pf");
    string         algo2         = cl.getValue<string> ("algo2",           "ak5pf");
    bool           useNPU        = cl.getValue<bool>   ("useNPU",            false);
    int            rebinEta      = cl.getValue<int>    ("rebinEta",              1);
    int            rebinRho      = cl.getValue<int>    ("rebinRho",              1);
    string         era           = cl.getValue<string> ("era",             "<era>");
    vector<string> formats       = cl.getVector<string>("formats",              "");
    bool           drawParaCoord = cl.getValue<bool>   ("drawParaCoord",     false);
                   projThenDiv   = cl.getValue<bool>   ("projThenDiv",        true);
    bool           forTesting    = cl.getValue<bool>   ("forTesting",        false);
    bool           debug         = cl.getValue<bool>   ("debug",             false);

    if (!cl.check()) return 0;
    cl.print();

    TBenchmark* m_benchmark = new TBenchmark();
    m_benchmark->Reset();
    m_benchmark->Start("event");

    if(inputDir.empty())      inputDir  = string(gSystem->pwd())+"/";
    if(outputDir.empty())     outputDir = string(gSystem->pwd())+"/";
    if(inputDir.back()!='/')  inputDir +="/";
    if(outputDir.back()!='/') outputDir+="/";

    string algo12 = algo1+"_"+algo2;
    if (algo1 == algo2) algo12 = algo1;
   
    vector<int> vdebug = {7,42,43,44,82};

    // Open the input file and retrieve all relevant TProfile3D's
    TString inputFilename = inputDir+"output_"+algo12+".root";
    THnSparseF *prof=0, *profPt=0, *profEntries=0;
    if(!getInputHistograms(inputFilename, prof, profPt, profEntries, useNPU))
        return -1;

    pair<float,float> rho_bounds = find_rho_bounds(prof,profPt,profEntries,rebinEta,rebinRho);

    // Divide O/A and pT histograms to get <O/A> and <pT> (i.e. sum(O/A)/nentries = <O/A>)
    if(!projThenDiv) {
        cout << "jet_l1_correction_x Dividing the sum(O/A) and sum(pTreco) histograms by the nentries histogram to get <O/A> and <pTreco> ... " << endl;
        divideHistograms(prof,   profEntries, "<O/A>");
        divideHistograms(profPt, profEntries, "<pTreco>");
        //cout << "\tDoing <O/A> ... " << flush;
        //prof->Divide(profEntries);
        //cout << "DONE" << endl << "\tDoing <pTreco> ... " << flush;
        //profPt->Divide(profEntries);
        //cout << "DONE" << endl;
    }

    // Create the output file to store the graphs
    TString rootFilename = outputDir+"Parameter_"+algo12+".root";
    cout << "jet_l1_correction_x Opening the output ROOT file " << rootFilename << " ... " << flush;
    TFile *fout = new TFile (rootFilename,"RECREATE");
    TString oDir = "graphs";
    fout->mkdir("graphs");
    fout->mkdir("canvases");
    fout->mkdir("projections");
    if(drawParaCoord)
        fout->mkdir("sparse");
    cout << "DONE" << endl;

    // The vector to save the results of all fits
    vector<FitRes> fitResultsGraph;
    vector<FitRes> fitResults;
    vector<TGraphErrors*> graphs;
    int nperpad(5);

    // Create the txt file and the function describing the chosen spline type (Akima)
    string txtFilename = outputDir+era+"_L1FastJet_"+algo12+".txt";
    string spline_function = "[0]+((x-[1])*([2]+((x-[1])*([3]+((x-[1])*[4])))))";
    ofstream outF = createTxtFile(txtFilename,spline_function,forTesting);

    // Loop over all etas
    for (int iEta = 1; iEta <= prof->GetAxis(0)->GetNbins(); iEta+=rebinEta){
        // Reset fitResults vector for each eta
        fitResults.clear();

        if(debug && std::find(vdebug.begin(), vdebug.end(), iEta) == vdebug.end()) continue;
        cout<< "Analyzing ieta="<<iEta<<" eta="<<prof->GetAxis(0)->GetBinCenter(iEta) << endl
            << "****************************" << endl;

        for (int iRho = 1; iRho <= prof->GetAxis(1)->GetNbins(); iRho+=rebinRho){
            cout<< "\tAnalyzing irho="<<iRho<<" rho="<<prof->GetAxis(1)->GetBinCenter(iRho) << endl
            << "\t****************************" << endl;

            if( (iRho%nperpad==1) || iRho==1) {
                graphs.clear();
                fitResultsGraph.clear();
            }

            // Create the graph
            prof->GetAxis(0)->SetRange(iEta,iEta+(rebinEta-1));
            prof->GetAxis(1)->SetRange(iRho,iRho+(rebinRho-1));
            profPt->GetAxis(0)->SetRange(iEta,iEta+(rebinEta-1));
            profPt->GetAxis(1)->SetRange(iRho,iRho+(rebinRho-1));
            profEntries->GetAxis(0)->SetRange(iEta,iEta+(rebinEta-1));
            profEntries->GetAxis(1)->SetRange(iRho,iRho+(rebinRho-1));
            TH1* prof_proj = nullptr;
            TH1* profPt_proj = nullptr;
            TH1* profEntries_proj = nullptr;
            if(!projThenDiv) {
                profPt_proj = profPt->Projection(3,2,"E"); //(y,x,option)=(refpt,tnpu,calc errors);
                prof_proj = prof->Projection(3,2,"E"); //(y,x,option)=(refpt,tnpu,calc errors);
                profEntries_proj = profEntries->Projection(3,2,"E"); //(y,x,option)=(refpt,tnpu,calc errors);
            }
            else {
                prof_proj = prof->Projection(3,"E"); //(x,option)=(refpt,calc errors);
                profPt_proj = profPt->Projection(3,"E"); //(x,option)=(refpt,calc errors);
                profEntries_proj = profEntries->Projection(3,"E"); //(x,option)=(refpt,calc errors);
            }

            // Rebin if necessary
            // Currently reducing the number of TNPU bins from 200 to 50
            //prof_proj->Rebin2D(4,1);
            //profPt_proj->Rebin2D(4,1);
            //profEntries_proj->Rebin2D(4,1);

            pair<float,float> etaBoundaries = make_pair(prof->GetAxis(0)->GetBinLowEdge(iEta),prof->GetAxis(0)->GetBinUpEdge(iEta+(rebinEta-1)));
            pair<float,float> rhoBoundaries = make_pair(prof->GetAxis(1)->GetBinLowEdge(iRho),prof->GetAxis(1)->GetBinUpEdge(iRho+(rebinRho-1)));
            stringstream ss;
            ss << "OffOAVsJetPt_JetEta" <<  etaBoundaries.first << "to" <<  etaBoundaries.second << "_Rho" << rhoBoundaries.first << "to" << rhoBoundaries.second;
            TGraphErrors* graph = (projThenDiv) ? getGraph(ss.str(), dynamic_cast<TH1D*>(prof_proj), dynamic_cast<TH1D*>(profPt_proj), dynamic_cast<TH1D*>(profEntries_proj)):
                                                  getGraph(ss.str(), dynamic_cast<TH2D*>(prof_proj), dynamic_cast<TH2D*>(profPt_proj), dynamic_cast<TH2D*>(profEntries_proj));

            // Save the projections
            ss.str("");
            ss << "OffOAVsTnpuVsRefpt_JetEta" <<  etaBoundaries.first << "to" <<  etaBoundaries.second << "_Rho" << rhoBoundaries.first << "to" << rhoBoundaries.second;
            prof_proj->SetName(ss.str().c_str());
            ss.str("");
            ss << "PtVsTnpuVsRefpt_JetEta" <<  etaBoundaries.first << "to" <<  etaBoundaries.second << "_Rho" << rhoBoundaries.first << "to" << rhoBoundaries.second;
            profPt_proj->SetName(ss.str().c_str());
            fout->cd("projections");
            prof_proj->Write();
            profPt_proj->Write();
            fout->cd();
            delete prof_proj;
            delete profPt_proj;
            delete profEntries_proj;
            //TGraphErrors* graph = getGraph(iEta, iRho, prof, profPt, profEntries);
            cout << "\t\tGraph for pT, Eta, Rho created successfully" << endl;

            // Needs to be at least 4 entries
            if(graph->GetN()<5){
                cout<<"\t\t WARNING: Graph has only " << graph->GetN() << " entries. Skipping (eta,rho)=(" 
                    << iEta << "," << iRho << ")" << endl;
            }
            else {

                cout << "\t\tSorting the (x,y) values ... " << flush;
                double* px = graph->GetX();
                double* py = graph->GetY();
                unsigned int N = graph->GetN();
                double *px_sorted = (double*) malloc(N * sizeof(double));
                double *py_sorted = (double*) malloc(N * sizeof(double));
                int* index_x = (int*)malloc(N * sizeof(int));
                TMath::Sort((int)N,px,index_x,false);
                for(unsigned int ip=0; ip<N; ip++) {
                    px_sorted[ip] = px[index_x[ip]];
                    py_sorted[ip] = py[index_x[ip]];
                    //cout << "Point/index_x " << ip << "/" << index_x[ip] << " = (" << px[index_x[ip]] << "," << py[index_x[ip]] << ")" << endl;
                }
                cout << "DONE" << endl;
      
                // Do the fitting
                cout << "\t\tInitialize Spline ... " << flush;
                gsl_spline *spline_akima = gsl_spline_alloc(gsl_interp_akima, graph->GetN());
                gsl_spline_init(spline_akima, px_sorted, py_sorted, N);
                PiecewiseSpline* pspline = new PiecewiseSpline(string("spline_")+graph->GetName(),graph,{},ROOT_spline_type::TS3,false);
                pspline->setSpline(PiecewiseSpline::gslToROOT_spline(spline_akima,"TSpline3_akima"));
                TF1* pfunc = new TF1("fit",spline_function.c_str(),graph->GetX()[0],graph->GetX()[graph->GetN()-1]);
                pspline->setPartialFunction(pfunc);
                gsl_spline_free(spline_akima);
                cout << "DONE" << endl;

                // Put this fit result in the vector fitResults
                FitRes fitres;
                fitres.etalowedge = prof->GetAxis(0)->GetBinLowEdge(iEta);
                fitres.etaupedge  = prof->GetAxis(0)->GetBinUpEdge(iEta+(rebinEta-1));
                fitres.rholowedge = prof->GetAxis(1)->GetBinLowEdge(iRho);
                fitres.rhoupedge  = prof->GetAxis(1)->GetBinUpEdge(iRho+(rebinRho-1));
                fitres.pspline    = pspline;
                fitres.igraph     = graphs.size();
                fitResultsGraph.push_back(fitres);
                fitResults.push_back(fitres);
            }

            // Save the graph to file
            fout->cd("graphs");
            graph->Write();
            fout->cd();

            graphs.push_back(graph);
            if( (iRho%nperpad==0) || iRho>=prof->GetAxis(1)->GetNbins() ) {
                fout->cd("canvases");
                createCanvas(graphs,fitResultsGraph,outputDir,string(algo12),formats)->Write();
                fout->cd();
            }
        }// all rho bins

        // write the to the text file all of the results for each eta bin
        // Need to do it here so that we can fin the last rho bin and reset its upper bound
        unsigned int nFR = fitResults.size();
        for(unsigned int iFR=0; iFR<nFR; iFR++) {
            //Add in missing upper rho bins so that all eta are symmetric with respect to rho
            if(iFR == nFR-1 && fitResults[iFR].rhoupedge<rho_bounds.second) {
                FitRes tmp = fitResults[iFR];
                tmp.rholowedge = tmp.rhoupedge;
                tmp.rhoupedge += rebinRho;
                fitResults.emplace_back(tmp);
                nFR = fitResults.size();
            }
            //Add in the missing lower rho bins so that all eta are symmetric with respect to rho
            else if(iFR == 0 && fitResults[iFR].rholowedge>rho_bounds.first) {
                //Find the end of the first rho bin
                unsigned rho_end=0;
                for(unsigned int jFR=0; jFR<nFR; jFR++) {
                    if(fitResults[jFR].rholowedge!=fitResults[jFR+1].rholowedge) {
                        rho_end=jFR;
                        break;
                    }
                }
                vector<FitRes> tmp(fitResults.begin(),fitResults.begin()+rho_end+1);
                for(unsigned int tFR=0; tFR<tmp.size(); tFR++) {
                    tmp[tFR].rhoupedge = tmp[tFR].rholowedge;
                    tmp[tFR].rholowedge -= rebinRho;
                }
                fitResults.insert(fitResults.begin(),tmp.begin(),tmp.end());
                nFR = fitResults.size();
                iFR--;
                continue;
            }
            //Add in missing rho bins which are in the middle of a set of rho bins (per eta)
            //Copy the paramters from the next lowest rho bin
            else if(iFR!=nFR-1 && fitResults[iFR].rhoupedge!=fitResults[iFR+1].rholowedge) {
                FitRes tmp = fitResults[iFR];
                tmp.rholowedge = tmp.rhoupedge;
                tmp.rhoupedge += rebinRho;
                fitResults.insert(fitResults.begin()+iFR+1,tmp);
                nFR = fitResults.size();
            }
            else if(iFR == nFR-1)
                fitResults[iFR].rhoupedge = 200;
            writeToTxtFile(outF, fitResults[iFR], forTesting, ((iFR==0)?true:false));
        }// rho bins with filled graphs

    }// eta bins

    if(drawParaCoord) {
        fout->cd("sparse");
        drawSparse(prof)->Write();
        drawSparse(profPt)->Write();
    }

    // close the file and clean up
    outF.close();
    fout->Close();
    delete fout;

    m_benchmark->Stop("event");
    cout << "jet_l1_correction_x" << endl
         << "\tCPU time = " << m_benchmark->GetCpuTime("event") << " s" << endl
         << "\tReal time = " << m_benchmark->GetRealTime("event") << " s" << endl;
    delete m_benchmark;

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
// This method tries to obtain all the relevant TProfiles from the inputFilename
// It returns true if successfull
bool getInputHistograms(TString inputFilename, THnSparseF *& prof, THnSparseF *& profPt, 
                        THnSparseF *& profRho, bool useNPU){
   
    cout << "jet_l1_correction_x::getInputProfiles Opening the file " << inputFilename
         << " and retriving the input THnSparse ... " << flush;

    // Open the input file
    TFile *fin= new TFile(inputFilename);
    if(!fin->IsOpen()) {
        cout << endl << "\tERROR jet_synchfit_xx::getInputProfiles() could not open file " 
             << inputFilename<< endl;
        return false;
    }
   
    // Get the histos
    prof    = (THnSparseF*) fin->Get(Form("p_offOverA_etaRhoVs%spusVsJetPt",(useNPU ? "N" : "Tn"))); //offOverA(eta, rho, refpt) 
    profPt  = (THnSparseF*) fin->Get(Form("p_PtAve_etaRhoVs%spusVsJetPt",(useNPU ? "N" : "Tn"))); // pt(eta, rho, refpt) 
    profRho = (THnSparseF*) fin->Get(Form("p_entries_etaRhoVs%spusVsJetPt",(useNPU ? "N" : "Tn")));// rho(eta, rho, refpt) 
   
    if (!prof || !profPt  || !profRho) {
        cout << endl << "\tERROR jet_synchfit_xx::getInputProfiles() could not retrieve THnSparse named "
             << "either of  p_offOverA_etaRhoVsNpusVsJetPt, p_PtAve_etaRhoVsNpusVsJetPt, "
             << " or p_RhoAve_etaRhoVsNpusVsJetPt"<<endl;
        return false;
    }
   
    cout << "DONE" << endl;

    // if everything went well just return true.
    return true;  
}

//______________________________________________________________________________
// This method returns the maximum rho value for all eta by looping through the barrel region and checking the minimum requirements
// to add a point to the graphs. If at least 4 entries will be added to a graph, then this becomes the next highest rho.
pair<float,float> find_rho_bounds(THnSparseF *& prof, THnSparseF *& profPt, THnSparseF *& profEntries, int rebinEta, int rebinRho) {
    cout << "jet_l1_correction_x::find_rho_bounds()  finding the maximum rho bin ... " << endl;
    float min_rho = 9999, max_rho = 0, max_eta = -9999, min_eta = -9999;
    int skipRho = 25, counter = 0, max_counter = 0, min_counter = 0, current_index = 0, total_to_check = (52-31)*(prof->GetAxis(1)->GetNbins()-skipRho);
    stringstream ss;
    for (int iEta = 31; iEta <= 52; iEta+=rebinEta){
        for (int iRho = 1; iRho <= prof->GetAxis(1)->GetNbins(); iRho+=rebinRho){
            if(iRho==5) iRho+=skipRho;
            ss.str("");
            ss << "\tProgress: (iEta,iRho,counter,min_rho,max_rho)=(" << iEta << "," << iRho << "," << counter << "," << min_rho << "," << max_rho << ") ";
            current_index = (iEta-31)*(prof->GetAxis(1)->GetNbins()-skipRho)+((iRho<10) ? iRho : iRho-skipRho)+1;
            loadbar2(current_index,total_to_check,50,ss.str());
            counter = 0;
            prof->GetAxis(0)->SetRange(iEta,iEta+(rebinEta-1));
            prof->GetAxis(1)->SetRange(iRho,iRho+(rebinRho-1));
            profPt->GetAxis(0)->SetRange(iEta,iEta+(rebinEta-1));
            profPt->GetAxis(1)->SetRange(iRho,iRho+(rebinRho-1));
            profEntries->GetAxis(0)->SetRange(iEta,iEta+(rebinEta-1));
            profEntries->GetAxis(1)->SetRange(iRho,iRho+(rebinRho-1));
            TH1D* prof_proj = prof->Projection(3,"E"); //(x,option)=(refpt,calc errors);
            TH1D* profPt_proj = profPt->Projection(3,"E"); //(x,option)=(refpt,calc errors);
            TH1D* profEntries_proj = profEntries->Projection(3,"E"); //(x,option)=(refpt,calc errors);
            prof_proj->Divide(profEntries_proj);
            profPt_proj->Divide(profEntries_proj);
            for (int irefpt = 1; irefpt <= prof_proj->GetXaxis()->GetNbins() ; irefpt++){
                if (profEntries_proj->GetBinContent (irefpt) &&
                    prof_proj->GetBinError      (irefpt)  > ABS_ERROR_MIN &&
                    (fabs(prof_proj->GetBinError(irefpt)/prof_proj->GetBinContent(irefpt)))<REL_ERROR_MAX &&
                    (fabs(prof_proj->GetBinError(irefpt)/prof_proj->GetBinContent(irefpt)))>REL_ERROR_MIN &&
                    profPt_proj ->GetBinContent (irefpt)  > PT_CONT_MIN &&
                    profPt_proj ->GetBinError   (irefpt)  > PT_ERROR_MIN ) {
                    counter++;
                }
            }
            if(counter>=5 && max_rho<prof->GetAxis(1)->GetBinUpEdge(iRho+(rebinRho-1))) {
                max_rho = prof->GetAxis(1)->GetBinUpEdge(iRho+(rebinRho-1));
                max_counter = counter;
                max_eta = prof->GetAxis(0)->GetBinCenter(iEta+(rebinEta-1));
            }
            if(counter>=5 && min_rho>prof->GetAxis(1)->GetBinLowEdge(iRho+(rebinRho-1))) {
                min_rho = prof->GetAxis(1)->GetBinLowEdge(iRho+(rebinRho-1));
                min_counter = counter;
                min_eta = prof->GetAxis(0)->GetBinCenter(iEta+(rebinEta-1));
            }

            delete prof_proj;
            delete profPt_proj;
            delete profEntries_proj;
        }
    }
    cout << endl << "Minimum rho: " << min_rho << endl
         << "\tOccured at eta = " << min_eta << " with " << min_counter << " entries" << endl;
    cout << "Maximum rho: " << max_rho << endl
         << "\tOccured at eta = " << max_eta << " with " << max_counter << " entries" << endl;
    if(min_rho==1) {
        cout << "Changing minimum rho to be 0 because most likely this value occurs outside of the checked eta range." << endl;
        min_rho = 0;
    }
    return make_pair(min_rho,max_rho);
}

//______________________________________________________________________________
// This is a helper function to the divideHistogram function
// It tests if certain conditions are met before division can occur
bool checkConsistency(const THnSparseF *h1, const THnSparseF *h2, const char *tag) {
   if (h1->GetNdimensions()!=h2->GetNdimensions()) {
      Warning(tag,"Different number of dimensions, cannot carry out operation on the histograms");
      return false;
   }
   for (Int_t dim = 0; dim < h1->GetNdimensions(); dim++) {
      if (h1->GetAxis(dim)->GetNbins()!=h2->GetAxis(dim)->GetNbins()) {
         Warning(tag,"Different number of bins on axis %i, cannot carry out operation on the histograms", dim);
         return false;
      }
   }
   return true;
}

//______________________________________________________________________________
// This is a reimplementation of the THnSparse divide function. The original function loops over all of the bins in the
//  sparse histogram. This is incredibly inefficient if very few bins are filled, but necessary if one doesn't know that
//  the filled bins have the same coordinates in each histogram. Since we can guarantee a one-to-one mapping between the
//  filled bins in one histogram and the filled bins in the other, we can simply loop over the filled bins.
//void divideHistograms(THnSparse *&numerator, THnSparse *&denominator, int depth, string resultantDescription, string option) {
void divideHistograms(THnSparseF *&numerator, THnSparseF *&denominator, string resultantDescription) {
/*
    //Attempt at recursive version

    std::transform(option.begin(), option.end(), option.begin(), ::tolower);

    if(option.find("debug")!=string::npos) {
        for(int i=numerator->GetNdimensions()-depth; i>-1;i--)
            cout << "\t";
        cout << "divideHistograms::At depth = " << depth << endl;
    }

    if (depth==0) {
        Int_t bin = GetGlobalBin(coord);
        integral += GetBinContent(bin)*fBinEntries.fArray[bin];
    }        
    else {
        for (int icoord = ranges[depth-1].first; fRange<=ranges[depth-1].second; fRange++) {
            if(option.Contains("debug")){
                for(int i=numerator->GetNdimensions()-depth; i>-1;i--)
                    cout << "\t";
                cout << "divideHistograms::At index = " << fRange << endl;
            }
            coord[depth-1] = fRange;
            divideHistograms(numerator,denominator,depth-1,resultantDescription,option);
        }
    }
*/

    cout << "\tDoing " << resultantDescription << " ... " << endl;

    // Divide numerator histogram by denominator
    // numerator = numerator/(denominator)
    // Note that if denominator has Sumw2 set, Sumw2 is automatically called for
    // numerator if not already set.
    // The resulting errors are calculated assuming uncorrelated content.

    // Check consistency of the input
    if (!checkConsistency(numerator, denominator, "Divide")) {
        Error("Divide(numerator,denominator)", "The two histograms did not pass the consistency checks.");
        std::terminate();
    }

    // Trigger error calculation if denominator has it
    cout << "\t\tTrigger error calculation if denominator has it ... " << flush;
    bool wantErrors=numerator->GetCalculateErrors();
    if (!numerator->GetCalculateErrors() && denominator->GetCalculateErrors())
        wantErrors=true;
    cout << "DONE" << endl;

    // Remember original histogram statistics
    cout << "\t\tRemember original histogram statistics ... " << flush;
    Double_t nEntries = numerator->GetEntries();
    cout << "DONE" << endl;

    cout << "\t\tSet numerator Sumw2 ... " << flush;
    if (wantErrors) numerator->Sumw2();
    bool didWarn = false;
    cout << "DONE" << endl;

    // Now divide the contents: also in this case we have the intersection of the sets of bins
    int* coord = new int[numerator->GetNdimensions()];
    memset(coord, 0, sizeof(int) * numerator->GetNdimensions());
    double err = 0.;
    double b22 = 0.;
    int nbins = numerator->GetNbins();
    for (Long64_t i = 0; i < nbins; ++i) {
        loadbar2(i+1,nbins,50,"\t\tProgress: ");
        //if(i%10==0) cout << endl << "Why is this so slow?! (" << i << "/" << nbins << ")" << endl;

        // Get the content of the bin from the first histogram
        double v1 = numerator->GetBinContent(i, coord);
        // Now look at the bin with the same coordinates in h
        double v2 = denominator->GetBinContent(i); //coord);

        if (!v2) {
            v1 = 0.;
            v2 = 1.;
            if (!didWarn) {
                Warning("Divide(numerator,denominator)", "Histogram denominator has empty bins - division by zero! Setting bin to 0.");
                didWarn = true;
            }
        }
        //numerator->SetBinContent(coord, v1 / v2);
        numerator->SetBinContent(i, v1 / v2);
        if (wantErrors) {
            //double err1 = numerator->GetBinError(coord) * v2;
            double err1 = numerator->GetBinError(i) * v2;
            //double err2 = denominator->GetBinError(coord) * v1;
            double err2 = denominator->GetBinError(i) * v1;
            b22 = v2 * v2;
            err = (err1 * err1 + err2 * err2) / (b22 * b22);
            //numerator->SetBinError(coord, TMath::Sqrt(err));
            numerator->SetBinError(i, TMath::Sqrt(err));
        }
    }
    delete [] coord;
    numerator->SetEntries(nEntries);
    cout << endl;
}

//______________________________________________________________________________
// This method returns the graph from all the THnSparse's
TGraphErrors * getGraph(int iEta, int iRho, const THnSparseF * prof,
                        const THnSparseF * profPt, const THnSparseF * profEntries){
    pair<float,float> etaBoundaries = make_pair(prof->GetAxis(0)->GetBinLowEdge(iEta),prof->GetAxis(0)->GetBinUpEdge(iEta));
    pair<float,float> rhoBoundaries = make_pair(prof->GetAxis(1)->GetBinLowEdge(iRho),prof->GetAxis(1)->GetBinUpEdge(iRho));;
    string nameTitle = Form("OffOAVsJetPt_JetEta%fto%f_Rho%fto%f",
                             etaBoundaries.first,etaBoundaries.second,rhoBoundaries.first,rhoBoundaries.second);

    // The returning graph, with its name and title
    TGraphErrors *graph  = new TGraphErrors();
    graph->SetName(nameTitle.c_str());
    graph->SetTitle((nameTitle+";p_{T}^{pu};OffsetOverArea").c_str());
    int nEvt = 0;
    cout << "\tNBins (mu,pT):  (" << prof->GetAxis(2)->GetNbins() << "," << prof->GetAxis(3)->GetNbins() << ")" << endl;
   
    Int_t *bins = new Int_t[4];

    for (int imu = 1 ; imu <= prof->GetAxis(2)->GetNbins() ; imu ++){      
        for (int irefpt = 1; irefpt <= prof->GetAxis(3)->GetNbins() ; irefpt++){
            bins[0] = iEta; bins[1] = iRho; bins[2] = imu; bins[3] = irefpt;
            cout << "sfsg1" << endl;
            int Gbin = prof->GetBin(bins);
            cout << "sfsg2" << endl;
            nEvt += profEntries->GetBinContent(Gbin);
            // avoid points with empty content or too small error
            if (profEntries->GetBinContent (Gbin) &&
                prof->GetBinError      (Gbin)  > ABS_ERROR_MIN &&
                (fabs(prof->GetBinError(Gbin)/prof->GetBinContent(Gbin)))<REL_ERROR_MAX &&// 0.3 ==> 0.5
                (fabs(prof->GetBinError(Gbin)/prof->GetBinContent(Gbin)))>REL_ERROR_MIN &&// 0.05 ==> 0.01
                profPt ->GetBinContent (Gbin)  > PT_CONT_MIN &&
                profPt ->GetBinError   (Gbin)  > PT_ERROR_MIN ){
            
                // get the relevant values
                double pt   = profPt->GetBinContent(Gbin);
                double pte  = profPt->GetBinError  (Gbin);
                double ooa  = prof  ->GetBinContent(Gbin);
                double ooae = prof  ->GetBinError  (Gbin);
                
                // Store the values
                double n = graph->GetN();
                graph->SetPoint(n, pt, ooa);
                graph->SetPointError(n, pte, ooae);
            }//if
        } // irefpt  
    }// irho
   
   cout << "\tgraph has (Entries,Evts):  (" << graph->GetN() << "," << nEvt << ")" << endl;
   //cout<<"\tgraph has "<<graph->GetN()<<" entries"<<"& # of Evts: " << nEvt<< endl;
   
   // return it
   return graph;
   
}

//______________________________________________________________________________
TGraphErrors * getGraph(string nameTitle, const TH2D * prof, const TH2D * profPt, const TH2D * profEntries) {
    // The returning graph, with its name and title
    TGraphErrors *graph  = new TGraphErrors();
    graph->SetName(nameTitle.c_str());
    graph->SetTitle((nameTitle+";p_{T}^{pu};OffsetOverArea").c_str());
    int nEvt = 0;
    cout << "\tNBins (mu,pT):  (" << prof->GetXaxis()->GetNbins() << "," << prof->GetYaxis()->GetNbins() << ")" << endl;

    for (int imu = 1 ; imu <= prof->GetXaxis()->GetNbins() ; imu ++){      
        for (int irefpt = 1; irefpt <= prof->GetYaxis()->GetNbins() ; irefpt++){
            nEvt += profEntries->GetBinContent(imu,irefpt);
            // avoid points with empty content or too small error
            if (profEntries->GetBinContent (imu,irefpt) &&
                prof->GetBinError      (imu,irefpt)  > ABS_ERROR_MIN &&
                (fabs(prof->GetBinError(imu,irefpt)/prof->GetBinContent(imu,irefpt)))<REL_ERROR_MAX &&// 0.3 ==> 0.5
                (fabs(prof->GetBinError(imu,irefpt)/prof->GetBinContent(imu,irefpt)))>REL_ERROR_MIN &&// 0.05 ==> 0.01
                profPt ->GetBinContent (imu,irefpt)  > PT_CONT_MIN &&
                profPt ->GetBinError   (imu,irefpt)  > PT_ERROR_MIN ){
            
                // get the relevant values
                double pt   = profPt->GetBinContent(imu,irefpt);
                double pte  = profPt->GetBinError  (imu,irefpt);
                double ooa  = prof  ->GetBinContent(imu,irefpt);
                double ooae = prof  ->GetBinError  (imu,irefpt);
                
                // Store the values
                double n = graph->GetN();
                graph->SetPoint(n, pt, ooa);
                graph->SetPointError(n, pte, ooae);
            }//if
        } // irefpt  
    }// irho
   
    cout << "\tgraph has (Entries,Evts):  (" << graph->GetN() << "," << nEvt << ")" << endl;
   
    // return it
    return graph;
}

//______________________________________________________________________________
TGraphErrors * getGraph(string nameTitle, TH1D * prof, TH1D * profPt, const TH1D * profEntries) {
    // The returning graph, with its name and title
    TGraphErrors *graph  = new TGraphErrors();
    graph->SetName(nameTitle.c_str());
    graph->SetTitle((nameTitle+";p_{T}^{pu};OffsetOverArea").c_str());
    int nEvt = 0;
    cout << "\tNBins (mu,pT):  (" << prof->GetXaxis()->GetNbins() << "," << prof->GetYaxis()->GetNbins() << ")" << endl;

    if(projThenDiv) {
        prof->Divide(profEntries);
        profPt->Divide(profEntries);
    }

    for (int irefpt = 1; irefpt <= prof->GetXaxis()->GetNbins() ; irefpt++){
        nEvt += profEntries->GetBinContent(irefpt);
        // avoid points with empty content or too small error
        if (profEntries->GetBinContent (irefpt) &&
            prof->GetBinError      (irefpt)  > ABS_ERROR_MIN &&
            (fabs(prof->GetBinError(irefpt)/prof->GetBinContent(irefpt)))<REL_ERROR_MAX &&// 0.3 ==> 0.5
            (fabs(prof->GetBinError(irefpt)/prof->GetBinContent(irefpt)))>REL_ERROR_MIN &&// 0.05 ==> 0.01
            profPt ->GetBinContent (irefpt)  > PT_CONT_MIN &&
            profPt ->GetBinError   (irefpt)  > PT_ERROR_MIN ){
        
            // get the relevant values
            int    n    = graph ->GetN();
            double pt   = profPt->GetBinContent(irefpt);
            double pte  = profPt->GetBinError  (irefpt);
            double ooa  = prof  ->GetBinContent(irefpt);
            double ooae = prof  ->GetBinError  (irefpt);
            
            // make sure that is the average x value of the last point is the same as this point that we do not allow them to overlap
            // we want a function that is continuous and continuously differentiable
            // in this case shift this next point by a very tiny value
            if (n>0 && graph->GetX()[n-1]==pt) pt+=0.0001;

            // Store the values
            graph->SetPoint(n, pt, ooa);
            graph->SetPointError(n, pte, ooae);
        }//if
    } // irefpt  
   
    cout << "\tgraph has (Entries,Evts):  (" << graph->GetN() << "," << nEvt << ")" << endl;
   
    // return it
    return graph;
}

//______________________________________________________________________________
// This method creates the txt file for the corrections
ofstream createTxtFile(string txtFilename, string function, bool forTesting) {
    cout << "jet_l1_correction_x::createTxtFile Writting the text file " << txtFilename << " ... " << flush;
    // Create the stream 
    ofstream outF(txtFilename);
   
    // Produce the first line
    string fname;
    if(forTesting)
       fname = "{3 JetEta Rho JetPt 1 JetPt 1";
    else
       fname = Form("{3 JetEta Rho JetPt 2 JetPt JetA max(0.0001,1-y*(%s)/x)",function.c_str());
    outF << fname <<" Correction L1FastJet}"<<endl;

    cout << "DONE" << endl;
    return outF;
}

//______________________________________________________________________________
// Write one eta and rho line in the text file
void writeToTxtFile(ofstream& outF, const FitRes& fitResult, bool forTesting, bool verbose) {
    if(verbose) cout << "\tjet_l1_correction_x::writeToTxtFile Writting to the text file ... " << flush;

    //For eta-dependent spline clipping
    int pt_limit = 70;
   
    //For eta-dependent spline clipping
    if      ( (fitResult.etaupedge>0 && fitResult.etaupedge < 0.6091) || (fitResult.etalowedge<0 && fitResult.etalowedge > -0.6091) ) pt_limit = 3000;
    else if ( (fitResult.etaupedge>0 && fitResult.etaupedge < 0.9571) || (fitResult.etalowedge<0 && fitResult.etalowedge > -0.9571) ) pt_limit = 2700;
    else if ( (fitResult.etaupedge>0 && fitResult.etaupedge < 1.3051) || (fitResult.etalowedge<0 && fitResult.etalowedge > -1.3051) ) pt_limit = 2000;
    else if ( (fitResult.etaupedge>0 && fitResult.etaupedge < 2.0431) || (fitResult.etalowedge<0 && fitResult.etalowedge > -2.0431) ) pt_limit = 1400;
    else if ( (fitResult.etaupedge>0 && fitResult.etaupedge < 2.51  ) || (fitResult.etalowedge<0 && fitResult.etalowedge > -2.51  ) ) pt_limit = 900;
    else if ( (fitResult.etaupedge>0 && fitResult.etaupedge < 2.9641) || (fitResult.etalowedge<0 && fitResult.etalowedge > -2.9641) ) pt_limit = 500;
    else if ( (fitResult.etaupedge>0 && fitResult.etaupedge < 3.6641) || (fitResult.etalowedge<0 && fitResult.etalowedge > -3.6641) ) pt_limit = 300;
    else if ( (fitResult.etaupedge>0 && fitResult.etaupedge < 4.0131) || (fitResult.etalowedge<0 && fitResult.etalowedge > -4.0131) ) pt_limit = 200;
    else if ( (fitResult.etaupedge>0 && fitResult.etaupedge < 4.5381) || (fitResult.etalowedge<0 && fitResult.etalowedge > -4.5381) ) pt_limit = 100;

    bool abovePtLimit = false;
    bool lastLine = false;

    for(int isection=0; isection<fitResult.pspline->getNSections(); isection++) {
        if(lastLine) continue;

        pair<double,double> bounds = fitResult.pspline->getSectionBounds(isection);
        //When you go beyond a range of validity the default behavior is to return to the correction value at the closest bound to the range of validity
        //When you go outside a bin boundary (i.e. the program cannot find the bin you are supposed to be in) then the default behavior is to return 1.0
        //This will protext against that happening when the pT is just above where the last MC bin is.
        //6500 is chosen as that is the 2015-2017 beam energy.

        if(isection==fitResult.pspline->getNSections()-1) lastLine = true;
        if(bounds.second >= pt_limit) {
            abovePtLimit = true;
            lastLine = true;
        }

        //Still using eta dependent clipping
        int npar = forTesting ? 2 : (int)(fitResult.pspline->getNpar()+4);
        outF<<setw(15)<<fitResult.etalowedge<<setw(15)<<fitResult.etaupedge
            <<setw(15)<<fitResult.rholowedge<<setw(15)<<fitResult.rhoupedge
            <<setw(15)<<bounds.first<<setw(15)<<(lastLine ? 6500 : bounds.second)
            <<setw(15)<<npar //Number of parameters + 4 for the JetPt and JetA boundaries
            <<setw(15)<<bounds.first<<setw(15)<<(abovePtLimit ? pt_limit : bounds.second);
        if(!forTesting) {
           outF<<setw(15)<<0<<setw(15)<<10; //Area boundaries

           TF1* spline_func = fitResult.pspline->setParameters(isection);
           for(int p=0; p<fitResult.pspline->getNpar(); p++) {
              outF<<setw(15)<<spline_func->GetParameter(p);
           }
        }
        outF<<endl;
    }

   if(verbose) cout << "DONE" << endl;  
}

//______________________________________________________________________________
// This method creates canvases showing the spline fits
TCanvas* createCanvas(const vector<TGraphErrors*>& graphs, const vector<FitRes>& fitResults,
                      string outputDir, string algo12, const vector<string>& formats) {
    TH1D* frame = new TH1D();
    frame->GetXaxis()->SetLimits(3.0001,5000.0);
    frame->GetXaxis()->SetMoreLogLabels();
    frame->GetXaxis()->SetNoExponent();
    frame->GetXaxis()->SetTitle("p_{T}^{reco} (GeV)");
    frame->GetYaxis()->SetRangeUser(0.0,40.0);
    frame->GetYaxis()->SetTitle("Offset/A");

    string gname = string(graphs[0]->GetName());
    stringstream sscname;
    int pos = gname.rfind("to");
    sscname << gname.substr(0,pos+2);
    gname = string(graphs[graphs.size()-1]->GetName());
    sscname << gname.substr(gname.rfind("to")+2);

    TCanvas* c = tdrCanvas(sscname.str().c_str(),frame,14,0,true);
    c->SetLogx();

    double legx(0.45), legy(0.9), legw(0.4);
    double legxmin = legx;
    double legymin = legy;
    double legxmax = legx+legw;
    double legymax = legymin - (graphs.size()+1)*0.055;

    TLegend* leg = tdrLeg(legxmin,legymin,legxmax,legymax);
    leg->SetHeader(gname.substr(0,gname.find("Rho")+3).c_str());

    bool warned = false;
    for(unsigned int igraph=0; igraph<graphs.size(); igraph++) {

        gname = string(graphs[igraph]->GetName());

        //
        // Draw the graph
        // Create a legend entry for that graph
        //
        int colorNumber = (igraph!=9) ? igraph+1 : 11;
        tdrDraw(graphs[igraph],"P",kFullCircle,colorNumber,kSolid,colorNumber);
        pos = gname.find("Rho");
        int rpos = gname.rfind("to");
        leg->AddEntry(graphs[igraph],Form("%s<#rho<%s",gname.substr(pos+3,rpos-pos-3).c_str(),gname.substr(rpos+2).c_str()),"p");

        //
        // Turn off the fit stats because apparently they are not being turned off by the gStyle
        //
        c->Update();
        TPaveStats* stat = (TPaveStats*)graphs[igraph]->GetListOfFunctions()->FindObject("stats");
        if(stat) {
            stat->SetOptFit(0);
            stat->SetOptStat(0);
            stat->SetX1NDC(1.1);
            stat->SetX2NDC(1.1);
            stat->SetY1NDC(1.1);
            stat->SetY2NDC(1.1);
            c->Modified();
        }
        else {
            if(!warned) {
                cout << "WARNING::createCanvas Did not find the \"stats\" object" << endl;
                warned = true;
            }
        }

        //
        // Draw the splines for each graph, if they exist
        //
        if(fitResults.size()>0 && graphs[igraph]->GetN()>=5 && fitResults[igraph].igraph == igraph) {
            PiecewiseSpline* spline = fitResults[igraph].pspline;
            for(int isection=0; isection<spline->getNSections(); isection++) {
                pair<double,double> bounds = spline->getSectionBounds(isection);
                TF1* spline_func = (TF1*)spline->setParameters(isection)->Clone("tmp_func");
                spline_func->SetRange(bounds.first,bounds.second);
                spline_func->SetLineColor(colorNumber);
                spline_func->SetLineStyle(kDotted);
                spline_func->Draw("same");
            }
        }
    }
    //
    // Draw the legend and save the canvas
    //
    leg->Draw("same");

    for(unsigned int iformat=0; iformat<formats.size(); iformat++) {
        c->SaveAs(Form("%s%s%s",outputDir.c_str(),c->GetName(),formats[iformat].c_str()));
    }

    return c;
}

//______________________________________________________________________________
// Convert a THnSparse to a TTree using efficient iteration through the THnSparse
// and draw a THnSparse using TParallelCoord.
// The plot will contain one line for each filled bin,
// with the bin's coordinates on each axis, and the bin's content on
// the rightmost axis.
TCanvas* drawSparse(THnSparseF* hs) {
    TCanvas* canv = new TCanvas("hDrawSparse", "Drawing a sparse hist");
    canv->Divide(2);

    if(!hs) {
        cout << "ERROR::jet_l1_correction_x::drawSparse Could not retrieve THnSparse" << endl;
        return canv;
    }

    // draw it
    canv->cd(1);
    drawsparse_draw(hs);

    // project it
    canv->cd(2);
    TH3D* h3proj = hs->Projection(1, 2, 3);
    h3proj->SetLineColor(kOrange);
    h3proj->SetDirectory(0);
    h3proj->Draw("lego1");

    return canv;
}

//______________________________________________________________________________
void drawsparse_draw(THnSparseF* h) {
    // Draw a THnSparse using TParallelCoord, creating a temporary TTree.
    cout << "drawsparse_draw::begin" << endl;
    cout << "drawsparse_draw::before toTree" << endl;
    TTree* tree = toTree(h);
    cout << "drawsparse_draw::after toTree" << endl;

    TString whatToDraw;
    TIter iLeaf(tree->GetListOfLeaves());
    const TLeaf* leaf = 0;
    while ((leaf = (const TLeaf*)iLeaf())) {
        if (whatToDraw.Length())
            whatToDraw += ":";
        whatToDraw += leaf->GetName();
    }
    tree->Draw(whatToDraw, "", "para");
    TParallelCoord* parallelCoord = (TParallelCoord*)gPad->GetListOfPrimitives()->FindObject("ParaCoord");
    cout << "drawsparse_draw::before loop over var" << endl;
    TIter iVar(parallelCoord->GetVarList());
    TParallelCoordVar* var = 0;
    for (Int_t d = 0;(var = (TParallelCoordVar*) iVar()) && d < h->GetNdimensions(); ++d) {
        TAxis* axis = h->GetAxis(d);
        var->SetHistogramBinning(axis->GetNbins());
        var->SetCurrentLimits(axis->GetXmin(), axis->GetXmax());
        var->SetTitle(axis->GetTitle());
    }
    var->SetTitle("bin content");
    cout << "drawsparse_draw::end" << endl;
}

//______________________________________________________________________________
TTree* toTree(THnSparseF* h) {
    // Creates a TTree and fills it with the coordinates of all
    // filled bins. The tree will have one branch for each dimension,
    // and one for the bin content.

    cout << "\ttoTree ... " << flush;

    Int_t dim = h->GetNdimensions();
    TString name(h->GetName()); name += "_tree";
    TString title(h->GetTitle()); title += " tree";

    TTree* tree = new TTree(name, title);
    Double_t* x = new Double_t[dim + 1];
    memset(x, 0, sizeof(Double_t) * (dim + 1));

    TString branchname;
    for (Int_t d = 0; d < dim; ++d) {
        if (branchname.Length())
            branchname += ":";
        TAxis* axis = h->GetAxis(d);
        branchname += axis->GetName();
        branchname += "/D";
    }
    tree->Branch("coord", x, branchname);
    tree->Branch("bincontent", &x[dim], "bincontent/D");

    Int_t *bins = new Int_t[dim];
    for (Long64_t i = 0; i < h->GetNbins(); ++i) {
        x[dim] = h->GetBinContent(i, bins);
        for (Int_t d = 0; d < dim; ++d) {
            x[d] = h->GetAxis(d)->GetBinCenter(bins[d]);
        }

        tree->Fill();
    }

    delete [] bins;

    cout << "DONE" << endl;
    return tree;
}

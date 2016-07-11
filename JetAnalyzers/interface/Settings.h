#ifndef SETTINGS_H
#define SETTINGS_H

//ROOT libraries
#include "TROOT.h"
#include "TSystem.h"
#include "TString.h"
#include "TAttMarker.h"

//C++ libraries
#include <iostream>
#include <string>
#include <map>
#include <vector>

const int NETA                        = 82;
const int NETA_Half                   = 41;
const int NETA_Coarse                 = 8;
const int NETA_HLT                    = 78;
//const int NPtBins                     = 37;
const int NPtBins                     = 51;
const int NPtBinsHLT                  = 40;
const int NPhi                        = 11;
const int NPileup                     = 12;
const int NAlgs                       = 151;
const int NRespBins                   = 600;
const int RespLow                     = 0;
const int RespHigh                    = 6;
const int CorrLow                     = 0;
const int CorrHigh                    = 10;
const int NRhoBins                    = 1000;
const int NRHO                        = 50;
const int RhoLow                      = 0;
const int RhoHigh                     = 100;
const int NTNPU                       = 200;
const int NNPU                        = 200;
const int NAlgorithms                 = 6;
const int NJetTypes                   = 27;
const int NCorrectionLevels           = 7;
const int NConeSizes                  = 10;
const int NDetectorNames              = 4;
const int NDetectorRegions            = 3;
const int NPFcat                      = 7;
const int NPDGIDcat                   = 7;
const int NHistogramTypes             = 12;

//const char Pt[NPtBins+1][10]          = {"10","10.5","11","11.5","12","12.5","13","13.5","14","15","17","20","23","27","30","35","40","45","57","72","90","120","150","200","300","400","550","750","1000","1500","2000","2500","3000","3500","4000","4500","5000","10000"};
const char Pt[NPtBins+1][10]          = {"1","2","3","4","5","6","7","8","9","10","11","12","13","14","15",
										 "17","20","23","27","30","35","40","45","57","72","90","120","150",
										 "200","300","400","550","750","1000","1500","2000","2500","3000",
										 "3500","4000","4500","5000","5500","6000","6500","7000","7500",
										 "8000","8500","9000","9500","10000"};

const char eta_boundaries[NETA+1][10] = {"-5.191","-4.889","-4.716","-4.538","-4.363","-4.191","-4.013","-3.839","-3.664","-3.489",
"-3.314", "-3.139","-2.964","-2.853","-2.65", "-2.5",  "-2.322","-2.172","-2.043","-1.93",
"-1.83",  "-1.74", "-1.653","-1.566","-1.479","-1.392","-1.305","-1.218","-1.131","-1.044",
"-0.957", "-0.879","-0.783","-0.696","-0.609","-0.522","-0.435","-0.348","-0.261","-0.174",
"-0.087", "0",     "0.087", "0.174", "0.261", "0.348", "0.435" ,"0.522", "0.609", "0.696",
"0.783",  "0.879", "0.957", "1.044", "1.131", "1.218", "1.305" ,"1.392", "1.479", "1.566",
"1.653",  "1.74",  "1.83",  "1.93",  "2.043", "2.172", "2.322" ,"2.5",   "2.65",  "2.853",
"2.964",  "3.139", "3.314", "3.489", "3.664", "3.839", "4.013" ,"4.191", "4.363", "4.538",
"4.716","4.889","5.191"};

const char eta_boundaries_coarse[NETA_Coarse+1][10] = {"-5","-3","-2.5","-1.3","0","1.3","2.5","3","5"};

const char eta_boundaries_HLT[NETA_HLT+1][10] = {"-5.191","-4.538","-4.363","-4.191","-4.013","-3.839","-3.664","-3.489",
"-3.314", "-3.139","-2.964","-2.853","-2.65", "-2.5",  "-2.322","-2.172","-2.043","-1.93",
"-1.83",  "-1.74", "-1.653","-1.566","-1.479","-1.392","-1.305","-1.218","-1.131","-1.044",
"-0.957", "-0.879","-0.783","-0.696","-0.609","-0.522","-0.435","-0.348","-0.261","-0.174",
"-0.087", "0",     "0.087", "0.174", "0.261", "0.348", "0.435" ,"0.522", "0.609", "0.696",
"0.783",  "0.879", "0.957", "1.044", "1.131", "1.218", "1.305" ,"1.392", "1.479", "1.566",
"1.653",  "1.74",  "1.83",  "1.93",  "2.043", "2.172", "2.322" ,"2.5",   "2.65",  "2.853",
"2.964",  "3.139", "3.314", "3.489", "3.664", "3.839", "4.013" ,"4.191", "4.363", "4.538","5.191"};

const char pileup_boundaries[NPileup][10] = {"0","0","1","5","6","10","11","15","16","25","26","inf"};

const double veta[NETA+1] = {-5.191,-4.889,-4.716,-4.538,-4.363,-4.191,-4.013,-3.839,-3.664,-3.489,
-3.314, -3.139,-2.964,-2.853,-2.65, -2.5,  -2.322,-2.172,-2.043,-1.93,
-1.83,  -1.74, -1.653,-1.566,-1.479,-1.392,-1.305,-1.218,-1.131,-1.044,
-0.957, -0.879,-0.783,-0.696,-0.609,-0.522,-0.435,-0.348,-0.261,-0.174,
-0.087, 0,     0.087, 0.174, 0.261, 0.348, 0.435 ,0.522, 0.609, 0.696,
0.783,  0.879, 0.957, 1.044, 1.131, 1.218, 1.305 ,1.392, 1.479, 1.566,
1.653,  1.74,  1.83,  1.93,  2.043, 2.172, 2.322 ,2.5,   2.65,  2.853,
2.964,  3.139, 3.314, 3.489, 3.664, 3.839, 4.013 ,4.191, 4.363, 4.538,
4.716,4.889,5.191};

const double veta_half[NETA_Half+1] = {0, 0.087, 0.174, 0.261, 0.348, 0.435 ,0.522, 0.609, 0.696,
0.783,  0.879, 0.957, 1.044, 1.131, 1.218, 1.305 ,1.392, 1.479, 1.566,
1.653,  1.74,  1.83,  1.93,  2.043, 2.172, 2.322 ,2.5,   2.65,  2.853,
2.964,  3.139, 3.314, 3.489, 3.664, 3.839, 4.013 ,4.191, 4.363, 4.538,
4.716,4.889,5.191};

const double veta_coarse[NETA_Coarse+1] = {-5.0,-3.0,-2.5,-1.3,0,1.3,2.5,3,5};

const double veta_HLT[NETA_HLT+1] = {-5.191,-4.538,-4.363,-4.191,-4.013,-3.839,-3.664,-3.489,
-3.314, -3.139,-2.964,-2.853,-2.65, -2.5,  -2.322,-2.172,-2.043,-1.93,
-1.83,  -1.74, -1.653,-1.566,-1.479,-1.392,-1.305,-1.218,-1.131,-1.044,
-0.957, -0.879,-0.783,-0.696,-0.609,-0.522,-0.435,-0.348,-0.261,-0.174,
-0.087, 0,     0.087, 0.174, 0.261, 0.348, 0.435 ,0.522, 0.609, 0.696,
0.783,  0.879, 0.957, 1.044, 1.131, 1.218, 1.305 ,1.392, 1.479, 1.566,
1.653,  1.74,  1.83,  1.93,  2.043, 2.172, 2.322 ,2.5,   2.65,  2.853,
2.964,  3.139, 3.314, 3.489, 3.664, 3.839, 4.013 ,4.191, 4.363, 4.538,5.191};

//const double vpt[NPtBins+1] = {10,10.5,11,11.5,12,12.5,13,13.5,14,15,17,20,23,27,30,35,40,45,57,72,90,120,150,200,300,400,550,750,1000,1500,2000,2500,3000,3500,4000,4500,5000,10000};
const double vpt[NPtBins+1] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,17,20,23,27,30,35,40,45,57,72,90,120,150,
							   200,300,400,550,750,1000,1500,2000,2500,3000,3500,4000,4500,5000,5500,6000,
							   6500,7000,7500,8000,8500,9000,9500,10000};

const double vpt_HLT[NPtBinsHLT+1] = {0.1,1,7,10,10.5,11,11.5,12,12.5,13,13.5,14,15,17,20,23,27,30,35,40,45,57,72,90,120,150,
									  200,300,400,550,750,1000,1500,2000,2500,3000,3500,4000,4500,5000,10000};

const double vphi[NPhi+1] = {-3.141,-2.700,-2.100,-1.500,-0.900,-0.300,0.300,0.900,1.500,2.100,2.700,3.141};

const double vrho[NRHO+1] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,
							 36,37,38,39,40,41,42,43,44,45,46,47,48,49,50};

const double vtnpu[NTNPU+1] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,
							   36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,
						   	   71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,
						   	   106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,
						   	   134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,
						   	   162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,
						   	   190,191,192,193,194,195,196,197,198,199,200};

const double vnpu[NNPU+1] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,
							   36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,
						   	   71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,
						   	   106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,
						   	   134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,
						   	   162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,
						   	   190,191,192,193,194,195,196,197,198,199,200};

const double vpileup[NPileup] = {0,0,1,5,6,10,11,15,16,25,26,1000};

const char algs_all[NAlgs][20] = {"ic5calo","ak4calo","ak5calo","gk5calo","sc5calo","kt4calo","kt5calo","ca4calo","ca5calo","ak5calol1","ak5calol1off","ak5calol1offl2l3","ic5calol2l3","ak5calol2l3","gk5calol2l3","sc5calol2l3","kt4calol2l3","kt5calol2l3","ca4calol2l3","ca5calol2l3","ak7calo","gk7calo","sc7calo","kt6calo","kt7calo","ca6calo","ca7calo","ak7calol1","ak7calol1off","ak7calol2l3","gk7calol2l3","sc7calol2l3","kt6calol2l3","kt7calol2l3","ca6calol2l3","ca7calol2l3","ak5caloHLT","ak5caloHLTl1","ic5pf","ak5pf","gk5pf","sc5pf","kt4pf","kt5pf","ca4pf","ca5pf","ak5pfl1","ak5pfl1l2l3","ak5pfl1off","ic5pfl2l3","ak5pfl2l3","gk5pfl2l3","sc5pfl2l3","kt4pfl2l3","kt5pfl2l3","ca4pfl2l3","ca5pfl2l3","ak7pf","gk7pf","sc7pf","kt6pf","kt7pf","ca6pf","ca7pf","ak7pfl1","ak7pfl1off","ak7pfl2l3","gk7pfl2l3","sc7pfl2l3","kt6pfl2l3","kt7pfl2l3","ca6pfl2l3","ca7pfl2l3","ak5pfchs","ak5pfchsl1","ak5pfchsl1l2l3","ak5pfchsl1off","ak7pfchs","ak7pfchsl1","ak7pfchsl1off","ak5pfHLT","ak5pfHLTl1","ak5pfchsHLT","ak5pfchsHLTl1","ic5jpt","sc5jpt","ak5jpt","ak7jpt","ak5jptl1","ak5jptl1off","ak5jptl1l2l3","ak7jptl1","ak7jptl1off","ak1pf","ak2pf","ak3pf","ak4pf","ak6pf","ak8pf","ak9pf","ak10pf","ak1pfl1","ak2pfl1","ak3pfl1","ak4pfl1","ak6pfl1","ak8pfl1","ak9pfl1","ak10pfl1","ak1pfchs","ak2pfchs","ak3pfchs","ak4pfchs","ak6pfchs","ak8pfchs","ak9pfchs","ak10pfchs","ak1pfchsl1","ak2pfchsl1","ak3pfchsl1","ak4pfchsl1","ak6pfchsl1","ak8pfchsl1","ak9pfchsl1","ak10pfchsl1","ak5calordl1","ak5pfrdl1","ak5pfchsrdl1","ak7calordl1","ak7pfrdl1","ak7pfchsrdl1","ak1puppi","ak2puppi","ak3puppi","ak4puppi","ak5puppi","ak6puppi","ak7puppi","ak8puppi","ak9puppi","ak10puppi","ak1puppil1","ak2puppil1","ak3puppil1","ak4puppil1","ak5puppil1","ak6puppil1","ak7puppil1","ak8puppil1","ak9puppil1","ak10puppil1"};

const double drmax_all[NAlgs] = {0.210,0.250,0.210,0.240,0.200,0.240,0.190,0.220,0.250,0.250,0.250,0.210,0.205,0.210,0.240,0.200,0.240,0.190,0.220,0.350,0.290,0.340,0.275,0.325,0.250,0.300,0.350,0.350,0.280,0.290,0.340,0.275,0.325,0.250,0.300,0.250,0.250,0.105,0.250,0.110,0.135,0.120,0.145,0.115,0.125,0.250,0.250,0.250,0.105,0.110,0.110,0.135,0.120,0.145,0.115,0.125,0.350,0.150,0.180,0.180,0.210,0.150,0.190,0.350,0.350,0.150,0.150,0.180,0.180,0.210,0.150,0.190,0.250,0.250,0.250,0.250,0.350,0.350,0.350,0.250,0.250,0.250,0.250,0.210,0.240,0.205,0.205,0.205,0.205,0.205,0.205,0.205,0.150,0.200,0.300,0.400,0.450,0.500,0.150,0.200,0.300,0.400,0.450,0.500,0.150,0.200,0.300,0.400,0.450,0.500,0.150,0.200,0.300,0.400,0.450,0.500,0.250,0.250,0.250,0.350,0.350,0.350};

const TString algorithms[NAlgorithms] = {"IC","GK","SC","KT","AK","CA"};

const TString jet_types[NJetTypes] = {"Calo","JPT","TRK","PF","PFchs","PFPuppi","PUPPI",
		"CaloHLT","PFHLT","PFchsHLT","JPTHLT",
		"tau",
        "tauHPSlooseCombDBcorrAll",
        "tauHPSlooseCombDBcorrOneProng0Pi0",
        "tauHPSlooseCombDBcorrOneProng1Pi0",
        "tauHPSlooseCombDBcorrOneProng2Pi0",
        "tauHPSlooseCombDBcorrThreeProng0Pi0",
        "tauHPSmediumCombDBcorrAll",
        "tauHPSmediumCombDBcorrOneProng0Pi0",
        "tauHPSmediumCombDBcorrOneProng1Pi0",
        "tauHPSmediumCombDBcorrOneProng2Pi0",
        "tauHPSmediumCombDBcorrThreeProng0Pi0",
        "tauHPStightCombDBcorrAll",
        "tauHPStightCombDBcorrOneProng0Pi0",
        "tauHPStightCombDBcorrOneProng1Pi0",
        "tauHPStightCombDBcorrOneProng2Pi0",
        "tauHPStightCombDBcorrThreeProng0Pi0",
};


const TString correction_levels[NCorrectionLevels] = {"L1Offset","L1FastJet","L2Relative","L3Absolute","L2L3Residual","L5Flavor","L7Parton"};

const TString cone_sizes[NConeSizes] = {"0.1","0.2","0.3","0.4","0.5","0.6","0.7","0.8","0.9","1.0"};

const TString detector_names[NDetectorNames] = {"Barrel","Inner Endcap","Outer Endcap","Forward"};

const TString detector_regions_eta[NDetectorNames] = {"|#eta| < 1.3","1.3 < |#eta| < 2.5","2.5 < |#eta| < 3.0","3.0 < |#eta| < 5.0"};

const TString detector_regions[NDetectorRegions] = {"Barrel","Endcap","Forward"};

const int closureColors[NDetectorNames] = {kBlack, kBlue, kRed, kMagenta};

const int closureShapes[NDetectorNames] = {kFullCircle, kOpenCircle, kFullTriangleUp, kOpenTriangleUp};

const int colDet[NDetectorNames] = {kGreen+3,kBlue+1,kYellow+2,kRed+1};

const int colDetRegions[NDetectorRegions] = {kGreen+3,kBlue+1,kRed+1};

//const TString PFstr[NPFcat] = {"chf","nhf","nef","cef","muf","hfhf","hfef"};
const TString PFstr[NPFcat] = {"nef","cef","muf","nhf","hfhf","hfef","chf"};

const TString PFstr_long[NPFcat] = {"Photons","Em deposits","Em deposits","Neutral hadrons","Hadronic deposits","Em deposits","Charged hadrons"};

const std::vector<std::string> flavors = {"all","uds","ud","s","c","b","q","g","slc","slb"};

const TString pdgidstr[NPDGIDcat] = {"nJ","qJ","cJ","bJ","gJ","aJ","aqJ"};

const TString pdgidstrLegend[NPDGIDcat] = {"nJ (unknown PDGID)","qJ","cJ","bJ","gJ","aJ (all jets)","aqJ (quark jets)"};

const int colPDGID[NPDGIDcat] = {kYellow+2,kGreen+2,kCyan+2,kBlue+2,kRed+2,kBlack,kMagenta+2};

//const int colNpv[6] = {kYellow+2,kGreen+2,kCyan+2,kBlue+2,kMagenta+2,kRed+2}; //0,5,10,15,20,25
const int colNpv[48] = {kRed+2,kOrange+2,kYellow+2,kSpring+2,kGreen+2,kTeal+2,kCyan+2,kAzure+2,kBlue+2,kViolet+2,kMagenta+2,kPink+2,
					   kRed,kOrange,kYellow,kSpring,kGreen,kTeal,kCyan,kAzure,kBlue,kViolet,kMagenta,kPink,
					   kRed-7,kOrange-7,kYellow-7,kSpring-7,kGreen-7,kTeal-7,kCyan-7,kAzure-7,kBlue-7,kViolet-7,kMagenta-7,kPink-7,
					   kRed-9,kOrange-9,kYellow-9,kSpring-9,kGreen-9,kTeal-9,kCyan-9,kAzure-9,kBlue-9,kViolet-9,kMagenta-9,kPink-9};

const TString histogram_types[NHistogramTypes] = {"TH1","TH1F","TH1D","TH2","TH2F","TH2D","TH3","TH3F","TH3D","TProfile","TProfile2D","TProfile3D"};

const double XminCalo[NDetectorNames] = {30.0,30.0,30.0,30.0};

//const double XminPF[NDetectorNames] = {6,6,6,6};
const double XminPF[NDetectorNames] = {10.0,10.0,10.0,10.0};

const double Xmax[NDetectorNames] = {5000,3000,2000,190};

#endif

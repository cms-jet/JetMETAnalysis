////////////////////////////////////////////////////////////////////////////////
//
// JERWriter
// ---------
//
//                                          11/05/2010 Hauke Held <held@cern.ch>
////////////////////////////////////////////////////////////////////////////////



#include "TF1.h"
#include "TFile.h"
#include "TProfile.h"

#include <cassert>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <utility>

#include <string>
#include <vector>
#include <map>
#include <set>
#include <math.h>

using namespace std;

struct JERWriter;

////////////////////////////////////////////////////////////////////////////////
// DEFINE JERWRITER STRUCT
////////////////////////////////////////////////////////////////////////////////
struct JERWriter
{
    // data members
    string                alg;
    string                era;
    string                prefix;
    bool                  isptres;
    bool                  do2ndBins;
    bool                  writeHeader;
    string                profstr;
    map<string,TF1*>      name2func;
    set<string>           binisabs;

    set<string>              xpars;
    set<string>              pars;
    map<string,set<float> >  bins;
    map<string,set<float> >  secondBins;

    vector<string>     dscb;
    map<string,string> dscb2pars;
    map<string,string> dscb2names;
    set<string>        absbins;

    //void init();


    // constructors
    JERWriter(){init();}
    JERWriter(const string& n,const string& e,const string& p, const bool b,const bool b2,const bool b3,string s)
    : alg(n),era(e),prefix(p),isptres(b),do2ndBins(b2),writeHeader(b3),profstr(s){init();}

    // write the resolution file

    void writeJER()
    {
        if (alg.find("l2l3")==string::npos) return;
        if (0==name2func.size()) {cout<<"Fill all entries first!"<<endl;return;}

        decodeEntries();

        if (xpars.size()!=1) {cout<<"Can only handle 1 independent variable at a time!"<<endl;return;}
        string xpar = (*xpars.begin());

        string profy=""; TFile* proffile; TProfile* prof; TF1* proffit(0);
        if (profstr != "" && do2ndBins){
            if (profstr.find("_") == string::npos) { cout << "Please use format p_Yvar_Xvar for profile" << endl; return; }
            string sub = profstr.substr( profstr.find("_")+1 );
            if (sub.find("_") == string::npos) { cout << "Please use format p_Yvar_Xvar for profile" << endl; return; }

            profy = sub.substr( 0, sub.find("_") );
            string profx = sub.substr( sub.find("_")+1 );

            string binstr = (*secondBins.begin()).first;
            if (profx.compare(binstr) != 0) { cout << "X variables don't match. Please use format p_Yvar_Xvar for profile" << endl; return; }

            string filename = profstr + ".root";
            proffile = TFile::Open(filename.c_str());
            prof = (TProfile*) proffile->Get(profstr.c_str());
            proffit = new TF1("proffit", "1++x");
            prof->Fit(proffit, "Q");
        }
    
        map<string,set<float> >::iterator itbins;
        map<string,set<float> >::iterator it2ndBins = secondBins.begin();

        for (itbins=bins.begin();itbins!=bins.end();itbins++) {

            stringstream ssfile;

            //write the file header
            time_t rawtime;
            time (&rawtime);

            if(writeHeader) {
                ssfile<<"######################################################\n"
                      <<"## auto generated file containing JER resolutions     \n"
                      <<"## produced: "<<ctime(&rawtime)
                      <<"## by JetMETAnalysis/JetAnalyzers/bin/JERWriter.h  \n"
                      <<"######################################################\n\n";

                ssfile<<"######################################################\n"
                      <<"## ERA: "<<era<<"  ALG: "<<alg<<"  TYPE: "<<(*itbins).first<<endl
                      <<"######################################################\n\n";      
            }
            if (isptres) {
                //ssfile<<"[resolution]"<<endl;
	            //ssfile<<"{1 "<<(*itbins).first
	            //      <<" 1 "<<xpar<<" DSCB Resolution ";
	            //for (unsigned i=0;i<dscb.size()-1;i++)ssfile<<dscb[i]<<":";
	            //ssfile<<dscb[dscb.size()-1]<<"}\n"
	            //      <<"-5. 5. 3 0. 99999. 1.\n";

            	// special for the mean here...

            	//ssfile<<"[mean]\n"
            	//      <<"{1 "<<(*itbins).first<<" 1 "<<xpar<<" [0] PAR0 \\mu}\n"
            	//      <<"-9.9 9.9 3 0. 99999. 1.\n";


                for (unsigned i=0;i<dscb.size();i++) {
                    //ssfile<<"["<<dscb[i]<<"]\n";

                    stringstream ssfirstgraph;
                    ssfirstgraph<<(*dscb2pars.find(dscb[i])).second;
                    ssfirstgraph<<"Vs"+xpar+"_"+(*itbins).first;

                    set<float>::iterator itvars = (*itbins).second.begin();
                    ssfirstgraph<<(*itvars)<<"to";
                    itvars++;
                    ssfirstgraph<<(*itvars);

                    set<float>::iterator it2ndVars = (*it2ndBins).second.begin();
                    if (do2ndBins){
                        ssfirstgraph<<"_"+(*it2ndBins).first<<(*it2ndVars)<<"to";
                        it2ndVars++;
                        ssfirstgraph<<(*it2ndVars);
                    }      

                    map<string,TF1*>::iterator itfunc = name2func.find(ssfirstgraph.str());

                    TF1* ff = (itfunc==name2func.end()) ? 0 : (*itfunc).second;

                    if (0==ff) {cout<<"WARNING: did not find func "<<ssfirstgraph.str()<<endl;continue;}

                    if (do2ndBins){
                        string xfile;
                        if (profy != "" ) xfile = profy;
                        else xfile = (*it2ndBins).first;

                        ssfile<<"{2    "<<(*itbins).first<<"    "<<xfile <<"    1    "+xpar+"    "
                              <<ff->GetTitle()  //<<" PAR"<<i
                              <<"    "<<(*dscb2names.find(dscb[i])).second<<"}\n";
                    }
                    else{
                        ssfile<<"{1 "<<(*itbins).first<<" 1 "+xpar+" "
		                      <<ff->GetTitle()  //<<" PAR"<<i
                              <<" "<<(*dscb2names.find(dscb[i])).second<<"}\n";
                    }

                    vector<TF1*> fhistory;
                    bool         fskip = false;

                    if (binisabs.end()!=binisabs.find((*itbins).first)) {
	                    // absolute binning!!!

                        set<float>::reverse_iterator ritvars;

                        for (ritvars=(*itbins).second.rbegin();ritvars!=(*itbins).second.rend();ritvars++) {
                            float var1(-1.0),var2(-1.0);

                            stringstream ssfunc;
                            set<float>::reverse_iterator rithelper = ritvars; rithelper++;
                            ssfunc<<(*dscb2pars.find(dscb[i])).second;
                            ssfunc<<"Vs"+xpar+"_"+(*itbins).first; 
                            ssfunc<<(*rithelper)<<"to";
                            var1 *= ((*ritvars)==5.) ? (9.9) : (*ritvars);
                            ssfunc<<(*ritvars);
                            var2 *= ((*rithelper)==5.) ? (9.9) : (*rithelper);

                            if (do2ndBins){
                                for (set<float>::iterator it2ndVars=(*it2ndBins).second.begin();
                                     it2ndVars!=--(*it2ndBins).second.end();it2ndVars++) {

                                    stringstream ss2ndVar;
                                    float var2nd1 = (*it2ndVars);
                                    ss2ndVar<<"_"+(*it2ndBins).first<<var2nd1<<"to";
                                    it2ndVars++;
                                    float var2nd2 = (*it2ndVars);
                                    ss2ndVar<<var2nd2;
                                    it2ndVars--;

                                    string name = ssfunc.str()+ss2ndVar.str();
                                    itfunc = name2func.find( name );

                                    TF1* f = (itfunc==name2func.end()) ? 0 : (*itfunc).second;
                                    if (0==f) {
                                        if (name.find("4.7to0") == string::npos && name.find("5.191to0") == string::npos && name.find("0to0") == string::npos)
                                            cout << name << " did not fit" << endl;
                                        continue;
                                    }

                                    if (profy != "") {
                                        var2nd1 = roundf(proffit->Eval(var2nd1) * 100) / 100;
                                        if (var2nd1 < 2) var2nd1 = 0;
                                        var2nd2 = roundf(proffit->Eval(var2nd2) * 100) / 100;
                                    }

                                    ssfile<<var1<<setw(8)<<(abs(var2)<0.001 ? 0 : var2)<<setw(8)<<var2nd1<<setw(8)<<var2nd2<<setw(8)<<(2+f->GetNpar())
                                    <<setw(8)<< f->GetXmin()<<setw(8)<< f->GetXmax();
                                    for (int itf=0;itf<f->GetNpar()-1;itf++)
                                        ssfile<<setw(15)<<setprecision(4)<<f->GetParameter(itf);
                                    ssfile<<setw(15)<<setprecision(4)<<f->GetParameter(f->GetNpar()-1);
                                    ssfile<<endl;
                                }
                            }
                            else{
                                itfunc = name2func.find(ssfunc.str());

                                TF1* f = (itfunc==name2func.end()) ? 0 : (*itfunc).second;

                                if (0==f&&fhistory.size()>0) f = fhistory.back();
                                else fhistory.push_back(f);

                                if      (0==f&&!fskip) {ssfile<<var1;fskip=true;continue;}
                                else if (0==f&&fskip)  {continue;}

                                if (!fskip) ssfile<<var1;
                                if (0==f) {ssfile<<"ERROR\n";continue;}

                                ssfile<<" "<<var2<<" "<<(2+f->GetNpar())
                                    //<<" 0. 99999. ";
                                      << " " << f->GetXmin() << " " << f->GetXmax() << " ";
                                for (int itf=0;itf<f->GetNpar()-1;itf++) 
                                    ssfile<<f->GetParameter(itf)<<" ";
                                ssfile<<f->GetParameter(f->GetNpar()-1);
                                ssfile<<endl; fskip = false;

                                if (++rithelper==(*itbins).second.rend()) break;
                            }
                        }
                    }
                    for (itvars=(*itbins).second.begin();itvars!=(*itbins).second.end();itvars++) {

                        float var1(1.0),var2(1.0);

                        stringstream ssfunc;
                        ssfunc<<(*dscb2pars.find(dscb[i])).second;
                        ssfunc<<"Vs"+xpar+"_"+(*itbins).first; 
                        ssfunc<<(*itvars)<<"to";
                        var1 *= ((*itvars)==5.) ? (9.9) : (*itvars);
                        set<float>::iterator ithelper = itvars; ithelper++;
                        ssfunc<<(*ithelper);
                        var2 *= ((*ithelper)==5.) ? (9.9) : (*ithelper);

                        if (do2ndBins){
                            for (set<float>::iterator it2ndVars=(*it2ndBins).second.begin();it2ndVars!=--(*it2ndBins).second.end();it2ndVars++) {

                                stringstream ss2ndVar;
                                float var2nd1 = (*it2ndVars);
                                ss2ndVar<<"_"+(*it2ndBins).first<<var2nd1<<"to";
                                it2ndVars++;
                                float var2nd2 = (*it2ndVars);
                                ss2ndVar<<var2nd2;
                                it2ndVars--;

                                string name = ssfunc.str()+ss2ndVar.str();
                                itfunc = name2func.find( name );

                                TF1* f = (itfunc==name2func.end()) ? 0 : (*itfunc).second;
                                if (0==f) {
                                    if (name.find("4.7to0") == string::npos && name.find("0to4.7") == string::npos && name.find("5.191to0") == string::npos && name.find("0to0") == string::npos)
                                        cout << name << " did not fit" << endl;
                                    continue;
                                }

                                if (profy != "") {
                                    var2nd1 = roundf(proffit->Eval(var2nd1) * 100) / 100;
                                    if (var2nd1 < 2) var2nd1 = 0;
                                    var2nd2 = roundf(proffit->Eval(var2nd2) * 100) / 100; 
                                }

                                ssfile<<var1<<setw(8)<<var2<<setw(8)<<var2nd1<<setw(8)<<var2nd2<<setw(8)<<(2+f->GetNpar())
                                      <<setw(8)<< f->GetXmin()<<setw(8)<< f->GetXmax();
                                for (int itf=0;itf<f->GetNpar()-1;itf++)
                                    ssfile<<setw(15)<<setprecision(4)<<f->GetParameter(itf);
                                ssfile<<setw(15)<<setprecision(4)<<f->GetParameter(f->GetNpar()-1);
                                ssfile<<endl;
                            }
                        }
                        else{

                            itfunc = name2func.find(ssfunc.str());

                            TF1* f = (itfunc==name2func.end()) ? 0 : (*itfunc).second;

                            if (0==f&&fhistory.size()>0) f = fhistory.back();
                            else fhistory.push_back(f);

                            if      (0==f&&!fskip) {ssfile<<var1;fskip=true;continue;}
                            else if (0==f&&fskip)  {continue;}

                            if (!fskip) ssfile<<var1;
                            if (0==f) {ssfile<<"ERROR\n";continue;}

                            ssfile<<" "<<var2<<" "<<(2+f->GetNpar())
                                //<<" 0. 99999. ";
                                  << " " << f->GetXmin() << " " << f->GetXmax() << " ";
                            for (int itf=0;itf<f->GetNpar()-1;itf++) 
                                ssfile<<f->GetParameter(itf)<<" ";
                            ssfile<<f->GetParameter(f->GetNpar()-1);
                            ssfile<<endl; fskip = false;

                            if (++ithelper==(*itbins).second.end()) break;
                        }
                    }
                    fhistory.clear();
	           } // dscb[i]++
            }
            else {
                cout<<"ERROR - non pt resolutions not yet implemented!!!"<<endl;
                return;
            }
            //printFile(ssfile);

            ofstream jerfile;
            stringstream ssfilename;

            string algname;

            if      (alg.find("ak5pfl2l3")!=string::npos)   algname="AK5PF";
            else if (alg.find("AK5PFL2L3")!=string::npos)   algname="AK5PF";
            else if (alg.find("ak5calol2l3")!=string::npos) algname="AK5Calo";
            else if (alg.find("AK5CALOL2L3")!=string::npos) algname="AK5Calo";
            else if (alg.find("ak5jptl2l3")!=string::npos)  algname="AK5JPT";
            else if (alg.find("AK5JPTL2L3")!=string::npos)  algname="AK5JPT";
            else if (alg.find("ak5genl2l3")!=string::npos)  algname="AK5GEN";
            else if (alg.find("AK5GENL2L3")!=string::npos)  algname="AK5GEN";
            else algname=alg;


            if (!prefix.empty()) ssfilename<<prefix<<"_";
            if (!era.empty()) ssfilename<<era<<"_";

            ssfilename<<"PtResolution_"<<algname;
            if ((*itbins).first.find("JetEta")==string::npos)
                ssfilename<<"_"<<(*itbins).first;

            ssfilename<<".txt";

            jerfile.open(ssfilename.str().c_str(), ofstream::trunc);
            if (!jerfile.is_open()) {
                cout<<"ERROR: Could not create "<<ssfilename.str()<<endl;
                return;
            }

            jerfile<<ssfile.str();
            jerfile.close();
            cout<<"Created JER file: "<<ssfilename.str()<<endl;     

            if (do2ndBins) it2ndBins++;
        } // itbins++
    } // writeJER
   

    void printFile(const stringstream& ss)
    {
        cout<<ss.str()<<endl;
    }
  
    // check absolute
    void checkAbs()
    {
        map<string,set<float> >::iterator itbins;

        for (itbins=bins.begin();itbins!=bins.end();itbins++) {

            if (binisabs.end()!=binisabs.find((*itbins).first)) continue;
            if (absbins.end()==(absbins.find((*itbins).first))) continue;

            bool abs = true;
            set<float>::iterator itvars;

            for (itvars=(*itbins).second.begin(); itvars!=(*itbins).second.end();itvars++) {
                if ( (*itvars)<0.0 ) abs = false;
            }
            if (abs) binisabs.insert((*itbins).first);
        }
    }

    // decode entries
    void decodeEntries()
    {
        if (alg.find("l2l3")==string::npos) return;
        if (0==name2func.size()) {cout<<"Fill all entries first!"<<endl;return;}

        map<string,TF1*>::iterator itg;
        for (itg=name2func.begin();itg!=name2func.end();itg++) {
            string gname   = (*itg).first;

            size_t vspos   = gname.find("Vs");
            string parname = (vspos!=string::npos) ? gname.substr(0,vspos) : "";
            pars.insert(parname);

            string xparname = (vspos!=string::npos) ? gname.substr(vspos+2,gname.find("_")-vspos-2) : "";
            xpars.insert(xparname);

            int numBins = 2;
            if (!do2ndBins){
                assert (gname.find("_")==gname.rfind("_"));
                assert (gname.find("to")==gname.rfind("to"));
                numBins = 1;
            }

            for (int i=0; i<numBins; i++){
                size_t binpos = gname.find("_");
                string binname = (binpos!=string::npos) ? gname.substr(binpos+1) : "";

                size_t bin1pos = binname.find_first_of("1234567890");
                size_t topos   = binname.find("to");

                string bintype = binname.substr(0,bin1pos);
                string svar1   = binname.substr(bin1pos,topos-bin1pos);
                string svar2   = binname.substr(topos+2);

                stringstream vvar1,vvar2; float fvar1,fvar2;
                vvar1<<svar1; vvar2<<svar2; 
                vvar1>>fvar1; vvar2>>fvar2;

                set<float> vars;
                vars.insert(fvar1);vars.insert(fvar2);

                if (i==0) { //firstBins   
                    map<string,set<float> >::iterator itbins = bins.find(bintype);
                    if (itbins==bins.end()) {
                        bins.insert(pair<string,set<float> >(bintype,vars));
                    }
                    else {
                        (*itbins).second.insert(fvar1);
                        (*itbins).second.insert(fvar2);
                    }
                    gname = binname;
                }
                else if (i==1) { //secondBins
                    map<string,set<float> >::iterator itbins = secondBins.find(bintype);
                    if (itbins==secondBins.end()) {
                        secondBins.insert(pair<string,set<float> >(bintype,vars));
                    }
                    else {
                        (*itbins).second.insert(fvar1);
                        (*itbins).second.insert(fvar2);
                    }
                }
            }
        } // name2func++
    
        checkAbs();
    }

    void printDecode()
    {
        set<string>::iterator itpars;
        map<string,set<float> >::iterator itbins;
        set<float>::iterator itvals;

        for (itpars=pars.begin();itpars!=pars.end();itpars++) {
            cout<<"Par: "<<(*itpars)<<" has bins: "<<endl;
            for (itbins=bins.begin();itbins!=bins.end();itbins++) {
                cout<<"->bintype: "<<(*itbins).first<<endl;
                cout<<"--> ";
                for (itvals=(*itbins).second.begin(); itvals!=(*itbins).second.end();itvals++) {
                    cout<<(*itvals)<<" ";
                }
                cout<<endl;
            }
        }
    }

    // add a new graph / fit pair
    void addEntry(const string& graphname,TF1* func)
    {
        if (alg.find("l2l3")==string::npos) return;
        if (isptres && graphname.find("VsRefPt_")==string::npos &&
            graphname.find("VsJetPt_")==string::npos) return;

        name2func.erase(graphname);
        name2func.insert(pair<string,TF1*>(graphname,func));
    }

    // print a list of all graphnames and functions...
    void printEntries()
    {
        cout<<"***Printing list of all entries: "<<alg<<endl;

        map <string,TF1*>::iterator it;
        for (it=name2func.begin();it!=name2func.end();it++) {
            cout<<"gname: "<<(*it).first<<"   ftitle: "<<(*it).second->GetTitle()<<endl;
        }
    }

    // initialize variables
    void init()
    {
        // possible absolute binning variables

        absbins.insert("JetY");
        absbins.insert("JetEta");

        // for the dscb function

        //dscb.push_back("mean");
        dscb.push_back("sigma");
        //dscb.push_back("aone");
        //dscb.push_back("pone");
        //dscb.push_back("atwo");
        //dscb.push_back("ptwo");

        dscb2pars["mean"]  =string("RelRsp");
        dscb2pars["sigma"] =string("RelRes");
        dscb2pars["aone"]  =string("Aone");
        dscb2pars["atwo"]  =string("Atwo");
        dscb2pars["pone"]  =string("Pone");
        dscb2pars["ptwo"]  =string("Ptwo");

        dscb2names["mean"] =string("\\mu");
        dscb2names["sigma"]=string("Resolution");
        dscb2names["aone"] =string("Aone");
        dscb2names["atwo"] =string("Atwo");
        dscb2names["pone"] =string("Pone");
        dscb2names["ptwo"] =string("Ptwo");
    }

};

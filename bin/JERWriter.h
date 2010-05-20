////////////////////////////////////////////////////////////////////////////////
//
// JERWriter
// ---------
//
//                                          11/05/2010 Hauke Held <held@cern.ch>
////////////////////////////////////////////////////////////////////////////////



#include <TF1.h>

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



struct JERWriter;

////////////////////////////////////////////////////////////////////////////////
// DEFINE JERWRITER STRUCT
////////////////////////////////////////////////////////////////////////////////
struct JERWriter
{
  // data members
  std::string                alg;
  std::string                era;
  std::string                prefix;
  bool                       isptres;
  std::map<std::string,TF1*> name2func;
  std::set<std::string>      binisabs;

  std::set<std::string>                   pars;
  std::map<std::string,std::set<float> >  bins;

  std::vector<std::string>   dscb;
  std::map<std::string,std::string> dscb2pars;
  std::map<std::string,std::string> dscb2names;
  std::set<std::string>             absbins;

  //void init();


  // constructors
  JERWriter(){init();}
  JERWriter(const std::string& n,
	    const std::string& e,
	    const std::string& p,
	    const bool b)
    : alg(n),era(e),prefix(p),isptres(b){init();}


  // write the resolution file

  void writeJER()
  {
    using namespace std;
    if (alg.find("l2l3")==string::npos) return;
    if (0==name2func.size()) {cout<<"Fill all entries first!"<<endl;return;}

    decodeEntries();
    
    map<string,set<float> >::iterator itbins;
    for (itbins=bins.begin();itbins!=bins.end();itbins++) {

      stringstream ssfile;

      //write the file header
      time_t rawtime;
      time (&rawtime);

      ssfile<<"######################################################\n"
	    <<"## auto generated file containing JER resolutions     \n"
	    <<"## produced: "<<ctime(&rawtime)
	    <<"## by JetMETAnalysis/JetAnalyzers/bin/JERWriter.h  \n"
	    <<"######################################################\n\n";
     
      ssfile<<"######################################################\n"
	    <<"## ERA: "<<era<<"  ALG: "<<alg<<"  TYPE: "<<(*itbins).first<<endl
	    <<"######################################################\n\n";
      
      ssfile<<"[resolution]"<<endl;
      
      if (isptres) {
	ssfile<<"{1 "<<(*itbins).first
	      <<" 1 RefPt DSCB Resolution ";
	for (unsigned i=0;i<dscb.size()-1;i++)ssfile<<dscb[i]<<":";
	ssfile<<dscb[dscb.size()-1]<<"}\n"
	      <<"-5. 5. 3 0. 99999. 1.\n";

	// special for the mean here...

	ssfile<<"[mean]\n"
	      <<"{1 "<<(*itbins).first<<" 1 RefPt [0] PAR0 \\mu}\n"
	      <<"-9.9 9.9 3 0. 99999. 1.\n";


	for (unsigned i=1;i<dscb.size();i++) {
	  ssfile<<"["<<dscb[i]<<"]\n";


	  stringstream ssfirstgraph;
	  ssfirstgraph<<(*dscb2pars.find(dscb[i])).second;
	  ssfirstgraph<<"VsRefPt_"+(*itbins).first;

	  set<float>::iterator itvars = (*itbins).second.begin();
	  ssfirstgraph<<(*itvars)<<"to";
	  itvars++;
	  ssfirstgraph<<(*itvars);

	  map<string,TF1*>::iterator itfunc = name2func.find(ssfirstgraph.str());

	  TF1* ff = (itfunc==name2func.end()) ? 0 : (*itfunc).second;
	  
	  if (0==ff) {cout<<"ERROR: did not find func "<<ssfirstgraph.str()<<endl;return;}
	  
	  ssfile<<"{1 "<<(*itbins).first<<" 1 RefPt "
		<<ff->GetTitle()<<" PAR"<<i<<" "
		<<(*dscb2names.find(dscb[i])).second<<"}\n";


	  vector<TF1*> fhistory;
	  bool         fskip = false;
	  
	  if (binisabs.end()!=binisabs.find((*itbins).first)) {
	    // absolute binning!!!

	    set<float>::reverse_iterator ritvars;

	    for (ritvars=(*itbins).second.rbegin();
		 ritvars!=(*itbins).second.rend();ritvars++) {

	      float var1(-1.0),var2(-1.0);
	    
	      stringstream ssfunc;
	      set<float>::reverse_iterator rithelper = ritvars; rithelper++;
	      ssfunc<<(*dscb2pars.find(dscb[i])).second;
	      ssfunc<<"VsRefPt_"+(*itbins).first; 
	      ssfunc<<(*rithelper)<<"to";
	      var1 *= ((*ritvars)==5.) ? (9.9) : (*ritvars);
	      ssfunc<<(*ritvars);
	      var2 *= ((*rithelper)==5.) ? (9.9) : (*rithelper);
	      itfunc = name2func.find(ssfunc.str());

	      TF1* f = (itfunc==name2func.end()) ? 0 : (*itfunc).second;

	      if (0==f&&fhistory.size()>0) f = fhistory.back();
	      else fhistory.push_back(f);

	      if      (0==f&&!fskip) {ssfile<<var1;fskip=true;continue;}
	      else if (0==f&&fskip)  {continue;}

	      if (!fskip) ssfile<<var1;
	      if (0==f) {ssfile<<"ERROR\n";continue;}

	      ssfile<<" "<<var2<<" "<<(2+f->GetNpar())
		    <<" 0. 99999. ";
	      for (int itf=0;itf<f->GetNpar()-1;itf++) 
		ssfile<<f->GetParameter(itf)<<" ";
	      ssfile<<f->GetParameter(f->GetNpar()-1);
	      ssfile<<endl; fskip = false;
	    
	    if (++rithelper==(*itbins).second.rend()) break;
	    }
	  }

	  for (itvars=(*itbins).second.begin();
	       itvars!=(*itbins).second.end();itvars++) {
	    
	    float var1(1.0),var2(1.0);
	    
	    stringstream ssfunc;
	    ssfunc<<(*dscb2pars.find(dscb[i])).second;
	    ssfunc<<"VsRefPt_"+(*itbins).first; 
	    ssfunc<<(*itvars)<<"to";
	    var1 *= ((*itvars)==5.) ? (9.9) : (*itvars);
	    set<float>::iterator ithelper = itvars; ithelper++;
	    ssfunc<<(*ithelper);
	    var2 *= ((*ithelper)==5.) ? (9.9) : (*ithelper);
	    itfunc = name2func.find(ssfunc.str());

	    TF1* f = (itfunc==name2func.end()) ? 0 : (*itfunc).second;

	    if (0==f&&fhistory.size()>0) f = fhistory.back();
	    else fhistory.push_back(f);

	    if      (0==f&&!fskip) {ssfile<<var1;fskip=true;continue;}
	    else if (0==f&&fskip)  {continue;}

	    if (!fskip) ssfile<<var1;
	    if (0==f) {ssfile<<"ERROR\n";continue;}

	    ssfile<<" "<<var2<<" "<<(2+f->GetNpar())
		  <<" 0. 99999. ";
	    for (int itf=0;itf<f->GetNpar()-1;itf++) 
	      ssfile<<f->GetParameter(itf)<<" ";
	    ssfile<<f->GetParameter(f->GetNpar()-1);
	    ssfile<<endl; fskip = false;
	  
	    if (++ithelper==(*itbins).second.end()) break;
	  }
	  
	  fhistory.clear();

	} // dscb[i]++

      }
      else {
	cout<<"ERROR - non pt resolutions not yet implemented!!!"<<endl;return;
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

      ssfilename<<"PtResolution_"
		<<algname;
      if ((*itbins).first.find("JetEta")==string::npos)
	ssfilename<<"_"<<(*itbins).first;
		  
      ssfilename<<".txt";

      jerfile.open(ssfilename.str().c_str(), ofstream::trunc);
      if (!jerfile.is_open()) {
	cout<<"ERROR: Could not create "<<ssfilename.str()<<endl;return;
      }
      jerfile<<ssfile.str();
      jerfile.close();
      cout<<"Created JER file: "<<ssfilename.str()<<endl;
      
    } // itbins++
  } // writeJER


  void printFile(const std::stringstream& ss)
  {
    using namespace std;
    
    cout<<ss.str()<<endl;
  }
  
  // check absolute
  void checkAbs()
  {
    using namespace std;
    map<string,set<float> >::iterator itbins;
    
    for (itbins=bins.begin();itbins!=bins.end();itbins++) {
   
      if (binisabs.end()!=binisabs.find((*itbins).first)) continue;
      if (absbins.end()==(absbins.find((*itbins).first))) continue;
      
      bool abs = true;
      set<float>::iterator itvars;

      for (itvars=(*itbins).second.begin();
	   itvars!=(*itbins).second.end();itvars++) {
	if ( (*itvars)<0.0 ) abs = false;
      }
      if (abs) binisabs.insert((*itbins).first);
    }
  }

  // decode entries
  void decodeEntries()
  {
    using namespace std;

    if (alg.find("l2l3")==string::npos) return;
    if (0==name2func.size()) {cout<<"Fill all entries first!"<<endl;return;}
    
    map<string,TF1*>::iterator itg;
    for (itg=name2func.begin();itg!=name2func.end();itg++) {
      string gname   = (*itg).first;

      size_t vspos   = gname.find("Vs");
      string parname = (vspos!=string::npos) ? gname.substr(0,vspos) : "";
      pars.insert(parname);

      assert (gname.find("_")==gname.rfind("_"));
      assert (gname.find("to")==gname.rfind("to"));

      size_t binpos  = gname.find("_");
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
      
      map<string,set<float> >::iterator itbins = bins.find(bintype);

      if (itbins==bins.end()) {
	bins.insert(pair<string,set<float> >(bintype,vars));
      }
      else {
	(*itbins).second.insert(fvar1);
	(*itbins).second.insert(fvar2);
      }
    } // name2func++

    checkAbs();
  }

  void printDecode()
  {
    using namespace std;

    set<string>::iterator itpars;
    map<string,set<float> >::iterator itbins;
    set<float>::iterator itvals;

    for (itpars=pars.begin();itpars!=pars.end();itpars++) {
      cout<<"Par: "<<(*itpars)<<" has bins: "<<endl;
      for (itbins=bins.begin();itbins!=bins.end();itbins++) {
	cout<<"->bintype: "<<(*itbins).first<<endl;
	cout<<"--> ";
	for (itvals=(*itbins).second.begin();
	     itvals!=(*itbins).second.end();itvals++) {
	  cout<<(*itvals)<<" ";
	}
	cout<<endl;
      }
    }

  }

  // add a new graph / fit pair
  void addEntry(const std::string& graphname,TF1* func)
  {
    using namespace std;
    if (alg.find("l2l3")==string::npos) return;
    if (isptres && graphname.find("VsRefPt_")==string::npos) return;

    name2func.erase(graphname);
    name2func.insert(pair<string,TF1*>(graphname,func));
  }

  // print a list of all graphnames and functions...
  void printEntries()
  {
    using namespace std;

    cout<<"***Printing list of all entries: "<<alg<<endl;

    map <string,TF1*>::iterator it;
    for (it=name2func.begin();it!=name2func.end();it++) {
      cout<<"gname: "<<(*it).first<<"   ftitle: "<<(*it).second->GetTitle()<<endl;
    }
  }

  // initialize variables
  void init()
  {
    using namespace std;
    // possible absolute binning variables

    absbins.insert("JetY");
    absbins.insert("JetEta");

    // for the dscb function

    dscb.push_back("mean");
    dscb.push_back("sigma");
    dscb.push_back("aone");
    dscb.push_back("pone");
    dscb.push_back("atwo");
    dscb.push_back("ptwo");

    dscb2pars["mean"]  =string("RelRsp");
    dscb2pars["sigma"] =string("RelRes");
    dscb2pars["aone"]  =string("Aone");
    dscb2pars["atwo"]  =string("Atwo");
    dscb2pars["pone"]  =string("Pone");
    dscb2pars["ptwo"]  =string("Ptwo");

    dscb2names["mean"] =string("\\mu");
    dscb2names["sigma"]=string("\\sigma");
    dscb2names["aone"] =string("Aone");
    dscb2names["atwo"] =string("Atwo");
    dscb2names["pone"] =string("Pone");
    dscb2names["ptwo"] =string("Ptwo");
  }

};

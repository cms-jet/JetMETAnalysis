////////////////////////////////////////////////////////////////////////////////
//
// jet_add_histos_x
// ----------------
//
//                      09/09/2016 Alexx Perloff <alexx.stephen.perloff@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"
#include "JetMETAnalysis/JetUtilities/interface/ObjectLoader.h"

#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TFile.h"
#include "TH1F.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <exception>


using namespace std;


////////////////////////////////////////////////////////////////////////////////
// declare local functions
////////////////////////////////////////////////////////////////////////////////
void set_draw_attributes(TH1* h,
			 unsigned index,
			 bool fill,
			 const vector<int>& colors,
			 const vector<int>& fillstyles,
			 const vector<int>& markstyles);

vector<string> get_applicable_ranges(string quantity, const vector<string> &sranges);

vector<pair<double,double> > convert_range_from_string(const ObjectLoader<TH1F> &hl, const string &srange);

void split(const string &s, string delim, vector<string> &elems);

vector<string> split(const string &s, string delim);

string make_full_range(const string &variable);

std::string ReplaceString(std::string subject, const std::string& search, const std::string& replace);

void ReplaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace);

bool contains(const vector<string>& collection,const string& element);

bool histogram_in_range(const ObjectLoader<TH1F>& hl, const vector<unsigned int>& indices, const vector<pair<double,double> >& ranges);

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char** argv) {
	CommandLine cl;
	if (!cl.parse(argc,argv)) return 0;

	string         input      = cl.getValue<string> ("input");
    string         output     = cl.getValue<string> ("output",                      "");
	vector<string> algs       = cl.getVector<string>("algs",                        "");
	vector<string> variables  = cl.getVector<string>("variables", "pThat:JetEta:RefPt");
	vector<string> sranges    = cl.getVector<string>("sranges", "pThat:JetEta!:RefPt!");
	string         prefix     = cl.getValue<string> ("prefix",                      "");
	string         suffix     = cl.getValue<string> ("suffix",                      "");

	bool           fill       = cl.getValue<bool>   ("fill",                     false);
	vector<int>    colors     = cl.getVector<int>   ("colors",                      "");
	vector<int>    fillstyles = cl.getVector<int>   ("fillstyles",                  "");
	vector<int>    markstyles = cl.getVector<int>   ("markstyles",                  "");

	int            rebin      = cl.getValue<int>    ("rebin",                        1);
    bool           norm       = cl.getValue<bool>   ("norm",                     false);
	bool           verbose    = cl.getValue<bool>   ("verbose",                  false);

	if (!cl.check()) return 0;
	cl.print();

	if (verbose) cout<<"Verbosity not implemented...:/"<<endl;

    //
    // construct output file name from input file name if none given
    //
	if (output.empty()) {
		size_t pos=input.find(".root");
		output=input.substr(0,pos)+"_a.root";
		cout<<"*** write output to "<<output<<endl;
	}

    //
    // open input file and loop over input directories (=algorithms)
    //
	TFile* ifile = new TFile(input.c_str(),"READ");
	if (!ifile->IsOpen()) { cout<<"Can't open "<<input<<endl; return 0; }

	TFile* ofile = new TFile(output.c_str(),"UPDATE");
	if (!ofile->IsOpen()) { cout<<"Can't create "<<output<<endl; return 0; }

	TIter nextDir(ifile->GetListOfKeys());
	TKey* dirKey(0);
	while ((dirKey=(TKey*)nextDir())) {

		if (strcmp(dirKey->GetClassName(),"TDirectoryFile")!=0) continue;

		TDirectoryFile* idir = (TDirectoryFile*)dirKey->ReadObj();
		string alg(idir->GetName());

		if (algs.size()>0&&!contains(algs,alg)) {
			cout<<"No dir "<<alg<<" found"<<endl;
			continue;
		}

		if (0!=ofile->Get(idir->GetName())) {
			cout<<"directory '"<<alg<<"' exists already in "<<output<<", skip!"<<endl;
			continue;
		}

		TDirectoryFile* odir = (TDirectoryFile*)ofile->mkdir(idir->GetName());
		odir->cd();

		cout<<alg<<" ... "<<endl;
    
		vector<string> histogram_names_added;
		vector<TH1F*>  summed_histograms;

		//
		// loop over variables
		//
		for (unsigned int ivar=0; ivar<variables.size(); ivar++) {

			string variable=variables[ivar];
			size_t pos = variable.find(':');
			string tmp_quantity = (pos==string::npos) ? variable : variable.substr(0,pos);
			auto applicable_sranges = get_applicable_ranges(tmp_quantity, sranges);
			if(applicable_sranges.size()==0) {
				applicable_sranges.push_back(make_full_range(variable));
			}

			//
			// loop over the ranges and sum all histograms that meet the range requirements
			//
			for (unsigned int irange=0; irange<applicable_sranges.size(); irange++) {
				ObjectLoader<TH1F> hl;
				hl.load_objects(idir,variable);

				auto range = convert_range_from_string(hl, applicable_sranges[irange]);

				hl.begin_loop();

				vector<unsigned int> indices; TH1F* h(0); unsigned int ihisto(0);
				unsigned int icolor(0);
				while ((h=hl.next_object(indices))) {

					// Create the summed histogram
					if(ihisto==0) {
						stringstream ssname;
						if(!prefix.empty()) ssname<<prefix<<"_";
						ssname<<hl.quantity();
						for (unsigned int i=0;i<hl.nvariables();i++) {
							ssname<<"_"<<hl.variable(i)<<range[i].first<<"to"<<range[i].second;
						}
						if (!suffix.empty()) ssname<<"_"<<suffix;

						summed_histograms.push_back((TH1F*)h->Clone(ssname.str().c_str()));
						summed_histograms.back()->Reset();
						summed_histograms.back()->Sumw2();

						if(1!=rebin) {
							summed_histograms.back()->Rebin(rebin);
						}

						// Format the summed histogram
						icolor=ivar+irange;
						if (colors.empty()) summed_histograms.back()->SetLineColor(kBlack);
						else if (icolor>colors.size()-1) {
							cout<<"WARNING: #Histo Vs specified colors mismatch!"<<endl;
							summed_histograms.back()->SetLineColor(kBlack);
						}
						else summed_histograms.back()->SetLineColor(colors[icolor]);

						set_draw_attributes(summed_histograms.back(),icolor,fill,colors,fillstyles,markstyles);
					}

					if(histogram_in_range(hl, indices, range)) {

						if (1!=rebin) { 
							h->Rebin(rebin);
						}

						if (norm) {
							if ( !h->GetSumw2N() ) h->Sumw2();
							h->Scale(1./h->Integral());
						}

					
						histogram_names_added.push_back(h->GetName());
						summed_histograms.back()->Add(h);
					}

					ihisto++;
				} // histos
			} // ranges
		} // variables

    	//
    	// loop over all other histograms histograms
    	//
		TIter nextHist(idir->GetListOfKeys());
		TKey* histKey(0);
		while ((histKey=(TKey*)nextHist())) {
			if (strcmp(histKey->GetClassName(),"TH1F")!=0) continue;

			TH1F* hrsp = (TH1F*)histKey->ReadObj();
			string histname(hrsp->GetName());
			if(!contains(histogram_names_added,histname))
				hrsp->Write();
		}

		cout<<"summed histograms for *"+alg+"* completed ..."<<flush;
		odir->Write();
		odir->DeleteAll();
		delete odir;
		cout<<" and saved!\n"<<endl;
	}

	//
 	// update the input file
  	//
	cout<<"update output file "<<output<<" ..."<<flush;
	gROOT->GetListOfFiles()->Remove(ofile);
	ofile->Close();
	delete ofile;
	gROOT->GetListOfFiles()->Remove(ifile);
	ifile->Close();
	delete ifile;
	cout<<" DONE."<<endl;

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// implement local functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void set_draw_attributes(TH1* h,unsigned index,bool fill,
			 const vector<int>& colors,
			 const vector<int>& fillstyles,
			 const vector<int>& markstyles)
{
  if (0==h) return;

  Style_t markstyle = (markstyles.empty() || (index>markstyles.size()-1)) ? h->GetMarkerStyle() : markstyles[index];

  h->SetMarkerStyle(markstyle);

  if (fill) {
    Style_t fillstyle = (fillstyles.empty() || (index>fillstyles.size()-1)) ? (3001+index) : fillstyles[index];
    h->SetFillColor(h->GetLineColor());
    h->SetFillStyle(fillstyle);
  }
}

//______________________________________________________________________________
vector<string> get_applicable_ranges(string quantity, const vector<string> &sranges) {
	vector<string> ret;
	for(unsigned int irange=0; irange<sranges.size(); irange++) {
		size_t pos = sranges[irange].find(':');
		string tmp = (pos==string::npos) ? sranges[irange] : sranges[irange].substr(0,pos);
		if(quantity==tmp)
			ret.push_back((pos==string::npos)?"":sranges[irange].substr(pos+1));
	}
	if(ret.size()>1) {
		cout << "WARNING::jet_add_histos_x::get_applicable_ranges There are multiple ranges specified for quantity " << quantity << "." << endl
			 << "\tIt is up to the user to make sure that the ranges make sense (i.e. if there are overlaps in the ranges"
			 << " make sure that was the intention)." << endl; 
	}
	return ret;
}

//______________________________________________________________________________
vector<pair<double,double> > convert_range_from_string(const ObjectLoader<TH1F> &hl, const string &srange) {
	vector<pair<double,double> > ranges;
	vector<string> variables_ = split(srange, ":");
	size_t pos(0);

    if(variables_.size()!=hl.nvariables()) {
    	cout << "ERROR::jet_add_histos_x::convert_range_from_string The number of variables found in the range string ("
    	     << variables_.size() << ") does not equal the number of variables from the ObjectLoader (" << hl.nvariables() << ")" << endl;
    	std::terminate();
    }

	for (unsigned int i=0;i<variables_.size();i++) {
		double min(0), max(0);

		// Check to see if the user wants to use merge the entire range for this variable
		pos=variables_[i].find("!");
		if (pos!=string::npos) {
			min = hl.minimum(i,0);
			max = hl.maximum(i,hl.nobjects(i)-1);
			ranges.push_back(make_pair(min,max));
		}
		else {
      		string tmp=variables_[i].substr(hl.variable(i).length());
      		vector<string> vtmp=split(tmp,"to");
      		if (vtmp.size()!=2) continue;
      		stringstream ss;
      		ss<<vtmp.front()<<" "<<vtmp.back(); ss>>min>>max;
      		ranges.push_back(make_pair(min,max));
		}
	}

    if(ranges.size()!=variables_.size()) {
    	cout << "ERROR::jet_add_histos_x::convert_range_from_string The number of ranges found (" << ranges.size() << ") does not equal "
    		 << "the number of variables (" << variables_.size() << ")" << endl;
    	std::terminate();
    }

	return ranges;
}

//______________________________________________________________________________
void split(const string &s, string delim, vector<string> &elems) {
  string tmp=s;
  while (tmp.length()>0) {
    size_t pos=tmp.find(delim);
    if (pos==string::npos) {
      elems.push_back(tmp);
      tmp="";
    }
    else {
      elems.push_back(tmp.substr(0,pos));
      tmp=tmp.substr(pos+delim.length());
    }
  }
}

//______________________________________________________________________________
vector<string> split(const string &s, string delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}

//______________________________________________________________________________
string make_full_range(const string& variable) {
	string full_range_variable;
	vector<string> elems;
	split(variable,":",elems);
	for(unsigned int ielem=0; ielem<elems.size(); ielem++) {
		full_range_variable+=elems[ielem];
		if(ielem!=0) full_range_variable+="!";
		if(ielem!=elems.size()-1) full_range_variable+=":";
	}
	return full_range_variable;
}

//______________________________________________________________________________
std::string ReplaceString(std::string subject, const std::string& search, const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
    return subject;
}

//______________________________________________________________________________
void ReplaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
}

//______________________________________________________________________________
bool contains(const vector<string>& collection,const string& element) {
  vector<string>::const_iterator it;
  for (it=collection.begin();it!=collection.end();++it)
    if ((*it)==element) return true;
  return false;
}

//______________________________________________________________________________
bool histogram_in_range(const ObjectLoader<TH1F>& hl, const vector<unsigned int>& indices, const vector<pair<double,double> >& ranges) {
	bool in_range = true;
	for (unsigned int i=0;i<hl.nvariables();i++) {
		double varmin  = hl.minimum(i,indices[i]);
    	double varmax  = hl.maximum(i,indices[i]);
    	if(varmin<ranges[i].first || varmax>ranges[i].second) in_range = false;
	}
	return in_range;
}

////////////////////////////////////////////////////////////////////////////////
//
// HistogramLoader
// ---------------
//
//            07/07/2008 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/HistogramLoader.h"

#include <TKey.h>

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <map>


using namespace std;


////////////////////////////////////////////////////////////////////////////////
// declaration of local functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
vector<string> split(const string& str,const string& delim);


////////////////////////////////////////////////////////////////////////////////
// construction/destruction
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
HistogramLoader::HistogramLoader()
  : iloop_(0)
{
  
}


//______________________________________________________________________________
HistogramLoader::~HistogramLoader()
{
  
}


////////////////////////////////////////////////////////////////////////////////
// implementation of member functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
bool HistogramLoader::load_histograms(TDirectory* d,const string& expression)
{
  string tmp=expression;
  unsigned int pos=tmp.find(':');
  quantity_=(pos==string::npos)?tmp:tmp.substr(0,pos);
  tmp=(pos==string::npos)?"":tmp.substr(pos+1);
  if (tmp!="") variables_=split(tmp,":");
  
  map<int,double> fixed_values;
  for (unsigned int i=0;i<variables_.size();i++) {
    pos=variables_[i].find("@");
    if (pos==string::npos) continue;
    string value_as_string=variables_[i].substr(pos+1);
    variables_[i]=variables_[i].substr(0,pos);
    double value; stringstream ss; ss<<value_as_string;ss>>value;
    fixed_values[i]=value;
  }

  nhistograms_.clear();
  for (unsigned int i=0;i<variables_.size();i++) nhistograms_.push_back(0);
  bounds_.resize(nhistograms_.size());
  
  TIter next(d->GetListOfKeys());
  TKey* key(0);
  while ((key=(TKey*)next())) {
    if (strcmp(key->GetClassName(),"TH1F")!=0) continue;
    string histname(key->GetName());
    if (histname.find(quantity_)!=0) continue;
    vector<string> histo_variables;
    if (histname!=quantity_)
      histo_variables=split(histname.substr(quantity_.length()+1),"_");
    vector< std::pair<double,double> > bounds;
    if (variables_.size()!=histo_variables.size()) continue;
    for (unsigned int i=0;i<variables_.size();i++) {
      int pos=histo_variables[i].find(variables_[i]);
      if (pos!=0) continue;
      string tmp=histo_variables[i].substr(variables_[i].length());
      vector<string> vtmp=split(tmp,"to");
      if (vtmp.size()!=2) continue;
      stringstream ss; double min; double max;
      ss<<vtmp.front()<<" "<<vtmp.back(); ss>>min>>max;
      if (fixed_values.find(i)!=fixed_values.end()&&
	  (fixed_values[i]<min||fixed_values[i]>=max)) continue;
      bounds.push_back(std::make_pair(min,max));
    }
    if (bounds.size()!=variables_.size()) continue;
    
    vector<unsigned int> indices;
    for (unsigned int ivar=0;ivar<variables_.size();ivar++) {
      vector< std::pair<double,double> > varbounds=bounds_[ivar];
      if (varbounds.size()==0||bounds[ivar].first>=varbounds.back().second) {
	varbounds.push_back(bounds[ivar]);
	bounds_[ivar]=varbounds;
	indices.push_back(nhistograms_[ivar]);
	nhistograms_[ivar]++;
      }
      else {
	int i(0);
	vector< std::pair<double,double> >::iterator it=varbounds.begin();
	while (it!=varbounds.end()) {
	  if (*it==bounds[ivar]) {
	    indices.push_back(i);
	    break;
	  }
	  else if (it->first>bounds[ivar].second) {
	    vector< std::pair<double,double> >::iterator last=it;last--;
	    varbounds.insert(last,bounds[ivar]);
	    bounds_[ivar]=varbounds;
	    nhistograms_[ivar]++;
	    indices.push_back(i);
	    break;
	  }

	  ++i;++it;
	}
      }
    }
    compute_offset();
    set_histogram(indices,(TH1F*)key->ReadObj());
  }

  
  // DEBUG
  /*
    for (unsigned int ivar=0;ivar<variables_.size();ivar++) {
    for (unsigned int ih=0;ih<nhistograms_[ivar];ih++) {
    cout<<ivar<<" "<<ih
    <<" minimum="<<minimum(ivar,ih)
    <<" maximum="<<maximum(ivar,ih)<<endl;
    }
    }
    vector<unsigned int> indices; TH1F* h;
    begin_loop();
    while ((h=next_histogram(indices))) {
    for (unsigned int i=0;i<indices.size();i++) cout<<indices[i]<<" ";
    cout<<h->GetName()<<endl;  }
  */
  
  return true;
}


//______________________________________________________________________________
TH1F* HistogramLoader::histogram(unsigned int i) const
{
  vector<unsigned int> indices;
  indices.push_back(i);
  return histogram(indices);
}


//______________________________________________________________________________
TH1F* HistogramLoader::histogram(unsigned int i,unsigned int j) const
{
  vector<unsigned int> indices;
  indices.push_back(i);
  indices.push_back(j);
  return histogram(indices);
}


//______________________________________________________________________________
TH1F* HistogramLoader::histogram(unsigned int i,unsigned int j,unsigned int k) const
{
  vector<unsigned int> indices;
  indices.push_back(i);
  indices.push_back(j);
  indices.push_back(k);
  return histogram(indices);
}


//______________________________________________________________________________
TH1F* HistogramLoader::histogram(const std::vector<unsigned int>& indices) const
{
  if (indices.size()!=nhistograms_.size())
    throw std::runtime_error("HistogramLoader::histogram: indices.size()");
  for (unsigned int i=0;i<nhistograms_.size();i++)
    if (indices[i]>=nhistograms_[i])
      throw std::runtime_error("HistogramLoader::histogram: index out of range");
  unsigned int index(0);
  for (unsigned int i=0;i<indices.size();i++) index+=indices[i]*offset_[i];
  return histograms_[index];
}


//______________________________________________________________________________
TH1F* HistogramLoader::next_histogram(unsigned int& i) const
{
  vector<unsigned int> indices;
  TH1F* result=next_histogram(indices);
  i=indices.front();
  return result;
}


//______________________________________________________________________________
TH1F* HistogramLoader::next_histogram(unsigned int& i,unsigned int& j) const
{
  vector<unsigned int> indices;
  TH1F* result=next_histogram(indices);
  i=indices.front();
  j=indices.front();
  return result;
}


//______________________________________________________________________________
TH1F* HistogramLoader::next_histogram(unsigned int& i,unsigned int& j,
				      unsigned int& k) const
{
  vector<unsigned int> indices;
  TH1F* result=next_histogram(indices);
  i=indices[0];
  j=indices[1];
  k=indices[2];
  return result;
}


//______________________________________________________________________________
TH1F* HistogramLoader::next_histogram(vector<unsigned int>& indices) const
{
  indices.clear();
  if (iloop_==histograms_.size()) return 0;
  for (unsigned int i=0;i<nhistograms_.size();i++) {
    indices.push_back(iloop_);
    for (unsigned int j=0;j<i;j++) indices[i]-=indices[j]*offset_[j];
    indices[i]/=offset_[i];
  }
  TH1F* result=histograms_[iloop_];
  iloop_++;
  return result;
}


//______________________________________________________________________________
double HistogramLoader::minimum(unsigned int ivar,unsigned int ihistogram) const
{
  return bounds_[ivar][ihistogram].first;
}


//______________________________________________________________________________
double HistogramLoader::maximum(unsigned int ivar,unsigned int ihistogram) const
{
  return bounds_[ivar][ihistogram].second;
}


//______________________________________________________________________________
void HistogramLoader::compute_offset()
{
  offset_.clear();
  for (unsigned int i=1;i<nhistograms_.size();i++) {
    unsigned int tmp=nhistograms_[i];
    for (unsigned int j=i+1;j<nhistograms_.size();j++) tmp*=nhistograms_[j];
    offset_.push_back(tmp);
  }
  offset_.push_back(1);
}


//______________________________________________________________________________
void HistogramLoader::set_histogram(const vector<unsigned int>& indices,TH1F* h)
{
  unsigned int index(0);
  for (unsigned int i=0;i<nhistograms_.size();i++) {
    if (indices[i]>=nhistograms_[i]) throw std::runtime_error("index out of range");
    index+=indices[i]*offset_[i];
  }
  vector<TH1F*>::iterator it=histograms_.begin();
  for (unsigned int i=0;i<index;i++) ++it;
  histograms_.insert(it,h);
}
  

//////////////////////////////////////////////////////////////////////////////////
// implementation of local functions
//////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
vector<string> split(const string& str,const string& delim)
{
  vector<string> result;
  string tmp=str;
  while (tmp.length()>0) {
    string::size_type pos=tmp.find(delim);
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

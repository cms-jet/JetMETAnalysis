////////////////////////////////////////////////////////////////////////////////
//
// CommandLine
// -----------
//
//            06/15/2008 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"


#include <cassert>
#include <fstream>
#include <iomanip>
#include <deque>


using namespace std;


////////////////////////////////////////////////////////////////////////////////
// construction / destruction
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
CommandLine::CommandLine()
{
  
}


//______________________________________________________________________________
CommandLine::~CommandLine()
{
  
}


////////////////////////////////////////////////////////////////////////////////
// implementation of member functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
bool CommandLine::parse(int argc,char**argv)
{
  _exe = argv[0];
  _options.clear();
  _ordered_options.clear();
  _unknowns.clear();
  
  bool do_parse_files(true);
  
  for (int i=1;i<argc;i++) {
    string opt=argv[i];
    if(0!=opt.find("-")) {
      if (do_parse_files) {
	bool success = parse_file(opt);
	if (!success) return false;
	continue;
      }
      else {
	cerr<<"CommandLine ERROR: options must start with '-'!"<<endl;
	return false;
      }
    }
    else {
      do_parse_files = false;
    }
    opt.erase(0,1);
    string next=argv[i+1];
    if (i+1>=argc) {
      cerr<<"ERROR: option '"<<opt<<"' requires value!"<<endl;
      return false;
    }
    
    if (opt.find('+')==opt.length()-1) {
      opt.erase(opt.length()-1);
      if (_options.find(opt)==_options.end()) {
	cerr<<"CommandLine ERROR: try to extend vector which is not defined!"<<endl;
	return false;
      }
      _options[opt].first += ":::"+next;
    }
    else{
      _options[opt] = make_pair(next,false);
    }
    
    i++;
    if (i<argc-1) {
      next=argv[i+1];
      while (next.find("-")!=0) {
	_options[opt].first += ":::"+next;
	i++;
	next = (i<argc-1) ? argv[i+1] : "-";
      }
    }
  }
  
  return true;
}
 

//______________________________________________________________________________
bool CommandLine::check()
{
  bool result = true;
  OptionMap_t::const_iterator it;
  for (it = _options.begin();it!=_options.end();++it) {
    if (!it->second.second) {
      cout<<"CommandLine WARNING: unused option '"<<it->first<<"'!"<<endl;
      result = false;
    }
  }
  
  if (_unknowns.size()>0) {
    result = false;
    cout<<"\nCommandLine WARNING: "
	<<"the following parameters *must* be provided:"<<endl;
    for (StrVec_t::const_iterator it=_unknowns.begin();it!=_unknowns.end();++it)
      cout<<(*it)<<endl;
    cout<<endl;
  }
  return result;
}

//______________________________________________________________________________
bool CommandLine::partialCheck()
{
  bool result = true;
  if (_unknowns.size()>0) {
    result = false;
    cout<<"\nCommandLine WARNING: "
	<<"the following parameters *must* be provided:"<<endl;
    for (StrVec_t::const_iterator it=_unknowns.begin();it!=_unknowns.end();++it)
      cout<<(*it)<<endl;
    cout<<endl;
  }
  return result;
}

//______________________________________________________________________________
void CommandLine::print()
{
  cout<<"------------------------------------------------------------"<<endl;
  cout<<_exe<<" options:"<<endl;
  cout<<"------------------------------------------------------------"<<endl;
  for (StrVec_t::const_iterator itvec=_ordered_options.begin();
       itvec!=_ordered_options.end();++itvec) {
    OptionMap_t::const_iterator it=_options.find(*itvec);
    assert(it!=_options.end());
    if (it->second.first.find(":::")<string::npos) {
      string tmp=it->second.first;
      size_t length = tmp.length();
      size_t pos;
      do {
	pos = tmp.find(":::");
	if (tmp.length()==length) {
	  cout<<setiosflags(ios::left)<<setw(22)
	      <<it->first
	      <<resetiosflags(ios::left)
	      <<setw(3)<<"="
	      <<setiosflags(ios::right)<<setw(35)
	      <<tmp.substr(0,pos)
	      <<resetiosflags(ios::right)
	      <<endl;
	}
	else {
	  cout<<setiosflags(ios::right)<<setw(60)
	      <<tmp.substr(0,pos)
	      <<resetiosflags(ios::right)
	      <<endl;
	}
	tmp.erase(0,pos+3);
      }
      while (pos!=string::npos);
    }
    else {
      cout<<setiosflags(ios::left)<<setw(22)
	  <<it->first
	  <<resetiosflags(ios::left)
	  <<setw(3)<<"="
	  <<setiosflags(ios::right)<<setw(35)
	  <<it->second.first
	  <<resetiosflags(ios::right)
	  <<endl;
    }
  }
  cout<<"------------------------------------------------------------"<<endl;
}


//______________________________________________________________________________
bool CommandLine::parse_file(const string& file_name)
{
  ifstream fin(file_name.c_str());
  if (!fin.is_open()) {
    cout<<"Can't open configuration file "<<file_name<<endl;
    return false;
  }

  deque<string> tokens;
  tokens.push_back("");
  bool filter(false);
  bool instr(false);
  while (!fin.eof()) {
    char next;
    fin.get(next);
    if (!filter&&!instr&&next=='$') filter=true;
    if (next=='"') instr=(!instr);
    else if (!filter) {
      if ((next=='\n'||(!instr&&next==' '))&&!tokens.back().empty())
	tokens.push_back("");
      else if (instr||(next!=' '&&next!='\n'&&next!='\t'))
	tokens.back()=tokens.back()+next;
    }
    if (filter&&next=='\n') filter=false;
  }
  if (tokens.back().empty()) tokens.pop_back();
  
  string token, last_token, key;
  while (tokens.size()) {
    if (token!="") last_token = token;
    token = tokens.front(); tokens.pop_front();
    if (token=="include") {
      key="";
      string cfgfile = tokens.front(); tokens.pop_front();
      cout<<"CommandLine INFO: include configuration file: "<<cfgfile<<endl;
      bool success = parse_file(cfgfile);
      if (!success) return false;
    }
    else if (token=="=") {
      key = last_token;
      string value = tokens.front(); tokens.pop_front();
      if (key.find('+')==key.length()-1) {
	key.erase(key.length()-1);
	if (_options.find(key)==_options.end())_options[key]=make_pair(value,false);
	else _options[key].first += ":::"+value;
      }
      else {
	_options[key] = make_pair(value,false);
      }
    }
    else if (!key.empty()&&!token.empty()&&(tokens.size()==0||tokens.front()!="=")){
      _options[key].first += ":::"+token;
    }
  }
  
  return true;
}

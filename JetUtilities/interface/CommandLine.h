#ifndef COMMANDLINE_H
#define COMMANDLINE_H 1


#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>


//
// convert stringstream to element, allow std::string specialization
//
template <class T> void ssToVal(std::stringstream& ss,T& e){ss>>e;}
template <> void ssToVal(std::stringstream& ss,std::string& e){e=ss.str();}


class CommandLine
{
public:
  //
  // construction/destruction
  //
  CommandLine();
  ~CommandLine();
  
  //
  // member functions
  //
  bool parse(int argc,char**argv);
  bool check();
  bool partialCheck();
  void print();
  
  template <class T> T getValue(const std::string& name);
  template <class T> T getValue(const std::string& name, T default_value);

  template <class T> std::vector<T> getVector(const std::string& name);
  template <class T> std::vector<T> getVector(const std::string& name,
					      const std::string& default_as_string);
  
private:
  bool parse_file(const std::string& file_name);
  

private:
  //
  // internal typedefs
  //
  typedef std::map<std::string,std::pair<std::string,bool> > OptionMap_t;
  typedef std::vector<std::string>                           StrVec_t;
  

  //
  // member data
  //
  std::string _exe;
  OptionMap_t _options;
  StrVec_t    _ordered_options;
  StrVec_t    _unknowns;

};

//
// implemenentation of inline functions
//

//______________________________________________________________________________
template <class T>
T CommandLine::getValue(const std::string& name)
{
  T result;
  OptionMap_t::iterator it=_options.find(name);
  if (it!=_options.end()) {
    it->second.second = true;
    _ordered_options.push_back(name);
    std::stringstream ss;
    ss<<it->second.first;
    ssToVal(ss,result);
    return result;
  }
  _unknowns.push_back(name);
  return result;
}


//______________________________________________________________________________
template <class T>
T CommandLine::getValue(const std::string& name,T default_value)
{
  OptionMap_t::const_iterator it=_options.find(name);
  if (it!=_options.end()) return getValue<T>(name);
  std::string default_as_string;
  std::stringstream ss;
  ss<<default_value;
  default_as_string = ss.str();
  _options[name] = std::make_pair(default_as_string,true);
  _ordered_options.push_back(name);
  return default_value;
}


//______________________________________________________________________________
template <>
bool CommandLine::getValue<bool>(const std::string& name)
{
  OptionMap_t::iterator it=_options.find(name);
  if (it!=_options.end()) {
    it->second.second = true;
    _ordered_options.push_back(name);
    std::string val_as_string = it->second.first;
    if (val_as_string=="true") return true;
    if (val_as_string=="false") return false;
    int val_as_int;
    std::stringstream ss;
    ss<<val_as_string;
    ss>>val_as_int;
    return val_as_int;
  }
  _unknowns.push_back(name);
  return false;
}


//______________________________________________________________________________
template <>
bool CommandLine::getValue(const std::string& name,bool default_value)
{
  OptionMap_t::const_iterator it=_options.find(name);
  if (it!=_options.end()) return getValue<bool>(name);
  _options[name] = (default_value) ?
    std::make_pair("true",true) : std::make_pair("false",true);
  _ordered_options.push_back(name);
  return default_value;
}


//______________________________________________________________________________
template <class T>
std::vector<T> CommandLine::getVector(const std::string& name)
{
  std::vector<T> result;
  OptionMap_t::iterator it=_options.find(name);
  if (it!=_options.end()) {
    it->second.second = true;
    _ordered_options.push_back(name);
    std::string tmp=it->second.first;
    size_t pos;
    if (!tmp.empty()) {
      do {
	pos = tmp.find(":::");
	std::stringstream ss;
	ss<<tmp.substr(0,pos);
	tmp.erase(0,pos+3);
	T element;
	ssToVal(ss,element);
	result.push_back(element);
      }
      while (pos!=std::string::npos);
    }
  }
  else {
    _unknowns.push_back(name);
  }
  return result;
}

//______________________________________________________________________________
template <class T>
std::vector<T> CommandLine::getVector(const std::string& name,
				       const std::string& default_as_string)
{
  OptionMap_t::iterator it=_options.find(name);
  if (it==_options.end()) _options[name] = make_pair(default_as_string,false);
  return getVector<T>(name);
}



#endif

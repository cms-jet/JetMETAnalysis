#ifndef JETINFO_HH
#define JETINFO_HH

#include "JetMETAnalysis/JetAnalyzers/interface/Settings.h"

#include "TString.h"
#include "TObjString.h"
#include "TObjArray.h"
#include "TRegexp.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <assert.h>
#include <stdexcept>
#include <cmath>

////////////////////////////////////////////////////////////////////////////////
// define class
////////////////////////////////////////////////////////////////////////////////

class JetInfo{
public:
  TString abbreviation;
  TString alias;
  TString title;
  TString algorithm;
  int coneSize;
  TString jetType;
  TString corrAbbreviations;
public:
  JetInfo() : abbreviation(""), alias(""), title(""), algorithm(""), coneSize(0), jetType("") {}
  JetInfo(TString abbreviation_, TString alias_, TString title_, TString algorithm_, int coneSize_, TString jetType_) : 
    abbreviation(abbreviation_), alias(alias_), title(title_), algorithm(algorithm_), coneSize(coneSize_), jetType(jetType_) {}
  JetInfo(TString abbreviation_) : abbreviation(abbreviation_) {
    alias = getAlias(abbreviation);
    title = TString(get_legend_title(std::string(abbreviation)));
    algorithm = getAlgorithm(abbreviation);
    coneSize = getConeSize(abbreviation).Atoi();
    jetType = getJetType(abbreviation);
    corrAbbreviations = getCorrString(abbreviation);
  }
  ~JetInfo(){;}

  /// find the index to a specific T in a vector of T
  static int vfind(const TString a[], const int size, TString b);
  static int vfind(std::vector<TString> a, TString b);
  static int vfind(std::vector<double> a, double b);
  static int vfind(const double a[], const int size, double b);

  /// check if a vector of strings contains a certain element
  static bool contains(const std::vector<std::string>& collection,const std::string& element);
  static bool contains(const std::vector<TString>& collection,const TString& element);

  /// check if a vector of TStrings contains any strings that also contain TString b
  /// this does not need to be an exact match
  static bool contains_loose(const std::vector<TString>& collection, const TString& element);

  /// get the bin number for a specific ptgen according to the vector of bin edges 
  static int getBin(double x, const double boundaries[], int length);

  /// get the jet algorithm
  TString getAlgorithm(TString s);
  TString getAlgorithm() {return getAlgorithm(abbreviation);}

  ///Check is a jet algorithm exists in Settings. This is basically an specific use case for vfind+getAlgorithm
  static TString checkAlgorithm(TString s);

  /// get the jet cone size i.e. 5, 7
  TString getConeSize(TString s);
  TString getConeSize() {return getConeSize(abbreviation);}
  
  /// get the jet type i.e. Calo, PF, PFchs, JPT, PUPPI
  TString getJetType(TString s);
  TString getJetType() {return getJetType(abbreviation);}

  /// return true if the jet type if an HLT jet
  bool isHLT();
  
  /// get the correction string i.e. l1l2l3
  TString getCorrString(TString s);
  TString getCorrString() {return getCorrString(abbreviation);}
  
  /// get the uppercase version of the algorithm name
  TString getAlias(TString s);
  TString getAlias() {return getAlias(abbreviation);}

  /// return the abbreviation as a std::string
  std::string getAbbreviation() {return std::string(abbreviation);}

  /// returns a string containing all of the correction levels to be applied
  static std::string get_correction_levels(const std::vector<int>& levels, bool L1FastJet);

  /// returns the full path of each correction level concatinated into a single string
  static std::string get_correction_tags(const std::string& tag, const std::string& alg,
                                         const std::vector<int>& levels, const std::string& jecpath, bool L1FastJet);

  /// return the full name of the correction level
  static std::string get_level_tag(int level, bool L1FastJet);

  /// transform the alg label into a title, e.g.: kt4calo -> k_{T}, D=0.4 (Calo)
  static std::string get_legend_title(const std::string& alg, bool withSize = true, bool withAlg = true, bool parentheses = false);
  static TString get_legend_title(const TString& alg, bool withSize = true, bool withAlg = true, bool parentheses = false);

  /// Get the abbreviation for each detector section
  static TString get_detector_abbreviation(TString dn);

  /// get the index corresponding to the correct NPV, Rho, or NPU bin
  static int getBinIndex(int variable, int nbins = 6, int binWidth = 5);
  static int getBinIndex(float x, const double binsx[], const int size);
  static int getBinIndex(float x, const std::vector<float>& binsx);

  /// get the properly formatted legend entry for a specific rho, npv, or npu bin
  static TString getBinLegendEntry(int bin, TString type, int nbins = 6, int binWidth = 5);
  static TString getBinLegendEntry(TString type, int lowEdge, int highEdge = -1);

  /// get the index corresponding to the correct detector region
  static int getDetIndex(double eta);

  /// get the eta region associated with a given detector region name
  static TString getDetRegion(TString name);

  /// get the index corresponding to the correct PDGID bin
  static std::vector<int> getPDGIDIndecies(int pdgid);

  /// get a string based on a list of other objects and a delimiter
  /// use like:
  //string s = ListToString(vec_of_strings,"_","ak4pf_ak4pfchs")
  //or
  //TString s = ListToString(vec_of_tstrings,"_","ak4pf_ak4pfchs")
  static std::string ListToString ( const std::vector<std::string> &list, std::string delimiter = "_" );
  static TString     ListToString ( const std::vector<TString> &list,     TString delimiter = "_" );
  
  /// use like:
  //char buffer [50];
  //sprintf (buffer, "%d", StringToNumber(alg,-1));
  //result += string(buffer);
  template <typename T>
  static T StringToNumber ( const std::string &Text, T defValue = T() );

  //Excerpt from KParser https://github.com/kpedro88/Analysis/blob/master/KCode/KParser.h#L26-L33
  //Parses and tokenizes a std::string
  //generalization for processing a line
  static void process(std::string line, char delim, std::vector<std::string>& fields){
     std::stringstream ss(line);
     std::string field;
     while(getline(ss,field,delim)){
        fields.push_back(field);
     }
  }
   
  bool operator<(const JetInfo& rhs) const {
    if (algorithm.CompareTo(rhs.algorithm) < 0) return true;
    if (jetType.CompareTo(rhs.jetType) < 0) return true;
    if (coneSize < rhs.coneSize) return true;
    else return false;
  }
   /*  
   bool operator()(JetInfo const& a, JetInfo const& b) {
      if (a.algorithm.CompareTo(b.algorithm) < 0) return true;
      if (a.jetType.CompareTo(b.jetType) < 0) return true;
      if (a.coneSize < b.coneSize) return true;
      else return false;
   }
   */
};

#endif

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

  /// find the index to a specific TString in a vector of TStrings
  static int vfind(const TString a[], const int size, TString b);
  static int vfind(std::vector<TString> a, TString b);


  /// get the jet algorithm
  TString getAlgorithm(TString s);

  /// get the jet cone size i.e. 5, 7
  TString getConeSize(TString s);
  
  /// get the jet type i.e. Calo, PF, PFchs, JPT
  TString getJetType(TString s);
  
  /// get the correction string i.e. l1l2l3
  TString getCorrString(TString s);
  
  /// get the uppercase version of the algorithm name
  TString getAlias(TString s);

  /// transform the alg label into a title, e.g.: kt4calo -> k_{T}, D=0.4 (Calo)
  static std::string get_legend_title(const std::string& alg, bool withSize = true);
  
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

// This class libraries
#include "JetMETAnalysis/JetUtilities/interface/JetInfo.hh"

using namespace std;

//______________________________________________________________________________
string JetInfo::get_legend_title(const string& alg, bool withSize, bool withAlg, bool parentheses)
{
  string title;
  string tmp(alg);
  if      (alg.find("kt")==0) { title = "k_{T} R=";      tmp = tmp.substr(2); }
  else if (alg.find("sc")==0) { title = "SISCone R=";    tmp = tmp.substr(2); }
  else if (alg.find("ic")==0) { title = "ItCone R=";     tmp = tmp.substr(2); }
  else if (alg.find("mc")==0) { title = "MidCone R=";    tmp = tmp.substr(2); }
  else if (alg.find("ca")==0) { title = "Cam/Aachen R="; tmp = tmp.substr(2); }
  else if (alg.find("ak")==0) { title = "Anti-k_{T} R="; tmp = tmp.substr(2); }
  else return alg;
  
  string reco[11] = { "gen", "caloHLT", "calo", "pfHLT", "pfchsHLT", "pfpuppi", "pfchs", "pf", "trk", "jpt", "puppi" };
  string RECO[11] = { "Gen", "Calo@HLT", "Calo", "PF@HLT", "PF+CHS@HLT", "PF+PUPPI", "PF+CHS", "PF", "Tracks", "JPT", "PF+PUPPI" };

  string::size_type pos=string::npos; int ireco=-1;
  while (pos==string::npos&&ireco<11) { pos = tmp.find(reco[++ireco]); }
  if (pos==string::npos) return alg;
  
  double jet_size; stringstream ss1; ss1<<tmp.substr(0,pos); ss1>>jet_size;
  jet_size/=10.0;  stringstream ss2; ss2<<jet_size;

  if (withSize) {
     title += ss2.str();
  }
  else {
     title = title.substr(0,title.size()-4);
  }

  if (withAlg) {
     if (withSize)
        title += ", ";
     else
        title += " ";

     if (parentheses)
        title += "(" + RECO[ireco] + ")";
     else
        title += RECO[ireco];
  }
  else {
     return title;
  }

  return title;
}

//______________________________________________________________________________
TString JetInfo::get_legend_title(const TString& alg, bool withSize, bool withAlg, bool parentheses) {
  return TString(JetInfo::get_legend_title(string(alg),withSize,withAlg,parentheses));
}

//______________________________________________________________________________
int JetInfo::vfind(const TString a[], const int size, TString b) {
  for (int i=0; i<size; i++) {
    if (a[i].CompareTo(b,TString::kIgnoreCase)==0)
      return i;
  }
  return -1;
}

//______________________________________________________________________________
int JetInfo::vfind(vector<TString> a, TString b) {
   for (unsigned int i=0; i<a.size(); i++) {
      if (a[i].CompareTo(b,TString::kIgnoreCase)==0)
         return i;
   }
   return -1;
}

//______________________________________________________________________________
int JetInfo::vfind(vector<double> a, double b) {
   for (unsigned int i=0; i<a.size(); i++) {
      if (a[i] == b)
         return i;
   }
   return -1;
}

//______________________________________________________________________________
int JetInfo::vfind(const double a[], const int size, double b) {
  for (int i=0; i<size; i++) {
    if (a[i] == b)
      return i;
  }
  return -1;
}

//______________________________________________________________________________
bool JetInfo::contains(const vector<std::string>& collection,const std::string& element)
{
  vector<std::string>::const_iterator it;
  for (it=collection.begin();it!=collection.end();++it)
    if ((*it)==element) return true;
  return false;
}

//______________________________________________________________________________
bool JetInfo::contains(const vector<TString>& collection,const TString& element)
{
  vector<TString>::const_iterator it;
  for (it=collection.begin();it!=collection.end();++it)
    if ((*it)==element) return true;
  return false;
}

//______________________________________________________________________________
bool JetInfo::contains_loose(const vector<TString>& collection, const TString& element) {
  vector<TString>::const_iterator it;
  for (it=collection.begin();it!=collection.end();++it)
    if ((*it).Contains(element)) return true;
  return false;
}

//______________________________________________________________________________
int JetInfo::getBin(double x, const double boundaries[], int length) {
   int i;
   int n = length;
   if (n<=0) return -1;
   if (x<boundaries[0] || x>=boundaries[n])
      return -1;
   for(i=0;i<n;i++)
   {
      if (x>=boundaries[i] && x<boundaries[i+1])
         return i;
   }
   return 0; 
}

//______________________________________________________________________________
TString JetInfo::getAlgorithm(TString s) {
  TRegexp e1("^[a-zA-Z]+"); //gets algorithm
  TString alg = s(e1);

  int ialg = vfind(algorithms,NAlgorithms,alg);
  if(ialg>-1) {
    return algorithms[ialg];
  }
  else {
    cout << "ERROR::getAlgorithm Could not find the algorithm that corresponds to " << alg << "." << endl;
    assert(ialg>-1);
  }

  return "";
}

//______________________________________________________________________________
TString JetInfo::checkAlgorithm(TString s) {
  TRegexp e1("^[a-zA-Z]+"); //gets algorithm
  TString alg = s(e1);

  int ialg = vfind(algorithms,NAlgorithms,alg);
  if(ialg>-1) {
    return algorithms[ialg];
  }
  else {
    cout << "ERROR::checkAlgorithm Could not find the algorithm that corresponds to " << alg << "." << endl;
    return "unknown";
  }
}

//______________________________________________________________________________
TString JetInfo::getConeSize(TString s) {
  TRegexp e2("[0-9][0-9]?"); //gets cone size
  TString sizeString = s(e2);
  int size = sizeString.Atoi();
  std::ostringstream ss;
  ss.precision(1);
  ss << fixed << (size/10.0);
  int isize = vfind(cone_sizes,NConeSizes,TString(ss.str()));
  if(isize>-1) {
    return sizeString;
  }
  else {
    cout << "ERROR::getConeSize Could not find the cone size that corresponds to " << sizeString << "." << endl;
    assert(isize>-1);
  }

  return "";
}

//______________________________________________________________________________
TString JetInfo::getJetType(TString s) {
  TRegexp e2("[0-9][0-9]?"); //gets cone size
  TRegexp e3("l[0-9]"); //to get first l[0-9]

  int posSizeEnd = s(e2).Start()+s(e2).Length();
  int posFirstCorr = 0;
  if(s(e3).Length()>0) {
    posFirstCorr = s(e3).Start();
  }
  else {
    posFirstCorr = s.Length();
  }
  TString jtype = s(posSizeEnd,posFirstCorr-posSizeEnd);

  int itype = vfind(jet_types,NJetTypes,jtype);
  if(itype>-1) {
    return jet_types[itype];
  }
  else {
    cout << "ERROR::getJetType Could not find the jet type that corresponds to " << jtype << "." << endl;
    assert(itype>-1);
  }

  return "";
}

//______________________________________________________________________________
bool JetInfo::isHLT() {
  if(getJetType(abbreviation).Contains("hlt",TString::kIgnoreCase))
    return true;
  return false;
}

//______________________________________________________________________________
TString JetInfo::getCorrString(TString s) {
  TRegexp e3("l[0-9]"); //to get first l[0-9]

  int posFirstCorr = 0;
  if(s(e3).Length()>0) {
    posFirstCorr = s(e3).Start();
  }
  else {
    posFirstCorr = s.Length();
  }
  TString corrString = s(posFirstCorr,100);
  TObjArray* corrArray = corrString.Tokenize("l");
  vector<int> corrLevels;
  for(int l = 0; l<corrArray->GetEntries(); l++) {
    corrLevels.push_back(((TObjString*)(*corrArray)[l])->String().Atoi());
  }
  corrArray->Delete();
  delete corrArray;
  return corrString;
}

//______________________________________________________________________________
TString JetInfo::getAlias(TString s)
{
  TString res;
  res += getAlgorithm(s);
  res += getConeSize(s);
  res += getJetType(s);
  res += getCorrString(s);
  return res;
}

//______________________________________________________________________________
string JetInfo::get_correction_levels(const vector<int>& levels, bool L1FastJet)
{
  stringstream ssresult;
  for (unsigned int ilevel=0;ilevel<levels.size();++ilevel) {
    if (ilevel!=0) ssresult<<":";
    int level(levels[ilevel]);
    switch (level) {
    case 1 : 
       if (L1FastJet) {
          ssresult<<"L1FastJet"; break;
       }
       else {
          ssresult<<"L1Offset"; break;
       }
    case 2 : ssresult<<"L2Relative"; break;
    case 3 : ssresult<<"L3Absolute"; break;
    case 4 : ssresult<<"L4EMF"; break;
    case 5 : ssresult<<"L5Flavor"; break;
    case 6 : ssresult<<"L6SLB"; break;
    case 7 : ssresult<<"L7Parton"; break;
    default: throw std::runtime_error(((string)"get_correction_levels ERROR: "+
               (string)"invalid correction level").c_str());
    }
  }
    return ssresult.str();
}

//______________________________________________________________________________
string JetInfo::get_correction_tags(const string& era,const string& alg,
                                    const vector<int>& levels,
                                    const string& jecpath, bool L1FastJet)
{
  stringstream ssresult;
  for (unsigned int ilevel=0;ilevel<levels.size();ilevel++) {
    
    if (ilevel!=0) ssresult<<":";
    
    int level=levels[ilevel];
    stringstream ssera;
    
    if(jecpath[jecpath.length()-1] != '/') ssera<<jecpath<<"/"<<era<<"_";
    else ssera<<jecpath<<era<<"_";

    if      (level==1 && !L1FastJet) ssera<<"L1Offset_";
    else if (level==1 && L1FastJet) ssera<<"L1FastJet_";
    else if (level==2) ssera<<"L2Relative_";
    else if (level==3) ssera<<"L3Absolute_";
    else if (level==4) ssera<<"L4EMF_";
    else if (level==5) ssera<<"L5Flavor_";
    else if (level==6) ssera<<"L6SLB_";
    else if (level==7) ssera<<"L7Parton_";
    else throw std::runtime_error("unknown correction level");
    
    if (level==6) {
      ssresult<<ssera.str()<<".txt";
      continue;
    }

    JetInfo tmp(alg);
    ssera<<tmp.getAlias();

    ssresult<<ssera.str()<<".txt";
  }
  
  return ssresult.str();
}

//______________________________________________________________________________
string JetInfo::get_level_tag(int level, bool L1FastJet)
{
    if      (level==1 && !L1FastJet) return "_L1Offset_";
    else if (level==1 && L1FastJet)  return "_L1FastJet_";
    else if (level==2)               return "_L2Relative_";
    else if (level==3)               return "_L3Absolute_";
    else if (level==4)               return "_L4EMF_";
    else if (level==5)               return "_L5Flavor_";
    else if (level==6)               return "_L6SLB_";
    else if (level==7)               return "_L7Parton_";
    else if (level==23)              return "_L2L3Residual_";
    else throw std::runtime_error("unknown correction level");
}

//______________________________________________________________________________
TString JetInfo::get_detector_abbreviation(TString dn) {
  TObjArray* words = dn.Tokenize(" ");
  if(words->GetEntries()==1) {
    TString ret = dn(0,1);
    ret.ToUpper();
    ret+=ret;
    words->Delete();
    delete words;
    return ret;
  }
  else if(words->GetEntries()>1) {
    TString ret;
    for(int w=0; w<words->GetEntries(); w++) {
      ret = (((TObjString*)((*words)[w]))->String())(0,1) + ret;
    }
    ret.ToUpper();
    words->Delete();
    delete words;
    return ret;
  }
  else {
    cout << "ERROR::get_detector_abbreviation The number of words in the detector name is less than 1." << endl;
    assert(words>0);
  }
  words->Delete();
  delete words;
  return "";
}

//______________________________________________________________________________
int JetInfo::getBinIndex(int variable, int nbins, int binWidth){
   if (variable<0)
      return 0;
   if (variable>(nbins*binWidth)-1)
      return nbins-1;

   return int (variable / binWidth);
}//getBinIndex

//______________________________________________________________________________
int JetInfo::getBinIndex(float x, const double binsx[], const int size) {
   for (int ix=0;ix<size-1;ix++)
      if (x>=binsx[ix]&&x<binsx[ix+1]) return ix;
   return -1;
}//getBinIndex

//______________________________________________________________________________
int JetInfo::getBinIndex(float x,const vector<float>& binsx) {
   for (unsigned int ix=0;ix<binsx.size()-1;ix++)
      if (x>=binsx[ix]&&x<binsx[ix+1]) return ix;
   return -1;
}//getBinIndex

//______________________________________________________________________________
TString JetInfo::getBinLegendEntry(int bin, TString type, int nbins, int binWidth) {
  return Form("%i #leq " + type + " < %i",bin*binWidth, bin*binWidth+binWidth);
}//getBinLegendEntry

//______________________________________________________________________________
TString JetInfo::getBinLegendEntry(TString type, int lowEdge, int highEdge) {
  if(highEdge>-1)
    return Form("%i #leq " + type + " < %i",lowEdge, highEdge);
  else
    return Form("%i #leq " + type,lowEdge);
}//getBinLegendEntry

//______________________________________________________________________________
int JetInfo::getDetIndex(double eta){
   eta = fabs(eta);

   if (eta<1.3)
      return 0;
   else if (eta<2.5)
      return 1;
   else if (eta<3)
      return 2;

   return 3;

}//getDetIndex

//______________________________________________________________________________
TString JetInfo::getDetRegion(TString name) {
  int iRegion = vfind(detector_names,NDetectorNames,name);
  if(iRegion>-1) return detector_regions_eta[iRegion];
  else return "";
}

//______________________________________________________________________________
vector<int> JetInfo::getPDGIDIndecies(int pdgid) {
   pdgid = fabs(pdgid);
   vector<int> res;

   if(pdgid==0) res.push_back(0);
   if(pdgid>=1 && pdgid<=3) res.push_back(1);
   if(pdgid==4) res.push_back(2);
   if(pdgid==5) res.push_back(3);
   if(pdgid==21) res.push_back(4);
   res.push_back(5);
   if(pdgid>=1 && pdgid<=6) res.push_back(6);

   return res;
}//getPDGIDIndecies

//______________________________________________________________________________
string JetInfo::ListToString ( const std::vector<string> &list, string delimiter )
{
   string result;
   for(unsigned int i=0; i<list.size(); i++) {
      if(i<list.size()-1)
         result += list[i] + string(delimiter);
      else
         result += list[i];
   }
   return result;
}

//______________________________________________________________________________
TString JetInfo::ListToString ( const std::vector<TString> &list, TString delimiter )
{
   TString result;
   for(unsigned int i=0; i<list.size(); i++) {
      if(i<list.size()-1)
         result += list[i] + TString(delimiter);
      else
         result += list[i];
   }
   return result;
}

//______________________________________________________________________________
template <typename T>
T JetInfo::StringToNumber ( const string &Text, T defValue )
{
   stringstream ss;
   for ( string::const_iterator i=Text.begin(); i!=Text.end(); ++i ) {
      if (*i=='l')
         break;
      if ( isdigit(*i) || *i=='e' || *i=='-' || *i=='+' || *i=='.' )
         ss << *i;
   }
   T result;
   return ss >> result ? result : defValue;
}

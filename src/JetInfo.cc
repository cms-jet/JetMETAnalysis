// This class libraries
#include "JetMETAnalysis/JetUtilities/interface/JetInfo.hh"

using namespace std;

//______________________________________________________________________________
string JetInfo::get_legend_title(const string& alg, bool withSize)
{
  string title;
  string tmp(alg);
  if      (alg.find("kt")==0) { title = "k_{T}, R=";      tmp = tmp.substr(2); }
  else if (alg.find("sc")==0) { title = "SISCone, R=";    tmp = tmp.substr(2); }
  else if (alg.find("ic")==0) { title = "ItCone, R=";     tmp = tmp.substr(2); }
  else if (alg.find("mc")==0) { title = "MidCone. R=";    tmp = tmp.substr(2); }
  else if (alg.find("ca")==0) { title = "Cam/Aachen, R="; tmp = tmp.substr(2); }
  else if (alg.find("ak")==0) { title = "Anti k_{T}, R="; tmp = tmp.substr(2); }
  else return alg;
  
  string reco[9] = { "gen", "caloHLT", "calo", "pfHLT", "pfchsHLT", "pfchs", "pf", "trk", "jpt" };
  string RECO[9] = { "(Gen)", "(Calo@HLT)", "(Calo)", "(PFlow@HLT)", "(PFlow+CHS@HLT)", "(PFlow+CHS)", "(PFlow)", "(Tracks)", "(JPT)" };

  string::size_type pos=string::npos; int ireco=-1;
  while (pos==string::npos&&ireco<8) { pos = tmp.find(reco[++ireco]); }
  if (pos==string::npos) return alg;
  
  double jet_size; stringstream ss1; ss1<<tmp.substr(0,pos); ss1>>jet_size;
  jet_size/=10.0;  stringstream ss2; ss2<<jet_size;

  if (withSize)
    title += ss2.str() + " " + RECO[ireco];
  else {
    title = title.substr(0,title.size()-4);
    title += " " + RECO[ireco];
  }

  return title;
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
    cout << "ERROR::getAlias Could not find the cone size that corresponds to " << sizeString << "." << endl;
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
    cout << "ERROR::getAlias Could not find the jet type that corresponds to " << jtype << "." << endl;
    assert(itype>-1);
  }

  return "";
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

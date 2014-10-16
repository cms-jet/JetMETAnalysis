#ifndef EVTID_HH
#define EVTID_HH

// C++ Libraries
#include <map>
#include <utility>

using std::map;
using std::pair;

////////////////////////////////////////////////////////////////////////////////
// define class
////////////////////////////////////////////////////////////////////////////////

class evtid {
public:
   Long64_t run_, ls_, evt_;
   Float_t refpt0_;
   bool runDep_;
public:
   evtid() : run_(0), ls_(0), evt_(0), refpt0_(0), runDep_(false) {}
   evtid(Long64_t run, Long64_t ls, Long64_t evt, Float_t refpt0, bool runDep) : 
         run_(run), ls_(ls), evt_(evt), refpt0_(refpt0), runDep_(runDep) {}
   
   bool operator()(evtid const& a, evtid const& b) {
      if (!a.runDep_ && !b.runDep_) {
         if (a.run_ < b.run_) return true;
      }
      if (a.ls_ < b.ls_) return true;
      if (a.evt_ < b.evt_) return true;
      //if (a.refpt0_ < b.refpt0_) return true;
      else return false;
   }
};

// To make notation clearer
typedef map<evtid, pair<Long64_t, Long64_t> > IT;

#endif
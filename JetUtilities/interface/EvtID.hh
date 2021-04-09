#ifndef EVTID_HH
#define EVTID_HH

// ROOT Libraries
#include "TObject.h"

// C++ Libraries
#include <map>
#include <utility>

using std::map;
using std::pair;

typedef unsigned long long EventNumber_t;
typedef unsigned int LuminosityBlockNumber_t;
typedef unsigned int RunNumber_t;

////////////////////////////////////////////////////////////////////////////////
// define class
////////////////////////////////////////////////////////////////////////////////

class evtid : public TObject {
private:
   RunNumber_t run_;
   LuminosityBlockNumber_t ls_;
   EventNumber_t evt_;
   Float_t refpt0_;
public:
   evtid() : run_(0U), ls_(0U), evt_(0U), refpt0_(0) {}
   evtid(RunNumber_t run, LuminosityBlockNumber_t ls, EventNumber_t evt, Float_t refpt0) : 
         run_(run), ls_(ls), evt_(evt), refpt0_(refpt0) {}
   
   // ---------- const member functions ---------------------
   RunNumber_t run() const { return run_; }
   LuminosityBlockNumber_t luminosityBlock() const { return ls_; }
   EventNumber_t event() const { return evt_; }
   Float_t pt() const { return refpt0_; }

   void print() {
      cout << "EvtID:" << endl
           << "\trun: " << run_ << endl
           << "\tluminosityBlock: " << ls_ << endl
           << "\tevent: " << evt_ << endl
           << "\tRefPt0: " << refpt0_ << endl;
   }
   void setEvtid(RunNumber_t r, LuminosityBlockNumber_t ls, EventNumber_t e, Float_t pt) {
      run_ = r;
      ls_ = ls;
      evt_ = e;
      refpt0_ = pt;
   }
   void setEvent(EventNumber_t e) { evt_ = e; }
   void setLuminosityBlock(LuminosityBlockNumber_t ls) { ls_ = ls; }
   void setRefPt(Float_t pt) { refpt0_ = pt; }
   void setRun(RunNumber_t r) { run_ = r; }

   bool operator()(evtid const& a, evtid const& b) const {
      if (a.run_ < b.run_) return true;
      if (a.run_ > b.run_) return false;
      if (a.ls_ < b.ls_) return true;
      if (a.ls_ > b.ls_) return false;
      return (a.evt_ < b.evt_);
      //if (a.refpt0_ < b.refpt0_) return true;
   }

   ClassDef(evtid,2)
};

// To make notation clearer
typedef unsigned long long ull;
typedef map<evtid, pair<ull,ull> > IT;
typedef map<evtid, pair<ull,ull>, evtid > ITS;

#endif

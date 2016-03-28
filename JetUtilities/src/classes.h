#include "JetMETAnalysis/JetUtilities/interface/JRAEvent.h"
#include "JetMETAnalysis/JetUtilities/interface/TProfileMDF.h"
//#include "JetMETAnalysis/JetUtilities/interface/JRANtuple.h"
#include "JetMETAnalysis/JetUtilities/interface/PUNtuple.hh"
#include "JetMETAnalysis/JetUtilities/interface/JetInfo.hh"
#include "JetMETAnalysis/JetUtilities/interface/EvtID.hh"

#include "DataFormats/Common/interface/Wrapper.h"

#include <vector>

namespace { 
   struct dictionary {
      //JRAEvent jraeventdummy0;
      edm::Wrapper<JRAEvent> dummy1;
      std::vector<JRAEvent> dummy2;
      edm::Wrapper<std::vector<JRAEvent> > dummy3;
   };
}

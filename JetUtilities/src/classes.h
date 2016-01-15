#include "JetMETAnalysis/JetUtilities/interface/JRAEvent.h"
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

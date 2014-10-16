#ifndef JetUtilities_JRAEvent_h
#define JetUtilities_JRAEvent_h

#include <vector>


// a simple class
struct JRAEvent {
   explicit JRAEvent(int i, float f, long long l):nref(i),rho(f),rho50(f),rho_hlt(f),pthat(f),weight(f),npv(l),run(l),lumi(l),evt(l) { }
   JRAEvent():nref(-1),rho(-1),rho50(-1),rho_hlt(-1),pthat(-1),weight(-1),npv(-1),run(-1),lumi(-1),evt(-1) { }
   int nref;
   float rho, rho50, rho_hlt, pthat, weight;
   long long npv, run, lumi, evt;
   std::vector<int> npus, bxns, ntrks_lowpt, ntrks_highpt, nrefrank, refpdgid;
   std::vector<float> tnpus, sumpt_lowpt, sumpt_highpt, refe, refpt, refeta, refphi, refy, refdrjt, refdphijt, refarea, jte, jtpt, jteta, jtphi, jty, jtjec, jtarea, jtemf, jtchf, jtnhf, jtnef, jtcef, jtmuf, jthfhf, jthfef;
};

// this is our new product, it is simply a 
// collection of SampleProd held in an std::vector
typedef std::vector<JRAEvent> JRAEventCollection;

#endif

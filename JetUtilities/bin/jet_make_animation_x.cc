////////////////////////////////////////////////////////////////////////////////
//
// jet_make_animation_x
// --------------------
//
//                      07/28/2016 Alexx Perloff <alexx.stephen.perloff@cern.ch>
////////////////////////////////////////////////////////////////////////////////
// Adapted from https://root.cern.ch/doc/master/classTASImage.html#a9f020509ac01d8ecbc3564e30506932b

#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"

#include "TSystem.h"
#include "TImage.h"

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
int main(int argc,char** argv) {
    CommandLine cl;
    if (!cl.parse(argc,argv)) return 0;

    vector<string> inputs     = cl.getVector<string>("inputs");
    string         outputDir  = cl.getValue<string> ("outputDir",             "./");
    string         outputName = cl.getValue<string> ("outputName", "animation.gif");
    bool           loop       = cl.getValue<bool>   ("loop",                  true);
    int            delay      = cl.getValue<int>    ("delay",                    0); //10*NN milliseconds delay
    int            lastdelay  = cl.getValue<int>    ("lastdelay",                0); //10*NN milliseconds delay
    int            repeats    = cl.getValue<int>    ("repeats",                 -1);

    if (!cl.check()) return 0;
    cl.print();

    if(outputDir.empty())     outputDir = string(gSystem->pwd())+"/";
    if(outputDir.back()!='/') outputDir+="/";

    string output = outputDir+outputName;

    TImage *img = 0;
    gSystem->Unlink(output.c_str());  // delete existing file

    for(unsigned int iimage=0; iimage<inputs.size(); iimage++) {
        delete img; // delete previous image
        // Read image data. Image can be in any format, e.g. png, gif, etc.
        img = TImage::Open(inputs[iimage].c_str());

        string animationCmd; stringstream ss;
        if(loop && iimage==inputs.size()-1) {
            ss.str(""); (repeats>-1) ? ss << "+" : ss << "++"; ss << lastdelay;
            animationCmd += (lastdelay>0 || (lastdelay==0 && repeats>-1)) ? ss.str()+"++" : "++";
            ss.str(""); ss << repeats;
            animationCmd += (repeats>-1) ? ss.str() : "";
        }
        else {
            ss.str(""); ss << "+" << delay;
            animationCmd += (delay>0) ? ss.str() : "+";
        }
    
        img->WriteImage((output+animationCmd).c_str());
   }

   return 0;
}


#include <iostream>
#include <iomanip>
#include <string>

using std::cout;
using std::endl;
using std::flush;
using std::setw;
using std::string;

inline void loadbar2(unsigned int x, unsigned int n, unsigned int w = 50, string prefix = "") {
   if ( (x != n) && (x % (n/100) != 0) ) return;

   float ratio  =  x/(float)n;
   int   c      =  ratio * w;

   cout << prefix << setw(3) << (int)(ratio*100) << "% [";
   for (int x=0; x<c; x++) cout << "=";
   for (unsigned int x=c; x<w; x++) cout << " ";
   cout << "] (" << x << "/" << n << ")\r" << flush;
}

#ifndef VARIABLES_HH
#define VARIABLES_HH

//ROOT libraries
#include "TROOT.h"
#include "TSystem.h"
#include "TString.h"

//C++ libraries
#include <iostream>
#include <string>
#include <map>
#include <vector>

namespace VARIABLES {

   enum Variable {none, refpt, ptclpt, refeta, jtpt, jteta, mu, npv, npu, rho, area, other, unknown};
	static const unsigned int nVariable = 12;


	// A routine that returns a string given the VARIABLE
	std::string getVariableString(Variable var);
	
	// A routine that returns a string fit for a title
	std::string getVariableTitleString(Variable var);

	// A routine that returns a string fit for an axis title
	std::string getVariableAxisTitleString(Variable var, bool unit=true);

	// A routine that returns a VARIABLE given a string
	Variable getVariable(std::string str);

}

#endif

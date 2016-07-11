#include "JetMETAnalysis/JetUtilities/interface/Variables.hh"

using std::cout;
using std::endl;
using std::string;

namespace VARIABLES {
	//---------------------------------------------------------------------------
	// A routine that returns a string given the VARIABLE
	std::string getVariableString(Variable var) {      
	    if (var == none)      	 return "none";
	    else if (var == refpt)	 return "refpt";
	    else if (var == ptclpt)	 return "refpt";
	    else if (var == refeta)	 return "refeta";
	    else if (var == jtpt)	 return "jtpt";
	    else if (var == jteta)   return "jteta";
	    else if (var == mu)   	 return "mu";
	    else if (var == npv)	 return "npv";
	    else if (var == npu)	 return "npu";
	    else if (var == rho)	 return "rho";
	    else if (var == area)    return "area";
	    else if (var == other)   return "other";
	    else if (var == unknown) return "unknown";
	
	    cout<<"ERROR getVariableString cannot find the given var"<<endl;
	    return "ERROR";
	}
	
	// A routine that returns a string fit for a title
	std::string getVariableTitleString(Variable var) {      
	    if (var == none)      	 return "None";
	    else if (var == refpt)	 return "RefPt";
	    else if (var == ptclpt)	 return "RefPt";
	    else if (var == refeta)	 return "RefEta";
	    else if (var == jtpt)	 return "JetPt";
	    else if (var == jteta)   return "JetEta";
	    else if (var == mu)   	 return "mu";
	    else if (var == npv)	 return "NPV";
	    else if (var == npu)	 return "NPU";
	    else if (var == rho)	 return "Rho";
	    else if (var == area)    return "Area";
	    else if (var == other)   return "Other";
	    else if (var == unknown) return "Unknown";
	
	    cout<<"ERROR getVariableString cannot find the given var"<<endl;
	    return "ERROR";
	}

	// A routine that returns a string fit for a title
	std::string getVariableAxisTitleString(Variable var, bool unit) { 
	    if (var == none)      	 return "None";
	    else if (var == refpt)	 return unit ? "p_{T}^{Ref} (GeV)" : "p_{T}^{Ref}"; //ref->ptcl
	    else if (var == ptclpt)	 return unit ? "p_{T}^{ptcl} (GeV)" : "p_{T}^{ptcl}"; //ref->ptcl
	    else if (var == refeta)	 return "#eta^{Ref}"; //ref->ptcl
	    else if (var == jtpt)	 return unit ? "p_{T}^{Reco} (GeV)" : "p_{T}^{Reco}";
	    else if (var == jteta)   return "#eta^{Reco}";
	    else if (var == mu)   	 return "#mu";
	    else if (var == npv)	 return "N_{PV}";
	    else if (var == npu)	 return "N_{PU}";
	    else if (var == rho)	 return "#rho";
	    else if (var == area)    return "Area";
	    else if (var == other)   return "Other";
	    else if (var == unknown) return "Unknown";
	
	    cout<<"ERROR getVariableString cannot find the given var"<<endl;
	    return "ERROR";
	}
	
	// A routine that returns a VARIABLE given a string
	VARIABLES::Variable getVariable(std::string str) {  
	    if (str == "none")    	   return none;
	    else if (str == "refpt")   return refpt;
	    else if (str == "ptclpt")  return ptclpt;
	    else if (str == "refeta")  return refeta;
	    else if (str == "jtpt")    return jtpt;
	    else if (str == "jteta")   return jteta;
	    else if (str == "mu") 	   return mu;
	    else if (str == "npv")     return npv;
	    else if (str == "npu")     return npu;
	    else if (str == "rho")     return rho;
	    else if (str == "area")	   return area;
	    else if (str == "other")   return other;
	    else if (str == "unknown") return unknown;
	
	    cout<<"ERROR  getVariable cannot find the given string"<<endl;
	    return unknown;
	}
}

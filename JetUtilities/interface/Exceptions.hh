////////////////////////////////////////////////////////////////////////////////
//
// Exceptions
// ----------
//
//            09/04/2015 Alexx Perloff           <alexx.stephen.perloff@cern.ch>
////////////////////////////////////////////////////////////////////////////////
#ifndef Exceptions_HH
#define Exceptions_HH

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <exception>

////////////////////////////////////////////////////////////////////////////////
// define mismatch_exception class
////////////////////////////////////////////////////////////////////////////////
class mismatch_exception: public std::exception {
public:
	mismatch_exception(unsigned int x, unsigned int y) : size1(x), size2(y) {}

	virtual const std::string what(std::string name1, std::string name2) const throw() {
		std::stringstream ss;
		ss << "Size Mismatch: " << name1 << " and " << name2 << " are not of equal size (" << size1 << "!=" << size2 << ")." << std::endl;
		return ss.str();
	}

	template<typename T>
  	static void areEqual(const T& n1, const T& n2) {
        if ( n1 != n2 ) {
            throw mismatch_exception( n1, n2 );
        } 
        return;
    }

private:
	unsigned int size1, size2;

};

//extern mismatch_exception mis_ex;

#endif
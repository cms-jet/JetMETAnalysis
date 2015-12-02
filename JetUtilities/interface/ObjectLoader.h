#ifndef OBJECTLOADER_H
#define OBJECTLOADER_H 1


#include <TDirectory.h>

#include <string>
#include <vector>


template <class T> class ObjectLoader
{
public:
  //
  // construction/destruction
  //
  ObjectLoader();
  ~ObjectLoader();

  
  //
  // member functions
  //
  bool         load_objects(TDirectory* d,const std::string& expression);
  void         reset();
  void         print();
  template <class S>
  unsigned int ndigits(S number);
  
  std::string  quantity()                   const { return quantity_; }
  unsigned int nvariables()                 const { return variables_.size(); }
  unsigned int nobjects()                   const { return objects_.size(); }
  std::string  variable(unsigned int ivar)  const {
    return variables_[variable_index_[ivar]];
  }
  unsigned int nobjects(unsigned int ivar)  const {
    return nobjects_[variable_index_[ivar]];
  }


  T*           object(unsigned int i)       const;
  T*           object(unsigned int i,
		      unsigned int j)       const;
  T*           object(unsigned int i,
		      unsigned int j,
		      unsigned int k)       const;
  T*           object(const std::vector<unsigned int>& indices) const;
  
  void         begin_loop()                 const { iloop_=0; }
  
  T*           next_object(unsigned int& i) const;
  T*           next_object(unsigned int& i,
			   unsigned int& j) const;
  T*           next_object(unsigned int& i,
			   unsigned int& j,
			   unsigned int& k) const;
  T*           next_object(std::vector<unsigned int>& indices) const;
  
  double       minimum(unsigned int ivar,unsigned int iobj) const;
  double       maximum(unsigned int ivar,unsigned int iobj) const;
  
private:
  void           compute_offset();
  void           print_offset();
  void           set_object(const std::vector<unsigned int>& indices,T* obj);
  std::vector<std::string> split(const std::string& str,const std::string& delim);
  
  
private:
  //
  // member data
  //
  std::string                             quantity_;
  std::vector<std::string>                variables_;
  std::vector<unsigned int>               variable_index_;
  std::vector<unsigned int>               nobjects_;
  std::vector<unsigned int>               offset_;
  std::vector<T*>                         objects_;
  
  std::vector< std::vector< std::pair<double,double> > > bounds_;

  mutable unsigned int                    iloop_;
};


#include "ObjectLoader.icc"


#endif

#ifndef HISTOGRAMLOADER_H
#define HISTOGRAMLOADER_H 1


#include <TH1F.h>
#include <TDirectory.h>

#include <string>
#include <vector>


class HistogramLoader
{
public:
  //
  // construction/destruction
  //
  HistogramLoader();
  ~HistogramLoader();

  
  //
  // member functions
  //
  bool         load_histograms(TDirectory* d,const std::string& expression);
  
  std::string  quantity()                      const { return quantity_; }
  unsigned int nvariables()                    const { return variables_.size(); }
  std::string  variable(unsigned int ivar)     const { return variables_[ivar]; }
  unsigned int nhistograms(unsigned int ivar)  const { return nhistograms_[ivar]; }
  unsigned int nhistograms()                   const { return histograms_.size(); }

  TH1F*        histogram(unsigned int i)       const;
  TH1F*        histogram(unsigned int i,
			 unsigned int j)       const;
  TH1F*        histogram(unsigned int i,
			 unsigned int j,
			 unsigned int k)       const;
  TH1F*        histogram(const std::vector<unsigned int>& indices) const;
  
  void         begin_loop()                    const { iloop_=0; }
  TH1F*        next_histogram(unsigned int& i) const;
  TH1F*        next_histogram(unsigned int& i,
			      unsigned int& j) const;
  TH1F*        next_histogram(unsigned int& i,
			      unsigned int& j,
			      unsigned int& k) const;
  TH1F*        next_histogram(std::vector<unsigned int>& indices) const;
  
  double       minimum(unsigned int ivar,unsigned int ihistogram) const;
  double       maximum(unsigned int ivar,unsigned int ihistogram) const;
  
private:
  void         compute_offset();
  void         set_histogram(const std::vector<unsigned int>& indices,TH1F* h);
  
private:
  //
  // member data
  //
  std::string                             quantity_;
  std::vector<std::string>                variables_;
  std::vector<unsigned int>               nhistograms_;
  std::vector<unsigned int>               offset_;
  std::vector<TH1F*>                      histograms_;
  
  std::vector< std::vector< std::pair<double,double> > > bounds_;

  mutable unsigned int                    iloop_;
};


#endif

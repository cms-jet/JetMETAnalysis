#ifndef VECTORWRAPPER2D_HH
#define VECTORWRAPPER2D_HH

#include "JetMETAnalysis/JetAnalyzers/interface/VectorWrapper.h"

#include "TROOT.h"
#include "TNamed.h"
#include "TFile.h"
#include "TList.h"
#include "TProfile3D.h"

#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>

class VectorWrapper2D: public VectorWrapper
{
public:
   void push_back(VectorWrapper vw){vec.push_back(vw);}
   void push_back(int index, TProfile3D* prof){vec[index].push_back(prof);}
   void reserve(int size){vec.reserve(size);}
   unsigned int size(){return vec.size();}
   VectorWrapper back(){return vec.back();}
   VectorWrapper operator[](unsigned int a){return vec[a];}
   void writeFile(TString filename) {
      for(unsigned int i=0; i<vec.size(); i++) {
         if (i == 0)
            vec[i].writeFile(filename,"RECREATE");
         else
            vec[i].writeFile(filename,"UPDATE");
      }
   }
   std::vector<std::string> split(const std::string& str,const std::string& delim)
   {
      std::vector<std::string> result;                             
      std::string tmp=str;                                    
      while (tmp.length()>0) {                           
         size_t pos=tmp.find(delim);                      
         if (pos==std::string::npos) {                         
            result.push_back(tmp);                         
            tmp="";                                        
         }                                                
         else {                                           
            result.push_back(tmp.substr(0,pos));           
            tmp=tmp.substr(pos+delim.length());            
         }                                                
      }                                                  
      return result;                                     
   }
   void readFile(TString filename) {
      TFile* f = new TFile (filename);
      TList* list = (TList*)gDirectory->GetListOfKeys();
      TIter next(list);
      while (TObject* obj = next())
      {
         TProfile3D* p = (TProfile3D*)gDirectory->Get(obj->GetName());
         std::vector<std::string> index = split(std::string(obj->GetName()),"_");
         if (p != 0) {
            VectorWrapper vw;
            if (vec.size() == 0)
               vec.push_back(vw);
            if (int(vec.size()) == atoi(index[0].c_str()))
               vec.push_back(vw);
            if (int(vec.size()) == atoi(index[0].c_str())+1) {
               vec[atoi(index[0].c_str())].push_back((TProfile3D*)p->Clone());
            } else {
               std::cout << "WARNING::Cannot find the proper index to read in the VectorWrapper2D file." << std::endl;
            }
         }
      }
   }

private:
   std::vector<VectorWrapper> vec;
};

#endif

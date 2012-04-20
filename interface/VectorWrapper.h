#ifndef VECTORWRAPPER_HH
#define VECTORWRAPPER_HH

#include "TROOT.h"
#include "TNamed.h"
#include "TFile.h"
#include "TList.h"
#include "TProfile3D.h"

#include <iostream>
#include <vector>

class VectorWrapper: public TNamed
{
public:
   void push_back(TProfile3D* prof){vec.push_back(prof);}
   void reserve(int size){vec.reserve(size);}
   unsigned int size(){return vec.size();}
   TProfile3D* back(){return vec.back();}
   TProfile3D* operator[](unsigned int a){return vec[a];}
   void writeFile(TString filename, TString writeFlag = "RECREATE")
   {
      TFile* f = new TFile (filename,writeFlag);
      for(unsigned int i=0; i<vec.size(); i++)
      {
         vec[i]->Write();
      }
      f->Close();
      delete f;
   }
   void readFile(TString filename)
   {
      TFile* f = new TFile (filename);
      TList* list = (TList*)gDirectory->GetListOfKeys();
      TIter next(list);
      while (TObject* obj = next())
      {
         TProfile3D* p = (TProfile3D*)gDirectory->Get(obj->GetName());
         if (p !=0 ) vec.push_back((TProfile3D*)p->Clone());
      }
   }

private:
   std::vector<TProfile3D*> vec;
};

#endif

// Author: Rene Brun

#include "JetMETAnalysis/JetUtilities/interface/CommandLine.h"

#include "TROOT.h"
#include "TKey.h"
#include "TFile.h"
#include "TSystem.h"
#include "TTree.h"
#include "TString.h"

#include "iostream"
#include "vector"
#include "string"

using namespace std;

//Example of script showing how to copy all objects (including directories)
//from a source file.
//For each input file, a new directory is created in the current directory 
//with the name of the source file.
//After execution of:
// root > .x copyFiles.C
//the file result.root will contain 4 subdirectories:
// "tot100.root", "hsimple.root", "hs1.root","hs2.root"
      
void CopyDir(TDirectory *source, bool algs_to_skip, vector<TString> ati, int nentries, TString selection, vector<TString>& suffix, bool isSubdir) {
   //copy all objects and subdirs of directory source as a subdir of the current directory   
   for (unsigned int iati=0; iati<ati.size(); iati++) {
      if (algs_to_skip && TString(source->GetName()).CompareTo(ati[iati])==0)
         return;
   }

   source->ls();
   TDirectory *savdir = gDirectory;
   TDirectory *adir;
   if (!isSubdir)
      adir = savdir;
   else {
      TString new_dirname = source->GetName();
      if(suffix.size()>0) {
         new_dirname += suffix.front();
         suffix.erase(suffix.begin());
      }
      cout << source->GetName() << " ==> " << new_dirname << endl;
      adir = savdir->mkdir(new_dirname);
   }
   adir->cd();
   //loop on all entries of this directory
   TKey *key;
   TIter nextkey(source->GetListOfKeys());
   while ((key = (TKey*)nextkey())) {
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom(TDirectory::Class())) {
         source->cd(key->GetName());
         TDirectory *subdir = gDirectory;
         adir->cd();
         CopyDir(subdir,algs_to_skip,ati,nentries,selection,suffix,true);
         adir->cd();
      } else if (cl->InheritsFrom(TTree::Class())) {
         TTree *T = (TTree*)source->Get(key->GetName());
         adir->cd();
         TTree *newT = (selection.IsNull()) ? T->CloneTree(nentries,"fast") : T->CopyTree(selection,"",nentries,0);
         newT->Write();
      } else {
         source->cd();
         TObject *obj = key->ReadObj();
         adir->cd();
         obj->Write();
         delete obj;
      }
   }
   adir->SaveSelf(kTRUE);
   savdir->cd();
}
void CopyFile(const char *fname, bool algs_to_skip, vector<TString> ati, int nentries, TString selection, vector<TString>& suffix) {
   //Copy all objects and subdirs of file fname as a subdir of the current directory
   TDirectory *target = gDirectory;
   TFile *f = TFile::Open(fname);
   if (!f || f->IsZombie()) {
      printf("Cannot copy file: %s\n",fname);
      target->cd();
      return;
   }
   target->cd();
   CopyDir(f,algs_to_skip,ati,nentries,selection,suffix,false);
   delete f;
   target->cd();
}  
//void copyFiles() {
int main(int argc,char**argv)
{
   CommandLine cl;
   if (!cl.parse(argc,argv)) return 0;
   
   vector<TString>      input     = cl.getVector<TString>      ("input");
   TString              output    = cl.getValue<TString>       ("output",       "");
   vector<TString>      ati       = cl.getVector<TString>      ("ati",          ""); //alg_to_ignore
   vector<unsigned int> fati      = cl.getVector<unsigned int> ("fati",         ""); //file containing alg_to_ignore
   vector<TString>      suffix    = cl.getVector<TString>      ("suffix",       "");
   int                  nentries  = cl.getValue<int>           ("nentries",     -1); //number of entries to take from each tree
   TString              selection = cl.getValue<TString>       ("selection",    ""); //selection to apply to the copied entries of the TTrees
   bool                 help      = cl.getValue<bool>          ("help",      false);

   if (help) {cl.print(); return 0;}
   if(!cl.check()) return 0;
   cl.print();
   
   
   //main function copying 4 files as subdirectories of a new file
   if (output.IsNull()) output = "JRA_combined.root";
   TFile *f = TFile::Open(output,"RECREATE");
   for (unsigned int ifile=0; ifile<input.size(); ifile++) {
      bool algs_to_skip = false;
      for (unsigned int i=0; i<fati.size(); i++) {
         if (fati[i]==ifile)
            algs_to_skip=true;
      }
      CopyFile(input[ifile],algs_to_skip,ati,nentries,selection,suffix);
   }
   f->ls();
   delete f;
}

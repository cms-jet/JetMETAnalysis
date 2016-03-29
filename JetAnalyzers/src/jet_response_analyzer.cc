/// this class's header
#include "JetMETAnalysis/JetAnalyzers/interface/jet_response_analyzer.hh"

////////////////////////////////////////////////////////////////////////////////
// construction/destruction
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
jet_response_analyzer::jet_response_analyzer(const edm::ParameterSet& iConfig)
  : moduleLabel_      (iConfig.getParameter<std::string> ("@module_label"))
  , algs              (iConfig.getParameter<vector<string> >      ("algs"))
  , drmaxs            (iConfig.getParameter<vector<double> >    ("drmaxs"))
  , binspt            (iConfig.getParameter<vector<double> >    ("binspt"))
  , binseta           (iConfig.getParameter<vector<double> >   ("binseta"))
  , binsphi           (iConfig.getParameter<vector<double> >   ("binsphi"))
  , binsy             (iConfig.getParameter<vector<double> >     ("binsy"))
  , useweight         (iConfig.getParameter<bool>            ("useweight"))
  , xsection          (iConfig.getParameter<double>           ("xsection"))
  , nrefmax           (iConfig.getParameter<int>               ("nrefmax"))
  , nbinspt           (iConfig.getParameter<int>               ("nbinspt"))
  , nbinseta          (iConfig.getParameter<int>              ("nbinseta"))
  , nbinsphi          (iConfig.getParameter<int>              ("nbinsphi"))
  , nbinsy            (iConfig.getParameter<int>                ("nbinsy"))
  , etabarrelmin      (iConfig.getParameter<double>       ("etabarrelmin"))
  , etabarrelmax      (iConfig.getParameter<double>       ("etabarrelmax"))
  , dobalance         (iConfig.getParameter<bool>            ("dobalance"))
  , doflavor          (iConfig.getParameter<bool>             ("doflavor"))
  , noabsflavors      (iConfig.getParameter<bool>         ("noabsflavors"))
  , drmax             (iConfig.getParameter<double>              ("drmax"))
  , dphimin           (iConfig.getParameter<double>            ("dphimin"))
  , dojetpt           (iConfig.getParameter<bool>              ("dojetpt"))
  , dorefpt           (iConfig.getParameter<bool>              ("dorefpt"))
  , nbinsrelrsp       (iConfig.getParameter<int>           ("nbinsrelrsp"))
  , relrspmin         (iConfig.getParameter<double>          ("relrspmin"))
  , relrspmax         (iConfig.getParameter<double>          ("relrspmax"))
  , nbinsabsrsp       (iConfig.getParameter<int>           ("nbinsabsrsp"))
  , absrspmin         (iConfig.getParameter<double>          ("absrspmin"))
  , absrspmax         (iConfig.getParameter<double>          ("absrspmax"))
  , nbinsetarsp       (iConfig.getParameter<int>           ("nbinsetarsp"))
  , etarspmin         (iConfig.getParameter<double>          ("etarspmin"))
  , etarspmax         (iConfig.getParameter<double>          ("etarspmax"))
  , nbinsphirsp       (iConfig.getParameter<int>           ("nbinsphirsp"))
  , phirspmin         (iConfig.getParameter<double>          ("phirspmin"))
  , phirspmax         (iConfig.getParameter<double>          ("phirspmax"))
  , jtptmin           (iConfig.getParameter<double>            ("jtptmin"))
  , itlow             (iConfig.getParameter<int>                 ("itlow"))
  , ithigh            (iConfig.getParameter<int>                ("ithigh"))
  , earlyootlow       (iConfig.getParameter<int>           ("earlyootlow"))
  , earlyoothigh      (iConfig.getParameter<int>          ("earlyoothigh"))
  , lateootlow        (iConfig.getParameter<int>            ("lateootlow"))
  , lateoothigh       (iConfig.getParameter<int>           ("lateoothigh"))
  , totalootlow       (iConfig.getParameter<int>           ("totalootlow"))
  , totaloothigh      (iConfig.getParameter<int>          ("totaloothigh"))
  , weightfile        (iConfig.getParameter<string>         ("weightfile"))
  , MCPUReWeighting   (iConfig.getParameter<string>    ("MCPUReWeighting"))
  , DataPUReWeighting (iConfig.getParameter<string>  ("DataPUReWeighting"))
  //, getterOfProducts_(edm::ProcessMatch("JRAP"), this)
  , getterOfProducts_(edm::ModuleLabelMatch(moduleLabel_), this)
{

  callWhenNewProductsRegistered(getterOfProducts_);

  dorelrsp=(nbinsrelrsp>0);
  doabsrsp=(nbinsabsrsp>0);
  doetarsp=(nbinsetarsp>0);
  dophirsp=(nbinsphirsp>0);

  firstEvent = true;

  TH1::SetDefaultSumw2(true);

}//C'tor

//______________________________________________________________________________
jet_response_analyzer::~jet_response_analyzer(){

}//D'tor

////////////////////////////////////////////////////////////////////////////////
// implementation of member functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void jet_response_analyzer::beginJob() {
  
  //
  // evaluate drmin requirements for individual algorithms if provided
  //
  //if(algs_all.size()!=drmax_all.size()) {
  if(NAlgs!=NAlgs) {
    cout << "WARNING::The number of drmax specified does not match the number of algorithms specified." << endl
         << " The program cannot continue." << endl
         << " Check the Settings.h file." << endl;
    return;
  }

  cout << "Making the drmax to algorithm map ... ";
  if(algs.size()>0 && drmaxs.size()>0) {
    for (unsigned int ialg=0;ialg<algs.size();ialg++) {
     string alg=algs[ialg];
     float drmax_alg = drmaxs[ialg];
     alg2drmax[alg]=drmax_alg;
    }
  }
  else{
    for (int ialg=0;ialg<NAlgs;ialg++) {
     string alg=algs_all[ialg];
     float drmax_alg = drmax_all[ialg];
     alg2drmax[alg]=drmax_alg;
     algs.push_back(alg);
     //cout<<"drmax("<<alg<<") = "<<alg2drmax[alg]<<endl;
    }
  }
  cout << "DONE" << endl;

  // define flavors
  cout << "Getting the flavors ... ";
  if(doflavor) flavor = get_flavors(noabsflavors);
  else flavor.push_back("");
  cout << "DONE" << endl;

  //
  // get weights
  //
  weightHist = 0;
  /*
  if(!weightfile.IsNull()) {
    TFile* wfile = new TFile(weightfile,"READ");
    if (!wfile->IsOpen()) { cout<<"Can't open "<<weightfile<<endl; }
    gDirectory->cd(alg.c_str());
    if(!doflavor) {
      weightHist = (TH1D*)wfile->Get("we");
      if (weightHist==0) { cout<<"weightHist named \"we\" was not in file "<<weightfile<<endl; return 0; } 
      weightHist->Scale(1./weightHist->Integral(1,weightHist->FindBin(3)));
    }
    else {
      for(unsigned int f=0; f<flavor.size(); f++) {
      if(flavor[f]!="")
        weightMap[flavor[f]] = (TH2D*)gDirectory->Get(flavor[f].c_str());
      else
        weightMap[flavor[f]] = (TH2D*)gDirectory->Get("all_");
      if(weightMap[flavor[f]]==0) { cout<<"weightHist named \""<< flavor[f]<< "\" was not in file "<<weightfile<<endl; return 0; } 
      }
    }
  }
  */
  
  if(!MCPUReWeighting.IsNull() && !DataPUReWeighting.IsNull()) { 
    LumiWeights_ = edm::LumiReWeighting(string(MCPUReWeighting),string(DataPUReWeighting),"pileup","pileup_jt400");
  }

  weight = 1.0;
  flavorWeight = 1.0;
  //if (xsection>0.0) { weight = xsection/tree->GetEntries(); useweight = false; } //FIX ME!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  //
  // create the output file service
  //
  cout << "Creating the output file ... ";
  edm::Service<TFileService> fs;
  if (!fs) throw edm::Exception(edm::errors::Configuration,"TFileService missing from configuration!");
  cout << "DONE" << endl;

  for(unsigned int ialg = 0; ialg<algs.size(); ialg++) {
    //
    // create directory in output file and book histograms
    //
    // create the histograms
    //cout << "\tCreating Histograms...";
    bookHistograms(fs,algs[ialg]);
    //cout << "DONE" << endl;
  }
}

//______________________________________________________________________________
void jet_response_analyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  //
  // Load the objects of interest
  //
  getCollections(iEvent, iSetup, algs);
  if(firstEvent) {
     cout << "There were " << algHandles.size() << " algorithms found for module label " << moduleLabel_ << endl;
   }

  for (unsigned int ialg=0;ialg<algHandles.size();ialg++) {
    alg = algHandles[ialg].provenance()->moduleLabel();
    //cout << "alg = " << alg << endl;
    //cout<<alg<<" ... "<<endl;
    if (algs.size()>0&&!contains(algs,alg)) {
      if(firstEvent)
        cout << "WARNING::Skipping algorithm " << alg << "." << endl;
      continue;
    }

    float drmax_alg = drmax;
    if (alg2drmax.find(alg)!=alg2drmax.end()) {
      drmax_alg=alg2drmax[alg];
    }
    else {
      cout << "WARNING::Couldn't find algorithm " << alg << " in alg2drmax." << endl
           << " Using the programs default drmax of " << drmax << "." << endl;
    }

    //
    // fill histograms
    //
    if (nrefmax>0) nref = std::min((int)algHandles[ialg]->nref,nrefmax);
    for (unsigned char iref=0;iref<algHandles[ialg]->nref;iref++) {

      if (( dobalance&&algHandles[ialg]->refdphijt->at(iref)<dphimin)|| (!dobalance&&algHandles[ialg]->refdrjt->at(iref)>drmax_alg)) continue;

      if (algHandles[ialg]->jtpt->at(iref)<jtptmin) continue;

      if (!pileup_cut(itlow,ithigh,earlyootlow,earlyoothigh,lateootlow,lateoothigh,totalootlow,totaloothigh,
                      algHandles[ialg]->npus)) {
        //cout << "WARNING::Ref " << iref << " for algorithm " << algs[ialg] << " failed the PU test." << endl
        //     << "Skipping the event." << endl;
        continue;
      }

      float eta    = (binseta.size()&&binseta.front()>=0.)?std::abs(algHandles[ialg]->jteta->at(iref)):algHandles[ialg]->jteta->at(iref);
      float y      = (binsy.size()&&binsy.front()>=0.)?std::abs(algHandles[ialg]->jty->at(iref)):algHandles[ialg]->jty->at(iref);
      float pt     = algHandles[ialg]->jtpt->at(iref);
      float pdgid  = algHandles[ialg]->refpdgid->at(iref);

      float absrsp = algHandles[ialg]->jtpt->at(iref)-algHandles[ialg]->refpt->at(iref);
      float relrsp = algHandles[ialg]->jtpt->at(iref)/algHandles[ialg]->refpt->at(iref);
      float etarsp = algHandles[ialg]->jteta->at(iref)-algHandles[ialg]->refeta->at(iref);
      float phirsp = fmod(algHandles[ialg]->jtphi->at(iref)-algHandles[ialg]->refphi->at(iref)+3*M_PI,2*M_PI)-M_PI;

      //
      // retrieve the correct weight
      //
      if (!(xsection>0.0) && !useweight)
        weight = 1.0;
      if(!weightfile.IsNull()) {
        if(!doflavor && log10(algHandles[ialg]->refpt->at(iref))<3) {
          weight = weightHist->GetBinContent(weightHist->FindBin(log10(algHandles[ialg]->refpt->at(iref))));
        }
        else if(doflavor) {
          weight = weightMap["all_"]->GetBinContent(weightMap["all_"]->FindBin(pt,eta));
          if(noabsflavors)
           flavorWeight = weightMap[pdgid_to_flavor_name(pdgid)]->GetBinContent(weightMap[pdgid_to_flavor_name(pdgid)]->FindBin(pt,eta));
         else
           flavorWeight = weightMap[pdgid_to_flavor_name(fabs(pdgid))]->GetBinContent(weightMap[pdgid_to_flavor_name(fabs(pdgid))]->FindBin(pt,eta));
        }
      }
      else
        flavorWeight = weight;
      if(!MCPUReWeighting.IsNull() && !DataPUReWeighting.IsNull()) {
        double LumiWeight = LumiWeights_.weight(algHandles[ialg]->tnpus->at(1));
        weight *= LumiWeight;
      }

      if (eta>=etabarrelmin&&eta<=etabarrelmax) {
        if (dorefpt) {
          fill_histo(algHandles[ialg]->refpt->at(iref),weight,algHandles[ialg]->refpt->at(iref),binspt,"RefPt_Barrel_","RefPt");
          fill_histo(algHandles[ialg]->jtpt ->at(iref),weight,algHandles[ialg]->refpt->at(iref),binspt,"JetPt_Barrel_","RefPt");
          if (doflavor) {
            fill_histo(algHandles[ialg]->refpdgid->at(iref),algHandles[ialg]->refpt->at(iref),flavorWeight,
                       algHandles[ialg]->refpt->at(iref),binspt,"RefPt_Barrel_","RefPt",noabsflavors);
            fill_histo(algHandles[ialg]->refpdgid->at(iref),algHandles[ialg]->jtpt->at(iref),flavorWeight,
                       algHandles[ialg]->refpt->at(iref),binspt,"JetPt_Barrel_","RefPt",noabsflavors);
          }
        }
        if (dorelrsp&&dorefpt) {
          fill_histo(relrsp,weight,algHandles[ialg]->refpt->at(iref),binspt,"RelRsp_Barrel_","RefPt");
          if (doflavor) {
            fill_histo(algHandles[ialg]->refpdgid->at(iref),relrsp,flavorWeight,algHandles[ialg]->refpt->at(iref),
                       binspt,"RelRsp_Barrel_","RefPt",noabsflavors);
          }
        }
        if (doabsrsp&&dorefpt) {
          fill_histo(absrsp,weight,algHandles[ialg]->refpt->at(iref),binspt,"AbsRsp_Barrel_","RefPt");
          if (doflavor) {
            fill_histo(algHandles[ialg]->refpdgid->at(iref),absrsp,flavorWeight,algHandles[ialg]->refpt->at(iref),
                       binspt,"AbsRsp_Barrel_","RefPt",noabsflavors);
          }
        }
      }

      if (dojetpt) {
        fill_histo(algHandles[ialg]->jtpt->at(iref),weight,algHandles[ialg]->jtpt->at(iref), binspt,"JetPt_","JetPt");
        if (doflavor)
          fill_histo(algHandles[ialg]->refpdgid->at(iref),algHandles[ialg]->jtpt->at(iref),flavorWeight,
                     algHandles[ialg]->jtpt->at(iref),binspt,"JetPt_","JetPt",noabsflavors);
      }
      if (dorefpt) {
        fill_histo(algHandles[ialg]->refpt->at(iref),weight,algHandles[ialg]->refpt->at(iref),binspt,"RefPt_","RefPt");
        fill_histo(algHandles[ialg]->jtpt ->at(iref),weight,algHandles[ialg]->refpt->at(iref),binspt,"JetPt_","RefPt");
        if (doflavor) {
          fill_histo(algHandles[ialg]->refpdgid->at(iref),algHandles[ialg]->refpt->at(iref),flavorWeight,
                     algHandles[ialg]->refpt->at(iref),binspt,"RefPt_","RefPt",noabsflavors);
          fill_histo(algHandles[ialg]->refpdgid->at(iref),algHandles[ialg]->jtpt->at(iref),flavorWeight,
                     algHandles[ialg]->refpt->at(iref),binspt,"JetPt_","RefPt",noabsflavors);
        }
      }

      fill_histo(eta,weight,eta,binseta,"JetEta_","JetEta");
      if (doflavor) fill_histo(algHandles[ialg]->refpdgid->at(iref),eta,flavorWeight,eta,binseta,"JetEta_","JetEta",
                               noabsflavors);

      fill_histo(algHandles[ialg]->jtphi->at(iref),weight,algHandles[ialg]->jtphi->at(iref),binsphi,"JetPhi_","JetPhi");
      if (doflavor) fill_histo(algHandles[ialg]->refpdgid->at(iref),algHandles[ialg]->jtphi->at(iref),flavorWeight,
                               algHandles[ialg]->jtphi->at(iref),binsphi,"JetPhi_","JetPhi",noabsflavors);

      fill_histo(algHandles[ialg]->jty->at(iref),weight,algHandles[ialg]->jty->at(iref),binsy,"JetY_","JetY");
      if (doflavor) fill_histo(algHandles[ialg]->refpdgid->at(iref),algHandles[ialg]->jty->at(iref),flavorWeight,
                               algHandles[ialg]->jty->at(iref),binsy,"JetY_","JetY",noabsflavors);

      if (dojetpt) {
        fill_histo(algHandles[ialg]->jtpt->at(iref),weight,eta,algHandles[ialg]->jtpt->at(iref),binseta,binspt,
                   "JetPt_","JetEta","JetPt");
        fill_histo(algHandles[ialg]->jtpt->at(iref),weight,y,algHandles[ialg]->jtpt->at(iref),binsy,binspt,
                   "JetPt_","JetY","JetPt");

        if (doflavor) {
          fill_histo(algHandles[ialg]->refpdgid->at(iref),algHandles[ialg]->jtpt->at(iref),flavorWeight,
                     eta,algHandles[ialg]->jtpt->at(iref),binseta,binspt,"JetPt_","JetEta","JetPt",
                     noabsflavors);
          fill_histo(algHandles[ialg]->refpdgid->at(iref),algHandles[ialg]->jtpt->at(iref),flavorWeight,
                     y,algHandles[ialg]->jtpt->at(iref),binsy,binspt,"JetPt_","JetY","JetPt",
                     noabsflavors);
        }
      }

      if (dorefpt) {
        fill_histo(algHandles[ialg]->refpt->at(iref),weight,eta,algHandles[ialg]->refpt->at(iref),binseta,binspt,
                   "RefPt_","JetEta","RefPt");
        fill_histo(algHandles[ialg]->jtpt ->at(iref),weight,eta,algHandles[ialg]->refpt->at(iref),binseta,binspt,
                   "JetPt_","JetEta","RefPt");
        fill_histo(algHandles[ialg]->refpt->at(iref),weight,y,algHandles[ialg]->refpt->at(iref),binsy,binspt,
                   "RefPt_","JetY","RefPt");
        fill_histo(algHandles[ialg]->jtpt ->at(iref),weight,y,algHandles[ialg]->refpt->at(iref),binsy,binspt,
                   "JetPt_","JetY","RefPt");
        if (doflavor) {
          fill_histo(algHandles[ialg]->refpdgid->at(iref),algHandles[ialg]->refpt->at(iref),flavorWeight,
                     eta,algHandles[ialg]->refpt->at(iref),binseta,binspt,"RefPt_","JetEta","RefPt",
                     noabsflavors);
          fill_histo(algHandles[ialg]->refpdgid->at(iref),algHandles[ialg]->jtpt->at(iref),flavorWeight,
                     eta,algHandles[ialg]->refpt->at(iref),binseta,binspt,"JetPt_","JetEta","RefPt",
                     noabsflavors);
          fill_histo(algHandles[ialg]->refpdgid->at(iref),algHandles[ialg]->refpt->at(iref),flavorWeight,
                     y,algHandles[ialg]->refpt->at(iref),binsy,binspt,"RefPt_","JetY","RefPt",
                     noabsflavors);
          fill_histo(algHandles[ialg]->refpdgid->at(iref),algHandles[ialg]->jtpt->at(iref),flavorWeight,
                     y,algHandles[ialg]->refpt->at(iref),binsy,binspt,"JetPt_","JetY","RefPt",
                     noabsflavors);
        }
      }

      if (dorelrsp) {
        if (dojetpt) {
          fill_histo(relrsp,weight,algHandles[ialg]->jtpt->at(iref),binspt,"RelRsp_","JetPt");
          if (doflavor) fill_histo(algHandles[ialg]->refpdgid->at(iref),relrsp,flavorWeight,
                                   algHandles[ialg]->jtpt->at(iref), binspt,"RelRsp_","JetPt",
                                   noabsflavors);
        }
        if (dorefpt) {
          fill_histo(relrsp,weight,algHandles[ialg]->refpt->at(iref),binspt,"RelRsp_","RefPt");
          if (doflavor) fill_histo(algHandles[ialg]->refpdgid->at(iref),relrsp,flavorWeight,
                                   algHandles[ialg]->refpt->at(iref),binspt,"RelRsp_","RefPt",
                                   noabsflavors);
        }

        fill_histo(relrsp,weight,eta,binseta,"RelRsp_","JetEta");
        if (doflavor) fill_histo(algHandles[ialg]->refpdgid->at(iref),relrsp,flavorWeight,
                               eta,binseta,"RelRsp_","JetEta",noabsflavors);

        fill_histo(relrsp,weight,algHandles[ialg]->jtphi->at(iref),binsphi,"RelRsp_","JetPhi");
        if (doflavor) fill_histo(algHandles[ialg]->refpdgid->at(iref),relrsp,flavorWeight,
                                 algHandles[ialg]->jtphi->at(iref),binsphi,"RelRsp_","JetPhi",
                                 noabsflavors);

        fill_histo(relrsp,weight,algHandles[ialg]->jty->at(iref),binsy,"RelRsp_","JetY");
        if (doflavor) fill_histo(algHandles[ialg]->refpdgid->at(iref),relrsp,flavorWeight,
                                 algHandles[ialg]->jty->at(iref),binsy,"RelRsp_","JetY",
                                 noabsflavors);

        if (dojetpt) {
          fill_histo(relrsp,weight,eta,algHandles[ialg]->jtpt->at(iref),binseta,binspt,
                     "RelRsp_","JetEta","JetPt");
          fill_histo(relrsp,weight,y,algHandles[ialg]->jtpt->at(iref),binsy,binspt,
                     "RelRsp_","JetY","JetPt");
          if (doflavor) {
            fill_histo(algHandles[ialg]->refpdgid->at(iref),relrsp,flavorWeight,eta,algHandles[ialg]->jtpt->at(iref),
                       binseta,binspt,"RelRsp_","JetEta","JetPt",noabsflavors);
            fill_histo(algHandles[ialg]->refpdgid->at(iref),relrsp,flavorWeight,y,algHandles[ialg]->jtpt->at(iref),
                       binsy,binspt,"RelRsp_","JetY","JetPt",noabsflavors);
          }
        }
        if (dorefpt) {
          fill_histo(relrsp,weight,eta,algHandles[ialg]->refpt->at(iref),binseta,binspt,
                     "RelRsp_","JetEta","RefPt");
          fill_histo(relrsp,weight,y,algHandles[ialg]->refpt->at(iref),binsy,binspt,
                     "RelRsp_","JetY","RefPt");
          if (doflavor) {
            fill_histo(algHandles[ialg]->refpdgid->at(iref),relrsp,flavorWeight,eta,algHandles[ialg]->refpt->at(iref),
                       binseta,binspt,"RelRsp_","JetEta","RefPt",noabsflavors);
            fill_histo(algHandles[ialg]->refpdgid->at(iref),relrsp,flavorWeight,y,algHandles[ialg]->refpt->at(iref),
                       binsy,binspt,"RelRsp_","JetY","RefPt",noabsflavors);
          }
        }
      }

      if (doabsrsp) {
        if (dojetpt) {
          fill_histo(absrsp,weight,algHandles[ialg]->jtpt->at(iref), binspt,"AbsRsp_","JetPt");
          if (doflavor) fill_histo(algHandles[ialg]->refpdgid->at(iref),absrsp,flavorWeight,
                                   algHandles[ialg]->jtpt->at(iref),binspt,"AbsRsp_","JetPt",
                                   noabsflavors);
        }
        if (dorefpt) {
          fill_histo(absrsp,weight,algHandles[ialg]->refpt->at(iref),binspt,"AbsRsp_","RefPt");
          if (doflavor) fill_histo(algHandles[ialg]->refpdgid->at(iref),absrsp,flavorWeight,
                                   algHandles[ialg]->refpt->at(iref),binspt,"AbsRsp_","RefPt",
                                   noabsflavors);
        }

        fill_histo(absrsp,weight,eta,binseta,"AbsRsp_","JetEta");
        if (doflavor) fill_histo(algHandles[ialg]->refpdgid->at(iref),absrsp,flavorWeight,
                                 eta,binseta,"AbsRsp_","JetEta",noabsflavors);

        fill_histo(absrsp,weight,algHandles[ialg]->jtphi->at(iref),binsphi,"AbsRsp_","JetPhi");
        if (doflavor) fill_histo(algHandles[ialg]->refpdgid->at(iref),absrsp,flavorWeight,
                                 algHandles[ialg]->jtphi->at(iref),binsphi,"AbsRsp_","JetPhi",
                                 noabsflavors);

        fill_histo(absrsp,weight,algHandles[ialg]->jty->at(iref),binsy,"AbsRsp_","JetY");
        if (doflavor) fill_histo(algHandles[ialg]->refpdgid->at(iref),absrsp,flavorWeight,
                                 algHandles[ialg]->jty->at(iref),binsy,"AbsRsp_","JetY",
                                 noabsflavors);

        if (dojetpt) {
          fill_histo(absrsp,weight,eta,algHandles[ialg]->jtpt->at(iref),
                     binseta,binspt,"AbsRsp_","JetEta","JetPt");
          fill_histo(absrsp,weight,y,algHandles[ialg]->jtpt->at(iref),
                     binsy,binspt,"AbsRsp_","JetY","JetPt");
          if (doflavor) {
            fill_histo(algHandles[ialg]->refpdgid->at(iref),absrsp,flavorWeight,eta,algHandles[ialg]->jtpt->at(iref),
                       binseta,binspt,"AbsRsp_","JetEta","JetPt",noabsflavors);
            fill_histo(algHandles[ialg]->refpdgid->at(iref),absrsp,flavorWeight,y,algHandles[ialg]->jtpt->at(iref),
                       binsy,binspt,"AbsRsp_","JetY","JetPt",noabsflavors);
          }
        }
        if (dorefpt) {
          fill_histo(absrsp,weight,eta,algHandles[ialg]->refpt->at(iref),binseta,binspt,
                     "AbsRsp_","JetEta","RefPt");
          fill_histo(absrsp,weight,y,algHandles[ialg]->refpt->at(iref),binsy,binspt,
                     "AbsRsp_","JetY","RefPt");
          if (doflavor) {
            fill_histo(algHandles[ialg]->refpdgid->at(iref),absrsp,flavorWeight,eta,algHandles[ialg]->refpt->at(iref),
                       binseta,binspt,"AbsRsp_","JetEta","RefPt",noabsflavors);
            fill_histo(algHandles[ialg]->refpdgid->at(iref),absrsp,flavorWeight,y,algHandles[ialg]->refpt->at(iref),
                       binsy,binspt,"AbsRsp_","JetY","RefPt",noabsflavors);
          }
        }
      }

      if (doetarsp) {
        if (dojetpt) {
          fill_histo(etarsp,weight,algHandles[ialg]->jtpt->at(iref), binspt,"EtaRsp_","JetPt");
          if (doflavor) fill_histo(algHandles[ialg]->refpdgid->at(iref),etarsp,flavorWeight,
                                   algHandles[ialg]->jtpt->at(iref),binspt,"EtaRsp_","JetPt",
                                   noabsflavors);
        }
        if (dorefpt) {
          fill_histo(etarsp,weight,algHandles[ialg]->refpt->at(iref),binspt,"EtaRsp_","RefPt");
          if (doflavor) fill_histo(algHandles[ialg]->refpdgid->at(iref),etarsp,flavorWeight,
                                   algHandles[ialg]->refpt->at(iref),binspt,"EtaRsp_","RefPt",
                                   noabsflavors);
        }

        fill_histo(etarsp,weight,eta,binseta,"EtaRsp_","JetEta");
        if (doflavor) fill_histo(algHandles[ialg]->refpdgid->at(iref),etarsp,flavorWeight,
                                 eta,binseta,"EtaRsp_","JetEta",noabsflavors);

        fill_histo(etarsp,weight,algHandles[ialg]->jtphi->at(iref),binsphi,"EtaRsp_","JetPhi");
        if (doflavor) fill_histo(algHandles[ialg]->refpdgid->at(iref),etarsp,flavorWeight,
                                 algHandles[ialg]->jtphi->at(iref),binsphi,"EtaRsp_","JetPhi",
                                 noabsflavors);

        if (dojetpt) {
          fill_histo(etarsp,weight,eta,algHandles[ialg]->jtpt->at(iref),binseta,binspt,"EtaRsp_","JetEta","JetPt");
          if (doflavor) fill_histo(algHandles[ialg]->refpdgid->at(iref),etarsp,flavorWeight,
                                   eta,algHandles[ialg]->jtpt->at(iref),binseta,binspt,"EtaRsp_","JetEta","JetPt",
                                   noabsflavors);
          }
        if (dorefpt) {
          fill_histo(etarsp,weight,eta,algHandles[ialg]->refpt->at(iref),binseta,binspt,"EtaRsp_","JetEta","RefPt");
          if (doflavor) fill_histo(algHandles[ialg]->refpdgid->at(iref),etarsp,flavorWeight,
                                   eta,algHandles[ialg]->refpt->at(iref),binseta,binspt,"EtaRsp_","JetEta","RefPt",
                                   noabsflavors);
        }
      }

      if (dophirsp) {
        if (dojetpt) {
          fill_histo(phirsp,weight,algHandles[ialg]->jtpt->at(iref), binspt,"PhiRsp_","JetPt");
          if (doflavor) fill_histo(algHandles[ialg]->refpdgid->at(iref),phirsp,flavorWeight,
                                   algHandles[ialg]->jtpt->at(iref),binspt,"PhiRsp_","JetPt",
                                   noabsflavors);
        }
        if (dorefpt) {
          fill_histo(phirsp,weight,algHandles[ialg]->refpt->at(iref),binspt,"PhiRsp_","RefPt");
          if (doflavor) fill_histo(algHandles[ialg]->refpdgid->at(iref),phirsp,flavorWeight,
                                   algHandles[ialg]->refpt->at(iref),binspt,"PhiRsp_","RefPt",
                                   noabsflavors);
        }

        fill_histo(phirsp,weight,eta,binseta,"PhiRsp_","JetEta");
        if (doflavor) fill_histo(algHandles[ialg]->refpdgid->at(iref),phirsp,flavorWeight,
                                 eta,binseta,"PhiRsp_","JetEta",noabsflavors);

        fill_histo(phirsp,weight,algHandles[ialg]->jtphi->at(iref),binsphi,"PhiRsp_","JetPhi");
        if (doflavor) fill_histo(algHandles[ialg]->refpdgid->at(iref),phirsp,flavorWeight,
                                 algHandles[ialg]->jtphi->at(iref),binsphi,"PhiRsp_","JetPhi",
                                 noabsflavors);

        if (dojetpt) {
          fill_histo(phirsp,weight,eta,algHandles[ialg]->jtpt->at(iref),binseta,binspt,"PhiRsp_","JetEta","JetPt");
          if (doflavor) fill_histo(algHandles[ialg]->refpdgid->at(iref),phirsp,flavorWeight,
                                   eta,algHandles[ialg]->jtpt->at(iref),binseta,binspt,"PhiRsp_","JetEta","JetPt",
                                   noabsflavors);
        }
        if (dorefpt) {
          fill_histo(phirsp,weight,eta,algHandles[ialg]->refpt->at(iref),binseta,binspt,"PhiRsp_","JetEta","RefPt");
          if (doflavor) fill_histo(algHandles[ialg]->refpdgid->at(iref),phirsp,flavorWeight,
                                   eta,algHandles[ialg]->refpt->at(iref),binseta,binspt,"PhiRsp_","JetEta","RefPt",
                                   noabsflavors);
        }
      }
    }
  }
  if(firstEvent) firstEvent = false;
}


////////////////////////////////////////////////////////////////////////////////
// implement member functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void jet_response_analyzer::bookHistograms(edm::Service<TFileService>& fs, string alg) {
  //cout << "Creating sub-directory for " << alg << " ... ";
  //Make the sub-directory
  //subDirs.push_back(fs->mkdir(alg));
  //cout << "DONE" << endl;

  cout << "\tBooking histograms ... ";

  histosPerAlg.clear();

  // book pT histograms
  if (binspt.size()>=2) {
    for (unsigned int ipt=0;ipt<binspt.size()-1;++ipt) {

      string hname; float ptmin=binspt[ipt]; float ptmax=binspt[ipt+1];

      for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
        if (dojetpt) {
          //jetPtVsJetPt
          hname=flavor[iflv]+"JetPt_"+get_suffix("JetPt",ipt,binspt);
          histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T} [GeV]",nbinspt,ptmin,ptmax);
        }
        if (dorefpt) {
          //refPtVsRefPt
          hname=flavor[iflv]+"RefPt_"+get_suffix("RefPt",ipt,binspt);
          histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T}^{ref} [GeV]",nbinspt,ptmin,ptmax);

          //jetPtVsRefPt
          hname=flavor[iflv]+"JetPt_"+get_suffix("RefPt",ipt,binspt);
          histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T} [GeV]",3*nbinspt,0,3.0*ptmax);

          //refPtVsRefPtBarrel
          hname=flavor[iflv]+"RefPt_Barrel_"+get_suffix("RefPt",ipt,binspt);
          histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T}^{ref} [GeV]",nbinspt,ptmin,ptmax);

          //jetPtVsRefPtBarrel
          hname=flavor[iflv]+"JetPt_Barrel_"+get_suffix("RefPt",ipt,binspt);
          histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T} [GeV]",3*nbinspt,0,3.0*ptmax);
        }
        if (dorelrsp&&dojetpt) {
          //relRspVsJetPt
          hname=flavor[iflv]+"RelRsp_"+get_suffix("JetPt",ipt,binspt);
          histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T}/p_{T}^{ref}",
                                                        nbinsrelrsp,relrspmin,relrspmax);
        }
        if (dorelrsp&&dorefpt) {
          //relRspVsRefPt
          hname=flavor[iflv]+"RelRsp_"+get_suffix("RefPt",ipt,binspt);
          histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T}/p_{T}^{ref}",
                                                        nbinsrelrsp,relrspmin,relrspmax);

          //relRspVsRefPtBarrel
          hname=flavor[iflv]+"RelRsp_Barrel_"+get_suffix("RefPt",ipt,binspt);
          histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T}/p_{T}^{ref}",
                                                        nbinsrelrsp,relrspmin,relrspmax);
        }
        if (doabsrsp&&dojetpt) {
          //absRspVsJetPt
          hname=flavor[iflv]+"AbsRsp_"+get_suffix("JetPt",ipt,binspt);
          histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T}-p_{T}^{ref} [GeV]",
                                                        nbinsabsrsp,absrspmin,absrspmax);
        }
        if (doabsrsp&&dorefpt) {
          //absRspVsRefPt
          hname=flavor[iflv]+"AbsRsp_"+get_suffix("RefPt",ipt,binspt);
          histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T}-p_{T}^{ref} [GeV]",
                                                        nbinsabsrsp,absrspmin,absrspmax);

          //absRspVsRefPtBarrel
          hname=flavor[iflv]+"AbsRsp_Barrel_"+get_suffix("RefPt",ipt,binspt);
          histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T}-p_{T}^{ref} [GeV]",
                                                        nbinsabsrsp,absrspmin,absrspmax);
        }
        if (doetarsp&&dojetpt) {
          //etaRspVsJetPt
          hname=flavor[iflv]+"EtaRsp_"+get_suffix("JetPt",ipt,binspt);
          histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";|#eta|-|#eta^{ref}|",
                                                        nbinsetarsp,etarspmin,etarspmax);
        }
        if (doetarsp&&dorefpt) {
          //etaRspVsRefPt
          hname=flavor[iflv]+"EtaRsp_"+get_suffix("RefPt",ipt,binspt);
          histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";|#eta|-|#eta^{ref}|",
                                                        nbinsetarsp,etarspmin,etarspmax);
        }
        if (dophirsp&&dojetpt) {
          //phiRspVsJetPt
          hname=flavor[iflv]+"PhiRsp_"+get_suffix("JetPt",ipt,binspt);
          histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";#phi-#phi^{ref}",
                                                        nbinsphirsp,phirspmin,phirspmax);
        }
        if (dophirsp&&dorefpt) {
          //phiRspVsRefPt
          hname=flavor[iflv]+"PhiRsp_"+get_suffix("RefPt",ipt,binspt);
          histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";#phi-#phi^{ref}",
                                                        nbinsphirsp,phirspmin,phirspmax);
        }
      }
    }
  }

  // book eta histograms
  if (binseta.size()>=2) {
    for (unsigned int ieta=0;ieta<binseta.size()-1;++ieta) {

      string hname; float etamin=binseta[ieta]; float etamax=binseta[ieta+1];

      for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
        if (1) {
          //jetEtaVsJetEta
          hname=flavor[iflv]+"JetEta_"+get_suffix("JetEta",ieta,binseta);
          histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";#eta",
                                                        nbinseta,etamin,etamax);          
        }
        if (dorelrsp) {
          //relRspVsJetEta
          hname=flavor[iflv]+"RelRsp_"+get_suffix("JetEta",ieta,binseta);
          histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T}/p_{T}^{ref}",
                                                        nbinsrelrsp,relrspmin,relrspmax);
        }
        if (doabsrsp) {
          //absRspVsJetEta
          hname=flavor[iflv]+"AbsRsp_"+get_suffix("JetEta",ieta,binseta);
          histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T}-p_{T}^{ref} [GeV]",
                                                        nbinsabsrsp,absrspmin,absrspmax);
        }
        if (doetarsp) {
          //etaRspVsJetEta
          hname=flavor[iflv]+"EtaRsp_"+get_suffix("JetEta",ieta,binseta);
          histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";|#eta|-|#eta^{ref}|",
                                                        nbinsetarsp,etarspmin,etarspmax);
        }
        if (dophirsp) {
          //phiRspVsJetEta
          hname=flavor[iflv]+"PhiRsp_"+get_suffix("JetEta",ieta,binseta);
          histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";#phi-#phi^{ref}",
                                                        nbinsphirsp,phirspmin,phirspmax);
        }
      }
    }
  }

  // book phi histograms
  if (binsphi.size()>=2) {
    for (unsigned int iphi=0;iphi<binsphi.size()-1;++iphi) {

      string hname; float phimin=binsphi[iphi]; float phimax=binsphi[iphi+1];

      for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
        if (1) {
          //jetPhiVsJetPhi
          hname=flavor[iflv]+"JetPhi_"+get_suffix("JetPhi",iphi,binsphi);
          histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";#phi",
                                                        nbinsphi,phimin,phimax);
        }
        if (dorelrsp) {
          //relRspVsJetPhi
          hname=flavor[iflv]+"RelRsp_"+get_suffix("JetPhi",iphi,binsphi);
          histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T}/p_{T}^{ref}",
                                                        nbinsrelrsp,relrspmin,relrspmax);
        }
        if (doabsrsp) {
          //absRspVsJetPhi
          hname=flavor[iflv]+"AbsRsp_"+get_suffix("JetPhi",iphi,binsphi);
          histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T}-p_{T}^{ref} [GeV]",
                                                        nbinsabsrsp,absrspmin,absrspmax);
        }
        if (doetarsp) {
          //etaRspVsJetPhi
          hname=flavor[iflv]+"EtaRsp_"+get_suffix("JetPhi",iphi,binsphi);
          histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";|#eta|-|#eta^{ref}|",
                                                        nbinsetarsp,etarspmin,etarspmax);
        }
        if (dophirsp) {
          //phiRspVsJetPhi
          hname=flavor[iflv]+"PhiRsp_"+get_suffix("JetPhi",iphi,binsphi);
          histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";#phi-#phi^{ref}",
                                                        nbinsphirsp,phirspmin,phirspmax);
        }
      }
    }
  }

  // book y histograms (rapidity)
  if (binsy.size()>=2) {
    for (unsigned int iy=0;iy<binsy.size()-1;++iy) {

      string hname; float ymin=binsy[iy]; float ymax=binsy[iy+1];

      for (unsigned int iflv=0;iflv<flavor.size();iflv++) {
        if (1) {
          //jetYVsJetY
          hname=flavor[iflv]+"JetY_"+get_suffix("JetY",iy,binsy);
          histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";#y",
                                                        nbinsy,ymin,ymax);
        }
        if (dorelrsp) { 
          //relRspVsJetY
          hname=flavor[iflv]+"RelRsp_"+get_suffix("JetY",iy,binsy);
          histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T}/p_{T}^{ref}",
                                                        nbinsrelrsp,relrspmin,relrspmax);
        }
        if (doabsrsp) {
          //absRspVsJetY
          hname=flavor[iflv]+"AbsRsp_"+get_suffix("JetY",iy,binsy);
          histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T}-p_{T}^{ref} [GeV]",
                                                        nbinsabsrsp,absrspmin,absrspmax);
        }
      }
    }
  }

  // book eta/pT histograms
  if (binspt.size()>=2&&binseta.size()>=2) {
    for (unsigned int ieta=0;ieta<binseta.size()-1;++ieta) {

      string jetEtaSuffix=get_suffix("JetEta",ieta,binseta);

      for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++) {

        string hname; float ptmin=binspt[ipt]; float ptmax=binspt[ipt+1];

        string jetPtSuffix=get_suffix("JetPt",ipt,binspt);
        string refPtSuffix=get_suffix("RefPt",ipt,binspt);

        for (unsigned int iflv=0;iflv<flavor.size();iflv++) {

          if (dojetpt) {
            //jetPtJetPt
            //jetPtVsJetEtaJetPt
            hname=flavor[iflv]+"JetPt_"+jetEtaSuffix+"_"+jetPtSuffix;
            histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T}",nbinspt,ptmin,ptmax);
          }
          if (dorefpt) {
            //refPtRefPt
            //refPtVsJetEtaRefPt
            hname=flavor[iflv]+"RefPt_"+jetEtaSuffix+"_"+refPtSuffix;
            histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T}^{ref}",nbinspt,ptmin,ptmax);

            //jetPtRefPt
            //jetPtVsJetEtaRefPt
            hname=flavor[iflv]+"JetPt_"+jetEtaSuffix+"_"+refPtSuffix;
            histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T}",3*nbinspt,0,3.0*ptmax);
          }
          if (dorelrsp&&dojetpt) {
            //relRspJetPt
            //relRspVsJetEtaJetPt
            hname=flavor[iflv]+"RelRsp_"+jetEtaSuffix+"_"+jetPtSuffix;
            histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T}/p_{T}^{ref}",
                                                          nbinsrelrsp,relrspmin,relrspmax);
          }
          if (dorelrsp&&dorefpt) {
            //relRspRefPt
            //relRspVsJetEtaRefPt
            hname=flavor[iflv]+"RelRsp_"+jetEtaSuffix+"_"+refPtSuffix;
            histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T}/p_{T}^{ref}",
                                                          nbinsrelrsp,relrspmin,relrspmax);
          }
          if (doabsrsp&&dojetpt) {
            //absRspJetPt
            //absRspVsJetEtaJetPt
            hname=flavor[iflv]+"AbsRsp_"+jetEtaSuffix+"_"+jetPtSuffix;
            histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T}-p_{T}^{ref} [GeV]",
                                                          nbinsabsrsp,absrspmin,absrspmax);

            //absRspJetPt
            //absRspVsJetEtaJetPt
            hname=flavor[iflv]+"AbsRsp_"+jetEtaSuffix+"_"+jetPtSuffix;
            histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";|#eta|-|#eta^{ref}|",
                                                          nbinsabsrsp,absrspmin,absrspmax);                                                      
          }
          if (doabsrsp&&dorefpt) {
            //absRspRefPt
            //absRspVsJetEtaRefPt
            hname=flavor[iflv]+"AbsRsp_"+jetEtaSuffix+"_"+refPtSuffix;
            histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T}-p_{T}^{ref} [GeV]",
                                                          nbinsabsrsp,absrspmin,absrspmax);
          }
          if (doetarsp&&dorefpt) {
            //etaRspRefPt
            //etaRspVsJetEtaRefPt
            hname=flavor[iflv]+"EtaRsp_"+jetEtaSuffix+"_"+refPtSuffix;
            histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";|#eta|-|#eta^{ref}|",
                                                          nbinsetarsp,etarspmin,etarspmax);
          }
          if (doetarsp&&dojetpt) {
            //etaRspJetPt
            //etaRspVsJetEtaJetPt
            hname=flavor[iflv]+"EtaRsp_"+jetEtaSuffix+"_"+jetPtSuffix;
            histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";#phi-#phi^{ref}",
                                                          nbinsetarsp,etarspmin,etarspmax);
          }
          if (dophirsp&&dorefpt) {
            //phiRspRefPt
            //phiRspVsJetEtaRefPt
            hname=flavor[iflv]+"PhiRsp_"+jetEtaSuffix+"_"+refPtSuffix;
            histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";#phi-#phi^{ref}",
                                                          nbinsphirsp,phirspmin,phirspmax);
          }
          if (dophirsp&&dojetpt) {
            //phiRspJetPt
            //phiRspVsJetEtaJetPt
            hname=flavor[iflv]+"PhiRsp_"+jetEtaSuffix+"_"+jetPtSuffix;
            histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";#phi-#phi^{ref}",
                                                          nbinsphirsp,phirspmin,phirspmax);
          }          
        }
      }
    }
  }

  // book y/pT histograms
  if (binspt.size()>=2&&binsy.size()>=2) {
    for (unsigned int iy=0;iy<binsy.size()-1;++iy) {

      string jetYSuffix=get_suffix("JetY",iy,binsy);

      for (unsigned int ipt=0;ipt<binspt.size()-1;ipt++) {

        string hname; float ptmin=binspt[ipt]; float ptmax=binspt[ipt+1];

        string jetPtSuffix=get_suffix("JetPt",ipt,binspt);
        string refPtSuffix=get_suffix("RefPt",ipt,binspt);

        for (unsigned int iflv=0;iflv<flavor.size();iflv++) {

          if (dojetpt) {
            //jetPtJetPt
            //jetPtVsJetYJetPt
            hname=flavor[iflv]+"JetPt_"+jetYSuffix+"_"+jetPtSuffix;
            histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T}",nbinspt,ptmin,ptmax);
          }
          if (dorefpt) {
            //refPtRefPt
            //refPtVsJetYRefPt
            hname=flavor[iflv]+"RefPt_"+jetYSuffix+"_"+refPtSuffix;
            histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T}^{ref}",
                                                          nbinspt,ptmin,ptmax);

            //jetPtRefPt
            //jetPtVsJetYRefPt
            hname=flavor[iflv]+"JetPt_"+jetYSuffix+"_"+refPtSuffix;
            histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T}",
                                                          3*nbinspt,0,3.0*ptmax);
          }
          if (dorelrsp&&dojetpt) {
            //relRspJetPt
            //relRspVsJetYJetPt
            hname=flavor[iflv]+"RelRsp_"+jetYSuffix+"_"+jetPtSuffix;
            histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T}/p_{T}^{ref}",
                                                          nbinsrelrsp,relrspmin,relrspmax);
          }
          if (dorelrsp&&dorefpt) {
            //relRspRefPt
            //relRspVsJetYRefPt
            hname=flavor[iflv]+"RelRsp_"+jetYSuffix+"_"+refPtSuffix;
            histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T}/p_{T}^{ref}",
                                                          nbinsrelrsp,relrspmin,relrspmax);
          }
          if (doabsrsp&&dojetpt) {
            //absRspJetPt
            //absRspVsJetYJetPt
            hname=flavor[iflv]+"AbsRsp_"+jetYSuffix+"_"+jetPtSuffix;
            histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T}-p_{T}^{ref} [GeV]",
                                                          nbinsabsrsp,absrspmin,absrspmax);
          }
          if (doabsrsp&&dorefpt) {
            //absRspRefPt
            //absRspVsJetYRefPt
            hname=flavor[iflv]+"AbsRsp_"+jetYSuffix+"_"+refPtSuffix;
            histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";p_{T}-p_{T}^{ref} [GeV]",
                                                          nbinsabsrsp,absrspmin,absrspmax);
          }
          if (doabsrsp&&dojetpt) {
            //absRspJetPt
            hname=flavor[iflv]+"AbsRsp_"+jetYSuffix+"_"+jetPtSuffix;
            histosPerAlg[hname]=fs->make<TH1F>(hname.c_str(),";|#y|-|#y^{ref}|",
                                                          nbinsabsrsp,absrspmin,absrspmax);
          }
        }
      }
    }
  }
  
  algHistos[alg] = histosPerAlg;

  cout << "DONE" << endl;
}

//______________________________________________________________________________
void jet_response_analyzer::getCollections(const edm::Event& iEvent, const edm::EventSetup& iSetup, vector<string> algs) {

  //edm::Selector s(edm::ProcessNameSelector("JRAP") && edm::ModuleLabelSelector("ak*"));
  //edm::Selector s(edm::ProcessNameSelector("JRAP"));
  //iEvent.getMany(s,algHandles);

  //iEvent.getManyByType(algHandles);
  //for(unsigned int ialg=0;ialg<algHandles.size();ialg++) {
    //assert ( algHandles[ialg].isValid() );
    //cout << algHandles[ialg].provenance()->moduleLabel() << endl;
  //}

  //This used to work in 53X. It no longer works in 62X.
  /*
  for(unsigned int ialg=0; ialg<algs.size(); ialg++) {
    edm::Selector s(edm::ProcessNameSelector("JRAP") && edm::ModuleLabelSelector(algs[ialg]));
    iEvent.getMany(s,algHandles);
    for(unsigned int ihandle=0;ihandle<algHandles.size();ihandle++) {
      assert ( algHandles[ihandle].isValid() );
      cout << algHandles[ihandle].provenance()->moduleLabel() << endl;
    }
  }
  */
  getterOfProducts_.fillHandles(iEvent, algHandles);
  for(unsigned int ihandle=0;ihandle<algHandles.size();ihandle++) {
    assert ( algHandles[ihandle].isValid() );
    //cout << algHandles[ihandle].provenance()->moduleLabel() << endl;
  }
}

//______________________________________________________________________________
string jet_response_analyzer::get_suffix(const string& varname,int ibin,const vector<double>& bins)
{
  stringstream ss; ss<<varname<<bins[ibin]<<"to"<<bins[ibin+1];
  return ss.str();
}


//______________________________________________________________________________
int jet_response_analyzer::get_index(float x,const vector<double>& binsx)
{
  for (unsigned int ix=0;ix<binsx.size()-1;ix++)
    if (x>=binsx[ix]&&x<binsx[ix+1]) return ix;
  return -1;
}


//______________________________________________________________________________
vector<string> jet_response_analyzer::get_flavors(bool noabsflavors)
{
   vector<string> flavor;
   flavor.push_back("");
   if (!noabsflavors) {
      flavor.push_back("uds_");
      flavor.push_back("c_");
      flavor.push_back("b_");
      flavor.push_back("g_");
      flavor.push_back("slc_");
      flavor.push_back("slb_");
   }
   else {
      flavor.push_back("uds_");
      flavor.push_back("udsbar_");
      flavor.push_back("c_");
      flavor.push_back("cbar_");
      flavor.push_back("b_");
      flavor.push_back("bbar_");
      flavor.push_back("g_");
      flavor.push_back("slc_");
      flavor.push_back("slb_");
      flavor.push_back("udscbg_");
   }
   return flavor;
}


//______________________________________________________________________________
TString jet_response_analyzer::pdgid_to_flavor_name(int pdgid)
{
   if(pdgid<=3 && pdgid>=1)
      return "uds_";
   else if(pdgid>=-3 && pdgid<=-1)
      return "udsbar_";
   else if(pdgid==4)
      return "c_";
   else if(pdgid==-4)
      return "cbar_";
   else if(pdgid==5)
      return "b_";
   else if(pdgid==-5)
      return "bbar_";
   else if(fabs(pdgid)==21)
      return "g_";
   else if(fabs(pdgid)==411 || fabs(pdgid)==413)
      return "slc_";
   else if(fabs(pdgid)==511 || fabs(pdgid)==513)
      return "slb_";
   else
      return "all_";
}

//______________________________________________________________________________
bool jet_response_analyzer::known_pdgid(int pdgid)
{
  int abspdgid=fabs(pdgid);
  if ((pdgid>=1&&pdgid<=3)||(pdgid<=-1&&pdgid>=-3)||(pdgid==4)||(pdgid==-4)||
      (pdgid==5)||(pdgid==-5)||(abspdgid==21)||(abspdgid==411)||(abspdgid==413)||
      (abspdgid==511)||(abspdgid==513))
    return true;
  else 
    return false;
}

//______________________________________________________________________________
void jet_response_analyzer::fill_histo(float value,float weight,float x,const vector<double>& binsx,
                                       string prefix,string varname)
{
  if (binsx.size()==0) return;
  int ix=get_index(x,binsx);
  if(ix<0) return;
  string hname = prefix+get_suffix(varname,get_index(x,binsx),binsx);
  if(algHistos[alg].find(hname)!=algHistos[alg].end()) algHistos[alg][hname]->Fill(value,weight);
}

//______________________________________________________________________________
void jet_response_analyzer::fill_histo(int pdgid,float value,float weight,float x,const vector<double>& binsx,
                                       string prefix,string varname, bool noabsflavors)
{
  if (binsx.size()==0) return;
  int ix=get_index(x,binsx);
  if(ix<0) return;
  if(!known_pdgid(pdgid)) return;
  int abspdgid=fabs(pdgid);
  //int iflv(-1);
  if(noabsflavors) {
    string hname = string(pdgid_to_flavor_name(pdgid))+prefix+get_suffix(varname,get_index(x,binsx),binsx);
    if(algHistos[alg].find(hname)!=algHistos[alg].end()) algHistos[alg][hname]->Fill(value,weight);
    if((abspdgid>=1&&abspdgid<=5)||abspdgid==21) {
      string hname2 = string("udscbg_")+prefix+get_suffix(varname,get_index(x,binsx),binsx);
      if(algHistos[alg].find(hname2)!=algHistos[alg].end()) algHistos[alg][hname2]->Fill(value,weight);
    }
  }
  else {
    string hname = string(pdgid_to_flavor_name(abspdgid))+prefix+get_suffix(varname,get_index(x,binsx),binsx);
    if(algHistos[alg].find(hname)!=algHistos[alg].end()) algHistos[alg][hname]->Fill(value,weight);
  }
}

//______________________________________________________________________________
void jet_response_analyzer::fill_histo(float value,float weight,float x,float y,const vector<double>& binsx,
                                       const vector<double>& binsy,string prefix,string varname1,string varname2)
{
  if (binsx.size()==0||binsy.size()==0) return;
  int ix=get_index(x,binsx);
  int iy=get_index(y,binsy);
  if(ix<0||iy<0) return;
  string hname = prefix+get_suffix(varname1,get_index(x,binsx),binsx)+"_"+get_suffix(varname2,get_index(y,binsy),binsy);
  if(algHistos[alg].find(hname)!=algHistos[alg].end()) algHistos[alg][hname]->Fill(value,weight);
}


//______________________________________________________________________________
void jet_response_analyzer::fill_histo(int pdgid,float value,float weight,float x,float y,
                const vector<double>& binsx,const vector<double>& binsy,
                string prefix,string varname1,string varname2, bool noabsflavors)
{
  if (binsx.size()==0||binsy.size()==0) return;
  int ix=get_index(x,binsx);
  int iy=get_index(y,binsy);
  if(ix<0||iy<0) return;
  if(!known_pdgid(pdgid)) return;
  int abspdgid=fabs(pdgid);
  //int iflv(-1);
  if (noabsflavors) {
    string hname = string(pdgid_to_flavor_name(pdgid))+prefix+get_suffix(varname1,get_index(x,binsx),binsx)+"_"+
                   get_suffix(varname2,get_index(y,binsy),binsy);
    if(algHistos[alg].find(hname)!=algHistos[alg].end()) algHistos[alg][hname]->Fill(value,weight);
    if((abspdgid>=1&&abspdgid<=5)||abspdgid==21) {
      string hname2 = string("udscbg_")+prefix+get_suffix(varname1,get_index(x,binsx),binsx)+"_"+
                      get_suffix(varname2,get_index(y,binsy),binsy);
      if(algHistos[alg].find(hname2)!=algHistos[alg].end()) algHistos[alg][hname2]->Fill(value,weight);
    }
  }
  else {
      string hname = string(pdgid_to_flavor_name(abspdgid))+prefix+get_suffix(varname1,get_index(x,binsx),binsx)+"_"+
                     get_suffix(varname2,get_index(y,binsy),binsy);
      if(algHistos[alg].find(hname)!=algHistos[alg].end()) algHistos[alg][hname]->Fill(value,weight);
    }
}

//______________________________________________________________________________
bool jet_response_analyzer::contains(const vector<string>& collection,const string& element)
{
  vector<string>::const_iterator it;
  for (it=collection.begin();it!=collection.end();++it)
    if ((*it)==element) return true;
  return false;
}


//______________________________________________________________________________
bool jet_response_analyzer::it_pileup(int itlow, int ithigh, vector<int>* npus)
{
  if(npus->at(1)>=itlow && npus->at(1)<=ithigh) return true;
  return false;
}


//______________________________________________________________________________
bool jet_response_analyzer::oot_pileup(int earlyootlow, int earlyoothigh, int lateootlow, int lateoothigh,
                                       vector<int>* npus)
{
  if(npus->at(0)>=earlyootlow && npus->at(0)<=earlyoothigh && 
     npus->at(2)>=lateootlow && npus->at(2)<=lateoothigh) return true;
  return false;
}


//______________________________________________________________________________
bool jet_response_analyzer::total_oot_pileup(int totalootlow, int totaloothigh, vector<int>* npus)
{
  if(npus->at(0)+npus->at(2)>=totalootlow && npus->at(0)+npus->at(2)<=totaloothigh) return true;
  return false;
}


//______________________________________________________________________________
bool jet_response_analyzer::pileup_cut(int itlow, int ithigh, int earlyootlow, int earlyoothigh, 
                int lateootlow, int lateoothigh, int totalootlow, int totaloothigh, 
                vector<int>* npus)
{
  if(it_pileup(itlow,ithigh,npus) && 
     total_oot_pileup(totalootlow,totaloothigh,npus) && 
     oot_pileup(earlyootlow,earlyoothigh,lateootlow,lateoothigh,npus)) return true;
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// define jet_response_analyzer as a plugin
////////////////////////////////////////////////////////////////////////////////

DEFINE_FWK_MODULE(jet_response_analyzer);

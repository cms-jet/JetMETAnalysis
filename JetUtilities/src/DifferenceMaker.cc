// This class libraries
#include "JetMETAnalysis/JetUtilities/interface/DifferenceMaker.hh"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// implement DifferenceMaker class
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
DifferenceMaker::DifferenceMaker() {
    cout << "WARNING::DifferenceMaker() Default constructor not recommended." << endl;
    basepath     = "./";
    filenames    = {"l5.root"};
    object       = "AbsRspVsRefPt:JetEta";
    flavorDiff   = true;
    flavor1      = {"g"};
    flavor2      = {"ud"};
    outputDir    = "./";
    outputFormat = {".png",".eps"};

    //
    // Check that the input path exists
    // Also check that the input path has a slash at the end of it
    // Otherwise we'd have to handle this on a case by case basis
    //
    if(basepath.IsNull()) basepath = string (gSystem->pwd())+"/";
    if(!basepath.EndsWith("/")) basepath+="/";

    //
    // Check that the output path ends with a slash and that the path exists
    //
    if(!outputDir.EndsWith("/")) outputDir+="/";
    if(!gSystem->OpenDirectory(outputDir)) gSystem->mkdir(outputDir);

    //
    // Try to figure out the variable along the x-axis
    //
    if(object.Contains("VsRefPt"))
        var = VARIABLES::refpt;

    objects_loaded = false;
}

//______________________________________________________________________________
DifferenceMaker::DifferenceMaker(CommandLine& cl) {
  	//
  	// evaluate command-line / configuration file options
  	//
    basepath     = cl.getValue<TString>  ("basepath");
    filenames    = cl.getVector<TString> ("filenames",           "l5.root");
    algs         = cl.getVector<TString> ("algs",                       "");
    object       = cl.getValue<TString>  ("object", "AbsRspVsRefPt:JetEta");
    flavorDiff   = cl.getValue<bool>     ("flavorDiff",               true);
    flavor1      = cl.getVector<TString> ("flavor1",                  "ud");
    flavor2      = cl.getVector<TString> ("flavor2",                   "g");
    outputDir    = cl.getValue<TString>  ("outputDir",                "./");
    outputFormat = cl.getVector<TString> ("outputFormat",    ".png:::.eps");
    bool help    = cl.getValue<bool>     ("help",                    false);

    if (help) {cl.print(); return;}
	if (!cl.partialCheck()) return;
  	cl.print();

    //
    // Check that the input path exists
    // Also check that the input path has a slash at the end of it
    // Otherwise we'd have to handle this on a case by case basis
    //
    if(basepath.IsNull()) basepath = string (gSystem->pwd())+"/";
    if(!basepath.EndsWith("/")) basepath+="/";

    //
    // Check that the output path ends with a slash and that the path exists
    //
    if(!outputDir.EndsWith("/")) outputDir+="/";
    if(!gSystem->OpenDirectory(outputDir)) gSystem->mkdir(outputDir);

    //
    // If there are multiple filenames then the user must specify the same number
    //  of algorithms this is to make sure that there is a mapping between a given
    //  file and a given algorithm name. If this isn't done then there may be a case
    //  where one of the algs doesn't exist in one of the files. You also can't assume
    //  that the same algorithms will be present in both files or that they contain
    //  unique algorithms. This isn't the case when getting the algorithms from the files
    //  themself. There the exact number of algorithms is present automatically.
    //
    if(filenames.size()>1) {
        try {
            mismatch_exception::areEqual(filenames.size(),algs.size());
        }
        catch (mismatch_exception& e) {
            std::cerr << "ERROR DifferenceMaker::DifferenceMaker If there are multiple input filenames "
                      << "then there must be an equal number of algorithms specified so that there is a "
                      << "one-to-one correspondence" << endl;
            std::cerr << e.what(string("filenames"),string("algs"));
            std::terminate();
        }
    }

    //
    // If there is a single filename and multiple algorithms then it is assumed that
    //  all of the algorithms come from the same file. Still, the file must be opened
    //  multiple times for this program to work (and to be flexible). Thus, the filename
    //  vector must have the same size as the algs vector.
    //
    if(filenames.size()==1 && algs.size()>1)
        filenames = vector<TString>(algs.size(),filenames[0]);

    //
    // Check that if flavorDiff is set to true neither of the flavors are null
    //
    if(flavorDiff && (flavor1.empty()||flavor2.empty())) {
        cout << "ERROR DifferenceMaker::DifferenceMaker If flavorDiff is true then both flavors "
             << "must be non-null" << endl;
        std::terminate();
    }

    //
    // Check that is flavorDiff and the flavor container sizes are >1 that
    //  their sizes are equal
    //
    if(flavorDiff && (flavor1.size()>1 || flavor2.size()>1)
       && (flavor1.size()!=flavor2.size())) {
        cout << "ERROR DifferenceMaker::DifferenceMaker If flavorDiff is true and the flavor "
             << "containers have size >1, then they must be of equal size." << endl;
        std::terminate();
    }

    //
    // Check if all the entires in flavor1 or flavor2 are equal
    //
    flavor1AreEqual = find_if(flavor1.begin() + 1, flavor1.end(),
                              bind1st(not_equal_to<TString>(),
                                      flavor1.front())) == flavor1.end();
    flavor2AreEqual = find_if(flavor2.begin() + 1, flavor2.end(),
                              bind1st(not_equal_to<TString>(),
                                      flavor2.front())) == flavor2.end();

    //
    // Check that if algDiff is true there are either 0 or 2 algorithms listed
    // If 0 algorithms lists, then the check for two algorithms must be performed
    //  when getting algs from the file itself
    //
    //if(algDiff && (algs.size()!=0 && algs.size()!=2)) {
    //    cout << "ERROR DifferenceMaker::DifferenceMaker When algDiff is selected "
    //         << "the user must specify eithere 0 or 2 algorithms" << endl;
    //    std::terminate();
    //}

    //
    // Try to figure out the variable along the x-axis
    //
    if(object.Contains("VsRefPt"))
        var = VARIABLES::refpt;

    objects_loaded = false;
}

//______________________________________________________________________________
void DifferenceMaker::makeDifference() {
    //
    // This function is intentionally sparce. The intention is that this function
    //  will grow as more functionallity is added to this class (i.e. the difference
    //  between more types of objects).
    //
    if(flavorDiff)
        makeFlavorDifference();
}

//______________________________________________________________________________
void DifferenceMaker::makeFlavorDifference() {
    openInputFiles();
    openOutputFile();
    openCanvas();
    loopOverFilesAndDirectories();
    loopOverFilesAndAlgorithms();
    closeFiles();
}

//______________________________________________________________________________
void DifferenceMaker::openInputFiles() {
    //
    // Open the file containing the input objects
    //
    for(unsigned int ifile=0; ifile<filenames.size(); ifile++) {
        ifiles.push_back(TFile::Open(basepath+filenames[ifile],"READ"));
        if(ifiles.back() == 0) {
            cout << "ERROR DifferenceMaker::openInputFiles() Could not open the file "
                 << basepath << filenames[ifile] << endl;
            std::terminate();
        }
    }
}

//______________________________________________________________________________
void DifferenceMaker::openOutputFile() {
    //
    // Open/create the output directory and file
    //
    TString ofname;
    if(flavorDiff) {
        if(flavor1.size()==1) {
            ofname = Form("%s/%sMinus%s_%s_%s.root",outputDir.Data(),
                          flavor1[0].Data(),flavor2[0].Data(),object.Data(),
                          JetInfo::ListToString(algs,"_").Data());
        }
        else if(flavor1AreEqual && !flavor2AreEqual) {
            ofname = Form("%s/%sMinusX_%s_%s.root",outputDir.Data(),
                          flavor1[0].Data(),object.Data(),
                          JetInfo::ListToString(algs,"_").Data());
        }
        else if(!flavor1AreEqual && flavor2AreEqual) {
            ofname = Form("%s/XMinus%s_%s_%s.root",outputDir.Data(),
                          flavor2[0].Data(),object.Data(),
                          JetInfo::ListToString(algs,"_").Data());
        }
        else if(!flavor1AreEqual && !flavor2AreEqual) {
            ofname = Form("%s/XMinusY_%s_%s.root",outputDir.Data(),
                          object.Data(),JetInfo::ListToString(algs,"_").Data());
        }
    }
    ofile = TFile::Open(ofname,"RECREATE");
}

//______________________________________________________________________________
void DifferenceMaker::openCanvas() {
    TString name;

    if(flavor1.size()==1) {
        name = Form("%sMinus%s_%s_%s",flavor1[0].Data(),
                    flavor2[0].Data(),object.Data(),
                    JetInfo::ListToString(algs,"_").Data());
    }
    else if(flavor1AreEqual && !flavor2AreEqual) {
        name = Form("%sMinusX_%s_%s",flavor1[0].Data(),
                    object.Data(),JetInfo::ListToString(algs,"_").Data());
    }
    else if(!flavor1AreEqual && flavor2AreEqual) {
        name = Form("XMinus%s_%s_%s",flavor2[0].Data(),
                    object.Data(),JetInfo::ListToString(algs,"_").Data());
    }
    else if(!flavor1AreEqual && !flavor2AreEqual) {
        name = Form("XMinusY_%s_%s",object.Data(),
                    JetInfo::ListToString(algs,"_").Data());
    }

    bool flavorDiffSingleAlg = ifiles.size()==1 && algs.size()==1 && flavorDiff;
    int npavelabels = 2 + algs.size();
    if(flavorDiffSingleAlg) npavelabels+=flavor1.size();
    double paveymax = 1-0.08-0.035*(1-0.08-0.01)+0.025;//0.89;
    double paveymin = paveymax - (npavelabels)*0.055;

    TH1D* frame = new TH1D();
    frame->GetXaxis()->SetLimits(20.0,5000.0);
    frame->GetXaxis()->SetMoreLogLabels();
    frame->GetXaxis()->SetNoExponent();
    if(algs.size()==1 && algs[0].Contains("calo",TString::kIgnoreCase))
        frame->GetYaxis()->SetRangeUser(-0.01,0.2);
    else if(algs.size()==1 && algs[0].Contains("pf",TString::kIgnoreCase))
       frame->GetYaxis()->SetRangeUser(-0.01,0.2);//0.08);
    else
        frame->GetYaxis()->SetRangeUser(0.0,0.2);
    frame->GetXaxis()->SetTitle("p_{T}^{REF} (GeV)");
    TString yAxisTitle;
    if(flavorDiff) {
        if(flavor1.size()==1)
            yAxisTitle = Form("R_{%s}-R_{%s}",flavor1[0].Data(),flavor2[0].Data());
        else if(flavor1AreEqual && !flavor2AreEqual)
            yAxisTitle = Form("R_{%s}-R_{X} {X=%s}",flavor1[0].Data(),
                              JetInfo::ListToString(flavor2,",").Data());
        else if(!flavor1AreEqual && flavor2AreEqual)
            yAxisTitle = Form("R_{X}-R_{%s} {X=%s}",flavor2[0].Data(),
                              JetInfo::ListToString(flavor1,",").Data());
        else if(!flavor1AreEqual && !flavor2AreEqual)
            yAxisTitle = Form("R_{X}-R_{Y} {X=%s and Y=%s}",
                              JetInfo::ListToString(flavor1,",").Data(),
                              JetInfo::ListToString(flavor2,",").Data());
    }
    frame->GetYaxis()->SetTitle(yAxisTitle);
    canvases_legends.push_back(make_pair(tdrCanvas(name,frame,14,11,true),
                               tdrLeg(0.38,paveymin,0.78,paveymax)));
                               //tdrLeg(0.38,0.67,0.78,0.92)));
    canvases_legends.back().first->GetPad(0)->SetLogx();

    if(object.Contains("_JetEta",TString::kIgnoreCase)) {
       if(object.Contains("0to1.3"))
          canvases_legends.back().second->AddEntry((TObject*)0,"|#eta|<1.3","");
       else if(object.Contains("1.3to2.5"))
          canvases_legends.back().second->AddEntry((TObject*)0,"1.3<|#eta|<2.5","");
       else if(object.Contains("1.6to2.5"))
          canvases_legends.back().second->AddEntry((TObject*)0,"1.6<|#eta|<2.5","");
       else if(object.Contains("2.5to3"))
          canvases_legends.back().second->AddEntry((TObject*)0,"2.5<|#eta|<3","");
       else if(object.Contains("3to5"))
          canvases_legends.back().second->AddEntry((TObject*)0,"3<|#eta|<5","");
    }
    canvases_legends.back().second->AddEntry((TObject*)0,"Parton Flavor",""); //A.K.A Physics Definition
    //leg->AddEntry((TObject*)0,"Algorithmic Flavor","");
    if(flavorDiffSingleAlg) {
        canvases_legends.back().second->AddEntry((TObject*)0,ji->get_legend_title(algs[0]),"");
    }
}

//______________________________________________________________________________
void DifferenceMaker::loopOverFilesAndDirectories() {
    if (algs.size()==0) {
        for(vector<TFile*>::iterator it=ifiles.begin(); it!=ifiles.end(); it++) {
            TIter nextDir((*it)->GetListOfKeys());
            TKey* dirKey(0);
            while ((dirKey=(TKey*)nextDir())) {
                if (strcmp(dirKey->GetClassName(),"TDirectoryFile")!=0) continue;
                algs.push_back(dirKey->GetName());
                ifiles.insert(it,(*it));
                it++;
            }
        }
    }
}

//______________________________________________________________________________
void DifferenceMaker::getGraphs(TDirectoryFile* idir) {
    if(objects_loaded) {
        gl.erase(gl.begin(),gl.end());
    }

    if(flavorDiff) {
        //
        // loop over the flavors vectors
        //
        for(unsigned int iflavor=0; iflavor<flavor1.size(); iflavor++) {
            cout << "\tGetting object " << flavor1[iflavor] << object << " ... ";
            TGraph* tmp1 = (TGraphErrors*)idir->Get((flavor1[iflavor]+object).Data()); 
            cout << "DONE" << endl;
            cout << "\tGetting object " << flavor2[iflavor] << object << " ... ";
            TGraph* tmp2 = (TGraphErrors*)idir->Get((flavor2[iflavor]+object).Data()); 
            cout << "DONE" << endl;
            gl.push_back(make_pair(tmp1,tmp2));
            objects_loaded = true;
            if(gl[iflavor].first==0 || gl[iflavor].second==0) {
                cout << "ERROR DifferenceMaker::getGraphs one or more of the objects in gl "
                     << "are NULL" << endl;
                std::terminate();
            }
            if(TString(gl[iflavor].first->ClassName()).CompareTo("TGraphErrors")!=0) {
                cout << "ERROR DifferenceMaker::getGraphs the first object in gl does not "
                     << "inherit from TGraphErrors" << endl;
                std::terminate();
            }
            if(TString(gl[iflavor].second->ClassName()).CompareTo("TGraphErrors")!=0) {
                cout << "ERROR DifferenceMaker::getGraphs the second object in gl does not "
                     << "inherit from TGraphErrors" << endl;
                std::terminate();
            }
        }
        if(gl.size()==0) {
            cout << "ERROR DifferenceMaker::getGraphs did not find any objects matching "
                 << "the patern {" << JetInfo::ListToString(flavor1,",") << ";"
                 << JetInfo::ListToString(flavor2,",") << "}" <<  object << endl;
            std::terminate();
        }
    }

    //
    // Slightly redudant, but we want to catch all cases
    //
    if(gl.size()<1) {
        cout << "ERROR DifferenceMaker::getGraphs did not find any objects" << endl;
        std::terminate();
    }
}

//______________________________________________________________________________
void DifferenceMaker::makeDifferenceGraph(double epsilon_point) {
    for(unsigned int ig=0; ig<gl.size(); ig++) {
        int offset_first = 0, offset_second = 0;
        int N_ = min(gl[ig].first->GetN(),gl[ig].second->GetN());
        int ibin = 0;
        vector<double> x(N_,0.0), y(N_,0.0), exl(N_,0.0), exh(N_,0.0), ey(N_,0.0);
        for(int i=0; i<N_; i++) {
            double diff_x = gl[ig].first->GetX()[i+offset_first]-gl[ig].second->GetX()[i+offset_second];
            if(diff_x<-epsilon_point*gl[ig].second->GetX()[i+offset_second]) {offset_first++; i--; continue;}
            else if(diff_x>epsilon_point*gl[ig].first->GetX()[i+offset_first]) {offset_second++; i--; continue;}

            x[i] = (gl[ig].first->GetX()[i+offset_first]+gl[ig].second->GetX()[i+offset_second])/2.0;
            y[i] = (gl[ig].first->GetY()[i+offset_first]-gl[ig].second->GetY()[i+offset_second]);
            ey[i] = (TMath::Sqrt(TMath::Power(gl[ig].first->GetEY()[i+offset_first],2)+TMath::Power(gl[ig].second->GetEY()[i+offset_second],2)));
            if(var == VARIABLES::refpt)
                ibin = JetInfo::getBin(x[i],vpt,NPtBins);
            else {
                cout << "ERROR DifferenceMaker::makeDifferenceGraph cannot determine the "
                     << "variable on the x-axis" << endl;
                ibin = 0;
            }
            exl[i] = x[i]-vpt[ibin];
            exh[i] = vpt[ibin+1]-x[i];
            //cout << gl[ig].first->GetName() << " minus " << gl[ig].second->GetName() << endl;
            //cout << "i=" << i << " ibin=" << ibin << " x=" << x[i] << " y=" << y[i] << " ey=" << ey[i]
            //     << " exl=" << exl[i] << " exh=" << exh[i] << endl;
            //cout << "\toffset_first=" << offset_first << " offset_second=" << offset_second << endl;
        }
        TGraphAsymmErrors* diff = new TGraphAsymmErrors(N_,&x.at(0),&y.at(0),&exl.at(0),&exh.at(0),&ey.at(0),&ey.at(0));
        if(flavorDiff) {
           TString name = Form("%sMinus%s_%s_%s",flavor1[ig].Data(),flavor2[ig].Data(),
                               object.Data(),JetInfo::ListToString(algs,"_").Data());
           diff->SetNameTitle(name,name);
        }
        glDiff.push_back(diff);
    }
}

//______________________________________________________________________________
void DifferenceMaker::drawDifference(int count) {
    for(unsigned int ig=0; ig<glDiff.size(); ig++) {
        if(ifiles.size()==1 && algs.size()==1 && flavorDiff) {
            if(shapesMore[ig]==kFullDiamond || shapesMore[ig]==kOpenDiamond) {
                glDiff[ig]->SetMarkerSize(glDiff[ig]->GetMarkerSize()*1.4);
            }
            tdrDraw(glDiff[ig],"P",shapesMore[ig],colorsMore[ig]);
            canvases_legends.back().second->AddEntry(glDiff[ig],Form("R_{%s}-R_{%s}",
                                                     flavor1[ig].Data(),flavor2[ig].Data()),
                                                     "pl");
        }
        else {
            tdrDraw(glDiff[ig],"P",shapes[count],colors[count]);
            //ji = make_unique<JetInfo>(alg);
            canvases_legends.back().second->AddEntry(glDiff[ig],ji->get_legend_title(algs[count]),"pl");
            //canvases_legends.back().second->AddEntry(gl[2],"Anti-k_{T} R=0.5, Calo","pl");
            //canvases_legends.back().second->AddEntry(gl[2],"Anti-k_{T} R=0.5, PF","pl"); 
        }  
    }
}

//______________________________________________________________________________
void DifferenceMaker::writeToFile() {
    ofile->cd();
    for(auto it : glDiff) {
       it->Write();
    }
    for(auto it : canvases_legends) {
        it.first->Write();
        for(unsigned int iformat=0; iformat<outputFormat.size(); iformat++) {
            it.first->SaveAs(outputDir+it.first->GetName()+outputFormat[iformat]);
        }
    }
}

//______________________________________________________________________________
void DifferenceMaker::loopOverFilesAndAlgorithms() {

    //
    // Loop over the files/algorithms
    //
    for(unsigned int ifile=0; ifile<ifiles.size(); ifile++) {
        TDirectoryFile* idir = (TDirectoryFile*)ifiles[ifile]->GetDirectory(algs[ifile]);

        cout << ifiles[ifile]->GetName() << " ... " << endl;
        cout << "\t" << algs[ifile] << " ... " << endl;

        //
        // Load the input graphs from input files
        //
        getGraphs(idir);

        //
        // Calculate the difference between the graphs in the first two places in the gl vector
        // The put the difference graph into the third index
        //
        makeDifferenceGraph();

        //
        // Format and draw the difference graph
        //
        drawDifference(ifile);

        cout << "\t" << algs[ifile] << " is DONE" << endl;
        cout << ifiles[ifile]->GetName() << " is DONE" << endl;
    }

    //
    // Put the last formating touches on the canvas
    //
    canvases_legends.back().second->Draw("SAME");

    //
    // Write the canvas to a file
    // This canvas should now have all of the algorithms on a single canvas
    //
    writeToFile();
}

//______________________________________________________________________________
void DifferenceMaker::closeFiles() {
    cout << "Closing the output file ...";
    ofile->Close();
    cout << "DONE" << endl;
    cout << "Closing the input files ... ";
    for(unsigned int ifile=0; ifile<filenames.size(); ifile++) {
        ifiles[ifile]->Close();
    }
    cout << "DONE" << endl;
}

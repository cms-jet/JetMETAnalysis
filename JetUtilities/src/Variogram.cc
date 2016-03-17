// This class libraries
#include "JetMETAnalysis/JetUtilities/interface/Variogram.hh"

////////////////////////////////////////////////////////////////////////////////
// implement Variogram class
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
Variogram::Variogram(TH2D* data) : data_points(data) {
    TH1D* tmp = getZdistributionFromData(true);
    if(!tmp) {
        cout << "ERROR::Variogram::Could not initialize z_distribution in the constructor." << endl;
        std::terminate();
    }
    setZdistribution(tmp);
    pdist();
}

//______________________________________________________________________________
void Variogram::dist_to_vector_from_squareform(bool debug) {
    const double *cit;
    double *M = pairwise_distances->GetMatrixArray();
    int n = pairwise_distances->GetNrows();
    for (int i = 0; i < n - 1; i++) {
        cit = M + (i * n) + i + 1;
        for (int j = i + 1; j < n; j++, cit++) {
            pairwise_distances_vector.push_back(*cit);
        }
    }
    if(pairwise_distances_vector.size()!=(unsigned)n*(n-1)/2) {
        cout << "WARNING::Variogram::dist_to_vector_from_squareform::pairwise_distances_vector size not equal to n(n-1)/2" << endl;
    }
    if(debug) {
        cout << "pairwise_distances_vector array(" << endl;
        for(unsigned int i=0; i<pairwise_distances_vector.size(); i++) {
            cout << pairwise_distances_vector[i] << endl;
        }
        cout << ")" << endl;
    }
}

//______________________________________________________________________________
void Variogram::getNormalQuantiles() {
    Double_t pk;
    for (UInt_t k=1; k<=sorted_z_values.size(); k++){
       pk = (k-0.375)/(sorted_z_values.size()+0.25);
       normal_quantiles.push_back(TMath::NormQuantile(pk));
    }
    Double_t prob[]={0.25, 0.75};
    Double_t x[2];
    Double_t y[2];
    TMath::Quantiles(sorted_z_values.size(), 2, &sorted_z_values.at(0), y, prob, kTRUE);
    x[0] = TMath::NormQuantile(0.25);
    x[1] = TMath::NormQuantile(0.75);
    fXq1=x[0]; fXq2=x[1]; fYq1=y[0]; fYq2=y[1];
}

//______________________________________________________________________________
void Variogram::getSemivariogram(vector<int> &lags, int tol, bool pedestalSub) {
    // create a list of lists of indices of points having the ~same lag
    vector<vector<pair<int,int> > > index;
    vector<double> h;
    for(unsigned int ilag=0; ilag<lags.size(); ilag++) {
        index.push_back(lagindices(lags[ilag],tol));
        h.push_back(lags[ilag]);
    }
    for(unsigned int indices=0; indices<index.size(); indices++) {
        v.push_back(semivariance(index[indices]));
        //cout << "lag = " << h[indices] << " sv = " << v.back() << endl;
    }
    if(!pedestalSub) {
        sv = v;
    }
    else {
        for(unsigned int i=0; i<v.size(); i++) {
            sv.push_back(v[i]-v[0]);
        }
    }
    sill = getSill();

    if(h.size()!=sv.size()) {
        cout << "WARNING::Variogram::getSemivariogram::The number of lags and the number of semivariances are not equal." << endl;
    }

    semivariogram = new TGraph(sv.size(),&h.at(0),&sv.at(0));
}

//______________________________________________________________________________
vector<pair<int,int> > Variogram::lagindices(int lag, int tol) {
    vector<pair<int,int> > indices;
    for(int irow = 0; irow < pairwise_distances->GetNrows(); irow++) {
        for(int icol = 0; icol < pairwise_distances->GetNcols(); icol++) {
            if(icol <= irow) continue; //take out the repeated elements,since p is a *symmetric* distance matrix
            //grab the coordinates in a given range: lag +/- tolerance
            if( ((*pairwise_distances)(irow,icol) >= lag-tol) && ((*pairwise_distances)(irow,icol) < lag+tol) ) {
                indices.push_back(make_pair(irow,icol));
            }
        }
    }
    return indices;
}

//______________________________________________________________________________
TH1D* Variogram::getZdistributionFromData(bool get_z_values, int nbins) {
    pair<double,double> hrange = make_pair(data_points->GetMinimum(0.00000001),
                                           data_points->GetMaximum());
    TH1D* ret = new TH1D("z_distribution","Values From Z Axis",nbins,hrange.first,hrange.second);
    double content(0);
    for(int ibinX=1; ibinX<=data_points->GetNbinsX(); ibinX++) {
        for(int ibinY=1; ibinY<=data_points->GetNbinsY(); ibinY++) {
            content = data_points->GetBinContent(ibinX,ibinY);
            if(content==0) continue;
            ret->Fill(content);
            if(get_z_values) {
                sorted_z_values.push_back(content);
                sorted_points.push_back(point(data_points->GetXaxis()->GetBinCenter(ibinX),
                                        data_points->GetYaxis()->GetBinCenter(ibinY),content));
            }
        }
    }
    if(get_z_values) {
        std::sort(sorted_z_values.begin(),sorted_z_values.end());
        getNormalQuantiles();
        std::sort(sorted_points.begin(),sorted_points.end(),point_sort);
    }
    return ret;
}

//______________________________________________________________________________
void Variogram::pdist() {
    gSystem->Load("libMatrix.so");
    pairwise_distances = new TMatrixD(sorted_points.size(),sorted_points.size());
    cout << "sorted_points.size() = " << sorted_points.size() << endl;
    cout << "pairwise_distances->GetNrows() = " << pairwise_distances->GetNrows() << endl;
    cout << "pairwise_distances->GetNcols() = " << pairwise_distances->GetNcols() << endl;
    double dist_betw = 0;
    for(unsigned int i = 0; i < sorted_points.size(); i++) {
        for(unsigned int j = 0; j < i; j++) {
            dist_betw = sqrt(pow((sorted_points[i].X()-sorted_points[j].X()),2) + pow(sorted_points[i].Y()-sorted_points[j].Y(),2));
            (*pairwise_distances)(i,j) = dist_betw;
        }
    }
    TMatrixD mt = *pairwise_distances;
    mt.T();
    *pairwise_distances = (*pairwise_distances)+mt;
}

//______________________________________________________________________________
TCanvas* Variogram::plotSemivariogram(vector<int> &lags, int tol, string model, bool pedestalSub, map<string,double> *param) {
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(0);

    TH1D* frame = new TH1D();
    frame->GetXaxis()->SetLimits(lags.at(0),lags.back());
    frame->GetXaxis()->SetTitle("Lag Distance");
    frame->GetXaxis()->SetTitleOffset(1.0);
    frame->GetYaxis()->SetRangeUser(*std::min_element(sv.begin(),sv.end()),*std::max_element(sv.begin(),sv.end())*1.2);
    if(pedestalSub)
        frame->GetYaxis()->SetTitle(Form("Semivariance (pedestal of %f removed)",v[0]));
    else
        frame->GetYaxis()->SetTitle("Semivariance");
    frame->GetYaxis()->SetTitleOffset(1.75);

    TCanvas* c = tdrCanvas("Semivariogram",frame,14,0,true);
    c->cd(0);

    //Draw model
    TF1* smodel(0);
    if(model!="") {
        size_t pos = model.find(':'); assert(pos!=string::npos);
        string function_as_str = model.substr(0,pos); model = model.substr(pos+1);
        string a_as_str = model;
        if(function_as_str.find("spherical")!=string::npos) {
            smodel = new TF1("sphericalModelFunction",this,&Variogram::spherical,lags.at(0),lags.back(),2,"Variogram","spherical");
            smodel->SetParNames("a","sill"); //a=piecewise matching point
            std::stringstream ssa; ssa<<a_as_str; double a; ssa>>a;
            smodel->SetParameters(a,sill);
            semivariogram->Fit(smodel,"RS");
            smodel->Draw("same");
        }
    }

    //Draw semivariogram
    tdrDraw(semivariogram,"PL",kFullCircle,kBlack,kSolid,kBlack);

    //Draw sill line
    TLine* line = new TLine(lags[0],sill,lags.back(),sill);
    line->SetLineStyle(2);
    line->SetLineColor(kBlack);
    line->Draw("same");

    //Draw sill text
    TLatex t;
    t.SetTextFont(42);
    t.SetTextSize(0.042);
    t.DrawLatex(tol*2,sill*1.025,Form("%0.3f",sill));

    if(model!="") {
        c->Modified(); c->Update();
        TPaveStats *st = (TPaveStats*)semivariogram->GetListOfFunctions()->FindObject("stats");
        if(st) {
            st->SetX1NDC(st->GetX1NDC()-0.06); //new x start position
            st->SetX2NDC(st->GetX2NDC()-0.06); //new x end position
            double stats_height = st->GetY2NDC()-st->GetY1NDC();
            double starting_y = 0.16;
            st->SetY1NDC(starting_y); //new y start position
            st->SetY2NDC(stats_height+starting_y); //new y end position
            c->Modified();
        }
    }

    if(param) {
        param->insert(make_pair("Smp. Var.",sill));
        if(model!="") {
            param->insert(make_pair("sill (model)",smodel->GetParameter(1)));
            param->insert(make_pair("a (model)",smodel->GetParameter(0)));
        }
    }

    return c;
}

//______________________________________________________________________________
double Variogram::semivariance(vector<pair<int,int> > &indices) {
    // take the squared difference between the values of the variable of interest
    vector<double> z;
    for(unsigned int idx=0; idx<indices.size(); idx++) {
        z.push_back(pow(sorted_points[indices[idx].first].Z()-sorted_points[indices[idx].second].Z(),2));
    }
    // the semivariance is half the mean squared difference
    return TMath::Mean(z.size(),&z.at(0))/2.0;
}

//______________________________________________________________________________
void Variogram::showScattergrams(int lag, int tol) {

}

//______________________________________________________________________________
void Variogram::verifyNormality() {
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(0);

    pair<double,double> hrange = make_pair(data_points->GetMinimum(0.00000001),
                                           data_points->GetMaximum());

    TH1D* frame = new TH1D();
    frame->GetXaxis()->SetLimits(hrange.first,hrange.second);
    frame->GetXaxis()->SetTitle("Correction");
    frame->GetYaxis()->SetRangeUser(z_distribution->GetMinimum(),z_distribution->GetMaximum()*1.2);
    frame->GetYaxis()->SetTitle("Correction Multiplicity");

    TCanvas* c = tdrCanvas("Z Distribution",frame,14,0,true);
    c->cd(0);
    tdrDraw(z_distribution,"EP",kFullCircle,kBlack,kSolid,kBlack);

    TF1* fit = new TF1("fit","gausn",hrange.first,hrange.second);
    fit->SetParNames("N","mu","sigma");
    TFitResultPtr r = z_distribution->Fit(fit,"+RS");
    double chi2  = r->Chi2();
    int NDF      = fit->GetNDF();
    double N     = r->Parameter(0);
    double mu    = r->Parameter(1);
    double sigma = r->Parameter(2);
   
    TLegend* l = tdrLeg(0.61,0.6,0.86,0.88);
    l->SetHeader("Fit Results:");
    l->AddEntry((TObject*)0,Form("N = %.2f",N),"");
    l->AddEntry((TObject*)0,Form("#mu = %.2f",mu),"");
    l->AddEntry((TObject*)0,Form("#sigma = %.2f",sigma),"");
    l->AddEntry((TObject*)0,Form("#Chi^{2} = %.2f",chi2),"");
    l->AddEntry((TObject*)0,Form("N.D.F. = %i",NDF),"");
    l->Draw("same");


    if(normal_quantiles.size()>0) {
        //
        // Draw the probabilty vs quantile graph
        //
        z_vs_quantile = new TGraph(sorted_z_values.size(),&normal_quantiles.at(0),&sorted_z_values.at(0));
    
        TH1D* frame2 = new TH1D();
        frame2->GetXaxis()->SetLimits(normal_quantiles.at(0),normal_quantiles.back());
        frame2->GetXaxis()->SetTitle("Standard Normal Quantiles");
        frame2->GetYaxis()->SetRangeUser(sorted_z_values.at(0),sorted_z_values.back());
        frame2->GetYaxis()->SetTitle("Sorted Correction Values");
    
        TCanvas* c2 = tdrCanvas("Z Quantiles",frame2,14,0,true);
        c2->cd(0);
    
        tdrDraw(z_vs_quantile,"P",kFullCircle,kBlack);

        TF1* linear_fit = new TF1("linear_fit","pol1",normal_quantiles.at(0),normal_quantiles.back());
        linear_fit->SetLineColor(kRed);
        linear_fit->SetLineStyle(2);
        TFitResultPtr rr = z_vs_quantile->Fit(linear_fit,"+RS");

        TLegend* ll = tdrLeg(0.55,0.16,0.80,0.44);
        ll->SetHeader("Fit Results:");
        ll->AddEntry((TObject*)0,Form("Slope = %.2f",rr->Parameter(1)),"");
        ll->AddEntry((TObject*)0,Form("Intercept = %.2f",rr->Parameter(0)),"");
        ll->AddEntry((TObject*)0,Form("#Chi^{2} = %.2f",rr->Chi2()),"");
        ll->AddEntry((TObject*)0,Form("N.D.F. = %i",linear_fit->GetNDF()),"");
        ll->Draw("same");
    }
}

//______________________________________________________________________________
double Variogram::spherical(double *x, double *p) {
    auto a = p[0];
    auto c = p[1];

    if(x[0]<=a) return c*( 1.5*(x[0]/a) - 0.5*pow((x[0]/a),3.0) );
    else if (x[0]>a) return c+x[0]*0;
    else return -1;
}

/*
//______________________________________________________________________________
vector<double> Variogram::calc_uniform_order_statistic_medians(vector<double> &x):
    int N = x.size()
    vector<double> osm_uniform(N,0.0);
    osm_uniform.back() = pow(0.5,1.0/N);
    osm_uniform[0] = 1 - osm_uniform.back();
    for(unsigned int i=2; i<N; i++) {
      osm_uniform[i-1] = (i - 0.3175) / (N + 0.365);
    }
    return osm_uniform
*/
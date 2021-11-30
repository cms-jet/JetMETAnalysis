////////////////////////////////////////////////////////////////////////////////
//
// HistogramUtilities
// ------------------
//
//            04/07/2016 Alexx Perloff          <alexx.stephen.perloff@cern.ch>
////////////////////////////////////////////////////////////////////////////////

#include "JetMETAnalysis/JetUtilities/interface/HistogramUtilities.hh"

////////////////////////////////////////////////////////////////////////////////
// implementation of functions
////////////////////////////////////////////////////////////////////////////////

namespace HistUtil {
    //______________________________________________________________________________
    // A routine that returns the string given the HistogramMetric 
    string getHistogramMetricString(HistogramMetric type){
        if      (type == mu_h)    return "mu_h";
        else if (type == RMS_h)   return "RMS_h";
        else if (type == mu_f)    return "mu_f";
        else if (type == mpv)     return "mpv";
        else if (type == RMS_f)   return "RMS_f";
        else if (type == sigma_f) return "sigma_f";
        else if (type == median)  return "median";
        cout << "ERROR in HistUtil::getHistogramMetricString type=" << type << " not defined" << endl;
        return "none";
    }//getHistogramMetricString

    //______________________________________________________________________________
    // A routine that returns the HistogramMetric type given the string
    HistogramMetric getHistogramMetricType(string type){
        if      (type.find("mu_h")!=string::npos)    return mu_h;
        else if (type.find("RMS_h")!=string::npos)   return RMS_h;
        else if (type.find("mu_f")!=string::npos)    return mu_f;
        else if (type.find("mpv")!=string::npos)     return mpv;
        else if (type.find("RMS_f")!=string::npos)   return RMS_f;
        else if (type.find("sigma_f")!=string::npos) return sigma_f;
        else if (type.find("median")!=string::npos)  return median;
        else cout << "ERROR in HistUtil::getHistogramMetricType type=" << type << " not defined. Returning none." << endl;
        return none;
    }//getHistogramMetricType

    //______________________________________________________________________________
    // A routine that returns the associated metric
    HistogramMetric getAssociatedHistogramMetric(HistogramMetric type) {
        if      (type == mu_h)                     return RMS_h;
        else if (type == RMS_h)                    return mu_h;
        else if (type == mu_f || type == mpv)      return RMS_f;
        else if (type == RMS_f || type == sigma_f) return mu_f;
        else if (type == median )                  return RMS_h;
        else cout << "ERROR in HistUtil::getAssociatedHistogramMetric type=" << type << " not defined. Returning none." << endl;
        return none;
    }//getAssociatedHistogramMetric

    //______________________________________________________________________________
    // A routine that returns a given HistogramMetric
    pair<double,double> getHistogramMetric1D(HistogramMetric type, TH1* hist, double fallback_threshold, bool verbose) {
        if(!hist) {
            cout << "ERROR in HistUtil::getHistogramMetric1D the histogram is not defined (" << hist << ")" << endl;
            return make_pair(0.0,0.0);
        }

        if      (type == mu_h)  return make_pair(hist->GetMean(),hist->GetMeanError());
        else if (type == RMS_h) return make_pair(hist->GetRMS(),hist->GetRMSError());
        else if (type == mu_f || type == mpv || type == RMS_f || type == sigma_f) {
            TF1* f = dynamic_cast<TF1*>(hist->GetListOfFunctions()->Last());

            // Fallback if the fit doesn't exist or if the fit is so bad its error doesn't meet the fallback threshold
            if(!f) {
                if(verbose) cout << "WARNING in HistUtil::getHistogramMetric1D can't find the last function in hist" << endl
                                 << "\tFalling back to mu_h/RMS_h.";
                if(type == mu_f || type == mpv)
                   return getHistogramMetric1D(mu_h,hist);
                if(type == RMS_f || type == sigma_f)
                   return getHistogramMetric1D(RMS_h,hist);
            }
            if((type == mu_f || type == mpv) && f->GetParError(1)>fallback_threshold) {
                if(verbose) cout << "WARNING in HistUtil::getHistogramMetric1D The error on fit mean didn't pass the fallback threshold" << endl
                                 << "\tFalling back to mu_h.";
                return getHistogramMetric1D(mu_h,hist);
            }
            if((type == RMS_f || type == sigma_f) && f->GetParError(2)>fallback_threshold) {
                if(verbose) cout << "WARNING in HistUtil::getHistogramMetric1D The error on fit RMS/Sigma didn't pass the fallback threshold" << endl
                                 << "\tFalling back to RMS_h.";
                return getHistogramMetric1D(RMS_h,hist);
            }

            if      (type == mu_f  || type == mpv)     return make_pair(f->GetParameter(1),f->GetParError(1));
            else if (type == RMS_f || type == sigma_f) return make_pair(f->GetParameter(2),f->GetParError(2));
        }
        else if (type == median)  return getHistogramMedian1D(hist);
        else cout << "ERROR in HistUtil::getHistogramMetric1D type=" << type << " not defined" << endl;
        return make_pair(0.0,0.0);
    }//getHistogramMetric1D

    pair<double,double> getHistogramMetric1D(string type, TH1* hist, double fallback_threshold, bool verbose) {
        return getHistogramMetric1D(getHistogramMetricType(type),hist,fallback_threshold,verbose);
    }//getHistogramMetric1D

    //______________________________________________________________________________
    // A routine that returns the median of a given histogram and an error equal to the width of the bin that contains the histogram
    pair<double,double> getHistogramMedian1D(TH1* hist, bool debug) {
        if(!hist) {
            cout << "ERROR in HistUtil::getHistogramMedian1D the histogram is not defined (" << hist << ")" << endl;
            return make_pair(0.0,0.0);
        }

        // Compute the median for 1-d histogram hist
        Int_t nbins = hist->GetXaxis()->GetNbins();
        Double_t *x = new Double_t[nbins];
        Double_t *y = new Double_t[nbins];
        Double_t *b = new Double_t[nbins];
        for (Int_t i=0;i<nbins;i++) {
            x[i] = hist->GetXaxis()->GetBinCenter(i+1);
            y[i] = hist->GetBinContent(i+1);
            b[i] = hist->GetBinLowEdge(i+1);
        }
        Double_t median = Median(nbins,x,y,b,0,debug);
/*        // Set the error equal to the difference between the median and the bin boundary
        // Choose the difference which is biggest
        // We cannot determine the exact median from binned values
        // The best we can do is a linear approximation of where inside the bin the median resides
        // For an unbinned median follow http://davidmlane.com/hyperstat/A106993.html for the error
        Double_t low_edge = hist->GetBinLowEdge(hist->FindBin(median));
        Double_t width = hist->GetBinWidth(hist->FindBin(median));
        Double_t high_edge = hist->GetBinLowEdge(hist->FindBin(median)) + width;
        Double_t error = std::max(std::abs(median-low_edge),std::abs(high_edge-median));
*/

	// The error on the median is non-trivial
        // We use the "standard" relationship http://davidmlane.com/hyperstat/A106993.html
        // for the error. But this only works if your distribution is ~normal.
        // Fine binning is better to more precisely calculate the median & RMS
        // EffectiveEntries is used to account for weights
        Double_t error = 1.253 * hist->GetRMS() / TMath::Sqrt(hist->GetEffectiveEntries());	

        if (debug) {
 /*           cout << "\tlow_dege: " << low_edge << endl << "\thigh_edge: " << high_edge << endl
                 << "\twidth: " << width << endl << "\terror: " << error << endl;
            cout << "\tdouble check error: " << std::max(std::abs(median-low_edge),std::abs(high_edge-median)) << endl
                 << "\tabs(median-low_edge): " << std::abs(median-low_edge) << endl
                 << "\tabs(high_edge-median): " << std::abs(high_edge-median) << endl
                 << "\tmedian-low_edge: " << median-low_edge << endl
                 << "\thigh_edge-median: " << high_edge-median << endl;
*/
		cout << "\terror: " << error << endl;
            	cout << "\tRMS: " << hist->GetRMS() << endl
                << "\tentries: " << hist->GetEffectiveEntries() << endl;	
        } 
        delete [] x;
        delete [] y;
        delete [] b;
        return make_pair(median,error);
    }//getHistogramMedian

    template <typename T> double Median(Long64_t n, const T *a,  const Double_t *w, const Double_t* boundaries, Long64_t *work, bool debug) {
        const Int_t kWorkMax = 100;

        if (n <= 0 || !a) return 0;
        Bool_t isAllocated = kFALSE;
        Double_t median(0.0);
        Long64_t *ind;
        Long64_t workLocal[kWorkMax];

        if (work) {
           ind = work;
        } else {
           ind = workLocal;
           if (n > kWorkMax) {
              isAllocated = kTRUE;
              ind = new Long64_t[n];
           }
        }

        if (w) {
           Double_t sumTot2 = 0;
           for (Int_t j = 0; j < n; j++) {
              if (w[j] < 0) {
                 ::Error("TMath::Median","w[%d] = %.4e < 0 ?!",j,w[j]);
                 return 0;
              }
              sumTot2 += w[j];
           }

            sumTot2 /= 2.;

            TMath::Sort(n, a, ind, kFALSE);

            if(debug) {
                cout << "a = ";
                for(Int_t i = 0; i < n; i++) {
                    cout << a[i] << " ";
                }
                cout << endl;
                cout << "w = ";
                for(Int_t i = 0; i < n; i++) {
                    cout << w[i] << " ";
                }
                cout << endl;
                cout << "ind = ";
                for(Int_t i = 0; i < n; i++) {
                    cout << ind[i] << " ";
                }
                cout << endl;
                cout << "boundaries = ";
                for(Int_t i = 0; i <= n; i++) {
                    cout << boundaries[i] << " ";
                }
                cout << endl;
            }

            Double_t sum = 0.;
            Int_t jl;
            for (jl = 0; jl < n; jl++) {
                if(debug) cout << "Before:: jl = " << jl << "\tsum = " << sum << endl;
                sum += w[ind[jl]];
                if(debug) cout << "After::  jl = " << jl << "\tsum = " << sum << endl;
                if (sum >= sumTot2) break;
            }
            Double_t sumToJl = sum-w[ind[jl]];


            Int_t jh;
            sum = 2.*sumTot2;
            for (jh = n-1; jh >= 0; jh--) {
                if(debug) cout << "Before:: jh = " << jh << "\tsum = " << sum << endl;
                sum -= w[ind[jh]];
                if(debug) cout << "After::  jh = " << jh << "\tsum = " << sum << endl;
                if (sum <= sumTot2) break;
            }

            if(boundaries) {
                // More accurate method of determining the median
                // Median = L_m + [((N/2)-F_(m-1))/f_m]*c
                //     L_m: Lower limit of the median bar
                //     N: Total number of observations
                //     F_(m-1): Cumulative frequency of the bar preceding the median bar
                //              (i.e. the total number of observations in all bars below the median bar)
                //     f_m: Frequency of the median bar
                //     c: Median bar width
                if(debug) {
                    cout << "L_m = " << boundaries[ind[jl]] << endl
                         << "N = " << 2.*sumTot2 << endl 
                         << "F_(m-1) = " << sumToJl << endl
                         << "f_m = " << w[ind[jl]] << endl
                         << "c = " << (2.*(a[ind[jl]]-boundaries[ind[jl]])) << endl;
                }
                if (n%2 == 1)
                    median = boundaries[ind[jl]]+(((((2.*sumTot2)+1.)/2.)-sumToJl)/(w[ind[jl]]))*(2.*(a[ind[jl]]-boundaries[ind[jl]]));
                else 
                    median = boundaries[ind[jl]]+(((sumTot2)-sumToJl)/(w[ind[jl]]))*(2.*(a[ind[jl]]-boundaries[ind[jl]]));
            } else {
                // Traditional ROOT method
                //     Only returns the bin center of the bin containing the median
                median = 0.5*(a[ind[jl]]+a[ind[jh]]);
            }

            if(debug) {
                cout << "sumTot2 = " << sumTot2 << endl;
                cout << "jl = " << jl << "\tjh = " << jh << endl;
                cout << "ind[jl] = " << ind[jl] << "\tind[jh] = " << ind[jh] << endl;
                cout << "a[ind[jl]] = " << a[ind[jl]] << "\ta[ind[jh]] = " << a[ind[jh]] << endl;
            }

        } else {

            if (n%2 == 1)
               median = TMath::KOrdStat(n, a,n/2, ind);
            else {
               median = 0.5*(TMath::KOrdStat(n, a, n/2 -1, ind)+TMath::KOrdStat(n, a, n/2, ind));
            }
        }

        if (isAllocated)
            delete [] ind;
        return median;
    }//Median

    // Use like:
    // .L ../../JetUtilities/interface/HistogramUtilities.hh
    // .L ../../JetUtilities/src/HistogramUtilities.cc
    // HistUtil::testMedian()
    void testMedian() {
        TH1D* h = new TH1D("h","h",10,0,10);
        vector<int> values = {1,1,2,3,3,4,5,5,5,5,5,7,7,7,7,9};
        for(auto v : values)
            h->Fill(v);

        pair<double,double> hmedian = getHistogramMedian1D(h,true);
        cout << "Median = " << hmedian.first << " +/- " << hmedian.second << endl;
        delete h;
    }//testMedian

    //______________________________________________________________________________
    // A routine that returns a vector of HistogramMetrics and their errors (if any)
    vector<pair<double,double> > getHistogramMetric2D(HistogramMetric type, TH2* hist, AxisDirection axis, const vector<string>& names) {
        vector<pair<double,double> > ret;

        if(!hist) {
            cout << "ERROR in HistUtil::getHistogramMetric2D the histogram is not defined (" << hist << ")" << endl;
            return ret;
        }

        Int_t nbins;
        if(axis==Y) nbins = hist->GetYaxis()->GetNbins();
        else nbins = hist->GetXaxis()->GetNbins();

        bool use_names = true;
        if(int(names.size())!=nbins) {
            cout << "WARNING in HistUtil::getHistogramMetric2D the number of 1D histogram names (" << names.size()
                 << ") is not the same as the number of bins " << nbins
                 << "(i.e. the number of names is not the same as the number of histograms that will be created). "
                 << "Will delete each histogram and use a blank name." << endl;
            use_names = false;
        }

        cout << "Getting HistogramMetrics from 2D histogram ... " << endl;
        TH1 *h1(0);
        for (Int_t i=1;i<=nbins;i++) {
            loadbar2(i,nbins,50,"\t");

            if(axis==Y) h1 = hist->ProjectionY((use_names) ? names[i-1].c_str() : "",i,i);
            else h1 = hist->ProjectionX((use_names) ? names[i-1].c_str() : "",i,i);

            ret.push_back(getHistogramMetric1D(type,h1));

            if(!use_names)
                delete h1;
        } 

        return ret;
    }//getHistogramMetric2D

    vector<pair<double,double> > getHistogramMetric2D(string type, TH2* hist, AxisDirection axis, const vector<string>& names) {
        return getHistogramMetric2D(getHistogramMetricType(type),hist,axis,names);
    }//getHistogramMetric2D

}// end of namespace HistUtil

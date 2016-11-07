// This class libraries
#include "JetMETAnalysis/JetUtilities/interface/PiecewiseSpline.hh"

////////////////////////////////////////////////////////////////////////////////
// implement PiecewiseSpline class
////////////////////////////////////////////////////////////////////////////////

vector<double> PiecewiseSpline::getCoeff(int section) {
    double x,y,a,b,c,d,e,f;
    if(spline_type==TS5) {
        dynamic_cast<TSpline5*>(spline)->GetCoeff(section,x,y,b,c,d,e,f);
        a = 0;
        return {y,x,b,c,d,e,f};
    }
    if(spline_type==TSSteffen) {
        dynamic_cast<TSplineSteffen*>(spline)->GetCoeff(section,x,y,a,b,c,d);
        e = 0; f = 0;
        return {y,x,a,b,c,d};
    }
    else {
        dynamic_cast<TSpline3*>(spline)->GetCoeff(section,x,y,b,c,d);
        a = 0; e = 0; f = 0;
        return {y,x,b,c,d};
    }
}

int PiecewiseSpline::getNSections() {
    if(merge_points.size()==0) return spline->GetNp()-1;

    int number_sections = 0;
    double x,y;
    for(int isec=0; isec<spline->GetNp(); isec++) {
        spline->GetKnot(isec,x,y);
        if(x<partial_function->GetXmin()) number_sections++;
        else if(x>=partial_function->GetXmax()) number_sections++;
    }
    return number_sections++;
}

ROOT_spline_type PiecewiseSpline::getROOTSplineType(string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    if(s.find("spline3")!=string::npos)       return TS3;
    if(s.find("splineakima")!=string::npos)   return TS3;
    if(s.find("spline5")!=string::npos)       return TS5;
    if(s.find("splinesteffen")!=string::npos) return TSSteffen;
    else                                      return none;
}

int PiecewiseSpline::getSection(float x) {
    double x_knot,y_knot;

    //
    // Check upper and lower bounds
    //
    spline->GetKnot(0,x_knot,y_knot);
    if(x<x_knot) {
       cout << "WARNING::PiecewiseSpline::getSection The x value (" << x << ") is less than the minimum value of the spline (" << x_knot << ")." << endl;
        return 0;
    }
    spline->GetKnot(spline->GetNp()-1,x_knot,y_knot);
    if(x>x_knot) {
       cout << "WARNING::PiecewiseSpline::getSection The x value (" << x << ") is greater than the maximum value of the spline (" << x_knot << ")." << endl;
        return getNSections();
    }

    for(int isec=0; isec<spline->GetNp(); isec++) {
        spline->GetKnot(isec,x_knot,y_knot);
        if(x>x_knot) {
            continue;
        }
        else {
            if      (isSplineSection(isec-1) && (isec-1)<nSectionsBeforeFunction()) return isec-1;
            else if (isSplineSection(isec-1) && (isec-1)>nSectionsBeforeFunction()) return isec-1-nSectionsCoveredByFunction()+1;
            else return nSectionsBeforeFunction();
        }
    }

    cout << "WARNING::PiecewiseSpline::getSection Could not find the correct spline section." << endl;
    return -1;
}

pair<double,double> PiecewiseSpline::getSectionBounds(int section) {
    if(section>nSectionsBeforeFunction()) section = section+(nSectionsCoveredByFunction()-1);

    if(!isSplineSection(section)) return make_pair(partial_function->GetXmin(),partial_function->GetXmax());

    double x,y,xp1,yp1;
    spline->GetKnot(section,x,y);
    spline->GetKnot(section+1,xp1,yp1);
    return make_pair(x,xp1);
}

TSpline* PiecewiseSpline::gslToROOT_spline(gsl_spline* iSpline, string oName) {
    size_t n = iSpline->size;
    TSpline *oSpline(nullptr);

    if(iSpline->interp->type==gsl_interp_cspline) {
        const cspline_state_t *state = (const cspline_state_t *) iSpline->interp->state;
        oSpline = new TSpline3(oName.c_str(), iSpline->x, iSpline->y, n);
        for(unsigned int i=0; i<n; i++) {
            double b, c, d; 
            double x_hi = iSpline->x[i + 1];
            double x_lo = iSpline->x[i];
            double dx = x_hi - x_lo;
            const double y_lo = iSpline->y[i];
            const double y_hi = iSpline->y[i + 1];
            const double dy = y_hi - y_lo;
            const double c_i = state->c[i];
            const double c_ip1 = state->c[i + 1];
            b = (dy / dx) - dx * (c_ip1 + 2.0 * c_i) / 3.0;
            c = c_i;
            d = (c_ip1 - c_i) / (3.0 * dx);
            dynamic_cast<TSpline3*>(oSpline)->SetPointCoeff(i,b,c,d);
        }
    }
    else if(iSpline->interp->type==gsl_interp_akima) {
        const akima_state_t *state = (const akima_state_t *) iSpline->interp->state;
        oSpline = new TSpline3(oName.c_str(), iSpline->x, iSpline->y, n);
        for(unsigned int i=0; i<n; i++) {
            dynamic_cast<TSpline3*>(oSpline)->SetPointCoeff(i,state->b[i],state->c[i],state->d[i]);
        }
    }
    // Temporarily can't use steffen splines because of an older GSL version (3.16) in CMSSW
    // As soon as this is updated this pointer will be declared by GSL.
    // At least we can't convert them from GSL to ROOT. Can use the ROOT class TSplineSteffen
    // else if(iSpline->interp->type==gsl_interp_steffen) {
    //     const steffen_state_t *state = (const steffen_state_t *) iSpline->interp->state;
    //     oSpline = new TSplineSteffen(oName.c_str(), iSpline->x, iSpline->y, n);
    //     for(unsigned int i=0; i<n; i++) {
    //         dynamic_cast<TSplineSteffen*>(oSpline)->SetPointCoeff(i,state->a[i],state->b[i],state->c[i],state->d[i]);
    //     }
    // }
    else {
        cout << "ERROR::PiecewiseSpline::gslToROOT_spline The gsl_interp_type is unknown to this function and this cannot continue." << endl;
        std::terminate();
    }

   return oSpline;
}

bool PiecewiseSpline::isSplineSection(int section) {
    if(merge_points.size()==0) return true;

    double x,y;
    spline->GetKnot(section,x,y);
    if(x<partial_function->GetXmin() || x>=partial_function->GetXmax()) return true;
    return false;
}

int PiecewiseSpline::nSectionsBeforeFunction() {
    int ret = 0;
    for(int isec=0; isec<spline->GetNp(); isec++) {
        if(isSplineSection(isec)) ret++;
        else break;
    }
    return ret;
}

int PiecewiseSpline::nSectionsCoveredByFunction() {
    int ret = 0;
    for(int isec=0; isec<spline->GetNp(); isec++) {
        if(!isSplineSection(isec)) ret++;
    }
    return ret;
}

void PiecewiseSpline::setFullFunction(string function) {
    full_function = new TF1("fit",function.c_str(),spline->GetXmin(),spline->GetXmax());
}

TF1* PiecewiseSpline::setParameters(int section) {
    if(section>nSectionsBeforeFunction()) section = section+(nSectionsCoveredByFunction()-1);

    if(isSplineSection(section) && merge_points.size()==0) {
        vector<double> coeff = getCoeff(section);
        for(int i=0; i<num_spline_pars; i++) {
            full_function->SetParameter(i,coeff[i]);
        }
    }
    else if(isSplineSection(section)) {
        for(int i=0; i<num_pfunc_pars; i++) {
            full_function->SetParameter(i,0.0);
        }
        vector<double> coeff = getCoeff(section);
        for(int i=num_pfunc_pars; i<num_pfunc_pars+num_spline_pars; i++) {
            full_function->SetParameter(i,coeff[i-num_pfunc_pars]);
        }
    }
    else {
        for(int i=0; i<num_pfunc_pars; i++) {
            full_function->SetParameter(i,partial_function->GetParameter(i));
        }
        for(int i=num_pfunc_pars; i<num_pfunc_pars+num_spline_pars; i++) {
            full_function->SetParameter(i,0.0);
        }
    }
    if(merge_points.size()>0) {
        full_function->SetParameter(num_pfunc_pars+num_spline_pars,merge_points[0]);
        full_function->SetParameter(num_pfunc_pars+num_spline_pars+1,merge_points[1]);
    }

    return full_function;
}

void PiecewiseSpline::setPartialFunction(TF1* pf) {
    partial_function = pf;
    num_pfunc_pars = partial_function->GetNpar();
    num_spline_pars = (spline->ClassName()==string("TSpline3") || spline->ClassName()==string("TSplineSteffen")) ? 5 : 7;

    //Spline3: min(x>[11],x<[12])*([0]+[1]/(pow(log10(x),2)+[2])+[3]*exp(-[4]*(log10(x)-[5])*(log10(x)-[5])))+max(x>=[12],x<=[11])*([6]+(x-[7])*([8]+(x-[7])*([9]+(x-[7])*[10])))
    //Spline5: min(x>[13],x<[14])*([0]+[1]/(pow(log10(x),2)+[2])+[3]*exp(-[4]*(log10(x)-[5])*(log10(x)-[5])))+max(x>=[14],x<=[13])*([6]+(x-[7])*([8]+(x-[7])*([9]+(x-[7])*([10]+(x-[7])*([11]+(x-[7])*[12])))))
    stringstream ss;
    ss << "(min(x>[" << num_pfunc_pars + num_spline_pars << "],x<[" << num_pfunc_pars + num_spline_pars + 1 << "])*(" << partial_function->GetTitle()
       << "))+(max(x>=[" << num_pfunc_pars + num_spline_pars + 1 << "],x<=[" << num_pfunc_pars + num_spline_pars << "])*(["
       << num_pfunc_pars << "]+((x-[" << num_pfunc_pars+1 << "])*([" << num_pfunc_pars+2 << "]+((x-[" << num_pfunc_pars+1 << "])*(["
       << num_pfunc_pars+3 << "]+((x-[" << num_pfunc_pars+1 << "])*";

    if(spline->ClassName()==string("TSpline3") || spline->ClassName()==string("TSplineSteffen")) {
        ss << "[" << num_pfunc_pars+4 << "])))))))";
    }
    else {
        ss << "([" << num_pfunc_pars+4 << "]+((x-[" << num_pfunc_pars+1 << "])*([" << num_pfunc_pars+5 << "]+((x-[" << num_pfunc_pars+1
           << "])*[" << num_pfunc_pars+6 << "])))))))))))";
    }

    if(merge_points.size()>0)
        full_function = new TF1("fit",ss.str().c_str(),spline->GetXmin(),spline->GetXmax());
    else
        full_function = partial_function;
}
#include <fstream>
#include <iostream>
#include <TF1.h>
#include <math.h>
using namespace std;

void Corrfunction(){

	double tmp = 0;
	double etalow = 0;
	double etahigh = 0;
	double ptlow = 0;
	double pthigh = 0;
	vector<double> para;

	ifstream input("EcalMultifitHCALMethod3_L2Relative_AK4CaloHLTl1.txt");
	//ifstream input("EcalMultifitHCALMethod3_L2Relative_AK8CaloHLTl1.txt");
	ofstream output("CaloFactor_AK4.txt",ios::out);
	//ofstream output("CaloFactor_AK8.txt",ios::out);

  TF1* fun = new TF1("fun","[0]+([1]/((log10(x)^[2])+[3]))", 0, 3500);
	// ===================== validate PF Jet correction factor =======================
	for(int i=1;i<=702;i++){
		input >> tmp;

		if((i-1)%9 == 0) etalow = tmp;
		else if((i-2)%9 == 0) etahigh = tmp;
		else if((i-3)%9 == 0) continue;
		else if((i-4)%9 == 0) ptlow = tmp;
		else if((i-5)%9 == 0) pthigh = tmp;
		else{
			para.push_back(tmp);
		}

		fun->SetRange(ptlow,pthigh);

		if(i >= 9 && (i-9)%9 == 0){
			for(int j=0;j<4;j++){
				fun->SetParameter(j,para[j]);
			}

			fun->SetNpx(1000);
			if(fabs(fun->GetMinimum())<0.8 || fabs(fun->GetMaximum())>3.5) 
			  output << "Eta: " << etalow << " " << etahigh 
				       << "  Pt: " << ptlow << " " << pthigh 
				       << "  Minimum: " << fun->GetMinimum() 
				       << "  Maximum: " << fun->GetMaximum() << endl;

			if(fabs(fun->GetMinimum())>100 || fabs(fun->GetMaximum())>100) 
				cout << "******Crazy bin****** " << endl
					<< "Eta: " << etalow << " " << etahigh
					<< "Pt: " << ptlow << " " << pthigh 
					<< "Minimum: " << fun->GetMinimum()
					<< "Maximum: " << fun->GetMaximum() << endl;

		  para.clear();
		}
	}

}

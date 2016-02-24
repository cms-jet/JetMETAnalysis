import ROOT

from ROOT import TFile, TDirectory, TROOT, TSystem 
def GetKeyNames( self, dir = "" ):
        self.cd(dir)
        return [key.GetName() for key in ROOT.gDirectory.GetListOfKeys()]
TFile.GetKeyNames = GetKeyNames

#f = ROOT.TFile("/afs/cern.ch/work/f/fengwang/JECCalibration/CMSSW_7_4_0_pre9/src/JetMETAnalysis/JetAnalyzers/test/EcalMultifit_HCALMethod3/l2.root")
f = ROOT.TFile("./HLTBX25JEC/l2.root")

#keyList = f.GetKeyNames("ak4pfHLTl1")
keyList = f.GetKeyNames("ak8pfHLTl1")
#keyList = f.GetKeyNames("ak4caloHLTl1")
#keyList = f.GetKeyNames("ak8caloHLTl1")

keyCleanedList = [key for key in keyList if not key.find("AbsCorVsJetPt")]
print keyCleanedList, len(keyCleanedList)

allfuncs=[]

canv = ROOT.TCanvas("overlap","overlap",900,800)
ROOT.SetOwnership(canv,False)

for i,key in enumerate(keyCleanedList):
#	plot = f.Get("ak4pfHLTl1/"+key)
	plot = f.Get("ak8pfHLTl1/"+key)
#	plot = f.Get("ak4caloHLTl1/"+key)
#	plot = f.Get("ak8caloHLTl1/"+key)
	allfuncs.append(plot.GetFunction("fit"))

drawhist = ROOT.TH1D("drawhist","L2L3 Correction Factor VS JetPT",1000,0,3000)
drawhist.GetXaxis().SetRangeUser(0,3000)
drawhist.GetYaxis().SetRangeUser(0.8,2.8)
drawhist.SetXTitle("JetPT (GeV)")
drawhist.SetYTitle("Correction Factor")
drawhist.SetStats(0)
drawhist.Draw()
canv.SetLogx()
for i,func in enumerate(allfuncs):
	func.SetNpx(10000)
	func.Draw("same")	
ROOT.gPad.Update()
#canv.SaveAs("EcalMultifit_HCALMethod3_Calo_l2.png");
#canv.SaveAs("EcalMultifit_HCALMethod3_PF_l2.png");

#f1 = ROOT.TFile("HLTPFJet_L2L3JEC.root","RECREATE")
f1 = ROOT.TFile("HLTPFJet_L2L3JEC_AK8.root","RECREATE")
#f1 = ROOT.TFile("HLTCaloJet_L2L3JEC.root","RECREATE")
#f1 = ROOT.TFile("HLTCaloJet_L2L3JEC_AK8.root","RECREATE")

for i,key in enumerate(keyCleanedList):
#  plot = f.Get("ak4pfHLTl1/"+key)
  plot = f.Get("ak8pfHLTl1/"+key)
#  plot = f.Get("ak4caloHLTl1/"+key)
#  plot = f.Get("ak8caloHLTl1/"+key)
  plot.Write();
canv.Write();

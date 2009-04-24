################################################################################
#
# JetMETAnalysis/JetAnalyzers Makefile (for standalone use without CMSSW/SCRAM)
# -----------------------------------------------------------------------------
#
#
#             07/30/2008 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
################################################################################


CXX          = g++

TMPDIR       = $(STANDALONE_DIR)/tmp
LIBDIR       = $(STANDALONE_DIR)/lib
BINDIR       = $(STANDALONE_DIR)/bin

ROOTCXXFLAGS = $(shell $(ROOTSYS)/bin/root-config --cflags)
CXXFLAGS     = -O3 -Wall -fPIC -I. $(ROOTCXXFLAGS)

ROOTLIBS     = $(shell $(ROOTSYS)/bin/root-config --libs)
LIBS         = -L$(LIBDIR) -lJetUtilities

all:  setup bin

setup:
	rm -f JetMETAnalysis; ln -fs ../ JetMETAnalysis
	mkdir -p $(TMPDIR)
	mkdir -p $(LIBDIR)
	mkdir -p $(BINDIR)

bin:  jet_response_analyzer jet_response_fitter jet_response_plotter \
      jet_l2_correction jet_l3_correction jet_inspect_jra_histos

jet_response_analyzer:
	$(CXX) $(CXXFLAGS) bin/jet_response_analyzer_x.cc $(LIBS) $(ROOTLIBS) \
        -o $(BINDIR)/jet_response_analyzer_x

jet_response_fitter:
	$(CXX) $(CXXFLAGS) bin/jet_response_fitter_x.cc $(LIBS) $(ROOTLIBS) \
        -o $(BINDIR)/jet_response_fitter_x

jet_response_plotter:
	$(CXX) $(CXXFLAGS) bin/jet_response_plotter_x.cc $(LIBS) $(ROOTLIBS) \
        -o $(BINDIR)/jet_response_plotter_x

jet_l2_correction:
	$(CXX) $(CXXFLAGS) bin/jet_l2_correction_x.cc $(LIBS) $(ROOTLIBS) \
        -o $(BINDIR)/jet_l2_correction_x

jet_l3_correction:
	$(CXX) $(CXXFLAGS) bin/jet_l3_correction_x.cc $(LIBS) $(ROOTLIBS) \
        -o $(BINDIR)/jet_l3_correction_x

jet_inspect_jra_histos:
	$(CXX) $(CXXFLAGS) bin/jet_inspect_jra_histos_x.cc $(LIBS) $(ROOTLIBS) \
        -o $(BINDIR)/jet_inspect_jra_histos_x

clean:
	rm -rf JetMETAnalysis \
	       $(BINDIR)/jet_response_analyzer_x \
	       $(BINDIR)/jet_response_fitter_x \
	       $(BINDIR)/jet_response_plotter_x \
	       $(BINDIR)/jet_response_l2_correction_x \
	       $(BINDIR)/jet_response_l3_correction_x \
               $(BINDIR)/jet_inspect_jra_histos_x

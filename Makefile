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

bin:  jet_response_analyzer jet_response_fitter jet_response_and_resolution \
      jet_l2_correction jet_l3_correction \
      jet_weighted_spectrum jet_matching_efficiency jet_deltar_efficiency \
      jet_response_vs_deltar

jet_response_analyzer:
	$(CXX) $(CXXFLAGS) bin/jet_response_analyzer_x.cc $(LIBS) $(ROOTLIBS) \
        -o $(BINDIR)/jet_response_analyzer_x

jet_response_fitter:
	$(CXX) $(CXXFLAGS) bin/jet_response_fitter_x.cc $(LIBS) $(ROOTLIBS) \
        -o $(BINDIR)/jet_response_fitter_x

jet_response_and_resolution:
	$(CXX) $(CXXFLAGS) bin/jet_response_and_resolution_x.cc $(LIBS) $(ROOTLIBS) \
        -o $(BINDIR)/jet_response_and_resolution_x

jet_l2_correction:
	$(CXX) $(CXXFLAGS) bin/jet_l2_correction_x.cc $(LIBS) $(ROOTLIBS) \
        -o $(BINDIR)/jet_l2_correction_x

jet_l3_correction:
	$(CXX) $(CXXFLAGS) bin/jet_l3_correction_x.cc $(LIBS) $(ROOTLIBS) \
        -o $(BINDIR)/jet_l3_correction_x

jet_weighted_spectrum:
	$(CXX) $(CXXFLAGS) bin/jet_weighted_spectrum_x.cc $(LIBS) $(ROOTLIBS) \
        -o $(BINDIR)/jet_weighted_spectrum_x

jet_matching_efficiency:
	$(CXX) $(CXXFLAGS) bin/jet_matching_efficiency_x.cc $(LIBS) $(ROOTLIBS) \
        -o $(BINDIR)/jet_matching_efficiency_x

jet_deltar_efficiency:
	$(CXX) $(CXXFLAGS) bin/jet_deltar_efficiency_x.cc $(LIBS) $(ROOTLIBS) \
        -o $(BINDIR)/jet_deltar_efficiency_x

jet_response_vs_deltar:
	$(CXX) $(CXXFLAGS) bin/jet_response_vs_deltar_x.cc $(LIBS) $(ROOTLIBS) \
        -o $(BINDIR)/jet_response_vs_deltar_x

clean:
	rm -rf JetMETAnalysis \
	       $(BINDIR)/jet_response_analyzer_x \
	       $(BINDIR)/jet_response_fitter_x \
	       $(BINDIR)/jet_response_and_resolution_x \
	       $(BINDIR)/jet_response_l2_correction_x \
	       $(BINDIR)/jet_response_l3_correction_x \
               $(BINDIR)/jet_weighted_spectrum_x \
               $(BINDIR)/jet_matching_efficiency_x \
               $(BINDIR)/jet_deltar_efficiency_x \
               $(BINDIR)/jet_response_vs_deltar_x

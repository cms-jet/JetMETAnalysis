################################################################################
#
# JetMETAnalysis/JetAnalyzers Makefile (for standalone use without CMSSW/SCRAM)
# -----------------------------------------------------------------------------
#
#
#             07/30/2008 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
################################################################################


CXX          = g++

TMPDIR       = ../tmp
LIBDIR       = ../lib
BINDIR       = ../bin

ROOTCXXFLAGS = $(shell $(ROOTSYS)/bin/root-config --cflags)
CXXFLAGS     = -O3 -Wall -fPIC -I. $(ROOTCXXFLAGS)

ROOTLIBS     = $(shell $(ROOTSYS)/bin/root-config --libs)
LIBS         = -L$(LIBDIR) -lJetUtilities

all:  setup bin

setup:
	rm -f SchieferD; ln -fs ../ JetMETAnalysis
	mkdir -p $(TMPDIR)
	mkdir -p $(LIBDIR)
	mkdir -p $(BINDIR)

bin:  jet_response_analyzer jet_inspect_jra_histos

jet_response_analyzer:
	$(CXX) $(CXXFLAGS) bin/jet_response_analyzer_x.cc $(LIBS) $(ROOTLIBS) \
        -o $(BINDIR)/jet_response_analyzer_x

jet_inspect_jra_histos:
	$(CXX) $(CXXFLAGS) bin/jet_inspect_jra_histos_x.cc $(LIBS) $(ROOTLIBS) \
        -o $(BINDIR)/jet_inspect_jra_histos_x

clean:
	rm -rf JetMETAnalysis \
	       $(BINDIR)/jet_response_analyzer_x \
               $(BINDIR)/jet_inspect_jra_histos_x

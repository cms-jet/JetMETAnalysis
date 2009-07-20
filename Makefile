################################################################################
#
# JetMETAnalysis/JetUtilities Makefile (for standalone use outside CMSSW/SCRAM)
# -----------------------------------------------------------------------------
#
#
#             07/29/2008 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
################################################################################


CXX          = g++

TMPDIR       = $(STANDALONE_DIR)/tmp
LIBDIR       = $(STANDALONE_DIR)/lib
BINDIR       = $(STANDALONE_DIR)/bin

# maxosx
CXX_SHRD     = -dynamiclib
LIB_SUFX     = dylib

# linux
#CXX_SHRD     = -shared
#LIB_SUFX     = so


ROOTCXXFLAGS = $(shell $(ROOTSYS)/bin/root-config --cflags)
CXXFLAGS     = -O3 -Wall -fPIC -I. $(ROOTCXXFLAGS)

ROOTLIBS     = $(shell $(ROOTSYS)/bin/root-config --libs)

OBJS         = $(TMPDIR)/CommandLine.o $(TMPDIR)/RootStyle.o

LIB          = libJetUtilities.$(LIB_SUFX)
LIBS         = -L$(LIBDIR) -lJetUtilities


all: setup lib bin

setup:
	rm -f JetMETAnalysis; ln -sf ../ JetMETAnalysis
	mkdir -p $(TMPDIR)
	mkdir -p $(LIBDIR)
	mkdir -p $(BINDIR)

lib: $(OBJS)
	$(CXX) $(CXXFLAGS) $(CXX_SHRD) $(OBJS) $(ROOTLIBS) -o $(LIBDIR)/$(LIB) 

bin: jet_inspect_histos jet_inspect_graphs

jet_inspect_histos:
	$(CXX) $(CXXFLAGS) bin/jet_inspect_histos_x.cc $(LIBS) $(ROOTLIBS) \
        -o $(BINDIR)/jet_inspect_histos_x

jet_inspect_graphs:
	$(CXX) $(CXXFLAGS) bin/jet_inspect_graphs_x.cc $(LIBS) $(ROOTLIBS) \
        -o $(BINDIR)/jet_inspect_graphs_x

clean:
	rm -rf $(OBJS) $(LIBDIR)/$(LIB) JetMETAnalysis \
               $(BINDIR)/jet_inspect_histos_x \
               $(BINDIR)/jet_inspect_graphs_x


################################################################################
# $(OBJS)
################################################################################

$(TMPDIR)/CommandLine.o: interface/CommandLine.h src/CommandLine.cc
	$(CXX) $(CXXFLAGS) -c -o $(TMPDIR)/CommandLine.o src/CommandLine.cc

$(TMPDIR)/RootStyle.o: interface/RootStyle.h src/RootStyle.cc
	$(CXX) $(CXXFLAGS) -c -o $(TMPDIR)/RootStyle.o src/RootStyle.cc

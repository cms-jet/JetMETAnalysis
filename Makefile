################################################################################
#
# JetMETAnalysis/JetUtilities Makefile (for standalone use outside CMSSW/SCRAM)
# -----------------------------------------------------------------------------
#
# INSTRUCTIONS:
# =============
# setenv ROOTSYS /path/to/root
# setenv PATH $ROOTSYS/bin:${PATH}
# setenv LD_LIBRARY_PATH $ROOTSYS/lib
#
# mkdir standalone; cd standalone
# setenv STANDALONE_DIR $PWD
# setenv PATH $STANDALONE_DIR/bin:${PATH}
# setenv LD_LIBRARY_PATH $STANDALONE_DIR/lib:${LD_LIBRARY_PATH}
# cvs co -d JetUtilities CMSSW/CondFormats/JetUtilities
# cd JetUtilities
# make
#
# [you might want to stick these into e.g. $STANDALONE_DIR/setup.[c]sh]
#
#             07/29/2008 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
################################################################################

ifeq ($(STANDALONE_DIR),)
	standalone_dir:=../
	export STANDALONE_DIR:=$(standalone_dir)
endif

TMPDIR       = $(STANDALONE_DIR)/tmp
LIBDIR       = $(STANDALONE_DIR)/lib
BINDIR       = $(STANDALONE_DIR)/bin


CXX          = g++

ROOTCXXFLAGS = $(shell $(ROOTSYS)/bin/root-config --cflags)
CXXFLAGS     = -O3 -Wall -fPIC -I. $(ROOTCXXFLAGS)

ROOTLIBS     = $(shell $(ROOTSYS)/bin/root-config --libs)

OBJS         = $(TMPDIR)/CommandLine.o $(TMPDIR)/RootStyle.o

LIB          = libJetUtilities.so
LIBS         = -L$(LIBDIR) -lJetUtilities


all: setup lib bin

setup:
	rm -f JetMETAnalysis; ln -sf ../ JetMETAnalysis
	mkdir -p $(TMPDIR)
	mkdir -p $(LIBDIR)
	mkdir -p $(BINDIR)

lib: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared $(OBJS) $(ROOTLIBS) -o $(LIBDIR)/$(LIB) 

bin: jet_inspect_profiles jet_inspect_histos jet_inspect_graphs

jet_inspect_profiles:
	$(CXX) $(CXXFLAGS) bin/jet_inspect_profiles_x.cc $(LIBS) $(ROOTLIBS) \
        -o $(BINDIR)/jet_inspect_profiles_x

jet_inspect_histos:
	$(CXX) $(CXXFLAGS) bin/jet_inspect_histos_x.cc $(LIBS) $(ROOTLIBS) \
        -o $(BINDIR)/jet_inspect_histos_x

jet_inspect_graphs:
	$(CXX) $(CXXFLAGS) bin/jet_inspect_graphs_x.cc $(LIBS) $(ROOTLIBS) \
        -o $(BINDIR)/jet_inspect_graphs_x


clean:
	rm -rf $(OBJS) $(LIBDIR)/$(LIB) JetMETAnalysis \
               $(BINDIR)/jet_inspect_profiles_x \
               $(BINDIR)/jet_inspect_histos_x \
               $(BINDIR)/jet_inspect_graphs_x


################################################################################
# $(OBJS)
################################################################################

$(TMPDIR)/CommandLine.o: interface/CommandLine.h src/CommandLine.cc
	$(CXX) $(CXXFLAGS) -c -o $(TMPDIR)/CommandLine.o src/CommandLine.cc

$(TMPDIR)/RootStyle.o: interface/RootStyle.h src/RootStyle.cc
	$(CXX) $(CXXFLAGS) -c -o $(TMPDIR)/RootStyle.o src/RootStyle.cc

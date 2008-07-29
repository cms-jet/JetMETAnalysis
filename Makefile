################################################################################
#
# JetMETAnalysis/JetUtilities Makefile (for standalone use outside CMSSW/SCRAM)
# -----------------------------------------------------------------------------
#
#
#             07/29/2008 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
################################################################################


CXX          = g++

TMPDIR       = ../tmp
LIBDIR       = ../lib
BINDIR       = ../bin

# maxosx
#CXX_SHRD     = -dynamiclib
#LIB_SUFX     = dylib

# linux
CXX_SHRD     = -shared
LIB_SUFX     = so


ROOTCXXFLAGS = $(shell $(ROOTSYS)/bin/root-config --cflags)
CXXFLAGS     = -O3 -Wall -fPIC -I. $(ROOTCXXFLAGS)

ROOTLIBS     = $(shell $(ROOTSYS)/bin/root-config --libs)

OBJS         = $(TMPDIR)/CommandLine.o $(TMPDIR)/RootStyle.o \
	       $(TMPDIR)/HistogramLoader.o

LIB          = libJetUtilities.$(LIB_SUFX)
LIBS         = -L$(LIBDIR) -lJetUtilities


all: setup lib

setup:
	rm -f JetMETAnalysis; ln -sf ../ JetMETAnalysis
	mkdir -p $(TMPDIR)
	mkdir -p $(LIBDIR)
	mkdir -p $(BINDIR)

lib: $(OBJS)
	$(CXX) $(CXXFLAGS) $(CXX_SHRD) $(OBJS) $(ROOTLIBS) -o $(LIBDIR)/$(LIB) 

clean:
	rm -rf $(OBJS) $(LIBDIR)/$(LIB) JetMETAnalysis


################################################################################
# $(OBJS)
################################################################################

$(TMPDIR)/CommandLine.o: interface/CommandLine.h src/CommandLine.cc
	$(CXX) $(CXXFLAGS) -c -o $(TMPDIR)/CommandLine.o src/CommandLine.cc

$(TMPDIR)/HistogramLoader.o: interface/HistogramLoader.h src/HistogramLoader.cc
	$(CXX) $(CXXFLAGS) -c -o $(TMPDIR)/HistogramLoader.o src/HistogramLoader.cc

$(TMPDIR)/RootStyle.o: interface/RootStyle.h src/RootStyle.cc
	$(CXX) $(CXXFLAGS) -c -o $(TMPDIR)/RootStyle.o src/RootStyle.cc

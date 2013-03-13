#___________________________________________________________________[MAKEFILE]
#/////////////////////////////////////////////////////////////////////////////
# Name             MiniballMLLAnalyze.mk
# Purpose:         Compile and link user's analyzing code
# Targets:         M_analyze     -- user's analyze program
#                  clean         -- clean-up
# Author:          marabou
# Revision:
# Date:
# URL:
#/////////////////////////////////////////////////////////////////////////////

PREFIX		=	MiniballMLL
AFILE 		=	MiniballMLLAnalyze

INCLUDE 	=	-I$(MARABOU)/include
OBJPATH		=	$(MARABOU)/obj

ROOTCFLAGS	=	$(shell root-config --cflags)
ROOTLIBS	=	$(shell root-config --new --libs)
ROOTGLIBS	=	$(shell root-config --new --glibs)

# Linux with egcs
CXX 		=	g++
CXXFLAGS	=	-g -c -O -fPIC -Wno-write-strings $(INCLUDE) -I./

LD			=	g++
LFLAGS		=	-g
CXXFLAGS	+=	$(ROOTCFLAGS)
LIBS		=	$(ROOTLIBS)
GLIBS		=	-L$(ROOTSYS)/lib -lGraf $(ROOTGLIBS)

TMRBLIBS	=	-L$(MARABOU)/lib -lTMrbAnalyze -lTMrbTransport -lTMrbUtils

ULIB_HDRS	=

.SUFFIXES:	.cxx .h

#------------------------------------------------------------------------------

all:		testUnpack

testUnpack:	testUnpack.o lib$(AFILE).so
			$(LD) $(LFLAGS) -o testUnpack testUnpack.o \
					-L. -l$(AFILE) \
					$(TMRBLIBS) \
					$(GLIBS)

.cxx.o:
			$(CXX) $(CXXFLAGS) -o $@ $<

#___________________________________________________________________[MAKEFILE]
#/////////////////////////////////////////////////////////////////////////////
# Name             ExaAnalyze.mk
# Purpose:         Compile and link user's analyzing code
# Targets:         M_analyze     -- user's analyze program
#                  clean         -- clean-up
# Author:          MBS and ROOT Based Online Offline Utility
# Revision:         
# Date:            
# URL:             
#/////////////////////////////////////////////////////////////////////////////

AFILE 		=	ExaAnalyze

INCLUDE 	=	-I$(MARABOU)/include
OBJPATH		=	$(MARABOU)/obj

ROOTCFLAGS	=	$(shell root-config --cflags)
ROOTLIBS	=	$(shell root-config --new --libs)
ROOTGLIBS	=	$(shell root-config --new --glibs)

# Linux with egcs
CXX 		=	g++
CXXFLAGS	=	-c -O -fPIC $(INCLUDE) -I./

LD			=	g++
LFLAGS		=	-g
CXXFLAGS	+=	$(ROOTCFLAGS)
LIBS		=	$(ROOTLIBS)
GLIBS		=	$(ROOTGLIBS)

TMRBLIBS	=	-L$(MARABOU)/lib \
				-lTMrbAnalyze -lTMrbTransport -lTMrbUtils -lMutex -lN2ll

# Header files which ExaAnalyze.cxx depends on
USER_HDRS	=	\
				$(AFILE).h \
				$(AFILE)Globals.h

# Pieces of code to be included in ExaAnalyze.cxx
USER_CODE	=

# User code to be compiled separately
USER_OBJS 	=

# User classes to be documented via THtml
DOCS		=	\
				TUsrEvtReadout \
				TUsrSevtData

PROGRAMS	=	M_analyze

.SUFFIXES:	.cxx .h

#------------------------------------------------------------------------------

all:		$(PROGRAMS)

M_analyze:	$(OBJPATH)/M_analyze.o $(USER_CODE) $(USER_OBJS) lib$(AFILE).so
			$(LD) $(LFLAGS) -o M_analyze $(OBJPATH)/M_analyze.o $(USER_OBJS) \
					-L. -l$(AFILE) \
					$(TMRBLIBS) \
					$(GLIBS) -lpthread

lib$(AFILE).so: $(AFILE).o $(AFILE)Dict.o
			$(CXX) $(LDFLAGS) -shared $(AFILE).o $(AFILE)Dict.o -o lib$(AFILE).so

$(AFILE).o: $(USER_HDRS) $(USER_CODE) $(AFILE).cxx
			$(CXX) $(CXXFLAGS) $(AFILE).cxx

$(AFILE)Dict.cxx: $(USER_HDRS) $(AFILE).cxx
			rootcint -f $(AFILE)Dict.cxx -c $(INCLUDE) $(AFILE).h $(AFILE)LinkDef.h

$(AFILE)Dict.o:	$(AFILE)Dict.cxx
			$(CXX) $(CXXFLAGS) $(AFILE)Dict.cxx

doc:		$(AFILE).cxx
			@if [ ! -d HTML ]; then mkdir HTML; fi
			@r2html -s . -d HTML \
				-l libMutex.so \
				-l libTMrbUtils.so \
				-l libTGMrbUtils.so \
				-l libTMrbAnalyze.so \
				-l libTMrbTransport.so \
				-l `pwd`/lib$(AFILE).so \
				$(DOCS)
			@for i in $(AFILE).html $(USER_HDRS) $(USER_CODE); do \
				ln -f $$i HTML; \
			done
			@cd HTML; updhref *.html
			@cd HTML/src; updhref *.html

clean:
			rm -f *.o $(USER_OBJS) lib$(AFILE).so $(AFILE)Dict.cxx

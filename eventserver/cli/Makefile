########################################################################
# Copyright:
#   GSI, Gesellschaft fuer Schwerionenforschung mbH
#   Planckstr. 1
#   D-64291 Darmstadt
#   Germany
########################################################################
# MRevLib.make
# make ROOT event client for MBS remote event server (Linux version)
# assume all sources and header files in local directory
#
# create shared library MRevLib.so
# to be loaded in ROOT with gSystem.Load("MRevLib")
#
# created 28.10.98 by Horst Goeringer
########################################################################
#
ObjSuf        = o
SrcSuf        = cc
ExeSuf        =
DllSuf        = so
OutPutOpt     = -o

SHRLIB        = MRevLib.$(DllSuf)

ROOTLIBS      = -L$(ROOTSYS)/lib -lNew -lBase -lCint -lClib -lCont -lFunc \
                -lGraf -lGraf3d -lHist -lHtml -lMatrix -lMeta -lMinuit -lNet \
                -lPostscript -lProof -lRint -lTree -lUnix -lZip
ROOTGLIBS     = -lGpad -lGui -lGX11 -lX3d

# some tools (C functions)
TOOLLIB = libclitool.a

CC            = gcc
CFLAGS        = -O -D __STRICT_ANSI__ -D SYS_UNIX
CINCLS        = -I. \
                -I/usr/include -I/usr/include/sys
# Linux: system pathes required

CXX           = g++
CXXFLAGS      = -O -Wall -fPIC -I$(ROOTSYS)/include \
                -D Linux -DR__GLIBC -fno-exceptions
#               -D Linux -DR__GLIBC -fno-rtti -fno-exceptions

LD            = g++
LDFLAGS       = -g
SOFLAGS       = -Wl,-soname,MRevLib.so -shared
LIBS          = $(ROOTLIBS) -lg++ -lm -ldl -rdynamic
GLIBS         = $(ROOTLIBS) $(ROOTGLIBS) -L/usr/X11R6/lib \
                -lXpm -lX11 -lg++ -lm -ldl -rdynamic

ARFLAGS       = r

########################## modules ##################################

TOOLHEAD     =  ptrevcomm.h
HEAD         =  MRevBuffer.h \
                MRevLinkDef.h

DICT          = MRevDict
DICTSRC       = $(DICT).$(SrcSuf)
DICTOBJ       = $(DICT).$(ObjSuf)

SRCS          = MRevBuffer.$(SrcSuf)
OBJS          = MRevBuffer.$(ObjSuf)

TOOLSRC       = exitCli.c \
                rclose.c \
                swaplw.c
TOOLOBJ       = exitCli.o \
                rclose.o \
                swaplw.o

$(SHRLIB): $(OBJS) $(DICTOBJ) $(TOOLLIB)
	@echo "****************************************************************"
	@echo "Creating shared library MRevLib.so: ROOT event client for MBS"
	@echo "****************************************************************"
	$(LD) $(SOFLAGS) $(LDFLAGS) -o $(SHRLIB) \
           $(OBJS) $(DICTOBJ) -L. -lclitool

$(DICTSRC): $(HEAD)
	@echo "Generating dictionary ..."
	$(ROOTSYS)/bin/rootcint -f $(DICTSRC) -c $(HEAD)

$(DICTOBJ): $(DICTSRC)
	$(CXX) $(CXXFLAGS) -c $(DICTSRC)

$(OBJS): $(SRCS) $(HEAD) $(TOOLHEAD)
	$(CXX) $(CXXFLAGS) $(CINCLS) -c $(SRCS)
# Linux: need C include pathes here!

$(TOOLLIB): $(TOOLOBJ) $(TOOLSRC)
	ar $(ARFLAGS) $(TOOLLIB) $(TOOLOBJ)

.c.o:
	$(CC) $(CFLAGS) $(CINCLS) -c $<

cleanobj:
	rm $(OBJS) $(DICTOBJ) $(TOOLOBJ)

clean:
	rm $(OBJS) $(DICTOBJ) $(TOOLOBJ) $(DICTSRC) $(TOOLLIB) \
           $(SHRLIB) $(DICT).h


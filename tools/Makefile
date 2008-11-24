CXX           = g++
CXXFLAGS      = -c -O -fPIC -I./ -I$(MARABOU)/include -I$(ROOTSYS)/include 

LD			=	g++
LFLAGS		=	-g

HDRS		=	

SRCS		=	xrcm.cxx

OBJS		=	xrcm.o

LIBS		= $(MARABOU)/lib/libTMrbUtils.so \
			  $(MARABOU)/lib/libTMbsSetup.so

ROOTCFLAGS	= $(shell root-config --cflags)
ROOTLIBS	= $(shell root-config --new --libs)
ROOTGLIBS	= $(shell root-config --new --glibs)

INSTALL		=	:
NOINSTALL	=	

PROGRAMS	=	xrcm mbsSetup

.SUFFIXES:	.cxx .h

all:		$(PROGRAMS)

install:	$(PROGRAMS)
			@for p in $(PROGRAMS); do \
				echo "[Installing $$p on $(MARABOU)/bin]"; \
				cp $$p $(MARABOU)/bin; \
			done

xrcm:		xrcm.o
			$(CXX) $(LFLAGS) -o xrcm xrcm.o $(ROOTLIBS) $(LIBS)

mbsSetup:	mbsSetup.o
			$(CXX) $(LFLAGS) -o mbsSetup mbsSetup.o $(ROOTLIBS) $(LIBS)

clean:
			rm -f $(OBJS) core

.cxx.o:
		$(CXX) $(CXXFLAGS) -c $<	

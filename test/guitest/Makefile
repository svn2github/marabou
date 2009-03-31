INCLUDE 	=	-I$(MARABOU)/include

ROOTCFLAGS	=	$(shell root-config --cflags)
ROOTLIBS	=	$(shell root-config --new --libs)
ROOTGLIBS	=	$(shell root-config --new --glibs)

# Linux with egcs
CXX 		=	g++
CXXFLAGS	=	-g -c -O -fPIC $(INCLUDE) -I./

LD			=	g++
LFLAGS		=	-g
CXXFLAGS	+=	$(ROOTCFLAGS)
LIBS		=	$(ROOTLIBS)
GLIBS		=	$(ROOTGLIBS)

TMRBLIBS	=	-L$(MARABOU)/lib \
				-lTMrbUtils -lTNGMrbUtils

PROGRAMS	=	gt

.SUFFIXES:	.cxx .h

#------------------------------------------------------------------------------

all:		$(PROGRAMS)

gt: 		gt.o
			$(LD) $(LFLAGS) -o gt gt.o\
					$(TMRBLIBS) \
					$(GLIBS)

clean:
			rm -f *.o

.cxx.o:
			$(CXX) $(CXXFLAGS) -o $@ $<

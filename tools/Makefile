CXX           = g++
CXXFLAGS      = -c -O -fPIC -I$(MARABOU)/include -I./

LD			=	g++
LFLAGS		=	-g

HDRS		=	

SRCS		=	xrcm.cxx

OBJS		=	xrcm.o

INSTALL		=	:
NOINSTALL	=	

PROGRAMS	=	xrcm

.SUFFIXES:	.cxx .h

all:		$(PROGRAMS)

install:	$(PROGRAMS)
			@echo "[Installing xrcm]"; \
				scp xrcm $(MARABOU)/bin; \
				scp xrcm /usr/local/bin; \
			done

xrcm:		xrcm.o
			$(CXX) $(LFLAGS) -o xrcm xrcm.o $(ROOTLIBS) $(LIBS)

clean:
			rm -f $(OBJS) core

.cxx.o:
		$(CXX) $(CXXFLAGS) -c $<	

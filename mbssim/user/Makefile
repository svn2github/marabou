CC	 		=	g++

CFLAGS		=	-w -g -Wno-switch -c -O -DMBS_SIMUL_RDO -I$(MARABOU)/powerpc/readout/inc -I../inc -I$(MARABOU)/include $(shell root-config --cflags)

LD			=	g++
LFLAGS		=	-g

LIBS		= -L $(MARABOU)/lib -lMbsSimRdo -lTMrbUtils $(shell root-config --new --libs)

.SUFFIXES:	.cxx .c

all:		mbssimtst

mbssimtst: 	mbssimtst.o user_readout.o
			$(CC) -g -o mbssimtst mbssimtst.o $(LIBS)

clean:
			rm -f mbssimtst *.o

.cxx.o:
			$(CC) $(CFLAGS) -o $@ $<

.c.o:
			$(CC) $(CFLAGS) -o $@ $<


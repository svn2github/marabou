CC = gcc

PPCINC 		=	../../../include
CESINC		=	/usr/include/ces
RDOINC		=	../../../readout/inc
MBSINC		=	$(MBSROOT)/inc

CFLAGS	=	-pipe -O3 -ansi -g -w -DR__LYNXOS -DCPU_TYPE_$(GSI_CPU_PLATFORM) -DLYNX_VERSION_$(LV_CPP) -I../inc -I$(RDOINC) -I$(PPCINC) -I$(MBSINC) -I$(CESINC)

# ------------ Libaries ------------------------------------------------

AR = ar
ARFLAG = -rv

# ------------ Library-Objects ----------------------------------------

LIB_TARGETS = ../../../lib/$(GSI_OS_VERSION)/libMadc32.a

libMadc32.OBJS = madc32fct.o block.o

# ------------ Suffix Rules -------------------------------------------

.SUFFIXES:

# ----- C Code ---------
%.o: %.c
	$(CC) $< -c $(CFLAGS) -o $@

.SUFFIXES: .o .c

# ------------ Targets -------------------------------------------------

all: $(LIB_TARGETS) madc32

$(LIB_TARGETS): $(libMadc32.OBJS)
	$(AR) $(ARFLAGS) $@ $^
	ranlib $@

madc32:	madc32.o $(LIB_TARGETS)
		$(CC) -o madc32 madc32.o \
			-L ../../../lib/$(GSI_OS_VERSION) \
			-L /lib/ces \
			-lMadc32 -lUti -lvme -lbma -luio -lnetinet

clean:
	rm -f *.o core core.* $(LIB_TARGETS)

depend:
	gcc -MM $(CFLAGS) $(CPPFLAGS) *.c

# ------------ Dependencies --------------------------------------------

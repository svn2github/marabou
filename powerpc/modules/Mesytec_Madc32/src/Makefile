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

LIB_TARGETS = ../../../lib/$(LYNX_VERSION)/libMadc32.a

libSis_3300.OBJS = madc32.o

# ------------ Suffix Rules -------------------------------------------

.SUFFIXES:

# ----- C Code ---------
%.o: %.c
	$(CC) $< -c $(CFLAGS) -o $@ 

.SUFFIXES: .o .c 

# ------------ Targets -------------------------------------------------

all: $(LIB_TARGETS)$(CPPFLAGS) 

$(LIB_TARGETS): $(libSis_3300.OBJS)
	$(AR) $(ARFLAGS) $@ $^
	ranlib $@

lib: $(LIB_TARGETS)


clean:
	rm -f *.o core core.* $(LIB_TARGETS)

depend:
	gcc -MM $(CFLAGS) $(CPPFLAGS) *.c

# ------------ Dependencies --------------------------------------------

madc32.o:			madc32.c ../inc/madc32.h ../inc/madc32_protos.h

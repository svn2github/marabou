CC = gcc

PPCINC 		=	../../../include
CESINC		=	/usr/include/ces
UTIINC		=	../../../utils/inc
MBSINC		=	$(MBSROOT)/inc

CFLAGS	=	-pipe -O3 -ansi -g -w -DR__LYNXOS -DCPU_TYPE_$(GSI_CPU_PLATFORM) -DLYNX_VERSION_$(LV_CPP) -I$(MBSROOT)/inc -I../inc -I$(UTIINC) -I$(PPCINC) -I$(MBSINC) -I$(CESINC)

# ------------ Libaries ------------------------------------------------

AR = ar
ARFLAG = -rv

# ------------ Library-Objects ----------------------------------------

LIB_TARGETS = ../../../lib/$(GSI_OS_VERSION)/libSis_3300.a

libSis_3300.OBJS = sis_3300_fct.o sis_3300_readout.o

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

sis_3300_fct.o: sis_3300_fct.c ../inc/sis_3300.h ../inc/sis_3300_protos.h

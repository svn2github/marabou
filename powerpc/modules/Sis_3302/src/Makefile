CC = gcc

PPCINC 		=	../../../include
CESINC		=	/usr/include/ces
UTIINC		=	../../../utils/inc
C2LINC		=	../../../../c2lynx/inc
MBSINC		=	$(MBSROOT)/inc

CFLAGS	=	-pipe -O3 -ansi -g -w -DR__LYNXOS -DCPU_TYPE_$(GSI_CPU_PLATFORM) -DLYNX_VERSION_$(LV_CPP) -I../inc -I$(UTIINC) -I$(PPCINC) -I$(MBSINC) -I$(CESINC) -I$(C2LINC)

# ------------ Libaries ------------------------------------------------

AR = ar
ARFLAG = -rv

# ------------ Library-Objects ----------------------------------------

LIB_TARGETS = ../../../lib/$(GSI_OS_VERSION)/libSis_3302.a

libSis_3302.OBJS = sis_3302_fct.o sis_3302_readout.o

# ------------ Suffix Rules -------------------------------------------

.SUFFIXES:

# ----- C Code ---------
%.o: %.c
	$(CC) $< -c $(CFLAGS) -o $@

.SUFFIXES: .o .c

# ------------ Targets -------------------------------------------------

all: $(LIB_TARGETS)$(CPPFLAGS)

$(LIB_TARGETS): $(libSis_3302.OBJS)
	cp ../inc/*.h ../../../include
	$(AR) $(ARFLAGS) $@ $^
	ranlib $@

lib: $(LIB_TARGETS)

clean:
	rm -f *.o core core.* $(LIB_TARGETS)

depend:
	gcc -MM $(CFLAGS) $(CPPFLAGS) *.c

# ------------ Dependencies --------------------------------------------

sis_3302_fct.o: sis_3302_fct.c ../inc/sis_3302_database.h ../inc/sis_3302_protos.h  ../inc/sis_3302_layout.h

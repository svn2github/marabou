CC = gcc

PPCINC 		=	../../../include
CESINC		=	/usr/include/ces
UTIINC		=	../../../utils/inc
MBSINC		=	$(MBSROOT)/inc

CFLAGS	=	-pipe -O3 -ansi -g -w -DR__LYNXOS -DCPU_TYPE_$(GSI_CPU_PLATFORM) -DLYNX_VERSION_$(LV_CPP) -I../inc -I$(UTIINC) -I$(PPCINC) -I$(MBSINC) -I$(CESINC)

BIN_DIR = ../../../bin/$(GSI_OS_VERSION)

# ------------ Libaries ------------------------------------------------

AR = ar
ARFLAG = -rv

LIB		= $(MBSROOT)/lib_$(GSI_CPU_PLATFORM)

# ------------ Library-Objects ----------------------------------------

LIB_TARGETS = ../../../lib/$(GSI_OS_VERSION)/libMadc32.a

libMadc32.OBJS = madc32fct.o

# ------------ Suffix Rules -------------------------------------------

.SUFFIXES:

# ----- C Code ---------
%.o: %.c
	$(CC) $< -c $(CFLAGS) -o $@

.SUFFIXES: .o .c

# ------------ Targets -------------------------------------------------

all: $(LIB_TARGETS) madc32

$(LIB_TARGETS): $(libMadc32.OBJS)
	cp ../inc/*.h ../../../include
	$(AR) $(ARFLAGS) $@ $^
	ranlib $@

madc32:	madc32.o $(LIB_TARGETS)
		$(CC) -o madc32 madc32.o \
			-L ../../../lib/$(GSI_OS_VERSION) \
			-L /lib/ces \
			-lMadc32 -lUti \
			$(LIB)/lib_mbs.a $(LIB)/lib_tools.a \
			-llynx -lvme -lbma -luio -lnetinet
		mv madc32 $(BIN_DIR)

clean:
	rm -f *.o core core.* $(LIB_TARGETS)

depend:
	gcc -MM $(CFLAGS) $(CPPFLAGS) *.c

# ------------ Dependencies --------------------------------------------

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

LIB_TARGETS = ../../../lib/$(GSI_OS_VERSION)/libMqdc32.a

libMqdc32.OBJS = mqdc32fct.o block.o

# ------------ Suffix Rules -------------------------------------------

.SUFFIXES:

# ----- C Code ---------
%.o: %.c
	$(CC) $< -c $(CFLAGS) -o $@

.SUFFIXES: .o .c

# ------------ Targets -------------------------------------------------

all: $(LIB_TARGETS) mqdc32

$(LIB_TARGETS): $(libMqdc32.OBJS)
	$(AR) $(ARFLAGS) $@ $^
	ranlib $@

mqdc32:	mqdc32.o $(LIB_TARGETS)
		$(CC) -o mqdc32 mqdc32.o \
			-L ../../../lib/$(GSI_OS_VERSION) \
			-L /lib/ces \
			-lMqdc32 -lUti -llynx -lvme -lbma -luio -lnetinet

clean:
	rm -f *.o core core.* $(LIB_TARGETS)

depend:
	gcc -MM $(CFLAGS) $(CPPFLAGS) *.c

# ------------ Dependencies --------------------------------------------

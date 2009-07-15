CC = gcc 

PPCINC 		=	../../../include
CESINC		=	/usr/include/ces
MBSINC		=	$(MBSROOT)/inc

CFLAGS	=	-pipe -O3 -ansi -g -w -DR__LYNXOS -DCPU_TYPE_$(GSI_CPU_PLATFORM) -DLYNX_VERSION_$(LV_CPP) -I../inc -I$(PPCINC) -I$(MBSINC) -I$(CESINC)

# ------------ Libaries ------------------------------------------------

AR = ar
ARFLAG = -rv

# ------------ Library-Objects ----------------------------------------

LIB_TARGETS = ../../../lib/$(LYNX_VERSION)/libCaen_v1190.a

libCaen_v1190.OBJS = caen_v1190_fct.o

# ------------ Suffix Rules -------------------------------------------

.SUFFIXES:

# ----- C Code ---------
%.o: %.c
	$(CC) $< -c $(CFLAGS) -o $@ 

.SUFFIXES: .o .c 

# ------------ Targets -------------------------------------------------

all: $(LIB_TARGETS) caen_v1190

$(LIB_TARGETS): $(libCaen_v1190.OBJS)
	$(AR) $(ARFLAGS) $@ $^
	ranlib $@

caen_v1190:	caen_v1190.o $(LIB_TARGETS)
		$(CC) -o caen_v1190 caen_v1190.o \
			-L ../../../lib/$(LYNX_VERSION) \
			-L /lib/ces \
			-lCaen_v1190 -lUti -lvme -lnetinet
		cp caen_v1190 ../../../bin/$(LYNX_VERSION)

clean:
	rm -f *.o core core.* $(LIB_TARGETS)

depend:
	gcc -MM $(CFLAGS) $(CPPFLAGS) *.c

# ------------ Dependencies --------------------------------------------

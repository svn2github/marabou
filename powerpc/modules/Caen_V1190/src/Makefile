CC = gcc

PPCINC 		=	../../../include
CESINC		=	/usr/include/ces
UTIINC		=	../../../utils/inc
MBSINC		=	$(MBSROOT)/inc

CFLAGS	=	-pipe -O3 -ansi -g -w -DR__LYNXOS -DCPU_TYPE_$(GSI_CPU_PLATFORM) -DLYNX_VERSION_$(LV_CPP) -I$(MBSROOT)/inc -I../inc -I$(UTIINC) -I$(PPCINC) -I$(MBSINC) -I$(CESINC)

BIN_DIR = ../../../bin/$(GSI_OS_VERSION)

# ------------ Libaries ------------------------------------------------

AR = ar
ARFLAG = -rv

LIB		= $(MBSROOT)/lib_$(GSI_CPU_PLATFORM)

ifeq ($(GSI_CPU_PLATFORM),RIO2)
MBSLIBS	= -llynx -lvme -lbma -luio -lnetinet
else
MBSLIBS	= -llynx -lvme -lbma -lbus -luio -lnetinet
endif

# ------------ Library-Objects ----------------------------------------

ObjSuf		=	o
SrcSuf		=	c
LibSuf		=	a

LIBCAEN		=	../../../lib/$(GSI_OS_VERSION)/libCaen_v1x90.$(LibSuf)

OBJS		=	$(LIBCAEN)(caen_v1x90_fct.$(ObjSuf))

PROGS		=	caen_v1x90

.SUFFIXES: .$(SrcSuf)

all:			$(LIBCAEN) $(PROGS)

$(LIBCAEN):  	$(OBJS)
	ranlib $@
	cp ../inc/*.h ../../../include

caen_v1x90:	caen_v1x90.o $(LIBMADC32)
	$(CC) -o caen_v1x90 caen_v1x90.o \
		-L ../../../lib/$(GSI_OS_VERSION) \
		-L /lib/ces \
		-lCaen_v1x90 -lUti \
		$(LIB)/lib_mbs.a $(LIB)/lib_tools.a \
		$(MBSLIBS)
	mv caen_v1x90 $(BIN_DIR)
	
.$(SrcSuf).$(ObjSuf):
			$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o core core.* $(LIBCAEN)

depend:
	gcc -MM $(CFLAGS) $(CPPFLAGS) *.c

# ------------ Dependencies --------------------------------------------


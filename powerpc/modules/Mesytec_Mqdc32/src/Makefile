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

LIBMQDC32		=	../../../lib/$(GSI_OS_VERSION)/libMqdc32.$(LibSuf)

OBJS		=	$(LIBMQDC32)(mqdc32fct.$(ObjSuf))

PROGS		=	mqdc32

.SUFFIXES: .$(SrcSuf)

all:			$(LIBMQDC32) $(PROGS)

$(LIBMQDC32):  	$(OBJS)
	ranlib $@
	cp ../inc/*.h $(PPCINC)

mqdc32:	mqdc32.o $(LIBMQDC32)
	$(CC) -o mqdc32 mqdc32.o \
		-L ../../../lib/$(GSI_OS_VERSION) \
		-L /lib/ces \
		-lMqdc32 -lMesy -lUti \
		$(LIB)/lib_mbs.a $(LIB)/lib_tools.a \
		$(MBSLIBS)
	mv mqdc32 $(BIN_DIR)
	
.$(SrcSuf).$(ObjSuf):
			$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o core core.* $(LIBMQDC32)

depend:
	gcc -MM $(CFLAGS) $(CPPFLAGS) *.c

# ------------ Dependencies --------------------------------------------


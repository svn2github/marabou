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

LIBMTDC32		=	../../../lib/$(GSI_OS_VERSION)/libMtdc32.$(LibSuf)

OBJS		=	$(LIBMTDC32)(mtdc32fct.$(ObjSuf))

PROGS		=	mtdc32

.SUFFIXES: .$(SrcSuf)

all:			$(LIBMTDC32) $(PROGS)

$(LIBMTDC32):  	$(OBJS)
	ranlib $@
	cp ../inc/*.h $(PPCINC)

mtdc32:	mtdc32.o $(LIBMTDC32)
	$(CC) -o mtdc32 mtdc32.o \
		-L ../../../lib/$(GSI_OS_VERSION) \
		-L /lib/ces \
		-lMtdc32 -lMesy -lUti \
		$(LIB)/lib_mbs.a $(LIB)/lib_tools.a \
		$(MBSLIBS)
	mv mtdc32 $(BIN_DIR)
	
.$(SrcSuf).$(ObjSuf):
			$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o core core.* $(LIBMTDC32)

depend:
	gcc -MM $(CFLAGS) $(CPPFLAGS) *.c

# ------------ Dependencies --------------------------------------------


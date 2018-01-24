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

LIBMESY		=	../../../lib/$(GSI_OS_VERSION)/libMesy.$(LibSuf)

OBJS		=	$(LIBMESY)(mesy_common_fct.$(ObjSuf))

.SUFFIXES: .$(SrcSuf)

all:			$(LIBMESY)

$(LIBMESY):  	$(OBJS)
	ranlib $@
	cp ../inc/*.h $(PPCINC)

.$(SrcSuf).$(ObjSuf):
			$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o core core.* $(LIBMESY)

depend:
	gcc -MM $(CFLAGS) $(CPPFLAGS) *.c

# ------------ Dependencies --------------------------------------------


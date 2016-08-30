CC = gcc

PPCINC 		=	../../../include
CESINC		=	/usr/include/ces
UTIINC		=	../../../utils/inc
C2LINC		=	../../../../c2lynx/inc
MBSINC		=	$(MBSROOT)/inc

CFLAGS	=	-pipe -O3 -ansi -g -w -DR__LYNXOS -DCPU_TYPE_$(GSI_CPU_PLATFORM) -DLYNX_VERSION_$(LV_CPP) -I$(MBSROOT)/inc -I../inc -I$(UTIINC) -I$(PPCINC) -I$(MBSINC) -I$(CESINC) -I$(C2LINC)

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

LIBSIS3302		=	../../../lib/$(GSI_OS_VERSION)/libSis_3302.$(LibSuf)

OBJS		=	$(LIBSIS3302)(sis_3302_fct.$(ObjSuf)) $(LIBSIS3302)(sis_3302_readout.$(ObjSuf))

.SUFFIXES: .$(SrcSuf)

all:			$(LIBSIS3302) sis3302

$(LIBSIS3302):  	$(OBJS)
	ranlib $@
	cp ../inc/*.h ../../../include

.$(SrcSuf).$(ObjSuf):
			$(CC) $(CFLAGS) -c $<

sis3302:	sis3302.o $(LIBSIS3302)
	$(CC) -o sis3302 sis3302.o \
		-L ../../../lib/$(GSI_OS_VERSION) \
		-L /lib/ces \
		-lSis_3302 -lUti \
		$(LIB)/lib_mbs.a $(LIB)/lib_tools.a \
		$(MBSLIBS)
	mv sis3302 $(BIN_DIR)
	
clean:
	rm -f *.o core core.* $(LIBSIS3302)

depend:
	gcc -MM $(CFLAGS) $(CPPFLAGS) *.c

# ------------ Dependencies --------------------------------------------

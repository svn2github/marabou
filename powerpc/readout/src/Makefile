CC 			=	gcc

RDOINC		=	../inc
PPCINC		=	../../include
CESINC		=	/usr/include/ces

CFLAGS	=	-g -w -DR__LYNXOS -DCPU_TYPE_$(GSI_CPU_PLATFORM) -DLYNX_VERSION_$(LV_CPP) -I$(RDOINC) -I$(PPCINC) -I$(CESINC)

ObjSuf		=	o
SrcSuf		=	c
LibSuf		=	a
RDOLIB		=	../../lib/$(GSI_OS_VERSION)/libRdo.$(LibSuf)

BINDIR		=	../../bin/$(GSI_OS_VERSION)

INCS		=	$(RDOINC)/gd_camio.h \
				$(RDOINC)/gd_cnaf.h \
				$(RDOINC)/gd_readout.h \
				$(RDOINC)/gd_tstamp.h

OBJS		=	$(RDOLIB)(cc32_init.$(ObjSuf)) $(RDOLIB)(dgf_download.$(ObjSuf))

PROGS		=	caen \
				cc32 \
				cptm \
				dgfdown \
				histoDump \
				sis3801 \
				testParam

.SUFFIXES: .$(SrcSuf)

all:			$(RDOLIB) $(PROGS)

clean:
			rm -f *.o $(RDOLIB)

$(RDOLIB):  	$(OBJS)
	cp ../inc/*.h ../../include
	$(AR) $(ARFLAGS) $@ $^
	ranlib $@

caen:			caen.o $(RDOLIB)
				$(CC) -o caen caen.o \
				-L ../../lib/$(GSI_OS_VERSION) \
				-L /lib/ces \
				-lUti -lvme -lnetinet
				mv caen $(BINDIR)

sis3801:		sis3801.o $(RDOLIB)
				$(CC) -o sis3801 sis3801.o \
				-L ../../lib/$(GSI_OS_VERSION) \
				-L /lib/ces \
				-lRdo -lUti -lvme -lnetinet
				mv sis3801 $(BINDIR)

cc32:			cc32.o $(RDOLIB)
				$(CC) -o cc32 cc32.o \
				-L ../../lib/$(GSI_OS_VERSION) \
				-L /lib/ces \
				-lRdo -lUti -lvme -lnetinet
				mv cc32 $(BINDIR)

cptm:			cptm.o $(RDOLIB)
				$(CC) -o cptm cptm.o \
				-L ../../lib/$(GSI_OS_VERSION) \
				-L /lib/ces \
				-lRdo -lUti -lvme -lnetinet
				mv cptm $(BINDIR)

dgfdown:		dgfdown.o $(RDOLIB)
				$(CC) -o dgfdown dgfdown.o \
				-L ../../lib/$(GSI_OS_VERSION) \
				-L /lib/ces \
				-lRdo -lUti -lvme -lnetinet
				mv dgfdown $(BINDIR)

histoDump:		histoDump.o $(RDOLIB)
				$(CC) -o histoDump histoDump.o \
				-L ../../lib/$(GSI_OS_VERSION) \
				-L /lib/ces \
				-lRdo -lUti -lvme -lnetinet
				mv histoDump $(BINDIR)

testParam:		testParam.o $(RDOLIB)
				$(CC) -o testParam testParam.o \
				-L ../../lib/$(GSI_OS_VERSION) \
				-L /lib/ces \
				-lRdo -lUti -lvme -lnetinet
				mv testParam $(BINDIR)

.$(SrcSuf).$(ObjSuf):
			$(CC) $(CFLAGS) -c $<

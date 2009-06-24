CC 			=	gcc

UTIINC		=	../inc
PPCINC		=	../../include
CESINC		=	/usr/include/ces

CFLAGS	=	-g -w -DR__LYNXOS -DCPU_TYPE_$(GSI_CPU_PLATFORM) -DLYNX_VERSION_$(LV_CPP) -I$(UTIINC) -I$(PPCINC) -I$(CESINC) 

ObjSuf		=	o
SrcSuf		=	c
LibSuf		=	a
UTILIB		=	../../lib/$(LYNX_VERSION)/libUti.$(LibSuf)

BINDIR		=	../../bin/$(LYNX_VERSION)

INCS		=	$(UTIINC)/gd_camio.h \
				$(UTIINC)/gd_cnaf.h \
				$(UTIINC)/gd_readout.h \
				$(UTIINC)/gd_tstamp.h \
				$(UTIINC)/root_env.h \
				$(UTIINC)/unix_string.h


OBJS		=	$(UTILIB)(cc32_init.$(ObjSuf)) \
				$(UTILIB)(dgf_download.$(ObjSuf)) \
				$(UTILIB)(root_env.$(ObjSuf)) \
				$(UTILIB)(unix_string.$(ObjSuf))

PROGS		=	caen \
				cc32 \
				cptm \
				dgfdown \
				histoDump \
				sis3801 \
				testParam \
				envtst \
				uxtst

.SUFFIXES: .$(SrcSuf)

all:			$(UTILIB) $(PROGS)
 
clean:
				rm -f *.o $(UTILIB)

$(UTILIB):  	$(OBJS)

caen:			caen.o $(UTILIB)
				$(CC) -o caen caen.o \
				-L ../../lib/$(LYNX_VERSION) \
				-L /lib/ces \
				-lUti -lvme -lnetinet
				mv caen $(BINDIR)

sis3801:		sis3801.o $(UTILIB)
				$(CC) -o sis3801 sis3801.o \
				-L ../../lib/$(LYNX_VERSION) \
				-L /lib/ces \
				-lUti -lvme -lnetinet
				mv sis3801 $(BINDIR)

cc32:			cc32.o $(UTILIB)
				$(CC) -o cc32 cc32.o \
				-L ../../lib/$(LYNX_VERSION) \
				-L /lib/ces \
				-lUti -lvme -lnetinet
				mv cc32 $(BINDIR)

cptm:			cptm.o $(UTILIB)
				$(CC) -o cptm cptm.o \
				-L ../../lib/$(LYNX_VERSION) \
				-L /lib/ces \
				-lUti -lvme -lnetinet
				mv cptm $(BINDIR)

dgfdown:		dgfdown.o $(UTILIB)
				$(CC) -o dgfdown dgfdown.o \
				-L ../../lib/$(LYNX_VERSION) \
				-L /lib/ces \
				-lUti -lvme -lnetinet
				mv dgfdown $(BINDIR)

histoDump:		histoDump.o $(UTILIB)
				$(CC) -o histoDump histoDump.o \
				-L ../../lib/$(LYNX_VERSION) \
				-L /lib/ces \
				-lUti -lvme -lnetinet
				mv histoDump $(BINDIR)

testParam:		testParam.o $(UTILIB)
				$(CC) -o testParam testParam.o \
				-L ../../lib/$(LYNX_VERSION) \
				-L /lib/ces \
				-lUti -lvme -lnetinet
				mv testParam $(BINDIR)

envtst:			envtst.o $(UTILIB)
				$(CC) -o envtst envtst.o \
				-L ../../lib/$(LYNX_VERSION) \
				-L /lib/ces \
				-lUti -lvme -lnetinet
				mv envtst $(BINDIR)

uxtst:			uxtst.o $(UTILIB)
				$(CC) -o uxtst uxtst.o \
				-L ../../lib/$(LYNX_VERSION) \
				-L /lib/ces \
				-lUti -lvme -lnetinet
				mv uxtst $(BINDIR)

.$(SrcSuf).$(ObjSuf):
			$(CC) $(CFLAGS) -c $<

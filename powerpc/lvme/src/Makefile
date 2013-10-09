CC 			=	gcc

RDOINC		=	../inc
PPCINC		=	../../include
CESINC		=	/usr/include/ces

CFLAGS	=	-g -w -DR__LYNXOS -DCPU_TYPE_$(GSI_CPU_PLATFORM) -DLYNX_VERSION_$(LV_CPP) -I$(RDOINC) -I$(PPCINC) -I$(CESINC)

ObjSuf		=	o
SrcSuf		=	c
LibSuf		=	a

LVMELIB		=	../../lib/$(GSI_OS_VERSION)/libLvme.$(LibSuf)

OBJS		=	$(LVMELIB)(lvme.$(ObjSuf)) $(LVMELIB)(xvmelib.$(ObjSuf))

.SUFFIXES: .$(SrcSuf)

all:			$(LVMELIB)

clean:
			rm -f *.o $(LVMELIB)

$(LVMELIB):  	$(OBJS)
	ranlib $@
	cp ../inc/*.h ../../include

.$(SrcSuf).$(ObjSuf):
			$(CC) $(CFLAGS) -c $<

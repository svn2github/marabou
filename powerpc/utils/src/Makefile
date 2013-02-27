CC 			=	gcc

UTIINC		=	../inc
PPCINC		=	../../include
CESINC		=	/usr/include/ces

CFLAGS	=	-g -w -DR__LYNXOS -DCPU_TYPE_$(GSI_CPU_PLATFORM) -DLYNX_VERSION_$(LV_CPP) -I$(UTIINC) -I$(PPCINC) -I$(CESINC)

ObjSuf		=	o
SrcSuf		=	c
LibSuf		=	a
UTILIB		=	../../lib/$(GSI_OS_VERSION)/libUti.$(LibSuf)

BINDIR		=	../../bin/$(GSI_OS_VERSION)

INCS		=	$(UTIINC)/root_env.h $(UTIINC)/unix_string.h $(UTIINC)/block.h


OBJS		=	$(UTILIB)(mapping.$(ObjSuf)) $(UTILIB)(root_env.$(ObjSuf)) $(UTILIB)(unix_string.$(ObjSuf)) $(UTILIB)(block.$(ObjSuf)) $(UTILIB)(xvmelib.$(ObjSuf))

PROGS		=	envtst uxtst

.SUFFIXES: .$(SrcSuf)

all:			$(UTILIB) $(PROGS)

$(UTILIB):  	$(OBJS)
	cp ../inc/*.h ../../include
	$(AR) $(ARFLAGS) $@ $^
	ranlib $@

envtst:			envtst.o $(UTILIB)
				$(CC) -o envtst envtst.o \
				-L ../../lib/$(GSI_OS_VERSION) \
				-L /lib/ces \
				-lUti -lvme -lnetinet
				mv envtst $(BINDIR)

uxtst:			uxtst.o $(UTILIB)
				$(CC) -o uxtst uxtst.o \
				-L ../../lib/$(GSI_OS_VERSION) \
				-L /lib/ces \
				-lUti -lvme -lnetinet
				mv uxtst $(BINDIR)

.$(SrcSuf).$(ObjSuf):
			$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o core core.* $(UTILIB)

depend:
	gcc -MM $(CFLAGS) $(CPPFLAGS) *.c

# ------------ Dependencies --------------------------------------------

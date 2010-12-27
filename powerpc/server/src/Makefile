CXX 		=	g++

PPCINC 		=	../../include
SRVINC 		=	../inc
LWRINC		=	../../lwroot/inc
M2LINC		=	../../../c2lynx/inc
CESINC		=	/usr/include/ces

CXXFLAGS	=	-g -w -DR__LYNXOS -DCPU_TYPE_$(GSI_CPU_PLATFORM) -DLYNX_VERSION_$(LV_CPP) -I$(SRVINC) -I$(LWRINC) -I$(M2LINC) -I$(PPCINC) -I$(CESINC)

IclSuf		=	h
ObjSuf		=	o
SrcSuf		=	cxx
LibSuf		=	a

LWRLIB		=	../../lib/$(GSI_OS_VERSION)/libLwr.$(LibSuf)
SRVLIB		=	../../lib/$(GSI_OS_VERSION)/libSrv.$(LibSuf)

INCS		=	$(SRVINC)/SrvUtils.$(IclSuf) \
				$(SRVINC)/SrvSocket.$(IclSuf) \
				$(SRVINC)/SrvVMEModule.$(IclSuf) \
				$(SRVINC)/SrvSis3302.$(IclSuf) \
				$(SRVINC)/SrvCaen785.$(IclSuf) \
				$(SRVINC)/SrvVulomTB.$(IclSuf)

OBJS		=	$(SRVLIB)(SrvSocket.$(ObjSuf)) \
				$(SRVLIB)(SrvSis3302.$(ObjSuf)) \
				$(SRVLIB)(SrvCaen785.$(ObjSuf)) \
				$(SRVLIB)(SrvVulomTB.$(ObjSuf)) \
				$(SRVLIB)(SrvVMEModule.$(ObjSuf)) \
				$(SRVLIB)(SrvUtils.$(ObjSuf))

SRVPRG		=	../bin/$(GSI_OS_VERSION)/mrbLynxOsSrv

.SUFFIXES: .$(SrcSuf)

all:			$(SRVLIB) $(SRVPRG)

clean:
				rm -f *.o $(SRVLIB)

$(SRVPRG):		mrbLynxOsSrv.o $(SRVLIB) $(LWRLIB)
				$(CXX) -o mrbLynxOsSrv mrbLynxOsSrv.o \
				-L ../../lib/$(GSI_OS_VERSION) \
				-L /lib/ces \
				-lSrv -lLwr -lvme -llynx -lnetinet
				mv mrbLynxOsSrv ../../bin/$(GSI_OS_VERSION)


$(SRVLIB):   $(INCS) $(OBJS)
				ranlib $@

.$(SrcSuf).$(ObjSuf):
			$(CXX) $(CXXFLAGS) -c $<

CXX 		=	g++

PPCINC 		=	../../include
SRVINC 		=	../inc
LWRINC		=	../../lwroot/inc
M2LINC		=	../../../c2lynx/inc
CESINC		=	/usr/include/ces

CXXFLAGS	=	-g -w -DR__LYNXOS -DCPU_TYPE_$(GSI_CPU_PLATFORM) -DLYNX_VERSION_$(LV_CPP) -I$(MBSROOT)/inc -I$(SRVINC) -I$(LWRINC) -I$(M2LINC) -I$(PPCINC) -I$(CESINC)

IclSuf		=	h
ObjSuf		=	o
SrcSuf		=	cxx
LibSuf		=	a

LWRLIB		=	../../lib/$(GSI_OS_VERSION)/libLwr.$(LibSuf)
UTILIB		=	../../lib/$(GSI_OS_VERSION)/libUti.$(LibSuf)
SRVLIB		=	../../lib/$(GSI_OS_VERSION)/libSrv.$(LibSuf)

ifeq ($(GSI_CPU_PLATFORM),RIO2)
MBSLIBS		= -lUti -lvme -lbma -luio -llynx -lnetinet
else
MBSLIBS		= -lUti -lvme -lbma -lbus -luio -llynx -lnetinet
endif

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

$(SRVPRG):		mrbLynxOsSrv.o $(SRVLIB) $(LWRLIB) $(UTILIB)
				$(CXX) -o mrbLynxOsSrv mrbLynxOsSrv.o \
				-L ../../lib/$(GSI_OS_VERSION) \
				-L /lib/ces \
				-lSrv -lLwr $(MBSROOT)/lib_$(GSI_CPU_PLATFORM)/lib_mbs.a $(MBSLIBS)
				mv mrbLynxOsSrv ../../bin/$(GSI_OS_VERSION)

$(SRVLIB):   $(INCS) $(OBJS)
				ranlib $@
				cp ../inc/*.h ../../include

.$(SrcSuf).$(ObjSuf):
			$(CXX) $(CXXFLAGS) -c $<

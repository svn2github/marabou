CXX 		=	g++

PPCINC		=	../../include
LWRINC 		=	../inc
M2LINC		=	../../../c2lynx/inc

CXXFLAGS	=	-g -w -DR__LYNXOS -DCPU_TYPE_$(GSI_CPU_PLATFORM) -DLYNX_VERSION_$(LV_CPP) -I$(MBSROOT)/inc -I/usr/lib/g++-include -I$(LWRINC) -I$(M2LINC) -I$(PPCINC)

ObjSuf		=	o
SrcSuf		=	cxx
LibSuf		=	a
LWRLIB		=	../../lib/$(GSI_OS_VERSION)/libLwr.$(LibSuf)

AR = ar
ARFLAG = -rv

INCS		=	$(LWRINC)/LwrUtils.h \
				$(LWRINC)/LwrObject.h \
				$(LWRINC)/LwrNamed.h \
				$(LWRINC)/LwrCollection.h \
				$(LWRINC)/LwrSeqCollection.h \
				$(LWRINC)/LwrList.h \
				$(LWRINC)/LwrObjString.h \
				$(LWRINC)/LwrBytes.h \
				$(LWRINC)/LwrArray.h \
				$(LWRINC)/LwrArrayI.h \
				$(LWRINC)/LwrConfig.h \
				$(LWRINC)/LwrInetAddress.h \
				$(LWRINC)/LwrRefCnt.h \
				$(LWRINC)/LwrTypes.h \
				$(LWRINC)/LwrServerSocket.h \
				$(LWRINC)/LwrSocket.h \
				$(LWRINC)/LwrFileHandler.h \
				$(LWRINC)/LwrMonitor.h \
				$(LWRINC)/LwrString.h \
				$(LWRINC)/LwrLynxOsSystem.h \
				$(LWRINC)/LwrVarargs.h \
				$(LWRINC)/LwrNamedX.h \
				$(LWRINC)/LwrLofNamedX.h \
				$(LWRINC)/LwrDatime.h \
				$(LWRINC)/LwrLogger.h \
				$(M2LINC)/M2L_MessageTypes.h \
				$(M2LINC)/M2L_CommonStructs.h


OBJS		=	$(LWRLIB)(LwrUtils.$(ObjSuf)) \
				$(LWRLIB)(LwrArray.$(ObjSuf)) \
				$(LWRLIB)(LwrArrayI.$(ObjSuf)) \
				$(LWRLIB)(LwrObject.$(ObjSuf)) \
				$(LWRLIB)(LwrNamed.$(ObjSuf)) \
				$(LWRLIB)(LwrCollection.$(ObjSuf)) \
				$(LWRLIB)(LwrSeqCollection.$(ObjSuf)) \
				$(LWRLIB)(LwrList.$(ObjSuf)) \
				$(LWRLIB)(LwrObjString.$(ObjSuf)) \
				$(LWRLIB)(LwrInetAddress.$(ObjSuf)) \
				$(LWRLIB)(LwrSocket.$(ObjSuf)) \
				$(LWRLIB)(LwrFileHandler.$(ObjSuf)) \
				$(LWRLIB)(LwrMonitor.$(ObjSuf)) \
				$(LWRLIB)(LwrLynxOsSystem.$(ObjSuf))\
				$(LWRLIB)(LwrServerSocket.$(ObjSuf))\
				$(LWRLIB)(LwrString.$(ObjSuf)) \
				$(LWRLIB)(LwrNamedX.$(ObjSuf)) \
				$(LWRLIB)(LwrLofNamedX.$(ObjSuf)) \
				$(LWRLIB)(LwrDatime.$(ObjSuf)) \
				$(LWRLIB)(LwrLogger.$(ObjSuf))

.SUFFIXES: .$(SrcSuf)

all:			$(LWRLIB)

clean:
				rm -f *.o $(LWRLIB)

$(LWRLIB):  $(OBJS)
				ranlib $@
				cp ../inc/*.h ../../include

.$(SrcSuf).$(ObjSuf):
			$(CXX) $(CXXFLAGS) -c $<

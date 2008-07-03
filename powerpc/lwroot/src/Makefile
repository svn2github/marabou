CXX 		=	g++

INC 		=	../inc
M2LINC		=	../../../c2lynx/inc

CXXFLAGS	=	-g -w -DR__LYNXOS -DLYNX_VERSION=$(LV_CPP) -I$(INC) -I$(M2LINC)

ObjSuf		=	o
SrcSuf		=	cxx
LibSuf		=	a
LWRLIB		=	../lib/$(LYNX_VERSION)/libLwr.$(LibSuf)

INCS		=	$(INC)/LwrUtils.h \
				$(INC)/LwrObject.h \
				$(INC)/LwrNamed.h \
				$(INC)/LwrCollection.h \
				$(INC)/LwrSeqCollection.h \
				$(INC)/LwrList.h \
				$(INC)/LwrObjString.h \
				$(INC)/LwrBytes.h \
				$(INC)/LwrArray.h \
				$(INC)/LwrArrayI.h \
				$(INC)/LwrConfig.h \
				$(INC)/LwrInetAddress.h \
				$(INC)/LwrRefCnt.h \
				$(INC)/LwrTypes.h \
				$(INC)/LwrServerSocket.h \
				$(INC)/LwrSocket.h \
				$(INC)/LwrFileHandler.h \
				$(INC)/LwrMonitor.h \
				$(INC)/LwrString.h \
				$(INC)/LwrLynxOsSystem.h \
				$(INC)/LwrVarargs.h \
				$(INC)/LwrNamedX.h \
				$(INC)/LwrLofNamedX.h \
				$(INC)/LwrDatime.h \
				$(INC)/LwrLogger.h \
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
				rm -f *.o ../lib/$(LYNX_VERSION)/*.a

$(LWRLIB):  $(OBJS)

.$(SrcSuf).$(ObjSuf):
			$(CXX) $(CXXFLAGS) -c $<

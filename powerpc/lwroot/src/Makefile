CXX 		=	g++

INC 		=	../inc
M2LINC		=	../../../c2lynx/inc

CXXFLAGS	=	-g -w -DR__LYNXOS -I$(INC) -I$(M2LINC)

ObjSuf		=	o
SrcSuf		=	cxx
LibSuf		=	a
LWRLIB		=	../lib/libLwr.$(LibSuf)

INCS		=	$(INC)/LwrObject.h \
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
				$(INC)/LwrMessage.h \
				$(INC)/LwrRefCnt.h \
				$(INC)/LwrTypes.h \
				$(INC)/LwrServerSocket.h \
				$(INC)/LwrSocket.h \
				$(INC)/LwrString.h \
				$(INC)/LwrUnixSystem.h \
				$(INC)/LwrVarargs.h \
				$(M2LINC)/M2L_MessageTypes.h \
				$(M2LINC)/M2L_CommonStructs.h


OBJS		=	$(LWRLIB)(LwrArray.$(ObjSuf)) \
				$(LWRLIB)(LwrArrayI.$(ObjSuf)) \
				$(LWRLIB)(LwrObject.$(ObjSuf)) \
				$(LWRLIB)(LwrNamed.$(ObjSuf)) \
				$(LWRLIB)(LwrCollection.$(ObjSuf)) \
				$(LWRLIB)(LwrSeqCollection.$(ObjSuf)) \
				$(LWRLIB)(LwrList.$(ObjSuf)) \
				$(LWRLIB)(LwrObjString.$(ObjSuf)) \
				$(LWRLIB)(LwrMessage.$(ObjSuf)) \
				$(LWRLIB)(LwrInetAddress.$(ObjSuf)) \
				$(LWRLIB)(LwrSocket.$(ObjSuf)) \
				$(LWRLIB)(LwrUnixSystem.$(ObjSuf))\
				$(LWRLIB)(LwrServerSocket.$(ObjSuf))\
				$(LWRLIB)(LwrString.$(ObjSuf)) \
				$(LWRLIB)(LwrNamedX.$(ObjSuf)) \
				$(LWRLIB)(LwrLofNamedX.$(ObjSuf))

.SUFFIXES: .$(SrcSuf)

all:			libNR.$(LibSuf)  
 
clean:
				rm -f *.o ../lib/*.a

tst:			tst.o $(LWRLIB)
				$(CXX) -o tst tst.o -L ../lib -lNR

libNR.$(LibSuf):  $(OBJS)

.$(SrcSuf).$(ObjSuf):
			$(CXX) $(CXXFLAGS) -c $<

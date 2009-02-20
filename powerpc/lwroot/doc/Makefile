SRC 	=	../src/LwrArray.cxx \
			../src/LwrArrayI.cxx \
			../src/LwrCollection.cxx \
			../src/LwrDatime.cxx \
			../src/LwrFileHandler.cxx \
			../src/LwrInetAddress.cxx \
			../src/LwrList.cxx \
			../src/LwrLofNamedX.cxx \
			../src/LwrLogger.cxx \
			../src/LwrLynxOsSystem.cxx \
			../src/LwrMessage.cxx \
			../src/LwrMonitor.cxx \
			../src/LwrNamed.cxx \
			../src/LwrNamedX.cxx \
			../src/LwrObject.cxx \
			../src/LwrObjString.cxx \
			../src/LwrSeqCollection.cxx \
			../src/LwrServerSocket.cxx \
			../src/LwrSocket.cxx \
			../src/LwrString.cxx \
			../src/LwrUnixSystem.cxx \
			../src/LwrUtils.cxx

INC 	=	../inc/LwrArray.h \
			../inc/LwrArrayI.h \
			../inc/LwrBytes.h \
			../inc/LwrCollection.h \
			../inc/LwrConfig.h \
			../inc/LwrDatime.h \
			../inc/LwrFileHandler.h \
			../inc/LwrInetAddress.h \
			../inc/LwrIterator.h \
			../inc/LwrList.h \
			../inc/LwrLofNamedX.h \
			../inc/LwrLogger.h \
			../inc/LwrLynxOsSystem.h \
			../inc/LwrMessage.h \
			../inc/LwrMonitor.h \
			../inc/LwrNamed.h \
			../inc/LwrNamedX.h \
			../inc/LwrObject.h \
			../inc/LwrObjString.h \
			../inc/LwrRefCnt.h \
			../inc/LwrSeqCollection.h \
			../inc/LwrServerSocket.h \
			../inc/LwrSetColor.h \
			../inc/LwrSocket.h \
			../inc/LwrString.h \
			../inc/LwrSwap.h \
			../inc/LwrTypes.h \
			../inc/LwrUnixSystem.h \
			../inc/LwrUtils.h \
			../inc/LwrVarargs.h

all:		$(INC) $(SRC) lwr.cfg
			@rm -f doxygen.err
			@doxygen lwr.cfg 2>doxygen.err
			@if [ `cat doxygen.err | wc -l` -gt 0 ]; then nedit doxygen.err; fi

clean:
			touch $(SRC) ($INC)



#ifndef __LwrLynxOsSystem_h__
#define __LwrLynxOsSystem_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrLynxOsSystem.h
//! \brief			Light Weight ROOT
//! \details		Class definitions for ROOT under LynxOs: TLynxOsSystem
//! 				Class providing an interface to the LynxOs Operating System.
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.2 $     
//! $Date: 2009-02-03 08:29:20 $
//////////////////////////////////////////////////////////////////////////////

#include <errno.h>
#include "LwrTypes.h"
#include "LwrNamed.h"
#include "LwrList.h"
#include "LwrInetAddress.h"
#include "LwrSeqCollection.h"
#include "LwrFileHandler.h"

class TFileHandler;
class TFdSet;

class TLynxOsSystem: public TNamed {

protected:
   // static functions providing semi-low level interface to raw Unix
   int          UnixTcpConnect(const char *hostname, int port);
   static int          UnixUnixConnect(int port);
   int          UnixTcpService(int port, Bool_t reuse, int backlog);
   static int          UnixUnixService(int port, int backlog);
   int          UnixRecv(int sock, void *buf, int len, int flag);
   static int          UnixSend(int sock, const void *buf, int len, int flag);

public:
   TLynxOsSystem();
   virtual ~TLynxOsSystem();

   //---- Misc -------------------------------------------------
   const char       *HostName();

   //---- RPC --------------------------------------------------

   TInetAddress      GetHostByName(const char *server);
   TInetAddress      GetPeerName(int sock);
   TInetAddress      GetSockName(int sock);
   int               GetServiceByName(const char *service);
   char             *GetServiceByPort(int port);
   int               ConnectService(const char *server, int port);
   int               OpenConnection(const char *server, int port);
   int               AnnounceTcpService(int port, Bool_t reuse, int backlog);
   int               AnnounceUnixService(int port, int backlog);
   int               AcceptConnection(int sock);
   void              CloseConnection(int sock);
   int               RecvRaw(int sock, void *buffer, int length, int flag);
   int               SendRaw(int sock, const void *buffer, int length, int flag);
   int               RecvBuf(int sock, void *buffer, int length);
   int               SendBuf(int sock, const void *buffer, int length);
   int               SetSockOpt(int sock, int option, int val);
   int               GetSockOpt(int sock, int option, int *val);
   int               GetErrno(){return errno;};
   void              ResetErrno(){errno =0;};

   void              AddFileHandler(TFileHandler * FileHandler);
   TFileHandler     *RemoveFileHandler(TFileHandler * FileHandler);

   Int_t             Select(TList *act, Long_t to);
   Int_t             UnixSelect(Int_t nfds, TFdSet *readready, TFdSet *writeready, Long_t timeout);

   inline const Char_t * ClassName() const { return "TLynxOsSystem"; };

protected:
   TList            *fFileHandler;    // List of file handlers

};

#endif

#ifndef __LwrUnixSystem_h__
#define __LwrUnixSystem_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \brief			Light Weight ROOT
//! \details		Class definitions for ROOT under LynxOs: TUnixSystem<br>
//! 				Class providing an interface to the UNIX Operating System
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.4 $     
//! $Date: 2009-02-17 08:02:26 $
//////////////////////////////////////////////////////////////////////////////

#include <errno.h>
#include "LwrTypes.h"
#include "LwrNamed.h"
#include "LwrInetAddress.h"

class TUnixSystem: public TNamed {

protected:
   // static functions providing semi-low level interface to raw Unix
   Int_t          UnixTcpConnect(const Char_t *hostname, Int_t port);
   static Int_t          UnixUnixConnect(Int_t port);
   Int_t          UnixTcpService(Int_t port, Bool_t reuse, Int_t backlog);
   static Int_t          UnixUnixService(Int_t port, Int_t backlog);
   Int_t          UnixRecv(Int_t sock, void *buf, Int_t len, Int_t flag);
   static Int_t          UnixSend(Int_t sock, const void *buf, Int_t len, Int_t flag);

public:
   TUnixSystem();
   virtual ~TUnixSystem();

   //---- Misc -------------------------------------------------
   const Char_t       *HostName();

   //---- RPC --------------------------------------------------

   TInetAddress      GetHostByName(const Char_t *server);
   TInetAddress      GetPeerName(Int_t sock);
   TInetAddress      GetSockName(Int_t sock);
   Int_t               GetServiceByName(const Char_t *service);
   Char_t             *GetServiceByPort(Int_t port);
   Int_t               ConnectService(const Char_t *server, Int_t port);
   Int_t               OpenConnection(const Char_t *server, Int_t port);
   Int_t               AnnounceTcpService(Int_t port, Bool_t reuse, Int_t backlog);
   Int_t               AnnounceUnixService(Int_t port, Int_t backlog);
   Int_t               AcceptConnection(Int_t sock);
   void              CloseConnection(Int_t sock);
   Int_t               RecvRaw(Int_t sock, void *buffer, Int_t length, Int_t flag);
   Int_t               SendRaw(Int_t sock, const void *buffer, Int_t length, Int_t flag);
   Int_t               RecvBuf(Int_t sock, void *buffer, Int_t length);
   Int_t               SendBuf(Int_t sock, const void *buffer, Int_t length);
   Int_t               SetSockOpt(Int_t sock, Int_t option, Int_t val);
   Int_t               GetSockOpt(Int_t sock, Int_t option, Int_t *val);
   Int_t               GetErrno(){return errno;};
   void              ResetErrno(){errno =0;};

   inline const Char_t * ClassName() const { return "TUnixSystem"; };
};

#endif

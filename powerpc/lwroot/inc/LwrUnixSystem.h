#ifndef __LwrUnixSystem_h__
#define __LwrUnixSystem_h__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TUnixSystem                                                          //
//                                                                      //
// Class providing an interface to the UNIX Operating System.           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
// Special 'Light Weight ROOT' edition for LynxOs                       //
// O. Schaile                                                           //
//////////////////////////////////////////////////////////////////////////

#include <errno.h>
#include "LwrTypes.h"
#include "LwrNamed.h"
#include "LwrInetAddress.h"

class TUnixSystem: public TNamed {

protected:
   // static functions providing semi-low level interface to raw Unix
   int          UnixTcpConnect(const char *hostname, int port);
   static int          UnixUnixConnect(int port);
   int          UnixTcpService(int port, Bool_t reuse, int backlog);
   static int          UnixUnixService(int port, int backlog);
   int          UnixRecv(int sock, void *buf, int len, int flag);
   static int          UnixSend(int sock, const void *buf, int len, int flag);

public:
   TUnixSystem();
   virtual ~TUnixSystem();

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

   inline const Char_t * ClassName() const { return "TUnixSystem"; };
};

#endif

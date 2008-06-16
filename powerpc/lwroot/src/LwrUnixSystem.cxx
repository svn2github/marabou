//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TUnixSystem                                                          //
//                                                                      //
// Class providing an interface to the UNIX Operating System.           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
// Special 'Light weight ROOT' edition                                  //
// O. Schaile                                                           //
//////////////////////////////////////////////////////////////////////////

#include "LwrTypes.h"
#include "LwrUnixSystem.h"
#include "LwrSocket.h"


#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#if defined(R__SUN) || defined(R__SGI) || defined(R__HPUX) || defined(R__AIX) || \
    defined(R__LINUX) || defined(R__SOLARIS) || defined(R__ALPHA) || \
    defined(R__HIUX) || defined(R__FBSD)
#   include <dirent.h>
#else
#   include <sys/dir.h>
#endif
#if defined(ULTRIX) || defined(R__SUN)
#   include <sgtty.h>
#endif
#if defined(R__AIX) || defined(R__LINUX) || defined(R__ALPHA) || defined(R__SGI) || \
    defined(R__HIUX) || defined(R__FBSD) || defined(R__LYNXOS)
#   include <sys/ioctl.h>
#endif
#if defined(R__AIX) || defined(R__SOLARIS)
#   include <sys/select.h>
#endif
#if defined(R__LINUX) && !defined(R__MKLINUX)
#   define SIGSYS  SIGUNUSED       // SIGSYS does not exist in linux ??
#   include <dlfcn.h>
#endif
#include <syslog.h>
#include <sys/stat.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/param.h>
#include <pwd.h>
#include <errno.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#if defined(R__AIX)
#   define _XOPEN_EXTENDED_SOURCE
#   include <arpa/inet.h>
#   undef _XOPEN_EXTENDED_SOURCE
#   if !defined(_AIX41) && !defined(_AIX43)
    // AIX 3.2 doesn't have it
#   define HASNOT_INETATON
#   endif
#else
#   include <arpa/inet.h>
#endif
#include <sys/un.h>
#include <netdb.h>
#include <fcntl.h>
#if defined(R__SGI)
#include <net/soioctl.h>
#endif
#if defined(R__SOLARIS)
#   include <sys/systeminfo.h>
#   include <sys/filio.h>
#   include <sys/sockio.h>
#   define HASNOT_INETATON
#   define INADDR_NONE (UInt_t)-1
#endif
#if defined(R__HPUX)
#   include <symlink.h>
#   include <dl.h>
#   if defined(R__GNU)
       extern "C" {
          extern shl_t cxxshl_load(const char *path, int flags, long address);
          extern int   cxxshl_unload(shl_t handle);
       }
#   elif !defined(__STDCPP__)
#      include <cxxdl.h>
#   endif
    // print stack trace (HP undocumented internal call)
    extern "C" void U_STACK_TRACE();
#   if defined(hpux9)
extern "C" {
   extern void openlog(const char *, int, int);
   extern void syslog(int, const char *, ...);
   extern void closelog(void);
   extern int setlogmask(int);
}
#   define HASNOT_INETATON
#   endif
#endif
#if defined(R__ALPHA) && !defined(R__GNU)
#   define HASNOT_INETATON
#endif
#if defined(R__HIUX)
#   define HASNOT_INETATON
#endif

#if defined(R__SGI) || defined(R__SOLARIS)
#define HAVE_UTMPX_H
#define UTMP_NO_ADDR
#endif
#if defined(R__ALPHA) || defined(R__AIX) || defined(R__FBSD) || defined(R__LYNXOS)
#define UTMP_NO_ADDR
#endif

#if defined(R__LYNXOS)
extern "C" {
   extern int putenv(const char *);
   extern int inet_aton(const char *, struct in_addr *);
};
#endif

#ifdef HAVE_UTMPX_H
#include <utmpx.h>
#define STRUCT_UTMP struct utmpx
#else
#if defined(R__MKLINUX) && !defined(R__PPCEGCS)
        extern "C" {
#endif
#include <utmp.h>
#define STRUCT_UTMP struct utmp
#endif
#if !defined(UTMP_FILE) && defined(_PATH_UTMP)      // 4.4BSD
#define UTMP_FILE _PATH_UTMP
#endif
#if defined(UTMPX_FILE)                             // Solaris, SysVr4
#undef  UTMP_FILE
#define UTMP_FILE UTMPX_FILE
#endif
#ifndef UTMP_FILE
#define UTMP_FILE "/etc/utmp"
#endif

static STRUCT_UTMP *gUtmpContents;


const char *kServerPath     = "/tmp";
const char *kProtocolName   = "tcp";


//______________________________________________________________________________
TUnixSystem::TUnixSystem()
{ }

//______________________________________________________________________________
TUnixSystem::~TUnixSystem()
{
   // Reset to original state.

//   UnixResetSignals();
}

//______________________________________________________________________________
const char *TUnixSystem::HostName()
{
   // Return the system's host name.

      char hn[64];
#if defined(R__SOLARIS) && !defined(R__KCC)
      sysinfo(SI_HOSTNAME, hn, sizeof(hn));
#else
      gethostname(hn, sizeof(hn));
#endif
      return hn;
}

//______________________________________________________________________________
//---- RPC ---------------------------------------------------------------------

//______________________________________________________________________________
TInetAddress TUnixSystem::GetHostByName(const char *hostname)
{
   // Get Internet Protocol (IP) address of host. Returns an TInetAddress
   // object. To see if the hostname lookup was successfull call
   // TInetAddress::IsValid().

   struct hostent *host_ptr;
   struct in_addr  ad;
   const char     *host;
   int             type;
   UInt_t          addr;    // good for 4 byte addresses

#ifdef HASNOT_INETATON
   if ((addr = (UInt_t)inet_addr(hostname)) != INADDR_NONE) {
#else
   if (inet_aton(hostname, &ad)) {
      memcpy(&addr, &ad.s_addr, sizeof(ad.s_addr));
#endif
      if ((host_ptr = gethostbyaddr((const char *)&addr,
                                    sizeof(addr), AF_INET))) {
         host = host_ptr->h_name;
         type = AF_INET;
      } else {
         return TInetAddress("UnknownHost", ntohl(addr), -1);
      }
   } else if ((host_ptr = gethostbyname(hostname))) {
      // Check the address type for an internet host
      if (host_ptr->h_addrtype != AF_INET) {
         cerr << "TUnixSystem::GetHostByName(): " << hostname << " is not an internet host" << endl;
         return TInetAddress();
      }
      memcpy(&addr, host_ptr->h_addr, host_ptr->h_length);
      host = host_ptr->h_name;
      type = host_ptr->h_addrtype;
   } else {
      return TInetAddress(hostname, 0, -1);
   }

   return TInetAddress(host, ntohl(addr), type);
}

//______________________________________________________________________________
TInetAddress TUnixSystem::GetSockName(int sock)
{
   // Get Internet Protocol (IP) address of host and port #.

   struct sockaddr_in addr;
#if defined(R__AIX) || defined(R__FBSD)
   size_t len = sizeof(addr);
#elif defined(R__GLIBC)
   socklen_t len = sizeof(addr);
#else
   int len = sizeof(addr);
#endif

   if (getsockname(sock, (struct sockaddr *)&addr, &len) == -1) {
      cerr << "TUnixSystem::GetSockName(): error" << endl;
      return TInetAddress();
   }

   struct hostent *host_ptr;
   const char *hostname;
   int         family;
   UInt_t      iaddr;

   if ((host_ptr = gethostbyaddr((const char *)&addr.sin_addr,
                                 sizeof(addr.sin_addr), AF_INET))) {
      memcpy(&iaddr, host_ptr->h_addr, host_ptr->h_length);
      hostname = host_ptr->h_name;
      family   = host_ptr->h_addrtype;
   } else {
      memcpy(&iaddr, &addr.sin_addr, sizeof(addr.sin_addr));
      hostname = "????";
      family   = AF_INET;
   }

   return TInetAddress(hostname, ntohl(iaddr), family, ntohs(addr.sin_port));
}

//______________________________________________________________________________
TInetAddress TUnixSystem::GetPeerName(int sock)
{
   // Get Internet Protocol (IP) address of remote host and port #.

   struct sockaddr_in addr;
#if defined(R__AIX) || defined(R__FBSD)
   size_t len = sizeof(addr);
#elif defined(R__GLIBC)
   socklen_t len = sizeof(addr);
#else
   int len = sizeof(addr);
#endif

   if (getpeername(sock, (struct sockaddr *)&addr, &len) == -1) {
      cerr << "TUnixSystem::GetPeerName(): error" << endl;
      return TInetAddress();
   }

   struct hostent *host_ptr;
   const char *hostname;
   int         family;
   UInt_t      iaddr;

   if ((host_ptr = gethostbyaddr((const char *)&addr.sin_addr,
                                 sizeof(addr.sin_addr), AF_INET))) {
      memcpy(&iaddr, host_ptr->h_addr, host_ptr->h_length);
      hostname = host_ptr->h_name;
      family   = host_ptr->h_addrtype;
   } else {
      memcpy(&iaddr, &addr.sin_addr, sizeof(addr.sin_addr));
      hostname = "????";
      family   = AF_INET;
   }

   return TInetAddress(hostname, ntohl(iaddr), family, ntohs(addr.sin_port));
}

//______________________________________________________________________________
int TUnixSystem::GetServiceByName(const char *servicename)
{
   // Get port # of internet service.

   struct servent *sp;

   if ((sp = getservbyname(servicename, kProtocolName)) == 0) {
      cerr << "TUnixSystem::GetServiceByName(): no service "
				<< servicename << " with protocol " << kProtocolName << endl;
      return -1;
   }
   return ntohs(sp->s_port);
}

//______________________________________________________________________________
char *TUnixSystem::GetServiceByPort(int port)
{
   // Get name of internet service.

   struct servent *sp;

   if ((sp = getservbyport(port, kProtocolName)) == 0) {
       return Form("%d", port);
   }
   return sp->s_name;
}

//______________________________________________________________________________
int TUnixSystem::ConnectService(const char *servername, int port)
{
   // Connect to service servicename on server servername.

   if (!strcmp(servername, "unix"))
      return UnixUnixConnect(port);
   return UnixTcpConnect(servername, port);
}

//______________________________________________________________________________
int TUnixSystem::OpenConnection(const char *server, int port)
{
   // Open a connection to a service on a server. Try 3 times with an
   // interval of 1 second.

   for (int i = 0; i < 3; i++) {
      int fd = ConnectService(server, port);
      if (fd >= 0)
         return fd;
      sleep(1);
   }
   return -1;
}

//______________________________________________________________________________
int TUnixSystem::AnnounceTcpService(int port, Bool_t reuse, int backlog)
{
   // Announce TCP/IP service.

   return UnixTcpService(port, reuse, backlog);
}

//______________________________________________________________________________
int TUnixSystem::AnnounceUnixService(int port, int backlog)
{
   // Announce unix domain service.

   return UnixUnixService(port, backlog);
}

//______________________________________________________________________________
int TUnixSystem::AcceptConnection(int sock)
{
   // Accept a connection. In case of an error return -1. In case
   // non-blocking I/O is enabled and no connections are available
   // return -2.

   int soc = -1;

   while ((soc = ::accept(sock, 0, 0)) == -1 && GetErrno() == EINTR)
      ResetErrno();

   if (soc == -1) {
      if (GetErrno() == EWOULDBLOCK)
         return -2;
      else {
         cerr << "TUnixSystem::AcceptConnection(): error" << endl;
         return -1;
      }
   }

   return soc;
}

//______________________________________________________________________________
void TUnixSystem::CloseConnection(int sock)
{
   // Close socket.

   if (sock < 0) return;

#if !defined(R__AIX) || defined(_AIX41) || defined(_AIX43)
   //::shutdown(sock, 2);   // will also close connection of parent
#endif

   while (::close(sock) == -1 && GetErrno() == EINTR)
      ResetErrno();
}

//______________________________________________________________________________
int TUnixSystem::RecvBuf(int sock, void *buf, int length)
{
   // Receive a buffer headed by a length indicator. Lenght is the size of
   // the buffer. Returns the number of bytes received in buf or -1 in
   // case of error.

   Int_t header;

   if (UnixRecv(sock, &header, sizeof(header), 0) > 0) {
      int count = ntohl(header);

      if (count > length) {
         cerr << "TUnixSystem::RecvBuf(): record header exceeds buffer size" << endl;
         return -1;
      } else if (count > 0) {
         if (UnixRecv(sock, buf, count, 0) < 0) {
            cerr << "TUnixSystem::RecvBuf(): cannot receive buffer" << endl;
            return -1;
         }
      }
      return count;
   }
   return -1;
}

//______________________________________________________________________________
int TUnixSystem::SendBuf(int sock, const void *buf, int length)
{
   // Send a buffer headed by a length indicator. Returns length of sent buffer
   // or -1 in case of error.

   Int_t header = htonl(length);

   if (UnixSend(sock, &header, sizeof(header), 0) < 0) {
      cerr << "TUnixSystem::SendBuf(): cannot send header" << endl;
      return -1;
   }
   if (length > 0) {
      if (UnixSend(sock, buf, length, 0) < 0) {
         cerr << "TUnixSystem::SendBuf(): cannot send buffer" << endl;
         return -1;
      }
   }
   return length;
}

//______________________________________________________________________________
int TUnixSystem::RecvRaw(int sock, void *buf, int length, int opt)
{
   // Receive exactly length bytes into buffer. Use opt to receive out-of-band
   // data or to have a peek at what is in the buffer (see TSocket). Buffer
   // must be able to store at least length bytes. Returns the number of
   // bytes received (can be 0 if other side of connection was closed) or -1
   // in case of error, -2 in case of MSG_OOB and errno == EWOULDBLOCK, -3
   // in case of MSG_OOB and errno == EINVAL and -4 in case of kNoBlock and
   // errno == EWOULDBLOCK.

   int flag;

   switch (opt) {
   case kDefault:
      flag = 0;
      break;
   case kOob:
      flag = MSG_OOB;
      break;
   case kPeek:
      flag = MSG_PEEK;
      break;
   default:
      flag = 0;
      break;
   }

   int n;
   if ((n = UnixRecv(sock, buf, length, flag)) <= 0) {
      if (n == -1 && GetErrno() != EINTR)
         cerr << "TUnixSystem::RecvRaw(): cannot receive buffer" << endl;
      return n;
   }
   return length;
}

//______________________________________________________________________________
int TUnixSystem::SendRaw(int sock, const void *buf, int length, int opt)
{
   // Send exactly length bytes from buffer. Use opt to send out-of-band
   // data (see TSocket). Returns the number of bytes sent or -1 in case of
   // error.

   int flag;

   switch (opt) {
   case kDefault:
      flag = 0;
      break;
   case kOob:
      flag = MSG_OOB;
      break;
   case kPeek:            // receive only option (see RecvRaw)
   default:
      flag = 0;
      break;
   }

   if (this->UnixSend(sock, buf, length, flag) < 0) {
      cerr << "TUnixSystem::SendRaw(): cannot send buffer" << endl;
      return -1;
   }
   return length;
}

//______________________________________________________________________________
int TUnixSystem::SetSockOpt(int sock, int opt, int val)
{
   // Set socket option.

   if (sock < 0) return -1;

   switch (opt) {
   case kSendBuffer:
      if (setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&val, sizeof(val)) == -1) {
         cerr << "TUnixSystem::SetSockOpt(): setsockopt(SO_SNDBUF)" << endl;
         return -1;
      }
      break;
   case kRecvBuffer:
      if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&val, sizeof(val)) == -1) {
         cerr << "TUnixSystem::SetSockOpt(): setsockopt(SO_RCVBUF)" << endl;
         return -1;
      }
      break;
   case kOobInline:
      if (setsockopt(sock, SOL_SOCKET, SO_OOBINLINE, (char*)&val, sizeof(val)) == -1) {
         cerr << "TUnixSystem::SetSockOpt(): setsockopt(SO_OOBINLINE)" << endl;
         return -1;
      }
      break;
   case kKeepAlive:
      if (setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (char*)&val, sizeof(val)) == -1) {
         cerr << "TUnixSystem::SetSockOpt(): setsockopt(SO_KEEPALIVE)" << endl;
         return -1;
      }
      break;
   case kReuseAddr:
      if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&val, sizeof(val)) == -1) {
         cerr << "TUnixSystem::SetSockOpt(): setsockopt(SO_REUSEADDR)" << endl;
         return -1;
      }
      break;
   case kNoDelay:
      if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&val, sizeof(val)) == -1) {
         cerr << "TUnixSystem::SetSockOpt(): setsockopt(TCP_NODELAY)" << endl;
         return -1;
      }
      break;
   case kNoBlock:
      if (ioctl(sock, FIONBIO, (char*)&val) == -1) {
         cerr << "TUnixSystem::SetSockOpt(): ioctl(FIONBIO)" << endl;
         return -1;
      }
      break;
   case kProcessGroup:
      if (ioctl(sock, SIOCSPGRP, (char*)&val) == -1) {
         cerr << "TUnixSystem::SetSockOpt(): ioctl(SIOCSPGRP)" << endl;
         return -1;
      }
      break;
   case kAtMark:       // read-only option (see GetSockOpt)
   case kBytesToRead:  // read-only option
   default:
      cerr << "TUnixSystem::SetSockOpt(): illegal option - " << opt << endl;
      return -1;
   }
   return 0;
}

//______________________________________________________________________________
int TUnixSystem::GetSockOpt(int sock, int opt, int *val)
{
   // Get socket option.

   if (sock < 0) return -1;

#if defined(R__GLIBC) || defined(_AIX43)
   socklen_t optlen = sizeof(*val);
#elif defined(R__FBSD)
   size_t optlen = sizeof(*val);
#else
   int optlen = sizeof(*val);
#endif

   switch (opt) {
   case kSendBuffer:
      if (getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)val, &optlen) == -1) {
         cerr << "TUnixSystem::GetSockOpt(): getsockopt(SO_SNDBUF)" << endl;
         return -1;
      }
      break;
   case kRecvBuffer:
      if (getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)val, &optlen) == -1) {
         cerr << "TUnixSystem::GetSockOpt(): getsockopt(SO_RCVBUF)" << endl;
         return -1;
      }
      break;
   case kOobInline:
      if (getsockopt(sock, SOL_SOCKET, SO_OOBINLINE, (char*)val, &optlen) == -1) {
         cerr << "TUnixSystem::GetSockOpt(): getsockopt(SO_OOBINLINE)" << endl;
         return -1;
      }
      break;
   case kKeepAlive:
      if (getsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (char*)val, &optlen) == -1) {
         cerr << "TUnixSystem::GetSockOpt(): getsockopt(SO_KEEPALIVE)" << endl;
         return -1;
      }
      break;
   case kReuseAddr:
      if (getsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)val, &optlen) == -1) {
         cerr << "TUnixSystem::GetSockOpt(): getsockopt(SO_REUSEADDR)" << endl;
         return -1;
      }
      break;
   case kNoDelay:
      if (getsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)val, &optlen) == -1) {
         cerr << "TUnixSystem::GetSockOpt(): getsockopt(TCP_NODELAY)" << endl;
         return -1;
      }
      break;
   case kNoBlock:
      int flg;
      if ((flg = fcntl(sock, F_GETFL, 0)) == -1) {
         cerr << "TUnixSystem::GetSockOpt(): fcntl(F_GETFL)" << endl;
         return -1;
      }
      *val = flg & O_NDELAY;
      break;
   case kProcessGroup:
#if !defined(R__LYNXOS)
      if (ioctl(sock, SIOCGPGRP, (char*)val) == -1) {
         cerr << "TUnixSystem::GetSockOpt(): ioctl(SIOCGPGRP)" << endl;
         return -1;
      }
#else
      cerr << "TUnixSystem::GetSockOpt(): ioctl(SIOCGPGRP) not supported on LynxOS" << endl;
      return -1;
#endif
      break;
   case kAtMark:
#if !defined(R__LYNXOS)
      if (ioctl(sock, SIOCATMARK, (char*)val) == -1) {
         cerr << "TUnixSystem::GetSockOpt(): ioctl(SIOCATMARK)" << endl;
         return -1;
      }
#else
      cerr << "TUnixSystem::GetSockOpt(): ioctl(SIOCATMARK) not supported on LynxOS" << endl;
      return -1;
#endif
      break;
   case kBytesToRead:
#if !defined(R__LYNXOS)
      if (ioctl(sock, FIONREAD, (char*)val) == -1) {
         cerr << "TUnixSystem::GetSockOpt(): ioctl(FIONREAD)" << endl;
         return -1;
      }
#else
      cerr << "TUnixSystem::GetSockOpt(): ioctl(FIONREAD) not supported on LynxOS" << endl;
      return -1;
#endif
      break;
   default:
      cerr << "TUnixSystem::GetSockOpt(): illegal option - " << opt << endl;
      *val = 0;
      return -1;
   }
   return 0;
}

//______________________________________________________________________________
int TUnixSystem::UnixTcpConnect(const char *hostname, int port)
{
   // Open a TCP/IP connection to server and connect to a service (i.e. port).
   // Is called via the TSocket constructor.

   short  sport;
   struct servent *sp;

   if ((sp = getservbyport(port, kProtocolName)))
      sport = sp->s_port;
   else
      sport = htons(port);

   TInetAddress addr = GetHostByName(hostname);
   if (!addr.IsValid()) return -1;
   UInt_t adr = htonl(addr.GetAddress());

   struct sockaddr_in server;
   memset(&server, 0, sizeof(server));
   memcpy(&server.sin_addr, &adr, sizeof(adr));
   server.sin_family = addr.GetFamily();
   server.sin_port   = sport;

   // Create socket
   int sock;
   if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      cerr << "TUnixSystem::UnixConnectTcp(): socket error" << endl;
      return -1;
   }

   if (connect(sock, (struct sockaddr*) &server, sizeof(server)) < 0) {
      close(sock);
      return -1;
   }
   return sock;
}

//______________________________________________________________________________
int TUnixSystem::UnixUnixConnect(int port)
{
   // Connect to a Unix domain socket.

   int sock;
   char buf[100];
   struct sockaddr_un unserver;

   sprintf(buf, "%s/%d", kServerPath, port);

   unserver.sun_family = AF_UNIX;
   strcpy(unserver.sun_path, buf);

   // Open socket
   if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
      cerr << "TUnixSystem::UnixUnixConnect(): socket error" << endl;
      return -1;
   }

   if (connect(sock, (struct sockaddr*) &unserver, strlen(unserver.sun_path)+2) < 0) {
      close(sock);
      return -1;
   }
   return sock;
}

//______________________________________________________________________________
int TUnixSystem::UnixTcpService(int port, Bool_t reuse, int backlog)
{
   // Open a socket, bind to it and start listening for TCP/IP connections
   // on the port. Returns socket fd or -1 if socket() failed, -2 if bind() failed
   // or -3 if listen() failed.

   short  sport;
   struct servent *sp;

   if ((sp = getservbyport(port, kProtocolName)))
      sport = sp->s_port;
   else
      sport = htons(port);

   // Create tcp socket
   int sock;
   if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      cerr << "TUnixSystem::UnixTcpService(): socket error" << endl;
      return -1;
   }

   if (reuse)
      SetSockOpt(sock, kReuseAddr, 1);

   struct sockaddr_in inserver;
   memset(&inserver, 0, sizeof(inserver));
   inserver.sin_family = AF_INET;
   inserver.sin_addr.s_addr = htonl(INADDR_ANY);
   inserver.sin_port = sport;

   // Bind socket
   if (bind(sock, (struct sockaddr*) &inserver, sizeof(inserver))) {
      cerr << "TUnixSystem::UnixTcpService(): bind error" << endl;
      return -2;
   }

   // Start accepting connections
   if (listen(sock, backlog)) {
      cerr << "TUnixSystem::UnixTcpService(): listen error" << endl;
      return -3;
   }

   return sock;
}

//______________________________________________________________________________
int TUnixSystem::UnixUnixService(int port, int backlog)
{
   // Open a socket, bind to it and start listening for Unix domain connections
   // to it. Returns socket fd or -1.

   struct sockaddr_un unserver;
   int sock, oldumask;

   memset(&unserver, 0, sizeof(unserver));
   unserver.sun_family = AF_UNIX;

   // Assure that socket directory exists
   oldumask = umask(0);
   ::mkdir(kServerPath, 0777);
   umask(oldumask);
   sprintf(unserver.sun_path, "%s/%d", kServerPath, port);

   // Remove old socket
   unlink(unserver.sun_path);

   // Create socket
   if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
      cerr << "TUnixSystem::UnixUnixService(): socket error" << endl;
      return -1;
   }

   if (bind(sock, (struct sockaddr*) &unserver, strlen(unserver.sun_path)+2)) {
      cerr << "TUnixSystem::UnixUnixService(): bind error" << endl;
      return -1;
   }

   // Start accepting connections
   if (listen(sock, backlog)) {
      cerr << "TUnixSystem::UnixUnixService(): listen error" << endl;
      return -1;
   }

   return sock;
}

//______________________________________________________________________________
int TUnixSystem::UnixRecv(int sock, void *buffer, int length, int flag)
{
   // Receive exactly length bytes into buffer. Returns number of bytes
   // received. Returns -1 in case of error, -2 in case of MSG_OOB
   // and errno == EWOULDBLOCK, -3 in case of MSG_OOB and errno == EINVAL
   // and -4 in case of kNoBlock and errno == EWOULDBLOCK.

   ResetErrno();

   if (sock < 0) return -1;

   int n, nrecv = 0;
   char *buf = (char *)buffer;

   for (n = 0; n < length; n += nrecv) {
      if ((nrecv = recv(sock, buf+n, length-n, flag)) <= 0) {
         if (nrecv == 0)
            break;        // EOF
         if (flag == MSG_OOB) {
            if (GetErrno() == EWOULDBLOCK)
               return -2;
            else if (GetErrno() == EINVAL)
               return -3;
         }
         if (this->GetErrno() == EWOULDBLOCK)
            return -4;
         else {
            if (GetErrno() != EINTR)
               cerr << "TUnixSystem::UnixRecv(): recv error" << endl;
            return -1;
         }
      }
   }
   return n;
}

//______________________________________________________________________________
int TUnixSystem::UnixSend(int sock, const void *buffer, int length, int flag)
{
   // Send exactly length bytes from buffer. Returns -1 in case of error,
   // otherwise number of sent bytes.

   if (sock < 0) return -1;

   int n, nsent = 0;
   const char *buf = (const char *)buffer;

   for (n = 0; n < length; n += nsent) {
      if ((nsent = send(sock, buf+n, length-n, flag)) <= 0) {
         cerr << "TUnixSystem::UnixSend(): send error" << endl;
         return nsent;
      }
   }
   return n;
}


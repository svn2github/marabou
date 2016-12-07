//________________________________________________________[C++ IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrUnixSystem.cxx
//! \brief			Light Weight ROOT: TUnixSystem
//! \details		Class definitions for ROOT under LynxOs: TUnixSystem
//! 				Class providing an interface to the UNIX Operating System
//!
//! \author Otto.Schaile
//!
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.5 $
//! $Date: 2009-10-27 13:30:44 $
//////////////////////////////////////////////////////////////////////////////


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
          extern shl_t cxxshl_load(const Char_t * path, Int_t flags, long address);
          extern Int_t   cxxshl_unload(shl_t handle);
       }
#   elif !defined(__STDCPP__)
#      include <cxxdl.h>
#   endif
    // print stack trace (HP undocumented internal call)
    extern "C" void U_STACK_TRACE();
#   if defined(hpux9)
extern "C" {
   extern void openlog(const Char_t *, int, int);
   extern void syslog(int, const Char_t *, ...);
   extern void closelog(void);
   extern Int_t setlogmask(int);
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
   extern Int_t putenv(const Char_t *);
   extern Int_t inet_aton(const Char_t *, struct in_addr *);
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


const Char_t *kServerPath     = "/tmp";
const Char_t *kProtocolName   = "tcp";


//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
//! \details		Provides an interface to the UNIX Operating System
//////////////////////////////////////////////////////////////////////////////

TUnixSystem::TUnixSystem()
{ }

TUnixSystem::~TUnixSystem()
{ }

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns the system's host name
//////////////////////////////////////////////////////////////////////////////

const Char_t *TUnixSystem::HostName()
{

      Char_t hn[64];
#if defined(R__SOLARIS) && !defined(R__KCC)
      sysinfo(SI_HOSTNAME, hn, sizeof(hn));
#else
      gethostname(hn, sizeof(hn));
#endif
      return hn;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Gets host's ip address.<br>
//! 				Returns a TInetAddress object.<br>
//! 				Use method TInetAddress::IsValid() to check if success.
//! \param[in]		HostName	-- host name
//! \retval 		InetAddr	-- ip addr
/////////////////////////////////////////////////////////////////////////////

TInetAddress TUnixSystem::GetHostByName(const Char_t *HostName)
{

   struct hostent *host_ptr;
   struct in_addr  ad;
   const Char_t     *host;
   Int_t             type;
   UInt_t          addr;    // good for 4 byte addresses

#ifdef HASNOT_INETATON
   if ((addr = (UInt_t)inet_addr(HostName)) != INADDR_NONE) {
#else
   if (inet_aton(HostName, &ad)) {
      memcpy(&addr, &ad.s_addr, sizeof(ad.s_addr));
#endif
      if ((host_ptr = gethostbyaddr((const Char_t *)&addr,
                                    sizeof(addr), AF_INET))) {
         host = host_ptr->h_name;
         type = AF_INET;
      } else {
         return TInetAddress("UnknownHost", ntohl(addr), -1);
      }
   } else if ((host_ptr = gethostbyname(HostName))) {
      // Check the address type for an internet host
      if (host_ptr->h_addrtype != AF_INET) {
         cerr << "TUnixSystem::GetHostByName(): " << HostName << " is not an internet host" << endl;
         return TInetAddress();
      }
      memcpy(&addr, host_ptr->h_addr, host_ptr->h_length);
      host = host_ptr->h_name;
      type = host_ptr->h_addrtype;
   } else {
      return TInetAddress(HostName, 0, -1);
   }

   return TInetAddress(host, ntohl(addr), type);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Gets local ip address of a socket.<br>
//! 				Returns a TInetAddress object.<br>
//! 				Use method TInetAddress::IsValid() to check if success.
//! \param[in]		Sock		-- socket id
//! \retval 		InetAddr	-- local ip addr
/////////////////////////////////////////////////////////////////////////////

TInetAddress TUnixSystem::GetSockName(Int_t Sock)
{
   struct sockaddr_in addr;
#if defined(R__AIX) || defined(R__FBSD)
   size_t len = sizeof(addr);
#elif defined(R__GLIBC)
   socklen_t len = sizeof(addr);
#else
   Int_t len = sizeof(addr);
#endif

   if (getsockname(Sock, (struct sockaddr *)&addr, &len) == -1) {
      cerr << "TUnixSystem::GetSockName(): error" << endl;
      return TInetAddress();
   }

   struct hostent *host_ptr;
   const Char_t *hostname;
   Int_t         family;
   UInt_t      iaddr;

   if ((host_ptr = gethostbyaddr((const Char_t *)&addr.sin_addr,
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

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Gets remote ip address of a socket.<br>
//! 				Returns a TInetAddress object.<br>
//! 				Use method TInetAddress::IsValid() to check if success.
//! \param[in]		Sock		-- socket id
//! \retval 		InetAddr	-- remote ip addr
/////////////////////////////////////////////////////////////////////////////

TInetAddress TUnixSystem::GetPeerName(Int_t Sock)
{
   struct sockaddr_in addr;
#if defined(R__AIX) || defined(R__FBSD)
   size_t len = sizeof(addr);
#elif defined(R__GLIBC)
   socklen_t len = sizeof(addr);
#else
   Int_t len = sizeof(addr);
#endif

   if (getpeername(Sock, (struct sockaddr *)&addr, &len) == -1) {
      cerr << "TUnixSystem::GetPeerName(): error" << endl;
      return TInetAddress();
   }

   struct hostent *host_ptr;
   const Char_t *hostname;
   Int_t         family;
   UInt_t      iaddr;

   if ((host_ptr = gethostbyaddr((const Char_t *)&addr.sin_addr,
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

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns port number of internet service
//! \param[in]		Service		-- name of service
//! \retval 		Port		-- port number
/////////////////////////////////////////////////////////////////////////////

Int_t TUnixSystem::GetServiceByName(const Char_t *  Service)
{
   // Get port # of internet service.

   struct servent *sp;

   if ((sp = getservbyname(Service, kProtocolName)) == 0) {
      cerr << "TUnixSystem::GetServiceByName(): no service "
				<< Service << " with protocol " << kProtocolName << endl;
      return -1;
   }
   return ntohs(sp->s_port);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns service name for a given port
//! \param[in]		Port		-- port number
//! \retval 		Service		-- name of service
/////////////////////////////////////////////////////////////////////////////

Char_t * TUnixSystem::GetServiceByPort(Int_t Port)
{
   // Get name of internet service.

   struct servent *sp;

   if ((sp = getservbyport(Port, kProtocolName)) == 0) return Form("%d", Port);
   return sp->s_name;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Connects to server
//! \param[in]		Server		-- server name
//! \param[in]		Port		-- port number
//! \retval 		Socket		-- socket's file descr
/////////////////////////////////////////////////////////////////////////////

Int_t TUnixSystem::ConnectService(const Char_t * Server, Int_t Port)
{

   if (!strcmp(Host, "unix")) return this->UnixUnixConnect(Port);
   return this->UnixTcpConnect(Server, Port);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Tries to open connection to server.<br>
//! 				Returns
//! 					<ul>
//! 					<li> socket's file descriptor
//! 					<li> -1 after 3 unsuccessful trials
//! 					</ul>
//! \param[in]		Server		-- server name
//! \param[in]		Port		-- port number
//! \retval 		Socket		-- socket's file descr
/////////////////////////////////////////////////////////////////////////////

Int_t TUnixSystem::OpenConnection(const Char_t *  Server, Int_t Port)
{
   for (Int_t i = 0; i < 3; i++) {
      Int_t fd = ConnectService(Server, Port);
      if (fd >= 0) return fd;
      sleep(1);
   }
   return -1;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Opens a socket, binds to it and starts listening
//! 				for TCP/IP connections on given port.<br>
//! 				If reuse is true reuse the address, backlog specifies
//! 				how many sockets can be waiting to be accepted.<br>
//! 				Returns
//! 					<ul>
//! 					<li> socket fd </li>
//! 					<li> -1 if socket() failed
//! 					<li> -2 if bind() failed
//! 					<li> -3 if listen() failed
//! 					</ul>
//! \param[in]		Port		-- port number
//! \param[in]		Reuse		-- TRUE or FALSE
//! \param[in]		Backlog		-- number of sockets waiting
//! \retval 		Socket		-- socket's file descr
/////////////////////////////////////////////////////////////////////////////

Int_t TUnixSystem::AnnounceTcpService(Int_t Port, Bool_t Reuse, Int_t Backlog)
{
   return UnixTcpService(Port, Reuse, Backlog);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Announce unix domain service on path "kServerPath/<port>"
//! \param[in]		Port		-- port number
//! \param[in]		Backlog		-- number of sockets waiting
//! \retval 		Socket		-- socket's file descr
/////////////////////////////////////////////////////////////////////////////

Int_t TUnixSystem::AnnounceUnixService(Int_t Port, Int_t Backlog)
{
   return UnixUnixService(Port, Backlog);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Accepts a connection.<br>
//! 				In case of an error returns -1.<br>
//! 				In case non-blocking I/O is enabled
//! 				and no connections are available returns -2.
//! \param[in]		Sock		-- socket id
//! \retval 		Status		-- connection status
/////////////////////////////////////////////////////////////////////////////

Int_t TUnixSystem::AcceptConnection(Int_t Sock)
{
   Int_t sts = -1;

   while ((sts = ::accept(Sock, 0, 0)) == -1 && GetErrno() == EINTR) ResetErrno();

   if (sts == -1) {
      if (GetErrno() == EWOULDBLOCK)
         return -2;
      else {
         cerr << "TUnixSystem::AcceptConnection(): error" << endl;
         return -1;
      }
   }

   return sts;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Closes socket
//! \param[in]		Sock		-- socket id
/////////////////////////////////////////////////////////////////////////////

void TUnixSystem::CloseConnection(Int_t Sock)
{
   // Close socket.

   if (Sock < 0) return;

#if !defined(R__AIX) || defined(_AIX41) || defined(_AIX43)
   //::shutdown(sock, 2);   // will also close connection of parent
#endif

   while (::close(Sock) == -1 && GetErrno() == EINTR) ResetErrno();
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Receives a buffer headed by a length indicator.<br>
//! 				Length is the size of the buffer.<br>
//! 				Returns
//! 				<ul>
//! 				<li>	the number of bytes received in buf
//! 				<li>	or -1 in case of error
//! 				</ul>
//! \param[in]		Sock		-- socket id
//! \param[in]		Buf 		-- buffer
//! \param[in]		Length 		-- buffer length
//! \retval 		NofBytes	-- number of bytes received
/////////////////////////////////////////////////////////////////////////////

Int_t TUnixSystem::RecvBuf(Int_t Sock, void * Buf, Int_t Length)
{

   Int_t header;

   if (UnixRecv(Sock, &header, sizeof(header), 0) > 0) {
      Int_t count = ntohl(header);

      if (count > Length) {
         cerr << "TUnixSystem::RecvBuf(): record header exceeds buffer size" << endl;
         return -1;
      } else if (count > 0) {
         if (UnixRecv(Sock, Buf, count, 0) < 0) {
            cerr << "TUnixSystem::RecvBuf(): cannot receive buffer" << endl;
            return -1;
         }
      }
      return count;
   }
   return -1;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sends a buffer headed by a length indicator.<br>
//! 				Returns
//! 				<ul>
//! 				<li>	length of sent buffer
//! 				<li>	or -1 in case of error.
//! 				</ul>
//! \param[in]		Sock		-- socket id
//! \param[in]		Buf 		-- buffer
//! \param[in]		Length 		-- buffer length
//! \retval 		NofBytes	-- number of bytes sent
/////////////////////////////////////////////////////////////////////////////

Int_t TUnixSystem::SendBuf(Int_t Sock, const void * Buf, Int_t Length)
{

   Int_t header = htonl(Length);

   if (UnixSend(Sock, &header, sizeof(header), 0) < 0) {
      cerr << "TUnixSystem::SendBuf(): cannot send header" << endl;
      return -1;
   }
   if (Length > 0) {
      if (UnixSend(Sock, Buf, Length, 0) < 0) {
         cerr << "TUnixSystem::SendBuf(): cannot send buffer" << endl;
         return -1;
      }
   }
   return Length;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Receive exactly length bytes into buffer.<br>
//! 				Use Opt to receive out-of-band data or to have a peek
//! 				at what is in the buffer (see TSocket).<br>
//! 				Buffer must be able to store at least length bytes.<br>
//! 				Returns
//! 				<ul>
//! 				<li>	the number of bytes received
//! 						(can be 0 if other side of connection was closed)
//! 				<li>	-1 in case of error
//! 				<li>	-2 in case of MSG_OOB and errno == EWOULDBLOCK
//! 				<li>	-3 in case of MSG_OOB and errno == EINVAL
//! 				<li>	-4 in case of kNoBlock and errno == EWOULDBLOCK.
//! 				</ul>
//! \param[in]		Sock		-- socket id
//! \param[in]		Buf 		-- buffer
//! \param[in]		Length 		-- buffer length
//! \param[in]		Opt 		-- option
//! \retval 		NofBytes	-- number of bytes sent
/////////////////////////////////////////////////////////////////////////////

Int_t TUnixSystem::RecvRaw(Int_t Sock, void * Buf, Int_t Length, Int_t Opt)
{

   Int_t flag;

   switch (Opt) {
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

   Int_t n;
   if ((n = UnixRecv(Sock, Buf, Length, flag)) <= 0) {
      if (n == -1 && GetErrno() != EINTR)
         cerr << "TUnixSystem::RecvRaw(): cannot receive buffer" << endl;
      return n;
   }
   return Length;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sends exactly length bytes from buffer.<br>
//! 				Use opt to send out-of-band data (see TSocket).<BR>
//! 				Returns
//! 				<ul>
//! 				<li>	the number of bytes sent
//! 				<li>	or -1 in case of error.
///! 				</ul>
//! \param[in]		Sock		-- socket id
//! \param[in]		Buf 		-- buffer
//! \param[in]		Length 		-- buffer length
//! \param[in]		Opt 		-- option
//! \retval 		NofBytes	-- number of bytes sent
/////////////////////////////////////////////////////////////////////////////

Int_t TUnixSystem::SendRaw(Int_t Sock, const void * Buf, Int_t Length, Int_t Opt)
{
   Int_t flag;

   switch (Opt) {
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

   if (this->UnixSend(Sock, Buf, Length, flag) < 0) {
      cerr << "TUnixSystem::SendRaw(): cannot send buffer" << endl;
      return -1;
   }
   return Length;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets socket option
//! \param[in]		Sock		-- socket id
//! \param[in]		Opt 		-- option name
//! \param[in]		Val 		-- option value
//! \return 		0 or -1
/////////////////////////////////////////////////////////////////////////////

Int_t TUnixSystem::SetSockOpt(Int_t Sock, Int_t Opt, Int_t Val)
{
   if (Sock < 0) return -1;

   switch (Opt) {
   case kSendBuffer:
      if (setsockopt(Sock, SOL_SOCKET, SO_SNDBUF, (char*)&Val, sizeof(Val)) == -1) {
         cerr << "TUnixSystem::SetSockOpt(): setsockopt(SO_SNDBUF)" << endl;
         return -1;
      }
      break;
   case kRecvBuffer:
      if (setsockopt(Sock, SOL_SOCKET, SO_RCVBUF, (char*)&Val, sizeof(Val)) == -1) {
         cerr << "TUnixSystem::SetSockOpt(): setsockopt(SO_RCVBUF)" << endl;
         return -1;
      }
      break;
   case kOobInline:
      if (setsockopt(Sock, SOL_SOCKET, SO_OOBINLINE, (char*)&Val, sizeof(Val)) == -1) {
         cerr << "TUnixSystem::SetSockOpt(): setsockopt(SO_OOBINLINE)" << endl;
         return -1;
      }
      break;
   case kKeepAlive:
      if (setsockopt(Sock, SOL_SOCKET, SO_KEEPALIVE, (char*)&Val, sizeof(Val)) == -1) {
         cerr << "TUnixSystem::SetSockOpt(): setsockopt(SO_KEEPALIVE)" << endl;
         return -1;
      }
      break;
   case kReuseAddr:
      if (setsockopt(Sock, SOL_SOCKET, SO_REUSEADDR, (char*)&Val, sizeof(Val)) == -1) {
         cerr << "TUnixSystem::SetSockOpt(): setsockopt(SO_REUSEADDR)" << endl;
         return -1;
      }
      break;
   case kNoDelay:
      if (setsockopt(Sock, IPPROTO_TCP, TCP_NODELAY, (char*)&Val, sizeof(Val)) == -1) {
         cerr << "TUnixSystem::SetSockOpt(): setsockopt(TCP_NODELAY)" << endl;
         return -1;
      }
      break;
   case kNoBlock:
      if (ioctl(Sock, FIONBIO, (char*)&Val) == -1) {
         cerr << "TUnixSystem::SetSockOpt(): ioctl(FIONBIO)" << endl;
         return -1;
      }
      break;
   case kProcessGroup:
      if (ioctl(Sock, SIOCSPGRP, (char*)&Val) == -1) {
         cerr << "TUnixSystem::SetSockOpt(): ioctl(SIOCSPGRP)" << endl;
         return -1;
      }
      break;
   case kAtMark:       // read-only option (see GetSockOpt)
   case kBytesToRead:  // read-only option
   default:
      cerr << "TUnixSystem::SetSockOpt(): illegal option - " << Opt << endl;
      return -1;
   }
   return 0;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns socket option
//! \param[in]		Sock		-- socket id
//! \param[in]		Opt 		-- option name
//! \param[out]		Val 		-- option value
//! \return 		0 or -1
/////////////////////////////////////////////////////////////////////////////

Int_t TUnixSystem::GetSockOpt(Int_t Sock, Int_t Opt, Int_t * Val)
{
   if (Sock < 0) return -1;

#if defined(R__GLIBC) || defined(_AIX43)
   Socklen_t Optlen = sizeof(*Val);
#elif defined(R__FBSD)
   size_t Optlen = sizeof(*Val);
#else
   Int_t Optlen = sizeof(*Val);
#endif

   switch (Opt) {
   case kSendBuffer:
      if (getSockOpt(Sock, SOL_SockET, SO_SNDBUF, (char*)Val, &Optlen) == -1) {
         cerr << "TUnixSystem::GetSockOpt(): getSockOpt(SO_SNDBUF)" << endl;
         return -1;
      }
      break;
   case kRecvBuffer:
      if (getSockOpt(Sock, SOL_SockET, SO_RCVBUF, (char*)Val, &Optlen) == -1) {
         cerr << "TUnixSystem::GetSockOpt(): getSockOpt(SO_RCVBUF)" << endl;
         return -1;
      }
      break;
   case kOobInline:
      if (getSockOpt(Sock, SOL_SockET, SO_OOBINLINE, (char*)Val, &Optlen) == -1) {
         cerr << "TUnixSystem::GetSockOpt(): getSockOpt(SO_OOBINLINE)" << endl;
         return -1;
      }
      break;
   case kKeepAlive:
      if (getSockOpt(Sock, SOL_SockET, SO_KEEPALIVE, (char*)Val, &Optlen) == -1) {
         cerr << "TUnixSystem::GetSockOpt(): getSockOpt(SO_KEEPALIVE)" << endl;
         return -1;
      }
      break;
   case kReuseAddr:
      if (getSockOpt(Sock, SOL_SockET, SO_REUSEADDR, (char*)Val, &Optlen) == -1) {
         cerr << "TUnixSystem::GetSockOpt(): getSockOpt(SO_REUSEADDR)" << endl;
         return -1;
      }
      break;
   case kNoDelay:
      if (getSockOpt(Sock, IPPROTO_TCP, TCP_NODELAY, (char*)Val, &Optlen) == -1) {
         cerr << "TUnixSystem::GetSockOpt(): getSockOpt(TCP_NODELAY)" << endl;
         return -1;
      }
      break;
   case kNoBlock:
      Int_t flg;
      if ((flg = fcntl(Sock, F_GETFL, 0)) == -1) {
         cerr << "TUnixSystem::GetSockOpt(): fcntl(F_GETFL)" << endl;
         return -1;
      }
      *Val = flg & O_NDELAY;
      break;
   case kProcessGroup:
#if !defined(R__LYNXOS)
      if (ioctl(Sock, SIOCGPGRP, (char*)Val) == -1) {
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
      if (ioctl(Sock, SIOCATMARK, (char*)Val) == -1) {
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
      if (ioctl(Sock, FIONREAD, (char*)Val) == -1) {
         cerr << "TUnixSystem::GetSockOpt(): ioctl(FIONREAD)" << endl;
         return -1;
      }
#else
      cerr << "TUnixSystem::GetSockOpt(): ioctl(FIONREAD) not supported on LynxOS" << endl;
      return -1;
#endif
      break;
   default:
      cerr << "TUnixSystem::GetSockOpt(): illegal Option - " << Opt << endl;
      *Val = 0;
      return -1;
   }
   return 0;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Opens a TCP/IP connection to server and connects to a service/port.<br>
//! 				Called via the TSocket constructor.
//! \param[in]		HostName	-- host
//! \param[in]		Port 		-- port number
//! \retval 		Sock		-- socket id
/////////////////////////////////////////////////////////////////////////////

Int_t TUnixSystem::UnixTcpConnect(const Char_t *  HostName, Int_t Port)
{

   Short_t sport;
   struct servent *sp;

   if ((sp = getservbyport(Port, kProtocolName)))
      sport = sp->s_port;
   else
      sport = htons(Port);

   TInetAddress addr = GetHostByName(HostName);
   if (!addr.IsValid()) return -1;
   UInt_t adr = htonl(addr.GetAddress());

   struct sockaddr_in server;
   memset(&server, 0, sizeof(server));
   memcpy(&server.sin_addr, &adr, sizeof(adr));
   server.sin_family = addr.GetFamily();
   server.sin_port   = sport;

   // Create socket
   Int_t sock;
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

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Connects to a Unix domain socket
//! \param[in]		Port 		-- port number
//! \retval 		Sock		-- socket id
/////////////////////////////////////////////////////////////////////////////

Int_t TUnixSystem::UnixUnixConnect(Int_t Port)
{
   // Connect to a Unix domain socket.

   Int_t sock;
   Char_t buf[100];
   struct sockaddr_un unserver;

   sprintf(buf, "%s/%d", kServerPath, Port);

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

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Opens a socket, binds to it
//! 				and starts listening for TCP/IP connections on the port.<br>
//! 				Returns
//! 				<ul>
//! 				<li>	socket fd
//! 				<li>	-1 if socket() failed
//! 				<li>	-2 if bind() failed
//! 				<li>	-3 if listen() failed
//! 				</ul>
//! \param[in]		Port 		-- port number
//! \param[in]		Reuse 		-- TRUE if socket is to be re-used
//! \param[in]		Backlog
//! \retval 		Sock		-- socket id
/////////////////////////////////////////////////////////////////////////////

Int_t TUnixSystem::UnixTcpService(Int_t Port, Bool_t Reuse, Int_t Backlog)
{

   Short_t sPort;
   struct servent *sp;

   if ((sp = getservbyPort(Port, kProtocolName)))
      sPort = sp->s_Port;
   else
      sPort = htons(Port);

   // Create tcp socket
   Int_t sock;
   if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      cerr << "TUnixSystem::UnixTcpService(): socket error" << endl;
      return -1;
   }

   if (Reuse) SetSockOpt(sock, kReuseAddr, 1);

   struct sockaddr_in inserver;
   memset(&inserver, 0, sizeof(inserver));
   inserver.sin_family = AF_INET;
   inserver.sin_addr.s_addr = htonl(INADDR_ANY);
   inserver.sin_Port = sPort;

   // Bind socket
   if (bind(sock, (struct sockaddr*) &inserver, sizeof(inserver))) {
      cerr << "TUnixSystem::UnixTcpService(): bind error" << endl;
      return -2;
   }

   // Start accepting connections
   if (listen(sock, Backlog)) {
      cerr << "TUnixSystem::UnixTcpService(): listen error" << endl;
      return -3;
   }

   return sock;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Opens a socket, binds to it and starts listening
//! 				for Unix domain connections to it.<br>
//! 				Returns
//! 				<ul>
//! 				<li>	socket fd
//! 				<li>	-1 on error
//! 				</ul>
//! \param[in]		Port 		-- port number
//! \param[in]		Backlog
//! \retval 		Sock		-- socket id
/////////////////////////////////////////////////////////////////////////////

Int_t TUnixSystem::UnixUnixService(Int_t Port, Int_t Backlog)
{

   struct sockaddr_un unserver;
   Int_t sock, oldumask;

   memset(&unserver, 0, sizeof(unserver));
   unserver.sun_family = AF_UNIX;

   // Assure that socket directory exists
   oldumask = umask(0);
   ::mkdir(kServerPath, 0777);
   umask(oldumask);
   sprintf(unserver.sun_path, "%s/%d", kServerPath, Port);

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
   if (listen(sock, Backlog)) {
      cerr << "TUnixSystem::UnixUnixService(): listen error" << endl;
      return -1;
   }

   return sock;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Receives exactly length bytes into buffer.<br>
//! 				Returns
//! 				<ul>
//! 				<li>	number of bytes received
//! 				<li>	-1 in case of error
//! 				<li>	-2 in case of MSG_OOB and errno == EWOULDBLOCK
//! 				<li>	-3 in case of MSG_OOB and errno == EINVAL
//! 				<li>	-4 in case of kNoBlock and errno == EWOULDBLOCK.
//! 				</ul>
//! \param[in]		Sock 		-- socket id
//! \param[in]		Buffer 		-- buffer address
//! \param[in]		Length 		-- number of bytes to receive
//! \param[in]		Flag
//! \retval 		NofBytes	-- number of bytes received
/////////////////////////////////////////////////////////////////////////////

Int_t TUnixSystem::UnixRecv(Int_t Sock, void * Buffer, Int_t Length, Int_t Flag)
{

   ResetErrno();

   if (Sock < 0) return -1;

   Int_t n, nrecv = 0;
   Char_t * buf = (Char_t *) Buffer;

   for (n = 0; n < Length; n += nrecv) {
      if ((nrecv = recv(Sock, buf+n, Length-n, Flag)) <= 0) {
         if (nrecv == 0)
            break;        // EOF
         if (Flag == MSG_OOB) {
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

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sends exactly length bytes from buffer.<br>
//! 				Returns
//! 				<ul>
//! 				<li>	 number of bytes sent
//! 				<li>	-1 in case of error
//! 				</ul>
//! \param[in]		Sock 		-- socket id
//! \param[in]		Buffer 		-- buffer address
//! \param[in]		Length 		-- number of bytes to receive
//! \param[in]		Flag
//! \retval 		NofBytes	-- number of bytes received
/////////////////////////////////////////////////////////////////////////////

Int_t TUnixSystem::UnixSend(Int_t Sock, const void * Buffer, Int_t Length, Int_t Flag)
{
   if (Sock < 0) return -1;

   Int_t n, nsent = 0;
   const Char_t * buf = (const Char_t *) Buffer;

   for (n = 0; n < Length; n += nsent) {
      if ((nsent = send(Sock, buf+n, Length-n, Flag)) <= 0) {
         cerr << "TUnixSystem::UnixSend(): send error" << endl;
         return nsent;
      }
   }
   return n;
}


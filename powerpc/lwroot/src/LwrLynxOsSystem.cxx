//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TLynxOsSystem                                                        //
//                                                                      //
// Class providing an interface to the LynxOs Operating System.           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
// Special 'Light weight ROOT' edition                                  //
// O. Schaile                                                           //
//////////////////////////////////////////////////////////////////////////

#include <types.h>
#include <time.h>

#include "LwrTypes.h"
#include "LwrLynxOsSystem.h"
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

#define HOWMANY(x, y)   (((x)+((y)-1))/(y))

enum EAccessMode {
   kFileExists        = 0,
   kExecutePermission = 1,
   kWritePermission   = 2,
   kReadPermission    = 4
};

enum ESysConstants {
   kMAXSIGNALS       = 15,
   kMAXPATHLEN       = 8192,
   kBUFFERSIZE       = 8192,
   kItimerResolution = 10      // interval-timer resolution in ms
};

enum EFileModeMask {
   kS_IFMT   = 0170000,   // bitmask for the file type bitfields
   kS_IFSOCK = 0140000,   // socket
   kS_IFLNK  = 0120000,   // symbolic link
   kS_IFOFF  = 0110000,   // offline file
   kS_IFREG  = 0100000,   // regular file
   kS_IFBLK  = 0060000,   // block device
   kS_IFDIR  = 0040000,   // directory
   kS_IFCHR  = 0020000,   // character device
   kS_IFIFO  = 0010000,   // fifo
   kS_ISUID  = 0004000,   // set UID bit
   kS_ISGID  = 0002000,   // set GID bit
   kS_ISVTX  = 0001000,   // sticky bit
   kS_IRWXU  = 00700,     // mask for file owner permissions
   kS_IRUSR  = 00400,     // owner has read permission
   kS_IWUSR  = 00200,     // owner has write permission
   kS_IXUSR  = 00100,     // owner has execute permission
   kS_IRWXG  = 00070,     // mask for group permissions
   kS_IRGRP  = 00040,     // group has read permission
   kS_IWGRP  = 00020,     // group has write permission
   kS_IXGRP  = 00010,     // group has execute permission
   kS_IRWXO  = 00007,     // mask for permissions for others (not in group)
   kS_IROTH  = 00004,     // others have read permission
   kS_IWOTH  = 00002,     // others have write permisson
   kS_IXOTH  = 00001      // others have execute permission
};

inline Bool_t R_ISDIR(Int_t mode)  { return ((mode & kS_IFMT) == kS_IFDIR); }
inline Bool_t R_ISCHR(Int_t mode)  { return ((mode & kS_IFMT) == kS_IFCHR); }
inline Bool_t R_ISBLK(Int_t mode)  { return ((mode & kS_IFMT) == kS_IFBLK); }
inline Bool_t R_ISREG(Int_t mode)  { return ((mode & kS_IFMT) == kS_IFREG); }
inline Bool_t R_ISLNK(Int_t mode)  { return ((mode & kS_IFMT) == kS_IFLNK); }
inline Bool_t R_ISFIFO(Int_t mode) { return ((mode & kS_IFMT) == kS_IFIFO); }
inline Bool_t R_ISSOCK(Int_t mode) { return ((mode & kS_IFMT) == kS_IFSOCK); }
inline Bool_t R_ISOFF(Int_t mode)  { return ((mode & kS_IFMT) == kS_IFOFF); }

const char *kServerPath     = "/tmp";
const char *kProtocolName   = "tcp";

const Int_t kNFDBITS = (sizeof(Long_t) * 8);  // 8 bits per byte
const Int_t kFDSETSIZE = 256;                 // upto 256 file descriptors


class TFdSet {
private:
   ULong_t fds_bits[HOWMANY(kFDSETSIZE, kNFDBITS)];
public:
   TFdSet() { memset(fds_bits, 0, sizeof(fds_bits)); }
   TFdSet(const TFdSet &org) { memcpy(fds_bits, org.fds_bits, sizeof(org.fds_bits)); }
   TFdSet &operator=(const TFdSet &rhs) { if (this != &rhs) { memcpy(fds_bits, rhs.fds_bits, sizeof(rhs.fds_bits));} return *this; }
   void   Zero() { memset(fds_bits, 0, sizeof(fds_bits)); }
   void   Set(Int_t n)
   {
      if (n >= 0 && n < kFDSETSIZE) {
         fds_bits[n/kNFDBITS] |= (1UL << (n % kNFDBITS));
      } else {
         cerr << "TFdSet::Set(): fd (" << n << ") out of range [0.." << kFDSETSIZE-1 << "]" << endl;
      }
   }
   void   Clr(Int_t n)
   {
      if (n >= 0 && n < kFDSETSIZE) {
         fds_bits[n/kNFDBITS] &= ~(1UL << (n % kNFDBITS));
      } else {
         cerr << "TFdSet::Clr(): fd (" << n << ") out of range [0.." << kFDSETSIZE-1 << "]" << endl;
      }
   }
   Int_t  IsSet(Int_t n)
   {
      if (n >= 0 && n < kFDSETSIZE) {
         return (fds_bits[n/kNFDBITS] & (1UL << (n % kNFDBITS))) != 0;
      } else {
         cerr << "TFdSet::IsSet(): fd (" << n << ") out of range [0.." << kFDSETSIZE-1 << "]" << endl;
         return 0;
      }
   }
   ULong_t *GetBits() { return (ULong_t *)fds_bits; }
};

//______________________________________________________________________________
TLynxOsSystem::TLynxOsSystem()
{
	fFileHandler = new TList();
}

//______________________________________________________________________________
TLynxOsSystem::~TLynxOsSystem()
{
}

//______________________________________________________________________________
const char *TLynxOsSystem::HostName()
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
TInetAddress TLynxOsSystem::GetHostByName(const char *hostname)
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
         cerr << "TLynxOsSystem::GetHostByName(): " << hostname << " is not an internet host" << endl;
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
TInetAddress TLynxOsSystem::GetSockName(int sock)
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
      cerr << "TLynxOsSystem::GetSockName(): error" << endl;
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
TInetAddress TLynxOsSystem::GetPeerName(int sock)
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
      cerr << "TLynxOsSystem::GetPeerName(): error" << endl;
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
int TLynxOsSystem::GetServiceByName(const char *servicename)
{
   // Get port # of internet service.

   struct servent *sp;

   if ((sp = getservbyname(servicename, kProtocolName)) == 0) {
      cerr << "TLynxOsSystem::GetServiceByName(): no service "
				<< servicename << " with protocol " << kProtocolName << endl;
      return -1;
   }
   return ntohs(sp->s_port);
}

//______________________________________________________________________________
char *TLynxOsSystem::GetServiceByPort(int port)
{
   // Get name of internet service.

   struct servent *sp;

   if ((sp = getservbyport(port, kProtocolName)) == 0) {
       return Form("%d", port);
   }
   return sp->s_name;
}

//______________________________________________________________________________
int TLynxOsSystem::ConnectService(const char *servername, int port)
{
   // Connect to service servicename on server servername.

   if (!strcmp(servername, "unix"))
      return UnixUnixConnect(port);
   return UnixTcpConnect(servername, port);
}

//______________________________________________________________________________
int TLynxOsSystem::OpenConnection(const char *server, int port)
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
int TLynxOsSystem::AnnounceTcpService(int port, Bool_t reuse, int backlog)
{
   // Announce TCP/IP service.

   return UnixTcpService(port, reuse, backlog);
}

//______________________________________________________________________________
int TLynxOsSystem::AnnounceUnixService(int port, int backlog)
{
   // Announce unix domain service.

   return UnixUnixService(port, backlog);
}

//______________________________________________________________________________
int TLynxOsSystem::AcceptConnection(int sock)
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
         cerr << "TLynxOsSystem::AcceptConnection(): error" << endl;
         return -1;
      }
   }

   return soc;
}

//______________________________________________________________________________
void TLynxOsSystem::CloseConnection(int sock)
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
int TLynxOsSystem::RecvBuf(int sock, void *buf, int length)
{
   // Receive a buffer headed by a length indicator. Lenght is the size of
   // the buffer. Returns the number of bytes received in buf or -1 in
   // case of error.

   Int_t header;

   if (UnixRecv(sock, &header, sizeof(header), 0) > 0) {
      int count = ntohl(header);

      if (count > length) {
         cerr << "TLynxOsSystem::RecvBuf(): record header exceeds buffer size" << endl;
         return -1;
      } else if (count > 0) {
         if (UnixRecv(sock, buf, count, 0) < 0) {
            cerr << "TLynxOsSystem::RecvBuf(): cannot receive buffer" << endl;
            return -1;
         }
      }
      return count;
   }
   return -1;
}

//______________________________________________________________________________
int TLynxOsSystem::SendBuf(int sock, const void *buf, int length)
{
   // Send a buffer headed by a length indicator. Returns length of sent buffer
   // or -1 in case of error.

   Int_t header = htonl(length);

   if (UnixSend(sock, &header, sizeof(header), 0) < 0) {
      cerr << "TLynxOsSystem::SendBuf(): cannot send header" << endl;
      return -1;
   }
   if (length > 0) {
      if (UnixSend(sock, buf, length, 0) < 0) {
         cerr << "TLynxOsSystem::SendBuf(): cannot send buffer" << endl;
         return -1;
      }
   }
   return length;
}

//______________________________________________________________________________
int TLynxOsSystem::RecvRaw(int sock, void *buf, int length, int opt)
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
         cerr << "TLynxOsSystem::RecvRaw(): cannot receive buffer" << endl;
      return n;
   }
   return length;
}

//______________________________________________________________________________
int TLynxOsSystem::SendRaw(int sock, const void *buf, int length, int opt)
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
      cerr << "TLynxOsSystem::SendRaw(): cannot send buffer" << endl;
      return -1;
   }
   return length;
}

//______________________________________________________________________________
int TLynxOsSystem::SetSockOpt(int sock, int opt, int val)
{
   // Set socket option.

   if (sock < 0) return -1;

   switch (opt) {
   case kSendBuffer:
      if (setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&val, sizeof(val)) == -1) {
         cerr << "TLynxOsSystem::SetSockOpt(): setsockopt(SO_SNDBUF)" << endl;
         return -1;
      }
      break;
   case kRecvBuffer:
      if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&val, sizeof(val)) == -1) {
         cerr << "TLynxOsSystem::SetSockOpt(): setsockopt(SO_RCVBUF)" << endl;
         return -1;
      }
      break;
   case kOobInline:
      if (setsockopt(sock, SOL_SOCKET, SO_OOBINLINE, (char*)&val, sizeof(val)) == -1) {
         cerr << "TLynxOsSystem::SetSockOpt(): setsockopt(SO_OOBINLINE)" << endl;
         return -1;
      }
      break;
   case kKeepAlive:
      if (setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (char*)&val, sizeof(val)) == -1) {
         cerr << "TLynxOsSystem::SetSockOpt(): setsockopt(SO_KEEPALIVE)" << endl;
         return -1;
      }
      break;
   case kReuseAddr:
      if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&val, sizeof(val)) == -1) {
         cerr << "TLynxOsSystem::SetSockOpt(): setsockopt(SO_REUSEADDR)" << endl;
         return -1;
      }
      break;
   case kNoDelay:
      if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&val, sizeof(val)) == -1) {
         cerr << "TLynxOsSystem::SetSockOpt(): setsockopt(TCP_NODELAY)" << endl;
         return -1;
      }
      break;
   case kNoBlock:
      if (ioctl(sock, FIONBIO, (char*)&val) == -1) {
         cerr << "TLynxOsSystem::SetSockOpt(): ioctl(FIONBIO)" << endl;
         return -1;
      }
      break;
   case kProcessGroup:
      if (ioctl(sock, SIOCSPGRP, (char*)&val) == -1) {
         cerr << "TLynxOsSystem::SetSockOpt(): ioctl(SIOCSPGRP)" << endl;
         return -1;
      }
      break;
   case kAtMark:       // read-only option (see GetSockOpt)
   case kBytesToRead:  // read-only option
   default:
      cerr << "TLynxOsSystem::SetSockOpt(): illegal option - " << opt << endl;
      return -1;
   }
   return 0;
}

//______________________________________________________________________________
int TLynxOsSystem::GetSockOpt(int sock, int opt, int *val)
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
         cerr << "TLynxOsSystem::GetSockOpt(): getsockopt(SO_SNDBUF)" << endl;
         return -1;
      }
      break;
   case kRecvBuffer:
      if (getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)val, &optlen) == -1) {
         cerr << "TLynxOsSystem::GetSockOpt(): getsockopt(SO_RCVBUF)" << endl;
         return -1;
      }
      break;
   case kOobInline:
      if (getsockopt(sock, SOL_SOCKET, SO_OOBINLINE, (char*)val, &optlen) == -1) {
         cerr << "TLynxOsSystem::GetSockOpt(): getsockopt(SO_OOBINLINE)" << endl;
         return -1;
      }
      break;
   case kKeepAlive:
      if (getsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (char*)val, &optlen) == -1) {
         cerr << "TLynxOsSystem::GetSockOpt(): getsockopt(SO_KEEPALIVE)" << endl;
         return -1;
      }
      break;
   case kReuseAddr:
      if (getsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)val, &optlen) == -1) {
         cerr << "TLynxOsSystem::GetSockOpt(): getsockopt(SO_REUSEADDR)" << endl;
         return -1;
      }
      break;
   case kNoDelay:
      if (getsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)val, &optlen) == -1) {
         cerr << "TLynxOsSystem::GetSockOpt(): getsockopt(TCP_NODELAY)" << endl;
         return -1;
      }
      break;
   case kNoBlock:
      int flg;
      if ((flg = fcntl(sock, F_GETFL, 0)) == -1) {
         cerr << "TLynxOsSystem::GetSockOpt(): fcntl(F_GETFL)" << endl;
         return -1;
      }
      *val = flg & O_NDELAY;
      break;
   case kProcessGroup:
#if !defined(R__LYNXOS)
      if (ioctl(sock, SIOCGPGRP, (char*)val) == -1) {
         cerr << "TLynxOsSystem::GetSockOpt(): ioctl(SIOCGPGRP)" << endl;
         return -1;
      }
#else
      cerr << "TLynxOsSystem::GetSockOpt(): ioctl(SIOCGPGRP) not supported on LynxOS" << endl;
      return -1;
#endif
      break;
   case kAtMark:
#if !defined(R__LYNXOS)
      if (ioctl(sock, SIOCATMARK, (char*)val) == -1) {
         cerr << "TLynxOsSystem::GetSockOpt(): ioctl(SIOCATMARK)" << endl;
         return -1;
      }
#else
      cerr << "TLynxOsSystem::GetSockOpt(): ioctl(SIOCATMARK) not supported on LynxOS" << endl;
      return -1;
#endif
      break;
   case kBytesToRead:
#if !defined(R__LYNXOS)
      if (ioctl(sock, FIONREAD, (char*)val) == -1) {
         cerr << "TLynxOsSystem::GetSockOpt(): ioctl(FIONREAD)" << endl;
         return -1;
      }
#else
      cerr << "TLynxOsSystem::GetSockOpt(): ioctl(FIONREAD) not supported on LynxOS" << endl;
      return -1;
#endif
      break;
   default:
      cerr << "TLynxOsSystem::GetSockOpt(): illegal option - " << opt << endl;
      *val = 0;
      return -1;
   }
   return 0;
}

//______________________________________________________________________________
int TLynxOsSystem::UnixTcpConnect(const char *hostname, int port)
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
      cerr << "TLynxOsSystem::UnixConnectTcp(): socket error" << endl;
      return -1;
   }

   if (connect(sock, (struct sockaddr*) &server, sizeof(server)) < 0) {
      close(sock);
      return -1;
   }
   return sock;
}

//______________________________________________________________________________
int TLynxOsSystem::UnixUnixConnect(int port)
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
      cerr << "TLynxOsSystem::UnixUnixConnect(): socket error" << endl;
      return -1;
   }

   if (connect(sock, (struct sockaddr*) &unserver, strlen(unserver.sun_path)+2) < 0) {
      close(sock);
      return -1;
   }
   return sock;
}

//______________________________________________________________________________
int TLynxOsSystem::UnixTcpService(int port, Bool_t reuse, int backlog)
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
      cerr << "TLynxOsSystem::UnixTcpService(): socket error" << endl;
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
      cerr << "TLynxOsSystem::UnixTcpService(): bind error" << endl;
      return -2;
   }

   // Start accepting connections
   if (listen(sock, backlog)) {
      cerr << "TLynxOsSystem::UnixTcpService(): listen error" << endl;
      return -3;
   }

   return sock;
}

//______________________________________________________________________________
int TLynxOsSystem::UnixUnixService(int port, int backlog)
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
      cerr << "TLynxOsSystem::UnixUnixService(): socket error" << endl;
      return -1;
   }

   if (bind(sock, (struct sockaddr*) &unserver, strlen(unserver.sun_path)+2)) {
      cerr << "TLynxOsSystem::UnixUnixService(): bind error" << endl;
      return -1;
   }

   // Start accepting connections
   if (listen(sock, backlog)) {
      cerr << "TLynxOsSystem::UnixUnixService(): listen error" << endl;
      return -1;
   }

   return sock;
}

//______________________________________________________________________________
int TLynxOsSystem::UnixRecv(int sock, void *buffer, int length, int flag)
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
               cerr << "TLynxOsSystem::UnixRecv(): recv error" << endl;
            return -1;
         }
      }
   }
   return n;
}

//______________________________________________________________________________
int TLynxOsSystem::UnixSend(int sock, const void *buffer, int length, int flag)
{
   // Send exactly length bytes from buffer. Returns -1 in case of error,
   // otherwise number of sent bytes.

   if (sock < 0) return -1;

   int n, nsent = 0;
   const char *buf = (const char *)buffer;

   for (n = 0; n < length; n += nsent) {
      if ((nsent = send(sock, buf+n, length-n, flag)) <= 0) {
         cerr << "TLynxOsSystem::UnixSend(): send error" << endl;
         return nsent;
      }
   }
   return n;
}

//______________________________________________________________________________
void TLynxOsSystem::AddFileHandler(TFileHandler * h)
{
   // Add a file handler to the list of system file handlers. Only adds
   // the handler if it is not already in the list of file handlers.

	if (h && fFileHandler && (fFileHandler->FindObject(h) == 0)) fFileHandler->Add(h);
}

//______________________________________________________________________________
TFileHandler * TLynxOsSystem::RemoveFileHandler(TFileHandler *h)
{
   // Remove a file handler from the list of file handlers. Returns
   // the handler or 0 if the handler was not in the list of file handlers.

   if (fFileHandler) return (TFileHandler *) fFileHandler->Remove(h);
   return NULL;
}

//______________________________________________________________________________
Int_t TLynxOsSystem::Select(TList *act, Long_t to)
{
   // Select on file descriptors. The timeout to is in millisec. Returns
   // the number of ready descriptors, or 0 in case of timeout, or < 0 in
   // case of an error, with -2 being EINTR and -3 EBADF. In case of EINTR
   // the errno has been reset and the method can be called again. Returns
   // -4 in case the list did not contain any file handlers or file handlers
   // with file descriptor >= 0.

   Int_t rc = -4;

   TFdSet rd, wr;
   Int_t mxfd = -1;
   TIter next(act);
   TFileHandler *h = 0;
   while ((h = (TFileHandler *) next())) {
      Int_t fd = h->GetFd();
      if (fd > -1) {
         if (h->HasReadInterest()) {
            rd.Set(fd);
            mxfd = (mxfd > fd) ? mxfd : fd;
         }
         if (h->HasWriteInterest()) {
            wr.Set(fd);
            mxfd = (mxfd > fd) ? mxfd : fd;
         }
         h->ResetReadyMask();
      }
   }
   if (mxfd > -1)
      rc = UnixSelect(mxfd+1, &rd, &wr, to);

   // Set readiness bits
   if (rc > 0) {
      next.Reset();
      while ((h = (TFileHandler *) next())) {
         Int_t fd = h->GetFd();
         if (rd.IsSet(fd))
            h->SetReadReady();
         if (wr.IsSet(fd))
            h->SetWriteReady();
      }
   }

   return rc;
}

//______________________________________________________________________________
Int_t TLynxOsSystem::UnixSelect(Int_t nfds, TFdSet *readready, TFdSet *writeready, Long_t timeout)
{
   // Wait for events on the file descriptors specified in the readready and
   // writeready masks or for timeout (in milliseconds) to occur. Returns
   // the number of ready descriptors, or 0 in case of timeout, or < 0 in
   // case of an error, with -2 being EINTR and -3 EBADF. In case of EINTR
   // the errno has been reset and the method can be called again.

   int retcode;

   fd_set *rd = (readready)  ? (fd_set*)readready->GetBits()  : 0;
   fd_set *wr = (writeready) ? (fd_set*)writeready->GetBits() : 0;

   if (timeout >= 0) {
      struct timeval tv;
      tv.tv_sec  = Int_t(timeout / 1000);
      tv.tv_usec = (timeout % 1000) * 1000;
      retcode = select(nfds, rd, wr, 0, &tv);
   } else {
      retcode = select(nfds, rd, wr, 0, 0);
   }
   if (retcode == -1) {
      if (GetErrno() == EINTR) {
         ResetErrno();  // errno is not self reseting
         return -2;
      }
      if (GetErrno() == EBADF)
         return -3;
      return -1;
   }

   return retcode;
}

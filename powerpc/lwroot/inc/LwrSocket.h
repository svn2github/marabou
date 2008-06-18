#ifndef __LwrSocket_h__
#define __LwrSocket_h__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TSocket                                                              //
//                                                                      //
// This class implements client sockets. A socket is an endpoint for    //
// communication between two machines.                                  //
// The actual work is done via the TSystem class (either TUnixSystem,   //
// TWin32System or TMacSystem).                                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
// Special 'Light Weight ROOT' edition for LynxOs                       //
// O. Schaile                                                           //
//////////////////////////////////////////////////////////////////////////

#include "LwrNamed.h"
#include "LwrInetAddress.h"
#include "M2L_MessageTypes.h"

enum ESockOptions {
   kSendBuffer,        // size of send buffer
   kRecvBuffer,        // size of receive buffer
   kOobInline,         // OOB message inline
   kKeepAlive,         // keep socket alive
   kReuseAddr,         // allow reuse of local portion of address 5-tuple
   kNoDelay,           // send without delay
   kNoBlock,           // non-blocking I/O
   kProcessGroup,      // socket process group (used for SIGURG and SIGIO)
   kAtMark,            // are we at out-of-band mark (read only)
   kBytesToRead        // get number of bytes to read, FIONREAD (read only)
};

enum ESendRecvOptions {
   kDefault,           // default option (= 0)
   kOob,               // send or receive out-of-band data
   kPeek               // peek at incoming message (receive only)
};


class TSocket : public TNamed {

friend class TServerSocket;

protected:
   Int_t         fSocket;         // socket descriptor
   TString       fService;        // name of service (matches remote port #)
   TInetAddress  fAddress;        // remote internet address and port #

   TSocket() { fSocket = -1; fBytesSent = fBytesRecv = 0; }

private:
   TInetAddress  fLocalAddress;   // local internet address and port #
   UInt_t        fBytesSent;      // total bytes sent using this socket
   UInt_t        fBytesRecv;      // total bytes received over this socket

   static UInt_t fgBytesSent;     // total bytes sent by all socket objects
   static UInt_t fgBytesRecv;     // total bytes received by all socket objects

   void operator=(const TSocket &);  // not implemented
   void SetDescriptor(Int_t desc) { fSocket = desc; }

public:
   TSocket(TInetAddress address, const char *service);
   TSocket(TInetAddress address, Int_t port);
   TSocket(const char *host, const char *service);
   TSocket(const char *host, Int_t port);
   TSocket(Int_t descriptor);
   TSocket(const TSocket &s);
   virtual ~TSocket() { Close(); }

   virtual void          Close(Option_t *opt="");
   Int_t                 GetDescriptor() const { return fSocket; }
   TInetAddress          GetInetAddress() const { return fAddress; }
   virtual TInetAddress  GetLocalInetAddress();
   Int_t                 GetPort() const { return fAddress.GetPort(); }
   virtual Int_t         GetLocalPort();
   UInt_t                GetBytesSent() const { return fBytesSent; }
   UInt_t                GetBytesRecv() const { return fBytesRecv; }
   virtual Int_t         SendRaw(const void *buffer, Int_t length, ESendRecvOptions opt = kDefault);
   virtual Int_t         RecvRaw(void *buffer, Int_t length, ESendRecvOptions opt = kDefault);
   Bool_t                IsValid() const { return fSocket < 0 ? kFALSE : kTRUE; }
   Int_t                 GetErrorCode() const;
   virtual Int_t         SetOption(ESockOptions opt, Int_t val);
   virtual Int_t         GetOption(ESockOptions opt, Int_t &val);

   static  UInt_t        GetSocketBytesSent() { return fgBytesSent; }
   static  UInt_t        GetSocketBytesRecv() { return fgBytesRecv; }

   inline const Char_t * ClassName() const { return "TSocket"; };
};

#endif
#ifndef __LwrServerSocket_h__
#define __LwrServerSocket_h__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TServerSocket                                                        //
//                                                                      //
// This class implements server sockets. A server socket waits for      //
// requests to come in over the network. It performs some operation     //
// based on that request and then possibly returns a result to the      //
// requester. The actual work is done via the TSystem class (either     //
// TUnixSystem, TWin32System or TMacSystem).                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
// Special 'Light Weight ROOT' edition for LynxOs                       //
// O. Schaile                                                           //
//////////////////////////////////////////////////////////////////////////

#include "LwrNamed.h"
#include "LwrSocket.h"

class TServerSocket : public TSocket {

private:
   TServerSocket() { }
   TServerSocket(const TServerSocket &);
   void operator=(const TServerSocket &);

public:
   enum { kDefaultBacklog = 10 };

   TServerSocket(Int_t port, Bool_t reuse = kFALSE, Int_t backlog = kDefaultBacklog);
   TServerSocket(const char *service, Bool_t reuse = kFALSE, Int_t backlog = kDefaultBacklog);
   virtual ~TServerSocket() { Close(); }

   virtual TSocket      *Accept();
   virtual TInetAddress  GetLocalInetAddress();
   virtual Int_t         GetLocalPort();

   inline const Char_t * ClassName() const { return "TServerSocket"; };

#if 0
   Int_t         Send(const TMessage &);
   Int_t         Send(Int_t);
   Int_t         Send(const char *, Int_t = kMESS_STRING);
   Int_t         SendRaw(const void *, Int_t, ESendRecvOptions = kDefault);
   Int_t         Recv(TMessage &);
   Int_t         Recv(char *, Int_t);
   Int_t         Recv(char *, Int_t, Int_t &);
   Int_t         RecvRaw(void *, Int_t, ESendRecvOptions = kDefault);
#endif
};

#endif

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TServerSocket                                                        //
//                                                                      //
// This class implements server sockets. A server socket waits for      //
// requests to come in over the network. It performs some operation     //
// based on that request and then possibly returns a full duplex socket //
// to the requester.                                                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
// Special 'Light Weight ROOT' edition                                  //
// O. Schaile                                                           //
//////////////////////////////////////////////////////////////////////////

#include "iostream.h"
#include "iomanip.h"

#include "LwrTypes.h"
#include "LwrServerSocket.h"
#include "LwrSocket.h"
#include "LwrLynxOsSystem.h"

extern TLynxOsSystem * gSystem;

//______________________________________________________________________________
TServerSocket::TServerSocket(const char *service, Bool_t reuse, Int_t backlog)
{
   // Create a server socket object for a named service. Set reuse to true
   // to force reuse of the server socket (i.e. do not wait for the time
   // out to pass). Using backlog one can set the desirable queue length
   // for pending connections. Use IsValid() to check the validity of the
   // server socket. In case server socket is not valid use GetErrorCode()
   // to obtain the specific error value. These values are:
   //  0 = no error (socket is valid)
   // -1 = low level socket() call failed
   // -2 = low level bind() call failed
   // -3 = low level listen() call failed
   // Every valid server socket is added to the TROOT sockets list which
   // will make sure that any open sockets are properly closed on
   // program termination.

	if(!gSystem) printf("TLynxOsSystem not initialized\n");
	else {
		int port = gSystem->GetServiceByName(service);
		fService = service;

		if (port != -1) {
			fSocket = gSystem->AnnounceTcpService(port, reuse, backlog);
		} else {
			fSocket = -1;
		}
		fIsServerSocket = kTRUE;
	}
}

//______________________________________________________________________________
TServerSocket::TServerSocket(Int_t port, Bool_t reuse, Int_t backlog)
{
   // Create a server socket object on a specified port. Set reuse to true
   // to force reuse of the server socket (i.e. do not wait for the time
   // out to pass). Using backlog one can set the desirable queue length
   // for pending connections. Use IsValid() to check the validity of the
   // server socket. In case server socket is not valid use GetErrorCode()
   // to obtain the specific error value. These values are:
   //  0 = no error (socket is valid)
   // -1 = low level socket() call failed
   // -2 = low level bind() call failed
   // -3 = low level listen() call failed
   // Every valid server socket is added to the TROOT sockets list which
   // will make sure that any open sockets are properly closed on
   // program termination.

	if(!gSystem) printf("TLynxOsSystem not initialized\n");
	else {
		fService = gSystem->GetServiceByPort(port);
		fSocket = gSystem->AnnounceTcpService(port, reuse, backlog);
		fIsServerSocket = kTRUE;
   }
}

//______________________________________________________________________________
TSocket *TServerSocket::Accept()
{
   // Accept a connection on a server socket. Returns a full-duplex
   // communication TSocket object. If no pending connections are
   // present on the queue and nonblocking mode has not been enabled
   // with SetOption(kNoBlock,1) the call blocks until a connection is
   // present. The returned socket must be deleted by the user. The socket
   // is also added to the TROOT sockets list which will make sure that
   // any open sockets are properly closed on program termination.
   // In case of error 0 is returned and in case non-blocking I/O is
   // enabled and no connections are available -1 is returned.

   if (fSocket == -1) { return 0; }

   TSocket *socket = new TSocket;

   Int_t soc = gSystem->AcceptConnection(fSocket);
   if (soc == -1) { delete socket; return 0; }
   if (soc == -2) { delete socket; return (TSocket*) -1; }

   socket->fSocket  = soc;
   socket->fService = fService;
   socket->fAddress = gSystem->GetPeerName(socket->fSocket);

   return socket;
}

//______________________________________________________________________________
TInetAddress TServerSocket::GetLocalInetAddress()
{
   // Return internet address of host to which the server socket is bound,
   // i.e. the local host. In case of error TInetAddress::IsValid() returns
   // kFALSE.

   if (fSocket != -1) {
      if (fAddress.GetPort() == -1)
         fAddress = gSystem->GetSockName(fSocket);
      return fAddress;
   }
   return TInetAddress();
}

//______________________________________________________________________________
Int_t TServerSocket::GetLocalPort()
{
   // Get port # to which server socket is bound. In case of error returns -1.

   if (fSocket != -1) {
      if (fAddress.GetPort() == -1)
         fAddress = GetLocalInetAddress();
      return fAddress.GetPort();
   }
   return -1;
}

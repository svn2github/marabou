//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TSocket                                                              //
//                                                                      //
// This class implements client sockets. A socket is an endpoint for    //
// communication between two machines.                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
// Special 'Light Weight ROOT' edition                                  //
// O. Schaile                                                           //
//////////////////////////////////////////////////////////////////////////

#include "iostream.h"
#include "iomanip.h"

#include "LwrTypes.h"
#include "LwrSocket.h"
#include "LwrLynxOsSystem.h"

UInt_t TSocket::fgBytesSent = 0;
UInt_t TSocket::fgBytesRecv = 0;

extern TLynxOsSystem * gSystem;

//______________________________________________________________________________
TSocket::TSocket(TInetAddress addr, const char *service)
{
   // Create a socket. Connect to the named service at address addr.
   // Returns when connection has been accepted by remote side. Use IsValid()
   // to check the validity of the socket. Every socket is added to the TROOT
   // sockets list which will make sure that any open sockets are properly
   // closed on program termination.

	if(!gSystem) printf("TLynxOsSystem not initialized\n");
	else {
		fIsServerSocket = kFALSE;
		fService = service;
		fAddress = addr;
		fAddress.fPort = gSystem->GetServiceByName(service);
		fBytesSent = 0;
		fBytesRecv = 0;

		if (fAddress.GetPort() != -1) {
			fSocket = gSystem->OpenConnection(addr.GetHostName(), fAddress.GetPort());
		} else {
			fSocket = -1;
		}
	}
}

//______________________________________________________________________________
TSocket::TSocket(TInetAddress addr, Int_t port)
{
   // Create a socket. Connect to the specified port # at address addr.
   // Returns when connection has been accepted by remote side. Use IsValid()
   // to check the validity of the socket. Every socket is added to the TROOT
   // sockets list which will make sure that any open sockets are properly
   // closed on program termination.

	if(!gSystem) printf("TLynxOsSystem not initialized\n");
	else {
		fIsServerSocket = kFALSE;
		fService = gSystem->GetServiceByPort(port);
		fAddress = addr;
		fAddress.fPort = port;
		fBytesSent = 0;
		fBytesRecv = 0;

		fSocket = gSystem->OpenConnection(addr.GetHostName(), fAddress.GetPort());
		if (fSocket == -1) fAddress.fPort = -1;
	}
}

//______________________________________________________________________________
TSocket::TSocket(const char *host, const char *service)
{
   // Create a socket. Connect to named service on the remote host.
   // Returns when connection has been accepted by remote side. Use IsValid()
   // to check the validity of the socket. Every socket is added to the TROOT
   // sockets list which will make sure that any open sockets are properly
   // closed on program termination.

	if(!gSystem) printf("TLynxOsSystem not initialized\n");
	else {
		fIsServerSocket = kFALSE;
		fService = service;
		fAddress = gSystem->GetHostByName(host);
		fAddress.fPort = gSystem->GetServiceByName(service);
		fBytesSent = 0;
		fBytesRecv = 0;

		if (fAddress.GetPort() != -1) {
			fSocket = gSystem->OpenConnection(host, fAddress.GetPort());
		} else {
			fSocket = -1;
		}
	}
}

//______________________________________________________________________________
TSocket::TSocket(const char *host, Int_t port)
{
   // Create a socket. Connect to specified port # on the remote host.
   // Returns when connection has been accepted by remote side. Use IsValid()
   // to check the validity of the socket. Every socket is added to the TROOT
   // sockets list which will make sure that any open sockets are properly
   // closed on program termination.

	if(!gSystem) printf("TLynxOsSystem not initialized\n");
	else {
		fIsServerSocket = kFALSE;
		fService = gSystem->GetServiceByPort(port);
		fAddress = gSystem->GetHostByName(host);
		fAddress.fPort = port;
		fBytesSent = 0;
		fBytesRecv = 0;

		fSocket = gSystem->OpenConnection(host, fAddress.GetPort());
		if (fSocket == -1) fAddress.fPort = -1;
	}
}

//______________________________________________________________________________
TSocket::TSocket(Int_t desc)
{
   // Create a socket. The socket will use descriptor desc.

	if(!gSystem) printf("TLynxOsSystem not initialized\n");
	else {
		fIsServerSocket = kFALSE;
		fBytesSent = 0;
		fBytesRecv = 0;

		if (desc >= 0) {
			fSocket  = desc;
			fAddress = gSystem->GetPeerName(fSocket);
		} else {
			fSocket = -1;
		}
	}
}

//______________________________________________________________________________
TSocket::TSocket(const TSocket &s)
{
   // TSocket copy ctor.

	fIsServerSocket = kFALSE;
	fSocket       = s.fSocket;
	fService      = s.fService;
	fAddress      = s.fAddress;
	fLocalAddress = s.fLocalAddress;
	fBytesSent    = s.fBytesSent;
	fBytesRecv    = s.fBytesRecv;
}

//______________________________________________________________________________
void TSocket::Close(Option_t *)
{
   // Close the socket. Also called via the dtor.

	if (fSocket != -1) {
		gSystem->CloseConnection(fSocket);
	}
	fSocket = -1;
}

//______________________________________________________________________________
TInetAddress TSocket::GetLocalInetAddress()
{
   // Return internet address of local host to which the socket is bound.
   // In case of error TInetAddress::IsValid() returns kFALSE.

	if (fSocket != -1) {
		if (fLocalAddress.GetPort() == -1) fLocalAddress = gSystem->GetSockName(fSocket);
		return fLocalAddress;
	}
	return TInetAddress();
}

//______________________________________________________________________________
Int_t TSocket::GetLocalPort()
{
   // Return the local port # to which the socket is bound.
   // In case of error return -1.

	if (fSocket != -1) {
		if (fLocalAddress.GetPort() == -1) fLocalAddress = GetLocalInetAddress();
		return fLocalAddress.GetPort();
	}
	return -1;
}

//______________________________________________________________________________
Int_t TSocket::SendRaw(const void * Buffer, Int_t Length, ESendRecvOptions Opt)
{
   // Send a raw buffer of specified length. Using option kOob one can send
   // OOB data. Returns the number of bytes sent or -1 in case of error.

	if (fSocket == -1) return -1;

	Int_t nsent;

	if ((nsent = gSystem->SendRaw(fSocket, Buffer, Length, (int) Opt)) < 0) return -1;

	fBytesSent  += nsent;
	fgBytesSent += nsent;

	return nsent;
}

//______________________________________________________________________________
Int_t TSocket::RecvRaw(void *buffer, Int_t length, ESendRecvOptions opt)
{
   // Receive a raw buffer of specified length bytes. Using option kPeek
   // one can peek at incoming data. Returns -1 in case of error. In case
   // of opt == kOob: -2 means EWOULDBLOCK and -3 EINVAL. In case of non-blocking
   // mode (kNoBlock) -4 means EWOULDBLOCK.

	if (fSocket == -1) return -1;

	Int_t n;

	if ((n = gSystem->RecvRaw(fSocket, buffer, length, (int) opt)) <= 0) return n;

	fBytesRecv  += n;
	fgBytesRecv += n;

	return n;
}

//______________________________________________________________________________
Int_t TSocket::SetOption(ESockOptions opt, Int_t val)
{
   // Set socket options.

	if (fSocket == -1) return -1;

	return gSystem->SetSockOpt(fSocket, opt, val);
}

//______________________________________________________________________________
Int_t TSocket::GetOption(ESockOptions opt, Int_t &val)
{
   // Get socket options. Returns -1 in case of error.

	if (fSocket == -1) return -1;

	return gSystem->GetSockOpt(fSocket, opt, &val);
}

//______________________________________________________________________________
Int_t TSocket::GetErrorCode() const
{
   // Returns error code. Meaning depends on context where it is called.
   // If no error condition returns 0 else a value < 0.
   // For example see TServerSocket ctor.

	if (!IsValid()) return fSocket;

	return 0;
}

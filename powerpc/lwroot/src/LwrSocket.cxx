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
// Special 'Light Weight ROOT' edition                                  //
// O. Schaile                                                           //
//////////////////////////////////////////////////////////////////////////

#include "iostream.h"
#include "iomanip.h"

#include "LwrTypes.h"
#include "LwrSocket.h"
#include "LwrUnixSystem.h"
#include "LwrBytes.h"

UInt_t TSocket::fgBytesSent = 0;
UInt_t TSocket::fgBytesRecv = 0;

extern TUnixSystem * gSystem;

//______________________________________________________________________________
TSocket::TSocket(TInetAddress addr, const char *service)
{
   // Create a socket. Connect to the named service at address addr.
   // Returns when connection has been accepted by remote side. Use IsValid()
   // to check the validity of the socket. Every socket is added to the TROOT
   // sockets list which will make sure that any open sockets are properly
   // closed on program termination.

	if(!gSystem) printf("UnixSystem not initialized\n");
	else {
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

	if(!gSystem) printf("UnixSystem not initialized\n");
	else {
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

	if(!gSystem) printf("UnixSystem not initialized\n");
	else {
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

	if(!gSystem) printf("UnixSystem not initialized\n");
	else {
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

	if(!gSystem) printf("UnixSystem not initialized\n");
	else {
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
Int_t TSocket::Send(Int_t Kind)
{
   // Send a single message opcode. Use kind (opcode) to set the
   // TMessage "what" field. Returns the number of bytes that were sent
   // (always sizeof(Int_t)) and -1 in case of error. In case the kind has
   // been or'ed with kMESS_ACK, the call will only return after having
   // received an acknowledgement, making the sending process synchronous.

	TMessage mess(Kind);
	return this->Send(mess);
}

//______________________________________________________________________________
Int_t TSocket::Send(const char * Str, Int_t Kind)
{
   // Send a character string buffer. Use kind to set the TMessage "what" field.
   // Returns the number of bytes in the string str that were sent and -1 in
   // case of error. In case the kind has been or'ed with kMESS_ACK, the call
   // will only return after having received an acknowledgement, making the
   // sending process synchronous.

	TMessage mess(Kind);
	if (Str) mess.WriteString(Str);

	Int_t nsent;
	if ((nsent = this->Send(mess)) < 0) return -1;

	return nsent - sizeof(Int_t);    // - TMessage::What()
}

//______________________________________________________________________________
Int_t TSocket::Send(TMessage & Mess)
{
   // Send a TMessage object. Returns the number of bytes in the TMessage
   // that were sent and -1 in case of error. In case the TMessage::What
   // has been or'ed with kMESS_ACK, the call will only return after having
   // received an acknowledgement, making the sending process synchronous.

	if (fSocket == -1) return -1;

	Int_t nsent;
	Mess.SetLength();   //write length in first word of buffer
	if ((nsent = gSystem->SendRaw(fSocket, Mess.Buffer(), Mess.Length(), 0)) < 0) return -1;

	fBytesSent  += nsent;
	fgBytesSent += nsent;

   // If acknowledgement is desired, wait for it
	if (Mess.What() & kMESS_ACK) {
		Char_t buf[2];
		if (gSystem->RecvRaw(fSocket, buf, sizeof(buf), 0) < 0) return -1;
		if (strncmp(buf, "ok", 2)) return -1;
		fBytesRecv  += 2;
		fgBytesRecv += 2;
	}

	return nsent - sizeof(UInt_t);  //length - length header
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
Int_t TSocket::Recv(TMessage & Mess)
{
	if (fSocket == -1)  return -1;

	Int_t  n;
	UInt_t len;
	n = gSystem->RecvRaw(fSocket, &len, sizeof(Int_t), 0);
	if (n <= 0) return n;
	len = net2host(len);		// from network to host byte order

	UInt_t what;
	n = gSystem->RecvRaw(fSocket, &what, sizeof(UInt_t), 0);
	if (n <= 0) return n;
	what = net2host(what);		// from network to host byte order
	len -= sizeof(UInt_t);		// compensate for 'what' item

	Mess.AllocBuffer(len);
	if ((n = gSystem->RecvRaw(fSocket, Mess.Data(), len, 0)) <= 0) return n;

	Mess.SetWhat(what);

	fBytesRecv  += n + sizeof(UInt_t);
	fgBytesRecv += n + sizeof(UInt_t);

	this->Acknowledge(Mess);

	return n;
}

//______________________________________________________________________________
Int_t TSocket::Recv(char * Str, Int_t Max)
{
   // Receive a character string message of maximum max length. The expected
   // message must be of type kMESS_STRING. Returns length of received string
   // (can be 0 if otherside of connection is closed) or -1 in case of error
   // or -4 in case a non-blocking socket would block (i.e. there is nothing
   // to be read).

	Int_t n, kind;

	if ((n = Recv(Str, Max, kind)) <= 0) return n;

	if (kind != kMESS_STRING) return -1;

	return n;
}

//______________________________________________________________________________
Int_t TSocket::Recv(char * Str, Int_t Max, Int_t & Kind)
{
   // Receive a character string message of maximum max length. Returns in
   // kind the message type. Returns length of received string (can be 0 if
   // other side of connection is closed) or -1 in case of error or -4 in
   // case a non-blocking socket would block (i.e. there is nothing to be read).

	Int_t     n;
	TMessage mess;

	if ((n = Recv(mess)) <= 0) return n;

	Kind = mess.What();
	if (Str) {
		if (mess.Length() > (Int_t)sizeof(Int_t)) mess.ReadString(Str, Max);
		else Str[0] = 0;
	}

	return n - sizeof(Int_t);   // number of bytes read - TMessage::What()
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

//______________________________________________________________________________
void TSocket::Acknowledge(TMessage & Mess)
{
	if ((Mess.What() & (kMESS_ACK | kM2L_MESS_AWD)) == 0) return;

	if ((Mess.What() & kM2L_MESS_AWD) != 0) {
		Mess.SetWhat(Mess.What() & ~kM2L_MESS_AWD);
	} else {
		char ok[2] = { 'o', 'k' };
		if (gSystem->SendRaw(fSocket, ok, sizeof(ok), 0) < 0) return -1;
		Mess.SetWhat(Mess.What() & ~kMESS_ACK);

		fBytesSent  += 2;
		fgBytesSent += 2;
	}
	return;
}

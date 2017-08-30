//________________________________________________________[C++ IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrSocket.cxx
//! \brief			Light Weight ROOT: TSocket
//! \details		Class definitions for ROOT under LynxOs: TSocket
//! 				This class implements client sockets
//!
//! \author Otto.Schaile
//!
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.6 $     
//! $Date: 2009-05-26 13:07:42 $
//////////////////////////////////////////////////////////////////////////////

#ifdef CPU_TYPE_RIO2
#include <iostream.h>
#include <iomanip.h>
#else
#include <iostream>
#include <iomanip>
#endif


#include "LwrTypes.h"
#include "LwrSocket.h"
#include "LwrLynxOsSystem.h"

UInt_t TSocket::fgBytesSent = 0;
UInt_t TSocket::fgBytesRecv = 0;

extern TLynxOsSystem * gSystem;

//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
//! \class			TSocket	LwrSocket.h
//! \details		Creates a socket.<br>
//! 				Connects to the named service at address addr.<br>
//! 				Returns when connection has been accepted by remote side.
//! 				Use IsValid() to check the validity of the socket.
//! \param[in]		Addr		-- host addr
//! \param[in]		Service		-- service name
/////////////////////////////////////////////////////////////////////////////

TSocket::TSocket(TInetAddress Addr, const Char_t * Service)
{
	if(!gSystem) printf("TLynxOsSystem not initialized\n");
	else {
		fIsServerSocket = kFALSE;
		fService = Service;
		fAddress = Addr;
		fAddress.fPort = gSystem->GetServiceByName(Service);
		fBytesSent = 0;
		fBytesRecv = 0;

		if (fAddress.GetPort() != -1) {
			fSocket = gSystem->OpenConnection(Addr.GetHostName(), fAddress.GetPort());
		} else {
			fSocket = -1;
		}
	}
}

//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
//! \details		Creates a socket.<br>
//! 				Connects to the named service at address addr.<br>
//! 				Returns when connection has been accepted by remote side.
//! 				Use IsValid() to check the validity of the socket.
//! \param[in]		Addr		-- host addr
//! \param[in]		Port		-- port number
/////////////////////////////////////////////////////////////////////////////

TSocket::TSocket(TInetAddress Addr, Int_t Port)
{
	if(!gSystem) printf("TLynxOsSystem not initialized\n");
	else {
		fIsServerSocket = kFALSE;
		fService = gSystem->GetServiceByPort(Port);
		fAddress = Addr;
		fAddress.fPort = Port;
		fBytesSent = 0;
		fBytesRecv = 0;

		fSocket = gSystem->OpenConnection(Addr.GetHostName(), fAddress.GetPort());
		if (fSocket == -1) fAddress.fPort = -1;
	}
}

//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
//! \details		Creates a socket.<br>
//! 				Connects to named service on the remote host.<br>
//! 				Returns when connection has been accepted by remote side.
//! 				Use IsValid() to check the validity of the socket.
//! \param[in]		Host		-- host name
//! \param[in]		Service		-- service name
/////////////////////////////////////////////////////////////////////////////

TSocket::TSocket(const Char_t * Host, const Char_t * Service)
{
	if(!gSystem) printf("TLynxOsSystem not initialized\n");
	else {
		fIsServerSocket = kFALSE;
		fService = Service;
		fAddress = gSystem->GetHostByName(Host);
		fAddress.fPort = gSystem->GetServiceByName(Service);
		fBytesSent = 0;
		fBytesRecv = 0;

		if (fAddress.GetPort() != -1) {
			fSocket = gSystem->OpenConnection(Host, fAddress.GetPort());
		} else {
			fSocket = -1;
		}
	}
}

//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
//! \details		Creates a socket.<br>
//! 				Connect to specified port # on the remote host.<br>
//! 				Returns when connection has been accepted by remote side.
//! 				Use IsValid() to check the validity of the socket.
//! \param[in]		Host		-- host name
//! \param[in]		Port		-- port number
/////////////////////////////////////////////////////////////////////////////

TSocket::TSocket(const Char_t * Host, Int_t Port)
{
	if(!gSystem) printf("TLynxOsSystem not initialized\n");
	else {
		fIsServerSocket = kFALSE;
		fService = gSystem->GetServiceByPort(Port);
		fAddress = gSystem->GetHostByName(Host);
		fAddress.fPort = Port;
		fBytesSent = 0;
		fBytesRecv = 0;

		fSocket = gSystem->OpenConnection(Host, fAddress.GetPort());
		if (fSocket == -1) fAddress.fPort = -1;
	}
}

//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
//! \details		Creates a socket.<br>
//! 				The socket will use descriptor desc.<br>
//! \param[in]		Desc		-- socket descriptor
/////////////////////////////////////////////////////////////////////////////

TSocket::TSocket(Int_t Desc)
{
	if(!gSystem) printf("TLynxOsSystem not initialized\n");
	else {
		fIsServerSocket = kFALSE;
		fBytesSent = 0;
		fBytesRecv = 0;

		if (Desc >= 0) {
			fSocket  = Desc;
			fAddress = gSystem->GetPeerName(fSocket);
		} else {
			fSocket = -1;
		}
	}
}

//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
//! \details		Copy contructor
//! \param[in]		Sock		-- socket object
/////////////////////////////////////////////////////////////////////////////

TSocket::TSocket(const TSocket &Sock)
{
   // TSocket copy ctor.

	fIsServerSocket = kFALSE;
	fSocket       = Sock.fSocket;
	fService      = Sock.fService;
	fAddress      = Sock.fAddress;
	fLocalAddress = Sock.fLocalAddress;
	fBytesSent    = Sock.fBytesSent;
	fBytesRecv    = Sock.fBytesRecv;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Closes the socket
//! \param[in]		Opt 		-- option
/////////////////////////////////////////////////////////////////////////////

void TSocket::Close(Option_t * Opt)
{
   // Close the socket. Also called via the dtor.

	if (fSocket != -1) {
		gSystem->CloseConnection(fSocket);
	}
	fSocket = -1;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns internet address of local host
//! 				to which the socket is bound.<br>
//! 				In case of error TInetAddress::IsValid() returns kFALSE.
/////////////////////////////////////////////////////////////////////////////

TInetAddress TSocket::GetLocalInetAddress()
{

	if (fSocket != -1) {
		if (fLocalAddress.GetPort() == -1) fLocalAddress = gSystem->GetSockName(fSocket);
		return fLocalAddress;
	}
	return TInetAddress();
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns local port number
//! 				to which the socket is bound.<br>
//! \retval 		Port	-- port number
/////////////////////////////////////////////////////////////////////////////

Int_t TSocket::GetLocalPort()
{
	if (fSocket != -1) {
		if (fLocalAddress.GetPort() == -1) fLocalAddress = GetLocalInetAddress();
		return fLocalAddress.GetPort();
	}
	return -1;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sends a raw buffer of specified length.<br>
//! 				Using option kOob one can send OOB data.<br>
//! 				Returns
//! 				<ul>
//! 				<li>	number of bytes sent
//! 				<li>	-1 in case of error.
//! 				</ul>
//! \param[in]		Buffer 		-- buffer addr
//! \param[in]		Length 		-- length of data
//! \param[in]		Opt 		-- option
//! \retval 		NofBytes	-- number of bytes sent
/////////////////////////////////////////////////////////////////////////////

Int_t TSocket::SendRaw(const void * Buffer, Int_t Length, ESendRecvOptions Opt)
{

	if (fSocket == -1) return -1;

	Int_t nsent;

	if ((nsent = gSystem->SendRaw(fSocket, Buffer, Length, (int) Opt)) < 0) return -1;

	fBytesSent  += nsent;
	fgBytesSent += nsent;

	return nsent;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Receives a raw buffer of specified length bytes.<br>
//! 				Using option kPeek one can peek at incoming data.<br>
//! 				Returns
//! 				<ul>
//! 				<li>	number of bytes received
//! 				<li>	-1 in case of error
//! 				<li>	opt == kOob: -2 means EWOULDBLOCK and -3 EINVAL
//! 				<li>	opt == kNoBlock: -4 means EWOULDBLOCK.
//! 				</ul>
//! \param[in]		Buffer 		-- buffer addr
//! \param[in]		Length 		-- length of data
//! \param[in]		Opt 		-- option
//! \retval 		NofBytes	-- number of bytes sent
/////////////////////////////////////////////////////////////////////////////

Int_t TSocket::RecvRaw(void * Buffer, Int_t Length, ESendRecvOptions Opt)
{

	if (fSocket == -1) return -1;

	Int_t n;

	if ((n = gSystem->RecvRaw(fSocket, Buffer, Length, (int) Opt)) <= 0) return n;

	fBytesRecv  += n;
	fgBytesRecv += n;

	return n;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets socket options
//! \param[in]		Opt 		-- option name
//! \param[in]		Val 		-- option value
//! \return 		0 or -1
/////////////////////////////////////////////////////////////////////////////

Int_t TSocket::SetOption(ESockOptions Opt, Int_t Val)
{
	if (fSocket == -1) return -1;
	return gSystem->SetSockOpt(fSocket, Opt, Val);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns socket options
//! \param[in]		Opt 		-- option name
//! \param[out]		Val 		-- option value
//! \return 		0 or -1
/////////////////////////////////////////////////////////////////////////////

Int_t TSocket::GetOption(ESockOptions Opt, Int_t & Val)
{
	if (fSocket == -1) return -1;
	return gSystem->GetSockOpt(fSocket, Opt, &Val);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns error code.<br>
//! 				Meaning depends on context where it is called.
//! \return 		0 or -1
/////////////////////////////////////////////////////////////////////////////

Int_t TSocket::GetErrorCode() const
{
	if (!IsValid()) return -1;
	return 0;
}

//________________________________________________________[C++ IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
//! \brief			Light Weight ROOT: TServerSocket
//! \details		Class definitions for ROOT under LynxOs: TServerSocket<br>
//! 				This class implements server sockets.<br>
//! 				A server socket waits for requests to come in over the network.<br>
//! 				It performs some operation based on that request
//! 				and then possibly returns a full duplex socket to the requester.                                                    //
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.3 $     
//! $Date: 2009-02-17 08:02:26 $
//////////////////////////////////////////////////////////////////////////////

#include "iostream.h"
#include "iomanip.h"

#include "LwrTypes.h"
#include "LwrServerSocket.h"
#include "LwrSocket.h"
#include "LwrLynxOsSystem.h"

extern TLynxOsSystem * gSystem;

//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
//! \details		Creates a server socket object for a named service.<br>
//! 				Set reuse to TRUE to force reuse of the server socket
//! 				(i.e. do not wait for the time out to pass).<br>
//! 				Using backlog one can set the desirable queue length
//! 				for pending connections.<br>
//! 				Use IsValid() to check the validity of the server socket.
//! 				In case server socket is not valid use GetErrorCode()
//! 				to obtain the specific error value.<br>
//! 				These values are:
//! 				<ul>
//! 				<li>	0 = no error (socket is valid)
//! 				<li>	-1 = low level socket() call failed
//! 				<li>	-2 = low level bind() call failed
//! 				<li>	-3 = low level listen() call failed
//! 				</ul>
//! \param[in]		Service		-- service name
//! \param[in]		Reuse		-- TRUE if sockeet is to be re-used
//! \param[in]		Backlog		-- length of queue
/////////////////////////////////////////////////////////////////////////////

TServerSocket::TServerSocket(const Char_t * Service, Bool_t Reuse, Int_t Backlog)
{

	if (!gSystem) printf("TLynxOsSystem not initialized\n");
	else {
		Int_t port = gSystem->GetServiceByName(Service);
		fService = Service;

		if (port != -1) {
			fSocket = gSystem->AnnounceTcpService(port, Reuse, Backlog);
		} else {
			fSocket = -1;
		}
		fIsServerSocket = kTRUE;
	}
}

//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
//! \details		Creates a server socket object for a specified port.<br>
//! 				Set reuse to TRUE to force reuse of the server socket
//! 				(i.e. do not wait for the time out to pass).<br>
//! 				Using backlog one can set the desirable queue length
//! 				for pending connections.<br>
//! 				Use IsValid() to check the validity of the server socket.
//! 				In case server socket is not valid use GetErrorCode()
//! 				to obtain the specific error value.<br>
//! 				These values are:
//! 				<ul>
//! 				<li>	0 = no error (socket is valid)
//! 				<li>	-1 = low level socket() call failed
//! 				<li>	-2 = low level bind() call failed
//! 				<li>	-3 = low level listen() call failed
//! 				</ul>
//! \param[in]		Port		-- port number
//! \param[in]		Reuse		-- TRUE if sockeet is to be re-used
//! \param[in]		Backlog		-- length of queue
/////////////////////////////////////////////////////////////////////////////

TServerSocket::TServerSocket(Int_t Port, Bool_t Reuse, Int_t Backlog)
{
	if (!gSystem) printf("TLynxOsSystem not initialized\n");
	else {
		fService = gSystem->GetServiceByPort(Port);
		fSocket = gSystem->AnnounceTcpService(Port, Reuse, Backlog);
		fIsServerSocket = kTRUE;
   }
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Accepts a connection on a server socket.<br>
//! 				Returns a full-duplex communication TSocket object.
//! 				If no pending connections are present on the queue
//! 				and nonblocking mode has not been enabled
//! 				with SetOption(kNoBlock,1) the call blocks until a connection
//! 				is present. The returned socket must be deleted by the user.
//! 				Returns
//! 				<ul>
//! 				<li>	pointer to socket
//! 				<li>	NULL in case of error
//! 				<li>	-1 in case of non-blocking I/O and no connections available
//! 				</ul>
//! \retval 		Socket		-- ptr to TSocket object (has to be deleted by user)
/////////////////////////////////////////////////////////////////////////////

TSocket *TServerSocket::Accept()
{
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

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns internet address of local host
//!					to which server socket is bound.<br>
//! 				In case of error TInetAddress::IsValid() returns FALSE.
//! \retval 		InetAddr		-- internet address
/////////////////////////////////////////////////////////////////////////////

TInetAddress TServerSocket::GetLocalInetAddress()
{
   if (fSocket != -1) {
      if (fAddress.GetPort() == -1)
         fAddress = gSystem->GetSockName(fSocket);
      return fAddress;
   }
   return TInetAddress();
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns port number
//!					to which server socket is bound.<br>
//! 				Returns -1 in case of error.
//! \retval 		Port		-- port number
/////////////////////////////////////////////////////////////////////////////

Int_t TServerSocket::GetLocalPort()
{
   if (fSocket != -1) {
      if (fAddress.GetPort() == -1)
         fAddress = GetLocalInetAddress();
      return fAddress.GetPort();
   }
   return -1;
}

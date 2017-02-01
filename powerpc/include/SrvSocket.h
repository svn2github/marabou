#ifndef __SrvSocket_h__
#define __SrvSocket_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			SrvSocket.h
//! \brief			MARaBOU to Lynx: communication
//! \details		Class definitions for M2L server
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.1 $     
//! $Date: 2010-10-04 10:43:26 $
//////////////////////////////////////////////////////////////////////////////

#include "LwrTypes.h"
#include "LwrNamed.h"
#include "LwrServerSocket.h"
#include "LwrArrayI.h"
#include "LwrNamedX.h"
#include "LwrLofNamedX.h"

#include "M2L_MessageTypes.h"
#include "M2L_CommonStructs.h"

class SrvSocket : public TNamed {

	public:

		//! Constructor
		//! \param[in]		Name		-- socket name
		//! \param[in]		Port		-- port number
		//! \param[in]		NonBlocking -- non-blocking flag
		//! \param[in]		VerboseMode -- kTRUE if verbose mode
		SrvSocket(const Char_t * Name, const Char_t * Path, Int_t Port, Bool_t NonBlocking, Bool_t VerboseMode);

		//! Destructor
		~SrvSocket() {};													// default dtor

		//! Return port number
		//! \retval Port	-- port number
		inline Int_t GetPort() { return(fPort); };
		//! Return socket address
		//! \retval Socket	-- socket handle
		inline TSocket * GetSocket() { return(fSocket); };
		//! Check if socket valid
		//! \return TRUE or FALSE
		inline Bool_t IsValid() { return(fSocket ? fSocket->IsValid() : kFALSE); };
		//! Return non-blocking flag
		//! \return TRUE or FALSE
		inline Bool_t IsNonBlocking() { return(fNonBlocking); };
		//! Check if verbose mode
		//! \return TRUE or FALSE
		inline Bool_t IsVerbose() { return(fVerboseMode); };

		//! Return error code
		//! \retval Error	-- error code
		inline Int_t GetErrorCode() { return(fSocket ? fSocket->GetErrorCode() : 0); };

		//! Check message type
		TMrbNamedX * CheckMessageType(M2L_MsgHdr * Hdr, const Char_t * Method, Bool_t RecvFlag, Bool_t SwapFlag);

		//! Wait for messages
		void Listen();

		//! Send acknowledge message
		void Acknowledge(TSocket * Sock, EM2L_MessageType Type = kM2L_MESS_ACK_OK, const Char_t * Text = NULL);

		//! Send acknowledge message: normal message
		//! \param[in]		Sock		-- socket
		//! \param[in]		Text		-- error text
		inline void Message(TSocket * Sock, const Char_t * Text = NULL) { this->Acknowledge(Sock, kM2L_MESS_ACK_MESSAGE, Text); };

		//! Send acknowledge message: error
		//! \param[in]		Sock		-- socket
		//! \param[in]		Text		-- error text
		inline void Error(TSocket * Sock, const Char_t * Text = NULL) { this->Acknowledge(Sock, kM2L_MESS_ACK_ERROR, Text); };

		//! Send acknowledge message: warning
		//! \param[in]		Sock		-- socket
		//! \param[in]		Text		-- warning text
		inline void Warning(TSocket * Sock, const Char_t * Text = NULL) { this->Acknowledge(Sock, kM2L_MESS_ACK_WARNING, Text); };

		inline const Char_t * ClassName() const { return "SrvSocket"; };

	protected:
		Int_t fPort;					//!< port number
		Bool_t fNonBlocking;			//!< kTRUE if non/blocking mode
		Bool_t fVerboseMode;			//!< kTRUE if verbose

		TServerSocket * fSocket;		//!< server socket
		TMrbLofNamedX fMsgTypes;		//!< list of messages types
		TMrbLofNamedX fFctTypes;		//!< list of function types
};	

#endif

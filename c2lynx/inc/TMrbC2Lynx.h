#ifndef __TMrbC2Lynx_h__
#define __TMrbC2Lynx_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           c2lynx/inc/TMrbC2Lynx.h
// Purpose:        Connect to LynxOs/VME via tcp socket
// Class:          TMrbC2Lynx            -- base class
// Description:    Class definitions to establish an
//                 client/server connection to LynxOs.
// Author:         R. Lutter
// Revision:       $Id: TMrbC2Lynx.h,v 1.2 2008-04-23 07:48:48 Rudolf.Lutter Exp $   
// Date:           $Date: 2008-04-23 07:48:48 $
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TSystem.h"
#include "TString.h"

#include "TServerSocket.h"
#include "TSocket.h"
#include "TMessage.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"

#include "MessageTypes.h"

#include "M2L_MessageTypes.h"
#include "M2L_CommonStructs.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbC2Lynx
// Purpose:        MARaBOU client to connect to LynxOs/VME.
// Description:    Creates a MARaBOU client to connect to LynxOs.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbC2Lynx : public TObject {

	public:

		TMrbC2Lynx() { this->Reset(); };												// default ctor		

		TMrbC2Lynx(const Char_t * HostName, const Char_t * Server, Int_t Port = 9010, Bool_t NonBlocking = kFALSE, Bool_t UseXterm = kTRUE);		// ctor: connect to host

		~TMrbC2Lynx() {};							// default dtor

		inline const Char_t * GetHost() { return(fHost.Data()); };
		inline const Char_t * GetServerName() { return(fServerName.Data()); };
		inline const Char_t * GetServerPath() { return(fServerPath.Data()); };
		inline Int_t GetPort() { return(fPort); };

		inline TSocket * GetSocket() { return(fSocket); };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

		inline Bool_t IsVerbose() { return(fVerboseMode); };
		inline Bool_t IsDebug() { return(fDebugMode); };

		inline Bool_t IsNonBlocking() { return(fNonBlocking); };
		inline Bool_t UseXterm() { return(fUseXterm); };

		inline Bool_t IsConnected() { return(fSocket != NULL); };

	protected:
		void Reset(); 								// reset

	protected:
		Bool_t fVerboseMode;						// kTRUE if verbose mode on
		Bool_t fDebugMode;							// kTRUE if debug, includes verbose=on

		Bool_t fNonBlocking;						// non-blocking mode
		Bool_t fUseXterm;							// kTRUE if server logs to be shown in a xterm window

		TString fHost;								// host name
		TString fServerPath;						// where to start server from
		TString fServerName;						// server program
		TSocket * fSocket;							//! connection to server
		Int_t fPort;								// port number

	ClassDef(TMrbC2Lynx, 1)		// [Access to LynxOs] Establish connection to LynxOs/VME
};

#endif

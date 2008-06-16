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
// Revision:       $Id: TMrbC2Lynx.h,v 1.3 2008-06-16 15:00:21 Rudolf.Lutter Exp $   
// Date:           $Date: 2008-06-16 15:00:21 $
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TSystem.h"
#include "TString.h"

#include "TServerSocket.h"
#include "TSocket.h"
#include "TMessage.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"

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

		TMrbC2Lynx(const Char_t * HostName, const Char_t * Server, const Char_t * LogFile = NULL, Int_t Port = 9010, Bool_t NonBlocking = kFALSE, Bool_t UseXterm = kTRUE);		// ctor: connect to host

		~TMrbC2Lynx() {};							// default dtor

		inline const Char_t * GetHost() { return(fHost.Data()); };
		inline const Char_t * GetServerName() { return(fServerName.Data()); };
		inline const Char_t * GetServerPath() { return(fServerPath.Data()); };
		inline const Char_t * GetLogFile() { return(fLogFile.Data()); };
		inline Int_t GetPort() { return(fPort); };

		inline TSocket * GetSocket() { return(fSocket); };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

		inline Bool_t IsVerbose() { return(fVerboseMode); };
		inline Bool_t IsDebug() { return(fDebugMode); };

		inline Bool_t IsNonBlocking() { return(fNonBlocking); };
		inline Bool_t UseXterm() { return(fUseXterm); };

		inline Bool_t IsConnected() { return(fSocket != NULL); }

		Bool_t Connect();

		Bool_t Send(M2L_MsgHdr * Hdr);
		Bool_t Recv(M2L_MsgHdr * Hdr);

		inline UInt_t What(M2L_MsgHdr * Hdr) { return(Hdr->fWhat); };
		inline Int_t Length(M2L_MsgHdr * Hdr) { return(Hdr->fLength); };
 
		void Bye();

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
		TString fLogFile;							// where to write log info
		TSocket * fSocket;							//! connection to server
		Int_t fPort;								// port number

	ClassDef(TMrbC2Lynx, 1)		// [Access to LynxOs] Establish connection to LynxOs/VME
};

#endif

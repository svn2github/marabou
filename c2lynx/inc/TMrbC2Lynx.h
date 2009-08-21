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
// Revision:       $Id: TMrbC2Lynx.h,v 1.13 2009-08-21 10:02:32 Rudolf.Lutter Exp $
// Date:           $Date: 2009-08-21 10:02:32 $
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TSystem.h"
#include "TString.h"
#include "TNamed.h"

#include "TServerSocket.h"
#include "TSocket.h"
#include "TMessage.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"

#include "TC2LVMEModule.h"

#include "M2L_MessageTypes.h"
#include "M2L_CommonStructs.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbC2Lynx
// Purpose:        MARaBOU client to connect to LynxOs/VME.
// Description:    Creates a MARaBOU client to connect to LynxOs.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbC2Lynx : public TNamed {

	public:
		enum EC2LServerLog	{	kC2LServerLogNone	=	BIT(0),
								kC2LServerLogXterm	=	BIT(1),
								kC2LServerLogPipe	=	BIT(2),
								kC2LServerLogDebug	=	BIT(3)
		};

	public:

		TMrbC2Lynx() { this->Reset(); };												// default ctor

		TMrbC2Lynx(const Char_t * HostName, const Char_t * Server = NULL, const Char_t * LogFile = NULL, Int_t Port = 9010,
						const Char_t * ServerLog = "None", Bool_t ConnectFlag = kTRUE);		// ctor: connect to host

		~TMrbC2Lynx() {};							// default dtor

		Bool_t SetServerLog(const Char_t * Output);
		Bool_t SetServerLog(EC2LServerLog Output);
		inline void SetNonBlocking(Bool_t Flag = kFALSE) { fNonBlocking = Flag; };

		inline const Char_t * GetHost() { return(fHost.Data()); };
		inline const Char_t * GetServerName() { return(fServerName.Data()); };
		inline const Char_t * GetServerPath() { return(fServerPath.Data()); };
		inline const Char_t * GetLogFile() { return(fLogFile.Data()); };
		inline Int_t GetPort() { return(fPort); };

		inline TSocket * GetSocket() { return(fSocket); };

		inline Bool_t IsVerbose() { return(fVerboseMode); };
		inline Bool_t IsDebug() { return(fDebugMode); };

		inline Bool_t IsNonBlocking() { return(fNonBlocking); };
		inline Bool_t Log2Xterm() { return(fServerLog->GetIndex() & kC2LServerLogXterm); };
		inline Bool_t Log2Pipe() { return(fServerLog->GetIndex() & kC2LServerLogPipe); };
		inline TMrbNamedX * GetServerLog() { return(fServerLog); };
		inline FILE * GetPipe() { return(fPipe); };

		inline Bool_t IsConnected() { return(fSocket != NULL); }

		Bool_t Connect(Bool_t WaitFlag = kTRUE);
		Bool_t WaitForConnection();

		Bool_t Send(M2L_MsgHdr * Hdr);
		Bool_t Recv(M2L_MsgHdr * Hdr);

		inline void AddModule(TMrbNamedX * Module) { fLofModules.AddLast(Module); };
		inline TMrbLofNamedX * LofModules() { return(&fLofModules); };
		inline TC2LVMEModule * FindModule(const Char_t * ModuleName) { return((TC2LVMEModule *) fLofModules.FindByName(ModuleName)); };

		inline UInt_t What(M2L_MsgHdr * Hdr) { return(Hdr->fWhat); };
		inline Int_t Length(M2L_MsgHdr * Hdr) { return(Hdr->fLength); };

		void InitMessage(M2L_MsgHdr * Hdr, Int_t Length, UInt_t What);
		M2L_MsgHdr * AllocMessage(Int_t Length, Int_t Wc, UInt_t What);

		void Bye();

		Bool_t CheckAccessToLynxOs(const Char_t * Host, const Char_t * ServerPath);
		Bool_t CheckVersion(TString & Cpu, TString & Lynx, const Char_t * Host = NULL, const Char_t * ServerPath = NULL);

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		void Reset(); 								// reset

	protected:
		Bool_t fVerboseMode;						// kTRUE if verbose mode on
		Bool_t fDebugMode;							// kTRUE if debug, includes verbose=on

		TMrbNamedX * fServerLog;					// where to output server messages
		Bool_t fNonBlocking;						// non-blocking mode
		Bool_t fUseXterm;							// kTRUE if server logs to be shown in a xterm window

		TString fHost;								// host name
		TString fServerPath;						// where to start server from
		TString fServerName;						// server program
		TString fLogFile;							// where to write log info
		TSocket * fSocket;							//! connection to server
		Int_t fPort;								// port number

		FILE * fPipe;								//! stream for pipe output

		TMrbLofNamedX fLofServerLogs;				// list of server log modes
		TMrbLofNamedX fLofModules;					// list of vme/camac modules connected

	ClassDef(TMrbC2Lynx, 1)		// [Access to LynxOs] Establish connection to LynxOs/VME
};

#endif

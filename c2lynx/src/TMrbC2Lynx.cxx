//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           c2lynx/src/TMrbC2Lynx.cxx
// Purpose:        MARaBOU client to connect to LynxOs/VME
// Description:    Implements class methods for a client to LynxOs/VME
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbC2Lynx.cxx,v 1.5 2008-07-02 07:03:20 Rudolf.Lutter Exp $     
// Date:           $Date: 2008-07-02 07:03:20 $
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <time.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "Rtypes.h"
#include "TSystem.h"
#include "TEnv.h"
#include "TObjString.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbLogger.h"
#include "TMrbSystem.h"

#include "TMrbC2Lynx.h"

#include "SetColor.h"

extern TMrbLogger * gMrbLog;

TMrbC2Lynx * gMrbC2Lynx = NULL;

ClassImp(TMrbC2Lynx)

TMrbC2Lynx::TMrbC2Lynx(const Char_t * HostName, const Char_t * Server, const Char_t * LogFile, Int_t Port, Bool_t NonBlocking, Bool_t UseXterm) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbC2Lynx
// Purpose:        Connect to LynxOs
// Arguments:      Char_t * HostName      -- server host
//                 Char_t * Server        -- server path
//                 Char_t * LogFile       -- where to write log messages
//                 Int_t Port             -- tcp port
//                 Bool_t NonBlocking     -- kTRUE, if non-blocking mode
//                 Bool_t UseXterm        -- open cterm to show server output
// Results:        --
// Description:    Class constructor
//////////////////////////////////////////////////////////////////////////////

	if (gMrbC2Lynx != NULL) {
		gMrbLog->Err()	<< "Connection to LynxOs already established" << endl;
		gMrbLog->Flush(this->ClassName());
		this->MakeZombie();
	} else {
		this->Reset();

		fHost = HostName;
		fPort = Port;

		fNonBlocking = NonBlocking;
		fUseXterm = UseXterm;

		fSocket = NULL;
	
		fServerPath = Server;
		fServerName = gSystem->BaseName(Server);
		fLogFile = (LogFile == NULL || *LogFile == '\0') ? Form("%s/c2lynx.log", gSystem->WorkingDirectory()) : LogFile;

		if (this->Connect()) gMrbC2Lynx = this; else this->MakeZombie();
	}
}

Bool_t TMrbC2Lynx::Connect() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbC2Lynx::Connect
// Purpose:        Connect to server
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    (Re)Connects to server.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->IsConnected()) return(kTRUE);

	TSocket * s = new TSocket(fHost.Data(), fPort);
	Bool_t sockOk = s->IsValid();
	if (sockOk) {
		if (fVerboseMode || fDebugMode) {
			gMrbLog->Out()	<< "Connecting to server " << fHost << ":" << fPort
							<< " (progr " << fServerName << ")" << endl;
			gMrbLog->Flush(this->ClassName(), "Connect");
		}
		fSocket = s;
		return(kTRUE);
	} else {
		if (fVerboseMode || fDebugMode) {
			gMrbLog->Out()	<< "Trying to connect to server " << fHost << ":" << fPort
							<< " (progr " << fServerName << ")" << endl;
			gMrbLog->Flush(this->ClassName(), "Connect");
		}
		delete s;
		TString cmd1, cmd2;
		if (fUseXterm) {
			cmd1 = "xterm -title ";
			cmd1 += fServerName;
			cmd1 += "@";
			cmd1 += fHost;
			cmd1 += " -e rsh ";
		} else {
			cmd1 = "rsh ";
		}
		cmd1 += fHost;
		cmd1 += " ";

		cmd2 += fServerPath;
		cmd2 += " ";
		cmd2 += fPort;
		cmd2 += " ";
		cmd2 += fLogFile;
		cmd2 += " ";
		cmd2 += (fNonBlocking ? 1 : 0);
		cmd2 += " ";
		cmd2 += (fVerboseMode ? 1 : 0);

		if (fDebugMode) {
			gMrbLog->Out()	<< "[Debug mode] Start manually @ " << fHost << " >> " << cmd2 << " <<" << endl;
			gMrbLog->Flush(this->ClassName(), "Connect");
		} else {
			cmd1 += cmd2;
			cmd1 += " &";
			if (fVerboseMode) {
				gMrbLog->Out()	<< "Exec >> " << cmd1 << " <<" << endl;
				gMrbLog->Flush(this->ClassName());
			}
			gSystem->Exec(cmd1.Data());
		}

		Int_t wait = fDebugMode ? 100 : 10;
		for (Int_t i = 0; i < wait; i++) {
			sleep(1);
			cout << "." << flush;
			s = new TSocket(fHost.Data(), fPort);
			sockOk = s->IsValid();
			if (sockOk) break;
			delete s;
		}
		if (sockOk) {
			cout << " done." << endl;
			fSocket = s;
			return(kTRUE);
		} else {
			cout << endl;
			gMrbLog->Err()	<< "Can't connect to server/port " << fHost << ":" << fPort << endl;
			gMrbLog->Flush(this->ClassName(), "Connect");
			fSocket = NULL;
			return(kFALSE);
		}
	}
}
	
void TMrbC2Lynx::Reset() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbC2Lynx::Reset
// Purpose:        Reset internal data base
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Reset data base (protected method).
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger("c2lynx.log");
	
	fVerboseMode = gEnv->GetValue("TMrbC2Lynx.VerboseMode", kFALSE);
	fDebugMode = gEnv->GetValue("TMrbC2Lynx.DebugMode", kFALSE);

	fNonBlocking = kFALSE;
	fUseXterm = kFALSE;

	fHost.Resize(0);								// host name
	fServerPath.Resize(0);
	fServerName.Resize(0);

	fPort = -1;
	fSocket = NULL;
}

Bool_t TMrbC2Lynx::Send(M2L_MsgHdr * Hdr) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbC2Lynx::Send
// Purpose:        Send a message
// Arguments:      M2L_MsgHdr * Hdr  -- (header of) message to be sent
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sends a message over internal socket.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->IsConnected()) {
		Int_t n = fSocket->SendRaw((void *) Hdr, Hdr->fLength);
		if (n <= 0) {
			gMrbLog->Err()	<< "Couldn't send message (code=" << n << ")" << endl;
			gMrbLog->Flush(this->ClassName(), "Send");
			return(kFALSE);
		}
		return(kTRUE);
	} else {
		gMrbLog->Err()	<< "Not connected to server" << endl;
		gMrbLog->Flush(this->ClassName(), "Send");
		return(kFALSE);
	}
}

Bool_t TMrbC2Lynx::Recv(M2L_MsgHdr * Hdr) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbC2Lynx::Recv
// Purpose:        Receive a message
// Arguments:      M2L_MsgHdr * Hdr  -- (header of) message
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sends a message over internal socket.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->IsConnected()) {
		M2L_MsgHdr reqHdr;
		memcpy(&reqHdr, Hdr, sizeof(M2L_MsgHdr));
		if (reqHdr.fLength) {
			Int_t n = fSocket->RecvRaw((void *) Hdr, sizeof(M2L_MsgHdr));
			if (n != sizeof(M2L_MsgHdr)) {
				gMrbLog->Err()	<< "Couldn't receive header (error code=" << n << ")" << endl;
				gMrbLog->Flush(this->ClassName(), "Recv");
				return(kFALSE);
			}
			if (Hdr->fWhat >= kM2L_MESS_ACK_OK && Hdr->fWhat < kM2L_MESS_ACK_END) {
				M2L_Acknowledge ack;
				n = fSocket->RecvRaw((void *) ack.fText, Hdr->fLength - sizeof(M2L_MsgHdr));
				if (n <= 0) {
					gMrbLog->Err()	<< "Couldn't receive message (error code=" << n << ")" << endl;
					gMrbLog->Flush(this->ClassName(), "Recv");
					return(kFALSE);
				}
				switch (Hdr->fWhat) {
					case kM2L_MESS_ACK_OK:			return(kTRUE);
					case kM2L_MESS_ACK_MESSAGE: 	cout << "*-" << ack.fText << endl; break;
					case kM2L_MESS_ACK_WARNING: 	cerr << "%-" << ack.fText << endl; break;
					case kM2L_MESS_ACK_ERROR:		cerr << setred << "?-" << ack.fText << setblack << endl; break;
				}
				return(kFALSE);
			}
			n = fSocket->RecvRaw((void *)((Char_t *) Hdr + sizeof(M2L_MsgHdr)), Hdr->fLength - sizeof(M2L_MsgHdr));
			if (n <= 0) {
				gMrbLog->Err()	<< "Couldn't receive message (error code=" << n << ")" << endl;
				gMrbLog->Flush(this->ClassName(), "Recv");
				return(kFALSE);
			}
			return(kTRUE);
		} else {
			gMrbLog->Err()	<< "Requested message length is 0" << endl;
			gMrbLog->Flush(this->ClassName(), "Recv");
			return(kFALSE);
		}	
	} else {
		gMrbLog->Err()	<< "Not connected to server" << endl;
		gMrbLog->Flush(this->ClassName(), "Recv");
		return(kFALSE);
	}
}

void TMrbC2Lynx::Bye() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbC2Lynx::Bye
// Purpose:        Terminate session
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Sends a bye message to server and terminates connection.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->IsConnected()) {
		M2L_MsgHdr bye;
		bye.fWhat = kM2L_MESS_BYE;
		bye.fLength = sizeof(M2L_MsgHdr);
		this->Send(&bye);
		fSocket->Close();
		fSocket = NULL;
	} else {
		gMrbLog->Err()	<< "Not connected to server" << endl;
		gMrbLog->Flush(this->ClassName(), "Bye");
	}
	return;
}

void TMrbC2Lynx::InitMessage(M2L_MsgHdr * Hdr, Int_t Length, UInt_t What) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbC2Lynx::InitMessage
// Purpose:        Initialize message
// Arguments:      M2L_MsgHdr * Hdr    -- ptr to message (header)
//                 Int_t Length        -- message length
//                 UInt_t What         -- message type
// Results:        --
// Exceptions:
// Description:    Clears message data and fills header
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	memset((Char_t *) Hdr, 0, Length);
	Hdr->fLength = Length;
	Hdr->fWhat = What;
}


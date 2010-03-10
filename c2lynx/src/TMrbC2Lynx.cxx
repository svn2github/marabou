//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           c2lynx/src/TMrbC2Lynx.cxx
// Purpose:        MARaBOU client to connect to LynxOs/VME
// Description:    Implements class methods for a client to LynxOs/VME
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbC2Lynx.cxx,v 1.11 2010-03-10 12:08:10 Rudolf.Lutter Exp $
// Date:           $Date: 2010-03-10 12:08:10 $
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <time.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "Rtypes.h"
#include "TROOT.h"
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

static Char_t * gMsgBuffer = NULL; 			// common data storage for messages
static Int_t gBytesAllocated = 0;			// size of message storage

ClassImp(TMrbC2Lynx)

const SMrbNamedX kC2LLofServerLogs[] =
			{
				{TMrbC2Lynx::kC2LServerLogNone,	 	"none",		"No server output at all" 	},
				{TMrbC2Lynx::kC2LServerLogCout, 	"cout",		"Server output to cout/cerr" 	},
				{TMrbC2Lynx::kC2LServerLogXterm, 	"xterm",	"Server output to XTERM window"		},
				{0, 								NULL,			NULL						}
			};

TMrbC2Lynx::TMrbC2Lynx(const Char_t * HostName, const Char_t * Server, const Char_t * LogFile,
					Int_t Port, const Char_t * ServerLog, Bool_t ConnectFlag) : TNamed(HostName, "Marabou-Lynx connection") {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbC2Lynx
// Purpose:        Connect to LynxOs
// Arguments:      Char_t * HostName      -- server host
//                 Char_t * Server        -- server path
//                 Char_t * LogFile       -- where to write log messages
//                 Int_t Port             -- tcp port
//                 Char_t * ServerLog     -- where to output server logs
//                 Bool_t ConnectFlag     -- try to establish connection
// Results:        --
// Description:    Class constructor
//////////////////////////////////////////////////////////////////////////////

	if (gMrbC2Lynx != NULL) {
		gMrbLog->Err()	<< "Connection to LynxOs already established" << endl;
		gMrbLog->Flush(this->ClassName());
		this->MakeZombie();
	} else {
		this->Reset();

		fLofServerLogs.SetName("Server output modes");
		fLofServerLogs.AddNamedX(kC2LLofServerLogs);

		fHost = HostName;
		fPort = Port;

		fNonBlocking = kFALSE;
		fSocket = NULL;

		fServerLog = fLofServerLogs.FindByName(ServerLog);
		if (fServerLog == NULL) {
			gMrbLog->Err()	<< "Illegal server output mode - " << ServerLog << endl;
			gMrbLog->Flush(this->ClassName());
			this->MakeZombie();
		}

		if (!this->IsZombie()) {
			if (Server == NULL || *Server == '\0') {
				TString lynxVersion = gEnv->GetValue("TMrbC2Lynx.LynxVersion", "");
				if (lynxVersion.IsNull()) lynxVersion = gEnv->GetValue("TMbsSetup.LynxVersion", "");
				if (lynxVersion.IsNull()) {
					gMrbLog->Err()	<< "Lynx version missing - set \"TMrbC2Lynx.LynxVersion\" properly" << endl;
					gMrbLog->Flush(this->ClassName());
					this->MakeZombie();
				} else {
					fServerPath = Form("$MARABOU/powerpc/bin/%s/mrbLynxOsSrv", lynxVersion.Data());
					gSystem->ExpandPathName(fServerPath);
				}
			} else {
				fServerPath = Server;
			}
			if (!this->IsZombie()) {
				fServerName = gSystem->BaseName(fServerPath.Data());
				fLogFile = (LogFile == NULL || *LogFile == '\0') ? Form("%s/c2lynx.log", gSystem->WorkingDirectory()) : LogFile;

				if (ConnectFlag) {
					if (this->Connect(kTRUE)) {
						gMrbC2Lynx = this;
						gROOT->Append(this);
					} else {
						this->MakeZombie();
					}
				}
			}
		}
	}
}

Bool_t TMrbC2Lynx::Connect(Bool_t WaitFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbC2Lynx::Connect
// Purpose:        Connect to server
// Arguments:      Bool_t WaitFlag  -- wait for connection to establish if kTRUE
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    (Re)Connects to server.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->IsConnected()) return(kTRUE);

	TString cpu, lynx;
	if (!this->CheckVersion(cpu, lynx, fHost.Data(), fServerPath.Data())) {
		gMrbLog->Err()	<< "Version mismatch - cpu=" << cpu << ", lynx=" << lynx << endl;
		gMrbLog->Flush(this->ClassName(), "Connect");
		return(kFALSE);
	}

	TSocket * s = new TSocket(fHost.Data(), fPort);
	Bool_t sockOk = s->IsValid();
	if (sockOk) {
		if (fVerboseMode || fDebugMode) {
			gMrbLog->Out()	<< "Connecting to server " << fHost << ":" << fPort
							<< " (progr " << fServerName << ")" << endl;
			gMrbLog->Flush(this->ClassName(), "Connect");
		}
		fSocket = s;
		gMrbC2Lynx = this;
		gROOT->Append(this);
		return(kTRUE);
	} else {
		if (fVerboseMode || fDebugMode) {
			gMrbLog->Out()	<< "Trying to connect to server " << fHost << ":" << fPort
							<< " (progr " << fServerName << ")" << endl;
			gMrbLog->Flush(this->ClassName(), "Connect");
		}
		delete s;
		TString cmd1, cmd2;
		if (this->LogXterm()) {
			cmd1 = "konsole -title ";
			cmd1 += fServerName;
			cmd1 += "@";
			cmd1 += fHost;
			cmd1 += " -e rsh ";
		} else {
			cmd1 = "rsh ";
		}
		cmd1 += fHost;
		cmd1 += " ";

		cmd2 = fServerPath;
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
			TMrbNamedX * sl = this->GetServerLog();
			if (sl) {
				switch (sl->GetIndex()) {
					case kC2LServerLogNone:		cmd1 += " 1>/dev/null 2>/dev/null"; break;
					case kC2LServerLogCout:		cmd1 += " 2>/dev/null"; break;
					case kC2LServerLogXterm:	break;
				}
			}
			cmd1 += " &";
			if (fVerboseMode) {
				gMrbLog->Out()	<< "Exec >> " << cmd1 << " <<" << endl;
				gMrbLog->Flush(this->ClassName());
			}
			gSystem->Exec(cmd1.Data());
		}

		if (!WaitFlag) return(kTRUE);

		Int_t wait = fDebugMode ? 100 : 10;
		for (Int_t i = 0; i < wait; i++) {
			sleep(1);
			if(this->WaitForConnection()) return(kTRUE);
		}
		gMrbLog->Err()	<< "Can't connect to server/port " << fHost << ":" << fPort << endl;
		gMrbLog->Flush(this->ClassName(), "Connect");
		fSocket = NULL;
		return(kFALSE);
	}
}

Bool_t TMrbC2Lynx::WaitForConnection() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbC2Lynx::WaitForConnection
// Purpose:        Connect to server
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Wait for connection to establish.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->IsConnected()) return(kTRUE);
	TSocket * s = new TSocket(fHost.Data(), fPort);
	Bool_t sockOk = s->IsValid();
	if (sockOk) {
		fSocket = s;
		gMrbC2Lynx = this;
		gROOT->Append(this);
		return(kTRUE);
	}
	delete s;
	return(kFALSE);
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
	fServerLog = fLofServerLogs.FindByIndex(kC2LServerLogCout);

	fHost.Resize(0);								// host name
	fServerPath.Resize(0);
	fServerName.Resize(0);

	fPort = -1;
	fSocket = NULL;
}

Bool_t TMrbC2Lynx::SetServerLog(const Char_t * Output) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbC2Lynx::SetServerLog
// Purpose:        Define output mode
// Arguments:      Char_t * Output   -- output mode
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines output mode for server logs
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * sl = fLofServerLogs.FindByName(Output);
	if (sl == NULL) {
		gMrbLog->Err()	<< "Illegal output mode " << Output << endl;
		gMrbLog->Flush(this->ClassName(), "SetServerLog");
		return(kFALSE);
	} else {
		fServerLog = sl;
		return(kTRUE);
	}
}

Bool_t TMrbC2Lynx::SetServerLog(EC2LServerLog Output) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbC2Lynx::SetServerLog
// Purpose:        Define output mode
// Arguments:      EC2LServerLog Output   -- output mode
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines output mode for server logs
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * sl = fLofServerLogs.FindByIndex(Output);
	if (sl == NULL) {
		gMrbLog->Err()	<< "Illegal output mode " << setbase(16) << Output << setbase(10) << endl;
		gMrbLog->Flush(this->ClassName(), "SetServerLog");
		return(kFALSE);
	} else {
		fServerLog = sl;
		return(kTRUE);
	}
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
		M2L_Acknowledge a;
		this->InitMessage((M2L_MsgHdr *) &a, sizeof(M2L_Acknowledge), kM2L_MESS_BYE);
		if (this->Recv((M2L_MsgHdr *) &a)) {
			fSocket->Close();
			fSocket = NULL;
		}
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

M2L_MsgHdr * TMrbC2Lynx::AllocMessage(Int_t Length, Int_t Wc, UInt_t What) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbC2Lynx::AllocMessage
// Purpose:        Allocate message
// Arguments:      Int_t Length        -- message length
//                 Int_t Wc            -- length of data (words)
//                 UInt_t What         -- message type
// Results:        M2L_MsgHdr * Hdr    -- ptr to message (header)
// Exceptions:
// Description:    Allocates message on heap
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t lengthNeeded = Length + (Wc - 1) * sizeof(Int_t);
	if (gMsgBuffer) {
		if (gBytesAllocated < lengthNeeded) {
			free(gMsgBuffer);
			gMsgBuffer = NULL;
			gBytesAllocated = 0;
		}
	}
	if (gMsgBuffer == NULL) {
		gMsgBuffer = (Char_t *) calloc(1, lengthNeeded);
		gBytesAllocated = lengthNeeded;
	}
	memset(gMsgBuffer, 0, gBytesAllocated);
	this->InitMessage((M2L_MsgHdr *) gMsgBuffer, lengthNeeded, What);
	return((M2L_MsgHdr *) gMsgBuffer);
}

Bool_t TMrbC2Lynx::CheckAccessToLynxOs(const Char_t * Host, const Char_t * Path) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbC2Lynx::CheckAccessToLynxOs
// Purpose:        Check access to file
// Arguments:      Char_t * Host        -- name of ppc host
//                 Char_t * Path        -- path to lynxOs server
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks if path can be seen from lynxOs
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString host = Host;
	TString path = Path;

	TString cmd = Form("rsh %s -e pwd 2>&1", host.Data());
	FILE * rsh = gSystem->OpenPipe(cmd.Data(), "r");
	if (!rsh) {
		gMrbLog->Err()	<< "Can't exec command - \"" << cmd << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "CheckAccessToLynxOs");
		return(kFALSE);
	}

	char line[512];
	fgets(line, 512, rsh);
	TString str = line;
	if (str.IsNull() || str.Contains("No route")) {
		gMrbLog->Err()	<< "Can't access host - " << host << endl;
		gMrbLog->Flush(this->ClassName(), "CheckAccessToLynxOs");
		return(kFALSE);
	}

	cmd = Form("rsh %s -e \'echo @X@%cls %s%c@X@\'", host.Data(), '\`', path.Data(), '\`');
	FILE * ls = gSystem->OpenPipe(cmd.Data(), "r");
	if (!ls) {
		gMrbLog->Err()	<< "Can't exec command - \"" << cmd << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "CheckAccessToLynxOs");
		return(kFALSE);
	}

	fgets(line, 512, ls);
	str = line;
	if (str.Contains("@X@@X@") || !str.Contains(Form("@X@%s", path.Data()))) {
		gMrbLog->Err()	<< "Can't access file from LynxOs host - \"" << host << ":" << path << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "CheckAccessToLynxOs");
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TMrbC2Lynx::CheckVersion(TString & Cpu, TString & Lynx, const Char_t * Host, const Char_t * Path) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbC2Lynx::CheckVersion
// Purpose:        Check server version
// Arguments:      Char_t * Host        -- name of ppc host
//                 Char_t * Path        -- path to lynxOs server
// Results:        kTRUE / kFALSE
//                 TString & Cpu        -- cpu version
//                 TString & Lynx       -- lynx version
// Exceptions:
// Description:    Checks file on server machine for occurrence of "@#@" strings:
//                 @#@CPU=RIOn
//                 @#@LynxOs=n.n
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Cpu = "n.a";
	Lynx = "n.a";

	TString host = Host;
	if (host.IsNull()) host = fHost;

	TString serverPath = Path;
	if (serverPath.IsNull()) serverPath = fServerPath;

	if (!this->CheckAccessToLynxOs(host.Data(), serverPath.Data())) return(kFALSE);

	TString cmd = Form("rsh %s -e strings %s | grep @#@", host.Data(), serverPath.Data());
	FILE * grep = gSystem->OpenPipe(cmd.Data(), "r");
	if (!grep) {
		gMrbLog->Err()	<< "Can't exec grep command - \"" << cmd << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "CheckVersion");
		return(kFALSE);
	}

	Char_t buf[128];
	while (fgets(buf, 128, grep)) {
		TString str = buf;
		str.Resize(str.Length() - 1);
		if (str.Contains("@#@")) {
			Int_t x = str.Index("CPU=", 0);
			if (x > 0) {
				Cpu = str(x + 4, 10);
				continue;
			}
			x = str.Index("LynxOs=", 0);
			if (x > 0) {
				Lynx = str(x + 7, 10);
				continue;
			}
		}
	}
	fclose(grep);
	return(Cpu.CompareTo("n.a") != 0 && Lynx.CompareTo("n.a") != 0);
}

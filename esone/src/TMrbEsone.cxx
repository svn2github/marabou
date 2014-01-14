//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           esone/src/TMrbEsone.cxx
// Purpose:        ESONE client class
// Description:    Implements class methods to interface the ESONE client library
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbEsone.cxx,v 1.18 2010-03-10 12:08:10 Rudolf.Lutter Exp $
// Date:
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

#include "esone_protos.h"

#include "TMrbEsone.h"

#include "SetColor.h"

const SMrbNamedX kMrbLofCamacActions[] =		// list of camac actions
			{
				{kCANone,	 "n.a", 	 "No Action",											 },
				{kCA_cfsa,   "cfsa",	 "Single CAMAC Action, 24 bit"  						 },
				{kCA_cssa,   "cssa",	 "Single CAMAC Action, 16 bit"  						 },
				{kCA_cfga,   "cfga",	 "General Multiple CAMAC Action, 24 bit"				 },
				{kCA_csga,   "csga",	 "General Multiple CAMAC Action, 16 bit"				 },
				{kCA_cfmad,  "cfmad",	 "Address Scan, 24 bit" 								 },
				{kCA_csmad,  "csmad",	 "Address Scan, 16 bit" 								 },
				{kCA_cfubc,  "cfubc",	 "Controller Synchronized Block Xfer, 24 bit"			 },
				{kCA_csubc,  "csubc",	 "Controller Synchronized Block Xfer, 16 bit"			 },
				{kCA_cfubr,  "cfubr",	 "Controller Synchronized Block Xfer, Q-driven, 24 bit"  },
				{kCA_csubr,  "csubr",	 "Controller Synchronized Block Xfer, Q-driven, 16 bit"  },
				{0, 			NULL,		NULL							 				}
			};

const SMrbNamedXShort kMrbLofCnafNames[] =
							{
								{TMrbEsoneCnaf::kCnafCrate, 	"CRATE",			},
								{TMrbEsoneCnaf::kCnafStation,	"NSTATION"	 		},
								{TMrbEsoneCnaf::kCnafAddr,		"ADDRESS"			},
								{TMrbEsoneCnaf::kCnafFunction,	"FUNCTION"	 		},
								{TMrbEsoneCnaf::kCnafData,		"DATA"	 			},
								{0, 							NULL				}
							};

const SMrbNamedXShort kMrbLofCnafTypes[] =
							{
								{kCnafTypeRead, 		 "READ" 	 },
								{kCnafTypeWrite,		 "WRITE"	 },
								{kCnafTypeControl,  	 "CNTRL"	 },
								{kCnafTypeReadStatus,	 "RSTATUS"   },
								{0, 					NULL}
							};

const SMrbNamedXShort kMrbLofCamacFunctions[] =
							{
								{kCF_RD1,			 "read group 1 register"				 },
								{kCF_RD2,			 "read group 2 register"				 },
								{kCF_RC1,			 "read and clear group 1 register"  	 },
								{kCF_RCM,			 "read complement of group 1 register"   },
								{kCF_TLM,			 "test LAM" 							 },
								{kCF_CL1,			 "clear group 1 register"				 },
								{kCF_CLM,			 "clear LAM"							 },
								{kCF_CL2,			 "clear group 2 register"				 },
								{kCF_WT1,			 "write group 1 register"				 },
								{kCF_WT2,			 "write group 2 register"				 },
								{kCF_SS1,			 "selective set group 1 register"		 },
								{kCF_SS2,			 "selective set group 2 register"		 },
								{kCF_SC1,			 "selective clear group 1 register" 	 },
								{kCF_SC2,			 "selective clear group 2 register" 	 },
								{kCF_DIS,			 "disable"  							 },
								{kCF_XEQ,			 "execute"  							 },
								{kCF_ENB,			 "enable"								 },
								{kCF_TST,			 "test" 								 },
								{0, 								NULL									}
							};

const SMrbNamedXShort kMrbLofEsoneServers[] =
							{
								{TMrbEsone::kES_MBS,				"MBS"		},
								{TMrbEsone::kES_MARABOU,			"MARaBOU"	},
								{0, 								NULL		}
							};

const SMrbNamedXShort kMrbLofControllers[] =
							{
								{TMrbEsone::kCC_CBV,				"CBV"		},
								{TMrbEsone::kCC_CC32,				"CC32"		},
								{0, 								NULL		}
							};

TMrbLofNamedX * gMrbEsoneHosts = NULL;

extern TMrbLogger * gMrbLog;

ClassImp(TMrbEsone)

Bool_t TMrbEsone::Reset(Bool_t Offline) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::Reset
// Purpose:        Reset internal data base
// Arguments:      Bool_t Offline         -- camac access to be simulated?
// Results:        --
// Exceptions:
// Description:    Reset data base (protected method).
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString esoneServer;
	TString camacContr;
	TMrbNamedX * nx;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger("esone.log");

	fVerboseMode = gEnv->GetValue("TMrbEsone.VerboseMode", kFALSE);

	fLofCamacActions.SetName("CAMAC Actions");		// camac actions
	fLofCamacActions.AddNamedX(kMrbLofCamacActions);

	fLofCnafNames.SetName("CAMAC Registers");		// register names
	fLofCnafNames.AddNamedX(kMrbLofCnafNames);

	fLofCnafTypes.SetName("Cnaf Types");			// cnaf types
	fLofCnafTypes.AddNamedX(kMrbLofCnafTypes);

	fLofEsoneServers.SetName("ESONE Server");	// controllers
	fLofEsoneServers.AddNamedX(kMrbLofEsoneServers);

	fLofControllers.SetName("CAMAC Controller");	// controllers
	fLofControllers.AddNamedX(kMrbLofControllers);

	fLofCamacFunctions.SetName("CAMAC Functions");	// camac functions
	fLofControllers.AddNamedX(kMrbLofCamacFunctions);

	esoneServer = gEnv->GetValue("TMrbEsone.ServerType", "");
	if (esoneServer.Length() == 0) {
		gMrbLog->Err() << "Type of ESONE server undefined (\"TMrbEsone.ServerType\" has to be set)" << endl;
		gMrbLog->Flush(this->ClassName(), "Reset");
		return(kFALSE);
	} else if ((nx = fLofEsoneServers.FindByName(esoneServer.Data(), TMrbLofNamedX::kFindIgnoreCase)) == NULL) {
		gMrbLog->Err() << "No such ESONE server - " << esoneServer << endl;
		gMrbLog->Flush(this->ClassName(), "Reset");
		return(kFALSE);
	}
	fServerType.Set(nx);
	if (this->IsVerbose()) {
		gMrbLog->Out() << "Using ESONE server \"" << fServerType.GetName() << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "Reset", setblue);
	}

	camacContr = gEnv->GetValue("TMrbEsone.Controller", "");
	if (camacContr.Length() == 0) {
		gMrbLog->Err() << "CAMAC controller undefined (\"TMrbEsone.Controller\" has to be set)" << endl;
		gMrbLog->Flush(this->ClassName(), "Reset");
		return(kFALSE);
	} else if ((nx = fLofControllers.FindByName(camacContr.Data(), TMrbLofNamedX::kFindIgnoreCase)) == NULL) {
		gMrbLog->Err() << "No such CAMAC controller - " << camacContr << endl;
		gMrbLog->Flush(this->ClassName(), "Reset");
		return(kFALSE);
	}
	fController.Set(nx);

	if (this->GetServerIndex() == kES_MARABOU && this->GetControllerIndex() == kCC_CBV) {
		gMrbLog->Err()	<< "Illegal CAMAC controller - " << this->GetControllerName()
						<< " (ESONE server \"" << this->GetServerName()
						<< "\" implies controller \"CC32\")" << endl;
		gMrbLog->Flush(this->ClassName(), "Reset");
		return(kFALSE);
	}

	if (this->IsVerbose()) {
		gMrbLog->Out() << "Using CAMAC controller \"" << fController.GetName() << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "Reset", setblue);
	}

	fSingleStep = gEnv->GetValue("TMrbEsone.SingleStepMode", kFALSE);
	if (this->IsSingleStep()) {
		gMrbLog->Out() << "Running in SINGLE STEP mode" << endl;
		gMrbLog->Flush(this->ClassName(), "Reset", setblue);
	}

	fHost.Resize(0);								// host name
	fHostInet.Resize(0);
	fHostAddr = 0xFFFFFFFF;							// host address

	fOffline = Offline; 							// hardware access?

	fUseBroadCast = gEnv->GetValue("TMrbEsone.UseBroadCast", kTRUE); // use broadcast if applicable

	return(kTRUE);
}

Bool_t TMrbEsone::CheckConnection(const Char_t * Method) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::CheckConnection
// Purpose:        Check if connected to camac host
// Arguments:      Char_t * Method    -- calling method
// Results:        kTRUE/kFALSe
// Exceptions:
// Description:    Checks if there is a valid connection to camac.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->IsOffline() || fHostAddr > 0) return(kTRUE);

	gMrbLog->Err() << "No CAMAC host connected" << endl;
	gMrbLog->Flush(this->ClassName(), Method);
	return(kFALSE);
}

Bool_t TMrbEsone::CheckCrate(Int_t Crate, const Char_t * Method) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::CheckCrate
// Purpose:        Check if crate number is legal
// Arguments:      Int_t Crate        -- crate number
//                 Char_t * Method    -- calling method
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks if crate number is ok.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Crate <= 0 || Crate > kEsoneNofCrates) {
		gMrbLog->Err() << "Illegal crate number - C" << Crate << " (should be in [1," << kEsoneNofCrates << "])" << endl;
		gMrbLog->Flush(this->ClassName(), Method);
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TMrbEsone::StartServer(const Char_t * HostName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::StartServer
// Purpose:        Try to start esone server on host
// Arguments:      Char_t * HostName    -- host name
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Start server depending on type.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

//	if (!this->CheckConnection("StartServer")) return(kFALSE);

	switch (this->GetServerIndex()) {
		case kES_MBS:
			return(this->StartMbsServer(HostName));
		case kES_MARABOU:
			return(this->StartMarabouServer(HostName));
	}
	return(kFALSE);
}

Bool_t TMrbEsone::StartMbsServer(const Char_t * HostName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::StartMbsServer
// Purpose:        Try to start esone server on host
// Arguments:      Char_t * HostName    -- host name
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Tries to start mbs.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	FILE * pCmd;
	Char_t line[100];
	TString pLine;
	Bool_t found;
	TString remoteHome;
	TString prmOrDsp;

	this->SetOffline(kFALSE);

	fSetupPath = gEnv->GetValue("TMrbEsone.SetupPath", "/nfs/mbssys/esone");

	TString lynxVersion = gEnv->GetValue("TMrbEsone.LynxVersion", "");
	if (lynxVersion.Length() == 0) lynxVersion = gEnv->GetValue("TMbsSetup.LynxVersion", "2.5");

	TString mbsVersion = gEnv->GetValue("TMrbEsone.MbsVersion", "");
	if (mbsVersion.Length() == 0) mbsVersion = gEnv->GetValue("TMbsSetup.MbsVersion", "v22");

	Bool_t startPrompter = gEnv->GetValue("TMrbEsone.StartPrompter", kFALSE);

	// reset mbs
	gMrbLog->Out()	<< "Resetting ESONE server (type "
					<< fServerType.GetName() << ", controller "
					<< fController.GetName() << ") on host \"" << HostName << "\"." << endl;
	gMrbLog->Flush(this->ClassName(), "StartMbsServer", setmagenta);

	TString rCmd = "rsh ";
	rCmd += HostName;
	rCmd += " -l ";
	rCmd += gSystem->Getenv("USER");
	rCmd += " \"/mbs/";
	rCmd += mbsVersion;
	rCmd += "/script/remote_exe_p.sc /mbs/";
	rCmd += mbsVersion;
	rCmd += " ";
	rCmd += fSetupPath;
	rCmd += " m_remote reset -l";
	rCmd += " 2>>&/dev/null ";
	if (!this->IsVerbose()) rCmd += " >>&/dev/null";
	rCmd += "\"";
	if (this->IsVerbose()) {
		gMrbLog->Out()	<< "Exec >> " << rCmd.Data() << " <<" << endl;
		gMrbLog->Flush(this->ClassName(), "StartMbsServer", setmagenta);
	}
	gSystem->Exec(rCmd.Data());

	rCmd = "rsh ";
	rCmd += HostName;
	rCmd += " -l ";
	rCmd += gSystem->Getenv("USER");
	rCmd += " \"ps ax | fgrep m_\"";
	if (this->IsVerbose()) {
		gMrbLog->Out()	<< "Exec >> " << rCmd.Data() << " <<" << endl;
		gMrbLog->Flush(this->ClassName(), "StartMbsServer", setmagenta);
	}
	gSystem->Exec(rCmd.Data());

	cout	<< setmagenta << "Please wait " << setblack << ends << flush;
	fAborted = kFALSE;
	for (i = 0; i < 20 ; i++) {
		gSystem->ProcessEvents();
		if (this->IsAborted()) {
			cout << setred << " aborted." << setblack << endl;
			return(kFALSE);
		}
		sleep(1);
		cout << setmagenta << "." << setblack << ends << flush;
		found = kFALSE;
		pCmd = gSystem->OpenPipe(rCmd.Data(), "r");
		while (fgets(line, 100, pCmd) != NULL) {
			pLine = line;
			if (this->IsVerbose()) cout << "--- " << line << endl;
			if (pLine.Index("m_") == -1) break;
			found = kTRUE;
		}
		gSystem->ClosePipe(pCmd);
		if (!found) {
			cout << setmagenta << " done." << setblack << endl;
			break;
		}
	}

	if (found) {
		gMrbLog->Err() << "Can't RESET server on host \"" << HostName << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "StartMbsServer");
		return(kFALSE);
	}

// restart mbs
	gMrbLog->Out()	<< "Restarting ESONE server (type "
					<< fServerType.GetName() << ", controller "
					<< fController.GetName() << ") on host \"" << HostName << "\"." << endl;
	gMrbLog->Flush(this->ClassName(), "StartMbsServer", setmagenta);

	rCmd = "rsh ";
	rCmd += HostName;
	rCmd += " -l ";
	rCmd += gSystem->Getenv("USER");
	rCmd += " \"cp ";
	rCmd += fSetupPath;
	rCmd += "/.tcshrc .";
	if (!this->IsVerbose()) rCmd += " >>/dev/null";
	rCmd += "\"";
	if (this->IsVerbose()) {
		gMrbLog->Out()	<< "Exec >> " << rCmd.Data() << " <<" << endl;
		gMrbLog->Flush(this->ClassName(), "StartMbsServer", setmagenta);
	}
	gSystem->Exec(rCmd.Data());

	TString ppcHomeDir = gEnv->GetValue("TMbsEsone.PPCHomeDir", "");
	if (ppcHomeDir.IsNull()) ppcHomeDir = gEnv->GetValue("TMbsSetup.PPCHomeDir", "");
	if (ppcHomeDir.IsNull()) {
		gMrbLog->Err() << "No home dir given (check \"TMbsSetup.PPCHomeDir\" in .rootrc)" << endl;
		gMrbLog->Flush(this->ClassName(), "StartMbsServer");
		return(kFALSE);
	}
	ofstream login;
	ppcHomeDir += "/.login";
	login.open(ppcHomeDir.Data(), ios::out);
	login << "setenv LYNX_VERSION " << lynxVersion << endl;
	login << "setenv MBS_VERSION " << mbsVersion << endl;
	TString mpath = gSystem->Getenv("MARABOU");
	login << "setenv MARABOU_PATH " << mpath << endl;
	login << "set path = ( /bin /bin/ces /usr/bin /usr/local/bin /etc /usr/etc . ~/tools " << mpath << "/powerpc/bin/" << lynxVersion << " )" << endl;
	login << "source /mbs/login " << mbsVersion << endl;
	login.close();
	if (this->IsVerbose()) {
		gMrbLog->Out()	<< "Creating " << ppcHomeDir << endl;
		gMrbLog->Flush(this->ClassName(), "StartMbsServer", setmagenta);
	}
	gSystem->Exec(rCmd.Data());

	prmOrDsp = startPrompter ? "m_prompt" : "m_dispatch";

	rCmd = "rsh ";
	rCmd += HostName;
	rCmd += " -l ";
	rCmd += gSystem->Getenv("USER");
	rCmd += " \"/mbs/";
	rCmd += mbsVersion;
	rCmd += "/script/remote_exe_p.sc /mbs/";
	rCmd += mbsVersion;
	rCmd += " . ";
	rCmd += prmOrDsp;
	rCmd += " @";
	rCmd += fSetupPath;
	rCmd += "/";
	rCmd += fController.GetName();
	rCmd += "/startup_";
	rCmd += mbsVersion;
	rCmd += " 2>>&/dev/null ";
	if (!this->IsVerbose()) rCmd += " >>&/dev/null";
	rCmd += "\"";
	if (this->IsVerbose()) {
		gMrbLog->Out()	<< "Exec >> " << rCmd.Data() << " <<" << endl;
		gMrbLog->Flush(this->ClassName(), "StartMbsServer", setmagenta);
	}
	gSystem->Exec(rCmd.Data());

	rCmd = "rsh ";
	rCmd += HostName;
	rCmd += " -l ";
	rCmd += gSystem->Getenv("USER");
	rCmd += " \"ps ax | fgrep m_\"";
	if (this->IsVerbose()) {
		gMrbLog->Out()	<< "Exec >> " << rCmd.Data() << " <<" << endl;
		gMrbLog->Flush(this->ClassName(), "StartMbsServer", setmagenta);
	}
	gSystem->Exec(rCmd.Data());
	cout	<< setmagenta << "Please wait " << setblack << ends << flush;
	fAborted = kFALSE;
	for (i = 0; i < 30 ; i++) {
		gSystem->ProcessEvents();
		if (this->IsAborted()) {
			cout << setred << " aborted." << setblack << endl;
			return(kFALSE);
		}
		sleep(1);
		cout << setmagenta << "." << setblack << ends << flush;
		found = kFALSE;
		pCmd = gSystem->OpenPipe(rCmd.Data(), "r");
		while (fgets(line, 100, pCmd) != NULL) {
			pLine = line;
			if (this->IsVerbose()) cout << "--- " << line << endl;
			if (pLine.Index("m_") == -1) break;
			if (pLine.Index("m_esone") != -1) {
				found = kTRUE;
				break;
			}
		}
		gSystem->ClosePipe(pCmd);
		if (found) {
			cout << setmagenta << " done." << setblack << endl;
			break;
		}
	}

	if (found) {
		gMrbLog->Out()	<< "ESONE server (type " << fServerType.GetName()
						<< ", controller " << fController.GetName() << ") running on host \""
						<< HostName << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "StartMbsServer", setblue);
		return(kTRUE);
	} else {
		gMrbLog->Err()	<< "Can't start server (type " << fServerType.GetName()
						<< ", controller " << fController.GetName() << ") on host \""
						<< HostName << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "StartMbsServer");
		return(kFALSE);
	}
}

Bool_t TMrbEsone::StartMarabouServer(const Char_t * HostName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::StartMarabouServer
// Purpose:        Connect to MARaBOU server
// Arguments:      Char_t * HostName      -- server addr
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Establishes a server connection.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	this->SetOffline(kFALSE);

	fHost = HostName;
	fPort = gEnv->GetValue("TMrbEsone.ServerPort", 9010);

	fBufferSize = gEnv->GetValue("TMrbEsone.BufferSize", 0x8000);
	fNofCrates = gEnv->GetValue("TMrbEsone.NofCrates", 1);
	fCC32Base = gEnv->GetValue("TMrbEsone.CC32Base", 0x550000);

	fSocket = NULL;

	fServerPath = gEnv->GetValue("TMrbEsone.ServerPath", "");
	fServerProg = gSystem->BaseName(fServerPath.Data());

	// restart server
	gMrbLog->Out()	<< "Starting ESONE server (type "
					<< fServerType.GetName() << ", controller "
					<< fController.GetName() << ") on host \"" << HostName << "\"." << endl;
	gMrbLog->Flush(this->ClassName(), "StartMarabouServer", setmagenta);

	Bool_t useXterm = gEnv->GetValue("TMrbEsone.StartSrvInXterm", kTRUE);
	Bool_t nonBlocking = gEnv->GetValue("TMrbEsone.NonBlockingMode", kFALSE);

	fLynxClient = new TMrbC2Lynx(fHost.Data(), fServerPath.Data(), NULL, fPort);
	fLynxClient->SetNonBlocking(nonBlocking);
	fLynxClient->SetServerLog(useXterm ? TMrbC2Lynx::kC2LServerLogXterm : TMrbC2Lynx::kC2LServerLogCout);

	if ((fSocket = fLynxClient->GetSocket()) == NULL) {
		gMrbLog->Err()	<< "Can't connect to server/port " << fHost << ":" << fPort << endl;
		gMrbLog->Flush(this->ClassName(), "StartMarabouServer");
		return(kFALSE);
	}

	return(kTRUE);
}

UInt_t TMrbEsone::ConnectToHost(const Char_t * HostName, Bool_t Reconnect) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::ConnectToHost
// Purpose:        Establish connection to camac host
// Arguments:      Char_t * HostName  -- host name
//                 Bool_t Reconnect   -- kTRUE -> try to reconnect
// Results:        UInt_t             -- host address, 0 if error
// Exceptions:
// Description:    ESONE ccopen.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * host;
	UInt_t hostAddr;
	TInetAddress * ia;
	TString iAddr;

	if (gMrbEsoneHosts == NULL) {				// list of active camac hosts
		gMrbEsoneHosts = new TMrbLofNamedX("List of ESONE hosts");
	}

	if (Reconnect) fHost.Resize(0);

	if (!fHost.IsNull()) {						// may connect only once
		gMrbLog->Err() << "Already connected to host \"" << fHost << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "ConnectToHost");
		return(0);
	}

	ia = new TInetAddress(gSystem->GetHostByName(HostName));	// try to get full inet address
	iAddr = ia->GetHostName();
	delete ia;

	if (iAddr.CompareTo("UnknownHost") == 0) {					// hostname illegal
		gMrbLog->Err() << "Can't resolve host name - " << HostName << endl;
		gMrbLog->Flush(this->ClassName(), "ConnectToHost");
		return(0);
	}

	host = gMrbEsoneHosts->FindByName(HostName); 				// host already in internal host table?
	if (Reconnect || host == NULL) {
		if (!this->EsoneCCOPEN(HostName, hostAddr)) {			// no: try to connect via ESONE
			gMrbLog->Err() << "Can't connect to " << HostName << endl;
			this->PrintError("ccopen", "ConnectToHost");
			fHost.Resize(0);
			fHostInet.Resize(0);
			fHostAddr = 0xffffffff;
			return(0);
		}
		if (host == NULL) {
			host = new TMrbNamedX(hostAddr, HostName);  	// success: insert in internal table for further use
			gMrbEsoneHosts->AddNamedX(host);
		}
	}
	fHost = host->GetName();
	fHostAddr = host->GetIndex();
	return(fHostAddr);										// result: ESONE-style host address
}

Bool_t TMrbEsone::ClearDW(Int_t Crate) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::ClearDW
// Purpose:        Clear dataway
// Arguments:      Int_t Crate      -- crate number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    ESONE cccc (CAMAC Clear)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnection("ClearDW")) return(kFALSE);
	if (!this->CheckCrate(Crate, "ClearDW")) return(kFALSE);

	if (!this->EsoneCCCC(Crate)) {						// exec CAMAC-C
		gMrbLog->Err() << "Dataway clear failed for crate C" << Crate <<" -" << endl;
		this->PrintError("cccc", "ClearDW");
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TMrbEsone::SetDWInhibit(Int_t Crate, Bool_t Inhibit) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::SetDWInhibit
// Purpose:        Set/Clear dataway inhibit
// Arguments:      Int_t Crate      -- crate number
//                 Bool_t Inhibit   -- inhibit flag
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    ESONE ccci (CAMAC Inhibit)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnection("SetDWInhibit")) return(kFALSE);
	if (!this->CheckCrate(Crate, "SetDWInhibit")) return(kFALSE);

	if (!this->EsoneCCCI(Crate, Inhibit) == 0) {			// exec CAMAC-I
		gMrbLog->Err() << "Dataway inhibit failed for crate C" << Crate << " -" << endl;
		this->PrintError("ccci", "SetDWInhibit");
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TMrbEsone::DWIsInhibited(Int_t Crate) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::DWInhIsActive
// Purpose:        Test dataway inhibit
// Arguments:      Int_t Crate        -- crate number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Test the dataway inhibit flag (ESONE ctci).
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnection("DWInhibit")) return(kFALSE);
	if (!this->CheckCrate(Crate, "DWInhibit")) return(kFALSE);
	return(this->EsoneCTCI(Crate));
}

Bool_t TMrbEsone::InitDW(Int_t Crate) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::InitDW
// Purpose:        Initialize dataway
// Arguments:      Int_t Crate     -- crate number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    ESONE cccz (CAMAC Z)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckConnection("InitDW")) return(kFALSE);
	if (!this->CheckCrate(Crate, "InitDW")) return(kFALSE);

	if (!this->EsoneCCCZ(Crate)) { 			// exec CAMAC-Z
		gMrbLog->Err() << "Dataway initialize failed for crate C" << Crate << " -" << endl;
		this->PrintError("cccz", "InitDW");
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TMrbEsone::ExecCnaf(const Char_t * Cnaf, Bool_t D16Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::ExecCnaf
// Purpose:        Exec a single control CNAF
// Arguments:      Char_t * Cnaf     -- cnaf (ascii representation)
//                 Bool_t D16Flag    -- kTRUE if 16 bits only
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    ESONE cfsa / cssa. Control cnafs only.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbEsoneCnaf cnaf;

	if (!cnaf.Ascii2Int(Cnaf)) return(kFALSE);			// decode CNAF
	if (cnaf.IsRead()) cnaf.SetData(kEsoneNoData);
	return(this->ExecCnaf(cnaf, D16Flag));
}

Bool_t TMrbEsone::ExecCnaf(const Char_t * Cnaf, Int_t & Data, Bool_t D16Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::ExecCnaf
// Purpose:        Exec a single CNAF
// Arguments:      Char_t * Cnaf     -- cnaf (ascii representation)
//                 Int_t & Data      -- data
//                 Bool_t D16Flag    -- kTRUE if 16 bits only
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    ESONE cfsa / cssa.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbEsoneCnaf cnaf;

	if (!cnaf.Ascii2Int(Cnaf)) return(kFALSE);			// decode CNAF
	if (cnaf.IsWrite()) cnaf.SetData(Data);
	if (!this->ExecCnaf(cnaf, D16Flag)) return(kFALSE);
	if (cnaf.IsRead()) Data = cnaf.GetData();
	return(kTRUE);
}

Bool_t TMrbEsone::ExecCnaf(Int_t Crate, Int_t Station, Int_t Subaddr, Int_t Function, Bool_t D16Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::ExecCnaf
// Purpose:        Exec a single control CNAF
// Arguments:      Int_t Crate       -- crate number
//                 Int_t Station     -- station
//                 Int_t Subaddr     -- subaddress
//                 Int_t Function    -- function
//                 Bool_t D16Flag    -- kTRUE if 16 bits only
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    ESONE cfsa / cssa. Control cnafs only.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbEsoneCnaf cnaf;
	cnaf.Set(Crate, Station, Subaddr, Function);
	if (!cnaf.IsControl()) {
		gMrbLog->Err()	<< "[" << cnaf.Int2Ascii() << "] Not a CONTROL cnaf" << endl;
		gMrbLog->Flush(this->ClassName(), "ExecCnaf");
		return(kFALSE);
	}
	cnaf.ClearData();
	return(this->ExecCnaf(cnaf, D16Flag));
}

Bool_t TMrbEsone::ExecCnaf(Int_t Crate, Int_t Station, Int_t Subaddr, Int_t Function, Int_t & Data, Bool_t D16Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::ExecCnaf
// Purpose:        Exec a single CNAF
// Arguments:      Int_t Crate       -- crate number
//                 Int_t Station     -- station
//                 Int_t Subaddr     -- subaddress
//                 Int_t Function    -- function
//                 Int_t & Data      -- data
//                 Bool_t D16Flag    -- kTRUE if 16 bits only
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    ESONE cfsa / cssa.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbEsoneCnaf cnaf;
	Int_t data;

	data = IS_F_READ(Function) ? kEsoneNoData : Data;
	cnaf.Set(Crate, Station, Subaddr, Function, data);
	if (!this->ExecCnaf(cnaf, D16Flag)) return(kFALSE);
	if (cnaf.IsRead()) Data = cnaf.GetData();
	return(kTRUE);
}

Bool_t TMrbEsone::ExecCnaf(TMrbEsoneCnaf & Cnaf, Bool_t D16Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::ExecCnaf
// Purpose:        Exec a single CNAF
// Arguments:      TMrbEsoneCnaf & Cnaf     -- cnaf
//                 Bool_t D16Flag           -- kTRUE if 16 bits only
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    ESONE cfsa / cssa.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t ok;

	if (!this->CheckConnection("ExecCnaf")) return(kFALSE);
	if (!Cnaf.CheckCnaf()) return(kFALSE);		// check for completeness & consistency

	ok = this->EsoneCXSA(Cnaf, D16Flag);  		// exec single cnaf, 16 or 24 bit

	if (this->IsVerbose()) this->PrintResults("ExecCnaf", Cnaf);

	return(ok);
}

Int_t TMrbEsone::ExecCnafList(TObjArray & CnafList, Bool_t D16Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::ExecCnafList
// Purpose:        Exec a list of CNAFs
// Arguments:      TObjArray & CnafList    -- cnafs to be executed
//                 Bool_t D16Flag          -- kTRUE if 16 bits only
// Results:        Int_t NofCnafs          -- number of cnafs executed
// Exceptions:     Returns kEsoneError on error.
// Description:    ESONE cfga / csga.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t nofCnafs;

	if (!this->CheckConnection("ExecCnafList")) return(kEsoneError);

	Bool_t ok = kTRUE;
	for (Int_t i = 0; i <= CnafList.GetLast(); i++) {
		if (!((TMrbEsoneCnaf *) CnafList[i])->CheckCnaf()) ok = kFALSE;		// check for completeness & consistency
	}
	if (!ok) return(kEsoneError);

	nofCnafs = this->EsoneCXGA(CnafList, D16Flag);
	if (this->IsVerbose()) this->PrintResults("ExecCnafList", CnafList);
	return(nofCnafs);
}


Int_t TMrbEsone::AddressScan(const Char_t * Start, const Char_t * Stop, TArrayI & Data, TObjArray & Results, Bool_t D16Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::AddressScan
// Purpose:        Exec cnafs between N(start) and N(Stop)
// Arguments:      Char_t * Start            -- C.N.F to start with
//                 Char_t * Stop             -- C.N.F to stop with
//                 TArrayI & Data            -- data (read/write)
//                 TObjArray & Results       -- where to store resulting data
//                 Bool_t D16Flag            -- kTRUE if 16 bits only
// Results:        Int_t NofData             -- resulting number of data words
// Exceptions:     Returns kEsoneError on error.
// Description:    ESONE cfmad / csmad.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbEsoneCnaf startCnaf, stopCnaf;

	if (!this->CheckConnection("AddressScan")) return(kEsoneError);

	if (!startCnaf.Ascii2Int(Start)) return(kEsoneError);
	if (!stopCnaf.Ascii2Int(Stop)) return(kEsoneError);

	return(this->AddressScan(startCnaf, stopCnaf, Data, Results, D16Flag));
}

Int_t TMrbEsone::AddressScan(Int_t Crate, Int_t Start, Int_t Stop, Int_t Function, TArrayI & Data, TObjArray & Results, Bool_t D16Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::AddressScan
// Purpose:        Exec cnafs between N(start) and N(Stop)
// Arguments:      Int_t Crate               -- crate number
//                 Int_t Start               -- station to start with
//                 Int_t Stop                -- station to stop with
//                 Int_t Function            -- function code
//                 TArrayI & Data            -- data (read/write)
//                 TObjArray & Results       -- where to store resulting data
//                 Bool_t D16Flag            -- kTRUE if 16 bits only
// Results:        Int_t NofData             -- resulting number of data words
// Exceptions:     Returns kEsoneError on error.
// Description:    ESONE cfmad / csmad.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbEsoneCnaf startCnaf, stopCnaf;

	if (!this->CheckConnection("AddressScan")) return(kEsoneError);
	if (!this->CheckCrate(Crate, "AddressScan")) return(kEsoneError);

	startCnaf.Set(Crate, Start, 0, Function);
	stopCnaf.Set(Crate, Stop, 0, Function);

	return(this->AddressScan(startCnaf, stopCnaf, Data, Results, D16Flag));
}

Int_t TMrbEsone::AddressScan(TMrbEsoneCnaf & Start, TMrbEsoneCnaf & Stop, TArrayI & Data, TObjArray & Results, Bool_t D16Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::AddressScan
// Purpose:        Exec cnafs between N(start) and N(Stop)
// Arguments:      TMrbEsoneCnaf & Start     -- C.N.F to start with
//                 TMrbEsoneCnaf & Stop      -- C.N.F to stop with
//                 TArrayI & Data            -- data (read/write)
//                 TObjArray & Results       -- where to store resulting data
//                 Bool_t D16Flag            -- kTRUE if 16 bits only
// Results:        Int_t NofData             -- resulting number of data words
// Exceptions:     Returns kEsoneError on error.
// Description:    ESONE cfmad / csmad.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t crate = Start.GetC();
	if (!this->CheckCrate(crate, "AddressScan")) return(kEsoneError);
	if (Stop.GetC() == -1) Stop.SetC(crate);

	if (Stop.GetC() != crate) { 	// crate must be same
		gMrbLog->Err()	<< "[" << Start.Int2Ascii() << " : " << Stop.Int2Ascii()
						<< "] Crates different - start=C" << crate
						<< " != stop=C" << Stop.GetC() << endl;
		gMrbLog->Flush(this->ClassName(), "AdressScan");
		return(kEsoneError);
	}

	Int_t station = Start.GetN();
	if (Stop.GetN() < station) { 	// stations must be in order
		gMrbLog->Err()	<< "[" << Start.Int2Ascii() << " : " << Stop.Int2Ascii()
						<< "] Station out of order - start=N" << station
						<< " > stop=N" << Stop.GetN() << endl;
		gMrbLog->Flush(this->ClassName(), "AdressScan");
		return(kEsoneError);
	}

	if (Start.GetA() != -1) { 	// no subaddr allowed
		gMrbLog->Err() << "[" << Start.Int2Ascii() << "] Subaddr may not be given" << endl;
		gMrbLog->Flush(this->ClassName(), "AdressScan");
		return(kEsoneError);
	}

	if (Stop.GetA() != -1) { 	// no subaddr allowed
		gMrbLog->Err() << "[" << Stop.Int2Ascii() << "] Subaddr may not be given" << endl;
		gMrbLog->Flush(this->ClassName(), "AdressScan");
		return(kEsoneError);
	}

	Int_t function = Start.GetF();
	if (Stop.GetF() == -1) Stop.SetF(function);

	if (Stop.GetF() != function) { 	// must be same function
		gMrbLog->Err()	<< "[" << Start.Int2Ascii() << " : " << Stop.Int2Ascii()
						<< "] Function codes different - start=F" << function
						<< " != stop=F" << Stop.GetF() << endl;
		gMrbLog->Flush(this->ClassName(), "AdressScan");
		return(kEsoneError);
	}

	Int_t nofData = this->EsoneCXMAD(Start, Stop, Data, Results, D16Flag);
	if (this->IsVerbose()) this->PrintResults("AddressScan", Results);
	return(nofData);
}

Int_t TMrbEsone::BlockXfer(const Char_t * Cnaf, TArrayI & Data, Int_t Start, Int_t NofWords, Bool_t D16Flag, Bool_t QXfer) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::BlockXfer
// Purpose:        Start block transfer
// Arguments:      Char_t * Cnaf            -- C.N.F to be executed
//                 TArrayI & Data           -- data
//                 Int_t Start              -- data index to start with
//                 Int_t NofWords           -- number of words to xfer
//                 Bool_t D16Flag           -- kTRUE if 16 bits only
//                 Bool_t QXfer             -- kTRUE if Q-driven
// Results:        Int_t NofData            -- resulting number of data words
// Exceptions:     Returns kEsoneError on error.
// Description:    ESONE cfubc / csubc / cfubr / csubr.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbEsoneCnaf cnaf;

	if (!this->CheckConnection("BlockXfer")) return(kEsoneError);
	if (!cnaf.Ascii2Int(Cnaf)) return(kEsoneError);		// test if cnaf if legal
	return(this->BlockXfer(cnaf, Data, Start, NofWords, D16Flag, QXfer));
}

Int_t TMrbEsone::BlockXfer(Int_t Crate, Int_t Station, Int_t SubAddr, Int_t Function,
										TArrayI & Data, Int_t Start, Int_t NofWords, Bool_t D16Flag, Bool_t QXfer) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::BlockXfer
// Purpose:        Start block transfer
// Arguments:      Int_t Crate              -- crate number
//                 Int_t Station            -- station
//                 Int_t SubAddr            -- subaddress
//                 Int_t Function           -- function code
//                 TArrayI & Data           -- data
//                 Int_t Start              -- data index to start with
//                 Int_t NofWords           -- number of words to xfer
//                 Bool_t D16Flag           -- kTRUE if 16 bits only
//                 Bool_t QXfer             -- kTRUE if Q-driven
// Results:        Int_t NofData            -- resulting number of data words
// Exceptions:     Returns kEsoneError on error.
// Description:    ESONE cfubc / csubc / cfubr / csubr.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbEsoneCnaf cnaf;

	if (!this->CheckConnection("BlockXfer")) return(kEsoneError);
	cnaf.Set(Crate, Station, SubAddr, Function);
	return(this->BlockXfer(cnaf, Data, Start, NofWords, D16Flag, QXfer));
}

Int_t TMrbEsone::BlockXfer(TMrbEsoneCnaf & Cnaf, TArrayI & Data, Int_t Start, Int_t NofWords,
																		Bool_t D16Flag, Bool_t QXfer) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::BlockXfer
// Purpose:        Start block transfer
// Arguments:      TMrbEsoneCnaf & Cnaf     -- C.N.F to be executed
//                 TArrayI & Data           -- data
//                 Int_t Start              -- data index to start with
//                 Int_t NofWords           -- number of words to xfer
//                 Bool_t D16Flag           -- kTRUE if 16 bits only
//                 Bool_t QXfer             -- kTRUE if Q-driven
// Results:        Int_t NofData            -- resulting number of data words
// Exceptions:     Returns kEsoneError on error.
// Description:    ESONE cfubc / csubc / cfubr / csubr.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t repeat;
	Int_t nofData;
	Int_t nd;
	Int_t from, wc;

	if (!this->CheckConnection("BlockXfer")) return(kEsoneError);
	if (!Cnaf.CheckCnaf(~TMrbEsoneCnaf::kCnafData)) return(kEsoneError);	// check cnaf (all but data field)

	nofData = 0;
	repeat = (NofWords + kEsoneNofRepeats) / kEsoneNofRepeats;
	from = Start;
	for (; repeat--; from += kEsoneNofRepeats, NofWords -= kEsoneNofRepeats) {
		wc = (NofWords > kEsoneNofRepeats) ? kEsoneNofRepeats : NofWords;
		nd = this->EsoneCXUBX(Cnaf, Data, from, wc, D16Flag, QXfer);
		if (this->IsVerbose()) this->PrintResults("BlockXfer", Cnaf);
		if (this->IsError()) return(kEsoneError);
		nofData += nd;
	}
	return(nofData);
}

UInt_t TMrbEsone::GetStatus(Bool_t & XFlag, Bool_t & QFlag, Int_t & ErrorCode, TString & Error) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::GetStatus
// Purpose:        Get ESONE status of prev camac action
// Arguments:      Bool_t & XFlag         -- CAMAC X
//                 Bool_t & QFlag         -- CAMAC Q
//                 Int_t ErrorCode        -- error code
//                 TString & Error        -- error message
// Results:        CamacStatus_t Status   -- status word
// Exceptions:
// Description:    Reads and decodes the ESONE status register.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t status = this->EsoneCTSTAT();
	switch (status & 0x3) {
		case 0x0:	XFlag = kTRUE; QFlag = kTRUE; break;
		case 0x1:	XFlag = kTRUE; QFlag = kFALSE; break;
		case 0x2:	XFlag = kFALSE; QFlag = kTRUE; break;
		case 0x3:	XFlag = kFALSE; QFlag = kFALSE; break;
	}
	ErrorCode = status >> 2;
	if (ErrorCode != 0) {
		this->EsoneCERROR(Error, ErrorCode, kFALSE);
	} else {
		Error.Resize(0);
	}
	return(status);
}

TMrbNamedX * TMrbEsone::GetHost(UInt_t HostAddr) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::GetHost
// Purpose:        Get host by ESONE address
// Arguments:      UInt_t HostAddr           -- ESONE address
// Results:        TMrbNamedX * HostKey      -- host name / address
// Exceptions:
// Description:    Finds host by its address.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbEsoneHosts == NULL) return(NULL);
	return(gMrbEsoneHosts->FindByIndex((Int_t) HostAddr));
}

TMrbNamedX * TMrbEsone::GetHost(const Char_t * HostName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::GetHost
// Purpose:        Get host by name
// Arguments:      Char_t * HostName      -- host name
// Results:        TMrbNamedX * HostKey      -- host name / address
// Exceptions:
// Description:    Finds host by its name.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbEsoneHosts == NULL) return(NULL);
	return(gMrbEsoneHosts->FindByName(HostName, TMrbLofNamedX::kFindExact | TMrbLofNamedX::kFindIgnoreCase));
}

void TMrbEsone::Print(TMrbEsoneCnaf & Cnaf) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::Print
// Purpose:        Print cnafs & data
// Arguments:      TMrbEsoneCnaf & Cnaf  -- cnaf
// Results:        --
// Exceptions:
// Description:    Outputs cnaf, data, flags.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * action;
	const Char_t * xq;
	Int_t data;

	action = this->GetAction();

	if (action->GetIndex() == kCANone) {
		gMrbLog->Err() << "No CAMAC action" << endl;
		gMrbLog->Flush(this->ClassName(), "Print");
		return;
	}

	cout	<< endl
			<< "===========================================================================================" << endl
			<< " CAMAC action           : " << action->GetName() << endl
			<< "-------------------------------------------------------------------------------------------" << endl
			<< " CNAF                     Type          X? Q?  Data" << endl
			<< "..........................................................................................." << endl;

	cout	<< " " << setiosflags(ios::left)
			<< setw(25) << Cnaf.Int2Ascii(kFALSE);
	cout	<< setw(5) << fLofCnafTypes.FindByIndex((Int_t) Cnaf.GetType());
	xq = Cnaf.GetX()? "X" : "-";
	cout	<< resetiosflags(ios::left) << setw(10) << xq;
	xq = Cnaf.GetQ()? "Q" : "-";
	cout	<< setw(3) << xq;
	data = Cnaf.GetData();
	if ((data & kEsoneNoData) == 0) {
		cout	<< setw(10) << setbase(10) << data;
		cout	<< "    0" << setbase(8) << setiosflags(ios::left) << data;
		cout	<< "    0x" << setbase(16) << setiosflags(ios::left) << data;
		cout	<< setbase(10) << endl;
	}
	cout	<< "-------------------------------------------------------------------------------------------"
			<< endl << endl;
}

void TMrbEsone::Print(TObjArray & CnafList, Int_t From, Int_t To) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::Print
// Purpose:        Print cnafs & data
// Arguments:      TObjArray & CnafList  -- list of cnafs
//                 Int_t From            -- cnaf to start with
//                 Int_t To              -- cnaf to end with
// Results:        --
// Exceptions:
// Description:    Outputs cnaf, data, flags.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * action;
	const Char_t * xq;
	Int_t data;
	TMrbEsoneCnaf * cnaf;

	Int_t nofCnafs = CnafList.GetEntriesFast();
	if (nofCnafs == 0) {
		gMrbLog->Err()	<< "Cnaf list is EMPTY" << endl;
		gMrbLog->Flush(this->ClassName(), "Print");
		return;
	}

	if (To == -1) To = nofCnafs - 1;

	if (From < 0 || From > To || From > nofCnafs || To > nofCnafs) {
		gMrbLog->Err()	<< "Cnafs out of range - [" << From << "," << To << "] (should be in ["
						<< nofCnafs << "]" << endl;
		gMrbLog->Flush(this->ClassName(), "Print");
		return;
	}

	action = this->GetAction();

	if (action->GetIndex() == kCANone) {
		gMrbLog->Err() << "No CAMAC action" << endl;
		gMrbLog->Flush(this->ClassName(), "Print");
		return;
	}

	cout	<< endl
			<< "===========================================================================================" << endl
			<< " CAMAC action           : " << action->GetName() << endl
			<< "-------------------------------------------------------------------------------------------" << endl
			<< " CNAF                     Type          X? Q?  Data" << endl
			<< "..........................................................................................." << endl;

	for (Int_t i = From; i <= To; i++) {
		cnaf = (TMrbEsoneCnaf *) CnafList[i];
		cout	<< setw(3) << i << ": " << setiosflags(ios::left)
				<< setw(21) << cnaf->Int2Ascii(kFALSE);
		cout	<< setw(5) << fLofCnafTypes.FindByIndex((Int_t) cnaf->GetType());
		xq = cnaf->GetX()? "X" : "-";
		cout	<< resetiosflags(ios::left) << setw(10) << xq;
		xq = cnaf->GetQ()? "Q" : "-";
		cout	<< setw(3) << xq;
		data = cnaf->GetData();
		if ((data & kEsoneNoData) == 0) {
			cout	<< setw(10) << setbase(10) << data;
			cout	<< "    0" << setbase(8) << setiosflags(ios::left) << data;
			cout	<< "    0x" << setbase(16) << setiosflags(ios::left) << data;
			cout	<< setbase(10) << endl;
		}
	}
	cout	<< "-------------------------------------------------------------------------------------------"
			<< endl << endl;
}

const Char_t * TMrbEsone::GetError(TString & Error) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::GetError
// Purpose:        Get error of prev cnaf action
// Arguments:      TString & Error   -- where to put the error message
// Results:        Char_t * Error    -- error string
// Exceptions:
// Description:    Returns an error message.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t status = this->EsoneCTSTAT();
	Int_t errorCode = (Int_t) (status >> 2);
	this->EsoneCERROR(Error, errorCode, kFALSE);
	return(Error.Data());
}

void TMrbEsone::PrintError(const Char_t * EsoneCall, const Char_t * Method) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::PrintError
// Purpose:        Print error message
// Arguments:      Char_t * EsoneCall      -- ESONE call
//                 Char_t * Method         -- calling method
// Results:        --
// Exceptions:

// Description:    Error printout.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString error;

	if (EsoneCall != NULL) {
		this->EsoneCERROR(error, TMrbEsone::kLastError, kFALSE);
		if (!error.IsNull()) {
			gMrbLog->Err() << "Error in ESONE fct \"" << EsoneCall << "\" - " << error << endl;
			gMrbLog->Flush(this->ClassName(), Method);
		}
	} else {
		this->GetError(error);
		if (!error.IsNull()) {
			gMrbLog->Err() << "ESONE error - " << error << endl;
			gMrbLog->Flush(this->ClassName(), Method);
		}
	}
}

void TMrbEsone::PrintStatus(const Char_t * Method) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::PrintStatus
// Purpose:        Print status message
// Arguments:      Char_t * Method     -- calling method
// Results:        --
// Exceptions:
// Description:    Status.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * action;
	Bool_t xFlag;
	Bool_t qFlag;
	UInt_t status;
	Int_t errorCode;
	TString errorMsg;

	action = this->GetAction();

	if (action->GetIndex() == kCANone) {
		gMrbLog->Err() << "No CAMAC action" << endl;
		gMrbLog->Flush(this->ClassName(), "PrintStatus");
		return;
	}

	cout	<< endl << this->ClassName() << "::PrintStatus(): Current CAMAC Status -" << endl
			<< "-----------------------------------------------------------------------------" << endl
			<< "    CAMAC action           : " << action->GetName() << endl;

	status = this->GetStatus(xFlag, qFlag, errorCode, errorMsg);

	cout	<< "    X Response             : " << (xFlag ? "yes" : "no") << endl;
	cout	<< "    Q Response             : " << (qFlag ? "yes" : "no") << endl;
	cout	<< "    Error Code             : " << errorCode << endl;
	if (errorCode > 0) {
		cout	<< "    Error                  : " << errorMsg << endl;
	}
	cout	<< "-----------------------------------------------------------------------------" << endl;
}

void TMrbEsone::PrintResults(const Char_t * Method, TMrbEsoneCnaf & Cnaf) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::PrintResults
// Purpose:        Print results
// Arguments:      Char_t * Method            -- calling method
//                 TMrbEsoneCnaf & Cnaf       -- cnaf
// Results:        --
// Exceptions:
// Description:    Outputs cnaf results.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * action;
	TMrbNamedX * type;

	action = this->GetAction();

	cout	<< setmagenta
			<< this->ClassName() << "::" << Method << "(): " << action->GetName()
			<< setblack << endl;

	type = fLofCnafTypes.FindByIndex((Int_t) Cnaf.GetType());
	cout	<< setmagenta
			<< "                      CNAF=" << Cnaf.Int2Ascii() << " (" << type->GetName() << ")"
			<< ", X=" << (Cnaf.GetX() ? 1 : 0)
			<< ", Q=" << (Cnaf.GetQ() ? 1 : 0)
			<< ", Data=" << Cnaf.GetData() << " (0x" << setbase(16) << Cnaf.GetData() << setbase(10) << ")"
			<< setblack << endl;
}

void TMrbEsone::PrintResults(const Char_t * Method, TObjArray & CnafList) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::PrintResults
// Purpose:        Print results
// Arguments:      Char_t * Method            -- calling method
//                 TObjArray & CnafList       -- list of cnafs
// Results:        --
// Exceptions:
// Description:    Outputs cnaf results.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * action;
	TMrbNamedX * type;
	TMrbEsoneCnaf * cnaf;

	action = this->GetAction();

	cout	<< setmagenta
			<< this->ClassName() << "::" << Method << "(): " << action->GetName()
			<< setblack << endl;

	for (Int_t i = 0; i < CnafList.GetLast() + 1; i++) {
		cnaf = (TMrbEsoneCnaf *) CnafList.At(i);
		type = fLofCnafTypes.FindByIndex((Int_t) cnaf->GetType());
		cout	<< setmagenta
				<< "                      CNAF(" << i << ")=" << cnaf->Int2Ascii() << " (" << type->GetName() << ")"
				<< ", X=" << (cnaf->GetX() ? 1 : 0)
				<< ", Q=" << (cnaf->GetQ() ? 1 : 0)
				<< ", Data=" << cnaf->GetData() << " (0x" << setbase(16) << cnaf->GetData() << setbase(10) << ")"
				<< setblack << endl;
	}
}

Bool_t TMrbEsone::HasFastCamac() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::HasFastCamac
// Purpose:        Test for fast camac capability
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Tests if controller has fast camac
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	switch (fController.GetIndex()) {
		case kCC_CC32:
			return(kTRUE);
		case kCC_CBV:
		default:
			return(kFALSE);
	}
}

Bool_t TMrbEsone::HasBroadCast(Int_t & NsetMask, Int_t & NexecCmd) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::HasBroadCast
// Purpose:        Test for broadcast capability
// Arguments:      Int_t & NsetMask     -- station N to set broadcast mask
//                 Int_t & NexecCmd     -- station N to exec bc commands
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Tests if controller has broadcast capability.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	NsetMask = 0;
	NexecCmd = 0;
	if (fUseBroadCast) {
		switch (fController.GetIndex()) {
			case kCC_CC32:	NsetMask = kBroadCastSetMask_N_CC32;
							NexecCmd = kBroadCastExecCmd_N_CC32;
							return(kTRUE);
			default:		return(kFALSE);
		}
	}
	return(kFALSE);
}

Bool_t TMrbEsone::HasBroadCast() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::HasBroadCast
// Purpose:        Test for broadcast capability
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Tests if controller has broadcast capability.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fUseBroadCast) {
		switch (fController.GetIndex()) {
			case kCC_CC32:	return(kTRUE);
			default:		return(kFALSE);
		}
	}
	return(kFALSE);
}

Bool_t TMrbEsone::SetBroadCast(Int_t Crate, UInt_t BroadCast) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::SetBroadCast
// Purpose:        Set broadcast register
// Arguments:      Int_t Crate          -- crate number
//                 UInt_t BroadCast     -- bits to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes the broadcast mask register.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t sts;

	if (!this->CheckConnection("SetBroadCast")) return(kFALSE);
	if (!this->CheckCrate(Crate, "SetBroadCast")) return(kFALSE);

	if (this->HasBroadCast() > 0) {
		sts = this->ExecCnaf(Crate, 26, 0, 16, (Int_t) BroadCast);
		return(sts == 1);
	} else {
		gMrbLog->Err() << "No BroadCasting for CAMAC controller " << fController.GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "SetBroadCast");
		return(kFALSE);
	}
}

UInt_t TMrbEsone::GetBroadCast(Int_t Crate) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::SetBroadCast
// Purpose:        Read broadcast register
// Arguments:      Int_t Crate          -- crate number
// Results:        UInt_t BroadCast     -- bits on bc mask
// Exceptions:     Returns kEsoneError on error.
// Description:    Reads the broadcast mask register.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t bcData;

	if (!this->CheckConnection("GetBroadCast")) return((UInt_t) kEsoneError);
	if (!this->CheckCrate(Crate, "GetBroadCast")) return((UInt_t) kEsoneError);

	if (this->HasBroadCast() > 0) {
		if (this->ExecCnaf(Crate, 26, 0, 0, bcData)) return(bcData); else return((UInt_t) kEsoneError);
	} else {
		gMrbLog->Err() << "No BroadCasting for CAMAC controller " << fController.GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "GetBroadCast");
		return((UInt_t) kEsoneError);
	}
}

Bool_t TMrbEsone::AddToBroadCast(Int_t Crate, Int_t Station) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::AddBroadCast
// Purpose:        Add station to broadcast mask
// Arguments:      Int_t Crate        -- crate number
//                 Int_t Station      -- camac station
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Adds a bit to the broadcast mask register.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t bcData;

	if (!this->CheckConnection("AddToBroadCast")) return(kFALSE);
	if (!this->CheckCrate(Crate, "AddToBroadCast")) return(kFALSE);

	if (this->HasBroadCast() > 0) {
		if (Station < 1 || Station > 24) {
			gMrbLog->Err() << "Illegal camac station - " << Station << " (should be in [1, 24])" << endl;
			gMrbLog->Flush(this->ClassName(), "AddToBroadCast");
			return(kFALSE);
		}
		if (!this->ExecCnaf(Crate, 26, 0, 0, bcData)) return(kFALSE);
		bcData |= (1 << (Station - 1));
		return(this->ExecCnaf(Crate, 26, 0, 16, bcData));
	} else {
		gMrbLog->Err() << "No BroadCasting for CAMAC controller " << fController.GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "AddToBroadCast");
		return(kFALSE);
	}
}

Bool_t TMrbEsone::RemoveFromBroadCast(Int_t Crate, Int_t Station) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::RemoveFromBroadCast
// Purpose:        Remove station from broadcast mask
// Arguments:      Int_t Crate        -- crate number
//                 Int_t Station      -- camac station
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Removes a bit to the broadcast mask register.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t bcData;

	if (!this->CheckConnection("RemoveFromBroadCast")) return(kFALSE);
	if (!this->CheckCrate(Crate, "RemoveFromBroadCast")) return(kFALSE);

	if (this->HasBroadCast() > 0) {
		if (Station < 1 || Station > 24) {
			gMrbLog->Err() << "Illegal camac station - " << Station << " (should be in [1, 24])" << endl;
			gMrbLog->Flush(this->ClassName(), "RemoveFromBroadCast");
			return(kFALSE);
		}
		if (!this->ExecCnaf(Crate, 26, 0, 0, bcData)) return(kFALSE);
		bcData &= ~(1 << (Station - 1));
		return(this->ExecCnaf(Crate, 26, 0, 16, bcData));
	} else {
		gMrbLog->Err() << "No BroadCasting for CAMAC controller " << fController.GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "RemoveFromBroadCast");
		return(kFALSE);
	}
}


Bool_t TMrbEsone::SetAutoRead(Int_t Crate, Bool_t AutoRead) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::SetAutoRead
// Purpose:        Enable/disable AutoRead mode
// Arguments:      Int_t Crate       -- crate number
//                 Bool_t AutoRead   -- on/off flag
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Enables/disable auto read mode (CC32 only).
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t sts;

	if (!this->CheckConnection("SetAutoRead")) return(kFALSE);
	if (!this->CheckCrate(Crate, "SetAutoRead")) return(kFALSE);

	switch (fController.GetIndex()) {
		case kCC_CC32:
			if (!this->EsoneSpecial(kCnafTypeRead, Crate, 0, 0, 3, sts, kTRUE)) return(kFALSE);
			if (AutoRead) sts |= kAutoRead_CC32; else sts &= ~kAutoRead_CC32;
			return(this->EsoneSpecial(kCnafTypeWrite, Crate, 0, 0, 3, sts, kTRUE));
		case kCC_CBV:
		default:
			gMrbLog->Err() << "No AutoRead mode for CAMAC controller " << fController.GetName() << endl;
			gMrbLog->Flush(this->ClassName(), "SetAutoRead");
			return(kFALSE);
	}
}

Bool_t TMrbEsone::ReadDoubleWord(Int_t Crate, Bool_t ReadDW) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::ReadDoubleWord
// Purpose:        Enable/disable 32 bit mode
// Arguments:      Int_t Crate     -- crate number
//                 Bool_t ReadDW   -- on/off flag
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Enables/disable 32 bit mode (CC32 only).
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t data0 = 0;

	if (!this->CheckConnection("ReadDoubleWord")) return(kFALSE);
	if (!this->CheckCrate(Crate, "ReadDoubleWord")) return(kFALSE);

	switch (fController.GetIndex()) {
		case kCC_CC32:
			return(this->EsoneSpecial(kCnafTypeWrite, Crate, 27, ReadDW ? 2 : 3, 16, data0, kTRUE));
		case kCC_CBV:
		default:
			gMrbLog->Err() << "No 32 bit mode for CAMAC controller " << fController.GetName() << endl;
			gMrbLog->Flush(this->ClassName(), "ReadDoubleWord");
			return(kFALSE);
	}
}

Bool_t TMrbEsone::EsoneCDREG(UInt_t & Handle, Int_t Crate, Int_t Station, Int_t SubAddr) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::EsoneCDREG
// Purpose:        Encode C, N, A
// Arguments:      Int_t & Handle     -- identifier
//                 Int_t Crate        -- crate C
//                 Int_t Station      -- station N
//                 Int_t SubAddr      -- subaddr A
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Encodes ESONE-stype camac addr from C,N,A.
//                 Returns unique identifier (handle).
//                 Esone call: cdreg()
//
//                 >>> protected method <<<
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fHostAddr <= 0) return(kFALSE);

	if (this->IsMbsServer()) {
		cdreg(&Handle, fHostAddr, Crate, Station, SubAddr);
		return(kTRUE);
	} else {						// not yet implemented
		return(kFALSE);
	}
}

Bool_t TMrbEsone::EsoneCDREG(UInt_t & Handle, TMrbEsoneCnaf & Cnaf) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::EsoneCDREG
// Purpose:        Encode C, N, A
// Arguments:      Int_t & Handle        -- identifier
//                 TMrbEsoneCnaf & Cnaf  -- CNAF specs
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Encodes ESONE-stype camac addr from C,N,A.
//                 Returns unique identifier (handle).
//                 Esone call: cdreg()
//
//                 >>> protected method <<<
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t c, n, a;

	c = Cnaf.GetC(); if (c == -1) c = 0;
	n = Cnaf.GetN(); if (n == -1) n = 0;
	a = Cnaf.GetA(); if (a == -1) a = 0;

	return(this->EsoneCDREG(Handle, c, n, a));
}

Bool_t TMrbEsone::EsoneCDCTRL() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::EsoneCDCTRL
// Purpose:        Define controller
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines type of controller to be used (CBV or CC32)
//                 Esone call: cdctrl()
//
//                 >>> protected method <<<
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->IsMbsServer()) {
		cdctrl(fController.GetIndex());
		return(kTRUE);
	} else {
		if (fController.GetIndex() != TMrbEsone::kCC_CC32) return(kFALSE);
		return(kTRUE);
	}
}

Bool_t TMrbEsone::EsoneCCCC(Int_t Crate) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::EsoneCCCC
// Purpose:        Clear dataway
// Arguments:      Int_t Crate        -- crate C
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Clears dataway for given crate.
//                 Esone call: cccc()
//
//                 >>> protected method <<<
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t sts;
	UInt_t creg;

	if (!this->EsoneCDREG(creg, Crate)) return(kFALSE);
	if (!this->EsoneCDCTRL()) return(kFALSE);

	if (this->IsMbsServer()) {
		sts = cccc(creg);
		return(sts == 0);
	} else {						// not yet implemented
		return(kFALSE);
	}
}


Bool_t TMrbEsone::EsoneCCOPEN(const Char_t * HostName, UInt_t & HostAddress) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::EsoneCCOPEN
// Purpose:        Open connection to CAMAC
// Arguments:      Char_t * HostName    -- name of camac host
//                 UInt_t HostAddress   -- ... address
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Connects to camac.
//                 Esone call: ccopen()
//
//                 >>> protected method <<<
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t sts;

	if (this->IsOffline()) return(kTRUE);		// nothing to do if in offline mode

	if (this->IsMbsServer()) {
		if (this->IsSingleStep()) {
			cout << "[TMrbEsone::EsoneCCOPEN | Connect to CAMAC] Host=" << HostName << " ... "; getchar();
			sts = ccopen((Char_t *) HostName, &HostAddress);
			cout << "[TMrbEsone::EsoneCCOPEN] Status=" << sts << endl;
		} else {
			sts = ccopen((Char_t *) HostName, &HostAddress);
		}
		return(sts == 0);
	} else {						// not yet implemented
		return(kFALSE);
	}
}

Bool_t TMrbEsone::EsoneCCCI(Int_t Crate, Bool_t Inhibit) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::EsoneCCCI
// Purpose:        Set/clear dataway inhibit
// Arguments:      Int_t Crate        -- crate C
//                 Bool_t Inhibit     -- inhibit flag
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Inhibit/enable camac dataway
//                 Esone call: ccci()
//
//                 >>> protected method <<<
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t sts;
	UInt_t creg;

	if (this->IsOffline()) return(kTRUE);

	if (!this->EsoneCDREG(creg, Crate)) return(kFALSE);
	if (!this->EsoneCDCTRL()) return(kFALSE);

	if (this->IsMbsServer()) {
		if (this->IsSingleStep()) {
			cout << "[TMrbEsone::EsoneCCCI | Inhibit dataway] Crate=" << Crate << " Inhibit=" << (Inhibit ? "TRUE" : "FALSE") << " ... "; getchar();
			sts = ccci(creg, Inhibit ? kCF_ENB : kCF_DIS);
			cout << "[TMrbEsone::EsoneCCCI] Status=" << sts << endl;
		} else {
			sts = ccci(creg, Inhibit ? kCF_ENB : kCF_DIS);
		}
		return(sts == 0);
	} else {						// not yet implemented
		return(kFALSE);
	}
}

Bool_t TMrbEsone::EsoneCTCI(Int_t Crate) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::EsoneCTCI
// Purpose:        Test dataway inhibit
// Arguments:      Int_t Crate        -- crate C
// Results:        Bool_t Inhibit     -- inhibit flag
// Exceptions:
// Description:    Tests dataway inhibit
//                 Esone call: ctci()
//
//                 >>> protected method <<<
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t creg;
	Int_t dwinhFlag;

	if (this->IsOffline()) return(kTRUE);

	if (!this->EsoneCDREG(creg, Crate)) return(kFALSE);
	if (!this->EsoneCDCTRL()) return(kFALSE);

	if (this->IsMbsServer()) {
		if (this->IsSingleStep()) {
			cout << "[TMrbEsone::EsoneCTCI | Test dataway inhibit] Crate=" << Crate << " ... "; getchar();
			ctci(creg, &dwinhFlag);
			cout << "[TMrbEsone::EsoneCTCI] Inhibit=" << ((dwinhFlag == 1) ? "TRUE" : "FALSE") << endl;
		} else {
			ctci(creg, &dwinhFlag);
		}
		return(dwinhFlag == 1);
	} else {						// not yet implemented
		return(kFALSE);
	}
}

Bool_t TMrbEsone::EsoneCCCZ(Int_t Crate) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::EsoneCCCZ
// Purpose:        Initialize dataway
// Arguments:      Int_t Crate        -- crate C
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Initializes camac dataway
//                 Esone call: cccz()
//
//                 >>> protected method <<<
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t sts;
	UInt_t creg;

	if (this->IsOffline()) return(kTRUE);

	if (!this->EsoneCDREG(creg, Crate)) return(kFALSE);
	if (!this->EsoneCDCTRL()) return(kFALSE);

	if (this->IsMbsServer()) {
		if (this->IsSingleStep()) {
			cout << "[TMrbEsone::EsoneCCCZ | Initialize dataway] Crate=" << Crate << " ... "; getchar();
			sts = cccz(creg);
			cout << "[TMrbEsone::EsoneCCCZ] Status=" << sts << endl;
		} else {
			sts = cccz(creg);
		}
		return(sts == 0);
	} else {						// not yet implemented
		return(kFALSE);
	}
}

UInt_t TMrbEsone::EsoneCTSTAT() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::EsoneCTSTAT
// Purpose:        Test controller status
// Arguments:      --
// Results:        UInt_t Status      -- status information
// Exceptions:
// Description:    Tests controller status
//                 Esone call: ctstat()
//
//                 >>> protected method <<<
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t sts;

	if (this->IsOffline()) return(0);

	if (this->IsMbsServer()) {
		if (this->IsSingleStep()) {
			cout << "[TMrbEsone::EsoneCTSTAT | Controller status] ... "; getchar();
			ctstat(&sts);
			cout << "[TMrbEsone::EsoneCTSTAT] Status=" << sts << endl;
		} else {
			ctstat(&sts);
		}
		return(sts);
	} else {						// not yet implemented
		return(0);
	}
}

Bool_t TMrbEsone::EsoneCXSA(TMrbEsoneCnaf & Cnaf, Bool_t D16Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::EsoneCXSA
// Purpose:        Execute single cnaf
// Arguments:      TMrbEsoneCnaf & Cnaf  -- cnaf (+data)
//                 Bool_t D16Flag        -- 16 bit data if kTRUE
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Executes a single camac cnaf, 16 or 24 bit
//                 Esone call: cssa(), cfsa()
//                 >>> protected method <<<
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t sts;
	UInt_t camacCNA;
	Int_t camacData;

	if (this->IsMbsServer()) {
		if (!this->EsoneCDREG(camacCNA, Cnaf)) return(kFALSE);
		if (!this->EsoneCDCTRL()) return(kFALSE);
		camacData = Cnaf.IsWrite() ? Cnaf.GetData() : 0;
		if (D16Flag) {
			Cnaf.SetAction(kCA_cssa);
			this->SetAction(kCA_cssa);
			if (this->IsOffline()) return(kTRUE);
			if (this->IsSingleStep()) {
				cout << "[TMrbEsone::EsoneCXSA | Exec cnaf D16] " << Cnaf.Int2Ascii() << " ... "; getchar();
				sts = cssa(Cnaf.GetF(), camacCNA, &camacData, NULL); // exec single cnaf, 16 bit
				cout	<< "[TMrbEsone::EsoneCXSA] Status=" << sts
						<< " Data=" << camacData << " 0x" << setbase(16) << camacData<< setbase(10) << endl;
			} else {
				sts = cssa(Cnaf.GetF(), camacCNA, &camacData, NULL); // exec single cnaf, 16 bit
			}
		} else {
			Cnaf.SetAction(kCA_cfsa);
			this->SetAction(kCA_cfsa);
			if (this->IsOffline()) return(kTRUE);
			if (this->IsSingleStep()) {
				cout << "[TMrbEsone::EsoneCXSA | Exec cnaf D24] " << Cnaf.Int2Ascii() << " ... "; getchar();
				sts = cfsa(Cnaf.GetF(), camacCNA, &camacData, NULL); // exec single cnaf, 24 bit
				cout	<< "[TMrbEsone::EsoneCXSA] Status=" << sts
						<< " Data=" << camacData << " 0x" << setbase(16) << camacData<< setbase(10) << endl;
			} else {
				sts = cfsa(Cnaf.GetF(), camacCNA, &camacData, NULL); // exec single cnaf, 24 bit
			}
		}
		Cnaf.ClearStatus(); 		// clear error, X, Q
		Cnaf.ClearData();			// clear data
		if (sts == -1) {			// X = 0, Q = 0
			Cnaf.SetError();
			this->SetError();
			return(kFALSE);
		} else {
			Cnaf.SetX(); this->SetX(); 						// X = 1
			if (sts != 0) { Cnaf.SetQ(); this->SetQ(); } 	// Q = 1
			if (Cnaf.IsRead()) Cnaf.SetData(camacData);
			return(kTRUE);
		}
	} else {						// not yet implemented
		return(kFALSE);
	}
}

Int_t TMrbEsone::EsoneCXGA(TObjArray & CnafList, Bool_t D16Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::EsoneCXGA
// Purpose:        Execute cnaf list
// Arguments:      TObjArray & CnafList  -- list of cnafs (+data)
//                 Bool_t D16Flag        -- 16 bit data if kTRUE
// Results:        Int_t NofCnafs        -- number of cnafs executed
// Exceptions:     NofCnafs = kEsoneError on error
// Description:    Executes cnafs in the list, 16 or 24 bit
//                 Esone call: csga(), cfga()
//
//                 >>> protected method <<<
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t sts;
	Int_t nofCnafs;
	UInt_t cna;
	TMrbEsoneCnaf * cnaf;
	TArrayI camacF, camacCNA, camacData, camacXQ;
	TArrayI ctrlBlock(4);

	if (this->IsMbsServer()) {
		if (!this->EsoneCDCTRL()) return(kEsoneError);
		nofCnafs = CnafList.GetEntriesFast();
		this->SetCB(ctrlBlock, nofCnafs);
		camacF.Set(nofCnafs);
		camacCNA.Set(nofCnafs);
		camacData.Set(nofCnafs);
		camacXQ.Set(nofCnafs);
		this->SetAction(D16Flag ? kCA_csga : kCA_cfga);
		if (this->IsSingleStep()) cout << "[TMrbEsone::EsoneCXGA] List of CNAFs:" << endl;
		for (Int_t i = 0; i < nofCnafs; i++) {
			cnaf = (TMrbEsoneCnaf *) CnafList[i];
			if (this->IsSingleStep()) cout << "   " << i << ": " << cnaf->Int2Ascii() << endl;
			camacF[i] = cnaf->GetF();
			this->EsoneCDREG(cna, cnaf->GetC(), cnaf->GetN(), cnaf->GetA());
			camacCNA[i] = (Int_t) cna;
			camacData[i] = cnaf->IsWrite() ? cnaf->GetData() : 0;
			camacXQ[i] = 0;
			cnaf->SetAction(D16Flag ? kCA_csga : kCA_cfga);
		}
		if (this->IsOffline()) return(nofCnafs);
		if (D16Flag) {
			if (this->IsSingleStep()) {
				cout << "[TMrbEsone::EsoneCXGA | Exec cnaf list D16] ... "; getchar();
				sts = csga( camacF.GetArray(), camacCNA.GetArray(), camacData.GetArray(),
							camacXQ.GetArray(), ctrlBlock.GetArray());
				cout	<< "[TMrbEsone::EsoneCXGA] Status=" << sts << endl;
			} else {
				sts = csga( camacF.GetArray(), camacCNA.GetArray(), camacData.GetArray(),
							camacXQ.GetArray(), ctrlBlock.GetArray());
			}
		} else {
			if (this->IsSingleStep()) {
				cout << "[TMrbEsone::EsoneCXGA | Exec cnaf list D24] ... "; getchar();
				sts = cfga( camacF.GetArray(), camacCNA.GetArray(), camacData.GetArray(),
							camacXQ.GetArray(), ctrlBlock.GetArray());
				cout	<< "[TMrbEsone::EsoneCXGA] Status=" << sts << endl;
			} else {
				sts = cfga( camacF.GetArray(), camacCNA.GetArray(), camacData.GetArray(),
							camacXQ.GetArray(), ctrlBlock.GetArray());
			}
		}
		this->ClearStatus();		// clear status
		if (sts == 0) {
			nofCnafs = ctrlBlock[1];
			for (Int_t i = 0; i < nofCnafs; i++) {
				cnaf = (TMrbEsoneCnaf *) CnafList[i];
				if (cnaf->IsRead()) cnaf->SetData(camacData[i]);
				cnaf->ClearStatus();
				if (camacXQ[i] & 0x2) cnaf->SetX();
				if (camacXQ[i] & 0x1) cnaf->SetQ();
			}
		} else {
			nofCnafs = kEsoneError;
		}
		return(nofCnafs);
	} else {						// not yet implemented
		return(kEsoneError);
	}
}

Int_t TMrbEsone::EsoneCXMAD(TMrbEsoneCnaf & Start, TMrbEsoneCnaf & Stop, TArrayI & Data, TObjArray & Results, Bool_t D16Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::EsoneCXMAD
// Purpose:        Perform an address scan
// Arguments:      TMrbEsoneCnaf & Start -- start cnaf
//                 TMrbEsoneCnaf & Stop  -- stop cnaf
//                 TArrayI & Data        -- data (read/write)
//                 TObjArray & Results   -- resulting cnafs (+data)
//                 Bool_t D16Flag        -- 16 bit data if kTRUE
// Results:        Int_t NofWords        -- number of data words
// Exceptions:     NofWords = kEsoneError on error
// Description:    Address scan: Executes cnafs starting with 'start',
//                               ending with 'stop',
//                               stops whenever Q=0
//                 Esone call: csmad(), cfmad()
//
//                 >>> protected method <<<
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t sts;
	TArrayI camacCNA(2);
	TArrayI camacData(2048);
	TArrayI ctrlBlock(4);
	UInt_t cna;

	if (this->IsMbsServer()) {
		if (!this->EsoneCDCTRL()) return(kEsoneError);
		if (!this->EsoneCDREG(cna, Start)) return(kEsoneError);			// start
		camacCNA[0] = (Int_t) cna;
		if (!this->EsoneCDREG(cna, Stop)) return(kEsoneError);			// stop
		camacCNA[1] = (Int_t) cna;
		Results.Delete();						// ignored if mbs server
		this->SetCB(ctrlBlock, 2048);			// max number of cnafs

		if (!Start.IsWrite()) Data.Reset();		// reset data buffer in case of read/control cnafs

		if (this->IsOffline()) return(0);

		if (D16Flag) {
			Start.SetAction(kCA_csmad);
			Stop.SetAction(kCA_csmad);
			if (this->IsSingleStep()) {
				cout << "[TMrbEsone::EsoneCXMAD | Address scan D16] Start=" << Start.Int2Ascii() << " Stop=" << Stop.Int2Ascii() << " ... "; getchar();
				sts = csmad(Start.GetF(), camacCNA.GetArray(), Data.GetArray(), ctrlBlock.GetArray());
				cout	<< "[TMrbEsone::EsoneCXMAD] Status=" << sts << endl;
			} else {
				sts = csmad(Start.GetF(), camacCNA.GetArray(), Data.GetArray(), ctrlBlock.GetArray());
			}
		} else {
			Start.SetAction(kCA_cfmad);
			Stop.SetAction(kCA_cfmad);
			if (this->IsSingleStep()) {
				cout << "[TMrbEsone::EsoneCXMAD | Address scan D24] Start=" << Start.Int2Ascii() << " Stop=" << Stop.Int2Ascii() << " ... "; getchar();
				sts = cfmad(Start.GetF(), camacCNA.GetArray(), Data.GetArray(), ctrlBlock.GetArray());
				cout	<< "[TMrbEsone::EsoneCXMAD] Status=" << sts << endl;
			} else {
				sts = cfmad(Start.GetF(), camacCNA.GetArray(), Data.GetArray(), ctrlBlock.GetArray());
			}
		}
		return((sts == 0) ? ctrlBlock[1] : kEsoneError);
	} else {						// not yet implemented
		return(kEsoneError);
	}
}

Int_t TMrbEsone::EsoneCXUBX(TMrbEsoneCnaf & Cnaf, TArrayI & Data, Int_t Start, Int_t NofWords, Bool_t D16Flag, Bool_t QXfer) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::EsoneCXUBX
// Purpose:        Block transfer
// Arguments:      TMrbEsoneCnaf & Cnaf  -- cnaf
//                 TArrayI & Data        -- data
//                 Int_t Start           -- data index to start with
//                 Int_t NofWords        -- number of words to xfer
//                 Bool_t D16Flag        -- 16 bit data if kTRUE
//                 Bool_t QXfer          -- if xfer is to be controlled by Q
// Results:        Int_t NofWords        -- number of data words
// Exceptions:     NofWords = kEsoneError on error
// Description:    Executes a block transfer
//                 Esone call: csubr(), cfubr(), csubc(), cfubc()
//
//                 >>> protected method <<<
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TArrayI ctrlBlock(4);
	UInt_t camacCNA;
	Int_t sts;

	if (this->IsMbsServer()) {
		if (!this->EsoneCDCTRL()) return(kEsoneError);
		if (!this->EsoneCDREG(camacCNA, Cnaf)) return(kEsoneError);
		this->SetCB(ctrlBlock, NofWords);
		if (this->IsOffline()) return(0);
		if (QXfer) {
			if (D16Flag) {
				Cnaf.SetAction(kCA_csubr);
				if (this->IsSingleStep()) {
					cout << "[TMrbEsone::EsoneCXUBX | Block xfer D16/Q] Cnaf=" << Cnaf.Int2Ascii() << " ... "; getchar();
					sts = csubr(Cnaf.GetF(), camacCNA, Data.GetArray() + Start, ctrlBlock.GetArray());
					cout	<< "[TMrbEsone::EsoneCXUBX] Status=" << sts << endl;
				} else {
					sts = csubr(Cnaf.GetF(), camacCNA, Data.GetArray() + Start, ctrlBlock.GetArray());
				}
			} else {
				Cnaf.SetAction(kCA_cfubr);
				if (this->IsSingleStep()) {
					cout << "[TMrbEsone::EsoneCXUBX | Block xfer D24/Q] Cnaf=" << Cnaf.Int2Ascii() << " ... "; getchar();
					sts = cfubr(Cnaf.GetF(), camacCNA, Data.GetArray() + Start, ctrlBlock.GetArray());
					cout	<< "[TMrbEsone::EsoneCXUBX] Status=" << sts << endl;
				} else {
					sts = cfubr(Cnaf.GetF(), camacCNA, Data.GetArray() + Start, ctrlBlock.GetArray());
				}
			}
		} else {
			if (D16Flag) {
				Cnaf.SetAction(kCA_csubc);
				if (this->IsSingleStep()) {
					cout << "[TMrbEsone::EsoneCXUBX | Block xfer D16] Cnaf=" << Cnaf.Int2Ascii() << " ... "; getchar();
					sts = csubc(Cnaf.GetF(), camacCNA, Data.GetArray() + Start, ctrlBlock.GetArray());
					cout	<< "[TMrbEsone::EsoneCXUBX] Status=" << sts << endl;
				} else {
					sts = csubc(Cnaf.GetF(), camacCNA, Data.GetArray() + Start, ctrlBlock.GetArray());
				}
			} else {
				Cnaf.SetAction(kCA_cfubc);
				if (this->IsSingleStep()) {
					cout << "[TMrbEsone::EsoneCXUBX | Block xfer D24] Cnaf=" << Cnaf.Int2Ascii() << " ... "; getchar();
					sts = cfubc(Cnaf.GetF(), camacCNA, Data.GetArray() + Start, ctrlBlock.GetArray());
					cout	<< "[TMrbEsone::EsoneCXUBX] Status=" << sts << endl;
				} else {
					sts = cfubc(Cnaf.GetF(), camacCNA, Data.GetArray() + Start, ctrlBlock.GetArray());
				}
			}
		}
		return((sts == 0) ? ctrlBlock[1] : kEsoneError);
	} else {
		return(kEsoneError); 		// not yet implemented
	}
}

Bool_t TMrbEsone::EsoneSpecial(EMrbEsoneCnafType Type,
													Int_t Crate, Int_t Station, Int_t Subaddr, Int_t Function,
													Int_t & Data, Bool_t D16Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::EsoneSpecial
// Purpose:        Excecute special cnaf
// Arguments:      EMrbEsoneCnafType Type  -- cnaf type
//                 Int_t Crate             -- crate C
//                 Int_t Station           -- station N
//                 Int_t Subaddr           -- subaddress A
//                 Int_t Function          -- function F
//                 Int_t & Data            -- data i/o
//                 Bool_t D16Flag          -- 16 bit data if kTRUE
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Executes a special cnaf, 16 or 24 bit
//                 Esone call: cssa(), cfsa()
//                 >>> protected method <<<
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t sts;
	UInt_t camacCNA;
	Int_t camacData;
	TMrbEsoneCnaf cnaf;

	if (this->IsMbsServer()) {
		cnaf.Set(Crate, Station, Subaddr);
		if (!this->EsoneCDCTRL()) return(kFALSE);
		if (!this->EsoneCDREG(camacCNA, cnaf)) return(kFALSE);
		camacData = Data;
		if (D16Flag) {
			this->SetAction(kCA_cssa);
			if (this->IsOffline()) return(kTRUE);
			sts = cssa(Function, camacCNA, &camacData, NULL); // exec single cnaf, 16 bit
		} else {
			this->SetAction(kCA_cfsa);
			if (this->IsOffline()) return(kTRUE);
			sts = cfsa(Function, camacCNA, &camacData, NULL); // exec single cnaf, 24 bit
		}
		Data = 0;
		if (sts == -1) {			// X = 0, Q = 0
			this->SetError();
			return(kFALSE);
		} else if (Type == kCnafTypeRead) {
			Data = camacData;
			return(kTRUE);
		}
	} else {						// not yet implemented
		return(kFALSE);
	}
	return(kFALSE);
}

const Char_t * TMrbEsone::EsoneCERROR(TString & ErrMsg, Int_t ErrorCode, Bool_t DateFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::EsoneCERROR
// Purpose:        Report error
// Arguments:      TString & ErrMsg      -- error message
//                 Int_t ErrorCode       -- error code
//                 Bool_t DateFlag       -- include date if kTRUE
// Results:        Char_t * ErrMsg       -- pointer to error message
// Exceptions:
// Description:    Reports any error recently produced by a esone call
//                 Esone call: cerror()
//
//                 >>> protected method <<<
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->IsMbsServer()) {
		ErrMsg = cerror(ErrorCode, DateFlag ? 1 : 0);
		return(ErrMsg.Data());
	} else {
		return("");
	}
}


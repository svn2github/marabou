//__________________________________________________________________[C++ MAIN]
//////////////////////////////////////////////////////////////////////////////
// Name:           mrbLynxSrv.cxx
// Purpose:        MARaBOU's Lynx server
// Description:    Interface ROOT <--> Lynx
//
// Modules:
// Author:         R. Lutter
// Revision:
// Date:           Apr 2008
// Keywords:
//
//////////////////////////////////////////////////////////////////////////////

#include "iostream.h"
#include "iomanip.h"
#include <unistd.h>

#include "LwrTypes.h"
#include "LwrLynxOsSystem.h"
#include "LwrLogger.h"

#include "SrvSocket.h"
#include "SrvVMEModule.h"
#include "SrvSis3302.h"
#include "SrvCaen785.h"
#include "SrvVulomTB.h"

#include "SetColor.h"

#define SRV_SLEEP	3

#include "Version.h"

extern TLynxOsSystem * gSystem;

extern TMrbLofNamedX * gLofVMEProtos;		// list of vme prototypes
extern TMrbLofNamedX * gLofVMEModules;		// list of modules

extern TMrbLogger * gMrbLog;		// message logger

SrvSocket * gSrvSocket; 		// server socket

Int_t main(Int_t argc, Char_t *argv[]) {
//__________________________________________________________________[C++ MAIN]
//////////////////////////////////////////////////////////////////////////////
// Name:           mrbLynxSrv
// Purpose:        Lynx server
// Arguments:      Int_t Port           -- port number
//                 Char_t * LogFile     -- logfile
//                 Int_t NonBlocking    -- 0 or 1
//                 Int_t VerboseMode    -- 0 or 1
// Results:
// Exceptions:
// Description:
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t port;
	ULong_t * addr;
	Int_t bufsiz;
	Int_t nonBlocking;
	Int_t verboseMode;
	TString logFile;

	if (argc < 2) port = 9010; else port = atoi(argv[1]);
	if (argc < 3 || argv[2] == NULL || *argv[2] == '\0') {
		logFile = getenv("PWD");
		logFile += "/c2lynx.log";
	} else {
		logFile = argv[2];
	}
	if (argc < 4) nonBlocking = 0; else nonBlocking = atoi(argv[3]);
	if (argc < 5) verboseMode = 0; else verboseMode = atoi(argv[4]);

	gSystem = new TLynxOsSystem();

	gMrbLog = new TMrbLogger("", logFile.Data());

	TString serverName = basename(argv[0]);
	Char_t * sp = strrchr(argv[0], '/');
	if (sp) *sp = '\0';
	TString serverPath = argv[0];

	cout	<< setblue << "Initializing ..." << endl;
	cout	<< "---------------------------------------------------------------------------" << endl;
	cout	<< " Server           : " << serverName.Data() << endl;
	cout	<< " Path             : " << serverPath.Data() << endl;
	cout	<< " Port             : " << port << endl;
	cout	<< " Blocking         : " << ((nonBlocking == 1) ? "off" : "on") << endl;
	cout	<< " Verbose mode     : " << ((verboseMode == 1) ? "on" : "off") << endl;
	cout	<< " Built for        : " << cpuType + 3 << " " << lynxVersion + 3 << endl;
	cout	<< "---------------------------------------------------------------------------" << setblack << endl;

	gSrvSocket = new SrvSocket(serverName.Data(), serverPath.Data(), port, nonBlocking, verboseMode);
	if (gSrvSocket->IsZombie()) {
		sleep(SRV_SLEEP);
		exit(gSrvSocket->GetErrorCode());
	}

	gLofVMEProtos = new TMrbLofNamedX("List of VME prototypes");
	gLofVMEModules = new TMrbLofNamedX("List of VME modules");

	SrvSis3302 * sis3302 = new SrvSis3302();
	if (verboseMode && !sis3302->IsZombie()) sis3302->Print();
	SrvCaen785 * caen785 = new SrvCaen785();
	if (verboseMode && !caen785->IsZombie()) caen785->Print();
	SrvVulomTB * vulomTB = new SrvVulomTB();
	if (verboseMode && !vulomTB->IsZombie()) vulomTB->Print();

	gMrbLog->Out() << "Now listening to tcp socket  ..." << endl;
	gMrbLog->Flush(serverName.Data(), "", setblue);

	gSrvSocket->Listen();

	gMrbLog->Out() << "Shutting down ..." << endl;
	gMrbLog->Flush(serverName.Data(), "", setblue);
	sleep(SRV_SLEEP);
	gMrbLog->Close();
	exit(0);
};

//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           c2lynx/src/TMrbC2Lynx.cxx
// Purpose:        MARaBOU client to connect to LynxOs/VME
// Description:    Implements class methods for a client to LynxOs/VME
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbC2Lynx.cxx,v 1.2 2007-04-25 06:41:01 Rudolf.Lutter Exp $     
// Date:           $Date: 2007-04-25 06:41:01 $
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

ClassImp(TMrbC2Lynx)

TMrbC2Lynx::TMrbC2Lynx(const Char_t * HostName, const Char_t * Server, Int_t Port, Bool_t NonBlocking, Bool_t UseXterm) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbC2Lynx
// Purpose:        Connect to LynxOs
// Arguments:      Char_t * HostName      -- server host
//                 Char_t * Server        -- server path
//                 Int_t Port             -- tcp port
//                 Bool_t NonBlocking     -- kTRUE, if non-blocking mode
//                 Bool_t UseXterm        -- open cterm to show server output
// Results:        --
// Description:    Class constructor
//////////////////////////////////////////////////////////////////////////////

	this->Reset();

	fHost = HostName;
	fPort = Port;

	fNonBlocking = NonBlocking;
	fUseXterm = UseXterm;

	fSocket = NULL;
	
	fServerPath = Server;
	fServerName = gSystem->BaseName(Server);

	TSocket * s = new TSocket(fHost.Data(), fPort);
	Bool_t sockOk = s->IsValid();
	if (sockOk) {
		if (fVerboseMode || fDebugMode) {
			gMrbLog->Out()	<< "Connecting to server " << fHost << ":" << fPort
							<< " (progr " << fServerName << ")" << endl;
			gMrbLog->Flush(this->ClassName());
		}
		fSocket = s;
	} else {
		if (fVerboseMode || fDebugMode) {
			gMrbLog->Out()	<< "Trying to connect to server " << fHost << ":" << fPort
							<< " (progr " << fServerName << ")" << endl;
			gMrbLog->Flush(this->ClassName());
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
		cmd2 += (fNonBlocking ? 1 : 0);

		if (fDebugMode) {
			gMrbLog->Out()	<< "[Debug mode] Start manually @ " << fHost << " >> " << cmd2 << " <<" << endl;
			gMrbLog->Flush(this->ClassName());
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
		} else {
			cout << endl;
			gMrbLog->Err()	<< "Can't connect to server/port " << fHost << ":" << fPort << endl;
			gMrbLog->Flush(this->ClassName());
			fSocket = NULL;
			this->MakeZombie();
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

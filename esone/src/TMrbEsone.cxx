//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           esone/src/TMrbEsone.cxx
// Purpose:        ESONE client class
// Description:    Implements class methods to interface the ESONE client library
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <time.h>
#include <iostream.h>
#include <strstream.h>
#include <iomanip.h>
#include <fstream.h>

#include "Rtypes.h"
#include "TSystem.h"
#include "TEnv.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbLogger.h"

#include "esone_protos.h"

#include "TMrbEsone.h"

#include "SetColor.h"

const SMrbNamedX kMrbLofCamacActions[] =		// list of camac actions
			{
    		  {TMrbEsone::kCANone,		"n.a",		"No Action",												},
    		  {TMrbEsone::kCA_cfsa, 	"cfsa", 	"Single CAMAC Action, 24 bit",							  	},
    		  {TMrbEsone::kCA_cssa, 	"cssa", 	"Single CAMAC Action, 16 bit",							  	},
    		  {TMrbEsone::kCA_cfga, 	"cfga", 	"General Multiple CAMAC Action, 24 bit",  				  	},
    		  {TMrbEsone::kCA_csga, 	"csga", 	"General Multiple CAMAC Action, 16 bit",  				  	},
    		  {TMrbEsone::kCA_cfmad,	"cfmad",	"Address Scan, 24 bit",  								  	},
    		  {TMrbEsone::kCA_csmad,	"csmad",	"Address Scan, 16 bit",  								  	},
    		  {TMrbEsone::kCA_cfubc,	"cfubc",	"Controller Synchronized Block Xfer, 24 bit",			  	},
    		  {TMrbEsone::kCA_csubc,	"csubc",	"Controller Synchronized Block Xfer, 16 bit",			  	},
    		  {TMrbEsone::kCA_cfubr,	"cfubr",	"Controller Synchronized Block Xfer, Q-driven, 24 bit",    	},
    		  {TMrbEsone::kCA_csubr,	"csubr",	"Controller Synchronized Block Xfer, Q-driven, 16 bit",    	},
    		  {0,						NULL, 		NULL									 					}
    	  };

const SMrbNamedXShort kMrbLofCNAFNames[] =
							{
								{TMrbEsoneCNAF::kCnafCrate, 	"CRATE",			},
								{TMrbEsoneCNAF::kCnafStation,	"NSTATION"	 		},
								{TMrbEsoneCNAF::kCnafAddr,		"ADDRESS"			},
								{TMrbEsoneCNAF::kCnafFunction,	"FUNCTION"	 		},
								{TMrbEsoneCNAF::kCnafData,		"DATA"	 			},
								{0, 							NULL				}
							};

const SMrbNamedXShort kMrbLofCNAFTypes[] =
							{
								{TMrbEsoneCNAF::kCnafTypeRead,			"READ"  	},
								{TMrbEsoneCNAF::kCnafTypeWrite, 		"WRITE" 	},
								{TMrbEsoneCNAF::kCnafTypeControl,		"CNTRL" 	},
								{TMrbEsoneCNAF::kCnafTypeReadStatus,	"RSTATUS"	},
								{0, 									NULL}
							};

const SMrbNamedXShort kMrbLofControllers[] =
							{
								{TMrbEsone::kCC_CBV,					"CBV"		},
								{TMrbEsone::kCC_CC32,					"CC32"		},
								{0, 									NULL}
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

	TString camacContr;
	TMrbNamedX * nx;
		
	if (gMrbLog == NULL) gMrbLog = new TMrbLogger("esone.log");
	
	fVerboseMode = gEnv->GetValue("TMrbEsone.VerboseMode", kFALSE);
	
	fLofCamacActions.SetName("CAMAC Actions");		// camac actions
	fLofCamacActions.AddNamedX(kMrbLofCamacActions);
	
	fLofCNAFNames.SetName("CAMAC Registers");		// register names
	fLofCNAFNames.AddNamedX(kMrbLofCNAFNames);
	
	fLofCNAFTypes.SetName("CNAF Types");			// cnaf types
	fLofCNAFTypes.AddNamedX(kMrbLofCNAFTypes);
	
	fLofControllers.SetName("CAMAC Controller");	// controllers
	fLofControllers.AddNamedX(kMrbLofControllers);
	
	camacContr = gEnv->GetValue("TMrbEsone.Controller", "");
	if ((nx = fLofControllers.FindByName(camacContr.Data())) == NULL) {
		gMrbLog->Err() << "CAMAC controller undefined (\"TMrbEsone.Controller\" has to be set)" << endl;
		gMrbLog->Flush(this->ClassName(), "Reset");
		return(kFALSE);
	}
	fController.Set(nx);
	if (this->IsVerbose()) {
		gMrbLog->Out() << "Using CAMAC controller \"" << fController.GetName() << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "Reset", setblue);
	}
		
	fHost.Resize(0);								// host name
	fHostInet.Resize(0);
	fHostAddr = 0;									// host address

	fCrate = -1;									// crate number

	fError.Resize(0); 	 							// error message
	fErrorCode = 0;									// error code

	this->ResetBuffers(kMaxNofCnafs);				// reset buffers

	fOffline = Offline; 							// hardware access?
	
	fUseBroadCast = gEnv->GetValue("TMrbEsone.UseBroadCast", kTRUE); // use broadcast if applicable
		
	fAction = TMrbEsone::kCANone;					// type of last action
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

	if (fHostAddr > 0) return(kTRUE);

	gMrbLog->Err() << "No CAMAC host connected" << endl;
	gMrbLog->Flush(this->ClassName(), Method);
	return(kFALSE);
}

Int_t TMrbEsone::CheckCrate(Int_t Crate, const Char_t * Method) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::CheckCrate
// Purpose:        Check if crate number is legal
// Arguments:      Int_t Crate        -- crate number
//                 Char_t * Method    -- calling method
// Results:        Int_t Crate        -- crate number or -1 if error
// Exceptions:
// Description:    Checks if crate number is ok.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Crate == TMrbEsone::kDefaultCrate) Crate = fCrate;
	if (Crate == -1) {
		gMrbLog->Err() << "No crate defined" << endl;
		gMrbLog->Flush(this->ClassName(), Method);
		return(-1);
	}

	if (Crate < 0 || Crate > kMaxNofCrates) {
		gMrbLog->Err() << "Illegal crate number - " << Crate << " (should be in [0," << kMaxNofCrates << "])" << endl;
		gMrbLog->Flush(this->ClassName(), Method);
		return(-1);
	}
	return(Crate);
}

Bool_t TMrbEsone::CheckIndex(Int_t Index, const Char_t * Method) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::CheckIndex
// Purpose:        Check if index is legal
// Arguments:      Int_t Index        -- index in cnaf/data array
//                 Char_t * Method    -- calling method
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks if index is ok.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Index == -1) {					// single action
		if ((fAction & TMrbEsone::kCASingle) == 0) {
			gMrbLog->Err() << "Valid only with single CNAFs" << endl;
			gMrbLog->Flush(this->ClassName(), Method);
			return(kFALSE);
		}
		Index = 0;
	}

	if (this->IsOffline()) return(0);
	
	if (Index < 0 || Index >= fCurNofCnafs) {
		gMrbLog->Err() << "Illegal index - " << Index << " (should be in [0," << fCurNofCnafs << "])" << endl;
		gMrbLog->Flush(this->ClassName(), Method);
		return(kFALSE);
	}
	return(Index);
}

Int_t TMrbEsone::CheckSize(Int_t Size, const Char_t * Method) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::CheckSize
// Purpose:        Check if data size is legal
// Arguments:      Int_t Size         -- data size
//                 Char_t * Method    -- calling method
// Results:        Int_t Size         -- actual size
// Exceptions:
// Description:    Checks if data size is ok.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Size == -1) Size = fMaxNofCnafs;
	if (Size < 1 || Size > fMaxNofCnafs) {
		gMrbLog->Err()	<< "Number of data words out of bounds - " << Size
						<< " (should be in [1," << fMaxNofCnafs << "])" << endl;
		gMrbLog->Flush(this->ClassName(), Method);
		return(-1);
	}
	return(Size);
}

Bool_t TMrbEsone::StartServer(const Char_t * HostName, const Char_t * SetupPath, const Char_t * MbsVersion, Bool_t PrmFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::StartServer
// Purpose:        Try to start esone server on host
// Arguments:      Char_t * HostName    -- host name
//                 Char_t * SetupPath   -- where to find MBS setup files
//                 Char_t * MbsVersion  -- MBS version to be used
//                 Bool_t PrmFlag       -- start m_prompt if kTRUE
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Tries to start mbs.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	FILE * pCmd;
	ostrstream * rCmd;
	Char_t line[100];
	TString pLine;
	Bool_t found;
	TString remoteHome;
	TString prmOrDsp;

	this->SetOffline(kFALSE);
	
	// reset mbs
	cout	<< setmagenta
			<< this->ClassName() << "::StartServer(): Resetting ESONE server on host \""
			<< HostName << "\". Please wait ."
			<< setblack << ends << flush;

	rCmd = new ostrstream();
	*rCmd	<< "rsh " << HostName
			<< " -l " << gSystem->Getenv("USER")
			<< " \"/mbs/" << MbsVersion
			<< "/script/remote_exe.sc /mbs/" << MbsVersion
			<< " " << SetupPath << " m_remote reset -l >>/dev/null\""
			<< ends;
	gSystem->Exec(rCmd->str());
	rCmd->rdbuf()->freeze(0);
	delete rCmd;

	rCmd = new ostrstream();
	*rCmd	<< "rsh " << HostName << " -l " << gSystem->Getenv("USER") << " \"ps ax | fgrep m_\""
			<< ends;

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
		pCmd = gSystem->OpenPipe(rCmd->str(), "r");
		while (fgets(line, 100, pCmd) != NULL) {
			pLine = line;
			if (pLine.Index("m_") == -1) break;
			found = kTRUE;
		}
		gSystem->ClosePipe(pCmd);
		if (!found) {
			cout << setmagenta << " done." << setblack << endl;
			break;
		}
	}

	rCmd->rdbuf()->freeze(0);
	delete rCmd;

	if (found) {
		gMrbLog->Err() << "Can't RESET server on host \"" << HostName << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "StartServer");
		return(kFALSE);
	}

// restart mbs
	cout	<< setmagenta
			<< this->ClassName() << "::StartServer(): Restarting ESONE server ("
			<< fController.GetName() << ") on host \"" << HostName << "\". Please wait ."
			<< setblack << ends << flush;

	rCmd = new ostrstream();
	*rCmd	<< "rsh " << HostName
			<< " -l " << gSystem->Getenv("USER")
			<< " \"cp " << SetupPath << "/.tcshrc . >>/dev/null\""
			<< ends;
	gSystem->Exec(rCmd->str());
	rCmd->rdbuf()->freeze(0);
	delete rCmd;

	prmOrDsp = PrmFlag ? "m_prompt" : "m_dispatch";

	rCmd = new ostrstream();
	*rCmd	<< "rsh " << HostName
			<< " -l " << gSystem->Getenv("USER")
			<< " \"/mbs/" << MbsVersion
			<< "/script/remote_exe.sc /mbs/" << MbsVersion
			<< " . " << prmOrDsp << " @" << SetupPath << "/"
			<< fController.GetName() << "/startup >>/dev/null\""
			<< ends;
	gSystem->Exec(rCmd->str());
	rCmd->rdbuf()->freeze(0);
	delete rCmd;

	rCmd = new ostrstream();
	*rCmd	<< "rsh " << HostName << " -l " << gSystem->Getenv("USER") << " \"ps ax | fgrep m_\""
			<< ends;

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
		pCmd = gSystem->OpenPipe(rCmd->str(), "r");
		while (fgets(line, 100, pCmd) != NULL) {
			pLine = line;
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

	rCmd->rdbuf()->freeze(0);
	delete rCmd;

	if (found) {
		gMrbLog->Out()	<< "ESONE server (" << fController.GetName() << ") running on host \""
						<< HostName << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "StartServer", setblue);
		return(kTRUE);
	} else {
		gMrbLog->Err()	<< "Can't start server (" << fController.GetName() << ") on host \""
						<< HostName << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "StartServer");
		return(kFALSE);
	}
}

CamacHost_t TMrbEsone::ConnectToHost(const Char_t * HostName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::ConnectToHost
// Purpose:        Establish connection to camac host
// Arguments:      Char_t * HostName  -- host name
//                 Bool_t Offline     -- hardware access to be simulated by software?
// Results:        CamacHost_t        -- host address, 0 if error
// Exceptions:
// Description:    ESONE ccopen.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * host;
	CamacHost_t hostAddr;
	TInetAddress * ia;
	TString iAddr;

	if (gMrbEsoneHosts == NULL) {				// list of active camac hosts
		gMrbEsoneHosts = new TMrbLofNamedX("List of ESONE hosts");
	}

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
	if (host == NULL) {
		if (!this->IsOffline() && ccopen((Char_t *) HostName, &hostAddr) == -1) {		// no: try to connect via ESONE
			gMrbLog->Err() << "Can't connect to " << HostName << endl;
			this->PrintError("ccopen", "ConnectToHost");
			fHost.Resize(0);
			fHostInet.Resize(0);
			fHostAddr = 0xffffffff;
			return(0);
		}
		host = new TMrbNamedX(hostAddr, HostName);  			// success: insert in internal table for further use
		gMrbEsoneHosts->AddNamedX(host);
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
// Description:    ESONE cccc.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	CamacReg_t creg;

	if (!this->CheckConnection("ClearDW")) return(kFALSE);
	if ((Crate = this->CheckCrate(Crate, "ClearDW")) == -1) return(kFALSE);	// take default crate if not explicitly given

	creg = cdreg(&creg, fHostAddr, Crate, 0, 0);				// get ESONE-style CAMAC command: HOST.Cxx.N0.A0

	cdctrl(fController.GetIndex());							// define controller: cnaf code depends on it
	if (this->IsOffline() || cccc(creg) == 0) return(kTRUE); 	// exec CAMAC-C
	gMrbLog->Err() << "Dataway clear failed -" << endl;
	this->PrintError("cccc", "ClearDW");
	return(kFALSE);
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
// Description:    ESONE ccci.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	CamacReg_t creg;
	Int_t lflag;

	if (!this->CheckConnection("SetDWInhibit")) return(kFALSE);
	if ((Crate = this->CheckCrate(Crate, "SetDWInhibit")) == -1) return(kFALSE);	// take default crate if not explicitly given

	creg = cdreg(&creg, fHostAddr, Crate, 0, 0);				// get ESONE-style CAMAC command: HOST.Cxx.N0.A0

	lflag = Inhibit ? (Int_t) kFEnable : (Int_t) kFDisable; 					// enable / disable

	cdctrl(fController.GetIndex());							// define controller: cnaf code depends on it
	if (this->IsOffline() || ccci(creg, lflag) == 0) return(kTRUE);	// exec CAMAC-I
	gMrbLog->Err() << "Dataway inhibit failed -" << endl;
	this->PrintError("ccci", "SetDWInhibit");
	return(kFALSE);
}

Bool_t TMrbEsone::DWInhibit(Int_t Crate) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::DWInhibit
// Purpose:        Test dataway inhibit
// Arguments:      Int_t Crate        -- crate number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Test the dataway inhibit flag (ESONE ctci).
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	CamacReg_t creg;
	Int_t dwFlag;

	if (!this->CheckConnection("DWInhibit")) return(kFALSE);
	if ((Crate = this->CheckCrate(Crate, "DWInhibit")) == -1) return(kFALSE);	// take default crate if not explicitly given

	creg = cdreg(&creg, fHostAddr, Crate, 0, 0);				// get ESONE-style CAMAC command: HOST.Cxx.N0.A0

	if (this->IsOffline()) return(kTRUE);
	ctci(creg, &dwFlag);
	return(dwFlag == 1);
}

Bool_t TMrbEsone::InitDW(Int_t Crate) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::InitDW
// Purpose:        Initialize dataway
// Arguments:      Int_t Crate     -- crate number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    ESONE cccz.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	CamacReg_t creg;

	if (!this->CheckConnection("InitDW")) return(kFALSE);
	if ((Crate = this->CheckCrate(Crate, "InitDW")) == -1) return(kFALSE);	// take default crate if not explicitly given

	creg = cdreg(&creg, fHostAddr, Crate, 0, 0);				// get ESONE-style CAMAC command: HOST.Cxx.N0.A0

	cdctrl(fController.GetIndex());							// define controller: cnaf code depends on it
	if (this->IsOffline() || cccz(creg) == 0) return(kTRUE); 	// exec CAMAC-Z
	gMrbLog->Err() << "Dataway initialize failed -" << endl;
	this->PrintError("cccz", "InitDW");
	return(kFALSE);
}

Int_t TMrbEsone::ExecCnaf(const Char_t * Cnaf, CamacData_t Data, Bool_t D16Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::ExecCnaf
// Purpose:        Exec a single CNAF
// Arguments:      Char_t * Cnaf     -- cnaf (ascii representation)
//                 CamacData_t Data  -- data
//                 Bool_t D16Flag    -- kTRUE if 16 bits only
// Results:        Int_t TallyCount  -- -1 (error), 1 (normally)
// Exceptions:
// Description:    ESONE cfsa / cssa.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbEsoneCNAF cnaf;
	Int_t stsX;
	Int_t crate;

	if (!this->CheckConnection("ExecCnaf")) return(-1);

	if (!cnaf.Ascii2Int(Cnaf)) return(-1);			// decode CNAF

	if ((crate = cnaf.Get(TMrbEsoneCNAF::kCnafCrate)) == -1) {					// crate number explicitly given? 
		if ((crate = this->CheckCrate(crate, "ExecCnaf")) == -1) return(-1);	// no, try to take default
		cnaf.Set(TMrbEsoneCNAF::kCnafCrate, crate);
	}

	fFunction[0] = cnaf.Get(TMrbEsoneCNAF::kCnafFunction); // get actual function

	if (cnaf.IsWrite()) {
		if (Data == kNoData) Data = cnaf.Get(TMrbEsoneCNAF::kCnafData);
		if (Data == kNoData) {
			gMrbLog->Err() << "Function F" << fFunction[0] << " (WRITE) needs DATA" << endl;
			gMrbLog->Flush(this->ClassName(), "ExecCnaf");
			return(-1);
		}
		fData[0] = Data;								// store data if WRITE
		cnaf.Set(TMrbEsoneCNAF::kCnafData, Data);
	} else {
		fData[0] = kNoData;
	}

	if (!cnaf.CheckCnaf()) return(-1);					// check for completeness & consistency

	fCnaf[0] = cnaf.Int2Esone(fHostAddr);				// get CAMAC command: HOST.Cxx.Nxx.Axx
	fQX[0] = 0;

	if (D16Flag) {
		fAction = TMrbEsone::kCA_cssa;
		if (this->IsOffline()) return(1); 			// simulated by software
		stsX = cssa(fFunction[0], fCnaf[0], (Int_t *) this->GetDAddr(), NULL); // exec single cnaf, 16 bit
	} else {
		fAction = TMrbEsone::kCA_cfsa;
		if (this->IsOffline()) return(1); 			// simulated by software
		stsX = cfsa(fFunction[0], fCnaf[0], (Int_t *) this->GetDAddr(), NULL); // exec single cnaf, 24 bit
	}

	fCurNofCnafs = 1;
	if (stsX == -1) {			// X = 0, Q = 0
		fQX[0] = 0;
		fTally = -1;
	} else {
		fQX[0] = TMrbEsone::kXFlag; 					// X = 1
		if (stsX != 0) fQX[0] |= TMrbEsone::kQFlag; 	// Q = 1
		fTally = 1;
	}
	if (this->IsVerbose()) this->PrintResults("ExecCnaf", &cnaf);
	return(fTally);
}

Int_t TMrbEsone::ExecCnaf(Int_t Crate, Int_t Station, Int_t Subaddr, CamacFunction_t Function, CamacData_t Data, Bool_t D16Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::ExecCnaf
// Purpose:        Exec a single CNAF
// Arguments:      Int_t Crate               -- crate number
//                 Int_t Station             -- station
//                 Int_t Subaddr             -- subaddress
//                 CamacFunction_t Function  -- function
//                 CamacData_t Data          -- data
//                 Bool_t D16Flag            -- kTRUE if 16 bits only
// Results:        Int_t TallyCount          -- resulting number of data words
// Exceptions:
// Description:    ESONE cfsa / cssa.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t stsX;
	TMrbEsoneCNAF cnaf;

	if (!this->CheckConnection("ExecCnaf")) return(-1);

	if ((Crate = this->CheckCrate(Crate, "ExecCnaf")) == -1) return(-1);

	cnaf.Set(Crate, Station, Subaddr, Function, Data);
	if (!cnaf.CheckCnaf()) return(-1);				// check cnaf

	fFunction[0] = Function;
	if (cnaf.IsWrite()) {
		if (Data == kNoData) {
			gMrbLog->Err() << "Function F" << fFunction[0] << " (WRITE) needs DATA" << endl;
			gMrbLog->Flush(this->ClassName(), "ExecCnaf");
			return(-1);
		}
		fData[0] = Data;			// store data if WRITE
	} else fData[0] = kNoData;

	fCnaf[0] = cnaf.Int2Esone(fHostAddr);
	fQX[0] = 0;

	if (D16Flag) {
		fAction = TMrbEsone::kCA_cssa;
		if (this->IsOffline()) return(1); 				// simulated by software
		stsX = cssa(fFunction[0], fCnaf[0], (Int_t *) this->GetDAddr(), (Int_t *) this->GetQXAddr()); // exec single cnaf, 16 bit
	} else {
		fAction = TMrbEsone::kCA_cfsa;
		if (this->IsOffline()) return(1); 				// simulated by software
		stsX = cfsa(fFunction[0], fCnaf[0], (Int_t *) this->GetDAddr(), (Int_t *) this->GetQXAddr()); // exec single cnaf, 24 bit
	}

	fCurNofCnafs = 1;
	if (stsX == -1) {			// X = 0, Q = 0
		fQX[0] = 0;
		fTally = -1;
	} else {
		fQX[0] = TMrbEsone::kXFlag; 					// X = 1
		if (stsX != 0) fQX[0] |= TMrbEsone::kQFlag; 	// Q = 1
		fTally = 1;
	}
	if (this->IsVerbose()) this->PrintResults("ExecCnaf", &cnaf);
	return(fTally);
}

Int_t TMrbEsone::ExecCnafList(Int_t NofCnafs, Bool_t D16Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::ExecCnafList
// Purpose:        Exec a list of CNAFs
// Arguments:      Int_t NofCnafs    -- number of CNAFs to be executed
//                 Bool_t D16Flag    -- kTRUE if 16 bits only
// Results:        Int_t TallyCount  -- resulting number of data words
// Exceptions:
// Description:    ESONE cfga / csga.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t stsX;

	if (!this->CheckConnection("ExecCnafList")) return(-1);

//	if ((NofCnafs = this->CheckList(NofCnafs, "ExecCnafList")) == -1) return(-1);	// check consistency

	this->SetCB(NofCnafs);											// length of cnaf list
	this->ClearTally(); 											// no data
	this->ClearQX();												// clear Q & X

	if (D16Flag) {
		fAction = TMrbEsone::kCA_csga;
		if (this->IsOffline()) return(NofCnafs); 				// simulated by software
		stsX = csga((Int_t *) this->GetFAddr(), (Int_t *) this->GetCAddr(), (Int_t *) this->GetDAddr(), (Int_t *) this->GetQXAddr(), fCB);
	} else {
		fAction = TMrbEsone::kCA_cfga;
		if (this->IsOffline()) return(NofCnafs); 				// simulated by software
		stsX = cfga((Int_t *) this->GetFAddr(), (Int_t *) this->GetCAddr(), (Int_t *) this->GetDAddr(), (Int_t *) this->GetQXAddr(), fCB);
	}

	if (stsX == 0) fTally = fCB[1]; else fTally = -1;
	if (this->IsVerbose()) this->PrintResults("ExecCnafList");
	return(fTally);
}


Int_t TMrbEsone::AddressScan(const Char_t * Start, const Char_t * Stop, Int_t WordCount, Bool_t D16Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::AddressScan
// Purpose:        Exec cnafs between N(start) and N(Stop)
// Arguments:      Char_t * Start            -- C.N.F to start with
//                 Char_t * Stop             -- C.N.F to stop with
//                 Int_t WordCount           -- number of cnafs to execute / data to take
//                 Bool_t D16Flag            -- kTRUE if 16 bits only
// Results:        Int_t TallyCount          -- resulting number of data words
// Exceptions:
// Description:    ESONE cfmad / csmad.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t stsX;
	TMrbEsoneCNAF startCnaf, stopCnaf;
	Int_t crate;
	Int_t station;
	CamacFunction_t function;

	if (!this->CheckConnection("AddressScan")) return(-1);

	if (!startCnaf.Ascii2Int(Start)) return(-1);		// test if cnaf if legal
	if (!stopCnaf.Ascii2Int(Stop)) return(-1);

	crate = startCnaf.Get(TMrbEsoneCNAF::kCnafCrate);
	if ((crate = this->CheckCrate(crate, "AddressScan")) == -1) return(-1);
	if (stopCnaf.Get(TMrbEsoneCNAF::kCnafCrate) == -1) stopCnaf.Set(TMrbEsoneCNAF::kCnafCrate, crate);

	if (stopCnaf.Get(TMrbEsoneCNAF::kCnafCrate) != crate) { 	// crate must be same
		gMrbLog->Err()	<< "Crates different - start=C" << crate
						<< " != stop=C" << stopCnaf.Get(TMrbEsoneCNAF::kCnafCrate) << endl;
		gMrbLog->Flush(this->ClassName(), "AdressScan");
		return(-1);
	}

	station = startCnaf.Get(TMrbEsoneCNAF::kCnafStation);
	if (stopCnaf.Get(TMrbEsoneCNAF::kCnafStation) < station) { 	// stations must be in order
		gMrbLog->Err()	<< "Station out of order - start=N" << station
						<< " > stop=N" << stopCnaf.Get(TMrbEsoneCNAF::kCnafStation) << endl;
		gMrbLog->Flush(this->ClassName(), "AdressScan");
		return(-1);
	}

	if (startCnaf.Get(TMrbEsoneCNAF::kCnafAddr) != -1 || stopCnaf.Get(TMrbEsoneCNAF::kCnafAddr) != -1) { 	// no subaddr allowed
		gMrbLog->Err() << "Subaddresses may not be given" << endl;
		gMrbLog->Flush(this->ClassName(), "AdressScan");
		return(-1);
	}

	function = startCnaf.Get(TMrbEsoneCNAF::kCnafFunction);
	if (stopCnaf.Get(TMrbEsoneCNAF::kCnafFunction) == -1) stopCnaf.Set(TMrbEsoneCNAF::kCnafFunction, function);

	if ((CamacFunction_t) stopCnaf.Get(TMrbEsoneCNAF::kCnafFunction) != function) { 	// must be same function
		gMrbLog->Err()	<< "Function codes different - start=F" << function
						<< " != stop=F" << stopCnaf.Get(TMrbEsoneCNAF::kCnafFunction) << endl;
		gMrbLog->Flush(this->ClassName(), "AdressScan");
		return(-1);
	}

	fCnaf[0] = startCnaf.Int2Esone(fHostAddr);			// start
	fCnaf[1] = stopCnaf.Int2Esone(fHostAddr);			// stop
	fFunction[0] = function;							// camac function code
	if (WordCount == -1) WordCount = fMaxNofCnafs;
	this->SetCB(WordCount);

	if (!startCnaf.IsWrite()) this->ClearData();		// reset data buffer in case of read/control cnafs

	if (D16Flag) {
		fAction = TMrbEsone::kCA_csmad;
		if (this->IsOffline()) return(WordCount); 				// simulated by software
		stsX = csmad(fFunction[0], (Int_t *) this->GetCAddr(), (Int_t *) this->GetDAddr(), fCB);
	} else {
		fAction = TMrbEsone::kCA_cfmad;
		if (this->IsOffline()) return(WordCount); 				// simulated by software
		stsX = cfmad(fFunction[0], (Int_t *) this->GetCAddr(), (Int_t *) this->GetDAddr(), fCB);
	}
	if (stsX == 0) fTally = fCB[1]; else fTally = -1;
	if (this->IsVerbose()) this->PrintResults("AddressScan", &startCnaf, &stopCnaf);
	return(fTally);
}

Int_t TMrbEsone::AddressScan(Int_t Crate, Int_t Start, Int_t Stop, CamacFunction_t Function,
																	Int_t WordCount, Bool_t D16Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::AddressScan
// Purpose:        Exec cnafs between N(start) and N(Stop)
// Arguments:      Int_t Crate               -- crate number
//                 Int_t Start               -- station to start with
//                 Int_t Stop                -- station to stop with
//                 CamacFunction_t Function  -- function code
//                 Int_t WordCount           -- number of cnafs to execute / data to take
//                 Bool_t D16Flag            -- kTRUE if 16 bits only
// Results:        Int_t TallyCount          -- resulting number of data words
// Exceptions:
// Description:    ESONE cfmad / csmad.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t stsX;
	TMrbEsoneCNAF startCnaf, stopCnaf;

	if (!this->CheckConnection("AddressScan")) return(-1);

	if ((Crate = this->CheckCrate(Crate, "AddressScan")) == -1) return(-1);	// take default crate if not explicitly given

	startCnaf.Set(Crate, Start, 0, Function);
	stopCnaf.Set(Crate, Stop, 0, Function);

	if (Start > Stop) { 	// stations must be in order
		gMrbLog->Err() << "Station out of order - start=N" << Start << " > stop=N" << Stop << endl;
		gMrbLog->Flush(this->ClassName(), "AdressScan");
		return(-1);
	}

	if (!startCnaf.CheckCnaf(~TMrbEsoneCNAF::kCnafData)) return(-1);	// check cnafs (all but data field)
	if (!stopCnaf.CheckCnaf(~TMrbEsoneCNAF::kCnafData)) return(-1);

	fCnaf[0] = startCnaf.Int2Esone(fHostAddr);			// start
	fCnaf[1] = stopCnaf.Int2Esone(fHostAddr);			// stop
	fFunction[0] = Function;
	if (WordCount == -1) WordCount = fMaxNofCnafs;
	this->SetCB(WordCount);

	if (!startCnaf.IsWrite()) this->ClearData();		// reset data buffer in case of read/control cnafs

	if (D16Flag) {
		fAction = TMrbEsone::kCA_csmad;
		if (this->IsOffline()) return(WordCount); 			// simulated by software	
		stsX = csmad(fFunction[0], (Int_t *) this->GetCAddr(), (Int_t *) this->GetDAddr(), fCB);
	} else {
		fAction = TMrbEsone::kCA_cfmad;
		if (this->IsOffline()) return(WordCount); 			// simulated by software	
		stsX = cfmad(fFunction[0], (Int_t *) this->GetCAddr(), (Int_t *) this->GetDAddr(), fCB);
	}

	if (stsX == 0) fTally = fCB[1]; else fTally = -1;
	if (this->IsVerbose()) this->PrintResults("AddressScan", &startCnaf, &stopCnaf);
	return(fTally);
}

Int_t TMrbEsone::BlockXfer(const Char_t * Cnaf, Int_t NofCnafs, Bool_t D16Flag, Bool_t QXfer) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::BlockXfer
// Purpose:        Start block transfer
// Arguments:      Char_t * Cnaf            -- C.N.F to be executed
//                 Int_t NofCnafs           -- number of cnafs
//                 Bool_t D16Flag           -- kTRUE if 16 bits only
//                 Bool_t QXfer             -- kTRUE if Q-driven
// Results:        Int_t TallyCount         -- resulting number of data words
// Exceptions:
// Description:    ESONE cfubc / csubc / cfubr / csubr.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t stsX;
	TMrbEsoneCNAF cnaf;
	Int_t crate;
	CamacReg_t creg;
	Int_t offset, repeat;

	if (!this->CheckConnection("BlockXfer")) return(-1);

	if (!cnaf.Ascii2Int(Cnaf)) return(-1);		// test if cnaf if legal

	if ((crate = cnaf.Get(TMrbEsoneCNAF::kCnafCrate)) == -1) {							// crate number explicitly given? 
		if ((crate = this->CheckCrate(crate, "BlockXfer")) == -1) return(-1);	// no, try to take default
		cnaf.Set(TMrbEsoneCNAF::kCnafCrate, crate);
	}

	if (!cnaf.CheckCnaf(~TMrbEsoneCNAF::kCnafData)) return(-1);			// check cnaf (all but data field)

	fFunction[0] = cnaf.Get(TMrbEsoneCNAF::kCnafFunction);				// get actual function

	if (!cnaf.IsWrite()) this->ClearData(); 			// reset data buffer in case of read/control cnafs

	creg = cnaf.Int2Esone(fHostAddr);

	if (NofCnafs == -1) NofCnafs = fCurNofCnafs;		// default: current number of cnafs
	this->SetCB(NofCnafs);								// setup control block

	fTally = 0;
	offset = 0;
	repeat = (NofCnafs + kMaxRepeatCount - 1) / kMaxRepeatCount;
	for (; repeat--; offset += kMaxRepeatCount, NofCnafs -= kMaxRepeatCount) {
		if (NofCnafs > kMaxRepeatCount) this->SetCB(kMaxRepeatCount); else this->SetCB(NofCnafs);
		if (QXfer) {
			if (D16Flag) {
				fAction = TMrbEsone::kCA_csubr;
				if (this->IsOffline()) return(NofCnafs); 				// simulated by software	
				stsX = csubr(fFunction[0], creg, (Int_t *) this->GetDAddr() + offset, fCB);
			} else {
				fAction = TMrbEsone::kCA_cfubr;
				if (this->IsOffline()) return(NofCnafs); 				// simulated by software	
				stsX = cfubr(fFunction[0], creg, (Int_t *) this->GetDAddr() + offset, fCB);
			}
		} else {
			if (D16Flag) {
				fAction = TMrbEsone::kCA_csubc;
				if (this->IsOffline()) return(NofCnafs); 				// simulated by software	
				stsX = csubc(fFunction[0], creg, (Int_t *) this->GetDAddr() + offset, fCB);
			} else {
				fAction = TMrbEsone::kCA_cfubc;
				if (this->IsOffline()) return(NofCnafs); 				// simulated by software	
				stsX = cfubc(fFunction[0], creg, (Int_t *) this->GetDAddr() + offset, fCB);
			}
		}
		if (stsX == 0) fTally += fCB[1]; else fTally = -1;
		if (this->IsVerbose()) this->PrintResults("BlockXfer", &cnaf);
		if (fTally == -1) return(-1);
	}
	return(fTally);
}

Int_t TMrbEsone::BlockXfer(Int_t Crate, Int_t Station, Int_t SubAddr, CamacFunction_t Function,
										Int_t NofCnafs, Bool_t D16Flag, Bool_t QXfer) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::BlockXfer
// Purpose:        Start block transfer
// Arguments:      Int_t Crate              -- crate number
//                 Int_t Station            -- station
//                 Int_t SubAddr            -- subaddress
//                 CamacFunction_t Function -- function code
//                 Int_t NofCnafs           -- number of cnafs
//                 Bool_t D16Flag           -- kTRUE if 16 bits only
//                 Bool_t QXfer             -- kTRUE if Q-driven
// Results:        Int_t TallyCount         -- resulting number of data words
// Exceptions:
// Description:    ESONE cfubc / csubc / cfubr / csubr.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t stsX;
	TMrbEsoneCNAF cnaf;
	CamacReg_t creg;
	Int_t offset, repeat;

	if (!this->CheckConnection("BlockXfer")) return(-1);

	if ((Crate = this->CheckCrate(Crate, "BlockXfer")) == -1) return(-1);

	cnaf.Set(Crate, Station, SubAddr, Function);
	if (!cnaf.CheckCnaf(~TMrbEsoneCNAF::kCnafData)) return(-1);		// check cnaf (all but data field)

	fFunction[0] = cnaf.Get(TMrbEsoneCNAF::kCnafFunction);			// get actual function

	if (NofCnafs == -1) NofCnafs = fCurNofCnafs;					// default: current number of cnafs
	this->SetCB(NofCnafs);

	if (!cnaf.IsWrite()) this->ClearData();				// reset data buffer in case of read/control cnafs

	creg = cnaf.Int2Esone(fHostAddr);

	fTally = 0;
	offset = 0;
	repeat = (NofCnafs + kMaxRepeatCount - 1) / kMaxRepeatCount;
	for (; repeat--; offset += kMaxRepeatCount, NofCnafs -= kMaxRepeatCount) {
		if (NofCnafs > kMaxRepeatCount) this->SetCB(kMaxRepeatCount); else this->SetCB(NofCnafs);
		if (QXfer) {
			if (D16Flag) {
				fAction = TMrbEsone::kCA_csubr;
				if (this->IsOffline()) return(NofCnafs); 				// simulated by software	
				stsX = csubr(fFunction[0], creg, (Int_t *) this->GetDAddr() + offset, fCB);
			} else {
				fAction = TMrbEsone::kCA_cfubr;
				if (this->IsOffline()) return(NofCnafs); 				// simulated by software	
				stsX = cfubr(fFunction[0], creg, (Int_t *) this->GetDAddr() + offset, fCB);
			}
		} else {
			if (D16Flag) {
				fAction = TMrbEsone::kCA_csubc;
				if (this->IsOffline()) return(NofCnafs); 				// simulated by software	
//				Int_t n = (NofCnafs > kMaxRepeatCount) ? kMaxRepeatCount : NofCnafs;
//				Int_t * dp = (Int_t *) this->GetDAddr() + offset;
//				Int_t qresp = 0;
//				for (Int_t i = 0; i < n; i++, dp++) cssa(fFunction[0], creg, dp, &qresp);
//				stsX = 0;
				stsX = csubc(fFunction[0], creg, (Int_t *) this->GetDAddr() + offset, fCB);
			} else {
				fAction = TMrbEsone::kCA_cfubc;
				if (this->IsOffline()) return(NofCnafs); 				// simulated by software	
				stsX = cfubc(fFunction[0], creg, (Int_t *) this->GetDAddr() + offset, fCB);
			}
		}
		if (stsX == 0) fTally += fCB[1]; else fTally = -1;
		if (this->IsVerbose()) this->PrintResults("BlockXfer", &cnaf);
		if (fTally == -1) return(-1);
	}
	return(fTally);
}

void TMrbEsone::ResetBuffers(Int_t NofCnafs) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::ResetBuffers
// Purpose:        Define length of internal arrays
// Arguments:      Int_t NofCnafs       -- max number of cnafs
// Results:        --
// Exceptions:
// Description:    Defines buffer length and resets buffers.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fCnaf.Set(NofCnafs); fCnaf.Reset();
	fFunction.Set(NofCnafs); fFunction.Reset();
	fData.Set(NofCnafs); fData.Reset();
	fQX.Set(NofCnafs); fQX.Reset();
	fMaxNofCnafs = NofCnafs;
	fCurNofCnafs = -1;
	fTally = -1;
	fCB[0] = fCB[1] = fCB[2] = fCB[3] = 0;
}

Bool_t TMrbEsone::SetNofCnafs(Int_t NofCnafs) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::SetNofCnafs
// Purpose:        Define actual number of cnafs in list
// Arguments:      Int_t NofCnafs       -- number of cnafs
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines number of valid cnafs in list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (NofCnafs > fMaxNofCnafs) {
		gMrbLog->Err() << "Cnaf count out of range - " << NofCnafs << " (" << fMaxNofCnafs << " max)" << endl;
		gMrbLog->Flush(this->ClassName(), "SetNofCnafs");
		return(kFALSE);
	}
	fCurNofCnafs = NofCnafs;
	return(kTRUE);
}

void TMrbEsone::ClearCnaf() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::ClearCnaf
// Purpose:        Clear cnaf list
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Clears list of cnafs.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t n;

	n = (fMaxNofCnafs > kMaxNofCnafs) ? fMaxNofCnafs : kMaxNofCnafs;
	fCnaf.Set(n);
	fCnaf.Reset();
}

Bool_t TMrbEsone::SetCnaf(Int_t Index, const Char_t * Cnaf, CamacData_t Data) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::SetCnaf
// Purpose:        Set cnaf at given index
// Arguments:      Int_t Index      -- array index
//                 Char_t * Cnaf    -- C.N.A.F
//                 CamacData_t Data -- data
// Results:        kFALSE/kTRUE
// Exceptions:
// Description:    Sets a data element in cnaf list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbEsoneCNAF cnaf;

	if ((Index = this->CheckIndex(Index, "SetCnaf")) == -1) return(kFALSE);
	if (!this->CheckConnection("SetCnaf")) return(kFALSE);

	if (!cnaf.Ascii2Int(Cnaf)) return(kFALSE);
	cnaf.Set(TMrbEsoneCNAF::kCnafData, Data);
	if (!cnaf.CheckCnaf()) return(kFALSE);

	fCnaf[Index] = cnaf.Int2Esone(fHostAddr);
	fFunction[Index] = cnaf.Get(TMrbEsoneCNAF::kCnafFunction);
	fData[Index] = Data;
	fQX[Index] = 0;
	return(kTRUE);
}

Bool_t TMrbEsone::SetCnaf(Int_t Index, Int_t Crate, Int_t Station, Int_t Subaddr,
												CamacFunction_t Function, CamacData_t Data) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::SetCnaf
// Purpose:        Set cnaf at given index
// Arguments:      Int_t Index               -- array index
//                 Int_t Crate               -- crate number
//                 Int_t Station             -- station
//                 CamacFunction_t Function  -- function code
//                 CamacData_t Data          -- data
// Results:        kFALSE/kTRUE
// Exceptions:
// Description:    Sets a data element in cnaf list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbEsoneCNAF cnaf;

	if ((Index = this->CheckIndex(Index, "SetCnaf")) == -1) return(kFALSE);
	if (!this->CheckConnection("SetCnaf")) return(kFALSE);

	if (!cnaf.Set(Crate, Station, Subaddr, Function, Data)) return(kFALSE);

	fCnaf[Index] = cnaf.Int2Esone(fHostAddr);
	fFunction[Index] = cnaf.Get(TMrbEsoneCNAF::kCnafFunction);
	fData[Index] = Data;
	fQX[Index] = 0;
	return(kTRUE);
}

Bool_t TMrbEsone::GetCnaf(Int_t Index, TString & Host, TString & Cnaf, CamacData_t & Data) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::GetCnaf
// Purpose:        Get cnaf at given index
// Arguments:      Int_t Index              -- array index
//                 TString & Host            -- host name
//                 TString & Cnaf            -- cnaf (ascii representation)
//                 CamacData_t & Data        -- data
// Results:        kFALSE/kTRUE
// Exceptions:
// Description:    Returns an element of cnaf list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbEsoneCNAF cnaf;
	CamacReg_t creg;
	CamacHost_t hostAddr;
	Int_t crate;
	Int_t station;
	Int_t subaddr;
	TMrbNamedX * kp;

	if ((Index = this->CheckIndex(Index, "GetCnaf")) == -1) return(kFALSE);

	creg = fCnaf[Index];
	cgreg(creg, &hostAddr, &crate, &station, &subaddr);
	if (!cnaf.Set(crate, station, subaddr, fFunction[Index], fData[Index])) return(kFALSE);
	kp = gMrbEsoneHosts->FindByIndex((Int_t) hostAddr);
	if (kp) Host = kp->GetName(); else Host = "Unknown";
	Cnaf = cnaf.Int2Ascii(kFALSE);
	Data = fData[Index];
	return(kTRUE);
}

const Char_t * TMrbEsone::GetCnaf(Int_t Index, Bool_t DataFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::GetCnaf
// Purpose:        Get cnaf at given index
// Arguments:      Int_t Index               -- array index
// Results:        Char_t * Cnaf             -- full cnaf: HOST:Cxx.Nxx.Axx.Fxx D=xx
// Exceptions:
// Description:    Returns an element of cnaf list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbEsoneCNAF cnaf;
	CamacReg_t creg;
	CamacHost_t hostAddr;
	Int_t crate;
	Int_t station;
	Int_t subaddr;
	TMrbNamedX * kp;

	fCnafAscii.Resize(0);

	if ((Index = this->CheckIndex(Index, "GetCnaf")) == -1) return(fCnafAscii.Data());

	creg = fCnaf[Index];
	cgreg(creg, &hostAddr, &crate, &station, &subaddr);
	if (!cnaf.Set(crate, station, subaddr, fFunction[Index], fData[Index])) return(fCnafAscii.Data());
	kp = gMrbEsoneHosts->FindByIndex((Int_t) hostAddr);
	if (kp) fCnafAscii = kp->GetName(); else fCnafAscii = "Unknown";
	fCnafAscii += ":";
	fCnafAscii += cnaf.Int2Ascii(DataFlag);
	return(fCnafAscii.Data());
}

TMrbNamedX * TMrbEsone::GetCnafType(Int_t Index) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::GetCnafType
// Purpose:        Get cnaf type at given index
// Arguments:      Int_t Index               -- array index
// Results:        EMrbCnafType type         -- cnaf type
// Exceptions:
// Description:    Returns cnaf type.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	CamacFunction_t fct;
	TMrbEsoneCNAF::EMrbCnafType type;

	if ((Index = this->CheckIndex(Index, "GetCnafType")) == -1) return(NULL);
	fct = fFunction[Index];
	if (IS_F_READ(fct)) 			type = TMrbEsoneCNAF::kCnafTypeRead;
	else if (IS_F_WRITE(fct)) 		type = TMrbEsoneCNAF::kCnafTypeWrite;
	else if (IS_F_CNTL(fct)) {
		if (fct == 8 || fct == 27)	type = TMrbEsoneCNAF::kCnafTypeReadStatus;
		else						type = TMrbEsoneCNAF::kCnafTypeControl;
	} else return(NULL);

	return(fLofCNAFTypes.FindByIndex((Int_t) type));
}

void TMrbEsone::ClearData() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::ClearData
// Purpose:        Clear data buffer
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Clears data buffer.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t n;

	n = (fMaxNofCnafs > kMaxNofCnafs) ? fMaxNofCnafs : kMaxNofCnafs;
	fData.Set(n);
	fData.Reset();
}

Bool_t TMrbEsone::SetData(Int_t Index, CamacData_t Data) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::SetData
// Purpose:        Set data at given index
// Arguments:      Int_t Index        -- array index
//                 CamacData_t Data   -- data
// Results:        kFALSE/kTRUE
// Exceptions:
// Description:    Sets a data element in the data buffer.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if ((Index = this->CheckIndex(Index, "SetData")) == -1) return(kFALSE);
	fData[Index] = Data;
	return(kTRUE);
}

Bool_t TMrbEsone::SetData(CamacDArray & Data, Int_t NofData) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::SetData
// Purpose:        Set data from external array
// Arguments:      CamacDArray & Data    -- ext data array
//                 Int_t NofData         -- number of data points
// Results:        kFALSE/kTRUE
// Exceptions:
// Description:    Sets data buffer from external array.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if ((NofData = this->CheckSize(NofData, "SetData")) == -1) return(kFALSE);
	fCurNofCnafs = NofData;
	fData.Set(NofData);
	fData.Reset();
	for (Int_t i = 0; i < NofData; i++) fData[i] = Data[i];
	return(kTRUE);
}

Bool_t TMrbEsone::SetData(CamacData_t * Data, Int_t NofData) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::SetData
// Purpose:        Set data from external array
// Arguments:      CamacData_t * Data    -- ext data array
//                 Int_t NofData         -- number of data points
// Results:        kFALSE/kTRUE
// Exceptions:
// Description:    Sets data buffer from external array.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if ((NofData = this->CheckSize(NofData, "SetData")) == -1) return(kFALSE);
	fCurNofCnafs = NofData;
	fData.Set(NofData);
	fData.Reset();
	for (Int_t i = 0; i < NofData; i++) fData[i] = *Data++;
	return(kTRUE);
}

Bool_t TMrbEsone::SetData(UShort_t * Data, Int_t NofData) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::SetData
// Purpose:        Set data from external array
// Arguments:      UShort_t * Data       -- ext data array (16 bit)
//                 Int_t NofData         -- number of data points
// Results:        kFALSE/kTRUE
// Exceptions:
// Description:    Sets data buffer from external array.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if ((NofData = this->CheckSize(NofData, "SetData")) == -1) return(kFALSE);
	fCurNofCnafs = NofData;
	fData.Set(NofData);
	fData.Reset();
	for (Int_t i = 0; i < NofData; i++) fData[i] = (CamacData_t) *Data++;
	return(kTRUE);
}

CamacData_t TMrbEsone::GetData(Int_t Index) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::GetData
// Purpose:        Get data at a given index
// Arguments:      Int_t Index          -- data index
// Results:        CamacData_t Data     -- data
// Exceptions:
// Description:    Returns data stored at a given index.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if ((Index = this->CheckIndex(Index, "GetData")) == -1) return(kNoData);
	return(fData[Index]);
}

void TMrbEsone::ClearFunction() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::ClearFunction
// Purpose:        Clear function list
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Clears function list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t n;

	n = (fMaxNofCnafs > kMaxNofCnafs) ? fMaxNofCnafs : kMaxNofCnafs;
	fFunction.Set(n);
	fFunction.Reset();
}

Bool_t TMrbEsone::SetFunction(Int_t Index, CamacFunction_t Function) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::SetFunction
// Purpose:        Set data at given index
// Arguments:      Int_t Index              -- array index
//                 CamacFunction_t Function -- data
// Results:        kFALSE/kTRUE
// Exceptions:
// Description:    Sets an element in the function list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if ((Index = this->CheckIndex(Index, "SetFunction")) == -1) return(kFALSE);
	if (Function < 0 || Function > 31) {
		gMrbLog->Err() << "Illegal function code - " << Function << " (should be in [0,31])" << endl;
		gMrbLog->Flush(this->ClassName(), "SetFunction");
		return(kFALSE);
	}
	fFunction[Index] = Function;
	return(kTRUE);
}

CamacFunction_t TMrbEsone::GetFunction(Int_t Index) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::GetFunction
// Purpose:        Get function code at given index
// Arguments:      Int_t Index                  -- data index
// Results:        CamacFunction_t Function     -- function code
// Exceptions:
// Description:    Returns function code stored at a given index.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if ((Index = this->CheckIndex(Index, "GetFunction")) == -1) return(kNoData);
	return(fFunction[Index]);
}

void TMrbEsone::ClearQX() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::ClearQX
// Purpose:        Clear Q/X buffer
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Clears Q/X buffer.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t n;

	n = (fMaxNofCnafs > kMaxNofCnafs) ? fMaxNofCnafs : kMaxNofCnafs;
	fQX.Set(n);
	fQX.Reset();
}

Bool_t TMrbEsone::GetQ(Int_t Index) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::GetQ
// Purpose:        Get Q flag at given index
// Arguments:      Int_t Index         -- index
// Results:        Bool_t QFlag        -- Q flag
// Exceptions:
// Description:    Returns Q flag (bit 0) stored at a given index.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if ((Index = this->CheckIndex(Index, "GetQ")) == -1) return(kFALSE);
	return((fQX[Index] & 1) == 1);
}

Bool_t TMrbEsone::GetX(Int_t Index) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::GetX
// Purpose:        Get X flag at given index
// Arguments:      Int_t Index         -- index
// Results:        Bool_t XFlag        -- X flag
// Exceptions:
// Description:    Returns X flag (bit 1) stored at a given index.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if ((Index = this->CheckIndex(Index, "GetX")) == -1) return(kFALSE);
	return((fQX[Index] & 2) == 2);
}

CamacStatus_t TMrbEsone::GetStatus(Bool_t & XFlag, Bool_t & QFlag, CamacError_t & ErrorCode, TString & Error) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::GetStatus
// Purpose:        Get ESONE status of prev camac action
// Arguments:      Bool_t & XFlag         -- CAMAC X
//                 Bool_t & QFlag         -- CAMAC Q
//                 CamacError_t ErrorCode -- error code
//                 TString & Error        -- error message
// Results:        CamacStatus_t Status   -- status word
// Exceptions:
// Description:    Reads and decodes the ESONE status register.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	CamacStatus_t status;

	ctstat((Int_t *) &status);
	switch (status & 0x3) {
		case 0x0:	XFlag = kTRUE; QFlag = kTRUE; break;
		case 0x1:	XFlag = kTRUE; QFlag = kFALSE; break;
		case 0x2:	XFlag = kFALSE; QFlag = kTRUE; break;
		case 0x3:	XFlag = kFALSE; QFlag = kFALSE; break;
	}
	fErrorCode = status >> 2;
	fError.Resize(0);
	ErrorCode = fErrorCode;
	if (fErrorCode != 0) {
		fError = cerror(fErrorCode, 0);
		Error = fError;
	} else {
		fError.Resize(0);
		Error.Resize(0);
	}
	return(status);
}

const Char_t * TMrbEsone::GetError() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::GetError
// Purpose:        Get error of prev cnaf action
// Arguments:      --
// Results:        Char_t * Error    -- error string
// Exceptions:
// Description:    Returns an error message.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	CamacStatus_t status;

	ctstat((Int_t *) &status);
	fErrorCode = status >> 2;
	fError.Resize(0);
	if (fErrorCode != 0) {
		fError = cerror(fErrorCode, 0);
		return(fError.Data());
	} else return("");
}

TMrbNamedX * TMrbEsone::GetHost(CamacHost_t HostAddr) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::GetHost
// Purpose:        Get host by ESONE address
// Arguments:      CamacHost_t HostAddr   -- ESONE address
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

void TMrbEsone::Print(Int_t From, Int_t To) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::Print
// Purpose:        Print cnafs & data
// Arguments:      Int_t From            -- start
//                 Int_t To              -- stop
// Results:        --
// Exceptions:
// Description:    Outputs cnafs, data, flags.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	TMrbNamedX * action;
	const Char_t * XQ;
	UInt_t data;
	Int_t tally;
	TMrbNamedX * ctype;
	UInt_t ct;

	action = this->GetAction();

	if (action->GetIndex() == TMrbEsone::kCANone) {
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

	switch (action->GetIndex()) {
		case TMrbEsone::kCA_cfsa:
		case TMrbEsone::kCA_cssa:
				cout	<< " " << setiosflags(ios::left)
						<< setw(25) << this->GetCnaf(-1, kFALSE);
				cout	<< setw(5) << this->GetCnafType()->GetName();
				XQ = this->GetX()? "X" : "-"; cout	<< resetiosflags(ios::left) << setw(10) << XQ;
				XQ = this->GetQ()? "Q" : "-"; cout	<< setw(3) << XQ;
				data = this->GetData();
				if (data != kNoData) {
					cout	<< setw(10) << setbase(10) << data;
					cout	<< "    0" << setbase(8) << setiosflags(ios::left) << data;
					cout	<< "    0x" << setbase(16) << setiosflags(ios::left) << data;
				}
				cout	<< setbase(10) << endl;
				break;

		case TMrbEsone::kCA_cfga:
		case TMrbEsone::kCA_csga:
				for (i = 0; i < this->GetTally(); i++) {
					cout	<< " " << setiosflags(ios::left)
							<< setw(25) << this->GetCnaf(i, kFALSE);
					cout	<< setw(5) << this->GetCnafType(i)->GetName();
					XQ = this->GetX(i)? "X" : "-"; cout	<< resetiosflags(ios::left) << setw(10) << XQ;
					XQ = this->GetQ(i)? "Q" : "-"; cout	<< setw(3) << XQ;
					data = this->GetData(i);
					if (data != kNoData) {
						cout	<< setw(10) << setbase(10) << data;
						cout	<< "    0" << setbase(8) << setiosflags(ios::left) << data;
						cout	<< "    0x" << setbase(16) << setiosflags(ios::left) << data;
					}
					cout	<< setbase(10) << endl;
				}
				break;

		case TMrbEsone::kCA_cfubc:
		case TMrbEsone::kCA_csubc:
		case TMrbEsone::kCA_cfubr:
		case TMrbEsone::kCA_csubr:
				ctype = this->GetCnafType(0);
				ct = ctype->GetIndex();
				cout	<< " " << setiosflags(ios::left)
						<< setw(25) << this->GetCnaf(0, kFALSE);
				cout	<< setw(5) << ctype->GetName();
				XQ = this->GetX(0)? "X" : "-"; cout << resetiosflags(ios::left) << setw(10) << XQ;
				XQ = this->GetQ(0)? "Q" : "-"; cout	<< setw(3) << XQ << endl;
				if (ct == TMrbEsoneCNAF::kCnafTypeRead || ct == TMrbEsoneCNAF::kCnafTypeReadStatus) {
					tally = this->GetTally();
					if (!this->SetNofCnafs(tally)) return;
					for (i = 0; i < tally; i++) {
						data = this->GetData(i);
						if (data != kNoData) {
							cout	<< resetiosflags(ios::left) << setw(34) << i << ": "
									<< setw(18) << setbase(10) << data;
							cout	<< "    0" << setbase(8) << setiosflags(ios::left) << data;
							cout	<< "    0x" << setbase(16) << setiosflags(ios::left) << data;
						}
						cout	<< setbase(10) << endl;
					}
				}
				break;

		case TMrbEsone::kCA_cfmad:
		case TMrbEsone::kCA_csmad:
				ctype = this->GetCnafType(0);
				ct = ctype->GetIndex();
				cout	<< " " << setiosflags(ios::left)
						<< setw(25) << this->GetCnaf(0, kFALSE);
				cout	<< setw(5) << ctype->GetName() << " [START]" << endl;
				ctype = this->GetCnafType(1);
				cout	<< " " << setiosflags(ios::left)
						<< setw(25) << this->GetCnaf(1, kFALSE);
				cout	<< setw(5) << ctype->GetName() << " [STOP]" << endl;
				if (ct == TMrbEsoneCNAF::kCnafTypeRead || ct == TMrbEsoneCNAF::kCnafTypeReadStatus) {
					tally = this->GetTally();
					if (!this->SetNofCnafs(tally)) return;
					for (i = 0; i < tally; i++) {
						data = this->GetData(i);
						if (data != kNoData) {
							cout	<< resetiosflags(ios::left) << setw(34) << i << ": "
									<< setw(18) << setbase(10) << data;
							cout	<< "    0" << setbase(8) << setiosflags(ios::left) << data;
							cout	<< "    0x" << setbase(16) << setiosflags(ios::left) << data;
						}
						cout	<< setbase(10) << endl;
					}
				}
				break;

		default:	break;
	}
	cout	<< "-------------------------------------------------------------------------------------------"
			<< endl << endl;
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
	Char_t errBuf[256];

	if (EsoneCall != NULL) {
		strcpy(errBuf, cerror(-1, 0));
		gMrbLog->Err() << "Error in ESONE fct \"" << EsoneCall << "\" - " << errBuf << endl;
		gMrbLog->Flush(this->ClassName(), Method);
	} else {
		error = this->GetError();
		if (!error.IsNull()) {
			gMrbLog->Err() << "ESONE error - " << error << endl;
			gMrbLog->Flush(this->ClassName(), Method);
		}
	}
}

void TMrbEsone::PrintStatus() {
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
	Bool_t XFlag;
	Bool_t QFlag;
	CamacStatus_t status;
	CamacError_t ErrorCode;
	TString ErrorMsg;

	action = this->GetAction();

	if (action->GetIndex() == TMrbEsone::kCANone) {
		gMrbLog->Err() << "No CAMAC action" << endl;
		gMrbLog->Flush(this->ClassName(), "PrintStatus");
		return;
	}

	cout	<< endl << this->ClassName() << "::PrintStatus(): Current CAMAC Status -" << endl
			<< "-----------------------------------------------------------------------------" << endl
			<< "    CAMAC action           : " << action->GetName() << endl;

	status = this->GetStatus(XFlag, QFlag, ErrorCode, ErrorMsg);

	cout	<< "    X Response             : " << (XFlag ? "yes" : "no") << endl;
	cout	<< "    Q Response             : " << (QFlag ? "yes" : "no") << endl;
	cout	<< "    Error Code             : " << ErrorCode << endl;
	if (ErrorCode > 0) {
		cout	<< "    Error                  : " << cerror(ErrorCode, 0) << endl;
	}
	cout	<< "-----------------------------------------------------------------------------" << endl;
}

void TMrbEsone::PrintResults(const Char_t * Method, TMrbEsoneCNAF * c1, TMrbEsoneCNAF * c2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::PrintResults
// Purpose:        Print results
// Arguments:      Char_t * Method         -- calling method
//                 TMrbEsoneCNAF * c1      -- cnaf 1
//                 TMrbEsoneCNAF * c2      -- cnaf 2
// Results:        --
// Exceptions:
// Description:    Outputs cnaf results.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;

	kp = fLofCamacActions.FindByIndex((Int_t) fAction);
	cout	<< setmagenta
			<< this->ClassName() << "::" << Method << "(): " << kp->GetName()
			<< setblack << endl;

	switch (fAction) {
		case kCA_cfsa:
		case kCA_cssa:
			kp = fLofCNAFTypes.FindByIndex((Int_t) c1->GetType());
			cout	<< setmagenta
					<< "                           CNAF=" << c1->Int2Ascii() << " (" << kp->GetName() << ")"
					<< ", X=" << (this->GetX() ? 1 : 0)
					<< ", Q=" << (this->GetQ() ? 1 : 0)
					<< ", WC(in)=1, WC(out)=" << (Int_t) fTally
					<< setblack << endl;
			break;		
		case kCA_cfga:
		case kCA_csga:
		case kCA_cfmad:
		case kCA_csmad:
			break;
		case kCA_cfubc:
		case kCA_csubc:
		case kCA_cfubr:
		case kCA_csubr:
			kp = fLofCNAFTypes.FindByIndex((Int_t) c1->GetType());
			cout	<< setmagenta
					<< "                           CNAF=" << c1->Int2Ascii() << " (" << kp->GetName() << ")"
					<< ", X=" << (this->GetX(0) ? 1 : 0)
					<< ", Q=" << (this->GetQ(0) ? 1 : 0)
					<< ", WC(in)=" << fCB[0] << ", WC(out)=" << (Int_t) fTally
					<< setblack << endl;
			break;
	}
}

Int_t TMrbEsone::HasFastCamac() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::HasFastCamac
// Purpose:        Test for fast camac capability
// Arguments:      --
// Results:        Int_t FastCamacFct   -- function F to be used for
//                                         fast camac read
// Exceptions:     F = -1 -> no fast camac capability
// Description:    Returns function F to be used for fast camac read.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	switch (fController.GetIndex()) {
		case kCC_CC32:
			return(kFastCamac_F_CC32);
		case kCC_CBV:
		default:
			return(-1);
	}
}

Int_t TMrbEsone::HasBroadCast() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::HasBroadCast
// Purpose:        Test for broadcast capability
// Arguments:      --
// Results:        Int_t BroadCastStation   -- station N to be used for
//                                             broadcasting
// Exceptions:     N = -1 -> no broad cast capability
// Description:    Returns station N to be used for broadcasting.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fUseBroadCast) {
		switch (fController.GetIndex()) {
			case kCC_CC32:
				return(kBroadCast_N_CC32);
			default:
				return(-1);
		}
	} else return(-1);
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
	if ((Crate = this->CheckCrate(Crate, "SetBroadCast")) == -1) return(kFALSE);

	if (this->HasBroadCast() > 0) {
		sts = this->ExecCnaf(Crate, 26, 0, 16, (Int_t) BroadCast);
		return(sts == 1);
	} else {
		gMrbLog->Err() << "No BroadCasting for CAMAC controller " << fController.GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "SetBroadCast");
		return(kFALSE);
	}
}

UInt_t TMrbEsone::GetBroadCast() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::SetBroadCast
// Purpose:        Read broadcast register
// Arguments:      --
// Results:        UInt_t BroadCast     -- bits on bc mask
// Exceptions:
// Description:    Reads the broadcast mask register.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t sts;
	
	if (!this->CheckConnection("GetBroadCast")) return(0);

	if (this->HasBroadCast() > 0) {
		sts = this->ExecCnaf(fCrate, 26, 0, 0);
		if (sts == 1) return(this->GetData()); else return(0);
	} else {
		gMrbLog->Err() << "No BroadCasting for CAMAC controller " << fController.GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "GetBroadCast");
		return(0);
	}
}

Bool_t TMrbEsone::AddBroadCast(Int_t Station) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::AddBroadCast
// Purpose:        Add station to broadcast mask
// Arguments:      Int_t Station      -- camac station
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Adds a bit to the broadcast mask register.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t sts;
	UInt_t mask;
	
	if (!this->CheckConnection("AddBroadCast")) return(kFALSE);

	if (this->HasBroadCast() > 0) {
		if (Station < 1 || Station > 24) {
			gMrbLog->Err() << "Illegal camac station - " << Station << " (should be in [1, 24])" << endl;
			gMrbLog->Flush(this->ClassName(), "AddBroadCast");
			return(kFALSE);
		}
		sts = this->ExecCnaf(fCrate, 26, 0, 0);
		if (sts != 1) return(kFALSE);
		mask = (UInt_t) this->GetData() | (1 << (Station - 1));			
		sts = this->ExecCnaf(fCrate, 26, 0, 16, (Int_t) mask);
		return(sts == 1);
	} else {
		gMrbLog->Err() << "No BroadCasting for CAMAC controller " << fController.GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "AddBroadCast");
		return(kFALSE);
	}
}

Bool_t TMrbEsone::SetAutoRead(Bool_t AutoRead) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::SetAutoRead
// Purpose:        Enable/disable AutoRead mode
// Arguments:      Bool_t AutoRead   -- on/off flag
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Enables/disable auto read mode (CC32 only).
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t sts;
	
	if (!this->CheckConnection("SetAutoRead")) return(kFALSE);

	switch (fController.GetIndex()) {
		case kCC_CC32:
			sts = this->ExecCnaf(fCrate, 0, 0, 3);
			if (sts != 1) return(kFALSE);
			sts = this->GetData();
			if (AutoRead) sts |= kAutoRead_CC32; else sts &= ~kAutoRead_CC32;
			sts = this->ExecCnaf(fCrate, 0, 0, 3, sts);
			return(sts == 1);
		case kCC_CBV:
		default:
			gMrbLog->Err() << "No AutoRead mode for CAMAC controller " << fController.GetName() << endl;
			gMrbLog->Flush(this->ClassName(), "SetAutoRead");
			return(kFALSE);
	}
}

Bool_t TMrbEsone::ReadDoubleWord(Bool_t ReadDW) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEsone::ReadDoubleWord
// Purpose:        Enable/disable 32 bit mode
// Arguments:      Bool_t ReadDW   -- on/off flag
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Enables/disable 32 bit mode (CC32 only).
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t sts;
	
	if (!this->CheckConnection("ReadDoubleWord")) return(kFALSE);

	switch (fController.GetIndex()) {
		case kCC_CC32:
			sts = this->ExecCnaf(fCrate, 27, ReadDW ? 2 : 3, 16, 0);
			return(sts == 1);
		case kCC_CBV:
		default:
			gMrbLog->Err() << "No 32 bit mode for CAMAC controller " << fController.GetName() << endl;
			gMrbLog->Flush(this->ClassName(), "ReadDoubleWord");
			return(kFALSE);
	}
}

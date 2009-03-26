//__________________________________________________________________[C++ MAIN]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDA0816Exec
// Purpose:        DDA0816 Execution
// Description:    Opens i/o to DDA0816 device, reads scan data from file
//                 and starts scan accordingly.
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Date:           
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <time.h>

#include "TROOT.h"
#include "TRint.h"
#include "TSystem.h"
#include "TUnixSystem.h"
#include "TString.h"
#include "TApplication.h"
#include "TEnv.h"
#include "TFile.h"
#include "TServerSocket.h"
#include "TMessage.h"
#include "TSocket.h"
#include "TMonitor.h"
#include "TStopwatch.h"

#include "TSnkDDA0816.h"
#include "TSnkDDAMessage.h"
#include "TMrbLogger.h"
#include "TMrbSystem.h"

#include "SetColor.h"

#include "dda0816_P.h"
#include "dda0816_protos.h"

#include <errno.h>

extern TMrbLogger * gMrbLog;

extern void InitGui();

TString execMode;
Int_t scanPeriod;
TString workingDir;
TString dataFile;
TString progName;

static TSnkDDA0816 * DDA = NULL;
static Int_t dacX = -1;
static Int_t dacY = -1;
static Int_t dacH = -1;
static Int_t dacB = -1;

static TSocket * sock0 = NULL;
static TSocket * sock1 = NULL;
static TSnkDDAMessage ddaMsg;

static struct dda0816_curve cBufferX, cBufferY, cBufferH, cBufferB;

Bool_t verboseMode = kFALSE;
Bool_t debugMode = kFALSE;
Bool_t offlineMode = kFALSE;

Bool_t scanInProgress = kFALSE;

Bool_t DDAStartScan(const Char_t * ScanFile, Double_t Interval, Int_t NofCycles);
Bool_t DDAStopScan();
Bool_t DDAOpen();
void DDAFillBuffer(TSnkDDA0816 * DDA, Int_t Channel, struct dda0816_curve * Buffer, Bool_t Verbose);

TSnkDDAMessage * DDAListen(TMonitor * Mon);

VoidFuncPtr_t initfuncs[] = { InitGui, 0 };

TROOT root("DDAExec","Scanning with SNAKE", initfuncs);

int main(int argc, char **argv) {
//______________________________________________________________[MAIN PROGRAM]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAExec
// Purpose:        Operate DDA0816 device
//////////////////////////////////////////////////////////////////////////////

	TString path;
	Int_t port;
	TMrbSystem ux;
	TSnkDDAMessage * mp;
	Double_t stopAt;
	TMrbLofNamedX lofActions;
	TMrbLofNamedX lofExecModes;
	TMrbNamedX * nx;

	TStopwatch stopWatch;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger("ddaControl.log", "DDAExec");
	
	ux.GetBaseName(progName, argv[0]);

	lofActions.AddNamedX(kSnkLofActions);
	lofExecModes.AddNamedX(kSnkLofExecModes);
	lofExecModes.SetPatternMode();

	if(argc != 2) {
		cerr	<< setred
				<< endl
				<<	"Usage: " << progName << " port" << endl
				<< endl
				<<	"       ARGUMENT     MEANING" << endl
				<<	"       port         port number to be used for TCP connection"
				<< setblack <<	endl << endl;
		exit(0);
	}

	new TApplication("DDAExec", &argc, argv);
	gROOT->Reset();

	cBufferX.points = (Int_t *) calloc(1, sizeof(Int_t) * DDA08_MAXPOINTS);
	cBufferY.points = (Int_t *) calloc(1, sizeof(Int_t) * DDA08_MAXPOINTS);
	cBufferH.points = (Int_t *) calloc(1, sizeof(Int_t) * DDA08_MAXPOINTS);
	cBufferB.points = (Int_t *) calloc(1, sizeof(Int_t) * DDA08_MAXPOINTS);

	port = atoi(argv[1]);
	TServerSocket * ss = new TServerSocket(port, kTRUE);
	TMonitor * mon = new TMonitor;
	mon->Add(ss);
	cout << progName << ": Listening for connections ..." << endl;
	while (1) {
		mp = DDAListen(mon);
		if (mp != NULL) {
			UInt_t eMode = lofExecModes.FindPattern(mp->fExecMode);
			verboseMode = (eMode & kSnkEMVerbose) != 0;
			debugMode = (eMode & kSnkEMDebug) != 0;
			offlineMode = (eMode & kSnkEMOffline) != 0;
			if (verboseMode) {
				gMrbLog->Out() << "Setting VERBOSE mode" << endl;
				gMrbLog->Flush("DDAExec", "", setred);
			}
			if (debugMode) {
				gMrbLog->Out() << "Setting DEBUG mode" << endl;
				gMrbLog->Flush("DDAExec", "", setred);
			}
			if (offlineMode) {
				gMrbLog->Out() << "Setting OFFLINE mode" << endl;
				gMrbLog->Flush("DDAExec", "", setred);
			}
			nx = lofActions.FindByName(mp->fAction, TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase);
			ESnkAction action = nx ? (ESnkAction) nx->GetIndex() : kSnkActionUndef;
			switch (action) {
				case kSnkActionStartScan:
					if (scanInProgress) {
						gMrbLog->Err()	<< "Scan in progress - STOP command required" << endl;
						gMrbLog->Flush("DDAExec");
					} else {
						path = mp->fPath;
						path += "/";
						path += mp->fSaveScan;
						if (mp->fInterval < .001) stopAt = 1000000.;
						else					stopAt = mp->fInterval;
						DDAStartScan(path, mp->fInterval, mp->fCycles);
						stopWatch.Start();
						stopAt += stopWatch.RealTime();
						scanInProgress = kTRUE;
					}
					break;
				case kSnkActionStopScan:
					if (scanInProgress) {
						DDAStopScan();
						stopWatch.Stop();
					}
					scanInProgress = kFALSE;
					break;
				case kSnkActionKillServer:
					gMrbLog->Out()	<< "Got SERVER KILL command - bye." << endl;
					gMrbLog->Flush("DDAExec", setred);
					gApplication->Terminate(0);
				default:
					gMrbLog->Err()	<< "Action unknown - " << mp->fAction << endl;
					gMrbLog->Flush("DDAExec");
					break;
			}
		} else if (scanInProgress && (dda0816_GetPacer(dacX) == 0 || (stopWatch.RealTime() >= stopAt))) {
			DDAStopScan();
			stopWatch.Stop();
			scanInProgress = kFALSE;
		} else stopWatch.Start(kFALSE);
	}
}

TSnkDDAMessage * DDAListen(TMonitor * Mon) {
//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAListen
// Purpose:        Wait for socket messages
// Arguments:      TMonitor * Mon        -- monitor
// Results:        TSnkDDAMessage * Msg  -- message object
// Exceptions:     
// Description:    Listens for messages.
// Keywords:       
/////////////////////////////////////////////////////////////////////////////

	TMessage *mess;
	TSocket  *s;

	if (sock0 == NULL && sock1 == NULL) {
		s = Mon->Select();
	} else {
		s = Mon->Select(100);
		if (s == (TSocket *) -1) return(NULL);
	}

	if (s->IsA() == TServerSocket::Class()) {
		if (!sock0) {
			sock0 = ((TServerSocket *)s)->Accept();
			sock0->Send("Connection #0 established");
			gMrbLog->Out()	<< "Connection #0 established" << endl;
			gMrbLog->Flush("DDAListen");
			Mon->Add(sock0);
			return(NULL);
		} else if (!sock1) {
			sock1 = ((TServerSocket *)s)->Accept();
			sock1->Send("Connection #1 established");
			gMrbLog->Out()	<< "Connection #1 established" << endl;
			gMrbLog->Flush("DDAListen");
			Mon->Add(sock1);
			return(NULL);
		} else {
			gMrbLog->Err()	<< "Too many connections - max 2" << endl;
			gMrbLog->Flush(progName.Data());
			return(NULL);
		}
	} else if (s->IsA() == TSocket::Class()) {
		if (s->Recv(mess) > 0) {
			if (mess->What() == kMESS_STRING) {
				Char_t str[256];
				mess->ReadString(str, 256);
				TString mstr = str;
				if (ddaMsg.FromString(mstr)) {
					delete mess;
					if (debugMode) ddaMsg.Print();
					return(&ddaMsg);
				} else {
					gMrbLog->Out()	<< "Got message: " << str << endl;
					gMrbLog->Flush("DDAListen");
					return(NULL);
				}
			} else if (mess->What() == kMESS_OBJECT) {
				TObject * obj = mess->ReadObject(mess->GetClass());
				delete mess;
				if (obj->IsA() == TSnkDDAMessage::Class()) {
					if (debugMode) ((TSnkDDAMessage *) obj)->Print();
					return((TSnkDDAMessage *) obj);
				} else {
					gMrbLog->Out()	<< "Got object \"" << obj->ClassName() << "\":" << endl;
					gMrbLog->Flush("DDAListen");
					obj->Dump();
					return(NULL);
				}
			}
		} else {
			if (s == sock0) {
				Mon->Remove(sock0);
				sock0->Close();
				delete sock0;
				sock0 = NULL;
				gMrbLog->Out()	<< "Closing connection #0" << endl;
				gMrbLog->Flush("DDAListen");
				return(NULL);
			} else if (s == sock1) {
				Mon->Remove(sock1);
				sock1->Close();
				delete sock1;
				sock1 = NULL;
				gMrbLog->Out()	<< "Closing connection #1" << endl;
				gMrbLog->Flush("DDAListen");
				return(NULL);
			} else {
				if (sock0) {
					Mon->Remove(sock0);
					sock0->Close();
					delete sock0;
					sock0 = NULL;
				}
				if (sock1) {
					Mon->Remove(sock1);
					sock1->Close();
					delete sock1;
					sock1 = NULL;
				}
				gMrbLog->Out()	<< "Closing TCP connections" << endl;
				gMrbLog->Flush("DDAListen");				
				return(NULL);
			}
		}
	}
	return(NULL);
}

Bool_t DDAStartScan(const Char_t * ScanFile, Double_t ScanInterval, Int_t NofCycles) {
//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAStartScan
// Purpose:        Start actual scan
// Arguments:      Char_t * ScanFile      -- file containing scan data
//                 Double_t ScanInterval  -- scan period
//                 Int_t NofCycles        -- number of cycles to perform
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Opens DDA channels, writes scan data, and starts scan.
// Keywords:       
/////////////////////////////////////////////////////////////////////////////

	TFile * ddaFile;

	ddaFile = new TFile(ScanFile);
	if (!ddaFile->IsOpen()) {
		gMrbLog->Err()	<< "Can't open ROOT file - " << ScanFile << endl;
		gMrbLog->Flush("DDAStartScan");
		return(kFALSE);
	}

	DDA = (TSnkDDA0816 *) ddaFile->Get("DDA0816");
	if (DDA == NULL || DDA->IsA() != TSnkDDA0816::Class()) {
		gMrbLog->Err()	<< "File " << ScanFile << " doesn't contain a TSnkDDA0816 object" << endl; 
		gMrbLog->Flush("DDAStartScan");
		return(kFALSE);
	}

	if (offlineMode) {
		gMrbLog->Out()	<< "Running in OFFLINE mode, no access to DDA0816" << endl;
		gMrbLog->Flush("DDAStartScan", "", setgreen);
		if (ScanInterval < .001) {
			if (NofCycles > 0) {
				gMrbLog->Out()	<< "Starting scan for " << NofCycles << "cycle(s)" << endl;
				gMrbLog->Flush("DDAStartScan", "", setgreen);
			} else {
				gMrbLog->Out()	<< "Starting scan, waiting until STOP command" << endl;
				gMrbLog->Flush("DDAStartScan", "", setgreen);
			}
		} else {
			gMrbLog->Out()	<< "Starting scan, scan period = " << ScanInterval << " seconds" << endl;
			gMrbLog->Flush("DDAStartScan", "", setgreen);
		}
		gMrbLog->Out()	<< "Pacer clock            : "	<< DDA->GetPacerClock() << endl
						<< "Cycles                 : "	<< NofCycles << endl
						<< "Prescaler              : "	<< DDA->GetPreScaler()->GetName() << " ("
														<< setbase(16) << setiosflags(ios::showbase)
														<< DDA->GetPreScaler()->GetIndex()
														<< setbase(10) << resetiosflags(ios::showbase) << ")"
														<< endl
						<< "Clock source           : "	<< DDA->GetClockSource()->GetName() << " ("
														<< setbase(16) << setiosflags(ios::showbase)
														<< DDA->GetClockSource()->GetIndex()
														<< setbase(10) << resetiosflags(ios::showbase) << ")"
														<< endl;
		gMrbLog->Flush();

		DDAFillBuffer(DDA, kDacX, &cBufferX, kTRUE);
		DDAFillBuffer(DDA, kDacY, &cBufferY, kTRUE);
		DDAFillBuffer(DDA, kDacH, &cBufferH, kTRUE);
		DDAFillBuffer(DDA, kDacB, &cBufferB, kTRUE);
	} else {
		if (!DDAOpen()) return(kFALSE);

		dda0816_SetUpdateGroup(dacX);
		dda0816_SetUpdateGroup(dacY);
		dda0816_SetUpdateGroup(dacH);
	
		Int_t preScale = DDA->GetPreScaler()->GetIndex();
		Int_t pacerClock = DDA->GetPacerClock() - 1;
		if (preScale == kSnkPSExt) {
			preScale = 0;
			pacerClock = 0;
		}
		dda0816_SetPacerClock(dacX, pacerClock);
		dda0816_SetPreScaler(dacX, preScale);

		dda0816_SetOutputClockGeneration(dacX);
		dda0816_SetClockSource(dacX, DDA->GetClockSource()->GetIndex());

		if (debugMode) DDA->Print();

		DDAFillBuffer(DDA, kDacX, &cBufferX, debugMode);
		dda0816_SetCurve(dacX, cBufferX);
	
		DDAFillBuffer(DDA, kDacY, &cBufferY, debugMode);
		dda0816_SetCurve(dacY, cBufferY);
	
		DDAFillBuffer(DDA, kDacH, &cBufferH, debugMode);
		dda0816_SetCurve(dacH, cBufferH);
	
		DDAFillBuffer(DDA, kDacB, &cBufferB, debugMode);
		dda0816_SetCurve(dacB, cBufferB);
		if (ScanInterval < .001) {
			if (NofCycles > 0) {
				gMrbLog->Out()	<< "Starting scan for " << NofCycles << "cycle(s)" << endl;
				gMrbLog->Flush("DDAStartScan", "", setgreen);
				if (DDA->GetSoftScale(1) > 0) { 		// set cycle count for slow channel only
					dda0816_SetCycle(dacY, NofCycles);
					dda0816_SetCycle(dacX, 0);
				} else {
					dda0816_SetCycle(dacX, NofCycles);
					dda0816_SetCycle(dacY, 0);
				}
			} else {
				gMrbLog->Out()	<< "Starting scan, waiting until STOP command" << endl;
				gMrbLog->Flush("DDAStartScan", "", setgreen);
				dda0816_SetCycle(dacX, 0);		// infinite number of cycles
				dda0816_SetCycle(dacY, 0);
			}
		} else {
			gMrbLog->Out()	<< "Starting scan, scan period = " << ScanInterval << " seconds" << endl;
			gMrbLog->Flush("DDAStartScan", "", setgreen);
			dda0816_SetCycle(dacX, 0);			// infinite number of cycles
			dda0816_SetCycle(dacY, 0);
		}

		dda0816_SetCycle(dacH, 0);				// H and B are always =0 (infinity)
		dda0816_SetCycle(dacB, 0);

		dda0816_StartPacer(dacX);
	}
	return(kTRUE);
}

Bool_t DDAStopScan() {
//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAStartScan
// Purpose:        Start actual scan
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Opens DDA channels, writes scan data, and starts scan.
// Keywords:       
/////////////////////////////////////////////////////////////////////////////

	Double_t stopX, stopY;
	Int_t stopXS, stopYS;

	if (DDA == NULL) {
		gMrbLog->Err()	<< "No TSnkDDA0816 object (start failed?)" << endl; 
		gMrbLog->Flush("DDAStopScan");
		return(kFALSE);
	}

	if (debugMode) DDA->Print();

	stopX = DDA->GetDac(kDacX)->GetStopPos();
	stopY = DDA->GetDac(kDacY)->GetStopPos();
	stopXS = DDA->Dist2Ampl(kDacX, stopX) + DDA->GetDac(kDacX)->GetPos0();
	stopYS = DDA->Dist2Ampl(kDacY, stopY) + DDA->GetDac(kDacY)->GetPos0();

	if (offlineMode) {
		cout	<< setred
				<< progName << "::DDAStopScan(): Running in OFFLINE mode, no access to DDA0816" << endl;
		cout	<< setred
				<< progName << "::DDAStopScan(): Resetting to X=" << stopX << ", Y=" << stopY
				<< " (" << stopXS << "," << stopYS << ")"
				<< setblack << endl;
	} else {
		if (!DDAOpen()) return(kFALSE);

		dda0816_StopPacer(dacX);
	
		gMrbLog->Out()	<< progName << "::DDAStartScan(): Resetting to X=" << stopX << ", Y=" << stopY
						<< " (" << stopXS << "," << stopYS << ")" << endl;
		gMrbLog->Flush("DDAStopScan", "", setred);

		cBufferX.size = 1;
		cBufferX.points[0]= stopXS;
		cBufferX.SoftScale = 0;
		dda0816_SetCurve(dacX, cBufferX);

		cBufferX.size = 1;
		cBufferX.points[0]= stopYS;
		cBufferX.SoftScale = 0;
		dda0816_SetCurve(dacY, cBufferX);

		cBufferX.size = 1;
		cBufferX.points[0]= kSnkHystLow;
		cBufferX.SoftScale = 0;
		dda0816_SetCurve(dacH, cBufferX);

		cBufferX.size = 1;
		cBufferX.points[0]= kSnkBeamOff;
		cBufferX.SoftScale = 0;
		dda0816_SetCurve(dacB, cBufferX);

		dda0816_StartPacer(dacX);
		sleep (1);
		dda0816_StopPacer(dacX);
	}
	return(kTRUE);
}

Bool_t DDAOpen() {
//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAOpen
// Purpose:        Open DAC channels
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Opens DDA channels.
// Keywords:       
/////////////////////////////////////////////////////////////////////////////

	const Char_t * deviceName;

	if (dacX == -1) {
		deviceName = DDA->GetDac(kDacX)->GetDeviceName();
		dacX = open(deviceName, O_RDWR);
	}
	if (dacX == -1) {
		gMrbLog->Err() << gSystem->GetError() << " - " << deviceName << endl;
		gMrbLog->Flush("DDAOpen");
		return(kFALSE);
	}

	if (dacY == -1) {
		deviceName = DDA->GetDac(kDacY)->GetDeviceName();
		dacY = open(deviceName, O_RDWR);
	}
	if (dacY == -1) {
		gMrbLog->Err() << gSystem->GetError() << " - " << deviceName << endl;
		gMrbLog->Flush("DDAOpen");
		return(kFALSE);
	}

	if (dacH == -1) {
		deviceName = DDA->GetDac(kDacH)->GetDeviceName();
		dacH = open(deviceName, O_RDWR);
	}
	if (dacH == -1) {
		gMrbLog->Err() << gSystem->GetError() << " - " << deviceName << endl;
		gMrbLog->Flush("DDAOpen");
		return(kFALSE);
	}

	if (dacB == -1) {
		deviceName = DDA->GetDac(kDacB)->GetDeviceName();
		dacB = open(deviceName, O_RDWR);
	}
	if (dacB == -1) {
		gMrbLog->Err() << gSystem->GetError() << " - " << deviceName << endl;
		gMrbLog->Flush("DDAOpen");
		return(kFALSE);
	}
	return(kTRUE);
}

void DDAFillBuffer(TSnkDDA0816 * DDA, Int_t Channel, struct dda0816_curve * Buffer, Bool_t Verbose) {
//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAFillBuffer
// Purpose:        Fill scan buffer
// Arguments:      TSnkDDA0816 * DDA     -- scan settings
//                 Int_t Channel         -- channel
//                 dda0816_curve Buffer  -- buffer
//                 Bool_t Verbose        -- verbose flag
// Results:        --
// Exceptions:     
// Description:    Fills scan buffer
// Keywords:       
/////////////////////////////////////////////////////////////////////////////

	Int_t i;
	TSnkDDAChannel * ddaChn;
	Int_t * cdp;
	Int_t size;
	ofstream out;
	TString dataFile;
	Bool_t ofok;
			
	ddaChn = DDA->GetDac(Channel);
	cdp = ddaChn->GetCurveData();
	ofok = kFALSE;

	if (Verbose) {
		dataFile = "ddaData.chn";
		dataFile += Channel;
		dataFile += ".dat";
		out.open(dataFile.Data(), ios::out);
		cout	<< "Channel " << Channel << ":" << endl
				<< "   SoftScale            : " << ddaChn->GetSoftScale() << endl
				<< "   Number of pixels     : " << ddaChn->GetNofPixels() << endl
				<< "   Data written to file : " << dataFile
				<< endl;
		if (out.good()) {
			out	<< "Channel " << Channel << ":" << endl
				<< "   SoftScale           : " << ddaChn->GetSoftScale() << endl
				<< "   Number of pixels    : " << ddaChn->GetNofPixels() << endl;
				ofok = kTRUE;
		} else {
			gMrbLog->Err() << gSystem->GetError() << " - " << dataFile << endl;
			gMrbLog->Flush("DDAExec");
		}
	}
	Buffer->SoftScale = ddaChn->GetSoftScale();
	size = ddaChn->GetNofPixels();
	if (Channel <= kDacY) {
		if (DDA->GetScanMode() == kSnkSMLShape) {
			size = 2 * (DDA->GetDac(kDacX)->GetNofPixels() + DDA->GetDac(kDacY)->GetNofPixels());
		} else if (ddaChn->GetScanProfile() == kSnkSPTriangle) {
			size *= 2;
		}
	}
	Buffer->size = size;
	for (i = 0; i < Buffer->size; i++, cdp++) {
		Buffer->points[i] = *cdp;
		if (ofok) {
			if (i == 0) out << "   CurveData      :" << flush;
			if ((i % 10) == 0) out << endl << "           " << flush;
			out << setw(7) << *cdp << flush;
		}
	}
	if (ofok) {
		out << endl;
		out.close();
	}
}

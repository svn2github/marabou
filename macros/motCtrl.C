#include <iostream>
#include <iomanip>

#include "TROOT.h"
#include "TTimer.h"

#include "TMrbLogger.h"
#include "TMbsSetup.h"
#include "TMbsControl.h"

#include "motor.h"

//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             motCtrl.C
// Purpose:          MARABOU session controlled by stepping motor
// Author:           Rudi Lutter
// Revision:         
// Date:             Nov 2013
// Keywords:
//////////////////////////////////////////////////////////////////////////////

const char setred[]      =   "\033[31m";
const char setblue[]     =   "\033[34m";
const char setblack[]    =   "\033[39m";

Bool_t setupDone = kFALSE;
Bool_t daqRunning = kFALSE;
Bool_t motionInitDone = kFALSE;

TTimer * motTimer = NULL;
Motor * motor = NULL;

TMrbLogger * gMrbLog = NULL;
TMbsSetup * gMbsSetup = NULL;
TMbsControl * gMbsCtrl = NULL;

TString ppcName;
TString ppcPath;

struct motBase {
	Int_t origX;
	Int_t origZ;
	Double_t calibX;
	Double_t calibZ;
	Int_t speedX;
	Int_t speedZ;
	Double_t startX;
	Double_t startZ;
	Double_t deltaX;
	Double_t deltaZ;
	Int_t stepsX;
	Int_t stepsZ;
	Double_t curX;
	Double_t curZ;
	Int_t curStepX;
	Int_t curStepZ;
	Int_t nofSecs;
};

struct motBase mb;

Double_t getX() {
	if (!motionInitDone) {
		gMrbLog->Err() << "Calibration not done - call initMotion() first" << endl;
		gMrbLog->Flush("motCtrl.C", "getX");
		return -1.;
	}
	Int_t posX = motor->posX();
	Double_t x = (mb.origX - posX) / mb.calibX;
	return x;
}

Double_t getZ() {
	if (!motionInitDone) {
		gMrbLog->Err() << "Calibration not done - call initMotion() first" << endl;
		gMrbLog->Flush("motCtrl.C", "getZ");
		return -1.;
	}
	Int_t posZ = motor->posZ();
	Double_t z = (mb.origZ - posZ) / mb.calibZ;
	return z;
}

Bool_t moveTo(Double_t X, Double_t Z) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           moveTo()
// Purpose:        Move to (calibrated) position (X,Z)
// Arguments:      Double_t X          -- position X
//                 Double_t Z          -- ... Z
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Moves motor.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!motionInitDone) {
		gMrbLog->Err() << "Calibration not done - call initMotion() first" << endl;
		gMrbLog->Flush("motCtrl.C", "moveTo");
		return kFALSE;
	}

	Int_t posX = (Int_t) (mb.origX - mb.calibX * X);
	Int_t posZ = (Int_t) (mb.origZ - mb.calibZ * Z);
	motor->moveAbsolute(posX, 0, posZ);
	return kTRUE;
}

Bool_t atPosition() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           atPosition()
// Purpose:        Output current (calibrated) position to stdout
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Outputs position and stores values in database.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!motionInitDone) {
		gMrbLog->Err() << "Calibration not done - call initMotion() first" << endl;
		gMrbLog->Flush("motCtrl.C", "atPosition");
		return kFALSE;
	}

	mb.curX = getX();
	mb.curZ = getZ();
	gMrbLog->Out() << "Motor now at X=" << mb.curX << "mm, Z=" << mb.curZ << "mm (steps: " << motor->posX() << "," << motor->posZ() << ")" << endl;
	gMrbLog->Flush("motCtrl.C", "atPosition", setblue);
	return kTRUE;
}

Bool_t initMotion(Int_t OrigX, Int_t OrigZ, Double_t CalibX, Double_t CalibZ, Int_t SpeedX, Int_t SpeedZ) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           initMotion()
// Purpose:        Initialize stepping motor
// Arguments:      Int_t OrigX         -- origin X
//                 Int_t OrigZ         -- ... Z
//                 Double_t CalibX     -- calibration X
//                 Double_t CalibZ     -- ... Z
//                 Int_t SpeedX        -- speed X
//                 Int_t SpeedZ        -- ... Z
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Initializes motor and preforms reference run.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (motor == NULL) motor = new Motor();

	motor->referenceRun();

	mb.origX = OrigX;
	mb.origZ = OrigZ;
	mb.calibX = CalibX;
	mb.calibZ = CalibZ;
	mb.speedX = SpeedX;
	mb.speedZ = SpeedZ;
	motor->setSpeed(SpeedX, 0, SpeedZ);
	mb.stepsX = 0;
	mb.stepsZ = 0;
	mb.curStepX = 0;
	mb.curStepZ = 0;
	motionInitDone = kTRUE;
	moveTo(0, 0);
	atPosition();
	return kTRUE;
}

Bool_t motCtrl() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           motCtrl()
// Purpose:        Start motCtrl session
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Initialization
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t ok = kTRUE;

	cout << endl << setblue << "motCtrl.C: MARABOU data acquisition controlled by stepping motor" << setblack << endl;
	cout << "           Type \"usage()\" for help" << endl << endl;

	gMrbLog = new TMrbLogger("motCtrl.log");

	ppcName = gEnv->GetValue("TMbsSetup.EventBuilder.Name", "");
	if (ppcName.IsNull()) {
		gMrbLog->Err() << "Name of PPC missing: Please define \"TMbsSetup.EventBuilder.Name\" in .rootrc or call setupMbs(Char_t * PPC, Char_t * WorkingDir)" << endl;
		gMrbLog->Flush("motCtrl.C", "motCtrl");
		ok = kFALSE;
	}
	ppcPath = gEnv->GetValue("TMbsSetup.WorkingDir", "");
	if (ppcPath.IsNull()) {
		gMrbLog->Err() << "Working directory missing: Please define \"TMbsSetup.WorkingDir\" in .rootrc or call setupMbs(Char_t * PPC, Char_t * WorkingDir)" << endl;
		gMrbLog->Flush("motCtrl.C", "motCtrl");
		ok = kFALSE;
	}

	if (ok) {
		gMbsSetup = new TMbsSetup(".mbssetup");
		TString mbsVersion = gEnv->GetValue("TMbsSetup.MbsVersion", "v45");
		gMbsCtrl = new TMbsControl(ppcName.Data(), "S", kFALSE, mbsVersion.Data(), ppcPath.Data());
		setupDone = kTRUE;
	}

	if (motTimer == NULL) motTimer = new TTimer(0, "continueScan()");

	if (!ok) cout << setred << "           Call setupMbs() to define PPC and/or working dir!" << setblack << endl << endl;

	cout << "[Don't forget to call initMotion() to calibrate motor]" << endl;
	return ok;
}

Bool_t setupMbs(Char_t * PPC = "", Char_t * WorkingDir = "") {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           setupMbs()
// Purpose:        Setup MBS
// Arguments:      Char_t * PPC        -- name of PPC to be used
//                 Char_t * WorkingDir -- working dir relative to PPC's home
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Performs MBS setup (as does C_analyze)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString pn = PPC;
	if (!pn.IsNull()) ppcName = pn;
	if (ppcName.IsNull()) ppcName = gEnv->GetValue("TMbsSetup.EventBuilder.Name", "");
	if (ppcName.IsNull()) {
		gMrbLog->Err() << "Name of PPC missing: Please define \"TMbsSetup.EventBuilder.Name\" in .rootrc" << endl;
		gMrbLog->Flush("motCtrl.C", "setupMbs");
		return kFALSE;
	}
	
	TString rshCmd = "rsh ";
	rshCmd += ppcName;
	rshCmd += " pwd 2>&1";
	FILE * pwd = gSystem->OpenPipe(rshCmd, "r");
	Char_t pstr[1024];
	TString homeDir;
	while (fgets(pstr, 1024, pwd)) {
		homeDir = pstr;
		homeDir = homeDir.Strip(TString::kTrailing, '\n');
		homeDir = homeDir.Strip(TString::kBoth);
		if (homeDir[0] == '/') break;
	}
	gSystem->ClosePipe(pwd);
	if (homeDir[0] != '/') {
		gMrbLog->Err() << "Can't reach " << ppcName << "'s home" << endl;
		gMrbLog->Flush("motCtrl.C", "setupMbs");
		return kFALSE;
	}
	
	TString pp = WorkingDir;
	if (!pp.IsNull()) ppcPath = pp;
	if (ppcPath.IsNull()) ppcPath = gEnv->GetValue("TMbsSetup.WorkingDir", "");
	if (ppcPath.IsNull()) {
		gMrbLog->Err() << "Working directory missing: Please define \"TMbsSetup.WorkingDir\" in .rootrc" << endl;
		gMrbLog->Flush("motCtrl.C", "setupMbs");
		return kFALSE;
	}
	
	gMbsSetup->EvtBuilder()->SetProcName(ppcName.Data());
	gMbsSetup->ReadoutProc(0)->SetProcName(ppcName.Data());
	gMbsSetup->SetPath(ppcPath.Data());
	gMbsSetup->SetNofReadouts(1);
	gMbsSetup->SetMode((EMbsSetupMode) 1);
	gMbsSetup->ReadoutProc(0)->TriggerModule()->SetType((EMbsTriggerModuleType) 2);
	
	if (gMbsCtrl == NULL) {
		TString mbsVersion = gEnv->GetValue("TMbsSetup.MbsVersion", "v45");
		gMbsCtrl = new TMbsControl(ppcName.Data(), "S", kFALSE, mbsVersion.Data(), ppcPath.Data());
	}
	
	Bool_t ok = gMbsSetup->MakeSetupFiles();
	if (ok) {
		gMbsSetup->Save();
		setupDone = kTRUE;
	}
	return kTRUE;
}

Bool_t clearMbs() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           clearMbs()
// Purpose:        Setup MBS
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Performs MBS setup (as does C_analyze)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!setupDone) {
		gMrbLog->Err() << "Setup not done - call setupMbs() first" << endl;
		gMrbLog->Flush("motCtrl.C", "clearMbs");
		return kFALSE;
	}
	
	Int_t nb = gMbsCtrl->GetNofMbsProcs();
	if (nb < 0) {
		gMrbLog->Err() << "Can't reach " << ppcName << " - check settings" << endl;
		gMrbLog->Flush("motCtrl.C", "clearMbs");
		return kFALSE;
	} else if(nb > 1000){
		gMrbLog->Err() << "You are not owner of " << ppcName << " - reboot may be necessary" << endl;
		gMrbLog->Flush("motCtrl.C", "clearMbs");
		return kFALSE;
	} else if (nb > 0) {
		TString rshCmd = "rsh ";
		rshCmd += ppcName;
		rshCmd += " \"ps ax | grep m_\" 2>&1";
		FILE * psax = gSystem->OpenPipe(rshCmd, "r");
		Char_t pstr[1024];
		TString killCmd = "kill -9 ";
		while (fgets(pstr, 1024, psax)) {
			TString procId = pstr;
			procId = procId.Strip(TString::kTrailing, '\n');
			procId = procId.Strip(TString::kBoth);
			TObjArray * parr = procId.Tokenize(" ");
			procId = ((TObjString *) parr->At(0))->GetString();
			if (procId.IsDigit()) {
				killCmd += procId;
				killCmd += " ";
			}
		}
		gSystem->ClosePipe(psax);
		gMrbLog->Out() << "Killing MBS processes: " << killCmd << endl;
		gMrbLog->Flush("motCtrl.C", "clearMbs", setblue);
		rshCmd = "rsh ";
		rshCmd += ppcName;
		rshCmd += " \"";
		rshCmd += killCmd;
		rshCmd += "\"";
		gSystem->Exec(rshCmd.Data());
		
		gMbsCtrl->StopMbs();
		gMrbLog->Out() << "Checking if there are still MBS processes alive" << endl;
		gMrbLog->Flush("motCtrl.C", "clearMbs");
		if (gMbsCtrl->GetNofMbsProcs() > 0) {
			gMrbLog->Err() << "There are still MBS processes running" << endl;
			gMrbLog->Flush("motCtrl.C", "clearMbs");
			gMrbLog->Err() << "You may repeat clearMbs(), but " << ppcName << " may need a reboot" << endl;
			gMrbLog->Flush("motCtrl.C", "clearMbs");
			return kFALSE;
		} else {
			gMrbLog->Out() << "Ok, all MBS processes disappeared" << endl;
			gMrbLog->Flush("motCtrl.C", "clearMbs", setblue);
		}
	} else {
		gMrbLog->Out() << "No MBS processes were running" << endl;
		gMrbLog->Flush("motCtrl.C", "clearMbs", setblue);
	}
	return kTRUE;
}

Bool_t startMbs() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           startMbs()
// Purpose:        Exec @startup
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Performs MBS startup
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!setupDone) {
		gMrbLog->Err() << "Setup not done - call \"setupMbs()\" first" << endl;
		gMrbLog->Flush("motCtrl.C", "clearMbs");
		return kFALSE;
	}
	
	if (gMbsCtrl->GetNofMbsProcs() > 0) {
		gMrbLog->Err() << "There are still MBS processes running - call \"clearMbs()\" first" << endl;
		gMrbLog->Flush("motCtrl.C", "clearMbs");
		return kFALSE;
	}
	if (!gMbsCtrl->StartMbs()) return kFALSE;
	if (!gMbsCtrl->Startup()) return kFALSE;
	return kTRUE;
}
	
Bool_t startDaq() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           startDaq()
// Purpose:        Start DAQ
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Starts M_analyze, connects to MBS and starts DAQ
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t verbose = gEnv->GetValue("MotCtrl.VerboseMode", kFALSE);
	
	Int_t sx = (Int_t) getX();
	Int_t sz = (Int_t) getZ();
	TString runId = Form("%02d%02d", sx, sz);
	TString ofmt = gEnv->GetValue("MotCtrl.OutputFile", "run-X%02d-Z%02d.root");
	TString hfmt = gEnv->GetValue("MotCtrl.HistoFile", "histo-X%02d-Z%02d.root");
	TString runCmd = "./M_analyze ";
	runCmd += ppcName;
	runCmd += " S 0 0 ";
	runCmd += runId;
	runCmd += " 1 ";
	runCmd += Form(ofmt.Data(), sx, sz);
	runCmd += " 0 none ";
	runCmd += Form(hfmt.Data(), sx, sz);
	runCmd += " none 0 9090 0 &";
	if (verbose) {
		gMrbLog->Out() << "Starting M_analyze: " << runCmd << endl;
		gMrbLog->Flush("motCtrl.C", "startDAQ", setblue);
	}
	
	TString pidFile = "/tmp/M_analyze_";
	pidFile += gSystem->Getenv("USER");
	pidFile += ".9090";
	if (!gSystem->AccessPathName(pidFile.Data())) {
		gMrbLog->Err() << "M_analyze already running - call \"killAnalyze()\" first" << endl;
		gMrbLog->Flush("motCtrl.C", "startDAQ");
		return kFALSE;
	}
	ifstream pf;
	Bool_t pfx = kFALSE;
	Int_t timeout = 50;
	cout << setblue << "motCtrl.C:Waiting for M_analyze to start ." << ends;
	gSystem->Exec(runCmd);
	while (!pfx && timeout--) {
		cout << "." << ends;
		gSystem->ProcessEvents();
		pf.open(pidFile.Data(), ios::in);
		if (pfx = pf.good()) break;
		pf.close();
		pfx = kFALSE;
		gSystem->Sleep(1000);
	}
	cout << setblack << endl;
	if (!pfx) {
		gMrbLog->Err() << "M_analyze seems to be dead" << endl;
		gMrbLog->Flush("motCtrl.C", "startDAQ");
		return kFALSE;
	}
	gMrbLog->Out() << "M_analyze running, starting DAQ" << endl;
	gMrbLog->Flush("motCtrl.C", "startDAQ", setblue);

	gMbsCtrl->StartAcquisition();
	daqRunning = kTRUE;

	return daqRunning;
}	
	
Bool_t killAnalyze() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           killAnalyze()
// Purpose:        Kill existing instance of M_analyze
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Kills M_analyze connected to port 9090
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString pidFile = "/tmp/M_analyze_";
	pidFile += gSystem->Getenv("USER");
	pidFile += ".9090";
	ifstream pf(pidFile.Data(), ios::in);
	if (pf.good()) {
		TString pid;
		pf >> pid;
		TString killCmd = "kill -9 ";
		killCmd += pid;
		killCmd += " 2>/dev/null";
		gSystem->Exec(killCmd.Data());
		gSystem->Unlink(pidFile.Data());
		gMrbLog->Out() << "M_analyze with pid " << pid << " killed, pid file " << pidFile << " deleted" << endl;
		gMrbLog->Flush("motCtrl.C", "killAnalyze", setblue);
		return kTRUE;
	} else {
		gMrbLog->Err() << "M_analyze not running - no such file \"" << pidFile << "\"" << endl;
		gMrbLog->Flush("motCtrl.C", "killAnalyze");
		return kFALSE;
	}
}

Bool_t stopDaq() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           stopDaq()
// Purpose:        Stop DAQ
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Stops acquisition
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (daqRunning) {
		gMbsCtrl->StopAcquisition();
		TString pidFile = "/tmp/M_analyze_";
		pidFile += gSystem->Getenv("USER");
		pidFile += ".9090";
		Int_t timeout = 15;
		cout << setblue << "motCtrl: Waiting for M_analyze to stop ." << ends;
		for (;timeout--;) {
			cout << "." << ends;
			gSystem->ProcessEvents();
			if (gSystem->AccessPathName(pidFile.Data())) break;
			gSystem->Sleep(1000);
		}
		cout << endl;
		if (!gSystem->AccessPathName(pidFile.Data())) {
			gMrbLog->Err() << "M_analyze still running - file \"" << pidFile << "\"" << endl;
			gMrbLog->Flush("motCtrl.C", "stopDaq");
			return kFALSE;
		}
		daqRunning = kFALSE;
		gMbsCtrl->InitMbs();
		return kTRUE;
	} else {
		cout << setred << "DAQ not running" << setblack << endl;
		return kFALSE;
	}
}

void tryToStart(Int_t Try = 5) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           tryToStart()
// Purpose:        Perform clear/start in a loop
// Arguments:      Int_t Try     -- number of tries
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Try to start MBS.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	killAnalyze();
	Bool_t ok = kFALSE;
	for (; Try--;) {
		clearMbs();
		if (startMbs()) { ok = kTRUE; break; }
	}
	if (!ok) {
		gMrbLog->Err() << "Can't start MBS - maybe a \"reboot\" of ppc will help ..." << endl;
		gMrbLog->Flush("motCtrl.C", "tryToStart");
	}
}

Bool_t doScan(Int_t StartX, Int_t StartZ, Int_t DeltaX, Int_t DeltaZ, Int_t StepsX, Int_t StepsZ, Int_t NofSecs) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           doScan()
// Purpose:        Perform a XZ scan
// Arguments:      Int_t StartX     -- starting point X
//                 Int_t StartZ     -- ... Z
//                 Int_t DeltaX     -- increment X
//                 Int_t DeltaZ     -- ... Z
//                 Int_t StepsX     -- number of steps in X
//                 Int_t StepsZ         -- ... Z
//                 Int_t NofSecs    -- number of seconds to run
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Moves to (StartX,StartZ), sets timer, calls startDaq().
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!motionInitDone) {
		gMrbLog->Err() << "Calibration not done - call initMotion() first" << endl;
		gMrbLog->Flush("motCtrl.C", "atPosition");
		return kFALSE;
	}

	moveTo(StartX, StartZ);
	mb.startX = StartX;
	mb.startZ = StartZ;
	mb.curX = StartX;
	mb.curZ = StartZ;
	mb.deltaX = DeltaX;
	mb.deltaZ = DeltaZ;
	mb.stepsX = StepsX;
	mb.stepsZ = StepsZ;
	mb.curStepX = 0;
	mb.curStepZ = 0;
	mb.nofSecs = NofSecs;

	if (!startDaq()) return kFALSE;
	atPosition();
	motTimer->SetCommand("continueScan()");
	motTimer->Start(NofSecs * 1000, kTRUE);
	TDatime dstart;
	gMrbLog->Out() << "Timer start at: " << dstart.AsString() << endl;
	gMrbLog->Flush("motCtrl.C", "doScan", setblue);
	return kTRUE;
}

Bool_t continueScan() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           continueScan()
// Purpose:        Continue with next step
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Called on timer interrupts
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (daqRunning) {
		TDatime dstop;
		gMrbLog->Out() << "Timer stop at: " << dstop.AsString() << endl;
		gMrbLog->Flush("motCtrl.C", "continueScan", setblue);
		if (!stopDaq()) {
			gMrbLog->Err() << "Aborting scan" << endl;
			gMrbLog->Flush("motCtrl.C", "continueScan");
			return kFALSE;
		}
		mb.curX += mb.deltaX;
		mb.curStepX++;
		if (mb.curStepX >= mb.stepsX) {
			mb.curX = mb.startX;
			mb.curStepX = 0;
			mb.curZ += mb.deltaZ;
			mb.curStepZ++;
			if (mb.curStepZ >= mb.stepsZ) {
				atPosition();
				gMrbLog->Out() << "End of measurement" << endl;
				gMrbLog->Flush("motCtrl.C", "continueScan", setblue);
				return kTRUE;
			}
		}
		moveTo(mb.curX, mb.curZ);
		atPosition();
		if (!startDaq()) {
			gMrbLog->Err() << "DAQ not running - giving up" << endl;
			gMrbLog->Flush("motCtrl.C", "continueScan");
			return kFALSE;
		}
		motTimer->Start(mb.nofSecs * 1000, kTRUE);
		TDatime dstart;
		gMrbLog->Out() << "Timer start at: " << dstart.AsString() << endl;
		gMrbLog->Flush("motCtrl.C", "continueScan", setblue);
	}
	return kTRUE;
}

Bool_t abortScan() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           abortScan()
// Purpose:        Abort scan in progress
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	motTimer->Stop();
	stopDaq();
	atPosition();
	gMrbLog->Out() << "Aborting scan" << endl;
	gMrbLog->Flush("motCtrl.C", "abortScan", setblue);
	return kTRUE;
}

Bool_t mkReadout() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           mkReadout()
// Purpose:        Compile readout
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Compiles readout task under MBS
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!setupDone) {
		gMrbLog->Err() << "Setup not done - call \"setupMbs()\" first" << endl;
		gMrbLog->Flush("motCtrl.C", "mkReadout");
		return kFALSE;
	}
	
	ofstream of;
	of.open("ppc.mk", ios::out);
	of << "#!/bin/tcsh" << endl;
	of << "source /etc/csh.login" << endl;
	of << "source .login" << endl;
	TString path = gMbsSetup->GetPath();
	of << "cd " << path << endl;
	of << "cp ../*Readout.mk ." << endl;
	of << "make -f *Readout.mk clean all" << endl;
	of.close();
	gSystem->Exec("chmod +x ppc.mk");
	
	TString cmd = "rsh ";
	cmd += ppcName;
	cmd += " ";
	cmd += gSystem->WorkingDirectory();
	cmd += "/ppc.mk";
	gSystem->Exec(cmd);
	return kTRUE;
}

Bool_t mkAnalyze() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           mkAnalyze()
// Purpose:        Compile root/linux part of analysis
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Builds M_analyze and its library
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!setupDone) {
		gMrbLog->Err() << "Setup not done - call \"setupMbs()\" first" << endl;
		gMrbLog->Flush("motCtrl.C", "mkReadout");
		return kFALSE;
	}
	
	TString cmd = "make -f *Analyze.mk clean all";
	gSystem->Exec(cmd);
	return kTRUE;
}

void usage() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           usage()
// Purpose:        Print usage info
// Arguments:      --
// Results:        --
// Description:    Prints some comments on how to use motCtrl.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	cout << endl << setblue << "motCtrl.C: data acquisition controlled by stepping motor" << setblack << endl << endl;
	cout << "Start: (once at the beginning)" << endl;
        cout << "       export CPLUS_INCLUDE_PATH=.:$MARABOU/include:$ROOTSYS/include" << endl;
	cout << "       cp $MARABOU/macros/motLibs.C ." << endl;
	cout << "       cp $MARABOU/macros/motCtrl.C ." << endl;
	cout << "       (on every restart)" << endl;
	cout << "       root motLibs.C motCtrl.C+" << endl << endl;
	cout << "Commands:" << endl;
	cout << "   setupMbs(Char_t * PPC, Char_t * WorkingDir)  -- perform MBS setup" << endl;
	cout << "                                                   (takes PPC and WorkingDir from .rootrc if not given)" << endl;
	cout << "   clearMbs()                                   -- clear MBS: stop all tasks" << endl;
	cout << "   mkAnalyze()                                  -- compile and link M_analyze" << endl;
	cout << "   mkReadout()                                  -- compile and link m_read_meb" << endl;
	cout << "   startMbs()                                   -- perform @startup" << endl;
	cout << "   startDaq()                                   -- start acquisition" << endl;
	cout << "   stopDaq()                                    -- stop acquisition" << endl << endl;
	cout << "   killAnalyze()                                -- kill an existing instance of M_analyze still running" << endl << endl;
        cout << "   initMotion(Int_t OrigX, Int_t OrigZ, Double_t CalibX, Double_t CalibZ, Int_t SpeedX, Int_t SpeedZ) -- initialize motor" << endl;
	cout << "   doScan(Int_t X0, Int_t Z0, Int_t DeltaX, Int_t DeltaZ, Int_t NX, Int_t NZ, Int_t NofSecs) -- start scan" << endl;
	cout << "   abortScan()                                  -- abort scan in progress" << endl;
	cout << "   moveTo(Double_t X, Double_t Z)               -- move to position (X,Z) in mm" << endl;
	cout << "   atPosition()                                 -- output current position" << endl;
	cout << endl << endl;
	cout << "Motor control (intrinsic functions):" << endl;
	cout << "   motor->activeAxes()                                 -- show active axes" << endl;
	cout << "   motor->posX(), motor->posZ()                        -- show current position" << endl;
	cout << "   motor->speedX(), motor->speedZ()                    -- show current speed" << endl;
	cout << "   motor->minPosX(), motor->minPosZ(), motor->maxPosX(), motor->maxPosZ()   -- show limits" << endl;
	cout << "   motor->setSpeed(Int_t SX, Int_t SY, Int_t SZ);      -- set speed (default 100)" << endl;
	cout << "   motor->moveAbsolute(Int_t X, Int_t Y, Int_t Z);     -- move to given (X,Z)" << endl;
	cout << "   motor->moveRelative(Int_t DX, Int_t DY, Int_t DZ);  -- move on to (X+DX,Z+DZ)" << endl;
	cout << "   motor->setMinPositions(Int_t X, Int_t Y, Int_t Z);  -- set min position (default 0)" << endl;
	cout << "   motor->setMaxPositions(Int_t X, Int_t Y, Int_t Z);  -- set max position (default 6600/0/8000)" << endl;
	cout << "   motor->setActiveAxes(AXES activeAxes);              -- define axes (default XZ)" << endl;
	cout << "   motor->referenceRun();                              -- perform a reference run" << endl;
	cout << endl;
}


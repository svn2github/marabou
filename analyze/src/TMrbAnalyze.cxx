//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze.cxx
// Purpose:        Implement class methods for user's analysis
// Description:    This code defines class methods for data analysis
//                 common to all user applications
//
// Header files:   TMrbAnalyze.h     -- class definitions
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbAnalyze.cxx,v 1.92 2009-12-01 13:31:10 Rudolf.Lutter Exp $
// Date:
//////////////////////////////////////////////////////////////////////////////

#include "TEnv.h"
#include "TOrdCollection.h"
#include "TRegexp.h"
#include "TObjString.h"
#include "THashList.h"
#include "TMath.h"

#include "TMrbAnalyze.h"
#include "TUsrEvtStart.h"
#include "TUsrEvtStop.h"
#include "TUsrDeadTime.h"

#include "TMrbString.h"
#include "TMrbLofUserVars.h"
#include "TMrbTransport.h"

#include "SetColor.h"

// global pthread mutex to protect TMapped data
extern pthread_mutex_t global_data_mutex;

extern void PutPid(TMrbAnalyze::EMrbRunStatus);	// run-status control

// if Offline (replay) dont use TMapFile
Bool_t kUseMap = kTRUE;

static TH1F * hRateHistory = NULL;				// where the rate history is stored
static TH1F * hDTimeHistory = NULL;				// where the dead-time history is stored

TMrbAnalyze * gMrbAnalyze = NULL;				// user's base class

TUsrEvtStart * gStartEvent = NULL;				// pointer to "start" event (trigger 14)
TUsrEvtStop * gStopEvent = NULL;				// pointer to "stop" event (trigger 15)
TUsrDeadTime * gDeadTime = NULL;				// pointer to dead-time data

extern TMrbLogger * gMrbLog;					// message logger

extern TMrbLofUserVars * gMrbLofUserVars;		// list of user's vars and wdws

extern TMrbTransport * gMrbTransport;			// transport data from MBS

static const Char_t * calFmt1 = "%-15s%-15s%7d%7d%4d"; // format string for calibration printouts
static const Char_t * calFmt1h = "%-15s%-15s%7s%7s%4s";
static const Char_t * calFmt2 = "%14.4f";
static const Char_t * calFmt2h = "%14s";

ClassImp(TMrbAnalyze)							// implementation of user-specific classes

const SMrbNamedX kMrbLofDCorrTypes[] =
							{
								{TMrbDCorrListEntry::kDCorrNone, 			"None", 			"None"							},
								{TMrbDCorrListEntry::kDCorrConstFactor,		"ConstFactor",		"Constant factor"				},
								{TMrbDCorrListEntry::kDCorrFixedAngle, 		"FixedAngle",		"Fixed angle"					},
								{TMrbDCorrListEntry::kDCorrVariableAngle,	"VariableAngle",	"Depending on particle angle"	},
								{0, 										NULL,				NULL							}
							};

TMrbAnalyze::TMrbAnalyze(TMrbIOSpec * IOSpec) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::TMrbAnalyze
// Purpose:        Create an object to analyze user data
// Arguments:      TMrbIOSpec * IOSpec  -- i/o specifications
// Exceptions:
// Description:    Main class to initialize the user's analyzing procedures
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	fMessageLogger = gMrbLog;

	if (gMrbAnalyze != NULL) {
		gMrbLog->Err()	<< "Analyzer class already defined" << endl;
		gMrbLog->Flush(this->ClassName());
	} else {
		fVerboseMode = (Bool_t) gEnv->GetValue("TMrbAnalyze.VerboseMode", kFALSE);

		fWriteRootTree = kFALSE;
		fScaleDown = 1;
		fRunId = 0;
		fBranchSize = gEnv->GetValue("TMrbAnalyze.BranchSize", TMrbAnalyze::kBranchBufferSize);
		fSplitLevel = gEnv->GetValue("TMrbAnalyze.SplitLevel", TMrbAnalyze::kBranchSplitLevel);
		fRunStatus = TMrbAnalyze::M_ABSENT;
		fUpdateFlag = 0;
		fEventsProcessed = 0;
		fTimeOfLastUpdate = 0;
		fEventsProcPrev = 0;
		fHistFileVersion = 0;
		fReplayMode = kFALSE;
		fWriteRootTree = kFALSE;
		fRootFileOut = NULL;
		fRootFileIn = NULL;
		fFakeMode = kFALSE;

		fCurIOSpec = IOSpec;
		fLofIOSpecs.Delete();

		gStartEvent = new TUsrEvtStart();	// create special events for start and stop
		gStopEvent = new TUsrEvtStop();

		gDeadTime = new TUsrDeadTime(); 	// create dead-time object

		fModuleList.SetName("List of modules");		// bookkeeping: modules, params, and histos
		fParamList.SetName("List of params");
		fHistoList.SetName("List of histograms");

										// create a histogram to store rate history of last 300 secs
		hRateHistory = new TH1F("RateHist", "Rate history (last 300 seconds)", 300, -300, 0);
										// create a histogram to store dead-time history of last 300 secs
		hDTimeHistory = new TH1F("DeadTime", "Dead time history (last 300 seconds)", 300, -300, 0);

		fDumpCount = gEnv->GetValue("TMrbAnalyze.DumpCount", 0);

		fResourceName.Resize(0);		// reset resource name
		fResourceString.Resize(0);

		gMrbAnalyze = this; 			// save pointer so it can't called twice

		this->Initialize(IOSpec);		// initialize user objects

	}
}

Bool_t TMrbAnalyze::OpenRootFile(const Char_t * RootFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::OpenRootFile
// Purpose:        Open a file to replay tree data
// Arguments:      Char_t * RootFile   -- file name
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Opens a ROOT file for input.
//                 Replay mode only.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbIOSpec * ioSpec;
	Bool_t sts;

	ioSpec = new TMrbIOSpec();
	ioSpec->SetInputFile(RootFile, TMrbIOSpec::kInputRoot);
	sts = this->OpenRootFile(ioSpec);
	delete ioSpec;
	return(sts);
}

Int_t TMrbAnalyze::OpenFileList(TString & FileList, TMrbIOSpec * DefaultIOSpec) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::OpenFileList
// Purpose:        Read i/o specs from a file
// Arguments:      TString & FileList          -- list of i/o specs
//                 TMrbIOSpec * DefaultIOSpec  -- default values
// Results:        Int_t NofEntries            -- number of entries in file, 0 if error
// Exceptions:
// Description:    Reads a set of i/o specs.
//                 Format is
//                      inputFile  startEvent  stopEvent  paramFile  histoFile  outputFile
//
//                 where       value            meaning
//                 ---------------------------------------------------------------------------------
//                 inputFile                    name of input file
//                             xyz.root                file contains ROOT trees
//                             xyz.lmd                 file contains MBS list mode data
//                             xyz.med                 file contains MBS event data (as used by miniball)
//                 startEvent                   event / time stamp to start with
//                             N                       event number
//                             hh.mm.ss[:xxx]          time stamp in standard format
//                             :nnnnnn                 time stamp in seconds from start
//                             0                       read data from begin of file
//                 stopEvent                    event / time stamp to end with
//                             N                       event number
//                             hh.mm.ss[:xxx]          time stamp in standard format
//                             :nnnnnn                 time stamp in seconds from start
//                             0                       read data to end of file
//                 paramFile                    name of file where to reload params from
//                             xyz.root                file contains ROOT objects
//                             xyz.par                 file contains ASCII text
//                             -                   params remain unchanged
//                             none                don't load any params
//                 histoFile                    name of file to save histograms
//                             xyz.root                clear histo space on start, save histos to file at end
//                             +                       don't clear histo space, add data to current histos,
//                             ++                      same as +, but save to file currently open
//                 outputFile                   name of file to store tree data
//                             none                don't write tree data
//                             +                   append to current file
//                             xyz.root            open file for output
//
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	ifstream fileList(FileList.Data(), ios::in);
	if (!fileList.good()) {
		gMrbLog->Err()	<< gSystem->GetError() << " - " << FileList << endl;
		gMrbLog->Flush(this->ClassName(), "OpenFileList");
		return(kFALSE);
	}

	TRegexp rxroot("\\.root$");
	TRegexp rxmed("\\.med$");
	TRegexp rxlmd("\\.lmd$");

	Int_t errCnt = 0;
	Int_t lerrCnt = 0;
	Int_t lineCnt = 0;
	Int_t nofEntries = 0;
	TMrbIOSpec::EMrbInputMode inputMode = DefaultIOSpec->GetInputMode();

	TMrbIOSpec * lastIOSpec = DefaultIOSpec;
	TString line;
	for (;;) {
		line.ReadLine(fileList, kFALSE);
		if (fileList.eof()) break;
		lineCnt++;
		Bool_t lineHdr = kFALSE;
		line = line.Strip(TString::kBoth);
		if (line.Length() == 0 || line(0) == '#') continue;
		TObjArray * splitLine = line.Tokenize(" \t");
		Int_t nArgs = splitLine->GetEntries();
		if (nArgs < 6) {
			if (!lineHdr) {
				gMrbLog->Err()	<< "In file " << FileList << ", line " << lineCnt << ":" << endl;
				gMrbLog->Flush(this->ClassName(), "OpenFileList");
				lineHdr = kTRUE;
			}
			gMrbLog->Err()	<< "Too few arguments - " << nArgs << " (should be 6)" << endl;
			gMrbLog->Flush(this->ClassName(), "OpenFileList");
			errCnt++;
			delete splitLine;
			continue;
		} else if (nArgs > 6) {
			TString dummy = ((TObjString *) splitLine->At(6))->GetString();
			if (dummy(0) != '#') {
				if (!lineHdr) {
					gMrbLog->Err()	<< "In file " << FileList << ", line " << lineCnt << ":" << endl;
					gMrbLog->Flush(this->ClassName(), "OpenFileList");
					lineHdr = kTRUE;
				}
				gMrbLog->Wrn()	<< "Exceeding arguments - " << nArgs << " (should be 6) - ignored" << endl;
				gMrbLog->Flush(this->ClassName(), "OpenFileList");
			}
		}

// decode line
// input file
		TString inputFile = ((TObjString *) splitLine->At(0))->GetString();
		gSystem->ExpandPathName(inputFile);
		if (inputFile.Index(rxroot, 0) != -1)		inputMode = TMrbIOSpec::kInputRoot;
		else if (inputFile.Index(rxmed, 0) != -1)	inputMode = TMrbIOSpec::kInputMED;
		else if (inputFile.Index(rxlmd, 0) != -1)	inputMode = TMrbIOSpec::kInputLMD;
		else {
			if (!lineHdr) {
				gMrbLog->Err()	<< "In file " << FileList << ", line " << lineCnt << ":" << endl;
				gMrbLog->Flush(this->ClassName(), "OpenFileList");
				lineHdr = kTRUE;
			}
			gMrbLog->Err()	<< "Input Illegal extension - " << inputFile << " (should be .root)" << endl;
			gMrbLog->Flush(this->ClassName(), "OpenFileList");
			errCnt++;
		}
		if (gSystem->AccessPathName(inputFile.Data(), kFileExists)) {
			if (!lineHdr) {
				gMrbLog->Err()	<< "In file " << FileList << ", line " << lineCnt << ":" << endl;
				gMrbLog->Flush(this->ClassName(), "OpenFileList");
				lineHdr = kTRUE;
			}
			gMrbLog->Err()	<< "No such file - " << inputFile << endl;
			errCnt++;
		}
// start / stop
		TString startEvent = ((TObjString *) splitLine->At(1))->GetString();
		Int_t startValue;
		Bool_t startTimeFlag;
		if (!DefaultIOSpec->CheckStartStop(startEvent, startValue, startTimeFlag)) {
			if (!lineHdr) {
				gMrbLog->Err()	<< "In file " << FileList << ", line " << lineCnt << ":" << endl;
				gMrbLog->Flush(this->ClassName(), "OpenFileList");
				lineHdr = kTRUE;
			}
			gMrbLog->Err()	<< setred << "Not a legal start event - " << startEvent << endl;
			gMrbLog->Flush(this->ClassName(), "OpenFileList");
			errCnt++;
		}

		TString stopEvent = ((TObjString *) splitLine->At(2))->GetString();
		Int_t stopValue;
		Bool_t stopTimeFlag;
		if (!DefaultIOSpec->CheckStartStop(stopEvent, stopValue, stopTimeFlag)) {
			if (!lineHdr) {
				gMrbLog->Err()	<< "In file " << FileList << ", line " << lineCnt << ":" << endl;
				gMrbLog->Flush(this->ClassName(), "OpenFileList");
				lineHdr = kTRUE;
			}
			gMrbLog->Err()	<< "Not a legal stop event - " << stopEvent << endl;
			gMrbLog->Flush(this->ClassName(), "OpenFileList");
			errCnt++;
		}

		if (stopValue > 0) {
			if (startTimeFlag != stopTimeFlag) {
				if (!lineHdr) {
					gMrbLog->Err()	<< "In file " << FileList << ", line " << lineCnt << ":" << endl;
					gMrbLog->Flush(this->ClassName(), "OpenFileList");
					lineHdr = kTRUE;
				}
				gMrbLog->Err()	<< "Can't mix event count and time stamp" << endl;
				gMrbLog->Flush(this->ClassName(), "OpenFileList");
				errCnt++;
			}

			if (startValue > stopValue) {
				if (!lineHdr) {
					gMrbLog->Err()	<< "In file " << FileList << ", line " << lineCnt << ":" << endl;
					gMrbLog->Flush(this->ClassName(), "OpenFileList");
					lineHdr = kTRUE;
				}
				gMrbLog->Err()	<< "Illegal start/stop range - ["
						<< startEvent << "," << stopEvent << "]" << endl;
				gMrbLog->Flush(this->ClassName(), "OpenFileList");
				errCnt++;
			}
		}

// param file
		TString paramFile = ((TObjString *) splitLine->At(3))->GetString();
		TMrbIOSpec::EMrbParamMode lastpMode = lastIOSpec->GetParamMode();
		TMrbIOSpec::EMrbParamMode paramMode = TMrbIOSpec::kParamNone;
		if (paramFile.CompareTo("none") != 0) {
			if (paramFile.CompareTo("-") == 0) {
				paramFile = lastIOSpec->GetParamFile();
				if (lastpMode == TMrbIOSpec::kParamReload)	paramMode = TMrbIOSpec::kParamLoaded;
				else										paramMode = lastpMode;
			} else if (paramFile.Index(".root", 0) > 0) {
				paramMode = TMrbIOSpec::kParamReload;
			} else if (paramFile.Index(".par", 0) > 0) {
				paramMode = TMrbIOSpec::kParamReloadAscii;
			} else {
				if (!lineHdr) {
					gMrbLog->Err()	<< "In file " << FileList << ", line " << lineCnt << ":" << endl;
					gMrbLog->Flush(this->ClassName(), "OpenFileList");
					lineHdr = kTRUE;
				}
				gMrbLog->Err()	<< "Illegal extension - " << paramFile
								<< " (should be .root (standard ROOT) or .par (ASCII))" << endl;
				gMrbLog->Flush(this->ClassName(), "OpenFileList");
				errCnt++;
			}
		}

		if (paramMode == TMrbIOSpec::kParamReload && gSystem->AccessPathName(inputFile.Data(), kFileExists)) {
			if (!lineHdr) {
				gMrbLog->Err()	<< "In file " << FileList << ", line " << lineCnt << ":" << endl;
				gMrbLog->Flush(this->ClassName(), "OpenFileList");
				lineHdr = kTRUE;
			}
			gMrbLog->Err()	<< "No such file - " << paramFile << endl;
			gMrbLog->Flush(this->ClassName(), "OpenFileList");
			errCnt++;
		}

// histo file
		TString histoFile = ((TObjString *) splitLine->At(4))->GetString();
		TMrbIOSpec::EMrbHistoMode histoMode = TMrbIOSpec::kHistoNone;
		if (histoFile.CompareTo("none") != 0) {
			if (histoFile.CompareTo("+") == 0) {
				histoFile = lastIOSpec->GetHistoFile();
				histoMode = (TMrbIOSpec::EMrbHistoMode) TMrbIOSpec::kHistoAdd;
			} else if (histoFile.Index(".root", 0) > 0) {
				histoMode = (TMrbIOSpec::EMrbHistoMode) (TMrbIOSpec::kHistoSave | TMrbIOSpec::kHistoClear);
			} else {
				if (!lineHdr) {
					gMrbLog->Err()	<< "In file " << FileList << ", line " << lineCnt << ":" << endl;
					gMrbLog->Flush(this->ClassName(), "OpenFileList");
					lineHdr = kTRUE;
				}
				gMrbLog->Err()	<< "Hist: Illegal extension - " << histoFile << " (should be .root)" << endl;
				gMrbLog->Flush(this->ClassName(), "OpenFileList");
				errCnt++;
			}
		}

// output file
		TString outputFile = ((TObjString *) splitLine->At(5))->GetString();
		TMrbIOSpec::EMrbOutputMode outputMode = TMrbIOSpec::kOutputNone;
		TMrbIOSpec::EMrbOutputMode lastoMode = lastIOSpec->GetOutputMode();
		if (outputFile.CompareTo("none") != 0) {
			if (outputFile.CompareTo("+") == 0) {
				outputFile = lastIOSpec->GetOutputFile();
				outputMode = TMrbIOSpec::kOutputWriteRootTree;
				if ((lastoMode & TMrbIOSpec::kOutputWriteRootTree) == 0) {
					if (!lineHdr) {
						gMrbLog->Err()	<< "In file " << FileList << ", line " << lineCnt << ":" << endl;
						gMrbLog->Flush(this->ClassName(), "OpenFileList");
						lineHdr = kTRUE;
					}
					gMrbLog->Err()	<< "No output file open" << endl;
					gMrbLog->Flush(this->ClassName(), "OpenFileList");
					errCnt++;
				}
			} else if (outputFile.CompareTo("none") == 0) {
				outputMode = TMrbIOSpec::kOutputNone;
				if (lastoMode & TMrbIOSpec::kOutputWriteRootTree) {
					lastoMode = (TMrbIOSpec::EMrbOutputMode) (lastoMode | TMrbIOSpec::kOutputClose);
					lastIOSpec->SetOutputMode(lastoMode);
				}
			} else if (outputFile.Index(".root", 0) > 0) {
				outputMode = (TMrbIOSpec::EMrbOutputMode) (TMrbIOSpec::kOutputOpen | TMrbIOSpec::kOutputWriteRootTree);
				if (lastoMode & TMrbIOSpec::kOutputWriteRootTree) {
					lastoMode = (TMrbIOSpec::EMrbOutputMode) (lastoMode | TMrbIOSpec::kOutputClose);
					lastIOSpec->SetOutputMode(lastoMode);
				}
			} else {
				if (!lineHdr) {
					gMrbLog->Err()	<< "In file " << FileList << ", line " << lineCnt << ":" << endl;
					gMrbLog->Flush(this->ClassName(), "OpenFileList");
					lineHdr = kTRUE;
				}
				gMrbLog->Err()	<< "Output: Illegal extension - " << outputFile << " (should be .root)" << endl;
				gMrbLog->Flush(this->ClassName(), "OpenFileList");
				errCnt++;
			}
		}

// fill i/o spec (if w/o errors)
		if (errCnt == lerrCnt) {
			nofEntries++;
			TMrbIOSpec * ioSpec = new TMrbIOSpec();
			ioSpec->SetInputFile(inputFile.Data(), inputMode);
			ioSpec->SetStartStop(startTimeFlag, startValue, stopValue);
			ioSpec->SetOutputFile(outputFile.Data(), outputMode);
			ioSpec->SetParamFile(paramFile.Data(), paramMode);
			ioSpec->SetHistoFile(histoFile.Data(), histoMode);
			fLofIOSpecs.Add(ioSpec);
			lastIOSpec = ioSpec;
		}
		lerrCnt = errCnt;
		delete splitLine;
	}

// check if errors
	if (errCnt > 0) {
		gMrbLog->Err()	<< "Found " << errCnt << " error(s) in file list " << FileList << endl;
		gMrbLog->Flush(this->ClassName(), "OpenFileList");
		return(0);
	}

    if(fLofIOSpecs.GetSize() > 1)fLofIOSpecs.Remove(DefaultIOSpec);

	return(nofEntries);
}

Int_t TMrbAnalyze::ProcessFileList() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::ProcessFileList
// Purpose:        Process i/o specs in file list
// Arguments:      --
// Results:        Int_t NofEntries           -- number of entries processed
// Exceptions:
// Description:    Processes any entries in fLofIOSpecs.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t nofEntries;
	TMrbIOSpec * ioSpec;

	nofEntries = 0;
	this->ClearHistograms("*", ioSpec);
//  this was inside the loop on input files,
//  therefore filelists didnt work (OS)
   cout << " PutPid(TMrbAnalyze::M_RUNNING)" << endl;
   PutPid(TMrbAnalyze::M_RUNNING);
   this->SetRunStatus(TMrbAnalyze::M_RUNNING);

	TIterator * iter = fLofIOSpecs.MakeIterator();
	while ((ioSpec = (TMrbIOSpec *) iter->Next()) && this->TestRunStatus()) {
		this->SetCurIOSpec(ioSpec);
		gMrbLog->Out() << "[" << nofEntries + 1 << "] ";
		ioSpec->Print(gMrbLog->Out());
		gMrbLog->Flush(this->ClassName(), "ProcessFileList", setgreen);

		TMrbIOSpec::EMrbInputMode inputMode = ioSpec->GetInputMode();

		if (inputMode == TMrbIOSpec::kInputRoot) {
			if (this->OpenRootFile(ioSpec)) {
				this->ReloadParams(ioSpec);
				if (this->WriteRootTree(ioSpec)) {
					this->ReplayEvents(ioSpec);
					this->CloseRootTree(ioSpec);
					this->FinishRun(ioSpec);			// finish run (user may overwrite this method)
					this->SaveHistograms("*", ioSpec);
					this->ClearHistograms("*", ioSpec);
					nofEntries++;
				}
				if (!fFakeMode) {
					fRootFileIn->Close();
					fRootFileIn=NULL;
				}
			}
		} else if (inputMode == TMrbIOSpec::kInputMED || inputMode == TMrbIOSpec::kInputLMD) {
			if (gMrbTransport == NULL) {
				gMrbTransport = new TMrbTransport("M_analyze", "Connection to MBS");
				gMrbTransport->Version();
			}
			if (gMrbTransport) {
				gMrbTransport->SetStopFlag(kFALSE);
			 	if (gMrbTransport->Open(ioSpec->GetInputFile(), "F")) {
					gMrbTransport->OpenLogFile("M_analyze.log");
					this->ReloadParams(ioSpec);
					if (ioSpec->GetStartEvent() != 0) {
						gMrbLog->Err()	<< "[" << ioSpec->GetInputFile() << "] Start event != 0" << endl;
						gMrbLog->Flush(this->ClassName(), "ProcessFileList");
					}
               if (this->WriteRootTree(ioSpec)) {
						gMrbTransport->ReadEvents(ioSpec->GetStopEvent());
						if (inputMode == TMrbIOSpec::kInputMED) gMrbTransport->CloseMEDFile();
						else									gMrbTransport->CloseLMDFile();
						this->CloseRootTree(ioSpec);
						this->FinishRun(ioSpec);			// finish run (user may overwrite this method)
						this->SaveHistograms("*", ioSpec);
						this->ClearHistograms("*", ioSpec);
					}
					this->SetRunStatus(TMrbAnalyze::M_RUNNING); 	// for some (unknown) reason we end up with status "PAUSING", so we have to revive it.
				}
				nofEntries++;
			} else {
				gMrbLog->Err()	<< "[" << ioSpec->GetInputFile() << "] Can't open MBS transport" << endl;
				gMrbLog->Flush(this->ClassName(), "ProcessFileList");
			}
		}
	}

   cout << " PutPid(TMrbAnalyze::M_STOPPING)" << endl;
   PutPid(TMrbAnalyze::M_STOPPING);
	this->SetRunStatus(TMrbAnalyze::M_STOPPING);
	return(nofEntries);
}

Bool_t TMrbAnalyze::WriteRootTree(const Char_t * RootFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::WriteRootTree
// Purpose:        Open a file to write root data
// Arguments:      Char_t * RootFile   -- file name
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Opens a ROOT file for output.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbIOSpec * ioSpec;
	Bool_t sts;

	ioSpec = new TMrbIOSpec();
	ioSpec->SetOutputFile(RootFile, TMrbIOSpec::kOutputOpen);
	sts = this->WriteRootTree(ioSpec);
	delete ioSpec;
	return(sts);
}

Bool_t TMrbAnalyze::CloseRootTree(TMrbIOSpec * IOSpec) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::CloseRootTree
// Purpose:        Close root file output
// Arguments:      TMrbIOSpec * IOSpec    -- i/o spec
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks i/o spec for the "close bit" and closes output file.
//                 Closes anyway of i/o spec is not given.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbIOSpec::EMrbOutputMode outputMode;

	if (IOSpec == NULL) outputMode = TMrbIOSpec::kOutputClose;
	else				outputMode = IOSpec->GetOutputMode();

	if (fWriteRootTree) {
		if (outputMode & TMrbIOSpec::kOutputClose) {
			if (this->IsVerbose()) {
				cout	<< setblue
						<< this->ClassName() << "::Close(): Closing tree - " << fEventsProcessed << " event(s) written"
						<< setblack << endl;
			}
			fRootFileOut->Write();
			fRootFileOut->Close();
			if (this->IsVerbose()) {
				cout	<< setblue
						<< this->ClassName() << "::Close(): ROOT file \"" << fRootFileOut->GetName() << "\" closed"
						<< setblack << endl;
			}
			fWriteRootTree = kFALSE;
		}
	}
	return(kTRUE);
}

Bool_t TMrbAnalyze::TestRunStatus() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::TestRunStatus
// Purpose:        Test run status
// Arguments:
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Tests run status flag. Waits if it's PAUSING.
//                 Returns kFALSE on STOP.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fRunStatus == TMrbAnalyze::M_STOPPING) {
		cout << setred << this->ClassName() << "::TestRunStatus(): Termination flag detected" << setblack << endl;
		return(kFALSE);
	} else if (fRunStatus == TMrbAnalyze::M_PAUSING) {
        PutPid(TMrbAnalyze::M_PAUSING);
        while (fRunStatus == TMrbAnalyze::M_PAUSING) sleep(1);
		PutPid(fRunStatus);
		return(fRunStatus == TMrbAnalyze::M_STOPPING ? kFALSE : kTRUE);
	} else return(kTRUE);
}

TH1F * TMrbAnalyze::UpdateRateHistory() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::UpdateRateHistory
// Purpose:        Update the histogram showing the rate history
// Arguments:
// Results:        TH1F * rh      -- pointer to rate histogram
// Exceptions:
// Description:    Updates the rate history by adding a bin to the histogram.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t elapsed;
	time_t now;
	Int_t nofBins;
	Float_t rate;

	time(&now);

	if (fTimeOfLastUpdate > 0) {
		elapsed = now - fTimeOfLastUpdate;
//cout << "elapsed " << elapsed << endl;
		if (elapsed > 0) {
			nofBins = hRateHistory->GetNbinsX();
			rate = (fEventsProcessed - fEventsProcPrev) / elapsed;
			for (Int_t i = 1; i < nofBins; i++) {
				if (i + elapsed <= nofBins)
				   hRateHistory->SetBinContent(i, hRateHistory->GetBinContent(i+elapsed));
				else
				   hRateHistory->SetBinContent(i, 0);
			}
			hRateHistory->SetBinContent(nofBins, rate);
			hRateHistory->SetEntries(fEventsProcessed);
		}
	}
	fTimeOfLastUpdate = now;
	fEventsProcPrev = fEventsProcessed;
	return(hRateHistory);
}

TH1F * TMrbAnalyze::UpdateDTimeHistory() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::UpdateDTimeHistory
// Purpose:        Update the histogram showing the rate history
// Arguments:
// Results:        TH1F * rh      -- pointer to rate histogram
// Exceptions:
// Description:    Updates the rate history by adding a bin to the histogram.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t nofBins;

	nofBins = hDTimeHistory->GetNbinsX();
	for (Int_t i = 1; i < nofBins; i++) {
		hDTimeHistory->SetBinContent(i, hDTimeHistory->GetBinContent(i+1));
	}
	hDTimeHistory->SetBinContent(nofBins, gDeadTime->Get());
	hDTimeHistory->SetEntries(fEventsProcessed);
	return(hDTimeHistory);
}

void TMrbAnalyze::MarkHistogramsWithTime() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::MarkHistogramsWithTime
// Purpose:        Write a time stamp to all histograms in memory
// Arguments:
// Results:
// Exceptions:
// Description:    Loops thru the directory and writes time stamps to histograms.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TList * h;

	TDatime dt;

	h = gDirectory->GetList();
	TIter next(h);
	while(TObject * obj = next()) {
		if (obj->InheritsFrom("TH1")) obj->SetUniqueID(dt.Get());
	}
}

Bool_t TMrbAnalyze::ReloadParams(const Char_t * ParamFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::ReloadParams
// Purpose:        Reload parameters from file
// Arguments:      Char_t * ParamFile      -- file to be loaded
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Loops thru objects in file ParamFile, searches for
//                 variables, windows, and cuts.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString pFile;
	TMrbIOSpec::EMrbParamMode pMode;
	TMrbIOSpec * ioSpec;
	Bool_t sts;

	pFile = ParamFile;
	if (pFile.Index(".root", 0) > 0) {
		pMode = TMrbIOSpec::kParamReload;
	} else if (pFile.Index(".par", 0) > 0) {
		pMode = TMrbIOSpec::kParamReloadAscii;
	} else {
		gMrbLog->Err()	<< "Illegal extension - " << pFile
						<< " (should be .root (standard ROOT) or .par (ASCII))" << endl;
		gMrbLog->Flush(this->ClassName(), "ReloadParams");
		return(kFALSE);
	}

	ioSpec = new TMrbIOSpec();
	ioSpec->SetParamFile(pFile.Data(), pMode);
	sts = this->ReloadParams(ioSpec);
	delete ioSpec;
	return(sts);
}

Bool_t TMrbAnalyze::ReloadVarsAndWdws(const Char_t * VarWdwFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::ReloadVarsAndWdws
// Purpose:        Reload parameters (var & wdw settings) from file
// Arguments:      Char_t * VarWdwFile      -- file to be loaded
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Loops thru objects in file ParamFile, searches for
//                 variables, windows, and cuts.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString vwFile;
	TMrbIOSpec::EMrbParamMode pMode;
	TMrbIOSpec * ioSpec;
	Bool_t sts;

	vwFile = VarWdwFile;
	if (vwFile.Index(".root", 0) > 0) {
		pMode = TMrbIOSpec::kParamReload;
	} else if (vwFile.Index(".par", 0) > 0) {
		pMode = TMrbIOSpec::kParamReloadAscii;
	} else {
		gMrbLog->Err()	<< "Illegal extension - " << vwFile
						<< " (should be .root (standard ROOT) or .par (ASCII))" << endl;
		gMrbLog->Flush(this->ClassName(), "ReloadVarsAndWdws");
		return(kFALSE);
	}

	ioSpec = new TMrbIOSpec();
	ioSpec->SetParamFile(vwFile.Data(), pMode);
	sts = this->ReloadVarsAndWdws(ioSpec);
	delete ioSpec;
	return(sts);
}

Bool_t TMrbAnalyze::ReloadVarsAndWdws(TMrbIOSpec * IOSpec) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::ReloadVarsAndWdws
// Purpose:        Reload parameters (var & wdw settings) from file
// Arguments:      TMrbIOSpec * IOSpec    -- i/o specifications
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Loops thru objects in file ParamFile, searches for
//                 variables, windows, and cuts.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbIOSpec::EMrbParamMode pMode;
	TString pfName;
	TFile * parFile;
	TObject * vobj;
	TObject * fobj;
	TDirectory * gDirSave;

	if (gMrbLofUserVars == NULL) {
		gMrbLog->Err()	<< "No vars or wdws defined" << endl;
		gMrbLog->Flush(this->ClassName(), "ReloadVarsAndWdws");
		return(kFALSE);
	}

	pMode = IOSpec->GetParamMode();
	if ((pMode & TMrbIOSpec::kParamReload) == 0) return(kFALSE);
	pfName = IOSpec->GetParamFile();
	if (pMode == TMrbIOSpec::kParamReloadAscii) return(gMrbLofUserVars->ReadFromFile(pfName.Data()));

	gDirSave = gDirectory;						// save dir

//	gDirSave->ls();

	parFile = new TFile(pfName);
	if (!parFile->IsOpen()) {
		gMrbLog->Err()	<< gSystem->GetError() << " - " << pfName << endl;
		gMrbLog->Flush(this->ClassName(), "ReloadVarsAndWdws");
		gDirectory = gDirSave;
		delete parFile;
		return(kFALSE);
	}

	for (Int_t i = 0; i < (Int_t) gMrbLofUserVars->GetEntries(); i++) {	// loop thru var/wdw objects
		vobj = gMrbLofUserVars->Get(i);
		fobj = parFile->Get(vobj->GetName());	// look for this object in param file
		if (fobj != NULL) {
			if ((vobj->GetUniqueID() & kVarOrWindow) == (fobj->GetUniqueID() & kVarOrWindow)) {
				if (fobj->InheritsFrom("TMrbVariable")) {
					switch (((TMrbVariable *) fobj)->GetType()) {
						case kVarI:
							if (((TMrbVariable *) fobj)->IsArray()) {
								for (i = 0; i < ((TMrbVarArrayI *) vobj)->GetSize(); i++)
									((TMrbVarArrayI *) vobj)->Set(i, ((TMrbVarArrayI *) fobj)->Get(i));
							} else {
								((TMrbVarI *) vobj)->Set(((TMrbVarI *) fobj)->Get());
							}
							break;
						case kVarF:
							if (((TMrbVariable *) fobj)->IsArray()) {
								for (i = 0; i < ((TMrbVarArrayF *) vobj)->GetSize(); i++)
									((TMrbVarArrayF *) vobj)->Set(i, ((TMrbVarArrayF *) fobj)->Get(i));
							} else {
								((TMrbVarF *) vobj)->Set(((TMrbVarF *) fobj)->Get());
							}
							break;
					}
				} else {
#if 0
					gMrbLofUserVars->Replace(vobj, fobj);
					gROOT->GetList()->Remove(vobj);
					if (vobj) delete vobj;
					vobj = fobj;
                    if(fMapFile){
                        TObject* wold = (TObject*)fMapFile->Get(fobj->GetName());
                        if(wold)wold = (TObject*)fMapFile->Remove(fobj->GetName());
                        fMapFile->Add(fobj);
                        fMapFile->Update(fobj);
					}
#endif
					switch (((TMrbWindow *) fobj)->GetType()) {
						case kWindowI:
                            ((TMrbWindowI *) vobj)->Print();
							((TMrbWindowI *) vobj)->Set(((TMrbWindowI *) fobj)->GetLowerLimit(),
													((TMrbWindowI *) fobj)->GetUpperLimit());
                            cout << "ReloadVarsAndWdws: Reset " << vobj->GetName()  << " to: "
                                 << ((TMrbWindowI *) fobj)->GetLowerLimit() << " - "
                                 << ((TMrbWindowI *) fobj)->GetUpperLimit() << endl;
							break;

						case kWindowF:
							((TMrbWindowF *) vobj)->Set(((TMrbWindowF *) fobj)->GetLowerLimit(),
													((TMrbWindowF *) fobj)->GetUpperLimit());
							break;
						case kWindow2D:
							gMrbLofUserVars->Replace(vobj, fobj);
#if 0
							gROOT->GetList()->Remove(vobj);
							if (vobj) delete vobj;
							vobj = fobj;
                            if(fMapFile){
                                TMrbWindow2D* w2dold = (TMrbWindow2D*)fMapFile->Get(fobj->GetName());
                                if(w2dold)w2dold = (TMrbWindow2D*)fMapFile->Remove(fobj->GetName());
                                fMapFile->Add(fobj);
                                fMapFile->Update(fobj);
					        }
#endif
							break;
					}
				}
				if (this->IsVerbose()) {
					cout	<< this->ClassName() << "::ReloadParams(): Updated ";
					if (fobj->InheritsFrom("TMrbVariable")) ((TMrbVariable *) vobj)->Print("Short");
					else if (fobj->InheritsFrom("TMrbWindow")) ((TMrbWindow *) vobj)->Print("Short");
					else if (fobj->InheritsFrom("TMrbWindow2D")) cout << ((TMrbWindow2D *) vobj)->GetName() << endl;
				}
			} else delete fobj;
		}
	}

//	parFile->Close();
	this->SetUpdateFlag();

	delete parFile;

	gDirectory = gDirSave;

	return(kTRUE);
}

Int_t TMrbAnalyze::GetSizeOfMappedObjects(TMapFile * MapFile) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetSizeOfMappedObjects
// Purpose:        Determine total size of all objects mapped so far
// Arguments:      TMapFile * MapFile      -- current map file
// Results:        Int_t NofBytes          -- number of bytes allocated
// Exceptions:
// Description:    Loops thru objects in mapfile, calculates total size.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMapRec * mr = MapFile->GetFirst();
	if(!mr){
		gMrbLog->Err()	<< "No records" << endl;
		gMrbLog->Flush(this->ClassName(), "GetSizeOfMappedObjects");
		return(0);
	}
	Int_t size = 0;
    Bool_t ok = kTRUE;
	while (MapFile->OrgAddress(mr)) {
		if(!mr) break;
        Int_t s =  mr->GetBufSize();
        if(s <= 0)ok = kFALSE;
		size += s;
		mr = mr->GetNext();
	}
    if(!ok){
		gMrbLog->Err()	<< "MapFile too small: "<< size << endl;
		gMrbLog->Flush(this->ClassName(), "GetSizeOfMappedObjects");
        MapFile->Print();
		return(0);
    }
    cout	<< setgreen
			<< this->ClassName() << "Size used on Mapfile (KBytes): " << size / 1024
			<< setblack << endl;
	return(size);
}

Int_t TMrbAnalyze::SaveHistograms(const Char_t * Pattern, const Char_t * HistoFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::SaveHistograms
// Purpose:        Save mmap data to file
// Arguments:      Char_t * Pattern    -- pattern (regexp) to select histos
//                 Char_t * HistoFile  -- file name
// Results:        Int_t NofHistos    -- number of histos saved
// Exceptions:     NofHistos = -1 on error
// Description:    Writes mmap file contents to root file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbIOSpec * ioSpec;
	Int_t nofHistos;

	ioSpec = new TMrbIOSpec();
	ioSpec->SetHistoFile(HistoFile, TMrbIOSpec::kHistoSave);
	nofHistos = this->SaveHistograms(Pattern, ioSpec);
	delete ioSpec;
	return(nofHistos);
}

Int_t TMrbAnalyze::SaveHistograms(const Char_t * Pattern, TMrbIOSpec * IOSpec) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::SaveHistograms
// Purpose:        Save mmap data to file
// Arguments:      Char_t * Pattern      -- pattern (regexp) to select histos
//                 TMrbIOSpec * IOSpec   -- i/o specs
// Results:        Int_t NofHistos       -- number of histos saved
// Exceptions:     NofHistos = -1 on error
// Description:    Writes mmap file contents to root file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (IOSpec && (IOSpec->GetHistoMode() & TMrbIOSpec::kHistoSave) == 0) return(0);

	TString pattern = Pattern;
	pattern = pattern.Strip(TString::kBoth);
	Int_t histoCount = 0;
	Int_t wdwCount = 0;
	Int_t cutCount = 0;
	TRegexp * rexp = NULL;

	if (pattern.CompareTo("*") == 0) rexp = new TRegexp(pattern.Data(), kTRUE);
// generate a versioned filename
	TString histoFile = IOSpec->GetHistoFile();
	TString histoFileVersioned = histoFile;
	histoFileVersioned += ".";
	histoFileVersioned += fHistFileVersion++;

	if (this->IsVerbose())cout	<< setblue
			<< this->ClassName() << "::SaveHistograms(): Saving histogram(s) to file "
			<< histoFileVersioned << " - wait ..." << setblack << endl;
	pthread_mutex_lock(&global_data_mutex);
	TFile * hf = new TFile(histoFileVersioned.Data(), "RECREATE");
	if (!hf->IsOpen()) {
		gMrbLog->Err()	<< "Can't open histo file " << histoFileVersioned << endl;
		gMrbLog->Flush(this->ClassName(), "SaveHistograms");
	   pthread_mutex_unlock(&global_data_mutex);
		return(-1);
	}
	hf->cd();

   if(kUseMap){
		fMapFile->Update();		// force update

		const char * name;
		TMapRec * mr = fMapFile->GetFirst();
		if (mr) {
			while (fMapFile->OrgAddress(mr)) {
				if(!mr) break;
				name = mr->GetName();
				Bool_t ok = kFALSE;
				if (rexp == NULL) ok = kTRUE;
				else {
					TString shh(name);
					if(shh.Index(*rexp) >= 0) ok = kTRUE;
				}
				if (ok) {
					TObject * obj = NULL;
					obj = fMapFile->Get(name, obj);
					if (obj) obj->Write();
					if (this->IsVerbose()) cout << "   Writing: " << name << endl;
        		}
		   	 mr = mr->GetNext();
			}
		}
   } else {
		TIter next(gROOT->GetList());
		TObject * obj;
		while( (obj = (TObject*)next()) ){
			Bool_t writeIt = kFALSE;
			TString otype;
			TString shh(obj->GetName());
			if(shh.Index(*rexp) >= 0) {
				if(obj->InheritsFrom("TH1")) {
				  writeIt = kTRUE;
				  otype = "histo";
				  histoCount++;
				} else if (obj->InheritsFrom("TMrbWindow")) {
				  writeIt = kTRUE;
				  otype = "window";
				  wdwCount++;
				} else if (obj->InheritsFrom("TCutG")) {
				  writeIt = kTRUE;
				  otype = "cut";
				  cutCount++;
				  TMrbWindow2D * w = (TMrbWindow2D *) gMrbLofUserVars->Find(((TMrbWindow2D *)obj)->GetName());
				  if (w) obj = w;
				}
				if (writeIt) {
					obj->Write();
					if (this->IsVerbose()) cout << "   Writing " << otype << " " << obj->GetName() << endl;
            	}
			}
		}
	}
	if (histoCount) {
		gMrbLog->Out()	<< histoCount << " histogram(s) saved to " << histoFile << endl;
		gMrbLog->Flush(this->ClassName(), "SaveHistograms", setblue);
	}
	if (wdwCount) {
		gMrbLog->Out()	<< wdwCount << " 1dim window(s) saved to " << histoFile << endl;
		gMrbLog->Flush(this->ClassName(), "SaveHistograms", setblue);
	}
	if (cutCount) {
		gMrbLog->Out()	<< cutCount << " 2dim cut(s) saved to " << histoFile << endl;
		gMrbLog->Flush(this->ClassName(), "SaveHistograms", setblue);
	}
	hf->Close();
	pthread_mutex_unlock(&global_data_mutex);
// set link to newly created file, remove old
   if (!gSystem->AccessPathName(histoFile.Data())) {
	   gSystem->Unlink(histoFile.Data());
	}
	gSystem->Symlink(gSystem->BaseName(histoFileVersioned.Data()), histoFile.Data());
	if (fHistFileVersion > 1) {
	   TString oldfile(histoFile.Data());
		oldfile += ".";
		oldfile += (fHistFileVersion - 2);
      if (!gSystem->AccessPathName(oldfile.Data())) {
	      gSystem->Unlink(oldfile.Data());
	   }
	}
	return(histoCount + wdwCount + cutCount);
}

Int_t TMrbAnalyze::ClearHistograms(const Char_t * Pattern, TMrbIOSpec * IOSpec) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::ClearHistograms
// Purpose:        Clear histograms in mmap file
// Arguments:      Char_t * Pattern      -- pattern (regexp) to select histograms
//                 TMrbIOSpec * IOSpec   -- i/o specs
// Results:        Int_t NofHistos      -- number of histos cleared
// Exceptions:     NofHistos = -1 on error
// Description:    Clears shared memory.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString pattern;
	Int_t count;
	TRegexp * rexp;
	Bool_t hdr;

	if (IOSpec && (IOSpec->GetHistoMode() & TMrbIOSpec::kHistoClear) == 0) return(0);

	pattern = Pattern;
	pattern = pattern.Strip(TString::kBoth);
	count = 0;
	rexp = NULL;

	if (pattern.CompareTo("*") != 0) rexp = new TRegexp(pattern.Data(), kTRUE);

	TH1 * hh;

    TIter * next_obj;
    if(kUseMap)next_obj = new TIter(fMapFile->GetDirectory()->GetList());
    else       next_obj = new TIter(gROOT->GetList());

	TObject * obj;

	hdr = kFALSE;

	pthread_mutex_lock(&global_data_mutex);
	while ( (obj = (TObject *) (*next_obj)()) ) {
		Bool_t ok = kFALSE;
		if (obj->InheritsFrom("TH1")) {
			hh = (TH1*) obj;
			if (rexp == NULL) ok = kTRUE;
			else {
				TString shh(hh->GetName());

				if(shh.Index(*rexp) >= 0) ok = kTRUE;
			}
			if (ok) {
				if (this->IsVerbose()) {
					if (!hdr) cout << this->ClassName()
						           << "::ClearHistograms(" << pattern.Data()
						           << "): Zeroing follwing histograms ..." << endl;
					hdr = kTRUE;
					cout << "   " ; hh->Print();
				}
				TList * hlist = hh->GetListOfFunctions();
				TList * newlist = NULL;
				if (hlist && hlist->GetSize() > 0) {
					newlist = new TList();
					TIter next(hlist);
					while ( (obj = (TObject *) next()) ) newlist->Add(obj);
					hlist->Clear();
            	}
				hh->Reset();
				if (newlist) {
					hlist = hh->GetListOfFunctions();
					TIter next(newlist);
					while( (obj = (TObject *) next()) ) hlist->Add(obj);
					newlist->Clear();
					delete newlist;
				}
				count++;
			}
		}
	}
	if(kUseMap)fMapFile->Update(); 	// clear map file
	pthread_mutex_unlock(&global_data_mutex);
	gMrbLog->Out()	<< count << " histogram(s) zeroed" << endl;
	gMrbLog->Flush(this->ClassName(), "ClearHistograms", setblue);
	if (rexp) delete rexp;
	return(count);
}

void TMrbAnalyze::PrintStartStop(TUsrEvtStart * StartEvent, TUsrEvtStop * StopEvent) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::PrintStartStop
// Purpose:        Output time stamps for start and stop
// Arguments:      TUsrEvtStart * StartEvent            -- start event (default gStartEvent)
//                 TUsrEvtStop * StopEvent              -- stop event (default gStopEvent)
// Results:        --
// Exceptions:
// Description:    Outputs start/stop records read from root file
//                 (replay mode only)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t runTime;

	if (StartEvent == NULL) StartEvent = gStartEvent;
	if (StopEvent == NULL) StopEvent = gStopEvent;

	if (StartEvent->GetTreeIn() == NULL || StopEvent->GetTreeIn() == NULL) {
		gMrbLog->Err()	<< "No start/stop tree found" << endl;
		gMrbLog->Flush(this->ClassName(), "PrintStartStop");
	} else {
		cout	<< setblue
				<< this->ClassName() << "::PrintStartStop(): Start/stop info taken from file "
				<< fRootFileIn->GetName() << ":" << endl
				<< "--------------------------------------------------------------------------------" << endl;
		StartEvent->GetTreeIn()->GetEvent(0);
		StartEvent->Print("START Acquisition");
		StopEvent->GetTreeIn()->GetEvent(0);
		StopEvent->Print("STOP Acquisition");
		Int_t stop = StopEvent->GetTime();
		Int_t start = StartEvent->GetTime();
		if (stop > start) {
			runTime = StopEvent->GetTime() - StartEvent->GetTime();
			Int_t h = runTime / 3600;
			runTime = runTime % 3600;
			Int_t m = runTime / 60;
			Int_t s = runTime % 60;
			cout	<< setblue
					<< "Runtime             :   " << h << ":" << m << ":" << s << endl;
		}
		cout	<< setblue
				<< "--------------------------------------------------------------------------------"
				<< setblack << endl;
	}
}

void TMrbAnalyze::PrintStartStop(UInt_t StartTime, UInt_t StopTime) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::PrintStartStop
// Purpose:        Output time stamps for start and stop
// Arguments:      UInt_t StartTime            -- start time
//                 UInt_t StopTime             -- stop time
// Results:        --
// Exceptions:
// Description:    Outputs start/stop records read from root file
//                 (replay mode only)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t runTime;

	cout	<< setblue
			<< this->ClassName() << "::PrintStartStop(): Start/stop info taken from file "
			<< fRootFileIn->GetName() << ":" << endl
			<< "--------------------------------------------------------------------------------" << endl;
	TDatime d;
	d.Set(StartTime, kFALSE);
	cout	<< setblue << this->ClassName() << "::Print(): " "START Acquisition at " << d.AsString() << setblack << endl;
	d.Set(StopTime, kFALSE);
	cout	<< setblue << this->ClassName() << "::Print(): " "STOP Acquisition at " << d.AsString() << setblack << endl;

	runTime = StopTime - StartTime;
	Int_t h = runTime / 3600;
	runTime = runTime % 3600;
	Int_t m = runTime / 60;
	Int_t s = runTime % 60;
	cout	<< setblue
			<< "Runtime             :   " << h << ":" << m << ":" << s << endl;
	cout	<< setblue
			<< "--------------------------------------------------------------------------------"
			<< setblack << endl;
}

void TMrbAnalyze::InitializeLists(Int_t NofModules, Int_t NofParams) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::InitializeLists
// Purpose:        Initialize lists and resize
// Arguments:      Int_t NofModules    -- number of modules
//                 Int_t NofParams     -- number of params
// Results:        --
// Exceptions:
// Description:    Initializes (clears) lists and resizes them.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fNofModules = NofModules;
	fNofParams = NofParams;

	fModuleList.Delete();
	fModuleList.Expand(NofModules + 1);
	for (Int_t i = 0; i < NofModules + 1; i++) fModuleList.AddAt(NULL, i);
	fParamList.Delete();
	fParamList.Expand(NofParams);
	for (Int_t i = 0; i < NofParams; i++) fParamList.AddAt(NULL, i);
	fHistoList.Delete();
	fHistoList.Expand(NofParams);
	for (Int_t i = 0; i < NofParams; i++) fHistoList.AddAt(NULL, i);
	fCalibrationList.Delete();
	fCalibrationList.Expand(NofParams);
	for (Int_t i = 0; i < NofParams; i++) fCalibrationList.AddAt(NULL, i);
	fDCorrList.Delete();
	fDCorrList.Expand(NofParams);
	for (Int_t i = 0; i < NofParams; i++) fDCorrList.AddAt(NULL, i);
}

const Char_t * TMrbAnalyze::GetModuleName(Int_t ModuleIndex) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetModuleName
// Purpose:        Get module name by its index
// Arguments:      Int_t ModuleIndex   -- module index
// Results:        Char_t * ModuleName -- module name
// Exceptions:
// Description:    Searches for a module with specified index.
//                 Returns its name.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;

	if (ModuleIndex <= 0 || ModuleIndex > fModuleList.GetLast()) {
		gMrbLog->Err()	<< "Index out of range - " << ModuleIndex
						<< " (should be in [1," << fModuleList.GetLast() << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "GetModuleName");
		return(NULL);
	}
	nx = (TMrbNamedX *) fModuleList[ModuleIndex];
	if (nx == NULL) {
		gMrbLog->Err()	<< "No module with index = " << ModuleIndex << endl;
		gMrbLog->Flush(this->ClassName(), "GetModuleName");
		return(NULL);
	}
	return(nx->GetName());
}

const Char_t * TMrbAnalyze::GetModuleTitle(Int_t ModuleIndex) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetModuleTitle
// Purpose:        Get module title by its index
// Arguments:      Int_t ModuleIndex    -- module index
// Results:        Char_t * ModuleTitle -- module title
// Exceptions:
// Description:    Searches for a module with specified index.
//                 Returns its title.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;

	if (ModuleIndex <= 0 || ModuleIndex > fModuleList.GetLast()) {
		gMrbLog->Err()	<< "Index out of range - " << ModuleIndex
						<< " (should be in [1," << fModuleList.GetLast() << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "GetModuleTitle");
		return(NULL);
	}
	nx = (TMrbNamedX *) fModuleList[ModuleIndex];
	if (nx == NULL) {
		gMrbLog->Err()	<< "No module with index = " << ModuleIndex << endl;
		gMrbLog->Flush(this->ClassName(), "GetModuleTitle");
		return(NULL);
	}
	return(nx->GetTitle());
}

Int_t TMrbAnalyze::GetModuleIndex(const Char_t * ModuleName) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetModuleIndex
// Purpose:        Get module index by its name
// Arguments:      Char_t * ModuleName    -- module name
// Results:        Int_t ModuleIndex      -- module index
// Exceptions:
// Description:    Searches for a module with specified name.
//                 Returns its index.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;
	nx = fModuleList.FindByName(ModuleName);
	if (nx == NULL) {
		gMrbLog->Err()	<< "No such module - " << ModuleName << endl;
		gMrbLog->Flush(this->ClassName(), "GetModuleIndex");
		return(-1);
	} else {
		return(nx->GetIndex());
	}
}

Int_t TMrbAnalyze::GetModuleIndexByParam(const Char_t * ParamName) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetModuleIndexByParam
// Purpose:        Get module index by param name
// Arguments:      Char_t * ParamName      -- param name
// Results:        Int_t ModuleIndex       -- module index
// Exceptions:
// Description:    Searches for a param with specified name
//                 Returns module index.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;
	TMrbParamListEntry * ple;

	nx = fParamList.FindByName(ParamName);
	if (nx == NULL) {
		gMrbLog->Err()	<< "No such param - " << ParamName << endl;
		gMrbLog->Flush(this->ClassName(), "GetParamIndex");
		return(-1);
	}

	ple = (TMrbParamListEntry *) nx->GetAssignedObject();				// relative param index
	return(ple->GetModule()->GetIndex());
}

TMrbModuleListEntry * TMrbAnalyze::GetModuleListEntry(Int_t ModuleIndex) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetModuleListEntry
// Purpose:        Get module list entry by module index
// Arguments:      Int_t ModuleIndex                 -- module index
// Results:        TMrbModuleListEntry * ListEntry   -- entry
// Exceptions:
// Description:    Searches for a module with specified index.
//                 Returns entry in module list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;

	if (ModuleIndex <= 0 || ModuleIndex > fModuleList.GetLast()) {
		gMrbLog->Err()	<< "Index out of range - " << ModuleIndex
						<< " (should be in [1," << fModuleList.GetLast() << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "GetModuleName");
		return(NULL);
	}
	nx = (TMrbNamedX *) fModuleList[ModuleIndex];
	if (nx == NULL) {
		gMrbLog->Err()	<< "No module with index = " << ModuleIndex << endl;
		gMrbLog->Flush(this->ClassName(), "GetModuleName");
		return(NULL);
	}
	return((TMrbModuleListEntry *) nx->GetAssignedObject());
}

TMrbModuleListEntry * TMrbAnalyze::GetModuleListEntry(const Char_t * ModuleName) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetModuleListEntry
// Purpose:        Get module list entry by module name
// Arguments:      const Char_t * ModuleName         -- module name
// Results:        TMrbModuleListEntry * ListEntry   -- entry
// Exceptions:
// Description:    Searches for a module with specified name.
//                 Returns entry in module list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;
	nx = fModuleList.FindByName(ModuleName);
	if (nx == NULL) {
		gMrbLog->Err()	<< "No such module - " << ModuleName << endl;
		gMrbLog->Flush(this->ClassName(), "GetModuleIndex");
		return(NULL);
	} else {
		return((TMrbModuleListEntry *) nx->GetAssignedObject());
	}
}

void TMrbAnalyze::ResetModuleHits(Int_t StartIndex, Int_t StopIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           ResetModuleHits
// Purpose:        Clear module hit counters
// Arguments:      Int_t StartIndex     -- start index
//                 Int_t StopIndex      -- stop index
// Results:        --
// Exceptions:
// Description:    Reset hit counters for modules within given index limits in list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t allModules = (StartIndex == 0 && StopIndex == 0);
	for (Int_t i = 0; i < fModuleList.GetEntriesFast(); i++) {
		TMrbNamedX * nx = (TMrbNamedX *) fModuleList[i];
		if (nx) {
			if (allModules || (nx->GetIndex() >= StartIndex && nx->GetIndex() <= StopIndex)) {
				((TMrbModuleListEntry *) nx->GetAssignedObject())->ResetAllHits();
			}
		}
	}
}

const Char_t * TMrbAnalyze::GetParamName(Int_t ModuleIndex, Int_t RelParamIndex) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetParamName
// Purpose:        Get param name by its relative index
// Arguments:      Int_t ModuleIndex      -- module index
//                 Int_t RelParamIndex    -- relative param index
// Results:        Char_t * ParamName     -- param name
// Exceptions:
// Description:    Searches for a param with specified index.
//                 Returns its name.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;
	TMrbModuleListEntry * mle;
	Int_t px;

	if (ModuleIndex <= 0 || ModuleIndex > fModuleList.GetLast()) {
		gMrbLog->Err()	<< "Module index out of range - " << ModuleIndex
						<< " (should be in [1," << fModuleList.GetLast() << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "GetParamName");
		return(NULL);
	}
	nx = (TMrbNamedX *) fModuleList[ModuleIndex];
	if (nx == NULL) {
		gMrbLog->Err()	<< "No module with index = " << ModuleIndex << endl;
		gMrbLog->Flush(this->ClassName(), "GetParamName");
		return(NULL);
	}
	mle = (TMrbModuleListEntry *) nx->GetAssignedObject();
	if (RelParamIndex >= mle->GetNofParams()) {
		gMrbLog->Err()	<< "Rel param index out of range - " << RelParamIndex
						<< " (should be in [0," << mle->GetNofParams() - 1 << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "GetParamName");
		return(NULL);
	}
	px = mle->GetIndexOfFirstParam() + RelParamIndex;
	if (px < 0 || px > fParamList.GetLast()) {
		gMrbLog->Err()	<< "Abs param index out of range - " << px
						<< " (should be in [0," << fParamList.GetLast() << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "GetParamName");
		return(NULL);
	}
	nx = (TMrbNamedX *) fParamList[px];
	if (nx == NULL) {
		gMrbLog->Err()	<< "No param with index = " << px << endl;
		gMrbLog->Flush(this->ClassName(), "GetParamName");
		return(NULL);
	}
	return(nx->GetName());
}

const Char_t * TMrbAnalyze::GetParamName(Int_t AbsParamIndex) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetParamName
// Purpose:        Get param name by its absolute index
// Arguments:      Int_t AbsParamIndex    -- absolute param index
// Results:        Char_t * ParamName     -- param name
// Exceptions:
// Description:    Searches for a param with specified index.
//                 Returns its name.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;

	if (AbsParamIndex < 0 || AbsParamIndex > fParamList.GetLast()) {
		gMrbLog->Err()	<< "Abs param index out of range - " << AbsParamIndex
						<< " (should be in [0," << fParamList.GetLast() << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "GetParamName");
		return(NULL);
	}
	nx = (TMrbNamedX *) fParamList[AbsParamIndex];
	if (nx == NULL) {
		gMrbLog->Err()	<< "No param with index = " << AbsParamIndex << endl;
		gMrbLog->Flush(this->ClassName(), "GetParamName");
		return(NULL);
	}
	return(nx->GetName());
}

Int_t TMrbAnalyze::GetParamIndex(const Char_t * ParamName, Bool_t AbsFlag) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetParamIndex
// Purpose:        Get param index
// Arguments:      Char_t * ParamName     -- param name
//                 Bool_t AbsFlag         -- abs or rel?
// Results:        Int_t ParamIndex       -- param index
// Exceptions:
// Description:    Searches for a param with specified name
//                 Returns its index.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;
	TMrbParamListEntry * ple;
	TMrbModuleListEntry * mle;

	nx = fParamList.FindByName(ParamName);
	if (nx == NULL) {
		gMrbLog->Err()	<< "No such param - " << ParamName << endl;
		gMrbLog->Flush(this->ClassName(), "GetParamIndex");
		return(-1);
	}

	if (AbsFlag) return(nx->GetIndex());		// absolute param index

	ple = (TMrbParamListEntry *) nx->GetAssignedObject();				// relative param index
	mle = (TMrbModuleListEntry *) ple->GetModule()->GetAssignedObject();
	return(nx->GetIndex() - mle->GetIndexOfFirstParam());
}

Int_t TMrbAnalyze::GetParamIndex(Int_t ModuleIndex, Int_t RelParamIndex) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetParamIndex
// Purpose:        Get param index
// Arguments:      Int_t ModuleIndex      -- module index
//                 Int_t RelParamIndex    -- relative param index
// Results:        Int_t AbsParamIndex    -- absolute param index
// Exceptions:
// Description:    Calculates absolute param index.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;
	TMrbModuleListEntry * mle;
	Int_t px;

	if (ModuleIndex <= 0 || ModuleIndex > fModuleList.GetLast()) return(-1);
	nx = (TMrbNamedX *) fModuleList[ModuleIndex];
	mle = (TMrbModuleListEntry *) nx->GetAssignedObject();
	if (RelParamIndex >= mle->GetNofParams()) return(-1);
	px = mle->GetIndexOfFirstParam() + RelParamIndex;
	if (px < 0 || px > fParamList.GetLast()) return(-1);
	nx = (TMrbNamedX *) fParamList[px];
	if (nx == NULL) return(-1);
	return(px);
}

TMrbHistoListEntry * TMrbAnalyze::GetHistoListEntry(const Char_t * HistoName) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetHistoListEntry
// Purpose:        Get entry in histogram table
// Arguments:      Char_t * HistoName               -- histo name
// Results:        TMrbHistoListEntry * HistoEntry  -- address
// Exceptions:
// Description:    Searches for a histo with specified name
//                 Returns entry address.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;

	nx = fHistoList.FindByName(HistoName);
	if (nx) 	return((TMrbHistoListEntry *) nx->GetAssignedObject());
	else		return(NULL);
}

TH1 * TMrbAnalyze::GetHistoAddr(const Char_t * HistoName) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetHistoAddr
// Purpose:        Get histo addr by name
// Arguments:      Char_t * HistoName     -- histo name
// Results:        TH1 * HistoAddr        -- address
// Exceptions:
// Description:    Searches for a histo with specified name
//                 Returns histogram address.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;
	TMrbHistoListEntry * hle;

	nx = fHistoList.FindByName(HistoName);
	if (nx == NULL) {
		gMrbLog->Err()	<< "No such histogram - " << HistoName << endl;
		gMrbLog->Flush(this->ClassName(), "GetHistoAddr");
		return(NULL);
	}
	hle = (TMrbHistoListEntry *) nx->GetAssignedObject();
	return(hle->GetAddress());
}

TH1 * TMrbAnalyze::GetHistoAddr(Int_t AbsParamIndex) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetHistoAddr
// Purpose:        Get histogram address
// Arguments:      Int_t AbsParamIndex    -- absolute param/histo index
// Results:        TH1 * HistoAddr      -- histo address
// Exceptions:
// Description:    Returns histo address.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;

	if (AbsParamIndex < 0 || AbsParamIndex > fParamList.GetLast()) return(NULL);
	nx = (TMrbNamedX *) fHistoList[AbsParamIndex];
	if (nx == NULL) return(NULL);
	return(((TMrbHistoListEntry *) nx->GetAssignedObject())->GetAddress());
}

TH1 * TMrbAnalyze::GetHistoAddr(Int_t ModuleIndex, Int_t RelParamIndex) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetHistoAddr
// Purpose:        Get histogram by module number and param offset
// Arguments:      Int_t ModuleIndex      -- module index
//                 Int_t RelParamIndex    -- relative param index
// Results:        TH1 * HistoAddr        -- histogram address
// Exceptions:
// Description:    Returns histogram addr given by module and param.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;
	TMrbModuleListEntry * mle;
	Int_t px;

	if (ModuleIndex <= 0 || ModuleIndex > fModuleList.GetLast()) return(NULL);
	nx = (TMrbNamedX *) fModuleList[ModuleIndex];
	mle = (TMrbModuleListEntry *) nx->GetAssignedObject();
	if (RelParamIndex >= mle->GetNofParams()) return(NULL);
	px = mle->GetIndexOfFirstParam() + RelParamIndex;
	if (px < 0 || px > fHistoList.GetLast()) return(NULL);
	nx = (TMrbNamedX *) fHistoList[px];
	if (nx == NULL) return(NULL);
	return(((TMrbHistoListEntry *) nx->GetAssignedObject())->GetAddress());
}

TH1 * TMrbAnalyze::GetHistoFromList(TObjArray & HistoList, Int_t ModuleIndex, Int_t RelParamIndex) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetHistoFromList
// Purpose:        Get histogram from list
// Arguments:      TObjArray HistoList    -- array containing histograms
//                 Int_t ModuleIndex      -- module index
//                 Int_t RelParamIndex    -- relative param index
// Results:        TH1 * HistoAddr        -- histogram address
// Exceptions:
// Description:    Calculates absolute param index. Returns histo from list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;
	TMrbModuleListEntry * mle;
	Int_t px;

	if (ModuleIndex <= 0 || ModuleIndex > fModuleList.GetLast()) return(NULL);
	nx = (TMrbNamedX *) fModuleList[ModuleIndex];
	mle = (TMrbModuleListEntry *) nx->GetAssignedObject();
	if (RelParamIndex >= mle->GetNofParams()) return(NULL);
	px = mle->GetIndexOfFirstParam() + RelParamIndex;
	if (px < 0 || px > HistoList.GetEntriesFast()) return(NULL);
	return((TH1F *) HistoList[px]);
}

TH1 * TMrbAnalyze::GetHistoFromList(TObjArray & HistoList, Int_t AbsParamIndex) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetHistoFromList
// Purpose:        Get histogram from list
// Arguments:      TObjArray HistoList    -- array containing histograms
//                 Int_t AbsParamIndex    -- absolute param index
// Results:        TH1 * HistoAddr        -- histogram address
// Exceptions:
// Description:    Calculates absolute param index. Returns histo from list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (AbsParamIndex < 0 || AbsParamIndex > HistoList.GetEntriesFast()) return(NULL);
	return((TH1F *) HistoList[AbsParamIndex]);
}

TF1 * TMrbAnalyze::GetCalibration(const Char_t * CalibrationName) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetCalibration
// Purpose:        Get Calibration by its name
// Arguments:      Char_t * CalibrationName     -- calibration name
// Results:        TF1 * CalibAddr              -- address
// Exceptions:
// Description:    Searches for a calibrations function with specified name
//                 Returns address.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;
	TMrbCalibrationListEntry * cle;

	nx = fCalibrationList.FindByName(CalibrationName);
	if (nx == NULL) {
		gMrbLog->Err()	<< "No such calibration - " << CalibrationName << endl;
		gMrbLog->Flush(this->ClassName(), "GetCalibration");
		return(NULL);
	}
	cle = (TMrbCalibrationListEntry *) nx->GetAssignedObject();
	return(cle->GetAddress());
}

TF1 * TMrbAnalyze::GetCalibration(Int_t AbsParamIndex) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetCalibration
// Purpose:        Get calibration address
// Arguments:      Int_t AbsParamIndex       -- absolute param/calib index
// Results:        TF1 * CalibrationAddr     -- address
// Exceptions:
// Description:    Returns calibration address.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;

	if (AbsParamIndex < 0 || AbsParamIndex > fCalibrationList.GetLast()) return(NULL);
	nx = (TMrbNamedX *) fCalibrationList[AbsParamIndex];
	if (nx == NULL) return(NULL);
	return(((TMrbCalibrationListEntry *) nx->GetAssignedObject())->GetAddress());
}

TF1 * TMrbAnalyze::GetCalibration(Int_t ModuleIndex, Int_t RelParamIndex, Double_t & Gain, Double_t & Offset) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetCalibration
// Purpose:        Get gain and offset
// Arguments:      Int_t ModuleIndex         -- module index
//                 Int_t RelParamIndex       -- relative param index
// Results:        TF1 * CalibrationAddr     -- address
//                 Double_t & Gain           -- gain
//                 Double_t & Offset         -- offset
// Exceptions:
// Description:    Returns gain and offset.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TF1 * cal = this->GetCalibration(ModuleIndex, RelParamIndex);
	if (cal) {
		if (cal->GetParameter(0) == 1) {
			Gain = cal->GetParameter(2);
			Offset = cal->GetParameter(1);
		} else {
			gMrbLog->Err()	<< "Calibration not linear - module # " << ModuleIndex << ", param # " << RelParamIndex << endl;
			gMrbLog->Flush(this->ClassName(), "GetCalibration");
			Gain = 1.;
			Offset = 0.;
		}
	} else {
		Gain = 1.;
		Offset = 0.;
	}
	return(cal);
}

TF1 * TMrbAnalyze::GetCalibration(Int_t AbsParamIndex, Double_t & Gain, Double_t & Offset) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetCalibration
// Purpose:        Get gain and offset
// Arguments:      Int_t AbsParamIndex       -- absolute param/calib index
// Results:        TF1 * CalibrationAddr     -- address
//                 Double_t Gain             -- gain
//                 Double_t Offset           -- offset
// Exceptions:
// Description:    Returns gain and offset.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TF1 * cal = this->GetCalibration(AbsParamIndex);
	if (cal) {
		if (cal->GetParameter(0) == 1) {
			Gain = cal->GetParameter(2);
			Offset = cal->GetParameter(1);
		} else {
			gMrbLog->Err()	<< "Calibration not linear - param # " << AbsParamIndex << endl;
			gMrbLog->Flush(this->ClassName(), "GetCalibration");
			Gain = 1.;
			Offset = 0.;
		}
	} else {
		Gain = 1.;
		Offset = 0.;
	}
	return(cal);
}

TF1 * TMrbAnalyze::GetCalibration(Int_t ModuleIndex, Int_t RelParamIndex, TArrayD & Coeffs) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetCalibration
// Purpose:        Get gain and offset
// Arguments:      Int_t ModuleIndex         -- module index
//                 Int_t RelParamIndex       -- relative param index
// Results:        TF1 * CalibrationAddr     -- address
//                 TArrayD & Coeffs          -- calibration coefficients
// Exceptions:
// Description:    Returns calibration coeffs.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Coeffs.Reset(0.);
	TF1 * cal = this->GetCalibration(ModuleIndex, RelParamIndex);
	if (cal) {
		Int_t n = (Int_t) cal->GetParameter(0) + 2;
		Coeffs.Set(n, cal->GetParameters());
	}
	return(cal);
}

TF1 * TMrbAnalyze::GetCalibration(Int_t AbsParamIndex, TArrayD & Coeffs) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetCalibration
// Purpose:        Get gain and offset
// Arguments:      Int_t AbsParamIndex       -- absolute param/calib index
// Results:        TF1 * CalibrationAddr     -- address
//                 TArrayD & Coeffs          -- calibration coefficients
// Exceptions:
// Description:    Returns calibration coeffs.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Coeffs.Reset(0.);
	TF1 * cal = this->GetCalibration(AbsParamIndex);
	if (cal) {
		Int_t n = (Int_t) cal->GetParameter(0) + 2;
		Coeffs.Set(n, cal->GetParameters());
	}
	return(cal);
}

TF1 * TMrbAnalyze::GetCalibration(Int_t ModuleIndex, Int_t RelParamIndex) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetCalibration
// Purpose:        Get calibration by module number and param offset
// Arguments:      Int_t ModuleIndex          -- module index
//                 Int_t RelParamIndex        -- relative param index
// Results:        TF1 * CalibrationAddr      -- calibration address
// Exceptions:
// Description:    Returns calibration addr given by module and param.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;
	TMrbModuleListEntry * mle;
	Int_t px;

	if (ModuleIndex <= 0 || ModuleIndex > fModuleList.GetLast()) return(NULL);
	nx = (TMrbNamedX *) fModuleList[ModuleIndex];
	mle = (TMrbModuleListEntry *) nx->GetAssignedObject();
	if (RelParamIndex >= mle->GetNofParams()) return(NULL);
	px = mle->GetIndexOfFirstParam() + RelParamIndex;
	if (px < 0 || px > fCalibrationList.GetLast()) return(NULL);
	nx = (TMrbNamedX *) fCalibrationList[px];
	if (nx == NULL) return(NULL);
	return(((TMrbCalibrationListEntry *) nx->GetAssignedObject())->GetAddress());
}

TMrbCalibrationListEntry * TMrbAnalyze::GetCalibrationListEntry(Int_t ModuleIndex, Int_t RelParamIndex) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetCalibrationListEntry
// Purpose:        Get calibration entry from list
// Arguments:      Int_t ModuleIndex                     -- module index
//                 Int_t RelParamIndex                   -- relative param index
// Results:        TMrbCalibrationListEntry * ListEntry  -- entry
// Exceptions:
// Description:    Returns calibration list entry.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;
	TMrbModuleListEntry * mle;
	Int_t px;

	if (ModuleIndex <= 0 || ModuleIndex > fModuleList.GetLast()) return(NULL);
	nx = (TMrbNamedX *) fModuleList[ModuleIndex];
	mle = (TMrbModuleListEntry *) nx->GetAssignedObject();
	if (RelParamIndex >= mle->GetNofParams()) return(NULL);
	px = mle->GetIndexOfFirstParam() + RelParamIndex;
	if (px < 0 || px > fCalibrationList.GetLast()) return(NULL);
	nx = (TMrbNamedX *) fCalibrationList[px];
	if (nx == NULL) return(NULL);
	return((TMrbCalibrationListEntry *) nx->GetAssignedObject());
}

TMrbCalibrationListEntry * TMrbAnalyze::GetCalibrationListEntry(Int_t AbsParamIndex) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetCalibrationListEntry
// Purpose:        Get calibration entry from list
// Arguments:      Int_t AbsParamIndex                   -- absolute param/calib index
// Results:        TMrbCalibrationListEntry * ListEntry  -- entry
// Exceptions:
// Description:    Returns calibration list entry.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;

	if (AbsParamIndex < 0 || AbsParamIndex > fParamList.GetLast()) return(NULL);
	nx = (TMrbNamedX *) fCalibrationList[AbsParamIndex];
	if (nx == NULL) return(NULL);
	return((TMrbCalibrationListEntry *) nx->GetAssignedObject());
}

Double_t CalibPoly(Double_t * Xvalues, Double_t * Params) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           CalibPoly
// Purpose:        Calibrate using a polynom of given degree
// Arguments:      Double_t * Xvalues   -- array of x values
//                 Double_t * Params    -- array of parameters
// Results:        Double_t CalibResult -- resulting value
// Exceptions:
// Description:    Evaluates CalibResult = polynom(Xvalues[0])
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Double_t xVal = Xvalues[0];
	Int_t calDegree = (Int_t) Params[0];
	Double_t result = Params[1];
	for (Int_t parNo = 2; parNo <= calDegree + 1; parNo++) {
		result += Params[parNo] * xVal;
		xVal *= Xvalues[0];
	}
	return(result);
}

Int_t TMrbAnalyze::ReadCalibrationFromFile(const Char_t * CalibrationFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::ReadCalibrationFromFile
// Purpose:        Read calibration data from file
// Arguments:      Char_t * CalibrationFile   -- file name
// Results:        Int_t NofEntries           -- number of calibration entries
// Exceptions:
// Description:    Reads calibration data from file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TArrayD param;

	if (gSystem->AccessPathName(CalibrationFile, (EAccessMode) F_OK)) {
		gMrbLog->Err()	<< "Can't open calibration file - " << CalibrationFile << endl;
		gMrbLog->Flush(this->ClassName(), "ReadCalibrationFromFile");
		return(-1);
	}

	TEnv * cal = new TEnv(CalibrationFile);
	Int_t nofCalibs = 0;
	TString calType = cal->GetValue("Calib.Type", "linear");
	Bool_t isLinear;
	calType.ToLower();
	if (calType.CompareTo("linear") == 0) isLinear = kTRUE;
	else if (calType.CompareTo("poly") == 0) isLinear = kFALSE;
	else {
		gMrbLog->Wrn()	<< "Unsupported calibration type - " << calType << ", no calibration" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadCalibrationFromFile");
		return(0);
	}
	Int_t calDegree = cal->GetValue("Calib.Degree", 1);
	if (calDegree < 1) {
		gMrbLog->Err()	<< "Wrong polynomial degree - " << calDegree
						<< " (should be at least 1), no calibration" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadCalibrationFromFile");
		return(-1);
	}
	if (isLinear && calDegree != 1) {
		gMrbLog->Wrn()	<< "Wrong polynomial degree - " << calDegree << " (should be 1 for linear), ignored" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadCalibrationFromFile");
	}

	param.Set(calDegree + 2); 	// n+1 params for degree n, param#0 holds degree itself
	param.Reset(0.);
	param[0] = calDegree; 		// param#0 is used to inform fct calibrate about polynomial degree

	TObject * o = cal->GetTable()->First();
	while (o) {
		TString oName = o->GetName();
		if (oName.Contains(".Xmin")) {
			TString histoName = oName;
			Int_t dot = histoName.Index(".", 0);
			histoName = histoName(dot + 1, 1000);
			dot = histoName.Index(".", 0);
			histoName.Resize(dot);
			TMrbHistoListEntry * hle = this->GetHistoListEntry(histoName.Data());
			if (hle) {
				TString resName = "Calib.";
				resName += histoName;
				resName += ".Xmin";
				Double_t xmin = cal->GetValue(resName.Data(), 0.);
				resName = "Calib.";
				resName += histoName;
				resName += ".Xmax";
				Double_t xmax = cal->GetValue(resName.Data(), 1.);
				TString calName = histoName;
				if (calName(0) == 'h') {
					calName(0) = 'c';
				} else {
					calName(0,1).ToUpper();
					calName.Prepend("c");
				}
				if (isLinear) {
					resName = "Calib.";
					resName += histoName;
					resName += ".Offset";
					param[1] = cal->GetValue(resName.Data(), 0.);
					resName = "Calib.";
					resName += histoName;
					resName += ".Gain";
					param[2] = cal->GetValue(resName.Data(), 1.);
				} else {
					for (Int_t parNo = 1; parNo <= calDegree + 1; parNo++) {
						resName = "Calib.";
						resName += histoName;
						resName += ".A";
						resName += parNo - 1;
						param[parNo] = cal->GetValue(resName.Data(), 0.);
					}
				}
				TF1 * calFct;
				TMrbNamedX * nx = fCalibrationList.FindByName(calName.Data());
				if (nx) {
					TMrbCalibrationListEntry * cle = (TMrbCalibrationListEntry *) nx->GetAssignedObject();
					calFct = cle->GetAddress();
					delete calFct;
				}
				calFct = new TF1(calName.Data(), CalibPoly, xmin, xmax, calDegree + 2);
				calFct->SetParameters(param.GetArray());
				this->AddCalibrationToList(calFct, hle->GetParam()->GetIndex());
				nofCalibs++;
			} else {
				gMrbLog->Wrn()	<< "No such histo - " << histoName << ", calibration omitted" << endl;
				gMrbLog->Flush(this->ClassName(), "ReadCalibrationFromFile");
			}
		}
		o = cal->GetTable()->After(o);
	}
	delete cal;
	return(nofCalibs);
}

void TMrbAnalyze::PrintCalibration(ostream & Out) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::PrintCalibration
// Purpose:        Print calibration
// Arguments:      Out           -- output stream
// Results:        --
// Exceptions:
// Description:    Preints calibration data
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * cal;
	TIterator * iter = fCalibrationList.MakeIterator();
	Bool_t printHdr = kTRUE;
	while ( (cal = (TMrbNamedX *) iter->Next()) ) {
		if (printHdr) {
			Out << endl << "---------------------------------------------------------------------------------------------------------------" << endl;
			Out << "Calibration data:" << endl;
			Out << Form(calFmt1h, "Module", "Param", "Xmin", "Xmax", "Deg");
			for (Int_t i = 0; i < 4; i++) {
				TString coeff = "A";
				coeff += i;
				Out << Form(calFmt2h, coeff.Data());
			}
			Out << " ..." << endl;
			Out << "---------------------------------------------------------------------------------------------------------------" << endl;
		}
		printHdr = kFALSE;
		this->PrintCalibration(Out, cal);
	}
	Out << "---------------------------------------------------------------------------------------------------------------" << endl;
}

void TMrbAnalyze::PrintCalibration(ostream & Out, const Char_t * CalibrationName) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::PrintCalibration
// Purpose:        Print calibration by its name
// Arguments:      Out 		                    -- output
//                 Char_t * CalibrationName     -- calibration name (=histo name)
// Results:        --
// Exceptions:
// Description:    Searches for a calibration function with specified name
//                 Prints settings.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;

	nx = fCalibrationList.FindByName(CalibrationName);
	if (nx == NULL) {
		gMrbLog->Err()	<< "No such calibration - " << CalibrationName << endl;
		gMrbLog->Flush(this->ClassName(), "PrintCalibration");
		return;
	}
	this->PrintCalibration(Out, nx);
}

void TMrbAnalyze::PrintCalibration(ostream & Out, Int_t AbsParamIndex) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::PrintCalibration
// Purpose:        Print calibration by its name
// Arguments:      Out 		                 -- output
//                 Int_t AbsParamIndex       -- absolute param/calib index
// Results:        --
// Exceptions:
// Description:    Prints calibration data.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;

	if (AbsParamIndex < 0 || AbsParamIndex > fCalibrationList.GetLast()) return;
	nx = (TMrbNamedX *) fCalibrationList[AbsParamIndex];
	if (nx == NULL) {
		gMrbLog->Err()	<< "No such calibration - param # " << AbsParamIndex << endl;
		gMrbLog->Flush(this->ClassName(), "PrintCalibration");
		return;
	}
	this->PrintCalibration(Out, nx);
}

void TMrbAnalyze::PrintCalibration(ostream & Out, TMrbNamedX * CalX) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::PrintCalibration
// Purpose:        Print calibration by its name
// Arguments:      Out 		                 -- output
//                 TMrbNameX CalX            -- calibration entry
// Results:        --
// Exceptions:
// Description:    Prints calibration data.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbCalibrationListEntry * cle = (TMrbCalibrationListEntry *) CalX->GetAssignedObject();
	if (cle) {
		TString mod = Form("%s (%d)", cle->GetModule()->GetName(), cle->GetModule()->GetIndex());
		TString par = Form("%s (%d)", cle->GetParam()->GetName(), cle->GetParam()->GetIndex());
		TF1 * cal = cle->GetAddress();
		if (cal) {
			TArrayD params;
			Int_t n = (Int_t) cal->GetParameter(0);
			Out << Form(calFmt1, mod.Data(), par.Data(), (Int_t) cal->GetXmin(), (Int_t) cal->GetXmax(), n);
			n += 2;
			params.Set(n, cal->GetParameters());
			for (Int_t parNo = 1; parNo < n; parNo++) Out << Form(calFmt2, params[parNo]);
			Out << endl;
		}
	}
}

void TMrbAnalyze::PrintCalibration(ostream & Out, Int_t ModuleIndex, Int_t RelParamIndex) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::PrintCalibration
// Purpose:        Print calibration by its name
// Arguments:      Int_t ModuleIndex          -- module index
//                 Int_t RelParamIndex        -- relative param index
// Results:        --
// Exceptions:
// Description:    Prints calibration data.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;
	TMrbModuleListEntry * mle;
	Int_t px;

	if (ModuleIndex <= 0 || ModuleIndex > fModuleList.GetLast()) return;
	nx = (TMrbNamedX *) fModuleList[ModuleIndex];
	mle = (TMrbModuleListEntry *) nx->GetAssignedObject();
	if (RelParamIndex >= mle->GetNofParams()) return;
	px = mle->GetIndexOfFirstParam() + RelParamIndex;
	if (px < 0 || px > fCalibrationList.GetLast()) return;
	nx = (TMrbNamedX *) fCalibrationList[px];
	if (nx == NULL) {
		gMrbLog->Err()	<< "No such calibration - module # " << ModuleIndex << ", param # " << RelParamIndex << endl;
		gMrbLog->Flush(this->ClassName(), "PrintCalibration");
		return;
	}
	TMrbCalibrationListEntry * cle = (TMrbCalibrationListEntry *) nx->GetAssignedObject();
	if (cle) {
		TString mod = Form("%s (%d)", cle->GetModule()->GetName(), cle->GetModule()->GetIndex());
		TString par = Form("%s (%d)", cle->GetParam()->GetName(), cle->GetParam()->GetIndex());
		TF1 * cal = cle->GetAddress();
		if (cal) {
			Int_t n = (Int_t) cal->GetParameter(0);
			Out << Form(calFmt1, mod.Data(), par.Data(), n);
			for (Int_t parNo = 1; parNo <= n + 1; parNo++) Out << Form(calFmt2, cal->GetParameter(parNo));
			Out << endl;
		}
	}
}

TF1 * TMrbAnalyze::GetDCorr(const Char_t * DCorrName) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetDCorr
// Purpose:        Get doppler correction by its name
// Arguments:      Char_t * DCorrName     -- dcorr name
// Results:        TF1 * DCorrAddr        -- address
// Exceptions:
// Description:    Searches for a dcorr function with specified name
//                 Returns address.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;
	TMrbDCorrListEntry * dcle;

	nx = fDCorrList.FindByName(DCorrName);
	if (nx == NULL) {
		gMrbLog->Err()	<< "No such doppler correction - " << DCorrName << endl;
		gMrbLog->Flush(this->ClassName(), "GetDCorr");
		return(NULL);
	}
	dcle = (TMrbDCorrListEntry *) nx->GetAssignedObject();
	return(dcle->GetAddress());
}

TF1 * TMrbAnalyze::GetDCorr(Int_t AbsParamIndex) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetDCorr
// Purpose:        Get doppler correction address
// Arguments:      Int_t AbsParamIndex       -- absolute param/calib index
// Results:        TF1 * DCorrAddr           -- address
// Exceptions:
// Description:    Returns dcorr address.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;

	if (AbsParamIndex < 0 || AbsParamIndex > fParamList.GetLast()) return(NULL);
	nx = (TMrbNamedX *) fDCorrList[AbsParamIndex];
	if (nx == NULL) return(NULL);
	return(((TMrbDCorrListEntry *) nx->GetAssignedObject())->GetAddress());
}

TF1 * TMrbAnalyze::GetDCorr(Int_t ModuleIndex, Int_t RelParamIndex, Double_t & Factor) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetDCorr
// Purpose:        Get doppler corr factor
// Arguments:      Int_t ModuleIndex         -- module index
//                 Int_t RelParamIndex       -- relative param index
// Results:        TF1 * DCorrAddr           -- address
//                 Double_t & Factor         -- dcorr factor
// Exceptions:
// Description:    Returns dcorr factor. Valid only for dcorr type 'ConstFactor'.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TF1 * dcorr = this->GetDCorr(ModuleIndex, RelParamIndex);
	if (dcorr) {
		Factor = dcorr->GetParameter(0);
	} else {
		Factor = 1.;
	}
	return(dcorr);
}

TF1 * TMrbAnalyze::GetDCorr(Int_t AbsParamIndex, Double_t & Factor) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetDCorr
// Purpose:        Get doppler corr factor
// Arguments:      Int_t AbsParamIndex       -- absolute param/calib index
// Results:        TF1 * DCorrAddr           -- address
//                 Double_t & Factor         -- dcorr factor
// Exceptions:
// Description:    Returns dcorr factor. Valid only for dcorr type 'ConstFactor'.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TF1 * dcorr = this->GetCalibration(AbsParamIndex);
	if (dcorr) {
		Factor = dcorr->GetParameter(0);
	} else {
		Factor = 1.;
	}
	return(dcorr);
}

TF1 * TMrbAnalyze::GetDCorr(Int_t ModuleIndex, Int_t RelParamIndex, Double_t & Beta, Double_t & Angle) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetDCorr
// Purpose:        Get doppler corr factor
// Arguments:      Int_t ModuleIndex         -- module index
//                 Int_t RelParamIndex       -- relative param index
// Results:        TF1 * DCorrAddr           -- address
//                 Double_t & Beta           -- velocity
//                 Double_t & Angle          -- angle
//                 Bool_t InDegrees          -- angle to be returned in degrees
// Exceptions:
// Description:    Returns dcorr factor. Valid only for dcorr type 'FixedAngle'.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TF1 * dcorr = this->GetDCorr(ModuleIndex, RelParamIndex);
	if (dcorr) {
		Beta = dcorr->GetParameter(0);
		Angle = dcorr->GetParameter(1);
	} else {
		Beta = 0.;
		Angle = 0.;
	}
	return(dcorr);
}

TF1 * TMrbAnalyze::GetDCorr(Int_t AbsParamIndex, Double_t & Beta, Double_t & Angle) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetDCorr
// Purpose:        Get doppler corr factor
// Arguments:      Int_t AbsParamIndex       -- absolute param/calib index
// Results:        TF1 * DCorrAddr           -- address
//                 Double_t & Beta           -- velocity
//                 Double_t & Angle          -- angle
// Exceptions:
// Description:    Returns dcorr factor. Valid only for dcorr type 'FixedAngle'.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TF1 * dcorr = this->GetDCorr(AbsParamIndex);
	if (dcorr) {
		Beta = dcorr->GetParameter(0);
		Angle = dcorr->GetParameter(1);
	} else {
		Beta = 0.;
		Angle = 0.;
	}
	return(dcorr);
}

TF1 * TMrbAnalyze::GetDCorr(Int_t ModuleIndex, Int_t RelParamIndex) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetDCorr
// Purpose:        Get calibration by module number and param offset
// Arguments:      Int_t ModuleIndex          -- module index
//                 Int_t RelParamIndex        -- relative param index
// Results:        TF1 * DCorrAddr            -- calibration address
// Exceptions:
// Description:    Returns dcoor addr given by module and param.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;
	TMrbModuleListEntry * mle;
	Int_t px;

	if (ModuleIndex <= 0 || ModuleIndex > fModuleList.GetLast()) return(NULL);
	nx = (TMrbNamedX *) fModuleList[ModuleIndex];
	mle = (TMrbModuleListEntry *) nx->GetAssignedObject();
	if (RelParamIndex >= mle->GetNofParams()) return(NULL);
	px = mle->GetIndexOfFirstParam() + RelParamIndex;
	if (px < 0 || px > fDCorrList.GetLast()) return(NULL);
	nx = (TMrbNamedX *) fDCorrList[px];
	if (nx == NULL) return(NULL);
	return(((TMrbDCorrListEntry *) nx->GetAssignedObject())->GetAddress());
}

TMrbDCorrListEntry * TMrbAnalyze::GetDCorrListEntry(Int_t ModuleIndex, Int_t RelParamIndex) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetDCorrListEntry
// Purpose:        Get doppler corr entry from list
// Arguments:      Int_t ModuleIndex                     -- module index
//                 Int_t RelParamIndex                   -- relative param index
// Results:        TMrbDCorrListEntry * ListEntry        -- entry
// Exceptions:
// Description:    Returns doppler corr list entry.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;
	TMrbModuleListEntry * mle;
	Int_t px;

	if (ModuleIndex <= 0 || ModuleIndex > fModuleList.GetLast()) return(NULL);
	nx = (TMrbNamedX *) fModuleList[ModuleIndex];
	mle = (TMrbModuleListEntry *) nx->GetAssignedObject();
	if (RelParamIndex >= mle->GetNofParams()) return(NULL);
	px = mle->GetIndexOfFirstParam() + RelParamIndex;
	if (px < 0 || px > fDCorrList.GetLast()) return(NULL);
	nx = (TMrbNamedX *) fDCorrList[px];
	if (nx == NULL) return(NULL);
	return((TMrbDCorrListEntry *) nx->GetAssignedObject());
}

TMrbDCorrListEntry * TMrbAnalyze::GetDCorrListEntry(Int_t AbsParamIndex) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::TMrbDCorrListEntry
// Purpose:        Get doppler corr entry from list
// Arguments:      Int_t AbsParamIndex                   -- absolute param/calib index
// Results:        TMrbDCorrListEntry * ListEntry        -- entry
// Exceptions:
// Description:    Returns calibration list entry.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;

	if (AbsParamIndex < 0 || AbsParamIndex > fParamList.GetLast()) return(NULL);
	nx = (TMrbNamedX *) fDCorrList[AbsParamIndex];
	if (nx == NULL) return(NULL);
	return((TMrbDCorrListEntry *) nx->GetAssignedObject());
}

Double_t DCorrConstFactor(Double_t * Xvalues, Double_t * Params) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DCorrConstFactor
// Purpose:        Doppler correction using a constant factor
// Arguments:      Double_t * Xvalues   -- array of x values
//                 Double_t * Params    -- array of parameters
// Results:        Double_t EnergyDC    -- resulting energy value
// Exceptions:
// Description:    Returns energy corrected by a const factor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Double_t energyDC = Params[0] * Xvalues[0];
	return(energyDC);
}

Double_t DCorrFixedAngle(Double_t * Xvalues, Double_t * Params) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DCorrFixedAnlge
// Purpose:        Doppler correction using a fixed angle
// Arguments:      Double_t * Xvalues   -- array of x values
//                 Double_t * Params    -- array of parameters
// Results:        Double_t EnergyDC    -- resulting energy value
// Exceptions:
// Description:    Returns energy corrected due to angle and velocity.
//                 Angle given in radians.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Double_t beta = Params[0];
	Double_t angle = Params[1];
	Double_t energy = Xvalues[0];

	Double_t energyDC = (1 - beta * TMath::Cos(angle)) * energy;
	return(energyDC);
}

Int_t TMrbAnalyze::ReadDCorrFromFile(const Char_t * DCorrFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::ReadDCorrFromFile
// Purpose:        Read doppler correction data from file
// Arguments:      Char_t * ReadDCorrFromFile    -- file name
// Results:        Int_t NofEntries              -- number of calibration entries
// Exceptions:
// Description:    Reads dcorr data from file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbLofNamedX dcorrTypes;

	if (gSystem->AccessPathName(DCorrFile, (EAccessMode) F_OK)) {
		gMrbLog->Err()	<< "Can't open doppler correction file - " << DCorrFile << endl;
		gMrbLog->Flush(this->ClassName(), "ReadDCorrFromFile");
		return(-1);
	}

	TEnv * dcorr = new TEnv(DCorrFile);
	dcorrTypes.AddNamedX(kMrbLofDCorrTypes);
	const Char_t * dt = dcorr->GetValue("DCorr.Type", "None");
	TMrbNamedX * nx = dcorrTypes.FindByName(dt);
	if (nx == NULL) {
		gMrbLog->Err()	<< "Doppler correction file is of wrong type \"" <<  dt << "\" - " << DCorrFile << endl;
		gMrbLog->Flush(this->ClassName(), "ReadDCorrFromFile");
		return(-1);
	}

	Double_t beta = dcorr->GetValue("DCorr.Beta", 0.);
	Bool_t angleInDegrees = dcorr->GetValue("DCorr.AngleInDegrees", kTRUE);

	Int_t nofDCorrs = 0;
	TObject * o = dcorr->GetTable()->First();
	while (o) {
		TString oName = o->GetName();
		if (oName.Contains(".Xmin")) {
			TString histoName = oName;
			Int_t dot = histoName.Index(".", 0);
			histoName = histoName(dot + 1, 1000);
			dot = histoName.Index(".", 0);
			histoName.Resize(dot);
			TMrbHistoListEntry * hle = this->GetHistoListEntry(histoName.Data());
			if (hle) {
				TString resName = "DCorr.";
				resName += histoName;
				resName += ".Xmin";
				Double_t xmin = dcorr->GetValue(resName.Data(), 0.);
				resName = "DCorr.";
				resName += histoName;
				resName += ".Xmax";
				Double_t xmax = dcorr->GetValue(resName.Data(), 1.);
				resName = "DCorr.";
				resName += histoName;
				resName += ".Factor";
				TString x = dcorr->GetValue(resName.Data(), "");
				if (!x.IsNull() & nx->GetIndex() != TMrbDCorrListEntry::kDCorrConstFactor) {
					gMrbLog->Err()	<< "Doppler correction file has wrong entry \"" <<  resName << ": " << x << "\" - " << DCorrFile << endl;
					gMrbLog->Flush(this->ClassName(), "ReadDCorrFromFile");
					return(-1);
				}
				Double_t fact = dcorr->GetValue(resName.Data(), 1.);
				resName = "DCorr.";
				resName += histoName;
				resName += ".Angle";
				x = dcorr->GetValue(resName.Data(), "");
				Double_t angle = dcorr->GetValue(resName.Data(), 0.);
				if (!x.IsNull() & nx->GetIndex() == TMrbDCorrListEntry::kDCorrConstFactor) {
					gMrbLog->Err()	<< "Doppler correction file has wrong entry \"" <<  resName << ": " << x << "\" - " << DCorrFile << endl;
					gMrbLog->Flush(this->ClassName(), "ReadDCorrFromFile");
					return(-1);
				}
				TString dcName = histoName;
				if (dcName(0) == 'h') {
					dcName(0) = 'c';
					dcName.Prepend("d");
				} else {
					dcName(0,1).ToUpper();
					dcName.Prepend("dc");
				}
				TF1 * dcFct = NULL;
				if (nx->GetIndex() == TMrbDCorrListEntry::kDCorrConstFactor) {
					dcFct = new TF1(dcName.Data(), DCorrConstFactor, xmin, xmax, 1);
					dcFct->SetParameter(0, fact);
				} else if (nx->GetIndex() == TMrbDCorrListEntry::kDCorrFixedAngle) {
					dcFct = new TF1(dcName.Data(), DCorrFixedAngle, xmin, xmax, 2);
					dcFct->SetParameter(0, beta);
					if (angleInDegrees) angle *= TMath::Pi() / 180.;
					dcFct->SetParameter(1, angle);
				}
				if (dcFct) {
					this->AddDCorrToList(dcFct, hle->GetParam()->GetIndex());
					nofDCorrs++;
				}
			}
		}
		o = dcorr->GetTable()->After(o);
	}
	return(nofDCorrs);
}

Bool_t TMrbAnalyze::AddDCorrToList(TF1 * DCorrAddr, Int_t ModuleIndex, Int_t RelParamIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::AddDCorrToList
// Purpose:        Add a new doppler correction function to list
// Arguments:      TF1 * DCorrAddr        -- address
//                 Int_t ModuleIndex      -- list index
//                 Int_t RelParamIndex    -- relative param index
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Creates a new entry in dcorr list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nmx;
	TMrbNamedX * npx;
	TMrbNamedX * ncx;
	TMrbModuleListEntry * mle;
	TMrbParamListEntry * ple;
	TMrbDCorrListEntry * dcle;
	Int_t px;

	if (ModuleIndex <= 0 || ModuleIndex > fModuleList.GetLast()) {
		gMrbLog->Err()	<< "[" << DCorrAddr->GetName()
						<< "] Module index out of range - " << ModuleIndex
						<< " (should be in [1," << fModuleList.GetLast() << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "AddDCorrToList");
		return(kFALSE);
	}
	nmx = (TMrbNamedX *) fModuleList[ModuleIndex];
	if (nmx == NULL) {
		gMrbLog->Err()	<< "[" << DCorrAddr->GetName() << "] Module index not in use - " << ModuleIndex << endl;
		gMrbLog->Flush(this->ClassName(), "AddDCorrToList");
		return(kFALSE);
	}
	mle = (TMrbModuleListEntry *) nmx->GetAssignedObject();
	px = mle->GetIndexOfFirstParam();
	ncx = (TMrbNamedX *) fDCorrList[px + RelParamIndex];
	npx = (TMrbNamedX *) fParamList[px + RelParamIndex];
	if (npx == NULL) {
		gMrbLog->Err()	<< "[" << DCorrAddr->GetName() << "] No param assigned to index " << RelParamIndex
						<< " (abs " << px + RelParamIndex << ")" << endl;
		gMrbLog->Flush(this->ClassName(), "AddDCorrToList");
		return(kFALSE);
	}
	ple = (TMrbParamListEntry *) npx->GetAssignedObject();
	if (ncx == NULL) {
		ncx = new TMrbNamedX(npx->GetIndex(), DCorrAddr->GetName(), DCorrAddr->GetTitle());
		dcle = new TMrbDCorrListEntry(nmx, npx, DCorrAddr);
		ncx->AssignObject(dcle);
		fDCorrList.AddAt(ncx, npx->GetIndex());
	} else {
		dcle = (TMrbDCorrListEntry *) ncx->GetAssignedObject();
	}
	dcle->SetAddress(DCorrAddr);
	ple->SetDCorrAddress(DCorrAddr);
	return(kTRUE);
}

Bool_t TMrbAnalyze::AddDCorrToList(TF1 * DCorrAddr, Int_t AbsParamIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::AddDCorrToList
// Purpose:        Add a new dcorr function to list
// Arguments:      TF1 * DCorrAddr        -- address
//                 Int_t AbsParamIndex    -- absolute param index
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Creates a new entry in dcorr list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * npx;
	TMrbNamedX * ncx;
	TMrbParamListEntry * ple;
	TMrbDCorrListEntry * dcle;

	ncx = (TMrbNamedX *) fDCorrList[AbsParamIndex];
	npx = (TMrbNamedX *) fParamList[AbsParamIndex];
	if (npx == NULL) {
		gMrbLog->Err()	<< "[" << DCorrAddr->GetName() << "] No param assigned to index "
						<< AbsParamIndex << " (abs)" << endl;
		gMrbLog->Flush(this->ClassName(), "AddDCorrToList");
		return(kFALSE);
	}
	ple = (TMrbParamListEntry *) npx->GetAssignedObject();
	if (ncx == NULL) {
		ncx = new TMrbNamedX(npx->GetIndex(), DCorrAddr->GetName(), DCorrAddr->GetTitle());
		dcle = new TMrbDCorrListEntry(ple->GetModule(), npx, DCorrAddr);
		ncx->AssignObject(dcle);
		fDCorrList.AddAt(ncx, AbsParamIndex);
	} else {
		dcle = (TMrbDCorrListEntry *) ncx->GetAssignedObject();
	}
	dcle->SetAddress(DCorrAddr);
	ple->SetDCorrAddress(DCorrAddr);
	return(kTRUE);
}

TF1 * TMrbAnalyze::AddDCorrToList(	const Char_t * Name, const Char_t * Formula,
											Double_t Xmin, Double_t Xmax,
											Int_t ModuleIndex, Int_t RelParamIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::AddDCorrToList
// Purpose:        Add a new dcorr function to list
// Arguments:      Char_t * Name          -- name of calibration function
//                 Char_t * Formula       -- formula (type B)
//                 Double_t Xmin          -- minimum x
//                 Double_t Xmax          -- maximum x
//                 Int_t ModuleIndex      -- list index
//                 Int_t RelParamIndex    -- relative param index
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Creates a new entry in dcorr list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TF1 * dcorr = new TF1(Name, Formula, Xmin, Xmax);
	if (this->AddDCorrToList(dcorr, ModuleIndex, RelParamIndex)) return(dcorr); else return(NULL);
}

TObject * TMrbAnalyze::GetParamAddr(const Char_t * ParamName) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetParamAddr
// Purpose:        Get param addr by name
// Arguments:      Char_t * ParamName     -- param name
// Results:        TObject * ParamAddr    -- address
// Exceptions:
// Description:    Searches for a param with specified name
//                 Returns param address.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;
	TMrbParamListEntry * ple;

	nx = fParamList.FindByName(ParamName);
	if (nx == NULL) {
		gMrbLog->Err()	<< "No such param - " << ParamName << endl;
		gMrbLog->Flush(this->ClassName(), "GetParamAddr");
		return(NULL);
	}
	ple = (TMrbParamListEntry *) nx->GetAssignedObject();
	return(ple->GetAddress());
}

TObject * TMrbAnalyze::GetParamAddr(Int_t ModuleIndex, Int_t RelParamIndex) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetParamAddr
// Purpose:        Get param address
// Arguments:      Int_t ModuleIndex      -- module index
//                 Int_t RelParamIndex    -- relative param index
// Results:        Int_t * ParamAddr      -- param address
// Exceptions:
// Description:    Returns param address.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;
	TMrbModuleListEntry * mle;
	Int_t px;

	if (ModuleIndex <= 0 || ModuleIndex > fModuleList.GetLast()) return(NULL);
	nx = (TMrbNamedX *) fModuleList[ModuleIndex];
	if (nx == NULL) return(NULL);
	mle = (TMrbModuleListEntry *) nx->GetAssignedObject();
	if (RelParamIndex >= mle->GetNofParams()) return(NULL);
	px = mle->GetIndexOfFirstParam() + RelParamIndex;
	if (px < 0 || px > fParamList.GetLast()) return(NULL);
	nx = (TMrbNamedX *) fParamList[px];
	if (nx == NULL) return(NULL);
	return(((TMrbParamListEntry *) nx->GetAssignedObject())->GetAddress());
}

TObject * TMrbAnalyze::GetParamAddr(Int_t AbsParamIndex) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetParamAddr
// Purpose:        Get param address
// Arguments:      Int_t AbsParamIndex    -- absolute param index
// Results:        TObject * ParamAddr    -- param address
// Exceptions:
// Description:    Returns param address.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;

	if (AbsParamIndex < 0 || AbsParamIndex > fParamList.GetLast()) return(NULL);
	nx = (TMrbNamedX *) fParamList[AbsParamIndex];
	if (nx == NULL) return(NULL);
	return(((TMrbParamListEntry *) nx->GetAssignedObject())->GetAddress());
}

Bool_t TMrbAnalyze::AddModuleToList(const Char_t * ModuleName, const Char_t * ModuleTitle,
													Int_t ModuleIndex, Int_t AbsParamIndex,
													Int_t NofParams, Int_t TimeOffset) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::AddModuleToList
// Purpose:        Add a new module to list
// Arguments:      Char_t * ModuleName    -- name
//                 Char_t * ModuleTitle   -- title
//                 Int_t ModuleIndex      -- list index
//                 Int_t AbsParamIndex    -- absolute index of first param
//                 Int_t NofParams        -- number of active params
//                 Int_t TimeOffset       -- time offset
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Creates a new entry in module list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;
	TMrbModuleListEntry * mle;

	if (ModuleIndex <= 0 || ModuleIndex > fModuleList.GetLast()) {
		gMrbLog->Err()	<< "[" << ModuleName << "] Module index out of range - " << ModuleIndex
						<< " (should be in [1," << fModuleList.GetLast() << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "AddModuleToList");
		return(kFALSE);
	}
	nx = (TMrbNamedX *) fModuleList[ModuleIndex];
	if (nx != NULL) {
		gMrbLog->Err()	<< "[" << ModuleName << "] Module index " << ModuleIndex
						<< " already used by module " << nx->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "AddModuleToList");
		return(kFALSE);
	}
	nx = new TMrbNamedX(ModuleIndex, ModuleName, ModuleTitle);
	fModuleList.AddAt(nx, ModuleIndex);
	Bool_t ok = kTRUE;
	for (Int_t i = AbsParamIndex; i < AbsParamIndex + NofParams; i++) {
		if (fParamList[i] != NULL) {
			gMrbLog->Err()	<< "[" << ModuleName << "] param entry " << i << " not empty" << endl;
			gMrbLog->Flush(this->ClassName(), "AddModuleToList");
			ok = kFALSE;
		}
	}
	for (Int_t i = AbsParamIndex; i < AbsParamIndex + NofParams; i++) {
		if (fHistoList[i] != NULL) {
			gMrbLog->Err()	<< "[" << ModuleName << "] histogram entry " << i << " not empty" << endl;
			gMrbLog->Flush(this->ClassName(), "AddModuleToList");
			ok = kFALSE;
		}
	}
	if (!ok) return(kFALSE);

	mle = new TMrbModuleListEntry(NofParams, AbsParamIndex, TimeOffset, NULL, NULL);
	nx->AssignObject(mle);
	fModuleList.AddAt(nx, ModuleIndex);
	return(kTRUE);
}

Bool_t TMrbAnalyze::AddParamToList(const Char_t * ParamName, TObject * ParamAddr, Int_t ModuleIndex, Int_t RelParamIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::AddParamToList
// Purpose:        Add a new param to list
// Arguments:      Char_t * ParamName     -- name
//                 Int_t * ParamAddr      -- address
//                 Int_t ModuleIndex      -- list index
//                 Int_t RelParamIndex    -- relative param index
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Creates a new entry in param list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nmx;
	TMrbNamedX * npx;
	TMrbModuleListEntry * mle;
	TMrbParamListEntry * ple;
	Int_t px;

	if (ModuleIndex <= 0 || ModuleIndex > fModuleList.GetLast()) {
		gMrbLog->Err()	<< "[" << ParamName << "] Module index out of range - " << ModuleIndex
						<< " (should be in [1," << fModuleList.GetLast() << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "AddParamToList");
		return(kFALSE);
	}
	nmx = (TMrbNamedX *) fModuleList[ModuleIndex];
	if (nmx == NULL) {
		gMrbLog->Err()	<< "[" << ParamName << "] Module index not in use - " << ModuleIndex << endl;
		gMrbLog->Flush(this->ClassName(), "AddParamToList");
		return(kFALSE);
	}
	mle = (TMrbModuleListEntry *) nmx->GetAssignedObject();
	px = mle->GetIndexOfFirstParam();
	npx = (TMrbNamedX *) fParamList[px + RelParamIndex];
	if (npx != NULL) return(kFALSE);
	npx = new TMrbNamedX(px + RelParamIndex, ParamName);
	ple = new TMrbParamListEntry(nmx, (TObject *) ParamAddr, NULL);
	npx->AssignObject(ple);
	fParamList.AddAt(npx, px + RelParamIndex);
	if (RelParamIndex == 0) mle->SetFirstParam(npx);
	return(kTRUE);
}

Bool_t TMrbAnalyze::AddHistoToList(TH1 * HistoAddr, Int_t ModuleIndex, Int_t RelParamIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::AddHistoToList
// Purpose:        Add a new histo to list
// Arguments:      TH1 * HistoAddr        -- address
//                 Int_t ModuleIndex      -- list index
//                 Int_t RelParamIndex    -- relative param index
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Creates a new entry in histo list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nmx;
	TMrbNamedX * npx;
	TMrbNamedX * nhx;
	TMrbModuleListEntry * mle;
	TMrbParamListEntry * ple;
	TMrbHistoListEntry * hle;
	Int_t px;

	if (ModuleIndex <= 0 || ModuleIndex > fModuleList.GetLast()) {
		gMrbLog->Err()	<< "[" << HistoAddr->GetName()
						<< "] Module index out of range - " << ModuleIndex
						<< " (should be in [1," << fModuleList.GetLast() << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "AddHistoToList");
		return(kFALSE);
	}
	nmx = (TMrbNamedX *) fModuleList[ModuleIndex];
	if (nmx == NULL) {
		gMrbLog->Err()	<< "[" << HistoAddr->GetName() << "] Module index not in use - " << ModuleIndex << endl;
		gMrbLog->Flush(this->ClassName(), "AddHistoToList");
		return(kFALSE);
	}
	mle = (TMrbModuleListEntry *) nmx->GetAssignedObject();
	px = mle->GetIndexOfFirstParam();
	nhx = (TMrbNamedX *) fHistoList[px + RelParamIndex];
	npx = (TMrbNamedX *) fParamList[px + RelParamIndex];
	if (npx == NULL) {
		gMrbLog->Err()	<< "[" << HistoAddr->GetName() << "] No param assigned to index " << RelParamIndex
						<< " (abs " << px + RelParamIndex << ")" << endl;
		gMrbLog->Flush(this->ClassName(), "AddHistoToList");
		return(kFALSE);
	}
	ple = (TMrbParamListEntry *) npx->GetAssignedObject();
	if (nhx == NULL) {
		nhx = new TMrbNamedX(px, HistoAddr->GetName(), HistoAddr->GetTitle());
		hle = new TMrbHistoListEntry(nmx, npx, HistoAddr);
		nhx->AssignObject(hle);
		fHistoList.AddAt(nhx, px + RelParamIndex);
	} else {
		hle = (TMrbHistoListEntry *) nhx->GetAssignedObject();
	}
	hle->SetAddress(HistoAddr);
	ple->SetHistoAddress(HistoAddr);
	if (RelParamIndex == 0) mle->SetFirstHisto(nhx);
	return(kTRUE);
}

Int_t TMrbAnalyze::GetHistoIndex(Int_t ModuleIndex, Int_t RelParamIndex) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetHistoIndex
// Purpose:        Return absolute histo index
// Arguments:      Int_t ModuleIndex      -- list index
//                 Int_t RelParamIndex    -- relative param index
// Results:        Int_t HistoIndex       -- index
// Exceptions:
// Description:    Returns histo index
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nmx;
	TMrbModuleListEntry * mle;
	Int_t px;

	if (ModuleIndex <= 0 || ModuleIndex > fModuleList.GetLast()) {
		gMrbLog->Err()	<< "Module index out of range - " << ModuleIndex
						<< " (should be in [1," << fModuleList.GetLast() << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "GetHistoIndex");
		return(kFALSE);
	}
	nmx = (TMrbNamedX *) fModuleList[ModuleIndex];
	if (nmx == NULL) {
		gMrbLog->Err()	<< "Module index not in use - " << ModuleIndex << endl;
		gMrbLog->Flush(this->ClassName(), "GetHistoIndex");
		return(kFALSE);
	}
	mle = (TMrbModuleListEntry *) nmx->GetAssignedObject();
	px = mle->GetIndexOfFirstParam();
	return(px + RelParamIndex);
}

Bool_t TMrbAnalyze::AddCalibrationToList(TF1 * CalibrationAddr, Int_t ModuleIndex, Int_t RelParamIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::AddCalibrationToList
// Purpose:        Add a new calibration function to list
// Arguments:      TF1 * CalibrationAddr  -- address
//                 Int_t ModuleIndex      -- list index
//                 Int_t RelParamIndex    -- relative param index
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Creates a new entry in calibration list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nmx;
	TMrbNamedX * npx;
	TMrbNamedX * ncx;
	TMrbModuleListEntry * mle;
	TMrbParamListEntry * ple;
	TMrbCalibrationListEntry * cle;
	Int_t px;

	if (ModuleIndex <= 0 || ModuleIndex > fModuleList.GetLast()) {
		gMrbLog->Err()	<< "[" << CalibrationAddr->GetName()
						<< "] Module index out of range - " << ModuleIndex
						<< " (should be in [1," << fModuleList.GetLast() << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "AddCalibrationToList");
		return(kFALSE);
	}
	nmx = (TMrbNamedX *) fModuleList[ModuleIndex];
	if (nmx == NULL) {
		gMrbLog->Err()	<< "[" << CalibrationAddr->GetName() << "] Module index not in use - " << ModuleIndex << endl;
		gMrbLog->Flush(this->ClassName(), "AddCalibrationToList");
		return(kFALSE);
	}
	mle = (TMrbModuleListEntry *) nmx->GetAssignedObject();
	px = mle->GetIndexOfFirstParam();
	ncx = (TMrbNamedX *) fCalibrationList[px + RelParamIndex];
	npx = (TMrbNamedX *) fParamList[px + RelParamIndex];
	if (npx == NULL) {
		gMrbLog->Err()	<< "[" << CalibrationAddr->GetName() << "] No param assigned to index " << RelParamIndex
						<< " (abs " << px + RelParamIndex << ")" << endl;
		gMrbLog->Flush(this->ClassName(), "AddCalibrationToList");
		return(kFALSE);
	}
	ple = (TMrbParamListEntry *) npx->GetAssignedObject();
	if (ncx == NULL) {
		ncx = new TMrbNamedX(npx->GetIndex(), CalibrationAddr->GetName(), CalibrationAddr->GetTitle());
		cle = new TMrbCalibrationListEntry(nmx, npx, CalibrationAddr);
		ncx->AssignObject(cle);
		fCalibrationList.AddAt(ncx, npx->GetIndex());
	} else {
		cle = (TMrbCalibrationListEntry *) ncx->GetAssignedObject();
	}
	cle->SetAddress(CalibrationAddr);
	ple->SetCalibrationAddress(CalibrationAddr);
	return(kTRUE);
}

Bool_t TMrbAnalyze::AddCalibrationToList(TF1 * CalibrationAddr, Int_t AbsParamIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::AddCalibrationToList
// Purpose:        Add a new calibration function to list
// Arguments:      TF1 * CalibrationAddr  -- address
//                 Int_t AbsParamIndex    -- absolute param index
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Creates a new entry in calibration list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * npx;
	TMrbNamedX * ncx;
	TMrbParamListEntry * ple;
	TMrbCalibrationListEntry * cle;

	ncx = (TMrbNamedX *) fCalibrationList[AbsParamIndex];
	npx = (TMrbNamedX *) fParamList[AbsParamIndex];
	if (npx == NULL) {
		gMrbLog->Err()	<< "[" << CalibrationAddr->GetName() << "] No param assigned to index "
						<< AbsParamIndex << " (abs)" << endl;
		gMrbLog->Flush(this->ClassName(), "AddCalibrationToList");
		return(kFALSE);
	}
	ple = (TMrbParamListEntry *) npx->GetAssignedObject();
	if (ncx == NULL) {
		ncx = new TMrbNamedX(npx->GetIndex(), CalibrationAddr->GetName(), CalibrationAddr->GetTitle());
		cle = new TMrbCalibrationListEntry(ple->GetModule(), npx, CalibrationAddr);
		ncx->AssignObject(cle);
		fCalibrationList.AddAt(ncx, AbsParamIndex);
	} else {
		cle = (TMrbCalibrationListEntry *) ncx->GetAssignedObject();
	}
	cle->SetAddress(CalibrationAddr);
	ple->SetCalibrationAddress(CalibrationAddr);
	return(kTRUE);
}

TF1 * TMrbAnalyze::AddCalibrationToList(	const Char_t * Name, const Char_t * Formula,
											Double_t Xmin, Double_t Xmax,
											Int_t ModuleIndex, Int_t RelParamIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::AddCalibrationToList
// Purpose:        Add a new calibration function to list
// Arguments:      Char_t * Name          -- name of calibration function
//                 Char_t * Formula       -- formula (type B)
//                 Double_t Xmin          -- minimum x
//                 Double_t Xmax          -- maximum x
//                 Int_t ModuleIndex      -- list index
//                 Int_t RelParamIndex    -- relative param index
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Creates a new entry in calibration list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TF1 * cal = new TF1(Name, Formula, Xmin, Xmax);
	if (this->AddCalibrationToList(cal, ModuleIndex, RelParamIndex)) return(cal); else return(NULL);
}

void TMrbAnalyze::PrintLists(ostream & Out) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::PrintLists
// Purpose:        Output list contents
// Arguments:      ostream & Out     -- output stream
// Results:        --
// Exceptions:
// Description:    Printout of module, param, histo lists, resp.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Out	<< endl;
	TIterator * miter = fModuleList.MakeIterator();
	TMrbNamedX * nmx;
	while (nmx = (TMrbNamedX *) miter->Next()) {
		Out	<< "Module " << nmx->GetName() << ", Index = "
			<< nmx->GetIndex() << " (" << nmx->GetTitle() << ")" << endl;
		TMrbModuleListEntry * mle = (TMrbModuleListEntry *) nmx->GetAssignedObject();
		Int_t np = mle->GetNofParams();
		Int_t px = mle->GetIndexOfFirstParam();
		Bool_t h = kFALSE;
		for (Int_t i = 0; i < np; i++) {
			TMrbNamedX * npx = (TMrbNamedX *) fParamList[px];
			if (npx) {
				if (!h) {
					Out	<< "Param     RelX AbsX           Addr     Histogram                     Addr" << endl
						<< "-----------------------------------------------------------------------------------" << endl;
					h = kTRUE;
				}
				TMrbParamListEntry * ple = (TMrbParamListEntry *) npx->GetAssignedObject();
				TMrbNamedX * nhx = (TMrbNamedX *) fHistoList[px];
				TMrbHistoListEntry * hle;
				if (nhx) hle = (TMrbHistoListEntry *) nhx->GetAssignedObject();
				if (ple) {
					Out		<< setiosflags(ios::left) << setw(10) << npx->GetName()
							<< resetiosflags(ios::left)	<< setw(4) << i
							<< setw(5) << npx->GetIndex()
							<< setw(15) << ple->GetAddress();
					if (nhx) {
						TString hName = nhx->GetName();
						hName += " (";
						hName += nhx->GetTitle();
						hName += ")";
						Out	<< "     "	<< setiosflags(ios::left) << setw(30) << hName
										<< resetiosflags(ios::left) << hle->GetAddress();
					} else {
						Out	<< "     "	<< setiosflags(ios::left) << setw(30) << "(no histo assigned)";
					}
				}
			}
			Out	<< endl;
			px++;
		}
		Out	<< endl;
	}
}

void TMrbAnalyze::PrintLists(const Char_t * FileName) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::PrintLists
// Purpose:        Output list contents
// Arguments:      Char_t * FileName    -- output file
// Results:        --
// Exceptions:
// Description:    Printout of module, param, histo lists, resp.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	ofstream f(FileName, ios::out);
	if (!f.good()) {
		gMrbLog->Err()	<< gSystem->GetError() << " - " << FileName << endl;
		gMrbLog->Flush(this->ClassName(), "PrintLists");
	}
	this->PrintLists(f);
	f.close();
}

Bool_t TMrbAnalyze::DumpData(const Char_t * Prefix, Int_t Index,	const Char_t * CallingClass,
																	const Char_t * CallingMethod,
																	const Char_t * Msg,
																	const UShort_t * DataPtr, Int_t DataWC) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::DumpData
// Purpose:        Dump data to file
// Arguments:      Char_t * Prefix        -- file name prefix
//                 Int_t Index            -- file index
//                 Char_t * CallingClass  -- called by this class
//                 Char_t * CallingMethod -- from this method
//                 Char_t * Msg           -- message
//                 UShort_t * DataPtr     -- pointer to data array
//                 Int_t DataWC           -- word count
// Results:        --
// Exceptions:
// Description:    Dumps data to file <Prefix>-<Index>.dmp
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbString fileName;
	TString prefix;

	if (fDumpCount == 0) return(kTRUE); 			// dump turned off
	if (fDumpCount > 0) fDumpCount--;				// count number of dumps

	if (Prefix == NULL) Prefix = "";
	prefix = Prefix;
	prefix = prefix.Strip(TString::kBoth);
	if (prefix.Length() == 0) prefix = "data";

	if (CallingClass == NULL) CallingClass = "";
	if (CallingMethod == NULL) CallingMethod = "";

	fileName = prefix;
	if (Index >= 0) {
		fileName += "-";
		fileName += Index;
	}
	fileName += ".dmp";

	if (*CallingClass != '\0' && *CallingMethod != '\0')	cout << CallingClass << "::" << CallingMethod << "(): ";
	else													cout << this->ClassName() << "::DumpData(): ";

	cout << this->ClassName() << "::Dumping " << DataWC << " word(s) to file " << fileName;
	if (Msg != NULL && *Msg != '\0') cout << " (reason: \"" << Msg << "\")";
	cout << endl;

	ofstream f;
	f.open(fileName.Data(), ios::out);
	if (!f.good()) {
		gMrbLog->Err()	<< gSystem->GetError() << " - " << fileName << endl;
		gMrbLog->Flush(this->ClassName(), "DumpData");
		return(kFALSE);
	}
	f.write((const Char_t *) DataPtr, DataWC * sizeof(UShort_t));
	f.close();
	return(kTRUE);
}

Bool_t TMrbAnalyze::AddResourcesFromFile(const Char_t * ResourceFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::AddResourcesFromFile
// Purpose:        Add user's resource defs to gEnv
// Arguments:      Char_t * ResourceFile        -- resource file
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Opens resource file,
//                       adds contents to gEnv,
//                       extracts env variable "TMrbConfig.ResourceName"
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fResourceFile.Length() != 0) {				// already done?
		gMrbLog->Err()	<< "User's resource defs already loaded from file " << fResourceFile << endl;
		gMrbLog->Flush(this->ClassName(), "AddResourcesFromFile");
		return(kFALSE);
	}
	fResourceFile.Resize(0);						// reset
	fResourceName.Resize(0);
	fResourceString.Resize(0);
	gEnv->ReadFile(ResourceFile, kEnvUser); // add resources to user's env space
	fResourceName = gEnv->GetValue("TMrbConfig.ResourceName", "");
	if (fResourceName.Length() == 0) return(kFALSE);
	fResourceFile = ResourceFile;
	return(kTRUE);
}

const Char_t * TMrbAnalyze::GetResource(const Char_t * Resource) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetResource
// Purpose:        Return completed resource string
// Arguments:      Char_t * Resource         -- resource string
// Results:        Char_t * Resource         -- (completed) string
// Exceptions:
// Description:    Prepends user's resource name if present.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fResourceName.Length() > 0) {
		fResourceString = fResourceName;
		fResourceString += ".";
		fResourceString += Resource;
		return(fResourceString.Data());
	} else {
		return(Resource);
	}
}

Bool_t TMrbAnalyze::SetTimeOffset(Int_t ModuleIndex, Int_t Offset) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::SetTimeOffset
// Purpose:        Insert time offset in table
// Arguments:      Int_t ModuleIndex       -- module number
//                 Int_t Offset            -- time offset
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines a time offset. Time offsets are needed to
//                 build events from hit buffer data.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (ModuleIndex <= 0 || ModuleIndex > fModuleList.GetLast()) {
		gMrbLog->Err()	<< "Index out of range - " << ModuleIndex
						<< " (should be in [1," << fModuleList.GetLast() << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "SetTimeOffset");
		return(kFALSE);
	}
	TMrbNamedX * nx = (TMrbNamedX *) fModuleList[ModuleIndex];
	if (nx == NULL) {
		gMrbLog->Err()	<< "No module with index = " << ModuleIndex << endl;
		gMrbLog->Flush(this->ClassName(), "SetTimeOffset");
		return(kFALSE);
	}
	((TMrbModuleListEntry *) nx->GetAssignedObject())->SetTimeOffset(Offset);
	return(kTRUE);
}

Int_t TMrbAnalyze::GetTimeOffset(Int_t ModuleIndex) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetTimeOffset
// Purpose:        Get time offset from table
// Arguments:      Int_t ModuleIndex       -- module number
// Results:        Int_t Offset            -- time offset
// Exceptions:
// Description:    Returns the time offset for given module.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (ModuleIndex <= 0 || ModuleIndex > fModuleList.GetLast()) {
		gMrbLog->Err()	<< "Index out of range - " << ModuleIndex
						<< " (should be in [1," << fModuleList.GetLast() << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "GetTimeOffset");
		return(0);
	}
	TMrbNamedX * nx = (TMrbNamedX *) fModuleList[ModuleIndex];
	if (nx == NULL) {
		gMrbLog->Err()	<< "No module with index = " << ModuleIndex << endl;
		gMrbLog->Flush(this->ClassName(), "GetTimeOffset");
		return(0);
	}
	return(((TMrbModuleListEntry *) nx->GetAssignedObject())->GetTimeOffset());
}

void TMrbAnalyze::WaitForLock(const Char_t * Lockfile, const Char_t * Msg) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::WaitForLock
// Purpose:        Wait for lock file to disappear
// Arguments:      Char_t * Lockfile       -- lock file
//                 Char_t * Msg            -- message
// Results:        --
// Exceptions:
// Description:    Loops on file 'Lockfile'. Tests every second if it has disappeared.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t first = kTRUE;
	while (!gSystem->AccessPathName(Lockfile)) {
		if (first) {
			cout << "Break: ";
			if (Msg != NULL && *Msg != '\0') cout << Msg << "... ";
			cout << "Remove file \"" << Lockfile << "\" to continue ..." << endl;
		}
		first = kFALSE;
		sleep(1);
	}
	cout << "[Continuing]" << endl;
	gSystem->Exec(Form("touch %s", Lockfile));
}

void TMrbAnalyze::ResetEventsPerTrigger() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::ResetEventsPerTrigger
// Purpose:        Clear events per trigger
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Loops thu list of modules and clears event count
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TIterator * iter = fModuleList.MakeIterator();
	TMrbNamedX * nx;
	while (nx = (TMrbNamedX *) iter->Next()) {
		TMrbModuleListEntry * mle = (TMrbModuleListEntry *) nx->GetAssignedObject();
		mle->ResetEventsPerTrigger();
	}
}
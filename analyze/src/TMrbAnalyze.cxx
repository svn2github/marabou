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
// Revision:       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include <sstream>

#include <time.h>
#include <pthread.h>

#include "TF1.h"
#include "TArrayI.h"
#include "TEnv.h"
#include "TRegexp.h"
#include "TOrdCollection.h"

#include "TMrbAnalyze.h"
#include "mbsio_protos.h"

#include "SetColor.h"

#include "n2ll.h"

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

ClassImp(TMrbAnalyze)							// implementation of user-specific classes
ClassImp(TMrbIOSpec)
ClassImp(TUsrEvent)
ClassImp(TUsrEvtStart)
ClassImp(TUsrEvtStop)
ClassImp(TUsrDeadTime)
ClassImp(TUsrHit)
ClassImp(TUsrHitBuffer)
ClassImp(TUsrHBX)
ClassImp(TMrbModuleListEntry)
ClassImp(TMrbParamListEntry)
ClassImp(TMrbHistoListEntry)

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
		TTree::SetBranchStyle(0);
		fRunStatus = TMrbAnalyze::M_ABSENT;
		fUpdateFlag = 0;
		fEventsProcessed = 0;
		fTimeOfLastUpdate = 0;
		fEventsProcPrev = 0;
		fReplayMode = kFALSE;

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
//                      input  start  stop  param  histo  output
//
//                 where       value (*=dflt)   meaning       
//                 ---------------------------------------------------------------------------------
//                 input       n/a              name of input file (.root)
//                 start                        event / time stamp to start with
//                             0*                  read from beginning
//                 stop                         event / time stamp to end with
//                             0*                  read till end of data
//                                                 (time stamps have formats :nnnnnn or hh:mm:ss[:xxx])
//                 param                        name of param file:
//                             <file>              reload params from file <file>
//                                                     (.root -> standard ROOT,
//                                                      .par  -> ascii)
//                             -*                  don't change params
//                             none                no params at all
//                 histo                        name of file to save histograms:
//                             +*                  don't save, add to current mmap file
//                             <file>              save contents of mmap file to <file> (ext .root)
//                 output                       name of file to store tree data
//                             none                don't write tree data
//                             +*                  append to current file
//                             <file>              open <file> for output (ext .root)          
//                                                         
//                 If trailing args are omitted default values will be substituted
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbIOSpec * ioSpec;
	TMrbIOSpec * lastIOSpec;

	ifstream fileList;
	TString line;

	TString inputFile;
	TString startEvent;
	Int_t startValue;
	Bool_t startTimeFlag;
	TString stopEvent;
	Int_t stopValue;
	Bool_t stopTimeFlag;

	TString paramFile;
	TString histoFile;
	TString outputFile;
	TString dummy;

	TMrbIOSpec::EMrbInputMode inputMode;
	TMrbIOSpec::EMrbParamMode paramMode, lastpMode;
	TMrbIOSpec::EMrbHistoMode histoMode, lasthMode;
	TMrbIOSpec::EMrbOutputMode outputMode, lastoMode;

	Bool_t errCnt, lerrCnt;
	Int_t lineCnt;
	Int_t nofEntries;
	Bool_t lineHdr;

	fileList.open(FileList.Data(), ios::in);
	if (!fileList.good()) {
		gMrbLog->Err()	<< gSystem->GetError() << " - " << FileList << endl;
		gMrbLog->Flush(this->ClassName(), "OpenFileList");
		return(kFALSE);
	}

	errCnt = 0;
	lerrCnt = 0;
	lineCnt = 0;
	nofEntries = 0;
	inputMode = DefaultIOSpec->GetInputMode();
//	fLofIOSpecs.Delete();

	lastIOSpec = DefaultIOSpec;
   lastIOSpec->Print();
	for (;;) {
		line.ReadLine(fileList, kFALSE);
		if (fileList.eof()) break;
		lineCnt++;
		lineHdr = kFALSE;
		line = line.Strip(TString::kBoth);
		if (line.Length() == 0 || line(0) == '#') continue;
		istringstream decode(line.Data());

// decode line
// input file
		decode >> inputFile;	
		gSystem->ExpandPathName(inputFile);
		if (inputFile.Index(".root", 0) == -1) {
			if (!lineHdr) {
				gMrbLog->Err()	<< "In file " << FileList << ", line " << lineCnt << ":" << endl;
				gMrbLog->Flush(this->ClassName(), "OpenFileList");
				lineHdr = kTRUE;
			}
			gMrbLog->Err()	<< "Input Illegal extension - " << inputFile << " (should be .root)" << endl;
			gMrbLog->Flush(this->ClassName(), "OpenFileList");
			errCnt++;
		} else if (gSystem->AccessPathName(inputFile.Data(), kFileExists)) {
			if (!lineHdr) {
				gMrbLog->Err()	<< "In file " << FileList << ", line " << lineCnt << ":" << endl;
				gMrbLog->Flush(this->ClassName(), "OpenFileList");
				lineHdr = kTRUE;
			}
			gMrbLog->Err()	<< "No such file - " << inputFile << endl;
			errCnt++;
		}
// start / stop
		decode >> startEvent;
		if (!decode.good() || !DefaultIOSpec->CheckStartStop(startEvent, startValue, startTimeFlag)) {
			if (!lineHdr) {
				gMrbLog->Err()	<< "In file " << FileList << ", line " << lineCnt << ":" << endl;
				gMrbLog->Flush(this->ClassName(), "OpenFileList");
				lineHdr = kTRUE;
			}
			gMrbLog->Err()	<< setred << "Not a legal start event - " << startEvent << endl;
			gMrbLog->Flush(this->ClassName(), "OpenFileList");
			errCnt++;
		}

		decode >> stopEvent;
		if (!decode.good() || !DefaultIOSpec->CheckStartStop(stopEvent, stopValue, stopTimeFlag)) {
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
// parm file
		decode >> paramFile;
		lastpMode = lastIOSpec->GetParamMode();
		paramMode = TMrbIOSpec::kParamNone;
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
		decode >> histoFile;
		histoMode = TMrbIOSpec::kHistoNone;
		lasthMode = lastIOSpec->GetHistoMode();
		if (histoFile.CompareTo("none") != 0) {
			if (histoFile.CompareTo("+") == 0) {
				histoFile = lastIOSpec->GetHistoFile();
				histoMode = TMrbIOSpec::kHistoAdd;
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
		decode >> outputFile;
		outputMode = TMrbIOSpec::kOutputNone;
		lastoMode = lastIOSpec->GetOutputMode();
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
// check if end of argument list
		decode >> dummy;
		if (decode.good()) {
			if (dummy(0) != '#') {
				if (!lineHdr) {
					gMrbLog->Err()	<< "In file " << FileList << ", line " << lineCnt << ":" << endl;
					gMrbLog->Flush(this->ClassName(), "OpenFileList");
					lineHdr = kTRUE;
				}
				gMrbLog->Err()	<< "Exceeding argument at end of line - " << dummy << endl;
				gMrbLog->Flush(this->ClassName(), "OpenFileList");
				errCnt++;
			}
		}
// fill i/o spec (if w/o errors)
		if (errCnt == lerrCnt) {
			nofEntries++;
			ioSpec = new TMrbIOSpec();
			ioSpec->SetInputFile(inputFile.Data(), inputMode);
			ioSpec->SetStartStop(startTimeFlag, startValue, stopValue);
			ioSpec->SetOutputFile(outputFile.Data(), outputMode);
			ioSpec->SetParamFile(paramFile.Data(), paramMode);
			ioSpec->SetHistoFile(histoFile.Data(), histoMode);
			fLofIOSpecs.Add(ioSpec);
			lastIOSpec = ioSpec;
		}
		lerrCnt = errCnt;
	}
// check if errors
	if (errCnt > 0) {
		gMrbLog->Err()	<< "Found " << errCnt << " error(s) in file list " << FileList << endl;
		gMrbLog->Flush(this->ClassName(), "OpenFileList");
		return(0);
	}
    cout << "fLofIOSpecs.GetSize() " << fLofIOSpecs.GetSize()<< endl;
    cout << "nofEntries            " << nofEntries           << endl;
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

	ioSpec = (TMrbIOSpec *) fLofIOSpecs.First();
	nofEntries = 0;
	while (ioSpec) {
		if (this->IsVerbose()) {
			cout	<< setblue << this->ClassName() << "::ProcessFileList(): " << endl;
			cout << "        "; ioSpec->Print(cout);
			cout	<< setblack << endl;
		}
		if (this->OpenRootFile(ioSpec)) {
//			this->ExecFilterMacro();					*** no longer in use RL 12-2000 ***
			this->ReloadParams(ioSpec);
			if (this->WriteRootTree(ioSpec)) {
				this->ReplayEvents(ioSpec);
				this->CloseRootTree(ioSpec);
				if (this->SaveHistograms("*", ioSpec) != -1) {
					this->ClearHistograms("*", ioSpec);
				}
				nofEntries++;
			}
            if (!fFakeMode) {
				fRootFileIn->Close();
				fRootFileIn=NULL;
			}
		}
		ioSpec = (TMrbIOSpec *) fLofIOSpecs.After(ioSpec);
	}
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
		cout << this->ClassName() << "::TestRunStatus(): Termination flag detected" << endl;
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
		if (elapsed != 0) {
			nofBins = hRateHistory->GetNbinsX();
			rate = (fEventsProcessed - fEventsProcPrev) / elapsed;
			for (Int_t i = 1; i < nofBins; i++) {
				hRateHistory->SetBinContent(i, hRateHistory->GetBinContent(i+1));
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
/*
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
*/
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
/*
							gMrbLofUserVars->Replace(vobj, fobj);
							gROOT->GetList()->Remove(vobj);
							if (vobj) delete vobj;
							vobj = fobj;
                            if(fMapFile){
                                TMrbWindow2D* w2dold = (TMrbWindow2D*)fMapFile->Get(fobj->GetName());
                                if(w2dold)w2dold = (TMrbWindow2D*)fMapFile->Remove(fobj->GetName());
                                fMapFile->Add(fobj);
                                fMapFile->Update(fobj);
					        }
*/
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

Int_t TMrbAnalyze::GetSizeOfMappedObjects(TMapFile * MapFile) {
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

	TString histoFile;
	TString pattern;
	TRegexp * rexp;
	Int_t count;

	if (IOSpec && (IOSpec->GetHistoMode() & TMrbIOSpec::kHistoSave) == 0) return(0);

	pattern = Pattern;
	pattern = pattern.Strip(TString::kBoth);
	count = 0;
	rexp = NULL;

	if (pattern.CompareTo("*") == 0) rexp = new TRegexp(pattern.Data(), kTRUE);

	histoFile = IOSpec->GetHistoFile();
	TFile * hf = new TFile(histoFile.Data(), "RECREATE");
	if (!hf->IsOpen()) {
		gMrbLog->Err()	<< "Can't open histo file " << histoFile << endl;
		gMrbLog->Flush(this->ClassName(), "SaveHistograms");
		return(-1);
	}
	hf->cd();

	pthread_mutex_lock(&global_data_mutex);
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
					if (obj) {
						obj->Write();
						count++;
					}
					if (this->IsVerbose()) cout << "   Writing: " << name << endl;
        		}
		   	 mr = mr->GetNext();         
			}
		}
   } else {
      TIter next(gROOT->GetList());
      TObject * obj;
      while( (obj = (TObject*)next()) ){
         if(obj->InheritsFrom("TH1")){
            TString shh(obj->GetName());
    		   if(shh.Index(*rexp) >= 0){
               obj->Write();
				   count++;
				  	if (this->IsVerbose()) cout << "   Writing: " << obj->GetName() << endl;
            }
         }
      }
   }
	cout	<< setblue
			<< this->ClassName() << "::SaveHistograms(): " << count << " histogram(s) saved to " << histoFile
			<< setblack << endl;
	hf->Close();
	pthread_mutex_unlock(&global_data_mutex);
	return(count);
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
	cout	<< setblue
			<< this->ClassName() << "::ClearHistograms(): " << count << " histogram(s) zeroed"
			<< setblack << endl;
	if (rexp) delete rexp;
	return(count);
}

const UShort_t * TUsrEvent::NextSubevent(const MBSDataIO * BaseAddr, Int_t NofWords, Bool_t RawMode) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvent::NextSubevent
// Purpose:        Get next subevent
// Arguments:      const MBSDataIO * BaseAddr -- pointer to base of MBS data
//                 Int_t NofWords             -- expected number of data words
//                 Bool_t RawMode             -- pass data as is, don't decode
//                                               subevent structure
// Results:        const UShort_t * SevtData  -- pointer to current (unpacked) data
// Exceptions:
// Description:    Unpacks next subevent from current event.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t sevtType;

	mbs_set_sevt_minwc(BaseAddr, NofWords);		// min number of data words

	sevtType = RawMode ? mbs_next_sevent_raw(BaseAddr) : mbs_next_sevent(BaseAddr);
	if (sevtType == MBS_STYPE_EOE) {			// end of event
		return(NULL);
	} else if (sevtType == MBS_STYPE_ERROR) {	// error
		return(NULL);
	} else {									// legal data
		return((const UShort_t *) BaseAddr->sevt_data);
	}
}

UInt_t TUsrEvent::NextSubeventHeader(const MBSDataIO * BaseAddr) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvent::NextSubeventHeader
// Purpose:        Get next subevent header
// Arguments:      const MBSDataIO * BaseAddr -- pointer to base of MBS data
// Results:        UInt_t SevtType            -- subevent type
// Exceptions:
// Description:    Unpacks next subevent header from current event.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t sevtType;
	sevtType = mbs_next_sheader(BaseAddr);
	if (sevtType == MBS_STYPE_EOE || sevtType == MBS_STYPE_ABORT)	return(sevtType);
	else															return(BaseAddr->sevt_otype);
}

const UShort_t * TUsrEvent::NextSubeventData(const MBSDataIO * BaseAddr, Int_t NofWords, Bool_t RawMode) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvent::NextSubeventData
// Purpose:        Get next subevent
// Arguments:      const MBSDataIO * BaseAddr -- pointer to base of MBS data
//                 Int_t NofWords             -- expected number of data words
//                 Bool_t RawMode             -- pass data as is, don't decode
//                                               subevent structure
// Results:        const UShort_t * SevtData  -- pointer to current (unpacked) data
// Exceptions:
// Description:    Unpacks next subevent from current event.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t sevtType;

	mbs_set_sevt_minwc(BaseAddr, NofWords);		// min number of data words

	sevtType = RawMode ? mbs_next_sdata_raw(BaseAddr) : mbs_next_sdata(BaseAddr);
	if (sevtType == MBS_STYPE_ERROR) {			// error
		return(NULL);
	} else {									// legal data
		return((const UShort_t *) BaseAddr->sevt_data);
	}
}

Int_t TUsrEvent::CalcTimeDiff(TUsrEvent * BaseEvent) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvent::CalcTimeDiff
// Purpose:        Calculate time with respect to event
// Arguments:      TUsrEvent * BaseEvent      -- event used as time base
// Results:        Int_t TimeDiff             -- time diff in microsecs
// Exceptions:
// Description:    Calculates time difference from internal secs/nsecs values.
//                 Result is given in units of 100 micro seconds so it may be
//                 used as time stamp via TObject::SetUniqueID().
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t secsB, msecsB, msecs;
	Int_t timeDiff;

	secsB = BaseEvent->GetClockSecs();						// time to refer to (secs since 1-Jan-70)
	msecsB = BaseEvent->GetClockNsecs() / TMrbAnalyze::kTimeNsecsPerUnit;	// ... fraction of last second in nanosecs

	msecs = fClockNsecs / TMrbAnalyze::kTimeNsecsPerUnit; 				// nanosecs in current event

	if (msecs >= msecsB) {
		timeDiff = (fClockSecs - secsB) * TMrbAnalyze::kTimeUnitsPerSec + (msecs - msecsB);
	} else {
		timeDiff = (fClockSecs - secsB - 1) * TMrbAnalyze::kTimeUnitsPerSec + (TMrbAnalyze::kTimeUnitsPerSec - msecsB + msecs);
	}
	return(timeDiff);
}

Int_t TUsrEvent::CalcTimeRS() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvent::CalcTimeRS
// Purpose:        Calculate time in ROOT format
// Arguments:      
// Results:        Int_t TimeRS           -- time in ROOT format
// Exceptions:
// Description:    Calculates time from internal values and converts it to
//                 ROOT format. Result is stored in fTimeRS.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	struct tm * et;
	Int_t timeRS = 0;
	TDatime * trs;
	
	et = localtime((const long *) &fClockSecs);				// convert to time struct
	trs = new TDatime();						// date/time in ROOT style

	trs->Set(	et->tm_year + 1900,				// set time components
				et->tm_mon + 1,
				et->tm_mday,
				et->tm_hour,
				et->tm_min,
				et->tm_sec);

	fTimeRS = trs->Get();						// get ROOT's time representation

	return(timeRS);
}

//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtStart
// Purpose:        Create an event for "Start Acquisition"
// Description:    Creates an event for trigger 14 (Start)
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

TUsrEvtStart::TUsrEvtStart() {
	fType = 10; 					// event type (given by MBS)
	fSubtype = 1;					// ... subtype
	fTrigger = 14;					// trigger number 14: start
	fNofEvents = 0; 				// reset event counter
	fEventNumber = 0;				// event number provided by MBS
}

//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtStop
// Purpose:        Create an event for "Stop Acquisition"
// Description:    Creates an event for trigger 15 (Stop)
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

TUsrEvtStop::TUsrEvtStop() {
	fType = 10; 					// event type (given by MBS)
	fSubtype = 1;					// ... subtype
	fTrigger = 15;  				// trigger number 15: stop
	fNofEvents = 0; 				// reset event counter
	fEventNumber = 0;				// event number provided by MBS
}

Bool_t TUsrEvtStart::CreateTree() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtStart::CreateTree
// Purpose:        Create a tree for event "Start Acquisition"
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Creates a tree for event "Start Acquisition"
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbAnalyze->TreeToBeWritten()) {
		if (gMrbAnalyze->IsVerbose()) {
			cout	<< this->ClassName() << "::CreateTree(): Creating tree \"Start Acquisition\""
					<< endl;
		}
		fTreeOut = new TTree("Start", "Start Acquisition");
		fTreeOut->SetAutoSave(1000);					// automatic save after 1 kB
		fTreeOut->Branch("start", &fClockSecs, "start/i");
	}
	return(kTRUE);
}

Bool_t TUsrEvtStop::CreateTree() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtStop::CreateTree
// Purpose:        Create a tree for event "Stop Acquisition"
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Creates a tree for event "Stop Acquisition"
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbAnalyze->TreeToBeWritten()) {
		if (gMrbAnalyze->IsVerbose()) {
			cout	<< this->ClassName() << "::CreateTree(): Creating tree \"Stop Acquisition\""
					<< endl;
		}
		fTreeOut = new TTree("Stop", "Stop Acquisition");
		fTreeOut->SetAutoSave(1000);					// automatic save after 1 kB
		fTreeOut->Branch("stop", &fClockSecs, "stop/i");
	}
	return(kTRUE);
}

Bool_t TUsrEvtStart::ExtractTimeStamp(const s_vehe * EventData, const MBSDataIO * BaseAddr) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtStart::ExtractTimeStamp
// Purpose:        Get time stamp from (sub)event header
// Arguments:      const s_vehe * EventData   -- pointer to MBS event structure
//                 const MBSDataIO * BaseAddr -- pointer to MBS data base
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Decodes the event header,
//                 then looks for a subevent [9000,1] = time stamp
//                 (online mode only)
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t * timeStamp;				// pointer to time info

	timeStamp = (Int_t *) NextSubevent(BaseAddr);
	if (BaseAddr->sevt_id == 999) {
		fClockRes = *timeStamp++;
		fClockSecs = *timeStamp++;
		fClockNsecs = *timeStamp;
		if (gMrbAnalyze->TreeToBeWritten()) fTreeOut->Fill();
		return(kTRUE);
	} else {
		gMrbLog->Err()	<< "Illegal subevent id - " << BaseAddr->sevt_id << " (should be 999)" << endl;
		gMrbLog->Flush(this->ClassName(), "ExtractTimeStamp");
		return(kFALSE);
	}
}

Bool_t TUsrEvtStop::ExtractTimeStamp(const s_vehe * EventData, const MBSDataIO * BaseAddr) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtStop::ExtractTimeStamp
// Purpose:        Get time stamp from (sub)event header
// Arguments:      const s_vehe * EventData   -- pointer to MBS event structure
//                 const MBSDataIO * BaseAddr -- pointer to MBS data base
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Decodes the event header,
//                 then looks for a subevent [9000,1] = time stamp
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t * timeStamp;				// pointer to time info
	TString timeString;					// expanded time string

	timeStamp = (Int_t *) NextSubevent(BaseAddr);
	if (BaseAddr->sevt_id == 999) {
		fClockRes = *timeStamp++;
		fClockSecs = *timeStamp++;
		fClockNsecs = *timeStamp;
		if (gMrbAnalyze->TreeToBeWritten()) fTreeOut->Fill();
		return(kTRUE);
	} else {
		gMrbLog->Err()	<< "Illegal subevent id - " << BaseAddr->sevt_id << " (should be 999)" << endl;
		gMrbLog->Flush(this->ClassName(), "ExtractTimeStamp");
		return(kFALSE);
	}
}

Bool_t TUsrEvtStart::InitializeTree(TFile * RootFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtStart::InitializeTree
// Purpose:        Initialize start event tree to read data
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Initializes the start event tree.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	register TObject * obj;
	TString name;

	obj = RootFile->Get("Start");
	if (obj == NULL) {
		gMrbLog->Err()	<< "Tree \"Start\" not found in ROOT file " << RootFile->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "InitializeTree");
		return(kFALSE);
	}
	name = obj->ClassName();
	if (name.CompareTo("TTree") != 0) {
		gMrbLog->Err()	<< "Not a TTree object - " << obj->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "InitializeTree");
		return(kFALSE);
	}
	fTreeIn = (TTree *) obj;

	fTreeIn->SetBranchAddress("start", &fClockSecs);		// time of start event

	return(kTRUE);
}

Bool_t TUsrEvtStop::InitializeTree(TFile * RootFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtStop::InitializeTree
// Purpose:        Initialize stop event tree to read data
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Initializes the stop event tree.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	register TObject * obj;
	TString name;

	obj = RootFile->Get("Stop");
	if (obj == NULL) {
		gMrbLog->Err()	<< "Tree \"Stop\" not found in ROOT file " << RootFile->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "InitializeTree");
		return(kFALSE);
	}
	name = obj->ClassName();
	if (name.CompareTo("TTree") != 0) {
		gMrbLog->Err()	<< "Not a TTree object - " << obj->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "InitializeTree");
		return(kFALSE);
	}
	fTreeIn = (TTree *) obj;

	fTreeIn->SetBranchAddress("stop", &fClockSecs);		// time of stop event

	return(kTRUE);
}

void TMrbAnalyze::PrintStartStop() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::PrintStartStop
// Purpose:        Output time stamps for start and stop
// Arguments:      
// Results:        
// Exceptions:     
// Description:    Outputs start/stop records read from root file
//                 (replay mode only)
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t runTime;
	TString timeString;

	timeString.Resize(100);

	if (gStartEvent->GetTreeIn() == NULL || gStopEvent->GetTreeIn() == NULL) {
		gMrbLog->Err()	<< "No start/stop tree found" << endl;
		gMrbLog->Flush(this->ClassName(), "PrintStartStop");
	} else {
		cout	<< setblue
				<< this->ClassName() << "::PrintStartStop(): Start/stop info taken from file "
				<< fRootFileIn->GetName() << ":" << endl
				<< "--------------------------------------------------------------------------------" << endl;
		gStartEvent->GetTreeIn()->GetEvent(0);
		gStartEvent->Print();
		gStopEvent->GetTreeIn()->GetEvent(0);
		gStopEvent->Print();
		runTime = gStopEvent->GetTime() - gStartEvent->GetTime();
		strftime((Char_t *) timeString.Data(), timeString.Length(), "%H:%M:%S", gmtime((const long *) &runTime));
		cout	<< setblue
				<< "Runtime             :   " << timeString << endl;
		cout	<< setblue
				<< "--------------------------------------------------------------------------------"
				<< setblack << endl;
	}
}

void TUsrEvtStart::Print() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtStart::Print
// Purpose:        Output time stamp of last "start acquisition"
// Arguments:      
// Results:        
// Exceptions:     
// Description:    Outputs a message containing time of last start cmd
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString timeString;

	timeString.Resize(100);
	strftime((Char_t *) timeString.Data(), timeString.Length(), "%e-%b-%Y %H:%M:%S", localtime((const long *) &fClockSecs));
	cout	<< setblue << this->ClassName() << "::Print(): START Acquisition at " << timeString << setblack << endl;
}

void TUsrEvtStop::Print() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtStop::Print
// Purpose:        Output time stamp of last "stop acquisition"
// Arguments:      
// Results:        
// Exceptions:     
// Description:    Outputs a message containing time of last stop cmd
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString timeString;

	timeString.Resize(100);
	strftime((Char_t *) timeString.Data(), timeString.Length(), "%e-%b-%Y %H:%M:%S", localtime((const long *) &fClockSecs));
	cout	<< setblue << this->ClassName() << "::Print(): STOP Acquisition at " << timeString << setblack << endl;
}

void TUsrDeadTime::Set(Int_t TimeStamp, Int_t TotalEvents, Int_t AccuEvents, Int_t ScalerContents) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrDeadTime::Set
// Purpose:        Calculate dead time
// Arguments:      Int_t TimeStamp        -- time stamp (since last start)
//                 Int_t TotalEvents      -- total events gathered since last start
//                 Int_t AccuEvents       -- events accumulated within dead-time interval
//                 Int_t ScalerContents   -- contents of the dead-time scaler w/i interval
// Results:        --
// Exceptions:     
// Description:    Calculates dead time in percent.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fTimeStamp = TimeStamp;
	fTotalEvents = TotalEvents;
	fAccuEvents = AccuEvents;
	fScalerContents = ScalerContents;
	if (ScalerContents > 0) fDeadTime = (1. - (Float_t) AccuEvents / (Float_t) ScalerContents) * 100;
	else					fDeadTime = 0.;
}

Bool_t TUsrDeadTime::CreateTree() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrDeadTime::CreateTree
// Purpose:        Create tree to store dead-time data
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Creates a tree to output dead-time records to root file.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbAnalyze->TreeToBeWritten()) {
		if (gMrbAnalyze->IsVerbose()) {
			cout	<< this->ClassName() << "::CreateTree(): Creating tree \"DeadTime\""
					<< endl;
		}
		fTreeOut = new TTree("DeadTime", "Dead Time");
		fTreeOut->SetAutoSave(1000);						// automatic save after 1 kB
		fBranch = (TObject *) gDeadTime; 					// we need a '**' ref to this object
		fTreeOut->Branch("dtData", this->ClassName(), &fBranch, gMrbAnalyze->GetBranchSize(), 0);
	}
	return(kTRUE);
}

Bool_t TUsrDeadTime::InitializeTree(TFile * RootFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrDeadTime::InitializeTree
// Purpose:        Initialize dead-time tree to read data
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Initializes the dead-time tree.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	register TObject * obj;
	TString name;

	obj = RootFile->Get("DeadTime");
	if (obj == NULL) {
		gMrbLog->Err()	<< "Tree \"DeadTime\" not found in ROOT file " << RootFile->GetName() << endl;
		gMrbLog->Flush();
		gMrbLog->Err()	<< "WARNING - probably NO DEAD TIME info written to this file" << endl;
		gMrbLog->Flush(this->ClassName(), "InitializeTree");
		return(kFALSE);
	}
	name = obj->ClassName();
	if (name.CompareTo("TTree") != 0) {
		gMrbLog->Err()	<< "Not a TTree object - " << obj->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "InitializeTree");
		return(kFALSE);
	}
	fTreeIn = (TTree *) obj;
	fReplayMode = kTRUE;

	fTreeIn->SetBranchAddress("fTimeStamp", &fTimeStamp);					// time stamp since last start
	fTreeIn->SetBranchAddress("fTotalEvents", &fTotalEvents); 				// event count
	fTreeIn->SetBranchAddress("fAccuEvents", &fAccuEvents);					// accumulated events
	fTreeIn->SetBranchAddress("fScalerContents", &fScalerContents);			// contents of dead-tim scaler
	fTreeIn->SetBranchAddress("fDeadTime", &fDeadTime);						// dead time (%)
	fTreeIn->SetBranchStatus("*", 0);										// disable branch

	return(kTRUE);
}

void TUsrDeadTime::Print() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrDeadTime::Print
// Purpose:        Print last dead time record
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Dead-time printout.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString timeString;
	Int_t secs;

	secs = fTimeStamp / 10000;
	timeString.Resize(100);
	strftime((Char_t *) timeString.Data(), timeString.Length(), "%H:%M:%S", gmtime((const long *) &secs));
	gMrbLog->Out()	<< "Current dead time value = "
					<< setw(5) << setprecision(2) << fDeadTime << " %"
					<< " (evt " << fTotalEvents << ", time " << timeString << " since start)" << endl;
	gMrbLog->Flush(this->ClassName(), "Print", setblue);
}

Bool_t TMrbIOSpec::SetStartStop(TString & Start, TString & Stop) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbIOSpec::SetStartStop
// Purpose:        Set start/stop values
// Arguments:      TString & Start         -- event / time stamp to start with
//                 TString & Stop          -- event / time stamp to end with
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Sets start and stop events or time stamps.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t startValue, stopValue;
	Bool_t startTstampFlag, stopTstampFlag;

	if (!this->CheckStartStop(Start, startValue, startTstampFlag)) {
		gMrbLog->Err()	<< "Not a legal start event - " << Start << endl;
		gMrbLog->Flush(this->ClassName(), "SetStartStop");
		return(kFALSE);
	}
	if (!this->CheckStartStop(Stop, stopValue, stopTstampFlag)) {
		gMrbLog->Err()	<< "Not a legal stop event - " << Stop << endl;
		gMrbLog->Flush(this->ClassName(), "SetStartStop");
		return(kFALSE);
	}
	if (startTstampFlag != stopTstampFlag) {
		gMrbLog->Err()	<< "Can't mix event count and time stamp" << endl;
		gMrbLog->Flush(this->ClassName(), "SetStartStop");
		return(kFALSE);
	}
	if (startValue > stopValue) {
		gMrbLog->Err()	<< "Illegal start/stop range - [" << Start << "," << Stop << "]" << endl;
		gMrbLog->Flush(this->ClassName(), "SetStartStop");
		return(kFALSE);
	}
	this->SetStartStop(startTstampFlag, startValue, stopValue);
	return(kTRUE);
}

Bool_t TMrbIOSpec::CheckStartStop(TString & ValAscii, Int_t & Value, Bool_t & TimeStampFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbIOSpec::CheckStartStop
// Purpose:        Check if value is a legal start/stop event
// Arguments:      TString & ValAscii      -- ascii representation
//                 Int_t & Value           -- converted value
//                 Bool_t & TimeStampFlag  -- kTRUE, if value is a time stamp
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Checks if a given string represents a legal start/stop value.
//                 Formats: ValString = "n", n >= 0     -> event count
//                          ValString = ":hh:mm:ss:xxx" -> time stamp
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	TString valAscii;
	TString hmsx;
	Int_t n1, n2;
	Int_t nt, ts[4];

	valAscii = ValAscii.Strip(TString::kBoth);

	Value = 0;

	if (valAscii.Index(":", 0) == -1) {		// event count
		TimeStampFlag = kFALSE;
		valAscii += " ";					// trailing blank keeps istringstream happy
		istringstream v(valAscii.Data());
		v >> Value;
		return(v.good());
	} else {								// time stamp
		TimeStampFlag = kTRUE;
		valAscii += ":";					// add a trailing ":" to make method Index() easier
		nt = 0;
		for (i = 0; i < 4; i++) ts[i] = 0;
		n1 = (valAscii(0) == ':') ? 1 : 0; 	// skip leading ":" if present
		for (i = 0; i < 4; i++) {			// decode :-separated items
			n2 = valAscii.Index(":", n1);
			if (n2 == -1) break;
			hmsx = valAscii(n1, n2 - n1);
			istringstream v(hmsx.Data());
			v >> ts[i];
			if (!v.good()) return(kFALSE);	// not a number
			n1 = n2 + 1;
			nt++;
		}

		switch (nt) {
			case 1: Value = ts[0];		// "raw" time stamp - ":nnnnnn"
					return(kTRUE);
			case 2: return(kFALSE); 	// illegal
			case 3: 					// full stime stamp - ":hh:mm:ss[:xxx]"
			case 4: if (ts[0] > 59 || ts[1] > 59 || ts[2] > 59 || ts[3] > 999) return(kFALSE);	// illegal time format
					Value = ((ts[0] * 60 + ts[1]) * 60 + ts[2]) * 1000 + ts[3]; 	// time in millesecs
					return(kTRUE);
		}
	}
	return(kFALSE);
}

void TMrbIOSpec::ConvertToTimeStamp(TString & TimeStamp, Int_t TstampValue) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbIOSpec::ConvertToTimeStamp
// Purpose:        Convert number to time stamp
// Arguments:      TString & TimeStamp    -- time stamp (ascii)
//                 Int_t TstampValue      -- value
// Results:        --
// Exceptions:     
// Description:    Converts a number to hh:mm:ss:xxx
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t xxx, ss, mm, hh;
	ostringstream ts;

	xxx = TstampValue % 10000;
	TstampValue /= 10000;
	ss = TstampValue % 60;
	TstampValue /= 60;
	mm = TstampValue % 60;
	hh = TstampValue / 60;
	ts << hh << ":" << mm << ":" << ss << ends;
	if (xxx > 0) ts << ":" << xxx << ends;
	TimeStamp = ts.str().c_str();
//	ts.rdbuf()->freeze(0);
}	

void TMrbIOSpec::Print(ostream & out) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbIOSpec::Print
// Purpose:        Output current settings
// Arguments:      ostream & out       -- output channel
// Results:        --
// Exceptions:     
// Description:    Outputs settings to channel out (default = cout).
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t startEvent, stopEvent;
	TString startTstamp, stopTstamp;

	out << "Input from " << fInputFile;
	startEvent = this->GetStartEvent();
	stopEvent = this->GetStopEvent();
	if (startEvent == 0 && stopEvent == 0) {
		out << " (entire file)";
	} else if (this->IsTimeStampMode()) {
		this->ConvertToTimeStamp(startTstamp, startEvent);
		this->ConvertToTimeStamp(stopTstamp, stopEvent);
		if (stopEvent == 0) out << " (tstamp " << startTstamp << " - EOF)";
		else				out << " (tstamp " << startTstamp << " - " << stopTstamp << ")";
	} else {
		if (stopEvent == 0) out << " (events " << startEvent << " - EOF)";
		else				out << " (events " << startEvent << " - " << stopEvent << ")";
	}

	if (fOutputMode == TMrbIOSpec::kOutputNone) 			out << "; output: none";
	else if ((fOutputMode & TMrbIOSpec::kOutputOpen) == 0)	out << "; output appended to " << fOutputFile;
	else													out << "; output to " << fOutputFile;

	if (fParamMode == TMrbIOSpec::kParamNone)				out << "; params: none";
	else if (fParamMode == TMrbIOSpec::kParamReloadAscii)	out << "; params reloaded from " << fParamFile << " (ASCII)";
	else if (fParamMode & TMrbIOSpec::kParamReload) 		out << "; params reloaded from " << fParamFile;
	else													out << "; params unchanged";

	if (fHistoMode & TMrbIOSpec::kHistoSave)				out << "; histos saved to file " << fHistoFile;
	else if (fHistoMode & TMrbIOSpec::kHistoAdd) 			out << "; histos added to mmap file";
	if (fHistoMode & TMrbIOSpec::kHistoClear)				out << "; mmap data cleared";
	out << ";" << endl;
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
}

const Char_t * TMrbAnalyze::GetModuleName(Int_t ModuleIndex) {
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

const Char_t * TMrbAnalyze::GetModuleTitle(Int_t ModuleIndex) {
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

Int_t TMrbAnalyze::GetModuleIndex(const Char_t * ModuleName) {
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

const Char_t * TMrbAnalyze::GetParamName(Int_t ModuleIndex, Int_t RelParamIndex) {
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

const Char_t * TMrbAnalyze::GetParamName(Int_t AbsParamIndex) {
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

Int_t TMrbAnalyze::GetParamIndex(const Char_t * ParamName, Bool_t AbsFlag) {
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

Int_t TMrbAnalyze::GetParamIndex(Int_t ModuleIndex, Int_t RelParamIndex) {
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

TMrbHistoListEntry * TMrbAnalyze::GetHistoListEntry(const Char_t * HistoName) {
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

TH1 * TMrbAnalyze::GetHistoAddr(const Char_t * HistoName) {
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

TH1 * TMrbAnalyze::GetHistoAddr(Int_t AbsParamIndex) {
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

TH1 * TMrbAnalyze::GetHistoAddr(Int_t ModuleIndex, Int_t RelParamIndex) {
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

TH1 * TMrbAnalyze::GetHistoFromList(TObjArray & HistoList, Int_t ModuleIndex, Int_t RelParamIndex) {
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

TH1 * TMrbAnalyze::GetHistoFromList(TObjArray & HistoList, Int_t AbsParamIndex) {
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

TF1 * TMrbAnalyze::GetCalibration(const Char_t * CalibrationName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetCalibration
// Purpose:        Get Calibration by its name
// Arguments:      Char_t * CalibrationName     -- calibration name
// Results:        TH1 * HistoAddr              -- address
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

TF1 * TMrbAnalyze::GetCalibration(Int_t AbsParamIndex) {
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

	if (AbsParamIndex < 0 || AbsParamIndex > fParamList.GetLast()) return(NULL);
	nx = (TMrbNamedX *) fCalibrationList[AbsParamIndex];
	if (nx == NULL) return(NULL);
	return(((TMrbCalibrationListEntry *) nx->GetAssignedObject())->GetAddress());
}

TF1 * TMrbAnalyze::GetCalibration(Int_t ModuleIndex, Int_t RelParamIndex, Double_t & Gain, Double_t & Offset) {
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
		Gain = cal->GetParameter(1);
		Offset = cal->GetParameter(0);
	} else {
		Gain = 1.;
		Offset = 0.;
	}
	return(cal);
}

TF1 * TMrbAnalyze::GetCalibration(Int_t AbsParamIndex, Double_t & Gain, Double_t & Offset) {
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
		Gain = cal->GetParameter(1);
		Offset = cal->GetParameter(0);
	} else {
		Gain = 1.;
		Offset = 0.;
	}
	return(cal);
}

TF1 * TMrbAnalyze::GetCalibration(Int_t ModuleIndex, Int_t RelParamIndex) {
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

	if (gSystem->AccessPathName(CalibrationFile, (EAccessMode) F_OK)) {
		gMrbLog->Err()	<< "Can't open calibration file - " << CalibrationFile << endl;
		gMrbLog->Flush(this->ClassName(), "ReadCalibrationFromFile");
		return(-1);
	}

	TEnv * cal = new TEnv(CalibrationFile);
	Int_t nofCalibs = 0;
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
				resName = "Calib.";
				resName += histoName;
				resName += ".Gain";
				Double_t gain = cal->GetValue(resName.Data(), 1.);
				resName = "Calib.";
				resName += histoName;
				resName += ".Offset";
				Double_t offset = cal->GetValue(resName.Data(), 0.);
				TString calName = histoName;
				if (calName(0) == 'h') {
					calName(0) = 'c';
				} else {
					calName(0,1).ToUpper();
					calName.Prepend("h");
				}
				TF1 * calFct = new TF1(calName.Data(), "[0] + [1] * x", xmin, xmax);
				calFct->SetParameter(0, offset);
				calFct->SetParameter(1, gain);
				this->AddCalibrationToList(calFct, hle->GetParam()->GetIndex());
				nofCalibs++;
			}
		}
		o = cal->GetTable()->After(o);
	}
	return(nofCalibs);
}

Int_t * TMrbAnalyze::GetParamAddr(const Char_t * ParamName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetParamAddr
// Purpose:        Get param addr by name
// Arguments:      Char_t * ParamName     -- param name
// Results:        Int_t * ParamAddr      -- address
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

Int_t * TMrbAnalyze::GetParamAddr(Int_t ModuleIndex, Int_t RelParamIndex) {
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

Int_t * TMrbAnalyze::GetParamAddr(Int_t AbsParamIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::GetParamAddr
// Purpose:        Get param address
// Arguments:      Int_t AbsParamIndex    -- absolute param index
// Results:        Int_t * ParamAddr      -- param address
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

Bool_t TMrbAnalyze::AddParamToList(const Char_t * ParamName, Int_t * ParamAddr, Int_t ModuleIndex, Int_t RelParamIndex) {
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
	if (npx != NULL) {
		return(kFALSE);
	}
	npx = new TMrbNamedX(px + RelParamIndex, ParamName);
	ple = new TMrbParamListEntry(nmx, ParamAddr, NULL);
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
		gMrbLog->Err()	<< "::AddHistoToList(): [" << HistoAddr->GetName()
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
	if (nhx != NULL) return(kFALSE);
	npx = (TMrbNamedX *) fParamList[px + RelParamIndex];
	if (npx == NULL) {
		gMrbLog->Err()	<< "[" << HistoAddr->GetName() << "] No param assigned to index " << RelParamIndex
						<< " (abs " << px + RelParamIndex << ")" << endl;
		gMrbLog->Flush(this->ClassName(), "AddHistoToList");
		return(kFALSE);
	}
	ple = (TMrbParamListEntry *) npx->GetAssignedObject();
	nhx = new TMrbNamedX(px, HistoAddr->GetName(), HistoAddr->GetTitle());
	hle = new TMrbHistoListEntry(nmx, npx, HistoAddr);
	nhx->AssignObject(hle);
	fHistoList.AddAt(nhx, px + RelParamIndex);
	ple->SetHistoAddress(HistoAddr);
	if (RelParamIndex == 0) mle->SetFirstHisto(nhx);
	return(kTRUE);
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
	TMrbNamedX * nhx;
	TMrbModuleListEntry * mle;
	TMrbParamListEntry * ple;
	TMrbCalibrationListEntry * cle;
	Int_t px;

	if (ModuleIndex <= 0 || ModuleIndex > fModuleList.GetLast()) {
		gMrbLog->Err()	<< "::AddCalibrationToList(): [" << CalibrationAddr->GetName()
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
	nhx = (TMrbNamedX *) fCalibrationList[px + RelParamIndex];
	if (nhx != NULL) return(kFALSE);
	npx = (TMrbNamedX *) fParamList[px + RelParamIndex];
	if (npx == NULL) {
		gMrbLog->Err()	<< "[" << CalibrationAddr->GetName() << "] No param assigned to index " << RelParamIndex
						<< " (abs " << px + RelParamIndex << ")" << endl;
		gMrbLog->Flush(this->ClassName(), "AddCalibrationToList");
		return(kFALSE);
	}
	ple = (TMrbParamListEntry *) npx->GetAssignedObject();
	nhx = new TMrbNamedX(px, CalibrationAddr->GetName(), CalibrationAddr->GetTitle());
	cle = new TMrbCalibrationListEntry(nmx, npx, CalibrationAddr);
	nhx->AssignObject(cle);
	fCalibrationList.AddAt(nhx, px + RelParamIndex);
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

	TMrbNamedX * nmx;
	TMrbNamedX * npx;
	TMrbNamedX * nhx;
	TMrbModuleListEntry * mle;
	TMrbParamListEntry * ple;
	TMrbCalibrationListEntry * cle;
	Int_t px;

	px = mle->GetIndexOfFirstParam();
	nhx = (TMrbNamedX *) fCalibrationList[AbsParamIndex];
	if (nhx != NULL) return(kFALSE);
	npx = (TMrbNamedX *) fParamList[AbsParamIndex];
	if (npx == NULL) {
		gMrbLog->Err()	<< "[" << CalibrationAddr->GetName() << "] No param assigned to index "
						<< AbsParamIndex << " (abs)" << endl;
		gMrbLog->Flush(this->ClassName(), "AddCalibrationToList");
		return(kFALSE);
	}
	ple = (TMrbParamListEntry *) npx->GetAssignedObject();
	nhx = new TMrbNamedX(px, CalibrationAddr->GetName(), CalibrationAddr->GetTitle());
	cle = new TMrbCalibrationListEntry(nmx, npx, CalibrationAddr);
	nhx->AssignObject(cle);
	fCalibrationList.AddAt(nhx, AbsParamIndex);
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

void TMrbAnalyze::PrintLists(ostream & Out) {
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

	Int_t i, j;
	Int_t px, np;
	Bool_t h;
	TMrbNamedX * nmx;
	TMrbNamedX * npx;
	TMrbNamedX * nhx;
	TMrbModuleListEntry * mle;
	TMrbParamListEntry * ple;
	TMrbHistoListEntry * hle;
	TString hName;
	TMrbString addr;

	Out	<< endl;
	for (i = 0; i <= fModuleList.GetLast(); i++) {
		nmx = (TMrbNamedX *) fModuleList[i];
		if (nmx) {
			Out	<< "Module " << nmx->GetName() << ", Index = "
					<< nmx->GetIndex() << " (" << nmx->GetTitle() << ")" << endl;
			mle = (TMrbModuleListEntry *) nmx->GetAssignedObject();
			np = mle->GetNofParams();
			px = mle->GetIndexOfFirstParam();
			h = kFALSE;
			for (j = 0; j < np; j++) {
				npx = (TMrbNamedX *) fParamList[px];
				if (npx) {
					if (!h) {
						Out	<< "Param     RelX AbsX           Addr     Histogram                     Addr" << endl
								<< "-----------------------------------------------------------------------------------" << endl;
						h = kTRUE;
					}
					ple = (TMrbParamListEntry *) npx->GetAssignedObject();
					nhx = (TMrbNamedX *) fHistoList[px];
					hle = (TMrbHistoListEntry *) nhx->GetAssignedObject();
					addr.FromInteger((Int_t) ple->GetAddress(), 0, 0, 16);
					Out	<< setiosflags(ios::left) << setw(10) << npx->GetName()
							<< resetiosflags(ios::left)	<< setw(4) << j
							<< setw(5) << npx->GetIndex()
							<< setw(15) << addr;
					if (nhx && hle) {
						hName = nhx->GetName();
						hName += " (";
						hName += nhx->GetTitle();
						hName += ")";
						addr.FromInteger((Int_t) hle->GetAddress(), 0, 0, 16);
						Out	<< "     " << setiosflags(ios::left) << setw(30) << hName
								<< resetiosflags(ios::left) << addr;
					}
				}
				Out	<< endl;
				px++;
			}
		}
		Out	<< endl;
	}
}

void TMrbAnalyze::PrintLists(const Char_t * FileName) {
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

Int_t TMrbAnalyze::GetTimeOffset(Int_t ModuleIndex) {
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

TUsrHit::TUsrHit(Int_t BufferNumber, Int_t EventNumber, Int_t ModuleNumber, Int_t Channel,
										UShort_t BufferTimeHi, UShort_t EventTimeHi, UShort_t FastTrigTime,
										UShort_t * Data, Int_t NofData) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHit
// Purpose:        Store data of a single hit
// Arguments:      Int_t BufferNumber     -- buffer number
//                 Int_t EventNumber      -- event index within buffer
//                 Int_t ModuleNumber     -- module serial
//                 Int_t Channel          -- channel number
//                 UShort_t BufferTimeHi  -- event time, hi order
//                 UShort_t EventTimeHi   -- ..., middle
//                 UShort_t FastTrigTime  -- ..., low
//                 Int_t NofData          -- number of data words
//                 UShort_t * Data        -- hit data, length depending on format
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	this->SetBufferNumber(BufferNumber);
	this->SetEventNumber(EventNumber);
	this->SetModuleNumber(ModuleNumber);
	this->SetChannel(Channel);
	this->SetChannelTime(BufferTimeHi, EventTimeHi, FastTrigTime);
	this->ClearData();
	if (Data) this->CopyData(Data, NofData);
}

TUsrHit::TUsrHit(Int_t BufferNumber, Int_t EventNumber, Int_t ModuleNumber, Int_t Channel,
										UShort_t * ChannelTime,
										UShort_t * Data, Int_t NofData) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHit
// Purpose:        Store data of a single hit
// Arguments:      Int_t BufferNumber     -- buffer number
//                 Int_t EventNumber      -- event index within buffer
//                 Int_t ModuleNumber     -- module serial
//                 Int_t Channel          -- channel number
//                 UShort_t * ChannelTime -- channel time (48 bit)
//                 Int_t NofData          -- number of data words
//                 UShort_t * Data        -- hit data, length depending on format
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (ChannelTime) {
		this->SetChannelTime(*ChannelTime, *(ChannelTime + 1), *(ChannelTime + 2));
	} else {
		this->SetChannelTime(0, 0, 0);
	}
	this->SetBufferNumber(BufferNumber);
	this->SetEventNumber(EventNumber);
	this->SetModuleNumber(ModuleNumber);
	this->SetChannel(Channel);
	this->ClearData();
	if (Data) this->CopyData(Data, NofData);
}

Int_t TUsrHit::Compare(const TObject * Hit) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHit::Compare
// Purpose:        Compare time stamps
// Arguments:      TObject * hit    -- hit to be compared
// Results:        -1 / 0 / 1
// Exceptions:
// Description:    Compares to hits. Used by Sort() method.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	long long t1, t2;
	t1 = ushort2ll48(((TUsrHit *) this)->GetChannelTime());
	t2 = ushort2ll48(((TUsrHit *) Hit)->GetChannelTime());
	if (t1 == t2) {
		return(0);
	} else {
		return(t1 > t2 ? 1 : -1);
	}
}

Int_t TUsrHit::Compare(UShort_t * ChannelTime) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHit::Compare
// Purpose:        Compare time stamps
// Arguments:      UShort_t * ChannelTime   -- channel time
// Results:        -1 / 0 / 1
// Exceptions:
// Description:    Compares event time with given value.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	long long t1, t2;
	t1 = ushort2ll48(((TUsrHit *) this)->GetChannelTime());
	t2 = ushort2ll48(ChannelTime);
	if (t1 == t2) {
		return(0);
	} else {
		return(t1 > t2 ? 1 : -1);
	}
}

void TUsrHit::Print(ostream & Out, Bool_t PrintNames) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHit::Print
// Purpose:        Print data
// Arguments:      ostream & Out      -- output stream
//                 Bool_t PrintNames  -- kTRUE -> print module & param names
// Results:        --
// Exceptions:
// Description:    Prints one line of data.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	const Char_t * sp;
	TMrbString moduleName, paramName;
	
	if (PrintNames) {
		sp = gMrbAnalyze->GetModuleName(fModuleNumber);
		if (sp) {
			moduleName = sp;
			moduleName += "(";
			moduleName += fModuleNumber;
			moduleName += ")";
			sp = gMrbAnalyze->GetParamName(fModuleNumber, fChannel);
			if (sp) {
				paramName = sp;
				paramName += "(";
				paramName += fChannel;
				paramName += ")";
			} else {
				paramName = fChannel;
			}
		} else {
			moduleName = fModuleNumber;
			paramName = fChannel;
		}
		Out << Form("      %13d%13s%13d%13s%18lld%13d\n",
						fBufferNumber,
						moduleName.Data(),
						fEventNumber,
						paramName.Data(),
						ushort2ll48(fChannelTime),
						fData[kHitEnergy]);
	} else {
		Out << Form("      %13d%13d%13d%13d%18lld%13d\n",
						fBufferNumber,
						fModuleNumber,
						fEventNumber,
						fChannel,
						ushort2ll48(fChannelTime),
						fData[kHitEnergy]);
	}
}
	
const Char_t * TUsrHit::ChannelTime2Ascii(TString & TimeString) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHit::ChannelTime2Ascii
// Purpose:        Convert 48-bit time to ascii
// Arguments:      TString & TimeString  -- where to store event time
// Results:        Char_t * TimeString   -- same as arg#1
// Exceptions:
// Description:    Converts a 48-bit event time to ascii.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TimeString = Form("%18lld", ushort2ll48(fChannelTime));
	TimeString = TimeString.Strip(TString::kBoth);
	return(TimeString.Data());
}

Int_t TUsrHitBuffer::AllocClonesArray(Int_t NofEntries, Int_t HighWater) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           AllocClonesArray
// Purpose:        Allocate TClonesArray
// Arguments:      Int_t NofEntries      -- number of entries
//                 Int_t HighWater       -- high water limit
// Results:        Int_t NofEntries      -- number of entries allocated
// Exceptions:
// Description:    Allocates a TClonesArray object for this hitbuffer.
//                 Resets buffer.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fNofEntries = NofEntries;
	fOffset = 0;		// no longer in use -> see class TUsrHBX
	fHighWater = (HighWater >= fNofEntries) ? 0 : HighWater;
	fHits = new TClonesArray("TUsrHit", NofEntries);
	this->Reset();
	return(fNofEntries);
}

void TUsrHitBuffer::Reset() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHitBuffer::Reset
// Purpose:        Reset hit buffer
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Clears the TClonesArray and reset hit count.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fHits->Clear();
	fNofHits = 0;
}

TUsrHit * TUsrHitBuffer::AddHit(Int_t EventNumber, Int_t ModuleNumber, Int_t Channel,
										UShort_t BufferTimeHi, UShort_t EventTimeHi, UShort_t FastTrigTime,
										UShort_t * Data, Int_t NofData) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHitBuffer::AddHit
// Purpose:        Add data to hit buffer
// Arguments:      Int_t EventNumber      -- event index within buffer
//                 Int_t ModuleNumber     -- module serial
//                 Int_t Channel          -- channel number
//                 UShort_t BufferTimeHi  -- event time, hi order
//                 UShort_t EventTimeHi   -- ..., middle
//                 UShort_t FastTrigTime  -- ..., low
//                 Int_t NofData          -- number of data words
//                 UShort_t * Data        -- hit data, length depending on format
// Results:        TUsrHit * Hit          -- pointer to hit object
// Exceptions:
// Description:    Adds a new item to hit list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fNofHits >= fNofEntries) {
		gMrbLog->Err()	<< "[" << this->GetName() << "] Buffer overflow - " << fNofEntries << " entries max" << endl;
		gMrbLog->Flush(this->ClassName(), "AddHit");
		return(NULL);
	}
	TClonesArray &hits = * fHits;
	TUsrHit * hit = new(hits[fNofHits]) TUsrHit(gMrbAnalyze->GetEventsProcessed(),
																			EventNumber,
																			ModuleNumber, Channel,
																			BufferTimeHi, EventTimeHi, FastTrigTime,
																			Data, NofData);
	fNofHits++;
	return(hit);
}

TUsrHit * TUsrHitBuffer::AddHit(Int_t EventNumber, Int_t ModuleNumber, Int_t Channel,
										UShort_t * ChannelTime,
										UShort_t * Data, Int_t NofData) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHitBuffer::AddHit
// Purpose:        Add data to hit buffer
// Arguments:      Int_t EventNumber      -- event index within buffer
//                 Int_t ModuleNumber     -- module serial
//                 Int_t Channel          -- channel number
//                 UShort_t ChannelTime   -- channel time
//                 Int_t NofData          -- number of data words
//                 UShort_t * Data        -- hit data, length depending on format
// Results:        TUsrHit * Hit          -- pointer to hit object
// Exceptions:
// Description:    Adds a new item to hit list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fNofHits >= fNofEntries) {
		gMrbLog->Err()	<< "[" << this->GetName() << "] Buffer overflow - " << fNofEntries << " entries max" << endl;
		gMrbLog->Flush(this->ClassName(), "AddHit");
		return(NULL);
	}
	TClonesArray &hits = * fHits;
	TUsrHit * hit = new(hits[fNofHits]) TUsrHit(gMrbAnalyze->GetEventsProcessed(),
																			EventNumber,
																			ModuleNumber, Channel,
																			ChannelTime,
																			Data, NofData);
	fNofHits++;
	return(hit);
}

Bool_t TUsrHitBuffer::RemoveHit(TUsrHit * Hit) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHitBuffer::RemoveHit
// Purpose:        Remove a hit from buffer
// Arguments:      TUsrHit * Hit    -- hit
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Removes a hit from buffer. 
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fHits->Remove(Hit);
	fNofHits--;
	return(kTRUE);
}

Bool_t TUsrHitBuffer::RemoveHit(Int_t Index) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHitBuffer::RemoveHit
// Purpose:        Remove a hit from buffer
// Arguments:      Int_t Index    -- index
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Removes a hit from buffer. 
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fHits->RemoveAt(Index);
	fNofHits--;
	return(kTRUE);
}

Bool_t TUsrHitBuffer::IsHighWater(Bool_t Verbose) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHitBuffer::IsHighWater
// Purpose:        Check for high water
// Arguments:      Bool_t Verbose   -- if kTRUE: output warning
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks if number of hits higher than high water margin.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if ((fHighWater > 0) && (fNofHits >= fHighWater)) {
		if (Verbose) {
			gMrbLog->Err()	<< "[" << this->GetName() << "] High water limit reached: "
							<< fNofHits << " hits (high water limit is " << fHighWater << ")" << endl;
			gMrbLog->Flush(this->ClassName(), "IsHighWater");
		}
		return(kTRUE);
	}
	return(kFALSE);
}

void TUsrHitBuffer::Print(ostream & Out, Int_t Begin, Int_t End) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHitBuffer::Print
// Purpose:        Print data
// Arguments:      ostream & Out      -- output stream
//                 Int_t Begin        -- entry to start with
//                 Int_t End          -- entry to end with
// Results:        --
// Exceptions:
// Description:    Prints entry data.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TUsrHit * hit;
	Int_t nofHits;
						
	nofHits = this->GetNofHits();
	if (nofHits == 0) {
		gMrbLog->Err()	<< "[" << this->GetName() << "] Hit buffer is empty ("
							<< this->GetNofEntries() << " entries)" << endl;
		gMrbLog->Flush(this->ClassName(), "Print");
		return;
	}
	
	if (End == -1 || End >= nofHits) End = nofHits - 1;
	if (Begin > End) {
		gMrbLog->Err()	<< "[" << this->GetName() << "] Wrong index range - ["
						<< Begin << ", " << End << "] (should be in [0, " << nofHits - 1 << "]" << endl;
		gMrbLog->Flush(this->ClassName(), "Print");
		return;
	}
	
	Out << "Contents of buffer \"" << this->GetName()
		<< "\" (" << fNofHits << " hit(s) out of " << fNofEntries << "):" << endl;
	Out << Form("  Entry      %13s%13s%13s%13s%18s%13s\n",
									"Buffer",
									"Module",
									"Event",
									"Chn",
									"Time",
									"Energy");
	for (Int_t i = Begin; i <= End; i++) {
		hit = (TUsrHit *) fHits->At(i);
		Out << Form("  %5d", i);
		if (hit) hit->Print(Out, kTRUE); else Out << " ... [empty slot]" << endl;
	}
}

TUsrHBX::TUsrHBX(TObject * Event, TUsrHitBuffer * HitBuffer, Int_t Window) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHBX
// Purpose:        Access hit buffer elements
// Arguments:      TUsrHitBuffer * HitBuffer  -- hit buffer to be accessed
//                 Int_t Window               -- time stamp window
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fEvent = Event;
	fHitBuffer = HitBuffer;
	fHits = fHitBuffer->GetCA();
	fWindow = Window;
	fCurIndex = -1;
}

TUsrHit * TUsrHBX::FindHit(TUsrHit & HitProfile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHBX::FindHit
// Purpose:        Search for a given hit
// Arguments:      TUsrHit HitProfile  -- hit containing search profile
// Results:        TUsrHit * Hit       -- next hit matching given profile
// Exceptions:
// Description:    Searches for an entry with given profile.
//                 Profile.GetBufferNumber() == -1  -> return hits with ANY event number
//                 Profile.GetEventNumber() == -1   -> return hits with ANY event index
//                 Profile.GetModuleNumber() == -1  -> return hits with ANY module number
//                 Profile.GetChannelNumber() == -1 -> return hits with ANY channel number
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TUsrHit * hit;
	Int_t n;
		
	Int_t curIndex = fCurIndex;
	while (++curIndex < this->GetNofHits()) {
		hit = (TUsrHit *) fHits->At(curIndex);
		n = HitProfile.GetBufferNumber();
		if (n == -1 || n == hit->GetBufferNumber()) {
			n = HitProfile.GetModuleNumber();
			if (n == -1 || n == hit->GetModuleNumber()) {
				n = HitProfile.GetEventNumber();
				if (n == -1 || n == hit->GetEventNumber()) {
					n = HitProfile.GetChannel();
					if (n == -1 || n == hit->GetChannel()) {
						fCurIndex = curIndex;
						return(hit);
					}
				}
			}
		}
	}
	return(NULL);
}

TUsrHit * TUsrHBX::FindEvent(Int_t EventNumber) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHBX::FindEvent
// Purpose:        Find first hit of next event in buffer
// Arguments:      Int_t EventNumber   -- event number
// Results:        TUsrHit * Hit       -- next hit with given event number
// Exceptions:
// Description:    Searches for next hit with given event number.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TUsrHit * hit;

	Int_t curIndex = fCurIndex;
	while (++curIndex < this->GetNofHits()) {
		hit = (TUsrHit *) fHits->At(curIndex);
		if (hit->GetEventNumber() == EventNumber) {
			fCurIndex = curIndex;
			return(hit);
		}
	}
	return(NULL);
}

TUsrHit * TUsrHBX::FindNextEvent() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHBX::FindNextEvent
// Purpose:        Find first hit of next event in buffer
// Arguments:      --
// Results:        TUsrHit * Hit       -- first hit of next event
// Exceptions:
// Description:    Searches for first hit of next event.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TUsrHit * hit;
	Int_t evtNo;

	Int_t nofHits = this->GetNofHits();

	if (nofHits && fCurIndex < nofHits) {
		if (fCurIndex == -1) {
			evtNo = -1;
		} else {
			evtNo = ((TUsrHit *) fHits->At(fCurIndex))->GetEventNumber();
		}
		Int_t curIndex = fCurIndex;
		while (++curIndex < nofHits) {
			hit = (TUsrHit *) fHits->At(curIndex);
			if (hit->GetEventNumber() > evtNo) {
				fCurIndex = curIndex;
				return(hit);
			}
		}
	}
	return(NULL);
}

Bool_t TUsrHBX::HitInWindow(TUsrHit * Hit0) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHBX::HitInWindow
// Purpose:        Check if hit within time window
// Arguments:      TUsrHit * Hit0      -- hit with time = 0
// Results:        kTRUE/kFALSE
// Description:    Checks if hit within time window.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TUsrHit * hit;

	Int_t curIndex = (fCurIndex == -1) ? 0 : fCurIndex;
	if (curIndex >= this->GetNofHits()) return(kFALSE);

	hit = (TUsrHit *) fHits->At(curIndex);
	long long tDiff =	(ushort2ll48(hit->GetChannelTime()) - gMrbAnalyze->GetTimeOffset(hit->GetModuleNumber()))
						- (ushort2ll48(Hit0->GetChannelTime()) - gMrbAnalyze->GetTimeOffset(Hit0->GetModuleNumber()));
	if (tDiff < 0) tDiff = -tDiff;
	return(tDiff <= (long long) fWindow);
}		

TUsrEvent::TUsrEvent() {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvent
// Purpose:        Base class for user-defined events
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fLofHBXs.Delete();
	fLofSubevents.Delete();
}


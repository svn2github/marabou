//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           transport/src/TMrbTransport.cxx
//
// Purpose:        MARaBOU transport mechanism
//
// Description:    Implements class methods to read MBS data from file or server
//
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbTransport.cxx,v 1.16 2006-06-23 08:48:30 Marabou Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <stdio.h>
#include <iostream>
#include <errno.h>

#include "mbsio.h"
#include "mbsio_protos.h"

#include "TEnv.h"

#include "TMrbNamedX.h"
#include "TMrbTransport.h"
#include "TMrbLogger.h"

#include "SetColor.h"

// global pthread mutex to protect TMapped data
extern pthread_mutex_t global_data_mutex;

TMrbTransport * gMrbTransport = NULL;

static Char_t mbs_error_string[1024];

static SMrbNamedXShort kMrbXptLofTransportModes[]		=	{	// legal transport modes and their names
									{TMrbTransport::kSync,		"SYNCHRONOUS"		},  // synchronous: via transport manager
									{TMrbTransport::kAsync, 	"ASYNCHRONOUS"		},  // asynchronous: via stream server
									{TMrbTransport::kFile,		"FILE"				},  // file: data from (local) lmd file
									{TMrbTransport::kRemote,	"REMOTEFILE"		},  // remotefile: data from remote lmd file
									{0, 						NULL				}
								};

static SMrbNamedXShort kMrbXptLofBufferElements[]		=	{	// legal buffer elements and their names
									{TMrbTransport::kBufferHeader,		"BUFFERHEADER"		},
									{TMrbTransport::kFileHeader,		"FILEHEADER"		},
									{TMrbTransport::kEventHeader,		"EVENTHEADER"		},
									{TMrbTransport::kSubeventHeader,	"SUBEVENTHEADER"	},
									{TMrbTransport::kSubeventData,		"DATA"				},
									{0, 								NULL				}
								};

extern TMrbLogger * gMrbLog;					// message logger

ClassImp(TMrbTransport)

TMrbTransport::TMrbTransport(const Char_t * XptName, const Char_t * XptTitle) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTransport
//
// Purpose:        Create a MARaBOU transport object
// Arguments:      Char_t * XptName  -- name of transport
//                 Char_t * XptTitle -- (opt.) title
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

	if (*XptTitle == '\0') {
		SetTitle("MARaBOU transport");
	} else {
		SetTitle(XptTitle);
	}
	SetName(XptName);

	fInputFile.Remove(0);				// initialize data section
	fTransportMode = kUndefined;
	fBufferSize = -1;
	fMBSDataIO = NULL;

	fStopFlag = kFALSE;

	mbs_pass_errors(mbs_error_string);	// pass mbsio errors to local buffer

										// initialize lists of key words	
	fLofTransportModes.SetName("Transport Modes");	// ... transport modes
	fLofTransportModes.SetPatternMode();
	fLofTransportModes.AddNamedX(kMrbXptLofTransportModes);	

	fLofBufferElements.SetName("Buffer Elements");	// ...buffer elements
	fLofBufferElements.SetPatternMode();
	fLofBufferElements.AddNamedX(kMrbXptLofBufferElements);

	gMrbTransport = this; 				// holds addr of current readout def
}

Bool_t TMrbTransport::Open(const Char_t * File, const Char_t * Mode, Int_t BufferSize) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTransport::Open
// Purpose:        Open connection (file or network)
// Arguments:      Char_t * File     -- file name or network addr
//                 Char_t * Mode     -- transport mode (FILE, SYNC, ASYNC, or REMOTE)
//                 Int_t BufferSize  -- buffer size (default is 16k)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Opens a connection to MBS: either reading from file or via network.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	MBSDataIO *mbs;
	TMrbNamedX * pmod;

	ClearError();

	pmod = fLofTransportModes.FindByName(Mode, TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase);

	if (BufferSize <= 0) {
		gMrbLog->Err()	<< "Illegal buffer size - " << BufferSize << endl;
		gMrbLog->Flush(this->ClassName(), "Open");
		SetError();
		return(kFALSE);
	} else if (pmod == NULL) {
		gMrbLog->Err()	<< "Illegal transport mode - " << Mode << endl;
		gMrbLog->Flush(this->ClassName(), "Open");
		SetError();
		return(kFALSE);
	} else if ((mbs = mbs_open_file((Char_t *) File, (Char_t *) pmod->GetName(), BufferSize, NULL)) != NULL) {
		fInputFile = File;
		fTransportMode = (TMrbTransport::EMrbTransportMode) pmod->GetIndex();
		fBufferSize = BufferSize;
		fMBSDataIO = mbs;
		Int_t scaleDown = gEnv->GetValue("TMrbTransport.ScaleDown", 100);
		Int_t dumpInterval = gEnv->GetValue("TMrbTransport.DumpInterval", 0);
		TString showItems = gEnv->GetValue("TMrbTransport.ShowItems", "ES");
		this->SetShow(showItems.Data(), scaleDown);
		this->SetDumpInterval(dumpInterval);
		this->SetStat(scaleDown);
		return(kTRUE);
	} else {
		PrintMbsIoError("Open");
		SetError();
		return(kFALSE);
	}
}

Bool_t TMrbTransport::Close() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTransport::Close
// Purpose:        Close connection (file or network)
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Closes an open connection to MBS.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	ClearError();

	if (fMBSDataIO == NULL || fMBSDataIO->fileno == -1) return(kTRUE);

	if (!mbs_close_file(fMBSDataIO)) {
		PrintMbsIoError("Close");
		SetError();
		return(kFALSE);
	} else {
		return(kTRUE);
	}
}

Bool_t TMrbTransport::FreeBuffers() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTransport::FreeBuffers
// Purpose:        Free data base buffers
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Removes data base.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	ClearError();

	if (fMBSDataIO == NULL) return(kTRUE);

	if (!mbs_free_dbase(fMBSDataIO)) {
		PrintMbsIoError("FreeBuffers");
		SetError();
		return(kFALSE);
	} else {
		fMBSDataIO = NULL;
		return(kTRUE);
	}
}

Int_t TMrbTransport::ReadEvents(Int_t NofEvents) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTransport::ReadEvents
// Purpose:        Read events from buffer
// Arguments:      Int_t NofEvents  -- number of events to be read (0 = ad infinitum)
// Results:        Int_t Rstatus    -- status info
// Exceptions:
// Description:    Reads a given number of events from MBS buffer.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	Int_t nofEventsProcessed;
	Int_t nofErrors;
	Int_t abortOnError;
	UInt_t eventType = 0;

	ClearError();

	nofEventsProcessed = 0;
	nofErrors = 0;

	if (fMBSDataIO == NULL) {
		gMrbLog->Err()	<< "MBSIO not active" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadEvents");
		SetError();
		return(kFALSE);
	}

	abortOnError = 0;
	if (NofEvents == 0) {
		for (;;) {
//			pthread_mutex_lock(&global_data_mutex);
			do {
				eventType = mbs_next_event(fMBSDataIO);
				if (this->IsToBeStopped()) {
					gMrbLog->Out()	<< "Detecting STOP flag" << endl;
					gMrbLog->Flush(this->ClassName(), "ReadEvents");
//					eventType = MBS_ETYPE_EOF; break; 
					eventType = 12345; 
				}
			} while (eventType == MBS_ETYPE_WAIT);
//			pthread_mutex_unlock(&global_data_mutex);
			if (eventType == MBS_ETYPE_EOF) {
				gMrbLog->Out()	<< "End of file " << fInputFile << endl;
				gMrbLog->Flush(this->ClassName(), "ReadEvents");
				SetError();
				Close();
				break;
			} else if (eventType == MBS_ETYPE_ERROR) {
				nofErrors++;
				SetError();
				abortOnError++;
				if (abortOnError > TMrbTransport::kMaxErrors) {
					gMrbLog->Err()	<< "Aborting after " << abortOnError << " subsequent errors" << endl;
					gMrbLog->Flush(this->ClassName(), "ReadEvents");
					eventType = MBS_ETYPE_ABORT;
         			break;
				}
			} else if (eventType == MBS_ETYPE_ABORT) {
				nofErrors++;
				SetError();
				PrintMbsIoError("ReadEvents");
				gMrbLog->Err()	<< "Aborting" << endl;
				gMrbLog->Flush(this->ClassName(), "ReadEvents");
				break;
			} else {
				if (!ProcessEvent((s_vehe *) fMBSDataIO->evt_data)) {
					eventType = MBS_ETYPE_ERROR;
					break;
				}
				abortOnError = 0;
				nofEventsProcessed++;
			}
		}
	} else {
		for (i = 0; i < NofEvents; i++) {
			eventType = MBS_ETYPE_WAIT;
//			pthread_mutex_lock(&global_data_mutex);
			while (eventType == MBS_ETYPE_WAIT) {
				eventType = mbs_next_event(fMBSDataIO);
				if (this->IsToBeStopped()) {
					gMrbLog->Out()	<< "Detecting STOP flag" << endl;
					gMrbLog->Flush(this->ClassName(), "ReadEvents");
					eventType = MBS_ETYPE_EOF; break; 
				}
			}
//			pthread_mutex_unlock(&global_data_mutex);
			if (eventType == MBS_ETYPE_EOF) {
				gMrbLog->Out()	<< "End of file " << fInputFile << endl;
				gMrbLog->Flush(this->ClassName(), "ReadEvents");
				Close();
				SetError();
				break;
			} else if (eventType == MBS_ETYPE_ERROR) {
				nofErrors++;
				PrintMbsIoError("ReadEvents");
				SetError();
				abortOnError++;
				if (abortOnError > TMrbTransport::kMaxErrors) {
					gMrbLog->Err()	<< "Aborting after " << abortOnError << " subsequent errors" << endl;
					gMrbLog->Flush(this->ClassName(), "ReadEvents");
					eventType = MBS_ETYPE_ABORT;
					break;
				}
			} else if (eventType == MBS_ETYPE_ABORT) {
				nofErrors++;
				PrintMbsIoError("ReadEvents");
				SetError();
				gMrbLog->Err()	<< "Aborting" << endl;
				gMrbLog->Flush(this->ClassName(), "ReadEvents");
				break;
			} else {
				if (!ProcessEvent((s_vehe *) fMBSDataIO->evt_data)) {
					eventType = MBS_ETYPE_ERROR;
					break;
            	}
				abortOnError = 0;
				nofEventsProcessed++;
			}
		}
	}
	if (nofErrors > 0) {
		gMrbLog->Err()	<< this->ClassName() << "::ReadEvents(): " << nofEventsProcessed
						<< " event(s), " << nofErrors
						<< " error(s)" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadEvents");
	} else {
		gMrbLog->Out()	<< this->ClassName() << "::ReadEvents(): " << nofEventsProcessed
						<< " event(s), " << " no errors" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadEvents", setblue);
	}
	return(eventType);
}

UInt_t TMrbTransport::NextSubevent(UShort_t * SevtData) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTransport::NextSubevent
// Purpose:        Get next subevent
// Arguments:      UShort_t SevtData  -- pointer to (unpacked) subevent data
// Results:        Int_t Swc          -- word count
// Exceptions:
// Description:    Unpacks next subevent from current event.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t SevtType;

	ClearError();
	SevtType = mbs_next_sevent(fMBSDataIO);
	if (SevtType == MBS_STYPE_EOE) {			// end of event
		return(0);
	} else if (SevtType == MBS_STYPE_ERROR || SevtType == MBS_STYPE_ABORT) {	// error
		PrintMbsIoError("NextSubevent");
		SetError();
		return(0xffffffff);
	} else {									// legal data
		mbs_pass_sevent(fMBSDataIO, SevtData);
		return(fMBSDataIO->sevt_wc);
	}
}

Bool_t TMrbTransport::ProcessEvent(s_vehe * EventData) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTransport::ProcessEvent
// Purpose:        Process event data
// Arguments:      s_vehe * EventData  -- pointer to event data (including header)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    This is the event filling function.
//                 User will be called by method Analyze().
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UShort_t Data[1024];

	for (;;) {
		if (NextSubevent(Data) <= 0) break;
	}
	return(kTRUE);
}					


const UShort_t * TMrbTransport::NextSubevent() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTransport::NextSubevent
// Purpose:        Get next subevent
// Arguments:      none
// Results:        const UShort_t * SevtData  -- pointer to current (unpacked) data
// Exceptions:
// Description:    Unpacks next subevent from current event.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t SevtType;

	ClearError();
	SevtType = mbs_next_sevent(fMBSDataIO);
	if (SevtType == MBS_STYPE_EOE) {			// end of event
		return(NULL);
	} else if (SevtType == MBS_STYPE_ERROR || SevtType == MBS_STYPE_ABORT) {	// error
		PrintMbsIoError("NextSubevent");
		SetError();
		return(NULL);
	} else {									// legal data
		return((UShort_t *) fMBSDataIO->sevt_data);
	}
}

Bool_t TMrbTransport::Show(const Char_t * BufElemKey, const Char_t * Output) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTransport::Show
// Purpose:        Show buffer elem data
// Arguments:      Char_t * BufElemKey  -- buffer elem key (one char out of "FBES")
//                 Char_t * Output      -- output stream (NULL = stdout)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Outputs samples of buffer element data.
//                 Buffer element may be: F(ileheader), B(uffer), E(vent), or S(ubevent)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	FILE * out;
	Bool_t sts;
	TMrbNamedX *pbe;
	TString keyName;

	ClearError();

	if (Output != NULL) {
		out = fopen(Output, "a");
		if (out == NULL) {
			gMrbLog->Err()	<< "Can't open file \"" << Output << "\"" << endl;
			gMrbLog->Flush(this->ClassName(), "Show");
			SetError();
			return(kFALSE);
		}
	} else {
		out = stdout;
	}

	if ((pbe = fLofBufferElements.FindByName(BufElemKey, TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase)) == NULL) {
		gMrbLog->Err()	<< "Illegal buffer element - " << BufElemKey << endl;
		gMrbLog->Flush(this->ClassName(), "Show");
		return(kFALSE);
	}

	keyName = pbe->GetName();
	keyName.Resize(1);
	sts = mbs_show(fMBSDataIO, keyName.Data(), out);

	if (Output != NULL) fclose(out);

	if (!sts) {
		PrintMbsIoError("Show");
		SetError();
	}
	return(sts);
}

Bool_t TMrbTransport::SetShow(const Char_t * BufElemStr, Int_t ScaleDown, const Char_t * Output) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTransport::SetShow
// Purpose:        Define show profile
// Arguments:      Char_t * BufElemStr  -- string of buffer elem keys (out of "FBES")
//                 Int_t ScaleDown      -- scale down
//                 Char_t * Output      -- output stream (NULL = stdout)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines which buffer elements to be shown automatically.
//                 Buffer elements may be: F(ileheader), B(uffer), E(vent), or S(ubevent)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	FILE *out;
	Bool_t sts;
	UInt_t xbe;
	TString sbe;

	ClearError();

	if (Output != NULL) {
		out = fopen(Output, "a");
		if (out == NULL) {
			gMrbLog->Err()	<< "Can't open file \"" << Output << "\"" << endl;
			gMrbLog->Flush(this->ClassName(), "SetShow");
			SetError();
			return(kFALSE);
		}
	} else {
		out = stdout;
	}

	xbe = fLofBufferElements.FindPattern(BufElemStr, TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase);
	if (xbe == 0) {
		gMrbLog->Err()	<< "Illegal buffer element(s) - " << BufElemStr << endl;
		gMrbLog->Flush(this->ClassName(), "SetShow");
		return(kFALSE);
	}
	sbe.Remove(0);
	if (xbe & kBufferHeader) sbe += 'B';
	if (xbe & kFileHeader) sbe += 'F';
	if (xbe & kEventHeader) sbe += 'E';
	if (xbe & kSubeventHeader) sbe += 'S';

	sts = mbs_set_show(fMBSDataIO, sbe.Data(), ScaleDown, out);
	if (!sts) {
		PrintMbsIoError("SetShow");
		SetError();
	}
	return(sts);
}

Bool_t TMrbTransport::ShowStat(const Char_t * Output) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTransport::ShowStat
// Purpose:        Show events statistics
// Arguments:      Char_t * Output  -- output stream (NULL = stdout)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Outputs events statistics.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	FILE *out;
	Bool_t sts;

	ClearError();

	if (Output != NULL) {
		out = fopen(Output, "a");
		if (out == NULL) {
			gMrbLog->Err()	<< "Can't open file \"" << Output << "\"" << endl;
			gMrbLog->Flush(this->ClassName(), "ShowStat");
			SetError();
			return(kFALSE);
		}
	} else {
		out = stdout;
	}

	sts = mbs_show_stat(fMBSDataIO, out);
	if (Output != NULL) fclose(out);
	if (!sts) {
		PrintMbsIoError("ShowStat");
		SetError();
	}
	return(sts);
}

Bool_t TMrbTransport::SetStat(Int_t ScaleDown, const Char_t * Output) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTransport::SetStat
// Purpose:        Define statistics profile
// Arguments:      Int_t ScaleDown  -- scale down
//                 Char_t * Output  -- output stream (NULL = stdout)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines a scale down factor for automatic stats output.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	FILE *out;
	Bool_t sts;

	ClearError();

	if (Output != NULL) {
		out = fopen(Output, "a");
		if (out == NULL) {
			gMrbLog->Err()	<< "Can't open file \"" << Output << "\"" << endl;
			gMrbLog->Flush(this->ClassName(), "SetStat");
			SetError();
			return(kFALSE);
		}
	} else {
		out = stdout;
	}

	sts = mbs_set_stat(fMBSDataIO, ScaleDown, out);
	if (!sts) {
		PrintMbsIoError("SetStat");
		SetError();
	}
	return(sts);
}

Bool_t TMrbTransport::SetStream(Int_t MaxStreams, Int_t SlowDown) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTransport::SetStream
// Purpose:        Define stream profile
// Arguments:      Int_t MaxStreams  -- max number of streams to be processed (0 = ad infinitum)
//                 Int_t SlowDown    -- number of secs to wait after each request
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines how many streams to be processed.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t sts;

	ClearError();

	sts = mbs_set_stream(fMBSDataIO, MaxStreams, SlowDown);
	if (!sts) {
		PrintMbsIoError("SetStream");
		SetError();
	}
	return(sts);
}

Bool_t TMrbTransport::SetDumpInterval(Int_t NofRecs) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTransport::SetDumpInterval
// Purpose:        Define dump interval
// Arguments:      Int_t NofRecs   -- dump interval
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines when to dump records to disk.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	ClearError();

	mbs_set_dump(fMBSDataIO, NofRecs);
	return(kTRUE);
}

Bool_t TMrbTransport::OpenMEDFile(const Char_t * MEDFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTransport::OpenMEDFile
// Purpose:        Open MED file 
// Arguments:      Char_t * MEDFile    -- file name
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Opens a file to write data as MBS events.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t sts;

	ClearError();

	sts = mbs_open_med(MEDFile);
	if (sts) {
		gMrbLog->Out()	<< "Writing MBS event data to file " << MEDFile << endl;
		gMrbLog->Flush(this->ClassName(), "OpenMEDFile", setblue);
	} else {
		PrintMbsIoError("OpenMEDFile");
		SetError();
	}
	return(sts);
}

Bool_t TMrbTransport::CloseMEDFile() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTransport::CloseMEDFile
// Purpose:        Close MED file 
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Closes MED file which is currently open
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	ClearError();

	mbs_close_med();
	gMrbLog->Out()	<< "MED file closed" << endl;
	gMrbLog->Flush(this->ClassName(), "CloseMEDFile", setblue);
	return(kTRUE);
}

Bool_t TMrbTransport::OpenLMDFile(const Char_t * LMDFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTransport::OpenLMDFile
// Purpose:        Open LMD file 
// Arguments:      Char_t * LMDFile    -- lmd file
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Opens a file to write original LMD data.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t sts;

	ClearError();

	sts = mbs_open_lmd(LMDFile);
	if (sts) {
		gMrbLog->Out()	<< "Writing LMD data to file " << LMDFile << endl;
		gMrbLog->Flush(this->ClassName(), "OpenLMDFile", setblue);
	} else {
		PrintMbsIoError("OpenLMDFile");
		SetError();
	}
	return(sts);
}

Bool_t TMrbTransport::CloseLMDFile() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTransport::CloseLMDFile
// Purpose:        Close LMD file 
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Closes LMD file which is currently open
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	ClearError();

	mbs_close_lmd();
	gMrbLog->Out()	<< "LMD file closed" << endl;
	gMrbLog->Flush(this->ClassName(), "CloseLMDFile", setblue);
	return(kTRUE);
}

Bool_t TMrbTransport::OpenLogFile(const Char_t * LogFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTransport::OpenLogFile
// Purpose:        Open file for log output
// Arguments:      Char_t * LogFile    -- log file
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Opens a file for data/error logging.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t sts;

	ClearError();

	sts = mbs_open_log(LogFile);
	if (!sts) {
		PrintMbsIoError("OpenLogFile");
		SetError();
	}
	return(sts);
}

Bool_t TMrbTransport::PrintMbsIoError(const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTransport::PrintMbsIoError
// Purpose:        Output errors coming from mbsio package
// Arguments:      Char_t * Prefix  -- prefix to be output in front
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Reports errors passed over by mbsio package.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fErrorString = (Char_t *) mbs_error_string;

	gMrbLog->Err()	<< Prefix << "(): Error in mbsio package -" << endl;
	gMrbLog->Flush(this->ClassName(), "PrintMbsIoError");
	gMrbLog->Err()	<< fErrorString << endl;
	gMrbLog->Flush(this->ClassName(), "PrintMbsIoError");
	fErrorString.Remove(0);
	
	return(kTRUE);
}

Bool_t TMrbTransport::Version() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTransport::Version
// Purpose:        Output version
// Arguments:      --
// Results:        kTRUE
// Exceptions:
// Description:    Outputs some welcome text.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString vtext;
	TString frame;
	Int_t fl, i1, i2;

	frame = "+--------------------------------------------+";
	vtext = "TMrbTransport v%I% (%G%)";
	fl = frame.Length() - 2;
	i1 = (fl - vtext.Length())/2;
	i2 = fl - vtext.Length() - i1;

	cout << "  " << frame << endl;

	cout << "  |";
	cout << setw(i1) << " ";
	cout << vtext;
	cout << setw(i2) << " ";
	cout << "|" << endl; 

	cout << "  |                TMrbTransport               |" << endl;
	cout << "  |    MARaBOU class to connect to MBS data    |" << endl;
	cout << "  |            via file or tcp server          |" << endl;
	cout << "  |               (c) R. Lutter                |" << endl;
	cout << "  " << frame << endl << endl;

	return(kTRUE);
}

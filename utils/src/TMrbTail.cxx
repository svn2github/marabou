//__________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/src/TMrbTail.cxx
// Purpose:        MARaBOU utilities:
//                 MARaBOU's tail utility
// Description:    Implements class methods to output file contents like tail(1)
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbTail.cxx,v 1.4 2008-08-18 08:18:57 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include <sys/ioctl.h>

#include "TROOT.h"
#include "TObjString.h"
#include "SetColor.h"
#include "TMrbTail.h"

extern TMrbLogger * gMrbLog;

ClassImp(TMrbTail)

TMrbTail::TMrbTail(const Char_t * TailName, const Char_t * TailFile) : TMrbNamedX(0, TailName, "") {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTail
// Purpose:        MARaBOU's tail utility
// Arguments:      Char_t * TailName   -- name of object
//                 Char_t * TailFile   -- file to be opened
// Description:    Initializes MARaBOU's tail utility.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

	fFilDes = open(TailFile, O_RDONLY | O_NONBLOCK);
	if (fFilDes == -1) {
		gMrbLog->Err() << "Can't open file - " << TailFile << endl;
		gMrbLog->Flush(this->ClassName());
		this->MakeZombie();
	} else {
		TString title = TailName;
		title += " (file: ";
		title += TailFile;
		title += ")";
		this->SetTitle(title.Data());
		fStrm = fdopen(fFilDes, "r");
		fTimer = new TTimer(this, 100);
		fStopIt = kTRUE;
		fOutputMode = kMrbTailOutUndef;
		fLogger = NULL;
		fStdout = NULL;
		fStderr = NULL;
		fReceiver = NULL;
		fTextArray = NULL;
	}
}
		
TMrbTail::TMrbTail(const Char_t * TailName, FILE * TailStrm) : TMrbNamedX(0, TailName, "") {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTail
// Purpose:        MARaBOU's tail utility
// Arguments:      Char_t * TailName   -- name of object
//                 FILE * TailStrm     -- file stream
// Description:    Initializes MARaBOU's tail utility.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

	fFilDes = fileno(TailStrm);
	if (fFilDes == -1) {
		gMrbLog->Err() << "Can't open tail file" << endl;
		gMrbLog->Flush(this->ClassName());
		this->MakeZombie();
	} else {
		this->SetTitle(TailName);
		fStrm = TailStrm;
		ioctl(fFilDes, O_RDONLY | O_NONBLOCK);
		fTimer = new TTimer(this, 100);
		fStopIt = kTRUE;
		fOutputMode = kMrbTailOutUndef;
		fLogger = NULL;
		fStdout = NULL;
		fStderr = NULL;
		fReceiver = NULL;
		fTextArray = NULL;
	}
}
		
void TMrbTail::Start(Bool_t SkipToEnd) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTail::Start
// Purpose:        Start output
// Arguments:      Bool_t SkipToEnd  -- if kTRUE only future lines will be output
// Results:        --
// Exceptions:     
// Description:    Starts 
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Char_t str[1024];

	if (SkipToEnd) while (fgets(str, 1024, fStrm) != NULL);
	if (fOutputMode == kMrbTailOutUndef) this->SetOutput(gMrbLog ? gMrbLog : new TMrbLogger("tail.log"));
	if (fOutputMode == kMrbTailOutArray) fTextArray->Delete();
	fTimer->TurnOn();
	fStopIt = kFALSE;
}

Bool_t TMrbTail::HandleTimer(TTimer * Timer) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTail::HandleTimer
// Purpose:        Timer interrupt
// Arguments:      TTImer * Timer   -- timer
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Services a timer interrupt.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Char_t str[1024];

	Bool_t newData = kFALSE;
	while (fgets(str, 1024, fStrm) != NULL) {
		newData = kTRUE;
		str[strlen(str) - 1] = '\0';
		TString xstr = str;
		switch (fOutputMode) {
			case kMrbTailOutLogger: 	this->ToLogger(xstr); break;
			case kMrbTailOutStdio:		this->ToStdio(xstr); break;
			case kMrbTailOutArray:		this->ToArray(xstr); break;
		}	
	}
	if (newData && (fOutputMode == kMrbTailOutArray)) fReceiver->Notify();

	if (!fStopIt) Timer->TurnOn();
	return(kTRUE);
}

Bool_t TMrbTail::SetOutput(TMrbLogger * Logger) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTail::SetOutput
// Purpose:        Define output mode
// Arguments:      TMrbLogger * Logger   -- output to logger
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Defines output mode.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fOutputMode != kMrbTailOutUndef) {
		gMrbLog->Err() << "Output mode already defined" << endl;
		gMrbLog->Flush(this->ClassName());
		return(kFALSE);
	} else {
		fLogger = Logger;
		fOutputMode = kMrbTailOutLogger;
		return(kTRUE);
	}
}

Bool_t TMrbTail::SetOutput(ostream & Stdout, ostream & Stderr) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTail::SetOutput
// Purpose:        Define output mode
// Arguments:      ostream & Stdout     -- output to stdio
//                 ostream & Stderr     -- ... stderr
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Defines output mode.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fOutputMode != kMrbTailOutUndef) {
		gMrbLog->Err() << "Output mode already defined" << endl;
		gMrbLog->Flush(this->ClassName());
		return(kFALSE);
	} else {
		fStdout = &Stdout;
		fStderr = &Stderr;
		fOutputMode = kMrbTailOutStdio;
		return(kTRUE);
	}
}

Bool_t TMrbTail::SetOutput(TObject * Receiver, TObjArray * TextArray) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTail::SetOutput
// Purpose:        Define output mode
// Arguments:      TObject * Receiver    -- receiving object, to be notified
//                 TObjArray * TextArray -- text array
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Defines output mode.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fOutputMode != kMrbTailOutUndef) {
		gMrbLog->Err() << "Output mode already defined" << endl;
		gMrbLog->Flush(this->ClassName());
		return(kFALSE);
	} else {
		fReceiver = Receiver;
		fTextArray = TextArray;
		fOutputMode = kMrbTailOutArray;
		return(kTRUE);
	}
}

void TMrbTail::ToLogger(TString & Text) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTail::ToLogger
// Purpose:        Output to logger
// Arguments:      TString & Text   -- line of text to be output
// Results:        --
// Exceptions:     
// Description:    Outputs text to logger
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Text(0) == '?') 		fLogger->Err() << Text << endl;
	else if (Text(0) == '%')	fLogger->Wrn() << Text << endl;
	else						fLogger->Out() << Text << endl;
	fLogger->Flush();
}

void TMrbTail::ToStdio(TString & Text) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTail::ToStdio
// Purpose:        Output to stdio
// Arguments:      TString & Text   -- line of text to be output
// Results:        --
// Exceptions:     
// Description:    Outputs text to cout/cerr
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Text(0) == '?') 		*fStderr << Text << endl;
	else if (Text(0) == '%')	*fStderr << Text << endl;
	else						*fStdout << Text << endl;
}

void TMrbTail::ToArray(TString & Text) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTail::ToArray
// Purpose:        Output to array
// Arguments:      TString & Text   -- line of text to be output
// Results:        --
// Exceptions:     
// Description:    Outputs text to array
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fTextArray->Add(new TObjString(Text.Data()));
}

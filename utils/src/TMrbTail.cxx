//__________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/src/TMrbTail.cxx
// Purpose:        MARaBOU utilities:
//                 MARaBOU's tail utility
// Description:    Implements class methods to output file contents like tail(1)
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include "TROOT.h"
#include "TFile.h"
#include "SetColor.h"
#include "TMrbTail.h"

ClassImp(TMrbTail)

TMrbTail::TMrbTail(const Char_t * TailName, const Char_t * TailFile) : TMrbLogger(TailName, NULL) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTail
// Purpose:        MARaBOU's tail utility
// Arguments:      Char_t * TailName   -- name of object
//                 Char_t * TailFile   -- file to be opened
// Description:    Initializes MARaBOU's tail utility.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fFilDes = open(TailFile, O_RDONLY | O_NONBLOCK);
	if (fFilDes == -1) {
		this->Err() << "Can't open file - " << TailFile << endl;
		this->Flush(this->ClassName());
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
		fSkipOrigPrefix = kFALSE;
		fNewPrefix = "";
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

	Bool_t isError, isWarning, hasPrefix;
	Char_t str[1024];

	hasPrefix = fNewPrefix.Length() > 0;

	while (fgets(str, 1024, fStrm) != NULL) {
		str[strlen(str) - 1] = '\0';
		TString xstr = str;
		isError = xstr(0) == '?';
		isWarning = xstr(0) == '%';
		if (fSkipOrigPrefix) {
			Int_t n = xstr.Index("- ", 0);
			if (n >= 0) xstr = xstr(n + 2, 1024);
		}
		if (isError || isWarning) {
			if (hasPrefix) this->Err() << fNewPrefix << "- ";
			this->Err() << xstr << endl;
		} else {
			if (hasPrefix) this->Out() << fNewPrefix << "- ";
			this->Out() << xstr << endl;
		}
		this->Flush();
	}
	if (!fStopIt) Timer->TurnOn();
	return(kTRUE);
}

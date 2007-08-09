//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbIOSpec.cxx
// Purpose:        Implement class methods for user's analysis
// Description:    Specification of input/output
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbIOSpec.cxx,v 1.6 2007-08-09 13:32:27 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include "TSystem.h"
#include "TObjString.h"

#include "TMrbIOSpec.h"
#include "TMrbLogger.h"

extern TMrbLogger * gMrbLog;					// message logger

ClassImp(TMrbIOSpec)

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

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

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
//                          ValString = "hh:mm:ss:xxx" -> time stamp
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString valAscii = ValAscii.Strip(TString::kBoth);

	Value = 0;
	TString hmsx;

	Int_t idx = valAscii.Index(":", 0);
	if (idx == -1) {		// event count
		TimeStampFlag = kFALSE;
		Value = valAscii.Atoi();			// convert to integer
		return(kTRUE);
	} else {								// time stamp
		if (idx == 0) valAscii.Prepend(" ");
		TimeStampFlag = kTRUE;
		TObjArray * tsArr = valAscii.Tokenize(":");
		Int_t nts = tsArr->GetEntries();

		cout << "@@@ " << nts << endl;
		tsArr->Print();

		switch (nts) {
			case 1: break;

			case 2:
				{
					TString h = ((TObjString *) tsArr->At(0))->GetString();
					h = h.Strip(TString::kBoth);
					if (h.Length() == 0) {										// :nnnnnn -> raw time stamp
						Value = atoi(((TObjString *) tsArr->At(1))->GetString());
						delete tsArr;
						return(kTRUE);
					} else {													// hh:mm
						Int_t hh = atoi(h.Data());
						TString m = ((TObjString *) tsArr->At(1))->GetString();
						m = m.Strip(TString::kBoth);
						Int_t mm = (m.Length() == 0) ? 0 : atoi(m.Data());
						if (mm < 0 || mm > 59) return(kFALSE);
						Value = ((hh * 60 + mm) * 60) * 1000;
						delete tsArr;
						return(kTRUE);
					}
				}
			case 3:
				{
					TString h = ((TObjString *) tsArr->At(0))->GetString(); 		// hh:mm:ss
					h = h.Strip(TString::kBoth);
					Int_t hh = (h.Length() == 0) ? 0 : atoi(h.Data());
					TString m = ((TObjString *) tsArr->At(1))->GetString();
					m = m.Strip(TString::kBoth);
					Int_t mm = (m.Length() == 0) ? 0 : atoi(m.Data());
					TString s = ((TObjString *) tsArr->At(2))->GetString();
					s = s.Strip(TString::kBoth);
					Int_t ss = (s.Length() == 0) ? 0 : atoi(s.Data());
					if (mm < 0 || mm > 59 || ss < 0 || ss > 59) return(kFALSE);
					Value = ((hh * 60 + mm) * 60 + ss) * 1000;
					delete tsArr;
					return(kTRUE);
				}
			case 4:
				{
					TString h = ((TObjString *) tsArr->At(0))->GetString(); 		// hh:mm:ss:xxx
					h = h.Strip(TString::kBoth);
					Int_t hh = (h.Length() == 0) ? 0 : atoi(h.Data());
					TString m = ((TObjString *) tsArr->At(1))->GetString();
					m = m.Strip(TString::kBoth);
					Int_t mm = (m.Length() == 0) ? 0 : atoi(m.Data());
					TString s = ((TObjString *) tsArr->At(2))->GetString();
					s = s.Strip(TString::kBoth);
					Int_t ss = (s.Length() == 0) ? 0 : atoi(s.Data());
					TString x = ((TObjString *) tsArr->At(3))->GetString();
					x = x.Strip(TString::kBoth);
					Int_t xxx = (x.Length() == 0) ? 0 : atoi(x.Data());
					delete tsArr;
					if (mm < 0 || mm > 59 || ss < 0 || ss > 59 || xxx < 0 || xxx > 999) return(kFALSE);
					Value = ((hh * 60 + mm) * 60 + ss) * 1000 + xxx;
					return(kTRUE);
				}
			default:	break;
		}
		delete tsArr;
	}
	return(kFALSE);
}

void TMrbIOSpec::ConvertToTimeStamp(TString & TimeStamp, Int_t TstampValue) const {
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

	xxx = TstampValue % 10000;
	TstampValue /= 10000;
	ss = TstampValue % 60;
	TstampValue /= 60;
	mm = TstampValue % 60;
	hh = TstampValue / 60;
	TimeStamp = (xxx > 0) ?	Form("%d%02d:%02d:%d", hh, mm, ss, xxx) : Form("%d%02d:%02d", hh, mm, ss);
}	

void TMrbIOSpec::Print(ostream & out) const {
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

	if (fHistoMode & TMrbIOSpec::kHistoClear)				out << "; histo space cleared on start";
	else if (fHistoMode & TMrbIOSpec::kHistoAdd) 			out << "; accumulation of histos continuing";
	if (fHistoMode & TMrbIOSpec::kHistoSave)				out << "; histos to be saved to file " << fHistoFile;
	out << ";" << endl;
}


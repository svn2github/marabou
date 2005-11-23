//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbIOSpec.cxx
// Purpose:        Implement class methods for user's analysis
// Description:    Specification of input/output
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbIOSpec.cxx,v 1.2 2005-11-23 13:10:16 Rudolf.Lutter Exp $       
// Date:           

//Begin_Html
/*
<h2>Format of a file list entry</h2>

A file list (extension .list) may be used as input to <b>C_analyze</b>.
It will be read by <b>TMrbAnalyze::OpenFileList()</b>, and processed by <b>TMrbAnalyze::ProcessFileList()</b>.
Data will be stored in a <b>TMrbIOspec</b> object.

<p>
Format is<br>
<pre>
<b>inputFile</b> <b>startEvent</b> <b>stopEvent</b> <b>paramFile</b> <b>histoFile</b> <b>outputFile</b>
</pre>

</p>
<table border="1">
  <thead>
    <tr>
      <th scope=col>Item</th>
      <th scope=col>Value  </th>
      <th scope=col>Meaning</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><pre><b>inputFile</b></pre></td>
      <td>  </td>
      <td>name of input file</td>
    </tr>
    <tr>
      <td>  </td>
      <td>xyz.root</td>
      <td><div align="right">contains ROOT trees</div></td>
    </tr>
    <tr>
      <td>  </td>
      <td>xyz.lmd</td>
      <td><div align="right">contains MBS list mode data</div></td>
    </tr>
    <tr>
      <td>  </td>
      <td>xyz.med</td>
      <td><div align="right">contains MBS event data</div></td>
    </tr>
    <tr>
      <td><pre><b>startEvent</b></pre></td>
      <td>  </td>
      <td>event or time stamp to start with</td>
    </tr>
    <tr>
      <td>  </td>
      <td>N</td>
      <td><div align="right">event number</div></td>
    </tr>
    <tr>
      <td>  </td>
      <td>hh:mm:ss</td>
      <td><div align="right">time stamp</div></td>
    </tr>
    <tr>
      <td>  </td>
      <td>:nnnnnnnn</td>
      <td><div align="right">time stamp</div></td>
    </tr>
    <tr>
      <td>  </td>
      <td>0</td>
      <td><div align="right">read from begin of file</div></td>
    </tr>
    <tr>
      <td><pre><b>stopEvent</b></pre></td>
      <td>  </td>
      <td>event or time stamp to stop with</td>
    </tr>
    <tr>
      <td>  </td>
      <td>N</td>
      <td><div align="right">event number</div></td>
    </tr>
    <tr>
      <td>  </td>
      <td>hh:mm:ss[:xxx]</td>
      <td><div align="right">time stamp</div></td>
    </tr>
    <tr>
      <td>  </td>
      <td>:nnnnnnnn</td>
      <td><div align="right">time stamp</div></td>
    </tr>
    <tr>
      <td>  </td>
      <td>0</td>
      <td><div align="right">read to end of file</div></td>
    </tr>
    <tr>
      <td><pre><b>paramFile</b></pre></td>
      <td>  </td>
      <td>file to reload params from</td>
    </tr>
    <tr>
      <td>  </td>
      <td>xyz.root</td>
      <td><div align="right">contains ROOT objects</div></td>
    </tr>
    <tr>
      <td>  </td>
      <td>xyz.par</td>
      <td><div align="right">contains ASCII text</div></td>
    </tr>
    <tr>
      <td>  </td>
      <td>-</td>
      <td><div align="right">params remain unchanged</div></td>
    </tr>
    <tr>
      <td>  </td>
      <td>none</td>
      <td><div align="right">don't load params</div></td>
    </tr>
    <tr>
      <td><pre><b>histoFile</b></pre></td>
      <td>  </td>
      <td>file to store histograms</td>
    </tr>
    <tr>
      <td>  </td>
      <td>xyz.root</td>
      <td><div align="right">clear histos in memory on start<br>save histos to file at EOF</div></td>
    </tr>
    <tr>
      <td>  </td>
      <td>+</td>
      <td><div align="right">don't clear histos<br>continue accumulation<br>save to file currently in use</div></td>
    </tr>
     <tr>
      <td><pre><b>name of outputFile</b></pre></td>
      <td>  </td>
      <td>output file</td>
    </tr>
    <tr>
      <td>  </td>
      <td>xyz.root</td>
      <td><div align="right">write ROOT trees to file</div></td>
    </tr>
    <tr>
      <td>  </td>
      <td>+</td>
      <td><div align="right">append ROOT trees to file currently open</div></td>
    </tr>
    <tr>
      <td>  </td>
      <td>none</td>
      <td><div align="right">don't output tree data</div></td>
    </tr>
 </tbody>
</table>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TSystem.h"
#include "TObjString.h"

#include "TMrbIOSpec.h"
#include "TMrbLogger.h"
#include "TMrbString.h"

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

	TMrbString valAscii;
	TString hmsx;

	valAscii = ValAscii.Strip(TString::kBoth);

	Value = 0;

	if (valAscii.Index(":", 0) == -1) {		// event count
		TimeStampFlag = kFALSE;
		valAscii.ToInteger(Value);			// convert to integer
		return(kTRUE);
	} else {								// time stamp
		TimeStampFlag = kTRUE;
		TObjArray tsArr;
		Int_t nts = valAscii.Split(tsArr, ":");

		switch (nts) {
			case 1: break;

			case 2:
				{
					TString h = ((TObjString *) tsArr[0])->GetString();
					h = h.Strip(TString::kBoth);
					if (h.Length() == 0) {										// :nnnnnn -> raw time stamp
						Value = atoi(((TObjString *) tsArr[1])->GetString());
						return(kTRUE);
					} else {													// hh:mm
						Int_t hh = atoi(h.Data());
						TString m = ((TObjString *) tsArr[1])->GetString();
						m = m.Strip(TString::kBoth);
						Int_t mm = (m.Length() == 0) ? 0 : atoi(m.Data());
						if (mm < 0 || mm > 59) return(kFALSE);
						Value = ((hh * 60 + mm) * 60) * 1000;
						return(kTRUE);
					}
				}
			case 3:
				{
					TString h = ((TObjString *) tsArr[0])->GetString(); 		// hh:mm:ss
					h = h.Strip(TString::kBoth);
					Int_t hh = (h.Length() == 0) ? 0 : atoi(h.Data());
					TString m = ((TObjString *) tsArr[1])->GetString();
					m = m.Strip(TString::kBoth);
					Int_t mm = (m.Length() == 0) ? 0 : atoi(m.Data());
					TString s = ((TObjString *) tsArr[2])->GetString();
					s = s.Strip(TString::kBoth);
					Int_t ss = (s.Length() == 0) ? 0 : atoi(s.Data());
					if (mm < 0 || mm > 59 || ss < 0 || ss > 59) return(kFALSE);
					Value = ((hh * 60 + mm) * 60 + ss) * 1000;
					return(kTRUE);
				}
			case 4:
				{
					TString h = ((TObjString *) tsArr[0])->GetString(); 		// hh:mm:ss:xxx
					h = h.Strip(TString::kBoth);
					Int_t hh = (h.Length() == 0) ? 0 : atoi(h.Data());
					TString m = ((TObjString *) tsArr[1])->GetString();
					m = m.Strip(TString::kBoth);
					Int_t mm = (m.Length() == 0) ? 0 : atoi(m.Data());
					TString s = ((TObjString *) tsArr[2])->GetString();
					s = s.Strip(TString::kBoth);
					Int_t ss = (s.Length() == 0) ? 0 : atoi(s.Data());
					TString x = ((TObjString *) tsArr[3])->GetString();
					x = x.Strip(TString::kBoth);
					Int_t xxx = (x.Length() == 0) ? 0 : atoi(x.Data());
					if (mm < 0 || mm > 59 || ss < 0 || ss > 59 || xxx < 0 || xxx > 999) return(kFALSE);
					Value = ((hh * 60 + mm) * 60 + ss) * 1000 + xxx;
					return(kTRUE);
				}
			default:	break;
		}
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


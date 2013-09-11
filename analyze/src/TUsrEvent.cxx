//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvent.cxx
// Purpose:        Implement class methods for user's analysis
// Description:    Base class for user-defined events
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TUsrEvent.cxx,v 1.11 2011-09-16 12:12:32 Marabou Exp $
// Date:
//////////////////////////////////////////////////////////////////////////////

#include "TMrbAnalyze.h"
#include "TUsrEvent.h"
#include "mbsio_protos.h"
#include "SetColor.h"

extern TMrbLogger * gMrbLog;					// message logger

ClassImp(TUsrEvent)

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

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

	fBranch = NULL;
	fTreeOut = NULL;
	fTreeIn = NULL;
	fReplayMode = kFALSE;
	fFakeMode = kFALSE;
	fWriteTree = kFALSE;
	fNofEntries = kFALSE;
	fType = 0;
	fSubtype = 0;
	fTrigger = 0;
	fEventNumber = 0;
	fNofEvents = 0;
	fScaleDown = 0;

	fClockRes = 0;
	fClockSecs = 0;
	fClockNsecs = 0;
	fTimeRS = 0;

	fLofHBXs.Delete();
	fLofSubevents.Delete();
}

const UShort_t * TUsrEvent::NextSubevent(MBSDataIO * BaseAddr, Int_t NofWords, Bool_t RawMode) const {
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

UInt_t TUsrEvent::NextSubeventHeader(MBSDataIO * BaseAddr) const {
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

const UShort_t * TUsrEvent::NextSubeventData(MBSDataIO * BaseAddr, Int_t NofWords, Bool_t RawMode) const {
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

Int_t TUsrEvent::CalcTimeDiff(TUsrEvent * BaseEvent) const {
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

	time_t cls = fClockSecs;
	et = localtime((time_t *) &cls);					// convert to time struct
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

Int_t TUsrEvent::FillSevtFromHB(TUsrHBX * HBX, Int_t Hidx, Bool_t FillHisto, Int_t Didx) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvent::FillSevtFromHB
// Purpose:        Fill subevent from hitbuffer
// Arguments:      TUsrHBX * HBX     -- pointer to hit buffer
//                 Int_t Hidx        -- current index in hit buffer
//                 Bool_t FillHisto  -- kTRUE -> write hit data to histogram
//                 Int_t Didx        -- data index within hit
// Results:        Int_t NextIndex   -- index to be used in next call, -1 at end
// Exceptions:
// Description:    Writes hit data to subevent storage. Stops at end of an event.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t nofHits = HBX->GetNofHits();
	if (nofHits > 0 && Hidx < nofHits) {
		TUsrHit * h = HBX->At(Hidx);
		Int_t evtNo = h->GetEventNumber();
		Bool_t foundHit = kFALSE;
		for (Int_t hidx = Hidx; hidx < nofHits; hidx++) {
			h = HBX->At(hidx);
			if (h->GetEventNumber() != evtNo) return(foundHit ? hidx : -1);
			h->WriteToSevtData(Didx);
			if (FillHisto) h->FillHistogram(Didx);
			foundHit = kTRUE;
		}
		return(foundHit ? nofHits : -1);
	}
	return(-1);
}

Bool_t TUsrEvent::FillEventFromHB(TArrayI & LofIndices, Bool_t FillHisto, Int_t Didx, Int_t InitValue) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvent::FillEventFromHB
// Purpose:        Fill event from hitbuffer
// Arguments:      TArrayI & LofIndices   -- array of hitbuffer indices,
//                                           has to have size=0 on start
//                 Bool_t FillHisto       -- kTRUE -> write hit data to histogram
//                 Int_t Didx             -- data index within hit
//                 Int_t InitValue        -- init value to reset event vector
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Loops thru hitbuffers and fills subvent storage event by event.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t nofHbx = fLofHBXs.GetEntriesFast();

	if (LofIndices.GetSize() == 0) {
		LofIndices.Set(nofHbx);
		LofIndices.Reset(0);
	}
	Bool_t foundHbx = kFALSE;
	this->Reset(InitValue, kTRUE);
	for (Int_t hbx = 0; hbx < nofHbx; hbx++) {
		TUsrHBX * h = this->GetHBX(hbx);
		Int_t hidx = LofIndices[hbx];
		if (h && hidx != -1) {
			hidx = this->FillSevtFromHB(h, hidx, FillHisto, Didx);
			LofIndices[hbx] = hidx;
			foundHbx = kTRUE;
		}
	}
	return(foundHbx);
}

Int_t TUsrEvent::FillArrayFromHB(TUsrHBX * HBX, ULong64_t & TimeStamp, TArrayI & Data, Int_t Hidx, Int_t Didx, Int_t InitValue) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvent::FillArrayFromHB
// Purpose:        Fill array from hitbuffer
// Arguments:      TUsrHBX * HBX          -- pointer to hit buffer
//                 Int_t Hidx             -- current index in hit buffer
//                 Int_t Didx             -- data index within hit
//                 Int_t InitValue        -- init value for reset
// Results:        ULong64_t & TimeStamp  -- time stamp
//                 TArrayI & Data         -- data
//                 Int_t NextIndex   -- index to be used in next call, -1 at end
// Exceptions:
// Description:    Get next event from hit buffer.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t nofHits = HBX->GetNofHits();				// number of hits in hitbuffer
	if (nofHits > 0 && Hidx < nofHits) {
		Data.Reset(InitValue);						// reset data vector
		TUsrHit * hit = HBX->At(Hidx);				// inspect head of hitlist
		Int_t evtNo = hit->GetEventNumber();		// extract event number
		TimeStamp = hit->GetChannelTime();			// and timestamp
		Bool_t foundHit = kFALSE;
		for (Int_t hidx = Hidx; hidx < nofHits; hidx++) {		// loop over hits as long as event number doesn't change
			hit = HBX->At(hidx);
			if (hit->GetEventNumber() != evtNo) return(foundHit ? hidx : -1);		// return start index of next event
			Data[hit->GetChannel()] = hit->GetData(Didx);							// fill vector with data for given channel
			foundHit = kTRUE;
		}
		return(foundHit ? nofHits : -1);			// end of hitbuffer
	}
	return(-1);		// no more data
}

void TUsrEvent::Print(const Char_t * Text, UInt_t TimeStamp) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvent::Print
// Purpose:        Output time stamp
// Arguments:      Char_t * Text     -- text to be printed
//                 UInt_t TimeStamp  -- time stamp
// Results:
// Exceptions:
// Description:    Outputs a time stamp message
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (TimeStamp == 0) TimeStamp = fClockSecs;
	if (TimeStamp > 0) {
		TDatime d;
		d.Set((UInt_t) TimeStamp, kFALSE);
		gMrbLog->Out() << Text << " at " << d.AsString() << endl;
		gMrbLog->Flush(this->ClassName(), "Print", setblue);
	} else {
		gMrbLog->Wrn() << Text << ": No time stamp given" << endl;
		gMrbLog->Flush(this->ClassName(), "Print");
	}
}


//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvent.cxx
// Purpose:        Implement class methods for user's analysis
// Description:    Base class for user-defined events
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TUsrEvent.cxx,v 1.1 2005-11-23 11:51:53 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include "TMrbAnalyze.h"
#include "TUsrEvent.h"
#include "mbsio_protos.h"

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

	fLofHBXs.Delete();
	fLofSubevents.Delete();
}

const UShort_t * TUsrEvent::NextSubevent(const MBSDataIO * BaseAddr, Int_t NofWords, Bool_t RawMode) const {
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

UInt_t TUsrEvent::NextSubeventHeader(const MBSDataIO * BaseAddr) const {
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

const UShort_t * TUsrEvent::NextSubeventData(const MBSDataIO * BaseAddr, Int_t NofWords, Bool_t RawMode) const {
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

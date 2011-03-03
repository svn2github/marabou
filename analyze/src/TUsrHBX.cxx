//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHBX.cxx
// Purpose:        Implement class methods for user's analysis
// Description:    Wrapper around TUsrHitBuffer to access elements
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TUsrHBX.cxx,v 1.2 2011-03-03 12:59:47 Marabou Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include "TMrbAnalyze.h"
#include "TUsrHBX.h"

extern TMrbAnalyze * gMrbAnalyze;

ClassImp(TUsrHBX)

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
	fCurIndex = 0;
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

TUsrHit * TUsrHBX::FindHit(Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHBX::FindHit
// Purpose:        Search for next hit with given channel number
// Arguments:      Int_t Channel       -- channel
// Results:        TUsrHit * Hit       -- next hit matching channel number
// Exceptions:
// Description:    Searches for next hit with given channel number.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t curIndex = fResetDone ? -1 : fCurIndex;
	while (++curIndex < this->GetNofHits()) {
		TUsrHit * hit = (TUsrHit *) fHits->At(curIndex);
		if (hit->GetChannel() == Channel) {
			fCurIndex = curIndex;
			fResetDone = kFALSE;
			return(hit);
		}
	}
	return(NULL);
}

Int_t TUsrHBX::FindHitIndex(Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHBX::FindHitIndex
// Purpose:        Search for next hit with given channel number
// Arguments:      Int_t Channel       -- channel
// Results:        Int_t HitIndex      -- index of next hit matching channel number
// Exceptions:
// Description:    Searches for next hit with given channel number.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t curIndex = fResetDone ? -1 : fCurIndex;
	while (++curIndex < this->GetNofHits()) {
		TUsrHit * hit = (TUsrHit *) fHits->At(curIndex);
		if (hit->GetChannel() == Channel) {
			fCurIndex = curIndex;
			fResetDone = kFALSE;
			return(curIndex);
		}
	}
	return(-1);
}

TUsrHit * TUsrHBX::FindHit(Int_t ModuleIndex, Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHBX::FindHit
// Purpose:        Search for next hit with given module/channel number
// Arguments:      Int_t ModuleIndex   -- module index
//                 Int_t Channel       -- channel (= -1 -> don't care)
// Results:        TUsrHit * Hit       -- next hit matching module/channel number
// Exceptions:
// Description:    Searches for next hit with given module/channel number.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t curIndex = fResetDone ? -1 : fCurIndex;
	while (++curIndex < this->GetNofHits()) {
		TUsrHit * hit = (TUsrHit *) fHits->At(curIndex);
		if (hit->GetModuleNumber() == ModuleIndex) {
			if (Channel == -1 || hit->GetChannel() == Channel) {
				fCurIndex = curIndex;
				fResetDone = kFALSE;
				return(hit);
			}
		}
	}
	return(NULL);
}

Int_t TUsrHBX::FindHitIndex(Int_t ModuleIndex, Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHBX::FindHitIndex
// Purpose:        Search for next with given channel number
// Arguments:      Int_t ModuleIndex   -- module index
//                 Int_t Channel       -- channel (= -1 --> don't care)
// Results:        Int_t HitIndex      -- index of next hit matching module/channel number
// Exceptions:
// Description:    Searches for next hit with given module/channel number.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t curIndex = fResetDone ? -1 : fCurIndex;
	while (++curIndex < this->GetNofHits()) {
		TUsrHit * hit = (TUsrHit *) fHits->At(curIndex);
		if (hit->GetModuleNumber() == ModuleIndex) {
			if (Channel == -1 || hit->GetChannel() == Channel) {
				fCurIndex = curIndex;
				fResetDone = kFALSE;
				return(curIndex);
			}
		}
	}
	return(-1);
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
//                 Takes 1st event after reset.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TUsrHit * hit;
	Int_t evtNo;

	Int_t nofHits = this->GetNofHits();
	if (nofHits && fCurIndex < nofHits) {
		if (fResetDone) evtNo = -1;
		else			evtNo = ((TUsrHit *) fHits->At(fCurIndex))->GetEventNumber();
		fResetDone = kFALSE;
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

Bool_t TUsrHBX::HitInWindow(TUsrHit * Hit0) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHBX::HitInWindow
// Purpose:        Check if hit within time window
// Arguments:      TUsrHit * Hit0      -- hit with time = 0
// Results:        kTRUE/kFALSE
// Description:    Checks if current hit within time window.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TUsrHit * hit;

	if (fCurIndex >= this->GetNofHits()) return(kFALSE);

	hit = (TUsrHit *) fHits->At(fCurIndex);
	Long64_t tDiff =	(hit->GetChannelTime() - gMrbAnalyze->GetTimeOffset(hit->GetModuleNumber()))
						- (Hit0->GetChannelTime() - gMrbAnalyze->GetTimeOffset(Hit0->GetModuleNumber()));
	if (tDiff < 0) tDiff = -tDiff;
	return(tDiff <= (Long64_t) fWindow);
}		

TUsrHit * TUsrHBX::NextHit() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHBX::NextHit
// Purpose:        Return next hit
// Arguments:      --
// Results:        TUsrHit * Hit    -- next hit or NULL
// Description:    Returns next hit. Returns 1st hit after reset.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TUsrHit * hit;
	if (fResetDone) hit = (this->GetNofHits() > 0) ? (TUsrHit *) fHits->At(0) : NULL;
	else			hit = (++fCurIndex < this->GetNofHits()) ? (TUsrHit *) fHits->At(fCurIndex) : NULL;
	fResetDone = kFALSE;
	return(hit);
}

TUsrHit * TUsrHBX::CurHit() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHBX::CurHit
// Purpose:        Return current hit
// Arguments:      --
// Results:        TUsrHit * Hit    -- current hit or NULL
// Description:    Returns current hit.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TUsrHit * hit = (fCurIndex < this->GetNofHits()) ? (TUsrHit *) fHits->At(fCurIndex) : NULL;
	fResetDone = kFALSE;
	return(hit);
}


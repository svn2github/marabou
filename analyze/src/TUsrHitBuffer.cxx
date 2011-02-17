//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHitBuffer.cxx
// Purpose:        Implement class methods for user's analysis
// Description:    Maintain a buffer of hits
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TUsrHitBuffer.cxx,v 1.2 2011-02-17 12:43:35 Marabou Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include "TMrbAnalyze.h"
#include "TMrbLogger.h"
#include "TUsrHitBuffer.h"

extern TMrbAnalyze * gMrbAnalyze;
extern TMrbLogger * gMrbLog;

ClassImp(TUsrHitBuffer)

Int_t TUsrHitBuffer::AllocClonesArray(const Char_t * HitObj, Int_t NofEntries, Int_t HighWater) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           AllocClonesArray
// Purpose:        Allocate TClonesArray
// Arguments:      Char_t * HitObj       -- name of hit to be cloned
//                 Int_t NofEntries      -- number of entries
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
	fHits = new TClonesArray(HitObj, NofEntries);
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

Bool_t  TUsrHitBuffer::AddHit(const TUsrHit * Hit) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHitBuffer::AddHit
// Purpose:        Add a hist to hit buffer
// Arguments:      TUsrHit * Hit 
// Results:        true / false
// Exceptions:
// Description:    Adds a new item to hit list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fNofHits >= fNofEntries) {
		gMrbLog->Err()	<< "[" << this->GetName() << "] Buffer overflow - " << fNofEntries << " entries max" << endl;
		gMrbLog->Flush(this->ClassName(), "AddHit");
		return(kFALSE);
	}
	TClonesArray &hits = * fHits;
	TUsrHit * hit = new(hits[fNofHits]) TUsrHit();
	*hit = *Hit;
 	fNofHits++;
	return kTRUE;
}

TUsrHit * TUsrHitBuffer::GetHitAt(Int_t Index) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHitBuffer::AddHit
// Purpose:        Add a hist to hit buffer
// Arguments:      TUsrHit * Hit
//                 Int_t Index   -- where to add the hit
// Results:        true / false
// Exceptions:
// Description:    Adds a new item to hit list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Index >= fNofEntries) {
		gMrbLog->Err()	<< "[" << this->GetName() << "] Index out of range - " << Index
						<< "(" << fNofEntries << " entries max)" << endl;
		gMrbLog->Flush(this->ClassName(), "GetHitAt");
		return(NULL);
	}
	return (TUsrHit *) fHits->At(Index);
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

Bool_t TUsrHitBuffer::IsHighWater(Bool_t Verbose) const {
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

void TUsrHitBuffer::Print(ostream & Out, Int_t Begin, Int_t End, Bool_t EnergyLongFlag) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHitBuffer::Print
// Purpose:        Print data
// Arguments:      ostream & Out          -- output stream
//                 Int_t Begin            -- entry to start with
//                 Int_t End              -- entry to end with
//                 Bool_t EnergyLongFlag  -- kTRUE -> energy data is 32bit
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
		if (hit) hit->Print(Out, kTRUE, kTRUE, kTRUE); else Out << " ... [empty slot]" << endl;
	}
}


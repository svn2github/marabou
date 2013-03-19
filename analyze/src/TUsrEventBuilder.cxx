//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEventBuilder.cxx
// Purpose:        Implement class methods for a user-specific event builder
// Description:    A user-defined event builder
// Glossary        adc event       -- adc channel data accumulated with same gate
//                 physical event  -- data from any adc channel having time stamp within time diff 
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id$
// Date:
//////////////////////////////////////////////////////////////////////////////

#include "TMrbAnalyze.h"
#include "TUsrEventBuilder.h"
#include "SetColor.h"

extern TMrbLogger * gMrbLog;					// message logger
extern TMrbAnalyze * gMrbAnalyze;

ClassImp(TUsrEventBuilder)

void TUsrEventBuilder::CollectEventHeads() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEventBuilder::CollectEventHeads
// Purpose:        Collect event heads from hitbuffers
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Loops thru hitbuffers and stores first hits of all adc events
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fEventHeads.Clear();			// clear list of event heads
	for (Int_t hbxIdx = fFirstHbx; hbxIdx <= fNofHbx; hbxIdx++) {
		TUsrHBX * hbx = fEvent->GetHBX(hbxIdx);
		Int_t nofHits = hbx->GetNofHits();
		if (nofHits > 0) {
			Int_t evtNo = -1;
			for (Int_t hIdx = 0; hIdx < nofHits; hIdx++) {			// loop thru hitbuffer
				TUsrHit * hit = hbx->At(hIdx);
				Int_t e = hit->GetEventNumber();			// event number
				if (e <= evtNo) continue;				// next event?
				hit->SetData(hbxIdx, kHitHbxIndex);			// store buffer index
				hit->SetData(hIdx, kHitIndexWithinBuffer);		// and hit index within buffer
				fEventHeads.Add(hit);					// then add event head to list
				evtNo = e;						// continue with next event
			}
		}
	}
	fEventHeads.Sort();				// sort hits with time
	fEventIter = fEventHeads.MakeIterator();	// pointer to first entry
}

Bool_t TUsrEventBuilder::NextEvent() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:		TUsrEventBuilder::NextEvent
// Purpose:             Collects hits belonging to one event
// Arguments:           --
// Results:             kTRUE/kFALSE
// Exceptions:     
// Description:         Collects hit from all hitbuffers
//
// Keywords:	
//////////////////////////////////////////////////////////////////////////////

	ULong64_t tstamp = 0;
	fHitList.Clear();
	if (fDataArray) fDataArray->Reset(fResetValue); else fEvent->Reset(fResetValue, kTRUE);
	TUsrHit * hit;
	while ((hit = (TUsrHit *) fEventIter->Next())) {			// loop thru list of event heads
		ULong64_t ts = hit->GetChannelTime() - gMrbAnalyze->GetTimeOffset(hit->GetModuleNumber());
		if (tstamp == 0) {
			tstamp = ts;				// time stamp marks beginning of event
			fTimeStampLow = ts;
			fTimeStampHigh = ts;
		}
		if (ts > fTimeStampHigh) fTimeStampHigh = ts;
		
		Int_t tdiff = (Int_t) (ts - tstamp);	// calc time diff
		if ((abs(tdiff) < fMaxTimeDiff)) {
			this->FillVector(hit);				// hit belongs to this event: fill channel data into subevent vector
		} else {
			return kTRUE;					// start over with next event
		}
	}
	return kFALSE;			// all done
}

void TUsrEventBuilder::FillVector(TUsrHit * Head) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:		TUsrEventBuilder::FillVector
// Purpose:             Fills vector with hit data
// Arguments:		TUsrHit * Head      -- pointer to head of event
// Results:             --
// Exceptions:     
// Description:         Fills subevent vector with adc data belonging to one physical event
//
// Keywords:	
//////////////////////////////////////////////////////////////////////////////

	Int_t hbxIdx = Head->GetData(kHitHbxIndex);		// index in list of hitbuffers
	TUsrHBX * hbx = fEvent->GetHBX(hbxIdx);			// get hitbuffer
	Int_t nofHits = hbx->GetNofHits();			// and number of hits
	Int_t evtNo = Head->GetEventNumber();			// and event number
	Int_t hIdx = Head->GetData(kHitIndexWithinBuffer);	// and position of hit within adc event
	while (hIdx < nofHits) {
		TUsrHit * h = hbx->At(hIdx);			// get next hit as long as event number is same
		if (h->GetEventNumber() > evtNo) return;	// end of adc event, start over with next one
		if (fDataArray) {
			fDataArray->SetAt((Double_t) h->GetData(fDataIndex), (h->GetModuleNumber() - 1) * fNofChannels + h->GetChannel());
		} else {
			Int_t * p = (Int_t *) gMrbAnalyze->GetParamAddr(h->GetModuleNumber(), h->GetChannel());	// pointer to subevent data
			*p = h->GetData(fDataIndex);			// store (energy) value
		}
		fHitList.Add(h);						// add hit to list
		hIdx++;									// continue with next hit in buffer
	}
	return;		// all done
}



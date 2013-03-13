#ifndef __TUsrEventBuilder_h__
#define __TUsrEventBuilder_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEventBuilder.h
// Purpose:        Class to describe an event builder
// Description:
// Author:         R. Lutter
// Revision:       $Id$
// Date:
// URL:
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TObjArray.h"
#include "TIterator.h"
#include "TEnv.h"

#include "TUsrHit.h"
#include "TUsrHBX.h"
#include "TUsrEvent.h"

enum EMrbSpecialHitIndices	{	kHitHbxIndex		=	2,
								kHitIndexWithinBuffer	=	3
							};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEventBuilder
// Purpose:        A user-specific event builder
// Description:    Defines methods to collect physical events from hitbuffer data
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TUsrEventBuilder : public TObject {

	public:
		TUsrEventBuilder() {};
		TUsrEventBuilder(TUsrEvent * Event, Int_t NofHbx, Int_t FirstHbx = 1, Int_t MaxTimeDiff = 0, Int_t ResetValue = 0, Int_t DataIndex = TUsrHit::kHitEnergy) {
			this->SetEvent(Event);
			this->SetNofHbx(NofHbx, FirstHbx);
			if (MaxTimeDiff == 0) MaxTimeDiff = gEnv->GetValue("TMrbAnalyze.MaxTimeDiff", 10);
			this->SetMaxTimeDiff(MaxTimeDiff);
			this->SetResetValue(ResetValue);
			this->SetDataIndex(DataIndex);
			fEventHeads.Clear();
			fTimeStampLow = 0;
			fTimeStampHigh = 0;
			fDataArray = NULL;
			fNofChannels = 32;
		}
		~TUsrEventBuilder() {};

		void CollectEventHeads();
		inline void SetEvent(TUsrEvent * Event) { fEvent = Event; };
		inline void SetNofHbx(Int_t NofHbx, Int_t FirstHbx = 1) { fNofHbx = NofHbx; fFirstHbx = FirstHbx; };
		inline void SetMaxTimeDiff(Int_t MaxTimeDiff) { fMaxTimeDiff = MaxTimeDiff; };
		inline void SetResetValue(Int_t ResetValue) { fResetValue = ResetValue; };
		inline void SetDataIndex(Int_t DataIndex) { fDataIndex = DataIndex; };
		
		inline ULong64_t GetTimeStampLow() { return fTimeStampLow; };
		inline ULong64_t GetTimeStampHigh() { return fTimeStampHigh; };
		
		inline void UseArray(TArrayI * DataArray, Int_t NofChannels = 32) { fDataArray = DataArray; fNofChannels = NofChannels; };
		
		inline TObjArray * GetHitList() { return &fHitList; };

		Bool_t NextEvent();

	protected:
		void FillVector(TUsrHit * FirstHit);

	protected:
		TUsrEvent * fEvent;	// user's event class
		Int_t fNofHbx;		// number of hitbuffers
		Int_t fFirstHbx;	// index of first hitbuffer
		Int_t fMaxTimeDiff;	// max time diff within a physical event
		Int_t fResetValue;	// reset value
		Int_t fDataIndex;	// hit data index
		
		ULong64_t fTimeStampLow;	// time stamp, low value
		ULong64_t fTimeStampHigh;	// ... high value

		TObjArray fEventHeads;		// list of hits at beginning of adc events
		TIterator * fEventIter;		// iterator to step thru list of event heads
		
		TObjArray fHitList;			// list of hits belonging to one physical event
		
		TArrayI * fDataArray;		// array to store hit data, use subevent storage if = NULL
		Int_t fNofChannels;			// number of channels per adc

	ClassDef(TUsrEventBuilder, 1)	// [Analyze] A user-specific event builder
};

#endif

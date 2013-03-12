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
		}
		~TUsrEventBuilder() {};

		void CollectEventHeads();
		inline void SetEvent(TUsrEvent * Event) { fEvent = Event; };
		inline void SetNofHbx(Int_t NofHbx, Int_t FirstHbx = 1) { fNofHbx = NofHbx; fFirstHbx = FirstHbx; };
		inline void SetMaxTimeDiff(Int_t MaxTimeDiff) { fMaxTimeDiff = MaxTimeDiff; };
		inline void SetResetValue(Int_t ResetValue) { fResetValue = ResetValue; };
		inline void SetDataIndex(Int_t DataIndex) { fDataIndex = DataIndex; };

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

		TList fEventHeads;	// list of hits at beginning of adc events
		TIterator * fEventIter;	// iterator to step thru list of event heads

	ClassDef(TUsrEventBuilder, 1)	// [Analyze] A user-specific event builder
};

#endif

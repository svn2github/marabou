#ifndef __TUsrHBX_h__
#define __TUsrHBX_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHBX.h
// Purpose:        Class to implement an index for a hit buffer
// Description:
// Author:         R. Lutter
// Revision:       $Id: TUsrHBX.h,v 1.1 2005-11-23 11:51:53 Rudolf.Lutter Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TClonesArray.h"
#include "TUsrHit.h"
#include "TUsrHitBuffer.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHBX
// Purpose:        A wrapper to access hit buffer elements
// Description:    Provides means to access hit buffer data
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TUsrHBX : public TObject {

	public:
		TUsrHBX(TObject * Event, TUsrHitBuffer * HitBuffer, Int_t Window = 0);		// ctor
		~TUsrHBX() {};										// default dtor

		inline const Char_t * GetName() const { return(fHitBuffer->GetName()); };

		inline Int_t GetNofEntries() const { return(fHitBuffer->GetNofEntries()); };
		inline Int_t GetNofHits() const { return(fHitBuffer->GetNofHits()); };
		
		inline void SetWindow(Int_t Window) { fWindow = Window; };
		inline Int_t GetWindow() const { return(fWindow); };

		inline void SetIndex(Int_t Index) { fCurIndex = Index; };
		inline Int_t GetIndex() const { return(fCurIndex); };
		inline void ResetIndex() { fCurIndex = 0; fResetDone = kTRUE; };

		TUsrHit * FindHit(TUsrHit & HitProfile) ;								// search for next hit with given profile
		TUsrHit * FindHit(Int_t Channel);										// ... with given channel
		TUsrHit * FindHit(Int_t ModuleIndex, Int_t Channel);					// ... with given module/channel

		TUsrHit * At(Int_t Index) const { return ((TUsrHit *) fHits->At(Index)); }; 	// return hit at given index
		
		TUsrHit * FindEvent(Int_t EventNumber); 							// find an event
		TUsrHit * FindNextEvent();											// find first hit of next event

		inline Bool_t AtEnd() const { return(fCurIndex >= this->GetNofHits()); }; // kTRUE if at end of buffer

		Bool_t HitInWindow(TUsrHit * Hit0) const; 								// check if hit in time window

		TUsrHit * NextHit();												// get next hit
		TUsrHit * CurHit(); 												// get current hit

		inline TUsrHitBuffer * GetHitBuffer() const { return(fHitBuffer); };

		inline void Sort(Int_t UpTo = kMaxInt) { fHits->Sort(UpTo); };		// sort entries by time
		
	protected:
		TObject * fEvent;						// event / trigger
		Bool_t fResetDone;						// kTRUE after ResetIndex()
		Int_t fCurIndex;						// current index in buffer
		Int_t fWindow;							// time stamp window
		TUsrHitBuffer * fHitBuffer; 			//! hit buffer address
		TClonesArray * fHits;					//! access to hit array
			
	ClassDef(TUsrHBX, 1)				// [Analyze] Hit buffer index
};
	
#endif

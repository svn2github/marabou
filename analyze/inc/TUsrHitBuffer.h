#ifndef __TUsrHitBuffer_h__
#define __TUsrHitBuffer_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHitBuffer.h
// Purpose:        Class to store hits in a buffer
// Description:
// Author:         R. Lutter
// Revision:       $Id: TUsrHitBuffer.h,v 1.2 2011-02-17 12:43:35 Marabou Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TUsrHit.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrHitBuffer
// Purpose:        A buffer to store hit data
// Description:    TClonesArray to store hit data.
//                 Entries are indexed by module and channel number.
//                 Sorting may be done according to time stamp.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TUsrHitBuffer : public TObject {

	public:
		TUsrHitBuffer(Int_t NofEntries = 2500) {
   			fNofEntries = NofEntries;
   			fNofHits = 0;
   			fHits = NULL;
		};

		virtual ~TUsrHitBuffer() {
			if (fHits) delete fHits;
		};										// default dtor

		inline void SetName(const Char_t * BufferName) { fBufName = BufferName; };
		inline const Char_t * GetName() const { return fBufName.Data(); };

		Int_t AllocClonesArray(const Char_t * HitObj, Int_t NofEntries, Int_t HighWater = 0);	// allocate TClonesArray to store hits
		inline Int_t AllocClonesArray(Int_t NofEntries, Int_t HighWater = 0) { return(this->AllocClonesArray("TUsrHit", NofEntries, HighWater)); };

		void Reset();												// reset hit list

		TUsrHit * AddHit(Int_t EventNumber, Int_t ModuleNumber, Int_t Channel, 			// add a new hit
										UShort_t BufferTimeHi, UShort_t EventTimeHi, UShort_t FastTrigTime,
										UShort_t * Data, Int_t NofData);
		TUsrHit * AddHit(Int_t EventNumber, Int_t ModuleNumber, Int_t Channel, 
										UShort_t * ChannelTime,
										UShort_t * Data, Int_t NofData);
		Bool_t AddHit(const TUsrHit * Hit);					  //  add (append) ready made hit

		TUsrHit * GetHitAt(Int_t Index);					// get hit at given index

		Bool_t RemoveHit(TUsrHit * Hit);					// remove hit
		Bool_t RemoveHit(Int_t Index);

		inline Int_t GetNofEntries() const { return(fNofEntries); };
		inline Int_t GetNofHits() const { return(fNofHits); };
		
		inline void SetHighWater(Int_t HighWater) { fHighWater = (HighWater >= fNofEntries) ? 0 : HighWater; };
		inline Int_t GetHighWater() const { return(fHighWater); };
		Bool_t IsHighWater(Bool_t Verbose = kFALSE) const;
				
		inline void Sort(Int_t UpTo = kMaxInt) { fHits->Sort(UpTo); };		// sort entries by time

		inline TClonesArray * GetCA() const { return(fHits); };

		void Print(Option_t * Option) const { TObject::Print(Option); };
		void Print(ostream & Out, Int_t Begin = 0, Int_t End = -1, Bool_t EnergyLongFlag = kFALSE) const;	// print data
		inline void Print(Int_t Begin = 0, Int_t End = -1, Bool_t EnergyLongFlag = kFALSE) const { Print(cout, Begin, End, EnergyLongFlag); };
		
	protected:
		TString fBufName;						// buffer name
		Int_t fNofEntries;						// max number of entries
		Int_t fNofHits; 						// current number of hits
		Int_t fOffset;							// [obsolete, for compatibility reasons only]
		Int_t fHighWater;						// high water margin
		TClonesArray * fHits;			    	// array containing hit data
			
	ClassDef(TUsrHitBuffer, 1)					// [Analyze] Hit buffer
};

#endif

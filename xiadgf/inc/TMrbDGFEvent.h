#ifndef __TMrbDGFEvent_h__
#define __TMrbDGFEvent_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           xiadgf/inc/TMrbDGFEvent.h
// Purpose:        Interface to module XIA DGF-4C
// Class:          TMrbDGFEvent       -- DGF's event structure
// Description:    Class definitions to operate the XIA DGF-4C module.
// Author:         R. Lutter
// Revision:       $Id: TMrbDGFEvent.h,v 1.2 2004-09-28 13:47:33 rudi Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TSystem.h"
#include "TArrayI.h"
#include "TArrayD.h"

#include "TMrbDGFData.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFEvent
// Purpose:        Event bookkeeping
// Methods:
// Description:    Keeps track of DGF's events.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbDGFEvent: public TObject {

	public:
		TMrbDGFEvent() {				// default ctor
			fHitPattern = 0;
			fNofChannels = 0;
			fChannel.Set(TMrbDGFData::kNofChannels);
			fChannel.Reset();
			fTau.Set(TMrbDGFData::kNofChannels);
			fTau.Reset();
		};

		~TMrbDGFEvent() {}; 			// default dtor

		inline void SetHitPattern(UInt_t Pattern) { fHitPattern = Pattern; };
		inline void SetNofChannels(Int_t NofChannels) { fNofChannels = NofChannels; };
		inline void SetChannel(Int_t Channel, Int_t Index) { fChannel[Channel] = Index; };
		inline void SetTau(Int_t Channel, Double_t Tau) { fTau[Channel] = Tau; };
		inline UInt_t GetHitPattern() { return(fHitPattern); };
		inline Int_t GetNofChannels() { return(fNofChannels); };
		inline Int_t GetChannel(Int_t Channel) { return(fChannel[Channel]); };
		inline Double_t GetTau(Int_t Channel) { return(fTau[Channel]); };

	protected:
		UInt_t fHitPattern;				// hit pattern
		Int_t fNofChannels; 			// number of active channels
		TArrayI fChannel;				// addr of channel data
		TArrayD fTau;					// calculated tau value

	ClassDef(TMrbDGFEvent, 1)		// [XIA DGF-4C] DGF's event structure
};

#endif

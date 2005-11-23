#ifndef __TMrbModuleListEntry_h__
#define __TMrbModuleListEntry_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModuleListEntry.h
// Purpose:        Class to manage a list of modules
// Description:
// Author:         R. Lutter
// Revision:       $Id: TMrbModuleListEntry.h,v 1.1 2005-11-23 11:51:53 Rudolf.Lutter Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TArrayI.h"
#include "TMrbNamedX.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModuleListEntry
// Purpose:        An entry in user's module list
// Description:    Bookkeeping: Connects modules to params & histos
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TMrbModuleListEntry : public TObject {

	public:
		TMrbModuleListEntry(	Int_t NofParams = 0,						// ctor
								Int_t IndexOfFirstParam = 0,
								Int_t TimeOffset = 0,
								TMrbNamedX * FirstParam = NULL,
								TMrbNamedX * FirstHisto = NULL,
								TMrbNamedX * FirstSingle = NULL) :
										fNofParams(NofParams),
										fIndexOfFirstParam(IndexOfFirstParam),
										fTimeOffset(TimeOffset),
										fFirstParam(FirstParam),
										fFirstHisto(FirstHisto),
										fFirstSingle(FirstSingle) {
											fNofEventHits = 0;
											fNofChannelHits.Set(NofParams);
											fNofChannelHits.Reset();
										};

		virtual ~TMrbModuleListEntry() {}; 									// dtor

		inline Int_t GetNofParams() const { return(fNofParams); };
		inline void SetNofParams(Int_t NofParams) { fNofParams = NofParams; };
		inline Int_t GetTimeOffset() const { return(fTimeOffset); };
		inline void SetTimeOffset(Int_t TimeOffset) { fTimeOffset = TimeOffset; };
		inline Int_t GetIndexOfFirstParam() const { return(fIndexOfFirstParam); };
		inline void SetIndexOfFirstParam(Int_t FirstParam) { fIndexOfFirstParam = FirstParam; };
		inline TMrbNamedX * GetFirstParam() const { return(fFirstParam); };
		inline void SetFirstParam(TMrbNamedX * FirstParam) { fFirstParam = FirstParam; };
		inline TMrbNamedX * GetFirstHisto(Bool_t SingleFlag = kFALSE) const {
			return(SingleFlag ? fFirstSingle : fFirstHisto);
		};
		inline void SetFirstHisto(TMrbNamedX * FirstHisto, Bool_t SingleFlag = kFALSE) {
			if (SingleFlag) 	fFirstSingle = FirstHisto;
			else				fFirstHisto = FirstHisto;
		};

		void ResetChannelHits(Int_t Channel = -1);
		inline void ResetEventHits() { fNofEventHits = 0; };
		inline void ResetAllHits() { fNofEventHits = 0; fNofChannelHits.Reset(); };
		inline void IncrEventHits() { fNofEventHits++; };
		void IncrChannelHits(Int_t Channel);
		inline Int_t GetEventHits() { return(fNofEventHits); };
		Int_t GetChannelHits(Int_t Channel);

	protected:
		Int_t fNofParams;
		Int_t fIndexOfFirstParam;
		Int_t fTimeOffset;
		Int_t fNofEventHits;
		TArrayI fNofChannelHits;
		TMrbNamedX * fFirstParam;
		TMrbNamedX * fFirstHisto;
		TMrbNamedX * fFirstSingle;

	ClassDef(TMrbModuleListEntry, 0) 	// [Analyze] List of modules
};

#endif

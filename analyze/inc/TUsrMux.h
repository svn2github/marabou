#ifndef __TUsrMux_h__
#define __TUsrMux_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrMux.h
// Purpose:        Class to describe data structure of Mesytec Mux16 multiplexer
// Description:
// Author:         R. Lutter
// Revision:       $Id: TUsrMux.h,v 1.3 2007-10-25 17:24:12 Marabou Exp $       
// Date:           $Date: 2007-10-25 17:24:12 $
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <iostream>

#include "TObject.h"
#include "TObjArray.h"
#include "TString.h"
#include "TEnv.h"
#include "TH1F.h"

#include "TUsrHit.h"
#include "TUsrHitBuffer.h"
#include "TUsrHBX.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrMuxLookup
// Purpose:        Defines a lookup table
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TUsrMuxLookup : public TNamed, public TArrayI {

	friend class TUsrMux;

	public:
		TUsrMuxLookup(const Char_t * MuxName, Int_t Size) : TNamed(MuxName, MuxName) {
			fGoodHits = 0;
			fBadHits = 0;
			fHitRatio = 0;
			this->Set(Size);
			this->Reset(0);
		};

		inline void CountHit(Int_t Channel) {
			if (Channel > 0) {
				fGoodHits = (fGoodHits * 999 + 1) / 1000;
			} else {
				fBadHits = (fBadHits * 999 + 1) / 1000;
			}
		}

		inline Int_t GetEntries() {
			Int_t nofEntries = 0;
			for (Int_t i = 0; i < this->GetSize(); i++) {
				if (this->At(i) != 0) nofEntries++;
			}
			return(nofEntries);
		}

		inline Double_t CalculateRatio() { if (fGoodHits > 0) fHitRatio = fBadHits / fGoodHits; return(fHitRatio); };

	protected:		
		Double_t fGoodHits; 		// count good hits (central half of window)
		Double_t fBadHits;			// count bad hits (outer quarters of window)
		Double_t fHitRatio; 		// ratio bad/good
		
	ClassDef(TUsrMuxLookup, 0)				// [Analyze] Multiplexer lookup table
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrMuxEntry
// Purpose:        Define an entry in the mux channel table
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TUsrMuxEntry : public TObject {

	friend class TUsrMux;

	public:
		TUsrMuxEntry(TEnv * LkpEnv, TUsrMuxLookup * Lookup, TUsrHit * Hit = NULL) {
			fLkpEnv = LkpEnv;
			fLookup = Lookup;
			fHit = Hit;
		};

		~TUsrMuxEntry() {};

		const Char_t * PrintEntry(TString & Output);

	protected:		
		TEnv * fLkpEnv;
		TUsrMuxLookup * fLookup;
		TUsrHit * fHit;
		
	ClassDef(TUsrMuxEntry, 0)				// [Analyze] Multiplexer entry
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrMux
// Purpose:        Class to store hit data
// Description:    Describes a single hit.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TUsrMux : public TObject {

	public:
		TUsrMux() {};
		TUsrMux(const Char_t * ConfigFile);
		~TUsrMux() {};					// default dtor

		Bool_t Init(const Char_t * ConfigFile);
		void ClearTables();
		
		TUsrMuxLookup * MakeLookupTable(TEnv * LkpEnv);

		TUsrMuxEntry * IsMuxChannel(TUsrHit * Hit);
		Bool_t CollectHits(TUsrHBX * HbufIndex);
		void AccuSingles();
		inline void ResetIndex() { fHitIter = fLofMuxHits.MakeIterator(); };
		inline TUsrHit * NextHit() { return((TUsrHit *) fHitIter->Next()); };
		inline Int_t GetNofHits() { return(fLofMuxHits.GetEntriesFast()); };

		inline TObjArray * GetLofLookups() { return(&fLofLookups); };

		inline Bool_t IsVerbose() { return(fVerbose); };
		inline Bool_t IsDebug() { return(fDebug); };

		inline Int_t GetIndexOfFirstHisto() { return(fIndexOfFirstHisto); };
		inline Int_t GetNofHistograms() { return(fNofHistograms); };

		void PrintChanTable();
		void PrintLookup(TUsrMuxLookup * Lookup);
		void PrintLofMuxHits();

	protected:		
		TEnv * fConfig;
		Bool_t fVerbose;
		Bool_t fDebug;					// debug flag
		TString fDebugLock; 			// lock file for debug
		Int_t fSmin;					// min serial number
		Int_t fSmax;					// max serial number
		Int_t fChansPerModule;			// number of mux pchannels per adc
		Int_t fModuleRange; 			// number of channels per adc
		Int_t fIndexOfFirstHisto; 		// index of first mux histo in histo list
		Int_t fNofHistograms; 			// number of histograms
		TIterator * fHitIter;			// hit iterator
		TObjArray fChanTable;			// describes where mux channels are located in the adcs
		TObjArray fLofMuxEntries;  		// collects mux channels (TUsrMuxEntry) of current event
		TObjArray fLofMuxHits; 			// collects mux hits (TUsrHit) of current event
		TObjArray fLofLookups; 			// list of lookups generated (TUsrMuxLookup)
		
	ClassDef(TUsrMux, 1)				// [Analyze] Mesytec Mux16 Multiplexer
};

#endif

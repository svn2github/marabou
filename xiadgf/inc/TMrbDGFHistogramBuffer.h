#ifndef __TMrbDGFHistogramBuffer_h__
#define __TMrbDGFHistogramBuffer_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           xiadgf/inc/TMrbDGFHistogramBuffer.h
// Purpose:        Interface to module XIA DGF-4C
// Classes:        TMrbDGFHistogrambuffer    -- buffer structure for histograms
// Description:    Class definitions to operate the XIA DGF-4C module.
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TNamed.h"
#include "TSystem.h"
#include "TArrayI.h"
#include "TH1.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFHistogramBuffer
// Purpose:        DGF's histogram buffer
// Methods:
// Description:    Describes the histogram buffer.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbDGFHistogramBuffer : public TNamed, public TArrayI {

	public:
		TMrbDGFHistogramBuffer(const Char_t * BufferName = "dgfHbuf", TObject * Module = NULL); 	// ctor
		
		~TMrbDGFHistogramBuffer() {}; 										// default dtor

		void Reset();														// reset buffer
		inline Int_t GetNofChannels() { return(fNofChannels); };			// get/set number of DGF channels
		Bool_t SetActive(Int_t Channel, Int_t HistNo);						// set channel active
		Bool_t IsActive(Int_t Channel);										// test if channel active
		inline void SetNofChannels(Int_t NofChannels) { fNofChannels = NofChannels; };
		inline Int_t GetESize() { return(fESize); };						// get/set size of energy region
		inline Int_t GetESizePerChannel() { return(fESizePerChannel); };	// ... per DGF channel
		inline void SetESize(Int_t Size) { fESize = Size; if (fNofChannels) fESizePerChannel = Size / fNofChannels; };
		inline Int_t GetBSize() { return(fBSize); };						// get/set size of baseline region
		inline Int_t GetBSizePerChannel() { return(fBSizePerChannel); };	// ... per DGF channel
		inline void SetBSize(Int_t Size) { fBSize = Size; if (fNofChannels) fBSizePerChannel = Size / fNofChannels; };

		inline void SetModule(TObject * Module) { fModule = Module; };		// connect to dgf module
		
		void Print();														// output statistics

		Bool_t FillHistogram(Int_t Channel, Bool_t DrawIt = kTRUE);			// fill & draw histogram
		Int_t GetContents(Int_t Channel);									// get channel contents

		TH1F * Histogram(Int_t Channel);									// return histo addr
		Bool_t Save(const Char_t * McaFile = "mca.root", Int_t Channel = -1);		// save histograms

	protected:
		TObject * fModule;					//! ptr to dgf module
	
		Int_t fNofChannels;					// number of DGF channels
		Int_t fESize;						// size of energy region
		Int_t fESizePerChannel; 			// ... per DGF channel
		Int_t fBSize;						// size of base line region
		Int_t fBSizePerChannel; 			// ... per DGF channel
		Bool_t fIsActive[TMrbDGFData::kNofChannels];	// channel active?
		Int_t fHistNo[TMrbDGFData::kNofChannels];		// histogram number
		TH1F * fHistogram[TMrbDGFData::kNofChannels];	// histograms

	ClassDef(TMrbDGFHistogramBuffer, 1) 	// [XIA DGF-4C] Buffer to store histogram data
};

#endif

#ifndef __TMrbDGFEventBuffer_h__
#define __TMrbDGFEventBuffer_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           xiadgf/inc/TMrbDGFEventBuffer.h
// Purpose:        Interface to module XIA DGF-4C
// Class:          TMrbDGFEventBuffer   -- DGF's event buffer
// Description:    Class definitions to operate the XIA DGF-4C module.
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TNamed.h"
#include "TSystem.h"
#include "TH1.h"
#include "TF1.h"
#include "TString.h"
#include "TArrayI.h"
#include "TObjArray.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFEventBuffer
// Purpose:        DGF's list mode buffer
// Methods:
// Description:    Describes the list mode buffer.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbDGFEventBuffer : public TNamed, public TArrayI {

	public:

		// buffer header
		enum EMrbDGFBufferHeader	{	kBufNofWords		=	0,
										kModNumber,
										kFormat,
										kRunStartTime
									};
		enum						{	kBufHeaderLength	=	6	};

		// event header
		enum EMrbDGFEventHeader 	{	kHitPattern 		=	0,
										kEventTime
									};
		enum						{	kEventHeaderLength	=	3	};

		// channel header
		enum EMrbDGFChanHeader		{	kChanNofWords		=	0,
										kFastTrigger,
										kEnergy,
										kConstFractTriggerTime
									};
		enum						{	kChanHeaderLength	=	9	};

		enum						{	kTotalHeaderLength	=	kBufHeaderLength
															+	kEventHeaderLength
															+	kChanHeaderLength
									};

	public:
		TMrbDGFEventBuffer( const Char_t * BufferName = "dgfEbuf",
							TObject * Module = NULL,
							Int_t Index = -1, Int_t Event0 = -1);		// ctor

		~TMrbDGFEventBuffer() {}; 										// default dtor

		void Print(ostream & OutStrm, Int_t EventNumber = -1, const Char_t * ModuleInfo = NULL);
		inline void Print(Int_t EventNumber = -1) { this->Print(cout, EventNumber); };
		Bool_t PrintToFile(const Char_t * FileName, const Char_t * ModuleInfo);

		void Reset();													// reset buffer
		inline Int_t GetNofWords() { return(fNofWords); };				// get number of data words
		inline Int_t GetNofEvents() { return(fNofEvents); };			// get number of events

		inline Int_t GetGlobalIndex() { return(fGlobIndex); };			// global buffer index
		inline void SetGlobalIndex(Int_t Index) { fGlobIndex = Index; };
		inline Int_t GetGlobalEventNo() { return(fGlobEventNo); };		// global event number
		inline void SetGlobalEventNo(Int_t EventNo) { fGlobEventNo = EventNo; };
				
		Int_t GetNofChannels(Int_t EventNumber);						// get number of channels
		UInt_t GetHitPattern(Int_t EventNumber); 						// get hit pattern

		Int_t GetChannel(Int_t EventNumber, Int_t Channel, TArrayI & Data);

		Int_t GetNofWords(Int_t EventNumber, Int_t Channel);						// get number of words for a given channel

		inline UInt_t * GetDAddr() { return((UInt_t *) &fArray); }; 				// addr of data region

		inline void SetTraceError(Stat_t Error) { fTraceError = Error; };			// define common trace error
		inline Stat_t GetTraceError() { return(fTraceError); };

		inline void SetModule(TObject * Module) { fModule = Module; };				// connect to dgf module
		
		TH1F * FillHistogram(Int_t EventNumber, Int_t Channel, Bool_t DrawIt = kTRUE);	// fill trace data into histo

		Double_t CalcTau(Int_t EventNumber, Int_t Channel, Int_t GlobalEventNo,
							Double_t A0, Double_t A1, Double_t A2,
							Int_t From = 0, Int_t To = 0,
							Bool_t Verbose = kTRUE, Bool_t DrawIt = kTRUE); 		// calculate tau

		Double_t GetTau(Int_t EventNumber, Int_t Channel);							// recall calculated tau value
		
		inline TH1F * Histogram() { return(fHistogram); };							// addr of internal histo
		inline TF1 * Fit() { return(fTauFit); };									// addr of internal fit

		Bool_t SaveTraces(const Char_t * TraceFile = NULL,
			Int_t EventNumber = -1, Int_t Channel = -1);							// save trace data

		void MakeIndex();															// analyze buffer data

	protected:
		TObject * fModule;					//! ptr to dgf module

		Int_t fNofWords;					// number of words in this buffer
		Int_t fNofEvents;					// number of events in this buffer
		Stat_t fTraceError;					// error to be applied to trace data
		TObjArray fEventIndex;				// event index
		TH1F * fHistogram;					//! histogram
		TF1 * fTauFit;						//! tau fit;

		Int_t fGlobIndex;					// global buffer index
		Int_t fGlobEventNo; 				// global event number
		
	ClassDef(TMrbDGFEventBuffer, 1) 		// [XIA DGF-4C] Buffer to store event data
};

#endif

#ifndef __ExaAnalyze_h__
#define __ExaAnalyze_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           ExaAnalyze.h
// Purpose:        Define user-specific classes for data analysis
// Description:    Example configuration
//
// Author:         MBS and ROOT Based Online Offline Utility
// Revision:         
// Date:           Thu Nov 28 09:12:18 2002
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include <time.h>

#include "TEnv.h"
#include "TMrbLogger.h"

#include "TMrbAnalyze.h"
#include "TMrbTransport.h"

#include "TMrbLofData.h"

#include "ExaCommonIndices.h"



//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrSevtData
// Purpose:        Define a user-specific subevent class
// Description:    camac data
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TUsrSevtData : public TObject {

// declare all parent events as friends
// so user may address subevent data directly from event level
	friend class TUsrEvtReadout;

	public:
		TUsrSevtData() {		// default ctor
			fSevtName = "data";
		};
		~TUsrSevtData() {};		// default dtor

		inline const Char_t * GetName() { return(fSevtName.Data()); };

		Bool_t FillSubevent(const UShort_t * SevtData, Int_t SevtWC, Int_t TimeStamp);	// fill subevent from MBS data
		Bool_t BookParams();					// book params
		void Reset(Int_t InitValue = 0);		// reset param data
 		Bool_t BookHistograms();				// book histograms
 		Bool_t FillHistograms();				// fill histograms
		Bool_t AddBranch(TTree * Tree);				// add a branch to the tree
		Bool_t InitializeBranch(TTree * Tree);		// init branch addresses (replay mode only)
		inline Int_t GetTimeStamp() { return(fTimeStamp); };	// return time stamp (100 microsecs since start)

		

												// get param addr (method needed to access params from outside, eg. in macros)
		inline Int_t * GetAddrEtot() { return(&etot); };	// addr of etot
		inline Int_t GetEtot() { return(etot); };	// value of etot
		inline Int_t * GetAddrDe1() { return(&de1); };	// addr of de1
		inline Int_t GetDe1() { return(de1); };	// value of de1
		inline Int_t * GetAddrDe2() { return(&de2); };	// addr of de2
		inline Int_t GetDe2() { return(de2); };	// value of de2
		inline Int_t * GetAddrDe3() { return(&de3); };	// addr of de3
		inline Int_t GetDe3() { return(de3); };	// value of de3
		inline Int_t * GetAddrDe4() { return(&de4); };	// addr of de4
		inline Int_t GetDe4() { return(de4); };	// value of de4
		inline Int_t * GetAddrVeto() { return(&veto); };	// addr of veto
		inline Int_t GetVeto() { return(veto); };	// value of veto
		inline Int_t * GetAddrPos1r() { return(&pos1r); };	// addr of pos1r
		inline Int_t GetPos1r() { return(pos1r); };	// value of pos1r
		inline Int_t * GetAddrPos1l() { return(&pos1l); };	// addr of pos1l
		inline Int_t GetPos1l() { return(pos1l); };	// value of pos1l
		inline Int_t * GetAddrPos2r() { return(&pos2r); };	// addr of pos2r
		inline Int_t GetPos2r() { return(pos2r); };	// value of pos2r
		inline Int_t * GetAddrPos2l() { return(&pos2l); };	// addr of pos2l
		inline Int_t GetPos2l() { return(pos2l); };	// value of pos2l

		inline Int_t GetIndex() { return(kMrbSevtData); };			// subevent index
		inline UInt_t GetIndexBit() { return(kMrbSevtBitData); };	// subevent index bit

	protected:
		inline void SetTimeStamp(Int_t TimeStamp) { SetUniqueID(TimeStamp); };

	protected:
		TString fSevtName;							//! subevent name
		TObject * fMyAddr;						//! ptr to subevent to be used in TTree::Branch() (online)
		TBranch * fBranch;						//! branch address (replay only)

		Int_t fTimeStamp;						// time stamp, same as fUniqueID

// define private data members here
 		Int_t etot; 				// data.etot
 		Int_t de1; 				// data.de1
 		Int_t de2; 				// data.de2
 		Int_t de3; 				// data.de3
 		Int_t de4; 				// data.de4
 		Int_t veto; 				// data.veto
 		Int_t pos1r; 				// data.pos1r
 		Int_t pos1l; 				// data.pos1l
 		Int_t pos2r; 				// data.pos2r
 		Int_t pos2l; 				// data.pos2l

		
	ClassDef(TUsrSevtData, 1)		// [Analyze] Standard methods to store subevent data
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtReadout
// Purpose:        Define a user-specific event class
// Description:    Event assigned to trigger 1
//                 readout of camac data
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TUsrEvtReadout : public TUsrEvent {

	public:
		TUsrEvtReadout() {			// ctor
			fType = 10; 					// event type (given by MBS)
			fSubtype = 1;					// ... subtype
			fTrigger = kMrbTriggerReadout; 	// trigger number associated with this event
			fNofEvents = 0; 				// reset event counter
			fNofEntries = 0;				// number of entries read from tree
			fEventNumber = 0;				// event number provided by MBS
			fReplayMode = kFALSE;			// normally online data acquisition
		};
		~TUsrEvtReadout() {};		// default dtor

		Bool_t FillEvent(const s_vehe *, const MBSDataIO *);	// event filling function supplied by system
		Bool_t BookParams();									// book params for this event
		Bool_t BookHistograms();								// book histograms for this event
		Bool_t AllocHitBuffers();								// allocate hit buffers for subevents if needed
		Bool_t CreateTree();									// create a tree if needed
		Bool_t Analyze();										// user's analyzing code
		Bool_t InitializeTree(TFile * RootFile); 				// init tree (replay mode only)
		Bool_t ReplayTreeData(TMrbIOSpec * IOSpec);				// replay tree data
		void Reset(Int_t InitValue = 0);						// reset subevent data

		inline UInt_t GetSubeventPattern() { return(fSubeventPattern); };
		inline Bool_t ThereIsSubevent(UInt_t SubeventBit) { return((fSubeventPattern & SubeventBit) != 0); };

		
	protected:
		UInt_t fSubeventPattern;
		TUsrSevtData data;


		
	ClassDef(TUsrEvtReadout, 1) 	// [Analyze] Standard methods to handle event data
};
#endif

#ifndef __TUsrEvent_h__
#define __TUsrEvent_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvent.h
// Purpose:        Class to describe a user-defined event
// Description:
// Author:         R. Lutter
// Revision:       $Id: TUsrEvent.h,v 1.3 2007-08-08 11:15:59 Rudolf.Lutter Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TSystem.h"
#include "TTree.h"
#include "TBranch.h"
#include "TObjArray.h"

#include "mbsio.h"
#include "TUsrHBX.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvent
// Purpose:        Base class for user-defined events
// Description:    Defines methods common to all user-specific events
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TUsrEvent : public TObject {

	public:
		TUsrEvent(); 								// default ctor
		~TUsrEvent() { fLofSubevents.Delete(); fLofHBXs.Delete(); };	// dtor

		const UShort_t * NextSubevent(MBSDataIO * BaseAddr, Int_t NofWords = 0, Bool_t RawMode = kFALSE) const;	// get next subevent
		UInt_t NextSubeventHeader(MBSDataIO * BaseAddr) const;
		const UShort_t * NextSubeventData(MBSDataIO * BaseAddr, Int_t NofWords = 0, Bool_t RawMode = kFALSE) const;

		inline void SetScaleDown(Int_t ScaleDown) { fScaleDown = ScaleDown; }; // set individual scale down
		inline Int_t GetScaleDown() const { return(fScaleDown); };	// get current scale down
		inline Int_t GetClockSecs() const { return(fClockSecs); };	// return event time (secs since 1-Jan-70)
		inline Int_t GetClockNsecs() const { return(fClockNsecs); };	// return event time (nanosecs of current second)
		inline Int_t GetTimeStamp() const { return(GetUniqueID()); };	// return time stamp (100 microsecs since start)
		inline Int_t GetTimeRS() const { return(fTimeRS); };			// get time (ROOT style)
		Int_t CalcTimeDiff(TUsrEvent * Event) const;					// calc time diff with respect to event
		inline TTree * GetTreeIn() const { return(fTreeIn); }; 	// return tree addr (input)
		inline TTree * GetTreeOut() const { return(fTreeOut); }; 	// ... (output)

		inline void WriteTree(const Bool_t WriteFlag = kTRUE) {
			fWriteTree = (fTreeOut == NULL) ? kFALSE : WriteFlag;
		};
		inline Bool_t TreeToBeWritten() const { return(fWriteTree); };

		inline void SetReplayMode(const Bool_t ReplayMode = kTRUE) {
			fReplayMode = (fTreeIn == NULL) ? kFALSE : ReplayMode;
		};
		inline Bool_t IsReplayMode() const { return(fReplayMode); };

		inline void SetFakeMode(const Bool_t FakeMode = kTRUE) {
			fReplayMode = fFakeMode = FakeMode;
		};
		inline Bool_t IsFakeMode() const { return(fFakeMode); };

		inline Int_t GetNofEvents() const { return(fNofEvents); };
		inline Bool_t IsModScaleDown() const { return((fNofEvents % fScaleDown) == 0); };

																					// list of subevents
		inline void AddSubevent(TObject * Subevent, Int_t SevtSerial = -1) {		// add subevent to list
				if (SevtSerial == -1)	fLofSubevents.AddLast(Subevent);
				else					fLofSubevents.AddAtAndExpand(Subevent, SevtSerial);
		};
		inline TObjArray * GetLofSubevents() { return(&fLofSubevents); };
		inline TObject * GetSubevent(Int_t SevtSerial) { return(fLofSubevents.At(SevtSerial)); };	// get it from list
		inline Int_t GetNofSubevents() const { return(fLofSubevents.GetEntries()); };

																	// list of hit buffers
		inline void AddHBX(TUsrHBX * HBX, Int_t SevtSerial = -1) {	// add hit buffer wrapper to list
				if (SevtSerial == -1)	fLofHBXs.AddLast(HBX);
				else					fLofHBXs.AddAtAndExpand(HBX, SevtSerial);
		};
		inline TUsrHBX * GetHBX(Int_t SevtSerial) { return((TUsrHBX *) fLofHBXs.At(SevtSerial)); }; // get it from list

		void Print(const Char_t * Text, UInt_t TimeStamp = 0);

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		Int_t CalcTimeRS(); 				// convert time to ROOT style
		inline void SetTimeStamp(Int_t TimeStamp) { SetUniqueID(TimeStamp); };

	protected:
		TBranch * fBranch;					//! ptr to event to be used in TTree::Branch()

		TTree * fTreeOut;					//! tree to store data in ROOT format
		TTree * fTreeIn;					//! tree to read data from ROOT file
		Bool_t fReplayMode; 				//! kTRUE if replay mode on
		Bool_t fFakeMode; 					//! kTRUE if fake mode on
		Bool_t fWriteTree;					//! kTRUE if tree data should be written to file

		Int_t fNofEntries; 					// number of entries in ROOT tree (replay mode only)

		UInt_t fType;						// event type
		UInt_t fSubtype;					// ... subtype
		Int_t fTrigger;						// trigger number
		Int_t fEventNumber;					// event number provided by MBS
		Int_t fNofEvents; 					// number of events collected so far
		Int_t fScaleDown;					// individual scale down factor
		Int_t fClockRes;					// clock resolution in nsecs
		Int_t fClockSecs; 					// seconds since January 1, 1970
		Int_t fClockNsecs;  				// nano secs of current second
		Int_t fTimeRS;						// time (ROOT style)

		TObjArray fLofHBXs; 				//! list of hit buffer wrappers, indexed by hit buffer
		TObjArray fLofSubevents;			//! list of subevents

	ClassDef(TUsrEvent, 1)		// [Analyze] User's event structure
};

#endif

#ifndef __TUsrDeadTime_h__
#define __TUsrDeadTime_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrDeadTime.h
// Purpose:        Class to store dead time values
// Description:
// Author:         R. Lutter
// Revision:       $Id: TUsrDeadTime.h,v 1.1 2005-11-23 11:51:53 Rudolf.Lutter Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <iostream>
#include <iomanip>

#include "TObject.h"
#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrDeadTime
// Purpose:        Class to store dead-time data
// Description:    Defines methods for dead-time (sub)events
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TUsrDeadTime : public TObject {

	public:
		TUsrDeadTime() {							// ctor
			fTimeStamp = 0;
			fTotalEvents = 0;
			fAccuEvents = 0;
			fScalerContents = 0;
			fDeadTime = 0;
		};

		~TUsrDeadTime() {}; 						// default dtor

		void Set(Int_t TimeStamp, Int_t TotalEvents, Int_t AccuEvents, Int_t ScalerContents);
		inline Float_t Get() const { return(fDeadTime); };			// get dead time in %

		inline Int_t GetTimeStamp() const { return(fTimeStamp); };	// return time stamp (100 microsecs since start)
		inline Int_t GetTotalEvents() const { return(fTotalEvents); }; 	// get event number

		void Print(Option_t * Option) const { TObject::Print(Option); };
		void Print();

		inline void SetReplayMode(const Bool_t ReplayMode = kTRUE) {
			fReplayMode = (fTreeIn == NULL) ? kFALSE : ReplayMode;
		};
		inline Bool_t IsReplayMode() const { return(fReplayMode); };

		Bool_t CreateTree();										// create tree to hold dead-time data
		Bool_t InitializeTree(TFile * RootFile);					// initialize dead-time tree for replay
		inline TTree * GetTreeIn() const { return(fTreeIn); };
		inline TTree * GetTreeOut() const { return(fTreeOut); };
	
		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		Int_t fTimeStamp;					// time stamp (since start)
		Int_t fTotalEvents; 				// event count
		Int_t fAccuEvents;					// accumulated events
		Int_t fScalerContents;				// contents of dead-tim scaler
		Float_t fDeadTime;					// dead time (%)

		Bool_t fReplayMode;

		TTree * fTreeIn;					//! dead time (input)
		TTree * fTreeOut;					//! ... (output)
		TObject * fBranch;					//! ptr to dead-time obj to be used in TTree::Branch()

	ClassDef(TUsrDeadTime, 1)		// [Analyze] Keep track of dead time
};

#endif

//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtStop.cxx
// Purpose:        Implement class methods for user's analysis
// Description:    Class for stop event
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TUsrEvtStop.cxx,v 1.3 2007-08-08 11:16:00 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include "TMrbAnalyze.h"
#include "TUsrEvtStop.h"
#include "SetColor.h"

extern TMrbAnalyze * gMrbAnalyze;
extern TMrbLogger * gMrbLog;

ClassImp(TUsrEvtStop)

TUsrEvtStop::TUsrEvtStop() {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtStop
// Purpose:        Create an event for "Stop Acquisition"
// Description:    Creates an event for trigger 15 (Stop)
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fType = 10; 					// event type (given by MBS)
	fSubtype = 1;					// ... subtype
	fTrigger = 15;  				// trigger number 15: stop
	fNofEvents = 0; 				// reset event counter
	fEventNumber = 0;				// event number provided by MBS
}

Bool_t TUsrEvtStop::CreateTree() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtStop::CreateTree
// Purpose:        Create a tree for event "Stop Acquisition"
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Creates a tree for event "Stop Acquisition"
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbAnalyze->TreeToBeWritten()) {
		if (gMrbAnalyze->IsVerbose()) {
			cout	<< this->ClassName() << "::CreateTree(): Creating tree \"Stop Acquisition\""
					<< endl;
		}
		fTreeOut = new TTree("Stop", "Stop Acquisition");
		fTreeOut->SetAutoSave(1000);					// automatic save after 1 kB
		fTreeOut->Branch("stop", &fClockSecs, "stop/i");
	}
	return(kTRUE);
}

Bool_t TUsrEvtStop::ExtractTimeStamp(const s_vehe * EventData, MBSDataIO * BaseAddr) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtStop::ExtractTimeStamp
// Purpose:        Get time stamp from (sub)event header
// Arguments:      const s_vehe * EventData   -- pointer to MBS event structure
//                 const MBSDataIO * BaseAddr -- pointer to MBS data base
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Decodes the event header,
//                 then looks for a subevent [9000,1] = time stamp
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t * timeStamp;				// pointer to time info
	TString timeString;					// expanded time string

	timeStamp = (Int_t *) NextSubevent(BaseAddr);
	if (BaseAddr->sevt_id == 999) {
		fClockRes = *timeStamp++;
		fClockSecs = *timeStamp++;
		fClockNsecs = *timeStamp;
		if (gMrbAnalyze->TreeToBeWritten()) fTreeOut->Fill();
		return(kTRUE);
	} else {
		gMrbLog->Err()	<< "Illegal subevent id - " << BaseAddr->sevt_id << " (should be 999)" << endl;
		gMrbLog->Flush(this->ClassName(), "ExtractTimeStamp");
		return(kFALSE);
	}
}

Bool_t TUsrEvtStop::InitializeTree(TFile * RootFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtStop::InitializeTree
// Purpose:        Initialize stop event tree to read data
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Initializes the stop event tree.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	register TObject * obj;
	TString name;

	obj = RootFile->Get("Stop");
	if (obj == NULL) {
		gMrbLog->Err()	<< "Tree \"Stop\" not found in ROOT file " << RootFile->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "InitializeTree");
		return(kFALSE);
	}
	name = obj->ClassName();
	if (name.CompareTo("TTree") != 0) {
		gMrbLog->Err()	<< "Not a TTree object - " << obj->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "InitializeTree");
		return(kFALSE);
	}
	fTreeIn = (TTree *) obj;

	fTreeIn->SetBranchAddress("stop", &fClockSecs);		// time of stop event

	return(kTRUE);
}


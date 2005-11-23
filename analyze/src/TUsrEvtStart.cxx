//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtStart.cxx
// Purpose:        Implement class methods for user's analysis
// Description:    Class for start event
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TUsrEvtStart.cxx,v 1.1 2005-11-23 11:51:53 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include "TMrbAnalyze.h"
#include "TUsrEvtStart.h"
#include "SetColor.h"

extern TMrbAnalyze * gMrbAnalyze;
extern TMrbLogger * gMrbLog;

ClassImp(TUsrEvtStart)

TUsrEvtStart::TUsrEvtStart() {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtStart
// Purpose:        Create an event for "Start Acquisition"
// Description:    Creates an event for trigger 14 (Start)
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fType = 10; 					// event type (given by MBS)
	fSubtype = 1;					// ... subtype
	fTrigger = 14;					// trigger number 14: start
	fNofEvents = 0; 				// reset event counter
	fEventNumber = 0;				// event number provided by MBS
}

Bool_t TUsrEvtStart::CreateTree() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtStart::CreateTree
// Purpose:        Create a tree for event "Start Acquisition"
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Creates a tree for event "Start Acquisition"
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbAnalyze->TreeToBeWritten()) {
		if (gMrbAnalyze->IsVerbose()) {
			cout	<< this->ClassName() << "::CreateTree(): Creating tree \"Start Acquisition\""
					<< endl;
		}
		fTreeOut = new TTree("Start", "Start Acquisition");
		fTreeOut->SetAutoSave(1000);					// automatic save after 1 kB
		fTreeOut->Branch("start", &fClockSecs, "start/i");
	}
	return(kTRUE);
}

Bool_t TUsrEvtStart::ExtractTimeStamp(const s_vehe * EventData, const MBSDataIO * BaseAddr) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtStart::ExtractTimeStamp
// Purpose:        Get time stamp from (sub)event header
// Arguments:      const s_vehe * EventData   -- pointer to MBS event structure
//                 const MBSDataIO * BaseAddr -- pointer to MBS data base
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Decodes the event header,
//                 then looks for a subevent [9000,1] = time stamp
//                 (online mode only)
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t * timeStamp;				// pointer to time info

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

Bool_t TUsrEvtStart::InitializeTree(TFile * RootFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtStart::InitializeTree
// Purpose:        Initialize start event tree to read data
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Initializes the start event tree.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	register TObject * obj;
	TString name;

	obj = RootFile->Get("Start");
	if (obj == NULL) {
		gMrbLog->Err()	<< "Tree \"Start\" not found in ROOT file " << RootFile->GetName() << endl;
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

	fTreeIn->SetBranchAddress("start", &fClockSecs);		// time of start event

	return(kTRUE);
}

void TUsrEvtStart::Print() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtStart::Print
// Purpose:        Output time stamp of last "start acquisition"
// Arguments:      
// Results:        
// Exceptions:     
// Description:    Outputs a message containing time of last start cmd
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TDatime d;
	d.Set((UInt_t) fClockSecs, kFALSE);
	cout	<< setblue << this->ClassName() << "::Print(): START Acquisition at " << d.AsString() << setblack << endl;
}


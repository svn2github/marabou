//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrDeadTime.cxx
// Purpose:        Implement class methods for user's analysis
// Description:    Class for a dead-time event
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TUsrDeadTime.cxx,v 1.1 2005-11-23 11:51:53 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>

#include "TUsrDeadTime.h"

#include "TMrbLogger.h"
#include "TMrbAnalyze.h"
#include "SetColor.h"

extern TMrbAnalyze * gMrbAnalyze;
extern TMrbLogger * gMrbLog;

extern TUsrDeadTime * gDeadTime;

ClassImp(TUsrDeadTime)

void TUsrDeadTime::Set(Int_t TimeStamp, Int_t TotalEvents, Int_t AccuEvents, Int_t ScalerContents) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrDeadTime::Set
// Purpose:        Calculate dead time
// Arguments:      Int_t TimeStamp        -- time stamp (since last start)
//                 Int_t TotalEvents      -- total events gathered since last start
//                 Int_t AccuEvents       -- events accumulated within dead-time interval
//                 Int_t ScalerContents   -- contents of the dead-time scaler w/i interval
// Results:        --
// Exceptions:     
// Description:    Calculates dead time in percent.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fTimeStamp = TimeStamp;
	fTotalEvents = TotalEvents;
	fAccuEvents = AccuEvents;
	fScalerContents = ScalerContents;
	if (ScalerContents > 0) fDeadTime = (1. - (Float_t) AccuEvents / (Float_t) ScalerContents) * 100;
	else					fDeadTime = 0.;
}

Bool_t TUsrDeadTime::CreateTree() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrDeadTime::CreateTree
// Purpose:        Create tree to store dead-time data
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Creates a tree to output dead-time records to root file.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbAnalyze->TreeToBeWritten()) {
		if (gMrbAnalyze->IsVerbose()) {
			cout	<< this->ClassName() << "::CreateTree(): Creating tree \"DeadTime\""
					<< endl;
		}
		fTreeOut = new TTree("DeadTime", "Dead Time");
		fTreeOut->SetAutoSave(1000);						// automatic save after 1 kB
		fBranch = (TObject *) gDeadTime; 					// we need a '**' ref to this object
		fTreeOut->Branch("dtData", this->ClassName(), &fBranch, gMrbAnalyze->GetBranchSize(), 0);
	}
	return(kTRUE);
}

Bool_t TUsrDeadTime::InitializeTree(TFile * RootFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrDeadTime::InitializeTree
// Purpose:        Initialize dead-time tree to read data
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Initializes the dead-time tree.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	register TObject * obj;
	TString name;

	obj = RootFile->Get("DeadTime");
	if (obj == NULL) {
		gMrbLog->Wrn()	<< "Tree \"DeadTime\" not found in ROOT file " << RootFile->GetName() << endl;
		gMrbLog->Flush();
		gMrbLog->Wrn()	<< "Probably NO DEAD TIME info written to this file" << endl;
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
	fReplayMode = kTRUE;

	fTreeIn->SetBranchAddress("fTimeStamp", &fTimeStamp);					// time stamp since last start
	fTreeIn->SetBranchAddress("fTotalEvents", &fTotalEvents); 				// event count
	fTreeIn->SetBranchAddress("fAccuEvents", &fAccuEvents);					// accumulated events
	fTreeIn->SetBranchAddress("fScalerContents", &fScalerContents);			// contents of dead-tim scaler
	fTreeIn->SetBranchAddress("fDeadTime", &fDeadTime);						// dead time (%)
	fTreeIn->SetBranchStatus("*", 0);										// disable branch

	return(kTRUE);
}

void TUsrDeadTime::Print() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrDeadTime::Print
// Purpose:        Print last dead time record
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Dead-time printout.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t secs = fTimeStamp / 10000;
	gMrbLog->Out()	<< "Current dead time value = "
					<< setw(5) << setprecision(2) << fDeadTime << " %"
					<< " (evt " << fTotalEvents << ", time " << secs << "s since start)" << endl;
	gMrbLog->Flush(this->ClassName(), "Print", setblue);
}


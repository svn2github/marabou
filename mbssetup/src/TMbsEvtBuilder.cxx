//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbssetup/src/TMbsEvtBuilder.cxx
// Purpose:        MBS setup
// Description:    Implements class methods to define a MBS setup 
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMbsEvtBuilder.cxx,v 1.8 2005-01-10 12:59:24 rudi Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "Rtypes.h"

#include "TMrbLogger.h"

#include "TMbsSetup.h"
#include "TMbsEvtBuilder.h"

#include "SetColor.h"

extern TSystem * gSystem;
extern TMrbLogger * gMrbLog;
extern TMbsSetup * gMbsSetup;

ClassImp(TMbsEvtBuilder)

TMbsEvtBuilder::TMbsEvtBuilder() {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsEvtBuilder
// Purpose:        Define a MBS event builder
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger("mbssetup.log");
	gMbsSetup->CopyDefaults("EvtBuilder", kFALSE);	// get defaults
}

void TMbsEvtBuilder::RemoveSetup() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsEvtBuilder::RemoveSetup
// Purpose:        Remove entries from setup data base
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Remove resources "TMbsSetup.EvtBuilder.*".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	gMbsSetup->Remove("EvtBuilder", kFALSE);	// remove entries
}

void TMbsEvtBuilder::Reset() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsEvtBuilder::Reset
// Purpose:        Reset to default
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Reset resources "TMbsSetup.EvtBuilder.*" to default values.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	gMbsSetup->CopyDefaults("EvtBuilder", kFALSE, kTRUE);	// force copy
}

Bool_t TMbsEvtBuilder::SetProcName(const Char_t * ProcName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsEvtBuilder::SetProcName
// Purpose:        Set event builder name
// Arguments:      Char_t * ProcName    -- name
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines resource "TMbsSetup.EvtBuilder.Name".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TInetAddress * ia;
	TString procName;
	TString procType;
	TMrbNamedX * ptype;
	TString rshCmd;
	Int_t n;
	
	procName = ProcName;
	procName.ToLower();					// use always lower case names

	ia = new TInetAddress(gSystem->GetHostByName(ProcName));
	procName = ia->GetHostName();
	if (procName.CompareTo("UnknownHost") == 0) {
		gMrbLog->Err() << "No such processor - " << ProcName << endl;
		gMrbLog->Flush(this->ClassName(), "SetName");
		return(kFALSE);
	} else {
		gMbsSetup->Set("EvtBuilder.Name", ProcName);				// name
		gMbsSetup->Set("EvtBuilder.Address", ia->GetHostName());	// inet addr
		cout	<< this->ClassName() << "::SetName(): " << ProcName
				<< " has addr " << ia->GetHostName()
				<< " (" << ia->GetHostAddress() << ")"
				<< endl;

		ptype = this->GetType();
		if (ptype == NULL) {
			procType = ProcName;				// try to determine proc type
			if ((n = procType.Index("-")) >= 0) {
				procType = procType(0, n);
				ptype = gMbsSetup->fLofProcs.FindByName(procType.Data(),
												TMrbLofNamedX::kFindExact | TMrbLofNamedX::kFindIgnoreCase);
			}
		}
		if (ptype == NULL) {
			gMrbLog->Err() << ProcName << " is of unknown type - must be defined separately" << endl;
			gMrbLog->Flush(this->ClassName(), "SetName");
			return(kFALSE);
		} else {
			if (this->SetType((EMbsProcType) ptype->GetIndex())) {
				cout	<< this->ClassName() << "::SetName(): " << ProcName
						<< " is of type " << ptype->GetName()
						<< "(" << ptype->GetIndex() << ")"
						<< endl;
			} else {
				return(kFALSE);
			}
		}
		return(kTRUE);
	}
}

const Char_t * TMbsEvtBuilder::GetProcName() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsEvtBuilder::GetProcName
// Purpose:        Get event builder name
// Arguments:      --
// Results:        Char_t * ProcName    -- name
// Exceptions:
// Description:    Reads resource "TMbsSetup.EvtBuilder.Name".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	gMbsSetup->Get(fName, "EvtBuilder.Name");
	return(fName.Data());
}

const Char_t * TMbsEvtBuilder::GetProcAddr() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsEvtBuilder::GetProcAddr
// Purpose:        Get event builder address
// Arguments:      --
// Results:        Char_t * ProcAddr    -- addr
// Exceptions:
// Description:    Reads resource "TMbsSetup.EvtBuilder.Address".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	gMbsSetup->Get(fAddr, "EvtBuilder.Address");
	return(fAddr.Data());
}

Bool_t TMbsEvtBuilder::SetType(const Char_t * ProcType) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsEvtBuilder::SetType
// Purpose:        Set event builder type
// Arguments:      Char_t * ProcType    -- processor type
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines resource "TMbsSetup.EvtBuilder.Type".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * procType;
	ostringstream * proc;

	procType = gMbsSetup->fLofProcs.FindByName(ProcType, TMrbLofNamedX::kFindExact | TMrbLofNamedX::kFindIgnoreCase);
	if (procType == NULL) {
		gMrbLog->Err() << "Wrong processor type - " << ProcType << endl;
		gMrbLog->Flush(this->ClassName(), "SetType");
		cerr	<< this->ClassName() << "::SetType(): Legal types are - "
				<< endl;
		gMbsSetup->fLofProcs.Print(cerr, "   > ");
		return(kFALSE);
	}

	proc = new ostringstream();
	*proc << procType->GetName() << "(" << procType->GetIndex() << ")" << ends;
	gMbsSetup->Set("EvtBuilder.Type", proc->str().c_str());
//	proc->rdbuf()->freeze(0);
	delete proc;
	return(kTRUE);
}

Bool_t TMbsEvtBuilder::SetType(EMbsProcType ProcType) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsEvtBuilder::SetType
// Purpose:        Set event builder type
// Arguments:      EMbsProcType ProcType    -- processor type
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines resource "TMbsSetup.EvtBuilder.Type".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * procType;
	ostringstream * proc;

	procType = gMbsSetup->fLofProcs.FindByIndex(ProcType);
	if (procType == NULL) {
		gMrbLog->Err() << "Wrong processor type - " << ProcType << endl;
		gMrbLog->Flush(this->ClassName(), "SetType");
		cerr	<< this->ClassName() << "::SetType(): Legal types are - "
				<< endl;
		gMbsSetup->fLofProcs.Print(cerr, "   > ");
		return(kFALSE);
	}

	proc = new ostringstream();
	*proc << procType->GetName() << "(" << procType->GetIndex() << ")" << ends;
	gMbsSetup->Set("EvtBuilder.Type", proc->str().c_str());
//	proc->rdbuf()->freeze(0);
	delete proc;
	return(kTRUE);
}

TMrbNamedX * TMbsEvtBuilder::GetType() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsEvtBuilder::GetType
// Purpose:        Read event builder type
// Arguments:      --
// Results:        TMrbNamedX * ProcType
// Exceptions:
// Description:    Reads resource "TMbsSetup.EvtBuilder.Type".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * procType;
	TString resValue;
	Int_t n;

	gMbsSetup->Get(resValue, "EvtBuilder.Type");
	if ((n = resValue.Index("(")) >= 0) resValue = resValue(0, n);
	procType = gMbsSetup->fLofProcs.FindByName(resValue, TMrbLofNamedX::kFindExact | TMrbLofNamedX::kFindIgnoreCase);
	return(procType);
}

Bool_t TMbsEvtBuilder::SetCrate(Int_t Crate) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsEvtBuilder::SetCrate
// Purpose:        Define crate number
// Arguments:      Int_t Crate    -- crate number
// Results:        --
// Exceptions:
// Description:    Sets resource "TMbsSetup.EvtBuilder.Crate".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	gMbsSetup->Set("EvtBuilder.Crate", Crate);
	return(kTRUE);
}

Int_t TMbsEvtBuilder::GetCrate() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsEvtBuilder::GetCrate
// Purpose:        Return crate number
// Arguments:      --
// Results:        Int_t Crate  -- crate number
// Exceptions:
// Description:    Reads resource "TMbsSetup.EvtBuilder.Crate".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	return(gMbsSetup->Get("EvtBuilder.Crate", -1));
}

Bool_t TMbsEvtBuilder::SetFlushTime(Int_t Seconds) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsEvtBuilder::SetFlushTime
// Purpose:        Define flush time
// Arguments:      Int_t Seconds    -- flush time in secs
// Results:        --
// Exceptions:
// Description:    Sets resource "TMbsSetup.EvtBuilder.FlushTime".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	gMbsSetup->Set("EvtBuilder.FlushTime", Seconds);
	return(kTRUE);
}

Int_t TMbsEvtBuilder::GetFlushTime() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsEvtBuilder::GetFlushTime
// Purpose:        Return current flush time
// Arguments:      --
// Results:        Int_t Seconds  -- flush time in seconds
// Exceptions:
// Description:    Reads resource "TMbsSetup.EvtBuilder.FlushTime".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	return(gMbsSetup->Get("EvtBuilder.FlushTime", 0));
}

Bool_t TMbsEvtBuilder::SetVSBAddr(UInt_t Addr) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsEvtBuilder::SetVSBAddr
// Purpose:        Define vsb address
// Arguments:      UInt_t Addr    -- address
// Results:        --
// Exceptions:
// Description:    Sets resource "TMbsSetup.EvtBuilder.VSBAddr".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	gMbsSetup->Set("EvtBuilder.VSBAddr", (Int_t) Addr, 16);
	return(kTRUE);
}

UInt_t TMbsEvtBuilder::GetVSBAddr() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsEvtBuilder::GetVSBAddr
// Purpose:        Return vsb address
// Arguments:      --
// Results:        UInt_t Addr  -- address
// Exceptions:
// Description:    Reads resource "TMbsSetup.EvtBuilder.VSBAddr".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	return(gMbsSetup->Get("EvtBuilder.VSBAddr", 0));
}

void TMbsEvtBuilder::SetBuffers(Int_t BufferSize, Int_t NofBuffers, Int_t NofStreams) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsEvtBuilder::SetBuffers
// Purpose:        Define buffer size
// Arguments:      Int_t BufferSize   -- buffer size
//                 Int_t NofBuffers   -- buffers per stream
//                 Int_t NofStreams   -- number of streams
// Results:        --
// Exceptions:
// Description:    Sets resources "TMbsSetup.EvtBuilder.BufferSize",
//                                "TMbsSetup.EvtBuilder.NofBuffers",
//                 and            "TMbsSetup.EvtBuilder.NofStreams"
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	gMbsSetup->Set("EvtBuilder.BufferSize", BufferSize, 16);
	gMbsSetup->Set("EvtBuilder.NofBuffers", NofBuffers);
	gMbsSetup->Set("EvtBuilder.NofStreams", NofStreams);
}

Int_t TMbsEvtBuilder::GetBufferSize() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsEvtBuilder::GetBufferSize
// Purpose:        Return buffer size
// Arguments:      --
// Results:        Int_t BufferSize  -- buffer size
// Exceptions:
// Description:    Reads resource "TMbsSetup.EvtBuilder.BufferSize".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	return(gMbsSetup->Get("EvtBuilder.BufferSize", 0));
}

Int_t TMbsEvtBuilder::GetNofBuffers() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsEvtBuilder::GetNofBuffers
// Purpose:        Return number of buffers
// Arguments:      --
// Results:        Int_t NofBuffers  -- number of buffers
// Exceptions:
// Description:    Reads resource "TMbsSetup.EvtBuilder.NofBuffers".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	return(gMbsSetup->Get("EvtBuilder.NofBuffers", 0));
}

Int_t TMbsEvtBuilder::GetNofStreams() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsEvtBuilder::GetNofStreams
// Purpose:        Return number of streams
// Arguments:      --
// Results:        Int_t NofStreams  -- number of streams
// Exceptions:
// Description:    Reads resource "TMbsSetup.EvtBuilder.NofStreams".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	return(gMbsSetup->Get("EvtBuilder.NofStreams", 0));
}

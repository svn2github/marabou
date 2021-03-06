//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbssetup/src/TMbsReadoutProc.cxx
// Purpose:        MBS setup
// Description:    Implements class methods to define a MBS setup 
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMbsReadoutProc.cxx,v 1.24 2011-03-08 10:31:22 Marabou Exp $       
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
#include "TMrbSystem.h"

#include "TMbsSetup.h"
#include "TMbsReadoutProc.h"

#include "SetColor.h"

extern TSystem * gSystem;
extern TMrbLogger * gMrbLog;
extern TMbsSetup * gMbsSetup;

ClassImp(TMbsReadoutProc)

TMbsReadoutProc::TMbsReadoutProc(Int_t ProcNo) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc
// Purpose:        Define a MBS readout process
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbString rdoName;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger("mbssetup.log");
	
	fId = ProcNo;		// store process number
	if (fId >= 0) {
		rdoName = "Readout";
		rdoName += fId;
		TMrbLofNamedX lofSubs;
		lofSubs.AddNamedX(0, "$rdoNo", Form("%d", fId));
		gMbsSetup->CopyDefaults("Readout$rdoNo", kFALSE, kFALSE, &lofSubs);	// get defaults
	}
	fTriggerModule = new TMbsTriggerModule(fId);		// alloc trigger module
}

void TMbsReadoutProc::RemoveSetup() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::RemoveSetup
// Purpose:        Remove entries from setup data base
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Remove resources "TMbsSetup.ReadoutNNN.*".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbString rdoName;

	if (fId >= 0) {
		rdoName = "Readout";
		rdoName += fId;
		gMbsSetup->Remove(rdoName.Data(), kFALSE);	// remove entries
	}
}

void TMbsReadoutProc::Reset() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::Reset
// Purpose:        Reset to defaults
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Resets resources "TMbsSetup.ReadoutNNN.*" to default values.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbString rdoName;

	if (fId >= 0) {
		rdoName = "Readout";
		rdoName += fId;
		TMrbLofNamedX lofSubs;
		lofSubs.AddNamedX(0, "$rdoNo", Form("%d", fId));
		gMbsSetup->CopyDefaults("Readout$rdoNo", kFALSE, kFALSE, &lofSubs);	// get defaults
	}
}

Bool_t TMbsReadoutProc::SetProcName(const Char_t * ProcName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::SetProcName
// Purpose:        Set readout name
// Arguments:      Char_t * ProcName    -- readout name
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines resource "TMbsSetup.ReadoutNNN.Name".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TInetAddress * ia;
	TString r;
	TString procName;
	TString procType;
	TMrbNamedX * ptype;
	Int_t n;
	
	if (fId < 0) return(kFALSE);

	procName = ProcName;
	procName.ToLower();					// use always lower case names

	ia = new TInetAddress(gSystem->GetHostByName(ProcName));
	procName = ia->GetHostName();
	if (procName.CompareTo("UnknownHost") == 0) {
		gMrbLog->Err() << "No such processor - " << ProcName << endl;
		gMrbLog->Flush(this->ClassName(), "SetName");
		return(kFALSE);
	} else {
		gMbsSetup->Set(gMbsSetup->Resource(r, "Readout", fId, "Name"), ProcName);
		gMbsSetup->Set(gMbsSetup->Resource(r, "Readout", fId, "Address"), ia->GetHostName());
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

const Char_t * TMbsReadoutProc::GetProcName() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::GetProcName
// Purpose:        Get name of readout processor
// Arguments:      --
// Results:        Char_t * ProcName    -- readout name
// Exceptions:
// Description:    Reads resource "TMbsSetup.ReadoutNNN.Name".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;
	gMbsSetup->Get(fName, gMbsSetup->Resource(r, "Readout", fId, "Name"));
	return(fName.Data());
}

const Char_t * TMbsReadoutProc::GetProcAddr() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::GetProcAddr
// Purpose:        Get readout addr
// Arguments:      --
// Results:        Char_t * ProcAddr    -- readout addr
// Exceptions:
// Description:    Reads resource "TMbsSetup.ReadoutNNN.Address".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;
	gMbsSetup->Get(fAddr, gMbsSetup->Resource(r, "Readout", fId, "Address"));
	return(fAddr.Data());
}

Bool_t TMbsReadoutProc::SetType(const Char_t * ProcType) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::SetType
// Purpose:        Set readout type
// Arguments:      Char_t * ProcType    -- processor type
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines resource "TMbsSetup.ReadoutNNN.Type".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;
	TMrbNamedX * procType;
	TMrbString proc;

	if (fId < 0) return(kFALSE);

	procType = gMbsSetup->fLofProcs.FindByName(ProcType, TMrbLofNamedX::kFindExact | TMrbLofNamedX::kFindIgnoreCase);
	if (procType == NULL) {
		gMrbLog->Err() << "Wrong processor type - " << ProcType << endl;
		gMrbLog->Flush(this->ClassName(), "SetType");
		cerr	<< this->ClassName() << "::SetType(): Legal types are - "
				<< endl;
		gMbsSetup->fLofProcs.Print(cerr, "   > ");
		return(kFALSE);
	}

	proc = procType->GetName();
	proc += "(";
	proc += procType->GetIndex();
	proc += ")";
	gMbsSetup->Set(gMbsSetup->Resource(r, "Readout", fId, "Type"), proc.Data());
	return(kTRUE);
}

Bool_t TMbsReadoutProc::SetType(EMbsProcType ProcType) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::SetType
// Purpose:        Set readout type
// Arguments:      EMbsProcType ProcType    -- processor type
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines resource "TMbsSetup.ReadoutNNN.Type".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;
	TMrbNamedX * procType;
	TMrbString proc;

	if (fId < 0) return(kFALSE);

	procType = gMbsSetup->fLofProcs.FindByIndex(ProcType);
	if (procType == NULL) {
		gMrbLog->Err() << "Wrong processor type - " << ProcType << endl;
		gMrbLog->Flush(this->ClassName(), "SetType");
		cerr	<< this->ClassName() << "::SetType(): Legal types are - "
				<< endl;
		gMbsSetup->fLofProcs.Print(cerr, "   > ");
		return(kFALSE);
	}

	proc = procType->GetName();
	proc += "(";
	proc += procType->GetIndex();
	proc += ")";
	gMbsSetup->Set(gMbsSetup->Resource(r, "Readout", fId, "Type"), proc.Data());
	return(kTRUE);
}

TMrbNamedX * TMbsReadoutProc::GetType() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::GetType
// Purpose:        Read readout type
// Arguments:      --
// Results:        TMrbNamedX * ProcType
// Exceptions:
// Description:    Reads resource "TMbsSetup.ReadoutNNN.Type".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;
	TMrbNamedX * procType;
	TString resValue;
	Int_t n;

	gMbsSetup->Get(resValue, gMbsSetup->Resource(r, "Readout", fId, "Type"));
	if ((n = resValue.Index("(")) >= 0) resValue = resValue(0, n);
	if (resValue.IsNull()) resValue = gEnv->GetValue("TMbsSetup.ProcType", "undef");
	procType = gMbsSetup->fLofProcs.FindByName(resValue, TMrbLofNamedX::kFindExact | TMrbLofNamedX::kFindIgnoreCase);
	return(procType);
}

Bool_t TMbsReadoutProc::SetPath(const Char_t * Path, Bool_t Create) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::SetPath
// Purpose:        Define subdir where to write setup files
// Arguments:      Char_t * Path       -- path name
//                 Bool_t Create       -- kTRUE if path has to be created
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets resource "TMbsSetup.ReadoutNNN.Path".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Long_t dmy;
	Long_t flags;
	TString pathName;
	TString remoteHome;
	TString r;

	if (fId < 0) return(kFALSE);

	pathName = gMbsSetup->GetHomeDir();	// get home dir
	if (pathName.IsNull()) {
		remoteHome = gMbsSetup->RemoteHomeDir();
		if (remoteHome.IsNull()) {
			gMrbLog->Err() << "Can't set path \"" << Path << "\" -" << endl;
			gMrbLog->Flush(this->ClassName(), "SetPath");
			gMrbLog->Err() << "\"TMbsSetup.HomeDir\" has to be set properly first" << endl;
			gMrbLog->Flush(this->ClassName(), "SetPath");
			return(kFALSE);
		} else {
			gMbsSetup->SetHomeDir(remoteHome.Data());
			pathName = remoteHome;
		}
	}

	pathName = gMbsSetup->GetPath();	// get mbs working dir
	if (pathName.IsNull()) {
		gMrbLog->Err() << "Can't set path \"" << Path << "\" -" << endl;
		gMrbLog->Flush(this->ClassName(), "SetPath");
		gMrbLog->Err() << "\"TMbsSetup.Path\" has to be set properly first" << endl;
		gMrbLog->Flush(this->ClassName(), "SetPath");
		return(kFALSE);
	}

	if (!pathName.BeginsWith("/") && !pathName.BeginsWith("./")) {
		pathName = gMbsSetup->GetHomeDir();
		pathName += "/";
		pathName += gMbsSetup->GetPath();
	}
	pathName += "/";
	pathName += Path; 			// append subdir
	
	Long64_t dmy64;
	if (gSystem->GetPathInfo(pathName.Data(), &dmy, &dmy64, &flags, &dmy) != 0 || (flags & 0x2) == 0) {
		if (Create) {
			if (gSystem->MakeDirectory(pathName.Data()) == 0) {
				cout	<< this->ClassName() << "::SetPath(): Creating directory " << pathName
						<< endl;
			} else {
				gMrbLog->Err() << "Can't create directory - " << pathName << endl;
				gMrbLog->Flush(this->ClassName(), "SetPath");
				return(kFALSE);
			}
		} else {
			gMrbLog->Err() << "No such directory - " << pathName << endl;
			gMrbLog->Flush(this->ClassName(), "SetPath");
			return(kFALSE);
		}
	}
	gMbsSetup->Set(gMbsSetup->Resource(r, "Readout", fId, "Path"), Path);
	return(kTRUE);
}

const Char_t * TMbsReadoutProc::GetPath() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::GetPath
// Purpose:        Read subdir where to write setup files
// Arguments:      --
// Results:        Char_t * Path       -- path name
// Exceptions:
// Description:    Gets resource "TMbsSetup.ReadoutNNN.Path".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;
	gMbsSetup->Get(fPath, gMbsSetup->Resource(r, "Readout", fId, "Path"));
	return(fPath.Data());
}

Bool_t TMbsReadoutProc::SetCrate(Int_t Crate) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::SetCrate
// Purpose:        Define crate number
// Arguments:      Int_t Crate    -- crate number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets resource "TMbsSetup.ReadoutNNN.Crate".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;

	if (fId < 0) return(kFALSE);

	gMbsSetup->Set(gMbsSetup->Resource(r, "Readout", fId, "Crate"), Crate);
	return(kTRUE);
}

Int_t TMbsReadoutProc::GetCrate() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::GetCrate
// Purpose:        Return crate number
// Arguments:      --
// Results:        Int_t Crate  -- crate number
// Exceptions:
// Description:    Reads resource "TMbsSetup.ReadoutNNN.Crate".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;
	return(gMbsSetup->Get(gMbsSetup->Resource(r, "Readout", fId, "Crate"), 0));
}

Bool_t TMbsReadoutProc::SetCratesToBeRead(Int_t C1, Int_t C2, Int_t C3, Int_t C4, Int_t C5) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::SetCratesToBeRead
// Purpose:        Define crates connected to this readout
// Arguments:      Int_t Cxx     -- crate number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets resource "TMbsSetup.ReadoutNNN.CratesToBeRead".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	Int_t nofCrates;
	TString cstr;
	Int_t crate;
	TString r;

	TArrayI lofCrates(5);

	lofCrates[0] = C1;
	lofCrates[1] = C2;
	lofCrates[2] = C3;
	lofCrates[3] = C4;
	lofCrates[4] = C5;

	nofCrates = 0;
	for (i = 0; i < 5; i++) {
		crate = lofCrates[i];
		if (crate == -1) break;
		if (crate < 0 || crate >= kNofCrates) {
			gMrbLog->Err() << "Illegal crate - " << crate << " (should be in [0," << kNofCrates << "])" << endl;
			gMrbLog->Flush(this->ClassName(), "SetCratesToBeRead");
			return(kFALSE);
		}
		nofCrates++;
	}
	if (nofCrates == 0) {
		cstr.Resize(0);
	} else {
		cstr = gMbsSetup->EncodeArray(lofCrates, nofCrates);
		cstr = cstr(1, cstr.Length() - 2);				// strip parens "(...)"
	}
	return(gMbsSetup->Set(gMbsSetup->Resource(r, "Readout", fId, "CratesToBeRead"), cstr.Data()));
}

Int_t TMbsReadoutProc::GetCratesToBeRead(TArrayI & LofCrates) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::GetCratesToBeRead
// Purpose:        Return crates connected to this readout
// Arguments:      TArrayI & LofCrates    -- where to store crate numbers
// Results:        Int_t nofCrates        -- number if crates
// Exceptions:
// Description:    Reads resource "TMbsSetup.ReadoutNNN.CratesToBeRead".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t n1, n2;
	TString crateString;
	TString crate;
	TString cstr;
	Int_t one;
	Int_t crateNo;
	Int_t nofCrates;
	TString r;

	gMbsSetup->Get(crateString, gMbsSetup->Resource(r, "Readout", fId, "CratesToBeRead"));
	crateString += ",";

	n1 = 0;
	nofCrates = 0;
	while ((n2 = crateString.Index(",", n1)) != -1) {
		crate = crateString(n1, n2 - n1);
		crate = crate.Strip(TString::kBoth);
		cstr = crate + " 1";
		one = 0;
		istringstream s(cstr.Data());
		s >> crateNo >> one;
		if (one != 1) {
			gMrbLog->Err() << "Illegal crate number - " << crate << endl;
			gMrbLog->Flush(this->ClassName(), "GetCratesToBeRead");
		} else {
			LofCrates[nofCrates] = crateNo;
			nofCrates++;
		}
		n1 = n2 + 1;
	}
	return(nofCrates);
}

Bool_t TMbsReadoutProc::SetController(const Char_t * ContrlType) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::SetController
// Purpose:        Set crate controller
// Arguments:      Char_t * ContrlType    -- controller type
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines resource "TMbsSetup.ReadoutNNN.Controller".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;
	TMrbNamedX * contrlType;
	TMrbString cntrl;

	if (fId < 0) return(kFALSE);

	contrlType = gMbsSetup->fLofControllers.FindByName(ContrlType, TMrbLofNamedX::kFindExact | TMrbLofNamedX::kFindIgnoreCase);
	if (contrlType == NULL) {
		gMrbLog->Err() << "Wrong type - " << ContrlType << endl;
		gMrbLog->Flush(this->ClassName(), "SetController");
		cerr	<< this->ClassName() << "::SetController(): Legal controllers are - "
				<< endl;
		gMbsSetup->fLofControllers.Print(cerr, "   > ");
		return(kFALSE);
	}

	cntrl = contrlType->GetName();
	cntrl += "(";
	cntrl += contrlType->GetIndex();
	cntrl += ")";
	gMbsSetup->Set(gMbsSetup->Resource(r, "Readout", fId, "Controller"), cntrl.Data());
	return(kTRUE);
}

Bool_t TMbsReadoutProc::SetController(EMbsControllerType ContrlType) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::SetController
// Purpose:        Set crate controller
// Arguments:      EMbsControllerType ContrlType    -- controller type
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines resource "TMbsSetup.ReadoutNNN.Controller".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;
	TMrbNamedX * contrlType;
	TMrbString cntrl;

	if (fId < 0) return(kFALSE);

	contrlType = gMbsSetup->fLofControllers.FindByIndex(ContrlType);
	if (contrlType == NULL) {
		gMrbLog->Err() << "Wrong type - " << ContrlType << endl;
		gMrbLog->Flush(this->ClassName(), "SetController");
		cerr	<< this->ClassName() << "::SetController(): Legal controllers are - "
				<< endl;
		gMbsSetup->fLofControllers.Print(cerr, "   > ");
		return(kFALSE);
	}

	cntrl = contrlType->GetName();
	cntrl += "(";
	cntrl += contrlType->GetIndex();
	cntrl += ")";
	gMbsSetup->Set(gMbsSetup->Resource(r, "Readout", fId, "Controller"), cntrl.Data());
	return(kTRUE);
}

TMrbNamedX * TMbsReadoutProc::GetController() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::GetController
// Purpose:        Read crate controller
// Arguments:      --
// Results:        TMrbNamedX * ContrlType
// Exceptions:
// Description:    Reads resource "TMbsSetup.ReadoutNNN.Controller".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;
	TMrbNamedX * contrlType;
	TString resValue;
	Int_t n;

	gMbsSetup->Get(resValue, gMbsSetup->Resource(r, "Readout", fId, "Controller"));
	if ((n = resValue.Index("(")) >= 0) resValue = resValue(0, n);
	contrlType = gMbsSetup->fLofControllers.FindByName(resValue, TMrbLofNamedX::kFindExact | TMrbLofNamedX::kFindIgnoreCase);
	return(contrlType);
}

Bool_t TMbsReadoutProc::SetVSBAddr(UInt_t Addr) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::SetVSBAddr
// Purpose:        Define vsb address
// Arguments:      UInt_t Addr    -- address
// Results:        --
// Exceptions:
// Description:    Sets resource "TMbsSetup.ReadoutNNN.VSBAddr".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;
	gMbsSetup->Set(gMbsSetup->Resource(r, "Readout", fId, "VSBAddr"), (Int_t) Addr, 16);
	return(kTRUE);
}

UInt_t TMbsReadoutProc::GetVSBAddr() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::GetVSBAddr
// Purpose:        Return vsb address
// Arguments:      --
// Results:        UInt_t Addr  -- address
// Exceptions:
// Description:    Reads resource "TMbsSetup.ReadoutNNN.VSBAddr".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;
	return(gMbsSetup->Get(gMbsSetup->Resource(r, "Readout", fId, "VSBAddr"), 0));
}

Bool_t TMbsReadoutProc::SetPipeType(Int_t Type) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::SetPipeType
// Purpose:        Define pipe type
// Arguments:      Int_t Type    -- type
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets resource "TMbsSetup.ReadoutNNN.LocalPipeType".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;
	gMbsSetup->Set(gMbsSetup->Resource(r, "Readout", fId, "LocalPipeType"), Type, 16);
	return(kTRUE);
}

Bool_t TMbsReadoutProc::SetPipeBase(UInt_t Addr) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::SetPipeBase
// Purpose:        Define pipe address
// Arguments:      UInt_t Addr    -- address
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets resource "TMbsSetup.ReadoutNNN.LocalPipeBase".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;
	gMbsSetup->Set(gMbsSetup->Resource(r, "Readout", fId, "LocalPipeBase"), (Int_t) Addr, 16);
	return(kTRUE);
}

Int_t TMbsReadoutProc::GetPipeType() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::GetPipeType
// Purpose:        Return pipe type
// Arguments:      --
// Results:        Int_t Type  -- type
// Exceptions:
// Description:    Reads resource "TMbsSetup.ReadoutNNN.LocalPipeBase".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;
	return(gMbsSetup->Get(gMbsSetup->Resource(r, "Readout", fId, "LocalPipeType"), -1));
}

UInt_t TMbsReadoutProc::GetPipeBase() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::GetPipeBase
// Purpose:        Return pipe address
// Arguments:      --
// Results:        UInt_t Addr  -- address
// Exceptions:
// Description:    Reads resource "TMbsSetup.ReadoutNNN.LocalPipeBase".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;
	return(gMbsSetup->Get(gMbsSetup->Resource(r, "Readout", fId, "LocalPipeBase"), 0));
}

Bool_t TMbsReadoutProc::SetPipeSegLength(Int_t Length) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::SetPipeSegLength
// Purpose:        Define pipe segement length
// Arguments:      Int_t Length     -- length in bytes
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets resource "TMbsSetup.ReadoutNNN.LocalPipeSegmentLength".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;
	gMbsSetup->Set(gMbsSetup->Resource(r, "Readout", fId, "LocalPipeSegmentLength"), Length, 16);
	return(kTRUE);
}

Int_t TMbsReadoutProc::GetPipeSegLength() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::GetPipeSegLength
// Purpose:        Return pipe segment length
// Arguments:      --
// Results:        Int_t Length     -- length in bytes
// Exceptions:
// Description:    Reads resource "TMbsSetup.ReadoutNNN.LocalPipeSegmentLength".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;
	return(gMbsSetup->Get(gMbsSetup->Resource(r, "Readout", fId, "LocalPipeSegmentLength"), 0));
}

Bool_t TMbsReadoutProc::SetPipeLength(Int_t NofSevts) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::SetPipeLength
// Purpose:        Define number of subevents in pipe
// Arguments:      Int_t NofSevts     -- number of subevents
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets resource "TMbsSetup.ReadoutNNN.LocalPipeLength".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;
	gMbsSetup->Set(gMbsSetup->Resource(r, "Readout", fId, "LocalPipeLength"), NofSevts);
	return(kTRUE);
}

Int_t TMbsReadoutProc::GetPipeLength() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::GetPipeSegLength
// Purpose:        Return number of subevents in pipe
// Arguments:      --
// Results:        Int_t NofSevts     -- number of subevents
// Exceptions:
// Description:    Reads resource "TMbsSetup.ReadoutNNN.LocalPipeLength".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;
	return(gMbsSetup->Get(gMbsSetup->Resource(r, "Readout", fId, "LocalPipeLength"), 0));
}

Bool_t TMbsReadoutProc::SetSevtSize(Int_t Trigger, Int_t SevtSize) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::SetSevtSize
// Purpose:        Define subevent size
// Arguments:      Int_t Trigger   -- trigger number (0 = global size)
//                 Int_t SevtSize  -- subevent size
// Results:        --
// Exceptions:
// Description:    Sets resource "TMbsSetup.ReadoutNNN.SevtSize[.TriggerX]".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;

	if (Trigger < 0 || Trigger > kNofTriggers) {
		gMrbLog->Err() << "Illegal trigger - " << Trigger << endl;
		gMrbLog->Flush(this->ClassName(), "SetSevtSize");
		return(kFALSE);
	}

	if (Trigger == 0) {
		gMbsSetup->Set(gMbsSetup->Resource(r, "Readout", fId, "SevtSize"), SevtSize);
	} else {
		gMbsSetup->Set(gMbsSetup->Resource(r, "Readout", fId, "SevtSize.Trigger", Trigger), SevtSize);
	}
	return(kTRUE);
}

Int_t TMbsReadoutProc::GetSevtSize(Int_t Trigger) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::GetSevtSize
// Purpose:        Return subevent size
// Arguments:      Int_t Trigger   -- trigger number (0 = global)
// Results:        Int_t SevtSize  -- subevent size
// Exceptions:
// Description:    Reads resource "TMbsSetup.ReadoutNNN.SevtSize[.TriggerX]".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;

	if (Trigger < 0 || Trigger > kNofTriggers) {
		gMrbLog->Err() << "Illegal trigger - " << Trigger << endl;
		gMrbLog->Flush(this->ClassName(), "GetSevtSize");
		return(0);
	}

	if (Trigger == 0) {
		return(gMbsSetup->Get(gMbsSetup->Resource(r, "Readout", fId, "SevtSize"), 0));
	} else {
		return(gMbsSetup->Get(gMbsSetup->Resource(r, "Readout", fId, "SevtSize.Trigger", Trigger), 0));
	}
}

Bool_t TMbsReadoutProc::SetCodeName(const Char_t * CodeName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::SetCodeName
// Purpose:        Define name of makefile
// Arguments:      Char_t * CodeName     -- name of code files
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets resource "TMbsSetup.ReadoutNNN.CodeName".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;
	gMbsSetup->Set(gMbsSetup->Resource(r, "Readout", fId, "CodeName"), CodeName);
	return(kTRUE);
}

const Char_t * TMbsReadoutProc::GetCodeName() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::GetCodeName
// Purpose:        Return name of code file
// Arguments:      --
// Results:        Char_t * CodeName       -- file name
// Exceptions:
// Description:    Gets resource "TMbsSetup.ReadoutNNN.CodeName".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;
	gMbsSetup->Get(fCode, gMbsSetup->Resource(r, "Readout", fId, "CodeName"));
	return(fCode.Data());
}

Bool_t TMbsReadoutProc::CopyMakefile(const Char_t * SrcDir) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::LinkSourceCode
// Purpose:        Establish links to readout source
// Arguments:      Char_t * SrcDir       -- dir where readout code resides
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Copies Makefile from source dir to readout destination.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString mkFile;
	TString srcDir;
	TString srcPath;
	TString rdoPath;
	TString destPath;
	TString fname;
	Bool_t isOK;

	TMrbNamedX * setupMode;
	EMbsSetupMode smode;

	isOK = kTRUE;

	setupMode = gMbsSetup->GetMode();
	smode = (EMbsSetupMode) (setupMode ? setupMode->GetIndex() : 0);

	srcDir = SrcDir;
	if (srcDir.IsNull()) srcDir = gSystem->WorkingDirectory();

	mkFile = this->GetCodeName();
	if (mkFile.Length() == 0) {
		gMrbLog->Err() << "Source file not defined" << endl;
		gMrbLog->Flush(this->ClassName(), "CopyMakefile");
		isOK = kFALSE;
	}

	rdoPath = gMbsSetup->GetPath();
	if (rdoPath.Length() == 0) {
		gMrbLog->Err() << "Setup path not defined" << endl;
		gMrbLog->Flush(this->ClassName(), "CopyMakefile");
		isOK = kFALSE;
	} else if (!rdoPath.BeginsWith("/")) {
		destPath = gMbsSetup->GetHomeDir();
		if (destPath.Length() == 0) {
			gMrbLog->Err() << "Remote home directory not defined" << endl;
			gMrbLog->Flush(this->ClassName(), "CopyMakefile");
			isOK = kFALSE;
		} else {
			destPath += "/";
			destPath += rdoPath;
		}
	} else {
		destPath = rdoPath;
	}
	destPath += "/";

	if (smode == kModeMultiProc) {
		rdoPath = this->GetPath();
		if (rdoPath.Length() == 0) {
			gMrbLog->Err() << "Readout path not defined" << endl;
			gMrbLog->Flush(this->ClassName(), "CopyMakefile");
			isOK = kFALSE;
		} else {
			destPath += rdoPath;
			destPath += "/";
		}
	}

	if (!isOK) return(kFALSE);

	srcPath = srcDir;
	srcPath += "/";

	fname = mkFile + ".mk";
	TString fpath;
	TMrbSystem ux;
	ux.Which(fpath, srcDir.Data(), fname.Data());
	if (fpath.IsNull()) {
		gMrbLog->Err() << "No such file - " << fname << endl;
		gMrbLog->Flush(this->ClassName(), "CopyMakefile");
		isOK = kFALSE;
	}

	TString cmd = "cp " + srcPath + fname + " " + destPath;
	gSystem->Exec(cmd.Data());
	gMrbLog->Out() << "Copying " << srcPath + fname << " --> " << destPath << endl;
	gMrbLog->Flush(this->ClassName(), "CopyMakefile", setblue);

	return(isOK);
}

Bool_t TMbsReadoutProc::CompileReadout(const Char_t * Version) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc::CompileReadout
// Purpose:        Compile source code on remote host
// Arguments:      Char_t * Version   -- MBS version (prod, deve, vNN)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Compiles readout source.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t isOK = kTRUE;

	TMrbNamedX * setupMode = gMbsSetup->GetMode();
	EMbsSetupMode smode = (EMbsSetupMode) (setupMode ? setupMode->GetIndex() : 0);

	TString codeName = this->GetCodeName();
	if (codeName.Length() == 0) {
		gMrbLog->Err() << "Name of source code not defined" << endl;
		gMrbLog->Flush(this->ClassName(), "CompileReadout");
		isOK = kFALSE;
	}

	TString version = Version;
	if (version.Contains(".")) {
		version.ReplaceAll(".", "");
		version.Prepend("v");
	} else if (!version.BeginsWith("v")) {
		gMrbLog->Err() << "Malformed Mbs version - " << Version << endl;
		gMrbLog->Flush(this->ClassName(), "CompileReadout");
		isOK = kFALSE;
	}

	TString path = gMbsSetup->GetPath();
	TString homeDir;
	TString srcPath;
	if (path.Length() == 0) {
		gMrbLog->Err() << "Setup path not defined" << endl;
		gMrbLog->Flush(this->ClassName(), "CompileReadout");
		isOK = kFALSE;
	} else if (!path.BeginsWith("/")) {
		homeDir = gMbsSetup->GetHomeDir();
		if (homeDir.Length() == 0) {
			gMrbLog->Err() << "Remote home directory not defined" << endl;
			gMrbLog->Flush(this->ClassName(), "CompileReadout");
			isOK = kFALSE;
		} else {
			homeDir += "/";
			srcPath = homeDir;
			srcPath += path;
		}
	} else {
		srcPath = path;
	}
	srcPath += "/";

	if (smode == kModeMultiProc) {
		path = this->GetPath();
		if (path.Length() == 0) {
			gMrbLog->Err() << "Readout path not defined" << endl;
			gMrbLog->Flush(this->ClassName(), "CompileReadout");
			isOK = kFALSE;
		} else {
			srcPath += path;
			srcPath += "/";
		}
	}

	if (!isOK) return(kFALSE);

	TString mkFile = codeName + ".mk";
	TMrbSystem ux;
	TString mkpath;
	ux.Which(mkpath, srcPath.Data(), mkFile.Data());
	if (mkpath.IsNull()) {
		gMrbLog->Err() << "No such file - " << mkFile << " (searched on " << srcPath << ")" << endl;
		gMrbLog->Flush(this->ClassName(), "CompileReadout");
		isOK = kFALSE;
	}

	if (!isOK) return(kFALSE);

	TString proc = this->GetProcName();
	if (proc.Length() == 0) {
		gMrbLog->Err() << "Readout proc not defined" << endl;
		gMrbLog->Flush(this->ClassName(), "CompileReadout");
		return(kFALSE);
	}

	TString compileIt = codeName + ".sh";
	path = srcPath + compileIt;
	gSystem->Unlink(path.Data());

	ofstream sh(path.Data(), ios::out);
	if (!sh.good()) {
		gMrbLog->Err() << gSystem->GetError() << " - " << compileIt << endl;
		gMrbLog->Flush(this->ClassName(), "CompileReadout");
		return(kFALSE);
	}

	sh << "#!/bin/tcsh" << endl;
	sh << "#" << compileIt << ": shell script to compile readout source " << codeName << ".c" << endl;
	sh << endl;
	sh << "cd " << srcPath << endl;
	sh << "source /sys/gsi_lynx_env" << endl;
	sh << "set path = ( /bin /bin/ces /usr/bin /usr/local/bin /etc /usr/etc . ~/tools)" << endl;
	sh << "source /mbs/login " << Version << endl;
	sh << "make -f " << mkFile << " clean all" << endl;
	sh.close();

	TString errFile = srcPath;
	errFile += codeName + ".err";
	gSystem->Unlink(errFile.Data());

	cout	<< this->ClassName() << "::CompileReadout(): Compiling " << codeName << ".c"
			<< " on host " << proc << " (MBS version = " << Version << ") ..."
			<< endl << endl;

	TString cmd = Form("rsh %s 'cd %s; tcsh %s' 2>%s", proc.Data(), srcPath.Data(), compileIt.Data(), errFile.Data());
	gSystem->Exec(cmd.Data());

	Int_t nofErrors = 0;
	Int_t nofWarnings = 0;

	TString diagLine;
	ifstream diag(errFile, ios::in);
	if (!diag.good()) {
		gMrbLog->Err() << gSystem->GetError() << " - " << gSystem->BaseName(errFile.Data()) << endl;
		gMrbLog->Flush(this->ClassName(), "CompileReadout");
		isOK = kFALSE;
	} else {
		for (;;) {
			diagLine.ReadLine(diag, kFALSE);
			diagLine = diagLine.Strip(TString::kBoth);
			if (diag.eof()) {
				break;
			}
			if (diagLine.Index("warning:", 0) != -1) {
				nofWarnings++;
				cerr	<< setred
						<< diagLine
						<< setblack << endl;
			} else if (diagLine.Index("error:", 0) != -1) {
				nofErrors++;
				cerr	<< setred
						<< diagLine
						<< setblack << endl;
				isOK = kFALSE;
			} else if (diagLine.Index("Readout.c:", 0) != -1) {
				nofErrors++;
				cerr	<< setred
						<< diagLine
						<< setblack << endl;
				isOK = kFALSE;
			} else if (diagLine.Index("Error", 0) != -1) {
				nofErrors++;
				cerr	<< setred
						<< diagLine
						<< setblack << endl;
				isOK = kFALSE;
			} else if (diagLine.Index("Stop.", 0) != -1) {
				nofErrors++;
				cerr	<< setred
						<< diagLine
						<< setblack << endl;
				isOK = kFALSE;
			}
		}
		diag.close();

		if (nofErrors == 0 && nofWarnings == 0) {
			gMrbLog->Out() << "Readout task compiled & linked - ok" << endl;
			gMrbLog->Flush(this->ClassName(), "CompileReadout");
		} else if (nofErrors > 0) {
			gMrbLog->Err()	<< "Readout task compiled & linked - some error(s) & warning(s)" << endl;
			gMrbLog->Flush(this->ClassName(), "CompileReadout");
		} else {
			gMrbLog->Out()	<< "Readout task compiled & linked - see warning(s)" << endl;
			gMrbLog->Flush(this->ClassName(), "CompileReadout", setblue);
		}
	}

	return(isOK);
}

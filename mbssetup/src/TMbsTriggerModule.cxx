//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbssetup/src/TMbsTriggerModule.cxx
// Purpose:        MBS setup
// Description:    Implements class methods to define a MBS setup 
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMbsTriggerModule.cxx,v 1.5 2004-11-25 12:00:17 rudi Exp $       
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
#include "TMbsTriggerModule.h"

#include "SetColor.h"

extern TSystem * gSystem;
extern TMrbLogger * gMrbLog;
extern TMbsSetup * gMbsSetup;

ClassImp(TMbsTriggerModule)

TMbsTriggerModule::TMbsTriggerModule() {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsTriggerModule
// Purpose:        Define a trigger module
// Arguments:      
// Description:    Creates a trigger module.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger("mbssetup.log");
}

TMbsTriggerModule::TMbsTriggerModule(Int_t Id) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsTriggerModule
// Purpose:        Define a trigger module
// Arguments:      
// Description:    Creates a trigger module.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger("mbssetup.log");
	fId = Id;
}

Bool_t TMbsTriggerModule::SetType(const Char_t * ModuleType) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsTriggerModule::SetType
// Purpose:        Set trigger module type
// Arguments:      Char_t * ModuleType    -- module type
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines resource "TMbsSetup.ReadoutNNN.TriggerModule".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;
	TMrbNamedX * moduleType;
	ostringstream * mod;

	if (fId < 0) return(kFALSE);

	moduleType = gMbsSetup->fLofTriggerModules.FindByName(ModuleType, TMrbLofNamedX::kFindExact | TMrbLofNamedX::kFindIgnoreCase);
	if (moduleType == NULL) {
		gMrbLog->Err() << "Wrong module type - " << ModuleType << endl;
		gMrbLog->Flush(this->ClassName(), "SetType");
		cerr	<< this->ClassName() << "::SetType(): Legal types are - "
				<< endl;
		gMbsSetup->fLofTriggerModules.Print(cerr, "   > ");
		return(kFALSE);
	}

	mod = new ostringstream();
	*mod << moduleType->GetName() << "(" << moduleType->GetIndex() << ")" << ends;
	gMbsSetup->Set(gMbsSetup->Resource(r, "Readout", fId + 1, "TriggerModule.Type"), mod->str().c_str());
//	mod->rdbuf()->freeze(0);
	delete mod;
	return(kTRUE);
}

Bool_t TMbsTriggerModule::SetType(EMbsTriggerModuleType ModuleType) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsTriggerModule::SetType
// Purpose:        Set trigger module type
// Arguments:      EMbsTriggerModuleType ModuleType    -- module type
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines resource "TMbsSetup.ReadoutNNN.TriggerModule".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;
	TMrbNamedX * moduleType;
	ostringstream * mod;

	if (fId < 0) return(kFALSE);

	moduleType = gMbsSetup->fLofTriggerModules.FindByIndex(ModuleType);
	if (moduleType == NULL) {
		gMrbLog->Err() << "Wrong module type - " << ModuleType << endl;
		gMrbLog->Flush(this->ClassName(), "SetType");
		cerr	<< this->ClassName() << "::SetType(): Legal types are - "
				<< endl;
		gMbsSetup->fLofTriggerModules.Print(cerr, "   > ");
		return(kFALSE);
	}

	mod = new ostringstream();
	*mod << moduleType->GetName() << "(" << moduleType->GetIndex() << ")" << ends;
	gMbsSetup->Set(gMbsSetup->Resource(r, "Readout", fId + 1, "TriggerModule.Type"), mod->str().c_str());
//	mod->rdbuf()->freeze(0);
	delete mod;
	return(kTRUE);
}

TMrbNamedX * TMbsTriggerModule::GetType() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsTriggerModule::GetType
// Purpose:        Read trigger module type
// Arguments:      --
// Results:        TMrbNamedX * ModuleType
// Exceptions:
// Description:    Reads resource "TMbsSetup.ReadoutNNN.TriggerModule".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;
	TMrbNamedX * moduleType;
	TString resValue;
	Int_t n;

	gMbsSetup->Get(resValue, gMbsSetup->Resource(r, "Readout", fId + 1, "TriggerModule.Type"));
	if ((n = resValue.Index("(")) >= 0) resValue = resValue(0, n);
	moduleType = gMbsSetup->fLofTriggerModules.FindByName(resValue, TMrbLofNamedX::kFindExact | TMrbLofNamedX::kFindIgnoreCase);
	return(moduleType);
}

Bool_t TMbsTriggerModule::SetTriggerMode(const Char_t * Mode) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsTriggerModule::SetTriggerMode
// Purpose:        Set trigger mode
// Arguments:      Char_t * Mode    -- trigger mode
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines resource "TMbsSetup.ReadoutNNN.TriggerModule.Mode".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;
	TMrbNamedX * trigMode;
	TMrbString mod;

	if (fId < 0) return(kFALSE);

	trigMode = gMbsSetup->fLofTriggerModes.FindByName(Mode, TMrbLofNamedX::kFindExact | TMrbLofNamedX::kFindIgnoreCase);
	if (trigMode == NULL) {
		gMrbLog->Err() << "Wrong mode - " << Mode << endl;
		gMrbLog->Flush(this->ClassName(), "SetTriggerMode");
		cerr	<< this->ClassName() << "::SetTriggerMode(): Legal modes are - "
				<< endl;
		gMbsSetup->fLofTriggerModes.Print(cerr, "   > ");
		return(kFALSE);
	}

	mod = trigMode->GetName();
	mod += "(";
	mod += trigMode->GetIndex();
	mod += ")";
	gMbsSetup->Set(gMbsSetup->Resource(r, "Readout", fId + 1, "TriggerModule.Mode"), mod.Data());
	return(kTRUE);
}

Bool_t TMbsTriggerModule::SetTriggerMode(EMbsTriggerMode Mode) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsTriggerModule::SetTriggerMode
// Purpose:        Set trigger mode
// Arguments:      EMbsTriggerMode Mode    -- trigger mode
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines resource "TMbsSetup.ReadoutNNN.TriggerModule.Mode".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;
	TMrbNamedX * trigMode;
	TMrbString mod;

	if (fId + 1 == 0) return(kFALSE);

	trigMode = gMbsSetup->fLofTriggerModes.FindByIndex(Mode);
	if (trigMode == NULL) {
		gMrbLog->Err() << "Wrong mode - " << Mode << endl;
		gMrbLog->Flush(this->ClassName(), "SetTriggerMode");
		cerr	<< this->ClassName() << "::SetTriggerMode(): Legal modes are - "
				<< endl;
		gMbsSetup->fLofTriggerModes.Print(cerr, "   > ");
		return(kFALSE);
	}

	mod = trigMode->GetName();
	mod += "(";
	mod += trigMode->GetIndex();
	mod += ")";
	gMbsSetup->Set(gMbsSetup->Resource(r, "Readout", fId + 1, "TriggerModule.Mode"), mod.Data());
	return(kTRUE);
}

TMrbNamedX * TMbsTriggerModule::GetTriggerMode() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsTriggerModule::GetTriggerMode
// Purpose:        Read trigger mode
// Arguments:      --
// Results:        TMrbNamedX * Mode
// Exceptions:
// Description:    Reads resource "TMbsSetup.ReadoutNNN.TriggerModule.Mode".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;
	TMrbNamedX * trigMode;
	TString resValue;
	Int_t n;

	gMbsSetup->Get(resValue, gMbsSetup->Resource(r, "Readout", fId + 1, "TriggerModule.Mode"));
	if ((n = resValue.Index("(")) >= 0) resValue = resValue(0, n);
	trigMode = gMbsSetup->fLofTriggerModes.FindByName(resValue, TMrbLofNamedX::kFindExact | TMrbLofNamedX::kFindIgnoreCase);
	return(trigMode);
}

Bool_t  TMbsTriggerModule::SetConversionTime(Int_t ConvTime) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsTriggerModule::SetConversionTime
// Purpose:        Define conversion time
// Arguments:      Int_t ConvTime    -- conversion time
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets resource "TMbsSetup.ReadoutXXX.TriggerModule.ConvTime".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;

	if (fId < 0) return(kFALSE);

	gMbsSetup->Set(gMbsSetup->Resource(r, "Readout", fId + 1, "TriggerModule.ConvTime"), ConvTime);
	return(kTRUE);
}

Int_t TMbsTriggerModule::GetConversionTime() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsTriggerModule::GetConversionTime
// Purpose:        Return conversion time
// Arguments:      --
// Results:        Int_t ConvTime   -- conversion time
// Exceptions:
// Description:    Reads resource "TMbsSetup.ReadoutNNN.TriggerModule.ConvTime".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;
	return(gMbsSetup->Get(gMbsSetup->Resource(r, "Readout", fId + 1, "TriggerModule.ConvTime"), 0));
}

Bool_t TMbsTriggerModule::SetFastClearTime(Int_t FastClearTime) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsTriggerModule::SetFastClearTime
// Purpose:        Define FCT value
// Arguments:      Int_t FastClearTime    -- FCT value
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets resource "TMbsSetup.ReadoutXXX.TriggerModule.FastClearTime".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;

	if (fId < 0) return(kFALSE);

	gMbsSetup->Set(gMbsSetup->Resource(r, "Readout", fId + 1, "TriggerModule.FastClearTime"), FastClearTime);
	return(kTRUE);
}

Int_t TMbsTriggerModule::GetFastClearTime() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsTriggerModule::GetFastClearTime
// Purpose:        Return FCT value
// Arguments:      --
// Results:        Int_t FastClearTime   -- FCT value
// Exceptions:
// Description:    Reads resource "TMbsSetup.ReadoutNNN.TriggerModule.FastClearTime".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString r;
	return(gMbsSetup->Get(gMbsSetup->Resource(r, "Readout", fId + 1, "TriggerModule.FastClearTime"), 0));
}

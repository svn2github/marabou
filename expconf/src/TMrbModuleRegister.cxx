//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbModuleRegister.cxx
// Purpose:        MARaBOU configuration: module registers
// Description:    Implements class methods to describe a register
//                 of a CAMAC/VME module
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbModuleRegister.cxx,v 1.5 2004-09-28 13:47:32 rudi Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TEnv.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbLogger.h"
#include "TMrbConfig.h"
#include "TMrbModuleRegister.h"

#include "SetColor.h"

extern class TMrbConfig * gMrbConfig;
extern class TMrbLogger * gMrbLog;

ClassImp(TMrbModuleRegister)

TMrbModuleRegister::TMrbModuleRegister(TMrbModule * Module, Int_t NofChannels, TMrbNamedX * RegDef,
															Int_t InitValue, Int_t LowerLimit, Int_t UpperLimit,
															TMrbLofNamedX * BitNames, Bool_t PatternMode,
															EMrbRegisterAccess AccessMode) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModuleRegister
// Purpose:        Create a module register
// Arguments:      TMrbModule * Module            -- addr of parent module
//                 Int_t NofChannels              -- number of channels (0 = register is common)
//                 TMrbNamedX * RegDef               -- register index
//                 Int_t InitValue                -- initial value
//                 Int_t LowerLimit               -- lower limit
//                 Int_t UpperLimit               -- upper limit
//                 TMrbLofNamedX * BitNames      -- list of possible bit values
//                 Bool_t PatternMode             -- kTRUE if bitwise values
// Results:        --
// Exceptions:
// Description:    Defines a module register.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	fParent = Module;
	if (NofChannels == -1) NofChannels = fParent->GetNofChannels();
	fNofChannels = NofChannels;
	if (fNofChannels == 0) fValues.Set(1); else fValues.Set(fNofChannels);
	fRegDef = RegDef;
	fInitValue = InitValue;
	fLowerLimit = LowerLimit;
	if (UpperLimit == -1) UpperLimit = fParent->GetRange() - 1;
	fUpperLimit = UpperLimit;

	fAccessMode = AccessMode;
	
	Reset();

	fLofBitNames = BitNames;
	fPatternMode = kFALSE;
	if (fLofBitNames) {
		if (PatternMode) {
			fLofBitNames->SetPatternMode();
			fPatternMode = kTRUE;
		}
	}
}

Bool_t TMrbModuleRegister::SetFromResource(Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModuleRegister::SetFromResource
// Purpose:        Set register value from resource data base
// Arguments:      Int_t Value       -- default register value
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets value from gEnv resource
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t regValue;
	TString res, resn, mname;
	TMrbString rVal;
	
	res = this->Parent()->ClassName();
	res += ".";
	resn = res;
	mname = this->Parent()->GetName();
	mname(0,1).ToUpper();
	resn += mname;
	resn += ".";
	resn += this->GetName();
	res += this->GetName();

	rVal = gEnv->GetValue(res.Data(), "");
	rVal = rVal.Strip(TString::kBoth);
	if (rVal.Length() == 0) regValue = Value; else rVal.ToInteger(regValue);

	rVal = gEnv->GetValue(resn.Data(), "");
	rVal = rVal.Strip(TString::kBoth);
	if (rVal.Length() != 0) rVal.ToInteger(regValue);

	if (this->IsCommon())	return(this->Set(regValue));
	else					return(this->Set(-1, regValue));
}

Bool_t TMrbModuleRegister::Set(Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModuleRegister::Set
// Purpose:        Set register value
// Arguments:      Int_t Value       -- register value
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets value for a common register
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->IsReadOnly()) {
		gMrbLog->Err()	<< "[" << fParent->GetName() << "] "
						<< this->GetName() << " is READONLY - can't be set" << endl;
		gMrbLog->Flush(this->ClassName(), "Set");
		return(kFALSE);
	}
	
	if (this->IsCommon()) {
		if (Value < fLowerLimit || Value > fUpperLimit) {
			gMrbLog->Err()	<< "[" << fParent->GetName() << "] "
							<< this->GetName() << ": Value out of limits - " << Value
							<< ", should be in [" << fLowerLimit << "," << fUpperLimit << "]" << endl;
			gMrbLog->Flush(this->ClassName(), "Set");
			return(kFALSE);
		}
		fValues[0] = Value;
		return(kTRUE);
	} else {
		gMrbLog->Err()	<< "[" << fParent->GetName() << "] "
						<< this->GetName() << " is NOT COMMON, channel number missing" << endl;
		gMrbLog->Flush(this->ClassName(), "Set");
		return(kFALSE);
	}
}

Bool_t TMrbModuleRegister::Set(Int_t Channel, Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModuleRegister::Set
// Purpose:        Set register value
// Arguments:      Int_t Channel     -- channel number (-1: all channels)
//                 Int_t Value       -- register value
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets value for a channel-oriented register
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->IsReadOnly()) {
		gMrbLog->Err()	<< "[" << fParent->GetName() << "] "
						<< this->GetName() << " is READONLY - can't be set" << endl;
		gMrbLog->Flush(this->ClassName(), "Set");
		return(kFALSE);
	}
	
	if (this->IsPerChannel()) {
		if (Value < fLowerLimit || Value > fUpperLimit) {
			gMrbLog->Err()	<< "[" << fParent->GetName() << "] "
							<< this->GetName() << ": Value out of limits - " << Value
							<< ", should be in [" << fLowerLimit << "," << fUpperLimit << "]" << endl;
			gMrbLog->Flush(this->ClassName(), "Set");
			return(kFALSE);
		}
		if (Channel == -1) {
			for (Int_t i = 0; i < fNofChannels; i++) fValues[i] = Value;
		} else {
			if (fParent->GetChannel(Channel) == NULL) {
				gMrbLog->Err() << "[" << fParent->GetName() << "] "
							<< this->GetName() << ": Illegal channel - " << Channel << endl;
				gMrbLog->Flush(this->ClassName(), "Set");
				return(kFALSE);
			}
			fValues[Channel] = Value;
			return(kTRUE);
		}
	} else {
		gMrbLog->Err()	<< "[" << fParent->GetName() << "] "
						<< this->GetName() << " is COMMON, can't be addressed per channel" << endl;
		gMrbLog->Flush(this->ClassName(), "Set");
		return(kFALSE);
	}
	return(kFALSE);
}

Bool_t TMrbModuleRegister::Set(const Char_t * Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModuleRegister::Set
// Purpose:        Set register value
// Arguments:      Char_t * Value       -- string of bit values
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets value for a common register
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->IsReadOnly()) {
		gMrbLog->Err()	<< "[" << fParent->GetName() << "] "
						<< this->GetName() << " is READONLY - can't be set" << endl;
		gMrbLog->Flush(this->ClassName(), "Set");
		return(kFALSE);
	}
	
	if (this->IsCommon()) {
		if (this->HasBitNames()) {
			Int_t value;
			if (this->IsPatternMode()) {
				value = fLofBitNames->CheckPatternShort(this->ClassName(), "Set", Value);
			} else {
				TMrbNamedX * kp = fLofBitNames->FindByName(Value);
				if (kp == NULL) value = TMrbLofNamedX::kIllIndexBit;
				else			value = kp->GetIndex();
			}
			if (value == TMrbLofNamedX:: kIllIndexBit) {
				gMrbLog->Err()	<< "[" << fParent->GetName() << "] "
								<< this->GetName() << ": Illegal register value - " << Value << endl;
				gMrbLog->Flush(this->ClassName(), "Set");
				return(kFALSE);
			} else {
				return(this->Set(value));
			}
		} else {
			gMrbLog->Err()	<< "[" << fParent->GetName() << "] "
							<< this->GetName() << ": No bit names defined - can't resolve \"" << Value << "\"" << endl;
			gMrbLog->Flush(this->ClassName(), "Set");
			return(kFALSE);
		}
	} else {
		gMrbLog->Err()	<< "[" << fParent->GetName() << "] "
						<< this->GetName() << " is NOT COMMON, channel number missing" << endl;
		gMrbLog->Flush(this->ClassName(), "Set");
		return(kFALSE);
	}
}

Bool_t TMrbModuleRegister::Set(Int_t Channel, const Char_t * Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModuleRegister::Set
// Purpose:        Set register value
// Arguments:      Int_t Channel     -- channel number (-1: all channels)
//                 Int_t Value       -- register value
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets value for a channel-oriented register
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->IsReadOnly()) {
		gMrbLog->Err()	<< "[" << fParent->GetName() << "] "
						<< this->GetName() << " is READONLY - can't be set" << endl;
		gMrbLog->Flush(this->ClassName(), "Set");
		return(kFALSE);
	}
	
	if (this->IsPerChannel()) {
		if (this->HasBitNames()) {
			Int_t value;
			if (this->IsPatternMode()) {
				value = fLofBitNames->CheckPatternShort(this->ClassName(), "Set", Value);
			} else {
				TMrbNamedX * kp = fLofBitNames->FindByName(Value);
				if (kp == NULL) value = TMrbLofNamedX::kIllIndexBit;
				else			value = kp->GetIndex();
			}
			if (value == TMrbLofNamedX:: kIllIndexBit) {
				gMrbLog->Err()	<< "[" << fParent->GetName() << "] "
								<< this->GetName() << ": Illegal register value - " << Value << endl;
				gMrbLog->Flush(this->ClassName(), "Set");
				return(kFALSE);
			} else {
				return(this->Set(value));
			}
		} else {
			gMrbLog->Err()	<< "[" << fParent->GetName() << "] "
							<< this->GetName() << ": No bit names defined - can't resolve \"" << Value << "\"" << endl;
			gMrbLog->Flush(this->ClassName(), "Set");
			return(kFALSE);
		}
	} else {
		gMrbLog->Err()	<< "[" << fParent->GetName() << "] "
						<< this->GetName() << " is COMMON, can't be addressed per channel" << endl;
		gMrbLog->Flush(this->ClassName(), "Set");
		return(kFALSE);
	}
}

Int_t TMrbModuleRegister::Get(Int_t Channel) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModuleRegister::Get
// Purpose:        Get register value
// Arguments:      Int_t Channel      -- channel number (-1: common register)
// Results:        Int_t Value        -- register value
// Exceptions:
// Description:    Reads a register value
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Channel == -1) {
		if (this->IsPerChannel()) {
			gMrbLog->Err()	<< "[" << fParent->GetName() << "] "
							<< this->GetName() << " is NOT COMMON, channel number missing" << endl;
			gMrbLog->Flush(this->ClassName(), "Get");
			return(-1);
		} else {
			return(fValues[0]);
		}
	} else {
		if (this->IsCommon()) {
			gMrbLog->Err()	<< "[" << fParent->GetName() << "] "
							<< this->GetName() << " is COMMON, can't be addressed per channel" << endl;
			gMrbLog->Flush(this->ClassName(), "Get");
			return(-1);
		} else if (fParent->GetChannel(Channel) == NULL) {
				gMrbLog->Err() << this->GetName() << ": Illegal channel - " << Channel << endl;
				gMrbLog->Flush(this->ClassName(), "Get");
				return(-1);
		} else {
			return(fValues[Channel]);
		}
	}
}

void TMrbModuleRegister::Print(ostream & OutStrm, const Char_t * Prefix) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModuleRegister::Print
// Purpose:        Print settings
// Arguments:      ostream OutStrm     -- output stream
//                 Char_t * Prefix     -- prefix
// Results:        --
// Exceptions:
// Description:    Outputs register settings.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	TString accessMode;
	
	accessMode = "";
	if (this->IsReadOnly()) accessMode = "ReadOnly"; else if (this->IsWriteOnly()) accessMode = "WriteOnly";
	
	OutStrm << Prefix << "Register name  : "	<< this->GetName() << endl;
	OutStrm << Prefix << "Parent module  : "	<< this->Parent()->GetName() << endl;
	OutStrm << Prefix << "Type           : "	<< (this->Parent()->IsCamac() ? "CAMAC " : "VME ")
												<< (this->IsCommon() ? "Common " : "per Channel ")
												<< accessMode << endl;
	if (this->IsPerChannel()) OutStrm << Prefix << "Channels       : "	<< this->GetNofChannels() << endl;

	OutStrm << Prefix << "Initial value  : ";
	if (this->IsPatternMode())	OutStrm	<< setiosflags(ios::showbase) << setbase(16)
										<< fInitValue
										<< resetiosflags(ios::showbase) << setbase(10);
	else						OutStrm	<< fInitValue;
	if (this->HasBitNames()) fLofBitNames->PrintNames(OutStrm, " = ", fInitValue, kFALSE);
	OutStrm << endl;

	if (!this->IsPatternMode()) {
		OutStrm << Prefix << "Lower limit    : "	<< fLowerLimit << endl;
		OutStrm << Prefix << "Upper limit    : "	<< fUpperLimit << endl;
	}

	if (this->IsCommon()) {
		OutStrm << Prefix << "Current value  : ";
			if (this->IsPatternMode())	OutStrm	<< setiosflags(ios::showbase) << setbase(16)
												<< fValues[0]
												<< resetiosflags(ios::showbase) << setbase(10);
			else						OutStrm	<< fValues[0];
		if (this->HasBitNames()) fLofBitNames->PrintNames(OutStrm, " = ", fValues[0], kFALSE);
		OutStrm << endl;
	} else {
		OutStrm << Prefix << "Current values : ";
		for (i = 0; i < fNofChannels; i++) {
			OutStrm << Prefix << "                    " << setw(3) << i << ": ";
			if (this->IsPatternMode())	OutStrm	<< setiosflags(ios::showbase) << setbase(16)
												<< fValues[i]
												<< resetiosflags(ios::showbase) << setbase(10);
			else						OutStrm	<< fValues[i];
			if (this->HasBitNames()) fLofBitNames->PrintNames(OutStrm, " = ", fValues[i], kFALSE);
			OutStrm << endl;
		}
	}
}

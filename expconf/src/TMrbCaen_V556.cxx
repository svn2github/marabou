//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbCaen_V556.cxx
// Purpose:        MARaBOU configuration: CAEN modules
// Description:    Implements class methods to handle a CAEN adc type V556 
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TDirectory.h"

#include "TMrbLogger.h"
#include "TMrbConfig.h"
#include "TMrbVMEChannel.h"
#include "TMrbVMERegister.h"
#include "TMrbCaen_V556.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbCaen_V556)

TMrbCaen_V556::TMrbCaen_V556(const Char_t * ModuleName, UInt_t BaseAddr) :
									TMrbVMEModule(ModuleName, "Caen_V556", BaseAddr,
																TMrbCaen_V556::kAddrMod,
																TMrbCaen_V556::kSegSize,
																1, 8, 1 << 12) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V556
// Purpose:        Create an adc of type Caen V556
// Arguments:      Char_t * ModuleName      -- name of camac module
//                 UInt_t BaseAddr          -- base addr
// Results:        --
// Exceptions:
// Description:    Creates a vme adc of type CAEN V556.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString codeFile;
	UInt_t mTypeBits;
	TString mType;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	if (!this->IsZombie()) {
		if (gMrbConfig == NULL) {
			gMrbLog->Err() << "No config defined" << endl;
			gMrbLog->Flush(this->ClassName());
			this->MakeZombie();
		} else if (gMrbConfig->FindModule(ModuleName)) {
			gMrbLog->Err() << ModuleName << ": Module name already in use" << endl;
			gMrbLog->Flush(this->ClassName());
			this->MakeZombie();
		} else if (gMrbConfig->CheckModuleAddress(this)) {
			SetTitle("CAEN V556 ADC 8 x 12 bit"); 	// store module type
			mTypeBits = TMrbConfig::kModuleVME | TMrbConfig::kModuleListMode;
			gMrbConfig->GetLofModuleTypes()->Pattern2String(mType, mTypeBits);
			fModuleType.Set(mTypeBits, mType.Data());
			fDataType = gMrbConfig->GetLofDataTypes()->FindByIndex(TMrbConfig::kDataUShort);
			fNofShortsPerChannel = 1;
			fFFMode = kFALSE;
			codeFile = fModuleID.GetName();
			codeFile += ".code";
			if (LoadCodeTemplates(codeFile)) {
				DefineRegisters();
				gMrbConfig->AddModule(this);				// append to list of modules
				gDirectory->Append(this);
				fBlockReadout = kTRUE;						// block readout only
			} else {
				this->MakeZombie();
			}
		} else {
			this->MakeZombie();
		}
	}
}

void TMrbCaen_V556::DefineRegisters() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V556::DefineRegisters
// Purpose:        Define camac registers
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Defines status, common threshold, lower & upper thresholds, and offset.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	TMrbVMERegister * rp;

// control
	kp = new TMrbNamedX(TMrbCaen_V556::kRegControl, "Control");
	rp = new TMrbVMERegister(this, 0, kp,
							TMrbCaen_V556::kOffsControl,
							TMrbCaen_V556::kOffsControl,
							TMrbCaen_V556::kOffsControl,
							0, 0, 0xffff);
	rp->SetFromResource(0);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

// lower threshold
	kp = new TMrbNamedX(TMrbCaen_V556::kRegLowerThresh, "LowerThresh");
	rp = new TMrbVMERegister(this, 0, kp,
							TMrbCaen_V556::kOffsTHRL,
							TMrbCaen_V556::kOffsTHRL,
							TMrbCaen_V556::kOffsTHRL,
							0, 0, 255);
	rp->SetFromResource(0);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

// upper threshold
	kp = new TMrbNamedX(TMrbCaen_V556::kRegUpperThresh, "UpperThresh");
	rp = new TMrbVMERegister(this, 0, kp,
							TMrbCaen_V556::kOffsTHRH,
							TMrbCaen_V556::kOffsTHRH,
							TMrbCaen_V556::kOffsTHRH,
							0xc6, 0, 0xc6);
	rp->SetFromResource(0xc6);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

// delay
	kp = new TMrbNamedX(TMrbCaen_V556::kRegDelay, "Delay");
	rp = new TMrbVMERegister(this, 0, kp,
							TMrbCaen_V556::kOffsDelay,
							TMrbCaen_V556::kOffsDelay,
							TMrbCaen_V556::kOffsDelay,
							0, 0, 0x7);
	rp->SetFromResource(0);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
}

Bool_t TMrbCaen_V556::MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V556::MakeReadoutCode
// Purpose:        Write a piece of code for a caen adc
// Arguments:      ofstream & RdoStrm         -- file output stream
//                 EMrbModuleTag TagIndex     -- index of tag word taken from template file
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a caen v556 adc module.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString mnemoLC, mnemoUC;

	if (!fCodeTemplates.FindCode(TagIndex)) {
		gMrbLog->Err()	<< "No code loaded for tag "
						<< gMrbConfig->fLofModuleTags.FindByIndex(TagIndex)->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "MakeReadoutCode");
		return(kFALSE);
	}

	mnemoLC = this->GetMnemonic();
	mnemoUC = mnemoLC;
	mnemoUC.ToUpper();

	switch (TagIndex) {
		case TMrbConfig::kModuleDefs:
		case TMrbConfig::kModuleInitModule:
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.Substitute("$moduleTitle", this->GetTitle());
			fCodeTemplates.Substitute("$modulePosition", this->GetPosition());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$baseAddr", (Int_t) this->GetBaseAddr(), 16);
			fCodeTemplates.Substitute("$chnPattern", (Int_t) this->GetPatternOfChannelsUsed(), 16);
			fCodeTemplates.Substitute("$lowerThresh", (Int_t) this->Get(TMrbCaen_V556::kRegLowerThresh), 16);
			fCodeTemplates.Substitute("$upperThresh", (Int_t) this->Get(TMrbCaen_V556::kRegUpperThresh), 16);
			fCodeTemplates.WriteCode(RdoStrm);
			break;
		case TMrbConfig::kModuleClearModule:
		case TMrbConfig::kModuleReadModule:
		case TMrbConfig::kModuleFinishReadout:
		case TMrbConfig::kModuleStartAcquisition:
		case TMrbConfig::kModuleStopAcquisition:
		case TMrbConfig::kModuleStartAcquisitionGroup:
		case TMrbConfig::kModuleStopAcquisitionGroup:
		case TMrbConfig::kModuleUtilities:
		case TMrbConfig::kModuleDefineGlobals:
		case TMrbConfig::kModuleDefineLocalVarsInit:
		case TMrbConfig::kModuleDefineLocalVarsReadout:
		case TMrbConfig::kModuleDefinePrototypes:
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$nofParams", this->GetNofChannelsUsed());
			fCodeTemplates.Substitute("$chnPattern", (Int_t) this->GetPatternOfChannelsUsed(), 16);
			fCodeTemplates.WriteCode(RdoStrm);
			break;
	}
	return(kTRUE);
}


Bool_t TMrbCaen_V556::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbModuleTag TagIndex,
															TObject * Channel,
															Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V556::MakeReadoutCode
// Purpose:        Write a piece of code for a caen adc
// Arguments:      ofstream & RdoStrm           -- file output stream
//                 EMrbModuleTag TagIndex       -- index of tag word taken from template file
//                 TObject * Channel            -- channel
//                 Int_t Value                  -- value to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a caen v556 adc module.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbVMEChannel * chn;
	TString mnemoLC, mnemoUC;

	chn = (TMrbVMEChannel *) Channel;

	if (!fCodeTemplates.FindCode(TagIndex)) {
		gMrbLog->Err()	<< "No code loaded for tag "
						<< gMrbConfig->fLofModuleTags.FindByIndex(TagIndex)->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "MakeReadoutCode");
		return(kFALSE);
	}

	mnemoLC = this->GetMnemonic();
	mnemoUC = mnemoLC;
	mnemoUC.ToUpper();

	switch (TagIndex) {
		case TMrbConfig::kModuleInitChannel:
			break;
		case TMrbConfig::kModuleWriteSubaddr:
			break;
		case TMrbConfig::kModuleSetupReadout:
		case TMrbConfig::kModuleReadChannel:
		case TMrbConfig::kModuleIncrementChannel:
		case TMrbConfig::kModuleSkipChannels:
		case TMrbConfig::kModuleStoreData:
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$chnName", chn->GetName());
			fCodeTemplates.Substitute("$chnNo", chn->GetAddr());
			fCodeTemplates.Substitute("$data", Value);
			fCodeTemplates.WriteCode(RdoStrm);
			break;
	}
	return(kTRUE);
}

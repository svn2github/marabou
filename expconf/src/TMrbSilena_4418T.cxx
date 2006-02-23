//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbSilena_4418T.cxx
// Purpose:        MARaBOU configuration: Silena TDCs
// Description:    Implements class methods to handle Silena type 4418T
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbSilena_4418T.cxx,v 1.6 2006-02-23 09:28:49 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TDirectory.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbLogger.h"
#include "TMrbConfig.h"
#include "TMrbCamacChannel.h"
#include "TMrbCamacRegister.h"
#include "TMrbSilena_4418T.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbSilena_4418T)

const SMrbNamedX kMrbLofStatusBits[] =
		{
			{TMrbSilena_4418T::kStatusSUB,		"SUB",	"Subaddr disabled"				},
			{TMrbSilena_4418T::kStatusEEN,		"EEN",	"ECL enabled"					},
			{TMrbSilena_4418T::kStatusOVF,		"OVF",	"Overflow disabled" 			},
			{TMrbSilena_4418T::kStatusCCE,		"CCE",	"Zero suppression enabled"		},
			{TMrbSilena_4418T::kStatusCSR,		"CSR",	"Sequential readout enabled"	},
			{TMrbSilena_4418T::kStatusCLE,		"CLE",	"LAM enabled"					},
			{~TMrbSilena_4418T::kStatusSUB, 	"~SUB", "Subaddr enabled"				},
			{~TMrbSilena_4418T::kStatusEEN, 	"~EEN", "ECL disabled"					},
			{~TMrbSilena_4418T::kStatusOVF, 	"~OVF", "Overflow enabled"				},
			{~TMrbSilena_4418T::kStatusCCE, 	"~CCE", "Zero suppression disabled" 	},
			{~TMrbSilena_4418T::kStatusCSR, 	"~CSR", "Sequential readout disabled"	},
			{~TMrbSilena_4418T::kStatusCLE, 	"~CLE", "LAM disabled"					},
			{0, 								NULL									}
		};

TMrbSilena_4418T::TMrbSilena_4418T(const Char_t * ModuleName, const Char_t * ModulePosition) :
											TMrbCamacModule(ModuleName, "Silena_4418T", ModulePosition, 0, 8, 4096) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSilena_4418T
// Purpose:        Create an ADC of type Silena 4418/T
// Arguments:      Char_t * ModuleName      -- name of camac module
//                 Char_t * ModulePosition  -- B.C.N for this module
// Results:        --
// Exceptions:
// Description:    Creates a camac module of type Silena 4418/V.
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
		} else {
			SetTitle("Silena 4418/T TDC 8 x 4096 50ps/ch"); 	// store module type
			mTypeBits = TMrbConfig::kModuleCamac | TMrbConfig::kModuleListMode;
			gMrbConfig->GetLofModuleTypes()->Pattern2String(mType, mTypeBits);
			fModuleType.Set(mTypeBits, mType.Data());

			codeFile = fModuleID.GetName();
			codeFile += ".code";
			if (LoadCodeTemplates(codeFile)) {
				DefineRegisters();							// define module regs
				gMrbConfig->AddModule(this);				// append to list of modules
				gDirectory->Append(this);
				fZeroSuppression = kFALSE;
			} else {
				this->MakeZombie();
			}
		}
	}
}

Bool_t TMrbSilena_4418T::CheckSubeventType(TObject * Subevent) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSilena_4418T::CheckSubeventType
// Purpose:        Check if calling subevent is applicable
// Arguments:      
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    If zero suppression is turned on make sure that
//                 a subevent of type [10,3x] (Silena 4418 with zero suppr)
//                 is calling.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbSubevent *sevt;

	if (!this->HasZeroSuppression()) return(kTRUE);
	
	sevt = (TMrbSubevent *) Subevent;
	if (sevt->GetType() != 10 || sevt->GetSubtype() < 31 || sevt->GetSubtype() > 39)	return(kFALSE);
	else																				return(kTRUE);
}

void TMrbSilena_4418T::DefineRegisters() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSilena_4418T::DefineRegisters
// Purpose:        Define module registers
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Defines status, lower, upper threshold, and offset.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	TMrbLofNamedX * bNames;
	TMrbCamacRegister * rp;

// status
	kp = new TMrbNamedX(TMrbSilena_4418T::kRegStatus, "Status");
	rp = new TMrbCamacRegister(this, 0, kp, "F20.A14", "F20.A14", "F4.A14", 0x200, 0, 0xffff);
	rp->SetFromResource(TMrbSilena_4418T::kDefaultStatus);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	bNames = new TMrbLofNamedX();
	bNames->SetName("Status Bits");
	bNames->AddNamedX(kMrbLofStatusBits);	
	bNames->SetPatternMode();
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode();

// lower threshold
	kp = new TMrbNamedX(TMrbSilena_4418T::kRegLowerThresh, "LowerThresh");
	rp = new TMrbCamacRegister(this, fNofChannels, kp, "F17.A8", "F17.A8", "F1.A0", 0, 0, 255);
	rp->SetFromResource(0);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

// upper threshold
	kp = new TMrbNamedX(TMrbSilena_4418T::kRegUpperThresh, "UpperThresh");
	rp = new TMrbCamacRegister(this, fNofChannels, kp, "F17.A0", "F17.A0", "F1.A0", 255, 0, 255);
	rp->SetFromResource(255);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

// offset
	kp = new TMrbNamedX(TMrbSilena_4418T::kRegOffset, "Offset");
	rp = new TMrbCamacRegister(this, fNofChannels, kp, "F20.A0", "F20.A0", "F4.A0", 140, 0, 255);
	rp->SetFromResource(TMrbSilena_4418T::kDefaultOffset);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
}

Bool_t TMrbSilena_4418T::MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSilena_4418T::MakeReadoutCode
// Purpose:        Write a piece of code for a silena adc
// Arguments:      ofstream & RdoStrm         -- file output stream
//                 EMrbModuleTag TagIndex      -- index of tag word taken from template file
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a silena 4418/V module.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	TMrbCamacChannel *chn;
	TString mnemoLC, mnemoUC;

	Int_t statusReg;
	
	if (!fCodeTemplates.FindCode(TagIndex)) {
		gMrbLog->Err()	<< "No code loaded for tag "
						<< gMrbConfig->fLofModuleTags.FindByIndex(TagIndex)->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "MakeReadoutCode");
		return(kFALSE);
	}

	mnemoLC = this->GetMnemonic();
	mnemoUC = mnemoLC;
	mnemoUC.ToUpper();

	statusReg = this->Get(TMrbSilena_4418T::kRegStatus) + this->GetSerial();
	if (this->HasZeroSuppression()) statusReg |= TMrbSilena_4418T::kZeroCompressionStatus;
		
	switch (TagIndex) {
		case TMrbConfig::kModuleInitCommonCode:
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.Substitute("$moduleTitle", this->GetTitle());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$modulePosition", this->GetPosition());
			fCodeTemplates.WriteCode(RdoStrm);
			break;
		case TMrbConfig::kModuleInitModule:
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.Substitute("$moduleTitle", this->GetTitle());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$modulePosition", this->GetPosition());
			fCodeTemplates.Substitute("$status", this->Get(TMrbSilena_4418T::kRegStatus), 16);
			fCodeTemplates.WriteCode(RdoStrm);
			for (i = 0; i < fNofChannels; i++, chn++) {
				chn = (TMrbCamacChannel *) fChannelSpec[i];
				if (chn->IsUsed()) {
					this->MakeReadoutCode(RdoStrm, TMrbConfig::kModuleInitChannel, chn);
				}
			}
			break;
		case TMrbConfig::kModuleClearModule:
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
			fCodeTemplates.WriteCode(RdoStrm);
			break;
	}
	return(kTRUE);
}


Bool_t TMrbSilena_4418T::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbModuleTag TagIndex,
																TObject * Channel,
																Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSilena_4418T::MakeReadoutCode
// Purpose:        Write a piece of code for a silena adc
// Arguments:      ofstream & rdo               -- file output stream
//                 EMrbModuleTag TagIndex       -- index of tag word taken from template file
//                 TObject * Channel            -- channel
//                 Int_t Value                  -- value to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a silena 4418/V module.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbCamacChannel * chn;
	TString mnemoLC, mnemoUC;

	chn = (TMrbCamacChannel *) Channel;

	mnemoLC = this->GetMnemonic();
	mnemoUC = mnemoLC;
	mnemoUC.ToUpper();

	if (!fCodeTemplates.FindCode(TagIndex)) {
		gMrbLog->Err()	<< "No code loaded for tag "
						<< gMrbConfig->fLofModuleTags.FindByIndex(TagIndex)->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "MakeReadoutCode");
		return(kFALSE);
	}

	switch (TagIndex) {
		case TMrbConfig::kModuleInitChannel:
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$chnName", chn->GetName());
			fCodeTemplates.Substitute("$chnNo", chn->GetAddr());
			fCodeTemplates.Substitute("$lowerThresh", chn->Get(TMrbSilena_4418T::kRegLowerThresh));
			fCodeTemplates.Substitute("$upperThresh", chn->Get(TMrbSilena_4418T::kRegUpperThresh));
			fCodeTemplates.Substitute("$offset", chn->Get(TMrbSilena_4418T::kRegOffset));
			fCodeTemplates.WriteCode(RdoStrm);
			break;
		case TMrbConfig::kModuleWriteSubaddr:
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$chnName", chn->GetName());
			fCodeTemplates.Substitute("$chnNo", chn->GetAddr());
			fCodeTemplates.Substitute("$subaddr", chn->GetAddr() + 1);
			fCodeTemplates.WriteCode(RdoStrm);
			break;
		case TMrbConfig::kModuleSetupReadout:
			fCodeTemplates.InitializeCode(this->HasZeroSuppression() ? "%ZS%" : "%RR%");
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$chnName", chn->GetName());
			fCodeTemplates.Substitute("$chnNo", chn->GetAddr());
			fCodeTemplates.Substitute("$data", Value);
			fCodeTemplates.WriteCode(RdoStrm);
			break;
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

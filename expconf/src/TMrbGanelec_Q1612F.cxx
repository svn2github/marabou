//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbGanelec_Q1612F.cxx
// Purpose:        MARaBOU configuration: Ganalec ADCs/QDCs
// Description:    Implements class methods to handle Ganelec type QDC1612F
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <iostream.h>
#include <strstream.h>
#include <iomanip.h>
#include <fstream.h>

#include "TDirectory.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbLogger.h"
#include "TMrbConfig.h"
#include "TMrbCamacChannel.h"
#include "TMrbCamacRegister.h"
#include "TMrbGanelec_Q1612F.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbGanelec_Q1612F)

const SMrbNamedX kMrbLofStatusBits[] =
		{
			{TMrbGanelec_Q1612F::kStatusEPS,		"EPS",	"ECL Pedestal subtr enabled"	},
			{TMrbGanelec_Q1612F::kStatusECE,		"ECE",	"ECL Data compression enabled"	},
			{TMrbGanelec_Q1612F::kStatusEEN,		"EEN",	"ECL enabled"					},
			{TMrbGanelec_Q1612F::kStatusCPS,		"CPS",	"CAMAC Pedestal subtr enabled"	},
			{TMrbGanelec_Q1612F::kStatusCCE,		"CCE",	"CAMAC Data compression enabled"},
			{TMrbGanelec_Q1612F::kStatusCSR,		"CSR",	"Sequential readout enabled"	},
			{TMrbGanelec_Q1612F::kStatusCLE,		"CLE",	"LAM enabled"					},
			{TMrbGanelec_Q1612F::kStatusOFS,		"OFS",	"Overflow subtr enabled"		},
			{TMrbGanelec_Q1612F::kStatusIND,		"IND",	"Individual gates enabled"		},
			{~TMrbGanelec_Q1612F::kStatusEEN,		"~EEN", "ECL disabled"					},
			{~TMrbGanelec_Q1612F::kStatusCSR,		"~CSR", "Random readout enabled"		},
			{~TMrbGanelec_Q1612F::kStatusCLE,		"~CLE", "LAM disabled"					},
			{~TMrbGanelec_Q1612F::kStatusIND,		"~IND", "Common gates enabled"			},
			{0, 									NULL,	NULL							}
		};

TMrbGanelec_Q1612F::TMrbGanelec_Q1612F(const Char_t * ModuleName, const Char_t * ModulePosition) :
									TMrbCamacModule(ModuleName, "Ganelec_Q1612F", ModulePosition, 0, 16, 4096) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbGanelec_Q1612F
// Purpose:        Create an ADC of type Ganelec QDC1612F
// Arguments:      Char_t * ModuleName      -- name of camac module
//                 Char_t * ModulePosition  -- B.C.N for this module
// Results:        --
// Exceptions:
// Description:    Creates a camac module of type Ganelec QDC1612F
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
			SetTitle("Ganelec QDC1612F QDC 2 x 8 x 4096"); 	// store module type
			mTypeBits = TMrbConfig::kModuleCamac | TMrbConfig::kModuleListMode;
			gMrbConfig->GetLofModuleTypes()->Pattern2String(mType, mTypeBits);
			fModuleType.Set(mTypeBits, mType.Data());

			codeFile = fModuleID.GetName();
			codeFile += ".code";
			if (LoadCodeTemplates(codeFile)) {
				DefineRegisters();
				gMrbConfig->AddModule(this);				// append to list of modules
				gDirectory->Append(this);
			} else {
				this->MakeZombie();
			}
		}
	}
}

void TMrbGanelec_Q1612F::DefineRegisters() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbGanelec_Q1612F::DefineRegisters
// Purpose:        Define camac registers
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Defines status, offsets, and gate
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	TMrbLofNamedX * bNames;
	TMrbCamacRegister * rp;

// status
	kp = new TMrbNamedX(TMrbGanelec_Q1612F::kRegStatus, "Status");
	rp = new TMrbCamacRegister(this, 0, kp, "F16.A0", "F16.A0", "F0.A0", TMrbGanelec_Q1612F::kStatusIND, 0, 0xffff);
	rp->SetFromResource(TMrbGanelec_Q1612F::kStatusIND);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	bNames = new TMrbLofNamedX();
	bNames->SetName("Status Bits");
	bNames->AddNamedX(kMrbLofStatusBits);	
	bNames->SetPatternMode();
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode();

// common offset (analog)
	kp = new TMrbNamedX(TMrbGanelec_Q1612F::kRegCommonOffset, "CommonOffset");
	rp = new TMrbCamacRegister(this, 0, kp, "F16.A3", "F16.A3", "F0.A3", TMrbGanelec_Q1612F::kDefaultCommonOffset, 0, 255);
	rp->SetFromResource(TMrbGanelec_Q1612F::kDefaultOffset);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

// individual offsets (digital)
	kp = new TMrbNamedX(TMrbGanelec_Q1612F::kRegOffset, "Offset");
	rp = new TMrbCamacRegister(this, fNofChannels, kp, "F17.A0", "F17.A0", "F1.A0", TMrbGanelec_Q1612F::kDefaultOffset, 0, 255);
	rp->SetFromResource(0);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

// master gate (part of status reg)
	kp = new TMrbNamedX(TMrbGanelec_Q1612F::kRegMasterGate, "MasterGate");
	rp = new TMrbCamacRegister(this, 0, kp, "F16.A0", "F16.A0", "F0.A0", TMrbGanelec_Q1612F::kDefaultGate, 1, 16);
	rp->SetFromResource(TMrbGanelec_Q1612F::kDefaultGate);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
}

Bool_t TMrbGanelec_Q1612F::MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbGanelec_Q1612F::MakeReadoutCode
// Purpose:        Write a piece of code for a gan'elec qdc
// Arguments:      ofstream & RdoStrm         -- file output stream
//                 EMrbModuleTag TagIndex     -- index of tag word taken from template file
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a gan'elec QDC1612F module.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	TMrbCamacChannel *chn;
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
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.Substitute("$moduleTitle", this->GetTitle());
			fCodeTemplates.Substitute("$modulePosition", this->GetPosition());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.WriteCode(RdoStrm);
			break;
		case TMrbConfig::kModuleInitModule:
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.Substitute("$moduleTitle", this->GetTitle());
			fCodeTemplates.Substitute("$modulePosition", this->GetPosition());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$status", this->Get(TMrbGanelec_Q1612F::kRegStatus), 16);
			fCodeTemplates.Substitute("$mgate", this->Get(TMrbGanelec_Q1612F::kRegMasterGate));
			fCodeTemplates.Substitute("$commonOffset", this->Get(TMrbGanelec_Q1612F::kRegCommonOffset));
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


Bool_t TMrbGanelec_Q1612F::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbModuleTag TagIndex,
															TObject * Channel,
															Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbGanelec_Q1612F::MakeReadoutCode
// Purpose:        Write a piece of code for a gan'elec adc
// Arguments:      ofstream & RdoStrm           -- file output stream
//                 EMrbModuleTag TagIndex       -- index of tag word taken from template file
//                 TObject * Channel            -- channel
//                 Int_t Value                  -- value to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a gan'elec QDC1612F module.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbCamacChannel * chn;
	TString mnemoLC, mnemoUC;

	chn = (TMrbCamacChannel *) Channel;

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
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$chnName", chn->GetName());
			fCodeTemplates.Substitute("$chnNo", chn->GetAddr());
			fCodeTemplates.Substitute("$offset", chn->Get(TMrbGanelec_Q1612F::kRegOffset));
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

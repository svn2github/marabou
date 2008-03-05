//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbOrtec_413A.cxx
// Purpose:        MARaBOU configuration: Ortec ADCs
// Description:    Implements class methods to handle Ortec type 413A
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbOrtec_413A.cxx,v 1.1 2008-03-05 12:37:16 Rudolf.Lutter Exp $       
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
#include "TMrbOrtec_413A.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbOrtec_413A)

const SMrbNamedX kMrbLofStatusBits[] =
		{
			{TMrbOrtec_413A::kStatusZSD,		"ZSD",	"Zero suppression disabled"				},
			{TMrbOrtec_413A::kStatusCRE,		"CRE",	"CAMAC readout enabled"					},
			{TMrbOrtec_413A::kStatusSME,		"SME",	"Singles mode enabled"		 			},
			{TMrbOrtec_413A::kStatusCRA,		"CRA",	"CAMAC random access enabled"			},
			{TMrbOrtec_413A::kStatusCLE,		"CLE",	"LAM enabled"							},
			{TMrbOrtec_413A::kStatusOSD,		"OSD",	"Overflow suppression disabled"			},
			{~TMrbOrtec_413A::kStatusZSD,		"~ZSD",	"Zero suppression enabled"				},
			{~TMrbOrtec_413A::kStatusCRE,		"~CRE",	"CAMAC readout disabled"				},
			{~TMrbOrtec_413A::kStatusSME,		"~SME",	"Singles mode disabled"		 			},
			{~TMrbOrtec_413A::kStatusCRA,		"~CRA",	"CAMAC random access disabled"			},
			{~TMrbOrtec_413A::kStatusCLE,		"~CLE",	"LAM disabled"							},
			{~TMrbOrtec_413A::kStatusOSD,		"~OSD",	"Overflow suppression enabled"			},
			{0, 								NULL											}
		};

const SMrbNamedX kMrbLofGateBits[] =
		{
			{TMrbOrtec_413A::kGateG1D,		"G1D",	"Individual gate 1 disabled"				},
			{TMrbOrtec_413A::kGateG2D,		"G2D",	"Individual gate 2 disabled"				},
			{TMrbOrtec_413A::kGateG3D,		"G3D",	"Individual gate 3 disabled"				},
			{TMrbOrtec_413A::kGateG4D,		"G4D",	"Individual gate 4 disabled"				},
			{TMrbOrtec_413A::kGateMGD,		"MGD",	"Master gate disabled"						},
			{~TMrbOrtec_413A::kGateG1D,		"~G1D",	"Individual gate 1 enabled"					},
			{~TMrbOrtec_413A::kGateG2D,		"~G2D",	"Individual gate 2 enabled"					},
			{~TMrbOrtec_413A::kGateG3D,		"~G3D",	"Individual gate 3 enabled"					},
			{~TMrbOrtec_413A::kGateG4D,		"~G4D",	"Individual gate 4 enabled"					},
			{~TMrbOrtec_413A::kGateMGD,		"~MGD",	"Master gate enabled"						},
			{0, 								NULL											}
		};

TMrbOrtec_413A::TMrbOrtec_413A(const Char_t * ModuleName, const Char_t * ModulePosition) :
									TMrbCamacModule(ModuleName, "Ortec_413A", ModulePosition, 0, 4, 8064) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbOrtec_413A
// Purpose:        Create an ADC of type Ortec 413A
// Arguments:      Char_t * ModuleName      -- name of camac module
//                 Char_t * ModulePosition  -- B.C.N for this module
// Results:        --
// Exceptions:
// Description:    Creates a camac module of type Ortec 413A.
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
			SetTitle("Ortec 413A ADC 4 x 8064"); 	// store module type
			mTypeBits = TMrbConfig::kModuleCamac | TMrbConfig::kModuleListMode;
			mType = gMrbConfig->GetLofModuleTypes()->Pattern2String(mType, mTypeBits);
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

void TMrbOrtec_413A::DefineRegisters() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbOrtec_413A::DefineRegisters
// Purpose:        Define module registers
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Defines status, lower threshold, and gate.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	TMrbLofNamedX * bNames;
	TMrbCamacRegister * rp;

// status
	kp = new TMrbNamedX(TMrbOrtec_413A::kRegStatus, "Status");
	rp = new TMrbCamacRegister(this, 0, kp, "F16.A0", "F16.A0", "F0.A0", TMrbOrtec_413A::kDefaultStatus, 0, 0xffff);
	rp->SetFromResource(TMrbOrtec_413A::kDefaultStatus);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	bNames = new TMrbLofNamedX();
	bNames->SetName("Status Bits (Control Reg 1)");
	bNames->AddNamedX(kMrbLofStatusBits);	
	bNames->SetPatternMode();
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode();

// gate
	kp = new TMrbNamedX(TMrbOrtec_413A::kRegGate, "Gate");
	rp = new TMrbCamacRegister(this, 0, kp, "F16.A1", "F16.A1", "F0.A1", TMrbOrtec_413A::kDefaultGate, 0, 0xffff);
	rp->SetFromResource(TMrbOrtec_413A::kDefaultGate);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	bNames = new TMrbLofNamedX();
	bNames->SetName("Gate Bits (Control Reg 2)");
	bNames->AddNamedX(kMrbLofGateBits);	
	bNames->SetPatternMode();
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode();

// lower threshold
	kp = new TMrbNamedX(TMrbOrtec_413A::kRegLowerThresh, "LowerThresh");
	rp = new TMrbCamacRegister(this, fNofChannels, kp, "F17.A0", "F17.A0", "F1.A0", 0, 0, 255);
	rp->SetFromResource(TMrbOrtec_413A::kDefaultLowerThresh);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

}

Bool_t TMrbOrtec_413A::MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbOrtec_413A::MakeReadoutCode
// Purpose:        Write a piece of code for an ortec adc
// Arguments:      ofstream & RdoStrm         -- file output stream
//                 EMrbModuleTag TagIndex     -- index of tag word taken from template file
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of an ortec AD413A module.
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
			{
				fCodeTemplates.InitializeCode();
				fCodeTemplates.Substitute("$moduleName", this->GetName());
				fCodeTemplates.Substitute("$moduleTitle", this->GetTitle());
				fCodeTemplates.Substitute("$modulePosition", this->GetPosition());
				fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
				fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
				Int_t sBits = this->Get(TMrbOrtec_413A::kRegStatus);
				TString str;
				TMrbLofNamedX snames;
				snames.AddNamedX(kMrbLofStatusBits);	
				snames.SetPatternMode();
				snames.Pattern2String(str, sBits);
				fCodeTemplates.Substitute("$status", sBits, 16);
				fCodeTemplates.Substitute("$sBits", str.Data());
				Int_t gBits = this->Get(TMrbOrtec_413A::kRegGate);
				TMrbLofNamedX gnames;
				gnames.AddNamedX(kMrbLofGateBits);	
				gnames.SetPatternMode();
				gnames.Pattern2String(str, gBits);
				fCodeTemplates.Substitute("$gate", gBits, 16);
				fCodeTemplates.Substitute("$gBits", str.Data());
				fCodeTemplates.WriteCode(RdoStrm);
				for (i = 0; i < fNofChannels; i++, chn++) {
					chn = (TMrbCamacChannel *) fChannelSpec[i];
					if (chn->IsUsed()) {
						this->MakeReadoutCode(RdoStrm, TMrbConfig::kModuleInitChannel, chn);
					}
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


Bool_t TMrbOrtec_413A::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbModuleTag TagIndex,
															TMrbCamacChannel * Channel,
															Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbOrtec_413A::MakeReadoutCode
// Purpose:        Write a piece of code for an ortec adc
// Arguments:      ofstream & RdoStrm           -- file output stream
//                 EMrbModuleTag TagIndex       -- index of tag word taken from template file
//                 TMrbCamacChannel * Channel   -- channel
//                 Int_t Value                  -- value to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of an ortec AD413A module.
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
		case TMrbConfig::kModuleInitChannel:
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$chnName", Channel->GetName());
			fCodeTemplates.Substitute("$chnNo", Channel->GetAddr());
			fCodeTemplates.Substitute("$lowerThresh", Channel->Get(TMrbOrtec_413A::kRegLowerThresh));
			fCodeTemplates.WriteCode(RdoStrm);
			break;
		case TMrbConfig::kModuleWriteSubaddr:
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$chnName", Channel->GetName());
			fCodeTemplates.Substitute("$chnNo", Channel->GetAddr());
			fCodeTemplates.Substitute("$subaddr", Channel->GetAddr() + 1);
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
			fCodeTemplates.Substitute("$chnName", Channel->GetName());
			fCodeTemplates.Substitute("$chnNo", Channel->GetAddr());
			fCodeTemplates.Substitute("$data", Value);
			fCodeTemplates.WriteCode(RdoStrm);
			break;
	}
	return(kTRUE);
}

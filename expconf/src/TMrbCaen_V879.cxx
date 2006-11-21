//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbCaen_V879.cxx
// Purpose:        MARaBOU configuration: CAEN modules
// Description:    Implements class methods to handle a CAEN adc/tac type V879 
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbCaen_V879.cxx,v 1.6 2006-11-21 12:43:05 Rudolf.Lutter Exp $       
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
#include "TMrbCaen_V879.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbCaen_V879)

const SMrbNamedX kMrbBitSet1Bits[] =
		{
			{TMrbCaen_V879::kBitSet1SelAddrIntern,			"AddrIntern",		"Select address via internal reg"					},
			{0, 											NULL,				NULL								}
		};

const SMrbNamedX kMrbControl1Bits[] =
		{
			{TMrbCaen_V879::kBitControl1ProgResetModule,	"ResetModule",		"Reset module"						},
			{~TMrbCaen_V879::kBitControl1ProgResetModule,	"ResetDataOnly",	"Reset data only"					},
			{0, 											NULL,				NULL								}
		};

const SMrbNamedX kMrbBitSet2Bits[] =
		{
			{TMrbCaen_V879::kBitSet2Offline,				"Offline",			"ADC offline"						},
			{TMrbCaen_V879::kBitSet2OverRangeDis,			"OverRangeDisable",	"Disable over range check"			},
			{TMrbCaen_V879::kBitSet2LowThreshDis,			"LowThreshDisable",	"Disable low threshold check"		},
			{TMrbCaen_V879::kBitSet2SlideEna,				"SlideEnable",		"Enable sliding scale"				},
			{TMrbCaen_V879::kBitSet2AutoIncrEna,			"AutoIncrEnable",	"Enable auto increment"				},
			{TMrbCaen_V879::kBitSet2EmptyProgEna,			"EmptyProgEnable",	"Write empty header & EOB"			},
			{TMrbCaen_V879::kBitSet2AllTrigEna, 			"AllTriggerEnable",	"Incr event cnt on ALL trigs"		},
			{0, 											NULL,				NULL								}
		};

TMrbCaen_V879::TMrbCaen_V879(const Char_t * ModuleName, UInt_t BaseAddr, Bool_t IsTac) :
									TMrbVMEModule(ModuleName, "Caen_V879", BaseAddr,
																TMrbCaen_V879::kAddrMod,
																TMrbCaen_V879::kSegSize,
																0, 32, 1 << 12) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V879
// Purpose:        Create a adc/tac of type Caen V879
// Arguments:      Char_t * ModuleName      -- name of camac module
//                 UInt_t BaseAddr          -- base addr
//                 Bool_t IsTac             -- kTRUE if TAC
// Results:        --
// Exceptions:
// Description:    Creates a vme adc/tac of type CAEN V879.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString codeFile;
	UInt_t mTypeBits;
	TString mType;
	TMrbString title;

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
			title = "CAEN V879 ";
			title += IsTac ? "TAC" : "ADC";
			title += " x 12 bit";
			SetTitle(title.Data()); 	// store module type
			mTypeBits = TMrbConfig::kModuleVME | TMrbConfig::kModuleListMode;
			gMrbConfig->GetLofModuleTypes()->Pattern2String(mType, mTypeBits);
			fModuleType.Set(mTypeBits, mType.Data());
			fDataType = gMrbConfig->GetLofDataTypes()->FindByIndex(TMrbConfig::kDataUInt);
			fNofShortsPerChannel = 2;
			fFFMode = kFALSE;
			fRangeSlope = 0;
			fRangeOffset = 0;
			fZeroSuppression = kTRUE;
			fOverRangeCheck = kTRUE;
			fIsTac = IsTac;
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
		}
	}
}

void TMrbCaen_V879::DefineRegisters() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V879::DefineRegisters
// Purpose:        Define module registers
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	TMrbLofNamedX * bNames;
	TMrbVMERegister * rp;

// bit set/clear 1
	kp = new TMrbNamedX(TMrbCaen_V879::kRegBitSet1, "BitSet1");
	rp = new TMrbVMERegister(this, 0, kp,
								TMrbCaen_V879::kOffsBitSet1,
								TMrbCaen_V879::kOffsBitClear1,
								TMrbCaen_V879::kOffsBitSet1,
								0, 0, 0xff);
	rp->SetFromResource(0);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	bNames = new TMrbLofNamedX();
	bNames->SetName("BitSet1");
	bNames->AddNamedX(kMrbBitSet1Bits);	
	bNames->SetPatternMode();
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode();

// control 1
	kp = new TMrbNamedX(TMrbCaen_V879::kRegControl1, "Control1");
	rp = new TMrbVMERegister(this, 0, kp,
								TMrbCaen_V879::kOffsControl1,
								TMrbCaen_V879::kOffsControl1,
								TMrbCaen_V879::kOffsControl1,
								TMrbCaen_V879::kBitControl1ProgResetModule, 0, 0x3f);
	rp->SetFromResource(TMrbCaen_V879::kBitControl1ProgResetModule);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	bNames = new TMrbLofNamedX();
	bNames->SetName("Control1");
	bNames->AddNamedX(kMrbControl1Bits);	
	bNames->SetPatternMode();
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode();

// addr high
	kp = new TMrbNamedX(TMrbCaen_V879::kRegAddrHigh, "AddrHigh");
	rp = new TMrbVMERegister(this, 0, kp,
								TMrbCaen_V879::kOffsAddrHigh,
								TMrbCaen_V879::kOffsAddrHigh,
								TMrbCaen_V879::kOffsAddrHigh,
								0, 0, 0xff);
	rp->SetFromResource(0);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

// addr low
	kp = new TMrbNamedX(TMrbCaen_V879::kRegAddrLow, "AddrLow");
	rp = new TMrbVMERegister(this, 0, kp,
								TMrbCaen_V879::kOffsAddrLow,
								TMrbCaen_V879::kOffsAddrLow,
								TMrbCaen_V879::kOffsAddrLow,
								0, 0, 0xff);
	rp->SetFromResource(0);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

// event trigger
	kp = new TMrbNamedX(TMrbCaen_V879::kRegEvtTrig, "EventTrigger");
	rp = new TMrbVMERegister(this, 0, kp,
								TMrbCaen_V879::kOffsEvtTrig,
								TMrbCaen_V879::kOffsEvtTrig,
								TMrbCaen_V879::kOffsEvtTrig,
								0, 0, 0x1f);
	rp->SetFromResource(0);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

// control 1
	kp = new TMrbNamedX(TMrbCaen_V879::kRegBitSet2, "BitSet2");
	rp = new TMrbVMERegister(this, 0, kp,
								TMrbCaen_V879::kOffsBitSet2,
								TMrbCaen_V879::kOffsBitClear2,
								TMrbCaen_V879::kOffsBitSet2,
								0, 0, 0x7fff);
	rp->SetFromResource(0);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	bNames = new TMrbLofNamedX();
	bNames->SetName("BitSet2");
	bNames->AddNamedX(kMrbBitSet2Bits);	
	bNames->SetPatternMode();
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode();

	// lower threshold
	kp = new TMrbNamedX(TMrbCaen_V879::kRegThresh, "LowerThresh");
	rp = new TMrbVMERegister(this, fNofChannels, kp,
								TMrbCaen_V879::kOffsThresh,
								TMrbCaen_V879::kOffsThresh,
								TMrbCaen_V879::kOffsThresh,
								0, 0, 0xff);
	rp->SetFromResource(0);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
}

Bool_t TMrbCaen_V879::MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V879::MakeReadoutCode
// Purpose:        Write a piece of code for a caen adc/tac
// Arguments:      ofstream & RdoStrm         -- file output stream
//                 EMrbModuleTag TagIndex     -- index of tag word taken from template file
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a caen v879 adc/tac module.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	TString mnemoLC, mnemoUC;
	TMrbVMEChannel * chn;

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
		case TMrbConfig::kModuleInitCommonCode:
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.Substitute("$moduleTitle", this->GetTitle());
			fCodeTemplates.Substitute("$modulePosition", this->GetPosition());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$baseAddr", (Int_t) this->GetBaseAddr(), 16);
			fCodeTemplates.WriteCode(RdoStrm);
			break;
		case TMrbConfig::kModuleInitModule:
			fCodeTemplates.InitializeCode("%B%");
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.Substitute("$moduleTitle", this->GetTitle());
			fCodeTemplates.Substitute("$modulePosition", this->GetPosition());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$baseAddr", (Int_t) this->GetBaseAddr(), 16);
			fCodeTemplates.WriteCode(RdoStrm);
			fCodeTemplates.InitializeCode("%ZSP%");
			if (this->HasZeroSuppression()) {
				fCodeTemplates.Substitute("$onOrOff", "ON");
				fCodeTemplates.Substitute("$dont", "");
			} else {
				fCodeTemplates.Substitute("$onOrOff", "OFF");
				fCodeTemplates.Substitute("$dont", "don't");
			}
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.WriteCode(RdoStrm);
			fCodeTemplates.InitializeCode("%ORC%");
			if (this->HasOverRangeCheck()) {
				fCodeTemplates.Substitute("$onOrOff", "ON");
				fCodeTemplates.Substitute("$dont", "");
			} else {
				fCodeTemplates.Substitute("$onOrOff", "OFF");
				fCodeTemplates.Substitute("$dont", "don't");
			}
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.WriteCode(RdoStrm);

			if (this->IsTac()) {
				fCodeTemplates.InitializeCode("%TAC%");
				fCodeTemplates.Substitute("$rangeSlope", this->GetRangeSlope());
				fCodeTemplates.Substitute("$rangeOffset", this->GetRangeOffset());
				fCodeTemplates.Substitute("$moduleName", this->GetName());
				fCodeTemplates.WriteCode(RdoStrm);
			}

			fCodeTemplates.InitializeCode("%E%");
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.WriteCode(RdoStrm);
			for (i = 0; i < fNofChannels; i++, chn++) {
				chn = (TMrbVMEChannel *) fChannelSpec[i];
				this->MakeReadoutCode(RdoStrm, TMrbConfig::kModuleInitChannel, chn);
			}
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
			fCodeTemplates.Substitute("$moduleSerial", this->GetSerial());
			fCodeTemplates.WriteCode(RdoStrm);
			break;
	}
	return(kTRUE);
}


Bool_t TMrbCaen_V879::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbModuleTag TagIndex,
															TObject * Channel,
															Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V879::MakeReadoutCode
// Purpose:        Write a piece of code for a caen adc/tac
// Arguments:      ofstream & RdoStrm           -- file output stream
//                 EMrbModuleTag TagIndex       -- index of tag word taken from template file
//                 TObject * Channel            -- channel
//                 Int_t Value                  -- value to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a caen v879 adc/tac module.
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
			if (chn->IsUsed())	fCodeTemplates.InitializeCode("%U%");
			else				fCodeTemplates.InitializeCode("%N%");
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$chnName", chn->GetName());
			fCodeTemplates.Substitute("$chnNo", chn->GetAddr());
			fCodeTemplates.Substitute("$lowerThresh", chn->Get(TMrbCaen_V879::kRegThresh));
			fCodeTemplates.WriteCode(RdoStrm);
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
			fCodeTemplates.Substitute("$moduleSerial", this->GetSerial());
			fCodeTemplates.Substitute("$chnName", chn->GetName());
			fCodeTemplates.Substitute("$chnNo", chn->GetAddr());
			fCodeTemplates.Substitute("$data", Value);
			fCodeTemplates.WriteCode(RdoStrm);
			break;
	}
	return(kTRUE);
}

Bool_t TMrbCaen_V879::CheckSubeventType(TObject * Subevent) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V879::CheckSubeventType
// Purpose:        Check if calling subevent is applicable
// Arguments:      
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Makes sure that a subevent of type [10,4x] (CAEN)
//                 is calling.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbSubevent *sevt;

	sevt = (TMrbSubevent *) Subevent;
	if (sevt->GetType() != 10) return(kFALSE);
	if (sevt->GetSubtype() < 41 || sevt->GetSubtype() > 49) return(kFALSE);
	else													return(kTRUE);
}

Bool_t TMrbCaen_V879::SetRange(Int_t Slope, Int_t Offset) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V879::SetRange
// Purpose:        Set range registers
// Arguments:      Int_t Slope                -- slope of the range dac
//                 Int_t Offset               -- offset
// Results:        Int_t RegValue             -- resulting register value
// Exceptions:     Returns 0 on error (Nsec value out of range)
// Description:    
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->IsTac()) {
		gMrbLog->Err()	<< "Not in TAC mode" << endl;
		gMrbLog->Flush(this->ClassName(), "SetRange");
		return(kFALSE);
	}
	if (Slope < 0 || Slope > 255) {
		gMrbLog->Err()	<< "Illegal slope - " << Slope << " (should be in [0, 255])" << endl;
		gMrbLog->Flush(this->ClassName(), "SetRange");
		return(kFALSE);
	}
	if (Offset < 0 || Offset > 255) {
		gMrbLog->Err()	<< "Illegal offset - " << Offset << " (should be in [0, 255])" << endl;
		gMrbLog->Flush(this->ClassName(), "SetRange");
		return(kFALSE);
	}
	fRangeSlope = Slope;
	fRangeOffset = Offset;
	return(kTRUE);
}

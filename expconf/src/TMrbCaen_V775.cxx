//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbCaen_V775.cxx
// Purpose:        MARaBOU configuration: CAEN modules
// Description:    Implements class methods to handle a CAEN tdc type V775 
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbCaen_V775.cxx,v 1.6 2004-09-28 13:47:32 rudi Exp $       
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
#include "TMrbCaen_V775.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbCaen_V775)

const SMrbNamedX kMrbBitSet1Bits[] =
		{
			{TMrbCaen_V775::kBitSet1SelAddrIntern,			"AddrIntern",		"Select address via internal reg"					},
			{0, 											NULL,				NULL								}
		};

const SMrbNamedX kMrbControl1Bits[] =
		{
			{TMrbCaen_V775::kBitControl1ProgResetModule,	"ResetModule",		"Reset module"						},
			{~TMrbCaen_V775::kBitControl1ProgResetModule,	"ResetDataOnly",	"Reset data only"					},
			{0, 											NULL,				NULL								}
		};

const SMrbNamedX kMrbBitSet2Bits[] =
		{
			{TMrbCaen_V775::kBitSet2Offline,				"Offline",			"TDC offline"						},
			{TMrbCaen_V775::kBitSet2OverRangeDis,			"OverRangeDisable",	"Disable over range check"			},
			{TMrbCaen_V775::kBitSet2LowThreshDis,			"LowThreshDisable",	"Disable low threshold check"		},
			{TMrbCaen_V775::kBitSet2SlideEna,				"SlideEnable",		"Enable sliding scale"				},
			{TMrbCaen_V775::kBitSet2AutoIncrEna,			"AutoIncrEnable",	"Enable auto increment"				},
			{TMrbCaen_V775::kBitSet2EmptyProgEna,			"EmptyProgEnable",	"Write empty header & EOB"			},
			{TMrbCaen_V775::kBitSet2AllTrigEna, 			"AllTriggerEnable",	"Incr event cnt on ALL trigs"		},
			{0, 											NULL,				NULL								}
		};

TMrbCaen_V775::TMrbCaen_V775(const Char_t * ModuleName, UInt_t BaseAddr, Int_t NofChannels) :
									TMrbVMEModule(ModuleName, "Caen_V775", BaseAddr,
																TMrbCaen_V775::kAddrMod,
																TMrbCaen_V775::kSegSize,
																0, NofChannels, 1 << 12) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V775
// Purpose:        Create a tdc of type Caen V775
// Arguments:      Char_t * ModuleName      -- name of camac module
//                 UInt_t BaseAddr          -- base addr
//                 Int_t NofChannels        -- number of channels (16 or 32)
// Results:        --
// Exceptions:
// Description:    Creates a vme tdc of type CAEN V775.
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
		} else if (NofChannels != 16 && NofChannels != 32) {
			gMrbLog->Err()	<< ModuleName << ": Illegal number of channels - " << NofChannels
							<< " (should be 16 or 32)" << endl;
			gMrbLog->Flush(this->ClassName());
			this->MakeZombie();
		} else if (gMrbConfig->CheckModuleAddress(this)) {
			title = "CAEN V775 TDC ";
			title += NofChannels;		//16 or 32 channels
			title += " x 12 bit";
			SetTitle(title.Data()); 	// store module type
			mTypeBits = TMrbConfig::kModuleVME | TMrbConfig::kModuleListMode;
			gMrbConfig->GetLofModuleTypes()->Pattern2String(mType, mTypeBits);
			fModuleType.Set(mTypeBits, mType.Data());
			fDataType = gMrbConfig->GetLofDataTypes()->FindByIndex(TMrbConfig::kDataUShort);
			fNofShortsPerChannel = 1;
			fFFMode = kFALSE;
			fCommonStart = kTRUE;
			fFullScaleRange = 0x1E; 				// set full scale range to 1200 ns
			fFullScaleRangeNsecs = 1200;
			fZeroSuppression = kTRUE;
			fOverRangeCheck = kTRUE;
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

void TMrbCaen_V775::DefineRegisters() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V775::DefineRegisters
// Purpose:        Define camac registers
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
	kp = new TMrbNamedX(TMrbCaen_V775::kRegBitSet1, "BitSet1");
	rp = new TMrbVMERegister(this, 0, kp,
								TMrbCaen_V775::kOffsBitSet1,
								TMrbCaen_V775::kOffsBitClear1,
								TMrbCaen_V775::kOffsBitSet1,
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
	kp = new TMrbNamedX(TMrbCaen_V775::kRegControl1, "Control1");
	rp = new TMrbVMERegister(this, 0, kp,
								TMrbCaen_V775::kOffsControl1,
								TMrbCaen_V775::kOffsControl1,
								TMrbCaen_V775::kOffsControl1,
								TMrbCaen_V775::kBitControl1ProgResetModule, 0, 0x3f);
	rp->SetFromResource(TMrbCaen_V775::kBitControl1ProgResetModule);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	bNames = new TMrbLofNamedX();
	bNames->SetName("Control1");
	bNames->AddNamedX(kMrbControl1Bits);	
	bNames->SetPatternMode();
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode();

// addr high
	kp = new TMrbNamedX(TMrbCaen_V775::kRegAddrHigh, "AddrHigh");
	rp = new TMrbVMERegister(this, 0, kp,
								TMrbCaen_V775::kOffsAddrHigh,
								TMrbCaen_V775::kOffsAddrHigh,
								TMrbCaen_V775::kOffsAddrHigh,
								0, 0, 0xff);
	rp->SetFromResource(0);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

// addr low
	kp = new TMrbNamedX(TMrbCaen_V775::kRegAddrLow, "AddrLow");
	rp = new TMrbVMERegister(this, 0, kp,
								TMrbCaen_V775::kOffsAddrLow,
								TMrbCaen_V775::kOffsAddrLow,
								TMrbCaen_V775::kOffsAddrLow,
								0, 0, 0xff);
	rp->SetFromResource(0);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

// event trigger
	kp = new TMrbNamedX(TMrbCaen_V775::kRegEvtTrig, "EventTrigger");
	rp = new TMrbVMERegister(this, 0, kp,
								TMrbCaen_V775::kOffsEvtTrig,
								TMrbCaen_V775::kOffsEvtTrig,
								TMrbCaen_V775::kOffsEvtTrig,
								0, 0, 0x1f);
	rp->SetFromResource(0);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

// control 1
	kp = new TMrbNamedX(TMrbCaen_V775::kRegBitSet2, "BitSet2");
	rp = new TMrbVMERegister(this, 0, kp,
								TMrbCaen_V775::kOffsBitSet2,
								TMrbCaen_V775::kOffsBitClear2,
								TMrbCaen_V775::kOffsBitSet2,
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
	kp = new TMrbNamedX(TMrbCaen_V775::kRegThresh, "LowerThresh");
	rp = new TMrbVMERegister(this, fNofChannels, kp,
								TMrbCaen_V775::kOffsThresh,
								TMrbCaen_V775::kOffsThresh,
								TMrbCaen_V775::kOffsThresh,
								0, 0, 0xff);
	rp->SetFromResource(0);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
}

Bool_t TMrbCaen_V775::MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V775::MakeReadoutCode
// Purpose:        Write a piece of code for a caen tdc
// Arguments:      ofstream & RdoStrm         -- file output stream
//                 EMrbModuleTag TagIndex     -- index of tag word taken from template file
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a caen v775 tdc module.
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
			fCodeTemplates.InitializeCode("%CST%");
			fCodeTemplates.Substitute("$startOrStop", this->IsCommonStart() ? "START" : "STOP");
			fCodeTemplates.Substitute("$moduleName", this->GetName());
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
			fCodeTemplates.InitializeCode("%E%");
			fCodeTemplates.Substitute("$fullScaleRange", this->GetFullScaleRange(kFALSE), 16);
			fCodeTemplates.Substitute("$fullScaleNsecs", this->GetFullScaleRange(kTRUE));
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


Bool_t TMrbCaen_V775::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbModuleTag TagIndex,
															TObject * Channel,
															Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V775::MakeReadoutCode
// Purpose:        Write a piece of code for a caen tdc
// Arguments:      ofstream & RdoStrm           -- file output stream
//                 EMrbModuleTag TagIndex       -- index of tag word taken from template file
//                 TObject * Channel            -- channel
//                 Int_t Value                  -- value to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a caen v775 tdc module.
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
			fCodeTemplates.Substitute("$lowerThresh", chn->Get(TMrbCaen_V775::kRegThresh));
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

Bool_t TMrbCaen_V775::CheckSubeventType(TObject * Subevent) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V775::CheckSubeventType
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

Int_t TMrbCaen_V775::SetFullScaleRange(Int_t NanoSeconds) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V775::SetFullScaleRange
// Purpose:        Set full scale range register
// Arguments:      Int_t NanoSeconds          -- full scale range in nsecs
// Results:        Int_t RegValue             -- resulting register value
// Exceptions:     Returns 0 on error (Nsec value out of range)
// Description:    Calculates the value of the full scale range register from
//                 given nanoseconds.
//                 Full range values from 140 to 1200 nsecs correspond to
//                 register values from 0xFF to 0x1E with linear interpolation.
//                 Formula is therefore
//                      r(t) = 256 + (225/1060) * (140 - t)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (NanoSeconds < 140 || NanoSeconds > 1200) {
		gMrbLog->Err()	<< "Illegal Full Scale Range - " << NanoSeconds
						<< " ns (should be in [140,1200])" << endl;
		gMrbLog->Flush(this->ClassName(), "MakeReadoutCode");
		return(0);
	}

	fFullScaleRange = (Int_t) (0xFF + ((Float_t) (0xFF - 0x1E) / (1200 - 140)) * (140 - NanoSeconds));
	fFullScaleRangeNsecs = NanoSeconds;
	return(fFullScaleRange);
}

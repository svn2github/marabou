//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbMesytec_Mqdc32.cxx
// Purpose:        MARaBOU configuration: Mesytec modules
// Description:    Implements class methods to handle a Mesytec ADC type MQDC32
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbMesytec_Mqdc32.cxx,v 1.26 2012-01-18 11:11:32 Marabou Exp $
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
#include "TMrbResource.h"
#include "TMrbVMERegister.h"
#include "TMrbVMEChannel.h"
#include "TMrbMesytec_Mqdc32.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbMesytec_Mqdc32)

const SMrbNamedXShort kMrbAddressSource[] =
		{
			{	TMrbMesytec_Mqdc32::kAddressBoard,			"board" 	},
			{	TMrbMesytec_Mqdc32::kAddressRegister,		"register" 	},
			{	0,			 								NULL,		}
		};

const SMrbNamedXShort kMrbDataWidth[] =
		{
			{	TMrbMesytec_Mqdc32::kDataWidth8,			"8bit"		},
			{	TMrbMesytec_Mqdc32::kDataWidth16,			"16bit"		},
			{	TMrbMesytec_Mqdc32::kDataWidth32,			"32bit"		},
			{	TMrbMesytec_Mqdc32::kDataWidth64,			"64bit"		},
			{	0,			 								NULL,		}
		};

const SMrbNamedXShort kMrbMultiEvent[] =
		{
			{	TMrbMesytec_Mqdc32::kMultiEvtNo,			"singleEvent"	},
			{	TMrbMesytec_Mqdc32::kMultiEvtYes,			"multiEvent"	},
			{	TMrbMesytec_Mqdc32::kMultiEvtLim,			"multiLimWc"	},
			{	TMrbMesytec_Mqdc32::kMultiEvtNoBerr,		"multiNoBerr"	},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbMarkingType[] =
		{
			{	TMrbMesytec_Mqdc32::kMarkingTypeEvent,		"eventCounter"	},
			{	TMrbMesytec_Mqdc32::kMarkingTypeTs, 		"timeStamp" 	},
			{	TMrbMesytec_Mqdc32::kMarkingTypeXts, 		"extTimeStamp" 	},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbBankOperation[] =
		{
			{	TMrbMesytec_Mqdc32::kBankOprConnected,		"connected" 	},
			{	TMrbMesytec_Mqdc32::kBankOprIndependent,	"independent" 	},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbAdcResolution[] =
		{
			{	TMrbMesytec_Mqdc32::kAdcRes4k,				"4k"			},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbInputCoupling[] =
		{
			{	TMrbMesytec_Mqdc32::kInpCplAC,				"AC"			},
			{	TMrbMesytec_Mqdc32::kInpCplDC,				"DC"			},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbEclTerm[] =
		{
			{	TMrbMesytec_Mqdc32::kEclTermOff,			"off"			},

			{	TMrbMesytec_Mqdc32::kEclTermG0,				"gate0"			},
			{	TMrbMesytec_Mqdc32::kEclTermG1,				"gate1"			},
			{	TMrbMesytec_Mqdc32::kEclTermFcl,			"fclear"		},
			{	TMrbMesytec_Mqdc32::kEclTermIndivBank0,		"indivBank0"	},
			{	TMrbMesytec_Mqdc32::kEclTermIndivBank1,		"indivBank1"	},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbEclG1Osc[] =
		{
			{	TMrbMesytec_Mqdc32::kEclG1,					"gate1" 		},
			{	TMrbMesytec_Mqdc32::kEclOsc,				"oscillator" 	},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbEclFclRts[] =
		{
			{	TMrbMesytec_Mqdc32::kEclFcl,				"fclear"		},
			{	TMrbMesytec_Mqdc32::kEclRts,				"reset ts"		},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbGateSelect[] =
		{
			{	TMrbMesytec_Mqdc32::kGateSelNim,			"NIM"		},
			{	TMrbMesytec_Mqdc32::kGateSelEcl,			"ECL"		},
			{	0,			 								NULL,			}
		};

		const SMrbNamedXShort kMrbNimG1Osc[] =
		{
			{	TMrbMesytec_Mqdc32::kNimG1,					"gate1" 		},
			{	TMrbMesytec_Mqdc32::kNimOsc,				"oscillator" 	},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbNimFclRts[] =
		{
			{	TMrbMesytec_Mqdc32::kNimFcl,				"fclear"		},
			{	TMrbMesytec_Mqdc32::kNimRts,				"reset ts"		},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbNimBusy[] =
		{
			{	TMrbMesytec_Mqdc32::kNimBusy,				"busy"			},
			{	TMrbMesytec_Mqdc32::kNimG0Out,				"gate0"			},
			{	TMrbMesytec_Mqdc32::kNimG1Out,				"gate1"			},
			{	TMrbMesytec_Mqdc32::kNimCbusOut,			"cbus"			},
			{	TMrbMesytec_Mqdc32::kNimBufferFull,			"bufferFull"	},
			{	TMrbMesytec_Mqdc32::kNimAboveThresh,		"aboveThresh"	},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbPulserStatus[] =
		{
			{	TMrbMesytec_Mqdc32::kPulserOff, 			"off"			},
			{	TMrbMesytec_Mqdc32::kPulserAmpl0, 			"ampl0"			},
			{	TMrbMesytec_Mqdc32::kPulserAmpl100ns, 		"ampl100ns"		},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbTsSource[] =
		{
			{	TMrbMesytec_Mqdc32::kTstampVME, 			"vme"			},
			{	TMrbMesytec_Mqdc32::kTstampExtern, 			"extern"		},
			{	TMrbMesytec_Mqdc32::kTstampReset, 			"reset-enable"	},
			{	0,			 								NULL,			}
		};

TMrbMesytec_Mqdc32::TMrbMesytec_Mqdc32(const Char_t * ModuleName, UInt_t BaseAddr) :
									TMrbVMEModule(ModuleName, "Mesytec_Mqdc32", BaseAddr,
											0x09,
											TMrbMesytec_Mqdc32::kSegSize,
											1, 32, 1 << 13) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mqdc32
// Purpose:        Create a digitizing adc of type Mesytec MQDC-32
// Arguments:      Char_t * ModuleName      -- name of camac module
//                 UInt_t BaseAddr          -- base addr
// Results:        --
// Exceptions:
// Description:    Creates a vme module of type Mesytec MQDC-32.
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
			SetTitle("Mesytec charge integrating qdc 32 chn 12 bit"); 	// store module type
			codeFile = fModuleID.GetName();
			codeFile += ".code";
			if (LoadCodeTemplates(codeFile)) {
				DefineRegisters();
				mTypeBits = TMrbConfig::kModuleVME |
							TMrbConfig::kModuleListMode |
							TMrbConfig::kModuleMultiEvent |
							TMrbConfig::kModuleTimeStamp |
							TMrbConfig::kModuleQdc;
				gMrbConfig->GetLofModuleTypes()->Pattern2String(mType, mTypeBits);
				fModuleType.Set(mTypeBits, mType.Data());
				fDataType = gMrbConfig->GetLofDataTypes()->FindByIndex(TMrbConfig::kDataUInt);
				fNofShortsPerChannel = 2;		// 32 bits
				fNofShortsPerDatum = 1;
				fNofDataBits = 12;
				fBlockReadout = kTRUE;			// module has block readout
				fBlockXfer = kFALSE;
				fRepairRawData = kFALSE;

				fSettingsFile = Form("%sSettings.rc", this->GetName());

				fMCSTSignature = 0;
				fMCSTMaster = kFALSE;
				fCBLTSignature = 0;
				fFirstInChain = kFALSE;
				fLastInChain = kFALSE;

				gMrbConfig->AddModule(this);	// append to list of modules
				gDirectory->Append(this);
			} else {
				this->MakeZombie();
			}
		}
	}
}

void TMrbMesytec_Mqdc32::DefineRegisters() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mqdc32::DefineRegisters
// Purpose:        Define module registers
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Defines special registers
// Keywords:						tmpl.Substitute("$pulserStatus", this->PulserOn());

//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	TMrbLofNamedX * bNames;
	TMrbVMERegister * rp;

	kp = new TMrbNamedX(TMrbMesytec_Mqdc32::kRegAddrSource, "AddressSource");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mqdc32::kAddressBoard,
													TMrbMesytec_Mqdc32::kAddressBoard,
													TMrbMesytec_Mqdc32::kAddressRegister);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("AddressSource");
	bNames->AddNamedX(kMrbAddressSource);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mqdc32::kRegAddrReg, "AddressRegister");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, 0, 0, 0xFFFF);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Mqdc32::kRegModuleId, "ModuleId");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, 0xFF, 0, 0xFF);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Mqdc32::kRegDataWidth, "DataWidth");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mqdc32::kDataWidth32,
													TMrbMesytec_Mqdc32::kDataWidth8,
													TMrbMesytec_Mqdc32::kDataWidth64);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("DataWidth");
	bNames->AddNamedX(kMrbDataWidth);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mqdc32::kRegMultiEvent, "MultiEvent");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mqdc32::kMultiEvtNo,
													TMrbMesytec_Mqdc32::kMultiEvtNo,
													TMrbMesytec_Mqdc32::kMultiEvtNoBerr);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("MultiEvent");
	bNames->AddNamedX(kMrbMultiEvent);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mqdc32::kRegXferData, "XferData");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	0,	0,	0x7FFF);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Mqdc32::kRegMarkingType, "MarkingType");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mqdc32::kMarkingTypeEvent,
													TMrbMesytec_Mqdc32::kMarkingTypeEvent,
													TMrbMesytec_Mqdc32::kMarkingTypeXts);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("MarkingType");
	bNames->AddNamedX(kMrbMarkingType);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mqdc32::kRegBankOperation, "BankOperation");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mqdc32::kBankOprConnected,
													TMrbMesytec_Mqdc32::kBankOprConnected,
													TMrbMesytec_Mqdc32::kBankOprIndependent);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("BankOperation");
	bNames->AddNamedX(kMrbBankOperation);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mqdc32::kRegAdcResolution, "AdcResolution");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mqdc32::kAdcRes4k,
													TMrbMesytec_Mqdc32::kAdcRes4k,
													TMrbMesytec_Mqdc32::kAdcRes4k);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("AdcResolution");
	bNames->AddNamedX(kMrbAdcResolution);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mqdc32::kRegBankOffset, "BankOffset");
	rp = new TMrbVMERegister(this, 2, kp, 0, 0, 0, TMrbMesytec_Mqdc32::kBankOffsetDefault, 0, 255);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Mqdc32::kRegGateLimit, "GateLimit");
	rp = new TMrbVMERegister(this, 2, kp, 0, 0, 0, TMrbMesytec_Mqdc32::kGateLimitOff, 0, 255);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Mqdc32::kRegExpTrigDelay, "ExpTrigDelay");
	rp = new TMrbVMERegister(this, 2, kp, 0, 0, 0, TMrbMesytec_Mqdc32::kExpTrigDelOff, 0, 16384);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Mqdc32::kRegInputCoupling, "InputCoupling");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mqdc32::kInpCplAC,
													TMrbMesytec_Mqdc32::kInpCplAC,
													TMrbMesytec_Mqdc32::kInpCplDC);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("InputCoupling");
	bNames->AddNamedX(kMrbInputCoupling);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mqdc32::kRegEclTerm, "EclTerm");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mqdc32::kEclTermBanksIndiv,
													TMrbMesytec_Mqdc32::kEclTermOff,
													0x1F);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("EclTerm");
	bNames->AddNamedX(kMrbEclTerm);
	bNames->SetPatternMode(kTRUE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kTRUE);

	kp = new TMrbNamedX(TMrbMesytec_Mqdc32::kRegEclG1Osc, "EclG1Osc");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mqdc32::kEclG1,
													TMrbMesytec_Mqdc32::kEclG1,
													TMrbMesytec_Mqdc32::kEclOsc);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("EclG1Osc");
	bNames->AddNamedX(kMrbEclG1Osc);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mqdc32::kRegEclFclRts, "EclFclRts");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mqdc32::kEclFcl,
													TMrbMesytec_Mqdc32::kEclFcl,
													TMrbMesytec_Mqdc32::kEclRts);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("EclFclRts");
	bNames->AddNamedX(kMrbEclFclRts);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mqdc32::kRegGateSelect, "GateSelect");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mqdc32::kGateSelNim,
													TMrbMesytec_Mqdc32::kGateSelNim,
													TMrbMesytec_Mqdc32::kGateSelEcl);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("GateSelect");
	bNames->AddNamedX(kMrbGateSelect);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mqdc32::kRegNimG1Osc, "NimG1Osc");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mqdc32::kNimG1,
													TMrbMesytec_Mqdc32::kNimG1,
													TMrbMesytec_Mqdc32::kNimOsc);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("NimG1Osc");
	bNames->AddNamedX(kMrbNimG1Osc);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mqdc32::kRegNimFclRts, "NimFclRts");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mqdc32::kNimFcl,
													TMrbMesytec_Mqdc32::kNimFcl,
													TMrbMesytec_Mqdc32::kNimRts);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("NimFclRts");
	bNames->AddNamedX(kMrbNimFclRts);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mqdc32::kRegNimBusy, "NimBusy");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mqdc32::kNimBusy,
													TMrbMesytec_Mqdc32::kNimBusy,
													TMrbMesytec_Mqdc32::kNimAboveThresh);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("NimBusy");
	bNames->AddNamedX(kMrbNimBusy);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mqdc32::kRegPulserStatus, "PulserStatus");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mqdc32::kPulserOff,
													TMrbMesytec_Mqdc32::kPulserOff,
													TMrbMesytec_Mqdc32::kPulserAmpl100ns);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("PulserStatus");
	bNames->AddNamedX(kMrbPulserStatus);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mqdc32::kRegPulserDac, "PulserDac");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, 0, 0, 255);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Mqdc32::kRegTsSource, "TsSource");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mqdc32::kTstampVME,
													TMrbMesytec_Mqdc32::kTstampVME,
													TMrbMesytec_Mqdc32::kTstampExtern | TMrbMesytec_Mqdc32::kTstampReset);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("TsSource");
	bNames->AddNamedX(kMrbTsSource);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mqdc32::kRegTsDivisor, "TsDivisor");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, 1, 1, 0xFFFF);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Mqdc32::kRegMultHighLimit, "MultHighLimit");
	rp = new TMrbVMERegister(this, 2, kp, 0, 0, 0, TMrbMesytec_Mqdc32::kMultHighLimit0, 0, 0x3F);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	
	kp = new TMrbNamedX(TMrbMesytec_Mqdc32::kRegMultLowLimit, "MultLowLimit");
	rp = new TMrbVMERegister(this, 2, kp, 0, 0, 0, TMrbMesytec_Mqdc32::kMultLowLimit, 0, 0x3F);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Mqdc32::kRegThreshold, "Threshold");
	rp = new TMrbVMERegister(this, TMrbMesytec_Mqdc32::kNofChannels, kp, 0, 0, 0, 0, 0, 0x1FFF);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
}

TEnv * TMrbMesytec_Mqdc32::UseSettings(const Char_t * SettingsFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mqdc32::UseSettings
// Purpose:        Read settings from file
// Arguments:      Char_t * SettingsFile   -- settings file
// Results:        TEnv * settings         -- settings in ROOT's TEnv format
// Exceptions:
// Description:    Load settings to be used from env file
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fSettingsFile.IsNull()) fSettingsFile = Form("%sSettings.rc", this->GetName());

	if (SettingsFile == NULL || *SettingsFile == '\0') SettingsFile = fSettingsFile.Data();

	if (gSystem->AccessPathName(SettingsFile, kFileExists) == 0) {
		fSettingsFile = SettingsFile;
	} else {
		gMrbLog->Err() << "Settings file not found - " << SettingsFile << endl;
		gMrbLog->Flush(this->ClassName(), "UseSettings");
		TString sf = Form("%sSettings.rc.default", this->GetName());
		gMrbLog->Err()	<< "Creating DEFAULT settings file - " << sf << endl;
		gMrbLog->Flush(this->ClassName(), "UseSettings");
		gMrbLog->Err()	<< "Please edit this file and then rename it to \"" << SettingsFile << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "UseSettings");
		this->SaveSettings(sf.Data());
		return(NULL);
	}

	TMrbResource * mqdcEnv = new TMrbResource("MQDC32", fSettingsFile.Data());

	TString moduleName; mqdcEnv->Get(moduleName, ".ModuleName", "");
	if (moduleName.CompareTo(this->GetName()) != 0) {
		gMrbLog->Err() << "Module name different - \"" << moduleName << "\" (should be " << this->GetName() << ")" << endl;
		gMrbLog->Flush(this->ClassName(), "UseSettings");
		return(NULL);
	}
	moduleName(0,1).ToUpper();
	moduleName.Prepend(".");

	this->SetBlockXfer(mqdcEnv->Get(moduleName.Data(), "BlockXfer", kFALSE));
	this->RepairRawData(mqdcEnv->Get(moduleName.Data(), "RepairRawData", kFALSE));
	this->SetAddressSource(mqdcEnv->Get(moduleName.Data(), "AddressSource", kAddressBoard));
	this->SetAddressRegister(mqdcEnv->Get(moduleName.Data(), "AddressRegister", 0));
	this->SetMcstSignature(mqdcEnv->Get(moduleName.Data(), "MCSTSignature", (Int_t) fMCSTSignature));
	this->SetMcstMaster(mqdcEnv->Get(moduleName.Data(), "MCSTMaster", fMCSTMaster));
	this->SetCbltSignature(mqdcEnv->Get(moduleName.Data(), "CBLTSignature", (Int_t) fCBLTSignature));
	this->SetFirstInChain(mqdcEnv->Get(moduleName.Data(), "CBLTFirstInChain", fFirstInChain));
	this->SetLastInChain(mqdcEnv->Get(moduleName.Data(), "CBLTLastInChain", fLastInChain));
	Int_t mid = mqdcEnv->Get(moduleName.Data(), "ModuleId", 0xFF);
	if (mid == 0xFF) mid = this->GetSerial();
	this->SetModuleId(mid);
	this->SetDataWidth(mqdcEnv->Get(moduleName.Data(), "DataWidth", kDataWidth32));
	this->SetMultiEvent(mqdcEnv->Get(moduleName.Data(), "MultiEvent", kMultiEvtNo));
	this->SetXferData(mqdcEnv->Get(moduleName.Data(), "XferData", 0));
	this->SetMarkingType(mqdcEnv->Get(moduleName.Data(), "MarkingType", kMarkingTypeEvent));
	this->SetBankOperation(mqdcEnv->Get(moduleName.Data(), "BankOperation", kBankOprConnected));
	this->SetAdcResolution(mqdcEnv->Get(moduleName.Data(), "AdcResolution", kAdcRes4k));
	this->SetBankOffset(mqdcEnv->Get(moduleName.Data(), "BankOffset", "0", kBankOffsetDefault), 0);
	this->SetBankOffset(mqdcEnv->Get(moduleName.Data(), "BankOffset", "1", kBankOffsetDefault), 1);
	this->SetSlidingScaleOff(mqdcEnv->Get(moduleName.Data(), "SlidingScaleOff", kTRUE));
	this->SetSkipOutOfRange(mqdcEnv->Get(moduleName.Data(), "SkipOutOfRange", kFALSE));
	this->SetIgnoreThresholds(mqdcEnv->Get(moduleName.Data(), "IgnoreThresh", kFALSE));
	this->SetGateLimit(mqdcEnv->Get(moduleName.Data(), "GateLimit", "0", kGateLimitOff), 0);
	this->SetGateLimit(mqdcEnv->Get(moduleName.Data(), "GateLimit", "1", kGateLimitOff), 1);
	this->SetExpTrigDelay(mqdcEnv->Get(moduleName.Data(), "ExpTrigDelay", "0", kExpTrigDelOff), 0);
	this->SetExpTrigDelay(mqdcEnv->Get(moduleName.Data(), "ExpTrigDelay", "1", kExpTrigDelOff), 1);
	this->SetInputCoupling(mqdcEnv->Get(moduleName.Data(), "InputCoupling", kInpCplAC));
	this->SetEclTerm(mqdcEnv->Get(moduleName.Data(), "EclTerm", kEclTermBanksIndiv));
	this->SetEclG1Osc(mqdcEnv->Get(moduleName.Data(), "EclG1Osc:EclG1Osc", kEclG1));
	this->SetEclFclRts(mqdcEnv->Get(moduleName.Data(), "EclFclRts:EclFclRts", kEclFcl));
	this->SetGateSelect(mqdcEnv->Get(moduleName.Data(), "GateSelect", kGateSelNim));
	this->SetNimG1Osc(mqdcEnv->Get(moduleName.Data(), "NimG1Osc:NimG1Osc", kNimG1));
	this->SetNimFclRts(mqdcEnv->Get(moduleName.Data(), "NimFclRts:NimFclRts", kNimFcl));
	this->SetNimBusy(mqdcEnv->Get(moduleName.Data(), "NimBusy", kNimBusy));
	this->SetBufferThresh(mqdcEnv->Get(moduleName.Data(), "BufferThresh", 0));
	this->SetPulserStatus(mqdcEnv->Get(moduleName.Data(), "PulserStatus", 0));
	this->SetPulserDac(mqdcEnv->Get(moduleName.Data(), "PulserDac", 0));
	this->SetTsSource(mqdcEnv->Get(moduleName.Data(), "TsSource", kTstampVME));
	this->SetTsDivisor(mqdcEnv->Get(moduleName.Data(), "TsDivisor", 1));
	this->SetMultHighLimit(mqdcEnv->Get(moduleName.Data(), "MultHighLimit", "0", kMultHighLimit0), 0);
	this->SetMultHighLimit(mqdcEnv->Get(moduleName.Data(), "MultHighLimit", "1", kMultHighLimit1), 0);
	this->SetMultLowLimit(mqdcEnv->Get(moduleName.Data(), "MultLowLimit", "0", kMultLowLimit), 0);
	this->SetMultLowLimit(mqdcEnv->Get(moduleName.Data(), "MultLowLimit", "1", kMultLowLimit), 0);

	for (Int_t i = 0; i < TMrbMesytec_Mqdc32::kNofChannels; i++) {
		this->SetThreshold(mqdcEnv->Get(moduleName.Data(), "Thresh", Form("%d", i), 0), i);
	}

	fRange = 1 << 12;		// histo range
	fXmax = fRange;
	fBinRange = fRange;

	fSettings = mqdcEnv->Env();
	
	this->UpdateSettings();
	
	if (this->BlockXferEnabled()) {
		Int_t multi = this->GetMultiEvent();
		if (multi == kMultiEvtNoBerr) {
			gMrbLog->Err()	<< "[" << this->GetName() << "] BERR turned off (reg 0x6036 = " << kMultiEvtNoBerr << ")  while BlockXfer is ON" << endl;
			gMrbLog->Flush(this->ClassName(), "UseSettings");
		}
	}
	
	this->SetupMCST();
	this->SetupCBLT();
	
	return(mqdcEnv->Env());
}

Bool_t TMrbMesytec_Mqdc32::UpdateSettings() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mqdc32::UpdateSettings
// Purpose:        Update settings file if needed
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Check version of settings file and perform update if needed
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString settingsVersion;
	TMrbResource * madcEnv = new TMrbResource("MQDC32", fSettingsFile.Data());
	madcEnv->Get(settingsVersion, ".SettingsVersion", "");
	if (settingsVersion.CompareTo("7.2017") != 0) {
		gMrbLog->Out() << "Settings file \"" << fSettingsFile << "\" has wrong (old?) version \"" << settingsVersion << "\" (should be 7.2017)" << endl;
		gMrbLog->Flush(this->ClassName(), "UpdateSettings", setblue);
		TString oldFile = fSettingsFile;
		oldFile += "-old";
		gSystem->Rename(fSettingsFile, oldFile);
		this->SaveSettings(fSettingsFile.Data());
		gMrbLog->Out() << "Settings file \"" << fSettingsFile << "\" converted to version 4.2017, old one renamed to \"" << oldFile << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "UpdateSettings", setblue);
	}
	return kTRUE;
}

void TMrbMesytec_Mqdc32::SetupMCST() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mqdc32::SetupMCST
// Purpose:        Setup MCST mode
// Arguments:      --
// Results:       --
// Exceptions:
// Description:    Check if module is using MCST
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fMCSTSignature == 0) return;
	
	TMrbNamedX * mcst = gMrbConfig->GetLofMesytecMCST()->FindByIndex(fMCSTSignature);
	TObjArray * oa;
	if (mcst == NULL) {
		oa = new TObjArray();
		mcst = new TMrbNamedX(fMCSTSignature, "", "", oa);
		gMrbConfig->GetLofMesytecMCST()->AddNamedX(mcst);
	} else {
		oa = (TObjArray *) mcst->GetAssignedObject();
	}
	TMrbNamedX * m = new TMrbNamedX(fMCSTMaster ? 1 : 0, this->GetName(), "Mqdc32");
	oa->Add(m);
}

void TMrbMesytec_Mqdc32::SetupCBLT() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mqdc32::SetupCBLT
// Purpose:        Setup CBLT mode
// Arguments:      --
// Results:       --
// Exceptions:
// Description:    Check if module is using CBLT
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fCBLTSignature == 0) return;
	
	if (fMCSTSignature == 0) {
		gMrbLog->Err()	<< "[" << this->GetName() << "] CBLT cannot be used with MCST turned off" << endl;
		gMrbLog->Flush(this->ClassName(), "SetupCBLT");
		return;
	}

	if (fMCSTSignature == fCBLTSignature) {
		gMrbLog->Err()	<< "[" << this->GetName() << "] CBLT and MCST signatures have to be different" << endl;
		gMrbLog->Flush(this->ClassName(), "SetupCBLT");
		return;
	}

	TMrbNamedX * cblt = gMrbConfig->GetLofMesytecCBLT()->FindByIndex(fCBLTSignature);
	TObjArray * oa;
	if (cblt == NULL) {
		oa = new TObjArray();
		cblt = new TMrbNamedX(fCBLTSignature, "", "", oa);
		gMrbConfig->GetLofMesytecCBLT()->AddNamedX(cblt);
	} else {
		oa = (TObjArray *) cblt->GetAssignedObject();
	}
	TMrbNamedX * c;
	if (this->IsFirstInChain()) {
		if (!this->IsMcstMaster()) {
			gMrbLog->Err()	<< "[" << this->GetName() << "] module is \"fist in CBLT chain\" but NOT MCST master" << endl;
			gMrbLog->Flush(this->ClassName(), "SetupCBLT");
			return;
		}
		c = new TMrbNamedX(1, this->GetName(), "Mqdc32");
	} else if (this->IsLastInChain()) {
		c = new TMrbNamedX(-1, this->GetName(), "Mqdc32");
	} else {
		c = new TMrbNamedX(0, this->GetName(), "Mqdc32");
	}
	oa->Add(c);
}

Bool_t TMrbMesytec_Mqdc32::SaveSettings(const Char_t * SettingsFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mqdc32::SaveSettings
// Purpose:        Write settings to file
// Arguments:      Char_t * SettingsFile   -- settings file
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Write env data to file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fSettingsFile.IsNull()) fSettingsFile = Form("%sSettings.rc", this->GetName());

	if (SettingsFile == NULL || *SettingsFile == '\0') SettingsFile = fSettingsFile.Data();
	TString settingsFile = SettingsFile;

	ofstream settings(settingsFile.Data(), ios::out);
	if (!settings.good()) {
		gMrbLog->Err() << gSystem->GetError() << " - " << settingsFile << endl;
		gMrbLog->Flush(this->ClassName(), "SaveSettings");
		return(kFALSE);
	}

	TString tmplPath = gEnv->GetValue("TMrbConfig.TemplatePath", ".:config:$(MARABOU)/templates/config");
	TString tf = "Module_Mqdc32.rc.code";
	gSystem->ExpandPathName(tmplPath);

	const Char_t * fp = gSystem->Which(tmplPath.Data(), tf.Data());
	if (fp == NULL) {
		gMrbLog->Err()	<< "Template file not found -" << endl
						<<           "                 Searching on path " << tmplPath << endl
						<<           "                 for file          " << tf << endl;
		gMrbLog->Flush("SaveSettings");
		return(kFALSE);
	} else if (gMrbConfig->IsVerbose()) {
		gMrbLog->Out()	<< "Using template file " << fp << endl;
		gMrbLog->Flush("SaveSettings");
	}

	tf = fp;

	TMrbLofNamedX tags;
	tags.AddNamedX(TMrbConfig::kRcModuleSettings, "MODULE_SETTINGS");

	TString moduleLC = this->GetName();
	TString moduleUC = moduleLC;
	moduleUC(0,1).ToUpper();

	TMrbTemplate tmpl;
	TString line;
	if (tmpl.Open(tf.Data(), &tags)) {
		for (;;) {
			TMrbNamedX * tag = tmpl.Next(line);
			if (tmpl.IsEof()) break;
			if (tmpl.IsError()) continue;
			if (tmpl.Status() & TMrbTemplate::kNoTag) {
				if (line.Index("//-") != 0) settings << line << endl;
			} else {
				switch (tag->GetIndex()) {
					case TMrbConfig::kRcModuleSettings:
					{
						tmpl.InitializeCode("%Preamble%");
						tmpl.Substitute("$moduleName", moduleLC.Data());
						tmpl.Substitute("$moduleSerial", this->GetSerial());
						tmpl.Substitute("$moduleFirmware", "n/a");
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%AddressRegisters%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.Substitute("$addrSource", this->GetAddressSource());
						tmpl.Substitute("$addrReg", this->GetAddressRegister());
						tmpl.Substitute("$moduleId", this->GetModuleId());
						tmpl.Substitute("$mcstSignature", (Int_t) fMCSTSignature, 16);
						tmpl.Substitute("$mcstMaster", fMCSTMaster ? "TRUE" : "FALSE");
						tmpl.Substitute("$cbltSignature", (Int_t) fCBLTSignature);
						tmpl.Substitute("$cbltFirstInChain", fFirstInChain ? "TRUE" : "FALSE");
						tmpl.Substitute("$cbltLastInChain", fLastInChain ? "TRUE" : "FALSE");
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%FifoHandling%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.Substitute("$dataWidth", this->GetDataWidth());
						tmpl.Substitute("$multiEvent", this->GetMultiEvent());
						tmpl.Substitute("$markingType", this->GetMarkingType());
						tmpl.Substitute("$blockXfer", this->BlockXferEnabled() ? "TRUE" : "FALSE");
						tmpl.Substitute("$repairRawData", this->RawDataToBeRepaired() ? "TRUE" : "FALSE");
						tmpl.Substitute("$bufferThresh", this->GetBufferThresh());
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%OperationMode%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.Substitute("$bankOperation", this->GetBankOperation());
						tmpl.Substitute("$adcResolution", this->GetAdcResolution());
						tmpl.Substitute("$offsetBank0", this->GetBankOffset(0));
						tmpl.Substitute("$offsetBank1", this->GetBankOffset(1));
						tmpl.Substitute("$slidingScaleOff", this->SlidingScaleIsOff() ? "TRUE" : "FALSE");
						tmpl.Substitute("$skipOutOfRange", this->SkipOutOfRange() ? "TRUE" : "FALSE");
 						tmpl.Substitute("$ignoreThresh", this->IgnoreThresholds() ? "TRUE" : "FALSE");
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%GateGenerator%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.Substitute("$gateLimit0", this->GetGateLimit(0));
						tmpl.Substitute("$gateLimit1", this->GetGateLimit(1));
						tmpl.Substitute("$expTrigDelay0", this->GetExpTrigDelay(0));
						tmpl.Substitute("$expTrigDelay1", this->GetExpTrigDelay(1));
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%InputOutput%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.Substitute("$inputCoupling", this->GetInputCoupling());
						tmpl.Substitute("$eclTerm", this->GetEclTerm());
						tmpl.Substitute("$eclG1Osc", this->GetEclG1Osc());
						tmpl.Substitute("$eclFclRts", this->GetEclFclRts());
						tmpl.Substitute("$gateSelect", this->GetGateSelect());
						tmpl.Substitute("$nimG1Osc", this->GetNimG1Osc());
						tmpl.Substitute("$nimFclRts", this->GetNimFclRts());
						tmpl.Substitute("$nimBusy", this->GetNimBusy());
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%TestPulser%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.Substitute("$pulserStatus", this->GetPulserStatus());
						tmpl.Substitute("$pulserDac", this->GetPulserDac());
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%Counters%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.Substitute("$tsSource", this->GetTsSource());
						tmpl.Substitute("$tsDivisor", this->GetTsDivisor());
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%Multiplicity%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.Substitute("$multHighLimit0", this->GetMultHighLimit(0));
						tmpl.Substitute("$multHighLimit1", this->GetMultHighLimit(1));
						tmpl.Substitute("$multLowLimit0", this->GetMultLowLimit(0));
						tmpl.Substitute("$multLowLimit1", this->GetMultLowLimit(1));
						tmpl.WriteCode(settings);
						
						tmpl.InitializeCode("%Thresholds%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.WriteCode(settings);
						for (Int_t i = 0; i < TMrbMesytec_Mqdc32::kNofChannels; i++) {
							tmpl.InitializeCode("%ThreshLoop%");
							tmpl.Substitute("$moduleName", moduleUC.Data());
							tmpl.Substitute("$chn", i);
							tmpl.Substitute("$threshold", this->GetThreshold(i));
							tmpl.WriteCode(settings);
						}
					}
				}
			}
		}
	}
	settings.close();
	fSettingsFile = settingsFile;
	return(kTRUE);
}

Bool_t TMrbMesytec_Mqdc32::MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mqdc32::MakeReadoutCode
// Purpose:        Write a piece of code for a Mesytec MQDC-32
// Arguments:      ofstream & RdoStrm         -- file output stream
//                 EMrbModuleTag TagIndex     -- index of tag word taken from template file
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a Mesytec MQDC-32.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString mnemoLC, mnemoUC, moduleNameLC, moduleNameUC;

	if (!fCodeTemplates.FindCode(TagIndex)) {
		gMrbLog->Err()	<< "No code loaded for tag "
						<< gMrbConfig->fLofModuleTags.FindByIndex(TagIndex)->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "MakeReadoutCode");
		return(kFALSE);
	}

	moduleNameLC = this->GetName();
	moduleNameUC = moduleNameLC;
	moduleNameUC(0,1).ToUpper();

	mnemoLC = this->GetMnemonic();
	mnemoUC = mnemoLC;
	mnemoUC.ToUpper();

	Int_t subType = 0;
	TMrbSubevent * s;
	TIterator * siter = gMrbConfig->GetLofSubevents()->MakeIterator();
	while (s = (TMrbSubevent *) siter->Next()) {
		TMrbModule * m = (TMrbModule *) s->FindModuleBySerial(this->GetSerial());
		if (m) {
			subType = s->GetSubtype();
			break;
		}
	}

	TMrbResource * env = new TMrbResource("TMrbConfig", ".rootrc");

	TString pwd = gSystem->Getenv("PWD");
	if (pwd.Length() == 0) pwd = gSystem->WorkingDirectory();
	TString settings = pwd;
	settings += "/";
	settings += fSettingsFile;

	TString dump = pwd;
	dump += "/";
	dump += this->GetName();
	dump += "Dump.dat";

	switch (TagIndex) {
		case TMrbConfig::kModuleDefs:
		case TMrbConfig::kModuleInitCommonCode:
		case TMrbConfig::kModuleInitModule:
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$moduleNameLC", moduleNameLC);
			fCodeTemplates.Substitute("$moduleNameUC", moduleNameUC);
			fCodeTemplates.Substitute("$moduleTitle", this->GetTitle());
			fCodeTemplates.Substitute("$modulePosition", this->GetPosition());
			fCodeTemplates.Substitute("$moduleSerial", this->GetSerial());
			fCodeTemplates.Substitute("$sevtSubtype", subType);
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$baseAddr", (Int_t) this->GetBaseAddr(), 16);
			fCodeTemplates.Substitute("$settingsFile", settings.Data());
			fCodeTemplates.Substitute("$chnPattern", (Int_t) this->GetPatternOfChannelsUsed(), 16);
			fCodeTemplates.Substitute("$serial", this->GetSerial());
			fCodeTemplates.Substitute("$dumpFile", dump.Data());
			fCodeTemplates.WriteCode(RdoStrm);
			break;
		case TMrbConfig::kModuleInitBLT:
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$moduleName", moduleNameLC);
			fCodeTemplates.Substitute("$size", (Int_t) this->GetSegmentSize(), 16);
			fCodeTemplates.Substitute("$baseAddr", (Int_t) this->GetBaseAddr(), 16);
			fCodeTemplates.Substitute("$addrMod", 0x0b, 16);
			fCodeTemplates.Substitute("$fifoMode", "TRUE");
			fCodeTemplates.WriteCode(RdoStrm);
			break;
		case TMrbConfig::kModuleClearModule:
		case TMrbConfig::kModuleFinishReadout:
		case TMrbConfig::kModuleUtilities:
		case TMrbConfig::kModuleDefineGlobals:
		case TMrbConfig::kModuleDefineGlobalsOnce:
		case TMrbConfig::kModuleDefineLocalVarsInit:
		case TMrbConfig::kModuleDefineLocalVarsReadout:
		case TMrbConfig::kModuleDefinePrototypes:
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.Substitute("$nofParams", this->GetNofChannelsUsed());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.WriteCode(RdoStrm);
			break;
		case TMrbConfig::kModuleStartAcquisition:
		case TMrbConfig::kModuleStopAcquisition:
		case TMrbConfig::kModuleStartAcquisitionGroup:
		case TMrbConfig::kModuleStopAcquisitionGroup:
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.Substitute("$nofParams", this->GetNofChannelsUsed());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.WriteCode(RdoStrm);
			break;
		case TMrbConfig::kModuleReadModule:
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$chnPattern", (Int_t) this->GetPatternOfChannelsUsed(), 16);
			fCodeTemplates.Substitute("$settingsFile", fSettingsFile.Data());
			fCodeTemplates.Substitute("$updateFile", Form(".%sUpdate", this->GetName()));
			fCodeTemplates.WriteCode(RdoStrm);
			break;
		case TMrbConfig::kModuleDefineIncludePaths:
			{
				TString codeString;
				fCodeTemplates.InitializeCode();
				fCodeTemplates.Substitute("$marabouPath", gSystem->Getenv("MARABOU"));
				Int_t bNo = this->GetMbsBranchNo();
				TString mbsVersion = "v62"; gMrbConfig->GetMbsVersion(mbsVersion, bNo);
				TString lynxVersion = "2.5"; gMrbConfig->GetLynxVersion(lynxVersion, bNo);
				fCodeTemplates.Substitute("$mbsVersion", mbsVersion.Data());
				fCodeTemplates.Substitute("$lynxVersion", lynxVersion.Data());
				fCodeTemplates.CopyCode(codeString);
				env->Replace(codeString);
				gSystem->ExpandPathName(codeString);
				gMrbConfig->GetLofRdoIncludes()->Add(new TObjString(codeString.Data()));
			}
			break;
		case TMrbConfig::kModuleDefineLibraries:
			{
				TString codeString;
				fCodeTemplates.InitializeCode();
				fCodeTemplates.Substitute("$marabouPath", gSystem->Getenv("MARABOU"));
				Int_t bNo = this->GetMbsBranchNo();
				TString lv = "2.5"; gMrbConfig->GetLynxVersion(lv, bNo);
				TString lp;
				if (bNo != -1) {
					lp = gEnv->GetValue(Form("TMrbConfig.PPCLibraryPath.%d", bNo), "");
					if (lp.IsNull()) lp = gEnv->GetValue("TMrbConfig.PPCLibraryPath", Form("$MARABOU/powerpc/lib/%s", lv.Data()));
				} else {
					lp = gEnv->GetValue("TMrbConfig.PPCLibraryPath", Form("$MARABOU/powerpc/lib/%s", lv.Data()));
				}
				fCodeTemplates.Substitute("$ppcLibraryPath", lp.Data());
				fCodeTemplates.CopyCode(codeString);
				env->Replace(codeString);
				gSystem->ExpandPathName(codeString);
				gMrbConfig->GetLofRdoLibs()->Add(new TObjString(codeString.Data()));
			}
			break;
	}
	return(kTRUE);
}


Bool_t TMrbMesytec_Mqdc32::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbModuleTag TagIndex,
															TMrbVMEChannel * Channel,
															Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mqdc32::MakeReadoutCode
// Purpose:        Write a piece of code for a Mesytec MQDC-32
// Arguments:      ofstream & RdoStrm           -- file output stream
//                 EMrbModuleTag TagIndex       -- index of tag word taken from template file
//                 TMrbVMEChannel * Channel     -- channel
//                 Int_t Value                  -- value to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a Mesytec MQDC-32.
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
			fCodeTemplates.Substitute("$chnName", Channel->GetName());
			fCodeTemplates.Substitute("$chnNo", Channel->GetAddr());
			fCodeTemplates.Substitute("$data", Value);
			fCodeTemplates.WriteCode(RdoStrm);
			break;
	}
	return(kTRUE);
}

Bool_t TMrbMesytec_Mqdc32::CheckSubeventType(TMrbSubevent * Subevent) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mqdc32::CheckSubeventType
// Purpose:        Check if calling subevent is applicable
// Arguments:
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Makes sure that a subevent of type [10,8x] (CAEN)
//                 is calling.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Subevent->GetType() != 10) return(kFALSE);
	if (Subevent->GetSubtype() < 81 || Subevent->GetSubtype() > 83) return(kFALSE);
	else															return(kTRUE);
}

void TMrbMesytec_Mqdc32::PrintSettings(ostream & Out) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mqdc32::PrintSettings
// Purpose:        Print settings
// Arguments:      ostream & Out           -- where to print
// Results:        --
// Exceptions:
// Description:    Prints settings to stream.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString value;

	Out << "=========================================================================" << endl;
	Out << " Mesytec MQDC32 settings" << endl;
	Out << "-------------------------------------------------------------------------" << endl;
	Out << " Module                 : "	<< this->GetName() << endl;
	Out << " Serial                 : "	<< this->GetSerial() << endl;
	Out << " Address source         : "	<< this->FormatValue(value, TMrbMesytec_Mqdc32::kRegAddrSource) << endl;
	Out << "         register       : "	<< this->FormatValue(value, TMrbMesytec_Mqdc32::kRegAddrReg, -1, 16) << endl;
	Out << " Module ID              : "	<< this->FormatValue(value, TMrbMesytec_Mqdc32::kRegModuleId) << endl;
	Out << " Data width             : "	<< this->FormatValue(value, TMrbMesytec_Mqdc32::kRegDataWidth) << endl;
	Out << " Single/multi event     : "	<< this->FormatValue(value, TMrbMesytec_Mqdc32::kRegMultiEvent) << endl;
	Out << " Marking type           : "	<< this->FormatValue(value, TMrbMesytec_Mqdc32::kRegMarkingType) << endl;
	Out << " Block tansfer          : "	<< (this->BlockXferEnabled() ? "off"    : "on") << endl;
	Out << " Repair raw data        : "	<< (this->RawDataToBeRepaired() ? "off"    : "on") << endl;
	Out << " Bank operation         : "	<< this->FormatValue(value, TMrbMesytec_Mqdc32::kRegBankOperation) << endl;
	Out << " ADC resolution         : "	<< this->FormatValue(value, TMrbMesytec_Mqdc32::kRegAdcResolution) << endl;
	Out << " Buffer threshold       : "	<< this->GetBufferThresh() << endl;
	Out << " Sliding scale          : "	<< (this->SlidingScaleIsOff() ? "off"    : "on") << endl;
	Out << " Skip if out of range   : "	<< (this->SkipOutOfRange() ? "yes"    : "no") << endl;
	Out << " Ignore thresholds      : "	<< (this->IgnoreThresholds() ? "yes"    : "no") << endl;
	Out << " Bank offset            : bank 0: " << this->GetBankOffset(0) << endl;
	Out << "                        : bank 1: " << this->GetBankOffset(1) << endl;
	Out << " Gate limit             : bank 0: " << this->GetGateLimit(0) << endl;
	Out << "                        : bank 1: " << this->GetGateLimit(1) << endl;
	Out << " Exp trigger delay      : bank 0: " << this->GetExpTrigDelay(0) << endl;
	Out << "                        : bank 1: " << this->GetExpTrigDelay(1) << endl;
	Out << " Input coupling         : "	<< this->FormatValue(value, TMrbMesytec_Mqdc32::kRegInputCoupling) << endl;
	Out << " ECL termination        : "	<< this->FormatValue(value, TMrbMesytec_Mqdc32::kRegEclTerm) << endl;
	Out << "           input        : "	<< this->FormatValue(value, TMrbMesytec_Mqdc32::kRegEclG1Osc) << endl;
	Out << "    fcl or reset        : "	<< this->FormatValue(value, TMrbMesytec_Mqdc32::kRegEclFclRts) << endl;
	Out << " Gate selection         : "	<< this->FormatValue(value, TMrbMesytec_Mqdc32::kRegGateSelect) << endl;
	Out << " NIM       input        : "	<< this->FormatValue(value, TMrbMesytec_Mqdc32::kRegNimG1Osc) << endl;
	Out << "    fcl or reset        : "	<< this->FormatValue(value, TMrbMesytec_Mqdc32::kRegNimFclRts) << endl;
	Out << "            busy        : "	<< this->FormatValue(value, TMrbMesytec_Mqdc32::kRegNimBusy) << endl;
	Out << " Pulser   status        : "	<< this->FormatValue(value, TMrbMesytec_Mqdc32::kRegPulserStatus) << endl;
	Out << "             dac        : "	<< this->FormatValue(value, TMrbMesytec_Mqdc32::kRegPulserDac) << endl;
	Out << " Timestamp source       : "	<< this->FormatValue(value, TMrbMesytec_Mqdc32::kRegTsSource) << endl;
	Out << "          divisor       : "	<< this->FormatValue(value, TMrbMesytec_Mqdc32::kRegTsDivisor) << endl;
	Out << " Multiplicity limit high: bank 0: " << this->GetMultHighLimit(0) << endl;
	Out << "                     low: bank 0: " << this->GetMultLowLimit(0) << endl;
	Out << "              limit high: bank 1: " << this->GetMultHighLimit(1) << endl;
	Out << "                     low: bank 1: " << this->GetMultLowLimit(1) << endl;
	TString f = " Threshold memory    : ";
	for (Int_t ch = 0; ch < 32; ch++) {
		if ((ch % 8) == 0) {
			if (ch) Out << endl;
			Out << f;
			f = "                     : ";
		}
		Out << Form("%6d", this->GetThreshold(ch));
	}
	Out << endl;
	Out << "-------------------------------------------------------------------------" << endl << endl;
}

const Char_t * TMrbMesytec_Mqdc32::FormatValue(TString & Value, Int_t Index, Int_t SubIndex, Int_t Base) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mqdc32::FormatValue
// Purpose:        Prepare for printing
// Arguments:      TString & Value    -- where to put formatted value
//                 Int_t Index        -- register index
//                 Int_t SubIndex     -- subregister
//                 Int_t Base         -- numerical base
// Results:        Char_t * Value     -- formatted string
// Exceptions:
// Description:    Formats a value: number and text
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Value = "";

	TMrbNamedX * kp = fLofRegisters.FindByIndex(Index);
	if (kp) {
		TMrbVMERegister * rp = (TMrbVMERegister *) kp->GetAssignedObject();
		if (rp) {
			Int_t intVal = (SubIndex == -1) ? rp->Get() : rp->Get(SubIndex);
			TMrbLofNamedX * lofBitNames = rp->BitNames();
			if (lofBitNames) {
				if (lofBitNames->IsPatternMode() && (intVal > 0)) {
					lofBitNames->Pattern2String(Value, intVal, ":");
					switch (Base) {
						case 8: 	Value += Form(" (%#o)", intVal); return(Value.Data());
						case 10:	Value += Form(" (%d)", intVal); return(Value.Data());
						case 16:	Value += Form(" (%#x)", intVal); return(Value.Data());
					}
				} else {
					TMrbNamedX * bn = lofBitNames->FindByIndex(intVal);
					if (bn) {
						switch (Base) {
							case 8: 	Value = Form("%s (%#o)", bn->GetName(), bn->GetIndex()); return(Value.Data());
							case 10:	Value = Form("%s (%d)", bn->GetName(), bn->GetIndex()); return(Value.Data());
							case 16:	Value = Form("%s (%#x)", bn->GetName(), bn->GetIndex()); return(Value.Data());
						}
					}
				}
			}
			switch (Base) {
				case 8: 	Value = Form("%#o", intVal); return(Value.Data());
				case 10:	Value = Form("%d", intVal); return(Value.Data());
				case 16:	Value = Form("%#x", intVal); return(Value.Data());
			}
		}
	}
	return(Value.Data());
}

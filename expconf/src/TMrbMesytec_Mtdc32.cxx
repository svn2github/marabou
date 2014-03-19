//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbMesytec_Mtdc32.cxx
// Purpose:        MARaBOU configuration: Mesytec modules
// Description:    Implements class methods to handle a Mesytec TDC type MTDC32
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbMesytec_Mtdc32.cxx,v 1.26 2012-01-18 11:11:32 Marabou Exp $
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
#include "TMrbMesytec_Mtdc32.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbMesytec_Mtdc32)

const SMrbNamedXShort kMrbAddressSource[] =
		{
			{	TMrbMesytec_Mtdc32::kAddressBoard,			"board" 	},
			{	TMrbMesytec_Mtdc32::kAddressRegister,		"register" 	},
			{	0,			 								NULL,		}
		};

const SMrbNamedXShort kMrbDataWidth[] =
		{
			{	TMrbMesytec_Mtdc32::kDataWidth8,			"8bit"		},
			{	TMrbMesytec_Mtdc32::kDataWidth16,			"16bit"		},
			{	TMrbMesytec_Mtdc32::kDataWidth32,			"32bit"		},
			{	TMrbMesytec_Mtdc32::kDataWidth64,			"64bit"		},
			{	0,			 								NULL,		}
		};

const SMrbNamedXShort kMrbMultiEvent[] =
		{
			{	TMrbMesytec_Mtdc32::kMultiEvtNo,			"singleEvent"	},
			{	TMrbMesytec_Mtdc32::kMultiEvtYes,			"multiEvent"	},
			{	TMrbMesytec_Mtdc32::kMultiEvt1By1,			"oneByOne"		},
			{	TMrbMesytec_Mtdc32::kMultiEvtNoBerr,		"multiNoBerr"	},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbMarkingType[] =
		{
			{	TMrbMesytec_Mtdc32::kMarkingTypeEvent,		"eventCounter"	},
			{	TMrbMesytec_Mtdc32::kMarkingTypeTs, 		"timeStamp" 	},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbBankOperation[] =
		{
			{	TMrbMesytec_Mtdc32::kBankOprConnected,		"connected" 	},
			{	TMrbMesytec_Mtdc32::kBankOprIndependent,	"independent" 	},
			{	TMrbMesytec_Mtdc32::kBankOprZeroDeadTime, 	"zeroDeadTime" 	},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbAdcResolution[] =
		{
			{	TMrbMesytec_Mtdc32::kAdcRes2k,				"2k"			},
			{	TMrbMesytec_Mtdc32::kAdcRes4k,				"4k"			},
			{	TMrbMesytec_Mtdc32::kAdcRes4kHiRes,			"4k-HiRes"		},
			{	TMrbMesytec_Mtdc32::kAdcRes8k,				"8k"			},
			{	TMrbMesytec_Mtdc32::kAdcRes8kHiRes,			"8k-HiRes"		},
			{	TMrbMesytec_Mtdc32::kAdcDontOverride,		"no"			},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbOutputFormat[] =
		{
			{	TMrbMesytec_Mtdc32::kOutFmtAddr,			"addressed" 	},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbUseGG[] =
		{
			{	TMrbMesytec_Mtdc32::kUseGGnone,				"none"			},
			{	TMrbMesytec_Mtdc32::kUseGG0,				"gg0"			},
			{	TMrbMesytec_Mtdc32::kUseGG1,				"gg1"			},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbInputRange[] =
		{
			{	TMrbMesytec_Mtdc32::kInpRng4V,				"4V"			},
			{	TMrbMesytec_Mtdc32::kInpRng8V,				"8V"			},
			{	TMrbMesytec_Mtdc32::kInpRng10V,				"10V"			},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbEclTerm[] =
		{
			{	TMrbMesytec_Mtdc32::kEclTermOff,			"off"			},
			{	TMrbMesytec_Mtdc32::kEclTermG0,				"gate0"			},
			{	TMrbMesytec_Mtdc32::kEclTermG1,				"gate1"			},
			{	TMrbMesytec_Mtdc32::kEclTermBusy,			"busy"			},
			{	TMrbMesytec_Mtdc32::kEclTermFcl,			"fclear"		},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbEclG1OrOsc[] =
		{
			{	TMrbMesytec_Mtdc32::kEclG1,					"gate1" 		},
			{	TMrbMesytec_Mtdc32::kEclOsc,				"oscillator" 	},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbEclFclOrRts[] =
		{
			{	TMrbMesytec_Mtdc32::kEclFcl,				"fclear"		},
			{	TMrbMesytec_Mtdc32::kEclRts,				"reset ts"		},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbNimG1OrOsc[] =
		{
			{	TMrbMesytec_Mtdc32::kNimG1,					"gate1" 		},
			{	TMrbMesytec_Mtdc32::kNimOsc,				"oscillator" 	},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbNimFclOrRts[] =
		{
			{	TMrbMesytec_Mtdc32::kNimFcl,				"fclear"		},
			{	TMrbMesytec_Mtdc32::kNimRts,				"reset ts"		},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbNimBusy[] =
		{
			{	TMrbMesytec_Mtdc32::kNimBusy,				"busy"			},
			{	TMrbMesytec_Mtdc32::kNimG0Out,				"gate0"			},
			{	TMrbMesytec_Mtdc32::kNimG1Out,				"gate1"			},
			{	TMrbMesytec_Mtdc32::kNimCbusOut,			"cbusOut"		},
			{	TMrbMesytec_Mtdc32::kNimBufferFull,			"bufferFull"		},
			{	TMrbMesytec_Mtdc32::kNimOverThresh,			"overThresh"		},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbPulserStatus[] =
		{
			{	TMrbMesytec_Mtdc32::kPulserOff, 			"off"			},
			{	TMrbMesytec_Mtdc32::kPulserAmpl, 			"amplitude"		},
			{	TMrbMesytec_Mtdc32::kPulserAmplLow, 		"lowAmpl"		},
			{	TMrbMesytec_Mtdc32::kPulserAmplHigh, 		"highAmpl"		},
			{	TMrbMesytec_Mtdc32::kPulserAmplToggle, 		"toggle"		},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbTsSource[] =
		{
			{	TMrbMesytec_Mtdc32::kTstampVME, 			"vme"			},
			{	TMrbMesytec_Mtdc32::kTstampExtern, 			"extern"		},
			{	TMrbMesytec_Mtdc32::kTstampReset, 			"reset-enable"	},
			{	0,			 								NULL,			}
		};

TMrbMesytec_Mtdc32::TMrbMesytec_Mtdc32(const Char_t * ModuleName, UInt_t BaseAddr) :
									TMrbVMEModule(ModuleName, "Mesytec_Mtdc32", BaseAddr,
										0x09,
										TMrbMesytec_Mtdc32::kSegSize,
										1, 32, 1 << 13) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mtdc32
// Purpose:        Create a digitizing adc of type Mesytec MTDC-32
// Arguments:      Char_t * ModuleName      -- name of camac module
//                 UInt_t BaseAddr          -- base addr
// Results:        --
// Exceptions:
// Description:    Creates a vme module of type Mesytec MTDC-32.
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
			SetTitle("Mesytec peak-sensing adc 32 chn 11-13 bit"); 	// store module type
			codeFile = fModuleID.GetName();
			codeFile += ".code";
			if (LoadCodeTemplates(codeFile)) {
				DefineRegisters();
				mTypeBits = TMrbConfig::kModuleVME |
							TMrbConfig::kModuleListMode |
							TMrbConfig::kModuleMultiEvent |
							TMrbConfig::kModuleTimeStamp |
							TMrbConfig::kModuleTdc;
				gMrbConfig->GetLofModuleTypes()->Pattern2String(mType, mTypeBits);
				fModuleType.Set(mTypeBits, mType.Data());
				fDataType = gMrbConfig->GetLofDataTypes()->FindByIndex(TMrbConfig::kDataUInt);
				fNofShortsPerChannel = 2;		// 32 bits
				fNofShortsPerDatum = 1;
				fNofDataBits = 13;
				fBlockReadout = kTRUE;			// module has block readout
				fBlockXfer = kFALSE;
				fRepairRawData = kFALSE;

				fSettingsFile = Form("%sSettings.rc", this->GetName());

				gMrbConfig->AddModule(this);	// append to list of modules
				gDirectory->Append(this);
			} else {
				this->MakeZombie();
			}
		}
	}
}

void TMrbMesytec_Mtdc32::DefineRegisters() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mtdc32::DefineRegisters
// Purpose:        Define module registers
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Defines special registers
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	TMrbLofNamedX * bNames;
	TMrbVMERegister * rp;

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegAddrSource, "AddressSource");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mtdc32::kAddressBoard,
													TMrbMesytec_Mtdc32::kAddressBoard,
													TMrbMesytec_Mtdc32::kAddressRegister);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("AddressSource");
	bNames->AddNamedX(kMrbAddressSource);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegAddrReg, "AddressRegister");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, 0, 0, (2 << 16) - 1);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegModuleId, "ModuleId");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, 0xFF, 0, 0xFF);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegDataWidth, "DataWidth");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mtdc32::kDataWidth32,
													TMrbMesytec_Mtdc32::kDataWidth8,
													TMrbMesytec_Mtdc32::kDataWidth64);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("DataWidth");
	bNames->AddNamedX(kMrbDataWidth);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegMultiEvent, "MultiEvent");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mtdc32::kMultiEvtNo,
													TMrbMesytec_Mtdc32::kMultiEvtNo,
													TMrbMesytec_Mtdc32::kMultiEvtNoBerr);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("MultiEvent");
	bNames->AddNamedX(kMrbMultiEvent);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegXferData, "XferData");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	0,	0,	0x1 << 10);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegMarkingType, "MarkingType");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mtdc32::kMarkingTypeEvent,
													TMrbMesytec_Mtdc32::kMarkingTypeEvent,
													TMrbMesytec_Mtdc32::kMarkingTypeXts);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("MarkingType");
	bNames->AddNamedX(kMrbMarkingType);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegBankOperation, "BankOperation");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mtdc32::kBankOprConnected,
													TMrbMesytec_Mtdc32::kBankOprConnected,
													TMrbMesytec_Mtdc32::kBankOprZeroDeadTime);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("BankOperation");
	bNames->AddNamedX(kMrbBankOperation);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegAdcResolution, "AdcResolution");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mtdc32::kAdcRes4kHiRes,
													TMrbMesytec_Mtdc32::kAdcRes2k,
													TMrbMesytec_Mtdc32::kAdcRes8kHiRes);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("AdcResolution");
	bNames->AddNamedX(kMrbAdcResolution);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegOutputFormat, "OutputFormat");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mtdc32::kOutFmtAddr,
													TMrbMesytec_Mtdc32::kOutFmtAddr,
													TMrbMesytec_Mtdc32::kOutFmtAddr);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("OutputFormat");
	bNames->AddNamedX(kMrbOutputFormat);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegAdcOverride, "AdcOverride");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mtdc32::kAdcDontOverride,
													TMrbMesytec_Mtdc32::kAdcRes2k,
													TMrbMesytec_Mtdc32::kAdcDontOverride);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("AdcOverride");
	bNames->AddNamedX(kMrbAdcResolution);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegHoldDelay, "HoldDelay");
	rp = new TMrbVMERegister(this, 2, kp, 0, 0, 0, TMrbMesytec_Mtdc32::kGGDefaultDelay, 0, 255);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegHoldWidth, "HoldWidth");
	rp = new TMrbVMERegister(this, 2, kp, 0, 0, 0, TMrbMesytec_Mtdc32::kGGDefaultWidth, 0, 255);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegUseGG, "UseGG");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mtdc32::kUseGGnone,
													TMrbMesytec_Mtdc32::kUseGGnone,
													TMrbMesytec_Mtdc32::kUseGGboth);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("UseGG");
	bNames->AddNamedX(kMrbUseGG);
	bNames->SetPatternMode(kTRUE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kTRUE);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegInputRange, "InputRange");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mtdc32::kInpRng4V,
													TMrbMesytec_Mtdc32::kInpRng4V,
													TMrbMesytec_Mtdc32::kInpRng10V);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("InputRange");
	bNames->AddNamedX(kMrbInputRange);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegEclTerm, "EclTerm");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mtdc32::kEclTermOn,
													TMrbMesytec_Mtdc32::kEclTermOff,
													TMrbMesytec_Mtdc32::kEclTermOn);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("EclTerm");
	bNames->AddNamedX(kMrbEclTerm);
	bNames->SetPatternMode(kTRUE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kTRUE);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegEclG1OrOsc, "EclG1OrOsc");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mtdc32::kEclG1,
													TMrbMesytec_Mtdc32::kEclG1,
													TMrbMesytec_Mtdc32::kEclOsc);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("EclG1OrOsc");
	bNames->AddNamedX(kMrbEclG1OrOsc);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegEclFclOrRts, "EclFclOrRts");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mtdc32::kEclFcl,
													TMrbMesytec_Mtdc32::kEclFcl,
													TMrbMesytec_Mtdc32::kEclRts);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("EclFclOrRts");
	bNames->AddNamedX(kMrbEclFclOrRts);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegNimG1OrOsc, "NimG1OrOsc");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mtdc32::kNimG1,
													TMrbMesytec_Mtdc32::kNimG1,
													TMrbMesytec_Mtdc32::kNimOsc);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("NimG1OrOsc");
	bNames->AddNamedX(kMrbNimG1OrOsc);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegNimFclOrRts, "NimFclOrRts");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mtdc32::kNimFcl,
													TMrbMesytec_Mtdc32::kNimFcl,
													TMrbMesytec_Mtdc32::kNimRts);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("NimFclOrRts");
	bNames->AddNamedX(kMrbNimFclOrRts);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegNimBusy, "NimBusy");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mtdc32::kNimBusy,
													TMrbMesytec_Mtdc32::kNimBusy,
													TMrbMesytec_Mtdc32::kNimOverThresh);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("NimBusy");
	bNames->AddNamedX(kMrbNimBusy);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegPulserStatus, "PulserStatus");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mtdc32::kPulserOff,
													TMrbMesytec_Mtdc32::kPulserOff,
													TMrbMesytec_Mtdc32::kPulserAmplToggle);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("PulserStatus");
	bNames->AddNamedX(kMrbPulserStatus);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegTsSource, "TsSource");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mtdc32::kTstampVME,
													TMrbMesytec_Mtdc32::kTstampVME,
													TMrbMesytec_Mtdc32::kTstampExtern | TMrbMesytec_Mtdc32::kTstampReset);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("TsSource");
	bNames->AddNamedX(kMrbTsSource);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegTsDivisor, "TsDivisor");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, 1, 1, (2 << 16) - 1);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegThreshold, "Threshold");
	rp = new TMrbVMERegister(this, TMrbMesytec_Mtdc32::kNofChannels, kp, 0, 0, 0, 0, 0, (2 << 12) - 1);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
}

TEnv * TMrbMesytec_Mtdc32::UseSettings(const Char_t * SettingsFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mtdc32::UseSettings
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
		TString sf = Form("%sSettings.default", this->GetName());
		gMrbLog->Err()	<< "Creating DEFAULT settings file - " << sf << endl;
		gMrbLog->Flush(this->ClassName(), "UseSettings");
		gMrbLog->Err()	<< "Please edit this file and then rename it to \"" << SettingsFile << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "UseSettings");
		this->SaveSettings(sf.Data());
		return(NULL);
	}

	TMrbResource * madcEnv = new TMrbResource("MADC32", fSettingsFile.Data());

	TString moduleName; madcEnv->Get(moduleName, ".ModuleName", "");
	if (moduleName.CompareTo(this->GetName()) != 0) {
		gMrbLog->Err() << "Module name different - \"" << moduleName << "\" (should be " << this->GetName() << ")" << endl;
		gMrbLog->Flush(this->ClassName(), "UseSettings");
		return(NULL);
	}
	moduleName(0,1).ToUpper();
	moduleName.Prepend(".");

	this->UpdateSettings(madcEnv->Get(moduleName.Data(), "UpdateSettings", kFALSE));
	this->SetUpdateInterval(madcEnv->Get(moduleName.Data(), "UpdateInterval", 0));

	this->SetBlockXfer(madcEnv->Get(moduleName.Data(), "BlockXfer", kFALSE));
	this->RepairRawData(madcEnv->Get(moduleName.Data(), "RepairRawData", kFALSE));
	this->SetAddressSource(madcEnv->Get(moduleName.Data(), "AddressSource", kAddressBoard));
	this->SetAddressRegister(madcEnv->Get(moduleName.Data(), "AddressRegister", 0));
	this->SetMcstSignature(madcEnv->Get(moduleName.Data(), "MCSTSignature", 0x0));
	this->SetCbltSignature(madcEnv->Get(moduleName.Data(), "CBLTSignature", 0x0));
	this->SetFirstInChain(madcEnv->Get(moduleName.Data(), "FirstInChain", kTRUE));
	this->SetLastInChain(madcEnv->Get(moduleName.Data(), "LastInChain", kTRUE));
	Int_t mid = madcEnv->Get(moduleName.Data(), "ModuleId", 0xFF);
	if (mid == 0xFF) mid = this->GetSerial();
	this->SetModuleId(mid);
	this->SetDataWidth(madcEnv->Get(moduleName.Data(), "DataWidth", kDataWidth32));
	this->SetMultiEvent(madcEnv->Get(moduleName.Data(), "MultiEvent", kMultiEvtNo));
	this->SetXferData(madcEnv->Get(moduleName.Data(), "XferData", 0));
	this->SetMarkingType(madcEnv->Get(moduleName.Data(), "MarkingType", kMarkingTypeEvent));
	this->SetBankOperation(madcEnv->Get(moduleName.Data(), "BankOperation", kBankOprConnected));
	this->SetAdcResolution(madcEnv->Get(moduleName.Data(), "AdcResolution", kAdcRes4kHiRes));
	this->SetOutputFormat(madcEnv->Get(moduleName.Data(), "OutputFormat", kOutFmtAddr));
	this->SetAdcOverride(madcEnv->Get(moduleName.Data(), "AdcOverride", kAdcDontOverride));
	this->SetSlidingScaleOff(madcEnv->Get(moduleName.Data(), "SlidingScaleOff", kTRUE));
	this->SetSkipOutOfRange(madcEnv->Get(moduleName.Data(), "SkipOutOfRange", kFALSE));
	this->SetIgnoreThresholds(madcEnv->Get(moduleName.Data(), "IgnoreThresh", kFALSE));
	this->SetHoldDelay(madcEnv->Get(moduleName.Data(), "HoldDelay", "0", kGGDefaultDelay), 0);
	this->SetHoldDelay(madcEnv->Get(moduleName.Data(), "HoldDelay", "1", kGGDefaultDelay), 1);
	this->SetHoldWidth(madcEnv->Get(moduleName.Data(), "HoldWidth", "0", kGGDefaultWidth), 0);
	this->SetHoldWidth(madcEnv->Get(moduleName.Data(), "HoldWidth", "1", kGGDefaultWidth), 1);
	this->UseGG(madcEnv->Get(moduleName.Data(), "UseGG", kUseGG0));
	this->SetInputRange(madcEnv->Get(moduleName.Data(), "InputRange", kInpRng4V));
	this->SetEclTerm(madcEnv->Get(moduleName.Data(), "EclTerm", kEclTermOn));
	this->SetEclG1OrOsc(madcEnv->Get(moduleName.Data(), "EclG1OrOsc", kEclG1));
	this->SetEclFclOrRts(madcEnv->Get(moduleName.Data(), "EclFclOrRts", kEclFcl));
	this->SetNimG1OrOsc(madcEnv->Get(moduleName.Data(), "NimG1OrOsc", kNimG1));
	this->SetNimFclOrRts(madcEnv->Get(moduleName.Data(), "NimFclOrRts", kNimFcl));
	this->SetNimBusy(madcEnv->Get(moduleName.Data(), "NimBusy", kNimBusy));
	this->SetBufferThresh(madcEnv->Get(moduleName.Data(), "BufferThresh", 0));
	this->SetTestPulser(madcEnv->Get(moduleName.Data(), "TsSource", kTstampVME));
	this->SetTsDivisor(madcEnv->Get(moduleName.Data(), "TsDivisor", 1));

	for (Int_t i = 0; i < TMrbMesytec_Mtdc32::kNofChannels; i++) {
		this->SetThreshold(madcEnv->Get(moduleName.Data(), "Thresh", Form("%d", i), 0), i);
	}

	Int_t res = this->GetAdcResolution();		// calc actual histo range
	switch (res) {
		case kAdcRes2k: 		fRange = 1 << 11; this->SetXmax(2048.); break;
		case kAdcRes4k:
		case kAdcRes4kHiRes: 	fRange = 1 << 12; this->SetXmax(4096.); break;
		case kAdcRes8k:
		case kAdcRes8kHiRes: 	fRange = 1 << 13; this->SetXmax(8192.); break;
	}
	fXmax = fRange;
	fBinRange = fRange;

	fSettings = madcEnv->Env();
	return(madcEnv->Env());
}

Bool_t TMrbMesytec_Mtdc32::SaveSettings(const Char_t * SettingsFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mtdc32::SaveSettings
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
	TString tf = "Module_Mtdc32.rc.code";
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
						tmpl.Substitute("$outputFormat", this->GetOutputFormat());
						tmpl.Substitute("$adcOverride", this->GetAdcOverride());
						tmpl.Substitute("$slidingScaleOff", this->SlidingScaleIsOff() ? "TRUE" : "FALSE");
						tmpl.Substitute("$skipOutOfRange", this->SkipOutOfRange() ? "TRUE" : "FALSE");
 						tmpl.Substitute("$ignoreThresh", this->IgnoreThresholds() ? "TRUE" : "FALSE");
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%GateGenerator%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.Substitute("$holdDelay0", this->GetHoldDelay(0));
						tmpl.Substitute("$holdDelay1", this->GetHoldDelay(1));
						tmpl.Substitute("$holdWidth0", this->GetHoldWidth(0));
						tmpl.Substitute("$holdWidth1", this->GetHoldWidth(1));
						tmpl.Substitute("$useGateGenerator", this->GetGGUsed());
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%InputOutput%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.Substitute("$inputRange", this->GetInputRange());
						tmpl.Substitute("$eclTerm", this->GetEclTerm());
						tmpl.Substitute("$eclG1OrOsc", this->GetEclG1OrOsc());
						tmpl.Substitute("$eclFclOrRts", this->GetEclFclOrRts());
						tmpl.Substitute("$nimG1OrOsc", this->GetNimG1OrOsc());
						tmpl.Substitute("$nimFclOrRts", this->GetNimFclOrRts());
						tmpl.Substitute("$nimBusy", this->GetNimBusy());
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%TestPulser%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.Substitute("$pulserStatus", this->GetTestPulser());
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%Counters%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.Substitute("$tsSource", this->GetTsSource());
						tmpl.Substitute("$tsDivisor", this->GetTsDivisor());
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%Thresholds%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.WriteCode(settings);
						for (Int_t i = 0; i < TMrbMesytec_Mtdc32::kNofChannels; i++) {
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

Bool_t TMrbMesytec_Mtdc32::MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mtdc32::MakeReadoutCode
// Purpose:        Write a piece of code for a Mesytec MTDC-32
// Arguments:      ofstream & RdoStrm         -- file output stream
//                 EMrbModuleTag TagIndex     -- index of tag word taken from template file
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a Mesytec MTDC-32.
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
			{
				if (this->IsFirstInChain()) fCodeTemplates.InitializeCode("%M%"); else fCodeTemplates.InitializeCode("%N%");
				fCodeTemplates.Substitute("$moduleName", this->GetName());
				fCodeTemplates.Substitute("$nofParams", this->GetNofChannelsUsed());
				fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
				fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
				fCodeTemplates.WriteCode(RdoStrm);
			}
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
				fCodeTemplates.Substitute("$mbsVersion", gEnv->GetValue("TMbsSetup.MbsVersion", "v22"));
				fCodeTemplates.Substitute("$lynxVersion", gEnv->GetValue("TMbsSetup.LynxVersion", "2.5"));
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
				fCodeTemplates.Substitute("$mbsVersion", gEnv->GetValue("TMbsSetup.MbsVersion", "v22"));
				fCodeTemplates.Substitute("$lynxVersion", gEnv->GetValue("TMbsSetup.LynxVersion", "2.5"));
				fCodeTemplates.CopyCode(codeString);
				env->Replace(codeString);
				gSystem->ExpandPathName(codeString);
				gMrbConfig->GetLofRdoLibs()->Add(new TObjString(codeString.Data()));
			}
			break;
	}
	return(kTRUE);
}


Bool_t TMrbMesytec_Mtdc32::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbModuleTag TagIndex,
															TMrbVMEChannel * Channel,
															Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mtdc32::MakeReadoutCode
// Purpose:        Write a piece of code for a Mesytec MTDC-32
// Arguments:      ofstream & RdoStrm           -- file output stream
//                 EMrbModuleTag TagIndex       -- index of tag word taken from template file
//                 TMrbVMEChannel * Channel     -- channel
//                 Int_t Value                  -- value to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a Mesytec MTDC-32.
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

Bool_t TMrbMesytec_Mtdc32::CheckSubeventType(TMrbSubevent * Subevent) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mtdc32::CheckSubeventType
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

void TMrbMesytec_Mtdc32::PrintSettings(ostream & Out) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mtdc32::PrintSettings
// Purpose:        Print settings
// Arguments:      ostream & Out           -- where to print
// Results:        --
// Exceptions:
// Description:    Prints settings to stream.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString value;

	Out << "=========================================================================" << endl;
	Out << " Mesytec MADC32 settings" << endl;
	Out << "-------------------------------------------------------------------------" << endl;
	Out << " Module              : "	<< this->GetName() << endl;
	Out << " Serial              : "	<< this->GetSerial() << endl;
	if (this->SettingsToBeUpdated()) {
		Out << " Update settings     : every " << this->GetUpdateInterval() << " count(s)" << endl;
	} else {
		Out << " Update settings     : never" << endl;
	}
	Out << " Address source      : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegAddrSource) << endl;
	Out << "         register    : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegAddrReg, -1, 16) << endl;
	Out << " Module ID           : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegModuleId) << endl;
	Out << " Data width          : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegDataWidth) << endl;
	Out << " Single/multi event  : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegMultiEvent) << endl;
	Out << " Marking type        : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegMarkingType) << endl;
	Out << " Block tansfer       : "	<< (this->BlockXferEnabled() ? "off" : "on") << endl;
	Out << " Repair raw data     : "	<< (this->RawDataToBeRepaired() ? "off" : "on") << endl;
	Out << " Bank operation      : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegBankOperation) << endl;
	Out << " ADC resolution      : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegAdcResolution) << endl;
	Out << " Output format       : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegOutputFormat) << endl;
	Out << " ADC override        : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegAdcOverride) << endl;
	Out << " Buffer threshold    : "	<< this->GetBufferThresh() << endl;
	Out << " Sliding scale       : "	<< (this->SlidingScaleIsOff() ? "off" : "on") << endl;
	Out << " Skip if out of range: "	<< (this->SkipOutOfRange() ? "yes" : "no") << endl;
	Out << " Ignore thresholds   : "	<< (this->IgnoreThresholds() ? "yes" : "no") << endl;
	for (Int_t b = 0; b <= 1; b++) {
		Int_t hd = this->GetHoldDelay(b);
		Int_t ns;
		if (hd == 0) ns = 25; else if (hd == 1) ns = 150; else ns = hd * 50;
		Int_t hw = this->GetHoldWidth(b);
		Out << " GG bank " << b << " delay     : "	<< hd << " --> " << ns << " ns" << endl;
		Out << "           width     : "	<< hw << " --> " << (hw * 50) << " ns" << endl;
	}
	Out << " Use GG              : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegUseGG) << endl;
	Out << " Input range         : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegInputRange) << endl;
	Out << " ECL termination     : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegEclTerm) << endl;
	Out << "           input     : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegEclG1OrOsc) << endl;
	Out << "    fcl or reset     : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegEclFclOrRts) << endl;
	Out << " NIM       input     : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegNimG1OrOsc) << endl;
	Out << "    fcl or reset     : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegNimFclOrRts) << endl;
	Out << "            busy     : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegNimBusy) << endl;
	Out << " Pulser              : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegPulserStatus) << endl;
	Out << " Timestamp source    : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegTsSource) << endl;
	Out << "          divisor    : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegTsDivisor) << endl;
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

const Char_t * TMrbMesytec_Mtdc32::FormatValue(TString & Value, Int_t Index, Int_t SubIndex, Int_t Base) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mtdc32::FormatValue
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

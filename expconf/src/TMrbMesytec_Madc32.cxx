//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbMesytec_Madc32.cxx
// Purpose:        MARaBOU configuration: Mesytec modules
// Description:    Implements class methods to handle a Mesytec ADC type MADC32
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbMesytec_Madc32.cxx,v 1.5 2009-04-02 11:15:08 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TDirectory.h"
#include "TEnv.h"

#include "TMrbLogger.h"
#include "TMrbConfig.h"
#include "TMrbVMERegister.h"
#include "TMrbVMEChannel.h"
#include "TMrbMesytec_Madc32.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbMesytec_Madc32)

const SMrbNamedXShort kMrbAddressSource[] =
		{
			{	TMrbMesytec_Madc32::kAddressBoard,			"board" 	},
			{	TMrbMesytec_Madc32::kAddressRegister,		"register" 	},
			{	0,			 								NULL,		}
		};

const SMrbNamedXShort kMrbDataLengthFormat[] =
		{
			{	TMrbMesytec_Madc32::kDataLngFmt8,			"8bit"		},
			{	TMrbMesytec_Madc32::kDataLngFmt16,			"16bit"		},
			{	TMrbMesytec_Madc32::kDataLngFmt32,			"32bit"		},
			{	TMrbMesytec_Madc32::kDataLngFmt64,			"64bit"		},
			{	0,			 								NULL,		}
		};

const SMrbNamedXShort kMrbMultiEvent[] =
		{
			{	TMrbMesytec_Madc32::kMultiEvtNo,			"singleEvent"	},
			{	TMrbMesytec_Madc32::kMultiEvtYes,			"multiEvent"	},
			{	TMrbMesytec_Madc32::kMultiEvt1By1,			"oneByOne"		},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbMarkingType[] =
		{
			{	TMrbMesytec_Madc32::kMarkingTypeEvent,		"eventCounter"	},
			{	TMrbMesytec_Madc32::kMarkingTypeTimestamp,	"timeStamp" 	},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbBankOperation[] =
		{
			{	TMrbMesytec_Madc32::kBankOprConnected,		"connected" 	},
			{	TMrbMesytec_Madc32::kBankOprIndependent,	"independent" 	},
			{	TMrbMesytec_Madc32::kBankOprZeroDeadTime, 	"zeroDeadTime" 	},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbAdcResolution[] =
		{
			{	TMrbMesytec_Madc32::kAdcRes2k,				"2k"			},
			{	TMrbMesytec_Madc32::kAdcRes4k,				"4k"			},
			{	TMrbMesytec_Madc32::kAdcRes4kHires,			"4k-Hires"		},
			{	TMrbMesytec_Madc32::kAdcRes8k,				"8k"			},
			{	TMrbMesytec_Madc32::kAdcRes8kHires,			"8k-Hires"		},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbOutputFormat[] =
		{
			{	TMrbMesytec_Madc32::kOutFmtAddr,			"addressed" 	},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbUseGG[] =
		{
			{	TMrbMesytec_Madc32::kUseGG0,				"gg0"			},
			{	TMrbMesytec_Madc32::kUseGG1,				"gg1"			},
			{	TMrbMesytec_Madc32::kUseGGboth,				"both"			},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbInputRange[] =
		{
			{	TMrbMesytec_Madc32::kInpRng4V,				"4V"			},
			{	TMrbMesytec_Madc32::kInpRng8V,				"8V"			},
			{	TMrbMesytec_Madc32::kInpRng10V,				"10V"			},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbEclTerm[] =
		{
			{	TMrbMesytec_Madc32::kEclTermOff,			"off"			},
			{	TMrbMesytec_Madc32::kEclTermGate0,			"gate0"			},
			{	TMrbMesytec_Madc32::kEclTermGate1,			"gate1"			},
			{	TMrbMesytec_Madc32::kEclTermFastClear,		"fclear"		},
			{	TMrbMesytec_Madc32::kEclTermBusy,			"busy"			},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbEclGate1Osc[] =
		{
			{	TMrbMesytec_Madc32::kEclGate1,				"gate1" 		},
			{	TMrbMesytec_Madc32::kEclOscillator,			"oscillator" 	},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbEclFclRes[] =
		{
			{	TMrbMesytec_Madc32::kEclFastClear,			"fclear"		},
			{	TMrbMesytec_Madc32::kEclReset,				"reset"			},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbNimGate1Osc[] =
		{
			{	TMrbMesytec_Madc32::kNimGate1,				"gate1" 		},
			{	TMrbMesytec_Madc32::kNimOscillator,			"oscillator" 	},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbNimFclRes[] =
		{
			{	TMrbMesytec_Madc32::kNimFastClear,			"fclear"		},
			{	TMrbMesytec_Madc32::kNimReset,				"reset"			},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbNimBusy[] =
		{
			{	TMrbMesytec_Madc32::kNimBusy,				"busy"			},
			{	TMrbMesytec_Madc32::kNimGate0Out,			"gate0"			},
			{	TMrbMesytec_Madc32::kNimGate1Out,			"gate1"			},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbPulserStatus[] =
		{
			{	TMrbMesytec_Madc32::kPulserOff, 			"off"			},
			{	TMrbMesytec_Madc32::kPulserAmpl, 			"amplitude"		},
			{	TMrbMesytec_Madc32::kPulserAmplLow, 		"lowAmpl"		},
			{	TMrbMesytec_Madc32::kPulserAmplHigh, 		"highAmpl"		},
			{	TMrbMesytec_Madc32::kPulserAmplToggle, 		"toggle"		},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbTsSource[] =
		{
			{	TMrbMesytec_Madc32::kTstampVME, 			"vme"			},
			{	TMrbMesytec_Madc32::kTstampExtern, 			"extern"		},
			{	TMrbMesytec_Madc32::kTstampReset, 			"reset-enable"	},
			{	0,			 								NULL,			}
		};

TMrbMesytec_Madc32::TMrbMesytec_Madc32(const Char_t * ModuleName, UInt_t BaseAddr) :
									TMrbVMEModule(ModuleName, "Mesytec_Madc32", BaseAddr,
																TMrbMesytec_Madc32::kAddrMod,
																TMrbMesytec_Madc32::kSegSize,
																1, 32, 1 << 13) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Madc32
// Purpose:        Create a digitizing adc of type Mesytec MADC-32
// Arguments:      Char_t * ModuleName      -- name of camac module
//                 UInt_t BaseAddr          -- base addr
// Results:        --
// Exceptions:
// Description:    Creates a vme module of type Mesytec MADC-32.
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
							TMrbConfig::kModuleAdc;
				gMrbConfig->GetLofModuleTypes()->Pattern2String(mType, mTypeBits);
				fModuleType.Set(mTypeBits, mType.Data());
				fDataType = gMrbConfig->GetLofDataTypes()->FindByIndex(TMrbConfig::kDataUInt);
				fNofShortsPerChannel = 2;		// 32 bits
				fNofShortsPerDatum = 1;
				fNofDataBits = 13;
				fBlockReadout = kTRUE;			// module has block readout

				fBlockXfer = kFALSE;

				gMrbConfig->AddModule(this);				// append to list of modules
				gDirectory->Append(this);
			} else {
				this->MakeZombie();
			}
		}
	}
}

void TMrbMesytec_Madc32::DefineRegisters() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Madc32::DefineRegisters
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

	kp = new TMrbNamedX(TMrbMesytec_Madc32::kRegAddrSource, "AddressSource");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Madc32::kAddressBoard,
													TMrbMesytec_Madc32::kAddressBoard,
													TMrbMesytec_Madc32::kAddressRegister);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("AddressSource");
	bNames->AddNamedX(kMrbAddressSource);	
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Madc32::kRegAddrReg, "AddressRegister");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, 0, 0, (2 << 16) - 1);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Madc32::kRegModuleId, "ModuleId");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, 0xFF, 0, 0xFF);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Madc32::kRegBufferLength, "BufferLength");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	0,	0,	0x1 << 14);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Madc32::kRegDataLengthFormat, "DataLengthFormat");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Madc32::kDataLngFmt32,
													TMrbMesytec_Madc32::kDataLngFmt8,
													TMrbMesytec_Madc32::kDataLngFmt64);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("DataLengthFormat");
	bNames->AddNamedX(kMrbDataLengthFormat);	
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Madc32::kRegMultiEvent, "MultiEvent");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Madc32::kMultiEvtNo,
													TMrbMesytec_Madc32::kMultiEvtNo,
													TMrbMesytec_Madc32::kMultiEvt1By1);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("MultiEvent");
	bNames->AddNamedX(kMrbMultiEvent);	
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Madc32::kRegMarkingType, "MarkingType");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Madc32::kMarkingTypeEvent,
													TMrbMesytec_Madc32::kMarkingTypeEvent,
													TMrbMesytec_Madc32::kMarkingTypeTimestamp);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("MarkingType");
	bNames->AddNamedX(kMrbMarkingType);	
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Madc32::kRegBankOperation, "BankOperation");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Madc32::kBankOprConnected,
													TMrbMesytec_Madc32::kBankOprConnected,
													TMrbMesytec_Madc32::kBankOprZeroDeadTime);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("BankOperation");
	bNames->AddNamedX(kMrbBankOperation);	
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Madc32::kRegAdcResolution, "AdcResolution");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Madc32::kAdcRes4kHires,
													TMrbMesytec_Madc32::kAdcRes2k,
													TMrbMesytec_Madc32::kAdcRes8kHires);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("AdcResolution");
	bNames->AddNamedX(kMrbAdcResolution);	
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Madc32::kRegOutputFormat, "OutputFormat");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Madc32::kOutFmtAddr,
													TMrbMesytec_Madc32::kOutFmtAddr,
													TMrbMesytec_Madc32::kOutFmtAddr);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("OutputFormat");
	bNames->AddNamedX(kMrbOutputFormat);	
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Madc32::kRegAdcOverride, "AdcOverride");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Madc32::kAdcDontOverride,
													TMrbMesytec_Madc32::kAdcRes2k,
													TMrbMesytec_Madc32::kAdcDontOverride);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("AdcOverride");
	bNames->AddNamedX(kMrbAdcResolution);	
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Madc32::kRegHoldDelay, "HoldDelay");
	rp = new TMrbVMERegister(this, 2, kp, 0, 0, 0, TMrbMesytec_Madc32::kGGDefaultDelay, 0, 255);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Madc32::kRegHoldWidth, "HoldWidth");
	rp = new TMrbVMERegister(this, 2, kp, 0, 0, 0, TMrbMesytec_Madc32::kGGDefaultWidth, 0, 255);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Madc32::kRegUseGG, "UseGG");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Madc32::kUseGGnone,
													TMrbMesytec_Madc32::kUseGGnone,
													TMrbMesytec_Madc32::kUseGGboth);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("UseGG");
	bNames->AddNamedX(kMrbUseGG);	
	bNames->SetPatternMode(kTRUE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kTRUE);

	kp = new TMrbNamedX(TMrbMesytec_Madc32::kRegInputRange, "InputRange");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Madc32::kInpRng4V,
													TMrbMesytec_Madc32::kInpRng4V,
													TMrbMesytec_Madc32::kInpRng10V);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("InputRange");
	bNames->AddNamedX(kMrbInputRange);	
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Madc32::kRegEclTerm, "EclTerm");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Madc32::kEclTermOn,
													TMrbMesytec_Madc32::kEclTermOff,
													TMrbMesytec_Madc32::kEclTermOn);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("EclTerm");
	bNames->AddNamedX(kMrbEclTerm);	
	bNames->SetPatternMode(kTRUE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kTRUE);

	kp = new TMrbNamedX(TMrbMesytec_Madc32::kRegEclGate1OrOsc, "EclGate1OrOsc");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Madc32::kEclGate1,
													TMrbMesytec_Madc32::kEclGate1,
													TMrbMesytec_Madc32::kEclOscillator);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("EclGate1OrOsc");
	bNames->AddNamedX(kMrbEclGate1Osc);	
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Madc32::kRegEclFclOrReset, "EclFclOrReset");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Madc32::kEclFastClear,
													TMrbMesytec_Madc32::kEclFastClear,
													TMrbMesytec_Madc32::kEclReset);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("EclFclOrReset");
	bNames->AddNamedX(kMrbEclFclRes);	
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Madc32::kRegNimGate1OrOsc, "NimGate1OrOsc");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Madc32::kNimGate1,
													TMrbMesytec_Madc32::kNimGate1,
													TMrbMesytec_Madc32::kNimOscillator);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("NimGate1OrOsc");
	bNames->AddNamedX(kMrbNimGate1Osc);	
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Madc32::kRegNimFclOrReset, "NimFclOrReset");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Madc32::kNimFastClear,
													TMrbMesytec_Madc32::kNimFastClear,
													TMrbMesytec_Madc32::kNimReset);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("NimFclOrReset");
	bNames->AddNamedX(kMrbNimFclRes);	
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Madc32::kRegNimBusy, "NimBusy");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Madc32::kNimBusy,
													TMrbMesytec_Madc32::kNimBusy,
													TMrbMesytec_Madc32::kNimCbusOut);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("NimBusy");
	bNames->AddNamedX(kMrbNimBusy);	
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Madc32::kRegPulserStatus, "PulserStatus");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Madc32::kPulserOff,
													TMrbMesytec_Madc32::kPulserOff,
													TMrbMesytec_Madc32::kPulserAmplToggle);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("PulserStatus");
	bNames->AddNamedX(kMrbPulserStatus);	
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Madc32::kRegTsSource, "TsSource");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Madc32::kTstampVME,
													TMrbMesytec_Madc32::kTstampVME,
													TMrbMesytec_Madc32::kTstampReset);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("TsSource");
	bNames->AddNamedX(kMrbTsSource);	
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Madc32::kRegTsDivisor, "TsDivisor");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, 1, 1, (2 << 16) - 1);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Madc32::kRegThreshold, "Threshold");
	rp = new TMrbVMERegister(this, TMrbMesytec_Madc32::kNofChannels, kp, 0, 0, 0, 0, 0, (2 << 12) - 1);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
}

Bool_t TMrbMesytec_Madc32::UseSettings(const Char_t * SettingsFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Madc32::UseSettings
// Purpose:        Read settings from file
// Arguments:      Char_t * SettingsFile   -- settings file
// Results:        kTRUE/kFALSE
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
		gMrbLog->Out() << "Creating default settings file - " << fSettingsFile << endl;
		gMrbLog->Flush(this->ClassName(), "UseSettings");
		this->SaveSettings(fSettingsFile.Data());
		return(kFALSE);
	}

	TEnv * madcEnv = new TEnv(fSettingsFile.Data());

	TString moduleName = madcEnv->GetValue("MADC32.ModuleName", "unknown");
	if (moduleName.CompareTo(this->GetName()) != 0) {
		gMrbLog->Err() << "Module name different - " << moduleName << " (should be " << this->GetName() << ")" << endl;
		gMrbLog->Flush(this->ClassName(), "UseSettings");
		return(kFALSE);
	}
	moduleName(0,1).ToUpper();

	this->SetBlockXfer(madcEnv->GetValue(Form("MADC32.%s.BlockXfer", moduleName.Data()), kFALSE));
	this->SetAddressSource(madcEnv->GetValue(Form("MADC32.%s.AddressSource", moduleName.Data()), kAddressBoard));
	this->SetAddressRegister(madcEnv->GetValue(Form("MADC32.%s.AddressRegister", moduleName.Data()), 0));
	this->SetModuleId(madcEnv->GetValue(Form("MADC32.%s.ModuleId", moduleName.Data()), 0xFF));
	this->SetBufferLength(madcEnv->GetValue(Form("MADC32.%s.BufferLength", moduleName.Data()), 0));
	this->SetDataLengthFormat(madcEnv->GetValue(Form("MADC32.%s.DataLengthFormat", moduleName.Data()), kDataLngFmt32));
	this->SetMultiEvent(madcEnv->GetValue(Form("MADC32.%s.MultiEvent", moduleName.Data()), kMultiEvtNo));
	this->SetMarkingType(madcEnv->GetValue(Form("MADC32.%s.MarkingType", moduleName.Data()), kMarkingTypeEvent));
	this->SetBankOperation(madcEnv->GetValue(Form("MADC32.%s.BankOperation", moduleName.Data()), kBankOprConnected));
	this->SetAdcResolution(madcEnv->GetValue(Form("MADC32.%s.AdcResolution", moduleName.Data()), kAdcRes4kHires));
	this->SetOutputFormat(madcEnv->GetValue(Form("MADC32.%s.OutputFormat", moduleName.Data()), kOutFmtAddr));
	this->SetAdcOverride(madcEnv->GetValue(Form("MADC32.%s.AdcOverride", moduleName.Data()), kAdcDontOverride));
	this->SetHoldDelay(madcEnv->GetValue(Form("MADC32.%s.HoldDelay.0", moduleName.Data()), kGGDefaultDelay), 0);
	this->SetHoldDelay(madcEnv->GetValue(Form("MADC32.%s.HoldDelay.1", moduleName.Data()), kGGDefaultDelay), 1);
	this->SetHoldWidth(madcEnv->GetValue(Form("MADC32.%s.HoldWidth.0", moduleName.Data()), kGGDefaultWidth), 0);
	this->SetHoldWidth(madcEnv->GetValue(Form("MADC32.%s.HoldWidth.1", moduleName.Data()), kGGDefaultWidth), 1);
	this->UseGG(madcEnv->GetValue(Form("MADC32.%s.UseGG", moduleName.Data()), kUseGG0));
	this->SetInputRange(madcEnv->GetValue(Form("MADC32.%s.InputRange", moduleName.Data()), kInpRng4V));
	this->SetEclTerm(madcEnv->GetValue(Form("MADC32.%s.EclTerm", moduleName.Data()), kEclTermOn));
	this->SetEclGate1OrOsc(madcEnv->GetValue(Form("MADC32.%s.EclGate1OrOsc", moduleName.Data()), kEclGate1));
	this->SetEclFclOrReset(madcEnv->GetValue(Form("MADC32.%s.EclFclOrReset", moduleName.Data()), kEclFastClear));
	this->SetNimGate1OrOsc(madcEnv->GetValue(Form("MADC32.%s.NimGate1OrOsc", moduleName.Data()), kNimGate1));
	this->SetNimFclOrReset(madcEnv->GetValue(Form("MADC32.%s.NimFclOrReset", moduleName.Data()), kNimFastClear));
	this->SetNimBusy(madcEnv->GetValue(Form("MADC32.%s.NimBusy", moduleName.Data()), kNimBusy));
	this->SetTestPulser(madcEnv->GetValue(Form("MADC32.%s.TsSource", moduleName.Data()), kTstampVME));
	this->SetTsDivisor(madcEnv->GetValue(Form("MADC32.%s.TsDivisor", moduleName.Data()), 1));

	for (Int_t i = 0; i < TMrbMesytec_Madc32::kNofChannels; i++) {
		this->SetThreshold(madcEnv->GetValue(Form("MADC32.%s.Thresh.%d", moduleName.Data(), i), 0), i);
	}
	return(kTRUE);
}

Bool_t TMrbMesytec_Madc32::SaveSettings(const Char_t * SettingsFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Madc32::SaveSettings
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
	TString tf = "Module_Mesytec_Madc32.rc.code";
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

	TString moduleUC = this->GetName();
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
						tmpl.Substitute("$moduleName", moduleUC.Data());
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
						tmpl.Substitute("$bufferLength", this->GetBufferLength());
						tmpl.Substitute("$dlengthFormat", this->GetDataLengthFormat());
						tmpl.Substitute("$multiEvent", this->GetMultiEvent());
						tmpl.Substitute("$markingType", this->GetMarkingType());
						tmpl.Substitute("$blockXfer", this->BlockXferEnabled() ? "TRUE" : "FALSE");
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%OperationMode%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.Substitute("$bankOperation", this->GetBankOperation());
						tmpl.Substitute("$adcResolution", this->GetAdcResolution());
						tmpl.Substitute("$outputFormat", this->GetOutputFormat());
						tmpl.Substitute("$adcOverride", this->GetAdcOverride());
						tmpl.Substitute("$slidingScaleOff", this->SlidingScaleIsOff() ? "TRUE" : "FALSE");
						tmpl.Substitute("$skipOutOfRange", this->SkipOutOfRange() ? "TRUE" : "FALSE");
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
						tmpl.Substitute("$eclGate1OrOsc", this->GetEclGate1OrOsc());
						tmpl.Substitute("$eclFclOrReset", this->GetEclFclOrReset());
						tmpl.Substitute("$nimGate1OrOsc", this->GetNimGate1OrOsc());
						tmpl.Substitute("$nimFclOrReset", this->GetNimFclOrReset());
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
						for (Int_t i = 0; i < TMrbMesytec_Madc32::kNofChannels; i++) {
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

Bool_t TMrbMesytec_Madc32::MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Madc32::MakeReadoutCode
// Purpose:        Write a piece of code for a Mesytec MADC-32
// Arguments:      ofstream & RdoStrm         -- file output stream
//                 EMrbModuleTag TagIndex     -- index of tag word taken from template file
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a Mesytec MADC-32.
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

	TString pwd = gSystem->Getenv("PWD");
	if (pwd.Length() == 0) pwd = gSystem->WorkingDirectory();
	TString settings = pwd;
	settings += "/";
	settings += fSettingsFile;

	switch (TagIndex) {
		case TMrbConfig::kModuleDefs:
		case TMrbConfig::kModuleInitCommonCode:
		case TMrbConfig::kModuleInitModule:
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.Substitute("$moduleTitle", this->GetTitle());
			fCodeTemplates.Substitute("$modulePosition", this->GetPosition());
			fCodeTemplates.Substitute("$moduleSerial", this->GetSerial());
			fCodeTemplates.Substitute("$sevtSubtype", subType);
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$baseAddr", (Int_t) this->GetBaseAddr(), 16);
			fCodeTemplates.Substitute("$settingsFile", settings.Data());
			fCodeTemplates.WriteCode(RdoStrm);
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
			fCodeTemplates.WriteCode(RdoStrm);
			break;
		case TMrbConfig::kModuleDefineIncludePaths:
			{
				TString codeString;
				fCodeTemplates.InitializeCode();
				fCodeTemplates.Substitute("$marabouPath", gSystem->Getenv("MARABOU"));
				fCodeTemplates.Substitute("$mbsVersion", gEnv->GetValue("TMbsSetup.MbsVersion", "v22"));
				fCodeTemplates.CopyCode(codeString, " \\\n\t\t\t\t");
				gMrbConfig->GetLofRdoIncludes()->Add(new TObjString(codeString.Data()));
			}
			break;
		case TMrbConfig::kModuleDefineLibraries:
			{
				TString codeString;
				fCodeTemplates.InitializeCode();
				fCodeTemplates.Substitute("$marabouPath", gSystem->Getenv("MARABOU"));
				fCodeTemplates.Substitute("$mbsVersion", gEnv->GetValue("TMbsSetup.MbsVersion", "v22"));
				fCodeTemplates.CopyCode(codeString, " \\\n\t\t\t\t");
				gMrbConfig->GetLofRdoLibs()->Add(new TObjString(codeString.Data()));
			}
			break;
	}
	return(kTRUE);
}


Bool_t TMrbMesytec_Madc32::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbModuleTag TagIndex,
															TMrbVMEChannel * Channel,
															Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Madc32::MakeReadoutCode
// Purpose:        Write a piece of code for a Mesytec MADC-32
// Arguments:      ofstream & RdoStrm           -- file output stream
//                 EMrbModuleTag TagIndex       -- index of tag word taken from template file
//                 TMrbVMEChannel * Channel     -- channel
//                 Int_t Value                  -- value to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a Mesytec MADC-32.
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

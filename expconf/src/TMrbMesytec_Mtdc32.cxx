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
			{	TMrbMesytec_Mtdc32::kMarkingTypeXts, 		"extTimeStamp" 	},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbBankOperation[] =
		{
			{	TMrbMesytec_Mtdc32::kBankOprConnected,		"connected" 	},
			{	TMrbMesytec_Mtdc32::kBankOprIndependent,	"independent" 	},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbTdcResolution[] =
		{
			{	TMrbMesytec_Mtdc32::kTdcRes500,				"500ps"		},
			{	TMrbMesytec_Mtdc32::kTdcRes250,				"250ps"		},
			{	TMrbMesytec_Mtdc32::kTdcRes125,				"125ps"		},
			{	TMrbMesytec_Mtdc32::kTdcRes62_5,			"62.5ps"	},
			{	TMrbMesytec_Mtdc32::kTdcRes31_3,			"31.3ps"	},
			{	TMrbMesytec_Mtdc32::kTdcRes15_6,			"15.6ps"	},
			{	TMrbMesytec_Mtdc32::kTdcRes7_8,				"7.8ps"		},
			{	TMrbMesytec_Mtdc32::kTdcRes3_9,				"3.9ps"		},
			{	0,			 								NULL,		}
		};

const SMrbNamedXShort kMrbOutputFormat[] =
		{
			{	TMrbMesytec_Mtdc32::kOutFmtTimeDiff,		"timeDiff" 	},
			{	TMrbMesytec_Mtdc32::kOutFmtSingleHitFullTs,	"singleHitFullTs" 	},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbFirstHit[] =
		{
			{	TMrbMesytec_Mtdc32::kFirstHitOnly,			"firstHitOnly"	},
			{	TMrbMesytec_Mtdc32::kFirstHitAll,			"allHitsInWindow"	},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbEclTerm[] =
		{
			{	TMrbMesytec_Mtdc32::kEclTermOff,			"off"		},
			{	TMrbMesytec_Mtdc32::kEclTermT0,				"trig0"		},
			{	TMrbMesytec_Mtdc32::kEclTermT1,				"trig1"		},
			{	TMrbMesytec_Mtdc32::kEclTermRes,			"reset"		},
			{	TMrbMesytec_Mtdc32::kEclTermOn,				"on"		},
			{	0,			 								NULL,		}
		};

const SMrbNamedXShort kMrbEclT1Osc[] =
		{
			{	TMrbMesytec_Mtdc32::kEclT1,					"trig1" 		},
			{	TMrbMesytec_Mtdc32::kEclOsc,				"oscillator" 	},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbTrigSelect[] =
		{
			{	TMrbMesytec_Mtdc32::kTrigSelNim,			"NIM"		},
			{	TMrbMesytec_Mtdc32::kTrigSelEcl,			"ECL"		},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbNimT1Osc[] =
		{
			{	TMrbMesytec_Mtdc32::kNimT1,					"trig1" 		},
			{	TMrbMesytec_Mtdc32::kNimOsc,				"oscillator" 	},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbNimBusy[] =
		{
			{	TMrbMesytec_Mtdc32::kNimBusy,				"busy"			},
			{	TMrbMesytec_Mtdc32::kNimCbusOut,			"cbusOut"		},
			{	TMrbMesytec_Mtdc32::kNimBufferFull,			"bufferFull"	},
			{	TMrbMesytec_Mtdc32::kNimOverThresh,			"overThresh"	},
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
										1, 34, 1 << 16) {
//__________________________________________________________________[C++ CTOR]rts
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
			SetTitle("Mesytec time digitizer tdc 32+2 chn 16 bit"); 	// store module type
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
				fNofDataBits = 16;
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
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, 0, 0, 0xFFFF);
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
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	0,	0,	0x7FFF);
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
													TMrbMesytec_Mtdc32::kBankOprIndependent);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("BankOperation");
	bNames->AddNamedX(kMrbBankOperation);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegTdcResolution, "TdcResolution");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mtdc32::kTdcRes500,
													TMrbMesytec_Mtdc32::kTdcRes3_9,
													TMrbMesytec_Mtdc32::kTdcRes500);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("TdcResolution");
	bNames->AddNamedX(kMrbTdcResolution);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegOutputFormat, "OutputFormat");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mtdc32::kOutFmtTimeDiff,
													TMrbMesytec_Mtdc32::kOutFmtTimeDiff,
													TMrbMesytec_Mtdc32::kOutFmtSingleHitFullTs);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("OutputFormat");
	bNames->AddNamedX(kMrbOutputFormat);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegWinStart, "WinStart");
	rp = new TMrbVMERegister(this, 2, kp, 0, 0, 0, TMrbMesytec_Mtdc32::kWinStartDefault, 0, 0x7FFF);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegWinWidth, "WinWidth");
	rp = new TMrbVMERegister(this, 2, kp, 0, 0, 0, TMrbMesytec_Mtdc32::kWinWidthDefault, 0, 0x3FFF);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegFirstHit, "FirstHit");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, 0, 0, 0x3);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegTrigSrcTrig, "TrigSrcTrig");
	rp = new TMrbVMERegister(this, 2, kp, 0, 0, 0, 1, 0, 0x3);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegTrigSrcChan, "TrigSrcChan");
	rp = new TMrbVMERegister(this, 2, kp, 0, 0, 0, 0, 0, 0x3F);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegTrigSrcBank, "TrigSrcBank");
	rp = new TMrbVMERegister(this, 2, kp, 0, 0, 0, 0, 0, 0x3);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegNegEdge, "NegEdge");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, 0, 0, 0x3);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

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

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegEclT1Osc, "EclT1Osc");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mtdc32::kEclT1,
													TMrbMesytec_Mtdc32::kEclT1,
													TMrbMesytec_Mtdc32::kEclOsc);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("EclT1Osc");
	bNames->AddNamedX(kMrbEclT1Osc);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegTrigSelect, "TrigSelect");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mtdc32::kTrigSelNim,
													TMrbMesytec_Mtdc32::kTrigSelNim,
													TMrbMesytec_Mtdc32::kTrigSelEcl);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("TrigSelect");
	bNames->AddNamedX(kMrbTrigSelect);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);
	
	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegNimT1Osc, "NimT1Osc");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mtdc32::kNimT1,
													TMrbMesytec_Mtdc32::kNimT1,
													TMrbMesytec_Mtdc32::kNimOsc);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("NimT1Osc");
	bNames->AddNamedX(kMrbNimT1Osc);
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

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegPulserPattern, "PulserPattern");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, 0, 0, 0xFF);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegInputThresh, "InputThresh");
	rp = new TMrbVMERegister(this, 2, kp, 0, 0, 0, TMrbMesytec_Mtdc32::kInputThreshDefault, 0, 0xFF);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

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
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, 1, 1, 0xFFFF);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegMultHighLimit, "MultHighLimit");
	rp = new TMrbVMERegister(this, 2, kp, 0, 0, 0, TMrbMesytec_Mtdc32::kMultHighLimit0, 0, 0x3F);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	
	kp = new TMrbNamedX(TMrbMesytec_Mtdc32::kRegMultLowLimit, "MultLowLimit");
	rp = new TMrbVMERegister(this, 2, kp, 0, 0, 0, TMrbMesytec_Mtdc32::kMultLowLimit, 0, 0x3F);
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

	TMrbResource * mtdcEnv = new TMrbResource("MTDC32", fSettingsFile.Data());

	TString moduleName; mtdcEnv->Get(moduleName, ".ModuleName", "");
	if (moduleName.CompareTo(this->GetName()) != 0) {
		gMrbLog->Err() << "Module name different - \"" << moduleName << "\" (should be " << this->GetName() << ")" << endl;
		gMrbLog->Flush(this->ClassName(), "UseSettings");
		return(NULL);
	}
	moduleName(0,1).ToUpper();
	moduleName.Prepend(".");

	this->SetBlockXfer(mtdcEnv->Get(moduleName.Data(), "BlockXfer", kFALSE));
	this->RepairRawData(mtdcEnv->Get(moduleName.Data(), "RepairRawData", kFALSE));
	this->SetAddressSource(mtdcEnv->Get(moduleName.Data(), "AddressSource", kAddressBoard));
	this->SetAddressRegister(mtdcEnv->Get(moduleName.Data(), "AddressRegister", 0));
	this->SetMcstSignature(mtdcEnv->Get(moduleName.Data(), "MCSTSignature", 0x0));
	this->SetCbltSignature(mtdcEnv->Get(moduleName.Data(), "CBLTSignature", 0x0));
	this->SetFirstInChain(mtdcEnv->Get(moduleName.Data(), "FirstInChain", kTRUE));
	this->SetLastInChain(mtdcEnv->Get(moduleName.Data(), "LastInChain", kTRUE));
	Int_t mid = mtdcEnv->Get(moduleName.Data(), "ModuleId", 0xFF);
	if (mid == 0xFF) mid = this->GetSerial();
	this->SetModuleId(mid);
	this->SetDataWidth(mtdcEnv->Get(moduleName.Data(), "DataWidth", kDataWidth32));
	this->SetMultiEvent(mtdcEnv->Get(moduleName.Data(), "MultiEvent", kMultiEvtNo));
	this->SetXferData(mtdcEnv->Get(moduleName.Data(), "XferData", 0));
	this->SetMarkingType(mtdcEnv->Get(moduleName.Data(), "MarkingType", kMarkingTypeEvent));
	this->SetBankOperation(mtdcEnv->Get(moduleName.Data(), "BankOperation", kBankOprConnected));
	this->SetTdcResolution(mtdcEnv->Get(moduleName.Data(), "TdcResolution", kTdcRes500));
	this->SetOutputFormat(mtdcEnv->Get(moduleName.Data(), "OutputFormat", kOutFmtTimeDiff));
	this->SetWinStart(mtdcEnv->Get(moduleName.Data(), "WinStart", "0", kWinStartDefault), 0);
	this->SetWinStart(mtdcEnv->Get(moduleName.Data(), "WinStart", "1", kWinStartDefault), 1);
	this->SetWinWidth(mtdcEnv->Get(moduleName.Data(), "WinWidth", "0", kWinWidthDefault), 0);
	this->SetWinWidth(mtdcEnv->Get(moduleName.Data(), "WinWidth", "1", kWinWidthDefault), 1);
	this->SetTrigSrcTrig(mtdcEnv->Get(moduleName.Data(), "TrigSrcTrig", "0", 1), 0);
	this->SetTrigSrcChan(mtdcEnv->Get(moduleName.Data(), "TrigSrcChand", "0", 0), 0);
	this->SetTrigSrcBank(mtdcEnv->Get(moduleName.Data(), "TrigSrcBank", "0", 0), 0);
	this->SetTrigSrcTrig(mtdcEnv->Get(moduleName.Data(), "TrigSrcTrig", "1", 2), 1);
	this->SetTrigSrcChan(mtdcEnv->Get(moduleName.Data(), "TrigSrcChand", "1", 0), 1);
	this->SetTrigSrcBank(mtdcEnv->Get(moduleName.Data(), "TrigSrcBank", "1", 0), 1);
	this->SetFirstHit(mtdcEnv->Get(moduleName.Data(), "FirstHit", 3));
	this->SetNegativeEdge(mtdcEnv->Get(moduleName.Data(), "NegEdge", 3));
	this->SetEclTerm(mtdcEnv->Get(moduleName.Data(), "EclTerm", kEclTermOn));
	this->SetEclT1Osc(mtdcEnv->Get(moduleName.Data(), "EclT1Osc", kEclT1));
	this->SetTrigSelect(mtdcEnv->Get(moduleName.Data(), "TrigSelect", kTrigSelNim));
	this->SetNimT1Osc(mtdcEnv->Get(moduleName.Data(), "NimT1Osc", kNimT1));
	this->SetNimBusy(mtdcEnv->Get(moduleName.Data(), "NimBusy", kNimBusy));
	this->SetBufferThresh(mtdcEnv->Get(moduleName.Data(), "BufferThresh", 0));
	this->SetPulserStatus(mtdcEnv->Get(moduleName.Data(), "PulserStatus", kFALSE));
	this->SetPulserPattern(mtdcEnv->Get(moduleName.Data(), "PulserPattern", 0));
	this->SetInputThresh(mtdcEnv->Get(moduleName.Data(), "InputThresh", "0", kInputThreshDefault), 0);
	this->SetInputThresh(mtdcEnv->Get(moduleName.Data(), "InputThresh", "1", kInputThreshDefault), 1);
	this->SetTsSource(mtdcEnv->Get(moduleName.Data(), "TsSource", kTstampVME));
	this->SetTsDivisor(mtdcEnv->Get(moduleName.Data(), "TsDivisor", 1));
	this->SetMultHighLimit(mtdcEnv->Get(moduleName.Data(), "MultHighLimit", "0", kMultHighLimit0), 0);
	this->SetMultHighLimit(mtdcEnv->Get(moduleName.Data(), "MultHighLimit", "1", kMultHighLimit1), 1);
	this->SetMultLowLimit(mtdcEnv->Get(moduleName.Data(), "MultLowLimit", "0", kMultLowLimit), 0);
	this->SetMultLowLimit(mtdcEnv->Get(moduleName.Data(), "MultLowLimit", "1", kMultLowLimit), 1);

	fSettings = mtdcEnv->Env();
	return(mtdcEnv->Env());
}

Bool_t TMrbMesytec_Mtdc32::UpdateSettings() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mtdc32::UpdateSettings
// Purpose:        Update settings file if needed
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Check version of settings file and perform update if needed
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString settingsVersion;
	TMrbResource * mtdcEnv = new TMrbResource("MTDC32", fSettingsFile.Data());
	mtdcEnv->Get(settingsVersion, ".SettingsVersion", "");
	if (settingsVersion.CompareTo("10.2014") != 0) {
		gMrbLog->Out() << "Settings file \"" << fSettingsFile << "\" has wrong (old?) version \"" << settingsVersion << "\" (should be 10.2014)" << endl;
		gMrbLog->Flush(this->ClassName(), "UpdateSettings", setblue);
		TString oldFile = fSettingsFile;
		oldFile += "-old";
		gSystem->Rename(fSettingsFile, oldFile);
		this->SaveSettings(fSettingsFile.Data());
		gMrbLog->Out() << "Settings file \"" << fSettingsFile << "\" converted to version 10.2014, old one renamed to \"" << oldFile << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "UpdateSettings", setblue);
	}
	return kTRUE;
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
						tmpl.Substitute("$tdcResolution", this->GetTdcResolution());
						tmpl.Substitute("$outputFormat", this->GetOutputFormat());
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%Trigger%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.Substitute("$winStart0", this->GetWinStart(0));
						tmpl.Substitute("$winStart1", this->GetWinStart(1));
						tmpl.Substitute("$winWidth0", this->GetWinWidth(0));
						tmpl.Substitute("$winWidth1", this->GetWinWidth(1));
						tmpl.Substitute("$trigSrcTrig0", this->GetTrigSrcTrig(0));
						tmpl.Substitute("$trigSrcChan0", this->GetTrigSrcChan(0));
						tmpl.Substitute("$trigSrcBank0", this->GetTrigSrcBank(0));
						tmpl.Substitute("$trigSrcTrig1", this->GetTrigSrcTrig(1));
						tmpl.Substitute("$trigSrcChan1", this->GetTrigSrcChan(1));
						tmpl.Substitute("$trigSrcBank1", this->GetTrigSrcBank(1));
						tmpl.Substitute("$firstHit", this->GetFirstHit());
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%InputOutput%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.Substitute("$negEdge", this->GetNegativeEdge());
						tmpl.Substitute("$eclTerm", this->GetEclTerm());
						tmpl.Substitute("$eclT1Osc", this->GetEclT1Osc());
						tmpl.Substitute("$trigSelect", this->GetTrigSelect());
						tmpl.Substitute("$nimT1Osc", this->GetNimT1Osc());
						tmpl.Substitute("$nimBusy", this->GetNimBusy());
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%PulserThresh%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.Substitute("$pulserStatus", this->PulserOn());
						tmpl.Substitute("$pulserPattern", this->GetPulserPattern());
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
	Out << " Mesytec MTDC32 settings" << endl;
	Out << "-------------------------------------------------------------------------" << endl;
	Out << " Module                 : "	<< this->GetName() << endl;
	Out << " Serial                 : "	<< this->GetSerial() << endl;
	Out << " Address source         : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegAddrSource) << endl;
	Out << "         register       : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegAddrReg, -1, 16) << endl;
	Out << " Module ID              : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegModuleId) << endl;
	Out << " Data width             : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegDataWidth) << endl;
	Out << " Single/multi event     : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegMultiEvent) << endl;
	Out << " Marking type           : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegMarkingType) << endl;
	Out << " Block tansfer          : "	<< (this->BlockXferEnabled() ? "on" : "off") << endl;
	Out << " Repair raw data        : "	<< (this->RawDataToBeRepaired() ? "on" : "off") << endl;
	Out << " Bank operation         : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegBankOperation) << endl;
	Out << " TDC resolution         : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegTdcResolution) << endl;
	Out << " Output format          : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegOutputFormat) << endl;
	Out << " Buffer threshold       : "	<< this->GetBufferThresh() << endl;
	for (Int_t b = 0; b <= 1; b++) {
		Int_t ws = this->GetWinStart(b);
		Out << " Window start bank" << b << "     : " << ws << " ns" << endl;
		Int_t ww = this->GetWinWidth(b);
		Out << " Window width           : " << ww << " ns" << endl;
		Int_t tst = this->GetTrigSrcTrig(b);
		Int_t tsc = this->GetTrigSrcChan(b);
		Int_t tsb = this->GetTrigSrcBank(b);
		Out << " Trigger source         : trig=" << tst << ", chan=" << tsc << ", bank=" << tsb << endl;
	}
	Int_t fh = this->GetFirstHit();
	Out << " First hit              : "	<< fh << ", bank0=" << ((fh & 1) ? "1st hit only" : "all hits") << ", bank1=" << ((fh & 2) ? "1st hit only" : "all hits") << endl;
	Int_t ne = this->GetNegativeEdge();
	Out << " Negative Edge          : "	<< ne << ", bank0=" << ((ne & 1) ? "on" : "off") << ", bank1=" << ((ne & 2) ? "on" : "off") << endl;
	Out << " ECL termination        : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegEclTerm) << endl;
	Out << "           input        : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegEclT1Osc) << endl;
	Out << " Trigger selec          : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegTrigSelect) << endl;
	Out << " NIM       input        : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegNimT1Osc) << endl;
	Out << "            busy        : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegNimBusy) << endl;
	Out << " Pulser status          : "	<< (this->PulserOn() ? "on" : "off") << endl;
	Out << "        pattern         : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegPulserPattern) << endl;
	Out << " Timestamp source       : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegTsSource) << endl;
	Out << "          divisor       : "	<< this->FormatValue(value, TMrbMesytec_Mtdc32::kRegTsDivisor) << endl;
	Out << " Multiplicity limit high: bank 0: " << this->GetMultHighLimit(0) << endl;
	Out << "                     low: bank 0: " << this->GetMultLowLimit(0) << endl;
	Out << "              limit high: bank 1: " << this->GetMultHighLimit(1) << endl;
	Out << "                     low: bank 1: " << this->GetMultLowLimit(1) << endl;
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

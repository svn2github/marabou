//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbMesytec_Mdpp16.cxx
// Purpose:        MARaBOU configuration: Mesytec modules
// Description:    Implements class methods to handle a Mesytec DPP-16
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbMesytec_Mdpp16.cxx,v 1.26 2012-01-18 11:11:32 Marabou Exp $
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
#include "TMrbMesytec_Mdpp16.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbMesytec_Mdpp16)

const SMrbNamedXShort kMrbAddressSource[] =
		{
			{	TMrbMesytec_Mdpp16::kAddressBoard,			"board" 	},
			{	TMrbMesytec_Mdpp16::kAddressRegister,		"register" 	},
			{	0,			 								NULL,		}
		};

const SMrbNamedXShort kMrbDataWidth[] =
		{
			{	TMrbMesytec_Mdpp16::kDataWidth8,			"8bit"		},
			{	TMrbMesytec_Mdpp16::kDataWidth16,			"16bit"		},
			{	TMrbMesytec_Mdpp16::kDataWidth32,			"32bit"		},
			{	TMrbMesytec_Mdpp16::kDataWidth64,			"64bit"		},
			{	0,			 								NULL,		}
		};

const SMrbNamedXShort kMrbMultiEvent[] =
		{
			{	TMrbMesytec_Mdpp16::kMultiEvtNo,			"singleEvent"	},
			{	TMrbMesytec_Mdpp16::kMultiEvtYes,			"multiEvent"	},
			{	TMrbMesytec_Mdpp16::kMultiEvt1By1,			"oneByOne"		},
			{	TMrbMesytec_Mdpp16::kMultiEvtNoBerr,		"multiNoBerr"	},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbMarkingType[] =
		{
			{	TMrbMesytec_Mdpp16::kMarkingTypeEvent,		"eventCounter"	},
			{	TMrbMesytec_Mdpp16::kMarkingTypeTs, 		"timeStamp" 	},
			{	TMrbMesytec_Mdpp16::kMarkingTypeXts, 		"extTimeStamp" 	},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbAdcResolution[] =
		{
			{	TMrbMesytec_Mdpp16::kAdcRes16,				"16bit"		},
			{	TMrbMesytec_Mdpp16::kAdcRes15,				"15bit"		},
			{	TMrbMesytec_Mdpp16::kAdcRes14,				"14bit"		},
			{	TMrbMesytec_Mdpp16::kAdcRes13,				"13bit"		},
			{	TMrbMesytec_Mdpp16::kAdcRes12,				"12bit"		},
			{	0,			 								NULL,		}
		};

const SMrbNamedXShort kMrbTdcResolution[] =
		{
			{	TMrbMesytec_Mdpp16::kTdcRes781_32,			"781ps"		},
			{	TMrbMesytec_Mdpp16::kTdcRes391_64,			"391ps"		},
			{	TMrbMesytec_Mdpp16::kTdcRes195_128,			"195ps"		},
			{	TMrbMesytec_Mdpp16::kTdcRes98_256,			"98ps"	},
			{	TMrbMesytec_Mdpp16::kTdcRes49_512,			"49ps"	},
			{	TMrbMesytec_Mdpp16::kTdcRes24_1024,			"24ps"	},
			{	0,			 								NULL,		}
		};

const SMrbNamedXShort kMrbOutputFormat[] =
		{
			{	TMrbMesytec_Mdpp16::kOutFmtStandard,		"ampl&time" 	},
			{	TMrbMesytec_Mdpp16::kOutFmtAmplitude,		"amplitude" 	},
			{	TMrbMesytec_Mdpp16::kOutFmtTime,			"time" 	},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbFirstHit[] =
		{
			{	TMrbMesytec_Mdpp16::kFirstHitOnly,			"firstHitOnly"	},
			{	TMrbMesytec_Mdpp16::kFirstHitAll,			"allHitsInWindow"	},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbEcl3[] =
		{
			{	TMrbMesytec_Mdpp16::kEcl3Off,			"off"		},
			{	TMrbMesytec_Mdpp16::kEcl3Trig0,			"trig0"		},
			{	TMrbMesytec_Mdpp16::kEcl3UnTerm,		"unterm"	},
			{	0,			 							NULL,		}
		};

const SMrbNamedXShort kMrbEcl2[] =
		{
			{	TMrbMesytec_Mdpp16::kEcl2Off,			"off" 		},
			{	TMrbMesytec_Mdpp16::kEcl2Sync,			"sync" 		},
			{	TMrbMesytec_Mdpp16::kEcl2Trig1,			"trig1" 	},
			{	TMrbMesytec_Mdpp16::kEcl2UnTerm,		"unterm"	},
			{	0,			 							NULL,		}
		};

const SMrbNamedXShort kMrbEcl1[] =
		{
			{	TMrbMesytec_Mdpp16::kEcl1Off,			"off" 		},
			{	TMrbMesytec_Mdpp16::kEcl1Reset,			"reset" 	},
			{	TMrbMesytec_Mdpp16::kEcl1UnTerm,		"unterm"	},
			{	0,			 							NULL,		}
		};

const SMrbNamedXShort kMrbEcl0[] =
		{
			{	TMrbMesytec_Mdpp16::kEcl0Off,			"off" 		},
			{	TMrbMesytec_Mdpp16::kEcl0Busy,			"reset" 	},
			{	TMrbMesytec_Mdpp16::kEcl0DataReady,		"data ready"	},
			{	0,			 							NULL,		}
		};

const SMrbNamedXShort kMrbNim4[] =
		{
			{	TMrbMesytec_Mdpp16::kNim4Off,			"off"		},
			{	TMrbMesytec_Mdpp16::kNim4Trig0,			"trig0"		},
			{	0,			 							NULL,		}
		};

const SMrbNamedXShort kMrbNim3[] =
		{
			{	TMrbMesytec_Mdpp16::kNim3Off,			"off"		},
			{	TMrbMesytec_Mdpp16::kNim3Sync,			"sync"		},
			{	0,			 							NULL,		}
		};

const SMrbNamedXShort kMrbNim2[] =
		{
			{	TMrbMesytec_Mdpp16::kNim2Off,			"off"		},
			{	TMrbMesytec_Mdpp16::kNim2Trig1,			"trig1"		},
			{	TMrbMesytec_Mdpp16::kNim2Reset,			"reset"		},
			{	0,			 							NULL,		}
		};

const SMrbNamedXShort kMrbNim0[] =
		{
			{	TMrbMesytec_Mdpp16::kNim0Off,			"off"		},
			{	TMrbMesytec_Mdpp16::kNim0Cbus,			"cbus"		},
			{	TMrbMesytec_Mdpp16::kNim0BusyOut,		"busy out"		},
			{	TMrbMesytec_Mdpp16::kNim0DataReady,		"data ready"	},
			{	0,			 							NULL,		}
		};

const SMrbNamedXShort kMrbPulserStatus[] =
		{
			{	TMrbMesytec_Mdpp16::kPulserOff, 		"off"			},
			{	TMrbMesytec_Mdpp16::kPulserOn, 			"on"		},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbMonitor[] =
		{
			{	TMrbMesytec_Mdpp16::kMonitorOff, 		"off"			},
			{	TMrbMesytec_Mdpp16::kMonitorOn,			"on"		},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbTsSource[] =
		{
			{	TMrbMesytec_Mdpp16::kTstampVME, 			"vme"			},
			{	TMrbMesytec_Mdpp16::kTstampExtern, 			"extern"		},
			{	TMrbMesytec_Mdpp16::kTstampReset, 			"reset enable"	},
			{	0,			 								NULL,			}
		};

const SMrbNamedXShort kMrbBaseLineRes[] =
		{
			{	TMrbMesytec_Mdpp16::kBaseLineResSoft, 		"soft"			},
			{	TMrbMesytec_Mdpp16::kBaseLineResStrict, 	"strict"		},
			{	0,			 								NULL,			}
		};

TMrbMesytec_Mdpp16::TMrbMesytec_Mdpp16(const Char_t * ModuleName, UInt_t BaseAddr) :
									TMrbVMEModule(ModuleName, "Mesytec_Mdpp16", BaseAddr,
										0x09,
										TMrbMesytec_Mdpp16::kSegSize,
										1, 2 * kNofChannels, 1 << 16) {
//__________________________________________________________________[C++ CTOR]rts
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mdpp16
// Purpose:        Create a digitizing adc of type Mesytec MDPP-16
// Arguments:      Char_t * ModuleName      -- name of camac module
//                 UInt_t BaseAddr          -- base addr
// Results:        --
// Exceptions:
// Description:    Creates a vme module of type Mesytec MDPP-16.
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
			SetTitle("Mesytec time digitizer dpp 16+16 chn 12-16 bit"); 	// stopetry pretzellre module type
			codeFile = fModuleID.GetName();
			codeFile += ".code";
			if (LoadCodeTemplates(codeFile)) {
				DefineRegisters();
				mTypeBits = TMrbConfig::kModuleVME |
							TMrbConfig::kModuleListMode |
							TMrbConfig::kModuleMultiEvent |
							TMrbConfig::kModuleTimeStamp |
							TMrbConfig::kModuleDig;
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
				
				fMCSTSignature = 0;
				fCBLTSignature = 0;
				fFirstInChain = kFALSE;
				fLastInChain = kFALSE;

				fAllTFIntDiff = kValueToBeInit;
				fAllPoleZero = kValueToBeInit;
				fAllGain = kValueToBeInit;
				fAllThresh = kValueToBeInit;
				fAllShaping = kValueToBeInit;
				fAllBaseLineRes = kValueToBeInit;
				fAllResetTime = kValueToBeInit;
				fAllRiseTime = kValueToBeInit;
				fAllRiseTime = kValueToBeInit;

				gMrbConfig->AddModule(this);	// append to list of modules
//				gDirectory->Append(this);
			} else {
				this->MakeZombie();
			}
		}
	}
}

void TMrbMesytec_Mdpp16::DefineRegisters() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mdpp16::DefineRegisters
// Purpose:        Define module registers
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Defines special registers
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t verboseMode = gMrbConfig->IsVerbose();
	
	TMrbNamedX * kp;
	TMrbLofNamedX * bNames;
	TMrbVMERegister * rp;

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegAddrSource, "AddressSource");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mdpp16::kAddressBoard,
													TMrbMesytec_Mdpp16::kAddressBoard,
													TMrbMesytec_Mdpp16::kAddressRegister);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("AddressSource");
	bNames->AddNamedX(kMrbAddressSource);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegAddrReg, "AddressRegister");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, 0, 0, 0xFFFF);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegModuleId, "ModuleId");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, 0xFF, 0, 0xFF);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegDataWidth, "DataWidth");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mdpp16::kDataWidth32,
													TMrbMesytec_Mdpp16::kDataWidth8,
													TMrbMesytec_Mdpp16::kDataWidth64);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("DataWidth");
	bNames->AddNamedX(kMrbDataWidth);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegMultiEvent, "MultiEvent");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mdpp16::kMultiEvtNo,
													TMrbMesytec_Mdpp16::kMultiEvtNo,
													TMrbMesytec_Mdpp16::kMultiEvtNoBerr);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("MultiEvent");
	bNames->AddNamedX(kMrbMultiEvent);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegXferData, "XferData");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	0,	0,	0x7FFF);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegMarkingType, "MarkingType");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mdpp16::kMarkingTypeEvent,
													TMrbMesytec_Mdpp16::kMarkingTypeEvent,
													TMrbMesytec_Mdpp16::kMarkingTypeXts);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("MarkingType");
	bNames->AddNamedX(kMrbMarkingType);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegAdcResolution, "AdcResolution");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mdpp16::kAdcRes12,
													TMrbMesytec_Mdpp16::kAdcRes16,
													TMrbMesytec_Mdpp16::kAdcRes12);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("AdcResolution");
	bNames->AddNamedX(kMrbAdcResolution);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegTdcResolution, "TdcResolution");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mdpp16::kTdcRes781_32,
													TMrbMesytec_Mdpp16::kTdcRes24_1024,
													TMrbMesytec_Mdpp16::kTdcRes781_32);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("TdcResolution");
	bNames->AddNamedX(kMrbTdcResolution);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegOutputFormat, "OutputFormat");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mdpp16::kOutFmtStandard,
													TMrbMesytec_Mdpp16::kOutFmtStandard,
													TMrbMesytec_Mdpp16::kOutFmtTime);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("OutputFormat");
	bNames->AddNamedX(kMrbOutputFormat);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegWinStart, "WinStart");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, kWinStartDefault, 0, 0x7FFF);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegWinWidth, "WinWidth");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, kWinWidthDefault, 0, 0x3FFF);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegFirstHit, "FirstHit");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mdpp16::kFirstHitOnly,
													TMrbMesytec_Mdpp16::kFirstHitAll,
													TMrbMesytec_Mdpp16::kFirstHitOnly);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("FirstHit");
	bNames->AddNamedX(kMrbFirstHit);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegTrigSource, "TrigSource");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, 1, 0, 0x3FF);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegTrigOutput, "TrigOutput");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, 0x100, 0, 0x3FF);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegEcl3, "Ecl3");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mdpp16::kEcl3Off,
													TMrbMesytec_Mdpp16::kEcl3Off,
													TMrbMesytec_Mdpp16::kEcl3Trig0 | TMrbMesytec_Mdpp16::kEcl3UnTerm);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("Ecl3");
	bNames->AddNamedX(kMrbEcl3);
	bNames->SetPatternMode(kTRUE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kTRUE);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegEcl2, "Ecl2");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mdpp16::kEcl2Off,
													TMrbMesytec_Mdpp16::kEcl2Off,
													TMrbMesytec_Mdpp16::kEcl2Sync | TMrbMesytec_Mdpp16::kEcl2Trig1 | TMrbMesytec_Mdpp16::kEcl2UnTerm);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("Ecl2");
	bNames->AddNamedX(kMrbEcl2);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegEcl1, "Ecl1");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mdpp16::kEcl1Off,
													TMrbMesytec_Mdpp16::kEcl1Off,
													TMrbMesytec_Mdpp16::kEcl1Reset | TMrbMesytec_Mdpp16::kEcl1UnTerm);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("Ecl1");
	bNames->AddNamedX(kMrbEcl1);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegEcl0, "Ecl0");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mdpp16::kEcl0Off,
													TMrbMesytec_Mdpp16::kEcl0Off,
													TMrbMesytec_Mdpp16::kEcl0Busy | TMrbMesytec_Mdpp16::kEcl0DataReady);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("Ecl0");
	bNames->AddNamedX(kMrbEcl0);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegNim4, "Nim4");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mdpp16::kNim4Trig0,
													TMrbMesytec_Mdpp16::kNim4Off,
													TMrbMesytec_Mdpp16::kNim4Trig0);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("Nim4");
	bNames->AddNamedX(kMrbNim4);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegNim3, "Nim3");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mdpp16::kNim3Off,
													TMrbMesytec_Mdpp16::kNim3Off,
													TMrbMesytec_Mdpp16::kNim3Sync);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("Nim3");
	bNames->AddNamedX(kMrbNim3);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegNim2, "Nim2");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mdpp16::kNim2Trig1,
													TMrbMesytec_Mdpp16::kNim2Off,
													TMrbMesytec_Mdpp16::kNim2Trig1 | TMrbMesytec_Mdpp16::kNim2Reset);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("Nim2");
	bNames->AddNamedX(kMrbNim2);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegNim0, "Nim0");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mdpp16::kNim0Off,
													TMrbMesytec_Mdpp16::kNim0Off,
													TMrbMesytec_Mdpp16::kNim0Cbus | TMrbMesytec_Mdpp16::kNim0BusyOut | TMrbMesytec_Mdpp16::kNim0DataReady);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("Nim0");
	bNames->AddNamedX(kMrbNim0);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegPulserStatus, "Pulser Status");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mdpp16::kPulserOff,
													TMrbMesytec_Mdpp16::kPulserOff,
													TMrbMesytec_Mdpp16::kPulserOn);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("PulserStatus");
	bNames->AddNamedX(kMrbPulserStatus);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegPulserAmplitude, "Pulser Amplitude");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, TMrbMesytec_Mdpp16::kPulserMaxAmpl, 0, TMrbMesytec_Mdpp16::kPulserMaxAmpl);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegMonitor, "Monitor");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mdpp16::kMonitorOff,
													TMrbMesytec_Mdpp16::kMonitorOff,
													TMrbMesytec_Mdpp16::kMonitorOn);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("Monitor");
	bNames->AddNamedX(kMrbMonitor);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegMonitorChannel, "Monitor Channel");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, 0, 0, 15);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegMonitorWave, "Monitor Wave");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, 0, 0, 3);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegTsSource, "TsSource");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbMesytec_Mdpp16::kTstampVME,
													TMrbMesytec_Mdpp16::kTstampVME,
													TMrbMesytec_Mdpp16::kTstampExtern | TMrbMesytec_Mdpp16::kTstampReset);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("TsSource");
	bNames->AddNamedX(kMrbTsSource);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegTsDivisor, "TsDivisor");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, 1, 1, 0xFFFF);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegMultHighLimit, "MultHighLimit");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, kMultHighLimit, 0, 0xFF);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	if (verboseMode) { rp->Print(); cout << endl; }
	
	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegMultLowLimit, "MultLowLimit");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, kMultLowLimit, 0, 0xFF);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegTFIntDiff, "TFIntDiff");
	rp = new TMrbVMERegister(this, kNofChannelPairs, kp, 0, 0, 0, 2, 2, 0x7F);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegPoleZero, "PoleZero");
	rp = new TMrbVMERegister(this, kNofChannels, kp, 2, 0, 0, 0x40, 0x40, 0xFFFF);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegGain, "Gain");
	rp = new TMrbVMERegister(this, kNofChannelPairs, kp, 0, 0, 0, 200, 0, 25000);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegThresh, "Thresh0ld");
	rp = new TMrbVMERegister(this, kNofChannels, kp, 2, 0, 0, 0xFF, 0, 0xFFFF);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegShaping, "Shaping time");
	rp = new TMrbVMERegister(this, kNofChannelPairs, kp, 0, 0, 0, 0x64, 8, 2000);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegBaseLineRes, "Baseline restore");
	rp = new TMrbVMERegister(this, kNofChannelPairs, kp, 0, 0, 0, TMrbMesytec_Mdpp16::kBaseLineResOff,
													TMrbMesytec_Mdpp16::kBaseLineResOff,
													TMrbMesytec_Mdpp16::kBaseLineResStrict);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("BaseLineRes");
	bNames->AddNamedX(kMrbBaseLineRes);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegResetTime, "Reset time");
	rp = new TMrbVMERegister(this, kNofChannelPairs, kp, 0, 0, 0, 16, 0, 0x7F);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	if (verboseMode) { rp->Print(); cout << endl; }

	kp = new TMrbNamedX(TMrbMesytec_Mdpp16::kRegRiseTime, "Rise time");
	rp = new TMrbVMERegister(this, kNofChannelPairs, kp, 0, 0, 0, 0, 0, 0x7F);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	if (verboseMode) { rp->Print(); cout << endl; }
}

TEnv * TMrbMesytec_Mdpp16::UseSettings(const Char_t * SettingsFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mdpp16::UseSettings
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

	TMrbResource * mdppEnv = new TMrbResource("MDPP16", fSettingsFile.Data());

	TString moduleName; mdppEnv->Get(moduleName, ".ModuleName", "");
	if (moduleName.CompareTo(this->GetName()) != 0) {
		gMrbLog->Err() << "Module name different - \"" << moduleName << "\" (should be " << this->GetName() << ")" << endl;
		gMrbLog->Flush(this->ClassName(), "UseSettings");
		return(NULL);
	}
	moduleName(0,1).ToUpper();
	moduleName.Prepend(".");

	this->SetBlockXfer(mdppEnv->Get(moduleName.Data(), "BlockXfer", kFALSE));
	this->RepairRawData(mdppEnv->Get(moduleName.Data(), "RepairRawData", kFALSE));
	this->SetAddressSource(mdppEnv->Get(moduleName.Data(), "AddressSource", TMrbMesytec_Mdpp16::kAddressBoard));
	this->SetAddressRegister(mdppEnv->Get(moduleName.Data(), "AddressRegister", 0));
	this->SetMcstSignature(mdppEnv->Get(moduleName.Data(), "MCSTSignature", (Int_t) fMCSTSignature));
	this->SetCbltSignature(mdppEnv->Get(moduleName.Data(), "CBLTSignature", (Int_t) fCBLTSignature));
	this->SetFirstInChain(mdppEnv->Get(moduleName.Data(), "FirstInChain", fFirstInChain));
	this->SetLastInChain(mdppEnv->Get(moduleName.Data(), "LastInChain", fLastInChain));
	Int_t mid = mdppEnv->Get(moduleName.Data(), "ModuleId", 0xFF);
	if (mid == 0xFF) mid = this->GetSerial();
	this->SetModuleId(mid);
	this->SetDataWidth(mdppEnv->Get(moduleName.Data(), "DataWidth", TMrbMesytec_Mdpp16::kDataWidth32));
	this->SetMultiEvent(mdppEnv->Get(moduleName.Data(), "MultiEvent", TMrbMesytec_Mdpp16::kMultiEvtNo));
	this->SetXferData(mdppEnv->Get(moduleName.Data(), "XferData", 0));
	this->SetMarkingType(mdppEnv->Get(moduleName.Data(), "MarkingType", TMrbMesytec_Mdpp16::kMarkingTypeEvent));
	this->SetTdcResolution(mdppEnv->Get(moduleName.Data(), "TdcResolution", TMrbMesytec_Mdpp16::kTdcRes781_32));
	this->SetOutputFormat(mdppEnv->Get(moduleName.Data(), "OutputFormat", TMrbMesytec_Mdpp16::kOutFmtStandard));
	this->SetWinStart(mdppEnv->Get(moduleName.Data(), "WinStart", TMrbMesytec_Mdpp16::kWinStartDefault));
	this->SetWinWidth(mdppEnv->Get(moduleName.Data(), "WinWidth", TMrbMesytec_Mdpp16::kWinWidthDefault));
	
	Int_t src = mdppEnv->Get(moduleName.Data(), "TrigSrcTrig", 0);
	if (src != 0) {
		this->SetTrigSource(src & 0x3, 0, 0);
	} else {
		src = mdppEnv->Get(moduleName.Data(), "TrigSrcChan", 0);
		if (src != 0) {
			this->SetTrigSource(0, kChanAct | (src & 0x1F), 0);
		} else {
			src = mdppEnv->Get(moduleName.Data(), "TrigSrcBank", 0);
			if (src != 0) this->SetTrigSource(0, 0, src & 0x3);
		}
	}

	src = mdppEnv->Get(moduleName.Data(), "TrigOutChan", 0);
	if (src != 0) {
		this->SetTrigOutput(kChanAct | (src & 0x1F), 0);
	} else {
		src = mdppEnv->Get(moduleName.Data(), "TrigOutBank", 0);
		if (src != 0) this->SetTrigOutput(0, src & 0x3);
	}

	this->SetFirstHit(mdppEnv->Get(moduleName.Data(), "FirstHit", TMrbMesytec_Mdpp16::kFirstHitOnly));
	
	this->SetEcl3(mdppEnv->Get(moduleName.Data(), "Ecl3", TMrbMesytec_Mdpp16::kEcl3Off));
	this->SetEcl2(mdppEnv->Get(moduleName.Data(), "Ecl2", TMrbMesytec_Mdpp16::kEcl2Off));
	this->SetEcl1(mdppEnv->Get(moduleName.Data(), "Ecl1", TMrbMesytec_Mdpp16::kEcl1Off));
	this->SetEcl0(mdppEnv->Get(moduleName.Data(), "Ecl0", TMrbMesytec_Mdpp16::kEcl0Off));
	
	this->SetNim4(mdppEnv->Get(moduleName.Data(), "Nim4", TMrbMesytec_Mdpp16::kNim4Trig0));
	this->SetNim3(mdppEnv->Get(moduleName.Data(), "Nim3", TMrbMesytec_Mdpp16::kNim3Off));
	this->SetNim2(mdppEnv->Get(moduleName.Data(), "Nim2", TMrbMesytec_Mdpp16::kNim2Trig1));
	this->SetNim0(mdppEnv->Get(moduleName.Data(), "Nim0", TMrbMesytec_Mdpp16::kNim0Off));
	
	if (mdppEnv->Get(moduleName.Data(), "PulserStatus", kFALSE)) this->TurnPulserOn(); else this->TurnPulserOff();
	this->SetPulserAmplitude(mdppEnv->Get(moduleName.Data(), "PulserAmplitude", TMrbMesytec_Mdpp16::kPulserMaxAmpl));
	
	if (mdppEnv->Get(moduleName.Data(), "MonitorOn", kFALSE)) this->TurnMonitorOn(); else this->TurnMonitorOff();
	this->SetMonitorChannel(mdppEnv->Get(moduleName.Data(), "MonitorChan", 0));
	this->SetMonitorWave(mdppEnv->Get(moduleName.Data(), "MonitorWave", 0));
	
	this->SetTsSource(mdppEnv->Get(moduleName.Data(), "TsSource", kTstampVME));
	this->SetTsDivisor(mdppEnv->Get(moduleName.Data(), "TsDivisor", 1));
	this->SetMultHighLimit(mdppEnv->Get(moduleName.Data(), "MultHighLimit", kMultHighLimit));
	this->SetMultLowLimit(mdppEnv->Get(moduleName.Data(), "MultLowLimit", kMultLowLimit));

	Int_t tfIntDiff = mdppEnv->Get(moduleName.Data(), "TFIntDiff", "All", kValueNotSet);
	if (tfIntDiff >= 0) {
		fAllTFIntDiff = tfIntDiff;
		for (Int_t ch = 0; ch < kNofChannelPairs; ch++) this->SetTFIntDiff(kValueNotSet, ch);
	} else {
		fAllTFIntDiff = kValueNotSet;
		Int_t ch2 = 0;
		for (Int_t ch = 0; ch < kNofChannelPairs; ch++, ch2 += 2) this->SetTFIntDiff(mdppEnv->Get(moduleName.Data(), "TFIntDiff", Form("%d", ch2), kTFIntDiffDefault), ch);
	}
	
	Int_t poleZero = mdppEnv->Get(moduleName.Data(), "PoleZero", "All", kValueNotSet);
	if (poleZero >= 0) {
		fAllPoleZero = poleZero;
		for (Int_t ch = 0; ch < kNofChannels; ch++) this->SetPoleZero(kValueNotSet, ch);
	} else {
		fAllPoleZero = kValueNotSet;
		for (Int_t ch = 0; ch < kNofChannels; ch++) this->SetPoleZero(mdppEnv->Get(moduleName.Data(), "PoleZero", Form("%d", ch), kPoleZeroDefault), ch);
	}
	
	Int_t gain = mdppEnv->Get(moduleName.Data(), "Gain", "All", kValueNotSet);
	if (gain >= 0) {
		fAllGain = gain;
		for (Int_t ch = 0; ch < kNofChannelPairs; ch++) this->SetGain(kValueNotSet, ch);
	} else {
		fAllGain = kValueNotSet;
		Int_t ch2 = 0;
		for (Int_t ch = 0; ch < kNofChannelPairs; ch++, ch2 += 2) this->SetGain(mdppEnv->Get(moduleName.Data(), "Gain", Form("%d", ch2), kGainDefault), ch);
	}
	
	Int_t thresh = mdppEnv->Get(moduleName.Data(), "Thresh", "All", kValueNotSet);
	if (thresh >= 0) {
		fAllThresh = thresh;
		for (Int_t ch = 0; ch < kNofChannels; ch++) this->SetThresh(kValueNotSet, ch);
	} else {
		fAllThresh = kValueNotSet;
		for (Int_t ch = 0; ch < kNofChannels; ch++) this->SetThresh(mdppEnv->Get(moduleName.Data(), "Thresh", Form("%d", ch), kThreshDefault), ch);
	}
	
	Int_t shape = mdppEnv->Get(moduleName.Data(), "Shaping", "All", kValueNotSet);
	if (shape >= 0) {
		fAllShaping = shape;
		for (Int_t ch = 0; ch < kNofChannelPairs; ch++) this->SetShaping(kValueNotSet, ch);
	} else {
		fAllShaping = kValueNotSet;
		Int_t ch2 = 0;
		for (Int_t ch = 0; ch < kNofChannelPairs; ch++, ch2 += 2) this->SetShaping(mdppEnv->Get(moduleName.Data(), "Shaping", Form("%d", ch2), kShapingDefault), ch);
	}
	
	Int_t blres = mdppEnv->Get(moduleName.Data(), "BLRes", "All", kValueNotSet);
	if (blres >= 0) {
		fAllBaseLineRes = blres;
		for (Int_t ch = 0; ch < kNofChannelPairs; ch++) this->SetBaseLineRes(kValueNotSet, ch);
	} else {
		fAllBaseLineRes = kValueNotSet;
		Int_t ch2 = 0;
		for (Int_t ch = 0; ch < kNofChannelPairs; ch++, ch2 += 2) this->SetBaseLineRes(mdppEnv->Get(moduleName.Data(), "Shaping", Form("%d", ch2), kBLResDefault), ch);
	}
	
	Int_t resetTime = mdppEnv->Get(moduleName.Data(), "Reset", "All", kValueNotSet);
	if (resetTime >= 0) {
		fAllResetTime = resetTime;
		for (Int_t ch = 0; ch < kNofChannelPairs; ch++) this->SetResetTime(kValueNotSet, ch);
	} else {
		fAllResetTime = kValueNotSet;
		Int_t ch2 = 0;
		for (Int_t ch = 0; ch < kNofChannelPairs; ch++, ch2 += 2) this->SetResetTime(mdppEnv->Get(moduleName.Data(), "Reset", Form("%d", ch2), kResetTimeDefault), ch);	
	}
	
	Int_t riseTime = mdppEnv->Get(moduleName.Data(), "Rise", "All", kValueNotSet);
	if (riseTime >= 0) {
		fAllRiseTime = riseTime;
		for (Int_t ch = 0; ch < kNofChannelPairs; ch++) this->SetRiseTime(kValueNotSet, ch);
	} else {
		fAllRiseTime = kValueNotSet;
		Int_t ch2 = 0;
		for (Int_t ch = 0; ch < 8; ch++, ch2 += 2) this->SetResetTime(mdppEnv->Get(moduleName.Data(), "Rise", Form("%d", ch2), kRiseTimeDefault), ch);	
	}
	
	fSettings = mdppEnv->Env();
	return(mdppEnv->Env());
}

Bool_t TMrbMesytec_Mdpp16::UpdateSettings() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mdpp16::UpdateSettings
// Purpose:        Update settings file if needed
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Check version of settings file and perform update if needed
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString settingsVersion;
	TMrbResource * mdppEnv = new TMrbResource("MDPP16", fSettingsFile.Data());
	mdppEnv->Get(settingsVersion, ".SettingsVersion", "");
	if (settingsVersion.CompareTo("4.2016") != 0) {
		gMrbLog->Out() << "Settings file \"" << fSettingsFile << "\" has wrong (old?) version \"" << settingsVersion << "\" (should be 4.2016)" << endl;
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

Bool_t TMrbMesytec_Mdpp16::SaveSettings(const Char_t * SettingsFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mdpp16::SaveSettings
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
	TString tf = "Module_Mdpp16.rc.code";
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
						
						tmpl.Substitute("$mcstSignature", (Int_t) fMCSTSignature);
						tmpl.Substitute("$cbltSignature", (Int_t) fCBLTSignature);
						tmpl.Substitute("$firstInChain", fFirstInChain ? "TRUE" : "FALSE");
						tmpl.Substitute("$lastInChain", fLastInChain ? "TRUE" : "FALSE");
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%FifoHandling%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.Substitute("$dataWidth", this->GetDataWidth());
						tmpl.Substitute("$multiEvent", this->GetMultiEvent());
						tmpl.Substitute("$markingType", this->GetMarkingType());
						tmpl.Substitute("$xferData", this->GetXferData());
						tmpl.Substitute("$blockXfer", this->BlockXferEnabled() ? "TRUE" : "FALSE");
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%OperationMode%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.Substitute("$adcResolution", this->GetAdcResolution());
						tmpl.Substitute("$tdcResolution", this->GetTdcResolution());
						tmpl.Substitute("$outputFormat", this->GetOutputFormat());
						tmpl.Substitute("$repairRawData", this->RawDataToBeRepaired() ? "TRUE" : "FALSE");
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%Trigger%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.Substitute("$winStart", this->GetWinStart());
						tmpl.Substitute("$winWidth", this->GetWinWidth());
						TArrayI src(3);
						this->GetTrigSource(src);
						TString s = "$trigSrcTrig";
						tmpl.Substitute(s, src[0]);
						s = "$trigSrcChan";
						tmpl.Substitute(s, src[1]);
						s = "$trigSrcBank";
						tmpl.Substitute(s, src[2]);
						tmpl.Substitute("$firstHit", this->GetFirstHit());
						TArrayI out(2);
						this->GetTrigOutput(out);
						s = "$trigOutChan";
						tmpl.Substitute(s, out[0]);
						s = "$trigOutBank";
						tmpl.Substitute(s, out[1]);
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%InputOutput%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.Substitute("$ecl3", this->GetEcl3());
						tmpl.Substitute("$ecl2", this->GetEcl2());
						tmpl.Substitute("$ecl1", this->GetEcl1());
						tmpl.Substitute("$ecl0", this->GetEcl0());
						tmpl.Substitute("$nim4", this->GetNim4());
						tmpl.Substitute("$nim3", this->GetNim3());
						tmpl.Substitute("$nim2", this->GetNim2());
						tmpl.Substitute("$nim0", this->GetNim0());
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%TestPulser%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.Substitute("$pulserStatus", this->PulserOn() ? TMrbMesytec_Mdpp16::kPulserOn : TMrbMesytec_Mdpp16::kPulserOff);
						tmpl.Substitute("$pulserAmplitude", this->GetPulserAmplitude());
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%Monitor%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.Substitute("$monitorOn", this->MonitorOn() ? TMrbMesytec_Mdpp16::kMonitorOn : TMrbMesytec_Mdpp16::kMonitorOff);
						tmpl.Substitute("$monitorChan", this->GetMonitorChannel());
						tmpl.Substitute("$monitorWave", this->GetMonitorWave());
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%Counters%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.Substitute("$tsSource", this->GetTsSource());
						tmpl.Substitute("$tsDivisor", this->GetTsDivisor());
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%Multiplicity%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.Substitute("$multHighLimit", this->GetMultHighLimit());
						tmpl.Substitute("$multLowLimit", this->GetMultLowLimit());
						tmpl.WriteCode(settings);
						
						tmpl.InitializeCode("%ChannelSettings%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						Int_t allChan = (fAllTFIntDiff == kValueToBeInit) ? this->GetTFIntDiff(0) : fAllTFIntDiff;
						tmpl.Substitute("$tfIntDiffAll", allChan);
						Int_t ch2 = 0;
						for (Int_t ch = 0; ch < kNofChannelPairs; ch++, ch2 += 2) {
							s = Form("$tfIntDiff%d", ch2);
							tmpl.Substitute(s, (allChan == kValueNotSet) ? this->GetTFIntDiff(ch) : kValueNotSet);				
						}
						allChan = (fAllPoleZero == kValueToBeInit) ? this->GetPoleZero(0) : fAllPoleZero;
						tmpl.Substitute("$pzAll", allChan);
						for (Int_t ch = kNofChannels - 1; ch >= 0; ch--) {
							s = Form("$pz%d", ch);
							tmpl.Substitute(s, (allChan == kValueNotSet) ? this->GetPoleZero(ch) : kValueNotSet);
						}
						allChan = (fAllGain == kValueToBeInit) ? this->GetGain(0) : fAllGain;
						tmpl.Substitute("$gainAll", allChan);
						ch2 = 0;
						for (Int_t ch = 0; ch < kNofChannelPairs; ch++, ch2 += 2) {
							s = Form("$gain%d", ch2);
							tmpl.Substitute(s, (allChan == kValueNotSet) ? this->GetGain(ch) : kValueNotSet);				
						}
						allChan = (fAllThresh == kValueToBeInit) ? this->GetThresh(0) : fAllThresh;
						tmpl.Substitute("$thrAll", allChan);
						for (Int_t ch = kNofChannels - 1; ch >= 0; ch--) {
							s = Form("$thr%d", ch);
							tmpl.Substitute(s, (allChan == kValueNotSet) ? this->GetThresh(ch) : kValueNotSet);
						}
						allChan = (fAllShaping == kValueToBeInit) ? this->GetShaping(0) : fAllShaping;
						tmpl.Substitute("$shapeAll", allChan);
						ch2 = 0;
						for (Int_t ch = 0; ch < kNofChannelPairs; ch++, ch2 += 2) {
							s = Form("$shape%d", ch2);
							tmpl.Substitute(s, (allChan == kValueNotSet) ? this->GetShaping(ch) : kValueNotSet);				
						}
						allChan = (fAllBaseLineRes == kValueToBeInit) ? this->GetBaseLineRes(0) : fAllBaseLineRes;
						tmpl.Substitute("$blrAll", allChan);
						ch2 = 0;
						for (Int_t ch = 0; ch < kNofChannelPairs; ch++, ch2 += 2) {
							s = Form("$blr%d", ch2);
							tmpl.Substitute(s, (allChan == kValueNotSet) ? this->GetBaseLineRes(ch) : kValueNotSet);				
						}
						allChan = (fAllResetTime == kValueToBeInit) ? this->GetResetTime(0) : fAllResetTime;
						tmpl.Substitute("$resAll", allChan);
						ch2 = 0;
						for (Int_t ch = 0; ch < kNofChannelPairs; ch++, ch2 += 2) {
							s = Form("$res%d", ch2);
							tmpl.Substitute(s, (allChan == kValueNotSet) ? this->GetResetTime(ch) : kValueNotSet);				
						}
						allChan = (fAllRiseTime == kValueToBeInit) ? this->GetRiseTime(0) : fAllRiseTime;
						tmpl.Substitute("$riseAll", allChan);
						ch2 = 0;
						for (Int_t ch = 0; ch < kNofChannelPairs; ch++, ch2 += 2) {
							s = Form("$rise%d", ch2);
							tmpl.Substitute(s, (allChan == kValueNotSet) ? this->GetRiseTime(ch) : kValueNotSet);				
						}
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

Bool_t TMrbMesytec_Mdpp16::MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mdpp16::MakeReadoutCode
// Purpose:        Write a piece of code for a Mesytec MDPP-16
// Arguments:      ofstream & RdoStrm         -- file output stream
//                 EMrbModuleTag TagIndex     -- index of tag word taken from template file
// Results:        kTRGetTUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a Mesytec MDPP-16.
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
				if (this->McstEnabled()) fCodeTemplates.InitializeCode("%M%"); else fCodeTemplates.InitializeCode("%N%");
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
					lp = gEnv->GetValue(Form("TMrbConfig.PPCLibraryPath.%d", bNo), Form("$MARABOU/powerpc/lib/%s", lv.Data()));
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


Bool_t TMrbMesytec_Mdpp16::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbModuleTag TagIndex,
															TMrbVMEChannel * Channel,
															Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mdpp16::MakeReadoutCode
// Purpose:        Write a piece of code for a Mesytec MDPP-16
// Arguments:      ofstream & RdoStrm           -- file output stream
//                 EMrbModuleTag TagIndex       -- index of tag word taken from template file
//                 TMrbVMEChannel * Channel     -- channel
//                 Int_t Value                  -- value to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a Mesytec MDPP-16.
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

Bool_t TMrbMesytec_Mdpp16::CheckSubeventType(TMrbSubevent * Subevent) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mdpp16::CheckSubeventType
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

void TMrbMesytec_Mdpp16::PrintSettings(ostream & Out) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mdpp16::PrintSettings
// Purpose:        Print settings
// Arguments:      ostream & Out           -- where to print
// Results:        --
// Exceptions:
// Description:    Prints settings to stream.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString value;

	Out << "=========================================================================" << endl;
	Out << " Mesytec MDPP16 settings" << endl;
	Out << "-------------------------------------------------------------------------" << endl;
	Out << " Module                 : "	<< this->GetName() << endl;
	Out << " Serial                 : "	<< this->GetSerial() << endl;
	Out << " Address source         : "	<< this->FormatValue(value, TMrbMesytec_Mdpp16::kRegAddrSource) << endl;
	Out << "         register       : "	<< this->FormatValue(value, TMrbMesytec_Mdpp16::kRegAddrReg, -1, 16) << endl;
	Out << " Module ID              : "	<< this->FormatValue(value, TMrbMesytec_Mdpp16::kRegModuleId) << endl;
	Out << " Data width             : "	<< this->FormatValue(value, TMrbMesytec_Mdpp16::kRegDataWidth) << endl;
	Out << " Single/multi event     : "	<< this->FormatValue(value, TMrbMesytec_Mdpp16::kRegMultiEvent) << endl;
	Out << " Marking type           : "	<< this->FormatValue(value, TMrbMesytec_Mdpp16::kRegMarkingType) << endl;
	Out << " Block tansfer          : "	<< (this->BlockXferEnabled() ? "on" : "off") << endl;
	Out << " Repair raw data        : "	<< (this->RawDataToBeRepaired() ? "on" : "off") << endl;
	Out << " ADC resolution         : "	<< this->FormatValue(value, TMrbMesytec_Mdpp16::kRegAdcResolution) << endl;
	Out << " TDC resolution         : "	<< this->FormatValue(value, TMrbMesytec_Mdpp16::kRegTdcResolution) << endl;
	Out << " Output format          : "	<< this->FormatValue(value, TMrbMesytec_Mdpp16::kRegOutputFormat) << endl;
	Out << " Window start           : " << this->GetWinStart() << " ns" << endl;
	Out << " Window width           : " << this->GetWinWidth() << " ns" << endl;
	TArrayI src(3);
	this->GetTrigSource(src);
	Out << " Trigger source         : trig=" << src[0] << ", chan=" << src[1] << ", bank=" << src[2] << endl;
	TArrayI out(2);
	this->GetTrigOutput(out);
	Out << " Trigger output         : chan=" << out[0] << ", bank=" << out[1] << endl;
	Int_t fh = this->GetFirstHit();
	Out << " First hit              : "	<< fh << ((fh & 1) ? ", 1st hit only" : ", all hits") << endl;
	Out << " ECL    3               : "	<< this->FormatValue(value, TMrbMesytec_Mdpp16::kRegEcl3) << endl;
	Out << "        2               : "	<< this->FormatValue(value, TMrbMesytec_Mdpp16::kRegEcl2) << endl;
	Out << "        1               : "	<< this->FormatValue(value, TMrbMesytec_Mdpp16::kRegEcl1) << endl;
	Out << "        0               : "	<< this->FormatValue(value, TMrbMesytec_Mdpp16::kRegEcl0) << endl;
	Out << " NIM    4               : "	<< this->FormatValue(value, TMrbMesytec_Mdpp16::kRegNim4) << endl;
	Out << "        3               : "	<< this->FormatValue(value, TMrbMesytec_Mdpp16::kRegNim3) << endl;
	Out << "        2               : "	<< this->FormatValue(value, TMrbMesytec_Mdpp16::kRegNim2) << endl;
	Out << "        1               : trigger out" << endl;
	Out << "        0               : "	<< this->FormatValue(value, TMrbMesytec_Mdpp16::kRegNim0) << endl;
	Out << " Pulser status          : "	<< (this->PulserOn() ? "on" : "off") << endl;
	Out << "        amplitude       : "	<< this->FormatValue(value, TMrbMesytec_Mdpp16::kRegPulserAmplitude) << endl;
	Out << " Monitor                : "	<< (this->MonitorOn() ? "on" : "off") << endl;
	Out << "        channel         : "	<< this->FormatValue(value, TMrbMesytec_Mdpp16::kRegMonitorChannel) << endl;
	Out << "        wave            : "	<< this->FormatValue(value, TMrbMesytec_Mdpp16::kRegMonitorWave) << endl;
	Out << " Timestamp source       : "	<< this->FormatValue(value, TMrbMesytec_Mdpp16::kRegTsSource) << endl;
	Out << "          divisor       : "	<< this->FormatValue(value, TMrbMesytec_Mdpp16::kRegTsDivisor) << endl;
	Out << " Multiplicity limit high: " << this->GetMultHighLimit() << endl;
	Out << "                     low: " << this->GetMultLowLimit() << endl;
	Int_t allChan = fAllTFIntDiff;
	if (allChan == kValueNotSet) {
		Int_t ch2 = 0;
		TString s = " TF integ/diff       ";
		for (Int_t ch = 0; ch < kNofChannelPairs; ch++, ch2 += 2) {
			Out << s << ch2 << "/" << (ch2 + 1) << ": " << this->GetTFIntDiff(ch) << endl;
			s = "                     ";
		}
	} else {
		Out << " TF integ/diff       all: " << allChan << endl;
	}	
	allChan = fAllPoleZero;
	if (allChan == kValueNotSet) {
		TString s = " Pole/Zero             ";
		for (Int_t ch = 0; ch < kNofChannels; ch++) {
			Out << s << ch << ": " << this->GetPoleZero(ch) << endl;
			s = "                       ";
		}
	} else {
		Out << " Pole/Zero           all: " << allChan << endl;
	}	
	allChan = fAllGain;
	if (allChan == kValueNotSet) {
		Int_t ch2 = 0;
		TString s = " Gain                ";
		for (Int_t ch = 0; ch < kNofChannelPairs; ch++, ch2 += 2) {
			Out << s << ch2 << "/" << (ch2 + 1) << ": " << this->GetGain(ch) << endl;
			s = "                     ";
		}
	} else {
		Out << " Gain                all: " << allChan << endl;
	}	
	allChan = fAllThresh;
	if (allChan == kValueNotSet) {
		TString s = " Threshold             ";
		for (Int_t ch = 0; ch < kNofChannels; ch++) {
			Out << s << ch << ": " << this->GetThresh(ch) << endl;
			s = "                       ";
		}
	} else {
		Out << " Threshold           all: " << allChan << endl;
	}	
	allChan = fAllShaping;
	if (allChan == kValueNotSet) {
		Int_t ch2 = 0;
		TString s = " Shaping time        ";
		for (Int_t ch = 0; ch < kNofChannelPairs; ch++, ch2 += 2) {
			Out << s << ch2 << "/" << (ch2 + 1) << ": " << this->GetShaping(ch) << endl;
			s = "                     ";
		}
	} else {
		Out << " Shaping time        all: " << allChan << endl;
	}	
	allChan = fAllBaseLineRes;
	if (allChan == kValueNotSet) {
		Int_t ch2 = 0;
		TString s = " Baseline restore    ";
		for (Int_t ch = 0; ch < kNofChannelPairs; ch++, ch2 += 2) {
			Out << s << ch2 << "/" << (ch2 + 1) << ": " << this->GetBaseLineRes(ch) << endl;
			s = "                     ";
		}
	} else {
		Out << " Baseline restore    all: " << allChan << endl;
	}	
	allChan = fAllResetTime;
	if (allChan == kValueNotSet) {
		Int_t ch2 = 0;
		TString s = " Reset time          ";
		for (Int_t ch = 0; ch < kNofChannelPairs; ch++, ch2 += 2) {
			Out << s << ch2 << "/" << (ch2 + 1) << ": " << this->GetResetTime(ch) << endl;
			s = "                     ";
		}
	} else {
		Out << " Reset time          all: " << allChan << endl;
	}	
	allChan = fAllRiseTime;
	if (allChan == kValueNotSet) {
		Int_t ch2 = 0;
		TString s = " Rise time           ";
		for (Int_t ch = 0; ch < kNofChannelPairs; ch++, ch2 += 2) {
			Out << s << ch2 << "/" << (ch2 + 1) << ": " << this->GetRiseTime(ch) << endl;
			s = "                     ";
		}
	} else {
		Out << " Rise time           all: " << allChan << endl;
	}	
	Out << "-------------------------------------------------------------------------" << endl << endl;
}

const Char_t * TMrbMesytec_Mdpp16::FormatValue(TString & Value, Int_t Index, Int_t SubIndex, Int_t Base) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mdpp16::FormatValue
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
	
Bool_t TMrbMesytec_Mdpp16::SetTrigSource(Int_t Trig, Int_t Chan, Int_t Bank) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mdpp16::SetTrigSource
// Purpose:        Set trigger source
// Arguments:      Int_t Trig       -- trigger, 0 (default), bit0 = trig0, bit1 = trig1
//                 Int_t Chan       -- channel, 0 (default), kChanAct+0 ... kChanAct+31
//                 Int_t Bank       -- bank, 0 (default), bit0 = bank0, bit1 = bank1
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes trigger source.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t src = Trig & 0x3;
	if (src > 0) {
		this->SetTrigSource(src);
	} else {
		src = Chan;
		if (Chan & kChanAct) {
			this->SetTrigSource((src << 2));
		} else {
			src = Bank & 0x3;
			if (src > 0) this->SetTrigSource((src << 8));
			else return kFALSE;
		}
	}
	return kTRUE;
}

Bool_t TMrbMesytec_Mdpp16::GetTrigSource(TArrayI & TrigSource) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mdpp16::GetTrigSource
// Purpose:        Get trigger source
// Arguments:
// Results:        kTRUE/kFALSE
//                 TArrayI & TrigSource -- [0] -> trigger
//                                         [1] -> channel
//                                         [2] -> bank
// Exceptions:
// Description:    Reads trigger source.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t src = this->GetTrigSource();
	TrigSource[0] = src & 0x3;
	TrigSource[1] = ((src >> 2) & 0x1F) | kChanAct;
	TrigSource[2] = (src >> 8) & 0x3;
	return kTRUE;
}

Bool_t TMrbMesytec_Mdpp16::SetTrigOutput(Int_t Chan, Int_t Bank) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mdpp16::SetTrigOutput
// Purpose:        Set trigger output
// Arguments:      Int_t Chan       -- channel, 0 (default), kChanAct+0 ... kChanAct+31
//                 Int_t Bank       -- bank, 0 (default), bit0 = bank0, bit1 = bank1
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes trigger output.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t src = Chan;
	if (Chan & kChanAct) {
		this->SetTrigOutput((src << 2));
	} else {
		src = Bank & 0x3;
		if (src > 0) this->SetTrigOutput((src << 8));
		else return kFALSE;
	}
	return kTRUE;
}

Bool_t TMrbMesytec_Mdpp16::GetTrigOutput(TArrayI & TrigOutput) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Mdpp16::GetTrigOutput
// Purpose:        Get trigger output
// Arguments:
// Results:        kTRUE/kFALSE
//                 TArrayI & TrigOutput -- [0] -> channel
//                                         [1] -> bank
// Exceptions:
// Description:    Reads trigger output.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t src = this->GetTrigOutput();
	TrigOutput[0] = ((src >> 2) & 0x1F) | kChanAct;
	TrigOutput[1] = (src >> 8) & 0x3;
	return kTRUE;
}


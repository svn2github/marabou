//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbSis_3300.cxx
// Purpose:        MARaBOU configuration: SIS modules
// Description:    Implements class methods to handle a SIS digitizing adc type 3300
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbSis_3300.cxx,v 1.18 2012-01-18 11:11:32 Marabou Exp $
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
#include "TMrbSis_3300.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbSis_3300)

const SMrbNamedX kMrbAcquisitionMode[] =
		{
			{TMrbSis_3300::kAcqSingleEvent,			"SingleEvent",		"Single event mode"			},
			{TMrbSis_3300::kAcqMultiEvent,			"MultiEvent",		"Multi event mode"			},
			{TMrbSis_3300::kAcqGateChaining,		"GateChaining",		"Gate chaining mode"		},
			{0, 									NULL,				NULL						}
		};

const SMrbNamedX kMrbPageSize[] =
		{
			{TMrbSis_3300::kPageSize128k,			"128k", 			"131072 samples (128k)" 	},
			{TMrbSis_3300::kPageSize16k,			"16k"				"16384 samples (16k)"		},
			{TMrbSis_3300::kPageSize4k, 			"4k",				"4096 samples (4k)" 		},
			{TMrbSis_3300::kPageSize2k, 			"2k",				"2048 samples (2k)" 		},
			{TMrbSis_3300::kPageSize1k, 			"1k",				"1024 samples (1k)" 		},
			{TMrbSis_3300::kPageSize512s,			"512s",				"512 samples"				},
			{TMrbSis_3300::kPageSize256s,			"256s",				"256 samples"				},
			{TMrbSis_3300::kPageSize128s, 			"128s",				"128 samples"				},
			{0, 									NULL,				NULL						}
		};

const SMrbNamedX kMrbClockSource[] =
		{
			{TMrbSis_3300::kClockSource100, 		"100MHz"			"100 MHz clock"				},
			{TMrbSis_3300::kClockSource50, 			"50MHz"				"50 MHz clock"				},
			{TMrbSis_3300::kClockSource25, 			"25MHz"				"25 MHz clock"				},
			{TMrbSis_3300::kClockSource12_5, 		"12.5MHz"			"12.5 MHz clock"			},
			{TMrbSis_3300::kClockSource6_25, 		"6.25MHz"			"6.25 MHz clock"			},
			{TMrbSis_3300::kClockSource3_125, 		"3.125MHz"			"3.125 MHz clock"			},
			{TMrbSis_3300::kClockSourceExtern,		"external"			"External front panel clock"},
			{TMrbSis_3300::kClockSourceP2,			"P2"				"P2 mode clock" 			},
			{0, 									NULL,				NULL						}
		};

const SMrbNamedX kMrbBankFull[] =
		{
			{TMrbSis_3300::kBankFullDisabled,		"disabled", 		"No Signal on bank full"	},
			{TMrbSis_3300::kBankFullToUser, 		"user", 			"Send to user on bank full" },
			{TMrbSis_3300::kBankFullToStop, 		"stop"				"Send to stop on bank full" },
			{TMrbSis_3300::kBankFullToStart,		"start" 			"Send to start on ank full" },
			{0, 									NULL,				NULL						}
		};

const SMrbNamedX kMrbTrigOn[] =
		{
			{TMrbSis_3300::kTriggerOnDisabled,		"disabled"			"Do not generate trigger"	},
			{TMrbSis_3300::kTriggerOnEnabled,		"enabled"			"Generate trigger"			},
			{TMrbSis_3300::kTriggerOnEvenFIR,		"evenFIR", 			"Even FIR mode" 			},
			{TMrbSis_3300::kTriggerOnOddFIR,		"oddFIR",			"Odd FIR mode"				},
			{0, 									NULL,				NULL						}
		};

const SMrbNamedX kMrbTrigSlope[] =
		{
			{TMrbSis_3300::kTriggerSlopeDisabled,	"disabled",			"No slope check"			},
			{TMrbSis_3300::kTriggerSlopeNegative,	"negative", 		"Negative slope"			},
			{TMrbSis_3300::kTriggerSlopePositive,	"positive", 		"Positive slope"			},
			{TMrbSis_3300::kTriggerSlopeBipolar,	"bipolar",			"Biploar slope" 			},
			{0, 									NULL,				NULL						}
		};

TMrbSis_3300::TMrbSis_3300(const Char_t * ModuleName, UInt_t BaseAddr) :
									TMrbVMEModule(ModuleName, "Sis_3300", BaseAddr,
																TMrbSis_3300::kAddrMod,
																TMrbSis_3300::kSegSize,
																1, 8, 1 << 17) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSis_3300
// Purpose:        Create a digitizing adc of type SIS 3300
// Arguments:      Char_t * ModuleName      -- name of camac module
//                 UInt_t BaseAddr          -- base addr
// Results:        --
// Exceptions:
// Description:    Creates a vme module of type SIS 3300.
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
			SetTitle("SIS 3300 digitizing adc 8 chn 12 bit 100 MHz"); 	// store module type
			codeFile = fModuleID.GetName();
			codeFile += ".code";
			if (LoadCodeTemplates(codeFile)) {
				DefineRegisters();
				mTypeBits = TMrbConfig::kModuleVME |
							TMrbConfig::kModuleListMode |
							TMrbConfig::kModuleMultiEvent |
							TMrbConfig::kModuleAdc;
				gMrbConfig->GetLofModuleTypes()->Pattern2String(mType, mTypeBits);
				fModuleType.Set(mTypeBits, mType.Data());
				fDataType = gMrbConfig->GetLofDataTypes()->FindByIndex(TMrbConfig::kDataUInt);
				fNofShortsPerChannel = 2;		// 32 bits
				fNofShortsPerDatum = 1;
				fNofDataBits = 12;
				fBlockReadout = kTRUE;			// module has block readout

				fBlockXfer = kFALSE;
				fMaxEvents = kFALSE;
				fTriggerOut = kFALSE;
				fInvertTrigger = kFALSE;
				fRouteTrigger = kFALSE;
				fStartStopEnable = kFALSE;
				fAutoStartEnable = kFALSE;
				fAutoBankSwitch = kFALSE;
				fWrapAroundEnable = kFALSE;
				fTriggerArmed = kFALSE;

				fSettingsFile = Form("%sSettings.rc", this->GetName());

				fXmin = 0;									// X range: 4K
				fXmax = (Int_t) (1 << 12);

				fSampleRange = (Int_t) (1 << 12);			// sampling: 4K
				fSmin = 0;
				fSmax = fSampleRange;
				this->SetSampleBinning(4);

				fTriggerRange = (Int_t) (1 << 16);			// trigger: 64K
				fTriggerBaseLine = (Int_t) (1 << 15);
				fTmin = fTriggerBaseLine - 4096;
				fTmax = fTriggerBaseLine + 4096;
				this->SetTriggerBinning(8);

				fShaperOn = kFALSE;
				fShaperRange = (Int_t) (1 << 11);
				fShmin[kShapeShort] = -25;
				fShmax[kShapeShort] = 25;
				fShmin[kShapeLong] = -256;
				fShmax[kShapeLong] = 256;
				this->SetShaperBinRange(1024);

				gMrbConfig->AddModule(this);				// append to list of modules
				gDirectory->Append(this);
			} else {
				this->MakeZombie();
			}
		}
	}
}

void TMrbSis_3300::DefineRegisters() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSis_3300::DefineRegisters
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

	kp = new TMrbNamedX(TMrbSis_3300::kRegAcquisitionMode, "AcquisitionMode");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, TMrbSis_3300::kAcqMultiEvent, 0, TMrbSis_3300::kAcqLast - 1);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("AcquisitionMode");
	bNames->AddNamedX(kMrbAcquisitionMode);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbSis_3300::kRegReadGroup, "ReadGroup");
	rp = new TMrbVMERegister(this, TMrbSis_3300::kNofGroups, kp, 0, 0, 0, (Int_t) kTRUE, (Int_t) kFALSE, (Int_t) kTRUE);
	rp->SetBoolean();
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbSis_3300::kRegPageSize, "PageSize");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, TMrbSis_3300::kPageSize128k, 0, TMrbSis_3300::kPageSizeLast - 1);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("PageSize");
	bNames->AddNamedX(kMrbPageSize);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbSis_3300::kRegClockSource, "ClockSource");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, TMrbSis_3300::kClockSource100, 0, TMrbSis_3300::kClockSourceLast - 1);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("ClockSource");
	bNames->AddNamedX(kMrbClockSource);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbSis_3300::kRegStartDelay, "StartDelay");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, 0, 0, 65535);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbSis_3300::kRegStopDelay, "StopDelay");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, 0, 0, 65535);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbSis_3300::kRegBankFull, "BankFull");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, TMrbSis_3300::kBankFullDisabled, 0, TMrbSis_3300::kBankFullLast - 1);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("BankFull");
	bNames->AddNamedX(kMrbBankFull);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbSis_3300::kRegTriggerOn, "TrigOn");
	rp = new TMrbVMERegister(this, TMrbSis_3300::kNofGroups, kp, 0, 0, 0, TMrbSis_3300::kTriggerOnDisabled, 0, TMrbSis_3300::kTriggerOnLast - 1);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("TrigOn");
	bNames->AddNamedX(kMrbTrigOn);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbSis_3300::kRegTriggerThresh, "TrigThresh");
	rp = new TMrbVMERegister(this, TMrbSis_3300::kNofChannels, kp, 0, 0, 0, 0, 0, 65535);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbSis_3300::kRegTriggerSlope, "TrigSlope");
	rp = new TMrbVMERegister(this, TMrbSis_3300::kNofChannels, kp, 0, 0, 0, TMrbSis_3300::kTriggerSlopeDisabled, 0, TMrbSis_3300::kTriggerSlopeLast - 1);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("TrigSlope");
	bNames->AddNamedX(kMrbTrigSlope);
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbSis_3300::kRegPeakTime, "PeakTime");
	rp = new TMrbVMERegister(this, TMrbSis_3300::kNofGroups, kp, 0, 0, 0, 0, 0, 255);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbSis_3300::kRegGapTime, "GapTime");
	rp = new TMrbVMERegister(this, TMrbSis_3300::kNofGroups, kp, 0, 0, 0, 0, 0, 255);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbSis_3300::kRegPulseMode, "PulseMode");
	rp = new TMrbVMERegister(this, TMrbSis_3300::kNofGroups, kp, 0, 0, 0, (Int_t) kFALSE,  (Int_t) kFALSE, (Int_t) kTRUE);
	rp->SetBoolean();
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbSis_3300::kRegPulseLength, "PulseLength");
	rp = new TMrbVMERegister(this, TMrbSis_3300::kNofGroups, kp, 0, 0, 0, 0, 0, 15);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
}

TEnv * TMrbSis_3300::UseSettings(const Char_t * SettingsFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSis_3300::UseSettings
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

	TEnv * sisEnv = new TEnv(fSettingsFile.Data());

	TString moduleName = sisEnv->GetValue("SIS3300.ModuleName", "unknown");
	if (moduleName.CompareTo(this->GetName()) != 0) {
		gMrbLog->Err() << "Module name different - " << moduleName << " (should be " << this->GetName() << ")" << endl;
		gMrbLog->Flush(this->ClassName(), "UseSettings");
		return(NULL);
	}
	moduleName(0,1).ToUpper();

	this->SetBlockXfer(sisEnv->GetValue(Form("SIS3300.%s.BlockXfer", moduleName.Data()), kFALSE));
	this->SetAcquisitionMode(sisEnv->GetValue(Form("SIS3300.%s.AcquisitionMode", moduleName.Data()), TMrbSis_3300::kAcqMultiEvent));
	this->SetMaxEvents(sisEnv->GetValue(Form("SIS3300.%s.MaxEvents", moduleName.Data()), 0));
	this->SetPageSize(sisEnv->GetValue(Form("SIS3300.%s.PageSize", moduleName.Data()), TMrbSis_3300::kPageSize512s));
	this->SetClockSource(sisEnv->GetValue(Form("SIS3300.%s.ClockSource", moduleName.Data()), TMrbSis_3300::kClockSource100));
	this->SetStartDelay(sisEnv->GetValue(Form("SIS3300.%s.StartDelay", moduleName.Data()), 0));
	this->SetStopDelay(sisEnv->GetValue(Form("SIS3300.%s.StopDelay", moduleName.Data()), 0));
	this->SetTriggerToUserOut(sisEnv->GetValue(Form("SIS3300.%s.TriggerToUserOut", moduleName.Data()), kFALSE));
	this->InvertTrigger(sisEnv->GetValue(Form("SIS3300.%s.InvertTrigger", moduleName.Data()), kFALSE));
	this->RouteTriggerToStop(sisEnv->GetValue(Form("SIS3300.%s.RouteTriggerToStop", moduleName.Data()), kFALSE));
	this->EnableStartStop(sisEnv->GetValue(Form("SIS3300.%s.StartStopEnable", moduleName.Data()), kFALSE));
	this->EnableAutoStart(sisEnv->GetValue(Form("SIS3300.%s.AutoStartEnable", moduleName.Data()), kFALSE));
	this->EnableAutoBankSwitch(sisEnv->GetValue(Form("SIS3300.%s.AutoBankEnable", moduleName.Data()), kFALSE));
	this->SetBankFullToFront(sisEnv->GetValue(Form("SIS3300.%s.BankFullToFront", moduleName.Data()),  TMrbSis_3300::kBankFullDisabled));
	this->EnableWrapAround(sisEnv->GetValue(Form("SIS3300.%s.WrapAround", moduleName.Data()), kFALSE));
	this->EnableTriggerOnBankArmed(sisEnv->GetValue(Form("SIS3300.%s.TriggerOnBankArmed", moduleName.Data()), kFALSE));

	for (Int_t i = 0; i < TMrbSis_3300::kNofGroups; i++) {
		Bool_t grpEnabled = sisEnv->GetValue(Form("SIS3300.%s.Group%d.Readout", moduleName.Data(), i), kFALSE);
		if (grpEnabled) this->SetReadGroup(kTRUE, i);
	}
	for (Int_t i = 0; i < TMrbSis_3300::kNofGroups; i++) {
		this->SetTriggerOn(sisEnv->GetValue(Form("SIS3300.%s.Group%d.TrigOn", moduleName.Data(), i), TMrbSis_3300::kTriggerOnDisabled), i);
	}
	for (Int_t i = 0; i < TMrbSis_3300::kNofChannels; i++) {
		this->SetTriggerThresh(sisEnv->GetValue(Form("SIS3300.%s.Chan%d.TrigThresh", moduleName.Data(), i), 0), i);
	}
	for (Int_t i = 0; i < TMrbSis_3300::kNofChannels; i++) {
		this->SetTriggerSlope(sisEnv->GetValue(Form("SIS3300.%s.Chan%d.TrigSlope", moduleName.Data(), i), TMrbSis_3300::kTriggerSlopeDisabled), i);
	}
	for (Int_t i = 0; i < TMrbSis_3300::kNofGroups; i++) {
		this->SetPeakTime(sisEnv->GetValue(Form("SIS3300.%s.Group%d.PeakTime", moduleName.Data(), i), 0), i);
	}
	for (Int_t i = 0; i < TMrbSis_3300::kNofGroups; i++) {
		this->SetGapTime(sisEnv->GetValue(Form("SIS3300.%s.Group%d.GapTime", moduleName.Data(), i), 0), i);
	}
	for (Int_t i = 0; i < TMrbSis_3300::kNofGroups; i++) {
		this->EnablePulseMode(sisEnv->GetValue(Form("SIS3300.%s.Group%d.PulseMode", moduleName.Data(), i), kFALSE), i);
	}
	for (Int_t i = 0; i < TMrbSis_3300::kNofGroups; i++) {
		this->SetPulseLength(sisEnv->GetValue(Form("SIS3300.%s.Group%d.PulseLength", moduleName.Data(), i), 0), i);
	}

	return(sisEnv);
}

Bool_t TMrbSis_3300::SetSmin(Int_t Smin) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSis_3300::SetSmin
// Purpose:        Set lower limit
// Arguments:      Int_t Smin     -- lower limit
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets lower Y limit (sampling channel)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Smin < 0 || Smin > fSmax) {
		gMrbLog->Err()	<< this->GetName() << ": YMIN (sampling channel) out of range - " << Smin
						<< " (should be in [0, " << fSmax << "]" << endl;
		gMrbLog->Flush(this->ClassName(), "SetSmin");
		return(kFALSE);
	} else {
		fSmin = Smin;
		return(kTRUE);
	}
}

Bool_t TMrbSis_3300::SetSmax(Int_t Smax) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSis_3300::SetSmax
// Purpose:        Set upper limit
// Arguments:      Int_t Smax     -- upper limit
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets upper Y limit (sampling channel)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Smax == 0) Smax = fSampleRange;
	if (Smax < fSmin || Smax > fSampleRange) {
		gMrbLog->Err()	<< this->GetName() << ": YMAX (sampling channel) out of range - " << Smax
						<< " (should be in [" << fSmin << ", " << fSampleRange << "]" << endl;
		gMrbLog->Flush(this->ClassName(), "SetSmax");
		return(kFALSE);
	} else {
		fSmax = Smax;
		return(kTRUE);
	}
}

Bool_t TMrbSis_3300::SetTmin(Int_t Tmin) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSis_3300::SetTmin
// Purpose:        Set lower limit
// Arguments:      Int_t Tmin     -- lower limit
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets lower Y limit (trigger channel)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Tmin < 0 || Tmin > fTmax) {
		gMrbLog->Err()	<< this->GetName() << ": YMIN (trigger channel) out of range - " << Tmin
						<< " (should be in [0, " << fTmax << "]" << endl;
		gMrbLog->Flush(this->ClassName(), "SetTmin");
		return(kFALSE);
	} else {
		fTmin = Tmin;
		return(kTRUE);
	}
}

Bool_t TMrbSis_3300::SetTmax(Int_t Tmax) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSis_3300::SetTmax
// Purpose:        Set upper limit
// Arguments:      Int_t Tmax     -- upper limit
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets upper Y limit (trigger channel)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Tmax == 0) Tmax = fTriggerRange;
	if (Tmax < fTmin || Tmax > fTriggerRange) {
		gMrbLog->Err()	<< this->GetName() << ":YMAX (trigger channel) out of range - " << Tmax
						<< " (should be in [" << fTmin << ", " << fTriggerRange << "]" << endl;
		gMrbLog->Flush(this->ClassName(), "SetTmax");
		return(kFALSE);
	} else {
		fTmax = Tmax;
		return(kTRUE);
	}
}

Bool_t TMrbSis_3300::SetShmin(Int_t Shmin, Int_t ShaperIdx) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSis_3300::SetShmin
// Purpose:        Set lower limit
// Arguments:      Int_t Shmin     -- lower limit
//                 Int_t ShaperIdx -- shape index: short/long
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets lower Y limit (shaper)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Shmin < -kShaperMinMax || Shmin > kShaperMinMax || Shmin > fShmax[ShaperIdx]) {
		gMrbLog->Err()	<< this->GetName() << ": YMIN (shaper) out of range - " << Shmin
						<< " (should be [-" << kShaperMinMax << ", " << kShaperMinMax << "]" << endl;
		gMrbLog->Flush(this->ClassName(), "SetShmin");
		return(kFALSE);
	} else {
		fShmin[ShaperIdx] = Shmin;
		return(kTRUE);
	}
}

Bool_t TMrbSis_3300::SetShmax(Int_t Shmax, Int_t ShaperIdx) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSis_3300::SetShmax
// Purpose:        Set upper limit
// Arguments:      Int_t Shmax     -- upper limit
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets upper Y limit (shaper)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Shmax < -kShaperMinMax || Shmax > kShaperMinMax || Shmax < fShmin[ShaperIdx]) {
		gMrbLog->Err()	<< this->GetName() << ": YMAX (shaper) out of range - " << Shmax
						<< " (should be [-" << kShaperMinMax << ", " << kShaperMinMax << "]" << endl;
		gMrbLog->Flush(this->ClassName(), "SetShmax");
		return(kFALSE);
	} else {
		fShmax[ShaperIdx] = Shmax;
		return(kTRUE);
	}
}

Bool_t TMrbSis_3300::SaveSettings(const Char_t * SettingsFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSis_3300::SaveSettings
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
	TString tf = "Module_Sis_3300.rc.code";
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
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%ReadGroupHdr%");
						tmpl.WriteCode(settings);
						for (Int_t i = 0; i < TMrbSis_3300::kNofGroups; i++) {
							tmpl.InitializeCode("%ReadGroupLoop%");
							tmpl.Substitute("$moduleName", moduleUC.Data());
							tmpl.Substitute("$grp", i);
							tmpl.Substitute("$readout", this->GroupToBeRead(i) ? "TRUE" : "FALSE");
							tmpl.WriteCode(settings);
						}

						tmpl.InitializeCode("%Settings%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.Substitute("$blockXfer", this->BlockXferEnabled() ? "TRUE" : "FALSE");
						tmpl.Substitute("$acquisitionMode", this->GetAcquisitionMode());
						tmpl.Substitute("$maxEvents", this->GetMaxEvents());
						tmpl.Substitute("$pageSize", this->GetPageSize());
						tmpl.Substitute("$clockSource", this->GetClockSource());
						tmpl.Substitute("$startDelay", this->GetStartDelay());
						tmpl.Substitute("$stopDelay", this->GetStopDelay());
						tmpl.Substitute("$triggerToUserOut", this->TriggerToUserOut() ? "TRUE" : "FALSE");
						tmpl.Substitute("$invertTrigger", this->TriggerInverted() ? "TRUE" : "FALSE");
						tmpl.Substitute("$routeTriggerToStop", this->TriggerRoutedToStop() ? "TRUE" : "FALSE");
						tmpl.Substitute("$startStopEnable", this->StartStopEnabled() ? "TRUE" : "FALSE");
						tmpl.Substitute("$autoStartEnable", this->AutoStartEnabled() ? "TRUE" : "FALSE");
						tmpl.Substitute("$autoBankEnable", this->AutoBankSwitchEnabled() ? "TRUE" : "FALSE");
						tmpl.Substitute("$bankFullToFront", this->AutoBankSwitchEnabled() ? "TRUE" : "FALSE");
						tmpl.Substitute("$autoBankEnable", this->GetBankFullToFront());
						tmpl.Substitute("$wrapAround", this->WrapAroundEnabled() ? "TRUE" : "FALSE");
						tmpl.Substitute("$triggerOnBankArmed", this->TriggerOnBankArmedEnabled() ? "TRUE" : "FALSE");
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%TriggerOnHdr%");
						tmpl.WriteCode(settings);
						for (Int_t i = 0; i < TMrbSis_3300::kNofGroups; i++) {
							tmpl.InitializeCode("%TriggerOnLoop%");
							tmpl.Substitute("$moduleName", moduleUC.Data());
							tmpl.Substitute("$grp", i);
							tmpl.Substitute("$trigOn", this->GetTriggerOn(i));
							tmpl.WriteCode(settings);
						}

						tmpl.InitializeCode("%TriggerThreshHdr%");
						tmpl.WriteCode(settings);
						for (Int_t i = 0; i < TMrbSis_3300::kNofChannels; i++) {
							tmpl.InitializeCode("%TriggerThreshLoop%");
							tmpl.Substitute("$moduleName", moduleUC.Data());
							tmpl.Substitute("$chn", i);
							tmpl.Substitute("$trigThresh", this->GetTriggerThresh(i));
							tmpl.WriteCode(settings);
						}

						tmpl.InitializeCode("%TriggerSlopeHdr%");
						tmpl.WriteCode(settings);
						for (Int_t i = 0; i < TMrbSis_3300::kNofChannels; i++) {
							tmpl.InitializeCode("%TriggerSlopeLoop%");
							tmpl.Substitute("$moduleName", moduleUC.Data());
							tmpl.Substitute("$chn", i);
							tmpl.Substitute("$trigSlope", this->GetTriggerSlope(i));
							tmpl.WriteCode(settings);
						}

						tmpl.InitializeCode("%PeakTimeHdr%");
						tmpl.WriteCode(settings);
						for (Int_t i = 0; i < TMrbSis_3300::kNofGroups; i++) {
							tmpl.InitializeCode("%PeakTimeLoop%");
							tmpl.Substitute("$moduleName", moduleUC.Data());
							tmpl.Substitute("$grp", i);
							tmpl.Substitute("$peakTime", this->GetPeakTime(i));
							tmpl.WriteCode(settings);
						}

						tmpl.InitializeCode("%GapTimeHdr%");
						tmpl.WriteCode(settings);
						for (Int_t i = 0; i < TMrbSis_3300::kNofGroups; i++) {
							tmpl.InitializeCode("%GapTimeLoop%");
							tmpl.Substitute("$moduleName", moduleUC.Data());
							tmpl.Substitute("$grp", i);
							tmpl.Substitute("$gapTime", this->GetGapTime(i));
							tmpl.WriteCode(settings);
						}

						tmpl.InitializeCode("%PulseModeHdr%");
						tmpl.WriteCode(settings);
						for (Int_t i = 0; i < TMrbSis_3300::kNofGroups; i++) {
							tmpl.InitializeCode("%PulseModeLoop%");
							tmpl.Substitute("$moduleName", moduleUC.Data());
							tmpl.Substitute("$grp", i);
							tmpl.Substitute("$pulseMode", this->PulseModeEnabled(i) ? "TRUE" : "FALSE");
							tmpl.WriteCode(settings);
						}

						tmpl.InitializeCode("%PulseLengthHdr%");
						tmpl.WriteCode(settings);
						for (Int_t i = 0; i < TMrbSis_3300::kNofGroups; i++) {
							tmpl.InitializeCode("%PulseLengthLoop%");
							tmpl.Substitute("$moduleName", moduleUC.Data());
							tmpl.Substitute("$grp", i);
							tmpl.Substitute("$pulseLength", this->GetPulseLength(i));
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

Bool_t TMrbSis_3300::MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSis_3300::MakeReadoutCode
// Purpose:        Write a piece of code for a SIS digitizing adc
// Arguments:      ofstream & RdoStrm         -- file output stream
//                 EMrbModuleTag TagIndex     -- index of tag word taken from template file
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a SIS 3300 module.
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

	TMrbResource * env = new TMrbResource("TMrbConfig", ".rootrc");

	Int_t subType = 0;
	TIterator * siter = gMrbConfig->GetLofSubevents()->MakeIterator();
	TMrbSubevent * s;
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
				fCodeTemplates.Substitute("$marabouPPCDir", gSystem->Getenv("MARABOU_PPCDIR"));
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
				fCodeTemplates.Substitute("$marabouPPCDir", gSystem->Getenv("MARABOU_PPCDIR"));
				Int_t bNo = this->GetMbsBranchNo();
				TString mbsVersion = "v62"; gMrbConfig->GetMbsVersion(mbsVersion, bNo);
				TString lynxVersion = "2.5"; gMrbConfig->GetLynxVersion(lynxVersion, bNo);
				fCodeTemplates.Substitute("$mbsVersion", mbsVersion.Data());
				fCodeTemplates.Substitute("$lynxVersion", lynxVersion.Data());
				fCodeTemplates.CopyCode(codeString);
				env->Replace(codeString);
				gSystem->ExpandPathName(codeString);
				gMrbConfig->GetLofRdoLibs()->Add(new TObjString(codeString.Data()));
			}
			break;
	}
	return(kTRUE);
}


Bool_t TMrbSis_3300::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbModuleTag TagIndex,
															TMrbVMEChannel * Channel,
															Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSis_3300::MakeReadoutCode
// Purpose:        Write a piece of code for a SIS digitizing adc
// Arguments:      ofstream & RdoStrm           -- file output stream
//                 EMrbModuleTag TagIndex       -- index of tag word taken from template file
//                 TMrbVMEChannel * Channel     -- channel
//                 Int_t Value                  -- value to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a SIS 3300 module.
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

int TMrbSis_3300::GetPageSizeChan() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSis_3300::GetChansPerPage
// Purpose:        Return page size in channels
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Returns page size in channels rather then page size id
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	static Int_t pageSizes[]	=	{131072, 16384, 4096, 2048, 1024, 512, 256, 128};
	return(pageSizes[this->GetPageSize()]);
}

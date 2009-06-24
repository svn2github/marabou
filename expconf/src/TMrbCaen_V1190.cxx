//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbCaen_V1190.cxx
// Purpose:        MARaBOU configuration: CAEN V1190 TDC
// Description:    Implements class methods to handle a Mesytec ADC type MADC32
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbCaen_V1190.cxx,v 1.1 2009-06-24 14:01:19 Rudolf.Lutter Exp $       
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
#include "TMrbResource.h"
#include "TMrbVMERegister.h"
#include "TMrbVMEChannel.h"
#include "TMrbCaen_V1190.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbCaen_V1190)

const SMrbNamedXShort kMrbEdgeDetection[] =
		{
			{	TMrbCaen_V1190::kEdgeDetectPair,		"pair" 	},
			{	TMrbCaen_V1190::kEdgeDetectTrailing,	"trailing" 	},
			{	TMrbCaen_V1190::kEdgeDetectLeading, 	"leading" 	},
			{	TMrbCaen_V1190::kEdgeDetectBoth,		"leading & trailing" 	},
			{	0,			 					NULL,		}
		};

const SMrbNamedXShort kMrbResolution[] =
		{
			{	TMrbCaen_V1190::kRes800,		"800ps" },
			{	TMrbCaen_V1190::kRes200,		"200ps" },
			{	TMrbCaen_V1190::kRes100,		"100ps" },
			{	TMrbCaen_V1190::kResOff,		"not used" },
			{	0,			 					NULL,		}
		};

const SMrbNamedXShort kMrbPairResEdge[] =
		{
			{	TMrbCaen_V1190::kPairRes100,		"100ps" },
			{	TMrbCaen_V1190::kPairRes200,		"200ps" },
			{	TMrbCaen_V1190::kPairRes400,		"400ps" },
			{	TMrbCaen_V1190::kPairRes800,		"800ps" },
			{	TMrbCaen_V1190::kPairRes1600,		"1.6ns" },
			{	TMrbCaen_V1190::kPairRes3125,		"3.12ns"	},
			{	TMrbCaen_V1190::kPairRes6250,		"6.25ns"	},
			{	TMrbCaen_V1190::kPairRes12500,		"12.5ns"	},
			{	0,			 						NULL,		}
		};

const SMrbNamedXShort kMrbPairResWidth[] =
		{
			{	TMrbCaen_V1190::kPairRes100,		"100ps" },
			{	TMrbCaen_V1190::kPairRes200,		"200ps" },
			{	TMrbCaen_V1190::kPairRes400,		"400ps" },
			{	TMrbCaen_V1190::kPairRes800,		"800ps" },
			{	TMrbCaen_V1190::kPairRes1600,		"1.6ns" },
			{	TMrbCaen_V1190::kPairRes3125,		"3.12ns"	},
			{	TMrbCaen_V1190::kPairRes6250,		"6.25ns"	},
			{	TMrbCaen_V1190::kPairRes12500,		"12.5ns"	},
			{	TMrbCaen_V1190::kPairRes25ns,		"25ns"	},
			{	TMrbCaen_V1190::kPairRes50ns,		"50ns"	},
			{	TMrbCaen_V1190::kPairRes100ns,		"100ns" },
			{	TMrbCaen_V1190::kPairRes200ns,		"200ns" },
			{	TMrbCaen_V1190::kPairRes400ns,		"400ns" },
			{	TMrbCaen_V1190::kPairRes800ns,		"800ns" },
			{	0,			 					NULL,		}
		};

const SMrbNamedXShort kMrbDoubleHitRes[] =
		{
			{	TMrbCaen_V1190::kHitRes5,		"~5ns"	},
			{	TMrbCaen_V1190::kHitRes10,		"~10ns"	},
			{	TMrbCaen_V1190::kHitRes30,		"~30ns"	},
			{	TMrbCaen_V1190::kHitRes100,		"~100ns"	},
			{	0,			 					NULL,		}
		};

const SMrbNamedXShort kMrbEventSize[] =
		{
			{	TMrbCaen_V1190::kEvtSiz0,		"none"	},
			{	TMrbCaen_V1190::kEvtSiz1,		"1 evt" },
			{	TMrbCaen_V1190::kEvtSiz2,		"2 evts"	},
			{	TMrbCaen_V1190::kEvtSiz4,		"4 evts"	},
			{	TMrbCaen_V1190::kEvtSiz8,		"8 evts"	},
			{	TMrbCaen_V1190::kEvtSiz16,		"16 evts"	},
			{	TMrbCaen_V1190::kEvtSiz32,		"32 evts"	},
			{	TMrbCaen_V1190::kEvtSiz64,		"64 evts"	},
			{	TMrbCaen_V1190::kEvtSiz128,		"128 evts"	},
			{	TMrbCaen_V1190::kEvtSizNoLim,	"no limit"	},
			{	0,			 					NULL,		}
		};

TMrbCaen_V1190::TMrbCaen_V1190(const Char_t * ModuleName, UInt_t BaseAddr) :
									TMrbVMEModule(ModuleName, "Caen_V1190", BaseAddr,
																0,
																TMrbCaen_V1190::kSegSize,
																1, 128, 1 << 19) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V1190
// Purpose:        Create a Caen TDC V1190
// Arguments:      Char_t * ModuleName      -- name of camac module
//                 UInt_t BaseAddr          -- base addr
// Results:        --
// Exceptions:
// Description:    Creates a vme module of type CAEN V1190.
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
			SetTitle("CAEN V1190 multi-hit TDC 128 chn, 19 bit"); 	// store module type
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
				fNofDataBits = 19;

				fUpdateSettings = kFALSE;
				fUpdateInterval = 0;

				fTriggerMatching = kTRUE;
				fSubtractTrigTime = kTRUE;
				fHeaderTrailer = kTRUE;
				fExtendedTriggerTag = kTRUE;

				fSettingsFile = Form("%sSettings.rc", this->GetName());

				fChannelPattern.Set(4); 		// * 32 channels
				fChannelPattern.Reset(0xFFFFFFFF);


				gMrbConfig->AddModule(this);	// append to list of modules
				gDirectory->Append(this);
			} else {
				this->MakeZombie();
			}
		}
	}
}

void TMrbCaen_V1190::DefineRegisters() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V1190::DefineRegisters
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

	kp = new TMrbNamedX(TMrbCaen_V1190::kRegWindowWidth, "WindowWidth");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, kWindowWidthDefault, 1, 4095); 
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbCaen_V1190::kRegWindowOffset, "WindowOffset");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, kWindowOffsetDefault, -2048, 40);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbCaen_V1190::kRegSearchMargin, "SearchMargin");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, kSearchMarginDefault, 0, 4095);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbCaen_V1190::kRegRejectMargin, "RejectMargin");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, kRejectMarginDefault, 1, 4095);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

	kp = new TMrbNamedX(TMrbCaen_V1190::kRegEdgeDetection, "EdgeDetection");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbCaen_V1190::kEdgeDetectDefault,
													TMrbCaen_V1190::kEdgeDetectPair,
													TMrbCaen_V1190::kEdgeDetectBoth);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("EdgeDetection");
	bNames->AddNamedX(kMrbEdgeDetection);	
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbCaen_V1190::kRegResolution, "Resolution");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbCaen_V1190::kResDefault,
													TMrbCaen_V1190::kRes800,
													TMrbCaen_V1190::kResOff);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("Resolution");
	bNames->AddNamedX(kMrbResolution);	
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbCaen_V1190::kRegPairResEdge, "PairResEdge");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbCaen_V1190::kPairResDefault,
													TMrbCaen_V1190::kPairRes100,
													TMrbCaen_V1190::kPairRes12500);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("PairResEdge");
	bNames->AddNamedX(kMrbPairResEdge);	
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbCaen_V1190::kRegPairResWidth, "PairResWidth");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbCaen_V1190::kPairResDefault,
													TMrbCaen_V1190::kPairRes100,
													TMrbCaen_V1190::kPairRes800ns);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("PairResWidth");
	bNames->AddNamedX(kMrbPairResWidth);	
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbCaen_V1190::kRegDoubleHitRes, "DoubleHitRes");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbCaen_V1190::kHitResDefault,
													TMrbCaen_V1190::kHitRes5,
													TMrbCaen_V1190::kHitRes100);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("DoubleHitRes");
	bNames->AddNamedX(kMrbDoubleHitRes);	
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbCaen_V1190::kRegEventSize, "EventSize");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0,	TMrbCaen_V1190::kEvtSizDefault,
													TMrbCaen_V1190::kEvtSiz0,
													TMrbCaen_V1190::kEvtSizNoLim);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
	bNames = new TMrbLofNamedX();
	bNames->SetName("EventSize");
	bNames->AddNamedX(kMrbEventSize);	
	bNames->SetPatternMode(kFALSE);
	rp->SetLofBitNames(bNames);
	rp->SetPatternMode(kFALSE);

	kp = new TMrbNamedX(TMrbCaen_V1190::kRegAlmostFullLevel, "AlmostFullLevel");
	rp = new TMrbVMERegister(this, 0, kp, 0, 0, 0, TMrbCaen_V1190::kAlmostFullLevelDefault, 1, 32735); 
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
}

TMrbNamedX * TMrbCaen_V1190::GetEdgeDetection() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V1190::GetEdgeDetection
// Purpose:        Return current settings of edge detection
// Arguments:      --
// Results:        TMrbNamedX * EdgeDetection
// Exceptions:
// Description:    Returns name and index of current edge detection.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t setting = this->Get(TMrbCaen_V1190::kRegEdgeDetection);
	TMrbNamedX * nx = fLofRegisters.FindByIndex(TMrbCaen_V1190::kRegEdgeDetection);
	TMrbVMERegister * rp = (TMrbVMERegister *) nx->GetAssignedObject();
	TMrbLofNamedX * bn = rp->BitNames();
	TMrbNamedX * s = bn->FindByIndex(setting);
	if (s == NULL) {
		gMrbLog->Err() << "Wrong setting - edge detection = " << setting << endl;
		gMrbLog->Flush(this->ClassName(), "GetEdgeDetection");
	}
	return(s);
}
	
TMrbNamedX * TMrbCaen_V1190::GetResolution() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V1190::GetResolution
// Purpose:        Return current settings of individual LSB
// Arguments:      --
// Results:        TMrbNamedX * Resolution
// Exceptions:
// Description:    Returns name and index of current individual LSB.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t setting = this->Get(TMrbCaen_V1190::kRegResolution);
	TMrbNamedX * nx = fLofRegisters.FindByIndex(TMrbCaen_V1190::kRegResolution);
	TMrbVMERegister * rp = (TMrbVMERegister *) nx->GetAssignedObject();
	TMrbLofNamedX * bn = rp->BitNames();
	TMrbNamedX * s = bn->FindByIndex(setting);
	if (s == NULL) {
		gMrbLog->Err() << "Wrong setting - individual LSB = " << setting << endl;
		gMrbLog->Flush(this->ClassName(), "GetResolution");
	}
	return(s);
}
	
TMrbNamedX * TMrbCaen_V1190::GetDoubleHitRes() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V1190::GetDoubleHitRes
// Purpose:        Return current settings of double hit resolution
// Arguments:      --
// Results:        TMrbNamedX * DoubleHitRes
// Exceptions:
// Description:    Returns name and index of current double hit resolution.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t setting = this->Get(TMrbCaen_V1190::kRegDoubleHitRes);
	TMrbNamedX * nx = fLofRegisters.FindByIndex(TMrbCaen_V1190::kRegDoubleHitRes);
	TMrbVMERegister * rp = (TMrbVMERegister *) nx->GetAssignedObject();
	TMrbLofNamedX * bn = rp->BitNames();
	TMrbNamedX * s = bn->FindByIndex(setting);
	if (s == NULL) {
		gMrbLog->Err() << "Wrong setting - double hit resolution = " << setting << endl;
		gMrbLog->Flush(this->ClassName(), "GetDoubleHitRes");
	}
	return(s);
}
	
TMrbNamedX * TMrbCaen_V1190::GetPairResEdge() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V1190::GetPairResEdge
// Purpose:        Return current settings of pair resolution (edge)
// Arguments:      --
// Results:        TMrbNamedX * Edge
// Exceptions:
// Description:    Returns name and index of current pair resolution.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t setting = this->Get(TMrbCaen_V1190::kRegPairResEdge);
	TMrbNamedX * nx = fLofRegisters.FindByIndex(TMrbCaen_V1190::kRegPairResEdge);
	TMrbVMERegister * rp = (TMrbVMERegister *) nx->GetAssignedObject();
	TMrbLofNamedX * bn = rp->BitNames();
	TMrbNamedX * s = bn->FindByIndex(setting);
	if (s == NULL) {
		gMrbLog->Err() << "Wrong setting - pair resolution (edge) = " << setting << endl;
		gMrbLog->Flush(this->ClassName(), "GetPairResEdge");
	}
	return(s);
}
	
TMrbNamedX * TMrbCaen_V1190::GetPairResWidth() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V1190::GetPairResWidth
// Purpose:        Return current settings of pair resolution (width)
// Arguments:      --
// Results:        TMrbNamedX * Edge
// Exceptions:
// Description:    Returns name and index of current pair resolution.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t setting = this->Get(TMrbCaen_V1190::kRegPairResWidth);
	TMrbNamedX * nx = fLofRegisters.FindByIndex(TMrbCaen_V1190::kRegPairResWidth);
	TMrbVMERegister * rp = (TMrbVMERegister *) nx->GetAssignedObject();
	TMrbLofNamedX * bn = rp->BitNames();
	TMrbNamedX * s = bn->FindByIndex(setting);
	if (s == NULL) {
		gMrbLog->Err() << "Wrong setting - pair resolution (width) = " << setting << endl;
		gMrbLog->Flush(this->ClassName(), "GetPairResWidth");
	}
	return(s);
}
	
TMrbNamedX * TMrbCaen_V1190::GetEventSize() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V1190::GetPairResWidth
// Purpose:        Return current settings of pair resolution (width)
// Arguments:      --
// Results:        TMrbNamedX * Edge
// Exceptions:
// Description:    Returns name and index of current pair resolution.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t setting = this->Get(TMrbCaen_V1190::kRegEventSize);
	TMrbNamedX * nx = fLofRegisters.FindByIndex(TMrbCaen_V1190::kRegEventSize);
	TMrbVMERegister * rp = (TMrbVMERegister *) nx->GetAssignedObject();
	TMrbLofNamedX * bn = rp->BitNames();
	TMrbNamedX * s = bn->FindByIndex(setting);
	if (s == NULL) {
		gMrbLog->Err() << "Wrong setting - hits per event = " << setting << endl;
		gMrbLog->Flush(this->ClassName(), "GetEventSize");
	}
	return(s);
}
	
Bool_t TMrbCaen_V1190::EnableChannel(Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V1190::EnableChannel
// Purpose:        Enable a channel
// Arguments:      Int_t Channel    -- channel number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Activates a tdc channel
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Channel == TMrbCaen_V1190::kAllChannels) {
		for (Int_t chn = 0; chn < TMrbCaen_V1190::kNofChannels; chn++) this->EnableChannel(chn);
		return(kTRUE);
	}

	if (Channel < 0 || Channel >= TMrbCaen_V1190::kNofChannels) {
		gMrbLog->Err() << "Channel out of range - " << Channel << " (should be in [0," << (TMrbCaen_V1190::kNofChannels - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "EnableChannel");
		return(kFALSE);
	}

	Int_t idx = Channel / 32;
	Int_t chn = Channel - idx * 32;
	UInt_t bit = 1 << chn;
	fChannelPattern[idx] |= bit;
	return(kTRUE);
}

Bool_t TMrbCaen_V1190::DisableChannel(Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V1190::DisableChannel
// Purpose:        Disable a channel
// Arguments:      Int_t Channel    -- channel number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Deactivates a tdc channel
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Channel == TMrbCaen_V1190::kAllChannels) {
		for (Int_t chn = 0; chn < TMrbCaen_V1190::kNofChannels; chn++) this->DisableChannel(chn);
		return(kTRUE);
	}

	if (Channel < 0 || Channel >= TMrbCaen_V1190::kNofChannels) {
		gMrbLog->Err() << "Channel out of range - " << Channel << " (should be in [0," << (TMrbCaen_V1190::kNofChannels - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "DisableChannel");
		return(kFALSE);
	}

	Int_t idx = Channel / 32;
	Int_t chn = Channel - idx * 32;
	UInt_t bit = 1 << chn;
	fChannelPattern[idx] &= ~bit;
	return(kTRUE);
}

Bool_t TMrbCaen_V1190::UseSettings(const Char_t * SettingsFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V1190::UseSettings
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

	TMrbResource * caenEnv = new TMrbResource("CAEN_V1190", fSettingsFile.Data());

	TString moduleName = caenEnv->Get(".ModuleName", "");
	if (moduleName.CompareTo(this->GetName()) != 0) {
		gMrbLog->Err() << "Module name different - \"" << moduleName << "\" (should be " << this->GetName() << ")" << endl;
		gMrbLog->Flush(this->ClassName(), "UseSettings");
		return(kFALSE);
	}
	moduleName(0,1).ToUpper();
	moduleName.Prepend(".");

	this->UpdateSettings(caenEnv->Get(moduleName.Data(), "UpdateSettings", kFALSE));
	this->SetUpdateInterval(caenEnv->Get(moduleName.Data(), "UpdateInterval", 0));

	this->SetTriggerMatchingOn(caenEnv->Get(moduleName.Data(), "TriggerMatching", kTRUE));
	this->EnableTriggerTimeSubtraction(caenEnv->Get(moduleName.Data(), "SubtractTriggerTime", kTRUE));
	this->SetWindowWidthTicks(caenEnv->Get(moduleName.Data(), "WindowWidth", TMrbCaen_V1190::kWindowWidthDefault));
	this->SetWindowOffsetTicks(caenEnv->Get(moduleName.Data(), "WindowOffset", TMrbCaen_V1190::kWindowOffsetDefault));
	this->SetSearchMarginTicks(caenEnv->Get(moduleName.Data(), "SearchMargin", TMrbCaen_V1190::kSearchMarginDefault));
	this->SetRejectMarginTicks(caenEnv->Get(moduleName.Data(), "RejectMargin", TMrbCaen_V1190::kRejectMarginDefault));
	this->SetEdgeDetection(caenEnv->Get(moduleName.Data(), "EdgeDetection", TMrbCaen_V1190::kEdgeDetectDefault));
	this->SetResolution(caenEnv->Get(moduleName.Data(), "Resolution", TMrbCaen_V1190::kResDefault));
	this->SetDoubleHitRes(caenEnv->Get(moduleName.Data(), "DoubleHitResolution", TMrbCaen_V1190::kHitResDefault));
	this->SetPairResEdge(caenEnv->Get(moduleName.Data(), "PairResolution.Edge", TMrbCaen_V1190::kPairResDefault));
	this->SetPairResWidth(caenEnv->Get(moduleName.Data(), "PairResolution.Width", TMrbCaen_V1190::kPairResDefault));
	this->SetEventSize(caenEnv->Get(moduleName.Data(), "EventSize", TMrbCaen_V1190::kEvtSizDefault));
	this->SetAlmostFullLevel(caenEnv->Get(moduleName.Data(), "AlmostFull", TMrbCaen_V1190::kAlmostFullLevelDefault));
	this->EnableHeaderTrailer(caenEnv->Get(moduleName.Data(), "HeaderTrailer", kTRUE));
	this->EnableExtendedTriggerTag(caenEnv->Get(moduleName.Data(), "ExtTriggerTag", kTRUE));

	return(kTRUE);
}

Bool_t TMrbCaen_V1190::SaveSettings(const Char_t * SettingsFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V1190::SaveSettings
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
	TString tf = "Module_Caen_V1190.rc.code";
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
						tmpl.Substitute("$moduleFirmware", "n/a");
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%TriggerSettings%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.Substitute("$trigMatch", (this->TriggerMatchingIsOn() ? "TRUE" : "FALSE"));
						tmpl.Substitute("$width", this->GetWindowWidthTicks());
						tmpl.Substitute("$offset", this->GetWindowOffsetTicks());
						tmpl.Substitute("$search", this->GetSearchMarginTicks());
						tmpl.Substitute("$reject", this->GetRejectMarginTicks());
						tmpl.Substitute("$subTrigTime", (this->TriggerTimeSubtractionEnabled() ? "TRUE" : "FALSE"));
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%EdgeDetection%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.Substitute("$detectMode", this->GetEdgeDetection()->GetIndex());
						tmpl.Substitute("$resolution", this->GetResolution()->GetIndex());
						tmpl.Substitute("$hitRes", this->GetDoubleHitRes()->GetIndex());
						tmpl.Substitute("$pairEdge", this->GetPairResEdge()->GetIndex());
						tmpl.Substitute("$pairWidth", this->GetPairResWidth()->GetIndex());
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%EventSettings%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.Substitute("$eventSize", this->GetEventSize()->GetIndex());
						tmpl.Substitute("$headTrail", (this->HeaderTrailerEnabled() ? "TRUE" : "FALSE"));
						tmpl.Substitute("$extTrigTag", (this->ExtendedTriggerTagEnabled() ? "TRUE" : "FALSE"));
						tmpl.Substitute("$almostFull", this->GetAlmostFullLevel());
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%ChannelSettings%");
						tmpl.Substitute("$moduleName", moduleUC.Data());
						tmpl.Substitute("$chPat0", fChannelPattern[0], 16);
						tmpl.Substitute("$chPat32", fChannelPattern[1], 16);
						tmpl.Substitute("$chPat64", fChannelPattern[2], 16);
						tmpl.Substitute("$chPat96", fChannelPattern[3], 16);
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

Bool_t TMrbCaen_V1190::MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V1190::MakeReadoutCode
// Purpose:        Write a piece of code for a Mesytec MADC-32
// Arguments:      ofstream & RdoStrm         -- file output stream
//                 EMrbModuleTag TagIndex     -- index of tag word taken from template file
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a Mesytec MADC-32.
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
			fCodeTemplates.Substitute("$serial", this->GetSerial());
			fCodeTemplates.Substitute("$dumpFile", dump.Data());
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
				fCodeTemplates.Substitute("$lynxVersion", gEnv->GetValue("TMbsSetup.LynxVersion", "2.5"));
				fCodeTemplates.CopyCode(codeString, " \\\n\t\t\t\t");
				gMrbConfig->GetLofRdoLibs()->Add(new TObjString(codeString.Data()));
			}
			break;
	}
	return(kTRUE);
}


Bool_t TMrbCaen_V1190::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbModuleTag TagIndex,
															TMrbVMEChannel * Channel,
															Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V1190::MakeReadoutCode
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

Bool_t TMrbCaen_V1190::CheckSubeventType(TMrbSubevent * Subevent) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Madc32::CheckSubeventType
// Purpose:        Check if calling subevent is applicable
// Arguments:      
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Makes sure that a subevent of type [10,4x] (CAEN)
//                 is calling.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Subevent->GetType() != 10) return(kFALSE);
	if (Subevent->GetSubtype() < 45 || Subevent->GetSubtype() > 46) return(kFALSE);
	else															return(kTRUE);
}

void TMrbCaen_V1190::PrintSettings(ostream & Out) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V1190::PrintSettings
// Purpose:        Print settings
// Arguments:      ostream & Out           -- where to print
// Results:        --
// Exceptions:
// Description:    Prints settings to stream.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Out << "=========================================================================" << endl;
	Out << " CAEN V1190 settings" << endl;
	Out << "-------------------------------------------------------------------------" << endl;
	Out << " Module                  : "	<< this->GetName() << endl;
	Out << " Serial                  : "	<< this->GetSerial() << endl;
	if (this->SettingsToBeUpdated()) {
		Out << " Update settings         : every " << this->GetUpdateInterval() << " count(s)" << endl;
	} else {
		Out << " Update settings         : never" << endl;
	}
	Out << " Trigger mode            : "	<< (this->TriggerMatchingIsOn() ? "Trigger matching" : "Continuous storage") << endl;
	Out << " Window width            : "	<< this->GetWindowWidthTicks() << " (" << this->GetWindowWidthNsec() << "ns)" << endl;
	Out << " Window offset           : "	<< this->GetWindowOffsetTicks() << " (" << this->GetWindowOffsetNsec() << "ns)" << endl;
	Out << " Search margin           : "	<< this->GetSearchMarginTicks() << " (" << this->GetSearchMarginNsec() << "ns)" << endl;
	Out << " Reject margin           : "	<< this->GetRejectMarginTicks() << " (" << this->GetRejectMarginNsec() << "ns)" << endl;
	Out << " Subtract trigger time   : "	<< (this->TriggerTimeSubtractionEnabled() ? "On" : "Off") << endl;
	Out << " Edge detection          : "	<< "0x" << setbase(16) << this->GetEdgeDetection()->GetIndex() << setbase(10) << " (" << this->GetEdgeDetection()->GetName() << ")" << endl;
	Out << " Resolution              : "	<< "0x" << setbase(16) << this->GetResolution()->GetIndex() << setbase(10) << " (" << this->GetResolution()->GetName() << ")" << endl;
	Out << " Pair resolution (edge)  : "	<< "0x" << setbase(16) << this->GetPairResEdge()->GetIndex() << setbase(10) << " (" << this->GetPairResEdge()->GetName() << ")" << endl;
	Out << "                (width)  : "	<< "0x" << setbase(16) << this->GetPairResWidth()->GetIndex() << setbase(10) << " (" << this->GetPairResWidth()->GetName() << ")" << endl;
	Out << " Double hit resolution   : "	<< "0x" << setbase(16) << this->GetDoubleHitRes()->GetIndex() << setbase(10) << " (" << this->GetDoubleHitRes()->GetName() << ")" << endl;
	Out << " Hits per event          : "	<< "0x" << setbase(16) << this->GetEventSize()->GetIndex() << setbase(10) << " (" << this->GetEventSize()->GetName() << ")" << endl;
	Out << " Write header/trailer    : "	<< (this->HeaderTrailerEnabled() ? "Yes" : "No") << endl;
	Out << " Write ext trigger tag   : "	<< (this->ExtendedTriggerTagEnabled() ? "Yes" : "No") << endl;
	Out << " Almost full level       : "	<< this->GetAlmostFullLevel() << endl;
	Out << " Channel pattern 00 - 31 : "	<< "0x" << setbase(16) << fChannelPattern[0] << setbase(10) << endl;
	Out << "                 32 - 63 : "	<< "0x" << setbase(16) << fChannelPattern[1] << setbase(10) << endl;
	Out << "                 64 - 95 : "	<< "0x" << setbase(16) << fChannelPattern[2] << setbase(10) << endl;
	Out << "                 96 - 128: "	<< "0x" << setbase(16) << fChannelPattern[3] << setbase(10) << endl;
	Out << "-------------------------------------------------------------------------" << endl << endl;
}

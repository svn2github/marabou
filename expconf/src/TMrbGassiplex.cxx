//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbGassiplex.cxx
// Purpose:        MARaBOU configuration: Gassiplex
// Description:    Implements class methods to handle a Gassiplex module
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id$
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
#include "TMrbGassiplex.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbGassiplex)


TMrbGassiplex::TMrbGassiplex(const Char_t * ModuleName, UInt_t BaseAddr, Int_t NofChannels) :
									TMrbVMEModule(ModuleName, "Gassiplex", BaseAddr,
																TMrbGassiplex::kAddrMod,
																0,
																1, NofChannels, 1 << 11) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbGassiplex
// Purpose:        Create a Gassiplex module
// Arguments:      Char_t * ModuleName      -- name of camac module
//                 UInt_t BaseAddr          -- base addr
//                 Int_t NofChannels        -- number of channels
// Results:        --
// Exceptions:
// Description:    Creates a vme module of type Gassiplex.
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
		} else if ((NofChannels % kNofChansPerBoard) != 0) {
			gMrbLog->Err() << ModuleName << ": Wrong number of channels - " << NofChannels << " not modulo " << kNofChansPerBoard << endl;
			gMrbLog->Flush(this->ClassName());
			this->MakeZombie();
		} else {
			TString ppc = gEnv->GetValue("TMbsSetup.ProcType", "RIO3");
			if (ppc.CompareTo("RIO3") != 0 && ppc.CompareTo("RIO4") != 0) {
				gMrbLog->Err() << ModuleName << ": Wrong CPU type - " << ppc << " (should be RIO3 or RIO4)" << endl;
				gMrbLog->Flush(this->ClassName());
				this->MakeZombie();
			}
		}
		
		if (!this->IsZombie()) {
			SetTitle("Gassiplex multiplexer Nx64 chn 11 bit"); 	// store module type
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
				fNofDataBits = 11;
				fBlockReadout = kTRUE;			// module has block readout
				
				fNofChannels = NofChannels;
				fBlockXfer = kFALSE;
				fSettingsFile = Form("%sSettings.rc", this->GetName());

				gMrbConfig->AddModule(this);				// append to list of modules
				gDirectory->Append(this);
			} else {
				this->MakeZombie();
			}
		}
	}
}

void TMrbGassiplex::DefineRegisters() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbGassiplex::DefineRegisters
// Purpose:        Define module registers
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Defines special registers
// Keywords:
//////////////////////////////////////////////////////////////////////////////

}

TEnv * TMrbGassiplex::UseSettings(const Char_t * SettingsFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbGassiplex::UseSettings
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
		return(NULL);
	}

	return(NULL);		// not fully implemented!
}

Bool_t TMrbGassiplex::MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbGassiplex::MakeReadoutCode
// Purpose:        Write a piece of code for a SIS digitizing adc
// Arguments:      ofstream & RdoStrm         -- file output stream
//                 EMrbModuleTag TagIndex     -- index of tag word taken from template file
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a SIS 3302 module.
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
	mnemoUC(0,1).ToUpper();

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
			fCodeTemplates.Substitute("$moduleNameLC", moduleNameLC);
			fCodeTemplates.Substitute("$moduleNameUC", moduleNameUC);
			fCodeTemplates.Substitute("$moduleTitle", this->GetTitle());
			fCodeTemplates.Substitute("$modulePosition", this->GetPosition());
			fCodeTemplates.Substitute("$moduleSerial", this->GetSerial());
			fCodeTemplates.Substitute("$sevtSubtype", subType);
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$baseAddr", (Int_t) this->GetBaseAddr(), 16);
			fCodeTemplates.Substitute("$chnPattern", (Int_t) this->GetPatternOfChannelsUsed(), 16);
			fCodeTemplates.Substitute("$bufferSize", gMrbConfig->GetSevtSize(), 16);
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
			fCodeTemplates.Substitute("$moduleNameLC", moduleNameLC);
			fCodeTemplates.Substitute("$moduleNameUC", moduleNameUC);
			fCodeTemplates.Substitute("$nofParams", this->GetNofChannelsUsed());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.WriteCode(RdoStrm);
			break;
		case TMrbConfig::kModuleReadModule:
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$moduleNameLC", moduleNameLC);
			fCodeTemplates.Substitute("$moduleNameUC", moduleNameUC);
			fCodeTemplates.Substitute("$moduleSerial", this->GetSerial());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.WriteCode(RdoStrm);
			break;
		case TMrbConfig::kModuleDefineIncludePaths:
			{
				TString codeString;
				fCodeTemplates.InitializeCode();
				fCodeTemplates.Substitute("$marabouPath", gSystem->Getenv("MARABOU"));
				fCodeTemplates.Substitute("$lynxVersion", gEnv->GetValue("TMbsSetup.LynxVersion", "3.1"));
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
					fCodeTemplates.Substitute("$lynxVersion", gEnv->GetValue("TMbsSetup.LynxVersion", "3.1"));
				fCodeTemplates.ExpandPathName();
				fCodeTemplates.CopyCode(codeString);
				env->Replace(codeString);
				gSystem->ExpandPathName(codeString);
				gMrbConfig->GetLofRdoLibs()->Add(new TObjString(codeString.Data()));
			}
			break;
	}
	return(kTRUE);
}


Bool_t TMrbGassiplex::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbModuleTag TagIndex,
															TMrbVMEChannel * Channel,
															Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbGassiplex::MakeReadoutCode
// Purpose:        Write a piece of code for a SIS digitizing adc
// Arguments:      ofstream & RdoStrm           -- file output stream
//                 EMrbModuleTag TagIndex       -- index of tag word taken from template file
//                 TMrbVMEChannel * Channel     -- channel
//                 Int_t Value                  -- value to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a SIS 3302 module.
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
	mnemoUC(0,1).ToUpper();

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
			fCodeTemplates.Substitute("$moduleNameLC", moduleNameLC);
			fCodeTemplates.Substitute("$moduleNameUC", moduleNameUC);
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

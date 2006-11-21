//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbSis_3820.cxx
// Purpose:        MARaBOU configuration: SIS modules
// Description:    Implements class methods to handle a SIS scaler type 3820 
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbSis_3820.cxx,v 1.8 2006-11-21 13:38:46 Rudolf.Lutter Exp $       
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
#include "TMrbSis_3820.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbSis_3820)

TMrbSis_3820::TMrbSis_3820(const Char_t * ModuleName, UInt_t BaseAddr, Int_t FifoDepth) :
									TMrbVMEScaler(ModuleName, "Sis_3820", BaseAddr,
																TMrbSis_3820::kAddrMod,
																TMrbSis_3820::kSegSize,
																1, 32 * FifoDepth, 1 << 31) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSis_3820
// Purpose:        Create a scaler of type SIS 3820
// Arguments:      Char_t * ModuleName      -- name of camac module
//                 UInt_t BaseAddr          -- base addr
// Results:        --
// Exceptions:
// Description:    Creates a vme scaler of type SIS 3820.
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
			SetTitle("SIS 3820 scaler 32 x 32 bit"); 	// store module type
			codeFile = fModuleID.GetName();
			codeFile += ".code";
			if (LoadCodeTemplates(codeFile)) {
				DefineRegisters();
				mTypeBits = TMrbConfig::kModuleVME | TMrbConfig::kModuleListMode | TMrbConfig::kModuleScaler;
				gMrbConfig->GetLofModuleTypes()->Pattern2String(mType, mTypeBits);
				fModuleType.Set(mTypeBits, mType.Data());
				fDataType = gMrbConfig->GetLofDataTypes()->FindByIndex(TMrbConfig::kDataUInt);
				fNofShortsPerChannel = 2;
				fFifoDepth = FifoDepth; 		// fifo depth per channel
				fBlockReadout = kTRUE;			// module has block readout
				fNonClearingMode = kFALSE;		// clear on copy
				fDataFormat24 = kFALSE; 		// output 32 bit, no channel ids
				gMrbConfig->AddModule(this);				// append to list of modules
				gMrbConfig->AddScaler(this);				// and to list of scalers
				gDirectory->Append(this);
			} else {
				this->MakeZombie();
			}
		}
	}
}

void TMrbSis_3820::DefineRegisters() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSis_3820::DefineRegisters
// Purpose:        Define module registers
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Defines status, common threshold, lower & upper thresholds, and offset.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	TMrbVMERegister * rp;

// input mode
	kp = new TMrbNamedX(TMrbSis_3820::kRegControlStatus, "InputMode");
	rp = new TMrbVMERegister(this, 0, kp,
								TMrbSis_3820::kOffsControlStatus,
								TMrbSis_3820::kOffsControlStatus,
								TMrbSis_3820::kOffsControlStatus,
								1, 0, 6);
	rp->SetFromResource(1);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

// output mode
	kp = new TMrbNamedX(TMrbSis_3820::kRegControlStatus, "OutputMode");
	rp = new TMrbVMERegister(this, 0, kp,
								TMrbSis_3820::kOffsControlStatus,
								TMrbSis_3820::kOffsControlStatus,
								TMrbSis_3820::kOffsControlStatus,
								0, 0, 3);
	rp->SetFromResource(0);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);

// lne source
	kp = new TMrbNamedX(TMrbSis_3820::kRegControlStatus, "LNEsource");
	rp = new TMrbVMERegister(this, 0, kp,
								TMrbSis_3820::kOffsControlStatus,
								TMrbSis_3820::kOffsControlStatus,
								TMrbSis_3820::kOffsControlStatus,
								1, 0, 4);
	rp->SetFromResource(1);
	kp->AssignObject(rp);
	fLofRegisters.AddNamedX(kp);
}

Bool_t TMrbSis_3820::MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSis_3820::MakeReadoutCode
// Purpose:        Write a piece of code for a SIS scaler
// Arguments:      ofstream & RdoStrm         -- file output stream
//                 EMrbModuleTag TagIndex     -- index of tag word taken from template file
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a SIS 3820 scaler module.
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
			{
				fCodeTemplates.InitializeCode();
				fCodeTemplates.Substitute("$moduleName", this->GetName());
				fCodeTemplates.Substitute("$moduleTitle", this->GetTitle());
				fCodeTemplates.Substitute("$modulePosition", this->GetPosition());
				fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
				fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
				fCodeTemplates.Substitute("$baseAddr", (Int_t) this->GetBaseAddr(), 16);
				fCodeTemplates.Substitute("$inputMode", (Int_t) this->Get("InputMode"));
				fCodeTemplates.Substitute("$outputMode", (Int_t) this->Get("OutputMode"));
				fCodeTemplates.Substitute("$lneSource", (Int_t) this->Get("LNESource"));
				fCodeTemplates.Substitute("$nonClearingMode", this->NonClearingMode() ? 0x1 : 0x0);
				fCodeTemplates.Substitute("$DataFormat", this->DataFormat24() ? 0x4 : 0x0);
				Int_t pat;
				if (this->GetNofChannelsUsed() < 32) {
					if (this->CheckIfPatternIsContiguous()) {
						pat = ~(this->GetPatternOfChannelsUsed());
					} else {
						pat = 0;
						gMrbLog->Err()	<< "Pattern of channels used is not contiguous - 0x"
										<< setbase(16) << this->GetPatternOfChannelsUsed() << setbase(10)
										<< " (ignored: using 32 chns)" << endl;
						gMrbLog->Flush(this->ClassName(), "MakeReadoutCode");
					}
				} else {
					pat = 0;
				}
				fCodeTemplates.Substitute("$unusedChannels", pat, 16);
				fCodeTemplates.WriteCode(RdoStrm);
			}
			break;
		case TMrbConfig::kModuleStartAcquisition:
		case TMrbConfig::kModuleStopAcquisition:
			{
				if (this->Get("LNESource") == 0) {
					fCodeTemplates.InitializeCode("%FSR%");
					fCodeTemplates.Substitute("$moduleName", this->GetName());
					fCodeTemplates.WriteCode(RdoStrm);
				}
				fCodeTemplates.InitializeCode("%S%");
				fCodeTemplates.Substitute("$moduleName", this->GetName());
				fCodeTemplates.Substitute("$moduleTitle", this->GetTitle());
				fCodeTemplates.Substitute("$modulePosition", this->GetPosition());
				fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
				fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
				Int_t pat;
				if (this->GetNofChannelsUsed() < 32) {
					if (this->CheckIfPatternIsContiguous()) {
						pat = ~(this->GetPatternOfChannelsUsed());
					} else {
						pat = 0;
						gMrbLog->Err()	<< "Pattern of channels used is not contiguous - 0x"
										<< setbase(16) << this->GetPatternOfChannelsUsed() << setbase(10)
										<< " (ignored: using 32 chns)" << endl;
						gMrbLog->Flush(this->ClassName(), "MakeReadoutCode");
					}
				} else {
					pat = 0;
				}
				fCodeTemplates.Substitute("$unusedChannels", pat, 16);
				fCodeTemplates.WriteCode(RdoStrm);
			}
			break;
		case TMrbConfig::kModuleClearModule:
		case TMrbConfig::kModuleFinishReadout:
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
			{
				Int_t nofChannels = this->GetNofChannelsUsed();
				if (nofChannels < 32) {
					if (!this->CheckIfPatternIsContiguous()) {
						gMrbLog->Err()	<< "Pattern of channels used is not contiguous - 0x"
										<< setbase(16) << this->GetPatternOfChannelsUsed() << setbase(10)
										<< " (ignored: using 32 chns)" << endl;
						gMrbLog->Flush(this->ClassName(), "MakeReadoutCode");
						nofChannels = 32;
					}
				}
				TString iniTag;
				iniTag = (fFifoDepth > 1 ? "%MH" : "%SH");
				iniTag += (nofChannels < 32) ? "S%" : "%";
				fCodeTemplates.InitializeCode(iniTag.Data());
				fCodeTemplates.Substitute("$moduleName", this->GetName());
				fCodeTemplates.Substitute("$fifoDepth", fFifoDepth);
				fCodeTemplates.Substitute("$nofChannels", nofChannels);
				fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
				fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
				fCodeTemplates.WriteCode(RdoStrm);
			}
			break;
	}
	return(kTRUE);
}


Bool_t TMrbSis_3820::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbModuleTag TagIndex,
															TObject * Channel,
															Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSis_3820::MakeReadoutCode
// Purpose:        Write a piece of code for a SIS scaler
// Arguments:      ofstream & RdoStrm           -- file output stream
//                 EMrbModuleTag TagIndex       -- index of tag word taken from template file
//                 TObject * Channel            -- channel
//                 Int_t Value                  -- value to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a SIS 3820 scaler module.
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
			fCodeTemplates.Substitute("$chnName", chn->GetName());
			fCodeTemplates.Substitute("$chnNo", chn->GetAddr());
			fCodeTemplates.Substitute("$data", Value);
			fCodeTemplates.WriteCode(RdoStrm);
			break;
	}
	return(kTRUE);
}

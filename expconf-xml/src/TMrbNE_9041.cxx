//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbNE_9041.cxx
// Purpose:        MARaBOU configuration: Nuclear Enterprises dual input register
// Description:    Implements class methods to handle dual input reg NE 9041
// Header files:   TMrbConfig.h  -- MARaBOU class definitions
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbNE_9041.cxx,v 1.1 2008-03-05 12:37:16 Rudolf.Lutter Exp $       
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
#include "TMrbCamacChannel.h"
#include "TMrbNE_9041.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

// input class definitions */

ClassImp(TMrbNE_9041)

TMrbNE_9041::TMrbNE_9041(const Char_t * ModuleName, const Char_t * ModulePosition) :
                                         TMrbCamacModule(ModuleName, "NE_9041", ModulePosition, 0, 10, 1024) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbNE_9041
// Purpose:        Create a dual input reg NE 9041
// Arguments:      Char_t * ModuleName      -- name of camac module
//                 Char_t * ModulePosition  -- B.C.N for this module
// Results:        --
// Exceptions:
// Description:    Creates a camac module of type NE 9041.
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
			SetTitle("Dual Input Register NE 9041"); 			// set module type
			mTypeBits = TMrbConfig::kModuleCamac | TMrbConfig::kModuleListMode;
			gMrbConfig->GetLofModuleTypes()->Pattern2String(mType, mTypeBits);
			fModuleType.Set(mTypeBits, mType.Data());
			fWaitForQ = kTRUE;

			codeFile = fModuleID.GetName();
			codeFile += ".code";
			if (LoadCodeTemplates(codeFile)) {
				gMrbConfig->AddModule(this);				// append to list of modules
				gDirectory->Append(this);
			} else {
				this->MakeZombie();
			}
		}
	}
}

Bool_t TMrbNE_9041::MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbNE_9041::MakeReadoutCode
// Purpose:        Write a piece of code for a nuclear enterprise register
// Arguments:      ofstream & RdoStrm         -- file output stream
//                 EMrbModuleTag TagIndex      -- index of tag word taken from template file
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a nuclear enterprise NE 9041 module.
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
		case TMrbConfig::kModuleInitCommonCode:
		case TMrbConfig::kModuleInitModule:
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
			fCodeTemplates.Substitute("$moduleTitle", this->GetTitle());
			fCodeTemplates.Substitute("$modulePosition", this->GetPosition());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.WriteCode(RdoStrm);
			break;
	}
	return(kTRUE);
}


Bool_t TMrbNE_9041::MakeReadoutCode(ofstream & RdoStrm, 	TMrbConfig::EMrbModuleTag TagIndex,
															TMrbCamacChannel * Channel,
															Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbNE_9041::MakeReadoutCode
// Purpose:        Write a piece of code for a nuclear enterprise register
// Arguments:      ofstream & RdoStrm           -- file output stream
//                 EMrbModuleTag TagIndex       -- index of tag word taken from template file
//                 TMrbCamacChannel * Channel   -- channel
//                 Int_t Value                  -- value to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a nuclear enterprise NE 9041 module.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t chnNo;
	TString iniTag;
	TString mnemoLC, mnemoUC;

	chnNo = Channel->GetAddr();

	if (!fCodeTemplates.FindCode(TagIndex)) {
		gMrbLog->Err()	<< "No code loaded for tag "
						<< gMrbConfig->fLofModuleTags.FindByIndex(TagIndex)->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "MakeReadoutCode");
		return(kFALSE);
	}

	mnemoLC = this->GetMnemonic();
	mnemoUC = mnemoLC;
	mnemoUC.ToUpper();

	iniTag = this->WaitForQ() ? "%HQ%" : "%HL%";

	switch (TagIndex) {
		case TMrbConfig::kModuleInitChannel:
		case TMrbConfig::kModuleWriteSubaddr:
		case TMrbConfig::kModuleSetupReadout:
		case TMrbConfig::kModuleIncrementChannel:
		case TMrbConfig::kModuleSkipChannels:
		case TMrbConfig::kModuleStoreData:
			break;
		case TMrbConfig::kModuleReadChannel:
			fCodeTemplates.InitializeCode("%N%");
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$chnName", Channel->GetName());
			fCodeTemplates.Substitute("$chnNo", chnNo);
			fCodeTemplates.Substitute("$subAddr", chnNo & 1);
			fCodeTemplates.WriteCode(RdoStrm);
			if ((chnNo & 1) && (chnNo < fNofChannels - 1)) {
				fCodeTemplates.InitializeCode(iniTag.Data());
				fCodeTemplates.Substitute("$crateNo", this->GetCrate());
				fCodeTemplates.Substitute("$moduleName", this->GetName());
				fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
				fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
				fCodeTemplates.Substitute("$chnName", Channel->GetName());
				fCodeTemplates.Substitute("$chnNo", chnNo);
				fCodeTemplates.WriteCode(RdoStrm);
			}
			break;
	}
	return(kTRUE);
}

//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbUct_8904.cxx
// Purpose:        MARaBOU configuration: scaler of type UCT
// Description:    Implements class methods to handle UCT scaler
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TDirectory.h"

#include "TString.h"
#include "TMrbLogger.h"
#include "TMrbConfig.h"
#include "TMrbCamacChannel.h"
#include "TMrbUct_8904.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbUct_8904)

TMrbUct_8904::TMrbUct_8904(const Char_t * ScalerName, const Char_t * ScalerPosition) :
						TMrbCamacScaler(ScalerName, "Uct_8904", ScalerPosition, 1, 1, 1 << 32 - 1) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbUct_8904
// Purpose:        Create a scaler object of type UCT
// Arguments:      Char_t * ScalerName      -- scaler name
//                 Char_t * ScalerPosition  -- camac position
// Results:        --
// Exceptions:
// Description:    Create a new scaler of type UCT (TUM/N. Franz).
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
		} else if (gMrbConfig->FindModule(ScalerName)) {
			gMrbLog->Err() << ScalerName << ": Module name already in use" << endl;
			gMrbLog->Flush(this->ClassName());
			this->MakeZombie();
		} else {
			SetTitle("UCT Universal Timer/Counter N.Franz TUM");

			codeFile = fModuleID.GetName();
			codeFile += ".code";
			if (LoadCodeTemplates(codeFile)) {
				mTypeBits = TMrbConfig::kModuleCamac | TMrbConfig::kModuleScaler;
				gMrbConfig->GetLofModuleTypes()->Pattern2String(mType, mTypeBits);
				fModuleType.Set(mTypeBits, mType.Data());
				fDataType = gMrbConfig->GetLofDataTypes()->FindByIndex(TMrbConfig::kDataUInt);
				fNofShortsPerChannel = 2;

				this->SetFunctionName("uct_init", TMrbConfig::kScalerFctInit);
				this->SetFunctionName("uct_read", TMrbConfig::kScalerFctRead);
				this->SetFunctionName("uct_read", TMrbConfig::kScalerFctDeadTime);

				gMrbConfig->AddModule(this); 	// insert scaler in lists	
				gMrbConfig->AddScaler(this);

				gDirectory->Append(this);
			} else {
				this->MakeZombie();
			}
		}
	}
}

Bool_t TMrbUct_8904::MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbUct_8904::MakeReadoutCode
// Purpose:        Write a piece of code for a UCT scaler
// Arguments:      ofstream & RdoStrm           -- file output stream
//                 EMrbReadoutTag TagIndex      -- index of tag word taken from template file
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a uct 8904 module.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString mnemoLC, mnemoUC;

	mnemoLC = this->GetMnemonic();
	mnemoUC = mnemoLC;
	mnemoUC.ToUpper();

	if (!fCodeTemplates.FindCode(TagIndex)) {
		gMrbLog->Err()	<< "No code loaded for tag "
						<< gMrbConfig->fLofModuleTags.FindByIndex(TagIndex)->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "MakeReadoutCode");
		return(kFALSE);
	}

	switch (TagIndex) {
		case TMrbConfig::kModuleInitModule:
		case TMrbConfig::kModuleClearModule:
		case TMrbConfig::kModuleStartAcquisition:
		case TMrbConfig::kModuleStopAcquisition:
		case TMrbConfig::kModuleStartAcquisitionGroup:
		case TMrbConfig::kModuleStopAcquisitionGroup:
		case TMrbConfig::kModuleFinishReadout:
		case TMrbConfig::kModuleUtilities:
		case TMrbConfig::kModuleDefineGlobals:
		case TMrbConfig::kModuleDefineLocalVarsInit:
		case TMrbConfig::kModuleDefineLocalVarsReadout:
		case TMrbConfig::kModuleDefinePrototypes:
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.Substitute("$moduleTitle", this->GetTitle());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$modulePosition", this->GetPosition());
			fCodeTemplates.Substitute("$extGate", (Int_t) this->HasExternalGate());
			fCodeTemplates.WriteCode(RdoStrm);
			break;
	}
	return(kTRUE);
}

Bool_t TMrbUct_8904::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbModuleTag TagIndex,
															TObject * Channel,
															Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbUct_8904::MakeReadoutCode
// Purpose:        Write a piece of code for a caen scaler
// Arguments:      ofstream & RdoStrm           -- file output stream
//                 EMrbModuleTag TagIndex       -- index of tag word taken from template file
//                 TObject * Channel            -- channel
//                 Int_t Value                  -- value to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a caen v260 scaler module.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbCamacChannel * chn;
	TString mnemoLC, mnemoUC;

	chn = (TMrbCamacChannel *) Channel;

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

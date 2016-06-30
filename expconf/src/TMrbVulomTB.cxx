//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbVulomTB.cxx
// Purpose:        MARaBOU configuration: VULOM
// Description:    Implements class methods to handle a VULOM triggerBox
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbVulomTB.cxx,v 1.6 2009-09-10 12:52:30 Rudolf.Lutter Exp $
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
#include "TMrbVMERegister.h"
#include "TMrbVulomTB.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbVulomTB)


TMrbVulomTB::TMrbVulomTB(const Char_t * ModuleName, UInt_t BaseAddr, Int_t NofScalerChans, UInt_t ScalerSets) :
									TMrbVMEModule(ModuleName, "VulomTB", BaseAddr,
																TMrbVulomTB::kAddrMod,
																TMrbVulomTB::kSegSize,
																0, 3 * NofScalerChans, 1 << 31) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVulomTB
// Purpose:        Create a vulom module
// Arguments:      Char_t * ModuleName      -- name of camac module
//                 UInt_t BaseAddr          -- base addr
//                 Int_t NofScalerChans     -- number of scaler channels
//                 UInt_t ScalerSets        -- scaler sets
// Results:        --
// Exceptions:
// Description:    Creates a vulom of type trigger box.
//                 ScalerSets may be used to (de)activate scalers sets:
//                       kScSetInput, BIT(0)       - input
//                       kScSetInhibit, BIT(1)     - after inhibit
//                       kScSetScaleDown, BIT(2)   - after scale down
//                       and any combinations
//                       default is kScSetAll, 0x7 - all 3 sets of scalers
//                 NofScalerChans denotes the number of scaler channels used.
//                 The number of channels as seen by MARaBOU is then
//                       fNofChannels =  (# scaler sets) * NofScalerChans
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
			SetTitle("VulomTB TriggerBox GSI"); 	// store module type

			mTypeBits = TMrbConfig::kModuleVME | TMrbConfig::kModuleListMode | TMrbConfig::kModuleScaler;
			gMrbConfig->GetLofModuleTypes()->Pattern2String(mType, mTypeBits);
			fModuleType.Set(mTypeBits, mType.Data());

			fDataType = gMrbConfig->fLofDataTypes.FindByIndex(TMrbConfig::kDataUInt);
			fNofShortsPerChannel = 2;
			fNofShortsPerDatum = 2;
			fNofDataBits = 32;
			fBlockReadout = kTRUE;

			fNofScalerSets = 0;
			fScalerSetMask = ScalerSets;
			if (ScalerSets & kScSetInput) fNofScalerSets++;
			if (ScalerSets & kScSetInhibit) fNofScalerSets++;
			if (ScalerSets & kScSetScaleDown) fNofScalerSets++;
			fNofScalerChans = NofScalerChans;
			fScaleDown.Set(NofScalerChans);
			fEnableMask = 0;
			for (Int_t i = 0; i < NofScalerChans; i++) fEnableMask |= (1 << i);
			fRdoScaleDown = 1;

			codeFile = fModuleID.GetName();
			codeFile += ".code";
			if (LoadCodeTemplates(codeFile)) {
				gMrbConfig->AddModule(this);				// append to list of modules
				gDirectory->Append(this);
			} else {
				this->MakeZombie();
			}
			this->AllocateHistograms(kFALSE);				// don't allocate histograms
		}
	}
}

Bool_t TMrbVulomTB::SetScaleDown(Int_t ScaleDown, Int_t ChannelNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVulomTB::SetScaleDown
// Purpose:        Set scale down value
// Arguments:      Int_t ScaleDown      -- scale down value
//                 Int_t ChannelNo      -- channel number
// Results:        kTRUE/kFALSE
// Description:    Defines scale down value for a given channel
//////////////////////////////////////////////////////////////////////////////

	if (ChannelNo < 0 || ChannelNo > this->GetNofScalerChans()) {
		gMrbLog->Err() << "[" << this->GetName() << "]: Channel out of range - " << ChannelNo << " (should be in [0," << (this->GetNofScalerChans() - 1) << "])"<< endl;
		gMrbLog->Flush(this->ClassName(), "SetScaleDown");
		return(kFALSE);
	}
	if (ScaleDown < 1 || ScaleDown > kMaxScaleDown) {
		gMrbLog->Err() << "[" << this->GetName() << ", chn " << ChannelNo << "]: Scale down out of range - " << ScaleDown << " (should be in [1," << kMaxScaleDown << "])"<< endl;
		gMrbLog->Flush(this->ClassName(), "SetScaleDown");
		return(kFALSE);
	}
	fScaleDown[ChannelNo] = ScaleDown;
	return(kTRUE);
}

Int_t TMrbVulomTB::GetScaleDown(Int_t ChannelNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVulomTB::GetScaleDown
// Purpose:        Get scale down value
// Arguments:      Int_t ChannelNo      -- channel number
// Results:        Int_t ScaleDown      -- scale down value
// Description:    Returns scale down value for a given channel
//////////////////////////////////////////////////////////////////////////////

	if (ChannelNo < 0 || ChannelNo > this->GetNofScalerChans()) {
		gMrbLog->Err() << "[" << this->GetName() << "]: Channel out of range - " << ChannelNo << " (should be in [0," << (this->GetNofScalerChans() - 1) << "])"<< endl;
		gMrbLog->Flush(this->ClassName(), "GetScaleDown");
		return(-1);
	}
	return(fScaleDown[ChannelNo]);
}

Bool_t TMrbVulomTB::MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVulomTB::MakeReadoutCode
// Purpose:        Write a piece of code for a vulom triggerbox
// Arguments:      ofstream & RdoStrm         -- file output stream
//                 EMrbModuleTag TagIndex     -- index of tag word taken from template file
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a vulom triggerbox.
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
		case TMrbConfig::kModuleInitModule:
			{
			  fCodeTemplates.InitializeCode();
			  fCodeTemplates.Substitute("$moduleName", this->GetName());
			  fCodeTemplates.Substitute("$moduleTitle", this->GetTitle());
			  fCodeTemplates.Substitute("$modulePosition", this->GetPosition());
			  fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			  fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			  fCodeTemplates.Substitute("$baseAddr", (Int_t) this->GetBaseAddr(), 16);
			  fCodeTemplates.Substitute("$channelMask", (Int_t) this->GetEnableMask(), 16);
			  UInt_t scd = 0;
			  Int_t shift = 0;
			  for (Int_t i = 0; i < 8; i++, shift += 4) {
				  if (i >= this->GetNofScalerChans()) break;
				  scd |= (fScaleDown[i] << shift);
			  }
			  fCodeTemplates.Substitute("$scdLower", (Int_t) scd, 16);
			  scd = 0;
			  shift = 0;
			  for (Int_t i = 9; i < 16; i++, shift += 4) {
				  if (i >= this->GetNofScalerChans()) break;
				  scd |= (fScaleDown[i] << shift);
			  }
			  fCodeTemplates.Substitute("$scdUpper", (Int_t) scd, 16);
			  fCodeTemplates.WriteCode(RdoStrm);
			}
			break;
		case TMrbConfig::kModuleClearModule:
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.WriteCode(RdoStrm);
			break;
		case TMrbConfig::kModuleReadModule:
			fCodeTemplates.InitializeCode("%B%");
			fCodeTemplates.WriteCode(RdoStrm);
			if (fScalerSetMask & kScSetInput) {
				fCodeTemplates.InitializeCode("%RDO%");
				fCodeTemplates.Substitute("$moduleName", this->GetName());
				fCodeTemplates.Substitute("$scalerOffset", "VULOM_TBOX_SCALER1");
				fCodeTemplates.Substitute("$nofChannels", this->GetNofScalerChans());
				fCodeTemplates.WriteCode(RdoStrm);
			}
			if (fScalerSetMask & kScSetInhibit) {
				fCodeTemplates.InitializeCode("%RDO%");
				fCodeTemplates.Substitute("$moduleName", this->GetName());
				fCodeTemplates.Substitute("$scalerOffset", "VULOM_TBOX_SCALER2");
				fCodeTemplates.Substitute("$nofChannels", this->GetNofScalerChans());
				fCodeTemplates.WriteCode(RdoStrm);
			}
			if (fScalerSetMask & kScSetScaleDown) {
				fCodeTemplates.InitializeCode("%RDO%");
				fCodeTemplates.Substitute("$moduleName", this->GetName());
				fCodeTemplates.Substitute("$scalerOffset", "VULOM_TBOX_SCALER3");
				fCodeTemplates.Substitute("$nofChannels", this->GetNofScalerChans());
				fCodeTemplates.WriteCode(RdoStrm);
			}
			fCodeTemplates.InitializeCode("%E%");
			fCodeTemplates.Substitute("$rdoScaleDown", this->GetRdoScaleDown());
			fCodeTemplates.WriteCode(RdoStrm);
			break;
		case TMrbConfig::kModuleStartAcquisition:
		case TMrbConfig::kModuleStopAcquisition:
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$rdoScaleDown", this->GetRdoScaleDown());
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.WriteCode(RdoStrm);
			break;
		case TMrbConfig::kModuleFinishReadout:
		case TMrbConfig::kModuleStartAcquisitionGroup:
		case TMrbConfig::kModuleStopAcquisitionGroup:
			break;
		case TMrbConfig::kModuleDefineLocalVarsReadout:
		case TMrbConfig::kModuleUtilities:
		case TMrbConfig::kModuleDefineLocalVarsInit:
		case TMrbConfig::kModuleDefinePrototypes:
		case TMrbConfig::kModuleDefineGlobals:
			fCodeTemplates.InitializeCode();
			fCodeTemplates.WriteCode(RdoStrm);
			break;
	}
	return(kTRUE);
}


Bool_t TMrbVulomTB::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbModuleTag TagIndex,
															TMrbVMEChannel * Channel,
															Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVulomTB::MakeReadoutCode
// Purpose:        Write a piece of code for a vulom triggerbox
// Arguments:      ofstream & RdoStrm           -- file output stream
//                 EMrbModuleTag TagIndex       -- index of tag word taken from template file
//                 TMrbVMEChannel * Channel     -- channel
//                 Int_t Value                  -- value to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a vulom triggerbox.
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
		case TMrbConfig::kModuleWriteSubaddr:
		case TMrbConfig::kModuleSetupReadout:
		case TMrbConfig::kModuleReadChannel:
		case TMrbConfig::kModuleIncrementChannel:
		case TMrbConfig::kModuleSkipChannels:
		case TMrbConfig::kModuleStoreData:
			break;
	}
	return(kTRUE);
}

Bool_t TMrbVulomTB::MakeRcFile(ofstream & RcStrm, TMrbConfig::EMrbRcFileTag TagIndex, const Char_t * ResourceName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVulomTB::MakeRcFile
// Purpose:        Write a piece of code for a vulom triggerbox
// Arguments:      ofstream & RcStrm            -- file output stream
//                 EMrbRcFileTag TagIndex       -- index of tag word taken from template file
//                 Char_t * ResourceName        -- name of resource (first part of entry)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes special, i.e. non-standard code to rc file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	  TString moduleLC = this->GetName();
	  moduleLC(0,1).ToUpper();
	  RcStrm << Form("%s.Module.%s.NofScalerChannels:\t%d\n", ResourceName, moduleLC.Data(), this->GetNofScalerChans());
	  return(kTRUE);
}

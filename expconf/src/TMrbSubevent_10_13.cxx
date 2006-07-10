//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbSubevent_10_13.cxx
// Purpose:        MARaBOU configuration: subevents of type [10,13]
// Description:    Implements class methods to handle [10,13] subevents
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbSubevent_10_13.cxx,v 1.7 2006-07-10 10:49:07 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TDirectory.h"

#include "TMrbTemplate.h"
#include "TMrbLogger.h"
#include "TMrbConfig.h"
#include "TMrbModule.h"
#include "TMrbModuleChannel.h"
#include "TMrbSubevent_10_13.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbSubevent_10_13)

TMrbSubevent_10_13::TMrbSubevent_10_13(const Char_t * SevtName, const Char_t * SevtTitle, Int_t Crate)
																: TMrbSubevent(SevtName, SevtTitle, Crate) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent_10_13
// Purpose:        Create a subevent type [10,13]
// Arguments:      Char_t * SevtName       -- subevent name
//                 Char_t * SevtTitle      -- ... and title
//                 Int_t Crate             -- crate number
// Results:        --
// Exceptions:
// Description:    Create a new subevent of type [10,13] -
//
//                 Data format as given by the producer (MBS):
//                 -  same as subevent [10,1]
//                 -  data will be written together with channel ids
//                 -  empty channels will be SUPPRESSED
//                 -  only ONE MODULE per subevent possible!
//
//                 15------------------------------------0
//                 |           1st active chn            |
//                 |-------------------------------------|
//                 |           data for this chn         |
//                 |-------------------------------------|
//                 |                ...                  |
//                 |                ...                  |
//                 |-------------------------------------|
//                 |           last active chn           |
//                 |-------------------------------------|
//                 |           data for this chn         |
//                 15------------------------------------0
//
//                 Data storage by the consumer (ROOT):
//                 -  data stored in a TClonesArray, channel by channel
//                 -  each channel entry marked with a time stamp
//
//                 31------------------------------------0
//                 |             module serial           |
//                 |-------------------------------------|
//                 |            channel number           |
//                 |-------------------------------------|
//                 |               time stamp            |
//                 |-------------------------------------|
//                 |                [unused]             |
//                 |-------------------------------------|
//                 |             channel data            |
//                 31------------------------------------0
//
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	if (!this->IsZombie()) {
		fSevtDescr = "chn + data, stored in hit buffer";
		fSevtType = 10; 	 			// set subevent type & subtype
		fSevtSubtype = 13;
		if (*SevtTitle == '\0') this->SetTitle(Form("Subevent [%d,%d]: %s", fSevtType, fSevtSubtype, fSevtDescr.Data()));
		fLegalDataTypes = TMrbConfig::kDataUShort;	// only 16 bit words
		gMrbConfig->AddUserClass(TMrbConfig::kIclOptUserClass, "TMrbSubevent_HB");	// we need this base class
		gDirectory->Append(this);
	}
}

Bool_t TMrbSubevent_10_13::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbReadoutTag TagIndex,
															TMrbTemplate & Template,
															const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent_10_13::MakeReadoutCode
// Purpose:        Write a piece of code for subevent [10,13]
// Arguments:      ofstream & RdoStrm           -- file output stream
//                 EMrbReadoutTag TagIndex      -- index of tag word from template file
//                 TMrbTemplate & Template      -- template
//                 Char_t * Prefix              -- prefix to select template code
// Results:
// Exceptions:
// Description:    Writes code needed for subevent handling.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbModuleChannel * param;
	TMrbModule * parentModule, * module;
	Int_t parNo;
	Int_t nextChannel = 0;
	Int_t thisChannel, chDiff;

	switch (TagIndex) {
		case TMrbConfig::kRdoOnTriggerXX:
			parentModule = NULL;
			Template.InitializeCode("%SB%");
			Template.Substitute("$sevtName", this->GetName());
			Template.Substitute("$sevtType", (Int_t) fSevtType);
			Template.Substitute("$sevtSubtype", (Int_t) fSevtSubtype);
			Template.Substitute("$sevtSerial", fSerial);
			Template.Substitute("$crateNo", this->GetCrate());
			Template.WriteCode(RdoStrm);

			param = (TMrbModuleChannel *) fLofParams.First();
			parNo = 0;
			while (param) {
				thisChannel = param->GetAddr();
				if (param->Parent() != parentModule) {
					((TMrbModule *) param->Parent())->MakeReadoutCode(RdoStrm, TMrbConfig::kModuleSetupReadout, param);
					parentModule = (TMrbModule *) param->Parent();
					nextChannel = thisChannel;
				}
				if (parentModule->HasRandomReadout() && !parentModule->HasBlockReadout()) {
					chDiff = (thisChannel - nextChannel);
					if (chDiff != 0) {
						gMrbLog->Err()	<< parentModule->GetName() << " (param " << param->GetName()
										<< "): channels must be contiguous" << endl;
						gMrbLog->Flush(this->ClassName(), "MakeReadoutCode");
						return(kFALSE);
					}
					((TMrbModule *) parentModule)->MakeReadoutCode(RdoStrm, TMrbConfig::kModuleReadChannel, param);

					((TMrbModule *) parentModule)->MakeReadoutCode(RdoStrm, TMrbConfig::kModuleWriteSubaddr, param);
					nextChannel = thisChannel + 1;
					param = (TMrbModuleChannel *) fLofParams.After(param);
					parNo++;
				} else {									// module must be read as a whole
					((TMrbModule *) parentModule)->MakeReadoutCode(RdoStrm, TMrbConfig::kModuleReadModule);
					parNo += parentModule->GetNofChannelsUsed();
					param = (parNo <= fLofParams.GetLast()) ? (TMrbModuleChannel *) fLofParams.At(parNo) : NULL;				
				}
			}

			module = (TMrbModule *) fLofModules.First();
			while (module) {
				module->MakeReadoutCode(RdoStrm, TMrbConfig::kModuleFinishReadout);
				module = (TMrbModule *) fLofModules.After(module);
			}

			Template.InitializeCode("%SE%");
			Template.Substitute("$sevtNameLC", this->GetName());
			Template.WriteCode(RdoStrm);
			break;

		case TMrbConfig::kRdoIgnoreTriggerXX:
			module = (TMrbModule *) fLofModules.First();
			while (module) {
				module->MakeReadoutCode(RdoStrm, TMrbConfig::kModuleClearModule);
				module = (TMrbModule *) fLofModules.After(module);
			}
			break;
	}
	return(kTRUE);
}

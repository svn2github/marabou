//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbSubevent_10_14.cxx
// Purpose:        MARaBOU configuration: subevents of type [10,14]
// Description:    Implements class methods to handle [10,14] subevents
//                 Subevents of type [10,14] store a 0-padded vector of data
//                 without CAMAC ids.
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbSubevent_10_14.cxx,v 1.8 2008-12-10 11:07:18 Rudolf.Lutter Exp $       
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
#include "TMrbConfig.h"
#include "TMrbModule.h"
#include "TMrbModuleChannel.h"
#include "TMrbSubevent_10_14.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;

ClassImp(TMrbSubevent_10_14)

TMrbSubevent_10_14::TMrbSubevent_10_14(const Char_t * SevtName, const Char_t * SevtTitle, Int_t Crate)
																		: TMrbSubevent(SevtName, SevtTitle, Crate) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent_10_14
// Purpose:        Create a subevent type [10,14]
// Arguments:      Char_t * SevtName       -- subevent name
//                 Char_t * SevtTitle      -- ... and title
//                 Int_t Crate             -- crate number
// Results:        --
// Exceptions:
// Description:    Create a new subevent of type [10,14]
//
//                 Data format as given by the producer (MBS):
//                 -  data will be written as fixed-length vectors
//                 -  empty channels will be padded with a zero value
//                 -  data alignment to 32 bits by adding a dummy word
//                    0xffff in case of an odd word count
//                 -  several modules per subevent possible
//                 -  no module id, module sequence has to be fixed
//
//                 15------------------------------------0
//                 |           data of chn 0             |
//                 |-------------------------------------|
//                 |           data of chn 1             |
//                 |-------------------------------------|
//                 |                ...                  |
//                 |                ...                  |
//                 |-------------------------------------|
//                 |           data of chn N             |
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

	if (!this->IsZombie()) {
		fSevtDescr = "zero-padded data, stored in hit buffer";
		fSevtType = 10; 	 			// set subevent type & subtype
		fSevtSubtype = 14;
		if (*SevtTitle == '\0') this->SetTitle(Form("Subevent [%d,%d]: %s", fSevtType, fSevtSubtype, fSevtDescr.Data()));
		fLegalDataTypes = TMrbConfig::kDataUShort | TMrbConfig::kDataUInt; // word length: 16 or 32 bits
		gMrbConfig->AddUserClass(TMrbConfig::kIclOptUserClass, "TMrbSubevent_HB");	// we need this base class
		gDirectory->Append(this);
	}
}

Bool_t TMrbSubevent_10_14::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbReadoutTag TagIndex,
															TMrbTemplate & Template,
															const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent_10_14::MakeReadoutCode
// Purpose:        Write a piece of code for subevent [10,14]
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

	Int_t shortsPerParam = 0;
	Int_t shortsSoFar;

	TIterator * miter;

	switch (TagIndex) {
		case TMrbConfig::kRdoOnTriggerXX:
			Template.InitializeCode("%SB%");
			Template.Substitute("$sevtName", this->GetName());
			Template.Substitute("$sevtType", (Int_t) fSevtType);
			Template.Substitute("$sevtSubtype", (Int_t) fSevtSubtype);
			Template.Substitute("$sevtSerial", fSerial);
			Template.Substitute("$crateNo", this->GetCrate());
			Template.WriteCode(RdoStrm);

			parentModule = NULL;
			param = (TMrbModuleChannel *) fLofParams.First();
			parNo = 0;
			shortsSoFar = 0;
			while (param) {
				thisChannel = param->GetAddr();

				if (param->Parent() != parentModule) {
					parentModule = (TMrbModule *) param->Parent();
					shortsPerParam = parentModule->GetNofShortsPerChannel();
					if (parentModule->GetDataType()->GetIndex() == TMrbConfig::kDataUInt && (shortsSoFar % 2) == 1) {
						Template.InitializeCode("%AL%");
						Template.WriteCode(RdoStrm);
						shortsSoFar++;
					}
					parentModule->MakeReadoutCode(RdoStrm, TMrbConfig::kModuleSetupReadout, param);
					nextChannel = thisChannel;
				}

				if (parentModule->HasRandomReadout() && !parentModule->HasBlockReadout()) {
					chDiff = (thisChannel - nextChannel);
					if (chDiff > 1) {
						((TMrbModule *) parentModule)->MakeReadoutCode(RdoStrm, TMrbConfig::kModuleSkipChannels, param, chDiff);
					} else if (chDiff == 1) {
						((TMrbModule *) parentModule)->MakeReadoutCode(RdoStrm, TMrbConfig::kModuleIncrementChannel, param);
					}
					((TMrbModule *) parentModule)->MakeReadoutCode(RdoStrm, TMrbConfig::kModuleReadChannel, param);

					nextChannel = thisChannel + 1;
					param = (TMrbModuleChannel *) fLofParams.After(param);
					parNo++;
					shortsSoFar += shortsPerParam;
				} else {
					((TMrbModule *) parentModule)->MakeReadoutCode(RdoStrm, TMrbConfig::kModuleReadModule);
					parNo += parentModule->GetNofChannelsUsed();
					shortsSoFar += parentModule->GetNofChannelsUsed() * shortsPerParam;
					param = (parNo <= fLofParams.GetLast()) ? (TMrbModuleChannel *) fLofParams.At(parNo) : NULL;				
				}
			}
			miter = fLofModules.MakeIterator();
			while (module = (TMrbModule *) miter->Next()) module->MakeReadoutCode(RdoStrm, TMrbConfig::kModuleFinishReadout);

			if ((shortsSoFar % 2) == 1) {
				Template.InitializeCode("%AL%");
				Template.WriteCode(RdoStrm);
			}

			Template.InitializeCode("%SE%");
			Template.Substitute("$sevtNameLC", this->GetName());
			Template.WriteCode(RdoStrm);
			break;

		case TMrbConfig::kRdoIgnoreTriggerXX:
			miter = fLofModules.MakeIterator();
			while (module = (TMrbModule *) miter->Next()) module->MakeReadoutCode(RdoStrm, TMrbConfig::kModuleClearModule);
			break;
	}
	return(kTRUE);
}

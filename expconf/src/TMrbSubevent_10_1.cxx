//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbSubevent_10_1.cxx
// Purpose:        MARaBOU configuration: subevents of type [10,1]
// Description:    Implements class methods to handle [10,1] subevents
//                 See manual "GOOSY Buffer Structures" for a description
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbSubevent_10_1.cxx,v 1.4 2004-09-28 13:47:33 rudi Exp $       
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
#include "TMrbSubevent_10_1.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbSubevent_10_1)

TMrbSubevent_10_1::TMrbSubevent_10_1(const Char_t * SevtName, const Char_t * SevtTitle, Int_t Crate)
																: TMrbSubevent(SevtName, SevtTitle, Crate) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent_10_1
// Purpose:        Create a subevent type [10,1]
// Arguments:      Char_t * SevtName       -- subevent name
//                 Char_t * SevtTitle      -- ... and title
//                 Int_t Crate             -- crate number
// Results:        --
// Exceptions:
// Description:    Create a new subevent of type [10,1] -
//                 a standard GSI subevent.
//
//                 Data format as given by the producer (MBS):
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
//                 -  stored in a fixed-length vector, indexed by channel number
//                 -  empty channels padded with a zero value
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	if (!this->IsZombie()) {
		if (*SevtTitle == '\0') SetTitle("CAMAC subevent [10,1]: chn + data");
		fSevtType = 10; 	 			// set subevent type & subtype
		fSevtSubtype = 1;
		fLegalDataTypes = TMrbConfig::kDataUShort;	// only 16 bit words
		gDirectory->Append(this);
	}
}

Bool_t TMrbSubevent_10_1::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbReadoutTag TagIndex,
															TMrbTemplate & Template,
															const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent_10_1::MakeReadoutCode
// Purpose:        Write a piece of code for subevent [10,1]
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
	TString sevtName;

	switch (TagIndex) {
		case TMrbConfig::kRdoOnTriggerXX:
			parentModule = NULL;
			Template.InitializeCode("%SB%");
			Template.Substitute("$sevtNameLC", this->GetName());
			sevtName = this->GetName();
			sevtName(0,1).ToUpper();
			Template.Substitute("$sevtNameUC", sevtName.Data());
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
										<< "): Channels have to be contiguous" << endl;
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
			Template.Substitute("$sevtName", this->GetName());
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

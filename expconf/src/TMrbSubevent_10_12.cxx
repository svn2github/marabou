//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbSubevent_10_12.cxx
// Purpose:        MARaBOU configuration: subevents of type [10,12]
// Description:    Implements class methods to handle [10,12] subevents
//                 Subevents of type [10,12] have variable length data
//                 Data are with channel number
//                 Modules are separated by module headers
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbSubevent_10_12.cxx,v 1.6 2004-09-28 13:47:33 rudi Exp $       
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
#include "TMrbSubevent_10_12.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbSubevent_10_12)

TMrbSubevent_10_12::TMrbSubevent_10_12(const Char_t * SevtName, const Char_t * SevtTitle, Int_t Crate)
																: TMrbSubevent(SevtName, SevtTitle, Crate) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent_10_12
// Purpose:        Create a subevent type [10,12]
// Arguments:      Char_t * SevtName       -- subevent name
//                 Char_t * SevtTitle      -- ... and title
//                 Int_t Crate             -- crate number
// Results:        --
// Exceptions:
// Description:    Create a new subevent of type [10,12]
//
//                 Data format as given by the producer (MBS):
//                 -  several modules per subevent possible
//                 -  channel data 32 bit, arbitrary format
//                    has to be decoded by use of module id & serial
//                 -  2 words module header in front
//
//                 15               8|7                  0
//                 |=================|===================|
//                 |1|   module ID   |   module serial   |
//                 |-------------------------------------|
//                 |      word count including header    |
//                 |-------------------------------------|
//                 |   1st module, 1st active chn, MSW   |
//                 |-------------------------------------|
//                 |         1st active chn, LSW         |
//                 |-------------------------------------|
//                 |                ...                  |
//                 |                ...                  |
//                 |-------------------------------------|
//                 |  1st module, last active chn, MSW   |id &
//                 |-------------------------------------|
//                 |        last active chn, LSW         |
//                 |=====================================|
//                 |1|   module ID   |   module serial   |
//                 |-------------------------------------|
//                 |      word count including header    |
//                 |-------------------------------------|
//                 |                ...                  |
//                 |                ...                  |
//                 |-------------------------------------|
//                 |  last module, last active chn, MSW  |
//                 |-------------------------------------|
//                 |        last active chn, LSW         |
//                 15====================================0
//
//                 Data storage by the consumer (ROOT):
//                 -  stored in a fixed-length vector, indexed by channel number
//                 -  empty channels padded with reset (default 0) value
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	if (!this->IsZombie()) {
		if (*SevtTitle == '\0') SetTitle("CAMAC subevent [10,12]: hdr + chn + data");
		fSevtType = 10; 	 						// set subevent type & subtype
		fSevtSubtype = 12;
		fLegalDataTypes = TMrbConfig::kDataUShort;	// only 16 bit words
		gDirectory->Append(this);
	}
}

Bool_t TMrbSubevent_10_12::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbReadoutTag TagIndex,
															TMrbTemplate & Template,
															const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent_10_12::MakeReadoutCode
// Purpose:        Write a piece of code for subevent [10,12]
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
	TString moduleNameUC;

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
					if (parentModule != NULL) {
						Template.InitializeCode("%SME%");
						Template.Substitute("$moduleNameLC", parentModule->GetName());
						Template.Substitute("$moduleSerial", parentModule->GetSerial());
						Template.WriteCode(RdoStrm);
					}
					parentModule = (TMrbModule *) param->Parent();

					TString mnemoLC = parentModule->GetMnemonic();
					TString mnemoUC = mnemoLC;
					mnemoUC.ToUpper();

					Template.InitializeCode("%SMB%");
					moduleNameUC = parentModule->GetName();
					moduleNameUC(0,1).ToUpper();
					Template.Substitute("$moduleNameLC", parentModule->GetName());
					Template.Substitute("$moduleNameUC", moduleNameUC);
					Template.Substitute("$moduleType", mnemoUC);
					Template.Substitute("$moduleSerial", parentModule->GetSerial());
					Template.WriteCode(RdoStrm);

					parentModule->MakeReadoutCode(RdoStrm, TMrbConfig::kModuleSetupReadout, param);
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
			Template.InitializeCode("%SME%");
			Template.Substitute("$moduleNameLC", parentModule->GetName());
			Template.Substitute("$moduleSerial", parentModule->GetSerial());
			Template.WriteCode(RdoStrm);

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

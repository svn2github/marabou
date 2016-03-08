//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbSubevent_Sis_3302_3.cxx
// Purpose:        MARaBOU configuration: subevents of type [10,57] - SIS data
// Description:    Implements class methods to handle [10,57] subevents
//                 reflecting data structure of SIS 3XXX modules
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbSubevent_Sis_3302_3.cxx,v 1.4 2011-12-15 16:33:23 Marabou Exp $       
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
#include "TMrbSubevent_Sis_3302_3.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbSubevent_Sis_3302_3)

TMrbSubevent_Sis_3302_3::TMrbSubevent_Sis_3302_3(const Char_t * SevtName, const Char_t * SevtTitle, Int_t Crate)
																: TMrbSubevent(SevtName, SevtTitle, Crate) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent_Sis_3302_3
// Purpose:        Create a subevent type [10,57]
// Arguments:      Char_t * SevtName       -- subevent name
//                 Char_t * SevtTitle      -- ... and title
//                 Int_t Crate             -- crate number
// Results:        --
// Exceptions:
// Description:    Create a new subevent of type [10,57]
//                 used to store SIS list-mode data
//
//                 Data format as given by the producer (MBS) - same as [10,57]:
//
//                 31              16|15                 0
//                 |=================|===================|
//                 |    word count   |   module serial   |
//                 |=====================================|
//                 |   tstamp high   |   channel number  |
//                 |-------------------------------------|
//                 |          time stamp low             |
//                 |-------------------------------------|
//                 | raw data lngth  | energy data lngth | ... [0,0] normally
//                 |-------------------------------------|
//                 |     raw data    |  (traces, 16bit)  |
//                 |        ...      |        ...        |
//                 |-------------------------------------|
//                 |            energy data              |
//                 |              (32bit)                |
//                 |-------------------------------------|
//                 |            max energy               |
//                 |-------------------------------------|
//                 |            min energy               |
//                 |-------------------------------------|
//                 |           status bits               |
//                 |-------------------------------------|
//                 |       trailer (0xdeadbeef)          |
//                 |=====================================|
//                 |   tstamp high   |   channel number  |
//                 |-------------------------------------|
//                 |          time stamp low             |
//                 |-------------------------------------|
//                 |                ~~~                  |
//                 |                ~~~                  |
//                 32====================================0
//
//                 Data storage by the consumer (ROOT):
//                 -  data stored in a TClonesArray/TUsrHit, channel by channel
//                 -  each channel entry marked with event time
//
//                 31--------------------------------------0
//                 |             buffer number             |
//                 |---------------------------------------|
//                 |      event number within buffer       |
//                 |---------------------------------------|
//                 |             module number             |
//                 |---------------------------------------|
//                 |            channel number             |
//                 |---------------------------------------|
//                 |          0        |  time st (32..47) |
//                 |---------------------------------------|
//                 |           time stamp (0..31)          |
//                 |---------------------------------------|
//                 |             word count (=2)           |
//                 |---------------------------------------|
//                 |                    0                  |
//                 |---------------------------------------|
//                 |              max energy               |
//                 |---------------------------------------|
//
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->IsZombie()) {
		fSevtDescr = "SIS3302 data, multi-channel, multi-event";
		fSevtType = 10; 	 						// set subevent type & subtype
		fSevtSubtype = 57;
		if (*SevtTitle == '\0') this->SetTitle(Form("Subevent [%d,%d]: %s", fSevtType, fSevtSubtype, fSevtDescr.Data()));
		fLegalDataTypes = TMrbConfig::kDataULong;
		//gDirectory->Append(this);
	}
}

Bool_t TMrbSubevent_Sis_3302_3::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbReadoutTag TagIndex,
															TMrbTemplate & Template,
															const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent_Sis_3302_3::MakeReadoutCode
// Purpose:        Write a piece of code for subevent [10,57]
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
	Int_t thisChannel;
	TString sevtName;
	TString moduleNameUC;
	TIterator * miter;

	switch (TagIndex) {
		case TMrbConfig::kRdoOnTriggerXX:
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

			parentModule = NULL;
			param = (TMrbModuleChannel *) fLofParams.First();
			parNo = 0;
			while (param) {
				thisChannel = param->GetAddr();

				if (param->Parent() != parentModule) {
					parentModule = (TMrbModule *) param->Parent();
					parentModule->MakeReadoutCode(RdoStrm, TMrbConfig::kModuleSetupReadout, param);
					nextChannel = thisChannel;
				}

				((TMrbModule *) parentModule)->MakeReadoutCode(RdoStrm, TMrbConfig::kModuleReadModule);
				parNo += parentModule->GetNofChannelsUsed();
				param = (parNo <= fLofParams.GetLast()) ? (TMrbModuleChannel *) fLofParams.At(parNo) : NULL;				
			}
			miter = fLofModules.MakeIterator();
			while (module = (TMrbModule *) miter->Next()) module->MakeReadoutCode(RdoStrm, TMrbConfig::kModuleFinishReadout);

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

const Char_t * TMrbSubevent_Sis_3302_3::GetCommonCodeFile() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent_Sis_3302_3::GetCommonCodeFile
// Purpose:        Specify common code to be loaded
// Arguments:      --
// Results:        Char_t * CommonCodeFile   -- file containing common code
// Exceptions:
// Description:    Returns name of common code file to be used.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fXhit) {
		fInheritsFrom = "TMrbSubevent_Sis3302_Xhit";
		fCommonCodeFile = "Subevent_Sis3302_Xhit";
	} else {
		fInheritsFrom = "TMrbSubevent_Sis3302";
		fCommonCodeFile = "Subevent_Sis3302_Common";
	}
	gMrbConfig->AddUserClass(TMrbConfig::kIclOptUserClass, fInheritsFrom);
	return(fCommonCodeFile.Data());
}

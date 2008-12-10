//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbSubevent_Sis_2.cxx
// Purpose:        MARaBOU configuration: subevents of type [10,52] - SIS data
// Description:    Implements class methods to handle [10,52] subevents
//                 reflecting data structure of XIA DGF-4C modules
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbSubevent_Sis_2.cxx,v 1.8 2008-12-10 11:07:18 Rudolf.Lutter Exp $       
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
#include "TMrbSubevent_Sis_2.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbSubevent_Sis_2)

TMrbSubevent_Sis_2::TMrbSubevent_Sis_2(const Char_t * SevtName, const Char_t * SevtTitle, Int_t Crate)
																: TMrbSubevent(SevtName, SevtTitle, Crate) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent_Sis_2
// Purpose:        Create a subevent type [10,52]
// Arguments:      Char_t * SevtName       -- subevent name
//                 Char_t * SevtTitle      -- ... and title
//                 Int_t Crate             -- crate number
// Results:        --
// Exceptions:
// Description:    Create a new subevent of type [10,52]
//                 used to store SIS data in DGF-4C list-mode format
//
//
//                 Data format as given by the producer (MBS) - same as [10,52]:
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
//                 -  data stored in a TClonesArray, channel by channel
//                 -  each channel entry marked with event time
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->IsZombie()) {
		fSevtDescr = "SIS data, multi-module, multi-event, stored in hit buffer";
		fSevtType = 10; 	 							// set subevent type & subtype
		fSevtSubtype = 52;
		if (*SevtTitle == '\0') this->SetTitle(Form("Subevent [%d,%d]: %s", fSevtType, fSevtSubtype, fSevtDescr.Data()));
		fLegalDataTypes = TMrbConfig::kDataUShort;		// only 16 bit words
		gMrbConfig->AddUserClass(TMrbConfig::kIclOptUserClass, "TMrbSubevent_Sis");	// we need this base class
		gDirectory->Append(this);
	}
}

Bool_t TMrbSubevent_Sis_2::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbReadoutTag TagIndex,
															TMrbTemplate & Template,
															const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent_Sis_2::MakeReadoutCode
// Purpose:        Write a piece of code for subevent [10,52]
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
	TIterator * miter;

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
					mnemoUC(0,1).ToUpper();

					Template.InitializeCode("%SMB%");
					moduleNameUC = parentModule->GetName();
					moduleNameUC(0,1).ToUpper();
					Template.Substitute("$lng", 16);
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
			Template.Substitute("$lng", 16);
			Template.Substitute("$moduleNameLC", parentModule->GetName());
			Template.Substitute("$moduleSerial", parentModule->GetSerial());
			Template.WriteCode(RdoStrm);

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

//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbSubevent_Caen_2.cxx
// Purpose:        MARaBOU configuration: subevents of type [10,42] - CAEN data
// Description:    Implements class methods to handle [10,42] subevents
//                 reflecting data structure of XIA DGF-4C modules
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbSubevent_Caen_2.cxx,v 1.12 2008-12-10 11:07:18 Rudolf.Lutter Exp $       
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
#include "TMrbSubevent_Caen_2.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;

ClassImp(TMrbSubevent_Caen_2)

TMrbSubevent_Caen_2::TMrbSubevent_Caen_2(const Char_t * SevtName, const Char_t * SevtTitle, Int_t Crate)
																: TMrbSubevent(SevtName, SevtTitle, Crate) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent_Caen_2
// Purpose:        Create a subevent type [10,42]
// Arguments:      Char_t * SevtName       -- subevent name
//                 Char_t * SevtTitle      -- ... and title
//                 Int_t Crate             -- crate number
// Results:        --
// Exceptions:
// Description:    Create a new subevent of type [10,42]
//                 used to store CAEN data DGF-4C list-mode format
//
//
//                 Data format as given by the producer (MBS):
//                 -  up to MAXEVENTS events per module
//                 -  several modules per buffer
//
//                 31---------------16|15------8|7---------0
//                 |  0x200  |        |    wc   | modser#  | header
//                 |==================|====================|
//                 |  0x000  | chan   |        data        | channel data
//                 |------------------|--------------------|
//                 |         |        |        ...         |
//                 |==================|====================|
//                 |  0x400  |         event count         | trailer
//                 31======================================0
//
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
//                 |                  data                 |
//                 |---------------------------------------|
//
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->IsZombie()) {
		fSevtDescr = "CAEN data, multi-module, multi-event, stored in hit buffer";
		fSevtType = 10; 	 							// set subevent type & subtype
		fSevtSubtype = 42;
		if (*SevtTitle == '\0') this->SetTitle(Form("Subevent [%d,%d]: %s", fSevtType, fSevtSubtype, fSevtDescr.Data()));
		fLegalDataTypes = TMrbConfig::kDataUShort;		// only 16 bit words
		gMrbConfig->AddUserClass(TMrbConfig::kIclOptUserClass, "TMrbSubevent_Caen");	// we need this base class
		gDirectory->Append(this);
	}
}

Bool_t TMrbSubevent_Caen_2::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbReadoutTag TagIndex,
															TMrbTemplate & Template,
															const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent_Caen_2::MakeReadoutCode
// Purpose:        Write a piece of code for subevent [10,42]
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
	TString sevtName;
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
				if (param->Parent() != parentModule) {
					parentModule = (TMrbModule *) param->Parent();
					parentModule->MakeReadoutCode(RdoStrm, TMrbConfig::kModuleSetupReadout, param);
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

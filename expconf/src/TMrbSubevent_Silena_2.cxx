//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbSubevent_Silena_2.cxx
// Purpose:        MARaBOU configuration: subevents of type [10,32] - Silena data
// Description:    Implements class methods to handle [10,32] subevents
//                 Special format to be used for Silena 4418 ADCs running in
//                 zero-compression mode.
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <iostream.h>
#include <strstream.h>
#include <iomanip.h>
#include <fstream.h>

#include "TDirectory.h"

#include "TMrbTemplate.h"
#include "TMrbConfig.h"
#include "TMrbModule.h"
#include "TMrbModuleChannel.h"
#include "TMrbSubevent_Silena_2.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;

ClassImp(TMrbSubevent_Silena_2)

TMrbSubevent_Silena_2::TMrbSubevent_Silena_2(const Char_t * SevtName, const Char_t * SevtTitle, Int_t Crate)
																: TMrbSubevent(SevtName, SevtTitle, Crate) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent_Silena_2
// Purpose:        Create a subevent type [10,32]
// Arguments:      Char_t * SevtName       -- subevent name
//                 Char_t * SevtTitle      -- ... and title
//                 Int_t Crate             -- crate number
// Results:        --
// Exceptions:
// Description:    Create a new subevent of type [10,32]
//                 used to store Silena list-mode data (zero-compression mode)
//
//                 Data format as given by the producer (MBS):
//                 (for details see SILENA manual)
//                 -  header word containing module number and word count
//                 -  hit pattern reflecting active channels
//                 -  1 data word per active channel, may contain channel id
//                    in front (depending on status settings)
//                 -  several modules per buffer
//
//                 15               8|7                  0
//                 |=================|===================| module header
//                 |1|      |   wc   |   module number   |
//                 |-------------------------------------|
//                 |0|               |   hit pattern     |
//                 |-------------------------------------| data section
//                 |OVF|    SUB | 1st data word (12 bit) |
//                 |-------------------------------------|
//                 |                ~~~                  |
//                 |-------------------------------------|
//                 |OVF|    SUB |    last data word      |
//                 15====================================0
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
		if (*SevtTitle == '\0') SetTitle("CAMAC subevent [10,32]: Silena 4418 data with zero suppression");
		fSevtType = 10; 	 						// set subevent type & subtype
		fSevtSubtype = 32;
		fLegalDataTypes = TMrbConfig::kDataUShort;	// only 16 bit words
		gMrbConfig->AddUserClass("TMrbSubevent_Silena");	// we need this base class
		gDirectory->Append(this);
	}
}

Bool_t TMrbSubevent_Silena_2::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbReadoutTag TagIndex,
															TMrbTemplate & Template,
															const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent_Silena_2::MakeReadoutCode
// Purpose:        Write a piece of code for subevent [10,32]
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

//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbSubevent_Data_S.cxx
// Purpose:        MARaBOU configuration: subevents of type [10,91]
// Description:    Implements class methods to handle [10,91] subevents
//                 See manual "GOOSY Buffer Structures" for a description
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
#include "TMrbLogger.h"
#include "TMrbConfig.h"
#include "TMrbModule.h"
#include "TMrbModuleChannel.h"
#include "TMrbSubevent_Data_S.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbSubevent_Data_S)

TMrbSubevent_Data_S::TMrbSubevent_Data_S(const Char_t * SevtName, const Char_t * SevtTitle, Int_t NofWords, Int_t Crate)
																: TMrbSubevent(SevtName, SevtTitle, Crate) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent_Data_S
// Purpose:        Create a subevent type [10,91]
// Arguments:      Char_t * SevtName       -- subevent name
//                 Char_t * SevtTitle      -- ... and title
//                 Int_t NofWords          -- number of data words (16 bit)
//                 Int_t Crate             -- crate number
// Results:        --
// Exceptions:
// Description:    Create a new subevent of type [10,91] -
//                 plain integer data (16 bit).
//
//                 Data format as given by the producer (MBS):
//
//                 15------------------------------------0
//                 |               data word 1           |
//                 |-------------------------------------|
//                 |                ...                  |
//                 |                ...                  |
//                 |-------------------------------------|
//                 |              data word N            |
//                 15------------------------------------0
//
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	if (!this->IsZombie()) {
		if (*SevtTitle == '\0') SetTitle("CAMAC subevent [10,91]: chn + data");
		fSevtType = 10; 	 			// set subevent type & subtype
		fSevtSubtype = 64;
		fLegalDataTypes = TMrbConfig::kDataUShort;	// only 16 bit words
		fNofWords = NofWords;
		gDirectory->Append(this);
	}
}

Bool_t TMrbSubevent_Data_S::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbReadoutTag TagIndex,
															TMrbTemplate & Template,
															const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent_Data_S::MakeReadoutCode
// Purpose:        Write a piece of code for subevent [10,91]
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
	Int_t parNo;
	Int_t nextChannel = 0;
	Int_t thisChannel, chDiff;
	TString sevtName;

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

			param = (TMrbModuleChannel *) fLofParams.First();
			parNo = 0;
			while (param) {
				thisChannel = param->GetAddr();
				chDiff = (thisChannel - nextChannel);
				if (chDiff != 0) {
					gMrbLog->Err()	<< "Params must be contiguous" << endl;
					gMrbLog->Flush(this->ClassName(), "MakeReadoutCode");
					return(kFALSE);
				}
				Template.InitializeCode();
				Template.Substitute("$sevtName", this->GetName());
				Template.Substitute("$sevtType", (Int_t) fSevtType);
				Template.Substitute("$sevtSubtype", (Int_t) fSevtSubtype);
				Template.Substitute("$sevtSerial", fSerial);
				Template.Substitute("$crateNo", this->GetCrate());
				Template.Substitute("$parName", param->GetName());
				Template.Substitute("$parNo", parNo);
				Template.WriteCode(RdoStrm);
				nextChannel = thisChannel + 1;
				param = (TMrbModuleChannel *) fLofParams.After(param);
				parNo++;
			}
			break;

		case TMrbConfig::kRdoIgnoreTriggerXX:
			break;
	}
	return(kTRUE);
}

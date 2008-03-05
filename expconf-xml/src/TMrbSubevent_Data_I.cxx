//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbSubevent_Data_I.cxx
// Purpose:        MARaBOU configuration: subevents of type [10,65]
// Description:    Implements class methods to handle [10,65] subevents
//                 See manual "GOOSY Buffer Structures" for a description
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbSubevent_Data_I.cxx,v 1.1 2008-03-05 12:37:17 Rudolf.Lutter Exp $       
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
#include "TMrbSubevent_Data_I.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbSubevent_Data_I)

TMrbSubevent_Data_I::TMrbSubevent_Data_I(const Char_t * SevtName, const Char_t * SevtTitle, Int_t NofWords, Int_t Crate)
																: TMrbSubevent(SevtName, SevtTitle, Crate) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent_Data_I
// Purpose:        Create a subevent type [10,65]
// Arguments:      Char_t * SevtName       -- subevent name
//                 Char_t * SevtTitle      -- ... and title
//                 Int_t NofWords          -- number of data words (16 bit)
//                 Int_t Crate             -- crate number
// Results:        --
// Exceptions:
// Description:    Create a new subevent of type [10,65] -
//                 plain integer data (32 bit).
//
//                 Data format as given by the producer (MBS):
//
//                 31------------------------------------0
//                 |               data word 1           |
//                 |-------------------------------------|
//                 |                ...                  |
//                 |                ...                  |
//                 |-------------------------------------|
//                 |              data word N            |
//                 31------------------------------------0
//
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	if (!this->IsZombie()) {
		fSevtDescr = "UInt_t (32 bit) data";
		fSevtType = 10; 	 			// set subevent type & subtype
		fSevtSubtype = 65;
		if (*SevtTitle == '\0') this->SetTitle(Form("Subevent [%d,%d]: %s", fSevtType, fSevtSubtype, fSevtDescr.Data()));
		fLegalDataTypes = TMrbConfig::kDataUInt;	// only 32 bit words
		fNofWords = NofWords;
		gDirectory->Append(this);
	}
}

Bool_t TMrbSubevent_Data_I::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbReadoutTag TagIndex,
															TMrbTemplate & Template,
															const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent_Data_I::MakeReadoutCode
// Purpose:        Write a piece of code for subevent [10,65]
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
					return(kFALSE);
				}
				Template.InitializeCode();
				Template.Substitute("$sevtName", this->GetName());
				Template.Substitute("$sevtType", (Int_t) fSevtType);
				Template.Substitute("$sevtSubtype", (Int_t) fSevtSubtype);
				Template.Substitute("$sevtSerial", fSerial);
				Template.Substitute("$crateNo", this->GetCrate());
				Template.WriteCode(RdoStrm);
				nextChannel = thisChannel + 1;
				param = (TMrbModuleChannel *) fLofParams.After(param);
				parNo++;
			}

			Template.InitializeCode("%SE%");
			Template.Substitute("$sevtNameLC", this->GetName());
			Template.WriteCode(RdoStrm);
			break;

		case TMrbConfig::kRdoIgnoreTriggerXX:
			break;
	}
	return(kTRUE);
}

//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbSubeventRaw.cxx
// Purpose:        MARaBOU configuration: user-defined subevents
// Description:    Implements class methods to handle user-defined subevents
//                 See manual "GOOSY Buffer Structures" for a description
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
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
#include "TMrbSubeventRaw.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;

ClassImp(TMrbSubeventRaw)

TMrbSubeventRaw::TMrbSubeventRaw(const Char_t * SevtName, const Char_t * SevtTitle,
										UInt_t Type, UInt_t Subtype, Option_t * LegalDataTypes,
										Bool_t CreateSerial) : TMrbSubevent(SevtName, SevtTitle, 0) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubeventRaw
// Purpose:        Create a used-defined subevent
// Arguments:      Char_t * SevtName         -- subevent name
//                 Char_t * SevtTitle        -- ... and title
//                 UInt_t Type               -- subevent type
//                 UInt_t Subtype            -- ... subtype
//                 Option_t * LegalDataTypes -- data types
//                                              (a string out of CSILF, separated by ":")
//                 Bool_t CreateSerial       -- kTRUE if internal serial number
// Results:        --
// Exceptions:
// Description:    Create a used-defined subevent
//                 Data are expected to be in RAW mode.
//                 Data conversion is up to the user program.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t dty;

	if (!this->IsZombie()) {
		if (*SevtTitle == '\0') SetTitle("User-defined subevent");
		fSevtType = Type; 	 			// set subevent type & subtype
		fSevtSubtype = Subtype;
		dty = gMrbConfig->GetLofDataTypes()->CheckPattern(this->ClassName(), "CTOR", LegalDataTypes);
		if (dty == TMrbLofNamedX::kIllIndexBit) {
			this->MakeZombie();
		} else {
			fLegalDataTypes = dty;
			fSerialToBeCreated = CreateSerial;
			if (!CreateSerial) fSerial = (Subtype << 16) | Type;	// no internal serial: replaced by type
			gDirectory->Append(this);
		}
	}
}

Bool_t TMrbSubeventRaw::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbReadoutTag TagIndex,
															TMrbTemplate & Template,
															const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubeventRaw::MakeReadoutCode
// Purpose:        Write a piece of code for a user-defined subevent
// Arguments:      ofstream & RdoStrm           -- file output stream
//                 EMrbReadoutTag TagIndex      -- index of tag word from template file
//                 TMrbTemplate & Template      -- template
//                 Char_t * Prefix              -- prefix to select template code
// Results:
// Exceptions:
// Description:    Writes code needed for subevent handling.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	return(kTRUE);			// not yet implemented
}

//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbModuleRaw.cxx
// Purpose:        MARaBOU configuration: User-defined Modules
// Description:    Implements class methods to handle Silena type 4418V
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

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbLogger.h"
#include "TMrbConfig.h"
#include "TMrbModuleChannel.h"
#include "TMrbModuleRegister.h"
#include "TMrbModuleRaw.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbModuleRaw)

TMrbModuleRaw::TMrbModuleRaw(const Char_t * ModuleName, const Char_t * ModuleID, Int_t NofChannels, Int_t Range, Option_t * DataType) :
									TMrbModule(ModuleName, "UserDefined/RAW", NofChannels, Range) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModuleRaw
// Purpose:        Implement a user-defined module
// Arguments:      Char_t * ModuleName      -- name of camac module
//                 Char_t * ModuleID        -- module id
//                 Int_t NofChannels        -- number of channels
//                 Int_t Range              -- range
//                 Option_t * DataType      -- data type
// Results:        --
// Exceptions:
// Description:    
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString codeFile;
	UInt_t mTypeBits;
	TString mType;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	if (!this->IsZombie()) {
		if (gMrbConfig == NULL) {
			gMrbLog->Err() << "No config defined" << endl;
			gMrbLog->Flush(this->ClassName());
			this->MakeZombie();
		} else if (gMrbConfig->FindModule(ModuleName)) {
			gMrbLog->Err() << ModuleName << ": Module name already in use" << endl;
			gMrbLog->Flush(this->ClassName());
			this->MakeZombie();
		} else {
			SetTitle(ModuleID); 		 	// store module type
			mTypeBits = TMrbConfig::kModuleRaw | TMrbConfig::kModuleListMode;
			mType = gMrbConfig->GetLofModuleTypes()->Pattern2String(mType, mTypeBits);
			fModuleType.Set(mTypeBits, mType.Data());
			fDataType = gMrbConfig->GetLofDataTypes()->FindByName(DataType);
			if (fDataType == NULL) {
				gMrbLog->Err() << ModuleName << ": Illegal data type - " << DataType << endl;
				gMrbLog->Flush(this->ClassName());
				this->MakeZombie();
			} else {
				fNofShortsPerChannel = (fDataType->GetIndex() == TMrbConfig::kDataUInt) ? 2 : 1;
				fCrate = 0;

				for (Int_t nch = 0; nch < NofChannels; nch++) { 		// create array of params
					fChannelSpec.Add((TObject *) new TMrbModuleChannel(this, nch));
				}

				DefineRegisters();
				gMrbConfig->AddModule(this);				// append to list of modules
				gDirectory->Append(this);
			}
		}
	}
}

Bool_t TMrbModuleRaw::MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModuleRaw::MakeReadoutCode
// Purpose:        Write a piece of readout code
// Arguments:      ofstream & RdoStrm         -- file output stream
//                 EMrbModuleTag TagIndex     -- index of tag word taken from template file
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes readout code for a user-defined module.
//                 Not yet implemented.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	return(kTRUE);
}


Bool_t TMrbModuleRaw::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbModuleTag TagIndex,
															TObject * Channel,
															Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModuleRaw::MakeReadoutCode
// Purpose:        Write a piece of readout code
// Arguments:      ofstream & RdoStrm           -- file output stream
//                 EMrbModuleTag TagIndex       -- index of tag word taken from template file
//                 TObject * Channel            -- channel
//                 Int_t Value                  -- value to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes readout code for a user-defined module.
//                 Not yet implemented.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	return(kTRUE);
}

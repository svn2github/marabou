//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbModuleChannel.cxx
// Purpose:        MARaBOU configuration: single channel
// Description:    Implements class methods to describe a single channel
//                 of a CAMAC/VME module
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbModuleChannel.cxx,v 1.4 2004-09-28 13:47:32 rudi Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbLogger.h"
#include "TMrbConfig.h"
#include "TMrbModuleChannel.h"
#include "TMrbModuleRegister.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbModuleChannel)

TMrbModuleChannel::TMrbModuleChannel(TMrbModule * Module, Int_t Addr) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModuleChannel
// Purpose:        Create a single camac channel
// Arguments:      TMrbModule * Module        -- addr of parent module
//                 Int_t Addr                 -- address / channel number
// Results:        --
// Exceptions:
// Description:    Defines a single channel
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	fAddr = Addr; 					// current address / channel number
	fParent = Module;				// parent addr
	fArrayHead = NULL;				// not indexed
	fIndexRange = 1;				// single channel
	fStatus = TMrbConfig::kChannelSingle;	//...
	fIsUsed = kFALSE;				// not used
	fUsedBy = NULL;
}

Bool_t TMrbModuleChannel::Set(const Char_t * RegName, Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModuleChannel::Set
// Purpose:        Set register values for single channel
// Arguments:      Char_t * RegName  -- register name
//                 Int_t Value       -- register value
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets register values for given module channel
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	TMrbModuleRegister * rp;

	if ((kp = fParent->FindRegister(RegName)) != NULL) {
		rp = (TMrbModuleRegister *) kp->GetAssignedObject();
		return(rp->Set(fAddr, Value));
	} else {				
		gMrbLog->Err()	<< fParent->GetName() << ":" << this->GetName() << "(" << fAddr
						<< "): No such register - " << RegName << endl;
		gMrbLog->Flush(this->ClassName(), "Set");
		return(kFALSE);
	}
}

Bool_t TMrbModuleChannel::Set(Int_t RegIndex, Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModuleChannel::Set
// Purpose:        Set register values for single channel
// Arguments:      Int_t RegIndex       -- register index
//                 Int_t Value          -- register value
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets register values for a given module channel
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	TMrbModuleRegister * rp;

	if ((kp = fParent->FindRegister(RegIndex)) != NULL) {
		rp = (TMrbModuleRegister *) kp->GetAssignedObject();
		return(rp->Set(fAddr, Value));
	} else {				
		gMrbLog->Err()	<< fParent->GetName() << ":" << this->GetName() << "(" << fAddr
						<< "): No such register index - " << RegIndex << endl;
		gMrbLog->Flush(this->ClassName(), "Set");
		return(kFALSE);
	}
}

Bool_t TMrbModuleChannel::Set(const Char_t * RegName, const Char_t * Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModuleChannel::Set
// Purpose:        Set register values for single channel
// Arguments:      Char_t * RegName  -- register name
//                 Char_t * Value    -- register value (mnemonic)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets register values for given module channel
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	TMrbModuleRegister * rp;

	if ((kp = fParent->FindRegister(RegName)) != NULL) {
		rp = (TMrbModuleRegister *) kp->GetAssignedObject();
		return(rp->Set(fAddr, Value));
	} else {				
		gMrbLog->Err()	<< fParent->GetName() << ":" << this->GetName() << "(" << fAddr
						<< "): No such register - " << RegName << endl;
		gMrbLog->Flush(this->ClassName(), "Set");
		return(kFALSE);
	}
}

Bool_t TMrbModuleChannel::Set(Int_t RegIndex, const Char_t * Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModuleChannel::Set
// Purpose:        Set register values for single channel
// Arguments:      Int_t RegIndex       -- register index
//                 Char_t * Value       -- register value (mnemonic)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets register values for a given module channel
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	TMrbModuleRegister * rp;

	if ((kp = fParent->FindRegister(RegIndex)) != NULL) {
		rp = (TMrbModuleRegister *) kp->GetAssignedObject();
		return(rp->Set(fAddr, Value));
	} else {				
		gMrbLog->Err()	<< fParent->GetName() << ":" << this->GetName() << "(" << fAddr
						<< "): No such register index - " << RegIndex << endl;
		gMrbLog->Flush(this->ClassName(), "Set");
		return(kFALSE);
	}
}

Int_t TMrbModuleChannel::Get(const Char_t * RegName) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModuleChannel::Get
// Purpose:        Get register values of single channel
// Arguments:      Char_t * RegName  -- register name
// Results:        Int_t Value       -- register value
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Gets register values of a single module channel
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	TMrbModuleRegister * rp;

	if ((kp = fParent->FindRegister(RegName)) != NULL) {
		rp = (TMrbModuleRegister *) kp->GetAssignedObject();
		return(rp->Get(fAddr));
	} else {				
		gMrbLog->Err()	<< fParent->GetName() << ":" << this->GetName() << "(" << fAddr
						<< "): No such register - " << RegName << endl;
		gMrbLog->Flush(this->ClassName(), "Get");
		return(-1);
	}
}

Int_t TMrbModuleChannel::Get(Int_t RegIndex) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModuleChannel::Get
// Purpose:        Get register values for single channel
// Arguments:      Int_t RegIndex     -- register index
// Results:        Int_t Value        -- register value
// Exceptions:
// Description:    Gets register values of a given module channel
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	TMrbModuleRegister * rp;

	if ((kp = fParent->FindRegister(RegIndex)) != NULL) {
		rp = (TMrbModuleRegister *) kp->GetAssignedObject();
		return(rp->Get(fAddr));
	} else {					
		gMrbLog->Err() << fParent->GetName()
				<< ":" << this->GetName() << "(" << fAddr
				<< "): No such register index - " << RegIndex << endl;
		gMrbLog->Flush(this->ClassName(), "Get");
		return(-1);
	}
}

Int_t TMrbModuleChannel::GetIndex() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModuleChannel::GetIndex
// Purpose:        Get index with respect to array head
// Arguments:      
// Results:        Int_t Value    -- index
// Exceptions:
// Description:    Calculates offset from array head.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	switch (fStatus) {

		case TMrbConfig::kChannelArrElem:	return(fAddr - fArrayHead->GetAddr());

		case TMrbConfig::kChannelSingle:
		case TMrbConfig::kChannelArray:
		default:							return(0);
	}
}

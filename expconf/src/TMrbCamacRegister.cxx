//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbCamacRegister.cxx
// Purpose:        MARaBOU configuration: module registers
// Description:    Implements class methods to describe a register
//                 of a CAMAC module
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbCamacRegister.cxx,v 1.4 2004-09-28 13:47:32 rudi Exp $       
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
#include "TMrbCamacRegister.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbCamacRegister)

TMrbCamacRegister::TMrbCamacRegister(TMrbCamacModule * Module, Int_t NofChannels, TMrbNamedX * RegDef,
								const Char_t * CnafSet, const Char_t * CnafClear, const Char_t * CnafGet,
								Int_t InitValue, Int_t LowerLimit, Int_t UpperLimit,
								TMrbLofNamedX * BitNames, Bool_t PatternMode) :
																TMrbModuleRegister(Module, NofChannels,
																			RegDef, InitValue, LowerLimit, UpperLimit,
																			BitNames, PatternMode) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCamacRegister
// Purpose:        Create a camac register
// Arguments:      TMrbCamacModule * Module       -- addr of parent module
//                 Int_t NofChannels              -- number of channels (0 = register is common)
//                 TMrbNamedX * RegDef            -- register index
//                 Char_t * CnafSet               -- Axx.Fyy to set bits
//                 Char_t * CnafClear             -- Axx.Fyy to set bits
//                 Char_t * CnafGet               -- Axx.Fyy to get contents
//                 Int_t InitValue                -- initial value
//                 Int_t LowerLimit               -- lower limit
//                 Int_t UpperLimit               -- upper limit
//                 TMrbLofNamedX * BitNames       -- list of possible bit values
//                 Bool_t PatternMode             -- kTRUE if bitwise values
// Results:        --
// Exceptions:
// Description:    Defines a camac register.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t subAddr;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	fHasCnafs = 0;
	subAddr = -1;
	if (!this->IsZombie()) {
		if (CnafSet != NULL && *CnafSet != '\0') {
			fCnafSet.SetBit(TMrbCNAF::kCnafAddr | TMrbCNAF::kCnafFunction, TMrbCNAF::kCnafAddr | TMrbCNAF::kCnafFunction);
 			fCnafSet.Ascii2Int(CnafSet);
			fCnafSet.Set(TMrbCNAF::kCnafCrate, Module->GetCNAF(TMrbCNAF::kCnafCrate));
			subAddr = fCnafSet.Get(TMrbCNAF::kCnafAddr);
			fHasCnafs |= TMrbCamacRegister::kRegSetCnaf;
		}

		if (CnafClear != NULL && *CnafClear != '\0') {
			fCnafClear.SetBit(TMrbCNAF::kCnafAddr | TMrbCNAF::kCnafFunction, TMrbCNAF::kCnafAddr | TMrbCNAF::kCnafFunction);
			fCnafClear.Ascii2Int(CnafClear);
			fCnafClear.Set(TMrbCNAF::kCnafCrate, Module->GetCNAF(TMrbCNAF::kCnafCrate));
			if (subAddr == -1) subAddr = fCnafClear.Get(TMrbCNAF::kCnafAddr);
			else if (subAddr != fCnafClear.Get(TMrbCNAF::kCnafAddr)) {
				gMrbLog->Err()	<< "SubAddr A(x) different - Set:A" << subAddr
								<< " ... Clear:A" << fCnafGet.Get(TMrbCNAF::kCnafAddr) << endl;
				gMrbLog->Flush(this->ClassName());
				this->MakeZombie();
			} else {
				fHasCnafs |= TMrbCamacRegister::kRegClearCnaf;
			}
		}

		if (CnafGet != NULL && *CnafGet != '\0') {
			fCnafGet.SetBit(TMrbCNAF::kCnafAddr | TMrbCNAF::kCnafFunction, TMrbCNAF::kCnafAddr | TMrbCNAF::kCnafFunction);
			fCnafGet.Ascii2Int(CnafGet);
			fCnafGet.Set(TMrbCNAF::kCnafCrate, Module->GetCNAF(TMrbCNAF::kCnafCrate));
			if (subAddr == -1) subAddr = fCnafGet.Get(TMrbCNAF::kCnafAddr);
			else if (subAddr != fCnafGet.Get(TMrbCNAF::kCnafAddr)) {
				gMrbLog->Err()	<< "SubAddr A(x) different - Set/Clear:A" << subAddr
								<< " ... Get:A" << fCnafGet.Get(TMrbCNAF::kCnafAddr) << endl;
				gMrbLog->Flush(this->ClassName());
				this->MakeZombie();
			} else {
				fHasCnafs |= TMrbCamacRegister::kRegGetCnaf;
			}
		}
	}
}

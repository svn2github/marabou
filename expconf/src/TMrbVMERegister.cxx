//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbVMERegister.cxx
// Purpose:        MARaBOU configuration: module registers
// Description:    Implements class methods to describe a register
//                 of a VME module
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
//////////////////////////////////////////////////////////////////////////////

using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbConfig.h"
#include "TMrbVMERegister.h"

extern class TMrbConfig * gMrbConfig;

ClassImp(TMrbVMERegister)

TMrbVMERegister::TMrbVMERegister(TMrbVMEModule * Module, Int_t NofChannels, TMrbNamedX * RegDef,
											Int_t AddrSet, Int_t AddrClear, Int_t AddrGet,
											Int_t InitValue, Int_t LowerLimit, Int_t UpperLimit,
											TMrbLofNamedX * BitNames, Bool_t PatternMode) :
														TMrbModuleRegister(Module, NofChannels, RegDef, InitValue,
																				LowerLimit, UpperLimit,
																				BitNames, PatternMode) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVMERegister
// Purpose:        Create a VME register
// Arguments:      TMrbCamacModule * Module       -- addr of parent module
//                 Int_t NofChannels              -- number of channels
//                 TMrbNamedX * RegDef            -- register index
//                 Int_t AddrSet                  -- address offset to set bits
//                 Int_t AddrClear                -- address offset to clear bits
//                 Int_t AddrGet                  -- address offset to set contents
//                 Int_t InitValue                -- initial value
//                 Int_t LowerLimit               -- lower limit
//                 Int_t UpperLimit               -- upper limit
//                 TMrbLofNamedX * BitNames       -- list of possible bit values
//                 Bool_t PatternMode             -- kTRUE if bitwise values
// Results:        --
// Exceptions:
// Description:    Defines a VME register.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->IsZombie()) {
		fAddrSet = AddrSet;
		fAddrClear = AddrClear;
		fAddrGet = AddrGet;
	}
}

#ifndef __TMrbVMERegister_h__
#define __TMrbVMERegister_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbVMERegister.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbVMERegister   -- base class to describe a set of VME registers
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "Rtypes.h"
#include "TSystem.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"

#include "TMrbVMEModule.h"
#include "TMrbModuleRegister.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVMERegister
// Purpose:        Define methods for VME registers
// Description:    Describes a VME register.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbVMERegister : public TMrbModuleRegister {

	public:

		TMrbVMERegister() {};										// default ctor

		TMrbVMERegister(TMrbVMEModule * Module, Int_t NofChannels, TMrbNamedX * RegDef,
									Int_t AddrSet,
									Int_t AddrClear,
									Int_t AddrGet,
									Int_t InitValue = 0, Int_t LowerLimit = 0, Int_t UpperLimit = -1,
									TMrbLofNamedX * BitNames = NULL, Bool_t PatternMode = kFALSE);

		~TMrbVMERegister() {};									// default dtor
	
		inline UInt_t AddrSet() { return(fAddrSet); };
		inline UInt_t AddrClear() { return(fAddrClear); };
		inline UInt_t AddrGet() { return(fAddrGet); };

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbVMERegister.html&"); };

//		void Print(ostream & OutStrm, Char_t * Prefix = "");
//		inline virtual void Print() { Print(cout, ""); };						// print settings

	protected:
		UInt_t fAddrSet;					// register offset (set bits)
		UInt_t fAddrClear;					// ... (clear bits)
		UInt_t fAddrGet;					// ... (get contents)

	ClassDef(TMrbVMERegister, 1)	// [Config] Base class for internal registers of VME modules
};	

#endif

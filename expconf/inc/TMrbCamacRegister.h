#ifndef __TMrbCamacRegister_h__
#define __TMrbCamacRegister_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbCamacRegister.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbModuleRegister   -- base class to describe a set of camac registers
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

#include "TMrbModuleRegister.h"
#include "TMrbCamacModule.h"
#include "TMrbCNAF.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCamacRegister
// Purpose:        Define methods for camac registers
// Description:    Describes a camac register.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbCamacRegister : public TMrbModuleRegister {

	public:
		enum EMrbRegCnafBits	{	kRegGetCnaf 	=	BIT(0),
									kRegClearCnaf	=	BIT(1),
									kRegSetCnaf 	=	BIT(2)
								};

	public:

		TMrbCamacRegister() {};										// default ctor

		TMrbCamacRegister(TMrbCamacModule * Module, Int_t NofChannels, TMrbNamedX * RegDef,
												const Char_t * CnafSet = NULL,
												const Char_t * CnafClear = NULL,
												const Char_t * CnafGet = NULL,
												Int_t InitValue = 0, Int_t LowerLimit = 0, Int_t UpperLimit = -1,
												TMrbLofNamedX * BitNames = NULL, Bool_t PatternMode = kFALSE);

		~TMrbCamacRegister() {};									// default dtor
	
		inline Bool_t HasCnafs() { return(fHasCnafs != 0); };		// test if cnafs present
		inline Bool_t HasGetCnaf() { return((fHasCnafs & TMrbCamacRegister::kRegGetCnaf) != 0); };
		inline Bool_t HasClearCnaf() { return((fHasCnafs & TMrbCamacRegister::kRegClearCnaf) != 0); };
		inline Bool_t HasSetCnaf() { return((fHasCnafs & TMrbCamacRegister::kRegSetCnaf) != 0); };

		inline TMrbCNAF * CnafSet() { return(&fCnafSet); };
		inline TMrbCNAF * CnafClear() { return(&fCnafClear); };
		inline TMrbCNAF * CnafGet() { return(&fCnafGet); };
		
		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbCamacRegister.html&"); };

//		void Print(ostream & OutStrm, const Char_t * Prefix = "");
//		inline virtual void Print() { Print(cout, ""); };						// print settings

	protected:
		UInt_t fHasCnafs;							// any cnafs given?
		TMrbCNAF fCnafSet;							// cnaf to set bits
		TMrbCNAF fCnafClear;						// cnaf to clear bits
		TMrbCNAF fCnafGet;							// cnaf to get contents

	ClassDef(TMrbCamacRegister, 1)		// [Config] Base class describing a internal register of a CAMAC module
};	

#endif

#ifndef __TMrbCamacRegister_h__
#define __TMrbCamacRegister_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbCamacRegister.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbModuleRegister   -- base class to describe a set of camac registers
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbCamacRegister.h,v 1.6 2005-09-09 06:59:13 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

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
	
		inline Bool_t HasCnafs() const { return(fHasCnafs != 0); };		// test if cnafs present
		inline Bool_t HasGetCnaf() const { return((fHasCnafs & TMrbCamacRegister::kRegGetCnaf) != 0); };
		inline Bool_t HasClearCnaf() const { return((fHasCnafs & TMrbCamacRegister::kRegClearCnaf) != 0); };
		inline Bool_t HasSetCnaf() const { return((fHasCnafs & TMrbCamacRegister::kRegSetCnaf) != 0); };

		inline TMrbCNAF * CnafSet() { return(&fCnafSet); };
		inline TMrbCNAF * CnafClear() { return(&fCnafClear); };
		inline TMrbCNAF * CnafGet() { return(&fCnafGet); };
		
		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		UInt_t fHasCnafs;							// any cnafs given?
		TMrbCNAF fCnafSet;							// cnaf to set bits
		TMrbCNAF fCnafClear;						// cnaf to clear bits
		TMrbCNAF fCnafGet;							// cnaf to get contents

	ClassDef(TMrbCamacRegister, 1)		// [Config] Base class describing a internal register of a CAMAC module
};	

#endif

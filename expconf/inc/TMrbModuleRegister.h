#ifndef __TMrbModuleRegister_h__
#define __TMrbModuleRegister_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbModuleRegister.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbModuleRegister   -- base class to describe a set of module registers
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
#include "TObject.h"
#include "TArrayI.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbModule.h"

enum EMrbRegisterAccess 	{	kMrbRegAccessReadWrite,
								kMrbRegAccessReadOnly,
								kMrbRegAccessWriteOnly
							};
							
//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModuleRegister
// Purpose:        Define methods for module registers
// Description:    Describes a module register.
// Keywords:	
//////////////////////////////////////////////////////////////////////////////

class TMrbModuleRegister : public TObject {

	public:

		TMrbModuleRegister() {};										// default ctor

		TMrbModuleRegister(TMrbModule * Module, Int_t NofChannels, TMrbNamedX * RegDef,
												Int_t InitValue = 0, Int_t LowerLimit = 0, Int_t UpperLimit = -1,
												TMrbLofNamedX * BitNames = NULL, Bool_t PatternMode = kFALSE,
												EMrbRegisterAccess AccessMode = kMrbRegAccessReadWrite);

		~TMrbModuleRegister() {};									// default dtor
	
		Int_t Get(Int_t Channel = -1);		 						// get (common) value
		Bool_t Set(Int_t Value);									// set common value
		Bool_t Set(Int_t Channel, Int_t Value); 					// set channel value explicitly

		Bool_t Set(const Char_t * Value);							// set common value
		Bool_t Set(Int_t Channel, const Char_t * Value); 			// set channel value explicitly

		Bool_t SetFromResource(Int_t Value);						// set (common) value from resource database

		inline TMrbModule * Parent() { return(fParent); };		 	// return addr of parent module
		inline Int_t GetIndex() { return(fRegDef->GetIndex()); };	// register index
		inline const Char_t * GetName() { return(fRegDef->GetName()); };	// register name
		inline void Reset() { IsCommon() ? Set(fInitValue) : Set(-1, fInitValue); };	// reset to initial values

		inline Int_t GetNofChannels() { return(fNofChannels); };	// nyumber of channels

		inline Bool_t IsCommon() { return(fNofChannels == 0); };	// register is common for all channels
		inline Bool_t IsPerChannel() { return(fNofChannels > 0); };	// register has values per channel

		inline EMrbRegisterAccess GetAccessMode() { return(fAccessMode); }; // access mode: rd/wr, rdonly, wronly
		inline Bool_t IsReadOnly() { return(fAccessMode == kMrbRegAccessReadOnly); };
		inline Bool_t IsWriteOnly() { return(fAccessMode == kMrbRegAccessWriteOnly); };
		
		inline void SetPatternMode(Bool_t Flag = kTRUE) { if (fLofBitNames) fPatternMode = Flag; };	// has bitwise value
		inline Bool_t IsPatternMode() { return(fPatternMode); };
		inline void SetLofBitNames(TMrbLofNamedX * BitNames) { fLofBitNames = BitNames; }; 	// list of bit names
		inline Bool_t HasBitNames() { return(fLofBitNames != NULL); };		// test if bit names given
		inline TMrbLofNamedX * BitNames() { return(fLofBitNames); };		// list of bit names

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbModuleRegister.html&"); };

		void Print(ostream & OutStrm, const Char_t * Prefix = "");
		inline virtual void Print() { Print(cout, ""); };						// print settings

		void PrintBitNames();						// output bit names if present

	protected:
		TMrbModule * fParent;						// parent module
		Int_t fNofChannels; 						// number of channels
		TMrbNamedX * fRegDef;						// register name & index

		EMrbRegisterAccess fAccessMode; 			// access mode
		
		Bool_t fPatternMode;						// kTRUE if bitwise value
		TMrbLofNamedX * fLofBitNames;				// list of possible bit values

		Int_t fInitValue;							// init value
		Int_t fLowerLimit;							// lower limit
		Int_t fUpperLimit;							// upper limit
		TArrayI fValues;							// current values

	ClassDef(TMrbModuleRegister, 1) 	// [Config] A base class describing internal module registers
};	

#endif

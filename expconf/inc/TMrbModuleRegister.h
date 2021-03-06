#ifndef __TMrbModuleRegister_h__
#define __TMrbModuleRegister_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbModuleRegister.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbModuleRegister   -- base class to describe a set of module registers
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbModuleRegister.h,v 1.8 2006-06-23 08:48:30 Marabou Exp $       
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
	
		Int_t Get(Int_t Channel = -1) const;		 						// get (common) value
		Bool_t Set(Int_t Value);									// set common value
		Bool_t Set(Int_t Channel, Int_t Value); 					// set channel value explicitly

		Bool_t Set(const Char_t * Value);							// set common value
		Bool_t Set(Int_t Channel, const Char_t * Value); 			// set channel value explicitly

		Bool_t SetFromResource(Int_t Value);						// set (common) value from resource database

		inline TMrbModule * Parent() const { return(fParent); };		 	// return addr of parent module
		inline Int_t GetIndex() const { return(fRegDef->GetIndex()); };	// register index
		inline const Char_t * GetName() const { return(fRegDef->GetName()); };	// register name
		inline void Reset() { IsCommon() ? Set(fInitValue) : Set(-1, fInitValue); };	// reset to initial values

		inline Int_t GetNofChannels() const { return(fNofChannels); };	// nyumber of channels

		inline Bool_t IsCommon() const { return(fNofChannels == 0); };	// register is common for all channels
		inline Bool_t IsPerChannel() const { return(fNofChannels > 0); };	// register has values per channel

		inline EMrbRegisterAccess GetAccessMode() const { return(fAccessMode); }; // access mode: rd/wr, rdonly, wronly
		inline Bool_t IsReadOnly() const { return(fAccessMode == kMrbRegAccessReadOnly); };
		inline Bool_t IsWriteOnly() const { return(fAccessMode == kMrbRegAccessWriteOnly); };
		
		inline void SetPatternMode(Bool_t Flag = kTRUE) { if (fLofBitNames) fPatternMode = Flag; };	// has bitwise value
		inline Bool_t IsPatternMode() const { return(fPatternMode); };
		void SetBoolean(Bool_t Flag = kTRUE);	// has boolean value
		inline Bool_t IsBoolean() const { return(fIsBoolean); };
		inline void SetLofBitNames(TMrbLofNamedX * BitNames) { fLofBitNames = BitNames; }; 	// list of bit names
		inline Bool_t HasBitNames() const { return(fLofBitNames != NULL); };		// test if bit names given
		inline TMrbLofNamedX * BitNames() { return(fLofBitNames); };		// list of bit names

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

		void Print(Option_t * Option) const { TObject::Print(Option); }
		void Print(ostream & OutStrm, const Char_t * Prefix = "") const;
		inline virtual void Print() const { Print(cout, ""); };						// print settings

	protected:
		TMrbModule * fParent;						// parent module
		Int_t fNofChannels; 						// number of channels
		TMrbNamedX * fRegDef;						// register name & index

		EMrbRegisterAccess fAccessMode; 			// access mode
		
		Bool_t fPatternMode;						// kTRUE if bitwise value
		Bool_t fIsBoolean;							// kTRUE if value boolean

		TMrbLofNamedX * fLofBitNames;				// list of possible bit values

		Int_t fInitValue;							// init value
		Int_t fLowerLimit;							// lower limit
		Int_t fUpperLimit;							// upper limit
		TArrayI fValues;							// current values

	ClassDef(TMrbModuleRegister, 1) 	// [Config] A base class describing internal module registers
};	

#endif

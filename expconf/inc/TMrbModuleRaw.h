#ifndef __TMrbModuleRaw_h__
#define __TMrbModuleRaw_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbModuleRaw.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbModuleRaw     -- a user-defined module
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
#include <fstream.h>

#include "Rtypes.h"
#include "TSystem.h"
#include "TObject.h"

#include "TMrbModule.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModuleRaw
// Purpose:        Implement a user-defined module
// Description:    Defines a module of unknown (user-defined) type.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbModuleRaw : public TMrbModule {

	public:

		TMrbModuleRaw() {};														// default ctor
		TMrbModuleRaw(const Char_t * ModuleName, const Char_t * ModuleID, Int_t NofChannels, Int_t Range,
																Option_t * DataType = "I");	// ctor
		~TMrbModuleRaw() {};														// remove silena adc from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TObject * Channel, Int_t Value = 0);  	// generate code for given channel

		inline Bool_t ConvertToInt() { return(kFALSE); };						// data type remains unchanged
		virtual inline Bool_t IsRaw() { return(kTRUE); };						// indicates raw (user-defined) mode

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbModuleRaw.html&"); };

	protected:
		virtual void DefineRegisters() {}; 				// define registers

	ClassDef(TMrbModuleRaw, 1)		// [Config] A "raw" (user-defined) module
};

#endif

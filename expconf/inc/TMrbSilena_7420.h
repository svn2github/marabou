#ifndef __TMrbSilena_7420_h__
#define __TMrbSilena_7420_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSilena_7420.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSilena_7420      -- silena adc (NIM) + input register (tum/w.liebl)
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

#include "TMrbCamacModule.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSilena_7420
// Purpose:        Define a silena adc type 7420 (NIM)
// Description:    Defines a silena adc + input register (tum/w.liebl).
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSilena_7420 : public TMrbCamacModule {

	public:
		enum	EMrbRegisters	{	kRegLowerThresh,
									kRegUpperThresh
								};

	public:

		TMrbSilena_7420() {};												// default ctor
		TMrbSilena_7420(const Char_t * ModuleName, const Char_t * ModulePosition);		// define a new silena adc
		~TMrbSilena_7420() {};												// remove silena adc from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagINdex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TObject * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() { return("smdw"); }; 	// module mnemonic

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbSilena_7420.html&"); };

	protected:
		void DefineRegisters(); 						// define camac regs

	ClassDef(TMrbSilena_7420, 1)		// [Config] Base class describing a MBS subevent
};

#endif

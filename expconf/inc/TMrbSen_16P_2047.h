#ifndef __TMrbSen_16P_2047_h__
#define __TMrbSen_16P_2047_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSen_16P_2047.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSen_16P_2047     -- pattern unit
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       
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

#include "TMrbCamacModule.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSen_16P_2047
// Purpose:        Define a pattern unit of type SEN 16P 2047
// Description:    Defines a pattern unit of type SEN 16P 2047
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSen_16P_2047 : public TMrbCamacModule {

	public:

		TMrbSen_16P_2047() {};  													// default ctor
		TMrbSen_16P_2047(const Char_t * ModuleName, const Char_t * ModulePosition); // define a new pattern unit
		~TMrbSen_16P_2047() {};														// remove pattern unit from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TObject * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() const { return("sen_16p"); }; 	// module mnemonic

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbSen_16P_2047.html&"); };

	ClassDef(TMrbSen_16P_2047, 1)		// [Config] SEN 16P 2047, CAMAC pattern unit
};

#endif

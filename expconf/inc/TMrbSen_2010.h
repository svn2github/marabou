#ifndef __TMrbSen_2010_h__
#define __TMrbSen_2010_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSen_2010.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSen_2010         -- dual input register
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

#include "TMrbCamacModule.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSen_2010
// Purpose:        Define a dual input register SEN 2010
// Description:    Defines a dual input register SEN 2010
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSen_2010 : public TMrbCamacModule {

	public:

		TMrbSen_2010() {};  													// default ctor
		TMrbSen_2010(const Char_t * ModuleName, const Char_t * ModulePosition); // define a new pattern unit
		~TMrbSen_2010() {};														// remove pattern unit from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TObject * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() { return("sen_2010"); }; 	// module mnemonic

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbSen_2010.html&"); };

	ClassDef(TMrbSen_2010, 1)		// [Config] SEN 2010, dual CAMAC input register
};

#endif

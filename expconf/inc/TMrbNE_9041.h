#ifndef __TMrbNE_9041_h__
#define __TMrbNE_9041_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbNE_9041.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbNE_9041          -- dual input register
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
// Name:           TMrbNE_9041
// Purpose:        Define a dual input register Nuclear Enterprises 9041
// Description:    Defines a dual input register NE 9041
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbNE_9041 : public TMrbCamacModule {

	public:

		TMrbNE_9041() {};  														// default ctor
		TMrbNE_9041(const Char_t * ModuleName, const Char_t * ModulePosition); 	// define a new pattern unit
		~TMrbNE_9041() {};														// remove pattern unit from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TObject * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() { return("ne_9041"); }; 	// module mnemonic

		inline void SetWaitForQ(Bool_t Flag = kTRUE) { fWaitForQ = Flag; }; 	// q-wait or loop?
		inline Bool_t WaitForQ() { return(fWaitForQ); };

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbNE_9041.html&"); };

	protected:
		Bool_t fWaitForQ;

	ClassDef(TMrbNE_9041, 1)		// [Config] NE 9041, dual CAMAC input register
};

#endif

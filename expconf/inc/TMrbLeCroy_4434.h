#ifndef __TMrbLeCroy_4434_h__
#define __TMrbLeCroy_4434_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbLeCroy_4434.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbLeCroy_4434      -- lecroy scaler 4434 (list mode)
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
// Name:           TMrbLeCroy_4434
// Purpose:        Define a scaler LeCroy 4434
// Description:    Defines a scaler of type LeCroy 4434 to be used in list mode.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbLeCroy_4434 : public TMrbCamacModule {

	public:

		TMrbLeCroy_4434() {};  														// default ctor
		TMrbLeCroy_4434(const Char_t * ModuleName, const Char_t * ModulePosition);	// define a new lecroy scaler
		~TMrbLeCroy_4434() {};														// remove silena tdc from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TObject * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() { return("lecroy_4434"); }; 	// module mnemonic

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbLeCroy_4434.html&"); };

	ClassDef(TMrbLeCroy_4434, 1)		// [Config] LeCroy 4434, 32 chn CAMAC scaler
};

#endif

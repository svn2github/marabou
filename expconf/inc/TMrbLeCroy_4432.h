#ifndef __TMrbLeCroy_4432_h__
#define __TMrbLeCroy_4432_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbLeCroy_4432.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbLeCroy_4432      -- lecroy scaler 4432 (list mode)
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
// Name:           TMrbLeCroy_4432
// Purpose:        Define a scaler LeCroy 4432
// Description:    Defines a scaler of type LeCroy 4432 to be used in list mode.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbLeCroy_4432 : public TMrbCamacModule {

	public:

		TMrbLeCroy_4432() {};  														// default ctor
		TMrbLeCroy_4432(const Char_t * ModuleName, const Char_t * ModulePosition);	// define a new lecroy scaler
		~TMrbLeCroy_4432() {};														// remove silena tdc from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TObject * Channel, Int_t Value = 0);  	// generate code for given channel
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, TMrbTemplate & Template, const Char_t * Prefix = NULL) { return(kFALSE); }; // generate readout code

		virtual inline const Char_t * GetMnemonic() const { return("lecroy_4432"); }; 	// module mnemonic

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbLeCroy_4432.html&"); };

	ClassDef(TMrbLeCroy_4432, 1)		// [Config] LeCroy 4432, 32 chn CAMAC scaler
};

#endif

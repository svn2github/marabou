#ifndef __TMrbLeCroy_4448_h__
#define __TMrbLeCroy_4448_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbLeCroy_4448.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbLeCroy_4448      -- lecroy coinc unit 4448 (list mode)
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <iostream.h>
#include <strstream.h>
#include <iomanip.h>
#include <fstream.h>

#include "Rtypes.h"
#include "TSystem.h"
#include "TObject.h"

#include "TMrbCamacModule.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLeCroy_4448
// Purpose:        Define a coinc unit LeCroy 4448
// Description:    Defines a coinc unit of type LeCroy 4448 to be used in list mode.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbLeCroy_4448 : public TMrbCamacModule {

	public:

		TMrbLeCroy_4448() {};  														// default ctor
		TMrbLeCroy_4448(const Char_t * ModuleName, const Char_t * ModulePosition);	// define a new lecroy coinc unit
		~TMrbLeCroy_4448() {};														// remove silena tdc from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TObject * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() { return("lecroy_4448"); }; 	// module mnemonic

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbLeCroy_4448.html&"); };

	ClassDef(TMrbLeCroy_4448, 1)		// [Config] LeCroy 4448, 32 chn CAMAC coinc unit
};

#endif

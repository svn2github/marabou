#ifndef __TMrbLeCroy_2228A_h__
#define __TMrbLeCroy_2228A_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbLeCroy_2228A.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbLeCroy_2228A     -- lecroy tdc 2228A
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
// Name:           TMrbLeCroy_2228A
// Purpose:        Define a TDC of type LeCroy 2228A
// Description:    Defines a TDC of type LeCroy 2228A.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbLeCroy_2228A : public TMrbCamacModule {

	public:

		TMrbLeCroy_2228A() {};  														// default ctor
		TMrbLeCroy_2228A(const Char_t * ModuleName, const Char_t * ModulePosition);		// define a new lecroy tdc
		~TMrbLeCroy_2228A() {};															// remove silena tdc from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TObject * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() { return("lecroy_2228a"); }; 	// module mnemonic

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbLeCroy_2228A.html&"); };

	ClassDef(TMrbLeCroy_2228A, 1)		// [Config] LeCroy 2228A, 12 x 2K CAMAC TDC
};

#endif

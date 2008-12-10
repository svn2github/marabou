#ifndef __TMrbLeCroy_2280_h__
#define __TMrbLeCroy_2280_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbLeCroy_2280.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbLeCroy_2280     -- lecroy qdc 2280
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbLeCroy_2280.h,v 1.5 2008-12-10 12:13:49 Rudolf.Lutter Exp $       
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

class TMrbModuleChannel;

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLeCroy_2280
// Purpose:        Define a QDC of type LeCroy 2280
// Description:    Defines a QDC of type LeCroy 2280.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbLeCroy_2280 : public TMrbCamacModule {

	public:

		TMrbLeCroy_2280() {};  														// default ctor
		TMrbLeCroy_2280(const Char_t * ModuleName, const Char_t * ModulePosition);		// define a new lecroy tdc
		~TMrbLeCroy_2280() {};															// remove silena tdc from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TMrbModuleChannel * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() const { return("lecroy_2280"); }; 	// module mnemonic

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	ClassDef(TMrbLeCroy_2280, 1)		// [Config] LeCroy 2280, 48 x 4K CAMAC QDC
};

#endif

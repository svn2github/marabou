#ifndef __TMrbLeCroy_2228A_h__
#define __TMrbLeCroy_2228A_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbLeCroy_2228A.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbLeCroy_2228A     -- lecroy tdc 2228A
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbLeCroy_2228A.h,v 1.9 2008-12-10 12:13:49 Rudolf.Lutter Exp $       
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
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TMrbModuleChannel * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() const { return("lecroy_2228a"); }; 	// module mnemonic

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	ClassDef(TMrbLeCroy_2228A, 1)		// [Config] LeCroy 2228A, 12 x 2K CAMAC TDC
};

#endif

#ifndef __TMrbISN_4481_h__
#define __TMrbISN_4481_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbISN_4481.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbISN_4481     -- tdc isn4481
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbISN_4481.h,v 1.3 2008-12-10 12:13:49 Rudolf.Lutter Exp $       
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
// Name:           TMrbISN_4481
// Purpose:        Define an TDC of type ISN 4481
// Description:    Defines an TDC of type ISN 4481
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbISN_4481 : public TMrbCamacModule {

	public:
	public:

		TMrbISN_4481() {};														// default ctor
		TMrbISN_4481(const Char_t * ModuleName, const Char_t * ModulePosition);				// define a new silena adc
		~TMrbISN_4481() {};														// remove silena adc from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TMrbModuleChannel * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() const { return("ISN_4481"); }; 	// module mnemonic

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		void DefineRegisters(); 							// define camac registers

	ClassDef(TMrbISN_4481, 1) 	// [Config] ISN 4481, 8 x 4K TDC
};

#endif

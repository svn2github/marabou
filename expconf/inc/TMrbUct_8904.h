#ifndef __TMrbUct_8904_h__
#define __TMrbUct_8904_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbUct_8904.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbUct_8904         -- scaler uct tum/n.franz
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbUct_8904.h,v 1.9 2008-12-10 12:13:49 Rudolf.Lutter Exp $       
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

#include "TMrbCamacScaler.h"

class TMrbModuleChannel;

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbUct_8904
// Purpose:        Define a scaler of type UCT
// Description:    Defines a scaler type UCT (TUM, N. Franz).
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbUct_8904 : public TMrbCamacScaler {

	public:

		TMrbUct_8904() {};											// default ctor
		TMrbUct_8904(const Char_t * ScalerName, const Char_t * ScalerPosition);	// create a new scaler object
		~TMrbUct_8904() {};	 										// remove current scaler from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  		// generate part of code for this scaler
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TMrbModuleChannel * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() const { return("uct"); }; 	// module mnemonic

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	ClassDef(TMrbUct_8904, 1)		// [Config] Universal Counter/Timer 10 MHz (CAMAC)
};

#endif

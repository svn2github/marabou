#ifndef __TMrbCamacScaler_h__
#define __TMrbCamacScaler_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbCamacScaler.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbCamacScaler      -- base class for camac scalers
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

#include "TMrbCamacModule.h"
#include "TMrbScaler.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCamacScaler
// Purpose:        Define methods for a CAMAC scaler
// Description:    Defines a CAMAC scaler
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbCamacScaler : public TMrbCamacModule, public TMrbScaler {

	public:

		TMrbCamacScaler() {};											// default ctor
																		// ctor: define name & camac position
		TMrbCamacScaler(const Char_t * ScalerName, const Char_t * ScalerID, const Char_t * ScalerPosition,
															Int_t SubDevice, Int_t NofChannels, Int_t Range) :
								TMrbCamacModule(ScalerName, ScalerID, ScalerPosition, SubDevice, NofChannels, Range),
								TMrbScaler() {	fHistosToBeAllocated = kFALSE; };
		~TMrbCamacScaler() {};											// default dtor

		void Print(ostream & OutStrm, const Char_t * Prefix = "");	// show data
		inline virtual void Print() { Print(cout, ""); };

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbCamacScaler.html&"); };

	ClassDef(TMrbCamacScaler, 1)		// [Config] Base class describing a CAMAC scaler
};	

#endif

#ifndef __TMrbVMEScaler_h__
#define __TMrbVMEScaler_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbVMEScaler.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbVMEScaler      -- base class for VME scalers
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

#include "TMrbVMEModule.h"
#include "TMrbScaler.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVMEScaler
// Purpose:        Define methods for a VME scaler
// Description:    Defines a VME scaler
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbVMEScaler : public TMrbVMEModule, public TMrbScaler {

	public:

		TMrbVMEScaler() {};											// default ctor
		TMrbVMEScaler(const Char_t * ScalerName, const Char_t * ScalerID,
							UInt_t BaseAddr, UInt_t AddrMod, Int_t SegLength,
							Int_t Channel0, Int_t NofChannels, Int_t Range) :
									TMrbVMEModule(ScalerName, ScalerID,
													BaseAddr, AddrMod, SegLength, Channel0, NofChannels, Range),
									TMrbScaler() {	fHistosToBeAllocated = kFALSE; };
		~TMrbVMEScaler() {};											// default dtor

		void Print(Option_t * Option) const { TObject::Print(Option); }
		void Print(ostream & OutStrm, const Char_t * Prefix = "") const;	// show data
		inline virtual void Print() const { Print(cout, ""); };

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbVMEScaler.html&"); };

	ClassDef(TMrbVMEScaler, 1)		// [Config] A base class for VME scalers
};	

#endif

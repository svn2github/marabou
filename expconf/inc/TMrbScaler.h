#ifndef __TMrbScaler_h__
#define __TMrbScaler_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbScaler.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbScaler      -- base class for camac and vme scalers
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbScaler.h,v 1.5 2004-09-28 13:47:32 rudi Exp $       
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
#include "TObjArray.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbScaler
// Purpose:        Define methods for a scaler
// Description:    Defines a CAMAC/VME scaler
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbScaler {

	public:

		TMrbScaler();					// ctor

		virtual ~TMrbScaler() {};		// default dtor

		void Preset(Int_t Preset);													// preset scaler
		inline Int_t GetPresetValue() const { return(fPresetValue); };

		inline void SetMode(UInt_t ScalerMode) { fScalerMode |= ScalerMode; };		// scaler mode
		inline UInt_t GetMode() const { return(fScalerMode); };

		inline void SetDTScaler() { fScalerMode |= TMrbConfig::kScalerDeadTime; }; 	// to be used as dead time scaler
		inline Bool_t IsDTScaler() const { return((fScalerMode & TMrbConfig::kScalerDeadTime) != 0); };

		void SetExternalGate(Bool_t XGateFlag = kTRUE); 	// enable/disable external gate
		inline Bool_t HasExternalGate() const { return((fScalerMode & TMrbConfig::kScalerExternalGate) != 0); };

		void SetFunctionName(const Char_t * FctName, TMrbConfig::EMrbScalerFunction Fct);	// define function name
		const Char_t * GetFunctionName(TMrbConfig::EMrbScalerFunction Fct) const;

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbScaler.html&"); };

	protected:
		UInt_t fScalerMode; 			// scaler mode
		Int_t fPresetValue;				// preset
		TObjArray fLofFunctionNames;	// function names

	ClassDef(TMrbScaler, 1) 	// [Config] Base class describing a scaler (CAMAC or VME)
};	

#endif

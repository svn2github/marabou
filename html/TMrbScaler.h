#ifndef __TMrbScaler_h__
#define __TMrbScaler_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbScaler.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbScaler      -- base class for camac and vme scalers
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
		inline Int_t GetPresetValue() { return(fPresetValue); };

		inline void SetMode(UInt_t ScalerMode) { fScalerMode |= ScalerMode; };		// scaler mode
		inline UInt_t GetMode() { return(fScalerMode); };

		inline void SetDTScaler() { fScalerMode |= TMrbConfig::kScalerDeadTime; }; 	// to be used as dead time scaler
		inline Bool_t IsDTScaler() { return((fScalerMode & TMrbConfig::kScalerDeadTime) != 0); };

		void SetExternalGate(Bool_t XGateFlag = kTRUE); 	// enable/disable external gate
		inline Bool_t HasExternalGate() { return((fScalerMode & TMrbConfig::kScalerExternalGate) != 0); };

		void SetFunctionName(const Char_t * FctName, TMrbConfig::EMrbScalerFunction Fct);	// define function name
		const Char_t * GetFunctionName(TMrbConfig::EMrbScalerFunction Fct);

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbScaler.html&"); };

	protected:
		UInt_t fScalerMode; 			// scaler mode
		Int_t fPresetValue;				// preset
		TObjArray fLofFunctionNames;	// function names

	ClassDef(TMrbScaler, 1) 	// [Config] Base class describing a scaler (CAMAC or VME)
};	

#endif

//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbScaler.cxx
// Purpose:        MARaBOU configuration: scaler definitions
// Description:    Implements class methods to handle scalers
// Header files:   TMrbConfig.h  -- MARaBOU class definitions
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TMrbConfig.h"
#include "TMrbScaler.h"

// input class definitions */

ClassImp(TMrbScaler)

TMrbScaler::TMrbScaler() {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbScaler
// Purpose:        Base class for CAMAC/VME scalers
// Exceptions:     
// Description:    Scaler defs.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fScalerMode = 0;
	fPresetValue = 0;
	fLofFunctionNames.Expand(TMrbConfig::kNofScalerFunctions);
}

void TMrbScaler::Preset(Int_t Preset) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbScaler::Preset
// Purpose:        Preset scaler
// Arguments:      Int_t Preset      -- preset value
// Results:        
// Exceptions:     
// Description:    Sets the scaler preset value.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fPresetValue = Preset;
}

void TMrbScaler::SetExternalGate(Bool_t XGateFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbScaler::SetExternalgate
// Purpose:        Set external gate bit
// Arguments:      Bool_t XGateFlag   -- kTRUE if external gate enabled
// Results:        --
// Exceptions:     
// Description:    Enables/disables external gate.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (XGateFlag)	fScalerMode |= TMrbConfig::kScalerExternalGate;
	else			fScalerMode &= ~TMrbConfig::kScalerExternalGate;
}

void TMrbScaler::SetFunctionName(const Char_t * FctName, TMrbConfig::EMrbScalerFunction Fct) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbScaler::SetFunctionName
// Purpose:        Define function name
// Arguments:      Char_t * FctName         -- function name
//                 EMrbScalerFunction Fct   -- function code
// Results:        --
// Exceptions:     
// Description:    Defines a function name to be used for readout.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TObjString * str;
	str = new TObjString(FctName);
	fLofFunctionNames.AddAt(str, Fct);
}

const Char_t * TMrbScaler::GetFunctionName(TMrbConfig::EMrbScalerFunction Fct) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbScaler::GetFunctionName
// Purpose:        Return function name
// Arguments:      EMrbScalerFunction Fct   -- function code
// Results:        Char_t * FctName         -- function name
// Exceptions:     
// Description:    Returns a function name to be used for readout.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	return(((TObjString *) fLofFunctionNames.At(Fct))->GetString().Data());
}

#ifndef __TMbsTriggerModule_h__
#define __TMbsTriggerModule_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbssetup/inc/TMbsTriggerModule.h
// Purpose:        Define a MBS setup
// Class:          TMbsTriggerModule    -- trigger module
// Description:    Class definitions to generate a MBS setup.
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TSystem.h"

#include "TMrbNamedX.h"

#include "TMbsSetupCommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsTriggerModule
// Purpose:        Define a trigger module
// Methods:
// Description:    Creates a trigger module.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMbsTriggerModule : public TObject {

	public:

		TMbsTriggerModule();						// default ctor
		TMbsTriggerModule(Int_t Id);				// explicit ctor
		~TMbsTriggerModule() {};					// default dtor

		Bool_t SetType(const Char_t * ModuleType);		// module type
		Bool_t SetType(EMbsTriggerModuleType ModuleType);
		TMrbNamedX * GetType();

		Bool_t SetTriggerMode(const Char_t * Mode);		// trigger mode
		Bool_t SetTriggerMode(EMbsTriggerMode Mode);
		TMrbNamedX * GetTriggerMode();

		Bool_t SetConversionTime(Int_t ConvTime);			// conv time
		Int_t GetConversionTime();

		Bool_t SetFastClearTime(Int_t FastClearTime);		// fct
		Int_t GetFastClearTime();

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMbsTriggerModule.html&"); };

	protected:
		Int_t fId; 						// id (same as readout proc's id)

	ClassDef(TMbsTriggerModule, 1)		// [MBS Setup] Trigger module
};

#endif

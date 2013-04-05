#ifndef __TC2LCaen785_h__
#define __TC2LCaen785_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TC2LCaen785.h
// Purpose:        Connect to a CAEN V785 ADC
// Class:          TC2LCaen785            -- base class
// Description:    Class definitions to establish a connection to a VME
//                 module running under LynxOs.
// Author:         R. Lutter
// Revision:       $Id: TC2LCaen785.h,v 1.1 2008-07-04 11:58:06 Rudolf.Lutter Exp $   
// Date:           $Date: 2008-07-04 11:58:06 $
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TC2LVMEModule.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TC2LCaen785
// Purpose:        Connect to a CAEN V785 ADC
// Description:    Defines a module and tries to connect to it
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TC2LCaen785 : public TC2LVMEModule {

	public:

		TC2LCaen785() {};								// default ctor		

		TC2LCaen785(const Char_t * ModuleName,
			UInt_t Address = 0, Int_t SegSize = 0, Int_t NofChannels = 0, UInt_t Mapping = 0, Bool_t Offline = kFALSE)
						: TC2LVMEModule(ModuleName, "Caen785", Address, SegSize, NofChannels, Mapping, Offline) {};

		~TC2LCaen785() {};							// default dtor

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };
		
	ClassDef(TC2LVMEModule, 1)		// [Access to LynxOs] Connect to a Caen V785 adc
};

#endif

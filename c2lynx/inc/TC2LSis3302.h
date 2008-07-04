#ifndef __TC2LSis3302_h__
#define __TC2LSis3302_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TC2LSis3302.h
// Purpose:        Connect to a SIS 3302 flash ADC
// Class:          TC2LSis3302            -- base class
// Description:    Class definitions to establish a connection to a VME
//                 module running under LynxOs.
// Author:         R. Lutter
// Revision:       $Id: TC2LSis3302.h,v 1.1 2008-07-04 11:58:06 Rudolf.Lutter Exp $   
// Date:           $Date: 2008-07-04 11:58:06 $
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TC2LVMEModule.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TC2LSis3302
// Purpose:        Connect to a SIS 3302 ADC
// Description:    Defines a module and tries to connect to it
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TC2LSis3302 : public TC2LVMEModule {

	public:

		TC2LSis3302() {};								// default ctor		

		TC2LSis3302(const Char_t * ModuleName, UInt_t Address = 0, Int_t NofChannels = 0)
								: TC2LVMEModule(ModuleName, "Sis3302", Address, NofChannels) {};

		~TC2LSis3302() {};							// default dtor

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };
		
	ClassDef(TC2LSis3302, 1)		// [Access to LynxOs] Connect to a Sis3302 adc
};

#endif

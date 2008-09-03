#ifndef __TC2LVMEModule_h__
#define __TC2LVMEModule_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TC2LVMEModule.h
// Purpose:        Connect to a VME module
// Class:          TC2LVMEModule            -- base class
// Description:    Class definitions to establish a connection to a VME
//                 module running under LynxOs.
// Author:         R. Lutter
// Revision:       $Id: TC2LVMEModule.h,v 1.3 2008-09-03 14:23:54 Rudolf.Lutter Exp $   
// Date:           $Date: 2008-09-03 14:23:54 $
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TMrbC2Lynx.h"
#include "TMrbNamedX.h"

#include "M2L_MessageTypes.h"
#include "M2L_CommonStructs.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TC2LVMEModule
// Purpose:        Connect to a VME module running under LynxOs
// Description:    Defines a module and tries to connect to it
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TC2LVMEModule : public TMrbNamedX {

	public:

		TC2LVMEModule() {};								// default ctor		

		TC2LVMEModule(const Char_t * ModuleName, const Char_t * ModuleType, UInt_t Address = 0, Int_t NofChannels = 0);

		~TC2LVMEModule() {};							// default dtor

		inline UInt_t GetHandle() { return((UInt_t) this->GetIndex()); };
		inline const Char_t * GetType() { return(this->GetTitle()); };

		inline UInt_t GetAddress() { return(fBaseAddress); };
		inline Int_t GetNofChannels() { return(fNofChannels); };

		Bool_t Connect(UInt_t Address, Int_t NofChannels);
		Bool_t GetModuleInfo();

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		inline void SetAddress(UInt_t Address) { fBaseAddress = Address; };
		inline void SetNofChannels(Int_t NofChannels) { fNofChannels = NofChannels; };
		inline void SetHandle(UInt_t Handle) { this->SetIndex((Int_t) Handle); };

	protected:
		UInt_t fBaseAddress; 		// module address
		Int_t fNofChannels; 		// number of channels used
		
	ClassDef(TC2LVMEModule, 1)		// [Access to LynxOs] Connect to a VME module
};

#endif

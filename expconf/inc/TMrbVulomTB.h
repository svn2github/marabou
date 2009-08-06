#ifndef __TMrbVulomTB_h__
#define __TMrbVulomTB_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbVulomTB.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbVulomTB        -- Vulom TriggerBox
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbVulomTB.h,v 1.1 2009-08-06 08:30:58 Rudolf.Lutter Exp $
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

#include "TMrbVMEModule.h"

class TMrbVMEChannel;
class TMrbSubevent;

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVulomTB
// Purpose:        Define a VME Vulom TriggerBox
// Description:    Defines a VME Vulom TriggerBox
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbVulomTB : public TMrbVMEModule {

	public:
		enum				{	kSegSize			=	0x10000	};
		enum				{	kAddrMod			=	0x09	};


	public:

		TMrbVulomTB() {};  												// default ctor
		TMrbVulomTB(const Char_t * ModuleName, UInt_t BaseAddr); 			// define a new adc
		~TMrbVulomTB() {};												// default dtor

		virtual inline const Char_t * GetMnemonic() const { return("vulomTB"); }; 	// module mnemonic

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	ClassDef(TMrbVulomTB, 1)		// [Config] VulomTB, triggerBox GSI
};

#endif

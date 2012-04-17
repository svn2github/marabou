#ifndef __TMrbSubevent_Sis_3302_2_h__
#define __TMrbSubevent_Sis_3302_2_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSubevent_Sis_3302_2.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSubevent_Sis_3302_2   -- MBS subevent type [10,54]
//                                            dedicated format for Sis modules
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbSubevent_Sis_3302_2.h,v 1.1 2010-12-15 09:07:47 Marabou Exp $
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

#include "TMrbSubevent.h"
#include "TMrbTemplate.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent_Sis_3302_2
// Purpose:        Define MBS subevents of type [10,56]
// Methods:        MakeReadoutCode  -- output readout code for subevent [10,56]
// Description:    Defines a MBS subevent of type [10,56] - a special format
//                 to store data in a hitbuffer
//                 Generates code.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSubevent_Sis_3302_2 : public TMrbSubevent {

	public:

		TMrbSubevent_Sis_3302_2() {}; 				// default ctor
		TMrbSubevent_Sis_3302_2(const Char_t * SevtName, const Char_t * SevtTitle = "", Int_t Crate = -1);	// create a new SIS subevent
		~TMrbSubevent_Sis_3302_2() {};	 			// remove current subevent from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, 		// generate part of code for this subevent
											TMrbTemplate & Template, const Char_t * Prefix = NULL);

		inline Bool_t HasFixedLengthFormat() const { return(kFALSE); };			// variable length data
		inline Bool_t AllowsMultipleModules() const { return(kTRUE); };			// can store multiple modules

		inline Bool_t CheckModuleID(TMrbModule * Module) const {			// needs modules of type Sis
			return(Module->CheckID(TMrbConfig::kModuleSis_3302));
		};

		inline Bool_t NeedsHitBuffer() const { return(kTRUE); };  			// allocate hit buffer
		inline Bool_t HasPrivateCode() const { return(kTRUE); }; 			// use private code files
		inline Bool_t NeedsBranchMode() const { return(kTRUE); }; 			// needs branch mode

		inline const Char_t * GetCommonCodeFile() { return("Subevent_Sis3302_Common"); };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	ClassDef(TMrbSubevent_Sis_3302_2, 1) 	// [Config] Subevent type [10,56]: a format dedicated to SIS3302 modules
};

#endif

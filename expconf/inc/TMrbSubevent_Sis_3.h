#ifndef __TMrbSubevent_Sis_3_h__
#define __TMrbSubevent_Sis_3_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSubevent_Sis_3.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSubevent_Sis_3   -- MBS subevent type [10,43]
//                                         dedicated format for Sis modules
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

using namespace std;

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
// Name:           TMrbSubevent_Sis_3
// Purpose:        Define MBS subevents of type [10,53]
// Methods:        MakeReadoutCode  -- output readout code for subevent [10,53]
// Description:    Defines a MBS subevent of type [10,53] - a special format
//                 reflecting DGF's data structure.
//                 Generates code.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSubevent_Sis_3 : public TMrbSubevent {

	public:

		TMrbSubevent_Sis_3() {}; 				// default ctor
		TMrbSubevent_Sis_3(const Char_t * SevtName, const Char_t * SevtTitle = "", Int_t Crate = -1);	// create a new DGF subevent
		~TMrbSubevent_Sis_3() {};	 			// remove current subevent from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, 		// generate part of code for this subevent
											TMrbTemplate & Template, const Char_t * Prefix = NULL);

		inline Bool_t HasFixedLengthFormat() { return(kFALSE); };				// variable length data
		inline Bool_t AllowsMultipleModules() { return(kTRUE); };				// can store multiple modules

		inline Bool_t CheckModuleID(TMrbModule * Module) {						// needs modules of type Sis
			return(Module->CheckID(TMrbConfig::kModuleSis_3600) || Module->CheckID(TMrbConfig::kModuleSis_3801));
		};
				
		inline Bool_t NeedsHitBuffer() const { return(kTRUE); };  					// allocate hit buffer
		inline Bool_t HasPrivateCode() const { return(kTRUE); }; 					// use private code files
		inline Bool_t NeedsBranchMode() const { return(kTRUE); }; 					// needs branch mode
		inline const Char_t * GetCommonCodeFile() const { return("Subevent_Sis_Common"); };
		
		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbSubevent_Sis_3.html&"); };

	ClassDef(TMrbSubevent_Sis_3, 1) 	// [Config] Subevent type [10,53]: a format dedicated to SIS modules
};

#endif

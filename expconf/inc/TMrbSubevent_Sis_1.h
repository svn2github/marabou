#ifndef __TMrbSubevent_Sis_1_h__
#define __TMrbSubevent_Sis_1_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSubevent_Sis_1.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSubevent_Sis_1   -- MBS subevent type [10,51]
//                                          dedicated format CAEN modules
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       
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

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSubevent_Sis_1.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSubevent_Sis_1   -- MBS subevent type [10,51]
//                                         dedicated format for Sis modules
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSubevent_Sis_1 : public TMrbSubevent {

	public:

		TMrbSubevent_Sis_1() {}; 				// default ctor
		TMrbSubevent_Sis_1(const Char_t * SevtName, const Char_t * SevtTitle = "", Int_t Crate = -1);	// create a new DGF subevent
		~TMrbSubevent_Sis_1() {};	 			// remove current subevent from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, 		// generate part of code for this subevent
											TMrbTemplate & Template, const Char_t * Prefix = NULL);

		inline Bool_t HasFixedLengthFormat() const { return(kFALSE); };				// variable length data
		inline Bool_t AllowsMultipleModules() const { return(kTRUE); };				// can store multiple modules

		inline Bool_t CheckModuleID(TMrbModule * Module) {						// needs modules of type Sis
			return(Module->CheckID(TMrbConfig::kModuleSis_3600) || Module->CheckID(TMrbConfig::kModuleSis_3801));
		};
				
		inline Bool_t HasPrivateCode() const { return(kTRUE); }; 						// use private code files
		
		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbSubevent_Sis_1.html&"); };

	ClassDef(TMrbSubevent_Sis_1, 1) 	// [Config] Subevent type [10,51]: a format dedicated to CAEN modules
};

#endif

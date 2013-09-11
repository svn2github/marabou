#ifndef __TMrbSubevent_Gassiplex_3_h__
#define __TMrbSubevent_Gassiplex_3_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSubevent_Gassiplex_3.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSubevent_Gassiplex_3   -- MBS subevent type [10,103]
//                                               dedicated format Gassiplex modules
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id$
// Date:           $Date$
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
// Name:           TMrbSubevent_Gassiplex_3
// Purpose:        Define MBS subevents of type [10,103]
// Methods:        MakeReadoutCode  -- output readout code for subevent [10,103]
// Description:    Defines a MBS subevent of type [10,103] - a special format
//                 reflecting Gassiplex data structure.
//                 Generates code.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSubevent_Gassiplex_3 : public TMrbSubevent {

	public:

		TMrbSubevent_Gassiplex_3() {}; 				// default ctor
		TMrbSubevent_Gassiplex_3(const Char_t * SevtName, const Char_t * SevtTitle = "", Int_t Crate = -1);	// create a new Gassiplex subevent
		~TMrbSubevent_Gassiplex_3() {};	 			// remove current subevent from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, 		// generate part of code for this subevent
											TMrbTemplate & Template, const Char_t * Prefix = NULL);

		inline Bool_t HasFixedLengthFormat() const { return(kFALSE); };				// variable length data
		inline Bool_t AllowsMultipleModules() const { return(kTRUE); };				// can store multiple modules

		inline Bool_t CheckModuleID(TMrbModule * Module) const {				// needs modules of type Gassiplex
			return (Module->CheckID(TMrbConfig::kModuleGassiplex));
		};

		inline Bool_t NeedsHitBuffer() const { return(kTRUE); };  					// allocate hit buffer
		inline Bool_t HasPrivateCode() const { return(kTRUE); }; 					// use private code files
		inline Bool_t NeedsBranchMode() const { return(kTRUE); }; 					// needs branch mode
		inline const Char_t * GetCommonCodeFile() { return("Subevent_Gassiplex_Common"); };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	ClassDef(TMrbSubevent_Gassiplex_3, 1) 	// [Config] Subevent type [10,103]: a format dedicated to Gassiplex modules
};

#endif

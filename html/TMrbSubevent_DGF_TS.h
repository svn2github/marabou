#ifndef __TMrbSubevent_DGF_TS_h__
#define __TMrbSubevent_DGF_TS_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSubevent_DGF_TS.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSubevent_DGF_TS   -- MBS subevent type [10,23]
//                                          dedicated format for XIA DGF-4C modules
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <iostream.h>
#include <strstream.h>
#include <iomanip.h>
#include <fstream.h>

#include "Rtypes.h"
#include "TSystem.h"

#include "TMrbSubevent.h"
#include "TMrbTemplate.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent_DGF_TS
// Purpose:        Define MBS subevents of type [10,23]
// Methods:        MakeReadoutCode  -- output readout code for subevent [10,23]
// Description:    Defines a MBS subevent of type [10,23] - a special format
//                 reflecting DGF's data structure.
//                 Generates code.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSubevent_DGF_TS : public TMrbSubevent {

	public:

		TMrbSubevent_DGF_TS() {}; 				// default ctor
		TMrbSubevent_DGF_TS(const Char_t * SevtName, const Char_t * SevtTitle = "", Int_t Crate = -1);	// create a new DGF subevent
		~TMrbSubevent_DGF_TS() {};	 			// remove current subevent from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, 		// generate part of code for this subevent
											TMrbTemplate & Template, const Char_t * Prefix = NULL);

		inline Bool_t HasFixedLengthFormat() { return(kFALSE); };				// variable length data
		inline Bool_t AllowsMultipleModules() { return(kTRUE); };				// can store multiple modules

		inline Bool_t CheckModuleID(TMrbModule * Module) {						// needs modules of type XIA DGF-4C
			return(Module->CheckID(TMrbConfig::kModuleXia_DGF_4C));
		};
				
		inline Bool_t NeedsHitBuffer() { return(kTRUE); };  					// allocate hit buffer
		inline Bool_t ProvidesEventTime() { return(kTRUE); };						// has event time buffer
		inline Bool_t HasPrivateCode() { return(kTRUE); }; 						// use private code files
		inline Bool_t NeedsBranchMode() { return(kTRUE); }; 					// needs branch mode
		inline const Char_t * GetCommonCodeFile() { return("Subevent_TS_Common"); };
				
		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbSubevent_DGF_TS.html&"); };

	ClassDef(TMrbSubevent_DGF_TS, 1) 	// [Config] Subevent type [10,23]: a format dedicated to XIA DGF-4C modules
};

#endif

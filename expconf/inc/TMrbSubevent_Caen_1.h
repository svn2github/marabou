#ifndef __TMrbSubevent_Caen_1_h__
#define __TMrbSubevent_Caen_1_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSubevent_Caen_1.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSubevent_Caen_1   -- MBS subevent type [10,41]
//                                          dedicated format CAEN modules
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
#include <fstream.h>

#include "Rtypes.h"
#include "TSystem.h"

#include "TMrbSubevent.h"
#include "TMrbTemplate.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent_Caen_1
// Purpose:        Define MBS subevents of type [10,41]
// Methods:        MakeReadoutCode  -- output readout code for subevent [10,41]
// Description:    Defines a MBS subevent of type [10,41] - a special format
//                 reflecting CAEN's data structure.
//                 Generates code.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSubevent_Caen_1 : public TMrbSubevent {

	public:

		TMrbSubevent_Caen_1() {}; 				// default ctor
		TMrbSubevent_Caen_1(const Char_t * SevtName, const Char_t * SevtTitle = "", Int_t Crate = -1);	// create a new DGF subevent
		~TMrbSubevent_Caen_1() {};	 			// remove current subevent from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, 		// generate part of code for this subevent
											TMrbTemplate & Template, const Char_t * Prefix = NULL);

		inline Bool_t HasFixedLengthFormat() { return(kFALSE); };				// variable length data
		inline Bool_t AllowsMultipleModules() { return(kTRUE); };				// can store multiple modules

		inline Bool_t CheckModuleID(TMrbModule * Module) {						// needs modules of type CAEN
			return(Module->CheckID(TMrbConfig::kModuleCaenV775) || Module->CheckID(TMrbConfig::kModuleCaenV785));
		};
		
		inline Bool_t HasPrivateCode() { return(kTRUE); }; 						// use private code files
		
		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbSubevent_Caen_1.html&"); };

	ClassDef(TMrbSubevent_Caen_1, 1) 	// [Config] Subevent type [10,41]: a format dedicated to CAEN modules
};

#endif

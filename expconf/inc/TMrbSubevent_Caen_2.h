#ifndef __TMrbSubevent_Caen_2_h__
#define __TMrbSubevent_Caen_2_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSubevent_Caen_2.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSubevent_Caen_2   -- MBS subevent type [10,42]
//                                         dedicated format for CAEN modules
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbSubevent_Caen_2.h,v 1.5 2004-09-28 13:47:32 rudi Exp $       
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
// Name:           TMrbSubevent_Caen_2
// Purpose:        Define MBS subevents of type [10,42]
// Methods:        MakeReadoutCode  -- output readout code for subevent [10,42]
// Description:    Defines a MBS subevent of type [10,42] - a special format
//                 reflecting CAEN's data structure.
//                 Generates code.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSubevent_Caen_2 : public TMrbSubevent {

	public:

		TMrbSubevent_Caen_2() {}; 				// default ctor
		TMrbSubevent_Caen_2(const Char_t * SevtName, const Char_t * SevtTitle = "", Int_t Crate = -1);	// create a new DGF subevent
		~TMrbSubevent_Caen_2() {};	 			// remove current subevent from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, 		// generate part of code for this subevent
											TMrbTemplate & Template, const Char_t * Prefix = NULL);

		inline Bool_t HasFixedLengthFormat() const { return(kFALSE); };				// variable length data
		inline Bool_t AllowsMultipleModules() const { return(kTRUE); };				// can store multiple modules

		inline Bool_t CheckModuleID(TMrbModule * Module) const {						// needs modules of type CAEN
			return(Module->CheckID(TMrbConfig::kModuleCaenV775) || Module->CheckID(TMrbConfig::kModuleCaenV785));
		};
				
		inline Bool_t NeedsHitBuffer() const { return(kTRUE); };  					// allocate hit buffer
		inline Bool_t HasPrivateCode() const { return(kTRUE); }; 						// use private code files
		inline Bool_t NeedsBranchMode() const { return(kTRUE); }; 					// needs branch mode
		inline const Char_t * GetCommonCodeFile() const { return("Subevent_Caen_Common"); };
		
		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbSubevent_Caen_2.html&"); };

	ClassDef(TMrbSubevent_Caen_2, 1) 	// [Config] Subevent type [10,42]: a format dedicated to CAEN modules
};

#endif

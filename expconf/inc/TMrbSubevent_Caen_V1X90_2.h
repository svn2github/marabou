#ifndef __TMrbSubevent_Caen_V1X90_2_h__
#define __TMrbSubevent_Caen_V1X90_2_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSubevent_Caen_V1X90_2.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSubevent_Caen_V1X90_2   -- MBS subevent type [10,46]
//                                         dedicated format for CAEN modules
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbSubevent_Caen_V1X90_2.h,v 1.1 2009-07-20 07:09:44 Marabou Exp $       
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
// Name:           TMrbSubevent_Caen_V1X90_2
// Purpose:        Define MBS subevents of type [10,46]
// Methods:        MakeReadoutCode  -- output readout code for subevent [10,46]
// Description:    Defines a MBS subevent of type [10,46] - a special format
//                 reflecting data structure of Caen TDCs V1X90.
//                 Generates code.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSubevent_Caen_V1X90_2 : public TMrbSubevent {

	public:

		TMrbSubevent_Caen_V1X90_2() {}; 				// default ctor
		TMrbSubevent_Caen_V1X90_2(const Char_t * SevtName, const Char_t * SevtTitle = "", Int_t Crate = -1);	// create a new CAEN subevent
		~TMrbSubevent_Caen_V1X90_2() {};	 			// remove current subevent from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, 		// generate part of code for this subevent
											TMrbTemplate & Template, const Char_t * Prefix = NULL);

		inline Bool_t HasFixedLengthFormat() const { return(kFALSE); };				// variable length data
		inline Bool_t AllowsMultipleModules() const { return(kTRUE); };				// can store multiple modules

		inline Bool_t CheckModuleID(TMrbModule * Module) const {					// needs modules of type CAEN V1X90
			return(Module->CheckID(TMrbConfig::kModuleCaenV1X90));
		};
		
		inline Bool_t NeedsHitBuffer() const { return(kTRUE); };  					// allocate hit buffer
		inline Bool_t HasPrivateCode() const { return(kTRUE); }; 						// use private code files
		inline Bool_t NeedsBranchMode() const { return(kTRUE); }; 					// needs branch mode
		inline const Char_t * GetCommonCodeFile() const { return("Subevent_Caen_V1X90_Common"); };
		
		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	ClassDef(TMrbSubevent_Caen_V1X90_2, 1) 	// [Config] Subevent type [10,46]: a format dedicated to CAEN V1X90 modules
};

#endif

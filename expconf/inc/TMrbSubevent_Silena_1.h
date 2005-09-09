#ifndef __TMrbSubevent_Silena_1_h__
#define __TMrbSubevent_Silena_1_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSubevent_Silena_1.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSubevent_Silena_1 -- MBS subevent type [10,31]
//                                          dedicated format for Silena 4418 ADCs
//                                          running in zero-compressed mode
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbSubevent_Silena_1.h,v 1.6 2005-09-09 06:59:14 Rudolf.Lutter Exp $       
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
// Name:           TMrbSubevent_Silena_1
// Purpose:        Define MBS subevents of type [10,31]
// Methods:        MakeReadoutCode  -- output readout code for subevent [10,31]
// Description:    Defines a MBS subevent of type [10,31] - a special format
//                 to be used for Silena 4418 ADCs running in zero-compression mode.
//                 Generates code.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSubevent_Silena_1 : public TMrbSubevent {

	public:

		TMrbSubevent_Silena_1() {}; 				// default ctor
		TMrbSubevent_Silena_1(const Char_t * SevtName, const Char_t * SevtTitle = "", Int_t Crate = -1);	// create a new Silena subevent
		~TMrbSubevent_Silena_1() {};	 			// remove current subevent from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, 		// generate part of code for this subevent
											TMrbTemplate & Template, const Char_t * Prefix = NULL);

		inline Bool_t HasFixedLengthFormat() const { return(kFALSE); };				// variable length data
		inline Bool_t AllowsMultipleModules() const { return(kTRUE); };				// can store multiple modules

		inline Bool_t CheckModuleID(TMrbModule * Module) const {						// needs modules of type XIA DGF-4C
			return(Module->CheckID(TMrbConfig::kModuleSilena4418V) || Module->CheckID(TMrbConfig::kModuleSilena4418T));
		};
		
		inline Bool_t HasPrivateCode() const { return(kTRUE); }; 						// use private code files
		
		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	ClassDef(TMrbSubevent_Silena_1, 1) 	// [Config] Subevent type [10,31]: format for Silena 4418 ADCs with zero suppression
};

#endif

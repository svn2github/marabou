#ifndef __TMrbSubevent_Sis_3300_h__
#define __TMrbSubevent_Sis_3300_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSubevent_Sis_3300.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSubevent_Sis_3300   -- MBS subevent type [10,54]
//                                            dedicated format for Sis modules
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbSubevent_Sis_3302.h,v 1.1 2010-12-14 11:13:39 Marabou Exp $       
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
// Name:           TMrbSubevent_Sis_3300
// Purpose:        Define MBS subevents of type [10,54]
// Methods:        MakeReadoutCode  -- output readout code for subevent [10,54]
// Description:    Defines a MBS subevent of type [10,54] - a special format
//                 reflecting DGF's data structure.
//                 Generates code.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSubevent_Sis_3300 : public TMrbSubevent {

	public:

		TMrbSubevent_Sis_3300() {}; 				// default ctor
		TMrbSubevent_Sis_3300(const Char_t * SevtName, const Char_t * SevtTitle = "", Int_t Crate = -1);	// create a new SIS subevent
		~TMrbSubevent_Sis_3300() {};	 			// remove current subevent from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, 		// generate part of code for this subevent
											TMrbTemplate & Template, const Char_t * Prefix = NULL);

		virtual Bool_t MakeSpecialAnalyzeCode(ofstream & AnaStrm, TMrbConfig::EMrbAnalyzeTag TagIndex, TMrbTemplate & Template);  // generate part of code

		inline Bool_t HasFixedLengthFormat() const { return(kFALSE); };				// variable length data
		inline Bool_t AllowsMultipleModules() const { return(kFALSE); };			// one module only

		inline Bool_t CheckModuleID(TMrbModule * Module) const {						// needs modules of type Sis
			return(Module->CheckID(TMrbConfig::kModuleSis_3300));
		};
				
		inline Bool_t HasPrivateCode() const { return(kTRUE); }; 						// use private code files
		inline const Char_t * GetCommonCodeFile() const { return("Subevent_Sis3300_Common"); };
		
		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	ClassDef(TMrbSubevent_Sis_3300, 1) 	// [Config] Subevent type [10,54]: a format dedicated to SIS modules
};

#endif

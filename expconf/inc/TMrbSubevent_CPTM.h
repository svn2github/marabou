#ifndef __TMrbSubevent_CPTM_h__
#define __TMrbSubevent_CPTM_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSubevent_CPTM.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSubevent_CPTM   -- MBS subevent type [10,61]
//                                        dedicated format for C_PTM modules
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbSubevent_CPTM.h,v 1.1 2005-04-21 07:24:11 rudi Exp $       
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
// Name:           TMrbSubevent_CPTM
// Purpose:        Define MBS subevents of type [10,61]
// Methods:        MakeReadoutCode  -- output readout code for subevent [10,61]
// Description:    Defines a MBS subevent of type [10,61] - a special format
//                 to be used with C_PTM modules
//                 Generates code.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSubevent_CPTM : public TMrbSubevent {

	public:

		TMrbSubevent_CPTM() {}; 				// default ctor
		TMrbSubevent_CPTM(const Char_t * SevtName, const Char_t * SevtTitle = "", Int_t Crate = -1);	// create a new DGF subevent
		~TMrbSubevent_CPTM() {};	 			// remove current subevent from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, 		// generate part of code for this subevent
											TMrbTemplate & Template, const Char_t * Prefix = NULL);

		inline Bool_t HasFixedLengthFormat() const { return(kFALSE); };				// variable length data
		inline Bool_t AllowsMultipleModules() const { return(kTRUE); };				// can store multiple modules

		inline Bool_t CheckModuleID(TMrbModule * Module) const { return(Module->CheckID(TMrbConfig::kModuleCologne_CPTM)); };
				
		inline Bool_t NeedsHitBuffer() const { return(kTRUE); };  					// allocate hit buffer
		inline Bool_t HasPrivateCode() const { return(kTRUE); }; 						// use private code files
		inline Bool_t NeedsBranchMode() const { return(kTRUE); }; 					// needs branch mode
		
		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbSubevent_CPTM.html&"); };

	ClassDef(TMrbSubevent_CPTM, 1) 	// [Config] Subevent type [10,61]: a format used by C_PTM modules
};

#endif

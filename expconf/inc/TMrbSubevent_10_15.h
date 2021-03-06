#ifndef __TMrbSubevent_10_15_h__
#define __TMrbSubevent_10_15_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSubevent_10_15.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSubevent_10_15   -- MBS subevent type [10,15]
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbSubevent_10_15.h,v 1.2 2005-09-09 06:59:14 Rudolf.Lutter Exp $       
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
// Name:           TMrbSubevent_10_15
// Purpose:        Define MBS subevents of type [10,11]
// Description:    Defines a MBS subevent of type [10,15]. Generates code.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSubevent_10_15 : public TMrbSubevent {

	public:

		TMrbSubevent_10_15() {}; 				// default ctor
		TMrbSubevent_10_15(const Char_t * SevtName, const Char_t * SevtTitle = "", Int_t Crate = -1);	// create a new [10,13] subevent
		~TMrbSubevent_10_15() {};	 			// remove current subevent from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, 	// generate part of code for this subevent
										TMrbTemplate & Template, const Char_t * Prefix = NULL);

		inline Bool_t HasFixedLengthFormat() const { return(kFALSE); };		// variable length data
		inline Bool_t AllowsMultipleModules() const { return(kTRUE); };		// several modules per buffer
		inline Bool_t HasPrivateCode() const { return(kTRUE); }; 			// use private code files

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	ClassDef(TMrbSubevent_10_15, 1) 	// [Config] Subevent type [10,15]: raw data, buffered
};

#endif

#ifndef __TMrbSubevent_10_12_h__
#define __TMrbSubevent_10_12_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSubevent_10_12.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSubevent_10_12   -- MBS subevent type [10,12]
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
// Name:           TMrbSubevent_10_12
// Purpose:        Define MBS subevents of type [10,12]
// Methods:        MakeReadoutCode  -- output readout code for subevent [10,12]
// Description:    Defines a MBS subevent of type [10,12]. Generates code.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSubevent_10_12 : public TMrbSubevent {

	public:

		TMrbSubevent_10_12() {}; 				// default ctor
		TMrbSubevent_10_12(const Char_t * SevtName, const Char_t * SevtTitle = "", Int_t Crate = -1);	// create a new [10,12] subevent
		~TMrbSubevent_10_12() {};	 			// remove current subevent from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, 		// generate part of code for this subevent
											TMrbTemplate & Template, const Char_t * Prefix = NULL);

		inline Bool_t HasFixedLengthFormat() { return(kFALSE); };		// variable length data
		inline Bool_t AllowsMultipleModules() { return(kTRUE); };		// can store multiple modules

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbSubevent_10_12.html&"); };

	ClassDef(TMrbSubevent_10_12, 1) 	// [Config] Subevent type [10,12]: data with channel id, module headers
};

#endif

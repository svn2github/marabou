#ifndef __TMrbSubevent_10_13_h__
#define __TMrbSubevent_10_13_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSubevent_10_13.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSubevent_10_13    -- MBS subevent type [10,13]
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       
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
// Name:           TMrbSubevent_10_13
// Purpose:        Define MBS subevents of type [10,13]
// Description:    Defines a MBS subevent of type [10,13]. Generates code.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSubevent_10_13 : public TMrbSubevent {

	public:

		TMrbSubevent_10_13() {}; 				// default ctor
		TMrbSubevent_10_13(const Char_t * SevtName, const Char_t * SevtTitle = "", Int_t Crate = -1);	// create a new [10,13] subevent
		~TMrbSubevent_10_13() {};	 			// remove current subevent from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, 	// generate part of code for this subevent
										TMrbTemplate & Template, const Char_t * Prefix = NULL);

		inline Bool_t HasFixedLengthFormat() const { return(kFALSE); };		// variable length data
		inline Bool_t AllowsMultipleModules() const { return(kFALSE); };		// one module per subevent only
		inline Bool_t HasPrivateCode() const { return(kTRUE); }; 				// use private code files
		inline const Char_t * GetCommonCodeFile() const { return("Subevent_HB_Common"); };
		inline Bool_t NeedsHitBuffer() const { return(kTRUE); };						// allocate hit buffer
		inline Bool_t NeedsBranchMode() const { return(kTRUE); }; 					// needs branch mode

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbSubevent_10_13.html&"); };

	ClassDef(TMrbSubevent_10_13, 1)		// [Config] Subevent type [10,13]: data with channel id & time stamp
};

#endif

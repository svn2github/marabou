#ifndef __TMrbSubevent_10_11_h__
#define __TMrbSubevent_10_11_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSubevent_10_11.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSubevent_10_11   -- MBS subevent type [10,11]
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
// Name:           TMrbSubevent_10_11
// Purpose:        Define MBS subevents of type [10,11]
// Description:    Defines a MBS subevent of type [10,11]. Generates code.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSubevent_10_11 : public TMrbSubevent {

	public:

		TMrbSubevent_10_11() {};								// default ctor
		TMrbSubevent_10_11(const Char_t * SevtName, const Char_t * SevtTitle = "", Int_t Crate = -1);	// create a new [10,11] subevent
		~TMrbSubevent_10_11() {};	 							// remove current subevent from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex,  	// generate part of code for this subevent
														TMrbTemplate & Template, const Char_t * Prefix = NULL);

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbSubevent_10_11.html&"); };

	ClassDef(TMrbSubevent_10_11, 1) 	// [Config] Subevent type [10,11]: data without channel id, zero-padded
};

#endif

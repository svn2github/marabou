#ifndef __TMrbEvent_10_1_h__
#define __TMrbEvent_10_1_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbEvent_10_1.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbEvent_10_1       -- MBS event type [10,1]
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
#include <fstream>

#include "Rtypes.h"
#include "TSystem.h"

#include "TMrbEvent.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEvent_10_1
// Purpose:        Define MBS events of type [10,1]
// Description:    Defines a MBS event of type [10,1]. Generates code.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbEvent_10_1 : public TMrbEvent {

	public:

		TMrbEvent_10_1() {}; 				// default ctor

		TMrbEvent_10_1(Int_t TrigNumber, const Char_t * EvtName = "", const Char_t * EvtTitle = "");	// explicit ctor
		~TMrbEvent_10_1() {};	 			// remove current subevent from list

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbEvent_10_1.html&"); };

	ClassDef(TMrbEvent_10_1, 1) 	// [Config] Event type [10,1]: standard MBS event
};

#endif

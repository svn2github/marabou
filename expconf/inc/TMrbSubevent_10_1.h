#ifndef __TMrbSubevent_10_1_h__
#define __TMrbSubevent_10_1_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSubevent_10_1.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSubevent_10_1    -- MBS subevent type [10,1]
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbSubevent_10_1.h,v 1.6 2005-09-09 06:59:14 Rudolf.Lutter Exp $       
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
// Name:           TMrbSubevent_10_1
// Purpose:        Define MBS subevents of type [10,1]
// Description:    Defines a MBS subevent of type [10,1]. Generates code.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSubevent_10_1 : public TMrbSubevent {

	public:

		TMrbSubevent_10_1() {}; 				// default ctor
		TMrbSubevent_10_1(const Char_t * SevtName, const Char_t * SevtTitle = "", Int_t Crate = -1);	// create a new [10,1] subevent
		~TMrbSubevent_10_1() {};	 			// remove current subevent from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, 	// generate part of code for this subevent
										TMrbTemplate & Template, const Char_t * Prefix = NULL);

		inline Bool_t HasFixedLengthFormat() const { return(kFALSE); };		// variable length data
		inline Bool_t AllowsMultipleModules() const { return(kFALSE); };		// one module per subevent only

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	ClassDef(TMrbSubevent_10_1, 1)		// [Config] Subevent type [10,1]: data with channel id
};

#endif

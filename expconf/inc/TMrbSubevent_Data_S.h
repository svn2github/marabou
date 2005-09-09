#ifndef __TMrbSubevent_Data_S_h__
#define __TMrbSubevent_Data_S_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSubevent_Data_S.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSubevent_Data_S    -- MBS subevent type [10,64]
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbSubevent_Data_S.h,v 1.8 2005-09-09 06:59:14 Rudolf.Lutter Exp $       
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
// Name:           TMrbSubevent_Data_S
// Purpose:        Define MBS subevents of type [10,64]
// Description:    Defines a MBS subevent of type [10,64]. Generates code.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSubevent_Data_S : public TMrbSubevent {

	public:

		TMrbSubevent_Data_S() {}; 				// default ctor
		TMrbSubevent_Data_S(const Char_t * SevtName, const Char_t * SevtTitle = "", Int_t NofWords = 1000, Int_t Crate = -1);	// create a new [10,64] subevent
		~TMrbSubevent_Data_S() {};	 			// remove current subevent from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, 	// generate part of code for this subevent
										TMrbTemplate & Template, const Char_t * Prefix = NULL);

		inline Bool_t HasFixedLengthFormat() const { return(kFALSE); };		// variable length data
		inline Bool_t AllowsMultipleModules() const { return(kFALSE); };		// has no meaning in this context
		inline Bool_t NeedsModulesToBeAssigned() const { return(kFALSE); }; 	// no modules needed
		inline Int_t GetNofWords() const { return(fNofWords); };				// number of data words
		
		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	public:
		Int_t fNofWords;			// number of data words
	
	ClassDef(TMrbSubevent_Data_S, 1)		// [Config] Subevent type [10,64]: plain integer data (16 bit)
};

#endif

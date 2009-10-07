#ifndef __TMrbSubevent_Caen_V556_1_h__
#define __TMrbSubevent_Caen_V556_1_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSubevent_Caen_V556_1.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSubevent_Caen_V556_1   -- MBS subevent type [10,33]
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbSubevent_Caen_V556_1.h,v 1.1 2009-10-07 08:49:31 Rudolf.Lutter Exp $
// Date:           $Date: 2009-10-07 08:49:31 $
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
// Name:           TMrbSubevent_Caen_V556_1
// Purpose:        Define MBS subevents of type [10,33]
// Methods:        MakeReadoutCode  -- output readout code for subevent [10,33]
// Description:    Defines a MBS subevent of type [10,33] - a special format
//                 dedicated to Caen V556 ADCs.
//                 Generates code.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSubevent_Caen_V556_1 : public TMrbSubevent {

	public:

		TMrbSubevent_Caen_V556_1() {}; 				// default ctor
		TMrbSubevent_Caen_V556_1(const Char_t * SevtName, const Char_t * SevtTitle = "", Int_t Crate = -1);	// create a new CAEN subevent
		~TMrbSubevent_Caen_V556_1() {};	 			// remove current subevent from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, 		// generate part of code for this subevent
											TMrbTemplate & Template, const Char_t * Prefix = NULL);

		inline Bool_t HasFixedLengthFormat() const { return(kFALSE); };				// variable length data
		inline Bool_t AllowsMultipleModules() const { return(kTRUE); };				// can store multiple modules

		inline Bool_t CheckModuleID(TMrbModule * Module) const {					// needs modules of type CAEN V556
			return(Module->CheckID(TMrbConfig::kModuleCaenV556));
		};

		inline Bool_t HasPrivateCode() const { return(kTRUE); }; 						// use private code files

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	ClassDef(TMrbSubevent_Caen_V556_1, 1) 	// [Config] Subevent type [10,45]: a format dedicated to CAEN V556 modules
};

#endif

#ifndef __TMrbSubevent_Madc_2_h__
#define __TMrbSubevent_Madc_2_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSubevent_Madc_2.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSubevent_Madc_2   -- MBS subevent type [10,82]
//                                          dedicated format Mesytec MADC32 modules
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbSubevent_Madc_2.h,v 1.2 2009-06-24 13:59:12 Rudolf.Lutter Exp $
// Date:           $Date: 2009-06-24 13:59:12 $
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
// Name:           TMrbSubevent_Madc_2
// Purpose:        Define MBS subevents of type [10,82]
// Methods:        MakeReadoutCode  -- output readout code for subevent [10,82]
// Description:    Defines a MBS subevent of type [10,82] - a special format
//                 reflecting Mesytec MADC32' data structure.
//                 Generates code.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSubevent_Madc_2 : public TMrbSubevent {

	public:

		TMrbSubevent_Madc_2() {}; 				// default ctor
		TMrbSubevent_Madc_2(const Char_t * SevtName, const Char_t * SevtTitle = "", Int_t Crate = -1);	// create a new Mesytec subevent
		~TMrbSubevent_Madc_2() {};	 			// remove current subevent from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, 		// generate part of code for this subevent
											TMrbTemplate & Template, const Char_t * Prefix = NULL);

		inline Bool_t HasFixedLengthFormat() const { return(kFALSE); };				// variable length data
		inline Bool_t AllowsMultipleModules() const { return(kTRUE); };				// can store multiple modules

		inline Bool_t CheckModuleID(TMrbModule * Module) const {					// needs modules of type MADC32
			return(Module->CheckID(TMrbConfig::kModuleMesytecMadc32));
		};

		inline Bool_t NeedsHitBuffer() const { return(kTRUE); };  					// allocate hit buffer
		inline Bool_t HasPrivateCode() const { return(kTRUE); }; 					// use private code files
		inline Bool_t NeedsBranchMode() const { return(kTRUE); }; 					// needs branch mode
		inline const Char_t * GetCommonCodeFile() { return("Subevent_Madc_Common"); };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	ClassDef(TMrbSubevent_Madc_2, 1) 	// [Config] Subevent type [10,82]: a format dedicated to Mesytec MADC32 modules
};

#endif

#ifndef __TMrbSubeventRaw_h__
#define __TMrbSubeventRaw_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSubeventRaw.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSubeventRaw    -- a MBS subevent having raw (used-defined) data
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbSubeventRaw.h,v 1.5 2004-09-28 13:47:32 rudi Exp $       
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

#include "TMrbConfig.h"
#include "TMrbSubevent.h"
#include "TMrbTemplate.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubeventRaw
// Purpose:        Define a user-defined subevent
// Description:    Defines a subevent having any structure. Generates code.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSubeventRaw : public TMrbSubevent {

	public:

		TMrbSubeventRaw() {}; 														// default ctor
		TMrbSubeventRaw(const Char_t * SevtName, const Char_t * SevtTitle, 			// ctor
												UInt_t SevtType, UInt_t SevtSubtype,
												Option_t * LegalDataTypes = "I",
												Bool_t CreateSerial = kFALSE);
		~TMrbSubeventRaw() {};	 													// remove current subevent from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, 	// generate part of code for this subevent
										TMrbTemplate & Template, const Char_t * Prefix = NULL);

		inline Bool_t HasFixedLengthFormat() const { return(kFALSE); };		// variable length data
		inline Bool_t AllowsMultipleModules() const { return(kFALSE); };		// one module per subevent only
		inline Bool_t SerialToBeCreated() const { return(fSerialToBeCreated); }	// create unique serial internally?

		virtual inline Bool_t IsRaw() const { return(kTRUE); };				// indicates raw (userr-defined) mode

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbSubeventRaw.html&"); };

	protected:
		Bool_t fSerialToBeCreated;

	ClassDef(TMrbSubeventRaw, 1)		// [Config] A "raw" (user-defined) subevent
};

#endif

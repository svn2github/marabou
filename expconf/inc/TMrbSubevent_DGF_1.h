#ifndef __TMrbSubevent_DGF_1_h__
#define __TMrbSubevent_DGF_1_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSubevent_DGF_1.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSubevent_DGF_1   -- MBS subevent type [10,21]
//                                         dedicated format for XIA DGF-4C modules
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
// Name:           TMrbSubevent_DGF_1
// Purpose:        Define MBS subevents of type [10,21]
// Methods:        MakeReadoutCode  -- output readout code for subevent [10,21]
// Description:    Defines a MBS subevent of type [10,21] - a special format
//                 reflecting DGF's data structure.
//                 Generates code.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSubevent_DGF_1 : public TMrbSubevent {

	public:

		TMrbSubevent_DGF_1() {}; 				// default ctor
		TMrbSubevent_DGF_1(const Char_t * SevtName, const Char_t * SevtTitle = "", Int_t Crate = -1);	// create a new DGF subevent
		~TMrbSubevent_DGF_1() {};	 			// remove current subevent from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, 		// generate part of code for this subevent
											TMrbTemplate & Template, const Char_t * Prefix = NULL);

		Bool_t MakeRcFile(ofstream & RcStrm, TMrbConfig::EMrbRcFileTag TagIndex, const Char_t * ResourceName);
		
		inline Bool_t HasFixedLengthFormat() const { return(kFALSE); };				// variable length data
		inline Bool_t AllowsMultipleModules() const { return(kTRUE); };				// can store multiple modules

		inline Bool_t CheckModuleID(TMrbModule * Module) const {						// needs modules of type XIA DGF-4C
			return(Module->CheckID(TMrbConfig::kModuleXia_DGF_4C));
		};
		
		inline void SetClusterID(Int_t Number, Int_t Serial, const Char_t * Color) {
			fClusterNumber = Number;
			fClusterID.Set(Serial, Color);
		};
		inline Int_t GetClusterNumber() const { return(fClusterNumber); };
		inline Int_t GetClusterSerial() const { return(fClusterID.GetIndex()); }; 		// use TMrbNamedX object:
		inline const Char_t * GetClusterColor() const { return(fClusterID.GetName()); };	// index <- serial, name <- color

		inline Bool_t HasPrivateCode() const { return(kTRUE); }; 						// use private code files
		inline const Char_t * GetCommonCodeFile() const { return("Subevent_DGF_1_Common"); };
		
		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbSubevent_DGF_1.html&"); };

	protected:
		Int_t fClusterNumber;			// cluster number
		TMrbNamedX fClusterID;			// cluster serial & color

	ClassDef(TMrbSubevent_DGF_1, 1) 	// [Config] Subevent type [10,21]: a format dedicated to XIA DGF-4C modules
};

#endif

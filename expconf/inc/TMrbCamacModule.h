#ifndef __TMrbCamacModule_h__
#define __TMrbCamacModule_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbCamacModule.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbCamacModule      -- base class for camac modules
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

#include "TMrbTemplate.h"
#include "TMrbCNAF.h"
#include "TMrbModule.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCamacModule
// Purpose:        Define methods for a camac module
// Description:    Defines a camac module to be used in the MARaBOU environment.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbCamacModule : public TMrbModule {

	public:

		TMrbCamacModule() {};								// default ctor

															// explicit ctor:	name, id, position
															//					subdevice, nofchannels, range
		TMrbCamacModule(const Char_t * ModuleName, const Char_t * ModuleID,
								const Char_t * ModulePosition, Int_t SubDevice, Int_t NofChannels, Int_t Range);

		~TMrbCamacModule() {};								// default dtor

		inline const Char_t * GetPosition() {				// get B.C.N data
			return(fCNAF.Int2Ascii(kTRUE));
		};

		inline Int_t GetCNAF(TMrbCNAF::EMrbCNAF cnaf) {		// get single cnaf member
			return(fCNAF.Get(cnaf));
		};

		inline Int_t GetStation() { return(fCNAF.Get(TMrbCNAF::kCnafStation)); };
		
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, TMrbTemplate & Template, const Char_t * Prefix = NULL); // generate readout code

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbCamacModule.html&"); };

		void Print(ostream & OutStrm, const Char_t * Prefix = "");		// show data
		virtual inline void Print() { Print(cout, ""); };

	protected:
		TMrbCNAF fCNAF;										// module position

	ClassDef(TMrbCamacModule, 1)		// [Config] Base class for CAMAC modules
};	

#endif

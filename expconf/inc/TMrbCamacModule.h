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

namespace std {} using namespace std;

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

		inline const Char_t * GetPosition() const {				// get B.C.N data
			TString ca; return(fCNAF.Int2Ascii(ca, kTRUE));
		};

		inline Int_t GetCNAF(TMrbCNAF::EMrbCNAF cnaf) const {		// get single cnaf member
			return(fCNAF.Get(cnaf));
		};

		inline Int_t GetStation() const { return(fCNAF.Get(TMrbCNAF::kCnafStation)); };
		
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, TMrbTemplate & Template, const Char_t * Prefix = NULL); // generate readout code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex) { return(kFALSE); };  	// generate code for given channel
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TObject * Subevent, Int_t Value = 0) { return(kFALSE); }; 

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbCamacModule.html&"); };

		void Print(Option_t * Option) const { TObject::Print(Option); }
		void Print(ostream & OutStrm, const Char_t * Prefix = "") const;		// show data
		virtual inline void Print() const { Print(cout, ""); };

	protected:
		TMrbCNAF fCNAF;										// module position

	ClassDef(TMrbCamacModule, 1)		// [Config] Base class for CAMAC modules
};	

#endif

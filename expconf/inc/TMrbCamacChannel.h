#ifndef __TMrbCamacChannel_h__
#define __TMrbCamacChannel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbCamacChannel.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbModuleChannel    -- base class to describe a camac channel
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <iostream.h>
#include <strstream.h>
#include <iomanip.h>
#include <fstream.h>

#include "Rtypes.h"
#include "TSystem.h"

#include "TMrbModuleChannel.h"
#include "TMrbCamacModule.h"
#include "TMrbCNAF.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCamacChannel
// Purpose:        Define methods for a camac channel
// Description:    Describes a single exp parameter.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbCamacChannel : public TMrbModuleChannel {

	public:

		TMrbCamacChannel() {};					// default ctor

		TMrbCamacChannel(TMrbCamacModule * Module, Int_t Channel, TMrbCNAF & ModuleCNAF);	 // ctor: link channel to parent

		~TMrbCamacChannel() {};					// default dtor

		inline const Char_t * GetPosition() { 	// get B.C.N data
			return(fCNAF.Int2Ascii(kTRUE));
		};

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbCamacChannel.html&"); };

		void Print(ostream & OutStrm, Bool_t ArrayFlag, Bool_t SevtFlag, const Char_t * Prefix = "");	 	// show registers
		inline virtual void Print() { Print(cout, kFALSE, kFALSE, ""); };

	protected:
		TMrbCNAF fCNAF;							// channel position

	ClassDef(TMrbCamacChannel, 1)		// [Config] Base class describing a single CAMAC channel
};	

#endif

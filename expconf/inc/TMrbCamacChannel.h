#ifndef __TMrbCamacChannel_h__
#define __TMrbCamacChannel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbCamacChannel.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbModuleChannel    -- base class to describe a camac channel
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbCamacChannel.h,v 1.8 2005-09-09 06:59:13 Rudolf.Lutter Exp $       
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

		inline const Char_t * GetPosition(TString & Position) const { 	// get B.C.N data
			return(fCNAF.Int2Ascii(Position, kTRUE));
		};

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

		void Print(Option_t * Option) const { TObject::Print(Option); }
		void Print(ostream & OutStrm, Bool_t ArrayFlag, Bool_t SevtFlag, const Char_t * Prefix = "");	 	// show registers
		inline void Print() { Print(cout, kFALSE, kFALSE, ""); };

	protected:
		TMrbCNAF fCNAF;							// channel position

	ClassDef(TMrbCamacChannel, 1)		// [Config] Base class describing a single CAMAC channel
};	

#endif

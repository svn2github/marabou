#ifndef __TMrbVMEChannel_h__
#define __TMrbVMEChannel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbVMEChannel.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbVMEChannel    -- base class to describe a VME channel
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

#include "TMrbString.h"
#include "TMrbModuleChannel.h"
#include "TMrbVMEModule.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVMEChannel
// Purpose:        Define methods for VME channels
// Description:    Describes a single exp parameter.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbVMEChannel : public TMrbModuleChannel {

	public:

		TMrbVMEChannel() {};											// default ctor

		TMrbVMEChannel(TMrbVMEModule * Module, Int_t Channel);			// ctor: link channel to parent

		~TMrbVMEChannel() {};											// default dtor

		inline Int_t GetOffset() { return(fOffset); }; 					// get channel offset
		inline const Char_t * GetPosition() { return(fPosition.Data()); };	// crate & addr

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbVMEChannel.html&"); };

		void Print(ostream & OutStrm, Bool_t ArrayFlag, Bool_t SevtFlag, const Char_t * Prefix = "");	 	// show registers
		inline virtual void Print() { Print(cout, kFALSE, kFALSE, ""); };

	protected:
		Int_t fOffset;									// channel offset
		TMrbString fPosition;							// crate & addr

	ClassDef(TMrbVMEChannel, 1) 	// [Config] Base class for channels of a VME module
};	

#endif

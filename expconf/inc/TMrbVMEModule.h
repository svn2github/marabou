#ifndef __TMrbVMEModule_h__
#define __TMrbVMEModule_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbVMEModule.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbVMEModule        -- base class for vme modules
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
#include "TMrbTemplate.h"
#include "TMrbModule.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVMEModule
// Purpose:        Define methods for a VME module
// Description:    Defines a VME module to be used in the MARaBOU environment.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbVMEModule : public TMrbModule {

	public:

		TMrbVMEModule() {};										// default ctor

																// explicit ctor:	name, id, addr, modifier, segment
																//					nofchannels, range
		TMrbVMEModule(const Char_t * ModuleName, const Char_t * ModuleID,
							UInt_t BaseAddr, UInt_t AddrMod, Int_t SegLength,
							Int_t SubDevice, Int_t NofChannels, Int_t Range);

		~TMrbVMEModule() {};									// default dtor

		inline UInt_t GetBaseAddr() { return(fBaseAddr); };				// get base addr
		inline Int_t GetSegmentSize() { return(fSegmentSize); };		// segment size
		inline Int_t GetAddrModifier() { return(fAddrModifier); };		// addr modifier

		inline const Char_t * GetPosition() { return(fPosition.Data()); }; 	// crate & addr

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, 		// generate readout code
												TMrbTemplate & Template, const Char_t * Prefix = NULL);

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbVMEModule.html&"); };

		void Print(ostream & OutStrm, const Char_t * Prefix = "");		// show data
		virtual inline void Print() { Print(cout, ""); };

	protected:
		UInt_t fBaseAddr;					// base address
		UInt_t fAddrModifier;				// address modifier

		Int_t fSegmentSize; 				// segment size

		TMrbString fPosition;				// crate & addr

	ClassDef(TMrbVMEModule, 1)		// [Config] Base class for VME modules
};	

#endif

#ifndef __TMrbVMEModule_h__
#define __TMrbVMEModule_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbVMEModule.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbVMEModule        -- base class for vme modules
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbVMEModule.h,v 1.6 2004-09-28 13:47:32 rudi Exp $       
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

		inline UInt_t GetBaseAddr() const { return(fBaseAddr); };				// get base addr
		inline Int_t GetSegmentSize() const { return(fSegmentSize); };		// segment size
		inline Int_t GetAddrModifier() const { return(fAddrModifier); };		// addr modifier

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, 		// generate readout code
												TMrbTemplate & Template, const Char_t * Prefix = NULL);
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex) { return(kFALSE); };  	// generate code for given channel
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TObject * Subevent, Int_t Value = 0) { return(kFALSE); }; 

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbVMEModule.html&"); };

		void Print(Option_t * Option) const { TObject::Print(Option); }
		void Print(ostream & OutStrm, const Char_t * Prefix = "") const;		// show data
		virtual inline void Print() const { Print(cout, ""); };

	protected:
		UInt_t fBaseAddr;					// base address
		UInt_t fAddrModifier;				// address modifier

		Int_t fSegmentSize; 				// segment size

	ClassDef(TMrbVMEModule, 1)		// [Config] Base class for VME modules
};	

#endif

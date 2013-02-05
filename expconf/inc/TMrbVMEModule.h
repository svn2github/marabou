#ifndef __TMrbVMEModule_h__
#define __TMrbVMEModule_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbVMEModule.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbVMEModule        -- base class for vme modules
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbVMEModule.h,v 1.12 2011-05-18 11:04:49 Marabou Exp $
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
#include "TMrbModule.h"

class TMrbSubvent;

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

		inline void SetBaseAddr(UInt_t Base) { fBaseAddr = Base; };			// base address
		inline UInt_t GetBaseAddr() const { return(fBaseAddr); };

		inline void SetSegmentSize(Int_t SegSize) { fSegmentSize = SegSize; };		// segment size
		inline Int_t GetSegmentSize() const { return(fSegmentSize); };

		inline void SetAddrModifier(Int_t AddrMod) { fAddrModifier = AddrMod; };	// addr modifier
		inline Int_t GetAddrModifier() const { return(fAddrModifier); };

		virtual inline Bool_t UseLVME() const { return(kFALSE); };				// connect via LVME interface

		virtual inline Bool_t HasBlockXfer() const { return(kFALSE); };			// module is capable of using BLT

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, 		// generate readout code
												TMrbTemplate & Template, const Char_t * Prefix = NULL);
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex) { return(kFALSE); };  	// generate code for given channel
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TMrbSubevent * Subevent, Int_t Value = 0) { return(kFALSE); };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

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

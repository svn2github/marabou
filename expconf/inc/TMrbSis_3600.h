#ifndef __TMrbSis_3600_h__
#define __TMrbSis_3600_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSis_3600.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSis_3600        -- VME pattern unit
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbSis_3600.h,v 1.6 2005-09-09 06:59:14 Rudolf.Lutter Exp $       
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
#include "TObject.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSis_3600
// Purpose:        Define a VME module type SIS 3600
// Description:    Defines a VME module SIS 3600 (list mode)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSis_3600 : public TMrbVMEModule {

	public:
		enum				{	kSegSize		=	0x200	};
		enum				{	kAddrMod		=	0x39	};

		enum EMrbOffsets	{};

	public:

		TMrbSis_3600() {};  												// default ctor
		TMrbSis_3600(const Char_t * ModuleName, UInt_t BaseAddr, Int_t FifoDepth = 1); 	// define a new module
		~TMrbSis_3600() {};												// default dtor

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TObject * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() const { return("sis_3600"); }; 	// module mnemonic

		inline void SetFifoDepth(Int_t FifoDepth) { fFifoDepth = FifoDepth; };
		inline Int_t GetFifoDepth() const { return(fFifoDepth); };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		void DefineRegisters(); 							// define vme registers

	protected:
		Int_t fFifoDepth;			// fifo depth per channel

	ClassDef(TMrbSis_3600, 1)		// [Config] SIS 3600, VME pattern unit
};

#endif

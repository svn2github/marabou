#ifndef __TMrbGassiplex__
#define __TMrbGassiplex__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbGassiplex.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbGassiplex        -- Gassiplex module TUM/E12
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id$
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
#include "TEnv.h"
#include "TObject.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbGassiplex
// Purpose:        Define a Gassiplex module
// Description:    Defines a Gassiplex module
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbGassiplex : public TMrbVMEModule {

	public:
		enum				{	kAddrMod			=	0x09	};

		enum				{	kNofChansPerBoard	=	64	};

	public:

		TMrbGassiplex() {};  												// default ctor
		TMrbGassiplex(const Char_t * ModuleName, UInt_t BaseAddr, Int_t NofChannels); 			// define a new module
		~TMrbGassiplex() {};													// default dtor

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TMrbVMEChannel * Channel, Int_t Value = 0);  	// generate code for given channel
		
		virtual inline const Char_t * GetMnemonic() const { return("gassiplex"); }; 	// module mnemonic

		inline void SetBlockXfer(Bool_t Flag = kTRUE) { fBlockXfer = Flag; };
		inline Bool_t BlockXferEnabled() { return(fBlockXfer); };

		inline Bool_t HasBlockXfer() const { return(kTRUE); };			// module is capable of using BLT
		inline Bool_t HasOwnMapping() const { return(kTRUE); };			// module has its own vme mapping

		virtual Bool_t CheckProcType();
		
		TEnv * UseSettings(const Char_t * SettingsFile = NULL);

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		void DefineRegisters(); 							// define vme registers

	protected:
		Bool_t fBlockXfer;
		Int_t fNofChannels;
		TString fSettingsFile;

	ClassDef(TMrbGassiplex, 1)		// [Config] Gassiplex module
};

#endif

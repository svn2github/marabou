#ifndef __TMrbSis_3302_h__
#define __TMrbSis_3302_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSis_3302.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSis_3302        -- VME digitizer adc
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbSis_3302.h,v 1.5 2011-12-15 16:33:23 Marabou Exp $
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
// Name:           TMrbSis_3302
// Purpose:        Define a VME module type SIS 3302
// Description:    Defines a VME module SIS 3302
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSis_3302 : public TMrbVMEModule {

	public:
		enum				{	kSegSizeStart		=	0x0010000L	};
		enum				{	kSegSize2		=	0x4000000L	};
		enum				{	kSegSize3		=	0x8000000L	};
		enum				{	kAddrMod		=	0x09	};

		enum				{	kNofGroups		=	4	};
		enum				{	kNofChannels	=	8	};

	public:

		TMrbSis_3302() {};  												// default ctor
		TMrbSis_3302(const Char_t * ModuleName, UInt_t BaseAddr); 			// define a new module
		~TMrbSis_3302() {};													// default dtor

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TMrbVMEChannel * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() const { return("sis_3302"); }; 	// module mnemonic

		inline void SetTracingMode(Bool_t Flag = kFALSE) { fTracingMode = Flag; };
		inline Bool_t TracingEnabled() { return(fTracingMode); };

		inline void SetBlockXfer(Bool_t Flag = kTRUE) { fBlockXfer = Flag; };
		inline Bool_t BlockXferEnabled() { return(fBlockXfer); };

		inline void SetMaxEvents(Int_t Events) { fMaxEvents = Events; };
		inline Int_t GetMaxEvents() { return(fMaxEvents); };

		Bool_t UseSettings(const Char_t * SettingsFile = NULL);

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		void DefineRegisters(); 							// define vme registers

	protected:
		Bool_t fTracingMode;
		Bool_t fBlockXfer;
		Int_t fMaxEvents;
		TString fSettingsFile;
		TString fDumpFile;

	ClassDef(TMrbSis_3302, 1)		// [Config] SIS 3302, VME digitizing adc
};

#endif

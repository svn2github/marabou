#ifndef __TMrbCaen_V556_h__
#define __TMrbCaen_V556_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbCaen_V556.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbCaen_V556        -- 8 chn peak sensing ADC
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbCaen_V556.h,v 1.8 2008-01-14 09:48:51 Rudolf.Lutter Exp $       
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

#include "TMrbVMEModule.h"

class TMrbVMEChannel;

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V556
// Purpose:        Define a VME adc type CAEN V556
// Description:    Defines a VME adc CAEN V556
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbCaen_V556 : public TMrbVMEModule {

	public:
		enum				{	kSegSize		=	0x1000	};
		enum				{	kAddrMod		=	0x39	};

		enum EMrbOffsets	{	kOffsReset		=	0x1C,
								kOffsHF			=	0x1E,
								kOffsControl	=	0x1A,
								kOffsOutput		=	0x18,
								kOffsFF 		=	0x16,
								kOffsDelay		=	0x14,
								kOffsTHRH		=	0x12,
								kOffsTHRL		=	0x10,
								kOffsModuleType =	0xFC,
								kOffsVersion 	=	0xFE
							};

		enum EMrbRegisters	{	kRegLowerThresh,
								kRegUpperThresh,
								kRegControl,
								kRegDelay
							};

	public:

		TMrbCaen_V556() {};  												// default ctor
		TMrbCaen_V556(const Char_t * ModuleName, UInt_t BaseAddr); 				// define a new adc
		~TMrbCaen_V556() {};												// default dtor

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TMrbVMEChannel * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() const { return("caen_v556"); }; 	// module mnemonic

		inline void SetFFMode(Bool_t FFFlag = kTRUE) { fFFMode = FFFlag; };
		inline Bool_t IsFFMode() const { return(fFFMode); };

		inline Bool_t HasRandomReadout() const { return(kFALSE); };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		void DefineRegisters(); 							// define vme registers

	protected:
		Bool_t fFFMode;

	ClassDef(TMrbCaen_V556, 1)		// [Config] CAEN V556, 8 x 4K peak sensing VME ADC
};

#endif

#ifndef __TMrbCaen_V820_h__
#define __TMrbCaen_V820_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbCaen_V820.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbCaen_V820        -- 32 chn latching scaler
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbCaen_V820.h,v 1.10 2009-07-27 08:37:16 Rudolf.Lutter Exp $
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

#include "TMrbVMEScaler.h"

class TMrbVMEChannel;

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V820
// Purpose:        Define a VME adc type CAEN V820
// Description:    Defines a VME adc CAEN V820
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbCaen_V820 : public TMrbVMEScaler {

	public:
		enum				{	kSegSize			=	0x10000	};
		enum				{	kAddrMod			=	0x39	};

		enum EMrbOffsets	{	kOffsCounter0			=	0x1000,
								kOffsTestReg			=	0x1080,
								kOffsDwellTime			=	0x1104,
								kOffsControl			=	0x1108,
								kOffsControlBitSet 		=	0x111A,
								kOffsControlBitClear	=	0x110C,
								kOffsStatus 			=	0x110E,
								kOffsGeoAddr			=	0x1110,
								kOffsModuleReset 		=	0x1120,
								kOffsSoftClear			=	0x1122,
								kOffsSoftTrigger		=	0x1124,
								kOffsDummy32			=	0x1200,
								kOffsDummy16			=	0x1204,
								kOffsFirmware			=	0x1132
							};

		enum EMrbRegisters	{	kRegFirmWare,
								kRegGeoAddr,
								kRegBitSet,
								kRegBitClear,
								kRegStatus,
								kRegControl,
								kRegDummy32,
								kRegDummy16
							};

		enum				{	kBitControlAcqMode			 		=	BIT(0),
								kBitControlAutoRestart				=	BIT(7)
							};

	public:

		TMrbCaen_V820() {};  												// default ctor
		TMrbCaen_V820(const Char_t * ModuleName, UInt_t BaseAddr);  		// define a new scaler
		~TMrbCaen_V820() {};												// default dtor

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TMrbModuleChannel * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() const { return("caen_V820"); }; 	// module mnemonic

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		void DefineRegisters(); 							// define vme registers

	protected:
		Bool_t fFFMode;
		Bool_t fCommonStart;

	ClassDef(TMrbCaen_V820, 1)		// [Config] CAEN V820, 16/32 chn VME TDC
};

#endif

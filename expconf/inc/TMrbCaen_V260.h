#ifndef __TMrbCaen_V260_h__
#define __TMrbCaen_V260_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbCaen_V260.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbCaen_V260        -- 16 chn 24 bit VME scaler
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       
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

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V260
// Purpose:        Define a VME scaler type CAEN V260
// Description:    Defines a VME scaler CAEN V260 (list mode)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbCaen_V260 : public TMrbVMEScaler {

	public:
		enum				{	kSegSize		=	0x1000	};
		enum				{	kAddrMod		=	0x39	};

		enum EMrbOffsets	{	kOffsChannel0	=	0x10,
								kOffsClear		=	0x50,
								kOffsInhibit	=	0x52,
								kOffsEnable 	=	0x54,
								kOffsIncrement 	=	0x56,
								kOffsModuleType =	0xFC,
								kOffsVersion 	=	0xFE
							};

	public:

		TMrbCaen_V260() {};  												// default ctor
		TMrbCaen_V260(const Char_t * ModuleName, UInt_t BaseAddr); 			// define a new scaler
		~TMrbCaen_V260() {};												// default dtor

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TObject * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() const { return("caen_v260"); }; 	// module mnemonic

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbCaen_V260.html&"); };

	protected:
		void DefineRegisters(); 							// define vme registers

	ClassDef(TMrbCaen_V260, 1)		// [Config] CAEN V260, 16 x 24 bit VME scaler
};

#endif

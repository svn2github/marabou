#ifndef __TMrbSis_3801_h__
#define __TMrbSis_3801_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSis_3801.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSis_3801        -- 16 chn 24 bit VME scaler
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
// Name:           TMrbSis_3801
// Purpose:        Define a VME scaler type SIS 3801
// Description:    Defines a VME scaler SIS 3801 (list mode)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSis_3801 : public TMrbVMEScaler {

	public:
		enum				{	kSegSize		=	0x200	};
		enum				{	kAddrMod		=	0x39	};

		enum EMrbOffsets	{};

	public:

		TMrbSis_3801() {};  												// default ctor
		TMrbSis_3801(const Char_t * ModuleName, UInt_t BaseAddr, Int_t FifoDepth = 1); 	// define a new scaler
		~TMrbSis_3801() {};												// default dtor

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TObject * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() const { return("sis_3801"); }; 	// module mnemonic

		inline void SetFifoDepth(Int_t FifoDepth) { fFifoDepth = FifoDepth; };
		inline Int_t GetFifoDepth() const { return(fFifoDepth); };

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbSis_3801.html&"); };

	protected:
		void DefineRegisters(); 							// define vme registers

	protected:
		Int_t fFifoDepth;			// fifo depth per channel

	ClassDef(TMrbSis_3801, 1)		// [Config] SIS 3801, 32 x 24 bit VME scaler
};

#endif

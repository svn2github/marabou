#ifndef __TMrbAcromag_IP341_h__
#define __TMrbAcromag_IP341_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbAcromag_IP341.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbAcromag_IP341          -- a 16 channel sampling adc
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <iostream.h>
#include <strstream.h>
#include <iomanip.h>
#include <fstream.h>

#include "Rtypes.h"
#include "TSystem.h"
#include "TObject.h"

#include "TMrbVMEModule.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAcromag_IP341
// Purpose:        Define a VME adc type Acromag-9660/IP341
// Description:    Defines a VME adc Acromag-9660/IP341
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbAcromag_IP341 : public TMrbVMEModule {

	public:
		enum				{	kSegSize		=	0x400	};			// 1 kB
		enum				{	kAddrMod		=	0x29	};			// A16

	public:

		TMrbAcromag_IP341() {};  												// default ctor
		TMrbAcromag_IP341(const Char_t * ModuleName, UInt_t BaseAddr, Int_t SubDevice); 	// define a new adc
		~TMrbAcromag_IP341() {};												// default dtor

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TObject * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() { return("acro_ip341"); }; 	// module mnemonic

		virtual inline Int_t GetNofSubDevices() { return(4); };				// houses up to 4 ip modules
		inline Bool_t HasRandomReadout() { return(kFALSE); };				// has to be read out as a whole

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbAcromag_IP341.html&"); };

	protected:
		void DefineRegisters(); 							// define vme registers

	ClassDef(TMrbAcromag_IP341, 1)		// [Config] Acromag-9660/IP341, 16 chn sampling ADC, 14 bit
};

#endif

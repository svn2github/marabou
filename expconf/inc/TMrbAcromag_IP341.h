#ifndef __TMrbAcromag_IP341_h__
#define __TMrbAcromag_IP341_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbAcromag_IP341.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbAcromag_IP341          -- a 16 channel sampling adc
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbAcromag_IP341.h,v 1.9 2008-03-27 15:49:37 Rudolf.Lutter Exp $       
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
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TMrbVMEChannel * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() const { return("acro_ip341"); }; 	// module mnemonic

		virtual Bool_t CheckSubeventType(TMrbSubevent * Subevent) const;		// check if subevent type is HB_1 [10,71]

		virtual inline Int_t GetNofSubDevices() const { return(4); };				// houses up to 4 ip modules
		inline Bool_t HasRandomReadout() const { return(kFALSE); };				// has to be read out as a whole

		virtual inline Bool_t HasPrivateCode() const { return(kTRUE); }; 			// use private code files
		virtual inline const Char_t * GetPrivateCodeFile() const { return("Module_HB"); };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		void DefineRegisters(); 							// define vme registers

	ClassDef(TMrbAcromag_IP341, 1)		// [Config] Acromag-9660/IP341, 16 chn sampling ADC, 14 bit
};

#endif

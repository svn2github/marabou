#ifndef __TMrbAdcd_9701_h__
#define __TMrbAdcd_9701_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbAdcd_9701.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbAdcd_9701        -- dual flash adc tum/w.liebl
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

#include "TMrbCamacModule.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAdcd_9701
// Purpose:        Define a flash ADC of type ADCD 9701
// Description:    Defines a flash ADC of type ADCD 9701.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbAdcd_9701 : public TMrbCamacModule {

	public:
		enum	EMrbRegisters	{	kRegLowerThresh 	};

	public:

		TMrbAdcd_9701() {};  											// default ctor
		TMrbAdcd_9701(const Char_t * ModuleName, const Char_t * ModulePosition, Int_t SubDevice = 0, Int_t Channels = 2048); // define a new flash adc
		~TMrbAdcd_9701() {};											// remove adc from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TObject * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() const { return("adcd"); }; 	// module mnemonic

		virtual inline Int_t GetNofSubDevices() const { return(2); };				// houses 2 identical adcs
		virtual inline Bool_t HasRandomReadout() const { return(kFALSE); };		// must be read as a whole

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbAdcd_9701.html&"); };

	protected:
		void DefineRegisters(); 				// define camac regs

	ClassDef(TMrbAdcd_9701, 1)		// [Config] ADCD 9701, 2 x 2K CAMAC Flash ADC
};

#endif

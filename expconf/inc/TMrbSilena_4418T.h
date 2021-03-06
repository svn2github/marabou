#ifndef __TMrbSilena_4418T_h__
#define __TMrbSilena_4418T_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSilena_4418T.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSilena_4418T     -- silena tdc 4418/T
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbSilena_4418T.h,v 1.9 2008-12-10 12:13:49 Rudolf.Lutter Exp $       
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

class TMrbModuleChannel;
class TMrbSubevent;

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSilena_4418T
// Purpose:        Define an ADC of type Silena 4418/V
// Description:    Defines an ADC of type Silena 4418/V.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSilena_4418T : public TMrbCamacModule {

	public:
		enum	EMrbRegisters		{	kRegLowerThresh,
										kRegUpperThresh,
										kRegOffset,
										kRegStatus
									};

		enum	EMrbSilenaStatus	{	kStatusSUB	=	BIT(9),
										kStatusEEN	=	BIT(10),
										kStatusOVF	=	BIT(11),
										kStatusCCE	=	BIT(12),
										kStatusCSR	=	BIT(13),
										kStatusCLE	=	BIT(14)
									};

		enum						{	kDefaultOffset			=	140		};
		enum						{	kDefaultStatus			=	0x200	};
		enum						{	kZeroCompressionStatus	=	kStatusSUB | kStatusCCE | kStatusCSR	};

	public:

		TMrbSilena_4418T() {};  													// default ctor
		TMrbSilena_4418T(const Char_t * ModuleName, const Char_t * ModulePosition);	// define a new silena tdc
		~TMrbSilena_4418T() {};														// remove silena tdc from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TMrbModuleChannel * Channel, Int_t Value = 0);  	// generate code for given channel

		inline void SetZeroSuppression(Bool_t Flag = kTRUE) { fZeroSuppression = Flag; };	// zero compression on/off
		inline Bool_t HasZeroSuppression() const { return(fZeroSuppression); };
		
		virtual inline const Char_t * GetMnemonic() const { return("sil_4418t"); }; 	// module mnemonic

		virtual Bool_t CheckSubeventType(TMrbSubevent * Subevent) const;		// check if subevent type is [10,3x]

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		void DefineRegisters(); 							// define camac registers

	protected:
		Bool_t fZeroSuppression;
	
	ClassDef(TMrbSilena_4418T, 1)		// [Config] Silena 4418/T, 8 x 4K CAMAC TDC
};

#endif

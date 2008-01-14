#ifndef __TMrbSilena_4418V_h__
#define __TMrbSilena_4418V_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSilena_4418V.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSilena_4418V     -- silena adc 4418/V
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbSilena_4418V.h,v 1.8 2008-01-14 09:48:51 Rudolf.Lutter Exp $       
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

class TMrbCamacChannel;
class TMrbSubevent;

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSilena_4418V
// Purpose:        Define an ADC of type Silena 4418/V
// Description:    Defines an ADC of type Silena 4418/V.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSilena_4418V : public TMrbCamacModule {

	public:
		enum	EMrbRegisters		{	kRegLowerThresh,
										kRegUpperThresh,
										kRegOffset,
										kRegStatus,
										kRegCommonThresh
									};

		enum	EMrbSilenaStatus	{	kStatusSUB	=	BIT(9),
										kStatusEEN	=	BIT(10),
										kStatusOVF	=	BIT(11),
										kStatusCCE	=	BIT(12),
										kStatusCSR	=	BIT(13),
										kStatusCLE	=	BIT(14)
									};

		enum						{	kDefaultOffset			=	140										};
		enum						{	kDefaultCommonThresh	=	40										};
		enum						{	kDefaultStatus			=	kStatusSUB								};
		enum						{	kZeroCompressionStatus	=	kStatusSUB | kStatusCCE | kStatusCSR	};

	public:

		TMrbSilena_4418V() {};														// default ctor
		TMrbSilena_4418V(const Char_t * ModuleName, const Char_t * ModulePosition);	// define a new silena adc
		~TMrbSilena_4418V() {};														// remove silena adc from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TMrbCamacChannel * Channel, Int_t Value = 0);  	// generate code for given channel

		inline void SetZeroSuppression(Bool_t Flag = kTRUE) { fZeroSuppression = Flag; };	// zero compression on/off
		inline Bool_t HasZeroSuppression() const { return(fZeroSuppression); };
		
		virtual inline const Char_t * GetMnemonic() const { return("sil_4418v"); }; 	// module mnemonic

		virtual Bool_t CheckSubeventType(TMrbSubevent * Subevent) const;		// check if subevent type is [10,3x]

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		void DefineRegisters(); 							// define camac registers

	protected:
		Bool_t fZeroSuppression;							// zero compression on/off
	
	ClassDef(TMrbSilena_4418V, 1)	// [Config] Silena 4418/V, 8 x 4K CAMAC ADC
};

#endif

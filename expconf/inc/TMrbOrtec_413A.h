#ifndef __TMrbOrtec_413A_h__
#define __TMrbOrtec_413A_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbOrtec_413A.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbOrtec_413A     -- ortec ad413a
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbOrtec_413A.h,v 1.6 2004-09-28 13:47:32 rudi Exp $       
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
// Name:           TMrbOrtec_413A
// Purpose:        Define an ADC of type Ortec AD413A
// Description:    Defines an ADC of type Ortec AD413A.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbOrtec_413A : public TMrbCamacModule {

	public:
		enum	EMrbRegisters		{	kRegLowerThresh,
										kRegStatus,
										kRegGate
									};

		enum	EMrbOrtecStatus 	{	kStatusZSD	=	BIT(8),
										kStatusCRE	=	BIT(9),
										kStatusSME	=	BIT(12),
										kStatusCRA	=	BIT(13),
										kStatusCLE	=	BIT(14),
										kStatusOSD	=	BIT(15)
									};

		enum	EMrbOrtecGate	 	{	kGateG1D	=	BIT(0),
										kGateG2D	=	BIT(1),
										kGateG3D	=	BIT(2),
										kGateG4D	=	BIT(3),
										kGateMGD	=	BIT(4)
									};

		enum						{	kDefaultLowerThresh		=	36		};
		enum						{	kDefaultStatus			=	kStatusCRE | kStatusOSD | kStatusZSD | kStatusCRA		};
		enum						{	kDefaultGate			=	kGateG1D | kGateG2D | kGateG3D | kGateG4D		};

	public:

		TMrbOrtec_413A() {};														// default ctor
		TMrbOrtec_413A(const Char_t * ModuleName, const Char_t * ModulePosition);				// define a new silena adc
		~TMrbOrtec_413A() {};														// remove silena adc from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TObject * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() const { return("ortec_413a"); }; 	// module mnemonic

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbOrtec_413A.html&"); };

	protected:
		void DefineRegisters(); 							// define camac registers

	ClassDef(TMrbOrtec_413A, 1) 	// [Config] ORTEC 413A, 4 x 8K CAMAC ADC
};

#endif

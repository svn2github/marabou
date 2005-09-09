#ifndef __TMrbGanelec_Q1612F_h__
#define __TMrbGanelec_Q1612F_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbGanelec_Q1612F.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbGanelec_Q1612F	-- ganelec qdc 16 fold
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbGanelec_Q1612F.h,v 1.7 2005-09-09 06:59:13 Rudolf.Lutter Exp $       
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
// Name:           TMrbGanelec_Q1612F
// Purpose:        Define an QDC of type Ganelec QDC1612F
// Description:    Defines an QDC of type Ganelec QDC1612F.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbGanelec_Q1612F : public TMrbCamacModule {

	public:
		enum	EMrbRegisters		{	kRegOffset,
										kRegCommonOffset,
										kRegMasterGate,
										kRegStatus
									};

		enum	EMrbGanelecStatus	{	kStatusEPS	=	0x1,
										kStatusECE	=	0x2,
										kStatusEEN	=	0x4,
										kStatusCPS	=	0x8,
										kStatusCCE	=	0x10,
										kStatusCSR	=	0x20,
										kStatusCLE	=	0x40,
										kStatusOFS	=	0x80,
										kStatusIND	=	0x100,
									};

		enum						{	kDefaultOffset			=	0		};
		enum						{	kDefaultCommonOffset	=	0x7F	};
		enum						{	kDefaultGate			=	4		};

		enum						{	kBitMGT 	=	10		};
		enum						{	kMaskMGT	=	0x1E00	};

	public:

		TMrbGanelec_Q1612F() {};														// default ctor
		TMrbGanelec_Q1612F(const Char_t * ModuleName, const Char_t * ModulePosition);	// define a new silena adc
		~TMrbGanelec_Q1612F() {};														// remove silena adc from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TObject * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() const { return("gan_q1612f"); }; 	// module mnemonic

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		void DefineRegisters(); 							// define camac registers

	ClassDef(TMrbGanelec_Q1612F, 1) 	// [Config] GANELEC Q1612F, 16 x 4K CAMAC QDC
};

#endif

#ifndef __TMrbLeCroy_1176_h__
#define __TMrbLeCroy_1176_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbLeCroy_1176.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbLeCroy_1176        -- 32 chn peak sensing ADC
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbLeCroy_1176.h,v 1.5 2008-01-14 09:48:51 Rudolf.Lutter Exp $       
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
class TMrbSubevent;

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbLeCroy_1176
// Purpose:        Define a VME tdc type LeCroy 1176
// Description:    Defines a VME tdc LeCroy 1176
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbLeCroy_1176 : public TMrbVMEModule {

	public:
		enum				{	kSegSize			=	0x10000	};
		enum				{	kAddrMod			=	0x39	};

	public:

		TMrbLeCroy_1176() {};  												// default ctor
		TMrbLeCroy_1176(const Char_t * ModuleName, UInt_t BaseAddr); 			// define a new adc
		~TMrbLeCroy_1176() {};												// default dtor

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TMrbVMEChannel * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() const { return("lecroy_1176"); }; 	// module mnemonic

		inline void SetCommonStart(Bool_t CommonStartFlag = kTRUE) { fCommonStart = CommonStartFlag; };
		inline void SetCommonStop(Bool_t CommonStopFlag = kTRUE) { fCommonStart = !CommonStopFlag; };
		inline Bool_t IsCommonStart() const { return(fCommonStart); };

		inline Bool_t CheckSubeventType(TMrbSubevent * Subevent) const { return(kTRUE); };

		inline Bool_t HasRandomReadout() const { return(kFALSE); };

		virtual inline Bool_t HasPrivateCode() const { return(kFALSE); }; 			// use private code files
		
		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		void DefineRegisters(); 							// define vme registers

	protected:
		Bool_t fCommonStart;

	ClassDef(TMrbLeCroy_1176, 1)		// [Config] LeCroy 1176, 16 ch multi-hit multi-event VME TDC
};

#endif

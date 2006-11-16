#ifndef __TMrbCaen_V785_h__
#define __TMrbCaen_V785_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbCaen_V785.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbCaen_V785        -- 32 chn peak sensing ADC
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbCaen_V785.h,v 1.10 2006-11-16 09:28:05 Rudolf.Lutter Exp $       
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

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V785
// Purpose:        Define a VME adc type CAEN V785
// Description:    Defines a VME adc CAEN V785
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbCaen_V785 : public TMrbVMEModule {

	public:
		enum				{	kSegSize			=	0x10000	};
		enum				{	kAddrMod			=	0x39	};

		enum EMrbOffsets	{	kOffsOutput			=	0x0,
								kOffsFirmWare		=	0x1000,
								kOffsGeoAddr		=	0x1002,
								kOffsBitSet1		=	0x1006,
								kOffsBitClear1		=	0x1008,
								kOffsStatus1		=	0x100E,
								kOffsControl1		=	0x1010,
								kOffsAddrHigh		=	0x1012,
								kOffsAddrLow		=	0x1014,
								kOffsEvtTrig		=	0x1020,
								kOffsStatus2		=	0x1022,
								kOffsEvtCntLow		=	0x1024,
								kOffsEvtCntHigh 	=	0x1026,
								kOffsIncrEvt		=	0x1028,
								kOffsIncrOffs		=	0x102A,
								kOffsBitSet2		=	0x1032,
								kOffsBitClear2		=	0x1034,
								kOffsCrateSel		=	0x103C,
								kOffsTestEvtWrite	=	0x103E,
								kOffsEvtCntReset	=	0x1040,
								kOffsThresh 		=	0x1080
							};

		enum EMrbRegisters	{	kRegFirmWare,
								kRegGeoAddr,
								kRegBitSet1,
								kRegBitClear1,
								kRegStatus1,
								kRegControl1,
								kRegAddrHigh,
								kRegAddrLow,
								kRegEvtTrig,
								kRegStatus2,
								kRegEvtCntLow,
								kRegEvtCntHigh,
								kRegIncrEvt,
								kRegIncrReg,
								kRegBitSet2,
								kRegBitClear2,
								kRegCrateSel,
								kRegTestEvtWrite,
								kRegEvtCntReset,
								kRegThresh
							};

		enum				{	kBitSet1SelAddrIntern 				=	BIT(4)	};
		enum				{	kBitControl1ProgResetModule 		=	BIT(4)	};
		enum				{	kBitSet2Offline 					=	BIT(1),
								kBitSet2OverRangeDis				=	BIT(3),
								kBitSet2LowThreshDis				=	BIT(4),
								kBitSet2SlideEna					=	BIT(7),
								kBitSet2AutoIncrEna 				=	BIT(11),
								kBitSet2EmptyProgEna				=	BIT(12),
								kBitSet2AllTrigEna					=	BIT(14)
							};
		
	public:

		TMrbCaen_V785() {};  												// default ctor
		TMrbCaen_V785(const Char_t * ModuleName, UInt_t BaseAddr); 			// define a new adc
		~TMrbCaen_V785() {};												// default dtor

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TObject * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() const { return("caen_v785"); }; 	// module mnemonic

		inline void SetFFMode(Bool_t FFFlag = kTRUE) { fFFMode = FFFlag; };
		inline Bool_t IsFFMode() const { return(fFFMode); };

		virtual Bool_t CheckSubeventType(TObject * Subevent) const;		// check if subevent type is [10,4x]

		inline Bool_t HasRandomReadout() const { return(kFALSE); };

		inline void SetFineThresh(Bool_t FineFlag = kTRUE) { fFineThresh = FineFlag; }; // lower thresh in steps of 2 bits
		inline Bool_t HasFineThresh() const { return(fFineThresh); };

		inline void SetZeroSuppression(Bool_t ZsFlag = kTRUE) { fZeroSuppression = ZsFlag; };	// zero compression on/off
		inline Bool_t HasZeroSuppression() const { return(fZeroSuppression); };
		
		inline void SetOverRangeCheck(Bool_t OrFlag = kTRUE) { fOverRangeCheck = OrFlag; }; 	// range check on/off
		inline Bool_t HasOverRangeCheck() const { return(fOverRangeCheck); };
		
		virtual inline Bool_t HasPrivateCode() const { return(kTRUE); }; 			// use private code files
		virtual inline const Char_t * GetPrivateCodeFile() const { return("Module_Caen_Vxxx"); };
		
		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		void DefineRegisters(); 							// define vme registers

	protected:
		Bool_t fFFMode;
		Bool_t fFineThresh;

		Bool_t fZeroSuppression;
		Bool_t fOverRangeCheck;

	ClassDef(TMrbCaen_V785, 1)		// [Config] CAEN V785, 32 x 4K peak sensing VME ADC
};

#endif

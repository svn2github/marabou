#ifndef __TMrbCaen_V775_h__
#define __TMrbCaen_V775_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbCaen_V775.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbCaen_V775        -- 16/32 chn TDC
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

#include "TMrbVMEModule.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V775
// Purpose:        Define a VME adc type CAEN V775
// Description:    Defines a VME adc CAEN V775
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbCaen_V775 : public TMrbVMEModule {

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
								kOffsFullScaleRange	=	0x1060,
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
								kRegFullScaleRange,
								kRegThresh
							};

		enum				{	kBitSet1SelAddrIntern 				=	BIT(4)	};
		enum				{	kBitControl1ProgResetModule 		=	BIT(4)	};
		enum				{	kBitSet2Offline 					=	BIT(1),
								kBitSet2OverRangeDis				=	BIT(3),
								kBitSet2LowThreshDis				=	BIT(4),
								kBitSet2ValidControlDis				=	BIT(5),
								kBitSet2SlideEna					=	BIT(7),
								kBitSet2CommonStartDis				=	BIT(10),
								kBitSet2AutoIncrEna 				=	BIT(11),
								kBitSet2EmptyProgEna				=	BIT(12),
								kBitSet2AllTrigEna					=	BIT(14)
							};
		
	public:

		TMrbCaen_V775() {};  												// default ctor
		TMrbCaen_V775(const Char_t * ModuleName, UInt_t BaseAddr, Int_t NofChannels = 0); 	// define a new adc
		~TMrbCaen_V775() {};												// default dtor

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TObject * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() const { return("caen_V775"); }; 	// module mnemonic

		inline void SetFFMode(Bool_t FFFlag = kTRUE) { fFFMode = FFFlag; };
		inline Bool_t IsFFMode() const { return(fFFMode); };

		inline void SetCommonStart(Bool_t CommonStartFlag = kTRUE) { fCommonStart = CommonStartFlag; };
		inline void SetCommonStop(Bool_t CommonStopFlag = kTRUE) { fCommonStart = !CommonStopFlag; };
		inline Bool_t IsCommonStart() const { return(fCommonStart); };

		Int_t SetFullScaleRange(Int_t NanoSeconds);					// set full scale range in nanoseconds
		inline Int_t GetFullScaleRange(Bool_t NsecFlag = kTRUE) const { return(NsecFlag ? fFullScaleRangeNsecs : fFullScaleRange); };

		inline void SetZeroSuppression(Bool_t ZsFlag = kTRUE) { fZeroSuppression = ZsFlag; };	// zero compression on/off
		inline Bool_t HasZeroSuppression() const { return(fZeroSuppression); };
		
		inline void SetOverRangeCheck(Bool_t OrFlag = kTRUE) { fOverRangeCheck = OrFlag; }; 	// range check on/off
		inline Bool_t HasOverRangeCheck() const { return(fOverRangeCheck); };
		
		virtual Bool_t CheckSubeventType(TObject * Subevent) const;		// check if subevent type is [10,4x]

		inline Bool_t HasRandomReadout() const { return(kFALSE); };

		virtual inline Bool_t HasPrivateCode() const { return(kTRUE); }; 			// use private code files
		virtual inline const Char_t * GetPrivateCodeFile() const { return("Module_Caen_V7X5"); };
		
		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbCaen_V775.html&"); };

	protected:
		void DefineRegisters(); 							// define vme registers

	protected:
		Bool_t fFFMode;
		Bool_t fCommonStart;
		Bool_t fZeroSuppression;
		Bool_t fOverRangeCheck;

		Int_t fFullScaleRange;
		Int_t fFullScaleRangeNsecs;

	ClassDef(TMrbCaen_V775, 1)		// [Config] CAEN V775, 16/32 chn VME TDC
};

#endif

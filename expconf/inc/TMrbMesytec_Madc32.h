#ifndef __TMrbMesytec_Madc32_h__
#define __TMrbMesytec_Madc32_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbMesytec_Madc32.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbMesytec_Madc32        -- VME adc
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbMesytec_Madc32.h,v 1.1 2008-09-26 11:54:15 Rudolf.Lutter Exp $       
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

class TMrbVMEChannel;

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbMesytec_Madc32
// Purpose:        Define a VME module type Mesytec MADC32
// Description:    Defines a VME module MADC32
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbMesytec_Madc32 : public TMrbVMEModule {

	public:
		enum				{	kSegSize		=	0x10000L	};
		enum				{	kAddrMod		=	0x39		};

		enum				{	kNofChannels	=	32	};

		enum				{	kGGDefaultDelay	=	20	};
		enum				{	kGGDefaultWidth	=	50	};

		enum EMrbRegisters	{	kRegAddrSource,
								kRegAddrReg,
								kRegModuleId,
								kRegDataLengthFormat,
								kRegMultiEvent,
								kRegMarkingType,
								kRegBankOperation,
								kRegAdcResolution,
								kRegOutputFormat,
								kRegAdcOverride,
								kRegSlidingScaleOff,
								kRegSkipOutOfRange,
								kRegHoldDelay,
								kRegHoldWidth,
								kRegUseGG,
								kRegInputRange,
								kRegEclTerm,
								kRegEclGate1OrOsc,
								kRegEclFclOrReset,
								kRegEclBusy,
								kRegNimGate1OrOsc,
								kRegNimFclOrReset,
								kRegNimBusy,
								kRegPulserStatus,
								kRegTsSource,
								kRegTsDivisor,
								kRegThreshold
							};

		enum EMrbAddressSource			{	kAddressBoard		=	0,
											kAddressRegister
										};

		enum EMrbDataLengthFormat		{	kDataLngFmt8		=	0,
											kDataLngFmt16,
											kDataLngFmt32,
											kDataLngFmt64
										};

		enum EMrbMultiEvent				{	kMultiEvtNo			=	0,
											kMultiEvtYes,
											kMultiEvt1By1
										};

		enum EMrbMarkingType			{	kMarkingTypeEvent	=	0,
											kMarkingTypeTimestamp
										};

		enum EMrbBankOperation			{	kBankOprConnected	=	0,
											kBankOprIndependent,
											kBankOprZeroDeadTime
										};

		enum EMrbAdcResolution			{	kAdcRes2k			=	0,
											kAdcRes4k,
											kAdcRes4kHires,
											kAdcRes8k,
											kAdcRes8kHires,
											kAdcDontOverride
										};

		enum EMrbOutputFormat			{	kOutFmtAddr			=	0,
											kOutFmtCompact
										};

		enum EMrbUseGG					{	kUseGGnone			=	0,
											kUseGG0				=	BIT(0),
											kUseGG1 			=	BIT(1),
											kUseGGboth 			=	BIT(0) | BIT(1)
										};

		enum EMrbInputRange 			{	kInpRng4V 			=	0,
											kInpRng8V,
											kInpRng10V
										};

		enum EMrbEclTerm	 			{	kEclTermOff 		=	0,
											kEclTermGate0 		=	BIT(0),
											kEclTermGate1 		=	BIT(1),
											kEclTermFastClear 	=	BIT(2),
											kEclTermBusy		=	BIT(3),
											kEclTermOn			=	kEclTermGate0 | kEclTermGate1 | kEclTermFastClear | kEclTermBusy,
										};

		enum EMrbEclGate1Osc	 		{	kEclGate1			=	0,
											kEclOscillator
										};

		enum EMrbEclFclRes	 			{	kEclFastClear		=	0,
											kEclReset
										};

		enum EMrbNimGate1Osc	 		{	kNimGate1			=	0,
											kNimOscillator
										};

		enum EMrbNimFclRes	 			{	kNimFastClear		=	0,
											kNimReset
										};

		enum EMrbNimBusy		 		{	kNimBusy			=	0,
											kNimGate0Out,
											kNimGate1Out,
											kNimCbusOut
										};

		enum EMrbTestPulser		 		{	kPulserOff			=	0,
											kPulserAmpl			=	4,
											kPulserAmplLow		=	5,
											kPulserAmplHigh		=	6,
											kPulserAmplToggle	=	7
										};

		enum EMrbTsSource		 		{	kTstampVME			=	0,
											kTstampExtern		=	BIT(0),
											kTstampReset		=	BIT(1)
										};

	public:

		TMrbMesytec_Madc32() {};  													// default ctor
		TMrbMesytec_Madc32(const Char_t * ModuleName, UInt_t BaseAddr); 			// define a new module
		~TMrbMesytec_Madc32() {};													// default dtor

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TMrbVMEChannel * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() const { return("Mesytec_Madc32"); }; 	// module mnemonic

		inline void SetBlockXfer(Bool_t Flag = kTRUE) { fBlockXfer = Flag; };
		inline Bool_t BlockXferEnabled() { return(fBlockXfer); };

		inline Bool_t SetThreshold(Int_t Thresh, Int_t Channel = -1) { return(this->Set(TMrbMesytec_Madc32::kRegThreshold, Thresh, Channel)); };
		inline Int_t GetThreshold(Int_t Channel) { return(this->Get(TMrbMesytec_Madc32::kRegThreshold, Channel)); };

		inline Bool_t SetAddressSource(Int_t AddrSource) { return(this->Set(TMrbMesytec_Madc32::kRegAddrSource, AddrSource)); };
		inline Bool_t SetAddressSource(Char_t * AddrSource) { return(this->Set(TMrbMesytec_Madc32::kRegAddrSource, AddrSource)); };
		inline Int_t GetAddressSource() { return(this->Get(TMrbMesytec_Madc32::kRegAddrSource)); };

		inline Bool_t SetAddressRegister(Int_t Address) { return(this->Set(TMrbMesytec_Madc32::kRegAddrReg, Address)); };
		inline Int_t GetAddressRegister() { return(this->Get(TMrbMesytec_Madc32::kRegAddrReg)); };

		inline Bool_t SetModuleId(Int_t ModuleId) { return(this->Set(TMrbMesytec_Madc32::kRegModuleId, ModuleId)); };
		inline Int_t GetModuleId() { return(this->Get(TMrbMesytec_Madc32::kRegModuleId)); };

		inline Bool_t SetDataLengthFormat(Int_t Format) { return(this->Set(TMrbMesytec_Madc32::kRegDataLengthFormat, Format)); };
		inline Bool_t SetDataLengthFormat(Char_t * Format) { return(this->Set(TMrbMesytec_Madc32::kRegDataLengthFormat, Format)); };
		inline Int_t GetDataLengthFormat() { return(this->Get(TMrbMesytec_Madc32::kRegDataLengthFormat)); };

		inline Bool_t SetMultiEvent(Int_t Mode) { return(this->Set(TMrbMesytec_Madc32::kRegMultiEvent, Mode)); };
		inline Bool_t SetMultiEvent(Char_t * Mode) { return(this->Set(TMrbMesytec_Madc32::kRegMultiEvent, Mode)); };
		inline Int_t GetMultiEvent() { return(this->Get(TMrbMesytec_Madc32::kRegMultiEvent)); };

		inline Bool_t SetMarkingType(Int_t Mtype) { return(this->Set(TMrbMesytec_Madc32::kRegMarkingType, Mtype)); };
		inline Bool_t SetMarkingType(Char_t * Mtype) { return(this->Set(TMrbMesytec_Madc32::kRegMarkingType, Mtype)); };
		inline Int_t GetMarkingType() { return(this->Get(TMrbMesytec_Madc32::kRegMarkingType)); };

		inline Bool_t SetBankOperation(Int_t Oper) { return(this->Set(TMrbMesytec_Madc32::kRegBankOperation, Oper)); };
		inline Bool_t SetBankOperation(Char_t * Oper) { return(this->Set(TMrbMesytec_Madc32::kRegBankOperation, Oper)); };
		inline Int_t GetBankOperation() { return(this->Get(TMrbMesytec_Madc32::kRegBankOperation)); };

		inline Bool_t SetAdcResolution(Int_t Res) { return(this->Set(TMrbMesytec_Madc32::kRegAdcResolution, Res)); };
		inline Bool_t SetAdcResolution(Char_t * Res) { return(this->Set(TMrbMesytec_Madc32::kRegAdcResolution, Res)); };
		inline Int_t GetAdcResolution() { return(this->Get(TMrbMesytec_Madc32::kRegAdcResolution)); };

		inline Bool_t SetOutputFormat(Int_t Format) { return(this->Set(TMrbMesytec_Madc32::kRegOutputFormat, Format)); };
		inline Bool_t SetOutputFormat(Char_t * Format) { return(this->Set(TMrbMesytec_Madc32::kRegOutputFormat, Format)); };
		inline Int_t GetOutputFormat() { return(this->Get(TMrbMesytec_Madc32::kRegOutputFormat)); };

		inline Bool_t SetAdcOverride(Int_t Over) { return(this->Set(TMrbMesytec_Madc32::kRegAdcOverride, Over)); };
		inline Bool_t SetAdcOverride(Char_t * Over) { return(this->Set(TMrbMesytec_Madc32::kRegAdcOverride, Over)); };
		inline Int_t GetAdcOverride() { return(this->Get(TMrbMesytec_Madc32::kRegAdcOverride)); };

		inline void SetSlidingScaleOff(Bool_t Flag) { fSlidingScaleOff = Flag; };
		inline Bool_t SlidingScaleIsOff() { return(fSlidingScaleOff); };

		inline void SetSkipOutOfRange(Bool_t Flag) { fSkipOutOfRange = Flag; };
		inline Bool_t SkipOutOfRange() { return(fSkipOutOfRange); };

		inline Bool_t SetHoldDelay(Int_t Delay, Int_t GG) { return(this->Set(TMrbMesytec_Madc32::kRegHoldDelay, Delay, GG)); };
		inline Int_t GetHoldDelay(Int_t GG) { return(this->Get(TMrbMesytec_Madc32::kRegHoldDelay, GG)); };

		inline Bool_t SetHoldWidth(Int_t Width, Int_t GG) { return(this->Set(TMrbMesytec_Madc32::kRegHoldWidth, Width, GG)); };
		inline Int_t GetHoldWidth(Int_t GG) { return(this->Get(TMrbMesytec_Madc32::kRegHoldWidth, GG)); };

		inline Bool_t UseGG(Int_t GG) { return(this->Set(TMrbMesytec_Madc32::kRegUseGG, GG)); };
		inline Int_t UseGG(Char_t * GG) { return(this->Set(TMrbMesytec_Madc32::kRegUseGG, GG)); };
		inline Int_t GetGGUsed() { return(this->Get(TMrbMesytec_Madc32::kRegUseGG)); };

		inline Bool_t SetInputRange(Int_t Range) { return(this->Set(TMrbMesytec_Madc32::kRegInputRange, Range)); };
		inline Bool_t SetInputRange(Char_t * Range) { return(this->Set(TMrbMesytec_Madc32::kRegInputRange, Range)); };
		inline Int_t GetInputRange() { return(this->Get(TMrbMesytec_Madc32::kRegInputRange)); };

		inline Bool_t SetEclTerm(Int_t Term) { return(this->Set(TMrbMesytec_Madc32::kRegEclTerm, Term)); };
		inline Bool_t SetEclTerm(Char_t * Term) { return(this->Set(TMrbMesytec_Madc32::kRegEclTerm, Term)); };
		inline Int_t GetEclTerm() { return(this->Get(TMrbMesytec_Madc32::kRegEclTerm)); };

		inline Bool_t SetEclGate1OrOsc(Int_t GO) { return(this->Set(TMrbMesytec_Madc32::kRegEclGate1OrOsc, GO)); };
		inline Bool_t SetEclGate1OrOsc(Char_t * GO) { return(this->Set(TMrbMesytec_Madc32::kRegEclGate1OrOsc, GO)); };
		inline Int_t GetEclGate1OrOsc() { return(this->Get(TMrbMesytec_Madc32::kRegEclGate1OrOsc)); };

		inline Bool_t SetEclFclOrReset(Int_t FR) { return(this->Set(TMrbMesytec_Madc32::kRegEclFclOrReset, FR)); };
		inline Bool_t SetEclFclOrReset(Char_t * FR) { return(this->Set(TMrbMesytec_Madc32::kRegEclFclOrReset, FR)); };
		inline Int_t GetEclFclOrReset() { return(this->Get(TMrbMesytec_Madc32::kRegEclFclOrReset)); };

		inline Bool_t SetNimGate1OrOsc(Int_t GO) { return(this->Set(TMrbMesytec_Madc32::kRegNimGate1OrOsc, GO)); };
		inline Bool_t SetNimGate1OrOsc(Char_t * GO) { return(this->Set(TMrbMesytec_Madc32::kRegNimGate1OrOsc, GO)); };
		inline Int_t GetNimGate1OrOsc() { return(this->Get(TMrbMesytec_Madc32::kRegNimGate1OrOsc)); };

		inline Bool_t SetNimFclOrReset(Int_t FR) { return(this->Set(TMrbMesytec_Madc32::kRegNimFclOrReset, FR)); };
		inline Bool_t SetNimFclOrReset(Char_t * FR) { return(this->Set(TMrbMesytec_Madc32::kRegNimFclOrReset, FR)); };
		inline Int_t GetNimFclOrReset() { return(this->Get(TMrbMesytec_Madc32::kRegNimFclOrReset)); };

		inline Bool_t SetNimBusy(Int_t Busy) { return(this->Set(TMrbMesytec_Madc32::kRegNimBusy, Busy)); };
		inline Bool_t SetNimBusy(Char_t * Busy) { return(this->Set(TMrbMesytec_Madc32::kRegNimBusy, Busy)); };
		inline Int_t GetNimBusy() { return(this->Get(TMrbMesytec_Madc32::kRegNimBusy)); };

		inline Bool_t SetTestPulser(Int_t Mode) { return(this->Set(TMrbMesytec_Madc32::kRegPulserStatus, Mode)); };
		inline Bool_t SetTestPulser(Char_t * Mode) { return(this->Set(TMrbMesytec_Madc32::kRegPulserStatus, Mode)); };
		inline Int_t GetTestPulser() { return(this->Get(TMrbMesytec_Madc32::kRegPulserStatus)); };

		inline Bool_t SetTsSource(Int_t Source) { return(this->Set(TMrbMesytec_Madc32::kRegTsSource, Source)); };
		inline Bool_t SetTsSource(Char_t * Source) { return(this->Set(TMrbMesytec_Madc32::kRegTsSource, Source)); };
		inline Int_t GetTsSource() { return(this->Get(TMrbMesytec_Madc32::kRegTsSource)); };

		inline Bool_t SetTsDivisor(Int_t Address) { return(this->Set(TMrbMesytec_Madc32::kRegTsDivisor, Address)); };
		inline Int_t GetTsDivisor() { return(this->Get(TMrbMesytec_Madc32::kRegTsDivisor)); };

		Bool_t UseSettings(const Char_t * SettingsFile = NULL);
		Bool_t SaveSettings(const Char_t * SettingsFile = NULL);

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		void DefineRegisters(); 							// define vme registers

	protected:
		TString fSettingsFile;

		Bool_t fBlockXfer;
		Bool_t fSlidingScaleOff;
		Bool_t fSkipOutOfRange;
		Bool_t fAdcOverride;

	ClassDef(TMrbMesytec_Madc32, 1)		// [Config] MADC-32 peak sensing adc
};

#endif

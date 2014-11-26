#ifndef __TMrbMesytec_Madc32_h__
#define __TMrbMesytec_Madc32_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbMesytec_Madc32.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbMesytec_Madc32        -- VME adc
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbMesytec_Madc32.h,v 1.17 2012-01-18 11:11:32 Marabou Exp $
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
#include "TEnv.h"
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
		enum				{	kAddrMod		=	0x09		};

		enum				{	kNofChannels	=	32	};

		enum				{	kGGDefaultDelay	=	20	};
		enum				{	kGGDefaultWidth	=	50	};

		enum EMrbRegisters	{	kRegAddrSource,
								kRegAddrReg,
								kRegModuleId,
								kRegDataWidth,
								kRegMultiEvent,
								kRegXferData,
								kRegMarkingType,
								kRegBankOperation,
								kRegAdcResolution,
								kRegOutputFormat,
								kRegAdcOverride,
								kRegSlidingScaleOff,
								kRegSkipOutOfRange,
								kRegIgnoreThresh,
								kRegHoldDelay,
								kRegHoldWidth,
								kRegUseGG,
								kRegInputRange,
								kRegEclTerm,
								kRegEclG1Osc,
								kRegEclFclRts,
								kRegEclBusy,
								kRegNimG1Osc,
								kRegNimFclRts,
								kRegNimBusy,
								kRegPulserStatus,
								kRegTsSource,
								kRegTsDivisor,
								kRegThreshold
							};

		enum EMrbAddressSource			{	kAddressBoard		=	0,
											kAddressRegister
										};

		enum EMrbDataWidth				{	kDataWidth8		=	0,
											kDataWidth16,
											kDataWidth32,
											kDataWidth64
										};

		enum EMrbMultiEvent				{	kMultiEvtNo			=	0,
											kMultiEvtYes		=	1,
											kMultiEvt1By1		=	3,
											kMultiEvtNoBerr		=	5
										};

		enum EMrbMarkingType			{	kMarkingTypeEvent	=	0x0,
											kMarkingTypeTs		=	0x1,
											kMarkingTypeXts 	=	0x3
										};

		enum EMrbBankOperation			{	kBankOprConnected		=	0x0,
											kBankOprIndependent		=	0x1,
											kBankOprZeroDeadTime	=	0x3
										};

		enum EMrbAdcResolution			{	kAdcRes2k			=	0,
											kAdcRes4k,
											kAdcRes4kHiRes,
											kAdcRes8k,
											kAdcRes8kHiRes,
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
											kEclTermG0 			=	BIT(0),
											kEclTermG1 			=	BIT(1),
											kEclTermBusy 	 	=	BIT(2),
											kEclTermFcl 	 	=	BIT(3),
											kEclTermOn			=	kEclTermG0 | kEclTermG1 | kEclTermBusy | kEclTermFcl
										};

		enum EMrbEclG1Osc 	 			{	kEclG1				=	0,
											kEclOsc
										};

		enum EMrbEclFclRts				{	kEclFcl 			=	0,
											kEclRts
										};

		enum EMrbNimG1Osc	 			{	kNimG1				=	0,
											kNimOsc
										};

		enum EMrbNimFclRts				{	kNimFcl 			=	0,
											kNimRts
										};

		enum EMrbNimBusy		 		{	kNimBusy			=	0,
											kNimG0Out	=	1,
											kNimG1Out	=	2,
											kNimCbusOut	=	3,
											kNimBufferFull	=	4,
											kNimOverThresh 	=	8
										};

		enum EMrbPulserStatus		 	{	kPulserOff			=	0,
											kPulserAmpl			=	4,
											kPulserAmplLow		=	5,
											kPulserAmplHigh		=	6,
											kPulserAmplToggle	=	7
										};

		enum EMrbPulserDac				{	kPulserDacDefault	=	32		};

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

		virtual inline const Char_t * GetMnemonic() const { return("madc32"); }; 	// module mnemonic

		inline void SetBlockXfer(Bool_t Flag = kTRUE) { fBlockXfer = Flag; };
		inline Bool_t BlockXferEnabled() { return(fBlockXfer); };

		inline void RepairRawData(Bool_t Flag = kTRUE) { fRepairRawData = Flag; };
		inline Bool_t RawDataToBeRepaired() { return(fRepairRawData); };

		inline Bool_t SetThreshold(Int_t Thresh, Int_t Channel = -1) { return(this->Set(TMrbMesytec_Madc32::kRegThreshold, Thresh, Channel)); };
		inline Int_t GetThreshold(Int_t Channel) { return(this->Get(TMrbMesytec_Madc32::kRegThreshold, Channel)); };

		inline Bool_t SetAddressSource(Int_t AddrSource) { return(this->Set(TMrbMesytec_Madc32::kRegAddrSource, AddrSource)); };
		inline Bool_t SetAddressSource(Char_t * AddrSource) { return(this->Set(TMrbMesytec_Madc32::kRegAddrSource, AddrSource)); };
		inline Int_t GetAddressSource() { return(this->Get(TMrbMesytec_Madc32::kRegAddrSource)); };

		inline Bool_t SetAddressRegister(Int_t Address) { return(this->Set(TMrbMesytec_Madc32::kRegAddrReg, Address)); };
		inline Int_t GetAddressRegister() { return(this->Get(TMrbMesytec_Madc32::kRegAddrReg)); };

		inline Bool_t SetModuleId(Int_t ModuleId) { return(this->Set(TMrbMesytec_Madc32::kRegModuleId, ModuleId)); };
		inline Int_t GetModuleId() { return(this->Get(TMrbMesytec_Madc32::kRegModuleId)); };

		inline Bool_t SetXferData(Int_t Length) { return(this->Set(TMrbMesytec_Madc32::kRegXferData, Length)); };
		inline Int_t GetXferData() { return(this->Get(TMrbMesytec_Madc32::kRegXferData)); };

		inline Bool_t SetDataWidth(Int_t Format) { return(this->Set(TMrbMesytec_Madc32::kRegDataWidth, Format)); };
		inline Bool_t SetDataWidth(Char_t * Format) { return(this->Set(TMrbMesytec_Madc32::kRegDataWidth, Format)); };
		inline Int_t GetDataWidth() { return(this->Get(TMrbMesytec_Madc32::kRegDataWidth)); };

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

		inline void SetIgnoreThresholds(Bool_t Flag) { fIgnoreThresh = Flag; };
		inline Bool_t IgnoreThresholds() { return(fIgnoreThresh); };

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

		inline Bool_t SetEclG1Osc(Int_t GO) { return(this->Set(TMrbMesytec_Madc32::kRegEclG1Osc, GO)); };
		inline Bool_t SetEclG1Osc(Char_t * GO) { return(this->Set(TMrbMesytec_Madc32::kRegEclG1Osc, GO)); };
		inline Int_t GetEclG1Osc() { return(this->Get(TMrbMesytec_Madc32::kRegEclG1Osc)); };

		inline Bool_t SetEclFclRts(Int_t FR) { return(this->Set(TMrbMesytec_Madc32::kRegEclFclRts, FR)); };
		inline Bool_t SetEclFclRts(Char_t * FR) { return(this->Set(TMrbMesytec_Madc32::kRegEclFclRts, FR)); };
		inline Int_t GetEclFclRts() { return(this->Get(TMrbMesytec_Madc32::kRegEclFclRts)); };

		inline Bool_t SetNimG1Osc(Int_t GO) { return(this->Set(TMrbMesytec_Madc32::kRegNimG1Osc, GO)); };
		inline Bool_t SetNimG1Osc(Char_t * GO) { return(this->Set(TMrbMesytec_Madc32::kRegNimG1Osc, GO)); };
		inline Int_t GetNimG1Osc() { return(this->Get(TMrbMesytec_Madc32::kRegNimG1Osc)); };

		inline Bool_t SetNimFclRts(Int_t FR) { return(this->Set(TMrbMesytec_Madc32::kRegNimFclRts, FR)); };
		inline Bool_t SetNimFclRts(Char_t * FR) { return(this->Set(TMrbMesytec_Madc32::kRegNimFclRts, FR)); };
		inline Int_t GetNimFclRts() { return(this->Get(TMrbMesytec_Madc32::kRegNimFclRts)); };

		inline Bool_t SetNimBusy(Int_t Busy) { return(this->Set(TMrbMesytec_Madc32::kRegNimBusy, Busy)); };
		inline Bool_t SetNimBusy(Char_t * Busy) { return(this->Set(TMrbMesytec_Madc32::kRegNimBusy, Busy)); };
		inline Int_t GetNimBusy() { return(this->Get(TMrbMesytec_Madc32::kRegNimBusy)); };

		inline void SetBufferThresh(Int_t Thresh) { fBufferThresh = Thresh; };
		inline Int_t GetBufferThresh() { return(fBufferThresh); };

		inline Bool_t SetPulserStatus(Int_t Mode) { return(this->Set(TMrbMesytec_Madc32::kRegPulserStatus, Mode)); };
		inline Bool_t SetPulserStatus(Char_t * Mode) { return(this->Set(TMrbMesytec_Madc32::kRegPulserStatus, Mode)); };
		inline Int_t GetPulserStatus() { return(this->Get(TMrbMesytec_Madc32::kRegPulserStatus)); };

		inline Bool_t SetTsSource(Int_t Source) { return(this->Set(TMrbMesytec_Madc32::kRegTsSource, Source)); };
		inline Bool_t SetTsSource(Char_t * Source) { return(this->Set(TMrbMesytec_Madc32::kRegTsSource, Source)); };
		inline Int_t GetTsSource() { return(this->Get(TMrbMesytec_Madc32::kRegTsSource)); };

		inline Bool_t SetTsDivisor(Int_t Divisor) { return(this->Set(TMrbMesytec_Madc32::kRegTsDivisor, Divisor)); };
		inline Int_t GetTsDivisor() { return(this->Get(TMrbMesytec_Madc32::kRegTsDivisor)); };

		TEnv * UseSettings(const Char_t * SettingsFile = NULL);
		Bool_t SaveSettings(const Char_t * SettingsFile = NULL);
		inline TEnv * Settings() { return fSettings; };

		inline void EnableMCST(UInt_t Signature, Bool_t FirstInChain= kFALSE, Bool_t LastInChain = kFALSE) { fMCSTSignature = Signature; fFirstInChain = FirstInChain; fLastInChain = LastInChain; };
		inline void EnableCBLT(UInt_t Signature, Bool_t FirstInChain= kFALSE, Bool_t LastInChain = kFALSE) { fCBLTSignature = Signature; fFirstInChain = FirstInChain; fLastInChain = LastInChain; };
		inline void SetMcstSignature(UInt_t Signature) { fMCSTSignature = Signature; };
		inline void SetCbltSignature(UInt_t Signature) { fCBLTSignature = Signature; };
		inline void SetFirstInChain(Bool_t Flag) { fFirstInChain = Flag; };
		inline void SetLastInChain(Bool_t Flag) { fLastInChain = Flag; };
		inline Bool_t McstEnabled() { return (fMCSTSignature != 0); };
		inline Bool_t CbltEnabled() { return (fCBLTSignature != 0); };
		inline Bool_t IsFirstInChain() { return fFirstInChain; };
		inline Bool_t IsLastInChain() { return fLastInChain; };

		void PrintSettings(ostream & OutStrm);
		inline void PrintSettings() { this->PrintSettings(cout); };
		const Char_t * FormatValue(TString & Value, Int_t Index, Int_t SubIndex = -1, Int_t Base = 10);

		inline Bool_t HasPrivateCode() const { return(kTRUE); }; 			// use private code files
		inline const Char_t * GetPrivateCodeFile() const { return("Module_Mesytec"); };

		inline Bool_t HasBlockXfer() const { return(kTRUE); };				// module is capable of using BLT
		inline Bool_t UseA32Addressing() const { return(kTRUE); };			// we use a32 regardless of address

		virtual Bool_t CheckSubeventType(TMrbSubevent * Subevent) const;		// check if subevent type is [10,8x]

		inline Char_t * GetDeviceStruct() { return(Form("s_%s", this->GetName())); };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		void DefineRegisters(); 							// define vme registers
		Bool_t UpdateSettings();							// update settings

	protected:
		TString fSettingsFile;
		TEnv * fSettings;

		Bool_t fBlockXfer;
		Bool_t fRepairRawData;
		Bool_t fSlidingScaleOff;
		Bool_t fSkipOutOfRange;
		Bool_t fIgnoreThresh;
		Bool_t fAdcOverride;
		Int_t fBufferThresh;

		UInt_t fMCSTSignature;
		UInt_t fCBLTSignature;
		Bool_t fFirstInChain;
		Bool_t fLastInChain;

	ClassDef(TMrbMesytec_Madc32, 1)		// [Config] MADC-32 peak sensing adc
};

#endif

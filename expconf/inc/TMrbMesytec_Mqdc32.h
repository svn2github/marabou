#ifndef __TMrbMesytec_Mqdc32_h__
#define __TMrbMesytec_Mqdc32_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbMesytec_Mqdc32.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbMesytec_Mqdc32        -- VME qdc
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbMesytec_Mqdc32.h,v 1.17 2012-01-18 11:11:32 Marabou Exp $
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
// Name:           TMrbMesytec_Mqdc32
// Purpose:        Define a VME module type Mesytec MQDC32
// Description:    Defines a VME module MQDC32
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbMesytec_Mqdc32 : public TMrbVMEModule {

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
								kRegBankOffset,
								kRegSlidingScaleOff,
								kRegSkipOutOfRange,
								kRegIgnoreThresh,
								kRegGateLimit,
								kRegInputCoupling,
								kRegEclTerm,
								kRegEclG1OrOsc,
								kRegEclFclOrRts,
								kRegGateSelect,
								kRegNimG1OrOsc,
								kRegNimFclOrRts,
								kRegNimBusy,
								kRegPulserStatus,
								kRegPulserDac,
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

		enum EMrbBankOperation			{	kBankOprConnected	=	0,
											kBankOprIndependent
										};

		enum EMrbAdcResolution			{	kAdcRes4k			=	0
										};

		enum EMrbBankOffset				{	kBankOffsetDefault	=	128		};

		enum EMrbGateLimit				{	kGateLimitOff		=	255		};

		enum EMrbInputCoupling			{	kInpCplAC			=	0,
											kInpCplDC
										};

		enum EMrbEclTerm	 			{	kEclTermOff 		=	0,
											kEclTermG0 			=	BIT(0),
											kEclTermG1 			=	BIT(1),
											kEclTermFcl 	 	=	BIT(2),
											kEclTermIndivBank0	=	BIT(3),
											kEclTermIndivBank1	=	BIT(4),
											kEclTermOn			=	kEclTermG0 | kEclTermG1 | kEclTermFcl | kEclTermIndivBank0 | kEclTermIndivBank1,
											kEclTermBanksIndiv	=	kEclTermIndivBank0 | kEclTermIndivBank1
										};

		enum EMrbEclG1OrOsc 	 		{	kEclG1				=	0,
											kEclOsc
										};

		enum EMrbEclFclOrRes			{	kEclFcl 			=	0,
											kEclRts
										};

		enum EMrbGateSelect				{	kGateSelNim			=	0,
											kGateSelEcl
										};

		enum EMrbNimG1OrOsc	 			{	kNimG1				=	0,
											kNimOsc
										};

		enum EMrbNimFclOrRts			{	kNimFcl 			=	0,
											kNimRts
										};

		enum EMrbNimBusy		 		{	kNimBusy			=	0,
											kNimG0Out			=	1,
											kNimG1Out			=	2,
											kNimCbusOut			=	3,
											kNimBufferFull		=	4,
											kNimAboveThresh		=	8
										};

		enum EMrbTestPulser		 		{	kPulserOff			=	0,
											kPulserAmpl			=	4,
											kPulserAmplLow		=	5,
											kPulserAmplHigh		=	6,
											kPulserAmplToggle	=	7
										};

		enum EMrbTestPulserDac			{	kPulserDacDefault	=	32		};

		enum EMrbTsSource		 		{	kTstampVME			=	0,
											kTstampExtern		=	BIT(0),
											kTstampReset		=	BIT(1)
										};

	public:

		TMrbMesytec_Mqdc32() {};  													// default ctor
		TMrbMesytec_Mqdc32(const Char_t * ModuleName, UInt_t BaseAddr); 			// define a new module
		~TMrbMesytec_Mqdc32() {};													// default dtor

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TMrbVMEChannel * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() const { return("madc32"); }; 	// module mnemonic

		inline void SetBlockXfer(Bool_t Flag = kTRUE) { fBlockXfer = Flag; };
		inline Bool_t BlockXferEnabled() { return(fBlockXfer); };

		inline void RepairRawData(Bool_t Flag = kTRUE) { fRepairRawData = Flag; };
		inline Bool_t RawDataToBeRepaired() { return(fRepairRawData); };

		inline Bool_t SetThreshold(Int_t Thresh, Int_t Channel = -1) { return(this->Set(TMrbMesytec_Mqdc32::kRegThreshold, Thresh, Channel)); };
		inline Int_t GetThreshold(Int_t Channel) { return(this->Get(TMrbMesytec_Mqdc32::kRegThreshold, Channel)); };

		inline Bool_t SetAddressSource(Int_t AddrSource) { return(this->Set(TMrbMesytec_Mqdc32::kRegAddrSource, AddrSource)); };
		inline Bool_t SetAddressSource(Char_t * AddrSource) { return(this->Set(TMrbMesytec_Mqdc32::kRegAddrSource, AddrSource)); };
		inline Int_t GetAddressSource() { return(this->Get(TMrbMesytec_Mqdc32::kRegAddrSource)); };

		inline Bool_t SetAddressRegister(Int_t Address) { return(this->Set(TMrbMesytec_Mqdc32::kRegAddrReg, Address)); };
		inline Int_t GetAddressRegister() { return(this->Get(TMrbMesytec_Mqdc32::kRegAddrReg)); };

		inline Bool_t SetModuleId(Int_t ModuleId) { return(this->Set(TMrbMesytec_Mqdc32::kRegModuleId, ModuleId)); };
		inline Int_t GetModuleId() { return(this->Get(TMrbMesytec_Mqdc32::kRegModuleId)); };

		inline Bool_t SetXferData(Int_t Length) { return(this->Set(TMrbMesytec_Mqdc32::kRegXferData, Length)); };
		inline Int_t GetXferData() { return(this->Get(TMrbMesytec_Mqdc32::kRegXferData)); };

		inline Bool_t SetDataWidth(Int_t Format) { return(this->Set(TMrbMesytec_Mqdc32::kRegDataWidth, Format)); };
		inline Bool_t SetDataWidth(Char_t * Format) { return(this->Set(TMrbMesytec_Mqdc32::kRegDataWidth, Format)); };
		inline Int_t GetDataWidth() { return(this->Get(TMrbMesytec_Mqdc32::kRegDataWidth)); };

		inline Bool_t SetMultiEvent(Int_t Mode) { return(this->Set(TMrbMesytec_Mqdc32::kRegMultiEvent, Mode)); };
		inline Bool_t SetMultiEvent(Char_t * Mode) { return(this->Set(TMrbMesytec_Mqdc32::kRegMultiEvent, Mode)); };
		inline Int_t GetMultiEvent() { return(this->Get(TMrbMesytec_Mqdc32::kRegMultiEvent)); };

		inline Bool_t SetMarkingType(Int_t Mtype) { return(this->Set(TMrbMesytec_Mqdc32::kRegMarkingType, Mtype)); };
		inline Bool_t SetMarkingType(Char_t * Mtype) { return(this->Set(TMrbMesytec_Mqdc32::kRegMarkingType, Mtype)); };
		inline Int_t GetMarkingType() { return(this->Get(TMrbMesytec_Mqdc32::kRegMarkingType)); };

		inline Bool_t SetBankOperation(Int_t Oper) { return(this->Set(TMrbMesytec_Mqdc32::kRegBankOperation, Oper)); };
		inline Bool_t SetBankOperation(Char_t * Oper) { return(this->Set(TMrbMesytec_Mqdc32::kRegBankOperation, Oper)); };
		inline Int_t GetBankOperation() { return(this->Get(TMrbMesytec_Mqdc32::kRegBankOperation)); };

		inline Bool_t SetAdcResolution(Int_t Res) { return(this->Set(TMrbMesytec_Mqdc32::kRegAdcResolution, Res)); };
		inline Bool_t SetAdcResolution(Char_t * Res) { return(this->Set(TMrbMesytec_Mqdc32::kRegAdcResolution, Res)); };
		inline Int_t GetAdcResolution() { return(this->Get(TMrbMesytec_Mqdc32::kRegAdcResolution)); };

		inline Bool_t SetBankOffset(Int_t Offset, Int_t Bank) { return(this->Set(TMrbMesytec_Mqdc32::kRegBankOffset, Offset, Bank)); };
		inline Int_t GetBankOffset(Int_t Bank) { return(this->Get(TMrbMesytec_Mqdc32::kRegBankOffset, Bank)); };

		inline void SetSlidingScaleOff(Bool_t Flag) { fSlidingScaleOff = Flag; };
		inline Bool_t SlidingScaleIsOff() { return(fSlidingScaleOff); };

		inline void SetSkipOutOfRange(Bool_t Flag) { fSkipOutOfRange = Flag; };
		inline Bool_t SkipOutOfRange() { return(fSkipOutOfRange); };

		inline void SetIgnoreThresholds(Bool_t Flag) { fIgnoreThresh = Flag; };
		inline Bool_t IgnoreThresholds() { return(fIgnoreThresh); };

		inline Bool_t SetGateLimit(Int_t Limit, Int_t Bank) { return(this->Set(TMrbMesytec_Mqdc32::kRegGateLimit, Limit, Bank)); };
		inline Int_t GetGateLimit(Int_t Bank) { return(this->Get(TMrbMesytec_Mqdc32::kRegGateLimit, Bank)); };

		inline Bool_t SetInputCoupling(Int_t Coupling) { return(this->Set(TMrbMesytec_Mqdc32::kRegInputCoupling, Coupling)); };
		inline Bool_t SetInputCoupling(Char_t * Coupling) { return(this->Set(TMrbMesytec_Mqdc32::kRegInputCoupling, Coupling)); };
		inline Int_t GetInputCoupling() { return(this->Get(TMrbMesytec_Mqdc32::kRegInputCoupling)); };

		inline Bool_t SetEclTerm(Int_t Term) { return(this->Set(TMrbMesytec_Mqdc32::kRegEclTerm, Term)); };
		inline Bool_t SetEclTerm(Char_t * Term) { return(this->Set(TMrbMesytec_Mqdc32::kRegEclTerm, Term)); };
		inline Int_t GetEclTerm() { return(this->Get(TMrbMesytec_Mqdc32::kRegEclTerm)); };

		inline Bool_t SetEclG1OrOsc(Int_t GO) { return(this->Set(TMrbMesytec_Mqdc32::kRegEclG1OrOsc, GO)); };
		inline Bool_t SetEclG1OrOsc(Char_t * GO) { return(this->Set(TMrbMesytec_Mqdc32::kRegEclG1OrOsc, GO)); };
		inline Int_t GetEclG1OrOsc() { return(this->Get(TMrbMesytec_Mqdc32::kRegEclG1OrOsc)); };

		inline Bool_t SetEclFclOrRts(Int_t FR) { return(this->Set(TMrbMesytec_Mqdc32::kRegEclFclOrRts, FR)); };
		inline Bool_t SetEclFclOrRts(Char_t * FR) { return(this->Set(TMrbMesytec_Mqdc32::kRegEclFclOrRts, FR)); };
		inline Int_t GetEclFclOrRts() { return(this->Get(TMrbMesytec_Mqdc32::kRegEclFclOrRts)); };

		inline Bool_t SetGateSelect(Int_t Gate) { return(this->Set(TMrbMesytec_Mqdc32::kRegGateSelect, Gate)); };
		inline Bool_t SetGateSelect(Char_t * Gate) { return(this->Set(TMrbMesytec_Mqdc32::kRegGateSelect, Gate)); };
		inline Int_t GetGateSelect() { return(this->Get(TMrbMesytec_Mqdc32::kRegGateSelect)); };

		inline Bool_t SetNimG1OrOsc(Int_t GO) { return(this->Set(TMrbMesytec_Mqdc32::kRegNimG1OrOsc, GO)); };
		inline Bool_t SetNimG1OrOsc(Char_t * GO) { return(this->Set(TMrbMesytec_Mqdc32::kRegNimG1OrOsc, GO)); };
		inline Int_t GetNimG1OrOsc() { return(this->Get(TMrbMesytec_Mqdc32::kRegNimG1OrOsc)); };

		inline Bool_t SetNimFclOrRts(Int_t FR) { return(this->Set(TMrbMesytec_Mqdc32::kRegNimFclOrRts, FR)); };
		inline Bool_t SetNimFclOrRts(Char_t * FR) { return(this->Set(TMrbMesytec_Mqdc32::kRegNimFclOrRts, FR)); };
		inline Int_t GetNimFclOrRts() { return(this->Get(TMrbMesytec_Mqdc32::kRegNimFclOrRts)); };

		inline Bool_t SetNimBusy(Int_t Busy) { return(this->Set(TMrbMesytec_Mqdc32::kRegNimBusy, Busy)); };
		inline Bool_t SetNimBusy(Char_t * Busy) { return(this->Set(TMrbMesytec_Mqdc32::kRegNimBusy, Busy)); };
		inline Int_t GetNimBusy() { return(this->Get(TMrbMesytec_Mqdc32::kRegNimBusy)); };

		inline void SetBufferThresh(Int_t Thresh) { fBufferThresh = Thresh; };
		inline Int_t GetBufferThresh() { return(fBufferThresh); };

		inline Bool_t SetTestPulser(Int_t Mode) { return(this->Set(TMrbMesytec_Mqdc32::kRegPulserStatus, Mode)); };
		inline Bool_t SetTestPulser(Char_t * Mode) { return(this->Set(TMrbMesytec_Mqdc32::kRegPulserStatus, Mode)); };
		inline Int_t GetTestPulser() { return(this->Get(TMrbMesytec_Mqdc32::kRegPulserStatus)); };

		inline Bool_t SetTsSource(Int_t Source) { return(this->Set(TMrbMesytec_Mqdc32::kRegTsSource, Source)); };
		inline Bool_t SetTsSource(Char_t * Source) { return(this->Set(TMrbMesytec_Mqdc32::kRegTsSource, Source)); };
		inline Int_t GetTsSource() { return(this->Get(TMrbMesytec_Mqdc32::kRegTsSource)); };

		inline Bool_t SetTsDivisor(Int_t Divisor) { return(this->Set(TMrbMesytec_Mqdc32::kRegTsDivisor, Divisor)); };
		inline Int_t GetTsDivisor() { return(this->Get(TMrbMesytec_Mqdc32::kRegTsDivisor)); };

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

		TEnv * UseSettings(const Char_t * SettingsFile = NULL);
		Bool_t SaveSettings(const Char_t * SettingsFile = NULL);

		inline void UpdateSettings(Bool_t Flag = kTRUE) { fUpdateSettings = Flag; };	// update settings
		inline Bool_t SettingsToBeUpdated() { return(fUpdateSettings); };
		inline void SetUpdateInterval(Int_t Interval) { fUpdateInterval = Interval; };
		inline Int_t GetUpdateInterval() { return(fUpdateInterval); };

		void PrintSettings(ostream & OutStrm);
		inline void PrintSettings() { this->PrintSettings(cout); };
		const Char_t * FormatValue(TString & Value, Int_t Index, Int_t SubIndex = -1, Int_t Base = 10);

		inline Bool_t HasPrivateCode() const { return(kTRUE); }; 			// use private code files
		inline const Char_t * GetPrivateCodeFile() const { return("Module_Mqdc"); };

		inline Bool_t HasBlockXfer() const { return(kTRUE); };				// module is capable of using BLT
		inline Bool_t UseA32Addressing() const { return(kTRUE); };			// we use a32 regardless of address

 		virtual Bool_t CheckSubeventType(TMrbSubevent * Subevent) const;		// check if subevent type is [10,8x]

		inline Char_t * GetDeviceStruct() { return(Form("s_%s", this->GetName())); };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		void DefineRegisters(); 							// define vme registers

	protected:
		TString fSettingsFile;
		TEnv * fSettings;

		Bool_t fUpdateSettings;
		Int_t fUpdateInterval;
		Bool_t fBlockXfer;
		Bool_t fRepairRawData;
		Bool_t fSlidingScaleOff;
		Bool_t fSkipOutOfRange;
		Bool_t fIgnoreThresh;
		Int_t fBufferThresh;
		
		UInt_t fMCSTSignature;
		UInt_t fCBLTSignature;
		Bool_t fFirstInChain;
		Bool_t fLastInChain;

	ClassDef(TMrbMesytec_Mqdc32, 1)		// [Config] MQDC-32 charge integrating qdc
};

#endif

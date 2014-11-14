#ifndef __TMrbMesytec_Mtdc32_h__
#define __TMrbMesytec_Mtdc32_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbMesytec_Mtdc32.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbMesytec_Mtdc32        -- VME tdc
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbMesytec_Mtdc32.h,v 1.17 2012-01-18 11:11:32 Marabou Exp $
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
// Name:           TMrbMesytec_Mtdc32
// Purpose:        Define a VME module type Mesytec MADC32
// Description:    Defines a VME module MADC32
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbMesytec_Mtdc32 : public TMrbVMEModule {

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
								kRegTdcResolution,
								kRegOutputFormat,
								kRegWinStart,
								kRegWinWidth,
								kRegTrigSrcTrig,
								kRegTrigSrcChan,
								kRegTrigSrcBank,
								kRegFirstHit,
								kRegNegEdge,
								kRegEclTerm,
								kRegEclT1Osc,
								kRegEclFclRts,
								kRegTrigSelect,
								kRegNimT1Osc,
								kRegNimFclRts,
								kRegNimBusy,
								kRegPulserStatus,
								kRegPulserPattern,
								kRegInputThresh,
								kRegTsSource,
								kRegTsDivisor,
								kRegMultHighLimit,
								kRegMultLowLimit
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

		enum EMrbTdcResolution			{	kTdcRes500			=	9,
											kTdcRes250			=	8,
											kTdcRes125			=	7,
											kTdcRes62_5			=	6,
											kTdcRes31_3			=	5,
											kTdcRes15_6			=	4,
											kTdcRes7_8			=	3,
											kTdcRes3_9			=	2
										};

		enum EMrbOutputFormat			{	kOutFmtTimeDiff			=	0,
											kOutFmtSingleHitFullTs	=	1
										};

		enum EMrbFirstHit	 			{	kFirstHitOnly		=	1,
											kFirstHitAll		=	0
										};

		enum EMrbWinStart	 			{	kWinStartDefault		=	16384 - 16	};
		enum EMrbWinWidth	 			{	kWinWidthDefault		=	32	};

		enum EMrbEclTerm	 			{	kEclTermOff 		=	0,
											kEclTermT0 			=	BIT(0),
											kEclTermT1 			=	BIT(1),
											kEclTermRes		 	=	BIT(2),
											kEclTermOn			=	kEclTermT0 | kEclTermT1 | kEclTermRes
										};

		enum EMrbEclT1Osc 	 			{	kEclT1				=	0,
											kEclOsc
										};

		enum EMrbTrigSelect				{	kTrigSelNim			=	0,
											kTrigSelEcl
										};

		enum EMrbNimT1Osc	 			{	kNimT1				=	0,
											kNimOsc
										};

		enum EMrbNimBusy		 		{	kNimBusy			=	0,
											kNimCbusOut			=	3,
											kNimBufferFull		=	4,
											kNimOverThresh	 	=	8
										};

		enum EMrbPulserStatus		 	{	kPulserOff			=	0,
											kPulserOn			=	1
										};
										
		enum EMrbInputThresh	 		{	kInputThreshDefault		=	105	};
		
		enum EMrbTsSource		 		{	kTstampVME			=	0,
											kTstampExtern		=	BIT(0),
											kTstampReset		=	BIT(1)
										};
										
		enum EMrbMultLimit				{	kMultHighLimit0	=	32,
											kMultHighLimit1	=	16,
											kMultLowLimit	=	0
										};


	public:

		TMrbMesytec_Mtdc32() {};  													// default ctor
		TMrbMesytec_Mtdc32(const Char_t * ModuleName, UInt_t BaseAddr); 			// define a new module
		~TMrbMesytec_Mtdc32() {};													// default dtor

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TMrbVMEChannel * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() const { return("mtdc32"); }; 	// module mnemonic

		inline void SetBlockXfer(Bool_t Flag = kTRUE) { fBlockXfer = Flag; };
		inline Bool_t BlockXferEnabled() { return(fBlockXfer); };

		inline void RepairRawData(Bool_t Flag = kTRUE) { fRepairRawData = Flag; };
		inline Bool_t RawDataToBeRepaired() { return(fRepairRawData); };

		inline Bool_t SetAddressSource(Int_t AddrSource) { return(this->Set(TMrbMesytec_Mtdc32::kRegAddrSource, AddrSource)); };
		inline Bool_t SetAddressSource(Char_t * AddrSource) { return(this->Set(TMrbMesytec_Mtdc32::kRegAddrSource, AddrSource)); };
		inline Int_t GetAddressSource() { return(this->Get(TMrbMesytec_Mtdc32::kRegAddrSource)); };

		inline Bool_t SetAddressRegister(Int_t Address) { return(this->Set(TMrbMesytec_Mtdc32::kRegAddrReg, Address)); };
		inline Int_t GetAddressRegister() { return(this->Get(TMrbMesytec_Mtdc32::kRegAddrReg)); };

		inline Bool_t SetModuleId(Int_t ModuleId) { return(this->Set(TMrbMesytec_Mtdc32::kRegModuleId, ModuleId)); };
		inline Int_t GetModuleId() { return(this->Get(TMrbMesytec_Mtdc32::kRegModuleId)); };

		inline Bool_t SetXferData(Int_t Length) { return(this->Set(TMrbMesytec_Mtdc32::kRegXferData, Length)); };
		inline Int_t GetXferData() { return(this->Get(TMrbMesytec_Mtdc32::kRegXferData)); };

		inline Bool_t SetDataWidth(Int_t Format) { return(this->Set(TMrbMesytec_Mtdc32::kRegDataWidth, Format)); };
		inline Bool_t SetDataWidth(Char_t * Format) { return(this->Set(TMrbMesytec_Mtdc32::kRegDataWidth, Format)); };
		inline Int_t GetDataWidth() { return(this->Get(TMrbMesytec_Mtdc32::kRegDataWidth)); };

		inline Bool_t SetMultiEvent(Int_t Mode) { return(this->Set(TMrbMesytec_Mtdc32::kRegMultiEvent, Mode)); };
		inline Bool_t SetMultiEvent(Char_t * Mode) { return(this->Set(TMrbMesytec_Mtdc32::kRegMultiEvent, Mode)); };
		inline Int_t GetMultiEvent() { return(this->Get(TMrbMesytec_Mtdc32::kRegMultiEvent)); };

		inline Bool_t SetMarkingType(Int_t Mtype) { return(this->Set(TMrbMesytec_Mtdc32::kRegMarkingType, Mtype)); };
		inline Bool_t SetMarkingType(Char_t * Mtype) { return(this->Set(TMrbMesytec_Mtdc32::kRegMarkingType, Mtype)); };
		inline Int_t GetMarkingType() { return(this->Get(TMrbMesytec_Mtdc32::kRegMarkingType)); };

		inline Bool_t SetBankOperation(Int_t Oper) { return(this->Set(TMrbMesytec_Mtdc32::kRegBankOperation, Oper)); };
		inline Bool_t SetBankOperation(Char_t * Oper) { return(this->Set(TMrbMesytec_Mtdc32::kRegBankOperation, Oper)); };
		inline Int_t GetBankOperation() { return(this->Get(TMrbMesytec_Mtdc32::kRegBankOperation)); };

		inline Bool_t SetTdcResolution(Int_t Res) { return(this->Set(TMrbMesytec_Mtdc32::kRegTdcResolution, Res)); };
		inline Bool_t SetTdcResolution(Char_t * Res) { return(this->Set(TMrbMesytec_Mtdc32::kRegTdcResolution, Res)); };
		inline Int_t GetTdcResolution() { return(this->Get(TMrbMesytec_Mtdc32::kRegTdcResolution)); };

		inline Bool_t SetOutputFormat(Int_t Format) { return(this->Set(TMrbMesytec_Mtdc32::kRegOutputFormat, Format)); };
		inline Bool_t SetOutputFormat(Char_t * Format) { return(this->Set(TMrbMesytec_Mtdc32::kRegOutputFormat, Format)); };
		inline Int_t GetOutputFormat() { return(this->Get(TMrbMesytec_Mtdc32::kRegOutputFormat)); };

		inline Bool_t SetWinStart(Int_t Offset, Int_t Bank = 0) { return(this->Set(TMrbMesytec_Mtdc32::kRegWinStart, Offset, Bank)); };
		inline Int_t GetWinStart(Int_t Bank = 0) { return(this->Get(TMrbMesytec_Mtdc32::kRegWinStart, Bank)); };

		inline Bool_t SetWinWidth(Int_t Width, Int_t Bank = 0) { return(this->Set(TMrbMesytec_Mtdc32::kRegWinWidth, Width, Bank)); };
		inline Int_t GetWinWidth(Int_t Bank = 0) { return(this->Get(TMrbMesytec_Mtdc32::kRegWinWidth, Bank)); };

		inline Bool_t SetTrigSrcTrig(Int_t Trig, Int_t B = 0) { return(this->Set(TMrbMesytec_Mtdc32::kRegTrigSrcTrig, Trig, B)); };
		inline Int_t GetTrigSrcTrig(Int_t B = 0) { return(this->Get(TMrbMesytec_Mtdc32::kRegTrigSrcTrig, B)); };

		inline Bool_t SetTrigSrcChan(Int_t Chan, Int_t B = 0) { return(this->Set(TMrbMesytec_Mtdc32::kRegTrigSrcChan, Chan, B)); };
		inline Int_t GetTrigSrcChan(Int_t B = 0) { return(this->Get(TMrbMesytec_Mtdc32::kRegTrigSrcChan, B)); };

		inline Bool_t SetTrigSrcBank(Int_t Bank, Int_t B = 0) { return(this->Set(TMrbMesytec_Mtdc32::kRegTrigSrcBank, Bank, B)); };
		inline Int_t GetTrigSrcBank(Int_t B = 0) { return(this->Get(TMrbMesytec_Mtdc32::kRegTrigSrcBank, B)); };

		inline Bool_t SetFirstHit(Int_t Bits) { return(this->Set(TMrbMesytec_Mtdc32::kRegFirstHit, Bits)); };
		inline Int_t GetFirstHit() { return(this->Get(TMrbMesytec_Mtdc32::kRegFirstHit)); };
		
		inline Bool_t SetNegativeEdge(Int_t Edge) { return(this->Set(TMrbMesytec_Mtdc32::kRegNegEdge, Edge)); };
		inline Int_t GetNegativeEdge() { return(this->Get(TMrbMesytec_Mtdc32::kRegNegEdge)); };

		inline Bool_t SetEclTerm(Int_t Term) { return(this->Set(TMrbMesytec_Mtdc32::kRegEclTerm, Term)); };
		inline Bool_t SetEclTerm(Char_t * Term) { return(this->Set(TMrbMesytec_Mtdc32::kRegEclTerm, Term)); };
		inline Int_t GetEclTerm() { return(this->Get(TMrbMesytec_Mtdc32::kRegEclTerm)); };

		inline Bool_t SetEclT1Osc(Int_t TO) { return(this->Set(TMrbMesytec_Mtdc32::kRegEclT1Osc, TO)); };
		inline Bool_t SetEclT1Osc(Char_t * TO) { return(this->Set(TMrbMesytec_Mtdc32::kRegEclT1Osc, TO)); };
		inline Int_t GetEclT1Osc() { return(this->Get(TMrbMesytec_Mtdc32::kRegEclT1Osc)); };

		inline Bool_t SetTrigSelect(Int_t Trig) { return(this->Set(TMrbMesytec_Mtdc32::kRegTrigSelect, Trig)); };
		inline Bool_t SetTrigSelect(Char_t * Trig) { return(this->Set(TMrbMesytec_Mtdc32::kRegTrigSelect, Trig)); };
		inline Int_t GetTrigSelect() { return(this->Get(TMrbMesytec_Mtdc32::kRegTrigSelect)); };

		inline Bool_t SetNimT1Osc(Int_t TO) { return(this->Set(TMrbMesytec_Mtdc32::kRegNimT1Osc, TO)); };
		inline Bool_t SetNimT1Osc(Char_t * TO) { return(this->Set(TMrbMesytec_Mtdc32::kRegNimT1Osc, TO)); };
		inline Int_t GetNimT1Osc() { return(this->Get(TMrbMesytec_Mtdc32::kRegNimT1Osc)); };

		inline Bool_t SetNimBusy(Int_t Busy) { return(this->Set(TMrbMesytec_Mtdc32::kRegNimBusy, Busy)); };
		inline Bool_t SetNimBusy(Char_t * Busy) { return(this->Set(TMrbMesytec_Mtdc32::kRegNimBusy, Busy)); };
		inline Int_t GetNimBusy() { return(this->Get(TMrbMesytec_Mtdc32::kRegNimBusy)); };

		inline void SetBufferThresh(Int_t Thresh) { fBufferThresh = Thresh; };
		inline Int_t GetBufferThresh() { return(fBufferThresh); };

		inline void SetPulserStatus(Bool_t OnOff) { fPulserStatus = OnOff; };
		inline Bool_t PulserOn() { return fPulserStatus; };

		inline Bool_t SetPulserPattern(Int_t Pattern) { return(this->Set(TMrbMesytec_Mtdc32::kRegPulserPattern, Pattern)); };
		inline Int_t GetPulserPattern() { return(this->Get(TMrbMesytec_Mtdc32::kRegPulserPattern)); };

		inline Bool_t SetInputThresh(Int_t Thresh, Int_t Bank = 0) { return(this->Set(TMrbMesytec_Mtdc32::kRegInputThresh, Thresh, Bank)); };
		inline Int_t GetInputThresh(Int_t Bank = 0) { return(this->Get(TMrbMesytec_Mtdc32::kRegWinStart, Bank)); };

		inline Bool_t SetTsSource(Int_t Source) { return(this->Set(TMrbMesytec_Mtdc32::kRegTsSource, Source)); };
		inline Bool_t SetTsSource(Char_t * Source) { return(this->Set(TMrbMesytec_Mtdc32::kRegTsSource, Source)); };
		inline Int_t GetTsSource() { return(this->Get(TMrbMesytec_Mtdc32::kRegTsSource)); };

		inline Bool_t SetTsDivisor(Int_t Divisor) { return(this->Set(TMrbMesytec_Mtdc32::kRegTsDivisor, Divisor)); };
		inline Int_t GetTsDivisor() { return(this->Get(TMrbMesytec_Mtdc32::kRegTsDivisor)); };

		inline Bool_t SetMultHighLimit(Int_t Limit, Int_t Bank) { return(this->Set(TMrbMesytec_Mtdc32::kRegMultHighLimit, Limit, Bank)); };
		inline Int_t GetMultHighLimit(Int_t Bank) { return(this->Get(TMrbMesytec_Mtdc32::kRegMultHighLimit, Bank)); };
		inline Bool_t SetMultLowLimit(Int_t Limit, Int_t Bank) { return(this->Set(TMrbMesytec_Mtdc32::kRegMultLowLimit, Limit, Bank)); };
		inline Int_t GetMultLowLimit(Int_t Bank) { return(this->Get(TMrbMesytec_Mtdc32::kRegMultLowLimit, Bank)); };

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
		inline const Char_t * GetPrivateCodeFile() const { return("Module_Mtdc"); };

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
		Bool_t fPulserStatus;
		Int_t fBufferThresh;

		UInt_t fMCSTSignature;
		UInt_t fCBLTSignature;
		Bool_t fFirstInChain;
		Bool_t fLastInChain;

	ClassDef(TMrbMesytec_Mtdc32, 1)		// [Config] MTDC-32 tdc
};

#endif

#ifndef __TMrbMesytec_Mdpp16_h__
#define __TMrbMesytec_Mdpp16_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbMesytec_Mdpp16.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbMesytec_Mdpp16        -- VME adc
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbMesytec_Mdpp16.h,v 1.17 2012-01-18 11:11:32 Marabou Exp $
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
// Name:           TMrbMesytec_Mdpp16
// Purpose:        Define a VME module type Mesytec MDPP16
// Description:    Defines a VME module MDPP16
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbMesytec_Mdpp16 : public TMrbVMEModule {

	public:
		enum				{	kSegSize		=	0x10000L	};
		enum				{	kAddrMod		=	0x09		};

		enum				{	kNofChannels		=	16	};
		enum				{	kNofChannelPairs	=	8	};

		enum				{	kChanAct		=	0x20	};

		enum EMrbRegisters	{	kRegAddrSource,
								kRegAddrReg,
								kRegModuleId,
								kRegDataWidth,
								kRegMultiEvent,
								kRegXferData,
								kRegMarkingType,
								kRegTdcResolution,
								kRegAdcResolution,
								kRegOutputFormat,
								kRegWinStart,
								kRegWinWidth,
								kRegTrigSource,
								kRegFirstHit,
								kRegTrigOutput,
								kRegEcl3,
								kRegEcl2,
								kRegEcl1,
								kRegEcl0,
								kRegNim4,
								kRegNim3,
								kRegNim2,
								kRegNim1,
								kRegNim0,
								kRegPulserStatus,
								kRegPulserAmplitude,
								kRegMonitor,
								kRegMonitorChannel,
								kRegMonitorWave,
								kRegTsSource,
								kRegTsDivisor,
								kRegMultHighLimit,
								kRegMultLowLimit,
								kRegChannelPair,
								kRegTFIntDiff,
								kRegPoleZero,
								kRegGain,
								kRegThresh,
								kRegShaping,
								kRegBaseLineRes,
								kRegResetTime,
								kRegRiseTime
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
											kMultiEvtLim		=	3,
											kMultiEvtNoBerr		=	5
										};

		enum EMrbMarkingType			{	kMarkingTypeEvent	=	0x0,
											kMarkingTypeTs		=	0x1,
											kMarkingTypeXts 	=	0x3
										};

		enum EMrbOutputFormat			{	kOutFmtStandard		=	0,
											kOutFmtAmplitude,
											kOutFmtTime
										};

		enum EMrbAdcResolution			{	kAdcRes16			=	0,
											kAdcRes15,
											kAdcRes14,
											kAdcRes13,
											kAdcRes12
										};

		enum EMrbTdcResolution			{	kTdcRes781_32		=	5,
											kTdcRes391_64		=	4,
											kTdcRes195_128		=	3,
											kTdcRes98_256		=	2,
											kTdcRes49_512		=	1,
											kTdcRes24_1024		=	0
										};

		enum							{	kWinStartDefault	=	0x3ff0	};
		enum							{	kWinWidthDefault	=	0x20	};
		
		enum EMrbFirstHit	 			{	kFirstHitOnly		=	1,
											kFirstHitAll		=	0
										};

		enum EMrbEcl3		 			{	kEcl3Off		 	=	0,
											kEcl3Trig0 			=	BIT(0),
											kEcl3UnTerm 	 	=	BIT(4)
										};

		enum EMrbEcl2		 			{	kEcl2Off	 		=	0,
											kEcl2Sync 			=	BIT(0),
											kEcl2Trig1			=	BIT(1),
											kEcl2UnTerm 	 	=	BIT(4)
										};

		enum EMrbEcl1		 			{	kEcl1Off	 		=	0,
											kEcl1Reset 			=	BIT(0),
											kEcl1UnTerm 	 	=	BIT(4)
										};

		enum EMrbEcl0		 			{	kEcl0Off	 		=	0,
											kEcl0Busy 			=	BIT(2),
											kEcl0DataReady		=	BIT(3)
										};

		enum EMrbNim4		 			{	kNim4Off			=	0,
											kNim4Trig0			=	BIT(0)
										};

		enum EMrbNim3		 			{	kNim3Off			=	0,
											kNim3Sync			=	BIT(1)
										};

		enum EMrbNim2		 			{	kNim2Off			=	0,
											kNim2Trig1			=	BIT(0),
											kNim2Reset			=	BIT(1)
										};

		enum EMrbNim0		 			{	kNim0Off			=	0,
											kNim0Cbus			=	BIT(0),
											kNim0BusyOut		=	BIT(2),
											kNim0DataReady		=	BIT(3)
										};

		enum EMrbPulserStatus		 	{	kPulserOff			=	0,
											kPulserOn			=	1
										};

		enum EMrbMonitor			 	{	kMonitorOff			=	0,
											kMonitorOn			=	1
										};

		enum							{	kPulserMaxAmpl		=	0xFFF	};
		
		enum EMrbTsSource		 		{	kTstampVME			=	0,
											kTstampExtern		=	BIT(0),
											kTstampReset		=	BIT(1)
										};
										
		enum EMrbMultLimit				{	kMultHighLimit	=	255,
											kMultLowLimit	=	0
										};

		enum EMrbBaseLineRes			{	kBaseLineResOff		=	0,
											kBaseLineResSoft	=	1,
											kBaseLineResStrict	=	2
										};
										
		enum							{	kValueNotSet		=	-1	};
		enum							{	kValueToBeInit		=	-2	};
		
		enum							{	kTFIntDiffDefault	=	2	};
		enum							{	kPoleZeroDefault	=	0xFFFF	};
		enum							{	kGainDefault		=	200	};
		enum							{	kThreshDefault		=	0xFF	};
		enum							{	kShapingDefault		=	0x64	};
		enum							{	kBLResDefault		=	2	};
		enum							{	kResetTimeDefault	=	16	};
		enum							{	kRiseTimeDefault	=	0	};

	public:

		TMrbMesytec_Mdpp16() {};  													// default ctor
		TMrbMesytec_Mdpp16(const Char_t * ModuleName, UInt_t BaseAddr); 			// define a new module
		~TMrbMesytec_Mdpp16() {};													// default dtor

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TMrbVMEChannel * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() const { return("mdpp16"); }; 	// module mnemonic

		inline void SetBlockXfer(Bool_t Flag = kTRUE) { fBlockXfer = Flag; };
		inline Bool_t BlockXferEnabled() { return(fBlockXfer); };

		inline void RepairRawData(Bool_t Flag = kTRUE) { fRepairRawData = Flag; };
		inline Bool_t RawDataToBeRepaired() { return(fRepairRawData); };

		inline Bool_t SetAddressSource(Int_t AddrSource) { return(this->Set(TMrbMesytec_Mdpp16::kRegAddrSource, AddrSource)); };
		inline Bool_t SetAddressSource(Char_t * AddrSource) { return(this->Set(TMrbMesytec_Mdpp16::kRegAddrSource, AddrSource)); };
		inline Int_t GetAddressSource() { return(this->Get(TMrbMesytec_Mdpp16::kRegAddrSource)); };

		inline Bool_t SetAddressRegister(Int_t Address) { return(this->Set(TMrbMesytec_Mdpp16::kRegAddrReg, Address)); };
		inline Int_t GetAddressRegister() { return(this->Get(TMrbMesytec_Mdpp16::kRegAddrReg)); };

		inline Bool_t SetModuleId(Int_t ModuleId) { return(this->Set(TMrbMesytec_Mdpp16::kRegModuleId, ModuleId)); };
		inline Int_t GetModuleId() { return(this->Get(TMrbMesytec_Mdpp16::kRegModuleId)); };

		inline Bool_t SetXferData(Int_t Length) { return(this->Set(TMrbMesytec_Mdpp16::kRegXferData, Length)); };
		inline Int_t GetXferData() { return(this->Get(TMrbMesytec_Mdpp16::kRegXferData)); };

		inline Bool_t SetDataWidth(Int_t Format) { return(this->Set(TMrbMesytec_Mdpp16::kRegDataWidth, Format)); };
		inline Bool_t SetDataWidth(Char_t * Format) { return(this->Set(TMrbMesytec_Mdpp16::kRegDataWidth, Format)); };
		inline Int_t GetDataWidth() { return(this->Get(TMrbMesytec_Mdpp16::kRegDataWidth)); };

		inline Bool_t SetMultiEvent(Int_t Mode) { return(this->Set(TMrbMesytec_Mdpp16::kRegMultiEvent, Mode)); };
		inline Bool_t SetMultiEvent(Char_t * Mode) { return(this->Set(TMrbMesytec_Mdpp16::kRegMultiEvent, Mode)); };
		inline Int_t GetMultiEvent() { return(this->Get(TMrbMesytec_Mdpp16::kRegMultiEvent)); };

		inline Bool_t SetMarkingType(Int_t Mtype) { return(this->Set(TMrbMesytec_Mdpp16::kRegMarkingType, Mtype)); };
		inline Bool_t SetMarkingType(Char_t * Mtype) { return(this->Set(TMrbMesytec_Mdpp16::kRegMarkingType, Mtype)); };
		inline Int_t GetMarkingType() { return(this->Get(TMrbMesytec_Mdpp16::kRegMarkingType)); };

		inline Bool_t SetTdcResolution(Int_t Res) { return(this->Set(TMrbMesytec_Mdpp16::kRegTdcResolution, Res)); };
		inline Bool_t SetTdcResolution(Char_t * Res) { return(this->Set(TMrbMesytec_Mdpp16::kRegTdcResolution, Res)); };
		inline Int_t GetTdcResolution() { return(this->Get(TMrbMesytec_Mdpp16::kRegTdcResolution)); };

		inline Bool_t SetAdcResolution(Int_t Res) { return(this->Set(TMrbMesytec_Mdpp16::kRegAdcResolution, Res)); };
		inline Bool_t SetAdcResolution(Char_t * Res) { return(this->Set(TMrbMesytec_Mdpp16::kRegAdcResolution, Res)); };
		inline Int_t GetAdcResolution() { return(this->Get(TMrbMesytec_Mdpp16::kRegAdcResolution)); };

		inline Bool_t SetOutputFormat(Int_t Format) { return(this->Set(TMrbMesytec_Mdpp16::kRegOutputFormat, Format)); };
		inline Bool_t SetOutputFormat(Char_t * Format) { return(this->Set(TMrbMesytec_Mdpp16::kRegOutputFormat, Format)); };
		inline Int_t GetOutputFormat() { return(this->Get(TMrbMesytec_Mdpp16::kRegOutputFormat)); };

		inline Bool_t SetWinStart(Int_t Offset) { return(this->Set(TMrbMesytec_Mdpp16::kRegWinStart, Offset)); };
		inline Bool_t GetWinStart() { return(this-Get(TMrbMesytec_Mdpp16::kRegWinStart)); };
		inline Bool_t SetWinWidth(Int_t Width) { return(this->Set(TMrbMesytec_Mdpp16::kRegWinWidth, Width)); };
		inline Bool_t GetWinWidth() { return(this->Get(TMrbMesytec_Mdpp16::kRegWinWidth)); };

		inline Bool_t SetTrigSource(Int_t TrigSource) { return(this->Set(TMrbMesytec_Mdpp16::kRegTrigSource, TrigSource)); };
		Bool_t SetTrigSource(Int_t Trig, Int_t Chan, Int_t Bank);
		inline Int_t GetTrigSource() { return(this->Get(TMrbMesytec_Mdpp16::kRegTrigSource)); };
		Bool_t GetTrigSource(TArrayI & TrigSource);

		inline Bool_t SetFirstHit(Int_t Hit) { return(this->Set(TMrbMesytec_Mdpp16::kRegFirstHit, Hit)); };
		inline Bool_t SetFirstHit(Char_t * Hit) { return(this->Set(TMrbMesytec_Mdpp16::kRegFirstHit, Hit)); };
		inline Int_t GetFirstHit() { return(this->Get(TMrbMesytec_Mdpp16::kRegFirstHit)); };

		inline Bool_t SetTrigOutput(Int_t TrigOutput) { return(this->Set(TMrbMesytec_Mdpp16::kRegTrigOutput, TrigOutput)); };
		Bool_t SetTrigOutput(Int_t Chan, Int_t Bank);
		inline Int_t GetTrigOutput() { return(this->Get(TMrbMesytec_Mdpp16::kRegTrigOutput)); };
		Bool_t GetTrigOutput(TArrayI & TrigOutput);

		inline Bool_t SetEcl3(Int_t Bits) { return(this->Set(TMrbMesytec_Mdpp16::kRegEcl3, Bits)); };
		inline Bool_t SetEcl3(Char_t * Bits) { return(this->Set(TMrbMesytec_Mdpp16::kRegEcl3, Bits)); };
		inline Int_t GetEcl3() { return(this->Get(TMrbMesytec_Mdpp16::kRegEcl3)); };
		
		inline Bool_t SetEcl2(Int_t Bits) { return(this->Set(TMrbMesytec_Mdpp16::kRegEcl2, Bits)); };
		inline Bool_t SetEcl2(Char_t * Bits) { return(this->Set(TMrbMesytec_Mdpp16::kRegEcl2, Bits)); };
		inline Int_t GetEcl2() { return(this->Get(TMrbMesytec_Mdpp16::kRegEcl2)); };
		
		inline Bool_t SetEcl1(Int_t Bits) { return(this->Set(TMrbMesytec_Mdpp16::kRegEcl1, Bits)); };
		inline Bool_t SetEcl1(Char_t * Bits) { return(this->Set(TMrbMesytec_Mdpp16::kRegEcl1, Bits)); };
		inline Int_t GetEcl1() { return(this->Get(TMrbMesytec_Mdpp16::kRegEcl1)); };
		
		inline Bool_t SetEcl0(Int_t Bits) { return(this->Set(TMrbMesytec_Mdpp16::kRegEcl0, Bits)); };
		inline Bool_t SetEcl0(Char_t * Bits) { return(this->Set(TMrbMesytec_Mdpp16::kRegEcl0, Bits)); };
		inline Int_t GetEcl0() { return(this->Get(TMrbMesytec_Mdpp16::kRegEcl0)); };
		
		inline Bool_t SetNim4(Int_t Bits) { return(this->Set(TMrbMesytec_Mdpp16::kRegNim4, Bits)); };
		inline Bool_t SetNim4(Char_t * Bits) { return(this->Set(TMrbMesytec_Mdpp16::kRegNim4, Bits)); };
		inline Int_t GetNim4() { return(this->Get(TMrbMesytec_Mdpp16::kRegNim4)); };
		
		inline Bool_t SetNim3(Int_t Bits) { return(this->Set(TMrbMesytec_Mdpp16::kRegNim3, Bits)); };
		inline Bool_t SetNim3(Char_t * Bits) { return(this->Set(TMrbMesytec_Mdpp16::kRegNim3, Bits)); };
		inline Int_t GetNim3() { return(this->Get(TMrbMesytec_Mdpp16::kRegNim3)); };
		
		inline Bool_t SetNim2(Int_t Bits) { return(this->Set(TMrbMesytec_Mdpp16::kRegNim2, Bits)); };
		inline Bool_t SetNim2(Char_t * Bits) { return(this->Set(TMrbMesytec_Mdpp16::kRegNim2, Bits)); };
		inline Int_t GetNim2() { return(this->Get(TMrbMesytec_Mdpp16::kRegNim2)); };
		
		inline Bool_t SetNim0(Int_t Bits) { return(this->Set(TMrbMesytec_Mdpp16::kRegNim0, Bits)); };
		inline Bool_t SetNim0(Char_t * Bits) { return(this->Set(TMrbMesytec_Mdpp16::kRegNim0, Bits)); };
		inline Int_t GetNim0() { return(this->Get(TMrbMesytec_Mdpp16::kRegNim0)); };
		
		inline Bool_t TurnPulserOn() { return(this->Set(TMrbMesytec_Mdpp16::kRegPulserStatus, TMrbMesytec_Mdpp16::kPulserOn)); };
		inline Bool_t TurnPulserOff() { return(this->Set(TMrbMesytec_Mdpp16::kRegPulserStatus, TMrbMesytec_Mdpp16::kPulserOff)); };
		inline Bool_t PulserOn() { return(this->Get(TMrbMesytec_Mdpp16::kRegPulserStatus) == TMrbMesytec_Mdpp16::kPulserOn ? kTRUE : kFALSE); };

		inline Bool_t SetPulserAmplitude(Int_t Ampl) { return(this->Set(TMrbMesytec_Mdpp16::kRegPulserAmplitude, Ampl)); };
		inline Int_t GetPulserAmplitude() { return(this->Get(TMrbMesytec_Mdpp16::kRegPulserAmplitude)); };

		inline Bool_t SetTsSource(Int_t Source) { return(this->Set(TMrbMesytec_Mdpp16::kRegTsSource, Source)); };
		inline Bool_t SetTsSource(Char_t * Source) { return(this->Set(TMrbMesytec_Mdpp16::kRegTsSource, Source)); };
		inline Int_t GetTsSource() { return(this->Get(TMrbMesytec_Mdpp16::kRegTsSource)); };

		inline Bool_t SetTsDivisor(Int_t Divisor) { return(this->Set(TMrbMesytec_Mdpp16::kRegTsDivisor, Divisor)); };
		inline Int_t GetTsDivisor() { return(this->Get(TMrbMesytec_Mdpp16::kRegTsDivisor)); };

		inline Bool_t TurnMonitorOn() { return(this->Set(TMrbMesytec_Mdpp16::kRegMonitor, TMrbMesytec_Mdpp16::kMonitorOn)); };
		inline Bool_t TurnMonitorOff() { return(this->Set(TMrbMesytec_Mdpp16::kRegMonitor, TMrbMesytec_Mdpp16::kMonitorOff)); };
		inline Bool_t MonitorOn() { return(this->Get(TMrbMesytec_Mdpp16::kRegMonitor) == TMrbMesytec_Mdpp16::kMonitorOn ? kTRUE : kFALSE); };

		inline Bool_t SetMonitorChannel(Int_t Channel) { return(this->Set(TMrbMesytec_Mdpp16::kRegMonitorChannel, Channel)); };
		inline Int_t GetMonitorChannel() { return(this->Get(TMrbMesytec_Mdpp16::kRegMonitorChannel)); };
		inline Bool_t SetMonitorWave(Int_t Wave) { return(this->Set(TMrbMesytec_Mdpp16::kRegMonitorWave, Wave)); };
		inline Int_t GetMonitorWave() { return(this->Get(TMrbMesytec_Mdpp16::kRegMonitorWave)); };

		inline Bool_t SetMultHighLimit(Int_t Limit) { return(this->Set(TMrbMesytec_Mdpp16::kRegMultHighLimit, Limit)); };
		inline Int_t GetMultHighLimit() { return(this->Get(TMrbMesytec_Mdpp16::kRegMultHighLimit)); };
		inline Bool_t SetMultLowLimit(Int_t Limit) { return(this->Set(TMrbMesytec_Mdpp16::kRegMultLowLimit, Limit)); };
		inline Int_t GetMultLowLimit() { return(this->Get(TMrbMesytec_Mdpp16::kRegMultLowLimit)); };

		inline Bool_t SetTFIntDiff(Int_t TFval, Int_t Chan) { return(this->Set(TMrbMesytec_Mdpp16::kRegTFIntDiff, TFval, Chan)); };
		inline Int_t GetTFIntDiff(Int_t Chan) { return(this->Get(TMrbMesytec_Mdpp16::kRegTFIntDiff, Chan)); };

		inline Bool_t SetPoleZero(Int_t PZval, Int_t Chan) { return(this->Set(TMrbMesytec_Mdpp16::kRegPoleZero, PZval, Chan)); };
		inline Int_t GetPoleZero(Int_t Chan) { return(this->Get(TMrbMesytec_Mdpp16::kRegPoleZero, Chan)); };

		inline Bool_t SetGain(Int_t Gain, Int_t Chan) { return(this->Set(TMrbMesytec_Mdpp16::kRegGain, Gain, Chan)); };
		inline Int_t GetGain(Int_t Chan) { return(this->Get(TMrbMesytec_Mdpp16::kRegGain,Chan)); };

		inline Bool_t SetThresh(Int_t Thresh, Int_t Chan) { return(this->Set(TMrbMesytec_Mdpp16::kRegThresh, Thresh, Chan)); };
		inline Int_t GetThresh(Int_t Chan) { return(this->Get(TMrbMesytec_Mdpp16::kRegThresh, Chan)); };

		inline Bool_t SetShaping(Int_t Shaping, Int_t Chan) { return(this->Set(TMrbMesytec_Mdpp16::kRegShaping, Shaping, Chan)); };
		inline Int_t GetShaping(Int_t Chan) { return(this->Get(TMrbMesytec_Mdpp16::kRegShaping, Chan)); };

		inline Bool_t SetBaseLineRes(Int_t BLRval, Int_t Chan) { return(this->Set(TMrbMesytec_Mdpp16::kRegBaseLineRes, BLRval, Chan)); };
		inline Int_t GetBaseLineRes(Int_t Chan) { return(this->Get(TMrbMesytec_Mdpp16::kRegBaseLineRes, Chan)); };

		inline Bool_t SetResetTime(Int_t Reset, Int_t Chan) { return(this->Set(TMrbMesytec_Mdpp16::kRegResetTime, Reset, Chan)); };
		inline Int_t GetResetTime(Int_t Chan) { return(this->Get(TMrbMesytec_Mdpp16::kRegResetTime, Chan)); };

		inline Bool_t SetRiseTime(Int_t Rise, Int_t Chan) { return(this->Set(TMrbMesytec_Mdpp16::kRegRiseTime, Rise)); };
		inline Int_t GetRiseTime(Int_t Chan) { return(this->Get(TMrbMesytec_Mdpp16::kRegRiseTime, Chan)); };
		
		TEnv * UseSettings(const Char_t * SettingsFile = NULL);
		Bool_t SaveSettings(const Char_t * SettingsFile = NULL);
		inline TEnv * Settings() { return fSettings; };

		inline void EnableMCST(UInt_t Signature, Bool_t FirstInChain= kFALSE, Bool_t LastInChain = kFALSE) { fMCSTSignature = Signature; fFirstInChain = FirstInChain; fLastInChain = LastInChain; };
		inline void EnableCBLT(UInt_t Signature, Bool_t FirstInChain= kFALSE, Bool_t LastInChain = kFALSE) { fCBLTSignature = Signature; fFirstInChain = FirstInChain; fLastInChain = LastInChain; };
		inline void SetMcstSignature(UInt_t Signature) { fMCSTSignature = Signature; };
		inline void SetMcstMaster(Bool_t Flag) { fMCSTMaster = Flag; };
		inline void SetCbltSignature(UInt_t Signature) { fCBLTSignature = Signature; };
		inline void SetFirstInChain(Bool_t Flag) { fFirstInChain = Flag; };
		inline void SetLastInChain(Bool_t Flag) { fLastInChain = Flag; };
		inline Bool_t McstEnabled() { return (fMCSTSignature != 0); };
		inline Bool_t IsMcstMaster() { return fMCSTMaster; };
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
		void SetupMCST();									// MCST settings
		void SetupCBLT();									// CBLT settings

	protected:
		TString fSettingsFile;
		TEnv * fSettings;

		Bool_t fBlockXfer;
		Bool_t fRepairRawData;

		Bool_t fMonitorOn;
		
		UInt_t fMCSTSignature;
		Bool_t fMCSTMaster;
		UInt_t fCBLTSignature;
		Bool_t fFirstInChain;
		Bool_t fLastInChain;
		
		Int_t fAllTFIntDiff;
		Int_t fAllPoleZero;
		Int_t fAllGain;
		Int_t fAllThresh;
		Int_t fAllShaping;
		Int_t fAllBaseLineRes;
		Int_t fAllResetTime;
		Int_t fAllRiseTime;

	ClassDef(TMrbMesytec_Mdpp16, 1)		// [Config] MDPP-16 tracing adc
};

#endif

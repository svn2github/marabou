#ifndef __TC2LSis3302_h__
#define __TC2LSis3302_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TC2LSis3302.h
// Purpose:        Connect to a SIS 3302 flash ADC
// Class:          TC2LSis3302            -- base class
// Description:    Class definitions to establish a connection to a VME
//                 module running under LynxOs.
// Author:         R. Lutter
// Revision:       $Id: TC2LSis3302.h,v 1.15 2010-11-17 12:25:11 Marabou Exp $
// Date:           $Date: 2010-11-17 12:25:11 $
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TArrayI.h"
#include "TC2LVMEModule.h"
#include "M2L_CommonDefs.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TC2LSis3302
// Purpose:        Connect to a SIS 3302 ADC
// Description:    Defines a module and tries to connect to it
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TC2LSis3302 : public TC2LVMEModule {

	public:
		enum	{	kRcModuleSettings	=	1	};

	public:

		TC2LSis3302() {};		// default ctor

		TC2LSis3302(const Char_t * ModuleName, UInt_t Address = 0, Int_t NofChans = 0, Bool_t Offline = kFALSE)
								: TC2LVMEModule(ModuleName, "Sis3302", Address, NofChans, Offline) {};

		~TC2LSis3302() {};							// default dtor

		Bool_t GetModuleInfo(Int_t & BoardId, Int_t & MajorVersion, Int_t & MinorVersion);

		Bool_t SetUserLED(Bool_t & OnFlag);

		Bool_t ReadDac(TArrayI & DacValues, Int_t Chan = -1);
		Bool_t WriteDac(TArrayI & DacValues, Int_t Chan = -1);

		Bool_t KeyAddr(Int_t Key);
		inline Bool_t KeyReset() { return(this->KeyAddr(kSis3302KeyReset)); };

		Bool_t ReadEventConfig(Int_t & Bits, Int_t ChanNo);
		Bool_t WriteEventConfig(Int_t & Bits, Int_t ChanNo = kSis3302AllChans);
		Bool_t ReadEventExtendedConfig(Int_t & Bits, Int_t ChanNo);
		Bool_t WriteEventExtendedConfig(Int_t & Bits, Int_t ChanNo = kSis3302AllChans);
		Bool_t GetPolarity(Bool_t & InvertFlag, Int_t ChanNo);
		Bool_t SetPolarity(Bool_t & InvertFlag, Int_t ChanNo = kSis3302AllChans);
		Bool_t GetTriggerMode(Int_t & Bits, Int_t ChanNo);
		Bool_t SetTriggerMode(Int_t & Bits, Int_t ChanNo = kSis3302AllChans);
		Bool_t GetGateMode(Int_t & Bits, Int_t ChanNo);
		Bool_t SetGateMode(Int_t & Bits, Int_t ChanNo = kSis3302AllChans);
		Bool_t GetNextNeighborTriggerMode(Int_t & Bits, Int_t ChanNo);
		Bool_t SetNextNeighborTriggerMode(Int_t & Bits, Int_t ChanNo = kSis3302AllChans);
		Bool_t GetNextNeighborGateMode(Int_t & Bits, Int_t ChanNo);
		Bool_t SetNextNeighborGateMode(Int_t & Bits, Int_t ChanNo = kSis3302AllChans);

		Bool_t GetHeaderBits(Int_t & Bits, Int_t ChanNo);
		Bool_t SetHeaderBits(Int_t & Bits, Int_t ChanNo = kSis3302AllChans);
		Bool_t GetGroupId(Int_t & GroupId, Int_t ChanNo);

		Bool_t ReadEndAddrThresh(Int_t & Thresh, Int_t ChanNo);
		Bool_t WriteEndAddrThresh(Int_t & Thresh, Int_t ChanNo = kSis3302AllChans);

		Bool_t ReadPreTrigDelay(Int_t & Delay, Int_t ChanNo);
		Bool_t WritePreTrigDelay(Int_t & Delay, Int_t ChanNo = kSis3302AllChans);
		Bool_t ReadTrigGateLength(Int_t & Gate, Int_t ChanNo);
		Bool_t WriteTrigGateLength(Int_t & Gate, Int_t ChanNo = kSis3302AllChans);

		Bool_t ReadRawDataSampleLength(Int_t & SampleLength, Int_t ChanNo);
		Bool_t WriteRawDataSampleLength(Int_t & SampleLength, Int_t ChanNo = kSis3302AllChans);
		Bool_t ReadRawDataStartIndex(Int_t & Start, Int_t ChanNo);
		Bool_t WriteRawDataStartIndex(Int_t & Start, Int_t ChanNo = kSis3302AllChans);

		Bool_t ReadNextSampleAddr(Int_t & Addr, Int_t ChanNo);
		Bool_t ReadPrevBankSampleAddr(Int_t & Addr, Int_t ChanNo);
		Bool_t ReadActualSample(Int_t & Data, Int_t ChanNo);

		Bool_t ReadTrigPeakAndGap(Int_t & Peak, Int_t & Gap, Int_t ChanNo);
		Bool_t WriteTrigPeakAndGap(Int_t & Peak, Int_t & Gap, Int_t ChanNo);
		Bool_t ReadTrigPulseLength(Int_t & PulseLength, Int_t ChanNo);
		Bool_t WriteTrigPulseLength(Int_t & PulseLength, Int_t ChanNo);
		Bool_t ReadTrigInternalGate(Int_t & Gate, Int_t ChanNo);
		Bool_t WriteTrigInternalGate(Int_t & Gate, Int_t ChanNo);
		Bool_t ReadTrigInternalDelay(Int_t & Delay, Int_t ChanNo);
		Bool_t WriteTrigInternalDelay(Int_t & Delay, Int_t ChanNo);
		Bool_t GetTrigDecimation(Int_t & Decimation, Int_t ChanNo);
		Bool_t SetTrigDecimation(Int_t & Decimation, Int_t ChanNo = kSis3302AllChans);

		Bool_t ReadTrigThreshold(Int_t & Thresh, Int_t ChanNo);
		Bool_t WriteTrigThreshold(Int_t & Thresh, Int_t ChanNo = kSis3302AllChans);
		Bool_t GetTriggerGT(Bool_t & GTFlag, Int_t ChanNo);
		Bool_t SetTriggerGT(Bool_t & GTFlag, Int_t ChanNo = kSis3302AllChans);
		Bool_t GetTriggerOut(Bool_t & TrigOutFlag, Int_t ChanNo);
		Bool_t SetTriggerOut(Bool_t & TrigOutFlag, Int_t ChanNo = kSis3302AllChans);

		Bool_t ReadEnergyPeakAndGap(Int_t & Peak, Int_t & Gap, Int_t ChanNo);
		Bool_t WriteEnergyPeakAndGap(Int_t & Peak, Int_t & Gap, Int_t ChanNo = kSis3302AllChans);
		Bool_t GetEnergyDecimation(Int_t & Decimation, Int_t ChanNo);
		Bool_t SetEnergyDecimation(Int_t & Decimation, Int_t ChanNo = kSis3302AllChans);
		Bool_t ReadEnergyGateLength(Int_t & GateLength, Int_t ChanNo);
		Bool_t WriteEnergyGateLength(Int_t & GateLength, Int_t ChanNo = kSis3302AllChans);
		Bool_t GetTestBits(Int_t & Bits, Int_t ChanNo);
		Bool_t SetTestBits(Int_t & Bits, Int_t ChanNo = kSis3302AllChans);
		Bool_t ReadEnergySampleLength(Int_t & SampleLength, Int_t ChanNo);
		Bool_t WriteEnergySampleLength(Int_t & SampleLength, Int_t ChanNo = kSis3302AllChans);

		Bool_t ReadStartIndex(Int_t & IdxVal, Int_t IdxNo, Int_t ChanNo);
		Bool_t WriteStartIndex(Int_t & IdxVal, Int_t IdxNo, Int_t ChanNo = kSis3302AllChans);

		Bool_t ReadTauFactor(Int_t & Tau, Int_t ChanNo);
		Bool_t WriteTauFactor(Int_t & Tau, Int_t ChanNo = kSis3302AllChans);

		Bool_t GetLemoInMode(Int_t & Bits);
		Bool_t SetLemoInMode(Int_t & Bits);
		Bool_t GetLemoOutMode(Int_t & Bits);
		Bool_t SetLemoOutMode(Int_t & Bits);
		Bool_t GetLemoInEnableMask(Int_t & Bits);
		Bool_t SetLemoInEnableMask(Int_t & Bits);

		Bool_t GetClockSource(Int_t & ClockSource);
		Bool_t SetClockSource(Int_t & ClockSource);

		Bool_t SaveSettings(const Char_t * SettingsFile = NULL);
		Bool_t RestoreSettings(const Char_t * SettingsFile = NULL);

		Bool_t StartTraceCollection(Int_t & NofEvents, Int_t & ChanPattern);
		Bool_t ContinueTraceCollection();
		Bool_t StopTraceCollection();
		Bool_t GetTraceData(TArrayI & Data, Int_t & EventNo, Int_t ChanNo);
		Bool_t GetTraceLength(TArrayI & Data, Int_t ChanNo);
		Bool_t DumpTrace();

		Bool_t RampDac(TArrayI & Data, Int_t ChanNo);

		inline void SetFirmwareVersion(Int_t Major, Int_t Minor) { fMajorVersion = Major; fMinorVersion = Minor; };
		inline UInt_t GetFirmwareVersion() { return ((fMajorVersion << 8) | fMinorVersion); };
		inline UInt_t GetFirmwareMajor() { return fMajorVersion; };
		inline UInt_t GetFirmwareMinor() { return fMinorVersion; };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		Bool_t ExecFunction(Int_t Fcode, TArrayI & DataSend, TArrayI & DataRecv, Int_t Chan = kSis3302AllChans);

	protected:
		Int_t fMajorVersion;				// firmware version
		Int_t fMinorVersion;

	ClassDef(TC2LSis3302, 1)		// [Access to LynxOs] Connect to a Sis3302 adc
};

#endif

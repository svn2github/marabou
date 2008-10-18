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
// Revision:       $Id: TC2LSis3302.h,v 1.5 2008-10-18 17:09:14 Marabou Exp $   
// Date:           $Date: 2008-10-18 17:09:14 $
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

		TC2LSis3302(const Char_t * ModuleName, UInt_t Address = 0, Int_t NofChannels = 0, Bool_t Offline = kFALSE)
								: TC2LVMEModule(ModuleName, "Sis3302", Address, NofChannels, Offline) {};

		~TC2LSis3302() {};							// default dtor

		Bool_t GetModuleInfo(Int_t & BoardId, Int_t & MajorVersion, Int_t & MinorVersion);

		Bool_t ReadDac(TArrayI & DacValues, Int_t Adc = -1);
		Bool_t WriteDac(TArrayI & DacValues, Int_t Adc = -1);

		Bool_t KeyAddr(Int_t Key);
		inline Bool_t KeyReset() { return(this->KeyAddr(kSis3302KeyReset)); };

		Bool_t ReadEventConfig(Int_t & Bits, Int_t AdcNo);
		Bool_t WriteEventConfig(Int_t & Bits, Int_t AdcNo = kSis3302AllAdcs);
		Bool_t GetPolarity(Bool_t & InvertFlag, Int_t AdcNo);
		Bool_t SetPolarity(Bool_t & InvertFlag, Int_t AdcNo = kSis3302AllAdcs);
		Bool_t GetTriggerMode(Int_t & Bits, Int_t AdcNo);
		Bool_t SetTriggerMode(Int_t & Bits, Int_t AdcNo = kSis3302AllAdcs);

		Bool_t GetHeaderBits(Int_t & Bits, Int_t AdcNo);
		Bool_t SetHeaderBits(Int_t & Bits, Int_t AdcNo = kSis3302AllAdcs);
		Bool_t GetGroupId(Int_t & GroupId, Int_t AdcNo);

		Bool_t ReadEndAddrThresh(Int_t & Thresh, Int_t AdcNo);
		Bool_t WriteEndAddrThresh(Int_t & Thresh, Int_t AdcNo = kSis3302AllAdcs);

		Bool_t ReadPreTrigDelay(Int_t & Delay, Int_t AdcNo);
		Bool_t WritePreTrigDelay(Int_t & Delay, Int_t AdcNo = kSis3302AllAdcs);
		Bool_t ReadTrigGateLength(Int_t & Gate, Int_t AdcNo);
		Bool_t WriteTrigGateLength(Int_t & Gate, Int_t AdcNo = kSis3302AllAdcs);

		Bool_t ReadRawDataSampleLength(Int_t & Bits, Int_t AdcNo);
		Bool_t WriteRawDataSampleLength(Int_t & Bits, Int_t AdcNo = kSis3302AllAdcs);
		Bool_t ReadRawDataStartIndex(Int_t & Bits, Int_t AdcNo);
		Bool_t WriteRawDataStartIndex(Int_t & Bits, Int_t AdcNo = kSis3302AllAdcs);

		Bool_t ReadNextSampleAddr(Int_t & Addr, Int_t AdcNo);
		Bool_t ReadPrevBankSampleAddr(Int_t & Addr, Int_t AdcNo);
		Bool_t ReadActualSample(Int_t & Data, Int_t AdcNo);

		Bool_t ReadTrigPeakAndGap(Int_t & Peak, Int_t & Gap, Int_t AdcNo);
		Bool_t WriteTrigPeakAndGap(Int_t & Peak, Int_t & Gap, Int_t AdcNo);
		Bool_t ReadTrigPulseLength(Int_t & PulseLength, Int_t AdcNo);
		Bool_t WriteTrigPulseLength(Int_t & PulseLength, Int_t AdcNo);

		Bool_t ReadTrigThreshold(Int_t & Thresh, Int_t AdcNo);
		Bool_t WriteTrigThreshold(Int_t & Thresh, Int_t AdcNo = kSis3302AllAdcs);
		Bool_t GetTriggerGT(Bool_t & GTFlag, Int_t AdcNo);
		Bool_t SetTriggerGT(Bool_t & GTFlag, Int_t AdcNo = kSis3302AllAdcs);
		Bool_t GetTriggerOut(Bool_t & TrigOutFlag, Int_t AdcNo);
		Bool_t SetTriggerOut(Bool_t & TrigOutFlag, Int_t AdcNo = kSis3302AllAdcs);

		Bool_t ReadEnergyPeakAndGap(Int_t & Peak, Int_t & Gap, Int_t AdcNo);
		Bool_t WriteEnergyPeakAndGap(Int_t & Peak, Int_t & Gap, Int_t AdcNo = kSis3302AllAdcs);
		Bool_t GetDecimation(Int_t & Decimation, Int_t AdcNo);
		Bool_t SetDecimation(Int_t & Decimation, Int_t AdcNo = kSis3302AllAdcs);
		Bool_t ReadEnergyGateLength(Int_t & GateLength, Int_t AdcNo);
		Bool_t WriteEnergyGateLength(Int_t & GateLength, Int_t AdcNo = kSis3302AllAdcs);
		Bool_t GetTestBits(Int_t & Bits, Int_t AdcNo);
		Bool_t SetTestBits(Int_t & Bits, Int_t AdcNo = kSis3302AllAdcs);
		Bool_t ReadEnergySampleLength(Int_t & SampleLength, Int_t AdcNo);
		Bool_t WriteEnergySampleLength(Int_t & SampleLength, Int_t AdcNo = kSis3302AllAdcs);

		Bool_t ReadStartIndex(Int_t & IdxVal, Int_t IdxNo, Int_t AdcNo);
		Bool_t WriteStartIndex(Int_t & IdxVal, Int_t IdxNo, Int_t AdcNo = kSis3302AllAdcs);

		Bool_t ReadTauFactor(Int_t & Tau, Int_t AdcNo);
		Bool_t WriteTauFactor(Int_t & Tau, Int_t AdcNo = kSis3302AllAdcs);

		Bool_t GetExternalTrigger(Bool_t & Xtrig);
		Bool_t SetExternalTrigger(Bool_t & Xtrig);
		Bool_t GetExternalTimestampClear(Bool_t & TsClear);
		Bool_t SetExternalTimestampClear(Bool_t & TsClear);
		Bool_t GetInternalTrigger(Bool_t & Itrig);
		Bool_t SetInternalTrigger(Bool_t & Itrig);
		Bool_t GetClockSource(Int_t & ClockSource);
		Bool_t SetClockSource(Int_t & ClockSource);

		Bool_t SaveSettings(const Char_t * SettingsFile = NULL);
		Bool_t RestoreSettings(const Char_t * SettingsFile = NULL);

		Bool_t GetSingleEvent(TArrayI & Data, Int_t AdcNo);

		Bool_t AccuHistogram(TArrayI & Data, Int_t AdcNo, Int_t NofEvents = -1);
		Bool_t StartRun(Int_t AdcNo, Int_t NofEvents = -1);

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };
		
	protected:
		Bool_t ExecFunction(Int_t Fcode, TArrayI & DataSend, TArrayI & DataRecv, Int_t Adc = kSis3302AllAdcs);

	ClassDef(TC2LSis3302, 1)		// [Access to LynxOs] Connect to a Sis3302 adc
};

#endif

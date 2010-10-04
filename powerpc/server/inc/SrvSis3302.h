#ifndef __SrvSis3302_h__
#define __SrvSis3302_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           SrvSis3302.h
// Purpose:        MARaBOU to Lynx: VME modules
// Class:          Sis3302        -- flash adc SIS3302
// Description:    Class definitions for M2L server
// Author:         R. Lutter
// Revision:       $Id: SrvSis3302.h,v 1.1 2010-10-04 10:43:26 Marabou Exp $
// Date:
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "LwrArrayI.h"
#include "SrvVMEModule.h"

#include "M2L_MessageTypes.h"
#include "M2L_CommonStructs.h"
#include "M2L_CommonDefs.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           SrvSis3302
// Purpose:        Define methods for a SIS3302 adc
// Description:    Definitions for SIS3302
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class SrvSis3302 : public SrvVMEModule {

	public:

		SrvSis3302();
		~SrvSis3302() {};

		//! Try to access this module
		Bool_t TryAccess(SrvVMEModule * Module);
		//! Dispatch to given function
		M2L_MsgHdr * Dispatch(SrvVMEModule * Module, TMrbNamedX * Function, M2L_MsgData * Data);

		//! Read module info: board id & version numbers
		Bool_t GetModuleInfo(SrvVMEModule * Module, Int_t & BoardId, Int_t & MajorVersion, Int_t & MinorVersion, Bool_t PrintFlag = kTRUE);

		//! Read dac values
		Bool_t ReadDac(SrvVMEModule * Module, TArrayI & DacValues, Int_t AdcNo);
		//! Write dac values
		Bool_t WriteDac(SrvVMEModule * Module, TArrayI & DacValues, Int_t AdcNo = kSis3302AllAdcs);

		//! Switch user LED on/off
		Bool_t SetUserLED(SrvVMEModule * Module, Bool_t & OnFlag);

		//! Exec KEY command
		Bool_t KeyAddr(SrvVMEModule * Module, Int_t Key);
		//! Exec KEY command: reset
		//! \param[in]	Module	-- module address
		inline Bool_t KeyReset(SrvVMEModule * Module) { return(this->KeyAddr(Module, kSis3302KeyReset)); };
		//! Exec KEY command: reset sample
		//! \param[in]	Module	-- module address
		inline Bool_t KeyResetSample(SrvVMEModule * Module) { return(this->KeyAddr(Module, kSis3302KeyResetSample)); };
		//! Exec KEY command: generate trigger
		//! \param[in]	Module	-- module address
		inline Bool_t KeyTrigger(SrvVMEModule * Module) { return(this->KeyAddr(Module, kSis3302KeyTrigger)); };
		//! Exec KEY command: clear time stamp
		//! \param[in]	Module	-- module address
		inline Bool_t KeyClearTimestamp(SrvVMEModule * Module) { return(this->KeyAddr(Module, kSis3302KeyClearTimestamp)); };
		//! Exec KEY command: arm sampling for bank 1
		//! \param[in]	Module	-- module address
		inline Bool_t KeyArmBank1Sampling(SrvVMEModule * Module) { return(this->KeyAddr(Module, kSis3302KeyArmBank1Sampling)); };
		//! Exec KEY command: arm sampling for bank 2
		//! \param[in]	Module	-- module address
		inline Bool_t KeyArmBank2Sampling(SrvVMEModule * Module) { return(this->KeyAddr(Module, kSis3302KeyArmBank2Sampling)); };

		//! control and status
		Bool_t ReadControlStatus(SrvVMEModule * Module, Int_t & Bits);
		Bool_t WriteControlStatus(SrvVMEModule * Module, Int_t & Bits);
		//! event configuration
		Bool_t ReadEventConfig(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo);
		Bool_t WriteEventConfig(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo = kSis3302AllAdcs);
		Bool_t ReadEventExtendedConfig(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo);
		Bool_t WriteEventExtendedConfig(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo = kSis3302AllAdcs);
		//! event header & group id
		Bool_t SetHeaderBits(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo = kSis3302AllAdcs);
		Bool_t GetHeaderBits(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo);
		Bool_t GetGroupId(SrvVMEModule * Module, Int_t & GroupId, Int_t AdcNo);
		//! trigger mode
		Bool_t SetTriggerMode(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo = kSis3302AllAdcs);
		Bool_t GetTriggerMode(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo);
		//! gate mode
		Bool_t SetGateMode(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo = kSis3302AllAdcs);
		Bool_t GetGateMode(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo);
		//! next neighbors, trigger & gate
		Bool_t SetNextNeighborTriggerMode(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo = kSis3302AllAdcs);
		Bool_t GetNextNeighborTriggerMode(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo);
		Bool_t SetNextNeighborGateMode(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo = kSis3302AllAdcs);
		Bool_t GetNextNeighborGateMode(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo);
		//! trigger polarity
		Bool_t SetPolarity(SrvVMEModule * Module, Bool_t & InvertFlag, Int_t AdcNo = kSis3302AllAdcs);
		Bool_t GetPolarity(SrvVMEModule * Module, Bool_t & InvertFlag, Int_t AdcNo);

		//! end address threshold
		Bool_t ReadEndAddrThresh(SrvVMEModule * Module, Int_t & Thresh, Int_t AdcNo);
		Bool_t WriteEndAddrThresh(SrvVMEModule * Module, Int_t & Thresh, Int_t AdcNo = kSis3302AllAdcs);

		//! pre-trigger delay and gate length
		Bool_t ReadPreTrigDelay(SrvVMEModule * Module, Int_t & Delay, Int_t AdcNo);
		Bool_t WritePreTrigDelay(SrvVMEModule * Module, Int_t & Delay, Int_t AdcNo = kSis3302AllAdcs);
		Bool_t ReadTrigGateLength(SrvVMEModule * Module, Int_t & Gate, Int_t AdcNo);
		Bool_t WriteTrigGateLength(SrvVMEModule * Module, Int_t & Gate, Int_t AdcNo = kSis3302AllAdcs);

		//! raw data sample length & start
		Bool_t ReadRawDataSampleLength(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo);
		Bool_t WriteRawDataSampleLength(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo = kSis3302AllAdcs);
		Bool_t ReadRawDataStartIndex(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo);
		Bool_t WriteRawDataStartIndex(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo = kSis3302AllAdcs);

		Bool_t ReadNextSampleAddr(SrvVMEModule * Module, Int_t & Addr, Int_t AdcNo);
		Bool_t ReadPrevBankSampleAddr(SrvVMEModule * Module, Int_t & Addr, Int_t AdcNo);
		Bool_t ReadActualSample(SrvVMEModule * Module, Int_t & Data, Int_t AdcNo);

		//! trigger peak & gap
		Bool_t ReadTriggerPeakAndGap(SrvVMEModule * Module, Int_t & Peak, Int_t & Gap, Int_t AdcNo);
		Bool_t WriteTriggerPeakAndGap(SrvVMEModule * Module, Int_t & Peak, Int_t & Gap, Int_t AdcNo = kSis3302AllAdcs);
		//! trigger out pulse length
		Bool_t ReadTriggerPulseLength(SrvVMEModule * Module, Int_t & PulseLength, Int_t AdcNo);
		Bool_t WriteTriggerPulseLength(SrvVMEModule * Module, Int_t & PulseLength, Int_t AdcNo = kSis3302AllAdcs);
		//! trigger internal gate & delay
		Bool_t ReadTriggerInternalGate(SrvVMEModule * Module, Int_t & GateLength, Int_t AdcNo);
		Bool_t WriteTriggerInternalGate(SrvVMEModule * Module, Int_t & GateLength, Int_t AdcNo = kSis3302AllAdcs);
		Bool_t ReadTriggerInternalDelay(SrvVMEModule * Module, Int_t & DelayLength, Int_t AdcNo);
		Bool_t WriteTriggerInternalDelay(SrvVMEModule * Module, Int_t & DelayLength, Int_t AdcNo = kSis3302AllAdcs);
		//! trigger decimation
		Bool_t GetTriggerDecimation(SrvVMEModule * Module, Int_t & Decimation, Int_t AdcNo);
		Bool_t SetTriggerDecimation(SrvVMEModule * Module, Int_t & Decimation, Int_t AdcNo = kSis3302AllAdcs);

		//! trigger threshold
		Bool_t ReadTriggerThreshold(SrvVMEModule * Module, Int_t & Thresh, Int_t AdcNo);
		Bool_t WriteTriggerThreshold(SrvVMEModule * Module, Int_t & Thresh, Int_t AdcNo = kSis3302AllAdcs);
		Bool_t GetTriggerGT(SrvVMEModule * Module, Bool_t & GTFlag, Int_t AdcNo);
		Bool_t SetTriggerGT(SrvVMEModule * Module, Bool_t & GTFlag, Int_t AdcNo = kSis3302AllAdcs);
		Bool_t GetTriggerOut(SrvVMEModule * Module, Bool_t & TrigOutFlag, Int_t AdcNo);
		Bool_t SetTriggerOut(SrvVMEModule * Module, Bool_t & TrigOutFlag, Int_t AdcNo = kSis3302AllAdcs);

		//! energy peak & gap
		Bool_t ReadEnergyPeakAndGap(SrvVMEModule * Module, Int_t & Peak, Int_t & Gap, Int_t AdcNo);
		Bool_t WriteEnergyPeakAndGap(SrvVMEModule * Module, Int_t & Peak, Int_t & Gap, Int_t AdcNo = kSis3302AllAdcs);
		//! energy decimation
		Bool_t GetEnergyDecimation(SrvVMEModule * Module, Int_t & Decimation, Int_t AdcNo);
		Bool_t SetEnergyDecimation(SrvVMEModule * Module, Int_t & Decimation, Int_t AdcNo = kSis3302AllAdcs);
		//! energy gate length
		Bool_t ReadEnergyGateLength(SrvVMEModule * Module, Int_t & PulseLength, Int_t AdcNo);
		Bool_t WriteEnergyGateLength(SrvVMEModule * Module, Int_t & PulseLength, Int_t AdcNo = kSis3302AllAdcs);
		//! energy sample length & start
		Bool_t ReadEnergySampleLength(SrvVMEModule * Module, Int_t & PulseLength, Int_t AdcNo);
		Bool_t WriteEnergySampleLength(SrvVMEModule * Module, Int_t & PulseLength, Int_t AdcNo = kSis3302AllAdcs);
		Bool_t ReadStartIndex(SrvVMEModule * Module, Int_t & IdxVal, Int_t IdxNo, Int_t AdcNo);
		Bool_t WriteStartIndex(SrvVMEModule * Module, Int_t & IdxVal, Int_t IdxNo, Int_t AdcNo = kSis3302AllAdcs);
		//! test bits
		Bool_t GetTestBits(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo);
		Bool_t SetTestBits(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo = kSis3302AllAdcs);


		//! energy tau factor
		Bool_t ReadTauFactor(SrvVMEModule * Module, Int_t & Tau, Int_t AdcNo);
		Bool_t WriteTauFactor(SrvVMEModule * Module, Int_t & Tau, Int_t AdcNo = kSis3302AllAdcs);

		//! clock source
		Bool_t GetClockSource(SrvVMEModule * Module, Int_t & ClockSource);
		Bool_t SetClockSource(SrvVMEModule * Module, Int_t & ClockSource);

		//! lemo in & out
		Bool_t GetLemoInMode(SrvVMEModule * Module, Int_t & Bits);
		Bool_t SetLemoInMode(SrvVMEModule * Module, Int_t & Bits);
		Bool_t GetLemoOutMode(SrvVMEModule * Module, Int_t & Bits);
		Bool_t SetLemoOutMode(SrvVMEModule * Module, Int_t & Bits);
		Bool_t GetLemoInEnableMask(SrvVMEModule * Module, Int_t & Bits);
		Bool_t SetLemoInEnableMask(SrvVMEModule * Module, Int_t & Bits);

		//! start trace, get events
		Bool_t CollectTraces(SrvVMEModule * Module, Int_t & NofEvents, Int_t AdcNo = kSis3302AllAdcs);
		Bool_t GetEvent(SrvVMEModule * Module, TArrayI & Data, Int_t & EventNo, Int_t AdcNo);
		Bool_t GetDataLength(SrvVMEModule * Module, TArrayI & Data, Int_t AdcNo);

	protected:
		void SetupFunction(M2L_MsgData * Data, TArrayI & Array, Int_t & AdcNo);
		M2L_MsgHdr * FinishFunction(TArrayI & Array);

		Bool_t ReadPreTrigDelayAndGateLength(SrvVMEModule * Module, Int_t & Data, Int_t AdcNo);
		Bool_t WritePreTrigDelayAndGateLength(SrvVMEModule * Module, Int_t & Data, Int_t AdcNo = kSis3302AllAdcs);
		Bool_t ReadRawDataBufConfig(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo);
		Bool_t WriteRawDataBufConfig(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo = kSis3302AllAdcs);
		Bool_t ReadTriggerSetup(SrvVMEModule * Module, Int_t & Data, Int_t AdcNo);
		Bool_t WriteTriggerSetup(SrvVMEModule * Module, Int_t & Data, Int_t AdcNo);
		Bool_t ReadTriggerExtendedSetup(SrvVMEModule * Module, Int_t & Data, Int_t AdcNo);
		Bool_t WriteTriggerExtendedSetup(SrvVMEModule * Module, Int_t & Data, Int_t AdcNo);
		Bool_t ReadTriggerThreshReg(SrvVMEModule * Module, Int_t & Data, Int_t AdcNo);
		Bool_t WriteTriggerThreshReg(SrvVMEModule * Module, Int_t & Data, Int_t AdcNo);
		Bool_t ReadEnergySetup(SrvVMEModule * Module, Int_t & Data, Int_t AdcNo);
		Bool_t WriteEnergySetup(SrvVMEModule * Module, Int_t & Data, Int_t AdcNo);
		Bool_t ReadEnergyGateReg(SrvVMEModule * Module, Int_t & Data, Int_t AdcNo);
		Bool_t WriteEnergyGateReg(SrvVMEModule * Module, Int_t & Data, Int_t AdcNo);
		Bool_t ReadAcquisitionControl(SrvVMEModule * Module, Int_t & Data);
		Bool_t WriteAcquisitionControl(SrvVMEModule * Module, Int_t & Data);

		Bool_t DataReady(SrvVMEModule * Module);
		Bool_t Timeout(SrvVMEModule * Module, Int_t & Timeout);
		Bool_t ReadData(SrvVMEModule * Module, TArrayI & Data, Int_t NofWords, Int_t AdcNo);

	public:
		inline const Char_t * ClassName() const { return "SrvSis3302"; };

	protected:
		Int_t fTimeout;

};

#endif

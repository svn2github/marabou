#ifndef __SrvSis3302_h__
#define __SrvSis3302_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           SrvSis3302.h
// Purpose:        MARaBOU to Lynx: VME modules
// Class:          Sis3302        -- flash adc SIS3302
// Description:    Class definitions for M2L server
// Author:         R. Lutter
// Revision:       $Id: SrvSis3302.h,v 1.7 2011/07/26 08:41:50 Marabou Exp $
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
		//! Release this module and free memory
		Bool_t Release(SrvVMEModule * Module);
		//! Dispatch to given function
		M2L_MsgHdr * Dispatch(SrvVMEModule * Module, TMrbNamedX * Function, M2L_MsgData * Data = NULL);

		//! Dump register contents
		Bool_t DumpRegisters(SrvVMEModule * Module, Char_t * File = NULL);
		
		//! Read module info: board id & version numbers
		Bool_t GetModuleInfo(SrvVMEModule * Module, Int_t & BoardId, Int_t & MajorVersion, Int_t & MinorVersion, Bool_t PrintFlag = kTRUE);

		//! Check if reduced address space
		Bool_t CheckAddressSpace(SrvVMEModule * Module, Bool_t PrintFlag = kTRUE);
		inline void SetReduced(Bool_t Flag = kTRUE) { fReducedAddrSpace = Flag; };
		inline Bool_t IsReduced() { return(fReducedAddrSpace); };

		//! Read dac values
		Bool_t ReadDac(SrvVMEModule * Module, TArrayI & DacValues, Int_t ChanNo);
		//! Write dac values
		Bool_t WriteDac(SrvVMEModule * Module, Int_t & DacValue, Int_t ChanNo);
		Bool_t WriteDac(SrvVMEModule * Module, TArrayI & DacValues, Int_t ChanNo = kSis3302AllChans);

		//! Switch user LED on/off
		Bool_t SetUserLED(SrvVMEModule * Module, Bool_t & OnFlag);

		//! Exec KEY command
		Bool_t KeyAddr(SrvVMEModule * Module, Int_t Key);
		//! Exec KEY command: reset
		//! \param[in]	Module	-- module address
		inline Bool_t KeyReset(SrvVMEModule * Module) { return(this->KeyAddr(Module, kSis3302KeyReset)); };
		//! Exec KEY command: reset sample
		//! \param[in]	Module	-- module address
		inline Bool_t KeyResetSampling(SrvVMEModule * Module) { return(this->KeyAddr(Module, kSis3302KeyResetSampling)); };
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
		//! Exec KEY command: disarm sample
		//! \param[in]	Module	-- module address
		inline Bool_t KeyDisarmSampling(SrvVMEModule * Module) { return(this->KeyAddr(Module, kSis3302KeyDisarmSampling)); };

		//! control and status
		Bool_t ReadControlStatus(SrvVMEModule * Module, Int_t & Bits);
		Bool_t WriteControlStatus(SrvVMEModule * Module, Int_t & Bits);
		//! event configuration
		Bool_t ReadEventConfig(SrvVMEModule * Module, Int_t & Bits, Int_t ChanNo);
		Bool_t WriteEventConfig(SrvVMEModule * Module, Int_t & Bits, Int_t ChanNo = kSis3302AllChans);
		Bool_t ReadEventExtendedConfig(SrvVMEModule * Module, Int_t & Bits, Int_t ChanNo);
		Bool_t WriteEventExtendedConfig(SrvVMEModule * Module, Int_t & Bits, Int_t ChanNo = kSis3302AllChans);
		//! event header & group id
		Bool_t SetHeaderBits(SrvVMEModule * Module, Int_t & Bits, Int_t ChanNo = kSis3302AllChans);
		Bool_t GetHeaderBits(SrvVMEModule * Module, Int_t & Bits, Int_t ChanNo);
		Bool_t GetGroupId(SrvVMEModule * Module, Int_t & GroupId, Int_t ChanNo);
		//! trigger mode
		Bool_t SetTriggerMode(SrvVMEModule * Module, Int_t & Bits, Int_t ChanNo = kSis3302AllChans);
		Bool_t GetTriggerMode(SrvVMEModule * Module, Int_t & Bits, Int_t ChanNo);
		//! gate mode
		Bool_t SetGateMode(SrvVMEModule * Module, Int_t & Bits, Int_t ChanNo = kSis3302AllChans);
		Bool_t GetGateMode(SrvVMEModule * Module, Int_t & Bits, Int_t ChanNo);
		//! next neighbors, trigger & gate
		Bool_t SetNextNeighborTriggerMode(SrvVMEModule * Module, Int_t & Bits, Int_t ChanNo = kSis3302AllChans);
		Bool_t GetNextNeighborTriggerMode(SrvVMEModule * Module, Int_t & Bits, Int_t ChanNo);
		Bool_t SetNextNeighborGateMode(SrvVMEModule * Module, Int_t & Bits, Int_t ChanNo = kSis3302AllChans);
		Bool_t GetNextNeighborGateMode(SrvVMEModule * Module, Int_t & Bits, Int_t ChanNo);
		//! trigger polarity
		Bool_t SetPolarity(SrvVMEModule * Module, Bool_t & InvertFlag, Int_t ChanNo = kSis3302AllChans);
		Bool_t GetPolarity(SrvVMEModule * Module, Bool_t & InvertFlag, Int_t ChanNo);

		//! end address threshold
		Bool_t ReadEndAddrThresh(SrvVMEModule * Module, Int_t & Thresh, Int_t ChanNo);
		Bool_t WriteEndAddrThresh(SrvVMEModule * Module, Int_t & Thresh, Int_t ChanNo = kSis3302AllChans);

		//! pre-trigger delay and gate length
		Bool_t ReadPreTrigDelay(SrvVMEModule * Module, Int_t & Delay, Int_t ChanNo);
		Bool_t WritePreTrigDelay(SrvVMEModule * Module, Int_t & Delay, Int_t ChanNo = kSis3302AllChans);
		Bool_t ReadTrigGateLength(SrvVMEModule * Module, Int_t & Gate, Int_t ChanNo);
		Bool_t WriteTrigGateLength(SrvVMEModule * Module, Int_t & Gate, Int_t ChanNo = kSis3302AllChans);

		//! raw data sample length & start
		Bool_t ReadRawDataSampleLength(SrvVMEModule * Module, Int_t & Bits, Int_t ChanNo);
		Bool_t WriteRawDataSampleLength(SrvVMEModule * Module, Int_t & Bits, Int_t ChanNo = kSis3302AllChans);
		Bool_t ReadRawDataStartIndex(SrvVMEModule * Module, Int_t & Bits, Int_t ChanNo);
		Bool_t WriteRawDataStartIndex(SrvVMEModule * Module, Int_t & Bits, Int_t ChanNo = kSis3302AllChans);

		Bool_t ReadNextSampleAddr(SrvVMEModule * Module, Int_t & Addr, Int_t ChanNo);
		Bool_t ReadPrevBankSampleAddr(SrvVMEModule * Module, Int_t & Addr, Int_t ChanNo);
		Bool_t ReadActualSample(SrvVMEModule * Module, Int_t & Data, Int_t ChanNo);

		//! trigger peak & gap
		Bool_t ReadTriggerPeakAndGap(SrvVMEModule * Module, Int_t & Peak, Int_t & Gap, Int_t ChanNo);
		Bool_t WriteTriggerPeakAndGap(SrvVMEModule * Module, Int_t & Peak, Int_t & Gap, Int_t ChanNo = kSis3302AllChans);
		//! trigger out pulse length
		Bool_t ReadTriggerPulseLength(SrvVMEModule * Module, Int_t & PulseLength, Int_t ChanNo);
		Bool_t WriteTriggerPulseLength(SrvVMEModule * Module, Int_t & PulseLength, Int_t ChanNo = kSis3302AllChans);
		//! trigger internal gate & delay
		Bool_t ReadTriggerInternalGate(SrvVMEModule * Module, Int_t & GateLength, Int_t ChanNo);
		Bool_t WriteTriggerInternalGate(SrvVMEModule * Module, Int_t & GateLength, Int_t ChanNo = kSis3302AllChans);
		Bool_t ReadTriggerInternalDelay(SrvVMEModule * Module, Int_t & DelayLength, Int_t ChanNo);
		Bool_t WriteTriggerInternalDelay(SrvVMEModule * Module, Int_t & DelayLength, Int_t ChanNo = kSis3302AllChans);
		//! trigger decimation
		Bool_t GetTriggerDecimation(SrvVMEModule * Module, Int_t & Decimation, Int_t ChanNo);
		Bool_t SetTriggerDecimation(SrvVMEModule * Module, Int_t & Decimation, Int_t ChanNo = kSis3302AllChans);

		//! trigger threshold
		Bool_t ReadTriggerThreshold(SrvVMEModule * Module, Int_t & Thresh, Int_t ChanNo);
		Bool_t WriteTriggerThreshold(SrvVMEModule * Module, Int_t & Thresh, Int_t ChanNo = kSis3302AllChans);
		Bool_t GetTriggerGT(SrvVMEModule * Module, Bool_t & GTFlag, Int_t ChanNo);
		Bool_t SetTriggerGT(SrvVMEModule * Module, Bool_t & GTFlag, Int_t ChanNo = kSis3302AllChans);
		Bool_t GetTriggerOut(SrvVMEModule * Module, Bool_t & TrigOutFlag, Int_t ChanNo);
		Bool_t SetTriggerOut(SrvVMEModule * Module, Bool_t & TrigOutFlag, Int_t ChanNo = kSis3302AllChans);

		//! energy peak & gap
		Bool_t ReadEnergyPeakAndGap(SrvVMEModule * Module, Int_t & Peak, Int_t & Gap, Int_t ChanNo);
		Bool_t WriteEnergyPeakAndGap(SrvVMEModule * Module, Int_t & Peak, Int_t & Gap, Int_t ChanNo = kSis3302AllChans);
		//! energy decimation
		Bool_t GetEnergyDecimation(SrvVMEModule * Module, Int_t & Decimation, Int_t ChanNo);
		Bool_t SetEnergyDecimation(SrvVMEModule * Module, Int_t & Decimation, Int_t ChanNo = kSis3302AllChans);
		//! energy gate length
		Bool_t ReadEnergyGateLength(SrvVMEModule * Module, Int_t & PulseLength, Int_t ChanNo);
		Bool_t WriteEnergyGateLength(SrvVMEModule * Module, Int_t & PulseLength, Int_t ChanNo = kSis3302AllChans);
		//! energy sample length & start
		Bool_t ReadEnergySampleLength(SrvVMEModule * Module, Int_t & PulseLength, Int_t ChanNo);
		Bool_t WriteEnergySampleLength(SrvVMEModule * Module, Int_t & PulseLength, Int_t ChanNo = kSis3302AllChans);
		Bool_t ReadStartIndex(SrvVMEModule * Module, Int_t & IdxVal, Int_t IdxNo, Int_t ChanNo);
		Bool_t WriteStartIndex(SrvVMEModule * Module, Int_t & IdxVal, Int_t IdxNo, Int_t ChanNo = kSis3302AllChans);
		//! test bits
		Bool_t GetTestBits(SrvVMEModule * Module, Int_t & Bits, Int_t ChanNo);
		Bool_t SetTestBits(SrvVMEModule * Module, Int_t & Bits, Int_t ChanNo = kSis3302AllChans);


		//! energy tau factor
		Bool_t ReadTauFactor(SrvVMEModule * Module, Int_t & Tau, Int_t ChanNo);
		Bool_t WriteTauFactor(SrvVMEModule * Module, Int_t & Tau, Int_t ChanNo = kSis3302AllChans);

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

		//! trigger feedback
		Bool_t SetTriggerFeedback(SrvVMEModule * Module, Bool_t & Feedback);
		Bool_t GetTriggerFeedback(SrvVMEModule * Module, Bool_t & Feedback);
		
		//! start trace collection, get trace data
		Bool_t StartTraceCollection(SrvVMEModule * Module, Int_t & NofEvents, Int_t & ChanPatt);
		Bool_t ContinueTraceCollection(SrvVMEModule * Module);
		Bool_t PauseTraceCollection(SrvVMEModule * Module);
		Bool_t StopTraceCollection(SrvVMEModule * Module);
		Bool_t GetTraceData(SrvVMEModule * Module, TArrayI & Data, Int_t & EventNo, Int_t ChanNo);
		Bool_t GetTraceLength(SrvVMEModule * Module, TArrayI & Data, Int_t ChanNo);

		//! ramp dacs
		Bool_t RampDac(SrvVMEModule * Module, TArrayI & Data, Int_t ChanNo);

	protected:
		void SetupFunction(M2L_MsgData * Data, TArrayI & Array, Int_t & ChanNo);
		M2L_MsgHdr * FinishFunction(TArrayI & Array);
		
		ULong_t CA(ULong_t Address);

		Bool_t ReadPreTrigDelayAndGateLength(SrvVMEModule * Module, Int_t & Data, Int_t ChanNo);
		Bool_t WritePreTrigDelayAndGateLength(SrvVMEModule * Module, Int_t & Data, Int_t ChanNo = kSis3302AllChans);
		Bool_t ReadRawDataBufConfig(SrvVMEModule * Module, Int_t & Bits, Int_t ChanNo);
		Bool_t WriteRawDataBufConfig(SrvVMEModule * Module, Int_t & Bits, Int_t ChanNo = kSis3302AllChans);
		Bool_t ReadTriggerSetup(SrvVMEModule * Module, Int_t & Data, Int_t ChanNo);
		Bool_t WriteTriggerSetup(SrvVMEModule * Module, Int_t & Data, Int_t ChanNo);
		Bool_t ReadTriggerExtendedSetup(SrvVMEModule * Module, Int_t & Data, Int_t ChanNo);
		Bool_t WriteTriggerExtendedSetup(SrvVMEModule * Module, Int_t & Data, Int_t ChanNo);
		Bool_t ReadTriggerThreshReg(SrvVMEModule * Module, Int_t & Data, Int_t ChanNo);
		Bool_t WriteTriggerThreshReg(SrvVMEModule * Module, Int_t & Data, Int_t ChanNo);
		Bool_t ReadEnergySetup(SrvVMEModule * Module, Int_t & Data, Int_t ChanNo);
		Bool_t WriteEnergySetup(SrvVMEModule * Module, Int_t & Data, Int_t ChanNo);
		Bool_t ReadEnergyGateReg(SrvVMEModule * Module, Int_t & Data, Int_t ChanNo);
		Bool_t WriteEnergyGateReg(SrvVMEModule * Module, Int_t & Data, Int_t ChanNo);
		Bool_t ReadAcquisitionControl(SrvVMEModule * Module, Int_t & Data);
		Bool_t WriteAcquisitionControl(SrvVMEModule * Module, Int_t & Data);

		Bool_t DataReady(SrvVMEModule * Module);
		Bool_t ReadData(SrvVMEModule * Module, TArrayI & Data, Int_t NofWords, Int_t ChanNo);

		Bool_t ReadIRQConfiguration(SrvVMEModule * Module, Int_t & Vector, Int_t & Level, Bool_t & EnableFlag, Bool_t & RoakFlag);
		Bool_t WriteIRWConfiguration(SrvVMEModule * Module, Int_t Vector, Int_t Level = 0, Bool_t RoakMode = kFALSE);
		Bool_t EnableIRQ(SrvVMEModule * Module);
		Bool_t DisableIRQ(SrvVMEModule * Module);
		Bool_t EnableIRQSource(SrvVMEModule * Module, UInt_t IrqSource);
		Bool_t DisableIRQSource(SrvVMEModule * Module, UInt_t IrqSource);
		Bool_t ReadIRQSourceStatus(SrvVMEModule * Module, UInt_t & IrqStatus);
		Bool_t ReadIRQEnableStatus(SrvVMEModule * Module, UInt_t & IrqStatus);

		inline void SetStatus(UInt_t Bits) { fStatus |= Bits; };
		inline void ClearStatus(UInt_t Bits) { fStatus &= ~Bits; };
		inline UInt_t GetStatus() { return fStatus; };
		inline Bool_t IsStatus(UInt_t Bits) { return ((fStatus & Bits) != 0); };

		void SwitchSampling(SrvVMEModule * Module);
		Bool_t SetPageRegister(SrvVMEModule * Module, Int_t PageNumber);

	public:
		inline const Char_t * ClassName() const { return "SrvSis3302"; };

	protected:
		UInt_t fStatus;
		Int_t fTraceNo;
		Bool_t fTraceCollection;
		Bool_t fDumpTrace;
		Bool_t fReducedAddrSpace;
		Int_t fNofTry;
		UInt_t fSampling;

};

#endif

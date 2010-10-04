//________________________________________________________[C++ IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
//! \file			SrvSis3302.cxx
//! \brief			MARaBOU to Lynx: VME modules - SIS3302
//! \details		Implements class methods for SIS3302 adc
//!
//! $Author: Marabou $
//! $Mail			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.3 $
//! $Date: 2010-10-04 11:17:17 $
//////////////////////////////////////////////////////////////////////////////

#include "iostream.h"
#include "iomanip.h"

#include "SrvUtils.h"
#include "SrvSis3302.h"
#include "SrvSis3302_Layout.h"
#include "LwrLogger.h"
#include "SetColor.h"

extern TMrbLofNamedX * gLofVMEProtos;		//!< list of prototypes
extern TMrbLofNamedX * gLofVMEModules;		//!< list of actual modules

extern TMrbLogger * gMrbLog;				//!< message logger

extern Bool_t gSignalTrap;

TArrayI rawDataLength(kSis3302NofAdcs);
TArrayI energyDataLength(kSis3302NofAdcs);
TArrayI wordsPerEvent(kSis3302NofAdcs);
TArrayI nofEventsPerBuffer(kSis3302NofAdcs);

SrvSis3302::SrvSis3302() : SrvVMEModule(	"Sis3302",							//!< type
											"Digitizing ADC, 8ch 13(16)bit", 	//!< description
											0x09,								//!< address modifier: A32
											0x01000000,  						//!< segment size
											kSis3302NofAdcs,					//!< 8 channels/adcs
											  1 << 13) {						//!< range
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
//! \class			SrvSis3302	SrvSis3302.h
//! \brief			Create a SIS3302 prototype
//! \details		Prototype SIS3302
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger("", "c2lynx.log");
	this->SetID(SrvVMEModule::kModuleSis3302);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Access one of the registers to be sure the module is present
//! \param[in]		Module  -- module address
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::TryAccess(SrvVMEModule * Module) {

	Int_t boardId, majorVersion, minorVersion;
	if (!this->GetModuleInfo(Module, boardId, majorVersion, minorVersion, kTRUE)) return(kFALSE);
	if (boardId != 3302) {
		gMrbLog->Err()	<< "[" << Module->GetName()
						<< "]: Wrong module type - not a " << this->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "TryAccess");
		return(kFALSE);
	}

	fTraceCollection = kFALSE;
	fMultiEvent = kTRUE;
	fSampling = kSis3302KeyArmBank1Sampling;
	fTraceNo = 0;
	fDumpTrace = kFALSE;

	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Execute function
//! \param[in]		Module		-- module address
//! \param[in]		Function	-- function
//! \param[in,out]	Data		-- data
//! \retval 		Hdr 		-- ptr to output message
//////////////////////////////////////////////////////////////////////////////

M2L_MsgHdr * SrvSis3302::Dispatch(SrvVMEModule * Module, TMrbNamedX * Function, M2L_MsgData * Data) {

	Int_t adcNo;
	TArrayI data;
	Int_t d;

	this->SetupFunction(Data, data, adcNo);

	switch (Function->GetIndex()) {
		case kM2L_FCT_GET_MODULE_INFO:
			{
				Int_t BoardId;
				Int_t MajorVersion;
				Int_t MinorVersion;
				if (!this->GetModuleInfo(Module, BoardId, MajorVersion, MinorVersion, kFALSE)) return(NULL);
				M2L_VME_Return_Module_Info * m = (M2L_VME_Return_Module_Info *) allocMessage(sizeof(M2L_VME_Return_Module_Info), 1, kM2L_MESS_VME_EXEC_FUNCTION);
				m->fBoardId = swapIt(BoardId);
				m->fSerial = -1;
				m->fMajorVersion = swapIt(MajorVersion);
				m->fMinorVersion = swapIt(MinorVersion);
				return((M2L_MsgHdr *) m);
			}
		case kM2L_FCT_SIS_3302_SET_USER_LED:
			{
				Bool_t ledOn = (data[0] != 0);
				if (!this->SetUserLED(Module, ledOn)) return(NULL);
				data.Set(1); data[0] = ledOn ? 1 : 0;
				break;
			}
		case kM2L_FCT_SIS_3302_READ_DAC:
			{
				if (!this->ReadDac(Module, data, adcNo)) return(NULL);
				break;
			}
		case kM2L_FCT_SIS_3302_WRITE_DAC:
			{
				if (!this->WriteDac(Module, data, adcNo)) return(NULL);
				break;
			}
		case kM2L_FCT_SIS_3302_KEY_ADDR:
			{
				d = data[0];
				if (!this->KeyAddr(Module, d)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_READ_EVENT_CONFIG:
			{
				if (!this->ReadEventConfig(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_WRITE_EVENT_CONFIG:
			{
				d = data[0];
				if (!this->WriteEventConfig(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_READ_EVENT_EXTENDED_CONFIG:
			{
				if (!this->ReadEventExtendedConfig(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_WRITE_EVENT_EXTENDED_CONFIG:
			{
				d = data[0];
				if (!this->WriteEventExtendedConfig(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_GET_HEADER_BITS:
			{
				if (!this->GetHeaderBits(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_SET_HEADER_BITS:
			{
				d = data[0];
				if (!this->SetHeaderBits(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_GET_GROUP_ID:
			{
				if (!this->GetGroupId(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_GET_TRIGGER_MODE:
			{
				if (!this->GetTriggerMode(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_SET_TRIGGER_MODE:
			{
				d = data[0];
				if (!this->SetTriggerMode(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_GET_GATE_MODE:
			{
				if (!this->GetGateMode(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_SET_GATE_MODE:
			{
				d = data[0];
				if (!this->SetGateMode(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_GET_NNB_TRIGGER_MODE:
			{
				if (!this->GetNextNeighborTriggerMode(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_SET_NNB_TRIGGER_MODE:
			{
				d = data[0];
				if (!this->SetNextNeighborTriggerMode(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_GET_NNB_GATE_MODE:
			{
				if (!this->GetNextNeighborGateMode(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_SET_NNB_GATE_MODE:
			{
				d = data[0];
				if (!this->SetNextNeighborGateMode(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_GET_POLARITY:
			{
				Bool_t ifl;
				if (!this->GetPolarity(Module, ifl, adcNo)) return(NULL);
				data.Set(1); data[0] = ifl ? 1 : 0;
				break;
			}
		case kM2L_FCT_SIS_3302_SET_POLARITY:
			{
				Bool_t ifl = (data[0] != 0);
				if (!this->SetPolarity(Module, ifl, adcNo)) return(NULL);
				data.Set(1); data[0] = ifl ? 1 : 0;
				break;
			}
		case kM2L_FCT_SIS_3302_READ_END_ADDR_THRESH:
			{
				if (!this->ReadEndAddrThresh(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_WRITE_END_ADDR_THRESH:
			{
				d = data[0];
				if (!this->WriteEndAddrThresh(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_READ_PRETRIG_DELAY:
			{
				if (!this->ReadPreTrigDelay(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_WRITE_PRETRIG_DELAY:
			{
				d = data[0];
				if (!this->WritePreTrigDelay(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_READ_TRIGGER_GATE_LENGTH:
			{
				if (!this->ReadTrigGateLength(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_WRITE_TRIGGER_GATE_LENGTH:
			{
				d = data[0];
				if (!this->WriteTrigGateLength(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_READ_RAW_DATA_SAMPLE_LENGTH:
			{
				if (!this->ReadRawDataSampleLength(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_WRITE_RAW_DATA_SAMPLE_LENGTH:
			{
				d = data[0];
				Bool_t traceFlag = this->PauseTraceCollection(Module);
				if (!this->WriteRawDataSampleLength(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				if (traceFlag) this->ContinueTraceCollection(Module);
				break;
			}
		case kM2L_FCT_SIS_3302_READ_RAW_DATA_START_INDEX:
			{
				if (!this->ReadRawDataStartIndex(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_WRITE_RAW_DATA_START_INDEX:
			{
				d = data[0];
				if (!this->WriteRawDataStartIndex(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_READ_NEXT_SAMPLE_ADDR:
			{
				if (!this->ReadNextSampleAddr(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_READ_PREV_BANK_SAMPLE_ADDR:
			{
				if (!this->ReadPrevBankSampleAddr(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_READ_ACTUAL_SAMPLE:
			{
				if (!this->ReadActualSample(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_READ_TRIGGER_PEAK_AND_GAP:
			{
				Int_t p, g;
				if (!this->ReadTriggerPeakAndGap(Module, p, g, adcNo)) return(NULL);
				data.Set(2); data[0] = p; data[1] = g;
				break;
			}
		case kM2L_FCT_SIS_3302_WRITE_TRIGGER_PEAK_AND_GAP:
			{
				Bool_t traceFlag = this->PauseTraceCollection(Module);
				Int_t p, g;
				p = data[0]; g = data[1];
				if (!this->WriteTriggerPeakAndGap(Module, p, g, adcNo)) return(NULL);
				data.Set(2); data[0] = p; data[1] = g;
				if (traceFlag) this->ContinueTraceCollection(Module);
				break;
			}
		case kM2L_FCT_SIS_3302_READ_TRIGGER_PULSE_LENGTH:
			{
				if (!this->ReadTriggerPulseLength(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_WRITE_TRIGGER_PULSE_LENGTH:
			{
				d = data[0];
				if (!this->WriteTriggerPulseLength(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_READ_TRIGGER_INTERNAL_GATE:
			{
				if (!this->ReadTriggerInternalGate(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_WRITE_TRIGGER_INTERNAL_GATE:
			{
				d = data[0];
				if (!this->WriteTriggerInternalGate(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_READ_TRIGGER_INTERNAL_DELAY:
			{
				if (!this->ReadTriggerInternalDelay(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_WRITE_TRIGGER_INTERNAL_DELAY:
			{
				d = data[0];
				if (!this->WriteTriggerInternalDelay(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_GET_TRIGGER_DECIMATION:
			{
				if (!this->GetTriggerDecimation(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_SET_TRIGGER_DECIMATION:
			{
				d = data[0];
				if (!this->SetTriggerDecimation(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_READ_TRIGGER_THRESH:
			{
				if (!this->ReadTriggerThreshold(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_WRITE_TRIGGER_THRESH:
			{
				d = data[0];
				if (!this->WriteTriggerThreshold(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_GET_TRIGGER_GT:
			{
				Bool_t gt;
				if (!this->GetTriggerGT(Module, gt, adcNo)) return(NULL);
				data.Set(1); data[0] = gt ? 1 : 0;
				break;
			}
		case kM2L_FCT_SIS_3302_SET_TRIGGER_GT:
			{
				Bool_t gt = (data[0] != 0);
				if (!this->SetTriggerGT(Module, gt, adcNo)) return(NULL);
				data.Set(1); data[0] = gt ? 1 : 0;
				break;
			}
		case kM2L_FCT_SIS_3302_GET_TRIGGER_OUT:
			{
				Bool_t ofl;
				if (!this->GetTriggerOut(Module, ofl, adcNo)) return(NULL);
				data.Set(1); data[0] = ofl ? 1 : 0;
				break;
			}
		case kM2L_FCT_SIS_3302_SET_TRIGGER_OUT:
			{
				Bool_t ofl = (data[0] != 0);
				if (!this->SetTriggerOut(Module, ofl, adcNo)) return(NULL);
				data.Set(1); data[0] = ofl ? 1 : 0;
				break;
			}
		case kM2L_FCT_SIS_3302_READ_ENERGY_PEAK_AND_GAP:
			{
				Bool_t traceFlag = this->PauseTraceCollection(Module);
				Int_t p, g;
				if (!this->ReadEnergyPeakAndGap(Module, p, g, adcNo)) return(NULL);
				data.Set(2); data[0] = p; data[1] = g;
				if (traceFlag) this->ContinueTraceCollection(Module);
				break;
			}
		case kM2L_FCT_SIS_3302_WRITE_ENERGY_PEAK_AND_GAP:
			{
				Bool_t traceFlag = this->PauseTraceCollection(Module);
				Int_t p, g;
				p = data[0]; g = data[1];
				if (!this->WriteEnergyPeakAndGap(Module, p, g, adcNo)) return(NULL);
				data.Set(2); data[0] = p; data[1] = g;
				if (traceFlag) this->ContinueTraceCollection(Module);
				break;
			}
		case kM2L_FCT_SIS_3302_GET_ENERGY_DECIMATION:
			{
				if (!this->GetEnergyDecimation(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_SET_ENERGY_DECIMATION:
			{
				d = data[0];
				if (!this->SetEnergyDecimation(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_READ_ENERGY_GATE_LENGTH:
			{
				if (!this->ReadEnergyGateLength(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_WRITE_ENERGY_GATE_LENGTH:
			{
				d = data[0];
				if (!this->WriteEnergyGateLength(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_GET_TEST_BITS:
			{
				if (!this->GetTestBits(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_SET_TEST_BITS:
			{
				d = data[0];
				if (!this->SetTestBits(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_READ_ENERGY_SAMPLE_LENGTH:
			{
				if (!this->ReadEnergySampleLength(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_WRITE_ENERGY_SAMPLE_LENGTH:
			{
				Bool_t traceFlag = this->PauseTraceCollection(Module);
				d = data[0];
				if (!this->WriteEnergySampleLength(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				if (traceFlag) this->ContinueTraceCollection(Module);
				break;
			}
		case kM2L_FCT_SIS_3302_READ_START_INDEX:
			{
				Int_t x = data[0];
				if (!this->ReadStartIndex(Module, d, x, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_WRITE_START_INDEX:
			{
				Int_t x = data[0];
				d = data[1];
				if (!this->WriteStartIndex(Module, d, x, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_READ_TAU_FACTOR:
			{
				if (!this->ReadTauFactor(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_WRITE_TAU_FACTOR:
			{
				d = data[0];
				if (!this->WriteTauFactor(Module, d, adcNo)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_GET_CLOCK_SOURCE:
			{
				if (!this->GetClockSource(Module, d)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_SET_CLOCK_SOURCE:
			{
				d = data[0];
				if (!this->SetClockSource(Module, d)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_GET_LEMO_IN_MODE:
			{
				if (!this->GetLemoInMode(Module, d)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_SET_LEMO_IN_MODE:
			{
				d = data[0];
				if (!this->SetLemoInMode(Module, d)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_GET_LEMO_OUT_MODE:
			{
				if (!this->GetLemoOutMode(Module, d)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_SET_LEMO_OUT_MODE:
			{
				d = data[0];
				if (!this->SetLemoOutMode(Module, d)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_GET_LEMO_IN_ENABLE_MASK:
			{
				if (!this->GetLemoInEnableMask(Module, d)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_SET_LEMO_IN_ENABLE_MASK:
			{
				d = data[0];
				if (!this->SetLemoInEnableMask(Module, d)) return(NULL);
				data.Set(1); data[0] = d;
				break;
			}
		case kM2L_FCT_SIS_3302_START_TRACE_COLLECTION:
			{
				Int_t nofEvents = data[0];
				if (!this->StartTraceCollection(Module, nofEvents, adcNo)) return(NULL);
				break;
			}
		case kM2L_FCT_SIS_3302_CONT_TRACE_COLLECTION:
			{
				if (!this->ContinueTraceCollection(Module)) return(NULL);
				break;
			}
		case kM2L_FCT_SIS_3302_STOP_TRACE_COLLECTION:
			{
				if (!this->StopTraceCollection(Module)) return(NULL);
				break;
			}
		case kM2L_FCT_SIS_3302_GET_TRACE_DATA:
			{
				Int_t evtNo = data[0];
				if (!this->GetTraceData(Module, data, evtNo, adcNo)) return(NULL);
				break;
			}
		case kM2L_FCT_SIS_3302_GET_TRACE_LENGTH:
			{
				data.Set(kSis3302EventPreHeader);
				if (!this->GetTraceLength(Module, data, adcNo)) return(NULL);
				break;
			}
		case kM2L_FCT_SIS_3302_DUMP_TRACE:
			{
				fDumpTrace = kTRUE;
				break;
			}
		default:
			{
				gMrbLog->Err()	<< "[" << Module->GetName() << "]: Function not implemented - "
								<< Function->GetName() << " (" << setbase(16) << Function->GetIndex() << ")" << endl;
				gMrbLog->Flush(this->ClassName(), "Dispatch");
				return(NULL);
			}
	}


	return(this->FinishFunction(data));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Prepares a 'exec function' call
//! \param[in]		Data		-- incoming data
//! \param[in,out]	Array		-- array to hold data
//! \param[in]		AdcNo		-- adc/channel
//////////////////////////////////////////////////////////////////////////////

void SrvSis3302::SetupFunction(M2L_MsgData * Data, TArrayI & Array, Int_t & AdcNo) {

	AdcNo = swapIt(Data->fData[0]);
	Int_t wc = swapIt(Data->fWc) - 1;
	Int_t * dp = &Data->fData[1];
	Array.Reset();
	Array.Set(wc);
	for (Int_t i = 0; i < wc; i++) Array[i] = swapIt(*dp++);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Finishes 'exec function' call
//! \param[in,out]	Array        -- array holding data
//! \retval 		Msg       -- ptr to message (header)
//////////////////////////////////////////////////////////////////////////////

M2L_MsgHdr * SrvSis3302::FinishFunction(TArrayI & Array) {

	Int_t wc = Array.GetSize();
	M2L_VME_Return_Results * m = (M2L_VME_Return_Results *) allocMessage(sizeof(M2L_VME_Return_Results), wc, kM2L_MESS_VME_EXEC_FUNCTION);
	Int_t * dp = m->fData.fData;
	for (Int_t i = 0; i < wc; i++) *dp++ = swapIt(Array[i]);
	m->fData.fWc = swapIt(wc);
	return((M2L_MsgHdr *) m);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads (and outputs) module info
//! \param[in]		Module			-- module address
//! \param[out]		BoardId 		-- board id
//! \param[out]		MajorVersion	-- firmware version (major)
//! \param[out]		MinorVersion	-- firmware version (minor)
//! \param[in]		PrintFlag		-- output to gMrbLog if kTRUE
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::GetModuleInfo(SrvVMEModule * Module, Int_t & BoardId, Int_t & MajorVersion, Int_t & MinorVersion, Bool_t PrintFlag) {

	volatile ULong_t * firmWare = (volatile ULong_t *) Module->MapAddress(SIS3302_MODID);
	if (firmWare == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	ULong_t ident = *firmWare;
	if (this->CheckBusTrap(Module, SIS3302_MODID, "GetModuleInfo")) return(kFALSE);

	Int_t b = (ident >> 16) & 0xFFFF;
	BoardId = (b &0xF) + 10 * ((b >> 4) & 0xF) + 100 * ((b >> 8) & 0xF) + 1000 * ((b >> 12) & 0xF);
	MajorVersion = (ident >> 8) & 0xFF;
	MinorVersion = ident & 0xFF;
	if (PrintFlag) {
		gMrbLog->Out()	<< setbase(16)
						<< "SIS module info: addr (phys) 0x" << Module->GetBaseAddr()
						<< " (log) 0x" << Module->GetMappedAddr()
						<< " mod 0x" << Module->GetAddrModifier()
						<< setbase(10)
						<< " type " << BoardId
						<< " version " << setbase(16) << MajorVersion << setw(2) << setfill('0') << MinorVersion << endl;
		gMrbLog->Flush(this->ClassName(), "GetModuleInfo");
	}
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Turns user LED on/off
//! \param[in]		Module			-- module address
//! \param[in]		OnFlag			-- kTRUE if to be turned on
//! \param[in]		AdcNo 			-- adc number (ignored))
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::SetUserLED(SrvVMEModule * Module, Bool_t & OnFlag) {

	Int_t data;
	data = OnFlag ? 0x1 : 0x10000;
	if (!this->WriteControlStatus(Module, data)) return(kFALSE);

	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads dacs
//! \param[in]		Module		-- module address
//! \param[out]		DacValues	-- where to store dac values
//! \param[in]		AdcNo		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadDac(SrvVMEModule * Module, TArrayI & DacValues, Int_t AdcNo) {

	if (AdcNo != kSis3302AllAdcs && (AdcNo < 0 || AdcNo >= kSis3302NofAdcs)) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadDac");
		return(kFALSE);
	}

	Int_t firstAdc;
	Int_t lastAdc;
	if (AdcNo == kSis3302AllAdcs) {
		firstAdc = 0;
		lastAdc = kSis3302NofAdcs - 1;
	} else {
		firstAdc = AdcNo;
		lastAdc = AdcNo;
	}
	Int_t wc = lastAdc - firstAdc + 1;

	volatile Int_t * dacStatus = (volatile Int_t *) Module->MapAddress(SIS3302_DAC_CONTROL_STATUS);
	if (dacStatus == NULL) return(kFALSE);
	volatile Int_t * dacData = (volatile Int_t *) Module->MapAddress(SIS3302_DAC_DATA);
	if (dacData == NULL) return(kFALSE);

	Int_t maxTimeout = 5000;
	gSignalTrap = kFALSE;
	DacValues.Set(wc);
	Int_t idx = 0;
	for (Int_t adc = firstAdc; adc <= lastAdc; adc++, idx++) {
		*dacStatus = kSis3302DacCmdLoadShiftReg + (adc << 4);
		if (this->CheckBusTrap(Module, SIS3302_DAC_CONTROL_STATUS, "ReadDac")) return(kFALSE);
		Int_t timeout = 0 ;
		Int_t data;
		do {
			data = *dacStatus;
			if (this->CheckBusTrap(Module, SIS3302_DAC_CONTROL_STATUS, "ReadDac")) return(kFALSE);
			timeout++;
		} while (((data & kSis3302DacBusy) == kSis3302DacBusy) && (timeout <  maxTimeout));
		if (timeout >=  maxTimeout) return(kFALSE);

		DacValues[idx] = (*dacData >> 16) & 0xFFFF;
		if (this->CheckBusTrap(Module, SIS3302_DAC_DATA, "ReadDac")) return(kFALSE);
	}
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes dac values
//! \param[in]		Module		-- module address
//! \param[in]		DacValues	-- dac values to be written
//! \param[in]		AdcNo		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::WriteDac(SrvVMEModule * Module, TArrayI & DacValues, Int_t AdcNo) {

	if (AdcNo != kSis3302AllAdcs && (AdcNo < 0 || AdcNo >= kSis3302NofAdcs)) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteDac");
		return(kFALSE);
	}

	Int_t firstAdc;
	Int_t lastAdc;
	if (AdcNo == kSis3302AllAdcs) {
		firstAdc = 0;
		lastAdc = kSis3302NofAdcs - 1;
	} else {
		firstAdc = AdcNo;
		lastAdc = AdcNo;
	}
	Int_t wc = lastAdc - firstAdc + 1;
	if (wc > DacValues.GetSize()) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]:Wrong size of data array - "
						<< DacValues.GetSize() << " (should be at least " << wc << ")" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteDac");
		return(kFALSE);
	}

	volatile Int_t * dacStatus = (volatile Int_t *) Module->MapAddress(SIS3302_DAC_CONTROL_STATUS);
	if (dacStatus == NULL) return(kFALSE);
	volatile Int_t * dacData = (volatile Int_t *) Module->MapAddress(SIS3302_DAC_DATA);
	if (dacData == NULL) return(kFALSE);

	Int_t maxTimeout = 5000;
	Int_t idx = 0;
	gSignalTrap = kFALSE;
	for (Int_t adc = firstAdc; adc <= lastAdc; adc++, idx++) {
		*dacData = DacValues[idx];
		if (this->CheckBusTrap(Module, SIS3302_DAC_DATA, "WriteDac")) return(kFALSE);

		*dacStatus = kSis3302DacCmdLoadShiftReg + (adc << 4);
		if (this->CheckBusTrap(Module, SIS3302_DAC_CONTROL_STATUS, "WriteDac")) return(kFALSE);
		Int_t timeout = 0 ;
		Int_t data;
		do {
			data = *dacStatus;
			if (this->CheckBusTrap(Module, SIS3302_DAC_CONTROL_STATUS, "WriteDac")) return(kFALSE);
			timeout++;
		} while (((data & kSis3302DacBusy) == kSis3302DacBusy) && (timeout <  maxTimeout));
		if (timeout >=  maxTimeout) return(kFALSE);

		*dacStatus = kSis3302DacCmdLoad + (adc << 4);
		if (this->CheckBusTrap(Module, SIS3302_DAC_CONTROL_STATUS, "WriteDac")) return(kFALSE);
		timeout = 0 ;
		do {
			data = *dacStatus;
			if (this->CheckBusTrap(Module, SIS3302_DAC_CONTROL_STATUS, "WriteDac")) return(kFALSE);
			timeout++;
		} while (((data & kSis3302DacBusy) == kSis3302DacBusy) && (timeout <  maxTimeout));
		if (timeout >=  maxTimeout) return(kFALSE);
	}
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Executes KEY ADDR command
//! \param[in]		Module		-- module address
//! \param[in]		Key 		-- key
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::KeyAddr(SrvVMEModule * Module, Int_t Key) {

	Int_t offset = 0;
	switch (Key) {
		case kSis3302KeyReset:							offset = SIS3302_KEY_RESET; break;
		case kSis3302KeyResetSample:					offset = SIS3302_KEY_SAMPLE_LOGIC_RESET_404; break;
		case kSis3302KeyDisarmSample:					offset = SIS3302_KEY_DISARM; break;
		case kSis3302KeyTrigger:						offset = SIS3302_KEY_TRIGGER; break;
		case kSis3302KeyClearTimestamp: 				offset = SIS3302_KEY_TIMESTAMP_CLEAR; break;
		case kSis3302KeyArmBank1Sampling:				offset = SIS3302_KEY_DISARM_AND_ARM_BANK1; break;
		case kSis3302KeyArmBank2Sampling:				offset = SIS3302_KEY_DISARM_AND_ARM_BANK2; break;
		case kSis3302KeyResetDDR2Logic:					offset = SIS3302_KEY_RESET_DDR2_LOGIC; break;
		case kSis3302KeyMcaScanLNEPulse:				offset = SIS3302_KEY_MCA_SCAN_LNE_PULSE; break;
		case kSis3302KeyMcaScanArm:						offset = SIS3302_KEY_MCA_SCAN_ARM; break;
		case kSis3302KeyMcaScanStart:					offset = SIS3302_KEY_MCA_SCAN_START; break;
		case kSis3302KeyMcaScanDisable:					offset = SIS3302_KEY_MCA_SCAN_DISABLE; break;
		case kSis3302KeyMcaMultiscanStartResetPulse:	offset = SIS3302_KEY_MCA_MULTISCAN_START_RESET_PULSE; break;
		case kSis3302KeyMcaMultiscanArmScanArm:			offset = SIS3302_KEY_MCA_MULTISCAN_ARM_SCAN_ARM; break;
		case kSis3302KeyMcaMultiscanArmScanEnable:		offset = SIS3302_KEY_MCA_MULTISCAN_ARM_SCAN_ENABLE; break;
		case kSis3302KeyMcaMultiscanDisable:			offset = SIS3302_KEY_MCA_MULTISCAN_DISABLE; break;
		default:
			gMrbLog->Err()	<< "[" << Module->GetName() << "]: Illegal key value - " << Key << endl;
			gMrbLog->Flush(this->ClassName(), "KeyAddr");
			return(kFALSE);
	}

	volatile Int_t * keyAddr = (Int_t *) Module->MapAddress(offset);
	if (keyAddr == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*keyAddr = 0;
	if (Key == kSis3302KeyReset || Key == kSis3302KeyResetSample) sleep(1);
	if (this->CheckBusTrap(Module, offset, "KeyAddr")) return(kFALSE);
	return (kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads control and status register
//! \param[in]		Module			-- module address
//! \param[out]		Bits			-- data
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadControlStatus(SrvVMEModule * Module, Int_t & Bits) {

	Int_t offset = SIS3302_CONTROL_STATUS;

	volatile Int_t * ctrlStat = (volatile Int_t *) Module->MapAddress(offset);
	if (ctrlStat == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Bits = *ctrlStat;
	return (!this->CheckBusTrap(Module, offset, "ReadControlStatus"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes control and status register
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- data
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::WriteControlStatus(SrvVMEModule * Module, Int_t & Bits) {

	Int_t offset = SIS3302_CONTROL_STATUS;

	volatile Int_t * ctrlStat = (volatile Int_t *) Module->MapAddress(offset);
	if (ctrlStat == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*ctrlStat = Bits;
	if (this->CheckBusTrap(Module, offset, "WriteControlStatus")) return(kFALSE);
	return(this->ReadControlStatus(Module, Bits));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads event config register
//! \param[in]		Module			-- module address
//! \param[out]		Bits			-- configuration
//! \param[in]		AdcNo 			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadEventConfig(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadEventConfig");
		return(kFALSE);
	}

	Int_t offset;
	switch (AdcNo) {
		case 0:
		case 1: 	offset = SIS3302_EVENT_CONFIG_ADC12; break;
		case 2:
		case 3: 	offset = SIS3302_EVENT_CONFIG_ADC34; break;
		case 4:
		case 5: 	offset = SIS3302_EVENT_CONFIG_ADC56; break;
		case 6:
		case 7: 	offset = SIS3302_EVENT_CONFIG_ADC78; break;
	}

	volatile Int_t * evtConf = (volatile Int_t *) Module->MapAddress(offset);
	if (evtConf == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Bits = *evtConf;
	return (!this->CheckBusTrap(Module, offset, "ReadEventConfig"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes event config register
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- configuration
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::WriteEventConfig(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo) {

	if (AdcNo != kSis3302AllAdcs && (AdcNo < 0 || AdcNo >= kSis3302NofAdcs)) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteEventConfig");
		return(kFALSE);
	}

	Int_t offset;
	switch (AdcNo) {
		case kSis3302AllAdcs:
					offset = SIS3302_EVENT_CONFIG_ALL_ADC; break;
		case 0:
		case 1: 	offset = SIS3302_EVENT_CONFIG_ADC12; break;
		case 2:
		case 3: 	offset = SIS3302_EVENT_CONFIG_ADC34; break;
		case 4:
		case 5: 	offset = SIS3302_EVENT_CONFIG_ADC56; break;
		case 6:
		case 7: 	offset = SIS3302_EVENT_CONFIG_ADC78; break;
	}

	volatile Int_t * evtConf = (volatile Int_t *) Module->MapAddress(offset);
	if (evtConf == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*evtConf = Bits;
	if (this->CheckBusTrap(Module, offset, "WriteEventConfig")) return(kFALSE);
	return(this->ReadEventConfig(Module, Bits, AdcNo == kSis3302AllAdcs ? 1 : AdcNo));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads EXTENDED event config register
//! \param[in]		Module			-- module address
//! \param[out]		Bits			-- configuration
//! \param[in]		AdcNo 			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadEventExtendedConfig(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadEventExtendedConfig");
		return(kFALSE);
	}

	Int_t offset;
	switch (AdcNo) {
		case 0:
		case 1: 	offset = SIS3302_EVENT_EXTENDED_CONFIG_ADC12; break;
		case 2:
		case 3: 	offset = SIS3302_EVENT_EXTENDED_CONFIG_ADC34; break;
		case 4:
		case 5: 	offset = SIS3302_EVENT_EXTENDED_CONFIG_ADC56; break;
		case 6:
		case 7: 	offset = SIS3302_EVENT_EXTENDED_CONFIG_ADC78; break;
	}

	volatile Int_t * evtConf = (volatile Int_t *) Module->MapAddress(offset);
	if (evtConf == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Bits = *evtConf;
	return (!this->CheckBusTrap(Module, offset, "ReadEventExtendedConfig"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes EXTENDED event config register
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- configuration
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::WriteEventExtendedConfig(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo) {

	if (AdcNo != kSis3302AllAdcs && (AdcNo < 0 || AdcNo >= kSis3302NofAdcs)) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteEventExtendedConfig");
		return(kFALSE);
	}

	Int_t offset;
	switch (AdcNo) {
		case kSis3302AllAdcs:
					offset = SIS3302_EVENT_EXTENDED_CONFIG_ALL; break;
		case 0:
		case 1: 	offset = SIS3302_EVENT_EXTENDED_CONFIG_ADC12; break;
		case 2:
		case 3: 	offset = SIS3302_EVENT_EXTENDED_CONFIG_ADC34; break;
		case 4:
		case 5: 	offset = SIS3302_EVENT_EXTENDED_CONFIG_ADC56; break;
		case 6:
		case 7: 	offset = SIS3302_EVENT_EXTENDED_CONFIG_ADC78; break;
	}

	volatile Int_t * evtConf = (volatile Int_t *) Module->MapAddress(offset);
	if (evtConf == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*evtConf = Bits;
	if (this->CheckBusTrap(Module, offset, "WriteEventExtendedConfig")) return(kFALSE);
	return(this->ReadEventConfig(Module, Bits, AdcNo == kSis3302AllAdcs ? 1 : AdcNo));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns header data
//! \param[in]		Module		-- module address
//! \param[out]		Bits		-- bits
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::GetHeaderBits(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "GetHeaderBits");
		return(kFALSE);
	}

	Int_t bits;
	if (!this->ReadEventConfig(Module, bits, AdcNo)) return(kFALSE);
	bits >>= 17;		// header bit #1 -> event config bit #17
	Bits = bits & kSis3302HeaderMask;
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Defines header data
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- bits
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::SetHeaderBits(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo) {

	if (Bits < 0 || Bits > kSis3302HeaderMask) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Illegal header data - " << setbase(16) << Bits
						<< " (should be in [0," << kSis3302HeaderMask << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "SetHeaderBits");
		return(kFALSE);
	}

	if (Bits & 0x3) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Illegal header data - " << setbase(16) << Bits
						<< " (group id bits 0 & 1 are READ ONLY)" << endl;
		gMrbLog->Flush(this->ClassName(), "SetHeaderBits");
		return(kFALSE);
	}

	Bits >>= 2;			// bits 1 & 2 are read-only

	if (AdcNo == kSis3302AllAdcs) {
		Int_t b;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			b = Bits;
			if (!this->SetHeaderBits(Module, b, adc)) return(kFALSE);
		}
		Bits = b;
		return(kTRUE);
	}

	Int_t bits;
	if (!this->ReadEventConfig(Module, bits, AdcNo)) return(kFALSE);
	bits &= 0xFFFF;
	bits |=	(Bits << 19);		// header bit #1 -> event config bit #17, bits 1 & 2 read-only
	if (!this->WriteEventConfig(Module, bits, AdcNo)) return(kFALSE);
	return(this->ReadEventConfig(Module, Bits, AdcNo == kSis3302AllAdcs ? 0 : AdcNo));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns group id (read-only)
//! \param[in]		Module		-- module address
//! \param[out]		GroupId 	-- group id
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::GetGroupId(SrvVMEModule * Module, Int_t & GroupId, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "GetGroupId");
		return(kFALSE);
	}

	Int_t bits;
	if (!this->ReadEventConfig(Module, bits, AdcNo)) return(kFALSE);
	bits >>= 17;
	GroupId = bits & 0x3;
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns trigger mode
//! \param[in]		Module		-- module address
//! \param[out]		Bits		-- bits (0,1,2,3)
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::GetTriggerMode(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "GetTriggerMode");
		return(kFALSE);
	}

	Int_t bits;
	if (!this->ReadEventConfig(Module, bits, AdcNo)) return(kFALSE);
	if (AdcNo & 1) {
		bits >>= 10;
	} else {
		bits >>= 2;
	}
	Bits = bits & kSis3302TriggerBoth;
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Defines trigger mode
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- bits (0,1,2,3)
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::SetTriggerMode(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo) {

	if (Bits < kSis3302TriggerOff || Bits > kSis3302TriggerBoth) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Illegal trigger mode - " << setbase(16) << Bits
						<< " (should be in [" << kSis3302TriggerOff << "," << kSis3302TriggerBoth << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "SetTriggerMode");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t b;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			b = Bits;
			if (!this->SetTriggerMode(Module, b, adc)) return(kFALSE);
		}
		Bits = b;
		return(kTRUE);
	}

	Int_t bits;
	if (!this->ReadEventConfig(Module, bits, AdcNo)) return(kFALSE);
	if (AdcNo & 1) {
		bits &= ~(kSis3302TriggerBoth << 10);
		bits |=	(Bits << 10);
	} else {
		bits &= ~(kSis3302TriggerBoth << 2);
		bits |=	(Bits << 2);
	}
	if (!this->WriteEventConfig(Module, bits, AdcNo)) return(kFALSE);
	return(this->ReadEventConfig(Module, Bits, AdcNo == kSis3302AllAdcs ? 0 : AdcNo));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns gate mode
//! \param[in]		Module		-- module address
//! \param[out]		Bits		-- bits (0,1,2,3)
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::GetGateMode(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "GetGateMode");
		return(kFALSE);
	}

	Int_t bits;
	if (!this->ReadEventConfig(Module, bits, AdcNo)) return(kFALSE);
	if (AdcNo & 1) {
		bits >>= 12;
	} else {
		bits >>= 4;
	}
	Bits = bits & kSis3302GateBoth;
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Defines gate mode
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- bits (0,1,2,3)
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::SetGateMode(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo) {

	if (Bits < kSis3302GateOff || Bits > kSis3302GateBoth) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Illegal gate mode - " << setbase(16) << Bits
						<< " (should be in [" << kSis3302GateOff << "," << kSis3302GateBoth << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "SetGateMode");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t b;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			b = Bits;
			if (!this->SetGateMode(Module, b, adc)) return(kFALSE);
		}
		Bits = b;
		return(kTRUE);
	}

	Int_t bits;
	if (!this->ReadEventConfig(Module, bits, AdcNo)) return(kFALSE);
	if (AdcNo & 1) {
		bits &= ~(kSis3302GateBoth << 12);
		bits |=	(Bits << 12);
	} else {
		bits &= ~(kSis3302GateBoth << 4);
		bits |=	(Bits << 4);
	}
	if (!this->WriteEventConfig(Module, bits, AdcNo)) return(kFALSE);
	return(this->ReadEventConfig(Module, Bits, AdcNo == kSis3302AllAdcs ? 0 : AdcNo));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns next-neighbor trigger mode
//! \param[in]		Module		-- module address
//! \param[out]		Bits		-- bits (0,1,2,3)
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::GetNextNeighborTriggerMode(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "GetNextNeighborTriggerMode");
		return(kFALSE);
	}

	Int_t bits;
	if (!this->ReadEventExtendedConfig(Module, bits, AdcNo)) return(kFALSE);
	if (AdcNo & 1) {
		bits >>= 14;
	} else {
		bits >>= 6;
	}
	Bits = bits & kSis3302TriggerBoth;
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Defines next-neighbor trigger mode
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- bits (0,1,2,3)
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::SetNextNeighborTriggerMode(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo) {

	if (Bits < kSis3302TriggerOff || Bits > kSis3302TriggerBoth) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Illegal next-neighbor trigger mode - " << setbase(16) << Bits
						<< " (should be in [" << kSis3302TriggerOff << "," << kSis3302TriggerBoth << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "SetNextNeighborTriggerMode");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t b;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			b = Bits;
			if (!this->SetNextNeighborTriggerMode(Module, b, adc)) return(kFALSE);
		}
		Bits = b;
		return(kTRUE);
	}

	Int_t bits;
	if (!this->ReadEventExtendedConfig(Module, bits, AdcNo)) return(kFALSE);
	if (AdcNo & 1) {
		bits &= ~(kSis3302TriggerBoth << 10);
		bits |=	(Bits << 14);
	} else {
		bits &= ~(kSis3302TriggerBoth << 2);
		bits |=	(Bits << 6);
	}
	if (!this->WriteEventExtendedConfig(Module, bits, AdcNo)) return(kFALSE);
	return(this->ReadEventExtendedConfig(Module, Bits, AdcNo == kSis3302AllAdcs ? 0 : AdcNo));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns next-neighbor gate mode
//! \param[in]		Module		-- module address
//! \param[out]		Bits		-- bits (0,1,2,3)
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::GetNextNeighborGateMode(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "GetNextNeighborGateMode");
		return(kFALSE);
	}

	Int_t bits;
	if (!this->ReadEventConfig(Module, bits, AdcNo)) return(kFALSE);
	if (AdcNo & 1) {
		bits >>= 14;
	} else {
		bits >>= 6;
	}
	Bits = bits & kSis3302GateBoth;
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Defines next-neighbor gate mode
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- bits (0,1,2,3)
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::SetNextNeighborGateMode(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo) {

	if (Bits < kSis3302GateOff || Bits > kSis3302GateBoth) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Illegal next-neighbor gate mode - " << setbase(16) << Bits
						<< " (should be in [" << kSis3302GateOff << "," << kSis3302GateBoth << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "SetNextNeighborGateMode");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t b;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			b = Bits;
			if (!this->SetNextNeighborGateMode(Module, b, adc)) return(kFALSE);
		}
		Bits = b;
		return(kTRUE);
	}

	Int_t bits;
	if (!this->ReadEventConfig(Module, bits, AdcNo)) return(kFALSE);
	if (AdcNo & 1) {
		bits &= ~(kSis3302GateBoth << 14);
		bits |=	(Bits << 14);
	} else {
		bits &= ~(kSis3302GateBoth << 6);
		bits |=	(Bits << 6);
	}
	if (!this->WriteEventConfig(Module, bits, AdcNo)) return(kFALSE);
	return(this->ReadEventConfig(Module, Bits, AdcNo == kSis3302AllAdcs ? 0 : AdcNo));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns trigger polarity
//! \param[in]		Module		-- module address
//! \param[out]		InvertFlag	-- kTRUE if trigger is inverted
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::GetPolarity(SrvVMEModule * Module, Bool_t & InvertFlag, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "GetPolarity");
		return(kFALSE);
	}

	Int_t bits;
	if (!this->ReadEventConfig(Module, bits, AdcNo)) return(kFALSE);
	if (AdcNo & 1) bits >>= 8;
	InvertFlag = ((bits & kSis3302PolarityNegative) > 0);
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Defines trigger polarity
//! \param[in]		Module			-- module address
//! \param[in]		InvertFlag		-- kTRUE if neg polarity
//! \param[in]		AdcNo 			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::SetPolarity(SrvVMEModule * Module, Bool_t & InvertFlag, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs) {
		Bool_t ifl;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			ifl = InvertFlag;
			if (!this->SetPolarity(Module, ifl, adc)) return(kFALSE);
		}
		InvertFlag = ifl;
		return(kTRUE);
	}

	Int_t data;
	if (!this->ReadEventConfig(Module, data, AdcNo)) return(kFALSE);
	if (AdcNo & 1) {
		if (InvertFlag) data |= (kSis3302PolarityNegative << 8); else data &= ~(kSis3302PolarityNegative << 8);
	} else {
		data &= ~1;
		if (InvertFlag) data |= kSis3302PolarityNegative; else data &= ~kSis3302PolarityNegative;
	}
	if (!this->WriteEventConfig(Module, data, AdcNo)) return(kFALSE);

	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads end addr threshold register
//! \param[in]		Module		-- module address
//! \param[out]		Thresh		-- threshold
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadEndAddrThresh(SrvVMEModule * Module, Int_t & Thresh, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadEndAddrThresh");
		return(kFALSE);
	}

	Int_t offset;
	switch (AdcNo) {
		case 0:
		case 1: 	offset = SIS3302_END_ADDRESS_THRESHOLD_ADC12; break;
		case 2:
		case 3: 	offset = SIS3302_END_ADDRESS_THRESHOLD_ADC34; break;
		case 4:
		case 5: 	offset = SIS3302_END_ADDRESS_THRESHOLD_ADC56; break;
		case 6:
		case 7: 	offset = SIS3302_END_ADDRESS_THRESHOLD_ADC78; break;
	}

	volatile Int_t * endAddr = (volatile Int_t *) Module->MapAddress(offset);
	if (endAddr == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Thresh = *endAddr;
	return (!this->CheckBusTrap(Module, offset, "ReadEndAddrThresh"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes end addr threshold register
//! \param[in]		Module		-- module address
//! \param[in]		Thresh		-- threshold
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::WriteEndAddrThresh(SrvVMEModule * Module, Int_t & Thresh, Int_t AdcNo) {

	if (AdcNo != kSis3302AllAdcs && (AdcNo < 0 || AdcNo >= kSis3302NofAdcs)) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteEndAddrThresh");
		return(kFALSE);
	}

	if (Thresh > kSis3302EndAddrThreshMax) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Threshold out of range - max "
						<< setbase(16) << kSis3302EndAddrThreshMax << setbase(10) << endl;
		gMrbLog->Flush(this->ClassName(), "WriteEndAddrThresh");
		return(kFALSE);
	}

	Int_t offset;
	switch (AdcNo) {
		case kSis3302AllAdcs:	offset = SIS3302_END_ADDRESS_THRESHOLD_ALL_ADC; break;
		case 0:
		case 1: 	offset = SIS3302_END_ADDRESS_THRESHOLD_ADC12; break;
		case 2:
		case 3: 	offset = SIS3302_END_ADDRESS_THRESHOLD_ADC34; break;
		case 4:
		case 5: 	offset = SIS3302_END_ADDRESS_THRESHOLD_ADC56; break;
		case 6:
		case 7: 	offset = SIS3302_END_ADDRESS_THRESHOLD_ADC78; break;
	}

	volatile Int_t * endAddr = (volatile Int_t *) Module->MapAddress(offset);
	if (endAddr == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*endAddr = Thresh;
	if (this->CheckBusTrap(Module, offset, "WriteEndAddrThresh")) return(kFALSE);
	return(this->ReadEndAddrThresh(Module, Thresh, AdcNo == kSis3302AllAdcs ? 0 : AdcNo));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger register
//! \param[in]		Module		-- module address
//! \param[out]		Bits		-- bits
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadPreTrigDelayAndGateLength(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadPreTrigDelayAndGateLength");
		return(kFALSE);
	}

	Int_t offset;
	switch (AdcNo) {
		case 0:
		case 1: 	offset = SIS3302_PRETRIGGER_DELAY_TRIGGERGATE_LENGTH_ADC12; break;
		case 2:
		case 3: 	offset = SIS3302_PRETRIGGER_DELAY_TRIGGERGATE_LENGTH_ADC34; break;
		case 4:
		case 5: 	offset = SIS3302_PRETRIGGER_DELAY_TRIGGERGATE_LENGTH_ADC56; break;
		case 6:
		case 7: 	offset = SIS3302_PRETRIGGER_DELAY_TRIGGERGATE_LENGTH_ADC78; break;
	}

	volatile Int_t * trigReg = (volatile Int_t *) Module->MapAddress(offset);
	if (trigReg == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Bits = *trigReg;
	return (!this->CheckBusTrap(Module, offset, "ReadPreTrigDelayAndGateLength"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes trigger register
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- bits
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::WritePreTrigDelayAndGateLength(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo) {

	if (AdcNo != kSis3302AllAdcs && (AdcNo < 0 || AdcNo >= kSis3302NofAdcs)) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "WritePreTrigDelayAndGateLength");
		return(kFALSE);
	}

	Int_t offset;
	switch (AdcNo) {
		case kSis3302AllAdcs:	offset = SIS3302_PRETRIGGER_DELAY_TRIGGERGATE_LENGTH_ALL_ADC; break;
		case 0:
		case 1: 	offset = SIS3302_PRETRIGGER_DELAY_TRIGGERGATE_LENGTH_ADC12; break;
		case 2:
		case 3: 	offset = SIS3302_PRETRIGGER_DELAY_TRIGGERGATE_LENGTH_ADC34; break;
		case 4:
		case 5: 	offset = SIS3302_PRETRIGGER_DELAY_TRIGGERGATE_LENGTH_ADC56; break;
		case 6:
		case 7: 	offset = SIS3302_PRETRIGGER_DELAY_TRIGGERGATE_LENGTH_ADC78; break;
	}

	volatile Int_t * trigReg = (volatile Int_t *) Module->MapAddress(offset);
	if (trigReg == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*trigReg = Bits;
	if (this->CheckBusTrap(Module, offset, "WritePreTrigDelayAndGateLength")) return(kFALSE);
	return(this->ReadPreTrigDelayAndGateLength(Module, Bits, AdcNo == kSis3302AllAdcs ? 1 : AdcNo));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads pretrigger delay
//! \param[in]		Module		-- module address
//! \param[out]		Delay		-- delay
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadPreTrigDelay(SrvVMEModule * Module, Int_t & Delay, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadPreTrigDelay");
		return(kFALSE);
	}

	if (!this->ReadPreTrigDelayAndGateLength(Module, Delay, AdcNo)) return(kFALSE);
	Delay >>= 16;
	Delay -= 2;
	if (Delay < 0) Delay += 1024;
	Delay &= 0xFFF;
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes pretrigger delay
//! \param[in]		Module		-- module address
//! \param[in]		Delay		-- delay
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::WritePreTrigDelay(SrvVMEModule * Module, Int_t & Delay, Int_t AdcNo) {

	if (Delay < kSis3302PreTrigDelayMin || Delay > kSis3302PreTrigDelayMax) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Pretrigger delay out of range - "
						<< AdcNo << " (should be in [" << kSis3302PreTrigDelayMin << ","
						<< kSis3302PreTrigDelayMax << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "WritePreTrigDelay");
		return(kFALSE);
	}

	Int_t delay;
	if (AdcNo == kSis3302AllAdcs) {
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			delay = Delay;
			if (!this->WritePreTrigDelay(Module, delay, adc)) return(kFALSE);
		}
		Delay = delay;
		return(kTRUE);
	}

	if (!this->ReadPreTrigDelayAndGateLength(Module, delay, AdcNo)) return(kFALSE);

	Delay += 2;
	if (Delay >= 1024) Delay -= 1024;

	delay &= 0xFFFF;
	delay |= Delay << 16;
	Delay = delay;
	if (!this->WritePreTrigDelayAndGateLength(Module, Delay, AdcNo)) return(kFALSE);
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger gate length
//! \param[in]		Module		-- module address
//! \param[out]		Gate		-- gate
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadTrigGateLength(SrvVMEModule * Module, Int_t & Gate, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadTrigGateLength");
		return(kFALSE);
	}

	if (!this->ReadPreTrigDelayAndGateLength(Module, Gate, AdcNo)) return(kFALSE);
	Gate &= 0xFFFF;
	Gate += 1;
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes trigger gate length
//! \param[in]		Module		-- module address
//! \param[in]		Gate		-- gate
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::WriteTrigGateLength(SrvVMEModule * Module, Int_t & Gate, Int_t AdcNo) {

	if (Gate < kSis3302TrigGateLengthMin || Gate > kSis3302TrigGateLengthMax) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Trigger gate length out of range - "
						<< Gate << " (should be in [" << kSis3302TrigGateLengthMin << ","
						<< kSis3302TrigGateLengthMax << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteTrigGateLength");
		return(kFALSE);
	}

	Int_t gate;
	if (AdcNo == kSis3302AllAdcs) {
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			gate = Gate;
			if (!this->WriteTrigGateLength(Module, gate, adc)) return(kFALSE);
		}
		Gate = gate;
		return(kTRUE);
	}

	if (!this->ReadPreTrigDelayAndGateLength(Module, gate, AdcNo)) return(kFALSE);
	gate &= 0xFFFF0000;
	gate |= Gate - 1;
	Gate = gate;
	return(this->WritePreTrigDelayAndGateLength(Module, Gate, AdcNo));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads raw data buffer register
//! \param[in]		Module		-- module address
//! \param[out]		Bits		-- bits
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadRawDataBufConfig(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadRawDataBufConfig");
		return(kFALSE);
	}

	Int_t offset;
	switch (AdcNo) {
		case 0:
		case 1: 	offset = SIS3302_RAW_DATA_BUFFER_CONFIG_ADC12; break;
		case 2:
		case 3: 	offset = SIS3302_RAW_DATA_BUFFER_CONFIG_ADC34; break;
		case 4:
		case 5: 	offset = SIS3302_RAW_DATA_BUFFER_CONFIG_ADC56; break;
		case 6:
		case 7: 	offset = SIS3302_RAW_DATA_BUFFER_CONFIG_ADC78; break;
	}

	volatile Int_t * rawData = (volatile Int_t *) Module->MapAddress(offset);
	if (rawData == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Bits = *rawData;
	return (!this->CheckBusTrap(Module, offset, "ReadRawDataBufConfig"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes raw data buffer register
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- bits
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::WriteRawDataBufConfig(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo) {

	if (AdcNo != kSis3302AllAdcs && (AdcNo < 0 || AdcNo >= kSis3302NofAdcs)) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteRawDataBufConfig");
		return(kFALSE);
	}

	Int_t offset;
	switch (AdcNo) {
		case kSis3302AllAdcs:	offset = SIS3302_RAW_DATA_BUFFER_CONFIG_ALL_ADC; break;
		case 0:
		case 1: 	offset = SIS3302_RAW_DATA_BUFFER_CONFIG_ADC12; break;
		case 2:
		case 3: 	offset = SIS3302_RAW_DATA_BUFFER_CONFIG_ADC34; break;
		case 4:
		case 5: 	offset = SIS3302_RAW_DATA_BUFFER_CONFIG_ADC56; break;
		case 6:
		case 7: 	offset = SIS3302_RAW_DATA_BUFFER_CONFIG_ADC78; break;
	}

	volatile Int_t * rawData = (volatile Int_t *) Module->MapAddress(offset);
	if (rawData == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*rawData = Bits;
	if (this->CheckBusTrap(Module, offset, "WriteRawDataBufConfig")) return(kFALSE);
	return(this->ReadRawDataBufConfig(Module, Bits, AdcNo == kSis3302AllAdcs ? 1 : AdcNo));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads raw data sample length
//! \param[in]		Module			-- module address
//! \param[out]		SampleLength	-- length
//! \param[in]		AdcNo 			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadRawDataSampleLength(SrvVMEModule * Module, Int_t & SampleLength, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadRawDataSampleLength");
		return(kFALSE);
	}

	Int_t bits;
	if (!this->ReadRawDataBufConfig(Module, bits, AdcNo)) return(kFALSE);
	SampleLength = (bits >> 16) & 0xFFFF;
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes raw data sample length
//! \param[in]		Module			-- module address
//! \param[in]		SampleLength	-- length
//! \param[in]		AdcNo 			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::WriteRawDataSampleLength(SrvVMEModule * Module, Int_t & SampleLength, Int_t AdcNo) {

	if (this->IsStatus(kSis3302StatusCollectingTraces)) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Can't write raw data sample length - trace collection in progress ..." << endl;
		gMrbLog->Flush(this->ClassName(), "WriteRawDataSampleLength");
		return(kFALSE);
	}

	if (SampleLength < kSis3302RawDataSampleLengthMin || SampleLength > kSis3302RawDataSampleLengthMax) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Sample length out of range - "
						<< SampleLength << " (should be in [0," << kSis3302RawDataSampleLengthMax << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteRawDataSampleLength");
		return(kFALSE);
	}

	if (SampleLength % 4) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Wrong sample alignment - "
						<< SampleLength << " (should be mod 4)" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteRawDataSampleLength");
		return(kFALSE);
	}

	Int_t sl;
	if (AdcNo == kSis3302AllAdcs) {
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			sl = SampleLength;
			if (!this->WriteRawDataSampleLength(Module, sl, adc)) return(kFALSE);
		}
		SampleLength = sl;
		return(kTRUE);
	}

	if (!this->ReadRawDataBufConfig(Module, sl, AdcNo)) return(kFALSE);
	sl &= 0x0000FFFF;
	sl |= (SampleLength << 16);
	SampleLength = sl;
	return(this->WriteRawDataBufConfig(Module, SampleLength, AdcNo));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads raw data start index
//! \param[in]		Module			-- module address
//! \param[out]		StartIndex		-- start
//! \param[in]		AdcNo 			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadRawDataStartIndex(SrvVMEModule * Module, Int_t & StartIndex, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadRawDataStartIndex");
		return(kFALSE);
	}

	Int_t bits;
	if (!this->ReadRawDataBufConfig(Module, bits, AdcNo)) return(kFALSE);
	StartIndex = bits & 0xFFFF;
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes raw data start index
//! \param[in]		Module			-- module address
//! \param[in]		StartIndex		-- start
//! \param[in]		AdcNo 			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::WriteRawDataStartIndex(SrvVMEModule * Module, Int_t & StartIndex, Int_t AdcNo) {

	if (StartIndex < kSis3302RawDataStartIndexMin || StartIndex > kSis3302RawDataStartIndexMax) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Start index out of range - "
						<< StartIndex << " (should be in [0," << kSis3302RawDataStartIndexMax << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteRawDataStartIndex");
		return(kFALSE);
	}

	if (StartIndex & 0x1) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Wrong index alignment - "
						<< StartIndex << " (should be even)" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteRawDataStartIndex");
		return(kFALSE);
	}

	Int_t sx;
	if (AdcNo == kSis3302AllAdcs) {
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			sx = StartIndex;
			if (!this->WriteRawDataStartIndex(Module, sx, adc)) return(kFALSE);
		}
		StartIndex = sx;
		return(kTRUE);
	}

	if (!this->ReadRawDataBufConfig(Module, sx, AdcNo)) return(kFALSE);
	sx &= 0xFFFF0000;
	sx |= StartIndex;
	StartIndex = sx;
	return(this->WriteRawDataBufConfig(Module, StartIndex, AdcNo));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads next sample addr register
//! \param[in]		Module		-- module address
//! \param[out]		Addr		-- address
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadNextSampleAddr(SrvVMEModule * Module, Int_t & Addr, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadNextSampleAddr");
		return(kFALSE);
	}

	Int_t offset;
	switch (AdcNo) {
		case 0: 	offset = SIS3302_ACTUAL_SAMPLE_ADDRESS_ADC1; break;
		case 1: 	offset = SIS3302_ACTUAL_SAMPLE_ADDRESS_ADC2; break;
		case 2: 	offset = SIS3302_ACTUAL_SAMPLE_ADDRESS_ADC3; break;
		case 3: 	offset = SIS3302_ACTUAL_SAMPLE_ADDRESS_ADC4; break;
		case 4: 	offset = SIS3302_ACTUAL_SAMPLE_ADDRESS_ADC5; break;
		case 5: 	offset = SIS3302_ACTUAL_SAMPLE_ADDRESS_ADC6; break;
		case 6: 	offset = SIS3302_ACTUAL_SAMPLE_ADDRESS_ADC7; break;
		case 7: 	offset = SIS3302_ACTUAL_SAMPLE_ADDRESS_ADC8; break;
	}

	volatile Int_t * samplAddr = (volatile Int_t *) Module->MapAddress(offset);
	if (samplAddr == NULL) return(kFALSE);
	gSignalTrap = kFALSE;
	Addr = *samplAddr;
	return (!this->CheckBusTrap(Module, offset, "ReadNextSampleAddr"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads previous sample addr register
//! \param[in]		Module		-- module address
//! \param[in]		Addr		-- address
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadPrevBankSampleAddr(SrvVMEModule * Module, Int_t & Addr, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadPrevBankSampleAddr");
		return(kFALSE);
	}

	Int_t offset;
	switch (AdcNo) {
		case 0: 	offset = SIS3302_PREVIOUS_BANK_SAMPLE_ADDRESS_ADC1; break;
		case 1: 	offset = SIS3302_PREVIOUS_BANK_SAMPLE_ADDRESS_ADC2; break;
		case 2: 	offset = SIS3302_PREVIOUS_BANK_SAMPLE_ADDRESS_ADC3; break;
		case 3: 	offset = SIS3302_PREVIOUS_BANK_SAMPLE_ADDRESS_ADC4; break;
		case 4: 	offset = SIS3302_PREVIOUS_BANK_SAMPLE_ADDRESS_ADC5; break;
		case 5: 	offset = SIS3302_PREVIOUS_BANK_SAMPLE_ADDRESS_ADC6; break;
		case 6: 	offset = SIS3302_PREVIOUS_BANK_SAMPLE_ADDRESS_ADC7; break;
		case 7: 	offset = SIS3302_PREVIOUS_BANK_SAMPLE_ADDRESS_ADC8; break;
	}

	volatile Int_t * samplAddr = (volatile Int_t *) Module->MapAddress(offset);
	if (samplAddr == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Addr = *samplAddr;
	return (!this->CheckBusTrap(Module, offset, "ReadPrevBankSampleAddr"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads actual sample register
//! \param[in]		Module		-- module address
//! \param[out]		Data		-- sample data
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadActualSample(SrvVMEModule * Module, Int_t & Data, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadActualSample");
		return(kFALSE);
	}

	Int_t offset;
	switch (AdcNo) {
		case 0:
		case 1: 	offset = SIS3302_ACTUAL_SAMPLE_VALUE_ADC12; break;
		case 2:
		case 3: 	offset = SIS3302_ACTUAL_SAMPLE_VALUE_ADC34; break;
		case 4:
		case 5: 	offset = SIS3302_ACTUAL_SAMPLE_VALUE_ADC56; break;
		case 6:
		case 7: 	offset = SIS3302_ACTUAL_SAMPLE_VALUE_ADC78; break;
	}

	volatile Int_t * actSample = (volatile Int_t *) Module->MapAddress(offset);
	if (actSample == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Data = *actSample;
	if ((AdcNo & 1) == 0) Data >>= 16;
	return(!this->CheckBusTrap(Module, offset, "ReadActualSample"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger setup register
//! \param[in]		Module		-- module address
//! \param[in]		Data		-- data
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadTriggerSetup(SrvVMEModule * Module, Int_t & Data, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadTriggerSetup");
		return(kFALSE);
	}

	Int_t offset;
	switch (AdcNo) {
		case 0: 	offset = SIS3302_TRIGGER_SETUP_ADC1; break;
		case 1: 	offset = SIS3302_TRIGGER_SETUP_ADC2; break;
		case 2: 	offset = SIS3302_TRIGGER_SETUP_ADC3; break;
		case 3: 	offset = SIS3302_TRIGGER_SETUP_ADC4; break;
		case 4: 	offset = SIS3302_TRIGGER_SETUP_ADC5; break;
		case 5: 	offset = SIS3302_TRIGGER_SETUP_ADC6; break;
		case 6: 	offset = SIS3302_TRIGGER_SETUP_ADC7; break;
		case 7: 	offset = SIS3302_TRIGGER_SETUP_ADC8; break;
	}

	volatile Int_t * trigSetup = (volatile Int_t *) Module->MapAddress(offset);
	if (trigSetup == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Data = *trigSetup;
	return (!this->CheckBusTrap(Module, offset, "ReadTriggerSetup"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes trigger setup register
//! \param[in]		Module		-- module address
//! \param[in]		Data		-- data
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::WriteTriggerSetup(SrvVMEModule * Module, Int_t & Data, Int_t AdcNo) {

	if (AdcNo != kSis3302AllAdcs && (AdcNo < 0 || AdcNo >= kSis3302NofAdcs)) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteTriggerSetup");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t data;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			data = Data;
			if (!this->WriteTriggerSetup(Module, data, adc)) return(kFALSE);
		}
		Data = data;
		return(kTRUE);
	}

	Int_t offset;
	switch (AdcNo) {
		case 0: 	offset = SIS3302_TRIGGER_SETUP_ADC1; break;
		case 1: 	offset = SIS3302_TRIGGER_SETUP_ADC2; break;
		case 2: 	offset = SIS3302_TRIGGER_SETUP_ADC3; break;
		case 3: 	offset = SIS3302_TRIGGER_SETUP_ADC4; break;
		case 4: 	offset = SIS3302_TRIGGER_SETUP_ADC5; break;
		case 5: 	offset = SIS3302_TRIGGER_SETUP_ADC6; break;
		case 6: 	offset = SIS3302_TRIGGER_SETUP_ADC7; break;
		case 7: 	offset = SIS3302_TRIGGER_SETUP_ADC8; break;
	}

	volatile Int_t * trigSetup = (volatile Int_t *) Module->MapAddress(offset);
	if (trigSetup == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*trigSetup = Data;
	if (this->CheckBusTrap(Module, offset, "WriteTriggerSetup")) return(kFALSE);
	return(this->ReadTriggerSetup(Module, Data, AdcNo));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads EXTENDED trigger setup register
//! \param[in]		Module		-- module address
//! \param[in]		Data		-- data
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadTriggerExtendedSetup(SrvVMEModule * Module, Int_t & Data, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadTriggerExtendedSetup");
		return(kFALSE);
	}

	Int_t offset;
	switch (AdcNo) {
		case 0: 	offset = SIS3302_TRIGGER_SETUP_EXTENDED_ADC1; break;
		case 1: 	offset = SIS3302_TRIGGER_SETUP_EXTENDED_ADC2; break;
		case 2: 	offset = SIS3302_TRIGGER_SETUP_EXTENDED_ADC3; break;
		case 3: 	offset = SIS3302_TRIGGER_SETUP_EXTENDED_ADC4; break;
		case 4: 	offset = SIS3302_TRIGGER_SETUP_EXTENDED_ADC5; break;
		case 5: 	offset = SIS3302_TRIGGER_SETUP_EXTENDED_ADC6; break;
		case 6: 	offset = SIS3302_TRIGGER_SETUP_EXTENDED_ADC7; break;
		case 7: 	offset = SIS3302_TRIGGER_SETUP_EXTENDED_ADC8; break;
	}

	volatile Int_t * trigSetup = (volatile Int_t *) Module->MapAddress(offset);
	if (trigSetup == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Data = *trigSetup;
	return (!this->CheckBusTrap(Module, offset, "ReadTriggerExtendedSetup"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes EXTENDED trigger setup register
//! \param[in]		Module		-- module address
//! \param[in]		Data		-- data
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::WriteTriggerExtendedSetup(SrvVMEModule * Module, Int_t & Data, Int_t AdcNo) {

	if (AdcNo != kSis3302AllAdcs && (AdcNo < 0 || AdcNo >= kSis3302NofAdcs)) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteTriggerExtendedSetup");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t data;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			data = Data;
			if (!this->WriteTriggerExtendedSetup(Module, data, adc)) return(kFALSE);
		}
		Data = data;
		return(kTRUE);
	}

	Int_t offset;
	switch (AdcNo) {
		case 0: 	offset = SIS3302_TRIGGER_SETUP_EXTENDED_ADC1; break;
		case 1: 	offset = SIS3302_TRIGGER_SETUP_EXTENDED_ADC2; break;
		case 2: 	offset = SIS3302_TRIGGER_SETUP_EXTENDED_ADC3; break;
		case 3: 	offset = SIS3302_TRIGGER_SETUP_EXTENDED_ADC4; break;
		case 4: 	offset = SIS3302_TRIGGER_SETUP_EXTENDED_ADC5; break;
		case 5: 	offset = SIS3302_TRIGGER_SETUP_EXTENDED_ADC6; break;
		case 6: 	offset = SIS3302_TRIGGER_SETUP_EXTENDED_ADC7; break;
		case 7: 	offset = SIS3302_TRIGGER_SETUP_EXTENDED_ADC8; break;
	}

	volatile Int_t * trigSetup = (volatile Int_t *) Module->MapAddress(offset);
	if (trigSetup == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*trigSetup = Data;
	if (this->CheckBusTrap(Module, offset, "WriteTriggerExtendedSetup")) return(kFALSE);
	return(this->ReadTriggerExtendedSetup(Module, Data, AdcNo));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger peaking and gap times
//! \param[in]		Module		-- module address
//! \param[out]		Peak		-- peaking time
//! \param[out]		Gap 		-- gap time
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadTriggerPeakAndGap(SrvVMEModule * Module, Int_t & Peak, Int_t & Gap, Int_t AdcNo) {

	Int_t data, xdata;
	if (!this->ReadTriggerSetup(Module, data, AdcNo)) return(kFALSE);
	if (!this->ReadTriggerExtendedSetup(Module, xdata, AdcNo)) return(kFALSE);
	Peak = data & 0xFF;
	Peak |= (xdata & 0x3) >> 8;
	Int_t sumG = (data >> 8) & 0xFF;
	sumG |= xdata & 0x300;
	Gap = sumG - Peak;
	return(kTRUE);
}

Bool_t SrvSis3302::WriteTriggerPeakAndGap(SrvVMEModule * Module, Int_t & Peak, Int_t & Gap, Int_t AdcNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets trigger peaking and gap times
//! \param[in]		Module		-- module address
//! \param[in]		Peak		-- peaking time
//! \param[in]		Gap 		-- gap time
//! \param[in]		AdcNo 		-- adc number
//////////////////////////////////////////////////////////////////////////////

	if (this->IsStatus(kSis3302StatusCollectingTraces)) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Can't set trigger peak or gap - trace collection in progress ..." << endl;
		gMrbLog->Flush(this->ClassName(), "WriteTriggerPeakAndGap");
		return(kFALSE);
	}

	Int_t sumG = Peak + Gap;
	if (sumG > kSis3302TrigSumGMax || Peak < kSis3302TrigPeakMin || Peak > kSis3302TrigPeakMax || Gap < kSis3302TrigGapMin || Gap > kSis3302TrigGapMax) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Trigger peak time / gap time mismatch - "
						<< Peak << " ... " << Gap
						<< " (peak should be in [" << kSis3302TrigPeakMin << "," << kSis3302TrigPeakMax << "], gap should be in [" << kSis3302TrigGapMin << "," << kSis3302TrigGapMax << "], sumG=p+g <= " << kSis3302TrigSumGMax << endl;
		gMrbLog->Flush(this->ClassName(), "WriteTriggerPeakAndGap");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t g, p;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			g = Gap; p = Peak;
			if (!this->WriteTriggerPeakAndGap(Module, p, g, adc)) return(kFALSE);
		}
		Peak = p; Gap = g;
		return(kTRUE);
	}

	Int_t data, xdata;
	if (!this->ReadTriggerSetup(Module, data, AdcNo)) return(kFALSE);
	if (!this->ReadTriggerExtendedSetup(Module, xdata, AdcNo)) return(kFALSE);
	data &= 0xFFFF0000;
	data |= (sumG & 0xFF) << 8;
	data |= Peak & 0xFF;
	xdata &= 0xFFFF0000;
	xdata |= sumG & 0x300;
	xdata |= (Peak & 0x300) >> 8;
	if (!this->WriteTriggerSetup(Module, data, AdcNo)) return(kFALSE);
	if (!this->WriteTriggerExtendedSetup(Module, xdata, AdcNo)) return(kFALSE);
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger pulse length
//! \param[in]		Module			-- module address
//! \param[out]		PulseLength 	-- pulse length
//! \param[in]		AdcNo			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadTriggerPulseLength(SrvVMEModule * Module, Int_t & PulseLength, Int_t AdcNo) {

	Int_t data;
	if (!this->ReadTriggerSetup(Module, data, AdcNo)) return(kFALSE);
	PulseLength = (data >> 16) & 0xFF;
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets trigger pulse length
//! \param[in]		Module			-- module address
//! \param[in]		PulseLength 	-- pulse length
//! \param[in]		AdcNo 			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::WriteTriggerPulseLength(SrvVMEModule * Module, Int_t & PulseLength, Int_t AdcNo) {

	if (PulseLength < 0 || PulseLength > 0xFF) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Wrong trigger pulse length - "
						<< PulseLength << " (should be in [0," << 0xFF << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteTriggerPulseLength");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t pl;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			pl = PulseLength;
			if (!this->WriteTriggerPulseLength(Module, pl, adc)) return(kFALSE);
		}
		PulseLength = pl;
		return(kTRUE);
	}

	Int_t data;
	if (!this->ReadTriggerSetup(Module, data, AdcNo)) return(kFALSE);
	data &= 0xFF00FFFF;
	data |= ((PulseLength & 0xFF) << 16);
	if (!this->WriteTriggerSetup(Module, data, AdcNo)) return(kFALSE);
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger internal gate length
//! \param[in]		Module			-- module address
//! \param[out]		Gate			-- gate length
//! \param[in]		AdcNo			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadTriggerInternalGate(SrvVMEModule * Module, Int_t & Gate, Int_t AdcNo) {

	Int_t data;
	if (!this->ReadTriggerSetup(Module, data, AdcNo)) return(kFALSE);
	Gate = (data >> 24) & 0x3F;
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets trigger internal gate length
//! \param[in]		Module			-- module address
//! \param[in]		GateLength	 	-- gate length
//! \param[in]		AdcNo 			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::WriteTriggerInternalGate(SrvVMEModule * Module, Int_t & Gate, Int_t AdcNo) {

	if (Gate < 0 || Gate > 0x3F) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Wrong internal trigger gate - "
						<< Gate << " (should be in [0," << 0x3F << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteTriggerInternalGate");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t gl;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			gl = Gate;
			if (!this->WriteTriggerInternalGate(Module,gl, adc)) return(kFALSE);
		}
		Gate = gl;
		return(kTRUE);
	}

	Int_t data;
	if (!this->ReadTriggerSetup(Module, data, AdcNo)) return(kFALSE);
	data &= 0xFFFFFF;
	data |= (Gate << 24);
	if (!this->WriteTriggerSetup(Module, data, AdcNo)) return(kFALSE);
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger internal delay
//! \param[in]		Module			-- module address
//! \param[out]		Delay			-- delay length
//! \param[in]		AdcNo			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadTriggerInternalDelay(SrvVMEModule * Module, Int_t & Delay, Int_t AdcNo) {

	Int_t data;
	if (!this->ReadTriggerExtendedSetup(Module, data, AdcNo)) return(kFALSE);
	Delay = (data >> 24) & 0x3F;
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets trigger internal delay
//! \param[in]		Module			-- module address
//! \param[in]		Delay		 	-- delay length
//! \param[in]		AdcNo 			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::WriteTriggerInternalDelay(SrvVMEModule * Module, Int_t & Delay, Int_t AdcNo) {

	if (Delay < 0 || Delay > 0x3F) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Wrong internal trigger delay - "
						<< Delay << " (should be in [0," << 0x3F << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteTriggerInternalDelay");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t dl;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			dl = Delay;
			if (!this->WriteTriggerInternalDelay(Module, dl, adc)) return(kFALSE);
		}
		Delay = dl;
		return(kTRUE);
	}

	Int_t data;
	if (!this->ReadTriggerExtendedSetup(Module, data, AdcNo)) return(kFALSE);
	data &= 0xFFFFFF;
	data |= (Delay << 24);
	if (!this->WriteTriggerExtendedSetup(Module, data, AdcNo)) return(kFALSE);
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger decimation
//! \param[in]		Module			-- module address
//! \param[out]		Decimation		-- decimation
//! \param[in]		AdcNo 			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::GetTriggerDecimation(SrvVMEModule * Module, Int_t & Decimation, Int_t AdcNo) {

	Int_t data;
	if (!this->ReadTriggerExtendedSetup(Module, data, AdcNo)) return(kFALSE);
	Decimation = (data >> 16) & 0x3;
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets trigger decimation
//! \param[in]		Module			-- module address
//! \param[in]		Decimation		-- decimation
//! \param[in]		AdcNo 			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::SetTriggerDecimation(SrvVMEModule * Module, Int_t & Decimation, Int_t AdcNo) {

	if (Decimation < 0 || Decimation > 3) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Wrong trigger decimation - " << Decimation
						<< " (should be in [0,3]" << endl;
		gMrbLog->Flush(this->ClassName(), "SetTriggerDecimation");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t dc;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			dc = Decimation;
			if (!this->SetTriggerDecimation(Module, dc, adc)) return(kFALSE);
		}
		Decimation = dc;
		return(kTRUE);
	}

	Int_t data;
	if (!this->ReadTriggerExtendedSetup(Module, data, AdcNo)) return(kFALSE);
	data &= ~(0x3 << 16);
	data |= (Decimation << 16);
	if (!this->WriteTriggerExtendedSetup(Module, data, AdcNo)) return(kFALSE);
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger threshold register
//! \param[in]		Module		-- module address
//! \param[out]		Data		-- data
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadTriggerThreshReg(SrvVMEModule * Module, Int_t & Data, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadTriggerThreshReg");
		return(kFALSE);
	}

	Int_t offset;
	switch (AdcNo) {
		case 0: 	offset = SIS3302_TRIGGER_THRESHOLD_ADC1; break;
		case 1: 	offset = SIS3302_TRIGGER_THRESHOLD_ADC2; break;
		case 2: 	offset = SIS3302_TRIGGER_THRESHOLD_ADC3; break;
		case 3: 	offset = SIS3302_TRIGGER_THRESHOLD_ADC4; break;
		case 4: 	offset = SIS3302_TRIGGER_THRESHOLD_ADC5; break;
		case 5: 	offset = SIS3302_TRIGGER_THRESHOLD_ADC6; break;
		case 6: 	offset = SIS3302_TRIGGER_THRESHOLD_ADC7; break;
		case 7: 	offset = SIS3302_TRIGGER_THRESHOLD_ADC8; break;
	}

	volatile Int_t * trigThresh = (volatile Int_t *) Module->MapAddress(offset);
	if (trigThresh == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Data = *trigThresh;
	return (!this->CheckBusTrap(Module, offset, "ReadTriggerThreshReg"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes trigger threshold register
//! \param[in]		Module		-- module address
//! \param[in]		Data		-- data
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::WriteTriggerThreshReg(SrvVMEModule * Module, Int_t & Data, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteTriggerThreshReg");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t data;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			data = Data;
			if (!this->WriteTriggerThreshReg(Module, data, adc)) return(kFALSE);
		}
		Data = data;
		return(kTRUE);
	}

	Int_t offset;
	switch (AdcNo) {
		case 0: 	offset = SIS3302_TRIGGER_THRESHOLD_ADC1; break;
		case 1: 	offset = SIS3302_TRIGGER_THRESHOLD_ADC2; break;
		case 2: 	offset = SIS3302_TRIGGER_THRESHOLD_ADC3; break;
		case 3: 	offset = SIS3302_TRIGGER_THRESHOLD_ADC4; break;
		case 4: 	offset = SIS3302_TRIGGER_THRESHOLD_ADC5; break;
		case 5: 	offset = SIS3302_TRIGGER_THRESHOLD_ADC6; break;
		case 6: 	offset = SIS3302_TRIGGER_THRESHOLD_ADC7; break;
		case 7: 	offset = SIS3302_TRIGGER_THRESHOLD_ADC8; break;
	}

	volatile Int_t * trigThresh = (volatile Int_t *) Module->MapAddress(offset);
	if (trigThresh == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*trigThresh = Data;
	if (this->CheckBusTrap(Module, offset, "WriteTriggerThreshReg")) return(kFALSE);
	return(this->ReadTriggerThreshReg(Module, Data, AdcNo));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger threshold
//! \param[in]		Module		-- module address
//! \param[out]		Thresh		-- threshold
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadTriggerThreshold(SrvVMEModule * Module, Int_t & Thresh, Int_t AdcNo) {

	Int_t data;
	if (!this->ReadTriggerThreshReg(Module, data, AdcNo)) return(kFALSE);
	Thresh = data & 0xFFFF;
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes trigger threshold
//! \param[in]		Module		-- module address
//! \param[in]		Thresh		-- threshold
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::WriteTriggerThreshold(SrvVMEModule * Module, Int_t & Thresh, Int_t AdcNo) {

	if (Thresh < 0 || Thresh > 0xFFFF) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Wrong trigger threshold - "
						<< Thresh << " (should be in [0," << 0xFFFF << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteTriggerThreshold");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t th;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			th = Thresh;
			if (!this->WriteTriggerThreshold(Module, th, adc)) return(kFALSE);
		}
		Thresh = th;
		return(kTRUE);
	}

	Int_t data;
	if (!this->ReadTriggerThreshReg(Module, data, AdcNo)) return(kFALSE);
	data &= 0xFFFE0000;
	data |= (Thresh | 0x10000);
	if (!this->WriteTriggerThreshReg(Module, data, AdcNo)) return(kFALSE);
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger GT bit
//! \param[in]		Module		-- module address
//! \param[out]		Flag		-- GT
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::GetTriggerGT(SrvVMEModule * Module, Bool_t & Flag, Int_t AdcNo) {

	Int_t data;
	if (!this->ReadTriggerThreshReg(Module, data, AdcNo)) return(kFALSE);
	Flag = ((data & (0x1 << 25)) > 0);
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Turns trigger GT bit on or off
//! \param[in]		Module		-- module address
//! \param[in]		Flag		-- GT
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::SetTriggerGT(SrvVMEModule * Module, Bool_t & Flag, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs) {
		Bool_t fl;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			fl = Flag;
			if (!this->SetTriggerGT(Module, fl, adc)) return(kFALSE);
		}
		Flag = fl;
		return(kTRUE);
	}

	Int_t data;
	if (!this->ReadTriggerThreshReg(Module, data, AdcNo)) return(kFALSE);
	data &= ~(0x1 << 25);
	if (Flag) data |= (0x1 << 25);
	if (!this->WriteTriggerThreshReg(Module, data, AdcNo)) return(kFALSE);
	Flag = ((data & (0x1 << 25)) > 0);
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger OUT bit
//! \param[in]		Module		-- module address
//! \param[out]		Flag		-- OUT
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::GetTriggerOut(SrvVMEModule * Module, Bool_t & Flag, Int_t AdcNo) {

	Int_t data;
	if (!this->ReadTriggerThreshReg(Module, data, AdcNo)) return(kFALSE);
	Flag = ((data & (0x1 << 26)) > 0);
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Turns trigger OUT bit on or off
//! \param[in]		Module		-- module address
//! \param[in]		Flag		-- OUT
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::SetTriggerOut(SrvVMEModule * Module, Bool_t & Flag, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs) {
		Bool_t fl;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			fl = Flag;
			if (!this->SetTriggerOut(Module, fl, adc)) return(kFALSE);
		}
		Flag = fl;
		return(kTRUE);
	}

	Int_t data;
	if (!this->ReadTriggerThreshReg(Module, data, AdcNo)) return(kFALSE);
	data &= ~(0x1 << 26);
	if (Flag) data |= (0x1 << 26);
	if (!this->WriteTriggerThreshReg(Module, data, AdcNo)) return(kFALSE);
	Flag = ((data & (0x1 << 26)) > 0);
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads energy setup register
//! \param[in]		Module		-- module address
//! \param[out]		Data		-- data
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadEnergySetup(SrvVMEModule * Module, Int_t & Data, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadEnergySetup");
		return(kFALSE);
	}

	Int_t offset;
	switch (AdcNo) {
		case 0:
		case 1: 	offset = SIS3302_ENERGY_SETUP_GP_ADC12; break;
		case 2:
		case 3: 	offset = SIS3302_ENERGY_SETUP_GP_ADC34; break;
		case 4:
		case 5: 	offset = SIS3302_ENERGY_SETUP_GP_ADC56; break;
		case 6:
		case 7: 	offset = SIS3302_ENERGY_SETUP_GP_ADC78; break;
	}

	volatile Int_t * setup = (volatile Int_t *) Module->MapAddress(offset);
	if (setup == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Data = *setup;
	return (!this->CheckBusTrap(Module, offset, "ReadEnergySetup"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes energy setup register
//! \param[in]		Module		-- module address
//! \param[in]		Data		-- data
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::WriteEnergySetup(SrvVMEModule * Module, Int_t & Data, Int_t AdcNo) {

	if (AdcNo != kSis3302AllAdcs && (AdcNo < 0 || AdcNo >= kSis3302NofAdcs)) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteEnergySetup");
		return(kFALSE);
	}

	Int_t offset;
	switch (AdcNo) {
		case kSis3302AllAdcs:
					offset = SIS3302_ENERGY_SETUP_GP_ALL_ADC; break;
		case 0:
		case 1: 	offset = SIS3302_ENERGY_SETUP_GP_ADC12; break;
		case 2:
		case 3: 	offset = SIS3302_ENERGY_SETUP_GP_ADC34; break;
		case 4:
		case 5: 	offset = SIS3302_ENERGY_SETUP_GP_ADC56; break;
		case 6:
		case 7: 	offset = SIS3302_ENERGY_SETUP_GP_ADC78; break;
	}

	volatile Int_t * setup = (volatile Int_t *) Module->MapAddress(offset);
	if (setup == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*setup = Data;
	if (this->CheckBusTrap(Module, offset, "WriteEnergySetup")) return(kFALSE);
	return(this->ReadEnergySetup(Module, Data, (AdcNo == kSis3302AllAdcs) ? 1 : AdcNo));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads energy peaking and gap
//! \param[in]		Module		-- module address
//! \param[out]		Peak		-- peaking time
//! \param[out]		Gap 		-- gap time
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadEnergyPeakAndGap(SrvVMEModule * Module, Int_t & Peak, Int_t & Gap, Int_t AdcNo) {

	Int_t data;
	if (!this->ReadEnergySetup(Module, data, AdcNo)) return(kFALSE);
	Int_t p1 = data & 0xFF;
	Int_t p2 = (data >> 16) & 0x3;
	Peak = (p2 << 8) | p1;
	Gap = (data >> 8) & 0xFF;
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets energy peaking and gap
//! \param[in]		Module		-- module address
//! \param[in]		Peak		-- peaking time
//! \param[in]		Gap 		-- gap time
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::WriteEnergyPeakAndGap(SrvVMEModule * Module, Int_t & Peak, Int_t & Gap, Int_t AdcNo) {

	if (this->IsStatus(kSis3302StatusCollectingTraces)) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Can't set energy peak or gap - trace collection in progress ..." << endl;
		gMrbLog->Flush(this->ClassName(), "WriteEnergyPeakAndGap");
		return(kFALSE);
	}

	if (Peak < kSis3302EnergyPeakMin || Peak > kSis3302EnergyPeakMax) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Energy peak time mismatch - "
						<< Peak << " (should be in [0," << kSis3302EnergyPeakMax << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteEnergyPeakAndGap");
		return(kFALSE);
	}
	if (Gap < kSis3302EnergyGapMin || Gap > kSis3302EnergyGapMax) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Energy gap time mismatch - "
						<< Gap << " (should be in [0," << kSis3302EnergyGapMax << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteEnergyPeakAndGap");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t p, g;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			p = Peak; g = Gap;
			if (!this->WriteEnergyPeakAndGap(Module, p, g, adc)) return(kFALSE);
		}
		Peak = p; Gap = g;
		return(kTRUE);
	}

	Int_t data;
	if (!this->ReadEnergySetup(Module, data, AdcNo)) return(kFALSE);
	data &= 0xFFFC0000;
	data |= (Gap << 8);
	Int_t p = Peak & 0xFF;
	data |= Peak;
	p = (Peak >> 8) & 0x3;
	data |= (p << 16);
	if (!this->WriteEnergySetup(Module, data, AdcNo)) return(kFALSE);
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads energy decimation
//! \param[in]		Module			-- module address
//! \param[out]		Decimation		-- decimation
//! \param[in]		AdcNo 			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::GetEnergyDecimation(SrvVMEModule * Module, Int_t & Decimation, Int_t AdcNo) {

	Int_t data;
	if (!this->ReadEnergySetup(Module, data, AdcNo)) return(kFALSE);
	Decimation = (data >> 28) & 0x3;
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets energy decimation
//! \param[in]		Module			-- module address
//! \param[in]		Decimation		-- decimation
//! \param[in]		AdcNo 			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::SetEnergyDecimation(SrvVMEModule * Module, Int_t & Decimation, Int_t AdcNo) {

	if (Decimation < 0 || Decimation > 3) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Wrong energy decimation - " << Decimation
						<< " (should be in [0,3]" << endl;
		gMrbLog->Flush(this->ClassName(), "SetEnergyDecimation");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t dc;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			dc = Decimation;
			if (!this->SetEnergyDecimation(Module, dc, adc)) return(kFALSE);
		}
		Decimation = dc;
		return(kTRUE);
	}

	Int_t data;
	if (!this->ReadEnergySetup(Module, data, AdcNo)) return(kFALSE);
	data &= ~(0x3 << 28);
	data |= (Decimation << 28);
	if (!this->WriteEnergySetup(Module, data, AdcNo)) return(kFALSE);
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads energy gate register
//! \param[in]		Module		-- module address
//! \param[out]		Data		-- data
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadEnergyGateReg(SrvVMEModule * Module, Int_t & Data, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadEnergyGateReg");
		return(kFALSE);
	}

	Int_t offset;
	switch (AdcNo) {
		case 0:
		case 1: 	offset = SIS3302_ENERGY_GATE_LENGTH_ADC12; break;
		case 2:
		case 3: 	offset = SIS3302_ENERGY_GATE_LENGTH_ADC34; break;
		case 4:
		case 5: 	offset = SIS3302_ENERGY_GATE_LENGTH_ADC56; break;
		case 6:
		case 7: 	offset = SIS3302_ENERGY_GATE_LENGTH_ADC78; break;
	}

	volatile Int_t * gateReg = (volatile Int_t *) Module->MapAddress(offset);
	if (gateReg == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Data = *gateReg;
	return (!this->CheckBusTrap(Module, offset, "ReadEnergyGateReg"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets energy gate register
//! \param[in]		Module		-- module address
//! \param[in]		Data		-- data
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::WriteEnergyGateReg(SrvVMEModule * Module, Int_t & Data, Int_t AdcNo) {

	if (AdcNo != kSis3302AllAdcs && (AdcNo < 0 || AdcNo >= kSis3302NofAdcs)) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteEnergyGateReg");
		return(kFALSE);
	}

	Int_t offset;
	switch (AdcNo) {
		case kSis3302AllAdcs:
					offset = SIS3302_ENERGY_GATE_LENGTH_ALL_ADC; break;
		case 0:
		case 1: 	offset = SIS3302_ENERGY_GATE_LENGTH_ADC12; break;
		case 2:
		case 3: 	offset = SIS3302_ENERGY_GATE_LENGTH_ADC34; break;
		case 4:
		case 5: 	offset = SIS3302_ENERGY_GATE_LENGTH_ADC56; break;
		case 6:
		case 7: 	offset = SIS3302_ENERGY_GATE_LENGTH_ADC78; break;
	}

	volatile Int_t * gateReg = (volatile Int_t *) Module->MapAddress(offset);
	if (gateReg == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*gateReg = Data;
	if (this->CheckBusTrap(Module, offset, "WriteEnergyGateReg")) return(kFALSE);
	return(this->ReadEnergyGateReg(Module, Data, (AdcNo == kSis3302AllAdcs) ? 1 : AdcNo));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads energy gate length
//! \param[in]		Module  -- module address
//! \param[out]		GateLength     -- gate length
//! \param[in]		AdcNo              -- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadEnergyGateLength(SrvVMEModule * Module, Int_t & GateLength, Int_t AdcNo) {

	Int_t data;
	if (!this->ReadEnergyGateReg(Module, data, AdcNo)) return(kFALSE);
	GateLength = data & 0x1FF;
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets energy gate length
//! \param[in]		Module			-- module address
//! \param[in]		GateLength		-- gate length
//! \param[in]		AdcNo 			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::WriteEnergyGateLength(SrvVMEModule * Module, Int_t & GateLength, Int_t AdcNo) {

	if (GateLength < 0 || GateLength > 0x1FFFF) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Wrong energy gate length - " << GateLength
						<< " (should be in [0," << 0x1FFFF << "]" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteEnergyGateLength");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t gl;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			gl = GateLength;
			if (!this->WriteEnergyGateLength(Module, gl, adc)) return(kFALSE);
		}
		GateLength = gl;
		return(kTRUE);
	}

	Int_t data;
	if (!this->ReadEnergyGateReg(Module, data, AdcNo)) return(kFALSE);
	data &= 0xFFFE0000;
	data |= GateLength;
	if (!this->WriteEnergyGateReg(Module, data, AdcNo)) return(kFALSE);
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads test bits
//! \param[in]		Module		-- module address
//! \param[out]		Bits		-- bits
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::GetTestBits(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo) {

	Int_t data;
	if (!this->ReadEnergyGateReg(Module, data, AdcNo)) return(kFALSE);
	Bits = (data >> 28) & 0x3;
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes test bits
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- bits
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::SetTestBits(SrvVMEModule * Module, Int_t & Bits, Int_t AdcNo) {

	if (Bits < 0 || Bits > 0x3) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Wrong test bits - " << Bits
						<< " (should be in [0," << 0x3 << "]" << endl;
		gMrbLog->Flush(this->ClassName(), "SetTestBits");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t b;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			b = Bits;
			if (!this->SetTestBits(Module, b, adc)) return(kFALSE);
		}
		Bits = b;
		return(kTRUE);
	}

	Int_t data;
	if (!this->ReadEnergyGateReg(Module, data, AdcNo)) return(kFALSE);
	data &= 0xFFFFFFF;
	data |= (Bits << 28);
	if (!this->WriteEnergyGateReg(Module, data, AdcNo)) return(kFALSE);
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads energy sample length
//! \param[in]		Module			-- module address
//! \param[out]		SampleLength	-- length
//! \param[in]		AdcNo 			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadEnergySampleLength(SrvVMEModule * Module, Int_t & SampleLength, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadEnergySampleLength");
		return(kFALSE);
	}

	Int_t offset;
	switch (AdcNo) {
		case 0:
		case 1: 	offset = SIS3302_ENERGY_SAMPLE_LENGTH_ADC12; break;
		case 2:
		case 3: 	offset = SIS3302_ENERGY_SAMPLE_LENGTH_ADC34; break;
		case 4:
		case 5: 	offset = SIS3302_ENERGY_SAMPLE_LENGTH_ADC56; break;
		case 6:
		case 7: 	offset = SIS3302_ENERGY_SAMPLE_LENGTH_ADC78; break;
	}

	volatile Int_t * sample = (volatile Int_t *) Module->MapAddress(offset);
	if (sample == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	SampleLength = *sample;
	return (!this->CheckBusTrap(Module, offset, "ReadEnergySampleLength"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes energy sample length
//! \param[in]		Module			-- module address
//! \param[in]		SampleLength	-- length
//! \param[in]		AdcNo 			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::WriteEnergySampleLength(SrvVMEModule * Module, Int_t & SampleLength, Int_t AdcNo) {

	if (this->IsStatus(kSis3302StatusCollectingTraces)) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Can't write energy sample length - trace collection in progress ..." << endl;
		gMrbLog->Flush(this->ClassName(), "WriteEnergySampleLength");
		return(kFALSE);
	}

	if (AdcNo != kSis3302AllAdcs && (AdcNo < 0 || AdcNo >= kSis3302NofAdcs)) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteEnergySampleLength");
		return(kFALSE);
	}

	Int_t offset;
	switch (AdcNo) {
		case kSis3302AllAdcs:
					offset = SIS3302_ENERGY_SAMPLE_LENGTH_ALL_ADC; break;
		case 0:
		case 1: 	offset = SIS3302_ENERGY_SAMPLE_LENGTH_ADC12; break;
		case 2:
		case 3: 	offset = SIS3302_ENERGY_SAMPLE_LENGTH_ADC34; break;
		case 4:
		case 5: 	offset = SIS3302_ENERGY_SAMPLE_LENGTH_ADC56; break;
		case 6:
		case 7: 	offset = SIS3302_ENERGY_SAMPLE_LENGTH_ADC78; break;
	}
	volatile Int_t * sample = (volatile Int_t *) Module->MapAddress(offset);
	if (sample == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*sample = SampleLength;
	if (this->CheckBusTrap(Module, offset, "WriteEnergySampleLength")) return(kFALSE);
	return(this->ReadEnergySampleLength(Module, SampleLength, (AdcNo == kSis3302AllAdcs) ? 1 : AdcNo));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads tau value
//! \param[in]		Module		-- module address
//! \param[out]		Tau 		-- tau value
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadTauFactor(SrvVMEModule * Module, Int_t & Tau, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadTauFactor");
		return(kFALSE);
	}

	Int_t offset;
	switch (AdcNo) {
		case 0: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC1; break;
		case 1: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC2; break;
		case 2: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC3; break;
		case 3: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC4; break;
		case 4: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC5; break;
		case 5: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC6; break;
		case 6: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC7; break;
		case 7: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC8; break;
	}

	volatile Int_t * tauFactor = (volatile Int_t *) Module->MapAddress(offset);
	if (tauFactor == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Tau = *tauFactor;
	return (!this->CheckBusTrap(Module, offset, "ReadTauFactor"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes tau value
//! \param[in]		Module		-- module address
//! \param[in]		Tau 		-- tau value
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::WriteTauFactor(SrvVMEModule * Module, Int_t & Tau, Int_t AdcNo) {

	if (AdcNo != kSis3302AllAdcs && (AdcNo < 0 || AdcNo >= kSis3302NofAdcs)) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteTauFactor");
		return(kFALSE);
	}

	if (Tau < kSis3302EnergyTauMin || Tau > kSis3302EnergyTauMax) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Wrong tau factor - " << Tau
						<< " (should be in [0," << kSis3302EnergyTauMax << "]" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteTauFactor");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t t;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			t = Tau;
			if (!this->WriteTauFactor(Module, t, adc)) return(kFALSE);
		}
		Tau = t;
		return(kTRUE);
	}

	Int_t offset;
	switch (AdcNo) {
		case 0: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC1; break;
		case 1: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC2; break;
		case 2: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC3; break;
		case 3: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC4; break;
		case 4: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC5; break;
		case 5: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC6; break;
		case 6: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC7; break;
		case 7: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC8; break;
	}

	volatile Int_t * tauFactor = (volatile Int_t *) Module->MapAddress(offset);
	if (tauFactor == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*tauFactor = Tau;
	if (this->CheckBusTrap(Module, offset, "WriteTauFactor")) return(kFALSE);
	return(this->ReadTauFactor(Module, Tau, (AdcNo == kSis3302AllAdcs) ? 1 : AdcNo));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads sample start index
//! \param[in]		Module			-- module address
//! \param[out]		IndexValue		-- value
//! \param[in]		Index			-- index
//! \param[in]		AdcNo 			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadStartIndex(SrvVMEModule * Module, Int_t & IndexValue, Int_t Index, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadStartIndex");
		return(kFALSE);
	}

	Int_t offset;
	switch (Index) {
		case 0:
			{
				switch (AdcNo) {
					case 0:
					case 1: 	offset = SIS3302_ENERGY_SAMPLE_START_INDEX1_ADC12; break;
					case 2:
					case 3: 	offset = SIS3302_ENERGY_SAMPLE_START_INDEX1_ADC34; break;
					case 4:
					case 5: 	offset = SIS3302_ENERGY_SAMPLE_START_INDEX1_ADC56; break;
					case 6:
					case 7: 	offset = SIS3302_ENERGY_SAMPLE_START_INDEX1_ADC78; break;
				}
			}
			break;
		case 1:
			{
				switch (AdcNo) {
					case 0:
					case 1: 	offset = SIS3302_ENERGY_SAMPLE_START_INDEX2_ADC12; break;
					case 2:
					case 3: 	offset = SIS3302_ENERGY_SAMPLE_START_INDEX2_ADC34; break;
					case 4:
					case 5: 	offset = SIS3302_ENERGY_SAMPLE_START_INDEX2_ADC56; break;
					case 6:
					case 7: 	offset = SIS3302_ENERGY_SAMPLE_START_INDEX2_ADC78; break;
				}
			}
			break;
		case 2:
			{
				switch (AdcNo) {
					case 0:
					case 1: 	offset = SIS3302_ENERGY_SAMPLE_START_INDEX3_ADC12; break;
					case 2:
					case 3: 	offset = SIS3302_ENERGY_SAMPLE_START_INDEX3_ADC34; break;
					case 4:
					case 5: 	offset = SIS3302_ENERGY_SAMPLE_START_INDEX3_ADC56; break;
					case 6:
					case 7: 	offset = SIS3302_ENERGY_SAMPLE_START_INDEX3_ADC78; break;
				}
			}
			break;
		default:
			{
				gMrbLog->Err()	<< "[" << Module->GetName() << "]: Start index out of range - "
								<< Index << " (should be in [0,2])" << endl;
				gMrbLog->Flush(this->ClassName(), "ReadStartIndex");
				return(kFALSE);
			}
	}

	volatile Int_t * sampleIndex = (volatile Int_t *) Module->MapAddress(offset);
	if (sampleIndex == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	IndexValue = *sampleIndex;
	return (!this->CheckBusTrap(Module, offset, "ReadStartIndex"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes sample start index
//! \param[in]		Module			-- module address
//! \param[in]		IndexValue		-- value
//! \param[in]		Index			-- index
//! \param[in]		AdcNo 			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::WriteStartIndex(SrvVMEModule * Module, Int_t & IndexValue, Int_t Index, Int_t AdcNo) {

	if (AdcNo != kSis3302AllAdcs && (AdcNo < 0 || AdcNo >= kSis3302NofAdcs)) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteStartIndex");
		return(kFALSE);
	}

	if (IndexValue < 0 || IndexValue > 0x7FF) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Wrong sample index value - " << IndexValue
						<< " (should be in [0," << 0x7FF << "]" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteStartIndex");
		return(kFALSE);
	}

	if (Index < 0 || Index > 2) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Start index out of range - "
						<< Index << " (should be in [0,2])" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadStartIndex");
		return(kFALSE);
	}

	Int_t iv;
	if (AdcNo == kSis3302AllAdcs) {
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			iv = IndexValue;
			if (!this->WriteStartIndex(Module, iv, Index, adc)) return(kFALSE);
		}
		IndexValue = iv;
		return(kTRUE);
	}

	Int_t offset;
	switch (Index) {
		case 0:
			{
				switch (AdcNo) {
					case kSis3302AllAdcs:
								offset = SIS3302_ENERGY_SAMPLE_START_INDEX1_ALL_ADC; break;
					case 0:
					case 1: 	offset = SIS3302_ENERGY_SAMPLE_START_INDEX1_ADC12; break;
					case 2:
					case 3: 	offset = SIS3302_ENERGY_SAMPLE_START_INDEX1_ADC34; break;
					case 4:
					case 5: 	offset = SIS3302_ENERGY_SAMPLE_START_INDEX1_ADC56; break;
					case 6:
					case 7: 	offset = SIS3302_ENERGY_SAMPLE_START_INDEX1_ADC78; break;
				}
			}
			break;
		case 1:
			{
				switch (AdcNo) {
					case kSis3302AllAdcs:
								offset = SIS3302_ENERGY_SAMPLE_START_INDEX2_ALL_ADC; break;
					case 0:
					case 1: 	offset = SIS3302_ENERGY_SAMPLE_START_INDEX2_ADC12; break;
					case 2:
					case 3: 	offset = SIS3302_ENERGY_SAMPLE_START_INDEX2_ADC34; break;
					case 4:
					case 5: 	offset = SIS3302_ENERGY_SAMPLE_START_INDEX2_ADC56; break;
					case 6:
					case 7: 	offset = SIS3302_ENERGY_SAMPLE_START_INDEX2_ADC78; break;
				}
			}
			break;
		case 2:
			{
				switch (AdcNo) {
					case kSis3302AllAdcs:
								offset = SIS3302_ENERGY_SAMPLE_START_INDEX3_ALL_ADC; break;
					case 0:
					case 1: 	offset = SIS3302_ENERGY_SAMPLE_START_INDEX3_ADC12; break;
					case 2:
					case 3: 	offset = SIS3302_ENERGY_SAMPLE_START_INDEX3_ADC34; break;
					case 4:
					case 5: 	offset = SIS3302_ENERGY_SAMPLE_START_INDEX3_ADC56; break;
					case 6:
					case 7: 	offset = SIS3302_ENERGY_SAMPLE_START_INDEX3_ADC78; break;
				}
			}
			break;
	}


	volatile Int_t * sampleIndex = (volatile Int_t *) Module->MapAddress(offset);
	if (sampleIndex == NULL) return(kFALSE);
	gSignalTrap = kFALSE;
	*sampleIndex = IndexValue;
	if (this->CheckBusTrap(Module, offset, "WriteStartIndex")) return(kFALSE);
	return(this->ReadStartIndex(Module, IndexValue, Index, (AdcNo == kSis3302AllAdcs) ? 1 : AdcNo));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads acquisition control data
//! \param[in]		Module		-- module address
//! \param[out]		Data		-- data
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadAcquisitionControl(SrvVMEModule * Module, Int_t & Data) {

	Int_t offset = SIS3302_ACQUISITION_CONTROL;
	volatile Int_t * ctrl = (volatile Int_t *) Module->MapAddress(offset);
	if (ctrl == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Data = *ctrl;
	return (!this->CheckBusTrap(Module, offset, "ReadAcquisitionControl"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes acquisition control data
//! \param[in]		Module		-- module address
//! \param[in]		Data		-- data
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::WriteAcquisitionControl(SrvVMEModule * Module, Int_t & Data) {

	Int_t offset = SIS3302_ACQUISITION_CONTROL;
	volatile Int_t * ctrl = (volatile Int_t *) Module->MapAddress(offset);
	if (ctrl == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*ctrl = Data;
	return (!this->CheckBusTrap(Module, offset, "WriteAcquisitionControl"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads clock source
//! \param[in]		Module			-- module address
//! \param[out]		ClockSource		-- clock source
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::GetClockSource(SrvVMEModule * Module, Int_t & ClockSource) {
	Int_t data;
	if (!this->ReadAcquisitionControl(Module, data)) return(kFALSE);
	ClockSource = (data >> 12) & 0x7;
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets clock source
//! \param[in]		Module			-- module address
//! \param[in]		ClockSource		-- clock source
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::SetClockSource(SrvVMEModule * Module, Int_t & ClockSource) {
	if (ClockSource < 0 || ClockSource > 7) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Clock source out of range - "
						<< ClockSource << " (should be in [0,7])" << endl;
		gMrbLog->Flush(this->ClassName(), "SetClockSource");
		return(kFALSE);
	}
	Int_t data = (0x7 << 28);			// clear all bits
	if (!this->WriteAcquisitionControl(Module, data)) return(kFALSE);
	data = (ClockSource & 0x7) << 12;	// set bits
	return(this->WriteAcquisitionControl(Module, data));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads lemo-in mode
//! \param[in]		Module			-- module address
//! \param[out]		Bits			-- mode bits
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::GetLemoInMode(SrvVMEModule * Module, Int_t & Bits) {
	Int_t data;
	if (!this->ReadAcquisitionControl(Module, data)) return(kFALSE);
	Bits = data & 0x7;
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets lemo-in mode
//! \param[in]		Module			-- module address
//! \param[in]		Bits			-- mode bits
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::SetLemoInMode(SrvVMEModule * Module, Int_t & Bits) {
	if (Bits < 0 || Bits > 7) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: LEMO IN mode out of range - "
						<< Bits << " (should be in [0,7])" << endl;
		gMrbLog->Flush(this->ClassName(), "SetLemoInMode");
		return(kFALSE);
	}
	Int_t data = (0x7 << 16);			// clear all bits
	if (!this->WriteAcquisitionControl(Module, data)) return(kFALSE);
	data = Bits & 0x7;					// set bits
	return(this->WriteAcquisitionControl(Module, data));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads lemo-out mode
//! \param[in]		Module			-- module address
//! \param[out]		Bits			-- mode bits
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::GetLemoOutMode(SrvVMEModule * Module, Int_t & Bits) {
	Int_t data;
	if (!this->ReadAcquisitionControl(Module, data)) return(kFALSE);
	Bits = (data >> 4) & 0x3;
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets lemo-out mode
//! \param[in]		Module			-- module address
//! \param[in]		Bits			-- mode bits
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::SetLemoOutMode(SrvVMEModule * Module, Int_t & Bits) {
	if (Bits < 0 || Bits > 3) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: LEMO OUT mode out of range - "
						<< Bits << " (should be in [0,3])" << endl;
		gMrbLog->Flush(this->ClassName(), "SetLemoOutMode");
		return(kFALSE);
	}
	Int_t data = (0x3 << 20);			// clear all bits
	if (!this->WriteAcquisitionControl(Module, data)) return(kFALSE);
	data = (Bits & 0x3) << 4;			// set bits
	return(this->WriteAcquisitionControl(Module, data));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads lemo-in enable mask
//! \param[in]		Module			-- module address
//! \param[out]		Bits			-- bits
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::GetLemoInEnableMask(SrvVMEModule * Module, Int_t & Bits) {
	Int_t data;
	if (!this->ReadAcquisitionControl(Module, data)) return(kFALSE);
	Bits = (data >> 8) & 0x7;
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets lemo-in enable mask
//! \param[in]		Module			-- module address
//! \param[in]		Bits			-- bits
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::SetLemoInEnableMask(SrvVMEModule * Module, Int_t & Bits) {
	if (Bits < 0 || Bits > 7) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: LEMO IN enable mask out of range - "
						<< Bits << " (should be in [0,7])" << endl;
		gMrbLog->Flush(this->ClassName(), "SetLemoInEnableMask");
		return(kFALSE);
	}
	Int_t data = (0x7 << 24);			// clear all bits
	if (!this->WriteAcquisitionControl(Module, data)) return(kFALSE);
	data = (Bits & 0x7) << 8;			// set bits
	return(this->WriteAcquisitionControl(Module, data));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Starts collection of a given number of traces
//! \param[in]		Module			-- module address
//! \param[in]		NofEvents		-- number of events/traces
//! \param[in]		AdcPatt			-- adc pattern
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::StartTraceCollection(SrvVMEModule * Module, Int_t & NofEvents, Int_t AdcPatt) {

	Int_t maxWords = 0;
	Int_t bit = 1;
	Bool_t foundAdc = kFALSE;
	for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++, bit <<= 1) {
		if ((AdcPatt & bit) == 0) continue;
		foundAdc = kTRUE;
		Int_t rdl;
		if (!this->ReadRawDataSampleLength(Module, rdl, adc)) return(kFALSE);
		rawDataLength[adc] = rdl;		// 16bit, quad aligned
		Int_t edl;
		if (!this->ReadEnergySampleLength(Module, edl, adc)) return(kFALSE);
		energyDataLength[adc] = edl;	// 32bit
		wordsPerEvent[adc] = kSis3302EventHeader
							+ rdl/2		// raw data are 16bit on input
							+ edl
							+ kSis3302EventMinMax
							+ kSis3302EventTrailer;

		if (NofEvents == kSis3302MaxEvents) NofEvents = SIS3302_NEXT_ADC_OFFSET / (wordsPerEvent[adc] * sizeof(Int_t));	// max number of events
		Int_t nofWords = NofEvents * wordsPerEvent[adc];

		wordsPerEvent[adc] += rdl/2;	// raw data are 32bit on output!

		if (nofWords > (SIS3302_NEXT_ADC_OFFSET / sizeof(Int_t))) {
			gMrbLog->Wrn()	<< "[" << Module->GetName() << "]: Too many events - "
							<< NofEvents << " (event length is " << wordsPerEvent[adc] << "|0x" << setbase(16) <<  wordsPerEvent[adc] << " words, buffer size is " << SIS3302_NEXT_ADC_OFFSET << setbase(10) << " bytes)" << endl;
			gMrbLog->Flush(this->ClassName(), "StartTraceCollection");
			nofWords = SIS3302_NEXT_ADC_OFFSET - 1;
		}

		nofEventsPerBuffer[adc] = nofWords / wordsPerEvent[adc];

		if (nofWords > maxWords) maxWords = nofWords;
	}

	if (!foundAdc) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: At least 1 channel required" << endl;
		gMrbLog->Flush(this->ClassName(), "StartTraceCollection");
		return(kFALSE);
	}

	Int_t maxThresh = maxWords * 2;		// thresh has to be 16bit
	this->WriteEndAddrThresh(Module, maxThresh);
	this->KeyResetSample(Module);
	this->KeyClearTimestamp(Module);
	fSampling = kSis3302KeyArmBank1Sampling;
	this->ContinueTraceCollection(Module);

	this->SetStatus(kSis3302StatusCollectingTraces);

	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Stops trace collection
//! \param[in]		Module			-- module address
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::StopTraceCollection(SrvVMEModule * Module) {

	this->KeyDisarmSample(Module);
	fTraceCollection = kFALSE;
	this->ClearStatus(kSis3302StatusCollectingTraces);
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Continues trace collection
//! \param[in]		Module			-- module address
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ContinueTraceCollection(SrvVMEModule * Module) {

	this->KeyAddr(Module, fSampling);
	fTraceCollection = kTRUE;
	fNofTry = 0;
	this->SetStatus(kSis3302StatusCollectingTraces);
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Stops tracing temporarily
//! \param[in]		Module			-- module address
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::PauseTraceCollection(SrvVMEModule * Module) {
	Bool_t traceFlag = kFALSE;
	if (this->IsStatus(kSis3302StatusCollectingTraces)) {
		traceFlag = kTRUE;
		this->StopTraceCollection(Module);
	}
	return traceFlag;
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Get trace length
//! \param[in]		Module		-- module address
//! \param[out]		Data		-- where to store length values
//! \param[in]		AdcNo		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::GetTraceLength(SrvVMEModule * Module, TArrayI & Data, Int_t AdcNo) {

	fNofTry++;
	Data[0] = rawDataLength[AdcNo];
	Data[1] = energyDataLength[AdcNo];
	Data[3] = nofEventsPerBuffer[AdcNo];
	if (!fTraceCollection || !this->DataReady(Module)) {
		Data[2] = 0;
	} else {
		if (fMultiEvent) this->SwitchSampling(Module);
		Data[2] = wordsPerEvent[AdcNo];
		fTraceNo++;
	}
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Get trace data from adc buffer
//! \param[in]		Module		-- module address
//! \param[out]		Data		-- where to store event data
//! \param[in]		EventNo		-- event number
//! \param[in]		AdcNo		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::GetTraceData(SrvVMEModule * Module, TArrayI & Data, Int_t & EventNo, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "GetTraceData");
		return(kFALSE);
	}

	Int_t rdl = rawDataLength[AdcNo];
	Int_t edl = energyDataLength[AdcNo];
	Int_t wpt = wordsPerEvent[AdcNo];
	Int_t wpt2 = wpt - rdl/2;

	Int_t nextSample;
	if (!this->ReadNextSampleAddr(Module, nextSample, AdcNo)) return(kFALSE);
	nextSample &= 0x3FFFFC;
	nextSample >>= 1;

	Int_t evtStart;
	Int_t evtFirst, evtLast;

	if (EventNo == kSis3302MaxEvents) {
		evtStart = 0;
		evtFirst = 0;
		evtLast = (nextSample / (wpt - rdl/2)) - 1;
		Data.Set(nextSample + kSis3302EventPreHeader);
		Data[0] = rdl;
		Data[1] = edl;
		Data[2] = wpt;
		Data[3] = evtLast + 1;
	} else {
		evtStart = EventNo * wpt2 * sizeof(Int_t);
		evtFirst = EventNo;
		evtLast = EventNo;
		if (nextSample == 0 || (evtStart + wpt2 - 1) > nextSample) {
			gMrbLog->Err()	<< "[" << Module->GetName() << "]: Not enough trace data - can't get event #" << EventNo << endl;
			gMrbLog->Flush(this->ClassName(), "GetTraceData");
			return(kFALSE);
		}
		Data.Set(wpt + kSis3302EventPreHeader);
		Data[0] = rdl;
		Data[1] = edl;
		Data[2] = wpt;
		Data[3] = 1;
	}

	Int_t startAddr = SIS3302_ADC1_OFFSET + AdcNo * SIS3302_NEXT_ADC_OFFSET + evtStart;

	volatile Int_t * mappedAddr = (Int_t *) Module->MapAddress(startAddr);
	if (mappedAddr == NULL) return(kFALSE);

	gSignalTrap = kFALSE;

	Int_t k = kSis3302EventPreHeader;

	Bool_t start = kTRUE;
	ofstream dump;

	for (Int_t evtNo = evtFirst; evtNo <= evtLast; evtNo++) {

		for (Int_t i = 0; i < kSis3302EventHeader; i++, k++) Data[k] = *mappedAddr++;								// event header: 32bit words

		for (Int_t i = 0; i < rdl / 2; i++, k += 2) {			// raw data: fetch 2 samples packed in 32bit, store each in a single 32bit word
			Int_t d = *mappedAddr++;
			Data[k] = (d >> 16) & 0xFFFF;
			Data[k + 1] = d & 0xFFFF;
		}

		for (Int_t i = 0; i < edl; i++, k++) Data[k] = *mappedAddr++;			// event data: 32bit words

		Data[k] = *mappedAddr++;		// max energy
		Data[k + 1] = *mappedAddr++;	// min energy
		Data[k + 2] = *mappedAddr++;	// pile-up & trigger
		UInt_t trailer = (UInt_t) *mappedAddr;

		if (fDumpTrace | (trailer != 0xdeadbeef)) {
			if (start) {
				TString traceFile = Form("trace-%d.dmp", fTraceNo);
				char path[100];
				getcwd(path, 100);
				dump.open(traceFile.Data(), ios::out);
				if (!dump.good()) {
					gMrbLog->Err()	<< "[" << Module->GetName() << "]: Can't open file " << traceFile.Data() << " - dump cancelled" << endl;
					gMrbLog->Flush(this->ClassName(), "GetTraceData");
					fDumpTrace = kFALSE;
				} else {
					fDumpTrace = kTRUE;
					gMrbLog->Out()	<< "[" << Module->GetName() << "]: Dumping trace data to file " << path << "/" << traceFile.Data() << endl;
					gMrbLog->Flush(this->ClassName(), "GetTraceData");

					dump << "----------------------------------------------------[start of trace data]" << endl;
					dump << "Raw data length   : " << rdl << endl;
					dump << "Energy data lenght: " << edl << endl;
					dump << "Words per trace   : " << wpt << endl;
					Int_t thresh;
					this->ReadEndAddrThresh(Module, thresh, AdcNo);
					dump << "Start address     : 0x" << setbase(16) << startAddr << setbase(10) << endl;
					dump << "End thresh        : " << thresh << " 0x" << setbase(16) << thresh << setbase(10) << endl;
					dump << "Next sample       : " << nextSample << " 0x" << setbase(16) << nextSample << setbase(10) << endl;
				}
			}
			start = kFALSE;

			if (fDumpTrace)	{
				dump << "---------------------------------------------------------[header section]" << endl;
				dump << "Event number      : " << evtNo << endl;
				k = kSis3302EventPreHeader;
				TArrayI hdr(2);
				for (Int_t i = 0; i < kSis3302EventHeader; i++, k++) {
					hdr[i] = Data[k];
					dump << "Header[" << i << "]         : " << Form("%10u %#lx", Data[k], Data[k]) << endl;
				}
				unsigned long long ts = (UInt_t) hdr[1];
				UInt_t ts48 = ((UInt_t) hdr[0] >> 16) & 0xFFFF;
				ts += ts48 * 0x100000000LL;
				dump << "Time stamp        : " << ts << endl;

				if (rdl > 0) {
					dump << "----------------------------------------------[start of raw data section]" << endl;
					for (Int_t i = 0; i < rdl; i++, k ++) dump << Form("%5d: %10d %#lx", i, Data[k], Data[k]) << endl;
				}

				if (edl > 0) {
					dump << "----------------------------------------------[start of energy data section]" << endl;
					for (Int_t i = 0; i < edl; i++, k++) dump << Form("%5d: %10d %#lx", i, Data[k], Data[k]) << endl;
				}

				dump << "--------------------------------------------------------[trailer section]" << endl;
				dump << "Max energy       : " << Data[k] << endl;
				dump << "Min energy       : " << Data[k + 1] << endl;
				dump << "PileUp & trigger : 0x" << setbase(16) << Data[k + 2] << setbase(10) << endl;
				dump << "Trailer          : 0x" << setbase(16) << trailer << setbase(10) << endl;
				dump << "------------------------------------------------------[end of trace data]" << endl;
			}

			if (trailer != 0xdeadbeef) {
				gMrbLog->Err()	<< "[" << Module->GetName() << "]: Out of phase - trailer="
								<< setbase(16) << trailer << " (should be 0xdeadbeef)" << endl;
				gMrbLog->Flush(this->ClassName(), "GetTraceData");
				trailer = 0xdeadbeef;
			}
		}
		Data[k + 3] = trailer;	// trailer
	}

	if (fDumpTrace) dump.close();
	fDumpTrace = kFALSE;

	return(!this->CheckBusTrap(Module, startAddr, "GetTraceData"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Check if data ready
//! \param[in]		Module		-- module address
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::DataReady(SrvVMEModule * Module) {
	Int_t sts;
	this->ReadAcquisitionControl(Module, sts);
	return((sts & SIS3302_STATUS_END_THRESH) != 0);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Switch sampling
//! \param[in]		Module		-- module address
//////////////////////////////////////////////////////////////////////////////

void SrvSis3302::SwitchSampling(SrvVMEModule * Module) {
	if (!fTraceCollection || !fMultiEvent) return(0);
	Int_t pageNo;
	if (fSampling == kSis3302KeyArmBank1Sampling) {
		fSampling = kSis3302KeyArmBank2Sampling;
		pageNo = 0;
	} else {
		fSampling = kSis3302KeyArmBank1Sampling;
		pageNo = 4;
	}
	this->ContinueTraceCollection(Module);
	this->SetPageRegister(Module, pageNo);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Set page register
//! \param[in]		Module		-- module address
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::SetPageRegister(SrvVMEModule * Module, Int_t PageNumber) {
	Int_t offset = SIS3302_ADC_MEMORY_PAGE_REGISTER;
	volatile Int_t * pageReg = (volatile Int_t *) Module->MapAddress(offset);
	if (pageReg == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*pageReg = PageNumber;
	return (!this->CheckBusTrap(Module, offset, "SetPageRegister"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads irq configuration
//! \param[in]		Module		-- module address
//! \param[out] 	Vector		-- interrupt vector
//! \param[out] 	Level		-- interrupt level
//! \param[out] 	EnableFlag	-- interrupt enable
//! \param[out] 	RoakFlag	-- ROAK flag
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadIRQConfiguration(SrvVMEModule * Module, Int_t & Vector, Int_t & Level, Bool_t & EnableFlag, Bool_t & RoakFlag) {

	Int_t offset = SIS3302_IRQ_CONFIG;
	volatile Int_t * irq = (volatile Int_t *) Module->MapAddress(offset);
	if (irq == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	UInt_t irqBits = *irq;
	Vector = irqBits & 0xFF;
	irqBits >>= 8;
	Level = irqBits & 0x7;
	EnableFlag = ((irqBits & 0x8) != 0);
	RoakFlag = ((irqBits & 0x10) != 0);
	return (!this->CheckBusTrap(Module, offset, "ReadIRQConfiguration"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes acquisition control data
//! \param[in]		Module		-- module address
//! \param[in]	 	Vector		-- interrupt vector
//! \param[in]	 	Level		-- interrupt level
//! \param[in]	 	RoakFlag	-- ROAK flag
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::WriteIRWConfiguration(SrvVMEModule * Module, Int_t Vector, Int_t Level = 0, Bool_t RoakMode = kFALSE) {

	Int_t offset = SIS3302_IRQ_CONFIG;
	volatile Int_t * irq = (volatile Int_t *) Module->MapAddress(offset);
	if (irq == NULL) return(kFALSE);

	UInt_t irqBits = Level & 0x7;
	if (RoakMode) irqBits |= 0x10;
	irqBits <<= 8;
	irqBits |= Vector & 0xFF;
	gSignalTrap = kFALSE;
	*irq = irqBits;
	return (!this->CheckBusTrap(Module, offset, "WriteIRWConfiguration"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Enable IRQ source
//! \param[in]		Module		-- module address
//! \param[in] 		IrqSource	-- interrupt source
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::EnableIRQSource(SrvVMEModule * Module, UInt_t IrqSource) {

	if (IrqSource < 0 || IrqSource > 7) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Wrong IRQ source - "
						<< IrqSource << " (should be in [0,7])" << endl;
		gMrbLog->Flush(this->ClassName(), "EnableIRQSource");
		return(kFALSE);
	}

	Int_t offset = SIS3302_IRQ_CONTROL;
	volatile Int_t * irq = (volatile Int_t *) Module->MapAddress(offset);
	if (irq == NULL) return(kFALSE);
	gSignalTrap = kFALSE;
	UInt_t irqBits = *irq;
	*irq = irqBits | (1 << IrqSource);
	return (!this->CheckBusTrap(Module, offset, "EnableIRQSource"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Enable interrupt
//! \param[in]		Module		-- module address
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::EnableIRQ(SrvVMEModule * Module) {

	Int_t offset = SIS3302_IRQ_CONFIG;
	volatile Int_t * irq = (volatile Int_t *) Module->MapAddress(offset);
	if (irq == NULL) return(kFALSE);
	gSignalTrap = kFALSE;
	UInt_t irqBits = *irq;
	*irq = irqBits | 0x800;
	return (!this->CheckBusTrap(Module, offset, "EnableIRQ"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Disable IRQ source
//! \param[in]		Module		-- module address
//! \param[in] 		IrqSource	-- interrupt source
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::DisableIRQSource(SrvVMEModule * Module, UInt_t IrqSource) {

	if (IrqSource < 0 || IrqSource > 7) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Wrong IRQ source - "
						<< IrqSource << " (should be in [0,7])" << endl;
		gMrbLog->Flush(this->ClassName(), "DisableIRQSource");
		return(kFALSE);
	}

	Int_t offset = SIS3302_IRQ_CONTROL;
	volatile Int_t * irq = (volatile Int_t *) Module->MapAddress(offset);
	if (irq == NULL) return(kFALSE);
	gSignalTrap = kFALSE;
	UInt_t irqBits = *irq;
	*irq = irqBits & ~(0x10000 << IrqSource);
	return (!this->CheckBusTrap(Module, offset, "DisableIRQSource"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Disable interrupt
//! \param[in]		Module		-- module address
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::DisableIRQ(SrvVMEModule * Module) {

	Int_t offset = SIS3302_IRQ_CONFIG;
	volatile Int_t * irq = (volatile Int_t *) Module->MapAddress(offset);
	if (irq == NULL) return(kFALSE);
	gSignalTrap = kFALSE;
	UInt_t irqBits = *irq;
	*irq = irqBits & ~0x800;
	return (!this->CheckBusTrap(Module, offset, "DisableIRQSource"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Read irq status
//! \param[in]		Module		-- module address
//! \param[out		IrqStatus	-- interrupt status bits
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadIRQSourceStatus(SrvVMEModule * Module, UInt_t & IrqStatus) {

	Int_t offset = SIS3302_IRQ_CONTROL;
	volatile Int_t * irq = (volatile Int_t *) Module->MapAddress(offset);
	if (irq == NULL) return(kFALSE);
	gSignalTrap = kFALSE;
	IrqStatus = (*irq >> 24) & 0xFF;
	return (this->CheckBusTrap(Module, offset, "ReadIRQSourceStatus"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Read irq enable status
//! \param[in]		Module		-- module address
//! \param[out		IrqStatus	-- interrupt enable bits
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadIRQEnableStatus(SrvVMEModule * Module, UInt_t & IrqStatus) {

	Int_t offset = SIS3302_IRQ_CONTROL;
	volatile Int_t * irq = (volatile Int_t *) Module->MapAddress(offset);
	if (irq == NULL) return(kFALSE);
	gSignalTrap = kFALSE;
	IrqStatus = *irq & 0xFF;
	return (this->CheckBusTrap(Module, offset, "ReadIRQEnableStatus"));
}

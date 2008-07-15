//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TC2LSis3302.cxx
// Purpose:        MARaBOU client to connect to LynxOs/VME
// Description:    Implements class methods to connect to VME modules
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TC2LSis3302.cxx,v 1.2 2008-07-15 08:14:06 Rudolf.Lutter Exp $     
// Date:           $Date: 2008-07-15 08:14:06 $
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <iostream>
#include <iomanip>

#include "Rtypes.h"
#include "TEnv.h"
#include "TDatime.h"
#include "TROOT.h"

#include "TMrbC2Lynx.h"
#include "TC2LSis3302.h"

#include "TMrbTemplate.h"
#include "TMrbNamedX.h"
#include "TMrbLogger.h"
#include "SetColor.h"

#include "M2L_CommonDefs.h"

extern TMrbLogger * gMrbLog;
extern TMrbC2Lynx * gMrbC2Lynx;

ClassImp(TC2LSis3302)

Bool_t TC2LSis3302::ExecFunction(Int_t Fcode, TArrayI & DataSend, TArrayI & DataRecv, Int_t AdcNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TC2LSis3302::ExecFunction()
// Purpose:        Execute function
// Arguments:      Int_t Fcode            -- function code
//                 TArrayI & DataSend     -- data (send)
//                 TArrayI & DataRecv     -- data (receive)
//                 Int_t AdcNo            -- channel/adc
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Executes function.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t wc = DataSend.GetSize() + 1;
	M2L_VME_Exec_Function * x = (M2L_VME_Exec_Function *) gMrbC2Lynx->AllocMessage(sizeof(M2L_VME_Exec_Function), wc, kM2L_MESS_VME_EXEC_FUNCTION);
	x->fData.fData[0] = AdcNo;
	Int_t * dp = &x->fData.fData[1];
	for (Int_t i = 0; i < DataSend.GetSize(); i++, dp++) *dp = DataSend[i];
	x->fXhdr.fHandle = this->GetHandle();
	x->fXhdr.fCode = Fcode;
	x->fData.fWc = wc;
	if (gMrbC2Lynx->Send((M2L_MsgHdr *) x)) {
		wc = DataRecv.GetSize();
		M2L_VME_Return_Results * r = (M2L_VME_Return_Results *) gMrbC2Lynx->AllocMessage(sizeof(M2L_VME_Return_Results), wc, kM2L_MESS_VME_EXEC_FUNCTION);
		if (gMrbC2Lynx->Recv((M2L_MsgHdr *) r)) {
			wc = r->fData.fWc;
			DataRecv.Set(wc);
			Int_t * dp = r->fData.fData;
			for (Int_t i = 0; i < wc; i++, dp++) DataRecv[i] = *dp;
			return(kTRUE);
		} else {
			return(kFALSE);
		}
	} else {
		return(kFALSE);
	}
}

Bool_t TC2LSis3302::GetModuleInfo(Int_t & BoardId, Int_t & MajorVersion, Int_t & MinorVersion) {
	M2L_VME_Exec_Function x;
	gMrbC2Lynx->InitMessage((M2L_MsgHdr *) &x, sizeof(M2L_VME_Exec_Function), kM2L_MESS_VME_EXEC_FUNCTION);
	x.fData.fData[0] = kSis3302AllAdcs;
	x.fXhdr.fHandle = this->GetHandle();
	x.fXhdr.fCode = kM2L_FCT_GET_MODULE_INFO;
	x.fData.fWc = 1;
	if (gMrbC2Lynx->Send((M2L_MsgHdr *) &x)) {
		M2L_VME_Return_Module_Info r;
		if (gMrbC2Lynx->Recv((M2L_MsgHdr *) &r)) {
			BoardId = r.fBoardId;
			MajorVersion = r.fMajorVersion;
			MinorVersion = r.fMinorVersion;
			return(kTRUE);
		} else {
			return(kFALSE);
		}
	} else {
		return(kFALSE);
	}
}
	
Bool_t TC2LSis3302::ReadDac(TArrayI & DacValues, Int_t AdcNo) {
	TArrayI dataSend(0);
	return(this->ExecFunction(kM2L_FCT_SIS_3302_READ_DAC, dataSend, DacValues, AdcNo));
}
	
Bool_t TC2LSis3302::WriteDac(TArrayI & DacValues, Int_t AdcNo) {
	return(this->ExecFunction(kM2L_FCT_SIS_3302_WRITE_DAC, DacValues, DacValues, AdcNo));
}
	
Bool_t TC2LSis3302::KeyAddr(Int_t Key) {
	TArrayI keyData(1);
	keyData[0] = Key;
	return(this->ExecFunction(kM2L_FCT_SIS_3302_WRITE_DAC, keyData, keyData));
}

Bool_t TC2LSis3302::ReadEventConfig(Int_t & Bits, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_READ_EVENT_CONFIG, dataSend, bits, AdcNo)) return(kFALSE);
	Bits = bits[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::WriteEventConfig(Int_t & Bits, Int_t AdcNo) {
	TArrayI bits(1); bits[0] = Bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_WRITE_EVENT_CONFIG, bits, bits, AdcNo)) return(kFALSE);
	Bits = bits[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::SetHeaderBits(Int_t & Bits, Int_t AdcNo) {
	TArrayI bits(1); bits[0] = Bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_SET_HEADER_BITS, bits, bits, AdcNo)) return(kFALSE);
	Bits = bits[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::GetHeaderBits(Int_t & Bits, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_GET_HEADER_BITS, dataSend, bits, AdcNo)) return(kFALSE);
	Bits = bits[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::SetTriggerMode(Int_t & Bits, Int_t AdcNo) {
	TArrayI bits(1); bits[0] = Bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_SET_TRIGGER_MODE, bits, bits, AdcNo)) return(kFALSE);
	Bits = bits[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::GetTriggerMode(Int_t & Bits, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_GET_TRIGGER_MODE, dataSend, bits, AdcNo)) return(kFALSE);
	Bits = bits[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::SetPolarity(Bool_t & InvertFlag, Int_t AdcNo) {
	TArrayI flag(1); flag[0] = InvertFlag ? 1 : 0;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_SET_POLARITY, flag, flag, AdcNo)) return(kFALSE);
	InvertFlag = flag[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::GetPolarity(Bool_t & InvertFlag, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_GET_POLARITY, dataSend, bits, AdcNo)) return(kFALSE);
	InvertFlag = (bits[0] != 0);
	return(kTRUE);
}

Bool_t TC2LSis3302::ReadEndAddrThresh(Int_t & Thresh, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI thresh;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_READ_END_ADDR_THRESH, dataSend, thresh, AdcNo)) return(kFALSE);
	Thresh = thresh[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::WriteEndAddrThresh(Int_t & Thresh, Int_t AdcNo) {
	TArrayI thresh(1); thresh[0] = Thresh;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_WRITE_END_ADDR_THRESH, thresh, thresh, AdcNo)) return(kFALSE);
	Thresh = thresh[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::ReadPreTrigDelay(Int_t & Delay, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI delay;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_READ_PRETRIG_DELAY, dataSend, delay, AdcNo)) return(kFALSE);
	Delay = delay[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::WritePreTrigDelay(Int_t & Delay, Int_t AdcNo) {
	TArrayI delay(1); delay[0] = Delay;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_WRITE_PRETRIG_DELAY, delay, delay, AdcNo)) return(kFALSE);
	Delay = delay[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::ReadTrigGateLength(Int_t & Gate, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI gate;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_READ_TRIG_GATE_LENGTH, dataSend, gate, AdcNo)) return(kFALSE);
	Gate = gate[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::WriteTrigGateLength(Int_t & Gate, Int_t AdcNo) {
	TArrayI gate(1); gate[0] = Gate;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_WRITE_TRIG_GATE_LENGTH, gate, gate, AdcNo)) return(kFALSE);
	Gate = gate[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::ReadRawDataBufConfig(Int_t & Bits, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_READ_RAW_DATA_BUF_CONFIG, dataSend, bits, AdcNo)) return(kFALSE);
	Bits = bits[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::WriteRawDataBufConfig(Int_t & Bits, Int_t AdcNo) {
	TArrayI bits(1); bits[0] = Bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_WRITE_RAW_DATA_BUF_CONFIG, bits, bits, AdcNo)) return(kFALSE);
	Bits = bits[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::ReadNextSampleAddr(Int_t & Addr, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI addr;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_READ_NEXT_SAMPLE_ADDR, dataSend, addr, AdcNo)) return(kFALSE);
	Addr = addr[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::ReadPrevBankSampleAddr(Int_t & Addr, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI addr;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_READ_PREV_BANK_SAMPLE_ADDR, dataSend, addr, AdcNo)) return(kFALSE);
	Addr = addr[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::ReadActualSample(Int_t & Data, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI data;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_READ_ACTUAL_SAMPLE, dataSend, data, AdcNo)) return(kFALSE);
	Data = data[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::ReadTriggerPeakAndGap(Int_t & Peak, Int_t & Gap, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI peakgap;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_READ_TRIG_PEAK_AND_GAP, dataSend, peakgap, AdcNo)) return(kFALSE);
	Peak = peakgap[0];
	Gap = peakgap[1];
	return(kTRUE);
}

Bool_t TC2LSis3302::WriteTriggerPeakAndGap(Int_t & Peak, Int_t & Gap, Int_t AdcNo) {
	TArrayI peakgap(2); peakgap[0] = Peak; peakgap[1] = Gap;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_WRITE_TRIG_PEAK_AND_GAP, peakgap, peakgap, AdcNo)) return(kFALSE);
	Peak = peakgap[0];
	Gap = peakgap[1];
	return(kTRUE);
}

Bool_t TC2LSis3302::ReadTriggerPulseLength(Int_t & PulseLength, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI pulse;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_READ_TRIG_PULSE_LENGTH, dataSend, pulse, AdcNo)) return(kFALSE);
	PulseLength = pulse[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::WriteTriggerPulseLength(Int_t & PulseLength, Int_t AdcNo) {
	TArrayI pulse(1); pulse[0] = PulseLength;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_WRITE_TRIG_PULSE_LENGTH, pulse, pulse, AdcNo)) return(kFALSE);
	PulseLength = pulse[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::ReadTriggerThreshold(Int_t & Thresh, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI thresh;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_READ_TRIG_THRESH, dataSend, thresh, AdcNo)) return(kFALSE);
	Thresh = thresh[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::WriteTriggerThreshold(Int_t & Thresh, Int_t AdcNo) {
	TArrayI thresh(1); thresh[0] = Thresh;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_WRITE_TRIG_THRESH, thresh, thresh, AdcNo)) return(kFALSE);
	Thresh = thresh[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::GetTriggerGT(Bool_t & GTFlag, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_GET_TRIGGER_GT, dataSend, bits, AdcNo)) return(kFALSE);
	GTFlag = (bits[0] != 0);
	return(kTRUE);
}

Bool_t TC2LSis3302::SetTriggerGT(Bool_t & GTFlag, Int_t AdcNo) {
	TArrayI bits(1); bits[0] = GTFlag ? 1 : 0;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_SET_TRIGGER_GT, bits, bits, AdcNo)) return(kFALSE);
	GTFlag = (bits[0] != 0);
	return(kTRUE);
}

Bool_t TC2LSis3302::GetTriggerOut(Bool_t & OutFlag, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_GET_TRIGGER_OUT, dataSend, bits, AdcNo)) return(kFALSE);
	OutFlag = (bits[0] != 0);
	return(kTRUE);
}

Bool_t TC2LSis3302::SetTriggerOut(Bool_t & OutFlag, Int_t AdcNo) {
	TArrayI bits(1); bits[0] = OutFlag ? 1 : 0;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_SET_TRIGGER_OUT, bits, bits, AdcNo)) return(kFALSE);
	OutFlag = (bits[0] != 0);
	return(kTRUE);
}

Bool_t TC2LSis3302::ReadEnergyPeakAndGap(Int_t & Peak, Int_t & Gap, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI peakgap;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_READ_ENERGY_PEAK_AND_GAP, dataSend, peakgap, AdcNo)) return(kFALSE);
	Peak = peakgap[0];
	Gap = peakgap[1];
	return(kTRUE);
}

Bool_t TC2LSis3302::WriteEnergyPeakAndGap(Int_t & Peak, Int_t & Gap, Int_t AdcNo) {
	TArrayI peakgap(2); peakgap[0] = Peak; peakgap[1] = Gap;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_WRITE_ENERGY_PEAK_AND_GAP, peakgap, peakgap, AdcNo)) return(kFALSE);
	Peak = peakgap[0];
	Gap = peakgap[1];
	return(kTRUE);
}

Bool_t TC2LSis3302::GetDecimation(Int_t & Decimation, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI decim;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_GET_DECIMATION, dataSend, decim, AdcNo)) return(kFALSE);
	Decimation = decim[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::SetDecimation(Int_t & Decimation, Int_t AdcNo) {
	TArrayI decim(1); decim[0] = Decimation;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_SET_DECIMATION, decim, decim, AdcNo)) return(kFALSE);
	Decimation = decim[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::ReadEnergyGateLength(Int_t & GateLength, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI gate;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_READ_ENERGY_GATE_LENGTH, dataSend, gate, AdcNo)) return(kFALSE);
	GateLength = gate[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::WriteEnergyGateLength(Int_t & GateLength, Int_t AdcNo) {
	TArrayI gate(1); gate[0] = GateLength;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_WRITE_ENERGY_GATE_LENGTH, gate, gate, AdcNo)) return(kFALSE);
	GateLength = gate[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::GetTestBits(Int_t & Bits, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_GET_TEST_BITS, dataSend, bits, AdcNo)) return(kFALSE);
	Bits = bits[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::SetTestBits(Int_t & Bits, Int_t AdcNo) {
	TArrayI bits(1); bits[0] = Bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_SET_TEST_BITS, bits, bits, AdcNo)) return(kFALSE);
	Bits = bits[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::ReadEnergySampleLength(Int_t & SampleLength, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI sample;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_READ_ENERGY_SAMPLE_LENGTH, dataSend, sample, AdcNo)) return(kFALSE);
	SampleLength = sample[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::WriteEnergySampleLength(Int_t & SampleLength, Int_t AdcNo) {
	TArrayI sample(1); sample[0] = SampleLength;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_READ_ENERGY_SAMPLE_LENGTH, sample, sample, AdcNo)) return(kFALSE);
	SampleLength = sample[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::ReadStartIndex(Int_t & IdxVal, Int_t IdxNo, Int_t AdcNo) {
	TArrayI dataSend(1); dataSend[0] = IdxNo;
	TArrayI idxVal;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_READ_START_INDEX, dataSend, idxVal, AdcNo)) return(kFALSE);
	IdxVal = idxVal[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::WriteStartIndex(Int_t & IdxVal, Int_t IdxNo, Int_t AdcNo) {
	TArrayI idxVal; idxVal[0] = IdxNo;  idxVal[1] = IdxVal;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_WRITE_START_INDEX, idxVal, idxVal, AdcNo)) return(kFALSE);
	IdxVal = idxVal[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::ReadTauFactor(Int_t & Tau, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI tauFact;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_READ_TAU_FACTOR, dataSend, tauFact, AdcNo)) return(kFALSE);
	Tau = tauFact[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::WriteTauFactor(Int_t & Tau, Int_t AdcNo) {
	TArrayI tauFact(1); tauFact[0] = Tau;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_WRITE_TAU_FACTOR, tauFact, tauFact, AdcNo)) return(kFALSE);
	Tau = tauFact[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::GetExternalTrigger(Bool_t & Xtrig) {
	TArrayI dataSend(0);
	TArrayI bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_GET_EXTERNAL_TRIGGER, dataSend, bits, kSis3302AllAdcs)) return(kFALSE);
	Xtrig = (bits[0] != 0);
	return(kTRUE);
}

Bool_t TC2LSis3302::SetExternalTrigger(Bool_t & Xtrig) {
	TArrayI bits(1); bits[0] = Xtrig ? 1 : 0;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_SET_EXTERNAL_TRIGGER, bits, bits, kSis3302AllAdcs)) return(kFALSE);
	Xtrig = (bits[0] != 0);
	return(kTRUE);
}

Bool_t TC2LSis3302::GetExternalTimestampClear(Bool_t & TsClear) {
	TArrayI dataSend(0);
	TArrayI bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_GET_EXTERNAL_TIMESTAMP_CLEAR, dataSend, bits, kSis3302AllAdcs)) return(kFALSE);
	TsClear = (bits[0] != 0);
	return(kTRUE);
}

Bool_t TC2LSis3302::SetExternalTimestampClear(Bool_t & TsClear) {
	TArrayI bits(1); bits[0] = TsClear ? 1 : 0;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_SET_EXTERNAL_TIMESTAMP_CLEAR, bits, bits, kSis3302AllAdcs)) return(kFALSE);
	TsClear = (bits[0] != 0);
	return(kTRUE);
}

Bool_t TC2LSis3302::GetInternalTrigger(Bool_t & Itrig) {
	TArrayI dataSend(0);
	TArrayI bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_GET_INTERNAL_TRIGGER, dataSend, bits, kSis3302AllAdcs)) return(kFALSE);
	Itrig = (bits[0] != 0);
	return(kTRUE);
}

Bool_t TC2LSis3302::SetInternalTrigger(Bool_t & Itrig) {
	TArrayI bits(1); bits[0] = Itrig ? 1 : 0;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_SET_INTERNAL_TRIGGER, bits, bits, kSis3302AllAdcs)) return(kFALSE);
	Itrig = (bits[0] != 0);
	return(kTRUE);
}

Bool_t TC2LSis3302::GetClockSource(Int_t & ClockSource) {
	TArrayI dataSend(0);
	TArrayI clk;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_GET_INTERNAL_TRIGGER, dataSend, clk, kSis3302AllAdcs)) return(kFALSE);
	ClockSource = clk[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::SetClockSource(Int_t & ClockSource) {
	TArrayI clk(1); clk[0] = ClockSource;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_GET_INTERNAL_TRIGGER, clk, clk, kSis3302AllAdcs)) return(kFALSE);
	ClockSource = clk[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::SaveSettings(const Char_t * SettingsFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TC2LSis3302::SaveSettings
// Purpose:        Write settings to file
// Arguments:      Char_t * SettingsFile   -- settings file
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Write env data to file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fSettingsFile.IsNull()) fSettingsFile = Form("%sSettings.rc", this->GetName());

	if (SettingsFile == NULL || *SettingsFile == '\0') SettingsFile = fSettingsFile.Data();

	TString settingsFile = SettingsFile;

	ofstream settings(settingsFile.Data(), ios::out);
	if (!settings.good()) {
		gMrbLog->Err() << gSystem->GetError() << " - " << settingsFile << endl;
		gMrbLog->Flush(this->ClassName(), "SaveSettings");
		return(kFALSE);
	}

	TString tmplPath = gEnv->GetValue("TMrbConfig.TemplatePath", ".:config:$(MARABOU)/templates/config");
	TString tf = "Module_Sis_3302.rc.code";
	gSystem->ExpandPathName(tmplPath);

	const Char_t * fp = gSystem->Which(tmplPath.Data(), tf.Data());
	if (fp == NULL) {
		gMrbLog->Err()	<< "Template file not found -" << endl
						<<           "                 Searching on path " << tmplPath << endl
						<<           "                 for file          " << tf << endl;
		gMrbLog->Flush("SaveSettings");
		return(kFALSE);
	}
	
	tf = fp;
	
	TMrbLofNamedX tags;
	tags.AddNamedX(TC2LSis3302::kRcModuleSettings, "MODULE_SETTINGS");

	TMrbTemplate tmpl;
	TString line;
	if (tmpl.Open(tf.Data(), &tags)) {
		for (;;) {
			TMrbNamedX * tag = tmpl.Next(line);
			if (tmpl.IsEof()) break;
			if (tmpl.IsError()) continue;
			if (tmpl.Status() & TMrbTemplate::kNoTag) {
				if (line.Index("//-") != 0) settings << line << endl;
			} else {
				switch (tag->GetIndex()) {
					case TC2LSis3302::kRcModuleSettings:
					{
						TDatime d;
						Int_t boardId, majorVersion, minorVersion;
						tmpl.InitializeCode("%Preamble%");
						tmpl.Substitute("$author", gSystem->Getenv("USER"));
						tmpl.Substitute("$date", d.AsString());
						tmpl.Substitute("$moduleName", this->GetName());
						this->GetModuleInfo(boardId, majorVersion, minorVersion);
						tmpl.Substitute("$moduleFirmware", Form("%x%02x", majorVersion, minorVersion));
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%AcquisitionControl%");
						tmpl.Substitute("$moduleName", this->GetName());
						Int_t clockSource;
						this->GetClockSource(clockSource);
						tmpl.Substitute("$clockSource", clockSource);
						Bool_t xtrig, itrig;
						this->GetExternalTrigger(xtrig);
						tmpl.Substitute("$extTrig", xtrig ? "TRUE" : "FALSE");
						this->GetInternalTrigger(itrig);
						tmpl.Substitute("$intTrig", itrig ? "TRUE" : "FALSE");
						Bool_t tsClear;
						this->GetExternalTimestampClear(tsClear);
						tmpl.Substitute("$tsClear", tsClear ? "TRUE" : "FALSE");
						tmpl.WriteCode(settings);

						tmpl.InitializeCode("%DacSettings%");
						tmpl.WriteCode(settings);
						TArrayI dacValues(kSis3302NofAdcs);
						this->ReadDac(dacValues, kSis3302AllAdcs);
						for (Int_t i = 0; i < kSis3302NofAdcs; i++) {
							tmpl.InitializeCode("%DacSettingsLoop%");
							tmpl.Substitute("$moduleName", this->GetName());
							tmpl.Substitute("$adcNo", i);
							tmpl.Substitute("$dacValue", dacValues[i]);
							tmpl.WriteCode(settings);
						}

						tmpl.InitializeCode("%EventConfig%");
						tmpl.WriteCode(settings);
						Int_t chGrp = 12;
						for (Int_t i = 0; i < kSis3302NofAdcs; i += 2, chGrp += 22) {
							tmpl.InitializeCode("%EventConfigLoop%");
							tmpl.Substitute("$moduleName", this->GetName());
							tmpl.Substitute("$chGrp", chGrp);
							Int_t hdrBits;
							this->GetHeaderBits(hdrBits, i);
							tmpl.Substitute("$hdrBits",hdrBits, 16);
							Int_t trigMode;
							this->GetTriggerMode(trigMode, i);
							tmpl.Substitute("$trigMode",trigMode, 16);
							Bool_t invert;
							this->GetPolarity(invert, i);
							tmpl.Substitute("$invert",invert ? "TRUE" : "FALSE");
							tmpl.WriteCode(settings);
						}
					}
				}
			}
		}
	}
	settings.close();
	fSettingsFile = settingsFile;
	return(kTRUE);
}

//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TC2LSis3302.cxx
// Purpose:        MARaBOU client to connect to LynxOs/VME
// Description:    Implements class methods to connect to VME modules
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TC2LSis3302.cxx,v 1.16 2010-07-12 12:31:01 Rudolf.Lutter Exp $
// Date:           $Date: 2010-07-12 12:31:01 $
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
#include "TMrbResource.h"
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

	if (this->IsVerbose() || this->IsOffline()) {
		TMrbNamedX * f = this->FindFunction(Fcode);
		TString fn = f ? f->GetName() : "???";
		TString an = (AdcNo == kSis3302AllAdcs) ? "all" : Form("chn%d", AdcNo);
		cout << "[" << this->GetName() << ", " << an << "] Exec function - \""
					<< fn << "\" (0x" << setbase(16) << Fcode << setbase(10) << ")";
		for (Int_t i = 0; i < DataSend.GetSize(); i++) {
			if (i == 0) cout	<< ", data: "; else cout << ", ";
			cout	<< DataSend[i]
					<< "|0x" << setbase(16) << DataSend[i] << setbase(10);
		}
		cout << endl;
		if (this->IsOffline()) {
			DataRecv.Set(100);
			return(kTRUE);
		}
	}

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
	if (this->IsOffline()) return(kTRUE);
	M2L_VME_Exec_Function x;
	gMrbC2Lynx->InitMessage((M2L_MsgHdr *) &x, sizeof(M2L_VME_Exec_Function), kM2L_MESS_VME_EXEC_FUNCTION);
	x.fData.fData[0] = kSis3302AllAdcs;
	x.fXhdr.fHandle = this->GetHandle();
	x.fXhdr.fCode = kM2L_FCT_GET_MODULE_INFO;
	x.fData.fWc = 1;
	if (gMrbC2Lynx->Send((M2L_MsgHdr *) &x)) {
		M2L_VME_Return_Module_Info r;
		r.fHdr.fLength = sizeof(M2L_VME_Return_Module_Info) / sizeof(Int_t);
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

Bool_t TC2LSis3302::SetUserLED(Bool_t & OnFlag) {
	TArrayI ledOn(1); ledOn[0] = OnFlag ? 1 : 0;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_SET_USER_LED, ledOn, ledOn, kSis3302AllAdcs)) return(kFALSE);
	OnFlag = ledOn[0];
	return(kTRUE);
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
	return(this->ExecFunction(kM2L_FCT_SIS_3302_KEY_ADDR, keyData, keyData));
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

Bool_t TC2LSis3302::ReadEventExtendedConfig(Int_t & Bits, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_READ_EVENT_EXTENDED_CONFIG, dataSend, bits, AdcNo)) return(kFALSE);
	Bits = bits[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::WriteEventExtendedConfig(Int_t & Bits, Int_t AdcNo) {
	TArrayI bits(1); bits[0] = Bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_WRITE_EVENT_EXTENDED_CONFIG, bits, bits, AdcNo)) return(kFALSE);
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

Bool_t TC2LSis3302::GetGroupId(Int_t & GroupId, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_GET_GROUP_ID, dataSend, bits, AdcNo)) return(kFALSE);
	GroupId = bits[0];
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

Bool_t TC2LSis3302::SetGateMode(Int_t & Bits, Int_t AdcNo) {
	TArrayI bits(1); bits[0] = Bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_SET_GATE_MODE, bits, bits, AdcNo)) return(kFALSE);
	Bits = bits[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::GetGateMode(Int_t & Bits, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_GET_GATE_MODE, dataSend, bits, AdcNo)) return(kFALSE);
	Bits = bits[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::SetNextNeighborTriggerMode(Int_t & Bits, Int_t AdcNo) {
	TArrayI bits(1); bits[0] = Bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_SET_NNB_TRIGGER_MODE, bits, bits, AdcNo)) return(kFALSE);
	Bits = bits[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::GetNextNeighborTriggerMode(Int_t & Bits, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_GET_NNB_TRIGGER_MODE, dataSend, bits, AdcNo)) return(kFALSE);
	Bits = bits[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::SetNextNeighborGateMode(Int_t & Bits, Int_t AdcNo) {
	TArrayI bits(1); bits[0] = Bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_SET_NNB_GATE_MODE, bits, bits, AdcNo)) return(kFALSE);
	Bits = bits[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::GetNextNeighborGateMode(Int_t & Bits, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_GET_NNB_GATE_MODE, dataSend, bits, AdcNo)) return(kFALSE);
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
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_READ_TRIGGER_GATE_LENGTH, dataSend, gate, AdcNo)) return(kFALSE);
	Gate = gate[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::WriteTrigGateLength(Int_t & Gate, Int_t AdcNo) {
	TArrayI gate(1); gate[0] = Gate;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_WRITE_TRIGGER_GATE_LENGTH, gate, gate, AdcNo)) return(kFALSE);
	Gate = gate[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::ReadRawDataSampleLength(Int_t & Bits, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_READ_RAW_DATA_SAMPLE_LENGTH, dataSend, bits, AdcNo)) return(kFALSE);
	Bits = bits[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::WriteRawDataSampleLength(Int_t & Bits, Int_t AdcNo) {
	TArrayI bits(1); bits[0] = Bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_WRITE_RAW_DATA_SAMPLE_LENGTH, bits, bits, AdcNo)) return(kFALSE);
	Bits = bits[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::ReadRawDataStartIndex(Int_t & Bits, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_READ_RAW_DATA_START_INDEX, dataSend, bits, AdcNo)) return(kFALSE);
	Bits = bits[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::WriteRawDataStartIndex(Int_t & Bits, Int_t AdcNo) {
	TArrayI bits(1); bits[0] = Bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_WRITE_RAW_DATA_START_INDEX, bits, bits, AdcNo)) return(kFALSE);
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

Bool_t TC2LSis3302::ReadTrigPeakAndGap(Int_t & Peak, Int_t & Gap, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI peakgap;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_READ_TRIGGER_PEAK_AND_GAP, dataSend, peakgap, AdcNo)) return(kFALSE);
	Peak = peakgap[0];
	Gap = peakgap[1];
	return(kTRUE);
}

Bool_t TC2LSis3302::WriteTrigPeakAndGap(Int_t & Peak, Int_t & Gap, Int_t AdcNo) {
	TArrayI peakgap(2); peakgap[0] = Peak; peakgap[1] = Gap;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_WRITE_TRIGGER_PEAK_AND_GAP, peakgap, peakgap, AdcNo)) return(kFALSE);
	Peak = peakgap[0];
	Gap = peakgap[1];
	return(kTRUE);
}

Bool_t TC2LSis3302::ReadTrigPulseLength(Int_t & PulseLength, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI pulse;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_READ_TRIGGER_PULSE_LENGTH, dataSend, pulse, AdcNo)) return(kFALSE);
	PulseLength = pulse[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::WriteTrigPulseLength(Int_t & PulseLength, Int_t AdcNo) {
	TArrayI pulse(1); pulse[0] = PulseLength;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_WRITE_TRIGGER_PULSE_LENGTH, pulse, pulse, AdcNo)) return(kFALSE);
	PulseLength = pulse[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::ReadTrigInternalGate(Int_t & Gate, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI gate;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_READ_TRIGGER_INTERNAL_GATE, dataSend, gate, AdcNo)) return(kFALSE);
	Gate = gate[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::WriteTrigInternalGate(Int_t & Gate, Int_t AdcNo) {
	TArrayI gate(1); gate[0] = Gate;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_WRITE_TRIGGER_INTERNAL_GATE, gate, gate, AdcNo)) return(kFALSE);
	Gate = gate[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::ReadTrigInternalDelay(Int_t & Delay, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI delay;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_READ_TRIGGER_INTERNAL_DELAY, dataSend, delay, AdcNo)) return(kFALSE);
	Delay = delay[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::WriteTrigInternalDelay(Int_t & Delay, Int_t AdcNo) {
	TArrayI delay(1); delay[0] = Delay;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_WRITE_TRIGGER_INTERNAL_DELAY, delay, delay, AdcNo)) return(kFALSE);
	Delay = delay[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::GetTrigDecimation(Int_t & Decimation, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI decim;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_GET_TRIGGER_DECIMATION, dataSend, decim, AdcNo)) return(kFALSE);
	Decimation = decim[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::SetTrigDecimation(Int_t & Decimation, Int_t AdcNo) {
	TArrayI decim(1); decim[0] = Decimation;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_SET_TRIGGER_DECIMATION, decim, decim, AdcNo)) return(kFALSE);
	Decimation = decim[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::ReadTrigThreshold(Int_t & Thresh, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI thresh;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_READ_TRIGGER_THRESH, dataSend, thresh, AdcNo)) return(kFALSE);
	Thresh = thresh[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::WriteTrigThreshold(Int_t & Thresh, Int_t AdcNo) {
	TArrayI thresh(1); thresh[0] = Thresh;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_WRITE_TRIGGER_THRESH, thresh, thresh, AdcNo)) return(kFALSE);
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

Bool_t TC2LSis3302::GetEnergyDecimation(Int_t & Decimation, Int_t AdcNo) {
	TArrayI dataSend(0);
	TArrayI decim;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_GET_ENERGY_DECIMATION, dataSend, decim, AdcNo)) return(kFALSE);
	Decimation = decim[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::SetEnergyDecimation(Int_t & Decimation, Int_t AdcNo) {
	TArrayI decim(1); decim[0] = Decimation;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_SET_ENERGY_DECIMATION, decim, decim, AdcNo)) return(kFALSE);
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
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_WRITE_ENERGY_SAMPLE_LENGTH, sample, sample, AdcNo)) return(kFALSE);
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
	TArrayI idxVal(2); idxVal[0] = IdxNo;  idxVal[1] = IdxVal;
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

Bool_t TC2LSis3302::GetLemoInMode(Int_t & Bits) {
	TArrayI dataSend(0);
	TArrayI bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_GET_LEMO_IN_MODE, dataSend, bits, kSis3302AllAdcs)) return(kFALSE);
	Bits = bits[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::SetLemoInMode(Int_t & Bits) {
	TArrayI bits(1); bits[0] = Bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_SET_LEMO_IN_MODE, bits, bits, kSis3302AllAdcs)) return(kFALSE);
	Bits = bits[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::GetLemoOutMode(Int_t & Bits) {
	TArrayI dataSend(0);
	TArrayI bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_GET_LEMO_OUT_MODE, dataSend, bits, kSis3302AllAdcs)) return(kFALSE);
	Bits = bits[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::SetLemoOutMode(Int_t & Bits) {
	TArrayI bits(1); bits[0] = Bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_SET_LEMO_OUT_MODE, bits, bits, kSis3302AllAdcs)) return(kFALSE);
	Bits = bits[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::GetLemoInEnableMask(Int_t & Bits) {
	TArrayI dataSend(0);
	TArrayI bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_GET_LEMO_IN_ENABLE_MASK, dataSend, bits, kSis3302AllAdcs)) return(kFALSE);
	Bits = bits[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::SetLemoInEnableMask(Int_t & Bits) {
	TArrayI bits(1); bits[0] = Bits;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_SET_LEMO_IN_ENABLE_MASK, bits, bits, kSis3302AllAdcs)) return(kFALSE);
	Bits = bits[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::GetClockSource(Int_t & ClockSource) {
	TArrayI dataSend(0);
	TArrayI clk;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_GET_CLOCK_SOURCE, dataSend, clk, kSis3302AllAdcs)) return(kFALSE);
	ClockSource = clk[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::SetClockSource(Int_t & ClockSource) {
	TArrayI clk(1); clk[0] = ClockSource;
	if (!this->ExecFunction(kM2L_FCT_SIS_3302_SET_CLOCK_SOURCE, clk, clk, kSis3302AllAdcs)) return(kFALSE);
	ClockSource = clk[0];
	return(kTRUE);
}

Bool_t TC2LSis3302::StartTraceCollection(Int_t & NofEvents, Int_t AdcNo) {
	TArrayI dataSend(1);
	dataSend[0] = NofEvents;
	return(this->ExecFunction(kM2L_FCT_SIS_3302_START_TRACE_COLLECTION, dataSend, dataSend, AdcNo));
}

Bool_t TC2LSis3302::ContinueTraceCollection() {
	TArrayI dataSend(1);
	dataSend[0] = 0;
	return(this->ExecFunction(kM2L_FCT_SIS_3302_CONT_TRACE_COLLECTION, dataSend, dataSend, kSis3302AllAdcs));
}

Bool_t TC2LSis3302::StopTraceCollection() {
	TArrayI dataSend(1);
	dataSend[0] = 0;
	return(this->ExecFunction(kM2L_FCT_SIS_3302_START_TRACE_COLLECTION, dataSend, dataSend, kSis3302AllAdcs));
}

Bool_t TC2LSis3302::GetTraceLength(TArrayI & Data, Int_t AdcNo) {
	TArrayI dataSend(1);
	Data.Set(kSis3302EventPreHeader);
	return(this->ExecFunction(kM2L_FCT_SIS_3302_GET_TRACE_LENGTH, dataSend, Data, AdcNo));
}

Bool_t TC2LSis3302::GetTraceData(TArrayI & Data, Int_t & EventNo, Int_t AdcNo) {
	TArrayI dataSend(1);
	dataSend[0] = EventNo;
	return(this->ExecFunction(kM2L_FCT_SIS_3302_GET_TRACE_DATA, dataSend, Data, AdcNo));
}

Bool_t TC2LSis3302::DumpTrace() {
	TArrayI dataSend(1);
	TDatime now;
	dataSend[0] = 0;
	return(this->ExecFunction(kM2L_FCT_SIS_3302_DUMP_TRACE, dataSend, dataSend, kSis3302AllAdcs));
}

Bool_t TC2LSis3302::RestoreSettings(const Char_t * SettingsFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TC2LSis3302::RestoreSettings
// Purpose:        Read settings from file
// Arguments:      Char_t * SettingsFile   -- settings file
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Reads env data from file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fSettingsFile.IsNull()) fSettingsFile = Form("%sSettings.rc", this->GetName());

	if (SettingsFile == NULL || *SettingsFile == '\0') SettingsFile = fSettingsFile.Data();

	TString settingsFile = SettingsFile;
	gSystem->ExpandPathName(settingsFile);
	if (gSystem->AccessPathName(settingsFile.Data())) {
		gMrbLog->Err()	<< "Settings file not found - " << settingsFile << endl;
		gMrbLog->Flush("RestoreSettings");
		return(kFALSE);
	}

	TMrbResource * settings = new TMrbResource("SIS3302", settingsFile.Data());
	TString moduleName = settings->Get(".ModuleName", "");
	if (moduleName.IsNull()) {
		gMrbLog->Err()	<< "[" << settingsFile << "] Wrong format - module name missing" << endl;
		gMrbLog->Flush("RestoreSettings");
		return(kFALSE);
	}

	TString dotMod = Form(".%s", moduleName.Data());

	Int_t clockSource = settings->Get(dotMod.Data(), NULL, "ClockSource", 0);
	this->SetClockSource(clockSource);

	Int_t lemo = settings->Get(dotMod.Data(), NULL, "LemoOutMode", 0);
	this->SetLemoOutMode(lemo);
	lemo = settings->Get(dotMod.Data(), NULL, "LemoInMode", 0);
	this->SetLemoInMode(lemo);
	lemo = settings->Get(dotMod.Data(), NULL, "LemoInEnableMask", 0);
	this->SetLemoInEnableMask(lemo);

	TArrayI dacValues(kSis3302NofAdcs);
	for (Int_t i = 0; i < kSis3302NofAdcs; i++) {
		dacValues[i] = settings->Get(dotMod.Data(), "DacValue", Form("%d", i), 0);
	}
	this->WriteDac(dacValues, kSis3302AllAdcs);

	Int_t chGrp = 12;
	for (Int_t i = 0; i < kSis3302NofAdcs; i += 2, chGrp += 22) {
		Int_t hdrBits = settings->Get(dotMod.Data(), "HeaderBits", Form("%d", chGrp), 0);
		hdrBits &= 0x7ffc;
		this->SetHeaderBits(hdrBits, i);
	}

	for (Int_t i = 0; i < kSis3302NofAdcs; i++) {
		Int_t trigMode = settings->Get(dotMod.Data(), "TriggerMode", Form("%d", i), 0);
		this->SetTriggerMode(trigMode, i);
		Int_t gateMode = settings->Get(dotMod.Data(), "GateMode", Form("%d", i), 0);
		this->SetGateMode(gateMode, i);
		trigMode = settings->Get(dotMod.Data(), "NextNeighborTrigger", Form("%d", i), 0);
		this->SetNextNeighborTriggerMode(trigMode, i);
		gateMode = settings->Get(dotMod.Data(), "NextNeighborGate", Form("%d", i), 0);
		this->SetNextNeighborGateMode(gateMode, i);
		Bool_t invert = settings->Get(dotMod.Data(), "InvertSignal", Form("%d", i), kFALSE);
		this->SetPolarity(invert, i);
	}

	chGrp = 12;
	for (Int_t i = 0; i < kSis3302NofAdcs; i += 2, chGrp += 22) {
		Int_t delay = settings->Get(dotMod.Data(), "PretrigDelay", Form("%d", chGrp), 0);
		this->WritePreTrigDelay(delay, i);
		Int_t gate = settings->Get(dotMod.Data(), "TrigGateLength", Form("%d", chGrp), 0);
		this->WriteTrigGateLength(gate, i);
	}

	chGrp = 12;
	for (Int_t i = 0; i < kSis3302NofAdcs; i += 2, chGrp += 22) {
		Int_t length = settings->Get(dotMod.Data(), "RawDataSampleLength", Form("%d", chGrp), 0);
		this->WriteRawDataSampleLength(length, i);
		Int_t start = settings->Get(dotMod.Data(), "RawDataSampleStart", Form("%d", chGrp), 0);
		this->WriteRawDataStartIndex(start, i);
	}

	for (Int_t i = 0; i < kSis3302NofAdcs; i++) {
		Int_t peak = settings->Get(dotMod.Data(), "TrigPeakTime", Form("%d", i), 0);
		Int_t gap = settings->Get(dotMod.Data(), "TrigGapTime", Form("%d", i), 0);
		this->WriteTrigPeakAndGap(peak, gap, i);
		Int_t gate = settings->Get(dotMod.Data(), "TrigInternalGate", Form("%d", i), 0);
		this->WriteTrigInternalGate(gate, i);
		Int_t delay = settings->Get(dotMod.Data(), "TrigInternalDelay", Form("%d", i), 0);
		this->WriteTrigInternalDelay(delay, i);
		Int_t length = settings->Get(dotMod.Data(), "TrigPulseLength", Form("%d", i), 0);
		this->WriteTrigPulseLength(length, i);
		Int_t decim = settings->Get(dotMod.Data(), "TrigDecimation", Form("%d", i), 0);
		this->SetTrigDecimation(decim, i);
	}

	for (Int_t i = 0; i < kSis3302NofAdcs; i++) {
		Int_t thresh = settings->Get(dotMod.Data(), "TrigThresh", Form("%d", i), 0);
		this->WriteTrigThreshold(thresh, i);
		Bool_t gt = settings->Get(dotMod.Data(), "TrigGT", Form("%d", i), kFALSE);
		this->SetTriggerGT(gt, i);
		Bool_t out = settings->Get(dotMod.Data(), "TrigOut", Form("%d", i), kFALSE);
		this->SetTriggerOut(out, i);
	}

	chGrp = 12;
	for (Int_t i = 0; i < kSis3302NofAdcs; i += 2, chGrp += 22) {
		Int_t peak = settings->Get(dotMod.Data(), "EnergyPeakTime", Form("%d", chGrp), 0);
		Int_t gap = settings->Get(dotMod.Data(), "EnergyGapTime", Form("%d", chGrp), 0);
		this->WriteEnergyPeakAndGap(peak, gap, i);
		Int_t decim = settings->Get(dotMod.Data(), "EnergyDecimation", Form("%d", chGrp), 0);
		this->SetEnergyDecimation(decim, i);
	}

	chGrp = 12;
	for (Int_t i = 0; i < kSis3302NofAdcs; i += 2, chGrp += 22) {
		Int_t gate = settings->Get(dotMod.Data(), "EnergyGateLength", Form("%d", chGrp), 0);
		this->WriteEnergyGateLength(gate, i);
		Int_t test = settings->Get(dotMod.Data(), "EnergyTestBits", Form("%d", chGrp), 0);
		this->SetTestBits(test, i);
	}

	chGrp = 12;
	for (Int_t i = 0; i < kSis3302NofAdcs; i += 2, chGrp += 22) {
		Int_t length = settings->Get(dotMod.Data(), "EnergySampleLength", Form("%d", chGrp), 0);
		this->WriteEnergySampleLength(length, i);
		Int_t start = settings->Get(dotMod.Data(), "EnergySampleStart1", Form("%d", chGrp), 0);
		this->WriteStartIndex(start, 0, i);
		start = settings->Get(dotMod.Data(), "EnergySampleStart2", Form("%d", chGrp), 0);
		this->WriteStartIndex(start, 1, i);
		start = settings->Get(dotMod.Data(), "EnergySampleStart3", Form("%d", chGrp), 0);
		this->WriteStartIndex(start, 2, i);
	}

	for (Int_t i = 0; i < kSis3302NofAdcs; i++) {
		Int_t tau = settings->Get(dotMod.Data(), "EnergyTauFactor", Form("%d", i), 0);
		this->WriteTauFactor(tau, i);
	}

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

						tmpl.Substitute("$mcaMode", "FALSE");

						Int_t lemo;
						this->GetLemoInMode(lemo);
						tmpl.Substitute("$lemoIn", lemo);
						this->GetLemoOutMode(lemo);
						tmpl.Substitute("$lemoOut", lemo);
						this->GetLemoInEnableMask(lemo);
						tmpl.Substitute("$lemoEnableMask", lemo, 16);
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

						tmpl.InitializeCode("%EventHeader%");
						tmpl.WriteCode(settings);
						Int_t chGrp = 12;
						for (Int_t i = 0; i < kSis3302NofAdcs; i += 2, chGrp += 22) {
							tmpl.InitializeCode("%EventHeaderLoop%");
							tmpl.Substitute("$moduleName", this->GetName());
							tmpl.Substitute("$chGrp", chGrp);
							Int_t hdrBits;
							this->GetHeaderBits(hdrBits, i);
							tmpl.Substitute("$hdrBits", hdrBits, 16);
							tmpl.WriteCode(settings);
						}

						tmpl.InitializeCode("%EventTrigGateNextNeighbor%");
						tmpl.WriteCode(settings);
						for (Int_t i = 0; i < kSis3302NofAdcs; i++) {
							tmpl.InitializeCode("%EventTrigGateNNLoop%");
							tmpl.Substitute("$moduleName", this->GetName());
							tmpl.Substitute("$adcNo", i);
							Int_t trigMode;
							this->GetTriggerMode(trigMode, i);
							Char_t * tm;
							switch (trigMode) {
								case 0: tm = "NoTrig"; break;
								case 1: tm = "Int"; break;
								case 2: tm = "Ext"; break;
								case 3: tm = "Ext|Int"; break;
							}
							tmpl.Substitute("$trigMode", Form("%#x (%s)", trigMode, tm));
							Int_t gateMode;
							this->GetGateMode(gateMode, i);
							Char_t * gm;
							switch (gateMode) {
								case 0: gm = "NoGate"; break;
								case 1: gm = "Int"; break;
								case 2: gm = "Ext"; break;
								case 3: gm = "Ext|Int"; break;
							}
							tmpl.Substitute("$gateMode", Form("%#x (%s)", gateMode, gm));
							this->GetNextNeighborTriggerMode(trigMode, i);
							switch (trigMode) {
								case 0: tm = "NoTrig"; break;
								case 1: tm = "N+1"; break;
								case 2: tm = "N-1"; break;
								case 3: tm = "N+/-1"; break;
							}
							tmpl.Substitute("$nextNeighborTrigger", Form("%#x (%s)", trigMode, tm));
							this->GetNextNeighborGateMode(gateMode, i);
							switch (gateMode) {
								case 0: gm = "NoGate"; break;
								case 1: gm = "N+1"; break;
								case 2: gm = "N-1"; break;
								case 3: gm = "N+/-1"; break;
							}
							tmpl.Substitute("$nextNeighborGate", Form("%#x (%s)", gateMode, gm));
							Bool_t invert;
							this->GetPolarity(invert, i);
							tmpl.Substitute("$invert", invert ? "TRUE" : "FALSE");
							tmpl.WriteCode(settings);
						}

						tmpl.InitializeCode("%TrigDelayGate%");
						tmpl.WriteCode(settings);
						chGrp = 12;
						for (Int_t i = 0; i < kSis3302NofAdcs; i += 2, chGrp += 22) {
							tmpl.InitializeCode("%TrigDelayGateLoop%");
							tmpl.Substitute("$moduleName", this->GetName());
							tmpl.Substitute("$chGrp", chGrp);
							Int_t delay;
							this->ReadPreTrigDelay(delay, i);
							tmpl.Substitute("$delay", delay);
							Int_t gate;
							this->ReadTrigGateLength(gate, i);
							tmpl.Substitute("$gate", gate);
							tmpl.WriteCode(settings);
						}

						tmpl.InitializeCode("%RawDataSample%");
						tmpl.WriteCode(settings);
						chGrp = 12;
						for (Int_t i = 0; i < kSis3302NofAdcs; i += 2, chGrp += 22) {
							tmpl.InitializeCode("%RawDataSampleLoop%");
							tmpl.Substitute("$moduleName", this->GetName());
							tmpl.Substitute("$chGrp", chGrp);
							Int_t length;
							this->ReadRawDataSampleLength(length, i);
							tmpl.Substitute("$length", length);
							Int_t start;
							this->ReadRawDataStartIndex(start, i);
							tmpl.Substitute("$start", start);
							tmpl.WriteCode(settings);
						}

						tmpl.InitializeCode("%TriggerSetup%");
						tmpl.WriteCode(settings);
						for (Int_t i = 0; i < kSis3302NofAdcs; i++) {
							tmpl.InitializeCode("%TriggerSetupLoop%");
							tmpl.Substitute("$moduleName", this->GetName());
							tmpl.Substitute("$adc", i);
							Int_t peak, gap;
							this->ReadTrigPeakAndGap(peak, gap, i);
							tmpl.Substitute("$peakTime", peak);
							tmpl.Substitute("$gapTime", gap);
							Int_t gate;
							this->ReadTrigInternalGate(gate, i);
							tmpl.Substitute("$gateLength", gate);
							Int_t delay;
							this->ReadTrigInternalDelay(delay, i);
							tmpl.Substitute("$delay", delay);
							Int_t pulse;
							this->ReadTrigPulseLength(pulse, i);
							tmpl.Substitute("$pulseLength", pulse);
							Int_t decim;
							this->GetTrigDecimation(decim, i);
							char * dm;
							switch (decim) {
								case 0: dm = "NoDecim"; break;
								case 1: dm = "2-fold"; break;
								case 2: dm = "4-fold"; break;
								case 3: dm = "8-fold"; break;
							}
							tmpl.Substitute("$decim", Form("%d (%s)", decim, dm));
							tmpl.WriteCode(settings);
						}

						tmpl.InitializeCode("%TriggerThresh%");
						tmpl.WriteCode(settings);
						for (Int_t i = 0; i < kSis3302NofAdcs; i++) {
							tmpl.InitializeCode("%TriggerThreshLoop%");
							tmpl.Substitute("$moduleName", this->GetName());
							tmpl.Substitute("$adc", i);
							Int_t thresh;
							this->ReadTrigThreshold(thresh, i);
							tmpl.Substitute("$thresh", thresh);
							Bool_t gt, out;
							this->GetTriggerGT(gt, i);
							tmpl.Substitute("$gt", gt ? "TRUE" : "FALSE");
							this->GetTriggerOut(out, i);
							tmpl.Substitute("$out", out ? "TRUE" : "FALSE");
							tmpl.WriteCode(settings);
						}

						tmpl.InitializeCode("%EnergySetup%");
						tmpl.WriteCode(settings);
						chGrp = 12;
						for (Int_t i = 0; i < kSis3302NofAdcs; i += 2, chGrp += 22) {
							tmpl.InitializeCode("%EnergySetupLoop%");
							tmpl.Substitute("$moduleName", this->GetName());
							tmpl.Substitute("$chGrp", chGrp);
							Int_t peak, gap;
							this->ReadEnergyPeakAndGap(peak, gap, i);
							tmpl.Substitute("$peakTime", peak);
							tmpl.Substitute("$gapTime", gap);
							Int_t decim;
							this->GetEnergyDecimation(decim, i);
							char * dm;
							switch (decim) {
								case 0: dm = "NoDecim"; break;
								case 1: dm = "2-fold"; break;
								case 2: dm = "4-fold"; break;
								case 3: dm = "8-fold"; break;
							}
							tmpl.Substitute("$decim", Form("%d (%s)", decim, dm));
							tmpl.WriteCode(settings);
						}

						tmpl.InitializeCode("%EnergyGate%");
						tmpl.WriteCode(settings);
						chGrp = 12;
						for (Int_t i = 0; i < kSis3302NofAdcs; i += 2, chGrp += 22) {
							tmpl.InitializeCode("%EnergyGateLoop%");
							tmpl.Substitute("$moduleName", this->GetName());
							tmpl.Substitute("$chGrp", chGrp);
							Int_t gate;
							this->ReadEnergyGateLength(gate, i);
							tmpl.Substitute("$gate", gate);
							Int_t test;
							this->GetTestBits(test, i);
							tmpl.Substitute("$test", test);
							tmpl.WriteCode(settings);
						}

						tmpl.InitializeCode("%EnergySample%");
						tmpl.WriteCode(settings);
						chGrp = 12;
						for (Int_t i = 0; i < kSis3302NofAdcs; i += 2, chGrp += 22) {
							tmpl.InitializeCode("%EnergySampleLoop%");
							tmpl.Substitute("$moduleName", this->GetName());
							tmpl.Substitute("$chGrp", chGrp);
							Int_t length;
							this->ReadEnergySampleLength(length, i);
							tmpl.Substitute("$length", length);
							Int_t start;
							this->ReadStartIndex(start, 0, i);
							tmpl.Substitute("$start1", start);
							this->ReadStartIndex(start, 1, i);
							tmpl.Substitute("$start2", start);
							this->ReadStartIndex(start, 2, i);
							tmpl.Substitute("$start3", start);
							tmpl.WriteCode(settings);
						}

						tmpl.InitializeCode("%TauFactor%");
						tmpl.WriteCode(settings);
						for (Int_t i = 0; i < kSis3302NofAdcs; i++) {
							tmpl.InitializeCode("%TauFactorLoop%");
							tmpl.Substitute("$moduleName", this->GetName());
							tmpl.Substitute("$adc", i);
							Int_t tau;
							this->ReadTauFactor(tau, i);
							tmpl.Substitute("$tau", tau);
							tmpl.WriteCode(settings);
						}
					}
				}
			}
		}
	}
	settings.close();
	fSettingsFile = settingsFile;
	gMrbLog->Out()  << "[" << this->GetName() << "] Settings saved to file " << settingsFile << endl;
	gMrbLog->Flush(this->ClassName(), "SaveSettings", setblue);
	return(kTRUE);
}

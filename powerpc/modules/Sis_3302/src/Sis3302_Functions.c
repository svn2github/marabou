//__________________________________________________________[C IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
//! \file			Sis3302_Functions.c
//! \brief			Interface for SIS3302 ADCs
//! $Author: Rudolf.Lutter $
//! $Mail			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.3 $
//! $Date: 2010-04-22 13:44:41 $
//////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <allParam.h>
#include <ces/bmalib.h>
#include <errno.h>

#include "LwrTypes.h"

#include "Sis3302_Protos.h"
#include "Sis3302_Layout.h"
#include "Sis3302_Database.h"


#include "root_env.h"

#include "err_mask_def.h"
#include "errnum_def.h"

char msg[256];

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Allocate database
//! \param[in]		VmeAddr			-- vme address (mapped)
//! \param[in]		BaseAddr	 	-- base address (phys)
//! \param[in]		Name			-- module name
//! \param[in]		Serial			-- MARaBOU's serial number
//! \return 		Module			-- Pointer to struct s_sis_3302
//////////////////////////////////////////////////////////////////////////////

struct s_sis_3302 * sis_3302_alloc(ULong_t VmeAddr, volatile unsigned char * BaseAddr, char * Name, Int_t Serial)
{
	struct s_sis_3302 * s;
	s = (struct s_sis_3302 *) calloc(1, sizeof(struct s_sis_3302));
	if (s != NULL) {
		Module->baseAddr = base;
		Module->vmeAddr = vmeAddr;
		strcpy(Module->moduleName, name);
		Module->serial = serial;
		Module->verbose = 0;
		Module->segSize = 1000000;
		Module->lowerBound = 0;
		Module->upperBound = 0;
		Module->mappedAddr = 0;
		Module->addrMod = 0x9;
	} else {
		sprintf(msg, "[alloc] Can't allocate sis_3302 struct");
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
	}
	return(s);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Maps address to vme memory
//! \param[in]		Module			-- module database
//! \param[out]		Offset	 		-- register offset
//! \retval 		MappedAddr		--
//////////////////////////////////////////////////////////////////////////////

volatile char * (struct s_sis_3302 * Module, Int_t Offset) {

	struct pdparam_master s_param; 			// vme segment params
	UInt_t addr, low;

	s_param.iack = 1;
 	s_param.rdpref = 0;
 	s_param.wrpost = 0;
 	s_param.swap = SINGLE_AUTO_SWAP;
 	s_param.dum[0] = 0; 					// forces static mapping!

	Bool_t mapIt = false;
	if (Module->upperBound == 0) {
		mapIt = true;						// never done
	} else if (Offset < Module->lowerBound || Offset > Module->upperBound) {
		addr = return_controller(Module->mappedAddr, Module->segSize);
		if (addr == 0xFFFFFFFF) {
			sprintf(msg, "[mapAddress] Can't unmap log addr %#lx (seg size=%#lx, addr mod=%#x)", Module->mappedAddr, Module->addrMod);
			f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
			return(NULL);
		}
		mapIt = true;
	}

	if (mapIt) {
		low = (Offset / Module->segSize) * Module->segSize;
		addr = find_controller(Module->baseAddr + low, Module->segSize, Module->addrMod, 0, 0, &s_param);
		if (addr == 0xFFFFFFFF) {
			sprintf(msg, "[mapAddress] Can't map phys addr %#lx (size=%#lx, addr mod=%#x)", (Module->baseAddr + Offset), Module->segSize, Module->addrMod);
			f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
			return(NULL);
		}
		Module->mappedAddr = addr;
		Module->lowerBound = low;
		Module->upperBound = low + Module->segSize - 1;
	}
	return((volatile Char_t *) (Module->mappedAddr + (Offset - Module->lowerBound)));
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

boll_t sis3302_moduleInfo(struct s_sis_3302 * Module, Int_t * BoardId, Int_t * MajorVersion, Int_t * MinorVersion, Bool_t PrintFlag) {

	volatile ULong_t * firmWare;
	ULong_t ident;
	Int_t b;

	firmWare = (volatile ULong_t *) sis3302_mapAddress(Module, SIS3302_MODID);
	if (firmWare == NULL) return(false);

	ident = *firmWare;

	b = (ident >> 16) & 0xFFFF;
	BoardId = (b &0xF) + 10 * ((b >> 4) & 0xF) + 100 * ((b >> 8) & 0xF) + 1000 * ((b >> 12) & 0xF);
	MajorVersion = (ident >> 8) & 0xFF;
	MinorVersion = ident & 0xFF;
	if (PrintFlag) {
		sprintf(msg, "SIS module info: addr (phys) %#lx (log) %#lx mod %#x type %d version %x%0x", Module->baseAddr, Module->mappedAddr, Module->addrMod, BoardId, MajorVersion, MinorVersion);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
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

Bool_t sis3302_setUserLED(struct s_sis_3302 * Module, Bool_t OnFlag) {

	Int_t data;
	data = OnFlag ? 0x1 : 0x10000;
	if (!Module->WriteControlStatus(Module, data)) return(kFALSE);
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

Bool_t sis3302_readDac(struct s_sis_3302 * Module, Int_t DacValues[], Int_t AdcNo) {

	Int_t firstAdc;
	Int_t lastAdc;
	Int_t wc;
	Int_t adc;
	volatile Int_t * dacStatus;
	volatile Int_t * dacData;
	Int_t maxTimeout;
	Int_t idx;
	Int_t timeout;
	Int_t data;

	if (AdcNo != kSis3302AllAdcs && (AdcNo < 0 || AdcNo >= kSis3302NofAdcs)) {
		sprintf(msg, "[mapAddress] ADC number out of range - %d (should be in [0,%d]", AdcNo, (kSis3302NofAdcs - 1));
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		firstAdc = 0;
		lastAdc = kSis3302NofAdcs - 1;
	} else {
		firstAdc = AdcNo;
		lastAdc = AdcNo;
	}
	wc = lastAdc - firstAdc + 1;

	dacStatus = (volatile Int_t *) sis3302_mapAddress(SIS3302_DAC_CONTROL_STATUS);
	if (dacStatus == NULL) return(kFALSE);
	dacData = (volatile Int_t *) sis3302_mapAddress(SIS3302_DAC_DATA);
	if (dacData == NULL) return(kFALSE);

	maxTimeout = 5000 ;
	idx = 0;
	for (adc = firstAdc; adc <= lastAdc; adc++, idx++) {
		*dacStatus = kSis3302DacCmdLoadShiftReg + (adc << 4);
		timeout = 0 ;
		do {
			data = *dacStatus;
			timeout++;
		} while (((data & kSis3302DacBusy) == kSis3302DacBusy) && (timeout <  maxTimeout));
		if (timeout >=  maxTimeout) return(kFALSE);

		DacValues[idx] = (*dacData >> 16) & 0xFFFF;
		if (thiModule->CheckBusTrap(Module, SIS3302_DAC_DATA, "ReadDac")) return(kFALSE);
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
		gMrbLog->Flush(thiModule->ClassName(), "WriteDac");
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
		gMrbLog->Flush(thiModule->ClassName(), "WriteDac");
		return(kFALSE);
	}

	volatile Int_t * dacStatus = (volatile Int_t *) sis3302_mapAddress(SIS3302_DAC_CONTROL_STATUS);
	if (dacStatus == NULL) return(kFALSE);
	volatile Int_t * dacData = (volatile Int_t *) sis3302_mapAddress(SIS3302_DAC_DATA);
	if (dacData == NULL) return(kFALSE);

	Int_t maxTimeout = 5000;
	Int_t idx = 0;
	gSignalTrap = kFALSE;
	for (Int_t adc = firstAdc; adc <= lastAdc; adc++, idx++) {
		*dacData = DacValues[idx];
		if (thiModule->CheckBusTrap(Module, SIS3302_DAC_DATA, "WriteDac")) return(kFALSE);

		*dacStatus = kSis3302DacCmdLoadShiftReg + (adc << 4);
		if (thiModule->CheckBusTrap(Module, SIS3302_DAC_CONTROL_STATUS, "WriteDac")) return(kFALSE);
		Int_t timeout = 0 ;
		Int_t data;
		do {
			data = *dacStatus;
			if (thiModule->CheckBusTrap(Module, SIS3302_DAC_CONTROL_STATUS, "WriteDac")) return(kFALSE);
			timeout++;
		} while (((data & kSis3302DacBusy) == kSis3302DacBusy) && (timeout <  maxTimeout));
		if (timeout >=  maxTimeout) return(kFALSE);

		*dacStatus = kSis3302DacCmdLoad + (adc << 4);
		if (thiModule->CheckBusTrap(Module, SIS3302_DAC_CONTROL_STATUS, "WriteDac")) return(kFALSE);
		timeout = 0 ;
		do {
			data = *dacStatus;
			if (thiModule->CheckBusTrap(Module, SIS3302_DAC_CONTROL_STATUS, "WriteDac")) return(kFALSE);
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
			gMrbLog->Flush(thiModule->ClassName(), "KeyAddr");
			return(kFALSE);
	}

	volatile Int_t * keyAddr = (Int_t *) sis3302_mapAddress(offset);
	if (keyAddr == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*keyAddr = 0;
	if (Key == kSis3302KeyReset || Key == kSis3302KeyResetSample) sleep(1);
	if (thiModule->CheckBusTrap(Module, offset, "KeyAddr")) return(kFALSE);
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

	volatile Int_t * ctrlStat = (volatile Int_t *) sis3302_mapAddress(offset);
	if (ctrlStat == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Bits = *ctrlStat;
	return (!thiModule->CheckBusTrap(Module, offset, "ReadControlStatus"));
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

	volatile Int_t * ctrlStat = (volatile Int_t *) sis3302_mapAddress(offset);
	if (ctrlStat == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*ctrlStat = Bits;
	if (thiModule->CheckBusTrap(Module, offset, "WriteControlStatus")) return(kFALSE);
	return(thiModule->ReadControlStatus(Module, Bits));
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
		gMrbLog->Flush(thiModule->ClassName(), "ReadEventConfig");
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

	volatile Int_t * evtConf = (volatile Int_t *) sis3302_mapAddress(offset);
	if (evtConf == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Bits = *evtConf;
	return (!thiModule->CheckBusTrap(Module, offset, "ReadEventConfig"));
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
		gMrbLog->Flush(thiModule->ClassName(), "WriteEventConfig");
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

	volatile Int_t * evtConf = (volatile Int_t *) sis3302_mapAddress(offset);
	if (evtConf == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*evtConf = Bits;
	if (thiModule->CheckBusTrap(Module, offset, "WriteEventConfig")) return(kFALSE);
	return(thiModule->ReadEventConfig(Module, Bits, AdcNo == kSis3302AllAdcs ? 1 : AdcNo));
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
		gMrbLog->Flush(thiModule->ClassName(), "ReadEventExtendedConfig");
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

	volatile Int_t * evtConf = (volatile Int_t *) sis3302_mapAddress(offset);
	if (evtConf == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Bits = *evtConf;
	return (!thiModule->CheckBusTrap(Module, offset, "ReadEventExtendedConfig"));
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
		gMrbLog->Flush(thiModule->ClassName(), "WriteEventExtendedConfig");
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

	volatile Int_t * evtConf = (volatile Int_t *) sis3302_mapAddress(offset);
	if (evtConf == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*evtConf = Bits;
	if (thiModule->CheckBusTrap(Module, offset, "WriteEventExtendedConfig")) return(kFALSE);
	return(thiModule->ReadEventConfig(Module, Bits, AdcNo == kSis3302AllAdcs ? 1 : AdcNo));
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
		gMrbLog->Flush(thiModule->ClassName(), "GetHeaderBits");
		return(kFALSE);
	}

	Int_t bits;
	if (!thiModule->ReadEventConfig(Module, bits, AdcNo)) return(kFALSE);
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
		gMrbLog->Flush(thiModule->ClassName(), "SetHeaderBits");
		return(kFALSE);
	}

	if (Bits & 0x3) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Illegal header data - " << setbase(16) << Bits
						<< " (group id bits 0 & 1 are READ ONLY)" << endl;
		gMrbLog->Flush(thiModule->ClassName(), "SetHeaderBits");
		return(kFALSE);
	}

	Bits >>= 2;			// bits 1 & 2 are read-only

	if (AdcNo == kSis3302AllAdcs) {
		Int_t b;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			b = Bits;
			if (!thiModule->SetHeaderBits(Module, b, adc)) return(kFALSE);
		}
		Bits = b;
		return(kTRUE);
	}

	Int_t bits;
	if (!thiModule->ReadEventConfig(Module, bits, AdcNo)) return(kFALSE);
	bits &= 0xFFFF;
	bits |=	(Bits << 19);		// header bit #1 -> event config bit #17, bits 1 & 2 read-only
	if (!thiModule->WriteEventConfig(Module, bits, AdcNo)) return(kFALSE);
	return(thiModule->ReadEventConfig(Module, Bits, AdcNo == kSis3302AllAdcs ? 0 : AdcNo));
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
		gMrbLog->Flush(thiModule->ClassName(), "GetGroupId");
		return(kFALSE);
	}

	Int_t bits;
	if (!thiModule->ReadEventConfig(Module, bits, AdcNo)) return(kFALSE);
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
		gMrbLog->Flush(thiModule->ClassName(), "GetTriggerMode");
		return(kFALSE);
	}

	Int_t bits;
	if (!thiModule->ReadEventConfig(Module, bits, AdcNo)) return(kFALSE);
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
		gMrbLog->Flush(thiModule->ClassName(), "SetTriggerMode");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t b;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			b = Bits;
			if (!thiModule->SetTriggerMode(Module, b, adc)) return(kFALSE);
		}
		Bits = b;
		return(kTRUE);
	}

	Int_t bits;
	if (!thiModule->ReadEventConfig(Module, bits, AdcNo)) return(kFALSE);
	if (AdcNo & 1) {
		bits &= ~(kSis3302TriggerBoth << 10);
		bits |=	(Bits << 10);
	} else {
		bits &= ~(kSis3302TriggerBoth << 2);
		bits |=	(Bits << 2);
	}
	if (!thiModule->WriteEventConfig(Module, bits, AdcNo)) return(kFALSE);
	return(thiModule->ReadEventConfig(Module, Bits, AdcNo == kSis3302AllAdcs ? 0 : AdcNo));
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
		gMrbLog->Flush(thiModule->ClassName(), "GetGateMode");
		return(kFALSE);
	}

	Int_t bits;
	if (!thiModule->ReadEventConfig(Module, bits, AdcNo)) return(kFALSE);
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
		gMrbLog->Flush(thiModule->ClassName(), "SetGateMode");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t b;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			b = Bits;
			if (!thiModule->SetGateMode(Module, b, adc)) return(kFALSE);
		}
		Bits = b;
		return(kTRUE);
	}

	Int_t bits;
	if (!thiModule->ReadEventConfig(Module, bits, AdcNo)) return(kFALSE);
	if (AdcNo & 1) {
		bits &= ~(kSis3302GateBoth << 12);
		bits |=	(Bits << 12);
	} else {
		bits &= ~(kSis3302GateBoth << 4);
		bits |=	(Bits << 4);
	}
	if (!thiModule->WriteEventConfig(Module, bits, AdcNo)) return(kFALSE);
	return(thiModule->ReadEventConfig(Module, Bits, AdcNo == kSis3302AllAdcs ? 0 : AdcNo));
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
		gMrbLog->Flush(thiModule->ClassName(), "GetNextNeighborTriggerMode");
		return(kFALSE);
	}

	Int_t bits;
	if (!thiModule->ReadEventExtendedConfig(Module, bits, AdcNo)) return(kFALSE);
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
		gMrbLog->Flush(thiModule->ClassName(), "SetNextNeighborTriggerMode");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t b;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			b = Bits;
			if (!thiModule->SetNextNeighborTriggerMode(Module, b, adc)) return(kFALSE);
		}
		Bits = b;
		return(kTRUE);
	}

	Int_t bits;
	if (!thiModule->ReadEventExtendedConfig(Module, bits, AdcNo)) return(kFALSE);
	if (AdcNo & 1) {
		bits &= ~(kSis3302TriggerBoth << 10);
		bits |=	(Bits << 14);
	} else {
		bits &= ~(kSis3302TriggerBoth << 2);
		bits |=	(Bits << 6);
	}
	if (!thiModule->WriteEventExtendedConfig(Module, bits, AdcNo)) return(kFALSE);
	return(thiModule->ReadEventExtendedConfig(Module, Bits, AdcNo == kSis3302AllAdcs ? 0 : AdcNo));
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
		gMrbLog->Flush(thiModule->ClassName(), "GetNextNeighborGateMode");
		return(kFALSE);
	}

	Int_t bits;
	if (!thiModule->ReadEventConfig(Module, bits, AdcNo)) return(kFALSE);
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
		gMrbLog->Flush(thiModule->ClassName(), "SetNextNeighborGateMode");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t b;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			b = Bits;
			if (!thiModule->SetNextNeighborGateMode(Module, b, adc)) return(kFALSE);
		}
		Bits = b;
		return(kTRUE);
	}

	Int_t bits;
	if (!thiModule->ReadEventConfig(Module, bits, AdcNo)) return(kFALSE);
	if (AdcNo & 1) {
		bits &= ~(kSis3302GateBoth << 14);
		bits |=	(Bits << 14);
	} else {
		bits &= ~(kSis3302GateBoth << 6);
		bits |=	(Bits << 6);
	}
	if (!thiModule->WriteEventConfig(Module, bits, AdcNo)) return(kFALSE);
	return(thiModule->ReadEventConfig(Module, Bits, AdcNo == kSis3302AllAdcs ? 0 : AdcNo));
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
		gMrbLog->Flush(thiModule->ClassName(), "GetPolarity");
		return(kFALSE);
	}

	Int_t bits;
	if (!thiModule->ReadEventConfig(Module, bits, AdcNo)) return(kFALSE);
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
			if (!thiModule->SetPolarity(Module, ifl, adc)) return(kFALSE);
		}
		InvertFlag = ifl;
		return(kTRUE);
	}

	Int_t data;
	if (!thiModule->ReadEventConfig(Module, data, AdcNo)) return(kFALSE);
	if (AdcNo & 1) {
		if (InvertFlag) data |= (kSis3302PolarityNegative << 8); else data &= ~(kSis3302PolarityNegative << 8);
	} else {
		data &= ~1;
		if (InvertFlag) data |= kSis3302PolarityNegative; else data &= ~kSis3302PolarityNegative;
	}
	if (!thiModule->WriteEventConfig(Module, data, AdcNo)) return(kFALSE);

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
		gMrbLog->Flush(thiModule->ClassName(), "ReadEndAddrThresh");
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

	volatile Int_t * endAddr = (volatile Int_t *) sis3302_mapAddress(offset);
	if (endAddr == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Thresh = *endAddr;
	return (!thiModule->CheckBusTrap(Module, offset, "ReadEndAddrThresh"));
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
		gMrbLog->Flush(thiModule->ClassName(), "WriteEndAddrThresh");
		return(kFALSE);
	}

	if (Thresh > kSis3302EndAddrThreshMax) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Threshold out of range - max "
						<< setbase(16) << kSis3302EndAddrThreshMax << setbase(10) << endl;
		gMrbLog->Flush(thiModule->ClassName(), "WriteEndAddrThresh");
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

	volatile Int_t * endAddr = (volatile Int_t *) sis3302_mapAddress(offset);
	if (endAddr == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*endAddr = Thresh;
	if (thiModule->CheckBusTrap(Module, offset, "WriteEndAddrThresh")) return(kFALSE);
	return(thiModule->ReadEndAddrThresh(Module, Thresh, AdcNo == kSis3302AllAdcs ? 0 : AdcNo));
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
		gMrbLog->Flush(thiModule->ClassName(), "ReadPreTrigDelayAndGateLength");
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

	volatile Int_t * trigReg = (volatile Int_t *) sis3302_mapAddress(offset);
	if (trigReg == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Bits = *trigReg;
	return (!thiModule->CheckBusTrap(Module, offset, "ReadPreTrigDelayAndGateLength"));
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
		gMrbLog->Flush(thiModule->ClassName(), "WritePreTrigDelayAndGateLength");
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

	volatile Int_t * trigReg = (volatile Int_t *) sis3302_mapAddress(offset);
	if (trigReg == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*trigReg = Bits;
	if (thiModule->CheckBusTrap(Module, offset, "WritePreTrigDelayAndGateLength")) return(kFALSE);
	return(thiModule->ReadPreTrigDelayAndGateLength(Module, Bits, AdcNo == kSis3302AllAdcs ? 1 : AdcNo));
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
		gMrbLog->Flush(thiModule->ClassName(), "ReadPreTrigDelay");
		return(kFALSE);
	}

	if (!thiModule->ReadPreTrigDelayAndGateLength(Module, Delay, AdcNo)) return(kFALSE);
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
		gMrbLog->Flush(thiModule->ClassName(), "WritePreTrigDelay");
		return(kFALSE);
	}

	Int_t delay;
	if (AdcNo == kSis3302AllAdcs) {
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			delay = Delay;
			if (!thiModule->WritePreTrigDelay(Module, delay, adc)) return(kFALSE);
		}
		Delay = delay;
		return(kTRUE);
	}

	if (!thiModule->ReadPreTrigDelayAndGateLength(Module, delay, AdcNo)) return(kFALSE);

	Delay += 2;
	if (Delay >= 1024) Delay -= 1024;

	delay &= 0xFFFF;
	delay |= Delay << 16;
	Delay = delay;
	if (!thiModule->WritePreTrigDelayAndGateLength(Module, Delay, AdcNo)) return(kFALSE);
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
		gMrbLog->Flush(thiModule->ClassName(), "ReadTrigGateLength");
		return(kFALSE);
	}

	if (!thiModule->ReadPreTrigDelayAndGateLength(Module, Gate, AdcNo)) return(kFALSE);
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
		gMrbLog->Flush(thiModule->ClassName(), "WriteTrigGateLength");
		return(kFALSE);
	}

	Int_t gate;
	if (AdcNo == kSis3302AllAdcs) {
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			gate = Gate;
			if (!thiModule->WriteTrigGateLength(Module, gate, adc)) return(kFALSE);
		}
		Gate = gate;
		return(kTRUE);
	}

	if (!thiModule->ReadPreTrigDelayAndGateLength(Module, gate, AdcNo)) return(kFALSE);
	gate &= 0xFFFF0000;
	gate |= Gate - 1;
	Gate = gate;
	return(thiModule->WritePreTrigDelayAndGateLength(Module, Gate, AdcNo));
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
		gMrbLog->Flush(thiModule->ClassName(), "ReadRawDataBufConfig");
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

	volatile Int_t * rawData = (volatile Int_t *) sis3302_mapAddress(offset);
	if (rawData == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Bits = *rawData;
	return (!thiModule->CheckBusTrap(Module, offset, "ReadRawDataBufConfig"));
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
		gMrbLog->Flush(thiModule->ClassName(), "WriteRawDataBufConfig");
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

	volatile Int_t * rawData = (volatile Int_t *) sis3302_mapAddress(offset);
	if (rawData == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*rawData = Bits;
	if (thiModule->CheckBusTrap(Module, offset, "WriteRawDataBufConfig")) return(kFALSE);
	return(thiModule->ReadRawDataBufConfig(Module, Bits, AdcNo == kSis3302AllAdcs ? 1 : AdcNo));
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
		gMrbLog->Flush(thiModule->ClassName(), "ReadRawDataSampleLength");
		return(kFALSE);
	}

	Int_t bits;
	if (!thiModule->ReadRawDataBufConfig(Module, bits, AdcNo)) return(kFALSE);
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

	if (SampleLength < kSis3302RawDataSampleLengthMin || SampleLength > kSis3302RawDataSampleLengthMax) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Sample length out of range - "
						<< SampleLength << " (should be in [0," << kSis3302RawDataSampleLengthMax << "])" << endl;
		gMrbLog->Flush(thiModule->ClassName(), "WriteRawDataSampleLength");
		return(kFALSE);
	}

	if (SampleLength % 4) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Wrong sample alignment - "
						<< SampleLength << " (should be mod 4)" << endl;
		gMrbLog->Flush(thiModule->ClassName(), "WriteRawDataSampleLength");
		return(kFALSE);
	}

	Int_t sl;
	if (AdcNo == kSis3302AllAdcs) {
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			sl = SampleLength;
			if (!thiModule->WriteRawDataSampleLength(Module, sl, adc)) return(kFALSE);
		}
		SampleLength = sl;
		return(kTRUE);
	}

	if (!thiModule->ReadRawDataBufConfig(Module, sl, AdcNo)) return(kFALSE);
	sl &= 0x0000FFFF;
	sl |= (SampleLength << 16);
	SampleLength = sl;
	return(thiModule->WriteRawDataBufConfig(Module, SampleLength, AdcNo));
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
		gMrbLog->Flush(thiModule->ClassName(), "ReadRawDataStartIndex");
		return(kFALSE);
	}

	Int_t bits;
	if (!thiModule->ReadRawDataBufConfig(Module, bits, AdcNo)) return(kFALSE);
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
		gMrbLog->Flush(thiModule->ClassName(), "WriteRawDataStartIndex");
		return(kFALSE);
	}

	if (StartIndex & 0x1) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Wrong index alignment - "
						<< StartIndex << " (should be even)" << endl;
		gMrbLog->Flush(thiModule->ClassName(), "WriteRawDataStartIndex");
		return(kFALSE);
	}

	Int_t sx;
	if (AdcNo == kSis3302AllAdcs) {
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			sx = StartIndex;
			if (!thiModule->WriteRawDataStartIndex(Module, sx, adc)) return(kFALSE);
		}
		StartIndex = sx;
		return(kTRUE);
	}

	if (!thiModule->ReadRawDataBufConfig(Module, sx, AdcNo)) return(kFALSE);
	sx &= 0xFFFF0000;
	sx |= StartIndex;
	StartIndex = sx;
	return(thiModule->WriteRawDataBufConfig(Module, StartIndex, AdcNo));
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
		gMrbLog->Flush(thiModule->ClassName(), "ReadNextSampleAddr");
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

	volatile Int_t * samplAddr = (volatile Int_t *) sis3302_mapAddress(offset);
	if (samplAddr == NULL) return(kFALSE);
	gSignalTrap = kFALSE;
	Addr = *samplAddr;
	return (!thiModule->CheckBusTrap(Module, offset, "ReadNextSampleAddr"));
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
		gMrbLog->Flush(thiModule->ClassName(), "ReadPrevBankSampleAddr");
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

	volatile Int_t * samplAddr = (volatile Int_t *) sis3302_mapAddress(offset);
	if (samplAddr == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Addr = *samplAddr;
	return (!thiModule->CheckBusTrap(Module, offset, "ReadPrevBankSampleAddr"));
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
		gMrbLog->Flush(thiModule->ClassName(), "ReadActualSample");
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

	volatile Int_t * actSample = (volatile Int_t *) sis3302_mapAddress(offset);
	if (actSample == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Data = *actSample;
	if ((AdcNo & 1) == 0) Data >>= 16;
	return(!thiModule->CheckBusTrap(Module, offset, "ReadActualSample"));
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
		gMrbLog->Flush(thiModule->ClassName(), "ReadTriggerSetup");
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

	volatile Int_t * trigSetup = (volatile Int_t *) sis3302_mapAddress(offset);
	if (trigSetup == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Data = *trigSetup;
	return (!thiModule->CheckBusTrap(Module, offset, "ReadTriggerSetup"));
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
		gMrbLog->Flush(thiModule->ClassName(), "WriteTriggerSetup");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t data;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			data = Data;
			if (!thiModule->WriteTriggerSetup(Module, data, adc)) return(kFALSE);
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

	volatile Int_t * trigSetup = (volatile Int_t *) sis3302_mapAddress(offset);
	if (trigSetup == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*trigSetup = Data;
	if (thiModule->CheckBusTrap(Module, offset, "WriteTriggerSetup")) return(kFALSE);
	return(thiModule->ReadTriggerSetup(Module, Data, AdcNo));
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
		gMrbLog->Flush(thiModule->ClassName(), "ReadTriggerExtendedSetup");
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

	volatile Int_t * trigSetup = (volatile Int_t *) sis3302_mapAddress(offset);
	if (trigSetup == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Data = *trigSetup;
	return (!thiModule->CheckBusTrap(Module, offset, "ReadTriggerExtendedSetup"));
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
		gMrbLog->Flush(thiModule->ClassName(), "WriteTriggerExtendedSetup");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t data;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			data = Data;
			if (!thiModule->WriteTriggerExtendedSetup(Module, data, adc)) return(kFALSE);
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

	volatile Int_t * trigSetup = (volatile Int_t *) sis3302_mapAddress(offset);
	if (trigSetup == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*trigSetup = Data;
	if (thiModule->CheckBusTrap(Module, offset, "WriteTriggerExtendedSetup")) return(kFALSE);
	return(thiModule->ReadTriggerExtendedSetup(Module, Data, AdcNo));
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
	if (!thiModule->ReadTriggerSetup(Module, data, AdcNo)) return(kFALSE);
	if (!thiModule->ReadTriggerExtendedSetup(Module, xdata, AdcNo)) return(kFALSE);
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

	Int_t sumG = Peak + Gap;
	if (sumG > kSis3302TrigSumGMax || Peak < kSis3302TrigPeakMin || Peak > kSis3302TrigPeakMax || Gap < kSis3302TrigGapMin || Gap > kSis3302TrigGapMax) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Trigger peak time / gap time mismatch - "
						<< Peak << " ... " << Gap
						<< " (peak should be in [" << kSis3302TrigPeakMin << "," << kSis3302TrigPeakMax << "], gap should be in [" << kSis3302TrigGapMin << "," << kSis3302TrigGapMax << "], sumG=p+g <= " << kSis3302TrigSumGMax << endl;
		gMrbLog->Flush(thiModule->ClassName(), "WriteTriggerPeakAndGap");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t g, p;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			g = Gap; p = Peak;
			if (!thiModule->WriteTriggerPeakAndGap(Module, p, g, adc)) return(kFALSE);
		}
		Peak = p; Gap = g;
		return(kTRUE);
	}

	Int_t data, xdata;
	if (!thiModule->ReadTriggerSetup(Module, data, AdcNo)) return(kFALSE);
	if (!thiModule->ReadTriggerExtendedSetup(Module, xdata, AdcNo)) return(kFALSE);
	data &= 0xFFFF0000;
	data |= (sumG & 0xFF) << 8;
	data |= Peak & 0xFF;
	xdata &= 0xFFFF0000;
	xdata |= sumG & 0x300;
	xdata |= (Peak & 0x300) >> 8;
	if (!thiModule->WriteTriggerSetup(Module, data, AdcNo)) return(kFALSE);
	if (!thiModule->WriteTriggerExtendedSetup(Module, xdata, AdcNo)) return(kFALSE);
	Peak = data;		// we return contents of trigger setup register via 'Peak'
	Gap = xdata;		// ... and contents of extended trigger setup register via 'Gap'
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
	if (!thiModule->ReadTriggerSetup(Module, data, AdcNo)) return(kFALSE);
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
		gMrbLog->Flush(thiModule->ClassName(), "WriteTriggerPulseLength");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t pl;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			pl = PulseLength;
			if (!thiModule->WriteTriggerPulseLength(Module, pl, adc)) return(kFALSE);
		}
		PulseLength = pl;
		return(kTRUE);
	}

	Int_t data;
	if (!thiModule->ReadTriggerSetup(Module, data, AdcNo)) return(kFALSE);
	data &= 0xFF00FFFF;
	data |= ((PulseLength & 0xFF) << 16);
	if (!thiModule->WriteTriggerSetup(Module, data, AdcNo)) return(kFALSE);
	PulseLength = data;		// we return contents of trigger setup register via 'PulseLength'
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
	if (!thiModule->ReadTriggerSetup(Module, data, AdcNo)) return(kFALSE);
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
		gMrbLog->Flush(thiModule->ClassName(), "WriteTriggerInternalGate");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t gl;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			gl = Gate;
			if (!thiModule->WriteTriggerInternalGate(Module,gl, adc)) return(kFALSE);
		}
		Gate = gl;
		return(kTRUE);
	}

	Int_t data;
	if (!thiModule->ReadTriggerSetup(Module, data, AdcNo)) return(kFALSE);
	data &= 0xFFFFFF;
	data |= (Gate << 24);
	if (!thiModule->WriteTriggerSetup(Module, data, AdcNo)) return(kFALSE);
	Gate = data;		// we return contents of trigger setup register via 'Gate'
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
	if (!thiModule->ReadTriggerExtendedSetup(Module, data, AdcNo)) return(kFALSE);
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
		gMrbLog->Flush(thiModule->ClassName(), "WriteTriggerInternalDelay");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t dl;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			dl = Delay;
			if (!thiModule->WriteTriggerInternalDelay(Module, dl, adc)) return(kFALSE);
		}
		Delay = dl;
		return(kTRUE);
	}

	Int_t data;
	if (!thiModule->ReadTriggerExtendedSetup(Module, data, AdcNo)) return(kFALSE);
	data &= 0xFFFFFF;
	data |= (Delay << 24);
	if (!thiModule->WriteTriggerExtendedSetup(Module, data, AdcNo)) return(kFALSE);
	Delay = data;		// we return contents of trigger setup register via 'Delay'
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
	if (!thiModule->ReadTriggerExtendedSetup(Module, data, AdcNo)) return(kFALSE);
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
		gMrbLog->Flush(thiModule->ClassName(), "SetTriggerDecimation");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t dc;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			dc = Decimation;
			if (!thiModule->SetTriggerDecimation(Module, dc, adc)) return(kFALSE);
		}
		Decimation = dc;
		return(kTRUE);
	}

	Int_t data;
	if (!thiModule->ReadTriggerExtendedSetup(Module, data, AdcNo)) return(kFALSE);
	data &= ~(0x3 << 16);
	data |= (Decimation << 16);
	if (!thiModule->WriteTriggerExtendedSetup(Module, data, AdcNo)) return(kFALSE);
	Decimation = data;		// we return contents of energy setup register via 'Decimation'
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
		gMrbLog->Flush(thiModule->ClassName(), "ReadTriggerThreshReg");
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

	volatile Int_t * trigThresh = (volatile Int_t *) sis3302_mapAddress(offset);
	if (trigThresh == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Data = *trigThresh;
	return (!thiModule->CheckBusTrap(Module, offset, "ReadTriggerThreshReg"));
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
		gMrbLog->Flush(thiModule->ClassName(), "WriteTriggerThreshReg");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t data;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			data = Data;
			if (!thiModule->WriteTriggerThreshReg(Module, data, adc)) return(kFALSE);
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

	volatile Int_t * trigThresh = (volatile Int_t *) sis3302_mapAddress(offset);
	if (trigThresh == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*trigThresh = Data;
	if (thiModule->CheckBusTrap(Module, offset, "WriteTriggerThreshReg")) return(kFALSE);
	return(thiModule->ReadTriggerThreshReg(Module, Data, AdcNo));
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
	if (!thiModule->ReadTriggerThreshReg(Module, data, AdcNo)) return(kFALSE);
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
		gMrbLog->Flush(thiModule->ClassName(), "WriteTriggerThreshold");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t th;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			th = Thresh;
			if (!thiModule->WriteTriggerThreshold(Module, th, adc)) return(kFALSE);
		}
		Thresh = th;
		return(kTRUE);
	}

	Int_t data;
	if (!thiModule->ReadTriggerThreshReg(Module, data, AdcNo)) return(kFALSE);
	data &= 0xFFFF0000;
	data |= Thresh;
	if (!thiModule->WriteTriggerThreshReg(Module, data, AdcNo)) return(kFALSE);
	Thresh = data;		// we return contents of trigger threshold register via 'Thresh'
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
	if (!thiModule->ReadTriggerThreshReg(Module, data, AdcNo)) return(kFALSE);
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
			if (!thiModule->SetTriggerGT(Module, fl, adc)) return(kFALSE);
		}
		Flag = fl;
		return(kTRUE);
	}

	Int_t data;
	if (!thiModule->ReadTriggerThreshReg(Module, data, AdcNo)) return(kFALSE);
	data &= ~(0x1 << 25);
	if (Flag) data |= (0x1 << 25);
	if (!thiModule->WriteTriggerThreshReg(Module, data, AdcNo)) return(kFALSE);
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
	if (!thiModule->ReadTriggerThreshReg(Module, data, AdcNo)) return(kFALSE);
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
			if (!thiModule->SetTriggerOut(Module, fl, adc)) return(kFALSE);
		}
		Flag = fl;
		return(kTRUE);
	}

	Int_t data;
	if (!thiModule->ReadTriggerThreshReg(Module, data, AdcNo)) return(kFALSE);
	data &= ~(0x1 << 26);
	if (Flag) data |= (0x1 << 26);
	if (!thiModule->WriteTriggerThreshReg(Module, data, AdcNo)) return(kFALSE);
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
		gMrbLog->Flush(thiModule->ClassName(), "ReadEnergySetup");
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

	volatile Int_t * setup = (volatile Int_t *) sis3302_mapAddress(offset);
	if (setup == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Data = *setup;
	return (!thiModule->CheckBusTrap(Module, offset, "ReadEnergySetup"));
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
		gMrbLog->Flush(thiModule->ClassName(), "WriteEnergySetup");
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

	volatile Int_t * setup = (volatile Int_t *) sis3302_mapAddress(offset);
	if (setup == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*setup = Data;
	if (thiModule->CheckBusTrap(Module, offset, "WriteEnergySetup")) return(kFALSE);
	return(thiModule->ReadEnergySetup(Module, Data, (AdcNo == kSis3302AllAdcs) ? 1 : AdcNo));
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
	if (!thiModule->ReadEnergySetup(Module, data, AdcNo)) return(kFALSE);
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

	if (Peak < kSis3302EnergyPeakMin || Peak > kSis3302EnergyPeakMax) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Energy peak time mismatch - "
						<< Peak << " (should be in [0," << kSis3302EnergyPeakMax << "])" << endl;
		gMrbLog->Flush(thiModule->ClassName(), "WriteEnergyPeakAndGap");
		return(kFALSE);
	}
	if (Gap < kSis3302EnergyGapMin || Gap > kSis3302EnergyGapMax) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Energy gap time mismatch - "
						<< Gap << " (should be in [0," << kSis3302EnergyGapMax << "])" << endl;
		gMrbLog->Flush(thiModule->ClassName(), "WriteEnergyPeakAndGap");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t p, g;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			p = Peak; g = Gap;
			if (!thiModule->WriteEnergyPeakAndGap(Module, p, g, adc)) return(kFALSE);
		}
		Peak = p; Gap = g;
		return(kTRUE);
	}

	Int_t data;
	if (!thiModule->ReadEnergySetup(Module, data, AdcNo)) return(kFALSE);
	data &= 0xFFFC0000;
	data |= (Gap << 8);
	Int_t p = Peak & 0xFF;
	data |= Peak;
	p = (Peak >> 8) & 0x3;
	data |= (p << 16);
	if (!thiModule->WriteEnergySetup(Module, data, AdcNo)) return(kFALSE);
	Peak = data;		// we return contents of energy setup register via 'Peak'
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
	if (!thiModule->ReadEnergySetup(Module, data, AdcNo)) return(kFALSE);
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
		gMrbLog->Flush(thiModule->ClassName(), "SetEnergyDecimation");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t dc;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			dc = Decimation;
			if (!thiModule->SetEnergyDecimation(Module, dc, adc)) return(kFALSE);
		}
		Decimation = dc;
		return(kTRUE);
	}

	Int_t data;
	if (!thiModule->ReadEnergySetup(Module, data, AdcNo)) return(kFALSE);
	data &= ~(0x3 << 28);
	data |= (Decimation << 28);
	if (!thiModule->WriteEnergySetup(Module, data, AdcNo)) return(kFALSE);
	Decimation = data;		// we return contents of energy setup register via 'Decimation'
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
		gMrbLog->Flush(thiModule->ClassName(), "ReadEnergyGateReg");
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

	volatile Int_t * gateReg = (volatile Int_t *) sis3302_mapAddress(offset);
	if (gateReg == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Data = *gateReg;
	return (!thiModule->CheckBusTrap(Module, offset, "ReadEnergyGateReg"));
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
		gMrbLog->Flush(thiModule->ClassName(), "WriteEnergyGateReg");
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

	volatile Int_t * gateReg = (volatile Int_t *) sis3302_mapAddress(offset);
	if (gateReg == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*gateReg = Data;
	if (thiModule->CheckBusTrap(Module, offset, "WriteEnergyGateReg")) return(kFALSE);
	return(thiModule->ReadEnergyGateReg(Module, Data, (AdcNo == kSis3302AllAdcs) ? 1 : AdcNo));
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
	if (!thiModule->ReadEnergyGateReg(Module, data, AdcNo)) return(kFALSE);
	GateLength = data & 0xFFF;
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

	if (GateLength < 0 || GateLength > 0xFFF) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Wrong energy gate length - " << GateLength
						<< " (should be in [0," << 0xFFF << "]" << endl;
		gMrbLog->Flush(thiModule->ClassName(), "WriteEnergyGateLength");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t gl;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			gl = GateLength;
			if (!thiModule->WriteEnergyGateLength(Module, gl, adc)) return(kFALSE);
		}
		GateLength = gl;
		return(kTRUE);
	}

	Int_t data;
	if (!thiModule->ReadEnergyGateReg(Module, data, AdcNo)) return(kFALSE);
	data &= 0xFFFFF000;
	data |= GateLength;
	if (!thiModule->WriteEnergyGateReg(Module, data, AdcNo)) return(kFALSE);
	GateLength = data;		// we return contents of energy gate register via 'GateLength'
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
	if (!thiModule->ReadEnergyGateReg(Module, data, AdcNo)) return(kFALSE);
	Bits = (data > 12) & 0x3;
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
		gMrbLog->Flush(thiModule->ClassName(), "SetTestBits");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t b;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			b = Bits;
			if (!thiModule->SetTestBits(Module, b, adc)) return(kFALSE);
		}
		Bits = b;
		return(kTRUE);
	}

	Int_t data;
	if (!thiModule->ReadEnergyGateReg(Module, data, AdcNo)) return(kFALSE);
	data &= 0xFFFFBFFF;
	data |= (Bits << 12);
	if (!thiModule->WriteEnergyGateReg(Module, data, AdcNo)) return(kFALSE);
	Bits = data;		// we return contents of energy gate register via 'Bits'
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
		gMrbLog->Flush(thiModule->ClassName(), "ReadEnergySampleLength");
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

	volatile Int_t * sample = (volatile Int_t *) sis3302_mapAddress(offset);
	if (sample == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	SampleLength = *sample;
	return (!thiModule->CheckBusTrap(Module, offset, "ReadEnergySampleLength"));
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

	if (AdcNo != kSis3302AllAdcs && (AdcNo < 0 || AdcNo >= kSis3302NofAdcs)) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(thiModule->ClassName(), "WriteEnergySampleLength");
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
	volatile Int_t * sample = (volatile Int_t *) sis3302_mapAddress(offset);
	if (sample == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*sample = SampleLength;
	if (thiModule->CheckBusTrap(Module, offset, "WriteEnergySampleLength")) return(kFALSE);
	return(thiModule->ReadEnergySampleLength(Module, SampleLength, (AdcNo == kSis3302AllAdcs) ? 1 : AdcNo));
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
		gMrbLog->Flush(thiModule->ClassName(), "ReadTauFactor");
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

	volatile Int_t * tauFactor = (volatile Int_t *) sis3302_mapAddress(offset);
	if (tauFactor == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Tau = *tauFactor;
	return (!thiModule->CheckBusTrap(Module, offset, "ReadTauFactor"));
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
		gMrbLog->Flush(thiModule->ClassName(), "WriteTauFactor");
		return(kFALSE);
	}

	if (Tau < kSis3302EnergyTauMin || Tau > kSis3302EnergyTauMax) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Wrong tau factor - " << Tau
						<< " (should be in [0," << kSis3302EnergyTauMax << "]" << endl;
		gMrbLog->Flush(thiModule->ClassName(), "WriteTauFactor");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t t;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			t = Tau;
			if (!thiModule->WriteTauFactor(Module, t, adc)) return(kFALSE);
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

	volatile Int_t * tauFactor = (volatile Int_t *) sis3302_mapAddress(offset);
	if (tauFactor == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*tauFactor = Tau;
	if (thiModule->CheckBusTrap(Module, offset, "WriteTauFactor")) return(kFALSE);
	return(thiModule->ReadTauFactor(Module, Tau, (AdcNo == kSis3302AllAdcs) ? 1 : AdcNo));
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
		gMrbLog->Flush(thiModule->ClassName(), "ReadStartIndex");
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
				gMrbLog->Flush(thiModule->ClassName(), "ReadStartIndex");
				return(kFALSE);
			}
	}

	volatile Int_t * sampleIndex = (volatile Int_t *) sis3302_mapAddress(offset);
	if (sampleIndex == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	IndexValue = *sampleIndex;
	return (!thiModule->CheckBusTrap(Module, offset, "ReadStartIndex"));
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
		gMrbLog->Flush(thiModule->ClassName(), "WriteStartIndex");
		return(kFALSE);
	}

	if (IndexValue < 0 || IndexValue > 0x7FF) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Wrong sample index value - " << IndexValue
						<< " (should be in [0," << 0x7FF << "]" << endl;
		gMrbLog->Flush(thiModule->ClassName(), "WriteStartIndex");
		return(kFALSE);
	}

	if (Index < 0 || Index > 2) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Start index out of range - "
						<< Index << " (should be in [0,2])" << endl;
		gMrbLog->Flush(thiModule->ClassName(), "ReadStartIndex");
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		Int_t iv = IndexValue;
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			if (!thiModule->WriteStartIndex(Module, iv, Index, adc)) return(kFALSE);
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
	}


	volatile Int_t * sampleIndex = (volatile Int_t *) sis3302_mapAddress(offset);
	if (sampleIndex == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*sampleIndex = IndexValue;
	if (thiModule->CheckBusTrap(Module, offset, "WriteStartIndex")) return(kFALSE);
	return(thiModule->ReadStartIndex(Module, IndexValue, Index, (AdcNo == kSis3302AllAdcs) ? 1 : AdcNo));
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
	volatile Int_t * ctrl = (volatile Int_t *) sis3302_mapAddress(offset);
	if (ctrl == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Data = *ctrl;
	return (!thiModule->CheckBusTrap(Module, offset, "ReadAcquisitionControl"));
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
	volatile Int_t * ctrl = (volatile Int_t *) sis3302_mapAddress(offset);
	if (ctrl == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*ctrl = Data;
	return (!thiModule->CheckBusTrap(Module, offset, "WriteAcquisitionControl"));
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
	if (!thiModule->ReadAcquisitionControl(Module, data)) return(kFALSE);
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
	if (ClockSource < 0 || ClockSource > 6) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: Clock source out of range - "
						<< ClockSource << " (should be in [0,6])" << endl;
		gMrbLog->Flush(thiModule->ClassName(), "SetClockSource");
		return(kFALSE);
	}
	Int_t data = (0x7 << 28);			// clear all bits
	if (!thiModule->WriteAcquisitionControl(Module, data)) return(kFALSE);
	data = (ClockSource & 0x7) << 12;	// set bits
	return(thiModule->WriteAcquisitionControl(Module, data));
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
	if (!thiModule->ReadAcquisitionControl(Module, data)) return(kFALSE);
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
		gMrbLog->Flush(thiModule->ClassName(), "SetLemoInMode");
		return(kFALSE);
	}
	Int_t data = (0x7 << 16);			// clear all bits
	if (!thiModule->WriteAcquisitionControl(Module, data)) return(kFALSE);
	data = Bits & 0x7;					// set bits
	return(thiModule->WriteAcquisitionControl(Module, data));
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
	if (!thiModule->ReadAcquisitionControl(Module, data)) return(kFALSE);
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
		gMrbLog->Flush(thiModule->ClassName(), "SetLemoOutMode");
		return(kFALSE);
	}
	Int_t data = (0x3 << 20);			// clear all bits
	if (!thiModule->WriteAcquisitionControl(Module, data)) return(kFALSE);
	data = (Bits & 0x3) << 4;			// set bits
	return(thiModule->WriteAcquisitionControl(Module, data));
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
	if (!thiModule->ReadAcquisitionControl(Module, data)) return(kFALSE);
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
		gMrbLog->Flush(thiModule->ClassName(), "SetLemoInEnableMask");
		return(kFALSE);
	}
	Int_t data = (0x7 << 24);			// clear all bits
	if (!thiModule->WriteAcquisitionControl(Module, data)) return(kFALSE);
	data = (Bits & 0x7) << 8;			// set bits
	return(thiModule->WriteAcquisitionControl(Module, data));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads timeout value
//! \param[in]		Module			-- module address
//! \param[out]		Timeout			-- timeout in seconds
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::GetTimeout(SrvVMEModule * Module, Int_t & Timeout) {
	  Timeout = fTimeout;
	  return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets timeout value
//! \param[in]		Module			-- module address
//! \param[in]		Timeout			-- timeout in seconds
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::SetTimeout(SrvVMEModule * Module, Int_t & Timeout) {
	if (Timeout < 0) Timeout = 0;
	fTimeout = Timeout;
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads a single event
//! \param[in]		Module		-- module address
//! \param[out]		Data		-- where to store event data
//! \param[in]		AdcNo		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::GetSingleEvent(SrvVMEModule * Module, TArrayI & Data, Int_t AdcNo) {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(thiModule->ClassName(), "GetSingleEvent");
		return(kFALSE);
	}

	Int_t rawDataLength;
	if (!thiModule->ReadRawDataSampleLength(Module, rawDataLength, AdcNo)) return(kFALSE);

	Int_t energyDataLength;
	if (!thiModule->ReadEnergySampleLength(Module, energyDataLength, AdcNo)) return(kFALSE);

 	Int_t nofWords = kSis3302EventHeader
					+ rawDataLength
					+ energyDataLength
					+ kSis3302EventMinMax
					+ kSis3302EventTrailer;

	Int_t n = nofWords / 2;
	thiModule->WriteEndAddrThresh(Module, n);

	Data.Set(nofWords + kSis3302EventPreHeader);
	Data[0] = rawDataLength;
	Data[1] = energyDataLength;

	thiModule->KeyAddr(Module, kSis3302KeyResetSample);

	thiModule->KeyAddr(Module, kSis3302KeyArmBank1Sampling);

	Int_t timeout = fTimeout;
	while (!thiModule->DataReady(Module) && !thiModule->Timeout(Module, timeout));

	thiModule->KeyAddr(Module, kSis3302KeyDisarmSample);

	if (timeout < 0) return(kTRUE);

	return(thiModule->ReadData(Module, Data, nofWords, AdcNo));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Read ADC data
//! \param[in]		Module		-- module address
//! \param[out]		Data		-- where to store event data
//! \param[in]		NofWords	-- number of data words
//! \param[in]		AdcNo		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadData(SrvVMEModule * Module, TArrayI & Data, Int_t NofWords, Int_t AdcNo)  {

	if (AdcNo == kSis3302AllAdcs || AdcNo < 0 || AdcNo >= kSis3302NofAdcs) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: ADC number out of range - "
						<< AdcNo << " (should be in [0," << (kSis3302NofAdcs - 1) << "])" << endl;
		gMrbLog->Flush(thiModule->ClassName(), "ReadData");
		return(kFALSE);
	}

	Int_t nextSample;
	if (!thiModule->ReadNextSampleAddr(Module, nextSample, AdcNo)) return(kFALSE);
	nextSample &= 0x3FFFFC;
	nextSample >>= 1;
	Int_t startAddr = SIS3302_ADC1_OFFSET + AdcNo * SIS3302_NEXT_ADC_OFFSET;

	volatile Int_t * mappedAddr = (Int_t *) sis3302_mapAddress(startAddr);
	if (mappedAddr == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Int_t k = kSis3302EventPreHeader;
	for (Int_t i = 0; i < nextSample; i++, k += 2) {
		Int_t d = *mappedAddr++;
		Data[k] = d & 0xFFFF;
		Data[k + 1] = (d >> 16) & 0xFFFF;
	}

	return(!thiModule->CheckBusTrap(Module, startAddr, "ReadData"));
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Check if data ready
//! \param[in]		Module		-- module address
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::DataReady(SrvVMEModule * Module) {
	Int_t sts;
	thiModule->ReadAcquisitionControl(Module, sts);
	return((sts & SIS3302_STATUS_END_THRESH) != 0);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
//! \details		Check if timeout
//! \param[in]		Module		-- module address
//! \param[out]		Timeout		-- timeout counter
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::Timeout(SrvVMEModule * Module, Int_t & Timeout) {
	if (fTimeout == 0) return(kFALSE);
	if (Timeout == 0) {
	  	gMrbLog->Err()	<< "[" << Module->GetName() << "]: Timeout after " << fTimeout << " s" << endl;
		gMrbLog->Flush(thiModule->ClassName(), "Timeout");
		return(kTRUE);
	}
	sleep(1);
	Timeout--;
	return(kFALSE);
}

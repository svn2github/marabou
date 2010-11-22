//__________________________________________________________[C IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
//! \file			Sis3302_Functions.c
//! \brief			Interface for SIS3302 ADCs
//! $Author: Marabou $
//! $Mail			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.7 $
//! $Date: 2010-11-22 14:19:20 $
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

//________________________________________________________________[C FUNCTION]
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
		Module->baseAddr = BaseAddr;
		Module->vmeAddr = VmeAddr;
		strcpy(Module->moduleName, Name);
		Module->serial = Serial;
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

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Maps address to vme memory
//! \param[in]		Module			-- module database
//! \param[out]		Offset	 		-- register offset
//! \retval 		MappedAddr		--
//////////////////////////////////////////////////////////////////////////////

volatile char * sis3302_MapAddr(struct s_sis_3302 * Module, Int_t Offset) {

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


//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads (and outputs) module info
//! \param[in]		Module			-- module address
//! \param[out]		BoardId 		-- board id
//! \param[out]		MajorVersion	-- firmware version (major)
//! \param[out]		MinorVersion	-- firmware version (minor)
//! \param[in]		PrintFlag		-- output to gMrbLog if kTRUE
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_moduleInfo(struct s_sis_3302 * Module, Int_t * BoardId, Int_t * MajorVersion, Int_t * MinorVersion, Bool_t PrintFlag) {

	volatile ULong_t * firmWare;
	ULong_t ident;
	Int_t b;

	firmWare = (volatile ULong_t *) sis3302_mapAddress(Module, SIS3302_MODID);
	if (firmWare == NULL) return(kFALSE);

	ident = *firmWare;

	b = (ident >> 16) & 0xFFFF;
	BoardId = (b &0xF) + 10 * ((b >> 4) & 0xF) + 100 * ((b >> 8) & 0xF) + 1000 * ((b >> 12) & 0xF);
	MajorVersion = (ident >> 8) & 0xFF;
	MinorVersion = ident & 0xFF;
	if (PrintFlag) {
		sprintf(msg, "[moduleInfo] SIS module info: addr (phys) %#lx (log) %#lx mod %#x type %d version %x%02x", Module->baseAddr(), Module->mappedAddr(), Module->addrMod, BoardId, MajorVersion, MinorVersion);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
	}
	return(kTRUE);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Turns user LED on/off
//! \param[in]		Module			-- module address
//! \param[in]		OnFlag			-- kTRUE if to be turned on
//! \param[in]		AdcNo 			-- adc number (ignored))
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_setUserLED(struct s_sis_3302 * Module, Bool_t OnFlag) {

	UInt_t data;
	data = OnFlag ? 0x1 : 0x10000;
	if (!sis3302_writeControlStatus(Module, data)) return(kFALSE);
	return(kTRUE);
}

//________________________________________________________________[C FUNCTION]
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
	volatile Int_t * dacStatus;
	volatile Int_t * dacData;
	Int_t maxTimeout;
	Int_t timeout;
	Int_t data;
	Int_t adc;

	if (!sis3302_checkChannelNo(Module, "readDac", AdcNo)) return (kFALSE);

	if (AdcNo == kSis3302AllAdcs) {
		firstAdc = 0;
		lastAdc = kSis3302NofAdcs - 1;
	} else {
		firstAdc = AdcNo;
		lastAdc = AdcNo;
	}
	wc = lastAdc - firstAdc + 1;

	dacStatus = (volatile Int_t *) sis3302_mapAddress(Module, SIS3302_DAC_CONTROL_STATUS);
	if (dacStatus == NULL) return(kFALSE);
	dacData = (volatile Int_t *) sis3302_mapAddress(Module, SIS3302_DAC_DATA);
	if (dacData == NULL) return(kFALSE);

	maxTimeout = 5000;
	DacValues.Set(wc);
	Int_t idx = 0;
	for (adc = firstAdc; adc <= lastAdc; adc++, idx++) {
		*dacStatus = kSis3302DacCmdLoadShiftReg + (adc << 4);
		timeout = 0 ;
		do {
			data = *dacStatus;
			timeout++;
		} while (((data & kSis3302DacBusy) == kSis3302DacBusy) && (timeout <  maxTimeout));
		if (timeout >=  maxTimeout) return(kFALSE);

		DacValues[idx] = (*dacData >> 16) & 0xFFFF;
	}
	return(kTRUE);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes dac values
//! \param[in]		Module		-- module address
//! \param[in]		DacValues	-- dac values to be written
//! \param[in]		AdcNo		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_writeDac(struct s_sis_3302 * Module, Int_t DacValues[], Int_t AdcNo) {

	Int_t firstAdc;
	Int_t lastAdc;
	Int_t wc;
	volatile Int_t * dacStatus;
	volatile Int_t * dacData;
	Int_t maxTimeout;
	Int_t idx;
	Int_t data;
	Int_t adc;

	if (!sis3302_checkChannelNo(Module, "writeDac", AdcNo)) return (kFALSE);

	if (AdcNo == kSis3302AllAdcs) {
		firstAdc = 0;
		lastAdc = kSis3302NofAdcs - 1;
	} else {
		firstAdc = AdcNo;
		lastAdc = AdcNo;
	}
	wc = lastAdc - firstAdc + 1;

	dacStatus = (volatile Int_t *) sis3302_mapAddress(Module, SIS3302_DAC_CONTROL_STATUS);
	if (dacStatus == NULL) return(kFALSE);
	dacData = (volatile Int_t *) sis3302_mapAddress(Module, SIS3302_DAC_DATA);
	if (dacData == NULL) return(kFALSE);

	maxTimeout = 5000;
	idx = 0;
	for (adc = firstAdc; adc <= lastAdc; adc++, idx++) {
		*dacData = DacValues[idx];
		*dacStatus = kSis3302DacCmdLoadShiftReg + (adc << 4);
		timeout = 0 ;
		do {
			data = *dacStatus;
			timeout++;
		} while (((data & kSis3302DacBusy) == kSis3302DacBusy) && (timeout <  maxTimeout));
		if (timeout >=  maxTimeout) return(kFALSE);

		*dacStatus = kSis3302DacCmdLoad + (adc << 4);
		timeout = 0 ;
		do {
			data = *dacStatus;
			timeout++;
		} while (((data & kSis3302DacBusy) == kSis3302DacBusy) && (timeout <  maxTimeout));
		if (timeout >=  maxTimeout) return(kFALSE);
	}
	return(kTRUE);
}

Bool_t sis3302_writeDac_db(struct s_sis_3302 * Module, Int_t AdcNo) { return(sis3302_writeDac(Module, Module->dacValues[AdcNo], AdcNo)); }

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Executes KEY ADDR command
//! \param[in]		Module		-- module address
//! \param[in]		Key 		-- key
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_keyAddr(struct s_sis_3302 * Module, Int_t Key) {

	Int_t offset;
	volatile Int_t * keyAddr;

	offset = 0;
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
			sprintf(msg, "[keyAddr] [%s]: Illegal key value - %d", Module->moduleName, Key);
			f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
			return(kFALSE);
	}

	keyAddr = (Int_t *) sis3302_mapAddress(Module, offset);
	if (keyAddr == NULL) return(kFALSE);

	*keyAddr = 0;
	if (Key == kSis3302KeyReset || Key == kSis3302KeyResetSample) sleep(1);
	return (kTRUE);
}

//! Exec KEY command: reset
Bool_t sis3302_keyReset(struct s_sis_3302 * Module) { return(sis3302_keyAddr(Module, kSis3302KeyReset); }
//! Exec KEY command: reset sample
Bool_t sis3302_keyResetSample(struct s_sis_3302 * Module) { return(sis3302_keyAddr(Module, kSis3302KeyResetSample)); };
//! Exec KEY command: generate trigger
Bool_t sis3302_keyTrigger(struct s_sis_3302 * Module) { return(sis3302_keyAddr(Module, kSis3302KeyTrigger)); };
//! Exec KEY command: clear time stamp
Bool_t sis3302_keyClearTimestamp(struct s_sis_3302 * Module) { return(sis3302_keyAddr(Module, kSis3302KeyClearTimestamp)); };
//! Exec KEY command: arm sampling for bank 1
Bool_t sis3302_keyArmBank1Sampling(struct s_sis_3302 * Module) { return(sis3302_keyAddr(Module, kSis3302KeyArmBank1Sampling)); };
//! Exec KEY command: arm sampling for bank 2
Bool_t sis3302_keyArmBank2Sampling(struct s_sis_3302 * Module) { return(sis3302_keyAddr(Module, kSis3302KeyArmBank2Sampling)); };
//! Exec KEY command: disarm sample
Bool_t sis3302_keyDisarmSample(struct s_sis_3302 * Module) { return(sis3302_keyAddr(Module, kSis3302KeyDisarmSample)); };

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads control and status register
//! \param[in]		Module			-- module address
//! \return 		Bits			-- data
//////////////////////////////////////////////////////////////////////////////

UInt_t sis3302_readControlStatus(struct s_sis_3302 * Module) {

	Int_t offset;
	volatile UInt_t * ctrlStat;

	offset = SIS3302_CONTROL_STATUS;

	ctrlStat = (volatile UInt_t *) sis3302_mapAddress(Module, offset);
	if (ctrlStat == NULL) return(0);

	return (*ctrlStat);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes control and status register
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- data
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_writeControlStatus(struct s_sis_3302 * Module, UInt_t Bits) {

	Int_t offset;
	volatile UInt_t * ctrlStat;

	offset = SIS3302_CONTROL_STATUS;

	ctrlStat = (volatile UInt_t *) sis3302_mapAddress(Module, offset);
	if (ctrlStat == NULL) return(kFALSE);

	*ctrlStat = Bits;
	return(kTRUE);
}

Bool_t sis3302_writeControlStatus_db(struct s_sis_3302 * Module) { return(sis3302_writeControlStatus(Module, Module->controlStatus)); }

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads event config register
//! \param[in]		Module			-- module address
//! \param[in]		AdcNo 			-- adc number
//! \return 		Bits			-- configuration
//////////////////////////////////////////////////////////////////////////////

UInt_t sis3302_readEventConfig(struct s_sis_3302 * Module, Int_t AdcNo);

	volatile UInt_t * evtConf;
	Int_t offset;

	if (!sis3302_checkChannelNo(Module, "readEventConfig", AdcNo)) return (0xaffec0c0);

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

	evtConf = (volatile UInt_t *) sis3302_mapAddress(Module, offset);
	if (evtConf == NULL) return(0xaffec0c0);

	return (*evtConf);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes event config register
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- configuration
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_writeEventConfig(struct s_sis_3302 * Module, UInt_t Bits, Int_t AdcNo) {

	volatile Int_t * evtConf;
	Int_t offset;

	if (!sis3302_checkChannelNo(Module, "writeEventConfig", AdcNo)) return (kFALSE);

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

	evtConf = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (evtConf == NULL) return(kFALSE);

	*evtConf = Bits;
	return(kTRUE);
}

Bool_t sis3302_writeEventConfig_db(struct s_sis_3302 * Module, Int_t AdcNo) { return(sis3302_writeEventConfig(Module, Module->evtConfig[AdcNo/2], AdcNo)); }

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads EXTENDED event config register
//! \param[in]		Module			-- module address
//! \param[in]		AdcNo 			-- adc number
//! \return 		Bits			-- configuration
//////////////////////////////////////////////////////////////////////////////

UInt_t sis3302_readEventExtendedConfig(struct s_sis_3302 * Module, Int_t AdcNo) {

	Int_t offset;
	volatile Int_t * evtConf;

	if (!sis3302_checkChannelNo(Module, "readEventExtendedConfig", AdcNo)) return (0xaffec0c0);

	switch (AdcNo) {
		case 0:
		case 1: 	offset = SIS3302_EVENT_EXTENDED_CONFIG_ADC12; break;
		case 2:
		case 3: 	offset = SIS3302_EVENT_EXTENDED_CONFIG_ADC34; break;
		case 4:
		case 5: 	offset = SIS3302_EVENT_EXTENDED_CONFIG_ADC56; break;
		case 6:
		case 7: 	offset = SIS3302_EVENT_EXTENDED_CONFIG_ADC78; break;
	}sis3302_writeEventConfig

	evtConf = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (evtConf == NULL) return(0xaffec0c0);

	return (*evtConf);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes EXTENDED event config register
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- configuration
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_writeEventExtendedConfig(struct s_sis_3302 * Module, UInt_t Bits, Int_t AdcNo) {

	Int_t offset;
	volatile Int_t * evtConf;

	if (!sis3302_checkChannelNo(Module, "writeEventExtendedConfig", AdcNo)) return (kFALSE);

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

	evtConf = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (evtConf == NULL) return(kFALSE);

	*evtConf = Bits;
	return(kTRUE);
}

Bool_t sis3302_writeEventExtendedConfig_db(struct s_sis_3302 * Module, Int_t AdcNo) { return(sis3302_writeEventConfig(Module, Module->xEvtConfig[AdcNo/2], AdcNo)); }

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns header data
//! \param[in]		Module		-- module address
//! \param[in]		AdcNo 		-- adc number
//! \return 		Bits		-- header data
//////////////////////////////////////////////////////////////////////////////

UInt_t sis3302_getHeaderBits(struct s_sis_3302 * Module, Int_t AdcNo) {

	UInt_t bits;

	bits = sis3302_readEventConfig(Module, AdcNo));
	if (bits != 0xaffec0c0) {
		bits >>= 17;		// header bit #1 -> event config bit #17
		bits &= kSis3302HeaderMask;
	}
	return(bits);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Defines header data
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- bits
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_setHeaderBits(struct s_sis_3302 * Module, UInt_t Bits, Int_t AdcNo) {

	UInt_t bits;
	Int_t adc;

	if (Bits & ~kSis3302HeaderMask) {
		sprintf(msg, "[setHeaderBits] [%s]: Illegal header data - %#lx (should be in %#lx)", Module->moduleName,  Bits, kSis3302HeaderMask);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (Bits & 0x3) {
		sprintf(msg, "[setHeaderBits] [%s]: Illegal header data - %#lx (group id bits 0 & 1 are READ ONLY)", Module->moduleName,  Bits);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	Bits >>= 2;			// bits 1 & 2 are read-only

	if (AdcNo == kSis3302AllAdcs) {
		for (adc = 0; adc < kSis3302NofAdcs; adc++) {
			if (!sis3302_setHeaderBits(Module, Bits, adc)) return(kFALSE);
		}
		return(kTRUE);
	}

	bits = sis3302_readEventConfig(Module, AdcNo));
	if (bits == 0xaffec0c0) return(kFALSE);

	bits &= 0xFFFF;
	bits |=	(Bits << 19);		// header bit #1 -> event config bit #17, bits 1 & 2 read-only
	if (!sis3302_writeEventConfig(Module, bits, AdcNo)) return(kFALSE);
	return(kTRUE);
}

Bool_t sis3302_setHeaderBits_db(struct s_sis_3302 * Module, Int_t AdcNo) { return(sis3302_setHeaderBits(Module, Module->headerBits[AdcNo/2], AdcNo)); }

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns group id (read-only)
//! \param[in]		Module		-- module address
//! \param[in]		AdcNo 		-- adc number
//! \return 		GroupId 	-- group id
//////////////////////////////////////////////////////////////////////////////

UInt_t sis3302_getGroupId(struct s_sis_3302 * Module, Int_t AdcNo) {

	UInt_t bits;

	bits = sis3302_readEventConfig(Module, AdcNo));
	if (bits != 0xaffec0c0) {
		bits >>= 17;
		bits &= 0x3;
	}
	return(bits);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns trigger mode
//! \param[in]		Module		-- module address
//! \param[in]		AdcNo 		-- adc number
//! \return 		Bits		-- trigger mode
//////////////////////////////////////////////////////////////////////////////

UInt_t sis3302_getTriggerMode(struct s_sis_3302 * Module, Int_t AdcNo) {

	UInt_t bits;

	bits = sis3302_readEventConfig(Module, AdcNo));
	if (bits != 0xaffec0c0) {
		if (AdcNo & 1) {
			bits >>= 10;
		} else {
			bits >>= 2;
		}
		bits &= kSis3302TriggerBoth;
	}
	return(bits);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Defines trigger mode
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- trigger mode
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_setTriggerMode(struct s_sis_3302 * Module, Unt_t Bits, Int_t AdcNo);

	UInt_t bits;
	Int_t adc;

	if (Bits < kSis3302TriggerOff || Bits > kSis3302TriggerBoth) {
		sprintf(msg, "[setTriggerMode] [%s]: Illegal trigger mode - %#lx (should be in [%d,%d])", Module->moduleName,  Bits, kSis3302TriggerOff, kSis3302TriggerBoth);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		for (adc = 0; adc < kSis3302NofAdcs; adc++) {
			if (!sis3302_setTriggerMode(Module, Bits, adc)) return(kFALSE);
		}
		return(kTRUE);
	}

	bits = sis3302_readEventConfig(Module, AdcNo));
	if (bits == 0xaffec0c0) return(kFALSE);

	if (AdcNo & 1) {
		bits &= ~(kSis3302TriggerBoth << 10);
		bits |=	(Bits << 10);
	} else {
		bits &= ~(kSis3302TriggerBoth << 2);
		bits |=	(Bits << 2);
	}
	return(sis3302_writeEventConfig(Module, bits, AdcNo));
}

Bool_t sis3302_setTriggerMode_db(struct s_sis_3302 * Module, Int_t AdcNo) { return(sis3302_setTriggerMode(Module, Module->triggerMode[AdcNo], AdcNo)); }

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns gate mode
//! \param[in]		Module		-- module address
//! \param[out]		Bits		-- bits (0,1,2,3)
//! \param[in]		AdcNo 		-- adc number
//! \return 		Bits		-- gate mode
//////////////////////////////////////////////////////////////////////////////

UInt_t sis3302_getGateMode(struct s_sis_3302 * Module, Int_t AdcNo) {

	UInt_t bits;

	bits = sis3302_readEventConfig(Module, AdcNo));
	if (bits != 0xaffec0c0) {
		if (AdcNo & 1) {
			bits >>= 12;
		} else {
			bits >>= 4;
		}
		bits &= kSis3302GateBoth;
	}
	return(bits);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Defines gate mode
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- gate mode
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_setGateMode(struct s_sis_3302 * Module, UInt_t Bits, Int_t AdcNo) {

	UInt_t bits;
	Int_t adc;

	if (Bits < kSis3302GateOff || Bits > kSis3302GateBoth) {
		sprintf(msg, "[setGateMode] [%s]: Illegal gate mode - %#lx (should be in [%d,%d])", Module->moduleName,  Bits, kSis3302GateOff, kSis3302GateBoth);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		for (adc = 0; adc < kSis3302NofAdcs; adc++) {
			if (!sis3302_setGateMode(Module, Bits, adc)) return(kFALSE);
		}
		return(kTRUE);
	}

	bits = sis3302_readEventConfig(Module, AdcNo));
	if (bits == 0xaffec0c0) return(kFALSE;

	if (AdcNo & 1) {
		bits &= ~(kSis3302GateBoth << 12);
		bits |=	(Bits << 12);
	} else {
		bits &= ~(kSis3302GateBoth << 4);
		bits |=	(Bits << 4);
	}
	return(sis3302_writeEventConfig(Module, bits, AdcNo));
}

Bool_t sis3302_setGateMode_db(struct s_sis_3302 * Module, Int_t AdcNo) { return(sis3302_setGateMode(Module, Module->gateMode[AdcNo], AdcNo)); }

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns next-neighbor trigger mode
//! \param[in]		Module		-- module address
//! \param[in]		AdcNo 		-- adc number
//! \return 		Bits		-- trigger mode
//////////////////////////////////////////////////////////////////////////////

UInt_t sis3302_getNextNeighborTriggerMode(struct s_sis_3302 * Module, Int_t AdcNo) {

	UInt_t bits;

	bits = sis3302_readEventExtendedConfig(Module, AdcNo));
	if (bits != 0xaffec0c0) {
		if (AdcNo & 1) {
		bits >>= 14;
			bits >>= 14;
		} else {
			bits >>= 6;
		}
		bits &= kSis3302TriggerBoth;
	}
	return(bits);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Defines next-neighbor trigger mode
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- bits (0,1,2,3)
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_setNextNeighborTriggerMode(struct s_sis_3302 * Module, Int_t Bits, Int_t AdcNo) {

	UInt_t bits;
	Int_t adc;

	if (Bits < kSis3302TriggerOff || Bits > kSis3302TriggerBoth) {
		sprintf(msg, "[setNextNeighborTriggerMode] [%s]: Illegal next-neighbor trigger mode - %#lx (should be in [%d,%d])", Module->moduleName,  Bits, kSis3302TriggerOff, kSis3302TriggerBoth);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		for (adc = 0; adc < kSis3302NofAdcs; adc++) {
			if (!sis3302_setNextNeighborTriggerMode(Module, Bits, adc)) return(kFALSE);
		}
		return(kTRUE);
	}

	bits = sis3302_readEventExtendedConfig(Module, AdcNo));
	if (bits == 0xaffec0c0) return(kFALSE);

	if (AdcNo & 1) {
		bits &= ~(kSis3302TriggerBoth << 10);
		bits |=	(Bits << 14);
	} else {
		bits &= ~(kSis3302TriggerBoth << 2);
		bits |=	(Bits << 6);
	}
	return(sis3302_writeEventExtendedConfig(Module, bits, AdcNo));
}

Bool_t sis3302_setNextNeighborTriggerMode_db(struct s_sis_3302 * Module, Int_t AdcNo) { return(sis3302_setNextNeighborTriggerMode(Module, Module->nextNeighborTrigger[AdcNo], AdcNo)); }

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns next-neighbor gate mode
//! \param[in]		Module		-- module address
//! \param[in]		AdcNo 		-- adc number
//! \return 		Bits		-- gate mode
//////////////////////////////////////////////////////////////////////////////

UInt_t sis3302_getNextNeighborGateMode(struct s_sis_3302 * Module, Int_t AdcNo) {

	UInt_t bits;

	bits = sis3302_readEventExtendedConfig(Module, AdcNo));
	if (bits != 0xaffec0c0) {
		if (AdcNo & 1) {
			bits >>= 14;
		} else {
			bits >>= 6;
		}
		bits &= kSis3302GateBoth;
	}
	return(bits);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Defines next-neighbor gate mode
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- bits (0,1,2,3)
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_setNextNeighborGateMode(struct s_sis_3302 * Module, Int_t Bits, Int_t AdcNo) {

	UInt_t bits;
	Int_t adc;

	if (Bits < kSis3302GateOff || Bits > kSis3302GateBoth) {
		sprintf(msg, "[setNextNeighborTriggerMode] [%s]: Illegal next-neighbor gate mode - %#lx (should be in [%d,%d])", Module->moduleName,  Bits, kSis3302GateOff, kSis3302GateBoth);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		for (adc = 0; adc < kSis3302NofAdcs; adc++) {
			if (!sis3302_setNextNeighborGateMode(Module, Bits, adc)) return(kFALSE);
		}
		return(kTRUE);
	}

	bits = sis3302_readEventExtendedConfig(Module, AdcNo));
	if (bits == 0xaffec0c0) return(kFALSE);

	if (AdcNo & 1) {
		bits &= ~(kSis3302GateBoth << 14);
		bits |=	(Bits << 14);
	} else {
		bits &= ~(kSis3302GateBoth << 6);
		bits |=	(Bits << 6);
	}
	return(sis3302_writeEventConfig(Module, bits, AdcNo));
}

Bool_t sis3302_setNextNeighborGateMode_db(struct s_sis_3302 * Module, Int_t AdcNo) { return(sis3302_setNextNeighborGateMode(Module, Module->nextNeighborGate[AdcNo], AdcNo)); }

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns trigger polarity
//! \param[in]		Module		-- module address
//! \param[in]		AdcNo 		-- adc number
//! \return 		InvertFlag	-- kTRUE if trigger is inverted
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_polarityIsInverted(struct s_sis_3302 * Module, Int_t AdcNo) {

	UInt_t bits;
	Bool_t invertFlag;

	bits = sis3302_readEventConfig(Module, AdcNo));
	if (bits == 0xaffec0c0) return(kFALSE);

	if (AdcNo & 1) bits >>= 8;
	invertFlag = ((bits & kSis3302PolarityNegative) != 0);
	return(invertFlag);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Defines trigger polarity
//! \param[in]		Module			-- module address
//! \param[in]		InvertFlag		-- kTRUE if neg polarity
//! \param[in]		AdcNo 			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_setPolarity(struct s_sis_3302 * Module, Bool_t InvertFlag, Int_t AdcNo) {

	Int_t adc;
	UInt_t bits;

	if (AdcNo == kSis3302AllAdcs) {
		Bool_t ifl;
		for (adc = 0; adc < kSis3302NofAdcs; adc++) {
			if (!sis3302_setPolarity(Module, InvertFlag, adc)) return(kFALSE);
		}
		return(kTRUE);
	}

	bits = sis3302_readEventConfig(Module, AdcNo));
	if (bits == 0xaffec0c0) return(kFALSE);

	if (AdcNo & 1) {
		if (InvertFlag) bits |= (kSis3302PolarityNegative << 8); else bits &= ~(kSis3302PolarityNegative << 8);
	} else {
		data &= ~1;
		if (InvertFlag) bits |= kSis3302PolarityNegative; else bits &= ~kSis3302PolarityNegative;
	}
	return(sis3302_writeEventConfig(Module, bits, AdcNo));
}

Bool_t sis3302_setPolarity_db(struct s_sis_3302 * Module, Int_t AdcNo) { return(sis3302_setPolarity(Module, Module->invertSignal[AdcNo], AdcNo)); }

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads end addr threshold register
//! \param[in]		Module		-- module address
//! \param[in]		AdcNo 		-- adc number
//! \return 		Thresh		-- threshold
//////////////////////////////////////////////////////////////////////////////

Int_t sis3302_readEndAddrThresh(struct s_sis_3302 * Module, Int_t AdcNo) {

	Int_t offset;
	volatile Int_t * endAddr;

	if (!sis3302_checkChannelNo(Module, "readEndAddrThresh", AdcNo)) return (kFALSE);

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

	endAddr = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (endAddr == NULL) return(kFALSE);

	return (*endAddr);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes end addr threshold register
//! \param[in]		Module		-- module address
//! \param[in]		Thresh		-- threshold
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_writeEndAddrThresh(struct s_sis_3302 * Module, Int_t Thresh, Int_t AdcNo) {

	Int_t offset;
	volatile Int_t * endAddr;

	if (!sis3302_checkChannelNo(Module, "writeEndAddrThresh", AdcNo)) return (kFALSE);

	if (Thresh > kSis3302EndAddrThreshMax) {
		sprintf(msg, "[writeEndAddrThresh] [%s]: Threshold out of range - %#lx (max %#lx)", Module->moduleName, Thresh, kSis3302EndAddrThreshMax);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

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

	endAddr = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (endAddr == NULL) return(kFALSE);

	*endAddr = Thresh;
	return(kTRUE);
}

Bool_t sis3302_writeEndAddrThresh_db(struct s_sis_3302 * Module, Int_t AdcNo) { return(sis3302_writeEndAddrThresh(Module, Module->endAddrThresh[AdcNo/2], AdcNo)); }

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger register
//! \param[in]		Module		-- module address
//! \param[in]		AdcNo 		-- adc number
//! \return 		Bits		-- trigger register
//////////////////////////////////////////////////////////////////////////////

UInt_t sis3302_readPreTrigDelayAndGateLength(struct s_sis_3302 * Module, Int_t AdcNo) {

	Int_t offset;
	volatile Unt_t * trigReg;

	if (!sis3302_checkChannelNo(Module, "readPreTrigDelayAndGateLength", AdcNo)) return (0xaffec0c0);

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

	trigReg = (volatile UInt_t *) sis3302_mapAddress(Module, offset);
	if (trigReg == NULL) return(0xaffec0c0);

	return (*trigReg);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes trigger register
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- trigger register
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_writePreTrigDelayAndGateLength(struct s_sis_3302 * Module, UInt_t Bits, Int_t AdcNo) {

	Int_t offset;
	volatile Unt_t * trigReg;

	if (!sis3302_checkChannelNo(Module, "writePreTrigDelayAndGateLength", AdcNo)) return (kFALSE);

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

	trigReg = (volatile UInt_t *) sis3302_mapAddress(Module, offset);
	if (trigReg == NULL) return(kFALSE);

	*trigReg = Bits;
	return(TRUE);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads pretrigger delay
//! \param[in]		Module		-- module address
//! \param[in]		AdcNo 		-- adc number
//! \return 		Delay		-- delay
//////////////////////////////////////////////////////////////////////////////

Int_t sis3302_readPreTrigDelay(struct s_sis_3302 * Module, Int_t AdcNo) {

	UInt_t delay;

	delay = sis3302_readPreTrigDelayAndGateLength(Module, AdcNo);
	if (delay != 0xaffec0c0) {
		delay >>= 16;
		delay -= 2;
		if (delay < 0) delay += 1024;
		delay &= 0xFFF;
	}
	return((Int_t) delay);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes pretrigger delay
//! \param[in]		Module		-- module address
//! \param[in]		Delay		-- delay
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_writePreTrigDelay(struct s_sis_3302 * Module, Int_t Delay, Int_t AdcNo) {

	UInt_t delay;
	Int_t adc;

	if (AdcNo == kSis3302AllAdcs) {
		for (adc = 0; adc < kSis3302NofAdcs; adc++) {
			if (!sis3302_writePreTrigDelay(Module, Delay, adc)) return(kFALSE);
		}
		return(kTRUE);
	}

	delay = sis3302_readPreTrigDelayAndGateLength(Module, AdcNo);
	if (delay == 0xaffec0c0) return(delay);

	Delay += 2;
	if (Delay >= 1024) Delay -= 1024;

	delay &= 0xFFFF;
	delay |= Delay << 16;
	return(sis3302_writePreTrigDelayAndGateLength(Module, delay, AdcNo));
}

Bool_t sis3302_writePreTrigDelay_db(struct s_sis_3302 * Module, Int_t AdcNo) { return(sis3302_writePreTrigDelay(Module, Module->pretrigDelay[AdcNo/2], AdcNo)); }

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger gate length
//! \param[in]		Module		-- module address
//! \param[in]		AdcNo 		-- adc number
//! \return 		Gate		-- gate
//////////////////////////////////////////////////////////////////////////////

Int_t sis3302_readTrigGateLength(struct s_sis_3302 * Module, Int_t AdcNo) {

	UInt_t gate;

	gate = sis3302_readPreTrigDelayAndGateLength(Module, AdcNo);
	if (gate != 0xaffec0c0) {
		gate &= 0xFFFF;
		gate += 1;
	}
	return((Int_t) gate);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes trigger gate length
//! \param[in]		Module		-- module address
//! \param[in]		Gate		-- gate
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_writeTrigGateLength(struct s_sis_3302 * Module, Int_t Gate, Int_t AdcNo) {

	UInt_t gate;
	Int_t adc;

	if (AdcNo == kSis3302AllAdcs) {
		for (adc = 0; adc < kSis3302NofAdcs; adc++) {
			if (!sis3302_writeTrigGateLength(Module, Gate, adc)) return(kFALSE);
		}
		return(kTRUE);
	}

	gate = sis3302_readPreTrigDelayAndGateLength(Module, AdcNo);
	if (gate == 0xaffec0c0) return(gate);

	gate &= 0xFFFF0000;
	gate |= Gate - 1;
	return(sis3302_writePreTrigDelayAndGateLength(Module, gate, AdcNo));
}

Bool_t sis3302_writeTrigGateLength_db(struct s_sis_3302 * Module, Int_t AdcNo) { return(sis3302_writeTrigGateLength(Module, Module->trigGateLength[AdcNo/2], AdcNo)); }

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads raw data buffer register
//! \param[in]		Module		-- module address
//! \param[in]		AdcNo 		-- adc number
//! \return 		Bits		-- bits
//////////////////////////////////////////////////////////////////////////////

UInt_t sis3302_readRawDataBufConfig(struct s_sis_3302 * Module, Int_t AdcNo);

	Int_t offset;
	volatile UInt_t * rawData;

	if (!sis3302_checkChannelNo(Module, "sis3302_readRawDataBufConfig", AdcNo)) return (kFALSE);

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

	rawData = (volatile UInt_t *) sis3302_mapAddress(Module, offset);
	if (rawData == NULL) return(0xaffec0c0);

	return (*rawData);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes raw data buffer register
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- bits
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_writeRawDataBufConfig(struct s_sis_3302 * Module, UInt_t Bits, Int_t AdcNo) {

	Int_t offset;
	volatile UInt_t * rawData;

	if (!sis3302_checkChannelNo(Module, "writeRawDataSampleLength", AdcNo)) return (kFALSE);

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

	rawData = (volatile UInt_t *) sis3302_mapAddress(Module, offset);
	if (rawData == NULL) return(kFALSE);

	*rawData = Bits;
	return(kTRUE);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads raw data sample length
//! \param[in]		Module			-- module address
//! \param[in]		AdcNo 			-- adc number
//! \return 		SampleLength	-- length
//////////////////////////////////////////////////////////////////////////////

Int_t sis3302_readRawDataSampleLength(struct s_sis_3302 * Module, Int_t AdcNo) {

	UInt_t bits;

	bits = sis3302_readRawDataBufConfig(Module, AdcNo);
	if (bits != 0xaffec0c0) bits = (bits >> 16) & 0xFFFF;
	return((Int_t) bits);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes raw data sample length
//! \param[in]		Module			-- module address
//! \param[in]		Sample			-- length
//! \param[in]		AdcNo 			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_writeRawDataSampleLength(struct s_sis_3302 * Module, Int_t Sample, Int_t AdcNo) {

	Int_t adc;
	UInt_t sl;

	if (sis_3302_checkTraceCollectionInProgress(Module, "writeRawDataSampleLength")) return(kFALSE);

	if (Sample < kSis3302RawDataSampleLengthMin || SampleLength > kSis3302RawDataSampleLengthMax) {
		sprintf(msg, "[writeRawDataSampleLength] [%s]: Sample length out of range - %d (should be in [0,%d])", Module->moduleName, Sample, kSis3302RawDataSampleLengthMax);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (Sample % 4) {
		sprintf(msg, "[writeRawDataSampleLength] [%s]:  Wrong sample alignment -  %d (should be mod 4)", Module->moduleName, Sample);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		for (adc = 0; adc < kSis3302NofAdcs; adc++) {
			if (!sis3302_writeRawDataSampleLength(Module, Sample, adc)) return(kFALSE);
		}
		return(kTRUE);
	}

	sl = sis3302_readRawDataBufConfig(Module, AdcNo);
	if (sl != 0xaffec0c0) return(kFALSE);

	sl &= 0x0000FFFF;
	sl |= (SampleLength << 16);

	return(sis3302_writeRawDataBufConfig(Module, sl, AdcNo));
}

Bool_t sis3302_writeRawDataSampleLength_db(struct s_sis_3302 * Module, Int_t AdcNo) { return(sis3302_writeRawDataSampleLength(Module, Module->rawDataSampleLength[AdcNo/2], AdcNo)); }

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads raw data start index
//! \param[in]		Module			-- module address
//! \param[in]		AdcNo 			-- adc number
//! \return 		StartIndex		-- start
//////////////////////////////////////////////////////////////////////////////

Int_t sis3302_readRawDataStartIndex(struct s_sis_3302 * Module, Int_t AdcNo) {

	UInt_t sx;

	if (!Module, ("readRawDataStartIndex", AdcNo)) return (0xaffec0c0);

	sx = sis3302_readRawDataBufConfig(Module, AdcNo);
	if (sx != 0xaffec0c0) sx &= 0xFFFF;
	return((Int_t) sx);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes raw data start index
//! \param[in]		Module			-- module address
//! \param[in]		Index			-- start
//! \param[in]		AdcNo 			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_writeRawDataStartIndex(struct s_sis_3302 * Module, Int_t Index, Int_t AdcNo);

	UInt_t sx;

	if (Index < kSis3302RawDataStartIndexMin || Index > kSis3302RawDataStartIndexMax) {
		sprintf(msg, "[writeRawDataStartIndex] [%s]: Start index out of range - %d (should be in [0,%d])", Module->moduleName, Index, kSis3302RawDataStartIndexMax);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (Index & 0x1) {
		sprintf(msg, "[writeRawDataStartIndex] [%s]:  Wrong index alignment -  %d (should be even)", Module->moduleName, Index);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			if (!sis3302_writeRawDataStartIndex(Module, Index, adc)) return(kFALSE);
		}
		return(kTRUE);
	}

	sx = sis3302_readRawDataBufConfig(Module, AdcNo);
	if (sx != 0xaffec0c0) return(kFALSE);

	sx &= 0xFFFF0000;
	sx |= Index;

	return(sis3302_writeRawDataBufConfig(Module, sx, AdcNo));
}

Bool_t sis3302_writeRawDataStartIndex_db(struct s_sis_3302 * Module, Int_t AdcNo) { return(sis3302_writeRawDataSampleLength(Module, Module->rawDataSampleStart[AdcNo/2], AdcNo)); }

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads next sample addr register
//! \param[in]		Module		-- module address
//! \param[in]		AdcNo 		-- adc number
//! \return 		Addr		-- address
//////////////////////////////////////////////////////////////////////////////

Int_t sis3302_readNextSampleAddr(struct s_sis_3302 * Module, Int_t AdcNo) {

	Int_t offset;
	volatile Int_t * samplAddr;

	if (!sis3302_checkChannelNo(Module, "readNextSampleAddr", AdcNo)) return (0xaffec0c0);

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

	samplAddr = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (samplAddr == NULL) return(0xaffec0c0);
	return (*samplAddr);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads previous sample addr register
//! \param[in]		Module		-- module address
//! \param[in]		AdcNo 		-- adc number
//! \return 		Addr		-- address
//////////////////////////////////////////////////////////////////////////////

Int_t sis3302_readPrevBankSampleAddr(struct s_sis_3302 * Module, Int_t AdcNo) {

	Int_t offset;
	volatile Int_t * samplAddr;

	if (!sis3302_checkChannelNo(Module, "readPrevBankSampleAddr", AdcNo)) return (0xaffec0c0);

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

	samplAddr = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (samplAddr == NULL) return(0xaffec0c0);
	return (*samplAddr);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads actual sample register
//! \param[in]		Module		-- module address
//! \param[in]		AdcNo 		-- adc number
//! \return 		Data		-- sample data
//////////////////////////////////////////////////////////////////////////////

UInt_t sis3302_readActualSample(struct s_sis_3302 * Module, Int_t AdcNo);

	Int_t offset;
	volatile Int_t * actSample;
	UInt_t data;

	if (!sis3302_checkChannelNo(Module, "readPrevBankSampleAddr", AdcNo)) return (0xaffec0c0);

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

	actSample = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (actSample == NULL) return(0xaffec0c0);

	data = *actSample;
	if ((AdcNo & 1) == 0) data >>= 16;
	return (data);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger setup register
//! \param[in]		Module		-- module address
//! \param[in]		AdcNo 		-- adc number
//! \return 		Data		-- data
//////////////////////////////////////////////////////////////////////////////

UInt_t sis3302_readTriggerSetup(struct s_sis_3302 * Module, Int_t AdcNo) {

	Int_t offset;
	volatile Unt_t * trigSetup;

	if (!sis3302_checkChannelNo(Module, "readTriggerSetup", AdcNo)) return (0xaffec0c0);

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

	trigSetup = (volatile UInt_t *) sis3302_mapAddress(Module, offset);
	if (trigSetup == NULL) return(0xaffec0c0);

	return (*trigSetup);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes trigger setup register
//! \param[in]		Module		-- module address
//! \param[in]		Data		-- data
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_writeTriggerSetup(struct s_sis_3302 * Module, UInt_t Data, Int_t AdcNo) {

	Int_t offset;
	volatile UInt_t * trigSetup;
	Int_t adc;

	if (!sis3302_checkChannelNo(Module, "writeTriggerSetup", AdcNo)) return (kFALSE);

	if (AdcNo == kSis3302AllAdcs) {
		for (adc = 0; adc < kSis3302NofAdcs; adc++) {
			if (!sis3302_writeTriggerSetup(Module, Data, adc)) return(kFALSE);
		}
		return(kTRUE);
	}

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

	trigSetup = (volatile UInt_t *) sis3302_mapAddress(Module, offset);
	if (trigSetup == NULL) return(kFALSE);

	*trigSetup = Data;
	return(kTRUE);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads EXTENDED trigger setup register
//! \param[in]		Module		-- module address
//! \param[in]		AdcNo 		-- adc number
//! \return 		Data		-- data
//////////////////////////////////////////////////////////////////////////////

UInt_t sis3302_readTriggerExtendedSetup(struct s_sis_3302 * Module, Int_t AdcNo) {

	Int_t offset;
	volatile UInt_t * trigSetup;

	if (!sis3302_checkChannelNo(Module, "writeTriggerExtendedSetup", AdcNo)) return (0xaffec0c0);

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

	trigSetup = (volatile UInt_t *) sis3302_mapAddress(Module, offset);
	if (trigSetup == NULL) return(kFALSE);

	return (*trigSetup);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes EXTENDED trigger setup register
//! \param[in]		Module		-- module address
//! \param[in]		Data		-- data
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_writeTriggerExtendedSetup(struct s_sis_3302 * Module, UInt_t Data, Int_t AdcNo) {

	Int_t offset;
	volatile UInt_t * trigSetup;
	Int_t adc;

	if (!sis3302_checkChannelNo(Module, "writeTriggerSetup", AdcNo)) return (kFALSE);

	if (AdcNo == kSis3302AllAdcs) {
		for (adc = 0; adc < kSis3302NofAdcs; adc++) {
			if (!sis3302_writeTriggerExtendedSetup(Module, Data, adc)) return(kFALSE);
		}
		return(kTRUE);
	}

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

	trigSetup = (volatile UInt_t *) sis3302_mapAddress(Module, offset);
	if (trigSetup == NULL) return(kFALSE);

	*trigSetup = Data;
	return(kTRUE);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger peaking and gap times
//! \param[in]		Module		-- module address
//! \param[out]		PeakGap 	-- peak & gap time
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_readTriggerPeakAndGap(struct s_sis_3302 * Module, Int_t PeakGap[], Int_t AdcNo) {

	UInt_t data, xdata;
	Int_t peak, gap, sumG;

	data = sis3302_readTriggerSetup(Module, AdcNo);
	if (data == 0xaffec0c0) return(kFALSE);
	xdata = sis3302_readTriggerExtendedSetup(Module, AdcNo);
	if (xdata == 0xaffec0c0) return(kFALSE);

	peak = data & 0xFF;
	peak |= (xdata & 0x3) >> 8;
	sumG = (data >> 8) & 0xFF;
	sumG |= xdata & 0x300;
	gap = sumG - peak;
	PeakGap[0] = peak;
	PeakGap[1] = gap;
	return(kTRUE);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets trigger peaking and gap times
//! \param[in]		Module		-- module address
//! \param[in]		PeakGap 	-- peak & gap time
//! \param[in]		AdcNo 		-- adc number
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_writeTriggerPeakAndGap(struct s_sis_3302 * Module, Int_t Peak, Int Gap, Int_t AdcNo) {

	Int_t sumG;
	Int_t adc;
	UInt_t data, xdata;

	if (sis_3302_checkTraceCollectionInProgress(Module, "writeTriggerPeakAndGap")) return(kFALSE);

	sumG = Peak + Gap;

	if (sumG > kSis3302TrigSumGMax || Peak < kSis3302TrigPeakMin || Peak > kSis3302TrigPeakMax || Gap < kSis3302TrigGapMin || Gap > kSis3302TrigGapMax) {
		sprintf(msg, "[writeTriggerPeakAndGap] [%s]: Trigger peak time / gap time mismatch - %d ... %d (peak should be in [%d,%d]), gap should be in [%d,%d], sumG=p+g <= %d",
						  Module->moduleName, Peak, Gap, kSis3302TrigPeakMin, kSis3302TrigPeakMax, kSis3302TrigGapMin, kSis3302TrigGapMax, kSis3302TrigSumGMax);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		for (adc = 0; adc < kSis3302NofAdcs; adc++) {
			if (!sis3302_writeTriggerPeakAndGap(Module, Peak, Gap, adc)) return(kFALSE);
		}
		return(kTRUE);
	}

	data = sis3302_readTriggerSetup(Module, AdcNo);
	if (data == 0xaffec0c0) return(kFALSE);
	xdata = sis3302_readTriggerExtendedSetup(Module, AdcNo);
	if (xdata == 0xaffec0c0) return(kFALSE);

	data &= 0xFFFF0000;
	data |= (sumG & 0xFF) << 8;
	data |= Peak & 0xFF;
	xdata &= 0xFFFF0000;
	xdata |= sumG & 0x300;
	xdata |= (Gap & 0x300) >> 8;
	if (!sis3302_writeTriggerSetup(Module, data, AdcNo)) return(kFALSE);
	if (!sis3302_writeTriggerExtendedSetup(Module, xdata, AdcNo)) return(kFALSE);
	return(kTRUE);
}

Bool_t sis3302_writeTriggerPeakAndGap_db(struct s_sis_3302 * Module, Int_t AdcNo) { return(sis3302_writeTriggerPeakAndGap(Module, trigPeakTime[AdcNo], trigGapTime[AdcNo], AdcNo); }
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger pulse length
//! \param[in]		Module			-- module address
//! \param[in]		AdcNo			-- adc number
//! \return 		PulseLength 	-- pulse length
//////////////////////////////////////////////////////////////////////////////

Int_t sis3302_readTriggerPulseLength(struct s_sis_3302 * Module, Int_t AdcNo) {

	UInt_t data;
	data = sis3302_readTriggerSetup(Module, data, AdcNo);
	if (data != 0xaffec0c0) data = (data >> 16) & 0xFF;
	return((Int_t) data);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets trigger pulse length
//! \param[in]		Module			-- module address
//! \param[in]		PulseLength 	-- pulse length
//! \param[in]		AdcNo 			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_writeTriggerPulseLength(struct s_sis_3302 * Module, Int_t PulseLength, Int_t AdcNo) {

	Int_t adc;
	UInt_t data;

	if (PulseLength < 0 || PulseLength > 0xFF) {
		sprintf(msg, "[writeTriggerPulseLength] [%s]:  Wrong trigger pulse length - %d (should be in [0,%d])", Module->moduleName, PulseLength, 0xFF);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++) {
			if (!sis3302_writeTriggerPulseLength(Module, PulseLength, adc)) return(kFALSE);
		}
		return(kTRUE);
	}

	data = sis3302_readTriggerSetup(Module, AdcNo);
	if (data == 0xaffec0c0) return(kFALSE);

	data &= 0xFF00FFFF;
	data |= ((PulseLength & 0xFF) << 16);
	return(sis3302_writeTriggerSetup(Module, data, AdcNo));
}

Bool_t sis3302_writeTriggerPulseLength_db(struct s_sis_3302 * Module, Int_t AdcNo) { return(sis3302_writeTriggerPulseLength(Module, Module->trigPulseLength[AdcNo], AdcNo)); }

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger internal gate length
//! \param[in]		Module			-- module address
//! \param[in]		AdcNo			-- adc number
//! \return 		Gate			-- gate length
//////////////////////////////////////////////////////////////////////////////

Int_t sis3302_readTriggerInternalGate(struct s_sis_3302 * Module, Int_t AdcNo) {

	UInt_t data;
	data = sis3302_readTriggerSetup(Module, AdcNo);
	if (data != 0xaffec0c0) data = (data >> 24) & 0x3F;
	return((Int_t) data);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets trigger internal gate length
//! \param[in]		Module			-- module address
//! \param[in]		GateLength	 	-- gate length
//! \param[in]		AdcNo 			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_writeTriggerInternalGate(struct s_sis_3302 * Module, Int_t GateLength, Int_t AdcNo) {

	Int_t adc;
	UInt_t data;

	if (GateLength < 0 || GateLength > 0x3F) {
		sprintf(msg, "[writeTriggerInternalGate] [%s]:  Wrong  internal trigger gate - %d (should be in [0,%d])", Module->moduleName, GateLength, 0x3F);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		for (adc = 0; adc < kSis3302NofAdcs; adc++) {
			if (!this->WriteTriggerInternalGate(Module, GateLength, adc)) return(kFALSE);
		}
		return(kTRUE);
	}

	data = sis3302_readTriggerSetup(Module, AdcNo);
	if (data == 0xaffec0c0) return(kFALSE);

	data &= 0xFFFFFF;
	data |= (GateLength << 24);
	return(sis3302_writeTriggerSetup(Module, data, AdcNo));
}

Bool_t sis3302_writeTriggerInternalGate_db(struct s_sis_3302 * Module, Int_t AdcNo) { return(sis3302_writeTriggerPulseLength(Module, Module->trigInternalGate[AdcNo], AdcNo)); }

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger internal delay
//! \param[in]		Module			-- module address
//! \param[in]		AdcNo			-- adc number
//! \return 		Delay			-- delay length
//////////////////////////////////////////////////////////////////////////////

Int_t sis3302_readTriggerInternalDelay(struct s_sis_3302 * Module, Int_t AdcNo) {

	UInt_t data;
	data = sis3302_readTriggerExtendedSetup(Module, AdcNo);
	if (data != 0xaffec0c0) data = (data >> 24) & 0x3F;
	return((Int_t) data);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets trigger internal delay
//! \param[in]		Module			-- module address
//! \param[in]		Delay		 	-- delay length
//! \param[in]		AdcNo 			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_writeTriggerInternalDelay(struct s_sis_3302 * Module, Int_t DelayLength, Int_t AdcNo) {

	Int_t adc;
	UInt_t data;

	if (DelayLength < 0 || DelayLength > 0x3F) {
		sprintf(msg, "[writeTriggerInternalDelay] [%s]:  Wrong internal trigger delay - %d (should be in [0,%d])", Module->moduleName, DelayLength, 0x3F);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		for (adc = 0; adc < kSis3302NofAdcs; adc++) {
			if (!this->sis3302_writeTriggerInternalDelay(Module, DelayLength, adc)) return(kFALSE);
		}
		return(kTRUE);
	}

	data = sis3302_readTriggerExtendedSetup(Module, AdcNo);
	if (data == 0xaffec0c0) return(kFALSE);

	data &= 0xFFFFFF;
	data |= (Delay << 24);
	return(sis3302_writeTriggerExtendedSetup(Module, data, AdcNo));
}

Bool_t sis3302_writeTriggerInternalDelay_db(struct s_sis_3302 * Module, Int_t AdcNo) { return(sis3302_writeTriggerInternalDelay(Module, Module->trigInternalDelay[AdcNo], AdcNo)); }

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger decimation
//! \param[in]		Module			-- module address
//! \param[in]		AdcNo 			-- adc number
//! \return 		Decimation		-- decimation
//////////////////////////////////////////////////////////////////////////////

Int_t sis3302_getTriggerDecimation(struct s_sis_3302 * Module, Int_t AdcNo) {

	UInt_t data;
	data = sis3302_readTriggerExtendedSetup(Module, AdcNo);
	if (data != 0xaffec0c0) data = (data >> 16) & 0x3;
	return((Int_t) datas);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets trigger decimation
//! \param[in]		Module			-- module address
//! \param[in]		Decimation		-- decimation
//! \param[in]		AdcNo 			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_setTriggerDecimation(struct s_sis_3302 * Module, Int_t Decimation, Int_t AdcNo) {

	Int_t adc;
	UInt_t data;

	if (Decimation < 0 || Decimation > 3) {
		sprintf(msg, "[setTriggerDecimation] [%s]:  Wrong trigger decimation - %d (should be in [0,3])", Module->moduleName, DelayLength);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		for (adc = 0; adc < kSis3302NofAdcs; adc++) {
			if (!this->SetTriggerDecimation(Module, Decimation, adc)) return(kFALSE);
		}
		return(kTRUE);
	}

	data = sis3302_readTriggerExtendedSetup(Module, AdcNo);
	if (data == 0xaffec0c0) return(kFALSE);

	data &= ~(0x3 << 16);
	data |= (Decimation << 16);
	return(sis3302_writeTriggerExtendedSetup(Module, data, AdcNo));
}

Bool_t sis3302_setTriggerDecimation_db(struct s_sis_3302 * Module, Int_t AdcNo) { return(sis3302_writeTriggerThreshold(Module, Module->trigDecimation[AdcNo], AdcNo)); }

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger threshold register
//! \param[in]		Module		-- module address
//! \param[in]		AdcNo 		-- adc number
//! \return 		Data		-- data
//////////////////////////////////////////////////////////////////////////////

UInt_t sis3302_readTriggerThreshReg(struct s_sis_3302 * Module, Int_t AdcNo) {

	Int_t offset;
	volatile UInt_t * trigThresh;

	if (!sis3302_checkChannelNo(Module, "readTriggerThreshReg", AdcNo)) return (0xaffec0c0);

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

	trigThresh = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (trigThresh == NULL) return(0xaffec0c0);

	return (*trigThresh);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes trigger threshold register
//! \param[in]		Module		-- module address
//! \param[in]		Data		-- data
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_writeTriggerThreshReg(struct s_sis_3302 * Module, UInt_t Data, Int_t AdcNo);

	UInt_t data;
	Int_t adc;
	Int_t offset;
	volatile UInt_t * trigThresh;

	if (!sis3302_checkChannelNo(Module, "readTriggerThreshReg", AdcNo)) return (kFALSE);

	if (AdcNo == kSis3302AllAdcs) {
		for (adc = 0; adc < kSis3302NofAdcs; adc++) {
			if (!sis3302_writeTriggerThreshReg((Module, Data, adc)) return(kFALSE);
		}
		return(kTRUE);
	}

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

	trigThresh = (volatile UInt_t *) sis3302_mapAddress(Module, offset);
	if (trigThresh == NULL) return(kFALSE);

	*trigThresh = Data;
	return(kTRUE);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger threshold
//! \param[in]		Module		-- module address
//! \param[in]		AdcNo 		-- adc number
//! \return 		Thresh		-- threshold
//////////////////////////////////////////////////////////////////////////////

Int_t sis3302_readTriggerThreshold(struct s_sis_3302 * Module, Int_t AdcNo);

	UInt_t data;
	data = sis3302_readTriggerThreshReg(Module, AdcNo);
	if (data != 0xaffec0c0) data & 0xFFFF;
	return((Int_t) data);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes trigger threshold
//! \param[in]		Module		-- module address
//! \param[in]		Thresh		-- threshold
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_writeTriggerThreshold(struct s_sis_3302 * Module, Int_t Thresh, Int_t AdcNo) {

	Int_t th;
	Int_t adc;
	UInt_t data;

	if (Thresh < 0 || Thresh > 0xFFFF) {
		sprintf(msg, "[setTriggerDecimation] [%s]:  Wrong trigger threshold - %d (should be in [0,%d])", Module->moduleName, Thres, 0xFFFF);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		for (adc = 0; adc < kSis3302NofAdcs; adc++) {
			if (!sis3302_writeTriggerThreshold(Module, Thresh, adc)) return(kFALSE);
		}
		return(kTRUE);
	}

	data = sis3302_readTriggerThreshReg(Module, AdcNo);
	if (data == 0xaffec0c0) return(kFALSE);

	data &= 0xFFFE0000;
	data |= (Thresh | 0x10000);
	return(sis3302_writeTriggerThreshReg(Module, data, AdcNo));
}

Bool_t sis3302_writeTriggerThreshold_db(struct s_sis_3302 * Module, Int_t AdcNo) { return(sis3302_writeTriggerThreshold(Module, Module->trigThresh[AdcNo], AdcNo)); }

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger GT bit
//! \param[in]		Module		-- module address
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_getTriggerGT(struct s_sis_3302 * Module, Int_t AdcNo) {

	UInt_t data;
	data = sis3302_readTriggerThreshReg(Module, AdcNo);
	if (data == 0xaffec0c0) return(kFALSE);
	return((data & (0x1 << 25)) != 0);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Turns trigger GT bit on or off
//! \param[in]		Module		-- module address
//! \param[in]		GTFlag		-- GT
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_setTriggerGT(struct s_sis_3302 * Module, Bool_t GTFlag, Int_t AdcNo) {

	Int_t adc;
	UInt_t data;

	if (AdcNo == kSis3302AllAdcs) {
		for (adc = 0; adc < kSis3302NofAdcs; adc++) {
			if (!sis3302_setTriggerGT(Module, GTFlag, adc)) return(kFALSE);
		}
		return(kTRUE);
	}

	data = sis3302_readTriggerThreshReg(Module, AdcNo);
	if (data == 0xaffec0c0) return(kFALSE);

	data &= ~(0x1 << 25);
	if (GTFlag) data |= (0x1 << 25);
	return(sis3302_writeTriggerThreshReg(Module, data, AdcNo));
}

Bool_t sis3302_setTriggerGT_db(struct s_sis_3302 * Module, Int_t AdcNo) { return(sis3302_setTriggerGT(Module, Module->trigGT[AdcNo], AdcNo)); }

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger OUT bit
//! \param[in]		Module		-- module address
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_getTriggerOut(struct s_sis_3302 * Module, Int_t AdcNo) {

	UInt_t data;
	data = sis3302_readTriggerThreshReg(Module, AdcNo);
	if (data == 0xaffec0c0) return(kFALSE);
	return((data & (0x1 << 26)) > 0);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Turns trigger OUT bit on or off
//! \param[in]		Module		-- module address
//! \param[in]		TrigOutFlag	-- OUT
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_setTriggerOut(struct s_sis_3302 * Module, Bool_t TrigOutFlag, Int_t AdcNo);

	Int_t adc;
	UInt_t data;

	if (AdcNo == kSis3302AllAdcs) {
		for (adc = 0; adc < kSis3302NofAdcs; adc++) {
			if (!sis3302_setTriggerOut(Module, TrigOutFlag, adc)) return(kFALSE);
		}
		return(kTRUE);
	}

	data = sis3302_readTriggerThreshReg(Module, AdcNo);
	if (data == 0xaffec0c0) return(kFALSE);
	data &= ~(0x1 << 26);
	if (TrigOutFlag) data |= (0x1 << 26);
	return(sis3302_writeTriggerThreshReg(Module, data, AdcNo));
}

Bool_t sis3302_setTriggerOut_db(struct s_sis_3302 * Module, Int_t AdcNo) { return(sis3302_setTriggerOut(Module, Module->trigOut[AdcNo], AdcNo)); }

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads energy setup register
//! \param[in]		Module		-- module address
//! \param[in]		AdcNo 		-- adc number
//! \return 		Data		-- data
//////////////////////////////////////////////////////////////////////////////

UInt_t sis3302_readEnergySetup(struct s_sis_3302 * Module, Int_t AdcNo) {

	Int_t offset;
	volatile UInt_t * setup;

	if (!sis3302_checkChannelNo(Module, "readEnergySetup", AdcNo)) return (0xaffec0c0);

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

	setup = (volatile UInt_t *) sis3302_mapAddress(Module, offset);
	if (setup == NULL) return(0xaffec0c0);

	return (*setup);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes energy setup register
//! \param[in]		Module		-- module address
//! \param[in]		Data		-- data
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_writeEnergySetup(struct s_sis_3302 * Module, UInt_t Data, Int_t AdcNo) {

	Int_t offset;
	volatile UInt_t * setup;

	if (!sis3302_checkChannelNo(Module, "writeEnergySetup", AdcNo)) return (kFALSE);

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

	setup = (volatile UInt_t *) sis3302_mapAddress(Module, offset);
	if (setup == NULL) return(kFALSE);

	*setup = Data;
	return(kTRUE);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads energy peaking and gap
//! \param[in]		Module		-- module address
//! \param[out]		PeakGap		-- peaking and gap time
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_readEnergyPeakAndGap(struct s_sis_3302 * Module, Int_t PeakGap[], Int_t AdcNo) {

	UInt_t data;
	UInt_t p1, p2;

 	data = sis3302_readEnergySetup(Module, AdcNo);
	if (data == 0xaffec0c0) return(kFALSE);

	p1 = data & 0xFF;
	p2 = (data >> 16) & 0x3;
	PeakGap[0] = (Int_t) ((p2 << 8) | p1);
	PeakGap[1] = (Int_t) ((data >> 8) & 0xFF);
	return(kTRUE);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets energy peaking and gap
//! \param[in]		Module		-- module address
//! \param[in]		PeakGap		-- peaking and gap time
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_writeEnergyPeakAndGap(struct s_sis_3302 * Module, Int_t Peak, Int_t Gap, Int_t AdcNo) {

	Int_t adc;
	UInt_t data;
	Int_t p;

	if (sis_3302_checkTraceCollectionInProgress(Module, "writeriteEnergyPeakAndGap")) return(kFALSE);

	if (Peak < kSis3302EnergyPeakMin || Peak > kSis3302EnergyPeakMax) {
		sprintf(msg, "[writeEnergyPeakAndGap] [%s]:  Energy peak time mismatch - %d (should be in [%d,%d])", Module->moduleName, Peak, kSis3302EnergyPeakMin, kSis3302EnergyPeakMax);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}
	if (Gap < kSis3302EnergyGapMin || Gap > kSis3302EnergyGapMin) {
		sprintf(msg, "[writeEnergyPeakAndGap] [%s]:  Energy gap mismatch - %d (should be in [%d,%d])", Module->moduleName, Gap, kSis3302EnergyGapMin, kSis3302EnergyPeakMax);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		for (adc = 0; adc < kSis3302NofAdcs; adc++) {
			if (!this->WriteEnergyPeakAndGap(Module, Peak, Gap, adc)) return(kFALSE);
		}
		Peak = p; Gap = g;
		return(kTRUE);
	}

	data = sis3302_readEnergySetup(Module, AdcNo);
	if (data == 0xaffec0c0) return(kFALSE);

	data &= 0xFFFC0000;
	data |= (Gap << 8);
	p = Peak & 0xFF;
	data |= Peak;
	p = (Peak >> 8) & 0x3;
	data |= (p << 16);
	return (sis3302_writeEnergySetup(Module, data, AdcNo);
}

Bool_t sis3302_writeEnergyPeakAndGap_db(struct s_sis_3302 * Module, Int_t AdcNo) { return(sis3302_writeTriggerPeakAndGap(Module, energyPeakTime[AdcNo/2], energyGapTime[AdcNo/2], AdcNo); }

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads energy decimation
//! \param[in]		Module			-- module address
//! \param[in]		AdcNo 			-- adc number
//! \return 		Decimation		-- decimation
//////////////////////////////////////////////////////////////////////////////

Int_t sis3302_getTriggerDecimation(struct s_sis_3302 * Module, Int_t AdcNo) {

	UInt_t data;
	datga = sis3302_readEnergySetup(Module, AdcNo);
	if (data != 0xaffec0c0) data = (data >> 28) & 0x3;
	return(data);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets energy decimation
//! \param[in]		Module			-- module address
//! \param[in]		Decimation		-- decimation
//! \param[in]		AdcNo 			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_setTriggerDecimation(struct s_sis_3302 * Module, Int_t Decimation, Int_t AdcNo) {

	Int_t adc;
	UInt_t data;

	if (Decimation < 0 || Decimation > 3) {
		sprintf(msg, "[setTriggerDecimation] [%s]:  Wrong energy decimation - %d (should be in [0,3])", Module->moduleName, Decimation);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		for (adc = 0; adc < kSis3302NofAdcs; adc++) {
			if (!this->SetEnergyDecimation(Module, Decimation, adc)) return(kFALSE);
		}
		return(kTRUE);
	}

	data = sis3302_readEnergySetup(Module, AdcNo);
	if (data == 0xaffec0c0) return(kFALSE);

	data &= ~(0x3 << 28);
	data |= (Decimation << 28);
	return(sis3302_writeEnergySetup(Module, data, AdcNo));
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads energy gate register
//! \param[in]		Module		-- module address
//! \param[in]		AdcNo 		-- adc number
//! \return 		Data		-- data
//////////////////////////////////////////////////////////////////////////////

UInt_t sis3302_readEnergyGateReg(struct s_sis_3302 * Module, Int_t AdcNo) {

	Int_t offset;
	volatile UInt_t * gateReg;

	if (!sis3302_checkChannelNo(Module, "readEnergySetup", AdcNo)) return (0xaffec0c0);

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

	gateReg = (volatile UInt_t *) sis3302_mapAddress(Module, offset);
	if (gateReg == NULL) return(0xaffec0c0);

	return (*gateReg);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets energy gate register
//! \param[in]		Module		-- module address
//! \param[in]		Data		-- data
//! \param[in]		AdcNo 		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_writeEnergyGateReg(struct s_sis_3302 * Module, UInt_t Data, Int_t AdcNo) {

	Int_t offset;
	volatile UInt_t * gateReg;

	if (!sis3302_checkChannelNo(Module, "readEnergySetup", AdcNo)) return (kFALSE);

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

	gateReg = (volatile UInt_t *) sis3302_mapAddress(Module, offset);
	if (gateReg == NULL) return(kFALSE);

	*gateReg = Data;
	return(kTRUE);

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads energy gate length
//! \param[in]		Module  -- module address
//! \param[in]		AdcNo              -- adc number
//! \return 		GateLength     -- gate length
//////////////////////////////////////////////////////////////////////////////

Int_t sis3302_readEnergyGateLength(struct s_sis_3302 * Module, Int_t AdcNo) {

	UInt_t data;
	data = sis3302_readEnergyGateReg(Module, AdcNo);
	if (data != 0xaffec0c0) data &= 0x1FF;
	return((Int_t) data);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets energy gate length
//! \param[in]		Module			-- module address
//! \param[in]		GateLength		-- gate length
//! \param[in]		AdcNo 			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_writeEnergyGateLength(struct s_sis_3302 * Module, Int_t GateLength, Int_t AdcNo) {

	Int_t adc;
	UInt_t data;

	if (GateLength < 0 || GateLength > 0x1FFFF) {
		sprintf(msg, "[writeEnergyGateLength] [%s]:  Wrong energy gate length - %d (should be in [0,%d])", Module->moduleName, GateLength, 0x1FFFF);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (AdcNo == kSis3302AllAdcs) {
		for (adc = 0; adc < kSis3302NofAdcs; adc++) {
			if (!sis3302_writeEnergyGateLength(Module, GateLength, adc)) return(kFALSE);
		}
		return(kTRUE);
	}

	data = sis3302_readEnergyGateReg(Module, AdcNo);
	if (data == 0xaffec0c0) return(kFALSE);

	data &= 0xFFFE0000;
	data |= GateLength;
	return(sis3302_writeEnergyGateReg(Module, data, AdcNo));
}

Bool_t sis3302_writeEnergyGateLength_db(struct s_sis_3302 * Module, Int_t AdcNo) { return(sis3302_writeEnergyGateLength(Module, energyGateLength[AdcNo/2], AdcNo)); }

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads test bits
//! \param[in]		Module		-- module address
//! \param[in]		AdcNo 		-- adc number
//! \return 		Bits		-- bits
//////////////////////////////////////////////////////////////////////////////

UInt_t sis3302_getTestBits(struct s_sis_3302 * Module, Int_t AdcNo) {

	UInt_t data;
	data = sis3302_readEnergyGateReg(Module, AdcNo);
	if (data != 0xaffec0c0) data = (data >> 28) & 0x3;
	return(data);
}

//________________________________________________________________[C FUNCTION]
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

//________________________________________________________________[C FUNCTION]
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

	volatile Int_t * sample = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (sample == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	SampleLength = *sample;
	return (!this->CheckBusTrap(Module, offset, "ReadEnergySampleLength"));
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes energy sample length
//! \param[in]		Module			-- module address
//! \param[in]		SampleLength	-- length
//! \param[in]		AdcNo 			-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::WriteEnergySampleLength(SrvVMEModule * Module, Int_t & SampleLength, Int_t AdcNo) {

	if (sis_3302_checkTraceCollectionInProgress(Module, "writeEnergySampleLength")) return(kFALSE);

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
	volatile Int_t * sample = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (sample == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*sample = SampleLength;
	if (this->CheckBusTrap(Module, offset, "WriteEnergySampleLength")) return(kFALSE);
	return(this->ReadEnergySampleLength(Module, SampleLength, (AdcNo == kSis3302AllAdcs) ? 1 : AdcNo));
}

//________________________________________________________________[C FUNCTION]
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

	volatile Int_t * tauFactor = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (tauFactor == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Tau = *tauFactor;
	return (!this->CheckBusTrap(Module, offset, "ReadTauFactor"));
}

//________________________________________________________________[C FUNCTION]
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

	volatile Int_t * tauFactor = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (tauFactor == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*tauFactor = Tau;
	if (this->CheckBusTrap(Module, offset, "WriteTauFactor")) return(kFALSE);
	return(this->ReadTauFactor(Module, Tau, (AdcNo == kSis3302AllAdcs) ? 1 : AdcNo));
}

//________________________________________________________________[C FUNCTION]
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

	volatile Int_t * sampleIndex = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (sampleIndex == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	IndexValue = *sampleIndex;
	return (!this->CheckBusTrap(Module, offset, "ReadStartIndex"));
}

//________________________________________________________________[C FUNCTION]
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


	volatile Int_t * sampleIndex = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (sampleIndex == NULL) return(kFALSE);
	gSignalTrap = kFALSE;
	*sampleIndex = IndexValue;
	if (this->CheckBusTrap(Module, offset, "WriteStartIndex")) return(kFALSE);
	return(this->ReadStartIndex(Module, IndexValue, Index, (AdcNo == kSis3302AllAdcs) ? 1 : AdcNo));
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads acquisition control data
//! \param[in]		Module		-- module address
//! \param[out]		Data		-- data
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadAcquisitionControl(SrvVMEModule * Module, Int_t & Data) {

	Int_t offset = SIS3302_ACQUISITION_CONTROL;
	volatile Int_t * ctrl = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (ctrl == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	Data = *ctrl;
	return (!this->CheckBusTrap(Module, offset, "ReadAcquisitionControl"));
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes acquisition control data
//! \param[in]		Module		-- module address
//! \param[in]		Data		-- data
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::WriteAcquisitionControl(SrvVMEModule * Module, Int_t & Data) {

	Int_t offset = SIS3302_ACQUISITION_CONTROL;
	volatile Int_t * ctrl = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (ctrl == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*ctrl = Data;
	return (!this->CheckBusTrap(Module, offset, "WriteAcquisitionControl"));
}

//________________________________________________________________[C FUNCTION]
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

//________________________________________________________________[C FUNCTION]
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

//________________________________________________________________[C FUNCTION]
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

//________________________________________________________________[C FUNCTION]
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

//________________________________________________________________[C FUNCTION]
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

//________________________________________________________________[C FUNCTION]
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

//________________________________________________________________[C FUNCTION]
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

//________________________________________________________________[C FUNCTION]
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

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Starts collection of a given number of traces
//! \param[in]		Module			-- module address
//! \param[in]		NofEvents		-- number of events/traces
//! \param[in]		AdcPatt			-- adc pattern
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::StartTraceCollection(SrvVMEModule * Module, Int_t & NofEvents, Bool_t & MultiEvent, Int_t AdcPatt) {

	Int_t nofWordsAllAdcs = 0;
	Int_t bit = 1;
	Bool_t foundAdc = kFALSE;
	for (Int_t adc = 0; adc < kSis3302NofAdcs; adc++, bit <<= 1) {
		if ((AdcPatt & bit) == 0) continue;
		foundAdc = kTRUE;
		Int_t rdl;
		if (!this->ReadRawDataSampleLength(Module, rdl, adc)) return(kFALSE);
		fRawDataLength[adc] = rdl;		// 16bit, quad aligned
		Int_t edl;
		if (!this->ReadEnergySampleLength(Module, edl, adc)) return(kFALSE);
		fEnergyDataLength[adc] = edl;	// 32bit
		fWordsPerEvent[adc] = kSis3302EventHeader
							+ rdl/2		// raw data are 16bit on input
							+ edl
							+ kSis3302EventMinMax
							+ kSis3302EventTrailer;

//		if (NofEvents == kSis3302MaxEvents) NofEvents = SIS3302_MAX_NUMBER_SAMPLES / (fWordsPerEvent[adc] * 2);	// max number of events
		if (NofEvents == kSis3302MaxEvents) NofEvents = 10000 / (fWordsPerEvent[adc] * 2);	// max number of events
		Int_t nofWords = NofEvents * fWordsPerEvent[adc];

		if (nofWords > (SIS3302_NEXT_ADC_OFFSET / sizeof(Int_t))) {
			gMrbLog->Wrn()	<< "[" << Module->GetName() << "]: Too many events - "
							<< NofEvents << " (event length is " << fWordsPerEvent[adc] << "|0x" << setbase(16) <<  fWordsPerEvent[adc] << " words, buffer size is " << SIS3302_NEXT_ADC_OFFSET << setbase(10) << " bytes)" << endl;
			gMrbLog->Flush(this->ClassName(), "StartTraceCollection");
			nofWords = (0x3FFFFC >> 1);
		}

		fNofEventsPerBuffer[adc] = nofWords / fWordsPerEvent[adc];

		fWordsPerEvent[adc] += rdl/2;	// raw data are 32bit on output!

		if (nofWords > nofWordsAllAdcs) nofWordsAllAdcs = nofWords;
	}

	if (!foundAdc) {
		gMrbLog->Err()	<< "[" << Module->GetName() << "]: At least 1 channel required" << endl;
		gMrbLog->Flush(this->ClassName(), "StartTraceCollection");
		return(kFALSE);
	}

	fEndAddrThresh = nofWordsAllAdcs;			// end addr 32bit
	Int_t maxThresh = nofWordsAllAdcs * 2;		// thresh reg has to be 16bit
	maxThresh &= 0x3FFFFC;
	this->WriteEndAddrThresh(Module, maxThresh);
	this->KeyResetSample(Module);
	this->KeyClearTimestamp(Module);
	fSampling = kSis3302KeyArmBank1Sampling;
	this->ContinueTraceCollection(Module);

	this->SetStatus(kSis3302StatusCollectingTraces);

	fMultiEvent = MultiEvent;

	return(kTRUE);
}

//________________________________________________________________[C FUNCTION]
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

//________________________________________________________________[C FUNCTION]
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

//________________________________________________________________[C FUNCTION]
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

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Resumes trace collection after pause
//! \param[in]		Module			-- module address
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ResumeTraceCollection(SrvVMEModule * Module) {

	fSampling = kSis3302KeyArmBank1Sampling;
	this->KeyResetSample(Module);
	this->SetPageRegister(Module,0);
	this->ContinueTraceCollection(Module);
	return(kTRUE);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Get trace length
//! \param[in]		Module		-- module address
//! \param[out]		Data		-- where to store length values
//! \param[in]		AdcNo		-- adc number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::GetTraceLength(SrvVMEModule * Module, TArrayI & Data, Int_t AdcNo) {

	fNofTry++;
	Data[0] = fRawDataLength[AdcNo];
	Data[1] = fEnergyDataLength[AdcNo];
	Data[2] = 0;
	Data[3] = fNofEventsPerBuffer[AdcNo];
	if (fTraceCollection && this->DataReady(Module)) {
		Int_t nextSample = this->SwitchSampling(Module, AdcNo);
		if (nextSample > 0) fNextSampleAddr[AdcNo] = nextSample;
		Data[2] = fWordsPerEvent[AdcNo];
		fTraceNo++;
	}
	return(kTRUE);
}

//________________________________________________________________[C FUNCTION]
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

	Int_t rdl = fRawDataLength[AdcNo];
	Int_t edl = fEnergyDataLength[AdcNo];
	Int_t wpt = fWordsPerEvent[AdcNo];
	Int_t wpt2 = wpt - rdl/2;

	Int_t evtStart;
	Int_t evtFirst, evtLast;
	Int_t nextSample = fNextSampleAddr[AdcNo];

	if (EventNo == kSis3302MaxEvents) {
		evtStart = 0;
		evtFirst = 0;
		Int_t nevt = (nextSample / wpt2);
		if (nevt > fNofEventsPerBuffer[AdcNo]) nevt = fNofEventsPerBuffer[AdcNo];
		evtLast = nevt - 1;
		Data.Set((evtLast + 1) * wpt + kSis3302EventPreHeader);
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

	volatile Int_t * mappedAddr = (Int_t *) sis3302_mapAddress(Module, startAddr);
	if (mappedAddr == NULL) return(kFALSE);

	gSignalTrap = kFALSE;

	Bool_t start = kTRUE;
	ofstream dump;

	Int_t k = kSis3302EventPreHeader;
	for (Int_t evtNo = evtFirst; evtNo <= evtLast; evtNo++) {

		Int_t kStart = k;
		for (Int_t i = 0; i < kSis3302EventHeader; i++, k++) Data[k] = *mappedAddr++;					// event header: 32bit words

		for (Int_t i = 0; i < rdl / 2; i++, k += 2) {			// raw data: fetch 2 samples packed in 32bit, store each in a single 32bit word
			Int_t d = *mappedAddr++;
			Data[k] = (d >> 16) & 0xFFFF;
			Data[k + 1] = d & 0xFFFF;
		}

		for (Int_t i = 0; i < edl; i++, k++) Data[k] = *mappedAddr++;			// event data: 32bit words

		Data[k] = *mappedAddr++;		// max energy
		Data[k + 1] = *mappedAddr++;	// min energy
		Data[k + 2] = *mappedAddr++;	// pile-up & trigger
		UInt_t trailer = (UInt_t) *mappedAddr++;

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
				k = kStart;
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
		k += 4;
	}

	if (fDumpTrace) dump.close();
	fDumpTrace = kFALSE;

	return(!this->CheckBusTrap(Module, startAddr, "GetTraceData"));
}

//________________________________________________________________[C FUNCTION]
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

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Switch sampling
//! \param[in]		Module		-- module address
//! \param[in]		AdcNo		-- channel number
//! \return			NextSample	-- next sampling address
//////////////////////////////////////////////////////////////////////////////

Int_t SrvSis3302::SwitchSampling(SrvVMEModule * Module, Int_t AdcNo) {
	if (!fTraceCollection) return(0);

	Int_t nextSample;
	if (fMultiEvent) {
		if (fSampling == kSis3302KeyArmBank1Sampling) {
			fSampling = kSis3302KeyArmBank2Sampling;
			fPageNo = 0;
		} else {
			fSampling = kSis3302KeyArmBank1Sampling;
			fPageNo = 4;
		}
		this->SetPageRegister(Module, fPageNo);
		this->ContinueTraceCollection(Module);
		if (!this->ReadPrevBankSampleAddr(Module, nextSample, AdcNo)) nextSample = 0;
	} else {
		this->SetPageRegister(Module, 0);
		this->KeyDisarmSample(Module);
		if (!this->ReadNextSampleAddr(Module, nextSample, AdcNo)) nextSample = 0;
		fSampling = kSis3302KeyArmBank1Sampling;
	}
	if (nextSample > 0) {
		nextSample &= 0x3FFFFC;
		nextSample >>= 1;
	}
	return(nextSample);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Set page register
//! \param[in]		Module		-- module address
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::SetPageRegister(SrvVMEModule * Module, Int_t PageNumber) {
	Int_t offset = SIS3302_ADC_MEMORY_PAGE_REGISTER;
	volatile Int_t * pageReg = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (pageReg == NULL) return(kFALSE);

	gSignalTrap = kFALSE;
	*pageReg = PageNumber;
	return (!this->CheckBusTrap(Module, offset, "SetPageRegister"));
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Read page register
//! \param[in]		Module		-- module address
//! \return 		PageNumber	-- current page number
//////////////////////////////////////////////////////////////////////////////

Int_t SrvSis3302::GetPageRegister(SrvVMEModule * Module) {
	Int_t offset = SIS3302_ADC_MEMORY_PAGE_REGISTER;
	volatile Int_t * pageReg = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (pageReg == NULL) return(-1);

	gSignalTrap = kFALSE;
 	Int_t pageNumber = *pageReg;
	return (this->CheckBusTrap(Module, offset, "SetPageRegister") ? -1 : pageNumber);
}

//________________________________________________________________[C FUNCTION]
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
	volatile Int_t * irq = (volatile Int_t *) sis3302_mapAddress(Module, offset);
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

//________________________________________________________________[C FUNCTION]
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
	volatile Int_t * irq = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (irq == NULL) return(kFALSE);

	UInt_t irqBits = Level & 0x7;
	if (RoakMode) irqBits |= 0x10;
	irqBits <<= 8;
	irqBits |= Vector & 0xFF;
	gSignalTrap = kFALSE;
	*irq = irqBits;
	return (!this->CheckBusTrap(Module, offset, "WriteIRWConfiguration"));
}

//________________________________________________________________[C FUNCTION]
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
	volatile Int_t * irq = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (irq == NULL) return(kFALSE);
	gSignalTrap = kFALSE;
	UInt_t irqBits = *irq;
	*irq = irqBits | (1 << IrqSource);
	return (!this->CheckBusTrap(Module, offset, "EnableIRQSource"));
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Enable interrupt
//! \param[in]		Module		-- module address
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::EnableIRQ(SrvVMEModule * Module) {

	Int_t offset = SIS3302_IRQ_CONFIG;
	volatile Int_t * irq = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (irq == NULL) return(kFALSE);
	gSignalTrap = kFALSE;
	UInt_t irqBits = *irq;
	*irq = irqBits | 0x800;
	return (!this->CheckBusTrap(Module, offset, "EnableIRQ"));
}

//________________________________________________________________[C FUNCTION]
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
	volatile Int_t * irq = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (irq == NULL) return(kFALSE);
	gSignalTrap = kFALSE;
	UInt_t irqBits = *irq;
	*irq = irqBits & ~(0x10000 << IrqSource);
	return (!this->CheckBusTrap(Module, offset, "DisableIRQSource"));
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Disable interrupt
//! \param[in]		Module		-- module address
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::DisableIRQ(SrvVMEModule * Module) {

	Int_t offset = SIS3302_IRQ_CONFIG;
	volatile Int_t * irq = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (irq == NULL) return(kFALSE);
	gSignalTrap = kFALSE;
	UInt_t irqBits = *irq;
	*irq = irqBits & ~0x800;
	return (!this->CheckBusTrap(Module, offset, "DisableIRQSource"));
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Read irq status
//! \param[in]		Module		-- module address
//! \param[out		IrqStatus	-- interrupt status bits
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadIRQSourceStatus(SrvVMEModule * Module, UInt_t & IrqStatus) {

	Int_t offset = SIS3302_IRQ_CONTROL;
	volatile Int_t * irq = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (irq == NULL) return(kFALSE);
	gSignalTrap = kFALSE;
	IrqStatus = (*irq >> 24) & 0xFF;
	return (this->CheckBusTrap(Module, offset, "ReadIRQSourceStatus"));
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Read irq enable status
//! \param[in]		Module		-- module address
//! \param[out		IrqStatus	-- interrupt enable bits
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t SrvSis3302::ReadIRQEnableStatus(SrvVMEModule * Module, UInt_t & IrqStatus) {

	Int_t offset = SIS3302_IRQ_CONTROL;
	volatile Int_t * irq = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (irq == NULL) return(kFALSE);
	gSignalTrap = kFALSE;
	IrqStatus = *irq & 0xFF;
	return (this->CheckBusTrap(Module, offset, "ReadIRQEnableStatus"));
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Check if adc/channel number ok
//! \param[in]		Module		-- module address
//! \param[in]		Caller		-- calling function
//! \param[in]		AdcNo		-- channel number
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_checkChannelNo(struct s_sis_3302 * Module, Char_t * Caller, Int_t AdcNo) {
	if (AdcNo != kSis3302AllAdcs && (AdcNo < 0 || AdcNo >= kSis3302NofAdcs)) {
		sprintf(msg, "[%s] [%s]: ADC number out of range - %d (should be in [0,%d])", Caller, Module->moduleName, AdcNo, (kSis3302NofAdcs - 1));
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}
	return(kTRUE);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Check if trace collection in progress
//! \param[in]		Module		-- module address
//! \param[in]		Caller		-- calling function
//! \return 		TRUE or FALSE
//////////////////////////////////////////////////////////////////////////////

Bool_t sis3302_checkTraceCollectionInProgress(struct s_sis_3302 * Module, Char_t * Caller) {
	if (sis3302_isStatus(Module, kSis3302StatusCollectingTraces)) {
		sprintf(msg, "[%s] [%s]: Can't write raw data sample length - trace collection in progress ...", Caller, Module->moduleName);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

void sis3302_setStatus(struct s_sis_3302 * Module, UInt_t Bits) { Module->status |= Bits; };
void sis3302_clearStatus(struct s_sis_3302 * Module, UInt_t Bits) { Module->status &= ~Bits; };
UInt_t sis3302_getStatus(struct s_sis_3302 * Module) { return Module->status; };
Bool_t sis3302_isStatus(struct s_sis_3302 * Module, UInt_t Bits) { return ((Module->status & Bits) != 0); };

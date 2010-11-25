/*__________________________________________________________[C IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
//! \file			Sis3302_Functions.c
//! \brief			Interface for SIS3302 ADCs
//! $Author: Marabou $
//! $Mail			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.2 $
//! $Date: 2010-11-25 09:47:57 $
////////////////////////////////////////////////////////////////////////////*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <allParam.h>
#include <ces/bmalib.h>
#include <errno.h>

#include "LwrTypes.h"

#include "sis_3302_protos.h"
#include "sis_3302_layout.h"
#include "sis_3302_database.h"


#include "root_env.h"

#include "err_mask_def.h"
#include "errnum_def.h"

char msg[256];

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Allocate database
//! \param[in]		VmeAddr			-- vme address (mapped)
//! \param[in]		BaseAddr	 	-- base address (phys)
//! \param[in]		Name			-- module name
//! \param[in]		Serial			-- MARaBOU's serial number
//! \return 		Module			-- Pointer to struct s_sis_3302
////////////////////////////////////////////////////////////////////////////*/

struct s_sis_3302 * sis_3302_alloc(ULong_t VmeAddr, volatile unsigned char * BaseAddr, char * Name, Int_t Serial)
{
	struct s_sis_3302 * Module;
	Module = (struct s_sis_3302 *) calloc(1, sizeof(struct s_sis_3302));
	if (Module != NULL) {
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
	return(Module);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Maps address to vme memory
//! \param[in]		Module			-- module database
//! \param[out]		Offset	 		-- register offset
//! \retval 		MappedAddr		--
////////////////////////////////////////////////////////////////////////////*/

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


/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads (and outputs) module info
//! \param[in]		Module			-- module address
//! \param[out]		BoardId 		-- board id
//! \param[out]		MajorVersion	-- firmware version (major)
//! \param[out]		MinorVersion	-- firmware version (minor)
//! \param[in]		PrintFlag		-- output to gMrbLog if kTRUE
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

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

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Turns user LED on/off
//! \param[in]		Module			-- module address
//! \param[in]		OnFlag			-- kTRUE if to be turned on
//! \param[in]		ChanNo 			-- channel numer (ignored))
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_setUserLED(struct s_sis_3302 * Module, Bool_t OnFlag) {

	UInt_t data;
	data = OnFlag ? 0x1 : 0x10000;
	if (!sis3302_writeControlStatus(Module, data)) return(kFALSE);
	return(kTRUE);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads dacs
//! \param[in]		Module		-- module address
//! \param[out]		DacValues	-- where to store dac values
//! \param[in]		ChanNo		-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_readDac(struct s_sis_3302 * Module, Int_t DacValues[], Int_t ChanNo) {

	Int_t firstAdc;
	Int_t lastAdc;
	Int_t wc;
	volatile Int_t * dacStatus;
	volatile Int_t * dacData;
	Int_t maxTimeout;
	Int_t timeout;
	Int_t data;
	Int_t adc;

	if (!sis3302_checkChannelNo(Module, "readDac", ChanNo)) return (kFALSE);

	if (ChanNo == kSis3302AllChans) {
		firstAdc = 0;
		lastAdc = kSis3302NofChans - 1;
	} else {
		firstAdc = ChanNo;
		lastAdc = ChanNo;
	}
	wc = lastAdc - firstAdc + 1;

	dacStatus = (volatile Int_t *) sis3302_mapAddress(Module, SIS3302_DAC_CONTROL_STATUS);
	if (dacStatus == NULL) return(kFALSE);
	dacData = (volatile Int_t *) sis3302_mapAddress(Module, SIS3302_DAC_DATA);
	if (dacData == NULL) return(kFALSE);

	maxTimeout = 5000;
	DacValues.Set(wc);
	Int_t idx = 0;
	for (adc = firstAdc; chn <= lastAdc; chn++, idx++) {
		*dacStatus = kSis3302DacCmdLoadShiftReg + (chn << 4);
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

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes dac values
//! \param[in]		Module		-- module address
//! \param[in]		DacValues	-- dac values to be written
//! \param[in]		ChanNo		-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_writeDac(struct s_sis_3302 * Module, Int_t DacValues[], Int_t ChanNo) {

	Int_t firstAdc;
	Int_t lastAdc;
	Int_t wc;
	volatile Int_t * dacStatus;
	volatile Int_t * dacData;
	Int_t maxTimeout;
	Int_t idx;
	Int_t data;
	Int_t adc;

	if (!sis3302_checkChannelNo(Module, "writeDac", ChanNo)) return (kFALSE);

	if (ChanNo == kSis3302AllChans) {
		firstAdc = 0;
		lastAdc = kSis3302NofChans - 1;
	} else {
		firstAdc = ChanNo;
		lastAdc = ChanNo;
	}
	wc = lastAdc - firstAdc + 1;

	dacStatus = (volatile Int_t *) sis3302_mapAddress(Module, SIS3302_DAC_CONTROL_STATUS);
	if (dacStatus == NULL) return(kFALSE);
	dacData = (volatile Int_t *) sis3302_mapAddress(Module, SIS3302_DAC_DATA);
	if (dacData == NULL) return(kFALSE);

	maxTimeout = 5000;
	idx = 0;
	for (adc = firstAdc; chn <= lastAdc; chn++, idx++) {
		*dacData = DacValues[idx];
		*dacStatus = kSis3302DacCmdLoadShiftReg + (chn << 4);
		timeout = 0 ;
		do {
			data = *dacStatus;
			timeout++;
		} while (((data & kSis3302DacBusy) == kSis3302DacBusy) && (timeout <  maxTimeout));
		if (timeout >=  maxTimeout) return(kFALSE);

		*dacStatus = kSis3302DacCmdLoad + (chn << 4);
		timeout = 0 ;
		do {
			data = *dacStatus;
			timeout++;
		} while (((data & kSis3302DacBusy) == kSis3302DacBusy) && (timeout <  maxTimeout));
		if (timeout >=  maxTimeout) return(kFALSE);
	}
	return(kTRUE);
}

Bool_t sis3302_writeDac_db(struct s_sis_3302 * Module, Int_t ChanNo) { return(sis3302_writeDac(Module, Module->dacValues[ChanNo], ChanNo)); }

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Executes KEY ADDR command
//! \param[in]		Module		-- module address
//! \param[in]		Key 		-- key
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

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

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads control and status register
//! \param[in]		Module			-- module address
//! \return 		Bits			-- data
////////////////////////////////////////////////////////////////////////////*/

UInt_t sis3302_readControlStatus(struct s_sis_3302 * Module) {

	Int_t offset;
	volatile UInt_t * ctrlStat;

	offset = SIS3302_CONTROL_STATUS;

	ctrlStat = (volatile UInt_t *) sis3302_mapAddress(Module, offset);
	if (ctrlStat == NULL) return(0);

	return (*ctrlStat);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes control and status register
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- data
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

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

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads event config register
//! \param[in]		Module			-- module address
//! \param[in]		ChanNo 			-- channel numer
//! \return 		Bits			-- configuration
////////////////////////////////////////////////////////////////////////////*/

UInt_t sis3302_readEventConfig(struct s_sis_3302 * Module, Int_t ChanNo);

	volatile UInt_t * evtConf;
	Int_t offset;

	if (!sis3302_checkChannelNo(Module, "readEventConfig", ChanNo)) return (0xaffec0c0);

	switch (ChanNo) {
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

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes event config register
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- configuration
//! \param[in]		ChanNo 		-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_writeEventConfig(struct s_sis_3302 * Module, UInt_t Bits, Int_t ChanNo) {

	volatile Int_t * evtConf;
	Int_t offset;

	if (!sis3302_checkChannelNo(Module, "writeEventConfig", ChanNo)) return (kFALSE);

	switch (ChanNo) {
		case kSis3302AllChans:
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

Bool_t sis3302_writeEventConfig_db(struct s_sis_3302 * Module, Int_t ChanNo) { return(sis3302_writeEventConfig(Module, Module->evtConfig[ChanNo/2], ChanNo)); }

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads EXTENDED event config register
//! \param[in]		Module			-- module address
//! \param[in]		ChanNo 			-- channel numer
//! \return 		Bits			-- configuration
////////////////////////////////////////////////////////////////////////////*/

UInt_t sis3302_readEventExtendedConfig(struct s_sis_3302 * Module, Int_t ChanNo) {

	Int_t offset;
	volatile Int_t * evtConf;

	if (!sis3302_checkChannelNo(Module, "readEventExtendedConfig", ChanNo)) return (0xaffec0c0);

	switch (ChanNo) {
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

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes EXTENDED event config register
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- configuration
//! \param[in]		ChanNo 		-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_writeEventExtendedConfig(struct s_sis_3302 * Module, UInt_t Bits, Int_t ChanNo) {

	Int_t offset;
	volatile Int_t * evtConf;

	if (!sis3302_checkChannelNo(Module, "writeEventExtendedConfig", ChanNo)) return (kFALSE);

	switch (ChanNo) {
		case kSis3302AllChans:
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

Bool_t sis3302_writeEventExtendedConfig_db(struct s_sis_3302 * Module, Int_t ChanNo) { return(sis3302_writeEventConfig(Module, Module->xEvtConfig[ChanNo/2], ChanNo)); }

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns header data
//! \param[in]		Module		-- module address
//! \param[in]		ChanNo 		-- channel numer
//! \return 		Bits		-- header data
////////////////////////////////////////////////////////////////////////////*/

UInt_t sis3302_getHeaderBits(struct s_sis_3302 * Module, Int_t ChanNo) {

	UInt_t bits;

	bits = sis3302_readEventConfig(Module, ChanNo));
	if (bits != 0xaffec0c0) {
		bits >>= 17;		// header bit #1 -> event config bit #17
		bits &= kSis3302HeaderMask;
	}
	return(bits);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Defines header data
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- bits
//! \param[in]		ChanNo 		-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_setHeaderBits(struct s_sis_3302 * Module, UInt_t Bits, Int_t ChanNo) {

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

	if (ChanNo == kSis3302AllChans) {
		for (chn = 0; chn < kSis3302NofChans; chn++) {
			if (!sis3302_setHeaderBits(Module, Bits, chn)) return(kFALSE);
		}
		return(kTRUE);
	}

	bits = sis3302_readEventConfig(Module, ChanNo));
	if (bits == 0xaffec0c0) return(kFALSE);

	bits &= 0xFFFF;
	bits |=	(Bits << 19);		// header bit #1 -> event config bit #17, bits 1 & 2 read-only
	if (!sis3302_writeEventConfig(Module, bits, ChanNo)) return(kFALSE);
	return(kTRUE);
}

Bool_t sis3302_setHeaderBits_db(struct s_sis_3302 * Module, Int_t ChanNo) { return(sis3302_setHeaderBits(Module, Module->headerBits[ChanNo/2], ChanNo)); }

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns group id (read-only)
//! \param[in]		Module		-- module address
//! \param[in]		ChanNo 		-- channel numer
//! \return 		GroupId 	-- group id
////////////////////////////////////////////////////////////////////////////*/

UInt_t sis3302_getGroupId(struct s_sis_3302 * Module, Int_t ChanNo) {

	UInt_t bits;

	bits = sis3302_readEventConfig(Module, ChanNo));
	if (bits != 0xaffec0c0) {
		bits >>= 17;
		bits &= 0x3;
	}
	return(bits);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns trigger mo*/
//! \param[in]		Module		-- module address
//! \param[in]		ChanNo 		-- channel numer
//! \return 		Bits		-- trigger mode
////////////////////////////////////////////////////////////////////////////*/

UInt_t sis3302_getTriggerMode(struct s_sis_3302 * Module, Int_t ChanNo) {

	UInt_t bits;

	bits = sis3302_readEventConfig(Module, ChanNo));
	if (bits != 0xaffec0c0) {
		if (ChanNo & 1) {
			bits >>= 10;
		} else {
			bits >>= 2;
		}
		bits &= kSis3302TriggerBoth;
	}
	return(bits);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Defines trigger mode
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- trigger mode
//! \param[in]		ChanNo 		-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_setTriggerMode(struct s_sis_3302 * Module, Unt_t Bits, Int_t ChanNo);

	UInt_t bits;
	Int_t adc;

	if (Bits < kSis3302TriggerOff || Bits > kSis3302TriggerBoth) {
		sprintf(msg, "[setTriggerMode] [%s]: Illegal trigger mode - %#lx (should be in [%d,%d])", Module->moduleName,  Bits, kSis3302TriggerOff, kSis3302TriggerBoth);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (ChanNo == kSis3302AllChans) {
		for (chn = 0; chn < kSis3302NofChans; chn++) {
			if (!sis3302_setTriggerMode(Module, Bits, chn)) return(kFALSE);
		}
		return(kTRUE);
	}

	bits = sis3302_readEventConfig(Module, ChanNo));
	if (bits == 0xaffec0c0) return(kFALSE);

	if (ChanNo & 1) {
		bits &= ~(kSis3302TriggerBoth << 10);
		bits |=	(Bits << 10);
	} else {
		bits &= ~(kSis3302TriggerBoth << 2);
		bits |=	(Bits << 2);
	}
	return(sis3302_writeEventConfig(Module, bits, ChanNo));
}

Bool_t sis3302_setTriggerMode_db(struct s_sis_3302 * Module, Int_t ChanNo) { return(sis3302_setTriggerMode(Module, Module->triggerMode[ChanNo], ChanNo)); }

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns gate mode
//! \param[in]		Module		-- module address
//! \param[out]		Bits		-- bits (0,1,2,3)
//! \param[in]		ChanNo 		-- channel numer
//! \return 		Bits		-- gate mode
////////////////////////////////////////////////////////////////////////////*/

UInt_t sis3302_getGateMode(struct s_sis_3302 * Module, Int_t ChanNo) {

	UInt_t bits;

	bits = sis3302_readEventConfig(Module, ChanNo));
	if (bits != 0xaffec0c0) {
		if (ChanNo & 1) {
			bits >>= 12;
		} else {
			bits >>= 4;
		}
		bits &= kSis3302GateBoth;
	}
	return(bits);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Defines gate mode
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- gate mode
//! \param[in]		ChanNo 		-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_setGateMode(struct s_sis_3302 * Module, UInt_t Bits, Int_t ChanNo) {

	UInt_t bits;
	Int_t adc;

	if (Bits < kSis3302GateOff || Bits > kSis3302GateBoth) {
		sprintf(msg, "[setGateMode] [%s]: Illegal gate mode - %#lx (should be in [%d,%d])", Module->moduleName,  Bits, kSis3302GateOff, kSis3302GateBoth);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (ChanNo == kSis3302AllChans) {
		for (chn = 0; chn < kSis3302NofChans; chn++) {
			if (!sis3302_setGateMode(Module, Bits, chn)) return(kFALSE);
		}
		return(kTRUE);
	}

	bits = sis3302_readEventConfig(Module, ChanNo));
	if (bits == 0xaffec0c0) return(kFALSE;

	if (ChanNo & 1) {
		bits &= ~(kSis3302GateBoth << 12);
		bits |=	(Bits << 12);
	} else {
		bits &= ~(kSis3302GateBoth << 4);
		bits |=	(Bits << 4);
	}
	return(sis3302_writeEventConfig(Module, bits, ChanNo));
}

Bool_t sis3302_setGateMode_db(struct s_sis_3302 * Module, Int_t ChanNo) { return(sis3302_setGateMode(Module, Module->gateMode[ChanNo], ChanNo)); }

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns next-neighbor trigger mode
//! \param[in]		Module		-- module address
//! \param[in]		ChanNo 		-- channel numer
//! \return 		Bits		-- trigger mode
////////////////////////////////////////////////////////////////////////////*/

UInt_t sis3302_getNextNeighborTriggerMode(struct s_sis_3302 * Module, Int_t ChanNo) {

	UInt_t bits;

	bits = sis3302_readEventExtendedConfig(Module, ChanNo));
	if (bits != 0xaffec0c0) {
		if (ChanNo & 1) {
		bits >>= 14;
			bits >>= 14;
		} else {
			bits >>= 6;
		}
		bits &= kSis3302TriggerBoth;
	}
	return(bits);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Defines next-neighbor trigger mode
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- bits (0,1,2,3)
//! \param[in]		ChanNo 		-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_setNextNeighborTriggerMode(struct s_sis_3302 * Module, Int_t Bits, Int_t ChanNo) {

	UInt_t bits;
	Int_t adc;

	if (Bits < kSis3302TriggerOff || Bits > kSis3302TriggerBoth) {
		sprintf(msg, "[setNextNeighborTriggerMode] [%s]: Illegal next-neighbor trigger mode - %#lx (should be in [%d,%d])", Module->moduleName,  Bits, kSis3302TriggerOff, kSis3302TriggerBoth);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (ChanNo == kSis3302AllChans) {
		for (chn = 0; chn < kSis3302NofChans; chn++) {
			if (!sis3302_setNextNeighborTriggerMode(Module, Bits, chn)) return(kFALSE);
		}
		return(kTRUE);
	}

	bits = sis3302_readEventExtendedConfig(Module, ChanNo));
	if (bits == 0xaffec0c0) return(kFALSE);

	if (ChanNo & 1) {
		bits &= ~(kSis3302TriggerBoth << 10);
		bits |=	(Bits << 14);
	} else {
		bits &= ~(kSis3302TriggerBoth << 2);
		bits |=	(Bits << 6);
	}
	return(sis3302_writeEventExtendedConfig(Module, bits, ChanNo));
}

Bool_t sis3302_setNextNeighborTriggerMode_db(struct s_sis_3302 * Module, Int_t ChanNo) { return(sis3302_setNextNeighborTriggerMode(Module, Module->nextNeighborTrigger[ChanNo], ChanNo)); }

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns next-neighbor gate mode
//! \param[in]		Module		-- module address
//! \param[in]		ChanNo 		-- channel numer
//! \return 		Bits		-- gate mode
////////////////////////////////////////////////////////////////////////////*/

UInt_t sis3302_getNextNeighborGateMode(struct s_sis_3302 * Module, Int_t ChanNo) {

	UInt_t bits;

	bits = sis3302_readEventExtendedConfig(Module, ChanNo));
	if (bits != 0xaffec0c0) {
		if (ChanNo & 1) {
			bits >>= 14;
		} else {
			bits >>= 6;
		}
		bits &= kSis3302GateBoth;
	}
	return(bits);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Defines next-neighbor gate mode
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- bits (0,1,2,3)
//! \param[in]		ChanNo 		-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_setNextNeighborGateMode(struct s_sis_3302 * Module, Int_t Bits, Int_t ChanNo) {

	UInt_t bits;
	Int_t adc;

	if (Bits < kSis3302GateOff || Bits > kSis3302GateBoth) {
		sprintf(msg, "[setNextNeighborTriggerMode] [%s]: Illegal next-neighbor gate mode - %#lx (should be in [%d,%d])", Module->moduleName,  Bits, kSis3302GateOff, kSis3302GateBoth);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (ChanNo == kSis3302AllChans) {
		for (chn = 0; chn < kSis3302NofChans; chn++) {
			if (!sis3302_setNextNeighborGateMode(Module, Bits, chn)) return(kFALSE);
		}
		return(kTRUE);
	}

	bits = sis3302_readEventExtendedConfig(Module, ChanNo));
	if (bits == 0xaffec0c0) return(kFALSE);

	if (ChanNo & 1) {
		bits &= ~(kSis3302GateBoth << 14);
		bits |=	(Bits << 14);
	} else {
		bits &= ~(kSis3302GateBoth << 6);
		bits |=	(Bits << 6);
	}
	return(sis3302_writeEventConfig(Module, bits, ChanNo));
}

Bool_t sis3302_setNextNeighborGateMode_db(struct s_sis_3302 * Module, Int_t ChanNo) { return(sis3302_setNextNeighborGateMode(Module, Module->nextNeighborGate[ChanNo], ChanNo)); }

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Returns trigger polarity
//! \param[in]		Module		-- module address
//! \param[in]		ChanNo 		-- channel numer
//! \return 		InvertFlag	-- kTRUE if trigger is inverted
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_polarityIsInverted(struct s_sis_3302 * Module, Int_t ChanNo) {

	UInt_t bits;
	Bool_t invertFlag;

	bits = sis3302_readEventConfig(Module, ChanNo));
	if (bits == 0xaffec0c0) return(kFALSE);

	if (ChanNo & 1) bits >>= 8;
	invertFlag = ((bits & kSis3302PolarityNegative) != 0);
	return(invertFlag);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Defines trigger polarity
//! \param[in]		Module			-- module address
//! \param[in]		InvertFlag		-- kTRUE if neg polarity
//! \param[in]		ChanNo 			-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_setPolarity(struct s_sis_3302 * Module, Bool_t InvertFlag, Int_t ChanNo) {

	Int_t adc;
	UInt_t bits;

	if (ChanNo == kSis3302AllChans) {
		Bool_t ifl;
		for (chn = 0; chn < kSis3302NofChans; chn++) {
			if (!sis3302_setPolarity(Module, InvertFlag, chn)) return(kFALSE);
		}
		return(kTRUE);
	}

	bits = sis3302_readEventConfig(Module, ChanNo));
	if (bits == 0xaffec0c0) return(kFALSE);

	if (ChanNo & 1) {
		if (InvertFlag) bits |= (kSis3302PolarityNegative << 8); else bits &= ~(kSis3302PolarityNegative << 8);
	} else {
		data &= ~1;
		if (InvertFlag) bits |= kSis3302PolarityNegative; else bits &= ~kSis3302PolarityNegative;
	}
	return(sis3302_writeEventConfig(Module, bits, ChanNo));
}

Bool_t sis3302_setPolarity_db(struct s_sis_3302 * Module, Int_t ChanNo) { return(sis3302_setPolarity(Module, Module->invertSignal[ChanNo], ChanNo)); }

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads end addr threshold register
//! \param[in]		Module		-- module address
//! \param[in]		ChanNo 		-- channel numer
//! \return 		Thresh		-- threshold
////////////////////////////////////////////////////////////////////////////*/

Int_t sis3302_readEndAddrThresh(struct s_sis_3302 * Module, Int_t ChanNo) {

	Int_t offset;
	volatile Int_t * endAddr;

	if (!sis3302_checkChannelNo(Module, "readEndAddrThresh", ChanNo)) return (kFALSE);

	switch (ChanNo) {
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

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes end addr threshold register
//! \param[in]		Module		-- module address
//! \param[in]		Thresh		-- threshold
//! \param[in]		ChanNo 		-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_writeEndAddrThresh(struct s_sis_3302 * Module, Int_t Thresh, Int_t ChanNo) {

	Int_t offset;
	volatile Int_t * endAddr;

	if (!sis3302_checkChannelNo(Module, "writeEndAddrThresh", ChanNo)) return (kFALSE);

	if (Thresh > kSis3302EndAddrThreshMax) {
		sprintf(msg, "[writeEndAddrThresh] [%s]: Threshold out of range - %#lx (max %#lx)", Module->moduleName, Thresh, kSis3302EndAddrThreshMax);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	switch (ChanNo) {
		case kSis3302AllChans:	offset = SIS3302_END_ADDRESS_THRESHOLD_ALL_ADC; break;
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

Bool_t sis3302_writeEndAddrThresh_db(struct s_sis_3302 * Module, Int_t ChanNo) { return(sis3302_writeEndAddrThresh(Module, Module->endAddrThresh[ChanNo/2], ChanNo)); }

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger register
//! \param[in]		Module		-- module address
//! \param[in]		ChanNo 		-- channel numer
//! \return 		Bits		-- trigger register
////////////////////////////////////////////////////////////////////////////*/

UInt_t sis3302_readPreTrigDelayAndGateLength(struct s_sis_3302 * Module, Int_t ChanNo) {

	Int_t offset;
	volatile Unt_t * trigReg;

	if (!sis3302_checkChannelNo(Module, "readPreTrigDelayAndGateLength", ChanNo)) return (0xaffec0c0);

	switch (ChanNo) {
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

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes trigger register
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- trigger register
//! \param[in]		ChanNo 		-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_writePreTrigDelayAndGateLength(struct s_sis_3302 * Module, UInt_t Bits, Int_t ChanNo) {

	Int_t offset;
	volatile Unt_t * trigReg;

	if (!sis3302_checkChannelNo(Module, "writePreTrigDelayAndGateLength", ChanNo)) return (kFALSE);

	switch (ChanNo) {
		case kSis3302AllChans:	offset = SIS3302_PRETRIGGER_DELAY_TRIGGERGATE_LENGTH_ALL_ADC; break;
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

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads pretrigger delay
//! \param[in]		Module		-- module address
//! \param[in]		ChanNo 		-- channel numer
//! \return 		Delay		-- delay
////////////////////////////////////////////////////////////////////////////*/

Int_t sis3302_readPreTrigDelay(struct s_sis_3302 * Module, Int_t ChanNo) {

	UInt_t delay;

	delay = sis3302_readPreTrigDelayAndGateLength(Module, ChanNo);
	if (delay != 0xaffec0c0) {
		delay >>= 16;
		delay -= 2;
		if (delay < 0) delay += 1024;
		delay &= 0xFFF;
	}
	return((Int_t) delay);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes pretrigger delay
//! \param[in]		Module		-- module address
//! \param[in]		Delay		-- delay
//! \param[in]		ChanNo 		-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_writePreTrigDelay(struct s_sis_3302 * Module, Int_t Delay, Int_t ChanNo) {

	UInt_t delay;
	Int_t adc;

	if (ChanNo == kSis3302AllChans) {
		for (chn = 0; chn < kSis3302NofChans; chn++) {
			if (!sis3302_writePreTrigDelay(Module, Delay, chn)) return(kFALSE);
		}
		return(kTRUE);
	}

	delay = sis3302_readPreTrigDelayAndGateLength(Module, ChanNo);
	if (delay == 0xaffec0c0) return(delay);

	Delay += 2;
	if (Delay >= 1024) Delay -= 1024;

	delay &= 0xFFFF;
	delay |= Delay << 16;
	return(sis3302_writePreTrigDelayAndGateLength(Module, delay, ChanNo));
}

Bool_t sis3302_writePreTrigDelay_db(struct s_sis_3302 * Module, Int_t ChanNo) { return(sis3302_writePreTrigDelay(Module, Module->pretrigDelay[ChanNo/2], ChanNo)); }

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger gate length
//! \param[in]		Module		-- module address
//! \param[in]		ChanNo 		-- channel numer
//! \return 		Gate		-- gate
////////////////////////////////////////////////////////////////////////////*/

Int_t sis3302_readTrigGateLength(struct s_sis_3302 * Module, Int_t ChanNo) {

	UInt_t gate;

	gate = sis3302_readPreTrigDelayAndGateLength(Module, ChanNo);
	if (gate != 0xaffec0c0) {
		gate &= 0xFFFF;
		gate += 1;
	}
	return((Int_t) gate);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes trigger gate length
//! \param[in]		Module		-- module address
//! \param[in]		Gate		-- gate
//! \param[in]		ChanNo 		-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_writeTrigGateLength(struct s_sis_3302 * Module, Int_t Gate, Int_t ChanNo) {

	UInt_t gate;
	Int_t adc;

	if (ChanNo == kSis3302AllChans) {
		for (chn = 0; chn < kSis3302NofChans; chn++) {
			if (!sis3302_writeTrigGateLength(Module, Gate, chn)) return(kFALSE);
		}
		return(kTRUE);
	}

	gate = sis3302_readPreTrigDelayAndGateLength(Module, ChanNo);
	if (gate == 0xaffec0c0) return(gate);

	gate &= 0xFFFF0000;
	gate |= Gate - 1;
	return(sis3302_writePreTrigDelayAndGateLength(Module, gate, ChanNo));
}

Bool_t sis3302_writeTrigGateLength_db(struct s_sis_3302 * Module, Int_t ChanNo) { return(sis3302_writeTrigGateLength(Module, Module->trigGateLength[ChanNo/2], ChanNo)); }

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads raw data buffer register
//! \param[in]		Module		-- module address
//! \param[in]		ChanNo 		-- channel numer
//! \return 		Bits		-- bits
////////////////////////////////////////////////////////////////////////////*/

UInt_t sis3302_readRawDataBufConfig(struct s_sis_3302 * Module, Int_t ChanNo);

	Int_t offset;
	volatile UInt_t * rawData;

	if (!sis3302_checkChannelNo(Module, "sis3302_readRawDataBufConfig", ChanNo)) return (kFALSE);

	switch (ChanNo) {
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

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes raw data buffer register
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- bits
//! \param[in]		ChanNo 		-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_writeRawDataBufConfig(struct s_sis_3302 * Module, UInt_t Bits, Int_t ChanNo) {

	Int_t offset;
	volatile UInt_t * rawData;

	if (!sis3302_checkChannelNo(Module, "writeRawDataSampleLength", ChanNo)) return (kFALSE);

	switch (ChanNo) {
		case kSis3302AllChans:	offset = SIS3302_RAW_DATA_BUFFER_CONFIG_ALL_ADC; break;
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

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads raw data sample length
//! \param[in]		Module			-- module address
//! \param[in]		ChanNo 			-- channel numer
//! \return 		SampleLength	-- length
////////////////////////////////////////////////////////////////////////////*/

Int_t sis3302_readRawDataSampleLength(struct s_sis_3302 * Module, Int_t ChanNo) {

	UInt_t bits;

	bits = sis3302_readRawDataBufConfig(Module, ChanNo);
	if (bits != 0xaffec0c0) bits = (bits >> 16) & 0xFFFF;
	return((Int_t) bits);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes raw data sample length
//! \param[in]		Module			-- module address
//! \param[in]		Sample			-- length
//! \param[in]		ChanNo 			-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_writeRawDataSampleLength(struct s_sis_3302 * Module, Int_t Sample, Int_t ChanNo) {

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

	if (ChanNo == kSis3302AllChans) {
		for (chn = 0; chn < kSis3302NofChans; chn++) {
			if (!sis3302_writeRawDataSampleLength(Module, Sample, chn)) return(kFALSE);
		}
		return(kTRUE);
	}

	sl = sis3302_readRawDataBufConfig(Module, ChanNo);
	if (sl != 0xaffec0c0) return(kFALSE);

	sl &= 0x0000FFFF;
	sl |= (SampleLength << 16);

	return(sis3302_writeRawDataBufConfig(Module, sl, ChanNo));
}

Bool_t sis3302_writeRawDataSampleLength_db(struct s_sis_3302 * Module, Int_t ChanNo) { return(sis3302_writeRawDataSampleLength(Module, Module->rawDataSampleLength[ChanNo/2], ChanNo)); }

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads raw data start index
//! \param[in]		Module			-- module address
//! \param[in]		ChanNo 			-- channel numer
//! \return 		StartIndex		-- start
////////////////////////////////////////////////////////////////////////////*/

Int_t sis3302_readRawDataStartIndex(struct s_sis_3302 * Module, Int_t ChanNo) {

	UInt_t sx;

	if (!Module, ("readRawDataStartIndex", ChanNo)) return (0xaffec0c0);

	sx = sis3302_readRawDataBufConfig(Module, ChanNo);
	if (sx != 0xaffec0c0) sx &= 0xFFFF;
	return((Int_t) sx);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes raw data start index
//! \param[in]		Module			-- module address
//! \param[in]		Index			-- start
//! \param[in]		ChanNo 			-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_writeRawDataStartIndex(struct s_sis_3302 * Module, Int_t Index, Int_t ChanNo);

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

	if (ChanNo == kSis3302AllChans) {
		for (Int_t chn = 0; chn < kSis3302NofChans; chn++) {
			if (!sis3302_writeRawDataStartIndex(Module, Index, chn)) return(kFALSE);
		}
		return(kTRUE);
	}

	sx = sis3302_readRawDataBufConfig(Module, ChanNo);
	if (sx != 0xaffec0c0) return(kFALSE);

	sx &= 0xFFFF0000;
	sx |= Index;

	return(sis3302_writeRawDataBufConfig(Module, sx, ChanNo));
}

Bool_t sis3302_writeRawDataStartIndex_db(struct s_sis_3302 * Module, Int_t ChanNo) { return(sis3302_writeRawDataSampleLength(Module, Module->rawDataSampleStart[ChanNo/2], ChanNo)); }

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads next sample addr register
//! \param[in]		Module		-- module address
//! \param[in]		ChanNo 		-- channel numer
//! \return 		Addr		-- address
////////////////////////////////////////////////////////////////////////////*/

Int_t sis3302_readNextSampleAddr(struct s_sis_3302 * Module, Int_t ChanNo) {

	Int_t offset;
	volatile Int_t * samplAddr;

	if (!sis3302_checkChannelNo(Module, "readNextSampleAddr", ChanNo)) return (0xaffec0c0);

	switch (ChanNo) {
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

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads previous sample addr register
//! \param[in]		Module		-- module address
//! \param[in]		ChanNo 		-- channel numer
//! \return 		Addr		-- address
////////////////////////////////////////////////////////////////////////////*/

Int_t sis3302_readPrevBankSampleAddr(struct s_sis_3302 * Module, Int_t ChanNo) {

	Int_t offset;
	volatile Int_t * samplAddr;

	if (!sis3302_checkChannelNo(Module, "readPrevBankSampleAddr", ChanNo)) return (0xaffec0c0);

	switch (ChanNo) {
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

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads actual sample register
//! \param[in]		Module		-- module address
//! \param[in]		ChanNo 		-- channel numer
//! \return 		Data		-- sample data
////////////////////////////////////////////////////////////////////////////*/

UInt_t sis3302_readActualSample(struct s_sis_3302 * Module, Int_t ChanNo);

	Int_t offset;
	volatile Int_t * actSample;
	UInt_t data;

	if (!sis3302_checkChannelNo(Module, "readPrevBankSampleAddr", ChanNo)) return (0xaffec0c0);

	switch (ChanNo) {
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
	if ((ChanNo & 1) == 0) data >>= 16;
	return (data);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger setup register
//! \param[in]		Module		-- module address
//! \param[in]		ChanNo 		-- channel numer
//! \return 		Data		-- data
////////////////////////////////////////////////////////////////////////////*/

UInt_t sis3302_readTriggerSetup(struct s_sis_3302 * Module, Int_t ChanNo) {

	Int_t offset;
	volatile Unt_t * trigSetup;

	if (!sis3302_checkChannelNo(Module, "readTriggerSetup", ChanNo)) return (0xaffec0c0);

	switch (ChanNo) {
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

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes trigger setup register
//! \param[in]		Module		-- module address
//! \param[in]		Data		-- data
//! \param[in]		ChanNo 		-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_writeTriggerSetup(struct s_sis_3302 * Module, UInt_t Data, Int_t ChanNo) {

	Int_t offset;
	volatile UInt_t * trigSetup;
	Int_t adc;

	if (!sis3302_checkChannelNo(Module, "writeTriggerSetup", ChanNo)) return (kFALSE);

	if (ChanNo == kSis3302AllChans) {
		for (chn = 0; chn < kSis3302NofChans; chn++) {
			if (!sis3302_writeTriggerSetup(Module, Data, chn)) return(kFALSE);
		}
		return(kTRUE);
	}

	switch (ChanNo) {
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

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads EXTENDED trigger setup register
//! \param[in]		Module		-- module address
//! \param[in]		ChanNo 		-- channel numer
//! \return 		Data		-- data
////////////////////////////////////////////////////////////////////////////*/

UInt_t sis3302_readTriggerExtendedSetup(struct s_sis_3302 * Module, Int_t ChanNo) {

	Int_t offset;
	volatile UInt_t * trigSetup;

	if (!sis3302_checkChannelNo(Module, "writeTriggerExtendedSetup", ChanNo)) return (0xaffec0c0);

	switch (ChanNo) {
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

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes EXTENDED trigger setup register
//! \param[in]		Module		-- module address
//! \param[in]		Data		-- data
//! \param[in]		ChanNo 		-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_writeTriggerExtendedSetup(struct s_sis_3302 * Module, UInt_t Data, Int_t ChanNo) {

	Int_t offset;
	volatile UInt_t * trigSetup;
	Int_t adc;

	if (!sis3302_checkChannelNo(Module, "writeTriggerSetup", ChanNo)) return (kFALSE);

	if (ChanNo == kSis3302AllChans) {
		for (chn = 0; chn < kSis3302NofChans; chn++) {
			if (!sis3302_writeTriggerExtendedSetup(Module, Data, chn)) return(kFALSE);
		}
		return(kTRUE);
	}

	switch (ChanNo) {
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

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger peaking and gap times
//! \param[in]		Module		-- module address
//! \param[out]		PeakGap 	-- peak & gap time
//! \param[in]		ChanNo 		-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_readTriggerPeakAndGap(struct s_sis_3302 * Module, Int_t PeakGap[], Int_t ChanNo) {

	UInt_t data, xdata;
	Int_t peak, gap, sumG;

	data = sis3302_readTriggerSetup(Module, ChanNo);
	if (data == 0xaffec0c0) return(kFALSE);
	xdata = sis3302_readTriggerExtendedSetup(Module, ChanNo);
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

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets trigger peaking and gap times
//! \param[in]		Module		-- module address
//! \param[in]		PeakGap 	-- peak & gap time
//! \param[in]		ChanNo 		-- channel numer
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_writeTriggerPeakAndGap(struct s_sis_3302 * Module, Int_t Peak, Int Gap, Int_t ChanNo) {

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

	if (ChanNo == kSis3302AllChans) {
		for (chn = 0; chn < kSis3302NofChans; chn++) {
			if (!sis3302_writeTriggerPeakAndGap(Module, Peak, Gap, chn)) return(kFALSE);
		}
		return(kTRUE);
	}

	data = sis3302_readTriggerSetup(Module, ChanNo);
	if (data == 0xaffec0c0) return(kFALSE);
	xdata = sis3302_readTriggerExtendedSetup(Module, ChanNo);
	if (xdata == 0xaffec0c0) return(kFALSE);

	data &= 0xFFFF0000;
	data |= (sumG & 0xFF) << 8;
	data |= Peak & 0xFF;
	xdata &= 0xFFFF0000;
	xdata |= sumG & 0x300;
	xdata |= (Gap & 0x300) >> 8;
	if (!sis3302_writeTriggerSetup(Module, data, ChanNo)) return(kFALSE);
	if (!sis3302_writeTriggerExtendedSetup(Module, xdata, ChanNo)) return(kFALSE);
	return(kTRUE);
}

Bool_t sis3302_writeTriggerPeakAndGap_db(struct s_sis_3302 * Module, Int_t ChanNo) { return(sis3302_writeTriggerPeakAndGap(Module, trigPeakTime[ChanNo], trigGapTime[ChanNo], ChanNo); }
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger pulse length
//! \param[in]		Module			-- module address
//! \param[in]		ChanNo			-- channel numer
//! \return 		PulseLength 	-- pulse length
////////////////////////////////////////////////////////////////////////////*/

Int_t sis3302_readTriggerPulseLength(struct s_sis_3302 * Module, Int_t ChanNo) {

	UInt_t data;
	data = sis3302_readTriggerSetup(Module, data, ChanNo);
	if (data != 0xaffec0c0) data = (data >> 16) & 0xFF;
	return((Int_t) data);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets trigger pulse length
//! \param[in]		Module			-- module address
//! \param[in]		PulseLength 	-- pulse length
//! \param[in]		ChanNo 			-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_writeTriggerPulseLength(struct s_sis_3302 * Module, Int_t PulseLength, Int_t ChanNo) {

	Int_t adc;
	UInt_t data;

	if (PulseLength < 0 || PulseLength > 0xFF) {
		sprintf(msg, "[writeTriggerPulseLength] [%s]:  Wrong trigger pulse length - %d (should be in [0,%d])", Module->moduleName, PulseLength, 0xFF);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (ChanNo == kSis3302AllChans) {
		for (Int_t chn = 0; chn < kSis3302NofChans; chn++) {
			if (!sis3302_writeTriggerPulseLength(Module, PulseLength, chn)) return(kFALSE);
		}
		return(kTRUE);
	}

	data = sis3302_readTriggerSetup(Module, ChanNo);
	if (data == 0xaffec0c0) return(kFALSE);

	data &= 0xFF00FFFF;
	data |= ((PulseLength & 0xFF) << 16);
	return(sis3302_writeTriggerSetup(Module, data, ChanNo));
}

Bool_t sis3302_writeTriggerPulseLength_db(struct s_sis_3302 * Module, Int_t ChanNo) { return(sis3302_writeTriggerPulseLength(Module, Module->trigPulseLength[ChanNo], ChanNo)); }

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger internal gate length
//! \param[in]		Module			-- module address
//! \param[in]		ChanNo			-- channel numer
//! \return 		Gate			-- gate length
////////////////////////////////////////////////////////////////////////////*/

Int_t sis3302_readTriggerInternalGate(struct s_sis_3302 * Module, Int_t ChanNo) {

	UInt_t data;
	data = sis3302_readTriggerSetup(Module, ChanNo);
	if (data != 0xaffec0c0) data = (data >> 24) & 0x3F;
	return((Int_t) data);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets trigger internal gate length
//! \param[in]		Module			-- module address
//! \param[in]		GateLength	 	-- gate length
//! \param[in]		ChanNo 			-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_writeTriggerInternalGate(struct s_sis_3302 * Module, Int_t GateLength, Int_t ChanNo) {

	Int_t adc;
	UInt_t data;

	if (GateLength < 0 || GateLength > 0x3F) {
		sprintf(msg, "[writeTriggerInternalGate] [%s]:  Wrong  internal trigger gate - %d (should be in [0,%d])", Module->moduleName, GateLength, 0x3F);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (ChanNo == kSis3302AllChans) {
		for (chn = 0; chn < kSis3302NofChans; chn++) {
			if (!this->WriteTriggerInternalGate(Module, GateLength, chn)) return(kFALSE);
		}
		return(kTRUE);
	}

	data = sis3302_readTriggerSetup(Module, ChanNo);
	if (data == 0xaffec0c0) return(kFALSE);

	data &= 0xFFFFFF;
	data |= (GateLength << 24);
	return(sis3302_writeTriggerSetup(Module, data, ChanNo));
}

Bool_t sis3302_writeTriggerInternalGate_db(struct s_sis_3302 * Module, Int_t ChanNo) { return(sis3302_writeTriggerPulseLength(Module, Module->trigInternalGate[ChanNo], ChanNo)); }

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger internal delay
//! \param[in]		Module			-- module address
//! \param[in]		ChanNo			-- channel numer
//! \return 		Delay			-- delay length
////////////////////////////////////////////////////////////////////////////*/

Int_t sis3302_readTriggerInternalDelay(struct s_sis_3302 * Module, Int_t ChanNo) {

	UInt_t data;
	data = sis3302_readTriggerExtendedSetup(Module, ChanNo);
	if (data != 0xaffec0c0) data = (data >> 24) & 0x3F;
	return((Int_t) data);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets trigger internal delay
//! \param[in]		Module			-- module address
//! \param[in]		Delay		 	-- delay length
//! \param[in]		ChanNo 			-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_writeTriggerInternalDelay(struct s_sis_3302 * Module, Int_t DelayLength, Int_t ChanNo) {

	Int_t adc;
	UInt_t data;

	if (DelayLength < 0 || DelayLength > 0x3F) {
		sprintf(msg, "[writeTriggerInternalDelay] [%s]:  Wrong internal trigger delay - %d (should be in [0,%d])", Module->moduleName, DelayLength, 0x3F);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (ChanNo == kSis3302AllChans) {
		for (chn = 0; chn < kSis3302NofChans; chn++) {
			if (!this->sis3302_writeTriggerInternalDelay(Module, DelayLength, chn)) return(kFALSE);
		}
		return(kTRUE);
	}

	data = sis3302_readTriggerExtendedSetup(Module, ChanNo);
	if (data == 0xaffec0c0) return(kFALSE);

	data &= 0xFFFFFF;
	data |= (Delay << 24);
	return(sis3302_writeTriggerExtendedSetup(Module, data, ChanNo));
}

Bool_t sis3302_writeTriggerInternalDelay_db(struct s_sis_3302 * Module, Int_t ChanNo) { return(sis3302_writeTriggerInternalDelay(Module, Module->trigInternalDelay[ChanNo], ChanNo)); }

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger decimation
//! \param[in]		Module			-- module address
//! \param[in]		ChanNo 			-- channel numer
//! \return 		Decimation		-- decimation
////////////////////////////////////////////////////////////////////////////*/

Int_t sis3302_getTriggerDecimation(struct s_sis_3302 * Module, Int_t ChanNo) {

	UInt_t data;
	data = sis3302_readTriggerExtendedSetup(Module, ChanNo);
	if (data != 0xaffec0c0) data = (data >> 16) & 0x3;
	return((Int_t) datas);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets trigger decimation
//! \param[in]		Module			-- module address
//! \param[in]		Decimation		-- decimation
//! \param[in]		ChanNo 			-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_setTriggerDecimation(struct s_sis_3302 * Module, Int_t Decimation, Int_t ChanNo) {

	Int_t adc;
	UInt_t data;

	if (Decimation < 0 || Decimation > 3) {
		sprintf(msg, "[setTriggerDecimation] [%s]:  Wrong trigger decimation - %d (should be in [0,3])", Module->moduleName, DelayLength);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (ChanNo == kSis3302AllChans) {
		for (chn = 0; chn < kSis3302NofChans; chn++) {
			if (!this->SetTriggerDecimation(Module, Decimation, chn)) return(kFALSE);
		}
		return(kTRUE);
	}

	data = sis3302_readTriggerExtendedSetup(Module, ChanNo);
	if (data == 0xaffec0c0) return(kFALSE);

	data &= ~(0x3 << 16);
	data |= (Decimation << 16);
	return(sis3302_writeTriggerExtendedSetup(Module, data, ChanNo));
}

Bool_t sis3302_setTriggerDecimation_db(struct s_sis_3302 * Module, Int_t ChanNo) { return(sis3302_writeTriggerThreshold(Module, Module->trigDecimation[ChanNo], ChanNo)); }

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger threshold register
//! \param[in]		Module		-- module address
//! \param[in]		ChanNo 		-- channel numer
//! \return 		Data		-- data
////////////////////////////////////////////////////////////////////////////*/

UInt_t sis3302_readTriggerThreshReg(struct s_sis_3302 * Module, Int_t ChanNo) {

	Int_t offset;
	volatile UInt_t * trigThresh;

	if (!sis3302_checkChannelNo(Module, "readTriggerThreshReg", ChanNo)) return (0xaffec0c0);

	switch (ChanNo) {
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

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes trigger threshold register
//! \param[in]		Module		-- module address
//! \param[in]		Data		-- data
//! \param[in]		ChanNo 		-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_writeTriggerThreshReg(struct s_sis_3302 * Module, UInt_t Data, Int_t ChanNo);

	UInt_t data;
	Int_t adc;
	Int_t offset;
	volatile UInt_t * trigThresh;

	if (!sis3302_checkChannelNo(Module, "readTriggerThreshReg", ChanNo)) return (kFALSE);

	if (ChanNo == kSis3302AllChans) {
		for (chn = 0; chn < kSis3302NofChans; chn++) {
			if (!sis3302_writeTriggerThreshReg((Module, Data, chn)) return(kFALSE);
		}
		return(kTRUE);
	}

	switch (ChanNo) {
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

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger threshold
//! \param[in]		Module		-- module address
//! \param[in]		ChanNo 		-- channel numer
//! \return 		Thresh		-- threshold
////////////////////////////////////////////////////////////////////////////*/

Int_t sis3302_readTriggerThreshold(struct s_sis_3302 * Module, Int_t ChanNo);

	UInt_t data;
	data = sis3302_readTriggerThreshReg(Module, ChanNo);
	if (data != 0xaffec0c0) data & 0xFFFF;
	return((Int_t) data);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes trigger threshold
//! \param[in]		Module		-- module address
//! \param[in]		Thresh		-- threshold
//! \param[in]		ChanNo 		-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_writeTriggerThreshold(struct s_sis_3302 * Module, Int_t Thresh, Int_t ChanNo) {

	Int_t th;
	Int_t adc;
	UInt_t data;

	if (Thresh < 0 || Thresh > 0xFFFF) {
		sprintf(msg, "[setTriggerDecimation] [%s]:  Wrong trigger threshold - %d (should be in [0,%d])", Module->moduleName, Thres, 0xFFFF);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (ChanNo == kSis3302AllChans) {
		for (chn = 0; chn < kSis3302NofChans; chn++) {
			if (!sis3302_writeTriggerThreshold(Module, Thresh, chn)) return(kFALSE);
		}
		return(kTRUE);
	}

	data = sis3302_readTriggerThreshReg(Module, ChanNo);
	if (data == 0xaffec0c0) return(kFALSE);

	data &= 0xFFFE0000;
	data |= (Thresh | 0x10000);
	return(sis3302_writeTriggerThreshReg(Module, data, ChanNo));
}

Bool_t sis3302_writeTriggerThreshold_db(struct s_sis_3302 * Module, Int_t ChanNo) { return(sis3302_writeTriggerThreshold(Module, Module->trigThresh[ChanNo], ChanNo)); }

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger GT bit
//! \param[in]		Module		-- module address
//! \param[in]		ChanNo 		-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_getTriggerGT(struct s_sis_3302 * Module, Int_t ChanNo) {

	UInt_t data;
	data = sis3302_readTriggerThreshReg(Module, ChanNo);
	if (data == 0xaffec0c0) return(kFALSE);
	return((data & (0x1 << 25)) != 0);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Turns trigger GT bit on or off
//! \param[in]		Module		-- module address
//! \param[in]		GTFlag		-- GT
//! \param[in]		ChanNo 		-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_setTriggerGT(struct s_sis_3302 * Module, Bool_t GTFlag, Int_t ChanNo) {

	Int_t adc;
	UInt_t data;

	if (ChanNo == kSis3302AllChans) {
		for (chn = 0; chn < kSis3302NofChans; chn++) {
			if (!sis3302_setTriggerGT(Module, GTFlag, chn)) return(kFALSE);
		}
		return(kTRUE);
	}

	data = sis3302_readTriggerThreshReg(Module, ChanNo);
	if (data == 0xaffec0c0) return(kFALSE);

	data &= ~(0x1 << 25);
	if (GTFlag) data |= (0x1 << 25);
	return(sis3302_writeTriggerThreshReg(Module, data, ChanNo));
}

Bool_t sis3302_setTriggerGT_db(struct s_sis_3302 * Module, Int_t ChanNo) { return(sis3302_setTriggerGT(Module, Module->trigGT[ChanNo], ChanNo)); }

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads trigger OUT bit
//! \param[in]		Module		-- module address
//! \param[in]		ChanNo 		-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_getTriggerOut(struct s_sis_3302 * Module, Int_t ChanNo) {

	UInt_t data;
	data = sis3302_readTriggerThreshReg(Module, ChanNo);
	if (data == 0xaffec0c0) return(kFALSE);
	return((data & (0x1 << 26)) > 0);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Turns trigger OUT bit on or off
//! \param[in]		Module		-- module address
//! \param[in]		TrigOutFlag	-- OUT
//! \param[in]		ChanNo 		-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_setTriggerOut(struct s_sis_3302 * Module, Bool_t TrigOutFlag, Int_t ChanNo);

	Int_t adc;
	UInt_t data;

	if (ChanNo == kSis3302AllChans) {
		for (chn = 0; chn < kSis3302NofChans; chn++) {
			if (!sis3302_setTriggerOut(Module, TrigOutFlag, chn)) return(kFALSE);
		}
		return(kTRUE);
	}

	data = sis3302_readTriggerThreshReg(Module, ChanNo);
	if (data == 0xaffec0c0) return(kFALSE);
	data &= ~(0x1 << 26);
	if (TrigOutFlag) data |= (0x1 << 26);
	return(sis3302_writeTriggerThreshReg(Module, data, ChanNo));
}

Bool_t sis3302_setTriggerOut_db(struct s_sis_3302 * Module, Int_t ChanNo) { return(sis3302_setTriggerOut(Module, Module->trigOut[ChanNo], ChanNo)); }

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads energy setup register
//! \param[in]		Module		-- module address
//! \param[in]		ChanNo 		-- channel numer
//! \return 		Data		-- data
////////////////////////////////////////////////////////////////////////////*/

UInt_t sis3302_readEnergySetup(struct s_sis_3302 * Module, Int_t ChanNo) {

	Int_t offset;
	volatile UInt_t * setup;

	if (!sis3302_checkChannelNo(Module, "readEnergySetup", ChanNo)) return (0xaffec0c0);

	switch (ChanNo) {
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

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes energy setup register
//! \param[in]		Module		-- module address
//! \param[in]		Data		-- data
//! \param[in]		ChanNo 		-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_writeEnergySetup(struct s_sis_3302 * Module, UInt_t Data, Int_t ChanNo) {

	Int_t offset;
	volatile UInt_t * setup;

	if (!sis3302_checkChannelNo(Module, "writeEnergySetup", ChanNo)) return (kFALSE);

	switch (ChanNo) {
		case kSis3302AllChans:
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

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads energy peaking and gap
//! \param[in]		Module		-- module address
//! \param[out]		PeakGap		-- peaking and gap time
//! \param[in]		ChanNo 		-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_readEnergyPeakAndGap(struct s_sis_3302 * Module, Int_t PeakGap[], Int_t ChanNo) {

	UInt_t data;
	UInt_t p1, p2;

 	data = sis3302_readEnergySetup(Module, ChanNo);
	if (data == 0xaffec0c0) return(kFALSE);

	p1 = data & 0xFF;
	p2 = (data >> 16) & 0x3;
	PeakGap[0] = (Int_t) ((p2 << 8) | p1);
	PeakGap[1] = (Int_t) ((data >> 8) & 0xFF);
	return(kTRUE);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets energy peaking and gap
//! \param[in]		Module		-- module address
//! \param[in]		PeakGap		-- peaking and gap time
//! \param[in]		ChanNo 		-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_writeEnergyPeakAndGap(struct s_sis_3302 * Module, Int_t Peak, Int_t Gap, Int_t ChanNo) {

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

	if (ChanNo == kSis3302AllChans) {
		for (chn = 0; chn < kSis3302NofChans; chn++) {
			if (!this->WriteEnergyPeakAndGap(Module, Peak, Gap, chn)) return(kFALSE);
		}
		Peak = p; Gap = g;
		return(kTRUE);
	}

	data = sis3302_readEnergySetup(Module, ChanNo);
	if (data == 0xaffec0c0) return(kFALSE);

	data &= 0xFFFC0000;
	data |= (Gap << 8);
	p = Peak & 0xFF;
	data |= Peak;
	p = (Peak >> 8) & 0x3;
	data |= (p << 16);
	return (sis3302_writeEnergySetup(Module, data, ChanNo);
}

Bool_t sis3302_writeEnergyPeakAndGap_db(struct s_sis_3302 * Module, Int_t ChanNo) { return(sis3302_writeTriggerPeakAndGap(Module, Module->energyPeakTime[ChanNo/2], energyGapTime[ChanNo/2], ChanNo); }

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads energy decimation
//! \param[in]		Module			-- module address
//! \param[in]		ChanNo 			-- channel numer
//! \return 		Decimation		-- decimation
////////////////////////////////////////////////////////////////////////////*/

Int_t sis3302_getEnergyDecimation(struct s_sis_3302 * Module, Int_t ChanNo) {

	UInt_t data;
	datga = sis3302_readEnergySetup(Module, ChanNo);
	if (data != 0xaffec0c0) data = (data >> 28) & 0x3;
	return(data);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets energy decimation
//! \param[in]		Module			-- module address
//! \param[in]		Decimation		-- decimation
//! \param[in]		ChanNo 			-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_setEnergyDecimation(struct s_sis_3302 * Module, Int_t Decimation, Int_t ChanNo) {

	Int_t adc;
	UInt_t data;

	if (Decimation < 0 || Decimation > 3) {
		sprintf(msg, "[setEnergyDecimation] [%s]:  Wrong energy decimation - %d (should be in [0,3])", Module->moduleName, Decimation);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (ChanNo == kSis3302AllChans) {
		for (chn = 0; chn < kSis3302NofChans; chn++) {
			if (!sis3302_setEnergyDecimation(Module, Decimation, chn)) return(kFALSE);
		}
		return(kTRUE);
	}

	data = sis3302_readEnergySetup(Module, ChanNo);
	if (data == 0xaffec0c0) return(kFALSE);

	data &= ~(0x3 << 28);
	data |= (Decimation << 28);
	return(sis3302_writeEnergySetup(Module, data, ChanNo));
}

Bool_t sis3302_setEnergyDecimation_db(struct s_sis_3302 * Module, Int_t ChanNo) { return(sis3302_setEnergyDecimation(Module, Module->energyDecimation[ChanNo/2], ChanNo)); };

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads energy gate register
//! \param[in]		Module		-- module address
//! \param[in]		ChanNo 		-- channel numer
//! \return 		Data		-- data
////////////////////////////////////////////////////////////////////////////*/

UInt_t sis3302_readEnergyGateReg(struct s_sis_3302 * Module, Int_t ChanNo) {

	Int_t offset;
	volatile UInt_t * gateReg;

	if (!sis3302_checkChannelNo(Module, "readEnergySetup", ChanNo)) return (0xaffec0c0);

	switch (ChanNo) {
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

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets energy gate register
//! \param[in]		Module		-- module address
//! \param[in]		Data		-- data
//! \param[in]		ChanNo 		-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_writeEnergyGateReg(struct s_sis_3302 * Module, UInt_t Data, Int_t ChanNo) {

	Int_t offset;
	volatile UInt_t * gateReg;

	if (!sis3302_checkChannelNo(Module, "readEnergySetup", ChanNo)) return (kFALSE);

	switch (ChanNo) {
		case kSis3302AllChans:
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

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads energy gate length
//! \param[in]		Module  -- module address
//! \param[in]		ChanNo              -- channel numer
//! \return 		GateLength     -- gate length
////////////////////////////////////////////////////////////////////////////*/

Int_t sis3302_readEnergyGateLength(struct s_sis_3302 * Module, Int_t ChanNo) {

	UInt_t data;
	data = sis3302_readEnergyGateReg(Module, ChanNo);
	if (data != 0xaffec0c0) data &= 0x1FF;
	return((Int_t) data);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets energy gate length
//! \param[in]		Module			-- module address
//! \param[in]		GateLength		-- gate length
//! \param[in]		ChanNo 			-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_writeEnergyGateLength(struct s_sis_3302 * Module, Int_t GateLength, Int_t ChanNo) {

	Int_t adc;
	UInt_t data;

	if (GateLength < 0 || GateLength > 0x1FFFF) {
		sprintf(msg, "[writeEnergyGateLength] [%s]:  Wrong energy gate length - %d (should be in [0,%d])", Module->moduleName, GateLength, 0x1FFFF);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (ChanNo == kSis3302AllChans) {
		for (chn = 0; chn < kSis3302NofChans; chn++) {
			if (!sis3302_writeEnergyGateLength(Module, GateLength, chn)) return(kFALSE);
		}
		return(kTRUE);
	}

	data = sis3302_readEnergyGateReg(Module, ChanNo);
	if (data == 0xaffec0c0) return(kFALSE);

	data &= 0xFFFE0000;
	data |= GateLength;
	return(sis3302_writeEnergyGateReg(Module, data, ChanNo));
}

Bool_t sis3302_writeEnergyGateLength_db(struct s_sis_3302 * Module, Int_t ChanNo) { return(sis3302_writeEnergyGateLength(Module, Module->energyGateLength[ChanNo/2], ChanNo)); }

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads test bits
//! \param[in]		Module		-- module address
//! \param[in]		ChanNo 		-- channel numer
//! \return 		Bits		-- bits
////////////////////////////////////////////////////////////////////////////*/

UInt_t sis3302_getTestBits(struct s_sis_3302 * Module, Int_t ChanNo) {

	UInt_t data;
	data = sis3302_readEnergyGateReg(Module, ChanNo);
	if (data != 0xaffec0c0) data = (data >> 28) & 0x3;
	return(data);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes test bits
//! \param[in]		Module		-- module address
//! \param[in]		Bits		-- bits
//! \param[in]		ChanNo 		-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_setTestBits(struct s_sis_3302 * Module, UInt_t Bits, Int_t ChanNo) {

	Int_t b;
	UInt_t data;

	if (Bits < 0 || Bits > 0x3) {
		sprintf(msg, "[setTestBits] [%s]:  Wrong test bits - %#x (should be in [0, 0x3])", Module->moduleName, Bits);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (ChanNo == kSis3302AllChans) {
		for (Int_t chn = 0; chn < kSis3302NofChans; chn++) {
			b = Bits;
			if (!this->SetTestBits(Module, b, chn)) return(kFALSE);
		}
		Bits = b;
		return(kTRUE);
	}

	Data = sis3302_readEnergyGateReg(Module, ChanNo);
	if (data == 0xaffec0c0) return(kFALSE);
	data &= 0xFFFFFFF;
	data |= (Bits << 28);
	return(sis3302_writeEnergyGateReg(Module, data, ChanNo));
}

Bool_t sis3302_setTestBits_db(struct s_sis_3302 * Module, Int_t ChanNo) { return(sis3302_setTestBits(Module, Module->energyTestBits[ChanNo/2], ChanNo)); }

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads energy sample length
//! \param[in]		Module			-- module address
//! \param[in]		ChanNo 			-- channel numer
//! \return 		SampleLength	-- length
////////////////////////////////////////////////////////////////////////////*/

Int_t sis3302_readEnergySampleLength(struct s_sis_3302 * Module, Int_t ChanNo) {

	Int_t offset;
	volatile Int_t * sample;

	if (!sis3302_checkChannelNo(Module, "readEnergySampleLength", ChanNo)) return (0xaffec0c0);

	switch (ChanNo) {
		case 0:
		case 1: 	offset = SIS3302_ENERGY_SAMPLE_LENGTH_ADC12; break;
		case 2:
		case 3: 	offset = SIS3302_ENERGY_SAMPLE_LENGTH_ADC34; break;
		case 4:
		case 5: 	offset = SIS3302_ENERGY_SAMPLE_LENGTH_ADC56; break;
		case 6:
		case 7: 	offset = SIS3302_ENERGY_SAMPLE_LENGTH_ADC78; break;
	}

	sample = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (sample == NULL) return(0xaffec0c0);

	return(*sample);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes energy sample length
//! \param[in]		Module			-- module address
//! \param[in]		SampleLength	-- length
//! \param[in]		ChanNo 			-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_writeEnergySampleLength(struct s_sis_3302 * Module, Int_t SampleLength, Int_t ChanNo) {

	Int_t offset;
	volatile Int_t * sample;

	if (sis_3302_checkTraceCollectionInProgress(Module, "writeEnergySampleLength")) return(kFALSE);
	if (!sis3302_checkChannelNo(Module, "writeEnergySampleLength", ChanNo)) return (kFALSE);

	switch (ChanNo) {
		case kSis3302AllChans:
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
	sample = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (sample == NULL) return(kFALSE);

	*sample = SampleLength;
	return(kTRUE);
}

Bool_t sis3302_writeEnergySampleLength_db(struct s_sis_3302 * Module, Int_t ChanNo) { return(sis3302_writeEnergySampleLength(Module, Module->energySampleLength[ChanNo/2], ChanNo)); }

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads tau value
//! \param[in]		Module		-- module address
//! \param[in]		ChanNo 		-- channel numer
//! \return 		Tau 		-- tau value
////////////////////////////////////////////////////////////////////////////*/

Int_t sis3302_readTauFactor(struct s_sis_3302 * Module, Int_t ChanNo) {

	Int_t offset;
	volatile Int_t * tauFactor;

	if (!sis3302_checkChannelNo(Module, "readTauFactor", ChanNo)) return (0xaffec0c0);

	switch (ChanNo) {
		case 0: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC1; break;
		case 1: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC2; break;
		case 2: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC3; break;
		case 3: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC4; break;
		case 4: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC5; break;
		case 5: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC6; break;
		case 6: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC7; break;
		case 7: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC8; break;
	}

	tauFactor = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (tauFactor == NULL) return(0xaffec0c0);

	return (*tauFactor);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes tau value
//! \param[in]		Module		-- module address
//! \param[in]		Tau 		-- tau value
//! \param[in]		ChanNo 		-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_writeTauFactor(struct s_sis_3302 * Module, Int_t Tau, Int_t ChanNo) {

	Int_t offset;
	volatile Int_t * tauFactor;

	if (!sis3302_checkChannelNo(Module, "writeTauFactor", ChanNo)) return (kFALSE);

	if (Tau < kSis3302EnergyTauMin || Tau > kSis3302EnergyTauMax) {
		sprintf(msg, "[writeTauFactor] [%s]:  Wrong tau factor - %d (should be in [0, " << kSis3302EnergyTauMax << "])", Module->moduleName, Tau);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (ChanNo == kSis3302AllChans) {
		for (Int_t chn = 0; chn < kSis3302NofChans; chn++) {
			if (!sis3302_writeTauFactor(Module, Tau, chn)) return(kFALSE);
		}
		return(kTRUE);
	}

	switch (ChanNo) {
		case 0: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC1; break;
		case 1: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC2; break;
		case 2: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC3; break;
		case 3: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC4; break;
		case 4: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC5; break;
		case 5: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC6; break;
		case 6: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC7; break;
		case 7: 	offset = SIS3302_ENERGY_TAU_FACTOR_ADC8; break;
	}

	tauFactor = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (tauFactor == NULL) return(kFALSE);

	*tauFactor = Tau;
	return(kTRUE);
}

Bool_t sis3302_writeTauFactor_db(struct s_sis_3302 * Module, Int_t ChanNo) { return(sis3302_writeTauFactor(Module, Module->energyTauFactor[ChanNo], ChanNo)); }

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads sample start index
//! \param[in]		Module			-- module address
//! \param[in]		IdxNo			-- index
//! \param[in]		ChanNo 			-- channel numer
//! \return 		IdxVal			-- start index value
////////////////////////////////////////////////////////////////////////////*/

Int_t sis3302_readStartIndex(struct s_sis_3302 * Module, Int_t IdxNo, Int_t ChanNo) {}

	Int_t offset;
	volatile Int_t * sampleIndex;

	if (!sis3302_checkChannelNo(Module, "readStartIndex", ChanNo)) return (0xaffec0c0);

	switch (Index) {
		case 0:
			{
				switch (ChanNo) {
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
				switch (ChanNo) {
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
				switch (ChanNo) {
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
				sprintf(msg, "[readStartIndex] [%s]:  Start index out of range - %d (should be in [0,2])", Module->moduleName, Index);
				f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
				return(0xaffec0c0);
			}
	}

	sampleIndex = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (sampleIndex == NULL) return(0xaffec0c0);

	return(*sampleIndex);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes sample start index
//! \param[in]		Module			-- module address
//! \param[in]		IdxVal			-- value
//! \param[in]		IdxNo			-- index
//! \param[in]		ChanNo 			-- channel numer
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_writeStartIndex(struct s_sis_3302 * Module, Int_t IdxVal, Int_t IdxNo, Int_t ChanNo) {

	Int_t offset;
  volatile Int_t * sampleIndex;

	if (!sis3302_checkChannelNo(Module, "writeStartIndex", ChanNo)) return (kFALSE);

	if (IdxVal < 0 || IdxVal > 0x7FF) {
		sprintf(msg, "[writeStartIndex] [%s]:  Wrong sample index value - %d (should be in [0,2047])", Module->moduleName, IdxVal);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (IdxNo < 0 || IdxNo > 2) {
		sprintf(msg, "[writeStartIndex] [%s]:  Start index out of range - %d (should be in [0,2])", Module->moduleName, IdxNo);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}

	if (ChanNo == kSis3302AllChans) {
		for (Int_t chn = 0; chn < kSis3302NofChans; chn++) {
			if (!sis3302_writeStartIndex(Module, IdxVal, IdxNo, chn)) return(kFALSE);
		}
		return(kTRUE);
	}

	switch (Index) {
		case 0:
			{
				switch (ChanNo) {
					case kSis3302AllChans:
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
				switch (ChanNo) {
					case kSis3302AllChans:
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
				switch (ChanNo) {
					case kSis3302AllChans:
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


	sampleIndex = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (sampleIndex == NULL) return(kFALSE);
	*sampleIndex = IdxVal;
	return(kTRUE);
}

Bool_t sis3302_writeStartIndex_db(struct s_sis_3302 * Module, Int_t IdxNo, Int_t ChanNo) { return(sis3302_writeStartIndex(Module, Module->energySampleStart[IdxNo][ChanNo/2], ChanNo)); }

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads acquisition control data
//! \param[in]		Module		-- module address
//! \return 		Data		-- data
////////////////////////////////////////////////////////////////////////////*/

UInt_t sis3302_readAcquisitionControl(struct s_sis_3302 * Module) {

	Int_t offset;
	volatile Int_t * ctrl;

	offset = SIS3302_ACQUISITION_CONTROL;
	ctrl = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (ctrl == NULL) return(0xaffec0c0);

	return (*ctrl);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Writes acquisition control data
//! \param[in]		Module		-- module address
//! \param[in]		Data		-- data
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_writeAcquisitionControl(struct s_sis_3302 * Module, UInt_t Data) {

	Int_t offset;
	volatile Int_t * ctrl;

	offset = SIS3302_ACQUISITION_CONTROL;
	ctrl = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (ctrl == NULL) return(kFALSE);

	*ctrl = Data;
	return (kTRUE);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads clock source
//! \param[in]		Module			-- module address
//! \return 		ClockSource		-- clock source
////////////////////////////////////////////////////////////////////////////*/

Int_t sis3302_getClockSource(struct s_sis_3302 * Module) {
	Int_t data;
	data = sis3302_readAcquisitionControl(Module);
	if (data != 0xaffec0c0) data = (data >> 12) & 0x7;
	return(data);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets clock source
//! \param[in]		Module			-- module address
//! \param[in]		ClockSource		-- clock source
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_setClockSource(struct s_sis_3302 * Module, Int_t ClockSource);

	UInt_t data;

	if (ClockSource < 0 || ClockSource > 7) {
		sprintf(msg, "[setClockSource] [%s]:  Clock source out of range - %d (should be in [0,7])", Module->moduleName, ClockSource);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}
	data = (0x7 << 28);			// clear all bits
	if (!sis3302_writeAcquisitionControl(Module, data)) return(kFALSE);
	data = (ClockSource & 0x7) << 12;	// set bits
	return(sis3302_writeAcquisitionControl(Module, data));
}

Bool_t sis3302_setClockSource_db(struct s_sis_3302 * Module) { return(sis3302_setClockSource(Module. Module->clockSource)); };

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads lemo-in mode
//! \param[in]		Module			-- module address
//! \return 		Bits			-- mode bits
////////////////////////////////////////////////////////////////////////////*/

Int_t sis3302_getLemoInMode(struct s_sis_3302 * Module) {
	Int_t data;
	data = sis3302_readAcquisitionControl(Module);
	if (data != 0xaffec0c0) data &= 0x7;
	return(data);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets lemo-in mode
//! \param[in]		Module			-- module address
//! \param[in]		Bits			-- mode bits
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_setLemoInMode(struct s_sis_3302 * Module, Int_t Bits) {
	Int_t data;
	if (Bits < 0 || Bits > 7) {
		sprintf(msg, "[setLemoInMode] [%s]:  LEMO IN mode out of range - %d (should be in [0,7])", Module->moduleName, ClockSource);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}
	data = (0x7 << 16);			// clear all bits
	if (!sis3302_writeAcquisitionControl(Module, data)) return(kFALSE);
	data = Bits & 0x7;					// set bits
	return(sis3302_writeAcquisitionControl(Module, data));
}

Bool_t sis3302_setLemoInMode_db(struct s_sis_3302 * Module) { return(sis3302_setLemoInMode(Module, Module->lemoInMode); };

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads lemo-out mode
//! \param[in]		Module			-- module address
//! \return 		Bits			-- mode bits
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_getLemoOutMode(struct s_sis_3302 * Module) {
	Int_t data;
	data = sis3302_readAcquisitionControl(Module);
	if (data != 0xaffec0c0) data = (data >> 4) & 0x3;
	return(data);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets lemo-out mode
//! \param[in]		Module			-- module address
//! \param[in]		Bits			-- mode bits
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_setLemoOutMode(struct s_sis_3302 * Module, Int_t Bits) {
	Int_t data;
	if (Bits < 0 || Bits > 3) {
		sprintf(msg, "[setLemoOutMode] [%s]:  LEMO OUT mode out of range - %d (should be in [0,3])", Module->moduleName, ClockSource);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}
	data = (0x3 << 20);			// clear all bits
	if (!sis3302_writeAcquisitionControl(Module, data)) return(kFALSE);
	data = (Bits & 0x3) << 4;			// set bits
	return(sis3302_writeAcquisitionControl(Module, data));
}

Bool_t sis3302_setLemoOutMode_db(struct s_sis_3302 * Module) { return(sis3302_setLemoOutMode(Module, Module->lemoOutMode); };

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Reads lemo-in enable mask
//! \param[in]		Module			-- module address
//! \return 		Bits			-- bits
////////////////////////////////////////////////////////////////////////////*/

Int_t sis3302_getLemoInEnableMask(struct s_sis_3302 * Module);
	Int_t data;
	data = sis3302_readAcquisitionControl(Module);
	if (data != 0xaffec0c0) data = (data >> 8) & 0x7;
	return(data);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Sets lemo-in enable mask
//! \param[in]		Module			-- module address
//! \param[in]		Bits			-- bits
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_setLemoInEnableMask(struct s_sis_3302 * Module, Int_t Bits) {
	Int_t data;
	if (Bits < 0 || Bits > 7) {
		sprintf(msg, "[setLemoInEnableMask] [%s]:  LEMO IN enable mask out of range - %d (should be in [0,7])", Module->moduleName, Bits);
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}
	data = (0x7 << 24);			// clear all bits
	if (!sis3302_writeAcquisitionControl(Module, data)) return(kFALSE);
	data = (Bits & 0x7) << 8;			// set bits
	return(sis3302_writeAcquisitionControl(Module, data));
}

Bool_t sis3302_setLemoInEnableMask_db(struct s_sis_3302 * Module) { return(sis3302_setLemoOutMode(Module, Module->lemoInEnableMask); };

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Check if data ready
//! \param[in]		Module		-- module address
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_dataReady(struct s_sis_3302 * Module) {
	Int_t data;
	data = sis3302_readAcquisitionControl(Module);
	if (data == 0xaffec0c0) return(kFALSE);
	return((data & SIS3302_STATUS_END_THRESH) != 0);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Switch sampling
//! \param[in]		Module		-- module address
//! \param[in]		ChanNo		-- channel number
//! \return			NextSample	-- next sampling address
////////////////////////////////////////////////////////////////////////////*/

Int_t sis3302_switchSampling(struct s_sis_3302 * Module, Int_t ChanNo) {

	Int_t nextSample;
	Int_t currentPage;

	if (Module->autoBankSwitch) {
		if (Module->currentBank == kSis3302KeyArmBank1Sampling) {
			Module->currentBank = kSis3302KeyArmBank2Sampling;
			currentPage = 0;
		} else {
			Module->currentBank = kSis3302KeyArmBank1Sampling;
			currentPage = 4;
		}
		sis3302_setPageRegister(Module, currentPage);
		nextSample = sis3302_readPrevBankSampleAddr(Module, ChanNo);
	} else {
		sis3302_setPageRegister(Module, 0);
		sis3302_keyDisarmSample(Module);
		nextSmaple = sis3302_readNextSampleAddr(Module, ChanNo);
		Module->currentBank = kSis3302KeyArmBank1Sampling;
	}
	if (nextSample != 0xaffec0c0) {
		nextSample &= 0x3FFFFC;
		nextSample >>= 1;
	}
	return(nextSample);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Set page register
//! \param[in]		Module		-- module address
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_setPageRegister(struct s_sis_3302 * Module, Int_t PageNumber) {

	Int_t offset;
	volatile Int_t * pageReg;

	offset = SIS3302_ADC_MEMORY_PAGE_REGISTER;
	pageReg = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (pageReg == NULL) return(kFALSE);

	*pageReg = PageNumber;
	return (kTRUE);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Read page register
//! \param[in]		Module		-- module address
//! \return 		PageNumber	-- current page number
////////////////////////////////////////////////////////////////////////////*/

Int_t sis3302_getPageRegister(struct s_sis_3302 * Module) {

	Int_t offset;
	volatile Int_t * pageReg;

	offset = SIS3302_ADC_MEMORY_PAGE_REGISTER;
	pageReg = (volatile Int_t *) sis3302_mapAddress(Module, offset);
	if (pageReg == NULL) return(affec0c0);

	return (*pageReg);
}

/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
//! \details		Check if adc/channel number ok
//! \param[in]		Module		-- module address
//! \param[in]		Caller		-- calling function
//! \param[in]		ChanNo		-- channel number
//! \return 		TRUE or FALSE
////////////////////////////////////////////////////////////////////////////*/

Bool_t sis3302_checkChannelNo(struct s_sis_3302 * Module, Char_t * Caller, Int_t ChanNo) {
	if (ChanNo != kSis3302AllChans && (ChanNo < 0 || ChanNo >= kSis3302NofChans)) {
		sprintf(msg, "[%s] [%s]: Channel number out of range - %d (should be in [0,%d])", Caller, Module->moduleName, ChanNo, (kSis3302NofChans - 1));
		f_ut_send_msg("__sis_3302", msg, ERR__MSG_INFO, MASK__PRTT);
		return(kFALSE);
	}
	return(kTRUE);
}

void sis3302_setStatus(struct s_sis_3302 * Module, UInt_t Bits) { Module->status |= Bits; };
void sis3302_clearStatus(struct s_sis_3302 * Module, UInt_t Bits) { Module->status &= ~Bits; };
UInt_t sis3302_getStatus(struct s_sis_3302 * Module) { return Module->status; };
Bool_t sis3302_isStatus(struct s_sis_3302 * Module, UInt_t Bits) { return ((Module->status & Bits) != 0); };

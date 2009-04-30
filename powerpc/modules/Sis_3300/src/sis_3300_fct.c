/*____________________________________________________________________[C CODE]
//////////////////////////////////////////////////////////////////////////////
// Name:           sis_3300.c
// Purpose:        Code for module SIS3300
// Description:    Supplies functions for SIS3300 digitizing ADC
// Author:         R. Lutter (adopted from code written by L. Maier)
// Revision:       
// Date:           
// Keywords:       
////////////////////////////////////////////////////////////////////////////*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <allParam.h>
#include <ces/bmalib.h>
#include <errno.h>

#include "gd_readout.h"

#include "sis_3300.h"
#include "sis_3300_protos.h"

#include "root_env.h"

#include "err_mask_def.h"
#include "errnum_def.h"

char msg[256];

struct s_sis_3300 * sis_3300_alloc(unsigned long vmeAddr, volatile unsigned char * base, char * name, int serial)
{
	struct s_sis_3300 * s;
	s = (struct s_sis_3300 *) calloc(1, sizeof(struct s_sis_3300));
	if (s != NULL) {
		s->baseAddr = base;
		s->vmeAddr = vmeAddr;
		strcpy(s->moduleName, name);
		s->serial = serial;
		s->verbose = 0;
	} else {
		sprintf(msg, "[alloc] Can't allocate sis_3000 struct");
		f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
	}
	return(s);
}

void sis_3300_module_info(struct s_sis_3300 * s)
{
	unsigned long ident;
	unsigned short modId;
	unsigned short version;
	ident = GET32(s->baseAddr, SIS_3300_MODULE_ID);
	modId = (ident >> 16) & 0xFFFF;
	version = (ident >> 12) & 0xF;
	sprintf(msg, "[module info] Id %#x, version %x", modId, version);
	f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
}

int sis_3300_fill_struct(struct s_sis_3300 * s, char * file)
{
	char res[256];
	const char * sp;
	int i;

	if (root_env_read(file) < 0) {
		sprintf(msg, "[fill_struct] Error reading file %s", file);
		f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
		return(0);
	}

	s->verbose = root_env_getval_b("SIS3300.VerboseMode", 0);
#if 0
	if (s->verbose) {
		sprintf(msg, "[fill_struct] SIS3300.VerboseMode: %d", s->verbose);
		f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
	}
#endif

	sp = root_env_getval_s("SIS3300.ModuleName", "");
	if (strcmp(sp, s->moduleName) != 0) {
		sprintf(msg, "[fill_struct] %s: Wrong module %s - should be %s", file, sp, s->moduleName);
		f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
		return(0);
	}
#if 0
	if (s->verbose) {
		sprintf(msg, "[fill_struct] SIS3300.ModuleName: %s", s->moduleName);
		f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
	}
#endif

	s->dumpRegsOnInit = root_env_getval_b("SIS3300.DumpRegisters", 0);
#if 0
	if (s->verbose) {
		sprintf(msg, "[fill_struct] SIS3300.DumpRegisters: %d", s->dumpRegsOnInit);
		f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
	}
#endif

	sprintf(res, "SIS3300.%s.BlockXfer", s->moduleName);
	s->blockTransOn = root_env_getval_b(res, 0);
#if 0
	if (s->verbose) {
		sprintf(msg, "[fill_struct] %s: %d", res, s->blockTransOn);
		f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
	}
#endif
	sprintf(res, "SIS3300.%s.AcquisitionMode", s->moduleName);
	s->acqMode = root_env_getval_i(res, 1);
#if 0
	if (s->verbose) {
		sprintf(msg, "[fill_struct] %s: %d", res, s->acqMode);
		f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
	}
#endif
	sprintf(res, "SIS3300.%s.MaxEvents", s->moduleName);
	s->maxNumEvts = root_env_getval_i(res, 0);
#if 0
	if (s->verbose) {
		sprintf(msg, "[fill_struct] %s: %d", res, s->maxNumEvts);
		f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
	}
#endif
	sprintf(res, "SIS3300.%s.PageSize", s->moduleName);
	s->pageSize = root_env_getval_i(res, 5);
#if 0
	if (s->verbose) {
		sprintf(msg, "[fill_struct] %s: %d", res, s->pageSize);
		f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
	}
#endif
	sprintf(res, "SIS3300.%s.ClockSource", s->moduleName);
	s->clockSource = root_env_getval_i(res, 0);
#if 0
	if (s->verbose) {
		sprintf(msg, "[fill_struct] %s: %d", res, s->clockSource);
		f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
	}
#endif
	sprintf(res, "SIS3300.%s.StartDelay", s->moduleName);
	s->startDelay = root_env_getval_i(res, 0);
#if 0
	if (s->verbose) {
		sprintf(msg, "[fill_struct] %s: %d", res, s->startDelay);
		f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
	}
#endif
	sprintf(res, "SIS3300.%s.StopDelay", s->moduleName);
	s->stopDelay = root_env_getval_i(res, 0);
#if 0
	if (s->verbose) {
		sprintf(msg, "[fill_struct] %s: %d", res, s->stopDelay);
		f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
	}
#endif
	sprintf(res, "SIS3300.%s.TriggerToUserOut", s->moduleName);
	s->triggerOut = root_env_getval_b(res, 0);
#if 0
	if (s->verbose) {
		sprintf(msg, "[fill_struct] %s: %d", res, s->triggerOut);
		f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
	}
#endif
	sprintf(res, "SIS3300.%s.InvertTrigger", s->moduleName);
	s->inverseTriggerOut = root_env_getval_b(res, 0);
#if 0
	if (s->verbose) {
		sprintf(msg, "[fill_struct] %s: %d", res, s->inverseTriggerOut);
		f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
	}
#endif
	sprintf(res, "SIS3300.%s.RouteTriggerToStop", s->moduleName);
	s->triggerRouting = root_env_getval_b(res, 0);
#if 0
	if (s->verbose) {
		sprintf(msg, "[fill_struct] %s: %d", res, s->triggerRouting);
		f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
	}
#endif
	sprintf(res, "SIS3300.%s.StartStopEnable", s->moduleName);
	s->frontStartStop = root_env_getval_b(res, 0);
#if 0
	if (s->verbose) {
		sprintf(msg, "[fill_struct] %s: %d", res, s->frontStartStop);
		f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
	}
#endif
	sprintf(res, "SIS3300.%s.AutoStartEnable", s->moduleName);
	s->autoStart = root_env_getval_b(res, 0);
#if 0
	if (s->verbose) {
		sprintf(msg, "[fill_struct] %s: %d", res, s->autoStart);
		f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
	}
#endif
	sprintf(res, "SIS3300.%s.BankFullToFront", s->moduleName);
	s->bankFullLemo = root_env_getval_i(res, 0);
#if 0
	if (s->verbose) {
		sprintf(msg, "[fill_struct] %s: %d", res, s->bankFullLemo);
		f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
	}
#endif
	sprintf(res, "SIS3300.%s.WrapAround", s->moduleName);
	s->wrapAround = root_env_getval_b(res, 0);
#if 0
	if (s->verbose) {
		sprintf(msg, "[fill_struct] %s: %d", res, s->wrapAround);
		f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
	}
#endif
	sprintf(res, "SIS3300.%s.TriggerOnBankArmed", s->moduleName);
	s->triggerArmedStart = root_env_getval_b(res, 0);
#if 0
	if (s->verbose) {
		sprintf(msg, "[fill_struct] %s: %d", res, s->triggerArmedStart);
		f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
	}
#endif
	for (i = 0; i < NOF_GROUPS; i++) {
		sprintf(res, "SIS3300.%s.Group%d.Readout", s->moduleName, i);
		s->channelGrpOn[i] = root_env_getval_b(res, 0);
#if 0
		if (s->verbose) {
			sprintf(msg, "[fill_struct] %s: %d", res, s->channelGrpOn[i]);
			f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
		}
#endif
	}
	for (i = 0; i < NOF_GROUPS; i++) {
		sprintf(res, "SIS3300.%s.Group%d.TrigOn", s->moduleName, i);
		s->triggerOn[i] = root_env_getval_i(res, 0);
#if 0
		if (s->verbose) {
			sprintf(msg, "[fill_struct] %s: %d", res, s->triggerMode[i]);
			f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
		}
#endif
	}
	for (i = 0; i < NOF_CHANNELS; i++) {
		sprintf(res, "SIS3300.%s.Chan%d.TrigThresh", s->moduleName, i);
		s->threshold[i] = root_env_getval_i(res, 0);
#if 0
		if (s->verbose) {
			sprintf(msg, "[fill_struct] %s: %d", res, s->threshold[i]);
			f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
		}
#endif
	}
	for (i = 0; i < NOF_CHANNELS; i++) {
		sprintf(res, "SIS3300.%s.Chan%d.TrigSlope", s->moduleName, i);
		s->triggerMode[i] = root_env_getval_i(res, 0);
#if 0
		if (s->verbose) {
			sprintf(msg, "[fill_struct] %s: %d", res, s->triggerOn[i]);
			f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
		}
#endif
	}
	for (i = 0; i < NOF_GROUPS; i++) {
		sprintf(res, "SIS3300.%s.Group%d.PeakTime", s->moduleName, i);
		s->peakingTime[i] = root_env_getval_i(res, 0);
#if 0
		if (s->verbose) {
			sprintf(msg, "[fill_struct] %s: %d", res, s->peakingTime[i]);
			f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
		}
#endif
	}
	for (i = 0; i < NOF_GROUPS; i++) {
		sprintf(res, "SIS3300.%s.Group%d.GapTime", s->moduleName, i);
		s->gapTime[i] = root_env_getval_i(res, 0);
#if 0
		if (s->verbose) {
			sprintf(msg, "[fill_struct] %s: %d", res, s->gapTime[i]);
			f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
		}
#endif
	}
	for (i = 0; i < NOF_GROUPS; i++) {
		sprintf(res, "SIS3300.%s.Group%d.PulseMode", s->moduleName, i);
		s->pulsMode[i] = root_env_getval_b(res, 0);
#if 0
		if (s->verbose) {
			sprintf(msg, "[fill_struct] %s: %d", res, s->pulsMode[i]);
			f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
		}
#endif
	}
	for (i = 0; i < NOF_GROUPS; i++) {
		sprintf(res, "SIS3300.%s.Group%d.PulseLength", s->moduleName, i);
		s->pulsLength[i] = root_env_getval_i(res, 0);
#if 0
		if (s->verbose) {
			sprintf(msg, "[fill_struct] %s: %d", res, s->pulsLength[i]);
			f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
		}
#endif
	}
}

void sis_3300_enable_bma(struct s_sis_3300 * s)
{
/* use blocktransfer */
/* buffersize (in 32bit words) : Memory     = 131072 * 4    */
/*                               TimeStamps = 1024 * 4          */ 
/*                               TriggerReg = 1024 * 4          */ 
	if (s->blockTransOn == 1) {  
		s->bltBufferSize = BLT_BUFFER_SIZE;
		if ((s->bma = bmaAlloc(s->bltBufferSize)) == NULL) {
 			sprintf(msg, "[enable_bma] %s, turning BlockXfer OFF", sys_errlist[errno]);
			f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
			s->blockTransOn = 0;
			return;
		} 

/* get bltBuffer */
		s->bltBuffer = (unsigned char *) s->bma->virtBase;
	} else {
		s->bma = NULL;
		s->bltBufferSize = 0;
		s->bltBuffer = NULL;
	}
}

void sis_3300_reset(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_KEY_RESET, 0x1);	
}

void sis_3300_setStartDelay(struct s_sis_3300 * s, uint16_t delay)
{
	SET32(s->baseAddr, SIS_3300_EXT_START_DELAY, (uint32_t) delay);
}

uint16_t sis_3300_getStartDelay(struct s_sis_3300 * s)
{
	return GET32(s->baseAddr, SIS_3300_EXT_START_DELAY) & 0xffff;
}

void sis_3300_setStopDelay(struct s_sis_3300 * s, uint16_t delay)
{
	SET32(s->baseAddr, SIS_3300_EXT_STOP_DELAY, (uint32_t) delay);
}

uint16_t sis_3300_getStopDelay(struct s_sis_3300 * s)
{
	return GET32(s->baseAddr, SIS_3300_EXT_STOP_DELAY) & 0xffff;
}

void sis_3300_setTimeStampPredivider(struct s_sis_3300 * s, uint16_t divider)
{
	SET32(s->baseAddr, SIS_3300_TIMESTAMP_PREDIVIDER, (uint32_t)divider);
}

uint16_t sis_3300_getTimeStampPredivider(struct s_sis_3300 * s)
{
	return (uint16_t)(GET32(s->baseAddr, SIS_3300_TIMESTAMP_PREDIVIDER) & 0xffff);
}

void sis_3300_startSampling(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_KEY_START, 0x1);
}

void sis_3300_stopSampling(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_KEY_STOP, 0x1);
}

void sis_3300_startAutoBankSwitch(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_KEY_START_AUTO_BANK_SWITCH, 0x1);
}

void sis_3300_stopAutoBankSwitch(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_KEY_STOP_AUTO_BANK_SWITCH, 0x1);
}

void sis_3300_clearBankFull(struct s_sis_3300 * s, uint8_t bank)
{
	switch (bank) {
	case 0:
	SET32(s->baseAddr, SIS_3300_KEY_BANK1_FULL_FLAG, 0x1);
		break;
	case 1:
	SET32(s->baseAddr, SIS_3300_KEY_BANK2_FULL_FLAG, 0x1);
		break;
	default:
		break;
	}
}

/* ------------- Event Timestamp Directory ------------------------------ */

uint32_t sis_3300_getEventTimeStamp(struct s_sis_3300 * s, uint8_t bank, uint16_t event)
{
	if (event < 1024) {
		switch (bank) {
		case 0:
			return GET32(s->baseAddr, SIS_3300_BANK1_TIME_DIR + event * 4) & 0xffffff;
			break;
			
		case 1:
			return GET32(s->baseAddr, SIS_3300_BANK2_TIME_DIR + event * 4) & 0xffffff;
			break;
		}
	}
	
	return 0;
}

/* ------------- Event Configuration Register --------------------------- */

void sis_3300_setPageSize(struct s_sis_3300 * s, uint8_t pagesize)
{
	uint32_t bits = (GET32(s->baseAddr, SIS_3300_GROUP1_EVT_CONFIG) & 0xfffffff8);
		bits |= (pagesize & 0x07);
	SET32(s->baseAddr, SIS_3300_EVT_CONFIG, bits);
}

uint8_t sis_3300_getPageSize(struct s_sis_3300 * s)
{
	return (uint8_t)(GET32(s->baseAddr, SIS_3300_GROUP1_EVT_CONFIG) & 0x7);
}

void sis_3300_enableWrapAroundMode(struct s_sis_3300 * s)
{
	uint32_t bits = (GET32(s->baseAddr, SIS_3300_GROUP1_EVT_CONFIG) & 0xfffffff7) | 0x8;
	SET32(s->baseAddr, SIS_3300_EVT_CONFIG, bits);
}

void sis_3300_disableWrapAroundMode(struct s_sis_3300 * s)
{
	uint32_t bits = GET32(s->baseAddr, SIS_3300_GROUP1_EVT_CONFIG) & 0xfffffff7;
	SET32(s->baseAddr, SIS_3300_EVT_CONFIG, bits);
}

uint8_t sis_3300_wrapAroundModeEnabled(struct s_sis_3300 * s)
{
	return ((GET32(s->baseAddr, SIS_3300_GROUP1_EVT_CONFIG) & 0xfffffff7) != 0 ? 1 : 0);
}

void sis_3300_enableGateChaining(struct s_sis_3300 * s)
{
	uint32_t bits = (GET32(s->baseAddr, SIS_3300_GROUP1_EVT_CONFIG) & 0xffffffef) | 0x10;
	SET32(s->baseAddr, SIS_3300_EVT_CONFIG, bits);
}

void sis_3300_disableGateChaining(struct s_sis_3300 * s)
{
	uint32_t bits = (GET32(s->baseAddr, SIS_3300_GROUP1_EVT_CONFIG) & 0xffffffef);
	SET32(s->baseAddr, SIS_3300_EVT_CONFIG, bits);
}

uint8_t sis_3300_gateChainingEnabled(struct s_sis_3300 * s)
{
	return ((GET32(s->baseAddr, SIS_3300_GROUP1_EVT_CONFIG) & 0xffffffef) != 0 ? 1 : 0);
}

void sis_3300_setAverage(struct s_sis_3300 * s, uint8_t average)
{
	uint32_t bits = GET32(s->baseAddr, SIS_3300_GROUP1_EVT_CONFIG) & 0xfff8ffff;
	bits &= ((average & 0x07) << 16);
	SET32(s->baseAddr, SIS_3300_EVT_CONFIG, bits);
}

uint8_t sis_3300_getAverage(struct s_sis_3300 * s)
{
	return (GET32(s->baseAddr, SIS_3300_GROUP1_EVT_CONFIG) & 0x070000) >> 16;
}

/* ------------- Threshold Register ------------------------------------- */

void sis_3300_setThreshold(struct s_sis_3300 * s, uint8_t channel, uint16_t value)
{
	uint32_t bits;
	bits = value;
	
	switch (channel) {
	case 0:
			bits = bits << 16;
			bits |= (GET32(s->baseAddr, SIS_3300_GROUP1_THRESHOLD) & 0x0000ffff);
			SET32(s->baseAddr, SIS_3300_GROUP1_THRESHOLD, bits);
			break;
	case 1:
		bits |= (GET32(s->baseAddr, SIS_3300_GROUP1_THRESHOLD) & 0xffff0000);
			SET32(s->baseAddr, SIS_3300_GROUP1_THRESHOLD, bits);
			break;
	case 2:
			bits = bits << 16;
			bits |= (GET32(s->baseAddr, SIS_3300_GROUP2_THRESHOLD) & 0x0000ffff);
			SET32(s->baseAddr, SIS_3300_GROUP2_THRESHOLD, bits);
			break;
	case 3:
			bits |= (GET32(s->baseAddr, SIS_3300_GROUP2_THRESHOLD) & 0xffff0000);
			SET32(s->baseAddr, SIS_3300_GROUP2_THRESHOLD, bits);
			break;
	case 4:
			bits = bits << 16;
			bits |= (GET32(s->baseAddr, SIS_3300_GROUP3_THRESHOLD) & 0x0000ffff);
			SET32(s->baseAddr, SIS_3300_GROUP3_THRESHOLD, bits);
			break;
	case 5:
			bits |= (GET32(s->baseAddr, SIS_3300_GROUP3_THRESHOLD) & 0xffff0000);
			SET32(s->baseAddr, SIS_3300_GROUP3_THRESHOLD, bits);
			break;
	case 6:
			bits = bits << 16;
			bits |= (GET32(s->baseAddr, SIS_3300_GROUP4_THRESHOLD) & 0x0000ffff);
			SET32(s->baseAddr, SIS_3300_GROUP4_THRESHOLD, bits);
			break;
	case 7:
			bits |= (GET32(s->baseAddr, SIS_3300_GROUP4_THRESHOLD) & 0xffff0000);
			SET32(s->baseAddr, SIS_3300_GROUP4_THRESHOLD, bits);
			break;
	}
}

uint16_t sis_3300_getThreshold(struct s_sis_3300 * s, uint8_t channel)
{
	uint16_t threshold;

	switch (channel) {
	case 0:
		threshold = GET32(s->baseAddr, SIS_3300_GROUP1_THRESHOLD) >> 16;
		break;
	case 1:
		threshold = GET32(s->baseAddr, SIS_3300_GROUP1_THRESHOLD);
		break;
	case 2:
		threshold = GET32(s->baseAddr, SIS_3300_GROUP2_THRESHOLD) >> 16;
		break;
	case 3:
		threshold = GET32(s->baseAddr, SIS_3300_GROUP2_THRESHOLD);
		break;
	case 4:
		threshold = GET32(s->baseAddr, SIS_3300_GROUP3_THRESHOLD) >> 16;
		break;
	case 5:
		threshold = GET32(s->baseAddr, SIS_3300_GROUP3_THRESHOLD);
		break;
	case 6:
		threshold = GET32(s->baseAddr, SIS_3300_GROUP4_THRESHOLD) >> 16;
		break;
	case 7:
		threshold = GET32(s->baseAddr, SIS_3300_GROUP4_THRESHOLD);
		break;
	default:
		threshold = 0;
		break;
	}

	return threshold;
}


/* ------------- Trigger Flag Clear Counter Register -------------------- */

void sis_3300_setTriggerClearCounter(struct s_sis_3300 * s, uint8_t channelGrp, uint16_t value)
{
	switch (channelGrp) {
	case 0:
		SET32(s->baseAddr, SIS_3300_TRIGGER_FLAG_CLR_CNT_ADC12, value);
		break;
	case 1:
		SET32(s->baseAddr, SIS_3300_TRIGGER_FLAG_CLR_CNT_ADC34, value);
		break;
	case 2:
		SET32(s->baseAddr, SIS_3300_TRIGGER_FLAG_CLR_CNT_ADC56, value);
		break;
	case 3:
		SET32(s->baseAddr, SIS_3300_TRIGGER_FLAG_CLR_CNT_ADC78, value);
		break;
	default:
		break;
	}
}

uint16_t sis_3300_getTriggerClearCounter(struct s_sis_3300 * s, uint8_t channelGrp)
{
	uint16_t counter;
	switch (channelGrp) {
	case 0:
		counter = GET32(s->baseAddr, SIS_3300_TRIGGER_FLAG_CLR_CNT_ADC12) & 0xffff;
		break;
	case 1:
		counter = GET32(s->baseAddr, SIS_3300_TRIGGER_FLAG_CLR_CNT_ADC34) & 0xffff;
		break;
	case 2:
		counter = GET32(s->baseAddr, SIS_3300_TRIGGER_FLAG_CLR_CNT_ADC56) & 0xffff;
		break;
	case 3:
		counter = GET32(s->baseAddr, SIS_3300_TRIGGER_FLAG_CLR_CNT_ADC78) & 0xffff;
		break;
	default:
		counter = 0;
	}
	
	return counter;
} 

/* ------------- Clock Predivider Register ------------------------------ */

void sis_3300_setClockPredivider(struct s_sis_3300 * s, uint8_t predivider)
{
	SET32(s->baseAddr, SIS_3300_CLOCK_PREDIVIDER_ALL_ADC , (uint32_t)predivider);
}

uint8_t sis_3300_getClockPredivider(struct s_sis_3300 * s)
{
	return (uint8_t)(GET32(s->baseAddr, SIS_3300_CLOCK_PREDIVIDER_ADC12) & 0xff);
}

/* ------------- Number of Sample Register ------------------------------ */

void sis_3300_setNumberOfSamples(struct s_sis_3300 * s, uint8_t value)
{
	SET32(s->baseAddr, SIS_3300_NO_OF_SAMPLE_ALL_ADC, (uint32_t)value);
}

uint8_t sis_3300_getNumberOfSamples(struct s_sis_3300 * s)
{
	return (uint8_t)(GET32(s->baseAddr, SIS_3300_NO_OF_SAMPLE_ADC12) & 0xff);
}

/* ------------- Trigger Setup Register --------------------------------- */

void sis_3300_setTriggerGapTime(struct s_sis_3300 * s, uint8_t channelGrp, uint8_t value)
{
	uint32_t bits;
	bits = ((uint32_t)value) << 8;
	
	switch (channelGrp) {
	case 0:
		bits |= (GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC12) & 0xffff00ff);
		SET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC12, bits);
		break;
	case 1:
		bits |= (GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC34) & 0xffff00ff);
		SET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC34, bits);
		break;
	case 2:
		bits |= (GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC56) & 0xffff00ff);
		SET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC56, bits);
		break;
	case 3:
		bits |= (GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC78) & 0xffff00ff);
		SET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC78, bits);
		break;
	default:
		break;
	}
}

uint8_t sis_3300_getTriggerGapTime(struct s_sis_3300 * s, uint8_t channelGrp)
{
	uint8_t gaptime;
	
	switch (channelGrp) {
	case 0:
		gaptime = (GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC12) & 0xff00) >> 8;
		break;
	case 1:
		gaptime = (GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC34) & 0xff00) >> 8;
		break;
	case 2:
		gaptime = (GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC56) & 0xff00) >> 8;
		break;
	case 3:
		gaptime = (GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC78) & 0xff00) >> 8;
		break;
	default:
		gaptime = 0;
		break;
	}
	
	return gaptime;
}

void sis_3300_setTriggerPeakingTime(struct s_sis_3300 * s, uint8_t channelGrp,
																	 uint8_t value)
{
	uint32_t bits;

	switch (channelGrp) {
	case 0:
		bits = GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC12) & 0xffffff00;
		bits |= value;
		SET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC12, bits);
		break;
	case 1:
		bits = GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC34) & 0xffffff00;
		bits |= value;
		SET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC34, bits);
		break;
	case 2:
		bits = GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC56) & 0xffffff00;
		bits |= value;
		SET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC56, bits);
		break;
	case 3:
		bits = GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC78) & 0xffffff00;
		bits |= value;
		SET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC78, bits);
		break;
	default:
		break;
	}
}

uint8_t sis_3300_getTriggerPeakingTime(struct s_sis_3300 * s, uint8_t channelGrp)
{
	uint8_t peaktime = 0;

	switch (channelGrp) {
	case 0:
		peaktime = (GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC12) & 0xff);
		break;
	case 1:
		peaktime = (GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC34) & 0xff);
		break;
	case 2:
		peaktime = (GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC56) & 0xff);
		break;
	case 3:
		peaktime = (GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC78) & 0xff);
		break;
	default:
		break;
	}
	return peaktime;
}

void sis_3300_setTriggerPulseLength(struct s_sis_3300 * s, uint8_t channelGrp, uint8_t value)
{
	uint32_t bits;
	
	bits = ((uint32_t)(value & 0x0f)) << 16;
	switch (channelGrp) {
	case 0:
		bits |= (GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC12) & 0xfff0ffff);
		SET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC12, bits);
		break;
	case 1:
		bits |= (GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC34) & 0xfff0ffff);
		SET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC34, bits);
		break;
	case 2:
		bits |= (GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC56) & 0xfff0ffff);
		SET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC56, bits);
		break;
	case 3:
		bits |= (GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC78) & 0xfff0ffff);
		SET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC78, bits);
		break;
	}
}

uint8_t sis_3300_getTriggerPulseLength(struct s_sis_3300 * s, uint8_t channelGrp)
{
	uint8_t pulselength;
	
	switch (channelGrp) {
	case 0:
		pulselength = 
			(GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC12) >> 16) & 0x0f;
		break;
	case 1:
		pulselength = 
			(GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC34) >> 16) & 0x0f;
		break;
	case 2:
		pulselength = 
			(GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC56) >> 16) & 0x0f;
		break;
	case 3:
		pulselength = 
			(GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC78) >> 16) & 0x0f;
		break;
	default:
		pulselength = 0;
		break;
	}
	
	return pulselength;
}

/* BE CAREFUL: contrary to to the sis3300 manual we are counting the	 */
/* channels from 0 to 7, so even corresponds to channel 0, 2, 4, 6		 */
void sis_3300_enableFIRTestEven(struct s_sis_3300 * s, uint8_t channelGrp)
{
	switch (channelGrp) {
	case 0:
		CLRB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC12, 20);
		break;
	case 1:
		CLRB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC34, 20);
		break;
	case 2:
		CLRB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC56, 20);
		break;
	case 3:
		CLRB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC78, 20);
		break;
	default:
		break;
	} 
}

void sis_3300_disableFIRTestEven(struct s_sis_3300 * s, uint8_t channelGrp)
{
	switch (channelGrp) {
	 case 0:
		SETB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC12, 20);
		break;
	case 1:
		SETB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC34, 20);
		break;
	case 2:
		SETB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC56, 20);
		break;
	 case 3:
		SETB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC78, 20);
		break;
	default:
		break;
	 } 
}

uint8_t sis_3300_FIRTestEvenEnabled(struct s_sis_3300 * s, uint8_t channelGrp)
{
	uint8_t ret;
	
	switch (channelGrp) {
	case 0:
		ret = TSTB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC12, 20);
		break;
	case 1:
		ret = TSTB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC34, 20);
		break;
	case 2:
		ret = TSTB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC56, 20);
		break;
	case 3:
		ret = TSTB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC78, 20);
		break;
	default:
		ret = 0;
		break;
	}
	return (ret != 0) ? 0 : 1;
}

void sis_3300_enableFIRTestMode(struct s_sis_3300 * s, uint8_t channelGrp)
{
	switch (channelGrp) {
	case 0:
		SETB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC12, 21);
		break;
	case 1:
		SETB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC34, 21);
		break;
	case 2:
		SETB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC56, 21);
		break;
	case 3:
		SETB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC78, 21);
		break;
	default:
		break;
	}
}

void sis_3300_disableFIRTestMode(struct s_sis_3300 * s, uint8_t channelGrp)
{
	switch (channelGrp) {
	case 0:
		CLRB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC12, 21);
		break;
	case 1:
		CLRB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC34, 21);
		break;
	case 2:
		CLRB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC56, 21);
		break;
	case 3:
		CLRB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC78, 21);
		break;
	default:
		break;
	}
}

uint8_t sis_3300_FIRTestModeEnabled(struct s_sis_3300 * s, uint8_t channelGrp)
{
	uint8_t ret;
	
	switch (channelGrp) {
	case 0:
		ret = TSTB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC12, 21);
		break;
	case 1:
		ret = TSTB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC34, 21);
		break;
	case 2:
		ret = TSTB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC56, 21);
		break;
	case 3:
		ret = TSTB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC78, 21);
		break;
	default:
		ret = 0;
		break;
	}
	return (ret != 0) ? 1 : 0;
}

void sis_3300_setTriggerMode(struct s_sis_3300 * s, uint8_t channel, uint8_t mode)
{ 
	/* mode == 0: none, mode == 1: less, mode == 2 greater, mode == 3 both */
	
	uint8_t shift;
	uint32_t bits;
	uint32_t mask;
		
	bits = mode & 0x03;
	mask = 0x03;
	shift = ((channel % 2) == 0) ? 24 : 26;
	
	mask = ~(mask << shift);
	bits = bits << shift;
 
	switch (channel) {
	case 0:
	case 1:
		bits |= (GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC12) & mask);
		SET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC12, bits);
		break;
	case 2:
	case 3:
		bits |= (GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC34) & mask);
		SET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC34, bits);
		break;
	case 4:
	case 5:
		bits |= (GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC56) & mask);
		SET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC56, bits);
		break;
	case 6:
	case 7:
		bits |= (GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC78) & mask);
		SET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC78, bits);
		break;
	}
}

uint8_t sis_3300_getTriggerMode(struct s_sis_3300 * s, uint8_t channel)
{ /* mode = 0: none, mode == 1: less, mode == 2 greater, mode == 3 both */
	
	uint8_t ret;
	uint8_t shift;
	uint32_t mask;
	
	mask = 0x03;
	shift = ((channel % 2) == 0) ? 24 : 26;
	mask = mask << shift;
	
	switch (channel) {
	case 0:
	case 1:
		ret = (GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC12) & mask) >> shift;
		break;
	case 2:
	case 3:
		ret = (GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC34) & mask) >> shift;
		break;
	case 4:
	case 5:
		ret = (GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC56) & mask) >> shift;
		break;
	case 6:
	case 7:
		ret = (GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC78) & mask) >> shift;
		break;
	default: 
		ret = 0;
		break;
	}
	
	return ret;
}

void sis_3300_enablePulseMode(struct s_sis_3300 * s, uint8_t channel)
{
	switch (channel) {
	case 0:
		SETB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC12, 28);
		break;
	case 1:
		SETB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC34, 28);
		break;
	case 2:
		SETB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC56, 28);
		break;
	case 3:
		SETB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC78, 28);
		break;
	default:
		break;
	}
}

void sis_3300_disablePulseMode(struct s_sis_3300 * s, uint8_t channel)
{
	switch (channel) {
	case 0:
		CLRB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC12, 28);
		break;
	case 1:
		CLRB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC34, 28);
		break;
	case 2:
		CLRB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC56, 28);
		break;
	case 3:
		CLRB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC78, 28);
		break;
	default:
		break;
	}
}

uint8_t sis_3300_pulseModeEnabled(struct s_sis_3300 * s, uint8_t channel)
{
	uint8_t ret;
	
	switch (channel) {
	case 0:
		ret = TSTB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC12, 28);
		break;
	case 1:
		ret = TSTB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC34, 28);
		break;
	case 2:
		ret = TSTB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC56, 28);
		break;
	case 3:
		ret = TSTB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC78, 28);
		break;
		
	default:
		ret = 0;
		break;
	}
	return (ret != 0 ? 1 : 0);
}

void sis_3300_enableFIRTrigger(struct s_sis_3300 * s, uint8_t channelGrp)
{
	switch (channelGrp) {
	case 0:
		SETB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC12, 31);
		break;
	case 1:
		SETB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC34, 31);
		break;
	case 2:
		SETB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC56, 31);
		break;
	case 3:
		SETB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC78, 31);
		break;
	default:
		break;
	}
}

void sis_3300_disableFIRTrigger(struct s_sis_3300 * s, uint8_t channelGrp)
{
	switch (channelGrp) {
	case 0:
		CLRB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC12, 31);
		break;
	case 1:
		CLRB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC34, 31);
		break;
	case 2:
		CLRB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC56, 31);
		break;
	case 3:
		CLRB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC78, 31);
		break;
	default:
		break;
	}
}

uint8_t sis_3300_FIRTriggerEnabled(struct s_sis_3300 * s, uint8_t channel)
{
	uint8_t ret;
	
	switch (channel) {
	case 0:
		ret = TSTB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC12, 31);
		break;
	case 1:
		ret = TSTB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC34, 31);
		break;
	case 2:
		ret = TSTB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC56, 31);
		break;
	case 3:
		ret = TSTB32(s->baseAddr,SIS_3300_TRIGGER_SETUP_ADC78, 31);
		break;
	default:
		ret = 0;
		break;
	}
	
	return (ret != 0 ? 1 : 0);
}

uint32_t sis_3300_getTriggerSetupRegister(struct s_sis_3300 * s, uint8_t channelGrp)
{
	switch (channelGrp) {
	case 0:
		return GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC12);
		break;
	case 1:
		return GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC34);
		break;
	case 2:
		return GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC56);
		break;
	case 3:
		return GET32(s->baseAddr, SIS_3300_TRIGGER_SETUP_ADC78);
		break;
	default:
		break;
	}
	
	return 0;
}

/* ------------- Max Number Event Register ------------------------------ */

void sis_3300_setMaxNumberOfEvents(struct s_sis_3300 * s, uint16_t maxno)
{
	SET32(s->baseAddr, SIS_3300_MAX_NUMBER_OF_EVENTS_ALL_ADC, (uint32_t) maxno);
}

uint16_t sis_3300_getMaxNumberOfEvents(struct s_sis_3300 * s)
{
	return (uint16_t)(GET32(s->baseAddr, SIS_3300_MAX_NUMBER_OF_EVENTS_ADC12) & 0xffff);
}

/* -------------- Acquisition Control Register --------------------------- */

void sis_3300_armBank(struct s_sis_3300 * s, uint8_t bank)
{
	switch (bank) {
	case 0:
		SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, SIS_3300_ACQ_BANK1_CLOCK_ON);
		break;
	case 1:
		SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, SIS_3300_ACQ_BANK2_CLOCK_ON);
		break;
	default:
		break;
	}
}

void sis_3300_disarmBank(struct s_sis_3300 * s, uint8_t bank)
{
	switch (bank) {
	case 0:
		SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, SIS_3300_ACQ_BANK1_CLOCK_OFF);
		break;
	case 1:
		SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, SIS_3300_ACQ_BANK2_CLOCK_OFF);
		break;
	default:
		break;
	}
}

uint8_t sis_3300_bankArmed(struct s_sis_3300 * s, uint8_t bank)
{
	uint8_t armed;
	
	switch (bank) {
	case 0:
		armed =
			GET32(s->baseAddr, SIS_3300_ACQ_STATUS) & SIS_3300_ACQ_BANK1_CLOCK_ON;
		break;
	case 1:
		armed =
			GET32(s->baseAddr, SIS_3300_ACQ_STATUS) & SIS_3300_ACQ_BANK2_CLOCK_ON;
		break;
	default:
		armed = 0;
		break;
	}
	
	return (armed != 0 ? 1 : 0);
}

void sis_3300_enableAutoBankSwitch(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, 0x4);
}

void sis_3300_disableAutoBankSwitch(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, 0x40000);
}

uint8_t sis_3300_autoBankSwitchEnabled(struct s_sis_3300 * s)
{
	return ((GET32(s->baseAddr, SIS_3300_ACQ_STATUS) & 0x4) != 0 ? 1 : 0);
}

void sis_3300_enableAutostart(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, SIS_3300_ACQ_AUTOSTART_ON);
}

void sis_3300_disableAutostart(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, SIS_3300_ACQ_AUTOSTART_OFF);
}

uint8_t sis_3300_autostartEnabled(struct s_sis_3300 * s)
{
	return ((GET32(s->baseAddr, SIS_3300_ACQ_STATUS) & SIS_3300_ACQ_AUTOSTART_ON) != 0 ? 1 : 0);
}

void sis_3300_enableMultiEventMode(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, SIS_3300_ACQ_MULTIEVENT_ON);
}

void sis_3300_disableMultiEventMode(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, SIS_3300_ACQ_MULTIEVENT_OFF);
}

uint8_t sis_3300_multiEventModeEnabled(struct s_sis_3300 * s)
{
	return ((GET32(s->baseAddr, SIS_3300_ACQ_STATUS) & SIS_3300_ACQ_MULTIEVENT_ON) != 0 ? 1 : 0);
}

void sis_3300_enableStartDelay(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, SIS_3300_ACQ_STARTDELAY_ON);
}

void sis_3300_disableStartDelay(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, SIS_3300_ACQ_STARTDELAY_OFF);
}

uint8_t sis_3300_startDelayEnabled(struct s_sis_3300 * s)
{
	return ((GET32(s->baseAddr, SIS_3300_ACQ_STATUS) & SIS_3300_ACQ_STARTDELAY_ON) != 0 ? 1 : 0);
}

void sis_3300_enableStopDelay(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, SIS_3300_ACQ_STOPDELAY_ON);
}

void sis_3300_disableStopDelay(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, SIS_3300_ACQ_STOPDELAY_OFF);
}

uint8_t sis_3300_stopDelayEnabled(struct s_sis_3300 * s)
{
	return ((GET32(s->baseAddr, SIS_3300_ACQ_STATUS) & SIS_3300_ACQ_STOPDELAY_ON) != 0 ? 1 : 0);
}

void sis_3300_enableFrontStartStop(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, SIS_3300_ACQ_FRONTSTART_ON);
}

void sis_3300_disableFrontStartStop(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, SIS_3300_ACQ_FRONTSTART_OFF);
}

uint8_t sis_3300_frontStartStopEnabled(struct s_sis_3300 * s)
{
	return ((GET32(s->baseAddr, SIS_3300_ACQ_STATUS) & SIS_3300_ACQ_FRONTSTART_ON) != 0 ? 1 : 0);
}

void sis_3300_enableP2StartStop(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, SIS_3300_ACQ_P2START_OFF);
}

void sis_3300_disableP2StartStop(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, SIS_3300_ACQ_P2START_OFF);
}

uint8_t sis_3300_P2StartStopEnabled(struct s_sis_3300 * s)
{
	return ((GET32(s->baseAddr, SIS_3300_ACQ_STATUS) & SIS_3300_ACQ_P2START_ON) != 0 ? 1 : 0);
}

void sis_3300_enableFrontPanelGateMode(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, SIS_3300_ACQ_GATE_ON);
}

void sis_3300_disableFrontPanelGateMode(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, SIS_3300_ACQ_FRONTSTART_OFF);
}

uint8_t sis_3300_gateModeFrontPanelEnabled(struct s_sis_3300 * s)
{
	return ((GET32(s->baseAddr, SIS_3300_ACQ_STATUS) & SIS_3300_ACQ_GATE_ON) != 0 ? 1 : 0);
}

void sis_3300_enableExtClockRandomMode(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, 0x0800);
}

void sis_3300_disableExtClockRandomMode(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, 0x8000000);
}

uint8_t sis_3300_extClockRandomModeEnabled(struct s_sis_3300 * s)
{
	return ((GET32(s->baseAddr, SIS_3300_ACQ_STATUS) & 0x0800) != 0 ? 1 : 0);
}

void sis_3300_setClockSource(struct s_sis_3300 * s, uint8_t mode)
{
	if (mode <= 7) {
		/* first you have to clear all clock sources */
		SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, SIS_3300_ACQ_CLEAR_CLKSRC);
		
		switch (mode) {
		case 0:
			SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, SIS_3300_ACQ_CLKSRC0);
			break;
		case 1:
			SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, SIS_3300_ACQ_CLKSRC1);
			break;
		case 2:
			SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, SIS_3300_ACQ_CLKSRC2);
			break;
		case 3:
			SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, SIS_3300_ACQ_CLKSRC3);
			break;
		case 4:
			SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, SIS_3300_ACQ_CLKSRC4);
			break;
		case 5:
			SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, SIS_3300_ACQ_CLKSRC5);
			break;
		case 6:
			SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, SIS_3300_ACQ_CLKSRC6);
			break;
		case 7:
			SET32(s->baseAddr, SIS_3300_ACQ_CONTROL, SIS_3300_ACQ_CLKSRC7);
			break;
		}
	}
}

uint8_t sis_3300_getClockSource(struct s_sis_3300 * s)
{
	return (GET32(s->baseAddr, SIS_3300_ACQ_CONTROL) & 0x7000) >> 12;
}

uint8_t sis_3300_getADC_BUSY(struct s_sis_3300 * s)
{
	return ((GET32(s->baseAddr, SIS_3300_ACQ_CONTROL) & 0x10000) != 0 ? 1 : 0);
}

uint8_t sis_3300_getBankSwitchBusy(struct s_sis_3300 * s)
{
	return ((GET32(s->baseAddr, SIS_3300_ACQ_CONTROL) & 0x40000) != 0 ? 1 : 0); 
}

uint8_t sis_3300_getBankBusy(struct s_sis_3300 * s, uint8_t bank)
{
	switch (bank) {
	case 0:
		return ((GET32(s->baseAddr, SIS_3300_ACQ_CONTROL) & 0x100000) != 0 ? 1 : 0);
		break;
	case 1:
		return ((GET32(s->baseAddr, SIS_3300_ACQ_CONTROL) & 0x400000) != 0 ? 1 : 0);
	default:
		break;
	}
	
	return 0;
}

uint8_t sis_3300_getBankFull(struct s_sis_3300 * s, uint8_t bank)
{
	switch (bank) {
	case 0:
		return ((GET32(s->baseAddr, SIS_3300_ACQ_CONTROL) & 0x200000) != 0 ? 1 : 0);
		break;
	case 1:
		return ((GET32(s->baseAddr, SIS_3300_ACQ_CONTROL) & 0x800000) != 0 ? 1 : 0);
		break;
	default:
		break;
	}
	
	return 0;
}


/* ------------ Module Control Register ---------------------- ---------- */


void sis_3300_setUserLED(struct s_sis_3300 * s, uint8_t on)
{
	if (on == 1)
		SET32(s->baseAddr, SIS_3300_CONTROL, SIS_3300_CTL_LED_ON);
	if (on == 0)
		SET32(s->baseAddr, SIS_3300_CONTROL, SIS_3300_CTL_LED_OFF);
}

uint8_t sis_3300_getUserLED(struct s_sis_3300 * s)
{
	return ((GET32(s->baseAddr, SIS_3300_STATUS) & SIS_3300_CTL_LED_ON) != 0 ? 1 : 0);
}

void sis_3300_setUserOutput(struct s_sis_3300 * s, uint8_t on)
{
	if (on == 1)
		SET32(s->baseAddr, SIS_3300_CONTROL, SIS_3300_CTL_USEROUT_ON);
	if (on == 0)
		SET32(s->baseAddr, SIS_3300_CONTROL, SIS_3300_CTL_USEROUT_OFF);
}

uint8_t sis_3300_getUserOutput(struct s_sis_3300 * s)
{
	return ((GET32(s->baseAddr, SIS_3300_STATUS) & SIS_3300_CTL_USEROUT_ON) != 0 ? 1 : 0);
}

void sis_3300_enableTriggerOut(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_CONTROL, SIS_3300_CTL_ENABLE_TRIGOUT);
}

void sis_3300_disableTriggerOut(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_CONTROL, SIS_3300_CTL_ENABLE_USEROUT);
}

uint8_t sis_3300_triggerOutEnabled(struct s_sis_3300 * s)
{
	return ((GET32(s->baseAddr, SIS_3300_STATUS) & SIS_3300_CTL_ENABLE_TRIGOUT) != 0 ? 1 : 0);
}

void sis_3300_enableInverseTriggerOut(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_CONTROL, SIS_3300_CTL_INVERSE_TRIGOUT);
}

void sis_3300_disableInverseTriggerOut(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_CONTROL, SIS_3300_CTL_STRAIGHT_TRIGOUT);
}

uint8_t sis_3300_inverseTriggerOutEnabled(struct s_sis_3300 * s)
{
	return ((GET32(s->baseAddr, SIS_3300_STATUS) & SIS_3300_CTL_INVERSE_TRIGOUT) != 0 ? 1 : 0);
}

void sis_3300_enableTriggerArmedStart(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_CONTROL, SIS_3300_CTL_TRIGOUT_ARMSTART);
}

void sis_3300_disableTriggerArmedStart(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_CONTROL, SIS_3300_CTL_TRIGOUT_ARMED);
}

uint8_t sis_3300_triggerArmedStartEnabled(struct s_sis_3300 * s)
{
	return ((GET32(s->baseAddr, SIS_3300_STATUS) & SIS_3300_CTL_TRIGOUT_ARMSTART) != 0 ? 1 : 0);
}

void sis_3300_enableTriggerRouting(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_CONTROL, 0x00000040);
}

void sis_3300_disableTriggerRouting(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_CONTROL, 0x00400000);
}

uint8_t sis_3300_triggerRoutingEnabled(struct s_sis_3300 * s)
{
	return ((GET32(s->baseAddr, SIS_3300_STATUS) & 0x0040) != 0 ? 1 : 0);
}

void sis_3300_enableBankFullLemo(struct s_sis_3300 * s, uint8_t lemo)
{
	if (lemo < 3) {
		SET32(s->baseAddr, SIS_3300_CONTROL, 0x07000000);
		SET32(s->baseAddr, SIS_3300_CONTROL, 0x00000100 << lemo);
	}
}

void sis_3300_disableBankFullLemo(struct s_sis_3300 * s)
{
	SET32(s->baseAddr, SIS_3300_CONTROL, 0x07000000);
}

uint8_t sis_3300_bankFullLemoEnabled(struct s_sis_3300 * s)
{
	return ((GET32(s->baseAddr, SIS_3300_STATUS) & 0x0700) >> 8);
}

uint8_t sis_3300_getP2_TEST_IN(struct s_sis_3300 * s)
{
	return ((GET32(s->baseAddr, SIS_3300_STATUS) & SIS_3300_CTL_USEROUT_OFF) != 0 ? 1 : 0);
}

uint8_t sis_3300_getP2_RESET_IN(struct s_sis_3300 * s)
{
	return ((GET32(s->baseAddr, SIS_3300_STATUS) & SIS_3300_CTL_ENABLE_USEROUT) != 0 ? 1 : 0);
}

uint8_t sis_3300_getP2_SAMPLE_IN(struct s_sis_3300 * s)
{
	return ((GET32(s->baseAddr, SIS_3300_STATUS) & (SIS_3300_CTL_ENABLE_USEROUT << 1)) != 0 ? 1 : 0);
}


/* -------- IRQ Config Register ----------------------------------------- */

void	sis_3300_enableVMEIRQ(struct s_sis_3300 * s) 
{
 SET32(s->baseAddr, SIS_3300_IRQ_CONFIG, SIS_3300_IRQ_ENABLE); 
}

void	sis_3300_enableROAKIRQ(struct s_sis_3300 * s) 
{
 SET32(s->baseAddr, SIS_3300_IRQ_CONFIG, SIS_3300_IRQ_ROAK); 
}


/* -------- IRQ Control Register ------------------------------------------ */

void sis_3300_enableIRQ(struct s_sis_3300 * s, uint8_t source)
{
	switch (source) {
	case 0:
		SET32(s->baseAddr, SIS_3300_IRQ_CONTROL, SIS_3300_IRQ_ENABLE0);
		break;
	case 1:
		SET32(s->baseAddr, SIS_3300_IRQ_CONTROL, SIS_3300_IRQ_ENABLE1);
		break;
	case 2:
		SET32(s->baseAddr, SIS_3300_IRQ_CONTROL, SIS_3300_IRQ_ENABLE2);	
		break;
	case 3:
		SET32(s->baseAddr, SIS_3300_IRQ_CONTROL, SIS_3300_IRQ_ENABLE3);
		break;
	}
}

void sis_3300_disableIRQ(struct s_sis_3300 * s, uint8_t source)
{
	switch (source) {
	case 0:
		SET32(s->baseAddr, SIS_3300_IRQ_CONTROL, SIS_3300_IRQ_DISABLE0);
		break;
	case 1:
		SET32(s->baseAddr, SIS_3300_IRQ_CONTROL, SIS_3300_IRQ_DISABLE1);
		break;
	case 2:
		SET32(s->baseAddr, SIS_3300_IRQ_CONTROL, SIS_3300_IRQ_DISABLE2);	
		break;
	case 3:
		SET32(s->baseAddr, SIS_3300_IRQ_CONTROL, SIS_3300_IRQ_DISABLE3);
		break;
	}
}

uint8_t sis_3300_IRQEnabled(struct s_sis_3300 * s, uint8_t source)
{
	uint8_t ret = 0;
	
	switch (source) {
	case 0:
		ret = GET32(s->baseAddr, SIS_3300_IRQ_CONTROL) & SIS_3300_IRQ_ENABLE0;
		break;
	case 1:
		ret = GET32(s->baseAddr, SIS_3300_IRQ_CONTROL) & SIS_3300_IRQ_ENABLE1;
		break;
	case 2:
		ret = GET32(s->baseAddr, SIS_3300_IRQ_CONTROL) & SIS_3300_IRQ_ENABLE2;	
		break;
	case 3:
		ret = GET32(s->baseAddr, SIS_3300_IRQ_CONTROL) & SIS_3300_IRQ_ENABLE3;
		break;
	}
	
	return (ret != 0) ? 1 : 0;
}

void sis_3300_clearIRQ(struct s_sis_3300 * s, uint8_t source)
{
	switch (source) {
	case 0:
		SET32(s->baseAddr, SIS_3300_IRQ_CONTROL, SIS_3300_IRQ_DISABLE0 << 4);
		break;
	case 1:
		SET32(s->baseAddr, SIS_3300_IRQ_CONTROL, SIS_3300_IRQ_DISABLE1 << 4);
		break;
	case 2:
		SET32(s->baseAddr, SIS_3300_IRQ_CONTROL, SIS_3300_IRQ_DISABLE2 << 4);	
		break;
	case 3:
		SET32(s->baseAddr, SIS_3300_IRQ_CONTROL, SIS_3300_IRQ_DISABLE3 << 4);
		break;
	}
}

uint8_t sis_3300_getIRQStatusFlag(struct s_sis_3300 * s, uint8_t source)
{
	uint8_t ret = 0;
	
	switch (source) {
	case 0:
		ret = GET32(s->baseAddr, SIS_3300_IRQ_CONTROL) & 
			(SIS_3300_IRQ_DISABLE0 << 4);
		break;
	case 1:
		ret = GET32(s->baseAddr, SIS_3300_IRQ_CONTROL) & 
			(SIS_3300_IRQ_DISABLE0 << 4);
		break;
	case 2:
		ret = GET32(s->baseAddr, SIS_3300_IRQ_CONTROL) & 
			(SIS_3300_IRQ_DISABLE0 << 4);	
		break;
	case 3:
		ret = GET32(s->baseAddr, SIS_3300_IRQ_CONTROL) & 
			(SIS_3300_IRQ_DISABLE0 << 4);
		break;
	}
	
	return (ret != 0) ? 1 : 0;
}


uint8_t sis_3300_getIRQStatus(struct s_sis_3300 * s, uint8_t source)
{
	uint8_t ret = 0;
	
	switch (source) {
	case 0:
		ret = GET32(s->baseAddr, SIS_3300_IRQ_CONTROL) & SIS_3300_IRQ_SOURCE0_STAT;
		break;
	case 1:
		ret = GET32(s->baseAddr, SIS_3300_IRQ_CONTROL) & SIS_3300_IRQ_SOURCE1_STAT;
		break;
	case 2:
		ret = GET32(s->baseAddr, 
										SIS_3300_IRQ_CONTROL) & SIS_3300_IRQ_SOURCE2_STAT;	
		break;
	case 3:
		ret = GET32(s->baseAddr, SIS_3300_IRQ_CONTROL) & SIS_3300_IRQ_SOURCE3_STAT;
		break;
	}
	
	return (ret != 0) ? 1 : 0;
}

uint8_t sis_3300_getIRQInternalStatus(struct s_sis_3300 * s)
{
	return GET32(s->baseAddr, SIS_3300_IRQ_CONTROL) & SIS_3300_IRQ_INTERNAL_STAT;
}

uint8_t sis_3300_getIRQVMEStatus(struct s_sis_3300 * s)
{
	return GET32(s->baseAddr, SIS_3300_IRQ_CONTROL) & SIS_3300_IRQ_STAT;
}

/* ---------- Functions to be inlined ----------------------------------- */

uint32_t sis_3300_getTriggerEventDirectory(struct s_sis_3300 * s, uint8_t bank,
																					uint16_t event)
{
	if (event < 1024) {
		switch (bank) {
		case 0:
			return 
				GET32(s->baseAddr, SIS_3300_EVENT_DIRECTORY_BANK1_ALL_ADC + event * 4);
			break;
		case 1:
			return	
				GET32(s->baseAddr, SIS_3300_EVENT_DIRECTORY_BANK2_ALL_ADC + event * 4);
			break;
		}
	}
	return 0;
}

uint8_t sis_3300_getEventWrap(struct s_sis_3300 * s, uint8_t bank, uint16_t event)
{
	return (sis_3300_getTriggerEventDirectory(s, bank, event) & 0x80000) >> 19; 
}


uint32_t sis_3300_getPageBegin(struct s_sis_3300 * s, uint8_t bank, uint8_t channelGrp, uint16_t event)
{
	
	uint32_t offset = event * sis_3300_getPageSizeVal(s) * 4;
	switch (bank) {
	case 0:
		switch (channelGrp) {
		case 0:
			return SIS_3300_BANK1_GROUP1 + offset;
		case 1:
			return SIS_3300_BANK1_GROUP2 + offset;
		case 2:
			return SIS_3300_BANK1_GROUP3 + offset;
		case 3:
			return SIS_3300_BANK1_GROUP4 + offset;
		default:
			return 0;
		}
		
	case 1:
		switch (channelGrp) {
		case 0:
			return SIS_3300_BANK2_GROUP1 + offset;
		case 1:
			return SIS_3300_BANK2_GROUP2 + offset;
		case 2:
			return SIS_3300_BANK2_GROUP3 + offset;
		case 3:
			return SIS_3300_BANK2_GROUP4 + offset;
		default:
			return 0;
		}
	}
	
	return 0;
}

uint32_t sis_3300_getPageEnd(struct s_sis_3300 * s, uint8_t bank, uint8_t channelGrp, uint16_t event)
{
	return sis_3300_getPageBegin(s, bank, channelGrp, event + 1) - 4;
}


uint32_t sis_3300_getEventEnd(struct s_sis_3300 * s, uint8_t bank, uint8_t channelGrp, uint16_t event)
{															 
	return	sis_3300_getPageBegin(s, bank, channelGrp, 0) + 
		4 * ((sis_3300_getTriggerEventDirectory(s, bank, event) & 0x01ffff));
}

uint32_t sis_3300_getAddressCounter(struct s_sis_3300 * s, uint8_t bank)
{
	switch (bank) {
	case 0:
		return 
			GET32(s->baseAddr, SIS_3300_BANK1_GROUP1_ADR_COUNTER) & 0x01ffff;			
		break;
	case 1:
		return 
			GET32(s->baseAddr, SIS_3300_BANK2_GROUP1_ADR_COUNTER) & 0x01ffff;
		break;
	default:
		break;
	}
	
	return 0;
}

uint16_t sis_3300_getEventCounter(struct s_sis_3300 * s, uint8_t bank)
{
	uint16_t evtCnt = 0;

	switch (bank) {
	case 0:
		/* all groups contain same no. of events */
		evtCnt = GET32(s->baseAddr, SIS_3300_BANK1_GROUP1_EVT_COUNTER) & 0x0fff; 
		break;
	case 1:
		evtCnt = GET32(s->baseAddr, SIS_3300_BANK2_GROUP1_EVT_COUNTER) & 0x0fff;
		break;
	default:
		break;
	}
	
	return (evtCnt > 0) ? evtCnt - 1 : 0;
}

uint32_t sis_3300_getActualSampleValue(struct s_sis_3300 * s, uint8_t channelGrp)
{
	switch (channelGrp) {
	case 0:
		return	GET32(s->baseAddr, SIS_3300_ACTUAL_SAMPLE_VALUE_ADC12); 
		break;
	case 1:
		return	GET32(s->baseAddr, SIS_3300_ACTUAL_SAMPLE_VALUE_ADC34); 
		break;
	case 2:
		return	GET32(s->baseAddr, SIS_3300_ACTUAL_SAMPLE_VALUE_ADC56); 
		break;
	case 3:
		return	GET32(s->baseAddr, SIS_3300_ACTUAL_SAMPLE_VALUE_ADC78); 
		break;
	default:
		break;
	}
	
	return 0;
}

/* ---------------------------------------------------------------------- */

float sis_3300_getFrequency(struct s_sis_3300 * s)
{
	float freq;
	
	switch (sis_3300_getClockSource(s)) {
	case 0x0:
		freq = 100;
		break;
	case 0x1:
		freq = 50;
		break;
	case 0x2:
		freq = 25;
		break;
	case 0x3:
	 freq = 12.5;
	 break;
	case 0x4:
		freq = 6.25;
		break;
	case 0x5:
		freq = 3.125;
		break;
	default:										 /* front panel or P2 */
		freq = 0;									 /* unknown */
		break;
	}
	return freq;
}

uint32_t sis_3300_getPageSizeVal(struct s_sis_3300 * s)
{
	uint32_t pagesize;
	switch (sis_3300_getPageSize(s)) {
	case 0x0:
		pagesize = 0x20000;
		break;
	case 0x1:
		pagesize = 0x4000;
		break;
	case 0x2:
		pagesize = 0x1000;
		break;
	case 0x3:
		pagesize = 0x0800;
		break;
	case 0x4:
		pagesize = 0x0400;
		break;
	case 0x5:
		pagesize = 0x0200;
		break;
	case 0x6:
		pagesize = 0x0100;
		break;
	case 0x7:
		pagesize = 0x0080;
		break;
	default:
		pagesize = 0x20000;
		break;
	}
	
	return pagesize;
}

/* ---------------------------------------------------------------------- */

int sis_3300_initialize(struct s_sis_3300 * s)
{
	uint16_t ch;
	
	sis_3300_reset(s);

	switch (s->acqMode) {
	case SIS_3300_SINGLE_EVT:
		sis_3300_disableMultiEventMode(s);
		sis_3300_disableGateChaining(s);
		sis_3300_setPageSize(s, s->pageSize);
		s->maxNumEvts = 0;
		break;
		
	case SIS_3300_MULTI_EVT:
		sis_3300_enableMultiEventMode(s);
		sis_3300_disableGateChaining(s);
		sis_3300_setPageSize(s, s->pageSize);
		break;
		
	case SIS_3300_GATE_CHAINING:
		sis_3300_enableMultiEventMode(s);
		sis_3300_enableFrontPanelGateMode(s);
		sis_3300_enableGateChaining(s);
		s->frontStartStop = 1;
		s->autoStart = 0;
		s->wrapAround = 0;
		s->triggerRouting = 0;
		break;
		
	default:
		break;
	}
		
	sis_3300_setClockSource(s, s->clockSource);
	
	if (s->maxNumEvts != 0) {
		sis_3300_setMaxNumberOfEvents(s, s->maxNumEvts);
	}
	if (s->startDelay != 0) { 
		sis_3300_setStartDelay(s, s->startDelay);
		sis_3300_enableStartDelay(s);
	}
	if (s->stopDelay) {
		sis_3300_setStopDelay(s, s->stopDelay);
		sis_3300_enableStopDelay(s);
	}
	if (s->frontStartStop) {
		sis_3300_enableFrontStartStop(s);
	}
	if (s->autoBankSwitch) {
		sis_3300_enableAutoBankSwitch(s);
		if (s->bankFullLemo) {
			sis_3300_enableBankFullLemo(s, s->bankFullLemo - 1);
		}
	}
	if (s->autoStart) {
		sis_3300_enableAutostart(s);
	}	
	if (s->wrapAround) {
		sis_3300_enableWrapAroundMode(s);
	}
	if (s->triggerArmedStart) {
		sis_3300_enableTriggerArmedStart(s);
	}
	if (s->triggerOut) {
		sis_3300_enableTriggerOut(s);
	}
	if (s->inverseTriggerOut) {
		sis_3300_enableInverseTriggerOut(s);
	}
	if (s->triggerRouting) {
		sis_3300_enableTriggerRouting(s);
	}
	
	for (ch = 0; ch < 8; ch++) {
		uint8_t chGrp = ch / 2;
		if (s->triggerOn[chGrp] > 0) {
			sis_3300_setTriggerPeakingTime(s, chGrp, s->peakingTime[chGrp]);
			sis_3300_setTriggerGapTime(s, chGrp, s->gapTime[chGrp]);
			sis_3300_setThreshold(s, ch, s->threshold[ch]);
			sis_3300_setTriggerMode(s, ch, s->triggerMode[ch]);
			
			if ((ch % 2) == 1) {
				if (s->triggerOn[chGrp] == 2) {
					sis_3300_enableFIRTestEven(s, chGrp);
					sis_3300_enableFIRTestMode(s, chGrp);
				} else if (s->triggerOn[chGrp] == 3) {
					sis_3300_disableFIRTestEven(s, chGrp);	
					sis_3300_enableFIRTestMode(s, chGrp);
				}
				if (s->pulsMode[chGrp] != 0) {
					sis_3300_enablePulseMode(s, chGrp);
					sis_3300_setTriggerPulseLength(s, chGrp, s->pulsLength[chGrp]);
				}
				sis_3300_enableFIRTrigger(s, chGrp);
			}
		}
	}		

	if (s->blockTransOn == 1) sis_3300_enable_bma(s);

	if (s->dumpRegsOnInit) sis_3300_dumpRegisters(s);

	return 0;
}


static char * showBits(uint32_t val)
{
	static char buffer[64]; 
	unsigned int c;
	int i;

	for (i = 31, c = 0; i >= 0; i--) {
		if ((i != 31) && ((i + 1) % 8) == 0) {
			buffer[c++] = ' ';
		}
		buffer[c++] = (val >> i) & 0x01 ? '1' : '0';
	}
	
	buffer[c++] = '\0';

	return buffer;
}

uint32_t sis_3300_getRevision(struct s_sis_3300 * s)
{
  return GET32(s->baseAddr, SIS_3300_MODULE_ID);
}

void sis_3300_dumpRegisters(struct s_sis_3300 * s)
{
	unsigned int i;

	char fileName[200];
	FILE * f;

	sprintf(fileName, "%s_dump.dat", s->moduleName);
	f = fopen(fileName,	"w");
	fprintf(f, "ModuleId: 0x%04x	Revision: 0x%04x\n", 
					sis_3300_getRevision(s) >> 16 & 0xffff,
					sis_3300_getRevision(s) & 0xffff); 
	fprintf(f, "Control Register:\n0x%08x\t%s\n", 
					GET32(s->baseAddr, SIS_3300_CONTROL),
					showBits(GET32(s->baseAddr, SIS_3300_CONTROL))
					);
	
	fprintf(f, "Acquisition Control Register:\n0x%08x\t%s\n", 
					GET32(s->baseAddr, SIS_3300_ACQ_STATUS),
					showBits(GET32(s->baseAddr, SIS_3300_ACQ_STATUS))
					);

	fprintf(f, "Bank0 armed: %d\n", 
					sis_3300_bankArmed(s, 0));
	fprintf(f, "Bank1 armed: %d\n", 
					sis_3300_bankArmed(s, 1));
	fprintf(f, "Maximum number Events: %d (%d)\n",
					sis_3300_getMaxNumberOfEvents(s), s->maxNumEvts);
	fprintf(f, "Bank0 Event Counter: %d\n",
					sis_3300_getEventCounter(s, 0));
	fprintf(f, "Bank1 Event Counter: %d\n",
					sis_3300_getEventCounter(s, 1));
	fprintf(f, "AutoBankSwitch: %d\n",
					sis_3300_autoBankSwitchEnabled(s));
	fprintf(f, "AutoStart: %d\n",
					sis_3300_autostartEnabled(s));
	fprintf(f, "TriggerArmedStart: %d\n",
					sis_3300_triggerArmedStartEnabled(s));
	fprintf(f, "TriggerRouting: %d\n",
					sis_3300_triggerRoutingEnabled(s));
	fprintf(f, "TriggerOut: %d\n",
					sis_3300_triggerOutEnabled(s));
	fprintf(f, "InverseTriggerOut: %d\n",
					sis_3300_inverseTriggerOutEnabled(s));
	fprintf(f, "BankFullLemoOut: %d\n",
					sis_3300_bankFullLemoEnabled(s));
	fprintf(f, "Clock Source: %d ( ^= %f Hz)\n", 
					sis_3300_getClockSource(s),
					sis_3300_getFrequency(s));
	fprintf(f, "Start Delay: %d\n", sis_3300_getStartDelay(s));
	fprintf(f, "Stop Delay: %d\n", sis_3300_getStopDelay(s));
	fprintf(f, "Time Stamp Predivider: %d\n",
					sis_3300_getTimeStampPredivider(s));
	fprintf(f, "Page Size: %d (^= %d samples)\n", 
					sis_3300_getPageSize(s),
					sis_3300_getPageSizeVal(s)); 
	fprintf(f, "Event Configuration Register: \n0x%08x\t%s\n",
					GET32(s->baseAddr, SIS_3300_GROUP1_EVT_CONFIG),
					showBits(GET32(s->baseAddr, SIS_3300_GROUP1_EVT_CONFIG))
					); 
	for (i = 0; i < NOF_CHANNELS; i++) {
		fprintf(f, "Threshold channel %d: %d\t"
						"Trigger Mode: 0x%02x\n", i,
						sis_3300_getThreshold(s, i),
						sis_3300_getTriggerMode(s, i)
						);
	}
	fprintf(f, "Clock Predivider Register: %d\n",
					sis_3300_getClockPredivider(s));
	
	fprintf(f, "Number of Samples Register: %d\n\n",
					sis_3300_getNumberOfSamples(s));
	for (i = 0; i < NOF_GROUPS; i++) {
		fprintf(f, "Trigger Setup Register channelGrp: %d\n", i);
		fprintf(f, "0x%08x\t%s\n", 
						sis_3300_getTriggerSetupRegister(s, i),
						showBits(sis_3300_getTriggerSetupRegister(s, i)));
		fprintf(f, "TriggerPulseLength: %d\n", 
						sis_3300_getTriggerPulseLength(s, i));
		fprintf(f, "TriggerGapTime: %d\n", 
						sis_3300_getTriggerGapTime(s, i));
		fprintf(f, "TriggerPeakingTime: %d\n", 
						sis_3300_getTriggerPeakingTime(s, i));
		fprintf(f, "TestEven: %d	FIRTestMode: %d	PulseMode: %d	"
						"FIRTrigger: %d\n", 
						sis_3300_FIRTestEvenEnabled(s, i),
						sis_3300_FIRTestModeEnabled(s, i),
						sis_3300_pulseModeEnabled(s, i),
						sis_3300_FIRTriggerEnabled(s, i)
						);
		fprintf(f, "\n");
	}

	for (i = 0; i < NOF_GROUPS; i++) {
		fprintf(f, "Actual Sample Value ADC%d: 0x%08x\n", i,
						sis_3300_getActualSampleValue(s, i));
	}
	fclose(f);
	sprintf(msg, "[dumpRegisters] Dumped setings to file %s\n", fileName);
	f_ut_send_msg("__sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
}


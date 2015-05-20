/*_______________________________________________________[C LIBRARY FUNCTIONS]
//////////////////////////////////////////////////////////////////////////////
//! \file			madc32fct.c
//! \brief			Code for module MADC32
//! \details		Implements functions to handle modules of type Mesytec MADC32
//!
//! $Author: Marabou $
//! $Mail			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.19 $
//! $Date: 2011-05-18 11:04:49 $
////////////////////////////////////////////////////////////////////////////*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ces/uiocmd.h>
#include <ces/bmalib.h>
#include <errno.h>

#include "gd_readout.h"

#include "madc32.h"
#include "madc32_database.h"
#include "madc32_protos.h"

#include "root_env.h"
#include "mapping_database.h"
#include "bmaErrlist.h"

#include "err_mask_def.h"
#include "errnum_def.h"

int numData;
int rdoWc;

int evtWc;
int lastData;

char msg[256];

struct s_madc32 * be;


struct s_madc32 * madc32_alloc(char * moduleName, struct s_mapDescr * md, int serial)
{
	struct s_madc32 * s;
	int firmware, mainRev;

	s = (struct s_madc32 *) calloc(1, sizeof(struct s_madc32));
	if (s != NULL) {
		s->md = md;
		strcpy(s->moduleName, moduleName);
		strcpy(s->prefix, "m_read_meb");
		strcpy(s->mpref, "madc32: ");
		s->serial = serial;
		s->verbose = FALSE;
		s->dumpRegsOnInit = FALSE;

		s->mcstSignature = 0x0;
		s->cbltSignature = 0x0;
		s->firstInChain = FALSE;
		s->lastInChain = FALSE;

		firmware = GET16(s->md->vmeBase, MADC32_FIRMWARE_REV);
		mainRev = (firmware >> 8) & 0xff;
		s->memorySize = (mainRev >= 2) ? (8*1024 + 2) : (1024 + 2);
	} else {
		sprintf(msg, "[%salloc] %s: Can't allocate madc32 struct", s->mpref, s->moduleName);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
	}
	return s;
}

void madc32_initialize(struct s_madc32 * s)
{ }

bool_t madc32_useBLT(struct s_madc32 * s) {
	return s->blockXfer;
}

void madc32_soft_reset(struct s_madc32 * s)
{
  SET16(s->md->vmeBase, MADC32_SOFT_RESET, 0x1);
}

uint16_t madc32_getThreshold(struct s_madc32 * s, uint16_t channel)
{
	uint16_t thresh = 0;
	if (channel < MADC_NOF_CHANNELS) thresh = GET16(s->md->vmeBase, MADC32_THRESHOLD + sizeof(uint16_t) * channel) & MADC32_THRESHOLD_MASK;
	return thresh;
}

void madc32_setThreshold_db(struct s_madc32 * s, uint16_t channel)
{
	if (channel < MADC_NOF_CHANNELS) madc32_setThreshold(s, channel, s->threshold[channel]);
}

void madc32_setThreshold(struct s_madc32 * s, uint16_t channel, uint16_t thresh)
{
	if (channel < MADC_NOF_CHANNELS) SET16(s->md->vmeBase, MADC32_THRESHOLD + sizeof(uint16_t) * channel, thresh & MADC32_THRESHOLD_MASK);
}

void madc32_setAddrReg_db(struct s_madc32 * s) { madc32_setAddrReg(s, s->addrReg); }

void madc32_setAddrReg(struct s_madc32 * s, uint16_t vmeAddr)
{
	if (vmeAddr) {
		SET16(s->md->vmeBase, MADC32_ADDR_REG, vmeAddr);
		SET16(s->md->vmeBase, MADC32_ADDR_SOURCE, MADC32_ADDR_SOURCE_REG);
		s->md->vmeBase = vmeAddr;
	}
}

uint16_t madc32_getAddrReg(struct s_madc32 * s)
{
	uint16_t source = GET16(s->md->vmeBase, MADC32_ADDR_SOURCE);
	if (source & MADC32_ADDR_SOURCE_REG) return GET16(s->md->vmeBase, MADC32_ADDR_REG);
	else return 0;
}

void madc32_setModuleId_db(struct s_madc32 * s) { madc32_setModuleId(s, s->moduleId); }

void madc32_setModuleId(struct s_madc32 * s, uint16_t id)
{
	SET16(s->md->vmeBase, MADC32_MODULE_ID, id & MADC32_MODULE_ID_MASK);
}

uint16_t madc32_getModuleId(struct s_madc32 * s)
{
	return GET16(s->md->vmeBase, MADC32_MODULE_ID) & MADC32_MODULE_ID_MASK;
}

uint16_t madc32_getFifoLength(struct s_madc32 * s)
{
	return GET16(s->md->vmeBase, MADC32_BUFFER_DATA_LENGTH) & MADC32_BUFFER_DATA_LENGTH_MASK;
}

void madc32_setDataWidth_db(struct s_madc32 * s) { madc32_setDataWidth(s, s->dataWidth); }

void madc32_setDataWidth(struct s_madc32 * s, uint16_t width)
{
	SET16(s->md->vmeBase, MADC32_DATA_LENGTH_FORMAT, width & MADC32_DATA_LENGTH_FORMAT_MASK);
}

uint16_t madc32_getDataWidth(struct s_madc32 * s)
{
	return GET16(s->md->vmeBase, MADC32_DATA_LENGTH_FORMAT) & MADC32_DATA_LENGTH_FORMAT_MASK;
}

void madc32_setMultiEvent_db(struct s_madc32 * s) { madc32_setMultiEvent(s, s->multiEvent); }

void madc32_setMultiEvent(struct s_madc32 * s, uint16_t mode)
{
	SET16(s->md->vmeBase, MADC32_MULTI_EVENT, mode & MADC32_MULTI_EVENT_MASK);
}

uint16_t madc32_getMultiEvent(struct s_madc32 * s)
{
	return GET16(s->md->vmeBase, MADC32_MULTI_EVENT) & MADC32_MULTI_EVENT_MASK;
}

void madc32_setXferData_db(struct s_madc32 * s) { madc32_setXferData(s, s->xferData); }

void madc32_setXferData(struct s_madc32 * s, uint16_t wc)
{
	SET16(s->md->vmeBase, MADC32_MAX_XFER_DATA, wc & MADC32_MAX_XFER_DATA_MASK);
}

uint16_t madc32_getXferData(struct s_madc32 * s)
{
	return GET16(s->md->vmeBase, MADC32_MAX_XFER_DATA) & MADC32_MAX_XFER_DATA_MASK;
}

void madc32_setMarkingType_db(struct s_madc32 * s) { madc32_setMarkingType(s, s->markingType); }

void madc32_setMarkingType(struct s_madc32 * s, uint16_t type)
{
	SET16(s->md->vmeBase, MADC32_MARKING_TYPE, type & MADC32_MARKING_TYPE_MASK);
}

uint16_t madc32_getMarkingType(struct s_madc32 * s)
{
	return GET16(s->md->vmeBase, MADC32_MARKING_TYPE) & MADC32_MARKING_TYPE_MASK;
}

void madc32_setBankOperation_db(struct s_madc32 * s) { madc32_setBankOperation(s, s->bankOperation); }

void madc32_setBankOperation(struct s_madc32 * s, uint16_t oper)
{
	SET16(s->md->vmeBase, MADC32_BANK_OPERATION, oper & MADC32_BANK_OPERATION_MASK);
}

uint16_t madc32_getBankOperation(struct s_madc32 * s)
{
	return GET16(s->md->vmeBase, MADC32_BANK_OPERATION) & MADC32_BANK_OPERATION_MASK;
}

void madc32_setAdcResolution_db(struct s_madc32 * s) { madc32_setAdcResolution(s, s->adcResolution); }

void madc32_setAdcResolution(struct s_madc32 * s, uint16_t res)
{
	SET16(s->md->vmeBase, MADC32_ADC_RESOLUTION, res & MADC32_ADC_RESOLUTION_MASK);
}

uint16_t madc32_getAdcResolution(struct s_madc32 * s)
{
	return GET16(s->md->vmeBase, MADC32_ADC_RESOLUTION) & MADC32_ADC_RESOLUTION_MASK;
}

void madc32_setOutputFormat_db(struct s_madc32 * s) { madc32_setOutputFormat(s, s->outputFormat); }

void madc32_setOutputFormat(struct s_madc32 * s, uint16_t format)
{
	SET16(s->md->vmeBase, MADC32_OUTPUT_FORMAT, format & MADC32_OUTPUT_FORMAT_MASK);
}

uint16_t madc32_getOutputFormat(struct s_madc32 * s)
{
	return GET16(s->md->vmeBase, MADC32_OUTPUT_FORMAT) & MADC32_OUTPUT_FORMAT_MASK;
}

void madc32_setAdcOverride_db(struct s_madc32 * s) { madc32_setAdcOverride(s, s->adcOverride); }

void madc32_setAdcOverride(struct s_madc32 * s, uint16_t over)
{
	SET16(s->md->vmeBase, MADC32_ADC_OVERRIDE, over & MADC32_ADC_RESOLUTION_MASK);
}

uint16_t madc32_getAdcOverride(struct s_madc32 * s)
{
	return GET16(s->md->vmeBase, MADC32_ADC_OVERRIDE) & MADC32_ADC_RESOLUTION_MASK;
}

void madc32_setSlidingScaleOff_db(struct s_madc32 * s) { madc32_setSlidingScaleOff(s, s->slidingScaleOff); }

void madc32_setSlidingScaleOff(struct s_madc32 * s, bool_t flag)
{
	uint16_t f = flag ? 1 : 0;
	SET16(s->md->vmeBase, MADC32_SLIDING_SCALE_OFF, f);
}

bool_t madc32_getSlidingScaleOff(struct s_madc32 * s)
{
	uint16_t f = GET16(s->md->vmeBase, MADC32_SLIDING_SCALE_OFF) & MADC32_SLIDING_SCALE_OFF_MASK;
	return (f != 0) ? TRUE : FALSE;
}

void madc32_setSkipOutOfRange_db(struct s_madc32 * s) { madc32_setSkipOutOfRange(s, s->skipOutOfRange); }

void madc32_setSkipOutOfRange(struct s_madc32 * s, bool_t flag)
{
	uint16_t f = flag ? 1 : 0;
	SET16(s->md->vmeBase, MADC32_SKIP_OUT_OF_RANGE, f);
}

bool_t madc32_getSkipOutOfRange(struct s_madc32 * s)
{
	uint16_t f = GET16(s->md->vmeBase, MADC32_SKIP_OUT_OF_RANGE) & MADC32_SKIP_OUT_OF_RANGE_MASK;
	return (f != 0) ? TRUE : FALSE;
}

void madc32_setIgnoreThresholds_db(struct s_madc32 * s) { madc32_setIgnoreThresholds(s, s->ignoreThresh); }

void madc32_setIgnoreThresholds(struct s_madc32 * s, bool_t flag)
{
	uint16_t f = flag ? 1 : 0;
	SET16(s->md->vmeBase, MADC32_IGNORE_THRESHOLDS, f);
}

bool_t madc32_getIgnoreThresholds(struct s_madc32 * s)
{
	uint16_t f = GET16(s->md->vmeBase, MADC32_IGNORE_THRESHOLDS) & MADC32_IGNORE_THRESH_MASK;
	return (f != 0) ? TRUE : FALSE;
}

void madc32_setHoldDelay_db(struct s_madc32 * s, uint16_t gg) { madc32_setHoldDelay(s, gg, s->ggHoldDelay[gg]); }

void madc32_setHoldDelay(struct s_madc32 * s, uint16_t gg, uint16_t delay)
{
	int offs;
	switch (gg) {
		case 0: offs = MADC32_GG_HOLD_DELAY_0; break;
		case 1: offs = MADC32_GG_HOLD_DELAY_1; break;
		default: return;
	}
	SET16(s->md->vmeBase, offs, delay & MADC32_GG_HOLD_DELAY_MASK);
}

uint16_t madc32_getHoldDelay(struct s_madc32 * s, uint16_t gg)
{
	int offs;
	switch (gg) {
		case 0: offs = MADC32_GG_HOLD_DELAY_0; break;
		case 1: offs = MADC32_GG_HOLD_DELAY_1; break;
		default: return 0;
	}
	return GET16(s->md->vmeBase, offs) & MADC32_GG_HOLD_DELAY_MASK;
}

void madc32_setHoldWidth_db(struct s_madc32 * s, uint16_t gg) { madc32_setHoldWidth(s, gg, s->ggHoldWidth[gg]); }

void madc32_setHoldWidth(struct s_madc32 * s, uint16_t gg, uint16_t width)
{
	int offs;
	switch (gg) {
		case 0: offs = MADC32_GG_HOLD_WIDTH_0; break;
		case 1: offs = MADC32_GG_HOLD_WIDTH_1; break;
		default: return;
	}
	SET16(s->md->vmeBase, offs, width & MADC32_GG_HOLD_WIDTH_MASK);
}

uint16_t madc32_getHoldWidth(struct s_madc32 * s, uint16_t gg)
{
	int offs;
	switch (gg) {
		case 0: offs = MADC32_GG_HOLD_WIDTH_0; break;
		case 1: offs = MADC32_GG_HOLD_WIDTH_1; break;
		default: return 0;
	}
	return GET16(s->md->vmeBase, offs) & MADC32_GG_HOLD_WIDTH_MASK;
}

void madc32_useGG_db(struct s_madc32 * s) { madc32_useGG(s, s->useGG); }

void madc32_useGG(struct s_madc32 * s, uint16_t gg)
{
	SET16(s->md->vmeBase, MADC32_USE_GATE_GENERATOR, gg & MADC32_USE_GATE_GENERATOR_MASK);
}

uint16_t madc32_getGGUsed(struct s_madc32 * s)
{
	return GET16(s->md->vmeBase, MADC32_USE_GATE_GENERATOR) & MADC32_USE_GATE_GENERATOR_MASK;
}

void madc32_setInputRange_db(struct s_madc32 * s) { madc32_setInputRange(s, s->inputRange); }

void madc32_setInputRange(struct s_madc32 * s, uint16_t range)
{
	SET16(s->md->vmeBase, MADC32_INPUT_RANGE, range & MADC32_INPUT_RANGE_MASK);
}

uint16_t madc32_getInputRange(struct s_madc32 * s)
{
	return GET16(s->md->vmeBase, MADC32_INPUT_RANGE) & MADC32_INPUT_RANGE_MASK;
}

void madc32_setEclTerm_db(struct s_madc32 * s) { madc32_setEclTerm(s, s->eclTerm); }

void madc32_setEclTerm(struct s_madc32 * s, uint16_t term)
{
	SET16(s->md->vmeBase, MADC32_ECL_TERMINATORS, term & MADC32_ECL_TERMINATORS_MASK);
}

uint16_t madc32_getEclTerm(struct s_madc32 * s)
{
	return GET16(s->md->vmeBase, MADC32_ECL_TERMINATORS) & MADC32_ECL_TERMINATORS_MASK;
}

void madc32_setEclG1Osc_db(struct s_madc32 * s) { madc32_setEclG1Osc(s, s->eclG1Osc); }

void madc32_setEclG1Osc(struct s_madc32 * s, uint16_t go)
{
	SET16(s->md->vmeBase, MADC32_ECL_G1_OSC, go & MADC32_ECL_G1_OSC_MASK);
}

uint16_t madc32_getEclG1Osc(struct s_madc32 * s)
{
	return GET16(s->md->vmeBase, MADC32_ECL_G1_OSC) & MADC32_ECL_G1_OSC_MASK;
}

void madc32_setEclFclRts_db(struct s_madc32 * s) { madc32_setEclFclRts(s, s->eclFclRts); }

void madc32_setEclFclRts(struct s_madc32 * s, uint16_t fr)
{
	SET16(s->md->vmeBase, MADC32_ECL_FCL_RTS, fr & MADC32_ECL_FCL_RTS_MASK);
}

uint16_t madc32_getEclFclRts(struct s_madc32 * s)
{
	return GET16(s->md->vmeBase, MADC32_ECL_FCL_RTS) & MADC32_ECL_FCL_RTS_MASK;
}

void madc32_setNimG1Osc_db(struct s_madc32 * s) { madc32_setNimG1Osc(s, s->nimG1Osc); }

void madc32_setNimG1Osc(struct s_madc32 * s, uint16_t go)
{
	SET16(s->md->vmeBase, MADC32_NIM_G1_OSC, go & MADC32_NIM_G1_OSC_MASK);
}

uint16_t madc32_getNimG1Osc(struct s_madc32 * s)
{
	return GET16(s->md->vmeBase, MADC32_NIM_G1_OSC) & MADC32_NIM_G1_OSC_MASK;
}


void madc32_setNimFclRts_db(struct s_madc32 * s) { madc32_setNimFclRts(s, s->nimFclRts); }

void madc32_setNimFclRts(struct s_madc32 * s, uint16_t fr)
{
	SET16(s->md->vmeBase, MADC32_NIM_FCL_RTS, fr & MADC32_NIM_FCL_RTS_MASK);
}

uint16_t madc32_getNimFclRts(struct s_madc32 * s)
{
	return GET16(s->md->vmeBase, MADC32_NIM_FCL_RTS) & MADC32_NIM_FCL_RTS_MASK;
}

void madc32_setNimBusy_db(struct s_madc32 * s) { madc32_setNimBusy(s, s->nimBusy); }

void madc32_setNimBusy(struct s_madc32 * s, uint16_t busy)
{
	SET16(s->md->vmeBase, MADC32_NIM_BUSY, busy & MADC32_NIM_BUSY_MASK);
}

uint16_t madc32_getNimBusy(struct s_madc32 * s)
{
	return GET16(s->md->vmeBase, MADC32_NIM_BUSY) & MADC32_NIM_BUSY_MASK;
}

void madc32_setPulserStatus_db(struct s_madc32 * s) { madc32_setPulserStatus(s, s->pulserStatus); }

void madc32_setPulserStatus(struct s_madc32 * s, uint16_t mode)
{
	SET16(s->md->vmeBase, MADC32_PULSER_STATUS, mode & MADC32_PULSER_STATUS_MASK);
}

uint16_t madc32_getPulserStatus(struct s_madc32 * s)
{
	return GET16(s->md->vmeBase, MADC32_PULSER_STATUS) & MADC32_PULSER_STATUS_MASK;
}

void madc32_setTsSource_db(struct s_madc32 * s) { madc32_setTsSource(s, s->ctraTsSource); }

void madc32_setTsSource(struct s_madc32 * s, uint16_t source)
{
	SET16(s->md->vmeBase, MADC32_CTRA_TS_SOURCE, source & MADC32_CTRA_TS_SOURCE_MASK);
}

uint16_t madc32_getTsSource(struct s_madc32 * s)
{
	return GET16(s->md->vmeBase, MADC32_CTRA_TS_SOURCE) & MADC32_CTRA_TS_SOURCE_MASK;
}

void madc32_setTsDivisor_db(struct s_madc32 * s) { madc32_setTsDivisor(s, s->ctraTsDivisor); }

void madc32_setTsDivisor(struct s_madc32 * s, uint16_t div)
{
	SET16(s->md->vmeBase, MADC32_CTRA_TS_DIVISOR, div & MADC32_CTRA_TS_DIVISOR_MASK);
}

uint16_t madc32_getTsDivisor(struct s_madc32 * s)
{
	return GET16(s->md->vmeBase, MADC32_CTRA_TS_DIVISOR) & MADC32_CTRA_TS_DIVISOR_MASK;
}

void madc32_moduleInfo(struct s_madc32 * s)
{
	int firmware, mainRev, subRev;
	firmware = GET16(s->md->vmeBase, MADC32_FIRMWARE_REV);
	mainRev = (firmware >> 8) & 0xff;
	subRev = firmware & 0xff;
	sprintf(msg, "[%smodule_info] %s: phys addr %#lx, mapped to %#lx, firmware %02x.%02x (%dk memory)",
									s->mpref,
									s->moduleName,
									s->md->physAddrVME,
									s->md->vmeBase,
									mainRev, subRev, (mainRev >= 2) ? 8 : 1);
	f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
}

void madc32_setPrefix(struct s_madc32 * s, char * prefix)
{
	strcpy(s->prefix, prefix);
	strcpy(s->mpref, "");
}

bool_t madc32_fillStruct(struct s_madc32 * s, char * file)
{
	char res[256];
	char mnUC[256];
	const char * sp;
	int i;

	if (root_env_read(file) < 0) {
		sprintf(msg, "[%sfill_struct] %s: Error reading file %s", s->mpref, s->moduleName, file);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		return FALSE;
	}

	sprintf(msg, "[%sfill_struct] %s: Reading settings from file %s", s->mpref, s->moduleName, file);
	f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);

	s->verbose = root_env_getval_b("MADC32.VerboseMode", FALSE);

	s->dumpRegsOnInit = root_env_getval_b("MADC32.DumpRegisters", FALSE);

	sp = root_env_getval_s("MADC32.ModuleName", "");
	if (strcmp(s->moduleName, "madc32") != 0 && strcmp(sp, s->moduleName) != 0) {
		sprintf(msg, "[%sfill_struct] %s: File %s contains wrong module name - %s", s->mpref, s->moduleName, file, sp);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		return FALSE;
	}
	strcpy(mnUC, sp);
	mnUC[0] = toupper(mnUC[0]);

	sprintf(res, "MADC32.%s.BlockXfer", mnUC);
	s->blockXfer = root_env_getval_b(res, FALSE);

	sprintf(res, "MADC32.%s.RepairRawData", mnUC);
	s->repairRawData = root_env_getval_b(res, FALSE);

	for (i = 0; i < MADC_NOF_CHANNELS; i++) {
		sprintf(res, "MADC32.%s.Thresh.%d", mnUC, i);
		s->threshold[i] = root_env_getval_i(res, MADC32_THRESHOLD_DEFAULT);
	}

	sprintf(res, "MADC32.%s.AddrSource", mnUC);
	s->addrSource = root_env_getval_i(res, MADC32_ADDR_SOURCE_DEFAULT);

	if (s->addrSource == MADC32_ADDR_SOURCE_REG) {
		sprintf(res, "MADC32.%s.AddrReg", mnUC);
		s->addrReg = root_env_getval_i(res, 0);
		if (s->addrReg == 0) {
			sprintf(msg, "[%sfill_struct] %s: vme addr missing (addr source = reg)", s->mpref, s->moduleName);
			f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		}
	}

	sprintf(res, "MADC32.%s.MCSTSignature", mnUC);
	s->mcstSignature = root_env_getval_x(res, 0x0);
	sprintf(res, "MADC32.%s.CBLTSignature", mnUC);
	s->cbltSignature = root_env_getval_x(res, 0x0);
	sprintf(res, "MADC32.%s.FirstInChain", mnUC);
	s->firstInChain = root_env_getval_b(res, FALSE);
	sprintf(res, "MADC32.%s.LastInChain", mnUC);
	s->lastInChain = root_env_getval_b(res, FALSE);

	sprintf(res, "MADC32.%s.ModuleId", mnUC);
	s->moduleId = root_env_getval_i(res, MADC32_MODULE_ID_DEFAULT);

	sprintf(res, "MADC32.%s.FifoLength", mnUC);
	s->fifoLength = root_env_getval_i(res, 0);

#if 0
	if (s->fifoLength == 0) {
		sprintf(msg, "[%sfill_struct] %s: fifo length missing", s->mpref, s->moduleName);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
	}
#endif

	sprintf(res, "MADC32.%s.DataWidth", mnUC);
	s->dataWidth = root_env_getval_i(res, MADC32_DATA_LENGTH_FORMAT_DEFAULT);

	sprintf(res, "MADC32.%s.MultiEvent", mnUC);
	s->multiEvent = root_env_getval_i(res, MADC32_MULTI_EVENT_DEFAULT);

	sprintf(res, "MADC32.%s.XferData", mnUC);
	s->xferData = root_env_getval_i(res, MADC32_MAX_XFER_DATA_DEFAULT);

	sprintf(res, "MADC32.%s.MarkingType", mnUC);
	s->markingType = root_env_getval_i(res, MADC32_MARKING_TYPE_DEFAULT);

	sprintf(res, "MADC32.%s.BankOperation", mnUC);
	s->bankOperation = root_env_getval_i(res, MADC32_BANK_OPERATION_DEFAULT);

	sprintf(res, "MADC32.%s.AdcResolution", mnUC);
	s->adcResolution = root_env_getval_i(res, MADC32_ADC_RESOLUTION_DEFAULT);

	sprintf(res, "MADC32.%s.OutputFormat", mnUC);
	s->outputFormat = root_env_getval_i(res, MADC32_OUTPUT_FORMAT_DEFAULT);

	sprintf(res, "MADC32.%s.AdcOverride", mnUC);
	s->adcOverride = root_env_getval_i(res, MADC32_ADC_OVERWRITE_DEFAULT);
	if (s->adcOverride == MADC32_ADC_OVERWRITE_DEFAULT) s->adcOverride = s->adcResolution;

	sprintf(res, "MADC32.%s.SlidingScaleOff", mnUC);
	s->slidingScaleOff = root_env_getval_b(res, FALSE);

	sprintf(res, "MADC32.%s.SkipOutOfRange", mnUC);
	s->skipOutOfRange = root_env_getval_b(res, FALSE);

	for (i = 0; i <= 1; i++) {
		sprintf(res, "MADC32.%s.HoldDelay.%d", mnUC, i);
		s->ggHoldDelay[i] = root_env_getval_i(res, MADC32_GG_HOLD_DELAY_DEFAULT);
	}

	for (i = 0; i <= 1; i++) {
		sprintf(res, "MADC32.%s.HoldWidth.%d", mnUC, i);
		s->ggHoldWidth[i] = root_env_getval_i(res, MADC32_GG_HOLD_WIDTH_DEFAULT);
	}

	sprintf(res, "MADC32.%s.UseGG", mnUC);
	s->useGG = root_env_getval_i(res, MADC32_USE_GATE_GENERATOR_DEFAULT);

	sprintf(res, "MADC32.%s.InputRange", mnUC);
	s->inputRange = root_env_getval_i(res, MADC32_INPUT_RANGE_DEFAULT);

	sprintf(res, "MADC32.%s.EclTerm", mnUC);
	s->eclTerm = root_env_getval_i(res, MADC32_ECL_TERMINATORS_DEFAULT);

	sprintf(res, "MADC32.%s.EclG1Osc", mnUC);
	s->eclG1Osc = root_env_getval_i(res, MADC32_ECL_G1_OSC_DEFAULT);

	sprintf(res, "MADC32.%s.EclFclRts", mnUC);
	s->eclFclRts = root_env_getval_i(res, MADC32_NIM_FCL_RTS_DEFAULT);

	sprintf(res, "MADC32.%s.NimG1Osc", mnUC);
	s->nimG1Osc = root_env_getval_i(res, MADC32_NIM_G1_OSC_DEFAULT);

	sprintf(res, "MADC32.%s.NimFclRts", mnUC);
	s->nimFclRts = root_env_getval_i(res, MADC32_NIM_FCL_RTS_DEFAULT);

	sprintf(res, "MADC32.%s.NimBusy", mnUC);
	s->nimBusy = root_env_getval_i(res, MADC32_NIM_BUSY_DEFAULT);

	sprintf(res, "MADC32.%s.PulserStatus", mnUC);
	s->pulserStatus = root_env_getval_i(res, MADC32_PULSER_STATUS_DEFAULT);

	sprintf(res, "MADC32.%s.TsSource", mnUC);
	s->ctraTsSource = root_env_getval_i(res, MADC32_CTRA_TS_SOURCE_DEFAULT);

	sprintf(res, "MADC32.%s.TsDivisor", mnUC);
	s->ctraTsDivisor = root_env_getval_i(res, MADC32_CTRA_TS_DIVISOR_DEFAULT);

	return TRUE;
}

void madc32_loadFromDb(struct s_madc32 * s, uint32_t chnPattern)
{
	int ch;
	int gg;
	uint32_t bit;

	madc32_setAddrReg_db(s);
	madc32_setMcstCblt_db(s);
	madc32_setModuleId_db(s);
	madc32_setDataWidth_db(s);
	madc32_setMultiEvent_db(s);
	madc32_setMarkingType_db(s);
	madc32_setXferData_db(s);
	madc32_setBankOperation_db(s);
	madc32_setAdcResolution_db(s);
	madc32_setOutputFormat_db(s);
	madc32_setAdcOverride_db(s);
	madc32_setSlidingScaleOff_db(s);
	madc32_setSkipOutOfRange_db(s);
	for (gg = 0; gg <= 1; gg++) {
		madc32_setHoldDelay_db(s, gg);
		madc32_setHoldWidth_db(s, gg);
	}
	madc32_useGG_db(s);
	madc32_setInputRange_db(s);
	madc32_setEclTerm_db(s);
	madc32_setEclG1Osc_db(s);
	madc32_setEclFclRts_db(s);
	madc32_setNimG1Osc_db(s);
	madc32_setNimFclRts_db(s);
	madc32_setNimBusy_db(s);
	madc32_setPulserStatus_db(s);
	madc32_setTsSource_db(s);
	madc32_setTsDivisor_db(s);

	bit = 1;
	for (ch = 0; ch < MADC_NOF_CHANNELS; ch++) {
		if (chnPattern & bit) madc32_setThreshold_db(s, ch); else madc32_setThreshold(s, ch, MADC32_D_CHAN_INACTIVE);
		bit <<= 1;
	}
}


bool_t madc32_dumpRegisters(struct s_madc32 * s, char * file)
{
	FILE * f;

	int ch;
	int gg;
	bool_t mcstOrCblt, flag;;

	if (!s->dumpRegsOnInit) return(TRUE);

	f = fopen(file, "w");
	if (f == NULL) {
		sprintf(msg, "[%sdumpRegisters] %s: Error writing file %s", s->mpref, s->moduleName, file);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		return FALSE;
	}

	sprintf(msg, "[%sdumpRegisters] %s: Dumping settings to file %s", s->mpref, s->moduleName, file);
	f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);

	fprintf(f, "Thresholds [0x4000]:\n");
	for (ch = 0; ch < MADC_NOF_CHANNELS; ch++) fprintf(f, "   %2d: %d\n", ch, madc32_getThreshold(s, ch));

	mcstOrCblt = FALSE;
	if (flag = madc32_mcstIsEnabled(s))
					fprintf(f, "MCST signature [0x6024]   : %#x\n", madc32_getMcstSignature(s));
	else				fprintf(f, "MCST                      : disabled\n");
	mcstOrCblt |= flag;
	if (flag = madc32_cbltIsEnabled(s))
					fprintf(f, "CBLT signature [0x6022]   : %#x\n", madc32_getCbltSignature(s));
	else				fprintf(f, "CBLT                      : disabled\n");
	mcstOrCblt |= flag;
	if (mcstOrCblt) {
		if (madc32_isFirstInChain(s)) fprintf(f, "MCST/CBLT chain           : first module in chain\n");
		else if (madc32_isLastInChain(s)) fprintf(f, "MCST/CBLT chain           : last module in chain\n");
		else fprintf(f, "MCST/CBLT chain   : module in the middle\n");
	}
	fprintf(f, "Module ID [0x6004]        : %d\n", madc32_getModuleId(s));
	fprintf(f, "Data width [0x6032]       : %d\n", madc32_getDataWidth(s));
	fprintf(f, "Multi event [0x6036]      : %d\n", madc32_getMultiEvent(s));
	fprintf(f, "Max xfer data wc [0x601A] : %d\n", madc32_getXferData(s));
	fprintf(f, "Marking type [0x6038]     : %d\n", madc32_getMarkingType(s));
	fprintf(f, "Bank operation [0x6040]   : %d\n", madc32_getBankOperation(s));
	fprintf(f, "Adc resolution [0x6042]   : %d\n", madc32_getAdcResolution(s));
	fprintf(f, "Output format [0x6044]    : %d\n", madc32_getOutputFormat(s));
	fprintf(f, "Adc override [0x6046]     : %d\n", madc32_getAdcOverride(s));
	fprintf(f, "Sliding scale off [0x6048]: %d\n", madc32_getSlidingScaleOff(s));
	fprintf(f, "Skip out of range [0x604A]: %d\n", madc32_getSkipOutOfRange(s));
	for (gg = 0; gg <= 1; gg++) {
		fprintf(f, "Hold delay %d [0x605%d]     : %d\n", gg, gg*2, madc32_getHoldDelay(s, gg));
		fprintf(f, "Hold width %d [0x605%d]     : %d\n", gg, gg*2 + 4, madc32_getHoldWidth(s, gg), gg*2 + 4);
	}
	fprintf(f, "Use GG [0x6058]           : %d\n", madc32_getGGUsed(s));
	fprintf(f, "Input range [0x6060]      : %d\n", madc32_getInputRange(s));
	fprintf(f, "Ecl termination [0x6062]  : %#x\n", madc32_getEclTerm(s));
	fprintf(f, "Ecl gate or osc [0x6064]  : %d\n", madc32_getEclG1Osc(s));
	fprintf(f, "Ecl fcl or reset [0x6066] : %d\n", madc32_getEclFclRts(s));
	fprintf(f, "Nim gate or osc [0x606A]  : %d\n", madc32_getNimG1Osc(s));
	fprintf(f, "Nim fcl or reset [0x606C] : %d\n", madc32_getNimFclRts(s));
	fprintf(f, "Nim busy [0x606E]         : %d\n", madc32_getNimBusy(s));
	fprintf(f, "Pulser status [0x6070]    : %d\n", madc32_getPulserStatus(s));
	fprintf(f, "Timestamp source [0x6096] : %#x\n", madc32_getTsSource(s));
	fprintf(f, "Timestamp divisor [0x6098]: %d\n", madc32_getTsDivisor(s));
	fclose(f);
}

bool_t madc32_dumpRaw(struct s_madc32 * s, char * file)
{
	int i;
	FILE * f;

	if (!s->dumpRegsOnInit) return(TRUE);

	f = fopen(file, "w");
	if (f == NULL) {
		sprintf(msg, "[%sdumpRaw] %s: Error writing file %s", s->mpref, s->moduleName, file);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		return FALSE;
	}

	sprintf(msg, "[%sdumpRaw] %s: Dumping raw data to file %s", s->mpref, s->moduleName, file);
	f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);

	for (i = 0x6000; i < 0x60B0; i += 2) {
		fprintf(f, "%#lx %#x\n", i, GET16(s->md->vmeBase, i));
	}
	fclose(f);
}

void madc32_printDb(struct s_madc32 * s)
{
	int ch;
	int gg;

	printf("Thresholds:\n");
	for (ch = 0; ch < MADC_NOF_CHANNELS; ch++) printf("   %2d: %d\n", ch, s->threshold[ch]);

	printf("Addr reg          : %#x\n", s->addrReg);

	if (s->mcstSignature != 0)
					printf("MCST signature    : %#x\n", s->mcstSignature);
	else				printf("MCST              : disabled\n");
	if (s->cbltSignature != 0)
					printf("CBLT signature    : %#x\n", s->cbltSignature);
	else				printf("CBLT              : disabled\n");
	if ((s->mcstSignature != 0) || (s->cbltSignature != 0)) {
		if (s->firstInChain) printf("MCST/CBLT chain   : first module in chain\n");
		else if (s->lastInChain) printf("MCST/CBLT chain   : last module in chain\n");
		else printf("MCST/CBLT chain   : module in the middle\n");
	}
	printf("Module ID         : %d\n", s->moduleId);
	printf("Data width        : %d\n", s->dataWidth);
	printf("Multi event       : %d\n", s->multiEvent);
	printf("Max xfer data wc  : %d\n", s->xferData);
	printf("Marking type      : %d\n", s->markingType);
	printf("Bank operation    : %d\n", s->bankOperation);
	printf("Adc resolution    : %d\n", s->adcResolution);
	printf("Output format     : %d\n", s->outputFormat);
	printf("Adc override      : %d\n", s->adcOverride);
	printf("Sliding scale off : %d\n", s->slidingScaleOff);
	printf("Skip out of range : %d\n", s->skipOutOfRange);
	for (gg = 0; gg <= 1; gg++) {
		printf("Hold delay %d      : %d\n", gg, s->ggHoldDelay[gg]);
		printf("Hold width %d      : %d\n", gg, s->ggHoldWidth[gg]);
	}
	printf("Use GG            : %d\n", s->useGG);
	printf("Input range       : %d\n", s->inputRange);
	printf("Ecl termination   : %#x\n", s->eclTerm);
	printf("Ecl gate or osc   : %d\n", s->eclG1Osc);
	printf("Ecl fcl or reset  : %d\n", s->eclFclRts);
	printf("Nim gate or osc   : %d\n", s->nimG1Osc);
	printf("Nim fcl or reset  : %d\n", s->nimFclRts);
	printf("Nim busy          : %d\n", s->nimBusy);
	printf("Pulser status     : %d\n", s->pulserStatus);
	printf("Timestamp source  : %#x\n", s->ctraTsSource);
	printf("Timestamp divisor : %d\n", s->ctraTsDivisor);
}

int madc32_readout(struct s_madc32 * s, uint32_t * pointer)
{
	static int addrOffset = 0;

	uint32_t * dataStart, *p;
	uint32_t data;
	unsigned int i, j;
	int bmaError;
	int bmaCount;
	int tryIt;
	int numData, nd;

	uint32_t ptrloc;

	int mode;
	int sts;

	dataStart = pointer;
        /*
	tryIt = 20;
	while (tryIt-- && !madc32_dataReady(s)) { usleep(1000); }
        */

	numData = (int) madc32_getFifoLength(s);	
	nd = (int) madc32_getFifoLength(s);
	  
	tryIt = 10;
	while (tryIt-- && (nd != numData)) {
		numData = nd;
		nd = (int) madc32_getFifoLength(s);
	}
	if (numData == 0) return(0);

	if (tryIt <= 0) {
		sprintf(msg, "[%sreadout] %s: Error while reading event data (numData=%d != %d)", s->mpref, s->moduleName, numData, nd);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		return 0;
	}

	if (s->blockXfer) {
		ptrloc = getPhysAddr((char *) pointer, numData * sizeof(uint32_t));
		if (ptrloc == NULL) return(0);
		
		bmaError = bma_read(s->md->bltBase + MADC32_DATA, ptrloc, numData, s->md->bltModeId);
		if (bmaError != 0) {
			if (bmaError < 0) {
				sprintf(msg, "[%sreadout] %s: Error %d while reading event data (numData=%d)", s->mpref, s->moduleName, bmaError, numData);
			} else {
				sprintf(msg, "[%sreadout] %s: Error \"%s\" (%d) while reading event data (numData=%d)", s->mpref, s->moduleName, bmaErrlist[bmaError], bmaError, numData);
			}
			f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
			return(0);
		}
			
		pointer += numData;
	} else {
		for (i = 0; i < numData; i++) *pointer++ = GET32(s->md->vmeBase, MADC32_DATA);
	}

	if (s->repairRawData) pointer = madc32_repairRawData(s, pointer, dataStart);
	
	madc32_resetReadout(s);

	return (pointer - dataStart);
}

int madc32_readTimeB(struct s_madc32 * s, uint32_t * pointer)
{
	uint16_t * p16 = pointer;
	*p16++ = 0;
	*p16++ = GET16(s->md->vmeBase, MADC32_CTRB_TIME_0);
	*p16++ = GET16(s->md->vmeBase, MADC32_CTRB_TIME_1);
	*p16++ = GET16(s->md->vmeBase, MADC32_CTRB_TIME_2);
	return 4 * sizeof(uint16_t) / sizeof(uint32_t);
}

bool_t madc32_dataReady(struct s_madc32 * s)
{
	return TSTB16(s->md->vmeBase, MADC32_DATA_READY, 0) ;
}

void madc32_resetReadout(struct s_madc32 * s)
{
	SET16(s->md->vmeBase, MADC32_READOUT_RESET, 0x1);
}

void madc32_resetTimestamp(struct s_madc32 * s)
{
	SET16(s->md->vmeBase, MADC32_CTRA_RESET_A_OR_B, 0x3);
}

bool_t madc32_testBusError(struct s_madc32 * s)
{
	return TSTB16(s->md->vmeBase, MADC32_MULTI_EVENT, MADC32_MULTI_EVENT_BERR) == 1 ? FALSE : TRUE;
}

void madc32_enableBusError(struct s_madc32 * s)
{
	CLRB16(s->md->vmeBase, MADC32_MULTI_EVENT, MADC32_MULTI_EVENT_BERR);
}

void madc32_disableBusError(struct s_madc32 * s)
{
	SETB16(s->md->vmeBase, MADC32_MULTI_EVENT, MADC32_MULTI_EVENT_BERR);
}

void madc32_startAcq(struct s_madc32 * s)
{
	SET16(s->md->vmeBase, MADC32_START_ACQUISITION, 0x0);
	madc32_resetFifo(s);
	madc32_resetReadout(s);
	madc32_resetTimestamp(s);
 	SET16(s->md->vmeBase, MADC32_START_ACQUISITION, 0x1);
}

void madc32_stopAcq(struct s_madc32 * s)
{
	SET16(s->md->vmeBase, MADC32_START_ACQUISITION, 0x0);
	madc32_resetFifo(s);
	madc32_resetReadout(s);
	madc32_resetTimestamp(s);
}

void madc32_resetFifo(struct s_madc32 * s)
{
	SET16(s->md->vmeBase, MADC32_FIFO_RESET, 0x1);
}

void madc32_setMcstCblt_db(struct s_madc32 * s) {
	madc32_setMcstAddr(s, s->mcstSignature);
	madc32_setCbltAddr(s, s->cbltSignature);
	if (s->firstInChain) madc32_setFirstInChain(s);
	else if (s->lastInChain) madc32_setLastInChain(s);
	else madc32_setMiddleOfChain(s);
}

void madc32_setMcstAddr(struct s_madc32 * s, unsigned long Signature) {
	SET16(s->md->vmeBase, MADC32_MCST_ADDRESS, Signature);
	if (Signature != 0) {
		s->mcstAddr = mapAdditionalVME(s->md, (Signature & 0xFF) << 24, 0);
		if (s->mcstAddr) madc32_setMcstEnable(s); else madc32_setMcstDisable(s);
	} else {
		madc32_setMcstDisable(s);
	}
}

uint16_t madc32_getMcstSignature(struct s_madc32 * s) {
	uint16_t addr8;
	addr8 = GET16(s->md->vmeBase, MADC32_MCST_ADDRESS);
	return addr8;
}

void madc32_setMcstEnable(struct s_madc32 * s) {
	SET16(s->md->vmeBase, MADC32_CBLT_MCST_CONTROL, MADC32_MCST_ENA);
}

void madc32_setMcstDisable(struct s_madc32 * s) {
	SET16(s->md->vmeBase, MADC32_CBLT_MCST_CONTROL, MADC32_MCST_DIS);
}

bool_t madc32_mcstIsEnabled(struct s_madc32 * s) {
	uint16_t ctrl;
	ctrl = GET16(s->md->vmeBase, MADC32_CBLT_MCST_CONTROL);
	return ((ctrl & MADC32_MCST_DIS) != 0);
}

void madc32_setCbltAddr(struct s_madc32 * s, unsigned long Signature) {
	SET16(s->md->vmeBase, MADC32_CBLT_ADDRESS, Signature);
	if (Signature != 0) {
		s->cbltAddr = mapAdditionalVME(s->md, (Signature & 0xFF) << 24, 0);
		if (s->cbltAddr) madc32_setCbltEnable(s); else madc32_setCbltDisable(s);
	} else {
		madc32_setCbltDisable(s);
	}
}

uint16_t madc32_getCbltSignature(struct s_madc32 * s) {
	uint16_t addr8;
	addr8 = GET16(s->md->vmeBase, MADC32_CBLT_ADDRESS);
	return addr8;
}

void madc32_setCbltEnable(struct s_madc32 * s) {
	SET16(s->md->vmeBase, MADC32_CBLT_MCST_CONTROL, MADC32_CBLT_ENA);
}

void madc32_setCbltDisable(struct s_madc32 * s) {
	SET16(s->md->vmeBase, MADC32_CBLT_MCST_CONTROL, MADC32_CBLT_DIS);
}

bool_t madc32_cbltIsEnabled(struct s_madc32 * s) {
	uint16_t ctrl;
	ctrl = GET16(s->md->vmeBase, MADC32_CBLT_MCST_CONTROL);
	return ((ctrl & MADC32_CBLT_DIS) != 0);
}

void madc32_setFirstInChain(struct s_madc32 * s) {
	SET16(s->md->vmeBase, MADC32_CBLT_MCST_CONTROL, MADC32_CBLT_FIRST_ENA);
	SET16(s->md->vmeBase, MADC32_CBLT_MCST_CONTROL, MADC32_CBLT_LAST_DIS);
}

bool_t madc32_isFirstInChain(struct s_madc32 * s) {
	uint16_t ctrl;
	ctrl = GET16(s->md->vmeBase, MADC32_CBLT_MCST_CONTROL);
	return ((ctrl & MADC32_CBLT_FIRST_DIS) != 0);
}

void madc32_setLastInChain(struct s_madc32 * s) {
	SET16(s->md->vmeBase, MADC32_CBLT_MCST_CONTROL, MADC32_CBLT_LAST_ENA);
	SET16(s->md->vmeBase, MADC32_CBLT_MCST_CONTROL, MADC32_CBLT_FIRST_DIS);
}

bool_t madc32_isLastInChain(struct s_madc32 * s) {
	uint16_t ctrl;
	ctrl = GET16(s->md->vmeBase, MADC32_CBLT_MCST_CONTROL);
	return ((ctrl & MADC32_CBLT_LAST_DIS) != 0);
}

void madc32_setMiddleOfChain(struct s_madc32 * s) {
	SET16(s->md->vmeBase, MADC32_CBLT_MCST_CONTROL, MADC32_CBLT_FIRST_DIS);
	SET16(s->md->vmeBase, MADC32_CBLT_MCST_CONTROL, MADC32_CBLT_LAST_DIS);
}

bool_t madc32_isMiddleOfChain(struct s_madc32 * s) {
	bool_t first, last;
	first = madc32_isFirstInChain(s);
	last = madc32_isLastInChain(s);
	return (!first && !last);
}

void madc32_startAcq_mcst(struct s_madc32 * s)
{
	SET16(s->mcstAddr, MADC32_START_ACQUISITION, 0x0);
	madc32_resetFifo_mcst(s);
	madc32_resetReadout_mcst(s);
	madc32_resetTimestamp_mcst(s);
	SET16(s->mcstAddr, MADC32_START_ACQUISITION, 0x1);
}

void madc32_stopAcq_mcst(struct s_madc32 * s)
{
	SET16(s->mcstAddr, MADC32_START_ACQUISITION, 0x0);
	madc32_resetFifo_mcst(s);
	madc32_resetReadout_mcst(s);
	madc32_resetTimestamp_mcst(s);
}

void madc32_resetFifo_mcst(struct s_madc32 * s)
{
	SET16(s->mcstAddr, MADC32_FIFO_RESET, 0x1);
}

void madc32_resetReadout_mcst(struct s_madc32 * s)
{
	SET16(s->mcstAddr, MADC32_READOUT_RESET, 0x1);
}

void madc32_resetTimestamp_mcst(struct s_madc32 * s)
{
	SET16(s->mcstAddr, MADC32_CTRA_RESET_A_OR_B, 0x3);
}

uint32_t * madc32_repairRawData(struct s_madc32 * s, uint32_t * pointer, uint32_t * dataStart) {
	return pointer;
}

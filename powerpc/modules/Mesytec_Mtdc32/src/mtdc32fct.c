/*_______________________________________________________[C LIBRARY FUNCTIONS]
//////////////////////////////////////////////////////////////////////////////
//! \file			mtdc32fct.c
//! \brief			Code for module MTDC32
//! \details		Implements functions to handle modules of type Mesytec MTDC32
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
#include <sigcodes.h>

#include "gd_readout.h"

#include "mtdc32.h"
#include "mtdc32_database.h"
#include "mtdc32_protos.h"

#include "root_env.h"
#include "mapping_database.h"
#include "bmaErrlist.h"

#include "err_mask_def.h"
#include "errnum_def.h"

void catchBerr();

int numData;
int rdoWc;

int evtWc;
int lastData;

char msg[256];

struct s_mtdc32 * be;


struct s_mtdc32 * mtdc32_alloc(char * moduleName, struct s_mapDescr * md, int serial)
{
	struct s_mtdc32 * s;
	int firmware, mainRev;

	s = (struct s_mtdc32 *) calloc(1, sizeof(struct s_mtdc32));
	if (s != NULL) {
		s->md = md;
		strcpy(s->moduleName, moduleName);
		strcpy(s->prefix, "m_read_meb");
		strcpy(s->mpref, "mtdc32: ");
		s->serial = serial;
		s->verbose = FALSE;
		s->dumpRegsOnInit = FALSE;

		s->mcstSignature = 0x0;
		s->cbltSignature = 0x0;
		s->firstInChain = FALSE;
		s->lastInChain = FALSE;

		firmware = GET16(s->md->vmeBase, MTDC32_FIRMWARE_REV);
		mainRev = (firmware >> 8) & 0xff;
		s->memorySize = (mainRev >= 2) ? (8*1024 + 2) : (1024 + 2);
	} else {
		sprintf(msg, "[%salloc] %s: Can't allocate mtdc32 struct", s->mpref, s->moduleName);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
	}
	return s;
}

void mtdc32_initialize(struct s_mtdc32 * s)
{
	be = s;
	signal(SIGBUS, catchBerr);
	mtdc32_disableBusError(s);
	mtdc32_resetReadout(s);
	sprintf(msg, "[%sinitialize] %s: Block xfer is %s", s->mpref, s->moduleName, s->blockXfer ? "ON" : "OFF");
	f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
	if (s->repairRawData) {
		sprintf(msg, "[%sinitialize] %s: Raw data will be repaired", s->mpref, s->moduleName);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
	}
}

bool_t mtdc32_useBLT(struct s_mtdc32 * s) {
	return s->blockXfer;
}

void mtdc32_soft_reset(struct s_mtdc32 * s)
{
  SET16(s->md->vmeBase, MTDC32_SOFT_RESET, 0x1);
}

uint16_t mtdc32_getThreshold(struct s_mtdc32 * s, uint16_t channel)
{
	uint16_t thresh = 0;
	if (channel < MTDC_NOF_CHANNELS) thresh = GET16(s->md->vmeBase, MTDC32_THRESHOLD + sizeof(uint16_t) * channel) & MTDC32_THRESHOLD_MASK;
	return thresh;
}

void mtdc32_setThreshold_db(struct s_mtdc32 * s, uint16_t channel)
{
	if (channel < MTDC_NOF_CHANNELS) mtdc32_setThreshold(s, channel, s->threshold[channel]);
}

void mtdc32_setThreshold(struct s_mtdc32 * s, uint16_t channel, uint16_t thresh)
{
	if (channel < MTDC_NOF_CHANNELS) SET16(s->md->vmeBase, MTDC32_THRESHOLD + sizeof(uint16_t) * channel, thresh & MTDC32_THRESHOLD_MASK);
}

void mtdc32_setAddrReg_db(struct s_mtdc32 * s) { mtdc32_setAddrReg(s, s->addrReg); }

void mtdc32_setAddrReg(struct s_mtdc32 * s, uint16_t vmeAddr)
{
	if (vmeAddr) {
		SET16(s->md->vmeBase, MTDC32_ADDR_REG, vmeAddr);
		SET16(s->md->vmeBase, MTDC32_ADDR_SOURCE, MTDC32_ADDR_SOURCE_REG);
		s->md->vmeBase = vmeAddr;
	}
}

uint16_t mtdc32_getAddrReg(struct s_mtdc32 * s)
{
	uint16_t source = GET16(s->md->vmeBase, MTDC32_ADDR_SOURCE);
	if (source & MTDC32_ADDR_SOURCE_REG) return GET16(s->md->vmeBase, MTDC32_ADDR_REG);
	else return 0;
}

void mtdc32_setModuleId_db(struct s_mtdc32 * s) { mtdc32_setModuleId(s, s->moduleId); }

void mtdc32_setModuleId(struct s_mtdc32 * s, uint16_t id)
{
	SET16(s->md->vmeBase, MTDC32_MODULE_ID, id & MTDC32_MODULE_ID_MASK);
}

uint16_t mtdc32_getModuleId(struct s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_MODULE_ID) & MTDC32_MODULE_ID_MASK;
}

uint16_t mtdc32_getFifoLength(struct s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_BUFFER_DATA_LENGTH) & MTDC32_BUFFER_DATA_LENGTH_MASK;
}

void mtdc32_setDataWidth_db(struct s_mtdc32 * s) { mtdc32_setDataWidth(s, s->dataWidth); }

void mtdc32_setDataWidth(struct s_mtdc32 * s, uint16_t width)
{
	SET16(s->md->vmeBase, MTDC32_DATA_LENGTH_FORMAT, width & MTDC32_DATA_LENGTH_FORMAT_MASK);
}

uint16_t mtdc32_getDataWidth(struct s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_DATA_LENGTH_FORMAT) & MTDC32_DATA_LENGTH_FORMAT_MASK;
}

void mtdc32_setMultiEvent_db(struct s_mtdc32 * s) { mtdc32_setMultiEvent(s, s->multiEvent); }

void mtdc32_setMultiEvent(struct s_mtdc32 * s, uint16_t mode)
{
	SET16(s->md->vmeBase, MTDC32_MULTI_EVENT, mode & MTDC32_MULTI_EVENT_MASK);
}

uint16_t mtdc32_getMultiEvent(struct s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_MULTI_EVENT) & MTDC32_MULTI_EVENT_MASK;
}

void mtdc32_setXferData_db(struct s_mtdc32 * s) { mtdc32_setXferData(s, s->xferData); }

void mtdc32_setXferData(struct s_mtdc32 * s, uint16_t wc)
{
	SET16(s->md->vmeBase, MTDC32_MAX_XFER_DATA, wc & MTDC32_MAX_XFER_DATA_MASK);
}

uint16_t mtdc32_getXferData(struct s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_MAX_XFER_DATA) & MTDC32_MAX_XFER_DATA_MASK;
}

void mtdc32_setMarkingType_db(struct s_mtdc32 * s) { mtdc32_setMarkingType(s, s->markingType); }

void mtdc32_setMarkingType(struct s_mtdc32 * s, uint16_t type)
{
	SET16(s->md->vmeBase, MTDC32_MARKING_TYPE, type & MTDC32_MARKING_TYPE_MASK);
}

uint16_t mtdc32_getMarkingType(struct s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_MARKING_TYPE) & MTDC32_MARKING_TYPE_MASK;
}

void mtdc32_setBankOperation_db(struct s_mtdc32 * s) { mtdc32_setBankOperation(s, s->bankOperation); }

void mtdc32_setBankOperation(struct s_mtdc32 * s, uint16_t oper)
{
	SET16(s->md->vmeBase, MTDC32_BANK_OPERATION, oper & MTDC32_BANK_OPERATION_MASK);
}

uint16_t mtdc32_getBankOperation(struct s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_BANK_OPERATION) & MTDC32_BANK_OPERATION_MASK;
}

void mtdc32_setAdcResolution_db(struct s_mtdc32 * s) { mtdc32_setAdcResolution(s, s->adcResolution); }

void mtdc32_setAdcResolution(struct s_mtdc32 * s, uint16_t res)
{
	SET16(s->md->vmeBase, MTDC32_ADC_RESOLUTION, res & MTDC32_ADC_RESOLUTION_MASK);
}

uint16_t mtdc32_getAdcResolution(struct s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_ADC_RESOLUTION) & MTDC32_ADC_RESOLUTION_MASK;
}

void mtdc32_setOutputFormat_db(struct s_mtdc32 * s) { mtdc32_setOutputFormat(s, s->outputFormat); }

void mtdc32_setOutputFormat(struct s_mtdc32 * s, uint16_t format)
{
	SET16(s->md->vmeBase, MTDC32_OUTPUT_FORMAT, format & MTDC32_OUTPUT_FORMAT_MASK);
}

uint16_t mtdc32_getOutputFormat(struct s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_OUTPUT_FORMAT) & MTDC32_OUTPUT_FORMAT_MASK;
}

void mtdc32_setAdcOverride_db(struct s_mtdc32 * s) { mtdc32_setAdcOverride(s, s->adcOverride); }

void mtdc32_setAdcOverride(struct s_mtdc32 * s, uint16_t over)
{
	SET16(s->md->vmeBase, MTDC32_ADC_OVERRIDE, over & MTDC32_ADC_RESOLUTION_MASK);
}

uint16_t mtdc32_getAdcOverride(struct s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_ADC_OVERRIDE) & MTDC32_ADC_RESOLUTION_MASK;
}

void mtdc32_setSlidingScaleOff_db(struct s_mtdc32 * s) { mtdc32_setSlidingScaleOff(s, s->slidingScaleOff); }

void mtdc32_setSlidingScaleOff(struct s_mtdc32 * s, bool_t flag)
{
	uint16_t f = flag ? 1 : 0;
	SET16(s->md->vmeBase, MTDC32_SLIDING_SCALE_OFF, f);
}

bool_t mtdc32_getSlidingScaleOff(struct s_mtdc32 * s)
{
	uint16_t f = GET16(s->md->vmeBase, MTDC32_SLIDING_SCALE_OFF) & MTDC32_SLIDING_SCALE_OFF_MASK;
	return (f != 0) ? TRUE : FALSE;
}

void mtdc32_setSkipOutOfRange_db(struct s_mtdc32 * s) { mtdc32_setSkipOutOfRange(s, s->skipOutOfRange); }

void mtdc32_setSkipOutOfRange(struct s_mtdc32 * s, bool_t flag)
{
	uint16_t f = flag ? 1 : 0;
	SET16(s->md->vmeBase, MTDC32_SKIP_OUT_OF_RANGE, f);
}

bool_t mtdc32_getSkipOutOfRange(struct s_mtdc32 * s)
{
	uint16_t f = GET16(s->md->vmeBase, MTDC32_SKIP_OUT_OF_RANGE) & MTDC32_SKIP_OUT_OF_RANGE_MASK;
	return (f != 0) ? TRUE : FALSE;
}

void mtdc32_setIgnoreThresholds_db(struct s_mtdc32 * s) { mtdc32_setIgnoreThresholds(s, s->ignoreThresh); }

void mtdc32_setIgnoreThresholds(struct s_mtdc32 * s, bool_t flag)
{
	uint16_t f = flag ? 1 : 0;
	SET16(s->md->vmeBase, MTDC32_IGNORE_THRESHOLDS, f);
}

bool_t mtdc32_getIgnoreThresholds(struct s_mtdc32 * s)
{
	uint16_t f = GET16(s->md->vmeBase, MTDC32_IGNORE_THRESHOLDS) & MTDC32_IGNORE_THRESH_MASK;
	return (f != 0) ? TRUE : FALSE;
}

void mtdc32_setHoldDelay_db(struct s_mtdc32 * s, uint16_t gg) { mtdc32_setHoldDelay(s, gg, s->ggHoldDelay[gg]); }

void mtdc32_setHoldDelay(struct s_mtdc32 * s, uint16_t gg, uint16_t delay)
{
	int offs;
	switch (gg) {
		case 0: offs = MTDC32_GG_HOLD_DELAY_0; break;
		case 1: offs = MTDC32_GG_HOLD_DELAY_1; break;
		default: return;
	}
	SET16(s->md->vmeBase, offs, delay & MTDC32_GG_HOLD_DELAY_MASK);
}

uint16_t mtdc32_getHoldDelay(struct s_mtdc32 * s, uint16_t gg)
{
	int offs;
	switch (gg) {
		case 0: offs = MTDC32_GG_HOLD_DELAY_0; break;
		case 1: offs = MTDC32_GG_HOLD_DELAY_1; break;
		default: return 0;
	}
	return GET16(s->md->vmeBase, offs) & MTDC32_GG_HOLD_DELAY_MASK;
}

void mtdc32_setHoldWidth_db(struct s_mtdc32 * s, uint16_t gg) { mtdc32_setHoldWidth(s, gg, s->ggHoldWidth[gg]); }

void mtdc32_setHoldWidth(struct s_mtdc32 * s, uint16_t gg, uint16_t width)
{
	int offs;
	switch (gg) {
		case 0: offs = MTDC32_GG_HOLD_WIDTH_0; break;
		case 1: offs = MTDC32_GG_HOLD_WIDTH_1; break;
		default: return;
	}
	SET16(s->md->vmeBase, offs, width & MTDC32_GG_HOLD_WIDTH_MASK);
}

uint16_t mtdc32_getHoldWidth(struct s_mtdc32 * s, uint16_t gg)
{
	int offs;
	switch (gg) {
		case 0: offs = MTDC32_GG_HOLD_WIDTH_0; break;
		case 1: offs = MTDC32_GG_HOLD_WIDTH_1; break;
		default: return 0;
	}
	return GET16(s->md->vmeBase, offs) & MTDC32_GG_HOLD_WIDTH_MASK;
}

void mtdc32_useGG_db(struct s_mtdc32 * s) { mtdc32_useGG(s, s->useGG); }

void mtdc32_useGG(struct s_mtdc32 * s, uint16_t gg)
{
	SET16(s->md->vmeBase, MTDC32_USE_GATE_GENERATOR, gg & MTDC32_USE_GATE_GENERATOR_MASK);
}

uint16_t mtdc32_getGGUsed(struct s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_USE_GATE_GENERATOR) & MTDC32_USE_GATE_GENERATOR_MASK;
}

void mtdc32_setInputRange_db(struct s_mtdc32 * s) { mtdc32_setInputRange(s, s->inputRange); }

void mtdc32_setInputRange(struct s_mtdc32 * s, uint16_t range)
{
	SET16(s->md->vmeBase, MTDC32_INPUT_RANGE, range & MTDC32_INPUT_RANGE_MASK);
}

uint16_t mtdc32_getInputRange(struct s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_INPUT_RANGE) & MTDC32_INPUT_RANGE_MASK;
}

void mtdc32_setEclTerm_db(struct s_mtdc32 * s) { mtdc32_setEclTerm(s, s->eclTerm); }

void mtdc32_setEclTerm(struct s_mtdc32 * s, uint16_t term)
{
	SET16(s->md->vmeBase, MTDC32_ECL_TERMINATORS, term & MTDC32_ECL_TERMINATORS_MASK);
}

uint16_t mtdc32_getEclTerm(struct s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_ECL_TERMINATORS) & MTDC32_ECL_TERMINATORS_MASK;
}

void mtdc32_setEclG1OrOsc_db(struct s_mtdc32 * s) { mtdc32_setEclG1OrOsc(s, s->eclG1OrOsc); }

void mtdc32_setEclG1OrOsc(struct s_mtdc32 * s, uint16_t go)
{
	SET16(s->md->vmeBase, MTDC32_ECL_G1_OR_OSC, go & MTDC32_ECL_G1_OR_OSC_MASK);
}

uint16_t mtdc32_getEclG1OrOsc(struct s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_ECL_G1_OR_OSC) & MTDC32_ECL_G1_OR_OSC_MASK;
}

void mtdc32_setEclFclOrRts_db(struct s_mtdc32 * s) { mtdc32_setEclFclOrRts(s, s->eclFclOrRts); }

void mtdc32_setEclFclOrRts(struct s_mtdc32 * s, uint16_t fr)
{
	SET16(s->md->vmeBase, MTDC32_ECL_FCL_OR_RES_TS, fr & MTDC32_ECL_FCL_OR_RES_TS_MASK);
}

uint16_t mtdc32_getEclFclOrRts(struct s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_ECL_FCL_OR_RES_TS) & MTDC32_ECL_FCL_OR_RES_TS_MASK;
}

void mtdc32_setNimG1OrOsc_db(struct s_mtdc32 * s) { mtdc32_setNimG1OrOsc(s, s->nimG1OrOsc); }

void mtdc32_setNimG1OrOsc(struct s_mtdc32 * s, uint16_t go)
{
	SET16(s->md->vmeBase, MTDC32_NIM_G1_OR_OSC, go & MTDC32_NIM_G1_OR_OSC_MASK);
}

uint16_t mtdc32_getNimG1OrOsc(struct s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_NIM_G1_OR_OSC) & MTDC32_NIM_G1_OR_OSC_MASK;
}


void mtdc32_setNimFclOrRts_db(struct s_mtdc32 * s) { mtdc32_setNimFclOrRts(s, s->nimFclOrRts); }

void mtdc32_setNimFclOrRts(struct s_mtdc32 * s, uint16_t fr)
{
	SET16(s->md->vmeBase, MTDC32_NIM_FCL_OR_RES_TS, fr & MTDC32_NIM_FCL_OR_RES_TS_MASK);
}

uint16_t mtdc32_getNimFclOrRts(struct s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_NIM_FCL_OR_RES_TS) & MTDC32_NIM_FCL_OR_RES_TS_MASK;
}

void mtdc32_setNimBusy_db(struct s_mtdc32 * s) { mtdc32_setNimBusy(s, s->nimBusy); }

void mtdc32_setNimBusy(struct s_mtdc32 * s, uint16_t busy)
{
	SET16(s->md->vmeBase, MTDC32_NIM_BUSY, busy & MTDC32_NIM_BUSY_MASK);
}

uint16_t mtdc32_getNimBusy(struct s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_NIM_BUSY) & MTDC32_NIM_BUSY_MASK;
}

void mtdc32_setBufferThresh_db(struct s_mtdc32 * s) { mtdc32_setBufferThresh(s, s->bufferThresh); }

void mtdc32_setBufferThresh(struct s_mtdc32 * s, uint16_t bufferThresh)
{
	SET16(s->md->vmeBase, MTDC32_IRQ_THRESH, bufferThresh);
}

uint16_t mtdc32_getBufferThresh(struct s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_IRQ_THRESH);
}

void mtdc32_setTestPulser_db(struct s_mtdc32 * s) { mtdc32_setTestPulser(s, s->testPulserStatus); }

void mtdc32_setTestPulser(struct s_mtdc32 * s, uint16_t mode)
{
	SET16(s->md->vmeBase, MTDC32_TEST_PULSER_STATUS, mode & MTDC32_TEST_PULSER_STATUS_MASK);
}

uint16_t mtdc32_getTestPulser(struct s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_TEST_PULSER_STATUS) & MTDC32_TEST_PULSER_STATUS_MASK;
}

void mtdc32_setTsSource_db(struct s_mtdc32 * s) { mtdc32_setTsSource(s, s->ctraTsSource); }

void mtdc32_setTsSource(struct s_mtdc32 * s, uint16_t source)
{
	SET16(s->md->vmeBase, MTDC32_CTRA_TS_SOURCE, source & MTDC32_CTRA_TS_SOURCE_MASK);
}

uint16_t mtdc32_getTsSource(struct s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_CTRA_TS_SOURCE) & MTDC32_CTRA_TS_SOURCE_MASK;
}

void mtdc32_setTsDivisor_db(struct s_mtdc32 * s) { mtdc32_setTsDivisor(s, s->ctraTsDivisor); }

void mtdc32_setTsDivisor(struct s_mtdc32 * s, uint16_t div)
{
	SET16(s->md->vmeBase, MTDC32_CTRA_TS_DIVISOR, div & MTDC32_CTRA_TS_DIVISOR_MASK);
}

uint16_t mtdc32_getTsDivisor(struct s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_CTRA_TS_DIVISOR) & MTDC32_CTRA_TS_DIVISOR_MASK;
}

void mtdc32_moduleInfo(struct s_mtdc32 * s)
{
	int firmware, mainRev, subRev;
	firmware = GET16(s->md->vmeBase, MTDC32_FIRMWARE_REV);
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

void mtdc32_setPrefix(struct s_mtdc32 * s, char * prefix)
{
	strcpy(s->prefix, prefix);
	strcpy(s->mpref, "");
}

bool_t mtdc32_fillStruct(struct s_mtdc32 * s, char * file)
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

	s->verbose = root_env_getval_b("MTDC32.VerboseMode", FALSE);

	s->dumpRegsOnInit = root_env_getval_b("MTDC32.DumpRegisters", FALSE);

	s->updSettings = root_env_getval_b("MTDC32.UpdateSettings", FALSE);
	s->updInterval = root_env_getval_i("MTDC32.UpdateInterval", 0);
	s->updCountDown = 0;

	sp = root_env_getval_s("MTDC32.ModuleName", "");
	if (strcmp(s->moduleName, "mtdc32") != 0 && strcmp(sp, s->moduleName) != 0) {
		sprintf(msg, "[%sfill_struct] %s: File %s contains wrong module name - %s", s->mpref, s->moduleName, file, sp);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		return FALSE;
	}
	strcpy(mnUC, sp);
	mnUC[0] = toupper(mnUC[0]);

	sprintf(res, "MTDC32.%s.BlockXfer", mnUC);
	s->blockXfer = root_env_getval_b(res, FALSE);

	sprintf(res, "MTDC32.%s.RepairRawData", mnUC);
	s->repairRawData = root_env_getval_b(res, FALSE);

	for (i = 0; i < MTDC_NOF_CHANNELS; i++) {
		sprintf(res, "MTDC32.%s.Thresh.%d", mnUC, i);
		s->threshold[i] = root_env_getval_i(res, MTDC32_THRESHOLD_DEFAULT);
	}

	sprintf(res, "MTDC32.%s.AddrSource", mnUC);
	s->addrSource = root_env_getval_i(res, MTDC32_ADDR_SOURCE_DEFAULT);

	if (s->addrSource == MTDC32_ADDR_SOURCE_REG) {
		sprintf(res, "MTDC32.%s.AddrReg", mnUC);
		s->addrReg = root_env_getval_i(res, 0);
		if (s->addrReg == 0) {
			sprintf(msg, "[%sfill_struct] %s: vme addr missing (addr source = reg)", s->mpref, s->moduleName);
			f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		}
	}

	sprintf(res, "MTDC32.%s.MCSTSignature", mnUC);
	s->mcstSignature = root_env_getval_x(res, 0x0);
	sprintf(res, "MTDC32.%s.CBLTSignature", mnUC);
	s->cbltSignature = root_env_getval_x(res, 0x0);
	sprintf(res, "MTDC32.%s.FirstInChain", mnUC);
	s->firstInChain = root_env_getval_b(res, FALSE);
	sprintf(res, "MTDC32.%s.LastInChain", mnUC);
	s->lastInChain = root_env_getval_b(res, FALSE);

	sprintf(res, "MTDC32.%s.ModuleId", mnUC);
	s->moduleId = root_env_getval_i(res, MTDC32_MODULE_ID_DEFAULT);

	sprintf(res, "MTDC32.%s.FifoLength", mnUC);
	s->fifoLength = root_env_getval_i(res, 0);

#if 0
	if (s->fifoLength == 0) {
		sprintf(msg, "[%sfill_struct] %s: fifo length missing", s->mpref, s->moduleName);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
	}
#endif

	sprintf(res, "MTDC32.%s.DataWidth", mnUC);
	s->dataWidth = root_env_getval_i(res, MTDC32_DATA_LENGTH_FORMAT_DEFAULT);

	sprintf(res, "MTDC32.%s.MultiEvent", mnUC);
	s->multiEvent = root_env_getval_i(res, MTDC32_MULTI_EVENT_DEFAULT);

	sprintf(res, "MTDC32.%s.XferData", mnUC);
	s->xferData = root_env_getval_i(res, MTDC32_MAX_XFER_DATA_DEFAULT);

	sprintf(res, "MTDC32.%s.MarkingType", mnUC);
	s->markingType = root_env_getval_i(res, MTDC32_MARKING_TYPE_DEFAULT);

	sprintf(res, "MTDC32.%s.BankOperation", mnUC);
	s->bankOperation = root_env_getval_i(res, MTDC32_BANK_OPERATION_DEFAULT);

	sprintf(res, "MTDC32.%s.AdcResolution", mnUC);
	s->adcResolution = root_env_getval_i(res, MTDC32_ADC_RESOLUTION_DEFAULT);

	sprintf(res, "MTDC32.%s.OutputFormat", mnUC);
	s->outputFormat = root_env_getval_i(res, MTDC32_OUTPUT_FORMAT_DEFAULT);

	sprintf(res, "MTDC32.%s.AdcOverride", mnUC);
	s->adcOverride = root_env_getval_i(res, MTDC32_ADC_OVERWRITE_DEFAULT);
	if (s->adcOverride == MTDC32_ADC_OVERWRITE_DEFAULT) s->adcOverride = s->adcResolution;

	sprintf(res, "MTDC32.%s.SlidingScaleOff", mnUC);
	s->slidingScaleOff = root_env_getval_b(res, FALSE);

	sprintf(res, "MTDC32.%s.SkipOutOfRange", mnUC);
	s->skipOutOfRange = root_env_getval_b(res, FALSE);

	for (i = 0; i <= 1; i++) {
		sprintf(res, "MTDC32.%s.HoldDelay.%d", mnUC, i);
		s->ggHoldDelay[i] = root_env_getval_i(res, MTDC32_GG_HOLD_DELAY_DEFAULT);
	}

	for (i = 0; i <= 1; i++) {
		sprintf(res, "MTDC32.%s.HoldWidth.%d", mnUC, i);
		s->ggHoldWidth[i] = root_env_getval_i(res, MTDC32_GG_HOLD_WIDTH_DEFAULT);
	}

	sprintf(res, "MTDC32.%s.UseGG", mnUC);
	s->useGG = root_env_getval_i(res, MTDC32_USE_GATE_GENERATOR_DEFAULT);

	sprintf(res, "MTDC32.%s.InputRange", mnUC);
	s->inputRange = root_env_getval_i(res, MTDC32_INPUT_RANGE_DEFAULT);

	sprintf(res, "MTDC32.%s.EclTerm", mnUC);
	s->eclTerm = root_env_getval_i(res, MTDC32_ECL_TERMINATORS_DEFAULT);

	sprintf(res, "MTDC32.%s.EclG1OrOsc", mnUC);
	s->eclG1OrOsc = root_env_getval_i(res, MTDC32_ECL_G1_OR_OSC_DEFAULT);

	sprintf(res, "MTDC32.%s.EclFclOrRts", mnUC);
	s->eclFclOrRts = root_env_getval_i(res, MTDC32_NIM_FCL_OR_RES_TS_DEFAULT);

	sprintf(res, "MTDC32.%s.NimG1OrOsc", mnUC);
	s->nimG1OrOsc = root_env_getval_i(res, MTDC32_NIM_G1_OR_OSC_DEFAULT);

	sprintf(res, "MTDC32.%s.NimFclOrRts", mnUC);
	s->nimFclOrRts = root_env_getval_i(res, MTDC32_NIM_FCL_OR_RES_TS_DEFAULT);

	sprintf(res, "MTDC32.%s.NimBusy", mnUC);
	s->nimBusy = root_env_getval_i(res, MTDC32_NIM_BUSY_DEFAULT);

	sprintf(res, "MTDC32.%s.PulserStatus", mnUC);
	s->testPulserStatus = root_env_getval_i(res, MTDC32_TEST_PULSER_STATUS_DEFAULT);

	sprintf(res, "MTDC32.%s.TsSource", mnUC);
	s->ctraTsSource = root_env_getval_i(res, MTDC32_CTRA_TS_SOURCE_DEFAULT);

	sprintf(res, "MTDC32.%s.TsDivisor", mnUC);
	s->ctraTsDivisor = root_env_getval_i(res, MTDC32_CTRA_TS_DIVISOR_DEFAULT);

	return TRUE;
}

void mtdc32_loadFromDb(struct s_mtdc32 * s, uint32_t chnPattern)
{
	int ch;
	int gg;
	uint32_t bit;

	mtdc32_setAddrReg_db(s);
	mtdc32_setMcstCblt_db(s);
	mtdc32_setModuleId_db(s);
	mtdc32_setDataWidth_db(s);
	mtdc32_setMultiEvent_db(s);
	mtdc32_setMarkingType_db(s);
	mtdc32_setXferData_db(s);
	mtdc32_setBankOperation_db(s);
	mtdc32_setAdcResolution_db(s);
	mtdc32_setOutputFormat_db(s);
	mtdc32_setAdcOverride_db(s);
	mtdc32_setSlidingScaleOff_db(s);
	mtdc32_setSkipOutOfRange_db(s);
	for (gg = 0; gg <= 1; gg++) {
		mtdc32_setHoldDelay_db(s, gg);
		mtdc32_setHoldWidth_db(s, gg);
	}
	mtdc32_useGG_db(s);
	mtdc32_setInputRange_db(s);
	mtdc32_setEclTerm_db(s);
	mtdc32_setEclG1OrOsc_db(s);
	mtdc32_setEclFclOrRts_db(s);
	mtdc32_setNimG1OrOsc_db(s);
	mtdc32_setNimFclOrRts_db(s);
	mtdc32_setNimBusy_db(s);
	mtdc32_setTestPulser_db(s);
	mtdc32_setTsSource_db(s);
	mtdc32_setTsDivisor_db(s);

	bit = 1;
	for (ch = 0; ch < MTDC_NOF_CHANNELS; ch++) {
		if (chnPattern & bit) mtdc32_setThreshold_db(s, ch); else mtdc32_setThreshold(s, ch, MTDC32_D_CHAN_INACTIVE);
		bit <<= 1;
	}
}


bool_t mtdc32_dumpRegisters(struct s_mtdc32 * s, char * file)
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
	for (ch = 0; ch < MTDC_NOF_CHANNELS; ch++) fprintf(f, "   %2d: %d\n", ch, mtdc32_getThreshold(s, ch));

	mcstOrCblt = FALSE;
	if (flag = mtdc32_mcstIsEnabled(s))
					fprintf(f, "MCST signature [0x6024]   : %#x\n", mtdc32_getMcstSignature(s));
	else				fprintf(f, "MCST                      : disabled\n");
	mcstOrCblt |= flag;
	if (flag = mtdc32_cbltIsEnabled(s))
					fprintf(f, "CBLT signature [0x6022]   : %#x\n", mtdc32_getCbltSignature(s));
	else				fprintf(f, "CBLT                      : disabled\n");
	mcstOrCblt |= flag;
	if (mcstOrCblt) {
		if (mtdc32_isFirstInChain(s)) fprintf(f, "MCST/CBLT chain           : first module in chain\n");
		else if (mtdc32_isLastInChain(s)) fprintf(f, "MCST/CBLT chain           : last module in chain\n");
		else fprintf(f, "MCST/CBLT chain   : module in the middle\n");
	}
	fprintf(f, "Module ID [0x6004]        : %d\n", mtdc32_getModuleId(s));
	fprintf(f, "Data width [0x6032]       : %d\n", mtdc32_getDataWidth(s));
	fprintf(f, "Multi event [0x6036]      : %d\n", mtdc32_getMultiEvent(s));
	fprintf(f, "Buffer thresh [0x6018]    : %d\n", mtdc32_getBufferThresh(s));
	fprintf(f, "Max xfer data wc [0x601A] : %d\n", mtdc32_getXferData(s));
	fprintf(f, "Marking type [0x6038]     : %d\n", mtdc32_getMarkingType(s));
	fprintf(f, "Bank operation [0x6040]   : %d\n", mtdc32_getBankOperation(s));
	fprintf(f, "Adc resolution [0x6042]   : %d\n", mtdc32_getAdcResolution(s));
	fprintf(f, "Output format [0x6044]    : %d\n", mtdc32_getOutputFormat(s));
	fprintf(f, "Adc override [0x6046]     : %d\n", mtdc32_getAdcOverride(s));
	fprintf(f, "Sliding scale off [0x6048]: %d\n", mtdc32_getSlidingScaleOff(s));
	fprintf(f, "Skip out of range [0x604A]: %d\n", mtdc32_getSkipOutOfRange(s));
	for (gg = 0; gg <= 1; gg++) {
		fprintf(f, "Hold delay %d [0x605%d]     : %d\n", gg, gg*2, mtdc32_getHoldDelay(s, gg));
		fprintf(f, "Hold width %d [0x605%d]     : %d\n", gg, gg*2 + 4, mtdc32_getHoldWidth(s, gg), gg*2 + 4);
	}
	fprintf(f, "Use GG [0x6058]           : %d\n", mtdc32_getGGUsed(s));
	fprintf(f, "Input range [0x6060]      : %d\n", mtdc32_getInputRange(s));
	fprintf(f, "Ecl termination [0x6062]  : %#x\n", mtdc32_getEclTerm(s));
	fprintf(f, "Ecl gate or osc [0x6064]  : %d\n", mtdc32_getEclG1OrOsc(s));
	fprintf(f, "Ecl fcl or reset [0x6066] : %d\n", mtdc32_getEclFclOrRts(s));
	fprintf(f, "Nim gate or osc [0x606A]  : %d\n", mtdc32_getNimG1OrOsc(s));
	fprintf(f, "Nim fcl or reset [0x606C] : %d\n", mtdc32_getNimFclOrRts(s));
	fprintf(f, "Nim busy [0x606E]         : %d\n", mtdc32_getNimBusy(s));
	fprintf(f, "Pulser status [0x6070]    : %d\n", mtdc32_getTestPulser(s));
	fprintf(f, "Timestamp source [0x6096] : %#x\n", mtdc32_getTsSource(s));
	fprintf(f, "Timestamp divisor [0x6098]: %d\n", mtdc32_getTsDivisor(s));
	fclose(f);
}

bool_t mtdc32_dumpRaw(struct s_mtdc32 * s, char * file)
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

void mtdc32_printDb(struct s_mtdc32 * s)
{
	int ch;
	int gg;

	printf("Thresholds:\n");
	for (ch = 0; ch < MTDC_NOF_CHANNELS; ch++) printf("   %2d: %d\n", ch, s->threshold[ch]);

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
	printf("Buffer thresh     : %d\n", s->bufferThresh);
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
	printf("Ecl gate or osc   : %d\n", s->eclG1OrOsc);
	printf("Ecl fcl or reset  : %d\n", s->eclFclOrRts);
	printf("Nim gate or osc   : %d\n", s->nimG1OrOsc);
	printf("Nim fcl or reset  : %d\n", s->nimFclOrRts);
	printf("Nim busy          : %d\n", s->nimBusy);
	printf("Pulser status     : %d\n", s->testPulserStatus);
	printf("Timestamp source  : %#x\n", s->ctraTsSource);
	printf("Timestamp divisor : %d\n", s->ctraTsDivisor);
}

int mtdc32_readout(struct s_mtdc32 * s, uint32_t * pointer)
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

	tryIt = 20;
	while (tryIt-- && !mtdc32_dataReady(s)) { usleep(1000); }

	numData = (int) mtdc32_getFifoLength(s);	
	nd = (int) mtdc32_getFifoLength(s);
	while (tryIt-- && (nd != numData)) {
		numData = nd;
		nd = (int) mtdc32_getFifoLength(s);
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
		
		bmaError = bma_read(s->md->bltBase + MTDC32_DATA, ptrloc, numData, s->md->bltModeId);
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
		for (i = 0; i < numData; i++) *pointer++ = GET32(s->md->vmeBase, MTDC32_DATA);
	}

	if (s->repairRawData) pointer = mtdc32_repairRawData(s, pointer, dataStart);
	
	mtdc32_resetReadout(s);

	return (pointer - dataStart);
}

int mtdc32_readTimeB(struct s_mtdc32 * s, uint32_t * pointer)
{
	uint16_t * p16 = pointer;
	*p16++ = 0;
	*p16++ = GET16(s->md->vmeBase, MTDC32_CTRB_TIME_0);
	*p16++ = GET16(s->md->vmeBase, MTDC32_CTRB_TIME_1);
	*p16++ = GET16(s->md->vmeBase, MTDC32_CTRB_TIME_2);
	return 4 * sizeof(uint16_t) / sizeof(uint32_t);
}

bool_t mtdc32_dataReady(struct s_mtdc32 * s)
{
	return TSTB16(s->md->vmeBase, MTDC32_DATA_READY, 0) ;
}

void mtdc32_resetReadout(struct s_mtdc32 * s)
{
	SET16(s->md->vmeBase, MTDC32_READOUT_RESET, 0x1);
}

void mtdc32_resetTimestamp(struct s_mtdc32 * s)
{
	SET16(s->md->vmeBase, MTDC32_CTRA_RESET_A_OR_B, 0x3);
}

bool_t mtdc32_testBusError(struct s_mtdc32 * s)
{
	return TSTB16(s->md->vmeBase, MTDC32_MULTI_EVENT, MTDC32_MULTI_EVENT_BERR) == 1 ? FALSE : TRUE;
}

void mtdc32_enableBusError(struct s_mtdc32 * s)
{
	CLRB16(s->md->vmeBase, MTDC32_MULTI_EVENT, MTDC32_MULTI_EVENT_BERR);
}

void mtdc32_disableBusError(struct s_mtdc32 * s)
{
	SETB16(s->md->vmeBase, MTDC32_MULTI_EVENT, MTDC32_MULTI_EVENT_BERR);
}

void mtdc32_startAcq(struct s_mtdc32 * s)
{
	SET16(s->md->vmeBase, MTDC32_START_ACQUISITION, 0x0);
	mtdc32_resetFifo(s);
	mtdc32_resetReadout(s);
	mtdc32_resetTimestamp(s);
 	SET16(s->md->vmeBase, MTDC32_START_ACQUISITION, 0x1);
}

void mtdc32_stopAcq(struct s_mtdc32 * s)
{
	SET16(s->md->vmeBase, MTDC32_START_ACQUISITION, 0x0);
	mtdc32_resetFifo(s);
	mtdc32_resetReadout(s);
	mtdc32_resetTimestamp(s);
}

void mtdc32_resetFifo(struct s_mtdc32 * s)
{
	SET16(s->md->vmeBase, MTDC32_FIFO_RESET, 0x1);
}

void mtdc32_setMcstCblt_db(struct s_mtdc32 * s) {
	mtdc32_setMcstAddr(s, s->mcstSignature);
	mtdc32_setCbltAddr(s, s->cbltSignature);
	if (s->firstInChain) mtdc32_setFirstInChain(s);
	else if (s->lastInChain) mtdc32_setLastInChain(s);
	else mtdc32_setMiddleOfChain(s);
}

void mtdc32_setMcstAddr(struct s_mtdc32 * s, unsigned long Signature) {
	SET16(s->md->vmeBase, MTDC32_MCST_ADDRESS, Signature);
	if (Signature != 0) {
		s->mcstAddr = mapAdditionalVME(s->md, (Signature & 0xFF) << 24, 0);
		if (s->mcstAddr) mtdc32_setMcstEnable(s); else mtdc32_setMcstDisable(s);
	} else {
		mtdc32_setMcstDisable(s);
	}
}

uint16_t mtdc32_getMcstSignature(struct s_mtdc32 * s) {
	uint16_t addr8;
	addr8 = GET16(s->md->vmeBase, MTDC32_MCST_ADDRESS);
	return addr8;
}

void mtdc32_setMcstEnable(struct s_mtdc32 * s) {
	SET16(s->md->vmeBase, MTDC32_CBLT_MCST_CONTROL, MTDC32_MCST_ENA);
}

void mtdc32_setMcstDisable(struct s_mtdc32 * s) {
	SET16(s->md->vmeBase, MTDC32_CBLT_MCST_CONTROL, MTDC32_MCST_DIS);
}

bool_t mtdc32_mcstIsEnabled(struct s_mtdc32 * s) {
	uint16_t ctrl;
	ctrl = GET16(s->md->vmeBase, MTDC32_CBLT_MCST_CONTROL);
	return ((ctrl & MTDC32_MCST_DIS) != 0);
}

void mtdc32_setCbltAddr(struct s_mtdc32 * s, unsigned long Signature) {
	SET16(s->md->vmeBase, MTDC32_CBLT_ADDRESS, Signature);
	if (Signature != 0) {
		s->cbltAddr = mapAdditionalVME(s->md, (Signature & 0xFF) << 24, 0);
		if (s->cbltAddr) mtdc32_setCbltEnable(s); else mtdc32_setCbltDisable(s);
	} else {
		mtdc32_setCbltDisable(s);
	}
}

uint16_t mtdc32_getCbltSignature(struct s_mtdc32 * s) {
	uint16_t addr8;
	addr8 = GET16(s->md->vmeBase, MTDC32_CBLT_ADDRESS);
	return addr8;
}

void mtdc32_setCbltEnable(struct s_mtdc32 * s) {
	SET16(s->md->vmeBase, MTDC32_CBLT_MCST_CONTROL, MTDC32_CBLT_ENA);
}

void mtdc32_setCbltDisable(struct s_mtdc32 * s) {
	SET16(s->md->vmeBase, MTDC32_CBLT_MCST_CONTROL, MTDC32_CBLT_DIS);
}

bool_t mtdc32_cbltIsEnabled(struct s_mtdc32 * s) {
	uint16_t ctrl;
	ctrl = GET16(s->md->vmeBase, MTDC32_CBLT_MCST_CONTROL);
	return ((ctrl & MTDC32_CBLT_DIS) != 0);
}

void mtdc32_setFirstInChain(struct s_mtdc32 * s) {
	SET16(s->md->vmeBase, MTDC32_CBLT_MCST_CONTROL, MTDC32_CBLT_FIRST_ENA);
	SET16(s->md->vmeBase, MTDC32_CBLT_MCST_CONTROL, MTDC32_CBLT_LAST_DIS);
}

bool_t mtdc32_isFirstInChain(struct s_mtdc32 * s) {
	uint16_t ctrl;
	ctrl = GET16(s->md->vmeBase, MTDC32_CBLT_MCST_CONTROL);
	return ((ctrl & MTDC32_CBLT_FIRST_DIS) != 0);
}

void mtdc32_setLastInChain(struct s_mtdc32 * s) {
	SET16(s->md->vmeBase, MTDC32_CBLT_MCST_CONTROL, MTDC32_CBLT_LAST_ENA);
	SET16(s->md->vmeBase, MTDC32_CBLT_MCST_CONTROL, MTDC32_CBLT_FIRST_DIS);
}

bool_t mtdc32_isLastInChain(struct s_mtdc32 * s) {
	uint16_t ctrl;
	ctrl = GET16(s->md->vmeBase, MTDC32_CBLT_MCST_CONTROL);
	return ((ctrl & MTDC32_CBLT_LAST_DIS) != 0);
}

void mtdc32_setMiddleOfChain(struct s_mtdc32 * s) {
	SET16(s->md->vmeBase, MTDC32_CBLT_MCST_CONTROL, MTDC32_CBLT_FIRST_DIS);
	SET16(s->md->vmeBase, MTDC32_CBLT_MCST_CONTROL, MTDC32_CBLT_LAST_DIS);
}

bool_t mtdc32_isMiddleOfChain(struct s_mtdc32 * s) {
	bool_t first, last;
	first = mtdc32_isFirstInChain(s);
	last = mtdc32_isLastInChain(s);
	return (!first && !last);
}

void mtdc32_startAcq_mcst(struct s_mtdc32 * s)
{
	SET16(s->mcstAddr, MTDC32_START_ACQUISITION, 0x0);
	mtdc32_resetFifo_mcst(s);
	mtdc32_resetReadout_mcst(s);
	mtdc32_resetTimestamp_mcst(s);
	SET16(s->mcstAddr, MTDC32_START_ACQUISITION, 0x1);
}

void mtdc32_stopAcq_mcst(struct s_mtdc32 * s)
{
	SET16(s->mcstAddr, MTDC32_START_ACQUISITION, 0x0);
	mtdc32_resetFifo_mcst(s);
	mtdc32_resetReadout_mcst(s);
	mtdc32_resetTimestamp_mcst(s);
}

void mtdc32_resetFifo_mcst(struct s_mtdc32 * s)
{
	SET16(s->mcstAddr, MTDC32_FIFO_RESET, 0x1);
}

void mtdc32_resetReadout_mcst(struct s_mtdc32 * s)
{
	SET16(s->mcstAddr, MTDC32_READOUT_RESET, 0x1);
}

void mtdc32_resetTimestamp_mcst(struct s_mtdc32 * s)
{
	SET16(s->mcstAddr, MTDC32_CTRA_RESET_A_OR_B, 0x3);
}

bool_t mtdc32_updateSettings(struct s_mtdc32 * s, char * updFile)
{
	struct stat sbuf;
	if (s->updCountDown <= 0) {
		s->updCountDown = s->updInterval;
		if (stat(updFile, &sbuf) == 0) {
			unlink(updFile);
			return TRUE;
		}
	}
	return FALSE;
}

uint32_t * mtdc32_repairRawData(struct s_mtdc32 * s, uint32_t * pointer, uint32_t * dataStart) {
	return pointer;
}

void catchBerr() {
	
	mtdc32_resetReadout(be);
	sprintf(msg, "[%scatchBerr] %s: Bus Error!", be->mpref, be->moduleName);
	f_ut_send_msg(be->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
}

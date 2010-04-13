/*_______________________________________________________[C LIBRARY FUNCTIONS]
//////////////////////////////////////////////////////////////////////////////
//! \file			madc32fct.c
//! \brief			Code for module MADC32
//! \details		Implements functions to handle modules of type Mesytec MADC32
//!
//! $Author: Rudolf.Lutter $
//! $Mail			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.9 $
//! $Date: 2010-04-13 08:28:52 $
////////////////////////////////////////////////////////////////////////////*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <allParam.h>
#include <ces/bmalib.h>
#include <errno.h>
#include <sigcodes.h>

#include "gd_readout.h"

#include "madc32.h"
#include "madc32_protos.h"

#include "root_env.h"

#include "err_mask_def.h"
#include "errnum_def.h"

void catchBerr();

char msg[256];

struct s_madc32 * madc32_alloc(unsigned long vmeAddr, volatile unsigned char * base, char * moduleName, int serial)
{
	struct s_madc32 * s;
	s = (struct s_madc32 *) calloc(1, sizeof(struct s_madc32));
	if (s != NULL) {
		s->baseAddr = base;
		s->vmeAddr = vmeAddr;
		strcpy(s->moduleName, moduleName);
		strcpy(s->prefix, "m_read_meb");
		strcpy(s->mpref, "madc32: ");
		s->serial = serial;
		s->verbose = FALSE;
		s->dumpRegsOnInit = FALSE;
	} else {
		sprintf(msg, "[%salloc] %s: Can't allocate madc32 struct", s->mpref, s->moduleName);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
	}
	return s;
}

void madc32_initialize(struct s_madc32 * s)
{
	signal(SIGBUS, catchBerr);
	madc32_disableBusError(s);
	madc32_enable_bma(s);
	madc32_resetReadout(s);
}

void madc32_setBltBlockSize(struct s_madc32 * s, uint32_t size)
{
  if ((size * sizeof(uint32_t)) <= s->bltBufferSize) s->bltBlockSize = size;
}

void madc32_reset(struct s_madc32 * s)
{
  SET16(s->baseAddr, MADC32_SOFT_RESET, 0x1);
}

uint16_t madc32_getThreshold(struct s_madc32 * s, uint16_t channel)
{
	uint16_t thresh = 0;
	if (channel < NOF_CHANNELS) thresh = GET16(s->baseAddr, MADC32_THRESHOLD + sizeof(uint16_t) * channel) & MADC32_THRESHOLD_MASK;
	return thresh;
}

void madc32_setThreshold_db(struct s_madc32 * s, uint16_t channel)
{
	if (channel < NOF_CHANNELS) madc32_setThreshold(s, channel, s->threshold[channel]);
}

void madc32_setThreshold(struct s_madc32 * s, uint16_t channel, uint16_t thresh)
{
	if (channel < NOF_CHANNELS) SET16(s->baseAddr, MADC32_THRESHOLD + sizeof(uint16_t) * channel, thresh & MADC32_THRESHOLD_MASK);
}

void madc32_setAddrReg_db(struct s_madc32 * s) { madc32_setAddrReg(s, s->addrReg); }

void madc32_setAddrReg(struct s_madc32 * s, uint16_t vmeAddr)
{
	if (vmeAddr) {
		SET16(s->baseAddr, MADC32_ADDR_REG, vmeAddr);
		SET16(s->baseAddr, MADC32_ADDR_SOURCE, MADC32_ADDR_SOURCE_REG);
		s->baseAddr = vmeAddr;
	}
}

uint16_t madc32_getAddrReg(struct s_madc32 * s)
{
	uint16_t source = GET16(s->baseAddr, MADC32_ADDR_SOURCE);
	if (source & MADC32_ADDR_SOURCE_REG) return GET16(s->baseAddr, MADC32_ADDR_REG);
	else return 0;
}

void madc32_setModuleId_db(struct s_madc32 * s) { madc32_setModuleId(s, s->moduleId); }

void madc32_setModuleId(struct s_madc32 * s, uint16_t id)
{
	SET16(s->baseAddr, MADC32_MODULE_ID, id & MADC32_MODULE_ID_MASK);
}

uint16_t madc32_getModuleId(struct s_madc32 * s)
{
	return GET16(s->baseAddr, MADC32_MODULE_ID) & MADC32_MODULE_ID_MASK;
}

uint16_t madc32_getFifoLength(struct s_madc32 * s)
{
	return GET16(s->baseAddr, MADC32_BUFFER_DATA_LENGTH) & MADC32_BUFFER_DATA_LENGTH_MASK;
}

void madc32_setDataWidth_db(struct s_madc32 * s) { madc32_setDataWidth(s, s->dataWidth); }

void madc32_setDataWidth(struct s_madc32 * s, uint16_t width)
{
	SET16(s->baseAddr, MADC32_DATA_LENGTH_FORMAT, width & MADC32_DATA_LENGTH_FORMAT_MASK);
}

uint16_t madc32_getDataWidth(struct s_madc32 * s)
{
	return GET16(s->baseAddr, MADC32_DATA_LENGTH_FORMAT) & MADC32_DATA_LENGTH_FORMAT_MASK;
}

void madc32_setMultiEvent_db(struct s_madc32 * s) { madc32_setMultiEvent(s, s->multiEvent); }

void madc32_setMultiEvent(struct s_madc32 * s, uint16_t mode)
{
	SET16(s->baseAddr, MADC32_MULTI_EVENT, mode & MADC32_MULTI_EVENT_MASK);
}

uint16_t madc32_getMultiEvent(struct s_madc32 * s)
{
	return GET16(s->baseAddr, MADC32_MULTI_EVENT) & MADC32_MULTI_EVENT_MASK;
}

void madc32_setXferData_db(struct s_madc32 * s) { madc32_setXferData(s, s->xferData); }

void madc32_setXferData(struct s_madc32 * s, uint16_t wc)
{
	SET16(s->baseAddr, MADC32_MAX_XFER_DATA, wc & MADC32_MAX_XFER_DATA_MASK);
}

uint16_t madc32_getXferData(struct s_madc32 * s)
{
	return GET16(s->baseAddr, MADC32_MAX_XFER_DATA) & MADC32_MAX_XFER_DATA_MASK;
}

void madc32_setMarkingType_db(struct s_madc32 * s) { madc32_setMarkingType(s, s->markingType); }

void madc32_setMarkingType(struct s_madc32 * s, uint16_t type)
{
	SET16(s->baseAddr, MADC32_MARKING_TYPE, type & MADC32_MARKING_TYPE_MASK);
}

uint16_t madc32_getMarkingType(struct s_madc32 * s)
{
	return GET16(s->baseAddr, MADC32_MARKING_TYPE) & MADC32_MARKING_TYPE_MASK;
}

void madc32_setBankOperation_db(struct s_madc32 * s) { madc32_setBankOperation(s, s->bankOperation); }

void madc32_setBankOperation(struct s_madc32 * s, uint16_t oper)
{
	SET16(s->baseAddr, MADC32_BANK_OPERATION, oper & MADC32_BANK_OPERATION_MASK);
}

uint16_t madc32_getBankOperation(struct s_madc32 * s)
{
	return GET16(s->baseAddr, MADC32_BANK_OPERATION) & MADC32_BANK_OPERATION_MASK;
}

void madc32_setAdcResolution_db(struct s_madc32 * s) { madc32_setAdcResolution(s, s->adcResolution); }

void madc32_setAdcResolution(struct s_madc32 * s, uint16_t res)
{
	SET16(s->baseAddr, MADC32_ADC_RESOLUTION, res & MADC32_ADC_RESOLUTION_MASK);
}

uint16_t madc32_getAdcResolution(struct s_madc32 * s)
{
	return GET16(s->baseAddr, MADC32_ADC_RESOLUTION) & MADC32_ADC_RESOLUTION_MASK;
}

void madc32_setOutputFormat_db(struct s_madc32 * s) { madc32_setOutputFormat(s, s->outputFormat); }

void madc32_setOutputFormat(struct s_madc32 * s, uint16_t format)
{
	SET16(s->baseAddr, MADC32_OUTPUT_FORMAT, format & MADC32_OUTPUT_FORMAT_MASK);
}

uint16_t madc32_getOutputFormat(struct s_madc32 * s)
{
	return GET16(s->baseAddr, MADC32_OUTPUT_FORMAT) & MADC32_OUTPUT_FORMAT_MASK;
}

void madc32_setAdcOverride_db(struct s_madc32 * s) { madc32_setAdcOverride(s, s->adcOverride); }

void madc32_setAdcOverride(struct s_madc32 * s, uint16_t over)
{
	SET16(s->baseAddr, MADC32_ADC_OVERRIDE, over & MADC32_ADC_RESOLUTION_MASK);
}

uint16_t madc32_getAdcOverride(struct s_madc32 * s)
{
	return GET16(s->baseAddr, MADC32_ADC_OVERRIDE) & MADC32_ADC_RESOLUTION_MASK;
}

void madc32_setSlidingScaleOff_db(struct s_madc32 * s) { madc32_setSlidingScaleOff(s, s->slidingScaleOff); }

void madc32_setSlidingScaleOff(struct s_madc32 * s, bool_t flag)
{
	uint16_t f = flag ? 1 : 0;
	SET16(s->baseAddr, MADC32_SLIDING_SCALE_OFF, f);
}

bool_t madc32_getSlidingScaleOff(struct s_madc32 * s)
{
	uint16_t f = GET16(s->baseAddr, MADC32_SLIDING_SCALE_OFF) & MADC32_SLIDING_SCALE_OFF_MASK;
	return (f != 0) ? TRUE : FALSE;
}

void madc32_setSkipOutOfRange_db(struct s_madc32 * s) { madc32_setSkipOutOfRange(s, s->skipOutOfRange); }

void madc32_setSkipOutOfRange(struct s_madc32 * s, bool_t flag)
{
	uint16_t f = flag ? 1 : 0;
	SET16(s->baseAddr, MADC32_SKIP_OUT_OF_RANGE, f);
}

bool_t madc32_getSkipOutOfRange(struct s_madc32 * s)
{
	uint16_t f = GET16(s->baseAddr, MADC32_SKIP_OUT_OF_RANGE) & MADC32_SKIP_OUT_OF_RANGE_MASK;
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
	SET16(s->baseAddr, offs, delay & MADC32_GG_HOLD_DELAY_MASK);
}

uint16_t madc32_getHoldDelay(struct s_madc32 * s, uint16_t gg)
{
	int offs;
	switch (gg) {
		case 0: offs = MADC32_GG_HOLD_DELAY_0; break;
		case 1: offs = MADC32_GG_HOLD_DELAY_1; break;
		default: return 0;
	}
	return GET16(s->baseAddr, offs) & MADC32_GG_HOLD_DELAY_MASK;
}

void madc32_setHoldWidth_db(struct s_madc32 * s, uint16_t gg) { madc32_setHoldWidth(s, gg, s->ggHoldWidth[gg]); }

void madc32_setHoldWidth(struct s_madc32 * s, uint16_t width, uint16_t gg)
{
	int offs;
	switch (gg) {
		case 0: offs = MADC32_GG_HOLD_WIDTH_0; break;
		case 1: offs = MADC32_GG_HOLD_WIDTH_1; break;
		default: return;
	}
	SET16(s->baseAddr, offs, width & MADC32_GG_HOLD_WIDTH_MASK);
}

uint16_t madc32_getHoldWidth(struct s_madc32 * s, uint16_t gg)
{
	int offs;
	switch (gg) {
		case 0: offs = MADC32_GG_HOLD_WIDTH_0; break;
		case 1: offs = MADC32_GG_HOLD_WIDTH_1; break;
		default: return 0;
	}
	return GET16(s->baseAddr, offs) & MADC32_GG_HOLD_WIDTH_MASK;
}

void madc32_useGG_db(struct s_madc32 * s) { madc32_useGG(s, s->useGG); }

void madc32_useGG(struct s_madc32 * s, uint16_t gg)
{
	SET16(s->baseAddr, MADC32_USE_GATE_GENERATOR, gg & MADC32_USE_GATE_GENERATOR_MASK);
}

uint16_t madc32_getGGUsed(struct s_madc32 * s)
{
	return GET16(s->baseAddr, MADC32_USE_GATE_GENERATOR) & MADC32_USE_GATE_GENERATOR_MASK;
}

void madc32_setInputRange_db(struct s_madc32 * s) { madc32_setInputRange(s, s->inputRange); }

void madc32_setInputRange(struct s_madc32 * s, uint16_t range)
{
	SET16(s->baseAddr, MADC32_INPUT_RANGE, range & MADC32_INPUT_RANGE_MASK);
}

uint16_t madc32_getInputRange(struct s_madc32 * s)
{
	return GET16(s->baseAddr, MADC32_INPUT_RANGE) & MADC32_INPUT_RANGE_MASK;
}

void madc32_setEclTerm_db(struct s_madc32 * s) { madc32_setEclTerm(s, s->eclTerm); }

void madc32_setEclTerm(struct s_madc32 * s, uint16_t term)
{
	SET16(s->baseAddr, MADC32_ECL_TERMINATORS, term & MADC32_ECL_TERMINATORS_MASK);
}

uint16_t madc32_getEclTerm(struct s_madc32 * s)
{
	return GET16(s->baseAddr, MADC32_ECL_TERMINATORS) & MADC32_ECL_TERMINATORS_MASK;
}

void madc32_setEclG1OrOsc_db(struct s_madc32 * s) { madc32_setEclG1OrOsc(s, s->eclG1OrOsc); }

void madc32_setEclG1OrOsc(struct s_madc32 * s, uint16_t go)
{
	SET16(s->baseAddr, MADC32_ECL_G1_OR_OSC, go & MADC32_ECL_G1_OR_OSC_MASK);
}

uint16_t madc32_getEclG1OrOsc(struct s_madc32 * s)
{
	return GET16(s->baseAddr, MADC32_ECL_G1_OR_OSC) & MADC32_ECL_G1_OR_OSC_MASK;
}

void madc32_setEclFclOrRts_db(struct s_madc32 * s) { madc32_setEclFclOrRts(s, s->eclFclOrRts); }

void madc32_setEclFclOrRts(struct s_madc32 * s, uint16_t fr)
{
	SET16(s->baseAddr, MADC32_ECL_FCL_OR_RES_TS, fr & MADC32_ECL_FCL_OR_RES_TS_MASK);
}

uint16_t madc32_getEclFclOrRts(struct s_madc32 * s)
{
	return GET16(s->baseAddr, MADC32_ECL_FCL_OR_RES_TS) & MADC32_ECL_FCL_OR_RES_TS_MASK;
}

void madc32_setNimG1OrOsc_db(struct s_madc32 * s) { madc32_setNimG1OrOsc(s, s->nimG1OrOsc); }

void madc32_setNimG1OrOsc(struct s_madc32 * s, uint16_t go)
{
	SET16(s->baseAddr, MADC32_NIM_G1_OR_OSC, go & MADC32_NIM_G1_OR_OSC_MASK);
}

uint16_t madc32_getNimG1OrOsc(struct s_madc32 * s)
{
	return GET16(s->baseAddr, MADC32_NIM_G1_OR_OSC) & MADC32_NIM_G1_OR_OSC_MASK;
}


void madc32_setNimFclOrRts_db(struct s_madc32 * s) { madc32_setNimFclOrRts(s, s->nimFclOrRts); }

void madc32_setNimFclOrRts(struct s_madc32 * s, uint16_t fr)
{
	SET16(s->baseAddr, MADC32_NIM_FCL_OR_RES_TS, fr & MADC32_NIM_FCL_OR_RES_TS_MASK);
}

uint16_t madc32_getNimFclOrRts(struct s_madc32 * s)
{
	return GET16(s->baseAddr, MADC32_NIM_FCL_OR_RES_TS) & MADC32_NIM_FCL_OR_RES_TS_MASK;
}

void madc32_setNimBusy_db(struct s_madc32 * s) { madc32_setNimBusy(s, s->nimBusy); }

void madc32_setNimBusy(struct s_madc32 * s, uint16_t busy)
{
	SET16(s->baseAddr, MADC32_NIM_BUSY, busy & MADC32_NIM_BUSY_MASK);
}

uint16_t madc32_getNimBusy(struct s_madc32 * s)
{
	return GET16(s->baseAddr, MADC32_NIM_BUSY) & MADC32_NIM_BUSY_MASK;
}

void madc32_setTestPulser_db(struct s_madc32 * s) { madc32_setTestPulser(s, s->testPulserStatus); }

void madc32_setTestPulser(struct s_madc32 * s, uint16_t mode)
{
	SET16(s->baseAddr, MADC32_TEST_PULSER_STATUS, mode & MADC32_TEST_PULSER_STATUS_MASK);
}

uint16_t madc32_getTestPulser(struct s_madc32 * s)
{
	return GET16(s->baseAddr, MADC32_TEST_PULSER_STATUS) & MADC32_TEST_PULSER_STATUS_MASK;
}

void madc32_setTsSource_db(struct s_madc32 * s) { madc32_setTsSource(s, s->ctraTsSource); }

void madc32_setTsSource(struct s_madc32 * s, uint16_t source)
{
	SET16(s->baseAddr, MADC32_CTRA_TS_SOURCE, source & MADC32_CTRA_TS_SOURCE_MASK);
}

uint16_t madc32_getTsSource(struct s_madc32 * s)
{
	return GET16(s->baseAddr, MADC32_CTRA_TS_SOURCE) & MADC32_CTRA_TS_SOURCE_MASK;
}

void madc32_setTsDivisor_db(struct s_madc32 * s) { madc32_setTsDivisor(s, s->ctraTsDivisor); }

void madc32_setTsDivisor(struct s_madc32 * s, uint16_t div)
{
	SET16(s->baseAddr, MADC32_CTRA_TS_DIVISOR, div & MADC32_CTRA_TS_DIVISOR_MASK);
}

uint16_t madc32_getTsDivisor(struct s_madc32 * s)
{
	return GET16(s->baseAddr, MADC32_CTRA_TS_DIVISOR) & MADC32_CTRA_TS_DIVISOR_MASK;
}

void madc32_moduleInfo(struct s_madc32 * s)
{
	int firmware;
	firmware = GET16(s->baseAddr, MADC32_FIRMWARE_REV);
	sprintf(msg, "[%smodule_info] %s: phys addr %#lx, mapped to %#lx, firmware %02x.%02x",
									s->mpref,
									s->moduleName,
									s->vmeAddr,
									s->baseAddr,
									((firmware >> 8) & 0xff), (firmware & 0xff));
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

	s->updSettings = root_env_getval_b("MADC32.UpdateSettings", FALSE);
	s->updInterval = root_env_getval_i("MADC32.UpdateInterval", 0);
	s->updCountDown = 0;

	sp = root_env_getval_s("MADC32.ModuleName", "");
	if (strcmp(s->moduleName, "madc32") != 0 && strcmp(sp, s->moduleName) != 0) {
		sprintf(msg, "[%sfill_struct] %s: File %s contains wrong module name - %s", s->mpref, s->moduleName, file, sp);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		return FALSE;
	}
	strcpy(mnUC, sp);
	mnUC[0] = toupper(mnUC[0]);

	sprintf(res, "MADC32.%s.BlockXfer", mnUC);
	s->blockTransOn = root_env_getval_b(res, FALSE);

	for (i = 0; i < NOF_CHANNELS; i++) {
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

	sprintf(res, "MADC32.%s.EclG1OrOsc", mnUC);
	s->eclG1OrOsc = root_env_getval_i(res, MADC32_ECL_G1_OR_OSC_DEFAULT);

	sprintf(res, "MADC32.%s.EclFclOrRts", mnUC);
	s->eclFclOrRts = root_env_getval_i(res, MADC32_NIM_FCL_OR_RES_TS_DEFAULT);

	sprintf(res, "MADC32.%s.NimG1OrOsc", mnUC);
	s->nimG1OrOsc = root_env_getval_i(res, MADC32_NIM_G1_OR_OSC_DEFAULT);

	sprintf(res, "MADC32.%s.NimFclOrRts", mnUC);
	s->nimFclOrRts = root_env_getval_i(res, MADC32_NIM_FCL_OR_RES_TS_DEFAULT);

	sprintf(res, "MADC32.%s.NimBusy", mnUC);
	s->nimBusy = root_env_getval_i(res, MADC32_NIM_BUSY_DEFAULT);

	sprintf(res, "MADC32.%s.PulserStatus", mnUC);
	s->testPulserStatus = root_env_getval_i(res, MADC32_TEST_PULSER_STATUS_DEFAULT);

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
	madc32_setEclG1OrOsc_db(s);
	madc32_setEclFclOrRts_db(s);
	madc32_setNimG1OrOsc_db(s);
	madc32_setNimFclOrRts_db(s);
	madc32_setNimBusy_db(s);
	madc32_setTestPulser_db(s);
	madc32_setTsSource_db(s);
	madc32_setTsDivisor_db(s);

	bit = 1;
	for (ch = 0; ch < NOF_CHANNELS; ch++) {
		if (chnPattern & bit) madc32_setThreshold_db(s, ch); else madc32_setThreshold(s, ch, MADC32_D_CHAN_INACTIVE);
		bit <<= 1;
	}
}


bool_t madc32_dumpRegisters(struct s_madc32 * s, char * file)
{
	FILE * f;

	int ch;
	int gg;

	if (!s->dumpRegsOnInit) return(TRUE);

	f = fopen(file, "w");
	if (f == NULL) {
		sprintf(msg, "[%sdumpRegisters] %s: Error writing file %s", s->mpref, s->moduleName, file);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		return FALSE;
	}

	sprintf(msg, "[%sdumpRegisters] %s: Dumping settings to file %s", s->mpref, s->moduleName, file);
	f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);

	fprintf(f, "Thresholds:\n");
	for (ch = 0; ch < NOF_CHANNELS; ch++) fprintf(f, "   %2d: %d\n", ch, madc32_getThreshold(s, ch));

	fprintf(f, "Addr reg          : %#x\n", madc32_getAddrReg(s));
	fprintf(f, "Module ID         : %d\n", madc32_getModuleId(s));
	fprintf(f, "Data width        : %d\n", madc32_getDataWidth(s));
	fprintf(f, "Multi event       : %d\n", madc32_getMultiEvent(s));
	fprintf(f, "Max xfer data wc  : %d\n", madc32_getXferData(s));
	fprintf(f, "Marking type      : %d\n", madc32_getMarkingType(s));
	fprintf(f, "Bank operation    : %d\n", madc32_getBankOperation(s));
	fprintf(f, "Adc resolution    : %d\n", madc32_getAdcResolution(s));
	fprintf(f, "Output format     : %d\n", madc32_getOutputFormat(s));
	fprintf(f, "Adc override      : %d\n", madc32_getAdcOverride(s));
	fprintf(f, "Sliding scale off : %d\n", madc32_getSlidingScaleOff(s));
	fprintf(f, "Skip out of range : %d\n", madc32_getSkipOutOfRange(s));
	for (gg = 0; gg <= 1; gg++) {
		fprintf(f, "Hold delay %d      : %d\n", gg, madc32_getHoldDelay(s, gg));
		fprintf(f, "Hold width %d      : %d\n", gg, madc32_getHoldWidth(s, gg));
	}
	fprintf(f, "Use GG            : %d\n", madc32_getGGUsed(s));
	fprintf(f, "Input range       : %d\n", madc32_getInputRange(s));
	fprintf(f, "Ecl termination   : %#x\n", madc32_getEclTerm(s));
	fprintf(f, "Ecl gate or osc   : %d\n", madc32_getEclG1OrOsc(s));
	fprintf(f, "Ecl fcl or reset  : %d\n", madc32_getEclFclOrRts(s));
	fprintf(f, "Nim gate or osc   : %d\n", madc32_getNimG1OrOsc(s));
	fprintf(f, "Nim fcl or reset  : %d\n", madc32_getNimFclOrRts(s));
	fprintf(f, "Nim busy          : %d\n", madc32_getNimBusy(s));
	fprintf(f, "Pulser status     : %d\n", madc32_getTestPulser(s));
	fprintf(f, "Timestamp source  : %#x\n", madc32_getTsSource(s));
	fprintf(f, "Timestamp divisor : %d\n", madc32_getTsDivisor(s));
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
		fprintf(f, "%#lx %#x\n", i, GET16(s->baseAddr, i));
	}
	fclose(f);
}

void madc32_printDb(struct s_madc32 * s)
{
	int ch;
	int gg;

	printf("Thresholds:\n");
	for (ch = 0; ch < NOF_CHANNELS; ch++) printf("   %2d: %d\n", ch, s->threshold[ch]);

	printf("Addr reg          : %#x\n", s->addrReg);
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
	printf("Ecl gate or osc   : %d\n", s->eclG1OrOsc);
	printf("Ecl fcl or reset  : %d\n", s->eclFclOrRts);
	printf("Nim gate or osc   : %d\n", s->nimG1OrOsc);
	printf("Nim fcl or reset  : %d\n", s->nimFclOrRts);
	printf("Nim busy          : %d\n", s->nimBusy);
	printf("Pulser status     : %d\n", s->testPulserStatus);
	printf("Timestamp source  : %#x\n", s->ctraTsSource);
	printf("Timestamp divisor : %d\n", s->ctraTsDivisor);
}

void madc32_enable_bma(struct s_madc32 * s)
{
/* use blocktransfer */
/* buffersize (in 32bit words) : Memory     = 1026 * 4    */

	if (s->blockTransOn) {
		s->bltBufferSize = MADC32_BLT_BUFFER_SIZE;
		if ((s->bma = bmaAlloc(s->bltBufferSize)) == NULL) {
			sprintf(msg, "[%senable_bma] %s: %s, turning BlockXfer OFF", s->mpref, s->moduleName,  sys_errlist[errno]);
			f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
			s->blockTransOn = FALSE;
			return;
		}
		s->bltBuffer = (unsigned char *) s->bma->virtBase;
		sprintf(msg, "[%senable_bma] %s: turning block xfer ON (buffer=%#lx, size=%d)", s->mpref, s->moduleName, s->bltBuffer, s->bltBufferSize);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
	} else {
		s->bma = NULL;
		s->bltBufferSize = 0;
		s->bltBuffer = NULL;
		sprintf(msg, "[%senable_bma] %s: Block xfer is OFF", s->mpref, s->moduleName);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
	}
}

int madc32_readout(struct s_madc32 * s, uint32_t * pointer)
{
	uint32_t * dataStart;
	uint32_t data;
	int numData;
	unsigned int i;

	dataStart = pointer;

	if (!madc32_dataReady(s)) {
		*pointer++ = 0xaffec0c0;
		madc32_resetFifo(s);
		madc32_startAcq(s);
		return (pointer - dataStart);
	}

	numData = (int) madc32_getFifoLength(s);

	if (s->blockTransOn) {
		if (bmaResetChain(s->bma) < 0) {
			sprintf(msg, "[%sreadout] %s: resetting block xfer chai
n failed", s->mpref, s->moduleName);
			f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
			return(0);
		}

		if (bmaReadChain(s->bma, 0, s->vmeAddr + MADC32_DATA, numData, 0x0b) < 0) {
			sprintf(msg, "[%sreadout] %s: block xfer failed while reading event data", s->mpref, s->moduleName);
			f_ut_send_msg("sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
			return(0);
		}

		if (bmaStartChain(s->bma) < 0) {
			sprintf(msg, "[%sreadout] %s: starting block xfer chain failed", s->mpref, s->moduleName);
			f_ut_send_msg("sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
			return(0);
		}

		if (bmaWaitChain(s->bma) < 0) {
			sprintf(msg, "[%sreadout] %s: waiting for block xfer chain failed", s->mpref, s->moduleName);
			f_ut_send_msg("sis_3300", msg, ERR__MSG_INFO, MASK__PRTT);
			return(0);
		}

		memcpy(pointer, s->bltBuffer, sizeof(uint32_t) * numData);
		pointer += numData;
	} else {
		for (i = 0; i < numData; i++) {
			data = GET32(s->baseAddr, MADC32_DATA);
			if (data == 0) {
				s->evtp++; *s->evtp = (MADC32_M_TRAILER | 0x00525252);
				pointer = madc32_pushEvent(s, pointer);
				madc32_resetEventBuffer(s);
				s->skipData = TRUE;
				continue;
			} else if ((data & MADC32_M_SIGNATURE) == MADC32_M_HEADER) {
				s->skipData = FALSE;
				if (s->evtp != s->evtBuf) {
					s->evtp++; *s->evtp = (MADC32_M_TRAILER | 0x00252525);
					pointer = madc32_pushEvent(s, pointer);
				}
				madc32_resetEventBuffer(s);
				*s->evtp = data;
			} else if ((data & MADC32_M_SIGNATURE) == MADC32_M_TRAILER) {
				if (s->skipData) continue;
				s->evtp++; *s->evtp = data;
				pointer = madc32_pushEvent(s, pointer);
				madc32_resetEventBuffer(s);
			} else if ((data & MADC32_M_SIGNATURE) == MADC32_M_EOB) {
				break;
			} else {
				if (s->skipData) continue;
				s->evtp++; *s->evtp = data;
			}
		}
	}

	madc32_resetReadout(s);

	return (pointer - dataStart);
}

uint32_t * madc32_pushEvent(struct s_madc32 * s, uint32_t * pointer) {
	int i;
	int wc = s->evtp - s->evtBuf + 1;
	uint32_t * p = s->evtBuf;
	for (i = 0; i < wc; i++) *pointer++ = *p++;
	return (pointer);
}

int madc32_readTimeB(struct s_madc32 * s, uint32_t * pointer)
{
	uint16_t * p16 = pointer;
	*p16++ = 0;
	*p16++ = GET16(s->baseAddr, MADC32_CTRB_TIME_0);
	*p16++ = GET16(s->baseAddr, MADC32_CTRB_TIME_1);
	*p16++ = GET16(s->baseAddr, MADC32_CTRB_TIME_2);
	return 4 * sizeof(uint16_t) / sizeof(uint32_t);
}

bool_t madc32_dataReady(struct s_madc32 * s)
{
	return TSTB16(s->baseAddr, MADC32_DATA_READY, 0) ;
}

void madc32_resetReadout(struct s_madc32 * s)
{
	SET16(s->baseAddr, MADC32_READOUT_RESET, 0x1);
}

bool_t madc32_testBusError(struct s_madc32 * s)
{
	return TSTB16(s->baseAddr, MADC32_MULTI_EVENT, MADC32_MULTI_EVENT_BERR) == 1 ? FALSE : TRUE;
}

void madc32_enableBusError(struct s_madc32 * s)
{
	CLRB16(s->baseAddr, MADC32_MULTI_EVENT, MADC32_MULTI_EVENT_BERR);
}

void madc32_disableBusError(struct s_madc32 * s)
{
	SETB16(s->baseAddr, MADC32_MULTI_EVENT, MADC32_MULTI_EVENT_BERR);
}

void madc32_startAcq(struct s_madc32 * s)
{
	SET16(s->baseAddr, MADC32_START_ACQUISITION, 0x0);
	madc32_resetFifo(s);
	madc32_resetReadout(s);
	madc32_resetEventBuffer(s);
	SET16(s->baseAddr, MADC32_START_ACQUISITION, 0x1);
}

void madc32_stopAcq(struct s_madc32 * s)
{
	SET16(s->baseAddr, MADC32_START_ACQUISITION, 0x0);
	madc32_resetFifo(s);
	madc32_resetReadout(s);
	madc32_resetEventBuffer(s);
}

void madc32_resetFifo(struct s_madc32 * s)
{
	SET16(s->baseAddr, MADC32_FIFO_RESET, 0x1);
}

bool_t madc32_updateSettings(struct s_madc32 * s, char * updFile)
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

void madc32_resetEventBuffer(struct s_madc32 * s) {
	memset(s->evtBuf, 0, sizeof(s->evtBuf));
	s->evtp = s->evtBuf;
	s->skipData = FALSE;
}

void catchBerr() {}

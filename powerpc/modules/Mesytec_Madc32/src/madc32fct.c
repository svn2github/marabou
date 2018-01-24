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
#include <signal.h>
#include <math.h>
#include <string.h>
#include <ces/uiocmd.h>
#include <ces/bmalib.h>
#include <errno.h>

#include "gd_readout.h"

#include "mesy.h"
#include "mesy_database.h"
#include "mesy_protos.h"

#include "madc32.h"
#include "madc32_database.h"
#include "madc32_protos.h"

#include "root_env.h"
#include "mapping_database.h"
#include "bmaErrlist.h"

#include "err_mask_def.h"
#include "errnum_def.h"

void(*signal(sig, func))();
bool_t busError;

int numData;
int rdoWc;

int evtWc;
int lastData;

char msg[256];

uint16_t madc32_getThreshold(s_madc32 * s, uint16_t channel)
{
	uint16_t thresh = 0;
	if (channel < MADC_NOF_CHANNELS) thresh = GET16(s->m.md->vmeBase, MADC32_THRESHOLD + sizeof(uint16_t) * channel) & MADC32_THRESHOLD_MASK;
	return thresh;
}

void madc32_setThreshold_db(s_madc32 * s, uint16_t channel)
{
	if (channel < MADC_NOF_CHANNELS) madc32_setThreshold(s, channel, s->threshold[channel]);
}

void madc32_setThreshold(s_madc32 * s, uint16_t channel, uint16_t thresh)
{
	if (channel < MADC_NOF_CHANNELS) SET16(s->m.md->vmeBase, MADC32_THRESHOLD + sizeof(uint16_t) * channel, thresh & MADC32_THRESHOLD_MASK);
}

void madc32_setBankOperation_db(s_madc32 * s) { madc32_setBankOperation(s, s->bankOperation); }

void madc32_setBankOperation(s_madc32 * s, uint16_t oper)
{
	SET16(s->m.md->vmeBase, MADC32_BANK_OPERATION, oper & MADC32_BANK_OPERATION_MASK);
}

uint16_t madc32_getBankOperation(s_madc32 * s)
{
	return GET16(s->m.md->vmeBase, MADC32_BANK_OPERATION) & MADC32_BANK_OPERATION_MASK;
}

void madc32_setAdcResolution_db(s_madc32 * s) { madc32_setAdcResolution(s, s->adcResolution); }

void madc32_setAdcResolution(s_madc32 * s, uint16_t res)
{
	SET16(s->m.md->vmeBase, MADC32_ADC_RESOLUTION, res & MADC32_ADC_RESOLUTION_MASK);
}

uint16_t madc32_getAdcResolution(s_madc32 * s)
{
	return GET16(s->m.md->vmeBase, MADC32_ADC_RESOLUTION) & MADC32_ADC_RESOLUTION_MASK;
}

void madc32_setOutputFormat_db(s_madc32 * s) { madc32_setOutputFormat(s, s->outputFormat); }

void madc32_setOutputFormat(s_madc32 * s, uint16_t format)
{
	SET16(s->m.md->vmeBase, MADC32_OUTPUT_FORMAT, format & MADC32_OUTPUT_FORMAT_MASK);
}

uint16_t madc32_getOutputFormat(s_madc32 * s)
{
	return GET16(s->m.md->vmeBase, MADC32_OUTPUT_FORMAT) & MADC32_OUTPUT_FORMAT_MASK;
}

void madc32_setAdcOverride_db(s_madc32 * s) { madc32_setAdcOverride(s, s->adcOverride); }

void madc32_setAdcOverride(s_madc32 * s, uint16_t over)
{
	SET16(s->m.md->vmeBase, MADC32_ADC_OVERRIDE, over & MADC32_ADC_RESOLUTION_MASK);
}

uint16_t madc32_getAdcOverride(s_madc32 * s)
{
	return GET16(s->m.md->vmeBase, MADC32_ADC_OVERRIDE) & MADC32_ADC_RESOLUTION_MASK;
}

void madc32_setSlidingScaleOff_db(s_madc32 * s) { madc32_setSlidingScaleOff(s, s->slidingScaleOff); }

void madc32_setSlidingScaleOff(s_madc32 * s, bool_t flag)
{
	uint16_t f = flag ? 1 : 0;
	SET16(s->m.md->vmeBase, MADC32_SLIDING_SCALE_OFF, f);
}

bool_t madc32_getSlidingScaleOff(s_madc32 * s)
{
	uint16_t f = GET16(s->m.md->vmeBase, MADC32_SLIDING_SCALE_OFF) & MADC32_SLIDING_SCALE_OFF_MASK;
	return (f != 0) ? TRUE : FALSE;
}

void madc32_setSkipOutOfRange_db(s_madc32 * s) { madc32_setSkipOutOfRange(s, s->skipOutOfRange); }

void madc32_setSkipOutOfRange(s_madc32 * s, bool_t flag)
{
	uint16_t f = flag ? 1 : 0;
	SET16(s->m.md->vmeBase, MADC32_SKIP_OUT_OF_RANGE, f);
}

bool_t madc32_getSkipOutOfRange(s_madc32 * s)
{
	uint16_t f = GET16(s->m.md->vmeBase, MADC32_SKIP_OUT_OF_RANGE) & MADC32_SKIP_OUT_OF_RANGE_MASK;
	return (f != 0) ? TRUE : FALSE;
}

void madc32_setIgnoreThresholds_db(s_madc32 * s) { madc32_setIgnoreThresholds(s, s->ignoreThresh); }

void madc32_setIgnoreThresholds(s_madc32 * s, bool_t flag)
{
	uint16_t f = flag ? 1 : 0;
	SET16(s->m.md->vmeBase, MADC32_IGNORE_THRESHOLDS, f);
}

bool_t madc32_getIgnoreThresholds(s_madc32 * s)
{
	uint16_t f = GET16(s->m.md->vmeBase, MADC32_IGNORE_THRESHOLDS) & MADC32_IGNORE_THRESH_MASK;
	return (f != 0) ? TRUE : FALSE;
}

void madc32_setHoldDelay_db(s_madc32 * s, uint16_t gg) { madc32_setHoldDelay(s, gg, s->ggHoldDelay[gg]); }

void madc32_setHoldDelay(s_madc32 * s, uint16_t gg, uint16_t delay)
{
	int offs;
	switch (gg) {
		case 0: offs = MADC32_GG_HOLD_DELAY_0; break;
		case 1: offs = MADC32_GG_HOLD_DELAY_1; break;
		default: return;
	}
	SET16(s->m.md->vmeBase, offs, delay & MADC32_GG_HOLD_DELAY_MASK);
}

uint16_t madc32_getHoldDelay(s_madc32 * s, uint16_t gg)
{
	int offs;
	switch (gg) {
		case 0: offs = MADC32_GG_HOLD_DELAY_0; break;
		case 1: offs = MADC32_GG_HOLD_DELAY_1; break;
		default: return 0;
	}
	return GET16(s->m.md->vmeBase, offs) & MADC32_GG_HOLD_DELAY_MASK;
}

void madc32_setHoldWidth_db(s_madc32 * s, uint16_t gg) { madc32_setHoldWidth(s, gg, s->ggHoldWidth[gg]); }

void madc32_setHoldWidth(s_madc32 * s, uint16_t gg, uint16_t width)
{
	int offs;
	switch (gg) {
		case 0: offs = MADC32_GG_HOLD_WIDTH_0; break;
		case 1: offs = MADC32_GG_HOLD_WIDTH_1; break;
		default: return;
	}
	SET16(s->m.md->vmeBase, offs, width & MADC32_GG_HOLD_WIDTH_MASK);
}

uint16_t madc32_getHoldWidth(s_madc32 * s, uint16_t gg)
{
	int offs;
	switch (gg) {
		case 0: offs = MADC32_GG_HOLD_WIDTH_0; break;
		case 1: offs = MADC32_GG_HOLD_WIDTH_1; break;
		default: return 0;
	}
	return GET16(s->m.md->vmeBase, offs) & MADC32_GG_HOLD_WIDTH_MASK;
}

void madc32_useGG_db(s_madc32 * s) { madc32_useGG(s, s->useGG); }

void madc32_useGG(s_madc32 * s, uint16_t gg)
{
	SET16(s->m.md->vmeBase, MADC32_USE_GATE_GENERATOR, gg & MADC32_USE_GATE_GENERATOR_MASK);
}

uint16_t madc32_getGGUsed(s_madc32 * s)
{
	return GET16(s->m.md->vmeBase, MADC32_USE_GATE_GENERATOR) & MADC32_USE_GATE_GENERATOR_MASK;
}

void madc32_setInputRange_db(s_madc32 * s) { madc32_setInputRange(s, s->inputRange); }

void madc32_setInputRange(s_madc32 * s, uint16_t range)
{
	SET16(s->m.md->vmeBase, MADC32_INPUT_RANGE, range & MADC32_INPUT_RANGE_MASK);
}

uint16_t madc32_getInputRange(s_madc32 * s)
{
	return GET16(s->m.md->vmeBase, MADC32_INPUT_RANGE) & MADC32_INPUT_RANGE_MASK;
}

void madc32_setEclTerm_db(s_madc32 * s) { madc32_setEclTerm(s, s->eclTerm); }

void madc32_setEclTerm(s_madc32 * s, uint16_t term)
{
	SET16(s->m.md->vmeBase, MADC32_ECL_TERMINATORS, term & MADC32_ECL_TERMINATORS_MASK);
}

uint16_t madc32_getEclTerm(s_madc32 * s)
{
	return GET16(s->m.md->vmeBase, MADC32_ECL_TERMINATORS) & MADC32_ECL_TERMINATORS_MASK;
}

void madc32_setEclG1Osc_db(s_madc32 * s) { madc32_setEclG1Osc(s, s->eclG1Osc); }

void madc32_setEclG1Osc(s_madc32 * s, uint16_t go)
{
	SET16(s->m.md->vmeBase, MADC32_ECL_G1_OSC, go & MADC32_ECL_G1_OSC_MASK);
}

uint16_t madc32_getEclG1Osc(s_madc32 * s)
{
	return GET16(s->m.md->vmeBase, MADC32_ECL_G1_OSC) & MADC32_ECL_G1_OSC_MASK;
}

void madc32_setEclFclRts_db(s_madc32 * s) { madc32_setEclFclRts(s, s->eclFclRts); }

void madc32_setEclFclRts(s_madc32 * s, uint16_t fr)
{
	SET16(s->m.md->vmeBase, MADC32_ECL_FCL_RTS, fr & MADC32_ECL_FCL_RTS_MASK);
}

uint16_t madc32_getEclFclRts(s_madc32 * s)
{
	return GET16(s->m.md->vmeBase, MADC32_ECL_FCL_RTS) & MADC32_ECL_FCL_RTS_MASK;
}

void madc32_setNimG1Osc_db(s_madc32 * s) { madc32_setNimG1Osc(s, s->nimG1Osc); }

void madc32_setNimG1Osc(s_madc32 * s, uint16_t go)
{
	SET16(s->m.md->vmeBase, MADC32_NIM_G1_OSC, go & MADC32_NIM_G1_OSC_MASK);
}

uint16_t madc32_getNimG1Osc(s_madc32 * s)
{
	return GET16(s->m.md->vmeBase, MADC32_NIM_G1_OSC) & MADC32_NIM_G1_OSC_MASK;
}


void madc32_setNimFclRts_db(s_madc32 * s) { madc32_setNimFclRts(s, s->nimFclRts); }

void madc32_setNimFclRts(s_madc32 * s, uint16_t fr)
{
	SET16(s->m.md->vmeBase, MADC32_NIM_FCL_RTS, fr & MADC32_NIM_FCL_RTS_MASK);
}

uint16_t madc32_getNimFclRts(s_madc32 * s)
{
	return GET16(s->m.md->vmeBase, MADC32_NIM_FCL_RTS) & MADC32_NIM_FCL_RTS_MASK;
}

void madc32_setNimBusy_db(s_madc32 * s) { madc32_setNimBusy(s, s->nimBusy); }

void madc32_setNimBusy(s_madc32 * s, uint16_t busy)
{
	SET16(s->m.md->vmeBase, MADC32_NIM_BUSY, busy & MADC32_NIM_BUSY_MASK);
}

uint16_t madc32_getNimBusy(s_madc32 * s)
{
	return GET16(s->m.md->vmeBase, MADC32_NIM_BUSY) & MADC32_NIM_BUSY_MASK;
}

void madc32_setPulserStatus_db(s_madc32 * s) { madc32_setPulserStatus(s, s->pulserStatus); }

void madc32_setPulserStatus(s_madc32 * s, uint16_t mode)
{
	SET16(s->m.md->vmeBase, MADC32_PULSER_STATUS, mode & MADC32_PULSER_STATUS_MASK);
}

uint16_t madc32_getPulserStatus(s_madc32 * s)
{
	return GET16(s->m.md->vmeBase, MADC32_PULSER_STATUS) & MADC32_PULSER_STATUS_MASK;
}

bool_t madc32_fillStruct(s_madc32 * s, char * file)
{
	char res[256];
	char mnUC[256];
	const char * sp;
	int i;

	s_mesy * m = &s->m;
	
	if (root_env_read(file) < 0) {
		sprintf(msg, "[%sfill_struct] %s: Error reading file %s", m->mpref, m->moduleName, file);
		f_ut_send_msg(s->m.prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		return FALSE;
	}

	sprintf(msg, "[%sfill_struct] %s: Reading settings from file %s", m->mpref, m->moduleName, file);
	f_ut_send_msg(s->m.prefix, msg, ERR__MSG_INFO, MASK__PRTT);

	m->verbose = root_env_getval_b("MADC32.VerboseMode", FALSE);

	m->dumpRegsOnInit = root_env_getval_b("MADC32.DumpRegisters", FALSE);

	sp = root_env_getval_s("MADC32.ModuleName", "");
	if (strcmp(m->moduleName, "madc32") != 0 && strcmp(sp, m->moduleName) != 0) {
		sprintf(msg, "[%sfill_struct] %s: File %s contains wrong module name - %s", m->mpref, m->moduleName, file, sp);
		f_ut_send_msg(s->m.prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		return FALSE;
	}
	strcpy(mnUC, sp);
	mnUC[0] = toupper(mnUC[0]);

	sprintf(res, "MADC32.%s.BlockXfer", mnUC);
	m->blockXfer = root_env_getval_b(res, FALSE);

	sprintf(res, "MADC32.%s.RepairRawData", mnUC);
	m->repairRawData = root_env_getval_b(res, FALSE);

	sprintf(res, "MADC32.%s.ReportReadErrors", mnUC);
	m->reportReadErrors = root_env_getval_i(res, 0);

	for (i = 0; i < MADC_NOF_CHANNELS; i++) {
		sprintf(res, "MADC32.%s.Thresh.%d", mnUC, i);
		s->threshold[i] = root_env_getval_i(res, MADC32_THRESHOLD_DEFAULT);
	}

	sprintf(res, "MADC32.%s.AddrSource", mnUC);
	m->addrSource = root_env_getval_i(res, MESY_ADDR_SOURCE_DEFAULT);

	if (m->addrSource == MESY_ADDR_SOURCE_REG) {
		sprintf(res, "MADC32.%s.AddrReg", mnUC);
		m->addrReg = root_env_getval_i(res, 0);
		if (m->addrReg == 0) {
			sprintf(msg, "[%sfill_struct] %s: vme addr missing (addr source = reg)", m->mpref, m->moduleName);
			f_ut_send_msg(s->m.prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		}
	}

	sprintf(res, "MADC32.%s.MCSTSignature", mnUC);
	m->mcstSignature = root_env_getval_x(res, 0x0);
	sprintf(res, "MADC32.%s.MCSTMaster", mnUC);
	m->mcstMaster = root_env_getval_b(res, FALSE);
	sprintf(res, "MADC32.%s.CBLTSignature", mnUC);
	m->cbltSignature = root_env_getval_x(res, 0x0);
	sprintf(res, "MADC32.%s.FirstInCbltChain", mnUC);
	m->firstInCbltChain = root_env_getval_b(res, FALSE);
	sprintf(res, "MADC32.%s.LastInCbltChain", mnUC);
	m->lastInCbltChain = root_env_getval_b(res, FALSE);

	sprintf(res, "MADC32.%s.ModuleId", mnUC);
	m->moduleId = root_env_getval_i(res, MESY_MODULE_ID_DEFAULT);

	sprintf(res, "MADC32.%s.DataWidth", mnUC);
	m->dataWidth = root_env_getval_i(res, MESY_DATA_LENGTH_FORMAT_DEFAULT);

	sprintf(res, "MADC32.%s.MultiEvent", mnUC);
	m->multiEvent = root_env_getval_i(res, MESY_MULTI_EVENT_DEFAULT);

	if ((m->multiEvent & MESY_MULTI_EVENT_BERR) == 0) {
		sprintf(msg, "[%sfill_struct] %s: Readout with BERR enabled", m->mpref, m->moduleName);
	} else {
		sprintf(msg, "[%sfill_struct] %s: Readout with EOB enabled", m->mpref, m->moduleName);
	}
	f_ut_send_msg(m->prefix, msg, ERR__MSG_INFO, MASK__PRTT);

	sprintf(res, "MADC32.%s.XferData", mnUC);
	m->xferData = root_env_getval_i(res, MESY_MAX_XFER_DATA_DEFAULT);

	sprintf(res, "MADC32.%s.MarkingType", mnUC);
	m->markingType = root_env_getval_i(res, MESY_MARKING_TYPE_DEFAULT);

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
	m->ctraTsSource = root_env_getval_i(res, MESY_CTRA_TS_SOURCE_DEFAULT);

	sprintf(res, "MADC32.%s.TsDivisor", mnUC);
	m->ctraTsDivisor = root_env_getval_i(res, MESY_CTRA_TS_DIVISOR_DEFAULT);

	return TRUE;
}

void madc32_loadFromDb(s_madc32 * s, uint32_t chnPattern)
{
	int ch;
	int gg;
	uint32_t bit;

	s_mesy * m = &s->m;
	
	mesy_setAddrReg_db(m);
	mesy_setMcstCblt_db(m);
	mesy_setModuleId_db(m);
	mesy_setDataWidth_db(m);
	mesy_setMultiEvent_db(m);
	mesy_setMarkingType_db(m);
	mesy_setXferData_db(m);
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
	mesy_setTsSource_db(m);
	mesy_setTsDivisor_db(m);

	bit = 1;
	for (ch = 0; ch < MADC_NOF_CHANNELS; ch++) {
		if (chnPattern & bit) madc32_setThreshold_db(s, ch); else madc32_setThreshold(s, ch, MESY_D_CHAN_INACTIVE);
		bit <<= 1;
	}
}


bool_t madc32_dumpRegisters(s_madc32 * s, char * file)
{
	FILE * f;

	int ch;
	int gg;
	bool_t mcstOrCblt, flag;

	s_mesy * m = &s->m;
	
	if (!m->dumpRegsOnInit) return(TRUE);

	f = fopen(file, "w");
	if (f == NULL) {
		sprintf(msg, "[%sdumpRegisters] %s: Error writing file %s", m->mpref, m->moduleName, file);
		f_ut_send_msg(s->m.prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		return FALSE;
	}

	sprintf(msg, "[%sdumpRegisters] %s: Dumping settings to file %s", m->mpref, m->moduleName, file);
	f_ut_send_msg(s->m.prefix, msg, ERR__MSG_INFO, MASK__PRTT);

	fprintf(f, "Thresholds [0x4000]:\n");
	for (ch = 0; ch < MADC_NOF_CHANNELS; ch++) fprintf(f, "   %2d: %d\n", ch, madc32_getThreshold(s, ch));

	mcstOrCblt = FALSE;
	if (flag = mesy_mcstIsEnabled(m))
					fprintf(f, "MCST signature [0x6024]   : %#x\n", mesy_getMcstSignature(m));
	else			fprintf(f, "MCST                      : disabled\n");
	mcstOrCblt |= flag;
	if (flag = mesy_cbltIsEnabled(m))
					fprintf(f, "CBLT signature [0x6022]   : %#x\n", mesy_getCbltSignature(m));
	else				fprintf(f, "CBLT                      : disabled\n");
	mcstOrCblt |= flag;
	if (mcstOrCblt) {
		if (mesy_isFirstInCbltChain(m)) fprintf(f, "CBLT chain                : first module in chain\n");
		else if (mesy_isLastInCbltChain(m)) fprintf(f, "CBLT chain                : last module in chain\n");
		else fprintf(f, "CBLT chain        : module in the middle\n");
	}
	fprintf(f, "Module ID [0x6004]        : %d\n", mesy_getModuleId(m));
	fprintf(f, "Data width [0x6032]       : %d\n", mesy_getDataWidth(m));
	fprintf(f, "Multi event [0x6036]      : %d\n", mesy_getMultiEvent(m));
	fprintf(f, "Max xfer data wc [0x601A] : %d\n", mesy_getXferData(m));
	fprintf(f, "Marking type [0x6038]     : %d\n", mesy_getMarkingType(m));
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
	fprintf(f, "Timestamp source [0x6096] : %#x\n", mesy_getTsSource(m));
	fprintf(f, "Timestamp divisor [0x6098]: %d\n", mesy_getTsDivisor(m));
	fclose(f);
}

void madc32_printDb(s_madc32 * s)
{
	int ch;
	int gg;

	s_mesy * m = &s->m;
	
	printf("Thresholds:\n");
	for (ch = 0; ch < MADC_NOF_CHANNELS; ch++) printf("   %2d: %d\n", ch, s->threshold[ch]);

	printf("Addr reg          : %#x\n", m->addrReg);

	if (m->mcstSignature != 0) {
		printf("MCST signature    : %#x\n", m->mcstSignature);
		if (m->mcstMaster != 0) printf("MCST master       : yes\n");
	} else {
		printf("MCST              : disabled\n");
	}
	if (m->cbltSignature != 0) {
		printf("CBLT signature    : %#x\n", m->cbltSignature);
		if (m->firstInCbltChain) printf("CBLT chain        : first module in chain\n");
		else if (m->lastInCbltChain) printf("CBLT chain        : last module in chain\n");
		else printf("CBLT chain        : module in the middle\n");
	} else {
		printf("CBLT              : disabled\n");
	}
	printf("Module ID         : %d\n", m->moduleId);
	printf("Data width        : %d\n", m->dataWidth);
	printf("Multi event       : %d\n", m->multiEvent);
	printf("Max xfer data wc  : %d\n", m->xferData);
	printf("Marking type      : %d\n", m->markingType);
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
	printf("Timestamp source  : %#x\n", m->ctraTsSource);
	printf("Timestamp divisor : %d\n", m->ctraTsDivisor);
}


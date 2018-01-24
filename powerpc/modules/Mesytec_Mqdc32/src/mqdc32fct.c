/*_______________________________________________________[C LIBRARY FUNCTIONS]
//////////////////////////////////////////////////////////////////////////////
//! \file			mqdc32fct.c
//! \brief			Code for module MQDC32
//! \details		Implements functions to handle modules of type Mesytec MQDC32
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

#include "mqdc32.h"
#include "mqdc32_database.h"
#include "mqdc32_protos.h"

#include "root_env.h"
#include "mapping_database.h"
#include "bmaErrlist.h"

#include "err_mask_def.h"
#include "errnum_def.h"

char msg[256];

uint16_t mqdc32_getThreshold(s_mqdc32 * s, uint16_t channel)
{
	uint16_t thresh = 0;
	if (channel < MQDC_NOF_CHANNELS) thresh = GET16(s->m.md->vmeBase, MQDC32_THRESHOLD + sizeof(uint16_t) * channel) & MQDC32_THRESHOLD_MASK;
	return thresh;
}

void mqdc32_setThreshold_db(s_mqdc32 * s, uint16_t channel)
{
	if (channel < MQDC_NOF_CHANNELS) mqdc32_setThreshold(s, channel, s->threshold[channel]);
}

void mqdc32_setThreshold(s_mqdc32 * s, uint16_t channel, uint16_t thresh)
{
	if (channel < MQDC_NOF_CHANNELS) SET16(s->m.md->vmeBase, MQDC32_THRESHOLD + sizeof(uint16_t) * channel, thresh & MQDC32_THRESHOLD_MASK);
}

void mqdc32_setBankOperation_db(s_mqdc32 * s) { mqdc32_setBankOperation(s, s->bankOperation); }

void mqdc32_setBankOperation(s_mqdc32 * s, uint16_t oper)
{
	SET16(s->m.md->vmeBase, MQDC32_BANK_OPERATION, oper & MQDC32_BANK_OPERATION_MASK);
}

uint16_t mqdc32_getBankOperation(s_mqdc32 * s)
{
	return GET16(s->m.md->vmeBase, MQDC32_BANK_OPERATION) & MQDC32_BANK_OPERATION_MASK;
}

void mqdc32_setAdcResolution_db(s_mqdc32 * s) { mqdc32_setAdcResolution(s, s->adcResolution); }

void mqdc32_setAdcResolution(s_mqdc32 * s, uint16_t res)
{
	SET16(s->m.md->vmeBase, MQDC32_ADC_RESOLUTION, res & MQDC32_ADC_RESOLUTION_MASK);
}

uint16_t mqdc32_getAdcResolution(s_mqdc32 * s)
{
	return GET16(s->m.md->vmeBase, MQDC32_ADC_RESOLUTION) & MQDC32_ADC_RESOLUTION_MASK;
}

void mqdc32_setBankOffset_db(s_mqdc32 * s, uint16_t bank) { mqdc32_setBankOffset(s, bank, s->bankOffset[bank]); }

void mqdc32_setBankOffset(s_mqdc32 * s, uint16_t bank, uint16_t offset)
{
	int addr;
	switch (bank) {
		case 0: addr = MQDC32_OFFSET_BANK_0; break;
		case 1: addr = MQDC32_OFFSET_BANK_1; break;
		default: return;
	}
	SET16(s->m.md->vmeBase, addr, offset & MQDC32_BANK_OFFSET_MASK);
}

uint16_t mqdc32_getBankOffset(s_mqdc32 * s, uint16_t bank)
{
	int addr;
	switch (bank) {
		case 0: addr = MQDC32_OFFSET_BANK_0; break;
		case 1: addr = MQDC32_OFFSET_BANK_1; break;
		default: return;
	}
	return GET16(s->m.md->vmeBase, addr) & MQDC32_BANK_OFFSET_MASK;
}

void mqdc32_setGateLimit_db(s_mqdc32 * s, uint16_t bank) { mqdc32_setGateLimit(s, bank, s->gateLimit[bank]); }

void mqdc32_setGateLimit(s_mqdc32 * s, uint16_t bank, uint16_t limit)
{
	int addr;
	switch (bank) {
		case 0: addr = MQDC32_GATE_LIMIT_BANK_0; break;
		case 1: addr = MQDC32_GATE_LIMIT_BANK_1; break;
		default: return;
	}
	SET16(s->m.md->vmeBase, addr, limit & MQDC32_GATE_LIMIT_MASK);
}

uint16_t mqdc32_getGateLimit(s_mqdc32 * s, uint16_t bank)
{
	int addr;
	switch (bank) {
		case 0: addr = MQDC32_GATE_LIMIT_BANK_0; break;
		case 1: addr = MQDC32_GATE_LIMIT_BANK_1; break;
		default: return;
	}
	return GET16(s->m.md->vmeBase, addr) & MQDC32_GATE_LIMIT_MASK;
}

void mqdc32_setSlidingScaleOff_db(s_mqdc32 * s) { mqdc32_setSlidingScaleOff(s, s->slidingScaleOff); }

void mqdc32_setSlidingScaleOff(s_mqdc32 * s, bool_t flag)
{
	uint16_t f = flag ? 1 : 0;
	SET16(s->m.md->vmeBase, MQDC32_SLIDING_SCALE_OFF, f);
}

bool_t mqdc32_getSlidingScaleOff(s_mqdc32 * s)
{
	uint16_t f = GET16(s->m.md->vmeBase, MQDC32_SLIDING_SCALE_OFF) & MQDC32_SLIDING_SCALE_OFF_MASK;
	return (f != 0) ? TRUE : FALSE;
}

void mqdc32_setSkipOutOfRange_db(s_mqdc32 * s) { mqdc32_setSkipOutOfRange(s, s->skipOutOfRange); }

void mqdc32_setSkipOutOfRange(s_mqdc32 * s, bool_t flag)
{
	uint16_t f = flag ? 1 : 0;
	SET16(s->m.md->vmeBase, MQDC32_SKIP_OUT_OF_RANGE, f);
}

bool_t mqdc32_getSkipOutOfRange(s_mqdc32 * s)
{
	uint16_t f = GET16(s->m.md->vmeBase, MQDC32_SKIP_OUT_OF_RANGE) & MQDC32_SKIP_OUT_OF_RANGE_MASK;
	return (f != 0) ? TRUE : FALSE;
}

void mqdc32_setIgnoreThresholds_db(s_mqdc32 * s) { mqdc32_setIgnoreThresholds(s, s->ignoreThresh); }

void mqdc32_setIgnoreThresholds(s_mqdc32 * s, bool_t flag)
{
	uint16_t f = flag ? 1 : 0;
	SET16(s->m.md->vmeBase, MQDC32_IGNORE_THRESHOLDS, f);
}

bool_t mqdc32_getIgnoreThresholds(s_mqdc32 * s)
{
	uint16_t f = GET16(s->m.md->vmeBase, MQDC32_IGNORE_THRESHOLDS) & MQDC32_IGNORE_THRESH_MASK;
	return (f != 0) ? TRUE : FALSE;
}

void mqdc32_setInputCoupling_db(s_mqdc32 * s) { mqdc32_setInputCoupling(s, s->inputCoupling); }

void mqdc32_setInputCoupling(s_mqdc32 * s, uint16_t coupling)
{
	SET16(s->m.md->vmeBase, MQDC32_INPUT_COUPLING, coupling & MQDC32_INPUT_COUPLING_MASK);
}

uint16_t mqdc32_getInputCoupling(s_mqdc32 * s)
{
	return GET16(s->m.md->vmeBase, MQDC32_INPUT_COUPLING) & MQDC32_INPUT_COUPLING_MASK;
}

void mqdc32_setEclTerm_db(s_mqdc32 * s) { mqdc32_setEclTerm(s, s->eclTerm); }

void mqdc32_setEclTerm(s_mqdc32 * s, uint16_t term)
{
	SET16(s->m.md->vmeBase, MQDC32_ECL_TERMINATORS, term & MQDC32_ECL_TERMINATORS_MASK);
}

uint16_t mqdc32_getEclTerm(s_mqdc32 * s)
{
	return GET16(s->m.md->vmeBase, MQDC32_ECL_TERMINATORS) & MQDC32_ECL_TERMINATORS_MASK;
}

void mqdc32_setEclG1Osc_db(s_mqdc32 * s) { mqdc32_setEclG1Osc(s, s->eclG1Osc); }

void mqdc32_setEclG1Osc(s_mqdc32 * s, uint16_t go)
{
	SET16(s->m.md->vmeBase, MQDC32_ECL_G1_OSC, go & MQDC32_ECL_G1_OSC_MASK);
}

uint16_t mqdc32_getEclG1Osc(s_mqdc32 * s)
{
	return GET16(s->m.md->vmeBase, MQDC32_ECL_G1_OSC) & MQDC32_ECL_G1_OSC_MASK;
}

void mqdc32_setEclFclRts_db(s_mqdc32 * s) { mqdc32_setEclFclRts(s, s->eclFclRts); }

void mqdc32_setEclFclRts(s_mqdc32 * s, uint16_t fr)
{
	SET16(s->m.md->vmeBase, MQDC32_ECL_FCL_RTS, fr & MQDC32_ECL_FCL_RTS_MASK);
}

uint16_t mqdc32_getEclFclRts(s_mqdc32 * s)
{
	return GET16(s->m.md->vmeBase, MQDC32_ECL_FCL_RTS) & MQDC32_ECL_FCL_RTS_MASK;
}

void mqdc32_setGateSelect_db(s_mqdc32 * s) { mqdc32_setGateSelect(s, s->gateSelect); }

void mqdc32_setGateSelect(s_mqdc32 * s, uint16_t select)
{
	SET16(s->m.md->vmeBase, MQDC32_GATE_SELECT, select & MQDC32_GATE_SELECT_MASK);
}

uint16_t mqdc32_getGateSelect(s_mqdc32 * s)
{
	return GET16(s->m.md->vmeBase, MQDC32_GATE_SELECT) & MQDC32_GATE_SELECT_MASK;
}

void mqdc32_setNimG1Osc_db(s_mqdc32 * s) { mqdc32_setNimG1Osc(s, s->nimG1Osc); }

void mqdc32_setNimG1Osc(s_mqdc32 * s, uint16_t go)
{
	SET16(s->m.md->vmeBase, MQDC32_NIM_G1_OSC, go & MQDC32_NIM_G1_OSC_MASK);
}

uint16_t mqdc32_getNimG1Osc(s_mqdc32 * s)
{
	return GET16(s->m.md->vmeBase, MQDC32_NIM_G1_OSC) & MQDC32_NIM_G1_OSC_MASK;
}


void mqdc32_setNimFclRts_db(s_mqdc32 * s) { mqdc32_setNimFclRts(s, s->nimFclRts); }

void mqdc32_setNimFclRts(s_mqdc32 * s, uint16_t fr)
{
	SET16(s->m.md->vmeBase, MQDC32_NIM_FCL_RTS, fr & MQDC32_NIM_FCL_RTS_MASK);
}

uint16_t mqdc32_getNimFclRts(s_mqdc32 * s)
{
	return GET16(s->m.md->vmeBase, MQDC32_NIM_FCL_RTS) & MQDC32_NIM_FCL_RTS_MASK;
}

void mqdc32_setNimBusy_db(s_mqdc32 * s) { mqdc32_setNimBusy(s, s->nimBusy); }

void mqdc32_setNimBusy(s_mqdc32 * s, uint16_t busy)
{
	SET16(s->m.md->vmeBase, MQDC32_NIM_BUSY, busy & MQDC32_NIM_BUSY_MASK);
}

uint16_t mqdc32_getNimBusy(s_mqdc32 * s)
{
	return GET16(s->m.md->vmeBase, MQDC32_NIM_BUSY) & MQDC32_NIM_BUSY_MASK;
}

void mqdc32_setPulserStatus_db(s_mqdc32 * s) { mqdc32_setPulserStatus(s, s->pulserStatus); }

void mqdc32_setPulserStatus(s_mqdc32 * s, uint16_t mode)
{
	SET16(s->m.md->vmeBase, MQDC32_PULSER_STATUS, mode & MQDC32_PULSER_STATUS_MASK);
}

uint16_t mqdc32_getPulserStatus(s_mqdc32 * s)
{
	return GET16(s->m.md->vmeBase, MQDC32_PULSER_STATUS) & MQDC32_PULSER_STATUS_MASK;
}

void mqdc32_setTsSource_db(s_mqdc32 * s) { mqdc32_setTsSource(s, s->ctraTsSource); }

void mqdc32_setTsSource(s_mqdc32 * s, uint16_t source)
{
	SET16(s->m.md->vmeBase, MESY_CTRA_TS_SOURCE, source & MESY_CTRA_TS_SOURCE_MASK);
}

bool_t mqdc32_fillStruct(s_mqdc32 * s, char * file)
{
	char res[256];
	char mnUC[256];
	const char * sp;
	int i;
	s_mesy * m;
	
	m = &s->m;

	if (root_env_read(file) < 0) {
		sprintf(msg, "[%sfill_struct] %s: Error reading file %s", m->mpref, m->moduleName, file);
		f_ut_send_msg(m->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		return FALSE;
	}

	sprintf(msg, "[%sfill_struct] %s: Reading settings from file %s", m->mpref, m->moduleName, file);
	f_ut_send_msg(m->prefix, msg, ERR__MSG_INFO, MASK__PRTT);

	m->verbose = root_env_getval_b("MQDC32.VerboseMode", FALSE);

	m->dumpRegsOnInit = root_env_getval_b("MQDC32.DumpRegisters", FALSE);

	sp = root_env_getval_s("MQDC32.ModuleName", "");
	if (strcmp(m->moduleName, "mqdc32") != 0 && strcmp(sp, m->moduleName) != 0) {
		sprintf(msg, "[%sfill_struct] %s: File %s contains wrong module name - %s", m->mpref, m->moduleName, file, sp);
		f_ut_send_msg(m->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		return FALSE;
	}
	strcpy(mnUC, sp);
	mnUC[0] = toupper(mnUC[0]);

	sprintf(res, "MQDC32.%s.BlockXfer", mnUC);
	m->blockXfer = root_env_getval_b(res, FALSE);

	sprintf(res, "MQDC32.%s.RepairRawData", mnUC);
	m->repairRawData = root_env_getval_b(res, FALSE);

	sprintf(res, "MQDC32.%s.ReportReadErrors", mnUC);
	m->reportReadErrors = root_env_getval_i(res, 0);

	for (i = 0; i < MQDC_NOF_CHANNELS; i++) {
		sprintf(res, "MQDC32.%s.Thresh.%d", mnUC, i);
		s->threshold[i] = root_env_getval_i(res, MQDC32_THRESHOLD_DEFAULT);
	}

	sprintf(res, "MQDC32.%s.AddrSource", mnUC);
	m->addrSource = root_env_getval_i(res, MESY_ADDR_SOURCE_DEFAULT);

	if (m->addrSource == MESY_ADDR_SOURCE_REG) {
		sprintf(res, "MQDC32.%s.AddrReg", mnUC);
		m->addrReg = root_env_getval_i(res, 0);
		if (m->addrReg == 0) {
			sprintf(msg, "[%sfill_struct] %s: vme addr missing (addr source = reg)", m->mpref, m->moduleName);
			f_ut_send_msg(m->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		}
	}

	sprintf(res, "MQDC32.%s.MCSTSignature", mnUC);
	m->mcstSignature = root_env_getval_x(res, 0x0);
	sprintf(res, "MQDC32.%s.MCSTMaster", mnUC);
	m->mcstMaster = root_env_getval_b(res, FALSE);
	sprintf(res, "MQDC32.%s.CBLTSignature", mnUC);
	m->cbltSignature = root_env_getval_x(res, 0x0);
	sprintf(res, "MQDC32.%s.FirstInCbltChain", mnUC);
	m->firstInCbltChain = root_env_getval_b(res, FALSE);
	sprintf(res, "MQDC32.%s.LastInCbltChain", mnUC);
	m->lastInCbltChain = root_env_getval_b(res, FALSE);

	sprintf(res, "MQDC32.%s.ModuleId", mnUC);
	m->moduleId = root_env_getval_i(res, MESY_MODULE_ID_DEFAULT);

	sprintf(res, "MQDC32.%s.DataWidth", mnUC);
	m->dataWidth = root_env_getval_i(res, MESY_DATA_LENGTH_FORMAT_DEFAULT);

	sprintf(res, "MQDC32.%s.MultiEvent", mnUC);
	m->multiEvent = root_env_getval_i(res, MESY_MULTI_EVENT_DEFAULT);
	
	if ((m->multiEvent & MESY_MULTI_EVENT_BERR) == 0) {
		sprintf(msg, "[%sfill_struct] %s: Readout with BERR enabled", m->mpref, m->moduleName);
	} else {
		sprintf(msg, "[%sfill_struct] %s: Readout with EOB enabled", m->mpref, m->moduleName);
	}
	f_ut_send_msg(m->prefix, msg, ERR__MSG_INFO, MASK__PRTT);

	sprintf(res, "MQDC32.%s.XferData", mnUC);
	m->xferData = root_env_getval_i(res, MESY_MAX_XFER_DATA_DEFAULT);

	sprintf(res, "MQDC32.%s.MarkingType", mnUC);
	m->markingType = root_env_getval_i(res, MESY_MARKING_TYPE_DEFAULT);

	sprintf(res, "MQDC32.%s.BankOperation", mnUC);
	s->bankOperation = root_env_getval_i(res, MQDC32_BANK_OPERATION_DEFAULT);

	sprintf(res, "MQDC32.%s.AdcResolution", mnUC);
	s->adcResolution = root_env_getval_i(res, MQDC32_ADC_RESOLUTION_DEFAULT);

	for (i = 0; i <= 1; i++) {
		sprintf(res, "MQDC32.%s.BankOffset.%d", mnUC, i);
		s->bankOffset[i] = root_env_getval_i(res, MQDC32_BANK_OFFSET_DEFAULT);
	}

	for (i = 0; i <= 1; i++) {
		sprintf(res, "MQDC32.%s.GateLimit.%d", mnUC, i);
		s->gateLimit[i] = root_env_getval_i(res, MQDC32_GATE_LIMIT_DEFAULT);
	}

	sprintf(res, "MQDC32.%s.SlidingScaleOff", mnUC);
	s->slidingScaleOff = root_env_getval_b(res, FALSE);

	sprintf(res, "MQDC32.%s.SkipOutOfRange", mnUC);
	s->skipOutOfRange = root_env_getval_b(res, FALSE);

	sprintf(res, "MQDC32.%s.IgnoreThresh", mnUC);
	s->ignoreThresh = root_env_getval_b(res, FALSE);

	sprintf(res, "MQDC32.%s.InputCoupling", mnUC);
	s->inputCoupling = root_env_getval_i(res, MQDC32_INPUT_COUPLING_DEFAULT);

	sprintf(res, "MQDC32.%s.EclTerm", mnUC);
	s->eclTerm = root_env_getval_i(res, MQDC32_ECL_TERMINATORS_DEFAULT);

	sprintf(res, "MQDC32.%s.EclG1Osc", mnUC);
	s->eclG1Osc = root_env_getval_i(res, MQDC32_ECL_G1_OSC_DEFAULT);

	sprintf(res, "MQDC32.%s.EclFclRts", mnUC);
	s->eclFclRts = root_env_getval_i(res, MQDC32_NIM_FCL_RTS_DEFAULT);

	sprintf(res, "MQDC32.%s.GateSelect", mnUC);
	s->gateSelect = root_env_getval_i(res, MQDC32_GATE_SELECT_DEFAULT);

	sprintf(res, "MQDC32.%s.NimG1Osc", mnUC);
	s->nimG1Osc = root_env_getval_i(res, MQDC32_NIM_G1_OSC_DEFAULT);

	sprintf(res, "MQDC32.%s.NimFclRts", mnUC);
	s->nimFclRts = root_env_getval_i(res, MQDC32_NIM_FCL_RTS_DEFAULT);

	sprintf(res, "MQDC32.%s.NimBusy", mnUC);
	s->nimBusy = root_env_getval_i(res, MQDC32_NIM_BUSY_DEFAULT);

	sprintf(res, "MQDC32.%s.PulserStatus", mnUC);
	s->pulserStatus = root_env_getval_i(res, MQDC32_PULSER_STATUS_DEFAULT);

	sprintf(res, "MQDC32.%s.TsSource", mnUC);
	m->ctraTsSource = root_env_getval_i(res, MESY_CTRA_TS_SOURCE_DEFAULT);

	sprintf(res, "MQDC32.%s.TsDivisor", mnUC);
	m->ctraTsDivisor = root_env_getval_i(res, MESY_CTRA_TS_DIVISOR_DEFAULT);

	return TRUE;
}

void mqdc32_loadFromDb(s_mqdc32 * s, uint32_t chnPattern)
{
	int ch;
	int b;
	uint32_t bit;
	s_mesy * m;
	
	m = &s->m;

	mesy_setAddrReg_db(m);
	mesy_setMcstCblt_db(m);
	mesy_setModuleId_db(m);
	mesy_setDataWidth_db(m);
	mesy_setMultiEvent_db(m);
	mesy_setMarkingType_db(m);
	mesy_setXferData_db(m);
	mqdc32_setBankOperation_db(s);
	mqdc32_setAdcResolution_db(s);
	for (b = 0; b <= 1; b++) {
		mqdc32_setBankOffset_db(s, b);
		mqdc32_setGateLimit_db(s, b);
	}
	mqdc32_setSlidingScaleOff_db(s);
	mqdc32_setSkipOutOfRange_db(s);
	mqdc32_setIgnoreThresholds_db(s);
	mqdc32_setInputCoupling_db(s);
	mqdc32_setEclTerm_db(s);
	mqdc32_setEclG1Osc_db(s);
	mqdc32_setEclFclRts_db(s);
	mqdc32_setGateSelect_db(s);
	mqdc32_setNimG1Osc_db(s);
	mqdc32_setNimFclRts_db(s);
	mqdc32_setNimBusy_db(s);
	mqdc32_setPulserStatus_db(s);
	mesy_setTsSource_db(m);
	mesy_setTsDivisor_db(m);

	bit = 1;
	for (ch = 0; ch < MQDC_NOF_CHANNELS; ch++) {
		if (chnPattern & bit) mqdc32_setThreshold_db(s, ch); else mqdc32_setThreshold(s, ch, MESY_D_CHAN_INACTIVE);
		bit <<= 1;
	}
}


bool_t mqdc32_dumpRegisters(s_mqdc32 * s, char * file)
{
	FILE * f;

	int ch;
	int b;
	bool_t mcstOrCblt, flag;
	s_mesy * m;
	
	m = &s->m;

	if (!m->dumpRegsOnInit) return(TRUE);

	f = fopen(file, "w");
	if (f == NULL) {
		sprintf(msg, "[%sdumpRegisters] %s: Error writing file %s", m->mpref, m->moduleName, file);
		f_ut_send_msg(m->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		return FALSE;
	}

	sprintf(msg, "[%sdumpRegisters] %s: Dumping settings to file %s", m->mpref, m->moduleName, file);
	f_ut_send_msg(m->prefix, msg, ERR__MSG_INFO, MASK__PRTT);

	fprintf(f, "Thresholds:\n");
	for (ch = 0; ch < MQDC_NOF_CHANNELS; ch++) fprintf(f, "   %2d: %d\n", ch, mqdc32_getThreshold(s, ch));

	fprintf(f, "Addr reg          : %#x\n", mesy_getAddrReg(m));
	mcstOrCblt = FALSE;
	if (flag = mesy_mcstIsEnabled(m))
					fprintf(f, "MCST signature [0x6024]   : %#x\n", mesy_getMcstSignature(m));
	else				fprintf(f, "MCST                      : disabled\n");
	mcstOrCblt |= flag;
	if (flag = mesy_cbltIsEnabled(m))
					fprintf(f, "CBLT signature [0x6022]   : %#x\n", mesy_getCbltSignature(m));
	else				fprintf(f, "CBLT                      : disabled\n");
	mcstOrCblt |= flag;
	if (mcstOrCblt) {
		if (mesy_isFirstInCbltChain(m)) fprintf(f, "MCST/CBLT chain           : first module in chain\n");
		else if (mesy_isLastInCbltChain(m)) fprintf(f, "MCST/CBLT chain           : last module in chain\n");
		else fprintf(f, "MCST/CBLT chain   : module in the middle\n");
	}
	fprintf(f, "Module ID         : %d\n", mesy_getModuleId(m));
	fprintf(f, "Data width        : %d\n", mesy_getDataWidth(m));
	fprintf(f, "Multi event       : %d\n", mesy_getMultiEvent(m));
	fprintf(f, "Max xfer data wc  : %d\n", mesy_getXferData(m));
	fprintf(f, "Marking type      : %d\n", mesy_getMarkingType(m));
	fprintf(f, "Bank operation    : %d\n", mqdc32_getBankOperation(s));
	fprintf(f, "Adc resolution    : %d\n", mqdc32_getAdcResolution(s));
	for (b = 0; b <= 1; b++) {
		fprintf(f, "Bank offset %d     : %d\n", b, mqdc32_getBankOffset(s, b));
	}
	fprintf(f, "Sliding scale off : %d\n", mqdc32_getSlidingScaleOff(s));
	fprintf(f, "Skip out of range : %d\n", mqdc32_getSkipOutOfRange(s));
	fprintf(f, "Ignore thresholds : %d\n", mqdc32_getIgnoreThresholds(s));
	for (b = 0; b <= 1; b++) {
		fprintf(f, "Gate limit %d      : %d\n", b, mqdc32_getGateLimit(s, b));
	}
	fprintf(f, "Input coupling    : %d\n", mqdc32_getInputCoupling(s));
	fprintf(f, "Ecl termination   : %#x\n", mqdc32_getEclTerm(s));
	fprintf(f, "Ecl gate or osc   : %d\n", mqdc32_getEclG1Osc(s));
	fprintf(f, "Ecl fcl or reset  : %d\n", mqdc32_getEclFclRts(s));
	fprintf(f, "Gate select       : %d\n", mqdc32_getGateSelect(s));
	fprintf(f, "Nim gate or osc   : %d\n", mqdc32_getNimG1Osc(s));
	fprintf(f, "Nim fcl or reset  : %d\n", mqdc32_getNimFclRts(s));
	fprintf(f, "Nim busy          : %d\n", mqdc32_getNimBusy(s));
	fprintf(f, "Pulser status     : %d\n", mqdc32_getPulserStatus(s));
	fprintf(f, "Timestamp source  : %#x\n", mesy_getTsSource(m));
	fprintf(f, "Timestamp divisor : %d\n", mesy_getTsDivisor(m));
	fclose(f);
}

void mqdc32_printDb(s_mqdc32 * s)
{
	int ch;
	int b;
	s_mesy * m;
	
	m = &s->m;

	printf("Thresholds:\n");
	for (ch = 0; ch < MQDC_NOF_CHANNELS; ch++) printf("   %2d: %d\n", ch, s->threshold[ch]);

	printf("Addr reg          : %#x\n", m->addrReg);
	if (m->mcstSignature != 0)
					printf("MCST signature    : %#x\n", m->mcstSignature);
	else				printf("MCST              : disabled\n");
	if (m->cbltSignature != 0)
					printf("CBLT signature    : %#x\n", m->cbltSignature);
	else				printf("CBLT              : disabled\n");
	if ((m->mcstSignature != 0) || (m->cbltSignature != 0)) {
		if (m->firstInCbltChain) printf("MCST/CBLT chain   : first module in chain\n");
		else if (m->lastInCbltChain) printf("MCST/CBLT chain   : last module in chain\n");
		else printf("MCST/CBLT chain   : module in the middle\n");
	}
	printf("Module ID         : %d\n", m->moduleId);
	printf("Data width        : %d\n", m->dataWidth);
	printf("Multi event       : %d\n", m->multiEvent);
	printf("Max xfer data wc  : %d\n", m->xferData);
	printf("Marking type      : %d\n", m->markingType);
	printf("Bank operation    : %d\n", s->bankOperation);
	printf("Adc resolution    : %d\n", s->adcResolution);
	for (b = 0; b <= 1; b++) {
		printf("Bank offset %d     : %d\n", b, s->bankOffset[b]);
	}
	printf("Sliding scale off : %d\n", s->slidingScaleOff);
	printf("Skip out of range : %d\n", s->skipOutOfRange);
	printf("Ignore thesholds  : %d\n", s->ignoreThresh);
	for (b = 0; b <= 1; b++) {
		printf("Gate limit %d      : %d\n", b, s->gateLimit[b]);
	}
	printf("Input coupling    : %d\n", s->inputCoupling);
	printf("Ecl termination   : %#x\n", s->eclTerm);
	printf("Ecl gate or osc   : %d\n", s->eclG1Osc);
	printf("Ecl fcl or reset  : %d\n", s->eclFclRts);
	printf("Gate select       : %d\n", s->gateSelect);
	printf("Nim gate or osc   : %d\n", s->nimG1Osc);
	printf("Nim fcl or reset  : %d\n", s->nimFclRts);
	printf("Nim busy          : %d\n", s->nimBusy);
	printf("Pulser status     : %d\n", s->pulserStatus);
	printf("Timestamp source  : %#x\n", m->ctraTsSource);
	printf("Timestamp divisor : %d\n", m->ctraTsDivisor);
}


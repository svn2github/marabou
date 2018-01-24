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

#include "mtdc32.h"
#include "mtdc32_database.h"
#include "mtdc32_protos.h"

#include "root_env.h"
#include "mapping_database.h"
#include "bmaErrlist.h"

#include "err_mask_def.h"
#include "errnum_def.h"

char msg[256];

void mtdc32_setBankOperation_db(s_mtdc32 * s) { mtdc32_setBankOperation(s, s->bankOperation); }

void mtdc32_setBankOperation(s_mtdc32 * s, uint16_t oper)
{
	SET16(s->m.md->vmeBase, MTDC32_BANK_OPERATION, oper & MTDC32_BANK_OPERATION_MASK);
}

uint16_t mtdc32_getBankOperation(s_mtdc32 * s)
{
	return GET16(s->m.md->vmeBase, MTDC32_BANK_OPERATION) & MTDC32_BANK_OPERATION_MASK;
}

void mtdc32_setTdcResolution_db(s_mtdc32 * s) { mtdc32_setTdcResolution(s, s->tdcResolution); }

void mtdc32_setTdcResolution(s_mtdc32 * s, uint16_t res)
{
	SET16(s->m.md->vmeBase, MTDC32_TDC_RESOLUTION, res & MTDC32_TDC_RESOLUTION_MASK);
}

uint16_t mtdc32_getTdcResolution(s_mtdc32 * s)
{
	return GET16(s->m.md->vmeBase, MTDC32_TDC_RESOLUTION) & MTDC32_TDC_RESOLUTION_MASK;
}

void mtdc32_setOutputFormat_db(s_mtdc32 * s) { mtdc32_setOutputFormat(s, s->outputFormat); }

void mtdc32_setOutputFormat(s_mtdc32 * s, uint16_t format)
{
	SET16(s->m.md->vmeBase, MTDC32_OUTPUT_FORMAT, format & MTDC32_OUTPUT_FORMAT_MASK);
}

uint16_t mtdc32_getOutputFormat(s_mtdc32 * s)
{
	return GET16(s->m.md->vmeBase, MTDC32_OUTPUT_FORMAT) & MTDC32_OUTPUT_FORMAT_MASK;
}

void mtdc32_setWinStart_db(s_mtdc32 * s, uint16_t bnk) { mtdc32_setWinStart(s, bnk, s->winStart[bnk]); }

void mtdc32_setWinStart(s_mtdc32 * s, uint16_t bnk, int16_t start)
{
	int addr;
	switch (bnk) {
		case 0: addr = MTDC32_WIN_START_0; break;
		case 1: addr = MTDC32_WIN_START_1; break;
		default: return;
	}
	SET16(s->m.md->vmeBase, addr, (start + 16384));
}

int16_t mtdc32_getWinStart(s_mtdc32 * s, uint16_t bnk)
{
	int addr;
	switch (bnk) {
		case 0: addr = MTDC32_WIN_START_0; break;
		case 1: addr = MTDC32_WIN_START_1; break;
		default: return 0;
	}
	return GET16(s->m.md->vmeBase, addr) - 16384;
}

void mtdc32_setWinWidth_db(s_mtdc32 * s, uint16_t bnk) { mtdc32_setWinWidth(s, bnk, s->winWidth[bnk]); }

void mtdc32_setWinWidth(s_mtdc32 * s, uint16_t bnk, int16_t width)
{
	int addr;
	switch (bnk) {
		case 0: addr = MTDC32_WIN_WIDTH_0; break;
		case 1: addr = MTDC32_WIN_WIDTH_1; break;
		default: return;
	}
	SET16(s->m.md->vmeBase, addr, width);
}

int16_t mtdc32_getWinWidth(s_mtdc32 * s, uint16_t bnk)
{
	int addr;
	switch (bnk) {
		case 0: addr = MTDC32_WIN_WIDTH_0; break;
		case 1: addr = MTDC32_WIN_WIDTH_1; break;
		default: return 0;
	}
	return GET16(s->m.md->vmeBase, addr);
}

void mtdc32_setTrigSource_db(s_mtdc32 * s, uint16_t bnk) { mtdc32_setTrigSource(s, bnk, s->trigSrcTrig[bnk], s->trigSrcChan[bnk], s->trigSrcBank[bnk]); }

void mtdc32_setTrigSource(s_mtdc32 * s, uint16_t bnk, uint16_t trig, uint16_t chan, uint16_t bank)
{
	uint16_t trigSource;
	int addr;
	trigSource = 0;
	if (trig > 0) {
		trigSource = trig & MTDC32_TRIG_SRC_TRIG_MASK;
	} else if (chan >= 0) {
		trigSource = chan & (MTDC32_TRIG_SRC_CHAN_MASK) << 2;
	} else if (bank > 0) {
		trigSource = bank & (MTDC32_TRIG_SRC_BANK_MASK) << 8;
	}
	switch (bnk) {
		case 0: addr = MTDC32_TRIG_SOURCE_0; break;
		case 1: addr = MTDC32_TRIG_SOURCE_1; break;
		default: return;
	}
	SET16(s->m.md->vmeBase, addr, trigSource);
}

uint16_t mtdc32_getTrigSource(s_mtdc32 * s, uint16_t bnk)
{
	int addr;
	switch (bnk) {
		case 0: addr = MTDC32_TRIG_SOURCE_0; break;
		case 1: addr = MTDC32_TRIG_SOURCE_1; break;
		default: return;
	}
	return GET16(s->m.md->vmeBase, addr);
}

uint16_t mtdc32_getTrigSrcTrig(s_mtdc32 * s, uint16_t bnk)
{
	int addr;
	uint16_t trigSource;
	switch (bnk) {
		case 0: addr = MTDC32_TRIG_SOURCE_0; break;
		case 1: addr = MTDC32_TRIG_SOURCE_1; break;
		default: return;
	}
	trigSource = GET16(s->m.md->vmeBase, addr);
	return (trigSource & MTDC32_TRIG_SRC_TRIG_MASK);
}

uint16_t mtdc32_getTrigSrcChan(s_mtdc32 * s, uint16_t bnk)
{
	int addr;
	uint16_t trigSource;
	switch (bnk) {
		case 0: addr = MTDC32_TRIG_SOURCE_0; break;
		case 1: addr = MTDC32_TRIG_SOURCE_1; break;
		default: return;
	}
	trigSource = GET16(s->m.md->vmeBase, addr);
	return ((trigSource >> 2) & MTDC32_TRIG_SRC_CHAN_MASK);
}

uint16_t mtdc32_getTrigSrcBank(s_mtdc32 * s, uint16_t bnk)
{
	int addr;
	uint16_t trigSource;
	switch (bnk) {
		case 0: addr = MTDC32_TRIG_SOURCE_0; break;
		case 1: addr = MTDC32_TRIG_SOURCE_1; break;
		default: return;
	}
	trigSource = GET16(s->m.md->vmeBase, addr);
	return ((trigSource >> 8) & MTDC32_TRIG_SRC_BANK_MASK);
}

void mtdc32_setFirstHit_db(s_mtdc32 * s) { mtdc32_setFirstHit(s, s->firstHit); }

void mtdc32_setFirstHit(s_mtdc32 * s, uint16_t fhit)
{
	SET16(s->m.md->vmeBase, MTDC32_FIRST_HIT, fhit & MTDC32_FIRST_HIT_MASK);
}

uint16_t mtdc32_getFirstHit(s_mtdc32 * s)
{
	return GET16(s->m.md->vmeBase, MTDC32_FIRST_HIT) & MTDC32_FIRST_HIT_MASK;
}

void mtdc32_setNegEdge_db(s_mtdc32 * s) { mtdc32_setNegEdge(s, s->negEdge); }

void mtdc32_setNegEdge(s_mtdc32 * s, uint16_t edge)
{
	SET16(s->m.md->vmeBase, MTDC32_NEG_EDGE, edge & MTDC32_NEG_EDGE_MASK);
}

uint16_t mtdc32_getNegEdge(s_mtdc32 * s)
{
	return GET16(s->m.md->vmeBase, MTDC32_NEG_EDGE) & MTDC32_NEG_EDGE_MASK;
}

void mtdc32_setEclTerm_db(s_mtdc32 * s) { mtdc32_setEclTerm(s, s->eclTerm); }

void mtdc32_setEclTerm(s_mtdc32 * s, uint16_t term)
{
	SET16(s->m.md->vmeBase, MTDC32_ECL_TERMINATORS, term & MTDC32_ECL_TERM_MASK);
}

uint16_t mtdc32_getEclTerm(s_mtdc32 * s)
{
	return GET16(s->m.md->vmeBase, MTDC32_ECL_TERMINATORS) & MTDC32_ECL_TERM_MASK;
}

void mtdc32_setEclT1Osc_db(s_mtdc32 * s) { mtdc32_setEclT1Osc(s, s->eclT1Osc); }

void mtdc32_setEclT1Osc(s_mtdc32 * s, uint16_t go)
{
	SET16(s->m.md->vmeBase, MTDC32_ECL_TRIG1_OSC, go & MTDC32_ECL_TRIG1_OSC_MASK);
}

uint16_t mtdc32_getEclT1Osc(s_mtdc32 * s)
{
	return GET16(s->m.md->vmeBase, MTDC32_ECL_TRIG1_OSC) & MTDC32_ECL_TRIG1_OSC_MASK;
}

void mtdc32_setTrigSelect_db(s_mtdc32 * s) { mtdc32_setTrigSelect(s, s->trigSelect); }

void mtdc32_setTrigSelect(s_mtdc32 * s, uint16_t select)
{
	SET16(s->m.md->vmeBase, MTDC32_TRIG_SELECT, select & MTDC32_TRIG_SELECT_MASK);
}

uint16_t mtdc32_getTrigSelect(s_mtdc32 * s)
{
	return GET16(s->m.md->vmeBase, MTDC32_TRIG_SELECT) & MTDC32_TRIG_SELECT_MASK;
}

void mtdc32_setNimT1Osc_db(s_mtdc32 * s) { mtdc32_setNimT1Osc(s, s->nimT1Osc); }

void mtdc32_setNimT1Osc(s_mtdc32 * s, uint16_t go)
{
	SET16(s->m.md->vmeBase, MTDC32_NIM_TRIG1_OSC, go & MTDC32_NIM_TRIG1_OSC_MASK);
}

uint16_t mtdc32_getNimT1Osc(s_mtdc32 * s)
{
	return GET16(s->m.md->vmeBase, MTDC32_NIM_TRIG1_OSC) & MTDC32_NIM_TRIG1_OSC_MASK;
}


void mtdc32_setNimBusy_db(s_mtdc32 * s) { mtdc32_setNimBusy(s, s->nimBusy); }

void mtdc32_setNimBusy(s_mtdc32 * s, uint16_t busy)
{
	SET16(s->m.md->vmeBase, MTDC32_NIM_BUSY, busy & MTDC32_NIM_BUSY_MASK);
}

uint16_t mtdc32_getNimBusy(s_mtdc32 * s)
{
	return GET16(s->m.md->vmeBase, MTDC32_NIM_BUSY) & MTDC32_NIM_BUSY_MASK;
}

void mtdc32_setPulserStatus_db(s_mtdc32 * s) { mtdc32_setPulserStatus(s, s->pulserStatus); }

void mtdc32_setPulserStatus(s_mtdc32 * s, uint16_t mode)
{
	SET16(s->m.md->vmeBase, MTDC32_PULSER_STATUS, mode & MTDC32_PULSER_STATUS_MASK);
}

uint16_t mtdc32_getPulserStatus(s_mtdc32 * s)
{
	return GET16(s->m.md->vmeBase, MTDC32_PULSER_STATUS) & MTDC32_PULSER_STATUS_MASK;
}

void mtdc32_setPulserPattern_db(s_mtdc32 * s) { mtdc32_setPulserPattern(s, s->pulserPattern); }

void mtdc32_setPulserPattern(s_mtdc32 * s, uint16_t pattern)
{
	SET16(s->m.md->vmeBase, MTDC32_PULSER_PATTERN, pattern & MTDC32_PULSER_PATTERN_MASK);
}

uint16_t mtdc32_getPulserPattern(s_mtdc32 * s)
{
	return GET16(s->m.md->vmeBase, MTDC32_PULSER_PATTERN) & MTDC32_PULSER_PATTERN_MASK;
}

void mtdc32_setInputThresh_db(s_mtdc32 * s, uint16_t bnk) { mtdc32_setInputThresh(s, bnk, s->inputThresh[bnk]); }

void mtdc32_setInputThresh(s_mtdc32 * s, uint16_t bnk, uint16_t thresh)
{
	int addr;
	switch (bnk) {
		case 0: addr = MTDC32_INPUT_THRESH_0; break;
		case 1: addr = MTDC32_INPUT_THRESH_1; break;
		default: return;
	}
	SET16(s->m.md->vmeBase, addr, thresh & MTDC32_INPUT_THRESH_MASK);
}

uint16_t mtdc32_getInputThresh(s_mtdc32 * s, uint16_t bnk)
{
	int addr;
	switch (bnk) {
		case 0: addr = MTDC32_INPUT_THRESH_0; break;
		case 1: addr = MTDC32_INPUT_THRESH_1; break;
		default: return 0;
	}
	return GET16(s->m.md->vmeBase, addr) & MTDC32_INPUT_THRESH_MASK;
}

void mtdc32_setMultLimit_db(s_mtdc32 * s, uint16_t bnk) { mtdc32_setMultLimit(s, bnk, s->multLowLimit[bnk], s->multHighLimit[bnk]); }

void mtdc32_setMultLimit(s_mtdc32 * s, uint16_t bnk, uint16_t llim, uint16_t hlim)
{
	int al, ah;
	switch (bnk) {
		case 0: al = MTDC32_MULT_LOW_LIM_0;
				ah = MTDC32_MULT_HIGH_LIM_0;
				break;
		case 1: al = MTDC32_MULT_LOW_LIM_1;
				ah = MTDC32_MULT_HIGH_LIM_1;
				break;
		default: return;
	}
	SET16(s->m.md->vmeBase, al, llim & MTDC32_MULT_LIMIT_MASK);
	SET16(s->m.md->vmeBase, ah, hlim & MTDC32_MULT_LIMIT_MASK);
}

uint16_t mtdc32_getMultHighLimit(s_mtdc32 * s, uint16_t bnk)
{
	int addr;
	switch (bnk) {
		case 0: addr = MTDC32_MULT_HIGH_LIM_0; break;
		case 1: addr = MTDC32_MULT_HIGH_LIM_1; break;
		default: return;
	}
	return GET16(s->m.md->vmeBase, addr) & MTDC32_MULT_LIMIT_MASK;
}

uint16_t mtdc32_getMultLowLimit(s_mtdc32 * s, uint16_t bnk)
{
	int addr;
	switch (bnk) {
		case 0: addr = MTDC32_MULT_LOW_LIM_0; break;
		case 1: addr = MTDC32_MULT_LOW_LIM_1; break;
		default: return;
	}
	return GET16(s->m.md->vmeBase, addr) & MTDC32_MULT_LIMIT_MASK;
}

bool_t mtdc32_fillStruct(s_mtdc32 * s, char * file)
{
	char res[256];
	char mnUC[256];
	const char * sp;
	int i;
	int val;
	s_mesy * m;
	
	m = &s->m;

	if (root_env_read(file) < 0) {
		sprintf(msg, "[%sfill_struct] %s: Error reading file %s", m->mpref, m->moduleName, file);
		f_ut_send_msg(m->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		return FALSE;
	}

	sprintf(msg, "[%sfill_struct] %s: Reading settings from file %s", m->mpref, m->moduleName, file);
	f_ut_send_msg(m->prefix, msg, ERR__MSG_INFO, MASK__PRTT);

	m->verbose = root_env_getval_b("MTDC32.VerboseMode", FALSE);

	m->dumpRegsOnInit = root_env_getval_b("MTDC32.DumpRegisters", FALSE);

	sp = root_env_getval_s("MTDC32.ModuleName", "");
	if (strcmp(m->moduleName, "mtdc32") != 0 && strcmp(sp, m->moduleName) != 0) {
		sprintf(msg, "[%sfill_struct] %s: File %s contains wrong module name - %s", m->mpref, m->moduleName, file, sp);
		f_ut_send_msg(m->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		return FALSE;
	}
	strcpy(mnUC, sp);
	mnUC[0] = toupper(mnUC[0]);

	sprintf(res, "MTDC32.%s.BlockXfer", mnUC);
	m->blockXfer = root_env_getval_b(res, FALSE);

	sprintf(res, "MTDC32.%s.RepairRawData", mnUC);
	m->repairRawData = root_env_getval_b(res, FALSE);

	sprintf(res, "MTDC32.%s.ReportReadErrors", mnUC);
	m->reportReadErrors = root_env_getval_i(res, 0);

	sprintf(res, "MTDC32.%s.AddrSource", mnUC);
	m->addrSource = root_env_getval_i(res, MESY_ADDR_SOURCE_DEFAULT);

	if (m->addrSource == MESY_ADDR_SOURCE_REG) {
		sprintf(res, "MTDC32.%s.AddrReg", mnUC);
		m->addrReg = root_env_getval_i(res, 0);
		if (m->addrReg == 0) {
			sprintf(msg, "[%sfill_struct] %s: vme addr missing (addr source = reg)", m->mpref, m->moduleName);
			f_ut_send_msg(m->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		}
	}

	sprintf(res, "MTDC32.%s.MCSTSignature", mnUC);
	m->mcstSignature = root_env_getval_x(res, 0x0);
	sprintf(res, "MTDC32.%s.MCSTMaster", mnUC);
	m->mcstMaster = root_env_getval_b(res, FALSE);
	sprintf(res, "MTDC32.%s.CBLTSignature", mnUC);
	m->cbltSignature = root_env_getval_x(res, 0x0);
	sprintf(res, "MTDC32.%s.FirstInCbltChain", mnUC);
	m->firstInCbltChain = root_env_getval_b(res, FALSE);
	sprintf(res, "MTDC32.%s.LastInCbltChain", mnUC);
	m->lastInCbltChain = root_env_getval_b(res, FALSE);

	sprintf(res, "MTDC32.%s.ModuleId", mnUC);
	m->moduleId = root_env_getval_i(res, MESY_MODULE_ID_DEFAULT);

	sprintf(res, "MTDC32.%s.DataWidth", mnUC);
	m->dataWidth = root_env_getval_i(res, MESY_DATA_LENGTH_FORMAT_DEFAULT);

	sprintf(res, "MTDC32.%s.MultiEvent", mnUC);
	m->multiEvent = root_env_getval_i(res, MESY_MULTI_EVENT_DEFAULT);

	if ((m->multiEvent & MESY_MULTI_EVENT_BERR) == 0) {
		sprintf(msg, "[%sfill_struct] %s: Readout with BERR enabled", m->mpref, m->moduleName);
	} else {
		sprintf(msg, "[%sfill_struct] %s: Readout with EOB enabled", m->mpref, m->moduleName);
	}
	f_ut_send_msg(m->prefix, msg, ERR__MSG_INFO, MASK__PRTT);

	sprintf(res, "MTDC32.%s.XferData", mnUC);
	m->xferData = root_env_getval_i(res, MESY_MAX_XFER_DATA_DEFAULT);

	sprintf(res, "MTDC32.%s.MarkingType", mnUC);
	m->markingType = root_env_getval_i(res, MESY_MARKING_TYPE_DEFAULT);

	sprintf(res, "MTDC32.%s.BankOperation", mnUC);
	s->bankOperation = root_env_getval_i(res, MTDC32_BANK_OPERATION_DEFAULT);

	sprintf(res, "MTDC32.%s.TdcResolution", mnUC);
	s->tdcResolution = root_env_getval_i(res, MTDC32_TDC_RESOLUTION_DEFAULT);

	sprintf(res, "MTDC32.%s.OutputFormat", mnUC);
	s->outputFormat = root_env_getval_i(res, MTDC32_OUTPUT_FORMAT_DEFAULT);

	for (i = 0; i <= 1; i++) {
		sprintf(res, "MTDC32.%s.WinStart.%d", mnUC, i);
		s->winStart[i] = root_env_getval_i(res, MTDC32_WIN_START_DEFAULT);
	}

	for (i = 0; i <= 1; i++) {
		sprintf(res, "MTDC32.%s.WinWidth.%d", mnUC, i);
		s->winWidth[i] = root_env_getval_i(res, MTDC32_WIN_WIDTH_DEFAULT);
	}

	for (i = 0; i <= 1; i++) {
		sprintf(res, "MTDC32.%s.TrigSrcTrig.%d", mnUC, i);
		s->trigSrcTrig[i] = root_env_getval_i(res, 0);
	}

	for (i = 0; i <= 1; i++) {
		sprintf(res, "MTDC32.%s.TrigSrcChan.%d", mnUC, i);
		val = root_env_getval_i(res, -1);
		if (val >= 0) s->trigSrcChan[i] = MTDC32_TRIG_SRC_CHAN_ACTIVE | (val & MTDC32_TRIG_SRC_CHAN_MASK);
	}

	for (i = 0; i <= 1; i++) {
		sprintf(res, "MTDC32.%s.TrigSrcBank.%d", mnUC, i);
		s->trigSrcBank[i] = root_env_getval_i(res, 0);
	}

	sprintf(res, "MTDC32.%s.FirstHit", mnUC);
	s->firstHit = root_env_getval_i(res, MTDC32_FIRST_HIT_DEFAULT);

	sprintf(res, "MTDC32.%s.NegEdge", mnUC);
	s->negEdge = root_env_getval_i(res, MTDC32_NEG_EDGE_DEFAULT);

	sprintf(res, "MTDC32.%s.EclTerm", mnUC);
	s->eclTerm = root_env_getval_i(res, MTDC32_ECL_TERM_DEFAULT);

	sprintf(res, "MTDC32.%s.EclT1Osc", mnUC);
	s->eclT1Osc = root_env_getval_i(res, MTDC32_ECL_TRIG1_OSC_DEFAULT);

	sprintf(res, "MTDC32.%s.TrigSelect", mnUC);
	s->trigSelect = root_env_getval_i(res, MTDC32_TRIG_SELECT_DEFAULT);

	sprintf(res, "MTDC32.%s.NimT1Osc", mnUC);
	s->nimT1Osc = root_env_getval_i(res, MTDC32_NIM_TRIG1_OSC_DEFAULT);

	sprintf(res, "MTDC32.%s.NimBusy", mnUC);
	s->nimBusy = root_env_getval_i(res, MTDC32_NIM_BUSY_DEFAULT);

	sprintf(res, "MTDC32.%s.PulserStatus", mnUC);
	s->pulserStatus = root_env_getval_i(res, MTDC32_PULSER_STATUS_DEFAULT);

	sprintf(res, "MTDC32.%s.PulserPattern", mnUC);
	s->pulserPattern = root_env_getval_i(res, MTDC32_PULSER_PATTERN_DEFAULT);

	for (i = 0; i <= 1; i++) {
		sprintf(res, "MTDC32.%s.InputThresh.%d", mnUC, i);
		s->inputThresh[i] = root_env_getval_i(res, MTDC32_INPUT_THRESH_DEFAULT);
	}
	
	sprintf(res, "MTDC32.%s.TsSource", mnUC);
	m->ctraTsSource = root_env_getval_i(res, MESY_CTRA_TS_SOURCE_DEFAULT);

	sprintf(res, "MTDC32.%s.TsDivisor", mnUC);
	m->ctraTsDivisor = root_env_getval_i(res, MESY_CTRA_TS_DIVISOR_DEFAULT);

	sprintf(res, "MTDC32.%s.MultHighLimit.0", mnUC);
	s->multHighLimit[0] = root_env_getval_i(res, MTDC32_HIGH_LIMIT_DEFAULT_0);
	sprintf(res, "MTDC32.%s.MultHighLimit.1", mnUC);
	s->multHighLimit[1] = root_env_getval_i(res, MTDC32_HIGH_LIMIT_DEFAULT_1);
	
	for (i = 0; i <= 1; i++) {
		sprintf(res, "MTDC32.%s.MultLowLimit.%d", mnUC, i);
		s->multLowLimit[i] = root_env_getval_i(res, MTDC32_LOW_LIMIT_DEFAULT);
	}

	return TRUE;
}

void mtdc32_loadFromDb(s_mtdc32 * s, uint32_t chnPattern)
{
	int ch;
	int bnk;
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
	mtdc32_setBankOperation_db(s);
	mtdc32_setTdcResolution_db(s);
	mtdc32_setOutputFormat_db(s);
	for (bnk = 0; bnk <= 1; bnk++) {
		mtdc32_setWinStart_db(s, bnk);
		mtdc32_setWinWidth_db(s, bnk);
	}
	for (bnk = 0; bnk <= 1; bnk++) mtdc32_setTrigSource_db(s, bnk);
	mtdc32_setFirstHit_db(s);
	mtdc32_setNegEdge_db(s);
	mtdc32_setEclTerm_db(s);
	mtdc32_setEclT1Osc_db(s);
	mtdc32_setTrigSelect_db(s);
	mtdc32_setNimT1Osc_db(s);
	mtdc32_setNimBusy_db(s);
	mtdc32_setPulserStatus_db(s);
	mtdc32_setPulserPattern_db(s);
	for (bnk = 0; bnk <= 1; bnk++) mtdc32_setInputThresh_db(s, bnk);
	mesy_setTsSource_db(m);
	mesy_setTsDivisor_db(m);
	for (bnk = 0; bnk <= 1; bnk++) mtdc32_setMultLimit_db(s, bnk);
}

bool_t mtdc32_dumpRegisters(s_mtdc32 * s, char * file)
{
	FILE * f;

	int ch;
	int bnk;
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
	fprintf(f, "Module ID [0x6004]        : %d\n", mesy_getModuleId(m));
	fprintf(f, "Data width [0x6032]       : %d\n", mesy_getDataWidth(m));
	fprintf(f, "Multi event [0x6036]      : %d\n", mesy_getMultiEvent(m));
/*	fprintf(f, "Buffer thresh [0x6018]    : %d\n", mtdc32_getBufferThresh(s)); */
	fprintf(f, "Max xfer data wc [0x601A] : %d\n", mesy_getXferData(m));
	fprintf(f, "Marking type [0x6038]     : %d\n", mesy_getMarkingType(m));
	fprintf(f, "Bank operation [0x6040]   : %d\n", mtdc32_getBankOperation(s));
	fprintf(f, "Tdc resolution [0x6042]   : %d\n", mtdc32_getTdcResolution(s));
	fprintf(f, "Output format [0x6044]    : %d\n", mtdc32_getOutputFormat(s));
	for (bnk = 0; bnk <= 1; bnk++) {
		fprintf(f, "Window start %d [0x605%d]   : %d\n", bnk, bnk*2, mtdc32_getWinStart(s, bnk));
		fprintf(f, "Window width %d [0x605%d]   : %d\n", bnk, bnk*2 + 4, mtdc32_getWinWidth(s, bnk));
	}
	for (bnk = 0; bnk <= 1; bnk++) fprintf(f, "Trigger source %d         : %x\n", bnk, mtdc32_getTrigSource(s, bnk));
	fprintf(f, "First hit [0x605C]        : %d\n", mtdc32_getFirstHit(s));
	fprintf(f, "Negative edge [0x6060]    : %d\n", mtdc32_getNegEdge(s));
	fprintf(f, "Ecl termination [0x6062]  : %#x\n", mtdc32_getEclTerm(s));
	fprintf(f, "Ecl trig or osc [0x6064]  : %d\n", mtdc32_getEclT1Osc(s));
	fprintf(f, "Trigger select [0x6068]   : %d\n", mtdc32_getTrigSelect(s));
	fprintf(f, "Nim trig or osc [0x606A]  : %d\n", mtdc32_getNimT1Osc(s));
	fprintf(f, "Nim busy [0x606E]         : %d\n", mtdc32_getNimBusy(s));
	fprintf(f, "Pulser status [0x6070]    : %d\n", mtdc32_getPulserStatus(s));
	fprintf(f, "Pulser pattern [0x6072]   : %d\n", mtdc32_getPulserPattern(s));
	fprintf(f, "Input thresh 0 [0x6078]   : %d\n", mtdc32_getInputThresh(s, 0));
	fprintf(f, "Input thresh 1 [0x607A]   : %d\n", mtdc32_getInputThresh(s, 1));
	fprintf(f, "Timestamp source [0x6096] : %#x\n", mesy_getTsSource(m));
	fprintf(f, "Timestamp divisor [0x6098]: %d\n", mesy_getTsDivisor(m));
	fprintf(f, "Mult high lim 0 [0x60B0]  : %d\n", mtdc32_getMultHighLimit(s, 0));
	fprintf(f, "Mult low lim 0 [0x60B2]   : %d\n", mtdc32_getMultLowLimit(s, 0));
	fprintf(f, "Mult high lim 1 [0x60B4]  : %d\n", mtdc32_getMultHighLimit(s, 1));
	fprintf(f, "Mult low lim 1 [0x60B6]   : %d\n", mtdc32_getMultLowLimit(s, 1));
	fclose(f);
}

void mtdc32_printDb(s_mtdc32 * s)
{
	int ch;
	int bnk;
	s_mesy * m;
	
	m = &s->m;

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
	printf("Buffer thresh     : %d\n", s->bufferThresh);
	printf("Max xfer data wc  : %d\n", m->xferData);
	printf("Marking type      : %d\n", m->markingType);
	printf("Bank operation    : %d\n", s->bankOperation);
	printf("Tdc resolution    : %d\n", s->tdcResolution);
	printf("Output format     : %d\n", s->outputFormat);
	for (bnk = 0; bnk <= 1; bnk++) {
		printf("Window start %d    : %d\n", bnk, s->winStart[bnk]);
		printf("Window width %d    : %d\n", bnk, s->winWidth[bnk]);
	}
	for (bnk = 0; bnk <= 1; bnk++) printf("Trigger source %d         : %x\n", bnk, mtdc32_getTrigSource(s, bnk));
	printf("First hit         : %d\n", s->firstHit);
	printf("Negative edge     : %d\n", s->negEdge);
	printf("Ecl termination   : %#x\n", s->eclTerm);
	printf("Ecl trig or osc   : %d\n", s->eclT1Osc);
	printf("Trigger select    : %d\n", s->trigSelect);
	printf("Nim trig or osc   : %d\n", s->nimT1Osc);
	printf("Nim busy          : %d\n", s->nimBusy);
	printf("Pulser status     : %d\n", s->pulserStatus);
	printf("Pulser pattern    : %d\n", s->pulserPattern);
	for (bnk = 0; bnk <= 1; bnk++) printf("Input thresh %d   : %d\n", bnk, s->inputThresh[bnk]);
	printf("Timestamp source  : %#x\n", m->ctraTsSource);
	printf("Timestamp divisor : %d\n", m->ctraTsDivisor);
	for (bnk = 0; bnk <= 1; bnk++) {
		printf("Mult low lim %d    : %d\n", bnk, s->multLowLimit[bnk]);
		printf("Mult high lim %d   : %d\n", bnk, s->multHighLimit[bnk]);
	}
}



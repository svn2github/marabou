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

#include "mtdc32.h"
#include "mtdc32_database.h"
#include "mtdc32_protos.h"

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

s_mtdc32 * be;

void mtdc32_catchBerr() { busError = TRUE; }

s_mtdc32 * mtdc32_alloc(char * moduleName, s_mapDescr * md, int serial)
{
	s_mtdc32 * s;
	int firmware, mainRev;

	s = (s_mtdc32 *) calloc(1, sizeof(s_mtdc32));
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
		s->firstInCbltChain = FALSE;
		s->lastInCbltChain = FALSE;
		
		s->mcstAddr = 0;
		s->cbltAddr = 0;

		firmware = GET16(s->md->vmeBase, MTDC32_FIRMWARE_REV);
		mainRev = (firmware >> 8) & 0xff;
		s->memorySize = (mainRev >= 2) ? (8*1024 + 2) : (1024 + 2);
		
		busError = FALSE;
	} else {
		sprintf(msg, "[%salloc] %s: Can't allocate mtdc32 struct", s->mpref, s->moduleName);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
	}
	return s;
}

bool_t mtdc32_initBLT(s_mtdc32 * s, bool_t flag)
{
	if (!s->blockXfer) return flag;			/* not using BLT -> nothing to do */
	
	if (flag == kBLTInitError) {			/* BLT not available */
		s->blockXfer = FALSE;
		return kBLTInitError;
	}
		
	if (flag == kBLTInitNotDone) {			/* we have to call InitBLT() once */
		if (!initBLT()) {
			s->blockXfer = FALSE;			/* not successful: turn off BLT */
			return kBLTInitError;
		}
	}
	
	if (s->md->mappingBLT == kVMEMappingUndef) {	/* already mapped? */
		if (mapBLT(s->md, 0xb) == NULL) {			/* no, map BLT page */
			s->blockXfer = FALSE;					/* no succedss, turn BLT off */
			return kBLTInitDone;					/* BLT status remains unchanged */
		}
	}

	setBLTMode(s->md, BMA_M_Vsz32, BMA_M_WzD32, TRUE);
	return kBLTInitDone;
}

bool_t mtdc32_useBLT(s_mtdc32 * s) {
	return s->blockXfer;
}

void mtdc32_soft_reset(s_mtdc32 * s)
{
  SET16(s->md->vmeBase, MTDC32_SOFT_RESET, 0x1);
}

void mtdc32_setAddrReg_db(s_mtdc32 * s) { mtdc32_setAddrReg(s, s->addrReg); }

void mtdc32_setAddrReg(s_mtdc32 * s, uint16_t vmeAddr)
{
	if (vmeAddr) {
		SET16(s->md->vmeBase, MTDC32_ADDR_REG, vmeAddr);
		SET16(s->md->vmeBase, MTDC32_ADDR_SOURCE, MTDC32_ADDR_SOURCE_REG);
		s->md->vmeBase = vmeAddr;
	}
}

uint16_t mtdc32_getAddrReg(s_mtdc32 * s)
{
	uint16_t source = GET16(s->md->vmeBase, MTDC32_ADDR_SOURCE);
	if (source & MTDC32_ADDR_SOURCE_REG) return GET16(s->md->vmeBase, MTDC32_ADDR_REG);
	else return 0;
}

void mtdc32_setModuleId_db(s_mtdc32 * s) { mtdc32_setModuleId(s, s->moduleId); }

void mtdc32_setModuleId(s_mtdc32 * s, uint16_t id)
{
	SET16(s->md->vmeBase, MTDC32_MODULE_ID, id & MTDC32_MODULE_ID_MASK);
}

uint16_t mtdc32_getModuleId(s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_MODULE_ID) & MTDC32_MODULE_ID_MASK;
}

uint16_t mtdc32_getFifoLength(s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_BUFFER_DATA_LENGTH) & MTDC32_BUFFER_DATA_LENGTH_MASK;
}

void mtdc32_setDataWidth_db(s_mtdc32 * s) { mtdc32_setDataWidth(s, s->dataWidth); }

void mtdc32_setDataWidth(s_mtdc32 * s, uint16_t width)
{
	SET16(s->md->vmeBase, MTDC32_DATA_LENGTH_FORMAT, width & MTDC32_DATA_LENGTH_FORMAT_MASK);
}

uint16_t mtdc32_getDataWidth(s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_DATA_LENGTH_FORMAT) & MTDC32_DATA_LENGTH_FORMAT_MASK;
}

void mtdc32_setMultiEvent_db(s_mtdc32 * s) { mtdc32_setMultiEvent(s, s->multiEvent); }

void mtdc32_setMultiEvent(s_mtdc32 * s, uint16_t mode)
{
	SET16(s->md->vmeBase, MTDC32_MULTI_EVENT, mode & MTDC32_MULTI_EVENT_MASK);
}

uint16_t mtdc32_getMultiEvent(s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_MULTI_EVENT) & MTDC32_MULTI_EVENT_MASK;
}

void mtdc32_setXferData_db(s_mtdc32 * s) { mtdc32_setXferData(s, s->xferData); }

void mtdc32_setXferData(s_mtdc32 * s, uint16_t wc)
{
	SET16(s->md->vmeBase, MTDC32_MAX_XFER_DATA, wc & MTDC32_MAX_XFER_DATA_MASK);
}

uint16_t mtdc32_getXferData(s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_MAX_XFER_DATA) & MTDC32_MAX_XFER_DATA_MASK;
}

void mtdc32_setMarkingType_db(s_mtdc32 * s) { mtdc32_setMarkingType(s, s->markingType); }

void mtdc32_setMarkingType(s_mtdc32 * s, uint16_t type)
{
	SET16(s->md->vmeBase, MTDC32_MARKING_TYPE, type & MTDC32_MARKING_TYPE_MASK);
}

uint16_t mtdc32_getMarkingType(s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_MARKING_TYPE) & MTDC32_MARKING_TYPE_MASK;
}

void mtdc32_setBankOperation_db(s_mtdc32 * s) { mtdc32_setBankOperation(s, s->bankOperation); }

void mtdc32_setBankOperation(s_mtdc32 * s, uint16_t oper)
{
	SET16(s->md->vmeBase, MTDC32_BANK_OPERATION, oper & MTDC32_BANK_OPERATION_MASK);
}

uint16_t mtdc32_getBankOperation(s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_BANK_OPERATION) & MTDC32_BANK_OPERATION_MASK;
}

void mtdc32_setTdcResolution_db(s_mtdc32 * s) { mtdc32_setTdcResolution(s, s->tdcResolution); }

void mtdc32_setTdcResolution(s_mtdc32 * s, uint16_t res)
{
	SET16(s->md->vmeBase, MTDC32_TDC_RESOLUTION, res & MTDC32_TDC_RESOLUTION_MASK);
}

uint16_t mtdc32_getTdcResolution(s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_TDC_RESOLUTION) & MTDC32_TDC_RESOLUTION_MASK;
}

void mtdc32_setOutputFormat_db(s_mtdc32 * s) { mtdc32_setOutputFormat(s, s->outputFormat); }

void mtdc32_setOutputFormat(s_mtdc32 * s, uint16_t format)
{
	SET16(s->md->vmeBase, MTDC32_OUTPUT_FORMAT, format & MTDC32_OUTPUT_FORMAT_MASK);
}

uint16_t mtdc32_getOutputFormat(s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_OUTPUT_FORMAT) & MTDC32_OUTPUT_FORMAT_MASK;
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
	SET16(s->md->vmeBase, addr, (start + 16384));
}

int16_t mtdc32_getWinStart(s_mtdc32 * s, uint16_t bnk)
{
	int addr;
	switch (bnk) {
		case 0: addr = MTDC32_WIN_START_0; break;
		case 1: addr = MTDC32_WIN_START_1; break;
		default: return 0;
	}
	return GET16(s->md->vmeBase, addr) - 16384;
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
	SET16(s->md->vmeBase, addr, width);
}

int16_t mtdc32_getWinWidth(s_mtdc32 * s, uint16_t bnk)
{
	int addr;
	switch (bnk) {
		case 0: addr = MTDC32_WIN_WIDTH_0; break;
		case 1: addr = MTDC32_WIN_WIDTH_1; break;
		default: return 0;
	}
	return GET16(s->md->vmeBase, addr);
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
	SET16(s->md->vmeBase, addr, trigSource);
}

uint16_t mtdc32_getTrigSource(s_mtdc32 * s, uint16_t bnk)
{
	int addr;
	switch (bnk) {
		case 0: addr = MTDC32_TRIG_SOURCE_0; break;
		case 1: addr = MTDC32_TRIG_SOURCE_1; break;
		default: return;
	}
	return GET16(s->md->vmeBase, addr);
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
	trigSource = GET16(s->md->vmeBase, addr);
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
	trigSource = GET16(s->md->vmeBase, addr);
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
	trigSource = GET16(s->md->vmeBase, addr);
	return ((trigSource >> 8) & MTDC32_TRIG_SRC_BANK_MASK);
}

void mtdc32_setFirstHit_db(s_mtdc32 * s) { mtdc32_setFirstHit(s, s->firstHit); }

void mtdc32_setFirstHit(s_mtdc32 * s, uint16_t fhit)
{
	SET16(s->md->vmeBase, MTDC32_FIRST_HIT, fhit & MTDC32_FIRST_HIT_MASK);
}

uint16_t mtdc32_getFirstHit(s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_FIRST_HIT) & MTDC32_FIRST_HIT_MASK;
}

void mtdc32_setNegEdge_db(s_mtdc32 * s) { mtdc32_setNegEdge(s, s->negEdge); }

void mtdc32_setNegEdge(s_mtdc32 * s, uint16_t edge)
{
	SET16(s->md->vmeBase, MTDC32_NEG_EDGE, edge & MTDC32_NEG_EDGE_MASK);
}

uint16_t mtdc32_getNegEdge(s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_NEG_EDGE) & MTDC32_NEG_EDGE_MASK;
}

void mtdc32_setEclTerm_db(s_mtdc32 * s) { mtdc32_setEclTerm(s, s->eclTerm); }

void mtdc32_setEclTerm(s_mtdc32 * s, uint16_t term)
{
	SET16(s->md->vmeBase, MTDC32_ECL_TERMINATORS, term & MTDC32_ECL_TERM_MASK);
}

uint16_t mtdc32_getEclTerm(s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_ECL_TERMINATORS) & MTDC32_ECL_TERM_MASK;
}

void mtdc32_setEclT1Osc_db(s_mtdc32 * s) { mtdc32_setEclT1Osc(s, s->eclT1Osc); }

void mtdc32_setEclT1Osc(s_mtdc32 * s, uint16_t go)
{
	SET16(s->md->vmeBase, MTDC32_ECL_TRIG1_OSC, go & MTDC32_ECL_TRIG1_OSC_MASK);
}

uint16_t mtdc32_getEclT1Osc(s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_ECL_TRIG1_OSC) & MTDC32_ECL_TRIG1_OSC_MASK;
}

void mtdc32_setTrigSelect_db(s_mtdc32 * s) { mtdc32_setTrigSelect(s, s->trigSelect); }

void mtdc32_setTrigSelect(s_mtdc32 * s, uint16_t select)
{
	SET16(s->md->vmeBase, MTDC32_TRIG_SELECT, select & MTDC32_TRIG_SELECT_MASK);
}

uint16_t mtdc32_getTrigSelect(s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_TRIG_SELECT) & MTDC32_TRIG_SELECT_MASK;
}

void mtdc32_setNimT1Osc_db(s_mtdc32 * s) { mtdc32_setNimT1Osc(s, s->nimT1Osc); }

void mtdc32_setNimT1Osc(s_mtdc32 * s, uint16_t go)
{
	SET16(s->md->vmeBase, MTDC32_NIM_TRIG1_OSC, go & MTDC32_NIM_TRIG1_OSC_MASK);
}

uint16_t mtdc32_getNimT1Osc(s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_NIM_TRIG1_OSC) & MTDC32_NIM_TRIG1_OSC_MASK;
}


void mtdc32_setNimBusy_db(s_mtdc32 * s) { mtdc32_setNimBusy(s, s->nimBusy); }

void mtdc32_setNimBusy(s_mtdc32 * s, uint16_t busy)
{
	SET16(s->md->vmeBase, MTDC32_NIM_BUSY, busy & MTDC32_NIM_BUSY_MASK);
}

uint16_t mtdc32_getNimBusy(s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_NIM_BUSY) & MTDC32_NIM_BUSY_MASK;
}

void mtdc32_setPulserStatus_db(s_mtdc32 * s) { mtdc32_setPulserStatus(s, s->pulserStatus); }

void mtdc32_setPulserStatus(s_mtdc32 * s, uint16_t mode)
{
	SET16(s->md->vmeBase, MTDC32_PULSER_STATUS, mode & MTDC32_PULSER_STATUS_MASK);
}

uint16_t mtdc32_getPulserStatus(s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_PULSER_STATUS) & MTDC32_PULSER_STATUS_MASK;
}

void mtdc32_setPulserPattern_db(s_mtdc32 * s) { mtdc32_setPulserPattern(s, s->pulserPattern); }

void mtdc32_setPulserPattern(s_mtdc32 * s, uint16_t pattern)
{
	SET16(s->md->vmeBase, MTDC32_PULSER_PATTERN, pattern & MTDC32_PULSER_PATTERN_MASK);
}

uint16_t mtdc32_getPulserPattern(s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_PULSER_PATTERN) & MTDC32_PULSER_PATTERN_MASK;
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
	SET16(s->md->vmeBase, addr, thresh & MTDC32_INPUT_THRESH_MASK);
}

uint16_t mtdc32_getInputThresh(s_mtdc32 * s, uint16_t bnk)
{
	int addr;
	switch (bnk) {
		case 0: addr = MTDC32_INPUT_THRESH_0; break;
		case 1: addr = MTDC32_INPUT_THRESH_1; break;
		default: return 0;
	}
	return GET16(s->md->vmeBase, addr) & MTDC32_INPUT_THRESH_MASK;
}

void mtdc32_setTsSource_db(s_mtdc32 * s) { mtdc32_setTsSource(s, s->ctraTsSource); }

void mtdc32_setTsSource(s_mtdc32 * s, uint16_t source)
{
	SET16(s->md->vmeBase, MTDC32_CTRA_TS_SOURCE, source & MTDC32_CTRA_TS_SOURCE_MASK);
}

uint16_t mtdc32_getTsSource(s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_CTRA_TS_SOURCE) & MTDC32_CTRA_TS_SOURCE_MASK;
}

void mtdc32_setTsDivisor_db(s_mtdc32 * s) { mtdc32_setTsDivisor(s, s->ctraTsDivisor); }

void mtdc32_setTsDivisor(s_mtdc32 * s, uint16_t div)
{
	SET16(s->md->vmeBase, MTDC32_CTRA_TS_DIVISOR, div & MTDC32_CTRA_TS_DIVISOR_MASK);
}

uint16_t mtdc32_getTsDivisor(s_mtdc32 * s)
{
	return GET16(s->md->vmeBase, MTDC32_CTRA_TS_DIVISOR) & MTDC32_CTRA_TS_DIVISOR_MASK;
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
	SET16(s->md->vmeBase, al, llim & MTDC32_MULT_LIMIT_MASK);
	SET16(s->md->vmeBase, ah, hlim & MTDC32_MULT_LIMIT_MASK);
}

uint16_t mtdc32_getMultHighLimit(s_mtdc32 * s, uint16_t bnk)
{
	int addr;
	switch (bnk) {
		case 0: addr = MTDC32_MULT_HIGH_LIM_0; break;
		case 1: addr = MTDC32_MULT_HIGH_LIM_1; break;
		default: return;
	}
	return GET16(s->md->vmeBase, addr) & MTDC32_MULT_LIMIT_MASK;
}

uint16_t mtdc32_getMultLowLimit(s_mtdc32 * s, uint16_t bnk)
{
	int addr;
	switch (bnk) {
		case 0: addr = MTDC32_MULT_LOW_LIM_0; break;
		case 1: addr = MTDC32_MULT_LOW_LIM_1; break;
		default: return;
	}
	return GET16(s->md->vmeBase, addr) & MTDC32_MULT_LIMIT_MASK;
}

void mtdc32_moduleInfo(s_mtdc32 * s)
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

void mtdc32_setPrefix(s_mtdc32 * s, char * prefix)
{
	strcpy(s->prefix, prefix);
	strcpy(s->mpref, "");
}

bool_t mtdc32_fillStruct(s_mtdc32 * s, char * file)
{
	char res[256];
	char mnUC[256];
	const char * sp;
	int i;
	int val;

	if (root_env_read(file) < 0) {
		sprintf(msg, "[%sfill_struct] %s: Error reading file %s", s->mpref, s->moduleName, file);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		return FALSE;
	}

	sprintf(msg, "[%sfill_struct] %s: Reading settings from file %s", s->mpref, s->moduleName, file);
	f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);

	s->verbose = root_env_getval_b("MTDC32.VerboseMode", FALSE);

	s->dumpRegsOnInit = root_env_getval_b("MTDC32.DumpRegisters", FALSE);

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
	sprintf(res, "MTDC32.%s.MCSTMaster", mnUC);
	s->mcstMaster = root_env_getval_b(res, FALSE);
	sprintf(res, "MTDC32.%s.CBLTSignature", mnUC);
	s->cbltSignature = root_env_getval_x(res, 0x0);
	sprintf(res, "MTDC32.%s.FirstInCbltChain", mnUC);
	s->firstInCbltChain = root_env_getval_b(res, FALSE);
	sprintf(res, "MTDC32.%s.LastInCbltChain", mnUC);
	s->lastInCbltChain = root_env_getval_b(res, FALSE);

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

	if ((s->multiEvent & MTDC32_MULTI_EVENT_BERR) == 0) {
		sprintf(msg, "[%sfill_struct] %s: Readout with BERR enabled", s->mpref, s->moduleName);
	} else {
		sprintf(msg, "[%sfill_struct] %s: Readout with EOB enabled", s->mpref, s->moduleName);
	}
	f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);

	sprintf(res, "MTDC32.%s.XferData", mnUC);
	s->xferData = root_env_getval_i(res, MTDC32_MAX_XFER_DATA_DEFAULT);

	sprintf(res, "MTDC32.%s.MarkingType", mnUC);
	s->markingType = root_env_getval_i(res, MTDC32_MARKING_TYPE_DEFAULT);

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
	s->ctraTsSource = root_env_getval_i(res, MTDC32_CTRA_TS_SOURCE_DEFAULT);

	sprintf(res, "MTDC32.%s.TsDivisor", mnUC);
	s->ctraTsDivisor = root_env_getval_i(res, MTDC32_CTRA_TS_DIVISOR_DEFAULT);

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

	mtdc32_setAddrReg_db(s);
	mtdc32_setMcstCblt_db(s);
	mtdc32_setModuleId_db(s);
	mtdc32_setDataWidth_db(s);
	mtdc32_setMultiEvent_db(s);
	mtdc32_setMarkingType_db(s);
	mtdc32_setXferData_db(s);
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
	mtdc32_setTsSource_db(s);
	mtdc32_setTsDivisor_db(s);
	for (bnk = 0; bnk <= 1; bnk++) mtdc32_setMultLimit_db(s, bnk);
}

bool_t mtdc32_dumpRegisters(s_mtdc32 * s, char * file)
{
	FILE * f;

	int ch;
	int bnk;
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
		if (mtdc32_isFirstInCbltChain(s)) fprintf(f, "MCST/CBLT chain           : first module in chain\n");
		else if (mtdc32_isLastInCbltChain(s)) fprintf(f, "MCST/CBLT chain           : last module in chain\n");
		else fprintf(f, "MCST/CBLT chain   : module in the middle\n");
	}
	fprintf(f, "Module ID [0x6004]        : %d\n", mtdc32_getModuleId(s));
	fprintf(f, "Data width [0x6032]       : %d\n", mtdc32_getDataWidth(s));
	fprintf(f, "Multi event [0x6036]      : %d\n", mtdc32_getMultiEvent(s));
/*	fprintf(f, "Buffer thresh [0x6018]    : %d\n", mtdc32_getBufferThresh(s)); */
	fprintf(f, "Max xfer data wc [0x601A] : %d\n", mtdc32_getXferData(s));
	fprintf(f, "Marking type [0x6038]     : %d\n", mtdc32_getMarkingType(s));
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
	fprintf(f, "Timestamp source [0x6096] : %#x\n", mtdc32_getTsSource(s));
	fprintf(f, "Timestamp divisor [0x6098]: %d\n", mtdc32_getTsDivisor(s));
	fprintf(f, "Mult high lim 0 [0x60B0]  : %d\n", mtdc32_getMultHighLimit(s, 0));
	fprintf(f, "Mult low lim 0 [0x60B2]   : %d\n", mtdc32_getMultLowLimit(s, 0));
	fprintf(f, "Mult high lim 1 [0x60B4]  : %d\n", mtdc32_getMultHighLimit(s, 1));
	fprintf(f, "Mult low lim 1 [0x60B6]   : %d\n", mtdc32_getMultLowLimit(s, 1));
	fclose(f);
}

bool_t mtdc32_dumpRaw(s_mtdc32 * s, char * file)
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

	for (i = 0x6000; i < 0x60B6; i += 2) {
		fprintf(f, "%#lx %#x\n", i, GET16(s->md->vmeBase, i));
	}
	fclose(f);
}

void mtdc32_printDb(s_mtdc32 * s)
{
	int ch;
	int bnk;

	printf("Addr reg          : %#x\n", s->addrReg);

	if (s->mcstSignature != 0)
					printf("MCST signature    : %#x\n", s->mcstSignature);
	else				printf("MCST              : disabled\n");
	if (s->cbltSignature != 0)
					printf("CBLT signature    : %#x\n", s->cbltSignature);
	else				printf("CBLT              : disabled\n");
	if ((s->mcstSignature != 0) || (s->cbltSignature != 0)) {
		if (s->firstInCbltChain) printf("MCST/CBLT chain   : first module in chain\n");
		else if (s->lastInCbltChain) printf("MCST/CBLT chain   : last module in chain\n");
		else printf("MCST/CBLT chain   : module in the middle\n");
	}
	printf("Module ID         : %d\n", s->moduleId);
	printf("Data width        : %d\n", s->dataWidth);
	printf("Multi event       : %d\n", s->multiEvent);
	printf("Buffer thresh     : %d\n", s->bufferThresh);
	printf("Max xfer data wc  : %d\n", s->xferData);
	printf("Marking type      : %d\n", s->markingType);
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
	printf("Timestamp source  : %#x\n", s->ctraTsSource);
	printf("Timestamp divisor : %d\n", s->ctraTsDivisor);
	for (bnk = 0; bnk <= 1; bnk++) {
		printf("Mult low lim %d    : %d\n", bnk, s->multLowLimit[bnk]);
		printf("Mult high lim %d   : %d\n", bnk, s->multHighLimit[bnk]);
	}
}

int mtdc32_readout(s_mtdc32 * s, uint32_t * pointer)
{
	static int addrOffset = 0;

	uint32_t * dataStart, *p;
	uint32_t data;
	unsigned int i, j;
	int bmaError;
	int bmaCount;
	int tryIt;
	int numData, nd;
	int nofeobs;

	uint32_t ptrloc;

	int mode;
	int sts;

	dataStart = pointer;
	
	numData = (int) mtdc32_getFifoLength(s);	
	
	if (numData == 0) return(0);

	if (s->blockXfer) {
		ptrloc = getPhysAddr((char *) pointer, numData * sizeof(uint32_t));
		if (ptrloc == NULL) {
			sprintf(msg, "[%sreadout] %s: Can't relocate mapped pointer %#lx to phys addr - BLT turned off", s->mpref, s->moduleName, pointer);
			f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
			s->blockXfer = FALSE;
		}
	}
	
	if (s->blockXfer) {
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
	} else if ((s->multiEvent & MTDC32_MULTI_EVENT_BERR) == 0) {
		busError = FALSE;
		nd = 0;
		signal(SIGBUS, mtdc32_catchBerr);
		while (1) {
			nd++;
			data = GET32(s->md->vmeBase, MTDC32_DATA);
			if (busError) {
				signal(SIGBUS, SIG_DFL);
				busError = FALSE;
				break;
			}
			if (i == 0) {
				if ((data & 0xF0000000) != 0x40000000) {
					sprintf(msg, "[%sreadout] %s: Wrong header at start of data - %#x", s->mpref, s->moduleName, data);
					f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
				}
			}	
			*pointer++ = data;
		}
	} else {
		while (1) {
			data = GET32(s->md->vmeBase, MTDC32_DATA);
			if (i == 0) {
				if ((data & 0xF0000000) != 0x40000000) {
					sprintf(msg, "[%sreadout] %s: Wrong header at start of data - %#x", s->mpref, s->moduleName, data);
					f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
				}
			}	
			if (data == 0x80000000) break;
			*pointer++ = data;
		}
	}

	if (s->repairRawData) pointer = mtdc32_repairRawData(s, pointer, dataStart);
	
	mtdc32_resetReadout(s);

	return (pointer - dataStart);
}

int mtdc32_readTimeB(s_mtdc32 * s, uint32_t * pointer)
{
	uint16_t * p16 = pointer;
	*p16++ = 0;
	*p16++ = GET16(s->md->vmeBase, MTDC32_CTRB_TIME_0);
	*p16++ = GET16(s->md->vmeBase, MTDC32_CTRB_TIME_1);
	*p16++ = GET16(s->md->vmeBase, MTDC32_CTRB_TIME_2);
	return 4 * sizeof(uint16_t) / sizeof(uint32_t);
}

bool_t mtdc32_dataReady(s_mtdc32 * s)
{
	return TSTB16(s->md->vmeBase, MTDC32_DATA_READY, 0) ;
}

void mtdc32_resetReadout(s_mtdc32 * s)
{
	SET16(s->md->vmeBase, MTDC32_READOUT_RESET, 0x1);
}

void mtdc32_resetTimestamp(s_mtdc32 * s)
{
	SET16(s->md->vmeBase, MTDC32_CTRA_RESET_A_OR_B, 0x3);
}

bool_t mtdc32_testBusError(s_mtdc32 * s)
{
	int bit;
	return TSTB16(s->md->vmeBase, MTDC32_MULTI_EVENT, MTDC32_MULTI_EVENT_BERR) == 1 ? FALSE : TRUE;
}

void mtdc32_enableBusError(s_mtdc32 * s)
{
	CLRB16(s->md->vmeBase, MTDC32_MULTI_EVENT, MTDC32_MULTI_EVENT_BERR);
}

void mtdc32_disableBusError(s_mtdc32 * s)
{
	SETB16(s->md->vmeBase, MTDC32_MULTI_EVENT, MTDC32_MULTI_EVENT_BERR);
}

void mtdc32_startAcq(s_mtdc32 * s)
{
	if (mtdc32_mcstIsEnabled(s)) {
		if (s->mcstMaster) {
			SET16(s->mcstAddr, MTDC32_START_ACQUISITION, 0x0);
			mtdc32_resetFifo_mcst(s);
			mtdc32_resetReadout_mcst(s);
			mtdc32_resetTimestamp_mcst(s);
			SET16(s->mcstAddr, MTDC32_START_ACQUISITION, 0x1);
		}
	} else {	
		SET16(s->md->vmeBase, MTDC32_START_ACQUISITION, 0x0);
		mtdc32_resetFifo(s);
		mtdc32_resetReadout(s);
		mtdc32_resetTimestamp(s);
		SET16(s->md->vmeBase, MTDC32_START_ACQUISITION, 0x1);
	}
}

void mtdc32_stopAcq(s_mtdc32 * s)
{
	if (mtdc32_mcstIsEnabled(s)) {
		if (s->mcstMaster) {
			SET16(s->mcstAddr, MTDC32_START_ACQUISITION, 0x0);
			mtdc32_resetFifo_mcst(s);
			mtdc32_resetReadout_mcst(s);
			mtdc32_resetTimestamp_mcst(s);
		}
	} else {	
		SET16(s->md->vmeBase, MTDC32_START_ACQUISITION, 0x0);
		mtdc32_resetFifo(s);
		mtdc32_resetReadout(s);
		mtdc32_resetTimestamp(s);
	}
}

void mtdc32_resetFifo(s_mtdc32 * s)
{
	SET16(s->md->vmeBase, MTDC32_FIFO_RESET, 0x1);
}

void mtdc32_initMCST(s_mtdc32 * s)
{	
	if (mtdc32_mcstIsEnabled(s) && mtdc32_isMcstMaster(s)) {
		if (s->mcstAddr == 0) s->mcstAddr = mapAdditionalVME(s->md, (s->mcstSignature & 0xFF) << 24, 0);
		sprintf(msg, "[%smsctInit] %s: MCST initialized - signature %#x, addr %#lx\n", s->mpref, s->moduleName, s->mcstSignature, s->mcstAddr);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
	}
}

void mtdc32_setMcstCblt_db(s_mtdc32 * s) {
	mtdc32_setMcstSignature(s, s->mcstSignature);
	mtdc32_setCbltSignature(s, s->cbltSignature);
	if (s->firstInCbltChain) mtdc32_setFirstInCbltChain(s);
	else if (s->lastInCbltChain) mtdc32_setLastInCbltChain(s);
	else mtdc32_setMiddleOfCbltChain(s);
}

void mtdc32_setMcstSignature(s_mtdc32 * s, unsigned long Signature) {
	SET16(s->md->vmeBase, MTDC32_MCST_ADDRESS, Signature);
	if (Signature != 0) mtdc32_setMcstEnable(s); else mtdc32_setMcstDisable(s);
}

uint16_t mtdc32_getMcstSignature(s_mtdc32 * s) {
	uint16_t addr8;
	addr8 = GET16(s->md->vmeBase, MTDC32_MCST_ADDRESS);
	return addr8;
}

bool_t mtdc32_isMcstMaster(s_mtdc32 * s) { return s->mcstMaster; }

void mtdc32_setMcstEnable(s_mtdc32 * s) {
	SET16(s->md->vmeBase, MTDC32_CBLT_MCST_CONTROL, MTDC32_MCST_ENA);
}

void mtdc32_setMcstDisable(s_mtdc32 * s) {
	SET16(s->md->vmeBase, MTDC32_CBLT_MCST_CONTROL, MTDC32_MCST_DIS);
}

bool_t mtdc32_mcstIsEnabled(s_mtdc32 * s) {
	uint16_t ctrl;
	ctrl = GET16(s->md->vmeBase, MTDC32_CBLT_MCST_CONTROL);
	return ((ctrl & MTDC32_MCST_DIS) != 0);
}

void mtdc32_setCbltSignature(s_mtdc32 * s, unsigned long Signature) {
	SET16(s->md->vmeBase, MTDC32_CBLT_ADDRESS, Signature);
	if (Signature != 0) mtdc32_setCbltEnable(s); else mtdc32_setCbltDisable(s);
}

uint16_t mtdc32_getCbltSignature(s_mtdc32 * s) {
	uint16_t addr8;
	addr8 = GET16(s->md->vmeBase, MTDC32_CBLT_ADDRESS);
	return addr8;
}

void mtdc32_setCbltEnable(s_mtdc32 * s) {
	SET16(s->md->vmeBase, MTDC32_CBLT_MCST_CONTROL, MTDC32_CBLT_ENA);
}

void mtdc32_setCbltDisable(s_mtdc32 * s) {
	SET16(s->md->vmeBase, MTDC32_CBLT_MCST_CONTROL, MTDC32_CBLT_DIS);
}

bool_t mtdc32_cbltIsEnabled(s_mtdc32 * s) {
	uint16_t ctrl;
	ctrl = GET16(s->md->vmeBase, MTDC32_CBLT_MCST_CONTROL);
	return ((ctrl & MTDC32_CBLT_DIS) != 0);
}

void mtdc32_setFirstInCbltChain(s_mtdc32 * s) {
	SET16(s->md->vmeBase, MTDC32_CBLT_MCST_CONTROL, MTDC32_CBLT_FIRST_ENA);
	SET16(s->md->vmeBase, MTDC32_CBLT_MCST_CONTROL, MTDC32_CBLT_LAST_DIS);
}

bool_t mtdc32_isFirstInCbltChain(s_mtdc32 * s) {
	uint16_t ctrl;
	ctrl = GET16(s->md->vmeBase, MTDC32_CBLT_MCST_CONTROL);
	return ((ctrl & MTDC32_CBLT_FIRST_DIS) != 0);
}

void mtdc32_setLastInCbltChain(s_mtdc32 * s) {
	SET16(s->md->vmeBase, MTDC32_CBLT_MCST_CONTROL, MTDC32_CBLT_LAST_ENA);
	SET16(s->md->vmeBase, MTDC32_CBLT_MCST_CONTROL, MTDC32_CBLT_FIRST_DIS);
}

bool_t mtdc32_isLastInCbltChain(s_mtdc32 * s) {
	uint16_t ctrl;
	ctrl = GET16(s->md->vmeBase, MTDC32_CBLT_MCST_CONTROL);
	return ((ctrl & MTDC32_CBLT_LAST_DIS) != 0);
}

void mtdc32_setMiddleOfCbltChain(s_mtdc32 * s) {
	SET16(s->md->vmeBase, MTDC32_CBLT_MCST_CONTROL, MTDC32_CBLT_FIRST_DIS);
	SET16(s->md->vmeBase, MTDC32_CBLT_MCST_CONTROL, MTDC32_CBLT_LAST_DIS);
}

bool_t mtdc32_isMiddleOfCbltChain(s_mtdc32 * s) {
	bool_t first, last;
	first = mtdc32_isFirstInCbltChain(s);
	last = mtdc32_isLastInCbltChain(s);
	return (!first && !last);
}

void mtdc32_resetFifo_mcst(s_mtdc32 * s)
{
	SET16(s->mcstAddr, MTDC32_FIFO_RESET, 0x1);
}

void mtdc32_resetReadout_mcst(s_mtdc32 * s)
{
	SET16(s->mcstAddr, MTDC32_READOUT_RESET, 0x1);
}

void mtdc32_resetTimestamp_mcst(s_mtdc32 * s)
{
	SET16(s->mcstAddr, MTDC32_CTRA_RESET_A_OR_B, 0x3);
}

uint32_t * mtdc32_repairRawData(s_mtdc32 * s, uint32_t * pointer, uint32_t * dataStart) {
	return pointer;
}



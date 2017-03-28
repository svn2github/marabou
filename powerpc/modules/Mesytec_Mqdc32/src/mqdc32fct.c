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

#include "mqdc32.h"
#include "mqdc32_database.h"
#include "mqdc32_protos.h"

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

void mqdc32_catchBerr() { busError = TRUE; }

struct s_mqdc32 * mqdc32_alloc(char * moduleName, struct s_mapDescr * md, int serial)
{
	struct s_mqdc32 * s;
	int firmware, mainRev;

	s = (struct s_mqdc32 *) calloc(1, sizeof(struct s_mqdc32));
	if (s != NULL) {
		s->md = md;
		strcpy(s->moduleName, moduleName);
		strcpy(s->prefix, "m_read_meb");
		strcpy(s->mpref, "mqdc32: ");
		s->serial = serial;
		s->verbose = FALSE;
		s->dumpRegsOnInit = FALSE;

		s->mcstSignature = 0x0;
		s->cbltSignature = 0x0;
		s->firstInChain = FALSE;
		s->lastInChain = FALSE;
		
		s->mcstAddr = 0;
		s->cbltAddr = 0;


		firmware = GET16(s->md->vmeBase, MQDC32_FIRMWARE_REV);
		mainRev = (firmware >> 8) & 0xff;
		s->memorySize = (mainRev >= 2) ? (8*1024 + 2) : (1024 + 2);
		
		busError = FALSE;
	} else {
		sprintf(msg, "[%salloc] %s: Can't allocate mqdc32 struct", s->mpref, s->moduleName);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
	}
	return s;
}

void mqdc32_initialize(struct s_mqdc32 * s)
{ }

bool_t mqdc32_useBLT(struct s_mqdc32 * s) {
	return s->blockXfer;
}

void mqdc32_soft_reset(struct s_mqdc32 * s)
{
/*  SET16(s->md->vmeBase, MQDC32_SOFT_RESET, 0x1);	*/
}

uint16_t mqdc32_getThreshold(struct s_mqdc32 * s, uint16_t channel)
{
	uint16_t thresh = 0;
	if (channel < MQDC_NOF_CHANNELS) thresh = GET16(s->md->vmeBase, MQDC32_THRESHOLD + sizeof(uint16_t) * channel) & MQDC32_THRESHOLD_MASK;
	return thresh;
}

void mqdc32_setThreshold_db(struct s_mqdc32 * s, uint16_t channel)
{
	if (channel < MQDC_NOF_CHANNELS) mqdc32_setThreshold(s, channel, s->threshold[channel]);
}

void mqdc32_setThreshold(struct s_mqdc32 * s, uint16_t channel, uint16_t thresh)
{
	if (channel < MQDC_NOF_CHANNELS) SET16(s->md->vmeBase, MQDC32_THRESHOLD + sizeof(uint16_t) * channel, thresh & MQDC32_THRESHOLD_MASK);
}

void mqdc32_setAddrReg_db(struct s_mqdc32 * s) { mqdc32_setAddrReg(s, s->addrReg); }

void mqdc32_setAddrReg(struct s_mqdc32 * s, uint16_t vmeAddr)
{
	if (vmeAddr) {
		SET16(s->md->vmeBase, MQDC32_ADDR_REG, vmeAddr);
		SET16(s->md->vmeBase, MQDC32_ADDR_SOURCE, MQDC32_ADDR_SOURCE_REG);
		s->md->vmeBase = vmeAddr;
	}
}

uint16_t mqdc32_getAddrReg(struct s_mqdc32 * s)
{
	uint16_t source = GET16(s->md->vmeBase, MQDC32_ADDR_SOURCE);
	if (source & MQDC32_ADDR_SOURCE_REG) return GET16(s->md->vmeBase, MQDC32_ADDR_REG);
	else return 0;
}

void mqdc32_setModuleId_db(struct s_mqdc32 * s) { mqdc32_setModuleId(s, s->moduleId); }

void mqdc32_setModuleId(struct s_mqdc32 * s, uint16_t id)
{
	SET16(s->md->vmeBase, MQDC32_MODULE_ID, id & MQDC32_MODULE_ID_MASK);
}

uint16_t mqdc32_getModuleId(struct s_mqdc32 * s)
{
	return GET16(s->md->vmeBase, MQDC32_MODULE_ID) & MQDC32_MODULE_ID_MASK;
}

uint16_t mqdc32_getFifoLength(struct s_mqdc32 * s)
{
	return GET16(s->md->vmeBase, MQDC32_BUFFER_DATA_LENGTH) & MQDC32_BUFFER_DATA_LENGTH_MASK;
}

void mqdc32_setDataWidth_db(struct s_mqdc32 * s) { mqdc32_setDataWidth(s, s->dataWidth); }

void mqdc32_setDataWidth(struct s_mqdc32 * s, uint16_t width)
{
	SET16(s->md->vmeBase, MQDC32_DATA_LENGTH_FORMAT, width & MQDC32_DATA_LENGTH_FORMAT_MASK);
}

uint16_t mqdc32_getDataWidth(struct s_mqdc32 * s)
{
	return GET16(s->md->vmeBase, MQDC32_DATA_LENGTH_FORMAT) & MQDC32_DATA_LENGTH_FORMAT_MASK;
}

void mqdc32_setMultiEvent_db(struct s_mqdc32 * s) { mqdc32_setMultiEvent(s, s->multiEvent); }

void mqdc32_setMultiEvent(struct s_mqdc32 * s, uint16_t mode)
{
	SET16(s->md->vmeBase, MQDC32_MULTI_EVENT, mode & MQDC32_MULTI_EVENT_MASK);
}

uint16_t mqdc32_getMultiEvent(struct s_mqdc32 * s)
{
	return GET16(s->md->vmeBase, MQDC32_MULTI_EVENT) & MQDC32_MULTI_EVENT_MASK;
}

void mqdc32_setXferData_db(struct s_mqdc32 * s) { mqdc32_setXferData(s, s->xferData); }

void mqdc32_setXferData(struct s_mqdc32 * s, uint16_t wc)
{
	SET16(s->md->vmeBase, MQDC32_MAX_XFER_DATA, wc & MQDC32_MAX_XFER_DATA_MASK);
}

uint16_t mqdc32_getXferData(struct s_mqdc32 * s)
{
	return GET16(s->md->vmeBase, MQDC32_MAX_XFER_DATA) & MQDC32_MAX_XFER_DATA_MASK;
}

void mqdc32_setMarkingType_db(struct s_mqdc32 * s) { mqdc32_setMarkingType(s, s->markingType); }

void mqdc32_setMarkingType(struct s_mqdc32 * s, uint16_t type)
{
	SET16(s->md->vmeBase, MQDC32_MARKING_TYPE, type & MQDC32_MARKING_TYPE_MASK);
}

uint16_t mqdc32_getMarkingType(struct s_mqdc32 * s)
{
	return GET16(s->md->vmeBase, MQDC32_MARKING_TYPE) & MQDC32_MARKING_TYPE_MASK;
}

void mqdc32_setBankOperation_db(struct s_mqdc32 * s) { mqdc32_setBankOperation(s, s->bankOperation); }

void mqdc32_setBankOperation(struct s_mqdc32 * s, uint16_t oper)
{
	SET16(s->md->vmeBase, MQDC32_BANK_OPERATION, oper & MQDC32_BANK_OPERATION_MASK);
}

uint16_t mqdc32_getBankOperation(struct s_mqdc32 * s)
{
	return GET16(s->md->vmeBase, MQDC32_BANK_OPERATION) & MQDC32_BANK_OPERATION_MASK;
}

void mqdc32_setAdcResolution_db(struct s_mqdc32 * s) { mqdc32_setAdcResolution(s, s->adcResolution); }

void mqdc32_setAdcResolution(struct s_mqdc32 * s, uint16_t res)
{
	SET16(s->md->vmeBase, MQDC32_ADC_RESOLUTION, res & MQDC32_ADC_RESOLUTION_MASK);
}

uint16_t mqdc32_getAdcResolution(struct s_mqdc32 * s)
{
	return GET16(s->md->vmeBase, MQDC32_ADC_RESOLUTION) & MQDC32_ADC_RESOLUTION_MASK;
}

void mqdc32_setBankOffset_db(struct s_mqdc32 * s, uint16_t bank) { mqdc32_setBankOffset(s, bank, s->bankOffset[bank]); }

void mqdc32_setBankOffset(struct s_mqdc32 * s, uint16_t bank, uint16_t offset)
{
	int addr;
	switch (bank) {
		case 0: addr = MQDC32_OFFSET_BANK_0; break;
		case 1: addr = MQDC32_OFFSET_BANK_1; break;
		default: return;
	}
	SET16(s->md->vmeBase, addr, offset & MQDC32_BANK_OFFSET_MASK);
}

uint16_t mqdc32_getBankOffset(struct s_mqdc32 * s, uint16_t bank)
{
	int addr;
	switch (bank) {
		case 0: addr = MQDC32_OFFSET_BANK_0; break;
		case 1: addr = MQDC32_OFFSET_BANK_1; break;
		default: return;
	}
	return GET16(s->md->vmeBase, addr) & MQDC32_BANK_OFFSET_MASK;
}

void mqdc32_setGateLimit_db(struct s_mqdc32 * s, uint16_t bank) { mqdc32_setGateLimit(s, bank, s->gateLimit[bank]); }

void mqdc32_setGateLimit(struct s_mqdc32 * s, uint16_t bank, uint16_t limit)
{
	int addr;
	switch (bank) {
		case 0: addr = MQDC32_GATE_LIMIT_BANK_0; break;
		case 1: addr = MQDC32_GATE_LIMIT_BANK_1; break;
		default: return;
	}
	SET16(s->md->vmeBase, addr, limit & MQDC32_GATE_LIMIT_MASK);
}

uint16_t mqdc32_getGateLimit(struct s_mqdc32 * s, uint16_t bank)
{
	int addr;
	switch (bank) {
		case 0: addr = MQDC32_GATE_LIMIT_BANK_0; break;
		case 1: addr = MQDC32_GATE_LIMIT_BANK_1; break;
		default: return;
	}
	return GET16(s->md->vmeBase, addr) & MQDC32_GATE_LIMIT_MASK;
}

void mqdc32_setSlidingScaleOff_db(struct s_mqdc32 * s) { mqdc32_setSlidingScaleOff(s, s->slidingScaleOff); }

void mqdc32_setSlidingScaleOff(struct s_mqdc32 * s, bool_t flag)
{
	uint16_t f = flag ? 1 : 0;
	SET16(s->md->vmeBase, MQDC32_SLIDING_SCALE_OFF, f);
}

bool_t mqdc32_getSlidingScaleOff(struct s_mqdc32 * s)
{
	uint16_t f = GET16(s->md->vmeBase, MQDC32_SLIDING_SCALE_OFF) & MQDC32_SLIDING_SCALE_OFF_MASK;
	return (f != 0) ? TRUE : FALSE;
}

void mqdc32_setSkipOutOfRange_db(struct s_mqdc32 * s) { mqdc32_setSkipOutOfRange(s, s->skipOutOfRange); }

void mqdc32_setSkipOutOfRange(struct s_mqdc32 * s, bool_t flag)
{
	uint16_t f = flag ? 1 : 0;
	SET16(s->md->vmeBase, MQDC32_SKIP_OUT_OF_RANGE, f);
}

bool_t mqdc32_getSkipOutOfRange(struct s_mqdc32 * s)
{
	uint16_t f = GET16(s->md->vmeBase, MQDC32_SKIP_OUT_OF_RANGE) & MQDC32_SKIP_OUT_OF_RANGE_MASK;
	return (f != 0) ? TRUE : FALSE;
}

void mqdc32_setIgnoreThresholds_db(struct s_mqdc32 * s) { mqdc32_setIgnoreThresholds(s, s->ignoreThresh); }

void mqdc32_setIgnoreThresholds(struct s_mqdc32 * s, bool_t flag)
{
	uint16_t f = flag ? 1 : 0;
	SET16(s->md->vmeBase, MQDC32_IGNORE_THRESHOLDS, f);
}

bool_t mqdc32_getIgnoreThresholds(struct s_mqdc32 * s)
{
	uint16_t f = GET16(s->md->vmeBase, MQDC32_IGNORE_THRESHOLDS) & MQDC32_IGNORE_THRESH_MASK;
	return (f != 0) ? TRUE : FALSE;
}

void mqdc32_setInputCoupling_db(struct s_mqdc32 * s) { mqdc32_setInputCoupling(s, s->inputCoupling); }

void mqdc32_setInputCoupling(struct s_mqdc32 * s, uint16_t coupling)
{
	SET16(s->md->vmeBase, MQDC32_INPUT_COUPLING, coupling & MQDC32_INPUT_COUPLING_MASK);
}

uint16_t mqdc32_getInputCoupling(struct s_mqdc32 * s)
{
	return GET16(s->md->vmeBase, MQDC32_INPUT_COUPLING) & MQDC32_INPUT_COUPLING_MASK;
}

void mqdc32_setEclTerm_db(struct s_mqdc32 * s) { mqdc32_setEclTerm(s, s->eclTerm); }

void mqdc32_setEclTerm(struct s_mqdc32 * s, uint16_t term)
{
	SET16(s->md->vmeBase, MQDC32_ECL_TERMINATORS, term & MQDC32_ECL_TERMINATORS_MASK);
}

uint16_t mqdc32_getEclTerm(struct s_mqdc32 * s)
{
	return GET16(s->md->vmeBase, MQDC32_ECL_TERMINATORS) & MQDC32_ECL_TERMINATORS_MASK;
}

void mqdc32_setEclG1Osc_db(struct s_mqdc32 * s) { mqdc32_setEclG1Osc(s, s->eclG1Osc); }

void mqdc32_setEclG1Osc(struct s_mqdc32 * s, uint16_t go)
{
	SET16(s->md->vmeBase, MQDC32_ECL_G1_OSC, go & MQDC32_ECL_G1_OSC_MASK);
}

uint16_t mqdc32_getEclG1Osc(struct s_mqdc32 * s)
{
	return GET16(s->md->vmeBase, MQDC32_ECL_G1_OSC) & MQDC32_ECL_G1_OSC_MASK;
}

void mqdc32_setEclFclRts_db(struct s_mqdc32 * s) { mqdc32_setEclFclRts(s, s->eclFclRts); }

void mqdc32_setEclFclRts(struct s_mqdc32 * s, uint16_t fr)
{
	SET16(s->md->vmeBase, MQDC32_ECL_FCL_RTS, fr & MQDC32_ECL_FCL_RTS_MASK);
}

uint16_t mqdc32_getEclFclRts(struct s_mqdc32 * s)
{
	return GET16(s->md->vmeBase, MQDC32_ECL_FCL_RTS) & MQDC32_ECL_FCL_RTS_MASK;
}

void mqdc32_setGateSelect_db(struct s_mqdc32 * s) { mqdc32_setGateSelect(s, s->gateSelect); }

void mqdc32_setGateSelect(struct s_mqdc32 * s, uint16_t select)
{
	SET16(s->md->vmeBase, MQDC32_GATE_SELECT, select & MQDC32_GATE_SELECT_MASK);
}

uint16_t mqdc32_getGateSelect(struct s_mqdc32 * s)
{
	return GET16(s->md->vmeBase, MQDC32_GATE_SELECT) & MQDC32_GATE_SELECT_MASK;
}

void mqdc32_setNimG1Osc_db(struct s_mqdc32 * s) { mqdc32_setNimG1Osc(s, s->nimG1Osc); }

void mqdc32_setNimG1Osc(struct s_mqdc32 * s, uint16_t go)
{
	SET16(s->md->vmeBase, MQDC32_NIM_G1_OSC, go & MQDC32_NIM_G1_OSC_MASK);
}

uint16_t mqdc32_getNimG1Osc(struct s_mqdc32 * s)
{
	return GET16(s->md->vmeBase, MQDC32_NIM_G1_OSC) & MQDC32_NIM_G1_OSC_MASK;
}


void mqdc32_setNimFclRts_db(struct s_mqdc32 * s) { mqdc32_setNimFclRts(s, s->nimFclRts); }

void mqdc32_setNimFclRts(struct s_mqdc32 * s, uint16_t fr)
{
	SET16(s->md->vmeBase, MQDC32_NIM_FCL_RTS, fr & MQDC32_NIM_FCL_RTS_MASK);
}

uint16_t mqdc32_getNimFclRts(struct s_mqdc32 * s)
{
	return GET16(s->md->vmeBase, MQDC32_NIM_FCL_RTS) & MQDC32_NIM_FCL_RTS_MASK;
}

void mqdc32_setNimBusy_db(struct s_mqdc32 * s) { mqdc32_setNimBusy(s, s->nimBusy); }

void mqdc32_setNimBusy(struct s_mqdc32 * s, uint16_t busy)
{
	SET16(s->md->vmeBase, MQDC32_NIM_BUSY, busy & MQDC32_NIM_BUSY_MASK);
}

uint16_t mqdc32_getNimBusy(struct s_mqdc32 * s)
{
	return GET16(s->md->vmeBase, MQDC32_NIM_BUSY) & MQDC32_NIM_BUSY_MASK;
}

void mqdc32_setPulserStatus_db(struct s_mqdc32 * s) { mqdc32_setPulserStatus(s, s->pulserStatus); }

void mqdc32_setPulserStatus(struct s_mqdc32 * s, uint16_t mode)
{
	SET16(s->md->vmeBase, MQDC32_PULSER_STATUS, mode & MQDC32_PULSER_STATUS_MASK);
}

uint16_t mqdc32_getPulserStatus(struct s_mqdc32 * s)
{
	return GET16(s->md->vmeBase, MQDC32_PULSER_STATUS) & MQDC32_PULSER_STATUS_MASK;
}

void mqdc32_setTsSource_db(struct s_mqdc32 * s) { mqdc32_setTsSource(s, s->ctraTsSource); }

void mqdc32_setTsSource(struct s_mqdc32 * s, uint16_t source)
{
	SET16(s->md->vmeBase, MQDC32_CTRA_TS_SOURCE, source & MQDC32_CTRA_TS_SOURCE_MASK);
}

uint16_t mqdc32_getTsSource(struct s_mqdc32 * s)
{
	return GET16(s->md->vmeBase, MQDC32_CTRA_TS_SOURCE) & MQDC32_CTRA_TS_SOURCE_MASK;
}

void mqdc32_setTsDivisor_db(struct s_mqdc32 * s) { mqdc32_setTsDivisor(s, s->ctraTsDivisor); }

void mqdc32_setTsDivisor(struct s_mqdc32 * s, uint16_t div)
{
	SET16(s->md->vmeBase, MQDC32_CTRA_TS_DIVISOR, div & MQDC32_CTRA_TS_DIVISOR_MASK);
}

uint16_t mqdc32_getTsDivisor(struct s_mqdc32 * s)
{
	return GET16(s->md->vmeBase, MQDC32_CTRA_TS_DIVISOR) & MQDC32_CTRA_TS_DIVISOR_MASK;
}

void mqdc32_moduleInfo(struct s_mqdc32 * s)
{
	int firmware, mainRev, subRev;
	firmware = GET16(s->md->vmeBase, MQDC32_FIRMWARE_REV);
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

void mqdc32_setPrefix(struct s_mqdc32 * s, char * prefix)
{
	strcpy(s->prefix, prefix);
	strcpy(s->mpref, "");
}

bool_t mqdc32_fillStruct(struct s_mqdc32 * s, char * file)
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

	s->verbose = root_env_getval_b("MQDC32.VerboseMode", FALSE);

	s->dumpRegsOnInit = root_env_getval_b("MQDC32.DumpRegisters", FALSE);

	sp = root_env_getval_s("MQDC32.ModuleName", "");
	if (strcmp(s->moduleName, "mqdc32") != 0 && strcmp(sp, s->moduleName) != 0) {
		sprintf(msg, "[%sfill_struct] %s: File %s contains wrong module name - %s", s->mpref, s->moduleName, file, sp);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		return FALSE;
	}
	strcpy(mnUC, sp);
	mnUC[0] = toupper(mnUC[0]);

	sprintf(res, "MQDC32.%s.BlockXfer", mnUC);
	s->blockXfer = root_env_getval_b(res, FALSE);

	sprintf(res, "MADC32.%s.RepairRawData", mnUC);
	s->repairRawData = root_env_getval_b(res, FALSE);

	for (i = 0; i < MQDC_NOF_CHANNELS; i++) {
		sprintf(res, "MQDC32.%s.Thresh.%d", mnUC, i);
		s->threshold[i] = root_env_getval_i(res, MQDC32_THRESHOLD_DEFAULT);
	}

	sprintf(res, "MQDC32.%s.AddrSource", mnUC);
	s->addrSource = root_env_getval_i(res, MQDC32_ADDR_SOURCE_DEFAULT);

	if (s->addrSource == MQDC32_ADDR_SOURCE_REG) {
		sprintf(res, "MQDC32.%s.AddrReg", mnUC);
		s->addrReg = root_env_getval_i(res, 0);
		if (s->addrReg == 0) {
			sprintf(msg, "[%sfill_struct] %s: vme addr missing (addr source = reg)", s->mpref, s->moduleName);
			f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		}
	}

	sprintf(res, "MQDC32.%s.MCSTSignature", mnUC);
	s->mcstSignature = root_env_getval_x(res, 0x0);
	sprintf(res, "MQDC32.%s.CBLTSignature", mnUC);
	s->cbltSignature = root_env_getval_x(res, 0x0);
	sprintf(res, "MQDC32.%s.FirstInChain", mnUC);
	s->firstInChain = root_env_getval_b(res, FALSE);
	sprintf(res, "MQDC32.%s.LastInChain", mnUC);
	s->lastInChain = root_env_getval_b(res, FALSE);

	sprintf(res, "MQDC32.%s.ModuleId", mnUC);
	s->moduleId = root_env_getval_i(res, MQDC32_MODULE_ID_DEFAULT);

	sprintf(res, "MQDC32.%s.FifoLength", mnUC);
	s->fifoLength = root_env_getval_i(res, 0);

#if 0
	if (s->fifoLength == 0) {
		sprintf(msg, "[%sfill_struct] %s: fifo length missing", s->mpref, s->moduleName);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
	}
#endif

	sprintf(res, "MQDC32.%s.DataWidth", mnUC);
	s->dataWidth = root_env_getval_i(res, MQDC32_DATA_LENGTH_FORMAT_DEFAULT);

	sprintf(res, "MQDC32.%s.MultiEvent", mnUC);
	s->multiEvent = root_env_getval_i(res, MQDC32_MULTI_EVENT_DEFAULT);
	
	if ((s->multiEvent & MQDC32_MULTI_EVENT_BERR) == 0) {
		sprintf(msg, "[%sfill_struct] %s: Readout with BERR enabled", s->mpref, s->moduleName);
	} else {
		sprintf(msg, "[%sfill_struct] %s: Readout with EOB enabled", s->mpref, s->moduleName);
	}
	f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);

	sprintf(res, "MQDC32.%s.XferData", mnUC);
	s->xferData = root_env_getval_i(res, MQDC32_MAX_XFER_DATA_DEFAULT);

	sprintf(res, "MQDC32.%s.MarkingType", mnUC);
	s->markingType = root_env_getval_i(res, MQDC32_MARKING_TYPE_DEFAULT);

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
	s->ctraTsSource = root_env_getval_i(res, MQDC32_CTRA_TS_SOURCE_DEFAULT);

	sprintf(res, "MQDC32.%s.TsDivisor", mnUC);
	s->ctraTsDivisor = root_env_getval_i(res, MQDC32_CTRA_TS_DIVISOR_DEFAULT);

	return TRUE;
}

void mqdc32_loadFromDb(struct s_mqdc32 * s, uint32_t chnPattern)
{
	int ch;
	int b;
	uint32_t bit;

	mqdc32_setAddrReg_db(s);
	mqdc32_setMcstCblt_db(s);
	mqdc32_setModuleId_db(s);
	mqdc32_setDataWidth_db(s);
	mqdc32_setMultiEvent_db(s);
	mqdc32_setMarkingType_db(s);
	mqdc32_setXferData_db(s);
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
	mqdc32_setTsSource_db(s);
	mqdc32_setTsDivisor_db(s);

	bit = 1;
	for (ch = 0; ch < MQDC_NOF_CHANNELS; ch++) {
		if (chnPattern & bit) mqdc32_setThreshold_db(s, ch); else mqdc32_setThreshold(s, ch, MQDC32_D_CHAN_INACTIVE);
		bit <<= 1;
	}
}


bool_t mqdc32_dumpRegisters(struct s_mqdc32 * s, char * file)
{
	FILE * f;

	int ch;
	int b;

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

	fprintf(f, "Thresholds:\n");
	for (ch = 0; ch < MQDC_NOF_CHANNELS; ch++) fprintf(f, "   %2d: %d\n", ch, mqdc32_getThreshold(s, ch));

	fprintf(f, "Addr reg          : %#x\n", mqdc32_getAddrReg(s));
	mcstOrCblt = FALSE;
	if (flag = mqdc32_mcstIsEnabled(s))
					fprintf(f, "MCST signature [0x6024]   : %#x\n", mqdc32_getMcstSignature(s));
	else				fprintf(f, "MCST                      : disabled\n");
	mcstOrCblt |= flag;
	if (flag = mqdc32_cbltIsEnabled(s))
					fprintf(f, "CBLT signature [0x6022]   : %#x\n", mqdc32_getCbltSignature(s));
	else				fprintf(f, "CBLT                      : disabled\n");
	mcstOrCblt |= flag;
	if (mcstOrCblt) {
		if (mqdc32_isFirstInChain(s)) fprintf(f, "MCST/CBLT chain           : first module in chain\n");
		else if (mqdc32_isLastInChain(s)) fprintf(f, "MCST/CBLT chain           : last module in chain\n");
		else fprintf(f, "MCST/CBLT chain   : module in the middle\n");
	}
	fprintf(f, "Module ID         : %d\n", mqdc32_getModuleId(s));
	fprintf(f, "Data width        : %d\n", mqdc32_getDataWidth(s));
	fprintf(f, "Multi event       : %d\n", mqdc32_getMultiEvent(s));
	fprintf(f, "Max xfer data wc  : %d\n", mqdc32_getXferData(s));
	fprintf(f, "Marking type      : %d\n", mqdc32_getMarkingType(s));
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
	fprintf(f, "Timestamp source  : %#x\n", mqdc32_getTsSource(s));
	fprintf(f, "Timestamp divisor : %d\n", mqdc32_getTsDivisor(s));
	fclose(f);
}

bool_t mqdc32_dumpRaw(struct s_mqdc32 * s, char * file)
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

void mqdc32_printDb(struct s_mqdc32 * s)
{
	int ch;
	int b;

	printf("Thresholds:\n");
	for (ch = 0; ch < MQDC_NOF_CHANNELS; ch++) printf("   %2d: %d\n", ch, s->threshold[ch]);

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
	printf("Timestamp source  : %#x\n", s->ctraTsSource);
	printf("Timestamp divisor : %d\n", s->ctraTsDivisor);
}

int mqdc32_readout(struct s_mqdc32 * s, uint32_t * pointer)
{
	static int addrOffset = 0;

	uint32_t * dataStart;
	uint32_t * dp;
	uint32_t data;
	unsigned int i;
	int bmaError;
	int bmaCount;
	int tryIt;
	int numData, nd;
	int chn;

	uint32_t ptrloc;

	int mode;
	int sts;

	dataStart = pointer;

	numData = (int) mqdc32_getFifoLength(s);	
	
	if (numData == 0) return(0);

	if (tryIt <= 0) {
		sprintf(msg, "[%sreadout] %s: Error while reading event data (numData=%d != %d)", s->mpref, s->moduleName, numData, nd);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		return 0;
	}

	if (s->blockXfer) {
		ptrloc = getPhysAddr((char *) pointer, numData * sizeof(uint32_t));
		if (ptrloc == NULL) return(0);
		bmaError = bma_read(s->md->bltBase + MQDC32_DATA, ptrloc, numData, s->md->bltModeId);
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
	} else if ((s->multiEvent & MQDC32_MULTI_EVENT_BERR) == 0) {
		busError = FALSE;
		nd = 0;
		signal(SIGBUS, mqdc32_catchBerr);
		while (1) {
			nd++;
			data = GET32(s->md->vmeBase, MQDC32_DATA);
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
			data = GET32(s->md->vmeBase, MQDC32_DATA);
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

	if (s->repairRawData) pointer = mqdc32_repairRawData(s, pointer, dataStart);
	
	mqdc32_resetReadout(s);

	return (pointer - dataStart);
}


int mqdc32_readTimeB(struct s_mqdc32 * s, uint32_t * pointer)
{
	uint16_t * p16 = pointer;
	*p16++ = 0;
	*p16++ = GET16(s->md->vmeBase, MQDC32_CTRB_TIME_0);
	*p16++ = GET16(s->md->vmeBase, MQDC32_CTRB_TIME_1);
	*p16++ = GET16(s->md->vmeBase, MQDC32_CTRB_TIME_2);
	return 4 * sizeof(uint16_t) / sizeof(uint32_t);
}

bool_t mqdc32_dataReady(struct s_mqdc32 * s)
{
	return TSTB16(s->md->vmeBase, MQDC32_DATA_READY, 0) ;
}

void mqdc32_resetReadout(struct s_mqdc32 * s)
{
	SET16(s->md->vmeBase, MQDC32_READOUT_RESET, 0x1);
}

bool_t mqdc32_testBusError(struct s_mqdc32 * s)
{
	return TSTB16(s->md->vmeBase, MQDC32_MULTI_EVENT, MQDC32_MULTI_EVENT_BERR) == 1 ? FALSE : TRUE;
}

void mqdc32_enableBusError(struct s_mqdc32 * s)
{
	CLRB16(s->md->vmeBase, MQDC32_MULTI_EVENT, MQDC32_MULTI_EVENT_BERR);
}

void mqdc32_disableBusError(struct s_mqdc32 * s)
{
	SETB16(s->md->vmeBase, MQDC32_MULTI_EVENT, MQDC32_MULTI_EVENT_BERR);
}

void mqdc32_startAcq(struct s_mqdc32 * s)
{
	SET16(s->md->vmeBase, MQDC32_START_ACQUISITION, 0x0);
	mqdc32_resetFifo(s);
	mqdc32_resetReadout(s);
	mqdc32_resetTimestamp(s);
	SET16(s->md->vmeBase, MQDC32_START_ACQUISITION, 0x1);
}

void mqdc32_stopAcq(struct s_mqdc32 * s)
{
	SET16(s->md->vmeBase, MQDC32_START_ACQUISITION, 0x0);
	mqdc32_resetFifo(s);
	mqdc32_resetReadout(s);
	mqdc32_resetTimestamp(s);
}

void mqdc32_resetFifo(struct s_mqdc32 * s)
{
	SET16(s->md->vmeBase, MQDC32_FIFO_RESET, 0x1);
}

void mqdc32_resetTimestamp(struct s_mqdc32 * s)
{
	SET16(s->md->vmeBase, MQDC32_CTRA_RESET_A_OR_B, 0x3);
}

void mqdc32_setMcstCblt_db(struct s_mqdc32 * s) {
	mqdc32_setMcstAddr(s, s->mcstSignature);
	mqdc32_setCbltAddr(s, s->cbltSignature);
	if (s->firstInChain) mqdc32_setFirstInChain(s);
	else if (s->lastInChain) mqdc32_setLastInChain(s);
	else mqdc32_setMiddleOfChain(s);
}

void mqdc32_setMcstAddr(struct s_mqdc32 * s, unsigned long Signature) {
	SET16(s->md->vmeBase, MQDC32_MCST_ADDRESS, Signature);
	if (Signature != 0) {
		if (s->mcstAddr == 0) s->mcstAddr = mapAdditionalVME(s->md, (Signature & 0xFF) << 24, 0);
		if (s->mcstAddr) {
			mqdc32_setMcstEnable(s);
			sprintf(msg, "[%ssetMcstAddr] %s: MCST enabled - signature=%#x, addr=%#x", s->mpref, s->moduleName, Signature, s->mcstAddr);
		} else {
			mqdc32_setMcstDisable(s);
			sprintf(msg, "[%ssetMcstAddr] %s: MCST disabled - signature=%#x", s->mpref, s->moduleName, Signature);
		}
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
	} else {
		mqdc32_setMcstDisable(s);
	}
}

uint16_t mqdc32_getMcstSignature(struct s_mqdc32 * s) {
	uint16_t addr8;
	addr8 = GET16(s->md->vmeBase, MQDC32_MCST_ADDRESS);
	return addr8;
}

void mqdc32_setMcstEnable(struct s_mqdc32 * s) {
	SET16(s->md->vmeBase, MQDC32_CBLT_MCST_CONTROL, MQDC32_MCST_ENA);
}

void mqdc32_setMcstDisable(struct s_mqdc32 * s) {
	SET16(s->md->vmeBase, MQDC32_CBLT_MCST_CONTROL, MQDC32_MCST_DIS);
}

bool_t mqdc32_mcstIsEnabled(struct s_mqdc32 * s) {
	uint16_t ctrl;
	ctrl = GET16(s->md->vmeBase, MQDC32_CBLT_MCST_CONTROL);
	return ((ctrl & MQDC32_MCST_DIS) != 0);
}

void mqdc32_setCbltAddr(struct s_mqdc32 * s, unsigned long Signature) {
	SET16(s->md->vmeBase, MQDC32_CBLT_ADDRESS, Signature);
	if (Signature != 0) {
		if (s->cbltAddr == 0) s->cbltAddr = mapAdditionalVME(s->md, (Signature & 0xFF) << 24, 0);
		if (s->cbltAddr) {
			mqdc32_setCbltEnable(s);
			sprintf(msg, "[%ssetCbltAddr] %s: CBLT enabled - signature=%#x, addr=%#x", s->mpref, s->moduleName, Signature, s->cbltAddr);
		} else {
			mqdc32_setCbltDisable(s);
			sprintf(msg, "[%ssetCbltAddr] %s: CBLT disabled - signature=%#x", s->mpref, s->moduleName, Signature);
		}
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
	} else {
		mqdc32_setCbltDisable(s);
	}
}

uint16_t mqdc32_getCbltSignature(struct s_mqdc32 * s) {
	uint16_t addr8;
	addr8 = GET16(s->md->vmeBase, MQDC32_CBLT_ADDRESS);
	return addr8;
}

void mqdc32_setCbltEnable(struct s_mqdc32 * s) {
	SET16(s->md->vmeBase, MQDC32_CBLT_MCST_CONTROL, MQDC32_CBLT_ENA);
}

void mqdc32_setCbltDisable(struct s_mqdc32 * s) {
	SET16(s->md->vmeBase, MQDC32_CBLT_MCST_CONTROL, MQDC32_CBLT_DIS);
}

bool_t mqdc32_cbltIsEnabled(struct s_mqdc32 * s) {
	uint16_t ctrl;
	ctrl = GET16(s->md->vmeBase, MQDC32_CBLT_MCST_CONTROL);
	return ((ctrl & MQDC32_CBLT_DIS) != 0);
}

void mqdc32_setFirstInChain(struct s_mqdc32 * s) {
	SET16(s->md->vmeBase, MQDC32_CBLT_MCST_CONTROL, MQDC32_CBLT_FIRST_ENA);
	SET16(s->md->vmeBase, MQDC32_CBLT_MCST_CONTROL, MQDC32_CBLT_LAST_DIS);
}

bool_t mqdc32_isFirstInChain(struct s_mqdc32 * s) {
	uint16_t ctrl;
	ctrl = GET16(s->md->vmeBase, MQDC32_CBLT_MCST_CONTROL);
	return ((ctrl & MQDC32_CBLT_FIRST_DIS) != 0);
}

void mqdc32_setLastInChain(struct s_mqdc32 * s) {
	SET16(s->md->vmeBase, MQDC32_CBLT_MCST_CONTROL, MQDC32_CBLT_LAST_ENA);
	SET16(s->md->vmeBase, MQDC32_CBLT_MCST_CONTROL, MQDC32_CBLT_FIRST_DIS);
}

bool_t mqdc32_isLastInChain(struct s_mqdc32 * s) {
	uint16_t ctrl;
	ctrl = GET16(s->md->vmeBase, MQDC32_CBLT_MCST_CONTROL);
	return ((ctrl & MQDC32_CBLT_LAST_DIS) != 0);
}

void mqdc32_setMiddleOfChain(struct s_mqdc32 * s) {
	SET16(s->md->vmeBase, MQDC32_CBLT_MCST_CONTROL, MQDC32_CBLT_FIRST_DIS);
	SET16(s->md->vmeBase, MQDC32_CBLT_MCST_CONTROL, MQDC32_CBLT_LAST_DIS);
}

bool_t mqdc32_isMiddleOfChain(struct s_mqdc32 * s) {
	bool_t first, last;
	first = mqdc32_isFirstInChain(s);
	last = mqdc32_isLastInChain(s);
	return (!first && !last);
}

void mqdc32_startAcq_mcst(struct s_mqdc32 * s)
{
	SET16(s->mcstAddr, MQDC32_START_ACQUISITION, 0x0);
	mqdc32_resetFifo_mcst(s);
	mqdc32_resetReadout_mcst(s);
	mqdc32_resetTimestamp_mcst(s);
	SET16(s->mcstAddr, MQDC32_START_ACQUISITION, 0x1);
}

void mqdc32_stopAcq_mcst(struct s_mqdc32 * s)
{
	SET16(s->mcstAddr, MQDC32_START_ACQUISITION, 0x0);
	mqdc32_resetFifo_mcst(s);
	mqdc32_resetReadout_mcst(s);
	mqdc32_resetTimestamp_mcst(s);
}

void mqdc32_resetFifo_mcst(struct s_mqdc32 * s)
{
	SET16(s->mcstAddr, MQDC32_FIFO_RESET, 0x1);
}

void mqdc32_resetReadout_mcst(struct s_mqdc32 * s)
{
	SET16(s->mcstAddr, MQDC32_READOUT_RESET, 0x1);
}

void mqdc32_resetTimestamp_mcst(struct s_mqdc32 * s)
{
	SET16(s->mcstAddr, MQDC32_CTRA_RESET_A_OR_B, 0x3);
}

uint32_t * mqdc32_repairRawData(struct s_mqdc32 * s, uint32_t * pointer, uint32_t * dataStart) {
	return pointer;
}


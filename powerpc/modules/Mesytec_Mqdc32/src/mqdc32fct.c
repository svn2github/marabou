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
#include <math.h>
#include <string.h>
#include <allParam.h>
#include <ces/bmalib.h>
#include <errno.h>
#include <sigcodes.h>

#include "gd_readout.h"

#include "mqdc32.h"
#include "mqdc32_protos.h"

#include "root_env.h"

#include "err_mask_def.h"
#include "errnum_def.h"

void catchBerr();

char msg[256];
static struct dmachain *bma_page;

struct s_mqdc32 * mqdc32_alloc(unsigned long vmeAddr, volatile unsigned char * base, char * moduleName, int serial)
{
	struct s_mqdc32 * s;
	int firmware, mainRev;

	s = (struct s_mqdc32 *) calloc(1, sizeof(struct s_mqdc32));
	if (s != NULL) {
		s->baseAddr = base;
		s->vmeAddr = vmeAddr;
		strcpy(s->moduleName, moduleName);
		strcpy(s->prefix, "m_read_meb");
		strcpy(s->mpref, "mqdc32: ");
		s->serial = serial;
		s->verbose = FALSE;
		s->dumpRegsOnInit = FALSE;

		firmware = GET16(s->baseAddr, MQDC32_FIRMWARE_REV);
		mainRev = (firmware >> 8) & 0xff;
		s->memorySize = (mainRev >= 2) ? (8*1024 + 2) : (1024 + 2);
	} else {
		sprintf(msg, "[%salloc] %s: Can't allocate mqdc32 struct", s->mpref, s->moduleName);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
	}
	return s;
}

void mqdc32_initialize(struct s_mqdc32 * s)
{
	signal(SIGBUS, catchBerr);
	mqdc32_disableBusError(s);
	mqdc32_enable_bma(s);
	mqdc32_resetReadout(s);
}

void mqdc32_setBltBlockSize(struct s_mqdc32 * s, uint32_t size)
{
  if ((size * sizeof(uint32_t)) <= s->bltBufferSize) s->bltBlockSize = size;
}

void mqdc32_reset(struct s_mqdc32 * s)
{
  SET16(s->baseAddr, MQDC32_SOFT_RESET, 0x1);
}

uint16_t mqdc32_getThreshold(struct s_mqdc32 * s, uint16_t channel)
{
	uint16_t thresh = 0;
	if (channel < NOF_CHANNELS) thresh = GET16(s->baseAddr, MQDC32_THRESHOLD + sizeof(uint16_t) * channel) & MQDC32_THRESHOLD_MASK;
	return thresh;
}

void mqdc32_setThreshold_db(struct s_mqdc32 * s, uint16_t channel)
{
	if (channel < NOF_CHANNELS) mqdc32_setThreshold(s, channel, s->threshold[channel]);
}

void mqdc32_setThreshold(struct s_mqdc32 * s, uint16_t channel, uint16_t thresh)
{
	if (channel < NOF_CHANNELS) SET16(s->baseAddr, MQDC32_THRESHOLD + sizeof(uint16_t) * channel, thresh & MQDC32_THRESHOLD_MASK);
}

void mqdc32_setAddrReg_db(struct s_mqdc32 * s) { mqdc32_setAddrReg(s, s->addrReg); }

void mqdc32_setAddrReg(struct s_mqdc32 * s, uint16_t vmeAddr)
{
	if (vmeAddr) {
		SET16(s->baseAddr, MQDC32_ADDR_REG, vmeAddr);
		SET16(s->baseAddr, MQDC32_ADDR_SOURCE, MQDC32_ADDR_SOURCE_REG);
		s->baseAddr = vmeAddr;
	}
}

uint16_t mqdc32_getAddrReg(struct s_mqdc32 * s)
{
	uint16_t source = GET16(s->baseAddr, MQDC32_ADDR_SOURCE);
	if (source & MQDC32_ADDR_SOURCE_REG) return GET16(s->baseAddr, MQDC32_ADDR_REG);
	else return 0;
}

void mqdc32_setModuleId_db(struct s_mqdc32 * s) { mqdc32_setModuleId(s, s->moduleId); }

void mqdc32_setModuleId(struct s_mqdc32 * s, uint16_t id)
{
	SET16(s->baseAddr, MQDC32_MODULE_ID, id & MQDC32_MODULE_ID_MASK);
}

uint16_t mqdc32_getModuleId(struct s_mqdc32 * s)
{
	return GET16(s->baseAddr, MQDC32_MODULE_ID) & MQDC32_MODULE_ID_MASK;
}

uint16_t mqdc32_getFifoLength(struct s_mqdc32 * s)
{
	return GET16(s->baseAddr, MQDC32_BUFFER_DATA_LENGTH) & MQDC32_BUFFER_DATA_LENGTH_MASK;
}

void mqdc32_setDataWidth_db(struct s_mqdc32 * s) { mqdc32_setDataWidth(s, s->dataWidth); }

void mqdc32_setDataWidth(struct s_mqdc32 * s, uint16_t width)
{
	SET16(s->baseAddr, MQDC32_DATA_LENGTH_FORMAT, width & MQDC32_DATA_LENGTH_FORMAT_MASK);
}

uint16_t mqdc32_getDataWidth(struct s_mqdc32 * s)
{
	return GET16(s->baseAddr, MQDC32_DATA_LENGTH_FORMAT) & MQDC32_DATA_LENGTH_FORMAT_MASK;
}

void mqdc32_setMultiEvent_db(struct s_mqdc32 * s) { mqdc32_setMultiEvent(s, s->multiEvent); }

void mqdc32_setMultiEvent(struct s_mqdc32 * s, uint16_t mode)
{
	SET16(s->baseAddr, MQDC32_MULTI_EVENT, mode & MQDC32_MULTI_EVENT_MASK);
}

uint16_t mqdc32_getMultiEvent(struct s_mqdc32 * s)
{
	return GET16(s->baseAddr, MQDC32_MULTI_EVENT) & MQDC32_MULTI_EVENT_MASK;
}

void mqdc32_setXferData_db(struct s_mqdc32 * s) { mqdc32_setXferData(s, s->xferData); }

void mqdc32_setXferData(struct s_mqdc32 * s, uint16_t wc)
{
	SET16(s->baseAddr, MQDC32_MAX_XFER_DATA, wc & MQDC32_MAX_XFER_DATA_MASK);
}

uint16_t mqdc32_getXferData(struct s_mqdc32 * s)
{
	return GET16(s->baseAddr, MQDC32_MAX_XFER_DATA) & MQDC32_MAX_XFER_DATA_MASK;
}

void mqdc32_setMarkingType_db(struct s_mqdc32 * s) { mqdc32_setMarkingType(s, s->markingType); }

void mqdc32_setMarkingType(struct s_mqdc32 * s, uint16_t type)
{
	SET16(s->baseAddr, MQDC32_MARKING_TYPE, type & MQDC32_MARKING_TYPE_MASK);
}

uint16_t mqdc32_getMarkingType(struct s_mqdc32 * s)
{
	return GET16(s->baseAddr, MQDC32_MARKING_TYPE) & MQDC32_MARKING_TYPE_MASK;
}

void mqdc32_setBankOperation_db(struct s_mqdc32 * s) { mqdc32_setBankOperation(s, s->bankOperation); }

void mqdc32_setBankOperation(struct s_mqdc32 * s, uint16_t oper)
{
	SET16(s->baseAddr, MQDC32_BANK_OPERATION, oper & MQDC32_BANK_OPERATION_MASK);
}

uint16_t mqdc32_getBankOperation(struct s_mqdc32 * s)
{
	return GET16(s->baseAddr, MQDC32_BANK_OPERATION) & MQDC32_BANK_OPERATION_MASK;
}

void mqdc32_setAdcResolution_db(struct s_mqdc32 * s) { mqdc32_setAdcResolution(s, s->adcResolution); }

void mqdc32_setAdcResolution(struct s_mqdc32 * s, uint16_t res)
{
	SET16(s->baseAddr, MQDC32_ADC_RESOLUTION, res & MQDC32_ADC_RESOLUTION_MASK);
}

uint16_t mqdc32_getAdcResolution(struct s_mqdc32 * s)
{
	return GET16(s->baseAddr, MQDC32_ADC_RESOLUTION) & MQDC32_ADC_RESOLUTION_MASK;
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
	SET16(s->baseAddr, addr, offset & MQDC32_BANK_OFFSET_MASK);
}

uint16_t mqdc32_getBankOffset(struct s_mqdc32 * s, uint16_t bank)
{
	int addr;
	switch (bank) {
		case 0: addr = MQDC32_OFFSET_BANK_0; break;
		case 1: addr = MQDC32_OFFSET_BANK_1; break;
		default: return;
	}
	return GET16(s->baseAddr, addr) & MQDC32_BANK_OFFSET_MASK;
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
	SET16(s->baseAddr, addr, limit & MQDC32_GATE_LIMIT_MASK);
}

uint16_t mqdc32_getGateLimit(struct s_mqdc32 * s, uint16_t bank)
{
	int addr;
	switch (bank) {
		case 0: addr = MQDC32_GATE_LIMIT_BANK_0; break;
		case 1: addr = MQDC32_GATE_LIMIT_BANK_1; break;
		default: return;
	}
	return GET16(s->baseAddr, addr) & MQDC32_GATE_LIMIT_MASK;
}

void mqdc32_setSlidingScaleOff_db(struct s_mqdc32 * s) { mqdc32_setSlidingScaleOff(s, s->slidingScaleOff); }

void mqdc32_setSlidingScaleOff(struct s_mqdc32 * s, bool_t flag)
{
	uint16_t f = flag ? 1 : 0;
	SET16(s->baseAddr, MQDC32_SLIDING_SCALE_OFF, f);
}

bool_t mqdc32_getSlidingScaleOff(struct s_mqdc32 * s)
{
	uint16_t f = GET16(s->baseAddr, MQDC32_SLIDING_SCALE_OFF) & MQDC32_SLIDING_SCALE_OFF_MASK;
	return (f != 0) ? TRUE : FALSE;
}

void mqdc32_setSkipOutOfRange_db(struct s_mqdc32 * s) { mqdc32_setSkipOutOfRange(s, s->skipOutOfRange); }

void mqdc32_setSkipOutOfRange(struct s_mqdc32 * s, bool_t flag)
{
	uint16_t f = flag ? 1 : 0;
	SET16(s->baseAddr, MQDC32_SKIP_OUT_OF_RANGE, f);
}

bool_t mqdc32_getSkipOutOfRange(struct s_mqdc32 * s)
{
	uint16_t f = GET16(s->baseAddr, MQDC32_SKIP_OUT_OF_RANGE) & MQDC32_SKIP_OUT_OF_RANGE_MASK;
	return (f != 0) ? TRUE : FALSE;
}

void mqdc32_setIgnoreThresholds_db(struct s_mqdc32 * s) { mqdc32_setIgnoreThresholds(s, s->ignoreThresh); }

void mqdc32_setIgnoreThresholds(struct s_mqdc32 * s, bool_t flag)
{
	uint16_t f = flag ? 1 : 0;
	SET16(s->baseAddr, MQDC32_IGNORE_THRESHOLDS, f);
}

bool_t mqdc32_getIgnoreThresholds(struct s_mqdc32 * s)
{
	uint16_t f = GET16(s->baseAddr, MQDC32_IGNORE_THRESHOLDS) & MQDC32_IGNORE_THRESH_MASK;
	return (f != 0) ? TRUE : FALSE;
}

void mqdc32_setInputCoupling_db(struct s_mqdc32 * s) { mqdc32_setInputCoupling(s, s->inputCoupling); }

void mqdc32_setInputCoupling(struct s_mqdc32 * s, uint16_t coupling)
{
	SET16(s->baseAddr, MQDC32_INPUT_COUPLING, coupling & MQDC32_INPUT_COUPLING_MASK);
}

uint16_t mqdc32_getInputCoupling(struct s_mqdc32 * s)
{
	return GET16(s->baseAddr, MQDC32_INPUT_COUPLING) & MQDC32_INPUT_COUPLING_MASK;
}

void mqdc32_setEclTerm_db(struct s_mqdc32 * s) { mqdc32_setEclTerm(s, s->eclTerm); }

void mqdc32_setEclTerm(struct s_mqdc32 * s, uint16_t term)
{
	SET16(s->baseAddr, MQDC32_ECL_TERMINATORS, term & MQDC32_ECL_TERMINATORS_MASK);
}

uint16_t mqdc32_getEclTerm(struct s_mqdc32 * s)
{
	return GET16(s->baseAddr, MQDC32_ECL_TERMINATORS) & MQDC32_ECL_TERMINATORS_MASK;
}

void mqdc32_setEclG1OrOsc_db(struct s_mqdc32 * s) { mqdc32_setEclG1OrOsc(s, s->eclG1OrOsc); }

void mqdc32_setEclG1OrOsc(struct s_mqdc32 * s, uint16_t go)
{
	SET16(s->baseAddr, MQDC32_ECL_G1_OR_OSC, go & MQDC32_ECL_G1_OR_OSC_MASK);
}

uint16_t mqdc32_getEclG1OrOsc(struct s_mqdc32 * s)
{
	return GET16(s->baseAddr, MQDC32_ECL_G1_OR_OSC) & MQDC32_ECL_G1_OR_OSC_MASK;
}

void mqdc32_setEclFclOrRts_db(struct s_mqdc32 * s) { mqdc32_setEclFclOrRts(s, s->eclFclOrRts); }

void mqdc32_setEclFclOrRts(struct s_mqdc32 * s, uint16_t fr)
{
	SET16(s->baseAddr, MQDC32_ECL_FCL_OR_RES_TS, fr & MQDC32_ECL_FCL_OR_RES_TS_MASK);
}

uint16_t mqdc32_getEclFclOrRts(struct s_mqdc32 * s)
{
	return GET16(s->baseAddr, MQDC32_ECL_FCL_OR_RES_TS) & MQDC32_ECL_FCL_OR_RES_TS_MASK;
}

void mqdc32_setGateSelect_db(struct s_mqdc32 * s) { mqdc32_setGateSelect(s, s->gateSelect); }

void mqdc32_setGateSelect(struct s_mqdc32 * s, uint16_t select)
{
	SET16(s->baseAddr, MQDC32_GATE_SELECT, select & MQDC32_GATE_SELECT_MASK);
}

uint16_t mqdc32_getGateSelect(struct s_mqdc32 * s)
{
	return GET16(s->baseAddr, MQDC32_GATE_SELECT) & MQDC32_GATE_SELECT_MASK;
}

void mqdc32_setNimG1OrOsc_db(struct s_mqdc32 * s) { mqdc32_setNimG1OrOsc(s, s->nimG1OrOsc); }

void mqdc32_setNimG1OrOsc(struct s_mqdc32 * s, uint16_t go)
{
	SET16(s->baseAddr, MQDC32_NIM_G1_OR_OSC, go & MQDC32_NIM_G1_OR_OSC_MASK);
}

uint16_t mqdc32_getNimG1OrOsc(struct s_mqdc32 * s)
{
	return GET16(s->baseAddr, MQDC32_NIM_G1_OR_OSC) & MQDC32_NIM_G1_OR_OSC_MASK;
}


void mqdc32_setNimFclOrRts_db(struct s_mqdc32 * s) { mqdc32_setNimFclOrRts(s, s->nimFclOrRts); }

void mqdc32_setNimFclOrRts(struct s_mqdc32 * s, uint16_t fr)
{
	SET16(s->baseAddr, MQDC32_NIM_FCL_OR_RES_TS, fr & MQDC32_NIM_FCL_OR_RES_TS_MASK);
}

uint16_t mqdc32_getNimFclOrRts(struct s_mqdc32 * s)
{
	return GET16(s->baseAddr, MQDC32_NIM_FCL_OR_RES_TS) & MQDC32_NIM_FCL_OR_RES_TS_MASK;
}

void mqdc32_setNimBusy_db(struct s_mqdc32 * s) { mqdc32_setNimBusy(s, s->nimBusy); }

void mqdc32_setNimBusy(struct s_mqdc32 * s, uint16_t busy)
{
	SET16(s->baseAddr, MQDC32_NIM_BUSY, busy & MQDC32_NIM_BUSY_MASK);
}

uint16_t mqdc32_getNimBusy(struct s_mqdc32 * s)
{
	return GET16(s->baseAddr, MQDC32_NIM_BUSY) & MQDC32_NIM_BUSY_MASK;
}

void mqdc32_setTestPulser_db(struct s_mqdc32 * s) { mqdc32_setTestPulser(s, s->testPulserStatus); }

void mqdc32_setTestPulser(struct s_mqdc32 * s, uint16_t mode)
{
	SET16(s->baseAddr, MQDC32_TEST_PULSER_STATUS, mode & MQDC32_TEST_PULSER_STATUS_MASK);
}

uint16_t mqdc32_getTestPulser(struct s_mqdc32 * s)
{
	return GET16(s->baseAddr, MQDC32_TEST_PULSER_STATUS) & MQDC32_TEST_PULSER_STATUS_MASK;
}

void mqdc32_setTsSource_db(struct s_mqdc32 * s) { mqdc32_setTsSource(s, s->ctraTsSource); }

void mqdc32_setTsSource(struct s_mqdc32 * s, uint16_t source)
{
	SET16(s->baseAddr, MQDC32_CTRA_TS_SOURCE, source & MQDC32_CTRA_TS_SOURCE_MASK);
}

uint16_t mqdc32_getTsSource(struct s_mqdc32 * s)
{
	return GET16(s->baseAddr, MQDC32_CTRA_TS_SOURCE) & MQDC32_CTRA_TS_SOURCE_MASK;
}

void mqdc32_setTsDivisor_db(struct s_mqdc32 * s) { mqdc32_setTsDivisor(s, s->ctraTsDivisor); }

void mqdc32_setTsDivisor(struct s_mqdc32 * s, uint16_t div)
{
	SET16(s->baseAddr, MQDC32_CTRA_TS_DIVISOR, div & MQDC32_CTRA_TS_DIVISOR_MASK);
}

uint16_t mqdc32_getTsDivisor(struct s_mqdc32 * s)
{
	return GET16(s->baseAddr, MQDC32_CTRA_TS_DIVISOR) & MQDC32_CTRA_TS_DIVISOR_MASK;
}

void mqdc32_moduleInfo(struct s_mqdc32 * s)
{
	int firmware, mainRev, subRev;
	firmware = GET16(s->baseAddr, MQDC32_FIRMWARE_REV);
	mainRev = (firmware >> 8) & 0xff;
	subRev = firmware & 0xff;
	sprintf(msg, "[%smodule_info] %s: phys addr %#lx, mapped to %#lx, firmware %02x.%02x (%dk memory)",
									s->mpref,
									s->moduleName,
									s->vmeAddr,
									s->baseAddr,
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

	s->updSettings = root_env_getval_b("MQDC32.UpdateSettings", FALSE);
	s->updInterval = root_env_getval_i("MQDC32.UpdateInterval", 0);
	s->updCountDown = 0;

	sp = root_env_getval_s("MQDC32.ModuleName", "");
	if (strcmp(s->moduleName, "mqdc32") != 0 && strcmp(sp, s->moduleName) != 0) {
		sprintf(msg, "[%sfill_struct] %s: File %s contains wrong module name - %s", s->mpref, s->moduleName, file, sp);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		return FALSE;
	}
	strcpy(mnUC, sp);
	mnUC[0] = toupper(mnUC[0]);

	sprintf(res, "MQDC32.%s.BlockXfer", mnUC);
	s->blockXfer = root_env_getval_i(res, 0);

	for (i = 0; i < NOF_CHANNELS; i++) {
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

	sprintf(res, "MQDC32.%s.EclG1OrOsc", mnUC);
	s->eclG1OrOsc = root_env_getval_i(res, MQDC32_ECL_G1_OR_OSC_DEFAULT);

	sprintf(res, "MQDC32.%s.EclFclOrRts", mnUC);
	s->eclFclOrRts = root_env_getval_i(res, MQDC32_NIM_FCL_OR_RES_TS_DEFAULT);

	sprintf(res, "MQDC32.%s.GateSelect", mnUC);
	s->eclFclOrRts = root_env_getval_i(res, MQDC32_GATE_SELECT_DEFAULT);

	sprintf(res, "MQDC32.%s.NimG1OrOsc", mnUC);
	s->nimG1OrOsc = root_env_getval_i(res, MQDC32_NIM_G1_OR_OSC_DEFAULT);

	sprintf(res, "MQDC32.%s.NimFclOrRts", mnUC);
	s->nimFclOrRts = root_env_getval_i(res, MQDC32_NIM_FCL_OR_RES_TS_DEFAULT);

	sprintf(res, "MQDC32.%s.NimBusy", mnUC);
	s->nimBusy = root_env_getval_i(res, MQDC32_NIM_BUSY_DEFAULT);

	sprintf(res, "MQDC32.%s.PulserStatus", mnUC);
	s->testPulserStatus = root_env_getval_i(res, MQDC32_TEST_PULSER_STATUS_DEFAULT);

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
	mqdc32_setEclG1OrOsc_db(s);
	mqdc32_setEclFclOrRts_db(s);
	mqdc32_setGateSelect_db(s);
	mqdc32_setNimG1OrOsc_db(s);
	mqdc32_setNimFclOrRts_db(s);
	mqdc32_setNimBusy_db(s);
	mqdc32_setTestPulser_db(s);
	mqdc32_setTsSource_db(s);
	mqdc32_setTsDivisor_db(s);

	bit = 1;
	for (ch = 0; ch < NOF_CHANNELS; ch++) {
		if (chnPattern & bit) mqdc32_setThreshold_db(s, ch); else mqdc32_setThreshold(s, ch, MQDC32_D_CHAN_INACTIVE);
		bit <<= 1;
	}
}


bool_t mqdc32_dumpRegisters(struct s_mqdc32 * s, char * file)
{
	FILE * f;

	int ch;
	int b;

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
	for (ch = 0; ch < NOF_CHANNELS; ch++) fprintf(f, "   %2d: %d\n", ch, mqdc32_getThreshold(s, ch));

	fprintf(f, "Addr reg          : %#x\n", mqdc32_getAddrReg(s));
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
	fprintf(f, "Ecl gate or osc   : %d\n", mqdc32_getEclG1OrOsc(s));
	fprintf(f, "Ecl fcl or reset  : %d\n", mqdc32_getEclFclOrRts(s));
	fprintf(f, "Gate select       : %d\n", mqdc32_getGateSelect(s));
	fprintf(f, "Nim gate or osc   : %d\n", mqdc32_getNimG1OrOsc(s));
	fprintf(f, "Nim fcl or reset  : %d\n", mqdc32_getNimFclOrRts(s));
	fprintf(f, "Nim busy          : %d\n", mqdc32_getNimBusy(s));
	fprintf(f, "Pulser status     : %d\n", mqdc32_getTestPulser(s));
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
		fprintf(f, "%#lx %#x\n", i, GET16(s->baseAddr, i));
	}
	fclose(f);
}

void mqdc32_printDb(struct s_mqdc32 * s)
{
	int ch;
	int b;

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
	printf("Ecl gate or osc   : %d\n", s->eclG1OrOsc);
	printf("Ecl fcl or reset  : %d\n", s->eclFclOrRts);
	printf("Gate select       : %d\n", s->gateSelect);
	printf("Nim gate or osc   : %d\n", s->nimG1OrOsc);
	printf("Nim fcl or reset  : %d\n", s->nimFclOrRts);
	printf("Nim busy          : %d\n", s->nimBusy);
	printf("Pulser status     : %d\n", s->testPulserStatus);
	printf("Timestamp source  : %#x\n", s->ctraTsSource);
	printf("Timestamp divisor : %d\n", s->ctraTsDivisor);
}

void mqdc32_enable_bma(struct s_mqdc32 * s)
{
/* use blocktransfer */
/* buffersize (in 32bit words) : Memory     = 1026 * 4    */

	int bmaError;

	if (s->blockXfer == MQDC32_BLT_NORMAL) {
		s->bltBufferSize = s->memorySize;
		s->bltBuffer = calloc(s->memorySize, sizeof(uint32_t));

		bmaError = bma_open();
		if (bmaError != 0) {
			sprintf(msg, "[%senable_bma] %s: %s, turning block xfer OFF - %s (%d)", s->mpref, s->moduleName,  sys_errlist[errno], errno);
			f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
			s->blockXfer = MQDC32_BLT_OFF;
			bma_close();
			return;
		}

		bma_set_mode(BMA_DEFAULT_MODE, BMA_M_VMESize, BMA_M_Vsz32);
		bma_set_mode(BMA_DEFAULT_MODE, BMA_M_WordSize, BMA_M_WzD32);
		bma_set_mode(BMA_DEFAULT_MODE, BMA_M_AmCode, BMA_M_AmA32U);
#ifdef CPU_TYPE_RIO3
		bma_set_mode(BMA_DEFAULT_MODE, BMA_M_VMEAdrInc, BMA_M_VaiFifo);

		bma_page = (struct dmachain *) malloc (sizeof(struct dmachain *) * 1000);
		bmaError = vmtopm(getpid(), bma_page, (char *) s->bltBuffer, s->memorySize * sizeof(uint32_t));
		if (bmaError != 0) {
			sprintf(msg, "[%sreadout] %s: %s (%d) when calling vmtopm() - turning block xfer OFF", s->mpref, s->moduleName, sys_errlist[errno], errno);
			f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
			s->blockXfer = MQDC32_BLT_OFF;
			bma_close();
			return;
		}
		s->bltDestination = (uint32_t) bma_page->address;
#else
		s->bltDestination = (uint32_t) s->bltBuffer;
#endif

		sprintf(msg, "[%senable_bma] %s: turning block xfer ON (mode=NORMAL, buffer=%#lx, size=%d)", s->mpref, s->moduleName, s->bltBuffer, s->bltBufferSize);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
	} else if (s->blockXfer == MQDC32_BLT_CHAINED) {
		s->bltBufferSize = s->memorySize;
		if ((s->bma = bmaAlloc(s->bltBufferSize)) == NULL) {
			sprintf(msg, "[%senable_bma] %s: %s, turning block xfer OFF - %s (%d)", s->mpref, s->moduleName,  sys_errlist[errno], errno);
			f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
			s->blockXfer = MQDC32_BLT_OFF;
			getchar();
			return;
		}
		s->bltBuffer = (unsigned char *) s->bma->virtBase;
		sprintf(msg, "[%senable_bma] %s: turning block xfer ON (mode=CHAINED, buffer=%#lx, size=%d)", s->mpref, s->moduleName, s->bltBuffer, s->bltBufferSize);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
	} else {
		s->blockXfer = MQDC32_BLT_OFF;
		s->bma = NULL;
		s->bltBufferSize = 0;
		s->bltBuffer = NULL;
		sprintf(msg, "[%senable_bma] %s: Block xfer is OFF", s->mpref, s->moduleName);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
	}
}

int mqdc32_readout(struct s_mqdc32 * s, uint32_t * pointer)
{
	uint32_t * dataStart;
	uint32_t data;
	unsigned int i;
	int bmaError;
	int tryIt;
	int numData;

	dataStart = pointer;

	tryIt = 20;
	while (tryIt-- && !mqdc32_dataReady(s)) { usleep(1000); }
	if (tryIt <= 0) {
		*pointer++ = 0xaffec0c0;
		mqdc32_resetFifo(s);
		mqdc32_startAcq(s);
		return (pointer - dataStart);
	}

	numData = (int) mqdc32_getFifoLength(s);

	if (s->blockXfer == MQDC32_BLT_NORMAL) {
		bmaError = bma_read((unsigned long) s->vmeAddr + MQDC32_DATA, s->bltDestination, numData, BMA_DEFAULT_MODE);
		if (bmaError != 0) {
			sprintf(msg, "[%sreadout] %s: %s (%d) while reading event data", s->mpref, s->moduleName, sys_errlist[errno], errno);
			f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
			return(0);
		}

		bmaError = bma_wait(BMA_DEFAULT_MODE);
		if (bmaError != 0) {
			sprintf(msg, "[%sreadout] %s: %s (%d) while waiting for block xfer", s->mpref, s->moduleName, sys_errlist[errno], errno);
			f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
			return(0);
		}
		memcpy(pointer, s->bltBuffer, sizeof(uint32_t) * numData);
		pointer += numData;
	} else if (s->blockXfer == MQDC32_BLT_CHAINED) {
		if (bmaResetChain(s->bma) < 0) {
		 sprintf(msg, "[%sreadout] %s: %s (%d) when resetting chain", s->mpref, s->moduleName, sys_errlist[errno], errno);
		 f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		 return(0);
		}

		if (bmaReadChain(s->bma, 0, s->vmeAddr + MQDC32_DATA, numData, 0x0b) < 0) {
		  sprintf(msg, "[%sreadout] %s: %s (%d) while reading event data", s->mpref, s->moduleName, sys_errlist[errno], errno);
		  f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		  return(0);
		}

		if (bmaStartChain(s->bma) < 0) {
		  sprintf(msg, "[%sreadout] %s: %s (%d) when starting chain", s->mpref, s->moduleName, sys_errlist[errno], errno);
		  f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		  return(0);
		}

		if (bmaWaitChain(s->bma) < 0) {
		  sprintf(msg, "[%sreadout] %s: %s (%d) while waiting for chained xfer", s->mpref, s->moduleName, sys_errlist[errno], errno);
		  f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		  return(0);
		}
		memcpy(pointer, s->bltBuffer, sizeof(uint32_t) * numData);
		pointer += numData;
	} else {
		s->blockXfer = MQDC32_BLT_OFF;
		for (i = 0; i < numData; i++) {
			data = GET32(s->baseAddr, MQDC32_DATA);
			if (data == 0) {
				printf("data=0!\n");
				s->evtp++; *s->evtp = (MQDC32_M_TRAILER | 0x00525252);
				pointer = mqdc32_pushEvent(s, pointer);
				mqdc32_resetEventBuffer(s);
				s->skipData = TRUE;
				continue;
			} else if ((data & MQDC32_M_SIGNATURE) == MQDC32_M_HEADER) {
				s->skipData = FALSE;
				if (s->evtp != s->evtBuf) {
					s->evtp++; *s->evtp = (MQDC32_M_TRAILER | 0x00252525);
					pointer = mqdc32_pushEvent(s, pointer);
				}
				mqdc32_resetEventBuffer(s);
				*s->evtp = data;
			} else if ((data & MQDC32_M_SIGNATURE) == MQDC32_M_TRAILER) {
				if (s->skipData) continue;
				s->evtp++; *s->evtp = data;
				pointer = mqdc32_pushEvent(s, pointer);
				mqdc32_resetEventBuffer(s);
			} else if ((data & MQDC32_M_SIGNATURE) == MQDC32_M_EOB) {
				break;
			} else {
				if (s->skipData) continue;
				s->evtp++; *s->evtp = data;
			}
		}
	}

	mqdc32_resetReadout(s);

	return (pointer - dataStart);
}

uint32_t * mqdc32_pushEvent(struct s_mqdc32 * s, uint32_t * pointer) {
	int i;
	int wc = s->evtp - s->evtBuf + 1;
	uint32_t * p = s->evtBuf;
	for (i = 0; i < wc; i++) *pointer++ = *p++;
	return (pointer);
}

int mqdc32_readTimeB(struct s_mqdc32 * s, uint32_t * pointer)
{
	uint16_t * p16 = pointer;
	*p16++ = 0;
	*p16++ = GET16(s->baseAddr, MQDC32_CTRB_TIME_0);
	*p16++ = GET16(s->baseAddr, MQDC32_CTRB_TIME_1);
	*p16++ = GET16(s->baseAddr, MQDC32_CTRB_TIME_2);
	return 4 * sizeof(uint16_t) / sizeof(uint32_t);
}

bool_t mqdc32_dataReady(struct s_mqdc32 * s)
{
	return TSTB16(s->baseAddr, MQDC32_DATA_READY, 0) ;
}

void mqdc32_resetReadout(struct s_mqdc32 * s)
{
	SET16(s->baseAddr, MQDC32_READOUT_RESET, 0x1);
}

bool_t mqdc32_testBusError(struct s_mqdc32 * s)
{
	return TSTB16(s->baseAddr, MQDC32_MULTI_EVENT, MQDC32_MULTI_EVENT_BERR) == 1 ? FALSE : TRUE;
}

void mqdc32_enableBusError(struct s_mqdc32 * s)
{
	CLRB16(s->baseAddr, MQDC32_MULTI_EVENT, MQDC32_MULTI_EVENT_BERR);
}

void mqdc32_disableBusError(struct s_mqdc32 * s)
{
	SETB16(s->baseAddr, MQDC32_MULTI_EVENT, MQDC32_MULTI_EVENT_BERR);
}

void mqdc32_startAcq(struct s_mqdc32 * s)
{
	SET16(s->baseAddr, MQDC32_START_ACQUISITION, 0x0);
	mqdc32_resetFifo(s);
	mqdc32_resetReadout(s);
	mqdc32_resetEventBuffer(s);
	SET16(s->baseAddr, MQDC32_START_ACQUISITION, 0x1);
}

void mqdc32_stopAcq(struct s_mqdc32 * s)
{
	SET16(s->baseAddr, MQDC32_START_ACQUISITION, 0x0);
	mqdc32_resetFifo(s);
	mqdc32_resetReadout(s);
	mqdc32_resetEventBuffer(s);
}

void mqdc32_resetFifo(struct s_mqdc32 * s)
{
	SET16(s->baseAddr, MQDC32_FIFO_RESET, 0x1);
}

bool_t mqdc32_updateSettings(struct s_mqdc32 * s, char * updFile)
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

void mqdc32_resetEventBuffer(struct s_mqdc32 * s) {
	memset(s->evtBuf, 0, sizeof(s->evtBuf));
	s->evtp = s->evtBuf;
	s->skipData = FALSE;
}

void catchBerr() {
	printf("BUS ERROR\n");
}
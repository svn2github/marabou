/*_______________________________________________________[C LIBRARY FUNCTIONS]
//////////////////////////////////////////////////////////////////////////////
//! \file			caen_v1190fct.c
//! \brief			Code for module caen_v1190
//! \details		Implements functions to handle modules of type Mesytec caen_v1190
//!
//! $Author: Rudolf.Lutter $
//! $Mail			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.1 $       
//! $Date: 2009-06-30 13:14:53 $
////////////////////////////////////////////////////////////////////////////*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <allParam.h>
#include <ces/bmalib.h>
#include <errno.h>

#include "gd_readout.h"

#include "CAEN_V1190.h"
#include "caen_v1190_protos.h"

#include "root_env.h"

#include "err_mask_def.h"
#include "errnum_def.h"

char msg[256];

struct s_caen_v1190 * caen_v1190_alloc(unsigned long vmeAddr, volatile unsigned char * base, char * moduleName, int serial)
{
	struct s_caen_v1190 * s;
	s = (struct s_caen_v1190 *) calloc(1, sizeof(struct s_caen_v1190));
	if (s != NULL) {
		s->baseAddr = base;
		s->vmeAddr = vmeAddr;
		strcpy(s->moduleName, moduleName);
		strcpy(s->prefix, "m_read_meb");
		strcpy(s->mpref, "caen_v1190: ");
		s->serial = serial;
		s->verbose = FALSE;
		s->dumpRegsOnInit = FALSE;
	} else {
		sprintf(msg, "[%salloc] %s: Can't allocate caen_v1190 struct", s->mpref, s->moduleName);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
	}
	return s;
}

void caen_v1190_setTriggerMatchingOn_db(struct s_caen_v1190 * s) { caen_v1190_setTriggerMatchingOn90_setAddrReg(s, s->trigMatchingOn); };

void caen_v1190_setTriggerMatchingOn(struct s_caen_v1190 * s, bool_t flag)
{
	uint_16 opCode = flag ? CAEN_V1190_OP_TRG_MATCH : CAEN_V1190_OP_CONT_STOR;
	caen_v1190_writeMicro(s->baseAddr, opCode);
	s->trigMatchingOn = flag;
}

bool_t caen_v1190_triggerMatchingIsOn(struct s_caen_v1190 * s)
{
	caen_v1190_writeMicro(s->baseAddr, CAEN_V1190_OP_READ_ACQ_MOD);
	s->trigMatchingOn = (caen_v1190_readMicro(s->baseAddr) != 0);
	return(s->trigMatchingOn);
}

void caen_v1190_setWindowWidth_db(struct s_caen_v1190 * s) { caen_v1190_setWindowWidth(s, s->windowWidth); };

void caen_v1190_setWindowWidth(struct s_caen_v1190 * s, uint16_t ticks)
{
	caen_v1190_writeMicro(s->baseAddr, CAEN_V1190_OP_SET_WIN_WIDTH);
	caen_v1190_writeMicro(s->baseAddr, ticks);
	s->windowWidth = ticks;
}

uint16_t caen_v1190_getWindowWidth(struct s_caen_v1190 * s) { return(s->windowWidth); };

void caen_v1190_setWindowOffset_db(struct s_caen_v1190 * s) { caen_v1190_setWindowWidth(s, s->windowOffset); };

void caen_v1190_setWindowOffset(struct s_caen_v1190 * s, uint16_t ticks)
{
	caen_v1190_writeMicro(s->baseAddr, CAEN_V1190_OP_SET_WIN_OFFS);
	caen_v1190_writeMicro(s->baseAddr, ticks);
	s->windowOffset = ticks;
}

uint16_t caen_v1190_getWindowOffset(struct s_caen_v1190 * s) { return(s->windowOffset); };

void caen_v1190_setRejectMargin_db(struct s_caen_v1190 * s) { caen_v1190_setWindowWidth(s, s->rejectMargin); };

void caen_v1190_setRejectMargin(struct s_caen_v1190 * s, uint16_t ticks)
{
	caen_v1190_writeMicro(s->baseAddr, CAEN_V1190_OP_SET_REJ_MARGIN);
	caen_v1190_writeMicro(s->baseAddr, ticks);
	s->rejectMargin = ticks;
}

uint16_t caen_v1190_getRejectMargin(struct s_caen_v1190 * s) { return(s->rejectMargin); };

void caen_v1190_setSearchMargin_db(struct s_caen_v1190 * s) { caen_v1190_setWindowWidth(s, s->searchMargin); };

void caen_v1190_setSearchMargin(struct s_caen_v1190 * s, uint16_t ticks) {
	caen_v1190_writeMicro(s->baseAddr, CAEN_V1190_OP_SET_SW_MARGIN);
	caen_v1190_writeMicro(s->baseAddr, ticks);
	s->searchMargin = ticks;
}

uint16_t caen_v1190_getSearchMargin(struct s_caen_v1190 * s) { return(s->searchMargin); };

void caen_v1190_enableTriggerTimeSubtraction_db(struct s_caen_v1190 * s) { caen_v1190_setWindowWidth(s, s->enaTrigSubtraction); };

void caen_v1190_enableTriggerTimeSubtraction(struct s_caen_v1190 * s, bool_t flag)
{
	uint_16 opCode = flag ? CAEN_V1190_OP_EN_SUB_TRG : CAEN_V1190_OP_DIS_SUB_TRG;
	caen_v1190_writeMicro(s->baseAddr, opCode);
	s->enaTrigSubtraction = flag;
}

bool_t caen_v1190_triggerTimeSubtractionEnabled(struct s_caen_v1190 * s) { return(s->enaTrigSubtraction); };

void caen_v1190_updateTriggerConf(struct s_caen_v1190 * s) {
	caen_v1190_writeMicro(s->baseAddr, CAEN_V1190_OP_READ_TRG_CONF);
	s->windowWidth = caen_v1190_readMicro(s->baseAddr);
	s->windowOffset = caen_v1190_readMicro(s->baseAddr);
	s->searchMargin = caen_v1190_readMicro(s->baseAddr);
	s->rejectMargin = caen_v1190_readMicro(s->baseAddr);
	s->enaTrigSubtraction = (caen_v1190_readMicro(s->baseAddr) != 0)
}

void caen_v1190_setResolution_db(struct s_caen_v1190 * s) { caen_v1190_setResolution(s, s->edgeDetectionMode, s->resolution, s->pairResWidth); };

void caen_v1190_setResolution(struct s_caen_v1190 * s, uint16_t mode, uint16_t resolution, uint16_t pairWidth)
{
	uint16_t pairRes;

	caen_v1190_writeMicro(s->baseAddr, CAEN_V1190_OP_SET_DETECTION);
	caen_v1190_writeMicro(s->baseAddr, mode);

	if (mode == CAEN_V1190_B_EDGE_PAIR) {
		pairRes = (pairWidth & CAEN_V1190_M_EDGE_PAIR_WIDTH) << CAEN_V1190_SH_EDGE_PAIR_WIDTH + (resolution & CAEN_V1190_M_EDGE_RESOLUTION);
		caen_v1190_writeMicro(s->baseAddr, CAEN_V1190_OP_SET_PAIR_RES);
		caen_v1190_writeMicro(s->baseAddr, pairRes);
		s->pairResolution = resolution;
		s->pairWidth = pairWidth;
	} else {
		caen_v1190_writeMicro(s->baseAddr, CAEN_V1190_OP_SET_TR_LEAD_LSB);
		caen_v1190_writeMicro(s->baseAddr, resolution);
		s->edgeResolution = resolution;
		s->pairWidth = NOT_USED;
	}
	s->edgeDetectionMode = mode;
}

uint16_t caen_v1190_getEdgeDetection(struct s_caen_v1190 * s) { return(s->edgeDetectionMode); };

uint16_t caen_v1190_getEdgeResolution(struct s_caen_v1190 * s) { return(s->edgeResolution); };

uint16_t caen_v1190_getPairResolution(struct s_caen_v1190 * s) { return(s->pairResolution); };

uint16_t caen_v1190_getPairWidth(struct s_caen_v1190 * s) { return(s->pairWidth); };

void caen_v1190_updateResolution(struct s_caen_v1190 * s)
{
	uint16_t resolution;

	caen_v1190_writeMicro(s->baseAddr, CAEN_V1190_OP_READ_DETECTION);
	s->edgeDetectionMode = caen_v1190_readMicro(s->baseAddr);
	resolution = caen_v1190_readMicro(s->baseAddr);

	if (mode == CAEN_V1190_B_EDGE_PAIR) {
		s->edgeResolution = NOT_USED;
		s->pairResolution = resolution & CAEN_V1190_M_EDGE_RESOLUTION;
		s->pairWidth = (resolution >> CAEN_V1190_SH_EDGE_PAIR_WIDTH) & CAEN_V1190_M_EDGE_PAIR_WIDTH;
	} else {
		s->edgeResolution = resolution;
		s->pairResolution = NOT_USED;
		s->pairWidth = NOT_USED;
	}
}

void caen_v1190_setDeadTime_db(struct s_caen_v1190 * s) { caen_v1190_setWindowWidth(s, s->deadTime); };

void caen_v1190_setDeadTime(struct s_caen_v1190 * s, uint16_t ticks)
{
	caen_v1190_writeMicro(s->baseAddr, CAEN_V1190_OP_SET_DEAD_TIME);
	caen_v1190_writeMicro(s->baseAddr, ticks);
	s->deadTime = ticks;
}

uint16_t caen_v1190_getDeadTime(struct s_caen_v1190 * s)
{
	caen_v1190_writeMicro(s->baseAddr, CAEN_V1190_OP_READ_DEAD_TIME);
	s->deadTime = caen_v1190_readMicro(s->baseAddr);
	return(s->deadTime);
}

void caen_v1190_setEventSize_db(struct s_caen_v1190 * s) { caen_v1190_setWindowWidth(s, s->eventSize); };
void caen_v1190_setEventSize(struct s_caen_v1190 * s, uint16_t hits)
{
	caen_v1190_writeMicro(s->baseAddr, CAEN_V1190_OP_SET_EVENT_SIZE);
	caen_v1190_writeMicro(s->baseAddr, hits);
	s->eventSize = hits;
}

uint16_t caen_v1190_getEventSize(struct s_caen_v1190 * s)
{
	caen_v1190_writeMicro(s->baseAddr, CAEN_V1190_OP_READ_EVENT_SIZE);
	s->eventSize = caen_v1190_readMicro(s->baseAddr);
	return(s->eventSize);
}

void caen_v1190_setFifoSize_db(struct s_caen_v1190 * s) { caen_v1190_setWindowWidth(s, s->fifoSize); };

void caen_v1190_setFifoSize(struct s_caen_v1190 * s, uint16_t words)
{
	caen_v1190_writeMicro(s->baseAddr, CAEN_V1190_OP_SET_FIFO_SIZE);
	caen_v1190_writeMicro(s->baseAddr, words);
	s->fifoSize = words;
}

uint16_t caen_v1190_getFifoSize(struct s_caen_v1190 * s)
{
	caen_v1190_writeMicro(s->baseAddr, CAEN_V1190_OP_READ_FIFO_SIZE);
	s->fifoSize = caen_v1190_readMicro(s->baseAddr);
	return(s->fifoSize);
}

void caen_v1190_enableHeaderTrailer_db(struct s_caen_v1190 * s) { caen_v1190_setWindowWidth(s, s->enaHeaderTrailer); };

void caen_v1190_enableHeaderTrailer(struct s_caen_v1190 * s, bool_t flag)
{
	uint16_t opCode = flag ? CAEN_V1190_OP_EN_HEADER_TRAILER : CAEN_V1190_OP_DIS_HEADER_TRAILER;
	caen_v1190_writeMicro(s->baseAddr, opCode);
	s->enaHeaderTrailer = flag;
}

bool_t caen_v1190_headerTrailerEnabled(struct s_caen_v1190 * s)
{
	caen_v1190_writeMicro(s->baseAddr, CAEN_V1190_OP_READ_HEADER_TRAILER);
	s->enaHeaderTrailer = (caen_v1190_readMicro(s->baseAddr) != 0);
	return(s->enaHeaderTrailer);
}

void caen_v1190_enableChannel_db(struct s_caen_v1190 * s)
{
	caen_v1190_writeMicro(s->baseAddr, CAEN_V1190_OP_WRITE_EN_PATTERN32);
	for (w = 0; w < NOF_CHANNEL_WORDS; w++) caen_v1190_writeMicro(s->baseAddr, s->channels[w]);
}

void caen_v1190_enableChannel(struct s_caen_v1190 * s, uint16_t * chn)
{
	caen_v1190_writeMicro(s->baseAddr, CAEN_V1190_OP_WRITE_EN_PATTERN32);
	for (w = 0; w < NOF_CHANNEL_WORDS; w++, chn++) caen_v1190_writeMicro(s->baseAddr, *chn);
}

void caen_v1190_getChannelEnabled(struct s_caen_v1190 * s)
{
	caen_v1190_writeMicro(s->baseAddr, CAEN_V1190_OP_READ_EN_PATTERN32);
	for (w = 0; w < NOF_CHANNEL_WORDS; w++) s->channels[w] = caen_v1190_readMicro(s->baseAddr);
}

void caen_v1190_enableExtendedTriggerTag_db(struct s_caen_v1190 * s) { caen_v1190_setWindowWidth(s, s->extTrigTag); };

void caen_v1190_enableExtendedTriggerTag(struct s_caen_v1190 * s, bool_t flag)
{
	uint16_t control = this->caen_v1190_getControl();
	if (flag) {
		control |= CAEN_V1190_B_CREG_TRIGGER_TAG_ENABLE;
	} else {
		control &= ~CAEN_V1190_B_CREG_TRIGGER_TAG_ENABLE;
	}
	s->extTrigTag = flag;
}

bool_t caen_v1190_extendedTriggerTagEnabled(struct s_caen_v1190 * s)
{
	s->extTrigTag = ((this->caen_v1190_getControl() & CAEN_V1190_B_CREG_TRIGGER_TAG_ENABLE) != 0);
	return(s->extTrigTag);
}

void caen_v1190_setAlmostFullLevel_db(struct s_caen_v1190 * s) { caen_v1190_setAlmostFullLevel(s, s->almostFullLevel); };

void caen_v1190_setAlmostFullLevel(struct s_caen_v1190 * s, uint16_t level)
{
	SET16(s->baseAddr, CAEN_V1190_A_ALMOSTFULLLEVEL, level & 0xFFFF);
	s->almostFullLevel = level & 0xFFFF;
}

uint16_t caen_v1190_getAlmostFullLevel(struct s_caen_v1190 * s)
{
	s->almostFullLevel = GET16(s->baseAddr, CAEN_V1190_A_ALMOSTFULLLEVEL) & 0xFFFF;
	return(s->almostFullLevel);
}

void caen_v1190_setControl(struct s_caen_v1190 * s, uint16_t control)
{
	SET16(s->baseAddr, CAEN_V1190_A_CONTROLREGISTER, control);
}

uint16_t caen_v1190_getControl()
{
	return(GET16(s->baseAddr, CAEN_V1190_A_CONTROLREGISTER));
}

void caen_v1190_moduleInfo(struct s_caen_v1190 * s)
{
	int firmware;
	firmware = GET16(s->baseAddr, caen_v1190_FIRMWARE_REV);
	sprintf(msg, "[%smodule_info] %s: phys addr %#lx, mapped to %#lx, firmware %02x.%02x",
									s->mpref,
									s->moduleName,
									s->vmeAddr,
									s->baseAddr,
									((firmware >> 8) & 0xff), (firmware & 0xff));
	f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
}
 
void caen_v1190_setPrefix(struct s_caen_v1190 * s, char * prefix)
{
	strcpy(s->prefix, prefix);
	strcpy(s->mpref, "");
}

bool_t caen_v1190_fillStruct(struct s_caen_v1190 * s, char * file)
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

	s->verbose = root_env_getval_b("CAEN_V1190.VerboseMode", FALSE);

	s->dumpRegsOnInit = root_env_getval_b("CAEN_V1190.DumpRegisters", FALSE);

	s->updSettings = root_env_getval_b("CAEN_V1190.UpdateSettings", FALSE);
	s->updInterval = root_env_getval_i("CAEN_V1190.UpdateInterval", 0);
	s->updCountDown = 0;

	sp = root_env_getval_s("CAEN_V1190.ModuleName", "");
	if (strcmp(s->moduleName, "caen_v1190") != 0 && strcmp(sp, s->moduleName) != 0) {
		sprintf(msg, "[%sfill_struct] %s: File %s contains wrong module name - %s", s->mpref, s->moduleName, file, sp);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		return FALSE;
	}
	strcpy(mnUC, sp);
	mnUC[0] = toupper(mnUC[0]);

	uint16_t windowWidth;
	uint16_t windowOffset;
	uint16_t rejectMargin;
	uint16_t searchMargin;
	bool_t enaTrigSubtraction;
	uint16_t edgeDetectionMode;
	uint16_t edgeResolution;
	uint16_t pairResolution;
	uint16_t pairWidth;
	uint16_t deadTime;
	uint16_t eventSize;
	uint16_t fifoSize;
	uint16_t almostFullLevel;
	bool_t enaHeaderTrailer;
	bool_t enaExtTrigTag;

	uint16_t channels[NOF_CHAN_WORDS];

	sprintf(res, "CAEN_V1190.%s.TriggerMatching", mnUC);
	s->trigMatchingOn = root_env_getval_i(res, kTRUE);

	sprintf(res, "CAEN_V1190.%s.WindowWidth", mnUC);
	s->windowWidth = root_env_getval_i(res, CAEN_V1190_WINDOW_WIDTH_DEFAULT);

	sprintf(res, "CAEN_V1190.%s.WindowOffset", mnUC);
	s->windowOffset = root_env_getval_i(res, CAEN_V1190_WINDOW_OFFSET_DEFAULT);

	sprintf(res, "CAEN_V1190.%s.RejectMargin", mnUC);
	s->rejectMargin = root_env_getval_i(res, CAEN_V1190_REJECT_MARGIN_DEFAULT);

	sprintf(res, "CAEN_V1190.%s.SearchMargin", mnUC);
	s->rejectMargin = root_env_getval_i(res, CAEN_V1190_SEARCH_MARGIN_DEFAULT);

	sprintf(res, "CAEN_V1190.%s.SubtractTriggerTime", mnUC);
	s->trigMatchingOn = root_env_getval_i(res, kTRUE);

	sprintf(res, "CAEN_V1190.%s.EdgeDetection", mnUC);
	s->rejectMargin = root_env_getval_i(res, CAEN_V1190_EDGE_DETECTION_DEFAULT);

	sprintf(res, "CAEN_V1190.%s.EdgeResolution", mnUC);
	s->rejectMargin = root_env_getval_i(res, CAEN_V1190_EDGE_RESOLUTION_DEFAULT);

	sprintf(res, "CAEN_V1190.%s.PairResolution", mnUC);
	s->rejectMargin = root_env_getval_i(res, CAEN_V1190_PAIR_RESOLUTION_DEFAULT);

	sprintf(res, "CAEN_V1190.%s.PairWidth", mnUC);
	s->rejectMargin = root_env_getval_i(res, CAEN_V1190_PAIR_WIDTH_DEFAULT);

	sprintf(res, "CAEN_V1190.%s.DeadTime", mnUC);
	s->rejectMargin = root_env_getval_i(res, CAEN_V1190_DEAD_TIME_DEFAULT);

	sprintf(res, "CAEN_V1190.%s.EventSize", mnUC);
	s->rejectMargin = root_env_getval_i(res, CAEN_V1190_EVENT_SIZE_DEFAULT);

	sprintf(res, "CAEN_V1190.%s.FifoSize", mnUC);
	s->rejectMargin = root_env_getval_i(res, CAEN_V1190_FIFO_SIZE_DEFAULT);

	sprintf(res, "CAEN_V1190.%s.AlmostFull", mnUC);
	s->rejectMargin = root_env_getval_i(res, CAEN_V1190_ALMOST_FULL_DEFAULT);

	sprintf(res, "CAEN_V1190.%s.HeaderTrailer", mnUC);
	s->trigMatchingOn = root_env_getval_i(res, kTRUE);

	sprintf(res, "CAEN_V1190.%s.ExtTriggerTag", mnUC);
	s->trigMatchingOn = root_env_getval_i(res, kFALSE);

	for (i = 0; i < NOF_CHANNEL_WORDS; i++) {
		sprintf(res, "CAEN_V1190.%s.Ch%d", mnUC, i);
		s->channels[i] = root_env_getval_i(res, 0xFFFFFFFF);
	}

	return TRUE;
}

void caen_v1190_loadFromDb(struct s_caen_v1190 * s)
{
	caen_v1190_setTriggerMatchingOn_db(s);
	caen_v1190_setWindowWidth_db(s);
	caen_v1190_setWindowOffset_db(s);
	caen_v1190_setRejectMargin_db(s);
	caen_v1190_setSearchMargin_db(s);
	caen_v1190_enableTriggerTimeSubtraction_db(s);
	caen_v1190_setResolution_db(s);
	caen_v1190_setDeadTime_db(s);
	caen_v1190_setEventSize_db(s);
	caen_v1190_setFifoSize_db(s);
	caen_v1190_enableHeaderTrailer_db(s);
	caen_v1190_enableExtendedTriggerTag_db(s);
	caen_v1190_setAlmostFullLevel_db(s);
	caen_v1190_enableChannel_db(s);
}

bool_t caen_v1190_dumpRegisters(struct s_caen_v1190 * s, char * file)
{
	FILE * f;

	int i;
	if (!s->dumpRegsOnInit) return(TRUE);

	f = fopen(file, "w");
	if (f == NULL) {
		sprintf(msg, "[%sdump_regs] %s: Error writing file %s", s->mpref, s->moduleName, file);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		return FALSE;
	}

	sprintf(msg, "[%sdump_regs] %s: Dumping settings to file %s", s->mpref, s->moduleName, file);
	f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);

	fprintf(f, "Thresholds:\n");
	for (ch = 0; ch < NOF_CHANNELS; ch++) fprintf(f, "   %2d: %d\n", ch, caen_v1190_getThreshold(s, ch));

	fprintf(f, "Addr reg          : %#x\n", caen_v1190_getAddrReg(s));
	fprintf(f, "Module ID         : %d\n", caen_v1190_getModuleId(s));
	fprintf(f, "Data width        : %d\n", caen_v1190_getDataWidth(s));
	fprintf(f, "Multi event       : %d\n", caen_v1190_getMultiEvent(s));
	fprintf(f, "Max xfer data wc  : %d\n", caen_v1190_getXferData(s));
	fprintf(f, "Marking type      : %d\n", caen_v1190_getMarkingType(s));
	fprintf(f, "Bank operation    : %d\n", caen_v1190_getBankOperation(s));
	fprintf(f, "Adc resolution    : %d\n", caen_v1190_getAdcResolution(s));
	fprintf(f, "Output format     : %d\n", caen_v1190_getOutputFormat(s));
	fprintf(f, "Adc override      : %d\n", caen_v1190_getAdcOverride(s));
	fprintf(f, "Sliding scale off : %d\n", caen_v1190_getSlidingScaleOff(s));
	fprintf(f, "Skip out of range : %d\n", caen_v1190_getSkipOutOfRange(s));
	for (gg = 0; gg <= 1; gg++) {
		fprintf(f, "Hold delay %d      : %d\n", gg, caen_v1190_getHoldDelay(s, gg));
		fprintf(f, "Hold width %d      : %d\n", gg, caen_v1190_getHoldWidth(s, gg));
	}
	fprintf(f, "Use GG            : %d\n", caen_v1190_getGGUsed(s));
	fprintf(f, "Input range       : %d\n", caen_v1190_getInputRange(s));
	fprintf(f, "Ecl termination   : %#x\n", caen_v1190_getEclTerm(s));
	fprintf(f, "Ecl gate or osc   : %d\n", caen_v1190_getEclG1OrOsc(s));
	fprintf(f, "Ecl fcl or reset  : %d\n", caen_v1190_getEclFclOrRts(s));
	fprintf(f, "Nim gate or osc   : %d\n", caen_v1190_getNimG1OrOsc(s));
	fprintf(f, "Nim fcl or reset  : %d\n", caen_v1190_getNimFclOrRts(s));
	fprintf(f, "Nim busy          : %d\n", caen_v1190_getNimBusy(s));
	fprintf(f, "Pulser status     : %d\n", caen_v1190_getTestPulser(s));
	fprintf(f, "Timestamp source  : %#x\n", caen_v1190_getTsSource(s));
	fprintf(f, "Timestamp divisor : %d\n", caen_v1190_getTsDivisor(s));
	fclose(f);
}

void caen_v1190_printDb(struct s_caen_v1190 * s)
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

bool_t caen_v1190_updateSettings(struct s_caen_v1190 * s, char * updFile)
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


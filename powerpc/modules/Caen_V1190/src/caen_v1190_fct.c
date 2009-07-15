/*_______________________________________________________[C LIBRARY FUNCTIONS]
//////////////////////////////////////////////////////////////////////////////
//! \file			caen_v1190fct.c
//! \brief			Code for module caen_v1190
//! \details		Implements functions to handle modules of type Mesytec caen_v1190
//!
//! $Author: Marabou $
//! $Mail			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.5 $       
//! $Date: 2009-07-15 14:34:53 $
////////////////////////////////////////////////////////////////////////////*/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <allParam.h>
#include <ces/bmalib.h>
#include <errno.h>

#include "vmelib.h"

#include "gd_readout.h"

#include "caen_v1190.h"
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
		s->verify = FALSE;
		s->dumpRegsOnInit = FALSE;
	} else {
		sprintf(msg, "[%salloc] %s: Can't allocate caen_v1190 struct", s->mpref, s->moduleName);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
	}
	return s;
}

void caen_v1190_setTriggerMatchingOn_db(struct s_caen_v1190 * s) { caen_v1190_setTriggerMatchingOn(s, s->trigMatchingOn); };

void caen_v1190_setTriggerMatchingOn(struct s_caen_v1190 * s, bool_t flag)
{
	uint16_t opCode = flag ? CAEN_V1190_OP_TRG_MATCH : CAEN_V1190_OP_CONT_STOR;
	caen_v1190_writeMicro(s, opCode, NO_ARG);
	s->trigMatchingOn = flag;
}

bool_t caen_v1190_triggerMatchingIsOn(struct s_caen_v1190 * s)
{
	s->trigMatchingOn = ((caen_v1190_readMicro(s, CAEN_V1190_OP_READ_ACQ_MOD) & CAEN_V1190_B_TRIGGER_MATCHING_ON) != 0);
	return(s->trigMatchingOn);
}

void caen_v1190_setWindowWidth_db(struct s_caen_v1190 * s) { caen_v1190_setWindowWidth(s, s->windowWidth); };

void caen_v1190_setWindowWidth(struct s_caen_v1190 * s, uint16_t ticks)
{
	caen_v1190_writeMicro(s, CAEN_V1190_OP_SET_WIN_WIDTH, ticks);
	s->windowWidth = ticks;
}

uint16_t caen_v1190_getWindowWidth(struct s_caen_v1190 * s) { return(s->windowWidth); };

void caen_v1190_setWindowOffset_db(struct s_caen_v1190 * s) { caen_v1190_setWindowOffset(s, s->windowOffset); };

void caen_v1190_setWindowOffset(struct s_caen_v1190 * s, uint16_t ticks)
{
	caen_v1190_writeMicro(s, CAEN_V1190_OP_SET_WIN_OFFS, ticks);
	s->windowOffset = ticks;
}

uint16_t caen_v1190_getWindowOffset(struct s_caen_v1190 * s) { return(s->windowOffset); };

void caen_v1190_setRejectMargin_db(struct s_caen_v1190 * s) { caen_v1190_setRejectMargin(s, s->rejectMargin); };

void caen_v1190_setRejectMargin(struct s_caen_v1190 * s, uint16_t ticks)
{
	caen_v1190_writeMicro(s, CAEN_V1190_OP_SET_REJ_MARGIN, ticks);
	s->rejectMargin = ticks;
}

uint16_t caen_v1190_getRejectMargin(struct s_caen_v1190 * s) { return(s->rejectMargin); };

void caen_v1190_setSearchMargin_db(struct s_caen_v1190 * s) { caen_v1190_setSearchMargin(s, s->searchMargin); };

void caen_v1190_setSearchMargin(struct s_caen_v1190 * s, uint16_t ticks) {
	caen_v1190_writeMicro(s, CAEN_V1190_OP_SET_SW_MARGIN, ticks);
	s->searchMargin = ticks;
}

uint16_t caen_v1190_getSearchMargin(struct s_caen_v1190 * s) { return(s->searchMargin); };

void caen_v1190_enableTriggerTimeSubtraction_db(struct s_caen_v1190 * s) { caen_v1190_enableTriggerTimeSubtraction(s, s->enaTrigSubtraction); };

void caen_v1190_enableTriggerTimeSubtraction(struct s_caen_v1190 * s, bool_t flag)
{
	uint16_t opCode = flag ? CAEN_V1190_OP_EN_SUB_TRG : CAEN_V1190_OP_DIS_SUB_TRG;
	caen_v1190_writeMicro(s, opCode, NO_ARG);
	s->enaTrigSubtraction = flag;
}

bool_t caen_v1190_triggerTimeSubtractionEnabled(struct s_caen_v1190 * s) { return(s->enaTrigSubtraction); };

void caen_v1190_readTriggerSettings(struct s_caen_v1190 * s) {
	uint16_t data[5];
	memset(data, 0, 5 * sizeof(uint16_t));
	caen_v1190_readMicroBlock(s, CAEN_V1190_OP_READ_TRG_CONF, data, 5);
	s->windowWidth = data[0];
	s->windowOffset = data[1];
	s->searchMargin = data[2];
	s->rejectMargin = data[3];
	s->enaTrigSubtraction = ((data[4] & CAEN_V1190_B_TRIGGER_TIME_SUBTR_ON) != 0);
}

void caen_v1190_setResolution_db(struct s_caen_v1190 * s)
{
	if (s->edgeDetectionMode == CAEN_V1190_B_EDGE_PAIR) {
		caen_v1190_setResolution(s, s->edgeDetectionMode, s->pairResolution, s->pairWidth);
	} else {
		caen_v1190_setResolution(s, s->edgeDetectionMode, s->edgeResolution, NOT_USED);
	}
};

void caen_v1190_setResolution(struct s_caen_v1190 * s, uint16_t mode, uint16_t resolution, uint16_t pairWidth)
{
	uint16_t pairRes;
	caen_v1190_writeMicro(s, CAEN_V1190_OP_SET_DETECTION, mode);

	if (mode == CAEN_V1190_B_EDGE_PAIR) {
		resolution &= CAEN_V1190_M_PAIR_RESOLUTION;
		pairRes = (pairWidth & CAEN_V1190_M_PAIR_WIDTH) << CAEN_V1190_SH_PAIR_WIDTH + resolution;
		caen_v1190_writeMicro(s, CAEN_V1190_OP_SET_PAIR_RES, pairRes);
		s->pairResolution = resolution;
		s->pairWidth = pairWidth;
	} else {
		resolution &= CAEN_V1190_M_EDGE_RESOLUTION;
		caen_v1190_writeMicro(s, CAEN_V1190_OP_SET_TR_LEAD_LSB, resolution);
		s->edgeResolution = resolution;
		s->pairWidth = NOT_USED;
	}
	s->edgeDetectionMode = mode;
}

uint16_t caen_v1190_getEdgeDetection(struct s_caen_v1190 * s) { return(s->edgeDetectionMode); };

uint16_t caen_v1190_getEdgeResolution(struct s_caen_v1190 * s) { return(s->edgeResolution); };

uint16_t caen_v1190_getPairResolution(struct s_caen_v1190 * s) { return(s->pairResolution); };

uint16_t caen_v1190_getPairWidth(struct s_caen_v1190 * s) { return(s->pairWidth); };

void caen_v1190_readResolutionSettings(struct s_caen_v1190 * s)
{
	uint16_t resolution;

	s->edgeDetectionMode = caen_v1190_readMicro(s, CAEN_V1190_OP_READ_DETECTION);
	resolution = caen_v1190_readMicro(s, CAEN_V1190_OP_READ_RES);

	if (s->edgeDetectionMode == CAEN_V1190_B_EDGE_PAIR) {
		s->edgeResolution = NOT_USED;
		s->pairResolution = resolution & CAEN_V1190_M_EDGE_RESOLUTION;
		s->pairWidth = (resolution >> CAEN_V1190_SH_PAIR_WIDTH) & CAEN_V1190_M_PAIR_WIDTH;
	} else {
		s->edgeResolution = resolution;
		s->pairResolution = NOT_USED;
		s->pairWidth = NOT_USED;
	}
}

void caen_v1190_setDeadTime_db(struct s_caen_v1190 * s) { caen_v1190_setDeadTime(s, s->deadTime); };

void caen_v1190_setDeadTime(struct s_caen_v1190 * s, uint16_t ticks)
{
	caen_v1190_writeMicro(s, CAEN_V1190_OP_SET_DEAD_TIME, ticks);
	s->deadTime = ticks;
}

uint16_t caen_v1190_getDeadTime(struct s_caen_v1190 * s)
{
	s->deadTime = caen_v1190_readMicro(s, CAEN_V1190_OP_READ_DEAD_TIME);
	return(s->deadTime);
}

void caen_v1190_setEventSize_db(struct s_caen_v1190 * s) { caen_v1190_setEventSize(s, s->eventSize); };

void caen_v1190_setEventSize(struct s_caen_v1190 * s, uint16_t hits)
{
	caen_v1190_writeMicro(s, CAEN_V1190_OP_SET_EVENT_SIZE, hits);
	s->eventSize = hits;
}

uint16_t caen_v1190_getEventSize(struct s_caen_v1190 * s)
{
	s->eventSize = caen_v1190_readMicro(s, CAEN_V1190_OP_READ_EVENT_SIZE);
	return(s->eventSize);
}

void caen_v1190_setFifoSize_db(struct s_caen_v1190 * s) { caen_v1190_setFifoSize(s, s->fifoSize); };

void caen_v1190_setFifoSize(struct s_caen_v1190 * s, uint16_t words)
{
	caen_v1190_writeMicro(s, CAEN_V1190_OP_SET_FIFO_SIZE, words);
	s->fifoSize = words;
}

uint16_t caen_v1190_getFifoSize(struct s_caen_v1190 * s)
{
	s->fifoSize = caen_v1190_readMicro(s, CAEN_V1190_OP_READ_FIFO_SIZE);
	return(s->fifoSize);
}

void caen_v1190_enableHeaderTrailer_db(struct s_caen_v1190 * s) { caen_v1190_enableHeaderTrailer(s, s->enaHeaderTrailer); };

void caen_v1190_enableHeaderTrailer(struct s_caen_v1190 * s, bool_t flag)
{
	uint16_t opCode = flag ? CAEN_V1190_OP_EN_HEADER_TRAILER : CAEN_V1190_OP_DIS_HEADER_TRAILER;
	caen_v1190_writeMicro(s, opCode, NO_ARG);
	s->enaHeaderTrailer = flag;
}

bool_t caen_v1190_headerTrailerEnabled(struct s_caen_v1190 * s)
{
	s->enaHeaderTrailer = (caen_v1190_readMicro(s, CAEN_V1190_OP_READ_HEADER_TRAILER) != 0);
	return(s->enaHeaderTrailer);
}

void caen_v1190_enableChannel_db(struct s_caen_v1190 * s)
{
	caen_v1190_writeMicroBlock(s, CAEN_V1190_OP_WRITE_EN_PATTERN, s->channels, NOF_CHANNEL_WORDS);
}

void caen_v1190_enableChannel(struct s_caen_v1190 * s, uint16_t * chnPat)
{
	caen_v1190_writeMicroBlock(s, CAEN_V1190_OP_WRITE_EN_PATTERN, chnPat, NOF_CHANNEL_WORDS);
}

void caen_v1190_getChannelEnabled(struct s_caen_v1190 * s)
{
	caen_v1190_readMicroBlock(s, CAEN_V1190_OP_READ_EN_PATTERN, s->channels, NOF_CHANNEL_WORDS);
}

void caen_v1190_enableExtendedTriggerTag_db(struct s_caen_v1190 * s) { caen_v1190_enableExtendedTriggerTag(s, s->enaExtTrigTag); };

void caen_v1190_enableExtendedTriggerTag(struct s_caen_v1190 * s, bool_t flag)
{
	uint16_t control = caen_v1190_getControl(s);
	if (flag) {
		control |= CAEN_V1190_B_CREG_TRIGGER_TAG_ENABLE;
	} else {
		control &= ~CAEN_V1190_B_CREG_TRIGGER_TAG_ENABLE;
	}
	caen_v1190_setControl(s, control);
	s->enaExtTrigTag = flag;
}

bool_t caen_v1190_extendedTriggerTagEnabled(struct s_caen_v1190 * s)
{
	s->enaExtTrigTag = ((caen_v1190_getControl(s) & CAEN_V1190_B_CREG_TRIGGER_TAG_ENABLE) != 0);
	return(s->enaExtTrigTag);
}

void caen_v1190_enableEmptyEvent_db(struct s_caen_v1190 * s) { caen_v1190_enableEmptyEvent(s, s->enaEmptyEvent); };

void caen_v1190_enableEmptyEvent(struct s_caen_v1190 * s, bool_t flag)
{
	uint16_t control = caen_v1190_getControl(s);
	if (flag) {
		control |= CAEN_V1190_B_CREG_EMPTY_EVENT;
	} else {
		control &= ~CAEN_V1190_B_CREG_EMPTY_EVENT;
	}
	caen_v1190_setControl(s, control);
	s->enaEmptyEvent = flag;
}

bool_t caen_v1190_emptyEventEnabled(struct s_caen_v1190 * s)
{
	s->enaEmptyEvent = ((caen_v1190_getControl(s) & CAEN_V1190_B_CREG_EMPTY_EVENT) != 0);
	return(s->enaEmptyEvent);
}

void caen_v1190_enableEventFifo_db(struct s_caen_v1190 * s) { caen_v1190_enableEventFifo(s, s->enaEventFifo); };

void caen_v1190_enableEventFifo(struct s_caen_v1190 * s, bool_t flag)
{
	uint16_t control = caen_v1190_getControl(s);
	if (flag) {
		control |= CAEN_V1190_B_CREG_EVENT_FIFO_ENABLE;
	} else {
		control &= ~CAEN_V1190_B_CREG_EVENT_FIFO_ENABLE;
	}
	caen_v1190_setControl(s, control);
	s->enaEventFifo = flag;
}

bool_t caen_v1190_eventFifoEnabled(struct s_caen_v1190 * s)
{
	s->enaEventFifo = ((caen_v1190_getControl(s) & CAEN_V1190_B_CREG_EVENT_FIFO_ENABLE) != 0);
	return(s->enaEventFifo);
}

void caen_v1190_setAlmostFullLevel_db(struct s_caen_v1190 * s) { caen_v1190_setAlmostFullLevel(s, s->almostFullLevel); };

void caen_v1190_setAlmostFullLevel(struct s_caen_v1190 * s, uint16_t level)
{
	SET_DATA(s, CAEN_V1190_A_ALMOSTFULLLEVEL, level & 0xFFFF);
	s->almostFullLevel = level & 0xFFFF;
}

uint16_t caen_v1190_getAlmostFullLevel(struct s_caen_v1190 * s)
{
	s->almostFullLevel = GET_DATA(s, CAEN_V1190_A_ALMOSTFULLLEVEL) & 0xFFFF;
	return(s->almostFullLevel);
}

void caen_v1190_setControl(struct s_caen_v1190 * s, uint16_t control)
{
	SET_DATA(s, CAEN_V1190_A_CONTROLREGISTER, control);
}

uint16_t caen_v1190_getControl(struct s_caen_v1190 * s)
{
	return(GET_DATA(s, CAEN_V1190_A_CONTROLREGISTER));
}

uint16_t caen_v1190_getStatus(struct s_caen_v1190 * s)
{
	return(GET_DATA(s, CAEN_V1190_A_STATUSREGISTER));
}

void caen_v1190_reset(struct s_caen_v1190 * s)
{
	SET_DATA(s, CAEN_V1190_A_RESET, 0);
}

void caen_v1190_softClear(struct s_caen_v1190 * s)
{
	SET_DATA(s, CAEN_V1190_A_CLEAR, 0);
}

bool_t caen_v1190_moduleInfo(struct s_caen_v1190 * s)
{
	uint32_t oui, model, serial, revision, type;

	oui = caen_v1190_read_config_rom(s, CAEN_V1190_CPROM_OUI2, 3);
	if (oui != 0x0040e6) {
		sprintf(msg, "[%smoduleInfo] %s: Wrong manufacturer code - %#x", s->mpref, s->moduleName, oui);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		return FALSE;
	}
	model = caen_v1190_read_config_rom(s, CAEN_V1190_CPROM_MODELNUMBER2, 3);
	if((model != 1190) && (model != 1290)) {
		sprintf(msg, "[%smoduleInfo] %s: Wrong model - v%d", s->mpref, s->moduleName, model);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		return FALSE;
	}

	type = caen_v1190_read_config_rom(s, CAEN_V1190_CPROM_BOARDVERSION, 1);
	serial = caen_v1190_read_config_rom(s, CAEN_V1190_CPROM_SERIALNUMBER1, 2);
	revision = caen_v1190_read_config_rom(s, CAEN_V1190_CPROM_REVISION3, 4);

	sprintf(msg, "[%smodule_info] %s: phys addr %#lx, mapped to %#lx, module CAEN v%d%c, serial %d, rev %d",
									s->mpref,
									s->moduleName,
									s->vmeAddr,
									s->baseAddr,
									model,
									(type == 0 ? 'A' : 'B'),
									serial,
									revision);
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
	s->verify = root_env_getval_b("CAEN_V1190.Verify", FALSE);
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

	sprintf(res, "CAEN_V1190.%s.TriggerMatching", mnUC);
	s->trigMatchingOn = root_env_getval_b(res, TRUE);

	sprintf(res, "CAEN_V1190.%s.WindowWidth", mnUC);
	s->windowWidth = root_env_getval_i(res, CAEN_V1190_WINDOW_WIDTH_DEFAULT);

	sprintf(res, "CAEN_V1190.%s.WindowOffset", mnUC);
	s->windowOffset = root_env_getval_i(res, CAEN_V1190_WINDOW_OFFSET_DEFAULT);

	sprintf(res, "CAEN_V1190.%s.RejectMargin", mnUC);
	s->rejectMargin = root_env_getval_i(res, CAEN_V1190_REJECT_MARGIN_DEFAULT);

	sprintf(res, "CAEN_V1190.%s.SearchMargin", mnUC);
	s->searchMargin = root_env_getval_i(res, CAEN_V1190_SEARCH_MARGIN_DEFAULT);

	sprintf(res, "CAEN_V1190.%s.SubtractTriggerTime", mnUC);
	s->enaTrigSubtraction = root_env_getval_b(res, TRUE);

	sprintf(res, "CAEN_V1190.%s.EdgeDetection", mnUC);
	s->edgeDetectionMode = root_env_getval_i(res, CAEN_V1190_EDGE_DETECTION_DEFAULT);

	sprintf(res, "CAEN_V1190.%s.EdgeResolution", mnUC);
	s->edgeResolution = root_env_getval_i(res, CAEN_V1190_EDGE_RESOLUTION_DEFAULT);

	sprintf(res, "CAEN_V1190.%s.PairResolution", mnUC);
	s->pairResolution = root_env_getval_i(res, CAEN_V1190_PAIR_RESOLUTION_DEFAULT);

	sprintf(res, "CAEN_V1190.%s.PairWidth", mnUC);
	s->pairWidth = root_env_getval_i(res, CAEN_V1190_PAIR_WIDTH_DEFAULT);

	sprintf(res, "CAEN_V1190.%s.DeadTime", mnUC);
	s->deadTime = root_env_getval_i(res, CAEN_V1190_DEAD_TIME_DEFAULT);

	sprintf(res, "CAEN_V1190.%s.EventSize", mnUC);
	s->eventSize = root_env_getval_i(res, CAEN_V1190_EVENT_SIZE_DEFAULT);

	sprintf(res, "CAEN_V1190.%s.FifoSize", mnUC);
	s->fifoSize = root_env_getval_i(res, CAEN_V1190_FIFO_SIZE_DEFAULT);

	sprintf(res, "CAEN_V1190.%s.AlmostFull", mnUC);
	s->almostFullLevel = root_env_getval_i(res, CAEN_V1190_ALMOST_FULL_DEFAULT);

	sprintf(res, "CAEN_V1190.%s.HeaderTrailer", mnUC);
	s->enaHeaderTrailer = root_env_getval_b(res, TRUE);

	sprintf(res, "CAEN_V1190.%s.ExtTriggerTag", mnUC);
	s->enaExtTrigTag = root_env_getval_b(res, FALSE);

	sprintf(res, "CAEN_V1190.%s.EmptyEvent", mnUC);
	s->enaEmptyEvent = root_env_getval_b(res, FALSE);

	sprintf(res, "CAEN_V1190.%s.EventFifo", mnUC);
	s->enaEventFifo = root_env_getval_b(res, TRUE);

	for (i = 0; i < NOF_CHANNEL_WORDS; i++) {
		sprintf(res, "CAEN_V1190.%s.Ch%d", mnUC, i);
		s->channels[i] = root_env_getval_i(res, DATA_ERROR) & 0xFFFF;
	}

	return TRUE;
}

void caen_v1190_loadFromDb(struct s_caen_v1190 * s)
{
	char * sp;

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
	caen_v1190_enableEmptyEvent_db(s);
	caen_v1190_enableEventFifo_db(s);
	caen_v1190_setAlmostFullLevel_db(s);
	caen_v1190_enableChannel_db(s);
}

bool_t caen_v1190_dumpRegisters(struct s_caen_v1190 * s, char * file)
{
	FILE * f;

	int i;
	if (!s->dumpRegsOnInit) return(TRUE);

	if (file == NULL) {
		f = stdout;
	} else {
		f = fopen(file, "w");
		if (f == NULL) {
			sprintf(msg, "[%sdump_regs] %s: Error writing file %s", s->mpref, s->moduleName, file);
			f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
			return FALSE;
		}
	}

	if (file != NULL) {
		sprintf(msg, "[%sdump_regs] %s: Dumping settings to file %s", s->mpref, s->moduleName, file);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
	}

	fprintf(f, "Trigger matching           : %s\n", (caen_v1190_triggerMatchingIsOn(s) ? "On" : "Off"));
	caen_v1190_readTriggerSettings(s);
	fprintf(f, "Window width               : %#x\n", caen_v1190_getWindowWidth(s));
	fprintf(f, "Window offset              : %#x\n", caen_v1190_getWindowOffset(s));
	fprintf(f, "Search margin              : %#x\n", caen_v1190_getSearchMargin(s));
	fprintf(f, "Reject margin              : %#x\n", caen_v1190_getRejectMargin(s));
	fprintf(f, "Subtract trigger time      : %s\n", (caen_v1190_triggerTimeSubtractionEnabled(s) ? "Yes" : "No"));
	caen_v1190_readResolutionSettings(s);
	fprintf(f, "Edge detection             : %#x\n", caen_v1190_getEdgeDetection(s));
	if (caen_v1190_getEdgeDetection(s) == CAEN_V1190_B_EDGE_PAIR) {
		fprintf(f, "Pair resolution            : %#x\n", caen_v1190_getPairResolution(s));
		fprintf(f, "Pair width                 : %#x\n", caen_v1190_getPairWidth(s));
	} else {
		fprintf(f, "Edge resolution            : %#x\n", caen_v1190_getEdgeResolution(s));
	}
	fprintf(f, "Dead time                  : %#x\n", caen_v1190_getDeadTime(s));
	fprintf(f, "Event size                 : %#x\n", caen_v1190_getEventSize(s));
	fprintf(f, "Fifo size                  : %#x\n", caen_v1190_getFifoSize(s));
	fprintf(f, "Write header/trailer       : %s\n", (caen_v1190_headerTrailerEnabled(s) ? "Yes" : "No"));
	fprintf(f, "Write h/t on empty events  : %s\n", (caen_v1190_emptyEventEnabled(s) ? "Yes" : "No"));
	fprintf(f, "Enable event fifo          : %s\n", (caen_v1190_eventFifoEnabled(s) ? "Yes" : "No"));
	fprintf(f, "Write ext trigger tag      : %s\n", (caen_v1190_extendedTriggerTagEnabled(s) ? "Yes" : "No"));
	fprintf(f, "Almost full level          : %d\n", caen_v1190_getAlmostFullLevel(s));
	caen_v1190_getChannelEnabled(s);
	fprintf(f, "Channel pattern            :\n");
	for (i = 0; i < NOF_CHANNEL_WORDS; i++) {
		fprintf(f, "         %3d - %3d         : %#x\n", (i * 16), ((i + 1) * 16 - 1), s->channels[i] & 0xFFFF);
	}
	if (file != NULL) fclose(f);
}

void caen_v1190_printDb(struct s_caen_v1190 * s)
{
	int i;
	printf("Trigger matching           : %s\n", (s->trigMatchingOn ? "On" : "Off"));
	printf("Window width               : %#x\n", s->windowWidth);
	printf("Window offset              : %#x\n", s->windowOffset);
	printf("Search margin              : %#x\n", s->searchMargin);
	printf("Reject margin              : %#x\n", s->rejectMargin);
	printf("Subtract trigger time      : %s\n", (s->enaTrigSubtraction ? "Yes" : "No"));
	printf("Edge detection             : %#x\n", s->edgeDetectionMode);
	if (s->edgeDetectionMode == CAEN_V1190_B_EDGE_PAIR) {
		printf("Pair resolution            : %#x\n", s->pairResolution);
		printf("Pair width                 : %#x\n", s->pairWidth);
	} else {
		printf("Edge resolution            : %#x\n", s->edgeResolution);
	}
	printf("Dead time                  : %#x\n", s->deadTime);
	printf("Event size                 : %#x\n", s->eventSize);
	printf("Fifo size                  : %#x\n", s->fifoSize);
	printf("Write header/trailer       : %s\n", s->enaHeaderTrailer ? "Yes" : "No");
	printf("Write h/t on empty events  : %s\n", (caen_v1190_emptyEventEnabled(s) ? "Yes" : "No"));
	printf("Enable event fifo          : %s\n", (caen_v1190_eventFifoEnabled(s) ? "Yes" : "No"));
	printf("Write ext trigger tag      : %s\n", s->enaExtTrigTag ? "Yes" : "No");
	printf("Almost full level          : %d\n", s->almostFullLevel);
	printf("Channel pattern            :\n");
	for (i = 0; i < NOF_CHANNEL_WORDS; i++) {
		printf("         %3d - %3d         : %#x\n", (i * 16), ((i + 1) * 16 - 1), s->channels[i] & 0xFFFF);
	}
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

bool_t caen_v1190_waitWrite(struct s_caen_v1190 * s, uint16_t opCode)
{
	int try;

	for (try = 0; try < HANDSHAKE_TIMEOUT; try++) {
		if ((MICRO_HSHAKE(s) & CAEN_V1190_B_WRITE_OK) != 0) return TRUE;
	}

	sprintf(msg, "[%swaitWrite] %s: Handshake timeout on WRITE (opCode = %#x)", s->mpref, s->moduleName, opCode);
	f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);

	return FALSE;
}

bool_t caen_v1190_waitRead(struct s_caen_v1190 * s, uint16_t opCode)
{
	int try;

	for (try = 0; try < HANDSHAKE_TIMEOUT; try++) {
		if ((MICRO_HSHAKE(s) & CAEN_V1190_B_READ_OK) != 0) return TRUE;
	}

	sprintf(msg, "[%swaitRead] %s: Handshake timeout on READ (opCode = %#x)", s->mpref, s->moduleName, opCode);
	f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
	return FALSE;
}

bool_t caen_v1190_readPending(struct s_caen_v1190 * s, uint16_t opCode)
{
	if ((MICRO_HSHAKE(s) & CAEN_V1190_B_READ_OK) == 0) return FALSE;

	sprintf(msg, "[%sreadPending] %s: READ request pending (opCode = %#x)", s->mpref, s->moduleName, opCode);
	f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
	return TRUE;
}

bool_t caen_v1190_writeMicro(struct s_caen_v1190 * s, uint16_t opCode, uint16_t data)
{
	if (caen_v1190_readPending(s, opCode)) return(-1);
	if (!caen_v1190_waitWrite(s, opCode)) return(FALSE);
	SET_MICRO_DATA(s, opCode);
	if (data != NO_ARG) {
		if (!caen_v1190_waitWrite(s, opCode)) return(FALSE);
		SET_MICRO_DATA(s, data);
	}
	caen_v1190_waitWrite(s, opCode);
	return TRUE;
}

uint16_t caen_v1190_writeMicroBlock(struct s_caen_v1190 * s, uint16_t opCode, uint16_t * data, int nofWords)
{
	uint16_t d;
	int n = 0;
	if (caen_v1190_readPending(s, opCode)) return(-1);
	if (!caen_v1190_waitWrite(s, opCode)) return(-1);
	SET_MICRO_DATA(s, opCode);
	for (; nofWords--; n++) {
		if (!caen_v1190_waitWrite(s, opCode)) return(-1);
		SET_MICRO_DATA(s, *data++);
	}
	caen_v1190_waitWrite(s, opCode);
	return n;
}

uint16_t caen_v1190_readMicro(struct s_caen_v1190 * s, uint16_t opCode)
{
	if (!caen_v1190_waitWrite(s, opCode)) return(DATA_ERROR);
	SET_MICRO_DATA(s, opCode);
	if (!caen_v1190_waitRead(s, opCode)) return(DATA_ERROR);
	return(GET_MICRO_DATA(s));
}

uint16_t caen_v1190_readMicroBlock(struct s_caen_v1190 * s, uint16_t opCode, uint16_t * data, int nofWords)
{
	int i;
	uint16_t d;
	int n = 0;
	if (!caen_v1190_waitWrite(s, opCode)) return(-1);
	SET_MICRO_DATA(s, opCode);
	for (i = 0; i < nofWords; i++, n++) {
		if (!caen_v1190_waitRead(s, opCode)) return(-1);
		*data++ = GET_MICRO_DATA(s);
	}
	return n;
}

uint32_t caen_v1190_read_config_rom(struct s_caen_v1190 * s, uint16_t offset, int nofWords)
{
	uint32_t n = 0;

	offset += CAEN_V1190_A_CONFIGROM;
	for (;nofWords--; offset += 4) {
		n <<= 8;
		n |= GET_DATA(s, offset) & 0xFF;
	}
	return n;
}

void caen_v1190_startAcq(struct s_caen_v1190 * s) {};
void caen_v1190_stopAcq(struct s_caen_v1190 * s) {};

bool_t caen_v1190_waitFifoReady(struct s_caen_v1190 * s)
{
	int i;
	uint16_t sts;
	for (i = 0; i < WAIT_FIFO_READY; i++) {
		sts = GET_DATA(s, CAEN_V1190_A_EVENTFIFOSTATUS);
		if (sts & CAEN_V1190_B_FIFO_READY) return TRUE;
	}
	return FALSE;
}

bool_t caen_v1190_waitDataReady(struct s_caen_v1190 * s)
{
	int i;
	uint16_t sts;
	for (i = 0; i < WAIT_DATA_READY; i++) {
		sts = GET_DATA(s, CAEN_V1190_A_STATUSREGISTER);
		if (sts & CAEN_V1190_B_DATA_READY) return TRUE;
	}
	return FALSE;
}

int caen_v1190_getEventWcFromFifo(struct s_caen_v1190 * s)
{
	return GET_DATA32(s, CAEN_V1190_A_EVENTFIFO);
}

int caen_v1190_readout(struct s_caen_v1190 * s, uint32_t * pointer)
{
	int i;
	int nofWords;
	uint32_t data;
	uint32_t * dataStart = pointer;

#if 0
	if (caen_v1190_waitDataReady(s)) {
		sprintf(msg, "[%sreadout] %s: data ready", s->mpref, s->moduleName);
		f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		for (;;) {
			data = GET_DATA32(s, CAEN_V1190_A_OUTPUTBUFFER);
			sprintf(msg, "[%sreadout] %s: data = %#lx", s->mpref, s->moduleName, data);
			f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
			sleep(1);
			if ((data & CAEN_V1190_M_TYPE) == CAEN_V1190_B_GLOBAL_HEADER) {
				sprintf(msg, "[%sreadout] %s: got a header %#lx", s->mpref, s->moduleName, data);
				f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
			}
			*pointer++ = data;
			if ((data & CAEN_V1190_M_TYPE) == CAEN_V1190_B_GLOBAL_TRAILER) {
				sprintf(msg, "[%sreadout] %s: got a trailer %#lx", s->mpref, s->moduleName, data);
				f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
				break;
			}
		}
	}
#endif

	if (caen_v1190_waitFifoReady(s)) {
		nofWords = caen_v1190_getEventWcFromFifo(s);
		for (i = 0; i < nofWords; i++) {
			data = GET_DATA32(s, CAEN_V1190_A_OUTPUTBUFFER);
			if (i == 0 && (data & CAEN_V1190_M_TYPE) != CAEN_V1190_B_GLOBAL_HEADER) {
				sprintf(msg, "[%sreadout] %s: Out of phase - not a global header (%#lx) @ wc=%d", s->mpref, s->moduleName, data, i);
				f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
			}
			*pointer++ = data;
		}
		if ((data & CAEN_V1190_M_TYPE) != CAEN_V1190_B_GLOBAL_TRAILER) {
			sprintf(msg, "[%sreadout] %s: Out of phase - not a global trailer (%#lx) @ wc=%d", s->mpref, s->moduleName, data, i);
			f_ut_send_msg(s->prefix, msg, ERR__MSG_INFO, MASK__PRTT);
		}
	}

	caen_v1190_softClear(s);
	return (pointer - dataStart);
}



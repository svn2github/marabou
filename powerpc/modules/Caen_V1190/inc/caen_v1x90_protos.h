#ifndef __CAEN_V1X90_PROTOS_H__
#define __CAEN_V1X90_PROTOS_H__

/*_______________________________________________________________[HEADER FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			caen_v1x90_protos.h
//! \brief			Definitions for CAEN V1x90 TDC
//! \details		Prototypes for Caen V1x90
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.1 $     
//! $Date: 2009-07-28 08:16:20 $
////////////////////////////////////////////////////////////////////////////*/


struct s_caen_v1x90 * caen_v1x90_alloc(unsigned long vmeAddr, volatile unsigned char * base, char * moduleName, int serial);

bool_t caen_v1x90_moduleInfo(struct s_caen_v1x90 * s);
void caen_v1x90_setPrefix(struct s_caen_v1x90 * s, char * prefix);

bool_t caen_v1x90_fillStruct(struct s_caen_v1x90 * s, char * file);

void caen_v1x90_loadFromDb(struct s_caen_v1x90 * s);

bool_t caen_v1x90_dumpRegisters(struct s_caen_v1x90 * s, char * file);
void caen_v1x90_printDb(struct s_caen_v1x90 * s);

void caen_v1x90_reset(struct s_caen_v1x90 * s);

void caen_v1x90_setTriggerMatchingOn(struct s_caen_v1x90 * s, bool_t flag);
void caen_v1x90_setTriggerMatchingOn_db(struct s_caen_v1x90 * s);
bool_t caen_v1x90_triggerMatchingIsOn(struct s_caen_v1x90 * s);

void caen_v1x90_setWindowWidth(struct s_caen_v1x90 * s, uint16_t ticks);
void caen_v1x90_setWindowWidth_db(struct s_caen_v1x90 * s);
uint16_t caen_v1x90_getWindowWidth(struct s_caen_v1x90 * s);

void caen_v1x90_setWindowOffset(struct s_caen_v1x90 * s, uint16_t ticks);
void caen_v1x90_setWindowOffset_db(struct s_caen_v1x90 * s);
uint16_t caen_v1x90_getWindowOffset(struct s_caen_v1x90 * s);

void caen_v1x90_setRejectMargin(struct s_caen_v1x90 * s, uint16_t ticks);
void caen_v1x90_setRejectMargin_db(struct s_caen_v1x90 * s);
uint16_t caen_v1x90_getRejectMargin(struct s_caen_v1x90 * s);

void caen_v1x90_setSearchMargin(struct s_caen_v1x90 * s, uint16_t ticks);
void caen_v1x90_setSearchMargin_db(struct s_caen_v1x90 * s);
uint16_t caen_v1x90_getSearchMargin(struct s_caen_v1x90 * s);

void caen_v1x90_enableTriggerTimeSubtraction(struct s_caen_v1x90 * s, bool_t flag);
void caen_v1x90_enableTriggerTimeSubtraction_db(struct s_caen_v1x90 * s);
bool_t caen_v1x90_triggerTimeSubtractionEnabled(struct s_caen_v1x90 * s);

void caen_v1x90_readTriggerSettings(struct s_caen_v1x90 * s);

void caen_v1x90_setResolution(struct s_caen_v1x90 * s, uint16_t mode, uint16_t resolution, uint16_t pairWidth);
void caen_v1x90_setResolution_db(struct s_caen_v1x90 * s);
uint16_t caen_v1x90_getEdgeDetection(struct s_caen_v1x90 * s);
uint16_t caen_v1x90_getEdgeResolution(struct s_caen_v1x90 * s);
uint16_t caen_v1x90_getPairResolution(struct s_caen_v1x90 * s);
uint16_t caen_v1x90_getPairWidth(struct s_caen_v1x90 * s);

void caen_v1x90_readResolutionSettings(struct s_caen_v1x90 * s);

void caen_v1x90_setDeadTime(struct s_caen_v1x90 * s, uint16_t ticks);
void caen_v1x90_setDeadTime_db(struct s_caen_v1x90 * s);
uint16_t caen_v1x90_getDeadTime(struct s_caen_v1x90 * s);

void caen_v1x90_enableErrorMark(struct s_caen_v1x90 * s, bool_t flag);
void caen_v1x90_enableErrorMark_db(struct s_caen_v1x90 * s);
bool_t caen_v1x90_errorMarkEnabled(struct s_caen_v1x90 * s);

void caen_v1x90_setEventSize(struct s_caen_v1x90 * s, uint16_t hits);
void caen_v1x90_setEventSize_db(struct s_caen_v1x90 * s);
uint16_t caen_v1x90_getEventSize(struct s_caen_v1x90 * s);

void caen_v1x90_setFifoSize(struct s_caen_v1x90 * s, uint16_t words);
void caen_v1x90_setFifoSize_db(struct s_caen_v1x90 * s);
uint16_t caen_v1x90_getFifoSize(struct s_caen_v1x90 * s);

void caen_v1x90_enableHeaderTrailer(struct s_caen_v1x90 * s, bool_t flag);
void caen_v1x90_enableHeaderTrailer_db(struct s_caen_v1x90 * s);
bool_t caen_v1x90_headerTrailerEnabled(struct s_caen_v1x90 * s);

void caen_v1x90_enableExtendedTriggerTag(struct s_caen_v1x90 * s, bool_t flag);
void caen_v1x90_enableExtendedTriggerTag_db(struct s_caen_v1x90 * s);
bool_t caen_v1x90_extendedTriggerTagEnabled(struct s_caen_v1x90 * s);

void caen_v1x90_enableEmptyEvent(struct s_caen_v1x90 * s, bool_t flag);
void caen_v1x90_enableEmptyEvent_db(struct s_caen_v1x90 * s);
bool_t caen_v1x90_emptyEventEnabled(struct s_caen_v1x90 * s);

void caen_v1x90_enableEventFifo(struct s_caen_v1x90 * s, bool_t flag);
void caen_v1x90_enableEventFifo_db(struct s_caen_v1x90 * s);
bool_t caen_v1x90_eventFifoEnabled(struct s_caen_v1x90 * s);

void caen_v1x90_setAlmostFullLevel(struct s_caen_v1x90 * s, uint16_t level);
void caen_v1x90_setAlmostFullLevel_db(struct s_caen_v1x90 * s);
uint16_t caen_v1x90_getAlmostFullLevel(struct s_caen_v1x90 * s);

void caen_v1x90_enableChannel(struct s_caen_v1x90 * s, uint16_t * chnPat);
void caen_v1x90_enableChannel_db(struct s_caen_v1x90 * s);
void caen_v1x90_getChannelPattern(struct s_caen_v1x90 * s);

void caen_v1x90_setControl(struct s_caen_v1x90 * s, uint16_t control);
uint16_t caen_v1x90_getControl(struct s_caen_v1x90 * s);

uint16_t caen_v1x90_geStatus(struct s_caen_v1x90 * s);

bool_t caen_v1x90_updateSettings(struct s_caen_v1x90 * s, char * updFile);

bool_t caen_v1x90_waitWrite(struct s_caen_v1x90 * s, uint16_t opCode);
bool_t caen_v1x90_waitRead(struct s_caen_v1x90 * s, uint16_t opCode);
bool_t caen_v1x90_readPending(struct s_caen_v1x90 * s, uint16_t opCode);

bool_t caen_v1x90_writeMicro(struct s_caen_v1x90 * s, uint16_t opCode, uint16_t data);
uint16_t caen_v1x90_writeMicroBlock(struct s_caen_v1x90 * s, uint16_t opCode, uint16_t * data, int nofWords);

uint16_t caen_v1x90_readMicro(struct s_caen_v1x90 * s, uint16_t opCode);
uint16_t caen_v1x90_readMicroBlock(struct s_caen_v1x90 * s, uint16_t opCode, uint16_t * data, int nofWords);

uint32_t caen_v1x90_read_config_rom(struct s_caen_v1x90 * s, uint16_t offset, int nofWords);

void caen_v1x90_softClear();
void caen_v1x90_reset();

void caen_v1x90_startAcq(struct s_caen_v1x90 * s);
void caen_v1x90_stopAcq(struct s_caen_v1x90 * s);

bool_t caen_v1x90_waitFifoReady(struct s_caen_v1x90 * s);
bool_t caen_v1x90_waitDataReady(struct s_caen_v1x90 * s);
int caen_v1x90_getEventWcFromFifo(struct s_caen_v1x90 * s);

int caen_v1x90_readout(struct s_caen_v1x90 * s, uint32_t * pointer);

#endif

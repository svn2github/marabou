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


s_caen_v1x90 * caen_v1x90_alloc(unsigned long vmeAddr, volatile unsigned char * base, char * moduleName, int serial);

bool_t caen_v1x90_moduleInfo(s_caen_v1x90 * s);
void caen_v1x90_setPrefix(s_caen_v1x90 * s, char * prefix);

bool_t caen_v1x90_fillStruct(s_caen_v1x90 * s, char * file);

void caen_v1x90_loadFromDb(s_caen_v1x90 * s);

bool_t caen_v1x90_dumpRegisters(s_caen_v1x90 * s, char * file);
void caen_v1x90_printDb(s_caen_v1x90 * s);

void caen_v1x90_reset(s_caen_v1x90 * s);

void caen_v1x90_setTriggerMatchingOn(s_caen_v1x90 * s, bool_t flag);
void caen_v1x90_setTriggerMatchingOn_db(s_caen_v1x90 * s);
bool_t caen_v1x90_triggerMatchingIsOn(s_caen_v1x90 * s);

void caen_v1x90_setWindowWidth(s_caen_v1x90 * s, uint16_t ticks);
void caen_v1x90_setWindowWidth_db(s_caen_v1x90 * s);
uint16_t caen_v1x90_getWindowWidth(s_caen_v1x90 * s);

void caen_v1x90_setWindowOffset(s_caen_v1x90 * s, uint16_t ticks);
void caen_v1x90_setWindowOffset_db(s_caen_v1x90 * s);
uint16_t caen_v1x90_getWindowOffset(s_caen_v1x90 * s);

void caen_v1x90_setRejectMargin(s_caen_v1x90 * s, uint16_t ticks);
void caen_v1x90_setRejectMargin_db(s_caen_v1x90 * s);
uint16_t caen_v1x90_getRejectMargin(s_caen_v1x90 * s);

void caen_v1x90_setSearchMargin(s_caen_v1x90 * s, uint16_t ticks);
void caen_v1x90_setSearchMargin_db(s_caen_v1x90 * s);
uint16_t caen_v1x90_getSearchMargin(s_caen_v1x90 * s);

void caen_v1x90_enableTriggerTimeSubtraction(s_caen_v1x90 * s, bool_t flag);
void caen_v1x90_enableTriggerTimeSubtraction_db(s_caen_v1x90 * s);
bool_t caen_v1x90_triggerTimeSubtractionEnabled(s_caen_v1x90 * s);

void caen_v1x90_readTriggerSettings(s_caen_v1x90 * s);

void caen_v1x90_setResolution(s_caen_v1x90 * s, uint16_t mode, uint16_t resolution, uint16_t pairWidth);
void caen_v1x90_setResolution_db(s_caen_v1x90 * s);
uint16_t caen_v1x90_getEdgeDetection(s_caen_v1x90 * s);
uint16_t caen_v1x90_getEdgeResolution(s_caen_v1x90 * s);
uint16_t caen_v1x90_getPairResolution(s_caen_v1x90 * s);
uint16_t caen_v1x90_getPairWidth(s_caen_v1x90 * s);

void caen_v1x90_readResolutionSettings(s_caen_v1x90 * s);

void caen_v1x90_setDeadTime(s_caen_v1x90 * s, uint16_t ticks);
void caen_v1x90_setDeadTime_db(s_caen_v1x90 * s);
uint16_t caen_v1x90_getDeadTime(s_caen_v1x90 * s);

void caen_v1x90_enableErrorMark(s_caen_v1x90 * s, bool_t flag);
void caen_v1x90_enableErrorMark_db(s_caen_v1x90 * s);
bool_t caen_v1x90_errorMarkEnabled(s_caen_v1x90 * s);

void caen_v1x90_setEventSize(s_caen_v1x90 * s, uint16_t hits);
void caen_v1x90_setEventSize_db(s_caen_v1x90 * s);
uint16_t caen_v1x90_getEventSize(s_caen_v1x90 * s);

void caen_v1x90_setFifoSize(s_caen_v1x90 * s, uint16_t words);
void caen_v1x90_setFifoSize_db(s_caen_v1x90 * s);
uint16_t caen_v1x90_getFifoSize(s_caen_v1x90 * s);

void caen_v1x90_enableHeaderTrailer(s_caen_v1x90 * s, bool_t flag);
void caen_v1x90_enableHeaderTrailer_db(s_caen_v1x90 * s);
bool_t caen_v1x90_headerTrailerEnabled(s_caen_v1x90 * s);

void caen_v1x90_enableExtendedTriggerTag(s_caen_v1x90 * s, bool_t flag);
void caen_v1x90_enableExtendedTriggerTag_db(s_caen_v1x90 * s);
bool_t caen_v1x90_extendedTriggerTagEnabled(s_caen_v1x90 * s);

void caen_v1x90_enableEmptyEvent(s_caen_v1x90 * s, bool_t flag);
void caen_v1x90_enableEmptyEvent_db(s_caen_v1x90 * s);
bool_t caen_v1x90_emptyEventEnabled(s_caen_v1x90 * s);

void caen_v1x90_enableEventFifo(s_caen_v1x90 * s, bool_t flag);
void caen_v1x90_enableEventFifo_db(s_caen_v1x90 * s);
bool_t caen_v1x90_eventFifoEnabled(s_caen_v1x90 * s);

void caen_v1x90_setAlmostFullLevel(s_caen_v1x90 * s, uint16_t level);
void caen_v1x90_setAlmostFullLevel_db(s_caen_v1x90 * s);
uint16_t caen_v1x90_getAlmostFullLevel(s_caen_v1x90 * s);

void caen_v1x90_enableChannel(s_caen_v1x90 * s, uint16_t * chnPat);
void caen_v1x90_enableChannel_db(s_caen_v1x90 * s);
void caen_v1x90_getChannelPattern(s_caen_v1x90 * s);

void caen_v1x90_setControl(s_caen_v1x90 * s, uint16_t control);
uint16_t caen_v1x90_getControl(s_caen_v1x90 * s);

uint16_t caen_v1x90_geStatus(s_caen_v1x90 * s);

bool_t caen_v1x90_updateSettings(s_caen_v1x90 * s, char * updFile);

bool_t caen_v1x90_waitWrite(s_caen_v1x90 * s, uint16_t opCode);
bool_t caen_v1x90_waitRead(s_caen_v1x90 * s, uint16_t opCode);
bool_t caen_v1x90_readPending(s_caen_v1x90 * s, uint16_t opCode);

bool_t caen_v1x90_writeMicro(s_caen_v1x90 * s, uint16_t opCode, uint16_t data);
uint16_t caen_v1x90_writeMicroBlock(s_caen_v1x90 * s, uint16_t opCode, uint16_t * data, int nofWords);

uint16_t caen_v1x90_readMicro(s_caen_v1x90 * s, uint16_t opCode);
uint16_t caen_v1x90_readMicroBlock(s_caen_v1x90 * s, uint16_t opCode, uint16_t * data, int nofWords);

uint32_t caen_v1x90_read_config_rom(s_caen_v1x90 * s, uint16_t offset, int nofWords);

void caen_v1x90_softClear();
void caen_v1x90_reset();

void caen_v1x90_startAcq(s_caen_v1x90 * s);
void caen_v1x90_stopAcq(s_caen_v1x90 * s);

bool_t caen_v1x90_waitFifoReady(s_caen_v1x90 * s);
bool_t caen_v1x90_waitDataReady(s_caen_v1x90 * s);
int caen_v1x90_getEventWcFromFifo(s_caen_v1x90 * s);

int caen_v1x90_readout(s_caen_v1x90 * s, uint32_t * pointer);

#endif

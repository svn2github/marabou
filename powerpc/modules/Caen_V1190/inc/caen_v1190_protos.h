#ifndef __CAEN_V1190_PROTOS_H__
#define __CAEN_V1190_PROTOS_H__

/*_______________________________________________________________[HEADER FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			caen_v1190_protos.h
//! \brief			Definitions for CAEN V1190 TDC
//! \details		Prototypes for Caen V1190
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.4 $     
//! $Date: 2009-07-15 11:45:36 $
////////////////////////////////////////////////////////////////////////////*/


struct s_caen_v1190 * caen_v1190_alloc(unsigned long vmeAddr, volatile unsigned char * base, char * moduleName, int serial);

bool_t caen_v1190_moduleInfo(struct s_caen_v1190 * s);
void caen_v1190_setPrefix(struct s_caen_v1190 * s, char * prefix);

bool_t caen_v1190_fillStruct(struct s_caen_v1190 * s, char * file);

void caen_v1190_loadFromDb(struct s_caen_v1190 * s);

bool_t caen_v1190_dumpRegisters(struct s_caen_v1190 * s, char * file);
void caen_v1190_printDb(struct s_caen_v1190 * s);

void caen_v1190_reset(struct s_caen_v1190 * s);

void caen_v1190_setTriggerMatchingOn(struct s_caen_v1190 * s, bool_t flag);
void caen_v1190_setTriggerMatchingOn_db(struct s_caen_v1190 * s);
bool_t caen_v1190_triggerMatchingIsOn(struct s_caen_v1190 * s);

void caen_v1190_setWindowWidth(struct s_caen_v1190 * s, uint16_t ticks);
void caen_v1190_setWindowWidth_db(struct s_caen_v1190 * s);
uint16_t caen_v1190_getWindowWidth(struct s_caen_v1190 * s);

void caen_v1190_setWindowOffset(struct s_caen_v1190 * s, uint16_t ticks);
void caen_v1190_setWindowOffset_db(struct s_caen_v1190 * s);
uint16_t caen_v1190_getWindowOffset(struct s_caen_v1190 * s);

void caen_v1190_setRejectMargin(struct s_caen_v1190 * s, uint16_t ticks);
void caen_v1190_setRejectMargin_db(struct s_caen_v1190 * s);
uint16_t caen_v1190_getRejectMargin(struct s_caen_v1190 * s);

void caen_v1190_setSearchMargin(struct s_caen_v1190 * s, uint16_t ticks);
void caen_v1190_setSearchMargin_db(struct s_caen_v1190 * s);
uint16_t caen_v1190_getSearchMargin(struct s_caen_v1190 * s);

void caen_v1190_enableTriggerTimeSubtraction(struct s_caen_v1190 * s, bool_t flag);
void caen_v1190_enableTriggerTimeSubtraction_db(struct s_caen_v1190 * s);
bool_t caen_v1190_triggerTimeSubtractionEnabled(struct s_caen_v1190 * s);

void caen_v1190_readTriggerSettings(struct s_caen_v1190 * s);

void caen_v1190_setResolution(struct s_caen_v1190 * s, uint16_t mode, uint16_t resolution, uint16_t pairWidth);
void caen_v1190_setResolution_db(struct s_caen_v1190 * s);
uint16_t caen_v1190_getEdgeDetection(struct s_caen_v1190 * s);
uint16_t caen_v1190_getEdgeResolution(struct s_caen_v1190 * s);
uint16_t caen_v1190_getPairResolution(struct s_caen_v1190 * s);
uint16_t caen_v1190_getPairWidth(struct s_caen_v1190 * s);

void caen_v1190_readResolutionSettings(struct s_caen_v1190 * s);

void caen_v1190_setDeadTime(struct s_caen_v1190 * s, uint16_t ticks);
void caen_v1190_setDeadTime_db(struct s_caen_v1190 * s);
uint16_t caen_v1190_getDeadTime(struct s_caen_v1190 * s);

void caen_v1190_setEventSize(struct s_caen_v1190 * s, uint16_t hits);
void caen_v1190_setEventSize_db(struct s_caen_v1190 * s);
uint16_t caen_v1190_getEventSize(struct s_caen_v1190 * s);

void caen_v1190_setFifoSize(struct s_caen_v1190 * s, uint16_t words);
void caen_v1190_setFifoSize_db(struct s_caen_v1190 * s);
uint16_t caen_v1190_getFifoSize(struct s_caen_v1190 * s);

void caen_v1190_enableHeaderTrailer(struct s_caen_v1190 * s, bool_t flag);
void caen_v1190_enableHeaderTrailer_db(struct s_caen_v1190 * s);
bool_t caen_v1190_headerTrailerEnabled(struct s_caen_v1190 * s);

void caen_v1190_enableExtendedTriggerTag(struct s_caen_v1190 * s, bool_t flag);
void caen_v1190_enableExtendedTriggerTag_db(struct s_caen_v1190 * s);
bool_t caen_v1190_extendedTriggerTagEnabled(struct s_caen_v1190 * s);

void caen_v1190_enableEmptyEvent(struct s_caen_v1190 * s, bool_t flag);
void caen_v1190_enableEmptyEvent_db(struct s_caen_v1190 * s);
bool_t caen_v1190_emptyEventEnabled(struct s_caen_v1190 * s);

void caen_v1190_enableEventFifo(struct s_caen_v1190 * s, bool_t flag);
void caen_v1190_enableEventFifo_db(struct s_caen_v1190 * s);
bool_t caen_v1190_eventFifoEnabled(struct s_caen_v1190 * s);

void caen_v1190_setAlmostFullLevel(struct s_caen_v1190 * s, uint16_t level);
void caen_v1190_setAlmostFullLevel_db(struct s_caen_v1190 * s);
uint16_t caen_v1190_getAlmostFullLevel(struct s_caen_v1190 * s);

void caen_v1190_enableChannel(struct s_caen_v1190 * s, uint16_t * chnPat);
void caen_v1190_enableChannel_db(struct s_caen_v1190 * s);
void caen_v1190_getChannelPattern(struct s_caen_v1190 * s);

void caen_v1190_setControl(struct s_caen_v1190 * s, uint16_t control);
uint16_t caen_v1190_getControl(struct s_caen_v1190 * s);

uint16_t caen_v1190_geStatus(struct s_caen_v1190 * s);

bool_t caen_v1190_updateSettings(struct s_caen_v1190 * s, char * updFile);

bool_t caen_v1190_waitWrite(struct s_caen_v1190 * s, uint16_t opCode);
bool_t caen_v1190_waitRead(struct s_caen_v1190 * s, uint16_t opCode);
bool_t caen_v1190_readPending(struct s_caen_v1190 * s, uint16_t opCode);

bool_t caen_v1190_writeMicro(struct s_caen_v1190 * s, uint16_t opCode, uint16_t data);
uint16_t caen_v1190_writeMicroBlock(struct s_caen_v1190 * s, uint16_t opCode, uint16_t * data, int nofWords);

uint16_t caen_v1190_readMicro(struct s_caen_v1190 * s, uint16_t opCode);
uint16_t caen_v1190_readMicroBlock(struct s_caen_v1190 * s, uint16_t opCode, uint16_t * data, int nofWords);

uint32_t caen_v1190_read_config_rom(struct s_caen_v1190 * s, uint16_t offset, int nofWords);

void caen_v1190_softClear();
void caen_v1190_reset();

void caen_v1190_startAcq(struct s_caen_v1190 * s);
void caen_v1190_stopAcq(struct s_caen_v1190 * s);

bool_t caen_v1190_waitFifoReady(struct s_caen_v1190 * s);
bool_t caen_v1190_waitDataReady(struct s_caen_v1190 * s);
int caen_v1190_getEventWcFromFifo(struct s_caen_v1190 * s);

int caen_v1190_readout(struct s_caen_v1190 * s, uint32_t * pointer);

#endif

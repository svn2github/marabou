#ifndef __MADC32_PROTOS_H__
#define __MADC32_PROTOS_H__

#include "mapping_database.h"
#include "madc32_database.h"
#include "madc32.h"

/*_______________________________________________________________[HEADER FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			madc32_protos.h
//! \brief			Definitions for Mesytec Madc32 ADC
//! \details		Prototypes for Mesytec Madc32
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.14 $
//! $Date: 2011-04-29 07:19:03 $
////////////////////////////////////////////////////////////////////////////*/


struct s_madc32 * madc32_alloc(char * moduleName, struct s_mapDescr * md, int serial);

void madc32_moduleInfo(struct s_madc32 * s);
void madc32_setPrefix(struct s_madc32 * s, char * prefix);

bool_t madc32_fillStruct(struct s_madc32 * s, char * file);

void madc32_loadFromDb(struct s_madc32 * s, uint32_t chnPattern);

void madc32_initialize(struct s_madc32 * s);

void madc32_enableBLT(struct s_madc32 * s);
bool_t madc32_useBLT(struct s_madc32 * s);

bool_t madc32_dumpRegisters(struct s_madc32 * s, char * file);
bool_t madc32_dumpRaw(struct s_madc32 * s, char * file);
void madc32_printDb(struct s_madc32 * s);

void madc32_soft_reset(struct s_madc32 * s);

void madc32_initDefaults(struct s_madc32 * s);

void madc32_setThreshold(struct s_madc32 * s, uint16_t channel,  uint16_t thresh);
void madc32_setThreshold_db(struct s_madc32 * s, uint16_t channel);
uint16_t madc32_getThreshold(struct s_madc32 * s, uint16_t channel);

void madc32_setAddrReg(struct s_madc32 * s, uint16_t vmeAddr);
void madc32_setAddrReg_db(struct s_madc32 * s);
uint16_t madc32_getAddrReg(struct s_madc32 * s);

void madc32_setModuleId(struct s_madc32 * s, uint16_t id);
void madc32_setModuleId_db(struct s_madc32 * s);
uint16_t madc32_getModuleId(struct s_madc32 * s);

uint16_t madc32_getFifoLength(struct s_madc32 * s);

void madc32_setDataWidth(struct s_madc32 * s, uint16_t format);
void madc32_setDataWidth_db(struct s_madc32 * s);
uint16_t madc32_getDataWidth(struct s_madc32 * s);

void madc32_setMultiEvent(struct s_madc32 * s, uint16_t mode);
void madc32_setMultiEvent_db(struct s_madc32 * s);
uint16_t madc32_getMultiEvent(struct s_madc32 * s);

void madc32_setXferData(struct s_madc32 * s, uint16_t words);
void madc32_setXferData_db(struct s_madc32 * s);
uint16_t madc32_getXferData(struct s_madc32 * s);

void madc32_setMarkingType(struct s_madc32 * s, uint16_t type);
void madc32_setMarkingType_db(struct s_madc32 * s);
uint16_t madc32_getMarkingType(struct s_madc32 * s);

void madc32_setBankOperation(struct s_madc32 * s, uint16_t oper);
void madc32_setBankOperation_db(struct s_madc32 * s);
uint16_t madc32_getBankOperation(struct s_madc32 * s);

void madc32_setAdcResolution(struct s_madc32 * s, uint16_t res);
void madc32_setAdcResolution_db(struct s_madc32 * s);
uint16_t madc32_getAdcResolution(struct s_madc32 * s);

void madc32_setOutputFormat(struct s_madc32 * s, uint16_t format);
void madc32_setOutputFormat_db(struct s_madc32 * s);
uint16_t madc32_getOutputFormat(struct s_madc32 * s);

void madc32_setAdcOverride(struct s_madc32 * s, uint16_t over);
void madc32_setAdcOverride_db(struct s_madc32 * s);
uint16_t madc32_getAdcOverride(struct s_madc32 * s);

void madc32_setSlidingScaleOff(struct s_madc32 * s, bool_t flag);
void madc32_setSlidingScaleOff_db(struct s_madc32 * s);
bool_t madc32_getSlidingScaleOff(struct s_madc32 * s);

void madc32_setSkipOutOfRange(struct s_madc32 * s, bool_t flag);
void madc32_setSkipOutOfRange_db(struct s_madc32 * s);
bool_t madc32_getSkipOutOfRange(struct s_madc32 * s);

void madc32_setIgnoreThresholds(struct s_madc32 * s, bool_t flag);
void madc32_setIgnoreThresholds_db(struct s_madc32 * s);
bool_t madc32_getIgnoreThresholds(struct s_madc32 * s);

void madc32_setHoldDelay(struct s_madc32 * s, uint16_t gg, uint16_t delay);
void madc32_setHoldDelay_db(struct s_madc32 * s, uint16_t gg);
uint16_t madc32_getHoldDelay(struct s_madc32 * s, uint16_t gg);

void madc32_setHoldWidth(struct s_madc32 * s, uint16_t gg, uint16_t width);
void madc32_setHoldWidth_db(struct s_madc32 * s, uint16_t gg);
uint16_t madc32_getHoldWidth(struct s_madc32 * s, uint16_t gg);

void madc32_useGG(struct s_madc32 * s, uint16_t gg);
void madc32_useGG_db(struct s_madc32 * s);
uint16_t madc32_getGGUsed(struct s_madc32 * s);

void madc32_setInputRange(struct s_madc32 * s, uint16_t range);
void madc32_setInputRange_db(struct s_madc32 * s);
uint16_t madc32_getInputRange(struct s_madc32 * s);

void madc32_setEclTerm(struct s_madc32 * s, uint16_t term);
void madc32_setEclTerm_db(struct s_madc32 * s);
uint16_t madc32_getEclTerm(struct s_madc32 * s);

void madc32_setEclG1Osc(struct s_madc32 * s, uint16_t go);
void madc32_setEclG1Osc_db(struct s_madc32 * s);
uint16_t madc32_getEclG1Osc(struct s_madc32 * s);

void madc32_setEclFclRts(struct s_madc32 * s, uint16_t fr);
void madc32_setEclFclRts_db(struct s_madc32 * s);
uint16_t madc32_getEclFclRts(struct s_madc32 * s);

void madc32_setNimG1Osc(struct s_madc32 * s, uint16_t go);
void madc32_setNimG1Osc_db(struct s_madc32 * s);
uint16_t madc32_getNimG1Osc(struct s_madc32 * s);

void madc32_setNimFclRts(struct s_madc32 * s, uint16_t fr);
void madc32_setNimFclRts_db(struct s_madc32 * s);
uint16_t madc32_getNimFclRts(struct s_madc32 * s);

void madc32_setNimBusy(struct s_madc32 * s, uint16_t busy);
void madc32_setNimBusy_db(struct s_madc32 * s);
uint16_t madc32_getNimBusy(struct s_madc32 * s);

void madc32_setPulserStatus(struct s_madc32 * s, uint16_t mode);
void madc32_setPulserStatus_db(struct s_madc32 * s);
uint16_t madc32_getPulserStatus(struct s_madc32 * s);

void madc32_setTsSource(struct s_madc32 * s, uint16_t source);
void madc32_setTsSource_db(struct s_madc32 * s);
uint16_t madc32_getTsSource(struct s_madc32 * s);

void madc32_setTsDivisor(struct s_madc32 * s, uint16_t div);
void madc32_setTsDivisor_db(struct s_madc32 * s);
uint16_t madc32_getTsDivisor(struct s_madc32 * s);

int madc32_readout(struct s_madc32 * s, uint32_t * pointer);
bool_t madc32_dataReady(struct s_madc32 * s);
void madc_resetReadout(struct s_madc32 * s);

int madc32_readTimeB(struct s_madc32 * s, uint32_t * pointer);

bool_t madc32_testBusError(struct s_madc32 * s);
void madc32_enableBusError(struct s_madc32 * s);
void madc32_disableBusError(struct s_madc32 * s);

void madc32_startAcq(struct s_madc32 * s);
void madc32_stopAcq(struct s_madc32 * s);

void madc32_resetFifo(struct s_madc32 * s);
void madc32_resetTimestamp(struct s_madc32 * s);

void madc32_setMcstSignature(struct s_madc32 * s, unsigned long Signature);
uint16_t madc32_getMcstSignature(struct s_madc32 * s);

void madc32_setCbltSignature(struct s_madc32 * s, unsigned long Signature);
uint16_t madc32_getCbltSignature(struct s_madc32 * s);

void madc32_setFirstInChain(struct s_madc32 * s);
bool_t madc32_isFirstInChain(struct s_madc32 * s);

void madc32_setLastInChain(struct s_madc32 * s);
bool_t madc32_isLastInChain(struct s_madc32 * s);
void madc32_setMiddleOfChain(struct s_madc32 * s);
bool_t madc32_isMiddleOfChain(struct s_madc32 * s);

void madc32_setMcstEnable(struct s_madc32 * s);
void madc32_setMcstDisable(struct s_madc32 * s);
bool_t madc32_mcstIsEnabled(struct s_madc32 * s);
void madc32_setCbltEnable(struct s_madc32 * s);
void madc32_setCbltDisable(struct s_madc32 * s);
bool_t madc32_cbltIsEnabled(struct s_madc32 * s);

void madc32_setMcstCblt_db(struct s_madc32 * s);

void madc32_startAcq_mcst(struct s_madc32 * s);
void madc32_stopAcq_msct(struct s_madc32 * s);
void madc32_resetFifo_mcst(struct s_madc32 * s);
void madc32_resetTimestamp_mcst(struct s_madc32 * s);
void madc_resetReadout_mcst(struct s_madc32 * s);

uint32_t * madc32_repairRawData(struct s_madc32 * s, uint32_t * pointer, uint32_t * dataStart);
#endif

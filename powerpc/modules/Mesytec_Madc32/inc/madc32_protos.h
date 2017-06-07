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


s_madc32 * madc32_alloc(char * moduleName, s_mapDescr * md, int serial);

void madc32_moduleInfo(s_madc32 * s);
void madc32_setPrefix(s_madc32 * s, char * prefix);

bool_t madc32_fillStruct(s_madc32 * s, char * file);

void madc32_loadFromDb(s_madc32 * s, uint32_t chnPattern);

bool_t madc32_initBLT(s_madc32 * s, bool_t flag);
void madc32_enableBLT(s_madc32 * s);
bool_t madc32_useBLT(s_madc32 * s);

bool_t madc32_dumpRegisters(s_madc32 * s, char * file);
bool_t madc32_dumpRaw(s_madc32 * s, char * file);
void madc32_printDb(s_madc32 * s);

void madc32_soft_reset(s_madc32 * s);

void madc32_initDefaults(s_madc32 * s);

void madc32_setThreshold(s_madc32 * s, uint16_t channel,  uint16_t thresh);
void madc32_setThreshold_db(s_madc32 * s, uint16_t channel);
uint16_t madc32_getThreshold(s_madc32 * s, uint16_t channel);

void madc32_setAddrReg(s_madc32 * s, uint16_t vmeAddr);
void madc32_setAddrReg_db(s_madc32 * s);
uint16_t madc32_getAddrReg(s_madc32 * s);

void madc32_setModuleId(s_madc32 * s, uint16_t id);
void madc32_setModuleId_db(s_madc32 * s);
uint16_t madc32_getModuleId(s_madc32 * s);

uint16_t madc32_getFifoLength(s_madc32 * s);

void madc32_setDataWidth(s_madc32 * s, uint16_t format);
void madc32_setDataWidth_db(s_madc32 * s);
uint16_t madc32_getDataWidth(s_madc32 * s);

void madc32_setMultiEvent(s_madc32 * s, uint16_t mode);
void madc32_setMultiEvent_db(s_madc32 * s);
uint16_t madc32_getMultiEvent(s_madc32 * s);

void madc32_setXferData(s_madc32 * s, uint16_t words);
void madc32_setXferData_db(s_madc32 * s);
uint16_t madc32_getXferData(s_madc32 * s);

void madc32_setMarkingType(s_madc32 * s, uint16_t type);
void madc32_setMarkingType_db(s_madc32 * s);
uint16_t madc32_getMarkingType(s_madc32 * s);

void madc32_setBankOperation(s_madc32 * s, uint16_t oper);
void madc32_setBankOperation_db(s_madc32 * s);
uint16_t madc32_getBankOperation(s_madc32 * s);

void madc32_setAdcResolution(s_madc32 * s, uint16_t res);
void madc32_setAdcResolution_db(s_madc32 * s);
uint16_t madc32_getAdcResolution(s_madc32 * s);

void madc32_setOutputFormat(s_madc32 * s, uint16_t format);
void madc32_setOutputFormat_db(s_madc32 * s);
uint16_t madc32_getOutputFormat(s_madc32 * s);

void madc32_setAdcOverride(s_madc32 * s, uint16_t over);
void madc32_setAdcOverride_db(s_madc32 * s);
uint16_t madc32_getAdcOverride(s_madc32 * s);

void madc32_setSlidingScaleOff(s_madc32 * s, bool_t flag);
void madc32_setSlidingScaleOff_db(s_madc32 * s);
bool_t madc32_getSlidingScaleOff(s_madc32 * s);

void madc32_setSkipOutOfRange(s_madc32 * s, bool_t flag);
void madc32_setSkipOutOfRange_db(s_madc32 * s);
bool_t madc32_getSkipOutOfRange(s_madc32 * s);

void madc32_setIgnoreThresholds(s_madc32 * s, bool_t flag);
void madc32_setIgnoreThresholds_db(s_madc32 * s);
bool_t madc32_getIgnoreThresholds(s_madc32 * s);

void madc32_setHoldDelay(s_madc32 * s, uint16_t gg, uint16_t delay);
void madc32_setHoldDelay_db(s_madc32 * s, uint16_t gg);
uint16_t madc32_getHoldDelay(s_madc32 * s, uint16_t gg);

void madc32_setHoldWidth(s_madc32 * s, uint16_t gg, uint16_t width);
void madc32_setHoldWidth_db(s_madc32 * s, uint16_t gg);
uint16_t madc32_getHoldWidth(s_madc32 * s, uint16_t gg);

void madc32_useGG(s_madc32 * s, uint16_t gg);
void madc32_useGG_db(s_madc32 * s);
uint16_t madc32_getGGUsed(s_madc32 * s);

void madc32_setInputRange(s_madc32 * s, uint16_t range);
void madc32_setInputRange_db(s_madc32 * s);
uint16_t madc32_getInputRange(s_madc32 * s);

void madc32_setEclTerm(s_madc32 * s, uint16_t term);
void madc32_setEclTerm_db(s_madc32 * s);
uint16_t madc32_getEclTerm(s_madc32 * s);

void madc32_setEclG1Osc(s_madc32 * s, uint16_t go);
void madc32_setEclG1Osc_db(s_madc32 * s);
uint16_t madc32_getEclG1Osc(s_madc32 * s);

void madc32_setEclFclRts(s_madc32 * s, uint16_t fr);
void madc32_setEclFclRts_db(s_madc32 * s);
uint16_t madc32_getEclFclRts(s_madc32 * s);

void madc32_setNimG1Osc(s_madc32 * s, uint16_t go);
void madc32_setNimG1Osc_db(s_madc32 * s);
uint16_t madc32_getNimG1Osc(s_madc32 * s);

void madc32_setNimFclRts(s_madc32 * s, uint16_t fr);
void madc32_setNimFclRts_db(s_madc32 * s);
uint16_t madc32_getNimFclRts(s_madc32 * s);

void madc32_setNimBusy(s_madc32 * s, uint16_t busy);
void madc32_setNimBusy_db(s_madc32 * s);
uint16_t madc32_getNimBusy(s_madc32 * s);

void madc32_setPulserStatus(s_madc32 * s, uint16_t mode);
void madc32_setPulserStatus_db(s_madc32 * s);
uint16_t madc32_getPulserStatus(s_madc32 * s);

void madc32_setTsSource(s_madc32 * s, uint16_t source);
void madc32_setTsSource_db(s_madc32 * s);
uint16_t madc32_getTsSource(s_madc32 * s);

void madc32_setTsDivisor(s_madc32 * s, uint16_t div);
void madc32_setTsDivisor_db(s_madc32 * s);
uint16_t madc32_getTsDivisor(s_madc32 * s);

int madc32_readout(s_madc32 * s, uint32_t * pointer);
bool_t madc32_dataReady(s_madc32 * s);

int madc32_readTimeB(s_madc32 * s, uint32_t * pointer);

bool_t madc32_testBusError(s_madc32 * s);
void madc32_enableBusError(s_madc32 * s);
void madc32_disableBusError(s_madc32 * s);

void madc32_startAcq(s_madc32 * s);
void madc32_stopAcq(s_madc32 * s);

void madc32_resetFifo(s_madc32 * s);
void madc32_resetTimestamp(s_madc32 * s);
void madc_resetReadout(s_madc32 * s);

void madc32_resetFifo_mcst(s_madc32 * s);
void madc32_resetTimestamp_mcst(s_madc32 * s);
void madc_resetReadout_mcst(s_madc32 * s);

void madc32_initMCST(s_madc32 * s);

void madc32_setMcstSignature(s_madc32 * s, unsigned long Signature);
uint16_t madc32_getMcstSignature(s_madc32 * s);

bool_t madc32_isMcstMaster(s_madc32 * s);

void madc32_setCbltSignature(s_madc32 * s, unsigned long Signature);
uint16_t madc32_getCbltSignature(s_madc32 * s);

void madc32_setFirstInCbltChain(s_madc32 * s);
bool_t madc32_isFirstInCbltChain(s_madc32 * s);

void madc32_setLastInCbltChain(s_madc32 * s);
bool_t madc32_isLastInCbltChain(s_madc32 * s);

void madc32_setMiddleOfCbltChain(s_madc32 * s);
bool_t madc32_isMiddleOfCbltChain(s_madc32 * s);

void madc32_setMcstEnable(s_madc32 * s);
void madc32_setMcstDisable(s_madc32 * s);
bool_t madc32_mcstIsEnabled(s_madc32 * s);
void madc32_setCbltEnable(s_madc32 * s);
void madc32_setCbltDisable(s_madc32 * s);
bool_t madc32_cbltIsEnabled(s_madc32 * s);

void madc32_setMcstCblt_db(s_madc32 * s);

uint32_t * madc32_repairRawData(s_madc32 * s, uint32_t * pointer, uint32_t * dataStart);
#endif

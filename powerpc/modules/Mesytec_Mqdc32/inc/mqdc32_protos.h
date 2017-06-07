#ifndef __MQDC32_PROTOS_H__
#define __MQDC32_PROTOS_H__

#include "mapping_database.h"
#include "mqdc32_database.h"
#include "mqdc32.h"

/*_______________________________________________________________[HEADER FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			mqdc32_protos.h
//! \brief			Definitions for Mesytec mqdc32 ADC
//! \details		Prototypes for Mesytec mqdc32
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.14 $
//! $Date: 2011-04-29 07:19:03 $
////////////////////////////////////////////////////////////////////////////*/


s_mqdc32 * mqdc32_alloc(char * moduleName, s_mapDescr * md, int serial);

void mqdc32_moduleInfo(s_mqdc32 * s);
void mqdc32_setPrefix(s_mqdc32 * s, char * prefix);

bool_t mqdc32_fillStruct(s_mqdc32 * s, char * file);

void mqdc32_loadFromDb(s_mqdc32 * s, uint32_t chnPattern);

bool_t mqdc32_initBLT(s_mqdc32 * s, bool_t flag);
void mqdc32_enableBLT(s_mqdc32 * s);
bool_t mqdc32_useBLT(s_mqdc32 * s);

bool_t mqdc32_dumpRegisters(s_mqdc32 * s, char * file);
bool_t mqdc32_dumpRaw(s_mqdc32 * s, char * file);
void mqdc32_printDb(s_mqdc32 * s);

void mqdc32_soft_reset(s_mqdc32 * s);

void mqdc32_initDefaults(s_mqdc32 * s);

void mqdc32_setThreshold(s_mqdc32 * s, uint16_t channel,  uint16_t thresh);
void mqdc32_setThreshold_db(s_mqdc32 * s, uint16_t channel);
uint16_t mqdc32_getThreshold(s_mqdc32 * s, uint16_t channel);

void mqdc32_setAddrReg(s_mqdc32 * s, uint16_t vmeAddr);
void mqdc32_setAddrReg_db(s_mqdc32 * s);
uint16_t mqdc32_getAddrReg(s_mqdc32 * s);

void mqdc32_setMcstCblt_db(s_mqdc32 * s);

void mqdc32_setModuleId(s_mqdc32 * s, uint16_t id);
void mqdc32_setModuleId_db(s_mqdc32 * s);
uint16_t mqdc32_getModuleId(s_mqdc32 * s);

uint16_t mqdc32_getFifoLength(s_mqdc32 * s);

void mqdc32_setDataWidth(s_mqdc32 * s, uint16_t format);
void mqdc32_setDataWidth_db(s_mqdc32 * s);
uint16_t mqdc32_getDataWidth(s_mqdc32 * s);

void mqdc32_setMultiEvent(s_mqdc32 * s, uint16_t mode);
void mqdc32_setMultiEvent_db(s_mqdc32 * s);
uint16_t mqdc32_getMultiEvent(s_mqdc32 * s);

void mqdc32_setXferData(s_mqdc32 * s, uint16_t words);
void mqdc32_setXferData_db(s_mqdc32 * s);
uint16_t mqdc32_getXferData(s_mqdc32 * s);

void mqdc32_setMarkingType(s_mqdc32 * s, uint16_t type);
void mqdc32_setMarkingType_db(s_mqdc32 * s);
uint16_t mqdc32_getMarkingType(s_mqdc32 * s);

void mqdc32_setBankOperation(s_mqdc32 * s, uint16_t oper);
void mqdc32_setBankOperation_db(s_mqdc32 * s);
uint16_t mqdc32_getBankOperation(s_mqdc32 * s);

void mqdc32_setAdcResolution(s_mqdc32 * s, uint16_t res);
void mqdc32_setAdcResolution_db(s_mqdc32 * s);
uint16_t mqdc32_getAdcResolution(s_mqdc32 * s);

void mqdc32_setBankOffset(s_mqdc32 * s, uint16_t bank, uint16_t offset);
void mqdc32_setBankOffset_db(s_mqdc32 * s, uint16_t bank);
uint16_t mqdc32_getBankOffset(s_mqdc32 * s, uint16_t bank);

void mqdc32_setGateLimit(s_mqdc32 * s, uint16_t bank, uint16_t limit);
void mqdc32_setGateLimit_db(s_mqdc32 * s, uint16_t bank);
uint16_t mqdc32_getGateLimit(s_mqdc32 * s, uint16_t bank);

void mqdc32_setSlidingScaleOff(s_mqdc32 * s, bool_t flag);
void mqdc32_setSlidingScaleOff_db(s_mqdc32 * s);
bool_t mqdc32_getSlidingScaleOff(s_mqdc32 * s);

void mqdc32_setSkipOutOfRange(s_mqdc32 * s, bool_t flag);
void mqdc32_setSkipOutOfRange_db(s_mqdc32 * s);
bool_t mqdc32_getSkipOutOfRange(s_mqdc32 * s);

void mqdc32_setIgnoreThresholds(s_mqdc32 * s, bool_t flag);
void mqdc32_setIgnoreThresholds_db(s_mqdc32 * s);
bool_t mqdc32_getIgnoreThresholds(s_mqdc32 * s);

void mqdc32_setInputCoupling(s_mqdc32 * s, uint16_t range);
void mqdc32_setInputCoupling_db(s_mqdc32 * s);
uint16_t mqdc32_getInputCoupling(s_mqdc32 * s);

void mqdc32_setEclTerm(s_mqdc32 * s, uint16_t term);
void mqdc32_setEclTerm_db(s_mqdc32 * s);
uint16_t mqdc32_getEclTerm(s_mqdc32 * s);

void mqdc32_setEclG1Osc(s_mqdc32 * s, uint16_t go);
void mqdc32_setEclG1Osc_db(s_mqdc32 * s);
uint16_t mqdc32_getEclG1Osc(s_mqdc32 * s);

void mqdc32_setEclFclRts(s_mqdc32 * s, uint16_t fr);
void mqdc32_setEclFclRts_db(s_mqdc32 * s);
uint16_t mqdc32_getEclFclRts(s_mqdc32 * s);

void mqdc32_setGateSelect(s_mqdc32 * s, uint16_t nimOrEcl);
void mqdc32_setGateSelect_db(s_mqdc32 * s);
uint16_t mqdc32_getGateSelect(s_mqdc32 * s);

void mqdc32_setNimG1Osc(s_mqdc32 * s, uint16_t go);
void mqdc32_setNimG1Osc_db(s_mqdc32 * s);
uint16_t mqdc32_getNimG1Osc(s_mqdc32 * s);

void mqdc32_setNimFclRts(s_mqdc32 * s, uint16_t fr);
void mqdc32_setNimFclRts_db(s_mqdc32 * s);
uint16_t mqdc32_getNimFclRts(s_mqdc32 * s);

void mqdc32_setNimBusy(s_mqdc32 * s, uint16_t busy);
void mqdc32_setNimBusy_db(s_mqdc32 * s);
uint16_t mqdc32_getNimBusy(s_mqdc32 * s);

void mqdc32_setPulserStatus(s_mqdc32 * s, uint16_t mode);
void mqdc32_setulserStatus_db(s_mqdc32 * s);
uint16_t mqdc32_getPulserStatus(s_mqdc32 * s);

void mqdc32_setTsSource(s_mqdc32 * s, uint16_t source);
void mqdc32_setTsSource_db(s_mqdc32 * s);
uint16_t mqdc32_getTsSource(s_mqdc32 * s);

void mqdc32_setTsDivisor(s_mqdc32 * s, uint16_t div);
void mqdc32_setTsDivisor_db(s_mqdc32 * s);
uint16_t mqdc32_getTsDivisor(s_mqdc32 * s);

void mqdc32_setMultLimit(s_mqdc32 * s, uint16_t bnk, uint16_t llim, uint16_t hlim);
void mqdc32_setMultLimit_db(s_mqdc32 * s, uint16_t bnk);
uint16_t mqdc32_getMultHighLimit(s_mqdc32 * s, uint16_t bnk);
uint16_t mqdc32_getMultLowLimit(s_mqdc32 * s, uint16_t bnk);

int mqdc32_readout(s_mqdc32 * s, uint32_t * pointer);
bool_t mqdc32_dataReady(s_mqdc32 * s);
void mqdc_resetReadout(s_mqdc32 * s);

int mqdc32_readTimeB(s_mqdc32 * s, uint32_t * pointer);

bool_t mqdc32_testBusError(s_mqdc32 * s);
void mqdc32_enableBusError(s_mqdc32 * s);
void mqdc32_disableBusError(s_mqdc32 * s);

void mqdc32_startAcq(s_mqdc32 * s);
void mqdc32_stopAcq(s_mqdc32 * s);

void mqdc32_resetFifo(s_mqdc32 * s);
void mqdc32_resetTimestamp(s_mqdc32 * s);

void mqdc32_initMCST(s_mqdc32 * s);

void mqdc32_setMcstSignature(s_mqdc32 * s, unsigned long Signature);
uint16_t mqdc32_getMcstSignature(s_mqdc32 * s);

void mqdc32_setMcstMaster(s_mqdc32 * s);
bool_t mqdc32_isMcstMaster(s_mqdc32 * s);

void mqdc32_setCbltSignature(s_mqdc32 * s, unsigned long Signature);
uint16_t mqdc32_getCbltSignature(s_mqdc32 * s);

void mqdc32_setFirstInCbltChain(s_mqdc32 * s);
bool_t mqdc32_isFirstInCbltChain(s_mqdc32 * s);

void mqdc32_setLastInCbltChain(s_mqdc32 * s);
bool_t mqdc32_isLastInCbltChain(s_mqdc32 * s);
void mqdc32_setMiddleOfCbltChain(s_mqdc32 * s);
bool_t mqdc32_isMiddleOfCbltChain(s_mqdc32 * s);

void mqdc32_setMcstEnable(s_mqdc32 * s);
void mqdc32_setMcstDisable(s_mqdc32 * s);
bool_t mqdc32_mcstIsEnabled(s_mqdc32 * s);
void mqdc32_setCbltEnable(s_mqdc32 * s);
void mqdc32_setCbltDisable(s_mqdc32 * s);
bool_t mqdc32_cbltIsEnabled(s_mqdc32 * s);

void mqdc32_setMcstCblt_db(s_mqdc32 * s);

void mqdc32_startAcq_mcst(s_mqdc32 * s);
void mqdc32_stopAcq_msct(s_mqdc32 * s);
void mqdc32_resetFifo_mcst(s_mqdc32 * s);
void mqdc32_resetTimestamp_mcst(s_mqdc32 * s);
void mqdc_resetReadout_mcst(s_mqdc32 * s);

uint32_t * mqdc32_repairRawData(s_mqdc32 * s, uint32_t * pointer, uint32_t * dataStart);
#endif
